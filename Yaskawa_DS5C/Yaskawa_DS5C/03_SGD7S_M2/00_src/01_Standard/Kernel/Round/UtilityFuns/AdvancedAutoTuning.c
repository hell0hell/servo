/****************************************************************************
Description	: Advanced Auto Tuning
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
****************************************************************************/
#include "MercuryGlobals.h"
#include "BaseControlMain.h"
#include "TuneLib.h"
#include "Mlib.h"
#include "KLib.h"
#include "AdvancedAutoTuning.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "ExCtrlPrmCalc.h"
#include "BaseControlPrmCalc.h"
#include "PnPrmListTbl.h"
#include "ModelControl.h"
#include "PosManager.h"
#include "CheckMotSts.h"
#include "FnJatOffLine.h"
#include "GainChange.h"


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
static PRM_RSLT	AdatExecuteAutoTuning( ADATHNDL *AdatHndl, FUNEXE *FunExe );
static PRM_RSLT AdatInitial( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP);

static void	 	AdatSetTuningModeMax( ADATV *AdatV, LONG XfbPulse1, LONG AxisMotType );
static PRM_RSLT AdatModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP );
static LONG		AdatPcalMinDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor );
LONG 	AdatPcalDefautDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor ); /* <S145> */

static PRM_RSLT	AdatRefInModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP );
static LONG		AdatRefInExeModeExeCheck( ADATP *AdatP );
static void 	AdatScanCSetReq( ADATV *AdatV );
static void		AdatScanCRequest( USHORT Request, ADATV *AdatV );

static PRM_RSLT	AdatJratSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status);
static PRM_RSLT	AdatNormVibDetect( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT AdatOptGainSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatNotchFilSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatKpGainSet(ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatOptKpSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatMfcAdjustment( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatFinishOpe( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT err_status );

static void		AdatGainDownforVibration( ADATV *AdatV, ADATP *AdatP, USHORT ser_kv, UCHAR down_ratio);
static SHORT	AdatNotchFilReset( ADATV *AdatV, ADATP *AdatP );
static SHORT 	AdatAresCtrlReset( ADATV *AdatV, ADATP *AdatP );
static SHORT 	TuneAdatSetSpdCtrlGain( LONG mode, ADATP *AdatP);
static SHORT	TuneAdatSetPosCtrlGain( LONG mode, ADATP *AdatP);
static SHORT	TuneAdatSetMdlCtrlGain( LONG mode, ADATP *AdatP );
static void		MfcPrmReSet( ADATV *AdatV, ADATP *AdatP );
static void		Mfc2MassReset( ADATV *AdatV, ADATP *AdatP );
static USHORT	SetMfc2MassSequence( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, ULONG ADAT_MFC );

static void		AdatUndoRamGain( ADATV *AdatV, ADATP *AdatP );
static void		AdatUndoRamGainforJrat( ADATV *AdatV, ADATP *AdatP );
static LONG		AdatUpdateGainInEeprom( ADATREG *AdatReg, ADATP *AdatP );
static SHORT	AdatPrgJogDrive( ADATV	*AdatV, ADATP *AdatP );
static void		AdatPJogInit( ADATV	*AdatV, ADATP *AdatP );
static void		AdatPJogDrvSet( ADATV *AdatV, ADATP *AdatP, ULONG pattern );

static SHORT	LpxAdatVibLvlSet( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatCheckKv( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatSearchKv( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatKpGainSet( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatSearchKp( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatAdjustMfcGain( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatCheckMfcGain( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatRefInTypeDrvStatus( ADATHNDL *AdatHndl, ULONG DrvPattern, BOOL PrefInFlag );


/****************************************************************************/
/*		Start AAT Initialize												*/
/****************************************************************************/
void AdatSetStrPtr( void *Axis )
{
	AXIS_HANDLE *pAxis;
	ADATHNDL *AdatHndl;

	pAxis = ((AXIS_HANDLE*)Axis);
	AdatHndl = pAxis->AdatHndl;

	/* Initialize */
	AdatHndl->AdatP.FnCmnCtrl    = pAxis->FnCmnCtrl;
	AdatHndl->AdatP.UniPrms      = pAxis->UniPrms;
	AdatHndl->AdatP.MencV        = pAxis->MencV;
	AdatHndl->AdatP.BaseCtrlData = pAxis->BaseControls;
	AdatHndl->AdatP.RegManager   = pAxis->RegManager;
	AdatHndl->AdatP.SeqMotSts	 = pAxis->SeqMotSts;
	AdatHndl->AdatP.SeqCtrlOut   = pAxis->SeqCtrlOut;
	AdatHndl->AdatP.AlmManager   = pAxis->AlmManager;
	AdatHndl->AdatP.SettlingTime = &(pAxis->BaseControls->SettlingTime);
	AdatHndl->AdatP.ANotch       = pAxis->ANotch;
	AdatHndl->AdatP.RemVibFreq   = pAxis->RemVibFreq;
	AdatHndl->AdatP.FftAna       = pAxis->FftAna;
	AdatHndl->AdatP.SvAsicRegs   = pAxis->SvAsicRegs;
	AdatHndl->AdatP.MotSpdMafil  = pAxis->MotSpdMafil;
	AdatHndl->AdatP.DetVib       = pAxis->DetVib;
	AdatHndl->AdatP.SvControlIf  = pAxis->SvControlIf;
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode Begin
****************************************************************************/
PRM_RSLT FnEnterAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATREG	 *AdatReg;
	ADATV	 *AdatV;
	ADATP	 *AdatP;
	PRM_RSLT errRes;

	AdatReg	  = &Axis->AdatHndl->AdatReg;
	AdatV	  = &Axis->AdatHndl->AdatV;
	AdatP	  = &Axis->AdatHndl->AdatP;

	/* プログラムJOG運転 変数セット */
	MlibCopyLongMemory( &AdatP->BaseCtrlData->PJogHdl.PJogV,
						&Axis->AdatHndl->OrgPJogV,						/* @@ check */
						sizeof(AdatP->BaseCtrlData->PJogHdl.PJogV)>>2 );

	/* プログラムJOG運転 指令作成変数リセット */
	PrgJogReset( &AdatP->BaseCtrlData->PJogHdl );

	/* アドバンストオートチューニング関連情報クリア	*/
	MlibResetLongMemory( AdatV, sizeof(Axis->AdatHndl->AdatV)>>2 );
	AdatSetTuningModeMax( AdatV, AdatP->UniPrms->Bprm->XfbPulse1, AdatP->MencV->AxisMotType );
	errRes = AdatModeCheck( AdatReg, AdatV, AdatP );

	if( errRes == PRM_RSLT_SUCCESS )
	{
		/* サーボオン中の場合は入らずに終了	*/
		if(AdatP->FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
		
		/* モータレステストモードの場合は入らずに終了 *//* <S1B7> */
		if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
		{
			return(PRM_RSLT_CONDITION_ERR);
		}

		AdatP->FnCmnCtrl->FnSvControl = TRUE;	/* Fnサーボ運転権獲得	*/
		FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */
	}
	AdatV->f.RefInType = FALSE;
	AdatReg->AdatState = ADAT_STS_INIT;			/* ステータス初期化		*/

	return(errRes);
}

/****************************************************************************
	Fn201 Tuning mode Check
****************************************************************************/
static void	AdatSetTuningModeMax( ADATV	*AdatV, LONG XfbPulse1, LONG AxisMotType )
{
	AdatV->TunModeMax = ADAT_LEVEL_MAX;						/* 初期値は最大設定値	*/

		if( AxisMotType == MOTTYPE_ROTARY )					/* 回転形モータ			*/
		{
			//フルク未対応
			//if( Kprm.f.FencUse == FALSE )					/* セミクローズ制御		*/
			{
				if( XfbPulse1 <= 0x2000 )					/* 13bit[p/rev]以下		*/
				{
					AdatV->TunModeMax = ADAT_LEVEL_MIN;		/* 調整モード：標準のみ	*/
				}
			}
		}
		return;
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode Condition Check
****************************************************************************/
static PRM_RSLT AdatModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP )
{
	BASE_CTRL_HNDL *BaseCtrlData;
	PRM_RSLT rc;

	BaseCtrlData = AdatP->BaseCtrlData;
	rc = PRM_RSLT_SUCCESS;

	/* サーボオン中、トルク制御、自動ゲイン切替有効時は、データ異常(REGIF_DAMTHERR)とする。		*/
	/* 偏差レス制御、予測制御、ノーマルオートチューニング有効時も、データ異常であるが、未対応	*/
	if( ((BaseCtrlData->BeComplete == TRUE) && (BaseCtrlData->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ))
	  || (GselGet_AtGselStatus( &BaseCtrlData->GainChange ) == TRUE))
	{
		rc = PRM_RSLT_CONDITION_ERR;
	}

	/* チューニングモード整合性チェック	*/
	if( AdatV->TunModeMax < AdatReg->AdatLevel )
	{
		rc = PRM_RSLT_CONDITION_ERR;
	}

	AdatV->ModeMin    = ADAT_MODE_MIN;
	AdatV->ModeMax    = ADAT_MODE_MAX;
	AdatV->ModeSetMin = ADAT_SCROLL_MIN;
	AdatV->ModeSetMax = ADAT_SCROLL_MAX;

	return( rc );
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode Execution
****************************************************************************/
PRM_RSLT	FnExecuteAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT rc;
	rc = AdatExecuteAutoTuning( Axis->AdatHndl, FunExe );
	return rc;
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode End
****************************************************************************/
void FnLeaveAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATV			*AdatV;
	ADATP			*AdatP;
	REG_ACC_T		AccessPrm;
	TUNELESS_CFGPRM	TuneLessCfgPrm;

	AdatV = &Axis->AdatHndl->AdatV;
	AdatP = &Axis->AdatHndl->AdatP;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* ラウンド制御中のSEN信号ON要求をI/Oの状態で設定する */
	AdatP->FnCmnCtrl->FnSensOnReq = (((AdatP->SeqCtrlOut->RspSeqBits & SENS_ON_COMP_BITMSK)) == TRUE)
																		? TRUE : FALSE;

	AdatP->FnCmnCtrl->State.TuningRun      = FALSE;
	AdatP->FnCmnCtrl->State.TuningPctrlRun = FALSE;

	/* 運転権開放処理(ScanC IF)	*/
	AdatP->FnCmnCtrl->FnSvControl = FALSE;

	/* イナーシャ同定でもKv,JL等を元に戻すのでAdatUndoRamGain()の前であることが必要	*/
	CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );

	MlibCopyLongMemory( &Axis->AdatHndl->OrgPJogV,						/* @@  check */
						&AdatP->BaseCtrlData->PJogHdl.PJogV,
						sizeof( AdatP->BaseCtrlData->PJogHdl.PJogV )>>2 );

	AdatUndoRamGain( AdatV, AdatP );

	DetVibResetMaxMinSpeedErrorFn( AdatP->DetVib );
	DetVibObsCalculateInitPrmAAT( &(AdatP->DetVib->DetVibObs),
	                              AdatP->BaseCtrlData->TuneLessCtrl.conf.TuningLessUse,
	                              AdatP->UniPrms->Bprm->SvCycleNs );

	/* 調整レス機能スイッチ再計算 */
	TuneLessCfgPrm.gnmode	= (AdatP->UniPrms->Prm)->gnmode;
	TuneLessCfgPrm.mfctype	= (AdatP->UniPrms->Prm)->mfctype;
	TuneLessCfgPrm.avibsw	= (AdatP->UniPrms->Prm)->avibsw;
	TuneLessCfgPrm.DatLevel	= (AdatP->UniPrms->Prm)->DatLevel;
	TuneLessCfgPrm.tfuncsw 	= (AdatP->UniPrms->Prm)->tfuncsw;
	TuneLessCfgPrm.flg_wf 	= (AdatP->UniPrms->Prm)->MencP.flg_wf;
	TuneLessCalculateInitPrm( AdatP->BaseCtrlData,
	                          AdatP->UniPrms->Bprm,
	                          AdatP->MencV,
							  AdatP->SvAsicRegs,
						      &TuneLessCfgPrm );

#if 0 /* 2012.08.31 Y.Oka ★モータ速度検出移動平均現状未対応★ */
	/* モータ速度検出用移動平均時定数を戻す	*/
	if( AdatP->BaseCtrlData->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{
		BpiChangeSpdDetMaFil( AdatP->MotSpdMafil,
							  0,
							  (USHORT*)&(AdatP->SvAsicRegs->AsicMicroReg->MREG_SPDMANUMBITI) );
	}
	else
	{
		BpiChangeSpdDetMaFil( AdatP->MotSpdMafil,
							  -1,
							  (USHORT*)&(AdatP->SvAsicRegs->AsicMicroReg->MREG_SPDMANUMBITI) );
	}
#endif

	/* 終了処理	*/
	AdatP->FnCmnCtrl->JogSpeed    = 0;
	AdatP->FnCmnCtrl->FnSvonReq   = FALSE;
	AdatP->FnCmnCtrl->FnCtrlMcmd  = CTRLMODE_NOCMD;
	AdatP->FnCmnCtrl->FnSensOnReq = FALSE;
	AdatP->FnCmnCtrl->FnSvControl = FALSE;

}

/****************************************************************************
	Fn202 Reference Input Type Advanced Auto Tuning mode Begin
****************************************************************************/
PRM_RSLT	FnEnterRefInAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATREG		*AdatReg;
	ADATV		*AdatV;
	ADATP		*AdatP;
	PRM_RSLT 	errRes;

	AdatReg   = &Axis->AdatHndl->AdatReg;
	AdatV     = &Axis->AdatHndl->AdatV;
	AdatP     = &Axis->AdatHndl->AdatP;
	errRes    = PRM_RSLT_SUCCESS;

	/* アドバンストオートチューニング関連情報クリア */
	MlibResetLongMemory( AdatV, sizeof(Axis->AdatHndl->AdatV)>>2 );

	/* 調整モード最大値設定 */
	AdatSetTuningModeMax( AdatV, AdatP->UniPrms->Bprm->XfbPulse1, AdatP->MencV->AxisMotType );

	AdatV->f.RefInType = TRUE;
	errRes = AdatRefInModeCheck(AdatReg, AdatV, AdatP);

	/* モータレステストモードの場合は入らずに終了 *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return(PRM_RSLT_CONDITION_ERR);
	}

	AdatReg->AdatState = ADAT_STS_INIT;	/* ステータス初期化		*/
	FunExe->HoldFnMode = TRUE;			/* Fnモード自動終了なし */

	return(errRes);
}


/****************************************************************************
	Fn202 Advanced Auto Tuning mode Condition Check
****************************************************************************/
static PRM_RSLT	AdatRefInModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP )
{
	BASE_CTRL_HNDL *BaseCtrlData;
	PRM_RSLT errRes;

	BaseCtrlData = AdatP->BaseCtrlData;
	errRes = PRM_RSLT_SUCCESS;

	/* トルク制御、速度制御でかつサーボオン中、または自動ゲイン切替有効時は、データ異常(REGIF_DAMTHERR)とする。*/
	/* 偏差レス制御、予測制御、ノーマルオートチューニング有効時も、データ異常であるが、未対応	*/
	if( ((BaseCtrlData->BeComplete == TRUE) && (BaseCtrlData->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS))
	  || (GselGet_AtGselStatus( &BaseCtrlData->GainChange ) == TRUE)
 	  || (BaseCtrlData->TuneLessCtrl.conf.TuningLessUse  == TRUE) )
	{
		errRes = PRM_RSLT_CONDITION_ERR;
	}

	/* チューニングモード整合性チェック	*/
	if(AdatV->TunModeMax < AdatReg->AdatLevel)
	{
		errRes = PRM_RSLT_CONDITION_ERR;
	}

	AdatV->ModeMin = ADAT_MODE_MIN;
	AdatV->ModeMax = ADAT_MODE_MAX;
	AdatV->ModeSetMin = ADAT_REFIN_SCROLL_MIN;
	AdatV->ModeSetMax = ADAT_REFIN_SCROLL_MAX;

	return(errRes);
}

/****************************************************************************
	Fn202 Advanced Auto Tuning mode Execution
****************************************************************************/
PRM_RSLT	FnExecuteRefInAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT rc;
	rc = AdatExecuteAutoTuning( Axis->AdatHndl, FunExe );
	return rc;
}


/****************************************************************************
	Fn202 Reference Input Type Advanced Auto Tuning mode End
****************************************************************************/
void FnLeaveRefInAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATV	*AdatV;
	ADATP	*AdatP;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	AdatV	   = &Axis->AdatHndl->AdatV;
	AdatP	   = &Axis->AdatHndl->AdatP;
	FnCmnCtrl  = Axis->FnCmnCtrl;

	FnCmnCtrl->State.TuningRun      = FALSE;
	FnCmnCtrl->State.TuningPctrlRun = FALSE;

	AdatUndoRamGain( AdatV, AdatP );

	/* 振動検出用差速度最大値・最小値リセット */
	DetVibResetMaxMinSpeedError( AdatP->DetVib );

	/* 振動検出用差速度最大値・最小値リセット */
	DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs,
	                         VIBOBSGN, 0, DETVIBHP, (AdatP->UniPrms->Bprm)->SvCycleNs);

	/* 終了処理	*/
	FunExe->HoldFnMode = FALSE;				/* Fnモード自動終了なし */
}


/****************************************************************************
	Advanced Auto Tuning  Execution (Fn201,Fn202 Common)
****************************************************************************/
static PRM_RSLT	AdatExecuteAutoTuning( ADATHNDL *AdatHndl, FUNEXE *FunExe )
{
	ADATREG	*AdatReg;
	ADATV	*AdatV;
	ADATP	*AdatP;
	REG_ACC_T AccessPrm;

	PRM_RSLT rc;
	ULONG	 data;
	LONG	 UnRvibFrequency;

	AdatReg	  = &AdatHndl->AdatReg;
	AdatV	  = &AdatHndl->AdatV;
	AdatP	  = &AdatHndl->AdatP;

	rc  = PRM_RSLT_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/*------------------------------------------------------------------------------------------*/
	/*	共通操作処理																			*/
	/*------------------------------------------------------------------------------------------*/
	if( AdatV->f.DrvErrDetect == TRUE )
	{
		if(   (AdatP->AlmManager->AlmCode != 0x00)
			|| (AdatP->SeqCtrlOut->MainPowerOn == FALSE)
			|| (AdatP->SeqCtrlOut->OverTrvlSts == TRUE)
			|| ((AdatV->f.RefInType == TRUE) && (AdatP->FnCmnCtrl->State.TuningRun == TRUE) && (AdatP->SeqCtrlOut->BaseEnableReq == FALSE))
			|| (AdatP->BaseCtrlData->GainChange.ActGain != 1)
			|| (AdatP->SeqCtrlOut->HwbbSts == TRUE) ) /* GAIN1以外 もしくは HWBB状態			*/
		{
			AdatV->ErrState = ADAT_NOTRDYERR;			/* 運転準備未完エラー 					*/
			FunExe->State   = ADAT_SEQ_END;				/* 最終設定状態	    					*/
		}
		else if( AdatV->ErrState <= ADAT_NG )
		{
			FunExe->State = ADAT_SEQ_END;
		}
	}

	switch ( FunExe->CmdCode )
	{
		case FCMD_SV :
			if( AdatV->f.RefInType == FALSE )
			{
//				if( (AdatP->AlmManager->AlmReady    == TRUE)		/* アラームあり 		*//* <S085> */
				if( (AdatP->AlmManager->AlmReady    == FALSE)		/* アラームあり 		*/
				 || (AdatP->SeqCtrlOut->HwbbSts     == TRUE)		/* HWBBあり				*/
//				 || (AdatP->SeqCtrlOut->MainPowerOn == TRUE)		/* 主回路OFF			*//* <S085> */
				 || (AdatP->SeqCtrlOut->MainPowerOn == FALSE)		/* 主回路OFF			*/
				 || (AdatP->SeqCtrlOut->OverTrvlSts == TRUE) )		/* OTあり				*/
				{
					;
				}
				else
				{
//					if( AdatP->FnCmnCtrl->FnSvonReq == TRUE )
					if( AdatP->FnCmnCtrl->FnSvonReq != FALSE )		/* <S085> */
					{
						AdatP->FnCmnCtrl->FnSvonReq = FALSE;
						FunExe->State = ADAT_SEQ_INIT;
					}
				}
			}
			break;

		case FCMD_STOP :
			AdatV->ErrState = ADAT_COMERR;
			FunExe->State = ADAT_SEQ_END;
			break;

		case FCMD_UP :
		case FCMD_DOWN :
		default :
			break;
	}

	switch ( FunExe->State )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	初期状態																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_INIT :
			AdatP->FnCmnCtrl->State.TuningRun = FALSE;

			if( (AdatV->f.RefInType == FALSE)	 					/* 内部指令型				*/
			 || (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)/* COIN						*/
			 || (AdatP->SeqCtrlOut->MotStop == TRUE) )				/* モータ停止検出 			*/
			{	/* 指令停止判定	*/
				FunExe->State = ADAT_SEQ_START;				/* モータ停止後、初期化処理へ		*/
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	初期化処理状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_START :
			AdatV->f.GainTuneRun = FALSE;

			/* アドバンストAT初期化処理	*/
			rc = AdatInitial( AdatReg, AdatV, AdatP );
			TuneFilGainCalbyFilType( AdatP->ANotch, (UCHAR)AdatReg->AdatFilType );
			/*----------------------------------------------------------------------------------*/
			/*	ゲインの初期化																	*/
			/*----------------------------------------------------------------------------------*/
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );				/* 現設定Kvとバランス		*/
			data = ADAT_GRAT_50( AdatP->UniPrms->Prm->loophz );		/* Kvを1/2					*/
			TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)data, pndef_posgn.DataLength );

			/* チューニングモード設定は通信で行われるためイナーシャ設定へ */
			FunExe->State = ADAT_SEQ_JRATSET;

			//補助機能実行時のデータ書込み禁止設定関数
			//RpiFunDataWriteDisableSet( FPRM_IF, ON, pfexe );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	イナーシャ設定状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_JRATSET :
			AdatV->f.GainTuneRun = TRUE;

			/* アドバンストATイナーシャ設定処理 */
			rc = AdatJratSet( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	通常振動レベル検出＆負荷トルク測定状態												*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_NORMVIB:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = TRUE;
			AdatReg->AdatState = ADAT_STS_VIBDET; /* 振動レベル測定実行中						*/

			/* アドバンストAT通常振動レベル検出処理	*/
			rc = AdatNormVibDetect( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	最適Kv、Tiゲイン探索状態															*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_SEARCHOPTKV:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = TRUE;
			AdatReg->AdatState = ADAT_STS_OPTGNADJ; /* 動作評価ゲイン探索実行中					*/

			/* アドバンストAT最適ゲイン探索処理 */
			rc = AdatOptGainSearch( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	ノッチフィルタ設定状態																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_NOTCHSET:
			AdatV->f.GainTuneRun = TRUE;
			AdatReg->AdatState = ADAT_STS_VIBDISSET; /* 振動抑制設定実行中						*/

			/* アドバンストATノッチフィルタ設定処理	*/
			rc = AdatNotchFilSet( AdatReg, AdatV, AdatP, FunExe, AdatV->FftStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kp設定状態																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_KPSET:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = TRUE;
			AdatReg->AdatState = ADAT_STS_OPTGNADJ; /* 動作評価ゲイン探索実行中 				*/

			/* アドバンストAT Kp設定処理 */
			rc = AdatKpGainSet( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	最適Kpゲイン探索状態																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_SEARCHOPTKP:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = FALSE;
			/* 動作評価ゲイン探索実行中	*/
			AdatReg->AdatState = ADAT_STS_OPTGNADJ;
			/* アドバンストAT最適Kpゲイン探索処理	*/
			rc = AdatOptKpSearch( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	モデル追従制御調整状態																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_ADJMFC:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = FALSE;
			/* 動作評価ゲイン探索実行中 */
			AdatReg->AdatState = ADAT_STS_OPTGNADJ;
			/* アドバンストATモデル追従制御調整処理	*/
			rc = AdatMfcAdjustment( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	終了表示状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_ENDDISP:
			FunExe->State = ADAT_SEQ_END;
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	最終設定状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_END:
			/* アドバンストAT終了設定処理 */
			rc = AdatFinishOpe( AdatReg, AdatV, AdatP, FunExe, AdatV->ErrState );
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	終了待ち状態																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_WAIT:
			AdatP->FnCmnCtrl->State.TuningRun = FALSE;
			break;
#if 0
		//Σ-Ⅴ-MDでは使用しない
		/*--------------------------------------------------------------------------------------*/
		/*	後処理状態																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_RETURN:
			if( AdatV->f.RefInType == TRUE )
			{
				FnLeaveRefInAdvAutoTuning( FunExe, Axis );
			}
			else
			{
				FnLeaveAdvAutoTuning( FunExe, Axis);
			}
			break;

		case ADAT_SEQ_NOOPWAIT :
			if( KlibGetLongTimerMs( FunExe->LongTimer ) > 1000 )
			{
				FunExe->State = ADAT_SEQ_RETURN;
			}
			break;
#endif
		/*--------------------------------------------------------------------------------------*/
		/*	MFC(剛体+機台モデル)設定状態														*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_2MASSSET:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = FALSE;

			AdatReg->AdatState = ADAT_STS_VIBDISSET;		/* MFC(剛体+機台モデル)設定実行中 	*/

			/* MFC(剛体+機台モデル)設定済みフラグON 											*/
			/* 振動検出して2慣性MFCを有効にしたらAdatV->f.Used2MassMfc = TRUE					*/
			AdatV->f.Used2MassMfc = TRUE;

			/* Kpmのデフォルトは反共振周波数にする。Kpm [1/s] = wL / 2 [Hz]	*/
			/* もし，反共振周波数が1.0Hz以下だったらKpm=1.0[1/s]			*/
			UnRvibFrequency = RemVibGetRemVibFreq( AdatP->RemVibFreq );

			if( UnRvibFrequency <= VIBMFC_DET_FRQ_MIN )
			{
				data = VIBMFC_DET_FRQ_MIN;
			}
			else
			{
				data = ( UnRvibFrequency >> 1 );
			}
			/* Pn141 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)data, pndef_mdlgn.DataLength );

			/* 振動周波数を設定，MFC(剛体+機台モデル)をONにする */
			data = UnRvibFrequency;
			/* Pn145 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlantfrq, data, pndef_mdlantfrq.DataLength );
			/* Pn146 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, data, pndef_mdlresfrq.DataLength );
			PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
			data = AdatP->UniPrms->Prm->mdlsw | VIBMFC_PRM_ACTIVE;
			/* Pn140 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, data, pndef_mdlsw.DataLength );
			FunExe->State = AdatV->NextSeqPtr;				/* 状態ポインタを元に戻す 			*/

			break;
		default:
			break;
	}
	/*------------------------------------------------------------------------------------------*/
	/*	プログラムJOG指令シーケンス																*/
	/*------------------------------------------------------------------------------------------*/
//	if(FunExe->State != ADAT_SEQ_RETURN)
	{
		if( AdatV->f.RefInType == FALSE )
		{
			AdatV->PrgJogStat = AdatPrgJogDrive( AdatV, AdatP );
			if( AdatV->PrgJogStat != ADAT_PJOG_NOP )
			{	/* 運転実行ステータスがNOP以外 → 運転中 */
				switch ( FunExe->CmdCode )
				{
					case FCMD_UP:		/* Forward revolution (継続実行) 						*/
						if( AdatV->MoveDistance > 0 )
						{
							KlibRstLongTimer( &AdatV->PrgJogDrvCntr );
						}
						break;

					case FCMD_DOWN:		/* Reverse revolution (継続実行) 						*/
						if( AdatV->MoveDistance < 0 )
						{
							KlibRstLongTimer( &AdatV->PrgJogDrvCntr );
						}
						break;

					case FCMD_NONE:		/* 命令なし												*/
					default:
						if( (KlibGetLongTimerMs( AdatV->PrgJogDrvCntr ) > (ULONG)(AdatP->FnCmnCtrl->SafeStopTime * 1000)) /* <S195> */
															&& (AdatP->FnCmnCtrl->SafeStopTime != 65535) )
						{	/* 安全停止時間[s]以上で停止 */
							AdatP->BaseCtrlData->PJogHdl.PJogV.Var.RoutCmd = PJOGCMD_ABORT; /* 運転中断 */
							AdatV->ErrState = ADAT_COMERR;									/* 通信異常	*/
						}
						break;
				}
			}
			else
			{	/* 運転停止中	*/
				if(AdatV->PrgJogStat < ADAT_PJOG_NOP )
				{/* 異常設定ならエラーステータスを設定	*/
					AdatV->ErrState = AdatV->PrgJogStat;
				}

				if( AdatV->f.DrvErrDetect == FALSE )
				{/* 運転停止中は無条件に通信異常検出ウエイトタイマ初期化 */
					KlibRstLongTimer( &AdatV->PrgJogDrvCntr );
				}
			}
		}
	}
	/*------------------------------------------------------------------------------------------*/
	/*	周波数解析処理シーケンス																*/
	/*------------------------------------------------------------------------------------------*/
	AdatV->FftStat = TuneVibrationTrace( AdatP->DetVib, AdatP->FftAna, AdatV->f.FftReq, 0,  FALSE);

	/*------------------------------------------------------------------------------------------*/
	/*	COIN出力ステータス作成																	*/
	/*------------------------------------------------------------------------------------------*/
	AdatReg->AdatState |= AdatV->CoinWaitStat;						/* COIN出力ステータス		*/

	return rc;
}

/****************************************************************************
	Fn202 Advanced Auto Tuning mode Condition Check
****************************************************************************/
static LONG	AdatRefInExeModeExeCheck( ADATP *AdatP )
{
	LONG rc;
	rc = TRUE;

//	if(((AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS) && (AdatP->BaseCtrlData->BeComplete == TRUE))
	if((AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS) || (AdatP->BaseCtrlData->BeComplete == FALSE)	/* <S085> */
	 || (AdatP->AlmManager->AlmCode != 0x00)
	 || (AdatP->SeqCtrlOut->HwbbSts == TRUE)
	 || (AdatP->SeqCtrlOut->MainPowerOn == FALSE)
	 || (AdatP->SeqCtrlOut->OverTrvlSts == TRUE) )
	{
		rc = FALSE;
	}
	return rc;
}

/****************************************************************************
	Fn201 Update One Parameter Tuning Gain In EEPROM
****************************************************************************/
static LONG	AdatUpdateGainInEeprom( ADATREG *AdatReg, ADATP *AdatP )
{
	PRM_RSLT		rc;
	REGIF_CONFIG_T *RegCfg;
	DOBS_CTRL	   *DobsCtrl;
	MFCTRL		   *MFControl;

//	rc        = PRM_RSLT_SUCCESS;/*<S09E>*/
	RegCfg    = AdatP->RegManager->hPnReg;
	DobsCtrl  = &AdatP->BaseCtrlData->DobsCtrl;
	MFControl = &AdatP->BaseCtrlData->MFControl;

	/* Pn100 速度ループゲイン */
	rc = PrmWriteValueToEeprom( RegCfg, &pndef_loophz, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn101 速度ループ積分時定数 */
	rc = PrmWriteValueToEeprom( RegCfg, &pndef_pitime, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn102 位置ループゲイン */
	rc = PrmWriteValueToEeprom( RegCfg, &pndef_posgn, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn103 慣性モーメント比 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_jrate, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn401 1段目第1トルク指令フィルタ時定数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn409 1段目ノッチフィルタ周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40A 1段目ノッチフィルタQ値 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40B 1段目ノッチフィルタ深さ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchdamp1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40C 2段目ノッチフィルタ周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40D 2段目ノッチフィルタQ値 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn408 トルク関係機能スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	if( DobsCtrl->var.DobsAct == TRUE )
	{
		/* Pn121 摩擦補償ゲイン */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn123 摩擦補償係数 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dtrqgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn125 摩擦補償ゲイン補正 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobjgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
	}

	if(   ( AdatReg->AdatLevel == ADAT_LEVEL_POS )
	   || ( AdatReg->AdatLevel == ADAT_LEVEL_POS_ST ) )
	{

		/* Pn141 モデル追従制御ゲイン */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn142 モデル追従制御ゲイン補正 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlzeta, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn143 モデル追従制御バイアス(正転方向) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlptff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn144 モデル追従制御バイアス(逆転方向) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlntff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		if(MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS)
		{	/* 2慣性MFC有効	*/
			/* Pn145 振動抑制１周波数A */
			rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlantfrq, 0 );
			if( rc != PRM_RSLT_SUCCESS )
			{
				return(rc);
			}

			/* Pn146 振動抑制１周波数B */
			rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlresfrq, 0 );
			if( rc != PRM_RSLT_SUCCESS )
			{
				return(rc);
			}
		}

		/* Pn147 モデル追従制御速度フィードフォワード補償 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlvff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
	}

	/* Pn140 モデル追従制御関連スイッチ */
	/* AAT実行前MFC有効で”Level=1”のときはPn140=xx00をEEPROMに保存する必要がある */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn161 Ａ型制振周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfrq, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn163 Ａ型制振ダンピングゲイン */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibdamp, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn160 制振制御関連スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn170 ダイナミックオートチューニング関連スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_DatLevel, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	return(rc);
}

/****************************************************************************
		アドバンストオートチューニングパラメータUNDO処理

	概要:	パラメータを機能実行前のRAM状態に戻す。
****************************************************************************/
static void	AdatUndoRamGain( ADATV *AdatV, ADATP *AdatP )
{
	REGIF_CONFIG_T *RegCfg;
	RegCfg   = AdatP->RegManager->hPnReg;

	/* Pn100 速度ループゲイン */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_loophz,	0 );

	/* Pn101 速度ループ積分時定数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pitime,	0 );

	/* Pn102 位置ループゲイン */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_posgn,		0 );

	/* Pn103 慣性モーメント比 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_jrate,		0 );

	/* Pn401 1段目第1トルク指令フィルタ時定数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_trqfil11,	0 );

	/* Pn409 1段目ノッチフィルタ周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1,	0 );

	/* Pn40A 1段目ノッチフィルタQ値 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1,	0 );

	/* Pn40B 1段目ノッチフィルタ深さ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchdamp1, 0 );

	/* Pn40C 2段目ノッチフィルタ周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2,	0 );

	/* Pn40D 2段目ノッチフィルタQ値 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2,	0 );

	/* Pn408 トルク関係機能スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw,	0 );

	/* Pn121 摩擦補償ゲイン */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobgn,		0 );

	/* Pn123 摩擦補償係数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dtrqgn,	0 );

	/* Pn125 摩擦補償ゲイン補正 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobjgn,	0 );

	/* Pn141 モデル追従制御ゲイン */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlgn,		0 );

	/* Pn142 モデル追従制御ゲイン補正 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlzeta,	0 );

	/* Pn143 モデル追従制御バイアス(正転方向) */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlptff,	0 );

	/* Pn144 モデル追従制御バイアス(逆転方向) */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlntff,	0 );

	/* Pn145 振動抑制１周波数A */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlantfrq, 0 );

	/* Pn146 振動抑制１周波数B */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlresfrq, 0 );

	/* Pn147 モデル追従制御速度フィードフォワード補償 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlvff,	0 );

	/* Pn140 モデル追従制御関連スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlsw,		0 );

	/* Pn161 A型制振周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq,	0 );

	/* Pn163 A型制振ダンピングゲイン */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp,	0 );

	/* Pn160 制振制御関連スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibsw,	0 );

	/* Pn310 振動検出スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_vibsel,	0 );

	/* Pn311 振動検出感度 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_vibsens,	0 );

	/* Pn533 プログラムJOG移動速度 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogrotspd,0 );

	/* Pn534 プログラムJOG加減速時間 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogacctm, 0 );

	/* Pn531 プログラムJOG移動距離 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogdistl, 0 );

	/* Pn535 プログラムJOG待ち時間 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogwaittm,0 );

	/* Pn585 プログラムJOG最高速度(リニア) */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjoglinspd,0 );

	/* Pn560 残留振動検出幅 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_remdetw,	0 );

	/* Pn170 ダイナミックオートチューニング関連スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_DatLevel,	0 );

	AdatUndoRamGainforJrat( AdatV, AdatP );

}

/****************************************************************************
		アドバンストオートチューニング 慣性モーメント推定UNDO処理

	概要:	慣性モーメント推定関連のパラメータを機能実行前のRAM状態に戻す。
****************************************************************************/
static void	AdatUndoRamGainforJrat( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA 		*Prm;
	REGIF_CONFIG_T  *RegCfg;

	Prm      = AdatP->UniPrms->Prm;
	RegCfg   = AdatP->RegManager->hPnReg;
	AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl = (AdatV->f.OrgIPSpdControl)? TRUE : FALSE;

	/* 位置指令フィルタ起動 */
	BpiPcmdFilSwitch( &AdatP->BaseCtrlData->PosManager, TRUE );

	/* MFC強制無効解除	 	*/
	MfcSetMfcCtrlSwitch( &AdatP->BaseCtrlData->MFControl, TRUE );

	/*----------------------------------------------------------------------*/
	/*	自動Ａ型設定可否判断												*/
	/*		イナーシャ同定有効時はＡ型強制OFFされるため、					*/
	/*		再度、A型自動調整有効判定を行う									*/
	/*----------------------------------------------------------------------*/
	if( ( ((Prm->avibsw >> 4) & 0x0F)  == FALSE) 	/* 自動調整無効			*/
		|| ((Prm->avibsw & 0x0F) == TRUE) )			/* または、A型既に有効	*/
	{
		AdatV->f.UsedAResCtrl = TRUE;				/* 設定済み				*/
	}
	else
	{
		AdatV->f.UsedAResCtrl = FALSE;				/* 未設定				*/
	}

	/* Pn109 フィードフォワード */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ffgn, 0 );

	/* Pn408 トルク関係機能スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( Prm->tfuncsw & 0xF000 )
	{
		Prm->tfuncsw = Prm->tfuncsw | 0x1000;
		PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	}

	/* Pn10B ゲイン関係応用スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_gnmode, 0 );

}

/****************************************************************************
		アドバンストオートチューニング動作指令作成処理

	概要:	プログラムJOGを利用して，位置指令を作成する。
        	本体は，プログラムJOGを利用
			運転停止要求があっても、運転パターンは、一通り実行する。
			プログラムJOG指令完了(DEN)後の待ち時間は、300msとする
			繰り返し回数は、プログラムJOGのPJogP->Timesでなく、LEVEL設定に従って
			アドバンストオートチューニング独自に管理する。
****************************************************************************/
static SHORT	AdatPrgJogDrive( ADATV *AdatV, ADATP *AdatP )
{
	PJOGV	*PJogV;
	SHORT	rc;

	PJogV = &AdatP->BaseCtrlData->PJogHdl.PJogV;
	rc    = AdatV->PrgJogStat; 					/* リターンコードを前回ステータスに初期化 	*/

	if( (PJogV->Var.State == PJOG_ABORT) || (PJogV->Var.State == PJOG_ERROR)
	  || (AdatV->PrgJogPattern == ADAT_PJOG_STOP) )
	{	/* プログラムJOG運転中止 */
		AdatV->PrgJogPtr = 0;					/* 運転停止。シーケンスを初期設定にセット	*/
		rc = ADAT_PJOG_NOP;						/* 運転停止状態へ							*/
	}

	switch ( AdatV->PrgJogPtr )
	{
		/*----------------------------------------------------------------------------------*/
		/*	Initial process of Jog sequence 												*/
		/*----------------------------------------------------------------------------------*/
		case 0:
			AdatPJogInit( AdatV, AdatP );
			AdatV->PrgJogPtr = 1;
			AdatV->PrgJogPattern = AdatV->PrgJogPatternReq;
			break;

		case 1:
			AdatV->PrgJogActPattern = (AdatV->PrgJogPattern & 0x0000000F);

			if( AdatV->PrgJogActPattern == ADAT_PJOG_STOP )
			{	/* 運転停止要求あり	*/
				rc = ADAT_PJOG_NOP;							/* 運転停止状態へ				*/
			}
			else
			{
				AdatV->PrgJogPatternCnt = 0;
				AdatPJogDrvSet( AdatV, AdatP, AdatV->PrgJogActPattern );/* 運転パターン設定	*/
				AdatV->PrgJogPtr = 2;						/* シーケンスポインタ更新		*/
				rc = ADAT_PJOG_EXEC;
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	運転開始処理																	*/
		/*----------------------------------------------------------------------------------*/
		case 2:
			BpiRsetCoinOffTime( AdatP->SettlingTime );
			BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* オーバシュート量最大値クリア		*/

			if( AdatV->f.GainTuneRun == TRUE )
			{	/* チューニング中は，一旦，クリアする */
				DetVibResetMaxMinSpeedError( AdatP->DetVib );
			}

			PJogV->Var.RoutCmd = PJOGCMD_START;
			KlibRstLongTimer( &AdatV->PrgJogMovStartCntr );
			AdatV->PrgJogPtr = 3;							/* 移動完了待ちへ				*/
			rc = ADAT_PJOG_EXEC;
			break;

		/*----------------------------------------------------------------------------------*/
		/*	移動指令完了待ち																*/
		/*----------------------------------------------------------------------------------*/
		case 3:
			if( (PJogV->Var.RoutCmd == PJOGCMD_START) && (PJogV->Var.State == PJOG_START) )
			{
				PJogV->Var.RoutCmd = PJOGCMD_NONE;
			}

			if( KlibGetLongTimerMs( AdatV->PrgJogMovStartCntr ) > KPI_SCCYCLEMS * 2 )
			{	/* ScanCでPJOGCMD_STARTをセットするため、ScanCが必ず1回走る時間だけ待つ 	*/
				if( (PJogV->Var.State == PJOG_ABORT) || (PJogV->Var.State == PJOG_END)
				 || (PJogV->Var.State == PJOG_ERROR) )
				{
					PJogV->Var.RoutCmd = PJOGCMD_NONE;
					KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
					KlibRstLongTimer( &AdatV->CoinWaitCntr );
					AdatV->CoinWaitStat = ADAT_STS_COINWAIT;
					AdatV->PrgJogPtr = 4;					/* 運転完了待ちへ				*/
				}
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	運転完了待ち																	*/
		/*----------------------------------------------------------------------------------*/
		case 4:
			if( AdatV->f.CoinIgnore == TRUE )
			{
				if( PJogV->Cnst.RevDir == FALSE)
				{
					rc = ADAT_PJOG_PLUSEND;
				}
				else
				{
					rc = ADAT_PJOG_MINUSEND;
				}
				AdatV->PrgJogPtr = 5;						/* ウエイトへ					*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				AdatV->CoinWaitStat = ADAT_STS_COINNOWAIT;
			}
			else if( AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE ) /* COINラッチ完了 */
			{
				if( PJogV->Cnst.RevDir == FALSE)
				{
					rc = ADAT_PJOG_PLUSEND;
				}
				else
				{
					rc = ADAT_PJOG_MINUSEND;
				}
				AdatV->PrgJogPtr = 5;					 	/* ウエイトへ					*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				AdatV->CoinWaitStat = ADAT_STS_COINNOWAIT;
			}
			else if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT1 )
			{	/* 約10秒位置決め完了しないときはCOIN待ちエラー	*/
				AdatV->ErrState = ADAT_COINERR;
				AdatV->CoinWaitStat = ADAT_STS_COINERR;		/* エラー出力ON					*/
			}
			else if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT2 )
			{	/* COIN 出力待ち警告中	*/
				AdatV->CoinWaitStat = ADAT_STS_COINWARN;	/* 警告出力						*/
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	ウエイト																		*/
		/*----------------------------------------------------------------------------------*/
		case 5:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr )
											>=	(ULONG)((PJogV->Cnst.WaitTime + AATPJOGWAITTM)) )
			{	/* 運転完了		*/
				AdatV->PrgJogPtr = 6;					/* 次回運転準備へ					*/
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	次回運転準備																	*/
		/*----------------------------------------------------------------------------------*/
		case 6:
			AdatV->PrgJogPatternCnt++;
			AdatV->PrgJogActPattern = ((AdatV->PrgJogPattern
											>> (4 * AdatV->PrgJogPatternCnt)) & 0x0000000F);

			if( AdatV->PrgJogActPattern == 0x0F)
			{	/* 運転繰り返し	*/
				/* 繰り返し処理	*/
				if( ++AdatV->PrgJogDrvCnt >= AdatV->PrgJogTimes )
				{	/* 繰り返し回数実行した	*/
					AdatV->PrgJogDrvCnt = 0;				/* 繰り返しカウンタ初期設定		*/
					AdatV->PrgJogPatternCnt++;
				}
				else
				{	/* 繰り返し回数未満		*/
					AdatV->PrgJogPatternCnt--;
				}
				AdatV->PrgJogActPattern = ((AdatV->PrgJogPattern
											>> (4 * AdatV->PrgJogPatternCnt)) & 0x0000000F);
			}

			/* シーケンス設定	*/
			if( AdatV->PrgJogActPattern == 0x00 )
			{	/* 運転完了	*/
				AdatV->PrgJogPtr = 7;			/* 運転停止。シーケンスを初期設定にセット 	*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				rc = ADAT_PJOG_FIN;				/* 運転完了ステータスセット					*/
			}
			else
			{	/* 運転継続	*/
				AdatPJogDrvSet( AdatV, AdatP, AdatV->PrgJogActPattern );/* 運転パターン設定	*/
				AdatV->PrgJogPtr = 2;							/* シーケンスポインタ更新	*/
				rc = ADAT_PJOG_EXEC;
			}
			break;

		case 7:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr )
										>= (ULONG)((PJogV->Cnst.WaitTime + AATPJOGWAITTM) / 2 ))
			{	/* 運転完了	*/
				AdatV->PrgJogPtr = 0;						/* 次回運転準備へ				*/
			}
			break;

		default:
			AdatV->ErrState = ADAT_PROGERR;					/* 異常終了						*/
			break;
	}

	return rc;
}



/****************************************************************************
	アドバンストオートチューニング プログラムJOG初期化処理
****************************************************************************/
static void	AdatPJogInit( ADATV	*AdatV, ADATP *AdatP )
{
	PJOGHNDL *PJogHdl;
	FUN_CMN_CONTROL *FnCmnCtrl;

	PJogHdl = &AdatP->BaseCtrlData->PJogHdl;
	FnCmnCtrl = AdatP->FnCmnCtrl;

	PJogHdl->PJogV.Var.RoutCmd = PJOGCMD_INIT;
	PrgJogReset( PJogHdl );

	/* 移動回数 */
	PJogHdl->PJogV.Cnst.MTimes = 1;		/* 移動回数は１回に固定			*/
	AdatV->PrgJogTimes  = 3;			/* プログラム繰り返し回数設定	*/
	AdatV->PrgJogDrvCnt = 0;			/* 繰り返しカウンタ初期設定		*/
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_PJOG;
}

/****************************************************************************
	アドバンストオートチューニング プログラムJOGパラメータ設定処理

	概要:	運転パターンに従って、プログラムJOG関連の
			移動距離、速度、加速度、待ち時間、繰り返し回数を設定する。
****************************************************************************/
static void	AdatPJogDrvSet( ADATV *AdatV, ADATP *AdatP, ULONG pattern )
{
	LONG	sx;
	LONG	kx;
	LONG	jratex;
	LONG	wk_spd;
	LONG	trq_per;						/* 定格100%トルク						*/
	CHAR	pjog_switch;					/* 加速選択スイッチ						*/
	LONG	baseVel_wk;						/* 基準速度[min-1] DDモータ[0.1min-1]	*/
	LONG	VelData;
	LONG	AccData;
	LONG	AccGain;

	PJOGV	*PJogV;
	JATHNDL	*JatHdl;
	PRMDATA *Prm;
	BPRMDAT	*Bprm;
	REG_ACC_T AccessPrm;

	PJogV  = &AdatP->BaseCtrlData->PJogHdl.PJogV;
	JatHdl = &AdatP->BaseCtrlData->JatHdl;
	Prm    =  AdatP->UniPrms->Prm;
	Bprm   =  AdatP->UniPrms->Bprm;

	trq_per = TUNEACCTRQ2;
	jratex = Prm->jrate + 100;
	pjog_switch = 0;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( pattern )
	{
		case 0x03:		/* 通常送り正転					*/
		case 0x04:		/* 通常送り逆転					*/
			/* ストローク設定	[指令単位]				*/
			/* [1000指令単位]→[指令単位]変換=125倍		*/
			PJogV->Cnst.Distance = MlibAbs32( AdatV->MoveDistance ) / 8;

			/* 速度設定			[min-1]					*/
			/* プログラムJOG速度に対する倍率=1倍		*/
			wk_spd = 1;

			/* 待ち時間設定		[ms]					*/
			/* プログラムJOG開始待ち時間=300ms+300ms	*/
			PJogV->Cnst.WaitTime = AATPJOGWAITTM;
			AdatV->f.CoinIgnore = FALSE;
			break;

		case 0x06:		/* イナーシャ同定早送り正転		*/
		case 0x07:		/* イナーシャ同定早送り逆転		*/
			/* ストローク設定	[指令単位]				*/
			/* [1000指令単位]→[指令単位]変換=1000倍 	*/
			if( MlibAbs32( AdatV->MoveDistance ) > AdatV->DefaultDistance )
			{
				PJogV->Cnst.Distance = AdatV->DefaultDistance;
			}
			else
			{
				PJogV->Cnst.Distance = MlibAbs32( AdatV->MoveDistance );
			}

			/* 速度設定	[min-1]							*/
			/* プログラムJOG速度に対する倍率=2倍		*/
			wk_spd = 2;
			/* イナーシャ比，トルク比設定 */
			if( AdatV->JratDrvCnt != 0 )
			{
				/* 同定中は，ある程度負荷が把握できているので，同定値に応じて加速トルクを決定	*/
				trq_per = JRATACCTRQ2;							/* 定格トルク100%加速			*/
				if( AdatV->Jrate > pndef_jrate.UpperLimit )
				{
					jratex = pndef_jrate.UpperLimit + 100;
				}
				else
				{

					jratex = JatHdl->EstimatJrat + 100;			/* イナーシャ同定値＋100%		*/
				}
			}
			else
			{
				/* イナーシャ同定最初の動作指令							*/
				/* 同定初回は，どんな負荷がついているかわからないので，	*/
				/* 「等価イナーシャで8%加速トルク」とする				*/
				trq_per = JRATACCTRQ;
				jratex = (USHORT)Prm->Pn324 + 100;
			}
			pjog_switch = 1;		/* 加速度設定選択 */
			/* 待ち時間設定[ms] 									*/
			/* プログラムJOG開始待ち時間=0ms, シーケンス遷移分減算 	*/
			PJogV->Cnst.WaitTime = 0;
			AdatV->f.CoinIgnore = TRUE;
			break;

		case 0x01:		/* 早送り正転					*/
		case 0x02:		/* 早送り逆転					*/
		default:		/* デフォルトは早送り正転		*/
			/* ストローク設定	[指令単位]				*/
			/* [1000指令単位]→[指令単位]変換=1000倍	*/
			PJogV->Cnst.Distance = MlibAbs32( AdatV->MoveDistance );

			/* 速度設定			[min-1]					*/
			/* プログラムJOG速度に対する倍率=2倍		*/
			wk_spd = 2;
			/* 待ち時間設定		[ms]					*/
			/* プログラムJOG開始待ち時間=300ms+300ms	*/
			PJogV->Cnst.WaitTime = AATPJOGWAITTM;
			AdatV->f.CoinIgnore = FALSE;
			break;
	}

	/* 運転方向設定				*/
	switch ( pattern )
	{
		case 0x02:		/* 早送り逆転	*/
		case 0x04:		/* 通常送り逆転	*/
			PJogV->Cnst.MTimes = 1;
			if( AdatV->MoveDistance >= 0 )
			{
				/* 運転設定				*/
				PJogV->Cnst.Pattern = TRUE;
				/* 運転方向初期設定		*/
				PJogV->Cnst.RevDir = TRUE;
			}
			else
			{
				/* 運転設定				*/
				PJogV->Cnst.Pattern = FALSE;
				/* 運転方向初期設定		*/
				PJogV->Cnst.RevDir = FALSE;
			}
			break;
		case 0x06:		/* イナーシャ同定早送り正転	*/
		case 0x07:		/* イナーシャ同定早送り逆転	*/
			PJogV->Cnst.MTimes = 4;			/* 移動回数は2往復に固定 */
			if( AdatV->MoveDistance >= 0 )
			{
				/* 運転設定				*/
				PJogV->Cnst.Pattern = 4;
				/* 運転方向初期設定		*/
				PJogV->Cnst.RevDir = TRUE;
			}
			else
			{
				/* 運転設定				*/
				PJogV->Cnst.Pattern = 5;
				/* 運転方向初期設定		*/
				PJogV->Cnst.RevDir = FALSE;
			}
			break;
		case 0x01:		/* 早送り正転	*/
		case 0x03:		/* 通常送り正転	*/
		default:
			PJogV->Cnst.MTimes = 1;
			if( AdatV->MoveDistance >= 0 )
			{
				/* 運転設定				*/
				PJogV->Cnst.Pattern = FALSE;
				/* 運転方向初期設定		*/
				PJogV->Cnst.RevDir = FALSE;
			}
			else
			{
				/* 運転設定				*/
				PJogV->Cnst.Pattern = TRUE;
				/* 運転方向初期設定		*/
				PJogV->Cnst.RevDir = TRUE;
			}
			break;
	}

	/* 基準速度		*/
	if( AdatP->MencV->LowSpdMotor == TRUE )
	{	/* DDモータ	*/
		baseVel_wk = Bprm->NorRatSpd * 10 / 3;	/* 基準速度=定格速度÷3 */
										/* 10倍するのは，PJOGROTSPD単位が[0.1min-1]となるため	*/
	}
	else if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
	{	/* リニアモータ	*/
		baseVel_wk = MlibMIN( (Bprm->NorRatSpd / 3), (Bprm->NorMaxSpd / 2) );
											/* 基準速度は、定格速度÷3と最高速度÷2の小さい方	*/
	}
	else
	{	/* 回転型モータ	*/
		baseVel_wk = Bprm->NorRatSpd / 3;							/* 基準速度は、定格速度÷3	*/
	}

	/*------------------------------------------------------------------------------------------*/
	/*	加速トルク指定（速度は基準速度）														*/
	/*------------------------------------------------------------------------------------------*/
	/* 速度設定	[min-1] */
#if (FLOAT_USE==TRUE)
	VelData = baseVel_wk * wk_spd * Bprm->Kmonspd ;
#else
	VelData = MlibMulgainSx48( baseVel_wk * wk_spd, Bprm->Kmonspd );
#endif /* FLOAT_USE */
	if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )				/* リニアモータ 			*/
	{
		TunePrmWriteRam( &AccessPrm, &pndef_pjoglinspd, (LONG)VelData, pndef_pjoglinspd.DataLength );
	}
	else
	{
		TunePrmWriteRam( &AccessPrm, &pndef_pjogrotspd, (LONG)VelData, pndef_pjogrotspd.DataLength );
	}

	/* 加速度設定 [ms] */
	kx = MlibScalKxgain( jratex, Bprm->PerMaxTrq, 100, &sx, 0 );	/* Bprm.PerMaxTrq[%] /100	*/
	kx = MlibPcalKxkskx( kx, Bprm->OvrSpd, 0x1000000, &sx, 0 );
	kx = MlibPcalKxksks( kx, Bprm->Jmot, Bprm->MaxTrq, &sx, 0 );
	AccGain = MlibPcalKxgain( kx, 10000, trq_per, &sx, 24 );		/* 1/基準加速度	[ms/Nos]	*/
#if (FLOAT_USE==TRUE)
	AccData = PJogV->Cnst.MaxSpd * AccGain;
#else
	AccData = MlibMulgain( PJogV->Cnst.MaxSpd, AccGain );
#endif /* FLOAT_USE */

	if( AccData > pndef_pjogacctm.UpperLimit )
	{
		AccData = pndef_pjogacctm.UpperLimit;
	}
	else if( AccData < pndef_pjogacctm.LowerLimit )
	{
		AccData = pndef_pjogacctm.LowerLimit;
	}
	TunePrmWriteRam( &AccessPrm, &pndef_pjogacctm, (LONG)AccData, pndef_pjogacctm.DataLength );

	if( pjog_switch == 1 )
	{	/* イナーシャ同定	*/
		/* 基準移動距離計算	[指令単位]	*/
		/*--------------------------------------------------------------------------------------*/
		/*	目標トルクにて加速し，最大運転速度で基準一定速度時間移動した場合の移動距離			*/
		/*	動作指令の基準となる移動距離を算出する												*/
		/*																						*/
		/*	基準移動距離 [指令単位] = 最大運転速度[指令単位/ms] * 基準一定速度時間[ms]			*/
		/*																						*/
		/*									最大運転速度[指令単位/ms^2]							*/
		/*								+ -------------------------------						*/
		/*										加速度[指令単位/ms^2]							*/
		/*																						*/
		/*--------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( PJogV->Cnst.MaxSpd, 1000, Bprm->NorOvrSpd, &sx, 0 );
		kx = MlibPcalKxkskx( kx, Bprm->OvrPspd, 1, &sx, 24 );
#if (FLOAT_USE==TRUE)
		kx = (AATCONSVELTM * kx ) + ( Prm->pjogacctm * kx );
#else
		kx = MlibMulgain( AATCONSVELTM, kx ) + MlibMulgain( Prm->pjogacctm, kx );
#endif /* FLOAT_USE */

		/*	動作速度再計算[min-1] */
		/*--------------------------------------------------------------------------------------*/
		/*	加速度一定条件で基準移動距離と指令ストロークから動作速度を再計算					*/
		/*																						*/
		/*											  指令ストローク(L_ref)[指令単位]			*/
		/*	修正速度[min-1] = 最大運転速度[min-1] * (-----------------------------------)^0.5	*/
		/*												基準移動距離(L_base)[指令単位]			*/
		/*																						*/
		/*--------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( PJogV->Cnst.Distance, 1, kx, &sx, 24 );/* L_ref/L_base				*/
#if (FLOAT_USE==TRUE)
		kx = (VelData * VelData) * kx ;				/* = Vmax^2 * L_ref/L_base	*/
#else
		kx = MlibMulgain( (VelData * VelData), kx );				/* = Vmax^2 * L_ref/L_base	*/
#endif /* FLOAT_USE */
		kx = MlibSqrtu32( kx );										/* 修正速度	[min-1]			*/

		/*--------------------------------------------------------------------------------------*/
		/*	最大，最小クランプ																	*/
		/*--------------------------------------------------------------------------------------*/
		if( kx > 2 * VelData )
		{
			kx = 2 * VelData;								/* 基準速度×2でクランプ			*/
		}
		else if( kx < VelData / 2 )
		{
			kx = VelData / 2;								/* 基準速度÷2でクランプ			*/
		}

		if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
		{
			TunePrmWriteRam( &AccessPrm, &pndef_pjoglinspd, (LONG)kx, pndef_pjoglinspd.DataLength );
		}
		else
		{
			TunePrmWriteRam( &AccessPrm, &pndef_pjogrotspd, (LONG)kx, pndef_pjogrotspd.DataLength );
		}

		/*	加速時間再計算 [ms] */
#if (FLOAT_USE==TRUE)
		AccData = PJogV->Cnst.MaxSpd * AccGain ;	/* 修正加速時間	[ms]			*/
#else
		AccData = MlibMulgain( PJogV->Cnst.MaxSpd, AccGain );	/* 修正加速時間	[ms]			*/
#endif /* FLOAT_USE */
		if( AccData > pndef_pjogacctm.UpperLimit )
		{
			AccData = pndef_pjogacctm.UpperLimit;
		}
		else if( AccData < JRATPJOGACC )
		{
			AccData = JRATPJOGACC;
		}
		else if( AccData < pndef_pjogacctm.LowerLimit )
		{
			AccData = pndef_pjogacctm.LowerLimit;
		}
		TunePrmWriteRam( &AccessPrm, &pndef_pjogacctm, AccData, pndef_pjogacctm.DataLength );
	}

	if( ( PJogV->Cnst.PrmUnMatch == TRUE ) || ( PJogV->Cnst.PrmUnMatch2 == TRUE ) )
	{
		ALMSetServoAlarm( AdatP->AlmManager, ALM_PRMUNMATCH );/* A.042 : パラメータ組合わせ異常 */
	}

	AdatV->f.DrvPatternSet = TRUE;
}

/****************************************************************************
	アドバンストオートチューニング初期化処理

	概要:アドバンスオートチューニング開始時にフラグ、関連データの初期化を行う。
****************************************************************************/
static PRM_RSLT AdatInitial( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP)
{
	ULONG	data;
	PRM_RSLT rc;

	PRMDATA *Prm;
	MFCTRL	*MFControl;
	REG_ACC_T AccessPrm;

	Prm       =  AdatP->UniPrms->Prm;
	MFControl = &AdatP->BaseCtrlData->MFControl;
	rc = PRM_RSLT_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	AdatV->ModeSetPtr      = 0;
	AdatV->JratSetPtr      = 0;
	AdatV->VibDetPtr       = 0;
	AdatV->KvTunePtr       = 0;
	AdatV->KpTunePtr       = 0;
	AdatV->KpSetPtr        = 0;
	AdatV->MfcTunePtr      = 0;
	AdatV->NotchSetPtr     = 0;
	AdatV->PrgJogPtr       = 0;
	AdatV->PrgJogPatternReq= ADAT_PJOG_STOP;
	AdatV->PrgJogPattern   = ADAT_PJOG_STOP;
	AdatV->JratChangeTimes = 0;
	AdatV->JratDrvCnt      = 0;
	AdatV->VibMaxKv        = 0;
	AdatV->VibMaxKvAresOFF = 0;			/* A型ON前のゲイン。現在のゲインと比較するので初期値は0	*/
	AdatV->MaxKv = Prm->adatgmax;		/* 探索最大ゲインを最大値ゲインに設定  					*/
	AdatV->MaxKp = AdatV->MaxKv;		/* 設定Kp最大値初期値は探索最大Kvと同じ					*/
	/* 振動検出レベルの初期化 */
	AdatP->DetVib->conf.VibCheckLevel = ADAT_GRAT_70( AdatP->DetVib->conf.VibCheckLevel );
	/* AdatV->VibSpeedは，振動レベル測定でセットされるが念のため初期化を行う	*/
	AdatV->VibSpeed = AdatP->DetVib->conf.VibCheckLevel;
	AdatV->JratErr  = ADAT_JRAT_OK;
	AdatV->f.CoinWaitStart = FALSE;		/* COIN出力待ち処理リセット								*/
	AdatV->f.Used2MassMfc  = FALSE;		/* MFC(剛体+機台モデル)設定済みフラグを初期化			*/
	AdatV->f.Mfc2MassOK    = FALSE;		/* MFC(剛体+機台モデル)最終調整済みフラグを初期化		*/
	AdatV->f.MfcLastSet    = FALSE;		/* MFC(剛体+機台モデル)調整完了							*/
	AdatV->Set2MassMfcSequence = 0;		/* MFC(剛体+機台モデル)設定時シーケンスを初期化			*/
	AdatV->f.StopDetectMode2   = FALSE;	/* 停止判定にCOIN、MotStopを使用する					*/
	AdatV->frq_buf = Prm->mdlantfrq;	/* 共振・反共振周波数バッファ							*/
	AdatV->Kpm_buf = Prm->mdlgn; 		/* Kpmバッファ											*/
	AdatV->Kv_buf  = Prm->loophz;		/* Kvバッファ											*/

	/* すでにMFC(剛体+機台モデル)設定済みの時にはMFC(剛体+機台モデル)の調整はしない	*/
	if(( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
			|| (((Prm->mdlsw >> 8) & 0x000F) != 0x0001))
	{
		AdatV->f.Used2MassOld = TRUE;
	}
	else
	{
		AdatV->f.Used2MassOld = FALSE;
	}

	AdatP->FnCmnCtrl->FnSensOnReq = TRUE;
	AdatReg->AdatState = ADAT_STS_INIT;	/* ステータス初期化	*/
	AdatV->MinDistance = AdatPcalMinDistance( AdatP,
											  AdatP->MencV->AxisMotType,
	                                          AdatP->MencV->LowSpdMotor );
	/* 下3桁を切り捨てる処理を追加 */
	AdatV->DefaultDistance = AdatPcalDefautDistance( AdatP,
													 AdatP->MencV->AxisMotType,
	                                                 AdatP->MencV->LowSpdMotor ) / 1000 * 1000 ;

	if( (1000*MlibAbs32( AdatReg->AdatMovep.Long )) < AdatV->MinDistance )
	{
		if(AdatReg->AdatMovep.Long < 0)
		{
			AdatV->MoveDistance = -AdatV->DefaultDistance;
		}
		else if(AdatReg->AdatMovep.Long >= 0)
		{
			AdatV->MoveDistance = AdatV->DefaultDistance;
		}
		AdatReg->AdatMovep.Long = AdatV->MoveDistance / 1000;
	}
	else
	{
		AdatV->MoveDistance = 1000 * AdatReg->AdatMovep.Long;
	}

	AdatScanCRequest( ADAT_NO_OPE, AdatV );

	/* 自動ノッチ設定可否判断	*/
	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) == 0x0000 )
	{
		AdatV->f.UsedAllNtFil = TRUE;
	}
	else
	{
		AdatV->f.UsedAllNtFil = FALSE;
	}
	/* 自動A型設定可否判断		*/
	if( ( ((Prm->avibsw >> 4) & 0x0F)  == FALSE) 	/* 自動調整無効						*/
		|| ((Prm->avibsw & 0x0F) == TRUE) )			/* または、A型既に有効				*/
	{
		AdatV->f.UsedAResCtrl = TRUE;				/* 設定済み 						*/
	}
	else
	{
		AdatV->f.UsedAResCtrl = FALSE; 				/* 未設定 							*/
	}

	AdatV->ModeSetPtr = AdatV->ModeSetMin;

	/* 発振検出	*/
//	data = 0x0000FFF0 & Prm->vibsens;//<S09E>
	data = 0x0000FFF0 & Prm->vibsel;
	TunePrmWriteRam( &AccessPrm, &pndef_vibsel, (LONG)data, pndef_vibsel.DataLength );

	/* 停止時トルクハイパスフィルタ周波数	*/
	AdatV->KTrqHpFil = MlibPcalKf1gain( STPTRQFIL, (KPI_SCCYCLEMS*1000), 1 );

	if(MFControl->conf.MFCModelSetting <= ADATMFC_ONE_MASS)
	{	/* MFC無効 or 剛体MFC	*/
		/* モデル追従制御無効	*/
		data = 0x0000FFF0 & Prm->mdlsw;
		TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)data, pndef_mdlsw.DataLength );
		/* 外乱オブザーバ無効	*/
		data = 0x00000FFF & Prm->tfuncsw;
		TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)data, pndef_tfuncsw.DataLength );
	}
	/* HPFを設定し、低周波振動を除去する */
	DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
	                         	 DETVIBLP, 0, AdatP->UniPrms->Bprm->SvCycleNs );
	/*----------------------------------------------------------------------------------*/
	/*	残留振動周波数検出感度の初期化													*/
	/*----------------------------------------------------------------------------------*/
	data = Prm->remdetw * ADAT_LFVIBDET_LVL / 100;
	if( (LONG)data > pndef_remdetw.UpperLimit )
	{
		data = pndef_remdetw.UpperLimit;
	}
	TunePrmWriteRam( &AccessPrm, &pndef_remdetw, (LONG)data, pndef_remdetw.DataLength );

	/*----------------------------------------------------------------------------------*/
	/*	ゲインの初期化																	*/
	/*----------------------------------------------------------------------------------*/
	TuneFilGainCalbyFilType( AdatP->ANotch, (UCHAR)AdatReg->AdatFilType );
	AdatV->Jrate = Prm->jrate;
	AdatV->f.OrgIPSpdControl = (AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl)? TRUE : FALSE;
	return rc;
}

/****************************************************************************
	最小移動距離 [1指令単位]
****************************************************************************/
static LONG	AdatPcalMinDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor )
{
	BPRMDAT *Bprm;
	POS_MNG_HNDL *PosManager;
	LONG sx;
	LONG kx;
	LONG lwork;

	Bprm = AdatP->UniPrms->Bprm;
	PosManager = &AdatP->BaseCtrlData->PosManager;

	lwork = (LowSpdMotor)? (2*10) : (2*1);

	if( AxisMotType == MOTTYPE_LINEAR )
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, MINIMUM_LINEAR_MM, 1, &sx, 0 );
	}
	else
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, 1, lwork, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, PosManager->conf.Egear.a, PosManager->conf.Egear.b, &sx, -30 );
	return ( kx );

}

/****************************************************************************
	初期移動距離計算 [1指令単位]
****************************************************************************/
LONG AdatPcalDefautDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor ) /* <S145> */
{
	BPRMDAT *Bprm;
	POS_MNG_HNDL *PosManager;
	LONG	sx;
	LONG	kx;
	LONG	lwork;

	Bprm = AdatP->UniPrms->Bprm;
	PosManager = &AdatP->BaseCtrlData->PosManager;

	lwork = (LowSpdMotor)? (10*10) : (10*1);


	if( AxisMotType == MOTTYPE_LINEAR )
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, DEFAULT_STROKE_LINEAR, 10, &sx, 0 );
	}
	else
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, DEFAULT_STROKE_ROTARY, lwork, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, PosManager->conf.Egear.a, PosManager->conf.Egear.b, &sx, -30 );
	if( kx > ADAT_DIST_MAX )
	{
		kx = ADAT_DIST_MAX;
	}
	if( kx < -ADAT_DIST_MAX )
	{
		kx = -ADAT_DIST_MAX;
	}
	return ( kx );
}

/****************************************************************************
	アドバンスドオートチューニング スキャンC探索要求(Round)

	概要:	スキャンCへのチューニング要求のコマンドとフラグを
			Roundにてセットする。
****************************************************************************/
static void AdatScanCRequest( USHORT Request, ADATV *AdatV )
{
	AdatV->ScanCReqCmd = Request;/* 暫定的にセット */
	AdatV->ScanCStat = ADAT_SEAR_CONTINUE;
	AdatV->ScanCReqFlg = TRUE;
}


/****************************************************************************
	アドバンスドオートチューニング スキャンC探索受付(ScanC)

	概要:	スキャンCのチューニング要求のコマンドとフラグを
			スキャンCにて受け取る。
****************************************************************************/
static void AdatScanCSetReq( ADATV *AdatV )
{
	if(	AdatV->ScanCReqFlg == TRUE)
	{
		AdatV->ScanCRequest = AdatV->ScanCReqCmd;	/* ScanC 処理要求セット		*/
		AdatV->ScanCStat    = ADAT_NO_OPE;			/* エラーステータスクリア	*/
		AdatV->ScanCReqFlg = FALSE;
	}
}

/****************************************************************************
	速度ループゲイン変更処理

	概要:	速度ループの各ゲインを要求に応じて変更する。
****************************************************************************/
static SHORT	TuneAdatSetSpdCtrlGain( LONG mode, ADATP *AdatP)
{

	PRMDATA   *Prm;
	REG_ACC_T AccessPrm;
	USHORT	kvdata;
	USHORT	data;
	SHORT	rc;
	SHORT	NotchState;

	rc  = ADAT_OK;
	Prm = AdatP->UniPrms->Prm;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( mode )
	{
		case GAIN_UP:
			kvdata = Prm->loophz + KVUP_UNIT;		/* 2.0[Hz] 増加					*/
			break;

		case GAIN_DW:
			kvdata = ADAT_GRAT_94( Prm->loophz );	/* 15/16 = 0.9375 6.25%減少		*/
			break;

		case GAIN_CAL:
		default:
			kvdata = Prm->loophz;					/* 現状のまま					*/
			break;
	}

	if( kvdata < KV_UNLMT )
	{
		kvdata = KV_UNLMT;							/* 下限値未満は下限値とする		*/
		rc = ADAT_NG;								/* 異常終了とする				*/
	}
	else if( kvdata > Prm->adatgmax )
	{
		kvdata = Prm->adatgmax;						/* 上限値超は上限値とする		*/
	}

	/*------------------------------------------------------------------------------*/
	/*	速度ループゲイン(Kv)														*/
	/*------------------------------------------------------------------------------*/
	TunePrmWriteRam( &AccessPrm, &pndef_loophz, (LONG)kvdata, pndef_loophz.DataLength );
	/*------------------------------------------------------------------------------*/
	/*	速度ループ積分時定数(Ti)													*/
	/*------------------------------------------------------------------------------*/
	data = TuneKvFuncTi( kvdata, AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl );
	TunePrmWriteRam( &AccessPrm, &pndef_pitime, (LONG)data, pndef_pitime.DataLength );
	/*------------------------------------------------------------------------------*/
	/*	外乱オブザーバ関連パラメータ												*/
	/*------------------------------------------------------------------------------*/
	if( AdatP->BaseCtrlData->DobsCtrl.var.DobsAct == TRUE )
	{ /* 外乱オブザーバ有効 */
		data = TuneKvFuncDobsgn( kvdata );
		TunePrmWriteRam( &AccessPrm, &pndef_dobgn, (LONG)data, pndef_dobgn.DataLength );
	}
	/*------------------------------------------------------------------------------*/
	/*	ノッチフィルタ関連 ＆ Tf パラメータ											*/
	/*------------------------------------------------------------------------------*/
	NotchState = TuneNotchQ( AdatP->ANotch, AdatP->RegManager->hPnReg,
	                         	 	 	 	 	 	 kvdata, Prm->jrate, 1, Prm );
	if( (NotchState < 0) && (NotchState != -3) )
	{	/* ノッチフィルタ設定異常（Q値が上限値に達した）*/
		rc = ADAT_NTCHNG;		/* ノッチ計算異常		*/
	}
	return rc;
}

/****************************************************************************
		アドバンストオートチューニングイナーシャ設定処理

	概要:	イナーシャ同定処理自体は「オートチューニング機能」を利用する。
			ここでは、「オンラインオートチューニング機能」を実行するために
			必要なデータをセットする。
			イナーシャ比は、正転時同定値と逆転時同定値の平均値とする。
			ただし、正転／逆転の同定値の差が31.25%を超える場合は、エラーとする。
****************************************************************************/
static PRM_RSLT	AdatJratSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA *Prm;
	REG_ACC_T AccessPrm;
	PRM_RSLT rc;
	ULONG data;
	BOOL TrqClamp;
	TUNELESS_CFGPRM TuneLessCfgPrm;

	rc  = PRM_RSLT_SUCCESS;
	Prm = AdatP->UniPrms->Prm;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	TrqClamp = (AdatP->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO & 1);
	if( AdatV->f.RefInType == FALSE)
	{	/* J同定時のリミットチェックは内部指令AATのみ */
		if( TrqClamp == TRUE )
		{
			AdatV->JratSetPtr = 0;
			AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
			AdatV->ErrState = ADAT_J_ERR;
			AdatV->JratErr  = ADAT_JRAT_ERR4;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	調整レス強制無効処理																		*/
	/*		イナーシャ同定ありの場合は調整レスを無効としてAATを実行									*/
	/*----------------------------------------------------------------------------------------------*/
	if( AdatP->BaseCtrlData->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{	/* 調整レス有効	*/
		if(AdatReg->AdatMode == FALSE)								/* イナーシャ同定あり			*/
		{
			/* RAMパラメータ機能スイッチ無効 */
			data = 0x0000FFF0 & Prm->DatLevel;
			TunePrmWriteRam( &AccessPrm, &pndef_DatLevel, (LONG)data, pndef_DatLevel.DataLength );

			/* 調整レス機能選択スイッチ */
			TuneLessCfgPrm.gnmode	= Prm->gnmode;
			TuneLessCfgPrm.mfctype	= Prm->mfctype;
			TuneLessCfgPrm.avibsw	= Prm->avibsw;
			TuneLessCfgPrm.DatLevel	= Prm->DatLevel;
			TuneLessCfgPrm.tfuncsw 	= Prm->tfuncsw;
			TuneLessCfgPrm.flg_wf 	= Prm->MencP.flg_wf;
			TuneLessCalculateInitPrm(AdatP->BaseCtrlData,
			                         AdatP->UniPrms->Bprm,
			                         AdatP->MencV,
									 AdatP->SvAsicRegs,
			                         &TuneLessCfgPrm);

			/* 振動検出用差速度最大値・最小値リセット */
			DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBSGN, 0,
			                         	 DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs);

#if 0 /* 2012.08.31 Y.Oka ★モータ速度検出移動平均現状未対応★ */
			/* モータ速度検出用移動平均時定数をデフォルトに戻す		*/
			BpiChangeSpdDetMaFil( AdatP->MotSpdMafil, -1,
						(USHORT*)&(AdatP->SvAsicRegs->AsicMicroReg->MREG_SPDMANUMBITI) );
#endif
		}
		else
		{
			AdatV->ErrState = ADAT_MOSESETERR;						/* モード設定エラー				*/
		}
	}

	switch ( FunExe->CmdCode )
	{
		case FCMD_SV :
			if( AdatV->f.RefInType == FALSE )
			{
				if( AdatP->AlmManager->Status.AlmFlag || AdatP->SeqCtrlOut->HwbbSts ||
					!AdatP->SeqCtrlOut->MainPowerOn || AdatP->SeqCtrlOut->OverTrvlSts )
				{
					rc = PRM_RSLT_CONDITION_ERR;
				}
				else
				{
					AdatP->FnCmnCtrl->FnSvonReq = ~AdatP->FnCmnCtrl->FnSvonReq;
				}
			}
			return rc;

		default :
			break;
	}

	switch ( AdatV->JratSetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	移動方向入力待ち																	*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			AdatReg->AdatState = ADAT_STS_DRVWAIT;	/* 初期設定完了 入力待ち */
			AdatP->FnCmnCtrl->State.TuningRun = FALSE;

			/* モードスイッチ無効 */
			data  = Prm->gnmode;
			data &= 0x0000FFF0;
			data |= 0x00000004;
			TunePrmWriteRam( &AccessPrm, &pndef_gnmode, (LONG)data, pndef_gnmode.DataLength );

			/*----------------------------------------------------------------------------------*/
			/*	Ａ型制振強制無効																*/
			/*----------------------------------------------------------------------------------*/
			if( (AdatV->f.RefInType == FALSE) && (AdatReg->AdatMode == FALSE) )	/* イナーシャ同定あり	*/
			{
				/*Ａ型制振無効 */
				data = 0x0000FFF0 & Prm->avibsw;
				TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (LONG)data, pndef_avibsw.DataLength );
				AdatV->f.UsedAResCtrl = TRUE;	/* Ａ型制振自動設定済みにしてＡ型制振の自動設定機能をOFFする	*/
			}

			switch ( FunExe->CmdCode )
			{
				case FCMD_UP :
					/* サーボオンチェック & 移動方向チェック */
					if( ((AdatV->f.RefInType == FALSE) && (AdatP->SeqCtrlOut->BaseEnableReq == TRUE) && (AdatV->MoveDistance > 0)) )
					{
						AdatV->f.DrvErrDetect = TRUE;
						AdatP->FnCmnCtrl->State.TuningRun = TRUE;
						AdatV->JratSetPtr = 1;	/* サーボオンしていて、かつ移動方向とキー入力が同じ	*/
												/* 場合に限り、チューニング開始へ					*/
					}
					else
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					break;

				case FCMD_DOWN :
					if( ((AdatV->f.RefInType == FALSE)
					   && (AdatP->SeqCtrlOut->BaseEnableReq == TRUE)
					   && (AdatV->MoveDistance < 0)) )
					{
						AdatV->f.DrvErrDetect = TRUE;
						AdatP->FnCmnCtrl->State.TuningRun = TRUE;
						AdatV->JratSetPtr = 1;
					}
					else
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					break;

				case FCMD_EXEC :
					if( (AdatV->f.RefInType == TRUE) && (AdatRefInExeModeExeCheck(AdatP)) )
					{
						AdatV->f.DrvErrDetect = TRUE;
						AdatP->FnCmnCtrl->State.TuningRun = TRUE;
						AdatV->JratSetPtr = 1;
					}
					else
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					break;

				case FCMD_NONE :
					break;

				default:
					rc = PRM_RSLT_CONDITION_ERR;
					break;
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	運転開始																			*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			TunePrmWriteRam( &AccessPrm, &pndef_ffgn, JRATE_VFFGN, pndef_ffgn.DataLength );
			AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl = FALSE;
			BpiPcmdFilSwitch( &AdatP->BaseCtrlData->PosManager, FALSE );  	/* 位置指令フィルタ停止   */
			MfcSetMfcCtrlSwitch( &AdatP->BaseCtrlData->MFControl, FALSE );	/* MFC強制無効 		 	  */
			AdatP->BaseCtrlData->DobsCtrl.var.DobsAct = 0;					/* 外乱オブザーバ強制無効 */
			AdatV->LastEstimatJrat = -1;									/* 同定値前回値初期化	  */

			if( (AdatV->f.RefInType == FALSE) && (AdatReg->AdatMode == FALSE) )
			{	/* イナーシャ同定有効 	*/
				AdatReg->AdatState = ADAT_STS_JRUN;				/* イナーシャ同定実行中			*/
				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN5;	/* プログラムJOG運転パターン5	*/
				AdatV->JratSetPtr = 2;							/* 同定中処理へ					*/
				AdatV->Jrate = Prm->Pn324;
				/* Pn103 */
				TunePrmWriteRam( &AccessPrm, &pndef_jrate, AdatV->Jrate, pndef_jrate.DataLength );
			}
			else
			{	/* イナーシャ同定無効	*/
				CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
				AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
				AdatV->JratSetPtr = 0;							/* 初期設定状態へ				*/
				AdatUndoRamGainforJrat( AdatV, AdatP );
				FunExe->State = ADAT_SEQ_NORMVIB;				/* 通常振動レベル探索へ			*/
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	運転開始待ち中																		*/
		/*--------------------------------------------------------------------------------------*/
		case 2:
			AdatScanCRequest( ADAT_SEQ_SEARCHOPTKV, AdatV );			   /* スキャンC処理要求 */
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				CpiInitJatOffLine( &AdatP->BaseCtrlData->JatHdl,
									AdatP->UniPrms,
									AdatP->RegManager,
									AdatP->BaseCtrlData->GainChange,
									1,
									AdatP->BaseCtrlData->PJogHdl.PJogV.Cnst.Distance,
									AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm,
									AdatP->UniPrms->Bprm->SvCycleNs);

				if( AdatV->PrgJogStat == ADAT_PJOG_EXEC )
				{	/* 運転開始	*/
					AdatV->JratSetPtr = 3;
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
				}
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	イナーシャ同定中																	*/
		/*--------------------------------------------------------------------------------------*/
		case 3:
		/*--------------------------------------------------------------------------------------*/
		/*	同定エラー判別																		*/
		/*--------------------------------------------------------------------------------------*/
			if( CpiGetJatExeErrStatus( &AdatP->BaseCtrlData->JatHdl ) == NG )
			{	/* イナーシャ同定中実行エラー	*/
				AdatV->JratSetPtr = 0;
				AdatV->ErrState = ADAT_J_ERR;
				AdatV->JratErr = ADAT_JRAT_ERR5;
			}
			if( CpiGetJatErrStatus( &AdatP->BaseCtrlData->JatHdl ) == NG )
			{
				/*------------------------------------------------------------------------------*/
				/*	イナーシャ初期値補正														*/
				/*		大イナーシャ時のKp/Kvバランスによる振動対策。同定初回のみ行う			*/
				/*------------------------------------------------------------------------------*/
				if( ( KlibGetLongTimerMs( AdatV->JratVibWaitCntr ) >= ADATVIBWAITTM3 )	/* 振動収束待ち100ms	*/
					 || (AdatV->JratChangeTimes == 0) )									/* エラーなし			*/
				{
					if( (CpiGetJatExeErrStatus( &AdatP->BaseCtrlData->JatHdl ) != NG) && (AdatV->JratDrvCnt == 0 )
							&& (AdatV->JratChangeTimes < ADAT_JRAT_CHGCNT) )
					{
						AdatV->f.VibDetectinCycle = TRUE;	/* 振動ステータスON */
						AdatV->Jrate = AdatV->Jrate + ADAT_JRAT_CHG_RATE;
						TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)AdatV->Jrate, pndef_jrate.DataLength );
						AdatV->JratChangeTimes++;
						KlibRstLongTimer( &AdatV->JratVibWaitCntr );
					}
					else
					{
						AdatV->JratSetPtr = 0;
						AdatV->ErrState = ADAT_J_ERR;
						AdatV->JratErr = ADAT_JRAT_ERR3;
					}
				}
			}

		/*--------------------------------------------------------------------------------------*/
		/*	振動判別自動ノッチ																	*/
		/*--------------------------------------------------------------------------------------*/
			if( (status == ADAT_SEAR_VIBEND) || (status == ADAT_SEAR_STOPVIBEND) )
			{	/* 振動検出終了		*/
				TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP );
				if( ( AdatV->f.UsedAllNtFil == FALSE ) || ( AdatV->f.UsedAResCtrl == FALSE ) )
				{
					/* スキャンC処理要求 */
					AdatV->NextSeqPtr = ADAT_SEQ_JRATSET;
					FunExe->State = ADAT_SEQ_NOTCHSET;
				}
				AdatScanCRequest( ADAT_NO_OPE, AdatV );			/* スキャンC処理要求			*/
				AdatV->f.VibDetectinCycle = TRUE;				/* 振動ステータスON				*/
			}
		/*--------------------------------------------------------------------------------------*/
		/*	イナーシャ同定完了判断																*/
		/*--------------------------------------------------------------------------------------*/
			if(( AdatV->PrgJogStat == ADAT_PJOG_FIN )
					||  ( AdatV->PrgJogStat == ADAT_PJOG_NOP ))	/* ノッチ設定から復帰時NOPになっている可能性が有る	*/
			{
				if( CpiGetJatCompStatus( &AdatP->BaseCtrlData->JatHdl ) == OK )
				{
					KlibRstLongTimer( &AdatV->JratVibWaitCntr2 );
					if( AdatV->f.VibDetectinCycle == TRUE )
					{	/* 今回運転中に振動検出した	*/
						AdatV->f.VibDetectinCycle  = FALSE;	/* 振動ステータスクリア				*/
						AdatV->JratSetPtr = 8;				/* イナーシャ再同定開始待ち待ちへ	*/
						CpiInitJatOffLine( &AdatP->BaseCtrlData->JatHdl,
											AdatP->UniPrms,
											AdatP->RegManager,
											AdatP->BaseCtrlData->GainChange,
											0,
											AdatP->BaseCtrlData->PJogHdl.PJogV.Cnst.Distance,
											AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm,
											AdatP->UniPrms->Bprm->SvCycleNs);

					}
					else
					{	/* 振動検出なし					*/
						/* 同定値をパラメータに仮反映	*/
						CpiSetEstimateJrat( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
						AdatV->JratSetPtr = 4;					/* パラメータ書込み待ちへ		*/
					}
				}
				else
				{
					AdatV->JratSetPtr = 0;
					AdatV->ErrState = ADAT_J_ERR;
					AdatV->JratErr = ADAT_JRAT_ERR1;
				}
			}
			break;

		case 4:
		/*--------------------------------------------------------------------------------------*/
		/*	イナーシャ同定完了																	*/
		/*--------------------------------------------------------------------------------------*/
			if( ( AdatV->PrgJogStat == ADAT_PJOG_FIN )
					||  ( AdatV->PrgJogStat == ADAT_PJOG_NOP ))	/* ノッチ設定から復帰時NOPになっている可能性がある	*/
			{
				if( (AdatV->LastEstimatJrat <= ADAT_ERR_ULMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat ))
							&& (AdatV->LastEstimatJrat >= ADAT_ERR_LLMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat )) )
				{	/* 正逆差が3.125%以内 → 完了		*/
					CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
					AdatV->JratSetPtr = 5;					/* パラメータ書込み待ちへ			*/
				}
				else if( AdatV->JratDrvCnt > ADAT_JDRV_MAXCNT )
				{
					if( (AdatV->LastEstimatJrat <= ADAT_ERR2_ULMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat ))
							&& (AdatV->LastEstimatJrat >= ADAT_ERR2_LLMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat )) )
					{	/* 正逆差が31.25%以内→ 完了*/
						CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
						AdatV->JratSetPtr = 5;				/* パラメータ書込み待ちへ			*/
					}
					else
					{	/* 正逆差が許容値を超えている	*/
						AdatV->ErrState = ADAT_J_ERR;		/* イナーシャ同定エラー				*/
						AdatV->JratSetPtr = 0;				/* 初期設定状態へ					*/
						AdatV->JratErr = ADAT_JRAT_ERR2;
					}
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
				}
				else
				{
					AdatV->JratSetPtr = 8;					/* イナーシャ再同定開始待ち待ちへ	*/
					CpiInitJatOffLine( &AdatP->BaseCtrlData->JatHdl,
										AdatP->UniPrms,
										AdatP->RegManager,
										AdatP->BaseCtrlData->GainChange,
										0,
										AdatP->BaseCtrlData->PJogHdl.PJogV.Cnst.Distance,
										AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm,
										AdatP->UniPrms->Bprm->SvCycleNs);

				}
				AdatV->LastEstimatJrat = AdatP->BaseCtrlData->JatHdl.EstimatJrat;
				AdatV->JratDrvCnt++;						/* 同定回数カウンタインクリメント	*/
			}
			AdatV->Jrate = AdatP->BaseCtrlData->JatHdl.EstimatJrat;
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	パラメータ計算 ＆ 通常振動レベル探索待ち											*/
		/*--------------------------------------------------------------------------------------*/
		case 5:
			AdatReg->AdatState = ADAT_STS_JEND;					/* イナーシャ同定完了			*/
			if( AdatV->PrgJogStat == ADAT_PJOG_NOP )
			{
				switch ( FunExe->CmdCode )
				{
					case FCMD_UP :
						if( AdatV->MoveDistance > 0 )
						{	/* 移動方向とキー入力が同じ場合に限り、測定開始へ	*/
							AdatV->JratSetPtr = 6;				/* パラメータ書き込み処理へ		*/
						}
						break;

					case FCMD_DOWN :
						if( AdatV->MoveDistance < 0 )
						{
							AdatV->JratSetPtr = 6;				/* パラメータ書き込み処理へ		*/
						}
						break;

					case FCMD_EWRT: /* <S195> */
						AccessPrm.EepWrite = TRUE;
						TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)AdatV->Jrate, pndef_jrate.DataLength );
						break;

					case FCMD_NONE :
						break;

					default:
						rc = PRM_RSLT_CONDITION_ERR;
						break;
				}
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	パラメータ計算 ＆ EEPROM書き込み													*/
		/*--------------------------------------------------------------------------------------*/
		case 6:
			AdatV->JratSetPtr = 0;								/* 初期設定状態へ				*/
			/* Pn103 */
			AccessPrm.EepWrite = TRUE;							/* <S195> */
			TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)AdatV->Jrate, pndef_jrate.DataLength );
			AdatUndoRamGainforJrat( AdatV, AdatP );
			FunExe->State = ADAT_SEQ_NORMVIB;					/* 通常振動レベル探索へ			*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	運転開始待ち中																		*/
		/*--------------------------------------------------------------------------------------*/
		case 7:
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				if( AdatV->PrgJogStat == ADAT_PJOG_EXEC )
				{	/* 運転開始	*/
					AdatV->JratSetPtr = 3;
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
					AdatScanCRequest( ADAT_SEQ_SEARCHOPTKV, AdatV );	/* スキャンC振動検出処理要求	*/
				}
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	ウェイト																			*/
		/*--------------------------------------------------------------------------------------*/
		case 8:/* イナーシャ同定開始待ち500ms */
			if( KlibGetLongTimerMs( AdatV->JratVibWaitCntr2 ) >= ADATVIBWAITTM1 )
			{
				AdatV->JratSetPtr = 7;							/* 再度チューニング開始			*/
				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN5;	/* プログラムJOG運転パターン5	*/
			}
			break;
		default:
			break;
	}
	return rc;
}

/****************************************************************************
		アドバンストオートチューニング通常振動レベル探索

	概要:	通常運転中の振動レベルを測定する。
			通常振動レベル探索処理はスキャンCで行う
			また、停止時トルクの平均値を測定し、
			GRVLMT％以上で重力軸と判定し、外乱トルクに負荷トルクを加算する。
****************************************************************************/
static PRM_RSLT	AdatNormVibDetect( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA *Prm;
	PRM_RSLT rc;
	USHORT ret0;
	BOOL TrqClamp;
	REG_ACC_T AccessPrm;

	Prm  = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	TrqClamp = (AdatP->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO & 1);

	/*	ScanC処理でエラー発生した場合	*/
	if( (status == ADAT_SEAR_ERR) || (TrqClamp == TRUE) )
	{
		AdatV->VibDetPtr = 0;									/* 初期設定へ					*/
		AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;				/* プログラムJOG運転停止		*/
		AdatV->ErrState = ADAT_NG;								/* 異常終了						*/
	}

	switch ( AdatV->VibDetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	初期設定																			*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			/*----------------------------------------------------------------------------------*/
			/* ゲインの初期設定																	*/
			/*----------------------------------------------------------------------------------*/
			Prm->posgn = Prm->loophz / 2;
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* 設定Kvとのバランス			*/
			TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)Prm->posgn, pndef_posgn.DataLength );

			AdatV->f.DrvPatternSet = FALSE;
			AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;		/* プログラムJOG運転パターン1	*/
			AdatV->VibSpeed     = 0;							/* 通常振動レベル初期化			*/
			AdatV->TrqRefatStop = 0;							/* 通常停止時トルク初期化		*/
			AdatV->VibDetPtr    = 1;							/* 通常振動レベル探索状態へ		*/
			RemVibRsetRemVibrationFreq( AdatP->RemVibFreq );	/* 残留振動周波数クリア			*/

			if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
			{
				/* HPFを設定し、低周波振動を除去する	*/
				DetVibObsCalculateGains( &(AdatP->DetVib->DetVibObs), VIBOBS_KS, DETVIBLP,
				                         DETVIBHP2, AdatP->UniPrms->Bprm->SvCycleNs );
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	正転／逆転時負荷トルク測定要求														*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			AdatScanCRequest( ADAT_SEQ_NORMVIB, AdatV );		/* 通常振動レベル測定要求		*/
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				AdatV->VibDetPtr = 5;
			}
			AdatV->f.VibDetectinCycle  = FALSE;					/* 振動ステータスクリア			*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	移動完了待ち																		*/
		/*--------------------------------------------------------------------------------------*/
		case 5:
			/*----------------------------------------------------------------------------------*/
			/*	振動判別自動ノッチ																*/
			/* 		振動検出したらノッチ処理に移行。ノッチ設定済みの場合、						*/
			/*		振動検出なしになるまでゲインを下げる										*/
			/*----------------------------------------------------------------------------------*/
			if( (status == ADAT_SEAR_VIBEND) || (status == ADAT_SEAR_STOPVIBEND) )
			{	/* 振動検出終了 */
				if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* kvゲインDOWN		*/
				{
					AdatV->ErrState = ADAT_NG;
					return rc;
				}
				if( ( AdatV->f.UsedAllNtFil == FALSE ) || ( AdatV->f.UsedAResCtrl == FALSE ) )
				{
					AdatV->VibDetPtr  = 0;
					AdatV->NextSeqPtr = ADAT_SEQ_NORMVIB;
					FunExe->State = ADAT_SEQ_NOTCHSET;
				}
				AdatScanCRequest( ADAT_NO_OPE, AdatV );				/* スキャンC処理要求		*/
				AdatV->f.VibDetectinCycle = TRUE;					/* 振動ステータスON			*/
			}
			/* チューニング前にMFC(剛体+機台モデル)を設定していない時，状態シーケンス移動		*/
			if( AdatV->f.Used2MassOld == FALSE )
			{
				ret0 = SetMfc2MassSequence( AdatReg, AdatV, AdatP, FunExe, ADAT_SEQ_NORMVIB );
				if( ret0 )
				{
					FunExe->State = ADAT_SEQ_2MASSSET;	 /* モデル追従(剛体+機台モデル)設定処理へ*/
					AdatV->NextSeqPtr = ADAT_SEQ_NORMVIB;/* 設定後元の処理へ戻るためのバッファ	 */
					AdatV->f.VibDetectinCycle = TRUE;	 /* 振動ステータスON					 */
				}
			}
			if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
			{	/* プログラムJOG運転完了 → 停止状態とみなす */
				if( AdatV->f.VibDetectinCycle == TRUE )
				{	/* 今回運転中に振動検出した	*/
					AdatV->f.VibDetectinCycle = FALSE;			/* 振動ステータスクリア			*/
					AdatV->VibDetPtr = 0;						/* 振動していたら再トライ		*/
				}
				else
				{	/* 今回振動なし	*/
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
					AdatV->VibDetPtr = 0;						/* 初期設定へ					*/
					FunExe->State = ADAT_SEQ_SEARCHOPTKV;		/* 最適ゲイン探索へ				*/
				}
			}
			break;

		default:
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc;
}


/****************************************************************************************************
		MFC(剛体+機台モデル)シーケンスに移動する
****************************************************************************************************/
static USHORT	SetMfc2MassSequence( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, ULONG ADAT_MFC )
{
	UINT set_mfc_f;
	LONG UnRvibFrequency;

	UnRvibFrequency = RemVibGetRemVibFreq( AdatP->RemVibFreq );

	set_mfc_f = ( UnRvibFrequency >= VIBMFC_DET_FRQ_MIN ) &&	/* 検出周波数 >= 振動検出下限値		*/
				( UnRvibFrequency <= VIBMFC_DET_FRQ_MAX ) &&	/* 検出周波数 <= 振動検出上限値		*/
				( AdatV->f.Used2MassMfc == FALSE ) &&			/* MFC(剛体+機台モデル)未設定		*/
				((AdatReg->AdatLevel == ADAT_LEVEL_POS)			/* チューニングレベル=位置決め専用	*/
			  || (AdatReg->AdatLevel == ADAT_LEVEL_POS_ST) );	/* チューニングレベル=位置決め(OV)	*/

		if( set_mfc_f )
		{
			AdatV->Set2MassMfcSequence = (USHORT)ADAT_MFC;	/*	振動抑制したシーケンスを保存 */

			switch( ADAT_MFC )
			{
				case ADAT_SEQ_NORMVIB:				/* 通常振動レベル測定処理						*/
					AdatV->VibDetPtr = 0;
					break;

#if 0	/* 最適Kv探索処理の時にはMFC(剛体+機台モデル)設定は行わない			*/
				case ADAT_SEQ_SEARCHOPTKV:			/* 最適Kv探索処理(状態シーケンスは初期化しない)	*/
					break;
#endif
				case ADAT_SEQ_ADJMFC:				/* モデル追従調整処理							*/
					AdatV->MfcTunePtr = ADAT_MFC_INI;
					break;
			}
			return 1;
		}
		return 0;
}

/****************************************************************************************************
		アドバンストオートチューニング最適ゲイン探索状態

	概要:	最適ゲイン探索をスキャンＣに要求する。
			スキャンＣから、ノッチフィルタ設定の要求を受けたときは、
			次回、ノッチフィルタ設定処理を要求する。
****************************************************************************************************/
static PRM_RSLT	AdatOptGainSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA *Prm;
	REG_ACC_T AccessPrm;
	PRM_RSLT rc;
	ULONG data;
	UCHAR tmpGainRatio;

	Prm  = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;											/* 振動処理後ゲイン割合 初期値=80% 			*/

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;


	/*	ScanC処理でエラー発生した場合	*/
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->KvTunePtr = ADAT_OPTGN_INI;						/* 初期設定へ								*/
		AdatV->ErrState  = ADAT_NG;								/* 異常終了									*/
	}

	switch ( AdatV->KvTunePtr )
	{
		/*--------------------------------------------------------------------------------------------------*/
		/*	初期設定																						*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_INI:
			/* 探索する最大ゲインを設定 */
			AdatV->MaxKv = Prm->adatgmax;						/* 探索最大ゲインを最大値ゲインに設定		*/
			/*----------------------------------------------------------------------------------------------*/
			/*	外乱オブザーバ設定																			*/
			/*		ファーストモードは、無条件に外乱オブザーバ有効とする(設定はワンパラバランス)。			*/
			/*		位置決め専用モードは、無条件に外乱オブザーバ、MFCを有効とする(設定はワンパラバランス)。	*/
			/*		その他のモードは、機能スイッチONでワンパラバランスの外乱オブザーバ有効とする。			*/
			/*----------------------------------------------------------------------------------------------*/
			switch ( AdatReg->AdatLevel )
			{	/* チューニングレベル */
				case ADAT_LEVEL_POS:							/* 位置決め専用								*/
				case ADAT_LEVEL_POS_ST:							/* 位置決め専用(オーバーシュート重視)		*/
//					TuneDobsActive( Prm );						/* 外乱オブザーバ初期設定					*/
					TuneDobsActive( Prm, AccessPrm.hRegCfg );	/* 外乱オブザーバ初期設定					*///<S09E>
					break;

				default:
					if( AdatP->BaseCtrlData->MFControl.conf.MFCModelSetting != 0 )
					{	/* AAT実行開始前モデル追従制御有効	*/
						/* モデル追従制御無効				*/
						data = 0x0000FF00 & Prm->mdlsw;
						TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, data, pndef_mdlsw.DataLength );
					}

					if( AdatP->BaseCtrlData->DobsCtrl.var.DobsAct == TRUE )
					{	/* 外乱オブザーバ有効 */
//						TuneDobsActive( Prm );		/* 外乱オブザーバ初期設定 */
						TuneDobsActive( Prm, AccessPrm.hRegCfg );	/* 外乱オブザーバ初期設定 *///<S09E>
					}
					break;
			}
			/* ゲイン設定 */
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* ゲイン計算								*/
			/* 振動レベル設定 						*/
			/* ゲイン探索時は通常振動レベルの2倍	*/
			AdatP->DetVib->conf.VibCheckLevel = AdatV->VibSpeed * RESGMAX / 100;
			AdatV->f.VibDetectinCycle = TRUE;					/* 振動ステータス初期化。初期値は振動あり	*/
			AdatV->f.DrvPatternSet  = FALSE;
			AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;		/* プログラムJOG運転パターン１				*/
			AdatV->KvTunePtr = ADAT_OPTGN_INIWAIT;				/* ゲイン変更による振動収束待ち				*/
			AdatV->f.KvTuneWait = FALSE;						/* チューニング停止フラグＯＦＦ				*/
			KlibRstLongTimer( &AdatV->KvTuneWaitCntr );			/* モータ停止待ちタイマ初期化				*/

			if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
			{
				/* HPFを設定し、低周波振動を除去する	*/
				DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
				                         DETVIBLP, DETVIBHP3, AdatP->UniPrms->Bprm->SvCycleNs );
			}
			break;

		/*--------------------------------------------------------------------------------------------------*/
		/*	ゲイン変更による振動収束待ち																	*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_INIWAIT:
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
				{	/* プログラムJOG運転完了 → １周期待つ	*/
					AdatV->f.DrvPatternSet  = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;		/* プログラムJOG運転パターン1		*/
					AdatV->KvTunePtr        = ADAT_OPTGN_REQ;			 /* 最適ゲイン探索要求状態へ		*/
				}
			}
			break;

		/*--------------------------------------------------------------------------------------------------*/
		/*	最適ゲイン探索要求																				*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_REQ:
			AdatScanCRequest( ADAT_SEQ_SEARCHOPTKV, AdatV );			/* スキャンC処理要求				*/
			AdatV->KvTunePtr = ADAT_OPTGN_SEARCH;						/* 最適ゲイン探索状態へ				*/
			break;

		/*--------------------------------------------------------------------------------------------------*/
		/*	最適ゲイン探索状態																				*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_SEARCH:
			if( status == ADAT_SEAR_NORMALEND )
			{	/* 正常終了。探索最大ゲインでも振動しなかったとき	*/
				AdatV->f.DrvPatternSet  = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;		/* プログラムJOG運転パターン3			*/
				AdatV->KvTunePtr        = ADAT_OPTGN_CHKKV_REQ;		/* 現在を最適ゲインとして安全確認要求	*/
				Prm->loophz = AdatV->MaxKv;
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* ゲイン計算							*/
			}
			else if( (status == ADAT_SEAR_VIBEND) || (status == ADAT_SEAR_STOPVIBEND) )
			{		/* 振動検出 or 停止時振動検出終了	*/
				TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP );			/* KvゲインDOWN							*/
				if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
				{
					/* 既にノッチフィルタ設定済み	*/
					AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, 50 );		/* 振動後処理 			*/
					KlibRstLongTimer( &AdatV->KvTuneCntr );
					AdatV->f.DrvPatternSet  = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* プログラムJOG運転パターン3 			*/
					AdatV->KvTunePtr     = ADAT_OPTGN_WAIT;
					AdatV->NextSubSeqPtr = ADAT_OPTGN_CHKKV_REQ;
				}
				else
				{	/* ノッチフィルタ設定処理要求	*/
					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;	/* プログラムJOG運転パターン1			*/
					AdatV->KvTunePtr = ADAT_OPTGN_REQ;
					AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKV;
					FunExe->State = ADAT_SEQ_NOTCHSET;
				}
			}
			else if( status == ADAT_SEAR_NOVIBDETECT )
			{
				/*------------------------------------------------------------------------------------------*/
				/*	外部指令では，Kv探索中に指令が停止する場合がありえる									*/
				/*	その際，Kvがどんどん上がるのを防ぐため，運転開始前(モータ停止中)は，					*/
				/*	シーケンスを更新せず，Kv UPを行わないようにする											*/
				/*	本処理は，外部指令時のみ行う															*/
				/*------------------------------------------------------------------------------------------*/
				if( AdatV->PrgJogStat != ADAT_PJOG_WAIT )
				{
					if( TuneAdatSetSpdCtrlGain( GAIN_UP, AdatP ) == ADAT_NTCHNG )	/* ゲインUP */
					{	/* ノッチQ値が上限に達した場合は正常終了とする	*/
						if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
						{
							AdatV->f.DrvPatternSet = FALSE;
							AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* プログラムJOG運転パターン3	*/
						}
						AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;	/* 現在を最適ゲインとして安全確認要求	*/
					}
					else
					{
						if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
						{
							AdatV->f.DrvPatternSet = FALSE;
							AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;	/* プログラムJOG運転パターン1	*/
						}
						AdatV->KvTunePtr = ADAT_OPTGN_REQ;					/* 最適ゲイン探索要求状態へ		*/
					}
					AdatV->f.KvTuneWait = FALSE;							/* チューニング停止フラグOFF	*/
					KlibRstLongTimer( &AdatV->KvTuneWaitCntr );				/* 動作中はタイマクリア			*/
				}
				else if( ( KlibGetLongTimerMs( AdatV->KvTuneWaitCntr ) <= ADATSTOPWAITTM )/* 停止待ち3000ms以内	*/
				&& (AdatV->f.KvTuneWait == FALSE) )
				{
					if( TuneAdatSetSpdCtrlGain( GAIN_UP, AdatP ) == ADAT_NTCHNG )	/* ゲインUP 			*/
					{	/* ノッチQ値が上限に達した場合は正常終了とする	*/
						AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;	/* 現在を最適ゲインとして安全確認要求	*/
					}
					else
					{
						AdatV->KvTunePtr = ADAT_OPTGN_REQ;					/* 最適ゲイン探索要求状態へ		*/
					}
				}
				else
				{	/* 停止待ち時間経過→ゲインチューニング停止	*/
					AdatV->f.KvTuneWait = TRUE;								/* チューニング停止フラグON		*/
				}
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	振動収束待ち状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_WAIT :
			if( KlibGetLongTimerMs( AdatV->KvTuneCntr ) >= ADATVIBWAITTM1 )/* 振動収束待ち500ms	*/
			{
				AdatV->KvTunePtr = AdatV->NextSubSeqPtr;
				Prm->loophz = AdatV->MaxKv;					/* 探索ゲイン最大値に戻す			*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );	/* ゲイン計算						*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	指令応答安全確認要求																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_CHKKV_REQ:
			AdatScanCRequest( ADAT_CHKKV, AdatV );			/* スキャンC処理要求				*/
			AdatV->KvTunePtr = ADAT_OPTGN_CHKKV;			/* 最適ゲイン探索状態2へ			*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	最適ゲイン探索状態2（振動検出後の確認区間）											*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_CHKKV:
			switch ( status )
			{
				case ADAT_SEAR_STOPVIBEND :				/* 停止時振動検出終了					*/
					tmpGainRatio = 50;					/* 振動処理後ゲイン割合 初期値=80%		*/
				/* Fall Through */
				case ADAT_SEAR_VIBEND :					/* 振動検出終了							*/
					AdatV->f.VibDetectinCycle = TRUE;	/* 振動ステータスON						*/
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* KvゲインDOWN */
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}

					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;/* プログラムJOG運転パターン3	*/

					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* 既にノッチフィルタ設定済み	→ 処理続行	*/
						/* 振動後処理	*/
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->KvTuneCntr );
						AdatV->KvTunePtr = ADAT_OPTGN_WAIT;
						AdatV->NextSubSeqPtr = ADAT_OPTGN_CHKKV_REQ;
					}
					else
					{	/* ノッチフィルタ設定未完	*/
						AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;
						AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKV;
						FunExe->State = ADAT_SEQ_NOTCHSET;
					}
					break;

				default :
					break;
			}
			/*----------------------------------------------------------------------------------*/
			/*	運転パターン終了																*/
			/*----------------------------------------------------------------------------------*/
			if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
			{	/* プログラムJOG運転完了 */
				/*------------------------------------------------------------------------------*/
				/*	運転ステータスクリア														*/
				/*		一旦クリアしないと，振動後の再トライ時に，運転ステータスが				*/
				/*		ADAT_PRGJOG_FIN のままとなり，実際に運転していないのに，振動未検出		*/
				/*		となるのを防止するため													*/
				/*------------------------------------------------------------------------------*/
				if( ( AdatV->f.VibDetectinCycle == TRUE )
						|| ( AdatV->f.DrvPatternSet == FALSE ) )/* 指令パターン未完了			*/
				{	/* 今回運転パターン中で振動した	*/
					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;/* プログラムJOG運転パターン3	*/
					AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;	 /* 再トライ					*/
				}
				else
				{	/* 振動なし → 安全確認完了 */
					/*--------------------------------------------------------------------------*/
					/*	モデル追従制御調整シーケンス判別										*/
					/*		LEVEL 2の場合は，モデル追従制御調整を行う							*/
					/*--------------------------------------------------------------------------*/
					AdatScanCRequest( ADAT_NO_OPE, AdatV );	/* 処理終了後は待機状態を要求		*/
					AdatV->KvTunePtr = ADAT_OPTGN_INI;		/* 次回チューニングシーケンスセット	*/
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;

					if( AdatV->f.RefInType == TRUE )
					{	/* 指令入力型AAT */
						FunExe->State = ADAT_SEQ_KPSET;				/* Kp設定処理 				*/
					}
					else
					{
						if( ( AdatReg->AdatLevel == ADAT_LEVEL_POS )
								|| ( AdatReg->AdatLevel == ADAT_LEVEL_POS_ST ) )
						{
							FunExe->State = ADAT_SEQ_ADJMFC;		/* モデル追従制御調整処理	*/
						}
						else
						{
							FunExe->State = ADAT_SEQ_SEARCHOPTKP;	/* Kp探索処理 				*/
						}
					}
				}
				AdatV->f.VibDetectinCycle = FALSE;					/* 振動ステータスクリア		*/
			}
			break;

		default :
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc ;
}

/****************************************************************************************************
	振動後処理
	ゲイン下げて振動収束待ちに移行
****************************************************************************************************/
static void	AdatGainDownforVibration(
	ADATV 	*AdatV,
	ADATP 	*AdatP,
	USHORT	ser_kv,								/* 保存する探索最大ゲイン			*/
	UCHAR	down_ratio							/* ゲインDown率						*/
	)
{
	PRMDATA *Prm;
	Prm = AdatP->UniPrms->Prm;

	/* 振動したゲイン80%を探索する最大値とする	*/
	AdatV->MaxKv = ADAT_GRAT_80(ser_kv);

	switch ( down_ratio )
	{
		case 80:
			Prm->loophz = ADAT_GRAT_80( Prm->loophz );		/* ゲインを80%に下げる */
			break;

		case 50:
			Prm->loophz = ADAT_GRAT_50( Prm->loophz );		/* ゲインを50%に下げる */
			break;

		default:
			break;
	}
	TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );				/* ゲイン計算			*/
	return;
}


/****************************************************************************************************
		アドバンストオートチューニングノッチフィルタ設定状態

	概要:	周波数解析要求を立て、周波数解析後ノッチフィルタを設定する。
			ノッチフィルタは、最もピークの大きい周波数に対して設定する。
			Q値はKv，イナーシャから自動的に計算する。
			周波数解析異常の場合などで，ノッチ設定異常でノッチ無効のままの場合，
			探索ゲインを現在のゲインにセットし，今以上にゲインが上がらないようにする
****************************************************************************************************/
static PRM_RSLT	AdatNotchFilSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status)
{
	PRM_RSLT 		rc;
	USHORT			KvGain;
	PRMDATA 		*Prm;

	rc  = PRM_RSLT_SUCCESS;
	Prm = AdatP->UniPrms->Prm;
	AdatV->ErrState = ADAT_CONTINUE;

	switch ( AdatV->NotchSetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	初期設定																			*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			AdatV->NotchSetStat = ADAT_STS_NF_SETOK;		/* ノッチ設定ステータスの初期化		*/
			AdatV->MaxKvbk = Prm->loophz;					/* 振動したKvを保存					*/
			/* ノッチ解除処理、Ａ型解除処理 */
			if( (AdatNotchFilReset( AdatV, AdatP ) == TRUE )
					|| (AdatAresCtrlReset( AdatV, AdatP ) == TRUE) )
			{	/* ノッチが解除された→ノッチなしで再度ゲイン探索を行う	*/
				if(AdatV->NextSeqPtr > ADAT_SEQ_SEARCHOPTKV)
				{	/* Kv探索シーケンス以降は、ノッチ解除処理後Kv探索シーケンスを再実行	*/
					AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKV;
				}
				AdatV->NotchSetPtr = 4;						/* 終了設定へ						*/

				/* ノッチ解除後の調整シーケンスを先頭から実行するためシーケンスポインタクリア	*/
				AdatV->KvTunePtr  = 0;
				AdatV->KpTunePtr  = 0;
				AdatV->MfcTunePtr = 0;
				AdatV->KpSetPtr   = 0;
			}
			else if( ( (AdatV->VibMaxKv > Prm->loophz) && (AdatV->VibMaxKvAresOFF > Prm->loophz) )
						|| ( ((Prm->tfuncsw & 0x0F0F) == 0x0101) && ((Prm->avibsw & 0x0F) != 0x00) ) )
			{	/* ノッチorＡ型がONしたゲインより現Kvが小さい→ノッチorＡ型ONしてもゲインが上がらなかった   */
				/* または 2段ともノッチ有効、Ａ型制振有効													*/
				/* →共振抑制機能が利用できないので処理は行わない											*/
				AdatV->NotchSetStat = ADAT_STS_NF_SETERR;	/* ノッチ異常とする					*/
				AdatV->NotchSetPtr = 4;						/* 終了設定へ						*/
			}
			else
			{
				AdatV->f.FftReq    = TRUE;					/* 周波数解析要求ＯＮ				*/
				AdatV->NotchSetPtr = 5;						/* トレース完了待ちへ				*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	トレース完了待ち																	*/
		/*--------------------------------------------------------------------------------------*/
		case 5:
			if( DetVibTraceGetStatus( AdatP->DetVib ) == DETVIBTRC_TRCEND )
			{
				Prm->loophz = ADAT_GRAT_50( Prm->loophz );	/* ゲインを50%に下げる				*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );	/* ゲイン計算						*/
				AdatV->NotchSetPtr = 1;						/* 周波数解析待ちへ					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	周波数解析完了待ち																	*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			if( status == TUNE_FREQ_SEARCHEND )
			{	/* 周波数解析完了 */
				AdatV->f.FftReq = FALSE;						/* 待機状態へ					*/
				AdatV->NotchSetPtr = 2;							/* ノッチフィルタ設定へ			*/
				Prm->loophz = AdatV->MaxKvbk;					/* Kvを戻す						*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* ゲイン計算					*/
			}
			else if( status == TUNE_FREQ_SEARCHERR )
			{	/* エラー終了 */
				AdatV->f.FftReq = FALSE;						/* 待機状態へ					*/
				AdatV->NotchSetStat = ADAT_STS_NF_ANAERR;		/* 周波数解析異常を設定			*/
				AdatV->NotchSetPtr = 4;							/* 終了設定へ					*/
				Prm->loophz = AdatV->MaxKvbk;					/* Kvを戻す						*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* ゲイン計算					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	ノッチフィルタ設定																	*/
		/*--------------------------------------------------------------------------------------*/
		case 2:
				/*------------------------------------------------------------------------------*/
				/*	ノッチフィルタ設定															*/
				/*------------------------------------------------------------------------------*/
				if(AdatV->f.UsedAllNtFil == FALSE)
				{
					/*--------------------------------------------------------------------------*/
					/*	ノッチフィルタ or Ａ型制振設定条件判定									*/
					/*		ﾉｯﾁ設定のKv下限を設ける。最小KvでノッチQ値制限されるような周波数は	*/
					/*		A型制振を優先してONする												*/
					/*--------------------------------------------------------------------------*/
					if( (AdatV->f.UsedAResCtrl == FALSE)								/* A型制振調整可能			*/
							&& (FftGetAnaRsltVibmRf( AdatP->FftAna ) <= AVIB_FRQ_MAX)	/* 周波数がA型制振上限以下	*/
							&& (Prm->loophz < TUNE_ANOTCH_MINKV) )						/* ノッチON最小Kv以下		*/
					{
							KvGain = TUNE_ANOTCH_MINKV;				/* ノッチ設定最小Kv設定		*/
					}
					else
					{
							KvGain = Prm->loophz;					/* Kv設定値					*/
					}

					/* 第１ピーク周波数	*/
					AdatP->ANotch->ANotchData.conf.freq = FftGetAnaRsltVibmRf( AdatP->FftAna );
					AdatP->ANotch->ANotchData.conf.KvGain = KvGain;
					/* ノッチ設定ステータスセット		*/
					AdatV->ANotchSetStat = (CHAR)TuneNotchFilterSetting( AdatP->ANotch,
					                                                     Prm, AdatP->RegManager->hPnReg );
				}
				else
				{
					AdatV->ANotchSetStat = 0x10;		/* 全ノッチ使用中の場合ノッチエラーにする	*/
				}

				if( AdatV->ANotchSetStat > 0x02 )
				{	/* ノッチ設定異常	*/
					/*------------------------------------------------------------------------------*/
					/*	Ａ型制振設定																*/
					/*------------------------------------------------------------------------------*/
					if(AdatV->f.UsedAResCtrl == FALSE)
					{
//						AdatV->AResSetStat = (CHAR)TuneATypeResSet( FftGetAnaRsltVibmRf( AdatP->FftAna ),
//						                                            	((Prm->avibsw >> 4) & 0x0F), Prm );
						AdatV->AResSetStat = (CHAR)TuneATypeResSet( FftGetAnaRsltVibmRf( AdatP->FftAna ),
						                                            	((Prm->avibsw >> 4) & 0x0F), Prm,
																		AdatP->RegManager->hPnReg );/*<S09E>*/
					}
					else
					{
						AdatV->AResSetStat = 0x10;					/* Ａ型使用中はエラーにする		*/
					}

					if( AdatV->AResSetStat >= 0x10 )				/* Ａ型設定異常					*/
					{
						AdatV->NotchSetStat = ADAT_STS_NF_SETERR;	/* ノッチＱ値，Ａ型設定異常		*/
						AdatV->NotchSetPtr = 4;						/* 終了設定へ					*/
					}
					else
					{
						AdatV->NotchSetPtr = 3;						/* 振動収束待ちへ				*/
						KlibRstLongTimer( &AdatV->NotchSetCntr );	/* 振動収束待ちカウンタ初期化	*/
						AdatV->VibMaxKvAresOFF = Prm->loophz;		/* Ａ型OFFでの発振限界ゲイン取得*/
						AdatV->AResSetStatBk = AdatV->AResSetStat;	/* Ａ型設定したステータスを保存	*/
					}
				}
				else
				{	/* ノッチ設定正常	*/
					AdatV->NotchSetPtr = 3;							/* 振動収束待ちへ					*/
					KlibRstLongTimer( &AdatV->NotchSetCntr );		/* 振動収束待ちカウンタ初期化		*/
					AdatV->VibMaxKv = Prm->loophz;					/* ノッチOFFでの発振限界ゲイン取得	*/
					AdatV->ANotchSetStatBk = AdatV->ANotchSetStat;	/* ノッチ設定したステータスを保存	*/
				}
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* Tf計算のため必要				*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	ノッチ設定後の振動収束待ち															*/
		/*--------------------------------------------------------------------------------------*/
		case 3:
			if( KlibGetLongTimerMs( AdatV->NotchSetCntr ) >= ADATVIBWAITTM3 ) /* 振動収束待ち300ms */
			{
				AdatV->NotchSetPtr = 4;							/* 終了設定へ					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	終了設定																			*/
		/*--------------------------------------------------------------------------------------*/
		case 4:
			switch ( AdatV->NotchSetStat )
			{
				case ADAT_STS_NF_SETERR :				/* ノッチＱ値異常 → フィルタ設定未完	*/
				case ADAT_STS_NF_ANAERR:				/* 周波数解析異常						*/
					/* ゲインの再設定	*/
					AdatV->MaxKv = ADAT_GRAT_80( Prm->loophz );		/* 振動ゲインの80%に設定 	*/
					Prm->loophz  = ADAT_GRAT_80( Prm->loophz );		/* ゲインを80%に下げる	 	*/
					break;

				case ADAT_STS_NF_SETOK:
				default:
					break;
			}
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );	/* ゲイン計算 							*/
			AdatV->NotchSetPtr = 0;						/* 初期設定へ 							*/
			FunExe->State  = AdatV->NextSeqPtr;			/* 保存していたシーケンスポインタを次のポインタに設定 */
			AdatV->ErrState = ADAT_OK;					/* 正常終了	 							*/
			break;

		default :
			break;
	}
	return rc ;
}


/****************************************************************************************************
	ノッチフィルタ設定解除処理

	概要:	ノッチOFF時の発振限界ゲイン(AdatV.VibMaxKv)の62.5%以下
			になったら，第1，第2ノッチフィルタのうち最後に設定したノッチを解除する。
			どのノッチを解除するかは，tune_inf.notch_statusを参照する
			ノッチを解除した場合は，以降のチューニングでノッチをＯＮしないように
			AdatV.f.UsedAllNtFilを”設定すみ”にする
****************************************************************************************************/
static SHORT	AdatNotchFilReset( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA  *Prm;
	AUTONOTCH *ANotch;
	REGIF_CONFIG_T *RegCfg;
	REG_ACC_T AccessPrm;
	SHORT	rc;

	Prm = AdatP->UniPrms->Prm;
	ANotch = AdatP->ANotch;
	RegCfg = AdatP->RegManager->hPnReg;
	rc = FALSE;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	if( (Prm->tfuncsw & 0x0F0F) != 0x0000 )
	{	/* ONしているノッチがある	*/
		if( ADAT_GRAT_62( AdatV->VibMaxKv ) > Prm->loophz )
		{	/* ノッチOFF時の限界ゲインの62.5%以下になった	*/
			switch ( AdatV->ANotchSetStatBk )
			{	/* ノッチ完了ステータス	*/
				case 0x01:								/* 前回1段目ノッチON		*/
					/* 第1ノッチOFF	*/
					Prm->tfuncsw &= 0xFFF0;
					/* Pn408 */
					TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, Prm->tfuncsw, pndef_tfuncsw.DataLength );
					/* Pn409 */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );
					/* Pn40A */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1, 0 );
					rc = TRUE;
					break;

				case 0x02:								/* 前回2段目ノッチON		*/
					/* 第2ノッチOFF	*/
					Prm->tfuncsw &= 0xF0FF;
					/* Pn408 */
					TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, Prm->tfuncsw, pndef_tfuncsw.DataLength );
					/* Pn40C */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );
					/* Pn40D */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2, 0 );
					rc = TRUE;
					break;

				default:
					break;
			}
			TuneNotchQ( ANotch, RegCfg, Prm->loophz, Prm->jrate, 1, Prm );
			/*----------------------------------------------------------------------*/
			/*	ノッチ解除後の処理													*/
			/*----------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				AdatV->ANotchSetStat   = 0;				/* ノッチステータスクリア	*/
				AdatV->ANotchSetStatBk = 0;				/* ノッチステータスクリア	*/
				AdatV->f.UsedAllNtFil  = TRUE;			/* フィルタ設定済みにする	*/
			}
		}
	}
	return rc;
}

/****************************************************************************************************
	A型制振制御設定解除処理

	概要:	A型制振OFF時の発振限界ゲイン(AdatV.VibMaxKvAresOFF)の62.5%以下
			になったら、A型制振制御設定を解除する。
			解除した場合は、以降のチューニングでA型をONしないように
			AdatV.f.UsedAResCtrlを”設定すみ”にする

	 out		: SHORT		rc				:	FALSE	:解除無し
												TRUE	:Ａ型制振解除
****************************************************************************************************/
static SHORT	AdatAresCtrlReset( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;
	REG_ACC_T AccessPrm;
	SHORT	rc;
	USHORT	data;

	Prm    = AdatP->UniPrms->Prm;
	RegCfg = AdatP->RegManager->hPnReg;
	rc = FALSE;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	if( (Prm->avibsw & 0x0F) != 0x00 )
	{	/* A型制振ONしている	*/
		if( ADAT_GRAT_62( AdatV->VibMaxKvAresOFF ) > Prm->loophz )
		{	/* ノッチOFF時の限界ゲインの62.5%以下になった	*/
			switch ( AdatV->AResSetStatBk )
			{	/* ノッチ完了ステータス	*/
				case 0x01:							/* 前回 自動でA型制振ON			*/
					/* A型制振OFF */
					data = 0xFFFFFFF0 & Prm->avibsw;
					/* Pn160 */
					TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (LONG)data, pndef_avibsw.DataLength );
					/* Pn161 */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq, 0 );
					/* Pn163 */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp, 0 );
					rc = TRUE;
					break;

				default:
					break;
			}
			/*----------------------------------------------------------------------*/
			/*	Ａ型制振解除後の処理												*/
			/*----------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				AdatV->AResSetStat = 0;				/* ステータスクリア				*/
				AdatV->AResSetStatBk = 0;			/* ステータスクリア				*/
				AdatV->f.UsedAResCtrl = TRUE;		/* Ａ型制振自動設定済みにする	*/
			}
		}
	}
	return rc;
}


/****************************************************************************************************
		アドバンストオートチューニングKpゲイン設定状態

	概要:	最適ゲイン探索後、Kp探索、Kpm探索前に
			Kpを徐々に大きくする。
			Kv探索中はKpを低く設定しているのでKp、Kpm探索開始
			時にKpをいきなり大きな値に設定したときのショック低減。
****************************************************************************************************/
static PRM_RSLT	AdatKpGainSet(ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA	*Prm;
	PRM_RSLT rc;
	UCHAR tmpGainRatio;

	Prm  = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;										/* 振動処理後ゲイン割合 初期値=80% */

	/* ScanC処理でエラー発生した場合 */
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->KpSetPtr = 0;								/* 初期設定へ 						*/
		AdatV->ErrState = ADAT_NG;							/* 異常終了   						*/
	}

	switch ( AdatV->KpSetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	初期設定																			*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			AdatV->f.DrvPatternSet  = FALSE;
			AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* プログラムJOG運転パターン3		*/

			/* 振動検出フィルタ設定	*/
			if( Prm->loophz > VIBOBS_KS )
			{/* HPFセット	*/
				DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, Prm->loophz,
				                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs);
			}
			else
			{/* HPFセット	*/
				DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
				                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
			}
			AdatP->FnCmnCtrl->State.TuningPctrlRun = TRUE;			/* 位置ループ探索開始		*/
			AdatV->KpSetPtr = 1;									/* Kp設定要求へ				*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kp設定要求																			*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			/* 設定する最大Kpを設定		*/
			AdatV->MaxKp = Prm->loophz;						/* 設定Kp最大値は現Kvと同じ			*/
			AdatScanCRequest( ADAT_SEQ_KPSET, AdatV );		/* スキャンC処理要求				*/
			AdatV->KpSetPtr = 2;							/* Kp設定状態へ						*/
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	Kp設定状態																			*/
		/*--------------------------------------------------------------------------------------*/
		case 2:
			switch ( status )
			{
				case ADAT_SEAR_NORMALEND :							/* 正常終了					*/
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;		/* プログラムJOG終了		*/
					AdatV->KpSetPtr = 10	;						/* 処理終了へ				*/
					break;
				case ADAT_SEAR_NOVIBDETECT :						/* 振動無し					*/
					/* KpゲインUP */
					TuneAdatSetPosCtrlGain( GAIN_UP, AdatP );
					AdatV->KpSetPtr = 1;
					break;
				case ADAT_SEAR_STOPVIBEND:							/* 停止時振動検出終了		*/
					/* ゲイン割合を50%に変更 */
					tmpGainRatio = 50;
				/* Fall Through */
				case ADAT_SEAR_VIBEND:								/* 振動検出終了				*/
					/* KvゲインDOWN */
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}
					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* 既にノッチフィルタ設定済み → 再実行	*/
						/* 振動後処理 */
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->KpSetCntr );
						AdatV->KpSetPtr = 3;
						AdatV->NextSubSeqPtr = 1;
					}
					else
					{	/* ノッチフィルタ設定未完	*/
						AdatV->KpSetPtr = 1;
						AdatV->NextSeqPtr = ADAT_SEQ_KPSET;
						FunExe->State = ADAT_SEQ_NOTCHSET;
						AdatV->ErrState = ADAT_OK;					/* 正常終了					*/
					}
					break;
					
				default :
					break;
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	振動収束待ち状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case 3:
			if( KlibGetLongTimerMs( AdatV->KpSetCntr ) >= ADATVIBWAITTM2 ) /* 振動収束待ち処理 300ms	*/
			{
				AdatV->KpSetPtr = AdatV->NextSubSeqPtr;
				Prm->loophz = AdatV->MaxKv;							/* 探索ゲイン最大値に戻す	*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* ゲイン計算				*/
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	終了処理																			*/
		/*--------------------------------------------------------------------------------------*/
		case 10:
			AdatScanCRequest( ADAT_NO_OPE, AdatV );				/* 処理終了後は待機状態を要求	*/
			AdatV->KpSetPtr = 0;								/* シーケンスクリア				*/
			AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;

			if( ( AdatReg->AdatLevel == ADAT_LEVEL_POS )
					|| ( AdatReg->AdatLevel == ADAT_LEVEL_POS_ST ) )
			{
				FunExe->State = ADAT_SEQ_ADJMFC;				/* モデル追従制御調整処理		*/
			}
			else
			{
				FunExe->State = ADAT_SEQ_SEARCHOPTKP;			/* Kp探索処理					*/
			}
			break;

		default :
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc ;
}



/****************************************************************************************************
	位置ループゲイン変更処理

	概要:	位置ループゲインを要求に応じて変更する。
****************************************************************************************************/
static SHORT	TuneAdatSetPosCtrlGain( LONG mode, ADATP *AdatP)
{
	PRMDATA		*Prm;
	SHORT		rc;
	LONG		data;
	REG_ACC_T 	AccessPrm;

	Prm = AdatP->UniPrms->Prm;
	rc  = ADAT_OK;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( mode )
	{
		case GAIN_UP:
			data = Prm->posgn + KPUP_UNIT;			/* 3.0 [1/s] 増加							*/
			break;

		case GAIN_DW:
			data = Prm->posgn - KPDOWN_UNIT;		/* 3.0 [1/s] 減少							*/
			break;

		case GAIN_BAL:
		case GAIN_BAL2:
			/*----------------------------------------------------------------------------------*/
			/*	位置ループゲイン(Kp)															*/
			/*----------------------------------------------------------------------------------*/
			if( AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl == TRUE )
			{	/* IP制御	*/
				data = 320000 / Prm->pitime;		/*  Kp[1/s] = 0.32 / Ti[s]					*/
			}
			else
			{	/* PI制御	*/
				data = ADAT_GRAT_150( Prm->loophz );/* Kp[1/s] = Kv[rad/s] / 4 ≒ 1.5 *Kv[Hz]	*/
			}

			if( mode == GAIN_BAL2 )
			{
				if( data >= Prm->posgn )
				{
					return rc;						/* 現在設定値以上の場合は、何もしない		*/
				}
			}
			break;

		case GAIN_BAL3:								/* モデル追従制御時Kpバランス				*/
		case GAIN_BAL4:
			/*----------------------------------------------------------------------------------*/
			/*	位置ループゲイン(Kp)															*/
			/*----------------------------------------------------------------------------------*/
			data = TuneKvFuncKp( Prm->loophz, Prm->pitime, AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl );

			if( mode == GAIN_BAL4 )
			{
				if( data >= Prm->posgn )
				{
					return rc;						/* 現在設定値以上の場合は、何もしない		*/
				}
			}
			break;

		default:
			data = Prm->posgn;						/* 現状のまま							    */
			break;
	}

	if( data < KP_UNLMT )
	{
		data = KP_UNLMT;							/* 下限値を超える場合は下限値とする			*/
		rc = ADAT_NG;								/* 異常終了とする							*/
	}
	else if( data > pndef_posgn.UpperLimit )
	{
		data = pndef_posgn.UpperLimit;				/* 上限値を超える場合は上限値とする			*/
	}

	TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)data, pndef_posgn.DataLength );
	return rc;
}

/****************************************************************************************************
	アドバンストオートチューニング最適Kpゲイン探索状態

	概要:	最適Kpゲイン探索をスキャンCに要求する。
****************************************************************************************************/
static PRM_RSLT	AdatOptKpSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA	 *Prm;
	PRM_RSLT rc;
	LONG user_maxGain;								/* ユーザレベルに応じた探索最大ゲイン割合		*/
	UCHAR tmpGainRatio;

	Prm = AdatP->UniPrms->Prm;
	rc  = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;								/* 振動処理後ゲイン割合 初期値=80%				*/

	AdatV->ErrState = ADAT_CONTINUE;

	/* ScanC処理でエラー発生した場合 */
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->KpTunePtr = 0;						/* 初期設定状態へ								*/
		AdatV->ErrState = ADAT_NG;					/* 異常終了										*/
	}

	switch ( AdatV->KpTunePtr )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	初期設定																				*/
		/*------------------------------------------------------------------------------------------*/
		case ADAT_OPTKP_INI:
			/* モータ停止検出(制御切替時のショック抑制のため) */
			if( (AdatV->PrgJogStat == ADAT_PJOG_NOP) && (AdatP->SeqCtrlOut->MotStop == TRUE) )
			{	/* プログラムJOG運転完了	*/
				/* 探索ゲイン最大値設定		*/
				user_maxGain = ADAT_LVL_80;						/* 80％ゲイン						*/
				AdatV->MaxKv = Prm->loophz;						/* 現在のゲインを探索最大ｹﾞｲﾝに設定	*/
#if (FLOAT_USE==TRUE)
				Prm->loophz = (USHORT)( (LONG)AdatV->MaxKv * user_maxGain );

				/* ノッチOFF時の発振限界ゲインを更新。												*/
				/* ノッチ解除処理でPrm->loophzと比較するためuser_maxGainに応じて再計算する			*/
				AdatV->VibMaxKv = (USHORT)( (LONG)AdatV->VibMaxKv * user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)( (LONG)AdatV->VibMaxKvAresOFF * user_maxGain );
#else
				Prm->loophz = (USHORT)MlibMulgain( (LONG)AdatV->MaxKv, user_maxGain );

				/* ノッチOFF時の発振限界ゲインを更新。												*/
				/* ノッチ解除処理でPrm->loophzと比較するためuser_maxGainに応じて再計算する			*/
				AdatV->VibMaxKv = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKv, user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKvAresOFF, user_maxGain );
#endif /* FLOAT_USE */

				/* ゲイン設定	*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* ゲイン計算     					*/
				TuneAdatSetPosCtrlGain( GAIN_BAL, AdatP );		/* Kpゲイン計算 					*/

				/* 振動レベル設定 					 */
				/* ゲイン探索時は通常振動レベルの2倍 */
				AdatP->DetVib->conf.VibCheckLevel = AdatV->VibSpeed * RESGMAX / 100;

				/* 発振検出用オブザーバゲイン変更(Kv > 80[Hz])	*/
				AdatP->FnCmnCtrl->State.TuningPctrlRun = TRUE;	/* 位置ループ探索開始				*/
				if( Prm->loophz > VIBOBS_KS )
				{	/* ObsGain = Kv	*/
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, Prm->loophz,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}
				else
				{
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}

				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* プログラムJOG運転パターン3		*/
				AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;			/* 最適Kpゲイン探索要求へ			*/
				AdatV->f.VibDetectinCycle = TRUE;				/* 振動ステータス初期化。			*/
																/* 初期値は振動あり					*/
			}
			break;

			/*--------------------------------------------------------------------------------------*/
			/*	最適Kpゲイン探索要求																*/
			/*--------------------------------------------------------------------------------------*/
			case ADAT_OPTKP_CHK_REQ:
				AdatScanCRequest( ADAT_SEQ_SEARCHOPTKP, AdatV );/* スキャンC処理要求				*/
				AdatV->KpTunePtr = ADAT_OPTKP_CHK;				/* 最適ゲイン探索へ					*/
				break;

			/*--------------------------------------------------------------------------------------*/
			/*	最適Kpゲイン探索状態																*/
			/*--------------------------------------------------------------------------------------*/
			case ADAT_OPTKP_CHK:
				switch ( status )
				{
					case ADAT_SEAR_STOPVIBEND:					/* 停止時振動検出終了				*/
						tmpGainRatio = 50;						/* ゲイン割合を50%に変更			*/
					/* Fall Through */

					case ADAT_SEAR_VIBEND:						/* 振動検出終了						*/
						AdatV->f.VibDetectinCycle = TRUE;		/* 振動ステータスＯＮ				*/
						if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* KvゲインDWON */
						{
							AdatV->ErrState = ADAT_NG;
							return rc;
						}
						TuneAdatSetPosCtrlGain( GAIN_BAL2, AdatP ); /* Kpゲイン計算 				*/

						if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
						{	/* 既にノッチフィルタ設定済み → 処理続行 */
							AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
							KlibRstLongTimer( &AdatV->KpTuneCntr );
							AdatV->KpTunePtr = ADAT_OPTKP_WAIT;
							AdatV->NextSubSeqPtr = ADAT_OPTKP_CHK_REQ;
																/* 振動後処理						*/
						}
						else
						{	/* ノッチフィルタ設定未完	*/
							AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;
							AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKP;
							FunExe->State = ADAT_SEQ_NOTCHSET;
							AdatV->ErrState = ADAT_OK;			/* 正常終了							*/
						}
						break;

					case ADAT_SEAR_COINBREAKEND:				/* COIN割れ検出終了					*/
						AdatV->f.VibDetectinCycle = TRUE;		/* 振動ステータスＯＮ				*/
						AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;	/* 振動していたら再トライ			*/

						if( TuneAdatSetPosCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* KpゲインDOWN */
						{
							AdatV->ErrState = ADAT_NG;
							return rc ;
						}
						break;

					default :
						break;
				}
				/*----------------------------------------------------------------------------------*/
				/*	運転パターン終了																*/
				/*----------------------------------------------------------------------------------*/
				if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
				{	/* プログラムJOG運転完了 */
					if( (AdatV->f.VibDetectinCycle == TRUE )
							|| ( AdatV->f.DrvPatternSet == FALSE ) ) /* 指令パターン未完了			*/
					{	/* 今回運転中に振動検出した	*/
						AdatV->f.VibDetectinCycle = FALSE;			 /* 振動ステータスクリア		*/
						AdatV->f.DrvPatternSet = FALSE;
						AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;/* プログラムJOG運転パターン3	*/
						AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;		 /* 振動していたら再トライ		*/
					}
					else
					{	/* 今回運転中振動しなかった → 探索完了 → 終了設定へ	*/
						AdatScanCRequest( ADAT_NO_OPE, AdatV );		/* 処理終了後は待機状態を要求	*/
						AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;	/* プログラムJOG運転停止		*/
						AdatV->KpTunePtr = ADAT_OPTKP_INI;			/* 次回シーケンスポインタセット	*/
						FunExe->State = ADAT_SEQ_ENDDISP;			/* 次回チューニングシーケンスセット	*/
						AdatV->ErrState = ADAT_OK;					/* 正常終了						*/
					}
				}
				break;

			/*--------------------------------------------------------------------------------------*/
			/*	振動収束待ち状態																	*/
			/*--------------------------------------------------------------------------------------*/
			case ADAT_OPTKP_WAIT:
				/* 振動収束待ち処理	300ms */
				if( KlibGetLongTimerMs( AdatV->KpTuneCntr ) >= ADATVIBWAITTM2 )
				{
					AdatV->KpTunePtr = AdatV->NextSubSeqPtr;
					Prm->loophz = AdatV->MaxKv;							/* 探索ゲイン最大値に戻す	*/
					TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* ゲイン計算				*/
				}
				break;

			default :
				AdatV->ErrState = ADAT_NG;
				break;
		}
		return rc ;
}

/****************************************************************************************************
		アドバンストオートチューニングモデル追従制御調整状態

	概要:	モデル追従制御を有効として，モデルループゲイン(Kpm)および
			トルクＦＦの調整を行う。その他の関連パラメータは，固定値として調整しない
****************************************************************************************************/
static PRM_RSLT	AdatMfcAdjustment( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	MFCTRL   *MFControl;
	PRMDATA	 *Prm;
	REG_ACC_T AccessPrm;
	PRM_RSLT rc;
	LONG	 user_maxGain;
	LONG	 UnRvibFrequency;
	LONG     SettlingTime;
	UCHAR	 tmpGainRatio;
	USHORT	 edata;
	USHORT	 ret0;

	MFControl = &AdatP->BaseCtrlData->MFControl;
	Prm = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;					/* 振動処理後ゲイン割合 初期値=80% 						*/
	UnRvibFrequency = RemVibGetRemVibFreq( AdatP->RemVibFreq );

	AdatV->ErrState = ADAT_CONTINUE;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;


	/*	ScanC処理でエラー発生した場合	*/
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->MfcTunePtr = 0;			/* 初期設定状態へ										*/
		AdatV->ErrState = ADAT_NG;		/* 異常終了												*/
	}

	switch ( AdatV->MfcTunePtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	初期設定																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_INI:
			/* モータ停止検出(制御切替時のショック抑制のため) */
			if( AdatP->SeqCtrlOut->MotStop == TRUE )
			{	/* プログラムJOG運転完了	*/
				AdatV->MaxKv = Prm->loophz;				/* 現在のゲインを探索最大ゲインに設定	*/
				user_maxGain = ADAT_LVL_80;				/* 探索最大ゲイン割合					*/

#if (FLOAT_USE==TRUE)
				Prm->loophz = (USHORT)( (LONG)AdatV->MaxKv * user_maxGain );
				/*	ノッチOFF時の発振限界ゲインを更新。											*/
				/*	ノッチ解除処理でPrm->loophzと比較するためADAT_LVL_80で再計算する			*/
				AdatV->VibMaxKv = (USHORT)( (LONG)AdatV->VibMaxKv * user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)( (LONG)AdatV->VibMaxKvAresOFF * user_maxGain);
#else
				Prm->loophz = (USHORT)MlibMulgain( (LONG)AdatV->MaxKv, user_maxGain );
				/*	ノッチOFF時の発振限界ゲインを更新。											*/
				/*	ノッチ解除処理でPrm->loophzと比較するためADAT_LVL_80で再計算する			*/
				AdatV->VibMaxKv = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKv, user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKvAresOFF,user_maxGain);
#endif /* FLOAT_USE */

				/* ゲイン設定	*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* ゲイン計算					*/
				TuneAdatSetPosCtrlGain( GAIN_BAL3, AdatP );		/* Kpゲイン計算					*/

				/* 検索最大Kpm初期化	*/
				AdatV->MaxKpm = Prm->adatgmax << 1;				/* 初期値は、Pn540×2			*/

				/*------------------------------------------------------------------------------*/
				/*	モデル追従設定																*/
				/*------------------------------------------------------------------------------*/
				/* パラメータ初期設定					*/
				/*	ANTFRQ，RESFRQは設定値を使用		*/
				/*	外部指令では，Kpm設定値から調整開始	*/
				switch ( AdatReg->AdatLevel )
				{	/* チューニングレベル */
					case ADAT_LEVEL_POS:				/* 位置決め専用							*/
//						TuneMfcActive( Prm );
						TuneMfcActive( Prm, AccessPrm.hRegCfg );//<S09E>
						break;
					case ADAT_LEVEL_POS_ST:				/* 位置決め専用(オーバーシュート重視)	*/
//						TuneMfcActiveStMode( Prm );
						TuneMfcActiveStMode( Prm, AccessPrm.hRegCfg );//<S09E>
						break;
					default:
						break;
				}
				if( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
				{	/* AAT前二慣性MFCが有効	*/
					Prm->mdlgn = TuneMfcKpmCalcAntFreq( Prm->mdlantfrq ) / 2;
				}
				else
				{	/* AAT前MFC無効or剛体MFC */
					Prm->mdlgn = Prm->posgn;
				}
				/* Kpm設定 								*/
				TuneAdatSetMdlCtrlGain( GAIN_BAL, AdatP );
				/* 振動レベル設定 						*/
				/* ゲイン探索時は通常振動レベルの2倍 	*/
				AdatP->DetVib->conf.VibCheckLevel = AdatV->VibSpeed * RESGMAX / 100;
				/* 発振検出用オブザーバゲイン変更(Kv > 80[Hz])	*/
				/* 位置ループ探索開始					*/
				AdatP->FnCmnCtrl->State.TuningPctrlRun = TRUE;
				if( Prm->loophz > VIBOBS_KS )
				{	/* ObsGain = Kv	*/
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, Prm->loophz,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}
				else
				{
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}

				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;	/* プログラムJOG運転パターン1	*/
				AdatV->MfcTunePtr = ADAT_MFC_INIWAIT;			/* ゲイン変更による振動収束待ち	*/
				AdatV->f.VibDetectinCycle = FALSE;				/* 振動ステータスクリア			*/
				RemVibRsetRemVibrationFreq( AdatP->RemVibFreq );/* 残留振動周波数クリア			*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	ゲイン変更による振動収束待ち														*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_INIWAIT:
			if( (MFControl->conf.MfcPexe.MFCModel == MFControl->conf.MfcPrm[0].MFCModel)/* 実行モデルがパラメータと同じか？			*/
				&& (MFControl->conf.MfcPexe.Kj2   == MFControl->conf.MfcPrm[0].Kj2)		/* 実行モデル周波数がパラメータと同じか？	*/
				&& (MFControl->conf.MfcPexe.Kf2   == MFControl->conf.MfcPrm[0].Kf2) )	/* 実行モデルゲインがパラメータと同じか？	*/
			{
				BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN割れ時間クリア				*/
				BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* オーバーシュート量最大値クリア	*/
				AdatV->MfcTunePtr = ADAT_MFC_MAXREQ;		/* Kpmチューニング要求へ			*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kpmチューニング要求																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_MAXREQ:
			AdatScanCRequest( ADAT_SEQ_ADJMFC, AdatV );		/* スキャンC処理要求				*/
			AdatV->MfcTunePtr = ADAT_MFC_MAXSEARCH;			/* Kpmチューニングへ				*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kpmチューニング状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_MAXSEARCH:
			switch ( status )
			{
				case ADAT_SEAR_STOPVIBEND:				/* 停止時振動検出終了					*/
					tmpGainRatio = 50;					/* ゲイン割合を50%に変更				*/
				/* Fall Through */
				case ADAT_SEAR_VIBEND:					/* 振動検出終了							*/
					AdatV->f.VibDetectinCycle = TRUE;	/* １回は全運転パターンを動作させるため */
														/*	初期値を振動ステータスＯＮとする	*/
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* KvゲインDOWN	*/
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}
					TuneAdatSetPosCtrlGain( GAIN_BAL4, AdatP ); /* Kpゲイン計算	*/

					/*--------------------------------------------------------------------------*/
					/*	Kpm探索中に振動したらKpm=KpとしてKpm最大値を再探索						*/
					/*--------------------------------------------------------------------------*/
					if(Prm->mdlgn > Prm->posgn)								/* Kpm > Kp			*/
					{
						Prm->mdlgn = Prm->posgn;							/* Kpm ← Kp		*/
						TuneAdatSetMdlCtrlGain( GAIN_BAL, AdatP );			/* Kpm設定			*/
						/* 検索Kpm最大値を1/2にする	*/
						AdatV->MaxKpm = MlibMAX( (AdatV->MaxKpm >> 1), Prm->mdlgn );
					}

					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* 既にノッチフィルタ設定済み → COIN割れ確認要求へ	*/
						/* 振動後処理 */
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->MfcTuneCntr );
						AdatV->MfcTunePtr = ADAT_MFC_WAIT;
						AdatV->NextSubSeqPtr = ADAT_MFC_MAXREQ;
					}
					else
					{	/* ノッチフィルタ設定未完	*/
						AdatV->MfcTunePtr = ADAT_MFC_MAXREQ;
						AdatV->NextSeqPtr = ADAT_SEQ_ADJMFC;
						FunExe->State = ADAT_SEQ_NOTCHSET;
						AdatV->ErrState = ADAT_OK;				/* 正常終了							*/
					}
					break;

				case ADAT_SEAR_COINBREAKEND:					/* COIN割れ検出終了					*/
					AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;		/* COIN割れ確認要求へ				*/
					AdatV->f.VibDetectinCycle = TRUE;			/* 1回は全運転パターンを動作させるため*/
																/* 初期値を振動ステータスONとする	*/
					TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* KpmゲインDOWN				*/
					break;

				case ADAT_SEAR_COINOK :
					TuneAdatSetMdlCtrlGain( GAIN_UP, AdatP );	/* KpmゲインUP						*/
					AdatV->MfcTunePtr = ADAT_MFC_MAXREQ;		/* Kpmチューニング確認要求へ		*/

					/*------------------------------------------------------------------------------*/
					/*	Kpm上限リミット処理															*/
					/*------------------------------------------------------------------------------*/
					if( (Prm->mdlgn > AdatV->MaxKpm) || (Prm->mdlgn == pndef_mdlgn.UpperLimit) )
					{	/* 上限値を超えた	*/
						AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;
						AdatV->f.VibDetectinCycle = TRUE;	/* 1回は全運転パターンを動作させるため	*/
															/* 初期値を振動ステータスONとする		*/
						AdatV->ErrState = ADAT_OK;
					}
					/*------------------------------------------------------------------------------*/
					/*  2慣性MFC有効にも関わらず、低周波振動を検出した場合は、ゲインを下げる		*/
					/*------------------------------------------------------------------------------*/
					if( (UnRvibFrequency >= VIBMFC_DET_FRQ_MIN)		/* 検出周波数 >= 振動検出下限値	*/
						&& (UnRvibFrequency <= VIBMFC_DET_FRQ_MAX)	/* 検出周波数 <= 振動検出上限値	*/
						&& (MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS) ) /* 2慣性MFC有効 	*/
					{
						AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;		/* COIN割れ確認要求へ					*/
						AdatV->f.VibDetectinCycle = TRUE;			/* 1回は全運転パターンを動作させるため，*/
																	/* 初期値を振動ステータスONとする		*/
						TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* KpmゲインDOWN						*/
					}
					if( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
					{	/* 2慣性MFC or 剛体＋機台MFC	*/
						if( Prm->mdlgn > (3 * Prm->mdlantfrq) )
						{	/* 上限値を超えた */
							AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;	/* COIN割れ確認要求へ					*/
							AdatV->f.VibDetectinCycle = TRUE;		/* 1回は全運転パターンを動作させるため	*/
																	/* 初期値を振動ステータスONとする		*/
							TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* KpmゲインDOWN					*/
						}
					}
					break;

				default :
					break;
			}
			/* 2慣性MFC設定をノッチ設定、COIN割れ検出より優先する */
			if( AdatV->f.Used2MassOld == FALSE )
			{
				ret0 = SetMfc2MassSequence( AdatReg, AdatV, AdatP, FunExe, ADAT_SEQ_ADJMFC );
				if( ret0 )
				{
					FunExe->State = ADAT_SEQ_2MASSSET;	/* モデル追従(剛体+機台モデル)設定処理へ*/
					AdatV->NextSeqPtr = ADAT_SEQ_ADJMFC;/* 設定後元の処理へ戻るためのバッファ	*/
					AdatV->f.VibDetectinCycle = TRUE;	/* 振動ステータスON						*/
				}
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	振動収束待ち状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_WAIT:
			/* 振動収束待ち処理	300ms */
			if( KlibGetLongTimerMs( AdatV->MfcTuneCntr ) >= ADATVIBWAITTM2 )
			{
				AdatV->MfcTunePtr = AdatV->NextSubSeqPtr;
				Prm->loophz = AdatV->MaxKv;						/* 探索ゲイン最大値に戻す		*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* ゲイン計算					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	COIN割れ確認要求状態																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_CHK_REQ:
			AdatV->settling_max = 0;							/* 整定時間の最大値をリセット	*/
			AdatScanCRequest( ADAT_VIBCOIN, AdatV );			/* スキャンC処理要求			*/
			AdatV->MfcTunePtr = ADAT_MFC_CHK;					/* COIN割れ確認へ				*/
			MfcPrmReSet( AdatV, AdatP );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	COIN割れ確認状態																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_CHK:
			switch ( status )
			{
				case ADAT_SEAR_STOPVIBEND:						/* 停止時振動検出終了			*/
					tmpGainRatio = 50;							/* ゲイン割合を50%に変更		*/
				/* Fall Through */
				case ADAT_SEAR_VIBEND:							/* 振動検出終了					*/
					AdatV->f.VibDetectinCycle = TRUE;			/* 振動ステータスON				*/
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG ) /* KvゲインDOWN	*/
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}
					TuneAdatSetPosCtrlGain( GAIN_BAL4, AdatP );	/* Kpゲイン計算					*/
					TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* KpmゲインDOWN				*/

					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* 既にノッチフィルタ設定済み → 処理続行	*/
						/* 振動後処理 */
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->MfcTuneCntr );
						AdatV->MfcTunePtr = ADAT_MFC_WAIT;
						AdatV->NextSubSeqPtr = ADAT_MFC_CHK_REQ;
					}
					else
					{	/* ノッチフィルタ設定未完	*/
						AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;
						AdatV->NextSeqPtr = ADAT_SEQ_ADJMFC;
						FunExe->State = ADAT_SEQ_NOTCHSET;
						AdatV->ErrState = ADAT_OK;					/* 正常終了					*/
					}
					break;

				case ADAT_SEAR_COINBREAKEND:						/* COIN割れ検出終了			*/
					AdatV->f.VibDetectinCycle = TRUE;				/* 振動ステータスON			*/
					AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;			/* 振動していたら再トライ	*/

					if( TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP) == ADAT_NG  )	/* KpmゲインDOWN */
					{
						AdatV->ErrState = ADAT_NG;
						Mfc2MassReset( AdatV, AdatP );
						return rc ;
					}
					break;

				default :
					break;
			}
			/* 整定時間の最大値を保持する */
			SettlingTime = BpiGetSettlingTime( AdatP->SettlingTime );
			if( AdatV->settling_max < SettlingTime )
			{
				AdatV->settling_max = SettlingTime;
			}

			/*----------------------------------------------------------------------------------*/
			/*	運転パターン終了																*/
			/*----------------------------------------------------------------------------------*/
			if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
			{	/* プログラムJOG運転完了 */
				if( (AdatV->f.VibDetectinCycle == TRUE )
						|| ( AdatV->f.DrvPatternSet == FALSE ) )	/* 指令パターン未完了		*/
				{	/* 今回運転中に振動検出した	*/
					AdatV->f.VibDetectinCycle = FALSE;				/* 振動ステータスクリア		*/
					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* プログラムJOG運転パターン3 */
					AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;			/* 振動していたら再トライ	  */
				}
				else
				{	/* 今回運転中振動しなかった → 調整完了 → 終了設定 */
					if( ( AdatV->f.Used2MassMfc == TRUE )
						&& ( AdatV->f.Mfc2MassOK == FALSE )
						&& ( AdatV->f.MfcLastSet == FALSE )
						&& ( AdatV->f.Used2MassOld == FALSE ) )
					{
					/*--------------------------------------------------------------------------*/
					/*		Kv調整未完状態(通常振動レベル測定シーケンス)で振動抑制ONした場合は	*/
					/*		剛体MFCでの整定時間確認動作は行わない								*/
					/*--------------------------------------------------------------------------*/
						if(AdatV->Set2MassMfcSequence == ADAT_SEQ_ADJMFC)
						{	/* MFC調整中に振動抑制設定を行った					*/
							/* 現在剛体+機台モデルになっているかどうかチェック	*/
							if( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
							{
								FunExe->State = ADAT_SEQ_ADJMFC;			/* 次回チューニングシーケンスセット	*/
								PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
								edata = (USHORT)((Prm->mdlsw) & (RIGMFC_PRM_ACTIVE));	/* Pn140.1 -> 0 */
								TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)edata, pndef_mdlsw.DataLength );

								AdatV->MfcTunePtr   = ADAT_MFC_INI;			/* 次回シーケンスポインタセット		*/
								AdatV->settling_buf = AdatV->settling_max;	/* 整定時間をバッファ				*/
								AdatV->Kpm_buf = (USHORT)Prm->mdlgn;		/* Kpmをバッファ					*/
								AdatV->frq_buf = (USHORT)Prm->mdlantfrq;	/* 共振・反共振周波数をバッファ		*/
								AdatV->Kv_buf  = (USHORT)Prm->loophz;		/* Kvをバッファ						*/
							}
							else
							{
								/* MFC(剛体+機台モデル)の整定時間が短い場合はパラメータを再設定	 */
								if( AdatV->settling_max > AdatV->settling_buf )
								{
									/* パラメータをMFC(剛体+機台モデル)に変更*/
									TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)AdatV->Kpm_buf, pndef_mdlgn.DataLength );			/* Pn141 */
									TunePrmWriteRam( &AccessPrm, &pndef_mdlantfrq, (LONG)AdatV->frq_buf, pndef_mdlantfrq.DataLength );	/* Pn145 */
									TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, (LONG)AdatV->frq_buf, pndef_mdlresfrq.DataLength );	/* Pn146 */

									PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
									edata =(USHORT)( ( Prm->mdlsw ) | ( VIBMFC_PRM_ACTIVE ) );
									TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)edata, pndef_mdlsw.DataLength );

									Prm->loophz = AdatV->Kv_buf;
									TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );
									TuneAdatSetPosCtrlGain( GAIN_BAL3, AdatP );
								}
								/*------------------------------------------------------------------------------*/
								/* 終了処理																		*/
								/*------------------------------------------------------------------------------*/
								/*	2慣性MFC設定での調整が完了。最終的に選択される機構モデルに関係なく、		*/
								/*	AdatV->f.Mfc2MassOK = TRUEとなる											*/
								/*------------------------------------------------------------------------------*/
								AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;	/* COIN割れ確認要求へ					*/
								AdatV->f.VibDetectinCycle = TRUE;		/* 1回は全運転パターンを動作させるため，*/
								AdatV->f.Mfc2MassOK = TRUE;
							}
						}
						else
						{
							AdatV->f.Mfc2MassOK = TRUE;					/* 2慣性最終調整完了					*/
						}
					}
					else
					{
						AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;		/* プログラムJOG運転停止				*/
						AdatScanCRequest( ADAT_NO_OPE, AdatV );			/* 処理終了後は待機状態を要求			*/
						AdatV->MfcTunePtr = ADAT_MFC_INI;				/* 次回シーケンスポインタセット			*/
						FunExe->State = ADAT_SEQ_ENDDISP;				/* 次回チューニングシーケンスセット		*/
						AdatV->ErrState = ADAT_OK;						/* 正常終了								*/
					}
				}
			}
			break;

		default :
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc ;
}

/****************************************************************************************************
	モデルループゲイン変更処理

	概要:モデルループゲインを要求に応じて変更する。
****************************************************************************************************/
static SHORT	TuneAdatSetMdlCtrlGain( LONG mode, ADATP *AdatP )
{
	PRMDATA *Prm;
	REG_ACC_T AccessPrm;
	LONG	data;
	SHORT	rc;

	Prm = AdatP->UniPrms->Prm;
	rc  = ADAT_OK;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( mode )
	{
		case GAIN_UP:
			data = ADAT_GRAT_106( Prm->mdlgn );		/* 17/16 = 1.0625 6.25%増加				*/
			break;

		case GAIN_DW:
			data = ADAT_GRAT_94( Prm->mdlgn );		/* 15/16 = 0.9375 6.25%減少				*/
			break;

		default:
			data = Prm->mdlgn;						/* 現状のまま							*/
			break;
	}

	if( data < KPM_UNLMT )
	{
		data = KPM_UNLMT;							/* 下限値を超える場合は下限値とする		*/
		rc = ADAT_NG;								/* 異常終了とする						*/
	}
	else if( data > pndef_mdlgn.UpperLimit )
	{
		data = pndef_mdlgn.UpperLimit;				/* 上限値を超える場合はクランプとする	*/
	}
	TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)data, pndef_mdlgn.DataLength );
	return rc;
}


/****************************************************************************************************
		MFC(剛体+機台モデル)設定に戻す
		剛体MFCで調整し、振動のため最小Kpmになってしまった場合、設定を2慣性に戻す
****************************************************************************************************/
static void	MfcPrmReSet( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA *Prm;
	USHORT	edata;
	REG_ACC_T AccessPrm;

	Prm = AdatP->UniPrms->Prm;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* MFC(剛体+機台モデル)調整済みであればそれを設定する	*/
	if( AdatV->f.MfcLastSet == TRUE )
	{
		/* パラメータをMFC(剛体+機台モデル)に変更*/
		/* Pn141 */
		TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)AdatV->Kpm_buf, pndef_mdlgn.DataLength );
		/* Pn145 */
		TunePrmWriteRam( &AccessPrm, &pndef_mdlantfrq, (LONG)AdatV->frq_buf, pndef_mdlantfrq.DataLength );
		/* Pn146 */
//		TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, (LONG)AdatV->Kpm_buf, pndef_mdlresfrq.DataLength );
		TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, (LONG)AdatV->frq_buf, pndef_mdlresfrq.DataLength );//<S09E>

		PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
		edata = (USHORT)( ( Prm->mdlsw ) | ( VIBMFC_PRM_ACTIVE ) );
		/* Pn140 */
//		TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)edata, pndef_ntchq1.DataLength );
		TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)edata, pndef_mdlsw.DataLength );//<S09E>

		Prm->loophz = AdatV->Kv_buf;
		TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );
		TuneAdatSetPosCtrlGain( GAIN_BAL3, AdatP );
	}
	return;
}


/****************************************************************************************************
		MFC(剛体+機台モデル)設定に戻す
****************************************************************************************************/
static void	Mfc2MassReset( ADATV *AdatV, ADATP *AdatP )
{
	/* MFC(剛体+機台モデル)調整済みであれば,正常終了												*/
	/* 剛体MFCで調整し、振動のため最小Kpmになってしまった場合、設定を２慣性に戻す					*/
	if( ( AdatV->f.Used2MassMfc == TRUE )
		&& ( AdatP->BaseCtrlData->MFControl.conf.MFCModelSetting <= ADATMFC_ONE_MASS) )
		{
			/* 正常終了に変更 */
			AdatV->f.MfcLastSet       = TRUE;
			AdatV->MfcTunePtr         = ADAT_MFC_CHK_REQ;	/* C割れ確認要求へ 						*/
			AdatV->f.VibDetectinCycle = TRUE;				/* １回は全運転パターンを動作させるため */
			AdatV->ErrState           = ADAT_CONTINUE;		/* 継続動作								*/
		}
		return;
}

/******************************************************************************************************
	アドバンストオートチューニング終了設定処理

	概要:	正常終了および異常終了時の処理を行う。
			正常終了時は、調整したパラメータをEEPROMに書き込む。
			異常終了時は、EEPROMには書き込まない。
			強制終了でopf.func_mode_clrを立てて、補助機能を終了する。
			変更したパラメータを元に戻すのは、func_clear()関数で行う。
******************************************************************************************************/
static PRM_RSLT	AdatFinishOpe( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT err_status )
{
	PJOGV *PJogV;
	PRM_RSLT rc;

	PJogV = &AdatP->BaseCtrlData->PJogHdl.PJogV;
	rc = PRM_RSLT_SUCCESS;
	AdatV->f.DrvErrDetect = FALSE;

	/* エラー時の共通処理																			*/
	if( err_status <= ADAT_NG )
	{	/* エラー発生 																				*/
		if( AdatV->f.RefInType == FALSE )
		{
			AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;		/* プログラムJOG運転停止				*/
			PJogV->Var.RoutCmd = PJOGCMD_ABORT;				/* プログラムJOG中止					*/
		}
		AdatV->ScanCRequest = ADAT_NO_OPE;					/* NOP処理要求セット					*/

		if( err_status > ADAT_COMERR )
		{	/* エラーコードが通信異常より大きい → サーボオフ										*/
			AdatP->FnCmnCtrl->FnSvonReq = FALSE;			/* 異常終了時はとりあえず、サーボオフ 	*/
			FunExe->State = ADAT_SEQ_WAIT;					/* 終了待ち状態へ					 	*/
		}
		else/* 減速停止後、エラー終了																*/
		{
			/* COINラッチ完了 or モータ停止検出														*/
			if( (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)
									|| (AdatP->SeqCtrlOut->MotStop == TRUE) )
			{/* COINラッチ完了 or モータ停止検出													*/
				FunExe->State = ADAT_SEQ_WAIT;					/* 終了待ち状態へ					*/
			}
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	エラー処理																					*/
	/*----------------------------------------------------------------------------------------------*/
	switch(err_status)
	{
		case ADAT_OK:											/* 正常終了							*/
			AdatReg->AdatState = ADAT_STS_DONE;					/* チューニング完了					*/
			if( AdatV->f.RefInType == FALSE )
			{
				AdatP->FnCmnCtrl->FnSvonReq = FALSE;			/* チューニング正常終了でサーボオフ	*/
			}

			switch ( FunExe->CmdCode )
			{
				case FCMD_EWRT:									/* EEPROM書込み 					*/
					AdatUpdateGainInEeprom( AdatReg, AdatP );
					FunExe->State = ADAT_SEQ_WAIT;				/* 現在のシーケンスポインタを保存 	*/
					break;

				case FCMD_NONE :
				case FCMD_UP :
				case FCMD_DOWN :
					break;

				default:
					rc = PRM_RSLT_CONDITION_ERR;
					break;
			}
			break;

		case ADAT_JST_ERR:										/* イナーシャ同定開始エラー			*/
		case ADAT_NOTRDYERR:									/* 運転準備未完エラー				*/
			FunExe->State = ADAT_SEQ_WAIT;						/* 現在のシーケンスポインタを保存	*/
			AdatReg->AdatState = ADAT_STS_NG;					/* 動作不良 						*/
			break;

		case ADAT_J_ERR:										/* イナーシャ同定エラー				*/
			FunExe->State = ADAT_SEQ_WAIT;						/* 現在のシーケンスポインタを保存	*/
			AdatReg->AdatState = ADAT_STS_JERR;					/* イナーシャ同定不良				*/
			break;

		case ADAT_COMERR:										/* 動作中の通信エラー				*/
			if( (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)
				|| (AdatP->SeqCtrlOut->MotStop == TRUE) )
			{
				FunExe->State = ADAT_SEQ_WAIT;					/* 現在のシーケンスポインタを保存	*/
			}
			AdatReg->AdatState = ADAT_STS_ABORT;				/* 動作中の異常による中断			*/
			break;

		default:												/* その他のエラーの場合				*/
			FunExe->State = ADAT_SEQ_WAIT;						/* 現在のシーケンスポインタを保存	*/
			AdatReg->AdatState = ADAT_STS_ERR;					/* チューニング失敗 				*/
			break;
	}
	return rc;
}

/****************************************************************************************************
	アドバンストオートチューニングスキャンCメイン

	概要:	スキャンCでのアドバンストオートチューニングのメイン処理
		 	処理要求にしたがって、以下のチューニング処理を行う。
****************************************************************************************************/
void	CpxAdvancedAutoTuning( ADATHNDL	*AdatHndl, FUN_CMN_CONTROL *FnCmnCtrl )
{
	SHORT rc;
	ULONG coinOffTimeCnt;
	LONG  wk1,wk2,wk3;
	BOOL  Prefon;

	ADATV	*AdatV;
	ADATP	*AdatP;

	rc = ADAT_SEAR_NORMALEND;
	AdatV = &AdatHndl->AdatV;
	AdatP = &AdatHndl->AdatP;

	if( AdatV->f.GainTuneRun == TRUE )
	{
		AdatScanCSetReq( AdatV );								/* Roundからの要求セット			*/
		/*------------------------------------------------------------------------------------------*/
		/*	振動検出フラグセット																	*/
		/*------------------------------------------------------------------------------------------*/
		AdatV->f.VibDetect = (AdatP->DetVib->var.VibDetFlg == TRUE) ? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	COIN割れ検出フラグセット																*/
		/*------------------------------------------------------------------------------------------*/
		/* 位置決め完了失敗累積時間取得 															*/
		coinOffTimeCnt = BpiGetCoinOffTime( AdatP->SettlingTime );
		/* 許容時間超過でCOIN割れ検出																*/
		AdatV->f.CoinBreak = (coinOffTimeCnt > COINOFFLMT) ? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	COINラッチフラグセット																	*/
		/*------------------------------------------------------------------------------------------*/
		AdatV->f.CoinLatch = ( AdatP->SettlingTime->StlgCoinLatch ) ? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	オーバーシュート過大フラグセット														*/
		/*------------------------------------------------------------------------------------------*/
		/*	許容オーバーシュート超過でオーバーシュートエラー検出									*/
		AdatV->f.OvsErr = ( AdatP->SettlingTime->UnMaxOverShoot > AdatP->SettlingTime->OvsErrDetLvl )
																						? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	停止時振動検出処理																		*/
		/*------------------------------------------------------------------------------------------*/
		/*	ハイパスフィルタ処理																	*/
		wk3 = MlibHpfilter1( (LONG)AdatP->SeqMotSts->var.TrqRefMon, AdatV->KTrqHpFil, &AdatV->KTrqHpftmp );
		if( AdatP->SettlingTime->StlgCoinLatch == FALSE )
		{
			KlibRstLongTimer( &AdatV->CoinOnCntr );
		}
		if( KlibGetLongTimerMs( AdatV->CoinOnCntr ) >= STPDETTM )
		{	/* COINラッチ完了から100[ms]経過 → 停止状態とみなす 									*/
			/* 最大最小振幅測定																		*/
			if( wk3 >= AdatV->MaxTrqRefatStop )
			{
				AdatV->MaxTrqRefatStop = wk3;
			}
			else if( wk3 <= AdatV->MinTrqRefatStop )
			{
				AdatV->MinTrqRefatStop = wk3;
			}
			/*--------------------------------------------------------------------------------------*/
			/* 停止時振動検出																		*/
			/*	通常トルクの STPVIBDET[%]以上かつ、定格STPVIBLMT[%]以上で検出						*/
			/*--------------------------------------------------------------------------------------*/
			wk1 = AdatV->TrqRefatStop * STPVIBDET / 100;			/* 停止時振動閾値トルク			*/
			wk2 = AdatV->MaxTrqRefatStop - AdatV->MinTrqRefatStop;	/* 停止時振動振幅				*/
#if (FLOAT_USE==TRUE)
			if( wk2 < ( STPVIBLMT * AdatP->UniPrms->Bprm->Knortrq ) )
			{
				wk2 = 0;											/* 閾値以下なら振動振幅クリア	*/
			}
#else
			if( wk2 < MlibMulgain( STPVIBLMT, AdatP->UniPrms->Bprm->Knortrq ) )
			{
				wk2 = 0;											/* 閾値以下なら振動振幅クリア	*/
			}
#endif /* FLOAT_USE */
			/* 閾値トルク超なら停止時振動検出 */
			AdatV->f.VibDetectatStop = (wk2 > wk1) ? TRUE : FALSE;
		}
		else
		{
			/* 停止時トルク振幅初期化	*/
			AdatV->MaxTrqRefatStop = wk3;							/* 初期値は現在のトルク指令		*/
			AdatV->MinTrqRefatStop = wk3;
			AdatV->f.VibDetectatStop = FALSE;						/* 停止時振動検出フラグ初期化	*/
		}

		/*------------------------------------------------------------------------------------------*/
		/*	アドバンストオートチューニング処理シーケンス											*/
		/*------------------------------------------------------------------------------------------*/
		switch ( AdatV->ScanCRequest )
		{
			case ADAT_NO_OPE:						/* no operation 								*/
				rc  = AdatV->ScanCStat;				/* 前回ステータスをセット						*/
				break;

			case ADAT_SEQ_NORMVIB:					/* 通常振動レベル測定							*/
				rc = LpxAdatVibLvlSet( AdatV, AdatP );
				rc = LpxAdatCheckKv( AdatV, AdatP );/* 振動レベル測定中も振動確認を行う				*/
				break;

			case ADAT_SEQ_SEARCHOPTKV:				/* 最適Kv、Tiゲイン探索							*/
				rc  = LpxAdatSearchKv( AdatV, AdatP );
				break;

			case ADAT_CHKKV:						/* 指令応答による安全確認						*/
				rc  = LpxAdatCheckKv( AdatV, AdatP );
				break;

			case ADAT_SEQ_KPSET:					/* Kp設定調整									*/
				rc = LpxAdatKpGainSet( AdatV, AdatP );
				break;

			case ADAT_SEQ_SEARCHOPTKP:				/* 最適Kpゲイン探索								*/
				rc = LpxAdatSearchKp( AdatV, AdatP );
				break;

			case ADAT_SEQ_ADJMFC:					/* モデル追従制御調整							*/
				rc = LpxAdatAdjustMfcGain( AdatV, AdatP );
				break;

			case ADAT_VIBCOIN:						/* モデル追従制御COIN割れ確認					*/
				rc = LpxAdatCheckMfcGain( AdatV, AdatP );
				break;

			default:								/* アドバンスドオートチューニング無効			*/
				rc = ADAT_SEAR_ERR;					/* 異常終了にする								*/
				break;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	エラー処理																				*/
		/*------------------------------------------------------------------------------------------*/
		if( AdatV->ScanCRequest != ADAT_NO_OPE )
		{	/* チューニング中																		*/
			if( ( AdatV->f.RefInType == FALSE ) && ( FnCmnCtrl->FnSvonReq == FALSE ) )
			{	/* サーボオフ 																		*/
				rc = ADAT_SEAR_ERR;					/* 探索処理中にサーボオフしたら、エラーとする	*/
			}
		}

		/*------------------------------------------------------------------------------------------*/
		/*	初期化処理																				*/
		/*------------------------------------------------------------------------------------------*/
		if((rc != ADAT_SEAR_CONTINUE) || (AdatV->ScanCRequest == ADAT_NO_OPE))
		{	/* 処理継続中以外 または、NOP															*/
			AdatV->ScanCRequest = ADAT_NO_OPE;		/* 処理終了後は待機状態になる					*/
													/* 処理実行継続するためには、roundから			*/
													/* 再度指令する									*/
			/* 差速度ピーク値クリア																	*/
			DetVibResetMaxMinSpeedError( AdatP->DetVib );

			/* 停止時トルク振幅初期化																*/
			AdatV->MaxTrqRefatStop = wk3;						/* 初期値は現在のトルク指令			*/
			AdatV->MinTrqRefatStop = wk3;

			BpiRsetCoinOffTime( AdatP->SettlingTime );			/* COIN割れ積算時間クリア			*/
			BpiRsetMaxOverShoot( AdatP->SettlingTime );			/* オーバーシュート量最大値クリア	*/
		}
		AdatV->ScanCStat = rc;											/* 処理ステータスセット		*/
		AdatV->f.LastCoinLatch = (AdatV->f.CoinLatch)? TRUE : FALSE;	/* COINラッチ信号保存 		*/
		/*------------------------------------------------------------------------------------------*/
		/*	運転ステータス作成処理																	*/
		/*		外部指令の場合，COINを見ない指令のような場合，										*/
		/*		Roundでは，COIN or ゼロ速を検出できない可能性があるためScanCで処理する				*/
		/*------------------------------------------------------------------------------------------*/
		if( AdatV->f.RefInType == TRUE )
		{
			Prefon = ( AdatP->SeqMotSts->var.dPcmdCnt != 0)? TRUE : FALSE;
			AdatV->PrgJogStat = LpxAdatRefInTypeDrvStatus( AdatHndl, AdatV->PrgJogPatternReq, Prefon );
		}
	}
}

/****************************************************************************
	アドバンストオートチューニング振幅レベル測定

	概要:	通常運転中の振動レベルを測定する。
			通常ゲインでの送り中の発振検出用差速度最大値を測定する。
****************************************************************************/
static SHORT	LpxAdatVibLvlSet( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	LONG	lwork;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* 振動レベル測定															*/
	/*--------------------------------------------------------------------------*/
	lwork = AdatP->DetVib->DetVibObs.var.MaxSpdErr - AdatP->DetVib->DetVibObs.var.MinSpdErr;
	if( lwork >= AdatV->VibSpeed )
	{
		AdatV->VibSpeed = lwork;					/* 最大振幅測定				*/
	}

	/*--------------------------------------------------------------------------*/
	/*	停止時通常トルク振動測定												*/
	/*--------------------------------------------------------------------------*/
	if( KlibGetLongTimerMs( AdatV->CoinOnCntr ) >= STPDETTM )
	{	/* COINラッチ完了から100[ms]経過 → 停止状態とみなす	*/
		lwork = AdatV->MaxTrqRefatStop - AdatV->MinTrqRefatStop;
		if( AdatV->TrqRefatStop < lwork )
		{
			AdatV->TrqRefatStop = lwork;			/* 最大振幅					*/
		}
	}
	return rc;
}


/****************************************************************************
	アドバンストオートチューニング 指令応答による安全確認

	概要:	指令応答による振動確認を行う。
			位置指令中に振動検出を行い、振動したら、ゲインを6.25%下げる。
			Tiは、Kvと連動させる。
****************************************************************************/
static SHORT	LpxAdatCheckKv( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* 停止時振動検出															*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.VibDetectatStop )
	{	/* 発振検出した	*/
		rc = ADAT_SEAR_STOPVIBEND;					/* 停止時振動検出終了		*/
	}
	/*--------------------------------------------------------------------------*/
	/* 発振検出																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* 振動検出終了				*/
	}
	return rc;
}


/****************************************************************************
	アドバンストオートチューニング最適ゲイン探索

	概要:	指令応答によるKvの最適値を求める。
			位置指令中に振動検出を行い、振動していなければ、
			Kvを2.0[Hz]ずつ上げる。
			Tiは、kvと連動させる。
****************************************************************************/
static SHORT	LpxAdatSearchKv( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc  = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* ゲイン探索最大値に到達した場合											*/
	/*--------------------------------------------------------------------------*/
	if( AdatP->UniPrms->Prm->loophz >= AdatV->MaxKv )
	{
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* ゲイン探索カウンタクリア	*/
		rc = ADAT_SEAR_NORMALEND;					/* 正常終了					*/
	}
	/*--------------------------------------------------------------------------*/
	/* 停止時振動検出															*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetectatStop )
	{	/* 発振検出した	*/
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* ゲイン探索カウンタクリア	*/
		rc = ADAT_SEAR_STOPVIBEND;					/* 停止時振動検出終了		*/
	}
	/*--------------------------------------------------------------------------*/
	/* 発振検出																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* ゲイン探索カウンタクリア	*/
		rc = ADAT_SEAR_VIBEND;						/* 振動検出終了				*/
	}
	else if( KlibGetLongTimerMs( AdatV->GainUpCntr ) > GNUPTM2 )
	{	/* 所定時間振動検出しなかった → ゲインUP	*/
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* ゲイン探索カウンタクリア	*/
		rc = ADAT_SEAR_NOVIBDETECT;					/* 振動未検出 ゲインＵＰ	*/
	}
	return rc;
}



/****************************************************************************
	アドバンストオートチューニング最大Kp探索

	概要:	Kpを一定時間毎にMaxKpまで上げる。
			振動検出を行い、振動していればステータスを返す。
			COIN信号を検出したら正常終了となる。
****************************************************************************/
static SHORT	LpxAdatKpGainSet( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* 発振検出																	*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.VibDetect )
	{
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* 振動検出終了				*/
	}
	/*--------------------------------------------------------------------------*/
	/* 振動無し																	*/
	/*--------------------------------------------------------------------------*/
	else if( KlibGetLongTimerMs( AdatV->KpGainUpCntr ) > GNUPTM2 )
	{	/* 所定時間振動検出しなかった → ゲインUP	*/
		KlibRstLongTimer( &AdatV->KpGainUpCntr );	/* ゲイン探索カウンタクリア	*/
		rc = ADAT_SEAR_NOVIBDETECT;					/* 振動未検出 ゲインUP		*/
	}
	/*--------------------------------------------------------------------------*/
	/* ゲイン探索最大値に到達した場合											*/
	/*--------------------------------------------------------------------------*/
	if( (AdatP->UniPrms->Prm->posgn >= AdatV->MaxKp)/* 探索Kp最大値以上か？	*/
			|| (AdatV->f.CoinLatch == TRUE) )		 /* COIN信号を検出したか？	*/
	{
		KlibRstLongTimer( &AdatV->KpGainUpCntr );	/* ゲイン探索カウンタクリア	*/
		rc = ADAT_SEAR_NORMALEND;					/* 正常終了					*/
	}
	return rc;
}

/****************************************************************************
	アドバンストオートチューニング最適Kpゲイン探索

	概要:	指令応答によるKpの最適値を求める。
			位置決め時のCOIN割れ検出を行い、COIN割れ許容時間を超えていれば
			位置決め時のCOIN割れ検出を行い、割れ許容時間を超えていれば、
			Kpを3.0[1/s]ずつ下げる。
			Kpを下げすぎてKvとのバランスが崩れた場合は、Kvを下げ、再トライ。
			このときKp、Tiは、Kvと連動させる。
****************************************************************************/
static SHORT	LpxAdatSearchKp( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* オーバーシュートエラー検出												*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.OvsErr )
	{	/* オーバーシュート許容値超過	*/
		BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* オーバーシュート量クリア	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN割れ検出終了			*/
	}
	/*--------------------------------------------------------------------------*/
	/* COIN割れ検出																*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.CoinBreak )
	{	/* COIN割れ許容値超過	*/
		BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN割れ積算時間クリア	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN割れ検出終了			*/
	}
	/*--------------------------------------------------------------------------*/
	/* 発振検出																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{	/* 発振検出した	*/
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* 振動検出終了				*/
	}
	return rc;
}

/****************************************************************************
	アドバンストオートチューニングモデル追従制御調整

	概要:	指令応答によるKpmの最大値を求める。
			位置決め時のCOIN割れ検出を行い、「COIN割れなし」なら、
			Kpmを3.125％ずつ上げる。
****************************************************************************/
static SHORT	LpxAdatAdjustMfcGain( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* オーバーシュートエラー検出												*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.OvsErr )
	{	/* オーバーシュート許容値超過	*/
		BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* オーバーシュート量クリア	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN割れ検出終了			*/
	}
	/*--------------------------------------------------------------------------*/
	/* COIN割れ検出																*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.CoinBreak )
	{	/* COIN割れ許容値超過	*/
		BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN割れ積算時間クリア	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN割れ検出終了			*/
	}
	/*--------------------------------------------------------------------------*/
	/* 発振検出																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{	/* 発振検出した	*/
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* 振動検出終了				*/
	}
	/*--------------------------------------------------------------------------*/
	/* Kpm UP																	*/
	/*--------------------------------------------------------------------------*/
	else if( (AdatV->f.CoinLatch == FALSE) && (AdatV->f.LastCoinLatch == TRUE) )
	{/* COIN信号OFFエッジ検出 */
		rc = ADAT_SEAR_COINOK;						/* 振動およびCOIN割れなし	*/
	}
	return rc;
}

/****************************************************************************
	アドバンストオートチューニングモデル追従制御COIN割れ確認

	概要:	指令応答によるKpmの最適値を求める。
			位置決め時のCOIN割れ検出を行い、「COIN割れ」なら、
			Kpmを3.0[1/s]ずつ下げる。
****************************************************************************/
static SHORT	LpxAdatCheckMfcGain( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* オーバーシュートエラー検出												*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.OvsErr )
	{	/* オーバーシュート許容値超過	*/
		BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* オーバーシュート量クリア	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN割れ検出終了			*/
	}
	/*--------------------------------------------------------------------------*/
	/* COIN割れ検出																*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.CoinBreak )
	{	/* COIN割れ許容値超過 */
		BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN割れ積算時間クリア	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN割れ検出終了			*/
	}
	/*--------------------------------------------------------------------------*/
	/* 発振検出																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{	/* 発振検出した	*/
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* 振動検出終了				*/
	}
	return rc;
}

/*****************************************************************************************
	アドバンストオートチューニング動作ステータス作成処理

	概要:	運転指令を外部指令器を使用してプログラムJOGを利用しない
			場合の運転ステータスを作成する。
        	内部／外部指令は本関数で吸収され、他のモジュールには影響しない

			運転指令の制限
			・COIN信号を確認後，次回指令を開始すること。COIN信号を確認しないで次回指令を
			  払い出すと，チューニングシーケンスが移らない
			  COIN信号ON後の停止時間が100ms以下の場合は，停止時振動検出が行えない
			・運転速度，加速度には制限を設けない。
			  ただし、イナーシャ同定，ゲインサーチ結果は，速度，加速度によって結果が
			  影響される。

			運転停止    ・・・フィルタ前位置指令セグメント=0がCOINラッチ後300[ms]
			位置決め完了・・・フィルタ前位置指令セグメント=0かつCOINラッチ
			正／逆方向  ・・・停止毎に正転／逆転を切り替え
*****************************************************************************************/
static SHORT LpxAdatRefInTypeDrvStatus( ADATHNDL *AdatHndl, ULONG DrvPattern, BOOL PrefInFlag )
{
	SHORT	rc;
	ADATREG *AdatReg;
	ADATV	*AdatV;
	ADATP	*AdatP;

	AdatReg = &AdatHndl->AdatReg;
	AdatV = &AdatHndl->AdatV;
	AdatP = &AdatHndl->AdatP;
	rc = AdatV->PrgJogStat;					/* リターンコードを前回ステータスに初期化	*/

	switch ( AdatV->PrgJogPtr )
	{
		/*------------------------------------------------------------------------------*/
		/*	Initial process of Jog sequence 											*/
		/*------------------------------------------------------------------------------*/
		case 0:
			if( DrvPattern == ADAT_PJOG_STOP )
			{	/* 運転停止要求あり														*/
				rc = ADAT_PJOG_NOP;						/* 運転停止状態へ				*/
			}
			else
			{
				AdatV->f.DrvPatternSet = TRUE;
				AdatV->PositioningTimes = 0;			/* 位置決め回数カウンタクリア	*/
				AdatV->PrgJogPtr = 1;					/* シーケンスポインタ更新		*/
				rc = ADAT_PJOG_WAIT;					/* 運転待機						*/
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	運転開始処理					 											*/
		/*------------------------------------------------------------------------------*/
		case 1:
			if( (PrefInFlag == TRUE) || (AdatP->SeqCtrlOut->TgOnSts == TRUE) )
			{	/* 移動指令あり															*/
				AdatV->PrgJogPtr = 2;					/* 移動完了待ちへ				*/
				rc = ADAT_PJOG_EXEC;
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	移動指令完了待ち				 											*/
		/*------------------------------------------------------------------------------*/
		case 2:
			if(( (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)
			  || (AdatP->SeqCtrlOut->MotStop) == TRUE) 	/* COINラッチ完了orモータ停止検出 */
			  || ((PrefInFlag == FALSE)
			  && (AdatV->f.StopDetectMode2 == TRUE) ) )	/* 停止判定モード2かつ指令停止	*/
			{	/* 指令停止判定															*/
				AdatV->PrgJogPtr = 3;					/* 運転完了待ちへ				*/
			}
		/* Fall Through */	/* 「運転完了待ち」と同じタイミングでCOIN判別したいためbreakなし*/
		/*------------------------------------------------------------------------------*/
		/*	運転完了待ち					 											*/
		/*------------------------------------------------------------------------------*/
		case 3:
			if(( AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE )/* COINラッチ完了 */
			 || ((PrefInFlag == FALSE)
			 && (AdatV->f.StopDetectMode2 == TRUE)))	/* 停止判定モード2かつ指令停止	*/
			{
				/* 運転方向別位置決め完了ステータスセット								*/
				/* 位置決め完了毎に正転／逆転をセットする								*/
				AdatV->PositioningTimes++;				/* 位置決め回数カウンタクリア	*/
				if( AdatV->PositioningTimes & 0x01 )
				{
					rc = ADAT_PJOG_PLUSEND;
				}
				else
				{
					rc = ADAT_PJOG_MINUSEND;
				}
				AdatV->CoinWaitStat = ADAT_STS_COINNOWAIT;
				AdatV->f.CoinWaitStart = FALSE;				/* COIN出力待ち処理終了		*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				AdatV->PrgJogPtr = 4;						/* ウェイト処理へ			*/
				KlibRstLongTimer( &AdatV->CoinWaitCntr );
			}
			else
			{
				/*----------------------------------------------------------------------*/
				/*	COIN出力待ち処理(位置制御のみ)										*/
				/*----------------------------------------------------------------------*/
				if( AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
				{	/* 位置制御モード 													*/
					if( (PrefInFlag == FALSE) &&			/* 指令なし					*/
						 (AdatV->f.CoinWaitStart == FALSE))	/* COIN出力待ち処理中でない	*/
					{
						KlibRstLongTimer( &AdatV->CoinWaitCntr );
						AdatV->CoinWaitStat = ADAT_STS_COINWAIT;
						AdatV->f.CoinWaitStart = TRUE;			/* COIN出力待ち処理開始	*/
					}
					if(AdatV->f.CoinWaitStart == TRUE)			/* COIN出力待ち処理中	*/
					{
						if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT1 )
						{
							/* 約10秒位置決め完了しないときはCOIN待ちエラー				*/
							AdatV->ErrState = ADAT_COINERR;
							AdatV->CoinWaitStat = ADAT_STS_COINERR;	/* エラー出力ON		*/
						}
						else if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT2 )
						{	/* COIN 出力待ち警告中	*/
							AdatV->CoinWaitStat = ADAT_STS_COINWARN;/* 警告出力			*/
						}
					}
				}
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	ウェイト						 											*/
		/*------------------------------------------------------------------------------*/
		case 4:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr ) >= ADATDRVWAIT )
			{	/* 運転完了																*/
				AdatV->PrgJogPtr = 5;						/* 次回運転準備へ			*/
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	次回運転準備					 											*/
		/*------------------------------------------------------------------------------*/
		case 5:
			if( ( AdatV->f.GainTuneRun == TRUE )			/* チューニング中			*/
				&& (AdatReg->AdatState != ADAT_STS_VIBDET) )/* 振動レベル測定ではない	*/
			{	/* チューニング中は，一旦，クリアする									*/
				DetVibResetMaxMinSpeedError( AdatP->DetVib );
			}
			if( (AdatV->PositioningTimes & 0x0004) != 0x0000 )
			{	/* 2往復完了。運転完了は，振動確認を確実に行うため，2往復で行う			*/
				AdatV->PositioningTimes = 0;				/* 位置決め回数カウンタクリア*/
				AdatV->PrgJogPtr = 6;						/* 運転停止ウェイト処理へ	*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );	/* ラウンドカウンタ保持		*/
				rc = ADAT_PJOG_FIN;							/* 運転完了ステータスセット	*/
			}
			else
			{
				AdatV->PrgJogPtr = 1;						/* 運転開始処理へ			*/
				rc = ADAT_PJOG_WAIT;						/* 運転待機					*/
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	運転停止						 											*/
		/*------------------------------------------------------------------------------*/
		case 6:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr ) >= ADATDRVWAIT )
			{	/* 運転完了																*/
				AdatV->PrgJogPtr = 0;		/* 運転停止。シーケンスを初期設定にセット	*/
			}
			break;

		default:
			AdatV->ErrState = ADAT_PROGERR;	/* 異常終了									*/
			break;
	}
	return rc;
}


