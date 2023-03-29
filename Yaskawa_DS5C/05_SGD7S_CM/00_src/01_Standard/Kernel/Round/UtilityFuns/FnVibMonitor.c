/****************************************************************************
  Description   : Online Vibration Monitor (Fn207)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.28   created

****************************************************************************/
#include "FnVibMonitor.h"
#include "BaseControls.h"
#include "TuneLib.h"
#include "KLib.h"
#include "PnPrmListTbl.h"
#include "MercuryGlobals.h"
#include "ExControls.h"


/****************************************************************************************************/
/*																									*/
/*		Fn207 : Online Vibration Monitor															*/
/*																									*/
/****************************************************************************************************/
/* -------- Fn207 : Vibration Freq. Display and Notch Filter Autoset Begin	-------*/
PRM_RSLT	FnEnterVibrationMonitor( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT	rc;
	VIBMON		*VibMon;

	rc = PRM_RSLT_SUCCESS;
	VibMon = Axis->VibMon;

	/* モータレステストモードの場合は入らずに終了する *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	VibMon->var.VibDetLvl = (SHORT)( DetVibGetVibCheckLevel(Axis->DetVib) >> 12 );
	VibMon->var.FftReq = FALSE;
	VibMon->OpeReg.VibmState = VMON_STS_END;

	/* Fnモード自動終了なし */
	FunExe->HoldFnMode = TRUE;

	return rc ;
}


/* -------- Fn207 : Vibration Freq. Display and Notch Filter Autoset End	-------*/
void	FnLeaveVibrationMonitor( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	/* Fnモード自動終了設定 */
	FunExe->HoldFnMode = FALSE;

	return;
}


/* -------- Fn207 : Vibration Freq. Display and Notch Filter Autoset Execution	-------*/
PRM_RSLT	FnExecuteVibrationMonitor( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	USHORT			VibFreq;
	PRM_RSLT		rc;
	LONG			lwk0;
	ULONG			data;
	REG_ACC_T		AccessPrm;
	VIBMON			*VibMon;
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;
	FUN_CMN_CONTROL		*FnCmnCtrl;		/* <S085> */

	FnCmnCtrl = Axis->FnCmnCtrl;		/* <S085> */
	VibMon = Axis->VibMon;
	Prm = Axis->UniPrms->Prm;
	RegCfg = Axis->RegManager->hPnReg;
	rc = PRM_RSLT_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* Call Servo Control Trace Module for Auto Notch */
	VibMon->var.FftStat = TuneVibrationTrace( Axis->DetVib,
											  Axis->FftAna,
											  Axis->VibMon->var.FftReq,
											  Axis->VibMon->var.VibDetLvl,
											  TuneLessGetTuneLessSts(&Axis->BaseControls->TuneLessCtrl) );

	switch( FunExe->State )
	{
	case VIBMON_SEQ_INIT:	/* 検出開始待ち処理	*/
	case VIBMON_SEQ_DETERR:
		switch( FunExe->CmdCode )
		{
		case FCMD_EXEC:		/* 検出開始		*/
			VibMon->var.FftReq = TRUE;
			VibMon->OpeReg.VibmState = VMON_STS_RUN;
			KlibRstLongTimer( &(VibMon->var.TimeOutCtr) );
			FunExe->State = VIBMON_SEQ_DETECT;
			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_STOP:
			VibMon->OpeReg.VibmState = VMON_STS_STOP;
			VibMon->var.FftReq = FALSE;
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case VIBMON_SEQ_DETECT:
		if( KlibGetLongTimerMs( VibMon->var.TimeOutCtr ) > 8000 )
		{ /* 8[s]経過 */
			VibMon->OpeReg.VibmState = VMON_STS_ERR;
			FunExe->State = VIBMON_SEQ_DETERR;
			VibMon->var.FftReq = FALSE;
			break;
		}

		if( VibMon->var.FftStat == TUNE_FREQ_SEARCHEND )
		{
			FunExe->State = VIBMON_SEQ_DISPLAY;
			VibMon->OpeReg.VibmState = VMON_STS_END;
			VibMon->var.FftReq = FALSE;
		}
		else if( VibMon->var.FftStat == TUNE_FREQ_SEARCHERR )
		{
			VibMon->OpeReg.VibmState = VMON_STS_ERR;
			FunExe->State = VIBMON_SEQ_DETERR;
			VibMon->var.FftReq = FALSE;
			break;
		}
		else
		{
			rc = PRM_RSLT_CONTINUE;
		}

		switch( FunExe->CmdCode )
		{
		case FCMD_STOP:		/* 検出中止		*/
			VibMon->OpeReg.VibmState = VMON_STS_STOP;
			FunExe->State = VIBMON_SEQ_INIT;
			VibMon->var.FftReq = FALSE;
			rc = PRM_RSLT_SUCCESS;
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case VIBMON_SEQ_DISPLAY:
		switch( FunExe->CmdCode )
		{
		case FCMD_EWRT:
			/* Get Vibration Frequency */
			VibFreq = FftGetAnaRsltVibmRf( Axis->FftAna );

			if( (VibMon->OpeReg.VibmState == VMON_STS_END) && (VibFreq != 0) )
			{
				FunExe->State = VIBMON_SEQ_CALC;
			}
			else
			{
				rc = PRM_RSLT_CMD_ERR;
			}
			break;

		case FCMD_EXEC:
			VibMon->var.FftReq = TRUE;
			VibMon->OpeReg.VibmState = VMON_STS_RUN;
			KlibRstLongTimer( &(VibMon->var.TimeOutCtr) );
			FunExe->State = VIBMON_SEQ_DETECT;
			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_NONE:
			break;

		case FCMD_STOP:
			VibMon->OpeReg.VibmState = VMON_STS_STOP;
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case VIBMON_SEQ_CALC:
		/* Get Vibration Frequency */
		VibFreq = FftGetAnaRsltVibmRf( Axis->FftAna );

		if( VibFreq > 500 )
		{
//			lrc = RpiPrmWriteRamForFn( &pndef_ntchfil1, (ULONG)AtxReg.VibmRf[0].Freq, pfexe->App.Hmsg );
#if 0
			TunePrmWriteRam( &AccessPrm, &pndef_ntchfil1, (ULONG)VibFreq, 2 );
#else		/* <S085> */
			rc = TunePrmWriteRam( &AccessPrm, &pndef_ntchfil1, (ULONG)VibFreq, 2 );
			if ( rc != PRM_RSLT_SUCCESS )
			{
				FnCmnCtrl->OperatorSts = -1;
			}
#endif
			/* EEPROM更新	*/
//			lrc = RpiFunPrmNvmemUpdate( &pndef_ntchfil1 );
			PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );

//			RpiPrmRead(&pndef_tfuncsw, &data );
			data = Prm->tfuncsw;
			data |= 0x00000001;
//			lrc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)data, pfexe->App.Hmsg );
#if 0
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (ULONG)data, 2 );
#else		/* <S085> */
			rc = TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (ULONG)data, 2 );
			if ( rc != PRM_RSLT_SUCCESS )
			{
				FnCmnCtrl->OperatorSts = -1;
			}
#endif
			/* EEPROM更新	*/
//			lrc = RpiFunPrmNvmemUpdate( &pndef_tfuncsw );
			PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
		}
		else
		{
			lwk0 = 50000 / VibFreq;
			if ( lwk0 > 63662 / Prm->loophz )
			{
				lwk0 = 63662 / Prm->loophz;
			}
//			lrc = RpiPrmWriteRamForFn( &pndef_trqfil11, (ULONG)lwk0, pfexe->App.Hmsg );
#if 0
			TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (ULONG)lwk0, 2 );
#else		/* <S085> */
			rc = TunePrmWriteRam( &AccessPrm, &pndef_trqfil11, (ULONG)lwk0, 2 );
			if ( rc != PRM_RSLT_SUCCESS )
			{
				FnCmnCtrl->OperatorSts = -1;
			}
#endif
			/* EEPROM更新	*/
//			lrc = RpiFunPrmNvmemUpdate( &pndef_trqfil11 );
#if 0
			PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
#else		/* <S085> */
			rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
			if ( rc != PRM_RSLT_SUCCESS )
			{
				FnCmnCtrl->OperatorSts = -1;
			}
			else
			{
				FnCmnCtrl->OperatorSts = 1;
			}
#endif
		}
		KlibRstLongTimer( &(VibMon->var.TimeOutCtr) );
		FunExe->State = VIBMON_SEQ_STORE;
		break;

	case VIBMON_SEQ_STORE:
		if( KlibGetLongTimerMs( VibMon->var.TimeOutCtr ) >= 1000 )
		{ /* 1[s]経過 */
			FunExe->State = VIBMON_SEQ_DISPLAY;
		}
		break;

	case VIBMON_SEQ_END:	/* 終了処理		*/
		FnLeaveVibrationMonitor( FunExe, Axis );
		break;

	default:
		break;
	}
	return rc;
}


/****************************************************************************************************/
