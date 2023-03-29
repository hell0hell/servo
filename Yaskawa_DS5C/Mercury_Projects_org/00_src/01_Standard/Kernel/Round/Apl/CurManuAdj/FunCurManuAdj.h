/*****************************************************************************
Description		: Fn00F Current Offset Manual Adjustment
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.17		created
T.Masuo		2013.12.27		ïsãÔçáèCê≥No. A257 ëŒâû	<S0A9>
*****************************************************************************/

#ifndef FUN_CURMANUADJ_H_
#define FUN_CURMANUADJ_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Monitor.h"		/* LCD operator monitor define head file */
#include	"PnPrmListTbl.h"/* Register and Parameter Access Interface */	/* <S0A9> */

/*--------------------------------------------------------------------------------------------------*/
/*		 Current Offset Manual Adjustment State														*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnCurManuAdjState
{
	FNCURMANUADJST_U 	= 0L,
	FNCURMANUADJST_V 	= 1L,
	FNCURMANUADJST_ADJU = 2L,
	FNCURMANUADJST_ADJV = 3L,
	FNCURMANUADJST_END  = 4L
};
/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNCURMANUADJV {
	struct{
		enum _enumFnCurManuAdjState State;	/* CurManuAdj (Fn00F) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
/* <S0A9> : Delete Start */
#if 0
		LONG UnReadIdxU;					/* Un read Register Index U Electric */
		LONG UnReadIdxV;					/* Un read Register Index V Electric */
		LONG ResZadjiuData;					/* Respone U Date */
		LONG ResZadjivData;					/* Respone V Date */
		LONG Reserved[(256/4)-3-4]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */
#endif
/* <S0A9> : Delete End */
		LONG DispZadjiuData;				/* Display Data (Ch1) */	/* <S0A9> Add */
		LONG DispZadjivData;				/* Display Data (Ch2) */	/* <S0A9> Add */
		LONG Reserved[(256/4)-3-4-2];		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0A9> Add */
	}var;
} FNCURMANUADJV;

/***************************************** end of file **********************************************/
#endif /* FUN_CURMANUADJ_H_ */
