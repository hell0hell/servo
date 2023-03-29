/*****************************************************************************
Description		: Fn008 Multi Turn Data Reset & Encoder Alarm Reset
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.17		created

*****************************************************************************/

#ifndef FUN_MTURNCLR_H_
#define FUN_MTURNCLR_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Monitor.h"		/* LCD operator monitor define head file */

/*--------------------------------------------------------------------------------------------------*/
/*		 Multi Turn Data Reset & Encoder Alarm Reset State											*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnMturnClrState
{
	FNMTURNCLRST_INIT		= 0L,
	FNMTURNCLRST_IDLE		= 1L,
	FNMTURNCLRST_MULTITURN	= 2L,
	FNMTURNCLRST_END 		= 3L
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNMTURNCLRV {
	struct{
		enum _enumFnMturnClrState State;	/* MturnClr (Fn008) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG ResData;						/* Respone Date */
		LONG OperatorStsIdx;				/* Operation Display Status Register Index */
		SHORT OperatorStsVal;				/* Operation Display Status */
		ULONG Cnt;							/* blink */
		LONG Reserved[(256/4)-3-4]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNMTURNCLRV;

/***************************************** end of file **********************************************/
#endif /* FUN_MTURNCLR_H_ */
