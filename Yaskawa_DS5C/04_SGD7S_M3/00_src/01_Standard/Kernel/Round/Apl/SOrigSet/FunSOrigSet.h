/*****************************************************************************
Description	: Fn020 Scale Origin Setting
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
#ifndef FUN_SORIGSET_H_
#define FUN_SORIGSET_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/* Fn020 Scale Origin Setting State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnSOrigSetState
{
	FNSORIGSETST_ORGSET1	= 0L,
	FNSORIGSETST_ORGSET2	= 1L,
	FNSORIGSETST_ORGSET3	= 2L,
	FNSORIGSETST_ORGSET4	= 3L,
	FNSORIGSETST_ORGSET5	= 4L,
	FNSORIGSETST_WAIT		= 5L,
	FNSORIGSETST_END		= 6L
};

typedef struct _FnSOrigSetV {
	struct{
		enum _enumFnSOrigSetState State;	/* Fn020 Scale Origin Setting (Fn020) Execution State 	*/
		LONG OpeModeIdx;					/* Operation Mode Register Index 						*/
		LONG OpeSetIdx;						/* Operation Set Register Index 						*/
		LONG OpeReadIdx;					/* Operation Read Register Index						*/
		ULONG ResData;						/* Response Data										*/
		LONG OperatorStsIdx;						/* Operation Display Status Register Index */
		SHORT OperatorStsVal;
		SHORT Dummy;
		LONG Reserved[(256/4)-3-4]; 		/* Reserved: All functions can use ram variables 		*/
											/* at least 256 bytes. */
	}var;
} FNSORIGSETV;



/***************************************** end of file **********************************************/
#endif	/*	FUN_SORIGSET_H_	*/
