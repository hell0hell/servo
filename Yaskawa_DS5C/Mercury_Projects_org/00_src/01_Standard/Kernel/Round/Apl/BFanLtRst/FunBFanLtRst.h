/*****************************************************************************
Description		: Built-in FAN Lifetime Reset (Fn110)
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2014.03.05		created

*****************************************************************************/
#ifndef FUN_BFANLTRST_H_
#define FUN_BFANLTRST_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/* Built-in FAN Lifetime Reset State																*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnBFanLtRstState
{
	FNBFANRSTST_EXE = 0L,
	FNBFANRSTST_END = 1L
};

/**
 * Fn110 Built-in FAN Lifetime Reset Variable
 */
typedef struct _FNBFanLtRstV {
	struct
	{
		enum _enumFnBFanLtRstState State;	/* Built-in FAN Lifetime Reset (Fn110) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG ResData;						/* Response Data */
		LONG Reserved[(256/4)-3-1]; 		/* Reserved: All functions can use ram variables */
											/* at least 256 bytes.	*/
	}var;
} FNBFANLTRSTV;


/***************************************** end of file **********************************************/
#endif /*	FUN_BFANLTRST_H_	*/
