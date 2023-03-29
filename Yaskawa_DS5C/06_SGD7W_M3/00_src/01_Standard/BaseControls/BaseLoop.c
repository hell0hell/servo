/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanAMain.c : ScanAメイン処理定義															*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ScanAメイン処理																			*/
/*																									*/
/*		ScanBより取得した位置、速度、トルク指令からASIC(μプログラム)へ渡す電流指令を算出する。		*/
/*		磁極検出未完時には磁極検出メイン処理を実行する。											*/
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
#include "BaseControls.h"
#include "SerialEncIf.h"
#include "PosManager.h"
#include "ResVibCtrl.h"
#include "GainChange.h"
#include "PowerManager.h"
#include "MotorPoleFind.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#include "AlarmManager.h"
#include "ExControls.h"
#include "ExCtrlPrmCalc.h"
#include "RippleComp.h"		/* <S1FD> */


static	void	LpxSvPositionManager( BASE_LOOPCTRLS *BaseLoops, POS_MNG_HNDL *PosManager );
static	void	LpxPosSpdFeedbackCal( BASE_LOOPCTRLS *BaseLoops );
static void	ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err );
extern void ApxMotorLessTestModeMechaModel( LONG TrqRef, BOOL BeComplete, MOTLESSTST *MotorLessTest );


#define NRFMAX	16014615
/****************************************************************************************************/
/*																									*/
/*		位置制御演算																				*//*<S00A>*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	BasePositionControlA(
			BASE_LOOPCTRLS *BaseLoops,
			KSGAIN	PerClrCmd,			/*<S00A>*/
			KSGAIN	IcvClrCmd,			/*<S00A>*/
			KSGAIN	Kp,					/*<S00A>*/
			LONG	PerClrEdge /* @@ */ )
{
	KSGAIN		SpdRefx;		/* 速度指令(一時保管用) *//*<S00A>*/
	POSCTRL		*PosCtrl;		/* 位置制御用変数 */
	POS_MNG_HNDL *PosMngV;

	PosCtrl = &BaseLoops->PosCtrl;
	PosMngV = &BaseLoops->BaseCtrls->PosManager;
/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差クリア処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( PerClrCmd )
	{
		PosCtrl->V.PerIvar64[0] = 0;
		PosCtrl->V.PerIvar64[1] = 0;
		PosCtrl->V.LastPacOut   = 0;
		return( 0 );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御積分クリア処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( IcvClrCmd )
	{
		PosCtrl->V.PerIvar64[0] = 0;
		PosCtrl->V.PerIvar64[1] = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：位置偏差計算																<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/*<V739>SafetyModuleｺｰﾄﾞ改善*/
#if	0 //( CCSW_SFTID == SFTID_EXIST && CCSW_HWID == HWID_SGDV )									  /*<V717>Mod:Start*/
	if (!((BoutV.ZctrlMode.b.zm == CTRLMODE_DSTOP) && (CoutSom.St.PosErrLt == 1)))
	{ PosMngV.PosErr = FlibPerrcalx( dPosRef, dPosFbk, PosMngV.Per64 ); }						  /*<V730>Mod*/
#else
	PosMngV->var.PosErr = FlibPerrcalx( BaseLoops->dPosRefi, BaseLoops->dPosFbki, PosMngV->var.Per64 ); 
#endif	

#else

/*<V739>SafetyModuleｺｰﾄﾞ改善*/
#if	0 //( CCSW_SFTID == SFTID_EXIST && CCSW_HWID == HWID_SGDV )									  /*<V717>Mod:Start*/
	if (!((BoutV.ZctrlMode.b.zm == CTRLMODE_DSTOP) && (CoutSom.St.PosErrLt == 1)))
	{ PosMngV.PosErr = MlibPerrcalx( dPosRef, dPosFbk, PosMngV.Per64 ); }						  /*<V730>Mod*/
#else
	PosMngV->var.PosErr = MlibPerrcalx( BaseLoops->dPosRefi, BaseLoops->dPosFbki, PosMngV->var.Per64 );
#endif																							  /*<V717>Mod:End  */
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：比例制御演算																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	PosCtrl->V.PacOut = PosMngV->var.PosErr * Kp;
#else
	PosCtrl->V.PacOut = MlibMulgain( PosMngV->var.PosErr, Kp );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：積分制御演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseLoops->BaseCtrls)->BeComplete == FALSE )
	{ /* ベースブロック */
		PosCtrl->V.PerIvar64[0] = 0;					/* SGDSでは、ベースブロック時、				*/
		PosCtrl->V.PerIvar64[1] = 0;					/* 位置積分だけクリアしている				*/
		PosCtrl->V.IacOut       = 0;
	}
	else if( (BaseLoops->BaseCtrls)->CtrlCmdPrm.Kpi == 0 )
	{ /* 積分ゲインゼロ時(処理時間対策) */
#if (FLOAT_USE==TRUE)
		PosCtrl->V.IacOut = ((PosCtrl->V.PerIvar64[1] + 1) / 2.0f);
#else
		PosCtrl->V.IacOut = ((PosCtrl->V.PerIvar64[1] + 1) >> 1);
#endif /* FLOAT_USE */
	}
	else
	{ /* ベースイネーブル時(積分ゲインゼロ以外) */
#if (FLOAT_USE==TRUE)
		PosCtrl->V.IacOut = FlibIntegral( PosCtrl->V.PacOut,
										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Kpi,
										  PosCtrl->V.PerIvar64 ); 
#else
		PosCtrl->V.IacOut = MlibIntegral( PosCtrl->V.PacOut,
										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Kpi,
										  PosCtrl->V.PerIvar64 );
#endif /* FLOAT_USE */
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：微分制御演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosCtrl->P.Kpd == 0 )							/* 微分ゲインゼロ時(処理時間対策)			*/
	{
		PosCtrl->V.DacOut = 0;							/* 微分制御出力 = ゼロ						*/
	}
	else
	{
#if (FLOAT_USE==TRUE)
		PosCtrl->V.DacOut = (PosCtrl->V.PacOut - PosCtrl->V.LastPacOut) * PosCtrl->P.Kpd;
#else
		PosCtrl->V.DacOut = MlibMulgain( PosCtrl->V.PacOut - PosCtrl->V.LastPacOut, PosCtrl->P.Kpd );
#endif /* FLOAT_USE */
	}
	PosCtrl->V.LastPacOut = PosCtrl->V.PacOut;
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御：ＰＩＤ制御出力																	*/
/*--------------------------------------------------------------------------------------------------*/
	SpdRefx = PosCtrl->V.PacOut + PosCtrl->V.IacOut + PosCtrl->V.DacOut;

/*--------------------------------------------------------------------------------------------------*/
/*		速度バイアス加算演算																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* ★SGDV未対応★ */
//	SpdRefx = VelocityBias( SpdRefx, PosCtrl->V.PosErr );	/* 速度バイアス加算演算					*/
	if( SpdRefx == 0 )
	{
		if( PosMngV->var.PosErr > 0 )		{ SpdRefx =  1;}
		else if( PosMngV->var.PosErr < 0 )	{ SpdRefx = -1;}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		位置制御出力リミット																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	PosCtrl->V.SpdRef = FlibLimitul( SpdRefx, NRFMAX, -NRFMAX );
#else
	PosCtrl->V.SpdRef = MlibLimitul( SpdRefx, NRFMAX, -NRFMAX );
#endif /* FLOAT_USE */

	return( PosCtrl->V.SpdRef );

}

/****************************************************************************************************/
/*																									*/
/*		サーボオン時位置偏差ありの速度リミット処理													*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	SvonSpeedLimit( BOOL BaseEnable, BASE_LOOPCTRLS *BaseLoops, KSGAIN SpdRefi )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	AbsSpdRefi;
	float	SpdRefx;
#else
	LONG	AbsSpdRefi;
	LONG	SpdRefx;
#endif /* FLOAT_USE */
	SPDCTRL *hSpdCtrl;

	hSpdCtrl = &BaseLoops->SpdCtrl;
	SpdRefx = SpdRefi;

	if(((BaseEnable == TRUE) && (hSpdCtrl->V.LastSvonFlag == FALSE))
			|| hSpdCtrl->V.SvonSpdLmtFlag )
	{
		AbsSpdRefi = SpdRefi;
		if( AbsSpdRefi < 0 )
		{
			AbsSpdRefi = -AbsSpdRefi;
		}

		if( AbsSpdRefi > (BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtLevel )
		{ /* 初回サーボオン時速度指令がPn529より大の場合 */
			/* サーボON時速度制限開始 */
			hSpdCtrl->V.SvonSpdLmtFlag = TRUE;
			if( SpdRefi >= 0 )
			{
				SpdRefx = (BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtLevel;
			}
			else
			{
				SpdRefx = -(BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtLevel;
			}
		}
		else if( AbsSpdRefi < (BaseLoops->BaseCtrls)->CtrlCmdPrm.SvonSpdLmtValue )
		{ /* 速度指令がPn529/10より小の場合 */
			/* サーボON時速度制限解除 */
			hSpdCtrl->V.SvonSpdLmtFlag = FALSE;
		}
	}
	hSpdCtrl->V.LastSvonFlag = BaseEnable;
	return( SpdRefx );
}


/****************************************************************************************************/
/*																									*/
/*		ゼロクランプ制御演算処理																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : スキャンＣからのゼロクランプ制御モード指令により行われる処理。							*/
/*			位置指令＝０による位置ループを組んでモータをクランプする処理。							*/
/*			フルクローズド制御の場合もセミクローズド換算で速度指令を計算する。						*/
/*																									*/
/*	注 意 : モータ位置制御であり、フルクローズド制御はサポートしていないので注意すること。			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	ZeroPositionControl( BASE_LOOPCTRLS *BaseLoops, KSGAIN dPosFbk )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
float	SpdRef;
#else
LONG	SpdRef;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ゼロクランプ制御																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位置偏差演算 */
	BaseLoops->ZxCtrl.V.PosError -= dPosFbk;
#if (FLOAT_USE==TRUE)
	SpdRef = ( (float)BaseLoops->ZxCtrl.V.PosError * BaseLoops->GselGains->ZcKp );/* 位置制御演算(P制御)		*/
#else
	SpdRef = MlibMulgain( BaseLoops->ZxCtrl.V.PosError, BaseLoops->GselGains->ZcKp );	/* 位置制御演算(P制御)		*/
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	if( SpdRef == 0 )
	{ /* 速度指令ゼロ時の補正 */
		if( BaseLoops->ZxCtrl.V.PosError > 0 )
		{
			SpdRef =  1;
		}
		else if( BaseLoops->ZxCtrl.V.PosError < 0 )
		{
			SpdRef = -1;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( SpdRef > BaseLoops->ZxCtrl.P.SpdLimit )
	{ /* 速度指令上限リミット */
		SpdRef =  BaseLoops->ZxCtrl.P.SpdLimit;
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( SpdRef < -BaseLoops->ZxCtrl.P.SpdLimit )
	{ /* 速度指令下限リミット */
		SpdRef = -BaseLoops->ZxCtrl.P.SpdLimit;
	}
/*--------------------------------------------------------------------------------------------------*/
	return( SpdRef );

}

/****************************************************************************************************/
/*																									*/
/*		調整レス制御有効時の速度指令位相進み補償演算												*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	LpxSpdRefPhaseComp(SPDCTRL *SpdCtrl, KSGAIN SpdRefx, KSGAIN SpdRefPhFil, KSGAIN SpdRefPhFilGn)/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	SpdRefComp;
#else
	LONG	SpdRefComp;
#endif /* FLOAT_USE */

	/* 制御切替え時の初期化処理		*//*<V392>*/
	if(SpdCtrl->V.SpdRefFilClrCmd == TRUE)
	{
		/* 速度指令位相進み補償	*/
		SpdCtrl->V.SpdRefPhLpfo = SpdRefx;
		SpdCtrl->V.SpdRefPhHpfTmp = SpdRefx;

		SpdCtrl->V.SpdRefFilClrCmd = FALSE;
	}
	/* 速度指令	*/
#if (FLOAT_USE==TRUE)
	SpdCtrl->V.SpdRefPhLpfo = FlibLpfilter1(SpdRefx, SpdRefPhFil, SpdCtrl->V.SpdRefPhLpfo);
	SpdCtrl->V.SpdRefPhHpfo = FlibHpfilter1(SpdRefx, SpdRefPhFil, &SpdCtrl->V.SpdRefPhHpfTmp);
#else
	SpdCtrl->V.SpdRefPhLpfo = MlibLpfilter1(SpdRefx, SpdRefPhFil, SpdCtrl->V.SpdRefPhLpfo);
	SpdCtrl->V.SpdRefPhHpfo = MlibHpfilter1(SpdRefx, SpdRefPhFil, &SpdCtrl->V.SpdRefPhHpfTmp);
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	SpdRefComp = SpdCtrl->V.SpdRefPhLpfo + ((LONG)SpdCtrl->V.SpdRefPhHpfo * SpdRefPhFilGn);
#else
	SpdRefComp = SpdCtrl->V.SpdRefPhLpfo + MlibMulgain(SpdCtrl->V.SpdRefPhHpfo, SpdRefPhFilGn);
#endif /* FLOAT_USE */

	return(SpdRefComp);
}

/****************************************************************************************************/
/*																									*/
/*		調整レス制御有効時の速度ＦＦ位相進み補償演算												*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	LpxSpdFFPhaseComp(SPDCTRL *SpdCtrl, KSGAIN SpdFFC, KSGAIN P_SpdFFC, KSGAIN SpdFFPhFil, KSGAIN SpdFFPhFilGn)/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	SpdFFCComp;
#else
	LONG	SpdFFCComp;
#endif /* FLOAT_USE */

	/* 制御切替え時の初期化処理		*//*<V392>*/
	if(SpdCtrl->V.SpdFFFilClrCmd == TRUE)
	{
		SpdCtrl->V.SpdFFFilClrCmd = FALSE;

		/* 速度ＦＦ位相進み補償	*/
		SpdCtrl->V.SpdFFPhLpfo = P_SpdFFC;
		SpdCtrl->V.SpdFFPhHpfTmp = P_SpdFFC;
	}
	/* 速度ＦＦ	*/
#if (FLOAT_USE==TRUE)
	SpdCtrl->V.SpdFFPhLpfo = FlibLpfilter1(SpdFFC, SpdFFPhFil, SpdCtrl->V.SpdFFPhLpfo);
	SpdCtrl->V.SpdFFPhHpfo = FlibHpfilter1(SpdFFC, SpdFFPhFil, &SpdCtrl->V.SpdFFPhHpfTmp);
#else	
	SpdCtrl->V.SpdFFPhLpfo = MlibLpfilter1(SpdFFC, SpdFFPhFil, SpdCtrl->V.SpdFFPhLpfo);
	SpdCtrl->V.SpdFFPhHpfo = MlibHpfilter1(SpdFFC, SpdFFPhFil, &SpdCtrl->V.SpdFFPhHpfTmp);
#endif /* FLOAT_USE */

#if (FLOAT_USE==TRUE)
	SpdFFCComp = SpdCtrl->V.SpdFFPhLpfo + (SpdCtrl->V.SpdFFPhHpfo * SpdFFPhFilGn);
#else
	SpdFFCComp = SpdCtrl->V.SpdFFPhLpfo + MlibMulgain(SpdCtrl->V.SpdFFPhHpfo, SpdFFPhFilGn);
#endif /* FLOAT_USE */

	return(SpdFFCComp);
}


/****************************************************************************************************/
/*																									*/
/*		速度制御演算																				*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	BaseSpeedControl( BASE_LOOPCTRLS *BaseLoops,
							  KSGAIN SpdFFC,			/* 速度FF補償値									*/
							  KSGAIN SpdFBC,			/* 速度FB補償値									*/
							  BOOL BaseEnable )			/* 位置FB増分値（ゼロクランプ処理用）			*/
{
#if (FLOAT_USE==TRUE)
	float	SpdRefx;
	float	d_SpdRefx, d_SpdFFC;
	float	SpdErrP;
	float	SpdErrI;
	float	TrqRef0;		/*<V263>*/
#else
	LONG	SpdRefx;
	LONG	d_SpdRefx, d_SpdFFC;
	LONG	SpdErrP;
	LONG	SpdErrI;
	LONG	TrqRef0;		/*<V263>*/
#endif /* FLOAT_USE */

	BOOL	neri_calc_on;
	POS_MNG_HNDL *PosMngV;

	PosMngV = &BaseLoops->BaseCtrls->PosManager;
/*--------------------------------------------------------------------------------------------------*/
/*		速度指令演算																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* サーボ制御指令クリア要求チェック */
	/* ★位置偏差クリア対応必要★ */
	if( BaseLoops->BaseCtrls->BBRefClrReq )
	{ /* (BaseBlock && OTではない) */
		/* 速度指令クリア */
		BaseLoops->SpdRefo = 0;
	}
	else if( (BaseLoops->CtrlMode == CTRLMODE_SPD) || (BaseLoops->CtrlMode == CTRLMODE_JOG) )
	{
		/* ソフトスタート演算 *//* ScanBからソフトスタートを移動する必要あり @@CHECK */
//		BaseLoops->SpdRefo = BaseSoftStartSpdRef( SpdRef, BaseLoops->SpdRefo );
		BaseLoops->SpdRefo = BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo;
	}
	else if( BaseLoops->CtrlMode == CTRLMODE_ZSRCH ) /* <S050> */
	{
		BaseLoops->SpdRefo = BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo;
	}
	else
	{
		/* 位置制御時はソフトスタートなし */
		BaseLoops->SpdRefo = BaseLoops->SpdRefi;
	}

	BaseLoops->SpdRefSum = MlibSatAdd32(BaseLoops->SpdRefo, BaseLoops->SpdFFCx);

/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロック時の処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseEnable == FALSE )
	{
		MlibResetLongMemory( &(BaseLoops->SpdCtrl.V), sizeof(BaseLoops->SpdCtrl.V)/4 );
		BaseLoops->ZxCtrl.V.PosError = 0;
		return( 0 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ速停止, ゼロクランプ処理																*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->ZCtrlMode == CTRLMODE_ZSTOP )		/* ゼロ速停止								*/
	{
		SpdRefx = 0;
		SpdFFC = 0;
		BaseLoops->TrqFFCx = 0;
	}
/* <S1F5> Start */
	/*----------------------------------------------------------------------------------------------*/
	else if( BaseLoops->ZCtrlMode == CTRLMODE_LDSTOP )	/* 直線減速度による減速停止					*/
	{
		SpdRefx = BaseLoops->BaseCtrls->CtrlCmdMngr.SpdLdstpRefo;
	}
/* <S1F5> End */
	/*----------------------------------------------------------------------------------------------*/
	else if( BaseLoops->ZCtrlMode == CTRLMODE_ZCLMP )	/* ゼロクランプ								*/
	{
		/* ゼロクランプ制御演算 */
		SpdRefx = ZeroPositionControl( BaseLoops, BaseLoops->dMotPos );
		SpdFFC = 0;
		BaseLoops->TrqFFCx = 0;
	}
	/*----------------------------------------------------------------------------------------------*/
	else												/* 通常制御									*/
	{
		SpdRefx = BaseLoops->SpdRefo;
		/* 2012.10.05 Y.Oka SGDVにはない。必要？ */
//		BaseLoops->ZxCtrl.V.PosError = 0;
	}
	/*----------------------------------------------------------------------------------------------*/
	if( BaseLoops->SpdCtrl.V.SvonSpdLmtFlag )			/* サーボON時速度リミット					*/
	{
		SpdFFC = 0;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	位相進みフィルタ処理	<V108>																*/
	/*----------------------------------------------------------------------------------------------*/
	d_SpdRefx = LpxSpdRefPhaseComp( &BaseLoops->SpdCtrl, SpdRefx,
									BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFil,
									BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFilGn );
	if( BaseLoops->CtrlMode == CTRLMODE_SPD )
	{
		d_SpdFFC = LpxSpdFFPhaseComp( &BaseLoops->SpdCtrl, SpdFFC, 0,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFil,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFilGn );
	}
	else
	{
		d_SpdFFC = LpxSpdFFPhaseComp( &BaseLoops->SpdCtrl, SpdFFC, SpdFFC,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFil,
									  BaseLoops->BaseCtrls->CtrlCmdPrm.TLPrm.SpdRefPhFilGn );
	}
//	if( ((BaseLoops->BaseCtrls)->TuneLessCtrl.var.TuneLessAct) )									  /* <S1C7> */
	if( (TuneLessGetTuneLessSts(&((BaseLoops->BaseCtrls)->TuneLessCtrl)))			/* TuneLessAct	*/
	/*	&& !(TuneLessGetTuneLessModeEx(&((BaseLoops->BaseCtrls)->TuneLessCtrl))) */ )	/* TuningLessEx	*/
	{	/* 調整レス有効 && 互換モード調整レス */
		SpdRefx = d_SpdRefx;
		SpdFFC = d_SpdFFC;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		モードスイッチ演算	<S0BA>																	*/
/*--------------------------------------------------------------------------------------------------*/
	ModeSwitch(	&BaseLoops->BaseCtrls->ModeSwData,
				BaseLoops->TrqRefo,
				(SpdRefx + SpdFFC),
				PosMngV->var.PosErr );

/*--------------------------------------------------------------------------------------------------*/
/*		速度制御演算																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BaseLoops->ZCtrlMode != CTRLMODE_ZSTOP) && (BaseLoops->ZCtrlMode != CTRLMODE_ZCLMP) )
	{
		BaseLoops->SpdRefSum = MlibSatAdd32(SpdRefx, SpdFFC);
	}

	if( ( (SpdRefx - BaseLoops->SpdObsFbki >= 0) && (BaseLoops->SpdCtrl.V.TrqRef < 0) )
			|| ( (SpdRefx - BaseLoops->SpdObsFbki < 0) && (BaseLoops->SpdCtrl.V.TrqRef > 0) ) )
	{ /* 速度偏差とトルク指令の符号が異なる */
		neri_calc_on = TRUE;
	}
	else
	{ /* 速度偏差とトルク指令の符号が同一(トルク指令 = 0含む) */
		neri_calc_on = FALSE;
	}

	if( (BaseLoops->CmdCtrlBit & ENBPCTRL_BIT)
			|| ((BaseLoops->BaseCtrls)->ModeSwData.var.ModeSWFlag
				&& (!BaseLoops->BaseCtrls->CtrlCmdPrm.IPSpdControl)) )
	{ /* 比例制御時 */
//		BaseLoops->uCycInputs.MREG_BITDAT |= BITDAT_PCONACTFLG;
		BaseLoops->PConActFlg = TRUE; /* <S064> */
#if (FLOAT_USE==TRUE)
		BaseLoops->SpdCtrl.V.SpdFbFilo = FlibLpfilter1(	BaseLoops->SpdObsFbki + SpdFBC, 
															BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil, 
															BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = SpdRefx - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = SpdErrP * BaseLoops->GselGains->Kv ;
#else
		BaseLoops->SpdCtrl.V.SpdFbFilo = MlibLpfilter1( BaseLoops->SpdObsFbki + SpdFBC,
														BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil,
														BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = SpdRefx - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = MlibMulgain( SpdErrP, BaseLoops->GselGains->Kv );
#endif /* FLOAT_USE */
	/*------------------------------------------------------------------------------------------*/
		if( !BaseLoops->BaseCtrls->CtrlCmdPrm.SpdIctrlKeep )
		{ /* 速度制御積分保持ではない場合 */
			/* 時定数を持って積分クリア */
#if (FLOAT_USE==TRUE)
			BaseLoops->SpdCtrl.V.IacOut = FlibLpfilter1( 0.0f,
														 BaseLoops->SpdCtrl.P.KVintegFil,
														 BaseLoops->SpdCtrl.V.IacOut );
			BaseLoops->SpdCtrl.V.Ivar64[1] = BaseLoops->SpdCtrl.V.IacOut *2.0f;
#else
			BaseLoops->SpdCtrl.V.IacOut = MlibLpfilter1( 0,
														 BaseLoops->SpdCtrl.P.KVintegFil,
														 BaseLoops->SpdCtrl.V.IacOut );
			BaseLoops->SpdCtrl.V.Ivar64[1] = BaseLoops->SpdCtrl.V.IacOut << 1;
#endif /* FLOAT_USE */
			BaseLoops->SpdCtrl.V.Ivar64[0] = 0;
		}
	}
	else
	{ /* 比例積分制御時 */
//		BaseLoops->uCycInputs.MREG_BITDAT &= BITDAT_PCONACTFLG;
		BaseLoops->PConActFlg = FALSE; /* <S064> */
#if (FLOAT_USE==TRUE)
		BaseLoops->SpdCtrl.V.SpdFbFilo = FlibLpfilter1(	BaseLoops->SpdObsFbki + SpdFBC, 
															BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil, 
															BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = ( SpdRefx * BaseLoops->BaseCtrls->CtrlCmdPrm.PI_rate ) - 
													BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = ( SpdErrP * BaseLoops->GselGains->Kv );
#else
		BaseLoops->SpdCtrl.V.SpdFbFilo = MlibLpfilter1( BaseLoops->SpdObsFbki + SpdFBC,
														BaseLoops->BaseCtrls->CtrlCmdPrm.KVfbFil,
														BaseLoops->SpdCtrl.V.SpdFbFilo );
		SpdErrP = MlibMulgainNolim( SpdRefx, BaseLoops->BaseCtrls->CtrlCmdPrm.PI_rate);
		SpdErrP = SpdErrP - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
		BaseLoops->SpdCtrl.V.PacOut = MlibMulgain( SpdErrP, BaseLoops->GselGains->Kv );
#endif /* FLOAT_USE */
	/*------------------------------------------------------------------------------------------*/
		SpdErrI = SpdRefx - BaseLoops->SpdCtrl.V.SpdFbFilo + SpdFFC;
	/*------------------------------------------------------------------------------------------*/
		/* トルク制限中ではない、または速度偏差とトルク指令の符号が異なる場合に積分する */
		if ( (FALSE == BaseLoops->TrqClamp) || (neri_calc_on == TRUE) )
		{
//#if (FLOAT_USE==TRUE)
//			BaseLoops->SpdCtrl.V.IacOut = FlibIntegral( SpdErrI, BaseLoops->GselGains->Kvi, &BaseLoops->SpdCtrl.V.Ivar64 );
//#else
			BaseLoops->SpdCtrl.V.IacOut = MlibIntegral( SpdErrI, BaseLoops->GselGains->Kvi, BaseLoops->SpdCtrl.V.Ivar64 );
//#endif /* FLOAT_USE */
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダレス制御										@@@ENCLESS@@@						*/
/*--------------------------------------------------------------------------------------------------*/
//	KpiMencLessSpdCtrl( &BaseLoops->SpdCtrl );				/* MencLess制御：モータ速度制御補正		*/

/*--------------------------------------------------------------------------------------------------*/
/*		速度制御出力																				*/
/*--------------------------------------------------------------------------------------------------*/
	TrqRef0 = BaseLoops->SpdCtrl.V.PacOut + BaseLoops->SpdCtrl.V.IacOut;
#if (FLOAT_USE==TRUE)
	BaseLoops->SpdCtrl.V.TrqRef = ( TrqRef0 * BaseLoops->GselGains->Kv2 );
#else
	BaseLoops->SpdCtrl.V.TrqRef = MlibMulgain29( TrqRef0, BaseLoops->GselGains->Kv2 );
#endif
	return( BaseLoops->SpdCtrl.V.TrqRef );
}


/****************************************************************************************************/
/*																									*/
/*		モードスイッチ演算																			*/
/*																									*/
/****************************************************************************************************/
static void ModeSwitch( MODE_SW *ModeSwData, LONG trq_ref, LONG spd_ref, LONG pos_err )
{
	LONG	ModeSWFlag;
	LONG	wk;

	/* ローカル変数の初期化 */
	ModeSWFlag = FALSE;
	wk = 0;

	switch( ModeSwData->conf.ModeSWSel )
	{
	case MODESW_TRQREF: /* モードスイッチ トルク指令 */
		wk = trq_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWTrqLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_SPDREF: /* モードスイッチ 速度指令 */
		wk = spd_ref;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWSpdLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_MOTACC: /* モードスイッチ 加速度 */
		wk = ModeSwData->var.Acceleration;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWAccLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_POSERR: /* モードスイッチ 偏差パルス */
		wk = pos_err;
		if( wk < 0 )
		{
			wk = -wk;
		}
		if( wk >= ModeSwData->conf.MSWErrLevel )
		{
			ModeSWFlag = TRUE;
		}
		break;

	case MODESW_NOTUSE: /* モードスイッチ 不使用 */
	default:
		break;
	}
	ModeSwData->var.ModeSWFlag = ModeSWFlag;
}



/****************************************************************************************************/
/*																									*/
/*		トルクフィルタ for 位置ループ & 速度ループ演算												*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	BaseTorqueFilterForPSCtrl( BASE_LOOPCTRLS *BaseLoops, KSGAIN TrqRefi,
											KSGAIN TrqFBC, KSGAIN TrqFFC, KSGAIN BaseEnable )/*<S00A>*//* <S0B8> */
{
	TRQFIL *TrqFil;
#if (FLOAT_USE==TRUE)
	float	TrqRefx;
	float	DisTrqRef1 = 0;			/* トルク外乱指令入力1 */
	float	DisTrqRef2 = 0;			/* トルク外乱指令入力2 */
#else
	LONG	TrqRefx;
	LONG	DisTrqRef1 = 0;			/* トルク外乱指令入力1 */
	LONG	DisTrqRef2 = 0;			/* トルク外乱指令入力2 */
#endif /* FLOAT_USE */

	TrqFil = &BaseLoops->TrqFil;
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転時のトルク外乱指令入力処理														*/
/*--------------------------------------------------------------------------------------------------*/
	BaseLoops->DisTrqRef3 = 0;						/* トルク外乱指令入力3							*/
#if (FLOAT_USE==TRUE)
//	switch( TrqFil->V.TrqInputPos ) /* <S0B7> */
	switch( BaseLoops->BaseCtrls->TblDrive.var.DisTrqInTiming )										  /* <S0B7> */
	{
	case 0x01:										/* トルク指令フィルタ前							*/
		DisTrqRef1 = (BaseLoops->BaseCtrls->TblDrive.var.TrqRef);
		break;
	case 0x02:										/* ローパスフィルタ後、ノッチフィルタ前			*/
		DisTrqRef2 = (BaseLoops->BaseCtrls->TblDrive.var.TrqRef);
		break;
	case 0x03:										/* トルク指令フィルタ後							*/
		BaseLoops->DisTrqRef3 = (BaseLoops->BaseCtrls->TblDrive.var.TrqRef);
		break;
	case 0x00:										/* なし											*/
	default :
		break;
	}
#else
	switch( BaseLoops->BaseCtrls->TblDrive.var.DisTrqInTiming )										  /* <S0B7> */
	{
	case 0x01:										/* トルク指令フィルタ前							*/
		DisTrqRef1 = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
		break;
	case 0x02:										/* ローパスフィルタ後、ノッチフィルタ前			*/
		DisTrqRef2 = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
		break;
	case 0x03:										/* トルク指令フィルタ後							*/
		BaseLoops->DisTrqRef3 = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
		break;
	case 0x00:										/* なし											*/
	default :
		break;
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロック時の処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseEnable == FALSE )
	{
		TrqFil->V.TrqFilClrCmd = FALSE;				/* 積分初期化コマンドクリア						*/
		TrqFil->V.FilOut = 0;
		TrqFil->V.FilOut3 = 0;
		TrqFil->V.LpFil2[0] = 0;
		TrqFil->V.LpFil2[1] = 0;
		TrqFil->V.LpFil2[2] = 0;
		TrqFil->V.LpFil2[3] = 0;
		return( 0 );
	}
#if (FLOAT_USE==TRUE)
	TrqRefx = FlibLimitul( (TrqRefi + DisTrqRef1), (float)0x01000000, (float)-0x01000000 );
#else
	TrqRefx = MlibLimitul( (TrqRefi + DisTrqRef1), 0x01000000, -0x01000000 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		1次ローパスフィルタ積分初期化処理(拡張制御ON/OFF時に使用する)								*/
/*--------------------------------------------------------------------------------------------------*/
	if(TrqFil->V.TrqFilClrCmd == TRUE)			/* 積分初期化コマンドあり							*/
	{
		TrqFil->V.FilOut = TrqRefx;				/* トルクフィルタ積分値を現入力値で初期化			*/
		TrqFil->V.TrqFilClrCmd = FALSE;			/* 積分初期化コマンドクリア							*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		1次ローパスフィルタ処理																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	TrqFil->V.FilOut = FlibLpfilter1( TrqRefx, BaseLoops->GselGains->Klpf, TrqFil->V.FilOut );
#else
	TrqFil->V.FilOut = MlibLpfilter1( TrqRefx, BaseLoops->GselGains->Klpf, TrqFil->V.FilOut );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		トルクＦＢ補償処理																			*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	TrqRefx = FlibLimitul( (TrqFil->V.FilOut + TrqFFC - TrqFBC + DisTrqRef2), 0x01000000, -0x01000000 );
#else
	TrqRefx = MlibLimitul( (TrqFil->V.FilOut + TrqFFC - TrqFBC + DisTrqRef2), 0x01000000, -0x01000000 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		1次ローパスフィルタ処理(トルク補償後)														*/
/*			注意：トルク補償後のフィルタは調整レスのロバスト性を悪化させるので、フィルタは			*/
/* 				  最小限にすること。																*/
/*--------------------------------------------------------------------------------------------------*/
		if(BaseLoops->BaseCtrls->CtrlCmdPrm.LpassFil3 == TRUE)
		{ /* トルク補償後トルクフィルタ有効 */
			if(BaseLoops->BaseCtrls->TuneLessCtrl.conf.TuningLessUse )
			{ /* 調整レス有効 */
#if (FLOAT_USE==TRUE)
				TrqFil->V.FilOut3 = FlibLpfilter1( TrqRefx,
												   (BaseLoops->BaseCtrls)->CtrlCmdPrm.TLPrm.Klpf3,
												   TrqFil->V.FilOut3 );
#else
				TrqFil->V.FilOut3 = MlibLpfilter1( TrqRefx,
												   (BaseLoops->BaseCtrls)->CtrlCmdPrm.TLPrm.Klpf3,
												   TrqFil->V.FilOut3 );
#endif /* FLOAT_USE */
				TrqRefx = TrqFil->V.FilOut3;
			}
			else
			{
				TrqFil->V.FilOut3 = 0;
			}
		}
		else
		{
			TrqFil->V.FilOut3 = 0;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		２次ローパスフィルタ処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		if(FALSE == BaseLoops->BaseCtrls->CtrlCmdPrm.LpassFil2)
		{
			TrqFil->V.LpFil2[0] = 0;
			TrqFil->V.LpFil2[1] = 0;
			TrqFil->V.LpFil2[2] = 0;
			TrqFil->V.LpFil2[3] = 0;
		}
		else
		{
#if (FLOAT_USE==TRUE)
			TrqRefx = FlibLowPassfilter2( TrqRefx,
										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Klpf2,
//										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Klpf2, 	/* S036 */
										  TrqFil->P.Klpf2, 						/* S036 */	/* TODO:計算結果未確認 */
										  TrqFil->V.LpFil2 );
#else
			TrqRefx = MlibLowPassfilter2( TrqRefx,
//										  (BaseLoops->BaseCtrls)->CtrlCmdPrm.Klpf2, 	/* S036 */
										  TrqFil->P.Klpf2, 						/* S036 */
										  TrqFil->V.LpFil2 );
#endif /* FLOAT_USE */
		}
/*--------------------------------------------------------------------------------------------------*/
//		BoutV.TrcCompTrqRef = TrqRefx;		/* 外乱入力前トルク指令									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		return( FlibLimitul( TrqRefx, (float)0x01000000, (float)-0x01000000 ) );
#else
		return( MlibLimitul( TrqRefx, 0x01000000, -0x01000000 ) );
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		トルク制御指令演算処理		<S0B8>															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : トルク制御モード時のトルク指令入力処理とトルク指令フィルタ処理を行う。					*/
/*																									*/
/*																									*/
/*	補足 1) トルクフィルタ演算用変数は、基本サーボ制御トルクフィルタ処理と共用し、					*/
/*			制御モード切替え時のトルク指令の連続性を確保する。										*/
/*		 2) ＯＴ発生時は、ＯＴシーケンス用(ScanC)に、一次ローパスフィルタ演算を行う。				*/
/*		 3) ゼロ速停止およびゼロクランプ制御は、基本サーボ制御処理に任せる。						*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	BaseTorqueControl( BASE_LOOPCTRLS *BaseLoops, BOOL BeComplete )
{
	LONG	TrqRefx;
//	LONG	TRefAd;//<S0C7>

/****************************************************************************************************/
/*		テーブル運転処理																			*/
/****************************************************************************************************/
	if( BaseLoops->BaseCtrls->TblDrive.var.TblCmd != TBLCMD_NOCMD )
	{
	/*----------------------------------------------------------------------------------------------*/
		if( BaseLoops->ZCtrlMode != CTRLMODE_ZNONE )		/* ゼロ速制御／ゼロクランプ制御時		*/
		{
			/* 速度制御演算 */
			BaseLoops->TrqRefi = BaseSpeedControl( BaseLoops,
												   BaseLoops->SpdFFCx,
												   BaseLoops->SpdFBCx,
												   BeComplete );
			TrqRefx = BaseLoops->TrqRefi;				/* トルク指令フィルタ処理を実行する			*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else if( BeComplete == FALSE )
		{ /* ベースブロック時 */
			BaseLoops->TrqRefo = 0;						/* Output = Input (No Filter)				*/
			BaseLoops->TrqFil.V.FilOut = 0;
			BaseLoops->TrqFil.V.LpFil2[0] = 0;
			BaseLoops->TrqFil.V.LpFil2[1] = 0;
			BaseLoops->TrqFil.V.LpFil2[2] = 0;
			BaseLoops->TrqFil.V.LpFil2[3] = 0;
			return	BaseLoops->TrqRefo;					/* トルク指令フィルタ処理を実行しない		*/
		}
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			/* トルク指令入力 = テーブル運転トルク指令 */
			BaseLoops->TrqRefi = BaseLoops->BaseCtrls->TblDrive.var.TrqRef;
			TrqRefx = BaseLoops->TrqRefi;				/* トルク指令フィルタ処理を実行する			*/
		}
	}

/****************************************************************************************************/
/*		テーブル運転処理以外																		*/
/****************************************************************************************************/
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		トルク指令入力処理																			*/
/*--------------------------------------------------------------------------------------------------*/
#if	SVFSEL_ADC_USE == 1
		/* アナログ指令 */
//		BaseLoops->TrqRefi = MlibLpfilter1( BaseLoops->TRefAd,
//											BaseLoops->TrqFil.A.Ktrflpf,
//											BaseLoops->TrqRefi );
		if( BaseLoops->MencV->PhaseReady == FALSE )						/* <S21C> */
		{/* 磁極検出中 */
			BaseLoops->TrqRefi = (BaseLoops->BaseCtrls)->CtrlCmdMngr.TrqRefo;
		}
		else
		{/* 位相検出完了 */
			BaseLoops->TrqRefi = MlibLpfilter1( BaseLoops->TRefAd,
												BaseLoops->TrqFil.A.Ktrflpf,
												BaseLoops->TrqRefi );
		}																/* <S21C> */
		
#else
		/* 通信指令 */
		BaseLoops->TrqRefi = (BaseLoops->BaseCtrls)->CtrlCmdMngr.TrqRefo;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		速度制限トルク減算処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		TrqRefx = BaseLoops->TrqRefi - (BaseLoops->BaseCtrls)->CtrlCmdMngr.OverTrq;

/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転時以外のゼロ速停止＆ゼロクランプ制御時の処理									*/
/*--------------------------------------------------------------------------------------------------*/
/*		コメント追加：下記変更によりトルク制御中のゼロ速停止はないので下記処理は不要				*/
/*			BKｼｰｹﾝｽにてPn506≠0でもﾄﾙｸ制御中はｾﾞﾛ速停止せず、制御継続に変更している					*/
/*			ﾄﾙｸ制御中にｾﾞﾛ速ｱﾗｰﾑ発生してもｾﾞﾛ速停止せず、Pn001.0の停止方法に従う					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BaseLoops->ZCtrlMode != CTRLMODE_ZNONE )
		{
			/* 前回トルク指令値をそのまま返す */
			return	BaseLoops->TrqRefo;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロック時の処理																		*/
/*--------------------------------------------------------------------------------------------------*/
		else if( BeComplete == FALSE )
		{
			BaseLoops->TrqRefo = 0;
			if ( BaseLoops->BaseCtrls->BBRefClrReq )
			{
				BaseLoops->TrqRefi = 0;
			}
			BaseLoops->TrqFil.V.FilOut = 0;
			BaseLoops->TrqFil.V.LpFil2[0] = 0;
			BaseLoops->TrqFil.V.LpFil2[1] = 0;
			BaseLoops->TrqFil.V.LpFil2[2] = 0;
			BaseLoops->TrqFil.V.LpFil2[3] = 0;
			return	BaseLoops->TrqRefo;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		フィルタなし処理		<V181>																*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->TrqFil.V.TrqFilDisable )
	{
		BaseLoops->TrqRefo   = MlibLimitul( TrqRefx, 0x01000000, -0x01000000 );
		BaseLoops->TrqFil.V.FilOut = TrqRefx;
		BaseLoops->TrqFil.V.LpFil2[0] = 0;
		BaseLoops->TrqFil.V.LpFil2[1] = 0;
		BaseLoops->TrqFil.V.LpFil2[2] = 0;
		BaseLoops->TrqFil.V.LpFil2[3] = 0;
		return	BaseLoops->TrqRefo;;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		１次ローパスフィルタ積分初期化処理(拡張制御ON/OFF時に使用する)								*/
/*--------------------------------------------------------------------------------------------------*/
/*			トルク制御時は LpxInitTrqCtrlModeVar( )で初期化処理を行うため、							*/
/*			ここでは積分初期化コマンドのクリアのみ行う。											*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->TrqFil.V.TrqFilClrCmd == TRUE )
	{ /* 積分初期化コマンドあり */
		/* 積分初期化コマンドクリア */
		BaseLoops->TrqFil.V.TrqFilClrCmd = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		１次ローパスフィルタ処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	BaseLoops->TrqFil.V.FilOut = MlibLpfilter1( TrqRefx,
												BaseLoops->TrqFil.A.Klpf,
												BaseLoops->TrqFil.V.FilOut );

	TrqRefx = MlibLimitul( BaseLoops->TrqFil.V.FilOut, 0x01000000, -0x01000000 );

/*--------------------------------------------------------------------------------------------------*/
/*		２次ローパスフィルタ処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( BaseLoops->BaseCtrls->CtrlCmdPrm.LpassFil2 == FALSE )
	{
		BaseLoops->TrqFil.V.LpFil2[0] = 0;
		BaseLoops->TrqFil.V.LpFil2[1] = 0;
		BaseLoops->TrqFil.V.LpFil2[2] = 0;
		BaseLoops->TrqFil.V.LpFil2[3] = 0;
	}
	else
	{
		TrqRefx = MlibLowPassfilter2( TrqRefx,
									  BaseLoops->TrqFil.P.Klpf2,
									  BaseLoops->TrqFil.V.LpFil2 );
	}

	BaseLoops->TrqRefo = MlibLimitul( TrqRefx, 0x01000000, -0x01000000 );

	return	BaseLoops->TrqRefo;

}



/****************************************************************************************************/
/*																									*/
/*		トルク制御モード切替初回変数初期化		<S0B8>												*/
/*																									*/
/****************************************************************************************************/
void	BaseInitTrqCtrlVariables( BASE_LOOPCTRLS *BaseLoops, SHORT IqRefMon, LONG *SpdRefo )
{
	if( BaseLoops->LastCtrlMode != CTRLMODE_TRQ )
	{ /* 前回の制御モードがトルク制御モード以外 */
		/* 今回トルク指令 = トルク指令フィルタ出力 = 前回q軸電流指令 */
		BaseLoops->TrqFil.V.FilOut = MlibMulhigh32( (IqRefMon << 16), 73300775 );
		BaseLoops->TrqRefi = BaseLoops->TrqFil.V.FilOut;

		/* トルク制御時は外乱入力 = 0 */
		BaseLoops->DisTrqRef3 = 0;

		/* トルク制御時は速度指令 = 0 */
		*SpdRefo = 0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		トルク指令出力 : Torque Reference --> ScanA AinK.IqRef/Idref								*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	位相補償方式A : 従来方式																		*/
/*																									*/
/*		IdRef = 0;																					*/
/*						  15000		TrqRef * 15000 * 2^8											*/
/*		IqRef = TrqRef * ------- = ----------------------											*/
/*						   2^24				2^32													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	位相補償方式B : IdRef/IqRef方式																	*/
/*																									*/
/*						  15000		MlibFASTSINS16( Pcmps + 0   )	  TrqRef * 1875 * SinX			*/
/*		IdRef = TrqRef * ------- * ------------------------------- = -----------------------		*/
/*						   2^24				16384							2^32 * 2^3				*/
/*																									*/
/*						  15000		MlibFASTSINS16( Pcmps + 256 )	  TrqRef * 1875 * CosX			*/
/*		IqRef = TrqRef * ------- * ------------------------------- = -----------------------		*/
/*						   2^24				16384							2^32 * 2^3				*/
/*																									*/
/****************************************************************************************************/
static void	KaiOutputTrqRef(BASE_LOOPCTRLS *BaseLoops, KSGAIN TrqRef, KSGAIN IdRef, KSGAIN DisTrq )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float IqRef;
#else
	LONG	IqRef;
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		電流指令演算 : 2^24 --> 15000																*/
/*--------------------------------------------------------------------------------------------------*/
#if( SVFSEL_PCMP_IDREF != 1 )						/* Pcmps by MotPhase							*/
	IdRef = MlibMulhigh32( IdRef,  3840000 );		/* IdRef(2^24)  --> IdRef(15000)				*/
	IqRef = MlibMulhigh32( TrqRef, 3840000 );		/* TrqRef(2^24) --> IqRef(15000)				*/
/*--------------------------------------------------------------------------------------------------*/
#else /* ( SVFSEL_PCMP_IDREF == 1 ) */				/* Pcmps by IdRef/IqRef							*/
	LONG	x,y;
	LONG	Pcmps;
	LONG	Pcmpx;
	LONG	Pcmpy;

	Pcmps = MlibABS( AinK.Pcmps );					/* Pcmps [65536/360deg]							*/
	Pcmpx = Pcmps >> 6;								/* Pcmpx [ 1024/360deg]							*/
	Pcmpy = Pcmps & 0x003F;							/* Pcmpy [Rem : 6bit]							*/
	/*----------------------------------------------------------------------------------------------*/
	x = MlibFASTSINS16( Pcmpx );					/* x = Sin( Pcmpx )								*/
	y = MlibFASTSINS16( Pcmpx + 1 );				/* y = Sin( Pcmpx + 1 )							*/
	y = (x + (((y - x) * Pcmpy)>>6)) * 1875;		/* y = (Sin( Pcmpx ) + dx) * 1875				*/
	IdRef = (MlibMulhigh32( TrqRef, y )+4) >> 3;	/* IdRef = TrqRef * Sin( x )					*/
	if( IdRef > 0 )
	{ /* IdRef is Negative Compensation */
		IdRef = -IdRef;
	}
	/*----------------------------------------------------------------------------------------------*/
	x = MlibFASTSINS16( Pcmpx + 256 );				/* x = Cos( Pcmpx )								*/
	y = MlibFASTSINS16( Pcmpx + 256 + 1 );			/* y = Cos( Pcmpx + 1 )							*/
	y = (x + (((y - x) * Pcmpy)>>6)) * 1875;		/* y = (Cos( Pcmpx ) + dx) * 1875				*/
	IqRef = (MlibMulhigh32( TrqRef, y )+4) >> 3;	/* IqRef = TrqRef * Cos( x )					*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		電流指令(d軸＆q軸)出力																		*/
/*--------------------------------------------------------------------------------------------------*/
	BaseLoops->IdRef   = (SHORT)IdRef;				/* IdRef  --> AinK.IdRef						*/
	BaseLoops->IqRef   = (SHORT)IqRef;				/* IqRef  --> AinK.IqRef	 					*/
/*--------------------------------------------------------------------------------------------------*/
/*		外乱トルク指令																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* DisTrq(2^24) --> AinK.IqDist(15000) */
	BaseLoops->IqDist = (SHORT)MlibMulhigh32( DisTrq, 3840000 );
/*--------------------------------------------------------------------------------------------------*/

}


/****************************************************************************************************/
/*																									*/
/*		安川サーボ制御処理メイン																	*/
/*																									*/
/****************************************************************************************************/
void ApxYecServoControl( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut ) /* <S0B8> */
{
	LONG			lwk;
	BOOL			BeComplete;
	TUNELESS_CTRL	*TuneLessCtrl;
	DOBS_CTRL		*DobsCtrl;
	RPLCMP			*RippleCmp;		/* <S14B> */
//	BOOL			RippleCmpExe;	/* <S14B> *//* <S1FD> */


	/* ローカル変数の初期化 */
	BeComplete		= (BaseLoops->BaseCtrls)->BeComplete;
	TuneLessCtrl	= &(BaseLoops->BaseCtrls->TuneLessCtrl);
	DobsCtrl		= &(BaseLoops->BaseCtrls->DobsCtrl);
	RippleCmp		= &(BaseLoops->RippleCmp);			/* <S14B> */
//	RippleCmpExe	= FALSE;							/* <S14B> *//* <S1FD> */

/*--------------------------------------------------------------------------------------------------*/
/*		制御用パラメータ転送処理																	*//* <S066> */
/*--------------------------------------------------------------------------------------------------*/
	/* Control Parameter Translate */
	if( TR_REQ_SCANA == TranlateCtrlGetReq(&(BaseLoops->BaseCtrls->TransduceCtrl)) )
	{
		TranlateCtrlPrm( &(BaseLoops->BaseCtrls->TransduceCtrl) );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		速度ＦＢ演算、位置指令作成																	*/
/*--------------------------------------------------------------------------------------------------*/
	LpxPosSpdFeedbackCal( BaseLoops );				/* モータ位置＆速度ＦＢ演算						*/

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ拡張制御																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( TRUE == TuneLessGetTuneLessUse(TuneLessCtrl) )
	{ /* 調整レス有効時 */
		/*------------------------------------------------------------------------------------------*/
		/*	調整レス制御 --> AoutV.SpdFbki, AoutV.TrqFBCx											*/
		/*------------------------------------------------------------------------------------------*/
		TuningLessCtrl( TuneLessCtrl,
						BaseLoops->SpdFbki,
						BaseLoops->TrqRefi,
						BaseLoops->TrqRefoLimit );

		/*------------------------------------------------------------------------------------------*/
		/*	調整レス制御演算結果取得																*/
		/*------------------------------------------------------------------------------------------*/
		if( TRUE == TuneLessGetTuneLessSts(TuneLessCtrl) )
		{ /* 調整レス有効時 */
			/* 速度フィードバック[2^24/OvrSpd] */
			BaseLoops->SpdObsFbki = TuneLessGetSpdObsFbk( TuneLessCtrl );
			/* トルクFB補償[2^24/MaxTrq] */
			BaseLoops->TrqFBCx += TuneLessGetDisTrqCmp( TuneLessCtrl );

#if 0 /* 2013.08.16 Y.Oka アナログモニタ未対応 @@Check */
			/* 推定速度[2^24/OvrSpd] */
			BoutV.AmonSpdObs = BaseLoops->SpdObsFbki;
			/* 外乱トルク[2^24/MaxTrq] */
			BoutV.AmonDisTrq  = -TuneLessGetDisTrq( TuneLessCtrl );
			/* 補償トルク[2^24/MaxTrq] */
			BoutV.AmonDisTrqCmp = -TuneLessGetDisTrqCmp( TuneLessCtrl );
#endif
		}
		else
		{ /* 調整レス無効時 */
			/* 速度フィードバック[2^24/OvrSpd]更新なし */
			/* トルクFB補償[2^24/MaxTrq]更新なし */
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;

#if 0 /* 2013.08.16 Y.Oka アナログモニタ未対応 @@Check */
			/* 推定速度[2^24/OvrSpd] */
			BoutV.AmonSpdObs = 0;
			/* 外乱トルク[2^24/MaxTrq] */
			BoutV.AmonDisTrq  = -TuneLessGetDisTrq( TuneLessCtrl );
			/* 補償トルク[2^24/MaxTrq] */
			BoutV.AmonDisTrqCmp = 0;
#endif
		}

		/* トルク指令フィルタ積分初期化要求 */
		if( TRUE == TuneLessGetTFilClrReq(TuneLessCtrl) )
		{
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;
		}
	}
	else
	{ /* 調整レス無効時 */
		/*------------------------------------------------------------------------------------------*/
		/*	外乱オブザーバ演算 --> AoutV.TrqFBCx													*/
		/*------------------------------------------------------------------------------------------*/
		DisturbObserver( DobsCtrl,
						 BaseLoops->SpdObsFbki,
						 BaseLoops->TrqRefoLimit );

		if( TRUE == DobsGetObserverSts(DobsCtrl) )
		{ /* 外乱オブザーバ有効時 */
			BaseLoops->TrqFBCx += DobsGetDisTrqCmp( DobsCtrl );
		}

		switch ( DobsGetExchangeSts(DobsCtrl) )
		{

		case DOBSCHNG_INV2ACT:	/* 無効⇒有効 */
			/* 速度積分トルク = 速度積分トルク - トルク補償 (トルク補償は負のため加算) */
#if (FLOAT_USE==TRUE)
			lwk = ((float)DobsCtrl->var.DisTrqCmp * BaseLoops->GselGains->Kv2Inv);
#else
			lwk = MlibMulgain(DobsCtrl->var.DisTrqCmp, BaseLoops->GselGains->Kv2Inv);
#endif /* FLOAT_USE */

			BaseLoops->SpdCtrl.V.Ivar64[1] += (lwk << 1);
			/* トルク指令フィルタ積分初期化要求 */
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;
			break;

		case DOBSCHNG_ACT2INV:	/* 有効⇒無効 */
			/* 速度積分トルク = 速度積分トルク + トルク補償 (トルク補償は負のため減算) */
#if (FLOAT_USE==TRUE)
			lwk = ((float)DobsCtrl->var.DisTrqCmp * BaseLoops->GselGains->Kv2Inv);
#else
			lwk = MlibMulgain(DobsCtrl->var.DisTrqCmp, BaseLoops->GselGains->Kv2Inv);
#endif /* FLOAT_USE */
			BaseLoops->SpdCtrl.V.Ivar64[1] -= (lwk << 1);
			/* トルク指令フィルタ積分初期化要求 */
			BaseLoops->TrqFil.V.TrqFilClrCmd = TRUE;
			break;

		case DOBS_NOCHANGE:		/* 切り替えなし */
		default:
			break;
		}

#if 0 /* 2013.08.16 Y.Oka アナログモニタ未対応 @@Check */
		/*--------------------------------------------------------------------------------------------------*/
		/*		モニタ用変数出力				注意)：”補償”トルクを出力するため外乱トルクと符号逆転		*/
		/*--------------------------------------------------------------------------------------------------*/
		/* 外乱オブザーバ推定速度[2^24/OvrSpd] */
		BoutV.AmonDobSpd  = DobsCtrl->var.SpdObs;
		/* 外乱トルク[2^24/MaxTrq] */
		BoutV.AmonDisTrq  = -DobsCtrl->var.DTrqLpfo;
		/* 摩擦補償トルク[2^24/MaxTrq] */
		BoutV.AmonDisTrqCmp = DobsCtrl->var.DisTrqCmp;
#endif


	}

	/************************************************************************************************/
	/* 制御ループ演算																				*/
	/************************************************************************************************/
	switch(BaseLoops->CtrlMode)
	{
	/*----------------------------------------------------------------------------------------------*/
	/* 位置制御モード & PJOGモード																	*/
	/*----------------------------------------------------------------------------------------------*/
	case CTRLMODE_POS:
		/* 位置指令更新 ＆ 位置偏差演算 <S14C> */
		LpxSvPositionManager( BaseLoops, &(BaseLoops->BaseCtrls->PosManager) );

		/* 位置制御演算処理 */
		BaseLoops->SpdRefp = BasePositionControlA( BaseLoops,
												   (BaseLoops->BaseCtrls)->CtrlCmdMngr.PerrClrReq,
												   (BaseLoops->CmdCtrlBit & CLRPOSINTG_BIT),
												   BaseLoops->GselGains->Kp,
												   BaseLoops->f.PerClrEdge /* @@ AP */ );

		/* サーボオン時速度リミット処理 */
		BaseLoops->SpdRefi = SvonSpeedLimit( BeComplete, BaseLoops, BaseLoops->SpdRefp );

		/* 速度制御演算 */
		BaseLoops->TrqRefi = BaseSpeedControl( BaseLoops,
											   BaseLoops->SpdFFCx,
											   BaseLoops->SpdFBCx,
											   BeComplete );
		/* トルクフィルタ */
		BaseLoops->TrqRefo = BaseTorqueFilterForPSCtrl( BaseLoops,									  /* <S0B8> */
														BaseLoops->TrqRefi,
														BaseLoops->TrqFBCx,
														BaseLoops->TrqFFCx,
														BeComplete );

//		RippleCmpExe = FALSE;	/* 速度リップル補償機能の無効 */	/* <S14B> *//*<500>*/

		break;

	/*----------------------------------------------------------------------------------------------*/
	/* 速度制御モード & JOGモード & 原点サーチモード												*/
	/*----------------------------------------------------------------------------------------------*/
	case CTRLMODE_SPD:	/* 速度制御モード */
	case CTRLMODE_ZSRCH:	/* @@ */
	case CTRLMODE_JOG:	/* 
						 * @@check JOG MODE
						 * switchの中にJOGと原点復帰がないため、補助機能実行するとﾄﾙｸが出ない
						 */
	/*----------------------------------------------------------------------------------------------*/
		/* 速度制御演算 */
		BaseLoops->TrqRefi = BaseSpeedControl( BaseLoops,
											   0,
											   BaseLoops->SpdFBCx,
											   BeComplete );
		/* トルクフィルタ */
		BaseLoops->TrqRefo = BaseTorqueFilterForPSCtrl( BaseLoops,									  /* <S0B8> */
														BaseLoops->TrqRefi,
														BaseLoops->TrqFBCx,
														BaseLoops->TrqFFCx,
														BeComplete );
/*<S1FD>*/
#if 0
/* <S14B> Start */
	if( RippleCmp->VelRipplePhaseReady == TRUE)	/* 磁極検出完了(位相検出完了) */
	{
		if(RippleCmp->RippleCmpThresholdSel == CMD_SPD)
		{	/* 速度リップル補償機能の有効化しきい値条件 = 「指令速度」 */
			if( RippleCmp->VelRippleCompSpd < BaseLoops->SpdRefo )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
		else
		{/* 速度リップル補償機能の有効化しきい値条件 = 「フィードバック速度」 */
			if( RippleCmp->VelRippleCompSpd < BaseLoops->SpdFbki )
			{
				RippleCmpExe = TRUE;
			}
			else
			{
				RippleCmpExe = FALSE;
			}
		}
	}
	else	/* 磁極検出未完了(位相検出未完了) */
	{
		RippleCmpExe = FALSE;
	}
/* <S14B> End */
#endif
		break;

	/*----------------------------------------------------------------------------------------------*/
	/* トルク制御モード																				*/
	/*----------------------------------------------------------------------------------------------*/
	case CTRLMODE_TRQ:	/* トルク制御モード *//* <S0B8> */
		/* トルク制御モード切り替えショック軽減処理 */
		BaseInitTrqCtrlVariables( BaseLoops,
								  CtrlLoopOut->IqRefMon,
								  &(BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo) );
		BaseLoops->TrqRefo = BaseTorqueControl( BaseLoops, BeComplete );
#if 0 /* <S0B8> */
#if	SVFSEL_ADC_USE == 1
		/* <S051> */
		BaseLoops->TrqRefi = MlibLpfilter1( BaseLoops->TRefAd, BaseLoops->TrqFil.A.Ktrflpf /* <S083> Klpf */, BaseLoops->TrqRefi );
		BaseLoops->TrqRefo = BaseLoops->TrqRefi - (BaseLoops->BaseCtrls)->CtrlCmdMngr.OverTrq;
#else
		BaseLoops->TrqRefo = /* BaseLoops->TrqRefi */
				(BaseLoops->BaseCtrls)->CtrlCmdMngr.TrqRefo -		/* @@ */
				(BaseLoops->BaseCtrls)->CtrlCmdMngr.OverTrq;

#endif
#endif /* <S0B8> */

/*<S1FD>*/
#if 0
/* <S14B> Start */
	/* 速度リップル補償機能の有効化しきい値条件確認 */
	if( RippleCmp->VelRipplePhaseReady == TRUE)	/* 磁極検出完了(位相検出完了) */
	{
		if( RippleCmp->VelRippleCompSpd < BaseLoops->SpdFbki )
		{
			RippleCmpExe = TRUE;
		}
		else
		{
			RippleCmpExe = FALSE;
		}
	}
	else	/* 磁極検出未完了(位相検出未完了) */
	{
		RippleCmpExe = FALSE;
	}
/* <S14B> End */
#endif
		break;

	default:
		break;
	}

	/* モータレステストモード内部メカモデル演算 *//* <S1B7> */
	if( BaseLoops->MotorLessTest->MotorLessTestMode == TRUE )
	{
		ApxMotorLessTestModeMechaModel( BaseLoops->TrqRefoLimit, BeComplete, BaseLoops->MotorLessTest );
	}

/* <S14B> Start */
	/*--------------------------------------------------------------------------------------------------*/
	/*		速度リップル補償	--> BaseLoops->TrqRefo														*/
	/*--------------------------------------------------------------------------------------------------*/
	/* リップル補正前トルク指令(トレース・アナログモニタ用データ)取得 */
	RippleCmp->TrqRefBeforeComp = BaseLoops->TrqRefo;

	if(RippleCmp->RplCmpPrm.RplCmpUse == TRUE)	/* 速度リップル補償機能 有効 *//*<S1FD>*/
	{
//		if(  RippleCmpExe == TRUE )	/* 速度リップル補償機能の有効化しきい値条件 成立 *//*<S1FD>*/
		{
#if 0	/* 速度リップル補償 機械角補正処理(将来用としてコメントアウト) */
			/* 機械角算出 （エンコーダビット数/360°） */
			lwk = (LONG)((LONG)((BaseLoops->MencV->MotPosX[0]) - (BaseLoops->MencV->MotCposX)) >> BaseLoops->MencV->MposSftR);
			BaseLoops->MencV->MotAngle = ( lwk >= 0 ) ? lwk : lwk + BaseLoops->MencV->PulseNo;

			/* [エンコーダビット数/360°] →　[65536/360°]*/
			lwk = (BaseLoops->MencV->EncBitNo > 16) ?  (lwk >> ( BaseLoops->MencV->EncBitNo - 16 ))
													  :(lwk << ( 16 - BaseLoops->MencV->EncBitNo ));
#endif
			/* 速度リップル補償機能の有効条件チェック */
			RippleCompChkThresholdVal( 	&(BaseLoops->RippleCmp),
										BaseLoops->CtrlMode,
										BaseLoops->SpdRefo,
										BaseLoops->SpdFbki,
										TuneLessGetTuneLessSts(&((BaseLoops->BaseCtrls)->TuneLessCtrl)));
			/* 速度リップル補償 */
			BaseLoops->TrqRefoComp = RippleCompensation( &(BaseLoops->RippleCmp),
														 0,//lwk,/*<S1FD>*/
														 BaseLoops->MencV->MotPhaseC,
														 BeComplete);

			//RippleCmp->AmonTrqComp1 = BaseLoops->TrqRefoComp;/*<S1FD>*/
			BaseLoops->TrqRefoComp  = BaseLoops->TrqRefoComp + BaseLoops->TrqRefo;
		}
//		else	/* 速度リップル補償機能の有効化しきい値条件 不成立 */
//		{
//			BaseLoops->TrqRefoComp = RippleCmp->AmonTrqComp1;
//			BaseLoops->TrqRefoComp =RippleCmp->TrqRefBeforeComp;	/* 速度リップル補償機能 無効 */
//		}
	}
	else
	{
		BaseLoops->TrqRefoComp = BaseLoops->TrqRefo;	/* 速度リップル補償機能 無効 */
	}
/* <S14B> End */

	/*--------------------------------------------------------------------------------------------------*/
	/*		<S0CD> 摩擦ﾓﾃﾞﾙ補償												*/
	/*--------------------------------------------------------------------------------------------------*/
//#if( FRICTRQ_COMP != 0 )
   BaseLoops->BaseCtrls->TrqMotFricComp = LpxMotorFricComp( BaseLoops->BaseCtrls->FrictionM.MotFricEnable, BaseLoops );
   BaseLoops->TrqRefoComp += BaseLoops->BaseCtrls->TrqMotFricComp;
//#endif
	/*--------------------------------------------------------------------------------------------------*/
	/*		トルク指令(CPU)	--> q軸電流指令(μプログラム)												*/
	/*--------------------------------------------------------------------------------------------------*/
	/* TrqRefo --> AinK.IqRef */
	KaiOutputTrqRef( BaseLoops, BaseLoops->TrqRefoComp, 0, BaseLoops->DisTrqRef3 );

	/*--------------------------------------------------------------------------------------------------*/
	/*		ScanB情報更新処理																			*/
	/*--------------------------------------------------------------------------------------------------*/
	/* 平均トルク指令更新(For ScanB) */
#if (FLOAT_USE==TRUE)
	BaseLoops->TrqRefo_a = ( BaseLoops->TrqRefo + BaseLoops->TrqRefo_l ) /2.0f;
#else
	BaseLoops->TrqRefo_a = ( BaseLoops->TrqRefo + BaseLoops->TrqRefo_l ) >> 1;
#endif /* FLOAT_USE */
	BaseLoops->TrqRefo_l = BaseLoops->TrqRefo;

}



/****************************************************************************************************/
/*																									*/
/*		モータ位置＆速度ＦＢ演算																	*/
/*																									*/
/****************************************************************************************************/
static	void	LpxPosSpdFeedbackCal( BASE_LOOPCTRLS *BaseLoops )
{
	EHVOBS_CTRL		*EhVobsCtrl;

	EhVobsCtrl = &(BaseLoops->BaseCtrls->EhVobsCtrl);

/*--------------------------------------------------------------------------------------------------*/
/*		位置ＦＢ更新																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* フルク未対応 2013.08.20 Y.Oka @@CHECK */
//	AoutV.dPosFbki = (Kprm.f.FencUse)? AinK.dFencPos : AinK.dMotPos;
//	BaseLoops->dPosFbki = BaseLoops->dMotPos;/* <S04D> */
	BaseLoops->dPosFbki = (BaseLoops->Bprm->FencUse)? BaseLoops->dFencPos : BaseLoops->dMotPos;/* <S04D> */

/*--------------------------------------------------------------------------------------------------*/
/*		速度ＦＢ更新																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位相補償速度オブザーバ前速度生成 */
	/* フルク未対応 2013.08.20 Y.Oka @@CHECK */
//	AoutV.SpdFbki = ( Iprm.f.PsFulcSelect ) ? AinK.FencSpd : AinK.MotSpd;
//	BaseLoops->SpdFbki = BaseLoops->MotSpd;/* <S04D> */
	BaseLoops->SpdFbki = ( BaseLoops->Bprm->PsFulcSelect ) ? BaseLoops->FencSpd : BaseLoops->MotSpd;

	/* 位相補償速度オブザーバ演算 */
	if( TRUE == EhVobsGetObserverUse(EhVobsCtrl) )
	{
#if( CSW_SVCF_EHVOBS_USE != 0 )
		/* FB速度 */
		BaseLoops->SpdObsFbki = EhSpeedObserver( EhVobsCtrl,
												 BaseLoops->SpdFbki,
												 BaseLoops->TrqRefi );
#else
		/* FB速度 */
		BaseLoops->SpdObsFbki = BaseLoops->SpdFbki;
#endif
	}
	else
	{
		/* FB速度 */
		BaseLoops->SpdObsFbki = BaseLoops->SpdFbki;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		平均速度更新(For ScanB)																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	BaseLoops->MotSpd_a = ( BaseLoops->MotSpd + BaseLoops->MotSpd_l ) / 2.0f;
#else
	BaseLoops->MotSpd_a = ( BaseLoops->MotSpd + BaseLoops->MotSpd_l ) >> 1;
#endif /* FLOAT_USE */
	BaseLoops->MotSpd_l = BaseLoops->MotSpd;

/*	Full Closed Encoder Position																	*//* <S04D> */
#if (FLOAT_USE==TRUE)
	BaseLoops->FencSpd_a = ( BaseLoops->FencSpd + BaseLoops->FencSpd_l ) / 2.0f
#else
	BaseLoops->FencSpd_a = ( BaseLoops->FencSpd + BaseLoops->FencSpd_l ) >> 1;
#endif /* FLOAT_USE */
	BaseLoops->FencSpd_l = BaseLoops->FencSpd;

	
#if (FLOAT_USE==TRUE)
	BaseLoops->SpdObsFbki_a = ( BaseLoops->SpdObsFbki + BaseLoops->SpdObsFbki_l ) /2.0f;
#else
	BaseLoops->SpdObsFbki_a = ( BaseLoops->SpdObsFbki + BaseLoops->SpdObsFbki_l ) >> 1;
#endif /* FLOAT_USE */

	BaseLoops->SpdObsFbki_l = BaseLoops->SpdObsFbki;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		モータ速度検出用移動平均フィルタ処理	<V259>	<S02F>										*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 速度制御周期の高速化による速度検出分解能の低下の対策として検出速度を移動平均フィルタ	*/
/*			に通す。ただし、接続されたエンコーダ分解能によって移動平均回数を変更する。				*/
/*			移動平均回数のオンラインでの変更は、BpiChangeSpdDetMaFil( )にて行うこと。				*/
/*																									*/
/****************************************************************************************************/
static KSGAIN	LpxSpdDetMaFilter( MOTSPDMAFIL *SpdMafil, KSGAIN MotSpd )/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	lwk0;
	float	output;
	float	idx_max;
#else
	LONG	lwk0;
	LONG	output;
	LONG	idx_max;
#endif /* FLOAT_USE */

	idx_max = 1 << SpdMafil->manumBit;

	/* バッファ内データ取得 */
	lwk0 = SpdMafil->ksub * SpdMafil->mabuf[SpdMafil->idx];
	/* 今回データ格納 */
	SpdMafil->mabuf[SpdMafil->idx] = MotSpd;

	SpdMafil->idx++;
	if( SpdMafil->idx >= idx_max )
	{
		/* インデックスをリセット */
		SpdMafil->idx = 0;
		SpdMafil->ksub = 1;
	}
	/* バッファ積算値更新 */
	SpdMafil->mabufSumx = SpdMafil->mabufSumx - lwk0 + MotSpd;
	/* 今回出力 */
	output = ( SpdMafil->mabufSumx >> SpdMafil->manumBit );

	return( output );
}



/****************************************************************************************************/
/*																									*/
/*		Kernel Input Service for ScanA																*/
/*																									*/
/****************************************************************************************************/
void	KpxInputScanA( BASE_LOOPCTRLS *BaseLoops )/*<S0C5>*/
{
#if (FLOAT_USE==TRUE)
	float			lwk0;									/* long Work							*/
	float			MotSpd;									/* Motor Speed							*/
	float			dMotPos;
	float			PosTrqLmt;
	float			NegTrqLmt;
#else	
	LONG			lwk0;									/* long Work							*/
	LONG			MotSpd;									/* Motor Speed							*/
	LONG			dMotPos;
	LONG			PosTrqLmt;
	LONG			NegTrqLmt;
	LONG			TuneLessMode;
	BOOL			TuningLessVComp;
	BOOL			VcompSts;
#endif /* FLOAT_USE */
	MENCV			*MencV;
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D> */
	MENCV			*FencV;
#endif /* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT) */
	BPRMDAT			*Bprm;
	BASE_CTRL_HNDL	*BaseCtrls;		/* <S004> */
	ASIC_HW_IF_STR	*pAsicHwReg;	/* <S004> */
	ALMMAN_HANDLE	*AlmManager;	/* <S018> */
	VFBTIMBASE		*Vfbtimbase;	/* <S067> */

	MencV	= BaseLoops->MencV;
	FencV	= BaseLoops->FencV;
	Bprm	= BaseLoops->Bprm;
	BaseCtrls = BaseLoops->BaseCtrls;		/* <S004> */
	pAsicHwReg = BaseLoops->pAsicHwReg;		/* <S004> */
	AlmManager = BaseLoops->AlmManager;		/* <S018> */
	Vfbtimbase = &(BaseLoops->Vfbtimbase);	/* <S067> */

/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダ位置データ読込み, 分周パルス出力処理												*/
/*--------------------------------------------------------------------------------------------------*/
	/* 設定されているエンコーダ位置データ読込み関数を実行 *//* <S1B7> */
//	SencReadPosition( MencV, pAsicHwReg, AlmManager, BaseCtrls->BeComplete );						  /* <S018> *//* <S0C5> */
	BaseLoops->SencReadPosition( MencV, pAsicHwReg, AlmManager, BaseCtrls->BeComplete, (void*)BaseLoops );
	dMotPos = ( MencV->RcvPosX[0] - MencV->RcvPosX[1] ) >> MencV->MposSftR;

/*--------------------------------------------------------------------------------------------------*/
/*		変化時刻タイマによる速度補正																*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*	差速度を元に正規化[pulse/ScanA]→[2^24/OvrSpd]												*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	MotSpd = dMotPos * (Bprm->KmotspdA);
#else
	MotSpd = MlibMulgain( dMotPos, Bprm->KmotspdA );					/* Motor Speed[2^24/OvrSpd]	*/
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		速度補正用内部状態取得	<S067>															*/
	/*----------------------------------------------------------------------------------------------*/
	TuneLessMode    = BaseLoops->BaseCtrls->TuneLessCtrl.conf.TuningLessMode;
	TuningLessVComp = BaseLoops->BaseCtrls->TuneLessCtrl.conf.TuningLessVComp;
	VcompSts = ( (TuneLessMode & TLMODE_SPDDETCHNG) && (TuningLessVComp == FALSE) ) ? FALSE : TRUE;
	/*----------------------------------------------------------------------------------------------*/
	/*		速度補正有効判定		<S067>															*/
	/*----------------------------------------------------------------------------------------------*/
	if( MencV->SpdCmpEnable == FALSE )
	{
		MencV->MotSpd = TimeBasesMotSpdCalc( Vfbtimbase, MencV, dMotPos, MotSpd, VcompSts, FALSE );
		MencV->SpdCmpEnable = TRUE;
	}
	else
	{
		MencV->MotSpd = TimeBasesMotSpdCalc( Vfbtimbase, MencV, dMotPos, MotSpd, VcompSts, TRUE );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	速度検出移動平均フィルタ処理		<V110>													*/
	/*----------------------------------------------------------------------------------------------*/
	if( BaseLoops->MotSpdMafil->manumBit == 0 )
	{ /* エンコーダ17bit以上 */
		MotSpd = MencV->MotSpd;
	}
	else
	{ /* エンコーダ16bit以下 */
		MotSpd = LpxSpdDetMaFilter( BaseLoops->MotSpdMafil, MencV->MotSpd );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Motor Position and Speed															*/
/*--------------------------------------------------------------------------------------------------*/
	/* Cal. Delta Position */
	lwk0 = MencV->MotPos - BaseLoops->LastMotPos;
	/* Update Motor Position */
	BaseLoops->LastMotPos = MencV->MotPos;

//<S00E>共通化#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE				/* M2,M3,etc */
//	lwk0 = Bprm->DirSign * lwk0;
//	BaseLoops->MotPos += lwk0;							/* Update Motor Position		*/
//	BaseLoops->dMotPos = lwk0;							/* Update Motor Delta Position	*/
//	BaseLoops->MotSpd  = MotSpd;						/* Motor Speed for ScanB		*/
//<S00E>共通化#else													/* Analog & Pulse */
	{ 
	CTRL_CMD_MNG *pcmd = &(BaseLoops->BaseCtrls->CtrlCmdMngr);	
	UINT PerClrEdge = ( pcmd->PerClrEdge == TRUE) && ( BaseLoops->f.PastInputClrEdge == FALSE );
	
	if( Bprm->RvsDir )									/* 逆回転モード							*/
	{
		BaseLoops->MotPos -=  lwk0;						/* Update Motor Position				*/
		if(	PerClrEdge && (Bprm->FencUse == FALSE) )
		{
			BaseLoops->dMotPos = BaseLoops->MotPos - pcmd->ClrFbPos;		//<V584>
		}
		else
		{
			BaseLoops->dMotPos = -lwk0;
		}
		BaseLoops->MotSpd  = -MotSpd;					/* Motor Speed for ScanB [2^24/OvrSpd]	*/
	}
	else												/* 正回転モード							*/
	{
		BaseLoops->MotPos += lwk0;						/* Update Motor Position				*/
		if(	PerClrEdge && (Bprm->FencUse == FALSE) )
		{
			BaseLoops->dMotPos = BaseLoops->MotPos - pcmd->ClrFbPos;
		}
		else
		{
			BaseLoops->dMotPos = lwk0;
		}

		BaseLoops->MotSpd  = MotSpd;					/* Motor Speed for ScanB [2^24/OvrSpd]	*/
	}
	/*----------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Input Full Closed Encoder Position															*/
/*--------------------------------------------------------------------------------------------------*//* <S04D> */
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)
	if( Bprm->FencUse )										/* フルクエンコーダ使用時				*/
	{
		if(Bprm->RvsDir )									/* 逆回転モード							*/
		{
//			lwk0 = -FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete );	/* <S0E0>:AlmManager追加 */
			lwk0 = -BaseLoops->FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete, (void*)BaseLoops );	/*<S1B7>*/
															/* フルクエンコーダ位置データ読込み		*/
			BaseLoops->FencSpd = -FencV->MotSpd;			/* Load Speed [2^24/OvrSpd]				*/
		}
		else												/* 正回転モード							*/
		{
//			lwk0 = FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete );	/* <S0E0>:AlmManager追加 */
			lwk0 = BaseLoops->FencReadPosition( FencV, AlmManager, BaseCtrls->BeComplete, (void*)BaseLoops );	/*<S1B7>*/
															/* フルクエンコーダ位置データ読込み		*/
			BaseLoops->FencSpd = FencV->MotSpd;				/* Load Speed [2^24/OvrSpd]				*/
		}
		/*------------------------------------------------------------------------------------------*/
		BaseLoops->FencPos += lwk0;							/* Update Fencoder Position				*/
		if(	PerClrEdge )
		{
			BaseLoops->dFencPos = BaseLoops->FencPos - pcmd->ClrFbPos;
		}
		else
		{
			BaseLoops->dFencPos = lwk0;
		}
		/*------------------------------------------------------------------------------------------*/
	}
#endif/* 	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)	*/
	BaseLoops->f.PastInputClrEdge = pcmd->PerClrEdge;		/* 偏差エッジクリア入力前回値保持		*/
	BaseLoops->f.PerClrEdge = PerClrEdge;					/* Perr Clear Edge Command Input */
	/* 
	 * @@ AP
	 */
	}

//#endif	<S00E>共通化

/*--------------------------------------------------------------------------------------------------*/
/*		Torque Reference after Torque Limit for Observer											*/
/*--------------------------------------------------------------------------------------------------*/
	PosTrqLmt = (BaseLoops->BaseCtrls)->TrqLimitData.var.PosTrqLmtOut;
	NegTrqLmt = (BaseLoops->BaseCtrls)->TrqLimitData.var.NegTrqLmtOut;
	if( BaseLoops->TrqRefo > PosTrqLmt )
	{
		BaseLoops->TrqRefoLimit = PosTrqLmt;
	}
	else if( BaseLoops->TrqRefo < NegTrqLmt )
	{
		BaseLoops->TrqRefoLimit = NegTrqLmt;
	}
	else
	{
		BaseLoops->TrqRefoLimit = BaseLoops->TrqRefo;
	}

	return;/*<S0C5>*/
}



/****************************************************************************
  Carrer frequency change Module
*****************************************************************************/
static void	BaseLoopChangeCarrerFreq( BASE_LOOPCTRLS *BaseLoops,
									  CTRL_LOOP_OUT *CtrlLoopOut,
									  ASICS *SvAsicRegs )
{
	CARRCHNG	*CarrerChange;
#if (FLOAT_USE==TRUE)
	float		AbsMotSpd;		/* FB速度絶対値 */
	float		MotIqref2;		/* q軸電流指令^2 */
	float		MotIqfb;		/* q軸電流検出値 */
	float		MotIqfb2;		/* q軸電流検出値^2 */
	float		MotIdfb;		/* d軸電流検出値 */
	float		MotIdfb2;		/* d軸電流検出値^2 */
	float		MotIfb2;		/* (q軸電流検出値^2) + (d軸電流検出値^2) */
	float		MotIq;			/* Max(q軸電流指令^2, |q軸電流FBベクトル|^2) */
#else
	LONG		AbsMotSpd;		/* FB速度絶対値 */
	LONG		MotIqref2;		/* q軸電流指令^2 */
	LONG		MotIqfb;		/* q軸電流検出値 */
	LONG		MotIqfb2;		/* q軸電流検出値^2 */
	LONG		MotIdfb;		/* d軸電流検出値 */
	LONG		MotIdfb2;		/* d軸電流検出値^2 */
	LONG		MotIfb2;		/* (q軸電流検出値^2) + (d軸電流検出値^2) */
	LONG		MotIq;			/* Max(q軸電流指令^2, |q軸電流FBベクトル|^2) */
#endif /* FLOAT_USE */

	CarrerChange = &(BaseLoops->CarrerChange);

/*--------------------------------------------------------------------------------------------------*/
/*		キャリア周波数切り替え処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( CarrerChange->conf.CarrChange )					/* キャリア切り替えフラグON							*/
	{
		/*---------- 絶対値計算処理 ----------------------------------------------------------------*/
		/* 電流指令絶対値計算	*/
		MotIqref2 = (LONG)BaseLoops->IqRef * (LONG)BaseLoops->IqRef;

		/* 電流FB絶対値計算		*/
//		MotIqfb = KPX_INPUT_IQMON( );
		MotIqfb = CtrlLoopOut->IqRefMon;
		MotIqfb2 = (LONG)MotIqfb * (LONG)MotIqfb;
//		MotIdfb = KPX_INPUT_IDMON( );
		MotIdfb = CtrlLoopOut->IdRefMon;
		MotIdfb2 = (LONG)MotIdfb * (LONG)MotIdfb;
		MotIfb2 = MotIqfb2 + MotIdfb2;
		/* 指令とFBで大きいほうの電流閾値判断する */
		if( MotIqref2 > MotIfb2 )
		{
			MotIq = MotIqref2;
		}
		else
		{
			MotIq = MotIfb2;
		}

		/* モータ速度絶対値計算 */
		if( BaseLoops->MotSpd < 0 )
		{
			AbsMotSpd = -BaseLoops->MotSpd;
		}
		else
		{
			AbsMotSpd = BaseLoops->MotSpd;
		}
		/*--------- キャリア周波数切り替え判断 & μプログラム書き込み処理 --------------------------*/
		if( CarrerChange->var.CarrState )
		{ /* 現在高キャリア設定 */
			/*--------- 閾値Bよりも大きくなったらキャリア周波数を下げる(Or) ------------------------*/
			if( ( MotIq > CarrerChange->conf.IqLimitB ) || ( AbsMotSpd > CarrerChange->conf.VelLimitB ) )
			{
				CarrerChange->var.CarrState = FALSE;
				/* ★μIF作成後アドレス変更★ */
				/* ★μIFへのアウトプットは専用処理とする。★ */
				SHalDrv_SetCarrerFreq( SvAsicRegs->AsicMicroReg, CarrerChange->conf.NormCarrCnt );
			}
		}
		else
		{ /* 現在通常キャリア設定 */
			/*--------- 閾値Aよりも小さくなったらキャリア周波数を上げる(And) ------------------------*/
			if( ( MotIq < CarrerChange->conf.IqLimitA ) && ( AbsMotSpd < CarrerChange->conf.VelLimitA ) )
			{
				CarrerChange->var.CarrState = TRUE;
				/* ★μIF作成後アドレス変更★ */
				/* ★μIFへのアウトプットは専用処理とする。★ */
				SHalDrv_SetCarrerFreq( SvAsicRegs->AsicMicroReg, CarrerChange->conf.HighCarrCnt );
			}
		}
	}
	return;
}


/****************************************************************************
  Phase compensation value calculation function
*****************************************************************************/
#define SVFSEL_PCMP_IDREF			0
/*--------------------------------------------------------------------------------------------------*/
/*		位相補償 : Pcmp = Pcmpd(遅れ補償) ＋ Pcmps(速度補償=Kpcmps*MotSpd)							*/
/*--------------------------------------------------------------------------------------------------*/
/*	注意1：	従来方式(SVFSEL_PCMP_IDREF != 1)では、回生時の速度補償が界磁強めになり、				*/
/*			OV等が発生しやすくなる。																*/
/*	注意2：	トルク指令の符号(電動／回生)で、速度補償(Pcmps)の符号を切り替える方式では、				*/
/*			無負荷時(トルク指令がゼロ近傍)に、速度補償が正負に振れるため問題がある。				*/
/*　注意3：	リニアモータの場合は、LpxUpdateLMPhase( )で位相更新。									*/
/*--------------------------------------------------------------------------------------------------*/
static void	BaseLoopMakePhaseCompValue( BASE_LOOPCTRLS *BaseLoops,
										CTRL_LOOP_OUT *CtrlLoopOut,
										MENCV *MencV,
										BOOL WfActive )
{
	BPRMDAT		*Bprm;
	PHASECMP	*PhaseCmp;
	KSGAIN		MotSpd;			/*<S00A>*/

	Bprm = BaseLoops->Bprm;
	PhaseCmp = &(BaseLoops->PhaseComp);
	MotSpd = BaseLoops->MotSpd * Bprm->DirSign;				/* Motor Speed [2^24/OvrSpd]			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Pcmpd[65536/360deg]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/* 位相補償(遅れ1) */
	PhaseCmp->var.Pcmpd = (MotSpd * PhaseCmp->conf.Kpcmpd);
	/* 位相補償(遅れ2) */
	PhaseCmp->var.Pcmpd2 = ( MotSpd * PhaseCmp->conf.Kpcmpd2 );
	/* 位相補間量の計算 */
	PhaseCmp->var.PcmpdItplt = ( MotSpd * PhaseCmp->conf.KpcmpdItplt );
#else
	/* 位相補償(遅れ1) */
	PhaseCmp->var.Pcmpd = (SHORT)MlibMulgainNolim( MotSpd, PhaseCmp->conf.Kpcmpd );
	/* 位相補償(遅れ2) */
	PhaseCmp->var.Pcmpd2 = (SHORT)MlibMulgainNolim( MotSpd, PhaseCmp->conf.Kpcmpd2 );
	/* 位相補間量の計算 */
	PhaseCmp->var.PcmpdItplt = (SHORT)MlibMulgainNolim( MotSpd, PhaseCmp->conf.KpcmpdItplt );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate Pcmps[65536/360deg]																*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位相補償1(速度) */
#if (FLOAT_USE==TRUE)
	if( MotSpd > PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = ( (MotSpd - PhaseCmp->conf.PcmpSpd1) *PhaseCmp->conf.Kpcmpsp1 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = ( (MotSpd + PhaseCmp->conf.PcmpSpd1) * PhaseCmp->conf.Kpcmpsp1 );
	}
#else
	if( MotSpd > PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = 
			(SHORT)MlibMulgainNolim( (MotSpd - PhaseCmp->conf.PcmpSpd1), PhaseCmp->conf.Kpcmpsp1 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd1 )
	{
		PhaseCmp->var.Pcmps1 = 
			(SHORT)MlibMulgainNolim( (MotSpd + PhaseCmp->conf.PcmpSpd1), PhaseCmp->conf.Kpcmpsp1 );
	}
#endif /* FLOAT_USE */
	else
	{
		PhaseCmp->var.Pcmps1 = 0;
	}
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* SGDV機能無効 */
	/* 位相補償2(速度) */
#if (FLOAT_USE==TRUE)
	if( MotSpd > PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 
			(SHORT)( (float)(MotSpd - PhaseCmp->conf.PcmpSpd2) * PhaseCmp->conf.Kpcmpsp2 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 	(SHORT)( (float)(MotSpd + PhaseCmp->conf.PcmpSpd2)  * PhaseCmp->conf.Kpcmpsp2);
			
#else
	if( MotSpd > PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 
			(SHORT)MlibMulgainNolim( (MotSpd - PhaseCmp->conf.PcmpSpd2), PhaseCmp->conf.Kpcmpsp2 );
	}
	else if( MotSpd < -PhaseCmp->conf.PcmpSpd2 )
	{
		PhaseCmp->var.Pcmps2 = 
			(SHORT)MlibMulgainNolim( (MotSpd + PhaseCmp->conf.PcmpSpd2), PhaseCmp->conf.Kpcmpsp2 );
	}
#endif /* FLOAT_USE */
	else
	{
		PhaseCmp->var.Pcmps2 = 0;
	}
#endif
	/*----------------------------------------------------------------------------------------------*/
	/* 位相補償合計(速度) [65536/360deg] */
	PhaseCmp->var.Pcmps = PhaseCmp->var.Pcmps1 + PhaseCmp->var.Pcmps2;


	if( SVFSEL_PCMP_IDREF != 1 )
	{ /* Pcmps by MotPhase */
//		if( ((Prm->dq_sw & 0x0800) == 0 )
//			|| (( Prm.flg_wf & 0x0001 ) == 0) )
		if( WfActive == TRUE )
		{ /* 電圧フィードバック方式弱め界磁使用時、速度補償はしない */
//			MencV->MotPhaseX = MencV->MotPhase + PhaseCmp->var.Pcmpd;
			MencV->MotPhaseX = MencV->MotPhase;
		}
		else
		{
			MencV->MotPhaseX = MencV->MotPhase + PhaseCmp->var.Pcmpd + PhaseCmp->var.Pcmps;
		}
	}
	else if( SVFSEL_PCMP_IDREF == 1 )
	{ /* Pcmps by IdRef/IqRef */
		MencV->MotPhaseX = MencV->MotPhase + PhaseCmp->var.Pcmpd;
	}

	MencV->MotPhaseC = MencV->MotPhase + PhaseCmp->var.Pcmpd2;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Kernel Output Service for ScanA																*/
/*																									*/
/****************************************************************************************************/
ALMID_T	KpxOutputScanA( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs, PRMDATA *Prm )
{
	MENCV			*MencV;
	USHORT			WfActive;
	ALMID_T			errorID;

	MencV = BaseLoops->MencV;
	errorID = NO_ALARM;

	/* Change Carrer Frequency */
	BaseLoopChangeCarrerFreq( BaseLoops, CtrlLoopOut, SvAsicRegs );

	/* Make Phase Compensetion Value */
	if( ((Prm->dq_sw & 0x0800) == 0 ) || (( Prm->MencP.flg_wf & 0x0001 ) == 0) )
	{
		WfActive = FALSE;
	}
	else
	{
		WfActive = TRUE;
	}
	BaseLoopMakePhaseCompValue( BaseLoops, CtrlLoopOut, MencV, WfActive );

	return errorID;
}




/****************************************************************************************************/
/*																									*/
/*		Initialize function for ScanA																*/
/*																									*/
/****************************************************************************************************/
void SysScanAInit(	BASE_LOOPCTRLS *BaseLoops,
					MENCV			*MencV,
					MENCV			*FencV,	/* <S04D> */
					BPRMDAT			*Bprm,
					MOTSPDMAFIL		*MotSpdMafil,		/* モータ速度検出用移動平均演算用変数		*/
					BASE_CTRL_HNDL 	*BaseCtrls,
					GSELGAINS		*GselGains,
					ASIC_HW_IF_STR	*pAsicHwReg,
					ALMMAN_HANDLE 	*AlmManager,														/* <S018> */
					MOTLESSTST 		*MotorLessTest)														/* <S1B7> */

{
	BaseLoops->MencV = MencV;
	BaseLoops->FencV = FencV;	/* <S04D> */
	BaseLoops->Bprm = Bprm;
	BaseLoops->MotSpdMafil = MotSpdMafil;
	BaseLoops->BaseCtrls = BaseCtrls;
	BaseLoops->GselGains = GselGains;
	BaseLoops->pAsicHwReg = pAsicHwReg;																	/* <S004> */
	BaseLoops->AlmManager = AlmManager;																	/* <S018> */
	BaseLoops->MotorLessTest = MotorLessTest;															/* <S1B7> */

	/* キャリア周波数切り替え用変数初期化 */
	MlibResetLongMemory( &(BaseLoops->CarrerChange.var), sizeof(BaseLoops->CarrerChange.var)/4 );
	/* 位相補償値演算用変数初期化 */
	MlibResetLongMemory( &(BaseLoops->PhaseComp.var), sizeof(BaseLoops->PhaseComp.var)/4 );
}



/****************************************************************************************************/
/*																									*/
/*		ScanA Cyclic data input function															*/
/*																									*/
/****************************************************************************************************/
//void SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl, BASE_CTRL_OUT *BaseCtrlOut )
void SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl,
												BASE_CTRL_OUT *BaseCtrlOut, CMD_CTRLBITS *CmdCtrl )	  /* <S050> */
{
	BaseLoops->LastCtrlMode = BaseLoops->CtrlMode;							/* <S007> */
	BaseLoops->CtrlMode = (USHORT)pBaseCtrl->CtrlCmdMngr.CtrlMode;
	BaseLoops->ZCtrlMode = (USHORT)pBaseCtrl->CtrlModeSet.ZctrlMode.zm;

	BaseLoops->CmdCtrlBit = CmdCtrl->CmdCtrlBit; /* <S050> */

//	BaseLoops->dPcmda = pBaseCtrl->CtrlCmdMngr.dPosRefo;
//	pBaseCtrl->CtrlCmdMngr.dPosRefo = 0;
//	LpxSvPositionManager( BaseLoops, &(pBaseCtrl->PosManager) );	/* <S14C> */
//	BaseLoops->SpdRefi = pBaseCtrl->CtrlCmdMngr.SpdRefo;
//	BaseLoops->TrqRefi = pBaseCtrl->CtrlCmdMngr.TrqRefo;

	BaseLoops->SpdFFCx = pBaseCtrl->CtrlCmdMngr.SpdFFCx;
	BaseLoops->TrqFFCx = pBaseCtrl->CtrlCmdMngr.TrqFFCx;
	BaseLoops->SpdFBCx = pBaseCtrl->CtrlCmdMngr.SpdFBCx;
	BaseLoops->TrqFBCx = pBaseCtrl->CtrlCmdMngr.TrqFBCx;

//	BaseLoops->TrqRefs;
//	BaseLoops->TrqClamp = FALSE; /* <S04B> */
	BaseLoops->TrqClamp = pBaseCtrl->CtrlCmdMngr.TrqClamp; /* <S04B> */

	if ( BaseLoops->LastCtrlMode == CTRLMODE_TRQ )       /* <S075> */
	{
		BaseLoops->SpdCtrl.V.SpdRefFilClrCmd = TRUE;     /* 速度指令位相進み補償フィルタ初期化 <S075> */
		BaseLoops->SpdCtrl.V.SpdFFFilClrCmd = TRUE;      /* 速度ＦＦ位相進み補償フィルタ初期化 <S075> */
	}
}



/* 2012.12.27 Y.Oka 暫定対応。後でちゃんと作ること。 */
void SysOutputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut )
{
//	POSCTRL		*PosCtrl;		/* 位置制御用変数 */
	POS_MNG_HNDL *PosMngV;
//	PosCtrl = &BaseLoops->PosCtrl;
	PosMngV = &BaseLoops->BaseCtrls->PosManager;

	CtrlLoopOut->MotPosErr = PosMngV->var.PosErr;	/* 位置アンプ偏差					[Pulse]				*/
	CtrlLoopOut->SpdRefo = BaseLoops->SpdRefo;		/* 速度指令出力						[2^24/OvrSpd]		*/
//	CtrlLoopOut->SpdFFC;							/* 速度FF補償						[2^24/OvrSpd]		*/
	CtrlLoopOut->MotSpd_a = BaseLoops->MotSpd_a;
	CtrlLoopOut->TrqRefo = BaseLoops->TrqRefo;		/* トルク指令出力					[2^24/MaxTrq]		*/
	CtrlLoopOut->TrqRefo_a = BaseLoops->TrqRefo;	/* トルク指令出力(平均値)			[2^24/MaxTrq]		*/
//	CtrlLoopOut->TrqRefMon = BaseLoops->TrqRefo;	/* トルク指令モニタ					[2^24/MaxTrq]		*/
//	CtrlLoopOut->TrqRefMon_a = BaseLoops->TrqRefo;	/* トルク指令モニタ					[2^24/MaxTrq]		*/
	CtrlLoopOut->TrqRefMon = 						/* @@Add トルク指令モニタ [2^24/MaxTrq] */
	MlibMulhigh32( (CtrlLoopOut->IqRefMon << 16), NORM_IQ_TO_TRQ );
	CtrlLoopOut->TrqRefMon_a = 						/* @@Add  トルク指令モニタ(平均値) [2^24/MaxTrq] */
	MlibMulhigh32( (CtrlLoopOut->IqRefMon_a << 16), NORM_IQ_TO_TRQ );

//	CtrlLoopOut->IdRefMon;							/* d軸電流指令モニタ				[15000/Imax]		*/
//	CtrlLoopOut->IqRefMon;							/* q軸電流指令モニタ				[15000/Imax]		*/
//	CtrlLoopOut->IqRefMon_a;						/* q軸電流指令モニタ(平均値)		[15000/Imax]		*/
//	CtrlLoopOut->IqRefMon_l;						/* q軸電流指令モニタ(前回値)		[15000/Imax]		*/
//	CtrlLoopOut->IqFbMon;							/* q軸電流FBモニタ										*/
//	CtrlLoopOut->IdFbMon;							/* d軸電流FBモニタ										*/
//	CtrlLoopOut->IqRefFilMon;						/* q軸電流指令モニタ(フィルタ後)						*/
//	CtrlLoopOut->PConActFlg = FALSE;				/* P制御状態確認フラグ									*//* <S064> */
//	CtrlLoopOut->SvonSpdLmtFlag = FALSE;			/* サーボON時速度制限中フラグ							*//* <S04B> */
	CtrlLoopOut->SvonSpdLmtFlag = BaseLoops->SpdCtrl.V.SvonSpdLmtFlag;	/* サーボON時速度制限中フラグ		*//* <S04B> */
//	CtrlLoopOut->SpdClamp = FALSE;					/* トルク制御時速度制限中フラグ							*//* <S04B> */
//	CtrlLoopOut->TrqClamp = FALSE;					/* トルク制限中フラグ									*//* <S04B> */
}


/****************************************************************************************************/
/*																									*/
/*		サーボ位置管理メイン関数																	*/
/*																									*/
/****************************************************************************************************/
static void	LpxSvPositionManager( BASE_LOOPCTRLS *BaseLoops, POS_MNG_HNDL *PosManager )
{
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令更新（125usに1回位置指令が更新される。それを62.5usで位置制御する為の処置）			*/
/*--------------------------------------------------------------------------------------------------*/
	if( PosManager->RefRenewCntA != PosManager->RefRenewCntB )
	{ /* ScanBで位置指令が更新された後の1回目	*/
		/* 1回目ScanAの位置指令を作成 */
		BaseLoops->dPosRefi = PosManager->CompdPosRefi >> 1;
		BaseLoops->dPcmda   = PosManager->CompdPcmda   >> 1;

		if( PosManager->RefRenewalChkCnt == 0 )
		{ /* 2回目ScanAが走らなかった場合の処置 */
			/* 前回の2回目ScanAの位置指令を加算 */
			BaseLoops->dPosRefi += BaseLoops->NextdPosRefi;
			BaseLoops->dPcmda   += BaseLoops->NextdPcmda;
		}

		/* 2回目ScanAの	位置指令を作成 */
		BaseLoops->NextdPosRefi = PosManager->CompdPosRefi - (PosManager->CompdPosRefi >> 1);
		BaseLoops->NextdPcmda   = PosManager->CompdPcmda   - (PosManager->CompdPcmda   >> 1);

		PosManager->RefRenewalChkCnt = 0;
	}
	else
	{ /* ScanBで位置指令が更新された後の2回目 */
		if( PosManager->RefRenewalChkCnt == 0 )
		{
			/* 2回目ScanAの位置指令を作成 */
			BaseLoops->dPosRefi = BaseLoops->NextdPosRefi;
			BaseLoops->dPcmda   = BaseLoops->NextdPcmda;
		}
		else
		{ /* ScanAが3回以上走った場合の処置 */
			/* 3回目以降は指令の作成を行わない */
			BaseLoops->dPosRefi = 0;
			BaseLoops->dPcmda   = 0;
		}
		PosManager->RefRenewalChkCnt++;
	}
	PosManager->RefRenewCntA = PosManager->RefRenewCntB;

/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差演算(指令単位)																<S14C>	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2012.09.07 Y.Oka ★位置偏差クリア機能現状未対応★ */
//	if( !BoutV.f.PerClrCmd )
//	if((BaseLoops->LastCtrlMode == CTRLMODE_SPD)||(BaseLoops->LastCtrlMode == CTRLMODE_TRQ))/* <S007> */
//	{/* 制御モード切替時	*/
//		BaseLoops->PosCtrl.V.Per64[0] = PosManager->Per64[0];
//		BaseLoops->PosCtrl.V.Per64[1] = PosManager->Per64[1];
//	}
	if( !(BaseLoops->BaseCtrls->CtrlCmdMngr.PerrClrReq) )
	{
#if (FLOAT_USE==TRUE)
		if( PosManager->var.EgearDisable )
		{
			FlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.InvalidEgear, &PosManager->var.PosErrA );
		}
		else
		{
			FlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.Egear, &PosManager->var.PosErrA );
		}
	}
#else
		if( PosManager->var.EgearDisable )
		{
			MlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.InvalidEgear, &PosManager->var.PosErrA );
		}
		else
		{
			MlibPerrcala( BaseLoops->dPcmda, BaseLoops->dPosFbki,
							&PosManager->conf.Egear, &PosManager->var.PosErrA );
		}
	}
#endif
	return;
}



/***************************************** end of file **********************************************/
