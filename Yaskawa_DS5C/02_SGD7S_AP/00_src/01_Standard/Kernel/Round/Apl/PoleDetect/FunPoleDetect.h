/*****************************************************************************
Description		: Magnetic Pole Adjust
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.25		created

*****************************************************************************/

#ifndef FUN_POLEDETECT_H_
#define FUN_POLEDETECT_H_

#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */
#include    "Parameter.h"	/* Parameter Definition Header File */
#include	"Monitor.h"		/* LCD operator monitor define head file */
#include	"PnPrmListTbl.h"


enum _enumFunPoleDetectState
{
	POLEADJ_INIT 		= 0L,		/* ���������� */
	POLEADJ_GAINCHANGE	= 1L,		/* ���Ɍ��o���x���[�v�Q�C���ύX�EEEPROM�����ݏ��� */
	POLEADJ_EXE1		= 2L,		/* ���Ɍ��o�J�n���� */
	POLEADJ_EXE2		= 3L,		/* ���Ɍ��o���s���� */
	POLEADJ_EXEAFTER	= 4L,		/* �d���ʑ���񏑍��ݗv���쐬�E�����݌��ʊĎ����� */
	POLEADJ_FNERR		= 5L,		/* �⏕�@�\�ُ�I�������F�I������҂����� */
	POLEADJ_END			= 6L		/* �⏕�@�\�I������ */
};

/******************************************************************************/
/*		STRUCT DEFINITION													  */
/******************************************************************************/
typedef struct _FUNPOLEDETECTV {
	struct{
		enum _enumFunPoleDetectState State;		/* PoleDetect (Fn080) Execution State */
		LONG OpeModeIdx;						/* Operation Mode Register Index */
		LONG OpeSetIdx;							/* Operation Set Register Index */
		ULONG DispTime;							/* Display Time (Panel Operator) */
		ULONG DPos;								/* �_�Ō��� */
		LONG OpePdetStateIdx;					/* Operation read Register Index 0x032 */
		ULONG ResPdetStateData;					/* Respone 0x032: ���Ɍ��o���� Date */
		LONG PrmPdetloophzIdx;					/* Prm read Register Index 0x481 */
		ULONG ResPdetloophzData;				/* Respone 0x481: ���Ɍ��o���x���[�v�Q�C��  Date */
		ULONG PdetKvVal;						/* ���Ɍ��o���x���[�v�Q�C��(Pn481)�F[0.1Hz] */
		BOOL PdetReq;							/* ���Ɍ��o�v��(�ʓd�v��) */
		LONG UnReadIdx_AlmCode;					/* Operation Set Register Index 0x50A */
		ULONG ResData_AlmCode;					/* Respone 0x50A Date */
		LONG Reserved[(256/4)-3-7]; 			/* Reserved: All functions can use ram variables at least 256 bytes. */
	}var;
} FUNPOLEDETECTV;

/***************************************** end of file **********************************************/
#endif /* FUN_POLEDETECT_H_ */
