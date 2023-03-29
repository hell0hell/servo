/*****************************************************************************
Description		: Fn00E Motor Current Offset Auto Adjustment Headfile
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

#ifndef FUN_CUR_AUTO_ADJ_H_
#define FUN_CUR_AUTO_ADJ_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/*		 Motor Current Offset Auto Adjustment State													*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnCurAutoAdjState
{
	FNCURAUTOADJ_IDLE = 0L,
	FNCURAUTOADJ_EXEC = 1L,
	FNCURAUTOADJ_END  = 2L
};
/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FnCurAutoAdjV {
	struct {
		enum _enumFnCurAutoAdjState State;	/* Motor Current Offset Auto Adjustment (Fn00E) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG ResData;						/* Response Data */
		ULONG DispTime;
		LONG Reserved[(256/4)-3-2]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNCURAUTOADJV;

/***************************************** end of file **********************************************/
#endif /* FUN_CUR_AUTO_ADJ_H_ */
