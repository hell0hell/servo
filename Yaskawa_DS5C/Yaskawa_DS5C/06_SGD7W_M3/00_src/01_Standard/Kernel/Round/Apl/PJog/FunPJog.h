/*****************************************************************************
Description		: Program JOG module
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.07		created

*****************************************************************************/
#ifndef FUN_PROGRAMJOG_H_
#define FUN_PROGRAMJOG_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/*		 Program JOG State																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnPJogState
{
	FNPROGRAMJOGST_INIT		= 0L,
	FNPROGRAMJOGST_SVOFF	= 1L,
	FNPROGRAMJOGST_SVON		= 2L,
	FNPROGRAMJOGST_RUN		= 3L,
	FNPROGRAMJOGST_STOP		= 4L,
	FNPROGRAMJOGST_END		= 5L
};

/**
 * Fn004 : Program JOG Variable
 */
typedef struct _FnProgramJogV {
	struct
	{
		enum _enumFnPJogState State;	/* PrgJog (Fn004) Execution State */
		LONG OpeModeIdx;				/* Operation Mode Register Index */
		LONG OpeSetIdx;					/* Operation Set Register Index */
		LONG OpeReadIdx;				/* Operation Read Register Index */
		ULONG ResData;					/* Response Data */
		LONG PrmPJogSwIdx;				/* プログラムJOG 運転関係スイッチ */
		BOOL RevDir;					/* プログラムJOG運転モード開始方向	*/

		ULONG	LongTime;				/* 補助機能実行処理時間計測用	*/
		ULONG DispTime;					/* Display Time (Panel Operator) */
		LONG	SafeStopCmd;			/* <S198> */

		//LONG Reserved[(256/4)-3-6]; 	/* Reserved: All functions can use ram variables */
		LONG Reserved[(256/4)-3-6-1]; 	/* Reserved: All functions can use ram variables *//* <S198> */
										/* at least 256 bytes. */
	}var;
} FNPROGRAMJOGV;

/***************************************** end of file **********************************************/
#endif	/*	FUN_PROGRAMJOG_H_	*/
