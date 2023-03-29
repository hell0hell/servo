/****************************************************************************
Description	: MECHATROLINK Position Filter
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef ACC_FIL_H_

#include "MtPRM.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define REAL_BUF_SZ	(512000 * 4) / (SCAN_N_DEFCYCLE * CFG_TIO_BASE_TIME)
#if (REAL_BUF_SZ > ACF_MAX_SZ)
	#define ACF_BUF_SZ	ACF_MAX_SZ
#else
	#define ACF_BUF_SZ	REAL_BUF_SZ
#endif

#define ACF_SW_NOFIL		0
#define ACF_SW_EXPFIL		1
#define ACF_SW_MAFIL		2



/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
typedef struct ACF_MAFIL_ {
	LONG		maSum;				/* �ړ����σt�B���^�T�� */
	LONG		maRem;				/* �ړ����σt�B���^�]�� */
	LONG		*maBuf;				/* �ړ����σt�B���^�o�b�t�@ */
	USHORT		maTb;				/* �ړ����σt�B���^���萔 */
	USHORT		maBufPtr;			/* �ړ����σt�B���^�o�b�t�@�|�C���^ */
	USHORT		maBufCnt;			/* �ړ����σt�B���^�o�͎��� */
	USHORT		dummy;				/* 4byte Alignment */
} ACF_MAFIL;


typedef struct ACF_IMAFIL_ {
	LONG		inPosSum;			/* ���͈ړ��ʐώZ */
	LONG		dPos;				/* �ړ��ʑ����l */
	LONG		lstPos;				/* �O��w�ߓ��͈ړ��� */
	LONG		dPosSum;			/* ��ԋ����ώZ */
	LONG		iRem;				/* �ړ����σt�B���^�]�� */
	UCHAR		iTbSft;				/* �ړ����σt�B���^���萔 */
	UCHAR		iTb;				/* �ړ����σt�B���^���萔 */
	UCHAR		iCnt;				/* ��ԉ񐔃J�E���^ */
	UCHAR		dummy;				/* 4byte Alignment */
} ACF_IMAFIL;


typedef struct ACF_HNDL_ {
	UCHAR		filSw;				/* ���݃t�B���^�^�C�v */
	UCHAR		dummy1[3];			/* 4byte Alignment */
	BOOL		filComp;			/* �t�B���^�����o������ */
	USHORT		expRem;				/* �w���֐��t�B���^�o�b�t�@�]�� */
	USHORT		dummy2;				/* 4byte Alignment */
	LONG		expBuf;				/* �w���֐��t�B���^�o�b�t�@ */
	LONG		expBias;			/* �w���֐��t�B���^�o�C�A�X���x */
	LONG		expTb;				/* �w���֐��t�B���^���萔 */
	LONG		maBuf[ACF_BUF_SZ];	/* �ړ����σt�B���^�o�b�t�@ */
	ULONG		maFilTb;			/* �ړ����σt�B���^���萔 */
	ACF_MAFIL	maFil;				/* �ړ����σt�B���^ */
	ACF_IMAFIL	maIFil;				/* �ړ����ϕ�ԃt�B���^ */
	ACF_PRM		*Prm;				/* �p�����[�^ */
	MC_BASE_PRM *bPrm;				/* ��{�p�����[�^ */
} ACF_HNDL;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* ACF_HNDL�̏��������� */
VOID AcfInit( ACF_HNDL *hdl, MC_BASE_PRM *bPrm, ACF_PRM *prm );
/* �t�B���^�������� */
VOID AcfGenerate( ACF_HNDL *hdl, LONG *spd, LONG sel, BOOL *den );
/* �t�B���^�N���A���� */
VOID AcfClear( ACF_HNDL *hdl );



#define ACC_FIL_H_
#endif /* ACC_FIL_H_ */



