#ifndef __FN_MFC_TUNE_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************
  Description   : Mfc with Vibration Suppression Tuning (Fn205)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.25   created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct MFCTUNE {
	struct {
		USHORT	MdlVibFreq;					/* 0x20B0 : �U���}���t��MFC�������g��					*/
		USHORT	MdlVibState;				/* 0x20B1 : �U���}���t��MFC��������						*/
		USHORT	MdlVibFreq2;				/* 0x20B2 : �U���}���@�\�`���[�j���O���g��2				*/
		USHORT	MdlVibFil2;					/* 0x20B3 : �U���}���@�\�`���[�j���O���g��2�␳			*/
		USHORT	MdlVibState2;				/* 0x20B4 : �U���}���@�\�ݒ�2�����m�F					*/
		USHORT	Dummy;						/* For Alignment										*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		BOOL	DispMatch;					/* �\���Ɠ��삪�����Ă��邩�H							*/
		ULONG	FfWaitCntr;					/* �҂����Ԍo�߃J�E���^									*/
		USHORT	DetFreq;					/* ���o���g��											*/
		USHORT	OrgMdlgn;					/* �ύX�O��Kpm											*/
		USHORT	OrgAntFrq;					/* �ύX�O��fa											*/
		USHORT	OrgResFrq;					/* �ύX�O��fr											*/
		USHORT	OrgMdlSw;					/* �ύX�O�̃X�C�b�`										*/
		USHORT	Dummy;						/* For Alignment										*/
	} var;
} MFCTUNE;



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum VIBMFC_SEQ_STS {
	VIBMFC_SEQ_INIT		= 0,				/* ��������												*/
	VIBMFC_SEQ_MEASURE,						/* �U�����o��											*/
	VIBMFC_SEQ_ADJUST,						/* �U�����g��������										*/
	VIBMFC_SEQ_SETREQ,						/* ���g��(�U���}���t�����f���Ǐ]����)�ݒ�				*/
	VIBMFC_SEQ_WAITMFC,						/* ���g���ݒ�L���҂�(���f���Ǐ]����͊��ɗL��)			*/
	VIBMFC_SEQ_WAITNORM,					/* �U���}���t�����f���Ǐ]����ݒ�L���҂�				*/
	VIBMFC_SEQ_SETCOMP,						/* �ݒ芮��												*/
	VIBMFC_SEQ_END,							/* �I������												*/
	VIBMFC_SEQ_NOOPWAIT,					/* No Operation �\���E�F�C�g							*/
};
/*--------------------------------------------------------------------------------------------------*/
enum VIBMFC_CMP_STS {
	VIBMFC_STS_COMP		= 0,				/* �����X�e�[�^�X�F�ݒ芮��/�C�j�V����					*/
	VIBMFC_STS_WAIT,						/* �����X�e�[�^�X�F�ݒ�҂�								*/
	VIBMFC_STS_TMOUT,						/* �����X�e�[�^�X�F�ݒ莸�s(�^�C���A�E�g)				*/
};
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_TUN_MAX			3		/* �Q�C�������ő包										*/
#define		VIBMFC_TUN_MIN			1		/* �Q�C��/���g�������ŏ���								*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_DET_FRQ_MIN		10		/* Fn023���̎c���U�����o���g������ 1.0Hz				*/
#define		VIBMFC_DET_FRQ_MAX		1000	/* Fn023���̎c���U�����o���g����� 100.0Hz				*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_ONE_MASS			0x01	/* ���̌n���f��											*/
#define		VIBMFC_TWO_MASS1		0x02	/* �Q�����n�n���f���P									*/
#define		VIBMFC_TWO_MASS2		0x03	/* �Q�����n�n���f���Q									*/
#define		VIBMFC_ONE_BASE			0x04	/* ���̌n�{�@�䃂�f��									*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_PRM_ACTIVE		0x0011	/* ���̌n�{�@�䃂�f���ݒ�(Pn140)						*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_FF_WAITTMOUT		8000	/* FF���x���ύX�^�C���A�E�g�F8[sec]						*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	RpiMfcTuningSetFreq( MFCTUNE *MfcTune, USHORT Frq );
PRM_RSLT	RpiMfcTuningSetFreq2( MFCTUNE *MfcTune, USHORT Frq );
PRM_RSLT	RpiMfcTuningSetFreq2Comp( MFCTUNE *MfcTune, USHORT Fil );
void		RpiRegSetVibMfcTuningFrequency( MFCTUNE *MfcTune, USHORT Frq );
void		RpiRegSetMdlVibFreq2( MFCTUNE *MfcTune, USHORT Frq2 );
void		RpiRegSetMdlVibFil2( MFCTUNE *MfcTune, USHORT Fil );



/****************************************************************************************************/
#define __FN_MFC_TUNE_H
#endif /* __FN_MFC_TUNE_H */
