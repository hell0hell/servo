/*****************************************************************************
Description		: Magnetic Pole Adjust
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.25		created

*****************************************************************************/

#ifndef FUN_POLEDETECT_H_
#define FUN_POLEDETECT_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include    "Parameter.h"	/* Parameter Definition Header File */
#include	"Monitor.h"		/* LCD operator monitor define head file */
#include	"PnPrmListTbl.h"


enum _enumFunPoleDetectState
{
	POLEADJ_INIT 		= 0L,		/* 初期化処理 */
	POLEADJ_GAINCHANGE	= 1L,		/* 磁極検出速度ループゲイン変更・EEPROM書込み処理 */
	POLEADJ_EXE1		= 2L,		/* 磁極検出開始処理 */
	POLEADJ_EXE2		= 3L,		/* 磁極検出実行処理 */
	POLEADJ_EXEAFTER	= 4L,		/* 電流位相情報書込み要求作成・書込み結果監視処理 */
	POLEADJ_FNERR		= 5L,		/* 補助機能異常終了処理：終了操作待ち処理 */
	POLEADJ_END			= 6L		/* 補助機能終了処理 */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FUNPOLEDETECTV {
	struct{
		enum _enumFunPoleDetectState State;		/* PoleDetect (Fn080) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		ULONG DispTime;							/* Display Time (Panel Operator) */
		ULONG DPos;								/* 点滅桁数 */
		LONG OpePdetStateIdx;					/* Operation read Register Index 0x032 */
		ULONG ResPdetStateData;					/* Respone 0x032: 磁極検出完了 Date */
		LONG PrmPdetloophzIdx;					/* Prm read Register Index 0x481 */
		ULONG ResPdetloophzData;				/* Respone 0x481: 磁極検出速度ループゲイン  Date */
		ULONG PdetKvVal;						/* 磁極検出速度ループゲイン(Pn481)：[0.1Hz] */
		BOOL PdetReq;							/* 磁極検出要求(通電要求) */
		LONG UnReadIdx_AlmCode;					/* Operation Set Register Index 0x50A */
		ULONG ResData_AlmCode;					/* Respone 0x50A Date */
		LONG Reserved[(256/4)-3-7]; 			/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FUNPOLEDETECTV;

/***************************************** end of file **********************************************/
#endif /* FUN_POLEDETECT_H_ */
