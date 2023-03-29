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
		EasyFFT�ł́A�g���[�X�ƃe�[�u���^�]������I�Ɏg�p����B���̂��߁A
		�{���W���[���ł́A�g���[�X�ƃe�[�u���^�]�̃��W�X�^�̂ݒ��ڎQ�ƁA
		�����������s�����ƂƂ���B
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

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI�� */
	if( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* ���[�^���X�e�X�g���[�h�̏ꍇ�͓��炸�ɏI�� *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fn�T�[�{�^�]���l�� */
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

	/* �������X���������v�� */
	TuneLessSetForceInvldReq( &(Axis->BaseControls->TuneLessCtrl) );

	/* OT�M���}�X�N�ݒ� */
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

	/* �ݒ���o�b�N�A�b�v */
	easyFftSetPrmBackup( EasyFFT );

	/* ���W�X�^�ݒ�(EasyFFT�J�n) */
	EasyFFT->OpeReg.EzFftState = EZFFT_STS_END;

	/* Fn���[�h�����I���Ȃ� */
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

	/* EEPROM�f�[�^�ŕύX�p�����[�^�����ɖ߂� */
	easyFftUndoRamGain( RegCfg );

	/* OT�M���}�X�N���� */
	SeqIoClearOtSigMask( Axis->IoSignals,
	                       Axis->UniPrms->Prm->pot_selsw,
	                       Axis->UniPrms->Prm->not_selsw );

	/* Restore Original Table Drive Data */
	MlibCopyLongMemory( &(Axis->BaseControls->TblDrive),
						&OrgTblDrive,
						sizeof(Axis->BaseControls->TblDrive)/4 );

	/* �T�[�{�I�t����܂őҋ@ */
	FnCmnCtrl->FnSvonReq = FALSE;
	while( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		;
	}

	/* �������X���������v������ */
	TuneLessClearForceInvldReq( &Axis->BaseControls->TuneLessCtrl );

	/* �e�[�u���^�]�X�g�b�v */
	TblDriveSetStopRequest( &(Axis->BaseControls->TblDrive) );

	/* �e�[�u���^�]�����g���[�X���s�v������ */
	/* 2011.02.28 Y.Oka �e�[�u���^�]���g���[�X�������A���Ƃ��� */
//	TblDriveSetWithTrcReq( &(Axis->BaseControls->TblDrive), FALSE );

	/* Fn�T�[�{�^�]���J�� */
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


	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	rc = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case EZFFT_SEQ_INIT:
		/* �f�[�^�g���[�X & �e�[�u���^�]���s�p���W�X�^�v�Z���� */
		easyFftSetTraceRegister( TrcReg, Axis->AxisNo, Prm->ezfftsw );

/* <S0C9>	Start */
	for( i=0; i < TRC_CH_MAXNUM; i++ )
	{
		TrcIncivPrm->TrcData[i] = TrcReg->TrcDataSel[i];		/* �f�[�^�g���[�X�Ώۃf�[�^�I��(1�`16)			*/
		TrcIncivPrm->TrcAxis[i] = TrcReg->TrcAxisSel[i];		/* �f�[�^�g���[�X�Ώێ��I��(1�`16)				*/
		TrcIncivPrm->IOTrcData[i] = TrcReg->IOTrcDataSel[i];	/* I/O�g���[�X1�Ώۃf�[�^�I��(1�`16)			*/
		TrcIncivPrm->IOTrcAxis[i] = TrcReg->IOTrcAxisSel[i];	/* I/O�g���[�X1�Ώێ��I��(1�`16)				*/
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

		/* �e�[�u���^�]�����g���[�X���s�v�� */
		/* 2011.02.28 Y.Oka �e�[�u���^�]���g���[�X�������A���Ƃ��� */
//		TblDriveSetWithTrcReq( TblDrive, TRUE );

		DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );
		FunExe->State = EZFFT_SEQ_READY;
		
	
		/* �e�[�u���^�]�X�^�[�g */
		rc = TblDrivePrepareProc( TblDrive );
		if( rc == PRM_RSLT_SUCCESS )
		{
			/* ���䃂�[�h�ݒ� */
			FnCmnCtrl->FnCtrlMcmd = TblDriveGetControlMode( TblDrive );
		}
		else
		{
			/* �I�������� */
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
			{ /* �A���[�������� || HWBB || ���HOFF */
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
		/* �T�[�{�I���ŏ����X�e�[�g�̈ڍs���� */
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

				/* �e�[�u���^�]���� */
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

				/* �e�[�u���^�]���� */
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

		/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs���� */
		if( Axis->SeqCtrlOut->BaseEnableReq == FALSE )
		{
			FunExe->State = EZFFT_SEQ_READY;
		}
		break;

	case EZFFT_SEQ_RESULT:			/* EasyFFT���� */
		rc = easyFftSequence( EasyFFT, TblDrive, TrcReg, Bprm );
		if( EasyFFT->OpeReg.EzFftState != EZFFT_STS_RUN )
		{
			if( EasyFFT->OpeReg.EzFftState == EZFFT_STS_END )
			{
				switch( Prm->tfuncsw & 0x0F0F )
				{
				case 0x0000:	/* �m�b�`�t�B���^���g�p���Ă��Ȃ��ꍇ		*/
				case 0x0100:	/* 2�i�ڃm�b�`�t�B���^���g�p���Ă���ꍇ	*/
					EasyFFT->OpeReg.EzFftFil1 = EasyFFT->OpeReg.EzFftDetf;
					EasyFFT->OpeReg.EzFftFil1 = TunePrmLimitUnsigned( EasyFFT->OpeReg.EzFftFil1, &pndef_ntchfil1 );
					break;

				case 0x0001:	/* 1�i�ڃm�b�`�t�B���^���g�p���Ă���ꍇ	*/
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
		/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs����	*/
		if( Axis->SeqCtrlOut->BaseEnableReq == FALSE )
		{
			FunExe->State = EZFFT_SEQ_READY;
		}
		break;

	case EZFFT_SEQ_END:	/* �I������		*/
		FnLeaveEasyFft( FunExe, Axis );
		break;

	default:
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	����g�U�����o																				*/
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
	{ /* �A���[�������� || �g���N������ || HWBB��Ԓ� || ���H�Ւf */
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

	/* �P�ʕϊ��p���o�ő���g�� (ScanB���g����1/2) */
	EasyFFT->conf.UnitMaxFreq = (USHORT)(1000000 / Bprm->SvCycleUs / 2);
	/* ���g���P�ʕϊ��Q�C�� ([2^22/UnitMaxFreq]�ɕϊ�) */
	EasyFFT->conf.KnormFrq = MlibScalKxgain( 1<<22, 1, EasyFFT->conf.UnitMaxFreq, NULL, 24 );
	/* Step���̃o���c�L���e�l (100[Hz]) */
	EasyFFT->conf.Tolerance = (100 << 22) / EasyFFT->conf.UnitMaxFreq;
	/* �I�[�o�[�t���[�΍� �P�ʋt�ϊ��p�V�t�g�� */
	/* ((EZFFT_RETURN_UNIT * ���o���g��) >> RetUnitShift)�ŒP�ʂ�߂�) */
	lwork = EasyFFT->conf.UnitMaxFreq / EZFFT_RETURN_UNIT;
	EasyFFT->conf.RetUnitShift = 0;
	while( lwork > 1 )
	{
		EasyFFT->conf.RetUnitShift++;
		lwork >>= 1;
	}
	EasyFFT->conf.RetUnitShift = 22 - EasyFFT->conf.RetUnitShift;

	/* ���o�ő���g���ݒ�(RAM�l) */
	Prm->swpmaxfrq = EasyFFT->conf.UnitMaxFreq;

	/* �e�[�u���w�߃o�b�t�@�|�C���^�ݒ� */
	EasyFFT->conf.TblBufAddr = (USHORT *)TableBuf;
	/* �g���[�X�o�b�t�@�|�C���^�ݒ� */
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
	case 0x0000:	/* �m�b�`�t�B���^���g�p���Ă��Ȃ��ꍇ */
	case 0x0100:	/* 2�i�ڃm�b�`�t�B���^���g�p���Ă���ꍇ */
		EasyFFT->var.NotchSw |= 0x0001;
		/* Pn409 : 1�i�ڃm�b�`�t�B���^���g�� */
//		rc = RpiPrmWriteRamForFn( &pndef_ntchfil1, EasyFFT->OpeReg.EzFftFil1, Hmsg );
		rc = TunePrmWriteRam( AccessPrm, &pndef_ntchfil1, EasyFFT->OpeReg.EzFftFil1, 2 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		break;

	case 0x0001:	/* 1�i�ڃm�b�`�t�B���^���g�p���Ă���ꍇ */
		EasyFFT->var.NotchSw |= 0x0100;
		/* Pn40C : 2�i�ڃm�b�`�t�B���^���g�� */
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

	/* Pn408 : �g���N�֌W�@�\�X�C�b�` */
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

	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
//	rc = RpiFunPrmNvmemUpdate( &pndef_tfuncsw );
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn409 1�i�ڃm�b�`�t�B���^���g�� */
//	rc = RpiFunPrmNvmemUpdate( &pndef_ntchfil1 );
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40C 2�i�ڃm�b�`�t�B���^���g�� */
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
	/* RAM�̒l��EEPROM�̒l�Ɠ����ɂ��� */
	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw, 0 );
	/* Pn409 1�i�ڃm�b�`�t�B���^���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );
	/* Pn40C 2�i�ڃm�b�`�t�B���^���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );

	return;
}



/* -------- Fn206  : Easy FFT Parameter Backup	-------- */
static void	easyFftSetPrmBackup( EASYFFT *EasyFFT )
{
	/* ���U���o���g���̉��� */
	EasyFFT->var.OrgFrqLowerLmt = EasyFFT->var.FrqLowerLmt;
	/* �|�����g���̍ŏ��l�̃o�b�N�A�b�v */
	EasyFFT->var.OrgFrqSwpMin = EasyFFT->var.FrqSwpMin;
	/* �|�����g���̍ő�l�̃o�b�N�A�b�v */
	EasyFFT->var.OrgFrqSwpMax = EasyFFT->var.FrqSwpMax;
	return;
}



/* -------- Fn206  : Easy FFT Undo Backuped Parameter	-------- */
static void	easyFftUndoPrmBackup( EASYFFT *EasyFFT )
{
	EasyFFT->var.FrqLowerLmt = EasyFFT->var.OrgFrqLowerLmt;	/* ���U���o���g���̉���					*/
	EasyFFT->var.FrqSwpMin = EasyFFT->var.OrgFrqSwpMin;		/* �|�����g���̍ŏ��l					*/
	EasyFFT->var.FrqSwpMax = EasyFFT->var.OrgFrqSwpMax;		/* �|�����g���̍ő�l					*/
	return;
}



/* -------- Fn206  : Easy FFT Set Trace Registers	-------- */
static void	easyFftSetTraceRegister( TRCREG *TrcReg, LONG ax, USHORT ezfftsw )
{
	LONG	AxisSel;

	/* �g���[�X�Ώێ��I��ݒ� */
//	AxisSel = ax << 8; /* <S1C8> */
	AxisSel = ax;

	TrcReg->TrcMode = 0x08;									/* �g���[�X���[�h						*/
	TrcReg->DataSize = 1;									/* �g���[�X�f�[�^�T�C�Y					*/
//<S153>	TrcReg->AreaSize = EZFFT_SIZ_TRACE;				/* �g���[�X�G���A�T�C�Y					*/
	TrcReg->TrcBuffSize = EZFFT_SIZ_TRACE;					/* <S153> �g���[�X�G���A�T�C�Y			*/
	TrcReg->SampIntv = 1;									/* �g���[�X�T���v�����O�Ԋu				*/

/* <S0C9>	Start */
	/* �g���[�X�ΏۑI�� */
//	TrcReg->TrcSel[0] = (USHORT)(AxisSel | 0x01);			/* �g���N/���͎w��						*/
//	TrcReg->TrcSel[2] = (USHORT)(AxisSel | 0x00);			/* �g���[�X����							*/
//	TrcReg->TrcSel[3] = (USHORT)(AxisSel | 0x00);			/* �g���[�X����							*/

//	switch( ezfftsw & 0x000F )
//	{
//	case 1:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x1E);		/* �O���O�g���N�w��						*/
//		break;

//	case 2:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x01);		/* �g���N/���͎w��						*/
//		break;

//	case 3:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x42);		/* ���׈ʒu����							*/
//		break;

//	case 0:
//	default:
//		TrcReg->TrcSel[1] = (USHORT)(AxisSel | 0x40);		/* �ʒu����FB����						*/
//		break;
//	}

//	/* IO�g���[�X�ΏۑI��(IO�g���[�X�Ȃ�) */
//	TrcReg->IOTrcSel[0] = 0;
//	TrcReg->IOTrcSel[1] = 0;
//	TrcReg->IOTrcSel[2] = 0;
//	TrcReg->IOTrcSel[3] = 0;

	/* �g���[�X���I�� */
	TrcReg->TrcAxisSel[0] = AxisSel;		/* �g���N/���͎w��						*/
	TrcReg->TrcAxisSel[2] = AxisSel;		/* �g���[�X����							*/
	TrcReg->TrcAxisSel[3] = AxisSel;		/* �g���[�X����							*/

	/* �g���[�X�ΏۑI�� */
	TrcReg->TrcDataSel[0] = 0x01;			/* �g���N/���͎w��						*/
	TrcReg->TrcDataSel[2] = 0x00;			/* �g���[�X����							*/
	TrcReg->TrcDataSel[3] = 0x00;			/* �g���[�X����							*/

	switch( ezfftsw & 0x000F )
	{
	case 1:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* �O���O�g���N�w��						*/
		TrcReg->TrcDataSel[1] = 0x1E;		/* �O���O�g���N�w��						*/
		break;

	case 2:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* �g���N/���͎w��						*/
		TrcReg->TrcDataSel[1] = 0x01;		/* �g���N/���͎w��						*/
		break;

	case 3:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* ���׈ʒu����							*/
		TrcReg->TrcDataSel[1] = 0x42;		/* ���׈ʒu����							*/

		break;

	case 0:
	default:
		TrcReg->TrcAxisSel[1] = AxisSel;	/* �ʒu����FB����						*/
		TrcReg->TrcDataSel[1] = 0x40;		/* �ʒu����FB����						*/
		break;
	}

	/* IO�g���[�X�ΏۑI��(IO�g���[�X�Ȃ�) */
	TrcReg->IOTrcAxisSel[0] = 0;
	TrcReg->IOTrcDataSel[0] = 0;

	TrcReg->IOTrcAxisSel[1] = 0;
	TrcReg->IOTrcDataSel[1] = 0;

	TrcReg->IOTrcAxisSel[2] = 0;
	TrcReg->IOTrcDataSel[2] = 0;

	TrcReg->IOTrcAxisSel[3] = 0;
	TrcReg->IOTrcDataSel[3] = 0;
/* <S0C9>	End */

	TrcReg->PtrgDnum = 0;									/* �v���g���K�f�[�^��					*/
	TrcReg->TrgSel = 0;										/* �g���K����							*/
	TrcReg->MTrgMode = 0;									/* �}���`�g���K���[�h					*/

}


/* -------- Fn206  : Easy FFT Table Set Drive Registers	-------- */
static void	easyFftSetTblCmdRegister( TBLDRIVE *TblDrive )
{
	TblDrive->OpeReg.TblMode = 0x05;						/* �e�[�u���^�]���䃂�[�h				*/
	TblDrive->OpeReg.TblLimit = 1;							/* �e�[�u���^�]��]�ʃ��~�b�g			*/
	TblDrive->OpeReg.TblFuncSel = 0x0002;					/* �e�[�u���^�]�@�\�I��					*/
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
	ULONG		DevStep;	/* Step�o���c�L */
	USHORT		DevDir;		/* ���t�o���c�L */
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

	case EZFFT_LSEQ_WAITSTOP:	/* �^�]���g���[�X���� */
		switch( TblDrive->OpeReg.TblState )
		{
			case TBLCMD_STS_POSLMT:
			case TBLCMD_STS_BB:
			case TBLCMD_STS_OT:
			case TBLCMD_STS_POWEROFF:
			case TBLCMD_STS_HWBB:
			case TBLCMD_STS_ALARM:
				/* �e�[�u���^�]�ُ�I��	*/
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

	case EZFFT_LSEQ_DETFRQ:	/* ���g�����o */
		if( EasyFFT->conf.UseHpFil )
		{
			/* HighPassFilter�v�Z */
			easyFftCalcHpFilter( EasyFFT, TrcReg );
		}
		/* ���g�����Z */
		easyFftGetFreqGainMax( EasyFFT, TrcReg );

		/* ���o�l�����������A�ݒ�ő�l�Ɠ����Ȃ��0�Ƃ��Č��o�G���[ */
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
		/*	== [���f] ==																*/
		/*		�I�[�v�����[�vor���[�v�̃}�j���A������Ȃ�A����E�ُ�Ɋւ�炸�I���B*/
		/*		����ȊO�i���[�v�E�����j�Ȃ�											*/
		/*			P���g���C�ȓ��Ȃ�Ē��킷��B										*/
		/*			P���g���C�𒴂��Ă���΁A											*/
		/*				���t�]���ŁA���̃`�����X������΁A���̕�����]�ɐi�ށB			*/
		/*				���̃`�����X�������A�O��OK������Όp��							*/
		/*				��L�ȊO�Ȃ�ُ�I��											*/
		/********************************************************************************/
		EasyFFT->var.FinalDetFrq = EasyFFT->var.DetFrq0;

		/* �}�j���A��(�I�[�v�����[�v�A�g���N����)�A���g���C�O��A�Б��^�]�̂� */
		/* �܂��́A�}�j���A��(�ʒu����)�A���g���C�O��A�Б��^�]�̂� */
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
				{/* ���g���C�L�薳�� */
					/* �ُ펞�A�������g���C�ȓ��Ȃ�A�ăg���CP++ */
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
					/* ���g���C�𒴂����Ƃ�*/
					EasyFFT->var.DetState[ EasyFFT->var.DetCount ] = EZFFT_STS_DETNG;
					EasyFFT->var.DetFrq[ EasyFFT->var.DetCount ] = EasyFFT->var.DetFrq0;
					EasyFFT->var.DetCount++;

					/* ������/�t�̕Е����ʖڂł��t�̃`�����X���L��΁A�u�����v���s����B*/
					if( EasyFFT->var.PosDrvCount == EasyFFT->var.NegDrvCount )
					{
						/* ����ُ͈킾���ǁA�O���OK�̂Ƃ� */
						if( EasyFFT->var.DetState0 == EZFFT_STS_END )
						{
							EasyFFT->var.DetState0 = EZFFT_STS_ERR;
							EasyFFT->var.SeqPtr = EZFFT_LSEQ_STPEND;
						}
						else
						{
						/* ������A�O���NG�̂Ƃ� */
							EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
							EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
						}
					}
					else
					{
						EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
						EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;/* �u�����v���s *//* �i�� */
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
					/* �����������ω�*/
					if( EasyFFT->var.DetState0 == EZFFT_STS_END )
					{
						/* ������O�������*/
						EasyFFT->var.SeqPtr = EZFFT_LSEQ_AVRFRQ;	/* ��������Ɋ���*/
					}
					else
					{
						/* ����͐��킾���A�O��ُ͈� */
						EasyFFT->var.DetState0 = EZFFT_STS_ERR;/* ���̌��ʂł͂Ȃ��B�ŏI����2*/
						EasyFFT->var.SeqPtr = EZFFT_LSEQ_STPEND;/* �Б��Ō���*/
					}
				}
				else
				{/* �Б�����*/
					EasyFFT->var.DetState0 = EZFFT_STS_END;/* =1 ���͐��� */
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;/* �ēx��]�֐i�� */
				}
			}
		}
		break;

	case EZFFT_LSEQ_AVRFRQ:	/* ���t�̕��ω� : STEP�ɂ�炸���t�����ɐ���I�������ꍇ */
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
		{/* �o���c�L�ُ� */
			/* ���t�̃o���c�L���傫���Ƃ� */
			if( EasyFFT->var.AvrReTryCnt < EasyFFT->conf.ReTryMaxCount )
			{
				/* ���g���C */
				EasyFFT->var.DetCount = 2 * EasyFFT->var.Step;
				EasyFFT->var.DetReTryCnt = 0;
				EasyFFT->var.AvrReTryCnt++;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_DRV;
			}
			else
			{
				if( EasyFFT->var.Step == EZFFT_STEP_2 )
				{
					/* Zoom��ُ͂̈� *//* ���[�j���O */
					EasyFFT->var.DetState0 = EZFFT_STS_ERR;
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_WARN;
				}
				else
				{
					/* 1�i�K�ڂňُ� */
					EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
					EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
				}
			}
		}
		else
		{/* �o���c�L���������Ƃ� */
			/* ���肵�� ��ɐi�� */
			EasyFFT->var.DetState0 = EZFFT_STS_END;
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_STPEND;
		}
		break;

	case EZFFT_LSEQ_STPEND:/* STEP���� */
		switch( EasyFFT->conf.ReTryMethod )
		{
		case EZFFT_ONESTEP_RETRY_0:
			/* =6 ����Ŋ��� 5,4�͂��̑O�Ɋ��� */
			EasyFFT->var.FinalDetFrq = EasyFFT->var.DetAvrFrq[ EasyFFT->var.Step ];
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_DONE;/* ����ŏI�� */
			break;

		case EZFFT_ONESTEP_RETRY_1:
			/* Zoom�͍s��Ȃ��BRetry��1��܂� */
			if( (EasyFFT->var.DetState0 != EZFFT_STS_END) && (EasyFFT->var.StepReTryCnt < EasyFFT->conf.ReTryMaxCount) )
			{
				/* ������x�ŏ����� */
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
				/* STEP-1���� */
				EasyFFT->var.AvrReTryCnt = 0;
				EasyFFT->var.DetReTryCnt = 0;
				EasyFFT->var.DetState0 = EZFFT_STS_RUN;
				EasyFFT->var.Step = EZFFT_STEP_2;
				EasyFFT->var.DetCount = 2 * EasyFFT->var.Step;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ZOOMREQ;	/* STEP-1�����@Zoom��͂܂Ŕ�� */
			}
			else
			{/* STEP-2�܂Ŋ��� */
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_AVRSTP;
			}
			break;
		}
		break;

	case EZFFT_LSEQ_AVRSTP:		/* 1���2��̕��ω� */
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
			/* �����Ɏ��� */
			EasyFFT->var.StepReTryCnt = 0;
			EasyFFT->var.DetState0 = EZFFT_STS_END;
			EasyFFT->var.SeqPtr = EZFFT_LSEQ_DONE;
		}
		else
		{/* �o���c�L�����e�l���傫���ꍇ */
			if( EasyFFT->conf.ReTryMaxCount > EasyFFT->var.StepReTryCnt )
			{
				/* �������ŏ�����g���C */
				if( EasyFFT->var.Zoom == TRUE )
				{ /* Zoom��͐ݒ�O�ɖ߂� */
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
			{ /* ���g���C�� */
				EasyFFT->var.DetState0 = EZFFT_STS_DETNG;
				EasyFFT->var.SeqPtr = EZFFT_LSEQ_ERR;
			}
		}
		break;

	case EZFFT_LSEQ_DONE:	/* �I��				*/
	case EZFFT_LSEQ_ERR:	/* �ُ�I��			*/
	case EZFFT_LSEQ_WARN:	/* ���[�j���O�I��	*/
		rc = PRM_RSLT_SUCCESS;
		EasyFFT->OpeReg.EzFftState = EasyFFT->var.DetState0;
		/* Hz���Z 2^22 * 125(EZFFT_RETURN_UNIT) = 1F400000(max) */
		EasyFFT->OpeReg.EzFftDetf =
			(USHORT)((EasyFFT->var.FinalDetFrq * EZFFT_RETURN_UNIT) >> EasyFFT->conf.RetUnitShift);
		break;

	case EZFFT_LSEQ_ZOOMREQ: /* Zoom�̍Đݒ� */
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
/*		�`���[�v�w�ߍ쐬����																		*/
/*																									*/
/****************************************************************************************************/
/*	�T�v�FChirp�w�߂��쐬���A�w�߃e�[�u���ɒu��														*/
/****************************************************************************************************/
static void		easyFftMakeChirpRefTrace( EASYFFT *EasyFFT )
{
	ULONG	work;
	LONG	theta;
	USHORT	i;

	for( i = 0; i < EZFFT_SIZ_TRACE; i++ )
	{
		/* �`���[�v�w�ߎ��g���擾 */
		work = easyFftGetChirpFreq( EasyFFT, i, EZFFT_ANAOBJ_REF );
		/* 2pi���܂ޒP�ʕϊ�, 360[deg]/1024 */
		theta = ((LONG)((work >> 7) * i) & 0x0000FFFF) >> 6;
		/* �e�[�u���w�ߓo�^(�|���g���N���Z�Q�C�����Z) */
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
/*		�`���[�v���g���擾����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v�F�|�����̂���_�̎��g�������߂�B�i�w�ߍ쐬�A���g������̗����ŗp����j					*/
/*																									*/
/*	�����F	LONG	ObjFlag			0 : �w�ߍ쐬�p													*/
/*									1 : ���g�����o�p												*/
/*			USHORT	count			�g���[�X�|�C���g�w��											*/
/*																									*/
/****************************************************************************************************/
static ULONG	easyFftGetChirpFreq( EASYFFT *EasyFFT, USHORT count, LONG ObjFlag )
{
	ULONG	work;
	LONG	lwork;

	if( ObjFlag == EZFFT_ANAOBJ_RES )
	{ /* ���g�����o */
		work = (ULONG)(EasyFFT->var.DeltaFrqRes * count);
	}
	else
	{ /* �w�ߍ쐬 */
		work = (ULONG)(EasyFFT->var.DeltaFrqRef * count);
	}

	/* ���g���P�ʕϊ� */
#if (FLOAT_USE==TRUE)
	lwork = (LONG)EasyFFT->var.FrqSwpMin * EasyFFT->conf.KnormFrq ;
#else
	lwork = MlibMulgain( EasyFFT->var.FrqSwpMin, EasyFFT->conf.KnormFrq );
#endif /* FLOAT_USE */

	if( EasyFFT->var.SwpDir == EZFFT_FRQ_H2L )
	{ /* �g���N�|�������F��ˍ� */
		work = (ULONG)lwork - work;
	}
	else
	{ /* �g���N�|�������F���˒� */
		work = (ULONG)lwork + work;
	}
	return work;
}



/****************************************************************************************************/
/*																									*/
/*		���g�����Z����																				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v�F���xFB or ���g���N�̐�Βl���ő�ƂȂ�l�Ƃ��̎��g�������߂�B							*/
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

		/* �`���[�v���g�����o */
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
/*		�g���[�X�e�[�u�� �n�C�p�X�t�B���^�����O����													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v�F�g���[�X�e�[�u���Ƀn�C�p�X�t�B���^��������B												*/
/*			�m�b�`���o�Ȃǂ̏ꍇ�A�⏞�g���N�̃t�B���^�����O���L���B								*/
/*			���������̃I�t�Z�b�g�A����g�̐U��������\�B											*/
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
/*		���g�����ω�����																			*/
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
/*		���g����͌��ʎ��ӂł̍ĉ�͏�������														*/
/*																									*/
/****************************************************************************************************/
static void		easyFftSetZoomAnalysis( EASYFFT *EasyFFT )
{
	USHORT	uswork;
	USHORT	Freq;

	/* Hz���Z 2^22 * 125(EZFFT_RETURN_UNIT) = 1F400000(max) */
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
/*		EasyFFT�g���N�U���ݒ菈�� (���W�X�^IF)														*/
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
