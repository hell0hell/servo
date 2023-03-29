/*****************************************************************************
Description		: Fn00D : Analog Monitor Gain Manual Adjustment
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.10.12		created
T.Masuo		2013.12.27		ïsãÔçáèCê≥No. A196 ëŒâû	<S0A9>
*****************************************************************************/

#ifndef FUN_MONGAINADJ_H_
#define FUN_MONGAINADJ_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Monitor.h"		/* LCD operator monitor define head file */
#include	"Parameter.h"	/* Parameter Definition Header File */
#include	"PnPrmListTbl.h"/* Register and Parameter Access Interface */	/* <S0A9> Add */


/*--------------------------------------------------------------------------------------------------*/
/*		 Analog Monitor Offset Manual Adjustment State												*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnMONGAINADJState
{
	FNMONGAINADJST_CH1		= 0L,
	FNMONGAINADJST_CH2		= 1L,
	FNMONGAINADJST_CH1ADJ	= 2L,
	FNMONGAINADJST_CH2ADJ	= 3L,
	FNMONGAINADJST_END		= 4L
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNMONGAINADJHV {
	struct{
		enum _enumFnMONGAINADJState State;	/* MonGainAdj (Fn00D) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
/* <S0A9> : Delete Start */
#if 0
		LONG UnReadIdxCh1;					/* Un read Register Index 0x50F */
		LONG ResCh1Data;					/* Respone 0x50F Date */
		LONG UnReadIdxCh2;					/* Un read Register Index 0x510 */
		LONG ResCh2Data;					/* Respone 0x510 Date */
#endif
/* <S0A9> : Delete End */
		LONG PrmReadIdxPrm6;				/* Prm read Register Index Prm6 */
		ULONG ResPrm6Data;					/* Respone Prm6 Date */
		LONG PrmReadIdxPrm7;				/* Prm read Register Index Prm7 */
		ULONG ResPrm7Data;					/* Respone Prm7 Date */
		LONG DispAdiData1;					/* Display Data (ìdó¨åüèoÉ[Éçí≤(Uëä)) */	/* <S0A9> Add */
		LONG DispAdiData2;					/* Display Data (ìdó¨åüèoÉ[Éçí≤(Vëä)) */	/* <S0A9> Add */
		//LONG Reserved[(256/4)-3-8]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0A9> Delete */
		LONG Reserved[(256/4)-9]; 			/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0A9> Add */
	}var;
} FNMONGAINADJHV;

/***************************************** end of file **********************************************/
#endif /* FUN_MONGAINADJ_H_ */
