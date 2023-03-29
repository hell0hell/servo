/****************************************************************************
Description	: Motor Less Test Mode
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  : none

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.11.11		created <S00X>

****************************************************************************/
#ifndef MOTORLESSTESTSTR_H__
#define MOTORLESSTESTSTR_H__

/*==========================================================================*/
/* I N C L U D E															*/
/*==========================================================================*/
#include "Basedef.h"
#include "PnPrmStruct.h"
#include "EncData.h"

/*==========================================================================*/
/* D E F I N E																*/
/*==========================================================================*/

/*==========================================================================*/
/* S T R U C T																*/
/*==========================================================================*/
/*****************************************************************************
*	���[�^���X�e�X�g���[�h�p�ϐ���`
*****************************************************************************/
/* �������J���f���p�p�����[�^��`	*/
typedef	struct MOTLESSMDLP {
	LONG	MdlKj;					/* ���f���C�i�[�V���Q�C��									*/
	LONG	MdlKvp;					/* ���x[2^24/OvrSpd]���ʒu����[pulse/Scan](����)�ϊ��W��	*/
	LONG	MdlKvpM;				/* ���x[2^24/OvrSpd]���ʒu����[pulse/Scan](���[�^)�ϊ��W��	*/
} MOTLESSMDLP;

/* �������J���f���p�p�����[�^���ϐ���`	*/
typedef	struct MOTLESSMDL {
	MOTLESSMDLP	P[2];				/* ���[�^���X�e�X�g���[�h�p�p�����[�^				*/
	MOTLESSMDLP	*PrmPtr;			/* �L���ȃp�����[�^�ւ̃|�C���^						*/

	struct {						/* ���[�^���X�e�X�g���[�h�p�ϐ��f�[�^��`	*/
		LONG	MotMdlSpdFbk;			/* ���f�����x�e�a				[2^24/OvrSpd]	*/
		LONG	MotMdldPosFbk;			/* ���[�^���f���ʒu�e�a����		[pluse/scan]	*/
		LONG	MotMdlPosFbk;			/* ���[�^���f���ʒu�e�a			[pluse]			*/
		LONG	MechaMdldPosFbk;		/* ���f���ʒu�e�a����			[pluse/scan]	*/
		LONG	MechaMdlPosFbk;			/* ���f���ʒu�e�a				[pluse]			*/
		LONG	Mvar;					/* ���f�����x�ϕ����Z�p							*/
		LONG	MotMdlSfbrem;			/* ���f�����x�e�a���܂�							*/
		LONG	MotMdlpfbrem;			/* ���f���ʒu�e�a���܂�							*/
		LONG	MotMdlpfbremM;			/* ���f�����[�^�ʒu�e�a���܂�					*/
	} V;
} MOTLESSMDL;

/*****************************************************************************
*	���[�^���X�e�X�g���[�h�f�[�^��`
*****************************************************************************/
/* �G���R�[�_�f�[�^	*/
typedef	union ENCODER_DATA {
	LONG		Long[2];			/* �ʒu�f�[�^(32bit�~2=64bit)��`		*/
	struct {						/* �ʒu�f�[�^�G���R�[�_�t�H�[�}�b�g��`	*/
		DWORDX	RxPosL;					/* �G���R�[�_�t�H�[�}�b�g�f�[�^(Low)  32bit	*/
		SHORT	RxPosH;					/* �G���R�[�_�t�H�[�}�b�g�f�[�^(High) 16bit	*/
		SHORT	Dummy;					/* �_�~�[�f�[�^						  16bit	*/
	} EncFormat;
} ENCODER_DATA;

/* �G���R�[�_IF */
typedef	struct MOTOR_LESS {
	BOOL			SemiEncConnect;	/* Semi-Closed Encoder Connected		*/
	BOOL			FullEncConnect;	/* Full-Closed Encoder Connected		*/
	BOOL			FencType;		/* Full-Closed Encoder Type				*/
	LONG			dSemiModePos;	/* �Z�~�ʒu�����f�[�^					*/
	LONG			dFullModePos;	/* �t���ʒu�����f�[�^					*/
	ENCODER_DATA	SencData;		/* �Z�~�G���R�[�_�f�[�^					*/
	ENCODER_DATA	FencData;		/* �t���G���R�[�_�f�[�^					*/
} MOTOR_LESS;

/*****************************************************************************
*	���[�^���X�e�X�g���[�hIF��`
*****************************************************************************/
typedef struct MOTLESSTST {
	BOOL		MotorLessTestMode;	/* ���[�^���X�e�X�g���[�h�t���O			*/

	/* API�̈����Ŏw�肪����ȃf�[�^�ւ̃|�C���^(���������ɐݒ肷��)	*/
	MENCV		*MencV;				/* ���[�^�G���R�[�_�f�[�^�ւ̃|�C���^	*/
	MENCV		*FencV;				/* �t���N�G���R�[�_�f�[�^�ւ̃|�C���^	*/
	PRMDATA		*Prm;				/* �p�����[�^�f�[�^�ւ̃|�C���^			*/

	/* ���[�^���X�e�X�g���[�h�p���[�N	*/
	MOTLESSMDL	MotorLessMdl;		/* �������J���f���p�f�[�^				*/
	MOTOR_LESS	MotorLessTestDatas;	/* �G���R�[�_IF�p�f�[�^					*/
} MOTLESSTST;

/*==========================================================================*/
/* E X T E R N   V A R I A B L E											*/
/*==========================================================================*/

/*==========================================================================*/
/* P R O T O T Y P E														*/
/*==========================================================================*/

#endif // MOTORLESSTESTSTR_H__
/*-------end of file----------------------------------------------------------------------------------------*/
