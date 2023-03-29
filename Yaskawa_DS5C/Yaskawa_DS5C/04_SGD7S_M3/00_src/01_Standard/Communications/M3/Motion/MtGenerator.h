/****************************************************************************
Description	: MECHATROLINK Motion Generator
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef MOTOIN_GEN_H_

#include "MtPRM.h"

/****************************************************************************
 Defines
****************************************************************************/
#define MG_HOLD_CTRL1	0	/* ������~1(�ʏ�p�����[�^�g�p) */
#define MG_HOLD_CTRL2	1	/* ������~2(��~�p�p�����[�^�g�p) */

#define VELREM_BIT_MSK	((1<<MG_VREMBIT_MAX)-1)
#define VELREM_CARRYUP	(1<<MG_VREMBIT_MAX)

/* motoin type */
enum MG_MTYPE {
	MNONE = 0,			/* 0:NOP���[�V���� */
	MINTERPOLATE,		/* 1:INTERPOLATE */
	MPOSING,			/* 2:POSING */ 
	MHOLD,				/* 3:HOLD */
	MFOLLOW,			/* 4:FOLLOW UP */
	MVELCTRL,			/* 5:VELCTRL */
	MVELCTRL_WITHACCR, 	/* 6:VELCTRL with Accr */
	MTRQCTRL,			/* 7:TRQCTRL */
	MSPOSING,			/* 8:SPOSING */
	MSHOLD,				/* 9:SHOLD */	
};


/* SPOSING���샂�[�h */
#define SPOSING_INI_MODE		0	/* SPOSING���������[�h 			*/
#define SPOSING_ACC_MODE		1	/* SPOSING�����A�葬���[�h 		*/
#define SPOSING_DEC_MODE1		2	/* SPOSING�������[�h			*/
#define SPOSING_DEC_MODE2		3	/* SPOSING�������[�h			*/

/* S���������䗦 */
#define S_RATIO_MAX	50
#define S_RATIO_MIN  0

/****************************************************************************
 Structure of the Motion Generator
****************************************************************************/
typedef struct MG_POSWK_ {
	LONG	err_dist;			/* �c�����i�w�߈ʒu����ڕW�ʒu�܂ł̋����j ****.[ref] */
	ULONG	err_dist0;			/* �c�����i�������j�i�w�߈ʒu����ڕW�ʒu�܂ł̋����j .**[ref] */
	ULONG	sd_dist0;			/* ���������i�������j�i���ݎw�ߑ��x�����~����܂ł̌��������j .**[ref] */
	LONG	sd_dist;			/* ���������i�������j�i���ݎw�ߑ��x�����~����܂ł̌��������j ****[ref] */
	LONG	sd_rem;				/* �������[���i���������Ǝc�����̍��Ŕ������镥���o���[���j ***.*[ref/cyc] */
	LONG	spdrem1;			/* �P�i�ڌ������̑��x�[�������o�� ***.*[ref/cyc] */
	LONG	spdrem2;			/* �Q�i�ڌ������̑��x�[�������o�� ***.*[ref/cyc] */
	LONG	spd;				/* ���ݎw�ߑ��x ***.*[ref/cyc] */
	LONG	tspd;				/* �ڕW���x ***.*[ref/cyc] */
	LONG	lspd;				/* �O��w�ߑ��x ***.*[ref/cyc] */
	LONG	bspd;				/* �w�ߏo�͑��x(dpos) ***.*[ref/cyc] */
} MG_POSWK;


//SPOSING�p���[�N
typedef struct MG_S_POSWK_ {
	LONG	ErrorDistance;		/* �c�����i�w�߈ʒu����ڕW�ʒu�܂ł̋����j ****.[ref] */
	ULONG	ErrorDistance0;		/* �c�����i�������j�i�w�߈ʒu����ڕW�ʒu�܂ł̋����j .**[ref] */
	LONG	StopDistance;		/* ���������i�������j�i���ݎw�ߑ��x�����~����܂ł̌��������j ****[ref] */
	ULONG	StopDistance0;		/* ���������i�������j�i���ݎw�ߑ��x�����~����܂ł̌��������j .**[ref] */
//	LONG	StopDistanceRem;	/* �������[���i���������Ǝc�����̍��Ŕ������镥���o���[���j ***.*[ref/cyc] */
	LONG	CurrentSpd;			/* ���ݎw�ߑ��x ***.*[ref/cyc] */
	LONG	TargetSpd;			/* �ڕW���x ***.*[ref/cyc] */
	LONG	LastSpd;			/* �O��w�ߑ��x ***.*[ref/cyc] */
	LONG	Segment;			/* �w�ߏo�͑��x(dpos) ***.*[ref/cyc] */
} MG_S_POSWK;


typedef struct MG_INOUT_ {
	LONG	Mtype;				/* Motion Type */
	LONG	Mtype_cancel;		/* Motion Type(�R�}���h�L�����Z���O) */	
	LONG	CtrlModeLst;		/* �O�񐧌䃂�[�h */
	LONG	Ctrl;				/* Motion����Option */
	LONG	DposIn;				/* �w�߈ړ���[ref] */
	LONG	Tspd;				/* POSING�p�ڕW���x[ref/s] */
	LONG	Vref;				/* ���x�w��[30bit/MAX] */
	LONG	Tref;				/* �g���N�w��[30bit/MAX] */
	LONG	FbSpeed;			/* FB���x[24bit/OS](����ؑ֎��̎w�ߗp) */

	LONG	DposOut;			/* �o�͈ړ���[ref](Vel->Pos) */
	LONG	VrefOut;			/* �o�͑��x�w��[24bit/OS] */
	LONG	TrefOut;			/* �o�̓g���N�w��[24bit/MAX] */
} MG_INOUT;


/* SPOSING�p�ϐ� */
typedef struct MG_SPOSING_ {
	LONG	State;				/* SPOSING��ԕϐ�						*/
	LONG	LastDposIn;			/* SPOSING�O��c����[ref]				*/
	LONG	LastTargetSpd;		/* SPOSING�O��ڕW���x[ref/cyc]			*/
	LONG	LastTargetPosition;	/* SPOSING�O��ڕW�ʒu[ref/cyc]			*/

	LONG	CurrentSratio;		/* S�����������Ԕ䗦[%]				*/
	LONG	InitAcc;			/* SPOSING�����������x ***.*[ref/cyc^2]	*/
	LONG	InitDec;			/* SPOSING�����������x ***.*[ref/cyc^2]	*/
	LONG	dJerkAcc;			/* Jerk(����) ***.*[ref/cyc^3]			*/
	LONG	dJerkDec;			/* Jerk(����) ***.*[ref/cyc^3]			*/
	LONG	JerkAccRate;		/* SPOSING�����x ***.*[ref/cyc^2]		*/
	LONG	JerkDecRate;		/* SPOSING�����x ***.*[ref/cyc^2]		*/
	LONG	CurrentAccRate;		/* �����x�w�� ***.*[ref/cyc^2]			*/
	LONG	CurrentDecRate;		/* �����x�w�� ***.*[ref/cyc^2]			*/
	LONG	AltSpd_a1;			/* �����؂�ւ����x1 ***.*[ref/cyc]		*/
	LONG	AltSpd_a2;			/* �����؂�ւ����x2 ***.*[ref/cyc]		*/
	LONG	AltSpd_d1;			/* �����؂�ւ����x1 ***.*[ref/cyc]		*/
	LONG	AltSpd_d2;			/* �����؂�ւ����x2 ***.*[ref/cyc]		*/
	LONG	InitSpdAcc;			/* �����J�n���x ***.*[ref/cyc]			*/
	LONG	InitSpdDec;			/* �����J�n���x ***.*[ref/cyc]			*/
	LONG	EndSpdDec;			/* �����I�����x ***.*[ref/cyc]			*/
	LONG	TargetSpd;			/* �ڕW���x ***.*[ref/cyc]				*/

	LONG	SpdRemAcc;			/* ���������x�[�� ***.*[ref/cyc]		*/
	LONG	SpdRemDec;			/* ���������x�[�� ***.*[ref/cyc]	    */
	LONG	dSpdRemAcc;			/* ���x�[���������x(����) ***.*[ref/cyc]*/
	LONG	dSpdRemDec;			/* ���x�[���������x(����) ***.*[ref/cyc]*/

	LONG	StopDistRem;		/* �c�����[�� ***.*[ref]				*/
	LONG	dStopDistRem;		/* �c�����[�������� ***.*[ref/cyc]		*/
	LONG	jStopDistRem;		/* �c�����[��������	***.*[ref/cyc]		*/
	LONG	TmpDecVal;			/* �����[�����������̂��߂̈ꎞ�ϐ�		*/
	LONG	DecCnt;				/* �����[�����������̂��߂̌�����		*/
	LONG	rStopDistRem;		/* �����[��������������ɗ]��			*/
	LONG	rdStopDistRem;		/* ����ɗ]��̕�����					*/

	LONG	LastSegment;		/* �O��Z�O�����g(���j�^�p)[ref/cyc^2]	*/
	LONG	AccMonitor;			/* �������x(���j�^�p)[ref/cyc^2]	*/
} MG_SPOSING;


typedef struct MG_HNDL_ {
	BOOL			cMinusDir;		/* ���ݕ����o������ */
	BOOL			tMinusDir;		/* ���݂̎w�߈ʒu����ڕW�ʒu�̕��� */
	ULONG			dpos0;			/* DPOS�̏����� for POSING�̎w�ߑ��x�i�������j�̐ώZ */
	LONG			dposRem;		/* �`�������ړ���(dpos)�]��         */
	LONG			Cspd;			/* ���ݎw�ߑ��x */
	LONG			VrefSum[2];		/* ���x�w�ߐώZ�l[24bit/OS](���x����) */
	MC_BASE_PRM		*basePrm;		/* MG��{�p�����[�^ */
	MG_PRM			*prm;			/* Motion�p�����[�^ */
	MG_SPOSING		sposing;		/* SPOSING�p�p�����[�^ 			*/
} MG_HNDL;

/****************************************************************************
 API's
****************************************************************************/
/* MG_HNDL�̏��������� */
VOID MtgInit( MG_HNDL *hdl, MC_BASE_PRM *bPrm, MG_PRM *prm );
/* ���[�V�����������C������ */
BOOL MtgGenerate(MG_HNDL* hdl, MG_INOUT* inOut);
/* ���������擾 */
LONG MtgGetDctDistance( MG_HNDL *hdl, LONG spd, LONG ctrl );
/* ���������擾(S��) */
LONG MtgGetDctSpDistance( MG_HNDL *hdl, LONG spd, LONG ctrl );
/* �g���N/���x���䎞 ���ݎw�ߑ��x�ݒ� */
LONG MtgSetCspd( MG_HNDL *hdl, LONG spd );
/* SPOSING���[�V�������� */
BOOL MtgSposing( MG_HNDL *hdl, MG_INOUT *inOut );
/* SHOLD���[�V�������� */
BOOL MtgShold( MG_HNDL *hdl, MG_INOUT *inOut );
/* S�����������v�Z */
BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
							LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 );
/* SPOSING �������AS���p�����[�^�Z�b�g */
void mtgSpSetPrm(MG_SPOSING* sposing, MG_PRM* prm, LONG TargetSpd);


#define MOTOIN_GEN_H_
#endif /* MOTOIN_GEN_H_ */


