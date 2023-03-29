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
	FNONEPRMTUNST_INIT 		= 0L,			/* ��������						*/
	FNONEPRMTUNST_JSET		= 1L,			/* �������[�����g��ݒ菈��		*/
	FNONEPRMTUNST_MODESEL	= 2L,			/* ���[�h�ݒ菈��				*/
	FNONEPRMTUNST_FILSEL	= 3L,			/* �t�B���^�^�C�v�ݒ菈��	 	*/
	FNONEPRMTUNST_PREDISP	= 4L,			/* �����O�Q�C���\������	 		*/
	FNONEPRMTUNST_TUNING	= 5L,			/* 1�p����������				*/
	FNONEPRMTUNST_POSTDISP	= 6L,			/* ������Q�C���\������			*/
	FNONEPRMTUNST_END		= 7L			/* �I������						*/
};



/*--------------------------------------------------------------------------------------------------*/
typedef struct _FnOnePrmTunV {
	struct{
		enum _enumFnOnePrmTunState	State;	/* Fn200 Dynamic Auto Tuning Execution State	*/
		LONG	OpeModeIdx;					/* Operation Mode Register Index 				*/
		LONG	OpeSetIdx;					/* Operation Set Register Index 				*/
		ULONG	ResData;					/* Response Data 								*/

		ULONG	AdatFilType;				/* �t�B���^�^�C�v�i2095H�j�ݒ� */
		ULONG	SpatMode;					/* 0x2050 : One Parameter Tuning Mode			*/
		ULONG	SpatLevel;					/* 0x2051 : One Parameter Tuning Level 			*/
		ULONG	SpatFFLevel;				/* 0x2052 : One Parameter Tuning FF Level 		*/
		LONG	UnActGain;					/* Un014  : �L���Q�C��*/
		USHORT	FfSetting;					/* FF�`���[�j���O�ݒ�			*/
		USHORT	Dummy1;						/* For Alignment */
		ULONG	Jrate;						/* Pn103 : �������[�����g��		*/

		LONG	OpeAdatFilTypeIdx;			/* Filter Type Setting Operation Index 			*/
		LONG	OpeSpatModeIdx;				/* 0x2050 : One Parameter Tuning Mode Operation Index 	*/
		LONG	OpeSpatLevelIdx;			/* 0x2051 : One Parameter Tuning Level Operation Index 	*/
		LONG	OpeSpatFFLevelIdx;			/* 0x2052 : One Parameter Tuning FF Level  Operation Index */
		LONG	OpeSpatLvlChgStIdx;			/* 0x2053 : One Parameter Tuning Level Changing Completed	*/
		LONG	OpeSpatANotchStIdx;			/* 0x2054 : One Parameter Tuning Auto Notch Completed	*/
		LONG	OpeSpatAresStIdx;			/* 0x2055 : One Parameter Tuning Auto 'A' Completed		*/
		LONG	PrmLoopHzIdx;				/* Pn100 : ���x���[�v�Q�C��	 Index			*/
		LONG	PrmPiTimeIdx;				/* Pn101 : ���x���[�v�ϕ����萔 Index		*/
		LONG	PrmPosGnIdx;				/* Pn102 : �ʒu���[�v�Q�C�� Index			*/
		LONG	PrmJratIdx;					/* Pn103 : �������[�����g�� Index			*/
		LONG	PrmMdlGnIdx;				/* Pn141 : ���f���Ǐ]����Q�C��	Index		*/
		LONG	PrmAvibSwIdx;				/* Pn160 : ���U����֘A�X�C�b�`	 Index		*/
		LONG	PrmTFuncSwIdx;				/* Pn408 : �g���N�֌W�@�\�X�C�b�` Index		*/
		LONG	UnActGainIdx;				/* Un014  : �L���Q�C��  Index 				*/

		UCHAR	DigMax;						/* �`���[�j���O���x���ړ��ő包			*/
		UCHAR	ModeMin;					/* �`���[�j���O���[�h�ݒ�ŏ��l			*/
		UCHAR	ModeMax;					/* �`���[�j���O���[�h�ݒ�ő�l			*/
		UCHAR	DPos;						/* �_�Ō���				*/

		SHORT	FftStat;					/* ���g����̓X�e�[�^�X	 		*/
		USHORT	Dummy2;						/* For Alignment */

		BOOL	TunSel;						/* FB/FF�`���[�j���O�I�� 		*/

		LONG	Reserved[(256/4)-3-26]; 	/* Reserved: All functions can use ram variables */
											/* at least 256 bytes. */
	}var;
} FNONEPRMTUNV;





/***************************************** end of file **********************************************/
#endif	/*	FUN_ONEPRMTUN_H_	*/
