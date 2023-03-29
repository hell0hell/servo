/*****************************************************************************
Description		: Fn012 : Software Version Display
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.10.12		created

*****************************************************************************/

#ifndef FUN_SOFTVER_H_
#define FUN_SOFTVER_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File */
#include	"Information.h" /* Servo Information Header File */


/*--------------------------------------------------------------------------------------------------*/
/*		 Software Version Display State																*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnSOFTVERState
{
	VERDISP_SEQ_DRV			= 0L,
	VERDISP_SEQ_YSPEC		= 1L,
	VERDISP_SEQ_ENC			= 2L,
	VERDISP_SEQ_OPTENC		= 3L,
	VERDISP_SEQ_MII			= 4L,
	VERDISP_SEQ_CMD			= 5L,
	VERDISP_SEQ_SAFETY		= 6L,
	VERDISP_SEQ_YSAFETY		= 7L,
	VERDISP_SEQ_FB			= 8L,
	VERDISP_SEQ_YFB			= 9L,
	VERDISP_SEQ_BOOT1		= 10L,
	VERDISP_SEQ_BOOT2		= 11L,
	VERDISP_SEQ_END			= 12L
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNSOFTVERHV {
	struct{
		enum _enumFnSOFTVERState State;		/* SoftVer (Fn012) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		LONG PrmReadIdx_encver;				/* Prm read Register Index 0xF01 */
		ULONG ResData_encver;				/* Respone 0xF01 Date */

//		LONG InfoReadIdx_id005;				/* 0x0005: 制御モジュールファームウェアバージョン */	/* <S0D1> Delete */
//		ULONG ResData_id005;				/* Respone 0x0005 Date */								/* <S0D1> Delete */
		LONG InfoReadIdx_id007;				/* 0x0007: 制御モジュールY仕様番号 */
		ULONG ResData_id007;				/* Respone 0x0007 Date */

//		LONG Reserved[(256/4)-3-2-4]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */	/* <S0D1> Delete */
		LONG Reserved[(256/4)-6]; 		/* Reserved: All functions can use ram variables at least 256 bytes. */		/* <S0D1> */
	}var;
} FNSOFTVERHV;

/***************************************** end of file **********************************************/
#endif /* FUN_SOFTVER_H_ */
