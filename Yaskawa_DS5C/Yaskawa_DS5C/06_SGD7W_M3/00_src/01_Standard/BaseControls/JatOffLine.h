#ifndef __JAT_OFFLINE_H 
#define __JAT_OFFLINE_H
/****************************************************************************
Description	: Jrate OffLine Function
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.kira  2011.03.01    created

*****************************************************************************/
#include "Basedef.h"
#include "ResVibCtrl.h"
#include "CtrlLoopHandler.h"

/****************************************************************************************************/
/*																									*/
/*	�萔��`										            									*/
/*																									*/
/****************************************************************************************************/
//#define	C10POW7			10000000					/* 10^7											*/
//#define	C10POW9			1000000000					/* 10^9											*/
//#define	C2PAIE7			62831853					/* (2*PAI) * 10^7								*/
#define	JSHBITNUM		7							/* �ώZ�g���N�V�t�g��							*/

/*--------------------------------------------------------------------------------------------------*/
/*	����V�[�P���X�萔��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	JATINITIAL		0x00						/* �C�i�[�V�����薢���s							*/
#define	JATCWSTART		0x01						/* ���]���t�]�C�i�[�V��������s					*/
#define	JATCCWSTART		0x02						/* �t�]�����]�C�i�[�V��������s					*/
#define	JATSUMEND		0x04						/* �g���N�ώZ���s����							*/
#define	JATEXEEND		0x08						/* �C�i�[�V��������s����						*/

/*--------------------------------------------------------------------------------------------------*/
/*	����X�e�[�^�X�萔��`																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	JATINITIAL		0x00						/* �C�i�[�V�����薢���s							*/
#define	JATCWCOMP		0x01						/* ���]���t�]�C�i�[�V��������s����				*/
#define	JATCCWCOMP		0x02						/* �t�]�����]�C�i�[�V��������s����				*/
#define	JATCOMP			(JATCWCOMP|JATCCWCOMP)		/* �C�i�[�V��������s����						*/
#define	JATJRATECOMP	0x04						/* �C�i�[�V������l�X�V����						*/
#define	JATEXEERR		0x40						/* ���s���G���[									*/
#define	JATERR			0x80						/* �G���[�X�e�[�^�X								*/

/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef struct _PRMBK {
	ULONG	kp;							/* �j���FPn102												*/
	ULONG	jrate;						/* �C�i�[�V����FPn103										*/
	ULONG	vffgn;						/* ���xFF�Q�C���FPn109										*/
} PRMBK;

typedef struct _JATP {
	LONG	mkv;						/* ���f���j��												*/
	LONG	mkvi;						/* ���f���j��i												*/
	LONG	mklpf;						/* ���f���s��												*/
	LONG	mkj;						/* ���f���C�i�[�V���Q�C��									*/
	LONG	mjrate;						/* ���f���C�i�[�V��											*/
	LONG	jfil;						/* ����l�X�V���[�p�X�t�B���^�Q�C��							*/
	LONG	jverrfil;					/* ����G���[���o�p���x�΍����[�p�X�t�B���^�Q�C��			*/
} JATP;

typedef struct _JATV {
	UCHAR	enable;						/* �C�i�[�V��������s���t���O	0:�����s  1:���s��			*/
	UCHAR	status;						/* �C�i�[�V������X�e�[�^�X									*/
	UCHAR	jarte_start;				/* �C�i�[�V������V�[�P���X									*/
	USHORT	jrate;						/* �C�i�[�V������l					[%]						*/
	LONG	mnfb;						/* ���f���t�B�[�h�o�b�N���x			[2^24/OvrSpd]			*/
	LONG	mnfbwrk[2];					/* 															*/
	LONG	mvref;						/* ���f�����x�w��					[2^24/OvrSpd]			*/
//	LONG	mner;						/* ���f�����x�΍�											*/
	LONG	mnerp;						/* ���f�����x�΍�(���)										*/
	LONG	mneri;						/* ���f�����x�΍�(�ϕ�)										*/
	LONG	mtrf;						/* ���f���g���N						[2^24/MaxTrq]			*/
	LONG	mneribf[2];					/*															*/
	LONG	jverr;						/* ����G���[�p���x�΍�				[2^24/OvrSpd]			*/
	LONG	sum1;						/* ���]���t�]���g���N�w�ߐώZ�l								*/
	LONG	msum1;						/* ���]���t�]���f���g���N�w�ߐώZ�l							*/
	LONG	sum2;						/* �t�]�����]���g���N�w�ߐώZ�l								*/
	LONG	msum2;						/* �t�]�����]���f���g���N�w�ߐώZ�l							*/
	LONG	j0;							/* ���]���t�]���C�i�[�V������l								*/
	LONG	j1;							/* ���C�i�[�V������l										*/
	LONG	j2;							/* �t�]�����]���C�i�[�V������l								*/
	LONG	TargetDist;					/* �g���N�ώZ�J�n�����i����ړ������~1/2�j		[�w�ߒP��]	*/
	LONG	fbsum;						/* �Ű�����蒆FB�ʒu(�Ű���J�n�ʒu����̐�Βl)	[�w�ߒP��]	*/
	LONG	fbsumrem;					/* 															*/
	BOOL	JatPrmSetReq;				/*															*/
} JATV;
/*--------------------------------------------------------------------------------------------------*/
typedef struct _JATHNDL{
	JATP	jatp;							/* �C�i�[�V������֘A�p�����[�^							*/
	JATP	jatp_wk;						/* �C�i�[�V������֘A�p�����[�^(�p�����[�^�ێ��p)		*/
	JATV	jatv;							/* �C�i�[�V������֘A�ϐ�								*/
	PRMBK	prmbk;							/* �֘A�p�����[�^�o�b�N�A�b�v�l							*/
	USHORT	EstimatJrat;					/* ���׃C�i�[�V���䓯��l			[%]					*/
}JATHNDL;

/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void BpxJatOfflineCalc( JATHNDL	*jathdl, EGEAR *Egear, CTRL_CMD_PRM *CtrlCmdPrm, 
														CTRL_CMD_MNG *CtrlCmdMngr, LONG	TrqRefo_a );
void CpxJatOfflineCalc( JATHNDL *JatHdl, LONG UnMotSpdx, DETVIB	*DetVib, 
														BOOL PConActFlag, BOOL PerrClrReq );

#endif //__JAT_OFFLINE_H 
/***************************************** end of file **********************************************/
