/*****************************************************************************
Description		: Init. Threshhold Level for Vibration Detect
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.24		created

*****************************************************************************/

#ifndef FUN_VIBLVLINIT_H_
#define FUN_VIBLVLINIT_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File */


enum _enumFnVibLvlInitState
{
	VIBDET_SEQ_INIT		= 0L,		/* ���o�J�n�҂����� */
	VIBDET_SEQ_DETECT	= 1L,		/* �U�����x�����o�� */
	VIBDET_SEQ_STORE	= 2L,		/* �U�����x���ۑ� */
	VIBDET_SEQ_ERROR	= 3L,		/* �U�����o�G���[ */
	VIBDET_SEQ_END		= 4L		/* �I������ */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNVIBLVLINITV {
	struct{
		enum _enumFnVibLvlInitState State;		/* VibLvlInit (Fn01B) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		ULONG LongTimer;						/* �⏕�@�\���s�������Ԍv���p */
		LONG OperatorStsIdx;					/* Operation Display Status Register Index 0x000F*/
		SHORT OperatorStsVal;					/* Operation Display Status */
		LONG Reserved[(256/4)-3-3];				/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNVIBLVLINITV;

/***************************************** end of file **********************************************/
#endif /* FUN_VIBLVLINIT_H_ */
