/****************************************************************************
  Description   : �A�i���O�w�ߒ���(Fn009, Fn00A, Fn00B)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2013 All Rights Reserved

  Project       : Mercury

  Create Date   : 2013-11-26

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


/* ���x�w�߃I�t�Z�b�g�̃}�j���A�������l�@�ő�l/�ŏ��l */
//#define	ZADJV_MAX			32767	/* <S0A9>	Delete */
//#define	ZADJV_MIN			-32768	/* <S0A9>	Delete */

/* <S0A9>	Start */
#if	( SVFSEL_VREFAD_24BIT != 0 )
#define	ZADJV_MAX			52500000
#define	ZADJV_MIN			-52500000
#define	ZADJV_RATE			3500			/* 52500000��15000 = 3500	*/
#else
#define	ZADJV_MAX			240000
#define	ZADJV_MIN			-240000
#define	ZADJV_RATE			16				/* 240000��15000 = 16		*/
#endif
/* <S0A9>	End */


/* �g���N�w�߃I�t�Z�b�g�̃}�j���A�������l�@�ő�l/�ŏ��l */
//#define	ZADJT_MAX		0xffff	/* <S0A9> Delete */
//#define	ZADJT_MIN		0x0000	/* <S0A9> Delete */
#define	ZADJT_MAX			127		/* <S0A9> Add */
#define	ZADJT_MIN			-127	/* <S0A9> Add */


/* ���[�U���x�� */
//#define	ACCLVL_SYSTEM	3	/* ���[�U���x�� = �V�X�e�����x�� */	/* <S101> */
#define	ACCLVL_SYSTEM	4		/* ���[�U���x�� = �V�X�e�����x�� */	/* <S101> */


//#define	OK	1					/* �d�����o�M���̃I�t�Z�b�g�蓮�����@���s�\ *///<S0C7>
//#define	NG	0					/* �d�����o�M���̃I�t�Z�b�g�蓮�����@���s�s�\ *///<S0C7>

#define	TREFZVADP		0x0C2	/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l				*/
#define	TREFZVADN		0x098	/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l				*/


/*		�w�ߗp�`�c�R���g���[��			*/
#define _UDL_REG_ADDR_  (ULONG)0xC0000000
#define KPX_REFAD_LATCH( )			(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ADSET1) |= (1<<14)

/* ���Z(64bit+32bit) */
#define	ADD6432( a, b, xx )		*(INT64*)xx = *(INT64*)(a) + (INT64)(b)

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* <S0BC> Start */
/* ���L��4�̊֐��͊֐����������֐����u\Kernel\Driver\AdcIf.c�v�ɂ���ׁA�֐����̂�ύX����B */
//static LONG AdjustRefAdZero( AXIS_HANDLE *Axis );
//static void TciAdjustRefAdZero( AXIS_HANDLE *Axis );
//static void TciReadTrefVrefADMean( LONG AdMean[3] );
//static void	MlibAdd6432( LONG *a, LONG b, LONG *x );

static LONG FnAdjustRefAdZero( AXIS_HANDLE *Axis );
static void FnTciAdjustRefAdZero( AXIS_HANDLE *Axis );
static void FnTciReadTrefVrefADMean( LONG AdMean[3] );
static void	FnMlibAdd6432( LONG *a, LONG b, LONG *x );
/* <S0BC> End */


/****************************************************************************
 Fn009: Entry function of Analog Reference Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgRefOffsetAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;	/* reference A/D adjusting  */

//#endif	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn009: Execute function of Analog Reference Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgRefOffsetAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

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
//	if( (SeqCtrlOut->MainPowerOn == FALSE)	/* <S0FC> */
//		|| (SeqCtrlOut->HwbbSts != FALSE)	/* <S0FC> */
//		|| (SeqCtrlOut->MotStop == FALSE)	/* <S0FC> */
//		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0) )	/* Base Enable */	/* <S0FC> */
	if( SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0 )			/* Base Enable */	/* <S0FC> */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* �������s */
//	AdjResult = AdjustRefAdZero(Axis);		/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
	AdjResult = FnAdjustRefAdZero(Axis);	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */

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

//#endif		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
	return PRM_RSLT_SUCCESS;
}
#if 0	/* <S0E8>	Delete:�{�֐��͏������Ȃ��ׁA�폜����B  */
/****************************************************************************
 Fn009: End function of Analog Reference Offset Auto Adjustment
****************************************************************************/
void FnLeaveAnlgRefOffsetAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */

	REGIF_CONFIG_T	*RegCfg;
	RegCfg = Axis->RegManager->hPnReg;

	/* �I������ */
	/* EEPROM�X�V���� */
	//PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );		/* PnE53 : ���x�w�߃[���� */					/* <S0A9>	Delete */
	//PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );		/* PnE56 : �g���N�w�߃[����(������/������) */	/* <S0A9>	Delete */

//#endif		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
}
#endif	/* <S0E8>	Delete:�{�֐��͏������Ȃ��ׁA�폜����B */


/****************************************************************************
 Fn00A: Entry function of Velocity Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgVelocityRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;


//	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;	/* reference A/D adjusting  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;	/* normal mode */	/* <S142> */

	FunExe->HoldFnMode = TRUE;				/* Fn���[�h�����I���Ȃ� */
//#endif		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn00A: Execute function of Velocity Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgVelocityRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRMDATA			*Prm;
	PRM_RSLT		rc;
	LONG			zadjvl_tmp;
	CHAR			Exe_flg;
	FUN_AXCOMMON	*AxCommon;
	CADC	*pAdc;	/* <S0F9> */

	Prm = Axis->UniPrms->Prm;
	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;
	pAdc = Axis->pAdc;	/* <S0F9> */

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
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
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:	/* UP,���],������ */
			if(Exe_flg == OK)
			{
				/* PnE53 : ���x�w�߃[�����̒l�ǂݍ��� */
				//zadjvl_tmp = (LONG)(SHORT)((Prm->zadjvl) & 0xFFFF);/* <S0A9> Data�^�ύX */
				zadjvl_tmp = Prm->zadjvl;							/* <S0A9> Data�^�ύX */

//				zadjvl_tmp += 1;
				zadjvl_tmp += ZADJV_RATE;

				if( zadjvl_tmp > ZADJV_MAX )
				{
					zadjvl_tmp = ZADJV_MAX;
				}
				/*  PnE53 : ���x�w�߃[�����ɍX�V�����l��RAM�֏������� */
				//Prm->zadjvl = (SHORT)zadjvl_tmp;
				Prm->zadjvl = zadjvl_tmp;	/* <S0A9> ���������@�Ή� */
				pAdc->P.ZadjVelo = zadjvl_tmp;							/* <S170> */

			}
			break;
		case FCMD_DOWN:	/* DOWN,�t�],�t���� */
			if(Exe_flg == OK)
			{
				//zadjvl_tmp = (LONG)(SHORT)((Prm->zadjvl) & 0xFFFF);/* <S0A9> ���������@�Ή�:Data�^�ύX */
				zadjvl_tmp = Prm->zadjvl;	/* <S0A9> Data�^�ύX */

//				zadjvl_tmp -= 1;
				zadjvl_tmp -= ZADJV_RATE;

				if( zadjvl_tmp < ZADJV_MIN )
				{
					zadjvl_tmp = ZADJV_MIN;
				}
				/*  PnE53 : ���x�w�߃[�����ɍX�V�����l���������� */
				//Prm->zadjvl = (SHORT)zadjvl_tmp;
				Prm->zadjvl = zadjvl_tmp;	/* <S0A9> */

				pAdc->P.ZadjVelo = Prm->zadjvl;	/*  PnE53 : ���x�w�߃[������Zero Adjust�Ɋi�[ */	/* <S0F9> */
			}
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
			break;
		}
//#endif		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	return rc ;
}

/****************************************************************************
 Fn00A: End function of Velocity Reference Offset Manual Adjustment
****************************************************************************/
void FnLeaveAnlgVelocityRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROM�X�V����	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );		/* PnE53 : ���x�w�߃[���� */
//	PrmStoreValueToEeprom( RegCfg, &pndef_zadjvh, 0 );		/* PnE54 : ���x�w�߃[���� */	/* <S0A9> ���������@�Ή� */


	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */

//#endif	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
}

/****************************************************************************
 Fn00B: Entry function of Torque Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterAnlgTorqueRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;


//	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;	/* reference A/D adjusting  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S142> */

	FunExe->HoldFnMode = TRUE;				/* Fn���[�h�����I���Ȃ� */

//#endif		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn00B: Execute function of Torque Reference Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteAnlgTorqueRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRMDATA			*Prm;
	PRM_RSLT		rc;
//	USHORT			zadjtp_n_tmp;	/* <S0A9> Delete */
//	SHORT			zadjtp_n_tmp;	/* <S0A9> Add */
	DBYTEX			zadjtp_n_tmp;
	CHAR			Exe_flg;
	FUN_AXCOMMON	*AxCommon;
	CADC			*pAdc = Axis->pAdc;	/* <S170> */

	Prm = Axis->UniPrms->Prm;
	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
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
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:	/* UP,���],������ */
			if(Exe_flg == OK)
			{
				/* PnE56 : �g���N�w�߃[����(������/������)�̒l�ǂݍ��� */
				//zadjtp_n_tmp = Prm->zadjtp_n.w;					/* <S0A9> Delete */
//				zadjtp_n_tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S0A9> Add */
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
//				if( zadjtp_n_tmp < ZADJT_MAX )
				if( ((CHAR)zadjtp_n_tmp.b.l < ZADJT_MAX) && ((CHAR)zadjtp_n_tmp.b.h < ZADJT_MAX) )
				{
//					zadjtp_n_tmp += 1;
					(CHAR)zadjtp_n_tmp.b.l++;
					(CHAR)zadjtp_n_tmp.b.h++;
				}
				/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
//				Prm->zadjtp_n.w = zadjtp_n_tmp;
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;		/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;		/* <S170> */
			}
			break;

		case FCMD_DOWN:	/* DOWN,�t�],�t���� */
			if(Exe_flg == OK)
			{
				/* PnE56 : �g���N�w�߃[����(������/������)�̒l�ǂݍ��� */
				//zadjtp_n_tmp = Prm->zadjtp_n.w;					/* <S0A9> Delete */
//				zadjtp_n_tmp = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);	/* <S0A9> Add */
				zadjtp_n_tmp.w = (SHORT)((Prm->zadjtp_n.w)& 0xFFFF);
//				if(zadjtp_n_tmp > ZADJT_MIN)
				if( ((CHAR)zadjtp_n_tmp.b.l > ZADJT_MIN) && ((CHAR)zadjtp_n_tmp.b.h > ZADJT_MIN) )
				{
//					zadjtp_n_tmp -= 1;
					(CHAR)zadjtp_n_tmp.b.l--;
					(CHAR)zadjtp_n_tmp.b.h--;
				}
				/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l���������� */
				Prm->zadjtp_n.w = zadjtp_n_tmp.w;
				pAdc->P.ZadjTrqP = (CHAR)zadjtp_n_tmp.b.l;		/* <S170> */
				pAdc->P.ZadjTrqN = (CHAR)zadjtp_n_tmp.b.h;		/* <S170> */
			}
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
			break;
		}
//#endif		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	return rc ;

}

/****************************************************************************
 Fn00B: End function of Torque Reference Offset Manual Adjustment
****************************************************************************/
void FnLeaveAnlgTorqueRefManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROM�X�V����	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );		/* PnE56 : �g���N�w�߃[����(������/������) */

	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */

//#endif	/* <S0A9> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
}


/****************************************************************************************************/
/*																									*/
/*		�w�߂`�c�뒲����(�I�����C���p)																*/
/*																									*/
/****************************************************************************************************/
//static LONG	AdjustRefAdZero( AXIS_HANDLE *Axis )	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
static LONG	FnAdjustRefAdZero( AXIS_HANDLE *Axis )		/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	LONG	i;
	CADC *pAdc;

	pAdc = Axis->pAdc;

/*--------------------------------------------------------------------------------------------------*/
/*		�w�߂`�c�뒲����																			*/
/*--------------------------------------------------------------------------------------------------*/
		pAdc->f.AdjustError = FALSE;					/* Reset AdjustError Flag */
		pAdc->Mode = ADCMD_ADJREFAD;					/* set adjust mode */
//		TciAdjustRefAdZero( Axis  );					/* Adjust VrefAD and TrefAD */	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
		FnTciAdjustRefAdZero( Axis  );					/* Adjust VrefAD and TrefAD */	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
		pAdc->Mode = ADCMD_NORMAL;						/* reset adjust mode */	/* <S0E9> */

/*--------------------------------------------------------------------------------------------------*/
/*		�I������																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( pAdc->f.AdjustError == FALSE )
		{
			return( KPI_RC_ADJOK );					/* Adjust OK								*/
		}
		else
		{
			return( KPI_RC_ADJERR );					/* Adjust Error								*/
		}

//#endif	/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
}

/****************************************************************************************************/
/*																									*/
/*		�w�߂`�c�뒲����(�I�t���C���^�I�����C���p)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �e�w�߂`�c�̓��̓[�����̒����p�f�[�^(�w�ߓ��̓[�����̃I�t�Z�b�g��)���쐬���A			*/
/*			���ꂼ��p�����[�^�ɐݒ肷��B															*/
/*			�Ȃ��A�I�t�Z�b�g�ʂ��ȉ��͈̔͂��O���ꍇ�́A�����ُ�Ƃ��ăI�t�Z�b�g�ʂ�				*/
/*			�[���ɂ���B																			*/
/*																									*/
/*			1) T-ref P [�ٸ�w��A/D ����] : -64 �� �̾�� �� 64	(64/4096=1.56%)						*/
/*																									*/
/*			2) T-ref N [�ٸ�w��A/D ����] : -64 �� �̾�� �� 64	(64/4096=1.56%)						*/
/*																									*/
/*			3) V-ref   [���x�w��A/D]	 : -52500000 �� �̾�� �� 52500000	(24Bit AD �̏ꍇ)		*/
/*										 : -200000   �� �̾�� �� 200000		(16Bit AD �̏ꍇ)		*/
/*																									*/
/*											24Bit AD �̏ꍇ ==> (52500000/(2^23*128)=4.889%)		*/
/*											16Bit AD �̏ꍇ ==> (  200000/(2^15*128)=4.768%)		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static void TciAdjustRefAdZero( AXIS_HANDLE *Axis )	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
static void FnTciAdjustRefAdZero( AXIS_HANDLE *Axis )	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */		/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	LONG	Zerox;
	LONG	Zeroy;
	LONG	AdMean[3];
	SHORT	XSat;
	SHORT	YSat;
	CADC	*pAdc;
	PRMDATA	*Prm;
	REGIF_CONFIG_T	*RegCfg;	/* <S0A9>  */

	pAdc = Axis->pAdc;
	Prm = Axis->UniPrms->Prm;
	XSat = FALSE;
	YSat = FALSE;
	RegCfg = Axis->RegManager->hPnReg;/* <S0A9>  */

/*--------------------------------------------------------------------------------------------------*/
/*		�`�c���ϒl�Ǎ���																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPX_SET_ADSEL_TREF( );					/* Select Tref AD Select Switch */
		KlibWaitms( 1 );						/* Reset Watchdog and Wait */
//		TciReadTrefVrefADMean( AdMean );		/* Read P-Tref, N-Tref, Vref AD Mean */	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
		FnTciReadTrefVrefADMean( AdMean );		/* Read P-Tref, N-Tref, Vref AD Mean */	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */


/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�w�ߗ뒲�f�[�^���Z���i�[																*/
/*--------------------------------------------------------------------------------------------------*/

		/* ����/�s���g���N�w�߂`�c�[���u���͎��̂`�c�l �쐬���m�F */
		Zerox = -AdMean[0] + TREFZVADP;			/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l �Z�o */
		//if( (Zerox < -64) || (Zerox > 64) )		/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l �m�F(10bit->12bit chk!!) */
		if( (Zerox < -127) || (Zerox > 127) )		/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l �m�F(10bit->12bit chk!!) */ /* <S169> */
		{
			pAdc->f.AdjustError = TRUE;
		}

		Zeroy = +AdMean[1] - TREFZVADN;			/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l �Z�o */
		//if( (Zeroy < -64) || (Zeroy > 64) )		/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l �m�F(10bit->12bit chk!!) */
		if( (Zeroy < -127) || (Zeroy > 127) )		/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l �m�F(10bit->12bit chk!!) */ /* <S169> */
		{
			pAdc->f.AdjustError = TRUE;
		}


		/* ����/�s���g���N�w�߂`�c�[���u���͎��̂`�c�l �쐬���m�F */
		if( Zerox >= TREFZVADP )				/* �Z�o���������g���N�w�߂`�c�[���u���͎��̂`�c�l���ݒ�l�ȏ�̏ꍇ */
		{
			XSat = TRUE;
		}
		if( Zeroy <= -(TREFZVADN) )				/* �Z�o�����s���g���N�w�߂`�c�[���u���͎��̂`�c�l���ݒ�l�ȉ��̏ꍇ */
		{
			YSat = TRUE;
		}
	/*----------------------------------------------------------------------------------------------*/
		if ( pAdc->f.AdjustError )
		{
			Zerox = 0;
			Zeroy = 0;
		}
		else if(XSat == TRUE && YSat == FALSE)	/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l���ݒ�l�ȏ� */
		{
			Zerox = Zeroy;
		}
		else if(XSat == FALSE && YSat == TRUE)	/* �Z�o�����s���g���N�w�߂`�c�[���u���͎��̂`�c�l���ݒ�l�ȉ��̏ꍇ */
		{
			Zeroy = Zerox;
		}
		else if(XSat == TRUE && YSat == TRUE)	/* �����g���N�w�߂`�c�[���u���͎��̂`�c�l���ݒ�l�ȏ�A�Z�o�����s���g���N�w�߂`�c�[���u���͎��̂`�c�l���ݒ�l�ȉ��̏ꍇ */
		{
			pAdc->f.AdjustError = TRUE;
			Zerox = 0;
			Zeroy = 0;
		}
		pAdc->P.ZadjTrqP = Zerox;								/* set Zero Adjust Variable */
		pAdc->P.ZadjTrqN = Zeroy;								/* set Zero Adjust Variable */
		Zerox = (Zerox & 0xFF) + (Zeroy << 8);					/* Combine P-Tref and N-Tref		*/
		//TODO:		Prm.zadjtp_n.w = (USHORT)Zerox;				/* Write Adjust Data to RAM			*/
		/*  PnE56 : �g���N�w�߃[����(������/������)�ɍX�V�����l��RAM�֏������� */
		Prm->zadjtp_n.w = (USHORT)Zerox;
		PrmStoreValueToEeprom( RegCfg, &pndef_zadjtp_n, 0 );	/* Write Adjust Data to EepROM */		/* <S0A9>	TODO���� */



/*--------------------------------------------------------------------------------------------------*/
/*		���x�w�ߗ뒲�f�[�^���Z���i�[																*/
/*--------------------------------------------------------------------------------------------------*/
#if	( SVFSEL_VREFAD_24BIT != 0 )
		Zerox = -AdMean[2];									/* ���x�w�߃[�����̒l �쐬 */
		if ( (Zerox < -52500000) || (Zerox > 52500000) )	/* �쐬�������x�w�߃[�����̒l �m�F */
		{
			Zerox = 0;
			KnlV.f.AdjustError = TRUE;
		}
#else
/*--------------------------------------------------------------------------------------------------*/
/*		-240000�`+240000�ȊO�̏ꍇ�A�G���[�ɂ���F	(15.36/0x7FFF)�~(240000/128) = 879mV			*/
/*																									*/
/*		�I�t�Z�b�g�����͈͂́A�}�j���A����ł́}750mV�ł��邪�A										*/
/*		Fn00A�̏�����A�I�t�Z�b�g�����͈͂�15000�Ŋ���؂������s�����ǂ��̂Ł}240000�ɂ���		*/
/*--------------------------------------------------------------------------------------------------*/
		Zerox = -AdMean[2];									/* ���x�w�߃[�����̒l �쐬 */
		if ( (Zerox < -240000) || (Zerox > 240000) )		/* �쐬�������x�w�߃[�����̒l �m�F */
		{
			Zerox = 0;
			pAdc->f.AdjustError = TRUE;
		}
#endif
	/*----------------------------------------------------------------------------------------------*/
		pAdc->P.ZadjVelo = Zerox;
//TODO:		Prm.zadjvl = (USHORT)Zerox;						/* Write Adjust Data to RAM					*/
//TODO:		Prm.zadjvh = (USHORT)(Zerox>>16);				/* Write Adjust Data to RAM					*/
		Prm->zadjvl = Zerox;								/* Write Adjust Data to RAM					*/	/* <S0A9>	TODO���� */
//		Prm->zadjvh = (USHORT)(Zerox>>16);					/* Write Adjust Data to RAM					*/	/* <S0A9>	TODO���� */
		PrmStoreValueToEeprom( RegCfg, &pndef_zadjvl, 0 );	/* Write Adjust Data to EepROM				*/	/* <S0A9> */

//#endif	/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
		return;
}
/****************************************************************************************************/
/*																									*/
/*		�g���N�w�߂`�c�����x�w�߂`�c���ϒl�Ǎ���(�I�t���C���^�I�����C���p)							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �g���N�w�߂`�c�l�y�ё��x�w�߂`�c�l���S�O�X�U��Ǎ��݁A���̕��ϒl��Ԃ��B				*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static void TciReadTrefVrefADMean( LONG AdMean[3] )	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
static void FnTciReadTrefVrefADMean( LONG AdMean[3] )	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

	LONG	i;
	LONG	Adlo;
	LONG	Adhi;
	LONG	TAdBuf[2];
	LONG	VAdBuf[2];


/*--------------------------------------------------------------------------------------------------*/
/*		�`�c�Ǎ���(4096��) : 30us * 4096 = 122.880 ms												*/
/*--------------------------------------------------------------------------------------------------*/
/*		<1>CPUAD��Repeat Mode�ɂ������߁ATrefAd�̋N�������𖳂������B								*/
/*--------------------------------------------------------------------------------------------------*/
		TAdBuf[0] = TAdBuf[1] = 0;						/* Clear Tref AD Buffer						*/
		VAdBuf[0] = VAdBuf[1] = 0;						/* Clear Vref AD Buffer						*/
		for( i=0; i < 0x1000; i++ )						/* Read Loop 4096 Times						*/
		{
			KlibWaitus( 30 );							/* Wait 30us for AD Conversion				*/
			TAdBuf[0] += KPX_READ_CPUAD0();				/* Read and Add Buf	(P-Tref)				*/
			TAdBuf[1] += KPX_READ_CPUAD1();				/* Read and Add Buf (N-Tref)				*/
			KlibWaitus( 18 );							/* Reset Watchdog and Wait					*/
		/*------------------------------------------------------------------------------------------*/
			KPX_REFAD_LATCH( );									/* Latch Vref AD Conv. Data			*/

#if	( SVFSEL_VREFAD_24BIT != 0 )	/* @SKLT@ */
			Adlo = (USHORT)(ASIC_RD( AREG_ACVLL ));				/* Read Low Word					*/
			Adhi = ((LONG)(ASIC_RD( AREG_ACVLH )<<24))>>8;		/* Read High Word					*/
			//MlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* Add  Buffer	(128*AdData)		*/	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
			FnMlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* Add  Buffer	(128*AdData)		*/	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
#else		/* 16bit AD */
			//Adlo = (SHORT)(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ACUL);	/* �b�� */	/* <S0BC> */
			Adlo = (USHORT)(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ACUL);		/* �b�� */	/* <S0BC> */
			Adhi = ((( Adlo << 16 ) >> 16 ) & 0xFFFF0000 );		/* <VZADJ>�����g��					*/
#endif
			//MlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* <V031>Add  Buffer (128*AdData)	*/	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
			FnMlibAdd6432( VAdBuf, ((Adhi+Adlo)<<7), VAdBuf );	/* <V031>Add  Buffer (128*AdData)	*/	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
		}

/*--------------------------------------------------------------------------------------------------*/
/*		���ϒl���Z																					*/
/*--------------------------------------------------------------------------------------------------*/
		AdMean[0] = (((TAdBuf[0]>>11)+1)>>1);							/* P-Tref AD Mean			*/
		AdMean[1] = (((TAdBuf[1]>>11)+1)>>1);							/* N-Tref AD Mean			*/
	 	AdMean[2] = (VAdBuf[1]<<20) | ((((ULONG)VAdBuf[0]>>11)+1)>>1);	/* Vref AD Mean				*/

//#endif	/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

		return;

}

/****************************************************************************************************/
/*																									*/
/*		���Z(64bit+32bit)																			*/
/*																									*/
/*		TODO: ��ŏ���																				*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//void	MlibAdd6432( LONG *a, LONG b, LONG *x )			/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
static void FnMlibAdd6432( LONG *a, LONG b, LONG *x )	/* <S0BC> �������̂̊֐������݂���ׁA�֐�����ύX */
{
//#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* AnalogIF */	/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */

		ADD6432( a, b, x );
//#endif														/* <S0BC> Delete:AP�`����̕⏕�@�\�ł͂Ȃ��ׁA�폜 */
		return;

}

/******************************* End of File *******************************/
