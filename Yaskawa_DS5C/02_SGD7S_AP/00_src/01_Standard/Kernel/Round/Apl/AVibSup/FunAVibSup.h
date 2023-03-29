/*****************************************************************************
Description		: A-type Vibration Suppression Control Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.26		created

*****************************************************************************/

#ifndef FUN_AVIBSUP_H_
#define FUN_AVIBSUP_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include    "Parameter.h"	/* Parameter Definition Header File */
#include	"PnPrmListTbl.h"
#include	"ResVibCtrl.h"


enum _enumFnAVibSupState
{
	AVIBSUP_INIT		= 0L,		/* 初期処理 */
	AVIBSUP_MODESEL		= 1L,		/* 自動/手動モード選択 */
	AVIBSUP_TRGWAIT		= 2l,		/* 振動検出トリガ待ち */
	AVIBSUP_MEASURE		= 3L,		/* 周波数解析中 */
	AVIBSUP_COMP		= 4L,		/* 周波数解析完了(フィルタ設定待ち) */
	AVIBSUP_SET			= 5L,		/* フィルタ設定中 */
	AVIBSUP_FREQTUNE	= 6L,		/* 周波数調整 */
	AVIBSUP_GAINTUNE	= 7L,		/* ダンピングゲイン調整 */
	AVIBSUP_END			= 8L		/* 終了処理 */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNAVIBSUPV {
	struct{
		enum _enumFnAVibSupState State;			/* AVibSup (Fn204) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		ULONG LongTimer;						/* 補助機能実行処理時間計測用 */
		LONG PrmAvibdampIdx;					/* Prm read Register Index 0x163 */
		ULONG ResAvibdampData;					/* Respone 0x163: A型制振ダンピングゲイン Date */
		LONG OpeAvibModeIdx;					/* Operation read Register Index 0x0A0 */
		ULONG ResAvibModeData;					/* Respone 0x0A0: A型制振機能調整ﾓｰﾄﾞ Date */
		LONG OpeAvibFreqIdx;					/* Operation read Register Index 0x0A2 */
		ULONG ResAvibFreqData;					/* Respone 0x0A2: A型制振機能調整周波数 Date */
		LONG PrmAvibfrqIdx;						/* Prm read Register Index 0x161 */
		ULONG ResAvibfrqData;					/* Respone 0x161: A型制振周波数 Date */
		LONG OpeAvibGainIdx;					/* Operation read Register Index 0x0A3 */
		ULONG ResAvibGainData;					/* Respone 0x0A3: A型制振機能調整ゲイン Date */
		ULONG AresVibControl;					/* A型制振制御設定 */
		BOOL Match;								/* 表示と動作があっているか */
		ULONG DPos;								/* 点滅桁数 */
		LONG OpeAvibStateIdx;					/* Operation read Register Index 0x0A1 */
		ULONG ResAvibStateData;					/* Respone 0x0A1: A型制振機能調整完了 Date */
		LONG PrmVibnormIdx;						/* Prm read Register Index 0x312 */
		ULONG ResVibnormData;					/* Respone 0x312: 振動検出レベル Date */
		LONG OpeVibmRf0FreqIdx;					/* Operation read Register Index 0x400 */
		ULONG ResVibmRf0FreqData;				/* Respone 0x400: 振動モニタ共振周波数1 Data*/
		LONG Reserved[(256/4)-3-20]; 			/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNAVIBSUPV;

/***************************************** end of file **********************************************/
#endif /* FUN_AVIBSUP_H_ */
