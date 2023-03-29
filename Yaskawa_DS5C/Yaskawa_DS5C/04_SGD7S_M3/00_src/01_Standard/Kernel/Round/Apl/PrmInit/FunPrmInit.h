/*****************************************************************************
Description		: Fn005 ParamInit Mode Headfile
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.17		created

*****************************************************************************/

#ifndef FUN_PRM_INIT_H_
#define FUN_PRM_INIT_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/*		 ParamInit  State																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnPrmInitState
{
	FNPRMINIT_IDLE = 0L,
	FNPRMINIT_EXEC = 1L,
	FNPRMINIT_END  = 2L
};
/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FnPrmInitV {
	struct {
		enum _enumFnPrmInitState State;		/* ParamInit (Fn005) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG ResData;						/* Response Data */
		LONG Reserved[(256/4)-3-1]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNPRMINITV;

/***************************************** end of file **********************************************/
#endif /* FUN_PRM_INIT_H_ */
