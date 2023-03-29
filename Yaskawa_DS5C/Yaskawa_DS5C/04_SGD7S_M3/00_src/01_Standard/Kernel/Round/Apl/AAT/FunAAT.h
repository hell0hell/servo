/*****************************************************************************
Description		: Fn201  Advanced Auto Tuning Mode Headfile
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.17		created

*****************************************************************************/

#ifndef FUN_AAT_H_
#define FUN_AAT_H_


#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File*/
#include	"Monitor.h"		/* Un Monitor *///<S197>

/*--------------------------------------------------------------------------------------------------*/
/*		 Advanced Auto Tuning  State																*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnRefAATState
{
	AAT_SEQ_INIT = 0L,							/* 初期状態							*/
	AAT_SEQ_MODESET = 1L,						/* チューニングモード設定状態		*/
	AAT_SEQ_JRATSET = 2L,						/* イナーシャ設定状態				*/
	AAT_TUNNING_WAIT = 3L,						/* チューニング待ち完了状態			*/
	AAT_SEQ_ENDDISP = 4L,						/* 終了表示状態						*/
	AAT_SEQ_END = 5L,							/* 最終設定状態						*/
	AAT_SEQ_WAIT = 6L,							/* 終了待ち状態						*/
	AAT_SEQ_RETURN = 7L, 						/* 後処理状態						*/
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/

typedef struct _FnAATV {
	struct {
		enum _enumFnRefAATState State;	/* Advanced Auto Tuning (Fn201) Execution State */

		LONG OpeModeIdx;			/* Operation Mode Register Index 				*/
		LONG OpeSetIdx;				/* Operation Set Register Index 				*/
		LONG TimeCntrIdex;			/*安全停止時間[s] Register Index				*/
		LONG AdatModeIdx;			/* 0x2090 : アドバンストオートチューニング関連モード設定 Register Index	*/
		LONG AdatLevelIdx;			/* 0x2091 : アドバンストオートチューニングモード設定 Register Index	*/
		LONG AdatMovepIdx;			/* 0x2092 : ｱﾄﾞﾊﾞﾝｽﾄAT移動距離	Register Index	*/
		LONG AdatStateIdx;			/* 0x2094 : ｱﾄﾞﾊﾞﾝｽﾄAT完了確認	Register Index	*/
		LONG AdatFilTypeIdx;		/* 0x2095 : ｱﾄﾞﾊﾞﾝｽﾄATﾌｨﾙﾀﾀｲﾌﾟ	Register Index	*/

		BOOL AdatMode;				/* 0x2090 : ｱﾄﾞﾊﾞﾝｽﾄATﾓｰﾄﾞ						*/
		ULONG AdatLevel;			/* 0x2091 : ｱﾄﾞﾊﾞﾝｽﾄATレベル					*/
		LONG AdatMovep;				/* 0x2092 : ｱﾄﾞﾊﾞﾝｽﾄAT移動距離					*/
		LONG AdatState;				/* 0x2094 : ｱﾄﾞﾊﾞﾝｽﾄAT完了確認					*/
		ULONG AdatFilType;			/* 0x2095 : ｱﾄﾞﾊﾞﾝｽﾄATﾌｨﾙﾀﾀｲﾌﾟ					*/

		LONG LoophzIdex;			/* Pn100 : 速度ループゲイン		Register Index			*/
		LONG PitimeIdex;			/* Pn101 : 速度ループ積分時定数	Register Index			*/
		LONG PosgnIdex;				/* Pn102 : 位置ループゲイン		Register Index			*/
		LONG JrateIdex;				/* Pn103 : 慣性モーメント比		Register Index			*/
		LONG MdlgnIdex;				/* Pn141 : モデル追従制御ゲイン	Register Index			*/
//		LONG Syssw1Idex;			/* PnE0B : システムスイッチ１	Register Index			*///<S197>

		LONG IntModeStateIdex;		/* 0xE705 : 内部動作状態						*///<S197

		LONG ModeSetPtr;			/* モード設定：シーケンスポインタ				*/
		LONG DPos;					/* 点滅桁数										*/
		LONG JratSetPtr;			/* 慣性モーメント同定：シーケンスポインタ		*/
//		LONG JratErr;				/* 慣性モーメント同定：エラー状態				*//*TODO: Delete */
		ULONG JratTimeCntr;			/* 安全停止時間カウンタ							*/
//		BOOL ServonFlag;			/* Servo on flag								*//*TODO: Delete */
		ULONG SafeTime;				/* Safe time									*/
		ULONG DispGain;				/* Disp Gain									*/
		LONG AmpType;				/* Amp   type									*/

		BOOL	DrvErrDetect;
		LONG	SafeStopCmd;														  /* <S195> */

		LONG Reserved[(256/4)-1-8-5-6-8]; 	/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNAATV;

/***************************************** end of file **********************************************/
#endif /* FUN_AAT_H_ */
