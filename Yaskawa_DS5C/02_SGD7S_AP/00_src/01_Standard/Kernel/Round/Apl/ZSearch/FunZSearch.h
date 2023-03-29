/*****************************************************************************
Description		: Fn003 Origin Point Search
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

#ifndef FUN_ZSEARCH_H_
#define FUN_ZSEARCH_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/*		 Origin Point Search State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnZSEARCHState
{
	FNZSEARCHST_INIT		= 0L,
	FNZSEARCHST_SVOFF		= 1L,
	FNZSEARCHST_SVON		= 2L,
	FNZSEARCHST_SEARCHEND 	= 3L,
	FNZSEARCHST_END			= 4L
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNZSEARCHV {
	struct{
		enum _enumFnZSEARCHState State;		/* ZSearch (Fn003) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		LONG OpeReadIdx;					/* Operation read Register Index */
		ULONG DispTime;						/* Display Time (Panel Operator) */
		ULONG ResData;						/* Respone Date */
		LONG Reserved[(256/4)-3-3]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNZSEARCHV;

/***************************************** end of file **********************************************/
#endif /* FUN_ZSEARCH_H_ */
