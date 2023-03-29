/****************************************************************************
Description	: Alarm Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.11.02     created

****************************************************************************/
#ifndef ALARM_MANAGER_H

#include "alarm.h"
#include "EepromIF.h"
#include "EncData.h"

/*****************************************************************************
*		アラーム管理テーブル個数定義
*****************************************************************************/
#define	ALM_BITTBL_NUM		8	/* アラーム登録ビットテーブルロングワード数	*/
#define	KPX_ALMTBLMAX	32					/* アラーム登録テーブル最大個数	*/
#define	KPX_ALMTRCMAX	10					/* アラーム履歴テーブル最大個数	*/
#define KPX_ALMDEFMAX	(32 * ALM_BITTBL_NUM)	/* アラーム定義個数最大値	*/
#define KPX_WRNDEFMAX	(32 * WRN_BITTBL_NUM)	/* ワーニング定義個数最大値	*/


/*****************************************************************************
*	アラームHistory管理構造体定義
*****************************************************************************/
/* Alarm Table	*/
typedef struct ALM_TBL {
	ALMID_T			AlmId;					/* アラームＩＤ					*/
	CPALM_TBL_DEF	AlmDef;					/* アラーム定義					*/
	ULONG			TmStamp;				/* アラーム検出タイムスタンプ	*/
} ALM_TBL;

/* Alarm Trace Header */
typedef union ALM_TRACE_HDR {
	USHORT			w;
	struct {
		UCHAR		Idx;
		UCHAR		Cnt;
	} b;
} ALM_TRACE_HDR;

/* Alarm Trace Data Table */
typedef struct ALM_TRACE_DATA {
	USHORT			AlmCode;				/* アラームコード				*/
	ULONG			TmStamp;				/* アラーム検出タイムスタンプ	*/
} ALM_TRACE_DATA;

/* Alarm Management Data */
typedef	struct ALM_HISTORY {
	UINT			AlmCnt;					/* アラーム個数(発生中)			*/
	UINT			LstAlmCnt;				/* 履歴登録個数	<S0FF>			*/
//	UINT			LstAlmDetCnt;			/* アラーム発生個数	<S0FF>		*/

	ALM_TBL			AlmTbl[KPX_ALMTBLMAX];	/* アラーム登録テーブル(A&W共用)*/

	ALM_TRACE_HDR	AlmTrcHdr;				/* アラーム履歴Index&Count		*/
	ALM_TRACE_DATA	AlmTrc[KPX_ALMTRCMAX];	/* アラーム履歴テーブル			*/
	ALM_TRACE_DATA	*AlmTrcSp;				/* アラーム履歴前回格納ポインタ	*/

	ULONG			EepAlmOLInfo;			/* アラーム履歴過負荷情報 */
} ALM_HISTORY;


/*****************************************************************************
*	アラーム管理ＩＦ構造体定義
*****************************************************************************/
/* 通信用拡張アラーム情報定義 */
typedef struct ALM_EX_INFO {
	BOOL	DevAlm;						/*	機器アラーム */
	BOOL	DevWrn;						/*	機器ワーニング	*/
	BOOL	CmdAlm;						/*	コマンドアラーム */
	BOOL	CommAlm;					/*	通信アラーム */
} ALM_EX_INFO;

/*	<S0FF>	*/
typedef struct ALM_STATUS {
	BOOL	AlmFlag			:1;			/* アラーム発生中フラグ				*/
	BOOL	WrnFlag			:1;			/* ワーニング発生中フラグ			*/
	BOOL	MencDataNG		:1;			/* モータエンコーダデータ無効		*/
	BOOL	PosDataNG		:1;			/* 位置データ無効					*/

	BOOL	DevAlm			:1;			/*	機器アラーム */
	BOOL	DevWrn			:1;			/*	機器ワーニング	*/
	BOOL	CmdAlm			:1;			/*	コマンドアラーム */
	BOOL	CommAlm			:1;			/*	通信アラーム */

	UINT	StopMode		:4;
	UINT	AlmOLInfo		:4;

	USHORT	AlmCode;					/* アラームコード情報				*/
} ALM_STATUS;


/* パラメータエラー情報定義	*/
typedef	struct PRMERR {
	UCHAR	ErrFlag;					/* エラーフラグ(オンライン用)		*/
	UCHAR	ErrCount;					/* エラーカウント(オフライン用)		*/
	USHORT	ErrPrmNo;					/* パラメータ番号(パラメータエラー)	*/
} PRMERR;

/* アラームコード情報定義	*//* <S04B> */
typedef	struct ALM_CODE_OUT {
	BOOL	Alo1;						/* ALO1 */
	BOOL	Alo2;						/* ALO2 */
	BOOL	Alo3;						/* ALO3 */
} ALM_CODE_OUT;


/* アラーム管理ＩＦデータ定義 */
typedef	struct ALMMAN_HANDLE {
/* Public member */
	BOOL			AlmRst;				/* ALMリセット要求					*/
	BOOL			WrnRst;				/* Warningリセット要求				*/
	BOOL			NetCmdAlmRst;		/* 通信用CMD_ALMリセット要求		*/
	BOOL			AlmEepRst;			/* EEPROMアラーム履歴要求Flag		*/

	ALM_STATUS		Status;				/* alarm status <S0FF>				*/
	BOOL			AlmReady;			/* アラームレディ					*/
	BOOL			WrnFlagX;			/* <S0FF>							*/
//<S0FF>	BOOL			AlmFlag;			/* アラーム発生中フラグ				*/
//<S0FF>	BOOL			WrnFlag;			/* ワーニング発生中フラグ			*/
//<S0FF>	BOOL			MencDataNG;			/* モータエンコーダデータ無効		*/
//<S0FF>	BOOL			PosDataNG;			/* 位置データ無効					*/
//<S0FF>	ALM_EX_INFO		AlmExInfo;			/* 拡張用アラーム情報				*/
//<S0FF>	ULONG			AlmOLInfo;			/* アラーム履歴過負荷情報			*/
//<S0FF>	ULONG			StopMode;			/* アラーム発生時の停止方法			*/

	USHORT			AlmCode;			/* アラームコード情報				*/
//	USHORT			AlmCodeOut;			/* アラームコード信号出力			*//* <S04B> */
	ALM_CODE_OUT	AlmCodeOut;			/* アラームコード信号出力			*//* <S04B> */
	PRMERR			PrmErrInfo;			/* パラメータエラー情報				*/
	ALM_HISTORY		Trace;				/* Alarm trace information			*/

/* Private member(do not write these values from other modules) */
	ULONG			AlmSet[ALM_BITTBL_NUM];		/* アラーム登録ビット		*/
	ULONG			AlmSetMsk[ALM_BITTBL_NUM];	/* アラームマスクビット		*/
	ULONG			AlmRstMsk[ALM_BITTBL_NUM];	/* アラームリセット不可ビット*/
	ULONG			WrnRstMsk[ALM_BITTBL_NUM];	/* アラームリセット不可ビット*/

	ULONG			StopModeParam;		/* アラーム発生時の停止方法			*/
	CPALM_TBL_DEF	DefTable;			/* Pointer to Alarm table */
	UINT			TableEntryNum;		/* Number of Table entry */
	void			*mutex;				/* for Interlock */

	UINT				myAsicNo;		/* axis number */
//	EEP_DEVICE_HANDLE	*EepDevice;		/* Eeprom Device handle *//* <Oka02S1D2> */
	EEP_QUE_BUFFER		EepQueBuf[32];	/* Eeprom Queue buffer */
	EEP_QUE_HANDLE		EepQue;			/* Eeprom Queue */
	volatile BOOL		AlmEepRstBusy;	/* EEPROMアラーム履歴BusyFlag		*/

	USHORT				IntAlmId;		/* 起動時検出アラームID */
	BOOL				IntAlmEnable;	/* 起動時検出アラーム有効 */
	BOOL				OCAlmOccurFlag1;/* 過電流(OC1)アラーム(A.100)発生フラグ(内部出力信号モニタ用) */	/* <S1A0> *//* <S1C9> */
	BOOL				OCAlmOccurFlag2;/* 過電流(OC2)アラーム(A.100)発生フラグ(内部出力信号モニタ用) */	/* <S1C9> */
	BOOL				WrnOutSel;		/* ワーニングコード出力選択 */	/* <S1A8> */

	USHORT				LtAlmReq;		/* アラームラッチEEPROM書き込み要求				 <S1CF> */
	USHORT				LtDtWtCt;		/* アラームラッチデータ書き込み中カウンタフラグ	 <S1CF> */
	ULONG				LtRomAdr;		/* 書き込みEEPROMアドレス						 <S1CF> */
	USHORT				*LtMemAdr;		/* 書き込みメモリーアドレス						 <S1CF> */
	USHORT				*AlmCdAdr;		/* アラームコード格納アドレス					 <S1CF> */
	ULONG				*TmStpAdr;		/* アラームタイムスタンプ格納アドレス			 <S1CF> */

	void				*hAlmLatchHndl;	/* アラームラッチ用データ						 <S212> */

	BOOL				AlmClear;		/* 特定アラームのクリア要求						 <S226> */
	ALMID_T				ClrAlmId;		/* クリアアラームＩＤ							 <S226> */

} ALMMAN_HANDLE;



/* API's */

/* for all axises */
void ALMInitGlobalHandle(ALMMAN_HANDLE **AxisTopHandle, LONG AxisNum);
void ALMSetGlobalAlarm(ALMID_T alm_id);
void ALMClearGlobalAlarm(ALMID_T alm_id);
BOOL ALMCheckGlobalAlarm(ALMID_T alm_id);

/* for an axis */
void ALMInitAlarmManager(ALMMAN_HANDLE *AlmMngr, CPALM_TBL_DEF AlmDefTbl,
						 UINT AlmDefCntx, UINT AxisNumber);
ULONG ALMStartupProcedure(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam,
										LONG AxisMotType, LONG AbsoEncType);
void ALMRuntimeProcess(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam);
void ALMRuntimeProcess2(ALMMAN_HANDLE *AlmMngr, UCHAR StopModeParam);		/* <S226> */
void ALMHistoryWriteService(ALMMAN_HANDLE *AlmMngr);

CPALM_TBL_DEF ALMSetServoAlarm(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
void ALMSetMask(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id, BOOL MaskSet);
BOOL ALMCheckEachState(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
BOOL ALMCheckSetMask(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);	/* @@ add */
void ALMSetPramError(ALMMAN_HANDLE *AlmMngr, USHORT ErrPrmNo);
void ALMClearWarning(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
BOOL ALMGetOtherAlarmChk(ALMMAN_HANDLE *AlmMngr, ALMID_T alm_id);
ULONG ALMGetStatus(ALMMAN_HANDLE *AlmMngr, BOOL *AlmFlag, BOOL *WrnFlag);
BOOL ALMGetExNetStatus(ALMMAN_HANDLE *AlmMngr, ALM_EX_INFO *AlmExInfo);
void ALMGetNetAlarmTableInfo(ALMMAN_HANDLE *AlmMngr, USHORT *AlmInfo, UINT Index);
void ALMGetNetAlarmTraceInfo(ALMMAN_HANDLE *AlmMngr, USHORT *AlmInfo, UINT Index);
USHORT ALMGetAlarmHistory(ALMMAN_HANDLE *AlmMngr, UINT Index, ULONG *TimeStamp);
LONG ALMGetNetAlarmTableCode(ALMMAN_HANDLE *AlmMngr, UCHAR *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmTableDetailCode(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmNum(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetNetWarningNum(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmTraceCode(ALMMAN_HANDLE *AlmMngr, UCHAR *AlmCode, LONG MaxInfo);
LONG ALMGetNetAlarmTraceDetailCode(ALMMAN_HANDLE *AlmMngr, USHORT *AlmCode, LONG MaxInfo);
LONG ALMGetAlarmInfo(ALMMAN_HANDLE *AlmMngr);
void ALMSetAlarmResetRequest(ALMMAN_HANDLE *AlmMngr);
void ALMSetWarningResetRequest(ALMMAN_HANDLE *AlmMngr);
BOOL ALMGetAlarmResetStatus(ALMMAN_HANDLE *AlmMngr);
BOOL ALMGetWarningResetStatus(ALMMAN_HANDLE *AlmMngr);
void ALMSetAlarmHistResetRequest(ALMMAN_HANDLE *AlmMngr);
BOOL ALMGetAlarmHistResetStatus(ALMMAN_HANDLE *AlmMngr);
void SetOutputAloSignals( ALMMAN_HANDLE *AlmMngr );	/* <S1A8> */
void LcdEep2EepRomWrite( UINT devId );	/* <S208> */

#define ALARM_MANAGER_H
#endif /* ALARM_MANAGER_H */
