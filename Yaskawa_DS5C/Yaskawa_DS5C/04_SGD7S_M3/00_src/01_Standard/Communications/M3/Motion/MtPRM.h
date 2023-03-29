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
#ifndef MOTOIN_PRM_H_

#include "UnitPrmStruct.h"


/****************************************************************************/
/* ���[�V��������@�\SW�F �����ɂ���ꍇ�̓R�����g�ɂ���					*/
/****************************************************************************/
#define BASE_SHIFT					9			/* �ׂ��扉�Z�p��{�V�t�g�� */

#define MAX_TRQ24					0x1000000		/* MAXTRQ */
#define MAX_SPD24					0x1000000		/* MAX���x */
#define DISTANCE_MAX				0x7FFFFFFF		/* �ړ�����MAX */
#define ACF_MAX_SZ					512				/* �ړ����σt�B���^�o�b�t�@�� */
#define FOLLOWUP_MAX_REF			32767			/* FollowUp���ő�ړ���[ref/cyc] */
#define MG_VREMBIT_MAX				24				/* MotionGenerator�]�葬�x���ZMAX�r�b�g�� */
#define MG_SPDDIV_RANGE				6				/* MotionGenerator�����_�ʒu�����ݒ�r�b�g�͈� */
#define MG_DEFAULT_SPDDIV			11				/* MotionGenerator�����_�r�b�g��(250us��) */

#define ZRET_DIST_DIR_IS_ABSOLUTE	0				/* ���_���A�ŏI���s���� ��Ε����w�� */
#define ZRET_DIST_DIR_IS_RELATIVE	1				/* ���_���A�ŏI���s���� ���Ε����w�� */
#define ZRET_LATCH_IS_CANCEL		0				/* ZRET�R�}���h���̃R�}���h�̐؂�ւ��ɂ�郉�b�`�L�����Z������̗L�� */
#define ZRET_LATCH_IS_NOCANCEL		1				/* ZRET�R�}���h���̃R�}���h�̐؂�ւ��ɂ�郉�b�`�L�����Z������̖��� */

#define BANK_NUM_MAX	16							/* �ő�o���N�� */
#define BANK_MEM_MAX	15							/* �ő僁���o�� */


/****************************************************************************/
/* Vendor Specifics I/O����t����`											*/
/****************************************************************************//* <S1C0> */
//#define VENDOR_IOPRM_NUM	4						/* SVCMD_IO(�o��)VendorSpecificsI/O����t���p�����[�^��		*/
#define VENDOR_IOBIT_NUM	(VENDOR_IOPRM_NUM * 4)	/* SVCMD_IO(�o��)VendorSpecificsI/O����t���Ώۃr�b�g��		*/
#define VENDOR_IOBIT_OFFSET	15						/* SVCMD_IO(�o��)VendorSpecificsI/O�̃I�t�Z�b�g��			*/

#define VENDOR_IOBIT_BANK	0						/* SVCMD_IO(�o��)VendorSpecificsI/O.Bank					*/
#define VENDOR_IOBIT_SO1	1						/* SVCMD_IO(�o��)VendorSpecificsI/O.SO1						*/
#define VENDOR_IOBIT_SO2	2						/* SVCMD_IO(�o��)VendorSpecificsI/O.SO2						*/
#define VENDOR_IOBIT_SO3	3						/* SVCMD_IO(�o��)VendorSpecificsI/O.SO3						*/
#define VENDOR_IOBIT_SO4	4						/* SVCMD_IO(�o��)VendorSpecificsI/O.SO4	(for SGD7W)			*/
#define VENDOR_IOBIT_SO5	5						/* SVCMD_IO(�o��)VendorSpecificsI/O.SO5	(for SGD7W)			*/
#define VENDOR_IOBIT_JSEL	6						/* SVCMD_IO(�o��)VendorSpecificsI/O.JSEL					*/
#define VENDOR_IOBIT_SRATIO	8						/* SVCMD_IO(�o��)VendorSpecificsI/O.SRATIO					*/
#define VENDOR_IOBIT_LTREQ1	9						/* SVCMD_IO(�o��)VendorSpecificsI/O.LT_REQ_EX1 (for FT)		*/
#define VENDOR_IOBIT_LTREQ2	10						/* SVCMD_IO(�o��)VendorSpecificsI/O.LT_REQ_EX2 (for FT)		*/
#define VENDOR_IOBIT_LTREQ3	11						/* SVCMD_IO(�o��)VendorSpecificsI/O.LT_REQ_EX3 (for FT)		*/


/****************************************************************************
 Structure of the Motion Parameters
****************************************************************************/
typedef struct MG_PRM_ {
	LONG	accrate1;				/* �����萔�P ***.*[ref/cyc^2] */
	LONG	accrate2;				/* �����萔�Q ***.*[ref/cyc^2] */
	LONG	altspd_a;				/* �����萔�֑ؑ��x ***.*[ref/cyc] */
	LONG	decrate1;				/* �����萔�P ***.*[ref/cyc^2] */
	LONG	decrate2;				/* �����萔�Q ***.*[ref/cyc^2] */
	LONG	altspd_d;				/* �����萔�֑ؑ��x ***.*[ref/cyc] */
	LONG	Sratio;					/* S���������䗦(SPOSING�p)[%] */
	
	LONG	accr_vel;				/* �����x(���x�w�ߗp) ***.*[ref/cyc^2] */
	LONG	decr_vel;				/* �����x(���x�w�ߗp) ***.*[ref/cyc^2] */
} MG_PRM;


typedef struct M3CONV_UNIT_ {
	LONG	m3SpdSelUnit;			/* ���x�P�ʑI��		0�`4							*/
	LONG	m3AccSelUnit;			/* �����x�P�ʑI��	0�`1							*/
	LONG	m3TrqSelUnit;			/* �g���N�P�ʑI��	0�`2							*/
	LONG	m3PosSelUnit;			/* �ʒu		�ϊ��Ȃ�								*/

	KSGAIN	m3SpdRefToUnit;			/* m3SpdSelUnit=0: �ϊ��Ȃ�										*/
									/* m3SpdSelUnit=1: �w�ߒP��/sec			--> �w�ߒP��/min		*/
									/* m3SpdSelUnit=2: �w�ߒP��/sec			--> ��i���x�ɑ΂���%	*/
									/* m3SpdSelUnit=3: �w�ߒP��/sec			--> RPM					*/
									/* m3SpdSelUnit=4: �w�ߒP��/sec			--> �ő呬�x/40000000h	*/
	KSGAIN	m3SpdUnitToRef;			/* m3SpdSelUnit=0: �ϊ��Ȃ�										*/
									/* m3SpdSelUnit=1: �w�ߒP��/min			--> �w�ߒP��/sec		*/
									/* m3SpdSelUnit=2: ��i���x�ɑ΂���%	--> �w�ߒP��/sec		*/
									/* m3SpdSelUnit=3: RPM					--> �w�ߒP��/sec		*/
									/* m3SpdSelUnit=4: �ő呬�x/40000000h	--> �w�ߒP��/sec		*/
	KSGAIN	m3SpdRpmToUnit;			/* m3SpdSelUnit=0: RPM					--> �w�ߒP��/sec		*/
									/* m3SpdSelUnit=1: RPM					--> �w�ߒP��/min		*/
									/* m3SpdSelUnit=2: RPM					--> ��i���x�ɑ΂���%	*/
									/* m3SpdSelUnit=3: �ϊ��Ȃ�										*/
									/* m3SpdSelUnit=4: RPM					--> �ő呬�x/40000000h	*/
	KSGAIN	m3SpdUnitToMax;			/* m3SpdSelUnit=0: �w�ߒP��/sec			--> �ő呬�x/40000000h	*/
									/* m3SpdSelUnit=1: �w�ߒP��/min			--> �ő呬�x/40000000h	*/
									/* m3SpdSelUnit=2: ��i���x�ɑ΂���%	--> �ő呬�x/40000000h	*/
									/* m3SpdSelUnit=3: RPM					--> �ő呬�x/40000000h	*/
									/* m3SpdSelUnit=4: �ϊ��Ȃ�										*/
	KSGAIN	m3SpdMaxToUnit;			/* m3SpdSelUnit=0: �ő呬�x/40000000h	--> �w�ߒP��/sec		*/
									/* m3SpdSelUnit=1: �ő呬�x/40000000h	--> �w�ߒP��/min		*/
									/* m3SpdSelUnit=2: �ő呬�x/40000000h	--> ��i���x�ɑ΂���%	*/
									/* m3SpdSelUnit=3: �ő呬�x/40000000h	--> RPM					*/
									/* m3SpdSelUnit=4: �ϊ��Ȃ�										*/
	KSGAIN	m3Spd24OsToUnit;		/* m3SpdSelUnit=0: 24BitOs				--> �w�ߒP��/sec		*/
									/* m3SpdSelUnit=1: 24BitOs				--> �w�ߒP��/min		*/
									/* m3SpdSelUnit=2: 24BitOs				--> ��i���x�ɑ΂���%	*/
									/* m3SpdSelUnit=3: 24BitOs				--> RPM					*/
									/* m3SpdSelUnit=4: 24BitOs				--> �ő呬�x/40000000h	*/

	KSGAIN	m3PosUnitToRef;			/* m3PosSelUnit=0: �ϊ��Ȃ�										*/
	KSGAIN	m3PosRefToUnit;			/* m3PosSelUnit=0: �ϊ��Ȃ�										*/

	KSGAIN	m3AccUnitToCyc2;		/* m3AccSelUnit=0: �w�ߒP��/sec^2	 --> **.*** �w�ߒP��/cyc^2	*/
									/* m3AccSelUnit=1: ��������(ms)		 --> �����x(�w�ߒP��/sec^2)	*/
	KSGAIN	m3AccUnitToOs;			/* m3AccSelUnit=0: �����x(ref/sec^2) --> OS���x/cyc				*/
									/* m3AccSelUnit=1: ��������(ms)		 --> OS���x/cyc				*/

	KSGAIN	m3TrqMaxToUnit;			/* m3TrqSelUnit=0: �ő�g���N/40000000h --> Nm(N)				*/
									/* m3TrqSelUnit=1: �ő�g���N/40000000h --> ��i�g���N�ɑ΂���%	*/
									/* m3TrqSelUnit=2: �ϊ��Ȃ�										*/
	KSGAIN	m3TrqUnitToMax;			/* m3TrqSelUnit=0: Nm(N)			   --> �ő�g���N/40000000h	*/
									/* m3TrqSelUnit=1: ��i�g���N�ɑ΂���% --> �ő�g���N/40000000h	*/
									/* m3TrqSelUnit=2: �ϊ��Ȃ�										*/
	KSGAIN	m3ReftoRpm;				/* �w�ߒP��/s �� RPM											*/
	KSGAIN	m3RpmtoRef;				/* RPM �� �w�ߒP��/s											*/
} M3CONV_UNIT;


typedef struct MC_BASE_PRM_ {
	LONG	MotionCycle;			/* ���[�V�����������[0.25us] */
//	LONG	trefMode;				/* ���x���䎞�̃g���N�w�ߎg�p�I�� */
	LONG	TrefModePos;			/* �ʒu���䎞�̃g���N�w�ߎg�p�I��(INTERPOLATE�n) */
//	LONG	TLimModePos;			/* �ʒu���䎞�̃g���N�����g�p�I��(POSING�n) */
	LONG	vLimModeTrq;			/* �g���N���䎞�̑��x�����g�p�I�� */

	KSGAIN	PosingRateKx;			/* [10000ref/s^s]->[***.*ref/cyc^cyc]�������x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	PosingSpdKx;			/* [ref/s]->[***.*ref/cyc]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	PosingSpdInvKx;			/* [***.*ref/cyc]->[ref/s]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	fspdRefKx;				/* [ref/�ʐM����]->[ref/s]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	refToOsKx;				/* [�w�ߒP��/s]->[24bit/OS]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	osToRefKx;				/* [24bit/OS]->[�w�ߒP��/s]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	maxToOsKx;				/* [30bit/MAX]->[24bit/OS]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	osToMaxKx;				/* [24bit/OS]->[30bit/MAX]���x�ϊ��m�[�}���C�Y�萔 */
	KSGAIN	trqKx;					/* [24bit/MAX]->[%]�m�[�}���C�Y�萔 */
//	KSGAIN	taccRateKx;				/* [(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
//	KSGAIN	taccRate7Kx;			/* [2^7*(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	VOID	(*SetCmdAccRate)();		/* �����x�w�ߌv�Z�֐��|�C���^<Rev2.01> */

	LONG	vel_sft_exp;			/* ���x����\�����_�r�b�g��ACCFIL�p [bit] */
	LONG	vel_shift;				/* ���x����\�����_�r�b�g��MG�p [bit] */
	LONG	vel_shift_s;			/* ���x����\�����؎̃r�b�g�� [bit] */
	LONG	vel_max;				/* �ő呬�x(=2^(31-vel_shift))[ref/cyc] */
	LONG	seg_num;				/* �Z�O�����g������ */
	ULONG	seg_max_ref;			/* �Z�O�����g���͍ő� */
	ULONG	PosingSpdMax;			/* POSING�ő呬�x */
	ULONG	mgSpdMax;				/* MG��POSING�ő呬�x */
	ULONG	osVel;					/* OS���x[ref/s] */
	MG_PRM	StopPrm;				/* ����~�p�����p�����[�^ */

	M3CONV_UNIT m3ConvUnit;			/* �P�ʕϊ�	*/
	LONG		m3_selMon1;			/* ���ʃ��j�^�I��1 */
	LONG		m3_selMon2;			/* ���ʃ��j�^�I��2 */
	LONG	opMon[2];				/* �I�v�V�������j�^ */
} MC_BASE_PRM;


typedef struct PSN_PRM_ {
	BOOL	sotEnableP;				/* ���]���\�t�g���~�b�g�L�� */
	BOOL	sotEnableN;				/* �t�]���\�t�g���~�b�g�L�� */
	BOOL	tposLimitEnable;		/* �w�߃\�t�g���~�b�g�L�� */
	BOOL	absPG;					/* ��޿PG�g�p */
	LONG	absOfsset;				/* ��ΒlPG�̾�Ēl(Pn803) */
	LONG	softLimit_P;			/* ���]���\�t�g���~�b�g�l */
	LONG	softLimit_N;			/* �t�]���\�t�g���~�b�g�l */
	LONG	ltRangeP;				/* Pn820:�������b�`�]�[�� */
	LONG	ltRangeN;				/* Pn822:�������b�`�]�[�� */
	ULONG	zPointWidth;			/* ���_�ʒu�͈� */
	ULONG	psetWidth;				/* Pset�͈� */
	ULONG	nearWidth;				/* Near�͈� */
	LONG	TdlyIn;					/* MPOS�␳���� */
	LONG	SegNum;					/* ������(���[�V��������/�T�[�{����) */
} PSN_PRM;


typedef struct MT_SEQ_PRM_ {
	LONG	zretsp1;				/* ZRET ���۰����x1 [ref/sec] */
	LONG	zretsp2;				/* ZRET ���۰����x2 [ref/sec] */
	LONG	zret_dist;				/* ZRET�ŏI���s���� */
	LONG	ex_posing_dist;			/* EX_POSING�ŏI���s���� */
	ULONG	rampTimeOut;			/* ������~�^�C���A�E�g����[���[�V��������] */
	LONG	zret_dir;				/* ���_���A���� */
	LONG	zretsp1_org;			/* ZRET ���۰����x1�i�p�����[�^�ݒ�l�j[ref/sec] */
	LONG	zretsp2_org;			/* ZRET ���۰����x2�i�p�����[�^�ݒ�l�j[ref/sec] */
} MT_SEQ_PRM;


typedef	union ACC_MATB_PRM_ {
	ULONG	lTb;					/* �ړ����ω�(Total) */
	USHORT	wTb[2];					/* high word:�ړ����ω�, low word:�ړ����ϕ�ԉ�  */
} MATB_PRM;


typedef struct ACCFIL_PRM_ {
	LONG	expbias;				/* �w���������o�C�A�X */
	LONG	exptb;					/* �w�����������萔 */
	MATB_PRM mafil;					/* �ړ����ω� */
} ACF_PRM;


typedef struct OPTION_MAP_ {
	LONG	accfilBit;				/* �������t�B���^�I���r�b�g�ʒu */
	LONG	accfilBitMsk;			/* �������t�B���^�I���r�b�g�}�X�N */
	LONG	gselBit;				/* �Q�C���I���r�b�g�ʒu */
	LONG	gselBitMsk;				/* �Q�C���I���r�b�g�}�X�N */
	LONG	vppiBit;				/* P_PI�I���r�b�g�ʒu */
	LONG	vppiBitMsk;				/* P_PI�I���r�b�g�}�X�N */
	LONG	ppiclrBit;				/* �ʒu�ϕ��N���A�r�b�g�ʒu */
	LONG	ppiclrBitMsk;			/* �ʒu�ϕ��r�b�g�}�X�N */
	LONG	pclBit;					/* P_CL�r�b�g�ʒu */
	LONG	pclBitMsk;				/* P_CL�r�b�g�}�X�N */
	LONG	nclBit;					/* N_CL�r�b�g�ʒu */
	LONG	nclBitMsk;				/* N_CL�r�b�g�}�X�N */
	LONG	outpBit;				/* OutSignal�r�b�g�ʒu */
	LONG	outpBitMsk;				/* OutSignal�r�b�g�}�X�N */
	LONG	bankBit;				/* BankSel�r�b�g�ʒu */
	LONG	bankBitMsk;				/* BankSel�r�b�g�}�X�N */
	LONG	ltBit;					/* LtDisable�r�b�g�ʒu */
	LONG	ltBitMsk;				/* LtDisable�r�b�g�}�X�N */
} OPTION_MAP;


typedef struct INSIG_MAP_ {
	LONG	bitNo[8];
	LONG	bitsMsk;
} INSIG_MAP;


typedef struct INSIG_M3_MAP_ {
	LONG	bitNo[15];				/* IO1-6���̓r�b�g�ʒu */
	LONG	bitsMsk;				/* �g���\�� (14bit�K�v) */
}INSIG_M3_MAP;

#if ( CCSW_SFTID == SFTID_EXIST )
typedef struct OUTSIG_M3_MAP_ {
	UCHAR	bitNo[16];			/* Out1-16�o�͗̓r�b�g�ʒu */
	ULONG	bitsMsk;			/* ����18bit���g�p���� */
}OUTSIG_M3_MAP;
#endif

typedef struct SVCMD_IO_OUT_MAP_ {/* <S1C0> */
	UCHAR	bitNo;					/* SVCMD_IO(�o��)16-31�r�b�g�ʒu */
	UCHAR	bitsMsk;				/* �}�X�N */
}SVCMD_IO_OUT_MAP;

typedef struct LTSEQ_PRM_ {
	LONG	seqNum;					/* �V�[�P���X�M���� */
	LONG	loopCnt;				/* �A���񐔐ݒ� */
	LONG	ltSgnl[8];				/* ���b�`�M�� */
} LTSEQ_PRM;


typedef struct PROF_PRM_ {
	LONG	zretDistDir;			/* ���_���A�ŏI���s�������� */
	LONG	zretLtCancel;			/* ZRET�R�}���h���̃R�}���h�̐؂�ւ��ɂ�郉�b�`�L�����Z������̗L���^���� */
	BOOL	initPgPos64Enable;		/* �����G���R�[�_�ʒu�i64bit�j�̗L���^���� */

	LONG	OutSigEnable;			/* Option���t�o�͐M���L��/���� *//* <S14D> */
} PROF_PRM;


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
	MC_BASE_PRM BasePrm;			/* ��{�p�����[�^ */
	MG_PRM		MgPrm;				/* MG���[�V�����p�����[�^ */
	MG_PRM		CmdRate;			/* �������w��<Rev2.01> */
	PSN_PRM		PsnPrm;				/* PositionManager�p�����[�^ */
	MT_SEQ_PRM	MtSeqPrm;			/* Motion�w�ߍ쐬�p�p�����[�^ */
	ACF_PRM		AccFilPrm;			/* �ʒu�w�߃t�B���^�p�����[�^ */
	OPTION_MAP	OptMap;				/* OPTION�w�߃r�b�g�ʒu�ݒ�p�����[�^ */
	INSIG_MAP	InSigMap;			/* Input Signal0-7���t */
	INSIG_M3_MAP InSigMonM3;		/* Input Signal0-7���t&�Z�[�t�e�B�I�v�V���� SVCMD_IO monitor�p */
	SVCMD_IO_OUT_MAP SvCmdIoOutMap[VENDOR_IOBIT_NUM];	/* SVCMD_IO(�o��)�C�ӊ���t���p *//* <S1C0> */
	MTBANK		Bnk;				/* Motion PRM Bank�f�[�^ */
	LTSEQ_PRM	LtPrm;
	PROF_PRM	ProfPrm;			/* �v���t�@�C���Ɉˑ������p�����[�^ */
#if ( CCSW_SFTID == SFTID_EXIST )											/*<V889>Add:Start*/
	OUTSIG_M3_MAP OutSigMonM3;	/* Output Signal0-7���t&�Z�[�t�e�B�I�v�V���� SVCMD_IO monitor�p */
#endif																		/*<V889>Add:End*/
} MT_PRM;


typedef struct MTPRM_INI_ {
	LONG	EGearA;					/* �d�q�M�AA(����) */
	LONG	EGearB;					/* �d�q�M�AB(���q) */
	LONG	PgPuls;					/* PG�p���X��[puls/rev],[puls/pitch] */
	LONG	MaxTrq;					/* MAX�g���N[%] */
	LONG	OverSpd;				/* OS���x[rpm],[mm/s] */
	LONG	MaxSpd;					/* MAX���x[rpm],[mm/s] */
	LONG	ScalePitch;				/* �X�P�[���s�b�`[mm/pitch] */
	LONG	OverSpdExp;				/* OS���x�w�� */
	LONG	MaxSpdExp;				/* MAX���x�w�� */
	LONG	ScalePitchExp;			/* �X�P�[���s�b�`�w�� */
	LONG	trefMode;				/* �g���N����Option */
	BOOL	absPG;					/* �A�u�\PG�g�p */
	BOOL	FullcCtrl;				/* �ٸ۰���ސ��� */
	BOOL	LinearMotor;			/* ��] or ���j�A */

	LONG	Cycle;					/* ���[�V�����������[0.25us] */
	LONG	SpdDiv;					/* ����\ */
	ULONG	SegNum;					/* ��ԑ���Z�O�����g�� */
	LONG	OptEnable;				/* Option���t�L�� */
	LONG	OptMap[5];				/* Option���t��` */
	LONG	OutSigEnable;			/* Option���t�o�͐M���L��/���� */
	LONG	dlyTime;				/* �w�ߏo�͂���FB���͂܂ł̎��� */
	USHORT	SvCycle;				/* �T�[�{����(���[�V��������/�T�[�{����) */
	
	LONG	RatTrqExp;				/* ���[�^�g���N�w�� */
	LONG	RatSpd;					/* ��i���x[rpm, mm/s] */
	LONG	RatTrq;					/* ��i�g���N[Nm/N] */
} MTPRM_INI;



/****************************************************************************
 API's
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *prm, MTPRM_INI *iniPrm, PRMDATA *pprm_ptr );
VOID MPrmCalcSvCmdIo(MT_PRM *mtPrm, PRMDATA *Prm );/* <S1C0> */
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

void MPrmSetInSigMonitor(MT_PRM *prm, USHORT data, UCHAR IoNum);
#if ( CCSW_SFTID == SFTID_EXIST )
void MPrmSetOutSigMonitor(MT_PRM* prm, USHORT data, UCHAR IoNum);
#endif




#endif /* MOTOIN_PRM_H_ */
#define MOTOIN_PRM_H_



