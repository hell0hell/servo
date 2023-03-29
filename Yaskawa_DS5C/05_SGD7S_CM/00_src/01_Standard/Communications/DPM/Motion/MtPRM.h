/****************************************************************************
Description	: MECHATROLINK Motion Paramter
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef		MOTOIN_PRM_H_

#include "UnitPrmStruct.h"

/****************************************************************************/
/* ���[�V��������@�\SW�F �����ɂ���ꍇ�̓R�����g�ɂ���					*/
/****************************************************************************/
#define PRM_BANK_ENABLE		/* �o���N�؂�ւ��L��SW */
//#define ACC_FIL_DISABLE	/* �t�B���^����SW */
//#define POSING_1RATEONLY	/* POSING 1�i�ږ���SW */

#define MAX_TRQ24			0x1000000	/* MAXTRQ */
#define MAX_SPD24			0x1000000	/* MAX���x */
#define DISTANCE_MAX		0x7FFFFFFF	/* �ړ�����MAX */
#define ACF_MAX_SZ			512			/* �ړ����σt�B���^�o�b�t�@�� */
#define FOLLOWUP_MAX_REF	32767		/* FollowUp���ő�ړ���[ref/cyc] */
#define MG_VREMBIT_MAX		24			/* MotionGenerator�]�葬�x���ZMAX�r�b�g�� */
#define MG_SPDDIV_RANGE		6			/* MotionGenerator�����_�ʒu�����ݒ�r�b�g�͈� */
#define MG_DEFAULT_SPDDIV	11			/* MotionGenerator�����_�r�b�g��(250us��) *//* <S006> */


#define BANK_NUM_MAX		16	/* �ő�o���N�� */
#define BANK_MEM_MAX		15	/* �ő僁���o�� */

/****************************************************************************
 Structure of the Motion Parameters
****************************************************************************/
typedef struct MG_PRM_ {
	LONG	accrate1;			/* �����萔�P ***.*[ref/cyc^2] */
	LONG	accrate2;			/* �����萔�Q ***.*[ref/cyc^2] */
	LONG	altspd_a;			/* �����萔�֑ؑ��x ***.*[ref/cyc] */
	LONG	decrate1;			/* �����萔�P ***.*[ref/cyc^2] */
	LONG	decrate2;			/* �����萔�Q ***.*[ref/cyc^2] */
	LONG	altspd_d;			/* �����萔�֑ؑ��x ***.*[ref/cyc] */
} MG_PRM;

typedef struct MC_BASE_PRM_ {
	LONG	MotionCycle;		/* ���[�V�����������[0.25us] */
//	BOOL	FullcCtrl;			/* Fullc�g�p */
//	BOOL	Linear;				/* ���j�A�g�p */
//	BOOL	ot_zstop;			/* OT����۸���� */
	LONG   	trefMode;			/* ���x���䎞�̃g���N�w�ߎg�p�I�� */
	LONG   	TrefModePos;		/* �ʒu���䎞�̃g���N�w�ߎg�p�I��(INTERPOLATE�n) */
	LONG   	TLimModePos;		/* �ʒu���䎞�̃g���N�����g�p�I��(POSING�n) */

	KSGAIN	PosingRateKx;		/* [10000ref/s^s]->[***.*ref/cyc^cyc]�������x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	PosingSpdKx;		/* [ref/s]->[***.*ref/cyc]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	PosingSpdInvKx;		/* [***.*ref/cyc]->[ref/s]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	fspdRefKx;			/* [ref/�ʐM����]->[ref/s]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	refToOsKx;			/* [�w�ߒP��/s]->[24bit/OS]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	osToRefKx;			/* [24bit/OS]->[�w�ߒP��/s]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	maxToOsKx;			/* [30bit/MAX]->[24bit/OS]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	osToMaxKx;			/* [24bit/OS]->[30bit/MAX]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	trqKx;				/* [24bit/MAX]->[%]�m�[�}���C�Y�萔 */
	KSGAIN	taccRateKx;			/* [(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	KSGAIN	taccRate7Kx;		/* [2^7*(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	VOID	(*SetCmdAccRate)();/* �����x�w�ߌv�Z�֐��|�C���^<Rev2.01> */

	LONG	vel_sft_exp;		/* ���x����\�����_�r�b�g��ACCFIL�p [bit] */
	LONG	vel_shift;			/* ���x����\�����_�r�b�g��MG�p [bit] */
	LONG	vel_shift_s;		/* ���x����\�����؎̃r�b�g�� [bit] */
	LONG	vel_max;			/* �ő呬�x(=2^(31-vel_shift))[ref/cyc] */
	LONG	seg_num;			/* �Z�O�����g������ */
	ULONG	seg_max_ref;		/* �Z�O�����g���͍ő� */
	ULONG	PosingSpdMax;		/* POSING�ő呬�x */
	ULONG	mgSpdMax;			/* MG��POSING�ő呬�x */
	MG_PRM	StopPrm;			/* ����~�p�����p�����[�^ */

	LONG	opMon[2];			/* �I�v�V�������j�^ *//* <S004> */
} MC_BASE_PRM;

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
	ULONG	psetWidth;			/* Pset�͈� */
	ULONG	nearWidth;			/* Near�͈� */
	ULONG	TdlyIn;				/* MPOS�␳���� */
	ULONG	SegNum;				/* ������(���[�V��������/�T�[�{����) */
} PSN_PRM;

typedef struct MT_SEQ_PRM_ {
	LONG	zretsp1;			/* ZRET ���۰����x1 [ref/sec] */
	LONG	zretsp2;			/* ZRET ���۰����x2 [ref/sec] */
	LONG	zret_dist;			/* ZRET�ŏI���s���� */
	LONG	ex_posing_dist;		/* EX_POSING�ŏI���s���� */
	ULONG	rampTimeOut;		/* ������~�^�C���A�E�g����[���[�V��������] */
	LONG	zret_dir;			/* ���_���A���� */
} MT_SEQ_PRM;

typedef	union ACC_MATB_PRM_ {
	ULONG	lTb;		/* �ړ����ω�(Total) */
	USHORT	wTb[2];		/* high word:�ړ����ω�, low word:�ړ����ϕ�ԉ�  */
} MATB_PRM;

typedef struct ACCFIL_PRM_ {
	LONG	expbias;			/* �w���������o�C�A�X */
	LONG	exptb;				/* �w�����������萔 */
	MATB_PRM mafil;				/* �ړ����ω� */
} ACF_PRM;

typedef struct OPTION_MAP_ {
	LONG	accfilBit;			/* �������t�B���^�I���r�b�g�ʒu */
	LONG	accfilBitMsk;		/* �������t�B���^�I���r�b�g�}�X�N */
	LONG	gselBit;			/* �Q�C���I���r�b�g�ʒu */
	LONG	gselBitMsk;			/* �Q�C���I���r�b�g�}�X�N */
	LONG	vppiBit;			/* P_PI�I���r�b�g�ʒu */
	LONG	vppiBitMsk;			/* P_PI�I���r�b�g�}�X�N */
	LONG	ppiclrBit;			/* �ʒu�ϕ��N���A�r�b�g�ʒu */
	LONG	ppiclrBitMsk;		/* �ʒu�ϕ��r�b�g�}�X�N */
	LONG	pclBit;				/* P_CL�r�b�g�ʒu */
	LONG	pclBitMsk;			/* P_CL�r�b�g�}�X�N */
	LONG	nclBit;				/* N_CL�r�b�g�ʒu */
	LONG	nclBitMsk;			/* N_CL�r�b�g�}�X�N */
	LONG	outpBit;			/* OutSignal�r�b�g�ʒu */
	LONG	outpBitMsk;			/* OutSignal�r�b�g�}�X�N */
	LONG	bankBit;			/* BankSel�r�b�g�ʒu */
	LONG	bankBitMsk;			/* BankSel�r�b�g�}�X�N */
	LONG	ltBit;				/* LtDisable�r�b�g�ʒu */
	LONG	ltBitMsk;			/* LtDisable�r�b�g�}�X�N */
} OPTION_MAP;

typedef struct INSIG_MAP_ {
	LONG	bitNo[8];
	LONG	bitsMsk;
} INSIG_MAP;

typedef struct LTSEQ_PRM_ {
	LONG	seqNum;				/* �V�[�P���X�M���� */
	LONG	loopCnt;			/* �A���񐔐ݒ� */
	LONG	ltSgnl[8];			/* ���b�`�M�� */
} LTSEQ_PRM;


typedef struct tagMTBANK {
	LONG		bankNum;				/* �o���N�� */
	LONG		memNum;					/* �����o�� */
	LONG		bankSel;				/* �o���N�Z���N�^���ݒl */
	LONG		cmdSel;					/* �o���N�Z���N�^�w�ߒl */
	ULONG		enaIdxBits;				/* ���ttable */
	struct {
		LONG	idx;					/* ���t�����o��`�e�[�u���ւ�Idx */
		LONG	highWd;					/* 4byte�����o����High Word�w�� */
	}memAttr[BANK_MEM_MAX];				/* �����o���t�e�[�u�� */
	MG_PRM		mgPrm[BANK_NUM_MAX];	/* MG�p�o���N�f�[�^�o�b�t�@ */
	ACF_PRM		acfPrm[BANK_NUM_MAX];	/* AccFilter�p�o���N�f�[�^�o�b�t�@ */
} MTBANK;


typedef struct MT_PRM_ {
	MC_BASE_PRM BasePrm;		/* ��{�p�����[�^ */
	MG_PRM		MgPrm;			/* MG���[�V�����p�����[�^ */
	MG_PRM		CmdRate;		/* �������w��<Rev2.01> */
	PSN_PRM		PsnPrm;			/* PositionManager�p�����[�^ */
	MT_SEQ_PRM	MtSeqPrm;		/* Motion�w�ߍ쐬�p�p�����[�^ */
	ACF_PRM		AccFilPrm;		/* �ʒu�w�߃t�B���^�p�����[�^ */
	OPTION_MAP	OptMap;			/* OPTION�w�߃r�b�g�ʒu�ݒ�p�����[�^ */
	INSIG_MAP	InSigMap;		/* Input Signal0-7���t */
#ifdef PRM_BANK_ENABLE
	MTBANK		Bnk;			/* Motion PRM Bank�f�[�^ */
#endif
	LTSEQ_PRM	LtPrm;
} MT_PRM;


typedef struct MTPRM_INI_ {
	LONG	EGearA;			/* �d�q�M�AA(����) */
	LONG	EGearB;			/* �d�q�M�AB(���q) */
	LONG	PgPuls;			/* PG�p���X��[puls/rev],[puls/pitch] */
	LONG	MaxTrq;			/* MAX�g���N[%] */
	LONG	OverSpd;		/* OS���x[rpm],[mm/s] */
	LONG	MaxSpd;			/* MAX���x[rpm],[mm/s] */
	LONG	ScalePitch;		/* �X�P�[���s�b�`[mm/pitch] */
	LONG	OverSpdExp;		/* OS���x�w�� */
	LONG	MaxSpdExp;		/* MAX���x�w�� */
	LONG	ScalePitchExp;	/* �X�P�[���s�b�`�w�� */
	LONG	trefMode;		/* �g���N����Option */
//	BOOL	ot_zstop;		/* OT����۸���� */
	BOOL	absPG;			/* ��޿PG�g�p */
	BOOL	FullcCtrl;		/* �ٸ۰���ސ��� */
	BOOL	LinearMotor;	/* ���j�A�g�p */

	LONG	Cycle;			/* ���[�V�����������[0.25us] */
	LONG	SpdDiv;			/* ����\ */
	ULONG	SegNum;			/* ��ԑ���Z�O�����g�� */
	LONG	OptEnable;		/* Option���t�L�� */
	LONG	OptMap[5];		/* Option���t��` */
	LONG	OutSigEnable;	/* Option���t�o�͐M���L��/���� */
	LONG	dlyTime;		/* �w�ߏo�͂���FB���͂܂ł̎��� */
	USHORT	SvCycle;		/* �T�[�{����(���[�V��������/�T�[�{����) */
} MTPRM_INI;


/****************************************************************************
 API's
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *prm, MTPRM_INI *iniPrm, PRMDATA *pprm_ptr );
LONG MPrmSetZretDir( MT_PRM *prm, BOOL data );
VOID MPrmCalcAccTimeRate(MT_PRM *prm, LONG Tspd, LONG Tacc, LONG Tdec);
LONG MPrmSetAccAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data);
LONG MPrmSetDecAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data);
LONG MPrmSetExpFilBias(MT_PRM *prm, ACF_PRM *acfPrm, LONG bias);
LONG MPrmSetExpFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG exptb);
LONG MPrmSetMaFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG matb);
LONG MPrmSetInSigMap(MT_PRM* prm, LONG data);

LONG MPrmSetAccRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);
LONG MPrmSetAccRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);
LONG MPrmSetDecRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);
LONG MPrmSetDecRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);


/* ����֐��}�N����` */

#define		MOTOIN_PRM_H_
#endif /* MOTOIN_PRM_H_ */

