/****************************************************************************
  Description   : �d�����oAD����(Fn00E, Fn00F)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-11-18

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"

#include "PnPrmListTbl.h"
#include "KLib.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define KPI_RC_NOTADJ	0		/* �������{�����s���� */
#define KPI_RC_ADJOK	1		/* �������� */
#define KPI_RC_ADJERR	-1		/* �������s */

#define OFFSET_ADJ		0		/* �d�����o�I�t�Z�b�g���� */
//#define GAIN_ADJ		1		/* �d�����o�Q�C������ */

/* <S057>	Start */
/* -------- Fn00F Current Offset Manual Adjustment -------- */
#define	ZADJI_MAX		511		/* �d�����o�M���̃I�t�Z�b�g�蓮�����l�@�ő�l */
#define	ZADJI_MIN		-512	/* �d�����o�M���̃I�t�Z�b�g�蓮�����l�@�ŏ��l */

//#define	ACCLVL_SYSTEM	3		/* ���[�U���x�� = �V�X�e�����x�� */	/* <S101> */
#define	ACCLVL_SYSTEM	4			/* ���[�U���x�� = �V�X�e�����x�� */	/* <S101> */

//#define	OK	1					/* �d�����o�M���̃I�t�Z�b�g�蓮�����@���s�\	*///<S0C7>
//#define	NG	0					/* �d�����o�M���̃I�t�Z�b�g�蓮�����@���s�s�\	*///<S0C7>
/* <S057>	End */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static LONG	KpiAdjustCurAdZero( AXIS_HANDLE *Axis );
static LONG	TciGetCurADOffset( ASICS *SvAsicRegs, PUMP_CTR_STR *pump_if_ptr, LONG AdZero[2], LONG Mode );
static LONG	TciReadCurADMean( ASICS *SvAsicRegs, PUMP_CTR_STR *pump_if_ptr, LONG AdMean[2], LONG Mode );

/****************************************************************************
 Fn00E: Entry function of Motor Current Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterMotorCurrentAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MOTLESSTST			*MotorLessTest;	/* ���[�^���X�e�X�g���[�hIF��`		*//* <S1B7> */

	FnCmnCtrl = Axis->FnCmnCtrl;
	MotorLessTest = Axis->MotorLessTest;									/* <S1B7> */

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* ���[�^���X�e�X�g���[�h���� *//* <S1B7> */
	if( (MotorLessTest->MotorLessTestMode == TRUE)
		&& (MotorLessTest->MotorLessTestDatas.SemiEncConnect == FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

//	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;
//	FnCmnCtrl->KnlMode = KPI_KMD_ADJCURAD;	/* �d�����o�`�c������ *//* <S085> */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn00E: Execute function of Motor Current Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteMotorCurrentAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	ASIC_HW_IF_STR	*AsicHwReg;

	LONG			AdjResult;
	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	AsicHwReg = Axis->SvAsicRegs->AsicHwReg;

	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if( (SeqCtrlOut->MainPowerOn == FALSE)
		|| (SeqCtrlOut->HwbbSts != FALSE)
		|| (SeqCtrlOut->MotStop == FALSE)
//		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) != 0) )	/* Base Block */
		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0) )	/* Base Enable */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* �������s */
	AdjResult = KpiAdjustCurAdZero(Axis);

	/* �����I�� */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;

	if(AdjResult == KPI_RC_NOTADJ){
		return PRM_RSLT_CONDITION_ERR;
	}
	else if(AdjResult == KPI_RC_ADJOK){
		/* �������Ȃ� */
	}
	else if(AdjResult == KPI_RC_ADJERR){
		return PRM_RSLT_CALC_ERR;
	}
	return PRM_RSLT_SUCCESS;
}

/* <S057>	Start */
/****************************************************************************
 Fn00E: End function of Motor Current Offset Auto Adjustment
****************************************************************************/
void FnLeaveMotorCurrentAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	REGIF_CONFIG_T	*RegCfg;			/* <S057> */
	RegCfg = Axis->RegManager->hPnReg;	/* <S057> */

	/* �I������		*/
	/* EEPROM�X�V����	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiu, 0 );		/* PnE50:�d�����o�[����(U��) */
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiv, 0 );		/* PnE51:�d�����o�[����(V��) */

	/* <S1B6>  �}�C�N���������� */
	pndef_zadjiu.PrmCalCallback( 0, 0, Axis, 0 );
	pndef_zadjiv.PrmCalCallback( 0, 0, Axis, 0 );
}

/****************************************************************************
 Fn00F: Entry function of Motor Current Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterMotorCurrentManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* ���[�^���X�e�X�g���[�h�L�� �� �G���R�[�_���ڑ��̏ꍇ�͓��炸�ɏI�� */
	/* TODO:���[�^���X�e�X�g���[�h���󖢑Ή� */
#if 0
	if( (Kprm.f.MotorLessTestMode && (MotorLessTestDatas.f.SemiEncConnect == FALSE)) )		/*<V245>*/
	{		
		return PRM_RSLT_CONDITION_ERR;
	}
#endif

//	FnCmnCtrl->KnlMode = KPI_KMD_ADJCURAD;	/* �d�����o�`�c������ */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;	/* normal mode */	/* <S142> */

	FunExe->HoldFnMode = TRUE;				/* Fn���[�h�����I���Ȃ� */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn00F: Execute function of Motor Current Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteMotorCurrentManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRMDATA			*Prm;
	PRM_RSLT		rc;
	LONG			zadjiu_tmp;
	LONG			zadjiv_tmp;
	CHAR			Exe_flg;
	FUN_AXCOMMON	*AxCommon;

	Prm = Axis->UniPrms->Prm;
	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

	/* ���[�U���x���m�F */
#if 0
	if(AxCommon->AccessLevel < ACCLVL_SYSTEM )
	{
		Exe_flg = NG;
	}
	else
	{
		Exe_flg = OK;
	}
#else
		Exe_flg = OK;
#endif
	switch( FunExe->State )
	{
	case 0:	/* �d�����o�[����(U��) ����	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE50:�d�����o�[����(U��)�̒l�ǂݍ��� */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp < ZADJI_MAX)
				{
					zadjiu_tmp+=1;
				}
				/* PnE50:�d�����o�[����(U��)�ɍX�V�����l���������� */
				Prm->zadjiu = (SHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;
		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE50:�d�����o�[����(U��)�̒l�ǂݍ��� */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp > ZADJI_MIN)
				{
					zadjiu_tmp-=1;
				}
				/* PnE50:�d�����o�[����(U��)�ɍX�V�����l���������� */
				Prm->zadjiu = (USHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;
			break;

		case FCMD_CH2:
			FunExe->State = 1;
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
			break;
		}
		break;

	case 1:	/* �d�����o�[����(V��) ����	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE51:�d�����o�[����(V��)�̒l�ǂݍ��� */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);


				if(zadjiv_tmp < ZADJI_MAX)
				{
					zadjiv_tmp += 1;
				}
				/* PnE51:�d�����o�[����(V��)�ɍX�V�����l���������� */
				Prm->zadjiv = (SHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;

		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE51:�d�����o�[����(V��)�̒l�ǂݍ��� */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

				if(zadjiv_tmp > ZADJI_MIN)
				{
					zadjiv_tmp -=1;
				}
				/* PnE51:�d�����o�[����(U��)�ɍX�V�����l���������� */
				Prm->zadjiv = (USHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
			break;
		}
		break;

	default:
		break;
	}

	return rc ;

}

/****************************************************************************
 Fn00F: End function of Motor Current Offset Manual Adjustment
****************************************************************************/
void FnLeaveMotorCurrentManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* �I������		*/
	/* EEPROM�X�V����	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiu, 0 );		/* PnE50:�d�����o�[����(U��) */
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiv, 0 );		/* PnE51:�d�����o�[����(V��) */

	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}

/* <S057>	End */

/****************************************************************************
 1103H : Entry function of the Current Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterMotorCurrentGainAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fn�T�[�{�^�]���l�� */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 1103H : Execute function of the Current Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteMotorCurrentGainAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	MENCV			*MencV;
	ALMMAN_HANDLE	*AlmManager;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	LONG			JogSpdP1;

	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	MencV = Axis->MencV;
	AlmManager = Axis->AlmManager;
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	errRes = PRM_RSLT_CONTINUE;

	if(FunExe->State == 0)
	{/* �T�[�{OFF��	*/
		JogSpdP1 = (MencV->AxisMotType == MOTTYPE_LINEAR) ? Prm->jogspd_l : Prm->jogspd;
		if(MencV->LowSpdMotor == FALSE)
		{
			JogSpdP1 = (10 * JogSpdP1);
		}
		JogSpdP1 = BprmSpeedLevelCal(Bprm, JogSpdP1, 0);

		switch(FunExe->CmdCode)
		{
		case FCMD_DOWN:	/* �T�[�{ON�Ɠ����ɋt�]	*/
			JogSpdP1 = -JogSpdP1;
		case FCMD_UP:	/* ���]	*/
			FnCmnCtrl->JogSpeed = JogSpdP1;
			if( (SeqCtrlOut->MainPowerOn == FALSE)
				|| (SeqCtrlOut->HwbbSts != FALSE)
				|| (AlmManager->AlmReady == FALSE) )
			{
				errRes = PRM_RSLT_CONDITION_ERR;
			}
			else
			{
				FnCmnCtrl->FnSvonReq = TRUE;
			}
			break;

		default:
			break;
		}

		if(SeqCtrlOut->BaseEnableReq != FALSE)
		{
			FunExe->State = 1;
		}
	}
	else if(FunExe->State == 1)
	{/* �T�[�{ON��	*/
		/* �������s */


		/* �����I��->�T�[�{�I�t */
		FnCmnCtrl->FnSvonReq = TRUE;
		FunExe->State = 2;
	}
	else
	{/* �I������ */
		if(SeqCtrlOut->BaseEnableReq == FALSE)
		{/* �T�[�{�I�t�m�F��I�� */
			errRes = PRM_RSLT_SUCCESS;
		}
	}

	return errRes;
}



/****************************************************************************************************/
/*																									*/
/*		�d�����o�`�c�뒲����(�I�����C���p)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �t���^�u���̓d�����o�`�c�뒲�p�f�[�^��ݒ肷��B										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG KpiAdjustCurAdZero( AXIS_HANDLE *Axis )
{
	LONG				i;
	LONG				AdZero[2];
	LONG				AdjustError;
	BOOL				HwbbSts;/* <HWBB> */
	ASIC_HW_IF_STR		*pAsicHwReg;
	REG_ACC_T			AccessPrm;
	FUN_CMN_CONTROL		*FnCmnCtrl;		/* <S085> */

	/* Initialize */
	AdjustError = FALSE;
	HwbbSts = Axis->SeqCtrlOut->HwbbSts;/* <HWBB> */
	pAsicHwReg = Axis->SvAsicRegs->AsicHwReg;
	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr	= 0;
	AccessPrm.hRegCfg	= Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx	= 0;
	AccessPrm.EepWrite	= TRUE;		/* EEPROM Write */
	AccessPrm.IfHandle	= NULL;
	FnCmnCtrl = Axis->FnCmnCtrl;		/* <S085> */

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n���� (Set Adjust Mode)																	*/
/*--------------------------------------------------------------------------------------------------*/
	FnCmnCtrl->KnlMode = KPI_KMD_ADJCURAD;	/* �d�����o�`�c������ *//* <S085> */
	/* �Ăяo�����Ń`�F�b�N�����̂ŁA�����ł͉������Ȃ� */
//	if( (MainPowerOn == FALSE) || (MotStopSts == FALSE) || (HwbbSts == TRUE) )/* <HWBB> */
//	{
//		return( KPI_RC_NOTADJ );						/* Error Return								*/
//	}
	/* �J�n�`�F�b�N�������Ȃ��̂ŁA�E�F�C�g���폜 */
//	KlibWaitms( 16 );

/*--------------------------------------------------------------------------------------------------*/
/*		�x�[�X�u���b�N�`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* ��knlMode == KPI_KMD_ADJCURAD�Ȃ�A�c�a��H���� LpxDBControlProcess()�͉����������Ȃ�		*/
	/* ���{��������DB OFF�Ƃ���																		*/
//	pAsicHwReg->AREG_ALMSET |= BIT5;					/* DB OFF									*/
	/* ���[�^���X�e�X�g���[�h�łȂ��Ƃ�OFF���� *//* <S1B7> */
	/* TODO: SGD-V�ł͈ȉ��̏�����OR���Ă���						*/
	/* (1) AxisRscC->BeSequence->StopMode == STOPMODE_DBSTOPFR		*/
	/* (2) AxisRscC->BeSequence->StopMode == STOPMODE_FREERUN		*/
	/* (3) AxisRscC->FnCmnCtrl->KnlMode == KPI_KMD_ADJCURAD			*/
	if( Axis->MotorLessTest->MotorLessTestMode == FALSE )
	{
		SHalDrv_DynamicBrakeOFF(pAsicHwReg);			/* DB OFF									*/
	}
	for( i=0; ; i++ )									/* �I��������BE or �^�C���I�[�o�[			*/
	{
		if( pAsicHwReg->AREG_FLTSTAT & 0x0400 )			/* FLTSTAT_10  1:�x�[�X�u���b�N				*/
		{
			break;
		}
		else if( i >= 1000 )							/* Check Time Over							*/
		{
			return( KPI_RC_NOTADJ );					/* Error Return								*/
		}
		KlibWaitms( 1 );								/* Wait 1ms									*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�뒲																			*/
/*--------------------------------------------------------------------------------------------------*/
	AdjustError = TciGetCurADOffset( Axis->SvAsicRegs, &(Axis->BeSequence->ChargePump), AdZero, OFFSET_ADJ );

	if( HwbbSts == TRUE )/* <HWBB> */
	{
		AdjustError = TRUE;								/* HWBB�Ȃ猋�ʂ��̗p���Ȃ�					*/
	}
	else if( AdjustError == FALSE )						/* Check Adjust Error						*/
	{
		/* �p�����[�^�v�Z�֐���call���Ē������ʂ��i�[												*/
		/* PnE50 �d�����o�[����(U��)																*/
		TunePrmWriteRam( &AccessPrm, &pndef_zadjiu, AdZero[0], 2 );

		/* PnE51 �d�����o�[����(V��)																*/
		TunePrmWriteRam( &AccessPrm, &pndef_zadjiv, AdZero[1], 2 );
	}


/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�뒲�I������																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* �����������ł̂ݐݒ肳��邽�߁A�����ł�A/D��~�A�����t���O�ݒ�����s���Ȃ� */
//	pAsicHwReg->AREG_ADFCNT2 = 0x0000;					/* AD��~									*/
	/* KPX_MPCMD_SYNC1( ); */							/* AD�����t���O�P(�z�X�g���荞�݂̑���)	*/
	/* �����ł�A/D�����t���O�ݒ�͎��s���Ȃ� */
//	pAsicHwReg->MREG_HSUR = MPCMD_SYNC1;				/* AD�����t���O�P(�z�X�g���荞�݂̑���)	*/
	/* ��knlMode == KPI_KMD_ADJCURAD�Ȃ�A�c�a��H���� LpxDBControlProcess()�͉����������Ȃ�		*/
	/* ���{��������DB ON�Ƃ���																		*/
	SHalDrv_DynamicBrakeON(pAsicHwReg);					/* DB ON									*/
//	pAsicHwReg->AREG_ALMSET &= ~(BIT5);					/* DB ON									*/

	if( AdjustError == FALSE )
	{
		return( KPI_RC_ADJOK );							/* Adjust OK								*/
	}
	else
	{
		return( KPI_RC_ADJERR );						/* Adjust Error								*/
	}
}



/****************************************************************************************************/
/*																									*/
/*		�d�����o�`�c�I�t�Z�b�g�擾����(�I�t���C���^�I�����C���p)									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T �v : �t���^�u���̓d�����o�`�c�I�t�Z�b�g�����p�f�[�^���擾����B								*/
/*			�Ȃ��A�I�t�Z�b�g�ʂ��}512�͈͂𒴂���ꍇ�y�сA�I�t�Z�b�g�ݒ��̃h���t�g��				*/
/*			�}2���ɓ���Ȃ������ꍇ�́A�����ُ�Ƃ��ăI�t�Z�b�g�ʂ��[���ɂ���B						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG TciGetCurADOffset( ASICS *SvAsicRegs, PUMP_CTR_STR *PumpOnCtrl, LONG AdZero[2], LONG Mode )
{
	long	Driftx;
	long	AdMean[2];
	long	AdMeanx[2];
	long	Errflag;

	/* ���[�J���ϐ��̏����� */
	Errflag = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�I�t�Z�b�g�擾																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( TciReadCurADMean( SvAsicRegs, PumpOnCtrl, AdMean, Mode ) == TRUE )
	{ /* ���ϒl�Ǎ��ݐ��� */
		if( (AdMean[0] >= -512) && (AdMean[0] <= 512) )	/* U���뒲�f�[�^����						*/
		{
			AdZero[0] = -AdMean[0];						/* Set Adjust Data							*/
		}
		else											/* U���뒲�f�[�^�ُ�						*/
		{
			AdZero[0] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	/*----------------------------------------------------------------------------------------------*/
		if( (AdMean[1] >= -512) && (AdMean[1] <= 512) )	/* V���뒲�f�[�^����						*/
		{
			AdZero[1] = -AdMean[1];						/* Set Adjust Data							*/
		}
		else											/* V���뒲�f�[�^�ُ�						*/
		{
			AdZero[1] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{ /* ���ϒl�Ǎ��ݎ��s(OC����) */
		AdZero[0] = 0;									/* Set Zero									*/
		AdZero[1] = 0;									/* Set Zero									*/
		Errflag = TRUE;									/* Set AdjustError Flag						*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�d�����o�`�c�h���t�g�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TciReadCurADMean( SvAsicRegs, PumpOnCtrl, AdMeanx, Mode ) == TRUE )
	{ /* ���ϒl�Ǎ��ݐ��� */
		Driftx = AdMean[0] - AdMeanx[0];				/* U���h���t�g�`�F�b�N						*/
		if( (Driftx < -2) || (Driftx > 2) )				/* �h���t�g���e�͈̓`�F�b�N					*/
		{
			AdZero[0] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	/*----------------------------------------------------------------------------------------------*/
		Driftx = AdMean[1] - AdMeanx[1];				/* V���h���t�g�`�F�b�N						*/
		if( (Driftx < -2) || (Driftx > 2) )				/* �h���t�g���e�͈̓`�F�b�N					*/
		{
			AdZero[1] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{ /* ���ϒl�Ǎ��ݎ��s(OC����) */
		AdZero[0] = 0;									/* Set Zero									*/
		AdZero[1] = 0;									/* Set Zero									*/
		Errflag = TRUE;									/* Set AdjustError Flag						*/
	}
	return Errflag;
}



/* �I�t���C�����[�h�����폜 start futami */
/****************************************************************************************************/
/*																									*/
/*		�d�����o�`�c���ϒl�Ǎ���(�I�����C���p)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �d�����o�`�c�l���S�O�X�U��Ǎ��݁A���̕��ϒl��Ԃ��B									*/
/*																									*/
/*	�� �� : U���d��ADC�l�̉���2bit�͖����Ȃ���, �V�t�g2bit�����Ŏ̂Ă�B							*/
/*			V���d��ADC�l�̉���2bit�͖����Ȃ���, �V�t�g2bit�����Ŏ̂Ă�B							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG TciReadCurADMean( ASICS *SvAsicRegs, PUMP_CTR_STR *PumpOnCtrl, long AdMean[2], long Mode )
{
	LONG				i;
	LONG				AdBuf[2];
	LONG				Errflag;
	ASIC_HW_IF_STR		*pAsicHwReg;
	ASIC_U_IF_STR		*pAsicMicroReg;

	/* ���[�J���ϐ��̏����� */
	Errflag = FALSE;
	pAsicHwReg = SvAsicRegs->AsicHwReg;
	pAsicMicroReg = SvAsicRegs->AsicMicroReg;

/* KpiAdjustCurAdUGain���s����JOG���[�h�ł̑��x�w�߂���͂��邽�߁AAdjPumponReq���͕s�v */
/*--------------------------------------------------------------------------------------------------*/
/*		Start Charge Pump																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Mode == OFFSET_ADJ )							/* Motor Current Offset Adjust Mode			*/
	{
		/* KPI_ALARM_CLEAR( ); */						/* ASIC�A���[���N���A						*/
		pAsicHwReg->AREG_ALMCLR = 0x802F;				/* ASIC�A���[���N���A						*/
		/* �d�����oA/D�I�t�Z�b�g�������͎��s(�ʏ��ScanC�̃`���[�W�|���v�������Ŏ��s���邽�ߕs�v)	*/
#ifndef	_JL076_USE_
		pAsicMicroReg->IntAdP.FccRst = 0x01;			/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
#else
		pAsicMicroReg->MREG_FCCRST = 0x01;				/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
#endif
		PumpOnCtrl->AdjPumponReq = TRUE;				/* Set PUMPON Request						*/
		KlibWaitms( 25 );								/* Wait Start Charg Pump					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Read AD 4096 Times 																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Mode == OFFSET_ADJ )							/* Motor Current Offset Adjust Mode			*/
	{
		/* KPI_ALARM_CLEAR( ); */						/* ASIC�A���[���N���A 						*/
		pAsicHwReg->AREG_ALMCLR = 0x802F;				/* ASIC�A���[���N���A 						*/
		/* �d�����oA/D�I�t�Z�b�g�������͎��s(�ʏ��ScanC�̃`���[�W�|���v�������Ŏ��s���邽�ߕs�v)	*/
#ifndef	_JL076_USE_
		pAsicMicroReg->IntAdP.FccRst = 0x01;			/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
#else
		pAsicMicroReg->MREG_FCCRST = 0x01;				/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
#endif
		/* ������������A/D�ϊ��ݒ�����Ă��邽�߁A�����ł͂��Ȃ� */
	//	pAsicHwReg->AREG_ADFCNT2 = 0x0038;				/* AD�ăX�^�[�g								*/
		/* KPX_MPCMD_SYNC1( ); */						/* AD�����t���O�P(�z�X�g���荞�݂̑���)	*/
		/* �����ł�A/D�����t���O�ݒ�͎��s���Ȃ� */
	//	pAsicHwReg->MREG_HSUR = MPCMD_SYNC1;			/* AD�����t���O�P(�z�X�g���荞�݂̑���)	*/
		/* �d�����oA/D�I�t�Z�b�g�������̂݁A�{���������s */
#ifndef	_JL076_USE_
		pAsicMicroReg->AdinV.IuOffsetIn = 0;			/* U���d�����o�[����						*/
		pAsicMicroReg->AdinV.IvOffsetIn = 0;			/* V���d�����o�[����						*/
#else
		pAsicMicroReg->MREG_IUOFIN = 0;					/* U���d�����o�[����						*/
		pAsicMicroReg->MREG_IUOFIN = 0;					/* V���d�����o�[����						*/
#endif
	}

	/* KPX_REFAD_LATCH( ); */							/* Latch AD Conversion Data					*/
	pAsicHwReg->AREG_ADSET1 |= BIT14;					/* Latch AD Conversion Data					*/

	AdBuf[0] = AdBuf[1] = 0;							/* Clear Buffer								*/
	for( i=0; i < 0x1000; i++ )							/* Read Loop 4096 Times						*/
	{
	/*----------------------------------------------------------------------------------------------*/
		/* KPX_REFAD_LATCH( ); */							/* Latch AD Conversion Data				*/
		pAsicHwReg->AREG_ADSET1 |= BIT14;					/* Latch AD Conversion Data				*/
		AdBuf[0] += ( (SHORT)(pAsicHwReg->AREG_AFUL)>>2 );	/* Read AD and Add Buf.(U-Phase)		*/
		AdBuf[1] += ( (SHORT)(pAsicHwReg->AREG_AFVL)>>2 );	/* Read AD and Add Buf.(V-Phase)		*/
		/* KpiResetWatchdogWaitus( 50 ); */					/* Wait 50us for AD CycleTime			*/
		/* intScanB()�Ŗ���E�H�b�`�h�b�O���Z�b�g�����s���邽�߁A�����ł͏������Ȃ� */
//		pAsicHwReg->AREG_HINT1 = 0x0000;					/* Reset Watchdog						*/
		/* �E�H�b�`�h�b�O���Z�b�g�������Ȃ��̂ŁA�E�F�C�g���폜 */
//		KlibWaitus( 50 );									/* Wait 50us							*/
		KlibWaitus( 50 );									/*  <S1A7> Wait 50us	A/D�ϊ��҂�		*/
//		pAsicHwReg->AREG_HINT1 = 0x0000;					/* Reset Watchdog						*/
	/*----------------------------------------------------------------------------------------------*/
		if( (pAsicHwReg->AREG_FLTSTAT & 0x0020) != 0 )		/* Check OC(Over Current)				*/
		{
			Errflag = TRUE;
			break;
		}
	} /* for( i=0; i < 0x1000; i++ ) */

/*--------------------------------------------------------------------------------------------------*/
/*		Stop Charge Pump																			*/
/*--------------------------------------------------------------------------------------------------*/
//	if( Mode == ONLINE_ADJ)								/* Online Adjust Mode						*/
//	{
//		PumpOnCtrl->AdjPumponReq = FALSE;				/* Reset PUMPON Request						*/
//	}
//	/* ASIC_WR( MREG_FCCRST, 0x01  ); */				/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
//	pAsicMicroReg->MREG_FCCRST	= 0x01;					/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/

	if( Mode == OFFSET_ADJ)								/* Online Adjust Mode						*/
	{
		PumpOnCtrl->AdjPumponReq = FALSE;				/* Reset PUMPON Request						*/
		/* �d�����oA/D�I�t�Z�b�g�������͎��s(�ʏ��ScanC�̃`���[�W�|���v�������Ŏ��s���邽�ߕs�v)	*/
#ifndef	_JL076_USE_
		pAsicMicroReg->IntAdP.FccRst = 0x01;			/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
#else
		pAsicMicroReg->MREG_FCCRST = 0x01;				/* AD�ϊ��f�W���[�V�����t�B���^��AlmClr		*/
#endif
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate AD Mean																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Errflag == FALSE )
	{
		AdMean[0] = ( (AdBuf[0]>>11) + 1 ) >> 1;		/* U-Phase Current AD Mean					*/
		AdMean[1] = ( (AdBuf[1]>>11) + 1 ) >> 1;		/* V-Phase Current AD Mean					*/
		return( TRUE );									/* Normal Return							*/
	}
	else
	{
		AdMean[0] = AdMean[1] = 0;						/* Clear Current AD Mean					*/
		return( FALSE );								/* Error Return								*/
	}
}
/* �I�t���C�����[�h�����폜 end futami */
/******************************* End of File *******************************/
