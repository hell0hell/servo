/*****************************************************************************
Description		: Fn207 V-Monitor Mode
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.17		created
K.Liu		2013.12.09		Modify
*****************************************************************************/

#ifndef FUN_VMONITOR_H_
#define FUN_VMONITOR_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/*		 ParamInit  State																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnVMonState
{
	VMON_SEQ_INIT = 0,
	VMON_SEQ_DETERR,
	VMON_SEQ_DETECT,
	VMON_SEQ_DISPLAY,
	VMON_SEQ_CALC,
	VMON_SEQ_STORE,
	VMON_SEQ_END
};
/******************************************************************************/
/*		V-Monitor DEFINITION													  */
/******************************************************************************/
typedef struct _FnVMonV {
	struct{
		enum _enumFnVMonState State;	/* V-Monitor (Fn207) Execution State 	*/
		LONG OpeModeIdx;				/* Operation Mode Register Index 		*/
		LONG OpeSetIdx;					/* Operation Set Register Index 		*/
		LONG VibmStateIdx;				/* V-monitor State Read Register Index 	*/
		LONG FreqIdx[3];				/*	Operation Read Register Freq Index	*/
		LONG VibmState;					/* V-monitor State 						*/
		LONG OperatorStsIdx;						/* Operation Display Status Register Index */
		SHORT OperatorStsVal;
		SHORT Dummy;
		ULONG Freq[3];					/* Freq									*/
		ULONG TimeOutCtr;				/* タイムアウト検出用カウンタ			*/
		LONG Reserved[(256/4)-6-8]; 	/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNVMONV;

/***************************************** end of file **********************************************/
#endif /* FUN_VMONITOR_H_ */
