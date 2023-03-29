/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SequenceIo.c : �V�[�P���XI/O�Ǘ�����														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{�V�[�P���X�Ǘ�����																*/
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
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SequenceIo.h"


/* �O���֐� */
extern void HALdrv_SetSOutC( int axis, HWSOUT hwsout );


#if 0	/*<S1F6>*/
/****************************************************************************************************/
/*																									*/
/*		�Z�[�t�e�B�g�v�a�a��Ԍ��o����																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : --�n�[�h���C���[�h�x�[�X�u���b�N��ԁiHWBB��ԁj�̌��o								*/
/*			BBON1LT/BBON2LT�M���̂ǂ��炩�������͗�����1�ƂȂ����ꍇ�iHWBB��Ԓ��j�ɁA				*/
/*			HWBB��Ԃ������t���O��TRUE���Z�b�g����B�����̐M����0�̏ꍇ�iHWBB��ԉ������j�ɂ́A		*/
/*			HWBB��Ԃ������t���O��FALSE���Z�b�g����B�܂��AHWBB��ԉ������ɂ͓��̓G�b�W���o����		*/
/*			���N���A����B																			*/
/*			BBONLT�M���̃��b�`�l���N���A����^�C�~���O�́A�T�[�{�I�t���͂����E���h�^�]����		*/
/*			�擾���Ă��Ȃ��ꍇ�ɍs���B																*/
/*			--BBON�M�����͎��ԍ��ُ�A���[�����o�p�̃t���O�Z�b�g									*/
/*			BBON1LT/BBON2LT�M���̂ǂ��炩�̓��̓G�b�W�����o�����ۂɁA���͎��ԍ��t���O��				*/
/*			TRUE���Z�b�g����BBBON1/BBON2�M���̗����̓��̓G�b�W�����o�����ۂɁA���͎��ԍ��t���O��	*/
/*			FALSE���Z�b�g����B�����̓��̓G�b�W�����o�ł��Ȃ��ꍇ�́A�O��̏�Ԃ�ێ�����B			*/
/*																									*/
/*	�� �� : �Ȃ�																					*/
/*�@�߂�l: �Ȃ�																					*/
/*																									*/
/****************************************************************************************************/
BOOL SeqIoDetectHardWiredBB( DET_HWBB_STS *DetHwbb, LONG svon_req,
										ASIC_HW_IF_STR *pAsicHwReg, BOOL HwbbSupport )
{
	BOOL	Bbon1Lt, Bbon2Lt;
	BOOL	HwbbSts;

/*--------------------------------------------------------------------------------------------------*/
/*		�n�[�h���C���[�h�x�[�X�u���b�N��ԃ`�F�b�N													*/
/*--------------------------------------------------------------------------------------------------*/
	/* �T�[�{�I�t���͂����E���h�^�]���̎擾�Ȃ������Ɍ��o�v���Ȃ� */
//	if( (0 == (CmdSeqBit & SERVO_ON_BIT)) && (0 == (CmdSeqBit & POL_DETECT_BIT)) && (!FnSvControl) )
	if(svon_req == 0)
	{
		/* BBON�M���̃��b�`�l���N���A */
		SHalDrv_ResetBBONLtStatus( pAsicHwReg );
	}

	/* ���݂�BBON�M����ԓǂݍ��݁i���j�^�p�j */
	SHalDrv_GetBBONStatus( pAsicHwReg, &DetHwbb->Bbon1, &DetHwbb->Bbon2 );

	/* ���݂�BBON�M�����b�`�l�ǂݍ��݁iHWBB���o�p�j */
	SHalDrv_GetBBONLtStatus( pAsicHwReg, &Bbon1Lt, &Bbon2Lt );

	/* BBON�M���̃G�b�W���o */
	if( Bbon1Lt )
	{
		DetHwbb->Bbon1Edge = TRUE;
	}
	if( Bbon2Lt )
	{
		DetHwbb->Bbon2Edge = TRUE;
	}

	/* HWBB��Ԑݒ菈�� */
	if( (Bbon1Lt) || (Bbon2Lt) )
	{
		/* BBON�M���ǂ��炩�ł����͂��� */
		if( HwbbSupport == TRUE )
		{ /* HWBB�@�\�Ή��@�� */
			DetHwbb->HwbbState = TRUE;
			DetHwbb->GateDrvDefect = FALSE;
		}
		else
		{ /* HWBB�@�\���Ή��@�� */
			DetHwbb->HwbbState = FALSE;
			DetHwbb->GateDrvDefect = TRUE;
		}
	}
	else
	{
		/* BBON�M���ǂ�������͂Ȃ� */
		if( (DetHwbb->Bbon1Edge) && (DetHwbb->Bbon2Edge) )
		{
			/* BBON1/BBON2�M���̃G�b�W�𗼕����o�������̂�HWBB��ԉ��� */
			DetHwbb->HwbbState = FALSE;
			DetHwbb->Bbon1Edge = FALSE;
			DetHwbb->Bbon2Edge = FALSE;
		}
	}

	/* Set ScanC Output Data */
	if( DetHwbb->HwbbState )
	{
		HwbbSts = TRUE;
	}
	else
	{
		HwbbSts = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���͐M���̎��ԍ��ُ팟�o�^�C�}�X�^�[�g�t���O�̐ݒ�											*/
/*--------------------------------------------------------------------------------------------------*/
	/* BBON�M�����͎��ԍ��t���O�ݒ菈�� */
	if( (DetHwbb->Bbon1Edge) ^ (DetHwbb->Bbon2Edge) )
	{
		/* BBON�M���ǂ��炩�݂̂̓��� */
		DetHwbb->LackOneBbon = TRUE;
	}
	else
	{
		/* BBON�M�������̓��� */
		if( (DetHwbb->Bbon1Edge) && (DetHwbb->Bbon2Edge) )
		{
			DetHwbb->LackOneBbon = FALSE;
		}
	}

	return	HwbbSts;
}
#endif	//#if 0	/*<S1F6>*/

#if 0	/*<S1F6>*/
/****************************************************************************************************/
/*																									*/
/*		�Z�[�t�e�B�@�\�@�����M����Ԍ��o����											<S0F1>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����M����Ԍ��o�pDPIN1, DPIN2�M���̏�Ԃ����o����										*/
/*          DPIN1, DPIN2�M���L��/������Ԃ����o����													*/
/*																									*/
/*	�� �� :																							*/
/*�@�߂�l:																							*/
/*																									*/
/****************************************************************************************************/
void SeqIoDetectTestPulseSignal( SEQ_IO_HNDL *pIoSignals)
{
	TEST_PULSE_DIAG *pTestPulseDiag;
	DET_HWBB_STS *pDetHwbbSts;

	pTestPulseDiag = &(pIoSignals->TestPulseDiag);
	pDetHwbbSts    = &(pIoSignals->DetHwbbSts);
/*--------------------------------------------------------------------------------------------------*/
/*		�M���̑O��l���X�V																			*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag->DpIn1LastState = pTestPulseDiag->DpIn1State;
	pTestPulseDiag->DpIn2LastState = pTestPulseDiag->DpIn2State;

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1�M���̗L���^������Ԕ��� (BBON��ԂłȂ� && �O��DPIN1�M���L�� �� ����DPIN1�M���͗L��	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon1)) && (pTestPulseDiag->DpIn1LastEnable) )
	{
		pTestPulseDiag->DpIn1Enable = TRUE;
	}
	else
	{
		pTestPulseDiag->DpIn1Enable = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN2�M���̗L���^������Ԕ��� (BBON��ԂłȂ� && �O��DPIN2�M���L�� �� ����DPIN2�M���͗L��	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon2)) && (pTestPulseDiag->DpIn2LastEnable) )
	{
		pTestPulseDiag->DpIn2Enable = TRUE;
	}
	else
	{
		pTestPulseDiag->DpIn2Enable = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2�M���̏�Ԏ擾																	*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag->DpIn1State = GET_DPIN1( );
	pTestPulseDiag->DpIn2State = GET_DPIN2( );
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2���(�O��l)�̍X�V																*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag->DpIn1LastEnable  = !(pDetHwbbSts->Bbon1);
	pTestPulseDiag->DpIn2LastEnable  = !(pDetHwbbSts->Bbon2);
}
#endif	/*<S1F6>*/

/* <S1F5> Start */
/*****************************************************************************
  Description: FSTP��Ԍ��o����
------------------------------------------------------------------------------
  Parameters:
  	FstpSignal;			Fstp �M���ǂݍ��ݏ��
	hSeqCtrlOut;		Sequence Control Message
	svon_req;			�T�[�{�I�t����
  Return:
	BOOL				FSTP���
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
BOOL SeqIoDetectFstp( BOOL FstpSignal, SEQ_CTRL_OUT *hSeqCtrlOut, LONG Svon_req )
{
	/* �T�[�{�I�t���͂����E���h�^�]���̎擾�Ȃ������Ɍ��o�v���Ȃ� */
	if ( Svon_req == 0 )
	{
		hSeqCtrlOut->FstpSts = FstpSignal;
	}
	/*FSTP�M�������͂��ꂽ�ꍇ*/
	if ( FstpSignal == TRUE )
	{
		hSeqCtrlOut->FstpSts = TRUE;
	}

	return hSeqCtrlOut->FstpSts;
}

/* <S1F5> End */

/****************************************************************************************************/
/*																									*/
/*		�ێ��u���[�L���䏈��																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		BK�V�[�P���X�����o�A�܂��́ANET-I/F����u���[�L�����v������̏ꍇ��BK�������				*/
/*		BK�V�[�P���X���o�A���ANET-I/F����̉����v���Ȃ��̏ꍇ��BK���								*/
/*																									*/
/****************************************************************************************************/
#ifndef HW_FBA_IO_AXIS
void SeqIoBrakeControl( SEQ_IOSIGNAL *BkSignal, BOOL *BrakeSts,
											BOOL SvBrakeReq, BOOL NetBrkReleaseReq )
{
//	SEQ_IOSIGNAL		*SeqIo;/*<S0C7>*/

	/* BK�V�[�P���X���o�Ȃ��A�܂��́ANET-I/F�����v������ */
	if( (SvBrakeReq == FALSE) || (NetBrkReleaseReq == TRUE) )
	{ /* BRAKE�J�����(BK Signal -> active) */
		*BrakeSts = TRUE;
	}
	else
	{ /* BRAKE���(BK Signal -> inactive) */
		*BrakeSts = FALSE;
	}
//<S045>	SeqIo = BkSignal;
//<S045>	SeqIo->SignalStatus = (*BrakeSts & SeqIo->Mask) ^ SeqIo->XorMask;
//<S045>	HALdrv_BrakeControl( SeqIo->PinNumber, SeqIo->SignalStatus );
}
#else
void SeqIoBrakeControlAx( SEQ_IOSIGNAL *BkSignal, BOOL *BrakeSts,
								BOOL SvBrakeReq, BOOL NetBrkReleaseReq, SERIAL_OUTPUT_AX *SerialOutputAx )
{
	SEQ_IOSIGNAL		*SeqIo;

	/* BK�V�[�P���X���o�Ȃ��A�܂��́ANET-I/F�����v������ */
	if( (SvBrakeReq == FALSE) || (NetBrkReleaseReq == TRUE) )
	{ /* BRAKE�J�����(BK Signal -> active) */
		*BrakeSts = TRUE;
	}
	else
	{ /* BRAKE���(BK Signal -> inactive) */
		*BrakeSts = FALSE;
	}
	SeqIo = BkSignal;
	SeqIo->SignalStatus = (*BrakeSts & SeqIo->Mask) ^ SeqIo->XorMask;
	HALdrv_BrakeControlAx( SeqIo->PinNumber, SeqIo->SignalStatus, SerialOutputAx );
}
#endif

/****************************************************************************************************/
/*																									*/
/*		�X�L�����b�l�b�g���[�N�h�e�o�͏���															*/
/*																									*/
/****************************************************************************************************/
void SeqIoOutputNetIf( SEQ_IO_HNDL *SeqIo, BE_SEQ_HNDL *BeSeq, SEQ_CTRL_OUT *SeqCtrlOut,
								 ALMMAN_HANDLE *AlmManager, MENCV *MencV/* <S082> BOOL PhaseReady */, BOOL LockNormalOpeDrive,
								 BOOL MotorLessTestMode )											/* <S1B7> */
{
	LONG	RspSeqBit, RspSigBit, RspCtrlBit;
	BOOL	sts;

	RspSeqBit = 0;
	RspSigBit = 0;
	RspCtrlBit = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		�V�[�P���X�����ԍ쐬																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* NetAlarm */
	RspSeqBit |= (((AlmManager->Status.AlmFlag) ? TRUE : FALSE) << ALM_DETECTED_BITNO);	/* <S0FF> */

	/* NetWarning */
	RspSeqBit |= (((AlmManager->Status.WrnFlag) ? TRUE : FALSE) << WRN_DETECTED_BITNO); /* <S0FF> */

	/* AlarmRstComp */
	RspSeqBit |= ((1^AlmManager->AlmRst) << ALM_RESET_COMP_BITNO);
	RspSeqBit |= ((1^AlmManager->AlmRst) << WRN_RESET_COMP_BITNO);

	/* ServoOnComp */
	sts = (1^SeqCtrlOut->BBSvcRefClrReq) & MencV->PhaseReady /*<S082> PhaseReady*/;
	RspSeqBit |= (sts << SERVO_ON_COMP_BITNO);

	/* SensOnComp */
//	sts = BeSeq->SenStatus;/* & BeSeq->EncReady*/;
	sts = BeSeq->SenStatus & MencV->EncReady /* <S082> */;
	RspSeqBit |= (sts << SENS_ON_COMP_BITNO);

	/* PolDetectComp */
	RspSeqBit |= (MencV->PhaseReady/* <S082> PhaseReady */<< POL_DET_COMP_BITNO);

	/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ��ݒ肷�� *//* <S1B7> */
	/* TODO:���̏������K���ł��邩�s�� */
	if( MotorLessTestMode == FALSE )
	{
		/* BK�V�[�P���X���o�Ȃ��A�܂��́ANET-I/F�����v������ 											*/
		RspSeqBit |= (SeqCtrlOut->BrkSigSts << BRK_RELEASING_BITNO);
	}

	/* Main Power ON */
	RspSeqBit |= (SeqCtrlOut->MainPowerOn << MAIN_POWER_ON_BITNO);

	/* �T�[�{���f�B�M�� */
	RspSeqBit |= (BeSeq->ServoReady << SERVO_READY_BITNO);

	/* Motor���f�B�M�� */
	RspSeqBit |= (BeSeq->SvReadyPhaseIgnore << MOTOR_READY_BITNO);

	SeqCtrlOut->RspSeqBits = RspSeqBit;

/*--------------------------------------------------------------------------------------------------*/
/*		���͐M����ԍ쐬																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* HWBB */
	RspSigBit |= ((SeqCtrlOut->HwbbSts | SeqCtrlOut->FstpSts) << HWBB_SIG_BITNO);  /* <S1F5> */
	/* HWBB1 */
	RspSigBit |= (SeqIo->DetHwbbSts.Bbon1 << HWBB1_SIG_BITNO);
	/* HWBB2 */
	RspSigBit |= (SeqIo->DetHwbbSts.Bbon2 << HWBB2_SIG_BITNO);
	/* P-OT */
	RspSigBit |= (SeqCtrlOut->PotSigSts << P_OT_SIG_BITNO);
	/* N-OT */
	RspSigBit |= (SeqCtrlOut->NotSigSts << N_OT_SIG_BITNO);
	/* E-STOP */
	RspSigBit |= ((SeqCtrlOut->EstpSigSts | SeqCtrlOut->FstpSts) << ESTP_SIG_BITNO); /* <S1F5> */

	/* DEC */
	RspSigBit |= (SeqIo->DecSignal.SignalStatus.v << DEC_SIG_BITNO);		/* <S045> */
	/* EXT1 */
	RspSigBit |= (SeqIo->ExtSignal[0].SignalStatus.v << EXT1_SIG_BITNO);	/* <S045> */
	/* EXT2 */
	RspSigBit |= (SeqIo->ExtSignal[1].SignalStatus.v << EXT2_SIG_BITNO);	/* <S045> */
	/* EXT3 */
	RspSigBit |= (SeqIo->ExtSignal[2].SignalStatus.v << EXT3_SIG_BITNO);	/* <S045> */

	SeqCtrlOut->RspSigBits = RspSigBit;

/*--------------------------------------------------------------------------------------------------*/
/*		�����ԍ쐬																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* SAFETY_STOP_BIT */
	RspCtrlBit |= (SeqCtrlOut->HwbbSts << SAFETY_STOP_BITNO);
	/* ���[�^��]��� */
	RspCtrlBit |= (SeqCtrlOut->TgOnSts << MOTOR_MOVING_BITNO);
	/* ���x��v���o���� */
	RspCtrlBit |= (SeqCtrlOut->VcmpSts << REACH_SPD_BITNO);
	/* �w�ߎ�t�\���	*/
	RspCtrlBit |= ((1^LockNormalOpeDrive) << OP_ENABLED_BITNO);

	SeqCtrlOut->RspCtrlBits = RspCtrlBit;
}




/****************************************************************************************************/
/*																									*/
/*		OT�M���}�X�N�ݒ�pAPI																		*/
/*																									*/
/****************************************************************************************************/
void SeqIoSetOtSigMask( SEQ_IO_HNDL *SeqIo )
{
	SEQ_IOSIGNAL	*IoSignal;

	/* Mask P-OT Signal */
	IoSignal = &(SeqIo->PotSignal);
	IoSignal->Mask = 0;
	IoSignal->XorMask = 0;
	IoSignal->PinNumber = 0;

	/* Mask N-OT Signal */
	IoSignal = &(SeqIo->NotSignal);
	IoSignal->Mask = 0;
	IoSignal->XorMask = 0;
	IoSignal->PinNumber = 0;
}


/****************************************************************************************************/
/*																									*/
/*		OT�M���}�X�N�����pAPI																		*/
/*																									*/
/****************************************************************************************************/
void SeqIoClearOtSigMask( SEQ_IO_HNDL *SeqIo, USHORT pot_selsw, USHORT not_selsw )
{
	SEQ_IOSIGNAL	*IoSignal;

	/* P-OT mapping */
	IoSignal = &(SeqIo->PotSignal);
	switch( pot_selsw >> 12 )
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(pot_selsw & 0xFF);
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(pot_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

	/* N-OT mapping */
	IoSignal = &(SeqIo->NotSignal);
	switch( not_selsw >> 12 )
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)( not_selsw & 0xFF );
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)( not_selsw & 0xFF );
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}
}

/****************************************************************************************************/
/*																									*/
/*		<S045> �X�L�����b�V�[�P���X�M�����͏���														*/
/*																									*/
/****************************************************************************************************/
void SeqIoInputSeqSignals( SEQ_IO_HNDL *IoSignals, ASIC_HW_IF_STR *AsicHwReg)
{
	HWSIN	SeqInputSignals = HALdrv_ReadInputSignals();
/*--------------------------------------------------------------------------------------------------*/
/*	�V�[�P���X����																					*/
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_START_INSIGX( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_GET_INSIGX( PotSignal, SeqInputSignals.dw );			/* P-OT */
	SEQIO_GET_INSIGX( NotSignal, SeqInputSignals.dw );			/* N-OT */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_END_INSIGX( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
}

/* <S21D> Start */
/*****************************************************************************
  Description:	�X�L�����b�V�[�P���X�M�����͏���(������)
------------------------------------------------------------------------------
  Parameters:
	IoSignals;				�V�[�P���XI/O
	Prm;					�p�����[�^�f�[�^
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void SeqIoInputSeqSignalsCom( SEQ_IO_HNDL *IoSignals, PRMDATA *Prm )
{
	/*--------------------------------------------------------------------------------------------------*/
	/*	�V�[�P���X����																					*/
	/*--------------------------------------------------------------------------------------------------*/
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
	UINT			HwSin;
	HwSin = IoSignals->HwSin;
	SEQIO_START_INSIGX_AP( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_GET_INSIGX_AP( ClinkansSignal, HwSin, ((Prm->insel7 >> 0) & 0x0F) );		/* /CLINKANS */
	SEQIO_GET_INSIGX_AP( CmcansSignal, HwSin, ((Prm->insel7 >> 4) & 0x0F) );		/* /CMCANS */
	SEQIO_GET_INSIGX_AP( ConvreadySignal, HwSin, ((Prm->insel7 >> 8) & 0x0F) );		/* /CONV_READY */
	SEQIO_GET_INSIGX_AP( ConvmconSignal, HwSin, ((Prm->insel7 >> 12) & 0x0F) );		/* /CONV_MCON */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_END_INSIGX_AP( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
#else
	HWSIN	SeqInputSignals = HALdrv_ReadInputSignals();
	SEQIO_START_INSIGX( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_GET_INSIGX( ClinkansSignal, SeqInputSignals.dw );		/* /CLINKANS */
	SEQIO_GET_INSIGX( CmcansSignal, SeqInputSignals.dw );		/* /CMCANS */
	SEQIO_GET_INSIGX( ConvreadySignal, SeqInputSignals.dw );	/* /CONV_READY */
	SEQIO_GET_INSIGX( ConvmconSignal, SeqInputSignals.dw );		/* /CONV_MCON */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_END_INSIGX( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
#endif
}
/* <S21D> End */

/****************************************************************************************************/
/*																									*/
/*		<S045> �X�L�����b�V�[�P���X�M���o�͏���														*/
/*																									*/
/****************************************************************************************************/
void SeqIoOutputSeqSignals( LONG ax, SEQ_IO_HNDL *SeqIo )		/* SGD7W IO���o�͑Ή�<S118> */
{
	HWSOUT OutX; OutX.dw = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		�V�[�P���X�o��																				*/
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_START_OUTSIGX( SeqIo );					/* Start Output Signal */
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) /* <S04B> */
	SEQIO_SET_OUTSIGX( BkSignal,   OutX.dw );		/* /BK */
	SEQIO_SET_OUTSIGX( VcmpSignal, OutX.dw );		/* /VCMP */
	SEQIO_SET_OUTSIGX( TgonSignal, OutX.dw );		/* /TGON */
	SEQIO_SET_OUTSIGX( SrdySignal, OutX.dw );		/* /S-RDY */
	SEQIO_SET_OUTSIGX( WarnSignal, OutX.dw );		/* /WARN */
#endif
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_SET_OUTSIGX( PreWrnSignal, OutX.dw );		/* /PreWrn */ /* <S154> */
	SEQIO_SET_OUTSIGX( ClinkSignal, OutX.dw );		/* /CLINK *//* <S21D> */
	SEQIO_SET_OUTSIGX( CmconSignal, OutX.dw );		/* /CMCON *//* <S21D> */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) /* <S04B> */
	SEQIO_SET_OUTSIGX( Alo1Signal, OutX.dw );		/* ALO1 */
	SEQIO_SET_OUTSIGX( Alo2Signal, OutX.dw );		/* ALO2 */
	SEQIO_SET_OUTSIGX( Alo3Signal, OutX.dw );		/* ALO3 */
#endif
/*--------------------------------------------------------------------------------------------------*/
//-	SEQIO_END_OUTSIGX_ScanC( OutX );				/* End Output Signal */
	SEQIO_END_OUTSIGX_ScanC( ax, OutX );			/* End Output Signal */
}

/***************************************** end of file **********************************************/
