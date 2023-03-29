/****************************************************************************
Description	: MECHATROLINK Motion Control Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef MOTION_CTRL_H_

#include "MtGenerator.h"
#include "Position.h"
#include "AccFilter.h"
#include "MtPRM.h"
#include "ServoIf.h"
#include "PRMBank.h"/* <S003> */


/****************************************************************************
 Defines
****************************************************************************/
#define MOTOR_STP_SPD			0x10000		/* モータ停止速度 24bit/OS / 256 */

#define	MCI_LTSGNL_CNFLCT		(-1)		/* ラッチ信号干渉 @CHECK@ */
#define	MCI_LTNO_ERR			(-2)		/* ラッチ番号異常 @CHECK@ */

#define MTLT_NO_SEQ_REQ			0			/* ラッチシーケンスなし状態 */
#define MTLT_SEQ_REQ			1			/* ラッチシーケンス起動状態 */
#define MTLT_SEQ_DONE			2			/* ラッチシーケンス完了状態 */
#define MTLT_CMP_STS			0x11		/* ラッチ完了状態 */

enum MT_OTSTATE {
	OTSTAT_NORMAL=0,			/* 0:Disable状態 */
	OTSTAT_STOP,				/* 1:停止中状態 */
	OTSTAT_WAITNORMAL,			/* 2:OT解除待ち状態 */
	OTSTAT_RETRACTION,			/* 3:引き戻し動作状態 */
};

/****************************************************************************
 Structure of the Motion Handle
****************************************************************************/
typedef struct MT_LATCH_ {
	LONG	Req;				/* ラッチ要求 */
	LONG	Cmp;				/* ラッチ完了 */
	LONG	Mode;				/* ラッチモード */
	LONG	Sgnl;				/* 選択信号 */

	LONG	LtDisable;			/* ラッチ無効 */
	LONG	SeqReq;				/* 連続ラッチ要求 */
	LONG	SeqCnt;				/* 連続ラッチシーケンスカウンタ */
	union {						/* 連続ラッチステータス */
		USHORT	wdCnt;
		UCHAR	cnt[2];			/* [0]:連続回数カウント,[1]:シーケンスカウント */
	}SeqSts;
} MT_LATCH;

typedef struct MT_CMD_ {
	LONG	TPos;					/* 目標位置[ref] */
	LONG	TSpeed;					/* 目標速度[ref/s] */
	LONG	Velocity;				/* VFF[re/s], 速度指令[2^30/MAX] */
	LONG	Trq;					/* トルク指令[MAX_TRQ/2^30] */
	LONG	HoldMod;				/* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	LONG	AccFilSel;				/* フィルタ選択 0,3: なし, 1:指数,2:移動平均 */
	LONG	LatchSgnl;				/* ラッチ信号選択 0:C相,n:EXTn */
	LONG	Mtype;					/* モーションタイプ */	
	LONG	Mtype_Cancel;			/* モーションタイプ(コマンドキャンセル前) */	
	LONG	CtrlMode;				/* 制御モード */		
	BOOL	CmdRstReq;				/* コマンドリセット要求 */
	BOOL	MtEenable;				/* モーション有効 */
	BOOL	ServoOn;				/* サーボON指令 */
	BOOL	SensOn;					/* センサON指令 */
	BOOL	SvonLock;				/* サーボON Lock */
	BOOL	CmdRateOn;				/* 加減速指令有効SW<Rev2.01> */
	BOOL	MtLatchReq;				/* モーションラッチ要求 */
} MT_CMD;

/* MT Manager ハンドラ */
typedef struct MT_HNDL_ {
	BOOL		ltSignlUse[4];		/* ラッチ信号利用フラグ C相,EXT1,EXT2,EXT3 */
	LONG		seqState;			/* 指令生成シーケンス状態 */
	LONG		otState;			/* OT状態 */
	LONG		mtState;			/* モーション状態 */
	LONG		crtMtype;			/* 現在モーションタイプ */
	LONG		ctrlMode;			/* 現在制御モード */
	ULONG		timer;				/* RampStop用タイマ */
	LONG		segCnt;				/* モーション払い出し回数カウンタ */
	LONG		lstOmnMon[2];		/* 前回オプションモニタ値 */
	MT_CMD		cmdData;			/* モーション指令データ */
	MG_INOUT	outData;			/* モーション出力データ */
	MT_LATCH	latch[LT_NUM];		/* ラッチ */
	MG_HNDL		mtgHdl;				/* MotionGeneratorハンドル */
	PSN_HNDL	psnHdl;				/* Positionハンドル */
	ACF_HNDL	acfHdl;				/* AccFilterハンドル */
	MT_PRM		mtPrm;				/* MotionPRMハンドル */
	SV_CYC_CMD	*svCmd;				/* Servo If Cyclic Command Data*/
	SV_CYC_RSP	*svRsp;				/* Servo If Cyclic Response Data*/
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
INT32 MtcReqLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, LONG i );
/* ラッチ要求キャンセル */
VOID MtcCancelLatch( MT_HNDL *hdl, LONG mode, LONG i );
/* プロファイルタイプに依存するモーション情報の設定(Motion Control) */
INT32 MtcSetMtInfoByProfileType( MT_HNDL *hdl, LONG type );

extern	const	UCHAR	LtMonTbl[];



#define MOTION_CTRL_H_
#endif /* MOTION_CTRL_H_ */



