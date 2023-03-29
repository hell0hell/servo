#ifndef	_PCMD_FILTER_H /* �ēǂݍ��ݖh�~�p */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PcmdFilter.h : �ʒu�w�߃t�B���^���Z�p�w�b�_�[�t�@�C��										*/
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
#include "Mlib.h"


/****************************************************************************************************/
/*																									*/
/*		�萔��`																					*/
/*																									*/
/****************************************************************************************************/
#define MAFIL_BUFNUM			256						/* �ړ����σt�B���^�o�b�t�@��				*/
#define	SUMX_LIM				0x3FFFFFFF				/* ExpFil.V.Pexfvar.sumx�F�I�[�o�t���[臒l	*/
#define	MAXOMEGA2(AvfCycleNs)	(127323954/AvfCycleNs)	/* 127323954 = 10^9 * 0.8 / (2*PAI)			*/
														/*						���g��2�ő�l[Hz]	*/


/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�w���������t�B���^�p�p�����[�^�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _EXPFIL {					/* �w���������t�B���^�p�p�����[�^���ϐ���`				*/
	struct	{								/* �w���������t�B���^�p�p�����[�^						*/
		LONG	Kf[2];							/* �w���������t�B���^�Q�C��							*/
		LONG	Pbias[2];						/* �w���������t�B���^�o�C�A�X						*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* �w���������t�B���^�p�ϐ��f�[�^						*/
		LONG	Filo;							/* �t�B���^�o��										*/
		PEXFV	Pexfvar;						/* �w���������t�B���^���Z�p�ϐ��\����				*/
		BOOL	RefZStatus;						/* �����o���������									*/
	} var;
} EXPFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		�ړ����σt�B���^�p�p�����[�^�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _MAFIL {						/* �ړ����σt�B���^�p�p�����[�^���ϐ���`				*/
	struct	{								/* �ړ����σt�B���^�p�p�����[�^							*/
		LONG	PmafNum[2];						/* �ړ����ω�										*/
		UCHAR	ImafSft[2];						/* ��Ԉړ����ω�(2^Sft)							*/
		UCHAR	SpareByte[2];					/* �\��												*/
		/*----------------------------------------------------------------------------------------------*//*<S0C3>*/
		LONG	PmafNumSec[2];						/* �ړ�����2��										*/
		UCHAR	ImafSftSec[2];						/* ��Ԉړ�����2��(2^Sft)							*/
		UCHAR	SpareByteSec[2];					/* �\��2												*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* �ړ����σt�B���^�p�ϐ��f�[�^							*/
		LONG	PcmdInX;						/* �����ԕ�Ԉړ����σt�B���^���Z�p					*/
		LONG	PcmdInY;						/* �����ԕ�Ԉړ����σt�B���^���Z�p					*/
		LONG	Index;							/* ��Ԉړ����σt�B���^���Z�p						*/
		LONG	Filo;							/* �t�B���^�o��										*/
		BOOL	RefZStatus;						/* �����o���������									*/
		PMAFV	Pmafvar;						/* �ړ����σt�B���^���Z�p�ϐ��\����					*/
		PIMFV	Pimfvar;						/* ��Ԉړ����σt�B���^���Z�p�ϐ��\����				*/
		LONG	Pmafbuf[ MAFIL_BUFNUM ];		/* �ړ����σt�B���^���Z�p�o�b�t�@					*/
		/*----------------------------------------------------------------------------------------------*//*<S0C3>*/
		LONG	PcmdInXSec;						/* �����ԕ�Ԉړ����σt�B���^2���Z�p					*/
		LONG	PcmdInYSec;						/* �����ԕ�Ԉړ����σt�B���^2���Z�p					*/
		LONG	IndexSec;							/* ��Ԉړ����σt�B���^2���Z�p						*/
		LONG	FiloSec;							/* �t�B���^2�o��										*/
		HIGHFV	PmafvarSec;						/* �ړ����σt�B���^2���Z�p�ϐ��\����					*/
		HIMFV	PimfvarSec;						/* ��Ԉړ����σt�B���^2���Z�p�ϐ��\����				*/
		LONG	PmafbufSec[ MAFIL_BUFNUM ];		/* �ړ����σt�B���^2���Z�p�o�b�t�@					*/
	} var;
} MAFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu�w�߃t�B���^�p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _PCMDFIL {					/* �ʒu�w�߃t�B���^���Z�p�p�����[�^���ϐ���`			*/
	EXPFIL	ExpFil;								/* �������t�B���^�p�ϐ�								*/
	MAFIL	MaFil;								/* �ړ����σt�B���^�p�ϐ�							*/
} PCMDFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		�U���}���t�B���^�p�p�����[�^�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct VIBSUPFILPRM {					/* �U���}���t�B���^�p�p�����[�^��`					*/
	LONG			Kff1;							/* �U���}���t�B���^�Q�C��1						*/
	LONG			Kff2;							/* �U���}���t�B���^�Q�C��2						*/
	LONG			Kff3;							/* �U���}���t�B���^�Q�C��3						*/
	BOOL			enable;							/* �U���}���t�B���^�@�\�I��						*/
} VIBSUPFILPRM;

typedef	struct VIBSUPFIL {						/* �U���}���t�B���^�p�p�����[�^���ϐ���`			*/
	struct {									/* �U���}���t�B���^�p�p�����[�^��`					*/
		VIBSUPFILPRM	VibSupPrm;					/* �U���}���t�B���^�p�p�����[�^					*/
		VIBSUPFILPRM	Pexe;						/* ���s�p�U���}���t�B���^�p�p�����[�^			*/
		BOOL			VibSupSetting;				/* �U���}���t�B���^�ݒ�							*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* �U���}���t�B���^�p�ϐ��f�[�^��`					*/
		LONG			rem1;						/* �U���}���t�B���^���Z�p						*/
		LONG			rem2;						/* �U���}���t�B���^���Z�p						*/
		LONG			rem3;						/* �U���}���t�B���^���Z�p						*/
		LONG			wkbf1;						/* �U���}���t�B���^���Z�p						*/
		LONG			wkbf2;						/* �U���}���t�B���^���Z�p						*/
		LONG			FilioErr[2];				/* �U���}���t�B���^���o�͊ԕ΍�					*/
		LONG			Buf;						/* �U���}���t�B���^���܂�						*/
		LONG			Filo;						/* �U���}���t�B���^�o��							*/
	} var;
} VIBSUPFIL;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	PcmdFilInitialize( PCMDFIL *PcmdFil, VIBSUPFIL *VibSupFil );
void	FFCmdFilInitialize( MAFIL *pMaFil );									/* <S1D0> */
LONG	FFcmdFilMoveAverageFilter( MAFIL *pMaFil, LONG FFcmdIn, BOOL FilStop );	/* <S1D0> */
LONG	PcmdFilRuntimeService( PCMDFIL *PcmdFil, LONG PcmdIn, LONG FilRest, BOOL FilStop );
/*--------------------------------------------------------------------------------------------------*/
void	PcmdFilCalculatePrmExpFilter( EXPFIL *ExpFil, USHORT PrmTime, KSGAIN KMotSpdConv,
													 UCHAR MaxVel, LONG SvCycleUs, LONG Flag );
void	PcmdFilCalculatePrmExpFilBias( EXPFIL *ExpFil, USHORT expbias1,
												LONG Egear_a, LONG Egear_b, LONG SvCycleUs );
void	PcmdFilCalculatePrmMoveAvergeFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad,
		                                      LONG SvCycleUs, LONG Flag );/*<S0C3>*/
void    PcmdFilCalculatePrmHighFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad,
		                                      LONG SvCycleUs, LONG Flag );/*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
void	PcmdFilSetRefZStatus( PCMDFIL *PcmdFil, BOOL status );
BOOL	PcmdFilGetRefZStatus( PCMDFIL *PcmdFil );
/*--------------------------------------------------------------------------------------------------*/
LONG	PcmdFilVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal );
BOOL	PcmdFilCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw,
												ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs );
void	PcmdFilCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil, LONG ScanTimeNs );


#define _PCMD_FILTER_H /* �ēǂݍ��ݖh�~�p */
#endif /* _PCMD_FILTER_H */
/***************************************** end of file **********************************************/
