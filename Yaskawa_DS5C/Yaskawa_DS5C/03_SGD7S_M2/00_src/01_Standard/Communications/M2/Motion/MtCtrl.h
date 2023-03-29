/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Control ヘッダファイル										*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	POSING用加速度指令追加							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTION_CTRL_H_

#include "MtGenerator.h"
#include "Position.h"
#include "AccFilter.h"
#include "MtPRM.h"
#include "ServoIf.h"
#include "PRMBank.h"/* <S004> */

/****************************************************************************
 Defines
****************************************************************************/
#define MOTOR_STP_SPD	0x10000		/* モータ停止速度 24bit/OS / 256 */

enum MT_OTSTATE {
	OTSTAT_NORMAL=0,	/* Disable状態 */
	OTSTAT_STOP,		/* 停止中状態 */
	OTSTAT_WAITNORMAL,	/* OT解除待ち状態 */
	OTSTAT_RETRACTION,	/* 引き戻し動作状態 <V679> */
};

#define MTLT_NO_SEQ_REQ	0	/* ラッチシーケンスなし状態 */
#define MTLT_SEQ_REQ	1	/* ラッチシーケンス起動状態 */
#define MTLT_SEQ_DONE	2	/* ラッチシーケンス完了状態 */
#define MTLT_CMP_STS	0x11	/* ラッチ完了状態 */

#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
enum MT_AMSTATE {
	AMSTAT_NORMAL=0,	/* Disable状態 */
	AMSTAT_STOP,		/* 停止中状態 */
	AMSTAT_WAITNORMAL,	/* OT解除待ち状態 */
};
#endif

/****************************************************************************
 Structure of the Motion Handle
****************************************************************************/
typedef struct MT_LATCH_ {
	LONG	Req;		/* ラッチ要求 */
	LONG	Cmp;		/* ラッチ完了 */
	LONG	Mode;		/* ラッチモード */
	LONG	Sgnl;		/* 選択信号 */

	LONG	LtDisable;	/* ラッチ無効 */
	LONG	SeqReq;		/* 連続ラッチ要求 */
	LONG	SeqCnt;		/* 連続ラッチシーケンスカウンタ */
	union {				/* 連続ラッチステータス */
		USHORT	wdCnt;
		UCHAR	cnt[2];	/* [0]:連続回数カウント,[1]:シーケンスカウント */
	}SeqSts;
} MT_LATCH;

typedef struct MT_CMD_ {
	LONG	TPos;		/* 目標位置[ref] */
	LONG	TSpeed;		/* 目標速度[ref/s] */
	LONG	Velocity;	/* VFF[re/s], 速度指令[2^30/MAX] */
	LONG	Trq;		/* トルク指令[MAX_TRQ/2^30] */
	LONG	HoldMod;	/* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	LONG	AccFilSel;	/* フィルタ選択 0,3: なし, 1:指数,2:移動平均 */
	LONG	LatchSgnl;	/* ラッチ信号選択 0:C相,n:EXTn */
	LONG	Mtype;		/* モーションタイプ */
	LONG	CtrlMode;	/* 制御モード */
	BOOL	CmdRstReq;	/* コマンドリセット要求 */
	BOOL	MtEenable;	/* モーション有効 */
	BOOL	ServoOn;	/* サーボON指令 */
	BOOL	SensOn;		/* センサON指令 */
	BOOL	SvonLock;	/* サーボON Lock */
	BOOL	CmdRateOn;	/* 加減速指令有効SW<Rev2.01> */
} MT_CMD;



typedef struct MT_HNDL_ {
	LONG		seqState;	/* 指令生成シーケンス状態 */
	LONG		otState;	/* OT状態 */
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
	UCHAR		amState;	/* AM状態 */
#endif
	LONG		mtState;	/* モーション状態 */
	LONG		crtMtype;	/* 現在モーションタイプ */
	LONG		ctrlMode;	/* 現在制御モード */
	ULONG		timer;		/* RampStop用タイマ */
	LONG		segCnt;		/* モーション払い出し回数カウンタ */
	MT_CMD		cmdData;	/* モーション指令データ */
	MG_INOUT	outData;	/* モーション出力データ */
	MT_LATCH	latch;		/* ラッチ */
	MG_HNDL		mtgHdl;		/* MotionGeneratorハンドル */
	PSN_HNDL	psnHdl;		/* Positionハンドル */
	ACF_HNDL	acfHdl;		/* AccFilterハンドル */
	MT_PRM		mtPrm;		/* MotionPRMハンドル */
	SV_CYC_CMD*	svCmd;		/* Servo If Cyclic Command Data*/
	SV_CYC_RSP*	svRsp;		/* Servo If Cyclic Response Data*/
} MT_HNDL;


/****************************************************************************
 API's
****************************************************************************/
/* Motion Componentの初期化処理 */
VOID MtcInit( MT_HNDL *hdl, SV_OPTION_IF *svop_if );
/* モーション出力処理 */
VOID MtcOutputData( MT_HNDL *hdl );
/* モーションデータ更新処理 */
VOID MtcInputData( MT_HNDL *hdl );
/* ラッチ要求発行 */
INT32 MtcReqLatch(MT_HNDL* hdl, LONG mode, LONG sgnl);
/* ラッチ要求キャンセル */
VOID MtcCancelLatch(MT_HNDL* hdl, LONG mode);


#define		MOTION_CTRL_H_
#endif /* MOTION_CTRL_H_ */
