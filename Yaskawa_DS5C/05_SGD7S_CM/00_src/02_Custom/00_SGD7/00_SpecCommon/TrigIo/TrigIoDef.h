/****************************************************************************
Description	: Trigger I/O functions
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
H.N.   2012.03.05     created

****************************************************************************/
#ifndef	_TRIGIO_DEF_H
#define	_TRIGIO_DEF_H

#include "Basedef.h"

/* define */
#define TRIG_IO_MAX_TBL_NUMBER	8		/* �g���KIO�e�[�u���� */
#define TRIG_IO_LSTPOS_MAX		32		/* �O��FB�ʒu�ۑ��p�o�b�t�@�� */

/* struct */
typedef struct TRIGIO_INFO_T {
	LONG	trigPosition;	/* �g���K�ʒu[pulse] */
	ULONG	manualDelay;	/* �}�j���A���x������[us] */
	SHORT	onAdjTime;		/* �o�͒�������[us] */ 
	USHORT	onTime;			/* IO�o�͎���[us] */
	USHORT	trigConfig;		/* �g���KIO�e�[�u���A���ԍ��A�o�̓s���A�ʉߕ��� */
	USHORT	axisNo;			/* ���ԍ� */
	USHORT	pinNo;			/* �o�̓s���ԍ� */
	SHORT	passDir;		/* �ʉߕ��� [1:�������A-1�t����] */
	
	BOOL	trigIOEnable;	/* �g���KIO�L�� [0:�����A1:�L��] */
	BOOL	trigIOMode;		/* ���[�h [0:�ʏ탂�[�h�A1:�}�j���A���o��] */
	BOOL	posReady;		/* �|�W�V����������� TRUE:�����AFALSE:���� */
	
	LONG	lstPos[TRIG_IO_LSTPOS_MAX];		/* MotAbsPos�̑O��l */
	USHORT	mSpdCycle;		/* ���[�^���x�Z�o����[62.5us] */
	USHORT	fbPosBufPtr;	/* �O��FB�ʒu�ۑ��p�o�b�t�@ */
}TRIGIO_INFO;

#endif	//	_TRIGIO_DEF_H
