/****************************************************************************
Description	: Serial Encoder Service for Scan task
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes	      :
	Name   Date          Description
	------------------------------------------------------------------------
	I.M.   2011.02.08    created

****************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"
#include "MLib.h"
#include "KLib.h"
#include "SencScanService.h"	/* <S004> */
#include "JL086ServoIpHw.h"	/* <S004> */
#include "AlarmManager.h"		/* <S018> */
#include "MercuryGlobals.h"	/* <S1B7> */
#include "MotorLessTest.h"	/* <S1B7> */

#define _TIME_BASE_TIMER_//�b��/* <S004> */

#ifdef _TIME_BASE_TIMER_
LONG	LpxTimeBasesMotPosCalc( MENCV *MencV );
#endif /* _TIME_BASE_TIMER_ */


/* �t���N���[�Y�h�G���R�[�_�ʒu�f�[�^��M����(HW:JL-086)	*//* <S04D> */
//static ALMID_T	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg );
static void	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE *AlmManager);/* <S0E0>:AlmManager�ǉ� */
/* �t���N���[�Y�h�G���R�[�_�ʒu�f�[�^��M����(SW)			*//* <S04D> */
static LONG	FencRcvPosSwProcess( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete ); /* <S0E0>:AlmManager�ǉ� */

/* ��]�^�F�V���A���G���R�[�_�ʒu�f�[�^��M����(HW:JL-086) *//* <S004> */ /* <S0C5> */
static void	SencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
															ALMMAN_HANDLE 	*AlmManager );
/* ��]�^�F�V���A���G���R�[�_�ʒu�f�[�^��M����(SW) *//* <S004> */ /* <S0C5> */
static void	SencRcvPosSwProcess( MENCV *MencV, ALMMAN_HANDLE 	*AlmManager,
																	BOOL BeComplete );

/* ���j�A���[�^�F���[�^�d���ʑ��X�V����								*/
static void	LmxUpdateMotPhase( MENCV *MencV, ALMMAN_HANDLE *AlmManager, LONG dMotPos );

/* <S1B7> >>>>> */
/* ���[�^���X�F�}���`�^�[���f�[�^�擾����	*/
static USHORT LpxGetSencMotLessMturnData( MENCV *MencV, MENCPRM *MencP, USHORT PosH, ULONG PosL );
/* ���[�^���X�F�}���`�^�[���f�[�^�ݒ菈��	*/
static void LpxSencMotLessMultiTurnSet(MENCPRM *MencP, ENCODER_DATA *EncData, SHORT MturnLimit);
static void MlibAdd6432( LONG *a, LONG b, LONG *x );	/* �b��Ή�(�֐���INT64���g�p���č쐬) */
/* <<<<< <S1B7> */
/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F�G���R�[�_�b���ʉ߃`�F�b�N����												*/
/*																									*/
/****************************************************************************************************//* <S029> */
void	LmxCheckSencCphasePass( MENCV *MencV )
{
//LONG	lwk;	/*<S0C7>*/
//LONG	remx;	/*<S0C7>*/
LONG	CposX;
LONG	CposChange;
/* <S21B>:Start */
LONG	lwk;
LONG	remx;
SENCX		*Enc;
Enc	= &MencV->EncIf;
/* <S21B>�Fend */

	CposX = 0;				/*<S0C4>*/
	CposChange = FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		���_�ʉ߃`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->CphaseLess == FALSE )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�b���ʉ߃`�F�b�N																		*/
		/*------------------------------------------------------------------------------------------*/
			if( (MencV->RxAlmCode & 0x0035) == 0x0001 )/* �Œ�r�b�g+�I�[�o�[�q�[�g�`�F�b�N	*/
			{
				if( (MencV->RxPosH[0] != MencV->RxPosH[1]) ||
					( (MencV->Cset==FALSE) && ((MencV->RxAlmCode & 0x0040)==0) ) )
				{
					CposChange = TRUE;
					MencV->Cset = TRUE;
					CposX = (LONG)(MencV->RxPosH[0] << HPOS_TO_XPOS);
					CposX = MencV->MposSign *(CposX + MencV->OrgDetDelayComp);
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�b���ʒu�ݒ菈��																		*/
		/*------------------------------------------------------------------------------------------*/
			if( MencV->Cset )
			{
			/*--------------------------------------------------------------------------------------*/
			/*	�b���ʒu���b�`����																	*/
			/*--------------------------------------------------------------------------------------*/
				if( CposChange )
				{
					MencV->IncreCposLatch = TRUE;				/* �b���ʒu���b�`����   --> ScanB	*/
					CposX = ((LONG)(MencV->MotPosX[0] - CposX) >> LMX_MPOS_SFT);	/* �C���N���␳��C���ʒu */
					MencV->MotCposX = MencV->MotPos - CposX;
				}
#if 0
			/*--------------------------------------------------------------------------------------*/
			/*	�p���X�ϊ����_�␳�ݒ菈��															*/
			/*--------------------------------------------------------------------------------------*/
				if( MencP.f.DivOut == TRUE )
				{
					if( CposChange )
					{
						remx = 0;
						lwk = MlibPfbkxremNolim( MencV->MotCposX, MencV->DivOutGain, &remx );
						lwk += MencV->InitDivPosOffset ;											
						ASIC_WR( AREG_POSET1, (USHORT)(lwk - 1) );
						ASIC_WR( AREG_POSET2, (USHORT)(lwk + 0) );
#if	SVFSEL_MICROPROGRAM_DIVOUT == 1
						MencV->DivOutOrgX1 = lwk - 1;				/* �p���X�ϊ����_�␳�P(�ێ��p)	*/
						MencV->DivOutOrgX2 = lwk + 0;				/* �p���X�ϊ����_�␳�Q(�ێ��p)	*/
#endif
					}
				/*----------------------------------------------------------------------------------*/
					lwk = MencV->MotPos - MencV->MotCposX;
					if( (lwk < MencP.DivOutCmskChk) && (lwk > -MencV->DivOutCmskChk) )
					{
						KPI_PCMASK_OFF( );					/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
//						KPI_PCMASK_OFF( RoutV.OutModeFlg );	/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
					}
					else
					{
						KPI_PCMASK_ON( );					/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
					}
				}
#endif
/* <S21B>:Start */
			/*--------------------------------------------------------------------------------------*/
			/*	�p���X�ϊ����_�␳�ݒ菈��															*/
			/*--------------------------------------------------------------------------------------*/
				if(MencV->DivInfo.DivOut != FALSE)
				{
					if(CposChange != FALSE)
					{
						remx = 0;
						lwk = MlibPfbkxremNolim(MencV->MotCposX, MencV->DivInfo.DivOutGain, &remx);
						lwk += MencV->DivInfo.InitDivPosOffset;
						/* �p���X�ϊ����_�␳1 */
						SHalDrv_SetOriginCorrect1( MencV->EncIf.SencSetReg, (lwk - 1) );
						/* �p���X�ϊ����_�␳2 */
						SHalDrv_SetOriginCorrect2( MencV->EncIf.SencSetReg, lwk );
#if	SVFSEL_MICROPROGRAM_DIVOUT == 1
						MencV->DivInfo.DivOutOrgX1 = lwk - 1;				/* �p���X�ϊ����_�␳�P(�ێ��p)	*/
						MencV->DivInfo.DivOutOrgX2 = lwk + 0;				/* �p���X�ϊ����_�␳�Q(�ێ��p)	*/
#endif
					}
					lwk = MencV->MposFromCpos;
					if((lwk < MencV->DivInfo.DivOutCmskChk) && (lwk > -MencV->DivInfo.DivOutCmskChk))
					{	/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
						Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;
						MencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;
					}
					else
					{	/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
						Enc->SencSetReg->AREG_WDT1SET |= BIT8;
						MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
					}
				}
/* <S21B>�Fend */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�b�����X�G���R�[�_(�A�u�\�X�P�[���̏ꍇ)�F�b���}�X�N�n�m�^�n�e�e�����̂ݍs��				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
#if 0
			/*--------------------------------------------------------------------------------------*/
			/*		��Έʒu�����_(=0)�ߖT�łb���}�X�N����������									*/
			/*--------------------------------------------------------------------------------------*/
			if( (MencV->MotAbsPos < MencV->DivOutCmskChk) && (MotAbsPos > -MencV->DivOutCmskChk) )
			{
//				KPI_PCMASK_OFF( RoutV.OutModeFlg );			/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
				KPI_PCMASK_OFF( );							/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
			}
			else
			{
				KPI_PCMASK_ON( );							/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
			}
#endif
/* <S21B>:Start */
			/*--------------------------------------------------------------------------------------*/
			/*		��Έʒu�����_(=0)�ߖT�łb���}�X�N����������									*/
			/*--------------------------------------------------------------------------------------*/
			if( (MencV->MotAbsPos < MencV->DivInfo.DivOutCmskChk) && (MencV->MotAbsPos > -MencV->DivInfo.DivOutCmskChk) )
			{
				Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;     /* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
			}
			else
			{
				Enc->SencSetReg->AREG_WDT1SET |= BIT8;      /* �p���X�ϊ�C���}�X�NON�ݒ�			*/
			}
/* <S21B>�Fend */
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}
/****************************************************************************
*
*		���j�A���[�^�F�X�L�����a�T�[�r�X����(Called from ScanB)
*
****************************************************************************/
BOOL LmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq )
{
	LONG	lwk1;
	LONG	LastMposFromCpos;

	if( MencV->CphaseLess != FALSE )
	{	/*------------------------------------------------------------------*/
		/*	AbsoEncoder : Reset Cphase Pass Signal							*/
		/*------------------------------------------------------------------*/
		MencV->CposOk = TRUE;
		LastMposFromCpos = MencV->MposFromCpos;
		MencV->MposFromCpos = MencV->MotAbsPos;
		lwk1 = LastMposFromCpos ^ MencV->MposFromCpos;
		MencV->MotCphPass = ( lwk1 < 0 ) ? TRUE : FALSE;
	}
	else
	{	/*------------------------------------------------------------------*/
		/*	IncreEncoder : Make MposFromCpos and Motor Cphase Pass Signal	*/
		/*------------------------------------------------------------------*/
		if( MencV->Cset == FALSE )
		{
			MencV->CposOk = FALSE;
			MencV->MotCphPass = FALSE;
		}
		else if( MencV->IncreCposLatch != FALSE )
		{
			MencV->CposOk = TRUE;
			MencV->IncreCposLatch = FALSE;/* <S029> �F�R�����g�A�E�g���� */
			*LatchClrReq = TRUE;
			MencV->MotCphPass = TRUE;

			/* <S029> : MD�d�l����Sigma7�d�l�ɕύX */
			MencV->MposFromCpos = MencV->MotPos - MencV->MotCposX;
//			MencV->MposFromCpos = MencV->CphaseDist;
		}
		else
		{
			MencV->CposOk = TRUE;
			LastMposFromCpos = MencV->MposFromCpos;

			/* <S029> MD�d�l����Sigma7�d�l�ɕύX */
			MencV->MposFromCpos = MencV->MotPos - MencV->MotCposX;
//			MencV->MposFromCpos = MencV->CphaseDist;
			lwk1 = LastMposFromCpos ^ MencV->MposFromCpos;
			MencV->MotCphPass = ( lwk1 < 0 ) ? TRUE : FALSE;
		}
	}

	return MencV->MotCphPass;
}


/****************************************************************************
*
*		��]�^�F�X�L�����a�T�[�r�X����(Called from ScanB)
*
****************************************************************************/
BOOL RmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq )
{
	LONG	lwk1;
	LONG	LastMotAngle;

	/*----------------------------------------------------------------------*/
	/*		Latch Encoder Data for ScanB									*/
	/*----------------------------------------------------------------------*/
//	MencV->ScanBPosH = MencV->RxPosH[0];			/* �X�L�����`�œǍ��񂾃f�[�^���g�p����B	*/
//	MencV->ScanBPosL = MencV->RxPosL[0].dw;

	/*----------------------------------------------------------------------*
	* Calculate Motor Angle, Motor Position from C-Phase, C-Phase Pass Signal
	*-----------------------------------------------------------------------*/
	if( (MencV->Cset != FALSE) && (MencV->CphaseLess == FALSE) )
	{
		/*------------------------------------------------------------------*/
		/*	MencV->MotAngle [pulse] : 0 �` (PulseNo - 1)					*/
		/*------------------------------------------------------------------*/
		LastMotAngle = MencV->MotAngle;
		/* 2011.01.07 Y.Oka �ʃv���O����I/F�ȗ����̂��߁ACphaseDist���g�p����悤�ύX */
#if 1/* <S00C> */
		lwk1 = (LONG)((LONG)((MencV->MotPosX[0]) - (MencV->MotCposX)) >> MencV->MposSftR); 
#else
		lwk1 = (LONG)( MencV->CphaseDist >> MencV->MposSftR ); /* TODO: CphaseDist���쐬�̂��߁AUn003��0�ɂȂ��Ă���!! */
#endif
		MencV->MotAngle = ( lwk1 >= 0 ) ? lwk1 : lwk1 + MencV->PulseNo;

		/*------------------------------------------------------------------*/
		/*	MencV->MposFromCpos [pulse] : (-PulseNo/2) �` (PulseNo/2 - 1)	*/
		/*------------------------------------------------------------------*/
		lwk1 = ( MencV->MotAngle < MencV->HalfPulseNo ) ?
			       MencV->MotAngle : MencV->MotAngle - MencV->PulseNo;

		MencV->MposFromCpos = lwk1;
		/*------------------------------------------------------------------*/
		/*	Make Motor Cphase Pass Signal									*/
		/*------------------------------------------------------------------*/
		/* 2010.08.25 Y.Oka �C���N���G���R�[�_�ڑ����̃��b�`�s��C�� */
		if( MencV->IncreCposLatch != FALSE )
		{ /* First-time CPhase Pass (Incremental Encoder) */
			MencV->CposOk = TRUE;
			MencV->IncreCposLatch = FALSE;/* <S029> �F�R�����g�A�E�g���� */
			*LatchClrReq = TRUE;
			MencV->MotCphPass = TRUE;
		}
		else
		{	/* Already CPhase Pass
				(In case of Absolute Encoder, IncreCposLatch is not TRUE) */
			MencV->CposOk = TRUE;
			lwk1 = MlibLABS( MencV->MotAngle - LastMotAngle );
			MencV->MotCphPass = ( lwk1 > MencV->HalfPulseNo ) ? TRUE : FALSE;
		}
	}
	else
	{
		MencV->CposOk = FALSE;
		MencV->MotCphPass = FALSE;
	}

	return MencV->MotCphPass;
}




/****************************************************************************
*
*		�t���N�G���R�[�_ : �b���ʉ߃`�F�b�N����
*
****************************************************************************/
static void FencCheckCphasePass(MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg)
{
	LONG lwk1;
	LONG remx;
	LONG CposX;
	LONG CposChange;

	CposX = 0;
	if(FencV->CphaseLess == FALSE)
	{
		CposChange = FALSE;
		/*------------------------------------------------------------------*/
		/* �b���ʉ߃`�F�b�N */
		/*------------------------------------------------------------------*/
		if((FencV->RxAlmCode & 0x0035) == 0x0001)
		{
			if( (FencV->RxPosH[0] != FencV->RxPosH[1]) ||
				((FencV->Cset == FALSE) && ((FencV->RxAlmCode & 0x0040) == 0)) )
			{
				CposChange = TRUE;
				FencV->Cset = TRUE;
				CposX = FencV->MposSign *
						(FSX_HposToXpos(FencV->RxPosH[0]) + FencV->OrgDetDelayComp);
			}
		}
		/*------------------------------------------------------------------*/
		/*	�b���ʒu�ݒ菈��												*/
		/*------------------------------------------------------------------*/
		if(FencV->Cset != FALSE)
		{
			/*	�b���ʒu���b�`�������t���N�ʒu���Z	*/
			FencV->CposOk = TRUE;

			if(CposChange)
			{
				FencV->MposFromCpos = (FencV->MotPosX[0] - CposX) >> FencV->MposSftR;
				FencV->MotCposX = FencV->MotPos - FencV->MposFromCpos;
				FencV->MotCphPass = TRUE;
			}
			else
			{
				lwk1 = FencV->MposFromCpos;
				FencV->MposFromCpos = FencV->MotPos - FencV->MotCposX;
				FencV->MotCphPass = ((lwk1 ^ FencV->MposFromCpos) < 0) ? TRUE : FALSE;
			}

			/*	�p���X�ϊ����_�␳�ݒ菈�� */
#if 0 /*<S02D>*/
			if(FencV->DivOut != FALSE)
			{
				if(CposChange != FALSE)
				{
					remx = 0;
					lwk1 = MlibPfbkxremNolim(FencV->MotCposX, FencV->DivOutGain, &remx);
					lwk1 += FencV->InitDivPosOffset;
					AsicHwReg->AREG_POSET1 = (USHORT)(lwk1 - 1);
					AsicHwReg->AREG_POSET2 = (USHORT)(lwk1 + 0);
				}

				lwk1 = FencV->MposFromCpos;
				if((lwk1 < FencV->DivOutCmskChk) && (lwk1 > -FencV->DivOutCmskChk))
				{	/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
					AsicHwReg->AREG_WDT1SET &= ~BIT8;
					FencV->DivCMaskStatus = DIV_CMASK_OFF;
				}
				else
				{	/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
					AsicHwReg->AREG_WDT1SET |= BIT8;
					FencV->DivCMaskStatus = DIV_CMASK_ON;
				}
			}
#else/*<S02D>*/
			if(FencV->DivInfo.DivOut != FALSE)
			{
				if(CposChange != FALSE)
				{
					remx = 0;
					lwk1 = MlibPfbkxremNolim(FencV->MotCposX, FencV->DivInfo.DivOutGain, &remx);
					lwk1 += FencV->DivInfo.InitDivPosOffset;
#if 0/* <S21B>:Start */
					AsicHwReg->AREG_POSET1 = (USHORT)(lwk1 - 1);
					AsicHwReg->AREG_POSET2 = (USHORT)(lwk1 + 0);
#else
					/* �p���X�ϊ����_�␳1 */
					SHalDrv_SetOriginCorrect1( FencV->EncIf.SencSetReg, (lwk1 - 1) );
					/* �p���X�ϊ����_�␳2 */
					SHalDrv_SetOriginCorrect2( FencV->EncIf.SencSetReg, lwk1 );
#endif/* <S21B>�Fend */
				}

				lwk1 = FencV->MposFromCpos;
				if((lwk1 < FencV->DivInfo.DivOutCmskChk) && (lwk1 > -FencV->DivInfo.DivOutCmskChk))
				{	/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
//					AsicHwReg->AREG_WDT1SET &= ~BIT8;
					FencV->EncIf.SencSetReg->AREG_WDT1SET &= ~BIT8; /* <S21B> */
					FencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;
				}
				else
				{	/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
//					AsicHwReg->AREG_WDT1SET |= BIT8;
					FencV->EncIf.SencSetReg->AREG_WDT1SET |= BIT8; /* <S21B> */
					FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
				}
			}
#endif/*<S02D>*/
		}

	}
	else
	{
		/*------------------------------------------------------------------*/
		/*		�b�����X�G���R�[�_(�A�u�\�X�P�[���̏ꍇ)					*/
		/*------------------------------------------------------------------*/
#if 0 /*<S02D>*/
		if(FencV->DivOut != FALSE)
		{
			/*	��Έʒu�����_(=0)�ߖT�łb���}�X�N���������� */
			if( (FencV->MotAbsPos < FencV->DivOutCmskChk) &&
				(FencV->MotAbsPos > -FencV->DivOutCmskChk) )
			{	/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
				AsicHwReg->AREG_WDT1SET &= ~BIT8;
				FencV->DivCMaskStatus = DIV_CMASK_OFF;
			}
			else
			{	/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
				AsicHwReg->AREG_WDT1SET |= BIT8;
				FencV->DivCMaskStatus = DIV_CMASK_ON;
			}
		}
#else/*<S02D>*/
		if(FencV->DivInfo.DivOut != FALSE)
		{
			/*	��Έʒu�����_(=0)�ߖT�łb���}�X�N���������� */
			if( (FencV->MotAbsPos < FencV->DivInfo.DivOutCmskChk) &&
				(FencV->MotAbsPos > -FencV->DivInfo.DivOutCmskChk) )
			{	/* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
//				AsicHwReg->AREG_WDT1SET &= ~BIT8;
				FencV->EncIf.SencSetReg->AREG_WDT1SET &= ~BIT8; /* <S21B> */
				FencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;
			}
			else
			{	/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
//				AsicHwReg->AREG_WDT1SET |= BIT8;
				FencV->EncIf.SencSetReg->AREG_WDT1SET |= BIT8; /* <S21B> */
				FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
			}
		}
#endif/*<S02D>*/
		/*	���_�ʉ߃`�F�b�N */
		lwk1 = FencV->MposFromCpos;
		FencV->MposFromCpos = FencV->MotAbsPos;
		FencV->MotCphPass = ((lwk1 ^ FencV->MposFromCpos) < 0) ? TRUE : FALSE;
	}

}


#if 0
/****************************************************************************
*
*	�t���N�G���R�[�_ : �^�C���x�[�X�^�C�}�𗘗p�������x���Z
*
****************************************************************************/
LONG LpxTimeBasesFencSpdCalc(ENCVFBTIMBASE FbTimeBase, LONG dPos, LONG dPosSpd, UCHAR EnableFlag)
{
	LONG lwk;

	if(EnableFlag == FALSE)
	{
		/*------------------------------------------------------------------*/
		/*	�␳����������													*/
		/*------------------------------------------------------------------*/
		FbTimeBase->Modflag = FALSE; 		/* ���␳���[�h�ݒ�	*/
		FbTimeBase->fencspeed = dPosSpd; 	/* ���␳�̂܂܏o��	*/
	}
	else
	{
		/*------------------------------------------------------------------*/
		/*	�␳�L��������													*/
		/*------------------------------------------------------------------*/
		/*	���x���Z���s����(�Q�p���X�����͒ʏ퍷�����x) */
		FbTimeBase->Modflag = (MlibABS(dPos) < VFBTINMINPLS) ? FALSE : TRUE;

		/*	���x���Z���s */
		/*------------------------------------------------------------------*/
		if(FbTimeBase->BaseTimeInterval != 0)
		{
			/* �ް���ϕω����ԂȂ� -> �ݺ��޴װ��	*/
			/* �x�[�X�^�C�}���g�p�������x���Z */
			lwk = (dPosSpd >> 2) * SAENCCLKNUM;
			lwk = lwk / (LONG) FbTimeBase->BaseTimeInterval;
			FbTimeBase->fencspeed = lwk << 2;
		}
		else
		{
			/* �␳�������B�̏ꍇ�F�G���R�[�_�ʒu�������x���Z�g�p */
			FbTimeBase->fencspeed = dPosSpd;
		}
	}

	/*	����l�ۑ�	*/
	FbTimeBase->LastBaseTimer = FbTimeBase->BaseTimer;
	FbTimeBase->LastModflag = FbTimeBase->Modflag;

	return (FbTimeBase->fencspeed);
}
#endif

/****************************************************************************
*
*		�t���N�G���R�[�_ : �ʐM�ُ폈��
*
****************************************************************************/
static LONG FencComErrorProc(MENCV *FencV, ALMMAN_HANDLE *AlmManager)/* <S0E0>:AlmManager�ǉ� */
{
	LONG FencPosX;
	LONG lwk;

	FencV->SeRxErr = TRUE;	/* Set Error Flag */
	++FencV->ComErrCnt; 	/* Count up ComErrCnt for Monitor	*/

	FencPosX = FencV->MotPosX[0];
	if(FencV->EncDataNG == FALSE)
	{
		if(FencV->MstErrCnt >= FencV->ComErrLimit)
		{
			FencV->EncDataNG = TRUE; 							/* �G���R�[�_�f�[�^����	*/
			/* <S0E0>:�A���[������ */
			ALMSetServoAlarm( AlmManager, ALM_FPGCOM );		/* A.CF1:�e�o�f�ʐM�ُ�			*/
			ALMSetMask( AlmManager, ALM_FPGCOMTIMER, TRUE );	/* A.CF2:�e�o�f�ʐM�^�C�}�ُ�	*/
		}
		else
		{
			++FencV->MstErrCnt;

			lwk = FencPosX - FencV->MotPosX[1];
			FencPosX += lwk;
		}
	}
	return FencPosX;
}

/***************************************************************************
*
*		�t���N�G���R�[�_ : �ʒu�f�[�^�Ǎ���
*
****************************************************************************//* <S04D> */
LONG FencReadPosition(MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	/* <S0E0>:AlmManager�ǉ� */
						void *BaseLoops)	/*<S1B7>*/
{
	LONG	dFencPos, FencPosX;
	LONG	FencSpd;
//	ALMID_T	err_code;	/*<S1B7> ���g�p*/

	ASIC_HW_IF_STR *sencSetReg;
	ASIC_HW_IF_STR *posCmpSetReg;

	sencSetReg = FencV->EncIf.SencSetReg;
	posCmpSetReg = FencV->EncIf.PosCmpSetReg;
	/*----------------------------------------------------------------------*/
	/*	���ڑ����															*/
	/*----------------------------------------------------------------------*/
	if((FencV->EncConnect == FALSE) || (FencV->EncDisable != FALSE))
	{
		return (0);
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�ω������^�C�}�ɂ��G���R�[�_�ʒu�␳����													*/
/*--------------------------------------------------------------------------------------------------*/
	if( FencV->PosCompMode == POSCOMP_SW )/*<S0C5>rename*/
	{	/*------------------------------------------------------------------------------------------*/
		/*		SW�ɂăG���R�[�_�ʒu�␳(��-�X�݊�)													*/
		/*------------------------------------------------------------------------------------------*/
		FencPosX = FencRcvPosSwProcess( FencV, AlmManager, BeComplete );	/* <S0E0>:AlmManager�ǉ� */
	}
	else
	{
//		err_code = FencRcvPosHwProcess( FencV, posCmpSetReg );/*<S0C5>rename*/
		FencRcvPosHwProcess( FencV, posCmpSetReg, AlmManager );/*<S0C5>rename*//* <S0E0>:AlmManager�ǉ� */
		/*------------------------------------------------------------------------------------------*/
		/*		JL-086�ɂăG���R�[�_�ʒu�␳														*/
		/*------------------------------------------------------------------------------------------*/
		if( FencV->PosCompMode == POSCOMP_ASIC )
		{
			FencPosX = ( SHal_GetCurMotPos( posCmpSetReg ) << FencV->MposSftR );
		}
		/*------------------------------------------------------------------------------------------*/
		/*		�G���R�[�_�ʒu�␳�Ȃ�																*/
		/*------------------------------------------------------------------------------------------*/
		else
		{
			FencPosX = ( SHal_GetCurRcvPos( posCmpSetReg ) << FencV->MposSftR );
		}
	}
	/*----------------------------------------------------------------------*/
	/*	�ʒu�������Z														*/
	/*----------------------------------------------------------------------*/
	dFencPos = (FencPosX - FencV->MotPosX[0]) >> FencV->MposSftR;
	FencV->ABphCntr = (UCHAR)(FencPosX >> FencV->MposSftR);

	FencV->MotPosX[1] = FencV->MotPosX[0];
	FencV->MotPosX[0] = FencPosX;
	/*----------------------------------------------------------------------*/
	/*	�O��PG���x���Z														*/
	/*----------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FencSpd = dFencPos * FencV->KConvLoadSpd;
#else
	FencSpd = MlibMulgain(dFencPos, FencV->KConvLoadSpd);
#endif /* FLOAT_USE */
#if 0	/* ���ω������^�C�}�ɂ�鑬�x�␳�͌��󖢑Ή��� */
	FencV->MotSpd = LpxTimeBasesFencSpdCalc(dFencPos, FencSpd, FencV->SpdCmpEnable);
	FencV->SpdCmpEnable = TRUE;
#else
	FencV->MotSpd = FencSpd;
#endif
	/*----------------------------------------------------------------------*/
	/*	�ʒu���Z [32bitRingPulse]											*/
	/*----------------------------------------------------------------------*/
	FencV->MotPos += dFencPos;
	FencV->MotAbsPos += dFencPos;

	/*----------------------------------------------------------------------*/
	/*	�����C���N���p���X�o�͏��� or �b���ʉ߃`�F�b�N����					*/
	/*----------------------------------------------------------------------*/
#if 0
	if(FencV->IncPulseReq != FALSE)
	{
		/*--------------------------------------------------------------------
		* �d���������̏��������Ƀ��[�^���ړ����Ă����ꍇ�̕����p���X�o�͑΍�
		*	ScanA������1��ڂ́A�J�E���^�v���Z�b�g�ƃp���X�ϊ��ʒu���Z�b�g����B
		*-------------------------------------------------------------------*/
		if(FencV->AbsoEncType == MENC_INCRE_TYPE)
		{
			/* �b���ʉ߃`�F�b�N���� */
			if(FencV->SeRxErr == FALSE)
			{
				FencCheckCphasePass();
			}
			/*	�����p���X�o�͏���	(���O�ɃJ�E���^�v���Z�b�g��ݒ�)*/
			if(FencV->DivOut == TRUE)
			{
				FencV->DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivOutGain, &FencV->DivOutRem);
				sencSetReg->AREG_POTEST = (USHORT)FencV->DivOutPos;
				sencSetReg->AREG_POSET0 = (USHORT)FencV->DivOutPos;
			}
			FencV->IncPulseReq = FALSE;
		}
		else
		{
			LpxOutFencIniIncPulse(); /* �����C���N���p���X�o�͏���	*/
		}
	}
	else
//#endif /*<S02D>�폜*/
	{
		/* �b���ʉ߃`�F�b�N���� */
		if(FencV->SeRxErr == FALSE)
		{
			FencCheckCphasePass(FencV, sencSetReg);
		}
		/* �����p���X�o��*/
		if(FencV->DivOut != FALSE)
		{
			FencV->DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivOutGain, &FencV->DivOutRem);
			sencSetReg->AREG_POSET0 = (USHORT) FencV->DivOutPos;
		}
	}
#else/*<S02D>*/
	if(FencV->DivInfo.IncPulseReq != FALSE)
	{
		/*----------------------------------------------------------------------*/
		/* �d���������̏��������Ƀ��[�^���ړ����Ă����ꍇ�̕����p���X�o�͑΍�   */
		/*	ScanA������1��ڂ́A�J�E���^�v���Z�b�g�ƃp���X�ϊ��ʒu���Z�b�g����B*/
		/*----------------------------------------------------------------------*/
		if(FencV->AbsoEncType == MENC_INCRE_TYPE)
		{
			/* �b���ʉ߃`�F�b�N���� */
			if(FencV->SeRxErr == FALSE)
			{
				FencCheckCphasePass(FencV, sencSetReg);
			}
			/*	�����p���X�o�͏���	(���O�ɃJ�E���^�v���Z�b�g��ݒ�)*/
			if(FencV->DivInfo.DivOut == TRUE)
			{
				FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
				sencSetReg->AREG_POTEST = (USHORT)FencV->DivInfo.DivOutPos;
				sencSetReg->AREG_POSET0 = (USHORT)FencV->DivInfo.DivOutPos;
			}
			FencV->DivInfo.IncPulseReq = FALSE;
		}
		else
		{
			/*----------------------------------------------------------------------*/
			/*	�����C���N���p���X�o�͏���	  									    */
			/*----------------------------------------------------------------------*/
			FencSencOutIniIncPulse( FencV, sencSetReg );
		}
	}
	else
	{
		/* �b���ʉ߃`�F�b�N���� */
		if(FencV->SeRxErr == FALSE)
		{
			FencCheckCphasePass(FencV, sencSetReg);
		}
		/* �����p���X�o��*/
		if( (FencV->DivInfo.DivOut != FALSE) && (FencV->DivInfo.IncPulseReq == FALSE))
		{
			FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
			sencSetReg->AREG_POSET0 = (USHORT) FencV->DivInfo.DivOutPos;
		}
	}

#endif/*<S02D>*/
	return (dFencPos);
}


/****************************************************************************************************/
/*																									*/
/*		�t���N���[�Y�h�G���R�[�_�ʒu�f�[�^��M����(HW:JL-086)										*/
/*																									*/
/****************************************************************************************************/
//static ALMID_T	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg)/* <S0E0> */
static void	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE *AlmManager)/* <S0E0> */
{
//	ALMID_T		err_code;/* <S0E0> */

//	err_code = NO_ALARM;									/* ���[�J���ϐ�������					*//* <S04D> */
	/*----------------------------------------------------------------------------------------------*/
	/*		�A���[�����̏���																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->SeRxErr   = SHal_GetSeRxErr( pAsicHwReg );		/* ��M�G���[�t���O�X�V 				*/
	MencV->EncDataNG = SHal_GetEncDataNG( pAsicHwReg );		/* �G���R�[�_�f�[�^�ُ�t���O�̍X�V 	*/
	/*----------------------------------------------------------------------------------------------*/
	if( SHal_GetComAlarm( pAsicHwReg ) )					/* �G���[��ʔ���(�ʐM�^�����x)			*/
	{
//		err_code = ALM_FPGCOM;								/* A.CF1 : �G���R�[�_�ʐM�ُ�			*//* <S0E0> */
		ALMSetServoAlarm( AlmManager, ALM_FPGCOM );			/* A.CF1:�e�o�f�ʐM�ُ�					*/
		ALMSetMask( AlmManager, ALM_FPGCOMTIMER, TRUE );	/* A.CF2:�e�o�f�ʐM�^�C�}�ُ�			*/
	}
	if( SHal_GetAccAlarm( pAsicHwReg ) )					/* �G���[��ʔ���(�ʐM�^�����x)			*/
	{
//		err_code = ALM_FPGCOM;								/* A.CF1 : �G���R�[�_�ʐM�ُ�(�����x)	*//* <S0E0> */
		ALMSetServoAlarm( AlmManager, ALM_FPGCOM );			/* A.CF1:�e�o�f�ʐM�ُ�					*/
		ALMSetMask( AlmManager, ALM_FPGCOMTIMER, TRUE );	/* A.CF2:�e�o�f�ʐM�^�C�}�ُ�			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�����ʒu�f�[�^�X�V																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RcvPosX[2] = MencV->RcvPosX[1];			/*<S0C5>*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];			/*<S0C5>*/
	MencV->RcvPosX[0] = (LONG)( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
	/*----------------------------------------------------------------------------------------------*/
	/*		��M�f�[�^L,H�X�V																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RxPosL[1].Long = MencV->RxPosL[0].Long;	/*<S0C5>*/
	MencV->RxPosH[1] 	  = MencV->RxPosH[0];		/*<S0C5>*/
	MencV->RxPosL[0].Long = (LONG) SHal_GetCurRxPosL( pAsicHwReg );
	MencV->RxPosH[0] 	  = (SHORT)SHal_GetCurRxPosH( pAsicHwReg );
	/*----------------------------------------------------------------------------------------------*/
	SHal_SrcClr( pAsicHwReg );								/* ��M�����t���O�X�V					*/
	/*----------------------------------------------------------------------------------------------*/
//	return err_code;/* <S0E0> */
	return;
}
/****************************************************************************************************/
/*																									*/
/*		�t���N���[�Y�h�G���R�[�_�ʒu�f�[�^��M����(SW)												*/
/*																									*/
/****************************************************************************************************/
static LONG	FencRcvPosSwProcess( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete ) /* <S0E0>:AlmManager�ǉ� */
{
	ULONG	RxFlg;
	LONG	FencPosX;
	USHORT	RxPosH_wk;
	USHORT	BaseTime;
	SENCX	*Enc;
	Enc = &FencV->EncIf;
	
	RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
	if( ((RxFlg & 0x00008000) == 0) /*&& (FencV->SeRxComp == FALSE)*/ )
	{
		/* ��M������ : �t���N�G���R�[�_��M�ُ폈�� */
		FencPosX = FencComErrorProc(FencV, AlmManager);
	}
	else
	{
		/* ��M������ : �t���N�G���R�[�_��M�������� */
		FencV->SeRxComp = FALSE; /* Reset Rx Complete Flag			*/

		/*------------------------------------------------------------------*/
		/*	��M�f�[�^�擾													*/
		/*------------------------------------------------------------------*/
		SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&Enc->RxData );

		/*------------------------------------------------------------------*/
		/*	��M�f�[�^������ϐ��ɐݒ�										*/
		/*------------------------------------------------------------------*/
//		RxPosL_wk = FencV->RxPosL;
		RxPosH_wk = (USHORT)FencV->RxPosH[0];
		FencV->RxPosH[1] = (LONG)RxPosH_wk;

		FencV->RxAlmCode 	= Enc->RxData.UsBuf[2];		/* Alarm Code		*/
		FencV->RxTimex.w.l 	= Enc->RxData.UsBuf[3];		/* TimeBase, etc	*/
		FencV->RxTimex.w.h 	= Enc->RxData.UsBuf[4];		/* TimeBase, etc	*/
		FencV->RxPosL[0].w.l= Enc->RxData.UsBuf[5];		/* Position Low		*/
		FencV->RxPosL[0].w.h= Enc->RxData.UsBuf[6];		/* Position High	*/
		FencV->RxPosH[0] 	= Enc->RxData.UsBuf[7];		/* Incre. Compen.	*/

		/*--------------------------------------------------------------------
		*  �x�[�X�^�C�}���O��l�Ɠ����ꍇ�A�ʐM�ُ�Ƃ���
		*--------------------------------------------------------------------*/
		BaseTime = (USHORT)(FencV->RxTimex.dw >> 8);
		if(FencV->FbTimeBase.BaseTimer == BaseTime)
		{
			/*--------------------------------------------------------------*/
			/*	�ʐM�ُ펞													*/
			/*--------------------------------------------------------------*/
			FencPosX = FencComErrorProc(FencV, AlmManager);
		}
		else
		{
			/*--------------------------------------------------------------*/
			/*	�ʐM���펞													*/
			/*--------------------------------------------------------------*/
			FencV->MstErrCnt = 0;
			FencV->SeRxErr = FALSE;
			FencV->EncDataNG = FALSE;

			FencPosX = FencV->MposSign *
					((FencV->RxPosL[0].Long >> FencV->MposSftR) << FencV->MposSftR);
		}

		FencV->FbTimeBase.BaseTimer = BaseTime;
	}
	
	return FencPosX;
}
/****************************************************************************************************/
/*																									*/
/*		�V���A���G���R�[�_�ʐM�ُ폈��																*/
/*																									*/
/****************************************************************************************************/
static void	LpxSencComErrorProc( MENCV *MencV, ALMMAN_HANDLE *AlmManager, LONG RcvPosX, BOOL AccErr )/*<S0C5>*/
{

/*--------------------------------------------------------------------------------------------------*/
/*		�A���[�����̏����F�A���[���ݒ�																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->MstErrCnt > MencV->ComErrLimit )				/* �G���[�J�E���^�`�F�b�N				*/
	{
		if( MencV->ComErrCnt >= MencV->AccErrCnt )			/* �G���[��ʔ���(�ʐM�^�����x)			*/
		{
			MencV->EncDataNG = TRUE;						/* �G���R�[�_�f�[�^����					*/
			ALMSetServoAlarm( AlmManager, ALM_PGCOM );		/* A.C90 : �G���R�[�_�ʐM�ُ�			*//*<S0C5>*/
		}
		else
		{
			MencV->EncDataNG = TRUE;						/* �G���R�[�_�f�[�^����					*/
			ALMSetServoAlarm( AlmManager, ALM_PGCOM1 );		/* A.C91 : �G���R�[�_�ʐM�ُ�(�����x)	*//*<S0C5>*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		��A���[�����̏����F�O��l�ŕ�ԉ��Z														*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->RcvPosX[2]  = MencV->RcvPosX[1];				/* �O�X��l�X�V							*/
		MencV->RcvPosX[1]  = MencV->RcvPosX[0];				/* �O��l�X�V							*/
		if( AccErr )
		{
			/* �����x�ُ펞�͕�Ԃ��Ȃ� */
			MencV->RcvPosX[0] = RcvPosX;
		}
		else
		{
			/* ����l�͕�ԉ��Z */
			MencV->RcvPosX[0] += (MencV->RcvPosX[1] - MencV->RcvPosX[2]);
		}
		MencV->MstErrCnt++;									/* Count up Error Counter				*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return;/*<S0C5>*/
}


/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_���_�ʉ߃`�F�b�N													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A�u�\�C���N���y�уC���N�����Ɍ��_�ʉ߂��`�F�b�N���A���_�ʒu�̐ݒ���s���Ƌ��ɁA		*/
/*			�����p���X�̏o�͂��s���B																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/* ���֐����̕ύX�v Y.Oka�� */
static void	RmxCheckSencOriginPass( MENCV *MencV )
{
	LONG	Orgx;
	LONG	Zsig;
	LONG	wk1, wk2, wk3;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ����_�ݒ聕�b���}�X�N��������															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->Cset == FALSE )
	{
		if( (MencV->RxAlmCode & 0x0035)!=0x0001 )			/* �Œ�r�b�g�`�F�b�N(bit0,2,4,5)	*/
		{
			MencV->RxPosH[0] = MencV->RxPosH[1];			/* ����̃C���N���␳�l�͎���Ɏ���	*/
		}														/* �z�����A�O��l���g��				*/
		else
		{
			if( MencV->Sigma3Encoder )						/* �G���R�[�_��ʃ`�F�b�N			*/
			{
				Zsig = (MencV->RxPosH[0] & 0x0001);			/* ���V�F���_�M��					*/
			}
			else
			{
				Zsig = (MencV->RxPosL[0].w.l & 0x0100);		/* ���U�F���_�M��					*/
			}
			Orgx = (MencV->RxPosH[0] >> MencV->IncreCompSft);		/* �C���N���␳��			*/
		/*------------------------------------------------------------------------------------------*/
		/*	���_���ʉ�Flag == 0  or �C���N���␳�ʕω�												*/
		/*------------------------------------------------------------------------------------------*/
			if( ((MencV->RxAlmCode & 0x0040) == 0) ||
				(Orgx != (MencV->RxPosH[1] >> MencV->IncreCompSft)) )
			{
				MencV->Cset = TRUE;							/* �b���ݒ芮��						*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	���_�M�� = 1 and ���ʃJ�E���^�����_�t��													*/
		/*------------------------------------------------------------------------------------------*/
			else if( Zsig != 0 )
			{
				if( (MencV->RxPosL[0].w.h & 0x000c) == 0x0000 )
				{
					MencV->Cset = TRUE;						/* �b���ݒ芮��						*/
					Orgx = MencV->RxPosL[0].w.h >> 4;
				}
				else if( (MencV->RxPosL[0].w.h & 0x000c) == 0x000c )
				{
					MencV->Cset = TRUE;						/* �b���ݒ芮��						*/
					Orgx = (MencV->RxPosL[0].w.h >> 4) + 1;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�ʑ����_�ݒ聕�b���}�X�N��������														*/
		/*------------------------------------------------------------------------------------------*/
			if( MencV->Cset )
			{
				MencV->MotOrgX = MencV->MposSign * (Orgx << MencV->MorgSft);
			/*--------------------------------------------------------------------------------------*/
				MencV->IncreCposLatch = TRUE;				/* �b���ʒu���b�`����   --> ScanB	*/
				MencV->MotCposX = MencV->MotOrgX;			/* �b���ʒu���b�`�f�[�^ --> ScanB	*/
			/*--------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
				if( MencV->DivOut == TRUE )					/* ���[�^�G���R�[�_�����o�͑I���H	*/
				{
					/* �C���N���p���_�o�͐ݒ� */
					Orgx = MencV->MotOrgX >> MencV->DivOutSft;
					/* �p���X�ϊ����_�␳1 */
					SHalDrv_SetOriginCorrect1( MencV->EncIf.SencSetReg, (Orgx - 1) );	/* <S04D> �����ύX */
					/* �p���X�ϊ����_�␳2 */
					SHalDrv_SetOriginCorrect2( MencV->EncIf.SencSetReg, Orgx );		/* <S04D> �����ύX */
					/* ����C���}�X�NOFF */
					MencV->DivCMaskStatus  = DIV_CMASK_OFF;
				}
#else/*<S02D>*/
				if( MencV->DivInfo.DivOut == TRUE )			/* ���[�^�G���R�[�_�����o�͑I���H	*/
				{
					/* �C���N���p���_�o�͐ݒ� */
					Orgx = MencV->MotOrgX >> MencV->DivInfo.DivOutSft;
					/* �p���X�ϊ����_�␳1 */
					SHalDrv_SetOriginCorrect1( MencV->EncIf.SencSetReg, (Orgx - 1) );
					/* �p���X�ϊ����_�␳2 */
					SHalDrv_SetOriginCorrect2( MencV->EncIf.SencSetReg, Orgx );
					/* ����C���}�X�NOFF */
					MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF;
					/* �p���X�ϊ�C���}�X�NOFF�ݒ�	*/
					MencV->EncIf.SencSetReg->AREG_WDT1SET &= ~BIT8;							/*<S02D>*/
				}
#endif/*<S02D>*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�b���ʉߐݒ菈��																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	if( MencV->Cset )
	{
		wk1 = (MencV->MotPosX[0] - MencV->MotOrgX);
		wk2 = (MencV->MotPosX[1] - MencV->MotOrgX);
		wk3 = wk1 >> MencV->DivOutSft;							/* ���񕪎����_��ʒu			*/
	/*----------------------------------------------------------------------------------------------*/
		wk2 = (wk1 ^ wk2) & 0xc0000000;
		wk1 = wk1 & 0xc0000000;
		if( (wk2 == 0xc0000000) && ((wk1 == 0x00000000) || (wk1 == 0xc0000000)) )
		{
			MencV->Cpass = TRUE;								/* �b���ʉߐݒ�(�ɐ����])			*/
			MencV->DivCpassPos = wk3;						/* �b���ʉߎ������ʒu�ۑ�			*/
		}
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivCpass == FALSE )						/* �������_�ʉ߃`�F�b�N				*/
		{
			if( MencV->Cpass )
			{
				if( ((MencV->DivCpassPos >= 0) && (wk3 >  3)) ||
					((MencV->DivCpassPos <  0) && (wk3 < -4))	)
				{
					MencV->DivCpass = TRUE;					/* �������_�ʉߐݒ�					*/
				}
			}
		}
		else
		{
			MencV->OrgOk = TRUE;								/* ���_�ݒ芮��						*/
		}
	}
#else/*<S02D>*/
	if( MencV->Cset )
	{
		wk1 = (MencV->MotPosX[0] - MencV->MotCposX);
		wk2 = (MencV->MotPosX[1] - MencV->MotCposX);
		wk3 = wk1 >> MencV->DivInfo.DivOutSft;					/* ���񕪎����_��ʒu			*/
	/*----------------------------------------------------------------------------------------------*/
		wk2 = (wk1 ^ wk2) & 0xc0000000;
		wk1 = wk1 & 0xc0000000;
		if( (wk2 == 0xc0000000) && ((wk1 == 0x00000000) || (wk1 == 0xc0000000)) )
		{
			MencV->Cpass = TRUE;								/* �b���ʉߐݒ�(�ɐ����])			*/
			MencV->DivInfo.DivCpassPos = wk3;					/* �b���ʉߎ������ʒu�ۑ�			*/
			if( (MencV->DivInfo.DivOut == TRUE) && (MencV->DivInfo.DivPass == FALSE) )
			{
				MencV->EncIf.SencSetReg->AREG_DIVSET |= (BIT11 | BIT8);/* �����b���o�͑I���Q			*/
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivCpass == FALSE )						/* �������_�ʉ߃`�F�b�N			*/
		{
			if( MencV->Cpass )
			{
				if( ((MencV->DivInfo.DivCpassPos >= 0) && (wk3 >  3)) ||
					((MencV->DivInfo.DivCpassPos <  0) && (wk3 < -4))	)
				{
					MencV->DivInfo.DivCpass = TRUE;					/* �������_�ʉߐݒ�				*/
				}
			}
		}
		else
		{
			if( MencV->DivInfo.DivOut == TRUE )					/* ���[�^�G���R�[�_�����o�͑I���H	*/
			{ 
				MencV->EncIf.SencSetReg->AREG_DINCSET &= ~(BIT10); /* �����펞�N���A�ݒ�OFF 			*/
			}
			MencV->OrgOk = TRUE;								/* ���_�ݒ芮��						*/
		}
	}
#endif/*<S02D>*/

}

/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�ʒu�f�[�^�Ǎ���													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �V���A���G���R�[�_����`�r�h�b����M�����ʒu�f�[�^���A�`�r�h�b���W�X�^����				*/
/*			�Ǎ��݁A���[�^�G���R�[�_�ϐ�(MencV)�ɐݒ肷��B											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	SencReadPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	void *BaseLoops )/* <S018> *//*<S0C5>*//*<S1B7>*/
{
	LONG		MotPosX;
	LONG		LastMotPosX;
	LONG		dMotPos;
	SENCX		*Enc;
	LONG		lwk1;  		/*<S02D>*/
	LONG		DivMotPosX; /*<S079>*/

	LastMotPosX	= MencV->MotPosX[0];
	Enc			= &MencV->EncIf;

/*--------------------------------------------------------------------------------------------------*/
/*		���ڑ����																					*/
/*--------------------------------------------------------------------------------------------------*/
	if((MencV->EncConnect == FALSE) || (MencV->EncDisable != FALSE))
	{
//		LpxSencComErrorProc(MencV, AlmManager, 0, FALSE);											  /* <S0DD> */
		MotPosX    = ( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
		DivMotPosX = SHal_GetCurRcvPos( pAsicHwReg );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�ω������^�C�}�ɂ��G���R�[�_�ʒu�␳����													*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		switch(MencV->PosCompMode)
		{
		/*------------------------------------------------------------------------------------------*/
		/*		JL-086�ɂăG���R�[�_�ʒu�␳(�ʏ폈��)												*/
		/*------------------------------------------------------------------------------------------*/
		case POSCOMP_ASIC:
			SencRcvPosHwProcess( MencV, pAsicHwReg, AlmManager );
			MotPosX    = ( SHal_GetCurMotPos( pAsicHwReg ) << MencV->MposSftR );
			DivMotPosX = SHal_GetCurMotPos( pAsicHwReg );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		SW�ɂăG���R�[�_�ʒu�␳(��-�X�݊�)													*/
		/*------------------------------------------------------------------------------------------*/
		case POSCOMP_SW:
			SencRcvPosSwProcess( MencV, AlmManager, BeComplete );
			MotPosX    = LpxTimeBasesMotPosCalc( MencV );
			DivMotPosX = SHal_GetCurRcvPos( pAsicHwReg );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		�G���R�[�_�ʒu�␳�Ȃ�																*/
		/*------------------------------------------------------------------------------------------*/
		case POSCOMP_NOP:
		default:
			SencRcvPosHwProcess( MencV, pAsicHwReg, AlmManager );
			MotPosX    = ( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
			DivMotPosX = SHal_GetCurRcvPos( pAsicHwReg );
			break;
		}
	}
#if 0
	if( MencV->PosCompCheck == POSCOMP_SW )
	{	/*------------------------------------------------------------------------------------------*/
		/*		SW�ɂăG���R�[�_�ʒu�␳(��-�X�݊�)													*/
		/*------------------------------------------------------------------------------------------*/
		err_code = SencRcvPosSwProcess( MencV, BeComplete );
		MotPosX  = LpxTimeBasesMotPosCalc( MencV );
	}
	else
	{
		err_code = SencRcvPosHwProcess( MencV, pAsicHwReg );
		/*------------------------------------------------------------------------------------------*/
		/*		JL-086�ɂăG���R�[�_�ʒu�␳														*/
		/*------------------------------------------------------------------------------------------*/
		if( MencV->PosCompCheck == POSCOMP_ASIC )
		{
			MotPosX = ( SHal_GetCurMotPos( pAsicHwReg ) << MencV->MposSftR );
			DivMotPosX = SHal_GetCurMotPos( pAsicHwReg );  /*<S079>*/
		}
		/*------------------------------------------------------------------------------------------*/
		/*		�G���R�[�_�ʒu�␳�Ȃ�																*/
		/*------------------------------------------------------------------------------------------*/
		else
		{
			MotPosX = ( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
		}
	}
#endif
	/*----------------------------------------------------------------------------------------------*/
	/* �␳��ʒu�f�[�^�X�V 																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->MotPosX[1]	= MencV->MotPosX[0];				/* �O��␳��ʒu�f�[�^					*/
	MencV->MotPosX[0]	= MotPosX;							/* ����␳��ʒu�f�[�^					*/
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�ʒu�o��(32bitRingPulse)																*/
/*--------------------------------------------------------------------------------------------------*/
	dMotPos = ( (MencV->MotPosX[0] - LastMotPosX) >> MencV->MposSftR );
	MencV->MotPos   = MencV->MotPos + dMotPos;

	if( MencV->AxisMotType == MOTTYPE_LINEAR )/*<S017>*/
	{ /* ���j�A�^ */
		LmxUpdateMotPhase( MencV, AlmManager, dMotPos );
	}
	else
	{ /* ��]�^ */
		MencV->MotPhase = (USHORT)(( (((MencV->MotPosX[0]) - (MencV->MotOrgX)) >>8) * MencV->Kmotphase)>>8);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�t�B�[�h�o�b�N�p���X�o�͏���																*/
/*--------------------------------------------------------------------------------------------------*/
	// �����C���N���o�͂Ȃ�
#if 0/*<S02D>*/
	if( (MencV->OrgOk == FALSE) && (MencV->SeRxErr == FALSE) )
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )/* <S029> : ���j�A�����ǉ� */
		{ /* ���j�A�^ */
			LmxCheckSencCphasePass(MencV);					/* ���_�ʉ߃`�F�b�N						*/
		}
		else
		{ /* ��]�^ */
			RmxCheckSencOriginPass(MencV);					/* ���_�ʉ߃`�F�b�N						*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͏���																			*/
/*--------------------------------------------------------------------------------------------------*/
	// �����C���N���o�͂Ȃ�
	if( MencV->DivOut == TRUE )
	{
		SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivOutSft) );
	}
#else/*<S02D>*/
	if( MencV->DivInfo.IncPulseReq == TRUE )				/* �����C���N���o�͗v���H				*/
	{
		/*------------------------------------------------------------------------------------------*/
		/* �d���������̏��������Ƀ��[�^���ړ����Ă����ꍇ�̕����p���X�o�͑΍�		     	        */
		/*		ScanA������1��ڂ́A�J�E���^�v���Z�b�g�ƃp���X�ϊ��ʒu���Z�b�g����B		        */
		/*------------------------------------------------------------------------------------------*/
		if( MencV->AbsoEncType == MENC_INCRE_TYPE )
		{
			if( MencV->SeRxErr == FALSE )
			{
				/*---------------------------------------------------------------------------------*/
				/*		���_�ʉ߃`�F�b�N����													   */
				/*---------------------------------------------------------------------------------*/
				if( MencV->AxisMotType == MOTTYPE_LINEAR )/*���j�A�����R��ǉ� <S040> */		/* TODO ���������̒��ɃC���N�����[�h�̕��򂪂���̂͂��������I�I�v�C��*/
				{ /* ���j�A�^ */
					LmxCheckSencCphasePass(MencV);
				}
				else
				{ /* ��]�^ */
					RmxCheckSencOriginPass(MencV);
				}
			}
			/*-------------------------------------------------------------------------------------*/
			/*		�����p���X�o�͏���	(���O�ɃJ�E���^�v���Z�b�g��ݒ�)						   */
			/*-------------------------------------------------------------------------------------*/
			if( MencV->DivInfo.DivOut == TRUE )
			{
#if 0/*<S079>*/
				Enc->SencSetReg->AREG_POTEST = (ULONG)(MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft);
				SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
#else/*<S079>*/
				if(MencV->DivInfo.DivOutASIC != FALSE)
				{ /* JL-086�ɂĕ����̏ꍇ */
					Enc->SencSetReg->AREG_POTEST = (ULONG) DivMotPosX;
					SHalDrv_SetDivOutPosition( Enc->SencSetReg, DivMotPosX );
				}
				else
				{
#if 0 /* <S21B>:Start */
					Enc->SencSetReg->AREG_POTEST = (ULONG)(MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft);
					SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
#else
					if( MencV->AxisMotType == MOTTYPE_LINEAR )
					{ /* ���j�A�^ */
						MencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dMotPos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem);
						SHalDrv_SetDivOutPosition( Enc->SencSetReg, (ULONG)MencV->DivInfo.DivOutPos );
					}
					else
					{ /* ��]�^ */
						Enc->SencSetReg->AREG_POTEST = (ULONG)(MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft);
						SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
					}
#endif/* <S21B>�Fend */
				}
#endif/*<S079>*/
			}
			MencV->DivInfo.IncPulseReq = FALSE;
		}
		else
		{
			/*-------------------------------------------------------------------------------------*/
			/*		�����C���N���p���X�o�͏���												       */
			/*-------------------------------------------------------------------------------------*/
			if( MencV->AxisMotType == MOTTYPE_LINEAR )/* <S209> : ���j�A�����ǉ� */
			{ /* ���j�A�^ */
				LmxOutSencIniIncPulse(MencV, pAsicHwReg, LMX_MPOS_SFT);
			}
			else
			{ /* ��]�^ */
				SencOutIniIncPulse( MencV, pAsicHwReg );
			}
			if((MencV->DivInfo.DivOutASIC != FALSE) && (MencV->DivInfo.IncPulseReq == FALSE))  /*<S079>*/
			{ /* JL-086�ɂĕ����̏ꍇ�A�����C���N���p���X�o�͊�����JL-086�ɂĕ������� */
				Enc->SencSetReg->AREG_DIVSET &= ~BIT1;
			}
		}
	}
	else if( (MencV->OrgOk == FALSE) && (MencV->SeRxErr == FALSE) )
	{
		/*------------------------------------------------------------------------------------------*/
		/*		���_�ʉ߃`�F�b�N����												                */
		/*------------------------------------------------------------------------------------------*/
//		RmxCheckSencOriginPass(MencV);            /* <S034>DELL */
		if( MencV->AxisMotType == MOTTYPE_LINEAR )/* <S029> : ���j�A�����ǉ� */
		{ /* ���j�A�^ */
			LmxCheckSencCphasePass(MencV);					/* ���_�ʉ߃`�F�b�N						*/
		}
		else
		{ /* ��]�^ */
			RmxCheckSencOriginPass(MencV);					/* ���_�ʉ߃`�F�b�N						*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�����p���X�o�͏���																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MencV->DivInfo.DivOut == TRUE) && (MencV->DivInfo.IncPulseReq == FALSE) && (MencV->DivInfo.DivOutASIC == FALSE) )
	{
		if( MencV->AxisMotType != MOTTYPE_LINEAR )/* <S21B>:���j�A�����ǉ� */
		{ /* ��]�^ */
			SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
		/*----------------------------------------------------------------------------------------------*/
		/*  C���o�͏���													<S02D>							*/
		/*----------------------------------------------------------------------------------------------*/
			if( MencV->DivInfo.DivPcmaskCtrl )
			{
				lwk1 = ( MencV->MotPosX[0] - MencV->MotOrgX);
				if( ( (lwk1 > -MencV->DivInfo.DivOutCmskChk) && (lwk1 < MencV->DivInfo.DivOutCmskChk)) && (MencV->Cset) )
				{   /* �p���X�ϊ�C���}�X�NOFF�ݒ�			*/
					Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;
					if( MencV->DivInfo.DivPass == FALSE )
					{
						if( MencV->OrgOk )
						{ /* �G���R�[�_�ʒu�␳�`���l��0���番��C��2�ɐݒ� */
							Enc->SencSetReg->AREG_DIVSET |= (BIT11 | BIT8);
						}
					}
				}
				else
				{	/* �p���X�ϊ�C���}�X�NON�ݒ�			*/
					Enc->SencSetReg->AREG_WDT1SET |= BIT8;
					/* �G���R�[�_�ʒu�␳�`���l��0���番��C���ʏ�ɐݒ� */
					Enc->SencSetReg->AREG_DIVSET |= BIT8;
				}
			}
		/*----------------------------------------------------------------------------------------------*/
		}
		else/* <S21B>:���j�A�����ǉ� */
		{/* �Ʊ�^ */
			MencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dMotPos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem);
			SHalDrv_SetDivOutPosition( Enc->SencSetReg, (ULONG)MencV->DivInfo.DivOutPos );			
		}
	}

#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�ʒu�f�[�^��M����(HW:JL-086)										*/
/*																									*/
/****************************************************************************************************//* <S004> */
static void	SencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE *AlmManager )/*<S0C5>*/
{
	/*----------------------------------------------------------------------------------------------*/
	/*		�A���[�����̏���																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->SeRxErr   = SHal_GetSeRxErr( pAsicHwReg );		/* ��M�G���[�t���O�X�V 				*/
	MencV->EncDataNG = SHal_GetEncDataNG( pAsicHwReg );		/* �G���R�[�_�f�[�^�ُ�t���O�̍X�V 	*/
	/*----------------------------------------------------------------------------------------------*/
	if( SHal_GetComAlarm( pAsicHwReg ) )					/* �G���[��ʔ���(�ʐM�^�����x)			*/
	{
		ALMSetServoAlarm( AlmManager, ALM_PGCOM );			/* A.C90 : �G���R�[�_�ʐM�ُ�			*//*<S0C5>*/
	}
	if( SHal_GetAccAlarm( pAsicHwReg ) )					/* �G���[��ʔ���(�ʐM�^�����x)			*/
	{
		ALMSetServoAlarm( AlmManager, ALM_PGCOM1 );			/* A.C91 : �G���R�[�_�ʐM�ُ�(�����x)	*//*<S0C5>*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		�␳�����ʒu�f�[�^�X�V																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RcvPosX[2] = MencV->RcvPosX[1];			/*<S0C5>*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];			/*<S0C5>*/
	MencV->RcvPosX[0] = (LONG)( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
	/*----------------------------------------------------------------------------------------------*/
	/*		��M�f�[�^L,H�X�V																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RxPosL[1].Long = MencV->RxPosL[0].Long;	/*<S0C5>*/
	MencV->RxPosH[1] 	  = MencV->RxPosH[0];		/*<S0C5>*/
	MencV->RxPosL[0].Long = (LONG) SHal_GetCurRxPosL( pAsicHwReg );
	MencV->RxPosH[0] 	  = (SHORT)SHal_GetCurRxPosH( pAsicHwReg );
	/*----------------------------------------------------------------------------------------------*/
	/*		�ω��ʒu�^�C�}�A�x�[�X�^�C�}�X�V														*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->BaseTimer	  = (USHORT)SHal_GetBaseTim( pAsicHwReg );									  /* <S067> */
	MencV->HenkaItiTimer  = (USHORT)SHal_GetHenkaItiTim( pAsicHwReg );								  /* <S067> */

	/*----------------------------------------------------------------------------------------------*/
	SHal_SrcClr( pAsicHwReg );								/* ��M�����t���O�X�V					*/
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		��]�^�F�V���A���G���R�[�_�ʒu�f�[�^��M����(SW)											*/
/*																									*/
/****************************************************************************************************//* <S004> */
static void	SencRcvPosSwProcess( MENCV *MencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete )	  /* <S0C5> */
{
	ULONG	ulwk;
	LONG	lwk1;
	LONG	RcvPosX;
	SENCX	*Enc;
	ULONG	RxFlg;

	Enc	= &MencV->EncIf;

/*--------------------------------------------------------------------------------------------------*/
/*		��M������ : �V���A���G���R�[�_��M�ُ폈��													*/
/*--------------------------------------------------------------------------------------------------*/
	RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
	if( ((RxFlg & 0x8000) == 0) && (MencV->SeRxComp == FALSE) )
	{
		MencV->ComErrCnt++;									/* Count up Error Counter				*/
		MencV->SeRxErr = TRUE;								/* Set Error Flag						*/
		LpxSencComErrorProc(MencV, AlmManager, 0, FALSE);	/*<S0C5>*/
	/*----------------------------------------------------------------------------------------------*/
	/*		�G���R�[�_�f�[�^�������̏���															*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->EncDataNG == TRUE )
		{
			if( BeComplete == FALSE )								/* �x�[�X�u���b�N��				*/
			{
			/*--------------------------------------------------------------------------------------*/
			/*	�O��l�E�O�X��l�X�V����															*/
			/*		�G���R�[�_�f�[�^������ && �x�[�X�u���b�N���ɁA									*/
			/*		����l�ƑO��l�̍����Ōv�Z����鑬�x���O�ɂ��邽�ߑO��l���X�V����B			*/
			/*--------------------------------------------------------------------------------------*/
				MencV->RxPosL[1]  = MencV->RxPosL[0];				/* �O��l�X�V					*/
				MencV->RxPosH[1]  = MencV->RxPosH[0];				/* �O��l�X�V					*/
				MencV->RcvPosX[2] = MencV->RcvPosX[1];				/* �O�X��l�X�V					*/
				MencV->RcvPosX[1] = MencV->RcvPosX[0];				/* �O��l�X�V					*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		��M������ : �V���A���G���R�[�_��M��������													*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	��M�f�[�^�擾																				*/
	/*----------------------------------------------------------------------------------------------*/
		SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&Enc->RxData );
	/*----------------------------------------------------------------------------------------------*/
	/*	��M��Flag�̃��Z�b�g�FIntSync Set --> ScanA Clear											*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->SeRxComp = FALSE;									/* Reset Rx Complete Flag		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	��M�f�[�^������ϐ��ɐݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->RxPosL[1]		= MencV->RxPosL[0];					/* �O��l�X�V					*/
		MencV->RxPosH[1]		= MencV->RxPosH[0];					/* �O��l�X�V					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->RxAlmCode		= (USHORT)(Enc->RxData.UsBuf[2]);	/* ����l�Ǎ��݁FAlarm Code		*/
		MencV->RxTimex.w.l		= (USHORT)(Enc->RxData.UsBuf[3]);	/* ����l�Ǎ��݁FTimeBase, etc	*/
		MencV->RxTimex.w.h		= (USHORT)(Enc->RxData.UsBuf[4]);	/* ����l�Ǎ��݁FTimeBase, etc	*/
		MencV->RxPosL[0].w.l	= (USHORT)(Enc->RxData.UsBuf[5]);	/* ����l�Ǎ��݁FPosition Low	*/
		MencV->RxPosL[0].w.h	= (USHORT)(Enc->RxData.UsBuf[6]);	/* ����l�Ǎ��݁FPosition High	*/
		MencV->RxPosH[0]		= (USHORT)(Enc->RxData.UsBuf[7]);	/* ����l�Ǎ��݁FIncre. Compen.	*/
	/*----------------------------------------------------------------------------------------------*/
	/*	�ʒu���Z																					*/
	/*----------------------------------------------------------------------------------------------*/
		RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long >> MencV->MposSftX) << MencV->MposSftR);
	/*----------------------------------------------------------------------------------------------*/
	/*	�����x���Z�`�F�b�N																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->AccChkEnable == FALSE )
		{
			lwk1 = 0;
			MencV->AccChkWaitCnt++;
			if (MencV->AccChkWaitCnt > 3)
			{
				MencV->AccChkEnable = TRUE;
			}
			MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = RcvPosX;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	�����x���Z : ���񑬓x - �O�񑬓x (����̉����x���ُ�ȏꍇ��,�O�X��̃f�[�^�ōČv�Z)		*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			lwk1 = (RcvPosX - MencV->RcvPosX[0]) - (MencV->RcvPosX[0] - MencV->RcvPosX[1]);
			if( (lwk1 > MencV->AccErrLvl) || (lwk1 < -(MencV->AccErrLvl)) )
			{
				lwk1 = ((RcvPosX - MencV->RcvPosX[1])>>1) - (MencV->RcvPosX[1] - MencV->RcvPosX[2]);
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	�����x�ُ펞 : �V���A���G���R�[�_�ʐM�ُ폈��												*/
	/*----------------------------------------------------------------------------------------------*/
		if( (lwk1 > MencV->AccErrLvl) || (lwk1 < -(MencV->AccErrLvl)) )
		{
			MencV->AccErrCnt++;								/* Count up Error Counter				*/
			MencV->SeRxErr = TRUE;							/* Set Error Flag						*/
			LpxSencComErrorProc(MencV, AlmManager, 0, FALSE);/*<S0C5>*/
		/*------------------------------------------------------------------------------------------*/
		/*		�G���R�[�_�f�[�^�������̏���														*/
		/*------------------------------------------------------------------------------------------*/
			if( MencV->EncDataNG == TRUE )
			{
				if( BeComplete == FALSE )								/* �x�[�X�u���b�N��			*/
				{
				/*----------------------------------------------------------------------------------*/
				/*	�O��l�E�O�X��l�X�V����														*/
				/*		�G���R�[�_�f�[�^������ && �x�[�X�u���b�N���ɁA								*/
				/*		����l�ƑO��l�̍����Ōv�Z����鑬�x���O�ɂ��邽�ߑO��l���X�V����B		*/
				/*----------------------------------------------------------------------------------*/
					MencV->RxPosL[1]  = MencV->RxPosL[0];				/* �O��l�X�V				*/
					MencV->RxPosH[1]  = MencV->RxPosH[0];				/* �O��l�X�V				*/
					MencV->RcvPosX[2] = MencV->RcvPosX[1];				/* �O�X��l�X�V				*/
					MencV->RcvPosX[1] = MencV->RcvPosX[0];				/* �O��l�X�V				*/
				}
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	�����x���펞 : ���[�^���Ɉʑ����Z															*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			MencV->MstErrCnt   = 0;							/* Reset Error Counter					*/
			MencV->SeRxErr     = FALSE;						/* Reset Error Flag						*/
			MencV->EncDataNG   = FALSE;						/* Reset Encoder Data NG Flag			*/
			MencV->RcvPosX[2]  = MencV->RcvPosX[1];			/* �O�X��ʒu�f�[�^						*/
			MencV->RcvPosX[1]  = MencV->RcvPosX[0];			/* �O��ʒu�f�[�^						*/
			MencV->RcvPosX[0]  = RcvPosX;					/* ����ʒu�f�[�^						*/
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�x�[�X�^�C�}�l�A�ω��ʒu�^�C�}�l�쐬			<S067>										*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->BaseTimer = (USHORT)(MencV->RxTimex.dw >> 8);
	ulwk = ((ULONG)MencV->RxPosL[0].w.l << 16) + (ULONG)MencV->RxTimex.w.h;
	MencV->HenkaItiTimer = (USHORT)(ulwk >> 8);

	return;
}


/****************************************************************************************************/
/*																									*/
/*		���j�A���[�^�F���[�^�d���ʑ��X�V����	<S018>												*/
/*																									*/
/****************************************************************************************************/
#define	LMX_MORG_CHKMAX		3
/*--------------------------------------------------------------------------------------------------*/
static void	LmxUpdateMotPhase( MENCV *MencV, ALMMAN_HANDLE *AlmManager, LONG dMotPos )
{
	LONG	lwk1;
	LONG	lwk2;
	LONG	LastMotPhaX;
	LONG	NewPoleInfo;
	LONG	MotPoleInfoX;

/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�ʑ��p�ʒu���Z																		*/
/*--------------------------------------------------------------------------------------------------*/
		LastMotPhaX = MencV->MotPhaX;						/* �O��l�ۑ�							*/
		lwk1 = MencV->MotPhaX + dMotPos;					/* ����l���Z							*/
	/*----------------------------------------------------------------------------------------------*/
		if( lwk1 >= (LONG)MencV->PulseNo )					/* Check Upper Limit					*/
		{													/*										*/
			lwk1 -= (LONG)MencV->PulseNo;					/* Adjust MotPos by PulseNo				*/
		}													/*										*/
		else if( lwk1 < 0 )									/* Check Lower Limit					*/
		{													/*										*/
			lwk1 += (LONG)MencV->PulseNo;					/* Adjust MotPos by PulseNo				*/
		}													/*										*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotPhaX = lwk1;								/* ���[�^�ʑ��p�ʒu�X�V					*/

/*--------------------------------------------------------------------------------------------------*/
/*		�|�[���Z���T����̏ꍇ																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MencV->withPoleSensor) && (!MencV->MotOrgChkEnd) && (MencV->MstErrCnt == 0) )
		{
			if( MencV->Sigma3Encoder )
			{
				NewPoleInfo = (MencV->RxPosH[0]>>1) & 0x07;
			}
			else
			{
				NewPoleInfo = (MencV->RxPosL[0].w.l>>9) & 0x07;
			}
			//if( MencV->PoleReverse == TRUE ){ NewPoleInfo = ~NewPoleInfo & 0x07; }	/* <V808>		*///���ƂőΉ����邱�ƁI�Ikira
			MotPoleInfoX = MencV->MotPoleInfo[0];
			MlibBITFILTER1( MencV->MotPoleInfo[0], MencV->MotPoleInfo[1], (UCHAR)NewPoleInfo );
			MotPoleInfoX = ((MencV->MotPoleInfo[0] ^ MotPoleInfoX)<<4) | MencV->MotPoleInfo[0];
		/*------------------------------------------------------------------------------------------*/
		/*	�t�u�v�M���F�ُ�`�F�b�N�FUVW = 000 or 111 �ňُ�										*/
		/*------------------------------------------------------------------------------------------*/
			if( (MencV->MotPoleInfo[0] == 0x00) || (MencV->MotPoleInfo[0] == 0x07) )
			{
				ALMSetServoAlarm( AlmManager, ALM_POLESENSOR );	/* A.C21 : �|�[���Z���T�ُ� 		*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�t�u�v�M���F�t���G�b�W�`�F�b�N�FMotPoleInfoX = UVW-Edge(6,5,4bit) + UVW-Sts(2,1,0bit) 	*/
		/*------------------------------------------------------------------------------------------*/
			else if( (MotPoleInfoX & 0x43) == 0x41 )		/* U-Edge & W-ON : 0 deg				*/
			{
				if( MencV->MotOrgChkCnt < LMX_MORG_CHKMAX )
				{
					++MencV->MotOrgChkCnt;					/* Count up Motor Origin Check Counter	*/
				}
				if( (MencV->MotOrgChkAlways == FALSE) && (MencV->MotOrgChkCnt >= LMX_MORG_CHKMAX) )
				{
					MencV->MotOrgChkEnd = TRUE;				/* Set Motor Origin Check End Flag		*/
				}
			/*--------------------------------------------------------------------------------------*/
			/*	���[�^���_�`�F�b�N���X�V�F�r�b�g�t�B���^�̒x����l�����đO��l���g�p����			*/
			/*--------------------------------------------------------------------------------------*/
				lwk1 = MlibABS( (LastMotPhaX - MencV->MotOrgXchk[0]) );
				lwk2 = MlibABS( (LastMotPhaX - MencV->MotOrgXchk[1]) );
				/*----------------------------------------------------------------------------------*/
				if( lwk1 > MencV->HalfPulseNo )		/*<V498>*/
				{
					lwk1 = MencV->PulseNo - lwk1;
				}
				if ( lwk2 > MencV->HalfPulseNo )		/*<V498>*/
				{
					lwk2 = MencV->PulseNo - lwk2;
				}
				/*----------------------------------------------------------------------------------*/
				if( lwk1 <= MencV->PulseNo30deg )
				{
					MencV->MotOrgX = LastMotPhaX;			/* ���[�^�ʑ����_�X�V					*/
				}
				else if( (MencV->MotOrgChkCnt >= LMX_MORG_CHKMAX) && (lwk2 > MencV->PulseNo30deg) )
				{
					ALMSetServoAlarm( AlmManager, ALM_PHASE );/* A.C20 : �ʑ��댟�o					*/
				}
			/*--------------------------------------------------------------------------------------*/
				MencV->MotOrgXchk[1] = MencV->MotOrgXchk[0];/* Update Motor Origin Check Data		*/
				MencV->MotOrgXchk[0] = LastMotPhaX;			/* Update Motor Origin Check Data		*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	�t�u�v�M���F�u���G�b�W�`�F�b�N(�K�v�ɉ����Ēǉ�,�������ӗv)								*/
		/*------------------------------------------------------------------------------------------*/
			else if( 0 && (MotPoleInfoX & 0x25) == 0x24 )	/* V-Edge & U-ON : 120deg/240deg		*/
			{
				;											/* ����UVW���F120deg					*/
				;											/* ����UWV���F240deg					*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	���[�^�ʑ��v�Z�F���Z���ʂ�USHORT���g�p����̂ŕ����␳�͕s�v(�����Ŋm�F����)			*/
		/*------------------------------------------------------------------------------------------*/
			lwk1 = MlibMulgainNolim( (MencV->MotPhaX - MencV->MotOrgX), MencV->Kmotphase );
			MencV->MotPhase = (USHORT)(lwk1 + MencV->PhaseOffset);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�|�[���Z���T�Ȃ��̏ꍇ																		*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			lwk1 = MlibMulgainNolim( (MencV->MotPhaX - MencV->MotOrgX), MencV->Kmotphase );
			MencV->MotPhase = (USHORT)(lwk1 + MencV->PhaseOffset + MencV->MpfPhaseOffset);
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


#ifdef _TIME_BASE_TIMER_
/****************************************************************************************************/
/*																									*/
/*		�ω������^�C�}�𗘗p�����ʒu�␳���Z														*/
/*																									*/
/*		�T�v�F�G���R�[�_���̕ω������^�C�}��p���ăG���R�[�_�ʒu��␳����						*/
/*			  �G���R�[�_�̃T���v�����O�ƁA�T�[�{�̐�������̃r�[�g���ۑ΍�((M�ZD1)V03-010)			*/
/*			  �T���v�����O�̂����␳���邽�߂ɁA�G���R�[�_���Ō�̃p���X�����b�`���Ă���			*/
/*			  �o�߂������ԁi�ω������^�C�}�j���g�p����B											*/
/*			  �ω������^�C�}�l��2.66us�����̂Ƃ��A�x�[�X�^�C�}��2.66us�O�̈ʒu���Z�o����B			*/
/*																									*/
/*		����: ���Z�̃I�[�o�t���[��h�~���邽�߈�����[2^32/rev]�P�ʂ�[pulse]�֊��Z���A���Z���		*/
/*			  �P�ʂ�߂��B																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		arg		�F	IN		LONG	RcvPosX		�F����G���R�[�_�ʒu(�␳�O)	[2^32/rev]			*/
/*				�F	IN		LONG	LastMotPosX	�F�O��G���R�[�_�ʒu(�␳��)	[2^32/rev]			*/
/*				�F	IN		UCHAR	MposSftR	�F�ʒu�f�[�^�V�t�g��(32-BitNo)	[-]					*/
/*				�F OUT		None																	*/
/*--------------------------------------------------------------------------------------------------*/
/*		return	�F			LONG				  ����G���R�[�_�ʒu(�␳��)	[2^32/rev]			*/
/*--------------------------------------------------------------------------------------------------*/
/*		g-ref	�F	MencV.RxPosL				: �ω������^�C�}�Q�Ɨp								*/
/*					MencV.RxTimex.w.h			: ����												*/
/*					MencP.MposSftR				: [pulse]��[2^32]�ϊ��p�V�t�g��						*/
/*--------------------------------------------------------------------------------------------------*/
/*		MACRO	:	RMX_XposToUpos	: [2^32/rev]��[pulse]											*/
/****************************************************************************************************/
LONG	LpxTimeBasesMotPosCalc( MENCV *MencV )
{
	LONG	MotPosCmp;										/* ����␳��G���R�[�_�ʒu[pulse]		*/
	ULONG	ulwk;											/* ���[�N								*/
	SHORT	swk;											/* ���[�N								*/
	USHORT	HenkaItiTimer;									/* �ω��ʒu�^�C�}						*/
	LONG	LinEncPos;																				/* <S004> */

	LinEncPos = 1 << (32 - MencV->MposSftR) ;				/* ���j�A�p�G���R�[�_�ʒu�I�[�o�[�t���[	*//* <S004> */
		/*------------------------------------------------------------------------------------------*/
		/*	���Z���� �F �␳���Z�ɂăI�[�o�t���[���Ȃ��悤�ɒP�ʂ����Z����							*/
		/*				[2^32/rev]��[pulse/rev]														*/
		/*------------------------------------------------------------------------------------------*/
		MotPosCmp = ((ULONG)MencV->RcvPosX[0]) >> MencV->MposSftR;	/* ������						*/
		if( MencV->SeRxErr == FALSE )						/* ��M�G���[ or �����x�ُ펞�␳����	*/
		{
			/*--------------------------------------------------------------------------------------*/
			/*	�ω������^�C�}�擾																	*/
			/*																						*/
			/*				  �ω��ʒu�^�C�}														*/
			/*	___________________��____________________											*/
			/*	|         |/////////|/////////|         |											*/
			/*	�P�P�P�P�P���P�P�P�P�P�P�P�P�P���P�P�P�P											*/
			/*	 MencV.RxPosL[0].w.l MencV.RxTimex.w.h												*/
			/*																						*/
			/*--------------------------------------------------------------------------------------*/
			ulwk = ((ULONG)MencV->RxPosL[0].w.l << 16) + (ULONG)MencV->RxTimex.w.h;
			HenkaItiTimer = (USHORT)(ulwk >> 8);
			/*--------------------------------------------------------------------------------------*/
			/*	�ʒu�␳																			*/
			/*																						*/
			/*	�@�v�Z���FP(t-2.66[us]) = P(t) + (P(t) - P(t-1)) �~ (��t-2.66[us])/62.5[us]		*/
			/*																						*/
			/*			(��t - 2.66[us]) / 62.5[us] = ��t / 62.5[us] - 2.66[us]/62.5[us]			*/
			/*		�G���R�[�_�N���b�N��6MHz�𕪕�C���q�Ɋ|���āC�N���b�N���ɕϊ�����ƁC			*//* <S004> */
			/*			= (��t*6[MHz] - 2.66[us]*6[MHz])/(62.5[us]*6[MHz])							*/
			/*																						*/
			/*		1 / (62.5[us]*6[MHz]) -> 1 / SAENCCLKNUM(375)									*/
			/*		2.66[us]*6[MHz] -> ENCLCHCLKNUM(16)												*/
			/*		��t*6[MHz] -> �ω��ʒu�^�C�}�J�E���^��											*/
			/*																						*/
			/*--------------------------------------------------------------------------------------*/
			if(HenkaItiTimer <= ENCLCHCLKNUM)
			{	/* �G���R�[�_���b�`�����ȉ�	*//* <S004> */
				swk = (SHORT)(HenkaItiTimer - ENCLCHCLKNUM);
				MotPosCmp = MotPosCmp + ((((MencV->RcvPosX[0] - MencV->MotPosX[0])>>MencV->MposSftR ) * swk) / SAENCCLKNUM);
				if( MotPosCmp >= 0 )
				{
					//no operation 						/* <S004> */
					//MotPosCmp = MotPosCmp;			/* <S004> */
				}
				else
				{
					if(( MencV->AxisMotType == MOTTYPE_LINEAR ))
					{
						MotPosCmp = MotPosCmp + LinEncPos;
					}
					else
					{
						MotPosCmp = MotPosCmp + MencV->PulseNo;
					}
				}
			}
		}
		/*------------------------------------------------------------------------------------------*/
		/*	���Z�㏈�� �F �P�ʂ�߂�	[pulse/rev]	�� [2^32/rev]									*/
		/*------------------------------------------------------------------------------------------*/
		return( MotPosCmp << MencV->MposSftR );

}
#endif /* _TIME_BASE_TIMER_ */



/* <S067> */
/****************************************************************************************************/
/*																									*/
/*		�^�C���x�[�X�^�C�}�ƕω������^�C�}�𗘗p�������x���Z										*/
/*																									*/
/*		�T�v�F�G���R�[�_���̃x�[�X�^�C�}�C����ѕω������^�C�}��p���đ��x���o����				*/
/*			  �]���̈ʒu���������ł́C�ʒu���b�`�Ԋu��62.5��s�ł��邱�Ƃ�O��Ƃ��Ă��邪�C			*/
/*			  ���ۂɂ́C�G���R�[�_�̃��b�`�Ԋu�ɂ������ɃY���Ă���B								*/
/*			  ���ꂪ�r�[�g�ɂȂ����肵�Ĉ��e�����y�ڂ����Ƃ�����B									*/
/*			  �G���R�[�_�^�C�}���𗘗p���邱�Ƃɂ��C���x���o���x�̌��オ���҂ł���B			*/
/*			  �������C�G���R�[�_�ʒu�������P�p���X�ȉ��̏ꍇ�́C�ω����ԊԊu���ُ�ɏ������Ȃ�ꍇ	*/
/*			  ������̂ŁC�ʒu�����Q�p���X�ȏ�Ŗ{������p����										*/
/*--------------------------------------------------------------------------------------------------*/
/*			  ���s�v���t���O��TRUE �̏ꍇ�͑��x�␳���Z���s���B										*/
/*			  ���s�v���t���O��FALSE�̏ꍇ�� Vfbtimbase.LastHenkaTime, Vfbtimbase.LastBaseTimer		*/
/*			  �̍쐬�̂ݍs���B																		*/
/*			  ���s�v���t���O�͓d���������A�p�����[�^�Čv�Z����FALSE�ƂȂ�B							*/
/*			  �V�݂������R��P1���[�h�Œ�����ɃG���R�[�_��M���J�n���Ă��珈�����n�߂邽�߁B		*/
/*			  ���ۂɂ�LastModflag�쐬�ɂ��P�X�L�����K�v�Ȃ��߁A�d��������ScanA�����s����͂��߂�	*/
/*			  �R�X�L�����ڂ���␳���J�n�����B													*/
/*																									*/
/*																									*/
/*		arg		�F	LONG	dPos	�F�G���R�[�_�ʒu����			[pulse/scan]					*/
/*					LONG	dPosSpd	�F�G���R�[�_�ʒu�������x		[2^24/OvrSpd]					*/
/*					UCHAR	EnableFlag�F�␳���s���t���O											*/
/*		g-ref	�F																					*/
/*		out		�F	LONG	motspeed�F���[�^���x					[2^24/OvrSpd]					*/
/*																									*/
/****************************************************************************************************/
LONG	TimeBasesMotSpdCalc( VFBTIMBASE *Vfbtimbase, MENCV *MencV, LONG dPos, LONG dPosSpd,
																	BOOL VcompSts, BOOL EnableFlag )
{

//	ULONG	ulwk;//<S0C7>
	LONG	lwk;

	/*----------------------------------------------------------------------------------------------*/
	/*	�ω������^�C�}�擾																			*/
	/*																								*/
	/*				  �ω��ʒu�^�C�}																*/
	/*	___________________��____________________													*/
	/*	|         |/////////|/////////|         |													*/
	/*	�P�P�P�P�P���P�P�P�P�P�P�P�P�P���P�P�P�P													*/
	/*	 MencV.RxPosL[0].w.l MencV.RxTimex.w.h														*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
//		ulwk = ((ULONG)MencV->RxPosL[0].w.l << 16) + (ULONG)MencV->RxTimex.w.h;						  /* <S067> */
//		Vfbtimbase->HenkaItiTimer = (USHORT)(ulwk >> 8);											  /* <S067> */
		Vfbtimbase->HenkaItiTimer = MencV->HenkaItiTimer;
	/*----------------------------------------------------------------------------------------------*/
	/*	�x�[�X�^�C�}�擾																			*/
	/*																								*/
	/*				  �x�[�X�^�C�}																	*/
	/*	___________________��____________________													*/
	/*	|         |/////////|/////////|         |													*/
	/*	�P�P�P�P�P���P�P�P�P�P�P�P�P�P���P�P�P�P													*/
	/*	  MencV->RxTimex.w.h    MencV->RxTimex.w.l													*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
//		Vfbtimbase->BaseTimer = (USHORT)(MencV->RxTimex.dw >> 8);									  /* <S067> */
		Vfbtimbase->BaseTimer = MencV->BaseTimer;
	/*----------------------------------------------------------------------------------------------*/
	/*	�ω��������Z																				*/
	/*----------------------------------------------------------------------------------------------*/
		Vfbtimbase->HenkaTime = Vfbtimbase->BaseTimer - Vfbtimbase->HenkaItiTimer;
		Vfbtimbase->dPos = dPos;							/* ���񍷕��ʒu�e�a�擾					*/

	/*----------------------------------------------------------------------------------------------*/
	/*	�␳�L������																				*/
	/*----------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	�␳����������																			*/
		/*------------------------------------------------------------------------------------------*/
		if( EnableFlag == FALSE )
		{
			Vfbtimbase->Modflag = FALSE;				/* ���␳���[�h�ݒ�							*/
			Vfbtimbase->MotSpeed = dPosSpd;				/* ���␳�̂܂܏o��							*/
		}
		else
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�␳�L��������																			*/
		/*------------------------------------------------------------------------------------------*/
			/*--------------------------------------------------------------------------------------*/
			/*	�ω����ԊԊu���Z																	*/
			/*--------------------------------------------------------------------------------------*/
			Vfbtimbase->TimeInterval = Vfbtimbase->HenkaTime - Vfbtimbase->LastHenkaTime;
			Vfbtimbase->BaseTimeInterval = Vfbtimbase->BaseTimer - Vfbtimbase->LastBaseTimer;
			/*--------------------------------------------------------------------------------------*/
			/*	���x���Z���s����																	*/
			/*--------------------------------------------------------------------------------------*/
			if(MlibABS(dPos) >= VFBTINMINPLS)					/* �����x��2�p���X�ȏ�				*/
			{
				if( Vfbtimbase->ModChgCnt < VFBTINMINCNT )		/* 5��A���Ȃ�ω��ʒu�^�C�}����	*/
				{
					Vfbtimbase->ModChgCnt++;
				}
			}
			else
			{
				Vfbtimbase->ModChgCnt = 0;
			}

			/*--------------------------------------------------------------------------------------*/
			/*	�펞�ω��ʒu�^�C�}���s����															*/
			/*--------------------------------------------------------------------------------------*/
//			if( (Iprm.TuningLessMode & 0x100)					/* ���x���o�ؑւ�����				*/
//				&& ( Gprm.f.TuningLessVComp == FALSE) )			/* ���x����Ƃ��Ďg�p��				*/
			if( VcompSts == FALSE )								/* ���x���o�ؑւ�����				*/
			{													/* ���x����Ƃ��Ďg�p��				*/
				if(Vfbtimbase->TimeInterval > SAENCCLKNUMC2)	/* �ω����ԊԊu����������~2����	*/
				{
					Vfbtimbase->TimeInterval = SAENCCLKNUMC2;
				}
				else if(MlibAbs32(Vfbtimbase->dPos) <= 1)		/* ����ʒu����FB��1�p���X�ȉ�		*/
				{
					if(Vfbtimbase->TimeInterval < SAENCCLKNUM)	/* �ω����ԊԊu�������������		*/
					{
						Vfbtimbase->TimeInterval = SAENCCLKNUM;
					}
				}
				Vfbtimbase->ModChgCnt = VFBTINMINCNT;			/* �펞�ω��ʒu�^�C�}�␳�L��		*/
			}

			if ( (Vfbtimbase->ModChgCnt < VFBTINMINCNT)				/* 5��A��2�p���X�ȏ�ł͂Ȃ�	*/
				|| (Vfbtimbase->TimeInterval < SAENCCLKNUMC2D3 )	/* �ω����Ԃ��ŏ��l����			*/
				|| (MencV->SeRxErr == TRUE) )						/* Set Error Flag				*/
			{
				Vfbtimbase->Modflag = FALSE;
			}
			else
			{
				Vfbtimbase->Modflag = TRUE;
			}

			if (Vfbtimbase->Modflag && Vfbtimbase->LastModflag)
			{
				/*----------------------------------------------------------------------------------*/
				/* �ω��ʒu�^�C�}���g�p�������x���Z													*/
				/*----------------------------------------------------------------------------------*/
				/* ���ӁFVfbtimbase->LastModflag�̏����l��FALSE�ł���B����āC�d����������̑��x	*/
				/*		 ���o�͈ʒu���������ƂȂ�B����āCVfbtimbase->LastHenkaTime�͕K���Z�b�g	*/
				/*		 ����Ă���{if�����ɓ���B����������Vfbtimbase->LastHenkaTime�̏����l��0��	*/
				/*		 �ǂ�																		*/
				/*----------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	���[�^���x���Z																	*/
				/*																					*/
				/*					dPosSpd(�ݺ��ވʒu�������x) * ���x���Z����[�ް���ϸۯ���]		*/
				/*	motorspeed = ---------------------------------------------------------------	*/
				/*							TimeInterval(�ω������Ԋu)[�ް���ϸۯ���]				*/
				/*																					*/
				/*	�����x���Z�������ް���ϸۯ����F���x���Z����:62.5[us]����375�ۯ�(�ް����:6MHz)	*/
				/*																					*/
				/*	motorspeed��P���Ɍv�Z����ƍō���]�t�߂�Dword�𒴂��Ă��܂��B					*/
				/*	���̂��߁C�܂��ݺ��ވʒu�������x���Qbit��Ă����Z����������Z�C�ω������ŏ��Z��	*/
				/*	�Qbit���ɖ߂��B	���Z��̑��x���o����\���Qbit�����邪�����e���Ȃ��B				*/
				/*	�{�����́C���x���Z�������ް���ϸۯ��� < 128�̏ꍇ��,Dword�𒴂��Ȃ��̂ŕs�v		*/
				/*																					*/
				/*----------------------------------------------------------------------------------*/
				if((Vfbtimbase->dPos == 0)								/* ����e�a���[��			*/
					 && (Vfbtimbase->LastdPos > 0))						/* ���O��e�a���[��		*/
				{
					/* �O�񑬓x���ێ�		*/
				}
				else
				{
					lwk = (dPosSpd >> 2) * SAENCCLKNUM;
					lwk = lwk / (LONG)Vfbtimbase->TimeInterval;
					Vfbtimbase->MotSpeed = lwk << 2;
				}	/*<V334>*/
			}
			else if((Vfbtimbase->BaseTimeInterval != 0)		/* �ް���ϕω����ԂȂ� -> �ݺ��޴װ��	*/
				&& (Vfbtimbase->LastSeRxErr == FALSE) )				/* Last Error Flag				*//*<V648>*/
			{
				/*----------------------------------------------------------------------------------*/
				/* �x�[�X�^�C�}���g�p�������x���Z													*/
				/*----------------------------------------------------------------------------------*/
				/*	���[�^���x���Z																	*/
				/*																					*/
				/*					dPosSpd(�ݺ��ވʒu�������x) * ���x���Z����[�ް���ϸۯ���]		*/
				/*	motorspeed = ---------------------------------------------------------------	*/
				/*						BaseTimeInterval(�ް���ϕω�����)[�ް���ϸۯ���]			*/
				/*																					*/
				/*	�����x���Z�������ް���ϸۯ����F���x���Z����:62.5[us]����375�ۯ�(�ް����:6MHz)	*/
				/*																					*/
				/*																					*/
				/*	motorspeed��P���Ɍv�Z����ƍō���]�t�߂�Dword�𒴂��Ă��܂��B					*/
				/*	���̂��߁C�܂��ݺ��ވʒu�������x���Qbit��Ă����Z����������Z�C�ω������ŏ��Z��	*/
				/*	�Qbit���ɖ߂��B	���Z��̑��x���o����\���Qbit�����邪�����e���Ȃ��B				*/
				/*	�{�����́C���x���Z�������ް���ϸۯ��� < 128�̏ꍇ��,Dword�𒴂��Ȃ��̂ŕs�v		*/
				/*																					*/
				/*----------------------------------------------------------------------------------*/
				lwk = (dPosSpd >> 2) * SAENCCLKNUM;
				lwk = lwk / (LONG)Vfbtimbase->BaseTimeInterval;
				Vfbtimbase->MotSpeed = lwk << 2;
			}
			else
			{
				/*----------------------------------------------------------------------------------*/
				/* �␳�������B�̏ꍇ�F�G���R�[�_�ʒu�������x���Z�g�p								*/
				/*----------------------------------------------------------------------------------*/
				Vfbtimbase->MotSpeed = dPosSpd;
			}
		}
		/*------------------------------------------------------------------------------------------*/
		/*	����l�ۑ�																				*/
		/*------------------------------------------------------------------------------------------*/
		Vfbtimbase->LastHenkaTime	= Vfbtimbase->HenkaTime;
		Vfbtimbase->LastBaseTimer	= Vfbtimbase->BaseTimer;
		Vfbtimbase->LastModflag		= Vfbtimbase->Modflag;
		Vfbtimbase->LastdPos		= Vfbtimbase->dPos;
		Vfbtimbase->LastSeRxErr		= MencV->SeRxErr;

		return (Vfbtimbase->MotSpeed);
}



#if 1 /*<S02D> �ȉ��̊֐��͎b��(�t���N�Ή��������K�v) */
#define	SVFSEL_PAOPOUT		1				/* �o�`�n���p���X�o��			*//*<S02D>*/
/****************************************************************************************************/
/*																									*/
/*		�t���N�G���R�[�_�F�����C���N���p���X�o�͏���					<S02D>						*/
/*																									*/
/****************************************************************************************************/
#define FMAXPULSEOUT		170
static void	FencSencOutIniIncPulse( MENCV  *FencV, ASIC_HW_IF_STR  *pAsicHwReg )
{
LONG	lwk;
LONG	MtnCur;
LONG	MtnSub;
LONG	MtnRem;
LONG	MtnSepos;
SHORT	MturnLatchCur;
SHORT	PosH;
DWORDX	PosL;

/*--------------------------------------------------------------------------------------------------*/
/*		�ʐM�ُ�`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( FencV->SeRxErr == TRUE )
		{
			return;											/* �ʐM�ُ펞�́A�������Ȃ�				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�X�P�[���t�](Pn002.3)���� (36bit��Βl�f�[�^�̔��]����)										*/
/*--------------------------------------------------------------------------------------------------*/
/*	        16bit               20bit              12bit											*/
/*	 |<---------------->|<------------------->|<---------->|   										*/
/*   +------------------+---------------------+------------+   										*/
/*   | FencV->RxPosH[0] |  FencV->RxPosL.dw     xxxxxxxxxxxx| ����12bit�͕s��l						*/
/*   +------------------+---------------------+------------+   										*/
/*   |  PosH            |  PosL.dw             000000000000| ����12bit��0�Œ�l						*/
/*   +------------------+---------------------+------------+   										*/
/*--------------------------------------------------------------------------------------------------*/
		PosH    = FencV->RxPosH[0];
		PosL.dw = (FencV->RxPosL[0].dw >> FencV->MposSftR) << FencV->MposSftR; /* ����12bit��0�ɂ���	*/

		if( FencV->RevAsmMotor )							/* ���[�^���]�ڑ�Flag�`�F�b�N			*/
		{ 
			PosH    = (~PosH);								/* �������]����							*/
			PosL.dw = (~PosL.dw) + 1;						/* �������]����							*/
			if( PosL.dw == 0 ){ ++PosH;}					/* Carry�̏ꍇ�́A��ʂ�+1				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[���f�[�^�擾����																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( FencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_REQUEST )	/* ���b�`�v���R�}���h�`�F�b�N		*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		��Έʒu36Bit�f�[�^�̏��32Bit�f�[�^�����o���A�}���`�^�[���̕����l���v�Z����		*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur    = ((USHORT)PosH << 16 ) | ((USHORT)PosL.w.h );
			MtnRem    = 0;
			MtnCur    = MlibPfbkxremNolim( MtnCur, FencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (PosH << (32-FSX_FPOS_SFT)) | ((ULONG)PosL.dw >> FSX_FPOS_SFT);
			FencV->DivInfo.DivOutRem = 0;
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )	/* <V526> */
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			FencV->DivInfo.MturnLatch = (SHORT)( MtnCur >> 16);
			FencV->DivInfo.PsoMturnLatch=(SHORT)( MtnCur >> 16);/*<S189>*/
		/*------------------------------------------------------------------------------------------*/
		/*		�����p���X���Z�b�g����																*/
		/*------------------------------------------------------------------------------------------*/
			FencV->DivInfo.DivOutPos   = 0;				/* �����o�̓p���X���Z�b�g				*/
			FencV->DivInfo.IncPulseOut = 0;				/* �C���N���p���X�o�̓��Z�b�g			*/
			FencV->DivInfo.PaoSeqMcmd  = KPX_MTLATCH_FINISH;/* ���� ==> FencSencOutputPaoSerial()  */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�����C���N���p���X�o�͏���																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( FencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_REQUEST )/* �p���X�o�̓R�}���h�`�F�b�N	*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*	�}���`�^�[���f�[�^�̕ω������Z															*/
		/*	��Έʒu36Bit�f�[�^�̉���32Bit�f�[�^�����o���A�����C���N���̕����l���v�Z����			*/
		/*	MtnSepos�̒l�𔻒f���Ȃ���MtnCur���v�Z����B											*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur        = ((USHORT)PosH << 16 ) | ((USHORT)PosL.w.h );
			MtnRem        = 0;
			MtnCur        = MlibPfbkxremNolim( MtnCur, FencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (PosH << (32-FSX_FPOS_SFT)) | ((ULONG)PosL.dw >> FSX_FPOS_SFT);
			FencV->DivInfo.DivOutRem   = 0;				/* �����o�̓p���X�]�胊�Z�b�g			*/
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			MturnLatchCur = (SHORT)( MtnCur >> 16);
			MtnSub = (USHORT)MturnLatchCur - (USHORT)FencV->DivInfo.MturnLatch;
		/*------------------------------------------------------------------------------------------*/
		/*		��Έʒu36Bit�f�[�^�̉���32Bit�f�[�^�����o���A�����C���N���̕����l���v�Z����		*/
		/*------------------------------------------------------------------------------------------*/
			MtnSepos = (MtnSepos & 0x000FFFFF );

			if( FencV->DivInfo.RvsDir )						/* �t��]���[�h�`�F�b�N					*/
			{
				MtnSepos = MtnSepos | 0xFFF00000;			/* �������]����							*/
				MtnSepos = (ULONG)MtnSepos - ( MtnSub << (32-FSX_FPOS_SFT) );
			}
			else
			{
				MtnSepos = (ULONG)MtnSepos + ( MtnSub << (32-FSX_FPOS_SFT) );
			}
		/*------------------------------------------------------------------------------------------*/
		/*		�����C���N���p���X�o�̓f�[�^�`�F�b�N���o�̓f�[�^�ݒ�								*/
		/*------------------------------------------------------------------------------------------*/
			lwk = MtnSepos - FencV->DivInfo.IncPulseOut;
			if( lwk > FMAXPULSEOUT )						/* �p���X�o�͏��						*/
			{
				FencV->DivInfo.IncPulseOut += FMAXPULSEOUT;/* �p���X�o�̓f�[�^�ݒ�					*/
			}
			else if( lwk < -FMAXPULSEOUT )					/* �p���X�o�͉���						*/
			{
				FencV->DivInfo.IncPulseOut -= FMAXPULSEOUT;/* �p���X�o�̓f�[�^�ݒ�					*/
			}
			else											/* �ŏI�p���X�o��						*/
			{
				FencV->DivInfo.IncPulseOut = MtnSepos;		/* �p���X�o�̓f�[�^�ݒ�					*/
				FencV->DivInfo.PaoSeqMcmd  = KPX_IPLSOUT_FINISH;/* ���� ==> FencSencOutputPaoSerial*/
				FencV->DivInfo.IncPulseReq = FALSE;			/* �����C���N���p���X�o�͊���			*/
			}
			FencV->DivInfo.DivOutPos = FencV->DivInfo.IncPulseOut;	/* �����o�̓p���X�X�V			*/
		/*------------------------------------------------------------------------------------------*/
			pAsicHwReg->AREG_POSET0	= FencV->DivInfo.IncPulseOut;	/* �p���X�o�͉�H������			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�R�}���h����																				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;												/* �R�}���h�҂�							*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		�t���N�G���R�[�_�F��Έʒu�f�[�^(64bit)�擾						<S02D>						*/
/*																									*/
/****************************************************************************************************/
static void FencSencGetAbsPos( MENCV  *FencV, BOOL RvsDir )
{
ULONG	PosL;
USHORT	PosH;
LONG	AbsPos[2];

/*--------------------------------------------------------------------------------------------------*/
/*		�G���R�[�_�f�[�^���b�`																		*/
/*--------------------------------------------------------------------------------------------------*/
		PosH = FencV->RxPosH[0];						/* �X�L�����`�œǍ��񂾃f�[�^���g�p���āA	*/
		PosL = (ULONG)FencV->RxPosL[0].dw;				/* ��Έʒu(36Bit)���v�Z����B				*/

		AbsPos[1] = (LONG)((SHORT)PosH) >> FSX_FPOS_SFT;
		AbsPos[0] = (PosH<<(32-FSX_FPOS_SFT))+((ULONG)PosL>>FSX_FPOS_SFT);

		/*------------------------------------------------------------------------------------------*/
		if( RvsDir ^ FencV->RevAsmMotor )				/* �t��]���[�h XOR Fenc�t��]���[�h		*/
		{
			AbsPos[1] = (~AbsPos[1]);					/* �������]����								*/
			AbsPos[0] = (~AbsPos[0]) + 1;				/* �������]����								*/
			if( AbsPos[0] == 0 ){ ++AbsPos[1];}			/* Carry�̏ꍇ�́A��ʂ�+1					*/
		}
		FencV->SenOnScalePosH = AbsPos[1];
		FencV->SenOnScalePosL = AbsPos[0];
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		�o�`�n���}���`�^�[���f�[�^�o��								<S02D>							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �}���`�^�[���f�[�^���w�茅�Ŋ����������A�X�L�[�����ɕϊ����Ďw��|�[�g�֏o�͂��A		*/
/*			���̌����������c���ۑ����܂��B�w�茅���f�[�^�̍ŏ�ʂł��鎖���K�v�ł��B				*/
/*																									*/
/*			��j*data = 32767 �� �T����(digit=5)���o�͂���ꍇ										*/
/*																									*/
/*				�o�̓f�[�^ = '3'																	*/
/*				�ۑ��f�[�^ = 2767																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void FencSencOutputPaoMtdata( MENCV *FencV, ASIC_HW_IF_STR *pAsicHwReg, ULONG digit )
{
ULONG	x;
ULONG	outx;

/*--------------------------------------------------------------------------------------------------*/
/*		���f�[�^�ϊ�																				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
		case  1: digit = 1;     break;
		case  2: digit = 10;    break;
		case  3: digit = 100;   break;
		case  4: digit = 1000;  break;
		case  5: digit = 10000; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�}���`�^�[���f�[�^�o��																		*/
/*--------------------------------------------------------------------------------------------------*/
		x = FencV->DivInfo.PaoSeqData / digit;
		outx = MlibEvenAsciiTbl[ x ];
		pAsicHwReg->AREG_UARTXD  = (USHORT)outx;
		FencV->DivInfo.PaoSeqData -= (USHORT)(x * digit);
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		�t���N�G���R�[�_�F�o�`�n���V���A���o��							<S02D>						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �A�u�\�G���R�[�_�g�p���A�r�d�m�M���ɂ��}���`�^�[���f�[�^�y�уA���[����				*/
/*			�o�`�n����̃V���A���o�͏������s���B													*/
/*																									*/
/*			1) �o�̓t�H�[�}�b�g : �Vbit �����p���e�B												*/
/*				 �`�r�h�b�̂s�w�c�́A�p���e�B�̕t���y�тV�r�b�g�f�[�^�����M�̋@�\��					*/
/*				 �Ȃ��̂ŁA�p���e�B���\�t�g�I�ɕt�������W�r�b�g�f�[�^�Ƃ��đ��M����					*/
/*																									*/
/*			2) �}���`�^�[�� : �o�}����������[CR]	( �}@@@@@ : -32768 �` +32768 )					*/
/*																									*/
/*			3) �A���[��		: �`�k�l�����D[CR]		( @@ : A.@@ )									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	FencSencOutputPaoSerial( MENCV *FencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							     BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag )
{
	USHORT	uswk;
	ASIC_HW_IF_STR *pAsicHwReg2;/*<S0C2>*/
	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address <S0C2>*/


/*--------------------------------------------------------------------------------------------------*/
/*		�@�\�L���`�F�b�N																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( SVFSEL_PAOPOUT == 0 )   { return;}				/* Mask Function						*/

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�I�����V�[�P���X������																*/
/*--------------------------------------------------------------------------------------------------*/
		if( DbBeReq == TRUE )
		{
			FencV->DivInfo.PaoSeqStep = 0;
			return;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�o�`�n�V���A���o�̓V�[�P���X																*/
/*--------------------------------------------------------------------------------------------------*/
		switch( FencV->DivInfo.PaoSeqStep )
		{
	/*----------------------------------------------------------------------------------------------*/
		case  0: /* Initial Step																	*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqWait = 0;
				FencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				if( FencV->PaoseqNormal == TRUE )				/* �d�������㏉�񓮍�?				*/
				{
					FencV->DivInfo.PaoSeqStep = (SenReq == FALSE)? 1 : 20;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;				/* �d�������㏉���SEN Lo->Hi��҂�	*/
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  1: /* Wait SEN Lo --> Hi (Output Multi-turn Data)										*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->PaoseqNormal = TRUE;						/* ���񓮍샂�[�h����				*/
				FencV->SenReady = FALSE;
				FencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				FencV->DivInfo.IncPulseReq = TRUE;
				if( SenReq == TRUE )
				{
					/*------------------------------------------------------------------------------*/
					/*		�c�`�o�̓��[�h�F�s�w�c�o��												*/
					/*------------------------------------------------------------------------------*/
					/*		Bit13 :	DASEL(�����o��DA�[�q�I��)	:  0 : DA,  1: TXD_1				*/
					/*------------------------------------------------------------------------------*/
					pAsicHwReg->AREG_PFNCSET  |= BIT13;			/* �c�`�o�̓��[�h�F�s�w�c�o��		*/
					pAsicHwReg2->AREG_PFNCSET &= ~BIT13;		/*PSO NO way <S0C2>	    */
					/*------------------------------------------------------------------------------*/
					pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;		/* �p���X�o�͉�H������				*/
					pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;		/* �p���X�o�͉�H������				*/

					pAsicHwReg->AREG_DINCSET  |= BIT10;			/* �����펞�N���A�r�b�g���Ă�		*/
					pAsicHwReg->AREG_WDT1SET |= BIT8;			/* �p���X�ϊ��b���}�X�N�n�m			*/
					FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;/* �p���X�ϊ��b���}�X�N�n�m		*/

					FencV->DivInfo.PaoSeqStep = 2;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  2: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( ++FencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
				{
					pAsicHwReg->AREG_DINCSET |= BIT5;			/* �b���ē�����						*/
					pAsicHwReg->AREG_DINCSET &= ~(BIT10);		/* �����펞�N���A���Ƃ�				*/
					pAsicHwReg->AREG_DINCSET &= ~(BIT5);		/* �b���ē�������					*/
					FencV->DivInfo.PaoSeqWait = 0;
					FencV->DivInfo.PaoSeqStep = 3;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  3: /* Multiturn Latch Request															*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqMcmd = KPX_MTLATCH_REQUEST;/* MultiturnLatch�w�� ==> ScanA	*/
				FencV->DivInfo.PaoSeqStep = 4;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  4: /* Wait Multiturn Latch															*/
	/*----------------------------------------------------------------------------------------------*/
				if( FencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_FINISH )	/* MultiturnLatch�����H		*/
				{
					FencV->DivInfo.PaoSeqData = FencV->DivInfo.MturnLatch;
					FencV->DivInfo.PaoSeqStep = 5;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  5: /* Output 'P'																		*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	UARTXD:�ėp�񓯊��|�[�g���M�f�[�^�F�ėp�񓯊��|�[�g���M�f�[�^�r�b�g�V-0   	    */
				/*----------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_UARTXD  = 'P';
				FencV->DivInfo.PaoSeqStep = 6;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  6: /* Output '+' or '-'																*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	�t��]���[�h																	*/
				/*----------------------------------------------------------------------------------*/
				if( FencV->DivInfo.RvsDir )					/* �t��]���[�h						*/
				{
					if( (SHORT)FencV->DivInfo.PaoSeqData >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = '-';
						FencV->DivInfo.PaoSeqData = FencV->DivInfo.PaoSeqData + 1;
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = '+';
						FencV->DivInfo.PaoSeqData = ~FencV->DivInfo.PaoSeqData;
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	����]���[�h																	*/
				/*----------------------------------------------------------------------------------*/
				else
				{
					if( (SHORT)FencV->DivInfo.PaoSeqData >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = '+';
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = '-';
						FencV->DivInfo.PaoSeqData = ~FencV->DivInfo.PaoSeqData + 1;
					}
				}
				FencV->DivInfo.PaoSeqStep = 7;
				break;
	/*----------------------------------------------------------------------------------------------*/
	/*			�P�O�i�f�[�^�o�� : �T����,�S����,�R����,�Q����,�P����								*/
	/*----------------------------------------------------------------------------------------------*/
		case  7: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 5 ); FencV->DivInfo.PaoSeqStep =  8; break;
		case  8: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 4 ); FencV->DivInfo.PaoSeqStep =  9; break;
		case  9: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 3 ); FencV->DivInfo.PaoSeqStep = 10; break;
		case 10: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 2 ); FencV->DivInfo.PaoSeqStep = 11; break;
		case 11: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 1 ); FencV->DivInfo.PaoSeqStep = 12; break;
	/*----------------------------------------------------------------------------------------------*/
		case 12: /* Output '0x0D'																	*/
	/*----------------------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
				FencV->DivInfo.PaoSeqStep = 13;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 13: /* Wait 2ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqStep = 14;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 14: /* IncPulse Output Request															*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*		�c�`�o�̓��[�h�F�p���X�o��										   		    */
				/*----------------------------------------------------------------------------------*/
				/*		Bit13 :	DASEL(�����o��DA�[�q�I��)	:  0 : DA,  1: TXD_1				    */
				/*----------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* �c�`�o�̓��[�h�F�p���X�o��		*/
				/*----------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_REQUEST;/* �����C���N���o�͎w�� ==> ScanA	*/
				FencV->DivInfo.PaoSeqStep = 15;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 15: /* Check IncPulse Output Complete													*/
	/*----------------------------------------------------------------------------------------------*/
				if( FencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_FINISH )	/* �����C���N���o�͊����H	*/
				{
					FencV->SenReady = TRUE;
					FencSencGetAbsPos( FencV, Bprm->RvsDir );	/* ��Έʒu�f�[�^�̎擾	*/
					pAsicHwReg->AREG_WDT1SET &= ~BIT8;			/* ��ٽ�ϊ��b��Ͻ��n�e�e  			*/
					FencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
					FencV->DivInfo.PaoSeqStep = 0;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 20: /* Wait SEN Hi --> Lo (Output Alarm)												*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					FencV->SenReady = FALSE;
					FencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
					FencV->DivInfo.IncPulseReq = TRUE;
					if( AlmFlag )								/* �A���[��������					*/
					{
						pAsicHwReg->AREG_PFNCSET  |= BIT13;		/* �c�`�o�̓��[�h�F�s�w�c�o��		*/
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/
						pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;	/* �p���X�o�͉�H������				*/

						FencV->DivInfo.PaoSeqData = AlmCode;	/* �A���[���R�[�h�擾				*/
						FencV->DivInfo.PaoSeqStep = 21;			/* �A���[���o�̓X�e�b�v�ɑJ��		*/
					}
					else										/* �A���[�������Ȃ�					*/
					{
						FencV->DivInfo.PaoSeqStep =  1;			/* Lo-->Hi�Ď��X�e�b�v�ɑJ��		*/
					}
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 21: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					if( ++FencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						FencV->DivInfo.PaoSeqWait = 0;
						FencV->DivInfo.PaoSeqStep = 22;
					}
				}
				else
				{
					FencV->DivInfo.PaoSeqWait = 0;
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 22: /* Output 'A'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = 'A';
					FencV->DivInfo.PaoSeqStep = 23;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 23: /* Output 'L'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = ('L' + 0x80);
					FencV->DivInfo.PaoSeqStep = 24;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 24: /* Output 'M'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = 'M';
					FencV->DivInfo.PaoSeqStep = 25;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 25: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((FencV->DivInfo.PaoSeqData >> 8) & 0x0F)];
					pAsicHwReg->AREG_UARTXD  = uswk;
					FencV->DivInfo.PaoSeqStep = 26;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 26: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((FencV->DivInfo.PaoSeqData>> 4) & 0x0F)];
					pAsicHwReg->AREG_UARTXD  = uswk;
					FencV->DivInfo.PaoSeqStep = 27;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 27: /* Output '.'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = '.';
					FencV->DivInfo.PaoSeqStep = 28;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 28: /* Output 0x0D																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
					FencV->DivInfo.PaoSeqStep = 29;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 29: /* End of Output Alarm																*/
	/*----------------------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* �c�`�o�̓��[�h�F�p���X�o��		*/
				FencV->DivInfo.PaoSeqStep = 1;
				break;
	/*----------------------------------------------------------------------------------------------*/
		default: /* Default																			*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqStep = 0;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}
#endif /*<S02D>*/

/* <S1B7> >>>>> */
/*****************************************************************************
  Description : ���[�^���X�F�G���R�[�_�ϐ�����������
----------------------------------------------------------------------------
  Parameters:
	MencV;					���[�^�G���R�[�_�f�[�^�ւ̃|�C���^
	AsicHwReg;				Asic H/W ���W�X�^�A�h���X
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxSencMotLessInitVariables( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg )
{

	/*----------------------------------------------------------------------*/
	/*	����ʒu�f�[�^(����,�O��,�O�X��)�̐ݒ�								*/
	/*----------------------------------------------------------------------*/
	MencV->MotPos  = 0;				/* Motor Position	[32bitRing]			*/
	MencV->MotPhaX = 0;				/* Motor PhasePos.	[PulseNo/360deg]	*/
	MencV->MotCposX = 0;			/* Motor CphasePos.	[32bitRing]			*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosH[1]  = MencV->RxPosH[0] = 0;	/* �O��C���N���␳�l�����ݒ�	*/
	MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = 0;
	MencV->MotPosX[0] = MencV->MotPosX[1] = 0;

	/*----------------------------------------------------------------------*/
	/*	�����Έʒu�f�[�^													*/
	/*----------------------------------------------------------------------*/
	MencV->MotAbsPos = 0;

	/*----------------------------------------------------------------------*/
	/*	����}���`�^�[���f�[�^�̐ݒ�										*/
	/*----------------------------------------------------------------------*/
	MencV->IniMturn = 0;
	MencV->IniIncre = 0;

	/*----------------------------------------------------------------------*/
	/*	�ʑ����_�̏����ݒ�													*/
	/*----------------------------------------------------------------------*/
	MencV->MotOrgX  = 0;			/* ���[�^���_							*/
	MencV->MotCposX = 0;			/* ���[�^�b���ʒu						*/
	MencV->MotAngle = 0;			/* ���[�^��]�p(�p���X)					*/
	MencV->PhaseOffset = 0;			/* �|�[���Z���T�I�t�Z�b�g�� 			*/

	/*----------------------------------------------------------------------*/
	/*	���[�^���_�`�F�b�N�p�ϐ��̏�����									*/
	/*----------------------------------------------------------------------*/
	MencV->MotOrgChkEnd = TRUE;		/* Set Motor Origin Check End Flag		*/

	/*----------------------------------------------------------------------*/
	/*	�����p���X�����ݒ�													*/
	/*----------------------------------------------------------------------*/
	MencV->DivInfo.DivOutRem = 0;
	MencV->DivInfo.DivOutPos = 0;
	MencV->DivInfo.InitDivPosOffset = 0;
	if( MencV->DivInfo.DivOut == TRUE )
	{
		AsicHwReg->AREG_POSET0 = (USHORT)MencV->DivInfo.DivOutPos;
		AsicHwReg->AREG_POTEST = (USHORT)MencV->DivInfo.DivOutPos;
	}

	/*----------------------------------------------------------------------*/
	/*	�b���t���O�֘A														*/
	/*----------------------------------------------------------------------*/
	MencV->Cset = TRUE;				/* �b���ݒ�Flag							*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		MencV->CphaseLess = TRUE;	/* �b�����XFlag							*/
	}
//[org]	KPI_PCMASK_OFF( );					/* �p���X�ϊ�C���}�X�NOFF�ݒ�	*/
	AsicHwReg->AREG_WDT1SET &= ~BIT8;		/* TODO: �K�v���I�I				*/
	MencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;	/* �p���X�ϊ�C���}�X�NOFF�ݒ�	*/

	/*----------------------------------------------------------------------*/
	/*	�G���R�[�_���f�B�ݒ�												*/
	/*----------------------------------------------------------------------*/
	MencV->PhaseReady = TRUE;		/* �ʑ����o����Flag						*/

	return;
}


/*****************************************************************************
  Description : ���[�^���X�F��]�^�ʒu�f�[�^�Ǎ���
----------------------------------------------------------------------------
  Parameters:
	MencV;					���[�^�G���R�[�_�f�[�^�ւ̃|�C���^
	pAsicHwReg;				Asic H/W ���W�X�^�A�h���X
	AlmManager;				�A���[���Ǘ��h�e�f�[�^��`�ւ̃|�C���^
	BeComplete;				�x�[�X�C�l�[�u���������
	BaseLoops;				���䃋�[�v���Z�p�f�[�^�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void RmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops )
{
	BPRMDAT		*Bprm;					/* ��{�p�����[�^��`�ւ̃|�C���^			*/
	MENCPRM		*MencP;					/* �G���R�[�_�p�����[�^��`�ւ̃|�C���^		*/
	MOTLESSTST	*MotorLessTest;			/* ���[�^���X�e�X�g���[�hIF��`�ւ̃|�C���^	*/
	MOTOR_LESS	*MotorLessTestDatas;	/* �G���R�[�_IF�p�f�[�^�ւ̃|�C���^			*/
	SENCX		*Enc;					/* �V���A���G���R�[�_IF��`�ւ̃|�C���^		*/

	LONG		dEncPos;				/* �G���R�[�_�ʒu����				*/
	LONG		dMotPos;				/* �ʒu����							*/
	LONG		RcvPosX;				/* �G���R�[�_����ʒu				*/
	LONG		LastMotPosX;			/* �O��ʒu							*/
	SHORT		Mturn;					/* �}���`�^�[��						*/

	/* �ϐ�������	*/
	Bprm				= ((BASE_LOOPCTRLS*)BaseLoops)->Bprm;
	MotorLessTest		= ((BASE_LOOPCTRLS*)BaseLoops)->MotorLessTest;
	MencP				= &(MotorLessTest->Prm->MencP);
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);
	Enc					= &(MencV->EncIf);

	LastMotPosX = MencV->MotPosX[0];

	/*----------------------------------------------------------------------*/
	/*	���[�^���X�e�X�g���[�h�p�ʒu�f�[�^�ݒ�֐����s						*/
	/*----------------------------------------------------------------------*/
	ApiSetMechaDatasMotorLessTestMode( Bprm, MotorLessTest );

	/*----------------------------------------------------------------------*/
	/*	�O��̈ʒu�f�[�^�ޔ�												*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[1] = MencV->RxPosL[0];			/* �O��l�X�V			*/
	MencV->RxPosH[1] = MencV->RxPosH[0];			/* �O��l�X�V			*/

	/*----------------------------------------------------------------------*/
	/*  ���[�^���f�������ʒu�����f�[�^�擾								*/
	/*----------------------------------------------------------------------*/
	dEncPos = (LONG)MotorLessTestDatas->dSemiModePos << MencV->MposSftX;
	MlibAdd6432( MotorLessTestDatas->SencData.Long, dEncPos, MotorLessTestDatas->SencData.Long );

	/*----------------------------------------------------------------------*/
	/*  �}���`�^�[�����~�b�g����											*/
	/*----------------------------------------------------------------------*/
//	if( MencV->AxisMotType == MOTTYPE_ROTARY )
//	{
//	}

	if( MencP->limmlt != 65535 )
	{
		Mturn = LpxGetSencMotLessMturnData( MencV, MencP,
											MotorLessTestDatas->SencData.EncFormat.RxPosH,
											MotorLessTestDatas->SencData.EncFormat.RxPosL.dw );
		if( Mturn > MencP->limmlt )
		{
			/* �}���`�^�[�����~�b�g�𒴂����ꍇ �� �}���`�^�[���f�[�^��0��ݒ�	*/
			LpxSencMotLessMultiTurnSet( MencP, &MotorLessTestDatas->SencData, 0 );
		}
		else if( Mturn < 0 )
		{
			/* �}���`�^�[�������̏ꍇ �� �}���`�^�[���f�[�^�Ƀ}���`�^�[�����~�b�g��ݒ�		*/
			LpxSencMotLessMultiTurnSet( MencP, &MotorLessTestDatas->SencData, MencP->limmlt );
		}
	}

	/*----------------------------------------------------------------------*/
	/*  �ʒu�f�[�^���G���R�[�_��M�f�[�^�ɃZ�b�g							*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[0].dw = MotorLessTestDatas->SencData.EncFormat.RxPosL.dw;
	MencV->RxPosH[0]    = MotorLessTestDatas->SencData.EncFormat.RxPosH;

	/*----------------------------------------------------------------------*/
	/*	�ʒu���Z															*/
	/*----------------------------------------------------------------------*/
	RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long>>MencV->MposSftX)<<MencV->MposSftR);

	/*----------------------------------------------------------------------*/
	/*	����ʒu�f�[�^�A�O��ʒu�f�[�^�X�V									*/
	/*----------------------------------------------------------------------*/
	MencV->SeRxErr    = FALSE;				/* Reset Error Flag				*/
	MencV->EncDataNG  = FALSE;				/* Reset Encoder Data NG Flag	*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];	/* �O��ʒu�f�[�^				*/
	MencV->RcvPosX[0] = RcvPosX;			/* ����ʒu�f�[�^				*/

	/*----------------------------------------------------------------------*/
	/*	���[�^�ʒu�o�͉��Z													*/
	/*----------------------------------------------------------------------*/
	MencV->MotPosX[1] = MencV->RcvPosX[1];	/* �O��ʒu�f�[�^				*/
	MencV->MotPosX[0] = MencV->RcvPosX[0];	/* ����ʒu�f�[�^				*/

	/*----------------------------------------------------------------------*/
	/*	���[�^�ʒu�o��(32bitRingPulse)										*/
	/*----------------------------------------------------------------------*/
	dMotPos = ( (MencV->MotPosX[0] - LastMotPosX) >> MencV->MposSftR );
	MencV->MotPos    += dMotPos;
	MencV->MotAbsPos += dMotPos;

	// ������ړ�
	/*----------------------------------------------------------------------*/
	/*	���[�^�d���ʑ��X�V													*/
	/*----------------------------------------------------------------------*/
	MencV->MotPhase = (USHORT)(( (((MencV->MotPosX[0]) - (MencV->MotOrgX)) >>8) * MencV->Kmotphase)>>8);

	/*----------------------------------------------------------------------*/
	/*	�t�B�[�h�o�b�N�p���X�o�͏���										*/
	/*----------------------------------------------------------------------*/
	//	���̏�����Mercury�ɂ͑��݂��Ȃ�
	/* <S20D> Start */
	if( MencV->DivInfo.IncPulseReq == TRUE )	/* �����C���N���o�͗v���H	*/
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* ���j�A�^ */
			/* �����C���N���p���X�o�͏��� */
			LmxOutSencIniIncPulse(MencV, pAsicHwReg, LMX_MPOS_SFT);
		}
		else
		{ /* ��]�^ */
			/* �����C���N���p���X�o�͏��� */
			SencOutIniIncPulse( MencV, pAsicHwReg );
		}
	}

	if((MencV->DivInfo.DivOutASIC != FALSE) && (MencV->DivInfo.IncPulseReq == FALSE))
	{ /* JL-086�ɂĕ����̏ꍇ�A�����C���N���p���X�o�͊�����JL-086�ɂĕ������� */
		Enc->SencSetReg->AREG_DIVSET &= ~BIT1;
	}
	/* <S20D> End */

	/*----------------------------------------------------------------------*/
	/*	���_�ʉ߃`�F�b�N����												*/
	/*----------------------------------------------------------------------*/
	if( (MencV->OrgOk == FALSE) && (MencV->SeRxErr == FALSE) )
	{
		RmxCheckSencOriginPass( MencV );		/* ���_�ʉ߃`�F�b�N	*/
	}

//	/*----------------------------------------------------------------------*/
//	/*	���[�^�d���ʑ��X�V													*/
//	/*----------------------------------------------------------------------*/
//	// ��Ɉړ�

	/*----------------------------------------------------------------------*/
	/*	�����p���X�o�͏���													*/
	/*----------------------------------------------------------------------*/
	// �����C���N���o�͂Ȃ�
	if( MencV->DivInfo.DivOut == TRUE )
	{
		SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
	}
	/*----------------------------------------------------------------------*/
	return;

}


/*****************************************************************************
  Description : ���[�^���X�F���j�A�ʒu�f�[�^�Ǎ���
----------------------------------------------------------------------------
  Parameters:
	MencV;					���[�^�G���R�[�_�f�[�^�ւ̃|�C���^
	pAsicHwReg;				Asic H/W ���W�X�^�A�h���X
	AlmManager;				�A���[���Ǘ��h�e�f�[�^��`�ւ̃|�C���^
	BeComplete;				�x�[�X�C�l�[�u���������
	BaseLoops;				���䃋�[�v���Z�p�f�[�^�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
	�b���(�쐬��)
*****************************************************************************/
void LmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops )
{
	BPRMDAT		*Bprm;					/* ��{�p�����[�^��`�ւ̃|�C���^			*/
	MOTLESSTST	*MotorLessTest;			/* ���[�^���X�e�X�g���[�hIF��`�ւ̃|�C���^	*/
	MOTOR_LESS	*MotorLessTestDatas;	/* �G���R�[�_IF�p�f�[�^�ւ̃|�C���^			*/
	SENCX		*Enc;					/* �V���A���G���R�[�_IF��`�ւ̃|�C���^		*/

	LONG		dEncPos;				/* �G���R�[�_�ʒu����				*/
	LONG		dMotPos;				/* �ʒu����							*/
	LONG		RcvPosX;				/* �G���R�[�_����ʒu				*/
	LONG		LastMotPosX;			/* �O��ʒu							*/

	/* �ϐ�������	*/
	Bprm				= ((BASE_LOOPCTRLS*)BaseLoops)->Bprm;
	MotorLessTest		= ((BASE_LOOPCTRLS*)BaseLoops)->MotorLessTest;
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);
	Enc					= &(MencV->EncIf);

	LastMotPosX = MencV->MotPosX[0];

	/*----------------------------------------------------------------------*/
	/*	���[�^���X�e�X�g���[�h�p�ʒu�f�[�^�ݒ�֐����s						*/
	/*----------------------------------------------------------------------*/
	ApiSetMechaDatasMotorLessTestMode( Bprm, MotorLessTest );

	/*----------------------------------------------------------------------*/
	/*	�O��̈ʒu�f�[�^�ޔ�												*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[1] = MencV->RxPosL[0];			/* �O��l�X�V			*/
	MencV->RxPosH[1] = MencV->RxPosH[0];			/* �O��l�X�V			*/

	/*----------------------------------------------------------------------*/
	/*  ���[�^���f�������ʒu�����f�[�^�擾								*/
	/*----------------------------------------------------------------------*/
//TODO:(Sigma-V)	dEncPos = (LONG)MotorLessTestDatas->dSemiModePos << MencV->MposSftLm;
	dEncPos = (LONG)MotorLessTestDatas->dSemiModePos << MencV->MposSftX;
	MlibAdd6432( MotorLessTestDatas->SencData.Long, dEncPos, MotorLessTestDatas->SencData.Long );

	/*----------------------------------------------------------------------*/
	/*  �ʒu�f�[�^���G���R�[�_��M�f�[�^�ɃZ�b�g							*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[0].dw = MotorLessTestDatas->SencData.EncFormat.RxPosL.dw;
	MencV->RxPosH[0]    = MotorLessTestDatas->SencData.EncFormat.RxPosH;

	/*----------------------------------------------------------------------*/
	/*	�ʒu���Z															*/
	/*----------------------------------------------------------------------*/
//TODO:(Sigma-V)	RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long>>MencV->MposSftLm)<<MencV->MposSftLm);
	RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long>>MencV->MposSftX)<<MencV->MposSftR);

	/*----------------------------------------------------------------------*/
	/*	����ʒu�f�[�^�A�O��ʒu�f�[�^�X�V									*/
	/*----------------------------------------------------------------------*/
	MencV->SeRxErr    = FALSE;				/* Reset Error Flag				*/
	MencV->EncDataNG  = FALSE;				/* Reset Encoder Data NG Flag	*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];	/* �O��ʒu�f�[�^				*/
	MencV->RcvPosX[0] = RcvPosX;			/* ����ʒu�f�[�^				*/

	/*----------------------------------------------------------------------*/
	/*	���[�^�ʒu�o�͉��Z													*/
	/*----------------------------------------------------------------------*/
	MencV->MotPosX[1] = MencV->RcvPosX[1];	/* ��]�`�Ə��������킹�邽��	*/
	MencV->MotPosX[0] = MencV->RcvPosX[0];	/* ��]�`�Ə��������킹�邽��	*/
	/*----------------------------------------------------------------------*/
//TODO:(Sigma-V)	dMotPos = (MencV.MotPosX[0] - LastMotPosX) >> MencV->MposSftLm;
	dMotPos = ( (MencV->MotPosX[0] - LastMotPosX) >> MencV->MposSftR );

	MencV->MotPos    += dMotPos;	/* Motor Position (32bitRingPulse)		*/
	MencV->MotAbsPos += dMotPos;	/* Motor Abs Position (32bitRingPulse)	*/

	/*----------------------------------------------------------------------*/
	/*	���[�^�d���ʑ��X�V													*/
	/*----------------------------------------------------------------------*/
	LmxUpdateMotPhase( MencV, AlmManager, dMotPos );

	/*----------------------------------------------------------------------*/
	/*	�����C���N���p���X�o�͏��� or �G���R�[�_�b���ʉ߃`�F�b�N����		*/
	/*----------------------------------------------------------------------*/
	if( MencV->DivInfo.IncPulseReq == TRUE )		/* �����C���N���o�͗v���H		*/
	{
//TODO:(Sigma-V)		LmxOutSencIniIncPulse( MencV->MposSftLm );	/* �����C���N���p���X�o�͏���	*/
	}
	else
	{
		if( MencV->SeRxErr == FALSE )
		{
			LmxCheckSencCphasePass( MencV );	/* �G���R�[�_�b���ʉ߃`�F�b�N����	*/
		}
		/*------------------------------------------------------------------*/
		/*	�����p���X�o��													*/
		/*------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			MencV->DivInfo.DivOutPos += MlibPfbkxremNolim( dMotPos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
			SHalDrv_SetDivOutPosition( Enc->SencSetReg, MencV->DivInfo.DivOutPos );
		}
	}
	/*----------------------------------------------------------------------*/
	return;
}


/*****************************************************************************
  Description : ���[�^���X�F�G���R�[�_�p�����[�^�ݒ菈��
	�G���R�[�_���ڑ����A���z���[�^�p�����[�^�ݒ��̃G���R�[�_����ݒ肷��
----------------------------------------------------------------------------
  Parameters:
	MencV;					���[�^�G���R�[�_�f�[�^�ւ̃|�C���^
	MencP;					�G���R�[�_�p�����[�^��`�ւ̃|�C���^
	b_prm2;					�@�\�I�����p�X�C�b�`�Q
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxSetSencInfoMotorLessTestMode( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 )
{

	/*----------------------------------------------------------------------*/
	/*	�G���R�[�_���ڑ����G���R�[�_�^�C�v�I��								*/
	/*----------------------------------------------------------------------*/
	if( MencP->typm_e.b.h & 0x10 )
	{
		MencV->AbsoEncoder = TRUE;
//TODO:		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w);
// ��L�֐����g�p���������AMotorPrmCfg.c�ɁAstatic�錾����Ă��ė��p�ł��Ȃ����߁A�W�J���Ďg�p
		if( 1 == ((b_prm2>>8) & 0x0F) )
		{	/* �A�u�\���C���N���Ƃ��Ďg�p	*/
			MencV->AbsoEncType = MENC_INCRE_TYPE;
		}
		else
		{	/* �A�u�\���A�u�\�Ƃ��Ďg�p		*/
			MencV->AbsoEncType = (MencP->typm_e.w & 0x1000)? MENC_MABSO_TYPE : MENC_SABSO_TYPE;
		}
	}
	else
	{
		MencV->AbsoEncoder = FALSE;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
	}
	/*----------------------------------------------------------------------*/
	/*	�V���A���R���o�[�^�X�P�[���s�b�`��񖳌��ݒ�						*/
	/*----------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		MencV->ScalePitchExp = 0xFF;
	}

		return;
}


/*****************************************************************************
  Description : ���[�^���X�F�}���`�^�[���f�[�^�擾����
	�G���R�[�_�f�[�^���珬���_�ʒu�𗘗p���ă}���`�^�[���f�[�^(16bit)�����o���B
----------------------------------------------------------------------------
  Parameters:
	MencV;					���[�^�G���R�[�_�f�[�^�ւ̃|�C���^
	MencP;					�G���R�[�_�p�����[�^��`�ւ̃|�C���^
	PosH;					�ʒu�f�[�^(H)
	PosL;					�ʒu�f�[�^(L)
  Return:
	�}���`�^�[���f�[�^(16bit)
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
static USHORT LpxGetSencMotLessMturnData( MENCV *MencV, MENCPRM *MencP, USHORT PosH, ULONG PosL )
{
	USHORT	Mturn;						/* �}���`�^�[���f�[�^	*/

	Mturn = (USHORT)(((PosH<<16) | ((ULONG)PosL>>16)) >> (MencP->bit_dp.b.h - 64));
	if( MencV->RevAsmMotor )
	{
		if( MencP->limmlt == 65535 )
		{
			Mturn = -Mturn;
		}
		else if( Mturn != 0 )
		{
			Mturn = MencP->limmlt - Mturn;
		}
	}

	return( Mturn );

}


/*****************************************************************************
  Description : ���[�^���X�F�}���`�^�[���f�[�^�ݒ菈��
	�}���`�^�[�����~�b�g���l�����āA�G���R�[�_�f�[�^�Ƀ}���`�^�[���f�[�^��ݒ肷��
----------------------------------------------------------------------------
  Parameters:
	MencP;					�G���R�[�_�p�����[�^��`�ւ̃|�C���^
	EncData;				�G���R�[�_�f�[�^�ւ̃|�C���^
	MturnLimit;				�}���`�^�[���f�[�^�l
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
static void LpxSencMotLessMultiTurnSet(MENCPRM *MencP, ENCODER_DATA *EncData, SHORT MturnLimit)
{
	DWORDX	MturnData;					/* �}���`�^�[���f�[�^	*/
	UCHAR	Dpoint;						/* �ʒu�f�[�^�����_�ʒu	*/
	UCHAR	Sftx;						/* �V�t�g��				*/

	/* �ϐ������� */
	Dpoint = MencP->bit_dp.b.h;
	Sftx   = 80 - Dpoint;

	/*----------------------------------------------------------------------*/
	/*	�}���`�^�[���f�[�^�v�Z												*/
	/*----------------------------------------------------------------------*/
	MturnData.dw = MturnLimit << (Dpoint-64);

	/*----------------------------------------------------------------------*/
	/*	�}���`�^�[���̈�N���A												*/
	/*----------------------------------------------------------------------*/
	EncData->EncFormat.RxPosH = 0;
	EncData->EncFormat.RxPosL.Ulong = ( EncData->EncFormat.RxPosL.Ulong << Sftx) >> Sftx;

	/*----------------------------------------------------------------------*/
	/*	�}���`�^�[���̈�f�[�^�Z�b�g										*/
	/*----------------------------------------------------------------------*/
	EncData->EncFormat.RxPosH     |= MturnData.w.h;
	EncData->EncFormat.RxPosL.w.h |= MturnData.w.l;

	return;
}


/*****************************************************************************
  Description : ���[�^���X�F�G���R�[�_�ϐ�����������
----------------------------------------------------------------------------
  Parameters:
	FencV;					�t���N�G���R�[�_�f�[�^�ւ̃|�C���^
	AsicHwReg;				Asic H/W ���W�X�^�A�h���X
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxFencMotLessInitVariables( MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg )
{

	/*----------------------------------------------------------------------*/
	/*	����ʒu�f�[�^(����,�O��)�̐ݒ�										*/
	/*----------------------------------------------------------------------*/
	FencV->MotPos = 0;						/* �t���N�ʒu [32bitRingPulse]	*/
	/*----------------------------------------------------------------------*/
	FencV->RxPosH[1] = FencV->RxPosH[0] = 0;
	FencV->MotPosX[0] = 0;
	FencV->MotPosX[1] = 0;

	/*----------------------------------------------------------------------*/
	/*	�����Έʒu�f�[�^													*/
	/*----------------------------------------------------------------------*/
	FencV->MotAbsPos = 0;

	/*----------------------------------------------------------------------*/
	/*	�����p���X�����ݒ�													*/
	/*----------------------------------------------------------------------*/
	FencV->DivInfo.DivOutRem = 0;
	FencV->DivInfo.DivOutPos = 0;

	/*----------------------------------------------------------------------*/
	/*	�b���t���O�֘A														*/
	/*----------------------------------------------------------------------*/
	FencV->Cset = TRUE;						/* �b���ݒ�Flag					*/
	FencV->CposOk = TRUE;					/* �b���ʒu�ݒ芮��Flag			*/
	FencV->CphaseLess = TRUE;
	/*----------------------------------------------------------------------*/
	if( FencV->DivInfo.DivOut == TRUE )
	{
		AsicHwReg->AREG_POSET0 = (USHORT)FencV->DivInfo.DivOutPos;
		AsicHwReg->AREG_POTEST = (USHORT)FencV->DivInfo.DivOutPos;
	}

	/*----------------------------------------------------------------------*/
	/*	�G���R�[�_���f�B�ݒ�												*/
	/*----------------------------------------------------------------------*/
	//�����Ȃ�
	/*----------------------------------------------------------------------*/

	/*----------------------------------------------------------------------*/
	/*	�@�\�L���I���F�C���N���p���X�o�͗v�����Z�b�g						*/
	/*----------------------------------------------------------------------*/
	//�����Ȃ�
	/*----------------------------------------------------------------------*/

	return;
}


/*****************************************************************************
  Description : ���[�^���X�F�G���R�[�_�p�����[�^�ݒ菈��
	�G���R�[�_���ڑ����A�G���R�[�_����ݒ肷��
----------------------------------------------------------------------------
  Parameters:
	FencV;					�t���N�G���R�[�_�f�[�^�ւ̃|�C���^
	MencP;					�G���R�[�_�p�����[�^��`�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxSetFencInfoMotorLessTestMode( MENCV *FencV, MENCPRM *MencP )
{
	/*----------------------------------------------------------------------*/
	/*	�G���R�[�_�r�b�g���A�����_�ʒu										*/
	/*----------------------------------------------------------------------*/
	//�����Ȃ�
	/*----------------------------------------------------------------------*/
	/*	�G���R�[�_���ڑ����G���R�[�_�^�C�v�I��								*/
	/*----------------------------------------------------------------------*/
	FencV->AbsoEncoder = FALSE;

	/*----------------------------------------------------------------------*/
	/*	��Βl�G���R�[�_�g�p���@�̐ݒ�										*/
	/*----------------------------------------------------------------------*/
	FencV->AbsoEncType = MENC_INCRE_TYPE;

	/*----------------------------------------------------------------------*/
	/*	�d���c�����^�C�v�̑I��												*/
	/*----------------------------------------------------------------------*/
	//FencP.f.EndatType = FALSE;

	/*----------------------------------------------------------------------*/
	/*	�G���R�[�_�^�C�v�^����\�ݒ�										*/
	/*----------------------------------------------------------------------*/
	FencV->EncType = (0x0000 | MencP->BitnoDpoint.b.l);

	return;
}


/*****************************************************************************
  Description : ���[�^���X�F�ʒu�f�[�^�Ǎ���
----------------------------------------------------------------------------
  Parameters:
	FencV;					�t���N�G���R�[�_�f�[�^�ւ̃|�C���^
	AsicHwReg;				Asic H/W ���W�X�^�A�h���X
	Axis;					���n���h��
  Return:
	�ʒu�f�[�^
----------------------------------------------------------------------------
  Note:
	�b���(�쐬��)
*****************************************************************************/
LONG LpxReadMotorLessFencPosition( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops )
{
	MOTLESSTST	*MotorLessTest;
	MOTOR_LESS	*MotorLessTestDatas;	/* �G���R�[�_IF�p�f�[�^�ւ̃|�C���^	*/

	LONG		dEncPos;				/* �G���R�[�_�ʒu����				*/
	LONG		dFencPos;				/* �ʒu����							*/
	LONG		FencSpd; 				/* �O��PG���x						*/

	ASIC_HW_IF_STR *sencSetReg;

	/* �ϐ�������	*/
	MotorLessTest		= ((BASE_LOOPCTRLS*)BaseLoops)->MotorLessTest;
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);
	sencSetReg			= FencV->EncIf.SencSetReg;

	/*----------------------------------------------------------------------*/
	/*	�p�����[�^���Z�b�g(�Čv�Z)��										*/
	/*----------------------------------------------------------------------*/
	/* dFencPos = 0 �Ƃ��� 		*/
	//if( FnCmnCtrl->KnlMode == KPI_KMD_PARAMRESET ){ return( 0 );}
	// KPI_KMD_PARAMRESET�͐ݒ肳��Ȃ�
	// FnCmnCtrl *FnCmnCtrl : AxisRsc->FnCmnCtrl

	/*--------------------------------------------------------------------------*/
	/*	��M��Flag(IntSync Set --> ScanB Clear)�^��M�G���[�J�E���^�̃��Z�b�g	*/
	/*--------------------------------------------------------------------------*/
	FencV->MstErrCnt = 0;						/* Reset Master Error Counter	*/
	FencV->SeRxErr = FALSE;						/* Reset Error Flag				*/
	FencV->EncDataNG = FALSE;					/* Reset Encoder Data NG Flag	*/
	/*--------------------------------------------------------------------------*/
	/*	�O��̈ʒu�f�[�^�ޔ�													*/
	/*--------------------------------------------------------------------------*/
	FencV->RxPosH[1] = FencV->RxPosH[0];		/* �O��l�X�V					*/

	/*--------------------------------------------------------------------------*/
	/*	���[�^���f�������ʒu�����f�[�^�擾									*/
	/*--------------------------------------------------------------------------*/
	dEncPos = (LONG)MotorLessTestDatas->dFullModePos << FencV->MposSftR;
	MlibAdd6432( MotorLessTestDatas->FencData.Long, dEncPos, MotorLessTestDatas->FencData.Long );

	/*--------------------------------------------------------------------------*/
	/*	�ʒu�f�[�^���G���R�[�_��M�f�[�^�ɃZ�b�g								*/
	/*--------------------------------------------------------------------------*/
	FencV->RxPosL[0].dw	= MotorLessTestDatas->FencData.EncFormat.RxPosL.dw;
	FencV->RxPosH[0]	= MotorLessTestDatas->FencData.EncFormat.RxPosH;

	/*--------------------------------------------------------------------------*/
	/*	�ʒu���Z																*/
	/*--------------------------------------------------------------------------*/
	FencV->MotPosX[1] = FencV->MotPosX[0];		/* �O��l�X�V					*/
	FencV->MotPosX[0] = FencV->MposSign * ((FencV->RxPosL[0].Long >> FencV->MposSftR) << FencV->MposSftR);

	/*--------------------------------------------------------------------------*/
	/*	�ʒu�������Z															*/
	/*--------------------------------------------------------------------------*/
	dFencPos = (FencV->MotPosX[0] - FencV->MotPosX[1]) >> FencV->MposSftR;
	FencV->ABphCntr = (UCHAR)(FencV->MotPosX[0]) >> FencV->MposSftR;

	/*--------------------------------------------------------------------------*/
	/*	�O��PG���x���Z															*/
	/*--------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FencSpd = dFencPos * FencV->KConvLoadSpd;
#else
	FencSpd = MlibMulgain(dFencPos, FencV->KConvLoadSpd);
#endif /* FLOAT_USE */
#if 0
	FencV->MotSpd = LpxTimeBasesFencSpdCalc(dFencPos, FencSpd, FencV->SpdCmpEnable);
	FencV->SpdCmpEnable = TRUE;
#else
	FencV->MotSpd = FencSpd;
#endif

	/*--------------------------------------------------------------------------*/
	/*	�ʒu���Z [32bitRingPulse]												*/
	/*--------------------------------------------------------------------------*/
	FencV->MotPos += dFencPos;
	FencV->MotAbsPos += dFencPos;

	/*--------------------------------------------------------------------------*/
	/*	�����C���N���p���X�o�͏��� or �b���ʉ߃`�F�b�N����						*/
	/*--------------------------------------------------------------------------*/
	if( FencV->DivInfo.IncPulseReq != FALSE )	/* �����C���N���o�͗v���H	*/
	{
		/*----------------------------------------------------------------------*/
		/* �d���������̏��������Ƀ��[�^���ړ����Ă����ꍇ�̕����p���X�o�͑΍�   */
		/*	ScanA������1��ڂ́A�J�E���^�v���Z�b�g�ƃp���X�ϊ��ʒu���Z�b�g����B*/
		/*----------------------------------------------------------------------*/
		if(FencV->AbsoEncType == MENC_INCRE_TYPE)
		{
			/* �b���ʉ߃`�F�b�N���� */
			if(FencV->SeRxErr == FALSE)
			{
				FencCheckCphasePass(FencV, sencSetReg);
			}
			/*	�����p���X�o�͏���	(���O�ɃJ�E���^�v���Z�b�g��ݒ�)*/
			if(FencV->DivInfo.DivOut == TRUE)
			{
				FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
				sencSetReg->AREG_POTEST = (USHORT)FencV->DivInfo.DivOutPos;
				sencSetReg->AREG_POSET0 = (USHORT)FencV->DivInfo.DivOutPos;
			}
			FencV->DivInfo.IncPulseReq = FALSE;
		}
		else
		{
			/*----------------------------------------------------------------------*/
			/*	�����C���N���p���X�o�͏���	  									    */
			/*----------------------------------------------------------------------*/
			FencSencOutIniIncPulse( FencV, sencSetReg );
		}
	}
	else
	{
		/* �b���ʉ߃`�F�b�N����	*/
		if( FencV->SeRxErr == FALSE )
		{
			FencCheckCphasePass(FencV, sencSetReg);
		}
		/* �����p���X�o�� */
		if( (FencV->DivInfo.DivOut != FALSE) && (FencV->DivInfo.IncPulseReq == FALSE) )
		{
			FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
			sencSetReg->AREG_POSET0 = (USHORT) FencV->DivInfo.DivOutPos;
		}
	}

	return( dFencPos );
}

void	MlibAdd6432( LONG *a, LONG b, LONG *x )
{
	INT64	*src;
	INT64	*dst;

	src = (INT64*)a;
	dst = (INT64*)x;
	*dst = *src + b;

	return;
}
/* <<<<< <S1B7> */

/***************************************** end of file **********************************************/
