/****************************************************************************
  Description   : A Type Vibration Tuning (Fn204)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.15   created

****************************************************************************/
#include "FnAtypeVibTune.h"
#include "MercuryGlobals.h"
#include "BaseControls.h"
#include "PnPrmListTbl.h"
#include "KLib.h"
#include "ExControls.h"


/****************************************************************************************************/
/* A Type Vibration Tuning (Fn204)																	*/
/****************************************************************************************************/
/* -------- Local Function Prototype Definition -------- */
static void		atypeVibSetTuningGain( AVIBTUNE *AvibTune, REG_ACC_T *AccessPrm, BOOL Match );
static PRM_RSLT	atypeVibUpdateGainInEeprom( REGIF_CONFIG_T *RegCfg );
static void		atypeVibUndoRamGain( REGIF_CONFIG_T *RegCfg );



/****************************************************************************************************/
/*																									*/
/*		Fn204 : A Type Vibration Tuning																*/
/*																									*/
/****************************************************************************************************/
/* -------- Fn204 A type Vibration Tuning mode Begin	-------- */
PRM_RSLT	FnEnterAtypeVibTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT	rc;
	AVIBTUNE	*AvibTune;
	PRMDATA		*Prm;

	AvibTune = Axis->AvibTune;
	Prm = Axis->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	RpiRegSetAtypeVibTuningFrequency( AvibTune, Prm->avibfrq );
	RpiRegSetAtypeVibTuningGain( AvibTune, Prm->avibdamp );

	if( ((Axis->BaseControls->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ) && (Axis->SeqCtrlOut->BeComplete == TRUE))
		|| (TuneLessGetTuneLessSts(&Axis->BaseControls->TuneLessCtrl) == TRUE) )
	{ /* (�g���N���䎞 && �T�[�{�I����) || �������X�L���� */
		rc = PRM_RSLT_CONDITION_ERR;
	}

	/* ���[�^���X�e�X�g���[�h�̏ꍇ�͓��炸�ɏI�� *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return(PRM_RSLT_CONDITION_ERR);
	}

	AvibTune->OpeReg.AvibState = AVIB_STS_NONCOMP;
	AvibTune->var.VibDetLvl = (DetVibGetVibCheckLevel(Axis->DetVib) >> 4);

	/* Fn���[�h�����I���Ȃ� */
	FunExe->HoldFnMode = TRUE;

	return(rc);
}


/* -------- Fn204 A type Vibration Tuning mode end	-------- */
void	FnLeaveAtypeVibTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	AVIBTUNE		*AvibTune;
	REGIF_CONFIG_T	*RegCfg;

	AvibTune = Axis->AvibTune;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROM�f�[�^�ŕύX�p�����[�^�����ɖ߂� */
	atypeVibUndoRamGain( RegCfg );
	MlibResetLongMemory( &(AvibTune->var), sizeof(AvibTune->var)/4 );

	return;
}


/* -------- Fn204 A type Vibration Tuning mode execution	-------- */
PRM_RSLT	FnExecuteAtypeVibTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	ULONG			data;
	USHORT			VibFreq;
	REG_ACC_T		AccessPrm;
	AVIBTUNE		*AvibTune;
	REGIF_CONFIG_T	*RegCfg;

	AvibTune = Axis->AvibTune;
	RegCfg = Axis->RegManager->hPnReg;
	rc = PRM_RSLT_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* �U�����o�g���[�X�v������ */
	AvibTune->var.FftStat = TuneVibrationTrace( Axis->DetVib,
												Axis->FftAna,
												AvibTune->var.FftReq,
												0,
												TuneLessGetTuneLessSts(&Axis->BaseControls->TuneLessCtrl) );

	switch ( FunExe->State )
	{
	case AVIB_SEQ_INIT:
		FunExe->State = AVIB_SEQ_MODESEL;
		DetVibResetMaxMinSpeedError( Axis->DetVib );
		break;

	case AVIB_SEQ_MODESEL:
		switch ( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			if( AvibTune->OpeReg.AvibMode == TRUE )
			{
				FunExe->State = AVIB_SEQ_SET;
				rc = PRM_RSLT_CONTINUE;
			}
			else
			{
				rc = PRM_RSLT_CONDITION_ERR;
			}
			break;

		case FCMD_EXEC2:
			if( AvibTune->OpeReg.AvibMode == FALSE )
			{
				AvibTune->OpeReg.AvibFreq = 0;
				AvibTune->OpeReg.AvibState = AVIB_STS_NONCOMP;
				FunExe->State = AVIB_SEQ_TRGWAIT;
				KlibRstLongTimer( &AvibTune->var.TimeOutCtr );
				rc = PRM_RSLT_CONTINUE;
			}
			else
			{
				rc = PRM_RSLT_CONDITION_ERR;
			}
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case AVIB_SEQ_TRGWAIT:
		if( DetVibGetSpdErrPeak(Axis->DetVib) > AvibTune->var.VibDetLvl )
		{
			AvibTune->var.FftReq = TRUE;
			FunExe->State = AVIB_SEQ_MEASURE;
		}
		else
		{
			if( KlibGetLongTimerMs( AvibTune->var.TimeOutCtr ) > 8000 )
			{ /* 8sec�o��	*/
				AvibTune->OpeReg.AvibState = AVIB_STS_TIMEOUT;
				FunExe->State = AVIB_SEQ_MODESEL;
			}
		}
		rc = PRM_RSLT_CONTINUE;
		break;

	case AVIB_SEQ_MEASURE:
		if( AvibTune->var.FftStat == TUNE_FREQ_SEARCHEND )
		{
			AvibTune->var.FftReq = FALSE;

			/* �U�����g���擾 */
			VibFreq = FftGetAnaRsltVibmRf( Axis->FftAna );

			if( (VibFreq >= AVIB_FRQ_MIN) && (VibFreq <= AVIB_FRQ_MAX) )
			{
				AvibTune->OpeReg.AvibFreq = VibFreq;
				AvibTune->OpeReg.AvibState = AVIB_STS_COMP;
				FunExe->State = AVIB_SEQ_COMP;
			}
			else
			{
				AvibTune->OpeReg.AvibState = AVIB_STS_RANGEOV;
				FunExe->State = AVIB_SEQ_TRGWAIT;
				KlibRstLongTimer( &AvibTune->var.TimeOutCtr );
			}
		}
		else if( AvibTune->var.FftStat == TUNE_FREQ_SEARCHERR )
		{
			AvibTune->var.FftReq = FALSE;
			AvibTune->OpeReg.AvibState = AVIB_STS_NONVIB;
			FunExe->State = AVIB_SEQ_TRGWAIT;
			KlibRstLongTimer( &AvibTune->var.TimeOutCtr );
		}
		rc = PRM_RSLT_CONTINUE;
		break;

	case AVIB_SEQ_COMP:
		switch ( FunExe->CmdCode )
		{
		case FCMD_EXEC: 
			FunExe->State = AVIB_SEQ_SET;
			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case AVIB_SEQ_SET:
		AvibTune->var.Match = TRUE;
		atypeVibSetTuningGain( AvibTune, &AccessPrm, AvibTune->var.Match );

		/* Pn160 ���U����֘A�X�C�b�` */
//		RpiPrmRead(&pndef_avibsw, &data );
		data = Axis->UniPrms->Prm->avibsw;
		data |= 0x00000001;
//		lrc = RpiPrmWriteRamForFn( &pndef_avibsw, (USHORT)data, pfexe->App.Hmsg );
		TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (USHORT)data, 2 );
		FunExe->State = AVIB_SEQ_GAINTUNE;
		break;

	case AVIB_SEQ_GAINTUNE:
		switch ( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			atypeVibSetTuningGain( AvibTune, &AccessPrm, AvibTune->var.Match );
			break;

		case FCMD_EWRT:
//			atypeVibUpdateGainInEeprom( RegCfg );	
			rc = atypeVibUpdateGainInEeprom( RegCfg );	/* <S085> */
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case AVIB_SEQ_END:	/* �I������ */
		FnLeaveAtypeVibTuning( FunExe, Axis );
		break;

	default:
		break;
	}

	return (rc);
}


/* -------- Fn204 A type Vibration Tuning Gain	-------- */
static void	atypeVibSetTuningGain( AVIBTUNE *AvibTune, REG_ACC_T *AccessPrm, BOOL Match )
{
	if( Match == TRUE )
	{
		/* Pn161 �`�^���U���g�� */
//		rc = RpiPrmWriteRamForFn( &pndef_avibfrq, AvibTune->OpeReg.AvibFreq * 10, Hmsg );
		TunePrmWriteRam( AccessPrm, &pndef_avibfrq, AvibTune->OpeReg.AvibFreq*10, 2 );
		/* Pn163 �`�^���U�_���s���O�Q�C�� */
//		rc = RpiPrmWriteRamForFn( &pndef_avibdamp, AvibTune->OpeReg.AvibGain, Hmsg );
		TunePrmWriteRam( AccessPrm, &pndef_avibdamp, AvibTune->OpeReg.AvibGain, 2 );
	}
}


/* -------- Fn204 Update A type Vibration Tuning Gain In EEPROM	-------- */
static PRM_RSLT	atypeVibUpdateGainInEeprom( REGIF_CONFIG_T *RegCfg )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

//	rc = RpiFunPrmNvmemUpdate( &pndef_avibfrq );						/* Pn161	*/
	/* Pn161 �`�^���U���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfrq, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}
//	rc = RpiFunPrmNvmemUpdate( &pndef_avibgn );							/* Pn162	*/
	/* Pn162 �`�^���U�Q�C���␳ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibgn, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}
//	rc = RpiFunPrmNvmemUpdate( &pndef_avibdamp );						/* Pn163	*/
	/* Pn163 �`�^���U�_���s���O�Q�C�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibdamp, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}
//	rc = RpiFunPrmNvmemUpdate( &pndef_avibfil1 );						/* Pn164	*/
	/* Pn164 �`�^���U�t�B���^���萔1�␳ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}
//	rc = RpiFunPrmNvmemUpdate( &pndef_avibfil2 );						/* Pn165	*/
	/* Pn165 �`�^���U�t�B���^���萔2�␳ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}
//	rc = RpiFunPrmNvmemUpdate( &pndef_avibsw );							/* Pn160	*/
	/* Pn160 ���U����֘A�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}
	return( rc );
}


/* -------- Fn204 Write Gain To Ram With EEPROM Data	-------- */
static void	atypeVibUndoRamGain( REGIF_CONFIG_T *RegCfg )
{
	/* RAM�̒l��EEPROM�̒l�Ɠ����ɂ��� */
	/* Pn161 �`�^���U���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq, 0 );
	/* Pn162 �`�^���U�Q�C���␳ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibgn, 0 );
	/* Pn163 �`�^���U�_���s���O�Q�C�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp, 0 );
	/* Pn164 �`�^���U�t�B���^���萔1�␳ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfil1, 0 );
	/* Pn165 �`�^���U�t�B���^���萔2�␳ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfil2, 0 );
	/* Pn160 ���U����֘A�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibsw, 0 );

	return;
}


/****************************************************************************************************/
/*		A�^���U�@�\�������[�h�ݒ菈�� (���W�X�^IF)													*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetAtypeVibTuningMode( AVIBTUNE *AvibTune, USHORT Mode )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	if( ((SHORT)Mode >= AVIB_MODE_MIN) && (Mode <= AVIB_MODE_MAX) )
	{
		AvibTune->OpeReg.AvibMode = Mode;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}


/****************************************************************************************************/
/*		A�^���U�@�\�������g���ݒ菈�� (���W�X�^IF)													*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetAtypeVibTuningFrequency( AVIBTUNE *AvibTune, USHORT Frq )
{
	PRM_RSLT	rc;
	USHORT		FrqLimit;

	rc = PRM_RSLT_SUCCESS;
	FrqLimit = TunePrmLimitUnsigned( Frq*10, &pndef_avibfrq );

	if( (FrqLimit/10) == Frq )
	{
		AvibTune->OpeReg.AvibFreq = Frq;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}


/****************************************************************************************************/
/*		A�^���U�@�\�����Q�C���ݒ菈�� (���W�X�^IF)													*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetAtypeVibTuningGain( AVIBTUNE *AvibTune, USHORT Gain )
{
	PRM_RSLT	rc;
	USHORT		GainLimit;

	rc = PRM_RSLT_SUCCESS;
	GainLimit = TunePrmLimitUnsigned( Gain, &pndef_avibdamp );

	if( GainLimit == Gain )
	{
		AvibTune->OpeReg.AvibGain = Gain;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}


/****************************************************************************************************/
/*		A�^���U�@�\�������g���ݒ�(Pn161�̒l)���� 													*/
/****************************************************************************************************/
void	RpiRegSetAtypeVibTuningFrequency( AVIBTUNE *AvibTune, USHORT Frq )
{
	AvibTune->OpeReg.AvibFreq = Frq / 10;
	return;
}


/****************************************************************************************************/
/*		A�^���U�@�\�����Q�C���ݒ�(Pn162�̒l)���� 													*/
/****************************************************************************************************/
void	RpiRegSetAtypeVibTuningGain( AVIBTUNE *AvibTune, USHORT Gain )
{
	AvibTune->OpeReg.AvibGain = Gain;
	return;
}



/****************************************************************************************************/
