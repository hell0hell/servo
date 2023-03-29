/****************************************************************************
Description	: Definition of MECHATROLINK Response packet
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef RSP_HANDLER_H_


/* RSP ハンドラ */
typedef struct RSP_HNDL_ {
	LONG		enableLtMon;		/* ラッチモニタ強制変換要求		*/
	BOOL		rspSetDone;			/* コマンド応答完了フラグ		*/
	BOOL		exRspSetDone;		/* サブコマンド応答完了フラグ	*/
	BOOL		last_L_CMP;			/* 前回のラッチ完了状態			*/
} RSP_HNDL;



#define RSP_HANDLER_H_
#endif /* RSP_HANDLER_H_ */



