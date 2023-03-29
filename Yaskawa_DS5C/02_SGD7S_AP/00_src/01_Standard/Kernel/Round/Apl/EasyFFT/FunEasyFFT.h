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
	FNEASYFFTST_INIT 	= 0L,		/* ��������						*/
	FNEASYFFTST_READY	= 1L,		/* �^�]����						*/
	FNEASYFFTST_DRIVE	= 2L,		/* �^�]���쒆	 				*/
	FNEASYFFTST_RESULT	= 3L,		/* �^�]����(�t�B���^�ݒ�҂�)	*/
	FNEASYFFTST_END		= 4L,		/* �I������ 					*/
	FNEASYFFTST_TRQSET	= 5L,		/* �|���g���N�ݒ�				*/
	FNEASYFFTST_WAIT	= 6L,
};

/*--------------------------------------------------------------------------------------------------*/
typedef struct _FnEasyFFTV {
	struct{
		enum _enumFnEasyFFTState State;			/* Fn206 Easy FFT Execution State			*/
		LONG OpeModeIdx;						/* Operation Mode Register Index 			*/
		LONG OpeSetIdx;							/* Operation Set Register Index 			*/
		ULONG ResData;							/* Response Data */

		LONG OpeEzFftStateIdx;					/* 0x2061 : EasyFFT�����m�F	Index			*/
		LONG OpeEzFftTrqIdx;					/* 0x2062 : EasyFFT�g���N�U��	Index 		*/
		LONG OpeEzFftDetfIdx;					/* 0x2410 : EasyFFT���o���g�� Index			*/
		LONG OpeEzFftFil1Idx;					/* 0x2411 : EasyFFT�t�B���^���g���P Index	*/
		LONG OpeEzFftFil2Idx;					/* 0x2412 : EasyFFT�t�B���^���g���Q	Index 	*/
		LONG PrmTFuncSwIdx;						/* Pn408 : �g���N�֌W�@�\�X�C�b�`	Index	*/
		LONG PrmEzFftTrqIdx;					/* Pn456 : �|���g���N�w�ߐU��		Index	*/

		USHORT	EzFftStateVal;					/* 0x2061 : EasyFFT�����m�F			*/
		USHORT EzFftTrq;						/* 0x2062 : EasyFFT�g���N�U��				*/
		UCHAR DPos;								/* �_�Ō���									*/
		UCHAR Dummy[3];							/* For Alignment */
		ULONG DispTime;							/* Display Time (Panel Operator) */

		LONG Reserved[(256/4)-3-11]; 			/* Reserved: All functions can use ram variables */
												/* at least 256 bytes. */
	}var;
} FNEASYFFTV;





/***************************************** end of file **********************************************/
#endif	/*	FUN_EASYFFT_H_	*/
