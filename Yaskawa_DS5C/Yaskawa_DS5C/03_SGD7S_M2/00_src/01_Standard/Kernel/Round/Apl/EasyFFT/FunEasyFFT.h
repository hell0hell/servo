/*****************************************************************************
Description		: Fn206 Easy FFT Measuremen
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.27		created

*****************************************************************************/
#ifndef FUN_EASYFFT_H_
#define FUN_EASYFFT_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"PnPrmListTbl.h"

/*--------------------------------------------------------------------------------------------------*/
/* Fn206 Easy FFT  State																	*/
/*--------------------------------------------------------------------------------------------------*/
enum _enumFnEasyFFTState
{
	FNEASYFFTST_INIT 	= 0L,		/* 初期処理						*/
	FNEASYFFTST_READY	= 1L,		/* 運転準備						*/
	FNEASYFFTST_DRIVE	= 2L,		/* 運転動作中	 				*/
	FNEASYFFTST_RESULT	= 3L,		/* 運転完了(フィルタ設定待ち)	*/
	FNEASYFFTST_END		= 4L,		/* 終了処理 					*/
	FNEASYFFTST_TRQSET	= 5L,		/* 掃引トルク設定				*/
	FNEASYFFTST_WAIT	= 6L,
};

/*--------------------------------------------------------------------------------------------------*/
typedef struct _FnEasyFFTV {
	struct{
		enum _enumFnEasyFFTState State;			/* Fn206 Easy FFT Execution State			*/
		LONG OpeModeIdx;						/* Operation Mode Register Index 			*/
		LONG OpeSetIdx;							/* Operation Set Register Index 			*/
		ULONG ResData;							/* Response Data */

		LONG OpeEzFftStateIdx;					/* 0x2061 : EasyFFT完了確認	Index			*/
		LONG OpeEzFftTrqIdx;					/* 0x2062 : EasyFFTトルク振幅	Index 		*/
		LONG OpeEzFftDetfIdx;					/* 0x2410 : EasyFFT検出周波数 Index			*/
		LONG OpeEzFftFil1Idx;					/* 0x2411 : EasyFFTフィルタ周波数１ Index	*/
		LONG OpeEzFftFil2Idx;					/* 0x2412 : EasyFFTフィルタ周波数２	Index 	*/
		LONG PrmTFuncSwIdx;						/* Pn408 : トルク関係機能スイッチ	Index	*/
		LONG PrmEzFftTrqIdx;					/* Pn456 : 掃引トルク指令振幅		Index	*/

		USHORT	EzFftStateVal;					/* 0x2061 : EasyFFT完了確認			*/
		USHORT EzFftTrq;						/* 0x2062 : EasyFFTトルク振幅				*/
		UCHAR DPos;								/* 点滅桁数									*/
		UCHAR Dummy[3];							/* For Alignment */
		ULONG DispTime;							/* Display Time (Panel Operator) */

		LONG Reserved[(256/4)-3-11]; 			/* Reserved: All functions can use ram variables */
												/* at least 256 bytes. */
	}var;
} FNEASYFFTV;





/***************************************** end of file **********************************************/
#endif	/*	FUN_EASYFFT_H_	*/
