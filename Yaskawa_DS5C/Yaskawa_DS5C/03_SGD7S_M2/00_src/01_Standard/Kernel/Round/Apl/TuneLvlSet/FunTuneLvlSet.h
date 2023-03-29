/*****************************************************************************
Description		: Fn200 Dynamic Auto Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.20		created

*****************************************************************************/
#ifndef FUN_TUNELVLSET_H_
#define FUN_TUNELVLSET_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/*--------------------------------------------------------------------------------------------------*/
/* Fn200 Dynamic Auto Tuning State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnTuneLvlSetState
{
	FNTUNELVLSETST_INIT 		= 0L,		/* 初期処理						*/
	FNTUNELVLSETST_JLEVELSEL	= 1L,		/* 負荷レベル設定処理			*/
	FNTUNELVLSETST_TUNING		= 2L,		/* ゲインレベル調整処理			*/
	FNTUNELVLSETST_END			= 3L		/* 終了処理						*/
};


/*--------------------------------------------------------------------------------------------------*/
typedef struct _FnTuneLvlSetV {
	struct{
		enum _enumFnTuneLvlSetState State;	/* Fn200 Dynamic Auto Tuning Execution State 			*/
		LONG OpeModeIdx;					/* Operation Mode Register Index 						*/
		LONG OpeSetIdx;						/* Operation Set Register Index 						*/
		ULONG ResData;						/* Response Data 										*/

		LONG OpeDatGnLevelIdx;				/* DAT Gain Level Register Index 						*/
		LONG OpeDatJratLevelIdx;			/* DAT Jrat Level Register Index 						*/
		LONG OpeSpatANotchStIdx;			/* 0x2054 : One Parameter Tuning Auto Notch Completed	*/

		USHORT DatGnLevel;					/* DAT Gain Level 										*/
		USHORT DatJratLevel;				/* DAT Jrat Level 										*/

		LONG PrmTFuncSwIdx;					/* Pn408 : トルク関係機能スイッチ	Index				*/

		LONG Reserved[(256/4)-3-7]; 		/* Reserved: All functions can use ram variables 		*/
											/* at least 256 bytes. */
	}var;
} FNTUNELVLSETV;





/***************************************** end of file **********************************************/
#endif	/*	FUN_TUNELVLSET_H_	*/
