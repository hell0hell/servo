/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion PRM ソースファイル											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	加速度指令追加									*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*	<V744>	2010.04.16	K.Ando												*/
/*		<< 移動平均時間がパラメータ値通りに動作しない不具合修正 >>			*/
/*		・不具合No：A0329													*/
/*		・現象：256.0ms - 384.0msの範囲にて256.0msで動作してしまう			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

/* includes */
#include "Basedef.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"

/****************************************************************************
 Private functions
****************************************************************************/
/*  べき乗演算処理 */
static	VOID mPrmCalcPow( LONG data, LONG exp, LONG *num, LONG *deno );

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
	FullcCtrl		;ﾌﾙｸﾛｰｽﾞﾄﾞ制御
	Linear			;リニア使用
	absPG			;ｱﾌﾞｿPG使用
	ot_zstop;		;OT時ｾﾞﾛｸﾗﾝﾌﾟ
	trefMode		;TRef Option
	OptMap			;OPTION指令割付
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	LONG kx, sx;
	LONG ky, sy;
	LONG maxSpd_num, maxSpd_deno;
	LONG ovSpd_num, ovSpd_deno;
	LONG pitch_num, pitch_deno;

/* モーションサイクルセット */
	mtPrm->BasePrm.MotionCycle = iniPrm->Cycle;
//	iniPrm->SpdDiv = MG_DEFAULT_SPDDIV;				/* <S006> *//* S06C /

/* 速度の単位変換定数の計算[ref/通信周期]->[ref/s] */
	sx = 0;
	mtPrm->BasePrm.fspdRefKx = MlibPcalKxgain(1, 4000000, iniPrm->Cycle, &sx, 24);	

/* トルクの単位変換定数の計算[24bit/MAX]->[%] */
	sx = 0;
	mtPrm->BasePrm.trqKx = MlibPcalKxgain(1, iniPrm->MaxTrq, MAX_TRQ24, &sx, 24);	

	mPrmCalcPow(1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno);
	mPrmCalcPow(1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno);
	
/* 速度の単位変換定数の計算[30bit/MAX]->[24bit/OS] */
	/* buf = spd * maxSpd/(ovSpd*64) */
	/* buf = spd * maxSpd * (maxSpd_num/maxSpd_deno) / (ovSpd*(ovSpd_num/ovSpd_deno)*64) */
	sx = sy = 0;
	kx = MlibPcalKxmulx(iniPrm->MaxSpd, maxSpd_num, ovSpd_deno, &sx);
	ky = MlibPcalKxmulx(iniPrm->OverSpd*64, ovSpd_num, maxSpd_deno, &sy);
	mtPrm->BasePrm.maxToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
	
/* 速度の単位変換定数の計算[24bit/OS]->[30bit/MAX] */
	mtPrm->BasePrm.osToMaxKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);

/* 速度の単位変換定数の計算[指令単位/s]->[24bit/OS] */
/* 速度の単位変換定数の計算[24bit/OS]->[指令単位/s] */
	if( iniPrm->LinearMotor != FALSE )
	{
		sx = sy = 0;
		mPrmCalcPow(1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno);
		kx = MlibPcalKxmulx(iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx);
		kx = MlibPcalKxmulx(kx, ovSpd_deno, pitch_num, &sx);
		ky = MlibPcalKxmulx(iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy);  
		ky = MlibPcalKxmulx(ky, pitch_deno, ovSpd_num, &sy);                                     
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);
	}
	else
	{
		sx = sy = 0;
		kx = MlibPcalKxmulx(60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx);
		ky = MlibPcalKxmulx(iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy);
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);
	}

/* 速度の単位変換定数の計算[**.*** ref/制御周期]→[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx(1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx((1 << iniPrm->SpdDiv), iniPrm->Cycle, 1, &sy);
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	
	mtPrm->BasePrm.PosingSpdMax = MlibMulgain32(0x40000000, mtPrm->BasePrm.PosingSpdInvKx);
/* POSING分解能算出 --OverSpeedレベルを小数点位置とする */
	ovSpd_num = MlibMulgain32(MAX_SPD24, mtPrm->BasePrm.osToRefKx);	/* Over Speed [ref/s] */
	for(sx = MG_SPDDIV_RANGE; sx > 0; sx--)
	{
		maxSpd_num = mtPrm->BasePrm.PosingSpdMax >> sx;
		if(ovSpd_num < maxSpd_num)
		{
			break;
		}
	}
	maxSpd_deno = iniPrm->SpdDiv + sx;	/* 小数点ビット位置 */

/* 加減速定数の計算[10000 x ref/s^2]→[**.*** ref/制御周期^2] */
	/* buf = rate * 2^vel_shift * 10000 / (4000000[.25us]/cyc[.25us])^2 */
	/* buf = rate * 2^vel_shift * cyc[.25us]^2 / (40000)^2 */
	sx = 0;
	kx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 40000, &sx, 0);
	mtPrm->BasePrm.PosingRateKx = MlibPcalKxgain(kx, iniPrm->Cycle, 40000, &sx, 24);
	
/* 速度単位変換定数の計算[ref/s]→[**.*** ref/制御周期] */
	/* buf = spd * 2^vel_shift / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * 2^vel_shift * cyc[.25us] / 4000000[.25us] */
	sx = 0;
	mtPrm->BasePrm.PosingSpdKx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 4000000, &sx, 24);
	
/* 速度の単位変換定数の計算[**.*** ref/制御周期]→[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx(1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, 1, &sy);
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	
	mtPrm->BasePrm.mgSpdMax = MlibMulgain32(0x40000000, mtPrm->BasePrm.PosingSpdInvKx);

/* MG_BASE_PRMセット */
	mtPrm->BasePrm.vel_sft_exp = iniPrm->SpdDiv;
	mtPrm->BasePrm.vel_shift = maxSpd_deno;
	mtPrm->BasePrm.vel_shift_s = (MG_VREMBIT_MAX - maxSpd_deno);
	mtPrm->BasePrm.vel_max = 1 << (31 - maxSpd_deno);
	mtPrm->BasePrm.seg_num = iniPrm->SegNum;

	sx = 0;
	kx = MlibPcalKxgain(mtPrm->BasePrm.PosingSpdMax, iniPrm->Cycle, 4000000, &sx, 24);
	mtPrm->BasePrm.seg_max_ref = MlibMulgain32(iniPrm->SegNum, kx);

//	mtPrm->BasePrm.FullcCtrl = iniPrm->FullcCtrl;	/* ﾌﾙｸﾛｰｽﾞﾄﾞ制御 */ /* 未使用	*/
//	mtPrm->BasePrm.Linear = iniPrm->Linear;		/* リニア使用 */ /* 未使用	*/
//	mtPrm->BasePrm.ot_zstop = iniPrm->ot_zstop;/* 未使用	*/

/* ACC_TIME用加速度単位変換定数の計算[(ref/s)/(0.1ms)]→[**.*** ref/制御周期^2] <Rev2.01>*/
	sx = sy = 0;
	kx = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, iniPrm->Cycle, &sx);
	ky = MlibPcalKxmulx(40000, 40000, 1, &sy);
	mtPrm->BasePrm.taccRateKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/* ACC_TIME用加速度単位変換定数の計算[(2^7*ref/s)/(0.1ms)]→[**.*** ref/制御周期^2] <Rev2.01>*/
	sy = 0;
	ky = MlibPcalKxmulx(40000, 40000, (1<<7), &sy);
	mtPrm->BasePrm.taccRate7Kx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/* TFF/TLIM/ACC_TIME使用選択 <Rev2.01>*/
	mtPrm->BasePrm.trefMode = (UCHAR)iniPrm->trefMode;
	mtPrm->BasePrm.TrefModePos = ((iniPrm->OptEnable&0x00F0) != 0) ? iniPrm->trefMode : 0;
	mtPrm->BasePrm.TLimModePos = ((iniPrm->OptEnable&0x0F00) == 0) ? mtPrm->BasePrm.TrefModePos : 0;
	mtPrm->BasePrm.SetCmdAccRate = ((iniPrm->OptEnable&0x0F00) != 0) ? &MPrmCalcAccTimeRate : NULL;

/* 位置情報関連PRMセット */
	mtPrm->PsnPrm.absPG = iniPrm->absPG;		/* ｱﾌﾞｿPG使用 */
	mtPrm->PsnPrm.TdlyIn = iniPrm->dlyTime;
	mtPrm->PsnPrm.SegNum = iniPrm->SvCycle;

/* Option指令割付定義 */
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

static VOID mPrmCalcPow(LONG data, LONG exp, LONG* num, LONG* deno)
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
		exp = (ULONG)MlibLABS(exp);
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


/****************************************************************************
 加速度1設定(MG用)
****************************************************************************/
LONG MPrmSetAccRate1(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate1 = data;
	return 0;
}

/****************************************************************************
 加速度2設定(MG用)
****************************************************************************/
LONG MPrmSetAccRate2(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate2 = data;
	return 0;
}

/****************************************************************************
 加速度切り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetAccAltSpd(MT_PRM* prm, MG_PRM* mgPrm, LONG data)
{
	mgPrm->altspd_a = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}

/****************************************************************************
 減速度1設定(MG用)
****************************************************************************/
LONG MPrmSetDecRate1(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
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
LONG MPrmSetDecRate2(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->decrate2 = data;
	return 0;
}

/****************************************************************************
 減速度り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetDecAltSpd(MT_PRM* prm, MG_PRM* mgPrm, LONG data)
{
	mgPrm->altspd_d = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}

/****************************************************************************
 非常停止用減速度1設定(MG用)
****************************************************************************/
LONG MPrmSetStopDecRate1(MT_PRM* prm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	prm->BasePrm.StopPrm.decrate1 = data;
	return 0;
}

/****************************************************************************
 非常停止用減速度2設定(MG用)
****************************************************************************/
LONG MPrmSetStopDecRate2(MT_PRM* prm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	prm->BasePrm.StopPrm.decrate2 = data;
	return 0;
}

/****************************************************************************
 非常停止用減速度り替え速度設定(MG用)
****************************************************************************/
LONG MPrmSetStopDecAltSpd(MT_PRM* prm, LONG data)
{
	prm->BasePrm.StopPrm.altspd_d = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}


/****************************************************************************
 指数関数加減速バイアス設定(AccFilter)
****************************************************************************/
LONG MPrmSetExpFilBias(MT_PRM* prm, ACF_PRM* acfPrm, LONG bias)
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
LONG MPrmSetExpFilTb(MT_PRM* prm, ACF_PRM* acfPrm, ULONG exptb)
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
LONG MPrmSetMaFilTb(MT_PRM* prm, ACF_PRM* acfPrm, ULONG matb)
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
				if((1<<i) > iTb)
				{
				 	break;
				}
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
 原点復帰方向の設定(Motion Control)
 引数：data=TRUE：負方向、FALSE:正方向
****************************************************************************/
LONG MPrmSetZretDir(MT_PRM* prm, BOOL data)
{
	if(data)
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
****************************************************************************/
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
 加減速指令用 加速度設定(Motion Control) <Rev2.01>
 引数：Tacc[0.1ms], Tspd[ref/s]
****************************************************************************/
VOID MPrmCalcAccTimeRate(MT_PRM* prm, LONG Tspd, LONG Tacc, LONG Tdec)
{
	ULONG spd;
	LONG rate,accrate;

	spd = (ULONG)MlibLABS(Tspd);
	if(Tacc == 0)
	{
		Tacc = 1;
	}
	
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
