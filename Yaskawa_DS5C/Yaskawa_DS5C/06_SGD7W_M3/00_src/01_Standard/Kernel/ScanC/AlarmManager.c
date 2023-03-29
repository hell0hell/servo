/*****************************************************************************
Description	: Alarm Manager
-----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	-- APIs for an axis --
	ALMInitAlarmManager()			:Initialization of this module
	ALMStartupProcedure()			:Start up procedure after power on reset sequence
	ALMRuntimeProcess()				:Runtime process for Alarm Manager
	ALMSetServoAlarm()				:Set the alarm state
	ALMSetMask()					:Set the Alarm mask
	ALMCheckEachState()				:Check an Alarm state
	ALMSetPramError()				:Set the Parameter error
	ALMClearWarning()				:Clear the warning state
	ALMGetOtherAlarmChk()			:Get the other alarm state except "alm_id"
	ALMGetStatus()					:Get the alarm status
	ALMGetExNetStatus()				:Get the alarm status
	ALMGetNetAlarmTableDetailCode()	:Get the detected Alarm code with timestamp
	ALMGetNetAlarmTraceDetailCode()	:Get the Alarm History with timestamp
	ALMGetAlarmInfo()				:Get Alarm Code information
	ALMSetAlarmResetRequest()		:Request to reset the Alarm state
	ALMGetAlarmResetStatus()		:Get the alarm resetting state

	-- Special APIs for all axises --
	ALMInitGlobalHandle()			:Initialization of Global Handler
	ALMSetGlobalAlarm()				:Set the alarm to all axises
	ALMClearGlobalAlarm()			:Clear a detected alarm to all axises
	ALMCheckGlobalAlarm()			:Check whether specified alarm already detected.

Remarks		:
	This module uses the Mutex API of INTEGRITY operating system.
	In order to reuse this source code, the Mutex API has to be replaced.

-----------------------------------------------------------------------------
Changes		:
Name   Date          Description
-----------------------------------------------------------------------------
I.M.   2010.11.2     created

*****************************************************************************/
//#include <INTEGRITY.h>
#include "Basedef.h"
#include "AlarmManager.h"
#include "KLib.h"
#include "MLib.h"
#include "alarm.h"
#include "AlmLatch.h"			/* <S212> */

/* 2012.08.08 Y.Oka Make Mercury Base Software */
//#include <INTEGRITY.h>
/* 2012.08.08 Y.Oka Make Mercury Base Software */

/* defines */
#define	KPI_TMSTAMP_UNITMS		100		/* Time Stamp Unit [ms]				*/
#define	TIME_STAMP_SEC			36000	/* [sec] Check Count for TimeStamp	*/
#define ALM_CMDALM_TABLE_CLEAR   /* <S070> */
//#define ALM_WARNING_AUTO_CLEAR	/* <S184>SystemSetting.hで定義 */

#define PDET_OK_ALM_PHASEANGLE	(0x0C22)	/* 正常＇アラームA.C22  <S226> */

/* globals in this file */
static ALMMAN_HANDLE *AlmMngrGblHandle[MAX_AXIS_NUMBER];
static LONG AlmNumberOfAxes;

/* <S0FF> 
 *
 * Load and Reserve Specified Address.
 *
 *  arg1: pointer to the specified address
 *  return: load data.
 *
 * */
unsigned int __LWARX(unsigned int *arg1)
{
unsigned int r0;
	__asm
	{
		LDREX r0, [arg1,0]
	}
	return r0;
}

/* <S0FF> 
 *
 * Store the value to the specified address if still reserved.
 *
 *  arg1: pointer to the specified address
 *  arg2: store data.
 *  return: 0: store failed. 1: store succeeded.
 *
 * */
unsigned int __STWCX(unsigned int *arg1, unsigned int arg2)
{
unsigned int r0;
	__asm
	{
		STREX r0, arg2, [arg1,0]
		EOR r0, r0, #1
	}
	return r0;
}

/* <S0FF> */
UINT OS_AtomicModify(UINT *address, UINT mask, UINT delta)
{
	UINT old_value;
    while (1)
    {
		old_value = __LWARX(address);
		if (__STWCX(address, (old_value & ~mask) + delta))
			break;
	}
    return old_value;
}


/* <S0FF> */
BOOL OS_TestAndSet(UINT *address, UINT old_value, UINT new_value)
{
	UINT crt_val;
    while (1)
    {
    	crt_val = __LWARX(address);
		if(crt_val != old_value)
			return TRUE;
		if(__STWCX(address, new_value))
			break;
	}
	return FALSE;
}

/* <S0FF> */
void OS_AtomicSet(UINT *address, UINT new_value)
{
    while (1)
    {
    	__LWARX(address);
		if(__STWCX(address, new_value))
			break;
	}
}

/* <S0FF> */
UINT OS_LockAndLoadMemory(UINT *address)
{
	return __LWARX(address);
}

/* <S0FF> */
UINT OS_UnlockAndStoreMemory(UINT *address, UINT new_value)
{
	return __STWCX(address, new_value);
}



/*****************************************************************************
  Description: Initialization of Global Handler
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle of first axis
  Return:
	none
*****************************************************************************/
void ALMInitGlobalHandle(ALMMAN_HANDLE **AxisTopHandle, LONG AxisNum)
{
	INT ax;
	for( ax = 0; ax < AxisNum; ax++ )
	{
		AlmMngrGblHandle[ax] = AxisTopHandle[ax];
	}
	AlmNumberOfAxes = AxisNum;
}


/*****************************************************************************
  Description: Set an Alarm to all axises
------------------------------------------------------------------------------
  Parameters:
	alm_id;			Index number of the Alarm list table
  Return:
	none
*****************************************************************************/
void ALMSetGlobalAlarm(ALMID_T alm_id)
{
	INT ax;
	for( ax = 0; ax < AlmNumberOfAxes; ax++ )
	{ /* 全軸アラーム出力 */
		ALMSetServoAlarm( AlmMngrGblHandle[ax], alm_id );
	}
}

/*****************************************************************************
  Description: Clear a detected alarm to all axises
------------------------------------------------------------------------------
  Parameters:
	alm_id;			Index number of the Alarm list table
  Return:
	none
*****************************************************************************/
void ALMClearGlobalAlarm(ALMID_T alm_id)
{
	INT ax;
	for( ax = 0; ax < AlmNumberOfAxes; ax++ )
	{ /* 全軸アラームクリア */
		ALMClearWarning( AlmMngrGblHandle[ax], alm_id );
	}
}

/*****************************************************************************
  Description: Check whether specified alarm already detected.
------------------------------------------------------------------------------
  Parameters:
	alm_id;			Index number of the Alarm list table
  Return:
	none
*****************************************************************************/
BOOL ALMCheckGlobalAlarm(ALMID_T alm_id)
{
	return ALMCheckEachState( AlmMngrGblHandle[0], alm_id );
}

/* RAM上のアドレス情報  暗号化フォーマット対応 <S146> */
#define STACK_TOP_ADDRESS			0x67FAF500			/* スタックTOPアドレス				*/
#define ALM_SET_ADDRESS			(STACK_TOP_ADDRESS + 0x20)	/* 起動時アラームフラグ設定アドレス				*/
/* ランタイム通知用アラーム情報 */
#define ALARM_NONE					0x00000000		/* アラームなし									*/
#define ALARM_A024					0x00000001		/* A.024(初回ブート異常)						*/
#define ALARM_A025					0x00000002		/* A.025(ランタイムサムエラー、ランタイムIFエラー)	*/
/*****************************************************************************
  Description: Check boot password alarm
******************************************************************************/
static void almSetBootAlarm(ALMMAN_HANDLE *AlmMngr )
{
	if( *(VULONG *)(ALM_SET_ADDRESS) == ALARM_A024 )
	{
		/* A.024 : Flash初回起動時異常(工場試験機初回用アラーム) */
		ALMSetServoAlarm( AlmMngr, ALM_BOOT1 );
	}
	else if( *(VULONG *)(ALM_SET_ADDRESS) == ALARM_A025 )
	{
		/* A.025 : ランタイムファーム部異常(ミラー起動時アラーム) */
		ALMSetServoAlarm( AlmMngr, ALM_BOOT2 );
	}
	return;
}

/*****************************************************************************
  Description: Initialization of this module
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmDefTbl;		Pointer to the Alarm list table
	AlmDefCntx;		Number of the entry in the Alarm list table
  Return:
	none
*****************************************************************************/
void ALMInitAlarmManager(ALMMAN_HANDLE *AlmMngr, CPALM_TBL_DEF AlmDefTbl,
						 UINT AlmDefCntx, UINT AxisNumber)
{
	UINT	i;

	/* Initialize the member data in this module */
	MlibResetLongMemory(AlmMngr,  sizeof(ALMMAN_HANDLE)/4);

	AlmMngr->DefTable = AlmDefTbl;
	AlmMngr->TableEntryNum = AlmDefCntx;
//	AlmMngr->EepDevice = EepDevice;
	AlmMngr->myAsicNo = AxisNumber;
	EepdevCreateQueue(&AlmMngr->EepQue, AlmMngr->EepQueBuf,
						sizeof(AlmMngr->EepQueBuf)/sizeof(EEP_QUE_BUFFER));

	/* Create the mutex */
//	CreateLocalMutex((LocalMutex*)&AlmMngr->mutex);

	/* Check the Number of the entry in the Alarm list table  */
	if(AlmDefCntx > KPX_ALMDEFMAX)
	{
		KpiSystemError();
	}

	/* Set a pointer of the last trace buffer */
	AlmMngr->Trace.AlmTrcSp = &(AlmMngr->Trace.AlmTrc[0]);

	/* Set the mask value at reseting alarm */
	for(i = 0; i < AlmDefCntx; i++)
	{
		if(0 != (AlmMngr->DefTable[i].AlmAttr & ALMDEF_ATTR_NOTRESET))
		{
			AlmMngr->AlmRstMsk[i>>5] |= 1 << (i & 0x1F);
		}

		if(0 != (AlmMngr->DefTable[i].AlmAttr & ALMDEF_ATTR_WARNING))
		{
			AlmMngr->WrnRstMsk[i>>5] |= 1 << (i & 0x1F);
		}
	}

	/* 暗号化ブート時アラームチェック  <S146> */
	almSetBootAlarm(AlmMngr);
}

/*****************************************************************************
  Description: Set Alarm History Information
*****************************************************************************/
static void almSetDetecetedAlarmInfo(ALMMAN_HANDLE *AlmMngr /* <S0FF> , UCHAR StopModeParam */)
{
	UINT		i, cnt;
	UINT		attr, ofst;
	VUINT		idx;
	ALM_HISTORY	*Trace;
#ifdef CSW_BSP_SGDZ_IFBS80
	UCHAR		attrEx;
#endif

//	if(AlmMngr->AlmEepRstBusy != FALSE)
	if(( AlmMngr->AlmEepRstBusy != FALSE )||( AlmMngr->Status.AlmFlag == FALSE ))	/* <S1CF> */
	{
		return;
	}

	Trace = &AlmMngr->Trace;
	idx = Trace->AlmTrcHdr.b.Idx;
	cnt = Trace->AlmCnt;

	/*----------------------------------------------------------------------*/
	/* アラーム登録テーブルのチェック処理									*/
	/*----------------------------------------------------------------------*/
	while(Trace->LstAlmCnt != cnt)
	{
/* <S197> Start */
		if( Trace->LstAlmCnt > cnt )
		{
			Trace->LstAlmCnt = cnt;
		}
		else
		{
/* <S197> End */
			i = Trace->LstAlmCnt;
			attr = (Trace->AlmTbl[i].AlmDef)->AlmAttr;
			/*------------------------------------------------------------------*/
			/*		アラーム履歴テーブルへの登録処理							*/
			/*------------------------------------------------------------------*/
			if((attr & ALMDEF_ATTR_NOEEPROM) == 0)
			{
				if( ((Trace->AlmTbl[i].AlmDef)->AlmCode != (Trace->AlmTrcSp)->AlmCode)
					|| ((Trace->AlmTbl[i].TmStamp - (Trace->AlmTrcSp)->TmStamp) >= TIME_STAMP_SEC) )
				{
					Trace->AlmTrc[idx].AlmCode = (Trace->AlmTbl[i].AlmDef)->AlmCode;
					Trace->AlmTrc[idx].TmStamp = Trace->AlmTbl[i].TmStamp;
					Trace->AlmTrcSp = &Trace->AlmTrc[idx];

					/* Write Alarm Code to EEPROM */
					ofst = EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMCODE_OFS) + (idx*2);
//					EepdevPutQueue(&AlmMngr->EepQue, 0, ofst, Trace->AlmTrc[idx].AlmCode, 1);
					EepdevWriteNoSumValues( 0, ofst, (USHORT*)&(Trace->AlmTrc[idx].AlmCode), 1);	/* <S1CF> */
					/* Write Time stamp to EEPROM */
					ofst = EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMTIME_OFS) + (idx*4);
//					EepdevPutQueue(&AlmMngr->EepQue, 0, ofst, Trace->AlmTrc[idx].TmStamp, 2);
					EepdevWriteNoSumValues( 0, ofst, (USHORT*)&(Trace->AlmTrc[idx].TmStamp), 2);	/* <S1CF> */


					idx = (idx >= (KPX_ALMTRCMAX - 1)) ? 0 : idx + 1;
					if( Trace->AlmTrcHdr.b.Cnt < KPX_ALMTRCMAX )
					{	Trace->AlmTrcHdr.b.Cnt++;}
				}
#ifdef CSW_BSP_SGDZ_IFBS80
				/*--------------------------------------------------------------*/
				/*		起動時検出アラーム登録処理								*/
				/*--------------------------------------------------------------*/
				if( AlmMngr->IntAlmId != (USHORT)Trace->AlmTbl[i].AlmId)
				{
					attrEx = (Trace->AlmTbl[i].AlmDef)->AlmAttrEx;
					if((attrEx & ALMDEF_EXATTR_INIALM) && AlmMngr->IntAlmEnable)
					{
						/* Write Alarm Code to EEPROM */
						ofst = EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_INIALM_OFS);
//						EepdevPutQueue(&AlmMngr->EepQue, 0, ofst, (USHORT)Trace->AlmTbl[i].AlmId, 1);
						EepdevWriteNoSumValues( 0, ofst, (USHORT*)&(Trace->AlmTbl[i].AlmId), 1);	/* <S1CF> */
					}
				}
#endif
			}
			Trace->LstAlmCnt = i + 1;
		}
	}

	if(Trace->AlmTrcHdr.b.Idx != idx)
	{
		Trace->AlmTrcHdr.b.Idx = idx;
		/* Write Alarm Info. to EEPROM */
		ofst = EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMINFO_OFS);
//		EepdevPutQueue(&AlmMngr->EepQue, 0, ofst, Trace->AlmTrcHdr.w, 1);
		EepdevWriteNoSumValues( 0, ofst, (USHORT*)&(Trace->AlmTrcHdr.w), 1);	/* <S1CF> */
	}
}

/*****************************************************************************
  Description: Initialize the Alarm trace table
*****************************************************************************/
static void almInitAlmTraceTable(ALMMAN_HANDLE *AlmMngr)
{
	UINT			i;
	ALM_HISTORY		*Trace;

	Trace = &AlmMngr->Trace;

	/* アラーム履歴ヘッダ情報のチェック */
	if( (Trace->AlmTrcHdr.b.Cnt > KPX_ALMTRCMAX)
		|| (Trace->AlmTrcHdr.b.Idx > (KPX_ALMTRCMAX - 1))
		/*|| (Trace->AlmTrcHdr.b.Idx > Trace->AlmTrcHdr.b.Cnt)*/ )
	{
		Trace->AlmTrcHdr.w = 0x0000;
	}

	/* アラーム履歴前回格納ポインタの初期設定 */
	i = (Trace->AlmTrcHdr.b.Idx == 0) ?
			(KPX_ALMTRCMAX - 1) : (Trace->AlmTrcHdr.b.Idx - 1);

	Trace->AlmTrcSp = &Trace->AlmTrc[i];

	/* note:
	  アラーム履歴初回登録時に１Ｈ以内の同じアラームは登録しない
	  という条件を回避するため、AlmCodeをクリアしておく。
	*/
	if(Trace->AlmTrcHdr.b.Cnt == 0)
	{
		Trace->AlmTrcSp->AlmCode = 0x0000;
	}
}

/*****************************************************************************
  Description: Start up procedure after power on reset sequence
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
  Return:
	none			AlmOLInfo
*****************************************************************************/
ULONG ALMStartupProcedure(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam,
										LONG AxisMotType, LONG AbsoEncType)
{
	UINT				i;
	EEP_NOCHKSUM_VAL_T	eepVal;
	ULONG				AlmOLInfo;

	AlmMngr->StopModeParam = StopModeParam;
	AlmOLInfo = 0;

	/* Remove Masked Alarm/Warning */
	//LpxRemoveMaskedAlarm( );

	/* Read EEPROM Alarm Trace Data	*/
	if(0 == EepdevReadValues(0,
							 EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, 0),
							 //(USHORT*)&eepVal, EEP_TIMESTAMP_OFS/2) )		/* <S04E>:DELL */
							 (USHORT*)&eepVal, EEP_ENGINFO_OFS/2) )			/* <S04E>:ADD */
	{
		AlmMngr->Trace.AlmTrcHdr.w = eepVal.AlarmInfo;
		AlmMngr->Status.AlmOLInfo = eepVal.OLInfo;						/* <S0FF> */
		AlmOLInfo = AlmMngr->Status.AlmOLInfo;							/* <S0FF> */
		for(i = 0; i < KPX_ALMTRCMAX; i++)
		{
			AlmMngr->Trace.AlmTrc[i].AlmCode = eepVal.AlarmCode[i];
			AlmMngr->Trace.AlmTrc[i].TmStamp = eepVal.AlarmTime[i];
		}
	}

	/* Init. Alarm Trace Table	*/
	almInitAlmTraceTable( AlmMngr );

	/*----------------------------------------------------------------------*/
	/* Set the deteceted Alarm information									*/
	/*----------------------------------------------------------------------*/
	almSetDetecetedAlarmInfo(AlmMngr /* <S0FF> , StopModeParam*/);

	/* Reset OL informations */
	if( AlmMngr->Status.AlmOLInfo != 0x0000 )		/* <S0FF> */
	{
		/* 回生過負荷以外の過負荷情報リセット */
		AlmMngr->Status.AlmOLInfo &= ALMDEF_OLINFO_RGOL;			/* <S0FF> */
		AlmMngr->Trace.EepAlmOLInfo = AlmMngr->Status.AlmOLInfo;	/* <S0FF> */

		/* Write to EEPROM Alarm OverLoad Info. */
		eepVal.OLInfo = (USHORT)AlmMngr->Status.AlmOLInfo;			/* <S0FF> */
		EepdevWriteNoSumValues( 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_OLINFO_OFS),
								&eepVal.OLInfo,
								1 );
	}

#ifdef CSW_BSP_SGDZ_IFBS80
	/* 起動時検出アラーム有効条件チェック */
	if((AxisMotType == MOTTYPE_ROTARY) && (AbsoEncType != MENC_INCRE_TYPE))
	{
		USHORT almId;
		AlmMngr->IntAlmEnable = TRUE;
		/* Read EEPROM Initial Detect Alarm	*/
		if(0 == EepdevReadValues(AlmMngr->EepDevice, 0,
								 EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_INIALM_OFS),
								 &almId, EEP_INIALM_OFS/2) )
		{
			AlmMngr->IntAlmId = almId;
			if(almId!=0)
			{/* 起動時検出アラームセット */
				ALMSetServoAlarm(AlmMngr, (ALMID_T)almId);
			}
		}
	}
#endif

	/* EEPROMから読み出したOL情報。						*/
	/* 電源再投入時の過負荷ﾚﾍﾞﾙ50%開始の判定に使用する。*/
	return AlmOLInfo;
}

/*****************************************************************************
  アラームステータス更新処理 <S0FF>
*****************************************************************************/
static void almUpdateStatusFlags(ALMMAN_HANDLE *AlmMngr, CPALM_TBL_DEF alm_def_tbl)
{
	ALM_STATUS		stat, stat_tmp;
	ULONG			ex_attr, attr, stop_mode;

	/*----------------------------------------------------------------------*/
	/* Update Alarm status */
	/*----------------------------------------------------------------------*/
	attr = alm_def_tbl->AlmAttr;
	ex_attr = alm_def_tbl->AlmAttrEx;

	do {
		stat = AlmMngr->Status;
		stat_tmp = stat;

		if(FALSE != (ex_attr & ALMDEF_M3ATTR_CMDALM))
		{
			stat.CmdAlm = TRUE;
		}
		else if(FALSE != (ex_attr & ALMDEF_M3ATTR_COMALM))
		{
			stat.CommAlm = TRUE;
		}
		else
		{
			if(FALSE == (attr & ALMDEF_ATTR_WARNING))
			{	stat.DevAlm = TRUE;}
			else
			{	stat.DevWrn = TRUE;}
		}

		if(FALSE == (attr & ALMDEF_ATTR_WARNING))
		{/* Alarm */
			if(stat.AlmFlag == FALSE)
			{	stat.AlmCode = alm_def_tbl->AlmCode;}
			stat.AlmFlag = TRUE;
		}
		else
		{/* Warning */
			if(stat.AlmCode == 0)
			{	stat.AlmCode = alm_def_tbl->AlmCode;}

			stat.WrnFlag = TRUE;
		}

		/* モータエンコーダデータ無効フラグ処理	*/
		if(FALSE != (attr & ALMDEF_ATTR_MENCDANG))
		{
			stat.MencDataNG = TRUE;
		}
		if(FALSE != (attr & ALMDEF_ATTR_POSNG))
		{
			stat.PosDataNG = TRUE;
		}

		/* Set the stopping method */
//		stop_mode = alm_def_tbl->StopMode & 0xF;
		if( stat.AlmCode == PDET_OK_ALM_PHASEANGLE )	/* <S226> */
		{
			stop_mode = 0;
		}
		else
		{
			stop_mode = alm_def_tbl->StopMode & 0xF;
		}												/* <S226> */
		if( stop_mode == 0x0F )
		{	stop_mode = AlmMngr->StopModeParam;}

		if( stop_mode > stat.StopMode )
		{
			stat.StopMode = stop_mode;
		}

		/*	アラームＯＬ情報 */
		if(0 == (attr & ALMDEF_ATTR_WARNING))
		{
	//		AlmMngr->AlmOLInfo |= ((stop_mode >> 4) & 0x07);
			stat.AlmOLInfo |= ((alm_def_tbl->StopMode >> 4) & 0x07);
		}

	} while(OS_TestAndSet((UINT*)&AlmMngr->Status, *((UINT*)&stat_tmp), *((UINT*)&stat)));

}


/*****************************************************************************
  アラーム登録テーブルのリセット <S0FF>
*****************************************************************************/
static void almResetAlarmTable(ALMMAN_HANDLE *AlmMngr)
{
	UINT			i, j, cnt;
	UCHAR			Attrx;
	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];
//	BOOL			AlmFlag;	/* <S165> *//* <S199> Delete */
//	USHORT			AlmCode;	/* <S165> *//* <S199> Delete */
	ALM_TBL			*AlmTblSp;	/* <S165> */

/* <S165> Start */
	/* ローカル変数初期化 */
//	AlmCode = 0;		/* <S199> Delete */
//	AlmFlag = FALSE;	/* <S199> Delete */
/* <S165> End */

	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	i = j = 0;
	do
	{
		*((ULONG*)&AlmMngr->Status) = 0;
		cnt = AlmMngr->Trace.AlmCnt;
		while( i < cnt)
		{
			Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
			if(0 != (Attrx & ALMDEF_ATTR_NOTRESET))
			{
//<S0FF>				if(i != j)	/* i != jの場合はテーブル詰合せ	*/
				{
					AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
				}
				j++;	/* 非削除アラームのカウントアップ	*/

			}
			else
			{
				UINT idx = AlmMngr->Trace.AlmTbl[i].AlmId >> 5;
				AlmMngr->AlmSet[idx] &= AlmMngr->AlmRstMsk[idx];
			}
			i++;
		}

	} while(OS_TestAndSet(&(AlmMngr->Trace.AlmCnt), cnt, j));

	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	/* アラーム管理情報のリセット処理を変更する為、コメントアウト */
	for(i = 0; i < j; i++)
	{
		AlmMngr->Trace.AlmTbl[i] = AlmTbl[i];
		almUpdateStatusFlags(AlmMngr, AlmTbl[i].AlmDef);
/* <S199> Start */
/* アラーム信号情報リセット処理をfor分から出す。 */
/* <S165> Start */
#if 0
		/* ALO1, ALO2, ALO3信号情報リセット処理 */
		Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
		AlmTblSp = &AlmMngr->Trace.AlmTbl[i];

		if(0 != (Attrx & ALMDEF_ATTR_NOTRESET))	/* リセット不可アラーム/ワーニング */
		{
			if(0 != (Attrx & ALMDEF_ATTR_WARNING))
			{	/* ワーニング */
				if(AlmMngr->Status.AlmCode == 0)
				{
					AlmMngr->AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01;
					AlmMngr->AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01;
					AlmMngr->AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01;
				}
			}
			else
			{	/* アラーム */
				if(AlmFlag == FALSE)
				{
					AlmMngr->AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01;
					AlmMngr->AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01;
					AlmMngr->AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01;
				}
				AlmFlag = TRUE;
			}
		}
		else	/* リセット可能アラーム/ワーニング */
		{
			if(AlmMngr->Status.AlmCode != 0)
			{
				AlmMngr->AlmCodeOut.Alo1 = 0x00;
				AlmMngr->AlmCodeOut.Alo2 = 0x00;
				AlmMngr->AlmCodeOut.Alo3 = 0x00;
			}
		}
/* <S165> End */
#endif
/* <S199> End */
	}

/* <S1A8> Start */
	/*----------------------------------------------------------------------*/
	/*	ALO1, ALO2, ALO3信号情報設定処理									*/
	/*----------------------------------------------------------------------*/
	/* ALO1, ALO2, ALO3信号クリア */
	AlmMngr->AlmCodeOut.Alo1 = 0x00;
	AlmMngr->AlmCodeOut.Alo2 = 0x00;
	AlmMngr->AlmCodeOut.Alo3 = 0x00;

	/* アラームテーブルにアラーム、ワーニングがある場合は、ALO1, ALO2, ALO3信号情報設定する。	*/
	if( AlmMngr->Trace.AlmCnt > 0)
	{
		SetOutputAloSignals( AlmMngr );
	}
/* <S1A8> End */

/* <S1A8> Delete Start */
/* ALO1, ALO2, ALO3信号情報設定処理修正の為、コメントアウト */
#if 0
/* <S199> Start */
	/*----------------------------------------------------------------------*/
	/*	ALO1, ALO2, ALO3信号情報リセット処理								*/
	/*----------------------------------------------------------------------*/
		Attrx = (AlmMngr->Trace.AlmTbl[0].AlmDef)->AlmAttr;
		AlmTblSp = &AlmMngr->Trace.AlmTbl[0];

		if(0 != (Attrx & ALMDEF_ATTR_NOTRESET))	/* リセット不可アラーム/ワーニング */
		{
			if(0 != (Attrx & ALMDEF_ATTR_WARNING))
			{	/* ワーニング */
				if(AlmMngr->AlmCode != 0)
				{
					AlmMngr->AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01;
					AlmMngr->AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01;
					AlmMngr->AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01;
				}
			}
			else
			{	/* アラーム */
				AlmMngr->AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01;
				AlmMngr->AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01;
				AlmMngr->AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01;
			}
		}
		else	/* リセット可能アラーム/ワーニング */
		{
			if(AlmMngr->AlmCode != 0)
			{
				AlmMngr->AlmCodeOut.Alo1 = 0x00;
				AlmMngr->AlmCodeOut.Alo2 = 0x00;
				AlmMngr->AlmCodeOut.Alo3 = 0x00;
			}
		}
/* <S199> End */
#endif
/* <S1A8> Delete End */

//	AlmMngr->Trace.AlmCnt = j;
	AlmMngr->Trace.LstAlmCnt = j;
}

#if 0	/* <S0FF> */
static void almResetAlarmTable(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam)
{
	UINT			i, j, cnt;
	UCHAR			Attrx;
	UCHAR			StopModeX;
	UCHAR			StopModeY;
	UCHAR			MencDataNG;
	BOOL			AlmFlag;
	BOOL			WrnFlag;
	USHORT			AlmCode;
	ALM_CODE_OUT	AlmCodeOut; /* <S04B> */
	ALM_TBL			*AlmTblSp;
	ALM_EX_INFO		AlmExInfoL = {0};

	/* ローカル変数の初期化 */
	StopModeY = 0;
	MencDataNG = 0;
	AlmFlag = WrnFlag = FALSE;
	AlmCode = 0;
	AlmCodeOut.Alo1 = AlmCodeOut.Alo2 = AlmCodeOut.Alo3 = 0; /* <S04B> */

	/*	アラーム登録＆登録待ちビットテーブルのリセット */
	for(i = 0; i < ALM_BITTBL_NUM; i++)
	{
		AlmMngr->AlmSet[i] &= AlmMngr->AlmRstMsk[i];
	}

	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	j = 0;
	cnt = AlmMngr->Trace.AlmCnt;
	for(i = 0; i < cnt; i++)
	{
		Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
		if(0 != (Attrx & ALMDEF_ATTR_NOTRESET))
		{
			if(i != j)	/* i != jの場合はテーブル詰合せ	*/
			{
				AlmMngr->Trace.AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
			}

			AlmTblSp = &AlmMngr->Trace.AlmTbl[j];
			StopModeX = (AlmTblSp->AlmDef)->StopMode & 0x0F;
			if(StopModeX == 0x0F)
			{
				StopModeX = (UCHAR)StopModeParam;
			}
			if(StopModeX > StopModeY)
			{
				StopModeY = StopModeX;
			}

			if(0 != (Attrx & ALMDEF_ATTR_WARNING))
			{
				if(AlmCode == 0)
				{
					AlmCode = (AlmTblSp->AlmDef)->AlmCode;
					AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01; /* <S04B> */
					AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01; /* <S04B> */
					AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01; /* <S04B> */
				}
				WrnFlag = TRUE;
			}
			else
			{
				if(AlmFlag == FALSE)
				{
					AlmCode = (AlmTblSp->AlmDef)->AlmCode;
					AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01; /* <S04B> */
					AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01; /* <S04B> */
					AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01; /* <S04B> */
				}
				AlmFlag = TRUE;
			}

			/* モータエンコーダデータ無効？		*/
			if(0 != (Attrx & ALMDEF_ATTR_MENCDANG))
			{
				MencDataNG = TRUE;
			}

			/*	CMD_ALMのチェック */
			if(0 != (AlmTblSp->AlmDef->AlmAttrEx & ALMDEF_M3ATTR_CMDALM))
			{
				AlmExInfoL.CmdAlm = TRUE;
			}
			/*	COMM_ALMのチェック */
			else if(0 != (AlmTblSp->AlmDef->AlmAttrEx & ALMDEF_M3ATTR_COMALM))
			{
				AlmExInfoL.CommAlm = TRUE;
			}
			/*	D_ALM/D_WANのチェック */
			else
			{
				if(0 != (AlmTblSp->AlmDef->AlmAttr & ALMDEF_ATTR_WARNING))
				{	AlmExInfoL.DevWrn = TRUE;}
				else
				{	AlmExInfoL.DevAlm = TRUE;}
			}
			j++;	/* 非削除アラームのカウントアップ	*/
		}
	}

	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	AlmMngr->Trace.AlmCnt = j;
	AlmMngr->Trace.LstAlmCnt = j;
	AlmMngr->StopMode = StopModeY;
	AlmMngr->MencDataNG = MencDataNG;
	AlmMngr->AlmCode = AlmCode;
	AlmMngr->AlmCodeOut = AlmCodeOut; /* <S04B> */
	AlmMngr->AlmFlag = AlmFlag;
	AlmMngr->WrnFlag = WrnFlag;
	AlmMngr->AlmExInfo = AlmExInfoL;
}
#endif

/*****************************************************************************
  通信用CMD_ALMリセット処理 <S0FF> 
*****************************************************************************/
static void almResetNetCommandAlarm(ALMMAN_HANDLE *AlmMngr)
{
	UINT			i, j, cnt;
	UCHAR			Attrx;
	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];
	ALM_TBL			*AlmTblSp;		/* <S165> */
	USHORT			AlmCode = 0;	/* <S165> */
	UCHAR			AttrxEx;		/* <S165> */

	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	i = j = 0;
	do
	{
		*((ULONG*)&AlmMngr->Status) = 0;
		cnt = AlmMngr->Trace.AlmCnt;
		while( i < cnt)
		{
			Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttrEx;
			if(0 == (Attrx & ALMDEF_M3ATTR_CMDALM))
			{
//<S0FF>				if(i != j)	/* i != jの場合はテーブル詰合せ	*/
				{
					AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
				}
				j++;	/* 非削除アラームのカウントアップ	*/

			}
			else
			{
				UINT idx = AlmMngr->Trace.AlmTbl[i].AlmId >> 5;
				AlmMngr->AlmSet[idx] &= AlmMngr->AlmRstMsk[idx];
			}
			i++;
		}

	} while(OS_TestAndSet(&(AlmMngr->Trace.AlmCnt), cnt, j));

	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	for(i = 0; i < j; i++)
	{
		AlmMngr->Trace.AlmTbl[i] = AlmTbl[i];
		almUpdateStatusFlags(AlmMngr, AlmTbl[i].AlmDef);

/* <S165> Start */
		AlmTblSp = &AlmMngr->Trace.AlmTbl[i];
		Attrx   = AlmTblSp->AlmDef->AlmAttr;
		AttrxEx = AlmTblSp->AlmDef->AlmAttrEx;
		
		if(0 == (AttrxEx & ALMDEF_M3ATTR_CMDALM))	/* 非CMD_ALM属性時 */
		{
			if(0 != (Attrx & ALMDEF_ATTR_WARNING))
			{
				AlmMngr->AlmCodeOut.Alo1 = (AlmMngr->Status.AlmCode >> 0) & 0x01;
				AlmMngr->AlmCodeOut.Alo2 = (AlmMngr->Status.AlmCode >> 1) & 0x01;
				AlmMngr->AlmCodeOut.Alo3 = (AlmMngr->Status.AlmCode >> 2) & 0x01;
			}
		}
		else										/* CMD_ALM属性時 */
		{
#ifdef ALM_CMDALM_TABLE_CLEAR
			/* ワーニング登録フラグのクリア */
			AlmMngr->AlmSet[AlmTblSp->AlmId >> 5] &=
											~(1 << (AlmTblSp->AlmId & 0x1F));
#endif
		}
/* <S165> End */

	}

//	AlmMngr->Trace.AlmCnt = j;
	AlmMngr->Trace.LstAlmCnt = j;
}

#if 0 /* <S0FF> */
static void almResetNetCommandAlarm(ALMMAN_HANDLE *AlmMngr)
{
	UINT			i, j, cnt;
	UCHAR			Attrx;
	UCHAR			AttrxEx;
	BOOL			WrnFlag;
	USHORT			AlmCode;
	ALM_CODE_OUT	AlmCodeOut; /* <S04B> */
	ALM_TBL			*AlmTblSp;

	AlmCode = (AlmMngr->AlmFlag == FALSE) ? 0 : AlmMngr->AlmCode;
	WrnFlag = FALSE;
	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	j = 0;
	cnt = AlmMngr->Trace.AlmCnt;
	for(i = 0; i < cnt; i++)
	{
		AlmTblSp = &AlmMngr->Trace.AlmTbl[i];
		Attrx   = AlmTblSp->AlmDef->AlmAttr;
		AttrxEx = AlmTblSp->AlmDef->AlmAttrEx;

		if(0 == (AttrxEx & ALMDEF_M3ATTR_CMDALM))	/* 非CMD_ALM属性時 */
		{
#ifdef ALM_CMDALM_TABLE_CLEAR
			AlmMngr->Trace.AlmTbl[j] = *AlmTblSp;	/* テーブル詰合せデータ移動	*/
#endif
			if(0 != (Attrx & ALMDEF_ATTR_WARNING))
			{
				if(AlmCode == 0)
				{
					AlmCode = (AlmMngr->Trace.AlmTbl[j].AlmDef)->AlmCode;
					AlmMngr->AlmCodeOut.Alo1 = ((AlmMngr->Trace.AlmTbl[j].AlmDef)->AlmCode >> 0) & 0x01; /* <S04B> */
					AlmMngr->AlmCodeOut.Alo2 = ((AlmMngr->Trace.AlmTbl[j].AlmDef)->AlmCode >> 1) & 0x01; /* <S04B> */
					AlmMngr->AlmCodeOut.Alo3 = ((AlmMngr->Trace.AlmTbl[j].AlmDef)->AlmCode >> 2) & 0x01; /* <S04B> */
				}
				WrnFlag = TRUE;
			}
			j++;		/* 非削除アラームのカウントアップ	*/
		}
		else										/* CMD_ALM属性時 */
		{
#ifdef ALM_CMDALM_TABLE_CLEAR
			/* ワーニング登録フラグのクリア */
			AlmMngr->AlmSet[AlmTblSp->AlmId >> 5] &=
											~(1 << (AlmTblSp->AlmId & 0x1F));
#endif
		}
	}

#ifdef ALM_CMDALM_TABLE_CLEAR
	AlmMngr->Trace.AlmCnt = j;	/* アラームカウンタ更新	*/
	AlmMngr->Trace.LstAlmCnt = j;
#endif
	AlmMngr->AlmCode = AlmCode;
	AlmMngr->AlmCodeOut = AlmCodeOut; /* <S04B> */
	AlmMngr->WrnFlag = WrnFlag;
	AlmMngr->AlmExInfo.CmdAlm = FALSE;
}
#endif

/*****************************************************************************
  Warningリセット処理 <S0FF> 
*****************************************************************************/
static void almResetWarningTable(ALMMAN_HANDLE *AlmMngr)
{
	UINT			i, j, cnt;
	UCHAR			Attrx;
	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];

	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	i = j = 0;
	do
	{
		*((ULONG*)&AlmMngr->Status) = 0;
		cnt = AlmMngr->Trace.AlmCnt;
		while( i < cnt)
		{
			Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
			if(0 == (Attrx & ALMDEF_ATTR_WARNING))
			{
//<S0FF>				if(i != j)	/* i != jの場合はテーブル詰合せ	*/
				{
					AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
				}
				j++;	/* 非削除アラームのカウントアップ	*/

			}
			else
			{
				UINT idx = AlmMngr->Trace.AlmTbl[i].AlmId >> 5;
				AlmMngr->AlmSet[idx] &= AlmMngr->AlmRstMsk[idx];
			}
			i++;
		}

	} while(OS_TestAndSet(&(AlmMngr->Trace.AlmCnt), cnt, j));

	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	for(i = 0; i < j; i++)
	{
		AlmMngr->Trace.AlmTbl[i] = AlmTbl[i];
		almUpdateStatusFlags(AlmMngr, AlmTbl[i].AlmDef);
	}

//	AlmMngr->Trace.AlmCnt = j;
	AlmMngr->Trace.LstAlmCnt = j;
}

#if 0
static void almResetWarningTable(ALMMAN_HANDLE *AlmMngr)
{

	UINT			i, j, cnt;
	UCHAR			Attrx;

	/*	アラーム登録＆登録待ちビットテーブルのリセット */
	for(i = 0; i < ALM_BITTBL_NUM; i++)
	{
		AlmMngr->AlmSet[i] &= ~AlmMngr->WrnRstMsk[i];
	}

	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	j = 0;
	cnt = AlmMngr->Trace.AlmCnt;
	for(i = 0; i < cnt; i++)
	{
		Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
		if(0 == (Attrx & ALMDEF_ATTR_WARNING))
		{
			if(i != j)	/* i != jの場合はテーブル詰合せ	*/
			{
				AlmMngr->Trace.AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
			}

			j++;	/* 非削除アラームのカウントアップ	*/
		}
	}

	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	AlmMngr->Trace.AlmCnt = j;
	AlmMngr->Trace.LstAlmCnt = j;
	if(j == 0)
	{	AlmMngr->AlmCode = 0;}
	AlmMngr->WrnFlag = FALSE;
	AlmMngr->AlmExInfo.DevWrn = FALSE;
}
#endif

/*****************************************************************************
  Description: Runtime process for Alarm Manager <S0FF>
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
  Return:
	none
*****************************************************************************/
void ALMRuntimeProcess(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam)
{
	AlmMngr->StopModeParam = StopModeParam;

	/*----------------------------------------------------------------------*/
	/*	アラームリセット処理 : Alarm Reset Signal : OFF --> ON				*/
	/*----------------------------------------------------------------------*/
	if(AlmMngr->AlmRst != FALSE)
	{
		/* アラーム登録テーブルのリセット */
		almResetAlarmTable(AlmMngr);

		/* ALMリセット要求リセット */
		AlmMngr->AlmRst = FALSE;
		AlmMngr->WrnRst = FALSE;
		AlmMngr->NetCmdAlmRst = FALSE;

	}
	else if(AlmMngr->WrnRst != FALSE)
	{
		/* アラーム登録テーブルのリセット */
		almResetWarningTable(AlmMngr);

		/* ALMリセット要求リセット */
		AlmMngr->WrnRst = FALSE;
		AlmMngr->NetCmdAlmRst = FALSE;
	}
	else if(AlmMngr->NetCmdAlmRst != FALSE)
	{
		/* 通信用CMD_ALMを登録テーブルから削除 */
		almResetNetCommandAlarm(AlmMngr);

		/* CMD_ALMリセット要求リセット */
		AlmMngr->NetCmdAlmRst = FALSE;
	}
	else
	{
		;
	}

	/*----------------------------------------------------------------------*/
	/* Set Alarm Ready flag													*/
	/*----------------------------------------------------------------------*/
	AlmMngr->AlmReady = (AlmMngr->Status.AlmFlag == FALSE) ? TRUE : FALSE;
	AlmMngr->AlmCode = AlmMngr->Status.AlmCode;
	/*----------------------------------------------------------------------*/
	AlmMngr->WrnFlagX = (AlmMngr->Status.WrnFlag == FALSE) ? FALSE : TRUE;

}

#if 0	/* <S0FF> */
void ALMRuntimeProcess(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam)
{
	USHORT ofst;

	AlmMngr->StopModeParam = StopModeParam;
	/*----------------------------------------------------------------------*/
	/*	アラームリセット処理 : Alarm Reset Signal : OFF --> ON				*/
	/*----------------------------------------------------------------------*/
	if(AlmMngr->AlmRst != FALSE)
	{
		/* Blocks the calling Task until mutex is obtained. */
		WaitForLocalMutex((LocalMutex)AlmMngr->mutex);

		/* アラーム登録テーブルのリセット */
		almResetAlarmTable(AlmMngr, StopModeParam);

		/* Releases the mutex */
		ReleaseLocalMutex((LocalMutex)AlmMngr->mutex);
		/* ALMリセット要求リセット */
		AlmMngr->AlmRst = FALSE;
	}
	else if(AlmMngr->WrnRst != FALSE)
	{
		/* Blocks the calling Task until mutex is obtained. */
		WaitForLocalMutex((LocalMutex)AlmMngr->mutex);

		/* アラーム登録テーブルのリセット */
		almResetWarningTable(AlmMngr);

		/* Releases the mutex */
		ReleaseLocalMutex((LocalMutex)AlmMngr->mutex);
		/* ALMリセット要求リセット */
		AlmMngr->WrnRst = FALSE;
	}
	else
	{
	/*----------------------------------------------------------------------*/
	/* CMD_ALMリセット処理													*/
	/*----------------------------------------------------------------------*/
		if(AlmMngr->NetCmdAlmRst != FALSE)
		{
			/* Blocks the calling Task until mutex is obtained. */
			WaitForLocalMutex((LocalMutex)AlmMngr->mutex);

			/* 通信用CMD_ALMを登録テーブルから削除 */
			almResetNetCommandAlarm(AlmMngr);

			/* Releases the mutex */
			ReleaseLocalMutex((LocalMutex)AlmMngr->mutex);
			/* CMD_ALMリセット要求リセット */
			AlmMngr->NetCmdAlmRst = FALSE;
		}
	}

	/*----------------------------------------------------------------------*/
	/* Set Alarm Ready flag													*/
	/*----------------------------------------------------------------------*/
	AlmMngr->AlmReady = (AlmMngr->AlmFlag == FALSE) ? TRUE : FALSE;

	/*----------------------------------------------------------------------*/
	/* Set the detected Alarm information to the trace buffer				*/
	/*----------------------------------------------------------------------*/
	almSetDetecetedAlarmInfo(AlmMngr, StopModeParam);

	/*----------------------------------------------------------------------*/
	/*		アラームＯＬ情報のEEPROM書き込み要求処理						*/
	/*----------------------------------------------------------------------*/
	if(AlmMngr->AlmOLInfo != AlmMngr->Trace.EepAlmOLInfo)
	{
		AlmMngr->Trace.EepAlmOLInfo = AlmMngr->AlmOLInfo;
		/* Write OL Info. to EEPROM */
		ofst = EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_OLINFO_OFS);
		EepdevPutQueue(&AlmMngr->EepQue, 0, ofst, AlmMngr->Trace.EepAlmOLInfo, 1);
	}
}
#endif

/*****************************************************************************
  Description: Write Alarm history to EEPROM <S0FF> 
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
  Return:
	none
  Note:
    This function must be called from a lowest priority task.
*****************************************************************************/
void ALMHistoryWriteService(ALMMAN_HANDLE *AlmMngr)
{
	UINT	i;
	USHORT	ofst;
	USHORT	rstBuffer[20];

	/*----------------------------------------------------------------------*/
	/* Clear EEPROM Alarm History											*/
	/*----------------------------------------------------------------------*/
	if(AlmMngr->AlmEepRst != FALSE)
	{
		MlibResetLongMemory(&rstBuffer, sizeof(rstBuffer)/4);
		AlmMngr->AlmEepRstBusy = TRUE;

		/* Clear EEPROM Alarm Trace Data	*/
		EepdevWriteNoSumValues( 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMINFO_OFS),
								rstBuffer,
								1 );
		EepdevWriteNoSumValues( 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMCODE_OFS),
								rstBuffer,
								10 );
		EepdevWriteNoSumValues( 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMTIME_OFS),
								rstBuffer,
								20 );

		/* Clear Alarm Trace Data	*/
		AlmMngr->Trace.AlmTrcHdr.w = 0;
		for(i = 0; i < KPX_ALMTRCMAX; i++)
		{
			AlmMngr->Trace.AlmTrc[i].AlmCode = 0;
			AlmMngr->Trace.AlmTrc[i].TmStamp = 0;
		}

		AlmMngr->AlmEepRstBusy = FALSE;
		AlmMngr->AlmEepRst = FALSE;
	}

	/*----------------------------------------------------------------------*/
	/* Set the deteceted Alarm information to the trace buffer				*/
	/*----------------------------------------------------------------------*/
	almSetDetecetedAlarmInfo(AlmMngr);

	/*----------------------------------------------------------------------*/
	/* Write EEPROM Alarm History											*/
	/*----------------------------------------------------------------------*/
	EepdevWriteQuedValues(&AlmMngr->EepQue, 8);

	/*----------------------------------------------------------------------*/
	/*		アラームＯＬ情報のEEPROM書き込み要求処理						*/
	/*----------------------------------------------------------------------*/
	if(AlmMngr->Status.AlmOLInfo != AlmMngr->Trace.EepAlmOLInfo)
	{
		AlmMngr->Trace.EepAlmOLInfo = AlmMngr->Status.AlmOLInfo;
		/* Write OL Info. to EEPROM */
		ofst = EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_OLINFO_OFS);
//		EepdevPutQueue(&AlmMngr->EepQue, 0, ofst, AlmMngr->Trace.EepAlmOLInfo, 1);
		EepdevWriteNoSumValues( 0, ofst, (USHORT*)&(AlmMngr->Trace.EepAlmOLInfo), 1);	/* <S1CF> */
	}

	/*----------------------------------------------------------------------*/
	/* Write EEPROM Alarm Latch Data								 <S1CF> */
	/*----------------------------------------------------------------------*/
	if( AlmMngr->LtAlmReq != 0 )
	{
		if( AlmMngr->LtAlmReq == 1 )
		{
			if( AlmMngr->Trace.AlmCnt == AlmMngr->Trace.LstAlmCnt )
			{
				/* 0x833 : アラーム番号	、0x834 : タイムスタンプ 取得 */
				*( AlmMngr->AlmCdAdr ) = ALMGetAlarmHistory( AlmMngr, 0, AlmMngr->TmStpAdr );
				AlmMngr->LtAlmReq++;
			}
		}
		if( AlmMngr->LtAlmReq == 2 )
		{
			if( AlmMngr->LtDtWtCt != 0 )
			{
				EepdevWriteNoSumValues( 0, AlmMngr->LtRomAdr, AlmMngr->LtMemAdr, 1 );
				AlmMngr->LtMemAdr += 1;
				AlmMngr->LtRomAdr += 2;
				AlmMngr->LtDtWtCt--;
			}
			if( AlmMngr->LtDtWtCt == 0 )
			{
//				AlmMngr->LtAlmReq = 0;
				AlmMngr->LtAlmReq = 3;		/* <S212> */
			}
		}
/* <S212> */
		if( AlmMngr->LtAlmReq == 3 )
		{
			if ( AlmMngr->Status.AlmFlag == FALSE )
			{
				AlmMngr->LtAlmReq = 4;
			}
		}
/* <S212> */
	}
	/*----------------------------------------------------------------------*/
	/* Write EEPROM Lcd Display Data								 <S208> */
	/*----------------------------------------------------------------------*/
	LcdEep2EepRomWrite( 0 );
	/*----------------------------------------------------------------------*/
}

#if 0	/* <S0FF> */
void ALMHistoryWriteService(ALMMAN_HANDLE *AlmMngr)
{
	UINT	i;
	USHORT	rstBuffer[20];

	/*----------------------------------------------------------------------*/
	/* Clear EEPROM Alarm History											*/
	/*----------------------------------------------------------------------*/
	if(AlmMngr->AlmEepRst != FALSE)
	{
		MlibResetLongMemory(&rstBuffer, sizeof(rstBuffer)/4);
		AlmMngr->AlmEepRstBusy = TRUE;

		/* Clear EEPROM Alarm Trace Data	*/
		EepdevWriteNoSumValues( AlmMngr->EepDevice, 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMINFO_OFS),
								rstBuffer, 1 );
		EepdevWriteNoSumValues( AlmMngr->EepDevice, 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMCODE_OFS),
								rstBuffer, 10 );
		EepdevWriteNoSumValues( AlmMngr->EepDevice, 0,
								EEP_NOSUM_ADDRESS(AlmMngr->myAsicNo, EEP_ALMTIME_OFS),
								rstBuffer, 20 );

		/* Clear Alarm Trace Data	*/
		AlmMngr->Trace.AlmTrcHdr.w = 0;
		for(i = 0; i < KPX_ALMTRCMAX; i++)
		{
			AlmMngr->Trace.AlmTrc[i].AlmCode = 0;
			AlmMngr->Trace.AlmTrc[i].TmStamp = 0;
		}

		AlmMngr->AlmEepRstBusy = FALSE;
		AlmMngr->AlmEepRst = FALSE;
	}

	/*----------------------------------------------------------------------*/
	/* Write EEPROM Alarm History											*/
	/*----------------------------------------------------------------------*/
	EepdevWriteQuedValues(&AlmMngr->EepQue, AlmMngr->EepDevice, 8);

}
#endif


/*****************************************************************************
  Description: Set the alarm state <S0FF> 
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	alm_id;			Index number of the Alarm list table
  Return:
	FALSE: no alarm set, TRUE: alarm set
*****************************************************************************/
CPALM_TBL_DEF ALMSetServoAlarm(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id)
{
	ALM_TBL			*alm_tbl_ptr;
	CPALM_TBL_DEF	alm_def_tbl;
	UINT			crt_idx, idx;
	UINT			ArrayIdx, bitValue;
	UCHAR			Attr;	/* <S165> */
	ALM_TBL			*AlmTblSp;		/* <S1A8> */
	BOOL			AlmOutSigEnd;	/* <S1A8> */

	ArrayIdx = alm_id >> 5;
	bitValue = 1 << (alm_id&0x1F);
	alm_def_tbl = &AlmMngr->DefTable[alm_id];
	AlmOutSigEnd = FALSE;	/* <S1A8> */

	/*----------------------------------------------------------------------*/
	/* 同一アラーム/アラームマスク確認処理 */
	/*----------------------------------------------------------------------*/
	if((AlmMngr->AlmSetMsk[ArrayIdx] & bitValue) != 0 )
	{
		return NULL;
	}
	if((AlmMngr->AlmSet[ArrayIdx] & bitValue) != 0)
	{
		return alm_def_tbl;
	}

	/*----------------------------------------------------------------------*/
	/* Update Alarm status */
	/*----------------------------------------------------------------------*/
//	almUpdateStatusFlags(AlmMngr, alm_def_tbl);

//	OS_AtomicModify(&(AlmMngr->AlmSet[ArrayIdx]), bitValue, bitValue);

	/*----------------------------------------------------------------------*/
	/* アラーム登録処理 */
	/*----------------------------------------------------------------------*/
	do {
		crt_idx = AlmMngr->Trace.AlmCnt;
		idx = (crt_idx < (KPX_ALMTBLMAX-1)) ? (crt_idx + 1) : crt_idx;

		OS_AtomicModify((UINT*)&(AlmMngr->AlmSet[ArrayIdx]), bitValue, bitValue);
		almUpdateStatusFlags(AlmMngr, alm_def_tbl);

/* <S1A8> Delete Start */
/* アラームコード出力信号設定処理をDo While文のそとで実施する。 */
#if 0
/* <S165> Start */
	/*----------------------------------------------------------------------*/
	/* アラームコード出力信号設定処理 */
	/*----------------------------------------------------------------------*/
	if( (AlmMngr->Status.AlmFlag) || (AlmMngr->Status.WrnFlag) )
	{
		Attr = alm_def_tbl->AlmAttr;	/* アラーム属性取得 */
		if(0 != (Attr & ALMDEF_ATTR_WARNING))
		{	/* ワーニング */
			if(AlmMngr->Status.AlmFlag == FALSE)	/* アラームが先に発生していない場合 */
			{
				/* ALO1, ALO2,ALO3出力設定 */
				AlmMngr->AlmCodeOut.Alo1 = (alm_def_tbl->AlmCodeOut >> 0) & 0x01;
				AlmMngr->AlmCodeOut.Alo2 = (alm_def_tbl->AlmCodeOut >> 1) & 0x01;
				AlmMngr->AlmCodeOut.Alo3 = (alm_def_tbl->AlmCodeOut >> 2) & 0x01;
			}
		}
		else
		{	/* アラーム */
			if(AlmMngr->AlmCode == 0)	/* 先に他のアラームが発生していない場合 */
			{
				/* ALO1, ALO2,ALO3出力設定 */
				AlmMngr->AlmCodeOut.Alo1 = (alm_def_tbl->AlmCodeOut >> 0) & 0x01;
				AlmMngr->AlmCodeOut.Alo2 = (alm_def_tbl->AlmCodeOut >> 1) & 0x01;
				AlmMngr->AlmCodeOut.Alo3 = (alm_def_tbl->AlmCodeOut >> 2) & 0x01;
			}
		}
	}
/* <S165> End */
#endif
/* <S1A8> Delete End */

	} while(OS_TestAndSet((UINT*)&AlmMngr->Trace.AlmCnt, crt_idx, idx));

	if((idx < (KPX_ALMTBLMAX-1))	/* 通常処理 */
		|| (0 != (alm_def_tbl->AlmAttr & ALMDEF_ATTR_NOTRESET)) )/* テーブル満杯時 */
	{
		alm_tbl_ptr = &AlmMngr->Trace.AlmTbl[crt_idx];
		alm_tbl_ptr->AlmId   = alm_id;
		alm_tbl_ptr->AlmDef  = alm_def_tbl;
		alm_tbl_ptr->TmStamp = KlibGetTimeStamp();
	}

/* <S1A8> Start */
	/*----------------------------------------------------------------------*/
	/*	ALO1, ALO2, ALO3信号情報設定処理									*/
	/*----------------------------------------------------------------------*/
	SetOutputAloSignals( AlmMngr );
/* <S1A8> End */

	AlmLatchMonitorSequence( (ALMLATCH_HNDL*)AlmMngr->hAlmLatchHndl );	/* <S212> */

	return alm_def_tbl;
}

#if 0	/* <S0FF> */
CPALM_TBL_DEF ALMSetServoAlarm(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id)
{
	ALM_TBL			*alm_tbl_ptr;
	CPALM_TBL_DEF	alm_def_tbl;
	UCHAR			idx;
	UINT			ArrayIdx, bitValue;
	ULONG			ex_attr, attr, stop_mode;

	ArrayIdx = alm_id >> 5;
	bitValue = 1 << (alm_id&0x1F);
	alm_def_tbl = &AlmMngr->DefTable[alm_id];

	/*----------------------------------------------------------------------*/
	/* 同一アラーム/アラームマスク確認処理 */
	/*----------------------------------------------------------------------*/
	if((AlmMngr->AlmSetMsk[ArrayIdx] & bitValue) != 0 )
	{
		return NULL;
	}
	if((AlmMngr->AlmSet[ArrayIdx] & bitValue) != 0)
	{
		return alm_def_tbl;
	}

	/*----------------------------------------------------------------------*/
	/* Blocks the calling Task until mutex is obtained. */
	/*----------------------------------------------------------------------*/
	WaitForLocalMutex((LocalMutex)AlmMngr->mutex);

	/*----------------------------------------------------------------------*/
	/* アラーム登録処理 */
	/*----------------------------------------------------------------------*/
	idx = AlmMngr->Trace.AlmCnt;
	if(idx < (KPX_ALMTBLMAX-1))	/* 通常処理 */
	{
		alm_tbl_ptr = &AlmMngr->Trace.AlmTbl[idx];
		alm_tbl_ptr->AlmId   = alm_id;
		alm_tbl_ptr->AlmDef  = alm_def_tbl;
		alm_tbl_ptr->TmStamp = KlibGetTimeStamp();
		idx = idx + 1;
	}
	else
	{	/* テーブル満杯時非常処理(最終バッファに書き込み) */
		if(0 != (alm_def_tbl->AlmAttr & ALMDEF_ATTR_NOTRESET))
		{
			alm_tbl_ptr = &AlmMngr->Trace.AlmTbl[idx];
			alm_tbl_ptr->AlmId   = alm_id;
			alm_tbl_ptr->AlmDef  = alm_def_tbl;
			alm_tbl_ptr->TmStamp = KlibGetTimeStamp();
		}
	}

	AlmMngr->Trace.AlmCnt = idx;
	AlmMngr->AlmSet[ArrayIdx] |= bitValue;

	/*----------------------------------------------------------------------*/
	/* Update Network Alarm status */
	/*----------------------------------------------------------------------*/
	attr = alm_def_tbl->AlmAttr;
	ex_attr = alm_def_tbl->AlmAttrEx;
	if(FALSE != (ex_attr & ALMDEF_M3ATTR_CMDALM))
	{
		AlmMngr->AlmExInfo.CmdAlm = TRUE;
	}
	else if(FALSE != (ex_attr & ALMDEF_M3ATTR_COMALM))
	{
		AlmMngr->AlmExInfo.CommAlm = TRUE;
	}
	else
	{
		if(FALSE == (attr & ALMDEF_ATTR_WARNING))
		{	AlmMngr->AlmExInfo.DevAlm = TRUE;}
		else
		{	AlmMngr->AlmExInfo.DevWrn = TRUE;}
	}

	if(FALSE == (attr & ALMDEF_ATTR_WARNING))
	{/* Warning */
		if(AlmMngr->AlmFlag == FALSE)
		{
			AlmMngr->AlmCode = alm_def_tbl->AlmCode;
			AlmMngr->AlmCodeOut.Alo1 = ((alm_def_tbl->AlmCodeOut) >> 0) & 0x01; /* <S04B> */
			AlmMngr->AlmCodeOut.Alo2 = ((alm_def_tbl->AlmCodeOut) >> 1) & 0x01; /* <S04B> */
			AlmMngr->AlmCodeOut.Alo3 = ((alm_def_tbl->AlmCodeOut) >> 2) & 0x01; /* <S04B> */
		}

		AlmMngr->AlmFlag = TRUE;
	}
	else
	{
		if(AlmMngr->AlmCode == 0)
		{
			AlmMngr->AlmCode = alm_def_tbl->AlmCode;
			AlmMngr->AlmCodeOut.Alo1 = ((alm_def_tbl->AlmCodeOut) >> 0) & 0x01; /* <S04B> */
			AlmMngr->AlmCodeOut.Alo2 = ((alm_def_tbl->AlmCodeOut) >> 1) & 0x01; /* <S04B> */
			AlmMngr->AlmCodeOut.Alo3 = ((alm_def_tbl->AlmCodeOut) >> 2) & 0x01; /* <S04B> */
		}

		AlmMngr->WrnFlag = TRUE;
	}

	/* モータエンコーダデータ無効フラグ処理	*/
	if(FALSE != (attr & ALMDEF_ATTR_MENCDANG))
	{
		AlmMngr->MencDataNG = TRUE;
	}
	if(FALSE != (attr & ALMDEF_ATTR_POSNG))
	{
		AlmMngr->PosDataNG = TRUE;
	}

	/* Set the stopping method */
	stop_mode = alm_def_tbl->StopMode & 0xF;
	if( stop_mode == 0x0F )
	{	stop_mode = AlmMngr->StopModeParam;}

	if( stop_mode > AlmMngr->StopMode )
	{
		AlmMngr->StopMode = stop_mode;
	}

	/*	アラームＯＬ情報 */
	if(0 == (attr & ALMDEF_ATTR_WARNING))
	{
//		AlmMngr->AlmOLInfo |= ((stop_mode >> 4) & 0x07);
		AlmMngr->AlmOLInfo |= ((alm_def_tbl->StopMode >> 4) & 0x07);
	}

	/*----------------------------------------------------------------------*/
	/* Releases the mutex */
	/*----------------------------------------------------------------------*/
	ReleaseLocalMutex((LocalMutex)AlmMngr->mutex);

	return alm_def_tbl;
}
#endif


/*****************************************************************************
  Description: Set the Alarm mask
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	alm_id;			Index number of the Alarm list table
    MaskSet;		TRUE: Sets a mask, FALSE: Clear a mask
  Return:
	none
*****************************************************************************/
void ALMSetMask(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id, BOOL MaskSet)
{
	if(MaskSet != FALSE)
	{ /* マスクセット */
		AlmMngr->AlmSetMsk[alm_id>>5] |= 1<<(alm_id&0x1F);
	}
	else
	{ /* マスククリア */
		AlmMngr->AlmSetMsk[alm_id>>5] &= ~(1<<(alm_id&0x1F));
	}
}



/*****************************************************************************
  Description: Check an Alarm state
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	alm_id;			Index number of the Alarm list table
  Return:
	FALSE = no alarm, TRUE = alarm
*****************************************************************************/
BOOL ALMCheckEachState(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id)
{
	BOOL alm_state;
	alm_state = (AlmMngr->AlmSet[alm_id >> 5] >> (alm_id & 0x1F)) & 1;
	return alm_state;
}


/*****************************************************************************
  Description: Check the Specified Alarm Set Mask Status
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	alm_id;			Index number of the Alarm list table
  Return:
	FALSE = not masked, TRUE = masked
*****************************************************************************/
BOOL ALMCheckSetMask(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id)
{
	BOOL alm_state;
	alm_state = (AlmMngr->AlmSetMsk[alm_id >> 5] >> (alm_id & 0x1F)) & 1;
	return alm_state;
}


/*****************************************************************************
  Description: Set the Parameter error
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	ErrPrmNo;		Parameter number
  Return:
	none
*****************************************************************************/
void ALMSetPramError(ALMMAN_HANDLE *AlmMngr, USHORT ErrPrmNo)
{
	ALMSetServoAlarm(AlmMngr, ALM_PRM);		/* A.040 : パラメータ設定異常 */
	if(AlmMngr->PrmErrInfo.ErrFlag == FALSE)
	{
		AlmMngr->PrmErrInfo.ErrPrmNo = ErrPrmNo;
		AlmMngr->PrmErrInfo.ErrFlag = TRUE;
	}
}

/*****************************************************************************
  Description: Clear the warning state
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	alm_id;			Index number of the Alarm list table
  Return:
	none
*****************************************************************************/
void ALMClearWarning(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id)
{
//<S184>ワーニング自動クリア処理復活 Start
#if	(ALM_WARNING_AUTO_CLEAR == TRUE)	/* AP自動ワーニングクリア処理見直し <S1F1> */
	ULONG			i, j, cnt;
	ULONG			wrn_cnt;	/* ワーニングカウンター */
	ULONG			clr_cnt;	/* ワーニングクリアカウンター */
	UCHAR			Attrx;
	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];
	/*----------------------------------------------------------------------*/
	/*	アラームワーニング登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	i = j = 0;
	wrn_cnt = clr_cnt = 0;	/* ワーニングカウンター＆クリアカウンターのクリア */
	do
	{
		*((ULONG*)&AlmMngr->Status) = 0;
		cnt = AlmMngr->Trace.AlmCnt;
		while( i < cnt)
		{
			Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
			if(0 == (Attrx & ALMDEF_ATTR_WARNING))
			{	/* アラームの場合 */
				{
					AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
				}
				j++;	/* 非削除アラームのカウントアップ	*/
			}
			else
			{	/* ワーニングの場合 */
				wrn_cnt++;	/* ワーニングカウンター +1*/
				if(alm_id != AlmMngr->Trace.AlmTbl[i].AlmId)
				{	/* クリア対象アラームワーニングではない場合 */
					{
						AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
					}
					j++;	/* 非削除アラームのカウントアップ	*/
				}
				else
				{	/* クリア対象アラームワーニングの場合 */
					UINT idx = AlmMngr->Trace.AlmTbl[i].AlmId >> 5;
					AlmMngr->AlmSet[idx] &= AlmMngr->AlmRstMsk[idx];
					wrn_cnt--;	/* ワーニングカウンター -1*/
					clr_cnt++;	/* クリアカウンター +1*/
					AlmMngr->Trace.AlmCnt--;	/* アラームカウンター(ALM+WRN)-1 */
					/*	アナログ（7S_AP）対応(1秒自動クリア)のみIOクリア処理を入れる	*/
					if(AlmMngr->Trace.AlmCnt == 0)
					{
						AlmMngr->AlmCode = 0;
						AlmMngr->AlmCodeOut.Alo1 = 0;
						AlmMngr->AlmCodeOut.Alo2 = 0;
						AlmMngr->AlmCodeOut.Alo3 = 0;
					}
				}
			}
			i++;
		}
	} while(OS_TestAndSet(&(AlmMngr->Trace.AlmCnt), cnt, j));
	if(wrn_cnt == 0)
	{	/*	すべてのアラームワーニング(A.9xx)がなくなった場合のみ	*/
		AlmMngr->WrnFlagX = FALSE;	/*	/WARN出力信号データのOff	*/
		AlmMngr->Status.WrnFlag = FALSE;	/*	ワーニング発生中フラグのクリア	*/
	}
	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	for(i = 0; i < j; i++)
	{
		AlmMngr->Trace.AlmTbl[i] = AlmTbl[i];
		almUpdateStatusFlags(AlmMngr, AlmTbl[i].AlmDef);
	}
	if(clr_cnt > 0)
	{	/* アラームテーブル１個分のクリア */
		AlmMngr->Trace.AlmTbl[j].AlmId = NO_ALARM;	/* AlmIdのクリア(1個分) */
		AlmMngr->Trace.AlmTbl[j].AlmDef = 0x00000000;	/* アラーム定義ポインターのクリア(1個分) */
		AlmMngr->Trace.AlmTbl[j].TmStamp = 0x00000000;	/* タイムスタンプのクリア(1個分) */
	}
#else	/*#if	(ALM_WARNING_AUTO_CLEAR == TRUE)	AP以外自動ワーニングクリア処理見直し <S1F1> */
	ULONG			i, cnt;
	UCHAR			Attrx;
	/*----------------------------------------------------------------------*/
	/*	アラームワーニング登録テーブルの検索											*/
	/*----------------------------------------------------------------------*/
	i = 0;
	cnt = AlmMngr->Trace.AlmCnt;
	while( i < cnt)
	{
		Attrx = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr;
		if(1 == (Attrx & ALMDEF_ATTR_WARNING))
		{	/* ワーニングの場合 */
			if(alm_id == AlmMngr->Trace.AlmTbl[i].AlmId)
			{	/* クリア対象アラームワーニングの場合ビットデータのクリア */
				UINT idx = AlmMngr->Trace.AlmTbl[i].AlmId >> 5;
				AlmMngr->AlmSet[idx] &= AlmMngr->AlmRstMsk[idx];
			}
		}
		i++;
	}
#endif	/* #if	(ALM_WARNING_AUTO_CLEAR == TRUE)	AP自動ワーニングクリア処理見直し <S1F1> */

/* <S1A8> Start */
#if	(ALM_WARNING_AUTO_CLEAR == TRUE)
	/* アナログ（7S_AP）対応(1秒自動クリア)のみで実行される自動IOクリア処理後、	 */
	/* 現在発生しているアラームが存在している場合の処理							 */
	if(( AlmMngr->Trace.AlmCnt != 0 ) && (clr_cnt > 0) )
	{	/* 現在発生アラーム数が0ではない */
		/* アラーム/ワーニングのアラームコード出力信号を出力 */
		SetOutputAloSignals( AlmMngr );
	}
#endif
/* <S1A8> End */

//	AlmMngr->Trace.LstAlmCnt = j;		/* アラームヒストリ未登録対策 *//* <S196> */
//<S184>ワーニング自動クリア処理復活 End
#if 0
//A.910(過負荷ワーニング)などの自己クリアワーニングを
//2度検出してしまうためコメントアウトする。(SGDZ-BS63/80)
	if(0 != (AlmMngr->AlmSet[alm_id>>5] & (ULONG)(1<<(alm_id & 0x1F))))
	{ /* ワーニングあり */
		if(0 != (AlmMngr->DefTable[alm_id].AlmAttr & ALMDEF_ATTR_WARNING))
		{
#if	(ALM_WARNING_AUTO_CLEAR == TRUE)
			UINT	i;
			ULONG	alm, wrn;

			alm = wrn = 0;
			/* Blocks the calling Task until mutex is obtained. */
			WaitForLocalMutex((LocalMutex)AlmMngr->mutex);

			/* ワーニング状態フラグのクリア */
			AlmMngr->AlmSet[alm_id>>5] &= ~(1 << (alm_id & 0x1F));
			for(i = 0; i < ALM_BITTBL_NUM; i++)
			{
				alm |= AlmMngr->AlmSet[i] & (~AlmMngr->WrnRstMsk[i]);
				wrn |= AlmMngr->AlmSet[i] & AlmMngr->WrnRstMsk[i];
			}

			if(wrn == 0)
			{
				AlmMngr->WrnFlag = FALSE;
				if(alm == 0)
				{	AlmMngr->AlmCode = 0;}
			}

			/* Releases the mutex */
			ReleaseLocalMutex((LocalMutex)AlmMngr->mutex);
#else
			/* ワーニング状態フラグのクリア */
			AlmMngr->AlmSet[alm_id>>5] &= ~(1 << (alm_id & 0x1F));
#endif
		}
	}
#endif
}


/*****************************************************************************
  Description: Get the other alarm state except "alm_id"
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	alm_id;			Index number of the Alarm list table
  Return:
	FALSE: no alarm, TRUE: alarm issued
*****************************************************************************/
BOOL ALMGetOtherAlarmChk(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id)
{
	UINT i;
	
	if( AlmMngr->Trace.AlmCnt == 0 ) return( TRUE );		/* <S226> */
	
	for(i = 0; i < AlmMngr->Trace.AlmCnt; i++)
	{
		if( (AlmMngr->Trace.AlmTbl[i].AlmId != alm_id)
			&& (0 == ((AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr & ALMDEF_ATTR_WARNING)) )
		{
			return( TRUE );
		}
	}
	return( FALSE );
}

/*****************************************************************************
  Description: Get the alarm status
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmFlag;		Pointer to return the Alarm state
	WrnFlag;		Pointer to return the Warning state
  Return:
	Alarm code
*****************************************************************************/
ULONG ALMGetStatus(ALMMAN_HANDLE *AlmMngr, BOOL *AlmFlag, BOOL *WrnFlag)
{
	*AlmFlag = AlmMngr->Status.AlmFlag;	/* <S0FF> */
	*WrnFlag = AlmMngr->Status.WrnFlag;	/* <S0FF> */
	return AlmMngr->Status.AlmCode;		/* <S0FF> */
}

BOOL ALMGetExNetStatus(ALMMAN_HANDLE *AlmMngr, ALM_EX_INFO *AlmExInfo)
{
//<S0FF>	*AlmExInfo = AlmMngr->AlmExInfo;
	AlmExInfo->CmdAlm = AlmMngr->Status.CmdAlm;		/* <S0FF> */
	AlmExInfo->CommAlm = AlmMngr->Status.CommAlm;	/* <S0FF> */
	AlmExInfo->DevAlm = AlmMngr->Status.DevAlm;		/* <S0FF> */
	AlmExInfo->DevWrn = AlmMngr->Status.DevWrn;		/* <S0FF> */
//<S0FF>	return AlmMngr->PosDataNG;
	return AlmMngr->Status.PosDataNG;				/* <S0FF> */
}

/*****************************************************************************
  Description: Get the detected Alarm code(1ByteCode)  (Alarm, Warning)
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmCode;		Pointer to return the Alarm code
	MaxInfo;		Maximum Number of the trace buffer
  Return:
	actual size stored to AlmCode
*****************************************************************************/
LONG ALMGetNetAlarmTableCode(ALMMAN_HANDLE *AlmMngr, UCHAR *AlmCode, LONG MaxInfo)
{
	LONG	i, j;
	LONG	AlmCnt;

	if((LONG)AlmMngr->Trace.AlmCnt < MaxInfo)
	{
		AlmCnt = AlmMngr->Trace.AlmCnt;
	}
	else
	{
		AlmCnt = MaxInfo;
	}

	/*	アラーム情報のコピー */
	j=AlmCnt-1;
	for(i = 0; i < AlmCnt; i++)
	{
		AlmCode[i] = (UCHAR)((AlmMngr->Trace.AlmTbl[j].AlmDef)->AlmCode >> 4);
		j--;
	}
	/*	残りエリアゼロクリア */
	for( ; i < MaxInfo; i++)
	{
		AlmCode[i] = 0x00;
	}

	return AlmCnt;
}


/*****************************************************************************
  Description: Get the detected Alarm code(2ByteCode:Detail) (Alarm, Warning)
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmCode;		Pointer to return the Alarm code
	MaxInfo;		Maximum Number of the trace buffer
  Return:
	actual size stored to AlmCode
*****************************************************************************/
LONG ALMGetNetAlarmTableDetailCode(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo)
{
	LONG	i, j;
	LONG	AlmCnt;
	
	if((LONG)AlmMngr->Trace.AlmCnt < MaxInfo)
	{
		AlmCnt = AlmMngr->Trace.AlmCnt;
	}
	else
	{
		AlmCnt = MaxInfo;
	}

	/*	アラーム情報のコピー */
	j=AlmCnt-1;
	for(i = 0; i < AlmCnt; i++)
	{
		AlmCode[i] = (AlmMngr->Trace.AlmTbl[j].AlmDef)->AlmCode;
		j--;
	}
	/*	残りエリアゼロクリア */
	for( ; i < MaxInfo; i++)
	{
		AlmCode[i] = 0x0000;
	}

	return AlmCnt;
}


/*****************************************************************************
  Description: Get the detected Alarm num(Alarm Only)
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmCode;		Pointer to return the Alarm code
	MaxInfo;		Maximum Number of the trace buffer
  Return:
	actual size stored to AlmCode
*****************************************************************************/
LONG ALMGetNetAlarmNum(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo)
{
	LONG	i, k, AlmCnt;
	LONG	AlmWrnCnt;
	UCHAR	AlmAttr;

	if((LONG)AlmMngr->Trace.AlmCnt < MaxInfo)
	{
		AlmWrnCnt = AlmMngr->Trace.AlmCnt;
	}
	else
	{
		AlmWrnCnt = MaxInfo;
	}

	/*	アラーム情報のコピー */
	AlmCnt=0;
	for(i = 0; i < AlmWrnCnt; i++)
	{
		AlmAttr = ((AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr & 0x0001);
		if( AlmAttr != ALMDEF_ATTR_WARNING )/* ワーニング(1),アラーム(0)	*/
		{
			AlmCode[AlmCnt] = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmCode;
			AlmCnt++;
		}
	}
	/*	残りエリアゼロクリア */
	for( k=AlmCnt; k < MaxInfo; k++)
	{
		AlmCode[k] = 0x0000;
	}

	return AlmCnt;
}


/*****************************************************************************
  Description: Get the detected Warning num(Warning Only)
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmCode;		Pointer to return the Alarm code
	MaxInfo;		Maximum Number of the trace buffer
  Return:
	actual size stored to AlmCode
*****************************************************************************/
LONG ALMGetNetWarningNum(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo)
{
	LONG	i,  k, AlmCnt;
	LONG	AlmWrnCnt;
	UCHAR	AlmAttr;

	if((LONG)AlmMngr->Trace.AlmCnt < MaxInfo)
	{
		AlmWrnCnt = AlmMngr->Trace.AlmCnt;
	}
	else
	{
		AlmWrnCnt = MaxInfo;
	}

	/*	アラーム情報のコピー */
	AlmCnt=0;
	for(i = 0; i < AlmWrnCnt; i++)
	{
		AlmAttr = ((AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmAttr & 0x0001);
		if( AlmAttr == ALMDEF_ATTR_WARNING )/* ワーニング(1),アラーム(0)	*/
		{
			AlmCode[AlmCnt] = (AlmMngr->Trace.AlmTbl[i].AlmDef)->AlmCode;
			AlmCnt++;
		}
	}
	/*	残りエリアゼロクリア */
	for(k=AlmCnt; k < MaxInfo; k++)
	{
		AlmCode[k] = 0x0000;
	}

	return AlmCnt;
}

/*****************************************************************************
  Description: Get the Alarm(1ByteCode) History
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmCode;		Pointer to return the Alarm code
	MaxInfo;		Maximum Number of the trace buffer
  Return:
	actual size stored to AlmCode
*****************************************************************************/
LONG ALMGetNetAlarmTraceCode(ALMMAN_HANDLE *AlmMngr, UCHAR *AlmCode, LONG MaxInfo)
{
	LONG	i, j;
	LONG	AlmCnt;
	ALM_HISTORY *AlmHist;

	AlmHist = &AlmMngr->Trace;

	/* インデクス初期値設定	*/
	j = AlmHist->AlmTrcHdr.b.Idx - 1;
	if(j < 0)
	{
		j += KPX_ALMTRCMAX;
	}

	/*	アラーム情報コピー	*/
	if((LONG)AlmHist->AlmTrcHdr.b.Cnt < MaxInfo)
	{
		AlmCnt = AlmHist->AlmTrcHdr.b.Cnt;
	}
	else
	{
		AlmCnt = MaxInfo;
	}
	
	for(i = 0; i < AlmCnt; i++)
	{
		AlmCode[i] = (UCHAR)(AlmHist->AlmTrc[j].AlmCode >> 4);
		--j;
		if(j < 0)
		{
			j += KPX_ALMTRCMAX;
		}
	}

	/*	残りエリアゼロクリア */
	for( ; i < MaxInfo; i++)
	{
		AlmCode[i] = 0x00;
	}

	return AlmCnt;
}


/*****************************************************************************
  Description: Get the Alarm(2ByteCode:Detail) History
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmCode;		Pointer to return the Alarm code
	MaxInfo;		Maximum Number of the trace buffer
  Return:
	actual size stored to AlmCode
*****************************************************************************/
LONG ALMGetNetAlarmTraceDetailCode(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo)
{
	LONG	i, j;
	LONG	AlmCnt;
	ALM_HISTORY *AlmHist;

	AlmHist = &AlmMngr->Trace;

	/* インデクス初期値設定	*/
	j = AlmHist->AlmTrcHdr.b.Idx - 1;
	if(j < 0)
	{
		j += KPX_ALMTRCMAX;
	}

	/*	アラーム情報コピー	*/
	if((LONG)AlmHist->AlmTrcHdr.b.Cnt < MaxInfo)
	{
		AlmCnt = AlmHist->AlmTrcHdr.b.Cnt;
	}
	else
	{
		AlmCnt = MaxInfo;
	}
	
	for(i = 0; i < AlmCnt; i++)
	{
		AlmCode[i] = AlmHist->AlmTrc[j].AlmCode;
		--j;
		if(j < 0)
		{
			j += KPX_ALMTRCMAX;
		}
	}

	/*	残りエリアゼロクリア */
	for( ; i < MaxInfo; i++)
	{
		AlmCode[i] = 0x0000;
	}

	return AlmCnt;
}

/*****************************************************************************
  Description: Get the Alarm Table with alarm index for Network
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmInfo;		Pointer to return the Alarm Info
	Index;			Index of the Alarm History
  Return:
	Alarm code
*****************************************************************************/
void ALMGetNetAlarmTableInfo(ALMMAN_HANDLE *AlmMngr, USHORT *AlmInfo, UINT Index)
{
	LONG	i;
	ALM_HISTORY *AlmHist;

	AlmHist = &AlmMngr->Trace;

	if( Index >= AlmHist->AlmCnt )
	{
		AlmInfo[0] = 0x0000;
		AlmInfo[1] = 0x0000;
		AlmInfo[2] = 0x0000;
	}
	else
	{
		i = AlmHist->AlmCnt - 1 - Index;
		AlmInfo[0] = AlmHist->AlmTbl[i].AlmDef->AlmCode;
		AlmInfo[1] = ((USHORT*)(&AlmHist->AlmTbl[i].TmStamp))[0];
		AlmInfo[2] = ((USHORT*)(&AlmHist->AlmTbl[i].TmStamp))[1];
	}
}

/*****************************************************************************
  Description: Get the Alarm History with alarm index for Network
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	AlmInfo;		Pointer to return the Alarm Info
	Index;			Index of the Alarm History
  Return:
	Alarm code
*****************************************************************************/
void ALMGetNetAlarmTraceInfo(ALMMAN_HANDLE *AlmMngr, USHORT *AlmInfo, UINT Index)
{
	LONG	i;
	ALM_HISTORY *AlmHist;

	AlmHist = &AlmMngr->Trace;

	if(Index >= (UINT)AlmHist->AlmTrcHdr.b.Cnt)
	{
		AlmInfo[0] = 0x0000;
		AlmInfo[1] = 0x0000;
		AlmInfo[2] = 0x0000;
	}
	else
	{
		i = AlmHist->AlmTrcHdr.b.Idx - 1 - Index;
		if(i < 0)
		{
			i += KPX_ALMTRCMAX;
		}
		AlmInfo[0] = AlmHist->AlmTrc[i].AlmCode;
		AlmInfo[1] = ((USHORT*)(&AlmHist->AlmTrc[i].TmStamp))[0];
		AlmInfo[2] = ((USHORT*)(&AlmHist->AlmTrc[i].TmStamp))[1];
	}
}


/*****************************************************************************
  Description: Get the Alarm History with alarm index
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
	Index;			Index of the Alarm History
	TimeStamp;		Pointer to store the time stamp value
  Return:
	Alarm code
*****************************************************************************/
USHORT ALMGetAlarmHistory(ALMMAN_HANDLE *AlmMngr, UINT Index, ULONG *TimeStamp)
{
	LONG	i;
	USHORT	almCode;
	ALM_HISTORY *AlmHist;

	AlmHist = &AlmMngr->Trace;

	if(Index >= (UINT)AlmHist->AlmTrcHdr.b.Cnt)
	{
		almCode = 0;
		*TimeStamp = 0;
	}
	else
	{
		i = AlmHist->AlmTrcHdr.b.Idx - 1 - Index;
		if(i < 0)
		{
			i += KPX_ALMTRCMAX;
		}
		almCode = AlmHist->AlmTrc[i].AlmCode;
		*TimeStamp = AlmHist->AlmTrc[i].TmStamp;
	}

	return almCode;
}


/*****************************************************************************
  Description: Get Alarm code information
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
  Return:
	AlmCode
*****************************************************************************/
LONG ALMGetAlarmInfo(ALMMAN_HANDLE *AlmMngr)
{
	return AlmMngr->AlmCode;
}

/*****************************************************************************
  Description: Alarm Reset Request service
*****************************************************************************/
void ALMSetAlarmResetRequest(ALMMAN_HANDLE *AlmMngr)
{
	AlmMngr->AlmRst = TRUE;
}
void ALMSetWarningResetRequest(ALMMAN_HANDLE *AlmMngr)
{
	AlmMngr->WrnRst = TRUE;
}
BOOL ALMGetAlarmResetStatus(ALMMAN_HANDLE *AlmMngr)
{
	return (AlmMngr->AlmRst);
}
BOOL ALMGetWarningResetStatus(ALMMAN_HANDLE *AlmMngr)
{
	return (AlmMngr->WrnRst);
}

/*****************************************************************************
  Description: Alarm History Reset Request service
*****************************************************************************/
void ALMSetAlarmHistResetRequest(ALMMAN_HANDLE *AlmMngr)
{
	AlmMngr->AlmEepRst = TRUE;
}
BOOL ALMGetAlarmHistResetStatus(ALMMAN_HANDLE *AlmMngr)
{
	return (AlmMngr->AlmEepRst);
}

/* <S1A8> Start */
/*****************************************************************************
  Description: Set the alarm code output signals (ALO1, ALO2, ALO3) 
------------------------------------------------------------------------------
  Parameters:
	AlmMngr;		Pointer to Alarm Manger handle
  Return:
	Nothing
*****************************************************************************/
void SetOutputAloSignals( ALMMAN_HANDLE *AlmMngr )
{
	UINT		idx;
	UCHAR		Attr;
	ALM_TBL		*AlmTblSp;
	BOOL		SetAlmCodeOutputSig;	/* アラームコード出力信号設定完了フラグ */
	BOOL		SetWrnCodeOutputSig;	/* ワーニングコード出力信号設定完了フラグ */


	SetAlmCodeOutputSig = FALSE;
	SetWrnCodeOutputSig = FALSE;
	idx = 0;

/* <S1A8> Start */
	/*----------------------------------------------------------------------*/
	/*	ALO1, ALO2, ALO3信号情報設定処理									*/
	/*----------------------------------------------------------------------*/
	do
	{
		/* アラームヒストリテーブルの最初に登録されているアラームの属性取得 */
		Attr = (AlmMngr->Trace.AlmTbl[idx].AlmDef)->AlmAttr;
		AlmTblSp = &AlmMngr->Trace.AlmTbl[idx];

		if( AlmTblSp->AlmId != NO_ALARM )	/* アラームが存在する場合 */
		{
			if(0 != (Attr & ALMDEF_ATTR_WARNING))
			{	/* ワーニング */
				if( AlmMngr->WrnOutSel == TRUE )
				{	/* Pn001.3 = 1 : アラームコード出力信号はアラームコードとワーニングコードの両方を出力 */
					if( SetWrnCodeOutputSig == FALSE )	/* ワーニングが先に発生していない場合 */
					{
						/* ALO1, ALO2,ALO3出力設定 */
						AlmMngr->AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01;
						AlmMngr->AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01;
						AlmMngr->AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01;
						SetWrnCodeOutputSig = TRUE;
					}
				}
			}
			else
			{	/* アラーム */
				AlmMngr->AlmCodeOut.Alo1 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 0) & 0x01;
				AlmMngr->AlmCodeOut.Alo2 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 1) & 0x01;
				AlmMngr->AlmCodeOut.Alo3 = (((AlmTblSp->AlmDef)->AlmCodeOut) >> 2) & 0x01;
				SetAlmCodeOutputSig = TRUE;
			}
		}

		idx ++;	/* インデックス更新 */
		if( idx >= AlmMngr->Trace.AlmCnt )	/* インデックス > 発生アラーム数 */
		{
			SetAlmCodeOutputSig = TRUE;
			SetWrnCodeOutputSig = TRUE;
		}
	}while( SetAlmCodeOutputSig == FALSE );


	return;
}
/* <S1A8> End */

/********************************************************************************/
/*																				*/
/*		アラームクリア要求											 <S226>		*/
/*																				*/
/********************************************************************************/
BOOL AlmClearReq( ALMMAN_HANDLE *AlmMngr, ALMID_T AlmId )
{
	if( AlmMngr->AlmClear == FALSE )
	{
		AlmMngr->ClrAlmId = AlmId;
		AlmMngr->AlmClear = TRUE;
		return( TRUE );
	}
	return( FALSE );
}

/********************************************************************************/
/*																				*/
/*		アラームクリア												 <S226>		*/
/*																				*/
/********************************************************************************/
void ALMRuntimeProcess2( ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam )
{
	ULONG			i, j, cnt, idx;
	ALMID_T			AlmId;
	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];
	
	if( AlmMngr->AlmClear == FALSE )
	{
		return;
	}
	
	AlmMngr->StopModeParam = StopModeParam;
	AlmId = AlmMngr->ClrAlmId;
	
	/*----------------------------------------------------------------------*/
	/*	アラーム登録テーブルのリセット										*/
	/*----------------------------------------------------------------------*/
	i = j = 0;
	do
	{
		*((ULONG*)&AlmMngr->Status) = 0;
		cnt = AlmMngr->Trace.AlmCnt;
		while( i < cnt)
		{
			if( AlmId != AlmMngr->Trace.AlmTbl[i].AlmId )
			{
				AlmTbl[j] = AlmMngr->Trace.AlmTbl[i];
				j++;	/* 非削除アラームのカウントアップ	*/

			}
			else
			{
				idx = AlmMngr->Trace.AlmTbl[i].AlmId >> 5;
				AlmMngr->AlmSet[idx] &= AlmMngr->AlmRstMsk[idx];
			}
			i++;
		}
	} while(OS_TestAndSet(&(AlmMngr->Trace.AlmCnt), cnt, j));
	
	/*----------------------------------------------------------------------*/
	/*	アラーム管理情報のリセット											*/
	/*----------------------------------------------------------------------*/
	/* アラーム管理情報のリセット処理を変更する為、コメントアウト */
	for(i = 0; i < j; i++)
	{
		AlmMngr->Trace.AlmTbl[i] = AlmTbl[i];
		almUpdateStatusFlags(AlmMngr, AlmTbl[i].AlmDef);
	}
	
	/*----------------------------------------------------------------------*/
	/*	ALO1, ALO2, ALO3信号情報設定処理									*/
	/*----------------------------------------------------------------------*/
	/* ALO1, ALO2, ALO3信号クリア */
	AlmMngr->AlmCodeOut.Alo1 = 0x00;
	AlmMngr->AlmCodeOut.Alo2 = 0x00;
	AlmMngr->AlmCodeOut.Alo3 = 0x00;
	
	/* アラームテーブルにアラーム、ワーニングがある場合は、ALO1, ALO2, ALO3信号情報設定する。	*/
	if( AlmMngr->Trace.AlmCnt > 0)
	{
		SetOutputAloSignals( AlmMngr );
	}
	
	AlmMngr->Trace.LstAlmCnt = j;
	AlmMngr->AlmClear        = FALSE;
}

