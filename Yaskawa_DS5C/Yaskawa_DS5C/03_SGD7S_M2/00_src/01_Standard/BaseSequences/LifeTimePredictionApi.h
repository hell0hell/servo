/*****************************************************************************
Description		: �����\���@�\�w�b�h�t�B�[��
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.08.24		created

*****************************************************************************/

#ifndef LIFE_TIME_API_H
#define LIFE_TIME_API_H

#include "EepromIF.h"
#include "PowerManager.h"
#include "PnPrmListTbl.h"
#include "BaseEnableSequence.h"
#include "UnitPrmStruct.h"

/*--------------------------------------------------------------------------------------------------*/
/*		�\�h�ۑS�@�\�p��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_PRE_NO_OPERATION	0x00			/* �A���[���Ȃ�(�����s�v)							*/
#define CHK_PRE_SET_WRN		0x01				/* �S���A���[���o��									*/

/*--------------------------------------------------------------------------------------------------*/
/*		FAN�����\���@�\�n���h����`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _FANLIFETIME_HADNLE
{
struct{
	float FanTemperatureOffset;					/* ���͉��x�I�t�Z�b�g								*/
	float FanTimeOffset;						/* FAN�����I�t�Z�b�g����							*/
	}conf;
struct{
	float FanSurvivor;							/* ����FAN�����c����								*/
	}var;
}FANLIFETIME_HADNLE;

/*--------------------------------------------------------------------------------------------------*/
/*		�R���f���T�����\���@�\�n���h����`															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _CAPALIFETIME_HANDLE
{
struct {
	float	CapaSurvivor;						/* �R���f���T�����c����								*/
	}var;
}CAPALIFETIME_HANDLE;


/*--------------------------------------------------------------------------------------------------*/
/*		�˖h�����\���@�\�n���h����`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _RUSHLIFETIME_HANDLE
{
struct {
	float	RushLifeTimeReciprocal;				/* �˖h��H����(�P�ʂP��)�̋t��					*/
	}conf;
struct {
	float	RushSurvivor;						/* �˖h��H�����c����								*/
	ULONG	RushSwitchCount;					/* �˖h�����[�����								*/
	ULONG	RushSwCountLatch;					/* �˖h�����[����񐔃��b�`							*//* <S192> */
//	BOOL	RushCountChanged;					/* �˖h�����[����񐔕ύX							*//* <S180> */
	BOOL	LastChargeOk;						/* �O��ChargeOk���								*/
	BOOL 	CurrentChargeOk;					/* ����ChargeOk���								*/
	}var;
}RUSHLIFETIME_HANDLE;

/*--------------------------------------------------------------------------------------------------*/
/*		DB�����\���@�\�n���h����`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DBLIFETIME_HADNLE
{
struct {
	float	DBLifeTimeReciprocal;				/* DB��H����(�P�ʂP��)�̋t��							*/
	}conf;
struct {
	float	DBSurvivor;							/* DB��H�����c����									*/
	ULONG	DBSwitchCount;						/* DB�����[�����									*/
	ULONG	DBSwCountLatch;						/* DB�����[����񐔃��b�`								*//* <S192> */
//	BOOL	DBCountChanged;						/* DB�����[����񐔕ύX								*//* <S192> */
	BOOL	LastDbOn;							/* �O�� Dbon	���								*/
	}var;

}DBLIFETIME_HADNLE;

/*--------------------------------------------------------------------------------------------------*/
/*		���ω��x�v�Z�p�n���h����`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _AVG_TEMPERATURE_HANDLE
{
	struct
	{
		ULONG	Timer100msCntLast;				/* �O�񕽋ω��x�X�V���̉ғ����� [100mS]				*/
		SHORT	AvgIntTmpr;						/* ���ϓ������x [0.01��]							*/
		USHORT	Dummy;
	}var;

}AVGTMPR_HANDLE;


/*--------------------------------------------------------------------------------------------------*/
/*		�����\���@�\�n���h����`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _LIFETIME_HANDLE
{
	FANLIFETIME_HADNLE		FanLifeTimeHdl;		/* FAN�����\���@�\									*/
	CAPALIFETIME_HANDLE		CapaLifeTimeHdl;	/* �R���f���T�����@�\								*/
	RUSHLIFETIME_HANDLE		RushLifeTimeHdl;	/* �˖h�����\���@�\									*/
//	DBLIFETIME_HADNLE		DBLifeTimeHdl;		/* DB�����\���@�\									*//* <S180> */
	AVGTMPR_HANDLE			AvgTmprHdl;			/* ���ω��x�v�Z�p									*/
} LIFETIME_HANDLE;



/****************************************************************************************************/
/*																									*/
/*		�֐��ꗗ																					*/
/*																									*/
/****************************************************************************************************/
extern void LifeTimeSurvivorCalculateMain( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );// �����c�����v�ZMain�֐�
//extern LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker );		/* <S180> */
LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker, float DBSurvivor );	/* <S180> */
extern void LifeTimeSwithingCountInit( LIFETIME_HANDLE *hLifeTime );					// ��������񐔏������֐�

extern void AvgInternalTemperatureCalInit( AVGTMPR_HANDLE *hAvgTmprHdl, ULONG TimerCnt, SHORT InternalTmpr );
//LONG AvgInternalTemperatureCal(EEP_DEVICE_HANDLE *EepDevice, AVGTMPR_HANDLE *hAvgTmprHdl,		/* <S192> */
//										SHORT CurrentTmpr, ULONG CurrentTimerCnt );
LONG AvgInternalTemperatureCal(EEP_QUE_HANDLE *EepQue, AVGTMPR_HANDLE *hAvgTmprHdl,				/* <S192> */
								SHORT CurrentTmpr, ULONG CurrentTimerCnt );
//extern void LifeTimeDBSwitchingCount( LIFETIME_HANDLE *hLifeTime , BOOL TgOnSts, BOOL CurrentDbOn );		/* DB�����[����񐔂̓��[�^��]����DB�I�������ꍇ�̂݃J�E���g�A�b�v����*//* <S180> */
void LifeTimeDBSwitchingCount( DBLIFETIME_HADNLE *hDBLifeTime, BOOL TgOnSts, BOOL CurrentDbOn );	/* <S180> */
//void LifeTimePrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, CMN_PRMDATA *Cprm );		/* <S180> *//* <S186> *//* <S192> */
void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeDBSwithingCountInit( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeDBPrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );		/* <S192> */
void LifeTimeRushPrmStoreValueToEeprom( RUSHLIFETIME_HANDLE	*hRushLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );	/* <S192> */
#endif /* LIFE_TIME_API_H */

/***************************************** end of file **********************************************/

