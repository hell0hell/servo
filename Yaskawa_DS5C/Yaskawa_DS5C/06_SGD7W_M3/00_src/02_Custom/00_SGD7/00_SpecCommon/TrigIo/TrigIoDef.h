/****************************************************************************
Description	: Trigger I/O functions
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
H.N.   2012.03.05     created

****************************************************************************/
#ifndef	_TRIGIO_DEF_H
#define	_TRIGIO_DEF_H

#include "Basedef.h"

/* define */
#define TRIG_IO_MAX_TBL_NUMBER	8		/* トリガIOテーブル数 */
#define TRIG_IO_LSTPOS_MAX		32		/* 前回FB位置保存用バッファ数 */

/* struct */
typedef struct TRIGIO_INFO_T {
	LONG	trigPosition;	/* トリガ位置[pulse] */
	ULONG	manualDelay;	/* マニュアル遅延時間[us] */
	SHORT	onAdjTime;		/* 出力調整時間[us] */ 
	USHORT	onTime;			/* IO出力時間[us] */
	USHORT	trigConfig;		/* トリガIOテーブル、軸番号、出力ピン、通過方向 */
	USHORT	axisNo;			/* 軸番号 */
	USHORT	pinNo;			/* 出力ピン番号 */
	SHORT	passDir;		/* 通過方向 [1:正方向、-1逆方向] */
	
	BOOL	trigIOEnable;	/* トリガIO有効 [0:無効、1:有効] */
	BOOL	trigIOMode;		/* モード [0:通常モード、1:マニュアル出力] */
	BOOL	posReady;		/* ポジション準備状態 TRUE:完了、FALSE:未完 */
	
	LONG	lstPos[TRIG_IO_LSTPOS_MAX];		/* MotAbsPosの前回値 */
	USHORT	mSpdCycle;		/* モータ速度算出周期[62.5us] */
	USHORT	fbPosBufPtr;	/* 前回FB位置保存用バッファ */
}TRIGIO_INFO;

#endif	//	_TRIGIO_DEF_H
