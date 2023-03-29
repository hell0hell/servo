#ifndef __AUTO_NOTCH_SET_H 
#define __AUTO_NOTCH_SET_H
/****************************************************************************
Description	: Auto Notch Filter Parameter Setting Function
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.11.17    created

*****************************************************************************/
#include "Basedef.h"
#include "TuneLib.h"
#include "ResVibCtrl.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define		ANOTCH_MIN_SET_FREQ			400		/* �����m�b�`�ŏ��ݒ���g��	[Hz]					*/
#define		ANOTCH_MIN_SET_FREQ2		200		/* �������o�������m�b�`�ŏ��ݒ���g��	[Hz]		*/
#define		ANOTCH_NOTCH_SET_CHK_TIME	3000	/* �m�b�`�ݒ��҂�����		[ms]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		NOTCH_COMP_MAX_FREQ			800		/* �m�b�`���g���␳�ő���g��	[Hz]				*/
#define		NOTCH_COMP_RATIO			170		/* �m�b�`���g���␳����			[%]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		NOTCH_BASIC_VIB_LEVEL		600		/* �U�����o���x��(��])			[0.1min-1]			*/
#define		NOTCH_BASIC_VIB_LEVEL_L		200		/* �U�����o���x��(�Ʊ)			[0.1mm/s]			*/
#define		NOTCH_FILTYPE				2		/* �����m�b�`���̃t�B���^�^�C�v �^�C�v2�Œ�			*/
#define		NOTCH_FILTYPE2				3		/* 2000Hz�ȏ�̎����m�b�`���t�B���^�^�C�v �^�C�v3	*/
#define		FREQDETUNIT					20		/* ���g����͌��o���g������\						*/
#define		ANOTCH_FRAT_106( data )		(((data)*17)>>4)	/* 106.25%								*/




/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�����m�b�`�V�[�P���X�p �萔�E�ϐ��f�[�^��`													*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ANOTCHSEQ {
	struct {								/* �����m�b�`�V�[�P���X�p���W�X�^��`					*/
		USHORT		SpatANotchState;			/* 0x2054 : �����m�b�`�X�e�[�^�X					*/
	} OpeReg;
	struct {								/* �����m�b�`�V�[�P���X�p�萔��`						*/
		USHORT		kv;							/* �����m�b�`�v�Z�p���x���[�v�Q�C��		[0.1Hz]		*/
		USHORT		jrate;						/* �����m�b�`�v�Z�p���׊������[�����g/���ʔ�[%]		*/
		BOOL		EepromWriteProtect;			/* EEPROM�������݋֎~�t���O							*/
		BOOL		BaseEnableSts;				/* �T�[�{ON/OFF��ԃt���O							*/
		BOOL		TuneLessSts;				/* �������X�L���E�����t���O							*/
		BOOL		OverTravelSts;				/* P-OT/N-OT��ԃt���O								*/
		LONG		VibCheckLevelForTunLess ;	/* �����m�b�`�p�U�����o���x��			[2^24/OS]	*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �����m�b�`�V�[�P���X�p�ϐ���`						*/
		BOOL		FftReq;						/* ���g����͗v��									*/
		BOOL		NotchChk;					/* �m�b�`�`�F�b�N��									*/
		ULONG		NotchChkTimer;				/* �m�b�`�`�F�b�N�^�C�}(�U�����o�҂�)				*/
		SHORT		FftStat;					/* ���g����̓X�e�[�^�X								*/
		SHORT		ANotchSetStat;				/* �����m�b�`�ݒ茋�ʏ�							*/
		UCHAR		ANotchStat;					/* �����m�b�`�ݒ���s��							*/
		UCHAR		ANotch1Stat;				/* �����m�b�`�P�i�ڃm�b�`�ݒ茋��					*/
		UCHAR		ANotch2Stat;				/* �����m�b�`�Q�i�ڃm�b�`�ݒ茋��					*/
		UCHAR		ANotchPreStat;				/* �����m�b�`���O�m�b�`�ݒ�							*/
	} var;
} ANOTCHSEQ;



/*--------------------------------------------------------------------------------------------------*/
/*		�����m�b�`�V�[�P���X�R�[���p �|�C���^�\���̒�`												*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ANOTCHSEQCTRL {
	ANOTCHSEQ			*ANotchSeq;			/* �����m�b�`�V�[�P���X�p�\����							*/
	AUTONOTCH			*ANotch;			/* �����m�b�`�ݒ�p�\����								*/
	FFTANALYZE			*FftAna;			/* �U�����g�����(Online FFT)�p�\����					*/
	DETVIB				*DetVib;			/* �U�����o�p�\����										*/
	PRMDATA				*Prm;				/* �p�����[�^�f�[�^�\����								*/
	REGIF_CONFIG_T		*RegCfg;			/* ���W�X�^IF�R���t�B�O���[�V�����p�\����				*/
} ANOTCHSEQCTRL;



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchFilterSetting( ANOTCHSEQCTRL *ANotchCtrl );
void	AutoNotchInitialize( ANOTCHSEQ *ANotchSeq );
void	AutoNotchCalculatePrm( ANOTCHSEQ *ANotchSeq, USHORT kv, USHORT jrate);
void	AutoNotchCalculateInitPrm( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, LONG AmpType, UNI_PRM_STR *UniPrm );
void	RpiResetStatusANotch1( ANOTCHSEQ *ANotchSeq );
void	RpiResetStatusANotch2( ANOTCHSEQ *ANotchSeq );
UCHAR	RpiGetANotchPreStat( ANOTCHSEQ *ANotchSeq );
USHORT	RpiGetANotchSetStat( ANOTCHSEQ *ANotchSeq );
void	RpiSetRegSpatANotchState( ANOTCHSEQ *ANotchSeq );
void	RpiANotchFftReqSet( ANOTCHSEQ *ANotchSeq, FFTANALYZE *FftAna );
void	RpiANotchTrqFilCalc( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );
void	RpiANotchDisableReqSet( ANOTCHSEQ *ANotchSeq );
void	AutoNotchSetCondition( ANOTCHSEQ *ANotchSeq, BOOL EepromWriteProtect, 
										BOOL BaseEnableSts, BOOL TuneLessSts, BOOL OverTravelSts );


#endif //__AUTO_NOTCH_SET_H 
/***************************************** end of file **********************************************/
