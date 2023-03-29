/*****************************************************************************
Description		: 寿命予測機能ヘッドフィール
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
/*		予防保全機能用定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_PRE_NO_OPERATION	0x00			/* アラームなし(処理不要)							*/
#define CHK_PRE_SET_WRN		0x01				/* 全軸アラーム出力									*/

/*--------------------------------------------------------------------------------------------------*/
/*		FAN寿命予測機能ハンドル定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _FANLIFETIME_HADNLE
{
struct{
	float FanTemperatureOffset;					/* 周囲温度オフセット								*/
	float FanTimeOffset;						/* FAN交換オフセット時間							*/
	}conf;
struct{
	float FanSurvivor;							/* 内蔵FAN寿命残存率								*/
	}var;
}FANLIFETIME_HADNLE;

/*--------------------------------------------------------------------------------------------------*/
/*		コンデンサ寿命予測機能ハンドル定義															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _CAPALIFETIME_HANDLE
{
struct {
	float	CapaSurvivor;						/* コンデンサ寿命残存率								*/
	}var;
}CAPALIFETIME_HANDLE;


/*--------------------------------------------------------------------------------------------------*/
/*		突防寿命予測機能ハンドル定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _RUSHLIFETIME_HANDLE
{
struct {
	float	RushLifeTimeReciprocal;				/* 突防回路寿命(単位１回)の逆数					*/
	}conf;
struct {
	float	RushSurvivor;						/* 突防回路寿命残存率								*/
	ULONG	RushSwitchCount;					/* 突防リレー動作回数								*/
	ULONG	RushSwCountLatch;					/* 突防リレー動作回数ラッチ							*//* <S192> */
//	BOOL	RushCountChanged;					/* 突防リレー動作回数変更							*//* <S180> */
	BOOL	LastChargeOk;						/* 前回ChargeOk状態								*/
	BOOL 	CurrentChargeOk;					/* 今回ChargeOk状態								*/
	}var;
}RUSHLIFETIME_HANDLE;

/*--------------------------------------------------------------------------------------------------*/
/*		DB寿命予測機能ハンドル定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DBLIFETIME_HADNLE
{
struct {
	float	DBLifeTimeReciprocal;				/* DB回路寿命(単位１回)の逆数							*/
	}conf;
struct {
	float	DBSurvivor;							/* DB回路寿命残存率									*/
	ULONG	DBSwitchCount;						/* DBリレー動作回数									*/
	ULONG	DBSwCountLatch;						/* DBリレー動作回数ラッチ								*//* <S192> */
//	BOOL	DBCountChanged;						/* DBリレー動作回数変更								*//* <S192> */
	BOOL	LastDbOn;							/* 前回 Dbon	状態								*/
	}var;

}DBLIFETIME_HADNLE;

/*--------------------------------------------------------------------------------------------------*/
/*		平均温度計算用ハンドル定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _AVG_TEMPERATURE_HANDLE
{
	struct
	{
		ULONG	Timer100msCntLast;				/* 前回平均温度更新時の稼働時間 [100mS]				*/
		SHORT	AvgIntTmpr;						/* 平均内部温度 [0.01℃]							*/
		USHORT	Dummy;
	}var;

}AVGTMPR_HANDLE;


/*--------------------------------------------------------------------------------------------------*/
/*		寿命予測機能ハンドル定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _LIFETIME_HANDLE
{
	FANLIFETIME_HADNLE		FanLifeTimeHdl;		/* FAN寿命予測機能									*/
	CAPALIFETIME_HANDLE		CapaLifeTimeHdl;	/* コンデンサ寿命機能								*/
	RUSHLIFETIME_HANDLE		RushLifeTimeHdl;	/* 突防寿命予測機能									*/
//	DBLIFETIME_HADNLE		DBLifeTimeHdl;		/* DB寿命予測機能									*//* <S180> */
	AVGTMPR_HANDLE			AvgTmprHdl;			/* 平均温度計算用									*/
} LIFETIME_HANDLE;



/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
extern void LifeTimeSurvivorCalculateMain( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );// 寿命残存率計算Main関数
//extern LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker );		/* <S180> */
LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker, float DBSurvivor );	/* <S180> */
extern void LifeTimeSwithingCountInit( LIFETIME_HANDLE *hLifeTime );					// 寿命動作回数初期化関数

extern void AvgInternalTemperatureCalInit( AVGTMPR_HANDLE *hAvgTmprHdl, ULONG TimerCnt, SHORT InternalTmpr );
//LONG AvgInternalTemperatureCal(EEP_DEVICE_HANDLE *EepDevice, AVGTMPR_HANDLE *hAvgTmprHdl,		/* <S192> */
//										SHORT CurrentTmpr, ULONG CurrentTimerCnt );
LONG AvgInternalTemperatureCal(EEP_QUE_HANDLE *EepQue, AVGTMPR_HANDLE *hAvgTmprHdl,				/* <S192> */
								SHORT CurrentTmpr, ULONG CurrentTimerCnt );
//extern void LifeTimeDBSwitchingCount( LIFETIME_HANDLE *hLifeTime , BOOL TgOnSts, BOOL CurrentDbOn );		/* DBリレー動作回数はモータ回転中にDBオンした場合のみカウントアップする*//* <S180> */
void LifeTimeDBSwitchingCount( DBLIFETIME_HADNLE *hDBLifeTime, BOOL TgOnSts, BOOL CurrentDbOn );	/* <S180> */
//void LifeTimePrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, CMN_PRMDATA *Cprm );		/* <S180> *//* <S186> *//* <S192> */
void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeDBSwithingCountInit( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeDBPrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );		/* <S192> */
void LifeTimeRushPrmStoreValueToEeprom( RUSHLIFETIME_HANDLE	*hRushLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );	/* <S192> */
#endif /* LIFE_TIME_API_H */

/***************************************** end of file **********************************************/

