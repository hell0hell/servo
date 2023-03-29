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
/*		�I�v�V�����J�[�h���o�N���A�p �萔�E�ϐ��f�[�^��`					*/
/*--------------------------------------------------------------------------*/
typedef struct OPTCLR{
	struct {
		USHORT	OptCardNo;			/* 0x20D0:�I�v�V�����J�[�h�I��			*/
	} OpeReg;
	struct {
		USHORT	CmdOptId;			/* �w�߃I�v�V����ID						*/
		USHORT	SftOptId;			/* �Z�[�t�e�B�I�v�V����ID				*/
		USHORT	FbkOptId;			/* �t�B�[�h�o�b�N�I�v�V����ID			*/
	} var;
}OPTCLR;


/******************************* End of File ********************************/
