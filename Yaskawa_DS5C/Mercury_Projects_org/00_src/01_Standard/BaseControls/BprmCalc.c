/****************************************************************************************************/
/*																									*/
/*																									*/
/*		基本/中間パラメータ計算処理																	*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 基本/中間パラメータ計算処理																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MLib.h"
#include "Bprm.h"

#include <math.h> //平方根に使用

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static LONG BprmCommonMotorParamCal( BPRMDAT *Bprm, MENCPRM *MencP, BPRM_INICFG *CfgPrm, LONG AxisMotType );
static void BprmRotaryMotorParamCal( BPRMDAT *Bprm, MENCPRM *MencP, MENCV *MencV, MENCV *FencV );
static LONG BprmLinearMotorParamCal( BPRMDAT *Bprm, MENCPRM *MencP, MENCV *MencV );
static void HprmCommonMotorParamCal( BPRMDAT *Bprm );
static BOOL BprmAmpMotVoltInfoCal( BPRMDAT *Bprm, DBYTEX styp, DBYTEX typm_e );
static void BprmCalculateUnitTransFacter( BPRMDAT *Bprm, ULONG ratb2l, ULONG rata2l );
static void BprmCalculateSpdDetectUnit( BPRMDAT *Bprm );
static void BprmCalculateCurrentLoadGain( BPRMDAT *Bprm );
static void	BprmCalSpdRefAdGain( BPRMDAT *Bprm, USHORT vrefgn, USHORT gadjv );


/*--------------------------------------------------------------------------------------------------*/
/*		アンプ電圧管理テーブル定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct AMPVTBL {
		UCHAR	InVcd;								/* 入力電圧コード								*/
		UCHAR	OutVcd;								/* 出力電圧コード								*/
		UCHAR	DcinType;							/* 入力電源タイプ(AC:0,DC:1)					*/
		UCHAR	Out2VType;							/* 倍電圧仕様									*/
		LONG	DcVolt;								/* ＤＣ電圧 [0.0001V]							*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	VdetMax;							/* ＤＣ電圧検出値( データ at PnE70[V] )			*/
		USHORT	VdetAdMax;							/* ＡＤ変換結果  ( データ at PnE70[V] )			*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	AmpUnVolt;							/* Ｕｎモニタ表示用アンプ電圧 [V]				*/
		USHORT	MotUnVolt;							/* Ｕｎモニタ表示用モータ電圧 [V]				*/
} AMPVTBL;

/*--------------------------------------------------------------------------------------------------*/
#define		VDTAD100		859				/* AC100V			*//*<V578>*/
#define		VDTAD100F		867				/* AC100V(倍電圧)	*//*<V536>*/
#define		VDTAD200		867				/* AC200V			*/
#define		VDTAD400		859				/* AC400V			*/
#define		VDTAD12			0				/* DC12V			*/
#define		VDTAD24			0				/* DC24V			*/
#define		VDTAD48			798				/* DC48V			*/
#define		VDTAD72			0				/* DC72V			*/

/*--------------------------------------------------------------------------------------------------*/
const	AMPVTBL	AmpVtbl[16] = {
/*  Prm		 Volt				InVcd	OutVcd	DC	2VT		DcVolt   	VdetMax	VdetAdMax	AmpUnVolt	MotUnVolt	*/
/*------------------------------------------------------------------------------------------------------------------*/
/* 0x00 : AC 100V 		*/  {	0x00,	0x00,	0,	 0,		1414214,	255,	VDTAD100,	100,		100,	},
/* 0x01 : AC 200V 		*/  {	0x01,	0x01,	0,	 0,		2828427,	255,	VDTAD200,	200,		200,	},
/* 0x02 : AC 400V		*/  {	0x02,	0x02,	0,	 0,		5656854,	511,	VDTAD400,	400,		400,	},
/* 0x03 : DC  24V		*/  {	0x03,	0x03,	1,	 0,		 240000,	255,	VDTAD24, 	24,	 		24,		},
/* 0x04 : DC  48V		*/  {	0x04,	0x04,	1,	 0,		 480000,	255,	VDTAD48, 	48,	 		48,		},
/* 0x05 : AC 100V(x2)	*/  {	0x00,	0x01,	0,	 1,		2828427,	255,	VDTAD100F,	100,		200,	},
/* 0x06 : DC  72V       */	{	0x06,	0x06,	1,	 0,		 720000,	255,	VDTAD72,	72,			72,		},
/* 0x07 : DC  12V      	*/  {	0x07,	0x07,	1,	 0,		 120000,	255,	VDTAD12,	12,			12,		},
/* 0x08 :         		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x09 :         		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x0A :         		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x0B :         		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x0C :         		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x0D :         		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x0E : 		 		*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	},
/* 0x0F : 				*/  {	0xFF,	0xFF,	0,	 0,		2828427,	255,	0,			200,		200,	}};
/*------------------------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		サーボ制御周期設定																			*/
/*																									*/
/****************************************************************************************************/
void	BprmSetServoCycle( BPRMDAT *Bprm, LONG SACycleNs, LONG SBCycleNs )
{
	Bprm->SvCycleNs		= SBCycleNs;
	Bprm->SvCycleUs		= SBCycleNs/1000;
	Bprm->MPFCycleUs_SC	= Bprm->SvCycleUs;
	Bprm->MPFCycleUs_MR	= Bprm->SvCycleUs;
}


/****************************************************************************************************/
/*																									*/
/*		トルクレベル計算 : Torque [0.01%] --> LvlSel == 0 : NormalizedTorque [2^24/MaxTrq]			*/
/*						   Torque [0.01%] --> LvlSel != 0 : NormalizedTorque [15000/MaxTrq]			*/
/*																									*/
/****************************************************************************************************/
LONG	BprmTorqueLevelCal( BPRMDAT *Brpm, LONG Torque, LONG LvlSel )
{
#if (FLOAT_USE==TRUE)
	float	x;

	if( LvlSel == 0 )
	{
		x = Torque * Brpm->Ktrqlvl;	/* Torque [0.01%] --> [2^24/MaxTrq]		*/
	}
	else
	{
		x = Torque * Brpm->Ktrqlvls;	/* Torque [0.01%] --> [15000/MaxTrq]	*/
	}
#else
	LONG	x;

	if( LvlSel == 0 )
	{
		x = MlibMulgain( Torque, Brpm->Ktrqlvl );	/* Torque [0.01%] --> [2^24/MaxTrq]		*/
	}
	else
	{
		x = MlibMulgain( Torque, Brpm->Ktrqlvls );	/* Torque [0.01%] --> [15000/MaxTrq]	*/
	}
#endif
	return( x );
}


/****************************************************************************************************/
/*																									*/
/*		速度レベル計算 : Speed [0.1r/min, 0.1mm/s] --> NormalizedSpeed [2^24/OvrSpd]				*/
/*							LmtSel == 0 : NorMaxSpd Limit											*/
/*							LmtSel != 0 : NorOvrSpd(2^24) Limit										*/
/*																									*/
/****************************************************************************************************/
KSGAIN	BprmSpeedLevelCal( BPRMDAT *Brpm, KSGAIN Speed, KSGAIN LmtSel )				/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float x;
	x = Speed * Brpm->Kspdlvl;
	if(( LmtSel == 0 ) && ( x > (float)Brpm->NorMaxSpd ))
	{
		x = (float)Brpm->NorMaxSpd;
	}
#else
	LONG	x;
	x = MlibMulgain( Speed, Brpm->Kspdlvl );
	if(( LmtSel == 0 ) && ( x > Brpm->NorMaxSpd ))
	{
		x = Brpm->NorMaxSpd;
	}
#endif
	return( x );
}

/****************************************************************************************************/
/*																									*/
/*		位置レベル計算 : Position [Command Unit] --> Position [Pulse]								*/
/*																									*/
/****************************************************************************************************/
LONG	BprmPositionLevelCal( BPRMDAT *Brpm, LONG Position )
{
#if (FLOAT_USE==TRUE)
	return (Position* Brpm->Kposlvl);
#else
	return MlibMulgain30( Position, Brpm->Kposlvl );
#endif
}


/****************************************************************************************************/
/*																									*/
/*		基本パラメータ計算メイン																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : エンコーダパラメータ(モータパラメータ)等を基に、標準単位系の基本パラメータの			*/
/*			計算を行う。本基本パラメータは、以降の各種パラメータ計算で使用する。					*/
/*																									*/
/*	補 足 : 標準単位系の基本パラメータを使用することにより、モータパラメータの単位系の違い			*/
/*			等に影響されることがなく、各種パラメータ計算の標準化が図れる。							*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG BprmCalcBaseParameters( BPRMDAT *Bprm, MENCPRM *MencP, BPRM_INICFG *CfgPrm, MENCV *MencV, MENCV *FencV )
{
	LONG err;

	/* モータ共通基本パラメータ計算 */
	err = BprmCommonMotorParamCal( Bprm, MencP, CfgPrm, MencV->AxisMotType );

	/* モータ基本パラメータ計算 */
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		err |= BprmLinearMotorParamCal(Bprm, MencP, MencV );
		Bprm->LinearMotorType = TRUE;
	}
	else
	{ /* 回転型 */
		BprmRotaryMotorParamCal( Bprm, MencP, MencV, FencV );
		Bprm->LinearMotorType = FALSE;
	}

	// モータ共通中間パラメータ計算
	HprmCommonMotorParamCal( Bprm );

	/* 基本単位変換係数の計算						*/
	BprmCalculateUnitTransFacter( Bprm, CfgPrm->ratb2l, CfgPrm->rata2l );
	BprmCalculateSpdDetectUnit( Bprm );

	/* 速度指令AD演算ゲイン計算 */
	BprmCalSpdRefAdGain( Bprm, CfgPrm->vrefgn, CfgPrm->gadjv );

	/* 実効トルク(電流)演算ゲインの計算(Un009,Un145) */
	BprmCalculateCurrentLoadGain( Bprm );

	/*--------------------------------------------------------------------------------------------------*/
	/*		モータ速度演算ゲインの計算																	*/
	/*--------------------------------------------------------------------------------------------------*/
	/*																									*/
	/*					Kmotpls * 2^24         1000000													*/
	/*		KmotspdX = ---------------- * -----------------			[2^24/OvrSpd/dMotPos]				*/
	/*						OvrSpd         ScanXcycle [us]												*/
	/*																									*/
	/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->KmotspdA = (Bprm->Kmotspd * 1000000000.0F / KPI_SACYCLENS);
	Bprm->KmotspdB = (Bprm->Kmotspd * 1000000.0F / Bprm->SvCycleUs);
	Bprm->KmotspdC = (Bprm->Kmotspd * 1000000.0F / KPI_SCCYCLEUS);
#else
	Bprm->KmotspdA = MlibScalKskxkx( Bprm->Kmotspd, 1000000000, KPI_SACYCLENS, NULL, 24 );
	Bprm->KmotspdB = MlibScalKskxkx( Bprm->Kmotspd, 1000000, Bprm->SvCycleUs, NULL, 24 );
	Bprm->KmotspdC = MlibScalKskxkx( Bprm->Kmotspd, 1000000, KPI_SCCYCLEUS, NULL, 24 );
#endif /* FLOAT_USE */

	return err;
}



/****************************************************************************************************/
/*																									*/
/*		モータ共通基本パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
static LONG BprmCommonMotorParamCal( BPRMDAT *Bprm, MENCPRM *MencP, BPRM_INICFG *CfgPrm, LONG AxisMotType )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#endif /* FLOAT_USE */
	USHORT	Imax;
	LONG	err;

	err = BPRM_NOALM;
/*--------------------------------------------------------------------------------------------------*/
/*		アンプ＆モータ電圧関連情報の計算 : 入出力電圧コード, ＤＣ電圧[V], etc						*/
/*--------------------------------------------------------------------------------------------------*/
	if(TRUE == BprmAmpMotVoltInfoCal( Bprm, CfgPrm->styp, MencP->typm_e ))
	{
//		ALMSetPramError( AlmMngr, pndef_styp.RegNumber );		/* A.040 : パラメータ設定異常	*/
		err |= BPRM_NOTSUPPORT;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モータタイプコード																			*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->MotTypeCode = MencP->typm_e.b.l;

/*--------------------------------------------------------------------------------------------------*/
/*		指数の計算 : 10^exp																			*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FlibExpCal(MencP->exp_trat_jmot.b.l, &(Bprm->ExpTrq));
	FlibExpCal(MencP->exp_trat_jmot.b.h, &(Bprm->ExpJmot));
	FlibExpCal(MencP->exp_motw_spd.b.l, &(Bprm->ExpMotW));
	FlibExpCal(MencP->exp_motw_spd.b.h, &(Bprm->ExpSpd));
#else
	Bprm->ExpTrq  = MlibExpCal( MencP->exp_trat_jmot.b.l );
	Bprm->ExpJmot = MlibExpCal( MencP->exp_trat_jmot.b.h );
	Bprm->ExpMotW = MlibExpCal( MencP->exp_motw_spd.b.l  );
	Bprm->ExpSpd  = MlibExpCal( MencP->exp_motw_spd.b.h  );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		最大トルク[%]の計算 : Min{アンプ最大電流, モータ最大電流}									*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencP->imax <= CfgPrm->imaxs )
	{
		Imax = (USHORT)MencP->imax;
#if (FLOAT_USE==TRUE)
		Bprm->Kmottrq = 1.0F;
#else
		Bprm->Kmottrq = MlibScalKxgain( 1, 1, 1, NULL, -1 );
#endif /* FLOAT_USE */
	}
	else
	{
		Imax = (USHORT)CfgPrm->imaxs;
#if (FLOAT_USE==TRUE)
		Bprm->Kmottrq = 1.0F * (float)MencP->imax / (float)CfgPrm->imaxs;
#else
		Bprm->Kmottrq = MlibScalKxgain( 1, MencP->imax, CfgPrm->imaxs, NULL, -1 );
#endif /* FLOAT_USE */
	}
	Bprm->PerMaxTrq = (USHORT)((((Imax * 200)/MencP->irat) + 1) >> 1);

/*--------------------------------------------------------------------------------------------------*/
/*		ＯＳレベル[0.01%],[2^24]の計算																*/
/*--------------------------------------------------------------------------------------------------*/
	if( AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		if( MencP->vrat_max.b.h >= CfgPrm->maxvel_l )
		{
			Bprm->MaxVel = (UCHAR)CfgPrm->maxvel_l;
		}
		else
		{
			Bprm->MaxVel = MencP->vrat_max.b.h;
//			ALMSetServoAlarm( AlmMngr, ALM_VELPRM );
			err |= BPRM_VELPRM;	/* A.550 : 最高速度設定異常	*/
		}
	}
	else
	{ /* 回転型 */
		Bprm->MaxVel = MencP->vrat_max.b.h;
	}
	Bprm->PerOvrSpd = CfgPrm->ratmt2_os.b.h * MencP->oslv_pol.b.l;

#if 0//(FLOAT_USE==TRUE)	/*@@TODO正規化値調整中のため無効にしておく*/
	Bprm->NorOvrSpd = 100.0F;
	Bprm->NorMaxSpd = (float)Bprm->NorOvrSpd * 10000.0F / ((float)Bprm->PerOvrSpd+10000.0F);
	Bprm->NorRatSpd = (float)Bprm->NorMaxSpd * (float)MencP->vrat_max.b.l / (float)Bprm->MaxVel;
	Bprm->pNorOvrSpd = 100.0F;
	Bprm->nNorOvrSpd = -100.0F;
#else
	Bprm->NorOvrSpd = 0x01000000;
	Bprm->NorMaxSpd = MlibScalKxgain( Bprm->NorOvrSpd, 10000, (Bprm->PerOvrSpd+10000), NULL, -24 );
	Bprm->NorRatSpd = MlibScalKxgain( Bprm->NorMaxSpd, MencP->vrat_max.b.l, Bprm->MaxVel, NULL, -24 );
	Bprm->NorSuspSpd = MlibScalKxgain( Bprm->NorMaxSpd, 1, 10, NULL, -24 );
	/*<S201>Detecting the motor speed*/
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		最大電流[A]の計算 : Imax [0.1A]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->MaxCur = (float)Imax / 10.0F;
#else
		Bprm->MaxCur = MlibScalKxgain( Imax,  1,  10, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モータ容量[W]の計算 : motw [W]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		Bprm->MotW = Bprm->ExpMotW * (float)MencP->motw;
#else
	Bprm->MotW = MlibScalKskxkx( Bprm->ExpMotW, MencP->motw, 1, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モータ電流[A]の計算 : irat [0.1A]															*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->MotA = (float)MencP->irat / 10.0F;
#else
	Bprm->MotA = MlibScalKxgain( MencP->irat,  1, 10, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		電機子巻線抵抗[ohm]の計算 : motr [0.001ohm]													*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->MotR = (float)MencP->motr / 1000;
#else
	Bprm->MotR = MlibScalKxgain( MencP->motr,  1, 1000, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		インダクタンス[H]の計算 : motl [0.01mH]		for SPM Motor									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->MotLd  = (float)MencP->motl / 100000.0F;
	Bprm->MotLq  = (float)MencP->motl / 100000.0F;
	Bprm->MotLdb = (float)MencP->motl / 100000.0F;
#else
	Bprm->MotLd  = MlibScalKxgain( MencP->motl,  1, 100000, NULL, -1 );
	Bprm->MotLq  = MlibScalKxgain( MencP->motl,  1, 100000, NULL, -1 );
	Bprm->MotLdb = MlibScalKxgain( MencP->motl,  1, 100000, NULL, -1 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		アンプ／モータ容量比＆最大電流比の計算														*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	fw = (float)(CfgPrm->ampw<<16) / Bprm->MotW;
	if(fw > (float)0x01000000)
	{
		Bprm->AmpMotWatRatio = 0x01000000;
	}
	else
	{
		Bprm->AmpMotWatRatio = (LONG)fw;
	}
#else
	Bprm->AmpMotWatRatio  = MlibScalKxkxks( (CfgPrm->ampw<<16), 1, Bprm->MotW, NULL, -24 );
#endif /* FLOAT_USE */
	Bprm->AmpMotImaxRatio = (CfgPrm->imaxs<<16) / MencP->imax;

/*--------------------------------------------------------------------------------------------------*/
/*		PnE39 Bit4 : 電流ループ積分時定数切替スイッチ												*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->CurrentLoopTiSw = ( CfgPrm->dq_sw2 & 0x0010 ) ? TRUE : FALSE;

	return err;
}



/****************************************************************************************************/
/*																									*/
/*		回転形モータ基本パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
static void BprmRotaryMotorParamCal( BPRMDAT *Bprm, MENCPRM *MencP, MENCV *MencV, MENCV *FencV )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx,sx;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モータパルス数[pulse/rev]の計算																*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->MotPulse = MencV->PulseNo;

/*--------------------------------------------------------------------------------------------------*/
/*		モータパルス変換係数[rad/pulse]の計算														*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw=(float)C2PAIE7 / (float)MencV->PulseNo;
	Bprm->Kmotpls = fw / (float)C10POW7;
#else
	kx = MlibScalKxgain( C2PAIE7, 1, MencV->PulseNo, &sx,  0 );
	Bprm->Kmotpls = MlibPcalKxgain( kx, 1,  C10POW7, &sx, -1 );
#endif /* FLOAT_USE */
	Bprm->KmotplsFC = Bprm->Kmotpls;
	if(Bprm->FencUse != FALSE)
	{
#if (FLOAT_USE==TRUE) 
		fw=(float)C2PAIE7 / (float)FencV->PulseNo;
		Bprm->KmotplsFC = fw / (float)C10POW7;
#else
		kx = MlibScalKxgain( C2PAIE7, 1, FencV->PulseNo, &sx,  0 );
		Bprm->KmotplsFC = MlibPcalKxgain( kx, 1,  C10POW7, &sx, -1 );
#endif /* FLOAT_USE */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＦＢパルス数[pulse/rev]の計算																*/
/*--------------------------------------------------------------------------------------------------*/
	if(Bprm->FencUse != FALSE)
	{
		Bprm->FbPulse = FencV->PulseNo;					/* Fully-Closed Control					*/
	}
	else
	{
		Bprm->FbPulse = MencV->PulseNo;				/* Semi-Closed Control					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ＸＦＢパルス数[pulse/rev],[pulse/spitch]の計算												*/
/*--------------------------------------------------------------------------------------------------*/
	if(Bprm->FencUse != FALSE)
	{
		Bprm->XfbPulse1 = Bprm->FbPulse;					/* [pulse/rev]					*/
		Bprm->XfbPulse2 = 1 << MencP->BitnoDpoint.b.l;	/* [pulse/spitch]				*/
	}
	else
	{
		Bprm->XfbPulse1 = Bprm->FbPulse;			/* [pulse/rev]							*/
		Bprm->XfbPulse2 = Bprm->FbPulse;			/* [pulse/rev]							*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＦＢパルス変換係数[rad/pulse]の計算															*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	fw = (float)C2PAIE7 / (float)Bprm->FbPulse;
	Bprm->Kfbpls = fw / (float)C10POW7;
#else
	kx = MlibScalKxgain( C2PAIE7, 1, Bprm->FbPulse, &sx,  0 );
	Bprm->Kfbpls = MlibPcalKxgain( kx, 1,  C10POW7, &sx, -1 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		電気角変換係数 : MotorPosition[rad] --> ElectricAngle[rad]									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		Keangle = PoleNo / 2										PoleNo : [-]					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Keangle = (float)MencP->oslv_pol.b.h/2.0F;
#else
	Bprm->Keangle = MlibScalKxgain( MencP->oslv_pol.b.h, 1, 2, NULL, -1 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		定格速度[rad/s]の計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   2*PAI * RATVEL * 100								RATVEL    : [100r/min] 			*/
/*		RatSpd = ------------------------															*/
/*				           60																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MencP->vrat_max.b.l * (float)C2PAIE7 / 6000000.0F;
	Bprm->RatSpd = fw * Bprm->ExpSpd;
#else
	kx = MlibScalKxgain( MencP->vrat_max.b.l, C2PAIE7, 6000000, &sx,  0 );
	Bprm->RatSpd = MlibPcalKxkskx( kx, Bprm->ExpSpd, 1, &sx, -1 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		最大速度[rad/s]の計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   2*PAI * MAXVEL * 100 * (10000+PerOvrSpd)			MAXVEL    : [100r/min] 			*/
/*		OvrSpd = --------------------------------------------		PerOvrSpd : [0.01%]				*/
/*				               60 * 10000															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)Bprm->MaxVel * (float)C2PAIE7 / (float)C10POW7; //Bprm->MaxVel
	fw = fw * (float)(Bprm->PerOvrSpd + 10000) / 6000.0F;
	Bprm->OvrSpd = fw * Bprm->ExpSpd;
#else
	kx = MlibScalKxgain( Bprm->MaxVel, C2PAIE7, C10POW7, &sx,  0 );
	kx = MlibPcalKxgain( kx, (Bprm->PerOvrSpd + 10000), 6000, &sx,  0 );
	Bprm->OvrSpd = MlibPcalKxkskx( kx, Bprm->ExpSpd, 1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		最大トルク[Nm]の計算																		*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   RATTRQ * MAXTRQ 									RATTRQ : [0.01Nm]   			*/
/*		MaxTrq = -------------------								MAXTRQ : [%]					*/
/*				      100 * 100																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MencP->rattrq * (float)Bprm->PerMaxTrq / 10000.0F;
	Bprm->MaxTrq = fw * Bprm->ExpTrq;
#else
	kx = MlibScalKxgain( MencP->rattrq, Bprm->PerMaxTrq, 10000, &sx,  0 );
	Bprm->MaxTrq = MlibPcalKxkskx( kx, Bprm->ExpTrq, 1, &sx, -1 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		モータイナーシャ[kg*m^2]の計算																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				  JMOT 												JMOT : [10^-6 kgm^2]			*/
/*		Jmot = ------------																			*/
/*				 1000000																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)//check OK
	Bprm->Jmot = Bprm->ExpJmot * (float)MencP->jmot / 1000000.0F;
#else
	Bprm->Jmot = MlibScalKskxkx( Bprm->ExpJmot, MencP->jmot, 1000000, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ＥＭＦ定数[Vrms/rad/s]の計算																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				     60  * EMFCMP 									EMFCMP : [0.1mVrms/r/min]		*/
/*		MotEmf = -------------------																*/
/*				   2*PAI * 10000		  															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)//check OK
	Bprm->MotEmf = (float)MencP->emfcmp * 60000.0F / (float)C2PAIE7;
#else
	Bprm->MotEmf = MlibScalKxgain( MencP->emfcmp, 60000, C2PAIE7, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		オーバパルス速度の計算  [pulse/sec]															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					OvrSpd [rad/sec]																*/
/*		OvrPspd = ----------------------															*/
/*					Kfbpls [rad/pulse]																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->OvrPspd = Bprm->OvrSpd / Bprm->Kfbpls;
	Bprm->MaxPspd = Bprm->OvrPspd * (float)Bprm->NorMaxSpd / (float)Bprm->NorOvrSpd;
#else
	Bprm->OvrPspd = MlibScalKxksks( 1, Bprm->OvrSpd, Bprm->Kfbpls, NULL, -1 );
	Bprm->MaxPspd = MlibScalKskxkx( Bprm->OvrPspd, Bprm->NorMaxSpd, Bprm->NorOvrSpd, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		正規化速度演算係数 : r/min --> NormalizedSpeed												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					(2*PAI) * 2^24  																*/
/*		Knorspd = ------------------------							OvrSpd : [rad/s]				*/
/*					  60 * OvrSpd  																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->Knorspd = 1756906.0F /Bprm->OvrSpd;
#else
	Bprm->Knorspd = MlibScalKxkxks( 1756906, 1, Bprm->OvrSpd, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		正規化速度演算係数２ : r/min --> NormalizedSpeed											*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   		MAXVEL * 100 * (10000+PerOvrSpd)			MAXVEL    : [100r/min] 			*/
/*		NorspdConv = --------------------------------------------	PerOvrSpd : [0.01%]				*/
/*				               			10000														*/
/*																									*/
/*		Iprm.Kmonspd = 1/bprm_ptr->Knorspdを使用して	NormalizedSpeed --> r/min 換算を行うと		*/
/*		Un001表示が桁落ちするため、桁落ちしないように新たに換算ゲインを作成する						*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	fw = (float)Bprm->MaxVel * (float)(Bprm->PerOvrSpd + 10000) / 100.0F;
	Bprm->NorspdConv = fw * Bprm->ExpSpd / (float)0x1000000;
#else
	kx = MlibScalKxgain( Bprm->MaxVel, (Bprm->PerOvrSpd + 10000), 100, &sx, 0 );
	Bprm->NorspdConv = MlibPcalKxkskx( kx, Bprm->ExpSpd, 0x1000000, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		速度換算係数 : 0.1r/min --> pulse/ms								<V053>					*/
/*--------------------------------------------------------------------------------------------------*/
/*		               Bprm.XfbPulse1																*/
/*		KMotSpdConv = ----------------		Bprm.XfbPulse1 : [pulse/rev]							*/
/*		               60 * 1000 * 10																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->KMotSpdConv = (float)Bprm->XfbPulse1 / 60.0F / 10000.0F;
#else
	Bprm->KMotSpdConv = MlibScalKxgain( Bprm->XfbPulse1, 1, 60*10000, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		速度換算係数 : pulse/ScanA --> 2^24/OvrSpd								<V158>				*/
/*--------------------------------------------------------------------------------------------------*/
/*		                Bprm.Kmotpls                10^9		Ts				:[ns]				*/
/*		KConvLoadSpd = ---------------- * 2^24 * -----------	Bprm.Kmotpls	:[m/pulse]			*/
/*		                 Bprm.OvrSpd              Ts(ScanA)		Bprm.OvrSpd		:[m/s]				*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
//	fw = Bprm->Kmotpls * 1000000.0F / (float)Bprm->SvCycleNs;		/* <S04D>:DELL */
	fw = Bprm->KmotplsFC * 1000000.0F / (float)KPI_SACYCLENS;		/* <S04D>:ADD  */
	fw = fw * 1000.0F;
	Bprm->KConvLoadSpd = fw * (float)Bprm->NorOvrSpd / Bprm->OvrSpd;
#else
//	kx = MlibScalKskxkx( Bprm->Kmotpls, 1000000, Bprm->SvCycleNs, &sx, 0 );		/* <S04D>:DELL */
	kx = MlibScalKskxkx( Bprm->KmotplsFC, 1000000, KPI_SACYCLENS, &sx, 0 );		/* <S04D>:ADD  */
	kx = MlibPcalKxgain( kx, 1000, 1, &sx, 0 );
	Bprm->KConvLoadSpd = MlibPcalKxkxks( kx, Bprm->NorOvrSpd, Bprm->OvrSpd, &sx, 24 );
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		リニアモータ基本パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
static LONG BprmLinearMotorParamCal( BPRMDAT *Bprm, MENCPRM *MencP, MENCV *MencV )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx,sx;
#endif /* FLOAT_USE */

	LONG	ScalePulse;
	LONG	TmpFbpls, MaxSpeed;
	LONG	err;

	err = BPRM_NOALM;
/*--------------------------------------------------------------------------------------------------*/
/*		スケールピッチ数の設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->MonScalePitch = MencV->MonScalePitch;
	Bprm->MonScaleExp = MencV->MonScaleExp;
/*--------------------------------------------------------------------------------------------------*/
/*		モータパルス数[pulse/360deg]の計算															*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->MotPulse = MencV->PulseNo;
/*--------------------------------------------------------------------------------------------------*/
/*		モータパルス変換係数[m/pulse]の計算		(Prm.ppitch : [0.1mm/180deg])						*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw=(float)MencP->ppitch * 2.0F / (float)Bprm->MotPulse;
	Bprm->Kmotpls = fw / (float)10000;
#else
	kx = MlibScalKxgain( MencP->ppitch, 2, Bprm->MotPulse, &sx, 0 );
	Bprm->Kmotpls = MlibPcalKxgain( kx, 1,  10000, &sx, -1 );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		ＦＢパルス数[pulse/360deg],ＦＢパルス変換係数[m/pulse]の計算								*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->FbPulse = Bprm->MotPulse;
	Bprm->Kfbpls  = Bprm->Kmotpls;

/*--------------------------------------------------------------------------------------------------*/
/*		ＸＦＢパルス数[p/mm,p/spitch]の計算		(Prm.ppitch : [0.1mm/180deg])						*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->XfbPulse1 = MlibDiv6432( Bprm->MotPulse, 10, (2*MencP->ppitch) );	/* [pulse/mm]		*/
	Bprm->XfbPulse2 = 1 << MencP->bit_dp.b.l;										/* [pulse/spitch]	*/

/*--------------------------------------------------------------------------------------------------*/
/*		電気角変換係数 : MotorPosition[m] --> ElectricAngle[rad]									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		Keangle = (2 * PAI) / (2 * PolePitch / 10000)				PolePitch : [0.1mm/180deg]		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Keangle = (float)C2PAIE7/(float)(2000 * MencP->ppitch);
#else
	Bprm->Keangle = MlibScalKxgain( C2PAIE7, 1, (2000 * MencP->ppitch), NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		定格速度[m/s]の計算																			*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   RAVEL * 100										RATVEL : [100mm/s] 				*/
/*		RatSpd = ---------------																	*/
/*					  1000																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MencP->vrat_max.b.l * 100.0F / 1000.0F;
	Bprm->RatSpd = fw * Bprm->ExpSpd;
#else
	kx = MlibScalKxgain( MencP->vrat_max.b.l, 100, 1000, &sx,  0 );
	Bprm->RatSpd = MlibPcalKxkskx( kx, Bprm->ExpSpd, 1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		最大速度[m/s]の計算																			*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   MAXVEL * 100 * (10000+PerOvrSpd)					MAXVEL    : [100mm/s]			*/
/*		OvrSpd = ------------------------------------				PerOvrSpd : [0.01%]				*/
/*				           1000 * 10000																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)Bprm->MaxVel * (float)(Bprm->PerOvrSpd+10000) / (float)100000;
	Bprm->OvrSpd = fw * Bprm->ExpSpd;
#else
	kx = MlibScalKxgain( Bprm->MaxVel, (Bprm->PerOvrSpd+10000), 100000, &sx,  0 );
	Bprm->OvrSpd = MlibPcalKxkskx( kx, Bprm->ExpSpd, 1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		最大トルク[N]の計算																			*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   RATTRQ * MAXTRQ 									RATTRQ : [N]					*/
/*		MaxTrq = -------------------								MAXTRQ : [%]					*/
/*				         100																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)MencP->rattrq * (float)Bprm->PerMaxTrq / 100.0F;
	Bprm->MaxTrq = fw * Bprm->ExpTrq;
#else
	kx = MlibScalKxgain( MencP->rattrq, Bprm->PerMaxTrq, 100, &sx,  0 );
	Bprm->MaxTrq = MlibPcalKxkskx( kx, Bprm->ExpTrq, 1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モータイナーシャ[kg]の計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   JMOT 											JMOT : [10^-2 kg]				*/
/*		Jmot = ------------																			*/
/*				   100																				*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->Jmot = Bprm->ExpJmot * (float)MencP->jmot / 100.0F;
#else
	Bprm->Jmot = MlibScalKskxkx( Bprm->ExpJmot, MencP->jmot, 100, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ＥＭＦ定数[Vrms/m/s]の計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   EMFCMP * 1000 									EMFCMP : [0.1mVrms/mm/s]		*/
/*		MotEmf = -----------------																	*/
/*				       10000																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->MotEmf = (float)MencP->emfcmp / 10.0F;
#else
	Bprm->MotEmf = MlibScalKxgain( MencP->emfcmp, 1, 10, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		オーバパルス速度の計算  [pulse/sec]															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					OvrSpd [m/sec]																	*/
/*		OvrPspd = --------------------																*/
/*					Kfbpls [m/pulse]																*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->OvrPspd = Bprm->OvrSpd / Bprm->Kfbpls;
	Bprm->MaxPspd = Bprm->OvrPspd * (float)Bprm->NorMaxSpd / (float)Bprm->NorOvrSpd;
#else
	Bprm->OvrPspd = MlibScalKxksks( 1, Bprm->OvrSpd, Bprm->Kfbpls, NULL, -1 );
	Bprm->MaxPspd = MlibScalKskxkx( Bprm->OvrPspd, Bprm->NorMaxSpd, Bprm->NorOvrSpd, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		正規化速度演算係数 : mm/s --> NormalizedSpeed												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*						2^24																		*/
/*		Knorspd = -----------------									OvrSpd : [m/s]					*/
/*					1000 * OvrSpd																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)0x01000000 / 1000.0F;
	Bprm->Knorspd = fw / Bprm->OvrSpd;
#else
	kx = MlibScalKxgain( 0x01000000, 1, 1000, &sx, 0 );
	Bprm->Knorspd = MlibPcalKxkxks( kx, 1, Bprm->OvrSpd, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		正規化速度演算係数２ : mm/s --> NormalizedSpeed												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*						OvrSpd * 1000																*/
/*		NorspdConv = -----------------								OvrSpd : [m/s]					*/
/*							2^24																	*/
/*																									*/
/*		回転形の桁落ち対策で、換算ゲイン名を変更したので、リニア正規化速度演算係数も変更			*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->NorspdConv = Bprm->OvrSpd * 1000.0F / (float)0x1000000;
#else
	Bprm->NorspdConv = MlibScalKskxkx( Bprm->OvrSpd, 1000, 0x1000000, NULL, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		速度換算係数 : 0.1mm/s --> pulse/ms							<V053>							*/
/*--------------------------------------------------------------------------------------------------*/
/*		               ScalePulse * 10																*/
/*		KMotSpdConv = -----------------					ScalePulse : [0.01um/spitch]				*/
/*		                  ScalePitch					ScalePitch : [pulse/spitch]					*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	ScalePulse = 1 << MencP->bit_dp.b.l;
	fw               =(float)ScalePulse * 100.0F / (float)MencV->NormScalePitch;
	Bprm->KMotSpdConv = fw * (float)MencV->NormScaleExp;
#else
	ScalePulse = 1 << MencP->bit_dp.b.l;
	kx               = MlibScalKxgain( ScalePulse, 100, MencV->NormScalePitch, &sx,  0 );
	Bprm->KMotSpdConv = MlibPcalKxgain( kx,         MencV->NormScaleExp,     1, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		サンプル周期最大パルス周波数による最高速度制限処理					<V518>					*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	TmpFbpls = 40000.0F * 10.0F / Bprm->KMotSpdConv;
	TmpFbpls = TmpFbpls * 10000.0F / (float)(10000+Bprm->PerOvrSpd);
	MaxSpeed = (float)(Bprm->MaxVel * 100) * Bprm->ExpSpd;
#else
	TmpFbpls = MlibScalKxkxks( 40000, 10, Bprm->KMotSpdConv, &sx, 0 );
	TmpFbpls = MlibPcalKxgain( TmpFbpls, 10000, (10000+Bprm->PerOvrSpd), &sx, -24 );
	MaxSpeed = MlibScalKxkskx( (Bprm->MaxVel * 100), Bprm->ExpSpd, 1, NULL, -24 );
#endif /* FLOAT_USE */

	if( (TmpFbpls < MaxSpeed ) && (MencV->NormScalePitch != 0) )
	{ /* スケールピッチ = 0 時はアラーム検出しない */
//		ALMSetServoAlarm( AlmMngr, ALM_VELPRM );			/* A.550 : 最高速度設定異常				*/
		err |= BPRM_VELPRM;									/* A.550 : 最高速度設定異常				*/
	}

	return err;
}



/****************************************************************************************************/
/*																									*/
/*		モータ共通中間パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
static void HprmCommonMotorParamCal( BPRMDAT *Bprm )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		位置制御ゲイン中間パラメータの計算						Rotary			Linear				*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				Kfbpls * 2^24 						Kfbpls :	[rad/pulse]		[m/pulse]			*/
/*		Kpx = -----------------						OvrSpd :	[rad/s]			[m/s]				*/
/*				   OvrSpd																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Kpx = (float)0x1000000 * Bprm->Kfbpls / Bprm->OvrSpd; 
#else
	Bprm->Kpx = MlibScalKxksks( 0x1000000, Bprm->Kfbpls, Bprm->OvrSpd, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		速度制御ゲイン中間パラメータの計算						Rotary			Linear				*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				2*PAI * OvrSpd * Jmot 				OvrSpd :	[rad/s]			[m/s]				*/
/*		Kvx = -------------------------				Jmot   :	[kg*m^2]		[kg]				*/
/*				      MaxTrq						MaxTrq :	[Nm]			[N]					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	fw = Bprm->OvrSpd * (float)C2PAIE7 / (float)C10POW7;
	Bprm->Kvx = fw * Bprm->Jmot / Bprm->MaxTrq;
#else
	kx = MlibScalKskxkx( Bprm->OvrSpd, C2PAIE7, C10POW7, &sx,  0 );
	kx = MlibPcalKxksks( kx,   Bprm->Jmot,  Bprm->MaxTrq, &sx, -1 );
	Bprm->Kvx = kx;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モータ速度演算ゲイン中間パラメータの計算				Rotary			Linear				*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				    Kmotpls * 2^24 					Kmotpls :	[rad/pulse]		[m/pulse]			*/
/*		Kmotspd = ------------------				OvrSpd  :	[rad/s]			[m/s]				*/
/*				        OvrSpd																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->Kmotspd = (float)0x1000000 * Bprm->Kmotpls / Bprm->OvrSpd;
	Bprm->KmotspdFC = (float)0x1000000 * Bprm->KmotplsFC / Bprm->OvrSpd;
#else
	Bprm->Kmotspd = MlibScalKxksks( 0x1000000, Bprm->Kmotpls, Bprm->OvrSpd, &sx, -1 );
	Bprm->KmotspdFC = MlibScalKxksks( 0x1000000, Bprm->KmotplsFC, Bprm->OvrSpd, &sx, -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		位置速度ゲイン中間パラメータの計算						Rotary			Linear				*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				   	      Jmot * 2^24				Kfbpls :	[rad/pulse]		[m/pulse]			*/
/*		Kpvx = Kfbpls * ---------------				Jmot   :	[kg*m^2]		[kg]				*/
/*				    	    MaxTrq					MaxTrq :	[Nm]			[N]					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	fw = Bprm->Kfbpls * (float)0x1000000;
	Bprm->Kpvx = fw * Bprm->Jmot / Bprm->MaxTrq;
#else
	kx = MlibScalKskxkx( Bprm->Kfbpls, 0x1000000, 1,  &sx,  0 );
	kx = MlibPcalKxksks( kx, Bprm->Jmot, Bprm->MaxTrq, &sx, -1 );
	Bprm->Kpvx = kx;
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		アンプ＆モータ電圧管理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		PnE11.2	: アンプ電圧コード																	*/
/*		PnF00.2 : モータ電圧コード																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		PnE70	: Ｐ－Ｎ電圧検出レベル( 最大入力電圧[V] )											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		VdetAdMaxはBspのcfgファイルによってカスタマイズが可能										*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*																									*/
/*		アンプ＆モータ電圧関連情報の計算															*/
/*																									*/
/****************************************************************************************************/
static BOOL BprmAmpMotVoltInfoCal( BPRMDAT *Bprm, DBYTEX styp, DBYTEX typm_e )
{
	UCHAR	Idx;
	BOOL	err;

	Idx = (styp.b.h & 0x0F);
	err = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータチェック																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( AmpVtbl[Idx].InVcd == 0xFF )
	{
		Idx = 0;		/* AmpVolt = AC 100V					*/
		err = TRUE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モータ電圧コードの計算																		*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->MotVcode  = typm_e.b.h & 0x0F;
	Bprm->MotUnVolt = AmpVtbl[Bprm->MotVcode].MotUnVolt;

/*--------------------------------------------------------------------------------------------------*/
/*		アンプ入出力電圧コードの計算																*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->AmpInVcode  = AmpVtbl[Idx].InVcd;				/* サーボアンプ入力電圧コード		*/
	Bprm->AmpOutVcode = AmpVtbl[Idx].OutVcd;			/* サーボアンプ出力電圧コード		*/
	Bprm->AmpUnVolt   = AmpVtbl[Idx].AmpUnVolt;			/* Ｕｎモニタ表示用アンプ電圧 [V]	*/

/*--------------------------------------------------------------------------------------------------*/
/*		アンプ入力電圧タイプの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->AmpDcinType = AmpVtbl[Idx].DcinType;

/*--------------------------------------------------------------------------------------------------*/
/*		アンプ倍電圧仕様の設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->AmpOut2VType = (AmpVtbl[Idx].Out2VType == 0)? FALSE : TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		アンプＤＣ電圧[V]の計算																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Vdc = (float)AmpVtbl[Idx].DcVolt / 10000.0F;
#else
	Bprm->Vdc = MlibScalKxgain( AmpVtbl[Idx].DcVolt,  1,  10000,  NULL,  -1 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		アンプ主回路検出最大値の設定																*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->VdetMax = AmpVtbl[Idx].VdetMax;

/*--------------------------------------------------------------------------------------------------*/
/*		アンプ主回路検出ＡＤ最大値の設定															*/
/*--------------------------------------------------------------------------------------------------*/
	Bprm->VdetAdMax = AmpVtbl[Idx].VdetAdMax;

/*--------------------------------------------------------------------------------------------------*/
/*		主回路電圧補正値の下限値設定						0x170002c9 = 0.85×10^-4				*/
/*--------------------------------------------------------------------------------------------------*/
	if(Bprm->MotUnVolt == 24)
	{/*	DC24なら 0x180002c9 = 0.5×0.85×10^-4 */
#if (FLOAT_USE==TRUE) 
		Bprm->CompVolt = (float)AmpVtbl[Idx].DcVolt * 0.0000425F;
#else
		Bprm->CompVolt = MlibMulgain( AmpVtbl[Idx].DcVolt, 0x180002c9 );
#endif /* FLOAT_USE */
	}
	else
	{
#if (FLOAT_USE==TRUE) 
		Bprm->CompVolt = (float)AmpVtbl[Idx].DcVolt * 0.000085F;
#else
		Bprm->CompVolt = MlibMulgain( AmpVtbl[Idx].DcVolt, 0x170002c9 );
#endif /* FLOAT_USE */
	}

	return err;
}

/****************************************************************************************************/
/*																									*/
/*		基本単位変換係数の計算																		*/
/*																									*/
/****************************************************************************************************/
static void BprmCalculateUnitTransFacter( BPRMDAT *Bprm, ULONG ratb2l, ULONG rata2l )
{
#if (FLOAT_USE==TRUE)
	float		fw, plsno;
#else
	LONG		kx, sx, plsno;
#endif /* FLOAT_USE */

	KSGAIN		Kmotspd;
	KSGAIN		Knorspd;
	KSGAIN		NorspdConv;
#if (FLOAT_USE==FALSE)
	sx			= 0;
#endif /* FLOAT_USE */
	plsno		= (Bprm->FencUse != FALSE) ? Bprm->KmotspdFC: Bprm->Kmotspd;
	Kmotspd		= Bprm->Kmotspd;
	Knorspd		= Bprm->Knorspd;
	NorspdConv	= Bprm->NorspdConv;

/*--------------------------------------------------------------------------------------------------*/
/*		位置レベル演算係数   : Command Pulse     --> Feedback Pulse									*/
/*		速度レベル演算係数   : 0.1r/min, 0.1mm/s --> NormalizedSpeed  [2^24/OvrSpd]					*/
/*		トルクレベル演算係数 : 0.01%             --> NormalizedTorque [2^24/MaxTrq]					*/
/*		トルクレベル演算係数 : 0.01%             --> NormalizedTorque [15000/MaxTrq]				*/
/*--------------------------------------------------------------------------------------------------*/

#if (FLOAT_USE==TRUE) 
	Bprm->Kposlvl = (float)ratb2l/(float)rata2l;
	Bprm->Kspdlvl = Knorspd/10.0F;
	Bprm->Ktrqlvl = (float)0x01000000 / (100.0F * (float)Bprm->PerMaxTrq);
	Bprm->Ktrqlvls = 15000.0F / (100.0F * (float)Bprm->PerMaxTrq);

#else
	Bprm->Kposlvl  = MlibScalKxgain( 1, ratb2l, rata2l, NULL, 24 );
	Bprm->Kspdlvl  = MlibScalKskxkx( Knorspd, 1, 10, NULL, 24 );
	Bprm->Ktrqlvl  = MlibScalKxgain( 1, 0x01000000, (100*Bprm->PerMaxTrq), NULL, 24 );
	Bprm->Ktrqlvls = MlibScalKxgain( 1, 15000,      (100*Bprm->PerMaxTrq), NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		正規化速度演算係数 : r/min, mm/s --> NormalizedSpeed										*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		Bprm.Knorspd : r/min --> NormalizedSpeed	Rotary Motor									*/
/*		Bprm.Knorspd : mm/s  --> NormalizedSpeed	Linear Motor  									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Kmonorspd = Knorspd;
#else
	Bprm->Kmonorspd = MlibScalKskxkx( Knorspd, 1, 1, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		正規化トルク指令演算係数 : %RatedTorque --> NormalizedTorque								*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*						   2^24																		*/
/*		Iprm.Knortrq = -------------						Bprm.PerMaxTrq : [%]					*/
/*						 PerMaxTrq																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Knortrq = (float)0x01000000/(float)Bprm->PerMaxTrq;
#else
	Bprm->Knortrq = MlibScalKxgain( 1, 0x01000000, Bprm->PerMaxTrq, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用速度演算係数 : NormalizedSpeed --> r/min, mm/s										*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		Bprm.Knorspd : r/min --> NormalizedSpeed	Rotary Motor									*/
/*		Bprm.Knorspd : mm/s  --> NormalizedSpeed	Linear Motor  									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->Kmonspd = NorspdConv; 
#else
	Bprm->Kmonspd = MlibScalKskxkx( NorspdConv, 1, 1, NULL, 48 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用トルク指令演算係数 : NormalizedTorque --> %RatedTorque								*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*						 PerMaxTrq																	*/
/*		Iprm.Kmontrq = -------------						Bprm.PerMaxTrq : [%]					*/
/*						   2^24																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	Bprm->Kmontrq = (float)Bprm->PerMaxTrq/(float)0x01000000;
#else
	Bprm->Kmontrq = MlibScalKxgain( Bprm->PerMaxTrq, 1, 0x01000000, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用ＦＢパルス速度演算ゲインの計算 (Un000)												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*							 Hprm.Kmotspd * 1000000  												*/
/*		Iprm.Kmonpfbkspd = ---------------------------					Xcycle : 64ms				*/
/*							 Bprm.Knorspd * Xcycleus 												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = Kmotspd/Knorspd;
	Bprm->Kmonpfbkspd = fw * 1000000.0F / ((float)SVCDEF_UNSPD_SCMEANTMMS * 1000.0F);
#else
	kx = MlibScalKxksks( 1, Kmotspd, Knorspd, &sx,  0 );
	Bprm->Kmonpfbkspd = MlibPcalKxgain( kx, 1000000, (SVCDEF_UNSPD_SCMEANTMMS*1000), &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用指令パルス速度演算ゲインの計算 (Un007)												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*							 Egear.b * Hprm.Kmotspd * 1000000  										*/
/*		Iprm.Kmonpcmdspd = -------------------------------------		Xcycle : 64ms				*/
/*							 Egear.a * Bprm.Knorspd * Xcycleus 										*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)ratb2l / (float)rata2l;
	fw = fw * (float)plsno / Knorspd;
	Bprm->Kmonpcmdspd = fw * 1000000.0F / ((float)SVCDEF_UNSPD_SCMEANTMMS*1000.0F);
#else
	kx = MlibScalKxgain( 1,  ratb2l, rata2l, &sx,  0 );
	kx = MlibPcalKxksks( kx, plsno, Knorspd, &sx,  0 );
	Bprm->Kmonpcmdspd = MlibPcalKxgain( kx, 1000000, (SVCDEF_UNSPD_SCMEANTMMS*1000), &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用指令パルス速度演算ゲインの計算														*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*							 	Egear.b * Hprm.Kmotspd * 1000000  									*/
/*		Iprm.Kmonpcmdspd2ms = ----------------------------------------		Xcycle : 2ms			*/
/*							 	Egear.a * Bprm.Knorspd * Xcycleus 									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = (float)ratb2l / (float)rata2l;
	fw = fw * (float)plsno / Knorspd;
	Bprm->Kmonpcmdspd2ms = fw * 1000000.0F / (float)KPI_SCCYCLEUS;
#else
	kx = MlibScalKxgain( 1,  ratb2l, rata2l, &sx,  0 );
	kx = MlibPcalKxksks( kx, plsno, Knorspd, &sx,  0 );
	Bprm->Kmonpcmdspd2ms = MlibPcalKxgain( kx, 1000000, KPI_SCCYCLEUS, &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用パルス速度演算ゲインの計算 : Pulse/Scan --> NormalizedSpeed							*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*							 Hprm.Kpx * 1000000  													*/
/*		Iprm.Knorpulsspd = ----------------------				Xcycle : KPI_SBCYCLEUS				*/
/*							      Xcycleus 															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Knorpulsspd = Bprm->Kpx * 1000000.0F / (float)Bprm->SvCycleUs;
#else
	Bprm->Knorpulsspd = MlibScalKskxkx( Bprm->Kpx, 1000000, Bprm->SvCycleUs, NULL,  24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用電流指令演算係数 : NormalizedCurrent --> %RatedCurrent								*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*							PerMaxTrq																*/
/*		Iprm.Kmontrq = -----------------						Bprm.PerMaxTrq : [%]				*/
/*							15000																	*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	Bprm->Kmoncrnt = (float)Bprm->PerMaxTrq / 15000.0F;
#else
	Bprm->Kmoncrnt = MlibScalKxgain( Bprm->PerMaxTrq, 1, 15000, NULL, 24 );
#endif /* FLOAT_USE */

}

/****************************************************************************************************/
/*																									*/
/*		速度検出分解能計算[0.1r/min/pulse],[0.1mm/s/pulse]											*/
/*																									*/
/*		概要：１速度制御周期あたりのエンコーダ１パルスの速度換算値									*/
/*				Bprm.XfbPulse1:[pulse/rev],[p/mm]													*/
/*																									*/
/****************************************************************************************************/
static void BprmCalculateSpdDetectUnit( BPRMDAT *Bprm )
{
	LONG	sx;
#if (FLOAT_USE==TRUE)
	Bprm->SpdDetectUnit = (float)KPI_SACOUNTMS * 600000.0F / (float)Bprm->MotPulse;

#else
	sx = 0;
	Bprm->SpdDetectUnit = MlibScalKxgain( KPI_SACOUNTMS , 600000, Bprm->MotPulse, &sx, -24 );

#endif /* FLOAT_USE */
}

/****************************************************************************************************/
/*																									*/
/*		実効トルク(電流)演算ゲインの計算(Un009,Un145)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*					    128 * PerMaxTrq   			128 * 128 * PerMaxTrq							*/
/*		Iprm->Kcurload = -------------------------- = -----------------------------------			*/
/*					  15000 * √SCANC_MS(10000)	   15000 * √(16384*SCANC_MS(10000))				*/
/*																									*/
/****************************************************************************************************/
static void BprmCalculateCurrentLoadGain( BPRMDAT *Bprm )
{
	
#if (FLOAT_USE==TRUE) 
	float fw;
#else
	LONG		kx, sx;
#endif

#if (FLOAT_USE==TRUE) //check OK
	fw = sqrtf(	16384.0F * (float)KPI_SCANC_MS( 10000 ));
	fw = (128.0F * 128.0F) / fw;
	fw = fw * (float)Bprm->PerMaxTrq / 15000.0F;
	Bprm->Kcurload = fw;
#else
	sx = 0;
	kx = MlibSqrtu32( 16384 * KPI_SCANC_MS( 10000 ) );
	kx = MlibScalKxgain( 1,  (128 * 128),     kx,    &sx,  0 );
	kx = MlibPcalKxgain( kx, Bprm->PerMaxTrq, 15000, &sx, 24 );
	Bprm->Kcurload = kx;
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		速度指令ＡＤ演算ゲイン計算																	*/
/*																									*/
/*		<1>24bit AD --> 16bit AD																	*/
/****************************************************************************************************/
/*																									*/
/*					  4.096 * RatSpd * 2^24 * 100 * (Gadjv + 65536)         VrefgnD					*/
/*		Kvrefad = ------------------------------------------------------ * ---------				*/
/*					  2^15 * HWgain * OvrSpd * Vrefgn * 65536 * 2^7          10000					*/
/*																									*/
/*						16384 * RatSpd * (Gadjv + 65536)			VrefgnD							*/
/*				= ---------------------------------------------- * ---------						*/
/*					  10 * HWgain * OvrSpd * Vrefgn * 65536		     10000							*/
/*																									*/
/*																									*/
/*		RatSpd  : 定格速度				[rad/s], [m/s]												*/
/*		OvrSpd  : オーバスピード		[rad/s], [m/s]												*/
/*		Vrefgn  : 速度指令入力ゲイン	[0.01V/定格速度]											*/
/*		VrefgnD	: 同上ディレーティング	[0.01%]	(Pn309)												*/
/*		Gadjv   : 速度指令ゲイン調		[1/65536]													*/
/*		HWgain  : ＨＷ回路ゲイン(HWgain=4/15)														*/
/*		2^7     : 入力倍率補正(ゼロ調データが2^7倍されているため)									*/
/*																									*/
/*		参 照  : 詳細は、速度指令ＡＤゲイン調(Tstcmdif.c:TciAdjustVrefGain())を参照					*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void	BprmCalSpdRefAdGain( BPRMDAT *Bprm, USHORT vrefgn, USHORT gadjv )
{
#if (FLOAT_USE==TRUE)
	float fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		機能有無チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
/* ★暫定★ */
//	if( SVFSEL_VREFAD == 0 ){ return;}					/* Mask Function							*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/*<S008>*/

/*--------------------------------------------------------------------------------------------------*/
/*		速度指令ＡＤ演算ゲイン計算																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE) 
	fw = 16384.0F * 15.0F / 40.0F;
	fw = fw * Bprm->RatSpd / Bprm->OvrSpd;
	fw = fw / (float)vrefgn;
	fw = fw * ((float)(SHORT)gadjv + 65536.0F) / 65536.0F;
	Bprm->Kvrefad = fw;

#else
	kx = MlibScalKxgain( 16384, 15, 4*10, &sx, 0 );
	kx = MlibPcalKxksks( kx, Bprm->RatSpd, Bprm->OvrSpd, &sx,  0 );
	kx = MlibPcalKxgain( kx, 1, vrefgn, &sx, 0 );
	kx = MlibPcalKxgain( kx, ((SHORT)gadjv + 65536), 65536, &sx, 24 );
	Bprm->Kvrefad = kx;

#endif /* FLOAT_USE */
#endif		//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/*<S008>*/

	return;

}

/***************************************** end of file **********************************************/
