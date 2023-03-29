/*****************************************************************************
Description		: Model Following Control with Vibration Suppression Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.07.01		created

*****************************************************************************/

#ifndef FUN_VIBSUP_H_
#define FUN_VIBSUP_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Monitor.h"		/* LCD operator monitor define head file */
#include    "Parameter.h"	/* Parameter Definition Header File */
#include	"ModelControl.h"/* ダイレクトトルクリファレンス方式モデル追従制御関連処理定義 */


enum _enumFnVibSupState
{
	VIBSUP_INIT			= 0L,			/* 初期処理 */
	VIBSUP_ADJUST		= 1L,			/* 振動周波数微調整 */
	VIBSUP_SETREQ		= 2L,			/* 周波数(振動抑制付きモデル追従制御)設定 */
	VIBSUP_WAITMFC		= 3L,			/* 周波数設定有効待ち(モデル追従制御は既に有効) */
	VIBSUP_WAITNORM		= 4L,			/* 振動抑制付きモデル追従制御設定有効待ち */
	VIBSUP_SETCOMP		= 5L,			/* 設定完了 */
	VIBSUP_END			= 6L			/* 終了処理 */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNVIBSUPV {
	struct{
		enum _enumFnVibSupState State;			/* VibSup (Fn205) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		LONG PrmMdlantfrqIdx;					/* Prm read Register Index 0x145 */
		ULONG ResMdlantfrqData;					/* Respone 0x145: 振動抑制１周波数A Date */
		LONG PrmMdlresfrqIdx;					/* Prm read Register Index 0x146 */
		ULONG ResMdlresfrqData;					/* Respone 0x146: 振動抑制１周波数B Date */
		LONG OpeMdlVibFreqIdx;					/* Operation read Register Index 0x0B0 */
		ULONG ResMdlVibFreqData;				/* Respone 0x0B0: 振動抑制付きMFC調整周波数 Date */
		ULONG DetFreq;							/* 検出周波数 */
		ULONG DPos;								/* 点滅桁数 */
		BOOL Match;								/* 表示と動作があっているか */
		ULONG FfWaitCntr;						/* 待ち時間経過カウンタ */
		LONG UnRvibFrequencyIdx;				/* Un read Register Index 0x107 */
		ULONG ResRvibFrequencyData;				/* Respone 0x107: 残留振動周波数 Date */
		LONG Reserved[(256/4)-3-12];			/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNVIBSUPV;

/***************************************** end of file **********************************************/
#endif /* FUN_VIBSUP_H_ */
