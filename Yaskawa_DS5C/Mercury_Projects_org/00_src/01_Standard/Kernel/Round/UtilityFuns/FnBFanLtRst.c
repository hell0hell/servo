/*****************************************************************************
Description		: Built-in FAN Lifetime Reset (Fn110)	
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2014.03.05		created

*****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"


#define		TEN_HOURS_TO_100MS		360000		/* Ten Hours Convert To 100ms */


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnExecuteBFanLtRst(FUNEXE *FunExe, AXIS_HANDLE *Axis);	/* Execute Function of Built-in FAN Lifetime Reset*/



/*****************************************************************************
  Fn110: Execute Function of Built-in FAN Lifetime Reset
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		グローバルデータ(軸単位)
  Return:
	rc;			Error Status
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
PRM_RSLT FnExecuteBFanLtRst(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		rc;
	CMN_PRMDATA		*Cprm;
	ULONG 			CurrentTimerCnt;	/* 現在稼働時間	[100ms]*/
	USHORT 			FanOffsetTimer;		/* FAN 交換オフセット時間	[10h]*/
	FANLIFETIME_HADNLE		*hFanLifeTime;
	
	rc	 = PRM_RSLT_SUCCESS;
	Cprm = Axis->UniPrms->Cprm;
	hFanLifeTime = &(Axis->hLifeTime->FanLifeTimeHdl);
	
	
	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	CurrentTimerCnt = KlibGetTimeStamp();		/* 現在稼働時間取得 */
	FanOffsetTimer = CurrentTimerCnt / TEN_HOURS_TO_100MS;		/* [100ms]　－＞　[10h] */
	if ( Cprm->FanTimeOffset != FanOffsetTimer )
	{
		Cprm->FanTimeOffset = FanOffsetTimer;
		rc = PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_FanTimeOffset, 0 );
		if ( rc == PRM_RSLT_SUCCESS )
		{
			hFanLifeTime->conf.FanTimeOffset = (float)FanOffsetTimer;
		}
	}
	
	
	return (rc);
}



/***************************************** end of file **********************************************/
