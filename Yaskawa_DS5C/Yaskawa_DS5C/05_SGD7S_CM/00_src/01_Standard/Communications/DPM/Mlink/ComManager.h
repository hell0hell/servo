/****************************************************************************
Description	: MECHATROLINK Communication Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef		COM_MANAGER_H_
#define		COM_MANAGER_H_

/* includes */
#include "ComCfg.h"
#include "NetDriver.h"

/***************************************************************************/
#define COM_FUNC_CONTINUE   ERR_WAIT_COMP	/* 継続実行中 */
#define COM_FUNC_NORMAL_END ERR_NONE		/* 正常終了 */
#define COM_WDC_ERR			ERR_COM_WDC		/* WDT異常 */
#define COM_PHASE_ERROR     ERR_SYN_REQ		/* 遷移先異常 */
#define COM_RECV_ERR		ERR_COM_RCVSTP	/* 通信異常 */
#define COM_RECV_WARNG		ERR_COM_RCV		/* 通信ワーニング */
#define COM_APLCYC_ERR		ERR_DATA_SET	/* 通信周期異常 */
#define COM_TCYC_ERR		ERR_COM_TCYC	/* 伝送周期異常 */
#define COM_SYNC_ERR		ERR_GBL_SYNC	/* Phase移行処理失敗 */
#define COM_WDTOUT_ERR		ERR_SYNC_TOUT	/* WDC異常 */

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
#define SINGLE_TRANSFER 	  (0)			/* 単送 */
#define CONSECUTIVE_TRANSFER  (1)			/* 連送 */

/* デフォルト設定 */
#define COM_DEF_M2TCYC 		(16000)			/* デフォルト伝送周期[0.25us] */
#define COM_DEF_M1TCYC 		(8000)			/* デフォルト伝送周期[0.25us] */
#define MAX_COM_APL_CYC 	(128000)		/* 最大通信周期[0.25us] */
#define LINK_DATA_SIZE		(32)			/* リンクデータのサイズ（バイト） */

#define COM_RECV_ERR_LIMIT 	(2)				/* 通信異常検出値 */
#define COM_BUF_A			(0)				/* トグルバッファA面 */
#define COM_BUF_B			(1)				/* トグルバッファB面 */
#define COM_ALT_BUF			(2)				/* トグルバッファ数 */

#define PH_UP_TIMEOUT		(8000)			/* Phase UPタイムアウト[ms] */

/* RSP作成周期作成テーブル */
#define DTMOD_NUM   3						/* 転送モード数 */
#define MAX_COM_CYC 32						/* 最大通信周期 */

#define COM_APLCMD_TIME		0x01			/* 通信周期CMD解析タイミング */
#define COM_APLRSP_TIME		0x02			/* 通信周期RSP作成タイミング */

/* recvErrCntの値定義 */
#define COM_RCV_ERR_STS		COM_RECV_ERR_LIMIT
#define COM_RCV_WRN_STS		1
#define COM_RCV_NOERR_STS	0

/* WDTオフセット位置 */
#define M2_WDT_OFS				(15)					/* WDTオフセット位置（バイト） */

/* サブコマンドオフセット位置 */
#define M2_EX_CMD_OFS			(4)						/* サブコマンド開始位置(ダブルワード) */

/* comChkWdcEdge関連 */
#define COM_WDC_BUF_NUM		(3)				/* WDCエッジ検出用WDC値保存バッファ数 	*//*<V592>*/
#define WDC_EDGE_PATTERN_A	(1)				/* WDCエッジパターンA 					*//*<V592>*/
#define WDC_EDGE_PATTERN_B	(2)				/* WDCエッジパターンB 					*//*<V592>*/
#define WDC_EDGE_PATTERN_C	(3)				/* WDCエッジパターンC 					*//*<V592>*/

#define CRT_CMD_DISCONNECT	0x0f			/* 受信データのコマンドがDISCONNECT 	*//*<V593>*/


/* 通信用パラメータ構造体 */
typedef struct COM_NET_PRM_ {
	LONG	tCycle;			/* 伝送周期[0.25us] */
	LONG	tSlot;			/* タイムスロット[0.25us] */
	LONG	nodeAddress;	/* 自局アドレス */
	LONG	dataSize;		/* 伝送バイト数 */
	LONG	tSpeed;			/* 伝送速度 4Mbps, 10Mbps */
	LONG	aplCycle;		/* 通信周期[x 伝送周期] */
	LONG	tType;			/* 伝送方式 単送(0)/連送(1) */

	LONG	recvErrLimt;	/* 通信異常カウント */
	BOOL	wdtErrMask;		/* 通信異常(A.E60)無視 */
	BOOL	recvErrMask;	/* ＷＤＴ異常(A.E50)無視 */
	BOOL	recvWrnMask;	/* 通信警告(A.96*)無視 */
	BOOL	enaSoftRst;		/* DISC時ソフトリセットSW */
} COM_NET_PRM;

/* COM Manager ハンドラ */
typedef struct COM_HNDL_ {
	COM_NET_PRM	NetPrm;			/* 通信設定パラメータ */

	LONG		comCycCnt;		/* 通信周期カウント値 */
	LONG		recvErrCntNum;	/* 受信エラーカウント積算値 */
	UINT32		*CmdBufPtr;		/* 現在CMDバッファ */
	UINT32		*RspBufPtr;		/* 現在RSPバッファ */
	UINT32		CmdBuffer[16];	/* 現在CMDバッファ */
	UINT32		RspBuffer[16];	/* 現在RSPバッファ */
	BOOL		wdcErrChk;		/* WDCエラーチェック[0:無効/1:有効] */
	BOOL		recvErrChk;		/* 通信エラーチェック[0:無効/1:有効] */
	LONG		RecvErr;		/* 通信異常状態 [0:正常受信/1:受信異常]*/
	LONG		LrecvErr;		/* 前回伝送周期の通信異常状態 */
	LONG		recvErrCnt;		/* 受信エラーカウンタ(受信正常でリセット) */
	UCHAR		mWdc;			/* マスタWDC */
	UCHAR		mWdcForSync;	/* 同期開始時マスタWDC */
	UCHAR		sWdc;			/* スレーブWDC */
	LONG		transMode;		/* 伝送方式 */

	BOOL		gblReadReq;		/* GBLデータ取得要求 */
	VINT32		Phase;			/* 現在通信Phase */
	VINT32		reqPhase;		/* Phase移行要求 */
	UCHAR		stateOfPhUp;	/* Phase移行処理シーケンス状態 */
	UCHAR		stateOfSync;	/* WDCエッジ検出シーケンス状態 */
	UCHAR		wdChkCnt;		/* WDCエッジ検出正常回数 */
	VINT32		PhUpSts;		/* フェーズ移行実行結果 */
	LONG		startTime;		/* 計測開始時間(ms) */
	
	LONG		wdtOfs;			/* WDTオフセット位置(バイト) */
	LONG		exCmdOfs;		/* サブコマンドオフセット位置(ワード) */
	LONG		my_ax_number;	/* 自局軸番号 */
} COM_HNDL;

/* COM Manager 公開データ取得用ＡＰＩ */
#define ComGetNetPrmaplCycle(hdl)		(hdl->NetPrm.aplCycle)
#define ComGetNetPrmtType(hdl)			(hdl->NetPrm.tType)

/****************************************************************************
 API's
****************************************************************************/
/* モジュール初期化			*/
VOID ComInit(COM_HNDL* comHdl, LONG ax_no);
/* 通信モードパラメータ設定	*/
VOID ComNetPrmSet( COM_HNDL *hdl, ND_PARAM	nd_prm );
/* PhaseUP要求				*/
INT32 ComReqPhaseUp( COM_HNDL *hdl, LONG phs, LONG mode, LONG aplCyc );
/* PhaseDOWN要求			*/
INT32 ComReqPhaseDown(COM_HNDL* hdl, LONG phs);
/* Phase移行状態確認		*/
INT32 ComGetPhaseChangeState(COM_HNDL* hdl);
/* Phase移行実処理			*/
VOID ComRunPhaseChange(COM_HNDL* hdl);
/* CMD受信					*/
INT32 ComReceiveCmd(COM_HNDL* hdl);
/* RSP送信					*/
VOID ComSendRsp(COM_HNDL* hdl);
/* 送信バッファ設定			*/
VOID ComSetRspBuffer(COM_HNDL* hdl);
/* 通信サイクルの生成		*/
VOID ComGenerateAplCycle(COM_HNDL* hdl);
/* 通信異常検出				*/
INT32 ComCheckNetError(COM_HNDL* hdl);
/* レスポンスデータ作成タイミング取得 */
BOOL ComCheckRspTiming( COM_HNDL *pComHdl );
/* 通信ＡＳＩＣ再設定処理	*/
INT32 ComResetAsicProc(COM_NET_PRM* netPrm);
#endif /* COM_MANAGER_H_ */
