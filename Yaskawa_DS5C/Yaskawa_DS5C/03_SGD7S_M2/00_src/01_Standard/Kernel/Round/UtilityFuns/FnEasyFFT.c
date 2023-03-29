/****************************************************************************
  Description   : Easy FFT Measurement (Fn206)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.17   created
		EasyFFTでは、トレースとテーブル運転を内部的に使用する。このため、
		本モジュールでは、トレースとテーブル運転のレジスタのみ直接参照、
		書き換えを行うこととする。
****************************************************************************/
#include "FnEasyFFT.h"
#include "FnTableCommand.h"
#include "MercuryGlobals.h"
#include "BaseControls.h"
#include "DataTraceManager.h"
#include "TableDrive.h"
#include "AlarmManager.h"
#include "TuneLib.h"
#include "PnPrmListTbl.h"
#include "MLib.h"
#include "ExControls.h"


/****************************************************************************************************/
/* Local Variable Definition																		*/
/****************************************************************************************************/
static	TBLDRIVE	OrgTblDrive;



/****************************************************************************************************/
/* Easy FFT Measurement (Fn206)																		*/
/****************************************************************************************************/
/* -------- Local Function Prototype Definition -------- */
static PRM_RSLT	easyFftSetTuningGain( EASYFFT *EasyFFT, REG_ACC_T *AccessPrm );
static PRM_RSLT	easyFftUpdateGainInEeprom( REGIF_CONFIG_T *RegCfg );
static void		easyFftUndoRamGain( REGIF_CONFIG_T *RegCfg );
static void		easyFftSetPrmBackup( EASYFFT *EasyFFT );
static void		easyFftUndoPrmBackup( EASYFFT *EasyFFT );
static void		easyFftSetTraceRegister( TRCREG *TrcReg, LONG ax, USHORT ezfftsw );
static void		easyFftSetTblCmdRegister( TBLDRIVE *TblDrive );
static void		easyFftClearCounter( EASYFFT *EasyFFT );
static void		easyFftSetRefCommand( EASYFFT *EasyFFT, LONG SvCycleUs, USHORT PerMaxTrq );
static void		easyFftTuneRegClear( EASYFFT *EasyFFT );

/* -------- Local Function (for Sequence) Prototype Definition -------- */
static PRM_RSLT	easyFftSequence( EASYFFT *EasyFFT, TBLDRIVE *TblDrive, TRCREG *TrcReg, BPRMDAT *Bprm );
static void		easyFftMakeChirpRefTrace( EASYFFT *EasyFFT );
static ULONG	easyFftGetChirpFreq( EASYFFT *EasyFFT, USHORT count, LONG ObjFlag );
static void		easyFftGetFreqGainMax( EASYFFT *EasyFFT, TRCREG *TrcReg );
static void		easyFftCalcHpFilter( EASYFFT *EasyFFT, TRCREG *TrcReg );
static USHORT	easyFftGetAvrgFreq( ULONG frgavr, ULONG freq );
static void		easyFftSetZoomAnalysis( EASYFFT *EasyFFT );



/****************************************************************************************************/
/*																									*/
/*		Easy FFT Measurement (Fn206)																*/
/*																									*/
/****************************************************************************************************/
/* -------- Fn206 : Easy FFT Begin	-------*/
PRM_RSLT	FnEnterEasyFft( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT			rc;
	EASYFFT				*EasyFFT;
	PRMDATA				*Prm;
	FUN_CMN_CONTROL		*FnCmnCtrl;

	EasyFFT = Axis->EasyFFT;
	Prm = Axis->UniPrms->Prm;
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

	/* Initialize Easy FFT */
	EasyFFT->OpeReg.EzFftTrq = Prm->ezffttrq;

	/* サーボオン中の場合は入らずに終了 */
	if( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* モータレステストモードの場合は入らずに終了 *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	/* Save Original Table Drive Data */
	MlibCopyLongMemory( &OrgTblDrive,
						&(Axis->BaseControls->TblDrive),
						sizeof(Axis->BaseControls->TblDrive)/4 );

	/* Reset Easy FFT Variables */
	MlibResetLongMemory( &(EasyFFT->var), sizeof(EasyFFT->var)/4 );

	/* 調整レス強制無効要求 */
	TuneLessSetForceInvldReq( &(Axis->BaseControls->TuneLessCtrl) );

	/* OT信号マスク設定 */
	SeqIoSetOtSigMask( Axis->IoSignals );

//	RpiPrmRead( &pndef_tfuncsw, &data );
	EasyFFT->var.NotchSw = Prm->tfuncsw;
//	RpiPrmRead( &pndef_ntchfil1, &data );
	EasyFFT->OpeReg.EzFftFil1 = Prm->ntchfil1;
//	RpiPrmRead( &pndef_ntchfil2, &data );
	EasyFFT->OpeReg.EzFftFil2 = Prm->ntchfil2;
//	RpiPrmRead( &pndef_swpminfrq, &data );
	EasyFFT->var.FrqSwpMin = Prm->swpminfrq;
//	RpiPrmRead( &pndef_swpmaxfrq, &data );
	EasyFFT->var.FrqSwpMax = Prm->swpmaxfrq;

//	RpiPrmRead( &pndef_detfrqlmt, &data );
	EasyFFT->var.FrqLowerLmt = Prm->detfrqlmt;

	/* 設定情報バックアップ */
	easyFftSetPrmBackup( EasyFFT );

	/* レジスタ設定(EasyFFT開始) */
	EasyFFT->OpeReg.EzFftState = EZFFT_STS_END;

	/* Fnモード自動終了なし */
	FunExe->HoldFnMode = TRUE;

	return rc;
}


/* -------- Fn206 : Easy FFT End	-------*/
void	FnLeaveEasyFft( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	REGIF_CONFIG_T		*RegCfg;
	FUN_CMN_CONTROL		*FnCmnCtrl;

	RegCfg = Axis->RegManager->hPnReg;
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* EEPROMデータで変更パラメータを元に戻す */
	easyFftUndoRamGain( RegCfg );

	/* OT信号マスク解除 */
	SeqIoClearOtSigMask( Axis->IoSignals,
	                       Axis->UniPrms->Prm->pot_selsw,
	                       Axis->UniPrms->Prm->not_selsw );

	/* Restore Original Table Drive Data */
	MlibCopyLongMemory( &(Axis->BaseControls->TblDrive),
						&OrgTblDrive,
						sizeof(Axis->BaseControls->TblDrive)/4 );

	/* サーボオフするまで待機 */
	FnCmnCtrl->FnSvonReq = FALSE;
	while( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		;
	}

	/* 調整レス強制無効要求解除 */
	TuneLessClearForceInvldReq( &Axis->BaseControls->TuneLessCtrl );

	/* テーブル運転ストップ */
	TblDriveSetStopRequest( &(Axis->BaseControls->TblDrive) );

	/* テーブル運転同時トレース実行要求解除 */
	/* 2011.02.28 Y.Oka テーブル運転中トレースを強制連動とする */
//	TblDriveSetWithTrcReq( &(Axis->BaseControls->TblDrive), FALSE );

	/* Fnサーボ運転権開放 */
	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->FnSvControl = FALSE;

	return;
}


/* -------- Fn206 : Easy FFT Execution	-------*/
PRM_RSLT	FnExecuteEasyFft( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT			rc;
	REG_ACC_T			AccessPrm;
	EASYFFT				*EasyFFT;
	TBLDRIVE			*TblDrive;
	TRCREG				*TrcReg;
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	REGIF_CONFIG_T		*RegCfg;
	FUN_CMN_CONTROL		*FnCmnCtrl;
	ULONG	i;					/* <S0C9> */
	TRCINDIVPRM	*TrcIncivPrm;	/* <S0C9> */

	EasyFFT = Axis->EasyFFT;
	TblDrive = &(Axis->BaseControls->TblDrive);
	TrcReg = &(Axis->TrcHndl->TrcReg);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	RegCfg = Axis->RegManager->hPnReg;
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;
	TrcIncivPrm =  &(Axis->TrcHndl->TrcIncivPrm);	/* <S0C9> */


	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	rc = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case EZFFT_SEQ_INIT:
		/* データトレース & テーブル運転実行用レジスタ計算処理 */
		easyFftSetTraceRegister( TrcReg, Axis->AxisNo, Prm->ezfftsw );

/* <S0C9>	Start */
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		TrcIncivPrm->TrcData[i] = TrcReg->TrcDataSel[i];		/* データトレース対象データ選択(1～16)			*/
		TrcIncivPrm->TrcAxis[i] = TrcReg->TrcAxisSel[i];		/* データトレース対象軸選択(1～16)				*/
		TrcIncivPrm->IOTrcData[i] = TrcReg->IOTrcDataSel[i];	/* I/Oトレース1対象データ選択(1～16)			*/
		TrcIncivPrm->IOTrcAxis[i] = TrcReg->IOTrcAxisSel[i];	/* I/Oトレース1対象軸選択(1～16)				*/
	}
/* <S0C9>	End */

		easyFftSetTblCmdRegister( TblDrive );
		DtrcCalcParameter( Axis->TrcHndl, Axis->TraceBuffer );
		TblDriveGetTraceRegData( TblDrive,
								 TrcReg->DataSize,
								 TrcReg->SampIntv,
								 DtrcGetSampIntv(Axis->TrcHndl),
								 DtrcGetTrcDataNum(Axis->TrcHndl) );
		TblDriveCalculateParameter( TblDrive, Bprm->XfbPulse1 );

		/* テーブル運転同時トレース実行要求 */
		/* 2011.02.28 Y.Oka テーブル運転中トレースを強制連動とする */
//		TblDriveSetWithTrcReq( TblDrive, TRUE );

		DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );
		FunExe->State = EZFFT_SEQ_READY;
		
	
		/* テーブル運転スタート */
		rc = TblDrivePrepareProc( TblDrive );
		if( rc == PRM_RSLT_SUCCESS )
		{
			/* 制御モード設定 */
			FnCmnCtrl->FnCtrlMcmd = TblDriveGetControlMode( TblDrive );
		}
		else
		{
			/* 終了処理へ */
			FunExe->State = EZFFT_SEQ_END;
		}
		break;

	case EZFFT_SEQ_READY:
		switch( FunExe->CmdCode )
		{
		case FCMD_SV:
			if( ((Axis->AlmManager)->Status.AlmFlag != FALSE)		/* <S0E5> */ /* <S0FF> */
				|| (Axis->SeqCtrlOut->HwbbSts == TRUE)
				|| (Axis->SeqCtrlOut->MainPowerOn == FALSE) )
			{ /* アラーム発生中 || HWBB || 主回路OFF */
				rc = PRM_RSLT_CONDITION_ERR;
			}
			else
			{
				FnCmnCtrl->FnSvonReq = (~FnCmnCtrl->FnSvonReq) & 0x01;
			}
			break;

		case FCMD_EWRT:
			easyFftSetTuningGain( EasyFFT, &AccessPrm );
			easyFftUpdateGainInEeprom( RegCfg );
			break;

		case FCMD_UP:
		case FCMD_DOWN:
		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		/* サーボオンで処理ステートの移行処理 */
		if( Axis->SeqCtrlOut->BaseEnableReq == TRUE )
		{
			FunExe->State = EZFFT_SEQ_DRIVE;
		}
		break;

	case EZFFT_SEQ_DRIVE:
		switch( FunExe->CmdCode )
		{
		case FCMD_SV:
			FnCmnCtrl->FnSvonReq = FALSE;
			break;

		case FCMD_UP:
			easyFftTuneRegClear( EasyFFT );
			easyFftSetRefCommand( EasyFFT, Bprm->SvCycleUs, Bprm->PerMaxTrq );
			rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_FOR );
			if( rc == PRM_RSLT_SUCCESS )
			{
				EasyFFT->var.DrvDir = EZFFT_DRV_FORWARD;
				EasyFFT->var.PosDrvCount++;
				EasyFFT->OpeReg.EzFftState = EZFFT_STS_RUN;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;

				/* テーブル運転処理 */
				FunExe->State = EZFFT_SEQ_RESULT;
				rc = PRM_RSLT_CONTINUE;
			}
			break;

		case FCMD_DOWN:
			easyFftTuneRegClear( EasyFFT );
			easyFftSetRefCommand( EasyFFT, Bprm->SvCycleUs, Bprm->PerMaxTrq );
			rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_REV );
			if( rc == PRM_RSLT_SUCCESS )
			{
				EasyFFT->var.DrvDir = EZFFT_DRV_REVERSE;
				EasyFFT->var.NegDrvCount++;
				EasyFFT->OpeReg.EzFftState = EZFFT_STS_RUN;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;

				/* テーブル運転処理 */
				FunExe->State = EZFFT_SEQ_RESULT;
				rc = PRM_RSLT_CONTINUE;
			}
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}

		/* サーボオフで処理ステートの移行処理 */
		if( Axis->SeqCtrlOut->BaseEnableReq == FALSE )
		{
			FunExe->State = EZFFT_SEQ_READY;
		}
		break;

	case EZFFT_SEQ_RESULT:			/* EasyFFT処理 */
		rc = easyFftSequence( EasyFFT, TblDrive, TrcReg, Bprm );
		if( EasyFFT->OpeReg.EzFftState != EZFFT_STS_RUN )
		{
			if( EasyFFT->OpeReg.EzFftState == EZFFT_STS_END )
			{
				switch( Prm->tfuncsw & 0x0F0F )
				{
				case 0x0000:	/* ノッチフィルタを使用していない場合		*/
				case 0x0100:	/* 2段目ノッチフィルタを使用している場合	*/
					EasyFFT->OpeReg.EzFftFil1 = EasyFFT->OpeReg.EzFftDetf;
					EasyFFT->OpeReg.EzFftFil1 = TunePrmLimitUnsigned( EasyFFT->OpeReg.EzFftFil1, &pndef_ntchfil1 );
					break;

				case 0x0001:	/* 1段目ノッチフィルタを使用している場合	*/
					EasyFFT->OpeReg.EzFftFil2 = EasyFFT->OpeReg.EzFftDetf;
					EasyFFT->OpeReg.EzFftFil2 = TunePrmLimitUnsigned( EasyFFT->OpeReg.EzFftFil2, &pndef_ntchfil2 );
					break;

				default:
					break;
				}
			}
			easyFftClearCounter( EasyFFT );

			switch( FunExe->CmdCode )
			{
			case FCMD_UP:
				easyFftTuneRegClear( EasyFFT );
				easyFftSetRefCommand( EasyFFT, Bprm->SvCycleUs, Bprm->PerMaxTrq );
				rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_FOR );
				if( rc == PRM_RSLT_SUCCESS )
				{
					EasyFFT->var.DrvDir = EZFFT_DRV_FORWARD;
					EasyFFT->var.PosDrvCount++;
					EasyFFT->OpeReg.EzFftState = EZFFT_STS_RUN;
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
					rc = PRM_RSLT_CONTINUE;
				}
				break;

			case FCMD_DOWN:
				easyFftTuneRegClear( EasyFFT );
				easyFftSetRefCommand( EasyFFT, Bprm->SvCycleUs, Bprm->PerMaxTrq );
				rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_REV );
				if( rc == PRM_RSLT_SUCCESS )
				{
					EasyFFT->var.DrvDir = EZFFT_DRV_REVERSE;
					EasyFFT->var.NegDrvCount++;
					EasyFFT->OpeReg.EzFftState = EZFFT_STS_RUN;
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
					rc = PRM_RSLT_CONTINUE;
				}
				break;

			case FCMD_EWRT:
				easyFftSetTuningGain( EasyFFT, &AccessPrm );
//				easyFftUpdateGainInEeprom( RegCfg );
				rc = easyFftUpdateGainInEeprom( RegCfg );		/* <S085> */
				FunExe->State = EZFFT_SEQ_READY;
				break;

			default:
				break;
			}
		}
		else
		{
			DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );
		}

		switch( FunExe->CmdCode )
		{
		case FCMD_STOP:
			easyFftClearCounter( EasyFFT );
			easyFftTuneRegClear( EasyFFT );
			easyFftSetRefCommand( EasyFFT, Bprm->SvCycleUs, Bprm->PerMaxTrq );
			EasyFFT->OpeReg.EzFftState = EZFFT_STS_END;
			FunExe->State = EZFFT_SEQ_DRIVE;
			break;

		case FCMD_SV:
			FnCmnCtrl->FnSvonReq = (~FnCmnCtrl->FnSvonReq) & 0x01;
			EasyFFT->OpeReg.EzFftState = EZFFT_STS_END;
			break;

		case FCMD_NONE:
		case FCMD_UP:
		case FCMD_DOWN:
		case FCMD_EWRT:
			break;
		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		/* サーボオフで処理ステートの移行処理	*/
		if( Axis->SeqCtrlOut->BaseEnableReq == FALSE )
		{
			FunExe->State = EZFFT_SEQ_READY;
		}
		break;

	case EZFFT_SEQ_END:	/* 終了処理		*/
		FnLeaveEasyFft( FunExe, Axis );
		break;

	default:
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	低周波振動検出																				*/
	/*----------------------------------------------------------------------------------------------*/
	if( (EasyFFT->var.Drived == FALSE)
		&& (DetVibGetSpdErrPeakFn(Axis->DetVib) > (2 * DetVibGetVibCheckLevel(Axis->DetVib))) )
	{

		ALMSetServoAlarm( Axis->AlmManager, ALM_AT );
		DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );
	}

	if( (ALMGetAlarmInfo(Axis->AlmManager) != FALSE)
		|| (Axis->CtrlLoopOut->TrqClamp == TRUE)
		|| (Axis->SeqCtrlOut->HwbbSts == TRUE)
		|| (Axis->SeqCtrlOut->MainPowerOn == FALSE) )
	{ /* アラーム発生中 || トルク制限中 || HWBB状態中 || 主回路遮断 */
		FnCmnCtrl->FnSvonReq = FALSE;
		EasyFFT->OpeReg.EzFftState = EZFFT_STS_ERR;
	}

	return rc;
}


/* -------- Fn206  : Easy FFT Sequence Parameter Calculation	-------- */
void	EasyFftInitialize( EASYFFT *EasyFFT, USHORT *TableBuf, USHORT *TraceBuf, UNI_PRM_STR *prm_hndl_ptr )
{
	LONG		lwork;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	Prm = prm_hndl_ptr->Prm;
	Bprm = prm_hndl_ptr->Bprm;
	EasyFFT->OpeReg.EzFftMode = (Prm->ezfftsw & 0x0F00) >> 8;

	if( (Prm->ezfftsw & 0x000F) == 0x0001 )
	{
		EasyFFT->conf.UseHpFil = TRUE;
	}
	else
	{
		EasyFFT->conf.UseHpFil = FALSE;
	}

	EasyFFT->conf.ReTryMethod = (Prm->ezfftsw >> 8) & 0x000F;

	switch( EasyFFT->conf.ReTryMethod )
	{
		case EZFFT_ONESTEP_RETRY_0:
		case EZFFT_OPEN_MANUAL:
		case EZFFT_CLOSE_MANUAL:
			EasyFFT->conf.ReTryMaxCount = 0;
			break;

		case EZFFT_ONESTEP_RETRY_1:
			EasyFFT->conf.ReTryMaxCount = 1;
			break;

		default:
			EasyFFT->conf.ReTryMaxCount = EasyFFT->conf.ReTryMethod;
			break;
	}

	/* 単位変換用検出最大周波数 (ScanB周波数の1/2) */
	EasyFFT->conf.UnitMaxFreq = (USHORT)(1000000 / Bprm->SvCycleUs / 2);
	/* 周波数単位変換ゲイン ([2^22/UnitMaxFreq]に変換) */
	EasyFFT->conf.KnormFrq = MlibScalKxgain( 1<<22, 1, EasyFFT->conf.UnitMaxFreq, NULL, 24 );
	/* Step毎のバラツキ許容値 (100[Hz]) */
	EasyFFT->conf.Tolerance = (100 << 22) / EasyFFT->conf.UnitMaxFreq;
	/* オーバーフロー対策 単位逆変換用シフト数 */
	/* ((EZFFT_RETURN_UNIT * 検出周波数) >> RetUnitShift)で単位を戻す) */
	lwork = EasyFFT->conf.UnitMaxFreq / EZFFT_RETURN_UNIT;
	EasyFFT->conf.RetUnitShift = 0;
	while( lwork > 1 )
	{
		EasyFFT->conf.RetUnitShift++;
		lwork >>= 1;
	}
	EasyFFT->conf.RetUnitShift = 22 - EasyFFT->conf.RetUnitShift;

	/* 検出最大周波数設定(RAM値) */
	Prm->swpmaxfrq = EasyFFT->conf.UnitMaxFreq;

	/* テーブル指令バッファポインタ設定 */
	EasyFFT->conf.TblBufAddr = (USHORT *)TableBuf;
	/* トレースバッファポインタ設定 */
	EasyFFT->conf.TrcBufAddr = (USHORT *)TraceBuf;
	return;
}


/* -------- Fn206 : Easy FFT Tuning Gain	-------- */
static PRM_RSLT	easyFftSetTuningGain( EASYFFT *EasyFFT, REG_ACC_T *AccessPrm )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	switch( EasyFFT->var.NotchSw & 0x0F0F )
	{
	case 0x0000:	/* ノッチフィルタを使用していない場合 */
	case 0x0100:	/* 2段目ノッチフィルタを使用している場合 */
		EasyFFT->var.NotchSw |= 0x0001;
		/* Pn409 : 1段目ノッチフィルタ周波数 */
//		rc = RpiPrmWriteRamForFn( &pndef_ntchfil1, EasyFFT->OpeReg.EzFftFil1, Hmsg );
		rc = TunePrmWriteRam( AccessPrm, &pndef_ntchfil1, EasyFFT->OpeReg.EzFftFil1, 2 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		break;

	case 0x0001:	/* 1段目ノッチフィルタを使用している場合 */
		EasyFFT->var.NotchSw |= 0x0100;
		/* Pn40C : 2段目ノッチフィルタ周波数 */
//		rc = RpiPrmWriteRamForFn( &pndef_ntchfil2, EasyFFT->OpeReg.EzFftFil2, Hmsg );
		rc = TunePrmWriteRam( AccessPrm, &pndef_ntchfil2, EasyFFT->OpeReg.EzFftFil2, 2 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		break;

	default:
		break;
	}

	/* Pn408 : トルク関係機能スイッチ */
//	rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, EasyFFT->var.NotchSw, Hmsg );
	rc = TunePrmWriteRam( AccessPrm, &pndef_tfuncsw, EasyFFT->var.NotchSw, 2 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	return(rc);
}



/* -------- Fn206 : Easy FFT Gain In EEPROM	-------- */
static PRM_RSLT	easyFftUpdateGainInEeprom( REGIF_CONFIG_T *RegCfg )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/* Pn408 トルク関係機能スイッチ */
//	rc = RpiFunPrmNvmemUpdate( &pndef_tfuncsw );
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn409 1段目ノッチフィルタ周波数 */
//	rc = RpiFunPrmNvmemUpdate( &pndef_ntchfil1 );
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40C 2段目ノッチフィルタ周波数 */
//	rc = RpiFunPrmNvmemUpdate( &pndef_ntchfil2 );
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	return(rc);
}



/* -------- Fn206 : Write Gain To Ram With EEPROM Data	-------- */
static void	easyFftUndoRamGain( REGIF_CONFIG_T *RegCfg )
{
	/* RAMの値をEEPROMの値と同じにする */
	/* Pn408 トルク関係機能スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw, 0 );
	/* Pn409 1段目ノッチフィルタ周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );
	/* Pn40C 2段目ノッチフィルタ周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );

	return;
}



/* -------- Fn206  : Easy FFT Parameter Backup	-------- */
static void	easyFftSetPrmBackup( EASYFFT *EasyFFT )
{
	/* 発振検出周波数の下限 */
	EasyFFT->var.OrgFrqLowerLmt = EasyFFT->var.FrqLowerLmt;
	/* 掃引周波数の最小値のバックアップ */
	EasyFFT->var.OrgFrqSwpMin = EasyFFT->var.FrqSwpMin;
	/* 掃引周波数の最大値のバックアップ */
	EasyFFT->var.OrgFrqSwpMax = EasyFFT->var.FrqSwpMax;
	return;
}



/* -------- Fn206  : Easy FFT Undo Backuped Parameter	-------- */
static void	easyFftUndoPrmBackup( EASYFFT *EasyFFT )
{
	EasyFFT->var.FrqLowerLmt = EasyFFT->var.OrgFrqLowerLmt;	/* 発振検出周波数の下限					*/
	EasyFFT->var.FrqSwpMin = EasyFFT->var.OrgFrqSwpMin;		/* 掃引周波数の最小値					*/
	EasyFFT->var.FrqSwpMax = EasyFFT->var.OrgFrqSwpMax;		/* 掃引周波数の最大値					*/
	return;
}



/* -------- Fn206  : Easy FFT Set Trace Registers	-------- */
static void	easyFftSetTraceRegister( TRCREG *TrcReg, LONG ax, USHORT ezfftsw )
{
	LONG	AxisSel;

	/* トレース対象軸選択設定 */
//	AxisSel = ax << 8; /* <S1C8> */
	AxisSel = ax;

	TrcReg->TrcMode = 0x08;									/* トレースモード						*/
	TrcReg->DataSize = 1;									/* トレースデータサイズ					*/
//<S153>	TrcReg->AreaSize = EZFFT_SIZ_TRACE;				/* トレースエリアサイズ					*/
	TrcReg->TrcBuffSize = EZFFT_SIZ_TRACE;					/* <S153> トレースエリアサイズ			*/
	TrcReg->SampIntv = 1;									/* トレースサンプリング間隔				*/

/* <S0C9>	Start */
	/* トレース対象選択 */
//	TrcReg->TrcSel[0] = (USHORT)(AxisSel | 0x01);			/* トルク/推力指令						*/
//	TrcReg->TrcSel[2] = (USHORT)(AxisSel | 0x00);			/* トレース無し							*/
//	TrcReg->TrcSel[3] = (USHORT)(AxisSel | 0x00);			/* トレース無し							*/

//	switch( ezfftsw & 0x000F )
//	{
//	case 1:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x1E);		/* 外乱前トルク指令						*/
//		break;

//	case 2:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x01);		/* トルク/推力指令						*/
//		break;

//	case 3:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x42);		/* 負荷位置差分							*/
//		break;

//	case 0:
//	default:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x40);		/* 位置制御FB差分						*/
//		break;
//	}

//	/* IOトレース対象選択(IOトレースなし) */
//	TrcReg->IOTrcSel[0] = 0;
//	TrcReg->IOTrcSel[1] = 0;
//	TrcReg->IOTrcSel[2] = 0;
//	TrcReg->IOTrcSel[3] = 0;

	/* トレース軸選択 */
	TrcReg->TrcAxisSel[0] = AxisSel;		/* トルク/推力指令						*/
	TrcReg->TrcAxisSel[2] = AxisSel;		/* トレース無し							*/
	TrcReg->TrcAxisSel[3] = AxisSel;		/* トレース無し							*/

	/* トレース対象選択 */
	TrcReg->TrcDataSel[0] = 0x01;			/* トルク/推力指令						*/
	TrcReg->TrcDataSel[2] = 0x00;			/* トレース無し							*/
	TrcReg->TrcDataSel[3] = 0x00;			/* トレース無し							*/

	switch( ezfftsw & 0x000F )
	{
	case 1:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* 外乱前トルク指令						*/
		TrcReg->TrcDataSel[1] = 0x1E;		/* 外乱前トルク指令						*/
		break;

	case 2:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* トルク/推力指令						*/
		TrcReg->TrcDataSel[1] = 0x01;		/* トルク/推力指令						*/
		break;

	case 3:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* 負荷位置差分							*/
		TrcReg->TrcDataSel[1] = 0x42;		/* 負荷位置差分							*/

		break;

	case 0:
	default:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* 位置制御FB差分						*/
		TrcReg->TrcDataSel[1] = 0x40;		/* 位置制御FB差分						*/
		break;
	}

	/* IOトレース対象選択(IOトレースなし) */
	TrcReg->IOTrcAxisSel[0] = 0;
	TrcReg->IOTrcDataSel[0] = 0;

	TrcReg->IOTrcAxisSel[1] = 0;
	TrcReg->IOTrcDataSel[1] = 0;

	TrcReg->IOTrcAxisSel[2] = 0;
	TrcReg->IOTrcDataSel[2] = 0;

	TrcReg->IOTrcAxisSel[3] = 0;
	TrcReg->IOTrcDataSel[3] = 0;
/* <S0C9>	End */

	TrcReg->PtrgDnum = 0;									/* プリトリガデータ数					*/
	TrcReg->TrgSel = 0;										/* トリガ条件							*/
	TrcReg->MTrgMode = 0;									/* マルチトリガモード					*/

}


/* -------- Fn206  : Easy FFT Table Set Drive Registers	-------- */
static void	easyFftSetTblCmdRegister( TBLDRIVE *TblDrive )
{
	TblDrive->OpeReg.TblMode = 0x05;						/* テーブル運転制御モード				*/
	TblDrive->OpeReg.TblLimit = 1;							/* テーブル運転回転量リミット			*/
	TblDrive->OpeReg.TblFuncSel = 0x0002;					/* テーブル運転機能選択					*/
}


/* -------- Fn206  : Easy FFT Clear Counter and Step for retry	-------- */
static void	easyFftClearCounter( EASYFFT *EasyFFT )
{
	EasyFFT->var.DetCount = 0;
	EasyFFT->var.StepReTryCnt = 0;
	EasyFFT->var.AvrReTryCnt = 0;
	EasyFFT->var.DetReTryCnt = 0;
	EasyFFT->var.PosDrvCount = 0;
	EasyFFT->var.NegDrvCount = 0;

	EasyFFT->var.Step = EZFFT_STEP_1;
	EasyFFT->var.Drived = FALSE;
	EasyFFT->var.Zoom = FALSE;
	easyFftUndoPrmBackup( EasyFFT );

	return;
}


/* -------- Fn206  : Easy FFT Drive/Analyze Sequence	-------- */
static PRM_RSLT	easyFftSequence( EASYFFT *EasyFFT, TBLDRIVE *TblDrive, TRCREG *TrcReg, BPRMDAT *Bprm )
{
	PRM_RSLT	rc;
	PRM_RSLT	lrc;
	ULONG		ulwork;
	ULONG		DevStep;	/* Stepバラツキ */
	USHORT		DevDir;		/* 正逆バラツキ */
	USHORT		uswork;

	rc = PRM_RSLT_CONTINUE;

	switch( EasyFFT->var.SeqPtr )
	{
	case EZFFT_LSEQ_DRV:
		if( EasyFFT->var.Drived == TRUE )
		{
			if( EasyFFT->var.SameDir == FALSE )
			{
				EasyFFT->var.DrvDir = (~EasyFFT->var.DrvDir) & 0x01;
			}
			else
			{
				EasyFFT->var.SameDir = FALSE;
			}

			if( EasyFFT->var.DrvDir == EZFFT_DRV_FORWARD )
			{
				lrc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_FOR );
				EasyFFT->var.PosDrvCount++;
			}
			else
			{
				lrc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_REV );
				EasyFFT->var.NegDrvCount++;
			}

			if( lrc == PRM_RSLT_SUCCESS )
			{
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_WAITSTOP;
			}
			else
			{
				rc = PRM_RSLT_CONDITION_ERR;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
			}
		}
		else
		{
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_WAITSTOP;
		}
		break;

	case EZFFT_LSEQ_WAITSTOP:	/* 運転＆トレース完了 */
		switch( TblDrive->OpeReg.TblState )
		{
			case TBLCMD_STS_POSLMT:
			case TBLCMD_STS_BB:
			case TBLCMD_STS_OT:
			case TBLCMD_STS_POWEROFF:
			case TBLCMD_STS_HWBB:
			case TBLCMD_STS_ALARM:
				/* テーブル運転異常終了	*/
				rc = PRM_RSLT_CONDITION_ERR;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
				break;

			case TBLCMD_STS_TRQLMT:
			case TBLCMD_STS_FINISH:
				EasyFFT->var.Drived = TRUE;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DETFRQ;
				break;

			default:
				break;
		}
		break;

	case EZFFT_LSEQ_DETFRQ:	/* 周波数検出 */
		if( EasyFFT->conf.UseHpFil )
		{
			/* HighPassFilter計算 */
			easyFftCalcHpFilter( EasyFFT, TrcReg );
		}
		/* 周波数演算 */
		easyFftGetFreqGainMax( EasyFFT, TrcReg );

		/* 検出値が小さいか、設定最大値と同じなら非0として検出エラー */
		if( EasyFFT->var.SpcPeak0 == 0 )
		{
			EasyFFT->var.AnalyzeState = EZFFT_ANASTS_NG;
		}
		else
		{
#if (FLOAT_USE==TRUE)
			if( ((EasyFFT->var.SwpDir == EZFFT_FRQ_H2L) && (EasyFFT->var.DetFrq0 == ( (LONG)EasyFFT->var.FrqSwpMin * EasyFFT->conf.KnormFrq ))) ||
				 ((EasyFFT->var.SwpDir == EZFFT_FRQ_L2H) && (EasyFFT->var.DetFrq0 == ( (LONG)EasyFFT->var.FrqSwpMax * EasyFFT->conf.KnormFrq ))) )
			{
				EasyFFT->var.AnalyzeState = EZFFT_ANASTS_NG;
			}
#else
			if( ((EasyFFT->var.SwpDir == EZFFT_FRQ_H2L) && (EasyFFT->var.DetFrq0 == MlibMulgain( EasyFFT->var.FrqSwpMin, EasyFFT->conf.KnormFrq ))) ||
				 ((EasyFFT->var.SwpDir == EZFFT_FRQ_L2H) && (EasyFFT->var.DetFrq0 == MlibMulgain( EasyFFT->var.FrqSwpMax, EasyFFT->conf.KnormFrq ))) )
			{
				EasyFFT->var.AnalyzeState = EZFFT_ANASTS_NG;
			}
#endif /* FLOAT_USE */
			else
			{
				EasyFFT->var.AnalyzeState = EZFFT_ANASTS_OK;
			}
		}

		/********************************************************************************/
		/*	== [判断] ==																*/
		/*		オープンループor閉ループのマニュアル動作なら、正常・異常に関わらず終了。*/
		/*		それ以外（閉ループ・自動）なら											*/
		/*			Pリトライ以内なら再挑戦する。										*/
		/*			Pリトライを超えていれば、											*/
		/*				正逆評価で、次のチャンスがあれば、次の方向回転に進む。			*/
		/*				次のチャンスが無く、前にOKがあれば継続							*/
		/*				上記以外なら異常終了											*/
		/********************************************************************************/
		EasyFFT->var.FinalDetFrq = EasyFFT->var.DetFrq0;

		/* マニュアル(オープンループ、トルク制御)、リトライ０回、片側運転のみ */
		/* または、マニュアル(位置制御)、リトライ０回、片側運転のみ */
		if( (EasyFFT->conf.ReTryMethod == EZFFT_OPEN_MANUAL)
			|| (EasyFFT->conf.ReTryMethod == EZFFT_CLOSE_MANUAL) )
		{
			if( EasyFFT->var.AnalyzeState == EZFFT_ANASTS_NG )
			{
				EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
			}
			else
			{
				EasyFFT->var.DetState0 = EZFFT_STS_END;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DONE;
			}
		}
		else
		{
			if( EasyFFT->var.AnalyzeState == EZFFT_ANASTS_NG )
			{
				if( EasyFFT->var.DetReTryCnt < EasyFFT->conf.ReTryMaxCount )
				{/* リトライ有り無し */
					/* 異常時、もしリトライ以内なら、再トライP++ */
					EasyFFT->var.DetReTryCnt++;
					EasyFFT->var.DetState0 = EZFFT_STS_RUN;

					EasyFFT->var.SameDir = TRUE;
					if( EasyFFT->var.DrvDir == EZFFT_DRV_FORWARD )
					{
						EasyFFT->var.PosDrvCount--;
					}
					else
					{
						EasyFFT->var.NegDrvCount--;
					}
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
				}
				else
				{
					/* リトライを超えたとき*/
					EasyFFT->var.DetState[ EasyFFT->var.DetCount ] = EZFFT_STS_DETNG;
					EasyFFT->var.DetFrq[ EasyFFT->var.DetCount ] = EasyFFT->var.DetFrq0;
					EasyFFT->var.DetCount++;

					/* もし正/逆の片方が駄目でも逆のチャンスが有れば、「次を」実行する。*/
					if( EasyFFT->var.PosDrvCount == EasyFFT->var.NegDrvCount )
					{
						/* 今回は異常だけど、前回はOKのとき */
						if( EasyFFT->var.DetState0 == EZFFT_STS_END )
						{
							EasyFFT->var.DetState0 = EZFFT_STS_ERR;
							EasyFFT->var.SeqPtr = EZFFT_LSEQ_STPEND;
						}
						else
						{
						/* 今回も、前回もNGのとき */
							EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
							EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
						}
					}
					else
					{
						EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
						EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;/* 「次を」実行 *//* 進む */
						EasyFFT->var.DetReTryCnt = 0;
					}
				}
			}
			else
			{
				EasyFFT->var.DetState[ EasyFFT->var.DetCount ] = EZFFT_STS_END;
				EasyFFT->var.DetFrq[ EasyFFT->var.DetCount ] = EasyFFT->var.DetFrq0;
				EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ] = EasyFFT->var.DetFrq0;
				EasyFFT->var.DetCount++;

				EasyFFT->var.DetReTryCnt = 0;

				if( EasyFFT->var.PosDrvCount == EasyFFT->var.NegDrvCount )
				{
					/* 両側完了平均化*/
					if( EasyFFT->var.DetState0 == EZFFT_STS_END )
					{
						/* 今回も前回も正常*/
						EasyFFT->var.SeqPtr = EZFFT_LSEQ_AVRFRQ;	/* 両方正常に完了*/
					}
					else
					{
						/* 今回は正常だが、前回は異常 */
						EasyFFT->var.DetState0 = EZFFT_STS_ERR;/* 今の結果ではない。最終結果2*/
						EasyFFT->var.SeqPtr = EZFFT_LSEQ_STPEND;/* 片側で決定*/
					}
				}
				else
				{/* 片側完了*/
					EasyFFT->var.DetState0 = EZFFT_STS_END;/* =1 今は正常 */
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;/* 再度回転へ進む */
				}
			}
		}
		break;

	case EZFFT_LSEQ_AVRFRQ:	/* 正逆の平均化 : STEPによらず正逆が共に正常終了した場合 */
		EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ] =
			(EasyFFT->var.DetFrq[ 2*EasyFFT->var.Step ] + EasyFFT->var.DetFrq[ 2*EasyFFT->var.Step+1 ]) >> 1;
		DevDir = easyFftGetAvrgFreq( EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ],
									 EasyFFT->var.DetFrq[ 2*EasyFFT->var.Step ] );
		uswork = easyFftGetAvrgFreq( EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ],
									 EasyFFT->var.DetFrq[ 2*EasyFFT->var.Step+1 ] );

		if( uswork > DevDir )
		{
			DevDir = uswork;
		}

		if( DevDir > EZFFT_DEVLVL_DIR )
		{/* バラツキ異常 */
			/* 正逆のバラツキが大きいとき */
			if( EasyFFT->var.AvrReTryCnt < EasyFFT->conf.ReTryMaxCount )
			{
				/* リトライ */
				EasyFFT->var.DetCount = 2 * EasyFFT->var.Step;
				EasyFFT->var.DetReTryCnt = 0;
				EasyFFT->var.AvrReTryCnt++;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
			}
			else
			{
				if( EasyFFT->var.Step == EZFFT_STEP_2 )
				{
					/* Zoom解析の異常 *//* ワーニング */
					EasyFFT->var.DetState0 = EZFFT_STS_ERR;
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_WARN;
				}
				else
				{
					/* 1段階目で異常 */
					EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
				}
			}
		}
		else
		{/* バラツキが小さいとき */
			/* 決定して 先に進む */
			EasyFFT->var.DetState0 = EZFFT_STS_END;
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_STPEND;
		}
		break;

	case EZFFT_LSEQ_STPEND:/* STEP完了 */
		switch( EasyFFT->conf.ReTryMethod )
		{
		case EZFFT_ONESTEP_RETRY_0:
			/* =6 これで完了 5,4はこの前に完了 */
			EasyFFT->var.FinalDetFrq = EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ];
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_DONE;/* これで終了 */
			break;

		case EZFFT_ONESTEP_RETRY_1:
			/* Zoomは行わない。Retryは1回まで */
			if( (EasyFFT->var.DetState0 != EZFFT_STS_END) && (EasyFFT->var.StepReTryCnt < EasyFFT->conf.ReTryMaxCount) )
			{
				/* もう一度最初から */
				EasyFFT->var.Step = EZFFT_STEP_1;
				EasyFFT->var.DetCount = 2 * EasyFFT->var.Step;
				EasyFFT->var.AvrReTryCnt = 0;
				EasyFFT->var.DetReTryCnt = 0;
				EasyFFT->var.DetState0 = EZFFT_STS_RUN;
				EasyFFT->var.StepReTryCnt++;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
			}
			else
			{
				EasyFFT->var.FinalDetFrq = EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ];
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DONE;
			}
			break;

		default:
			if( EasyFFT->var.Step < EZFFT_STEP_2 )
			{
				/* STEP-1完了 */
				EasyFFT->var.AvrReTryCnt = 0;
				EasyFFT->var.DetReTryCnt = 0;
				EasyFFT->var.DetState0 = EZFFT_STS_RUN;
				EasyFFT->var.Step = EZFFT_STEP_2;
				EasyFFT->var.DetCount = 2 * EasyFFT->var.Step;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ZOOMREQ;	/* STEP-1完了　Zoom解析まで飛ぶ */
			}
			else
			{/* STEP-2まで完了 */
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_AVRSTP;
			}
			break;
		}
		break;

	case EZFFT_LSEQ_AVRSTP:		/* 1回と2回の平均化 */
		EasyFFT->var.FinalDetFrq = (EasyFFT->var.DetAvrFrq[0] + EasyFFT->var.DetAvrFrq[1]) >> 1;

		if( EasyFFT->var.FinalDetFrq > EasyFFT->var.DetAvrFrq[0] )
		{
			DevStep = EasyFFT->var.FinalDetFrq - EasyFFT->var.DetAvrFrq[0];
		}
		else
		{
			DevStep = EasyFFT->var.DetAvrFrq[0] - EasyFFT->var.FinalDetFrq;
		}

		if( EasyFFT->var.FinalDetFrq > EasyFFT->var.DetAvrFrq[1] )
		{
			ulwork = EasyFFT->var.FinalDetFrq - EasyFFT->var.DetAvrFrq[1];
		}
		else
		{
			ulwork = EasyFFT->var.DetAvrFrq[1] - EasyFFT->var.FinalDetFrq;
		}
		if( ulwork > DevStep )
		{
			DevStep = ulwork;
		}

		if( DevStep < EasyFFT->conf.Tolerance )
		{
			/* 順調に次へ */
			EasyFFT->var.StepReTryCnt = 0;
			EasyFFT->var.DetState0 = EZFFT_STS_END;
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_DONE;
		}
		else
		{/* バラツキが許容値より大きい場合 */
			if( EasyFFT->conf.ReTryMaxCount > EasyFFT->var.StepReTryCnt )
			{
				/* もう一回最初からトライ */
				if( EasyFFT->var.Zoom == TRUE )
				{ /* Zoom解析設定前に戻す */
					easyFftUndoPrmBackup( EasyFFT );
				}
				EasyFFT->var.Zoom = FALSE;
				EasyFFT->var.Step = EZFFT_STEP_1;
				EasyFFT->var.DetCount = 2 * EasyFFT->var.Step;
				EasyFFT->var.AvrReTryCnt = 0;
				EasyFFT->var.DetReTryCnt = 0;
				EasyFFT->var.DetState0 = EZFFT_STS_RUN;
				EasyFFT->var.StepReTryCnt++;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
			}
			else
			{ /* リトライ超 */
				EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
			}
		}
		break;

	case EZFFT_LSEQ_DONE:	/* 終了				*/
	case EZFFT_LSEQ_ERR:	/* 異常終了			*/
	case EZFFT_LSEQ_WARN:	/* ワーニング終了	*/
		rc = PRM_RSLT_SUCCESS;
		EasyFFT->OpeReg.EzFftState = EasyFFT->var.DetState0;
		/* Hz換算 2^22 * 125(EZFFT_RETURN_UNIT) = 1F400000(max) */
		EasyFFT->OpeReg.EzFftDetf =
			(USHORT)((EasyFFT->var.FinalDetFrq * EZFFT_RETURN_UNIT) >> EasyFFT->conf.RetUnitShift);
		break;

	case EZFFT_LSEQ_ZOOMREQ: /* Zoomの再設定 */
		EasyFFT->var.Zoom = TRUE;
		easyFftSetPrmBackup( EasyFFT );
		easyFftSetZoomAnalysis( EasyFFT );
		easyFftSetRefCommand( EasyFFT, Bprm->SvCycleUs, Bprm->PerMaxTrq );
		EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
		break;

	default:
		break;
	}
	return( rc );
}


/* -------- Fn206  : Easy FFT Drive Command Set	-------- */
static void	easyFftSetRefCommand( EASYFFT *EasyFFT, LONG SvCycleUs, USHORT PerMaxTrq )
{
	SHORT	swork;
	LONG	lwork;

	swork = EZFFT_SIZ_TRACE - 1;

	if( EasyFFT->var.FrqSwpMin > EasyFFT->var.FrqSwpMax )
	{
		EasyFFT->var.SwpDir = EZFFT_FRQ_H2L;
		lwork = EasyFFT->var.FrqSwpMin - EasyFFT->var.FrqSwpMax;
	}
	else
	{
		EasyFFT->var.SwpDir = EZFFT_FRQ_L2H;
		lwork = EasyFFT->var.FrqSwpMax - EasyFFT->var.FrqSwpMin;
	}
#if (FLOAT_USE==TRUE)
	lwork = ( MlibAbs32(lwork) * EasyFFT->conf.KnormFrq );
#else
	lwork = MlibMulgain( MlibAbs32(lwork), EasyFFT->conf.KnormFrq );
#endif /* FLOAT_USE */
	EasyFFT->var.DeltaFrqRes = lwork / swork;
	EasyFFT->var.DeltaFrqRef = EasyFFT->var.DeltaFrqRes >> 1;

	EasyFFT->var.KTrqRef = MlibScalKxgain( EasyFFT->OpeReg.EzFftTrq, 1, PerMaxTrq, NULL, 24 );
	lwork = (EasyFFT->var.FrqLowerLmt + EasyFFT->var.FrqSwpMin) >> 1;
	EasyFFT->var.KHpFilter = MlibPcalKf1gain( lwork, SvCycleUs, 1 );

	easyFftMakeChirpRefTrace( EasyFFT );
	return;
}


/* -------- Fn206  : Easy FFT Tuning Registger Clear	-------- */
static void	easyFftTuneRegClear( EASYFFT *EasyFFT )
{
	EasyFFT->OpeReg.EzFftDetf = 0;
	EasyFFT->OpeReg.EzFftFil1 = 0;
	EasyFFT->OpeReg.EzFftFil2 = 0;
	return;
}


/****************************************************************************************************/
/*																									*/
/*		チャープ指令作成処理																		*/
/*																									*/
/****************************************************************************************************/
/*	概要：Chirp指令を作成し、指令テーブルに置く														*/
/****************************************************************************************************/
static void		easyFftMakeChirpRefTrace( EASYFFT *EasyFFT )
{
	ULONG	work;
	LONG	theta;
	USHORT	i;

	for( i = 0; i < EZFFT_SIZ_TRACE; i++ )
	{
		/* チャープ指令周波数取得 */
		work = easyFftGetChirpFreq( EasyFFT, i, EZFFT_ANAOBJ_REF );
		/* 2piを含む単位変換, 360[deg]/1024 */
		theta = ((LONG)((work >> 7) * i) & 0x0000FFFF) >> 6;
		/* テーブル指令登録(掃引トルク演算ゲイン換算) */
#if (FLOAT_USE==TRUE)
		EasyFFT->conf.TblBufAddr[ i ] =
			(USHORT)( MlibFASTSINS16( (USHORT)theta ) * EasyFFT->var.KTrqRef );
#else
		EasyFFT->conf.TblBufAddr[ i ] =
			(USHORT)MlibMulgain( MlibFASTSINS16( (USHORT)theta ), EasyFFT->var.KTrqRef );
#endif /* FLOAT_USE */
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		チャープ周波数取得処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要：掃引時のある点の周波数を求める。（指令作成、周波数推定の両方で用いる）					*/
/*																									*/
/*	引数：	LONG	ObjFlag			0 : 指令作成用													*/
/*									1 : 周波数検出用												*/
/*			USHORT	count			トレースポイント指定											*/
/*																									*/
/****************************************************************************************************/
static ULONG	easyFftGetChirpFreq( EASYFFT *EasyFFT, USHORT count, LONG ObjFlag )
{
	ULONG	work;
	LONG	lwork;

	if( ObjFlag == EZFFT_ANAOBJ_RES )
	{ /* 周波数検出 */
		work = (ULONG)(EasyFFT->var.DeltaFrqRes * count);
	}
	else
	{ /* 指令作成 */
		work = (ULONG)(EasyFFT->var.DeltaFrqRef * count);
	}

	/* 周波数単位変換 */
#if (FLOAT_USE==TRUE)
	lwork = (LONG)EasyFFT->var.FrqSwpMin * EasyFFT->conf.KnormFrq ;
#else
	lwork = MlibMulgain( EasyFFT->var.FrqSwpMin, EasyFFT->conf.KnormFrq );
#endif /* FLOAT_USE */

	if( EasyFFT->var.SwpDir == EZFFT_FRQ_H2L )
	{ /* トルク掃引方向：低⇒高 */
		work = (ULONG)lwork - work;
	}
	else
	{ /* トルク掃引方向：高⇒低 */
		work = (ULONG)lwork + work;
	}
	return work;
}



/****************************************************************************************************/
/*																									*/
/*		周波数演算処理																				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要：速度FB or 実トルクの絶対値が最大となる値とその周波数を求める。							*/
/*													[Hz] ==> [4000*EasyFFT->var.DetFrq >>22]		*/
/*																									*/
/****************************************************************************************************/
static void		easyFftGetFreqGainMax( EASYFFT *EasyFFT, TRCREG *TrcReg )
{
	ULONG	Freq;
	ULONG	ulwork;
	USHORT	Offset;
	SHORT	swork;
	USHORT	i;

	EasyFFT->var.SpcPeak0 = 0;
	EasyFFT->var.DetFrq0 = 0;
#if (FLOAT_USE==TRUE)
	ulwork = (LONG)EasyFFT->var.FrqLowerLmt, EasyFFT->conf.KnormFrq;
#else
	ulwork = MlibMulgain( EasyFFT->var.FrqLowerLmt, EasyFFT->conf.KnormFrq );
#endif /* FLOAT_USE */

	for( i = 0; i < EZFFT_SIZ_TRACE; i++ )
	{
		Offset = TrcReg->AreaSize / TrcReg->TrcMode / TrcReg->DataSize;		/* <S153> */
//		Offset = TrcReg->AreaSize;	/* <S153> */
		swork = ( (SHORT) ( (EasyFFT->conf.TrcBufAddr[Offset + i] & 0x7FFF) << 1 ) ) >> 1;

		/* チャープ周波数検出 */
		Freq = easyFftGetChirpFreq( EasyFFT, i, EZFFT_ANAOBJ_RES );

		if( Freq >= ulwork )
		{
			if( MlibAbs32( EasyFFT->var.SpcPeak0 ) < MlibAbs32( swork ) )
			{
				EasyFFT->var.SpcPeak0 = (USHORT)MlibAbs32( swork );
				EasyFFT->var.DetFrq0  = Freq;
			}
		}
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		トレーステーブル ハイパスフィルタリング処理													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要：トレーステーブルにハイパスフィルタをかける。												*/
/*			ノッチ検出などの場合、補償トルクのフィルタリングが有効。								*/
/*			垂直軸時のオフセット、低周波の振れを除去可能。											*/
/*																									*/
/****************************************************************************************************/
static void		easyFftCalcHpFilter( EASYFFT *EasyFFT, TRCREG *TrcReg )
{
	LONG	wkcalctmp;
	SHORT	wkref, wkout;
	USHORT	Offset;
	USHORT	i;

	for( i = 0; i < EZFFT_SIZ_TRACE; i++ )
	{
//		Offset = TrcReg->AreaSize / TrcReg->TrcMode / TrcReg->DataSize;
		Offset = TrcReg->AreaSize;
		wkref = ( (SHORT) ( (EasyFFT->conf.TrcBufAddr[Offset + i] & 0x7FFF) << 1 ) ) >> 1;

		if( i != 0 )
		{
			wkout = (SHORT)MlibHpfilter1( (LONG)wkref, EasyFFT->var.KHpFilter, &wkcalctmp );
		}
		else
		{
			wkout = wkref;
		}
		EasyFFT->conf.TrcBufAddr[ Offset + i ] = wkout;
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		周波数平均化処理																			*/
/*																									*/
/****************************************************************************************************/
static USHORT	easyFftGetAvrgFreq( ULONG frgavr, ULONG freq )
{
	USHORT	AvrgFreq;
	ULONG	ulwork;

	if( freq < frgavr )
	{
		ulwork = frgavr - freq;
	}
	else
	{
		ulwork = freq - frgavr;
	}
	AvrgFreq = (USHORT)(ulwork * 1000 / frgavr);

	return(AvrgFreq);
}


/****************************************************************************************************/
/*																									*/
/*		周波数解析結果周辺での再解析準備処理														*/
/*																									*/
/****************************************************************************************************/
static void		easyFftSetZoomAnalysis( EASYFFT *EasyFFT )
{
	USHORT	uswork;
	USHORT	Freq;

	/* Hz換算 2^22 * 125(EZFFT_RETURN_UNIT) = 1F400000(max) */
	Freq = (USHORT)((EasyFFT->var.FinalDetFrq * EZFFT_RETURN_UNIT) >> EasyFFT->conf.RetUnitShift);

	if( EasyFFT->var.SwpDir == EZFFT_FRQ_H2L )
	{
		uswork = Freq - 300;		/* -300Hz */
		if( uswork > EasyFFT->var.FrqSwpMax )
		{
			EasyFFT->var.FrqSwpMax = uswork;
		}
		uswork = Freq - 200;		/* -200Hz */
		if( uswork > EasyFFT->var.FrqLowerLmt )
		{
			EasyFFT->var.FrqLowerLmt = uswork;
		}
		uswork = Freq + 200;		/* +200Hz */
		if( uswork < EasyFFT->var.FrqSwpMin )
		{
			EasyFFT->var.FrqSwpMin = uswork;
		}
	}
	else
	{
		uswork = Freq - 300;		/* -300Hz */
		if( uswork > EasyFFT->var.FrqSwpMin )
		{
			EasyFFT->var.FrqSwpMin = uswork;
		}
		uswork = Freq - 200;		/* -200Hz */
		if( uswork > EasyFFT->var.FrqLowerLmt )
		{
			EasyFFT->var.FrqLowerLmt = uswork;
		}
		uswork = Freq + 200;		/* +200Hz */
		if( uswork < EasyFFT->var.FrqSwpMax )
		{
			EasyFFT->var.FrqSwpMax = uswork;
		}
	}
	return;
}


/****************************************************************************************************/
/*		EasyFFTトルク振幅設定処理 (レジスタIF)														*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetEasyFftTrqLevel( EASYFFT *EasyFFT, USHORT Lvl )
{
	PRM_RSLT	rc;
	USHORT		LvlLimit;

	rc = PRM_RSLT_SUCCESS;
	LvlLimit = TunePrmLimitUnsigned( Lvl, &pndef_ezffttrq );

	if( LvlLimit == Lvl )
	{
		EasyFFT->OpeReg.EzFftTrq = Lvl;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}


/****************************************************************************************************/
