#ifndef __FN_ATYPE_VIB_TUNE_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************
  Description   : A Type Vibration Tuning (Fn204)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.15   created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"




/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct AVIBTUNE
{
	struct {
		USHORT	AvibMode;			/* 0x20A0 : A�^���U�@�\�������[�h�ݒ�							*/
		USHORT	AvibState;			/* 0x20A1 : A�^���U�@�\��������									*/
		USHORT	AvibFreq;			/* 0x20A2 : A�^���U�@�\�������g��								*/
		USHORT	AvibGain;			/* 0x20A3 : A�^���U�@�\�����Q�C��								*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		SHORT	FftStat;			/* ���g����̓X�e�[�^�X											*/
		SHORT	dummy;				/* for Alignment												*/
		BOOL	FftReq;				/* ���g����͗v��												*/
		BOOL	Match;				/* �\���Ɠ��삪�����Ă��邩�H									*/
		LONG	VibDetLvl;			/* �U�����o���x��												*/
		ULONG	TimeOutCtr;			/* �^�C���A�E�g���o�p�J�E���^									*/
	} var;
} AVIBTUNE;



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum AVIB_SEQ_STS {
	AVIB_SEQ_INIT		= 0,		/* ��������														*/
	AVIB_SEQ_MODESEL,				/* ����/�蓮���[�h�I��											*/
	AVIB_SEQ_TRGWAIT,				/* �U�����o�g���K�҂�											*/
	AVIB_SEQ_MEASURE,				/* ���g����͒�													*/
	AVIB_SEQ_COMP,					/* ���g����͊���(�t�B���^�ݒ�҂�)								*/
	AVIB_SEQ_SET,					/* �t�B���^�ݒ蒆												*/
	AVIB_SEQ_FREQTUNE,				/* ���g������													*/
	AVIB_SEQ_GAINTUNE,				/* �_���s���O�Q�C������											*/
	AVIB_SEQ_END,					/* �I������														*/
	AVIB_SEQ_NOOPWAIT,				/* No Operation �\���E�F�C�g									*/
};
/*--------------------------------------------------------------------------------------------------*/
enum AVIB_CMP_STS {
	AVIB_STS_NONCOMP	= 0,		/* �U�����o�����F���o������										*/
	AVIB_STS_COMP,					/* �U�����o�����F���o����										*/
	AVIB_STS_TIMEOUT,				/* �U�����o�����F���o�s��(�^�C���A�E�g)							*/
	AVIB_STS_RANGEOV,				/* �U�����o�����F���o�s��(���g���͈͊O)							*/
	AVIB_STS_NONVIB,				/* �U�����o�����F���o�s��(���g������s��)						*/
};
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_FRQ_MIN			100		/* ���o���g������ 100Hz									*/
#define		AVIB_FRQ_MAX			1000	/* ���o���g����� 1000Hz								*/
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_MODE_MIN			0		/* ���[�h����											*/
#define		AVIB_MODE_MAX			1		/* ���[�h���											*/
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_GTUN_MAX			2		/* �Q�C�������ő包										*/
#define		AVIB_FTUN_MAX			4		/* ���g�������ő包										*/
#define		AVIB_TUN_MIN			1		/* �Q�C��/���g�������ŏ���								*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetAtypeVibTuningMode( AVIBTUNE *AvibTune, USHORT Mode );
PRM_RSLT	RpiFunSetAtypeVibTuningFrequency( AVIBTUNE *AvibTune, USHORT Frq );
PRM_RSLT	RpiFunSetAtypeVibTuningGain( AVIBTUNE *AvibTune, USHORT Gain );
void		RpiRegSetAtypeVibTuningFrequency( AVIBTUNE *AvibTune, USHORT Frq );
void		RpiRegSetAtypeVibTuningGain( AVIBTUNE *AvibTune, USHORT Gain );



/****************************************************************************************************/
#define __FN_ATYPE_VIB_TUNE_H
#endif /* __FN_ATYPE_VIB_TUNE_H */
