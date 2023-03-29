/****************************************************************************
Description	: Definition of MECHATROLINK Response packet
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef RSP_HANDLER_H_


/* RSP ハンドラ */
typedef struct RSP_HNDL_ {
	ULONG		cPrmMon[2];			/* 固定モニタ選択1/2 */
	LONG		enableLtMon[2];		/* ラッチモニタ強制変換要求 */
	BOOL		rspSetDone;			/* コマンド応答完了フラグ */
	BOOL		exRspSetDone;		/* サブコマンド応答完了フラグ */
	BOOL		last_L_CMP[2];		/* 前回のラッチ完了状態1/2 */
} RSP_HNDL;



#define RSP_HANDLER_H_
#endif /* RSP_HANDLER_H_ */



