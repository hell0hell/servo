/*****************************************************************************
Description		: 寿命予測機能処理
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --

	LifeTimeSurvivorCalculateMain() :寿命残存率計算Main関数
	LifeTimeSwithingCountInit()		:寿命動作回数初期化関数
	AdetCheckPreWarningSigOut()		:予防保全ワーニング・予防保全信号出力
	AvgInternalTemperatureCalInit()	:平均内部温度算出初期化処理
	AvgInternalTemperatureCal()		:平均内部温度算出処理

	-- Locals --
	LifeTimeFanSurvivorCalculate()	:FAN寿命残存率計算関数
	LifeTimeCapaSurvivorCalculate()	:コンデンサ寿命残存率計算関数
	LifeTimeRushSurvivorCalculate()	:突防寿命残存率計算関数
	LifeTimeDBSurvivorCalculate()	:DB寿命残存率計算関数
	LifeTimeRushSwitchingCount()	:突防リレー動作回数は充電完了時に カウントアップする
	LifeTimeDBSwitchingCount()		:DBリレー動作回数はモータ回転中にDBオンした場合のみカウントアップする
	LifeTimePrmStoreValueToEeprom() :寿命機能用パラメータ書き込みTo　Eeprom

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.08.25		created

*****************************************************************************/

#include "LifeTimePredictionApi.h"

#define FAN_LIFE_CALC_BASE				26280.0F		/* FAN寿命計算要素 */
#define FAN_LIFE_TEMPERATURE_BASE		70.0F			/* FAN温度計算要素 */
#define CAPA_LIFE_CALC_BASE				50000.0F		/* コンデンサ寿命計算要素 */
#define CAPA_LIFE_TEMPERATURE_BASE		105.0F			/* コンデンサ温度計算要素 */
#define CONVERT_100MS					(10.0F * 60.0F * 60.0F)	/* (10*60*60) = 36000 1hour単位から100ms単位に換算する要素 */
#define LIFE_MONITOR_CALC_FACTOR		10000.0F 		/*モニタ単位は0.01%為、10000倍増大*/
#define LIFE_TIME_SURVIVOR_MAX			10000.0F 		/* 寿命残存率 上限値 *//* <S192> */

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
/*		API関数一覧																					*/
/*																									*/
/****************************************************************************************************/
void LifeTimeSurvivorCalculateMain( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );	/* 寿命残存率計算Main関数		*/
void LifeTimeSwithingCountInit( LIFETIME_HANDLE *hLifeTime );						/* 寿命動作回数初期化関数		*/
//LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker );		/* <S180> */
LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker, float DBSurvivor );	/* <S180> */
void AvgInternalTemperatureCalInit( AVGTMPR_HANDLE *hAvgTmprHdl, ULONG TimerCnt, SHORT InternalTmpr );
//LONG AvgInternalTemperatureCal(EEP_DEVICE_HANDLE *EepDevice, AVGTMPR_HANDLE *hAvgTmprHdl,		/* <S192> */
//										SHORT CurrentTmpr, ULONG CurrentTimerCnt );
LONG AvgInternalTemperatureCal(EEP_QUE_HANDLE *EepQue, AVGTMPR_HANDLE *hAvgTmprHdl,				/* <S192> */
								SHORT CurrentTmpr, ULONG CurrentTimerCnt );
//void LifeTimeDBSwitchingCount( LIFETIME_HANDLE *hLifeTime , BOOL TgOnSts, BOOL CurrentDbOn );	/* DBリレー動作回数はモータ回転中にDBオンした場合のみカウントアップする*//* <S180> */
void LifeTimeDBSwitchingCount( DBLIFETIME_HADNLE *hDBLifeTime, BOOL TgOnSts, BOOL CurrentDbOn );				/* <S180> */
//void LifeTimePrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, CMN_PRMDATA *Cprm );		/* <S180> *//* <S186> *//* <S192> */
void LifeTimeDBPrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );		/* <S192> */
void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeDBSwithingCountInit( DBLIFETIME_HADNLE *hDBLifeTime );			/* <S180> */
void LifeTimeRushPrmStoreValueToEeprom( RUSHLIFETIME_HANDLE	*hRushLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );	/* <S192> */

/****************************************************************************************************/
/*																									*/
/*		Local関数一覧																				*/
/*																									*/
/****************************************************************************************************/
static void LifeTimeFanSurvivorCalculate( LIFETIME_HANDLE *hLifeTime );		/* FAN寿命残存率計算関数 		*/
static void LifeTimeCapaSurvivorCalculate( LIFETIME_HANDLE *hLifeTime );	/* コンデンサ寿命残存率計算関数	*/
static void LifeTimeRushSurvivorCalculate( LIFETIME_HANDLE *hLifeTime );	/* 突防寿命残存率計算関数		*/
//static void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime );		/* DB寿命残存率計算関数		*//* <S180> */
//static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime );		/* 突防リレー動作回数は充電完了時に カウントアップす	*//* <S180> */
static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm );		/* <S180> */
//static void LifeTimePrmStoreValueToEeprom( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm ); /* 寿命機能用パラメータ書き込みTo　Eeprom *//* <S180> */

/*****************************************************************************
  Description:	突防リレー動作回数は充電完了時に カウントアップする
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
	RegCfg;					Regsiter Manager
	Cprm;					ユニット共通パラメータ構造体
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime )		/* <S180> */
static void LifeTimeRushSwitchingCount( LIFETIME_HANDLE *hLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )	/* <S180> */
{

	if ( hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk == TRUE )										/* 充電完了 */
	{
		if ( hLifeTime->RushLifeTimeHdl.var.LastChargeOk ==  FALSE)		/* 前回充電は未完了状態	*/
		{
			hLifeTime->RushLifeTimeHdl.var.RushSwitchCount ++;
			/* <S180> Start */
			/* 現在の突防リレー動作回数 の値をEepromに書き込み */
//			Cprm->RushSwichCount = hLifeTime->RushLifeTimeHdl.var.RushSwitchCount;		/* <S192> */
//			PrmStoreValueToEeprom( RegCfg, &pndef_RushSwichCount, 0 );					/* <S192> */
//			hLifeTime->RushLifeTimeHdl.var.RushCountChanged = TRUE;
			/* <S180> End */
		}
	}
	hLifeTime->RushLifeTimeHdl.var.LastChargeOk = hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk;
}

/*****************************************************************************
  Description:	DBリレー動作回数はモータ回転中にDBオンした場合のみカウントアップする
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;				DB寿命予測機能ハンドル
	TgOnSts						モータ回転状態
	CurrentDbOn					DB ON Flag
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if 0		/* <S180> */
void LifeTimeDBSwitchingCount( LIFETIME_HANDLE *hLifeTime , BOOL TgOnSts, BOOL CurrentDbOn )
{
	if ( TgOnSts == TRUE )				/* モータ回転中 */
	{
		/*前回の状態が DBオフ且今回の状態がDBオンの場合*/
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
	if ( TgOnSts == TRUE )				/* モータ回転中 */
	{
		/*前回の状態が DBオフ且今回の状態がDBオンの場合*/
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
  Description:	DBリレー動作動作回数初期化関数
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
	hDBLifeTime;			DB寿命予測機能ハンドル
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
  Description:	DB動作回数初期化関数
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;			DB寿命予測機能ハンドル
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
  Description:	寿命残存率計算Main関数
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void LifeTimeSurvivorCalculateMain( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )
{
//	LifeTimeRushSwitchingCount( hLifeTime );			/* 突防リレー動作回数は充電完了時に カウントアップする	*//* <S180> */
	LifeTimeRushSwitchingCount( hLifeTime, RegCfg, Cprm );		/* 突防リレー動作回数は充電完了時に カウントアップする	*//* <S180> */
	LifeTimeFanSurvivorCalculate( hLifeTime );			/* FAN寿命残存率計算  */
	LifeTimeCapaSurvivorCalculate( hLifeTime );			/* コンデンサ寿命残存率計算 */
	LifeTimeRushSurvivorCalculate( hLifeTime );			/*突防寿命残存率計算  */
//	LifeTimeDBSurvivorCalculate( hLifeTime );			/* DB寿命残存率計算 *//* <S180> */
//	LifeTimePrmStoreValueToEeprom( hLifeTime , RegCfg , Cprm ); /* 寿命機能用パラメータ書き込みTO　Eeprom　*//* <S180> */
}

/*****************************************************************************
  Description:	FAN寿命残存率計算関数
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimeFanSurvivorCalculate( LIFETIME_HANDLE *hLifeTime )
{
	float FanLifeTime;				/* 内蔵FAN寿命		*/
	float FanTemperatureOffset;     /* 周囲温度オフセット 	*/
	float AroundTemperature;		/*	周囲温度			*/
	float AvgIntTmpr;				/* 平均内部温度		*/
	float FanTimeOffset;			/* FAN交換オフセット時間*/
	float Timer100msCounter;
	float val;
	float ftmp;
	ULONG tmp1;
	ULONG tmp2;
	AvgIntTmpr = (float)hLifeTime->AvgTmprHdl.var.AvgIntTmpr * 0.01F ;			/* 単位は℃　*/
	FanTemperatureOffset = hLifeTime->FanLifeTimeHdl.conf.FanTemperatureOffset;
	AroundTemperature = AvgIntTmpr - FanTemperatureOffset;						/* 周囲温度 ：平均内部温度 - 周囲温度オフセット*/
	Timer100msCounter = (float)KlibGetTimeStamp();
	if ( AroundTemperature < 0.0F)
	{
		AroundTemperature = 0.0F;
	}

	tmp1 = (ULONG)(( FAN_LIFE_TEMPERATURE_BASE - AroundTemperature ) * 0.1F ); 						/* k = (70 - x) / 10 x：平均内部温度 - 周囲温度オフセット				*/
	tmp2 = (ULONG)(( FAN_LIFE_TEMPERATURE_BASE - AroundTemperature ) - (float)(tmp1 * 10U));		/* tmp2 = (70 - x) - (tmp1 * 10) 切り捨てた小数点以下					*/
	ftmp = LifeTimeCalTbl[tmp2];																	/* ← 2^(0/10) ～ 2^(10/10)の値が入った配列					*/
	FanLifeTime = (float)(( FAN_LIFE_CALC_BASE * ( 1 << tmp1 )) * ftmp )* CONVERT_100MS;			/* 内蔵FAN寿命 = 26280 * (1<<tmp1) * ftmp 	単位は100ms				*/
	/* 計算式：値単位(100ms) = FanTimeOffset(単位10hour) * 10 *CONVERT_100MS (1hour単位から100ms単位に換算する要素) 	*/
	FanTimeOffset =  hLifeTime->FanLifeTimeHdl.conf.FanTimeOffset  * 10.0F * CONVERT_100MS; 		/* 単位を10hから100msへ変換 */


	if ( Timer100msCounter > FanTimeOffset )
	{
#if 0	/* <S192> */
		/* 内蔵FAN残存率 = ((累積稼働時間 - FAN交換オフセット時間) / 内蔵FAN寿命) × 100*/
		val = (((Timer100msCounter - FanTimeOffset) ) / FanLifeTime) * LIFE_MONITOR_CALC_FACTOR ;
		if (val >= 65535.0F)
		{
			hLifeTime->FanLifeTimeHdl.var.FanSurvivor = 65535.0F;
		}
		else
		{
			hLifeTime->FanLifeTimeHdl.var.FanSurvivor = val ;     			/* 内蔵FAN寿命残存率 = ((累積稼働時間 - FAN交換オフセット時間 / 内蔵FAN寿命) × 100) */
		}
#else	/* <S192> */
		/* 内蔵FAN寿命残存率 = 100 - ((累積稼働時間 - FAN交換オフセット時間 / 内蔵FAN寿命) × 100) */
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
  Description:	コンデンサ寿命残存率計算関数
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimeCapaSurvivorCalculate( LIFETIME_HANDLE *hLifeTime )
{
	float CapaTime;										/* コンデンサ寿命 */
	float AvgIntTmpr;									/* 平均内部温度		*/
	float val;
	float Timer100msCounter;
	float ftmp;
	ULONG tmp1;
	ULONG tmp2;
	AvgIntTmpr = (float)hLifeTime->AvgTmprHdl.var.AvgIntTmpr * 0.01F ;							/* 単位は１℃　*/
	Timer100msCounter = (float)KlibGetTimeStamp();

	tmp1 = (ULONG)(( CAPA_LIFE_TEMPERATURE_BASE - AvgIntTmpr ) * 0.1F );						/* k = (105 - 平均内部温度) / 10 									*/
	tmp2 = (ULONG)(( CAPA_LIFE_TEMPERATURE_BASE - AvgIntTmpr ) - (float)( tmp1 * 10U ));		/* tmp2 = (105 - x) - (tmp1 * 10) 切り捨てた小数点以下				*/
	ftmp = LifeTimeCalTbl[tmp2];																/* ← 2^(0/10) ～ 2^(10/10)の値が入った配列				*/
	CapaTime = (float)( CAPA_LIFE_CALC_BASE * ( 1 << tmp1 ) * ftmp ) * CONVERT_100MS;  			/* コンデンサ寿命 = 50000 × (1<<tmp1) * ftmp  単位は100MS			*/
#if 0	/* <S192> */
	/* 内蔵コンデンサ寿命残存率 = ((累積稼働時間 / コンデンサ寿命) × 100) */
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
	/* 内蔵コンデンサ寿命残存率 = 100 - ((累積稼働時間 / コンデンサ寿命) × 100) */
	val = ( Timer100msCounter / CapaTime ) * LIFE_MONITOR_CALC_FACTOR ;
	hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor = LIFE_TIME_SURVIVOR_MAX - val;
	if ( hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor < 0.0F )
	{
		hLifeTime->CapaLifeTimeHdl.var.CapaSurvivor =  0.0F ;
	}
#endif
}

/*****************************************************************************
  Description:	突防寿命残存率計算関数
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimeRushSurvivorCalculate( LIFETIME_HANDLE *hLifeTime )
{
	float val;
#if 0	/* <S192> */
	/* 使用状況[%] = ((現在の動作回数(PnE84) / 寿命回数(PnE82)) * 100) */
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
	/* 突防寿命残存率 = 100 - ((現在の動作回数(PnE84) / 寿命回数(PnE82)) * 100) */
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
  Description:	DB寿命残存率計算関数
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;			DB寿命予測機能ハンドル
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//static void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime )		/* <S180> */
void LifeTimeDBSurvivorCalculate( DBLIFETIME_HADNLE *hDBLifeTime )				/* <S180> */
{
	float val;
#if 0	/* <S192> */
	/* 使用状況[%] = ((現在の動作回数(PnE86) / 寿命回数(PnE84)) * 100) */
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
	/* DB寿命残存率 = 100 - ((現在の動作回数(PnE86) / 寿命回数(PnE84)) * 100) */
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
  Description:	 突防リレー動作回数用パラメータ書き込みTO　Eeprom
------------------------------------------------------------------------------
  Parameters:
	hRushLifeTime;			突防寿命予測機能ハンドル
	RegCfg;					Regsiter Manager
	Prm;					パラメータデータ
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void LifeTimeRushPrmStoreValueToEeprom( RUSHLIFETIME_HANDLE	*hRushLifeTime, REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )
{
	hRushLifeTime->var.RushSwCountLatch = hRushLifeTime->var.RushSwitchCount;
	if ( Cprm->RushSwichCount !=  hRushLifeTime->var.RushSwCountLatch )		/*  突防リレー動作回数変更した場合	*/
	{
		/* 現在の突防リレー動作回数 の値をEepromに書き込み */
		Cprm->RushSwichCount = hRushLifeTime->var.RushSwCountLatch;
		PrmStoreValueToEeprom( RegCfg, &pndef_RushSwichCount, 0 );
	}
}
/* <S192> End */
/* <S180> Start */
/*****************************************************************************
  Description:	 DBリレー動作回数用パラメータ書き込みTO　Eeprom
------------------------------------------------------------------------------
  Parameters:
	hDBLifeTime;			DB寿命予測機能ハンドル
	RegCfg;					Regsiter Manager
	Prm;					パラメータデータ
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//void LifeTimePrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm )		/* <S186> *//* <S192> */
void LifeTimeDBPrmStoreValueToEeprom( DBLIFETIME_HADNLE *hDBLifeTime, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm )
{
//	if ( hDBLifeTime->var.DBCountChanged == TRUE )			/* DBリレー動作回数変更した場合	*/
	hDBLifeTime->var.DBSwCountLatch = hDBLifeTime->var.DBSwitchCount;	/* <S192> */
	if ( Prm->DBSwichCount != hDBLifeTime->var.DBSwCountLatch )			/* <S192> */
	{
		/* 現在の現在のDB動作回数 の値をEepromに書き込み*/
//		Cprm->DBSwichCount = hDBLifeTime->var.DBSwitchCount;		/* <S186> *//* <S192> */
		Prm->DBSwichCount = hDBLifeTime->var.DBSwCountLatch;		/* <S192> */
		PrmStoreValueToEeprom( RegCfg, &pndef_DBSwichCount, 0 );
	}
//	hDBLifeTime->var.DBCountChanged = FALSE;		/* <S192> */
}
#if 0
/*****************************************************************************
  Description:	 寿命機能用パラメータ書き込みTO　Eeprom
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
	RegCfg;					Regsiter Manager
	Cprm;					ユニット共通パラメータ構造体
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void LifeTimePrmStoreValueToEeprom( LIFETIME_HANDLE *hLifeTime , REGIF_CONFIG_T *RegCfg , CMN_PRMDATA *Cprm )
{
	if ( hLifeTime->DBLifeTimeHdl.var.DBCountChanged == TRUE )			/* DBリレー動作回数変更した場合	*/
	{
		Cprm->DBSwichCount = hLifeTime->DBLifeTimeHdl.var.DBSwitchCount & 0xFFFF;
		PrmStoreValueToEeprom( RegCfg, &pndef_DBSwichCount, 0 );		/* 現在の現在のDB動作回数（下位16bit） の値をEepromに書き込み*/
	}
	if ( hLifeTime->RushLifeTimeHdl.var.RushCountChanged == TRUE )		/* 突防リレー動作回数変更した場合 	*/
	{
		Cprm->RushSwichCount = hLifeTime->RushLifeTimeHdl.var.RushSwitchCount & 0xFFFF;
		PrmStoreValueToEeprom( RegCfg, &pndef_RushSwichCount, 0 );		/* 現在の突防リレー動作回数（下位16bit） の値をEepromに書き込み*/
	}

	if (hLifeTime->RushLifeTimeHdl.var.RushCountChanged == TRUE || hLifeTime->DBLifeTimeHdl.var.DBCountChanged == TRUE )
	{
		Cprm->DRSwithingCount = (( hLifeTime->RushLifeTimeHdl.var.RushSwitchCount & 0x00FF0000 ) >> 16 )
													+ ( hLifeTime->DBLifeTimeHdl.var.DBSwitchCount & 0x00FF0000 >> 8 );
		PrmStoreValueToEeprom( RegCfg, &pndef_DRSwithingCount, 0 );		/* 現在の突防/DBリレー動作回数（上位）　 の値をEepromに書き込み*/
	}
	hLifeTime->RushLifeTimeHdl.var.RushCountChanged = FALSE;
	hLifeTime->DBLifeTimeHdl.var.DBCountChanged = FALSE;
}
#endif
/* <S180> End */
/*****************************************************************************
  Description: 予防保全ワーニング・予防保全信号出力
------------------------------------------------------------------------------
  Parameters:
	hLifeTime;				寿命予測機能ハンドル
	PowerChecker;			主回路監視用構造体定義
  Return:
	RC						戻り値
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker )	/* <S180> */
LONG AdetCheckPreWarningSigOut( LIFETIME_HANDLE *hLifeTime, CHECK_MAIN_POWER *PowerChecker, float DBSurvivor )	/* <S180> */
{
	LONG	RC;
	RC = CHK_PRE_NO_OPERATION;


	/* 冷却FAN、コンデンサ、突防回路、DB回路のいずれかの寿命が閾値(90%)を超えた際 */
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
		PowerChecker->var.PreSigOut = TRUE;				/* 予防保全信号出力 */
	}
	else
	{
		PowerChecker->var.PreSigOut = FALSE;
	}
	return( RC );
}


/*****************************************************************************
  Description: 平均内部温度算出初期化処理
------------------------------------------------------------------------------
  Parameters:
	hAvgTmprHdl			Average Internal Temperature Cal. Hnadle
	TimerCnt			稼働時間
	InternalTmpr		平均内部温度[0.01℃]値
  Return:

------------------------------------------------------------------------------
  Note:
*****************************************************************************/
void AvgInternalTemperatureCalInit( AVGTMPR_HANDLE *hAvgTmprHdl, ULONG TimerCnt, SHORT InternalTmpr )
{
	/*平均内部温度 [0.01℃]変数初期化*/
	hAvgTmprHdl->var.AvgIntTmpr = InternalTmpr;

	/*前回更新時の稼働時間初期化*/
	hAvgTmprHdl->var.Timer100msCntLast = TimerCnt;

	return;
}


/*****************************************************************************
  Description: 平均内部温度算出処理
------------------------------------------------------------------------------
  Parameters:
	EepDevice			Eeprom Device handle
	hAvgTmprHdl			Average Internal Temperature Cal. Hnadle
	CurrentTmpr			現在温度情報 [℃]
	CurrentTimerCnt		現在稼働時間
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
	SHORT	CurrentAvgTmpr;		/* 新使用平均内部温度 */
	LONG	rslt;

	rslt = 0;

	/* 現在温度情報の単位を変更する「0.1℃」－＞「0.01℃」　*/
	CurrentTmpr = CurrentTmpr * 10;

	/* 使用平均内部温度算出 */
	lwk1 = (LONG)( hAvgTmprHdl->var.Timer100msCntLast >> 14 );		/* 現在までの平均回数 */
	lwk2 = ( hAvgTmprHdl->var.AvgIntTmpr * lwk1 ) + CurrentTmpr;	/* 現在までの合計値 */
	CurrentAvgTmpr = (SHORT)( lwk2 / (lwk1 + 1) );					/* 新使用平均内部温度 */

	/* 平均温度EEPROM書き込み */
	if ( CurrentAvgTmpr != hAvgTmprHdl->var.AvgIntTmpr )
	{
		/* 平均内部温度変数の値を更新する */
		hAvgTmprHdl->var.AvgIntTmpr = CurrentAvgTmpr;

		/* 平均内部温度の値をEEPROMへ書き込む  */
//		rslt = EepdevWriteNoSumValues( 0,							/* <S192> *//* <S1D2> */
//									   EEP_NOSUMCMN_ADDRESS(EEP_AVRINTERTMPR_OFS),
//									   (USHORT*)&CurrentAvgTmpr, 1 );
		rslt = EepdevPutQueue( EepQue, 0, EEP_NOSUMCMN_ADDRESS(EEP_AVRINTERTMPR_OFS),	/* <S192> */
								(USHORT)CurrentAvgTmpr, 1 );
	}

	/* 前回更新時の稼働時間 = 現在稼働時間 */
	hAvgTmprHdl->var.Timer100msCntLast = CurrentTimerCnt;

	return rslt;
}








/***************************************** end of file **********************************************/


