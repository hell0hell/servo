/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Generator �w�b�_�t�@�C��										*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTOIN_GEN_H_

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
	MNONE = 0,		/* 0:NOP���[�V���� */
	MINTERPOLATE,	/* 1:INTERPOLATE */
	MPOSING,		/* 2:POSING */ 
	MHOLD,			/* 3:HOLD */
	MFOLLOW,		/* 4:FOLLOW UP */
	MVELCTRL,		/* 5:VELCTRL */
	MTRQCTRL,		/* 6:TRQCTRL */
};


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

typedef struct MG_INOUT_ {
	LONG	Mtype;			/* Motion Type */
	LONG	Ctrl;			/* Motion����Option */
	LONG	DposIn;			/* �w�߈ړ���[ref] */
	LONG	Tspd;			/* POSING�p�ڕW���x[ref/s] */
	LONG	Vref;			/* ���x�w��[30bit/MAX] */
	LONG	Tref;			/* �g���N�w��[30bit/MAX] */

	LONG	DposOut;		/* �o�͈ړ���[ref](Vel->Pos) */
	LONG	VrefOut;		/* �o�͑��x�w��[24bit/OS] */
	LONG	TrefOut;		/* �o�̓g���N�w��[24bit/MAX] */
} MG_INOUT;


typedef struct MG_HNDL_ {
	BOOL			cMinusDir;	/* ���ݕ����o������ */
	BOOL			tMinusDir;	/* ���݂̎w�߈ʒu����ڕW�ʒu�̕��� */
	ULONG			dpos0;		/* DPOS�̏����� for POSING�̎w�ߑ��x�i�������j�̐ώZ */
	LONG			dposRem;	/* �`�������ړ���(dpos)�]��         */
	LONG			Cspd;		/* ���ݎw�ߑ��x */
	MC_BASE_PRM*	basePrm;	/* MG��{�p�����[�^ */
	MG_PRM*			prm;		/* Motion�p�����[�^ */
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
/* �g���N/���x���䎞 ���ݎw�ߑ��x�ݒ� */
LONG MtgSetCspd( MG_HNDL *hdl, LONG spd );


#define		MOTOIN_GEN_H_
#endif /* MOTOIN_GEN_H_ */
