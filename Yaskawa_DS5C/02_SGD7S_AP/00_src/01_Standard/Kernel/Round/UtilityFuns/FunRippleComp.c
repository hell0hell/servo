/****************************************************************************
  Description   : ���x���b�v���⏞��񏑂����݃��[�h�i200DH�j
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2014 All Rights Reserved

  Project       : Mercury

  Create Date   : 2014-02-08

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "PnPrmListTbl.h"

/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/
#define	MOTOR_SN_DATA_SIZE			8		/* ���[�^�V���A��No.�f�[�^�T�C�Y[word]		*/
#define	RI_PCOMP_NUM_ELE			6		/* ���x���b�v���⏞��(�d�C�p)������			*/
#define	EEPROM_WRITE_COMPLETE		0x0000	/* ���x���b�v���⏞��񏑍��݊m�F : ����	*/
#define	EEPROM_WRITE_NOTCOMPLETE	0x0000	/* ���x���b�v���⏞��񏑍��݊m�F : ������	*/
#define	EEPROM_WRITE_ERROR			0x0002	/* ���x���b�v���⏞��񏑍��݊m�F : ���s	*/


/****************************************************************************/
/*		PROTOTYPE															*/
/****************************************************************************/
static LONG WriteRippleCompPrmToEeprom( AXIS_HANDLE *Axis );
static LONG WriteMotorSerialNoToEeprom( AXIS_HANDLE *Axis );
static void WriteRippleCompDataToRam( AXIS_HANDLE *Axis );	/* <S163> */

/*******************************************************************************
 200DH: Entry function of Velocity Ripple Compensation Information Write Mode
********************************************************************************/
PRM_RSLT FnEnterRippleComp(FUNEXE *FunExe, AXIS_HANDLE *Axis)/*<S1FD>*/
{
//	PRM_RSLT rc;
//	FUN_CMN_CONTROL	*FnCmnCtrl;/* <S1FD> */
	RPLCMP			*RippleCmp;
	PRMDATA			*Prm;
	REG_ACC_T		AccessPrm;

//	FnCmnCtrl = Axis->FnCmnCtrl;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);
	Prm       = Axis->UniPrms->Prm;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* 200Dh���s���͋����I�ɑ��x���b�v���⏞�@�\��L���ɂ��� */
	RippleCmp->RplCmpPrm.RippleLstWrnMode = RippleCmp->RplCmpPrm.RippleWrnMode; /* �O��l�ۑ� */
	RippleCmp->RplCmpPrm.RippleWrnMode    = TRUE/*FALSE*/;

	/* Pn423�Čv�Z���� */
	TunePrmWriteRam( &AccessPrm, &pndef_RippleCompSw, (LONG)Prm->RippleCompSw, 2 );

	FunExe->HoldFnMode = TRUE;								/* Fn���[�h�����I���Ȃ� */
//	FnCmnCtrl->KnlMode = KPI_KMD_RIPPLECOMPWRITE;			/* ���x���b�v���⏞��񏑂����ݒ� *//* <S1FD> */
	RippleCmp->RippleCmpState = EEPROM_WRITE_NOTCOMPLETE;	/* ���x���b�v���⏞��񏑍��݊m�F = 01H:������ */

//	rc = PRM_RSLT_SUCCESS;
//	return (rc);
	return PRM_RSLT_SUCCESS;
}

/*******************************************************************************
 200DH: Execute function of Velocity Ripple Compensation Information Write Mode
********************************************************************************/
PRM_RSLT FnExecuteRippleComp(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT			errRes;
	LONG				WriteRslt;
//	MENCV				*MencV;		/*<S1FD>*/
//	FUN_CMN_CONTROL		*FnCmnCtrl;	/*<S1FD>*/
	RPLCMP				*RippleCmp;

//	FnCmnCtrl = Axis->FnCmnCtrl;	/*<S1FD>*/
//	MencV = Axis->MencV;			/*<S1FD>*/
	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	switch(FunExe->State)
	{
	case 0:
		switch(FunExe->CmdCode)
		{
/* <S163> Start */
		case FCMD_EXEC:	/* RAM�����ݏ���	*/
			errRes = PRM_RSLT_SUCCESS;			/* ���������ϐ��C��<S191> */
			WriteRippleCompDataToRam( Axis );
			break;
/* <S163> End */

		case FCMD_UP:	/* �ی쏈��	*/
			errRes = PRM_RSLT_SUCCESS;
			FunExe->State = 1;
			break;
		case FCMD_STOP:
		case FCMD_NONE:
			errRes = PRM_RSLT_SUCCESS;
			break;
		default:
			FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */
			errRes = PRM_RSLT_CONDITION_ERR;
			break;
		}
		break;

	case 1:	/* ���s�v���҂� */
		switch(FunExe->CmdCode)
		{
		case FCMD_EWRT:	/* EEPROM�����ݏ���	*/
			FunExe->State = 2;
			errRes = PRM_RSLT_CONTINUE;
			break;

		case FCMD_STOP:
		case FCMD_NONE:
			errRes = PRM_RSLT_SUCCESS;
			break;
		default:
			FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */
			errRes = PRM_RSLT_CONDITION_ERR;
			break;
		}
		break;

	case 2:	/* ���x���b�v���⏞��񏑂����ݏ��� */

			/* ���x���b�v���⏞�p�p�����[�^EEPROM�ɏ������� */
			WriteRslt = WriteRippleCompPrmToEeprom( Axis );

			if( WriteRslt == 0 )
			{/* EEPROM�ւ̏������ݐ��튮��  */
				errRes = PRM_RSLT_SUCCESS;
				RippleCmp->RippleCmpState = EEPROM_WRITE_COMPLETE;	/* ���x���b�v���⏞��񏑍��݊m�F = 00H:���� */
				FunExe->State = 1;
			}
			else
			{/* EEPROM�ւ̏������ُ݈튮��  */
				errRes = PRM_RSLT_CONDITION_ERR;
				RippleCmp->RippleCmpState = EEPROM_WRITE_ERROR;		/* ���x���b�v���⏞��񏑍��݊m�F = 02H:���s */
			}

			/* �ڑ����[�^�̃V���A���ԍ�EEPROM�ɏ������� */
			WriteRslt = WriteMotorSerialNoToEeprom( Axis );

			if( WriteRslt == 0 )
			{/* EEPROM�ւ̏������ݐ��튮��  */
				errRes = PRM_RSLT_SUCCESS;
				FunExe->State = 1;
				RippleCmp->RippleCmpState = EEPROM_WRITE_COMPLETE;	/* ���x���b�v���⏞��񏑍��݊m�F = 00H:���� */
			}
			else
			{/* EEPROM�ւ̏������ُ݈튮��  */
				errRes = PRM_RSLT_CONDITION_ERR;
				RippleCmp->RippleCmpState = EEPROM_WRITE_ERROR;		/* ���x���b�v���⏞��񏑍��݊m�F = 02H:���s */
			}

		break;

	default:
		errRes = PRM_RSLT_SUCCESS;
		break;
	}

	return errRes;
}

/*******************************************************************************
 200DH: End function of Velocity Ripple Compensation Information Write Mode
********************************************************************************/
void FnLeaveRippleComp(FUNEXE *FunExe, AXIS_HANDLE *Axis)/* <S1FD> */
{
//	FUN_CMN_CONTROL	*FnCmnCtrl;
//	REGIF_CONFIG_T	*RegCfg;					/* <S1FD> */
	RPLCMP			*RippleCmp;
	PRMDATA			*Prm;
	REG_ACC_T		AccessPrm;

//	FnCmnCtrl = Axis->FnCmnCtrl;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);
	Prm       = Axis->UniPrms->Prm;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* Pn423��200Dh���s�O�̏�Ԃɖ߂� */
	RippleCmp->RplCmpPrm.RippleWrnMode = RippleCmp->RplCmpPrm.RippleLstWrnMode; /* �O��l��߂� */

	/* Pn423�Čv�Z���� */
	TunePrmWriteRam( &AccessPrm, &pndef_RippleCompSw, (LONG)Prm->RippleCompSw, 2 );

	FunExe->HoldFnMode = FALSE;				/* Escape Fn mode */
//	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;	/* normal mode *//* <S1FD> */
}

/* <S163> Start */
/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞�p������RAM�ɏ������ݏ���													*/
/*																									*/
/*			�@ �\ :	SigmaWin+�ɂĎZ�o����A���W�X�^�ɏ������܂ꂽ									*/
/*					�u���x���b�v���⏞�p�����v��RAM�֏������ށB										*/
/*																									*/
/****************************************************************************************************/
static void WriteRippleCompDataToRam( AXIS_HANDLE *Axis )
{
	PRMDATA			*Prm;
	RPLCMP			*RippleCmp;
	LONG			i;

	/* ������ */
	Prm = Axis->UniPrms->Prm;
	RippleCmp = &(Axis->BaseLoops->RippleCmp);

	/*----------------------------------------------------------------------------------------------*/
	/*		���b�v���␳�p�����[�^�R�s�[(���W�X�^��RAM)												*/
	/*----------------------------------------------------------------------------------------------*/
	Prm->RippleCompGainE = RippleCmp->RplCmpPrm.RplCompGainEle;

	for( i = 0; i < RI_PCOMP_NUM_ELE; i++ )
	{	/* �d�C�p�␳ */
		Prm->RippleCompCycleE[i]     = RippleCmp->RplCmpPrm.RplCompCycleEle[i];
		Prm->RippleCompAmplitudeE[i] = RippleCmp->RplCmpPrm.RplCompAmplitudeEle[i];
		Prm->RippleCompPhaseE[i]     = RippleCmp->RplCmpPrm.RplCompPhaseEle[i];
	}

	return;
}
/* <S163> End */

/****************************************************************************************************/
/*																									*/
/*		���x���b�v���⏞�p�p�����[�^EEPROM�ɏ������ݏ���											*/
/*																									*/
/*			�@ �\ :	SigmaWin+�ɂĎZ�o����APnF**�֏������܂ꂽ										*/
/*					�u���x���b�v���⏞�p�p�����[�^�v��EepRom�֏������ށB							*/
/*																									*/
/****************************************************************************************************/
static LONG WriteRippleCompPrmToEeprom( AXIS_HANDLE *Axis )
{
	LONG			rc;
	REGIF_CONFIG_T	*RegCfg;
//	PRMDATA			*Prm;		/* <S163> Delete */
//	RPLCMP			*RippleCmp;	/* <S1FD> */
//	LONG			i;			/* <S1FD> */

	/* ������ */
	rc = PRM_RSLT_SUCCESS;
	RegCfg = Axis->RegManager->hPnReg;
//	Prm = Axis->UniPrms->Prm;					/* <S163> Delete */
//	RippleCmp = &(Axis->BaseLoops->RippleCmp);	/* <S1FD> */

/* <S163> Start */
/* ���b�v���␳�p�����[�^�R�s�[(���W�X�^��RAM)�́u����ݒ背�W�X�^(42H�F0x80004002/40H�F0x2001) = 0x0001(���s)�v�Ŏ��s����悤�Ɏd�l�ύX�B */
#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���b�v���␳�p�����[�^�R�s�[(���W�X�^��RAM)												*/
	/*----------------------------------------------------------------------------------------------*/
	Prm->RippleCompGainE = RippleCmp->RplCmpPrm.RplCompCycleEle[i];

	for( i = 0; i < RI_PCOMP_NUM_ELE; i++ )
	{	/* �d�C�p�␳ */
		Prm->RippleCompCycleE[i] = RippleCmp->RplCmpPrm.RplCompCycleEle[i];
		Prm->RippleCompAmplitudeE[i] = RippleCmp->RplCmpPrm.RplCompAmplitudeEle[i];
		Prm->RippleCompPhaseE[i] = RippleCmp->RplCmpPrm.RplCompPhaseEle[i];
	}
#endif
/* <S163> End */

	/*----------------------------------------------------------*/
	/*		EEPROM�ɏ������ݏ���								*/
	/*----------------------------------------------------------*/
	/* PnFA0 ���x���b�v���⏞�Q�C��(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompGainE, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA1 ���x���b�v���⏞��1��������(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA2 ���x���b�v���⏞��1�����U���l(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA3 ���x���b�v���⏞��1�����ʑ�(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA4 ���x���b�v���⏞��2��������(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA5 ���x���b�v���⏞��2�����U���l(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA6 ���x���b�v���⏞��2�����ʑ�(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA7 ���x���b�v���⏞��3��������(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle3, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA8 ���x���b�v���⏞��3�����U���l(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude3, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFA9 ���x���b�v���⏞��3�����ʑ�(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase3, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAA ���x���b�v���⏞��4��������(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle4, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAB ���x���b�v���⏞��4�����U���l(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude4, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAC ���x���b�v���⏞��4�����ʑ�(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase4, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAD ���x���b�v���⏞��5��������(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle5, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAE ���x���b�v���⏞��5�����U���l(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude5, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFAF ���x���b�v���⏞��5�����ʑ�(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase5, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFB0 ���x���b�v���⏞��6��������(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompCycle6, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFB1 ���x���b�v���⏞��6�����U���l(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompAmplitude6, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* PnFB2 ���x���b�v���⏞��6�����ʑ�(�d�C�p) */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_RippleCompPhase6, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	return(rc);
}

/****************************************************************************************************/
/*																									*/
/*		�ڑ����[�^�̃V���A���ԍ�EEPROM�ɏ������ݏ���												*/
/*																									*/
/*			�@ �\ :	�ڑ����[�^�̃V���A���ԍ���EepRom�֏������ށB									*/
/*																									*/
/****************************************************************************************************/
static LONG WriteMotorSerialNoToEeprom( AXIS_HANDLE *Axis )
{

	LONG	rc;
//	EEP_DEVICE_HANDLE	*EepDevice;/* <Oka02> */
	UINT				AxisNo;
	USHORT				SerialNo[MOTOR_SN_DATA_SIZE];
	LONG				ArrayIdx;

	/* ������ */
//	EepDevice = (Axis->RegManager->hPnReg)->hEepDev;/* <Oka02> */
	AxisNo = Axis->AxisNo;
	rc = PRM_RSLT_SUCCESS;

	/* ���[�^�V���A��No.�擾 */
	for(ArrayIdx = 0;  ArrayIdx < MOTOR_SN_DATA_SIZE; ArrayIdx++)
	{
		SerialNo[ArrayIdx] = (USHORT)(Axis->MencV->MotInfo.SNo[ArrayIdx*2 + 1] & 0x00FF);
		SerialNo[ArrayIdx] |= (USHORT)((Axis->MencV->MotInfo.SNo[ArrayIdx*2] & 0x00FF) << 8);
	}

	/* ���[�^�V���A��No.��EepRom�֏������� */
	for(ArrayIdx = 0;  ArrayIdx < MOTOR_SN_DATA_SIZE; ArrayIdx++)
	{
		/* write to EEPROM */
		rc = EepdevWriteNoSumValues( 0,															/* EEPROM Device ID(0: SPI(IF), 1: I2C(SDA) */
									 EEP_NOSUM_ADDRESS(AxisNo, EEP_MOTORSN_OFS) + (ArrayIdx*2),	/* Eeprom Address in bytes */
									 &SerialNo[ArrayIdx],										/* Pointer to the buffer for writing */
									 1 );														/* writing size in words */

		if( rc != 0 )	/* EEPROM�ւ̏������ݎ��s */
		{
			break;
		}

	}

	return (rc) ;
}
/***************************************** end of file **********************************************/
