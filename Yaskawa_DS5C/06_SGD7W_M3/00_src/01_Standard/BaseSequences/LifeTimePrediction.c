/*****************************************************************************
Description		: �����\���@�\����
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --

	LifeTimeSurvivorCalculateMain() :�����c�����v�ZMain�֐�
	LifeTimeSwithingCountInit()		:��������񐔏������֐�
	AdetCheckPreWarningSigOut()		:�\�h�ۑS���[�j���O�E�\�h�ۑS�M���o��
	AvgInternalTemperatureCalInit()	:���ϓ������x�Z�o����������
	AvgInternalTemperatureCal()		:���ϓ������x�Z�o����

	-- Locals --
	LifeTimeFanSurvivorCalculate()	:FAN�����c�����v�Z�֐�
	LifeTimeCapaSurvivorCalculate()	:�R���f���T�����c�����v�Z�֐�
	LifeTimeRushSurvivorCalculate()	:�˖h�����c�����v�Z�֐�
	LifeTimeDBSurvivorCalculate()	:DB�����c�����v�Z�֐�
	LifeTimeRushSwitchingCount()	:�˖h�����[����񐔂͏[�d�������� �J�E���g�A�b�v����
	LifeTimeDBSwitchingCount()		:DB�����[����񐔂̓��[�^��]����DB�I�������ꍇ�̂݃J�E���g�A�b�v����
	LifeTimePrmStoreValueToEeprom() :�����@�\�p�p�����[�^��������To�@Eeprom

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.08.25		created

*****************************************************************************/

#include "LifeTimePredictionApi.h"

#define FAN_LIFE_CALC_BASE				26280.0F		/* FAN�����v�Z�v�f */
#define FAN_LIFE_TEMPERATURE_BASE		70.0F			/* FAN���x�v�Z�v�f */
#define CAPA_LIFE_CALC_BASE				50000.0F		/* �R���f���T�����v�Z�v�f */
#define CAPA_LIFE_TEMPERATURE_BASE		105.0F			/* �R���f���T���x�v�Z�v�f */
#define CONVERT_100MS					(10.0F * 60.0F * 60.0F)	/* (10*60*60) = 36000 1hour�P�ʂ���100ms�P�ʂɊ��Z����v�f */
#define LIFE_MONITOR_CALC_FACTOR		10000.0F 		/*���j�^�P�ʂ�0.01%�ׁA10000�{����*/
#define LIFE_TIME_SURVIVOR_MAX			10000.0F 		/* �����c���� ����l *//* <S192> */

const float  LifeTimeCalTbl[10] =
{
	1.0F,		/*  2^(0/10) */
	1.07F,		/*  2^(1/10) */
	1.15F,		/*  2^(2/10) */
	1.23F,		/*  2^(3/10) */
	1.31F,		/*  2^(4/10) */
	1.41F,		/*  2^(5/10) */
	1.51F,		/*	2^(6/10) */
	1.62F,		/*  2^(7/10) */
	1.74F,		/*	2^(8/10) */
	1.87F,		/*	2^(9/10) */
};


/****************************************************************************************************/
/*																									*/
/*		API�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
void LifeTimeSurvivorCalculateMain( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );	/* �����c�����v�ZMain�֐�		*/
void LifeTimeSwithingCountInit( LIFETIME_HANDLE *hLifeTime );						/* ��������񐔏������֐�		*/
//LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker );		/* <S180> */
LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker, float DBSurvivor );	/* <S180> */
void AvgInternalTemperatureCalInit( AVGTMPR_HANDLE *hAvgTmprHdl, ULONG TimerCnt, SHORT InternalTmpr );
//LONG AvgInternalTemperatureCal(EEP_DEVICE_HANDLE *EepDevice, AVGTMPR_HANDLE *hAvgTmprHdl,		/* <S192> */
//										SHORT CurrentTmpr, ULONG CurrentTimerCnt );
LONG AvgInternalTemperatureCal(EEP_QUE_HANDLE *EepQue, AVGTMPR_HANDLE *hAvgTmprHdl,				/* <S192> */
								SHORT CurrentTmpr, ULONG CurrentTimerCnt );
//void LifeTimeDBSwitchingCount( LIFETIME_HANDLE *hLifeTime , BOOL TgOnSts, BOOL CurrentDbOn );	/* DB�����[����񐔂̓��[�^��]����DB�I�������ꍇ�̂݃J�E���g�A�b�v����*//* <S180> */
void LifeTimeDBSwitchingCount( DBLIFETIME_HADNLE *hDBLifeTime, BOOL TgOnSts, BOOL CurrentDbOn );				/* <S180> */
//void LifeTimePrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, CMN_PRMDATA *Cprm );		/* <S180> *//* <S186> *//* <S192> */
void LifeTimeDBPrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );		/* <S192> */
void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeDBSwithingCountInit( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeRushPrmStoreValueToEeprom( RUSHLIFETIME_HANDLE	*hRushLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );	/* <S192> */

/****************************************************************************************************/
/*																									*/
/*		Local�֐��ꗗ																				*/
/*																									*/
/****************************************************************************************************/
static void LifeTimeFanSurvivorCalculate( LIFETIME_HANDLE *hLifeTime );		/* FAN�����c�����v�Z�֐� 		*/
static void LifeTimeCapaSurvivorCalculate( LIFETIME_HANDLE *hLifeTime );	/* �R���f���T�����c�����v�Z�֐�	*/
static void LifeTimeRushSurvivorCalculate( LIFETIME_HANDLE *hLifeTime );	/* �˖h�����c�����v�Z�֐�		*/
//static void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime );		/* DB�����c�����v�Z�֐�		*//* <S180> */
//static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime );		/* �˖h�����[����񐔂͏[�d�������� �J�E���g�A�b�v��	*//* <S180> */
static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );		/* <S180> */
//static void LifeTimePrmStoreValueToEeprom( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm ); /* �����@�\�p�p�����[�^��������To�@Eeprom *//* <S180> */

/*****************************************************************************
  Description:	�˖h�����[����񐔂͏[�d�������� �J�E���g�A�b�v����
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
	RegCfg;					Regsiter Manager
	Cprm;					���j�b�g���ʃp�����[�^�\����
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime )		/* <S180> */
static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )	/* <S180> */
{

	if ( hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk == TRUE )										/* �[�d���� */
	{
		if ( hLifeTime->RushLifeTimeHdl.var.LastChargeOk ==  FALSE)		/* �O��[�d�͖��������	*/
		{
			hLifeTime->RushLifeTimeHdl.var.RushSwitchCount ++;
			/* <S180> Start */
			/* ���݂̓˖h�����[����� �̒l��Eeprom�ɏ������� */
//			Cprm->RushSwichCount = hLifeTime->RushLifeTimeHdl.var.RushSwitchCount;		/* <S192> */
//			PrmStoreValueToEeprom( RegCfg, &pndef_RushSwichCount, 0 );					/* <S192> */
//			hLifeTime->RushLifeTimeHdl.var.RushCountChanged = TRUE;
			/* <S180> End */
		}
	}
	hLifeTime->RushLifeTimeHdl.var.LastChargeOk = hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk;
}

/*****************************************************************************
  Description:	DB�����[����񐔂̓��[�^��]����DB�I�������ꍇ�̂݃J�E���g�A�b�v����
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;				DB�����\���@�\�n���h��
	TgOnSts						���[�^��]���
	CurrentDbOn					DB ON Flag
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if 0		/* <S180> */
void LifeTimeDBSwitchingCount( LIFETIME_HANDLE *hLifeTime , BOOL TgOnSts, BOOL CurrentDbOn )
{
	if ( TgOnSts == TRUE )				/* ���[�^��]�� */
	{
		/*�O��̏�Ԃ� DB�I�t������̏�Ԃ�DB�I���̏ꍇ*/
		if ( (hLifeTime->DBLifeTimeHdl.var.LastDbOn == FALSE) && (CurrentDbOn == TRUE ) )
		{
			hLifeTime->DBLifeTimeHdl.var.DBSwitchCount ++;
			hLifeTime->DBLifeTimeHdl.var.DBCountChanged = TRUE ;
		}
	}
	hLifeTime->DBLifeTimeHdl.var.LastDbOn = CurrentDbOn;
}
#else		/* <S180> */
void LifeTimeDBSwitchingCount( DBLIFETIME_HADNLE *hDBLifeTime, BOOL TgOnSts, BOOL CurrentDbOn )
{
	if ( TgOnSts == TRUE )				/* ���[�^��]�� */
	{
		/*�O��̏�Ԃ� DB�I�t������̏�Ԃ�DB�I���̏ꍇ*/
		if ( (hDBLifeTime->var.LastDbOn == FALSE) && (CurrentDbOn == TRUE ) )
		{
			hDBLifeTime->var.DBSwitchCount ++;
//			hDBLifeTime->var.DBCountChanged = TRUE ;	/* <S192> */
		}
	}
	hDBLifeTime->var.LastDbOn = CurrentDbOn;
}
#endif
/*****************************************************************************
  Description:	DB�����[���쓮��񐔏������֐�
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
	hDBLifeTime;			DB�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void LifeTimeSwithingCountInit( LIFETIME_HANDLE *hLifeTime )
{
//	hLifeTime->DBLifeTimeHdl.var.LastDbOn = FALSE;	/* <S180> */
	hLifeTime->RushLifeTimeHdl.var.LastChargeOk = FALSE;
}
/* <S180> Start */
/*****************************************************************************
  Description:	DB����񐔏������֐�
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;			DB�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void LifeTimeDBSwithingCountInit( DBLIFETIME_HADNLE *hDBLifeTime )
{
	hDBLifeTime->var.LastDbOn = FALSE;
}
/* <S180> End */

/*****************************************************************************
  Description:	�����c�����v�ZMain�֐�
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void LifeTimeSurvivorCalculateMain( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )
{
//	LifeTimeRushSwitchingCount( hLifeTime );			/* �˖h�����[����񐔂͏[�d�������� �J�E���g�A�b�v����	*//* <S180> */
	LifeTimeRushSwitchingCount( hLifeTime, RegCfg, Cprm );		/* �˖h�����[����񐔂͏[�d�������� �J�E���g�A�b�v����	*//* <S180> */
	LifeTimeFanSurvivorCalculate( hLifeTime );			/* FAN�����c�����v�Z  */
	LifeTimeCapaSurvivorCalculate( hLifeTime );			/* �R���f���T�����c�����v�Z */
	LifeTimeRushSurvivorCalculate( hLifeTime );			/*�˖h�����c�����v�Z  */
//	LifeTimeDBSurvivorCalculate( hLifeTime );			/* DB�����c�����v�Z *//* <S180> */
//	LifeTimePrmStoreValueToEeprom( hLifeTime , RegCfg , Cprm ); /* �����@�\�p�p�����[�^��������TO�@Eeprom�@*//* <S180> */
}

/*****************************************************************************
  Description:	FAN�����c�����v�Z�֐�
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimeFanSurvivorCalculate( LIFETIME_HANDLE *hLifeTime )
{
	float FanLifeTime;				/* ����FAN����		*/
	float FanTemperatureOffset;     /* ���͉��x�I�t�Z�b�g 	*/
	float AroundTemperature;		/*	���͉��x			*/
	float AvgIntTmpr;				/* ���ϓ������x		*/
	float FanTimeOffset;			/* FAN�����I�t�Z�b�g����*/
	float Timer100msCounter;
	float val;
	float ftmp;
	ULONG tmp1;
	ULONG tmp2;
	AvgIntTmpr = (float)hLifeTime->AvgTmprHdl.var.AvgIntTmpr * 0.01F ;			/* �P�ʂ́��@*/
	FanTemperatureOffset = hLifeTime->FanLifeTimeHdl.conf.FanTemperatureOffset;
	AroundTemperature = AvgIntTmpr - FanTemperatureOffset;						/* ���͉��x �F���ϓ������x - ���͉��x�I�t�Z�b�g*/
	Timer100msCounter = (float)KlibGetTimeStamp();
	if ( AroundTemperature < 0.0F)
	{
		AroundTemperature = 0.0F;
	}

	tmp1 = (ULONG)(( FAN_LIFE_TEMPERATURE_BASE - AroundTemperature ) * 0.1F ); 						/* k = (70 - x) / 10 x�F���ϓ������x - ���͉��x�I�t�Z�b�g				*/
	tmp2 = (ULONG)(( FAN_LIFE_TEMPERATURE_BASE - AroundTemperature ) - (float)(tmp1 * 10U));		/* tmp2 = (70 - x) - (tmp1 * 10) �؂�̂Ă������_�ȉ�					*/
	ftmp = LifeTimeCalTbl[tmp2];																	/* �� 2^(0/10) �` 2^(10/10)�̒l���������z��					*/
	FanLifeTime = (float)(( FAN_LIFE_CALC_BASE * ( 1 << tmp1 )) * ftmp )* CONVERT_100MS;			/* ����FAN���� = 26280 * (1<<tmp1) * ftmp 	�P�ʂ�100ms				*/
	/* �v�Z���F�l�P��(100ms) = FanTimeOffset(�P��10hour) * 10 *CONVERT_100MS (1hour�P�ʂ���100ms�P�ʂɊ��Z����v�f) 	*/
	FanTimeOffset =  hLifeTime->FanLifeTimeHdl.conf.FanTimeOffset  * 10.0F * CONVERT_100MS; 		/* �P�ʂ�10h����100ms�֕ϊ� */


	if ( Timer100msCounter > FanTimeOffset )
	{
#if 0	/* <S192> */
		/* ����FAN�c���� = ((�ݐωғ����� - FAN�����I�t�Z�b�g����) / ����FAN����) �~ 100*/
		val = (((Timer100msCounter - FanTimeOffset) ) / FanLifeTime) * LIFE_MONITOR_CALC_FACTOR ;
		if (val >= 65535.0F)
		{
			hLifeTime->FanLifeTimeHdl.var.FanSurvivor = 65535.0F;
		}
		else
		{
			hLifeTime->FanLifeTimeHdl.var.FanSurvivor = val ;     			/* ����FAN�����c���� = ((�ݐωғ����� - FAN�����I�t�Z�b�g���� / ����FAN����) �~ 100) */
		}
#else	/* <S192> */
		/* ����FAN�����c���� = 100 - ((�ݐωғ����� - FAN�����I�t�Z�b�g���� / ����FAN����) �~ 100) */
		val = (((Timer100msCounter - FanTimeOffset) ) / FanLifeTime) * LIFE_MONITOR_CALC_FACTOR ;
		hLifeTime->FanLifeTimeHdl.var.FanSurvivor = LIFE_TIME_SURVIVOR_MAX - val;
		if ( hLifeTime->FanLifeTimeHdl.var.FanSurvivor < 0.0F )
		{
			hLifeTime->FanLifeTimeHdl.var.FanSurvivor = 0.0F;
		}
#endif
	}
	else
	{
//		hLifeTime->FanLifeTimeHdl.var.FanSurvivor = 0.0F;					/* <S192> */
		hLifeTime->FanLifeTimeHdl.var.FanSurvivor = LIFE_TIME_SURVIVOR_MAX;	/* <S192> */
	}
}

/*****************************************************************************
  Description:	�R���f���T�����c�����v�Z�֐�
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimeCapaSurvivorCalculate( LIFETIME_HANDLE *hLifeTime )
{
	float CapaTime;										/* �R���f���T���� */
	float AvgIntTmpr;									/* ���ϓ������x		*/
	float val;
	float Timer100msCounter;
	float ftmp;
	ULONG tmp1;
	ULONG tmp2;
	AvgIntTmpr = (float)hLifeTime->AvgTmprHdl.var.AvgIntTmpr * 0.01F ;							/* �P�ʂ͂P���@*/
	Timer100msCounter = (float)KlibGetTimeStamp();

	tmp1 = (ULONG)(( CAPA_LIFE_TEMPERATURE_BASE - AvgIntTmpr ) * 0.1F );						/* k = (105 - ���ϓ������x) / 10 									*/
	tmp2 = (ULONG)(( CAPA_LIFE_TEMPERATURE_BASE - AvgIntTmpr ) - (float)( tmp1 * 10U ));		/* tmp2 = (105 - x) - (tmp1 * 10) �؂�̂Ă������_�ȉ�				*/
	ftmp = LifeTimeCalTbl[tmp2];																/* �� 2^(0/10) �` 2^(10/10)�̒l���������z��				*/
	CapaTime = (float)( CAPA_LIFE_CALC_BASE * ( 1 << tmp1 ) * ftmp ) * CONVERT_100MS;  			/* �R���f���T���� = 50000 �~ (1<<tmp1) * ftmp  �P�ʂ�100MS			*/
#if 0	/* <S192> */
	/* �����R���f���T�����c���� = ((�ݐωғ����� / �R���f���T����) �~ 100) */
	val = ( Timer100msCounter / CapaTime ) * LIFE_MONITOR_CALC_FACTOR ;
	if ( val >= 65535.0F )
	{
		hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor =  65535.0F ;
	}
	else
	{
		hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor = val;
	}
#else	/* <S192> */
	/* �����R���f���T�����c���� = 100 - ((�ݐωғ����� / �R���f���T����) �~ 100) */
	val = ( Timer100msCounter / CapaTime ) * LIFE_MONITOR_CALC_FACTOR ;
	hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor = LIFE_TIME_SURVIVOR_MAX - val;
	if ( hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor < 0.0F )
	{
		hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor =  0.0F ;
	}
#endif
}

/*****************************************************************************
  Description:	�˖h�����c�����v�Z�֐�
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimeRushSurvivorCalculate( LIFETIME_HANDLE *hLifeTime )
{
	float val;
#if 0	/* <S192> */
	/* �g�p��[%] = ((���݂̓����(PnE84) / ������(PnE82)) * 100) */
	val =  (float)hLifeTime->RushLifeTimeHdl.var.RushSwitchCount * hLifeTime->RushLifeTimeHdl.conf.RushLifeTimeReciprocal * LIFE_MONITOR_CALC_FACTOR ;
	if ( val  >= 65535.0F  )
	{
		hLifeTime->RushLifeTimeHdl.var.RushSurvivor =   65535.0F  ;
	}
	else
	{
		hLifeTime->RushLifeTimeHdl.var.RushSurvivor = val;
	}
#else	/* <S192> */
	/* �˖h�����c���� = 100 - ((���݂̓����(PnE84) / ������(PnE82)) * 100) */
	val =  (float)hLifeTime->RushLifeTimeHdl.var.RushSwitchCount *
			hLifeTime->RushLifeTimeHdl.conf.RushLifeTimeReciprocal * LIFE_MONITOR_CALC_FACTOR ;
	hLifeTime->RushLifeTimeHdl.var.RushSurvivor = LIFE_TIME_SURVIVOR_MAX - val;
	if ( hLifeTime->RushLifeTimeHdl.var.RushSurvivor < 0.0F )
	{
		hLifeTime->RushLifeTimeHdl.var.RushSurvivor = 0.0F;
	}
#endif
}

/*****************************************************************************
  Description:	DB�����c�����v�Z�֐�
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;			DB�����\���@�\�n���h��
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//static void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime )		/* <S180> */
void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime )				/* <S180> */
{
	float val;
#if 0	/* <S192> */
	/* �g�p��[%] = ((���݂̓����(PnE86) / ������(PnE84)) * 100) */
	val = (float)hDBLifeTime->var.DBSwitchCount * hDBLifeTime->conf.DBLifeTimeReciprocal * LIFE_MONITOR_CALC_FACTOR ;
	if ( val  >= 65535.0F  )
	{
		hDBLifeTime->var.DBSurvivor =  65535.0F ;
	}
	else
	{
		hDBLifeTime->var.DBSurvivor = val;
	}
#else	/* <S192> */
	/* DB�����c���� = 100 - ((���݂̓����(PnE86) / ������(PnE84)) * 100) */
	val = (float)hDBLifeTime->var.DBSwitchCount *
			hDBLifeTime->conf.DBLifeTimeReciprocal * LIFE_MONITOR_CALC_FACTOR ;
	hDBLifeTime->var.DBSurvivor = LIFE_TIME_SURVIVOR_MAX - val;
	if ( hDBLifeTime->var.DBSurvivor < 0.0F )
	{
		hDBLifeTime->var.DBSurvivor = 0.0F;
	}
#endif
}
/* <S192> Start */
/*****************************************************************************
  Description:	 �˖h�����[����񐔗p�p�����[�^��������TO�@Eeprom
------------------------------------------------------------------------------
  Parameters:
	hRushLifeTime;			�˖h�����\���@�\�n���h��
	RegCfg;					Regsiter Manager
	Prm;					�p�����[�^�f�[�^
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void LifeTimeRushPrmStoreValueToEeprom( RUSHLIFETIME_HANDLE	*hRushLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )
{
	hRushLifeTime->var.RushSwCountLatch = hRushLifeTime->var.RushSwitchCount;
	if ( Cprm->RushSwichCount !=  hRushLifeTime->var.RushSwCountLatch )		/*  �˖h�����[����񐔕ύX�����ꍇ	*/
	{
		/* ���݂̓˖h�����[����� �̒l��Eeprom�ɏ������� */
		Cprm->RushSwichCount = hRushLifeTime->var.RushSwCountLatch;
		PrmStoreValueToEeprom( RegCfg, &pndef_RushSwichCount, 0 );
	}
}
/* <S192> End */
/* <S180> Start */
/*****************************************************************************
  Description:	 DB�����[����񐔗p�p�����[�^��������TO�@Eeprom
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;			DB�����\���@�\�n���h��
	RegCfg;					Regsiter Manager
	Prm;					�p�����[�^�f�[�^
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//void LifeTimePrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm )		/* <S186> *//* <S192> */
void LifeTimeDBPrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm )
{
//	if ( hDBLifeTime->var.DBCountChanged == TRUE )			/* DB�����[����񐔕ύX�����ꍇ	*/
	hDBLifeTime->var.DBSwCountLatch = hDBLifeTime->var.DBSwitchCount;	/* <S192> */
	if ( Prm->DBSwichCount != hDBLifeTime->var.DBSwCountLatch )			/* <S192> */
	{
		/* ���݂̌��݂�DB����� �̒l��Eeprom�ɏ�������*/
//		Cprm->DBSwichCount = hDBLifeTime->var.DBSwitchCount;		/* <S186> *//* <S192> */
		Prm->DBSwichCount = hDBLifeTime->var.DBSwCountLatch;		/* <S192> */
		PrmStoreValueToEeprom( RegCfg, &pndef_DBSwichCount, 0 );
	}
//	hDBLifeTime->var.DBCountChanged = FALSE;		/* <S192> */
}
#if 0
/*****************************************************************************
  Description:	 �����@�\�p�p�����[�^��������TO�@Eeprom
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
	RegCfg;					Regsiter Manager
	Cprm;					���j�b�g���ʃp�����[�^�\����
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimePrmStoreValueToEeprom( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )
{
	if ( hLifeTime->DBLifeTimeHdl.var.DBCountChanged == TRUE )			/* DB�����[����񐔕ύX�����ꍇ	*/
	{
		Cprm->DBSwichCount = hLifeTime->DBLifeTimeHdl.var.DBSwitchCount & 0xFFFF;
		PrmStoreValueToEeprom( RegCfg, &pndef_DBSwichCount, 0 );		/* ���݂̌��݂�DB����񐔁i����16bit�j �̒l��Eeprom�ɏ�������*/
	}
	if ( hLifeTime->RushLifeTimeHdl.var.RushCountChanged == TRUE )		/* �˖h�����[����񐔕ύX�����ꍇ 	*/
	{
		Cprm->RushSwichCount = hLifeTime->RushLifeTimeHdl.var.RushSwitchCount & 0xFFFF;
		PrmStoreValueToEeprom( RegCfg, &pndef_RushSwichCount, 0 );		/* ���݂̓˖h�����[����񐔁i����16bit�j �̒l��Eeprom�ɏ�������*/
	}

	if (hLifeTime->RushLifeTimeHdl.var.RushCountChanged == TRUE || hLifeTime->DBLifeTimeHdl.var.DBCountChanged == TRUE )
	{
		Cprm->DRSwithingCount = (( hLifeTime->RushLifeTimeHdl.var.RushSwitchCount & 0x00FF0000 ) >> 16 )
													+ ( hLifeTime->DBLifeTimeHdl.var.DBSwitchCount & 0x00FF0000 >> 8 );
		PrmStoreValueToEeprom( RegCfg, &pndef_DRSwithingCount, 0 );		/* ���݂̓˖h/DB�����[����񐔁i��ʁj�@ �̒l��Eeprom�ɏ�������*/
	}
	hLifeTime->RushLifeTimeHdl.var.RushCountChanged = FALSE;
	hLifeTime->DBLifeTimeHdl.var.DBCountChanged = FALSE;
}
#endif
/* <S180> End */
/*****************************************************************************
  Description: �\�h�ۑS���[�j���O�E�\�h�ۑS�M���o��
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				�����\���@�\�n���h��
	PowerChecker;			���H�Ď��p�\���̒�`
  Return:
	RC						�߂�l
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker )	/* <S180> */
LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker, float DBSurvivor )	/* <S180> */
{
	LONG	RC;
	RC = CHK_PRE_NO_OPERATION;


	/* ��pFAN�A�R���f���T�A�˖h��H�ADB��H�̂����ꂩ�̎�����臒l(90%)�𒴂����� */
#if 0	/* <S192> */
	if( ( hLifeTime->FanLifeTimeHdl.var.FanSurvivor > 9000.0F ) ||
		( hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor > 9000.0F ) ||
		( hLifeTime->RushLifeTimeHdl.var.RushSurvivor > 9000.0F ) ||
		( DBSurvivor > 9000.0F ) )		/* <S180> */
//		( hLifeTime->DBLifeTimeHdl.var.DBSurvivor > 9000.0F ) )		/* <S180> */
#else	/* <S192> */
	if( ( hLifeTime->FanLifeTimeHdl.var.FanSurvivor < 1000.0F ) ||
		( hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor < 1000.0F ) ||
		( hLifeTime->RushLifeTimeHdl.var.RushSurvivor < 1000.0F ) ||
		( DBSurvivor < 1000.0F ) )
#endif
	{
		if( PowerChecker->conf.PreWarningSw == TRUE )
		{
			RC = CHK_PRE_SET_WRN;
		}
		PowerChecker->var.PreSigOut = TRUE;				/* �\�h�ۑS�M���o�� */
	}
	else
	{
		PowerChecker->var.PreSigOut = FALSE;
	}
	return( RC );
}


/*****************************************************************************
  Description: ���ϓ������x�Z�o����������
------------------------------------------------------------------------------
  Parameters:
	hAvgTmprHdl			Average Internal Temperature Cal. Hnadle
	TimerCnt			�ғ�����
	InternalTmpr		���ϓ������x[0.01��]�l
  Return:

------------------------------------------------------------------------------
  Note:
*****************************************************************************/
void AvgInternalTemperatureCalInit( AVGTMPR_HANDLE *hAvgTmprHdl, ULONG TimerCnt, SHORT InternalTmpr )
{
	/*���ϓ������x [0.01��]�ϐ�������*/
	hAvgTmprHdl->var.AvgIntTmpr = InternalTmpr;

	/*�O��X�V���̉ғ����ԏ�����*/
	hAvgTmprHdl->var.Timer100msCntLast = TimerCnt;

	return;
}


/*****************************************************************************
  Description: ���ϓ������x�Z�o����
------------------------------------------------------------------------------
  Parameters:
	EepDevice			Eeprom Device handle
	hAvgTmprHdl			Average Internal Temperature Cal. Hnadle
	CurrentTmpr			���݉��x��� [��]
	CurrentTimerCnt		���݉ғ�����
  Return:
	Error
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
//LONG AvgInternalTemperatureCal(EEP_DEVICE_HANDLE *EepDevice, AVGTMPR_HANDLE *hAvgTmprHdl,	/* <S192> */
LONG AvgInternalTemperatureCal(EEP_QUE_HANDLE *EepQue, AVGTMPR_HANDLE *hAvgTmprHdl,			/* <S192> */
								SHORT CurrentTmpr, ULONG CurrentTimerCnt )
{
	LONG	lwk1;
	LONG	lwk2;
	SHORT	CurrentAvgTmpr;		/* �V�g�p���ϓ������x */
	LONG	rslt;

	rslt = 0;

	/* ���݉��x���̒P�ʂ�ύX����u0.1���v�|���u0.01���v�@*/
	CurrentTmpr = CurrentTmpr * 10;

	/* �g�p���ϓ������x�Z�o */
	lwk1 = (LONG)( hAvgTmprHdl->var.Timer100msCntLast >> 14 );		/* ���݂܂ł̕��ω� */
	lwk2 = ( hAvgTmprHdl->var.AvgIntTmpr * lwk1 ) + CurrentTmpr;	/* ���݂܂ł̍��v�l */
	CurrentAvgTmpr = (SHORT)( lwk2 / (lwk1 + 1) );					/* �V�g�p���ϓ������x */

	/* ���ω��xEEPROM�������� */
	if ( CurrentAvgTmpr != hAvgTmprHdl->var.AvgIntTmpr )
	{
		/* ���ϓ������x�ϐ��̒l���X�V���� */
		hAvgTmprHdl->var.AvgIntTmpr = CurrentAvgTmpr;

		/* ���ϓ������x�̒l��EEPROM�֏�������  */
//		rslt = EepdevWriteNoSumValues( 0,							/* <S192> *//* <S1D2> */
//									   EEP_NOSUMCMN_ADDRESS(EEP_AVRINTERTMPR_OFS),
//									   (USHORT*)&CurrentAvgTmpr, 1 );
		rslt = EepdevPutQueue( EepQue, 0, EEP_NOSUMCMN_ADDRESS(EEP_AVRINTERTMPR_OFS),	/* <S192> */
								(USHORT)CurrentAvgTmpr, 1 );
	}

	/* �O��X�V���̉ғ����� = ���݉ғ����� */
	hAvgTmprHdl->var.Timer100msCntLast = CurrentTimerCnt;

	return rslt;
}








/***************************************** end of file **********************************************/


