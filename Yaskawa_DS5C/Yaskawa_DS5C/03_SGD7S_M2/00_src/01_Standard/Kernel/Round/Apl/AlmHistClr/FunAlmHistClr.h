/*****************************************************************************
Description		: Alarm Traceback Data Clear (Fn006)
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
#ifndef FUN_ALMHISTCLR_H_
#define FUN_ALMHISTCLR_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/* Alarm Traceback Data Clear State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnAlmHistClrState
{
	FNALMHISTCLRST_EXE = 0L,
	FNALMHISTCLRST_END = 1L
};

/**
 * Fn006 Alarm Traceback Data Clear Variable
 */
typedef struct _FNAlmHistClrV {
	struct
	{
		enum _enumFnAlmHistClrState State;	/* AlarmTraceClear (Fn006) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG ResData;						/* Response Data */
		LONG Reserved[(256/4)-3-1]; 		/* Reserved: All functions can use ram variables */
											/* at least 256 bytes.	*/
	}var;
} FNALMHISTCLRV;


/***************************************** end of file **********************************************/
#endif /*	FUN_ALMHISTCLR_H_	*/
