/*****************************************************************************
Description		: Fn01E : Servo & Motor ID Display	Mode
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.10.10		created

*****************************************************************************/

#ifndef FUN_SVMOTOP_ID_H_
#define FUN_SVMOTOP_ID_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Information.h" /* Servo Information Header File */
#include	"Parameter.h"	/* Parameter Definition Header File */

/*--------------------------------------------------------------------------------------------------*/
/*		 Servo & Motor ID Display  State																			*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnSvMotOpIDState
{
	IDDISP_SEQ_INIT = 0L,
	IDDISP_SEQ_DRV = 1L,
	IDDISP_SEQ_MOT = 2L,
	IDDISP_SEQ_ENC = 3L,
	IDDISP_SEQ_CMDOP = 4L,
	IDDISP_SEQ_ASCOP = 5L,
	IDDISP_SEQ_FBOP = 6L,
	IDDISP_SEQ_WAIT = 7L,
	IDDISP_SEQ_END = 8L,
};
/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FnSvMotOpIDV {
	struct{
		enum _enumFnSvMotOpIDState State;	/* Fn01E Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		ULONG DPos;						/* 点滅桁数						*/
		ULONG LongTimer;				/* 補助機能実行処理時間計測用						*/
#if 0/* <S0D8> */
		LONG InfoReadIdx_id800;				/* 0x0800: 制御モジュール形式文字列 */
		ULONG ResData_id800;				/* Respone 0x0800 Date */
		LONG InfoReadIdx_id80C;				/* 0x080C: 制御モジュールシリアルナンバー文字列 */
		ULONG ResData_id80C;				/* Respone 0x080C Date */
		LONG InfoReadIdx_id814;				/* 0x0814: 制御モジュール製造年月 */
		ULONG ResData_id814;				/* Respone 0x0814 Date */
#endif/* <S0D8> */
		LONG InfoReadIdx_id800;				/* 0x0800: 制御モジュール形式文字列 */
		ULONG ResData_id800;				/* Respone 0x0800 Date */
		LONG InfoReadIdx_id808;				/* 0x0808: 制御モジュールシリアルナンバー文字列 */
		ULONG ResData_id808;				/* Respone 0x080C Date */
		LONG InfoReadIdx_id810;				/* 0x0810: 制御モジュール製造年月 */
		ULONG ResData_id810;				/* Respone 0x0814 Date */
		LONG PrmReadIdx_motw;				/* Prm read Register Index 0xF02 */
		ULONG ResData_motw;					/* Respone Prm 0xF02 Date */

		LONG Reserved[(256/4)-3-2-6-2];		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNSVMOTOPIDV;

/***************************************** end of file **********************************************/
#endif /* FUN_SVMOTOP_ID_H_ */
