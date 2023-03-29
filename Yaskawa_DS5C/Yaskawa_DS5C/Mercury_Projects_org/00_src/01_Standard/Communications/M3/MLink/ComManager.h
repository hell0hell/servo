/****************************************************************************
Description	: MECHATROLINK Communication Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef COM_MANAGER_H_
#define COM_MANAGER_H_

#include "ComCfg.h"
#include "NetDriver.h"

/***************************************************************************/
#define MCOM_FUNC_NORMAL_END	0x0						/* 正常終了 */
#define COM_FUNC_CONTINUE   	0x1						/* 継続実行中 */
#define COM_FUNC_NORMAL_END		0x0						/* 正常終了 */
#define COM_FUNC_ERROR_END		-1						/* 異常終了 */
#define COM_WDC_ERR				ERR_COM_WDC				/* WDT異常 */
#define COM_PHASE_ERROR     	ERR_SYN_REQ				/* 遷移先異常 */
#define COM_RECV_ERR			ERR_COM_RCVSTP			/* 受信異常 */
#define COM_FCS_ERR				ERR_COM_FCS_A			/* FCS異常 */
#define COM_RECV_GBL_ERR		ERR_GBL_COM_A			/* グローバルデータ受信異常 */
#define COM_RECV_SYNC_ERR		ERR_GBL_SYNC			/* 同期通信異常 */
#define COM_RECV_WARNG			ERR_COM_RCV				/* 受信異常ワーニング */
#define COM_FCS_WARNG			ERR_COM_FCS_W			/* FCSワーニング */
#define COM_RECV_GBL_WARNG		ERR_GBL_COM_W			/* グローバルデータ受信ワーニング */
#define COM_APLCYC_ERR			ERR_DATA_SET			/* 通信周期異常 */
#define COM_TCYC_ERR			ERR_COM_TCYC			/* 伝送周期異常 */
#define COM_SYNC_ERR			ERR_GBL_SYNC			/* Phase移行処理失敗 */
#define COM_WDTOUT_ERR			ERR_SYNC_TOUT			/* WDC異常 */
#define COM_CHGE_BUFF_ERROR		ERR_COM_LSISYS			/* 交替バッファ切り替え失敗 */
#define	COM_RECV_ASYNC_ERR		ERR_COM_RCV_ASYNC_A		/* 受信異常(イベントドリブン通信) */
#define	COM_RECV_ASYNC_WARNG	ERR_COM_RCV_ASYNC_W		/* 受信警告(イベントドリブン通信) */

#define COM_WDT_OFS_ERR			(-1)					/* WDTオフセット値異常 */
#define COM_EXCMD_OFS_ERR		(-2)					/* サブコマンドオフセット値異常 */

#define COM_DATA_SIZE_ERR_BIT	ND_DATA_SIZE_ERR_BIT	/* 通信データサイズ設定異常ビット */
#define COM_AXIS_NO_ERR_BIT		ND_AXIS_NO_ERR_BIT		/* 局アドレス設定異常ビット */
#define COM_ASIC_STATUS_ERR_BIT	ND_ASIC_STATUS_ERR_BIT	/* JL-100ステータス異常ビット */


/* Phase移行処理シーケンス状態 */
typedef enum COM_PHUP_STEP_ {
	PH_IDOL = 0,										/* 定常状態 */
	PH_RESET,											/* Phase初期化状態  */
	PH_CHK_COM_CYCLE,									/* 通信周期チェック状態 */
	PH_SYNC_COMCYC,										/* 通信周期同期状態 */
} COM_PHUP_STEP;

/* 通信周期移行シーケンス状態 */
typedef enum COM_SYNC_STEP_ {
	DISABLE_SYNC = 0,									/* 通信周期 非同期 */
	START_COMCYC,										/* マスタＷＤＣエッジ検出開始 */
	READY_COMCYC,										/* マスタＷＤＣエッジ検出 */
	START_WDCCHK,										/* ＷＤＣチェック開始 */
	ENA_WDCCHK,											/* ＷＤＣチェック有効 */
} COM_SYNC_STEP;

/* データ転送方式 */
#define SINGLE_TRANSFER 		(0)						/* 単送 */
#define CONSECUTIVE_TRANSFER	(1)						/* 連送 */


/* デフォルト設定 */
#define MAX_COM_APL_CYC 		(128000)				/* 最大通信周期[0.25us] */
#define T_CYCLE_125US			(500)					/* 伝送周期125us[0.25us] */

#define COM_RECV_ERR_LIMIT 		(2)						/* 通信異常検出値 */
#define COM_BUF_A				(0)						/* トグルバッファA面 */
#define COM_BUF_B				(1)						/* トグルバッファB面 */
#define COM_ALT_BUF				(2)						/* トグルバッファ数 */

#define PH_UP_TIMEOUT			(8000)					/* Phase UPタイムアウト[ms] */


/* RSP作成周期作成テーブル */
#define DTMOD_NUM				(3)						/* 転送モード数 */
#define MAX_COM_CYC				(32)					/* 最大通信周期 */

#define COM_APLCMD_TIME			(0x01)					/* 通信周期CMD解析タイミング */
#define COM_APLRSP_TIME			(0x02)					/* 通信周期RSP作成タイミング */


/* recvErrCntの値定義 */
#define COM_RCV_ERR_STS			COM_RECV_ERR_LIMIT
#define COM_RCV_WRN_STS			(1)
#define COM_RCV_NOERR_STS		(0)

#define CRT_CMD_DISCONNECT		(0x0f)					/* 受信データのコマンドがDISCONNECT */


#ifdef __BIG_ENDIAN__
/* WDTオフセット位置 */
#define M3STD_WDT_OFS			(2)						/* WDTオフセット位置（バイト） */
#define M2_WDT_OFS				(12)					/* WDTオフセット位置（バイト） */
#define EVTDRV_WDT_OFS			(2)						/* WDTオフセット位置（バイト） */

/* サブコマンドオフセット位置 */
#define M3STD_EX_CMD_OFS		(8)						/* サブコマンド開始位置(ダブルワード) */ 
#define M2_EX_CMD_OFS			(4)						/* サブコマンド開始位置(ダブルワード) */
#define EVTDRV_EX_CMD_OFS		(8)						/* サブコマンド開始位置(ダブルワード) */
#else
/* WDTオフセット位置 */
#define M3STD_WDT_OFS			(1)						/* WDTオフセット位置（バイト） */
#define M2_WDT_OFS				(15)					/* WDTオフセット位置（バイト） */
#define EVTDRV_WDT_OFS			(1)						/* WDTオフセット位置（バイト） */

/* サブコマンドオフセット位置 */
#define M3STD_EX_CMD_OFS		(8)						/* サブコマンド開始位置(ダブルワード) */ 
#define M2_EX_CMD_OFS			(4)						/* サブコマンド開始位置(ダブルワード) */
#define EVTDRV_EX_CMD_OFS		(8)						/* サブコマンド開始位置(ダブルワード) *//*仮処置*/
#endif


/* 通信異常状態 */
#define RECV_NORMAL				(0)						/* 正常受信 */
#define RECV_CMD_ERR			(1)						/* 受信異常 */
#define RECV_FCS_ERR			(2)						/* FCS異常  */
#define RECV_GBL_ERR			(3)						/* グローバルデータ受信異常  */
#define	RECV_ASYNC_CMD_ERR		(4)						/* 受信異常(非同期通信) */


/* 非同期通信 */
#define COM_ASYNC_RCV_SUCCESS	ND_ASYNC_RCV_SUCCESS	/* 非同期通信 正常終了（受信) */
#define COM_ASYNC_SEND_SUCCESS	ND_ASYNC_SEND_SUCCESS	/* 非同期通信 正常終了（送信) */
#define COM_SENDING_FRAME		ND_SENDING_FRAME		/* 非同期通信 データ送信中 */
#define COM_SEND_FRAME_ERR		ND_SEND_FRAME_ERR		/* 非同期通信 送信異常 */
#define COM_ASYNC_RCV_ERR		ND_ASYNC_RCV_ERR		/* 非同期通信 受信異常 */
#define COM_ASYNC_TOUT			ND_ASYNC_TOUT			/* 非同期通信 受信処理タイムアウト */


/* 通信用パラメータ構造体 */
typedef struct COM_NET_PRM_ {
	LONG	nodeAddress;		/* 自局アドレス *//* <S07A> */
	LONG	dataSize;			/* 伝送バイト数 *//* <S07A> */
	
	LONG	aplCycle;			/* 通信周期[x 伝送周期] */
	LONG	tType;				/* 伝送方式 単送(0)/連送(1) */
	LONG	recvErrLimt;		/* 通信異常カウント */

	BOOL	wdtErrMask;			/* 通信異常(A.E60)無視 */
	BOOL	recvErrMask;		/* ＷＤＴ異常(A.E50)無視 */
	BOOL	recvWrnMask;		/* 通信警告(A.96*)無視 */
} COM_NET_PRM;



/* COM Manager ハンドラ */
typedef struct COM_HNDL_ {
	COM_NET_PRM		NetPrm;				/* 通信設定パラメータ */

	LONG			comCycCnt;			/* 通信周期カウント値 */
	USHORT			recvErrCntNum;		/* 受信エラーカウント積算値 */
	UINT32			*CmdBufPtr;			/* 現在CMDバッファ */
	UINT32			*RspBufPtr;			/* 現在RSPバッファ */
	UINT32			CmdBuffer[16];		/* 現在CMDバッファ */
	UINT32			RspBuffer[16];		/* 現在RSPバッファ */
	BOOL			wdcErrChk;			/* WDCエラーチェック[0:無効/1:有効] */
	BOOL			recvErrChk;			/* 通信エラーチェック[0:無効/1:有効] */
	LONG			RecvErr;			/* 通信異常状態 [0:正常受信/1:受信異常/2:FCS異常]*/
	LONG			LrecvErr;			/* 前回伝送周期の通信異常状態 */
	LONG			recvErrCnt;			/* 受信エラーカウンタ(受信正常でリセット) */
	UCHAR			mWdc;				/* マスタWDC */
	UCHAR			mWdcForSync;		/* 同期開始時マスタWDC */
	UCHAR			sWdc;				/* スレーブWDC */
	LONG			transMode;			/* 伝送方式 */

	VINT32			Phase;				/* 現在通信Phase */
	VINT32			reqPhase;			/* Phase移行要求 */
	COM_PHUP_STEP	stateOfPhUp;		/* Phase移行処理シーケンス状態 */
	COM_SYNC_STEP	stateOfSync;		/* WDCエッジ検出シーケンス状態 */
	LONG			wdChkCnt;			/* WDCエッジ検出正常回数 */
	VINT32			PhUpSts;			/* フェーズ移行実行結果 */
	ULONG			startTime;			/* 計測開始時間(ms) */
	
	LONG			wdtOfs;				/* WDTオフセット位置(バイト) */
	LONG			exCmdOfs;			/* サブコマンドオフセット位置(ワード) */
	LONG			my_ax_number;		/* 自局軸番号 */
} COM_HNDL;



/****************************************************************************
 API's
****************************************************************************/

/* COM Manager初期化処理 */
VOID ComInit( COM_HNDL *hdl, LONG ax_no );
/* 通信モードパラメータ設定	*//* <S07A> */
VOID ComNetPrmSet( COM_HNDL *hdl, ND_PARAM	nd_prm );
/* 通信Phase遷移(Phaseアップ)要求処理 */
INT32 ComReqPhaseUp( COM_HNDL *hdl, LONG phs, LONG mode, LONG aplCyc, BOOL AsyncMode );
/* 通信Phase遷移要求(Phaseダウン)処理 */
INT32 ComReqPhaseDown( COM_HNDL *hdl, LONG phs );
/* 通信Phase遷移実行処理 */
VOID ComRunPhaseChange( COM_HNDL *hdl );

/* 自局受信ステータス取得処理 */
//INT32 ComReceiveCmd( COM_HNDL *hComMngr );
/* 自局データの送信(ＡＳＩＣへのライト)処理 */
//VOID ComSendRsp( COM_HNDL *hComMngr );
/* スレーブWDCカウントアップ、送信用RSPバッファポインタ切替え処理 */

VOID ComSetRspBuffer( COM_HNDL *hComMngr );
/* 通信周期サイクルの生成処理 */
//VOID ComGenerateAplCycle( COM_HNDL *pComHdl, INT32 RecvErr );
VOID ComGenerateAplCycle( COM_HNDL *pComHdl );
/* 通信エラー(受信エラー、WDCエラー)の検出処理 */
INT32 ComCheckNetError( COM_HNDL *hdl );
/* WDTオフセット位置設定処理 */
INT32 ComSetWdtOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG wdtOfs );
/* サブコマンドオフセット位置設定処理 */
INT32 ComSetExCmdOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG exCmdOfs );
/* レスポンスデータ作成タイミング取得 */
BOOL ComCheckRspTiming( COM_HNDL *pComHdl );



#endif /* COM_MANAGER_H_ */


