/*********************************** Start of file **************************************************/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SftDiagFunc.c		: Safety Diagnostic function for Sigma-7(SGD7S/W)						*/
/*																									*/
/*		Identification																				*/
/*			Module ID		: 0002(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Module version	: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.03.14															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S1FF><S201>																*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SequenceIo.h"
#include "SftDiagFunc.h"
#include "HwDeviceIf.h"
#include "BaseEnableSequence.h"/*<S201>*/


/****************************************************************************************************/
/*																									*/
/*		�Z�[�t�e�B�f�f������������																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : --�Z�[�t�e�B�f�f��������																*/
/*			�EPnE09.bit8�ɉ�����HWBB�T�|�[�g�̗L����ݒ肷��										*/
/*			�EPnE40�̒l���e�X�g�p���X�ُ�A���[�����o���x���Ƃ��Đݒ肷��							*/
/*																									*/
/*	�� �� : USHORT			opdetsw			:PnE09.bit8												*/
/* 			DET_HWBB_STS	*DetHwbbSts		:HWBB�X�e�[�^�X�Ǘ��\����								*/
/*			USHORT			tplsfil			:PnE40�ݒ�l											*/
/*			TEST_PULSE_DIAG *TestPulseDiag	:�e�X�g�p���X�f�f�Ǘ��\����								*/
/*�@�߂�l: �Ȃ�																					*/
/*																									*/
/****************************************************************************************************/
void SysInitSafetyDevice( USHORT opdetsw, DET_HWBB_STS *DetHwbbSts, USHORT tplsfil, TEST_PULSE_DIAG *TestPulseDiag )
{
	LONG i;
	if( (opdetsw & 0x0100) == 0x0000 )
	{
		DetHwbbSts->hwbbsupport = FALSE;
	}
	else
	{
		DetHwbbSts->hwbbsupport = TRUE;
	}

	for(i=0;i<2;i++)
	{
		TestPulseDiag->DpInErrLvl = tplsfil;
		TestPulseDiag++;
	}
}


/****************************************************************************************************/
/*																									*/
/*		���͐M���^�C�~���O�ُ�`�F�b�N����															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : BBON1LT/BBON2LT�M�����A�N�e�B�u�ɂȂ鎞�ԍ����p�����[�^�ݒ�l�ȏ�̎��ԍ��̏ꍇ��		*/
/*			�A���[���Ƃ���B����́A�Q�d���M���ُ̈�i�n�[�h�E�F�A�̌̏�A���͐M���̒f���j��		*/
/*			���o���邽�߂̏��u�ł���B�{�����ł́A�Z�[�t�e�B�I�v�V�����J�[�h�̗L���ɂ��			*/
/*			�A���[���R�[�h�i���ޗL/��=A.Eb2/A.Eb1�j����уA���[�����o�ɗp����ݒ�l					*/
/*			�i�J�[�h�L/��= PnE4C/Pn620�j��ύX����B												*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetCheckBbonSignal( CHECK_HWBB_TIMING *pHwbbTmg, BOOL LackOneBbon )
{
	BOOL err;

	err = FALSE;
	/*--------------------------------------------------------------------------------------------------*/
	/*		BBON�M�����̓^�C�~���O�ُ�`�F�b�N����														*/
	/*--------------------------------------------------------------------------------------------------*/
		if( LackOneBbon )
		{
			pHwbbTmg->var.BbonTimeDiffCnt += KPI_SCCYCLEMS;
			if( pHwbbTmg->var.BbonTimeDiffCnt > pHwbbTmg->conf.MaxBbonTimeDiff )
			{
				/* �����ݒ�l�ŃN�����v */
				pHwbbTmg->var.BbonTimeDiffCnt = pHwbbTmg->conf.MaxBbonTimeDiff;

				/* A.Eb1 : ��~�M�����͎��ԍ��ߑ�(�J�[�h��) */
				err = TRUE;
			}
		}
		else		/* <S0E5> */
		{
			pHwbbTmg->var.BbonTimeDiffCnt = 0;
		}
		return err;
}


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



/****************************************************************************************************/
/*																									*/
/*		�Z�[�t�e�B�@�\�@�����M���ُ팟�o���� (Test Pulse Diagnostic)								*/
/*																									*/
/*		Identification																				*/
/*			Function ID		: 0001(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Function version: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.02.14															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S1FF><S201>																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A���v���̓����M��(DPIN1/DPIN2�M��)���p���X�M���łȂ��ꍇ�ُ͈팟�o����					*/
/*			HWBB��Ԃ̏ꍇ�ُ͈팟�o���Ȃ�															*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetTestPulseDiagnostic( TEST_PULSE_DIAG *TestPulseDiag )	/*<S1D1>*/
{
	BOOL err;

	err = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN�M���`�F�b�N�����iBBON=OFF�FHWBB��Ԃ̓`�F�b�N���Ȃ��j									*/
/*--------------------------------------------------------------------------------------------------*/
	if( TestPulseDiag->DpInEnable )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	DPIN�M����4ms�����̃p���X�M���łȂ��ꍇ�ُ͈�											*/
		/*------------------------------------------------------------------------------------------*/
		if( !(TestPulseDiag->DpInState^TestPulseDiag->DpInLastState) )
		{
			TestPulseDiag->DpInDiagState = FALSE;
			if( !((TestPulseDiag->DpInDiagState)^(TestPulseDiag->DpInLastDiagState)) )
			{
				TestPulseDiag->DpInErrCnt++;
				if( (TestPulseDiag->DpInErrCnt) > 2 )
				{
					err = TRUE;
				}
			}
			else
			{
				TestPulseDiag->DpInErrCnt = 0;
			}
		}
		else
		{
			TestPulseDiag->DpInDiagState = TRUE;
			TestPulseDiag->DpInErrCnt = 0;
		}
		TestPulseDiag->DpInLastDiagState = TestPulseDiag->DpInDiagState;
	}
/* <S1D1> >>>>> */
	else if( !(TestPulseDiag->DpInLastEnable) )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	BBON=OFF(HIGH)�̂Ƃ��ADPIN�M����HIGH�łȂ��ꍇ�ُ͈�										*/
		/*------------------------------------------------------------------------------------------*/
		if( TestPulseDiag->DpInState )
		{
			TestPulseDiag->DpInDiagState = FALSE;
			if( !((TestPulseDiag->DpInDiagState)^(TestPulseDiag->DpInLastDiagState)) )
			{
				TestPulseDiag->DpInErrCnt++;
				if( (TestPulseDiag->DpInErrCnt) > 2 )
				{
					err = TRUE;
				}
			}
			else
			{
				TestPulseDiag->DpInErrCnt = 0;
			}
		}
		else
		{
			TestPulseDiag->DpInDiagState = TRUE;
			TestPulseDiag->DpInErrCnt = 0;
		}
		TestPulseDiag->DpInLastDiagState = TestPulseDiag->DpInDiagState;
	}
/* <<<<< <S1D1> */

	return err;
}

BOOL GetHwbbSupportSts( DET_HWBB_STS *DetHwbbSts )
{
	return DetHwbbSts->hwbbsupport;
}


/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LpxSftMdlEncComErrDetect																	*/
/*							  Encoder alarm detection routine										*/
/*							  Processing in ScanB interruption										*/
/*							  Safety-related part													*/
/*																									*/
/*		Identification																				*/
/*			Function ID		: 0002(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Function version: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.02.14															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S0F1>																*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*																									*/
/*		Check Status of Input Signal													<S0F1>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �����M����Ԍ��o�pDPIN1, DPIN2�M���̏�Ԃ����o����										*/
/*          DPIN1, DPIN2�M���L��/������Ԃ����o����													*/
/*																									*/
/*	�� �� :																							*/
/*	�߂�l:																							*/
/*																									*/
/****************************************************************************************************/
void SeqIoDetectTestPulseSignal( SEQ_IO_HNDL *pIoSignals, LONG AxisNo )
{
	TEST_PULSE_DIAG *pTestPulseDiag1;
	TEST_PULSE_DIAG *pTestPulseDiag2;
	DET_HWBB_STS *pDetHwbbSts;

	pTestPulseDiag1 = &(pIoSignals->TestPulseDiag[0]);
	pTestPulseDiag2 = &(pIoSignals->TestPulseDiag[1]);
	pDetHwbbSts    = &(pIoSignals->DetHwbbSts);
/*--------------------------------------------------------------------------------------------------*/
/*		�M���̑O��l���X�V																			*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag1->DpInLastState = pTestPulseDiag1->DpInState;
	pTestPulseDiag2->DpInLastState = pTestPulseDiag2->DpInState;

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1�M���̗L���^������Ԕ��� (BBON��ԂłȂ� && �O��DPIN1�M���L�� �� ����DPIN1�M���͗L��	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon1)) && (pTestPulseDiag1->DpInLastEnable) )
	{
		pTestPulseDiag1->DpInEnable = TRUE;
	}
	else
	{
		pTestPulseDiag1->DpInEnable = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN2�M���̗L���^������Ԕ��� (BBON��ԂłȂ� && �O��DPIN2�M���L�� �� ����DPIN2�M���͗L��	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon2)) && (pTestPulseDiag2->DpInLastEnable) )
	{
		pTestPulseDiag2->DpInEnable = TRUE;
	}
	else
	{
		pTestPulseDiag2->DpInEnable = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2�M���̏�Ԏ擾																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 == AxisNo )
	{
		pTestPulseDiag1->DpInState = GET_DPIN01();
		pTestPulseDiag2->DpInState = GET_DPIN02();
	}
	else
	{
		pTestPulseDiag1->DpInState = GET_DPIN11();
		pTestPulseDiag2->DpInState = GET_DPIN12();
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2���(�O��l)�̍X�V																*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag1->DpInLastEnable  = !(pDetHwbbSts->Bbon1);
	pTestPulseDiag2->DpInLastEnable  = !(pDetHwbbSts->Bbon2);
}


/****************************************************************************/
/* �T�v�F�Z�[�t�e�B�@�\�@�����M���ُ팟�o�p�p���X�o�͏���		<S0F1>		*/
/*       4ms�����p���X�o��(ScanC����)										*/
/****************************************************************************/
/* ����		:	-															*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
void SftReverceTestPls( void )
{
	VULONG	*CheckPostAddr;

	CheckPostAddr = HALdrv_GetCheckPostAddr();
	*CheckPostAddr ^= (1<< SIL3_TEST_PLS0);
#if (CSW_BOARD_TYPE  == SGD7W_TYPE)
	*CheckPostAddr ^=(1<< SIL3_TEST_PLS1);
#endif		/* #if (CSW_BOARD_TYPE  == SGD7W_TYPE) */
}
//void HALdrv_ReverceTestPls( void )
//{
//	*HALdrv_CheckPost ^= (1<< SIL3_TEST_PLS);
//	return;
//}



/*********************************** End of file ****************************************************/
