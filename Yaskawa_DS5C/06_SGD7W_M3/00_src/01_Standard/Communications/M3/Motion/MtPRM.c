/****************************************************************************
Description	: MECHATROLINK Motion Paramter
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"


/****************************************************************************
 Private functions
****************************************************************************/
/*  べき乗演算処理 */
static	VOID mPrmCalcPow( LONG data, LONG exp, LONG *num, LONG *deno );
/*  TFF/TLIM使用選択処理 */
static	VOID mPrmTrefMode( MT_PRM *mtPrm, MTPRM_INI *iniPrm );
/* 速度単位変換 定数作成 */
static	VOID pcal_SpdInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* トルク単位変換 定数作成 */
static	VOID pcal_TrqInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 加速度単位変換 定数作成 */
static	void pcal_AccInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, PRMDATA *Prm);
/* 位置単位変換 定数作成 */
static	VOID pcal_PosInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	同じ単位の指数計算のみ */
static	KSGAIN pcal_SpdOdrPowKx( PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	同じ単位の指数計算のみ */
static	KSGAIN pcal_SpdMonPowKx( PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	指令単位/sec --> RPM(mm/s) */
static	KSGAIN pcal_SpdMonRefToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	RPM(mm/s) --> 指令単位/sec */
static	KSGAIN pcal_SpdOdrRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	RPM(mm/s) --> 指令単位/sec */
static	KSGAIN pcal_SpdMonRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	指令単位/sec --> 定格速度に対する％ */
static	KSGAIN pcal_SpdMonRefToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	定格速度に対する％ --> 指令単位/sec */
static	KSGAIN pcal_SpdOdrRatToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	指令単位/sec --> 最大速度/40000000h */
static	KSGAIN pcal_SpdOdrRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	指令単位/sec --> 最大速度/40000000h */
static	KSGAIN pcal_SpdMonRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	最大速度/40000000h --> 指令単位/sec */
static	KSGAIN pcal_SpdOdrMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	RPM(mm/s) --> 指令単位/min */
static	KSGAIN pcal_SpdMonRpmToMinKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	指令単位/min --> 指令単位/sec */
static	KSGAIN pcal_SpdOdrMinToRefKx( PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	指令単位/sec --> 指令単位/min */
static	KSGAIN pcal_SpdMonRefToMinKx( PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	RPM --> 定格速度に対する％ */
static	KSGAIN pcal_SpdMonRpmToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	RPM --> 最大速度/40000000h	  */
static	KSGAIN pcal_SpdOdrRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	RPM --> 最大速度/40000000h	  */
static	KSGAIN pcal_SpdMonRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	指令単位/min --> 最大速度/40000000h */
static	KSGAIN pcal_SpdOdrMinToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（指令変換）	定格速度に対する％ --> 最大速度/40000000h */
static	KSGAIN pcal_SpdOdrRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択	指令単位/sec --> RPM	10 -3乗固定 */
static	KSGAIN pcal_SpdRefToRpmKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm );
/* 速度単位選択	RPM --> 指令単位/sec	10の-3乗固定 */
static	KSGAIN pcal_SpdRpmToRefKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm );
/* 速度単位選択（モニタ変換）	24bitOS → 指令単位/sec */
static	KSGAIN pcal_SpdMon24OsToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	24bitOS → 定格速度に対する％ */
static	KSGAIN pcal_SpdMon24OsToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	24bitOS → min-1(RPM), mm/s */
static	KSGAIN pcal_SpdMon24OsToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	24bitOS → 最大速度/40000000h */
static	KSGAIN pcal_SpdMon24OsToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	最大速度/40000000h → 指令単位/sec */
static	KSGAIN pcal_SpdMonMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	最大速度/40000000h → 定格速度に対する％ */
static	KSGAIN pcal_SpdMonMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 速度単位選択（モニタ変換）	最大速度/40000000h → min-1(RPM), mm/s */
static	KSGAIN pcal_SpdMonMaxToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* トルク単位選択	同じ単位の指数計算のみ */
static	KSGAIN pcal_TrqPowKx( LONG TrqExp, LONG inv, PRMDATA *Prm );
/* トルク単位選択	Nm(N) --> 最大ﾄﾙｸ/40000000h				  */
static	KSGAIN pcal_TrqNmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* トルク単位選択	最大ﾄﾙｸ/40000000h --> Nm(N)				  */
static	KSGAIN pcal_TrqMaxToNmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* トルク単位選択	定格ﾄﾙｸに対する％ --> 最大ﾄﾙｸ/40000000h	  */
static	KSGAIN pcal_TrqRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* トルク単位選択	最大ﾄﾙｸ/40000000h --> 定格ﾄﾙｸに対する％	  */
static	KSGAIN pcal_TrqMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* 加速度(指令単位/sec^2)  --> 加速度(**.*** 指令単位/cyc^2) */
static	KSGAIN pcal_AccS2TOCyc2Kx( LONG maxSpd_deno, LONG cycle, LONG inv, PRMDATA *Prm );
/* 加速度(指令単位/sec^2) --> 加速度([2^24/OS]/cycle) */
static	KSGAIN pcal_AccRefToOsKx( MTPRM_INI *iniPrm, PRMDATA *Prm );
/* 現状未対応 */
/* 加速時間(ms)  --> 加速度(**.*** 指令単位/cyc^2)  */
//static	LONG pcal_AccMsToCyc2Kx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, USHORT cycle, PRMDATA *pprm_ptr );
/* 現状未対応 */
/* 加速時間(ms) --> 加速度([2^24/OS]/cycle) */
//static	LONG pcal_AccMsToOsKx( MTPRM_INI *iniPrm );
/* 同じ単位の指数計算のみ */
static	KSGAIN pcal_PosPowKx( LONG inv, PRMDATA *Prm );

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

/* <S1C0>：Start */
const UCHAR SvCmdBitMaskTable[VENDOR_IOBIT_NUM] = {
	0x0F,	/* Pn856.0：BANKのビットマスク				 */
	0x01,	/* Pn856.1：SO1のビットマスク				 */
	0x01,	/* Pn856.2：SO2のビットマスク				 */
	0x01,	/* Pn856.3：SO3のビットマスク				 */
	/* ----------------------------------------------	 */
	0x01,	/* Pn857.0：SO4のビットマスク				 */
	0x01,	/* Pn857.1：SO5のビットマスク				 */
	0x01,	/* Pn857.2：J_SELのビットマスク				 *//* 将来用	*/
	0x01,	/* Pn857.3：Reserveのビットマスク			 */
	/* ----------------------------------------------	 */
	0x01,	/* Pn858.0：S_RATIOのビットマスク			 *//* 将来用	*/
	0x01,	/* Pn858.1：LT_REQ_EX1のビットマスク		 *//* 将来用	*/
	0x01,	/* Pn858.2：LT_REQ_EX2のビットマスク		 *//* 将来用	*/
	0x01,	/* Pn858.3：LT_REQ_EX3のビットマスク		 *//* 将来用	*/
	/* ----------------------------------------------	 */
	0x01,	/* Pn859.0：Reserveのビットマスク			 */
	0x01,	/* Pn859.1：Reserveのビットマスク			 */
	0x01,	/* Pn859.2：Reserveのビットマスク			 */
	0x01,	/* Pn859.3：Reserveのビットマスク			 */
};
LONG EnaOutBit;
/* <S1C0>：End */

/****************************************************************************
 モーションパラメータ初期計算処理
 概要：現在設定されている、モーション周期、分解能設定から以下を設定
	1)単位変換定数の計算
	PosingRateKx	;[10000ref/s^s]->[***.*ref/cyc^cyc]速度変換定数
	PosingSpdKx		;[ref/s]->[***.*ref/cyc]速度変換定数
	PosingSpdInvKx	;[***.*ref/cyc]->[ref/s]速度変換定数
	PosingSpdMax	;POSING最大速度[ref/s]
	fspdRefKx		;[ref/通信周期]->[ref/s]速度変換定数
	refToOsKx		;[指令単位/s]->[24bit/OS]速度変換定数
	osToRefKx		;[24bit/OS]->[指令単位/s]速度変換定数
	maxToOsKx		;[30bit/MAX]->[24bit/OS]速度変換定数
	osToMaxKx		;[24bit/OS]->[30bit/MAX]速度変換定数
	trqKx			;[24bit/MAX]->[%]トルク変換定数
	2)モーション設定
	MotionCycle		;モーション制御周期[0.25us]
	MG_BASE_PRM		;MG用基本パラメータ全て
	3)サーボ情報
	absPG			;ｱﾌﾞｿPG使用
	trefMode		;TRef Option
	OptMap			;OPTION指令割付
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	LONG sx, kx;
	LONG ky, sy;
	LONG maxSpd_num;
	LONG maxSpd_deno;
	LONG ovSpd_num;
	LONG ovSpd_deno;
	LONG pitch_num;
	LONG pitch_deno;

/* モーションサイクルセット */
	mtPrm->BasePrm.MotionCycle = iniPrm->Cycle;
//	iniPrm->SpdDiv = MG_DEFAULT_SPDDIV;/* <S06C> */

/* 速度の単位変換定数の計算[ref/通信周期]->[ref/s] */
	sx = 0;
	mtPrm->BasePrm.fspdRefKx = MlibPcalKxgain( 1, 4000000, iniPrm->Cycle, &sx, 24 );

/* トルクの単位変換定数の計算[24bit/MAX]->[%] */
	sx = 0;
	mtPrm->BasePrm.trqKx = MlibPcalKxgain( 1, iniPrm->MaxTrq, MAX_TRQ24, &sx, 24 );

	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );

/* 速度の単位変換定数の計算[30bit/MAX]->[24bit/OS] */
	/* buf = spd * maxSpd/(ovSpd*64) */
	/* buf = spd * maxSpd * (maxSpd_num/maxSpd_deno) / (ovSpd*(ovSpd_num/ovSpd_deno)*64) */
	sx = sy = 0;
	kx = MlibPcalKxmulx( iniPrm->MaxSpd, maxSpd_num, ovSpd_deno, &sx );
	ky = MlibPcalKxmulx( iniPrm->OverSpd*64, ovSpd_num, maxSpd_deno, &sy );
	mtPrm->BasePrm.maxToOsKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

/* 速度の単位変換定数の計算[24bit/OS]->[30bit/MAX] */
	mtPrm->BasePrm.osToMaxKx = MlibPcalKxdivx( ky, sy, kx, sx, 24 );

/* 速度の単位変換定数の計算[指令単位/s]->[24bit/OS] */
/* 速度の単位変換定数の計算[24bit/OS]->[指令単位/s] */
	if( iniPrm->LinearMotor != FALSE )
	{
		sx = sy = 0;
		mPrmCalcPow( 1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxmulx( iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx );
		kx = MlibPcalKxmulx( kx, ovSpd_deno, pitch_num, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, pitch_deno, ovSpd_num, &sy );
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
	}
	else
	{
		sx = sy = 0;
		kx = MlibPcalKxmulx( 60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy );
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
	}


/* 速度の単位変換定数の計算[**.*** ref/制御周期]→[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx( 1, 4000000, 1, &sx );
	ky = MlibPcalKxmulx( (1 << iniPrm->SpdDiv), iniPrm->Cycle, 1, &sy );
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

#if (FLOAT_USE==TRUE)
	mtPrm->BasePrm.PosingSpdMax = (LONG)( (float)0x40000000 * mtPrm->BasePrm.PosingSpdInvKx );

/* POSING分解能算出 --OverSpeedレベルを小数点位置とする */
	ovSpd_num = (LONG)( (float)MAX_SPD24 * mtPrm->BasePrm.osToRefKx );	/* Over Speed [ref/s] */
#else
	mtPrm->BasePrm.PosingSpdMax = MlibMulgain32( 0x40000000, mtPrm->BasePrm.PosingSpdInvKx );

/* POSING分解能算出 --OverSpeedレベルを小数点位置とする */
	ovSpd_num = MlibMulgain32( MAX_SPD24, mtPrm->BasePrm.osToRefKx );	/* Over Speed [ref/s] */
#endif /* FLOAT_USE */

	for( sx = MG_SPDDIV_RANGE; sx > 0; sx-- )
	{
		maxSpd_num = mtPrm->BasePrm.PosingSpdMax >> sx;
		if(ovSpd_num < maxSpd_num)
		{
			break;
		}
	}
	maxSpd_deno = iniPrm->SpdDiv + sx;		/* 小数点ビット位置 */


/* 加減速定数の計算[10000 x ref/s^2]→[**.*** ref/制御周期^2] */
	/* buf = rate * 2^vel_shift * 10000 / (4000000[.25us]/cyc[.25us])^2 */
	/* buf = rate * 2^vel_shift * cyc[.25us]^2 / (40000)^2 */
	sx = 0;
	kx = MlibPcalKxgain( (1 << maxSpd_deno), iniPrm->Cycle, 40000, &sx, 0 );
	mtPrm->BasePrm.PosingRateKx = MlibPcalKxgain( kx, iniPrm->Cycle, 40000, &sx, 24 );

/* 速度単位変換定数の計算[ref/s]→[**.*** ref/制御周期] */
	/* buf = spd * 2^vel_shift / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * 2^vel_shift * cyc[.25us] / 4000000[.25us] */
	sx = 0;
	mtPrm->BasePrm.PosingSpdKx = MlibPcalKxgain( (1 << maxSpd_deno), iniPrm->Cycle, 4000000, &sx, 24 );

/* 速度の単位変換定数の計算[**.*** ref/制御周期]→[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx( 1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx( (1 << maxSpd_deno), iniPrm->Cycle, 1, &sy );
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
#if (FLOAT_USE==TRUE)
	mtPrm->BasePrm.mgSpdMax = (LONG)( (float)0x40000000 * mtPrm->BasePrm.PosingSpdInvKx );
#else
	mtPrm->BasePrm.mgSpdMax = MlibMulgain32( 0x40000000, mtPrm->BasePrm.PosingSpdInvKx );
#endif /* FLOAT_USE */

/* MG_BASE_PRMセット */
	mtPrm->BasePrm.vel_sft_exp = iniPrm->SpdDiv;
	mtPrm->BasePrm.vel_shift = maxSpd_deno;
	mtPrm->BasePrm.vel_shift_s = MG_VREMBIT_MAX - maxSpd_deno;
	mtPrm->BasePrm.vel_max = 1 << (31 - maxSpd_deno);
	mtPrm->BasePrm.seg_num = iniPrm->SegNum;

	sx = 0;
	kx = MlibPcalKxgain( mtPrm->BasePrm.PosingSpdMax, iniPrm->Cycle, 4000000, &sx, 24 );
#if (FLOAT_USE==TRUE)
	mtPrm->BasePrm.seg_max_ref = iniPrm->SegNum * kx;
#else
	mtPrm->BasePrm.seg_max_ref = MlibMulgain32( iniPrm->SegNum, kx );
#endif /* FLOAT_USE */
	mtPrm->BasePrm.osVel = ovSpd_num;

#if 0
/* ACC_TIME用加速度単位変換定数の計算[(ref/s)/(0.1ms)]→[**.*** ref/制御周期^2] <Rev2.01>*/
	sx = sy = 0;
	kx = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, iniPrm->Cycle, &sx);
	ky = MlibPcalKxmulx(40000, 40000, 1, &sy);
	mtPrm->BasePrm.taccRateKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);

/* ACC_TIME用加速度単位変換定数の計算[(2^7*ref/s)/(0.1ms)]→[**.*** ref/制御周期^2] <Rev2.01>*/
	sy = 0;
	ky = MlibPcalKxmulx( 40000, 40000, (1<<7), &sy );
	mtPrm->BasePrm.taccRate7Kx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
#endif

/* TFF/TLM/ACC_TIME使用選択	*/
#if 0 /* M2プロファイル対応	*/
	ｍPrmTrefMode( mtPrm, iniPrm );
#else
	mtPrm->BasePrm.TrefModePos = TREFMODE_TFF_TLIM;/* <S06D */
#endif
#if 0
	prm->BasePrm.SetCmdAccRate = ((iniPrm->OptEnable&0x0F00) != 0) ? &MPrmCalcAccTimeRate : NULL;
#endif

/* 位置情報関連PRMセット */
	mtPrm->PsnPrm.absPG = iniPrm->absPG;									/* アブソPG使用 */
	mtPrm->PsnPrm.TdlyIn = iniPrm->dlyTime;
	mtPrm->PsnPrm.SegNum = iniPrm->SvCycle;
	
	/* 速度単位変換 定数作成 */
	pcal_SpdInit( mtPrm, iniPrm, Prm );
	/* トルク単位変換 定数作成 */
	pcal_TrqInit( mtPrm, iniPrm, Prm );
	/* 加速度単位変換 定数作成 */
	pcal_AccInit( mtPrm, iniPrm, maxSpd_deno, Prm );
	/* 位置単位変換 定数作成 */
	pcal_PosInit( mtPrm, iniPrm, Prm );

/* Option指令割付定義 */
	mtPrm->ProfPrm.OutSigEnable = iniPrm->OutSigEnable;/* <S14D> */
	mtPrm->OptMap = OptMapTbl;	/* デフォルト値セット */
	if((iniPrm->OptEnable & 0x01) != 0)
	{
		if((iniPrm->OptMap[0]&0x00F0) == 0) mtPrm->OptMap.accfilBitMsk = 0;
		if((iniPrm->OptMap[0]&0xF000) == 0)	mtPrm->OptMap.gselBitMsk = 0;
		if((iniPrm->OptMap[1]&0x00F0) == 0)	mtPrm->OptMap.vppiBitMsk = 0;
		if((iniPrm->OptMap[1]&0xF000) == 0)	mtPrm->OptMap.ppiclrBitMsk = 0;
		if((iniPrm->OptMap[2]&0x00F0) == 0)	mtPrm->OptMap.pclBitMsk = 0;
		if((iniPrm->OptMap[2]&0xF000) == 0)	mtPrm->OptMap.nclBitMsk = 0;
		if((iniPrm->OptMap[3]&0x00F0) == 0)	mtPrm->OptMap.bankBitMsk = 0;
		if((iniPrm->OptMap[3]&0xF000) == 0)	mtPrm->OptMap.ltBitMsk = 0;
		if((iniPrm->OptMap[4]&0xF000) == 0)	mtPrm->OptMap.outpBitMsk = 0;
		if(iniPrm->OutSigEnable == 0)		mtPrm->OptMap.outpBitMsk = 0;

		mtPrm->OptMap.accfilBit = (iniPrm->OptMap[0] & 0xF);
		mtPrm->OptMap.gselBit = (iniPrm->OptMap[0] >> 8) & 0xF;
		mtPrm->OptMap.vppiBit = (iniPrm->OptMap[1] & 0xF);
		mtPrm->OptMap.ppiclrBit = (iniPrm->OptMap[1] >> 8) & 0xF;
		mtPrm->OptMap.pclBit = (iniPrm->OptMap[2] & 0xF);
		mtPrm->OptMap.nclBit = (iniPrm->OptMap[2] >> 8) & 0xF;
		mtPrm->OptMap.bankBit = (iniPrm->OptMap[3] & 0xF);
		mtPrm->OptMap.ltBit = (iniPrm->OptMap[3] >> 8) & 0xF;
		mtPrm->OptMap.outpBit = (iniPrm->OptMap[4] >> 8) & 0xF;
	}
	
	return 0;
}

/****************************************************************************
Vendor Specifics I/O割り付け計算
****************************************************************************//* <S1C0> */
VOID	MPrmCalcSvCmdIo(MT_PRM *mtPrm, PRMDATA *Prm )
{
	LONG	i,j,sft;
	USHORT	work;
	ULONG	enaOutbit;

	/* ローカル変数初期化 */
	enaOutbit = 0;

	/* パラメータの値に応じてセットする。	*/
	for(i = 0; i < VENDOR_IOBIT_NUM; i++)
	{
		j = i >> 2;
		sft = (i & 0x03) * 4;
		work = (Prm->svcmdIoOut[j] >> sft ) & 0x000F;
		if(work != 0)
		{
			mtPrm->SvCmdIoOutMap[i].bitNo = (UCHAR)(work + VENDOR_IOBIT_OFFSET);
			mtPrm->SvCmdIoOutMap[i].bitsMsk = SvCmdBitMaskTable[i];
			enaOutbit |= (LONG)SvCmdBitMaskTable[i] << mtPrm->SvCmdIoOutMap[i].bitNo;
		}
		else
		{
			mtPrm->SvCmdIoOutMap[i].bitNo = (UCHAR)0;
			mtPrm->SvCmdIoOutMap[i].bitsMsk = (UCHAR)0;
		}
	}
	
	/* PnB24にSVCMD_IO(出力)有効/無効状態をセットする。。	*/
	Prm->m3_enaOutbitL = ((Prm->m3_enaOutbitL & 0x0000FFFF) | enaOutbit);
}

/****************************************************************************
 べき乗演算処理
****************************************************************************/
static	VOID mPrmCalcPow( LONG data, LONG exp, LONG *num, LONG *deno )
{
	if( exp > 0 )
	{
		while( (exp--) > 0 )
		{
			data *= 10;
		}
		*num = data;
		*deno = 1;
	}
	else if( exp < 0 )
	{
		exp = MlibLABS(exp);
		while( (exp--) > 0 )
		{
			data *= 10;
		}
		*num = 1;
		*deno = data;
	}
	else
	{
		*num = data;
		*deno = 1;
	}
}



/****************************************************************************************************/
/*		速度単位選択	変換定数作成処理															*/
/****************************************************************************************************/
static	void pcal_SpdInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	mtPrm->BasePrm.m3ConvUnit.m3SpdSelUnit = Prm->m3_velUnitL;

	switch (mtPrm->BasePrm.m3ConvUnit.m3SpdSelUnit) {
		case 0:	/* 指令単位/sec */
			/*--------------------------------------------------------------------------------------*/
			/* 指令単位/sec --> 指令単位/sec														*/
			/* 指令　速度変換(位置制御)									（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 指令単位/sec --> 最大速度/40000000h													*/
			/* 指令　速度変換(速度制御、トルク制御)						（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrRefToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 内部演算単位 --> 指令単位/sec														*/
			/* モニタ　CSPD,FSPD,TSPD,SPD_LIM							（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonPowKx( Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRefKx( mtPrm, iniPrm, 1, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRefKx( mtPrm, iniPrm, 1, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> 指令単位/sec																	*/
			/* 															（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToRefKx( mtPrm, iniPrm, Prm );
			break;

		case 1:	/* 指令単位/min */
			/*--------------------------------------------------------------------------------------*/
			/* 指令単位/min --> 指令単位/sec														*/
			/* 指令　速度変換											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrMinToRefKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 指令単位/min --> 最大速度/40000000h													*/
			/* 指令　速度変換(速度制御、トルク制御)						（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrMinToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 内部演算単位 --> 指令単位/min														*/
			/* モニタ　CSPD,FSPD,TSPD,SPD_LIM							（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToMinKx( Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRefKx( mtPrm, iniPrm, 60, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRefKx( mtPrm, iniPrm, 60, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> 指令単位/min																	*/
			/* 															（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToMinKx( mtPrm, iniPrm, Prm );
			break;

		case 2:	/* 定格速度に対する％ */
			/*--------------------------------------------------------------------------------------*/
			/* 定格速度に対する％ --> 指令単位/sec 													*/
			/* 指令　速度変換											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrRatToRefKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 定格速度に対する％ --> 最大速度/40000000h											*/
			/* 指令　速度変換(速度制御、トルク制御)						（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrRatToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 内部演算単位 --> 定格速度に対する％													*/
			/* モニタ　CSPD,FSPD,TSPD,SPD_LIM							（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToRatKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRatKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRatKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> 定格速度に対する％ 		 													*/
			/* 															（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToRatKx( mtPrm, iniPrm, Prm );
			break;

		case 3:	/* min-1(RPM), mm/s */
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> 指令単位/sec																	*/
			/* 指令　速度変換											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrRpmToRefKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> 最大速度/40000000h		 													*/
			/* 指令　速度変換(速度制御、トルク制御)						（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrRpmToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> RPM 		 																	*/
			/* 															（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 内部演算単位 --> RPM																	*/
			/* モニタ　CSPD,FSPD,TSPD,SPD_LIM							（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToRpmKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRpmKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRpmKx( mtPrm, iniPrm, Prm );
			break;

		case 4:	/* 最大速度/40000000h */
			/*--------------------------------------------------------------------------------------*/
			/* 最大速度/40000000h --> 指令単位/sec 													*/
			/* 指令　速度変換											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrMaxToRefKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 最大速度/40000000h --> 最大速度/40000000h											*/
			/* 指令　速度変換(速度制御、トルク制御)						（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* 内部演算単位 --> 最大速度/40000000h													*/
			/* モニタ　CSPD,FSPD,TSPD,SPD_LIM							（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToMaxKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToMaxKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> 最大速度/40000000h		 													*/
			/* 															（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToMaxKx( mtPrm, iniPrm, Prm );
			break;

		default:
			break;

	}
	/* 原点復帰アプローチ速度、原点復帰クリープ速度変換用 */
	mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm = pcal_SpdRefToRpmKxMinus3( mtPrm, iniPrm );	/* 指令単位/s → RPM */
	mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef = pcal_SpdRpmToRefKxMinus3( mtPrm, iniPrm );	/* RPM → 指令単位/s */
}


/****************************************************************************************************/
/*		トルク単位選択	変換定数作成処理															*/
/****************************************************************************************************/
static	void pcal_TrqInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	SHORT	TrqExp;

	TrqExp = (SHORT)iniPrm->RatTrqExp;
	mtPrm->BasePrm.m3ConvUnit.m3TrqSelUnit = Prm->m3_trqUnitL;

	switch (mtPrm->BasePrm.m3ConvUnit.m3TrqSelUnit) {
		case 0:	/* Nm(N) */ /* 2009.01.20 Y.Oka 現状未サポート */
			/*--------------------------------------------------------------------------------------*/
			/* Nm(N) --> 最大ﾄﾙｸ/40000000h															*/
			/* 指令tlim、tff											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqUnitToMax = pcal_TrqNmToMaxKx(mtPrm, iniPrm, TrqExp, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* 最大ﾄﾙｸ/40000000h --> Nm(N) 															*/
			/* モニタ													（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqMaxToUnit = pcal_TrqMaxToNmKx(mtPrm, iniPrm, TrqExp, -1, Prm);
			break;

		case 1:	/* 定格ﾄﾙｸに対する％ */
			/*--------------------------------------------------------------------------------------*/
			/* 定格ﾄﾙｸに対する％ --> 最大ﾄﾙｸ/40000000h												*/
			/* 指令tlim、tff											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqUnitToMax = pcal_TrqRatToMaxKx(mtPrm, iniPrm, 0, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* 最大ﾄﾙｸ/40000000h --> 定格ﾄﾙｸに対する％ 												*/
			/* モニタ													（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqMaxToUnit = pcal_TrqMaxToRatKx(mtPrm, iniPrm, 0, -1, Prm);
			break;

		case 2:	/* 最大ﾄﾙｸ/40000000h */
			/*--------------------------------------------------------------------------------------*/
			/* 最大ﾄﾙｸ/40000000h --> 最大ﾄﾙｸ/40000000h 												*/
			/* 指令tlim、tff											（指数はそのまま）			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqUnitToMax = pcal_TrqPowKx(0, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* 最大ﾄﾙｸ/40000000h --> 最大ﾄﾙｸ/40000000h 												*/
			/* モニタ													（指数は反転）				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqMaxToUnit = pcal_TrqPowKx(0, -1, Prm);
			break;

		default:
			break;
	}
}


/****************************************************************************************************/
/*		加速度単位選択	変換定数作成処理															*/
/****************************************************************************************************/
static	void pcal_AccInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, PRMDATA *Prm)
{
	mtPrm->BasePrm.m3ConvUnit.m3AccSelUnit = Prm->m3_accUnitL;

	switch (mtPrm->BasePrm.m3ConvUnit.m3AccSelUnit)
	{
		case 0:
			/*--------------------------------------------------------------------------------------*/
			/* 加速度(指令単位/sec^2) --> 加速度(指令単位/cyc^2)									*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToCyc2 = pcal_AccS2TOCyc2Kx(maxSpd_deno, iniPrm->Cycle, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* 加速度(指令単位/sec^2) --> 加速度([2^24/OS]/cycle)									*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToOs = pcal_AccRefToOsKx(iniPrm, Prm);
			break;

//		case 1: /* 2009.01.20 Y.Oka 現状未サポート */
			/*--------------------------------------------------------------------------------------*/
			/* 加速時間(ms) --> 加速度(指令単位/sec^2)												*/
			/*--------------------------------------------------------------------------------------*/
//			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToCyc2 = pcal_AccMsToCyc2Kx(mtPrm, iniPrm, maxSpd_deno, cycle, pprm_ptr);
			/*--------------------------------------------------------------------------------------*/
			/* 加速時間(ms) --> 加速度([2^24/OS]/cycle)												*/
			/*--------------------------------------------------------------------------------------*/
//			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToOs = pcal_AccMsToOsKx(iniPrm);
//			break;

		default:
			break;
	}
}

/****************************************************************************************************/
/*		位置単位選択	変換定数作成処理															*/
/****************************************************************************************************/
static	void pcal_PosInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	/* 指令用 */
	mtPrm->BasePrm.m3ConvUnit.m3PosUnitToRef = pcal_PosPowKx(1, Prm);

	/* モニタ用 */
	mtPrm->BasePrm.m3ConvUnit.m3PosRefToUnit = pcal_PosPowKx(-1, Prm);
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	同じ単位の指数計算のみ											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrPowKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	同じ単位の指数計算のみ											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonPowKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
//	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	指令単位/sec --> RPM(mm/s)										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = (スケールピッチ(pm) * スケールピッチ指数) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* 回転型 */
		/* 	変換定数 = 60 / PgPuls */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	RPM(mm/s) --> 指令単位/sec										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = PgPuls * 10^9 / (スケールピッチ(pm) * スケールピッチ指数) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* 回転型 */
		/* 変換定数 = PgPuls / 60 */
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, 60, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	RPM(mm/s) --> 指令単位/sec										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = PgPuls * 10^9 / (スケールピッチ(pm) * スケールピッチ指数) */
		mPrmCalcPow(1 , 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* 回転型 */
		/* 変換定数 = PgPuls / 60 */
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, 60, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	指令単位/sec --> 定格速度に対する％								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = (スケールピッチ(pm) * スケールピッチ指数) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* 変換定数 = 変換定数 * (100 / 定格速度(mm/s)) */
	}
	else
	{ /* 回転型 */
		/* 変換定数 = (60 / PgPuls) * (100 / 定格回転速度(rpm)) */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	kx = MlibPcalKxgain( kx, 100, iniPrm->RatSpd, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	定格速度に対する％ --> 指令単位/sec								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRatToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L ) * 1;
	base = ( base + iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 分解能(mm) = PgPuls * 10^9 / (スケールピッチ(pm) * スケールピッチ指数) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* 変換定数 = 分解能(ｍｍ) * (定格回転速度(rpm) / 100) */
	}
	else
	{ /* 回転型 */
		/* 変換定数 = (PgPuls / 60) * (定格回転速度(rpm) / 100) */
		kx = MlibPcalKxgain( iniPrm->PgPuls, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx,  0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->RatSpd, 100, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	指令単位/sec --> 最大速度/40000000h								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );
	base = ( base - iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 分解能(mm) = (スケールピッチ(pm) * スケールピッチ指数) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* 変換定数 = 分解能(mm) * (40000000h / 最大回転速度(rpm)) */
	}
	else
	{ /* 回転型 */
		/* 変換定数 = (60 / PgPuls) * (40000000h / 最大回転速度(rpm)) */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
}
	kx = MlibPcalKxgain( kx, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	指令単位/sec --> 最大速度/40000000h								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = (base - iniPrm->MaxSpdExp);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 分解能(mm) = (スケールピッチ(pm) * スケールピッチ指数) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* 変換定数 = 分解能(mm) * (40000000h / 最大回転速度(rpm)) */
	}
	else
	{ /* 回転型 */
		/* 変換定数 = (60 / PgPuls) * (40000000h / 最大回転速度(rpm)) */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	最大速度/40000000h --> 指令単位/sec								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );
	base = ( base + iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 分解能(mm) = PgPuls * 10^9 / (スケールピッチ(pm) * スケールピッチ指数) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* 変換定数 = 分解能(mm) * 最大回転速度(rpm) / 40000000h) */
	}
	else
	{ /* 回転型 */
		/* 変換定数 = (PgPuls / 60) * (最大回転速度(rpm) / 40000000h) */
		kx = MlibPcalKxgain( iniPrm->PgPuls, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx,  0);
	}
	kx = MlibPcalKxgain(kx, iniPrm->MaxSpd, 0x40000000, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	RPM(mm/s) --> 指令単位/min										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToMinKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = PgPuls * 10^9 / (スケールピッチ(pm) * スケールピッチ指数) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 60, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* 回転型 */
		/* 変換定数 = PgPuls */
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, 1, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	指令単位/min --> 指令単位/sec									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrMinToRefKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	mPrmCalcPow( 1, base, &num, &deno );

	/* 変換定数 = 1 / 60 */
	sx = 0;
	kx = MlibPcalKxgain( 1, 1, 60, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	指令単位/sec --> 指令単位/min									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToMinKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );

	/* 変換定数 = 60 */
	sx = 0;
	kx = MlibPcalKxgain( 1, 60, 1, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	RPM --> 定格速度に対する％										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );

	mPrmCalcPow( 1, base, &num, &deno );

	/* 変換定数 = 100 / 定格回転速度(rpm) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 100, iniPrm->RatSpd, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	RPM --> 最大速度/40000000h	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	base = ( base - iniPrm->MaxSpdExp );

	mPrmCalcPow(1, base, &num, &deno);

	/* 変換定数 = (40000000h / 最大回転速度(rpm) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	RPM --> 最大速度/40000000h	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );

	mPrmCalcPow( 1, base, &num, &deno );

	/* 変換定数 = (40000000h / 最大回転速度(rpm) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	指令単位/min --> 最大速度/40000000h								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrMinToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );
	base = ( base - iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 分解能(mm) = (スケールピッチ(pm) * スケールピッチ指数) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* 変換定数 = 分解能(mm) * (40000000h / 最大回転速度(rpm)) */
	}
	else
	{ /* 回転型 */
		/* 変換定数 = (1 / PgPuls) * (40000000h / 最大回転速度(rpm)) */
		kx = MlibPcalKxgain( 1, 1, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	kx = MlibPcalKxgain( kx, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（指令変換）	定格速度に対する％ --> 最大速度/40000000h						*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	mPrmCalcPow( 1, base, &num, &deno );

	/* 変換定数 = (40000000h / 最大回転速度(rpm)) * (定格回転速度(rpm) / 100) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, iniPrm->RatSpd, 100, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択	指令単位/sec --> RPM	10 -3乗固定											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdRefToRpmKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = 3;
	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = (スケールピッチ(pm) * スケールピッチ指数) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* 回転型 */
		/* 	ロータリ：変換定数 = 60 / PgPuls */
		/* 	フルク  ：変換定数 = 60 / (Pn20A * ピッチ当りのパルス数)
		     ※ iniPrm.mt.PgPuls = Pn20A * ピッチ当りのパルス数	*/
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択	RPM --> 指令単位/sec	10の-3乗固定										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdRpmToRefKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = -3;
	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* 変換定数 = PgPuls * 10^9 / (スケールピッチ(nm) * スケールピッチ指数) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* 回転型 */
		/* ロータリ：変換定数 = PgPuls / 60 */
		/* 	フルク  ：変換定数 = (Pn20A * ピッチ当りのパルス数) / 60
		     ※ iniPrm.mt.PgPuls = Pn20A * ピッチ当りのパルス数	*/
		kx = MlibPcalKxgain( iniPrm->PgPuls, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx, 0 );
	}
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	24bitOS → 指令単位/sec											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	base, num, deno;
	LONG	kx, sx;
	LONG	ky, sy;
	LONG	ovSpd_num, ovSpd_deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	sx = sy = 0;

	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* (ギヤA * OverSpd * PgPuls * cnvVal) / (ギヤB * MAX_SPD24 * ScalePitch) */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxmulx( iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx );
		kx = MlibPcalKxmulx( kx, ovSpd_deno, pitch_num, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, pitch_deno, ovSpd_num, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}
	else
	{ /* 回転型 */
		/* (ギヤA * OverSpd * PgPuls * cnvVal) / (ギヤB * MAX_SPD24 * 60) */
		kx = MlibPcalKxmulx( 60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	24bitOS → 定格速度に対する％									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );
	sx = 0;

	/* (OverSpd * 100) / (MAX_SPD24 * 定格速度) */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, 100, iniPrm->RatSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->OverSpd, MAX_SPD24, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	24bitOS → min-1(RPM), mm/s										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	sx = 0;

	/* OverSpd / MAX_SPD24 */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->OverSpd, MAX_SPD24, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	24bitOS → 最大速度/40000000h									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );
	sx = 0;

	/* (OverSpd * 40000000H) / (MAX_SPD24 * 最大速度) */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->OverSpd, MAX_SPD24, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	最大速度/40000000h → 指令単位/sec								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	base, num, deno;
	LONG	kx, sx;
	LONG	ky, sy;
	LONG	maxSpd_num, maxSpd_deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );
	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	sx = sy = 0;

	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		/* (ギヤA * MaxSpd * PgPuls * cnvVal) / (ギヤB * MAX_SPD30 * ScalePitch) */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxmulx( iniPrm->ScalePitch, iniPrm->EGearB, 0x40000000, &sx );
		kx = MlibPcalKxmulx( kx, maxSpd_deno, pitch_num, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->MaxSpd, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, pitch_deno, maxSpd_num, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}
	else
	{ /* 回転型 */
		/* (ギヤA * MaxSpd * PgPuls * cnvVal) / (ギヤB * MAX_SPD30 * 60) */
		kx = MlibPcalKxmulx( 60*maxSpd_deno, iniPrm->EGearB, 0x40000000, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->MaxSpd*maxSpd_num, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	最大速度/40000000h → 定格速度に対する％						*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	maxSpd_num, maxSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );
	sx = 0;

	/* (MaxSpd * 100) / (MAX_SPD30 * 定格速度) */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, 100, iniPrm->RatSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->MaxSpd, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		速度単位選択（モニタ変換）	最大速度/40000000h → min-1(RPM), mm/s							*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonMaxToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	maxSpd_num, maxSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	sx = 0;

	/* MaxSpd / MAX_SPD30 */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->MaxSpd, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		トルク単位選択	同じ単位の指数計算のみ														*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqPowKx( LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );

	sx = 0;
//	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 48 );

	return lRet;
}


/* 2009.01.20 Y.Oka 現状未サポート */
/****************************************************************************************************/
/*		トルク単位選択	Nm(N) --> 最大ﾄﾙｸ/40000000h				 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqNmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + (-TrqExp) );

	mPrmCalcPow( 1, base, &num, &deno );
	/* 変換定数 = 40000000h / 最大ﾄﾙｸ(Nm(N)) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxTrq, &sx, 0 );
	kx = MlibPcalKxgain( kx, 100, iniPrm->RatTrq, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/* 2009.01.20 Y.Oka 現状未サポート */
/****************************************************************************************************/
/*		トルク単位選択	最大ﾄﾙｸ/40000000h --> Nm(N)				 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqMaxToNmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );
	/* 変換定数 = 最大ﾄﾙｸ(Nm(N)) / 40000000h */
	sx = 0;
	kx = MlibPcalKxgain( iniPrm->MaxTrq, iniPrm->RatTrq, 100, &sx, 0 );	/* % -> Nm */
	kx = MlibPcalKxgain( kx, 1, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		トルク単位選択	定格ﾄﾙｸに対する％ --> 最大ﾄﾙｸ/40000000h	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );
	/* 変換定数 = 40000000h / 最大ﾄﾙｸ(%) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxTrq, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		トルク単位選択	最大ﾄﾙｸ/40000000h --> 定格ﾄﾙｸに対する％	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );
	/* 変換定数 = 最大トルク(%) / 40000000h */
	sx = 0;
	kx = MlibPcalKxgain( 1, iniPrm->MaxTrq, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		加速度単位選択	加速度(指令単位/sec^2)  --> 加速度(**.*** 指令単位/cyc^2)					*/
/****************************************************************************************************/
/*  USHORT	Cycle			モーション制御周期[0.25us]												*/
/*	LONG 	maxSpd_deno 	小数点ビット位置														*/
/****************************************************************************************************/
/* buf = rate * 2^vel_shift * 10^accr_base_unit / (4000000[.25us]/cyc[.25us])^2						*/
/* buf = rate * 2^vel_shift * cyc[.25us]^2 * 10^accr_base_unit/ (4000000)^2							*/
/****************************************************************************************************/
static	KSGAIN pcal_AccS2TOCyc2Kx( LONG maxSpd_deno, LONG cycle, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = (Prm->m3_accBase4L) * inv;

	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
	kx = MlibPcalKxgain( (1 << maxSpd_deno), cycle, 4000000, &sx, 0 );
	kx = MlibPcalKxgain( kx, cycle, 4000000, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		加速度単位選択	加速度(指令単位/sec^2) --> 加速度([2^24/OS]/cycle)							*/
/****************************************************************************************************/
static	KSGAIN pcal_AccRefToOsKx( MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_accBase4L );
	mPrmCalcPow( 1, base, &num, &deno );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );

	if( iniPrm->LinearMotor != FALSE )
	{ /* リニア型 */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno );
		/*------------------------------------------------------------------------------------------*/
		/*	Gain = 10^n×ScalePitch×(B/A)×(1/PgPuls)×(2^24/OS)×cyc[.25us]						*/
		/*------------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( iniPrm->ScalePitch, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->Cycle, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, ovSpd_deno, iniPrm->OverSpd*ovSpd_num, &sx,0 );
		kx = MlibPcalKxgain( kx, num, deno, &sx,0 );
		lRet = MlibPcalKxgain( kx, MAX_SPD24, 4000000, &sx, 24 );
	}
	else
	{ /* 回転型 */
		/*------------------------------------------------------------------------------------------*/
		/*	Gain = 10^n×(B/A)×(1/PgPuls)×60×(2^24/OS)×cyc[.25us]								*/
		/*------------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( 60*ovSpd_deno, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
		kx = MlibPcalKxgain( kx, MAX_SPD24, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->Cycle, iniPrm->OverSpd*ovSpd_num, &sx, 0 );
		lRet = MlibPcalKxgain( kx, num, deno*4000000, &sx, 24 );
	}

	return lRet;
}


#if 0 /* 2009.1.20 Y.Oka 現状未対応 */
/****************************************************************************************************/
/*		加速度単位選択	加速時間(ms)  --> 加速度(**.*** 指令単位/cyc^2) 							*/
/****************************************************************************************************/
/*  USHORT	Cycle			モーション制御周期[0.25us]												*/
/*	LONG 	maxSpd_deno 	小数点ビット位置														*/
/****************************************************************************************************/
/*	加速度単位選択	加速時間(ms)  --> 加速度(指令単位/sec^2) --> 加速度(**.*** 指令単位/cyc^2) 		*/
/*	の変換を行う																					*/
/****************************************************************************************************/
/* 2008.12.05 Y.Oka "inv" = 1であり、本処理の使用はここのみであるため、引数を減らすためinvを削除。 */
//LONG pcal_AccMsToCyc2Kx(LONG maxSpd_deno, USHORT cycle, CHAR inv)
static	LONG pcal_AccMsToCyc2Kx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, USHORT cycle, PRMDATA *Prm )
{
	LONG	lRet, sx, kx;
	LONG	base, num, deno;
	LONG	sp;

//	base = (pprm_ptr->m3_accBase4L | (pprm_ptr->m3_accBase4H << 16)) * inv;
	base = (Prm->m3_accBase4L | (Prm->m3_accBase4H << 16));
	base = (base + iniPrm->MaxSpdExp);
	sx = 0;

	/*------------------------------------------------*/
	/* 加速時間(ms)  --> 加速度(指令単位/sec^2)の変換 */
	/*------------------------------------------------*/
	if(iniPrm->Linear)
	{
		/* 変換定数 = モータ定格速度(rpm) * (C022h,C023h) * 10^9  / (ｽｹｰﾙﾋﾟｯﾁ(nm) * ｽｹｰﾙﾋﾟｯﾁ指数) */
		sp = iniPrm->ScalePitch;
		kx = MlibPcalKxgain(iniPrm->RatSpd, iniPrm->PgPuls, 1, &sx, 0);
		kx = MlibPcalKxgain(kx, 1, sp, &sx, 0);
		base = base + 9 - iniPrm->ScalePitchExp;
	}
	else
	{
		/* 変換定数 = モータ定格速度(指令単位/sec) */
		kx = MlibPcalKxgain(iniPrm->RatSpd, iniPrm->PgPuls, 60, &sx, 0);
	}
	kx = MlibPcalKxgain(kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0);

	/*-----------------------------------------------------------------*/
	/* 加速度(指令単位/sec^2) --> 加速度(**.*** 指令単位/cyc^2) の変換 */
	/*-----------------------------------------------------------------*/
	kx = MlibPcalKxgain((1 << maxSpd_deno), cycle, 4000000, &sx, 0);
	kx = MlibPcalKxgain(kx, cycle, 4000000, &sx, 0);
	mPrmCalcPow(1, base, &num, &deno);
	lRet = MlibPcalKxgain(kx, num, deno, &sx, -30);

	return lRet;
}
#endif /* #if 0 */


#if 0/* 2009.1.20 Y.Oka 現状未対応 */
/****************************************************************************************************/
/*		加速度単位選択	加速時間(ms) --> 加速度([2^24/OS]/cycle)									*/
/****************************************************************************************************/
static	LONG pcal_AccMsToOsKx( MTPRM_INI *iniPrm )
{
	LONG	lRet=0x7FFFFF;	// 暫定処置

	if(iniPrm->Linear)
	{

	}
	else
	{

	}

	return lRet;
}
#endif /* #if 0 */


/****************************************************************************************************/
/*		位置単位選択	同じ単位の指数計算のみ														*/
/****************************************************************************************************/
static	KSGAIN pcal_PosPowKx( LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = ( Prm->m3_posBaseL ) * inv;

	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************
 原点復帰方向の設定(Motion Control)
 引数：data=TRUE：負方向、FALSE:正方向
****************************************************************************/
LONG MPrmSetZretDir( MT_PRM *prm, BOOL data )
{
	if( data )
	{
		prm->MtSeqPrm.zret_dir = -DISTANCE_MAX;
	}
	else
	{
		prm->MtSeqPrm.zret_dir = DISTANCE_MAX;
	}

	return 0;
}


/****************************************************************************
 入力信号モニタ割付設定(Motion API)
 引数：data  bit00-03 IO:0割付(n=0:割付なし、n=1-8:SI[n-1]信号)
             bit04-07 IO:1割付
                ;        ;
             bit28-31 IO:7割付
****************************************************************************//* <S004> */
LONG MPrmSetInSigMap(MT_PRM* prm, LONG data)
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

/****************************************************************************
 加速度1設定(MG用)
****************************************************************************/
LONG	MPrmSetAccRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;
#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate1 = data;
	return 0;
}


/****************************************************************************
 減速度り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetDecAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data)
{
#if (FLOAT_USE==TRUE)
	mgPrm->altspd_d = ( (float)data * prm->BasePrm.PosingSpdKx );
#else
	mgPrm->altspd_d = MlibMulgain32( data, prm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	return 0;
}


/****************************************************************************
 加速度切り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetAccAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data)
{
#if (FLOAT_USE==TRUE)
	mgPrm->altspd_a = ( (float)data * prm->BasePrm.PosingSpdKx );
#else
	mgPrm->altspd_a = MlibMulgain32( data, prm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	return 0;
}


/****************************************************************************
 指数関数加減速バイアス設定(AccFilter)
****************************************************************************/
LONG MPrmSetExpFilBias(MT_PRM *prm, ACF_PRM *acfPrm, LONG bias)
{
	LONG kx, sx;
	sx = 0;

	/* buf = spd / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * cyc[.25us] / 4000000[.25us] */
	kx = MlibPcalKxgain(1, prm->BasePrm.MotionCycle, 4000000, &sx, 24);

#if (FLOAT_USE==TRUE)
	acfPrm->expbias = MlibMulgain32(bias, kx);
#else
		acfPrm->expbias = bias * kx;
#endif
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
LONG MPrmSetExpFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG exptb)
{
	LONG buf;

	/* exptb = 2^vel_shift * (cyc /(exptb*400 + cyc)) */
	buf = ((1 << prm->BasePrm.vel_sft_exp) * prm->BasePrm.MotionCycle);	/* 2^vel_shift * cyc */
//	buf = buf / ((exptb*4) + prm->BasePrm.MotionCycle);/* <S033> */
	buf = buf / ((exptb*400) + prm->BasePrm.MotionCycle);/* <S033> */

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
LONG MPrmSetMaFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG matb)
{
	LONG	tb, i;
	LONG	nTb, iTb;
	MATB_PRM maPrm;

	//	tb = matb * 4;	/* [0.1ms]->[0.25us] *//* <S033> */
		tb = matb * 400;	/* [0.1ms]->[0.25us] *//* <S033> */
	if(tb >= prm->BasePrm.MotionCycle)
	{/* 時定数が制御サイクル以上の場合 */
		nTb = (tb / prm->BasePrm.MotionCycle);	/* 移動平均回数計算 */
		iTb = (USHORT)(nTb / ACF_MAX_SZ);
		if(iTb > 0)
		{
			for(i = 1; i < 5; i++)
			{
				if((1<<i) > iTb) break;
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
 加速度2設定(MG用)
****************************************************************************/
LONG	MPrmSetAccRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;

#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */

	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate2 = data;
	return 0;
}


/****************************************************************************
 減速度1設定(MG用)
****************************************************************************/
LONG	MPrmSetDecRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;
#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else	
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */

	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->decrate1 = data;
	return 0;
}


/****************************************************************************
 減速度2設定(MG用)
****************************************************************************/
LONG	MPrmSetDecRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;
	
#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */

	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->decrate2 = data;
	return 0;
}


/****************************************************************************
 入力信号モニタ(Pn860-866)
 引数：
MT_PRM	*prm		Ptパラメータ
USHORT	data	bit00-07	:CN1-**入力ビット位置
				bit08		:有効無効
			 	bit16-23	:CN1-**入力ビット位置
				bit24		:有効無効
UCHAR	CnNum	IO			:SI00～06(SI10～16)の入力番号
****************************************************************************/
void	MPrmSetInSigMonitor(MT_PRM *prm, USHORT data, UCHAR IoNum)
{

	prm->InSigMonM3.bitNo[IoNum]     = (UCHAR)  data & 0x000F;
	if((data & 0x0010)>>4 )
	{
		prm->InSigMonM3.bitsMsk |= (USHORT)(1 << IoNum);
	}
	else
	{
		prm->InSigMonM3.bitsMsk &= ~(USHORT)(1 << IoNum);
	}

	prm->InSigMonM3.bitNo[IoNum +1]  = (UCHAR)((data & 0x0F00)>>8 );
	if((data & 0x1000)>>12 )
	{
		prm->InSigMonM3.bitsMsk |= (USHORT)(1 << (IoNum+1));
	}
	else
	{
		prm->InSigMonM3.bitsMsk &= ~(USHORT)(1 << (IoNum+1));
	}

	return;
}

//#if ( CCSW_SFTID == SFTID_EXIST )/* <S1F9> */
/****************************************************************************
 出力信号モニタ(Pn868-86A)
 引数：
MTPRM*	prm		Ptパラメータ
USHORT	data	bit00-07	:CN1-**出力ビット位置
				bit08		:有効無効
			 	bit16-23	:CN1-**出力ビット位置
				bit24		:有効無効
UCHAR	CnNum	IO			:SO0～3(5)の入力番号
****************************************************************************/
VOID MPrmSetOutSigMonitor(MT_PRM* prm, USHORT data, UCHAR IoNum)
{

	prm->OutSigMonM3.bitNo[IoNum]     = (UCHAR)  data & 0x000F;
	if((data & 0x0010)>>4 )
	{
		prm->OutSigMonM3.bitsMsk |= (USHORT)(1 << IoNum);
	}
	else
	{
		prm->OutSigMonM3.bitsMsk &= ~(USHORT)(1 << IoNum);
	}

	prm->OutSigMonM3.bitNo[IoNum +1]  = (UCHAR)((data & 0x0F00)>>8 );
	if((data & 0x1000)>>12 )
	{
		prm->OutSigMonM3.bitsMsk |= (USHORT)(1 << (IoNum+1));
	}
	else
	{
		prm->OutSigMonM3.bitsMsk &= ~(USHORT)(1 << (IoNum+1));
	}

	return;
}
//#endif/* <S1F9> */

/* 2009.01.22 Y.Oka 加減速時間指定機能 現状未サポート */
/****************************************************************************
 加減速指令用 加速度設定(Motion Control) <Rev2.01>
 引数：Tacc[0.1ms], Tspd[ref/s]
****************************************************************************/
VOID MPrmCalcAccTimeRate(MT_PRM *prm, LONG Tspd, LONG Tacc, LONG Tdec)
{
#if 0
	ULONG spd;
	LONG rate,accrate;

	spd = (ULONG)MlibLABS( Tspd );
	if( Tacc == 0 )
	{
		Tacc = 1;
	}
	if( spd < 0x1000000 )
	{
		rate = ( spd << 7 ) / Tacc;
#if (FLOAT_USE==TRUE)
		rate = ( (float)rate * prm->BasePrm.taccRate7Kx );
#else
		rate = MlibMulgain32( rate, prm->BasePrm.taccRate7Kx );
#endif /* FLOAT_USE */
	}
	else
	{
		rate = ( spd + (Tacc>>1) ) / Tacc;
#if (FLOAT_USE==TRUE)
		rate = ( (float)rate * prm->BasePrm.taccRateKx );
#else
		rate = MlibMulgain32( rate, prm->BasePrm.taccRateKx );
#endif /* FLOAT_USE */
	}

	if( rate > 0x3FFFFFFF )
	{
		accrate = 0x3FFFFFFF;
	}
	else if( rate < 1 )
	{
		accrate = 1;
	}
	else
	{
		accrate = rate;
	}

	prm->CmdRate.accrate2 = accrate;
	prm->CmdRate.decrate2 = accrate;
#endif
}

#if 0/* M2プロファイル対応	*/
/****************************************************************************************************/
/*		TFF/TLIM使用選択																			*/
/****************************************************************************************************/
VOID ｍPrmTrefMode(MT_PRM* prm, MTPRM_INI* iniPrm)
{
	UCHAR	profileType = CmdGetProfileType(GetCmdHndl(0));

	if(profileType==M3STD_PROFILE)
	{
		prm->BasePrm.trefMode = TREFMODE_TFF_TLIM;
	}
	else
	{
		prm->BasePrm.trefMode = (UCHAR)iniPrm->trefMode;
	}
	prm->BasePrm.TrefModePos = ((iniPrm->OptEnable&0x00F0) != 0) ? prm->BasePrm.trefMode : 0;
	prm->BasePrm.TLimModePos = ((iniPrm->OptEnable&0x0F00) == 0) ? prm->BasePrm.TrefModePos : 0;

}
#endif

/***************************************** end of file **********************************************/
