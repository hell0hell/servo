/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MtPRM.c : Motion Parameter Source File														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	Histroy:																						*/
/*		Rev2.01 2007.12.14	加速度指令追加															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	<V744>	2010.04.16	K.Ando																		*/
/*		<< 移動平均時間がパラメータ値通りに動作しない不具合修正 >>									*/
/*		・不具合No：A0329																			*/
/*		・現象：256.0ms - 384.0msの範囲にて256.0msで動作してしまう									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "ScanN.h"
//#include "Basedef.h"
//#include "ServoIf.h"
//#include "Motion.h"
//#include "MlibSvc.h"

/****************************************************************************************************/
/*																									*/
/*  Motion Parameter Definitions 																	*/
/*																									*/
/****************************************************************************************************/
#if 0
#define MAX_TRQ24			0x1000000	/* MAXTRQ */
#define MAX_SPD24			0x1000000	/* MAX速度 */
#define DISTANCE_MAX		0x7FFFFFFF	/* 移動距離MAX */
#define ACF_MAX_SZ			512			/* 移動平均フィルタバッファ数 */
#define FOLLOWUP_MAX_REF	32767		/* FollowUp時最大移動量[ref/cyc] */
#define MG_VREMBIT_MAX		24			/* MotionGenerator余り速度演算MAXビット数 */
#define MG_SPDDIV_RANGE		4			/* MotionGenerator少数点位置自動設定ビット範囲 */
#endif


/****************************************************************************************************/
/*																									*/
/*  Motion Parameter Function Prototypes															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Grobal Functions																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG MPrmCalcInit(MTPRM* prm, MTPRM_INI* iniPrm);

LONG MPrmSetAccRate1(MTPRM* prm, MG_PRM* mgPrm, LONG rate);
LONG MPrmSetAccRate2(MTPRM* prm, MG_PRM* mgPrm, LONG rate);
LONG MPrmSetAccAltSpd(MTPRM* prm, MG_PRM* mgPrm, LONG rate);
LONG MPrmSetDecRate1(MTPRM* prm, MG_PRM* mgPrm, LONG rate);
LONG MPrmSetDecRate2(MTPRM* prm, MG_PRM* mgPrm, LONG rate);
LONG MPrmSetDecAltSpd(MTPRM* prm, MG_PRM* mgPrm, LONG rate);
LONG MPrmSetExpFilBias(MTPRM* prm, ACF_PRM* acfPrm, LONG bias);
LONG MPrmSetExpFilTb(MTPRM* prm, ACF_PRM* acfPrm, USHORT exptb);
LONG MPrmSetMaFilTb(MTPRM* prm, ACF_PRM* acfPrm, USHORT matb);

LONG MPrmSetStopDecRate1(MTPRM* prm, LONG rate);
LONG MPrmSetStopDecRate2(MTPRM* prm, LONG rate);
LONG MPrmSetStopDecAltSpd(MTPRM* prm, LONG rate);
LONG MPrmSetSegNum(MTPRM* prm, LONG cnt);

LONG MPrmSetSoftLimitFlag(MTPRM* prm, USHORT data);
LONG MPrmSetSoftLimitP(MTPRM* prm, LONG data);
LONG MPrmSetSoftLimitN(MTPRM* prm, LONG data);
LONG MPrmSetAbsOffset(MTPRM* prm, LONG data);
LONG MPrmSetLtRangeP(MTPRM* prm, LONG data);
LONG MPrmSetLtRangeN(MTPRM* prm, LONG data);
LONG MPrmSetZpointWidth(MTPRM* prm, LONG data);
LONG MPrmSetPsetWidth(MTPRM* prm, LONG data);
LONG MPrmSetNearWidth(MTPRM* prm, LONG data);

LONG MPrmSetZretDir(MTPRM* prm, BOOL data);
LONG MPrmSetZretSpd1(MTPRM* prm, LONG data);
LONG MPrmSetZretSpd2(MTPRM* prm, LONG data);
LONG MPrmSetZretDist(MTPRM* prm, LONG data);
LONG MPrmSetExPosingDist(MTPRM* prm, LONG data);
LONG MPrmSetRampTimer(MTPRM* prm, LONG data);

LONG MPrmSetInSigMap(MTPRM* prm, LONG data);
LONG MPrmSetLatchSeqNum(MTPRM* prm, LONG data);
LONG MPrmSetLatchSeqCnt(MTPRM* prm, LONG data);
LONG MPrmSetLatchSeqSgnl(MTPRM* prm, LONG data);
VOID MPrmCalcAccTimeRate(MTPRM* prm, LONG Tspd, LONG Tacc, LONG Tdec);

LONG MPrmSetTlimTimeOut1(MTPRM* prm, LONG data);
LONG MPrmSetTlimTimeOut2(MTPRM* prm, LONG data);
/*--------------------------------------------------------------------------------------------------*/
/* Private sub functions 																			*/
/*--------------------------------------------------------------------------------------------------*/
VOID mPrmCalcPow(LONG data, SHORT exp, LONG* num, LONG* deno);

#if 0
const OPTION_MAP OptMapTbl = {
	3,		/* 加減速フィルタ選択ビット位置 */
	0x3,	/* 加減速フィルタ選択ビットマスク */
	8,		/* ゲイン選択ビット位置 */
	0x3,	/* ゲイン選択ビットマスク */
	12,		/* P_PI選択ビット位置 */
	0x1,	/* P_PI選択ビットマスク */
	13,		/* 位置積分クリアビット位置 */
	0x1,	/* 位置積分ビットマスク */
	14,		/* P_CLビット位置 */
	0x1,	/* P_CLビットマスク */
	15,		/* N_CLビット位置 */
	0x1,	/* N_CLビットマスク */
	31,		/* OutSignalビット位置 */
	0x7,	/* OutSignalビットマスク */
	31,		/* BankSelビット位置 */
	0xF,	/* BankSelビットマスク */
	31,		/* LtDisableビット位置 */
	0x1,	/* LtDisableビットマスク */
};
#endif


/****************************************************************************************************/
/* モーションパラメータ初期計算処理																	*/
/*																									*/
/*																									*/
/* 概要：現在設定されている、モーション周期、分解能設定から以下を設定								*/
/*																									*/
/*	1)単位変換定数の計算																			*/
/*	PosingRateKx	;[10000ref/s^s]->[***.*ref/cyc^cyc]速度変換定数									*/
/*	PosingSpdKx		;[ref/s]->[***.*ref/cyc]速度変換定数											*/
/*	PosingSpdInvKx	;[***.*ref/cyc]->[ref/s]速度変換定数											*/
/*	PosingSpdMax	;POSING最大速度[ref/s]															*/
/*	fspdRefKx		;[ref/通信周期]->[ref/s]速度変換定数											*/
/*	refToOsKx		;[指令単位/s]->[24bit/OS]速度変換定数											*/
/*	osToRefKx		;[24bit/OS]->[指令単位/s]速度変換定数											*/
/*	maxToOsKx		;[30bit/MAX]->[24bit/OS]速度変換定数											*/
/*	osToMaxKx		;[24bit/OS]->[30bit/MAX]速度変換定数											*/
/*	trqKx			;[24bit/MAX]->[%]トルク変換定数													*/
/*																									*/
/*	2)モーション設定																				*/
/*	MotionCycle		;モーション制御周期[0.25us]														*/
/*	MG_BASE_PRM		;MG用基本パラメータ全て															*/
/*																									*/
/*	3)サーボ情報																					*/
/*	FullcCtrl		;ﾌﾙｸﾛｰｽﾞﾄﾞ制御																	*/
/*	Linear			;リニア使用																		*/
/*	absPG			;ｱﾌﾞｿPG使用																		*/
/*	ot_zstop;		;OT時ｾﾞﾛｸﾗﾝﾌﾟ																	*/
/*	trefMode		;TRef Option																	*/
/*	OptMap			;OPTION指令割付																	*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
LONG MPrmCalcInit(MTPRM* prm, MTPRM_INI* iniPrm)
{
	LONG kx, sx;
	LONG ky, sy;
	LONG maxSpd_num, maxSpd_deno;
	LONG ovSpd_num, ovSpd_deno;
	LONG pitch_num, pitch_deno;

/*--------------------------------------------------------------------------------------------------*/
/* モーションサイクルセット 																		*/
/*--------------------------------------------------------------------------------------------------*/
	prm->BasePrm.MotionCycle = iniPrm->Cycle;

/*--------------------------------------------------------------------------------------------------*/
/* 速度の単位変換定数の計算[ref/通信周期]->[ref/s] 													*/
/*--------------------------------------------------------------------------------------------------*/
	sx = 0;
	prm->BasePrm.fspdRefKx = MlibPcalKxgain(1, 4000000, iniPrm->Cycle, &sx, 24);	

/*--------------------------------------------------------------------------------------------------*/
/* トルクの単位変換定数の計算[24bit/MAX]->[%] */
/*--------------------------------------------------------------------------------------------------*/
	sx = 0;
	prm->BasePrm.trqKx = MlibPcalKxgain(1, iniPrm->MaxTrq, MAX_TRQ24, &sx, 24);	
/*--------------------------------------------------------------------------------------------------*/
/* トルクの単位変換定数の計算[%]->[24bit/MAX] */
/*--------------------------------------------------------------------------------------------------*/
	sx = 0;
	prm->BasePrm.invTrqKx = MlibPcalKxgain(1, MAX_TRQ24, iniPrm->MaxTrq, &sx, 24);	

	mPrmCalcPow(1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno);
	mPrmCalcPow(1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno);

/*--------------------------------------------------------------------------------------------------*/
/* 速度の単位変換定数の計算[30bit/MAX]->[24bit/OS] 													*/
/* buf = spd * maxSpd/(ovSpd*64) 																	*/
/* buf = spd * maxSpd * (maxSpd_num/maxSpd_deno) / (ovSpd*(ovSpd_num/ovSpd_deno)*64) 				*/
/*--------------------------------------------------------------------------------------------------*/
	sx = sy = 0;
	kx = MlibPcalKxmulx(iniPrm->MaxSpd, maxSpd_num, ovSpd_deno, &sx);
	ky = MlibPcalKxmulx(iniPrm->OverSpd*64, ovSpd_num, maxSpd_deno, &sy);
	prm->BasePrm.maxToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/*--------------------------------------------------------------------------------------------------*/
/* 速度の単位変換定数の計算[24bit/OS]->[30bit/MAX] */
/*--------------------------------------------------------------------------------------------------*/
	prm->BasePrm.osToMaxKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);

/*--------------------------------------------------------------------------------------------------*/
/* 速度の単位変換定数の計算[指令単位/s]->[24bit/OS] */
/* 速度の単位変換定数の計算[24bit/OS]->[指令単位/s] */
/*--------------------------------------------------------------------------------------------------*/
	sx = sy = 0;
	if(iniPrm->Linear)
	{
		mPrmCalcPow(1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno);
		kx = MlibPcalKxmulx(iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx);
		kx = MlibPcalKxmulx(kx, ovSpd_deno, pitch_num, &sx);
		ky = MlibPcalKxmulx(iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy);  
		ky = MlibPcalKxmulx(ky, pitch_deno, ovSpd_num, &sy);                                     
		prm->BasePrm.refToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
		prm->BasePrm.osToRefKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);
	}
	else
	{
		kx = MlibPcalKxmulx(60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx);
		ky = MlibPcalKxmulx(iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy);
		prm->BasePrm.refToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
		prm->BasePrm.osToRefKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);
	}

/*--------------------------------------------------------------------------------------------------*/
/* 速度の単位変換定数の計算[**.*** ref/制御周期]→[ref/s] 											*/
/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) 											*/
/*--------------------------------------------------------------------------------------------------*/
	sx = sy = 0;
	kx = MlibPcalKxmulx(1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx((1 << iniPrm->SpdDiv), iniPrm->Cycle, 1, &sy);
	prm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	
	prm->BasePrm.PosingSpdMax = MlibMulgain32(0x40000000, prm->BasePrm.PosingSpdInvKx);

/*--------------------------------------------------------------------------------------------------*/
/* INDEXER用: POSING分解能算出 																		*/
/*		 TODO)パラメータからの速度指令ゲイン切替対応												*/
/*		 TODO)内部位置指令電子ギア対応																*/
/*       *)INDEXERの最高速度は、99999999 * TspdGain * EgearX.b / EgearX.a ref/min                   */
/*       *)変形して、99999999 * TspdGain * 1/60 * Cycle[0.25us] / 4000000 * EgearX.b / EgearX.a     */
/*       *)簡略化して、25 * TspdGain * Cycle[0.25us] / 60 * EgearX.b / EgearX.a                     */
/*       *)ScanN周期=1000us、TspdGain=1000の場合、 99999999ref/min = 196E6AH ref/cycle              */
/*       *)40000000H を超えないような196E6AH << (sx)を設定する。                                    */
/*--------------------------------------------------------------------------------------------------*/
#if 1   /* TspdGain対応 */
	maxSpd_num  = (25 * iniPrm->Cycle * iniPrm->TspdGain * 1/*Iprm.EgearX.b*/) / (60 * 1/*Iprm.EgearX.a*/);
	if ( maxSpd_num == 0 ){ return -1; }                                    /* parameter error */
#else
	maxSpd_num  = (99999999 / 240000) * iniPrm->Cycle ;
#endif
	for ( sx=0 ; (maxSpd_num << sx) <= 0x40000000; sx ++ ) { ; }
	maxSpd_deno = (! (iniPrm->SpdDiv)) ? (sx -1) : (iniPrm->SpdDiv -1);		/* 小数点ビット位置 */
#if 0
/*--------------------------------------------------------------------------------------------------*/
/* M2用:POSING分解能算出 --OverSpeedレベルを小数点位置とする 										*/
/*--------------------------------------------------------------------------------------------------*/
	ovSpd_num = MlibMulgain32(MAX_SPD24, prm->BasePrm.osToRefKx);	/* Over Speed [ref/s] */
	for(sx = MG_SPDDIV_RANGE; sx > 0; sx--)
	{
		maxSpd_num = prm->BasePrm.PosingSpdMax >> sx;
		if(ovSpd_num < maxSpd_num)
			break;
	}
	maxSpd_deno = iniPrm->SpdDiv + sx;	/* 小数点ビット位置 */
#endif


/*--------------------------------------------------------------------------------------------------*/
/* 加減速定数の計算[10000 x ref/s^2]→[**.*** ref/制御周期^2] 										*/
/* buf = rate * 2^vel_shift * 10000 / (4000000[.25us]/cyc[.25us])^2 								*/
/* buf = rate * 2^vel_shift * cyc[.25us]^2 / (40000)^2 												*/
/*--------------------------------------------------------------------------------------------------*/
	sx = 0;
	kx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 40000, &sx, 0);
	prm->BasePrm.PosingRateKx = MlibPcalKxgain(kx, iniPrm->Cycle, 40000, &sx, 24);

/*--------------------------------------------------------------------------------------------------*/
/* INDEXER用: 加減速定数の計算[1000ref/min/msec]→[**.*** ref/制御周期^2]                         	*/
/*                                    1      Cycle[.25us]       Cycle[.25us]                        */
/* buf = rate * 2^vel_shift * 1000 * ---- * ---------------- * ----------------                     */
/*                                    60     4000000[.25us]     4000[0.25us]                        */
/*                             Cycle[.25us]     Cycle[.25us]                                        */
/*     = rate * 2^vel_shift * -------------- * ---------------                                      */
/*                              240000            4000                                              */
/*--------------------------------------------------------------------------------------------------*/
/*		 TODO)パラメータからの速度指令ゲイン切替対応版												*/
/*		 TODO)内部位置指令電子ギア対応																*/
/*                                         1      Cycle[.25us]       Cycle[.25us]      EgearX.b     */
/* buf = rate * 2^vel_shift * TspdGain * ---- * ---------------- * ---------------- * ----------    */
/*                                        60     4000000[.25us]     4000[0.25us]       EgearX.a     */
/*--------------------------------------------------------------------------------------------------*/
#if 1   /* TspdGain対応 */
	sx = 0;
	kx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->TspdGain, 60, &sx, 0);
	kx = MlibPcalKxgain(kx, iniPrm->Cycle, 4000000, &sx, 0);
	kx = MlibPcalKxgain(kx, iniPrm->Cycle, 4000, &sx, 0);
	prm->BasePrm.PosingRateKxIx = MlibPcalKxgain(kx, 1 /* Iprm.EgearX.b */, 1 /* Iprm.EgearX.a */, &sx, 24);
	//prm->BasePrm.PosingRateKxIx = MlibPcalKxgain(kx, iniPrm->Cycle, 4000, &sx, 24);
#else
	sx = 0;
	kx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 240000, &sx, 0);
	prm->BasePrm.PosingRateKxIx = MlibPcalKxgain(kx, iniPrm->Cycle, 4000, &sx, 24);
#endif

/*--------------------------------------------------------------------------------------------------*/
/* 速度単位変換定数の計算[ref/s]→[**.*** ref/制御周期] 											*/
/* buf = spd * 2^vel_shift / (4000000[.25us]/cyc[.25us]) 											*/
/* buf = spd * 2^vel_shift * cyc[.25us] / 4000000[.25us] 											*/
/*--------------------------------------------------------------------------------------------------*/
	sx = 0;
	prm->BasePrm.PosingSpdKx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 4000000, &sx, 24);
/*--------------------------------------------------------------------------------------------------*/
/* TODO: INDEXER速度単位変換定数の計算[1000ref/min]→[**.*** ref/制御周期]                          */
/*                                    1     Cycle[.25us]                                            */
/* buf = spd * 2^vel_shift * 1000 * ---- * ----------------                                         */
/*                                   60     4000000[.25us]                                          */
/*                            Cycle[.25us]                                                          */
/*     = spd * 2^vel_shift * ---------------                                                        */
/*                            240000[.25us]                                                         */
/*		 TODO)パラメータからの速度指令ゲイン切替対応												*/
/*		 TODO)内部位置指令電子ギア対応																*/
/*                                        1     Cycle[.25us]       EgearX.b                         */
/* buf = spd * 2^vel_shift * TspdGain * ---- * ---------------- * --------------                    */
/*                                       60     4000000[.25us]     EgearX.a                         */
/*--------------------------------------------------------------------------------------------------*/
#if 1   /* TspdGain対応 */
	sx = 0;
	kx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->TspdGain, 60, &sx, 0);
	kx = MlibPcalKxgain( kx, iniPrm->Cycle, 4000000, &sx, 0);
	prm->BasePrm.PosingSpdKxIx = MlibPcalKxgain( kx, 1 /* Iprm.EgearX.b */, 1 /* Iprm.EgearX.a */, &sx, 24);
	//prm->BasePrm.PosingSpdKxIx = MlibPcalKxgain( kx, iniPrm->Cycle, 4000000, &sx, 24);
#else
	sx = 0;
	prm->BasePrm.PosingSpdKxIx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 240000, &sx, 24);
#endif

/*--------------------------------------------------------------------------------------------------*/
/* TODO: INDEXER速度単位変換定数の計算[TspdGain*ref/min]→[rpm]                                     */
/*                      EgearX.b      Egear.b(Pn20E)        1                                       */
/*      kx = TspdGain *----------- * ---------------- * --------                                    */
/*                      EgearX.a      Egear.a(Pn210)     PgPulse                                    */
/*--------------------------------------------------------------------------------------------------*/
	if( (iniPrm->FullcCtrl) || (iniPrm->Linear) )
	{
		return -1;      /* TODO: リニア、フルク対応 */
	}
	else
	{
		sx = 0;
		kx = MlibPcalKxgain(iniPrm->TspdGain, iniPrm->EGearB, iniPrm->EGearA, &sx, 0);
		kx = MlibPcalKxgain(kx, 1 /* Iprm.EgearX.b */, 1 /* Iprm.EgearX.a */, &sx, 0);
		prm->BasePrm.RefToRpmKxIx = MlibPcalKxgain(kx, 1, iniPrm->PgPuls, &sx, 24);
	}

/*--------------------------------------------------------------------------------------------------*/
/* 速度の単位変換定数の計算[**.*** ref/制御周期]→[ref/s] 											*/
/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) 											*/
/*--------------------------------------------------------------------------------------------------*/
	sx = sy = 0;
	kx = MlibPcalKxmulx(1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, 1, &sy);
	prm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
	prm->BasePrm.mgSpdMax = MlibMulgain32(0x40000000, prm->BasePrm.PosingSpdInvKx);

/*--------------------------------------------------------------------------------------------------*/
/* MG_BASE_PRMセット 																				*/
/*--------------------------------------------------------------------------------------------------*/
	prm->BasePrm.vel_sft_exp = (UCHAR)iniPrm->SpdDiv;
	prm->BasePrm.vel_shift = (UCHAR)maxSpd_deno;
	prm->BasePrm.vel_shift_s = (UCHAR)(MG_VREMBIT_MAX - maxSpd_deno);
	prm->BasePrm.vel_max = 1 << (31 - maxSpd_deno);
	prm->BasePrm.seg_num = iniPrm->SegNum;
	sx = kx= 0;
	kx = MlibPcalKxgain(prm->BasePrm.PosingSpdMax, iniPrm->Cycle, 4000000, &sx, 24);
	prm->BasePrm.seg_max_ref = MlibMulgain32(iniPrm->SegNum, kx);

	prm->BasePrm.FullcCtrl = iniPrm->FullcCtrl;	/* ﾌﾙｸﾛｰｽﾞﾄﾞ制御 */
	prm->BasePrm.Linear = iniPrm->Linear;		/* リニア使用 */
	prm->BasePrm.ot_zstop = iniPrm->ot_zstop;

/*--------------------------------------------------------------------------------------------------*/
/* ACC_TIME用加速度単位変換定数の計算[(ref/s)/(0.1ms)]→[**.*** ref/制御周期^2] <Rev2.01>			*/
/*--------------------------------------------------------------------------------------------------*/
	sx = sy = 0;
	kx = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, iniPrm->Cycle, &sx);
	ky = MlibPcalKxmulx(40000, 40000, 1, &sy);
	prm->BasePrm.taccRateKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/*--------------------------------------------------------------------------------------------------*/
/* ACC_TIME用加速度単位変換定数の計算[(2^7*ref/s)/(0.1ms)]→[**.*** ref/制御周期^2] <Rev2.01>		*/
/*--------------------------------------------------------------------------------------------------*/
	sy = 0;
	ky = MlibPcalKxmulx(40000, 40000, (1<<7), &sy);
	prm->BasePrm.taccRate7Kx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/*--------------------------------------------------------------------------------------------------*/
/* TFF/TLIM/ACC_TIME使用選択 <Rev2.01>*/
/*--------------------------------------------------------------------------------------------------*/
	prm->BasePrm.trefMode = (UCHAR)iniPrm->trefMode;
	prm->BasePrm.TrefModePos = ((iniPrm->OptEnable&0x00F0) != 0) ? iniPrm->trefMode : 0;
	prm->BasePrm.TLimModePos = ((iniPrm->OptEnable&0x0F00) == 0) ? prm->BasePrm.TrefModePos : 0;
	prm->BasePrm.SetCmdAccRate = ((iniPrm->OptEnable&0x0F00) != 0) ? &MPrmCalcAccTimeRate : NULL;

/*--------------------------------------------------------------------------------------------------*/
/* 位置情報関連PRMセット */
/*--------------------------------------------------------------------------------------------------*/
	prm->PsnPrm.absPG = iniPrm->absPG;		/* ｱﾌﾞｿPG使用 */
	prm->PsnPrm.TdlyIn = iniPrm->dlyTime;
	prm->PsnPrm.SegNum = iniPrm->SvCycle;

/*--------------------------------------------------------------------------------------------------*/
/* Option指令割付定義 */
/*--------------------------------------------------------------------------------------------------*/
#if 0
	prm->OptMap = OptMapTbl;	/* デフォルト値セット */
	if((iniPrm->OptEnable & 0x01) != 0)
	{
		if((iniPrm->OptMap[0]&0x00F0) == 0) prm->OptMap.accfilBitMsk = 0;
		if((iniPrm->OptMap[0]&0xF000) == 0)	prm->OptMap.gselBitMsk = 0;
		if((iniPrm->OptMap[1]&0x00F0) == 0)	prm->OptMap.vppiBitMsk = 0;
		if((iniPrm->OptMap[1]&0xF000) == 0)	prm->OptMap.ppiclrBitMsk = 0;
		if((iniPrm->OptMap[2]&0x00F0) == 0)	prm->OptMap.pclBitMsk = 0;
		if((iniPrm->OptMap[2]&0xF000) == 0)	prm->OptMap.nclBitMsk = 0;
		if((iniPrm->OptMap[3]&0x00F0) == 0)	prm->OptMap.bankBitMsk = 0;
		if((iniPrm->OptMap[3]&0xF000) == 0)	prm->OptMap.ltBitMsk = 0;
		if((iniPrm->OptMap[4]&0xF000) == 0)	prm->OptMap.outpBitMsk = 0;
		if(iniPrm->OutSigEnable == 0)		prm->OptMap.outpBitMsk = 0;

		prm->OptMap.accfilBit = (iniPrm->OptMap[0] & 0xF);
		prm->OptMap.gselBit = (iniPrm->OptMap[0] >> 8) & 0xF;
		prm->OptMap.vppiBit = (iniPrm->OptMap[1] & 0xF);
		prm->OptMap.ppiclrBit = (iniPrm->OptMap[1] >> 8) & 0xF;
		prm->OptMap.pclBit = (iniPrm->OptMap[2] & 0xF);
		prm->OptMap.nclBit = (iniPrm->OptMap[2] >> 8) & 0xF;
		prm->OptMap.bankBit = (iniPrm->OptMap[3] & 0xF);
		prm->OptMap.ltBit = (iniPrm->OptMap[3] >> 8) & 0xF;
		prm->OptMap.outpBit = (iniPrm->OptMap[4] >> 8) & 0xF;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
	return 0;
}

VOID mPrmCalcPow(LONG data, SHORT exp, LONG* num, LONG* deno)
{
	if(exp > 0)
	{
		while((exp--) > 0)
			data *= 10;
		*num = data;
		*deno = 1;
	}
	else if(exp < 0)
	{
		exp = MlibABS(exp);
		while((exp--) > 0)
			data *= 10;
		*num = 1;
		*deno = data;
	}
	else
	{
		*num = data;
		*deno = 1;
	}
}
#pragma arm section code

/****************************************************************************
 加速度1設定(MG用)
****************************************************************************/
LONG MPrmSetAccRate1(MTPRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	mgPrm->accrate1 = data;
	return 0;
}

/****************************************************************************
 加速度2設定(MG用)
****************************************************************************/
LONG MPrmSetAccRate2(MTPRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	mgPrm->accrate2 = data;
	return 0;
}

/****************************************************************************
 加速度切り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetAccAltSpd(MTPRM* prm, MG_PRM* mgPrm, LONG data)
{
	mgPrm->altspd_a = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}

/****************************************************************************
 減速度1設定(MG用)
****************************************************************************/
LONG MPrmSetDecRate1(MTPRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	mgPrm->decrate1 = data;
	return 0;
}

/****************************************************************************
 減速度2設定(MG用)
****************************************************************************/
LONG MPrmSetDecRate2(MTPRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	mgPrm->decrate2 = data;
	return 0;
}

/****************************************************************************
 減速度り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetDecAltSpd(MTPRM* prm, MG_PRM* mgPrm, LONG data)
{
	mgPrm->altspd_d = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}

/****************************************************************************
 INTERPOLATE用セグメント分割回数設定(MG用)
****************************************************************************/
LONG MPrmSetSegNum(MTPRM* prm, LONG cnt)
{
	LONG sx,kx;

	prm->BasePrm.seg_num = cnt;
	sx = kx= 0;
	kx = MlibPcalKxgain(prm->BasePrm.PosingSpdMax, prm->BasePrm.MotionCycle, 4000000, &sx, 24);
	prm->BasePrm.seg_max_ref = MlibMulgain32(cnt, kx);
//	prm->BasePrm.seg_max_ref = (prm->BasePrm.seg_num * prm->BasePrm.vel_max);
	return 0;
}



/****************************************************************************
 非常停止用減速度1設定(MG用)
****************************************************************************/
LONG MPrmSetStopDecRate1(MTPRM* prm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	prm->BasePrm.StopPrm.decrate1 = data;
	return 0;
}

/****************************************************************************
 非常停止用減速度2設定(MG用)
****************************************************************************/
LONG MPrmSetStopDecRate2(MTPRM* prm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	prm->BasePrm.StopPrm.decrate2 = data;
	return 0;
}

/****************************************************************************
 非常停止用減速度り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetStopDecAltSpd(MTPRM* prm, LONG data)
{
	prm->BasePrm.StopPrm.altspd_d = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}






/****************************************************************************
 指数関数加減速バイアス設定(AccFilter)
****************************************************************************/
LONG MPrmSetExpFilBias(MTPRM* prm, ACF_PRM* acfPrm, LONG bias)
{
	LONG kx, sx=0;

	/* buf = spd / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * cyc[.25us] / 4000000[.25us] */
	kx = MlibPcalKxgain(1, prm->BasePrm.MotionCycle, 4000000, &sx, 24);
	acfPrm->expbias = MlibMulgain32(bias, kx);
	return 0;
}

/****************************************************************************
 指数関数加減速時定数設定(AccFilter)
 概要：指数関数フィルタの時定数ゲインを計算し、パラメータに設定
      Kt = t / ( t + exptb)
           Kt  :時定数ゲイン
           t   :サンプリング周期[0.25us]
           exptb :時定数[0.25us]
 引数：
	prm		- Motion Paramter構造体への参照先
	exptb	- 時定数 [0.1ms]
****************************************************************************/
LONG MPrmSetExpFilTb(MTPRM* prm, ACF_PRM* acfPrm, USHORT exptb)
{
	LONG buf;

	/* exptb = 2^vel_shift * (cyc /(exptb*4 + cyc)) */
	buf = ((1 << prm->BasePrm.vel_sft_exp) * prm->BasePrm.MotionCycle);	/* 2^vel_shift * cyc */
	buf = buf / ((exptb*400) + prm->BasePrm.MotionCycle);

	acfPrm->exptb = buf;	/* パラメータ保存 */

	return 0;
}

/****************************************************************************
 移動平均時定数設定(AccFilter)
 概要：移動平均フィルタの時定数計算し、パラメータに設定
 引数：
	prm		- Motion Paramter構造体への参照先
	matb	- 時定数 [0.1ms]
****************************************************************************/
LONG MPrmSetMaFilTb(MTPRM* prm, ACF_PRM* acfPrm, USHORT matb)
{
	LONG	tb, i;
	LONG	nTb, iTb;
	MATB_PRM maPrm;

	tb = matb * 400;	/* [0.1ms]->[0.25us] */
	if(tb >= prm->BasePrm.MotionCycle)
	{/* 時定数が制御サイクル以上の場合 */
		nTb = (tb / prm->BasePrm.MotionCycle);	/* 移動平均回数計算 */
		iTb = (USHORT)(nTb / ACF_MAX_SZ);
		if(iTb > 0)
		{
			for(i = 1; i < 5; i++)
			{
/* <V744>		if((1<<i) >= iTb) break;	*/
				if((1<<i) > iTb) break;		/* <V744> */
			}
			tb = nTb / (1<<i);
			maPrm.wTb[0] = (USHORT)i;
			maPrm.wTb[1] = (USHORT)((tb > ACF_MAX_SZ) ? ACF_MAX_SZ: tb);
		}
		else
		{
			maPrm.wTb[0] = 0;
			maPrm.wTb[1] = (USHORT)((nTb > ACF_MAX_SZ) ? ACF_MAX_SZ: nTb);
		}
		acfPrm->mafil.lTb = maPrm.lTb;
	}
	else
	{/* 時定数が制御サイクル未満の場合 */
		acfPrm->mafil.lTb = 0;
	}

	return 0;
}


/****************************************************************************
 ソフトリミットフラグ設定(Postion Manager)
 引数：data
    Bit0=0:正側ソフトリミット有効, =1:無効
    Bit1=0:負側ソフトリミット有効, =1:無効
    Bit8=0:指令ソフトリミット無効, =1:有効
****************************************************************************/
LONG MPrmSetSoftLimitFlag(MTPRM* prm, USHORT data)
{
	prm->PsnPrm.sotEnableP = ((data & 0x1) != 0) ? FALSE: TRUE;
	prm->PsnPrm.sotEnableN = ((data & 0x2) != 0) ? FALSE: TRUE;
	prm->PsnPrm.tposLimitEnable = ((data & 0x100) != 0) ? TRUE: FALSE;
	return 0;
}

/****************************************************************************
 正側ソフトリミット設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetSoftLimitP(MTPRM* prm, LONG data)
{
	prm->PsnPrm.softLimit_P = data;
	return 0;
}

/****************************************************************************
 負側ソフトリミット設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetSoftLimitN(MTPRM* prm, LONG data)
{
	prm->PsnPrm.softLimit_N = data;
	return 0;
}

/****************************************************************************
 原点位置オフセット設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetAbsOffset(MTPRM* prm, LONG data)
{
	prm->PsnPrm.absOfsset = data;
	return 0;
}

/****************************************************************************
 ラッチ領域設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetLtRangeP(MTPRM* prm, LONG data)
{
	prm->PsnPrm.ltRangeP = data;
	return 0;
}

/****************************************************************************
 ラッチ領域設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetLtRangeN(MTPRM* prm, LONG data)
{
	prm->PsnPrm.ltRangeN = data;
	return 0;
}

/****************************************************************************
 原点範囲設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetZpointWidth(MTPRM* prm, LONG data)
{
	prm->PsnPrm.zPointWidth = data;
	return 0;
}

/****************************************************************************
 位置決め幅設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetPsetWidth(MTPRM* prm, LONG data)
{
	prm->PsnPrm.psetWidth = data;
	return 0;
}

/****************************************************************************
 位置決め近傍幅設定(Postion Manager)
 引数：data[ref]
****************************************************************************/
LONG MPrmSetNearWidth(MTPRM* prm, LONG data)
{
	prm->PsnPrm.nearWidth = data;
	return 0;
}



/****************************************************************************
 原点復帰方向の設定(Motion Control)
 引数：data=TRUE：負方向、FALSE:正方向
****************************************************************************/
LONG MPrmSetZretDir(MTPRM* prm, BOOL data)
{
	if(data)
		prm->MtSeqPrm.zret_dir = -DISTANCE_MAX;
	else
		prm->MtSeqPrm.zret_dir = DISTANCE_MAX;

	return 0;
}

/****************************************************************************
 原点復帰アプローチ速度1の設定(Motion Control)
 引数：data [ref/s -> ref/cycle]
****************************************************************************/
LONG MPrmSetZretSpd1(MTPRM* prm, LONG data)
{
	prm->MtSeqPrm.zretsp1 = data;
	return 0;
}

/****************************************************************************
 原点復帰アプローチ速度2の設定(Motion Control)
 引数：data [ref/s -> ref/cycle]
****************************************************************************/
LONG MPrmSetZretSpd2(MTPRM* prm, LONG data)
{
	prm->MtSeqPrm.zretsp2 = data;
	return 0;
}

/****************************************************************************
 原点復帰最終走行距離の設定(Motion Control)
 引数：data [ref]
****************************************************************************/
LONG MPrmSetZretDist(MTPRM* prm, LONG data)
{
	prm->MtSeqPrm.zret_dist = data;
	return 0;
}

/****************************************************************************
  set homing distination       
  data : Creep speed
****************************************************************************/
LONG MPrmSetZretOSLvl(MTPRM* prm, LONG data)
{
LONG spd;
	spd = MlibMulgain( data, prm->BasePrm.RefToRpmKxIx );        /* ref/min -> rpm */
	spd = MlibMAX( spd * 2, 50 );                                /* minimum = 50rpm */
	//prm->MtSeqPrm.zretOSLevel = KpiSpeedLevelCal( spd * 10, 0 ); /* set OS level */
	//prm->MtSeqPrm.zretOSLevel = BprmSpeedLevelCal( spd * 10, 0 ); /* set OS level */
	//TODO:
	return 0;
}

/****************************************************************************
 EX_POSING最終走行距離の設定(Motion Control)
 引数：data [ref]
****************************************************************************/
LONG MPrmSetExPosingDist(MTPRM* prm, LONG data)
{
	prm->MtSeqPrm.ex_posing_dist = data;
	return 0;
}

/****************************************************************************
 サーボOFF減速停止用タイマの設定(Motion Control)
 引数：data [ms] (0-65535)
****************************************************************************/
LONG MPrmSetRampTimer(MTPRM* prm, LONG data)
{
	LONG sx, kx;

	sx = 0;
	kx = MlibPcalKxgain(data, 4000, prm->BasePrm.MotionCycle, &sx, 24);
	prm->MtSeqPrm.rampTimeOut = MlibMulgain32(1, kx);
//	prm->MtSeqPrm.rampTimeOut = (data*4000)/prm->BasePrm.MotionCycle;
	return 0;
}

/****************************************************************************
 入力信号モニタ割付設定(Motion API)
 引数：data  bit00-03 IO:0割付(n=0:割付なし、n=1-8:SI[n-1]信号)
             bit04-07 IO:1割付
                ;        ;
             bit28-31 IO:7割付
****************************************************************************/
#if 0
LONG MPrmSetInSigMap(MTPRM* prm, LONG data)
{
	USHORT msk = 0;
	UCHAR  map, cnt;

	for(cnt = 0; cnt < 8; cnt++)
	{
		map = (UCHAR)((data >> (cnt*4)) & 0xF);
		if(map == 0)
		{
			msk |= (0x1 << cnt);
			map = 1;
		}
		prm->InSigMap.bitNo[cnt] = map - 1;
	}
	prm->InSigMap.bitsMsk = ~msk;

	return 0;
}
#endif


/****************************************************************************
 ラッチシーケンス数設定(Motion Control)
 引数：data 
****************************************************************************/
#if 0
LONG MPrmSetLatchSeqNum(MTPRM* prm, LONG data)
{
	prm->LtPrm.seqNum = (USHORT)data;
	return 0;
}
#endif

/****************************************************************************
 ラッチシーケンス回数設定(Motion Control)
 引数：data 
****************************************************************************/
#if 0
LONG MPrmSetLatchSeqCnt(MTPRM* prm, LONG data)
{
	prm->LtPrm.loopCnt = (USHORT)data;
	return 0;
}
#endif

/****************************************************************************
 ラッチシーケンス信号設定(Motion Control)
 引数：data 
****************************************************************************/
#if 0
LONG MPrmSetLatchSeqSgnl(MTPRM* prm, LONG data)
{
	prm->LtPrm.ltSgnl[0] = (UCHAR)(data&0xF);
	prm->LtPrm.ltSgnl[1] = (UCHAR)((data>>4)&0xF);
	prm->LtPrm.ltSgnl[2] = (UCHAR)((data>>8)&0xF);
	prm->LtPrm.ltSgnl[3] = (UCHAR)((data>>12)&0xF);

	prm->LtPrm.ltSgnl[4] = (UCHAR)((data>>16)&0xF);
	prm->LtPrm.ltSgnl[5] = (UCHAR)((data>>20)&0xF);
	prm->LtPrm.ltSgnl[6] = (UCHAR)((data>>24)&0xF);
	prm->LtPrm.ltSgnl[7] = (UCHAR)((data>>28)&0xF);
	return 0;
}
#endif

/****************************************************************************
 加減速指令用 加速度設定(Motion Control) <Rev2.01>
 引数：Tacc[0.1ms], Tspd[ref/s]
****************************************************************************/
VOID MPrmCalcAccTimeRate(MTPRM* prm, LONG Tspd, LONG Tacc, LONG Tdec)
{
	ULONG spd;
	LONG rate,accrate;

	spd = (ULONG)MlibABS(Tspd);
	if(Tacc == 0){Tacc = 1;}
	if(spd < 0x1000000)
	{
		rate = (spd << 7) / Tacc;
		rate = MlibMulgain32(rate, prm->BasePrm.taccRate7Kx);
	}
	else
	{
		rate = (spd + (Tacc>>1)) / Tacc;
		rate = MlibMulgain32(rate, prm->BasePrm.taccRateKx);
	}
	accrate = MlibLIMITUL(rate, 0x3FFFFFFF, 1);

	prm->CmdRate.accrate2 = accrate;
	prm->CmdRate.decrate2 = accrate;
}

/****************************************************************************
 押付原点復帰の押し付け原点復帰時の押し付け検出時間の設定(Motion Control)
 引数：data [ms]
****************************************************************************/
LONG MPrmSetTlimTimeOut1(MTPRM* prm, LONG data)
{
	prm->MtSeqPrm.tlimTimeCnt1 = KPI_SCANN_MS( data );
	return 0;
}

/****************************************************************************
 押付原点復帰の押し付け原点復帰時の原点復帰完了待ち時間の設定(Motion Control)
 引数：data [ms]
****************************************************************************/
LONG MPrmSetTlimTimeOut2(MTPRM* prm, LONG data)
{
	prm->MtSeqPrm.tlimTimeCnt2 = KPI_SCANN_MS( data );
	return 0;
}
