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
#include "Basedef.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"


/****************************************************************************
 Private functions
****************************************************************************/
/*  �ׂ��扉�Z���� */
static	VOID mPrmCalcPow( LONG data, LONG exp, LONG *num, LONG *deno );
/*  TFF/TLIM�g�p�I������ */
static	VOID mPrmTrefMode( MT_PRM *mtPrm, MTPRM_INI *iniPrm );
/* ���x�P�ʕϊ� �萔�쐬 */
static	VOID pcal_SpdInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* �g���N�P�ʕϊ� �萔�쐬 */
static	VOID pcal_TrqInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* �����x�P�ʕϊ� �萔�쐬 */
static	void pcal_AccInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, PRMDATA *Prm);
/* �ʒu�P�ʕϊ� �萔�쐬 */
static	VOID pcal_PosInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	�����P�ʂ̎w���v�Z�̂� */
static	KSGAIN pcal_SpdOdrPowKx( PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�����P�ʂ̎w���v�Z�̂� */
static	KSGAIN pcal_SpdMonPowKx( PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> RPM(mm/s) */
static	KSGAIN pcal_SpdMonRefToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	RPM(mm/s) --> �w�ߒP��/sec */
static	KSGAIN pcal_SpdOdrRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	RPM(mm/s) --> �w�ߒP��/sec */
static	KSGAIN pcal_SpdMonRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> ��i���x�ɑ΂��道 */
static	KSGAIN pcal_SpdMonRefToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	��i���x�ɑ΂��道 --> �w�ߒP��/sec */
static	KSGAIN pcal_SpdOdrRatToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	�w�ߒP��/sec --> �ő呬�x/40000000h */
static	KSGAIN pcal_SpdOdrRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> �ő呬�x/40000000h */
static	KSGAIN pcal_SpdMonRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	�ő呬�x/40000000h --> �w�ߒP��/sec */
static	KSGAIN pcal_SpdOdrMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	RPM(mm/s) --> �w�ߒP��/min */
static	KSGAIN pcal_SpdMonRpmToMinKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	�w�ߒP��/min --> �w�ߒP��/sec */
static	KSGAIN pcal_SpdOdrMinToRefKx( PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> �w�ߒP��/min */
static	KSGAIN pcal_SpdMonRefToMinKx( PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	RPM --> ��i���x�ɑ΂��道 */
static	KSGAIN pcal_SpdMonRpmToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	RPM --> �ő呬�x/40000000h	  */
static	KSGAIN pcal_SpdOdrRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	RPM --> �ő呬�x/40000000h	  */
static	KSGAIN pcal_SpdMonRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	�w�ߒP��/min --> �ő呬�x/40000000h */
static	KSGAIN pcal_SpdOdrMinToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i�w�ߕϊ��j	��i���x�ɑ΂��道 --> �ő呬�x/40000000h */
static	KSGAIN pcal_SpdOdrRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI��	�w�ߒP��/sec --> RPM	10 -3��Œ� */
static	KSGAIN pcal_SpdRefToRpmKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm );
/* ���x�P�ʑI��	RPM --> �w�ߒP��/sec	10��-3��Œ� */
static	KSGAIN pcal_SpdRpmToRefKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm );
/* ���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� �w�ߒP��/sec */
static	KSGAIN pcal_SpdMon24OsToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� ��i���x�ɑ΂��道 */
static	KSGAIN pcal_SpdMon24OsToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� min-1(RPM), mm/s */
static	KSGAIN pcal_SpdMon24OsToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� �ő呬�x/40000000h */
static	KSGAIN pcal_SpdMon24OsToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�ő呬�x/40000000h �� �w�ߒP��/sec */
static	KSGAIN pcal_SpdMonMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�ő呬�x/40000000h �� ��i���x�ɑ΂��道 */
static	KSGAIN pcal_SpdMonMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���x�P�ʑI���i���j�^�ϊ��j	�ő呬�x/40000000h �� min-1(RPM), mm/s */
static	KSGAIN pcal_SpdMonMaxToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm );
/* �g���N�P�ʑI��	�����P�ʂ̎w���v�Z�̂� */
static	KSGAIN pcal_TrqPowKx( LONG TrqExp, LONG inv, PRMDATA *Prm );
/* �g���N�P�ʑI��	Nm(N) --> �ő��ٸ/40000000h				  */
static	KSGAIN pcal_TrqNmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* �g���N�P�ʑI��	�ő��ٸ/40000000h --> Nm(N)				  */
static	KSGAIN pcal_TrqMaxToNmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* �g���N�P�ʑI��	��i�ٸ�ɑ΂��道 --> �ő��ٸ/40000000h	  */
static	KSGAIN pcal_TrqRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* �g���N�P�ʑI��	�ő��ٸ/40000000h --> ��i�ٸ�ɑ΂��道	  */
static	KSGAIN pcal_TrqMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm );
/* �����x(�w�ߒP��/sec^2)  --> �����x(**.*** �w�ߒP��/cyc^2) */
static	KSGAIN pcal_AccS2TOCyc2Kx( LONG maxSpd_deno, LONG cycle, LONG inv, PRMDATA *Prm );
/* �����x(�w�ߒP��/sec^2) --> �����x([2^24/OS]/cycle) */
static	KSGAIN pcal_AccRefToOsKx( MTPRM_INI *iniPrm, PRMDATA *Prm );
/* ���󖢑Ή� */
/* ��������(ms)  --> �����x(**.*** �w�ߒP��/cyc^2)  */
//static	LONG pcal_AccMsToCyc2Kx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, USHORT cycle, PRMDATA *pprm_ptr );
/* ���󖢑Ή� */
/* ��������(ms) --> �����x([2^24/OS]/cycle) */
//static	LONG pcal_AccMsToOsKx( MTPRM_INI *iniPrm );
/* �����P�ʂ̎w���v�Z�̂� */
static	KSGAIN pcal_PosPowKx( LONG inv, PRMDATA *Prm );

const OPTION_MAP OptMapTbl = {
	3,		/* �������t�B���^�I���r�b�g�ʒu */
	0x3,	/* �������t�B���^�I���r�b�g�}�X�N */
	8,		/* �Q�C���I���r�b�g�ʒu */
	0x3,	/* �Q�C���I���r�b�g�}�X�N */
	12,		/* P_PI�I���r�b�g�ʒu */
	0x1,	/* P_PI�I���r�b�g�}�X�N */
	13,		/* �ʒu�ϕ��N���A�r�b�g�ʒu */
	0x1,	/* �ʒu�ϕ��r�b�g�}�X�N */
	14,		/* P_CL�r�b�g�ʒu */
	0x1,	/* P_CL�r�b�g�}�X�N */
	15,		/* N_CL�r�b�g�ʒu */
	0x1,	/* N_CL�r�b�g�}�X�N */
	31,		/* OutSignal�r�b�g�ʒu */
	0x7,	/* OutSignal�r�b�g�}�X�N */
	31,		/* BankSel�r�b�g�ʒu */
	0xF,	/* BankSel�r�b�g�}�X�N */
	31,		/* LtDisable�r�b�g�ʒu */
	0x1,	/* LtDisable�r�b�g�}�X�N */
};

/* <S1C0>�FStart */
const UCHAR SvCmdBitMaskTable[VENDOR_IOBIT_NUM] = {
	0x0F,	/* Pn856.0�FBANK�̃r�b�g�}�X�N				 */
	0x01,	/* Pn856.1�FSO1�̃r�b�g�}�X�N				 */
	0x01,	/* Pn856.2�FSO2�̃r�b�g�}�X�N				 */
	0x01,	/* Pn856.3�FSO3�̃r�b�g�}�X�N				 */
	/* ----------------------------------------------	 */
	0x01,	/* Pn857.0�FSO4�̃r�b�g�}�X�N				 */
	0x01,	/* Pn857.1�FSO5�̃r�b�g�}�X�N				 */
	0x01,	/* Pn857.2�FJ_SEL�̃r�b�g�}�X�N				 *//* �����p	*/
	0x01,	/* Pn857.3�FReserve�̃r�b�g�}�X�N			 */
	/* ----------------------------------------------	 */
	0x01,	/* Pn858.0�FS_RATIO�̃r�b�g�}�X�N			 *//* �����p	*/
	0x01,	/* Pn858.1�FLT_REQ_EX1�̃r�b�g�}�X�N		 *//* �����p	*/
	0x01,	/* Pn858.2�FLT_REQ_EX2�̃r�b�g�}�X�N		 *//* �����p	*/
	0x01,	/* Pn858.3�FLT_REQ_EX3�̃r�b�g�}�X�N		 *//* �����p	*/
	/* ----------------------------------------------	 */
	0x01,	/* Pn859.0�FReserve�̃r�b�g�}�X�N			 */
	0x01,	/* Pn859.1�FReserve�̃r�b�g�}�X�N			 */
	0x01,	/* Pn859.2�FReserve�̃r�b�g�}�X�N			 */
	0x01,	/* Pn859.3�FReserve�̃r�b�g�}�X�N			 */
};
LONG EnaOutBit;
/* <S1C0>�FEnd */

/****************************************************************************
 ���[�V�����p�����[�^�����v�Z����
 �T�v�F���ݐݒ肳��Ă���A���[�V���������A����\�ݒ肩��ȉ���ݒ�
	1)�P�ʕϊ��萔�̌v�Z
	PosingRateKx	;[10000ref/s^s]->[***.*ref/cyc^cyc]���x�ϊ��萔
	PosingSpdKx		;[ref/s]->[***.*ref/cyc]���x�ϊ��萔
	PosingSpdInvKx	;[***.*ref/cyc]->[ref/s]���x�ϊ��萔
	PosingSpdMax	;POSING�ő呬�x[ref/s]
	fspdRefKx		;[ref/�ʐM����]->[ref/s]���x�ϊ��萔
	refToOsKx		;[�w�ߒP��/s]->[24bit/OS]���x�ϊ��萔
	osToRefKx		;[24bit/OS]->[�w�ߒP��/s]���x�ϊ��萔
	maxToOsKx		;[30bit/MAX]->[24bit/OS]���x�ϊ��萔
	osToMaxKx		;[24bit/OS]->[30bit/MAX]���x�ϊ��萔
	trqKx			;[24bit/MAX]->[%]�g���N�ϊ��萔
	2)���[�V�����ݒ�
	MotionCycle		;���[�V�����������[0.25us]
	MG_BASE_PRM		;MG�p��{�p�����[�^�S��
	3)�T�[�{���
	absPG			;��޿PG�g�p
	trefMode		;TRef Option
	OptMap			;OPTION�w�ߊ��t
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	LONG sx, kx;
	LONG ky, sy;
	LONG maxSpd_num;
	LONG maxSpd_deno;
	LONG ovSpd_num;
	LONG ovSpd_deno;
	LONG pitch_num;
	LONG pitch_deno;

/* ���[�V�����T�C�N���Z�b�g */
	mtPrm->BasePrm.MotionCycle = iniPrm->Cycle;
//	iniPrm->SpdDiv = MG_DEFAULT_SPDDIV;/* <S06C> */

/* ���x�̒P�ʕϊ��萔�̌v�Z[ref/�ʐM����]->[ref/s] */
	sx = 0;
	mtPrm->BasePrm.fspdRefKx = MlibPcalKxgain( 1, 4000000, iniPrm->Cycle, &sx, 24 );

/* �g���N�̒P�ʕϊ��萔�̌v�Z[24bit/MAX]->[%] */
	sx = 0;
	mtPrm->BasePrm.trqKx = MlibPcalKxgain( 1, iniPrm->MaxTrq, MAX_TRQ24, &sx, 24 );

	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );

/* ���x�̒P�ʕϊ��萔�̌v�Z[30bit/MAX]->[24bit/OS] */
	/* buf = spd * maxSpd/(ovSpd*64) */
	/* buf = spd * maxSpd * (maxSpd_num/maxSpd_deno) / (ovSpd*(ovSpd_num/ovSpd_deno)*64) */
	sx = sy = 0;
	kx = MlibPcalKxmulx( iniPrm->MaxSpd, maxSpd_num, ovSpd_deno, &sx );
	ky = MlibPcalKxmulx( iniPrm->OverSpd*64, ovSpd_num, maxSpd_deno, &sy );
	mtPrm->BasePrm.maxToOsKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

/* ���x�̒P�ʕϊ��萔�̌v�Z[24bit/OS]->[30bit/MAX] */
	mtPrm->BasePrm.osToMaxKx = MlibPcalKxdivx( ky, sy, kx, sx, 24 );

/* ���x�̒P�ʕϊ��萔�̌v�Z[�w�ߒP��/s]->[24bit/OS] */
/* ���x�̒P�ʕϊ��萔�̌v�Z[24bit/OS]->[�w�ߒP��/s] */
	if( iniPrm->LinearMotor != FALSE )
	{
		sx = sy = 0;
		mPrmCalcPow( 1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxmulx( iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx );
		kx = MlibPcalKxmulx( kx, ovSpd_deno, pitch_num, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, pitch_deno, ovSpd_num, &sy );
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
	}
	else
	{
		sx = sy = 0;
		kx = MlibPcalKxmulx( 60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy );
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
	}


/* ���x�̒P�ʕϊ��萔�̌v�Z[**.*** ref/�������]��[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx( 1, 4000000, 1, &sx );
	ky = MlibPcalKxmulx( (1 << iniPrm->SpdDiv), iniPrm->Cycle, 1, &sy );
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

#if (FLOAT_USE==TRUE)
	mtPrm->BasePrm.PosingSpdMax = (LONG)( (float)0x40000000 * mtPrm->BasePrm.PosingSpdInvKx );

/* POSING����\�Z�o --OverSpeed���x���������_�ʒu�Ƃ��� */
	ovSpd_num = (LONG)( (float)MAX_SPD24 * mtPrm->BasePrm.osToRefKx );	/* Over Speed [ref/s] */
#else
	mtPrm->BasePrm.PosingSpdMax = MlibMulgain32( 0x40000000, mtPrm->BasePrm.PosingSpdInvKx );

/* POSING����\�Z�o --OverSpeed���x���������_�ʒu�Ƃ��� */
	ovSpd_num = MlibMulgain32( MAX_SPD24, mtPrm->BasePrm.osToRefKx );	/* Over Speed [ref/s] */
#endif /* FLOAT_USE */

	for( sx = MG_SPDDIV_RANGE; sx > 0; sx-- )
	{
		maxSpd_num = mtPrm->BasePrm.PosingSpdMax >> sx;
		if(ovSpd_num < maxSpd_num)
		{
			break;
		}
	}
	maxSpd_deno = iniPrm->SpdDiv + sx;		/* �����_�r�b�g�ʒu */


/* �������萔�̌v�Z[10000 x ref/s^2]��[**.*** ref/�������^2] */
	/* buf = rate * 2^vel_shift * 10000 / (4000000[.25us]/cyc[.25us])^2 */
	/* buf = rate * 2^vel_shift * cyc[.25us]^2 / (40000)^2 */
	sx = 0;
	kx = MlibPcalKxgain( (1 << maxSpd_deno), iniPrm->Cycle, 40000, &sx, 0 );
	mtPrm->BasePrm.PosingRateKx = MlibPcalKxgain( kx, iniPrm->Cycle, 40000, &sx, 24 );

/* ���x�P�ʕϊ��萔�̌v�Z[ref/s]��[**.*** ref/�������] */
	/* buf = spd * 2^vel_shift / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * 2^vel_shift * cyc[.25us] / 4000000[.25us] */
	sx = 0;
	mtPrm->BasePrm.PosingSpdKx = MlibPcalKxgain( (1 << maxSpd_deno), iniPrm->Cycle, 4000000, &sx, 24 );

/* ���x�̒P�ʕϊ��萔�̌v�Z[**.*** ref/�������]��[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx( 1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx( (1 << maxSpd_deno), iniPrm->Cycle, 1, &sy );
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
#if (FLOAT_USE==TRUE)
	mtPrm->BasePrm.mgSpdMax = (LONG)( (float)0x40000000 * mtPrm->BasePrm.PosingSpdInvKx );
#else
	mtPrm->BasePrm.mgSpdMax = MlibMulgain32( 0x40000000, mtPrm->BasePrm.PosingSpdInvKx );
#endif /* FLOAT_USE */

/* MG_BASE_PRM�Z�b�g */
	mtPrm->BasePrm.vel_sft_exp = iniPrm->SpdDiv;
	mtPrm->BasePrm.vel_shift = maxSpd_deno;
	mtPrm->BasePrm.vel_shift_s = MG_VREMBIT_MAX - maxSpd_deno;
	mtPrm->BasePrm.vel_max = 1 << (31 - maxSpd_deno);
	mtPrm->BasePrm.seg_num = iniPrm->SegNum;

	sx = 0;
	kx = MlibPcalKxgain( mtPrm->BasePrm.PosingSpdMax, iniPrm->Cycle, 4000000, &sx, 24 );
#if (FLOAT_USE==TRUE)
	mtPrm->BasePrm.seg_max_ref = iniPrm->SegNum * kx;
#else
	mtPrm->BasePrm.seg_max_ref = MlibMulgain32( iniPrm->SegNum, kx );
#endif /* FLOAT_USE */
	mtPrm->BasePrm.osVel = ovSpd_num;

#if 0
/* ACC_TIME�p�����x�P�ʕϊ��萔�̌v�Z[(ref/s)/(0.1ms)]��[**.*** ref/�������^2] <Rev2.01>*/
	sx = sy = 0;
	kx = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, iniPrm->Cycle, &sx);
	ky = MlibPcalKxmulx(40000, 40000, 1, &sy);
	mtPrm->BasePrm.taccRateKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);

/* ACC_TIME�p�����x�P�ʕϊ��萔�̌v�Z[(2^7*ref/s)/(0.1ms)]��[**.*** ref/�������^2] <Rev2.01>*/
	sy = 0;
	ky = MlibPcalKxmulx( 40000, 40000, (1<<7), &sy );
	mtPrm->BasePrm.taccRate7Kx = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
#endif

/* TFF/TLM/ACC_TIME�g�p�I��	*/
#if 0 /* M2�v���t�@�C���Ή�	*/
	��PrmTrefMode( mtPrm, iniPrm );
#else
	mtPrm->BasePrm.TrefModePos = TREFMODE_TFF_TLIM;/* <S06D */
#endif
#if 0
	prm->BasePrm.SetCmdAccRate = ((iniPrm->OptEnable&0x0F00) != 0) ? &MPrmCalcAccTimeRate : NULL;
#endif

/* �ʒu���֘APRM�Z�b�g */
	mtPrm->PsnPrm.absPG = iniPrm->absPG;									/* �A�u�\PG�g�p */
	mtPrm->PsnPrm.TdlyIn = iniPrm->dlyTime;
	mtPrm->PsnPrm.SegNum = iniPrm->SvCycle;
	
	/* ���x�P�ʕϊ� �萔�쐬 */
	pcal_SpdInit( mtPrm, iniPrm, Prm );
	/* �g���N�P�ʕϊ� �萔�쐬 */
	pcal_TrqInit( mtPrm, iniPrm, Prm );
	/* �����x�P�ʕϊ� �萔�쐬 */
	pcal_AccInit( mtPrm, iniPrm, maxSpd_deno, Prm );
	/* �ʒu�P�ʕϊ� �萔�쐬 */
	pcal_PosInit( mtPrm, iniPrm, Prm );

/* Option�w�ߊ��t��` */
	mtPrm->ProfPrm.OutSigEnable = iniPrm->OutSigEnable;/* <S14D> */
	mtPrm->OptMap = OptMapTbl;	/* �f�t�H���g�l�Z�b�g */
	if((iniPrm->OptEnable & 0x01) != 0)
	{
		if((iniPrm->OptMap[0]&0x00F0) == 0) mtPrm->OptMap.accfilBitMsk = 0;
		if((iniPrm->OptMap[0]&0xF000) == 0)	mtPrm->OptMap.gselBitMsk = 0;
		if((iniPrm->OptMap[1]&0x00F0) == 0)	mtPrm->OptMap.vppiBitMsk = 0;
		if((iniPrm->OptMap[1]&0xF000) == 0)	mtPrm->OptMap.ppiclrBitMsk = 0;
		if((iniPrm->OptMap[2]&0x00F0) == 0)	mtPrm->OptMap.pclBitMsk = 0;
		if((iniPrm->OptMap[2]&0xF000) == 0)	mtPrm->OptMap.nclBitMsk = 0;
		if((iniPrm->OptMap[3]&0x00F0) == 0)	mtPrm->OptMap.bankBitMsk = 0;
		if((iniPrm->OptMap[3]&0xF000) == 0)	mtPrm->OptMap.ltBitMsk = 0;
		if((iniPrm->OptMap[4]&0xF000) == 0)	mtPrm->OptMap.outpBitMsk = 0;
		if(iniPrm->OutSigEnable == 0)		mtPrm->OptMap.outpBitMsk = 0;

		mtPrm->OptMap.accfilBit = (iniPrm->OptMap[0] & 0xF);
		mtPrm->OptMap.gselBit = (iniPrm->OptMap[0] >> 8) & 0xF;
		mtPrm->OptMap.vppiBit = (iniPrm->OptMap[1] & 0xF);
		mtPrm->OptMap.ppiclrBit = (iniPrm->OptMap[1] >> 8) & 0xF;
		mtPrm->OptMap.pclBit = (iniPrm->OptMap[2] & 0xF);
		mtPrm->OptMap.nclBit = (iniPrm->OptMap[2] >> 8) & 0xF;
		mtPrm->OptMap.bankBit = (iniPrm->OptMap[3] & 0xF);
		mtPrm->OptMap.ltBit = (iniPrm->OptMap[3] >> 8) & 0xF;
		mtPrm->OptMap.outpBit = (iniPrm->OptMap[4] >> 8) & 0xF;
	}
	
	return 0;
}

/****************************************************************************
Vendor Specifics I/O����t���v�Z
****************************************************************************//* <S1C0> */
VOID	MPrmCalcSvCmdIo(MT_PRM *mtPrm, PRMDATA *Prm )
{
	LONG	i,j,sft;
	USHORT	work;
	ULONG	enaOutbit;

	/* ���[�J���ϐ������� */
	enaOutbit = 0;

	/* �p�����[�^�̒l�ɉ����ăZ�b�g����B	*/
	for(i = 0; i < VENDOR_IOBIT_NUM; i++)
	{
		j = i >> 2;
		sft = (i & 0x03) * 4;
		work = (Prm->svcmdIoOut[j] >> sft ) & 0x000F;
		if(work != 0)
		{
			mtPrm->SvCmdIoOutMap[i].bitNo = (UCHAR)(work + VENDOR_IOBIT_OFFSET);
			mtPrm->SvCmdIoOutMap[i].bitsMsk = SvCmdBitMaskTable[i];
			enaOutbit |= (LONG)SvCmdBitMaskTable[i] << mtPrm->SvCmdIoOutMap[i].bitNo;
		}
		else
		{
			mtPrm->SvCmdIoOutMap[i].bitNo = (UCHAR)0;
			mtPrm->SvCmdIoOutMap[i].bitsMsk = (UCHAR)0;
		}
	}
	
	/* PnB24��SVCMD_IO(�o��)�L��/������Ԃ��Z�b�g����B�B	*/
	Prm->m3_enaOutbitL = ((Prm->m3_enaOutbitL & 0x0000FFFF) | enaOutbit);
}

/****************************************************************************
 �ׂ��扉�Z����
****************************************************************************/
static	VOID mPrmCalcPow( LONG data, LONG exp, LONG *num, LONG *deno )
{
	if( exp > 0 )
	{
		while( (exp--) > 0 )
		{
			data *= 10;
		}
		*num = data;
		*deno = 1;
	}
	else if( exp < 0 )
	{
		exp = MlibLABS(exp);
		while( (exp--) > 0 )
		{
			data *= 10;
		}
		*num = 1;
		*deno = data;
	}
	else
	{
		*num = data;
		*deno = 1;
	}
}



/****************************************************************************************************/
/*		���x�P�ʑI��	�ϊ��萔�쐬����															*/
/****************************************************************************************************/
static	void pcal_SpdInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	mtPrm->BasePrm.m3ConvUnit.m3SpdSelUnit = Prm->m3_velUnitL;

	switch (mtPrm->BasePrm.m3ConvUnit.m3SpdSelUnit) {
		case 0:	/* �w�ߒP��/sec */
			/*--------------------------------------------------------------------------------------*/
			/* �w�ߒP��/sec --> �w�ߒP��/sec														*/
			/* �w�߁@���x�ϊ�(�ʒu����)									�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �w�ߒP��/sec --> �ő呬�x/40000000h													*/
			/* �w�߁@���x�ϊ�(���x����A�g���N����)						�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrRefToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �������Z�P�� --> �w�ߒP��/sec														*/
			/* ���j�^�@CSPD,FSPD,TSPD,SPD_LIM							�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonPowKx( Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRefKx( mtPrm, iniPrm, 1, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRefKx( mtPrm, iniPrm, 1, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> �w�ߒP��/sec																	*/
			/* 															�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToRefKx( mtPrm, iniPrm, Prm );
			break;

		case 1:	/* �w�ߒP��/min */
			/*--------------------------------------------------------------------------------------*/
			/* �w�ߒP��/min --> �w�ߒP��/sec														*/
			/* �w�߁@���x�ϊ�											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrMinToRefKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �w�ߒP��/min --> �ő呬�x/40000000h													*/
			/* �w�߁@���x�ϊ�(���x����A�g���N����)						�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrMinToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �������Z�P�� --> �w�ߒP��/min														*/
			/* ���j�^�@CSPD,FSPD,TSPD,SPD_LIM							�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToMinKx( Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRefKx( mtPrm, iniPrm, 60, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRefKx( mtPrm, iniPrm, 60, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> �w�ߒP��/min																	*/
			/* 															�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToMinKx( mtPrm, iniPrm, Prm );
			break;

		case 2:	/* ��i���x�ɑ΂��道 */
			/*--------------------------------------------------------------------------------------*/
			/* ��i���x�ɑ΂��道 --> �w�ߒP��/sec 													*/
			/* �w�߁@���x�ϊ�											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrRatToRefKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* ��i���x�ɑ΂��道 --> �ő呬�x/40000000h											*/
			/* �w�߁@���x�ϊ�(���x����A�g���N����)						�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrRatToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �������Z�P�� --> ��i���x�ɑ΂��道													*/
			/* ���j�^�@CSPD,FSPD,TSPD,SPD_LIM							�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToRatKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRatKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRatKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> ��i���x�ɑ΂��道 		 													*/
			/* 															�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToRatKx( mtPrm, iniPrm, Prm );
			break;

		case 3:	/* min-1(RPM), mm/s */
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> �w�ߒP��/sec																	*/
			/* �w�߁@���x�ϊ�											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrRpmToRefKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> �ő呬�x/40000000h		 													*/
			/* �w�߁@���x�ϊ�(���x����A�g���N����)						�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrRpmToMaxKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> RPM 		 																	*/
			/* 															�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �������Z�P�� --> RPM																	*/
			/* ���j�^�@CSPD,FSPD,TSPD,SPD_LIM							�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToRpmKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToRpmKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonMaxToRpmKx( mtPrm, iniPrm, Prm );
			break;

		case 4:	/* �ő呬�x/40000000h */
			/*--------------------------------------------------------------------------------------*/
			/* �ő呬�x/40000000h --> �w�ߒP��/sec 													*/
			/* �w�߁@���x�ϊ�											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToRef = pcal_SpdOdrMaxToRefKx( mtPrm, iniPrm, Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �ő呬�x/40000000h --> �ő呬�x/40000000h											*/
			/* �w�߁@���x�ϊ�(���x����A�g���N����)						�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdUnitToMax = pcal_SpdOdrPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* �������Z�P�� --> �ő呬�x/40000000h													*/
			/* ���j�^�@CSPD,FSPD,TSPD,SPD_LIM							�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRefToUnit = pcal_SpdMonRefToMaxKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3Spd24OsToUnit = pcal_SpdMon24OsToMaxKx( mtPrm, iniPrm, Prm );
			mtPrm->BasePrm.m3ConvUnit.m3SpdMaxToUnit = pcal_SpdMonPowKx( Prm );
			/*--------------------------------------------------------------------------------------*/
			/* RPM --> �ő呬�x/40000000h		 													*/
			/* 															�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3SpdRpmToUnit = pcal_SpdMonRpmToMaxKx( mtPrm, iniPrm, Prm );
			break;

		default:
			break;

	}
	/* ���_���A�A�v���[�`���x�A���_���A�N���[�v���x�ϊ��p */
	mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm = pcal_SpdRefToRpmKxMinus3( mtPrm, iniPrm );	/* �w�ߒP��/s �� RPM */
	mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef = pcal_SpdRpmToRefKxMinus3( mtPrm, iniPrm );	/* RPM �� �w�ߒP��/s */
}


/****************************************************************************************************/
/*		�g���N�P�ʑI��	�ϊ��萔�쐬����															*/
/****************************************************************************************************/
static	void pcal_TrqInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	SHORT	TrqExp;

	TrqExp = (SHORT)iniPrm->RatTrqExp;
	mtPrm->BasePrm.m3ConvUnit.m3TrqSelUnit = Prm->m3_trqUnitL;

	switch (mtPrm->BasePrm.m3ConvUnit.m3TrqSelUnit) {
		case 0:	/* Nm(N) */ /* 2009.01.20 Y.Oka ���󖢃T�|�[�g */
			/*--------------------------------------------------------------------------------------*/
			/* Nm(N) --> �ő��ٸ/40000000h															*/
			/* �w��tlim�Atff											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqUnitToMax = pcal_TrqNmToMaxKx(mtPrm, iniPrm, TrqExp, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* �ő��ٸ/40000000h --> Nm(N) 															*/
			/* ���j�^													�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqMaxToUnit = pcal_TrqMaxToNmKx(mtPrm, iniPrm, TrqExp, -1, Prm);
			break;

		case 1:	/* ��i�ٸ�ɑ΂��道 */
			/*--------------------------------------------------------------------------------------*/
			/* ��i�ٸ�ɑ΂��道 --> �ő��ٸ/40000000h												*/
			/* �w��tlim�Atff											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqUnitToMax = pcal_TrqRatToMaxKx(mtPrm, iniPrm, 0, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* �ő��ٸ/40000000h --> ��i�ٸ�ɑ΂��道 												*/
			/* ���j�^													�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqMaxToUnit = pcal_TrqMaxToRatKx(mtPrm, iniPrm, 0, -1, Prm);
			break;

		case 2:	/* �ő��ٸ/40000000h */
			/*--------------------------------------------------------------------------------------*/
			/* �ő��ٸ/40000000h --> �ő��ٸ/40000000h 												*/
			/* �w��tlim�Atff											�i�w���͂��̂܂܁j			*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqUnitToMax = pcal_TrqPowKx(0, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* �ő��ٸ/40000000h --> �ő��ٸ/40000000h 												*/
			/* ���j�^													�i�w���͔��]�j				*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3TrqMaxToUnit = pcal_TrqPowKx(0, -1, Prm);
			break;

		default:
			break;
	}
}


/****************************************************************************************************/
/*		�����x�P�ʑI��	�ϊ��萔�쐬����															*/
/****************************************************************************************************/
static	void pcal_AccInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, PRMDATA *Prm)
{
	mtPrm->BasePrm.m3ConvUnit.m3AccSelUnit = Prm->m3_accUnitL;

	switch (mtPrm->BasePrm.m3ConvUnit.m3AccSelUnit)
	{
		case 0:
			/*--------------------------------------------------------------------------------------*/
			/* �����x(�w�ߒP��/sec^2) --> �����x(�w�ߒP��/cyc^2)									*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToCyc2 = pcal_AccS2TOCyc2Kx(maxSpd_deno, iniPrm->Cycle, 1, Prm);
			/*--------------------------------------------------------------------------------------*/
			/* �����x(�w�ߒP��/sec^2) --> �����x([2^24/OS]/cycle)									*/
			/*--------------------------------------------------------------------------------------*/
			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToOs = pcal_AccRefToOsKx(iniPrm, Prm);
			break;

//		case 1: /* 2009.01.20 Y.Oka ���󖢃T�|�[�g */
			/*--------------------------------------------------------------------------------------*/
			/* ��������(ms) --> �����x(�w�ߒP��/sec^2)												*/
			/*--------------------------------------------------------------------------------------*/
//			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToCyc2 = pcal_AccMsToCyc2Kx(mtPrm, iniPrm, maxSpd_deno, cycle, pprm_ptr);
			/*--------------------------------------------------------------------------------------*/
			/* ��������(ms) --> �����x([2^24/OS]/cycle)												*/
			/*--------------------------------------------------------------------------------------*/
//			mtPrm->BasePrm.m3ConvUnit.m3AccUnitToOs = pcal_AccMsToOsKx(iniPrm);
//			break;

		default:
			break;
	}
}

/****************************************************************************************************/
/*		�ʒu�P�ʑI��	�ϊ��萔�쐬����															*/
/****************************************************************************************************/
static	void pcal_PosInit( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	/* �w�ߗp */
	mtPrm->BasePrm.m3ConvUnit.m3PosUnitToRef = pcal_PosPowKx(1, Prm);

	/* ���j�^�p */
	mtPrm->BasePrm.m3ConvUnit.m3PosRefToUnit = pcal_PosPowKx(-1, Prm);
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	�����P�ʂ̎w���v�Z�̂�											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrPowKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�����P�ʂ̎w���v�Z�̂�											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonPowKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
//	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> RPM(mm/s)										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* ��]�^ */
		/* 	�ϊ��萔 = 60 / PgPuls */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	RPM(mm/s) --> �w�ߒP��/sec										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = PgPuls * 10^9 / (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = PgPuls / 60 */
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, 60, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	RPM(mm/s) --> �w�ߒP��/sec										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = PgPuls * 10^9 / (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) */
		mPrmCalcPow(1 , 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = PgPuls / 60 */
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, 60, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> ��i���x�ɑ΂��道								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* �ϊ��萔 = �ϊ��萔 * (100 / ��i���x(mm/s)) */
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = (60 / PgPuls) * (100 / ��i��]���x(rpm)) */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	kx = MlibPcalKxgain( kx, 100, iniPrm->RatSpd, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	��i���x�ɑ΂��道 --> �w�ߒP��/sec								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRatToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L ) * 1;
	base = ( base + iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* ����\(mm) = PgPuls * 10^9 / (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* �ϊ��萔 = ����\(����) * (��i��]���x(rpm) / 100) */
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = (PgPuls / 60) * (��i��]���x(rpm) / 100) */
		kx = MlibPcalKxgain( iniPrm->PgPuls, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx,  0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->RatSpd, 100, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	�w�ߒP��/sec --> �ő呬�x/40000000h								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );
	base = ( base - iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* ����\(mm) = (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* �ϊ��萔 = ����\(mm) * (40000000h / �ő��]���x(rpm)) */
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = (60 / PgPuls) * (40000000h / �ő��]���x(rpm)) */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
}
	kx = MlibPcalKxgain( kx, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> �ő呬�x/40000000h								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = (base - iniPrm->MaxSpdExp);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* ����\(mm) = (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* �ϊ��萔 = ����\(mm) * (40000000h / �ő��]���x(rpm)) */
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = (60 / PgPuls) * (40000000h / �ő��]���x(rpm)) */
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	�ő呬�x/40000000h --> �w�ߒP��/sec								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );
	base = ( base + iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* ����\(mm) = PgPuls * 10^9 / (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* �ϊ��萔 = ����\(mm) * �ő��]���x(rpm) / 40000000h) */
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = (PgPuls / 60) * (�ő��]���x(rpm) / 40000000h) */
		kx = MlibPcalKxgain( iniPrm->PgPuls, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx,  0);
	}
	kx = MlibPcalKxgain(kx, iniPrm->MaxSpd, 0x40000000, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	RPM(mm/s) --> �w�ߒP��/min										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToMinKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = PgPuls * 10^9 / (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 60, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = PgPuls */
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, 1, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	�w�ߒP��/min --> �w�ߒP��/sec									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrMinToRefKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	mPrmCalcPow( 1, base, &num, &deno );

	/* �ϊ��萔 = 1 / 60 */
	sx = 0;
	kx = MlibPcalKxgain( 1, 1, 60, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�w�ߒP��/sec --> �w�ߒP��/min									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRefToMinKx( PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );

	/* �ϊ��萔 = 60 */
	sx = 0;
	kx = MlibPcalKxgain( 1, 60, 1, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	RPM --> ��i���x�ɑ΂��道										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );

	mPrmCalcPow( 1, base, &num, &deno );

	/* �ϊ��萔 = 100 / ��i��]���x(rpm) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 100, iniPrm->RatSpd, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	RPM --> �ő呬�x/40000000h	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	base = ( base - iniPrm->MaxSpdExp );

	mPrmCalcPow(1, base, &num, &deno);

	/* �ϊ��萔 = (40000000h / �ő��]���x(rpm) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	RPM --> �ő呬�x/40000000h	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonRpmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );

	mPrmCalcPow( 1, base, &num, &deno );

	/* �ϊ��萔 = (40000000h / �ő��]���x(rpm) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	�w�ߒP��/min --> �ő呬�x/40000000h								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrMinToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_velBase4L );
	base = ( base - iniPrm->MaxSpdExp );

	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* ����\(mm) = (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		/* �ϊ��萔 = ����\(mm) * (40000000h / �ő��]���x(rpm)) */
	}
	else
	{ /* ��]�^ */
		/* �ϊ��萔 = (1 / PgPuls) * (40000000h / �ő��]���x(rpm)) */
		kx = MlibPcalKxgain( 1, 1, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	kx = MlibPcalKxgain( kx, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i�w�ߕϊ��j	��i���x�ɑ΂��道 --> �ő呬�x/40000000h						*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdOdrRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_velBase4L );
	mPrmCalcPow( 1, base, &num, &deno );

	/* �ϊ��萔 = (40000000h / �ő��]���x(rpm)) * (��i��]���x(rpm) / 100) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, iniPrm->RatSpd, 100, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI��	�w�ߒP��/sec --> RPM	10 -3��Œ�											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdRefToRpmKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = 3;
	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = (�X�P�[���s�b�`(pm) * �X�P�[���s�b�`�w��) / 10^9 / PgPuls */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->ScalePitch, 1, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* ��]�^ */
		/* 	���[�^���F�ϊ��萔 = 60 / PgPuls */
		/* 	�t���N  �F�ϊ��萔 = 60 / (Pn20A * �s�b�`����̃p���X��)
		     �� iniPrm.mt.PgPuls = Pn20A * �s�b�`����̃p���X��	*/
		kx = MlibPcalKxgain( 1, 60, iniPrm->PgPuls, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI��	RPM --> �w�ߒP��/sec	10��-3��Œ�										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdRpmToRefKxMinus3( MT_PRM *mtPrm, MTPRM_INI *iniPrm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	pitch_num, pitch_deno;

	base = -3;
	sx = 0;
	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* �ϊ��萔 = PgPuls * 10^9 / (�X�P�[���s�b�`(nm) * �X�P�[���s�b�`�w��) */
		mPrmCalcPow( 1, 9 - iniPrm->ScalePitchExp, &pitch_num, &pitch_deno );
		kx = MlibPcalKxgain( iniPrm->PgPuls, 1, iniPrm->ScalePitch, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
	}
	else
	{ /* ��]�^ */
		/* ���[�^���F�ϊ��萔 = PgPuls / 60 */
		/* 	�t���N  �F�ϊ��萔 = (Pn20A * �s�b�`����̃p���X��) / 60
		     �� iniPrm.mt.PgPuls = Pn20A * �s�b�`����̃p���X��	*/
		kx = MlibPcalKxgain( iniPrm->PgPuls, iniPrm->EGearA, iniPrm->EGearB, &sx, 0 );
		kx = MlibPcalKxgain( kx, 1, 60, &sx, 0 );
	}
	mPrmCalcPow( 1, base, &num, &deno );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� �w�ߒP��/sec											*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	base, num, deno;
	LONG	kx, sx;
	LONG	ky, sy;
	LONG	ovSpd_num, ovSpd_deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	sx = sy = 0;

	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* (�M��A * OverSpd * PgPuls * cnvVal) / (�M��B * MAX_SPD24 * ScalePitch) */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxmulx( iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx );
		kx = MlibPcalKxmulx( kx, ovSpd_deno, pitch_num, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, pitch_deno, ovSpd_num, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}
	else
	{ /* ��]�^ */
		/* (�M��A * OverSpd * PgPuls * cnvVal) / (�M��B * MAX_SPD24 * 60) */
		kx = MlibPcalKxmulx( 60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� ��i���x�ɑ΂��道									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );
	sx = 0;

	/* (OverSpd * 100) / (MAX_SPD24 * ��i���x) */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, 100, iniPrm->RatSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->OverSpd, MAX_SPD24, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� min-1(RPM), mm/s										*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	sx = 0;

	/* OverSpd / MAX_SPD24 */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->OverSpd, MAX_SPD24, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	24bitOS �� �ő呬�x/40000000h									*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMon24OsToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );
	sx = 0;

	/* (OverSpd * 40000000H) / (MAX_SPD24 * �ő呬�x) */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->OverSpd, MAX_SPD24, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, ovSpd_num, ovSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�ő呬�x/40000000h �� �w�ߒP��/sec								*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonMaxToRefKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG cnvVal, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	base, num, deno;
	LONG	kx, sx;
	LONG	ky, sy;
	LONG	maxSpd_num, maxSpd_deno;
	LONG	pitch_num, pitch_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	mPrmCalcPow( 1, base, &num, &deno );
	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	sx = sy = 0;

	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		/* (�M��A * MaxSpd * PgPuls * cnvVal) / (�M��B * MAX_SPD30 * ScalePitch) */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp - 9, &pitch_num, &pitch_deno );
		kx = MlibPcalKxmulx( iniPrm->ScalePitch, iniPrm->EGearB, 0x40000000, &sx );
		kx = MlibPcalKxmulx( kx, maxSpd_deno, pitch_num, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->MaxSpd, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, pitch_deno, maxSpd_num, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}
	else
	{ /* ��]�^ */
		/* (�M��A * MaxSpd * PgPuls * cnvVal) / (�M��B * MAX_SPD30 * 60) */
		kx = MlibPcalKxmulx( 60*maxSpd_deno, iniPrm->EGearB, 0x40000000, &sx );
		kx = MlibPcalKxmulx( kx, 1, deno, &sx );
		ky = MlibPcalKxmulx( iniPrm->EGearA, iniPrm->MaxSpd*maxSpd_num, iniPrm->PgPuls, &sy );
		ky = MlibPcalKxmulx( ky, cnvVal, num, &sy );
//		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 24 );
		lRet = MlibPcalKxdivx( ky, sy, kx, sx, 48 );
	}

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�ő呬�x/40000000h �� ��i���x�ɑ΂��道						*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	maxSpd_num, maxSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	base = ( base - iniPrm->MaxSpdExp );
	sx = 0;

	/* (MaxSpd * 100) / (MAX_SPD30 * ��i���x) */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, 100, iniPrm->RatSpd, &sx, 0 );
	kx = MlibPcalKxgain( kx, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->MaxSpd, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		���x�P�ʑI���i���j�^�ϊ��j	�ő呬�x/40000000h �� min-1(RPM), mm/s							*/
/****************************************************************************************************/
static	KSGAIN pcal_SpdMonMaxToRpmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;
	LONG	maxSpd_num, maxSpd_deno;

	base = (LONG)( Prm->m3_velBase4L ) * (-1);
	sx = 0;

	/* MaxSpd / MAX_SPD30 */
	mPrmCalcPow( 1, base, &num, &deno );
	kx = MlibPcalKxgain( 1, num, deno, &sx, 0 );
	mPrmCalcPow( 1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno );
	kx = MlibPcalKxgain( kx, iniPrm->MaxSpd, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, maxSpd_num, maxSpd_deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		�g���N�P�ʑI��	�����P�ʂ̎w���v�Z�̂�														*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqPowKx( LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );

	sx = 0;
//	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 48 );

	return lRet;
}


/* 2009.01.20 Y.Oka ���󖢃T�|�[�g */
/****************************************************************************************************/
/*		�g���N�P�ʑI��	Nm(N) --> �ő��ٸ/40000000h				 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqNmToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + (-TrqExp) );

	mPrmCalcPow( 1, base, &num, &deno );
	/* �ϊ��萔 = 40000000h / �ő��ٸ(Nm(N)) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxTrq, &sx, 0 );
	kx = MlibPcalKxgain( kx, 100, iniPrm->RatTrq, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/* 2009.01.20 Y.Oka ���󖢃T�|�[�g */
/****************************************************************************************************/
/*		�g���N�P�ʑI��	�ő��ٸ/40000000h --> Nm(N)				 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqMaxToNmKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );
	/* �ϊ��萔 = �ő��ٸ(Nm(N)) / 40000000h */
	sx = 0;
	kx = MlibPcalKxgain( iniPrm->MaxTrq, iniPrm->RatTrq, 100, &sx, 0 );	/* % -> Nm */
	kx = MlibPcalKxgain( kx, 1, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		�g���N�P�ʑI��	��i�ٸ�ɑ΂��道 --> �ő��ٸ/40000000h	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqRatToMaxKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );
	/* �ϊ��萔 = 40000000h / �ő��ٸ(%) */
	sx = 0;
	kx = MlibPcalKxgain( 1, 0x40000000, iniPrm->MaxTrq, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		�g���N�P�ʑI��	�ő��ٸ/40000000h --> ��i�ٸ�ɑ΂��道	 									*/
/****************************************************************************************************/
static	KSGAIN pcal_TrqMaxToRatKx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG TrqExp, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = ( Prm->m3_trqBase4L ) * inv;
	base = ( base + TrqExp );

	mPrmCalcPow( 1, base, &num, &deno );
	/* �ϊ��萔 = �ő�g���N(%) / 40000000h */
	sx = 0;
	kx = MlibPcalKxgain( 1, iniPrm->MaxTrq, 0x40000000, &sx, 0 );
//	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 48 );

	return lRet;
}


/****************************************************************************************************/
/*		�����x�P�ʑI��	�����x(�w�ߒP��/sec^2)  --> �����x(**.*** �w�ߒP��/cyc^2)					*/
/****************************************************************************************************/
/*  USHORT	Cycle			���[�V�����������[0.25us]												*/
/*	LONG 	maxSpd_deno 	�����_�r�b�g�ʒu														*/
/****************************************************************************************************/
/* buf = rate * 2^vel_shift * 10^accr_base_unit / (4000000[.25us]/cyc[.25us])^2						*/
/* buf = rate * 2^vel_shift * cyc[.25us]^2 * 10^accr_base_unit/ (4000000)^2							*/
/****************************************************************************************************/
static	KSGAIN pcal_AccS2TOCyc2Kx( LONG maxSpd_deno, LONG cycle, LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx, kx;
	LONG	base, num, deno;

	base = (Prm->m3_accBase4L) * inv;

	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
	kx = MlibPcalKxgain( (1 << maxSpd_deno), cycle, 4000000, &sx, 0 );
	kx = MlibPcalKxgain( kx, cycle, 4000000, &sx, 0 );
	lRet = MlibPcalKxgain( kx, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************************************/
/*		�����x�P�ʑI��	�����x(�w�ߒP��/sec^2) --> �����x([2^24/OS]/cycle)							*/
/****************************************************************************************************/
static	KSGAIN pcal_AccRefToOsKx( MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	kx, sx;
	LONG	base, num, deno;
	LONG	ovSpd_num, ovSpd_deno;
	LONG	pitch_num, pitch_deno;

	base = ( Prm->m3_accBase4L );
	mPrmCalcPow( 1, base, &num, &deno );
	mPrmCalcPow( 1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno );

	if( iniPrm->LinearMotor != FALSE )
	{ /* ���j�A�^ */
		mPrmCalcPow( 1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno );
		/*------------------------------------------------------------------------------------------*/
		/*	Gain = 10^n�~ScalePitch�~(B/A)�~(1/PgPuls)�~(2^24/OS)�~cyc[.25us]						*/
		/*------------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( iniPrm->ScalePitch, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
		kx = MlibPcalKxgain( kx, pitch_num, pitch_deno, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->Cycle, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, ovSpd_deno, iniPrm->OverSpd*ovSpd_num, &sx,0 );
		kx = MlibPcalKxgain( kx, num, deno, &sx,0 );
		lRet = MlibPcalKxgain( kx, MAX_SPD24, 4000000, &sx, 24 );
	}
	else
	{ /* ��]�^ */
		/*------------------------------------------------------------------------------------------*/
		/*	Gain = 10^n�~(B/A)�~(1/PgPuls)�~60�~(2^24/OS)�~cyc[.25us]								*/
		/*------------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( 60*ovSpd_deno, iniPrm->EGearB, iniPrm->EGearA, &sx, 0 );
		kx = MlibPcalKxgain( kx, MAX_SPD24, iniPrm->PgPuls, &sx, 0 );
		kx = MlibPcalKxgain( kx, iniPrm->Cycle, iniPrm->OverSpd*ovSpd_num, &sx, 0 );
		lRet = MlibPcalKxgain( kx, num, deno*4000000, &sx, 24 );
	}

	return lRet;
}


#if 0 /* 2009.1.20 Y.Oka ���󖢑Ή� */
/****************************************************************************************************/
/*		�����x�P�ʑI��	��������(ms)  --> �����x(**.*** �w�ߒP��/cyc^2) 							*/
/****************************************************************************************************/
/*  USHORT	Cycle			���[�V�����������[0.25us]												*/
/*	LONG 	maxSpd_deno 	�����_�r�b�g�ʒu														*/
/****************************************************************************************************/
/*	�����x�P�ʑI��	��������(ms)  --> �����x(�w�ߒP��/sec^2) --> �����x(**.*** �w�ߒP��/cyc^2) 		*/
/*	�̕ϊ����s��																					*/
/****************************************************************************************************/
/* 2008.12.05 Y.Oka "inv" = 1�ł���A�{�����̎g�p�͂����݂̂ł��邽�߁A���������炷����inv���폜�B */
//LONG pcal_AccMsToCyc2Kx(LONG maxSpd_deno, USHORT cycle, CHAR inv)
static	LONG pcal_AccMsToCyc2Kx( MT_PRM *mtPrm, MTPRM_INI *iniPrm, LONG maxSpd_deno, USHORT cycle, PRMDATA *Prm )
{
	LONG	lRet, sx, kx;
	LONG	base, num, deno;
	LONG	sp;

//	base = (pprm_ptr->m3_accBase4L | (pprm_ptr->m3_accBase4H << 16)) * inv;
	base = (Prm->m3_accBase4L | (Prm->m3_accBase4H << 16));
	base = (base + iniPrm->MaxSpdExp);
	sx = 0;

	/*------------------------------------------------*/
	/* ��������(ms)  --> �����x(�w�ߒP��/sec^2)�̕ϊ� */
	/*------------------------------------------------*/
	if(iniPrm->Linear)
	{
		/* �ϊ��萔 = ���[�^��i���x(rpm) * (C022h,C023h) * 10^9  / (�����߯�(nm) * �����߯��w��) */
		sp = iniPrm->ScalePitch;
		kx = MlibPcalKxgain(iniPrm->RatSpd, iniPrm->PgPuls, 1, &sx, 0);
		kx = MlibPcalKxgain(kx, 1, sp, &sx, 0);
		base = base + 9 - iniPrm->ScalePitchExp;
	}
	else
	{
		/* �ϊ��萔 = ���[�^��i���x(�w�ߒP��/sec) */
		kx = MlibPcalKxgain(iniPrm->RatSpd, iniPrm->PgPuls, 60, &sx, 0);
	}
	kx = MlibPcalKxgain(kx, iniPrm->EGearA, iniPrm->EGearB, &sx, 0);

	/*-----------------------------------------------------------------*/
	/* �����x(�w�ߒP��/sec^2) --> �����x(**.*** �w�ߒP��/cyc^2) �̕ϊ� */
	/*-----------------------------------------------------------------*/
	kx = MlibPcalKxgain((1 << maxSpd_deno), cycle, 4000000, &sx, 0);
	kx = MlibPcalKxgain(kx, cycle, 4000000, &sx, 0);
	mPrmCalcPow(1, base, &num, &deno);
	lRet = MlibPcalKxgain(kx, num, deno, &sx, -30);

	return lRet;
}
#endif /* #if 0 */


#if 0/* 2009.1.20 Y.Oka ���󖢑Ή� */
/****************************************************************************************************/
/*		�����x�P�ʑI��	��������(ms) --> �����x([2^24/OS]/cycle)									*/
/****************************************************************************************************/
static	LONG pcal_AccMsToOsKx( MTPRM_INI *iniPrm )
{
	LONG	lRet=0x7FFFFF;	// �b�菈�u

	if(iniPrm->Linear)
	{

	}
	else
	{

	}

	return lRet;
}
#endif /* #if 0 */


/****************************************************************************************************/
/*		�ʒu�P�ʑI��	�����P�ʂ̎w���v�Z�̂�														*/
/****************************************************************************************************/
static	KSGAIN pcal_PosPowKx( LONG inv, PRMDATA *Prm )
{
	KSGAIN	lRet;
	LONG	sx;
	LONG	base, num, deno;

	base = ( Prm->m3_posBaseL ) * inv;

	mPrmCalcPow( 1, base, &num, &deno );
	sx = 0;
	lRet = MlibPcalKxgain( 1, num, deno, &sx, 24 );

	return lRet;
}


/****************************************************************************
 ���_���A�����̐ݒ�(Motion Control)
 �����Fdata=TRUE�F�������AFALSE:������
****************************************************************************/
LONG MPrmSetZretDir( MT_PRM *prm, BOOL data )
{
	if( data )
	{
		prm->MtSeqPrm.zret_dir = -DISTANCE_MAX;
	}
	else
	{
		prm->MtSeqPrm.zret_dir = DISTANCE_MAX;
	}

	return 0;
}


/****************************************************************************
 ���͐M�����j�^���t�ݒ�(Motion API)
 �����Fdata  bit00-03 IO:0���t(n=0:���t�Ȃ��An=1-8:SI[n-1]�M��)
             bit04-07 IO:1���t
                ;        ;
             bit28-31 IO:7���t
****************************************************************************//* <S004> */
LONG MPrmSetInSigMap(MT_PRM* prm, LONG data)
{
	USHORT msk = 0;
	UCHAR  map, cnt;

	for(cnt = 0; cnt < 8; cnt++)
	{
		map = (UCHAR)((data >> (cnt*4)) & 0xF);
		if(map == 0)
		{
			msk |= (0x1 << cnt);
			map = 1;
		}
		prm->InSigMap.bitNo[cnt] = map - 1;
	}
	prm->InSigMap.bitsMsk = ~msk;

	return 0;
}

/****************************************************************************
 �����x1�ݒ�(MG�p)
****************************************************************************/
LONG	MPrmSetAccRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;
#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate1 = data;
	return 0;
}


/****************************************************************************
 �����x��ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetDecAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data)
{
#if (FLOAT_USE==TRUE)
	mgPrm->altspd_d = ( (float)data * prm->BasePrm.PosingSpdKx );
#else
	mgPrm->altspd_d = MlibMulgain32( data, prm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	return 0;
}


/****************************************************************************
 �����x�؂�ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetAccAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data)
{
#if (FLOAT_USE==TRUE)
	mgPrm->altspd_a = ( (float)data * prm->BasePrm.PosingSpdKx );
#else
	mgPrm->altspd_a = MlibMulgain32( data, prm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	return 0;
}


/****************************************************************************
 �w���֐��������o�C�A�X�ݒ�(AccFilter)
****************************************************************************/
LONG MPrmSetExpFilBias(MT_PRM *prm, ACF_PRM *acfPrm, LONG bias)
{
	LONG kx, sx;
	sx = 0;

	/* buf = spd / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * cyc[.25us] / 4000000[.25us] */
	kx = MlibPcalKxgain(1, prm->BasePrm.MotionCycle, 4000000, &sx, 24);

#if (FLOAT_USE==TRUE)
	acfPrm->expbias = MlibMulgain32(bias, kx);
#else
		acfPrm->expbias = bias * kx;
#endif
	return 0;
}


/****************************************************************************
 �w���֐����������萔�ݒ�(AccFilter)
 �T�v�F�w���֐��t�B���^�̎��萔�Q�C�����v�Z���A�p�����[�^�ɐݒ�
      Kt = t / ( t + exptb)
           Kt  :���萔�Q�C��
           t   :�T���v�����O����[0.25us]
           exptb :���萔[0.25us]
 �����F
	prm		- Motion Paramter�\���̂ւ̎Q�Ɛ�
	exptb	- ���萔 [0.1ms]
****************************************************************************/
LONG MPrmSetExpFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG exptb)
{
	LONG buf;

	/* exptb = 2^vel_shift * (cyc /(exptb*400 + cyc)) */
	buf = ((1 << prm->BasePrm.vel_sft_exp) * prm->BasePrm.MotionCycle);	/* 2^vel_shift * cyc */
//	buf = buf / ((exptb*4) + prm->BasePrm.MotionCycle);/* <S033> */
	buf = buf / ((exptb*400) + prm->BasePrm.MotionCycle);/* <S033> */

	acfPrm->exptb = buf;	/* �p�����[�^�ۑ� */

	return 0;
}

/****************************************************************************
 �ړ����ώ��萔�ݒ�(AccFilter)
 �T�v�F�ړ����σt�B���^�̎��萔�v�Z���A�p�����[�^�ɐݒ�
 �����F
	prm		- Motion Paramter�\���̂ւ̎Q�Ɛ�
	matb	- ���萔 [0.1ms]
****************************************************************************/
LONG MPrmSetMaFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG matb)
{
	LONG	tb, i;
	LONG	nTb, iTb;
	MATB_PRM maPrm;

	//	tb = matb * 4;	/* [0.1ms]->[0.25us] *//* <S033> */
		tb = matb * 400;	/* [0.1ms]->[0.25us] *//* <S033> */
	if(tb >= prm->BasePrm.MotionCycle)
	{/* ���萔������T�C�N���ȏ�̏ꍇ */
		nTb = (tb / prm->BasePrm.MotionCycle);	/* �ړ����ω񐔌v�Z */
		iTb = (USHORT)(nTb / ACF_MAX_SZ);
		if(iTb > 0)
		{
			for(i = 1; i < 5; i++)
			{
				if((1<<i) > iTb) break;
			}
			tb = nTb / (1<<i);
			maPrm.wTb[0] = (USHORT)i;
			maPrm.wTb[1] = (USHORT)((tb > ACF_MAX_SZ) ? ACF_MAX_SZ: tb);
		}
		else
		{
			maPrm.wTb[0] = 0;
			maPrm.wTb[1] = (USHORT)((nTb > ACF_MAX_SZ) ? ACF_MAX_SZ: nTb);
		}
		acfPrm->mafil.lTb = maPrm.lTb;
	}
	else
	{/* ���萔������T�C�N�������̏ꍇ */
		acfPrm->mafil.lTb = 0;
	}

	return 0;
}


/****************************************************************************
 �����x2�ݒ�(MG�p)
****************************************************************************/
LONG	MPrmSetAccRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;

#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */

	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate2 = data;
	return 0;
}


/****************************************************************************
 �����x1�ݒ�(MG�p)
****************************************************************************/
LONG	MPrmSetDecRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;
#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else	
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */

	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->decrate1 = data;
	return 0;
}


/****************************************************************************
 �����x2�ݒ�(MG�p)
****************************************************************************/
LONG	MPrmSetDecRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate)
{
	LONG data;
	
#if (FLOAT_USE==TRUE)
	data = ( (float)rate * prm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( rate, prm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */

	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->decrate2 = data;
	return 0;
}


/****************************************************************************
 ���͐M�����j�^(Pn860-866)
 �����F
MT_PRM	*prm		Pt�p�����[�^
USHORT	data	bit00-07	:CN1-**���̓r�b�g�ʒu
				bit08		:�L������
			 	bit16-23	:CN1-**���̓r�b�g�ʒu
				bit24		:�L������
UCHAR	CnNum	IO			:SI00�`06(SI10�`16)�̓��͔ԍ�
****************************************************************************/
void	MPrmSetInSigMonitor(MT_PRM *prm, USHORT data, UCHAR IoNum)
{

	prm->InSigMonM3.bitNo[IoNum]     = (UCHAR)  data & 0x000F;
	if((data & 0x0010)>>4 )
	{
		prm->InSigMonM3.bitsMsk |= (USHORT)(1 << IoNum);
	}
	else
	{
		prm->InSigMonM3.bitsMsk &= ~(USHORT)(1 << IoNum);
	}

	prm->InSigMonM3.bitNo[IoNum +1]  = (UCHAR)((data & 0x0F00)>>8 );
	if((data & 0x1000)>>12 )
	{
		prm->InSigMonM3.bitsMsk |= (USHORT)(1 << (IoNum+1));
	}
	else
	{
		prm->InSigMonM3.bitsMsk &= ~(USHORT)(1 << (IoNum+1));
	}

	return;
}

//#if ( CCSW_SFTID == SFTID_EXIST )/* <S1F9> */
/****************************************************************************
 �o�͐M�����j�^(Pn868-86A)
 �����F
MTPRM*	prm		Pt�p�����[�^
USHORT	data	bit00-07	:CN1-**�o�̓r�b�g�ʒu
				bit08		:�L������
			 	bit16-23	:CN1-**�o�̓r�b�g�ʒu
				bit24		:�L������
UCHAR	CnNum	IO			:SO0�`3(5)�̓��͔ԍ�
****************************************************************************/
VOID MPrmSetOutSigMonitor(MT_PRM* prm, USHORT data, UCHAR IoNum)
{

	prm->OutSigMonM3.bitNo[IoNum]     = (UCHAR)  data & 0x000F;
	if((data & 0x0010)>>4 )
	{
		prm->OutSigMonM3.bitsMsk |= (USHORT)(1 << IoNum);
	}
	else
	{
		prm->OutSigMonM3.bitsMsk &= ~(USHORT)(1 << IoNum);
	}

	prm->OutSigMonM3.bitNo[IoNum +1]  = (UCHAR)((data & 0x0F00)>>8 );
	if((data & 0x1000)>>12 )
	{
		prm->OutSigMonM3.bitsMsk |= (USHORT)(1 << (IoNum+1));
	}
	else
	{
		prm->OutSigMonM3.bitsMsk &= ~(USHORT)(1 << (IoNum+1));
	}

	return;
}
//#endif/* <S1F9> */

/* 2009.01.22 Y.Oka ���������Ԏw��@�\ ���󖢃T�|�[�g */
/****************************************************************************
 �������w�ߗp �����x�ݒ�(Motion Control) <Rev2.01>
 �����FTacc[0.1ms], Tspd[ref/s]
****************************************************************************/
VOID MPrmCalcAccTimeRate(MT_PRM *prm, LONG Tspd, LONG Tacc, LONG Tdec)
{
#if 0
	ULONG spd;
	LONG rate,accrate;

	spd = (ULONG)MlibLABS( Tspd );
	if( Tacc == 0 )
	{
		Tacc = 1;
	}
	if( spd < 0x1000000 )
	{
		rate = ( spd << 7 ) / Tacc;
#if (FLOAT_USE==TRUE)
		rate = ( (float)rate * prm->BasePrm.taccRate7Kx );
#else
		rate = MlibMulgain32( rate, prm->BasePrm.taccRate7Kx );
#endif /* FLOAT_USE */
	}
	else
	{
		rate = ( spd + (Tacc>>1) ) / Tacc;
#if (FLOAT_USE==TRUE)
		rate = ( (float)rate * prm->BasePrm.taccRateKx );
#else
		rate = MlibMulgain32( rate, prm->BasePrm.taccRateKx );
#endif /* FLOAT_USE */
	}

	if( rate > 0x3FFFFFFF )
	{
		accrate = 0x3FFFFFFF;
	}
	else if( rate < 1 )
	{
		accrate = 1;
	}
	else
	{
		accrate = rate;
	}

	prm->CmdRate.accrate2 = accrate;
	prm->CmdRate.decrate2 = accrate;
#endif
}

#if 0/* M2�v���t�@�C���Ή�	*/
/****************************************************************************************************/
/*		TFF/TLIM�g�p�I��																			*/
/****************************************************************************************************/
VOID ��PrmTrefMode(MT_PRM* prm, MTPRM_INI* iniPrm)
{
	UCHAR	profileType = CmdGetProfileType(GetCmdHndl(0));

	if(profileType==M3STD_PROFILE)
	{
		prm->BasePrm.trefMode = TREFMODE_TFF_TLIM;
	}
	else
	{
		prm->BasePrm.trefMode = (UCHAR)iniPrm->trefMode;
	}
	prm->BasePrm.TrefModePos = ((iniPrm->OptEnable&0x00F0) != 0) ? prm->BasePrm.trefMode : 0;
	prm->BasePrm.TLimModePos = ((iniPrm->OptEnable&0x0F00) == 0) ? prm->BasePrm.TrefModePos : 0;

}
#endif

/***************************************** end of file **********************************************/
