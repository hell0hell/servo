#ifndef __FN_ONE_PRM_TUNE_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************
  Description   : One Parameter Tuning (Fn203)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.10   created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct ONEPRMV
{
	struct {
		USHORT	SpatMode;				/* 0x2050 : 1�p�����[�^�`���[�j���O���[�h					*/
		USHORT	SpatLevel;				/* 0x2051 : 1�p�����[�^�`���[�j���O���x��					*/
		USHORT	SpatFFLevel;			/* 0x2052 : 1�p���`���[�j���O�e�e���x��						*/
		USHORT	SpatLvlChgState;		/* 0x2053 : 1�p���`���[�j���O���x���ύX����					*/
		USHORT	SpatANotchState;		/* 0x2054 : 1�p���`���[�j���O�����m�b�`����					*/
		USHORT	SpatAresState;			/* 0x2055 : 1�p���`���[�j���O�����`�^���U����				*/
		USHORT	OnePrmFilType;			/* 0x2095 : 1�p���`���[�j���O�t�B���^�^�C�v(AAT�Ƌ���)		*/
		USHORT	Dummy;					/* for Alignment											*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		BOOL	TunRun;					/* �`���[�j���O���s��										*/
		BOOL	TunSel;					/* FB/FF�`���[�j���O�I��									*/
		BOOL	FftWait;				/* �����m�b�`���s�҂�										*/
		BOOL	NotchChk;				/* �����m�b�`�ݒ�㉞���`�F�b�N								*/
		BOOL	FftReq;					/* ���g����͗v��											*/
		BOOL	ModeUnmatch;			/* �`���[�j���O���[�h�A���}�b�`								*/
/*--------------------------------------------------------------------------------------------------*/
		ULONG	FftWaitTimer;			/* �����m�b�`���s�^�C�}										*/
		ULONG	FfWaitTimer;			/* FF�ݒ�҂��^�C�}											*/
		ULONG	NotchChkTimer;			/* �m�b�`�`�F�b�N�^�C�}										*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	ModeMin;				/* �`���[�j���O���[�h�ݒ�ŏ��l								*/
		UCHAR	ModeMax;				/* �`���[�j���O���[�h�ݒ�ő�l								*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	DigMax;					/* �`���[�j���O���x���ړ��ő包								*/
		UCHAR	Dummy1;					/* for Alignment											*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	FfSetting;				/* FF�`���[�j���O�ݒ�										*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	Jrate;					/* �������[�����g/�d�ʔ�ݒ�								*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	OrgKv;					/* �ύX�O��Kv												*/
		USHORT	OrgMdlgn;				/* �ύX�O��Kpm												*/
		USHORT	OrgNf1;					/* �ύX�O�̑�1�m�b�`�t�B���^���g��							*/
		USHORT	OrgNf2;					/* �ύX�O�̑�2�m�b�`�t�B���^���g��							*/
		USHORT	OrgNfQ1;				/* �ύX�O�̑�1�m�b�`�t�B���^Q�l								*/
		USHORT	OrgNfQ2;				/* �ύX�O�̑�2�m�b�`�t�B���^Q�l								*/
		USHORT	OrgAResf;				/* �ύX�O�̂`�^���U���g��									*/
		USHORT	OrgAResDGn;				/* �ύX�O�̂`�^���U�_���s���O�Q�C��							*/
/*--------------------------------------------------------------------------------------------------*/
		SHORT	FftStat;				/* ���g����̓X�e�[�^�X										*/
		SHORT	Dummy2;					/* for Alignment											*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	ANotchSetStat;			/* �����m�b�`�ݒ茋�ʏ�									*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	ANotchStat;				/* �����m�b�`�ݒ���s��									*/
		UCHAR	ANotch1Stat;			/* �����m�b�`�P�i�ڃm�b�`�ݒ茋��							*/
		UCHAR	ANotch2Stat;			/* �����m�b�`�Q�i�ڃm�b�`�ݒ茋��							*/
		UCHAR	ANotchPreStat;			/* �����m�b�`���O�m�b�`�ݒ�									*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	AResSetStat;			/* �����`�^�ݒ茋�ʏ�										*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	AResStat;				/* �����`�^�ݒ���s��										*/
		UCHAR	ARes1Stat;				/* �����`�^�ݒ茋��											*/
/*--------------------------------------------------------------------------------------------------*/
	} var;
} ONEPRMTUNE;


typedef	struct TUNEGAIN
{
	USHORT	loophz;					/* Pn100:���x���[�v�Q�C��										*/
	USHORT	pitime;					/* Pn101:���x���[�v�ϕ����萔									*/
	USHORT	posgn;					/* Pn102:�ʒu���[�v�Q�C��										*/
	USHORT	trqfil11;				/* Pn401:�g���N�t�B���^���萔									*/
	USHORT	mdlgn;					/* Pn141:���f�����[�v�Q�C��										*/
	USHORT	dobgn;					/* Pn121:���C�⏞�Q�C��											*/
	USHORT	dlpfil;					/* Pn124:���C�⏞���g���␳										*/
	USHORT	ntchq1;					/* Pn40A:��1�m�b�`�t�B���^Q�l									*/
	USHORT	ntchq2;					/* Pn40D:��2�m�b�`�t�B���^Q�l									*/
} TUNEGAIN;


typedef	struct TUNECALV
{
	ULONG		PIRate;
	ULONG		KpZeta;
	ULONG		Ti;
	TUNEGAIN	Gain;				/* �`���[�j���O�Q�C��											*/
} TUNECALV;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum OPTUNE_SEQ_STS {
	OPTUNE_SEQ_INIT				= 0,		/* ��������												*/
	OPTUNE_SEQ_JSET,						/* �������[�����g��ݒ菈��								*/
	OPTUNE_SEQ_MODESEL,						/* ���[�h�ݒ菈��										*/
	OPTUNE_SEQ_FILSEL,						/* �t�B���^�^�C�v�ݒ菈��								*/
	OPTUNE_SEQ_PREDISP,						/* �����O�Q�C���\������									*/
	OPTUNE_SEQ_TUNING,						/* 1�p����������										*/
	OPTUNE_SEQ_VIBSEL,						/* �U���}������											*/
	OPTUNE_SEQ_POSTDISP,					/* ������Q�C���\������									*/
	OPTUNE_SEQ_NOTCHERR,					/* �m�b�`���o�G���[����									*/
	OPTUNE_SEQ_END,							/* �I������												*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_FLAG_SEL {
	OPTUNE_FLAG_FBSEL			= 0,		/* �t�B�[�h�o�b�N�Q�C���I��								*/
	OPTUNE_FLAG_FFSEL,						/* �t�B�[�h�t�H���[�h�Q�C���I��							*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_ARES_DETVIB_STS {
	OPTUNE_STS_ARES_NOVIBDET	= 0,		/* �`�^���U�����X�e�[�^�X�F�U�������o					*/
	OPTUNE_STS_ARES_DETECT,					/* �`�^���U�����X�e�[�^�X�F�U�����o��					*/
	OPTUNE_STS_ARES_NOENABLE,				/* �`�^���U�����X�e�[�^�X�F�����ݒ�s��					*/
	OPTUNE_STS_ARES_DET_ERR,				/* �`�^���U�����X�e�[�^�X�F���o�G���[					*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_ARES_SET_STS {
	OPTUNE_STS_ARES_INIT		= 0,		/* �`�^���U�ݒ菉��										*/
	OPTUNE_STS_ARES_OK,						/* �`�^���U�ݒ萳�튮��									*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_ARES_COMP_STS {
	OPTUNE_STS_ARES_NODETECT	= 0,		/* �`�^���U�����X�e�[�^�X�F�����{						*/
	OPTUNE_STS_ARES_COMP,					/* �`�^���U�����X�e�[�^�X�F�ݒ芮��						*/
	OPTUNE_STS_ARES_NG,						/* �`�^���U�����X�e�[�^�X�F�ݒ莸�s						*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_FF_STS {
	OPTUNE_STS_FFCOMP			= 0,		/* FF���x���ύX������ԁF�ύX����/�C�j�V����			*/
	OPTUNE_STS_FFWAIT,						/* FF���x���ύX������ԁF�ύX�҂�						*/
	OPTUNE_STS_FFTMOUT,						/* FF���x���ύX������ԁF�ύX���s(�^�C���A�E�g)			*/
};
/*--------------------------------------------------------------------------------------------------*/


#define		OPTUNE_JTUN_MAX		5			/* �������[�����g/�d�ʔ䒲���ő包						*/
#define		OPTUNE_MFC_ACTIVE	0x0001		/* ���̌n�ݒ�(Pn140)									*/
#define		OPTUNE_MODE_MIN		0			/* 1�p�����[�h�����l									*/
#define		OPTUNE_MODE_MAX		3			/* 1�p�����[�h����l									*/
#define		TUNE_MAX_TI			3000		/* Ti����l												*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
void		OnePrmReSetTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT loophz, USHORT posgn, BOOL IPSpdControl );
PRM_RSLT	RpiFunSetOnePrmTuningMode( ONEPRMTUNE *OnePrmTune, USHORT Mode );
PRM_RSLT	RpiFunSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl );
PRM_RSLT	RpiFunSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl );
void		RpiRegSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain );
void		RpiRegSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain );



/****************************************************************************************************/
#define __FN_ONE_PRM_TUNE_H
#endif /* __FN_ONE_PRM_TUNE_H */
