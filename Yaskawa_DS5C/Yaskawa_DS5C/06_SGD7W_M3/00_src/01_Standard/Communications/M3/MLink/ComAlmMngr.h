/****************************************************************************
Description	: MECHATROLINK Alarm Manager Interface
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef COM_ALM_MNGR_H

#include "ComCfg.h"
#include "AlarmManager.h"


/* ALM Manager構造体 */
typedef struct ALM_HNDL_ {
	BOOL			almSts;					/* アラームステータス */
	BOOL			wrnSts;					/* ワーニングステータス */
	BOOL			clrReq;					/* アラームクリア要求 */
	BOOL			CommAlmBlkCancel;		/* COMM_ALM発生時BLK信号強制OFF(ブレーキ作動)フラグ */
	LONG			errCode;				/* アラーム・ワーニング表示コード */
	ALMMAN_HANDLE	*SvAlmMngr;				/* サーボアラーム情報 */
	ALM_EX_INFO		M3StdAlm;				/* M3標準プロファイル用アラーム情報 */
//	LONG			OpWrnCode;				/* パネオペ表示用ワーニングコード */
//	LONG			OpWrnTimer;				/* パネオペ表示用ワーニングコード更新タイマ */
	BOOL			CmdAlmClrMode;			/* CMD_ALM自動クリアモードフラグ */
	LONG			CmdAlmCode;				/* CMD_ALMコード */
	LONG			ExCmdAlmCode;			/* SUBCMD_ALMコード */
	LONG			CommAlmCode;			/* COMM_ALMコード */
	BOOL			PosDataNG;				/* 位置データ無効アラーム情報 */
} ALM_HNDL;


/* ALM Managerの初期化処理 */
VOID AlmInit( ALM_HNDL *hAlm, ALMMAN_HANDLE *SvAlmMngr );

/* アラーム管理処理 */
VOID AlmHandleMain( ALM_HNDL *hAlm );

/* アラーム登録 */
VOID AlmSetCode( ALM_HNDL *hAlm, LONG ax_no, INT32 alm_no );

/* CMD_ALMコードセット処理 */
VOID AlmSetCmdAlmCode( ALM_HNDL *hAlm, INT32 alm_no, BOOL ExCmd );
void AlmSetMask( ALM_HNDL *hAlm, INT32 idx, UCHAR enable );

/* CMD_ALM自動クリアモード設定処理 */
VOID AlmSetCmdAlmClearMode(ALM_HNDL *hAlm, LONG prof, ULONG comCtrl);
/* CMD_ALMクリア */
VOID AlmClearCmdSate( ALM_HNDL *hAlm, BOOL CmdAlm, BOOL ExCmdAlm );

/* アラームクリア */
INT32 AlmSetClearReq( ALM_HNDL *hAlm, BOOL req );

/* アラーム現在発生履歴取得(INDEX指定) */
//INT32 AlmGetCrtBuf( ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist );

/* アラーム現在発生履歴取得(全て) */
//INT32 AlmGetCrtBufAll( ALM_HNDL *hAlm, UCHAR *pHist );

/* アラーム現在発生履歴取得(全て、２バイト) */
INT32 AlmGetCrtDetailBufAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_num );

/* サーボアラーム発生履歴取得(INDEX指定) */
//INT32 AlmGetHistory( ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist );

/* サーボアラーム発生履歴取得(全て) */
//INT32 AlmGetHistoryAll( ALM_HNDL *hAlm, UCHAR *pHist );

/* サーボアラーム発生履歴取得(全て、２バイト) */
INT32 AlmGetHistoryDetailAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_nu );

void AlmRequestAlarmHistoryClear( ALM_HNDL *hAlm );
INT32 AlmGetAlarmHistoryClearStatus( ALM_HNDL *hAlm );



#define COM_ALM_MNGR_H
#endif /* COM_ALM_MNGR_H */



