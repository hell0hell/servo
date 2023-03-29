/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseEnableSequence.c : �x�[�X�C�l�[�u���V�[�P���X�Ǘ�������`								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �x�[�X�C�l�[�u���V�[�P���X�Ǘ�����														*/
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
/*	Note	:	����	2011.02.03	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "KLib.h"						/* <S053> */
#include "BaseEnableSequence.h"
#include "PowerManager.h"


#define CHARGE_PUMP_CMP_TIME 0x0080		/* �`���[�W�|���v�������ԁF125[us]*128 =16[ms]	*//* <S094> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
void	CpxZeroClampSequence( 						/* @@Zero Clamp Sequence 						*/
	ZC_CTRL_DATA *Zcv,								/* Zero Clamp Control							*/
	BASE_CTRL_OUT	*Bout,							/* Base Control Output 							*/
	FUN_CMN_CONTROL *Fctrl,							/* Function Control								*/
	SEQ_CTRL_OUT *Cout	);							/* Sequence Output 								*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxCheckServoOnCmdErr( 					/* <S0A7> Check Servo On Cmd Error (A.0B0)		*/
	BE_SEQ_HNDL 	*BeSeq, 						/* 												*/
	BOOL			ServoOnReq,						/* Servo On Command								*/
	FUN_CMN_CONTROL *FnCmnCtrl, 					/* 												*/
	CTRLMD 			CtrlMode );						/* Control Mode									*/

/****************************************************************************************************/
/*																									*/
/*		�T�[�{���f�B�M���ݒ菈��																	*/
/*																									*/
/****************************************************************************************************/
void BeSeqMakeServoReadySignal( BE_SEQ_HNDL *BeSeq, BOOL ready_cond, LONG PhaseReady )
{
	if(BeSeq->SvonEnable && ready_cond)
	{
		/* Servo Ready flag set (Ignore Phase) */
		BeSeq->SvReadyPhaseIgnore = TRUE;
		/* Servo Ready flag set */
		if( PhaseReady )
		{
			BeSeq->ServoReady = TRUE;
		}
		else
		{
			BeSeq->ServoReady = FALSE;
		}
	}
	else
	{
		/* Servo Ready flag set (Ignore Phase) */
		BeSeq->SvReadyPhaseIgnore = FALSE;
		/* Servo Ready flag set */
		BeSeq->ServoReady = FALSE;
	}
}



/****************************************************************************************************/
/*																									*/
/*		�Z���T�I��/�T�[�{�I���v���ݒ菈��															*/
/*																									*/
/****************************************************************************************************/
ALMID_T	BeSeqMakeServoOnRequest( BE_SEQ_HNDL *BeSeq, LONG CmdSeqBit,
       	                          CTRLMD CtrlMode, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV )
{
	ALMID_T svon_err;
	BOOL SensOnReq, LastSensOnReq;
	BOOL ServoOnReq,PdetReq;

	svon_err = NO_ALARM;

	if(FnCmnCtrl->KnlMode != KPI_KMD_NORMAL)
	{/* Test mode */
		BeSeq->SvonReq = FALSE;
		SensOnReq = (0 != (CmdSeqBit & SENSOR_ON_BIT));
		FnCmnCtrl->State.FnSvCtrlDisable = FALSE;
	}
	else
	{/* Normal mode */
		if(FnCmnCtrl->FnSvControl != FALSE)
		{/* Fn mode�ɂ��^�] */
			ServoOnReq = FnCmnCtrl->FnSvonReq;
			SensOnReq = (0 != (CmdSeqBit & SENSOR_ON_BIT));
			SensOnReq |= FnCmnCtrl->FnSensOnReq;
//			PdetReq = FnCmnCtrl->PdetReq;
			PdetReq = FnCmnCtrl->SvOnPdetReq;					/* <S21C> */

			FnCmnCtrl->State.LockNormalOpeDrive = TRUE;
		}
		else
		{/* �ʏ�w�߂ɂ��^�] */
			ServoOnReq = (0 != (CmdSeqBit & SERVO_ON_BIT));
			SensOnReq = (0 != (CmdSeqBit & SENSOR_ON_BIT));
			PdetReq = (0 != (CmdSeqBit & POL_DETECT_BIT));

			/* �T�[�{ON�w�߃`�F�b�N */
			if( LpxCheckServoOnCmdErr( BeSeq, ServoOnReq, FnCmnCtrl, CtrlMode ) )	/* <S0A7> */
			{
				svon_err = ALM_SVONCMD;		/* A.0B0 : SGDV�Ǝ኱�������Ⴄ�̒���!! */
				ServoOnReq = FALSE;
			}
#if 0		/* <S0A7> */
			if(FnCmnCtrl->State.LockNormalOpeDrive != FALSE)
			{/* �ʏ�^�]�֎~*/
				if( (CtrlMode.us.uwd_l == CTRLMODE_SPD) || (CtrlMode.us.uwd_l == CTRLMODE_TRQ) )
				{
					BeSeq->SenStatus = FALSE;
					FnCmnCtrl->State.LockNormalOpeDrive = FALSE;
				}
				else if( (ServoOnReq != FALSE) && (CtrlMode.us.uwd_l == CTRLMODE_POS) )
				{/* �T�[�{�I���w�߂��� & �ʒu���䃂�[�h�w�� */
					svon_err = ALM_SVONCMD;	/* A.0B0 : SGDV�Ǝ኱�������Ⴄ�̒���!! */
					ServoOnReq = FALSE;
				}
				else
				{	/* do nothing. */
					;
				}
			}
#endif
		}

		if( MencV->PhaseReady != FALSE)
		{
			BeSeq->SvonReq = ServoOnReq;
			FnCmnCtrl->State.FnSvCtrlDisable = ServoOnReq;
		}
		else
		{
			BeSeq->SvonReq = PdetReq;
			FnCmnCtrl->State.FnSvCtrlDisable = ServoOnReq | PdetReq;
			if((ServoOnReq != FALSE) /*&& (FnCmnCtrl->PdetReq == FALSE)*/ )
			{
				svon_err = ALM_PDET;	/* A.C52 */
			}
		}
	}

	/* SENSER_ON�v���擾 */
	LastSensOnReq = BeSeq->SenStatus;
	if( (MencV->AbsoEncType != MENC_INCRE_TYPE) && (MencV->AxisMotType == MOTTYPE_ROTARY) )
	{ /* ��Βl�G���R�[�_�g�p�� */
		if( (LastSensOnReq == FALSE) && (SensOnReq != FALSE) )
		{ /* SEN_ON���� */
#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
			if( MencV->DivInfo.DivOut == FALSE )
			{
				MencV->ReCalcAbsPosReq = TRUE;
			}
			else
			{
				;/*�������Ȃ�(�V���A���G���R�[�_�o�`�n���V���A���o�͂ɂāAMencV->ReCalcAbsPosReq = TRUE�Ƃ��� */
			}
#else/*<S02D>*/
			MencV->ReCalcAbsPosReq = TRUE;
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/
		}
	}

	/* ������ԍX�V */
	BeSeq->SenStatus = SensOnReq /* <S082> & MencV->EncReady */;

	return svon_err;
}


#if 0 /* PcmRlyDbControlProcess()�ɓ��� */
/****************************************************************************************************/
/*																									*/
/*		�c�a��H����																				*/
/*																									*/
/****************************************************************************************************/
void BeSeqDBControlProcess( BOOL DbOn, LONG RlyDbWay, USHORT faultState, ASIC_HW_IF_STR *pAsicHwReg,
							BOOL MotorLessTestMode )													/* <S1B7> */
{

	switch( RlyDbWay )
	{
	case DBCTRL_RLY_COMMON:				/* �˓��{�c�a�F�����[���� 								*/
		/* PcmRlyControlProcess()�ɂĐ��䂷�� */
		break;

	case DBCTRL_TRIACK_SEPARATE:		/* �˓��F�����[�G�c�a�F�T�C���X�^ 						*/
	case DBCTRL_RLY_SEPARATE:			/* �˓��F�����[�G�c�a�F�����[ 							*/
		if( ((faultState & SHAL_FAULT_BB) != 0) && (DbOn == TRUE) )
		{
			/* DBON */
			SHalDrv_DynamicBrakeON(pAsicHwReg);
		}
		else
		{
			/* ���[�^���X�e�X�g���[�h�łȂ����OFF���� *//* <S1B7> */
			/* TODO: SGD-V�ł͈ȉ��̏�����OR���Ă���							*/
			/* (1) AxisRscC->BeSequence->StopMode == STOPMODE_DBSTOPFR			*/
			/* (2) AxisRscC->BeSequence->StopMode == STOPMODE_FREERUN			*/
			/* (3) AxisRscC->FnCmnCtrl->KnlMode == KPI_KMD_ADJCURAD				*/
			if( MotorLessTestMode == FALSE )
			{
				/* DBOFF */
				SHalDrv_DynamicBrakeOFF(pAsicHwReg);
			}
		}
		break;

	case DBCTRL_TRIACK_COMMON:			/* �˓��{�c�a�F�T�C���X�^ 								*/
		/* ���Ή� */
		break;

	default:
		break;
	}

}
#endif


/****************************************************************************************************/
/*																									*/
/*		�X�L�����b���䃂�[�h�w��																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �e�V�[�P���X����̗v�����󂯂āA�D��x�Ɋ�Â����䃂�[�h�����肵�A�X�L�����a��			*/
/*			�΂��āA���䃂�[�h���w�߂���B															*/
/*			�Ȃ��A�ŏI�̃T�[�{���䃂�[�h�́A�X�L�����a�Ō��肳���(BoutV.CtrlMode)�B				*/
/*																									*/
/****************************************************************************************************/
ULONG BeSeqControlModeManager( BE_SEQ_HNDL *BeSeq, FUN_CMN_CONTROL *FnCmnCtrl )
{
	CTRLMD	CtrlMcmd;

	if(FnCmnCtrl->FnSvControl != FALSE)
	{/* Fn mode�ɂ��^�] */
		/* ScanC Control Mode <= ROUND�v�����[�h */
		CtrlMcmd.dw = (ULONG)FnCmnCtrl->FnCtrlMcmd;
	}
	else
	{	/* ScanC Control Mode <= NO CMD */
		CtrlMcmd.dw = CTRLMODE_NOCMD;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ScanC�F�[�����䃂�[�h�w��																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( BeSeq->StopMode == STOPMODE_ZEROSTOP )			/* �X�g�b�v���[�h�Ǘ�						*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZSTOP;					/* �[������~���[�h							*/
	}
	/*----------------------------------------------------------------------------------------------*/
/* <S1F5> Start */
	else if( BeSeq->StopMode == STOPMODE_LDECSTOP )		/* �X�g�b�v���[�h�Ǘ�						*/
	{
		CtrlMcmd.b.zm = CTRLMODE_LDSTOP;				/* ���������x�ɂ�錸����~���[�h			*/
	}
	/*----------------------------------------------------------------------------------------------*/
/* <S1F5> End */
	else if( BeSeq->OtSeqData.var.OtZeroClamp )			/* OT�V�[�P���X								*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZCLMP;					/* �[���N�����v���[�h						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( BeSeq->ZcCtrlData.ZcZeroClamp )			/* �[���N�����v�V�[�P���X					*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZCLMP;					/* �[���N�����v���[�h						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else												/* �f�t�H���g����							*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZNONE;					/* �[������Ȃ�								*/
	}

	return CtrlMcmd.dw;
}



/****************************************************************************************************/
/*																									*/
/*		�A���[����~���[�h�ݒ菈��																	*/
/*																									*/
/****************************************************************************************************/
void BeSeqInputAlmStopMode( BE_SEQ_HNDL *BeSeq, LONG AlmStopMode,   /* <S1F5> */
                            CTRLMD CtrlMode, BOOL PhaseReady, SEQ_CTRL_OUT *Cout )  /* <S1F5> */
// void BeSeqInputAlmStopMode( ALM_STOP *AlmStop, LONG AlmStopMode, /* <S1F5> */
//                           CTRLMD CtrlMode, BOOL PhaseReady, BOOL HwbbSts, BOOL MotStop )  /* <S1F5> */
{
/* <S1F5> Start */
	ALM_STOP *AlmStop;
	BOOL HwbbSts;
	BOOL MotStop;

	/* ���[�J���ϐ��̏����� */
	AlmStop = &(BeSeq->AlmStop);
	HwbbSts = Cout->HwbbSts;
	MotStop = Cout->MotStop;
/* <S1F5> End */
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���X�g�b�v���[�h�ݒ菈��																*/
/*--------------------------------------------------------------------------------------------------*/
	if( AlmStopMode == STOPMODE_ZEROSTOP )						/* �A���[���[������~�H				*/
	{
		if( (CtrlMode.b.cm == CTRLMODE_TRQ) ||					/* �g���N���䒆�H					*/
//			(AlmStop->conf.G2AlmStopMode == TRUE) ||			/* G2ALM��~���@��Pn001.0�ɏ]��		*/  /* <S1F5> */
			(AlmStop->conf.G2AlmStopMode == GR2STOPMODE_DEFAULT) ||	/* G2ALM��~���@��Pn001.0�ɏ]��	*/  /* <S1F5> */
			(HwbbSts == TRUE) ||								/* HWBB��Ԓ��H						*/
			(PhaseReady == FALSE) )								/* ���Ɍ��o��?						*/
		{
			/* Pn001.0�̒�~���@�ɕύX */
			AlmStopMode = AlmStop->conf.StopMode;
		}
/* <S1F5> Start */
		else
		{	/* G2ALM��~���@	*/
			AlmStopMode = AlmStop->conf.G2AlmStopMode;
		}
/* <S1F5> End */
	}
	AlmStop->var.AlmStopMode = AlmStopMode;
#if 0	/* <S1F5> Start	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���F�[������~�v���M��																*/
/*--------------------------------------------------------------------------------------------------*/
	if( AlmStopMode == STOPMODE_ZEROSTOP )
	{ /* �ُ��~���[�h : �[������~ */
		if( MotStop )
		{ /* ���[�^��~�� */
			/* �[������~�����t���OON */
			AlmStop->var.AlmZstopFinish = TRUE;
		}

		if( AlmStop->var.AlmZstopFinish == FALSE )
		{
			/* �[������~�v���t���OON */
			AlmStop->var.AlmZeroStop = TRUE;
		}
		else
		{
			/* �[������~�v���t���OOFF */
			AlmStop->var.AlmZeroStop = FALSE;
		}
	}
	else
	{ /* �ُ��~���[�h : �[������~�ȊO */
		AlmStop->var.AlmZeroStop = FALSE;
		AlmStop->var.AlmZstopFinish = FALSE;
	}
#endif	/* <S1F5> End	*/
/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		�A���[���X�g�b�v�V�[�P���X																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( BeSeq->LdstpTimeZero == TRUE )
	{	/* Pn30A = 0�Ƃ����ꍇ�A�[������~�ƂȂ�B�������APn406�ɂ��g���N�����͗L���B	*/
		if( AlmStop->var.AlmStopMode == GR2STOPMODE_LDSTOPDB )
		{
			AlmStop->var.AlmStopMode = GR2STOPMODE_ZSTOPDB;
		}
		else if( AlmStop->var.AlmStopMode == GR2STOPMODE_LDSTOPFR )
		{
			AlmStop->var.AlmStopMode = GR2STOPMODE_ZSTOPFR;
		}
		else
		{/* do nothing */
			;
		}
	}
	
	switch( AlmStop->var.Gr2StopStep )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:	/* Initial Step																			*/
/*--------------------------------------------------------------------------------------------------*/
		AlmStop->var.Gr2TrqLimit = FALSE;
		AlmStop->var.Gr2ZeroStop = FALSE;
		AlmStop->var.Gr2LinDecStop = FALSE;
		AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
		AlmStop->var.Gr2StopStep = 1;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:	/* Alm��~���[�h�ɂ�镪��																*/
/*--------------------------------------------------------------------------------------------------*/
		switch( AlmStop->var.AlmStopMode )
		{
			case GR2STOPMODE_ZSTOPDB:			/* �[������~��DB��~���							*/
			case GR2STOPMODE_ZSTOPFR:			/* �[������~���t���[�������						*/
				AlmStop->var.Gr2TrqLimit = TRUE;
				AlmStop->var.Gr2ZeroStop = TRUE;
				AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
				AlmStop->var.Gr2StopStep = 2;
				break;
			case GR2STOPMODE_LDSTOPDB:			/* Pn30A�ɏ]���ă��[�^��������~��DB��~���		*/
			case GR2STOPMODE_LDSTOPFR:			/* Pn30A�ɏ]���ă��[�^��������~���t���[�������	*/
				AlmStop->var.Gr2LinDecStop = TRUE;
				AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
				AlmStop->var.Gr2StopStep = 3;
				break;
			default:
				break;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:	/* �[������~���A�A���[���X�g�b�v�����^���[�^��~�҂�									*/
/*--------------------------------------------------------------------------------------------------*/
		if( (AlmStop->var.AlmStopMode != GR2STOPMODE_ZSTOPDB)
		 && (AlmStop->var.AlmStopMode != GR2STOPMODE_ZSTOPFR) )
		{
			AlmStop->var.Gr2TrqLimit = FALSE;
			AlmStop->var.Gr2ZeroStop = FALSE;
			AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
			AlmStop->var.Gr2StopStep = 1;
		}
		else if( MotStop )
		{
			AlmStop->var.Gr2ZeroStop = FALSE;
			if( AlmStop->var.AlmStopMode == GR2STOPMODE_ZSTOPFR )
			{
				AlmStop->var.Gr2DbopReq = GR2DBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:	/* ����������~���A�A���[���X�g�b�v�����^���[�^��~�҂�									*/
/*--------------------------------------------------------------------------------------------------*/
		if( (AlmStop->var.AlmStopMode != GR2STOPMODE_LDSTOPDB)
		 && (AlmStop->var.AlmStopMode != GR2STOPMODE_LDSTOPFR) )
		{
			AlmStop->var.Gr2LinDecStop = FALSE;
			AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
			AlmStop->var.Gr2StopStep = 1;
		}
		else if( MotStop )
		{
			AlmStop->var.Gr2LinDecStop = FALSE;
			if( AlmStop->var.AlmStopMode == GR2STOPMODE_LDSTOPFR )
			{
				AlmStop->var.Gr2DbopReq = GR2DBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
	default:
		break;
	}
/* <S1F5> End */
}

/* <S1F5> Start */
/****************************************************************************************************/
/*																									*/
/*		������~���[�h�ݒ菈��																		*/
/*																									*/
/****************************************************************************************************/
void BeSeqForceStopMode( BE_SEQ_HNDL *BeSeq, CTRLMD CtrlMode, BOOL MotStop, BOOL FstpSts )
{
	FORCE_STOP *ForceStop;
	ALM_STOP *AlmStop;

	/* ���[�J���ϐ��̏����� */
	ForceStop = &( BeSeq->ForceStop );
	AlmStop = &( BeSeq->AlmStop );

	switch( ForceStop->var.FstpStopStep )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:	/* Initial Step																			*/
/*--------------------------------------------------------------------------------------------------*/
		ForceStop->var.FstpTrqLimit = FALSE;
		ForceStop->var.FstpZeroStop = FALSE;
		ForceStop->var.FstpLinDecStop = FALSE;
		ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
		ForceStop->var.FstpMode = STOPMODE_NOTSTOP;
		ForceStop->var.FstpStopStep = 1;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:	/* ������~�Ď� (������~�łȂ����)													*/
/*--------------------------------------------------------------------------------------------------*/
		if( FstpSts == TRUE )
		{
			if( (CtrlMode.b.cm == CTRLMODE_TRQ)
			 || (ForceStop->conf.ForceStopMode == FSTOPMODE_DEFAULT) )
			{
				ForceStop->var.FstpMode = AlmStop->conf.StopMode;

				break;
			}
			else
			{
				ForceStop->var.FstpMode = ForceStop->conf.ForceStopMode;

				if( BeSeq->LdstpTimeZero == TRUE )
				{	/* Pn30A = 0�Ƃ����ꍇ�A�[������~�ƂȂ�B�������APn406�ɂ��g���N�����͗L���B	*/
					if( ForceStop->var.FstpMode == FSTOPMODE_LDSTOPDB )
					{
						ForceStop->var.FstpMode = FSTOPMODE_ZSTOPDB;
					}
					else if( ForceStop->var.FstpMode == FSTOPMODE_LDSTOPFR )
					{
						ForceStop->var.FstpMode = FSTOPMODE_ZSTOPFR;
					}
					else
					{/* do nothing */
						;
					}
				}
			}
			
			switch( ForceStop->var.FstpMode )
			{
			case FSTOPMODE_ZSTOPDB:
			case FSTOPMODE_ZSTOPFR:
				ForceStop->var.FstpTrqLimit = TRUE;
				ForceStop->var.FstpZeroStop = TRUE;
				ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
				ForceStop->var.FstpStopStep = 2;
				break;
			case FSTOPMODE_LDSTOPDB:
			case FSTOPMODE_LDSTOPFR:
				ForceStop->var.FstpLinDecStop = TRUE;
				ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
				ForceStop->var.FstpStopStep = 3;
				break;
			default:
				break;
			}
		}
		else
		{
			ForceStop->var.FstpMode = STOPMODE_NOTSTOP;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:	/* �[������~���A������~�����^���[�^��~�҂�											*/
/*--------------------------------------------------------------------------------------------------*/
		if( FstpSts == FALSE )
		{
			ForceStop->var.FstpTrqLimit = FALSE;
			ForceStop->var.FstpZeroStop = FALSE;
			ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
			ForceStop->var.FstpStopStep = 1;
		}
		else if( MotStop )
		{
			ForceStop->var.FstpZeroStop = FALSE;
			if( ForceStop->var.FstpMode == FSTOPMODE_ZSTOPFR )		/* <S1FF> */
			{
				ForceStop->var.FstpDbopReq = FSTPDBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:	/* ����������~���A������~�����^���[�^��~�҂�											*/
/*--------------------------------------------------------------------------------------------------*/
		if( FstpSts == FALSE )
		{
			ForceStop->var.FstpLinDecStop = FALSE;
			ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
			ForceStop->var.FstpStopStep = 1;
		}
		else if( MotStop )
		{
			ForceStop->var.FstpLinDecStop = FALSE;
			if( ForceStop->var.FstpMode == FSTOPMODE_LDSTOPFR )		/* <S1FF> */
			{
				ForceStop->var.FstpDbopReq = FSTPDBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
	default:
		break;
	}
}
/* <S1F5> End */

/****************************************************************************************************/
/*																									*/
/*		���J�j�J���u���[�L�V�[�P���X(�a�j�V�[�P���X)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �u���[�L���[�^���g���ꍇ�̃u���[�L�w�ߏ�������тc�a�V�[�P���X�ɑ΂���x�[�X			*/
/*			�C�l�[�u���w�ߏ������s���B																*/
/*			�u���[�L�I�����̃T�[�{�I�t�^�C�~���O�A���[�^��~���̃u���[�L�^�C�~���O�ݒ��			*/
/*			���[�U�[�萔�ɂ���]���A���Ԃ�ݒ�ł���B											*/
/*			OT���ُ̈폈���������āA�T�[�{�^�]�̃x�[�X�C�l�[�u���^�x�[�X�u���b�N�w�߂�				*/
/*			�{�V�[�P���X����w�߂����B															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL BeSeqMechaBrakeSequence( BE_SEQ_HNDL *BeSeq, CTRLMD CtrlMode,
    	                       LONG AbsMotSpd, BOOL HwbbStatus, USHORT faultState,
    						   BOOL BeComplete, BOOL MotorLessTestMode)								/* <S1B7> */
{
	BK_SEQ_DATA 	*BkSeqData;
//	ALM_STOP 		*alm_stop_ptr;  /* <S1F5> */
	ALM_STOP 		*AlmStop;  /* <S1F5> */
	FORCE_STOP		*ForceStop;  /* <S1F5> */

	/* ���[�J���ϐ��̏����� */
	BkSeqData = &BeSeq->BkSeqData;
//	alm_stop_ptr = &BeSeq->AlmStop;  /* <S1F5> */
	AlmStop = &BeSeq->AlmStop;  /* <S1F5> */
	ForceStop = &BeSeq->ForceStop;  /* <S1F5> */

/*--------------------------------------------------------------------------------------------------*/
/*		�u���[�L�V�[�P���X����																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( BkSeqData->var.BkStep )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:	/* Initial Step																			*/
/*--------------------------------------------------------------------------------------------------*/
		BkSeqData->var.Brake = TRUE;					/* �u���[�L�w�ߏo��							*/
		BkSeqData->var.BkZeroStop = FALSE;				/* �[������~�v���I�t						*/
		BkSeqData->var.BkLinDecStop = FALSE;			/* ����������~�v���I�t						*/	/* <S1F5> */
		BkSeqData->var.BkWait = 0;
		BkSeqData->var.BkStep = 1;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:	/* �T�[�{�I�t���ŁA�T�[�{�I������̂�҂��Ă�����										*/
/*--------------------------------------------------------------------------------------------------*/
		if( BeSeq->SvonReq && BeSeq->SvReadyPhaseIgnore )
		{
			/* Base Enable Request ON */
			BkSeqData->var.BkBeReq = TRUE;

			/* ���[�^���X�e�X�g���[�h��Ԃɂ�蔻����@��ύX���� *//* <S1B7> */
			if( MotorLessTestMode == FALSE )	/* ���[�^���X���[�h���� */
			{
				if( (faultState & SHAL_FAULT_BB) == 0 )
				{
					/* Brake Release */
					BkSeqData->var.Brake = FALSE;
					BkSeqData->var.BkWait = 0;
					BkSeqData->var.BkStep = 2;
				}
			}
			else								/* ���[�^���X���[�h�L�� */
			{
				if( BeComplete == TRUE )	/* �x�[�X�C�l�[�u�� */
				{
					/* Brake Release */
					BkSeqData->var.Brake = FALSE;
					BkSeqData->var.BkWait = 0;
					BkSeqData->var.BkStep = 2;
				}
			}
		}
		else
		{
			/* Base Enable Request OFF */
			BkSeqData->var.BkBeReq = FALSE;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2: /* �T�[�{�I�����ŁA�T�[�{�I�t�܂��̓T�[�{���f�B�I�t��҂��Ă�����						*/
/*--------------------------------------------------------------------------------------------------*/
		if( BeSeq->SvonReq && BeSeq->SvReadyPhaseIgnore )
		{ /* �T�[�{�I���v�� */
			;											/* �T�[�{�I���v���I�t�҂�					*/
		}
//		else if( alm_stop_ptr->var.AlmZeroStop )  /* <S1F5> */
		else if( (AlmStop->var.Gr2ZeroStop == TRUE)			/* <S1F5> */
			|| (ForceStop->var.FstpZeroStop == TRUE))		/* <S1F5> */
		{ /* �A���[���[������~���A�܂��͋�����~�[������~�� */		/* <S1F5> */
			;											/* �A���[���Ƌ�����~�F�[������~���I�t�҂�	*/   /* <S1FS1F5> */
		}
/* <S1F5> Start */
		else if( (AlmStop->var.Gr2LinDecStop == TRUE)
			|| (ForceStop->var.FstpLinDecStop == TRUE) )
		{
			BkSeqData->var.BkStep = 3;
		}
/* <S1F5> End */
		else if( AbsMotSpd >= BkSeqData->conf.BkonSpdLevel )
		{ /* ���[�^���x���u���[�L�w�߈ȏ� */			/* �o�͑��x(Pn507)�ȏ�Ȃ�					*/
			BkSeqData->var.BkBeReq = FALSE;				/* �u���[�L����������						*/
			BkSeqData->var.BkStep = 4;					/* �x�[�X�C�l�[�u���v���I�t					*/	/* <S1F5> */
		}
//		else if( (alm_stop_ptr->var.AlmStopMode > STOPMODE_ZEROSTOP)  /* <S1F5> */
		else if( (AlmStop->var.AlmStopMode >= STOPMODE_DBSTOPDB)  /* <S1F5> */
			  || (ForceStop->var.FstpMode >= STOPMODE_DBSTOPDB)   /* <S1F5> */
				|| (BkSeqData->conf.BkonDelayTime == 0)
				|| (HwbbStatus == TRUE) )
		{												/* �[������~�ȊO�̃A���[�� or HWBB���		*/
			BkSeqData->var.BkBeReq = FALSE;				/* or �T�[�{�I�t�x�ꎞ�Ԃ��[��				*/
			BkSeqData->var.Brake = TRUE;				/* �x�[�X�C�l�[�u���v���I�t��				*/
			BkSeqData->var.BkStep = 1;					/* �u���[�L�w�߂𓯎��ɏo��					*/
		}
		else											/* �A���[���F���� or �[������~				*/
		{												/* and �T�[�{�I�t�҂����� > 0				*/
			BkSeqData->var.Brake = TRUE;				/* �u���[�L�w�߂��o�͂��A					*/
			if( CtrlMode.b.cm == CTRLMODE_TRQ )			/* �g���N���䎞�́A							*/
			{											/* �x�[�X�u���b�N��҂ԁA����p��			*/
				BkSeqData->var.BkZeroStop = FALSE;
			}
			else
			{											/* �x�[�X�u���b�N��҂ԁA�[������~		*/
				BkSeqData->var.BkZeroStop = TRUE;
			}
			BkSeqData->var.BkStep = 5;			/* <S1F5> */
		}
		break;
/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
	case 3: /* �T�[�{�I�����ŁA�T�[�{�I�t�܂��̓T�[�{���f�B�I�t��҂��Ă�����	<S1F5>					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BeSeq->SvonReq && BeSeq->SvReadyPhaseIgnore )
		{ /* �T�[�{�I���v�� */
			;											/* �T�[�{�I���v���I�t�҂�					*/
		}
		else if( (AlmStop->var.Gr2LinDecStop == TRUE)
			|| (ForceStop->var.FstpLinDecStop == TRUE))
		{ /* ������~����������~�� */
			;										/* �A���[���Ƌ�����~�F����������~���I�t�҂�	*/
		}
		else if( (AlmStop->var.Gr2ZeroStop == TRUE)
			|| (ForceStop->var.FstpZeroStop == TRUE) )
		{
			BkSeqData->var.BkStep = 2;
		}
		else if( AbsMotSpd >= BkSeqData->conf.BkonSpdLevel )
		{ /* ���[�^���x���u���[�L�w�߈ȏ� */			/* �o�͑��x(Pn507)�ȏ�Ȃ�					*/
			BkSeqData->var.BkBeReq = FALSE;				/* �u���[�L����������						*/
			BkSeqData->var.BkStep = 4;					/* �x�[�X�C�l�[�u���v���I�t					*/
		}
		else if( (AlmStop->var.AlmStopMode >= STOPMODE_DBSTOPDB)
				|| (ForceStop->var.FstpMode >= STOPMODE_DBSTOPDB)
				|| (BkSeqData->conf.BkonDelayTime == 0)
				|| (HwbbStatus == TRUE) )
		{												/* �[������~�ȊO�̃A���[�� or HWBB���		*/
			BkSeqData->var.BkBeReq = FALSE;				/* or �T�[�{�I�t�x�ꎞ�Ԃ��[��				*/
			BkSeqData->var.Brake = TRUE;				/* �x�[�X�C�l�[�u���v���I�t��				*/
			BkSeqData->var.BkStep = 1;					/* �u���[�L�w�߂𓯎��ɏo��					*/
		}
		else											/* �A���[���F���� or �[������~				*/
		{												/* and �T�[�{�I�t�҂����� > 0				*/
			BkSeqData->var.Brake = TRUE;				/* �u���[�L�w�߂��o�͂��A					*/
			if( CtrlMode.b.cm == CTRLMODE_TRQ )			/* �g���N���䎞�́A							*/
			{											/* �x�[�X�u���b�N��҂ԁA����p��			*/
				BkSeqData->var.BkLinDecStop = FALSE;
			}
			else
			{										/* �x�[�X�u���b�N��҂ԁA����������~��~		*/
				BkSeqData->var.BkLinDecStop = TRUE;
			}
			BkSeqData->var.BkStep = 5;
		}
		break;
/* <S1F5> End */
/*--------------------------------------------------------------------------------------------------*/
	case 4:	/* ���[�^�^�]���̃T�[�{�I�t�ŁA���[�^���x���Ⴍ�Ȃ�̂�҂��Ă�����					*/   /*  */
/*--------------------------------------------------------------------------------------------------*/
		BkSeqData->var.BkWait += KPI_SCCYCLEMS;
		if( (BkSeqData->var.BkWait >= BkSeqData->conf.BkonWaitTime)
			|| (AbsMotSpd <= BkSeqData->conf.BkonSpdLevel) )
		{												/* �u���[�L�w�ߑ҂����Ԍo�� or				*/
			BkSeqData->var.Brake = TRUE;				/* ���[�^���x���u���[�L�w�ߏo�͑��x			*/
			BkSeqData->var.BkWait = 0;					/* �ȉ��ɂȂ�����							*/
			BkSeqData->var.BkStep = 1;					/* �u���[�L�w�߂��o�͂���					*/
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5: /* ���[�^��~���̃T�[�{�I�t�ŁA�u���[�L��������܂ŁA�a�a��҂��Ă�����				*/   /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
		BkSeqData->var.BkWait += KPI_SCCYCLEMS;
		if( (BkSeqData->var.BkWait >= BkSeqData->conf.BkonDelayTime)
//			|| (alm_stop_ptr->var.AlmStopMode > STOPMODE_ZEROSTOP)   /* <S1F5> */
			|| (HwbbStatus == TRUE) )
		{												/* �T�[�{�I�t�x�ꎞ�Ԍo��					*/
			BkSeqData->var.BkBeReq = FALSE;				/* �[������~�ȊO�̃A���[��������			*/
			BkSeqData->var.BkZeroStop = FALSE;			/* �x�[�X�C�l�[�u���v���I�t					*/
			BkSeqData->var.BkWait = 0;					/* and �[���X�g�b�v�v���I�t					*/
			BkSeqData->var.BkStep = 1;					/*											*/
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default: /*	�{�P�[�X�̓V�[�P���X�\�t�g�ُ�														*/
/*--------------------------------------------------------------------------------------------------*/
		KpiSystemError( );
		break;
	}

	return BkSeqData->var.BkBeReq;
}



/****************************************************************************************************/
/*																									*/
/*		�I�[�o�[�g���x����Ԍ��o����																*/
/*																									*/
/****************************************************************************************************/
BOOL BeSeqDetectOTStatus( OT_SEQ_DATA *OverTravelSeq, BASE_CTRL_OUT *BaseCtrlOut, BOOL PotSig, BOOL NotSig )
{
	CTRLMD	*CtrlMode;
	LONG	lwk, SpdRef;

	CtrlMode = &BaseCtrlOut->CtrlModeOut;

/*--------------------------------------------------------------------------------------------------*/
/*		OT���o�����菈��																			*/
/*--------------------------------------------------------------------------------------------------*/
	/*	�e�[�u���^�]���[�����E�[���N�����v����OT���o���Ȃ��B										*/
	if( BaseCtrlOut->TblZclamp == TRUE )
	{
		/* OT���o�֎~ */
		OverTravelSeq->var.OverTrvlSts = FALSE;
		return FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		OT���o����																					*/
/*--------------------------------------------------------------------------------------------------*/
	switch( CtrlMode->b.cm )
	{
/*--------------------------------------------------------------------------------------------------*/
	case CTRLMODE_SPD:	/* ���x���䃂�[�h 															*/
/*--------------------------------------------------------------------------------------------------*/
		/* ���x�w�ߎ擾 */
		SpdRef = BaseCtrlOut->OutputSpdRef;

		if( (SpdRef >= 0) && PotSig )
		{ /* ���x�w��:���� && POT��� */
			OverTravelSeq->var.OverTrvlSts = TRUE;
		}
		else if( (SpdRef <= 0) && NotSig )
		{ /* ���x�w��:���� && NOT��� */
			OverTravelSeq->var.OverTrvlSts = TRUE;
		}
		else
		{ /* OT��ԂȂ� */
			OverTravelSeq->var.OverTrvlSts = FALSE;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case CTRLMODE_POS:	/* �ʒu���䃂�[�h															*/
/*--------------------------------------------------------------------------------------------------*/
		lwk = BaseCtrlOut->PositionError;
		if( lwk == 0 )
		{ /* �ʒu�΍��Ȃ� */
			if( PotSig || NotSig )
			{ /* POT��� or NOT��� */
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else
			{ /* OT��ԂȂ� */
				OverTravelSeq->var.OverTrvlSts = FALSE;
			}
		}
		else
		{ /* �ʒu�΍����� */

			SpdRef = BaseCtrlOut->OutputSpdRef;
			if( (((lwk > 0) || (SpdRef >= 0)) && PotSig) )
			{ /* (�ʒu�΍�:���� || ���x�w��:����) && POT��� */
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else if( (((lwk < 0) || (SpdRef <= 0)) && NotSig) )
			{ /* (�ʒu�΍�:���� || ���x�w��:����) && NOT��� */
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else
			{ /* OT��ԂȂ� */
				OverTravelSeq->var.OverTrvlSts = FALSE;
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case CTRLMODE_TRQ:	/* �g���N���䐧�䃂�[�h														*/
/*--------------------------------------------------------------------------------------------------*/
		lwk = BaseCtrlOut->TrqCtrlRef;
		if( ((lwk >= 0) && PotSig) || ((lwk <= 0) && NotSig) )
		{ /* POT��� or NOT��� */
			OverTravelSeq->var.OtOffCnt = 0;
			if( OverTravelSeq->var.OtOnCnt >= KPI_SCANC_MS( 10 ) )
			{
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else
			{
				OverTravelSeq->var.OtOnCnt++;
//				OverTravelSeq->var.OverTrvlSts = FALSE; /* <S0B7> */
			}
		}
		else
		{
			OverTravelSeq->var.OtOnCnt = 0;
			if( OverTravelSeq->var.OtOffCnt >= KPI_SCANC_MS( 10 ) )
			{
				OverTravelSeq->var.OverTrvlSts = FALSE;
			}
			else
			{
				OverTravelSeq->var.OtOffCnt++;
//				OverTravelSeq->var.OverTrvlSts = TRUE; /* <S0B7> */
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:	/* JOG���[�h,etc																	*/
/*--------------------------------------------------------------------------------------------------*/
		OverTravelSeq->var.OverTrvlSts = FALSE;
		break;
	}
	return OverTravelSeq->var.OverTrvlSts;
}



/****************************************************************************************************/
/*																									*/
/*		�I�[�o�[�g���x���V�[�P���X(OT�V�[�P���X)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �}�V���̃X�g���[�N�G���h���z�����ꍇ�̔���~�������s���B								*/
/*			��~���[�h�̓��[�U�[�萔�̐ݒ�ɂ��3�ʂ�I���ł���B									*/
/*																									*/
/*	�߂�l�FOT�V�[�P���X����̃x�[�X�C�l�[�u���v��													*/
/*																									*/
/****************************************************************************************************/
// BOOL BeSeqOverTravelSequence( OT_SEQ_DATA *OverTravelSeq, LONG CtrlMode, BOOL MotStop, BOOL BeComplete )  /* <S1F5> */
BOOL BeSeqOverTravelSequence( OT_SEQ_DATA *OverTravelSeq, LONG CtrlMode, BOOL MotStop,     /* <S1F5> */
	                                                      BOOL BeComplete, BOOL LdstpTimeZero )    /* <S1F5> */
{
	LONG	CtrlMode2Lst;
	LONG	OtStopMode;
/*--------------------------------------------------------------------------------------------------*/
/*		OT�V�[�P���XNetWork I/F�p���䃂�[�h�O��l�ۑ�												*/
/*--------------------------------------------------------------------------------------------------*/
	OtStopMode = OverTravelSeq->conf.OtStopMode;
	CtrlMode2Lst = OverTravelSeq->var.CtrlMode2;
	OverTravelSeq->var.CtrlMode2 = CtrlMode;

/*--------------------------------------------------------------------------------------------------*/
/*		OT�V�[�P���X����																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( OverTravelSeq->var.OtStep )
	{
/*----------------------------------------------------------------------------------------------*/
	case 0: /* Initial Step																		*/
/*----------------------------------------------------------------------------------------------*/
		OverTravelSeq->var.OtBeReq = TRUE;
		OverTravelSeq->var.OtTrqLimit = FALSE;
		OverTravelSeq->var.OtZeroStop = FALSE;
		OverTravelSeq->var.OtLinDecStop = FALSE;   /* <S1F5> */
		OverTravelSeq->var.OtZeroClamp = FALSE;
		OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
		OverTravelSeq->var.OtWait = 0;
		OverTravelSeq->var.OtStep = 1;
		break;
/*----------------------------------------------------------------------------------------------*/
	case 1: /* OT�Ď� (OT�łȂ����)															*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == TRUE )	/* OT�`�F�b�N							*/
		{ /* OT��� */
			/* 2011.04.04 Y.Oka <V680> */
			if( (BeComplete == TRUE) && (OverTravelSeq->conf.OtWarningSw == TRUE) )
			{ /* BaseEnable && OT���[�j���O���o���� */
				OverTravelSeq->var.OtWarningDet = TRUE;
				OverTravelSeq->var.OtWarngWaitCnt = 0;
			}
		/*--------------------------------------------------------------------------------------*/
			/* �g���N���䎞��OT��~���@��DB��~�ɂ���											*/
			if( CtrlMode == CTRLMODE_TRQ )
			{ /* �g���N���䃂�[�h */
				OverTravelSeq->var.OtBeReq = FALSE;		/* �g���N���䃂�[�h�ł�OT��~��DB��~	*/
				OverTravelSeq->var.OtDbopReq = OTDBOP_DEF;
				OverTravelSeq->var.OtStep = 2;
				break;
			}
		/*--------------------------------------------------------------------------------------*/
			switch( OtStopMode )						/* OT��~���[�h�ɂ�镪��				*/
			{
			case OTSTOPMODE_DEFAULT:					/* �W����~���[�h(Pn001.0)				*/
				OverTravelSeq->var.OtBeReq = FALSE;
				OverTravelSeq->var.OtDbopReq = OTDBOP_DEF;
				OverTravelSeq->var.OtStep = 2;
				break;
			case OTSTOPMODE_ZSTOPZC:					/* �[������~���T�[�{���b�N				*/
				OverTravelSeq->var.OtTrqLimit = TRUE;
				OverTravelSeq->var.OtZeroStop = TRUE;
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 3;
				break;
			case OTSTOPMODE_ZSTOPFR:					/* �[������~���t���[����				*/
				OverTravelSeq->var.OtTrqLimit = TRUE;
				OverTravelSeq->var.OtZeroStop = TRUE;
/* <S1F5> Start */
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 5;
				break;
			case OTSTOPMODE_LDSTOPZC:					/* ����������~���T�[�{���b�N			*/
				if( LdstpTimeZero == TRUE )
				{	/* Pn30A = 0�Ƃ����ꍇ�A�[������~�ƂȂ�B�������APn406�ɂ��g���N�����͗L���B	*/
					OverTravelSeq->var.OtTrqLimit = TRUE;
					OverTravelSeq->var.OtZeroStop = TRUE;
				}
				else
				{
					OverTravelSeq->var.OtLinDecStop = TRUE;
				}
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 3;
				break;
			case OTSTOPMODE_LDSTOPFR:					/* ����������~���t���[����				*/
				if( LdstpTimeZero == TRUE )
				{	/* Pn30A = 0�Ƃ����ꍇ�A�[������~�ƂȂ�B�������APn406�ɂ��g���N�����͗L���B	*/
					OverTravelSeq->var.OtTrqLimit = TRUE;
					OverTravelSeq->var.OtZeroStop = TRUE;
				}
				else
				{
					OverTravelSeq->var.OtLinDecStop = TRUE;
				}
/* <S1F5> End */
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 5;
				break;
			default:
				 break;
			}
		}
		/* 2011.04.04 Y.Oka <V680> */
		else
		{ /* OT��Ԃł͂Ȃ��ꍇ */
			if( OverTravelSeq->var.OtWarngWaitCnt > KPI_SCANC_MS(1000) )
			{/* OT���[�j���O�����o������A1�b�ԕێ����ăN���A */
				OverTravelSeq->var.OtWarningDet = FALSE;
			}
			else
			{
				OverTravelSeq->var.OtWarngWaitCnt++;
			}
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 2: /* �W����~���@�Œ�~���AOT�����҂��^���[�^��~�҂�									*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT�����`�F�b�N					*/
		{
			OverTravelSeq->var.OtWait++;
			if( OverTravelSeq->var.OtWait >= KPI_SCANC_MS( 20 ) )
			{ /* OT�`���^�����O�h�~ */
				OverTravelSeq->var.OtBeReq = TRUE;			/* �x�[�X�C�l�[�u���v���I��			*/
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;	/* DB����v������					*/
				OverTravelSeq->var.OtWait = 0;
				OverTravelSeq->var.OtStep = 1;
			}
		}
//	<2010.11.17>���䃂�[�h�ύX	
//		else if((( OverTravelSeq->var.CtrlMode2 == POS_CTRL_MODE ) || ( OverTravelSeq->var.CtrlMode2 == SPD_CTRL_MODE ))
//				&& ( CtrlMode2Lst == NO_CTRL_MODE ) 
//				&& ( OtStopMode == OTSTOPMODE_ZSTOPZC ))
		else if((CtrlMode != CTRLMODE_TRQ)
				&& ( CtrlMode2Lst == CTRLMODE_TRQ ) 
				&& ( OtStopMode == OTSTOPMODE_ZSTOPZC ))
		{													/* �[������~���T�[�{���b�N			*/
			OverTravelSeq->var.OtBeReq = TRUE;				/* �x�[�X�C�l�[�u���v���I��			*/
			OverTravelSeq->var.OtTrqLimit = TRUE;
			OverTravelSeq->var.OtZeroStop = TRUE;
			OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
			OverTravelSeq->var.OtStep = 3;
		}
		else
		{
			OverTravelSeq->var.OtWait = 0;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 3: /* �[������~���AOT�����҂��^���[�^��~�҂�											*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT�����`�F�b�N					*/
		{
			OverTravelSeq->var.OtTrqLimit = FALSE;			/* OT�������ŁASTEP-1�ɑJ��			*/
			OverTravelSeq->var.OtZeroStop = FALSE;
			OverTravelSeq->var.OtLinDecStop = FALSE;   /* <S1F5> */
			OverTravelSeq->var.OtStep = 1;
		}
		else if( MotStop )									/* ���[�^��~�`�F�b�N				*/
		{
			OverTravelSeq->var.OtZeroStop = FALSE;			/* ���[�^��~�ŁASTEP-5�ɑJ��		*/
			OverTravelSeq->var.OtLinDecStop = FALSE;    /* <S1F5> */
			OverTravelSeq->var.OtZeroClamp = TRUE;
			OverTravelSeq->var.OtStep = 4;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 4: /* �[������~��A�[���N�����v���[�h�ŁAOT�����҂�									*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT�����`�F�b�N					*/
		{
			OverTravelSeq->var.OtTrqLimit = FALSE;			/* OT���g���N��������				*/
			OverTravelSeq->var.OtZeroClamp = FALSE;			/* OT���[���N�����v����				*/
			OverTravelSeq->var.OtStep = 1;					/* STEP-1�ɑJ��						*/
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 5: /* �[������~���AOT�����҂��^���[�^��~�҂�											*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT�����`�F�b�N					*/
		{
			OverTravelSeq->var.OtTrqLimit = FALSE;			/* OT�������ŁASTEP-1�ɑJ��			*/
			OverTravelSeq->var.OtZeroStop = FALSE;
			OverTravelSeq->var.OtLinDecStop = FALSE;     /* <S1F5> */
			OverTravelSeq->var.OtStep = 1;
		}
		else if( MotStop )									/* ���[�^��~�`�F�b�N				*/
		{
			OverTravelSeq->var.OtBeReq = FALSE;				/* ���[�^��~�ŁASTEP-5�ɑJ��		*/
			OverTravelSeq->var.OtTrqLimit = FALSE;
			OverTravelSeq->var.OtZeroStop = FALSE;
			OverTravelSeq->var.OtLinDecStop = FALSE;    /* <S1F5> */
			OverTravelSeq->var.OtDbopReq = OTDBOP_FREE;
			OverTravelSeq->var.OtStep = 6;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 6: /* �[������~��A�t���[���[�h�ŁAOT�����҂�											*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT�����`�F�b�N					*/
		{
			/* OT�`���^�����O�h�~ */
			OverTravelSeq->var.OtWait++;
			if( OverTravelSeq->var.OtWait >= KPI_SCANC_MS( 20 ) )
			{												/* �w�߂̋ɐ���OT�̏����ɂ��Ă���	*/
				OverTravelSeq->var.OtBeReq = TRUE;			/* �w�߂��[���ߖT�̎��AOT���`���^	*/
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;	/* �����O���N�����̂ŁAOT�I�t��		*/
				OverTravelSeq->var.OtWait = 0;				/* �t�B���^��������B				*/
				OverTravelSeq->var.OtStep = 1;
			}
		}
		else
		{
			OverTravelSeq->var.OtWait = 0;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	default: /*	�{�P�[�X�̓V�[�P���X�\�t�g�ُ�													*/
/*----------------------------------------------------------------------------------------------*/
		KpiSystemError( );
		break;
	}
	
	return OverTravelSeq->var.OtBeReq;
}



/****************************************************************************************************/
/*																									*/
/*		�T�[�{�X�g�b�v���[�h�w��																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �J�[�l���A���[���Ď�����ъe�V�[�P���X��������̃X�g�b�v�v�����󂯂āA�D��x			*/
/*			�Ɋ�Â��X�g�b�v���[�h�����肵�A�c�a�V�[�P���X����уX�L�����b���䃂�[�h�w��			*/
/*			�����ɃX�g�b�v���[�h���w�߂���B														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG BeSeqStopModeManager( BE_SEQ_HNDL *BeSeq)
{
	LONG	StopMode;
	BOOL 	ZeroStopReq;
	BOOL 	LinDecStopReq;    /*  */
	LONG 	OtDbopReq;
	LONG	Gr2DbopReq;       /* <S1F5> */
	LONG	FstpDbopReq;    /* <S1F5> */

	StopMode = BeSeq->StopMode;
	OtDbopReq = BeSeq->OtSeqData.var.OtDbopReq;
	Gr2DbopReq = BeSeq->AlmStop.var.Gr2DbopReq;			/* <S1F5> */
	FstpDbopReq = BeSeq->ForceStop.var.FstpDbopReq;		/* <S1F5> */
//	ZeroStopReq = BeSeq->BkSeqData.var.BkZeroStop     /* <S1F5> */
//				| BeSeq->OtSeqData.var.OtZeroStop | BeSeq->ZcCtrlData.ZcZeroStop;   /* <S1F5> */
	ZeroStopReq = BeSeq->BkSeqData.var.BkZeroStop | BeSeq->AlmStop.var.Gr2ZeroStop    /* <S1F5> */
				| BeSeq->ForceStop.var.FstpZeroStop | BeSeq->OtSeqData.var.OtZeroStop   /* <S1F5> */
				| BeSeq->ZcCtrlData.ZcZeroStop;    /* <S1F5> */

	LinDecStopReq = BeSeq->AlmStop.var.Gr2LinDecStop | BeSeq->BkSeqData.var.BkLinDecStop	/* <S1F5> */
				| BeSeq->ForceStop.var.FstpLinDecStop | BeSeq->OtSeqData.var.OtLinDecStop;   /* <S1F5> */
//	if( BeSeq->AlmStop.var.AlmStopMode > STOPMODE_ZEROSTOP )	/* �A���[���F����~�v����					*/   /* <S1F5> */
//	{          /* <S1F5> */
//		StopMode = BeSeq->AlmStop.var.AlmStopMode;			/* �[������~�ȊO�̒�~���[�h(DB,etc)		*/   /* <S1F5> */
//	}    /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
//	else if( BeSeq->BkSeqData.var.BkBeReq == FALSE )				/* �x�[�X�C�l�[�u���v���I�t��				*/	/* <S1F5> */
	if( (BeSeq->AlmStop.var.AlmStopMode >= STOPMODE_DBSTOPDB)	/* �A���[���FDB��~/�t���[������~	*/	/* <S1F5> */
	 || (BeSeq->ForceStop.var.FstpMode >= STOPMODE_DBSTOPDB))	/* ������~�FDB��~/�t���[������~	*/	/* <S1F5> */
	{
		StopMode = BeSeq->AlmStop.conf.StopMode;				/* �T�[�{�I�t���̒�~���[�h					*/
	}
	else if(( BeSeq->BkSeqData.var.BkBeReq == FALSE )				/* �x�[�X�C�l�[�u���v���I�t��		*/	/* <S1F5> */
		 &&(BeSeq->AlmStop.var.AlmStopMode == STOPMODE_NOTSTOP )	/* �A���[���F���~					*/	/* <S1F5> */
		 &&(BeSeq->ForceStop.var.FstpMode == STOPMODE_NOTSTOP ))	/* ������~�F���~					*/	/* <S1F5> */
	{
		StopMode = BeSeq->AlmStop.conf.StopMode;				/* �T�[�{�I�t���̒�~���[�h					*/
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( (BeSeq->OtSeqData.var.OtBeReq == FALSE) && (OtDbopReq == OTDBOP_DEF) )
	{ /* OT���W����~���@ */
		if( BeSeq->AlmStop.conf.StopMode == STOPMODE_DBSTOPDB )
		{ /* DB�Ń��[�^��~��DB�p��(Pn001.0==0) */
			/* DB�Ń��[�^��~��DB����(Pn001.0==1) */
			StopMode = STOPMODE_DBSTOPFR;
		}
		else
		{
			/* �T�[�{�I�t���̒�~���[�h */
			StopMode = BeSeq->AlmStop.conf.StopMode;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( ((BeSeq->OtSeqData.var.OtBeReq == FALSE) && (OtDbopReq == OTDBOP_FREE) )  /* <S1F5> */
		  || ((BeSeq->BkSeqData.var.BkBeReq == FALSE) && (Gr2DbopReq == GR2DBOP_FREE))   /* <S1F5> */
		  || ((BeSeq->BkSeqData.var.BkBeReq == FALSE) && (FstpDbopReq == FSTPDBOP_FREE)) )    /* <S1F5> */
	{													/* OT���t���[������~						*/
		StopMode = STOPMODE_FREERUN;					/* �t���[������~���[�h						*/
	}
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1F5> Start	*/
	else if( BeSeq->AlmStop.var.AlmZeroStop )					/* �A���[���F�[������~�v����				*/
	{
		StopMode = BeSeq->AlmStop.var.AlmStopMode;			/* �[������~���[�h							*/
	}
#endif	/* <S1F5> End	*/
/*--------------------------------------------------------------------------------------------------*/
	else if( ZeroStopReq )			/* ALM, FSTP, BK, OT, ZC�V�[�P���X�F�[������~�v����			*/   /* <S1F5> */
	{
		StopMode = STOPMODE_ZEROSTOP;					/* �[������~���[�h							*/
	}
/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
	else if( LinDecStopReq )		/* ALM, FSTP, OT�V�[�P���X�F���������x�ɂ�錸����~��~�v����	*/
	{
		StopMode = STOPMODE_LDECSTOP;					/* ���������x�ɂ�錸����~���[�h			*/
	}
/* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
	else												/* �x�[�X�C�l�[�u���v���I����				*/
	{
		StopMode = STOPMODE_NOTSTOP;					/* ��~���[�h����							*/
	}
/*--------------------------------------------------------------------------------------------------*/
	BeSeq->StopMode = StopMode;
	return StopMode;
}



/****************************************************************************************************/
/*																									*/
/*		�_�C�i�~�b�N�u���[�L�V�[�P���X(DB�V�[�P���X)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : OT�V�[�P���X��BK�V�[�P���X����̃x�[�X�C�l�[�u���v���ƁA�X�g�b�v���[�h�w��				*/
/*			��������̃X�g�b�v���[�h�w�߂��󂯂āA�T�[�{�̃x�[�X�C�l�[�u���^�x�[�X�u���b�N			*/
/*			����тc�a�I���^�I�t�̃^�C�~���O�̐�����s���B											*/
/*																									*/
/*			��~���[�h�̓��[�U�[�萔(Pn001.0)�ɂ��C�R��ނ̒�����I�ׂ�B							*/
/*				Pn001.1=0 : DB��~																	*/
/*				Pn001.1=1 : DB��~�A���[�^��~��DB�I�t�Ń��[�^�t���[����							*/
/*				Pn001.1=2 : DB�Ȃ��A�t���[������~													*/
/*																									*/
/*	�� �� : ASIC_PUMP_ON��OC�����������A�܂��A�x�[�X�C�l�[�u���v���Ƃ̃C���^���b�N���Ƃ遨(STATE5)	*/
/*			DB�T�C���X�^�d�l�̏ꍇ�̓��[�^��~�̑O��DB�I�t											*/
/*			�T�C���X�^�g�p�̏ꍇ���l������sysf.dbon�Ƃ͕ʂ�JL034_000.5(DBON)�𒼐ڐݒ肷��			*/
/*			�T�C���X�^�ɂ��DB�ł̓��[�^��]���P�x�T�C���X�^���I������΁ADBON���I�t���Ă�DB��		*/
/*			����������B���[�^���~�܂��Ă���I�t���Ă���ƃT�C���X�^���I�t����O�Ƀ��[�^��			*/
/*			�܂���������A�T�C���X�^���؂�Ȃ��Ȃ�댯������B����āA�T�C���X�^�ɂ��				*/
/*			DB�ł�DBON�㎟�̃T���v�����O(2ms��j�ɃI�t����B										*/
/*			OT���ɃT�[�{�I�t���ꂽ�ꍇ�A�T�[�{�I�t���̒�~���@�ɏ]���悤�C��						*/
/*			OT���̃T�[�{�I�t���čēx�T�[�{�I�����ꂽ�ꍇ�AOT�̒�~���[�h(�t���[)�ɂ���				*/
/*																									*/
/****************************************************************************************************/
BOOL BeSeqDynamicBrakeSequence( BE_SEQ_HNDL *BeSeq, ASICS *SvAsicRegs, LONG MotStop, BOOL EncDataNG,
								BOOL MotorLessTestMode )											/* <S1B7> */
{
	BOOL				DbOnRequest;
	DB_SEQ_DATA 		*DbSeqData;
	PUMP_CTR_STR 		*PumpOnCtrl;
	/* ���[�J���ϐ��̏����� */
	DbSeqData = &BeSeq->DbSeqData;
	PumpOnCtrl = &BeSeq->ChargePump;
	DbOnRequest = BeSeq->DbOn;

/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�C�l�[�u���v���̍쐬																	*/
/*--------------------------------------------------------------------------------------------------*/
	DbSeqData->DbBeReq = BeSeq->BkSeqData.var.BkBeReq & BeSeq->OtSeqData.var.OtBeReq;

/*--------------------------------------------------------------------------------------------------*/
/*		DB��BE�V�[�P���X																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( DbSeqData->DbStep )
	{
	/*----------------------------------------------------------------------------------------------*/
		case 0: /* Initial Step																		*/
	/*----------------------------------------------------------------------------------------------*/
			/* BaseBlock */
			SHalDrv_BaseBlock(SvAsicRegs->AsicHwReg, SvAsicRegs->AsicMicroReg); /* <S0E4> */

#ifdef CSW_CSTR_DB_REVERSE
			/* Enable DB Control <2010.12.02> */
			SHalDrv_EnableDynamicBrake(SvAsicRegs->AsicHwReg);
#endif

			PumpOnCtrl->PumpOnReq = FALSE;
			BeSeq->BeReqSeqOut = FALSE;

			DbSeqData->DbWait = 0;

			if( BeSeq->StopMode == STOPMODE_FREERUN )
			{
				DbOnRequest = FALSE;
				DbSeqData->DbStep = 1;
			}
			/* DB��~�E��~��t���[ */
			else if( BeSeq->StopMode == STOPMODE_DBSTOPFR )
			{
				if( EncDataNG == TRUE )
				{ /* �G���R�[�_�f�[�^NG����DB�I������ */
					DbOnRequest = TRUE;
				}
				else
				{
					DbOnRequest = FALSE;
				}
				DbSeqData->DbStep = 1;
			}
			/* DB��~�E��~��p�� */
			else
			{
				DbOnRequest = TRUE;
				DbSeqData->DbStep = 6;
			}
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 1: /* ���[�^��~�҂� : ���[�^��~��DB���I�t����										*/
	/*----------------------------------------------------------------------------------------------*/
		if(( BeSeq->StopMode == STOPMODE_DBSTOPFR )&&( EncDataNG == TRUE ))
											/* �G���R�[�_�f�[�^NG����DB�I�����Ă����ɗ��܂�			*/
		{									/* �����ɗ��܂�̂̓C�j�V�������ɃG���R�[�_�f�[�^NG��	*/
											/* �Ȃ����ꍇ�B�^�]��NG�̓X�e�b�v6�ɗ��܂�				*/
			DbOnRequest = TRUE;				/* �G���R�[�_�f�[�^NG����DB�I�����Ă����ɗ��܂�			*/
			PumpOnCtrl->PumpOnReq = FALSE;	/* �G���R�[�_�f�[�^NG����DB�I�����Ă����ɗ��܂�			*/
											/* �`���[�W�|���v�쓮�v���I�t -> LpxChargePumpControl	*/
		}									/* �G���R�[�_�f�[�^NG����DB�I�����Ă����ɗ��܂�			*/
		else if( MotStop )					/* �G���R�[�_�f�[�^���M�p�o���鎞�̂݃��[�^��~�`�F�b�N	*/
		{
			DbOnRequest = FALSE;
			PumpOnCtrl->PumpOnReq = FALSE;	/* �`���[�W�|���v�쓮�v���I�t -> LpxChargePumpControl 	*/

			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 2;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 2: /* �x�[�X�C�l�[�u���v���҂�															*/
	/*				��~���@��DB�Ȃ��܂���DB��t���[��DB���I�t���Ă���Ƃ����̏��					*/
	/*----------------------------------------------------------------------------------------------*/
		/* StopMode���ω�(BK.Seq��OT.Seq�̏����ɂ��)��DB��~�ADB�p������X�e�[�g�ύX */
		if( BeSeq->StopMode == STOPMODE_DBSTOPDB )
		{
			DbOnRequest = TRUE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 6;
		}
		else
		{
			/* �x�[�X�C�l�[�u�����`�F�b�N */
			if( DbSeqData->DbBeReq )
			{
				/* �`���[�W�|���v�쓮�v���I�� */
				PumpOnCtrl->PumpOnReq = TRUE;
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 3;
			}
			/* �x�[�X�C�l�[�u�����N�G�X�g�҂����ɃG���R�[�_�f�[�^NG�ƂȂ����ꍇ�̓V�[�P���X��1��	*/
			else if(( BeSeq->StopMode == STOPMODE_DBSTOPFR )&&( EncDataNG == TRUE ))
			{
				DbSeqData->DbStep = 1;
			}
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 3: /* �`���[�W�|���v�̐���																*/
	/* �x�[�X�C�l�[�u���v�����p�����Ă��邩�`�F�b�N���Ȃ���`���[�W�|���v�𐧌䂷��					*/
	/*----------------------------------------------------------------------------------------------*/
		/* �x�[�X�C�l�[�u���v���r�⏈�� */
		if( DbSeqData->DbBeReq == FALSE )
		{
			PumpOnCtrl->PumpOnReq = FALSE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 0;
		}
		/* �x�[�X�C�l�[�u���v���p�������� Wait32 = DB�����[�I�t->PumpOn�܂ł̑҂�(16)+PumpOn����(16) */
		else if( PumpOnCtrl->PumpOnComp == TRUE )						/*	�`���[�W�|���v���䊮��	*/
		{
			PumpOnCtrl->PumpOnReq = FALSE;

			SHalDrv_ResetAlarmLtStatus(SvAsicRegs->AsicHwReg);

			/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ��x�[�X�C�l�[�u������ *//* <S1B7> */
			if( MotorLessTestMode == FALSE )
			{
				SHalDrv_BaseEnable(SvAsicRegs->AsicHwReg, SvAsicRegs->AsicMicroReg);
			}

			BeSeq->BeReqSeqOut = TRUE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 4;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 4: /* �x�[�X�u���b�N�v���҂�															*/
	/* �iASIC��)�x�[�X�C�l�[�u���w�ߒ��͂����ɂ���													*/
	/*----------------------------------------------------------------------------------------------*/
		/* �x�[�X�C�l�[�u���v���r�⏈�� */
		if( DbSeqData->DbBeReq == FALSE )
		{
			/* �x�[�X�u���b�N���͓d�����o�l���O�a����̂ŁAIFB�Q�C�����O�ɂ���	*/
			/* PUMPON����Ƃ��A�d���w�߂Ƃ��d��FB�Ƀf�[�^�������PWM��			*/
			/* �f���[�e�B�[��50%�łȂ��Ȃ�A���܂��[�d�ł��Ȃ�					*/
			/* BaseBlock */
			SHalDrv_BaseBlock(SvAsicRegs->AsicHwReg, SvAsicRegs->AsicMicroReg); /* <S0E4> */

			BeSeq->BeReqSeqOut = FALSE;

			/*--------------------------------------------------------------*/
			/*			��~���[�h�̔���									*/
			/*--------------------------------------------------------------*/
			/* �t���[������~ */
			if( BeSeq->StopMode == STOPMODE_FREERUN )
			{
				DbOnRequest = FALSE;
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 2;
			}
			/* �c�a��~�E��~��t���[ */
			else if( BeSeq->StopMode == STOPMODE_DBSTOPFR )
			{
				if( EncDataNG == TRUE )
				{ /* �G���R�[�_�f�[�^NG�̏ꍇ */
					/*  �G���R�[�_�f�[�^NG����STEP1�֑J�� */
					DbSeqData->DbWait = 0;
					DbSeqData->DbStep = 5;
				}
				/* ��~���[�h��DB��~��t���[�̏ꍇ�Ń��[�^���~�܂��Ă���Ƃ�dbon���Ȃ��B*/
				/* 1�T���v�����O�҂��Ȃ��̂�OT���w��0�ߖT�Ń����[���`���^�邱�Ƃ̑΍� */
				else if( MotStop )
				{
					/* A.C90(�G���R�[�_�ʐM�ُ�)��������1�b��DB���I������ */
					DbOnRequest = FALSE;
					DbSeqData->DbWait = 0;
					DbSeqData->DbStep = 1;
				}
				else
				{
					DbSeqData->DbWait = 0;
					DbSeqData->DbStep = 5;
				}
			}
			/* �c�a��~�E��~��p�� */
			else
			{
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 5;
			}
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 5: /* �x�[�X�u���b�N�v���ɂ��c�a�I������												*/
	/* �x�[�X�u���b�N�v������P�T�C�N���x�点�Ăc�a���I������										*/
	/* ���t���[������~�̏ꍇ�͂����ւ͗��Ȃ�														*/
	/*----------------------------------------------------------------------------------------------*/
		/*--------------------------------------------------------------*/
		/*			��~���[�h�̔���									*/
		/*--------------------------------------------------------------*/
		/* �c�a��~�E��~��t���[ */
		if( BeSeq->StopMode == STOPMODE_DBSTOPFR )
		{
			if( EncDataNG == TRUE )
			{ /* �G���R�[�_�f�[�^NG�̏ꍇ */
				DbOnRequest = TRUE;				/* �G���R�[�_�f�[�^NG����DB�I�����Ă����ɗ��܂�		*/
			}
			else if( MotStop )
			{ /* �G���R�[�_�f�[�^���M�p�o����Ƃ��͒�~���`�F�b�N	*/
				/* A.C90(�G���R�[�_�ʐM�ُ�)��������DB���I������ */
				DbOnRequest = FALSE;
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 1;
			}
			else
			{
				DbOnRequest = TRUE;				/* ���[�^��]���͂c�a�I�����X�e�[�g�P�Œ�~��҂� 	*/
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 1;
			}
		}
		/* StopMode���ω�(BK.Seq��OT.Seq�̏����ɂ��)��StopMode���V���Ƀt���[�ɕω������ꍇ  */
		else if( BeSeq->StopMode == STOPMODE_FREERUN )
		{
			DbOnRequest = FALSE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 1;
		}
		/* �c�a��~�E��~��p�� */
		else
		{
			DbOnRequest = TRUE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 6;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 6: /* �c�a��~�E��~��p���̏ꍇ�̃x�[�X�u���b�N������									*/
	/* ���[�^��~�����x�[�X�C�l�[�u���v����������c�a���I�t����									*/
	/*----------------------------------------------------------------------------------------------*/
		if(((( EncDataNG == FALSE ) && MotStop ) && DbSeqData->DbBeReq )
		   || ( BeSeq->StopMode == STOPMODE_FREERUN )	/* ���́ADB��~������OT�t���[������~�L��	 */
		   || ( BeSeq->StopMode == STOPMODE_DBSTOPFR ))	/* ���́ADB��~������OT�W����~�iDB��~��t���[�����j*/
		{
			DbOnRequest = FALSE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 2;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		default: /*	�{�P�[�X�̓\�t�g�ُ�															*/
	/*----------------------------------------------------------------------------------------------*/
		KpiSystemError( );
		break;
	}

	BeSeq->DbOn = DbOnRequest;
	return DbOnRequest;
}

#if (CSW_AUTO_CHARGE_PUMP != FALSE)
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �`���[�W�|���v��ON/OFF������s���B														*/
/*																									*/
/*	�� �� : �o�v�l���䎞(�x�[�X�C�l�[�u����)�́A�`�r�h�b�Ń|���v�n�m�w�߂������Ƃ����				*/
/*			�̂ŁA�x�[�X�C�l�[�u���Ƃ̃C���^�[���b�N�͕s�v											*/
/****************************************************************************************************/
void BeSeqChargePumpControl( ASICS *SvAsicRegs, PUMP_CTR_STR *workptr, BOOL OCState,
								BOOL MotorLessTestMode,BOOL NegPumpReq )/* <S1B7> *//*<S201>*/
{
	ASIC_HW_IF_STR *pAsicHwReg = SvAsicRegs->AsicHwReg;
	ASIC_U_IF_STR *pAsicMicroReg = SvAsicRegs->AsicMicroReg;

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�뒲��																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( workptr->AdjPumponReq == TRUE )					/* �뒲�|���v�n�m�v���`�F�b�N				*/
	{
		/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ�ON����	*//* <S1B7> */
		if( MotorLessTestMode == FALSE )
		{
			SHalDrv_AutoChargePumpON(pAsicHwReg);		/* �A���`���[�W�|���vON */
		}
		workptr->SvonPumpCnt++;
		workptr->PumpOnComp = FALSE;					/* �`���[�W�|���v����						*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�X�L�����b : Charge Pump Control Request													*/
/*--------------------------------------------------------------------------------------------------*/
	else if( workptr->PumpOnReq == TRUE )				/* �X�L�����b�|���v�n�m�v���`�F�b�N			*/
	{
		if( OCState != FALSE )
		{
			SHalDrv_ChargePumpOFF(pAsicHwReg);
			return;
		}

		switch(workptr->PumpOnSeq)
		{
		case 0:	/* Wait for Relay ON */
			if( KlibGetLongTimerMs(workptr->TimerCnt) > 15 )
			{
				/*------------------------------------------------------------------------------*/
				/* �x�[�X�C�l�[�u���O�ɃA���[�����b�`���Z�b�g���s�Ȃ� 							*/
				/*------------------------------------------------------------------------------*/
				/* KPI_ALARM_CLEAR( );�`�r�h�b�A���[���N���A */
				SHalDrv_ResetAlarmLtStatus(pAsicHwReg);
				/*�`�c�ϊ��f�W���[�V�����t�B���^�̃A���[���N���A */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);

				workptr->SvonPumpCnt = 0;
				workptr->PumpOnSeq = 1;
				KlibRstLongTimer(&workptr->TimerCnt);

				SHalDrv_StartAutoChargePump(pAsicHwReg);
			}
			break;

		case 1:	/* Charg Pump ON/OFF */
			workptr->SvonPumpCnt++;
			if((0 == SHalDrv_GetAutoChargePumpState(pAsicHwReg))
				|| ( KlibGetLongTimerMs(workptr->TimerCnt) > 16 ))
			{
				/* �`�c�ϊ��f�W���[�V�����t�B���^�̃A���[���N���A */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);
				workptr->PumpOnComp = TRUE;			/* �`���[�W�|���v����				*/
				workptr->PumpOnSeq = 2;
			}

			break;

		default: /* Complete state */
			break;
		}
	}
/*<S201> Start*/
/*--------------------------------------------------------------------------------------------------*/
/*		ScanC : The Charge pump ON/OF Switch Control	                                                                               */	
/*--------------------------------------------------------------------------------------------------*/
	 else if( NegPumpReq == TRUE ) /*SCAN C: N-Arm ON required to checking*/
	{
		if( workptr->NegPumpCnt <=128 )/*Hardware Requirements*/
		 {
			if( ( workptr->NegPumpCnt & 0x0001 ) == 0 )/*Bit computing*/
			 {
			     SHalDrv_ChargePumpOFF(pAsicHwReg);/*ChargePumpOFF*/
			 }
			 else
			 {
			     SHalDrv_ChargePumpON(pAsicHwReg);/*ChargePumpON*/
			 }
			 workptr->NegPumpCnt++;/*Bit computing*/
		 }
		 else
		 {
		      SHalDrv_ChargePumpON(pAsicHwReg);/*KPX_PUMPON( );*/
		 }
    }
 /*<S201> END*/	
/*--------------------------------------------------------------------------------------------------*/
/*		�v���Ȃ�																					*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		KlibRstLongTimer(&workptr->TimerCnt);
		workptr->PumpOnSeq = 0;
		workptr->PumpOnComp = FALSE;						/* �`���[�W�|���v����					*/
		SHalDrv_ChargePumpOFF(pAsicHwReg);
		workptr->NegPumpCnt = 0;  /*<S201>*/
	}

}

#else	//(CSW_AUTO_CHARGE_PUMP != FALSE)

/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �`���[�W�|���v��ON/OFF������s���B														*/
/*																									*/
/*	�� �� : �o�v�l���䎞(�x�[�X�C�l�[�u����)�́A�`�r�h�b�Ń|���v�n�m�w�߂������Ƃ����				*/
/*			�̂ŁA�x�[�X�C�l�[�u���Ƃ̃C���^�[���b�N�͕s�v											*/
/*	��ScanB����call����Ă���̂Œ��ӁI																*/
/****************************************************************************************************/
void BeSeqChargePumpControl( ASICS *SvAsicRegs, PUMP_CTR_STR *workptr, BOOL OCState,
								BOOL MotorLessTestMode,BOOL NegPumpReq )/* <S1B7> *//*<S201>*/
{
	ASIC_HW_IF_STR *pAsicHwReg = SvAsicRegs->AsicHwReg;
	ASIC_U_IF_STR *pAsicMicroReg = SvAsicRegs->AsicMicroReg;

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�뒲��																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( workptr->AdjPumponReq == TRUE )					/* �뒲�|���v�n�m�v���`�F�b�N				*/
	{
		if( ( workptr->SvonPumpCnt & 0x0001 ) == 0 )	/* �X�L�����a�����łn�m�^�n�e�e				*/
		{
			SHalDrv_ChargePumpOFF(pAsicHwReg);			/* �`���[�W�|���vOFF						*/
		}
		else
		{
			/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ�ON����	*//* <S1B7> */
			if( MotorLessTestMode == FALSE )
			{
				SHalDrv_ChargePumpON(pAsicHwReg);		/* �`���[�W�|���vON							*/
			}
		}
		workptr->SvonPumpCnt++;
		workptr->PumpOnComp = FALSE;					/* �`���[�W�|���v����						*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�X�L�����b : Charge Pump Control Request													*/
/*--------------------------------------------------------------------------------------------------*/
	else if( workptr->PumpOnReq == TRUE )				/* �X�L�����b�|���v�n�m�v���`�F�b�N			*/
	{
		if( OCState != FALSE )
		{
			SHalDrv_ChargePumpOFF(pAsicHwReg);
			return;
		}

		switch(workptr->PumpOnSeq)
		{
		case 0:	/* Wait for Relay ON */
			if( KlibGetLongTimerMs(workptr->TimerCnt) > 15 )
			{
				/*------------------------------------------------------------------------------*/
				/* �x�[�X�C�l�[�u���O�ɃA���[�����b�`���Z�b�g���s�Ȃ� 							*/
				/*------------------------------------------------------------------------------*/
				/* KPI_ALARM_CLEAR( );�`�r�h�b�A���[���N���A */
				SHalDrv_ResetAlarmLtStatus(pAsicHwReg);
				/*�`�c�ϊ��f�W���[�V�����t�B���^�̃A���[���N���A */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);

				KlibRstLongTimer(&workptr->TimerCnt);
				workptr->SvonPumpCnt = 0;
				workptr->PumpOnSeq = 1;
			}
			break;

		case 1:	/* Charg Pump ON/OFF */
			if( ( workptr->SvonPumpCnt & 0x0001 ) == 0 )
			{
				SHalDrv_ChargePumpOFF(pAsicHwReg);
			}
			else
			{
				/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ�ON����	*//* <S1B7> */
				if( MotorLessTestMode == FALSE )
				{
					SHalDrv_ChargePumpON(pAsicHwReg);
				}
			}
			workptr->SvonPumpCnt++;
/* <S094>:ChargePump�������́APumpOff�ŕK���I���l�ɂ���B */
/* KlibGetLongTimerMs�֐����ƁA�^�C�}�̃��Z�b�g�^�C�~���O��ScanB�̏������ԓ���PumpOn�ŏI���\������B	*/
#if 0
			if( KlibGetLongTimerMs(workptr->TimerCnt) > 15 )
			{
#else
			if(workptr->SvonPumpCnt > CHARGE_PUMP_CMP_TIME)/* CHARGE_PUMP_CMP_TIME = 80H :16ms */
			{
#endif
				/* �`�c�ϊ��f�W���[�V�����t�B���^�̃A���[���N���A */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);
				workptr->PumpOnComp = TRUE;			/* �`���[�W�|���v����				*/
				workptr->PumpOnSeq = 2;
			}
			break;

		default: /* Complete state */
			break;
		}
	}
/*<S201> Start*/
/*--------------------------------------------------------------------------------------------------*/
/*		ScanC : The Charge pump ON/OF Switch Control	                                                                               */	
/*--------------------------------------------------------------------------------------------------*/
	 else if( NegPumpReq == TRUE ) /*SCAN C: N-Arm ON required to checking*/
	{
		if( workptr->NegPumpCnt <=128 )/*Hardware Requirements*/
		 {
			if( ( workptr->NegPumpCnt & 0x0001 ) == 0 )/*Bit computing*/
			 {
			     SHalDrv_ChargePumpOFF(pAsicHwReg);/*ChargePumpOFF*/
			 }
			 else
			 {
			     SHalDrv_ChargePumpON(pAsicHwReg);/*ChargePumpON*/
			 }
			 workptr->NegPumpCnt++;/*Bit computing*/
		 }
		 else
		 {
		      SHalDrv_ChargePumpON(pAsicHwReg);/*KPX_PUMPON( );*/
		 }
    }
 /*<S201> END*/	
/*--------------------------------------------------------------------------------------------------*/
/*		�v���Ȃ�																					*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		KlibRstLongTimer(&workptr->TimerCnt);
		workptr->PumpOnSeq = 0;
		workptr->PumpOnComp = FALSE;						/* �`���[�W�|���v����					*/
		SHalDrv_ChargePumpOFF(pAsicHwReg);
		workptr->NegPumpCnt = 0;  /*<S201>*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#endif //(CSW_AUTO_CHARGE_PUMP != FALSE)


/*<S201>Start*/	
/****************************************************************************************************/
/*																									*/
/*		���HOFF����IPM N���A�[������   �v�����o���Ă��܂�											         */
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ :                																			*/
/*			IPM��N���A�[������𐧌䂷��B															*/
/*			����v���́A�X�L�����a�Ń`�F�b�N�����B                        							*/
/*																							*/
/****************************************************************************************************/
#define N_PUMP_NONE		0											/* N���A�[�����䖳��			*/
#define N_PUMP_SUSPEND	1											/* N���A�[�����䒆�f			*/
#define N_PUMP_START	2											/* N���A�[������J�n			*/
/*--------------------------------------------------------------------------------------------------*/
/******************************************************************************************************/
void BeNegativePumpSequence( POWER_STATUS*   CoutK,   ALMMAN_HANDLE*     Alm,
						     DET_HWBB_STS*   DetHwbb, CHECK_MAIN_POWER*  PowerChecker,
						     DB_SEQ_DATA*    SvSeqV,  SEQ_CTRL_OUT* CoutV1,
						     ASIC_HW_IF_STR* AsicHwReg
						                   )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�ȉ������ꂩ�̏ꍇ�A�m���A�[�����n�e�e�Ƃ���												  */
/*		��AC�d�����͒�			��AC�d�����o�s���ђ�	���x�[�X�C�l�[�u����						    */
/*		���A���[��������		��HWBB���		    ��DC���̓��[�h						           */
/*--------------------------------------------------------------------------------------------------*/
	if( (CoutK->Acon)||
		(!CoutK->AconDetectReady)||
	    (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB)==0)||
	    (Alm->Status.AlmFlag) || 
	    (DetHwbb->HwbbState) || 
	    (PowerChecker->conf.Dcbus))
	{
		SvSeqV->NegPumpState = N_PUMP_NONE;/* N���A�[�����䖳��*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^��~���o�A�y�сA���[�^���f���x���x�����o�ɂ��N���A�[���𐧌�							*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		if( SvSeqV->NegPumpState == N_PUMP_START )
		{  
		    /*���f���x���x���̌��o��N���A�[�����䒆�f*/
			SvSeqV->NegPumpState = (CoutV1->MotSuspLvl)? N_PUMP_SUSPEND : N_PUMP_START;
		}
		else
		{
			/*	���[�^��~���o��N���A�[������J�n		*/
			SvSeqV->NegPumpState = (CoutV1->MotStop)? N_PUMP_START : N_PUMP_SUSPEND;
		}
    }
	/*--------------------------------------------------------------------------------------------------*/
	/*		N���A�[���I���v���쐬																		*/
	/*--------------------------------------------------------------------------------------------------*/
	CoutK->NegPumpReq = (SvSeqV->NegPumpState == N_PUMP_START)? TRUE : FALSE;
}

/*<S201>End*/	

/****************************************************************************************************/
/*																									*/
/*		�[���N�����v�V�[�P���X(�y�b�V�[�P���X)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���x����(Pn000.1=0)�A���x����̃[���N�����v����(Pn000.1=A)�ɂ����āA�O���M������		*/
/*			(/ZCLAMP)���L���ƂȂ����ꍇ�A���x�w�߂̃I�t�Z�b�g���ɂ��h���t�g���������邽�߁A		*/
/*			�ʒu�w�߁��O�ɂ��ʒu���[�v��g�݃��[�^���N�����v����@�\�B							*/
/*																									*/
/*			�{�V�[�P���X�����́A�[���N�����v�v���M���ƃ��[�^��]��Ԃ���[������~�v�������		*/
/*			�[���N�����v�v�����w�߂���V�[�P���X�ł���B											*/
/*			���[�^��]���Ƀ[���N�����v�̃��N�G�X�g�������ꍇ�́A���x�w�߂��O�Ƃ��A���[�^��]		*/
/*			���~�߂�[������~��Ƀ[���N�����v����B												*/
/*			���ۂɈʒu�w�߂O�ňʒu���[�v��g�ނ̂̓X�L�����a�ōs���B								*/
/*																									*/
/*			 1. ���_�T�[�`���ǂ����̊m�F															*/
/*				�T�[�`���Ȃ牽�����Ȃ��B--> ���䃂�[�h�Ǘ��ŏ���									*/
/*																									*/
/*			 2. �[���N�����v�v���̊m�F																*/
/*				���͐M������̃[���N�����v�v���M�������o����B										*/
/*				���j�I�v�V������̐ݒ�ɂ��Ă̓�-�U�𓥏P����B									*/
/*																									*/
/*			 3. ���[�^��~��Ԃ̊m�F																*/
/*				���[�^����~���Ă��邩���邢�͊��Ƀ[���N�����v���Ĉʒu���[�v��g��ł��邩��		*/
/*				�m�F����B																			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define	ZCLAMP_MARGIN	12710						/* 2^24 * (5rpm / 6600rpm) = 12710				*/
/*--------------------------------------------------------------------------------------------------*/
void	CpxZeroClampSequence( 						/* Zero Clamp Sequence 							*/
	ZC_CTRL_DATA *Zcv,								/* Zero Clamp Control							*/
	BASE_CTRL_OUT	*Bout,							/* Base Control Output 							*/
	FUN_CMN_CONTROL *Fctrl,							/* Function Control								*/
	SEQ_CTRL_OUT *Cout)								/* Sequence Output 								*/
{
LONG	lwk;
UINT bZclamp = (Bout->CmdSeqBit & ZEROCLAMP_BIT);
#define LPX_CHECK_SPDCTRLMODE( bout )  (((bout)->CtrlModeOut.b.cm) == CTRLMODE_SPD)
#define LPX_CHECK_RNDSVCTRL( fctrl )    ((fctrl)->FnSvControl)
LONG	SpdRefo = Bout->OutputSpdRef;
LONG	ZcSpdLevel = Zcv->P.ZcSpdLevel;				/* <S00C>										*/
/*--------------------------------------------------------------------------------------------------*/
/*		�[���N�����v�����v�����͐M���`�F�b�N														*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*	�@ �\ : �O�����͐M���ɂ��[���N�����v�M�����L���̏�Ԃɂ����āA�\�t�g�X�^�[�g���				*/
/*			���x�w�߂��A�p�����[�^(Pn501)�Őݒ肳���[���N�����v���x����菬�����ꍇ��				*/
/*			�[���N�����v�V�[�P���X�������J�n����v���t���O�𗧂Ă�B								*/
/*			�������A���o���x�̋߂��ł̃`���^�����O��h�����ߍŏ��ɂTrpm�̃}�[�W����������			*/
/*			�\�����x�Ō��o���s���A���̂܂ܐ����Ȍ��o���x���ȉ��ƂȂ����ꍇ�ɃV�[�P���X�v��			*/
/*			�t���O�𗧂Ă�B																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
		if( bZclamp && LPX_CHECK_SPDCTRLMODE(Bout) && (! LPX_CHECK_RNDSVCTRL(Fctrl) ) )
		{
			lwk = MlibABS( SpdRefo );
			if( lwk < (ZcSpdLevel + ZCLAMP_MARGIN) )
			{
				if( lwk < ZcSpdLevel )
				{
					Zcv->ZcReqSignal = TRUE;
				}
			}
			else
			{
				Zcv->ZcReqSignal = FALSE;
			}
		}
		else
		{
			Zcv->ZcReqSignal = FALSE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�[���N�����v�V�[�P���X																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (Zcv->ZcReqSignal) 						/* �[���N�����v���x�����o�A�܂��́A					*/
												/* �e�[�u���^�]����̃[���N�����v�w��(TODO:)		*/
		/* || (TblExeV.TblZmd.b.zm==CTRLMODE_ZCLMP) */ )	
	{
		if( (Cout->MotStop) ||					/* ���[�^�[��~���A�܂��́A							*/
			(Zcv->ZcZeroClamp))					/* �[���N�����v��									*/
		{
			Zcv->ZcZeroClamp = TRUE ;			/* �[���N�����v�v���ݒ� 							*/
			Zcv->ZcZeroStop = FALSE ;			/* �[������~�v�������ݒ� 							*/
		}
		else
		{
			Zcv->ZcZeroClamp = FALSE ;			/* �[���N�����v�v�������ݒ� 						*/
			Zcv->ZcZeroStop = TRUE ;			/* �[������~�v���ݒ� 								*/
		}
	}
	else
	{
			Zcv->ZcZeroClamp = FALSE ;			/* �[���N�����v�v�������ݒ� 						*/
			Zcv->ZcZeroStop = FALSE ;			/* �[������~�v�������ݒ� 							*/
	}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/* 	<S0A7> Check Servo On Command Error (A.0B0)														*/
/*																									*/
/*	1)SGD7-AP�^�̏ꍇ�A�⏕�@�\�^�]��A�T�[�{�I���w�߂ŃA���[���Ƃ���B								*/
/*	2)SGD7-M2,M3�^�̏ꍇ�A�Ƃ肠����SGDV-MD�Ɠ����d�l�ɂ���B���o�����͈ȉ��Q�ƁB					*/
/*																									*/
/*	���Q�l�FA.0B0���o����																			*/
/*	4)SGDV-AP: �⏕�@�\�ɂ��^�]��A��ʂ���T�[�{�I���w�߂����B									*/
/*	5)SGDV-M2,M3: �⏕�@�\�ɂ��^�]��ADISCONNECT�����ɏ�ʂ���T�[�{�I���w�߂����B				*/
/*	6)SGDV-MD: �⏕�@�\�ɂ��^�]��A���x����/�g���N����ɐ؂�ւ��Ȃ��ŏ�ʂ���T�[�{�I���w�߂����B*/
/*																									*/
/****************************************************************************************************/
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE
LONG	LpxCheckServoOnCmdErr( 						/* Check Servo On Cmd Error (A.0B0):AP			*/
	BE_SEQ_HNDL 	*BeSeq, 						/* 												*/
	BOOL			ServoOnReq,						/* Servo On Command								*/
	FUN_CMN_CONTROL *FnCmnCtrl, 					/* 												*/
	CTRLMD 			CtrlMode )						/* Control Mode									*/
{
LONG	err = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	if( FnCmnCtrl->State.LockNormalOpeDrive != FALSE )
	{	/* �ʏ�^�]�֎~*/
		if( ServoOnReq != FALSE )
		{
			err = TRUE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (err);
}
#else
LONG	LpxCheckServoOnCmdErr( 						/* Check Servo On Cmd Error (A.0B0):M2M3		*/
	BE_SEQ_HNDL 	*BeSeq, 						/* 												*/
	BOOL			ServoOnReq,						/* Servo On Command								*/
	FUN_CMN_CONTROL *FnCmnCtrl, 					/* 												*/
	CTRLMD 			CtrlMode )						/* Control Mode									*/
{
LONG	err = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	if(FnCmnCtrl->State.LockNormalOpeDrive != FALSE)
	{	/* �ʏ�^�]�֎~*/
		if( (CtrlMode.us.uwd_l == CTRLMODE_SPD) || (CtrlMode.us.uwd_l == CTRLMODE_TRQ) )
		{
			BeSeq->SenStatus = FALSE;
			FnCmnCtrl->State.LockNormalOpeDrive = FALSE;
		}
		else if( (ServoOnReq != FALSE) && (CtrlMode.us.uwd_l == CTRLMODE_POS) )
		{/* �T�[�{�I���w�߂��� & �ʒu���䃂�[�h�w�� */
			err = TRUE;
		}
		else
		{	/* do nothing. */
			;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (err);
}
#endif


/***************************************** end of file **********************************************/
