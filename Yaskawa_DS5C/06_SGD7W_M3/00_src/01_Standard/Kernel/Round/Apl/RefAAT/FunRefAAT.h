/*****************************************************************************
Description		: Fn202  Reference Input Type Advanced Auto Tuning Mode Headfile
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

#ifndef FUN_REF_AAT_H_
#define FUN_REF_AAT_H_


#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Parameter.h"	/* Parameter Definition Header File*/

/*--------------------------------------------------------------------------------------------------*/
/*		 Advanced Auto Tuning  State																*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnRefAATState
{
	REF_AAT_SEQ_INIT = 0L,				/* 初期状態							*/
	REF_AAT_SEQ_MODESET = 1L,			/* チューニングモード設定状態		*/
	REF_AAT_SEQ_JRATSET = 2L,			/* イナーシャ設定状態				*/
	REF_AAT_TUNNING_WAIT = 3L,			/* チューニング待ち完了状態			*/
	REF_AAT_SEQ_ENDDISP = 4L,			/* 終了表示状態						*/	
	REF_AAT_SEQ_END = 5L,				/* 最終設定状態						*/
	REF_AAT_SEQ_WAIT = 6L,				/* 終了待ち状態						*/
	REF_AAT_SEQ_RETURN 			= 7L		/* 後処理状態									*/
};


/****************************************************************************************************/
/*		STRUCT DEFINITION													  */
/****************************************************************************************************/

typedef struct _FnREFAATV {
	struct {
		enum _enumFnRefAATState State;		/* Fn202  Reference Input Type Execution State */

		LONG OpeModeIdx;				/* Operation Mode Register Index */
		LONG OpeSetIdx;					/* Operation Set Register Index */
		LONG AdatModeIdx;					/* 0x2090: アドバンストＡＴモード Index			*/
		LONG AdatLevelIdx;				/* 0x2091 : ｱﾄﾞﾊﾞﾝｽﾄATレベル	Index					*/
		LONG AdatStateIdx;				/* 0x2094 : ｱﾄﾞﾊﾞﾝｽﾄAT完了確認	Index					*/
		LONG AdatFilTypeIdx;			/* 0x2095 : ｱﾄﾞﾊﾞﾝｽﾄATﾌｨﾙﾀﾀｲﾌﾟ	Index					*/
		ULONG AdatMode;						/* 0x2090: アドバンストＡＴモード				*/
		ULONG AdatLevel;			/* 0x2091 : ｱﾄﾞﾊﾞﾝｽﾄATレベル								*/
		ULONG AdatState;					/* 0x2094 : ｱﾄﾞﾊﾞﾝｽﾄAT完了確認					*/
		ULONG AdatFilType;			/* 0x2095 : ｱﾄﾞﾊﾞﾝｽﾄATﾌｨﾙﾀﾀｲﾌﾟ								*/

		LONG LoophzIdex;			/* Pn100 : 速度ループゲイン									*/
		LONG PitimeIdex;			/* Pn101 : 速度ループ積分時定数								*/
		LONG PosgnIdex;				/* Pn102 : 位置ループゲイン									*/
		LONG MdlgnIdex;				/* Pn141 : モデル追従制御ゲイン								*/

		LONG ModeSetPtr;			/* モード設定：シーケンスポインタ							*/
		LONG DPos;					/* 点滅桁数													*/
		LONG JratSetPtr;			/* 慣性モーメント同定：シーケンスポインタ					*/
		ULONG DispGain;				/* Disp Gain				*/

		LONG Reserved[(256/4)-1-10-4-4];	/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNREFAATV;

/***************************************** end of file **********************************************/
#endif /* FUN_REF_AAT_H_ */
