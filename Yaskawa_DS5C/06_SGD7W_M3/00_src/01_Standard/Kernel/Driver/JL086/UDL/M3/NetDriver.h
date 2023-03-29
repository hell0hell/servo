/****************************************************************************
Description	: MECHATROLINK Communication Driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
 Name   Date            Description
----------------------------------------------------------------------------
 Y.Oka	2010.02.23		created.
 I.M.   2011.01.18		reviewed. 

****************************************************************************/
#ifndef NET_DRIVER_H_
#define NET_DRIVER_H_


#ifdef __BIG_ENDIAN__
#include "big_jl100_def.h" 
#else
#include "little_jl100_def.h"
#endif


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/

/* PHYのCH定義 */
#define ND_PORT_CH1					(0)			/* PHYのCH1 */
#define ND_PORT_CH2					(1)			/* PHYのCH2 */

/* Error code definitions */
#define ND_SUCCESS					(0)			/* 正常終了 */
#define ND_CONTINUE					(1)			/* 処理実行中 */
#define ND_COMM_STAT_RESET			(2)			/* コントローラが通信状態をリセットした */
#define ND_RCVED_TMC_FRAME			(3)			/* TMCフレーム受信 */
#define ND_NOT_RCV_TMC_FRAME		(4)			/* TMCフレーム未受信 */

#define ND_JL100_READY_TIMEOUT		(-1)		/* JL-100起動処理タイムアウト */
#define ND_JL100_PHY_INIT_ERR		(-2)		/* JL-100 PHYレジスタ初期化失敗 */
#define ND_SRAM_CHECK_ERROR			(-3)		/* JL-100内部RAMのリード・ライトチェック失敗 */
#define ND_JL100_INIT_ERROR			(-4)		/* JL-100初期化失敗 */

#define ND_MEASURE_TRANSDLY_ERROR	(-5)		/* 遅延計測要求失敗 */
#define ND_DETECT_SLV_CMP_ERROR		(-6)		/* 現在接続局検出失敗 */
#define ND_GET_CMODE_ERROR			(-7)		/* 通信モード取得失敗 */
#define ND_START_SYNC_ERROR			(-8)		/* 同期通信スタート失敗 */
#define ND_START_ASYNC_ERROR		(-9)		/* 非同期通信スタート失敗 */
#define ND_PORT_SEL_ERROR			(-10)		/* 未サポート通信モード選択 */

#define ND_CHANGE_CBUFF_ERROR		(-11)		/* 交替バッファ切り替え失敗 */
#define ND_NOT_SYNC_ERROR			(-12)		/* 同期通信異常 */
#define ND_GBL_RCV_ERROR			(-13)		/* 同期フレーム受信異常 */
#define ND_READ_CDATA_ERROR			(-14)		/* 指令データ読み出し失敗 */
#define ND_WRITE_RDATA_ERROR		(-15)		/* 応答データ書き込み失敗 */
#define ND_CHANGE_RBUFF_ERROR		(-16)		/* 交替バッファ切り替え失敗 */

#define ND_INIT_SEQ_ERROR			(-17)		/* JL-100初期化処理シーケンス異常 */
#define ND_GET_MYSTAT_ERROR			(-18)		/* 自局の指令データの受信異常 */

#define ND_DATA_READ_ERROR			(-19)		/* サイクリックデータ読み出し失敗 */
#define ND_DATA_RECV_ERROR			(-20)		/* サイクリックデータ受信失敗 */

/* JL-100 エラー情報 */
#define ND_DATA_SIZE_ERR_BIT		(1)			/* 通信データサイズ設定異常ビット */
#define ND_AXIS_NO_ERR_BIT			(2)			/* 局アドレス設定異常ビット */
#define ND_FCS_ERR_BIT				(4)			/* FCS異常ビット */
#define ND_ASIC_STATUS_ERR_BIT		(8)			/* JL-100ステータス異常ビット */

/* Message Communication SCF暫定 */
#define ND_RECV_BUSY				(2)			/* MSG受信待機状態 */
#define ND_SEND_BUSY				(1)			/* MSG送信中状態 */
#define ND_SUCCESS					(0)			/* 正常終了 */
#define ND_RECVMSG_ERR				(-1)		/* MSG受信エラー */
#define ND_RECVADR_ERR				(-2)		/* MSG受信アドレスエラー */
#define ND_SENDMSG_ERR				(-3)		/* MSG送信エラー */

/* イベントドリブン通信 */
#define ND_ASYNC_RCV_SUCCESS		(0)			/* 非同期通信 正常終了（受信) */
#define ND_ASYNC_SEND_SUCCESS		(1)			/* 非同期通信 正常終了（送信) */
#define ND_SENDING_FRAME			(2)			/* 非同期通信 データ送信中 */

#define ND_SEND_FRAME_ERR			(-1)		/* 非同期通信 送信異常 */
#define ND_ASYNC_RCV_ERR			(-2)		/* 非同期通信 受信異常 */
#define ND_ASYNC_TOUT				(-3)		/* 非同期通信 受信処理タイムアウト */

#define ND_ASYNC_REQ_RCV_SEQ		(0)			/* フレーム受信要求シーケンス	*//* <S0EC> */
#define ND_ASYNC_RCV_SEQ			(1)			/* 指令データ受信シーケンス		*/
#define ND_ASYNC_RCVBUF_SEQ			(2)			/* 指令データ受信シーケンス		*/
#define ND_ASYNC_SND_SEQ			(3)			/* 応答データ送信シーケンス		*/
#define ND_ASYNC_WAIT_SND_SEQ		(4)			/* 応答データ送信中シーケンス	*/

#define MASK_INT1					0x0001

#define DEF_WDT						(10000) 	/* WDT setting 4000us×20=80000us */
#define WDT_NOTCHK					(0)			/* ASICでホストCPUのWDTをチェックしない設定 */
#define DEF_MA_MAX					(MAX_AXIS_NUMBER)	/* 接続スレーブ数 */
#define JL100_READY_TIMEOUT			(10000)		/* JL100起動処理タイムアウト時間 = 10s(10000ms) */

/* Async. communication parameter */
#define DEF_ASYNC_PEER_ADR			(0x01)		/* Destination address of Async. communication */
#define DEF_ASYNC_FTYPE				(0x02)		/* Frame type of async communication(0x02 or 0x0C) */
#define DEF_ASYNC_DATA_SIZE			(64)		/* Async. communication data size (unit : byte) */

/* C1/C2 message communication parameter */
#define MSG_PKT_SIZE				(64)		/* ひとつのメッセージフレームで送信するデータ長（バイト） */

/* モニタスレーブ機能パラメータ */
#define MSLV_RSP_DATA				(0)			/* モニタスレーブ機能 応答データ受信 */
#define MSLV_CMD_DATA				(1)			/* モニタスレーブ機能 指令データ受信 */

#define ND_MAX_DATA_SIZE			(48)		/* サイクリックデータの最大受信サイズ */
#define RECV_STAT_REG_BIT_SIZE		(32)		/* 受信ステータスレジスタのビット数 */

#define PHYPORTADR_CH1				(2)			/* PHYのポートアドレス ch1 */
#define PHYPORTADR_CH2				(3)			/* PHYのポートアドレス ch2 */

/* Message buffer size */
#define ML_MSGBUF_SZ				512			/* 480+32 */

/* Error code */
#define ML_FUNC_SUCCESS				(0)
#define ML_RAMCHECK_ERR				(-1)
#define ML_RECV_ERR					(-2)
#define ML_ABORT_ERR				(-3)
#define ML_HNDSHK_ERR				(-4)
#define ML_RXBUF_ERR				(-5)
#define ML_TXBUF_ERR				(-6)



/****************************************************************************/
/*		STRUCT DEFINITION													*/
/****************************************************************************/
/* 通信パラメータ構造体 */
typedef struct ND_PARAM_ {
	USHORT	TCycle;			/* 伝送周期[0.25us] */
	UCHAR	NodeAddress;	/* 自局アドレス */
	UCHAR	DataSize;		/* 伝送バイト数 */
	UCHAR	MslvEnable;		/* モニタスレーブモード有効[1:有効 0:無効] */
} ND_PARAM;

/* Async. communication parameter structure */
typedef struct ASYNC_ND_PARAM_ {
	USHORT	rcvSrcAdr;		/* Received source address of async com. buffer */
	USHORT	rcvDstAdr;		/* Received destination address of async com. buffer */
	USHORT	rcvStat;		/* Receive frame status of async com. buffer */
	USHORT	rcvFtype;		/* Receive frame type of async com. buffer */
	USHORT	rcvSize;		/* Receive data size of async com. buffer */
	USHORT	sw;				/* Async. communication sequence flag */
} ASYNC_ND_PARAM;

/* typedefs of struct type */
typedef union _MLWD_DATA {
	USHORT wd;
	UCHAR  byte[2];
} MLWD_DATA;




/****************************************************************************/
/*		API's																*/
/****************************************************************************/
/* JL-100初期化 */
INT32 NdInit( ND_PARAM *nd_prmp, void* DeviceAddress );
/* 通常通信準備 */
INT32 NdNormalCommunicationIsReady( USHORT *t_mcycp, ULONG *t_int_ofs );
/* TMCフレームの受信確認 */
//SHORT NdReceiveTmcFrame( VOID );
/* CMDデータ取得 */
INT32 NdReceiveCMD( UINT st_no, ULONG *CmdBuffer );
/* RSPデータ送信 */
void NdSendRSP( UINT st_no, ULONG *RspBuffer );
/* 通信状態取得 */
INT32 NdBeginCommunication( ULONG *ests );
/* 伝送周期取得処理 */
USHORT NdGetTransmissionCycle( VOID );
/* 通信モード取得 */
USHORT NdGetComMode( VOID );
/* 非同期通信シーケンスフラグ取得 */
USHORT NdGetAsyncSw( VOID );
/* イベントドリブン通信時の通信データ受信（通信ドライバ） */
//ULONG NdBeginAsyncCyclicCmdRead( void );
/* イベントドリブン通信時の通信データ送信（通信ドライバ） */
//void NdSendAsyncRSP(ULONG st_no, ULONG *async_rbuff );
/* メッセージ用通信ドライバ */
LONG NdReqRecvMsg( UCHAR ch );
/* メッセージ受信完了チェック */
LONG NdGetRecvMsgSts( UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz );
/* メッセージデータリード */
void NdReadMsgData( UCHAR ch, USHORT *rcvData, USHORT rdSz );
/* メッセージデータライト */
void NdWriteMsgData( UCHAR ch, USHORT *sndData, USHORT wrSz );
/* メッセージ送信開始要求セット */
LONG NdReqSendMsg( UCHAR ch );
/* メッセージ送信完了チェック */
LONG NdGetSendMsgSts( UCHAR ch );
/* アボートメッセージ送信 */
void NdSendMsgAbort(UCHAR ch);
/* 通信パラメータ取得 */
ND_PARAM* NdGetNdParamPtr( VOID );
/* 通信ASICリセット */
void NdResetAsic( void *DeviceAddress ); /* <S14E> */

/* for DMA transfer */
INT32 NdChangeTglBufAddress(ULONG *RcvAltBufOfst, ULONG *SndAltBufOfst, UINT alt_no);
void *NdGetRecvCycDevBufAddress(UINT ch);
void *NdGetSndCycDevBufAddress(UINT ch);
void *NdGetRecvCycBufferPointer(UINT ch);
void *NdGetSndCycBufferPointer(UINT ch);
ULONG NdGetSndAltBufferAddress(void);
ULONG NdGetRecvAltBufferAddress(void);

VOID NdToggleChange( VOID );

/****************************************************************************/
#endif /* NET_DRIVER_H_ */
/* End of File */

