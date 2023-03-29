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
	AVIBSUP_INIT		= 0L,		/* �������� */
	AVIBSUP_MODESEL		= 1L,		/* ����/�蓮���[�h�I�� */
	AVIBSUP_TRGWAIT		= 2l,		/* �U�����o�g���K�҂� */
	AVIBSUP_MEASURE		= 3L,		/* ���g����͒� */
	AVIBSUP_COMP		= 4L,		/* ���g����͊���(�t�B���^�ݒ�҂�) */
	AVIBSUP_SET			= 5L,		/* �t�B���^�ݒ蒆 */
	AVIBSUP_FREQTUNE	= 6L,		/* ���g������ */
	AVIBSUP_GAINTUNE	= 7L,		/* �_���s���O�Q�C������ */
	AVIBSUP_END			= 8L		/* �I������ */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNAVIBSUPV {
	struct{
		enum _enumFnAVibSupState State;			/* AVibSup (Fn204) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		ULONG LongTimer;						/* �⏕�@�\���s�������Ԍv���p */
		LONG PrmAvibdampIdx;					/* Prm read Register Index 0x163 */
		ULONG ResAvibdampData;					/* Respone 0x163: A�^���U�_���s���O�Q�C�� Date */
		LONG OpeAvibModeIdx;					/* Operation read Register Index 0x0A0 */
		ULONG ResAvibModeData;					/* Respone 0x0A0: A�^���U�@�\����Ӱ�� Date */
		LONG OpeAvibFreqIdx;					/* Operation read Register Index 0x0A2 */
		ULONG ResAvibFreqData;					/* Respone 0x0A2: A�^���U�@�\�������g�� Date */
		LONG PrmAvibfrqIdx;						/* Prm read Register Index 0x161 */
		ULONG ResAvibfrqData;					/* Respone 0x161: A�^���U���g�� Date */
		LONG OpeAvibGainIdx;					/* Operation read Register Index 0x0A3 */
		ULONG ResAvibGainData;					/* Respone 0x0A3: A�^���U�@�\�����Q�C�� Date */
		ULONG AresVibControl;					/* A�^���U����ݒ� */
		BOOL Match;								/* �\���Ɠ��삪�����Ă��邩 */
		ULONG DPos;								/* �_�Ō��� */
		LONG OpeAvibStateIdx;					/* Operation read Register Index 0x0A1 */
		ULONG ResAvibStateData;					/* Respone 0x0A1: A�^���U�@�\�������� Date */
		LONG PrmVibnormIdx;						/* Prm read Register Index 0x312 */
		ULONG ResVibnormData;					/* Respone 0x312: �U�����o���x�� Date */
		LONG OpeVibmRf0FreqIdx;					/* Operation read Register Index 0x400 */
		ULONG ResVibmRf0FreqData;				/* Respone 0x400: �U�����j�^���U���g��1 Data*/
		LONG Reserved[(256/4)-3-20]; 			/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNAVIBSUPV;

/***************************************** end of file **********************************************/
#endif /* FUN_AVIBSUP_H_ */
