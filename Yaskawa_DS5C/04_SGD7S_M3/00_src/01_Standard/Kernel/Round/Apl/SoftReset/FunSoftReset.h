/*****************************************************************************
Description		: Fn030 Soft Reset Mode
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

#ifndef FUN_SOFT_RESET_H_
#define FUN_SOFT_RESET_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */\

/*--------------------------------------------------------------------------------------------------*/
/*		 Soft Reset  State																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnSoftResetState
{
	FNSOFTRESET_INIT = 0L,
	FNSOFTRESET_EXEC = 1L,
	FNSOFTRESET_END = 2L
};
/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FnSoftResetV {
	struct{
		enum _enumFnSoftResetState State;	/* Soft Reset Mode (Fn030) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG SelNo;						/* 	Select Number			*/
		LONG Reserved[(256/4)-3-1]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNSOFTRESETV;

/***************************************** end of file **********************************************/
#endif /* FUN_SOFT_RESET_H_ */
