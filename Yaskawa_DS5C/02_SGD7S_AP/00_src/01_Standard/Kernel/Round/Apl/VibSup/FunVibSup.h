/*****************************************************************************
Description		: Model Following Control with Vibration Suppression Tuning
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.07.01		created

*****************************************************************************/

#ifndef FUN_VIBSUP_H_
#define FUN_VIBSUP_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include	"Monitor.h"		/* LCD operator monitor define head file */
#include    "Parameter.h"	/* Parameter Definition Header File */
#include	"ModelControl.h"/* �_�C���N�g�g���N���t�@�����X�������f���Ǐ]����֘A������` */


enum _enumFnVibSupState
{
	VIBSUP_INIT			= 0L,			/* �������� */
	VIBSUP_ADJUST		= 1L,			/* �U�����g�������� */
	VIBSUP_SETREQ		= 2L,			/* ���g��(�U���}���t�����f���Ǐ]����)�ݒ� */
	VIBSUP_WAITMFC		= 3L,			/* ���g���ݒ�L���҂�(���f���Ǐ]����͊��ɗL��) */
	VIBSUP_WAITNORM		= 4L,			/* �U���}���t�����f���Ǐ]����ݒ�L���҂� */
	VIBSUP_SETCOMP		= 5L,			/* �ݒ芮�� */
	VIBSUP_END			= 6L			/* �I������ */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FNVIBSUPV {
	struct{
		enum _enumFnVibSupState State;			/* VibSup (Fn205) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		LONG PrmMdlantfrqIdx;					/* Prm read Register Index 0x145 */
		ULONG ResMdlantfrqData;					/* Respone 0x145: �U���}���P���g��A Date */
		LONG PrmMdlresfrqIdx;					/* Prm read Register Index 0x146 */
		ULONG ResMdlresfrqData;					/* Respone 0x146: �U���}���P���g��B Date */
		LONG OpeMdlVibFreqIdx;					/* Operation read Register Index 0x0B0 */
		ULONG ResMdlVibFreqData;				/* Respone 0x0B0: �U���}���t��MFC�������g�� Date */
		ULONG DetFreq;							/* ���o���g�� */
		ULONG DPos;								/* �_�Ō��� */
		BOOL Match;								/* �\���Ɠ��삪�����Ă��邩 */
		ULONG FfWaitCntr;						/* �҂����Ԍo�߃J�E���^ */
		LONG UnRvibFrequencyIdx;				/* Un read Register Index 0x107 */
		ULONG ResRvibFrequencyData;				/* Respone 0x107: �c���U�����g�� Date */
		LONG Reserved[(256/4)-3-12];			/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FNVIBSUPV;

/***************************************** end of file **********************************************/
#endif /* FUN_VIBSUP_H_ */
