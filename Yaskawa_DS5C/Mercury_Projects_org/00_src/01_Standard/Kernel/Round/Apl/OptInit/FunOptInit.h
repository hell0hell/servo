/*****************************************************************************
Description		: Fn014 Option Card Detection Clear Mode
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

#ifndef FUN_OPT_INIT_H_
#define FUN_OPT_INIT_H_

#include	"KnlApi.h"				/* Kernel API 		 */
#include	"LcdOp.h"				/* LCD Operator		 */
#include 	"Function.h"			/* Fn Function		 */
#include	"Operation.h"			/* Servo Operation	 */

enum _enumFnOptInitState
{
	OPTCLR_SEQ_INIT = 0L,			/* OptCardDetClear Initial					*/
	OPTCLR_SEQ_SEL_OP = 1L,			/* OptCardDetClear Select Operation Mode	*/
	OPTCLR_SEQ_REQWAIT = 2L,		/* OptCardDetClear Wait Response			*/
	OPTCLR_SEQ_CLEAR = 3L,			/* OptCardDetClear Clear Operation			*/
	OPTCLR_SEQ_NOPE = 4L,			/* OptCardDetClear No Operation				*//* <S0BE> */
	OPTCLR_SEQ_WAIT = 5L,			/* OptCardDetClear Sequence Wait			*//* <S0BE>:4L->5L */
	OPTCLR_SEQ_END = 6L				/* OptCardDetClear End						*//* <S0BE>:5L->6L */
};

enum OptCardDetClearDisplayState
{
	OPTCLR_BIT_NONE_DISPLAY = 0L,			/* Display Nothing					*/
	OPTCLR_BIT_COMMAND_DISPLAY = 1L,		/* Only Display COMMAND Mode		*/
	OPTCLR_BIT_SAFE_DISPLAY = 2L,			/* Only Display SAFE Mode			*/
	OPTCLR_BIT_COMMAND_SAFE_DISPLAY = 3L,	/* Display COMMAND And SAFE Mode	*/
	OPTCLR_BIT_FB_DISPLAY = 4L,				/* Only Display FB Mode				*/
	OPTCLR_BIT_COMMAND_FB_DISPLAY = 5L,		/* Display COMMAND And FB Mode		*/
	OPTCLR_BIT_SAFE_FB_DISPLAY = 6L,		/* Display SAFE And FB Mode			*/
	OPTCLR_BIT_ALL_DISPLAY = 7L				/* Display All Mode					*/
};

enum OptCardDetClearConnectState
{
	OPTCLR_CARD_NONE_CONNECT,				/* Connect Nothing					*/
	OPTCLR_CARD_COMMAND_CONNECT_ONLY,		/* Only Connect COMMAD Mode			*/
	OPTCLR_CARD_SAFE_CONNECT_ONLY,			/* Only Connect SAFE Mode			*/
	OPTCLR_CARD_COMMAND_SAFE_CONNECT,		/* Connect COMMAND And SAFE Mode	*/
	OPTCLR_CARD_FB_CONNECT_ONLY,			/* Only Connect FB Mode				*/
	OPTCLR_CARD_COMMAND_FB_CONNECT,			/* Connect COMMAND And FB Mode		*/
	OPTCLR_CARD_SAFE_FB_CONNECT,			/* Connect SAFE And FB Mode			*/
	OPTCLR_CARD_ALL_CONNECT					/* Connect All Mode					*/
};
#if 0/* <S0BE> */
enum OptCardDetClearMode
{
	OPTCLR_CARD_NOT,						/* Mode Nothing						*/
	OPTCLR_CARD_COMMAND,					/* COMMAND Mode						*/
	OPTCLR_CARD_SAFE,						/* SAFE Mode						*/
	OPTCLR_CARD_FB							/* FB Mode							*/
};
#endif

/********************************************************************************/
/*		STRUCT DEFINITION														*/
/********************************************************************************/
typedef struct _FnOptCardDetClearV {
	struct {
		enum _enumFnOptInitState State;	/* Option Card Detection Clear (Fn014) Execution State		*/
		LONG	OpeModeIdx;				/* Operation Mode Register Index							*/
		LONG	OpeSetIdx;				/* Operation Set Register Index								*/
		ULONG	ResData;				/* Response Data											*/
		ULONG	LongTimer;				/* 処理時間計測用											*/
		SHORT	SelNo;					/* クリア対象オプションカード選択							*/
		SHORT	SelMaxNo;				/* オプションカード選択数									*/
		SHORT	CardNoMin;				/* オプションカード最小番号									*/
		SHORT	CardNo;					/* クリア対象オプションカード番号							*/
		SHORT	Connected;				/* オプションカード接続状態									*/
		SHORT	CardNoMax;				/* オプションカード最大番号									*/

		LONG	OptCardNoIdx;			/* 0x20D0 : オプションカード選択			Index			*/
		LONG	OptCardInfoIdx;			/* 0xE51A : オプションカード検出情報 	Index		*/

		UCHAR	LPos;					/* Line position											*/
		UCHAR	Dummy1;					/* Dummy													*/
		UCHAR	Dummy2;					/* Dummy													*/
		UCHAR	Dummy3;					/* Dummy													*/
		LONG	Reserved[(256/4)-3-8];	/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNOPTCARDDETCLEARV;

/***************************************** end of file **********************************************/
#endif /* FUN_OPT_INIT_H_ */
