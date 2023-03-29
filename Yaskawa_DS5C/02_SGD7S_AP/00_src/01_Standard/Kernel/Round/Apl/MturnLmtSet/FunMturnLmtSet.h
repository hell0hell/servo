/*****************************************************************************
Description		: Multi Turn Limit Setting
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.07		created
K.Liu		2013.12.09		Modify
*****************************************************************************/
#ifndef FUN_MTURNLMTSET_H_
#define FUN_MTURNLMTSET_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */


/*--------------------------------------------------------------------------------------------------*/
/* Multi Turn Limit Setting State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnMturnLmtSetState
{
	FNMTURNLMTSETST_INIT		= 0L,
	FNMTURNLMTSETST_IDLE		= 1L,
	FNMTURNLMTSETST_EXECUTING	= 2L,
	FNMTURNLMTSETST_END			= 3L
};

/**
 * Fn003 : Multi Turn Limit Setting Variable
 */
typedef struct _FnMturnLmtSetV {
	struct
	{
		enum _enumFnMturnLmtSetState State;		/* Multi Turn Limit Setting (Fn003) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		LONG OperatorStsIdx;						/* Operation Display Status Register Index */
		SHORT OperatorStsVal;
		SHORT Dummy;
		ULONG ResData;							/* Response Data */
		LONG Reserved[(256/4)-3-3]; 			/* Reserved: All functions can use ram variables */
												/* at least 256 bytes. */
	}var;
} FNMTURNLMTSETV;



/***************************************** end of file **********************************************/
#endif /*	FUN_MTURNLMTSET_H_	*/
