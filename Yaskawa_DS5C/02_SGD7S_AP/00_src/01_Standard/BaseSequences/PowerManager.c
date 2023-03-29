/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PowerManager.c : �T�[�{�d���Ǘ�������`														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{�h���C�u�̓d���̊Ǘ����s���B														*/
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
/*	Note	:	����	2010.04.19	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "BaseCmnStruct.h"
#include "PowerManager.h"
#include "AlarmManager.h"
#include "MLib.h"
#include "KLib.h"
#include "HwDeviceIf.h"

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	CpxPowerManager( POW_MNG_HNDL *hPowerManager );											*/
/*	void	LpxRegeneControl( REGENE_CTRL *pRegCtrl, POWER_STATUS *PowSts, BOOL MotStop );			*/
/*	LONG	LpxDetectDcVolt( DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts, CHAR AdAdjust );			*/
/*	LONG	LpxCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc,			*/
/*																		POWER_STATUS *PowSts );		*/
/*	LONG	LpxCheckRegeneOverLoad( REGENE_CTRL *pRegCtrl, ULONG *AlmOLInfo,						*/
/*																	ANLG_UN_MON_DATA *MonData );	*/
/*	void	SetRegeneOverLoadSts( LONG RegOlSts );													*/
/*	BOOL	LpxCheckRushRxOverLoad( CHECK_RUSH *pRush, POWER_STATUS *pPowerSts );					*/
/*	BOOL	LpxCheckRegeneAlarm( REGENE_CTRL *pRegCtrl, BOOL PowerOn,								*/
/*											ASIC_HW_IF_STR *pAsicHwReg, UNI_PRM_STR *uni_prm_ptr );	*/
/*	void	PcmInitPowerManager( POW_MNG_HNDL *hPowerManager, ULONG AlmOLInfo );					*/
/*	void	PcmCountRegeneOnTime( POW_MNG_HNDL *hPowerManager );									*/
/*	void	PcmInputPowerStatus( POW_MNG_HNDL *hPowerManager, BOOL Acon1, BOOL RegeneAlm );			*/
/*	void	IprmcalRegenePowerGain( CHECK_REGOL *RegOL,												*/
/*													CMN_PRMDATA *cmnprm_ptr, BPRMDAT *bprm_ptr );	*/
/*	void	PcmCalculateVdetAdFil( DET_DC_VOLT *pDetectDc, USHORT vdetadfil, LONG SvCycleUs );		*/
/****************************************************************************************************/
static	void	LpxAcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
//static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, BOOL MotStopSts );		/* <S21D> */
static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, 							/* <S21D> */
												BOOL MotStop, REGENE_CTRL *RegeneCtrl );
//static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
//																			REGENE_CTRL *RegCtrl );		/* <S0F6> */
static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
																	REGENE_CTRL *RegCtrl, BOOL BeSts );	/* <S0F6> */
static	void	LpxCheckAcOpenPhase( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
#if( CSW_HW_DCBUS == FALSE )
static	void	LpxDetectAcOpenACON1( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
static	void	LpxDetectAcOpenACON12( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
#endif	/* ( CSW_HW_DCBUS == FALSE ) */
static	void	LpxCheckDcUnderVoltage( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts );
static	void	LpxAdDetectDcVolt( DET_DC_VOLT *pDetectDc );
static	void	LpxCheckPcovAlarm( PCOV_ERR_DETECT *PcovErrDetect, POWER_STATUS *PowSts );	/* <S053> */
static	BOOL	DischargeSequence( CHECK_MAIN_POWER *PowerChecker, LONG DcVolt ); /* <S1C6> */



/****************************************************************************************************/
/*																									*/
/*		�T�[�{�d���Ǘ����C��																		*/
/*																									*/
/****************************************************************************************************/
//void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts )								  /* <S0F6> */
void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts, BOOL BeSts )					  /* <S0F6> */
{
	/* �`�b�d���Ď����� */
	LpxAcPowerManager( &(hPowerManager->MainPowerChecker),
					   &(hPowerManager->PowerSts) );

	/* �c�b�d���Ď����� */
	LpxDcPowerManager( &(hPowerManager->MainPowerChecker),
					   &(hPowerManager->PowerSts),
					   MotStopSts,
					   &(hPowerManager->RegeneCtrl) );	/* <S21D> */

	/* �c�b�d���}�����d���� */
	LpxDischargeDcPower( &(hPowerManager->MainPowerChecker),
						 &(hPowerManager->PowerSts),
						 &(hPowerManager->RegeneCtrl),
						 BeSts );																	  /* <S0F6> */

	/* �`�b�d���������o���� */
	LpxCheckAcOpenPhase( &(hPowerManager->MainPowerChecker),
						 &(hPowerManager->PowerSts) );

	/* �c�b�d���s���d���Ď����� */
	LpxCheckDcUnderVoltage( &(hPowerManager->MainPowerChecker),
							&(hPowerManager->PowerSts) );

	/* ���ԓd�ʈُ�`�F�b�N���� <S053> */
	LpxCheckPcovAlarm( &(hPowerManager->PcovErrDetect),
					   &(hPowerManager->PowerSts) );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�`�b�d���Ď�����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�� �I :	(1) ���S�K�i����̗v���ŁA�`�b���͂��I�t�����ꍇ�A										*/
/*				�c�b�d������d���Ĉ��d���ȉ��܂ŉ����Ȃ���΂Ȃ�Ȃ��B							*/
/*			(2) �T�[�{�I���M�����A�N�e�B�u�ɌŒ肵�āA�`�b�d���̃I���^�I�t��						*/
/*				���[�^�ʓd�I���^�I�t���R���g���[�����郆�[�U�[������B								*/
/*				�`�b�d�����o���s��Ȃ��Ɠd�����؂�Ă����[�^�ʓd���p�����댯�ł���B				*/
/*																									*/
/*																									*/
/*	�@ �\ : �`�b���͓d���I���^�I�t�̌��o���s���B													*/
/*																									*/
/*  		acon���o�́A�`�b���C���̓d����Ԃ��t�H�g�J�v���ɂ��Ď�����B							*/
/*			aconin�̓t�H�g�J�v���o�͂��\�t�g�t�B���^�i125us�Q��ǂݑ������j�������̂ł���B			*/
/*			�t�H�g�J�v���͒P�ɐ��ł��邽�߁A�d�������̔��g����aconin�����o�ł��Ȃ��B				*/
/*			���̂��߁Aaconin�͓d���I����ԂŁA�d�������ŃI���^�I�t���J��Ԃ��B						*/
/*			aconin���I�t���Ă����Ȃ��Ƃ��d�����������͓d���I�t(acon1==FALSE)�ƌ��Ȃ��Ȃ��B			*/
/*																									*/
/*			�܂��A�u��ێ��ƌ����@�\������A���[�U�[�萔�Őݒ肵�����Ԃ͓d�����I�t���Ă�			*/
/*			����𑱂���d�l�ł���B																*/
/*			����ɂ��A�d���I�t���o�́Aaconin���I�t���Ă���d���������{�u��ێ����ԕ��҂B		*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxAcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
#if( CSW_HW_DCBUS == TRUE )
/* Mask Function */
		return;
#endif
/*<S201> Start */
/*--------------------------------------------------------------------------------------------------*/
/* �`�b�d���n�m���o�̕s���ђ��̃t���O�쐬      */ 
/* Initial Acon Wait Timer< KPI_SCANC_MS( 4 )*/
/*--------------------------------------------------------------------------------------------------*/
 if(PowerChecker->var.AconWaitTimer < KPI_SCANC_MS( 4 ))
 {
   	PowSts->AconDetectReady=FALSE;
 }
 else
 {
   	PowSts->AconDetectReady=TRUE;
 }
/*<S201> End*/
/*--------------------------------------------------------------------------------------------------*/
/*		�d������������̂�4ms�҂� ACON�M���΍�														*/
/*--------------------------------------------------------------------------------------------------*/
	if( PowerChecker->var.AconWaitTimer < KPI_SCANC_MS( 4 ) )
	{
		PowerChecker->var.AconWaitTimer++;
		PowSts->Acon = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�`�b�d���n�m���o����																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0		/* <S21D> */
	else if( (PowSts->Acon1Sig != FALSE) ||
			 ((PowerChecker->conf.DetectAcOn2 != FALSE) && (PowSts->Acon2Sig != FALSE)) )
#else		/* <S21D> */
	else if( ( (PowerChecker->conf.Dcbus == FALSE)
			&& ( (PowSts->Acon1Sig != FALSE)
			  || ( (PowerChecker->conf.DetectAcOn2 != FALSE) && (PowSts->Acon2Sig != FALSE) ) ) )
		  || ( (PowerChecker->conf.Dcbus == TRUE)
			&& (PowSts->ConvRdySig == TRUE) ) )
#endif

	{
		PowSts->Acon = TRUE;									/* AC Power On						*/
		PowerChecker->var.AcoffChkTimer = 0;					/* Reset Power Off Check Timer		*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{
		/* <S21D> Start */
		/* �u��ێ����� */
#if 0
		/* 2011.04.04 Y.Oka <V762> */
		if( PowerChecker->conf.Dcbus )
		{
			PowSts->Acon = FALSE;								/* AC Power Off						*/
		}
//		if( PowerChecker->var.AcoffChkTimer < PowerChecker->conf.AcoffDetctTime )
		/* 2011.04.04 Y.Oka <V762> */
		else if( PowerChecker->var.AcoffChkTimer < PowerChecker->conf.AcoffDetctTime )
#else
		if( PowerChecker->var.AcoffChkTimer < PowerChecker->conf.AcoffDetctTime )
#endif
		/* <S21D> End */

		{
			PowerChecker->var.AcoffChkTimer += KPI_SCCYCLEMS;
			if( PowerChecker->var.AcoffChkTimer > 30 )
			{
				PowSts->PowerOnEep = FALSE;						/* Power Off for EEPROM				*/
			}
		}
		else
		{
			PowSts->Acon = FALSE;								/* AC Power Off						*/
			PowSts->PowerOnEep = FALSE;							/* Power Off for EEPROM				*/
		}
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�c�b�d���Ď�����																			*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���͓d��������̂c�b�d���[�d��Ԃ��Ď����A�^�]���������t���O�𗧂Ă�B					*/
/*			�[�d������́A�[�d�����t���O���I�t���A�˓��d��������R��Z������w��������B			*/
/*			�e�ʂɂ��A�˓��d��������R�̒Z������͂c�a��H�I�t�̓���Ɖ�H�����ʉ�����			*/
/*			������̂�����B�˓��d��������R�̒Z������͐�p�̏������W���[���ɂčs���B				*/
/*			�[�d�Ɏ��s�����ꍇ�A�[�d���s�A���[���Ƃ���B�]���s���d���Ɠ��A���[���R�[�h��			*/
/*			���������A�A���[���R�[�h�̌n�̌������ɂ���ʂ���B									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, BOOL MotStop )		/* <S21D> */
static	void	LpxDcPowerManager( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts, 						/* <S21D> */
												BOOL MotStop, REGENE_CTRL *RegeneCtrl )

{
/*--------------------------------------------------------------------------------------------------*/
/*		�e�X�g���[�h																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( PowerChecker->conf.TestMode1 == TRUE )
	{
		PowSts->PowerOn = TRUE;										/* Power On for Control			*/
		PowSts->PowerOnEep = FALSE;									/* Power On for EEPROM			*/
		PowSts->ChargeOk = TRUE;									/* Charge OK for rush			*/
	}
#if( CSW_HW_DCBUS == FALSE )
	else if( PowerChecker->conf.Dcbus == FALSE )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		AC�d�����̓��[�h																		*/
	/*----------------------------------------------------------------------------------------------*/
//		if( PowSts->Acon )				/* <S21D> */
		if( PowSts->Acon == TRUE )		/* <S21D> */
		{
			//if( PowerChecker->var.DconChkTimer >= POWREADY_WAIT )
			if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerReadyWaitTime ) /* <S053>  */
			{ /* �p���[���f�B�҂����Ԍo�� */
				PowSts->PowerOn = TRUE;								/* Power On for Control			*/
				PowSts->PowerOnEep = TRUE;							/* Power On for EEPROM			*/
			}
			//else if( PowerChecker->var.DconChkTimer >= POWCHARG_WAIT )
			else if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
			{
				if( PowSts->DcVolt >= PowerChecker->conf.UvLevel )	/* DC�d���`�F�b�N				*/
				{
					/*------------------------------------------------------------------------------*/
					/* DB������P�F��]���̎��HOFF �� ON�΍�										*/
					/*------------------------------------------------------------------------------*/
					switch( PowerChecker->conf.RlyDbWay )
					{
					case DBCTRL_TRIACK_SEPARATE:		/* �˓��F�����[�GDB:�T�C���X�^ 				*/
					case DBCTRL_RLY_SEPARATE:			/* �˓��F�����[�GDB�F�����[ 				*/
					case DBCTRL_TRIACK_COMMON:			/* �˓�+DB:�����[+�T�C���X�^ 				*/
						PowSts->ChargeOk = TRUE;		/* ���[�^��]���Ɋւ�炸					*/
						break;

					case DBCTRL_RLY_COMMON:				/* �˓��{�c�a�F�����[���� 					*/
					default:							/* ���肦�Ȃ��ݒ莞 �����[���ʂƓ����ɂ���	*/
						//if(PowerChecker->var.DconChkTimer == POWCHARG_WAIT)
						if(PowerChecker->var.DconChkTimer == PowerChecker->conf.PowerChargeWaitTime )
						{
							if( MotStop == TRUE )
							{ /* ���[�^��~�� */
								/* ��~�Ń`���[�W���� */
								PowSts->ChargeOk = TRUE;
							}
							else
							{ /* ���[�^��]�� */
								/* ��]���͗��܂� */
								//PowerChecker->var.DconChkTimer = POWCHARG_WAIT - 1;
								PowerChecker->var.DconChkTimer = PowerChecker->conf.PowerChargeWaitTime - 1;	/* <S053> */
							}
						}
						break;
					}
					PowerChecker->var.DconChkTimer++;
				}
				else
				{
					ALMSetGlobalAlarm( ALM_PUV );				/* A.410 : �s���d��					*/
					PowSts->PowerOn = FALSE;
					PowSts->PowerOnEep = FALSE;
					PowSts->ChargeOk = FALSE;
					PowerChecker->var.DconChkTimer = 0;
				}
			}
			else
			{
				PowerChecker->var.DconChkTimer++;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		�`�b�d���n�e�e																			*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			PowSts->PowerOn  = FALSE;
			PowSts->PowerOnEep = FALSE;
			PowSts->ChargeOk = FALSE;
			PowerChecker->var.DconChkTimer = 0;
		}
	}
#endif
	else
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		DC�d�����̓��[�h																		*/
	/*----------------------------------------------------------------------------------------------*/
#if 0	/* <S21D> */
		if( PowSts->DcVolt >= PowerChecker->conf.UvLevel )			/* DC�d���`�F�b�N				*/
		{
			//if( PowerChecker->var.DconChkTimer >= POWREADY_WAIT )
			if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerReadyWaitTime ) /* <S053>  */
			{ /* �p���[���f�B�҂����Ԍo�� */
				PowSts->PowerOn = TRUE;								/* Power On for Control			*/
				PowSts->PowerOnEep = TRUE;							/* Power On for EEPROM			*/
			}
			else
			{
				//if( PowerChecker->var.DconChkTimer >= POWCHARG_WAIT )
				if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
				{
					/*------------------------------------------------------------------------------*/
					/* DB������P�F��]���̎��HOFF �� ON�΍�										*/
					/*------------------------------------------------------------------------------*/
					switch( PowerChecker->conf.RlyDbWay )
					{
					case DBCTRL_RLY_COMMON:				/* �˓��{�c�a�F�����[���� 					*/
					default:							/* ���肦�Ȃ��ݒ莞 �����[���ʂƓ����ɂ���	*/
						//if(PowerChecker->var.DconChkTimer == POWCHARG_WAIT)
						if(PowerChecker->var.DconChkTimer == PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
						{
							if( MotStop == TRUE )
							{ /* ���[�^��~�� */
								/* ��~�Ń`���[�W���� */
								PowSts->ChargeOk = TRUE;
							}
							else
							{ /* ���[�^��]�� */
								/* ��]���͗��܂� */
								//PowerChecker->var.DconChkTimer = POWCHARG_WAIT - 1;
								PowerChecker->var.DconChkTimer = PowerChecker->conf.PowerChargeWaitTime - 1; /* <S053> */
							}
						}
						break;
					case DBCTRL_TRIACK_SEPARATE:		/* �˓��F�����[�GDB:�T�C���X�^ 				*/
					case DBCTRL_RLY_SEPARATE:			/* �˓��F�����[�GDB�F�����[ 				*/
					case DBCTRL_TRIACK_COMMON:			/* �˓�+DB:�����[+�T�C���X�^ 				*/
						PowSts->ChargeOk = TRUE;		/* ���Ԃœ˓���H�̑�����s��				*/
						break;
					}
				}
				PowerChecker->var.DconChkTimer++;
			}
		}
		else
		{
			PowSts->PowerOn = FALSE;
			PowSts->PowerOnEep = FALSE;
			PowSts->ChargeOk = FALSE;
			PowerChecker->var.DconChkTimer = 0;
		}
	}
#else	/* <S21D> */
		if( PowSts->Acon == TRUE )
		{
			if( PowSts->DcVolt >= (LONG)PowerChecker->conf.UvLevel )		/* DC�d���`�F�b�N				*/
			{
				//if( PowerChecker->var.DconChkTimer >= POWREADY_WAIT )
				if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerReadyWaitTime ) /* <S053>  */
				{ /* �p���[���f�B�҂����Ԍo�� */
					PowSts->PowerOn = TRUE;								/* Power On for Control			*/
					PowSts->PowerOnEep = TRUE;							/* Power On for EEPROM			*/
				}
				else
				{
					//if( PowerChecker->var.DconChkTimer >= POWCHARG_WAIT )
					if( PowerChecker->var.DconChkTimer >= PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
					{
						/*------------------------------------------------------------------------------*/
						/* DB������P�F��]���̎��HOFF �� ON�΍�										*/
						/*------------------------------------------------------------------------------*/
						switch( PowerChecker->conf.RlyDbWay )
						{
						case DBCTRL_RLY_COMMON:				/* �˓��{�c�a�F�����[���� 					*/
						default:							/* ���肦�Ȃ��ݒ莞 �����[���ʂƓ����ɂ���	*/
							//if(PowerChecker->var.DconChkTimer == POWCHARG_WAIT)
							if(PowerChecker->var.DconChkTimer == PowerChecker->conf.PowerChargeWaitTime ) /* <S053> */
							{
								if( MotStop == TRUE )
								{ /* ���[�^��~�� */
									/* ��~�Ń`���[�W���� */
									PowSts->ChargeOk = TRUE;
								}
								else
								{ /* ���[�^��]�� */
									/* ��]���͗��܂� */
									//PowerChecker->var.DconChkTimer = POWCHARG_WAIT - 1;
									PowerChecker->var.DconChkTimer = PowerChecker->conf.PowerChargeWaitTime - 1; /* <S053> */
								}
							}
							break;
						case DBCTRL_TRIACK_SEPARATE:		/* �˓��F�����[�GDB:�T�C���X�^ 				*/
						case DBCTRL_RLY_SEPARATE:			/* �˓��F�����[�GDB�F�����[ 				*/
						case DBCTRL_TRIACK_COMMON:			/* �˓�+DB:�����[+�T�C���X�^ 				*/
							PowSts->ChargeOk = TRUE;		/* ���Ԃœ˓���H�̑�����s��				*/
							break;
						}
					}
					PowerChecker->var.DconChkTimer++;
				}
			}
			else
			{
				if( PowerChecker->var.DconChkTimer >= (ULONG)PowerChecker->conf.PowerChargeWaitTime )
				{ /* �c�b�[�d�����҂����Ԍo�� */
					ALMSetGlobalAlarm( ALM_PUV );					/* A.410 : �s���d��				*/
					PowSts->PowerOn = FALSE;
					PowSts->PowerOnEep = FALSE;
					PowerChecker->var.DconChkTimer = 0U;
				}
				PowerChecker->var.DconChkTimer++;

			}
		}
		else
		{
			PowSts->PowerOn = FALSE;
			PowSts->PowerOnEep = FALSE;
			PowerChecker->var.DconChkTimer = 0U;
		}
		PowSts->ChargeOk = PowSts->ConvMconSig;
#endif
	}
	/* <S21D> Start */
	/* �R���f���T�o���N����	*/
	PowerChecker->var.ClinkOut = PowSts->ChargeOk;
	if( PowerChecker->conf.CapaLinkFlag == TRUE )			/* �R���f���T�o���N�ڑ��t���O == TRUE		*/
	{
		if( PowerChecker->var.ClinkOut == TRUE )				/* �R���f���T�o���N�ڑ��o�͐M�� == ON	*/
		{
			if( PowSts->ClinkAnsSig == FALSE )					/* /CLINKANS��500ms�ȏ�ω����Ȃ��ꍇ	*/
			{
				if( PowerChecker->var.ClinkAnsErrChkTimer >= (ULONG)CLINKANSERRCHK_TIME )
				{
					ALMSetGlobalAlarm( ALM_CAPALINK );			/* A.332 : �R���f���T�o���N�ڑ��A���T�[�ُ�	*/
				}
				else
				{
					PowerChecker->var.ClinkAnsErrChkTimer++;
				}
			}
			else
			{
				PowerChecker->var.ClinkAnsErrChkTimer = 0U;
			}
		}
		else
		{
			PowerChecker->var.ClinkAnsErrChkTimer = 0U;
		}

		if( PowSts->ClinkAnsSig == TRUE )					/* �R���f���T�o���N�ڑ��Ď��M���i/CLINKANS�j== ON	*/
		{
			if( PowerChecker->var.ClinkAnsTimer >= (ULONG)CMCON_WAIT )
			{
				PowerChecker->var.CmconOut = TRUE;
				if( PowSts->CmcAnsSig == FALSE )			/* �˖h�����[��ԊĎ��M���i/CMCANS�j��/CMCON�ƈقȂ�ꍇ	*/
				{
					if( PowerChecker->var.CmcAnsErrChkTimer >= (ULONG)CMCANSERRCHK_TIME )
					{
						ALMSetGlobalAlarm( ALM_CAPAMC );	/* A.333 : �R���f���T�o���NMC�A���T�[�ُ�	*/
					}
					else
					{
						PowerChecker->var.CmcAnsErrChkTimer++;
					}
					PowerChecker->var.CapaChargeOk = FALSE;
				}
				else
				{
					PowerChecker->var.CmcAnsErrChkTimer = 0U;
					PowerChecker->var.CapaChargeOk = TRUE;
				}
			}
			else
			{
				PowerChecker->var.CmconOut = FALSE;
				PowerChecker->var.CmcAnsErrChkTimer = 0U;
				PowerChecker->var.ClinkAnsTimer++;
			}
		}
		else
		{
			PowerChecker->var.CmconOut = FALSE;
			PowerChecker->var.CmcAnsErrChkTimer = 0U;
			PowerChecker->var.ClinkAnsTimer = 0U;
		}
	}
	else
	{
		PowerChecker->var.CapaChargeOk = TRUE;
	}
	/* <S21D> End */
}



/****************************************************************************************************/
/*																									*/
/*		�c�b�d���}�����d����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ��d���I�t���ADC���C���ɍ��d�����c��Ȃ��悤�ɁA�񐶏����@�\�𗬗p����					*/
/*			�}�����d�������s���B																	*/
/*																									*/
/*			1) 400W�ȉ��ł́A�W���ł͉񐶒�R��������Ă��Ȃ��̂ŁA�{�@�\�͎��ۂɂ͓��삵�Ȃ��B		*/
/*			2) ����d���������A�ʏ폈���ɓ�����10ms�ԋ}�����d���������Ȃ��B							*/
/*			   ����́AACON�𔼔g�Ō��Ă��邽�߁A����E���C�������������ꂽ�ꍇ�A�d��������			*/
/*			   ACON�����o�ł����ɁA�d���������Ă���̂ɕ��d���邱�Ƃ�h�����߂ł���B				*/
/*			3) DC���͎d�l������A���[�U�[�萔�Őݒ肷��悤�ɂȂ��Ă���B							*/
/*			   AC���͎d�l�̂܂܂ŁADC���͂��ꂽ��A�t�̐ݒ������āA�A���v���đ���������			*/
/*			   ������B																				*/
/*			4) �Ԉ�����ݒ�̏đ��΍�Ƃ��āAACON�M����DC�d����Ԃ��A�}�����d��					*/
/*			   �C���^���b�N��݂���B																*/
/*			   ACON==FALSE�ŁA��莞�ԋ}�����d�������Ă��ADC�d����������Ȃ��ꍇ�A					*/
/*			   DC���͂���Ă���Ƃ݂Ȃ��A�z���A���[��A.33�𔭐����āA�}�����d�������~�߂�B			*/
/*			   �A���A400W�ȉ��͉񐶒�R��������Ă��Ȃ��̂ő������d�ł��Ȃ��̂ŁAA.33�����Ȃ��B		*/
/*			5) DC���͎d�l�̐ݒ�Ȃ̂�AC���͂��ꂽ�ꍇ��A.33�A���[���Ƃ���B							*/
/*			   DC���͎d�l�ł́A�񐶓�����~�߂�̂ŁAAC���͂����ƃ_�C�I�[�h�u���b�W��				*/
/*			   ���񐶃G�l���M�[�̓����ꂪ�����ߓd���ɂȂ�B�����h�����߂ł���B					*/
/*			   DC���͎d�l�̎��́A�{�C�|���͒[�q����DC���͂��Ȃ��Ƃ����Ȃ��B							*/
/*			   DC���͎d�l�ŉ񐶓�����~�߂�̂́ADC���͎d�l�ł͑�����DC�o�X�łȂ��ꍇ				*/
/*			   ����ʓI�ŁA�񐶏������s���ƌ��o���x���̒Ⴂ1���ɉ񐶏������W�����A�񐶏�����		*/
/*			   �H���̏Ⴗ��댯�����邩��ł���B													*/
/*			   DC���͎d�l�ŉ񐶏����@�\���~�����ꍇ��Y�d�l�őΉ�����B								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
//																			REGENE_CTRL *RegCtrl )
static	void	LpxDischargeDcPower( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts,
																REGENE_CTRL *RegCtrl, BOOL BeSts )
{
#if( CSW_HW_DCBUS == TRUE )
/* Mask Function */
	return;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		DC�d�����̓��[�h || �e�X�g���[�h1 : �}�����d���������Ȃ�									*/
/*--------------------------------------------------------------------------------------------------*/
	if( (PowerChecker->conf.Dcbus == TRUE) || (PowerChecker->conf.TestMode1 == TRUE) )
	{
#if 0	/* <S21D> */
		PowSts->Discharge = FALSE;
		PowerChecker->var.DischChkTimer = KPI_SCANC_MS( 500 );
#else	/* <S21D> */
		if( (PowerChecker->conf.Dcbus == TRUE) && (PowerChecker->conf.DcDischarge == TRUE) )
		{
			if( PowSts->Discharge == TRUE )
			{
				if( PowerChecker->var.ConvInSigDischErrChkTimer >= (ULONG)CONVINSIGDISCHERRCHK_TIME )
				{
					if( (PowerChecker->var.DcVoltOld - PowSts->DcVolt) < DCVOLT10V )
					{
						ALMSetGlobalAlarm( ALM_CONVIO );			/* A.331 : �d���֘A���͐M���ُ� */
					}
				}
				else
				{
					PowerChecker->var.ConvInSigDischErrChkTimer++;
				}
			}
			else
			{
				PowerChecker->var.ConvInSigDischErrChkTimer = 0U;
				PowerChecker->var.DcVoltOld = PowSts->DcVolt;
			}

			if( PowSts->Acon == TRUE )
			{ /* �R���o�[�^�������� */
				PowSts->Discharge = FALSE;
				PowerChecker->var.DischChkTimer = (ULONG)KPI_SCANC_MS( 500 );
			}
			else if( PowerChecker->var.DischChkTimer >= (ULONG)KPI_SCANC_MS(500) )
			{
				PowSts->Discharge = TRUE;								/* �}�����dON				*/
			}
			else
			{ /* do nothing */
				;
			}
		}
		else
		{
			PowSts->Discharge = FALSE;
			PowerChecker->var.DischChkTimer = (ULONG)KPI_SCANC_MS( 500 );
		}
#endif

/* 2011.04.04 Y.Oka <V762> */
#if 0
		if( (PowerChecker->conf.Dcbus == TRUE) && (PowSts->Acon == TRUE) )
		{
			ALMSetGlobalAlarm( ALM_WIR );					/* A.330 : ���H�z���G���[				*/
		}
#endif
		if( (PowerChecker->conf.Dcbus) && (PowSts->DcVolt > PowerChecker->conf.UvLevel) )
		{ /* DC���͑I�� && DC�d����UV���x���𒴂��Ă��� */
			if( (!PowSts->PowerOn) && (PowSts->Acon) )
			{ /* PowerOn=TRUE�܂ł�240ms�Ԃ�ACON���o */
				/* ACON�J�E���^�C���N�������g */
				PowerChecker->var.AconDetectCount++;
				if( PowerChecker->var.AconDetectCount >= KPI_SCANC_MS(10) )
				{ /* ACON���o��10ms�o�� */
					/* A.330 : ���H�z���G���[ */
					ALMSetGlobalAlarm( ALM_WIR );
					/* A.330�̌��o�����b�` */
					PowerChecker->var.Alm330Lt = TRUE;
					/* ACON�J�E���^ = 10ms */
					PowerChecker->var.AconDetectCount = KPI_SCANC_MS(10);
				}
			}
			else if( PowerChecker->var.Alm330Lt )
			{ /* �O�X�L�����ɂ�A.330���o�����b�` */
				/* A.330 : ���H�z���G���[ */
				ALMSetGlobalAlarm( ALM_WIR );
			}
			else
			{ /* DC�d������(����) */
				PowerChecker->var.Alm330Lt = FALSE;
				PowerChecker->var.AconDetectCount = 0;
			}
		}
		else
		{ /* DC�d���l��UV���x���ȉ� || DC���͑I���łȂ��ꍇ(AC���͑I����TestMode) */
			PowerChecker->var.Alm330Lt = FALSE;
			PowerChecker->var.AconDetectCount = 0;
		}
/* 2011.04.04 Y.Oka <V762> */

	}
/*--------------------------------------------------------------------------------------------------*/
/*		AC�d�����̓��[�h : �}�����d�������s��														*/
/*--------------------------------------------------------------------------------------------------*/
//	else if( PowSts->Acon == TRUE )																	  /* <S0F6> */
	else if( (PowSts->Acon == TRUE) || (BeSts == TRUE) )											  /* <S0F6> */
	{ /* ACON�� */
		PowSts->Discharge = FALSE;
		PowerChecker->var.DischChkTimer = KPI_SCANC_MS( 500 );
		PowerChecker->var.DischSeqNo = DISCH_INIT;
	}
	else if( PowerChecker->var.DischChkTimer >= KPI_SCANC_MS(500) )
	{ /* �ʏ��ACON��������BaseBlock�Ȃ�΁A�}�����d�V�[�P���X�J�n */
//		PowSts->Discharge = TRUE;												/* �}�����dON		*//* <S1C6> */
		PowSts->Discharge = DischargeSequence( PowerChecker, PowSts->DcVolt );	/* �}�����dSequence	*//* <S1C6> */
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�`�b�d�����̓��[�h : �ȉ��̃p�X�͍ŏ������ʂ�Ȃ� 											*/
/*--------------------------------------------------------------------------------------------------*/
	else if( PowerChecker->var.DischChkTimer < KPI_SCANC_MS(10) )
	{ /* ����d��������(Acon=LO)�̂ݒʂ鏈���B10ms�Ԃ͋}�����d�����Ȃ� */
		PowerChecker->var.DischChkTimer++;
		PowSts->Discharge = FALSE;								/* �}�����dOFF						*/
	}
	else if( PowerChecker->var.DischChkTimer < KPI_SCANC_MS(310) )	/* 300ms�ԋ}�����d����			*/
	{
		PowerChecker->var.DischChkTimer++;
		PowerChecker->var.WireAlmDetWait = 0;
		PowSts->Discharge = TRUE;								/* �}�����dON						*/
	}
	else														/* 310ms���DC�d����				*/
	{															/* �������x���ɂ�������ُ�Ƃ݂Ȃ�	*/
		PowSts->Discharge = FALSE;								/* �}�����dOFF						*/
		if( (PowSts->DcVolt > DCVOLT60V)
			&& ((RegCtrl->conf.ResistorCapa != 0) || (RegCtrl->conf.RegRxIn)) )
		{ /* �d��������ACON���o�ł��Ȃ����߁A�d��������(10ms)+2ms�o�ߌ��A.330�����o����			*/
			if( PowerChecker->var.WireAlmDetWait > KPI_SCANC_MS(10) )
			{
				ALMSetGlobalAlarm( ALM_WIR );					/* A.330 : ���H�z���G���[			*/
			}
			else
			{
				PowerChecker->var.WireAlmDetWait++;
			}
		}
		else
		{
			PowerChecker->var.WireAlmDetWait = 0;
		}
	}
#if 0 /* 2010.04.19 Y.Oka ASIC�̎w�߂͎g�p���Ȃ����߃R�����g�A�E�g */
/*--------------------------------------------------------------------------------------------------*/
/*		�}�����d�o�͏���																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( CoutK.f.Discharge == TRUE )
	{
		KPI_DISCHARGEON( );									/* ASIC DisCharge ON					*/
	}
	else
	{
		KPI_DISCHARGEOFF( );								/* ASIC DisCharge OFF					*/
	}
#endif
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�}�����d�V�[�P���X����													<S1C6>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �}�����d����������s���B																*/
/*																									*/
/*			DISCH_CHK_TIME[ms]���A3��ɓn��}�����d���A���H�d�����ቺ���邩�ǂ������m�F����B	*/
/*			���H�d�����ቺ���Ȃ��ꍇ�́A�}�����d�͍s��Ȃ��B										*/
/*																									*/
/****************************************************************************************************/
static	BOOL	DischargeSequence( CHECK_MAIN_POWER *PowerChecker, LONG DcVolt )
{
	BOOL	DischReq;	/* �}�����d�v�� */
	DischReq = FALSE;

	switch	( PowerChecker->var.DischSeqNo )
	{
	case DISCH_INIT: /* �}�����d�C�j�V���� */
		PowerChecker->var.DischStartVolt = DcVolt;					/* �}�����d�m�F�J�n���d���L��	*/
		PowerChecker->var.DischSeqNo = DISCH_CHK1ST;				/* �}�����d�m�F1��ڂ�			*/
		PowerChecker->var.DischActTimer = 1;						/* �}�����d���{���ԃN���A		*/
		DischReq = TRUE;											/* �}�����d�J�n					*/
	break;

	case DISCH_CHK1ST: /* �}�����d�m�F1��� */
		if( PowerChecker->var.DischActTimer == KPI_SCANC_MS(DISCH_CHK_TIME) )
		{ /* �}�����d����DISCH_CHK_TIME(20ms)�o�� */
			/* 1Scan���}�����d���~�߂� */
			DischReq = FALSE;										/* �}�����d�ꎞ��~				*/
			PowerChecker->var.DischActTimer++;
		}
		else if( PowerChecker->var.DischActTimer > KPI_SCANC_MS(DISCH_CHK_TIME) )
		{
			if( (PowerChecker->var.DischStartVolt - DcVolt) < PowerChecker->conf.DischCheckVlt )	  /* <S1D1> */
			{ /* �d����������PowerChecker->conf.DischCheckVlt(10V)�ȉ��̏ꍇ */
				/* �}�����d�Ɉُ킠�� */
				DischReq = FALSE;									/* �}�����d��~					*/
				PowerChecker->var.DischSeqNo = DISCH_FINISH;		/* �����X�e�b�v��				*/
			}
			else
			{ /* �}�����d�ɂ��d���̌������m�F臒l�𒴂����ꍇ */
				DischReq = TRUE;									/* �}�����d�ĊJ					*/
				PowerChecker->var.DischStartVolt = DcVolt;			/* �}�����d�m�F�J�n���d���L��	*/
				PowerChecker->var.DischActTimer = 1;				/* �}�����d���{���ԃN���A		*/
				PowerChecker->var.DischSeqNo = DISCH_CHK2ND;		/* �}�����d�m�F2��ڂ�			*/
			}
		}
		else
		{
			DischReq = TRUE;										/* �m�F�p�}�����d��				*/
			PowerChecker->var.DischActTimer++;
		}
	break;

	case DISCH_CHK2ND: /* �}�����d�m�F2��� */
		if( PowerChecker->var.DischActTimer == KPI_SCANC_MS(DISCH_CHK_TIME) )
		{ /* �}�����d����DISCH_CHK_TIME(20ms)�o�� */
			/* 1Scan���}�����d���~�߂� */
			DischReq = FALSE;										/* �}�����d�ꎞ��~				*/
			PowerChecker->var.DischActTimer++;
		}
		else if( PowerChecker->var.DischActTimer > KPI_SCANC_MS(DISCH_CHK_TIME) )
		{
			if( ((PowerChecker->var.DischStartVolt - DcVolt) < PowerChecker->conf.DischCheckVlt)	  /* <S1D1> */
				&& (DcVolt > PowerChecker->conf.DischCompVlt) )
			{ /* �d����������PowerChecker->conf.DischCheckVlt(10V)�ȉ� && �c���d����DischCompVlt(50V)�ȏ� */
				/* �}�����d�Ɉُ킠�� */
				DischReq = FALSE;									/* �}�����d��~					*/
				PowerChecker->var.DischSeqNo = DISCH_FINISH;		/* �����X�e�b�v��				*/
			}
			else
			{ /* �}�����d�ɂ��d���̌������m�F臒l�𒴂��� || �c���d����DischCompVlt(50V)�ȉ�		*/
				DischReq = TRUE;									/* �}�����d�ĊJ					*/
				PowerChecker->var.DischStartVolt = DcVolt;			/* �}�����d�m�F�J�n���d���L��	*/
				PowerChecker->var.DischActTimer = 1;				/* �}�����d���{���ԃN���A		*/
				PowerChecker->var.DischSeqNo = DISCH_CHK3RD;		/* �}�����d�m�F3��ڂ�			*/
			}
		}
		else
		{
			DischReq = TRUE;										/* �m�F�p�}�����d��				*/
			PowerChecker->var.DischActTimer++;
		}
		break;

	case DISCH_CHK3RD: /* �}�����d�m�F3��� */
		if( PowerChecker->var.DischActTimer == KPI_SCANC_MS(DISCH_CHK_TIME) )
		{ /* �}�����d����DISCH_CHK_TIME(20ms)�o�� */
			/* 1Scan���}�����d���~�߂� */
			DischReq = FALSE;										/* �}�����d�ꎞ��~				*/
			PowerChecker->var.DischActTimer++;
		}
		else if( PowerChecker->var.DischActTimer > KPI_SCANC_MS(DISCH_CHK_TIME) )
		{
			if( ((PowerChecker->var.DischStartVolt - DcVolt) < PowerChecker->conf.DischCheckVlt)	  /* <S1D1> */
				&& (DcVolt > PowerChecker->conf.DischCompVlt) )
			{ /* �d����������PowerChecker->conf.DischCheckVlt(10V)�ȉ� && �c���d����DischCompVlt(50V)�ȏ� */
				/* �}�����d�Ɉُ킠�� */
				DischReq = FALSE;									/* �}�����d��~					*/
				PowerChecker->var.DischSeqNo = DISCH_FINISH;		/* �����X�e�b�v��				*/
			}
			else
			{ /* �}�����d�ɂ��d���̌������m�F臒l�𒴂��� || �c���d����DischCompVlt(50V)�ȉ�		*/
				DischReq = TRUE;									/* �}�����d�ĊJ					*/
				PowerChecker->var.DischStartVolt = DcVolt;			/* �}�����d�m�F�J�n���d���L��	*/
				PowerChecker->var.DischActTimer = 1;				/* �}�����d���{���ԃN���A		*/
				PowerChecker->var.DischSeqNo = DISCH_ACTIVE;		/* �}�����d�{�Ԃ�				*/
			}
		}
		else
		{
			DischReq = TRUE;										/* �m�F�p�}�����d��				*/
			PowerChecker->var.DischActTimer++;
		}
		break;

	case DISCH_ACTIVE: /* �}�����d */
		if( PowerChecker->var.DischActTimer >= PowerChecker->conf.MainDischActTime )
		{ /* ���C���}�����d����(�[�d���ԂƓ���)�o�� */
			DischReq = FALSE;										/* �}�����d��~					*/
			PowerChecker->var.DischActTimer = 0;
			PowerChecker->var.DischSeqNo = DISCH_FINISH;			/* �����X�e�b�v��				*/
		}
		else
		{
			DischReq = TRUE;										/* �m�F�p�}�����d��				*/
			PowerChecker->var.DischActTimer++;
		}
		break;

	case DISCH_FINISH: /* �}�����d���� or �ُ�I�� (���H�ē����҂�) */
		DischReq = FALSE;											/* �}�����d��~					*/
																	/* ������͌Č��ŃX�e�b�v������ */
		break;

	default: /* �{�P�[�X�ւ͗��Ȃ� */
		DischReq = FALSE;											/* �}�����d��~					*/
		break;

	}

	return	DischReq;
}



/****************************************************************************************************/
/*																									*/
/*		�`�b�d���������o����																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �d���������o�����ɂ��A�����𕪊򂷂�B												*/
/*																									*/
/*			PnE0C bit2 = 0 : ACON1�M���݂̂ŁA�������o���s���B										*/
/*			PnE0C bit2 = 1 : ACON1/ACON2�M���ŁA�������o���s���B									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxCheckAcOpenPhase( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{

#if( CSW_HW_DCBUS == TRUE )
/* Mask Function */
	return;
#endif

	if( PowerChecker->conf.DetectAcOn2 != TRUE )
	{
		/* �`�b�d���������o���� (ACON1�M���݂̂Ō��o) */
		LpxDetectAcOpenACON1( PowerChecker, PowSts );
	}
	else
	{
		/* �`�b�d���������o���� (ACON1/ACON2�M���Ō��o) */
		LpxDetectAcOpenACON12( PowerChecker, PowSts );
	}
	return;
}



#if( CSW_HW_DCBUS == FALSE )
/****************************************************************************************************/
/*																									*/
/*		�`�b�d���������o����	(ACON1�M���݂̂Ō��o)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �R�����͎d�l�̃A���v�ɑ΂��Č������o���s���B											*/
/*			�������o�����Ȃ��ƁA���[�^���o�͎��c�b�d�����~�����A�d���s���ŏo�͂�					*/
/*			�o�Ȃ��Ȃ�댯������B																	*/
/*			�܂��A�R���f���T�̃��b�v���d���������A�򉻂���������B									*/
/*																									*/
/*			���o�����́AAcon1�M��(1=AC�d��ON, 0=AC�d��OFF)���A										*/
/*			�x�^Lo�Ȃ琳��A�p���X��Ȃ猇���A�x�^Hi�Ȃ��d���I�t�Ƃ���B							*/
/*																									*/
/*			�������o���Ԃ�1000ms�Ƃ��AACON��ԕω���LO->HI��100��ȏ�ŁA							*/
/*			�d�������A���[��(A.F10)�Ƃ���B															*/
/*			������ԂɂȂ����ꍇ�A50Hz�d����10ms����LO->HI��ԕω���								*/
/*			ScanC�ɂČ��o�ł���Ƃ��������ł́A1000ms��100���ԕω����N����B						*/
/*			�����100���ԕω����N�����ꍇ�ɓd�������A���[��(A.F10)�Ƃ���B						*/
/*																									*/
/*																									*/
/*					       __    __    __    __    __    __    __   								*/
/*			Acon1  _______|  |__|  |__|  |__|  |__|  |__|  |__|  |__								*/
/*																									*/
/*						  |<--------------------------->|											*/
/*								1000ms�ȏ�ŃA���[��												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxDetectAcOpenACON1( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�`�b�d�������`�F�b�N����																	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2011.04.04 Y.Oka <V762> */
//	if( (PowerChecker->conf.TestMode1==FALSE) &&
//		(PowerChecker->conf.Ac3Phase==TRUE) && (PowSts->PowerOn==TRUE) )
	if( (PowerChecker->conf.TestMode1 == FALSE)
		&& (PowerChecker->conf.Ac3Phase == TRUE)
		&& (PowSts->PowerOn == TRUE)
		&& (PowerChecker->conf.Dcbus == FALSE) )
/* 2011.04.04 Y.Oka <V762> */
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		�������o�^�C�}�[�X�V����																*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowerChecker->var.OpenChkTimer[0] != 0 )
		{
			PowerChecker->var.OpenChkTimer[0]++;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		Acon1�M���G�b�W���o 																	*/
	/*----------------------------------------------------------------------------------------------*/
		if( (PowSts->Acon1Sig == TRUE) && (PowSts->Acon1Sig_l == FALSE) )
		{
			PowerChecker->var.OpenChkTimer[1] = PowerChecker->var.OpenChkTimer[0];
			if( PowerChecker->var.OpenChkTimer[0] == 0 )
			{
				PowerChecker->var.OpenChkTimer[0] = 1;			/* �^�C�}�[�X�^�[�g					*/
			}
		}
		else if( (PowerChecker->var.OpenChkTimer[0] - PowerChecker->var.OpenChkTimer[1]) > KPI_SCANC_MS( 12 ) )
		{
			PowerChecker->var.OpenChkTimer[0] = 0;				/* �G�b�W�����o12ms�ȏ�Ń��Z�b�g	*/
			PowerChecker->var.OpenChkTimer[1] = 0;				/*									*/
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		�ُ�`�F�b�N : �������o�^�C�}�[1000ms�ȏ�ŃA���[��										*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowerChecker->var.OpenChkTimer[0] > ACOPEN_DETECT_WAIT )
		{
			ALMSetGlobalAlarm( ALM_PWPHASE );					/*  A.F10 : �d������				*/

			PowerChecker->var.OpenChkTimer[0] = 0;
			PowerChecker->var.OpenChkTimer[1] = 0;
		}
	}
	else
	{
		PowerChecker->var.OpenChkTimer[0] = 0;
		PowerChecker->var.OpenChkTimer[1] = 0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�`�b�d���������o����	(ACON1/ACON2�M���Ō��o)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �R�����͎d�l�̃A���v�ɑ΂��Č������o���s���B											*/
/*			�������o�����Ȃ��ƁA���[�^���o�͎��c�b�d�����~�����A�d���s���ŏo�͂�					*/
/*			�o�Ȃ��Ȃ�댯������B																	*/
/*			�܂��A�R���f���T�̃��b�v���d���������A�򉻂���������B									*/
/*																									*/
/*			ACON1�M��																				*/
/*				HI��LO��HI�E�E�̏ꍇ: ����															*/
/*				�x�^LO �̏ꍇ       : �d��OFF or R������											*/
/*				�x�^HI �̏ꍇ       : �n�[�h�E�F�A�゠�蓾�Ȃ�										*/
/*			������̏ꍇ(20ms�ȓ��̎����ŁAHI��LO��HI�E�E)�AAcOn1=TRUE�ƂȂ�B						*/
/*																									*/
/*			ACON2�M��																				*/
/*				HI��LO��HI�E�E�̏ꍇ: ����															*/
/*				�x�^LO�̏ꍇ        : �d��OFF or S������ or T������									*/
/*				�x�^HI�̏ꍇ        : �n�[�h�E�F�A�゠�蓾�Ȃ�										*/
/*			������̏ꍇ(20ms�ȓ��̎����ŁAHI��LO��HI�E�E)�AAcOn2=TRUE�ƂȂ�B						*/
/*																									*/
/*			AcOn1/AcOn2�ɂ��A���L��Ԃɕ��ނ����B												*/
/*				AcOn1= TRUE		AcOn2= TRUE �� ����													*/
/*				AcOn1=FALSE		AcOn2= TRUE �� R������												*/
/*				AcOn1= TRUE		AcOn2=FALSE �� S�� or T������										*/
/*				AcOn1=FALSE		AcOn2=FALSE �� �d��OFF												*/
/*																									*/
/*			�]���āA�������o�́AAcOn1��AcOn2�̔r���I�_���a(XOR)�Ō��o����B							*/
/*			�������o���Ԃ�1000ms�Ƃ��A�d�������A���[��(A.F10)�Ƃ���B								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxDetectAcOpenACON12( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�`�b�d�������`�F�b�N����																	*/
/*--------------------------------------------------------------------------------------------------*/
/* 2011.04.04 Y.Oka <V762> */
//	if( PowerChecker->conf.TestMode1 == FALSE )
	if( (PowerChecker->conf.TestMode1 == FALSE) && (PowerChecker->conf.Dcbus == FALSE) )
/* 2011.04.04 Y.Oka <V762> */
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		ACON1�M�����o																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowSts->Acon1Sig != FALSE)
		{
			PowerChecker->var.DetOpenAcOn1 = TRUE;
			PowerChecker->var.AcOn1OffCount = 0;
		}
		else
		{
			if( PowerChecker->var.AcOn1OffCount > KPI_SCANC_MS( 20 ) )
			{
				PowerChecker->var.DetOpenAcOn1 = FALSE;
			}
			else
			{
				PowerChecker->var.AcOn1OffCount++;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*		ACON2�M�����o 																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowSts->Acon2Sig != FALSE)
		{
			PowerChecker->var.DetOpenAcOn2 = TRUE;
			PowerChecker->var.AcOn2OffCount = 0;
		}
		else
		{
			if( PowerChecker->var.AcOn2OffCount > KPI_SCANC_MS( 20 ) )
			{
				PowerChecker->var.DetOpenAcOn2 = FALSE;
			}
			else
			{
				PowerChecker->var.AcOn2OffCount++;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	�p���[�I����� : �����`�F�b�N���s��															*/
	/*----------------------------------------------------------------------------------------------*/
		if( PowSts->PowerOn == TRUE )
		{
			if(PowerChecker->var.DetOpenAcOn1 ^ PowerChecker->var.DetOpenAcOn2)
			{
				PowerChecker->var.OpenChkTimer[0]++;			/* �������o�^�C�}�[�J�E���g�A�b�v	*/
			}
			else
			{
				PowerChecker->var.OpenChkTimer[0] = 0;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	�ُ�`�F�b�N : �������o�^�C�}�[1000ms�ȏ�ŃA���[��											*/
	/*----------------------------------------------------------------------------------------------*/
			if( PowerChecker->var.OpenChkTimer[0] > ACOPEN_DETECT_WAIT )
			{
				ALMSetGlobalAlarm( ALM_PWPHASE );				/*  A.F10 : �d������				*/
				PowerChecker->var.OpenChkTimer[0] = 0;
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	�p���[�I�t��� : �������o�^�C�}�[�N���A														*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			PowerChecker->var.OpenChkTimer[0] = 0;
		}
	}
	return;
}


#endif	/* ( CSW_HW_DCBUS == FALSE ) */

/****************************************************************************************************/
/*																									*/
/*		�c�b�d���s���d��(UV)�Ď�����																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �s���d�����o�̓��[�^�̓���s�ǂ�h�����߂ɍs���B										*/
/*			�c�b�d���d����������ƃ��[�^�d�l�e���O�a���āA���[�^�̉�]�����������ꂽ��A			*/
/*			�g���N���ቺ�����肷��B																*/
/*																									*/
/*			�d���̃o�b�N�p���[���������ƋN����ƍl�����邪�A�ʏ킻��ȏ������e�ʂ̓d����			*/
/*			�����B�˓������[�̌̏�A�_�C�I�[�h�u���b�W�̌̏�A�q���[�Y�n�f�A�R���f���T�̗e			*/
/*			�ʔ��������i�̌̏�ŋN����ꍇ�̕��������B												*/
/*																									*/
/*			���͓d���d���̉����́A200 x 0.85 = 170VAC --> 240VDC �ł���A							*/
/*			���b�v���d�����l���Ă��A�R���̏ꍇ 240 x sin60 �� 208V �܂ŉ����邱�Ƃ͂Ȃ��B			*/
/*			�P���̏ꍇ�������ƍl���A�}�[�W�����݂āA���o���x�����P�X�O�u�Ƃ���B					*/
/*			���d���d���Ή������l�̊����ōl����B													*/
/*			  �P�O�O�u�n	  �X�T�u																*/
/*			  �Q�O�O�u�n	�P�X�O�u																*/
/*			  �S�O�O�u�n	�R�W�O�u																*/
/*																									*/
/*			�e�X�g���[�h(ACON�펞��UV���o���Ȃ����[�h)�̎��͂t�u���o���Ȃ��B						*/
/*																									*/
/*																									*/
/*	�� �� : �c�b�d���ߓd��(OV)�Ď�																	*/
/*			�ߓd��(OV)�Ď��ɂ��ẮA�t���o�͂ŉ񐶂���ꍇ�A�d���㏸�������̂ŁA					*/
/*			�X�L�����a(Kernel : KpxCheckDcOverVoltage())�ŏ�������B								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	LpxCheckDcUnderVoltage( CHECK_MAIN_POWER *PowerChecker, POWER_STATUS *PowSts )
{
	if( (PowerChecker->conf.TestMode1 == FALSE) && (PowSts->PowerOn == TRUE) )
	{
		if ( PowSts->DcVolt < PowerChecker->conf.UvLevel )
		{
			if( PowerChecker->var.UvChkTimer >= PowerChecker->conf.UvFilter )
			{
				if( PowSts->DcVolt == 0 )
				{
					ALMSetGlobalAlarm( ALM_VDC );			/* A.030 : ���H���o���ُ�				*/
				}
				else
				{
					ALMSetGlobalAlarm( ALM_PUV );			/* A.410 : �s���d��						*/
				}
			}
			else
			{
				PowerChecker->var.UvChkTimer += KPI_SCCYCLEMS;
			}
		}
		else
		{
			PowerChecker->var.UvChkTimer = 0;
		}
		if ( PowSts->DcVolt < PowerChecker->conf.UvWrnLevel )
		{
			if( PowSts->DcVolt != 0 )
			{
				ALMSetGlobalAlarm( WRN_UV );				/* A.971 : �s���d�����[�j���O���o		*/
			}
		}
		else
		{
			ALMClearGlobalAlarm( WRN_UV );					/* A.971 : �s���d�����[�j���O�N���A		*/
		}
	}
	else
	{
		PowerChecker->var.UvChkTimer = 0;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�񐶏��� ( ���s���� : 250us )																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���[�^����������Ƃ��G�l���M�[���A���v�ɋA���Ă���̂�(�񐶓���)�ADC�d�����㏸����B	*/
/*			DC�d�����㏸����Ǝ��H�R���f���T������̂ŁA�㏸���~�߂鏈�����K�v�ł���B			*/
/*			DC���C���ɉ񐶒�R�����A�񐶃G�l���M�[���R�ŏ����DC�d���̏㏸��}����B			*/
/*			�񐶒�R�̓d���͉񐶃g�����W�X�^��ON/OFF�����BDC�d�����Ď����ĉ񐶃g�����W�X�^		*/
/*			�𐧌䂷��B																			*/
/*			�����݂�ON/OFF����Ɖ񐶃g�����W�X�^�̃X�C�b�`���O���X���傫���Ȃ�̂ŁA�q�X�e���V�X��	*/
/*			�݂���B																				*/
/*			200V�n�ł́A���H�R���f���T�ψ���400V�ł���̂ŁA390V��ON�C380V��OFF�Ƃ���B���̓d��	*/
/*			�d���Ή��������������ƃ}�[�W��������̂ŁA���������̌��o���x���ŗǂ��Ƃ���B			*/
/*																									*/
/*				�Q�O�O�u�n : �R�X�O�u�c�I���A�R�W�O�u�c�I�t											*/
/*				�P�O�O�u�n : �P�X�T�u�c�I���A�P�X�O�u�c�I�t											*/
/*				�S�O�O�u�n : �V�W�O�u�c�I���A�V�U�O�u�c�I�t											*/
/*																									*/
/****************************************************************************************************/
void	PcmRegeneControl( REGENE_CTRL *pRegCtrl, POWER_STATUS *PowSts, BOOL MotStop )
{

#if	( CSW_HW_REGENERATION_CTRL == TRUE )

#if 0 /* 2010.09.01 Y.Oka �Ăяo�����Ŋm�F����悤�ύX */
/*--------------------------------------------------------------------------------------------------*/
/*		���H���o�p�����[�^�G���[�̎��A�댯����̂��߉񐶂����s���Ȃ�								*/
/*--------------------------------------------------------------------------------------------------*/
	if( VdetPrmOk == FALSE )
	{
		return;											/* �񐶂���return							*/
	}
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׃A���[�������`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( ALMCheckGlobalAlarm( ALM_RGOL ) )				/* ���j�b�g���ʃA���[���̂��߁A0���ڂ̂�	*/
	{ /* �񐶉ߕ���(A.320)������ */
		if( pRegCtrl->RegAlmMotStop || MotStop )
		{
			pRegCtrl->RegAlmMotStop = TRUE;
		}
		else
		{
			pRegCtrl->RegAlmMotStop = FALSE;
		}
	}
	else
	{
		pRegCtrl->RegAlmMotStop = FALSE;
	}

/* 2010.08.17 Y.Oka ���H���o���ُ�(A.030)���͉񐶏������s��Ȃ��悤�ύX���� */
/*--------------------------------------------------------------------------------------------------*/
/*		���H���o���ُ�A���[�������`�F�b�N														*/
/*--------------------------------------------------------------------------------------------------*/
	if( ALMCheckGlobalAlarm( ALM_VDC ) )				/* ���j�b�g���ʃA���[���̂��߁A0���ڂ̂�	*/
	{ /* ���H���o���ُ�(A.030)������ */
		HALdrv_RegeneControl( FALSE );					/* ��Tr�I�t�w��							*/
		pRegCtrl->RegStopCnt = 0;						/* �񐶓���I���҂��J�E���^�N���A			*/
		pRegCtrl->RegStartCnt = 0;						/* �񐶓���J�n�҂��J�E���^�N���A			*/
	}
	else
	{ /* ���H���o���ُ�(A.030)�������� */
/*--------------------------------------------------------------------------------------------------*/
/*		�񐶓d�����x���`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
//		if( PowSts->DcVolt > pRegCtrl->conf.RegOnLevel )		/* <S21D> */
		if( PowSts->DcVolt > (LONG)pRegCtrl->conf.RegOnLevel )	/* <S21D> */
		{ /* DC�d������OnLevel�𒴂����ꍇ */
			pRegCtrl->RegStopCnt = 0;					/* �񐶓���I���҂��J�E���^�N���A			*/
#if 0	/* <S21D> */
			if( pRegCtrl->RegStartCnt > pRegCtrl->conf.RegStartFilter )
			{ /* �񐶓���J�n�҂����Ԍo�ߎ� */
				pRegCtrl->RegOn = TRUE;					/* �񐶏������s��							*/
			}
			else
			{ /* �񐶓���J�n�҂���� */
				pRegCtrl->RegStartCnt++;				/* �񐶓���J�n�҂��J�E���^�X�V				*/
			}
#else	/* <S21D> */
			if( pRegCtrl->conf.RegSyn == PNREGSYN )
			{	/* �񐶓����L���iPn00E.3=1�j�̏ꍇ */
				if( pRegCtrl->RegStartCnt > (pRegCtrl->conf.RegStartFilter + (LONG)pRegCtrl->conf.RegSynWaitTime) )
				{ /* �񐶓���J�n�҂����Ԍo�ߎ� */
					pRegCtrl->RegOn = TRUE;					/* �񐶏������s��							*/
				}
				else
				{ /* �񐶓���J�n�҂���� */
					pRegCtrl->RegStartCnt++;				/* �񐶓���J�n�҂��J�E���^�X�V				*/
				}
			}
			else
			{
				if( pRegCtrl->RegStartCnt > pRegCtrl->conf.RegStartFilter )
				{ /* �񐶓���J�n�҂����Ԍo�ߎ� */
					pRegCtrl->RegOn = TRUE;				/* �񐶏������s��							*/
				}
				else
				{ /* �񐶓���J�n�҂���� */
					pRegCtrl->RegStartCnt++;			/* �񐶓���J�n�҂��J�E���^�X�V				*/
				}
			}
#endif
		}
		else
		{
			pRegCtrl->RegStartCnt = 0;					/* �񐶓���J�n�҂��J�E���^�N���A			*/
//			if( PowSts->DcVolt < pRegCtrl->conf.RegOffLevel )		/* <S21D> */
			if( PowSts->DcVolt < (LONG)pRegCtrl->conf.RegOffLevel )	/* <S21D> */
			{ /* DC�d������OffLevel�ȉ��ɂȂ����� */
				if( pRegCtrl->RegStopCnt > pRegCtrl->conf.RegStopFilter )
				{ /* �񐶓���I���҂����Ԍo�ߎ� */
					pRegCtrl->RegOn = FALSE;			/* �񐶏������~�߂�							*/
				}
				else
				{ /* �񐶓���I���҂���� */
					pRegCtrl->RegStopCnt++;				/* �񐶓���I���҂��J�E���^�X�V				*/
				}
			}
			else
			{ /* 2010.06.28 Y.Oka 2��A����OFF���x������������ꍇ�ɉ�OFF�Ƃ���B */
				pRegCtrl->RegStopCnt = 0;				/* �񐶓���I���҂��J�E���^�[���N���A		*/
			}
		}

/*--------------------------------------------------------------------------------------------------*/
/*		��Tr ON/OFF�o��																			*/
/*--------------------------------------------------------------------------------------------------*/
/*		DC���͂łȂ��A�񐶕s���샂�[�h�łȂ��ꍇ�ɁA��Tr���I���ƂȂ�����͈ȉ��̒ʂ�B			*/
/*			1. �}�����d�v������(PowSts->Discharge == TRUE)											*/
/*			2. �񐶗v������ && �񐶈ُ�Ȃ� && (�񐶉ߕ��ׂȂ� || �񐶉ߕ��׌ヂ�[�^���쒆)			*/
/*--------------------------------------------------------------------------------------------------*/
/*		A.320���o��A���[�^��~�������Tr���I�t����B�܂��AA.300���o��A��Tr���I�t����			*/
/*		MD�J�����́A�O�t���񐶒�R���� or �񐶒�R�����̏ꍇ�̂݉�Tr���I������(�ݒ�~�X����OV)	*/
/*		�Ƃ��Ă������A���e�ʂŉ񐶒�R��������Ă��Ȃ��T�[�{�p�b�N�ŉ񐶈ُ���}�X�N���邽�߂�����	*/
/*		�K���Ȓ�R��ڑ�����悤�ȃP�[�X������A���̏ꍇ�ɋ}�����d���Ȃ��Ȃ�̂͂܂����Ƃ������Ƃ�	*/
/*		��-�X�Ɠ����̏����ɖ߂����B	*/
/*--------------------------------------------------------------------------------------------------*/
		if( (PowSts->Discharge == TRUE)
			|| ((pRegCtrl->RegOn == TRUE)
//				&& ((ALMCheckGlobalAlarm(ALM_RG) == FALSE) && (pRegCtrl->RegAlmMotStop == FALSE))) )	/* <S21D> */
			&& ((ALMCheckGlobalAlarm(ALM_RG) == FALSE) && (pRegCtrl->RegAlmMotStop == FALSE))			/* <S21D> */
			&& (ALMCheckGlobalAlarm(WRN_EXREG) == FALSE) ) )		/* A.924�i�񐶌x���j�Ȃ�	*/			/* <S21D> */
		{ /* �� || �}�����d�v������ */
#if 0	/* <S21D> */
//			if( (pRegCtrl->conf.ResistorCapa != 0) || (pRegCtrl->conf.RegRxIn) )					  /* <S1CA> */
//			{
				HALdrv_RegeneControl( TRUE );
				if( pRegCtrl->RegOn )
				{
					pRegCtrl->RegOnCount[0] += 2;		/* �񐶉ߕ��׌��o�̂��߉�TrOn���J�E���g	*/
				}
//			}
//			else																					  /* <S1CA> */
//			{ /* �O�t���񐶒�R�Ȃ� && �񐶒�R�����Ȃ� */
//				HALdrv_RegeneControl( FALSE );
//			}
#else	/* <S21D> */
//			if( (pRegCtrl->conf.ResistorCapa != 0U) || (pRegCtrl->conf.RegRxIn == TRUE) )
//			{ /* �O�t���񐶒�R���� || �񐶒�R�����̏ꍇ */
				if( (pRegCtrl->conf.RegSyn == PNREGSYN)
				 && (ALMCheckGlobalAlarm(WRN_RGOLF) == TRUE) )
				{ /* �񐶓����L�� && �񐶉ߕ��׃��[�j���O�iA.920�j�����̏ꍇ */
					HALdrv_RegeneControl( FALSE );
				}
				else
				{
					HALdrv_RegeneControl( TRUE );
					if( pRegCtrl->RegOn == TRUE )
					{
						pRegCtrl->RegOnCount[0] += 2;		/* �񐶉ߕ��׌��o�̂��߉�TrOn���J�E���g	*/
					}
				}
//			}
//			else
//			{ /* �O�t���񐶒�R�Ȃ� && �񐶒�R�����Ȃ� */
//				HALdrv_RegeneControl( FALSE );
//			}
#endif
		}
		else
		{ /* �� || �}�����d�v���Ȃ� */
			HALdrv_RegeneControl( FALSE );
		}
	}
/* 2011.04.04 Y.Oka <V764> */
/*--------------------------------------------------------------------------------------------------*/
	return;

#endif	//( CSW_HW_REGENERATION_CTRL == FALSE )
}



/****************************************************************************************************/
/*																									*/
/*		DC�d���d�����o���� ( ���s���� : 250us ) for SGDS & SGDX										*/
/*																									*/
/*	�@�\�F	���H�d�����o�����ɂ���Č��o�֐����g��������B���H�d��(KioV.DcVoltx)���쐬����B	*/
/*				�EPWM - �R���p���[�^�����i�Œ�Ǐ]�����j											*/
/*				�EPWM - �R���p���[�^�����i�ϒǏ]�����j											*/
/*				�EA/D�ϊ�����																		*/
/*																									*/
/*	�߂�l:																							*/
/*			DETECT_DC_NO_OPERATION	: ����I��(�����s�v)											*/
/*			ALM_VDC					: �S���A���[���o��	A.030 : ���H���o���ُ�					*/
/*																									*/
/****************************************************************************************************/
LONG	PcmDetectDcVolt( DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts )
{
	LONG		ret;
	LONG		DcVolt;

	/* ���[�J���ϐ��̏����� */
	ret = DETECT_DC_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���d�����o����																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch(pDetectDc->conf.VdetMode)
	{
	/*------------------------------------------------------------------------------------------*/
	/*		�o�v�l�|�R���p���[�^����															*/
	/*------------------------------------------------------------------------------------------*/
		/*--------------------------------------------------------------------------------------*/
		/*  �Œ�}2v/250us(���e��),�ρ}16v/250(���e��)�̏ꍇ									*/
		/*--------------------------------------------------------------------------------------*/
		case VDET_PWM_FIX_FOLLOW:
		case VDET_PWM_VAR_FOLLOW:
			/* 2010.06.08 Y.Oka ���󖢑Ή� */
//			LpxPwmDetectDcVolt();
			break;
	/*------------------------------------------------------------------------------------------*/
	/*	�`�^�c�ϊ�����																			*/
	/*------------------------------------------------------------------------------------------*/
		case VDET_AD:
			LpxAdDetectDcVolt( pDetectDc );
			break;
	/*------------------------------------------------------------------------------------------*/
	/*		���o�Ȃ�	(SVFSEL_SWVDET != 0�ł����ɗ���ُ͈̂� DC�d��=0�Ƃ��Ă���)				*/
	/*------------------------------------------------------------------------------------------*/
		default:
			pDetectDc->DcVoltx = 0;
			break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���d�����o�l�␳����																	*/
/*--------------------------------------------------------------------------------------------------*/
	DcVolt = ((pDetectDc->DcVoltx * (0x100 + pDetectDc->conf.VdetAdadj)) >> 8);
	if( DcVolt >= 0 )
	{
		PowSts->DcVolt = DcVolt;
	}
	else
	{
		PowSts->DcVolt = 0;
	}


/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���d�����o�l�ُ�`�F�b�N																*/
/*--------------------------------------------------------------------------------------------------*/
	if( PowSts->Acon == TRUE )
	{ /* Check AC Power ON */
		if( pDetectDc->DcvWaitCnt >= KPI_SCANC_MS(20) )
		{ /* Check DcVolt Wait Count */
			if( (pDetectDc->DcVoltx == 0) || (pDetectDc->DcVoltx == pDetectDc->conf.VdetMaxVolt ) )
			{
				if( pDetectDc->DcvErrCnt > KPI_SCANC_MS(2) )
				{ /* 8�� * 250�� = 2ms �A���ŃG���[ */
//					KPI_BASE_BLOCK( );						/* �`�r�h�b�x�[�X�u���b�N				*/
//					KPI_MPCMD_SVOFF( );						/* �}�C�N���v���O�����T�[�{�n�e�e		*/
					ALMSetGlobalAlarm( ALM_VDC );			/* A.030 : ���H���o���ُ�				*/
					pDetectDc->DcvErrCnt = 0;				/* Reset Error Counter					*/
					pDetectDc->DcvWaitCnt = 0;				/* Reset DcVolt Wait Counter			*/
					ret = ALM_VDC;							/* A.030 : ���H���o���ُ�				*/
				}
				else
				{
					pDetectDc->DcvErrCnt++;					/* Increment Error Count				*/
				}
			}
			else
			{
				pDetectDc->DcvErrCnt = 0;				/* Reset DcVolt Wait Counter			*/
			}
		}
		else
		{
			pDetectDc->DcvWaitCnt++;						/* Increment DcVolt Wait Count			*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else													/* AC Power OFF							*/
	{
		pDetectDc->DcvErrCnt = 0;			/* Reset Error Counter					*/
		pDetectDc->DcvWaitCnt = 0;			/* Reset DcVolt Wait Counter			*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		OV���o���� ( ���s���� : 250us )																*/
/*																									*/
/*	�@�\:	�ߓd�����o�͎��H���i�̕ی�̂��߂ɍs���B												*/
/*			�ψ����ł��Ⴂ�͎̂��H�R���f���T�ł���B												*/
/*			���H�R���f���T�͑ψ����z����Ɣ�������댯������B									*/
/*			�ʏ�񐶏����������āA�ߓd�����x���܂�DC�d�����㏸���邱�Ƃ͂Ȃ��B						*/
/*			�񐶏�����H�̕s��܂��͉񐶒�R�l�̕s�������ɂ��ߓd���ɂȂ邱�Ƃ����蓾��B		*/
/*			�t���o�͂ŉ񐶂���Ɠd���㏸�������̂ŁAScanB�ŏ�������B								*/
/*			DC�d�����x�����Ď����ĉߓd�����o���x�����z������ُ폈������B							*/
/*																									*/
/*	�߂�l:																							*/
/*			DETECT_DC_NO_OPERATION	: ����I��(�����s�v)											*/
/*			ALM_VDC					: �S���A���[���o��	A.030 : ���H���o���ُ�					*/
/*			ALM_OV					: �S���A���[���o��	A.400 : �ߓd��(�n�u)						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	@CHK@ : �A���[�����o�̘_������������!!!!!														*/
/*			�E�ߓd��(OV)�`�F�b�N�ƕs���d��(UV)�`�F�b�N�ŁA0xFF��0x00���`�F�b�N���Ă���				*/
/*			�E�c�b�d���d�����o�l�ُ�`�F�b�N(0x00,0xFF)�́A���̂��߂ُ̈�`�F�b�N���H				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
//LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts ) /* <S0F7> */
LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc,
														POWER_STATUS *PowSts, REGENE_CTRL *RegCtrl ) /* <S0F7> */
{
	LONG		ret;
	USHORT		OvLevel; /* <S0F7> */

	/* ���[�J���ϐ��̏����� */
	ret = DETECT_DC_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶎g�p�󋵃`�F�b�N														<S0F7>			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (RegCtrl->conf.ResistorCapa != 0) || (RegCtrl->conf.RegRxIn) )
	{ /* �񐶎g�p���� */
		OvLevel = PowerChecker->conf.OvLevel;
	}
	else
	{ /* �񐶕s�g�p */
		OvLevel = PowerChecker->conf.NonRegOvLevel;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���ߓd���`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
//	if( PowSts->DcVolt > (LONG)PowerChecker->conf.OvLevel )	/* DC�d����OV���x���𒴂����ꍇ			*//* <S0F7> */
	if( PowSts->DcVolt > (LONG)OvLevel )					/* DC�d����OV���x���𒴂����ꍇ			*/
	{
		if( pDetectDc->DcVoltx == pDetectDc->conf.VdetMaxVolt )
		{
			if( pDetectDc->VdcdetAlmCnt > pDetectDc->conf.VdcdetAlmFilter )
			{
//				KPI_BASE_BLOCK( );							/* �`�r�h�b�x�[�X�u���b�N				*/
//				KPI_MPCMD_SVOFF( );							/* �}�C�N���v���O�����T�[�{�n�e�e		*/
				ALMSetGlobalAlarm( ALM_VDC );				/* A.030 : ���H���o���ُ�				*/
				ret = ALM_VDC;								/* A.030 : ���H���o���ُ�				*/
			}
			else
			{
				pDetectDc->VdcdetAlmCnt++;					/* Increment Error Counter				*/
			}
		}
		/* 2010.06.08 Y.Oka �}�X�N�`�F�b�N�͊O�Ŏ��{����悤�ύX */
//		else if( !KPI_ALARMMSKCHK( ALM_OV ) )				/* �A���[���}�X�N�`�F�b�N				*/
		else
		{
			if( pDetectDc->OvdetAlmCnt > PowerChecker->conf.OvdetAlmFilter )
			{
//				KPI_BASE_BLOCK( );							/* �`�r�h�b�x�[�X�u���b�N				*/
//				KPI_MPCMD_SVOFF( );							/* �}�C�N���v���O�����T�[�{�n�e�e		*/
				ALMSetGlobalAlarm( ALM_OV );				/* A.400 : �ߓd��(�n�u)					*/
				ret = ALM_OV;								/* A.400 : ���H���o���ُ�				*/
			}
			else
			{
				pDetectDc->OvdetAlmCnt++;					/* Increment Error Counter				*/
			}
		}
	}
	else
	{
		pDetectDc->VdcdetAlmCnt = 0;						/* Reset Error Conter					*/
		pDetectDc->OvdetAlmCnt = 0;							/* Reset Error Conter					*/
	}

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		DC�d���d�����o���� ( ���s���� : 250us ) 													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@�\�@: A/D�ϊ����ꂽ���H���o�d�������o����													*/
/*																									*/
/*	���Ł@:	2007.02.07	K.Ando		<V134>															*/
/*			�`�c�Ǐo���l�ɎO�d���t�B���^��ǉ�														*/
/*																									*/
/****************************************************************************************************/
static	void	LpxAdDetectDcVolt( DET_DC_VOLT *pDetectDc )
{
	LONG	AdRead;
	LONG	DcVoltx;

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���`�c�Ǎ���																			*/
/*--------------------------------------------------------------------------------------------------*/
	AdRead = HALdrv_GetDcVoltFromAd( );						/* �c�b�d���`�c�Ǎ���					*/
	DcVoltx = MlibMulgain( AdRead, pDetectDc->conf.VdetAdNorm );

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���̃N�����v����																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( DcVoltx < 0 )
	{
		DcVoltx = 0;
	}
	else if( DcVoltx > pDetectDc->conf.VdetMaxVolt )
	{
		DcVoltx = pDetectDc->conf.VdetMaxVolt;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�c�b�d���̃t�B���^����																		*/
/*--------------------------------------------------------------------------------------------------*/
	pDetectDc->DcAdFili = DcVoltx;
	pDetectDc->DcAdFilo[0] =
		MlibLpfilter1( DcVoltx, pDetectDc->conf.VdetAdFil, pDetectDc->DcAdFilo[0] );
	pDetectDc->DcAdFilo[1] =
		MlibLpfilter1( pDetectDc->DcAdFilo[0], pDetectDc->conf.VdetAdFil, pDetectDc->DcAdFilo[1] );
	pDetectDc->DcAdFilo[2] =
		MlibLpfilter1( pDetectDc->DcAdFilo[1], pDetectDc->conf.VdetAdFil, pDetectDc->DcAdFilo[2] );
	pDetectDc->DcVoltx = pDetectDc->DcAdFilo[2];

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�񐶉ߕ��׃`�F�b�N���� (Call from ScanC)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �񐶃g�����W�X�^�̃I�����Ԃ��A�񐶒�R�̏���d�͂𐄒肵�A							*/
/*			�ߕ��׃��x�����z�����ꍇ�A�񐶉ߕ��׃A���[���Ƃ���B									*/
/*			�܂��A�P�O�b�Ԃ̕��ω񐶓d�͒l���v�Z����B												*/
/*																									*/
/*	�߂�l:																							*/
/*			CHK_REGOL_NO_OPERATION	: �񐶉ߕ��ׂȂ�(�����s�v)										*/
/*			CHK_REGOL_SET_ALARM		: �S���A���[���o��		A.320 : �񐶉ߕ���						*/
/*			CHK_REGOL_SET_WARNING	: �S�����[�j���O�o��	A.920 : �񐶉ߕ��׃��[�j���O			*/
/*			CHK_REGOL_CLR_WARNING	: �S�����[�j���O�N���A	A.920 : �񐶉ߕ��׃��[�j���O			*/
/*																									*/
/****************************************************************************************************/
LONG	PcmCheckRegeneOverLoad( REGENE_CTRL *pRegCtrl,ULONG *AlmOLInfo, ANLG_UN_MON_DATA *MonData )
{
	LONG	RegPower;
	LONG	LastRgolPowSum;
	LONG	ret;

	/* ���[�J���ϐ��̏����� */
	ret = CHK_REGOL_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶓d�͂̌v�Z																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pRegCtrl->conf.ResistorCapa == 0) && (pRegCtrl->conf.RegRxIn == FALSE) )
	{
		RegPower = 0;
	}
	else
	{
#if (FLOAT_USE==TRUE)
		RegPower = pRegCtrl->dRegOnCount * pRegCtrl->RegOL.conf.Krgpower;
#else
		RegPower = MlibMulgain30( pRegCtrl->dRegOnCount, pRegCtrl->RegOL.conf.Krgpower );
#endif /* FLOAT_USE */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��ׂ̃`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* 2010.09.01 Y.Oka �Ăяo�����Ŋm�F����悤�ύX */
//	if( (TestMode2 == FALSE) && (prm_ptr->iprm_ptr->Dcbus == FALSE) )
	{
		/*	�O��̉񐶓d�͐ώZ�l�ۑ� */
		LastRgolPowSum = pRegCtrl->RegOL.var.RgolPowSum;
		pRegCtrl->RegOL.var.RgolPowSum += (RegPower - pRegCtrl->RegOL.conf.RgolPowBase);
		if( pRegCtrl->RegOL.var.RgolPowSum < 0 )
		{
			pRegCtrl->RegOL.var.RgolPowSum = 0;
		}
	/*----------------------------------------------------------------------------------------------*/
		/* 2010.11.16 Y.Oka Pn600 = 0�ݒ�΍� */
//		if( pRegCtrl->RegOL.var.RgolPowSum >= pRegCtrl->RegOL.conf.RgolAlmLevel )
		if( pRegCtrl->RegOL.var.RgolPowSum > pRegCtrl->RegOL.conf.RgolAlmLevel )
		{
//			KPI_ALARMSET( ALM_RGOL );					/* A.320 : �񐶉ߕ��� 						*/
			ret |= CHK_REGOL_SET_ALARM;
			pRegCtrl->RegOL.var.LastRgolDetect = FALSE;	/* �O��N�����̉񐶉ߕ��׌��o�t���O�I�t		*/
		}
		/* 2010.11.16 Y.Oka Pn600 = 0�ݒ�΍� */
//		if( pRegCtrl->RegOL.var.RgolPowSum >= (pRegCtrl->RegOL.conf.RgolAlmLevel>>1) )
		if( pRegCtrl->RegOL.var.RgolPowSum > (pRegCtrl->RegOL.conf.RgolAlmLevel>>1) )
		{
//			KPI_WARNINGSET( WRN_RGOLF );				/* A.920 : �񐶉ߕ��׃��[�j���O				*/
//			ret = CHK_REGOL_SET_WARNING;
			if( !pRegCtrl->RegOL.var.LastRgolDetect || pRegCtrl->RegOL.var.RgPowUp )
			{ /* �O�񌟏o�Ȃ� || �d�͑����L�� */
				ret |= CHK_REGOL_SET_WARNING;			/* A.920 : �񐶉ߕ��׃��[�j���O				*/
			}
			else if( LastRgolPowSum < pRegCtrl->RegOL.var.RgolPowSum )
			{ /* �O��񐶓d�͂��瑝�� */
				ret |= CHK_REGOL_SET_WARNING;			/* A.920 : �񐶉ߕ��׃��[�j���O				*/
				pRegCtrl->RegOL.var.RgPowUp = TRUE;		/* �񐶓d�͑����t���O�I��					*/
														/* RgPowUp��TRUE��A�O�񌟏o�L���Ɋւ�炸�A*/
														/* ���[�j���O�̂��߁AFALSE�͕s�v			*/
			}
			else
			{ /* �O�񌟏o�L��œd�͑������Ȃ��ꍇ�̓��[�j���O�o�͂��Ȃ� */
				ret |= CHK_REGOL_CLR_WARNING;			/* A.920 : �񐶉ߕ��׃��[�j���O				*/
			}
		}
		else
		{
			if( pRegCtrl->RegOL.var.LastRgolDetect )
			{ /* �O��N�����̉񐶉ߕ��׌��o�t���O�I�� */
				pRegCtrl->RegOL.var.LastRgolDetect = FALSE;	/* �O��N�����̉񐶉ߕ��׌��o�t���O�I�t	*/
				*AlmOLInfo &= ~ALMDEF_OLINFO_RGOL;			/* �񐶉ߕ��׏�񃊃Z�b�g				*/
			}
//			KPI_WARNINGCLR( WRN_RGOLF );				/* A.920 : �񐶉ߕ��׃��[�j���O				*/
			ret |= CHK_REGOL_CLR_WARNING;				/* A.920 : �񐶉ߕ��׃��[�j���O				*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Un00A : �񐶓d�͗p�f�[�^	(10sec�񐶓d�͕��ϒl)	CoutV.UnMeanRgPower						*/
/*		Un143 : �񐶓d�͗p�f�[�^	(2ms�񐶓d�͒l)			CoutV.UnRgPower							*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	MonData->UnRgPower = RegPower * pRegCtrl->RegOL.conf.KunrgpowerCycle;
#else
	MonData->UnRgPower = MlibMulgain( RegPower, pRegCtrl->RegOL.conf.KunrgpowerCycle );
#endif /* FLOAT_USE */
	MonData->TrcRgPower = MonData->UnRgPower;
	pRegCtrl->RegOL.var.RgPowerSum += RegPower;

	pRegCtrl->RegOL.var.RgMeanCycle++;
	if( pRegCtrl->RegOL.var.RgMeanCycle >= KPI_SCANC_MS( 10000 ) )
	{
#if 0
#if (FLOAT_USE==TRUE)
		MonData->UnMeanRgPower = pRegCtrl->RegOL.var.RgPowerSum * pRegCtrl->RegOL.conf.Kunrgpower;
#else
		MonData->UnMeanRgPower = MlibMulgain( pRegCtrl->RegOL.var.RgPowerSum, pRegCtrl->RegOL.conf.Kunrgpower );
#endif /* FLOAT_USE */
#endif
/*<S123>--->*/
#if (FLOAT_USE==TRUE)
		pRegCtrl->UnMeanRgPower = pRegCtrl->RegOL.var.RgPowerSum * pRegCtrl->RegOL.conf.Kunrgpower;
#else
		pRegCtrl->UnMeanRgPower = MlibMulgain( pRegCtrl->RegOL.var.RgPowerSum, pRegCtrl->RegOL.conf.Kunrgpower );
#endif
/*<S123><---*/
		pRegCtrl->RegOL.var.RgPowerSum  = 0;
		pRegCtrl->RegOL.var.RgMeanCycle = 0;
	}

	return ret;
}


#if 0
/****************************************************************************************************/
/*																									*/
/*		�񐶉ߕ��׈ُ�A���[���^���[�j���O�ݒ菈�� (Call from ScanC)								*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : LpxCheckRegeneOverLoad()�ɂă`�F�b�N�����񐶉ߕ��׏�Ԃɉ����āA						*/
/*			�񐶉ߕ��׃A���[���Z�b�g�A�񐶉ߕ��׃��[�j���O�Z�b�g�A�񐶉ߕ��׃��[�j���O�N���A		*/
/*			���������{����B																		*/
/*																									*/
/****************************************************************************************************/
void	SetRegeneOverLoadSts( LONG RegOlSts )
{
	if( RegOlSts & CHK_REGOL_SET_ALARM )
	{
		/* A.320 : �񐶉ߕ��� */
		ALMSetGlobalAlarm( ALM_RGOL );
	}
	else if( RegOlSts & CHK_REGOL_SET_WARNING )
	{
		/* A.920 : �񐶉ߕ��׃��[�j���O */
		ALMSetGlobalAlarm( WRN_RGOLF );
	}
	else
	{
		/* A.920 : �񐶉ߕ��׃��[�j���O */
		ALMClearGlobalAlarm( WRN_RGOLF );
	}
	return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		�˓���R�ߕ��׃`�F�b�N���� (Call from ScanC)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���H�d���̃I���^�I�t�̕p�x�̃`�F�b�N���s���A											*/
/*			�Q���ԂɂP�O��I���^�I�t���ꂽ�ꍇ�A�˓���R�ߕ��׃A���[���Ƃ���B						*/
/*																									*/
/*			�Q���Ԗ��̒P���`�F�b�N�ł́A�P�W��^�Q���Ԃ��������\��������̂ŁA					*/
/*			���������炵���Q�i�`�F�b�N�Ƃ���B														*/
/*			����ł����S�ł͂Ȃ����A�܂��߂ɏ�������Ƒ�ςł���B									*/
/*																									*/
/*						   2min 																	*/
/*			Cnt[0] :  |<----------->|<----------->|<----------->|<----------->|<------ 				*/
/*			Cnt[1] :  ------>|<----------->|<----------->|<----------->|<----------->|				*/
/*								  2min  															*/
/*																									*/
/*	�߂�l:																							*/
/*			CHK_RUSH_NO_OPERATION	: �˓���R�ߕ��ׂȂ�(�����s�v)									*/
/*			CHK_RUSH_SET_ALARM		: �S���A���[���o��	A.740 : �˓���R�ߕ���						*/
/*																									*/
/****************************************************************************************************/
BOOL	PcmCheckRushRxOverLoad( CHECK_RUSH *pRush, POWER_STATUS *pPowerSts )
{
	BOOL	ret;

	/* ���[�J���ϐ��̏����� */
	ret = CHK_RUSH_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		�p���[�n�e�e==>�n�m���o																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( pPowerSts->PowerOn == TRUE )
	{
		if( pRush->PowerOnChk == FALSE )
		{
			pRush->PowerOnCnt[0]++;						/* Count up Power Off ==> On				*/
			pRush->PowerOnCnt[1]++;						/* Count up Power Off ==> On				*/
		}
		pRush->PowerOnChk = TRUE;
	}
	else
	{
		pRush->PowerOnChk = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�˓���R�ߕ��׃`�F�b�N																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (pRush->PowerOnCnt[0] >= 10) || (pRush->PowerOnCnt[1] >= 10) )
	{
		/* A.740 : �˓���R�ߕ��� */
//		KPI_ALARMSET( ALM_RSOL );
		ret = CHK_RUSH_SET_ALARM;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		���o�T�C�N�����䁕�J�E���^�N���A����														*/
/*--------------------------------------------------------------------------------------------------*/
	pRush->PowerOnChkCycle++;
	if( pRush->PowerOnChkCycle >= KPI_SCANC_MS( 2*60*1000 ) )
	{
		pRush->PowerOnCnt[0] = 0;
		pRush->PowerOnChkCycle = 0;
	}
	else if( pRush->PowerOnChkCycle == KPI_SCANC_MS( 1*60*1000 ) )
	{
		pRush->PowerOnCnt[1] = 0;
	}

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		�񐶈ُ�`�F�b�N���� (Call from ScanC)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �񐶈ُ팟�o�͉񐶂s���̒Z���̏�����o������̂ł���B									*/
/*			�p���[���f�B���A�N�e�B�u�̎��̉񐶂s���̃R���N�^(�h�f�a�s�̃h���C��)�̓d����			*/
/*			�R���p���[�^�ŊĎ����A��莞�Ԃk����Ԃ��������ꍇ�A�ُ�Ƃ݂Ȃ��B						*/
/*			����ȉ񐶓��쎞�͒Z�������ł̃I���^�I�t���J��Ԃ����A�����ԃI�����p�����邱			*/
/*			�Ƃ͂Ȃ��B																				*/
/*																									*/
/*			1) �ُ�Ƃ݂Ȃ����Ԃ͂S�O�O�����Ƃ���B����́A�񐶂s�����Z���̏Ⴕ���ꍇ�A				*/
/*			   �񐶒�R���đ����Ȃ����x�̎��Ԃł���B												*/
/*			2) �񐶒�R��������Ȃ��^�C�v������B����̓V�X�e���萔�ɂ���ʂ��A���o��				*/
/*			   �p�X����B																			*/
/*			3) ��-�U�ł́A�c�b�d�����̓��[�h�����Ă��Ȃ��������A�񐶉�H�͓������Ă���A			*/
/*			   �񐶂s�����Z���̏Ⴕ���ꍇ�đ��̊댯������̂Ō��o����悤�ɂ���B					*/
/*			4) �]���́A�񐶒�R�̒f���̌��o�����˂Ă������A�e�ʂɂ��񐶒�R���������́A			*/
/*			   �񐶒�R�O�t��������̂Ȃǂ���A�f�����o���邵�Ȃ��̔��f���v���ĕ��G�ɂ�				*/
/*			   ��̂ŁA�񐶒�R�f���͉񐶉ߕ��׌��o�A�ߓd�����o�ōs�����̂Ƃ���B					*/
/*			5) �񐶂s���R���N�^���o���C���ɍ���R�łȂ��ŁA�񐶒�R�f�������g�����x��				*/
/*			   �ɕۂ悤�ɂ���B																	*/
/*																									*/
/*	�߂�l :																						*/
/*			CHK_REGENE_NO_OPERATION	: �񐶈ُ�Ȃ�(�����s�v)										*/
/*			CHK_REGENE_SET_ALARM	: �S���A���[���o��	A.300 : �񐶈ُ�							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	2002.01.17 : �c�b���̓��[�h����тUKW�ȏ�ŃA���[�����o���������Ȃ�(SGDH)�Ή�					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL	PcmCheckRegeneAlarm( REGENE_CTRL *pRegCtrl, BOOL PowerOn )
{
	BOOL	ret;

	/* ���[�J���ϐ��̏����� */
	ret = CHK_REGENE_NO_OPERATION;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶈ُ�`�F�b�N����																		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
	if( (TestMode1 == FALSE)
		&&	((pRegCtrl->conf.RegRxIn == TRUE) || (pRegCtrl->conf.ResistorCapa != 0))
		/* 2010.09.01 Y.Oka �d�����̓��[�h�͌Ăяo�����Ŋm�F�B�e�ʂɂ͈ˑ����Ȃ��悤�ύX�B */
//		&&	((iprm_ptr->Dcbus == FALSE ) || (uni_prm_ptr->pprm_ptr->ampw < 6000))
		&&	(PowerOn == TRUE) )
//�@�e�X�g���[�h���͌Ăяo�����PowerOn=FALSE�Ƃ���Ȃǂ��đΏ�
#endif
	if( ((pRegCtrl->conf.RegRxIn == TRUE) || (pRegCtrl->conf.ResistorCapa != 0))
		&&	(PowerOn == TRUE) )
	{
		/*	(�񐶈ُ�M���I�� && ��Tr�w�߃I�t) || (�񐶈ُ�M���I�t && ��Tr�w�߃I��) */
//		if( ((pRegCtrl->RegAlmSig == TRUE ) && ((pAsicHwReg->AREG_IOSTAT1 & 0x0002) != 0)) ||
//			((pRegCtrl->RegAlmSig == FALSE) && ((pAsicHwReg->AREG_IOSTAT1 & 0x0002) == 0)) )
		if( ((pRegCtrl->RegAlmSig == TRUE ) && (FALSE == HALdrv_GetRegeneControlState())) ||
			((pRegCtrl->RegAlmSig == FALSE) && (FALSE != HALdrv_GetRegeneControlState())) )
		{
			if( pRegCtrl->RegChk.RegeneErrCnt <= KPI_SCANC_MS( 400 ) )
			{
				pRegCtrl->RegChk.RegeneErrCnt++;
			}
			else
			{
				/* A.300 : �񐶈ُ� */
//				KPI_ALARMSET( ALM_RG );
				ret = CHK_REGENE_SET_ALARM;
			}
			pRegCtrl->RegChk.RegeAlmSignal = TRUE;
		}
		else
		{
			pRegCtrl->RegChk.RegeneErrCnt = 0;
			pRegCtrl->RegChk.RegeAlmSignal = FALSE;
		}
	}
	else
	{
		pRegCtrl->RegChk.RegeneErrCnt = 0;
		pRegCtrl->RegChk.RegeAlmSignal = FALSE;
	}

	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		�T�[�{�p�b�N�����t�@����~�`�F�b�N����		<S0A3>											*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{�p�b�N�ɓ�������Ă���t�@���̒�~���Ď�����B									*/
/*			���ڃp���[����ɂ͊֌W���Ȃ����A���j�b�g���ʏ����̂��߁APCM���W���[���̈ꕔ�Ƃ���B		*/
/*																									*/
/*	���Ł@:																							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define FANLOCK_START_TIME	KPI_SCANC_MS( 10000 )	/* �t�@����~�m�F���J�n����܂ł̎���(10s)		*/
/*--------------------------------------------------------------------------------------------------*/
LONG	PcmCheckFanLock( CHECK_FANLOCK *FanLock, BOOL FanStop, BOOL BeSts )
{
	LONG	ret;
	ULONG	CumulativeOperationTime;	/* �ݐωғ�����[100ms](�t�@����~���[�j���O������) */	/* <S219> */

	ret = CHK_FANLOCK_NO_OPERATION;

	if( FanLock->var.FanLockStartCnt >= FANLOCK_START_TIME )
	{
		if( FanStop == TRUE )
		{
			if( FanLock->var.FanLockTimeCnt >= FanLock->conf.FanLockDetectTime )
			{
				if( BeSts == TRUE )
				{ /* SVON���̎�������ꍇ */
					ret = CHK_FANLOCK_SET_WARNING;			/* A.923 : �t�@����~���[�j���O			*/

					/* <S219> Start */
					/* �t�@����~���[�j���O���o 1�X�L�����ڂ̏ꍇ */
					if( FanLock->var.FanLockWrnFlag == FALSE )
					{
						/* ���ݗݐωғ����Ԏ擾(�t�@����~���[�j���O������) */
						FanLock->var.WrnStartTime = KlibGetTimeStamp();

						/* �t�@����~���[�j���O�����t���O = TRUE */
						FanLock->var.FanLockWrnFlag = TRUE;
					}

					/* ���ݗݐωғ����Ԏ擾(100[ms]�P��) */
					CumulativeOperationTime = KlibGetTimeStamp();

					/* ���ݗݐωғ����� - �t�@����~���[�j���O�������̗ݐωғ����Ԏ擾 > PnEE6:�T�[�{�I���������t�@����~�A���[�����o���� */
					if( ( CumulativeOperationTime - FanLock->var.WrnStartTime) > FanLock->conf.FanLockAlmDetectTime )
					{
						ret = CHK_FANLOCK_SET_ALARM;			/* A.7AB : �t�@����~�A���[��			*/

						/* �t�@����~���[�j���O�����t���O = FALSE */
						FanLock->var.FanLockWrnFlag = FALSE;
					}
					/* <S219> End */
				}
				else
				{ /* �S��SVOFF�̏ꍇ */
					ret = CHK_FANLOCK_SET_ALARM;			/* A.7AB : �t�@����~�A���[��			*/

					/* �t�@����~���[�j���O�����t���O = FALSE */	/* <S219> */
					FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */
				}
			}
			else
			{
				/* �t�@����~���ԃJ�E���g */
				FanLock->var.FanLockTimeCnt++;

				/* �t�@����~���[�j���O�����t���O = FALSE */	/* <S219> */
				FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */
			}
		}
		else
		{
			/* �t�@����~���ԃJ�E���g�N���A */
			FanLock->var.FanLockTimeCnt = 0;

			/* �t�@����~���[�j���O�����t���O = FALSE */	/* <S219> */
			FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */
		}
	}
	else
	{
		/* �d��������AFANLOCK_START_TIME�o�߂���܂Ńt�@����~�m�F���Ȃ� */
		FanLock->var.FanLockStartCnt++;

		/* �t�@����~���[�j���O�����t���O = FALSE */	/* <S219> */
		FanLock->var.FanLockWrnFlag = FALSE;			/* <S219> */

	}

	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		Initialize Unit Alarm Check Variables														*/
/*																									*/
/****************************************************************************************************/
void	PcmInitPowerManager( POW_MNG_HNDL *hPowerManager, ULONG AlmOLInfo )
{
	CHECK_MAIN_POWER	*pMainPowerChecker;
	CHECK_REGOL			*pRegOL;

	/* ���[�J���ϐ��̏����� */
	pMainPowerChecker = &(hPowerManager->MainPowerChecker);
	pRegOL = &(hPowerManager->RegeneCtrl.RegOL);

/*--------------------------------------------------------------------------------------------------*/
/*		���H�d���Ď��p�ϐ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(pMainPowerChecker->var), sizeof(pMainPowerChecker->var)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׌��o�p�ϐ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(pRegOL->var), sizeof(pRegOL->var)/4 );

	if( AlmOLInfo & ALMDEF_OLINFO_RGOL )
	{ /* �񐶉ߕ��׃A���[����񂠂�̏ꍇ */
		/* �O��N�����̉񐶉ߕ��׌��o�t���O�I�� */
		pRegOL->var.LastRgolDetect = TRUE;

		switch( pRegOL->conf.RgSumSel )
		{
		case REGPOWSUM_RATE95PER:					/*	�񐶉ߕ��׃A���[�����x����95����ݒ�		*/
			pRegOL->var.RgolPowSum = ( pRegOL->conf.RgolAlmLevel / 100 ) * 95;
			break;
		case REGPOWSUM_RATE50PER:					/*	�񐶉ߕ��׃A���[�����x����50����ݒ�		*/
			pRegOL->var.RgolPowSum = ( pRegOL->conf.RgolAlmLevel >> 1 );
			break;
		default:									/*	�񐶉ߕ��׃A���[�����x����50����ݒ�		*/
			pRegOL->var.RgolPowSum = ( pRegOL->conf.RgolAlmLevel >> 1 );
			break;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�˓��h�~��R�ߕ��׌��o�p�ϐ�������															*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(hPowerManager->Rush), sizeof(hPowerManager->Rush)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶈ُ팟�o�p�ϐ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* Reset Variable Data */
	MlibResetLongMemory( &(hPowerManager->RegeneCtrl.RegChk), sizeof(hPowerManager->RegeneCtrl.RegChk)/4 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Count Regene Transistor ON Time																*/
/*																									*/
/****************************************************************************************************/
void	PcmCountRegeneOnTime( POW_MNG_HNDL *hPowerManager )
{
	LONG	lwk0;

/*--------------------------------------------------------------------------------------------------*/
/*		Delta Regene.Tr On Count for ScanC Alarm Check												*/
/*--------------------------------------------------------------------------------------------------*/
	/* Current Regene ON Count */
	lwk0 = hPowerManager->RegeneCtrl.RegOnCount[0];
	/* Delta Regene ON Count */
	hPowerManager->RegeneCtrl.dRegOnCount = lwk0 - hPowerManager->RegeneCtrl.RegOnCount[1];
	/* Update Last Regene ON Count */
	hPowerManager->RegeneCtrl.RegOnCount[1] = lwk0;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		Count Regene Transistor ON Time																*/
/*																									*/
/****************************************************************************************************/
void	PcmInputPowerStatus( POW_MNG_HNDL *hPowerManager, BOOL Acon1, BOOL RegeneAlm )
{
	hPowerManager->PowerSts.Acon1Sig_l = hPowerManager->PowerSts.Acon1Sig;
	hPowerManager->PowerSts.Acon1Sig = Acon1;
//	hPowerManager->PowerSts.Acon2Sig = Acon2;
	hPowerManager->RegeneCtrl.RegAlmSig = RegeneAlm;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�񐶓d�͉��Z�Q�C���̌v�Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �񐶓d�͉��Z�Q�C���̌v�Z���s���B														*/
/*			�񐶓d�͂́A[5000/�R���o�[�^�e��]�̒P�ʂŁAScanC�Ԃ̕��ϒl�ŕ\������B					*/
/*			ScanB�����ŉ񐶃g�����W�X�^��ON/OFF���J�E���g����̂ŁA�񐶓d�͂́A						*/
/*			�����ƂȂ�B																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	SGD7*,INGRAM																					*/
/*						 (DcVolt)^2  * SBCYCLEUS * 5000  											*/
/*			�񐶓d�� = ----------------------------------- * dRegOnCount							*/
/*						(reger/1000) * SCCYCLEUS * ConvW  											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	SGDV																							*/
/*						 (DcVolt)^2  * SBCYCLEUS * 5000  											*/
/*			�񐶓d�� = ----------------------------------- * dRegOnCount							*/
/*						(reger/1000) * SCCYCLEUS * ampw  											*/
/*--------------------------------------------------------------------------------------------------*/
/*			Pn600 : Prm.regres			�񐶒�R�e��				[10W]							*/
/*			Pn603 : Prm.exreger			�񐶒�R�l					[10m��]							*/
/*			PnE1A : Prm.reger			�񐶒�R�l(�f�t�H���g)		[m��]							*/
/*			PnE1C : Prm.wre				�񐶒�R�e��(�f�t�H���g)	[0.1%]							*/
/*			PnE72 : Prm.regonlvl		�񐶃I�����x��				[V]								*/
/*			PnE7F : Prm.ConvW			�R���o�[�^�e��				[W]								*/
/****************************************************************************************************/
void	IprmcalRegenePowerGain( REGENE_CTRL *RegeneCtrl, BPRMDAT *Bprm, REGCFG_PRM *RegPrm )
{
	LONG		kx, sx;
	LONG		Vdcsqr;
	LONG		reger;		/* �񐶒�R�l */

/*--------------------------------------------------------------------------------------------------*/
/*		�d���̌v�Z																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( Bprm->AmpDcinType == 1 )							/* �A���v�c�b�d������				*/
	{
		Vdcsqr = 0;												/* �񐶂Ȃ�							*/
	}
	else														/* �A���v�c�b�d������				*/
	{
		/* Vdcsqr = (PnE72 ^ 2) */
		Vdcsqr = RegPrm->regonlvl * RegPrm->regonlvl;
		if( Bprm->AmpInVcode == 0x02 )
		{														/* 400V���͈ȊO�F�񐶒�R�l[m��]	*/
			Vdcsqr /= 10;										/* 400V����	   �F�񐶒�R�l[10m��]	*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶒�R�l�̑I��															<S0F4>			*/
/*--------------------------------------------------------------------------------------------------*/
	if( RegPrm->regres != 0 )
	{ /* Pn600�O�t���񐶒�R�e�ʐݒ�ς� */
		if( RegPrm->exreger != 0 )
		{ /* Pn603�O�t���񐶒�R�l�ݒ�ς� */
			/* �O�t���񐶒�R�l���g�p */
			if( Bprm->AmpInVcode == 0x02 )
			{ /* 400V���͂̏ꍇ */
				reger = RegPrm->exreger;						/* �P�ʕϊ��s�v[10m��]				*/
			}
			else
			{ /* 400V���͈ȊO�̏ꍇ */
				reger = RegPrm->exreger * 10U;					/* �P�ʕϊ�[10m��]�� [m��]			*/
			}
		}
		else
		{
			/* �f�t�H���g�񐶒�R�l���g�p */
			reger = RegPrm->reger;
		}
	}
	else
	{
		/* �f�t�H���g�񐶒�R�l���g�p */
		reger = RegPrm->reger;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶓d�͉��Z�Q�C���̌v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( Bprm->SvCycleNs != KPI_SBCYCLE_BASE_NS )
	{ /* ScanB���� 125us�ȊO */
		kx = MlibScalKxgain( Vdcsqr, Bprm->SvCycleUs/2, (KPI_SCCYCLEUS * reger), &sx, 0 );
	}
	else
	{ /* ScanB���� 125us */
		kx = MlibScalKxgain( Vdcsqr, Bprm->SvCycleUs, (KPI_SCCYCLEUS * reger), &sx, 0 );
	}

	kx = MlibScalKxgain( Vdcsqr, Bprm->SvCycleUs, (KPI_SCCYCLEUS * reger), &sx, 0 );
	kx = MlibPcalKxgain( kx, (1000 * 5000), RegPrm->ConvW, &sx, 24 );
	RegeneCtrl->RegOL.conf.Krgpower = kx;
}




/****************************************************************************************************/
/*																									*/
/*		�񐶉ߕ��׃��x���̌v�Z																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �񐶉ߕ��׃`�F�b�N�p�p�����[�^�̌v�Z���s���B											*/
/*																									*/
/*			1) �񐶉ߕ��׋��e�d��																	*/
/*			2) �񐶉ߕ��׃A���[�����x��																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL	PcalRegeneOverLoadLevel( REGENE_CTRL *RegeneCtrl,USHORT regres, USHORT wre, USHORT ConvW )
{
	BOOL			PrmSetErr;
	LONG			kx, sx;
	CHECK_REGOL		*pRegOL;
//	CMN_PRMDATA		*Cprm;

	/* ���[�J���ϐ��̏����� */
	PrmSetErr = FALSE;
	pRegOL = &(RegeneCtrl->RegOL);

	/* 2010.09.02 Y.Oka (>_<) ����R���o�[�^�e�ʂ�m�邷�ׂ��Ȃ����߁A�Œ�l���� */
//	Cprm->ConvW = 8000;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׋��e�d�͂̌v�Z																	*/
/*--------------------------------------------------------------------------------------------------*/
/*	��-7,INGRAM																						*/
/*		RgolPowBase = �񐶒�R�e�� * 5000 / ConvW 		[5000/�R���o�[�^�e��]						*/
/*											(��-7,INGRAM�ɐڑ����ꂽ�R���o�[�^�̗e�ʒl)				*/
/*--------------------------------------------------------------------------------------------------*/
/*	SGDV																							*/
/*		RgolPowBase = �񐶒�R�e�� * 5000 / AmpW 		[5000/�T�[�{��i�e��]						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( regres == 0 )									/* �ݒ�l�[���̏ꍇ(Pn600)					*/
	{
		pRegOL->conf.RgolPowBase = wre;					/* �񐶒�R�e��(�f�t�H���g)					*/
	}
//	else if( (Prm->regres * 10) <= Prm->ampw )			/* �T�[�{��i�o�͈ȓ��̏ꍇ					*/
	else if( (regres * 10) <= ConvW )					/* �R���o�[�^�e�ʈȓ��̏ꍇ					*/
	{
//		pRegOL->conf.RgolPowBase = (Prm->regres * 50000) / Prm->ampw;
		pRegOL->conf.RgolPowBase = (regres * 50000) / ConvW;
	}
	else												/* �R���o�[�^�o�͈ȏ�͐ݒ�ł��Ȃ�			*/
	{
		/* �p�����[�^�ݒ�ُ� */
		PrmSetErr = TRUE;
	}


/*--------------------------------------------------------------------------------------------------*/
/*		�񐶉ߕ��׃A���[�����x���̌v�Z																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		SWPF :	RgolAlmLevel = RgolPowBase * 2 * 100 sec											*/
/*							 = RgolPowBase * 2 * 100 * 1000 / SCCYCLEMS								*/
/*																									*/
/*				if( RgolPowSum >= RgolAlmLevel ){ �񐶉ߕ��׃A���[��;}								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		���U :	rgollv = rgbas * 2 * 100 sec														*/
/*					   = rgbas * 2 * 100 * 10^6 / (CYCLE*FORCNTLMT)									*/
/*					   = 2^14 * (rgbas * 12207) / (CYCLE*FORCNTLMT);								*/
/*																									*/
/*				if( rgolsm>>14 >= rgollv ){ �񐶉ߕ��׃A���[��;}									*/
/*																									*/
/*				*1) �����ݒ� : rgolsm = rgollv << 7;   --- �������́H, 50%����J�n�ł́H			*/
/*																									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( pRegOL->conf.RgolPowBase, (2*100*1000), KPI_SCCYCLEMS, NULL, -30 );
	pRegOL->conf.RgolAlmLevel = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶏���d�͉��Z�Q�C��(for Un00A)�̌v�Z														*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*								100																	*/
/*		Kunrgpower = -------------------------------------											*/
/*					  RgolPowBase * KPI_SCANC_MS( 10000 )											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 100, 1, pRegOL->conf.RgolPowBase, &sx,  0 );
	kx = MlibPcalKxgain( kx,  1, KPI_SCANC_MS( 10000 ),    &sx, 24 );
	pRegOL->conf.Kunrgpower = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶏���d�͉��Z�Q�C��(for Un143)�̌v�Z	10[%]												*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*								100							   1									*/
/*		Kunrgpower = -------------------------------------	* -----									*/
/*					  RgolPowBase * KPI_SCANC_MS( 2 )		   10									*/
/*																									*/
/*		�I�[�o�[�t���[�΍�łP�O[%]�\���ɂ��邽�߁A�P�O�ŏ��Z����									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 10, 1, pRegOL->conf.RgolPowBase,   &sx,  24 );
	pRegOL->conf.KunrgpowerCycle = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		�񐶒�R�e�ʎ擾																			*/
/*--------------------------------------------------------------------------------------------------*/
	RegeneCtrl->conf.ResistorCapa = regres;

	return PrmSetErr;
}


/****************************************************************************************************/
/*																									*/
/*		���H���o�`�c�p�O�d�����[�p�X�t�B���^	�t�B���^���萔�p�����[�^�v�Z						*/
/*																									*/
/****************************************************************************************************/
void	PcmCalculateVdetAdFil( DET_DC_VOLT *pDetectDc, USHORT vdetadfil, LONG SvCycleUs )
{
	if( (SvCycleUs*1000) != KPI_SBCYCLE_BASE_NS )
	{ /* ScanB���� 125us�ȊO */
		pDetectDc->conf.VdetAdFil = MlibPcalKf1gain( vdetadfil, SvCycleUs, 0 );
	}
	else
	{ /* ScanB���� 125us */
		pDetectDc->conf.VdetAdFil = MlibPcalKf1gain( vdetadfil, (2 * SvCycleUs), 0 );
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�˓��d���h�~��H����^�c�a��H����										<S20E>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : ���H�[�d�V�[�P���X�A����сA�c�a�V�[�P���X����̗v�����󂯁A�˓��^�c�a������@�I��	*/
/*			�ɏ]���āA�����[�A�T�C���X�^�𐧌䂷��B												*/
/*																									*/
/*	�� �X : INGRAM�ł͓˖h�����DB����𕪗����A�˖h����̓��j�b�g���ʏ����Ƃ��Ă����B��-7�V���[�Y	*/
/*			�ɂ����Ă���-7W�������ƂȂ邪�AH/W�I�ɃC���^�[���b�N������Ă��邽�ߎ�������Ƃ��ėǂ��B*/
/*			���̂��߁A��-�X�Ɠ����̏����ɕύX����B													*/
/*																									*/
/*			TestMode5�͎����@�p�@�\�ł���A���W�X�^�A�N�Z�X�ɂ�苭���I�ɓ˖h������s�����߁A		*/
/*			�{�����ł̓˖h����͍s��Ȃ��B���l��TestMode7�͋����I��DB������s�����߂̋@�\�B			*/
/*																									*/
/*			���[�^���X�e�X�g���[�h�L�����́A�����I�ɓ˖hOFF�ADBON��ԂƂ���B						*/
/*																									*/
/****************************************************************************************************/
void	PcmRlyDbControlProcess( ASIC_HW_IF_STR *pAsicHwReg, POW_MNG_HNDL *PowerManager,
								BOOL BbSts, BOOL DbOn, BOOL KnlMode, BOOL MotorLessTestMode )
{
	CHECK_MAIN_POWER	*PowerChecker;
	POWER_STATUS		*pPowSts;
	BOOL				RelaySts;
	BOOL				DbSts;

	PowerChecker = &PowerManager->MainPowerChecker;
	pPowSts = &PowerManager->PowerSts;
	RelaySts = FALSE;
	DbSts = TRUE;

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�˓���H����^�c�a��H����															*/
/*--------------------------------------------------------------------------------------------------*/
	switch( PowerChecker->conf.RlyDbWay )
	{
	case DBCTRL_RLY_COMMON:				/* �˓��{�c�a�F�����[���� 									*/
		if( (BbSts == TRUE)
			&& ((pPowSts->ChargeOk == FALSE) || ((KnlMode != KPI_KMD_ADJCURAD) && (DbOn == TRUE))) )
		{ /* BB && (�[�d���� || (!�d��AD������ && DBON�v������) ) */
			RelaySts = FALSE;			/* Relay OFF / DB ON */
		}
		else
		{
			RelaySts = TRUE;			/* Relay ON / DB OFF */
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	���L�p�����[�^�ݒ莞�̓p�����[�^�v�Z�Ńp�����[�^�ݒ�ُ�Ƃ���								*/
	/*	���˓��{�c�a�F�����[����																	*/
	/*	���˓��{�c�a:�����[+�T�C���X�^																*/
	/*----------------------------------------------------------------------------------------------*/
	case DBCTRL_TRIACK_SEPARATE:		/* �˓��F�����[�G�c�a�F�T�C���X�^ 							*/
	case DBCTRL_RLY_SEPARATE:			/* �˓��F�����[�G�c�a�F�����[ 								*/
		/* �˖h���� */
		if( (BbSts == TRUE) && (pPowSts->ChargeOk == FALSE) )
		{ /* BB && �[�d���� */
			RelaySts = FALSE;			/* Relay OFF */
		}
		else
		{ /* BE || �[�d���� */
			RelaySts = TRUE;			/* Relay ON */
		}

		/* DB���� */
		if( (BbSts == TRUE) && ( (KnlMode != KPI_KMD_ADJCURAD) && (DbOn == TRUE)) )
		{ /* BB && (!�d��AD������ && DBON�v������) */
			DbSts = TRUE;				/* DB ON */
		}
		else
		{
			DbSts = FALSE;				/* DB OFF */
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	�˓��{�c�a�F�T�C���X�^�̏ꍇ�ADBON�Ńp���X�o�͂𐧌�ARelayOn�œ˖h or DB��ON����d�g�݁B	*/
	/*	��DBOFF				�F�p���X�o�͂��Ȃ����߁ARelayOn/Off�ɂ�����炸�˖hOFF/DBON				*/
	/*	��DBON && RelayOn	�F�p���X�o�͂���A�˖hON/DBOFF											*/
	/*	��DBON && RelayOff	�F�p���X�o�͂���A�˖hOFF/DBON											*/
	/*----------------------------------------------------------------------------------------------*/
	case DBCTRL_TRIACK_COMMON:			/* �˓��{�c�a�F�T�C���X�^ 									*/
		if( !((BbSts == TRUE)
			  && ((pPowSts->ChargeOk == FALSE) || ((KnlMode != KPI_KMD_ADJCURAD) && (DbOn == TRUE)))) )
		{
			RelaySts = TRUE;			/* Relay ON */
			DbSts = TRUE;				/* Pulse Output */
		}
		else if( DbOn == TRUE )
		{
			RelaySts = FALSE;			/* Relay OFF */
			DbSts = TRUE;				/* Pulse Output */
		}
		else
		{
			RelaySts = FALSE;			/* Relay OFF */
			DbSts = FALSE;				/* Pulse Stop */
		}
		break;

	default:
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	���[�^���X�e�X�g���[�h�L�����́A�����I�ɓ˖hOFF�ADBON��ԂƂ���B							*/
	/*----------------------------------------------------------------------------------------------*/
	if( MotorLessTestMode == TRUE )
	{ /* ���[�^���X�L���� */
		RelaySts = FALSE;				/* Relay OFF */
		DbSts = TRUE;					/* DB ON */
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�˖h��H & DB��H�o�͐ݒ�																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( PowerChecker->conf.TestMode5 == FALSE )
	{
		if( ((PowerChecker->conf.RlyDbWay == DBCTRL_TRIACK_COMMON) || (PowerChecker->conf.RlyDbWay == DBCTRL_RLY_COMMON))
			&& (PowerChecker->conf.TestMode7 == TRUE) )
		{ /* �˓��{�c�a�F�����[���ʂ̏ꍇ�ADB����ɓ˖h����M�����g�p���� */
			; /* DB�����o�͒��͉������Ȃ� */
		}
		else
		{
			HALdrv_RelayControl( (void*)pAsicHwReg, RelaySts );	/* Relay ON/OFF */
		}
	}

//	if( (PowerChecker->conf.TestMode7 == FALSE) && (CSW_HW_DB_CTRL == TRUE) )
	if((( PowerChecker->conf.TestMode7 == FALSE )&&( CSW_HW_DB_CTRL == TRUE ))&&
									( PowerChecker->conf.RlyDbWay != DBCTRL_RLY_COMMON ))	/* <S212> */
	{
		if( (PowerChecker->conf.RlyDbWay == DBCTRL_TRIACK_COMMON)
			&& (PowerChecker->conf.TestMode5 == TRUE) )
		{ /* �˓��{�c�a�F�T�C���X�^�̏ꍇ�A�˖h�����DBON�M�����g�p���� */
			; /* �˖h���䋭���o�͒��͉������Ȃ� */
		}
		else
		{
			if( DbSts == TRUE )
			{
				if( PowerChecker->conf.RlyDbWay == DBCTRL_TRIACK_COMMON )
				{
					KlibWaitus( WAIT_RLY_DB );
				}
				SHalDrv_DynamicBrakeON( pAsicHwReg );			/* DBON */
			}
			else
			{
				SHalDrv_DynamicBrakeOFF( pAsicHwReg );			/* DBOFF */
			}
		}
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		���ԓd�ʈُ�`�F�b�N����(A.450)	<S053>														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �S�O�O�u���T�[�{�p�b�N�̎��H�R���f���T�̒��ԓd�ʂ����ꂽ���������M���i�o�b�n�u�j		*/
/*			���󂯂āA�M���n�m��PnE7A.2[100ms]�ԑ������ꍇA.450�u���H�R���f���T�ߓd���v�Ƃ���B	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	LpxCheckPcovAlarm( PCOV_ERR_DETECT *PcovErrDetect, POWER_STATUS *PowSts )
{
		if( PcovErrDetect->conf.PcovEna == TRUE )
		{
			if( (PcovErrDetect->var.Pcov == TRUE) && (PowSts->PowerOn == TRUE) )
			{
				if( PcovErrDetect->var.PcovErrCnt >= PcovErrDetect->conf.PcovErrCntLmt )
				{
					ALMSetGlobalAlarm( ALM_PCOV );		/* A.450 : ���H�R���f���T�ߓd���A���[��	*/
				}
				else
				{
					PcovErrDetect->var.PcovErrCnt++;
				}
			}
			else
			{
				PcovErrDetect->var.PcovErrCnt = 0;
			}
		}
		return;
}



/****************************************************************************************************/
/*																									*/
/*		DC�d���擾�pAPI		<S020>																	*/
/*																									*/
/****************************************************************************************************/
LONG	PcmGetDcVoltage( POWER_STATUS *PowSts )
{
	return	PowSts->DcVolt;
}
/***************************************** end of file **********************************************/
