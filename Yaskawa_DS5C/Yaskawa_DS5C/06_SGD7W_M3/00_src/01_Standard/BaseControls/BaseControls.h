/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControls.h : Local Header File for ScanB Software										*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_BASE_CONTROLS_H_
#define	_BASE_CONTROLS_H_


#include "KLib.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "EncData.h"
#include "JL086ServoIpHw.h"
#include "BaseCmnStruct.h"
#include "CtrlLoopHandler.h"
#include "GselHandler.h"
#include "MfcHandler.h"
#include "PcmdFilter.h" /* <S065> */
#include "ResVibCtrl.h"
#include "TableDrive.h"
#include "PnPrmStruct.h"
#include "Bprm.h"
#include "MicroPrmCalc.h"
#include "ProgramJog.h"
#include "JatOffLine.h"
#include "CtrlPrmTransducer.h"
#include "Pls.h"
#include "MotorLessTestStr.h"	/* <S1B7> */

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Bit No. Definition for LatchSel (ULONG LatchSel)											*/
/*--------------------------------------------------------------------------------------------------*/
#define	C_PHASE_LATCH		0x01

/* <S14B> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		���x���b�v���⏞�@�\�̗L�����������l����													*/
/*--------------------------------------------------------------------------------------------------*/
#define	CMD_SPD		0x00
#define	FB_SPD		0x01
/* <S14B> End */

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		�V�������X�p�p�����[�^�ϐ���` <S1A5> 														*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _TL2PRM {
	LONG		KrlpfResSup;		/* �@�B���U�}���t�B���^�Q�C��									*/
	LONG		KrlpfDisObs1[5];	/* �O���I�u�U�[�o2���t�B���^�Q�C��(�o���h�p�X�t�B���^�p)		*/
	LONG		KrlpfDisObs2[5];	/* �O���I�u�U�[�o2���t�B���^�Q�C��(�n�C�p�X�t�B���^�p)			*/
	LONG		KrCurfreq;			/* �d�����[�v�ߎ����f�����g��									*/
	LONG		KrCurZeta1;			/* �d�����[�v�ߎ����f�������W��1								*/
	LONG		KrCurZeta2;			/* �d�����[�v�ߎ����f�������W��2								*/
	KSGAIN		KrlpfCur[5];		/* �d�����[�v�ߎ����f��2���t�B���^�Q�C��						*/
} TL2PRM;

/*--------------------------------------------------------------------------------------------------*/
/*		�������X�p�p�����[�^�ϐ���`																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_PRM {
	USHORT		kv;				/* ���x���[�v�Q�C��									[0.1Hz]			*/
	USHORT		kv2;			/* �␳�㑬�x���[�v�Q�C��							[0.1Hz]			*/
	USHORT		jrate;			/* ���׊������[�����g/���ʔ�						[%]				*/
	USHORT		Ti;				/* ���x���[�v�ϕ����萔								[0.01ms]		*/
	USHORT		kp;				/* �ʒu���[�v�Q�C��									[0.1/s]			*/
	USHORT		klpf;			/* �g���N�⏞��g���N�w�߃t�B���^���萔				[0.01ms]		*/
	USHORT		f1;				/* �O���I�u�U�[�o���[�p�X�t�B���^1���g��			[0.1Hz]			*/
	USHORT		f1Comp;			/* �␳��O���I�u�U�[�o���[�p�X�t�B���^1���g��		[0.1Hz]			*/
	USHORT		f2;				/* �O���I�u�U�[�o���[�p�X�t�B���^2���g��			[0.1Hz]			*/
	USHORT		kj_dist;		/* �O���I�u�U�[�o�C�i�[�V���Q�C��					[%]				*/
	USHORT		kj_dist2;		/* �␳��O���I�u�U�[�o�C�i�[�V���Q�C��				[%]				*/
	USHORT		kd_dist;		/* �O���I�u�U�[�o�W��								[%]				*/
	USHORT		ks_vobs;		/* �ʑ��⏞���x�I�u�U�[�o�Q�C��						[Hz]			*/
	USHORT		kj_vobs;		/* �ʑ��⏞���x�I�u�U�[�o�C�i�[�V���Q�C��			[%]				*/
	USHORT		kj_vobs2;		/* �␳��ʑ��⏞���x�I�u�U�[�o�C�i�[�V���Q�C��		[%]				*/
	USHORT		vobsLpf;		/* �ʑ��⏞���x�I�u�U�[�oLPF���萔					[0.01ms]		*/
	USHORT		Stiff;			/* ���o�X�g�����ᗦ									[%]				*/
	USHORT		F1rate;			/* �O���I�u�U�[�o�t�B���^���g��1�ጸ��				[%]				*/
	LONG		vffPhCmpFrq1;	/* �ʑ��⏞�t�B���^���g��1							[0.1Hz]			*/
	LONG		vffPhCmpFrq2;	/* �ʑ��⏞�t�B���^���g��2							[0.1Hz]			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0							/* <S1A5> */
	LONG		KrlpfResSup;	/* �V�������X �@�B���U�}���t�B���^�Q�C��							*/
	LONG		KrlpfDisObs1[5];/* �V�������X �O���I�u�U�[�o2���t�B���^�Q�C��(�o���h�p�X�t�B���^�p)	*/
	LONG		KrlpfDisObs2[5];/* �V�������X �O���I�u�U�[�o2���t�B���^�Q�C��(�n�C�p�X�t�B���^�p)	*/
	LONG		KrCurfreq;		/* �V�������X �d�����[�v�ߎ����f�����g��							*/
	LONG		KrCurZeta1;		/* �V�������X �d�����[�v�ߎ����f�������W��1							*/
	LONG		KrCurZeta2;		/* �V�������X �d�����[�v�ߎ����f�������W��2							*/
	KSGAIN		KrlpfCur[5];	/* �V�������X �d�����[�v�ߎ����f��2���t�B���^�Q�C��					*//*<S00A>*/
#endif
	TL2PRM		Tl2Prm;			/* <S1A5> 															*/
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*		�������X�p���o�X�g�⏞�퉉�Z�p�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TUNELESS_DRCOM {				/* ���o�X�g�⏞��p�p�����[�^��`						*/
	struct {								/* �������X�p���W�X�^��`								*/
#if (FLOAT_USE==TRUE)
		float		KVfbFil;					/* �������X�p���x�e�a�t�B���^�Q�C��					*/
		float		KCmpOutlpf;					/* ���o�X�g�⏞��g���N�⏞���[�p�X�t�B���^�Q�C��	*/
		float		Krlpf1;						/* ���o�X�g�⏞�탍�[�p�X�t�B���^�Q�C��1			*/
		float		Krlpf2;						/* ���o�X�g�⏞�탍�[�p�X�t�B���^�Q�C��2			*/
		float		Krd;						/* ���o�X�g�⏞�g���N�W��							*/
		float		Krsj;						/* ���o�X�g�⏞��g���N�␳�Q�C��					*/
#else
		LONG		KVfbFil;					/* �������X�p���x�e�a�t�B���^�Q�C��					*/
		LONG		KCmpOutlpf;					/* ���o�X�g�⏞��g���N�⏞���[�p�X�t�B���^�Q�C��	*/
		LONG		Krlpf1;						/* ���o�X�g�⏞�탍�[�p�X�t�B���^�Q�C��1			*/
		LONG		Krlpf2;						/* ���o�X�g�⏞�탍�[�p�X�t�B���^�Q�C��2			*/
		LONG		Krd;						/* ���o�X�g�⏞�g���N�W��							*/
		LONG		Krsj;						/* ���o�X�g�⏞��g���N�␳�Q�C��					*/
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	TL2PRM	*pTl2;						/* Tuningless Parameter <S1A5> 								*/
	/*----------------------------------------------------------------------------------------------*/
	void	(*RobustCompensator)		/* Tuneingless Robust Compensator <S1A5> 					*/
			( void *pdrcmp, 			/* 															*/
			  LONG motspd, 				/* 															*/
			  LONG trqref );			/* 															*/
	/*----------------------------------------------------------------------------------------------*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �������X�p���W�X�^��`								*/
		LONG		DisTrq;						/* �O���g���N����l									*/
		LONG		DisTrqCmp;					/* �O���g���N�⏞�l			[2^24/MaxTrq]			*/
		LONG		TrqLpfo;					/* ���o�X�g�⏞�탍�[�p�X�t�B���^�o�͒l				*/
		LONG		TrqLpfo_old;				/* ���o�X�g�⏞��O�񃍁[�p�X�t�B���^�o�͒l			*/
		LONG		VelLpfo1;					/* ���o�X�g�⏞�퍷�����x���[�p�X�t�B���^�o�͒l1	*/
		LONG		VelLpfo2;					/* ���o�X�g�⏞�퍷�����x���[�p�X�t�B���^�o�͒l2	*/
		BOOL		TrqFilClrReq;				/* �g���N�w�߃t�B���^�ϕ��������v��					*/
		BOOL		CurrentStatus;				/* �L��/�����؂�ւ�����p���ݏ�ԃt���O			*/
		BOOL		LastStatus;					/* �L��/�����؂�ւ�����p�O���ԃt���O			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG		Krlpftmp1[4];				/* <S1A5> 2��LPF�t�B���^�������Z�p�ϐ�				*/
		LONG		Krlpftmp2[4];				/* <S1A5> 2��LPF�t�B���^�������Z�p�ϐ�				*/
		LONG		Krlpftmp3[4];				/* <S1A5> 2��LPF�t�B���^�������Z�p�ϐ�				*/
	/*----------------------------------------------------------------------------------------------*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_DRCOMP;

/*--------------------------------------------------------------------------------------------------*/
/*		�������X�p�ʑ��⏞���x�I�u�U�[�o�ϐ���`													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TUNELESS_EHVOBS {			/* �ʑ��⏞���x�I�u�U�[�o�p�p�����[�^��`				*/
	struct {								/* �������X�p���W�X�^��`								*/
#if (FLOAT_USE==TRUE)
		float		Kstf1;						/* �ʑ��⏞VOBS�Q�C��1								*/
		float		Kstf2;						/* �ʑ��⏞VOBS�Q�C��2								*/
		float		Kstf3;						/* �ʑ��⏞VOBS�Q�C��3								*/
		float		Kj;							/* �ʑ��⏞VOBS�ʑ��⏞�Q�C��						*/
		float		Lpf;						/* �ʑ��⏞VOBS���[�p�X�t�B���^�Q�C�� (LPF + Pn401�j*/
		float		Tf;							/* �ʑ��⏞VOBS���[�p�X�t�B���^���萔				*/
#else
		LONG		Kstf1;						/* �ʑ��⏞VOBS�Q�C��1								*/
		LONG		Kstf2;						/* �ʑ��⏞VOBS�Q�C��2								*/
		LONG		Kstf3;						/* �ʑ��⏞VOBS�Q�C��3								*/
		LONG		Kj;							/* �ʑ��⏞VOBS�ʑ��⏞�Q�C��						*/
		LONG		Lpf;						/* �ʑ��⏞VOBS���[�p�X�t�B���^�Q�C�� (LPF + Pn401�j*/
		LONG		Tf;							/* �ʑ��⏞VOBS���[�p�X�t�B���^���萔				*/
#endif /* FLOAT_USE */
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �������X�p���W�X�^��`								*/
		KSGAIN		Ivar64[2];					/* �ϕ����Z�p										*//*<S00A>*/
		LONG		EhSpdObs;					/* ���x����l										*/
		LONG		Wkbf1;						/* ���[�p�X�t�B���^�o�� 							*/
		LONG		Wkbf2;						/* �ʑ��⏞VOBS���Z�p���[�N�ϐ�						*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_EHVOBS;

/*--------------------------------------------------------------------------------------------------*/
/*		�������X����p�\���̒�`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_CTRL {
	TUNELESS_PRM		TuneLessPrm;		/* �������X�p�Q�C���p�����[�^�\����						*/
	TUNELESS_DRCOMP		Drcomp;				/* �������X�p���o�X�g�⏞���Z�ϐ�						*/
	TUNELESS_EHVOBS		EhVobs;				/* �������X�p�ʑ��⏞���xOBS���Z�ϐ�					*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �������X�p���W�X�^��`								*/
		USHORT			DatGnLevel;				/* 0x20E0 : DAT�Q�C�����׃�							*/
		USHORT			DatJratLevel;			/* 0x20E1 : DAT���׃��׃�							*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �������X�p�p�����[�^��`								*/
		BOOL			TuningLessUse;			/* �������X�g�p/�s�g�p�ݒ�							*/
		BOOL			TuningLessEx;			/* �V�������X�ݒ�									*/
		BOOL			TuningLessVComp;		/* �������X�ʑ��i�ݕ⏞�ݒ�							*/
		LONG			TuningLessType;			/* �������X�����^�C�v								*/
		LONG			TuningLessMode;			/* �������X���[�h									*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �������X�p�ϐ���`									*/
		LONG			SpdFbFilo;				/* ���xFB�t�B���^�o��								*/
		BOOL			TuneLessAct;			/* �������X�v��										*/
		BOOL			TuneLessVCompReq;		/* �������X�ʑ��i�ݕ⏞�v��							*/
		BOOL			TuneLessInvldReq;		/* �������X���������v��								*/
		BOOL			TuningFuncRun;			/* Fn200���s���t���O								*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_CTRL;



/*--------------------------------------------------------------------------------------------------*/
/*		�O���I�u�U�[�o�p�ϐ���`									<V221>							*/
/*--------------------------------------------------------------------------------------------------*/
#if 0		/* <S09B> */
typedef	struct DOBS_PRM {					/* �O���I�u�U�[�o�p�p�����[�^��`						*/
	KSGAIN				Ks;						/* �O���I�u�U�[�o�Q�C��								*/
	KSGAIN				Kj;						/* �O���I�u�U�[�o�C�i�[�V���Q�C��					*/
	KSGAIN				Ksj;					/* �O���I�u�U�[�o�g���N�Q�C��						*/
	KSGAIN				Kd;						/* �O���g���N�W��									*/
	KSGAIN				Klpf;					/* �O���I�u�U�[�o���[�p�X�t�B���^�Q�C��				*/
} DOBS_PRM;
#else
typedef GSELDOBS DOBS_PRM;
#endif

typedef	struct DOBS_CTRL {					/* �O���I�u�U�[�o�p�p�����[�^���ϐ���`					*/
//<S09B>	DOBS_PRM			DobsPrm[3];				/* �O���I�u�U�[�o�p�p�����[�^						*/
	DOBS_PRM			*pDobsPrm;				/* �O���I�u�U�[�o�p�p�����[�^ <S09B>				*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �O���I�u�U�[�o�p�ϐ��f�[�^��`						*/
		BOOL			DobsAct;				/* �O���I�u�U�[�o���s�v��							*/
		KSGAIN			Ivar64[2];				/* �ϕ����Z�p										*//*<S00A>*/
		LONG			SpdObs;					/* ���x����l										*/
		LONG			DisTrq;					/* �O���g���N����l									*/
		LONG			Hpftmp;					/* �n�C�p�X�t�B���^���Z�p							*/
		LONG			DTrqLpfo;				/* ���[�p�X�t�B���^�o�͊O���g���N����l				*/
		LONG			DisTrqCmp;				/* �O���g���N�⏞�l			[2^24/MaxTrq]			*/
		UCHAR			CurrentStatus;			/* �L��/�����؂�ւ�����p���ݏ�ԃt���O			*/
		UCHAR			LastStatus;				/* �L��/�����؂�ւ�����p�O���ԃt���O			*/
		UCHAR			ExchangeSts;			/* �@�\�L���E�����؂�ւ����(�V���b�N�y���p)		*/
		UCHAR			Dummy;					/* for Alignment									*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} DOBS_CTRL;



/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��⏞���x�I�u�U�[�o(VOBS)�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct EHVOBS_PRM {					/* �ʑ��⏞���x�I�u�U�[�o�p�p�����[�^��`				*/
#if (FLOAT_USE==TRUE)
	float				Kstf1;					/* �ʑ��⏞VOBS�Q�C��1(Pn127)						*/
	float				Kstf2;					/* �ʑ��⏞VOBS�Q�C��2(Pn127)						*/
	float				Kstf3;					/* �ʑ��⏞VOBS�Q�C��3(Pn127)						*/
	float				Kj;						/* �ʑ��⏞VOBS�ʑ��⏞�Q�C���iPn128�j				*/
	float				Lpf;					/* �ʑ��⏞VOBS���[�p�X�t�B���^�Q�C��(Pn13B+Pn401)	*/
	float				Tf;						/* �ʑ��⏞VOBS���[�p�X�t�B���^���萔				*/
	float				KTrqReflpf;				/* VOBS�g���N�w�߃��[�p�X�t�B���^�Q�C��				*/
#else
	
	LONG				Kstf1;					/* �ʑ��⏞VOBS�Q�C��1(Pn127)						*/
	LONG				Kstf2;					/* �ʑ��⏞VOBS�Q�C��2(Pn127)						*/
	LONG				Kstf3;					/* �ʑ��⏞VOBS�Q�C��3(Pn127)						*/
	LONG				Kj;						/* �ʑ��⏞VOBS�ʑ��⏞�Q�C���iPn128�j				*/
	LONG				Lpf;					/* �ʑ��⏞VOBS���[�p�X�t�B���^�Q�C��(Pn13B+Pn401)	*/
	LONG				Tf;						/* �ʑ��⏞VOBS���[�p�X�t�B���^���萔				*/
	LONG				KTrqReflpf;				/* VOBS�g���N�w�߃��[�p�X�t�B���^�Q�C��				*/
#endif /* FLOAT_USE */
} EHVOBS_PRM;

typedef	struct EHVOBS_CTRL {				/* �ʑ��⏞���x�I�u�U�[�o�p�p�����[�^���ϐ���`			*/
	EHVOBS_PRM			EhVobsPrm;				/* �ʑ��⏞���x�I�u�U�[�o�p�p�����[�^				*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �O���I�u�U�[�o�p�ϐ��f�[�^��`						*/
		BOOL			EhVobsUse;				/* �ʑ��⏞���x�I�u�U�[�o�L��/�����I��				*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* �ʑ��⏞���x�I�u�U�[�o�p�ϐ��f�[�^��`				*/
		KSGAIN			Ivar64[2];				/* �ϕ����Z�p										*//*<S00A>*/
		LONG			AccSum;					/* �����x�ώZ�l(���x����l)							*/
		LONG			EhSpdObs;				/* ���x����l										*/
		LONG			Wkbf1;					/* ���[�p�X�t�B���^�o�� 							*/
		LONG			Wkbf2;					/* �ʑ��⏞VOBS���Z�p���[�N�ϐ�						*/
		LONG			TrqRefLpfo;				/* VOBS�g���N�w�߃t�B���^����l						*/
	} var;
} EHVOBS_CTRL;

/* <S14B> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		���x���b�v���⏞�p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	RPLCMP_ORDER_MECHA	12				/* ���x���b�v���⏞�ő原��(�@�B�p)					*/
#define	RPLCMP_ORDER_ELE	6				/* ���x���b�v���⏞�ő原��(�d�C�p)					*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct RPLCMP_PRM_MECHA {		/* ���x���b�v���⏞�p�����[�^��`(�@�B�p)				*/
	LONG	CompGain;							/* ���x���b�v���⏞�Q�C��			[%]				*/
	USHORT	CompFrq[RPLCMP_ORDER_MECHA];		/* ���x���b�v���⏞����				[-]				*/
	LONG	CompAmpValue[RPLCMP_ORDER_MECHA];	/* ���x���b�v���⏞�U��				[15000/MaxTrq]	*/
	USHORT	PhaseOfset[RPLCMP_ORDER_MECHA];		/* ���x���b�v���⏞�ʑ�				[65536/360deg]	*/
}RPLCMP_PRM_MECHA;

typedef	struct RPLCMP_PRM_ELE {			/* ���x���b�v���⏞�p�����[�^��`(�d�C�p)				*/
	LONG	CompGain;							/* ���x���b�v���⏞�Q�C��			[%]				*/
	USHORT	CompFrq[RPLCMP_ORDER_ELE];			/* ���x���b�v���⏞����				[-]				*/
	LONG	CompAmpValue[RPLCMP_ORDER_ELE];		/* ���x���b�v���⏞�U��				[15000/MaxTrq]	*/
	USHORT	PhaseOfset[RPLCMP_ORDER_ELE];		/* ���x���b�v���⏞�ʑ�				[65536/360deg]	*/
}RPLCMP_PRM_ELE;

typedef	struct RPLCMP_PRM {				/* ���x���b�v���⏞�p�����[�^��`						*/
	RPLCMP_PRM_MECHA	RplCmpPrmMecha;				/* ���x���b�v���⏞�p�����[�^�i�@�B�p�j			*/
	RPLCMP_PRM_ELE		RplCmpPrmEle;				/* ���x���b�v���⏞�p�����[�^�i�d�C�p�j			*/
	/*----------------------------------------------------------------------------------------------*/
	BOOL	RplCmpUse;								/* ���x���b�v���⏞�g�p/�s�g�p�ݒ�				*/
	BOOL	RippleWrnMode;							/* ���x���b�v���⏞���s��v���[�j���O���o���[�h(0:���o����,1:���o���Ȃ�)*//* <S1FD> */
	BOOL	RippleLstWrnMode;						/* ���x���b�v���⏞���s��v���[�j���O���o���[�h�O��l(200Dh�p)*//* <S1FD> */
//	BOOL	RplCmpMode6f;							/* ���x���b�v���⏞���[�h�I��(1:1f�`6f/0:1f�`3f)*/
//	BOOL	RplCmpDobsMon;							/* ���j�^�o�͑I��(�O��OBS�Ƌ��p)				*/
//	USHORT	RplSpAnalyzeMode;						/* ���x���b�v���X�y�N�g����̓��[�h�I��			*/
	USHORT	RplCompGainEle;							/* ���x���b�v���⏞�Q�C��(�d�C�p)				*/	/* <S14B> */
	USHORT	RplCompCycleEle[RPLCMP_ORDER_ELE];		/* ���x���b�v���⏞��������(�d�C�p)				*/	/* <S14B> */
	SHORT	RplCompAmplitudeEle[RPLCMP_ORDER_ELE];	/* ���x���b�v���⏞�����U��(�d�C�p)				*/	/* <S14B> */
	USHORT	RplCompPhaseEle[RPLCMP_ORDER_ELE];		/* ���x���b�v���⏞�����ʑ�(�d�C�p)				*/	/* <S14B> */
} RPLCMP_PRM;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {						/* ���x���b�v���⏞�p�p�����[�^���ϐ���`				*/
	RPLCMP_PRM			RplCmpPrm;					/* ���x���b�v���⏞�p�p�����[�^					*/
/*--------------------------------------------------------------------------------------------------*/
	struct {										/* ���x���b�v���⏞�p�ϐ��f�[�^��`				*/
		LONG			TrqCompAll;					/* ���x���b�v���⏞�g���N		   [2^24/MaxTrq]*/
		BOOL			RippleWrnState;				/* ���x���b�v���⏞��񃏁[�j���O���(0:A.942����������,1:����)	*/	/* <S1FD> */
		BOOL			RippleCmpExe;				/* ���x���b�v���⏞�@�\�̗L�����������l�����m�F */
//		LONG			TrqComp[RPLCMP_ORDER];		/* ���x���b�v���⏞�g���N(��������)[2^24/MaxTrq]*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RippleCmpState;			/* ���x���b�v���⏞��񏑍��݊m�F(00H:����,01H:������,02H:���s)	*/
	BOOL	VelRipplePhaseReady;	/* ���x���b�v���⏞�p ���Ɍ��o�����t���O						*/
	LONG	RippleTrqCompMon;		/* ���x���b�v���␳�g���N(���j�^�p)	[2^24/MaxTrq]				*/
	BOOL	RippleCompMon;			/* ���x���b�v���⏞���M��(Un110 bit23:0=�␳����,1=�␳��)		*/
	USHORT	RippleCmpThresholdSel;	/* ���x���b�v���⏞�@�\�̗L�����������l�����I��					*/
	LONG	TrqRefBeforeComp;		/* �␳�O�g���N�w��(Trace�EAnaMon�p)[2^24/MaxTrq]				*/
	LONG	VelRippleCompSpd;		/* ���x���b�v���⏞�L�����x[min-1]								*/
} RPLCMP;
/* <S14B> End */

/*--------------------------------------------------------------------------------------------------*/
/*		���䃂�[�h�ؑ֒�`																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CTRL_CHANGE {
	LONG	dPosFbki;				/* �ʒu�����l(�O�񉞓�������̑����l)	[pulse]					*/
	LONG	CmdErr;					/* �ʒu�΍��␳�l [�w�ߒP��]									*/
	LONG	SpdFbki_l;				/* ���[�^���x(�O��l)					[2^24/OvrSpd]			*/
	LONG	SpdFbki_a;				/* ���[�^���x(�O��ƍ���̕��ϒl)		[2^24/OvrSpd]			*/
} CTRL_CHANGE;


/*--------------------------------------------------------------------------------------------------*/
/*		ScanB�p�m�[�h���ʕϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct SERVO_CONTROL_IF {
	SV_OPTION_IF	CmdOptionIf;			/* SV-OP I/F�n���h���ւ̃|�C���^						*/
	BOOL			CmdReady;				/* �w�ߎ�t�\�t���O */

	CMD_CTRLBITS	CmdCtrl;				/* ����w��												*/
	LONG			CmdSeqBit;				/* �V�[�P���X����w��									*/
	LONG			CmdLtBit;				/* ���b�`�v��											*/
	RSP_CTRLBITS	RspCtrl;				/* ������ */
	LONG			RspSigBit;				/* �V�[�P���X�M��										*/
	LONG			RspLtBit;				/* ���b�`���											*/

	BOOL			LtReqCphs;
	BOOL			LtReqExt1;
	BOOL			LtReqExt2;
	BOOL			LtReqExt3;
	BOOL			ExtLtEnable[3];
	SHAL_LATCH_STS	ExtLtSts[3];

	LONG			MonCphaLpos[2];			/* C�����b�`�ʒu	[�w�ߒP��]							*/
	LONG			MonExt1Lpos[2];			/* EXT1���b�`�ʒu	[�w�ߒP��]							*/
	LONG			MonExt2Lpos[2];			/* EXT2���b�`�ʒu	[�w�ߒP��]							*/
	LONG			MonExt3Lpos[2];			/* EXT3���b�`�ʒu	[�w�ߒP��]							*/

//	CTRL_CHANGE		CtrlChange;				/* ����ؑ֕ϐ�											*/
//	LONG			VirtualPositionError[2];/* ���z�ʒu�΍�		[�w�ߒP��]	(�ʐM�����^�C�~���O)	*/
//	LONG			FbPositionFB[2];		/* FB�ʒu			[FB�P��]	(�ʐM�����^�C�~���O)	*/

	ULONG			NetSout;				/* �ʐMI/F�o�͐M���w��									*/
	LONG			NetCtrlMcmd;			/* ���䃂�[�h(�ʐM����̎w��)							*/
	LONG			NetTrqFFC;				/* �g���NFF�w��		[2^24/MaxTrq]						*/
	LONG			NetTrqRef;				/* �g���N�w��		[2^24/MaxTrq]						*/
	LONG			NetSpdLmt;				/* ���x����			[2^24/OvrSpd]						*/
	LONG			NetSpdRef;				/* ���x�w��			[2^24/OvrSpd]						*/
	LONG			NetSpdFFC;				/* ���x�e�e�w��		[2^24/OvrSpd]						*/
	LONG			NetPosRef;				/* �ʒu�w��			[�w�ߒP��]							*/

	ULONG			NetIfMonInSignal1;		/* SI�|�[�getc���͐M����� for Network	�P���A���v		*//* <S0AC> *//* <S1C0>:rename */
	ULONG			NetIfMonInSignal2;		/* SI�|�[�getc���͐M����� for Network	�����A���v		*//* <S1C0> */

	LONG			*MonRamTable[0x50];	/* �g�����j�^�p�ϐ��i�[�e�[�u��					*//* <S1D7>�F�v�f���ύX */
	struct {								/* �g�����j�^�I��										*/
		LONG		MonSel1;				/* �g�����j�^�I��1										*/
		LONG		MonSel2;				/* �g�����j�^�I��2										*/
		LONG		MonSel3;				/* �g�����j�^�I��3										*/
		LONG		MonSel4;				/* �g�����j�^�I��4										*/
		LONG		MonSel5;				/* �g�����j�^�I��5										*/
		LONG		MonSel6;				/* �g�����j�^�I��6										*/
		LONG		MonSel7;				/* �g�����j�^�I��7										*/
		LONG		MonSel8;				/* �g�����j�^�I��8										*/
	} MonSel;
} SERVO_CONTROL_IF;


typedef	struct SETTLINGTIME {			/* ���莞�Ԋ֘A�����p�ϐ���`								*/
	BOOL	StlgCalcReq;					/* ���莞�ԎZ�o���t���O									*/
	BOOL	StlgLastCoinSts;				/* COIN�M���O��l										*/
	BOOL	StlgCoinLatch;					/* COIN���b�`���										*/
	ULONG	ProcessTimer;					/* ���莞�ԑ��菈�����s�^�C�~���O���o�p�^�C�}			*/
	LONG	StlgLastdPcmd;					/* �����ʒu�w�ߑO��l									*/
	LONG	StlgTimeCntr;					/* ���莞�Ԍv���p�J�E���^								*/
	LONG	TmpStlgTime;					/* ���莞�ԃJ�E���^�o�b�t�@								*/
	LONG	RsltStlgTime;					/* ���莞�ԉ��Z����										*/
	LONG	OverErrDir;						/* �I�[�o�[�V���[�g����΍��̕���						*/
	LONG	OverShoot;						/* �I�[�o�[�V���[�g�ʑ���p			[�w�ߒP��]			*/
	LONG	UnitChngGain;					/* �P�ʕϊ��p�Q�C��[cyc]��[1us]							*/
	LONG	UnMaxOverShoot;					/* Un106 : �ő�I�[�o�[�V���[�g��	[�w�ߒP��]			*/
	LONG	UnCoinOffTimer;					/* Un10B : �ʒu���ߊ������s���ԐώZ	[ms]				*/
	LONG	OvsErrDetLvl;					/* ���e�I�[�o�[�V���[�g��			[�w�ߒP��]			*/
	LONG	LastdPcmda;						/* �O��ʒu�w�ߑ����l[�w�ߒP��]							*/
} SETTLINGTIME;


typedef	struct UV_TLIM_DATA {
	struct {							/* �s���d�����[�j���O���g���N�����p�����[�^��`				*/
		BOOL			UvTrqLmtCtrl;		/* �s���d�����[�j���O���g���N��������I��				*/
		LONG			UvTLimSetting;		/* ���H�d���~�����g���N�����l			[2^24/MaxTrq]	*/
		LONG			DeltaUvTrqLmt;		/* ���H�d���~�����g���N�������A�g���N	[2^24/MaxTrq]	*/
		LONG			UvTrqLmtOffFil;		/* ���H�d���~�����g���N������������	[ScanB��]		*/
		SHORT			Dummy;				/* for Alignment										*/
	} conf;
	struct {							/* �s���d�����[�j���O���g���N�����ϐ���`					*/
		LONG			UvTrqLmt;			/* �s���d�����̃g���N����				[2^24/MaxTrq]	*/
		ULONG			UvTrqLmtWait;		/* �s���d�����̃g���N���������E�F�C�g�J�E���^			*/
	} var;
} UV_TLIM_DATA;


typedef	struct TRQ_LMT_DATA {
	struct {							/* �g���N�����p�����[�^��`									*/
		BOOL			TrefTrqLmt;			/* �g���N�w�߂��O���g���N�����Ƃ��Ďg�p					*/
		BOOL			TrefTrqLmtCL;		/* �g���N�w�߂��O���g���N�����Ƃ��Ďg�p(P-CL,N-CL�L����)*/
#if (FLOAT_USE==TRUE)
		float			EstopTrqLmt;		/* ����~�g���N					[2^24/MaxTrq]		*/
		float			FwIntTrqLmt;		/* ���]�����g���N����				[2^24/MaxTrq]		*/
		float			RvIntTrqLmt;		/* �t�]�����g���N����				[2^24/MaxTrq]		*/
		float			FwExtTrqLmt;		/* ���]�O���g���N����				[2^24/MaxTrq]		*/
		float			RvExtTrqLmt;		/* �t�]�O���g���N����				[2^24/MaxTrq]		*/
#else
		LONG			EstopTrqLmt;		/* ����~�g���N					[2^24/MaxTrq]		*/
		LONG			FwIntTrqLmt;		/* ���]�����g���N����				[2^24/MaxTrq]		*/
		LONG			RvIntTrqLmt;		/* �t�]�����g���N����				[2^24/MaxTrq]		*/
		LONG			FwExtTrqLmt;		/* ���]�O���g���N����				[2^24/MaxTrq]		*/
		LONG			RvExtTrqLmt;		/* �t�]�O���g���N����				[2^24/MaxTrq]		*/
#endif /* FLOAT_USE */
	} conf;
	struct {							/* �g���N�����ϐ���`										*/
		BOOL			PclSignal;			/* PCL Signal											*/
		BOOL			NclSignal;			/* NCL Signal											*/
#if (FLOAT_USE==TRUE)
		float			FwTrqLmt;			/* Forward Torque Limit(����)		[2^24/MaxTrq]		*/
		float			RvTrqLmt;			/* Reverse Torque Limit(����)		[2^24/MaxTrq]		*/
//		float			PosTrqLmt;			/* Positive Torque Limit(Network)	[2^24/MaxTrq]		*/
//		float			NegTrqLmt;			/* Negative Torque Limit(Network)	[2^24/MaxTrq]		*/
		float			PosTrqLmtOut;		/* Positive Torque Limit(�o��)		[2^24/MaxTrq]		*/
		float			NegTrqLmtOut;		/* Negative Torque Limit(�o��)		[2^24/MaxTrq]		*/
#else
		LONG			FwTrqLmt;			/* Forward Torque Limit(����)		[2^24/MaxTrq]		*/
		LONG			RvTrqLmt;			/* Reverse Torque Limit(����)		[2^24/MaxTrq]		*/
//		LONG			PosTrqLmt;			/* Positive Torque Limit(Network)	[2^24/MaxTrq]		*/
//		LONG			NegTrqLmt;			/* Negative Torque Limit(Network)	[2^24/MaxTrq]		*/
		LONG			PosTrqLmtOut;		/* Positive Torque Limit(�o��)		[2^24/MaxTrq]		*/
		LONG			NegTrqLmtOut;		/* Negative Torque Limit(�o��)		[2^24/MaxTrq]		*/
#endif /* FLOAT_USE */
	} var;
	UV_TLIM_DATA		UvTrqLimits;		/* �s���d�����[�j���O���g���N�����p�f�[�^				*/
} TRQ_LMT_DATA;


typedef	struct BASE_MOT_STS {
//	BOOL				MotCposOk;			/* C���ʉߊ����t���O									*/
	BOOL				MotCphPass;			/* C���ʉ߃t���O(1scan)									*/
	LONG				MposFromCpos;		/* CPOS����̃��[�^�ʒu		 [+/-(PulseNo/2)],[LM:LONG]	*/
	LONG				MotSpd;				/* ���[�^���x							[2^24/OvrSpd]	*/
	LONG				FencSpd;			/* ���[�^���x(�ٸ)						[2^24/OvrSpd]	*//* <S04D> */
	LONG				MotPos;				/* ���[�^�ʒu(ScanB�ώZ�l)				[32bitRingPulse]*/
	LONG				dMotPos;			/* Delta Motor Position					[pulse/ScanB]	*/
	BOOL				FencCphPass;		/* C���ʉ߃t���O(1scan)(�ٸ)							*//* <S04D> */
	LONG				FposFromCpos;		/* CPOS����̃��[�^�ʒu(�ٸ) [+/-(PulseNo/2)],[LM:LONG]	*//* <S04D> */
	LONG				FencPos;			/* ���[�^�ʒu(ScanB�ώZ�l)(�ٸ)			[32bitRingPulse]*//* <S04D> */
	LONG				dFencPos;			/* Delta Motor Position	(�ٸ)			[pulse/ScanB]	*//* <S04D> */
//	LONG				MotPosB;			/* ScanB�擾���[�^�ʒu(PG�f�[�^)		[pulse]			*//* <S04D>�F���g�p	*/
	LONG				MotSpdSumB;			/* Motor Speed Sum(ScanB����)							*/
	LONG				MotSpdCntB;			/* Motor Speed Cnt(ScanB����)							*/
//	LONG				MotSpdForOS;		/* Motor Speed OS�ُ팟�o�p				[2^24/OvrSpd]	*//* <S04D>�F���g�p	*/
	LONG				MotSpdForSeq;		/* Motor Speed �V�[�P���X�p(�⏞�Ȃ�)	[2^24/OvrSpd]	*//* <S0E5> */
//	SHORT				MotSpdForMp;		/* ��Program EMF�⏞�v�Z�p���[�^���x	[15000/OvrSpd]	*//* <S04D>�F���g�p	*/
	SHORT				Dummy;				/* for Alignment */
} BASE_MOT_STS;


typedef	struct PCMP_CALC_DATA {
	KSGAIN				Kpcmpd;				/* �ʑ��⏞(�x��)���Z�p�Q�C��1(�ʃv���O�����p)			*/
	KSGAIN				Kpcmpd2;			/* �ʑ��⏞(�x��)���Z�p�Q�C��2(�ʃv���O�����p)			*/
	KSGAIN				Kpcmpsp1;			/* �ʑ��⏞(���x)���Z�p�Q�C��1(�ʃv���O�����p)			*/
	KSGAIN				KpcmpdItplt;		/* �ʑ���ԗʉ��Z�W��(�ʃv���O�����p)					*/
	LONG				PcmpSpd1;			/* �ʑ��⏞1�J�n���x									*/
} PCMP_CALC_DATA;


typedef	struct POS_CTRL_STS {
	BOOL				RefZSignal;			/* Reference Zero Signal								*/
	BOOL				NearSignal;			/* Near Signal											*/
	BOOL				CoinSignal;			/* Coin Signal											*/
	BOOL				DenSignal;			/* Den Signal											*/
} POS_CTRL_STS;


typedef	struct BASE_CTRL_HNDL {
	MENCV				*MencV;				/* ���[�^�G���R�[�_�f�[�^�ւ̃|�C���^			 		*//* <S004> */
	MENCV				*FencV;				/* �t���N�G���R�[�_�f�[�^�ւ̃|�C���^			 		*//* <S04D> */
	CTRL_CMD_MNG		CtrlCmdMngr;		/* ����w�ߊǗ��f�[�^									*/
	CTRL_CMD_PRM		CtrlCmdPrm;			/* ����w�ߗp�p�����[�^									*/
	BASE_MOT_STS		MotSts;				/* ScanB�ɂĎ擾�E�쐬���郂�[�^���					*/
	CTRLMD_SET			CtrlModeSet;		/* ���䃂�[�h�֘A�f�[�^									*/
	POS_MNG_HNDL		PosManager;			/* �ʒu�Ǘ��p�f�[�^�n���h��								*/
	POS_LOOP_CTRL		FcPosLoop;			/* �t���N�p�ʒu���[�v����f�[�^ 						*/
	POS_CTRL_STS		PosCtrlSts;			/* �ʒu����X�e�[�^�X(DEN/COIN/NEAR)					*/
	TRQ_LMT_DATA		TrqLimitData;		/* �g���N�����֘A�f�[�^									*/

//	MOTSPDMAFIL			MotSpdMafil;		/* ���[�^���x���o�p�ړ����ω��Z�p�ϐ�					*/
	SETTLINGTIME		SettlingTime;		/* ���莞�Ԍv���֘A�����p�\����							*/

	ZX_CTRL_HNDL		ZeroClampCtrl;		/* �[���N�����v���䉉�Z�p�\����							*/
	PCMDFIL				PcmdFil;			/* �ʒu�w�߃t�B���^�p�ϐ�								*//* <S065> */
	MAFIL				VFFcmdFil;			/* ���x�t�B�[�h�t�H���[�h�w�߃t�B���^�p�ϐ�				*//* <S1D0> */
	MAFIL				TFFcmdFil;			/* �g���N�t�B�[�h�t�H���[�h�w�߃t�B���^�p�ϐ�			*//* <S1D0> */
	RESVIB				ResVib;				/* �U���}���p�ϐ�										*/
	VIBSUPFIL			VibSupFil;			/* �U���}���t�B���^�p�ϐ�								*/
	MODE_SW				ModeSwData;			/* ���[�hSW�p�ϐ�										*/
	GAIN_CHNG_HNDL		GainChange;			/* �Q�C���؂�ւ��p�\����								*/
	MFCTRL				MFControl;			/* ���f���Ǐ]����p�ϐ�									*/
	TUNELESS_CTRL		TuneLessCtrl;		/* �������X����p�f�[�^									*/
	DOBS_CTRL			DobsCtrl;			/* �O���I�u�U�[�o����p�f�[�^							*/
	EHVOBS_CTRL			EhVobsCtrl;			/* �ʑ��⏞���x�I�u�U�[�o����p�f�[�^					*/
	PCMP_CALC_DATA		PcmpCalcData;		/* �ʑ��⏞���Z�p�f�[�^									*/
	TBLDRIVE			TblDrive;			/* �e�[�u���^�]�p�\����									*/
	PJOGHNDL			PJogHdl;			/* �v���O����JOG�p�\����								*/
	JATHNDL				JatHdl;				/* �I�t���C���������[�����g����							*/

	BOOL				BeComplete;			/* �x�[�X�C�l�[�u���������								*/
//	BOOL				PdetCtrlReq;		/* ���Ɍ��o����v���t���O								*/
	BOOL				CLatchClrReq;		/* C Phase Position Latch Clear Request to ��ScanA		*/
	BOOL				BBRefClrReq;		/* Servo Control Ref. Clear Request from ScanC			*/
//	BOOL				OverTravel;			/* OT���(OT�V�[�P���X�����o��) from ScanC				*/
//	BOOL				PhaseReady;			/* Motor Phase Ready (Phase Found Finish)				*/
	BOOL				VrefInSts;			/* ���x�w�ߓ��͒��t���O									*//* <S0A1> */
	BOOL				TrefInSts;			/* �g���N�w�ߓ��͒��t���O								*//* <S0A1> */
	LONG				GselNum;			/* �}�j���A���Q�C���ؑ֗p�Q�C���ԍ�						*/

	WEAKENFIELD			WeakenField;		/* ��ߊE�����䉉�Z�p�ϐ�								*/
	MPKVVCOMP			MpKvvComp;			/* �}�C�N���d���⏞�Q�C���@�\�p�ϐ�						*/

	TRANSDUCE_CTRL		TransduceCtrl;		/* ����p�p�����[�^�ꊇ�]���p�ϐ�						*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL				PerClrEdge;			/* @@ Position Error Clear Signal Edge					*/
	LONG				ClrFbPos;			/* @@ Position Error Clear Latched FB Position[Pulse]	*/
	LONG				MonPerrLpos[2];		/* @@ Position Error Clear Latched FB Position[Ref]		*/
/*--------------------------------------------------------------------------------------------------*/
	FRICTION			FrictionM;			/* <S0CD> */
	LONG				TrqMotFricComp;		/* <S0CD> */
/*--------------------------------------------------------------------------------------------------*/
	BOOL				OverTravel;			/* <S18B> */
/*--------------------------------------------------------------------------------------------------*/
	LONG				AmondPosCmd;		/* <S1B6> �ʒu�w�ߍ���				[pulse/125us]		*/
	
} BASE_CTRL_HNDL;


/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���䉉�Z�p�ϐ���`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* �ʒu����p�萔���ϐ���`							*/
		struct	{								/* �ʒu����p�萔�f�[�^��`							*/
#if (FLOAT_USE==TRUE)
//		LONG	Kpi;								/* �ʒu������ϕ��Q�C��						*/
		float	Kpd;								/* �ʒu����������Q�C��						*/
		float	VelBias;							/* �ʒu���䑬�x�o�C�A�X							*/
		float	BiasAddLevel;						/* ���x�o�C�A�X���Z��		[Pulse, mm]			*/
//		LONG	KdPff;								/* �ʒu����ʒu�����l�e�e�Q�C��					*/
//		LONG	KdPffFil;							/* �ʒu����ʒu�����l�e�e�t�B���^�Q�C��			*/
#else
//		LONG	Kpi;								/* �ʒu������ϕ��Q�C��						*/
		LONG	Kpd;								/* �ʒu����������Q�C��						*/
		LONG	VelBias;							/* �ʒu���䑬�x�o�C�A�X							*/
		LONG	BiasAddLevel;						/* ���x�o�C�A�X���Z��		[Pulse, mm]			*/
//		LONG	KdPff;								/* �ʒu����ʒu�����l�e�e�Q�C��					*/
//		LONG	KdPffFil;							/* �ʒu����ʒu�����l�e�e�t�B���^�Q�C��			*/
#endif /* FLOAT_USE */
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* �ʒu����p�ϐ��f�[�^��`							*/
#if (FLOAT_USE==TRUE)
		float	SpdRef;								/* �ʒu����o��									*/
		float	PerIvar64[2];						/* �ʒu�΍��ϕ����Z�p							*/
		float	PacOut;								/* ��ᐧ��o��									*/
		float	IacOut;								/* �ϕ�����o��									*/
		float	DacOut;								/* ��������o��									*/
		float	LastPacOut;							/* �O���ᐧ��o��								*/
		float	dPFfFilo;							/* �ʒu�����l�e�e�t�B���^�o��					*/
#else
		LONG	SpdRef;								/* �ʒu����o��									*/
		LONG	PerIvar64[2];						/* �ʒu�΍��ϕ����Z�p							*/
		LONG	PacOut;								/* ��ᐧ��o��									*/
		LONG	IacOut;								/* �ϕ�����o��									*/
		LONG	DacOut;								/* ��������o��									*/
		LONG	LastPacOut;							/* �O���ᐧ��o��								*/
		LONG	dPFfFilo;							/* �ʒu�����l�e�e�t�B���^�o��					*/
#endif /* FLOAT_USE */
		} V;
} POSCTRL;
/*--------------------------------------------------------------------------------------------------*/
/*		���x���䉉�Z�p�ϐ���`																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* ���x����p�萔���ϐ���`							*/
		struct	{								/* ���x����p�萔�f�[�^��`							*/
//		LONG	IPSpdControl;
//		LONG	SpdIctrlKeep;
//		LONG	P_I;								/* ���x����o�h���䊄��							*/
//		LONG	SpdSfsAcc;							/* �\�t�g�X�^�[�g������							*/
//		LONG	SpdSfsDec;							/* �\�t�g�X�^�[�g������							*/
//		LONG	KVrfFil;							/* ���x�w�߃t�B���^�Q�C��						*/
//		LONG	KVfbFil;							/* ���x�e�a�t�B���^�Q�C��						*/
		KSGAIN	KVintegFil;							/* ���x�ϕ��N���A�t�B���^�Q�C��					*//*<S00A>*/
//		LONG	SpdRefPhFil;						/* �ʑ��i�ݗp�k�o�e(���x�w�ߗp)					*//*<V108>*/
//		LONG	SpdRefPhFilGn;						/* �ʑ��i�ݕ⏞�Q�C��(���x�w�ߗp)				*//*<V108>*/
////		LONG	SpdFFPhFil;							/* �ʑ��i�ݗp�k�o�e(���x�e�e�p)					*//*<V108>*/
////		LONG	SpdFFPhFilGn;						/* �ʑ��i�ݕ⏞�Q�C��(���x�e�e�p)				*//*<V108>*/
//		LONG	SvonSpdLmtLevel;
//		LONG	SvonSpdLmtValue;
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* ���x����ϐ��f�[�^��`							*/
#if (FLOAT_USE==TRUE)
		float	TrqRef;								/* ���x����o��									*/
		float	SpdFbFilo;							/* ���x�e�a�t�B���^�o��							*/
		float	PacOut;								/* ��ᐧ��o��									*/
		float	IacOut;								/* �ϕ�����o��									*/
		LONG	Ivar64[2];							/* �ϕ����䉉�Z�p								*/
		float	SpdRefPhLpfo;						/* �ʑ��i�ݗp�k�o�e�o��(���x�w�ߗp)				*//*<V108>*/
		float	SpdRefPhHpfo;						/* �ʑ��i�ݗp�g�o�e�o��(���x�w�ߗp)				*//*<V108>*/
		float	SpdRefPhHpfTmp;						/* �ʑ��i�ݗp�g�o�e���Z�p(���x�w�ߗp)			*//*<V108>*/

		float	SpdFFPhLpfo;						/* �ʑ��i�ݗp�k�o�e�o��(���x�e�e�p)				*//*<V108>*/
		float	SpdFFPhHpfo;						/* �ʑ��i�ݗp�g�o�e�o��(���x�e�e�p)				*//*<V108>*/
		float	SpdFFPhHpfTmp;						/* �ʑ��i�ݗp�g�o�e���Z�p(���x�e�e�p)			*//*<V108>*/
#else
		LONG	TrqRef;								/* ���x����o��									*/
		LONG	SpdFbFilo;							/* ���x�e�a�t�B���^�o��							*/
		LONG	PacOut;								/* ��ᐧ��o��									*/
		LONG	IacOut;								/* �ϕ�����o��									*/
		LONG	Ivar64[2];							/* �ϕ����䉉�Z�p								*/
		LONG	SpdRefPhLpfo;						/* �ʑ��i�ݗp�k�o�e�o��(���x�w�ߗp)				*//*<V108>*/
		LONG	SpdRefPhHpfo;						/* �ʑ��i�ݗp�g�o�e�o��(���x�w�ߗp)				*//*<V108>*/
		LONG	SpdRefPhHpfTmp;						/* �ʑ��i�ݗp�g�o�e���Z�p(���x�w�ߗp)			*//*<V108>*/
		LONG	SpdFFPhLpfo;						/* �ʑ��i�ݗp�k�o�e�o��(���x�e�e�p)				*//*<V108>*/
		LONG	SpdFFPhHpfo;						/* �ʑ��i�ݗp�g�o�e�o��(���x�e�e�p)				*//*<V108>*/
		LONG	SpdFFPhHpfTmp;						/* �ʑ��i�ݗp�g�o�e���Z�p(���x�e�e�p)			*//*<V108>*/
#endif /* FLOAT_USE */

		UCHAR	SpdRefFilClrCmd;					/* ���x�w�߈ʑ��i�ݕ⏞�t�B���^�������R�}���h	*//*<V392>*/
		UCHAR	SpdFFFilClrCmd;						/* ���x�e�e�ʑ��i�ݕ⏞�t�B���^�������R�}���h	*//*<V392>*/
		BOOL	SvonSpdLmtFlag;
		BOOL	LastSvonFlag;
		} V;
} SPDCTRL;
/*--------------------------------------------------------------------------------------------------*/
/*		�g���N���䉉�Z�p�ϐ���`																	*/
/*--------------------------------------------------------------------------------------------------*/
//typedef	struct	{								/* �g���N����p�萔���ϐ���`						*/
//		struct	{								/* �g���N����p�萔�f�[�^��`						*/
//		LONG	OverTrqLoopGain[3];					/* ���x�����p���[�v�Q�C��						*/
//		LONG	OverTrqIntegGain[3];				/* ���x�����p�ϕ��Q�C��							*/
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LONG	Kp;									/* �A�i���OF/B���Q�C��						*/
		LONG	Kvi;								/* �A�i���OF/B�ϕ��Q�C��						*/
		LONG	KdFF;								/* �A�i���OF/B���[�vFF							*/
		LONG	AnlgfbLimit;						/* �A�i���OF/B�ϕ����~�b�g�l					*/
		LONG	Bias;								/* �A�i���OF/B�o�C�A�X							*/
		USHORT	OverCntVal;							/* �A�i���OF/B�ߑ匟�o���ԃJ�E���g�l			*/
		USHORT	AnlgfbPolarSw;						/* �A�i���OF/B�ɐ��I��							*/
		LONG	KIntegPreset;						/* �A�i���OF/B�ϕ��v���Z�b�g�W��				*/
#endif
//		} P;
	/*----------------------------------------------------------------------------------------------*/
//		struct {								/* �g���N����p�ϐ��f�[�^��`						*/
//		LONG	SpdLimit;							/* �g���N���䎞�̑��x�����l						*/
//		LONG	OverTrqIntegBuf[2];
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LONG	TrqRefx;							/* �A�i���OF/B���͎w��							*/
		LONG	PacOut;								/* ��ᐧ��o��									*/
		LONG	IacOut;								/* �ϕ�����o��									*/
		LONG	Ivar64[2];							/* �ϕ����䉉�Z�p								*/
		LONG	AnlgFbMon;							/* �A�i���OF/B�l���j�^[100%]					*/
		USHORT	OverCnt;							/* �A�i���OF/B�ߑ匟�o���ԃJ�E���^				*/
		UCHAR	AnlgfbSts_l;						/* �O��A�i���OF/B���							*/
		UCHAR	AnlgfbSts_ll;						/* �O�X��A�i���OF/B���						*/
		LONG	DTrqLpfoLatch;						/* �O������l���b�`								*/
		UCHAR	SensorContact;						/* �A�i���OFB�Z���T�ڐG							*/
		UCHAR	InitDone;							/* �g���N����ւ̐ؑւ����̏��������Q�x���Ȃ��悤�ǉ� */
#endif
//		} V;
//} TRQCTRL;
/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�t�B���^���Z�p�ϐ���`																*/
/*--------------------------------------------------------------------------------------------------*/
/*		�p�����[�^ : ScanA��ScanB�Ō�																*/
/*		���Z�p�ϐ� : ScanA��ScanB�ŋ��L(���䃂�[�h�ؑ֎��̃g���N�w�ߘA�����m�ۂ̂���)				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* �g���N�t�B���^�p�萔���ϐ���`					*/
		struct {								/* ScanA : �g���N�t�B���^�p�萔��`					*/
#if (FLOAT_USE==TRUE)
		float	Klpf;								/* �P���x��t�B���^�Q�C��						*/
		float	Knxf[3];							/* �m�b�`�t�B���^�Q�C��							*/
		float	Klpf2[4];							/* �Q���x��t�B���^�Q�C��						*/
		float	Ktrflpf;							/* T-REF�t�B���^								*//* <V102> */
#else
		LONG	Klpf;								/* �P���x��t�B���^�Q�C��						*/
		LONG	Knxf[3];							/* �m�b�`�t�B���^�Q�C��							*/
		LONG	Klpf2[4];							/* �Q���x��t�B���^�Q�C��						*/
		LONG	Ktrflpf;							/* T-REF�t�B���^								*//* <V102> */
#endif /* FLOAT_USE */
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LONG	Kafblpf;							/* �A�i���OF/B�t�B���^							*/
#endif
		} A;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* ScanB : �g���N�t�B���^�p�萔��`					*/
#if (FLOAT_USE==TRUE)
		float	Knxf[3];							/* �m�b�`�t�B���^�Q�C��							*/
		float	Klpf2[4];							/* �Q���x��t�B���^�Q�C��						*/
		float	Ktrflpf;							/* T-REF�t�B���^								*//* <V102> */
		float	Klpf3;								/* �P���x��t�B���^�Q�C��(�ٸ�⏞��)			*//*<V371>*/
#else
		LONG	Knxf[3];							/* �m�b�`�t�B���^�Q�C��							*/
		LONG	Klpf2[4];							/* �Q���x��t�B���^�Q�C��						*/
		LONG	Ktrflpf;							/* T-REF�t�B���^								*//* <V102> */
		LONG	Klpf3;								/* �P���x��t�B���^�Q�C��(�ٸ�⏞��)			*//*<V371>*/
#endif /* FLOAT_USE */
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* �g���N�t�B���^�p�ϐ��f�[�^��`					*/
#if (FLOAT_USE==TRUE)
		float	FilOut;								/* �g���N�t�B���^�o��							*/
		float	FilOut3;							/* �g���N�t�B���^�o��(�ٸ�⏞��)				*//*<V371>*/
		float	NxFil[3];							/* �m�b�`�t�B���^���Z�p							*/
		float	LpFil2[4];							/* �Q���x��t�B���^���Z�p						*/
		UCHAR	TrqFilClrCmd;						/* �g���N�w�߃t�B���^�ϕ��������R�}���h			*/
		UCHAR	TrqFilDisable;						/* �g���N�w�߃t�B���^����						*//* <V181> */
//		UCHAR	TrqInputPos;						/* �g���N�O���w�ߓ��͈ʒu�I��					*//* <S0B7> */
#else
		LONG	FilOut;								/* �g���N�t�B���^�o��							*/
		LONG	FilOut3;							/* �g���N�t�B���^�o��(�ٸ�⏞��)				*//*<V371>*/
		LONG	NxFil[3];							/* �m�b�`�t�B���^���Z�p							*/
		LONG	LpFil2[4];							/* �Q���x��t�B���^���Z�p						*/
		UCHAR	TrqFilClrCmd;						/* �g���N�w�߃t�B���^�ϕ��������R�}���h			*/
		UCHAR	TrqFilDisable;						/* �g���N�w�߃t�B���^����						*//* <V181> */
//		UCHAR	TrqInputPos;						/* �g���N�O���w�ߓ��͈ʒu�I��					*//* <S0B7> */
#endif /* FLOAT_USE */
		} V;
} TRQFIL;
/*--------------------------------------------------------------------------------------------------*/
/*		�[������~���[���N�����v����p�ϐ���`														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* �[��������p�萔���ϐ���`						*/
		struct	{								/* �[��������p�萔�f�[�^��`						*/
#if (FLOAT_USE==TRUE)
		float	Kp;									/* �ʒu������Q�C��							*/
		float	SpdLimit;							/* ���x���䃊�~�b�g�l							*/
#else
		LONG	Kp;									/* �ʒu������Q�C��							*/
		LONG	SpdLimit;							/* ���x���䃊�~�b�g�l							*/
#endif /* FLOAT_USE */
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* �[��������p�ϐ��f�[�^��`						*/
		KSGAIN	PosError;							/* �ʒu�΍�										*//*<S00A>*/
		} V;
} ZXCTRL;

/*--------------------------------------------------------------------------------------------------*/
/*		�L�����A���g���؂�ւ��p�ϐ���`															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* �L�����A���g���؂�ւ��p�萔���ϐ���`			*/
		struct	{								/* �L�����A���g���؂�ւ��p�萔�f�[�^��`			*/
		BOOL	CarrChange;							/* �L�����A���g���؂�ւ��t���O [0:OFF,1:ON]	*/
		LONG	IqLimitA;							/* �L�����A�؂�ւ��d��臒lA	[2^24/MaxTrq]	*/
		LONG	IqLimitB;							/* �L�����A�؂�ւ��d��臒lB	[2^24/MaxTrq]	*/
		LONG	VelLimitA;							/* �L�����A�؂�ւ����x臒lA	[2^24/OvrSpd]	*/
		LONG	VelLimitB;							/* �L�����A�؂�ւ����x臒lB	[2^24/OvrSpd]	*/
		SHORT	NormCarrCnt;						/* �ʏ�L�����A�J�E���g�l		[-]				*/
		SHORT	HighCarrCnt;						/* �؂�ւ����L�����A�J�E���g�l	[-]				*/
		BOOL	MotCarrUse;							/* ���[�^�L�����A(PnF25)�L�� 	[0:OFF,1:ON]	*//* ���[�^�L�����A���g���؂�ւ�<S117> */
		SHORT	CarrCnt;							/* ���݃L�����A�J�E���g�l		[-]				*//* ���[�^�L�����A���g���؂�ւ�<S117> */
		} conf;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* �L�����A���g���؂�ւ��p�ϐ��f�[�^��`			*/
		BOOL	CarrState;							/* ���݂̃L�����A�ݒ�							*/
		} var;										/*	(0:��L�����A, 1:���L�����A)				*/
} CARRCHNG;


/*--------------------------------------------------------------------------------------------------*/
/*		�L�����A���g���؂�ւ��p�ϐ���`															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* �L�����A���g���؂�ւ��p�萔���ϐ���`			*/
		struct	{								/* �L�����A���g���؂�ւ��p�萔�f�[�^��`			*/
#if (FLOAT_USE==TRUE)
		float	Kpcmpd;								/* Pcmpd Calculate Gain1						*/
		float	Kpcmpd2;							/* Pcmpd Calculate Gain2						*/
		float	Kpcmpsp1;							/* Pcmps Calculate Gain1						*/
		float	PcmpSpd1;							/* Pcmps Calculate Speed1						*/
		float	KpcmpdItplt;						/* �ʑ���ԗʉ��Z�W��							*/
#else
		LONG	Kpcmpd;								/* Pcmpd Calculate Gain1						*/
		LONG	Kpcmpd2;							/* Pcmpd Calculate Gain2						*/
		LONG	Kpcmpsp1;							/* Pcmps Calculate Gain1						*/
		LONG	PcmpSpd1;							/* Pcmps Calculate Speed1						*/
		LONG	KpcmpdItplt;						/* �ʑ���ԗʉ��Z�W��							*/
#endif /* FLOAT_USE */
		} conf;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* �L�����A���g���؂�ւ��p�ϐ��f�[�^��`			*/
#if (FLOAT_USE==TRUE)
		float	Pcmpd;								/* �ʑ��⏞(�x��⏞1)			[65536/360deg]	*/
		float	Pcmpd2;								/* �ʑ��⏞(�x��⏞2)			[65536/360deg]	*/
		float	Pcmps;								/* �ʑ��⏞(���x�⏞�v)			[65536/360deg]	*/
		float	Pcmps1;								/* �ʑ��⏞(���x�⏞1)			[65536/360deg]	*/
		float	Pcmps2;								/* �ʑ��⏞(���x�⏞2)			[65536/360deg]	*/
		float	PcmpdItplt;							/* �ʑ���ԗ�									*/
#else
		SHORT	Pcmpd;								/* �ʑ��⏞(�x��⏞1)			[65536/360deg]	*/
		SHORT	Pcmpd2;								/* �ʑ��⏞(�x��⏞2)			[65536/360deg]	*/
		SHORT	Pcmps;								/* �ʑ��⏞(���x�⏞�v)			[65536/360deg]	*/
		SHORT	Pcmps1;								/* �ʑ��⏞(���x�⏞1)			[65536/360deg]	*/
		SHORT	Pcmps2;								/* �ʑ��⏞(���x�⏞2)			[65536/360deg]	*/
		SHORT	PcmpdItplt;							/* �ʑ���ԗ�									*/
#endif /* FLOAT_USE */
		} var;
} PHASECMP;


/*--------------------------------------------------------------------------------------------------*/
/*		for Base Loop																				*/
/*--------------------------------------------------------------------------------------------------*/
/* ���\���̍Č����K�v�� */
typedef struct BASE_LOOPCTRLS {
	MENCV			*MencV;
	MENCV			*FencV;																			/* <S04D> */
	BPRMDAT			*Bprm;
	MOTSPDMAFIL		*MotSpdMafil;		/* ���[�^���x���o�p�ړ����ω��Z�p�ϐ�						*/
	BASE_CTRL_HNDL	*BaseCtrls;
	GSELGAINS		*GselGains;
	ASIC_HW_IF_STR	*pAsicHwReg;		/* Asic H/W ���W�X�^�A�h���X								*//* <S004> */
	ALMMAN_HANDLE	*AlmManager;		/* <S018> */
/* <S1B7> >>>>> */
	MOTLESSTST		*MotorLessTest;		/* ���[�^���X�e�X�gIF��`									*/
	void			(*SencReadPosition)(MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
										ALMMAN_HANDLE *AlmManager, BOOL BeComplete,
										void *BaseLoops);
	LONG 			(*FencReadPosition)(MENCV *FencV,
										ALMMAN_HANDLE *AlmManager, BOOL BeComplete,
										void *BaseLoops);
/* <<<<< <S1B7> */
	/*----------------------------------------------------------------------------------------------*/
	POSCTRL			PosCtrl;			/* �ʒu����p�ϐ�											*/
	SPDCTRL			SpdCtrl;			/* ���x����p�ϐ�											*/
//	TRQCTRL			TrqCtrl;			/* �g���N����p�ϐ�											*/
	TRQFIL			TrqFil;				/* �g���N�t�B���^�p�ϐ�										*/
	ZXCTRL			ZxCtrl;				/* �[������~���[���N�����v����p�ϐ�						*/
	/*----------------------------------------------------------------------------------------------*/
	CARRCHNG		CarrerChange;		/* �L�����A���g���؂�ւ��p�ϐ�								*/
	PHASECMP		PhaseComp;			/* �ʑ��⏞�p�ϐ�											*/
	VFBTIMBASE		Vfbtimbase;			/* �x�[�X�^�C�}�ɂ��FB���x�␳�p�ϐ�						*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float			MotPos;
	float			MotSpd;
	float			dMotPos;
	float			LastMotPos;
#else
	LONG			MotPos;
	LONG			MotSpd;
	LONG			dMotPos;
	LONG			LastMotPos;
#endif /* FLOAT_USE */
/*Full Closed Encoder Define Start	*//* <S04D> */
#if (FLOAT_USE==TRUE)
	float			FencPos;
	float			FencSpd;
	float			dFencPos;
	float			LastFencPos;
#else
	LONG			FencPos;
	LONG			FencSpd;
	LONG			dFencPos;
	LONG			LastFencPos;
#endif /* FLOAT_USE */
/*Full Closed Encoder Define End	*/
	/*----------------------------------------------------------------------------------------------*/
	USHORT			CtrlMode;			/* ��{���[�v���䃂�[�h										*/
	USHORT			LastCtrlMode;		/* �O���{���[�v���䃂�[�h									*//* <S007> */
	USHORT			ZCtrlMode;			/* �[��������^�[���N�����v���䎞							*/
	USHORT			Dummy;
	/*----------------------------------------------------------------------------------------------*/
	LONG 			CmdCtrlBit;
	BOOL			PConActFlg;			/* P�����Ԋm�F�t���O										*//* <S064> */
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float			dPcmda;				/* �ʒu�w�ߑ����l���́@[�w�ߒP��]							*/
	float			dPosRefi;			/* �ʒu����F�ʒu�w�ߍ�������		[Pulse/Scan]			*/
	float			NextdPcmda;			/* �ʒu����F����ʒu�w�ߑ����l����	[�w�ߒP��]				*/
	float			NextdPosRefi;		/* �ʒu����F����ʒu�w�ߍ�������	[Pulse/Scan]			*/
	float			dPosFbki;			/* �ʒu����F�ʒu�e�a��������		[Pulse/Scan]			*/
	PMAFV			pmafvar;
	float 			pmafbuf[4];
	float			SpdRefp;			/* �ʒu����F���x�w�ߏo��			[2^24/OvrSpd]			*/
	float			SpdRefi;			/* ���x����F���x�w�ߓ���			[2^24/OvrSpd]			*/
	float			SpdRefo;			/* ���x����F���x�w�߂r�e�r�o��		[2^24/OvrSpd]			*/
	float			SpdRefSum;			/* ���x����F�w�ߑ��x(Ref+FF)		[2^24/OvrSpd]			*/
	float			SpdFbki;			/* ���x����F���x�e�a����			[2^24/OvrSpd]			*/
	float			SpdObsFbki;			/* ���x����F���x�e�a����(OBS��)	[2^24/OvrSpd]			*/
	float			TrqRefs;			/* ���x����F�g���N�w�ߏo��			[2^24/MaxTrq]			*/
	float			SpdFFCx;			/* �g������F���x�e�e�⏞			[2^24/OvrSpd]			*/
	float			SpdFBCx;			/* �g������F���x�e�a�⏞			[2^24/OvrSpd]			*/
	float			TrqFFCx;			/* �g������F�g���N�e�e�⏞			[2^24/MaxTrq]			*/
	float			TrqFBCx;			/* �g������F�g���N�e�a�⏞			[2^24/MaxTrq]			*/
	float			TrqRefi;			/* �g���N�t�B���^�F�g���N�w�ߓ���	[2^24/MaxTrq]			*/
	float			TrqRefo;			/* �g���N�t�B���^�F�g���N�w�ߏo��	[2^24/MaxTrq]			*/
	float			TrqRefo_l;			/* �g���N�t�B���^�F�g���N�w�ߏo��	[2^24/MaxTrq]			*/
	float			TrqRefo_a;			/* �g���N�t�B���^�F�g���N�w�ߏo��	[2^24/MaxTrq]			*/
	float			TrqRefoLimit;		/* �g���N������g���N�w��			[2^24/MaxTrq]			*/
	float			TrqRefoComp;		/* �␳��g���N�w�ߏo��				[2^24/MaxTrq]			*/
	float			DisTrqRef3;			/* �g���N�O���w�ߓ���				[2^24/MaxTrq]			*/
	float			MotSpd_l;			/* Motor Speed						[2^24/OvrSpd]			*/
	float			MotSpd_a;			/* Motor Speed						[2^24/OvrSpd]			*/
#else
	LONG			dPcmda;				/* �ʒu�w�ߑ����l���́@[�w�ߒP��]							*/
	LONG			dPosRefi;			/* �ʒu����F�ʒu�w�ߍ�������		[Pulse/Scan]			*/
	LONG			NextdPcmda;			/* �ʒu����F����ʒu�w�ߑ����l����	[�w�ߒP��]				*/
	LONG			NextdPosRefi;		/* �ʒu����F����ʒu�w�ߍ�������	[Pulse/Scan]			*/
	LONG			dPosFbki;			/* �ʒu����F�ʒu�e�a��������		[Pulse/Scan]			*/
	PMAFV			pmafvar;
	LONG 			pmafbuf[4];
	LONG			SpdRefp;			/* �ʒu����F���x�w�ߏo��			[2^24/OvrSpd]			*/
	LONG			SpdRefi;			/* ���x����F���x�w�ߓ���			[2^24/OvrSpd]			*/
	LONG			SpdRefo;			/* ���x����F���x�w�߂r�e�r�o��		[2^24/OvrSpd]			*/
	LONG			SpdRefSum;			/* ���x����F�w�ߑ��x(Ref+FF)		[2^24/OvrSpd]			*/
	LONG			SpdFbki;			/* ���x����F���x�e�a����			[2^24/OvrSpd]			*/
	LONG			SpdObsFbki;			/* ���x����F���x�e�a����(OBS��)	[2^24/OvrSpd]			*/
	LONG			TrqRefs;			/* ���x����F�g���N�w�ߏo��			[2^24/MaxTrq]			*/
	LONG			SpdFFCx;			/* �g������F���x�e�e�⏞			[2^24/OvrSpd]			*/
	LONG			SpdFBCx;			/* �g������F���x�e�a�⏞			[2^24/OvrSpd]			*/
	LONG			TrqFFCx;			/* �g������F�g���N�e�e�⏞			[2^24/MaxTrq]			*/
	LONG			TrqFBCx;			/* �g������F�g���N�e�a�⏞			[2^24/MaxTrq]			*/
	LONG			TrqRefi;			/* �g���N�t�B���^�F�g���N�w�ߓ���	[2^24/MaxTrq]			*/
	LONG			TrqRefo;			/* �g���N�t�B���^�F�g���N�w�ߏo��	[2^24/MaxTrq]			*/
	LONG			TrqRefo_l;			/* �g���N�t�B���^�F�g���N�w�ߏo��	[2^24/MaxTrq]			*/
	LONG			TrqRefo_a;			/* �g���N�t�B���^�F�g���N�w�ߏo��	[2^24/MaxTrq]			*/
	LONG			TrqRefoLimit;		/* �g���N������g���N�w��			[2^24/MaxTrq]			*/
	LONG			TrqRefoComp;		/* �␳��g���N�w�ߏo��				[2^24/MaxTrq]			*/
	LONG			DisTrqRef3;			/* �g���N�O���w�ߓ���				[2^24/MaxTrq]			*/
	LONG			MotSpd_l;			/* Motor Speed						[2^24/OvrSpd]			*/
	LONG			MotSpd_a;			/* Motor Speed						[2^24/OvrSpd]			*/
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	SHORT			IdRef;				/* d���d���w��						[15000/Imax]			*/
	SHORT			IqRef;				/* q���d���w��						[15000/Imax]			*/
	SHORT			IqDist;				/* q���O���g���N					[15000/MaxTrq]			*/
	BOOL			TrqClamp;
	/*----------------------------------------------------------------------------------------------*/
	SHORT			PowMonVq;			/* ����d�̓��j�^���Z�pq���d��								*//* <S020> */
	SHORT			PowMonIqd;			/* ����d�̓��j�^���Z�pq���d��								*//* <S020> */
	SHORT			PowMonVd;			/* ����d�̓��j�^���Z�pd���d��								*//* <S020> */
	SHORT			PowMonIdd;			/* ����d�̓��j�^���Z�pd���d��								*//* <S020> */
	/*----------------------------------------------------------------------------------------------*/
/*Full Closed Encoder Define Start	*//* <S04D> */
#if (FLOAT_USE==TRUE)
	float			FencSpd_l;			/* �O��Load Speed					[2^24/OvrSpd]			*/
	float			FencSpd_a;			/* ����Load Speed					[2^24/OvrSpd]			*/
	float			SpdObsFbki_l;		/* �O��Load Speed					[2^24/OvrSpd]			*/
	float			SpdObsFbki_a;		/* ����Load Speed					[2^24/OvrSpd]			*/
#else
	LONG			FencSpd_l;			/* �O��Load Speed					[2^24/OvrSpd]			*/
	LONG			FencSpd_a;			/* ����Load Speed					[2^24/OvrSpd]			*/
	LONG			SpdObsFbki_l;		/* �O��Load Speed					[2^24/OvrSpd]			*/
	LONG			SpdObsFbki_a;		/* ����Load Speed					[2^24/OvrSpd]			*/
	/*----------------------------------------------------------------------------------------------*/
#endif /* FLOAT_USE */
/*Full Closed Encoder Define End	*/
	struct {
		UCHAR	PastInputClrEdge; 		/* AP��p�t���O!!											*/
		UCHAR	PerClrEdge;		 		/* AP��p�t���O!!											*/
		UCHAR	Reserved1;		 		/* AP��p�t���O!!											*/
		UCHAR	Reserved2;		 		/* AP��p�t���O!!											*/
	} f;
	/*----------------------------------------------------------------------------------------------*/
	LONG			TRefAd;				/* Torque Reference [AD value] <S051> 						*/

	RPLCMP	RippleCmp;				/* ���b�v���⏞�p�ϐ�										*//* <S14B> */
} BASE_LOOPCTRLS;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* Baseloops */
void	SysScanAInit( BASE_LOOPCTRLS *BaseLoops, MENCV *MencV, MENCV *FencV, BPRMDAT *Bprm, MOTSPDMAFIL *MotSpdMafil,
		 	 BASE_CTRL_HNDL *BaseCtrls, GSELGAINS *GselGains,ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE 	*AlmManager, /* <S04D>:�����ǉ� */
		 	 MOTLESSTST *MotorLessTest );	/* <S1B7>:�����ǉ� */
void	KpxInputScanA( BASE_LOOPCTRLS *BaseLoops );/*<S0C5>*/
ALMID_T	KpxOutputScanA( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs, PRMDATA *Prm );
//void	SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl, BASE_CTRL_OUT *BaseCtrlOut );
void	SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl,
													BASE_CTRL_OUT *BaseCtrlOut, CMD_CTRLBITS *CmdCtrl );/* <S050> */
void 	SysOutputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut ); /* 2012.12.27 Y.Oka �b��Ή� */
void	ApxYecServoControl( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut ); /* <S0B8> */


/* BaseControls */
void BpiYecServoControl( BASE_LOOPCTRLS*, SERVO_CONTROL_IF*, CTRL_LOOP_OUT*, FUN_CMN_CONTROL*, BPRMDAT* );/* <S04D> */
void	BpiMpoleFindMakePosCmd( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
													FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm);/* <S04D> */
void BpiTorqueLimitControl( TRQ_LMT_DATA*, BASE_CTRL_HNDL*,	BOOL, ALMMAN_HANDLE*); /* <S145> */
void BpiChangeSpdDetMaFil( MOTSPDMAFIL*, LONG num );		/*<S02F>*/
LONG BpiBasePositionControl( POS_LOOP_CTRL *PosLoop, CTRL_CMD_MNG *CtrlCmd,
								 POS_MNG_HNDL *PosManager, KSGAIN Kp, KSGAIN Kpi );
void BpiInputMotorPosition(BASE_CTRL_HNDL *BaseControls, ASIC_U_IF_STR *pAsicMicroReg,
							BPRMDAT *Bprm, PRMDATA *Prm, CTRL_LOOP_OUT *CtrlLoopOut);/* <S04D>:�������� *//* <S086> */
void BpiOutputNetStatus(BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                        	CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm, PLSIFV *pPls);/* <S04D>:�����ǉ� */
void BpiInputControlCMD( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                         CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, USHORT uwd_l, LONG ax_no);/* <S1C0>�F�����ǉ� */

void InitBasePositionControl( POSCTRL *PosCtrl );	/* <S14C> */

#endif	/* _BASE_CONTROLS_H_*/
/***************************************** end of file **********************************************/

