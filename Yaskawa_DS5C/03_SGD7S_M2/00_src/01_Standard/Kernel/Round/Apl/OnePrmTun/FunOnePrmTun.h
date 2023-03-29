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
#ifndef FUN_ONEPRMTUN_H_
#define FUN_ONEPRMTUN_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"PnPrmListTbl.h"

/*--------------------------------------------------------------------------------------------------*/
/* Fn200 Dynamic Auto Tuning State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnOnePrmTunState
{
	FNONEPRMTUNST_INIT 		= 0L,			/* 初期処理						*/
	FNONEPRMTUNST_JSET		= 1L,			/* 慣性モーメント比設定処理		*/
	FNONEPRMTUNST_MODESEL	= 2L,			/* モード設定処理				*/
	FNONEPRMTUNST_FILSEL	= 3L,			/* フィルタタイプ設定処理	 	*/
	FNONEPRMTUNST_PREDISP	= 4L,			/* 調整前ゲイン表示処理	 		*/
	FNONEPRMTUNST_TUNING	= 5L,			/* 1パラ調整処理				*/
	FNONEPRMTUNST_POSTDISP	= 6L,			/* 調整後ゲイン表示処理			*/
	FNONEPRMTUNST_END		= 7L			/* 終了処理						*/
};



/*--------------------------------------------------------------------------------------------------*/
typedef struct _FnOnePrmTunV {
	struct{
		enum _enumFnOnePrmTunState	State;	/* Fn200 Dynamic Auto Tuning Execution State	*/
		LONG	OpeModeIdx;					/* Operation Mode Register Index 				*/
		LONG	OpeSetIdx;					/* Operation Set Register Index 				*/
		ULONG	ResData;					/* Response Data 								*/

		ULONG	AdatFilType;				/* フィルタタイプ（2095H）設定 */
		ULONG	SpatMode;					/* 0x2050 : One Parameter Tuning Mode			*/
		ULONG	SpatLevel;					/* 0x2051 : One Parameter Tuning Level 			*/
		ULONG	SpatFFLevel;				/* 0x2052 : One Parameter Tuning FF Level 		*/
		LONG	UnActGain;					/* Un014  : 有効ゲイン*/
		USHORT	FfSetting;					/* FFチューニング設定			*/
		USHORT	Dummy1;						/* For Alignment */
		ULONG	Jrate;						/* Pn103 : 慣性モーメント比		*/

		LONG	OpeAdatFilTypeIdx;			/* Filter Type Setting Operation Index 			*/
		LONG	OpeSpatModeIdx;				/* 0x2050 : One Parameter Tuning Mode Operation Index 	*/
		LONG	OpeSpatLevelIdx;			/* 0x2051 : One Parameter Tuning Level Operation Index 	*/
		LONG	OpeSpatFFLevelIdx;			/* 0x2052 : One Parameter Tuning FF Level  Operation Index */
		LONG	OpeSpatLvlChgStIdx;			/* 0x2053 : One Parameter Tuning Level Changing Completed	*/
		LONG	OpeSpatANotchStIdx;			/* 0x2054 : One Parameter Tuning Auto Notch Completed	*/
		LONG	OpeSpatAresStIdx;			/* 0x2055 : One Parameter Tuning Auto 'A' Completed		*/
		LONG	PrmLoopHzIdx;				/* Pn100 : 速度ループゲイン	 Index			*/
		LONG	PrmPiTimeIdx;				/* Pn101 : 速度ループ積分時定数 Index		*/
		LONG	PrmPosGnIdx;				/* Pn102 : 位置ループゲイン Index			*/
		LONG	PrmJratIdx;					/* Pn103 : 慣性モーメント比 Index			*/
		LONG	PrmMdlGnIdx;				/* Pn141 : モデル追従制御ゲイン	Index		*/
		LONG	PrmAvibSwIdx;				/* Pn160 : 制振制御関連スイッチ	 Index		*/
		LONG	PrmTFuncSwIdx;				/* Pn408 : トルク関係機能スイッチ Index		*/
		LONG	UnActGainIdx;				/* Un014  : 有効ゲイン  Index 				*/

		UCHAR	DigMax;						/* チューニングレベル移動最大桁			*/
		UCHAR	ModeMin;					/* チューニングモード設定最小値			*/
		UCHAR	ModeMax;					/* チューニングモード設定最大値			*/
		UCHAR	DPos;						/* 点滅桁数				*/

		SHORT	FftStat;					/* 周波数解析ステータス	 		*/
		USHORT	Dummy2;						/* For Alignment */

		BOOL	TunSel;						/* FB/FFチューニング選択 		*/

		LONG	Reserved[(256/4)-3-26]; 	/* Reserved: All functions can use ram variables */
											/* at least 256 bytes. */
	}var;
} FNONEPRMTUNV;





/***************************************** end of file **********************************************/
#endif	/*	FUN_ONEPRMTUN_H_	*/
