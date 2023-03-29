/*****************************************************************************************************
Description	: Serial Encoder Command Interface
------------------------------------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

------------------------------------------------------------------------------------------------------
Changes	      :
	Name   Date          Description
	--------------------------------------------------------------------------------------------------
	I.M.   2011.02.08    created
	H.H.   2013.10.08    Re-created

*****************************************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"
#include "MLib.h"
#include "KLib.h"


#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)/* <S04D> */
/*****************************************************************************************************
*																									  
*		�t���N�G���R�[�_ : �ʒu�Z���T�^�C�v�ݒ�														  
*																									  
******************************************************************************************************
*																									  
*	�@ �\ : �G���R�[�_�p�����[�^���A�A�u�\�^�C���N���^�R���o�[�^�Ȃǔ��f���A�����ϐ���ݒ肷��B	  
*																									  
*****************************************************************************************************/
BOOL	FencSetSensorType( MENCV *FencV, OPFBK_HNDL *FbkOpt )
{
BOOL rc;

	rc = FALSE;

	switch(FencV->TypeCode)
	{
	case 'E': /* �d���c�����^�C�v							*/
		FencV->SerialConvType = SCONVTYPE_ENDAT_ABS;
		FencV->AbsoEncoder = TRUE;
		FencV->OrgClrByWrAdrCmd = FALSE;
		FencV->AxisMotType = MOTTYPE_LINEAR;
		FbkOpt->var.f.PrmSumErrAsFulc = FALSE;				/* �`�F�b�N�T���ُ�t���O������			*/
		break;
	case 'A': /* �~�c�g���A�u�\�^�C�v						*/
		FencV->SerialConvType = SCONVTYPE_MITUTOYO_ABS;
		FencV->AbsoEncoder = TRUE;
		FencV->OrgClrByWrAdrCmd = FALSE;
		FencV->AxisMotType = MOTTYPE_LINEAR;
		FbkOpt->var.f.PrmSumErrAsFulc = FALSE;				/* �`�F�b�N�T���ُ�t���O������			*/
		break;
	case 'D': /* �r�l�r�C���N���^�C�v 						*/
		FencV->SerialConvType = SCONVTYPE_SONY_INC;
		FencV->AbsoEncoder = FALSE;
		FencV->OrgClrByWrAdrCmd = FALSE;
		FencV->AxisMotType = MOTTYPE_LINEAR;
		FbkOpt->var.f.PrmSumErrAsFulc = FALSE;				/* �`�F�b�N�T���ُ�t���O������			*/
		break;
	case 'L': /* �W���C���N���^�C�v 						*/
		FencV->SerialConvType = SCONVTYPE_STANDARD_INC;
		FencV->AbsoEncoder = FALSE;
		FencV->OrgClrByWrAdrCmd = TRUE;
		FencV->AxisMotType = MOTTYPE_LINEAR;
		FbkOpt->var.f.PrmSumErrAsFulc = FALSE;				/* �`�F�b�N�T���ُ�t���O������			*/
		break;
	default: /* �ُ�n				 						*/
		if(FbkOpt->conf.OpInfo.IfType == FBKOP_SCNV_MODULE )
		{
			rc = TRUE;										/* A.CF1 : �e�o�f�ʐM�ُ�				*/
			FencV->SerialConvType = SCONVTYPE_NONE;
			FencV->OrgClrByWrAdrCmd = FALSE;
			FencV->AbsoEncoder = FALSE;
			FencV->AxisMotType = MOTTYPE_LINEAR;
			FbkOpt->var.f.PrmSumErrAsFulc = TRUE;			/* ���o���ސݒ薢����Sum�ُ�Ƃ݂Ȃ�	*/
		}
		break;
	}
	return rc;
}
/*****************************************************************************************************
*
*	�t���N�G���R�[�_ : �^�C�v�R�[�h�ݒ�
*	�G���R�[�_�h�c�����A�C���N���^�d���������̔��f���s���B
*
*****************************************************************************************************/
static BOOL FencSetEncoderType(MENCV *FencV, OPFBK_HNDL *FbkOpt, DBYTEX BitnoDpoint)
{
BOOL rc;

	if( FbkOpt->conf.OpInfo.IfType != FBKOP_SCNV_MODULE )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	�V���A���R���o�[�^�^�C�v�̃t�B�[�h�o�b�N�I�v�V�����ł͂Ȃ��ꍇ							*/
		/*------------------------------------------------------------------------------------------*/
		if((FencV->EncInfo.Model[2] == 'D') && (FencV->EncInfo.Model[3] == 'P'))
		{
			rc = FencSetSensorType( FencV, FbkOpt );				/* �t���N�G���R�[�_�ʒu�Z���T�^�C�v�ݒ�	*/
		}
		else
		{
			rc = TRUE; /* A.CF1 : �e�o�f�ʐM�ُ� */
		}
	}
	else
	{
		/*------------------------------------------------------------------------------------------*/
		/*	�V���A���R���o�[�^�^�C�v�̃t�B�[�h�o�b�N�I�v�V�����̏ꍇ								*/
		/*------------------------------------------------------------------------------------------*/
		rc = FencSetSensorType( FencV, FbkOpt );					/* �t���N�G���R�[�_�ʒu�Z���T�^�C�v�ݒ�	*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	��Βl�G���R�[�_�g�p���@�̐ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	if(FencV->AbsoEncoder != FALSE)
	{
		FencV->AbsoEncType = MENC_MABSO_TYPE;
		FencV->EncType = (0x0100 | BitnoDpoint.b.l);
	}
	else
	{
		FencV->AbsoEncType = MENC_INCRE_TYPE;
		FencV->EncType = (0x0000 | BitnoDpoint.b.l);
	}

	return rc;
}


/*****************************************************************************************************
*
*	�t���N�G���R�[�_ : �p�����[�^�̓Ǎ���
*
*****************************************************************************************************/
static SENC_ERROR FencReadParamAll(MENCV *FencV, OPFBK_HNDL *FbkOpt, MENCPRM *MencP)
{
	UCHAR	PrmBuf[8];

	/*----------------------------------------------------------------------------------------------*/
	/*	�G���R�[�_�h�c�ǂݍ��ݏ���																	*/
	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x00, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	SencSetIDINFO(0x00, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x08, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	SencSetIDINFO(0x08, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x10, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	SencSetIDINFO(0x10, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x18, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	SencSetIDINFO(0x18, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------------------------------*/
	/*	���j�A�^�C�v�R�[�h																			*/
	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x40, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	FencV->TypeCode = (LONG)(PrmBuf[0]);

	/*----------------------------------------------------------------------------------------------*/
	/*	�r�b�g��,�����_�ʒu																			*/
	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x48, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	MencP->BitnoDpoint.b.l = PrmBuf[2];
	MencP->BitnoDpoint.b.h = PrmBuf[3];

	/*----------------------------------------------------------------------------------------------*/
	/*	�G���R�[�_�\�t�g�E�F�A�o�[�W����															*/
	/*----------------------------------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x58, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}
	if( FbkOpt->conf.OpInfo.IfType == FBKOP_SCNV_MODULE )
	{
	/* CPU�LFBOpt�ڑ����́A��߼�ݶ���Ver���ٸVer�����Ұ��l��ݒ� */
		FbkOpt->conf.OpInfo.SoftVer = PrmBuf[7];
		FencV->SoftVer = PrmBuf[7];
	}
	else
	{
	/* CPU��FBOpt�ڑ������ٸVer�����Ұ��l��ݒ� */
		FencV->SoftVer = PrmBuf[7];
	}
	
	/*	�h�c���ւ̏I�[�R�[�h�}��	*/
	SencFillNullIDINFO(&FencV->EncInfo);

	/*----------------------------------------------------------------------------------------------*/
	/*	�G���R�[�_�^�C�v�̐ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
	if(FALSE != FencSetEncoderType(FencV, FbkOpt, MencP->BitnoDpoint))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : �e�o�f�ʐM�ُ�	*/
	}

	return (SENCERR_SUCCESS);
}

/*****************************************************************************************************
*
*	�t���N�G���R�[�_��{�p�����[�^�v�Z
*	�T�[�{��{�p�����[�^�v�Z(BprmCalc.c)�ɕK�v�ȃG���R�[�_�p�����[�^�̌v�Z���s��
*
*****************************************************************************************************/
void FencBasePrmCalc(MENCV *FencV, MENCPRM *MencP, USHORT b_prm2, ULONG fcplsml,BOOL rvsDir)
{
	/*----------------------------------------------------------------------------------------------*/
	/*	�^�]�����̐ݒ�																				*/
	/*----------------------------------------------------------------------------------------------*/
	switch((b_prm2 >> 12) & 0x0F)
	{
	case 0: /* �g�p���Ȃ�													*/
		FencV->RevAsmMotor = FALSE;
		break;
	case 1: /* �W���^�]�����Ŏg�p����										*/
	case 2: /* �`,�a,�b���f��Ͻ����� 										*/
		FencV->RevAsmMotor = FALSE;
		break;
	case 3: /* �t�]�^�]�����Ŏg�p����										*/
	case 4: /* �`,�a,�b���f��Ͻ�����										*/
		FencV->RevAsmMotor = TRUE;
		break;
	default:/* �p�����[�^�ݒ�ُ�											*/
		FencV->RevAsmMotor = FALSE;
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	���[�^�P��]���Z�p���X���̐ݒ� : [pulse/MotRev]												*/
	/*----------------------------------------------------------------------------------------------*/
	FencV->PulseNo = fcplsml << MencP->BitnoDpoint.b.l;	/* MotPuseNo����u������*/
	FencV->HalfPulseNo = FencV->PulseNo >> 1;

	/*----------------------------------------------------------------------*/
	/*	�t��]���[�h�I��Flag�̐ݒ�							<S02D>			*/
	/*----------------------------------------------------------------------*/
	FencV->DivInfo.RvsDir = rvsDir;
}


/*****************************************************************************************************
*
*	�t���N�G���R�[�_�̏������E�ڑ�����
*
*****************************************************************************************************/
SENC_ERROR FencInitEncoder( MENCV *FencV, MENC_INIT_PRM *MencInitPrm, ASIC_HW_IF_STR *AsicHwReg,
										OPFBK_HNDL *FbkOpt, USHORT EncBaudrate, BOOL SencFbOptUse, BOOL rvsDir )
{
	SENC_ERROR err;

	err = SencConnect(FencV, AsicHwReg, EncBaudrate, FbkOpt, SencFbOptUse );
	if(SENCERR_SUCCESS == err)
	{
		/*	FencReadParamAll*/
		err = FencReadParamAll(FencV, FbkOpt, MencInitPrm->MencP);
		if(SENCERR_SUCCESS != err)
		{/* try again... */
			err = FencReadParamAll(FencV, FbkOpt, MencInitPrm->MencP);
		}
		if(SENCERR_SUCCESS == err)
		{
			/*----------------------------------------------------------------------------------*/
			/*		���_���Z�b�g�R�}���h�̑��M													*/
			/*----------------------------------------------------------------------------------*/
//			err = SencSendRstOrgCmd(FencV);
			/*	Calculate the base parameters for Fully closed encoder */
			FencBasePrmCalc(FencV, MencInitPrm->MencP, MencInitPrm->b_prm2, MencInitPrm->fcplsml, rvsDir);/*<S02D>*/
		}
	}

	return err;
}



/*****************************************************************************************************
*
*	�t���N�G���R�[�_ : �����p�����[�^�v�Z
*	�T�[�{��{�p�����[�^�v�Z(BprmCalc.c)��A�p�����[�^�v�Z���s��
*
*****************************************************************************************************/
static void FencInitPrmCalc(MENCV *FencV, MENCPRM *MencP,
                            USHORT scnvorgoffset, BOOL OrgOffsetEnable, LONG KConvLoadSpd)
{
	UCHAR 	BitNo;
	USHORT 	uswk;

	BitNo = MencP->BitnoDpoint.b.l;
	/*----------------------------------------------------------------------*/
	/*	�ʒu���Z�p�p�����[�^�̌v�Z											*/
	/*----------------------------------------------------------------------*/
	if( FencV->PosCompMode != POSCOMP_SW )/*<S0C5>rename*/
	{/* �ʒu�␳Ӽޭ�ق�p����ꍇ�́A�ʒu�␳Ӽޭ�ٓ��Ŕ��]����	*/
		FencV->MposSign = 1;
	}
	else
	{
		if(FencV->RevAsmMotor != FALSE)
		{
			FencV->MposSign = -1;
		}
		else
		{
			FencV->MposSign = 1;
		}
	}
	FencV->KConvLoadSpd = KConvLoadSpd;

	/*----------------------------------------------------------------------*/
	/*	�b�����X�t���O�ݒ�													*/
	/*----------------------------------------------------------------------*/
	if(FencV->AbsoEncoder)
	{
		FencV->CphaseLess = TRUE;
	}
	else
	{
		FencV->CphaseLess = FALSE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	�ʒu�f�[�^�v�Z�p�V�t�g�f�[�^																*/
	/*----------------------------------------------------------------------------------------------*/
	/*	���[�^���X�e�X�g���[�h�̏ꍇ�ABitNo=8, Dpoint=68�ƌŒ肳��A
	 * 	�V�t�g��12(=FSX_FPOS_SFT)�ƂȂ邽�߁A�����Ɋ܂߂Ȃ�
	 *----------------------------------------------------------------------*/
//	if(FbkOptP.OpInfo.IfType == FBKOP_SCNV_MODULE)
//	{
//		FencV->MposSftR = 32 - BitNo;
//		FencV->MposSftX = FencV->MposSftR - (80 - MencP->BitnoDpoint.b.h);
//	}
//	else
	{
		FencV->MposSftR = FSX_FPOS_SFT;
//		FencV->MposSftR = 32 - BitNo;
//		FencV->MposSftX = FSX_FPOS_SFT;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	PnE8C   : �V���A���ϊ����j�b�g���_�I�t�Z�b�g												*/
	/*																								*/
	/*	OrgDetDelayComp = PnE8C [0.01deg]�~2^BITNO / 100 / 360[deg]									*/
	/*----------------------------------------------------------------------------------------------*/
	/*		��C���ʒu�v�Z�Ŏg�p���邽�߁ALMX_MPOS_SFT�����V�t�g���Ă���								*/
	/*----------------------------------------------------------------------------------------------*/
	if(FALSE != OrgOffsetEnable)
	{
//		if((FencV->SenSpec & 0xC4) == 0x84)
//		{	/* Bit7=1,Bit6=0 ���� Bit2(���_�I�t�Z�b�g�L��)	*/
//			uswk = FencV->SenOrgOffset * 100; /* deg -> 0.01deg ���Z */
//		}
//		else
		{
			uswk = scnvorgoffset;	/* PnE8C�͌��X0.01deg */
		}
		FencV->OrgDetDelayComp = (((uswk << BitNo) / 100) << FencV->MposSftR) / 360;
	}
	else
	{
		FencV->OrgDetDelayComp = 0;
	}

}


/*****************************************************************************************************
 *
 *	�t���N�G���R�[�_ : �ϐ�����������
 *
 ****************************************************************************************************/
SENC_ERROR FencInitVariables(MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg,
                             MENCPRM *MencP, USHORT scnvorgoffset, BOOL OrgOffsetEnable,
                             LONG KConvLoadSpd, LONG sfgfail_b_h)
{
	LONG		WorkPos, mposSign;
	SENC_ERROR	err;

	/*----------------------------------------------------------------------------------------------*/
	/*	�V���A���G���R�[�_����̃f�[�^�ǂݍ���														*/
	/*----------------------------------------------------------------------------------------------*/
	err = SencReadInitialP1Data(FencV);

	/*----------------------------------------------------------------------------------------------*/
	/*	�t���N�G���R�[�_�����p�����[�^�v�Z															*/
	/*----------------------------------------------------------------------------------------------*/
	FencV->ComErrLimit = sfgfail_b_h;/*Prm.spgfail.b.h*/
	FencInitPrmCalc(FencV, MencP, scnvorgoffset, OrgOffsetEnable, KConvLoadSpd);

	/*----------------------------------------------------------------------------------------------*/
	/*	����ʒu�f�[�^(����,�O��)�̐ݒ�																*/
	/*----------------------------------------------------------------------------------------------*/
	/* �ʒu�␳Ӽޭ�ق�p����ꍇ�́A�ʒu�␳Ӽޭ�ٓ��Ŕ��]������MposSign�͏�ɐ��Ƃ��Ă���B	*/
	if(FencV->RevAsmMotor != FALSE)
	{
		mposSign = -1;
	}
	else
	{
		mposSign = 1;
	}
	FencV->MotPos = 0; /* �t���N�ʒu [32bitRingPulse] */
	FencV->RxPosH[1] = FencV->RxPosH[0];
	FencV->MotPosX[0] = mposSign *
					   ((FencV->RxPosL[0].Long >> FencV->MposSftR) << FencV->MposSftR);
	FencV->MotPosX[1] = FencV->MotPosX[0];

	/*----------------------------------------------------------------------------------------------*/
	/* �����Έʒu�f�[�^(��Έʒu36Bit�f�[�^�̉���32Bit�f�[�^�����o��)							*/
	/*----------------------------------------------------------------------------------------------*/
	if(FencV->AbsoEncoder != FALSE)
	{
		FencV->MotAbsPos = (FencV->RxPosH[0] << (32 - FSX_FPOS_SFT))
								| ((ULONG)FencV->RxPosL[0].dw >> FSX_FPOS_SFT);
		FencV->MotAbsPos = mposSign * FencV->MotAbsPos;
	}
	else
	{
		FencV->MotAbsPos = 0;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*		�����p���X�����ݒ�																		*/
	/*----------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	FencV->DivOutRem = 0;
	WorkPos = (ULONG)FencV->MotPosX[0] >>  FencV->MposSftR;
	FencV->DivOutPos = MlibPfbkxremNolim(WorkPos, FencV->DivOutGain, &FencV->DivOutRem);
	FencV->InitDivPosOffset = FencV->DivOutPos;

	if(FencV->AbsoEncoder == FALSE)
	{	/*------------------------------------------------------------------------------------------*/
		/* �b���t���O�֘A�F�C���N���G���R�[�_														*/
		/*------------------------------------------------------------------------------------------*/
		FencV->Cset = FALSE; 								/* �b���ݒ�Flag							*/
		FencV->CposOk = FALSE; 								/* �b���ʒu�ݒ芮��Flag					*/
		if(FencV->DivOut != FALSE)
		{
			/* �p���X�ϊ�C���}�X�NON�ݒ�	*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			FencV->DivCMaskStatus = DIV_CMASK_ON;
			/* �ʏ�b���o�͑I��				*/
			AsicHwReg->AREG_DIVSET &= 0xf3ff;
			FencV->DivModeStatus = DIV_MODE_NORMAL;

			AsicHwReg->AREG_POSET0 = (USHORT)FencV->DivOutPos;
			AsicHwReg->AREG_POTEST = (USHORT)FencV->DivOutPos;
		}
	}
	else
	{	/*------------------------------------------------------------------------------------------*/
		/* �b���t���O�֘A�F��Βl�G���R�[�_															*/
		/*------------------------------------------------------------------------------------------*/
		FencV->Cset = TRUE; 		/* �b���ݒ�Flag	*/
		FencV->CposOk = TRUE; 		/* �b���ʒu�ݒ芮��Flag */
		if(FencV->DivOut != FALSE)
		{
			/* �p���X�ϊ�C���}�X�NON�ݒ�	*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			FencV->DivCMaskStatus = DIV_CMASK_ON;
			/* �ʏ�b���o�͑I��				*/
			AsicHwReg->AREG_DIVSET &= 0xf3ff;
			FencV->DivModeStatus = DIV_MODE_NORMAL;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/* �b���t���O�֘A�F�b�����X�G���R�[�_															*/
	/*----------------------------------------------------------------------------------------------*/
	FencV->MotCphPass = FALSE; 								/* �b���ʉ�Flag							*/
//	FencV->MotCphPassB = FALSE; 							/* �b���ʉ�Flag	for ScanB				*/

	/*----------------------------------------------------------------------------------------------*/
	/* �G���R�[�_���f�B�ݒ� : �G���R�[�_�������E�p�����[�^�������̌�Őݒ�						 	*/
	/*----------------------------------------------------------------------------------------------*/
	if(FencV->EncConnect == FALSE) 							/* �G���R�[�_���ڑ���					*/
	{
		FencV->SenReady = FALSE; 							/* SEN�M����������Flag					*/
		FencV->IncPulseReq = FALSE; 						/* �����C���N���p���X�o�͗v��			*/
	}
	else if(FencV->AbsoEncType != FALSE) 					/* �A�u�\�G���R�[�_��					*/
	{
		FencV->SenReady = FALSE; 							/* SEN�M����������Flag					*/
		FencV->IncPulseReq = TRUE; 							/* �����C���N���p���X�o�͗v��			*/
	}
	else 													/* �C���N���G���R�[�_��					*/
	{
		FencV->SenReady = TRUE; 							/* SEN�M����������Flag					*/
		FencV->IncPulseReq = FALSE; 						/* �����C���N���p���X�o�͗v��			*/
	}

	/*----------------------------------------------------------------------------------------------*/
	/* �@�\�L���I���F�C���N���p���X�o�͗v�����Z�b�g													*/
	/*----------------------------------------------------------------------------------------------*/
	FencV->PaoseqNormal = FALSE; 							/* PAO�o��Seq ���񏈗��ݒ�				*/
//	FencV->IncPulseReq = FALSE; 							/* �����C���N���p���X�o�͗v��			*/
#else/*<S02D>*/
	FencV->DivInfo.DivOutRem = 0;
	WorkPos = (ULONG)FencV->MotPosX[0] >>  FencV->MposSftR;
	FencV->DivInfo.DivOutPos = MlibPfbkxremNolim(WorkPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
	FencV->DivInfo.InitDivPosOffset = FencV->DivInfo.DivOutPos;

	if(FencV->AbsoEncoder == FALSE)
	{	/*------------------------------------------------------------------*/
		/* �b���t���O�֘A�F�C���N���G���R�[�_								*/
		/*------------------------------------------------------------------*/
		FencV->Cset = FALSE; 		/* �b���ݒ�Flag							*/
		FencV->CposOk = FALSE; 		/* �b���ʒu�ݒ芮��Flag					*/
		if(FencV->DivInfo.DivOut != FALSE)
		{
			/* �p���X�ϊ�C���}�X�NON�ݒ�	*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
			/* �ʏ�b���o�͑I��				*/
			AsicHwReg->AREG_DIVSET &= 0xf3ff;
			FencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
			AsicHwReg->AREG_POSET0 = (ULONG)FencV->DivInfo.DivOutPos;
			AsicHwReg->AREG_POTEST = (ULONG)FencV->DivInfo.DivOutPos;
		}
	}
	else
	{	/*------------------------------------------------------------------*/
		/* �b���t���O�֘A�F��Βl�G���R�[�_									*/
		/*------------------------------------------------------------------*/
		FencV->Cset = TRUE; 		/* �b���ݒ�Flag	*/
		FencV->CposOk = TRUE; 		/* �b���ʒu�ݒ芮��Flag */
		if(FencV->DivInfo.DivOut != FALSE)
		{
			/* �p���X�ϊ�C���}�X�NON�ݒ�	*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
			/* �ʏ�b���o�͑I��				*/
			AsicHwReg->AREG_DIVSET &= 0xf3ff;
			FencV->DivInfo.DivModeStatus = DIV_MODE_NORMAL;
		}
	}
	/*----------------------------------------------------------------------*/
	/* �b���t���O�֘A�F�b�����X�G���R�[�_									*/
	/*----------------------------------------------------------------------*/
	FencV->MotCphPass = FALSE; 		/* �b���ʉ�Flag							*/

	/*----------------------------------------------------------------------*/
	/* �G���R�[�_���f�B�ݒ� : �G���R�[�_�������E�p�����[�^�������̌�Őݒ� 	*/
	/*----------------------------------------------------------------------*/
	if(FencV->EncConnect == FALSE) 		/* �G���R�[�_���ڑ���				*/
	{
		FencV->SenReady = FALSE; 		/* SEN�M����������Flag				*/
		FencV->DivInfo.IncPulseReq = FALSE; 	/* �����C���N���p���X�o�͗v��		*/
	}
	else if(FencV->AbsoEncType != FALSE) /* �A�u�\�G���R�[�_��				*/
	{
		FencV->SenReady = FALSE; 		/* SEN�M����������Flag				*/
		FencV->DivInfo.IncPulseReq = TRUE; 		/* �����C���N���p���X�o�͗v��		*/
	}
	else 								/* �C���N���G���R�[�_��				*/
	{
		FencV->SenReady = TRUE; 		/* SEN�M����������Flag				*/
		FencV->DivInfo.IncPulseReq = FALSE; 	/* �����C���N���p���X�o�͗v��		*/
	}

	/*----------------------------------------------------------------------*/
	/* �@�\�L���I���F�C���N���p���X�o�͗v�����Z�b�g							*/
	/*----------------------------------------------------------------------*/
	FencV->PaoseqNormal = FALSE; 		/* PAO�o��Seq ���񏈗��ݒ�			*/

	/*----------------------------------------------------------------------*/
	/* �����C���N���o�͗v���Z�b�g									<S02D>	*/
	/* �C���N�����ł�TRUE�ɃZ�b�g���AScanA��1��ڂ̏�����FALSE�ɃZ�b�g����	*/
	/*----------------------------------------------------------------------*/
#if 0
	if( SVFSEL_INCPOUT == 0 )
	{
		FencV->DivInfo.IncPulseReq = FALSE;
	}
	else
	{
		if( FencP->DivOut == TRUE) /* && (Kprm.f.MotorLessTestMode==FALSE) )*/
		{
			FencV->DivInfo.IncPulseReq = TRUE;
		}
	}
#endif/*<S02D>*/
#endif
	/*----------------------------------------------------------------------------------------------*/
	/* ��]���x�f�[�^�̏����l�ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	FencV->MotSpd = 0; 										/* Motor Speed [2^24/OvrSpd]			*/
	FencV->SpdCmpEnable = FALSE;							/* ���x�␳�����L��Flag					*/

	return err;
}





#if 0
/*****************************************************************************************************
 *
 *	�t���N�G���R�[�_ : �x�[�X�^�C�}�[�Ď�
 *
 ****************************************************************************************************/
void LpxCheckFencBaseTimer(MENCV *FencV)
{
	USHORT BaseTimer;

	BaseTimer = (USHORT) (FencV->RxTimex.dw >> 8);
	if(FencV->ChkAlarmCnt >= 2)		/* �R��ڈȍ~�`�F�b�N */
	{
		if(BaseTimer == FencV->ChkBaseTimer)				/* �^�C���x�[�X�^�C�}�ω�����? 			*/
		{
			KPI_ALARMSET(ALM_FPGCOMTIMER); 					/* A.CF2 : �e�o�f�ʐM�^�C�}�ُ�			*/
		}
	}
	FencV->ChkBaseTimer = BaseTimer; /* �^�C���x�[�X�^�C�}�O��l�X�V		*/

}
#endif

/*****************************************************************************************************
 *
 *	�t���N�G���R�[�_ : �A���[���Ď�
 *
 ****************************************************************************************************/
UCHAR FencGetFencAlarm(MENCV *FencV)
{
	UCHAR AlmCode;

	if( (FencV->MstErrCnt == 0) &&
		((FencV->EncConnect != FALSE) && (FencV->EncDisable == FALSE)) )
	{
//		LpxCheckFencBaseTimer(FencV); 						/* �x�[�X�^�C�}�[�`�F�b�N				*/
//		LpxCheckFencAlarmCode(FencV); 						/* �A���[���R�[�h�`�F�b�N				*/

		if(FencV->ChkAlarmCnt >= 2) /* �R��ڈȍ~�`�F�b�N */
		{
			AlmCode = (UCHAR)(FencV->RxAlmCode & 0x003B);
			MlibBITFILTER1( FencV->ChkAlmCode[0], FencV->ChkAlmCode[1], AlmCode );
		}
		else
		{
			FencV->ChkAlmCode[0] = 0;
			FencV->ChkAlmCode[1] = 0;
		}

		if(FencV->ChkAlarmCnt < 10)
		{
			FencV->ChkAlarmCnt++;
		}
	}
	else
	{
		FencV->ChkAlarmCnt = 0;
	}

	return ((UCHAR)FencV->ChkAlmCode[0]);
}

#endif/* CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT*/
/*** end of file ***/
