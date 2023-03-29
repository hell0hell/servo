#ifndef	_RES_VIB_CTRL_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ResVibCtrl.h : �U�����o�E���U����p�w�b�_�[�t�@�C��											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.11.11	Y.Oka	for INGRAM												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "JL086ServoIpReg.h"


/****************************************************************************************************/
/*																									*/
/*		�萔��`																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		for AType Restrain Vibration																*/
/*--------------------------------------------------------------------------------------------------*/
//#define	ARESCYCLE			KPI_SBCYCLENS		/* A�^���U���䉉�Z�T�C�N���^�C��(ScanB) [ns]		*/
//#define	C10POW7				10000000			/* 10^7												*/
//#define	C10POW9				1000000000			/* 10^9												*/
//#define	C2PAIE7				62831853			/* (2*PAI) * 10^7									*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Detect Vibration																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	DETECT_VIB_NOT_DETECT	0x0000			/* �U�����o���[�j���O�o��							*/
#define	DETECT_VIB_WARNING		0x0001			/* �U�����o���[�j���O�o��							*/
#define	DETECT_VIB_ALARM		0x0010			/* �U�����o�A���[���o��								*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Detect Vibration Trace																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	DETVIBTRC_BUFMAX		200				/* �ő�g���[�X�o�b�t�@��							*/
#define DETVIBTRC_NOCMD			0x00			/* �g���[�X�w�ߖ���									*/
#define	DETVIBTRC_TRCEXE		0x10			/* �g���[�X���s�w��									*/
#define	DETVIBTRC_TRCEND		0x1F			/* �g���[�X���s�I������								*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Vibration Detect Observer																*/
/*--------------------------------------------------------------------------------------------------*/
//#define	DETVIBCYCLENS	KPI_SBCYCLENS	/* �U�����o���Z�T�C�N���^�C��(ScanB)[ns]					*/
//#define	DETVIBCYCLEUS	KPI_SBCYCLEUS	/* �U�����o���Z�T�C�N���^�C��(ScanB)[us]					*/
#define	VIBOBSGN		800				/* �U�����o�p�I�u�U�[�o�Q�C��(80Hz)							*/
#define	DETVIBLP		3183			/* �����p���U�����o�p���[�p�X�t�B���^(3183us = 50Hz)		*/
#define	DETVIBHP		320				/* ���U���o�p�n�C�p�X�t�B���^�F0.32[ms]=500[Hz]				*/


/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�U�����o�p���x�I�u�U�[�o�ϐ���`															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DETVIBOBS {					/* �U�����o�p���x�I�u�U�[�o�p�����[�^���ϐ���`			*/
	struct	{								/* �U�����o�p���x�I�u�U�[�o�p�����[�^��`				*/
		KSGAIN	Ks;								/* �U�����o�p�I�u�U�[�o�Q�C��						*/
		LONG	Kj;								/* �U�����o�p�I�u�U�[�o�C�i�[�V���Q�C��				*/
		LONG	Klpf;							/* �U�����o�p���[�p�X�t�B���^�Q�C��					*/
		LONG	Khpf;							/* �U�����o�p�n�C�p�X�t�B���^�Q�C��					*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {								/* �U�����o�p���x�I�u�U�[�o�ϐ��f�[�^��`				*/
		LONG	Ivar64[2];						/* �ϕ����Z�p										*/
		LONG	AccSum;							/* �����x�ώZ�l(���x����l)							*/
		LONG	SpdObs;							/* ���x����l										*/
		LONG	AvSpdFbk;						/* 2�񕽋ϑ��x										*/
		LONG	LastSpdErr;						/* ���x�΍��O��l									*/
		LONG	AvSpdErr;						/* 2�񕽋ϑ��x�΍�									*/
		LONG	AvSpdErrbfFil;					/* �t�B���^�����O���x�΍�							*/
		LONG	AvSpdErrLpfOut;					/* 2�񕽋ϑ��x�΍�(���[�p�X�t�B���^�o��)			*/
		LONG	Hpftmp;							/* �n�C�p�X�t�B���^���Z�p							*/
		/*------------------------------------------------------------------------------------------*/
		LONG	TrcObsAvSpdErr;					/* Trace  : 2�񕽋ϑ��x�΍�(�����m�b�`�p�g���[�X)	*/
		BOOL	SpdErrClrReq;					/* Alarm  : �U�����o�p�����x�������v���t���O		*/
		LONG	MaxSpdErr;						/* Alarm  : �U�����o�p�����x�ő�l	[2^24/OvrSpd]	*/
		LONG	MinSpdErr;						/* Alarm  : �U�����o�p�����x�ŏ��l	[2^24/OvrSpd]	*/
		BOOL	SpdErrFnClrReq;					/* Fn�p   : �U�����o�p�����x�������v���t���O		*/
		LONG	MaxSpdErrFn;					/* Fn�p   : �U�����o�p�����x�ő�l	[2^24/OvrSpd]	*/
		LONG	MinSpdErrFn;					/* Fn�p   : �U�����o�p�����x�ŏ��l	[2^24/OvrSpd]	*/
		LONG	UnMaxSpdErr;					/* Un10C  : �U�����o�p�����x�ő�l	[2^24/OvrSpd]	*/
		LONG	UnMinSpdErr;					/* Un10C  : �U�����o�p�����x�ŏ��l	[2^24/OvrSpd]	*/
		/*------------------------------------------------------------------------------------------*/
	} var;
} DETVIBOBS;


/*--------------------------------------------------------------------------------------------------*/
/*		���x�I�u�U�[�o�ɂ�鑬�x�΍��g���[�X�p�f�[�^��`											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DETVIBTRC {					/* ���x�I�u�U�[�o�ɂ�鑬�x�΍��g���[�X�p�f�[�^��`		*/
		UCHAR	TrcCmd;							/* �g���[�X���s�w��(�w�߁^����)						*/
		UCHAR	TrcInt;							/* �g���[�X�Ԋu�w��									*/
		USHORT	TrcNum;							/* �g���[�X�񐔎w��									*/
//		LONG	*TrcVarA;						/* �g���[�X�ϐ��w��A [2^24/���K���l]				*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	TrcSamp;						/* �g���[�X�Ԋu����p								*/
		USHORT	TrcIndx;						/* �g���[�X�f�[�^�C���f�N�X							*/
		SHORT	TrcBufA[DETVIBTRC_BUFMAX];		/* �g���[�X�f�[�^�o�b�t�@A [16384/���K���l]			*/
		SHORT	TrcTrgLvl;						/* �g���[�X�g���K���x��								*/
} DETVIBTRC;


/*--------------------------------------------------------------------------------------------------*/
/*		���U�Ď��p�f�[�^��`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DETVIB {
	DETVIBTRC	DetVibTrc;						/* ���x�I�u�U�[�o�ɂ�鑬�x�΍��g���[�X�p�ϐ�		*/
	DETVIBOBS	DetVibObs;						/* �U�����o�p���x�I�u�U�[�o�ϐ�						*/
	struct	{									/* ���U�Ď��p�p�����[�^��`							*/
		LONG	VibCheckLevel;						/* �U�����o���x�� [2^24/OvrSpd]					*/
		LONG	JstVibCheckLevel;					/* �C�i�[�V�����莞�U�����o���x�� [2^27/OvrSpd]	*/
		LONG	VibCheckSel;						/* �U�����o�I��									*/
	} conf;
	struct	{									/* ���U�Ď��p�ϐ���`								*/
		BOOL	VibDetFlg;							/* �U�����o�t���O								*/
		BOOL	LowFreqVibDetFlg;					/* ����g�U�����o�t���O							*/
		BOOL	AatAlmCntFlg;						/* AAT�A���[�����o�p�o�ߎ��ԃJ�E���g�t���O		*/
		ULONG	AatAlmCnt;							/* AAT�A���[�����o�p�o�ߎ��ԃJ�E���g			*/
		ULONG	LowFreqVibAlmCnt;					/* ����g�U���A���[�����o�p�o�ߎ��ԃJ�E���g		*/
		LONG	SumSpdErr;							/* ����g�U�����o�p�����x�ώZ�l					*/
	} var;
} DETVIB;


/*--------------------------------------------------------------------------------------------------*/
/*		���U���䉉�Z�p�萔��`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct RESVIBP {						/* ���U����p�p�����[�^��`							*/
	LONG	Ks;										/* ���U�I�u�U�[�o�Q�C��							*/
	LONG	Kj;										/* ���U�I�u�U�[�o�C�i�[�V���Q�C��				*/
	LONG	Knd;									/* ���U���x�_���s���O�Q�C��						*/
	LONG	Klpf;									/* ���U���[�p�X�t�B���^�Q�C��					*/
	LONG	Khpf;									/* ���U�n�C�p�X�t�B���^�Q�C��					*/
	LONG	Khpf1;								    /* <S03E> 3�������U����n�C�p�X�t�B���^�Q�C��			*/
	LONG	Knd1;									/* <S03E> 3�������U����_���s���O�Q�C��2				*/
} RESVIB_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		���U���䉉�Z�p�ϐ���`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct RESVIB {							/* ���U����p�p�����[�^���ϐ���`					*/
		RESVIB_PRM	ResVibPrm;						/* ���U����p�p�����[�^							*/
		BOOL		AresVibSetting;					/* A�^���U����ݒ�								*/
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* ���U����p�ϐ��f�[�^��`							*/
		LONG		Ivar64[2];						/* �ϕ����Z�p									*/
		LONG		SpdObs;							/* ���x����l									*/
		LONG		Hpftmp;							/* �n�C�p�X�t�B���^���Z�p						*/
		LONG		dSpdLpfo;						/* ���[�p�X�t�B���^�o�͑��x�U������				*/
		LONG		dSpdCmp;						/* ���U�⏞�l		[2^24/OvrSpd]				*/
		LONG		MonResVib;						/* ���U���j�^�M��	[2^24/OvrSpd]				*/
		LONG	    Hpftmp1;						/* <S03E> 3�������U����n�C�p�X�t�B���^���Z�p			*/
	} var;
} RESVIB;

/*--------------------------------------------------------------------------------------------------*/
/*		���U������̓p�����[�^	for ResVibCalculatePrm()											*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct RESVIB_CFG_PRM {
	USHORT	jrate;								/* Pn103 : �������[�����g��							*/
	USHORT	avibfrq;							/* Pn161 : �`�^���U���g��							*/
	USHORT	avibgn;								/* Pn162 : �`�^���U�Q�C���␳						*/
	USHORT	avibdamp;							/* Pn163 : �`�^���U�_���s���O�Q�C��					*/
	SHORT	avibfil1;							/* Pn164 : �`�^���U�t�B���^���萔�P�␳				*/
	SHORT	avibfil2;							/* Pn165 : �`�^���U�t�B���^���萔�Q�␳				*/
	USHORT	avibdamp2;							/* <S03E> �@ Pn166 : �@3 �������U�_���s���O�Q�C��			*/
	USHORT	avibfrq2;							/* <S03E> �@Pn167 :�@3 �������U���g��				*/
} RESVIB_CFG_PRM;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	ResVibATypeResVibInitialize( RESVIB *ResVib );
LONG	ResVibATypeResVibCtrl( RESVIB *ResVib, LONG MotSpd, LONG TrqRef );

BOOL	ResVibCalculatePrmSW( RESVIB *ResVib, ULONG avibsw, BOOL TuningLess, ASICS *SvAsicRegs );
void	ResVibCalculatePrm( RESVIB *ResVib, RESVIB_CFG_PRM *Prm, KSGAIN Kvx, LONG ScanTimeNs, ASICS *SvAsicRegs );

void	DetVibObsInitialize( DETVIB *DetVib );
void	DetVibSpeedObserver( DETVIB *DetVib, LONG SpdFbki, LONG TrqRefMon );
void	DetVibSetSpeedError( DETVIB *DetVib, BOOL TuningLess, BOOL TuningRun );
LONG	DetVibGetSpdErrPeak( DETVIB *DetVib );
LONG	DetVibGetVibCheckLevel( DETVIB *DetVib );
void	DetVibResetMaxMinSpeedError( DETVIB *DetVib );
void	DetVibResetMaxMinSpeedErrorFn( DETVIB *DetVib );
LONG	DetVibGetSpdErrPeakFn( DETVIB *DetVib );

void	DetVibInitialize( DETVIB *DetVib );
LONG	DetVibDetectVibration( DETVIB *DetVib );
void	DetVibResetRequest( DETVIB *DetVib );
LONG	DetVibDetectLowFreqVibration( DETVIB *DetVib );
void	DetVibLowFreqResetRequest( DETVIB *DetVib );

void	DetVibTraceInitialize( DETVIB *DetVib );
void	DetVibTraceRuntimeService( DETVIB *DetVib );
UCHAR	DetVibTraceGetStatus( DETVIB *DetVib );
SHORT*	DetVibTraceGetDataAdr( DETVIB *DetVib );
USHORT	DetVibTraceGetIndex( DETVIB *DetVib );
void	DetVibTraceStartReq( DETVIB *DetVib );
void	DetVibTraceStopReq( DETVIB *DetVib );
void	DetVibTraceSetCondition( DETVIB *DetVib, USHORT TraceNum, UCHAR TraceInt, SHORT TrigerLvl );

void	DetVibObsCalculateInitPrm( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs );
void	DetVibObsCalculateGains( DETVIBOBS *DetVibObs, LONG ObsGain, LONG Lpf, LONG Hpf, LONG ScanTimeNs );
void	DetVibObsCalculateJGain( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs );
void	DetVibObsCalculateInitPrmAAT( DETVIBOBS *DetVibObs, BOOL TuningLess, LONG ScanTimeNs );


#define _RES_VIB_CTRL_H /* �ēǂݍ��ݖh�~�p */
#endif /* _RES_VIB_CTRL_H */
/***************************************** end of file **********************************************/
