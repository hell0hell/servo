/*****************************************************************************
Description		: Fn011 : Motot Type & Y Specification Display
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

#ifndef FUN_MOTORINFO_H_
#define FUN_MOTORINFO_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File */

/*--------------------------------------------------------------------------------------------------*/
/*		 Motot Type & Y Specification Display State													*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnMOTORINFOState
{
	FNMOTORINFOST_INIT		= 0L,
	FNMOTORINFOST_EXE		= 1L,
	FNMOTORINFOST_END		= 2L
};

/*--------------------------------------------------------------------------------------------------*/
/*		 Panel Fn011 Display State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumOPosMode
{
	FNMOTORINFOST_TYPM_E_L		= 0L,		/* モータ機種, 入力電圧					*/
	FNMOTORINFOST_MOTW			= 1L,		/* モータ容量							*/
	FNMOTORINFOST_TYPM_E_H		= 2L,		/* エンコーダタイプ, エンコーダ分解能	*/
	FNMOTORINFOST_YOPT			= 3L		/* Y仕様								*/
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNMOTORINFOHV {
	struct{
		enum _enumFnMOTORINFOState State;	/* MotorInfo (Fn011) Execution State */
		LONG OpeModeIdx;					/* Operation Mode Register Index */
		LONG OpeSetIdx;						/* Operation Set Register Index */
		enum _enumOPosMode DPos;			/* 点滅桁数 */
		LONG PrmReadIdx_typm_e;				/* Prm read Register Index 0xF00 */
		ULONG ResData_typm_e;				/* Respone Prm 0xF00 Date */
		LONG PrmReadIdx_exp_motw_spd;		/* Prm read Register Index 0xF19 */
		ULONG ResData_exp_motw_spd;			/* Respone Prm 0xF19 Date */
		LONG PrmReadIdx_motw;				/* Prm read Register Index 0xF02 */
		ULONG ResData_motw;					/* Respone Prm 0xF02 Date */
		LONG PrmReadIdx_bit_dp;				/* Prm read Register Index 0xF03 */
		ULONG ResData_bit_dp;				/* Respone Prm 0xF03 Date */
		LONG PrmReadIdx_yopt;				/* Prm read Register Index 0xE13 */
		ULONG ResData_yopt;					/* Respone Prm 0xE13 Date */
		LONG Reserved[(256/4)-3-11];		/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNMOTORINFOHV;

/***************************************** end of file **********************************************/
#endif /* FUN_MOTORINFO_H_ */
