/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Motion.h : Motion Module Common Header file 												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Motion ���ʃw�b�_�t�@�C��																	*/
/*																									*/
/*		1. Global Function Prototypes ( MtXXX.h )													*/
/*																									*/
/*		2. Global Data Definition ( MtXXX.h )														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.09.05  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef		_MOTION_H_
#define		_MOTION_H_

#include "KnlApi.h"					/* Kernel API */

typedef SV_CYC_CMD CYC_CMD;			/* �b�� */
typedef SV_CYC_RSP CYC_RSP;			/* �b�� */
typedef UINT RSP_SEQBITS;			/* �b�� */
typedef UINT RSP_LTBITS;			/* �b�� */
typedef UINT CMD_SEQBITS;			/* �b�� */
typedef UINT CMD_LTBITS;			/* �b�� */

/****************************************************************************************************/
/*																									*/
/*	Grobal Definitions																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* defines : Control Mode 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define MT_NO_CTRLMD	(0)		/* ���䖳�����[�h */
#define MT_POS_CTRLMD	(1)		/* �ʒu���䃂�[�h */
#define MT_VEL_CTRLMD	(2)		/* ���x���䃂�[�h */
#define MT_TRQ_CTRLMD	(3)		/* �g���N���䃂�[�h */

/*--------------------------------------------------------------------------------------------------*/
/* defines : ���b�`���[�h(API�ݒ�p) */
/*--------------------------------------------------------------------------------------------------*/
#define MOTION_LTMD			1	/* ���[�V�������b�`���[�h */
#define MOTION_MDL_LTMD		2	/* ���[�V�������b�`���[�h(SVCTRL) */
#define MODAL_LTMD			3	/* ���[�_�����b�`���[�h(Normal) */

/*--------------------------------------------------------------------------------------------------*/
/* defines : ���b�`���[�h(���������p) */
/*--------------------------------------------------------------------------------------------------*/
#define INTERPOL_LTMD		4	/* ���[�V�������b�`���b�`���[�h(INTERPOLATE) */
#define EXPOSING_LTMD		5	/* ���[�V�������b�`���b�`���[�h(EXPOSING) */
#define ZRET_LTMD			6	/* ���[�V�������b�`���b�`���[�h(ZRET) */
//#define ZRET_LOCK_LTMD		7	/* ���[�V�������b�`���b�`���[�h(ZRET��) */
#define NONE_LTMD			0	/* ���b�`���[�h�Ȃ� */

/*--------------------------------------------------------------------------------------------------*/
/* defines : Soft Limit                                                                             */
/*--------------------------------------------------------------------------------------------------*/
#define SOFT_LIMIT_P	0x1
#define SOFT_LIMIT_N	0x2

/*--------------------------------------------------------------------------------------------------*/
/* enums : Motion Types																				*/
/*--------------------------------------------------------------------------------------------------*/
enum MOTIONTYPE {
	MTYPE_NONE=0,	/* 0:���[�V������~�� */ 
	MTYPE_POSING,	/* 1:�ʒu���� */ 
	MTYPE_EXPOSING,	/* 2:�O���ʒu���� */ 
	MTYPE_HOMING,	/* 3:���_���A */
	MTYPE_FEED,		/* 4:�葬���� */
	MTYPE_EXFEED,	/* 5:�O���M���葬���� */
	MTYPE_HOLD,		/* 6:��~ */
	MTYPE_INTERPOL,	/* 7:��ԑ��� */ 
	MTYPE_VELCTRL,	/* 8:���x���� */
	MTYPE_TRQCTRL,	/* 9:�g���N���� */
	MTYPE_FOLLOW,	/* A:�t�H���[�A�b�v */ 
};

/*--------------------------------------------------------------------------------------------------*/
/* enums : Hold Mode																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _MT_HOLD_MODE {
	MT_HOLD_SLOWSTOP = 0,
	MT_HOLD_FASTSTOP = 1,
	MT_HOLD_EMGSTOP  = 2,
} MT_HOLD_MODE;

/*--------------------------------------------------------------------------------------------------*/
/* enums : Latch Signal																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _MT_LATCH_SGNL {
	MT_LATCH_CPHASE = 0,
	MT_LATCH_EXT1,
	MT_LATCH_EXT2,
	MT_LATCH_EXT3,
}  MT_LATCH_SGNL;


/****************************************************************************************************/
/*																									*/
/*  Motion Parameter Definitions 																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* ���[�V��������@�\SW�F �����ɂ���ꍇ�̓R�����g�ɂ���											*/
/*--------------------------------------------------------------------------------------------------*/
//#define PRM_BANK_ENABLE		/* �o���N�؂�ւ��L��SW */
#define ACC_FIL_DISABLE	    /* �t�B���^����SW */
#define POSING_1RATEONLY	/* POSING 1�i�ږ���SW */

/*--------------------------------------------------------------------------------------------------*/
#define MAX_TRQ24			0x1000000	/* MAXTRQ */
#define MAX_SPD24			0x1000000	/* MAX���x */
#define DISTANCE_MAX		0x7FFFFFFF	/* �ړ�����MAX */
#define ACF_MAX_SZ			512			/* �ړ����σt�B���^�o�b�t�@�� */
#define FOLLOWUP_MAX_REF	32767		/* FollowUp���ő�ړ���[ref/cyc] */
#define MG_VREMBIT_MAX		24			/* MotionGenerator�]�葬�x���ZMAX�r�b�g�� */
#define MG_SPDDIV_RANGE		4			/* MotionGenerator�����_�ʒu�����ݒ�r�b�g�͈� */


/****************************************************************************************************/
/*																									*/
/*		Type Definitions																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Motion Command Structure(MtApi.c)																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MCI_CMD_ {
	LONG		TPos;		/* �ڕW�ʒu[ref] */
	LONG		TSpeed;		/* �ڕW���x[ref/s] */
	LONG		Velocity;	/* VFF/���x�w�� [ref/s],[2^30/MAX] */
	LONG		Trq;		/* �g���N�w��[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;	/* �����g���N����[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;	/* �����g���N����[MAX_TRQ/2^30] */
	LONG		AccRate;	/* �����萔<Rev2.01> */
	LONG		DecRate;	/* �����萔<Rev2.01> */
	ULONG		Option;		/* �I�v�V�����w�� */
	UCHAR		HoldMod;	/* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	UCHAR		LatchSgnl;	/* ���b�`�M���I�� 0:C��,n:EXTn */
	BOOL		EnableRate;	/* �����x�w�ߗL��<Rev2.01> */
	BOOL		RstReq;		/* ���[�V�������Z�b�g�v�� */
} MCI_CMD;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Command Structure(MtCtrl.c)																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MT_CMD_ {
	LONG	TPos;		    /* �ڕW�ʒu[ref] */
	LONG	TSpeed;		    /* �ڕW���x[ref/s] */
	LONG	Velocity;	    /* VFF[re/s], ���x�w��[2^30/MAX] */
	LONG	Trq;		    /* �g���N�w��[MAX_TRQ/2^30] */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	Mtype;		    /* ���[�V�����^�C�v */
	UCHAR	bHold;	    	/* HOLD�w�� */
	UCHAR	HoldMod;	    /* ��~���@�I�� 0: ������~1,1:�}����~,2������~2 */
	UCHAR	LatchSgnl;	    /* ���b�`�M���I�� 0:C��,n:EXTn */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	AccFilSel;	    /* �t�B���^�I�� 0,3: �Ȃ�, 1:�w��,2:�ړ����� */
	UCHAR	CtrlMode;	    /* ���䃂�[�h */
	UCHAR	CmdRstReq;	    /* �R�}���h���Z�b�g�v�� */
	UCHAR	MtEenable;	    /* ���[�V�����L�� */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ServoOn;	    /* �T�[�{ON�w�� */
	UCHAR	SensOn;		    /* �Z���TON�w�� */
	UCHAR	SvonLock;	    /* �T�[�{ON Lock */
	UCHAR	CmdRateOn;	    /* �������w�ߗL��SW<Rev2.01> */
/*--------------------------------------------------------------------------------------------------*/
	LONG	Rdst;	        /* Registration distination offset [ref] */
	LONG	Rspd;		    /* Registration speed [ref/min] */
/*--------------------------------------------------------------------------------------------------*/
	LONG	TspdKx;			/* Speed Gain [spdref] -> [ref/cycle]									*/
//	LONG	Tlim;			/* �ٸ�Я�[%] */
} MT_CMD;

/*--------------------------------------------------------------------------------------------------*/
/*  Latch Command Structure																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MT_LATCH_ {
	UCHAR	Req;		/* ���b�`�v�� */
	UCHAR	Cmp;		/* ���b�`���� */
	UCHAR	Mode;		/* ���b�`���[�h */
	UCHAR	Sgnl;		/* �I��M�� */

	UCHAR	LtDisable;	/* ���b�`���� */
	UCHAR	SeqReq;		/* �A�����b�`�v�� */
	UCHAR	SeqCnt;		/* �A�����b�`�V�[�P���X�J�E���^ */
	union {				/* �A�����b�`�X�e�[�^�X */
		USHORT	wdCnt;
		UCHAR	cnt[2];	/* [0]:�A���񐔃J�E���g,[1]:�V�[�P���X�J�E���g */
	}SeqSts;
} MT_LATCH;


/*--------------------------------------------------------------------------------------------------*/
/*  Motion Generator Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MG_PRM_ {
	LONG	accrate1;			/* �����萔�P ***.*[ref/cyc^2] */
	LONG	accrate2;			/* �����萔�Q ***.*[ref/cyc^2] */
	LONG	altspd_a;			/* �����萔�֑ؑ��x ***.*[ref/cyc] */
	LONG	decrate1;			/* �����萔�P ***.*[ref/cyc^2] */
	LONG	decrate2;			/* �����萔�Q ***.*[ref/cyc^2] */
	LONG	altspd_d;			/* �����萔�֑ؑ��x ***.*[ref/cyc] */
} MG_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Control Base Parameter																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MC_BASE_PRM_ {
	LONG	MotionCycle;		/* ���[�V�����������[0.25us] */
	BOOL	FullcCtrl;			/* Fullc�g�p */
	BOOL	Linear;				/* ���j�A�g�p */
	BOOL	ot_zstop;			/* OT����۸���� */
	UCHAR	trefMode;			/* ���x���䎞�̃g���N�w�ߎg�p�I�� */
	UCHAR	TrefModePos;		/* �ʒu���䎞�̃g���N�w�ߎg�p�I��(INTERPOLATE�n) */
	UCHAR	TLimModePos;		/* �ʒu���䎞�̃g���N�����g�p�I��(POSING�n) */

	LONG	PosingRateKx;		/* [10000ref/s^s]->[***.*ref/cyc^cyc]�������x�ϊ��m�[�}���C�Y�萔 */
	LONG	PosingRateKxIx;		/* TODO:[(1000ref/min)/msec]->[***.*ref/cyc^cyc]�������x�ϊ��m�[�}���C�Y�萔 */
	LONG	PosingSpdKx;		/* [ref/s]->[***.*ref/cyc]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	PosingSpdKxIx;		/* TODO:[1000ref/min]->[***.*ref/cyc]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	PosingSpdInvKx;		/* [***.*ref/cyc]->[ref/s]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	RefToRpmKxIx;		/* TODO:[1000ref/min]->[rpm]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	fspdRefKx;			/* [ref/�ʐM����]->[ref/s]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	refToOsKx;			/* [�w�ߒP��/s]->[24bit/OS]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	osToRefKx;			/* [24bit/OS]->[�w�ߒP��/s]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	maxToOsKx;			/* [30bit/MAX]->[24bit/OS]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	osToMaxKx;			/* [24bit/OS]->[30bit/MAX]���x�ϊ��m�[�}���C�Y�萔 */
	LONG	trqKx;				/* [24bit/MAX]->[%]�m�[�}���C�Y�萔 */
	LONG	invTrqKx;			/* [%]->[24bit/MAX]�m�[�}���C�Y�萔 */
	LONG	taccRateKx;			/* [(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	LONG	taccRate7Kx;		/* [2^7*(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	VOID	(*SetCmdAccRate)();/* �����x�w�ߌv�Z�֐��|�C���^<Rev2.01> */

	UCHAR	vel_sft_exp;		/* ���x����\�����_�r�b�g��ACCFIL�p [bit] */
	UCHAR	vel_shift;			/* ���x����\�����_�r�b�g��MG�p [bit] */
	UCHAR	vel_shift_s;		/* ���x����\�����؎̃r�b�g�� [bit] */
	LONG	vel_max;			/* �ő呬�x(=2^(31-vel_shift))[ref/cyc] */
	LONG	seg_num;			/* �Z�O�����g������ */
	ULONG	seg_max_ref;		/* �Z�O�����g���͍ő� */
	ULONG	PosingSpdMax;		/* POSING�ő呬�x */
	ULONG	mgSpdMax;			/* MG��POSING�ő呬�x */
	MG_PRM	StopPrm;			/* ����~�p�����p�����[�^ */
} MC_BASE_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Position Parameter																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct PSN_PRM_ {
	BOOL	sotEnableP;			/* ���]���\�t�g���~�b�g�L�� */
	BOOL	sotEnableN;			/* �t�]���\�t�g���~�b�g�L�� */
	BOOL	tposLimitEnable;	/* �w�߃\�t�g���~�b�g�L�� */
	BOOL	absPG;				/* ��޿PG�g�p */
	LONG	absOfsset;			/* ��ΒlPG�̾�Ēl(Pn803) */
	LONG	softLimit_P;		/* ���]���\�t�g���~�b�g�l */
	LONG	softLimit_N;		/* �t�]���\�t�g���~�b�g�l */
	LONG	ltRangeP;			/* Pn820:�������b�`�]�[�� */
	LONG	ltRangeN;			/* Pn822:�������b�`�]�[�� */
	ULONG	zPointWidth;		/* ���_�ʒu�͈� */
	ULONG	psetWidth;			/* Near�͈� */
	ULONG	nearWidth;			/* Pset�͈� */
	SHORT	TdlyIn;				/* MPOS�␳���� */
	USHORT	SegNum;				/* ������(���[�V��������/�T�[�{����) */
} PSN_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Sequence Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MT_SEQ_PRM_ {
	LONG	zretsp1;			/* ZRET ���۰����x1 [ref/sec -> ref/cycle] */
	LONG	zretsp2;			/* ZRET ���۰����x2 [ref/sec -> ref/cycle] */
	LONG	zret_dist;			/* ZRET�ŏI���s���� [ref] */
	LONG	ex_posing_dist;		/* EX_POSING�ŏI���s���� */
	ULONG	rampTimeOut;		/* ������~�^�C���A�E�g����[���[�V��������] */
	ULONG	tlimTimeCnt1;		/* �����t�����_���A�ٸ�������o����1 */
	ULONG	tlimTimeCnt2;		/* �����t�����_���A�ٸ�������o����2 */
	LONG	zret_dir;			/* ���_���A���� */
	LONG	zretOSLevel;		/* zret overspeed alarm level [2^24/MaxSpd] */
} MT_SEQ_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Acc filter Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union ACC_MATB_PRM_ {
	ULONG	lTb;		/* �ړ����ω�(Total) */
	USHORT	wTb[2];		/* high word:�ړ����ω�, low word:�ړ����ϕ�ԉ�  */
} MATB_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Acc filter Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ACCFIL_PRM_ {
	LONG	expbias;			/* �w���������o�C�A�X */
	LONG	exptb;				/* �w�����������萔 */
	MATB_PRM mafil;				/* �ړ����ω� */
} ACF_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Option Map Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct OPTION_MAP_ {
	UCHAR	accfilBit;			/* �������t�B���^�I���r�b�g�ʒu */
	UCHAR	accfilBitMsk;		/* �������t�B���^�I���r�b�g�}�X�N */
	UCHAR	gselBit;			/* �Q�C���I���r�b�g�ʒu */
	UCHAR	gselBitMsk;			/* �Q�C���I���r�b�g�}�X�N */
	UCHAR	vppiBit;			/* P_PI�I���r�b�g�ʒu */
	UCHAR	vppiBitMsk;			/* P_PI�I���r�b�g�}�X�N */
	UCHAR	ppiclrBit;			/* �ʒu�ϕ��N���A�r�b�g�ʒu */
	UCHAR	ppiclrBitMsk;		/* �ʒu�ϕ��r�b�g�}�X�N */
	UCHAR	pclBit;				/* P_CL�r�b�g�ʒu */
	UCHAR	pclBitMsk;			/* P_CL�r�b�g�}�X�N */
	UCHAR	nclBit;				/* N_CL�r�b�g�ʒu */
	UCHAR	nclBitMsk;			/* N_CL�r�b�g�}�X�N */
	UCHAR	outpBit;			/* OutSignal�r�b�g�ʒu */
	UCHAR	outpBitMsk;			/* OutSignal�r�b�g�}�X�N */
	UCHAR	bankBit;			/* BankSel�r�b�g�ʒu */
	UCHAR	bankBitMsk;			/* BankSel�r�b�g�}�X�N */
	UCHAR	ltBit;				/* LtDisable�r�b�g�ʒu */
	UCHAR	ltBitMsk;			/* LtDisable�r�b�g�}�X�N */
} OPTION_MAP;

/*--------------------------------------------------------------------------------------------------*/
/*  Input Signal Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct INSIG_MAP_ {
	UCHAR	bitNo[8];
	USHORT	bitsMsk;
} INSIG_MAP;

/*--------------------------------------------------------------------------------------------------*/
/*  Sequencial Latch Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct LTSEQ_PRM_ {
	USHORT	seqNum;				/* �V�[�P���X�M���� */
	USHORT	loopCnt;			/* �A���񐔐ݒ� */
	UCHAR	ltSgnl[8];			/* ���b�`�M�� */
} LTSEQ_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion generator inout																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MG_INOUT_ {
	UCHAR	Mtype;			/* Motion Type */
	UCHAR	LastMtype;		/* Last Motion Type */
	USHORT	Ctrl;			/* Motion����Option */
/*--------------------------------------------------------------------------------------------------*/
	LONG	DposIn;			/* �w�߈ړ���[ref] */
	//LONG	Tspd;			/* POSING�p�ڕW���x[ref/s] */
	LONG	Tspd;			/* POSING�p�ڕW���x[ref/cycle] (�b��) */
	LONG	Vref;			/* ���x�w��[30bit/MAX] */
	LONG	Tref;			/* �g���N�w��[30bit/MAX] */
/*--------------------------------------------------------------------------------------------------*/
	LONG	DposOut;		/* �o�͈ړ���[ref](Vel->Pos) */
	LONG	VrefOut;		/* �o�͑��x�w��[24bit/OS] */
	LONG	TrefOut;		/* �o�̓g���N�w��[24bit/MAX] */
} MG_INOUT;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion generator handle																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MGHNDL_ {
	BOOL			cMinusDir;	/* ���ݕ����o������ */
	BOOL			tMinusDir;	/* ���݂̎w�߈ʒu����ڕW�ʒu�̕��� */
	ULONG			dpos0;		/* DPOS�̏����� for POSING�̎w�ߑ��x�i�������j�̐ώZ */
	LONG			dposRem;	/* �`�������ړ���(dpos)�]��         */
	LONG			Cspd;		/* ���ݎw�ߑ��x */
	MC_BASE_PRM*	basePrm;	/* MG��{�p�����[�^ */
	MG_PRM*			prm;		/* Motion�p�����[�^ */
} MGHNDL;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Parameter																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MTPRM_ {
	MC_BASE_PRM BasePrm;		/* ��{�p�����[�^ */
	MG_PRM		MgPrm;			/* MG���[�V�����p�����[�^ */
	MG_PRM		CmdRate;		/* �������w��<Rev2.01> */
	PSN_PRM		PsnPrm;			/* PositionManager�p�����[�^ */
	MT_SEQ_PRM	MtSeqPrm;		/* Motion�w�ߍ쐬�p�p�����[�^ */
	//ACF_PRM		AccFilPrm;		/* �ʒu�w�߃t�B���^�p�����[�^ */
	//OPTION_MAP	OptMap;			/* OPTION�w�߃r�b�g�ʒu�ݒ�p�����[�^ */
	//INSIG_MAP	InSigMap;		/* Input Signal0-7���t */
#ifdef PRM_BANK_ENABLE
	//MTBANK		Bnk;			/* Motion PRM Bank�f�[�^ */
#endif
	//LTSEQ_PRM	LtPrm;
} MTPRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Initial Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MTPRM_INI_ {
	LONG	EGearA;			/* �d�q�M�AA(����) */
	LONG	EGearB;			/* �d�q�M�AB(���q) */
	LONG	PgPuls;			/* PG�p���X��[puls/rev],[puls/pitch] */
	LONG	MaxTrq;			/* MAX�g���N[%] */
	LONG	OverSpd;		/* OS���x[rpm],[mm/s] */
	LONG	MaxSpd;			/* MAX���x[rpm],[mm/s] */
	LONG	ScalePitch;		/* �X�P�[���s�b�`[mm/pitch] */
	SHORT	OverSpdExp;		/* OS���x�w�� */
	SHORT	MaxSpdExp;		/* MAX���x�w�� */
	SHORT	ScalePitchExp;	/* �X�P�[���s�b�`�w�� */
	USHORT	trefMode;		/* �g���N����Option */
	BOOL	ot_zstop;		/* OT����۸���� */
	BOOL	absPG;			/* ��޿PG�g�p */
	BOOL	FullcCtrl;		/* �ٸ۰���ސ��� */
	BOOL	Linear;			/* ���j�A�g�p */

	USHORT	Cycle;			/* ���[�V�����������[0.25us] */
	USHORT	SpdDiv;			/* ����\ */
	ULONG	SegNum;			/* ��ԑ���Z�O�����g�� */
	USHORT	OptEnable;		/* Option���t�L�� */
	USHORT	OptMap[5];		/* Option���t��` */
	USHORT	OutSigEnable;	/* Option���t�o�͐M���L��/���� */
	SHORT	dlyTime;		/* �w�ߏo�͂���FB���͂܂ł̎��� */
	USHORT	SvCycle;		/* �T�[�{����(���[�V��������/�T�[�{����) */
/*--------------------------------------------------------------------------------------------------*/
	LONG	TspdGain;		/* �ڕW���x�Q�C�� */
/*--------------------------------------------------------------------------------------------------*/
} MTPRM_INI;

/*--------------------------------------------------------------------------------------------------*/
/*  Position Handle Structure																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct PSNHNDL_ {
	LONG		Tpos;		/* �ڕW�ʒu */
	LONG		Ipos;		/* �w�߈ʒu�i�t�B���^�O�j */
	LONG		Pos;		/* �w�߈ʒu�i�t�B���^��j */
	LONG		Mpos;		/* FB�ʒu���̓^�C�~���O�ł̎w�߈ʒu */
	LONG		Apos;		/* FB�ʒu */
	LONG		Perr;		/* �΍� */
	LONG		TlimPerr; /* �ٸ�ЯĎ��̈ʒu�΍� */
	LONG		Lpos;		/* ���b�`�ʒu */
	LONG		LstLpos;	/* �O�񃉃b�`�ʒu */
	LONG		Fbspd;		/* FB���x */
//	LONG		FbSpdx;		/* FB���x[2ms����] */
//	LONG		FbSpdSum;	/* FB���x[2ms����]�pSum�l */
	LONG		Pgcnt;		/* �ڕW�ʒu */
	LONG		Dpos;		/* ����o�͈ړ����� */
	LONG		lstDpos;	/* �O��o�͈ړ����� */
	BOOL		Den;		/* �w�ߏo�͊��� */
	BOOL		RefEnable;	/* ���t�@�����X�_�L�� */
	BOOL		PosEnable;	/* ���W�ʒu�L�� */
	UCHAR		SoftLimit;	/* �\�t�g���~�b�g��� */
//	UCHAR		FbSpdSumCnt;/* FB���x[2ms����]�p���� */	
	PSN_PRM*	Prm;		/* Potion�p�����[�^ */
} PSNHNDL;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Handle Structure																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MTHNDL_ {
	UCHAR		seqState;	/* �w�ߐ����V�[�P���X��� */
	UCHAR		otState;	/* OT��� */
//#if defined ( CCSW_SFTID ) && ( CCSW_SFTID == SFTID_EXIST )								/*<V717>Add:Start*/
//	UCHAR		amState;	/* AM��� */
//#endif															/*<V717>Add:End  */
	UCHAR		mtState;	/* ���[�V������� */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR		crtMtype;	/* ���݃��[�V�����^�C�v */
	UCHAR		ctrlMode;	/* ���ݐ��䃂�[�h */
	UCHAR		tlimSig[2]; /* �g���N���~�b�g�t�B���^ */
/*--------------------------------------------------------------------------------------------------*/
	LONG		ZretFspdSum;     /* feedback speed sum (for homing)                                 */
	LONG		ZretFspdSumCnt;  /* feedback speed sum count (for homing)                           */
/*--------------------------------------------------------------------------------------------------*/
	ULONG		timer;		/* RampStop�p�^�C�} */
	LONG		segCnt;		/* ���[�V���������o���񐔃J�E���^ */
	ULONG		tlimCnt;	/* �g���N���~�b�g�^�C�}�J�E���^ */
/*--------------------------------------------------------------------------------------------------*/
	MT_CMD		cmdData;	/* ���[�V�����w�߃f�[�^ */
	MG_INOUT	outData;	/* ���[�V�����o�̓f�[�^ */
	MT_LATCH	latch;		/* ���b�` */
	MGHNDL		mtgHdl;		/* MotionGenerator�n���h�� */
	PSNHNDL		psnHdl;		/* Position�n���h�� */
	//ACFHNDL		acfHdl;		/* AccFilter�n���h�� */
	MTPRM		mtPrm;		/* MotionPRM�n���h�� */
/*--------------------------------------------------------------------------------------------------*/
	struct {
		MTPRM_INI	mt;		/* ���[�V���������p�����[�^�f�[�^ */
	} iniPrm;				/* �����p�����[�^���[�h�f�[�^ */
/*--------------------------------------------------------------------------------------------------*/
	CYC_CMD*	svCmd;		/* Servo If Cyclic Command Data*/
	CYC_RSP*	svRsp;		/* Servo If Cyclic Response Data*/
} MTHNDL;

/****************************************************************************************************/
/*																									*/
/*		Grobal Function Prototypes																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  MtCtrl.c: Motion Control																		*/
/*--------------------------------------------------------------------------------------------------*/
extern VOID MtcInit(MTHNDL* hdl, SV_OPTION_IF* svData);				/* initialize motion */
extern VOID MtcOutputData(MTHNDL* hdl, SV_OPTION_IF *svData);		/* output motion command */
extern VOID MtcInputData(MTHNDL* hdl, SV_OPTION_IF *svData);		/* input motion data */
extern VOID MtcCreateInCmd(MTHNDL* hdl);							/* create motion in command */
void	MtcSetHomeMethod(long prm);							        /* set homing method */
/*--------------------------------------------------------------------------------------------------*/
/*  MtGenerator.c: Motion Generator																	*/
/*--------------------------------------------------------------------------------------------------*/
extern BOOL MtgGenerate(MGHNDL* hdl, MG_INOUT* inOut);				/* generate motion */
/*--------------------------------------------------------------------------------------------------*/
/*  MtPRM.c: Motion Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
LONG MPrmCalcInit(MTPRM* prm, MTPRM_INI* iniPrm);					/* calculate motion parameters  */
LONG MPrmSetZretDir(MTPRM* prm, BOOL data);							/* set homing dir               */
LONG MPrmSetZretSpd1(MTPRM* prm, LONG data);						/* set homing approach spd      */
LONG MPrmSetZretSpd2(MTPRM* prm, LONG data);						/* set homing creap spd         */
LONG MPrmSetZretDist(MTPRM* prm, LONG data);						/* set homing distination       */
LONG MPrmSetZretOSLvl(MTPRM* prm, LONG data);						/* set homing overspeed level   */
LONG MPrmSetSoftLimitFlag(MTPRM* prm, USHORT data);					/* sot enable or disable 		*/
LONG MPrmSetSoftLimitP(MTPRM* prm, LONG data);						/* set positive sot 			*/
LONG MPrmSetSoftLimitN(MTPRM* prm, LONG data);						/* set negative sot 			*/
LONG MPrmSetTlimTimeOut1(MTPRM* prm, LONG data);					/* set homing timeout1          */
LONG MPrmSetTlimTimeOut2(MTPRM* prm, LONG data);					/* set homing timeout2          */
/*--------------------------------------------------------------------------------------------------*/
/*  Position.c: Position Manager																	*/
/*--------------------------------------------------------------------------------------------------*/
extern VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm);
extern VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt);
extern BOOL PsnGetPsetSts(PSNHNDL* hdl);
extern BOOL PsnGetNearSts(PSNHNDL* hdl);
extern BOOL PsnGetZPointSts(PSNHNDL* hdl);
extern LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos);
extern LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos);
extern VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err);
extern UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos);
extern UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot);
extern VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos);
extern VOID PsnResetApos(PSNHNDL* hdl, BOOL abs);
extern BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt);
#define	PsnSetRefPoint(hdl, enable)	((hdl)->RefEnable = enable)
#define	PsnResetIPOS(hdl)			((hdl)->Ipos = (hdl)->Pos)
#define	PsnSetTPOSNoLimit(hdl,dpos)	((hdl)->Tpos = dpos + (hdl)->Ipos)
#define	PsnSetPOS(hdl, dpos)		{(hdl)->Pos += dpos;\
									 (hdl)->lstDpos = (hdl)->Dpos;\
									 (hdl)->Dpos = dpos;}
#define	PsnSetIPOS(hdl, dpos)		((hdl)->Ipos += dpos)
#define	PsnSetDenSts(hdl, den)		((hdl)->Den = den)
#define	PsnGetDenSts(hdl)			((hdl)->Den)
#define	PsnGetTPOS(hdl)				((hdl)->Tpos)
#define	PsnGetIPOS(hdl)				((hdl)->Ipos)
#define	PsnGetPOS(hdl)				((hdl)->Pos)
#define	PsnGetMPOS(hdl)				((hdl)->Mpos)
#define	PsnGetLPOS(hdl)				((hdl)->Lpos)
#define	PsnGetLstLPOS(hdl)			((hdl)->LstLpos)
#define	PsnGetAPOS(hdl)				((hdl)->Apos)
#define	PsnGetPERR(hdl)				((hdl)->Perr)
#define	PsnGetSOTSts(hdl)			((hdl)->SoftLimit)
#define	PsnGetFBSPD(hdl)			((hdl)->Fbspd)
#define	PsnAttachPRM(hdl, prm)		((hdl)->Prm = prm)
/*--------------------------------------------------------------------------------------------------*/
/*  MtApi.c: Motion API																				*/
/*--------------------------------------------------------------------------------------------------*/
extern INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern VOID MciSetOptionCmd(MTHNDL* hdl, ULONG option);

extern INT32 MciEnableState(MTHNDL* hdl, BOOL enable);
extern INT32 MciResetMotion(MTHNDL* hdl);
extern INT32 MciSetServoOn(MTHNDL* hdl, BOOL enable, BOOL reset);
extern INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable);

extern INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode);
extern INT32 MciSetCordinatePos(MTHNDL* hdl, LONG data, BOOL ref);

extern VOID MciOutputSignals(MTHNDL* hdl, USHORT signl);
extern VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable);
/*--------------------------------------------------------------------------------------------------*/
extern UCHAR MciGetMType(MTHNDL* hdl);
extern BOOL MciGetServoOnStatus(MTHNDL* hdl);
extern BOOL MciGetSensOnStatus(MTHNDL* hdl);
extern UCHAR MciGetCtrlMode(MTHNDL* hdl);
extern LONG MciGetMonitorData(MTHNDL* hdl, USHORT sel, UCHAR unit);
extern BOOL MciGetDenStatus(MTHNDL* hdl);
extern BOOL MciGetLatchStatus(MTHNDL* hdl ,UCHAR *ltEdge);
extern USHORT MciGetLatchSeqStatus(MTHNDL* hdl);

extern BOOL MciGetInPosStatus(MTHNDL* hdl);
extern BOOL MciGetNearStatus(MTHNDL* hdl);
extern BOOL MciGetZpointStatus(MTHNDL* hdl);
extern UCHAR MciGetSoftLimitStatus(MTHNDL* hdl);
extern LONG MciGetOutputCmdDist(MTHNDL* hdl);

//extern BOOL MciGetZspdStatus(MTHNDL* hdl);
//extern BOOL MciGetMainPowerStatus(MTHNDL* hdl);
extern BOOL MciGetVlimitStatus(MTHNDL* hdl);
extern BOOL MciGetTrqLimitStatus(MTHNDL* hdl);
extern BOOL MciGetVcmpStatus(MTHNDL* hdl);

extern USHORT MciGetInputSignals(MTHNDL* hdl);
extern USHORT MciGetInPortSignals(MTHNDL* hdl);

#if defined( CCSW_SFTID  ) && ( CCSW_SFTID == SFTID_EXIST )						/*<V717>Add:Start*/
extern USHORT MciGetSomStatus(void);
#endif													/*<V717>Add:End  */

/*--------------------------------------------------------------------------------------------------*/
//#if defined(__ghs__)
//#pragma ghs startzda
//#elif !defined(WIN32)
//#pragma section sidata begin
//#endif
extern MTHNDL	MtHdl;							/* motion handle */
//extern void	(*pIxxAxctrlReqHoming)( MT_CMD *mtcmd, long tspeed,
//									 MT_LATCH_SGNL sgnl );    /* Pointer Homing request    			*/
//#if defined(__ghs__)
//#pragma ghs endzda
//#elif !defined(WIN32)
//#pragma section sidata end
//#endif

#endif /* _MOTION_H_ */
