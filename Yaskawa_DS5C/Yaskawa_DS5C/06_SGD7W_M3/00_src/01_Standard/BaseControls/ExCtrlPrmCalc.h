/****************************************************************************************************/
/*																									*/
/*																									*/
/*		�������X�p�p�����[�^�v�Z�w�b�_�[�t�@�C��													*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.09.28	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_TUNELESS_PRM_CALC_H
#define _TUNELESS_PRM_CALC_H


#include "BaseControls.h"
#include "BaseControlPrmCalc.h"
#include "ResVibCtrl.h"
#include "AutoNotchSet.h"



/****************************************************************************************************/
/*																									*/
/*		STRUCT DEFINITION																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�������X���x����`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_GAIN_TBL {
	USHORT		Kv;				/* ���x���[�v�Q�C��									[0.1Hz]			*/
	USHORT		F1rate;			/* �I�u�U�[�o�Q�C���䗦								[%]				*/
} TUNELESS_GAIN_TBL;
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_JRATE_TBL {
	USHORT		Jrate;			/* ���׊������[�����g/���ʔ�						[%]				*/
	USHORT		dummy;			/* for alignment													*/
} TUNELESS_JRATE_TBL;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		DEFINES																						*/
/*																									*/
/****************************************************************************************************/
#define		PRM_SETTING_OK			0			/* �p�����[�^�ݒ�l����p(����)						*/
#define		PRM_SETTING_NG			-1			/* �p�����[�^�ݒ�l����p(�ُ�)						*/
#define		PRMERR_TUNELESS_LVL		0x01		/* Pn170 �ݒ�ُ�									*/
#define		PRMERR_TUNELESS_TYPE	0x02		/* Pn14F �ݒ�ُ�									*/


/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
/* �������X�Q�C���Z�o�p */
#define		TULESCYCLE			KPI_SACYCLENS		/* �������X���Z�T�C�N���^�C��(ScanA)		[ns]*/
#define		TULESCYCLEUS		((TULESCYCLE + 500)/1000)	/* �������X���Z�T�C�N���^�C��		[us]*/
/*--------------------------------------------------------------------------------------------------*/
#define		TLSPDDET13BIT		(KPI_SACOUNTMS*600000/8192)			/* 13bit�G���R�[�_���x���o����\			*/
#define		TLSPDDET200UM		(KPI_SACOUNTMS*10000/256*200/1000)	/* �X�P�[���s�b�`200um�̑��x���o����\		*/
/*--------------------------------------------------------------------------------------------------*/
#define		TLESSGLVLMAX		10				/* �������X�Q�C�����x���ő�l						*/
#define		TLESSJLVLMAX		5				/* �������X�C�i�[�V�����x���ő�l					*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNELESSJRAT		30				/* �������XJL�ڕW����͈�				[�{]		*/
#define		DISOBSFSTD			6000			/* �������XRobust�⏞����g��1�F(�)[0.1Hz]		*/
#define		TUNELESS_STIFFSTD	100				/* �������XRobust�����x	100%(�)					*/
/*--------------------------------------------------------------------------------------------------*/
//#define	DISOBSF11			TuneLessGain	/* �������XRobust�⏞����g��1�F���o�X�g���K��(�����΍�)[0.1Hz]	*/
#define		DISOBSF12			3000			/* �������XRobust�⏞����g��1�F���o�X�g���K��(�����΍�)[0.1Hz]	*/
#define		DISOBSF21			8000			/* �������XRobust�⏞����g��2�F�m�C�Y�J�b�g�p(�W��)[0.1Hz]		*/
#define		DISOBSF22			8000			/* �������XRobust�⏞����g��2�F�m�C�Y�J�b�g�p(�����΍�)[0.1Hz]	*/
#define		DISOBSF1MIN			1500			/* �������XRobust�⏞����g��1�ŏ��l[0.1Hz]			*/
/*--------------------------------------------------------------------------------------------------*/
#define		VOBSLPFGNHZ			(DISOBSFSTD/TUNELESSJRAT)	/* �������X���x�I�u�U�[�oLPF�v�Z���g��[0.1Hz]		*/
/*--------------------------------------------------------------------------------------------------*/
#define		DISOBSKJ			100				/* �������X�O���I�u�U�[�o�W���Q�C��		[%]			*/
#define		DISOBSKD			99				/* �������X�O���I�u�U�[�o�W��			[%]			*/
#define		VOBSKS				60				/* �������X���x�I�u�U�[�o�Q�C��			[Hz]		*/
#define		VOBSKJ				100				/* �������X���x�I�u�U�[�o�W���Q�C��		[%]			*/
#define		VPHASEF2			10000			/* �������X�p�ʑ��⏞�t�B���^���g��2	[0.1Hz]		*/
#define		VPHASEFULIM			1				/* �������X�p�ʑ��⏞�t�B���^���g�������l[0.1Hz]	*/
/*--------------------------------------------------------------------------------------------------*/
#define		EHVOBSTFULIM		2				/* �������X�ʑ��⏞���xOBS LpFil���萔�����l[10us]	*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_IP_KPTI		250000			/* IP�o�����X��Kp/Ti ��=1.0, Kp[1/s] = 0.25 / Ti[s]	*/
#define		TUNLESS_TREFFIL		10				/* �g���N�⏞��g���N�w�߃t�B���^���萔[0.01ms]		*/
/*--------------------------------------------------------------------------------------------------*/
//#define	TUNELESS_STIFF1		TunLessStiff	/* �������X���o�X�g�����x	(�W��)					*/
#define		TUNELESS_STIFF2		50				/* �������X���o�X�g�����x2	50%(���萫�d��)			*/
#define		TUNELESS_STIFF3		30				/* �������X���o�X�g�����x3	30%(���萫�d��)			*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBOBS_KS			800				/* �U�����o�I�u�U�[�oKs�Q�C��			[0.1Hz]		*/
/*--------------------------------------------------------------------------------------------------*/
//#define	DAT_GAINLEVEL_MAX	4				/* �������X�Q�C�����x���ő�l						*//* <S1C7> */
#define		DAT_GAINLEVEL_MAX	7				/* �������X�Q�C�����x���ő�l						*//* <S1C7> */
#define		DAT_GAINLEVEL_MIN	0				/* �������X�Q�C�����x���ŏ��l						*/
#define		DAT_JRATLEVEL_MAX	2				/* �������X�C�i�[�V�����x���ő�l					*/
#define		DAT_JRATLEVEL_MIN	0				/* �������X�C�i�[�V�����x���ŏ��l					*/
/*--------------------------------------------------------------------------------------------------*/
#define		TLEX_DISOBSFRQ		4000			/* �V�������X�O���I�u�U�[�o���g��					*/
#define		TLEX_DISOBSKD		99				/* �V�������X�O���I�u�U�[�o�W��						*/
#define		TLEX_VOBSLPFGAIN	200				/* �V�������X�ʑ��⏞���x�I�u�U�[�o�Q�C��			*//* <S1C7> */
//#define	TLEX_VOBSLPFGAIN	150				/* �V�������X�ʑ��⏞���x�I�u�U�[�o�Q�C��			*//* <S1C7> */
#define		TLEX_VPHASEF1		300				/* �V�������X�ʑ��⏞�t�B���^���g��1				*/
#define		TLEX_VPHASEF2		600				/* �V�������X�ʑ��⏞�t�B���^���g��2				*/
#define		TLEX_VOBSKS			120				/* �V�������X�ʑ��⏞���x�I�u�U�[�o�Q�C��			*//* <S1C7> */
//#define	TLEX_VOBSKS			160				/* �V�������X�ʑ��⏞���x�I�u�U�[�o�Q�C��			*//* <S1C7> */
#define		TLEX_TRQLPFIL		8				/* �V�������X�g���N�t�B���^���萔					*/
#define		TLEX_BNDPASSFILZN	10000			/* �V�����o���h�p�X�t�B���^�����W��					*/
#define		TLEX_RESSUPFIL		2000			/* �V�������X �@�B���U�}���t�B���^���萔			*/

/* �������X���[�h�ݒ�p */
enum TUNELESS_MODE_SELECT{
	TLMODE_NORMAL		= (LONG)0x00L,			/* TuningLessMode NORMAL							*/
	TLMODE_CORELESS		= (LONG)0x01L,			/* TuningLessMode for CoreLess Linear				*/
	TLMODE_STABLE_L		= (LONG)0x10L,			/* TuningLessMode Stable Form for Linear			*/
	TLMODE_STABLE_R		= (LONG)0x20L,			/* TuningLessMode Stable Form for Rotary			*/
	TLMODE_SILENT		= (LONG)0x40L,			/* TuningLessMode Silent Form						*/
	TLMODE_SPDDETCHNG	= (LONG)0x100L,			/* TuningLessMode Speed Detect Change Request		*/
};


/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
/* �O���I�u�U�[�o���Z�T�C�N���^�C��(���x���[�v�Ɠ������)[ns] */
#define	DOBSCYCLE		KPI_SACYCLENS


/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
/* �ʑ��⏞���x�I�u�U�[�o���Z�T�C�N���^�C��(���x���[�v�Ɠ������)[us] */
#define	EHSOBSCYCLE		KPI_SACYCLEUS


/****************************************************************************************************/
/*																									*/
/*		STRUCTS																						*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Tuning Less Structures for API function Parameters											*/
/****************************************************************************************************/
typedef struct _TUNELESS_CFGPRM {
	USHORT		gnmode;					/* Pn10B : �Q�C���֌W���p�X�C�b�`							*/
	USHORT		mfctype;				/* Pn14F : ����֘A�X�C�b�`									*/
	USHORT		avibsw;					/* Pn160 : ���U����֘A�X�C�b�`								*/
	USHORT		DatLevel;				/* Pn170 : �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�`		*/
	USHORT		tfuncsw;				/* Pn408 : �g���N�֌W�@�\�X�C�b�`							*/
	USHORT		flg_wf;					/* PnF1E : �t���O											*/
} TUNELESS_CFGPRM;

typedef struct _TUNELESS_SETPRM {
	USHORT		DatLevel;				/* Pn170 : �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�`		*/
	USHORT		trqfil11;				/* Pn401 : 1�i�ڑ�1�g���N�w�߃t�B���^���萔					*/
	USHORT		ampw;					/* PnE12 : �T�[�{�p�b�N�e��									*/
	USHORT		ignq;					/* PnE21 : �d�����[�v�Q�C��(Q)								*/
	USHORT		kiq;					/* PnE23 : �d�����[�v�ϕ����萔(Q)							*/
	USHORT		fbfil2;					/* PnEA8 : ��2���xF/B�t�B���^���萔							*/
	USHORT		TuneLessGain;			/* PnEDA : �������X�O���I�u�U�[�o�Q�C��						*/
	USHORT		TuneLessStiff;			/* PnEDB : �������X�C�i�[�V���␳�Q�C��						*/
	USHORT		TuneLessTrqLpf;			/* PnEDC : �������X�O���g���N���[�p�X�t�B���^���萔			*/
	USHORT		TuneLessVfbLpf;			/* PnEDD : �������X���xFB���[�p�X�t�B���^���萔				*/
	MENCPRM		*MencP;					/* PnF00 - PnF1E : �G���R�[�_�p�����[�^						*/
	DETVIB		*DetVib;				/* ���U�Ď��p�f�[�^��`										*/
	ANOTCHSEQ 	*ANotchSeq;				/* �����m�b�`�p�f�[�^��`									*/
} TUNELESS_SETPRM;


/****************************************************************************************************/
/*		Disturb Observer Structures for API function Parameters										*/
/****************************************************************************************************/
typedef struct _DOBS_CFGPRM {
	USHORT		jrate;					/* Pn103 : �������[�����g��									*/
	USHORT		loophz;					/* Pn100 : ���x���[�v�Q�C��									*/
	USHORT		loophz2;				/* Pn104 : ��2���x���[�v�Q�C��								*/
	USHORT		dobgn;					/* Pn121 : ���C�⏞�Q�C��									*/
	USHORT		dobgn2;					/* Pn122 : ��2���C�⏞�Q�C��								*/
	USHORT		dtrqgn;					/* Pn123 : ���C�⏞�W��										*/
	SHORT		dlpfil;					/* Pn124 : ���C�⏞���g���␳								*/
	USHORT		dobjgn;					/* Pn125 : ���C�⏞�Q�C���␳								*/
} DOBS_CFGPRM;


/****************************************************************************************************/
/*		Eh Velocity Observer Structures for API function Parameters									*/
/****************************************************************************************************/
typedef struct _EHVOBS_CFGPRM {
	USHORT		jrate;					/* Pn103 : �������[�����g��									*/
	USHORT		evobgn;					/* Pn127 : ���x�I�u�U�[�o�Q�C��								*/
	USHORT		evobjgn;				/* Pn128 : ���x�I�u�U�[�o�ʒu�⏞�Q�C��						*/
	USHORT		trqfil11;				/* Pn401 : 1�i�ڑ�1�g���N�w�߃t�B���^���萔					*/
} EHVOBS_CFGPRM;



/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
//VOID	TuneLessCalculatePrm( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm, TUNELESS_SETPRM *Prm );		  /* <S01D> */
BOOL	TuneLessCalculatePrm( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm,								  /* <S066> */
								TUNELESS_SETPRM *Prm, BASE_SPDLOOP_PRM *SpdCtrlPrm );
LONG	TuneLessCalculateInitPrm( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm,
								  MENCV *MencV, ASICS *SvAsicRegs, TUNELESS_CFGPRM *CfgPrm );
BOOL	TuneLessSetTrqFil3( BOOL TuningLess, BOOL TuningLessEx, ULONG flg_wf,
							BOOL TuningLessLowGainMotor );

/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
LONG	DobsCalculatePrmSW( BOOL *DobsAct, BOOL TuningLess, USHORT DobsSel );
VOID	DobsCalculatePrm( DOBS_PRM *DobsPrm, ASICS *SvAsicRegs,
						  BPRMDAT *Bprm, DOBS_CFGPRM *CfgPrm, LONG GselNo );

/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
BOOL	EhVobsCalculatePrmSW( BOOL *EhVobsUse, BOOL TuningLess, USHORT EhVobsSel );					  /* <S066> */
BOOL	EhVobsCalculatePrm( EHVOBS_CTRL *EhVobsCtrl, BPRMDAT *Bprm,									  /* <S066> */
							EHVOBS_CFGPRM *CfgPrm, TRANSDUCE_CTRL *TransduceCtrl );


/****************************************************************************************************/
/*																									*/
/*		EXTERN																						*/
/*																									*/
/****************************************************************************************************/
extern	void	RpiRegSetTuneLessLevel( ULONG Gain, TUNELESS_CTRL *TuneLessCtrl );




/***************************************** end of file **********************************************/
#endif /* _TUNELESS_PRM_CALC_H */
