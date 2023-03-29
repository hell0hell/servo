/****************************************************************************
  Description   : DCVolatage Gain Auto Adjustment
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	F.Ueda   2011.04.26   created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "PnPrmListTbl.h"
#include "TuneLib.h"
#include "MLib.h"
#include "FnDCVoltageAdj.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* SGDV �����������[�h�Ƃ͎d�l���قȂ�̂Œ��ӁI */
static ADJUST_STATUS TciAdjustMainDcVoltage( REGIF_CONFIG_T *hPnReg, CMN_PRMDATA *cmnprm_ptr, 
											 ASIC_HW_IF_STR *pAsicHwReg, CHECK_MAIN_POWER *pMainPowerChecker, DET_DC_VOLT *pDetectDc );
static LONG TciDetectTestVdc( LONG RepNum, CHECK_MAIN_POWER *MainPowerChecker, 
							  DET_DC_VOLT *pDetectDc );
/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/


/****************************************************************************************************/
/*		STATIC VARIABLES																			*/
/****************************************************************************************************/


/****************************************************************************
 0x2012: Entry function of DCVolatage Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterDcVoltageAutoAdj( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	LONG				no;

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	for(no = 0; no < NumberOfAxes; no++)
	{
		FnCmnCtrl = ((AXIS_HANDLE*)GetAxisHandle(no))->FnCmnCtrl;
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
	}
	
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 0x2012: Execute function of DCVolatage Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteDcVoltageAutoAdj( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	SEQ_CTRL_OUT				*SeqCtrlOut;
	REGIF_CONFIG_T				*hPnReg;
	CMN_PRMDATA					*cmnprm_ptr;
	ASIC_HW_IF_STR				*AsicHwReg;
	CHECK_MAIN_POWER 			*MainPowerChecker;
	DET_DC_VOLT					*DetectDc;
	ADJUST_STATUS				rslt;
	FUN_CMN_CONTROL				*FnCmnCtrl;
	LONG						no;

	/* Initialize */
	SeqCtrlOut				=	Axis->SeqCtrlOut;
	hPnReg					=	Axis->RegManager->hPnReg;
	cmnprm_ptr				=	Axis->UniPrms->Cprm;
	AsicHwReg				=	Axis->SvAsicRegs->AsicHwReg;
	MainPowerChecker		=	&(Axis->PowerManager->MainPowerChecker);
	DetectDc				=	&(Axis->PowerManager->DetectDcVolt);


	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	for(no = 0; no < NumberOfAxes; no++)
	{
		FnCmnCtrl = ((AXIS_HANDLE*)GetAxisHandle(no))->FnCmnCtrl;
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
	}
	
	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* ���H�d��ON/OFF�Ɋ֌W�Ȃ����{�\�Ƃ��邽�߁AMainPowerOn�͊Ď����Ȃ�				*/
	if ( (SeqCtrlOut->MotStop == FALSE)									/* Motor Moving		*/
		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0) )	/* Base Enable		*/
	{
		return PRM_RSLT_CONDITION_ERR;
	}
	
	rslt = TciAdjustMainDcVoltage( hPnReg,
								   cmnprm_ptr, 
								   AsicHwReg, 
								   MainPowerChecker,
								   DetectDc );
	
	if( rslt == ADJ_SUCCESS )
	{
		;		/* �������Ȃ� */
	}
	else if( rslt == ADJ_FAILURE )
	{
		return PRM_RSLT_CALC_ERR;
	}
	
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/*		���H�d���d�����o���x������																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ :	�����@����c�b�d��(PnE72)�����āA���̎��̂c�b�d�����o�l������̒l(PnE72)�ɂȂ�		*/
/*			�悤�ɁA�����p�����[�^��ݒ肷��B(��������\�F1/256)									*/
/*			�Ȃ��A�d�����o�l���ُ�̏ꍇ�y�ѓd���������x����������͈�(�}20%)���O���ꍇ�́A		*/
/*			�����ُ�Ƃ��ēd���������x�����[���ɂ���B												*/
/*																									*/
/*			1. �����菇																				*/
/*				1) ���K����d�����Z (��d���F�񐶃I�����x��(PnE72))								*/
/*				2) ���H�d���d�����ϒl�̎擾 (�d�����o4096��̕���)								*/
/*				3) ��d���d���������x���̌v�Z (����\�F1/256)										*/
/*				4) ��d���d���������x���̊i�[ (==>PnE5B�FPrm.pnadj/EEPROM)							*/
/*																									*/
/*			2. �֘A�p�����[�^																		*/
/*				1) PnE70 : P-N�d�����o���x��  [V]		(�ő匟�o�l�̎��̓d��)						*/
/*		 		2) PnE71 : �n�u���o���x��     [V]													*/
/*				3) PnE72 : �񐶃I�����x��     [V]		(�����@��������c�b�d��)					*/
/*		 		4) PnE73 : �t�u���o���x��     [V]													*/
/*				5) PnE5B : ����:P-N�d�����o���x������	(�������ʂ��i�[)							*/
/*																									*/
/*			3. �������x�����Z��																		*/
/*				1) �L����`																			*/
/*					VdcBase�F��c�b�d��,  VdcMean�F�������Ǎ��d�����ϒl							*/
/*					VdcAdj �F�������x���l,  VdcDet �F�c�b�d�����o�l(�������x���ŕ␳��̒l)			*/
/*				2) �c�b�d�����o�l�␳��																*/
/*				   VdcDet = VdcRead * (256 + VdcAdj) / 256											*/
/*						  = VdcRead + (VdcRead * VdcAdj / 256)										*/
/*				3) �������x�����Z��																	*/
/*				   �Ǎ��d���l�� VdcMean �̏ꍇ�ɁA�d�����o�l�� VdcBase �ɂȂ�悤�ɕ␳����B		*/
/*				   VdcBase = VdcMean + (VdcMean * VdcAdj / 256) ���								*/
/*				   VdcAdj  = (VdcBase - VdcMean) * 256 / VdcMean									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static ADJUST_STATUS TciAdjustMainDcVoltage( REGIF_CONFIG_T *hPnReg, CMN_PRMDATA *cmnprm_ptr, 
								    ASIC_HW_IF_STR *pAsicHwReg, CHECK_MAIN_POWER *pMainPowerChecker, DET_DC_VOLT *pDetectDc )
{
	LONG			i;
	LONG			VdcAdj;
	LONG			VdcBase;
	LONG			VdcMean;
	LONG			VdcTest;
	LONG			ErrFlag = FALSE;
	ADJUST_STATUS	rslt;
	REG_ACC_T		AccessPrm;

	/* Initialize */
	rslt = ADJ_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr	= 0;
	AccessPrm.hRegCfg	= hPnReg;
	AccessPrm.ArrayIdx	= 0;
	AccessPrm.EepWrite	= TRUE;		/* EEPROM Write */
	AccessPrm.IfHandle	= NULL;

/*--------------------------------------------------------------------------------------------------*/
/*		�J�n�`�F�b�N																				*/
/*			�\�t�g�E�F�A�ɂ��d�����o�łȂ��ꍇ�͒����ُ�ɂă��^�[������							*/
/*--------------------------------------------------------------------------------------------------*/
/* IprmcalSystemSwitch( )�FVdetMode == 0�̎�A.040�Ƃ��邽�߁A�����ł͊Ď����Ȃ�						*/
#if 0
		if( pDetectDc->conf.VdetMode == VDET_NONE )
		{
			rslt = ADJ_NONE;								/* �����ُ�t���O�ݒ�					*/
			return rslt;									/* �������s�����ɖ߂�					*/
		}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		������d����荞��																		*/
/*			������͉񐶃I���d��																	*/
/*--------------------------------------------------------------------------------------------------*/
		/* INGRAM�ł́A�C�ӂ̓��͓d���Œ����\�Ƃ���												*/
		VdcBase = pMainPowerChecker->conf.TestDcLevel;		/* ������d��[V]						*/
/*--------------------------------------------------------------------------------------------------*/
/*		��d���d�����ϒl�̎擾����																	*/
/*--------------------------------------------------------------------------------------------------*/
		VdcTest = TciDetectTestVdc( 100, pMainPowerChecker, pDetectDc );
		if( VdcTest < 0 ){ ErrFlag = TRUE;}					/* �ُ�`�F�b�N							*/
	/*----------------------------------------------------------------------------------------------*/
		for( VdcMean=0,i=0; i<4096; i++ )					/* ��d���d���擾����(4096��)			*/
		{
			if( ErrFlag == TRUE ){ break;}					/* �ُ펞�͏I��							*/
			VdcTest = TciDetectTestVdc( 1, pMainPowerChecker, pDetectDc );
			if( VdcTest > 0 )								/* ��d���d���擾����					*/
			{
				VdcMean += VdcTest;							/* ��d���d�����Z						*/
			}
			else											/* ��d���d���擾�ُ�					*/
			{
				ErrFlag = TRUE;								/* ���ϒl�擾�ُ�						*/
				break;										/* �ُ펞�͏I��							*/
			}
		}
		VdcMean = (((VdcMean>>11)+1)>>1);					/* ���ϒl�쐬							*/

/*--------------------------------------------------------------------------------------------------*/
/*		��d���d���������x���̌v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (ErrFlag == FALSE) && (VdcMean > 0) )			/* ���ϒl�擾����						*/
		{
			VdcAdj = ((VdcBase - VdcMean)<<8) / VdcMean;	/* �������x���v�Z						*/
			if( MlibABS( VdcAdj ) > VDCADJ_ERRLVL )			/* �������x���㉺���`�F�b�N				*/
			{
				VdcAdj = 0;									/* �������x�����[��						*/
				rslt = ADJ_FAILURE;
			}
		}
		else												/* ���ϒl�擾���s						*/
		{
			VdcAdj = 0;										/* �������x�����[��						*/
			rslt = ADJ_FAILURE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		�������x���̊i�[																			*/
/*--------------------------------------------------------------------------------------------------*/
		/* �p�����[�^�v�Z�֐���call���Ē������ʂ��i�[												*/
		VdcAdj = (VdcAdj & 0x00FF);							/* Make Byte Data						*/
		/* PnE5B ����:P-N�d�����o���x������, ���:P-N�d�����o���x���[����							*/
		TunePrmWriteRam( &AccessPrm, &pndef_pnadj, VdcAdj, 2 );

		return rslt;
}


/****************************************************************************************************/
/*																									*/
/*		�����@�p�c�b�d���d�����o����																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �u�W�T�O�̂o�v�l�o�͂��o�g�b�≏���Ăq�b�t�B���^��ʂ��ăA�i���O�d���Ƃ��A				*/
/*			���H���o���R���p���[�^��d���Ƃ���B												*/
/*																									*/
/*	�� �� : ��d���d���Ƃc�b�d�����R���p���[�^�Ŕ�r���A���ʂ��o�g�b�≏���āA					*/
/*			�u�W�T�O�̓��͐M���Ƃ��āA�u�W�T�O�ɓ`����B											*/
/*																									*/
/*			1) �R���p���[�^�o�͂��k���Ȃ�΁A��d���d���̕����c�b�d�����Ⴂ�B					*/
/*			   ���̏ꍇ�A�o�v�l�o�̓f�[�^��+1����B													*/
/*			2) �R���p���[�^�o�͂��g���Ȃ�΁A��d���d���̕����c�b�d����荂���B					*/
/*			   ���̏ꍇ�A�o�v�l�o�̓f�[�^��-1����B													*/
/*																									*/
/*			����ɂ��A��d���d���͏�ɂc�b�d���l�ɑ΂��ā}�P�̌덷�ŒǏ]����B					*/
/*			�o�v�l�f�[�^���c�b�d���Ƃ���B															*/
/*			�Ȃ��A�f�[�^���ő�l�ɂȂ����猟�o�ُ�Ƃ���B											*/
/*																									*/
/*			�o�v�l�f�[�^ : �Q�T�T�^�Q�Q�T�u�cAC100V�d�l�̏ꍇ										*/
/*			�o�v�l�f�[�^ : �Q�T�T�^�S�T�O�u�cAC200V�d�l�̏ꍇ										*/
/*			�o�v�l�f�[�^ : �Q�T�T�^�X�O�O�u�cAC400V�d�l�̏ꍇ										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG	TciDetectTestVdc( LONG RepNum, CHECK_MAIN_POWER *pMainPowerChecker, 
							  DET_DC_VOLT *pDetectDc )
{
	LONG		i;
	LONG		AdRead;
	LONG		VdcTest;
	
	/* Initialize */
	VdcTest = 0;
//	VdcTest	=	(LONG)(pMainPowerChecker->conf.TestDcLevel);

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���d�����o����																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pDetectDc->conf.VdetMode )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		�o�v�l�|�R���p���[�^����															*/
		/*------------------------------------------------------------------------------------------*/
			/*--------------------------------------------------------------------------------------*/
			/*  �Œ�}2v/250us(���e��),�ρ}16v/250(���e��)�̏ꍇ�����邪�����ł͒Ǐ]���Ԃ�		*/
			/*	�֌W�Ȃ����ߌŒ�Ǐ]������															*/
			/*--------------------------------------------------------------------------------------*/
			case VDET_PWM_FIX_FOLLOW:
			case VDET_PWM_VAR_FOLLOW:
#if 0														/* ���Ή��ł��邽�߁A�폜				*/
				/*----------------------------------------------------------------------------------*/
				/*	PWM Dummy Output																*/
				/*----------------------------------------------------------------------------------*/
				KPX_START_VDCPWM( );						/* �d���d�����o�pPWM�J�n				*/
				KpiResetWatchdogWaitus( 125 );				/* Reset Watchdog and Wait				*/
				/*----------------------------------------------------------------------------------*/
				/*	���ۂ̑���p PWM Output															*/
				/*----------------------------------------------------------------------------------*/
				for( i=0; i < RepNum; i++ )
				{
					KPX_OUT_VDCPWM( (USHORT)VdcTest );		/* PWM�o�͐ݒ�							*/
					KpiResetWatchdogWaitus( 125 );			/* Reset Watchdog and Wait				*/
				/*----------------------------------------------------------------------------------*/
					if( KPX_CHK_VDCLOW( ) )					/* Low�̏ꍇ�́APWM�o�͂�DC�d�����Ⴂ	*/
					{
						if( VdcTest < Bprm.VdetMax )		/* ����m�F								*/
						{
							++VdcTest;
						}
						else
						{
							return( -1 );					/* Max�܂ŏオ��ُ͈̂�				*/
						}
					}
				/*----------------------------------------------------------------------------------*/
					else									/* Hi�̏ꍇ�́APWM�o�͂�DC�d����荂��	*/
					{
						if( VdcTest > 0 )					/* �����m�F								*/
						{
							--VdcTest;
						}
						else
						{
							return( -1 );					/* 0(=0V)�܂ŉ�����ُ͈̂�				*/
						}
					}
				}
#endif														/* ���Ή��ł��邽�߁A�폜				*/
				VdcTest = -1;								/* �����ɗ�����ُ퉞���Ƃ���			*/
				break;
		/*------------------------------------------------------------------------------------------*/
		/*	�`�^�c�ϊ�����																			*/
		/*------------------------------------------------------------------------------------------*/
			case VDET_AD:
			/*--------------------------------------------------------------------------------------*/
			/*		�c�b�d���`�c�Ǎ���																*/
			/*--------------------------------------------------------------------------------------*/
				for( i=0; i < RepNum; i++ )
				{
					AdRead = HALdrv_GetDcVoltFromAd( );				/* �c�b�d���`�c�Ǎ���			*/
#if (FLOAT_USE==TRUE)
					VdcTest = (SHORT)(( AdRead * pDetectDc->conf.VdetAdNorm ));
#else
					VdcTest = (SHORT)(MlibMulgain( AdRead, pDetectDc->conf.VdetAdNorm ));
#endif /* FLOAT_USE */

					if( VdcTest >= pDetectDc->conf.VdetMaxVolt )
					{
						return( -1 );								/* Max�l�܂ŏオ��ُ͈̂�		*/
					}
				}
				break;
		/*------------------------------------------------------------------------------------------*/
		/*		���o�Ȃ�																			*/
		/*------------------------------------------------------------------------------------------*/
			default:
				VdcTest = -1;										/* ���o�Ȃ��ُ͈�				*/
				break;
		}

		return( VdcTest );
}

