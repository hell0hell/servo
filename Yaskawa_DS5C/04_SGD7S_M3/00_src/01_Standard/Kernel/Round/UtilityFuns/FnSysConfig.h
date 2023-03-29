/*****************************************************************************
  Description   : Fn005, Fn006, Fn014, Fn030, Parameter Recalc.
------------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : MERCURY

  Create Date   : 2014-01-08

*****************************************************************************/

enum OptCardDetClearMode
{
	OPTCLR_CARD_NOT,				/* Mode Nothing							*/
	OPTCLR_CARD_COMMAND,			/* COMMAND Mode							*/
	OPTCLR_CARD_SAFE,				/* SAFE Mode							*/
	OPTCLR_CARD_FB					/* FB Mode								*/
};

/****************************************************************************/
/*																			*/
/*		Struct Definition													*/
/*																			*/
/****************************************************************************/
/*--------------------------------------------------------------------------*/
/*		オプションカード検出クリア用 定数・変数データ定義					*/
/*--------------------------------------------------------------------------*/
typedef struct OPTCLR{
	struct {
		USHORT	OptCardNo;			/* 0x20D0:オプションカード選択			*/
	} OpeReg;
	struct {
		USHORT	CmdOptId;			/* 指令オプションID						*/
		USHORT	SftOptId;			/* セーフティオプションID				*/
		USHORT	FbkOptId;			/* フィードバックオプションID			*/
	} var;
}OPTCLR;


/******************************* End of File ********************************/
