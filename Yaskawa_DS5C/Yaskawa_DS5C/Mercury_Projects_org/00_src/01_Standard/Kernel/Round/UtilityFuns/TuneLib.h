#ifndef __TUNE_LIBRARY_H 
#define __TUNE_LIBRARY_H
/****************************************************************************
Description	: Tuning Function Library
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.11.16    created

*****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"
#include "PnPrmStruct.h"
#include "ResVibCtrl.h"



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		for Tuning Library																			*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_FREQ_SEARCHINIT	0		/* ���g����͏�������I��							*/
#define		TUNE_FREQ_SEARCHEND		1		/* ���g����͏�������I��							*/
#define		TUNE_FREQ_SEARCH		2		/* ���g����͏�����									*/
#define		TUNE_FREQ_SEARCHERR		-1		/* ���g����͏����ُ�I��							*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_MDLZETA			1000	/* ���f�����[�v�Q�C���␳ 				[0.1%]		*/
#define		TUNE_MFCKTFF			1000	/* ���f���g���N�t�B�[�h�t�H���[�h�⏞	[0.1%]		*/
#define		TUNE_MFCKVFF			1000	/* ���f�����x�t�B�[�h�t�H���[�h�⏞ 	[0.1%]		*/
#define		TUNE_MFCKVFF2			900		/* ���f�����x�t�B�[�h�t�H���[�h�⏞�Q	[0.1%]		*/
#define		TUNE_DTRQGN				85		/* ����O���W��     = 85[%]							*/
#define		TUNE_DOBJGN				100		/* �C�i�[�V���␳   = 100[%]						*/
#define		TUNE_DOBGNRATIO			100		/* �I�u�U�[�o�Q�C�� = 100[%]						*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_PI_TIKV			636620	/* PI�o�����X��Ti/Kv  636620 = 4 *1000000 / 2��		*/
#define		TUNE_IP_TIKV			318310	/* IP�o�����X��Ti/Kv  318310 = 2 *1000000 / 2��		*/
#define		TUNE_IP_KPTI			250000	/* IP�o�����X��Kp/Ti  ��=1.0, Kp[1/s] = 0.25 / Ti[s]*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_ANOTCH_DECA		10		/* 0.1Hz -> 1Hz�̒P�ʕϊ�							*/
#define		TUNE_ANOTCH_Q_MIN		50		/* �����m�b�`��Q�l����								*/
#define		TUNE_ANOTCH_Q_MAX		300		/* �����m�b�`��Q�l��� 3.00							*/
#define		TUNE_ANOTCH_Q_MAX_DET	500		/* ������o�p�̎����m�b�`��Q�l��� 5.00				*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_MAX_DLPFIL			4000	/* Kv�A���O���I�u�U�[�o���[�p�X�̏�� 400.0Hz		*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_RESFRQ_KV			100		/* Kv�␳�p���U���g��			[0.1Hz]				*/
#define		TUNE_ANTFRQ_KV			100		/* Kv�␳�p�����U���g��			[0.1Hz]				*/
#define		TUNE_ANTFRQ_NTQ1		100		/* �m�b�`Q�l�v�Z�p�����U���g��	[0.1Hz]				*/
#define		TUNE_ANTFRQ_NTQ2		100		/* �m�b�`Q�l�v�Z�p�����U���g��	[0.1Hz]				*/
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_FRQ_MIN			100		/* A�^���U���o���g������ 100Hz						*/
#define		AVIB_FRQ_MAX			1000	/* A�^���U���o���g����� 1000Hz						*/
#define 		AVIBDAMPGN				180		/* A�^���U���䒲���_���s���O�Q�C���ݒ�l 150%		*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_ANOTCH_MINKV		600		/* �m�b�`�t�B���^ON�Œ�Kv		[0.1Hz]				*/
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Auto Notch																				*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NOTCH_INIT		0x00	/* �m�b�`�ݒ菉��								*/
#define		OPTUNE_STS_NOTCH1_OK		0x01	/* 1�i�ڃm�b�`�ݒ萳�튮��						*/
#define		OPTUNE_STS_NOTCH2_OK		0x02	/* 2�i�ڃm�b�`�ݒ萳�튮��						*/
#define		OPTUNE_STS_NOTCHQ1_ERR		0x04	/* 1�i�ڃm�b�`Q�l�ُ�							*/
#define		OPTUNE_STS_NOTCHQ2_ERR		0x08	/* 2�i�ڃm�b�`Q�l�ُ�							*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NT_NODETECT		0		/* �m�b�`�����X�e�[�^�X�F�U�������o				*/
#define		OPTUNE_STS_NT_DETECT		1		/* �m�b�`�����X�e�[�^�X�F�U�����o��				*/
#define		OPTUNE_STS_NT_NOENABLE		2		/* �m�b�`�����X�e�[�^�X�F�����ݒ�s��			*/
#define		OPTUNE_STS_NT_DET_ERR		3		/* �m�b�`�����X�e�[�^�X�F���o�G���[				*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NT1_NODETECT		0		/* �m�b�`�����X�e�[�^�X�F1�i�ږ����{			*/
#define		OPTUNE_STS_NT1_COMP			1		/* �m�b�`�����X�e�[�^�X�F1�i�ڐݒ芮��			*/
#define		OPTUNE_STS_NT1_NG			2		/* �m�b�`�����X�e�[�^�X�F1�i�ڐݒ莸�s			*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NT2_NODETECT		0		/* �m�b�`�����X�e�[�^�X�F2�i�ږ����{			*/
#define		OPTUNE_STS_NT2_COMP			1		/* �m�b�`�����X�e�[�^�X�F2�i�ڐݒ芮��			*/
#define		OPTUNE_STS_NT2_NG			2		/* �m�b�`�����X�e�[�^�X�F2�i�ڐݒ莸�s			*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_PSTS_NODETECT		0		/* �m�b�`�����X�e�[�^�X�F���O�m�b�`�����{		*/
#define		OPTUNE_PSTS_NT1_COMP		1		/* �m�b�`�����X�e�[�^�X�F���O�m�b�`1�i��		*/
#define		OPTUNE_PSTS_NT2_COMP		2		/* �m�b�`�����X�e�[�^�X�F���O�m�b�`2�i��		*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Online Frequency Analysis																*/
/*--------------------------------------------------------------------------------------------------*/
#define	PEAK_DETECT_NUM		5					/* �X�y�N�g�����s�[�N���g�����o��				*/
#define	WORK_AREA_NUM		200					/* ���[�N�p�G���A�� [(Fmax-Fmin)/Sno]�Ƃ��邱��	*/
#define	REGULAR_SFT			16384				/* ���K���ʁF�X�y�N�g�����Z�o����OverFlow�΍�	*/
#define 	MINIMAM_FREQ		240					/* ���o���g�������l[Hz]							*/
#define	MINIMAM_SPECTRUM	3000				/* �X�y�N�g���s�[�N�����l						*/
#define	MINIMAM_SPECTRUM2	30000				/* �������X�����m�b�`�p�X�y�N�g���s�[�N�����l	*/
#define	ONFFT_OK_LMT		20					/* ���g�����臒l [�{]							*/
#define	ONFFT_OK_LMT2		50					/* �������X�����m�b�`���g�����臒l[�{]			*/
//#define	SAMPLETIME		4000					/* 1 / (�f�[�^�T���v������(sec)�~2)[Hz]			*/
#define	SAMPLETIME(SBScanTimeUs) (500000/SBScanTimeUs)	/* 1/(�f�[�^�T���v������(sec)�~2)[Hz]	*/




/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�����m�b�`�ݒ�p�萔��`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct AUTONOTCH_PRM {
	USHORT			KvrateTf;		/* ����Tf�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�					[�{]		*/
	USHORT			KvrateTfonNt;	/* �m�b�`ON���̎���Tf�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�		[�{]		*/
	USHORT			KvrateQ;		/* �m�b�`Q�l�v�Z�̃J�b�g�I�t��Kv�Ƃ̔�				[�{]		*/
	USHORT			J1Tf;			/* ����Tf�v�Z�̃��[�^���������[�����g��				[%]			*/
	USHORT			J1TfonNt;		/* �m�b�`ON���̎���Tf�v�Z�̃��[�^���������[�����g��	[%]			*/
	USHORT			J1Q;			/* �m�b�`ON����Q�l�v�Z�̃��[�^���������[�����g��	[%]			*/
	USHORT			TfMin;			/* ����Tf�v�Z�ŏ��ݒ�l								[0.01ms]	*/
	USHORT			NotchQMin;		/* �m�b�`Q�l�ŏ��ݒ�l								[-]			*/
} AUTONOTCH_PRM;



/*--------------------------------------------------------------------------------------------------*/
/*		�����m�b�`�ݒ�p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct AUTONOTCH_DATA {
	struct {						/* �U�����g����͗p�萔�f�[�^��`								*/
		USHORT			freq;			/* �����m�b�`���g��											*/
		USHORT			KvGain;			/* Tf, �m�b�`Q�l�Z�o�p�Q�C��								*/
		USHORT			Jrate;			/* Tf, �m�b�`Q�l�Z�o�p���׊������[�����g					*/
	} conf;
	struct {						/* �U�����g����͗p�ϐ��f�[�^��`								*/
		USHORT			NotchSw;		/* �����m�b�`�ݒ�I��										*/
		USHORT			TrqFil;			/* �g���N�t�B���^�ݒ�l										*/
		USHORT			NotchFil1;		/* 1�i�ڃm�b�`�t�B���^���g���ݒ�l							*/
		USHORT			NotchFil2;		/* 2�i�ڃm�b�`�t�B���^���g���ݒ�l							*/
		USHORT			NotchQ1;		/* 1�i�ڃm�b�`�t�B���^Q�l�ݒ�l								*/
		USHORT			NotchQ2;		/* 2�i�ڃm�b�`�t�B���^Q�l�ݒ�l								*/
	} var;
} AUTONOTCH_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		�����m�b�`�ݒ�p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct AUTONOTCH {
	AUTONOTCH_PRM	ANotchPrm;			/* �����m�b�`�ݒ�p�萔										*/
	AUTONOTCH_DATA	ANotchData;			/* �����m�b�`�ݒ�p�ϐ�										*/
} AUTONOTCH;



/*--------------------------------------------------------------------------------------------------*/
/*		�I�����C���U�����j�^�p�\���̒�`(���W�X�^�p)												*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FRQANA {
	USHORT		Freq;					/* ��͎��g��												*/
	USHORT		Amp;					/* ��͐U��													*/
} FRQANA;


/*--------------------------------------------------------------------------------------------------*/
/*		���g����͌��ʊi�[�p�\���̒�`(���W�X�^�p)													*/
/*--------------------------------------------------------------------------------------------------*/
/* �I�[�g�`���[�j���O�G���A���F2400H - 25FFH */
typedef	struct FFTANA_RSLT {
	FRQANA		VibmRf[3];				/* 0x2400 : �I�����C���U�����j�^���U���g��1					*/
										/* 0x2401 : �I�����C���U�����j�^���U���g��1�s�[�N�l			*/
										/* 0x2402 : �I�����C���U�����j�^���U���g��2					*/
										/* 0x2403 : �I�����C���U�����j�^���U���g��2�s�[�N�l			*/
										/* 0x2404 : �I�����C���U�����j�^���U���g��3					*/
										/* 0x2405 : �I�����C���U�����j�^���U���g��3�s�[�N�l			*/
	FRQANA		VibmAf;					/* 0x2406 : �I�����C���U�����j�^�����U���g��1				*/
										/* 0x2407 : �I�����C���U�����j�^�����U���g��1�s�[�N�l		*/
} FFTANA_RSLT;


/*--------------------------------------------------------------------------------------------------*/
/*		�U�����g����� �萔�\���̒�`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FFTANA_PRM {				/* �U�����g����͗p�p�����[�^��`						*/
	LONG		Spectrumval;				/* �U����͍��۔���X�y�N�g�����␳�l					*/
	LONG		Spectrumval2;				/* �������X�U����͍��۔���X�y�N�g�����␳�l			*/
	BOOL		ActiveFFtOn;				/* �������g����͗v���t���O								*/
	BOOL		TuneLessSts;				/* �������X�I�[�g�L���E�����t���O						*/
	LONG		SumpleNum;					/* �f�[�^�T���v����										*/
	LONG		AnaMaxFreq;					/* ��͍ő���g��										*/
	LONG		AnaMinFreq;					/* ��͍ŏ����g��										*/

	LONG		SampleTime;					/* 1 / (�f�[�^�T���v������(sec)�~2) [Hz]				*/
} FFTANA_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		�U�����g�����  �ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FFTANALYZE {				/* �U�����g����͗p�p�����[�^���ϐ���`					*/
	FFTANA_RSLT	FftAnaRslt;					/* �U�����g����͌��ʊi�[�p								*/
	FFTANA_PRM	FftAnaPrm;					/* �U�����g����͗p�p�����[�^							*/
	/*----------------------------------------------------------------------------------------------*/
	struct {								/* �U�����g����͗p�ϐ��f�[�^��`						*/
		ULONG	swork_max[PEAK_DETECT_NUM];		/* �X�y�N�g�����s�[�N�f�[�^							*/
		ULONG	freq_max[PEAK_DETECT_NUM];		/* ���g���s�[�N�f�[�^								*/
		ULONG	swork_min;						/* �X�y�N�g���������f�[�^							*/
		ULONG	freq_min;						/* ���g�������f�[�^									*/
		LONG	Reals[WORK_AREA_NUM];			/* RealPart�v�Z���[�N�p								*/
		LONG	Images[WORK_AREA_NUM];			/* ImagePart�v�Z���[�N�p							*/
	} var;
} FFTANALYZE;


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		for Tuning Library																			*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT	TunePrmWriteRam( REG_ACC_T *AccessPrm, CPRM_ATTR_T *PrmAttr, LONG Data, LONG Size );
USHORT	TunePrmLimitUnsigned( ULONG Input, CPRM_ATTR_T *PrmAttr );
SHORT	TunePrmLimitSigned( LONG Input, CPRM_ATTR_T *PrmAttr );
USHORT	TuneKvFuncTi( USHORT Kv, BOOL IPSpdControl );
USHORT	TuneKvFuncKp( USHORT Kv, USHORT Ti, BOOL IPSpdControl );
USHORT	TuneKvFuncDobsgn( USHORT Kv );
SHORT	TuneKvFuncDobLpFil( USHORT Kv, USHORT Dobsgn );
SHORT	TuneNotchQ( AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, USHORT kv, USHORT jrate, UCHAR sel, PRMDATA *Prm );
void	TuneGetNotchQSetting( AUTONOTCH *ANotch, USHORT *NotchQ1, USHORT *NotchQ2, USHORT *TrqFil );
USHORT	TuneNotchModKv( USHORT kv, USHORT jrate, USHORT j1rate );
SHORT	TuneNotchQset( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, USHORT Antfreq, USHORT *autoQ );
USHORT	TuneNotchTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, USHORT autoQ_Q1 );
USHORT	TuneOnePrmTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate );
SHORT	TuneNotchQset2( USHORT freq, USHORT Nfreq, USHORT *autoQ );
void	TuneFilGainCalbyFilType( AUTONOTCH *ANotch, UCHAR Type );
//void	TuneMfcActive( PRMDATA *Prm );
//void	TuneMfcActiveStMode( PRMDATA *Prm );
//void	TuneDobsActive( PRMDATA *Prm );
//SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm );
void	TuneMfcActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );										/*<S09E>*/
void	TuneMfcActiveStMode( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );								/*<S09E*/
void	TuneDobsActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );									/*<S09E>*/
SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );	/*<S09E>*/
SHORT	TuneNotchFilterSetting( AUTONOTCH *ANotch, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );
SHORT	TuneVibrationTrace( DETVIB *DetVib, FFTANALYZE *FftAna, UCHAR TraceRequest, SHORT TrgLvl, BOOL TuneLessSts );
USHORT	TuneMfcKpmCalcAntFreq( USHORT AntFreq );
void	TuneSetNotchFilSettingData( AUTONOTCH *ANotch, USHORT freq, USHORT KvGain, USHORT Jrate );


/*--------------------------------------------------------------------------------------------------*/
/*		for Online Frequency Analysis																*/
/*--------------------------------------------------------------------------------------------------*/
void	FftCalculateInitPrm( FFTANALYZE *FftAna, LONG SampleTimeUs );
SHORT	FftAnalyzeExecute( FFTANALYZE *FftAna, SHORT *TraceData );
void	FftSetAnaCondition( FFTANALYZE *FftAna, LONG SumpleNum, LONG AnaMaxFreq, LONG AnaMinFreq, BOOL TuneLessSts );
USHORT	FftGetAnaRsltVibmRf( FFTANALYZE *FftAna );
void	FftSetForceAnaReq( FFTANALYZE *FftAna, BOOL Setting );
BOOL	FftGetForceAnaReq( FFTANALYZE *FftAna );



/****************************************************************************************************/
/*																									*/
/*		Extern Definition																			*/
/*																									*/
/****************************************************************************************************/
extern	const	AUTONOTCH_PRM	AutoNotchTbl[ 4 ];



#endif //__TUNE_LIBRARY_H 
/***************************************** end of file **********************************************/
