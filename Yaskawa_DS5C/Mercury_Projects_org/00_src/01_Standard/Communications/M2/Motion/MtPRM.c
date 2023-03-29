/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion PRM �\�[�X�t�@�C��											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	�����x�w�ߒǉ�									*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*	<V744>	2010.04.16	K.Ando												*/
/*		<< �ړ����ώ��Ԃ��p�����[�^�l�ʂ�ɓ��삵�Ȃ��s��C�� >>			*/
/*		�E�s�No�FA0329													*/
/*		�E���ہF256.0ms - 384.0ms�͈̔͂ɂ�256.0ms�œ��삵�Ă��܂�			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

/* includes */
#include "Basedef.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"

/****************************************************************************
 Private functions
****************************************************************************/
/*  �ׂ��扉�Z���� */
static	VOID mPrmCalcPow( LONG data, LONG exp, LONG *num, LONG *deno );

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
	FullcCtrl		;�ٸ۰���ސ���
	Linear			;���j�A�g�p
	absPG			;��޿PG�g�p
	ot_zstop;		;OT����۸����
	trefMode		;TRef Option
	OptMap			;OPTION�w�ߊ��t
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *mtPrm, MTPRM_INI *iniPrm, PRMDATA *Prm )
{
	LONG kx, sx;
	LONG ky, sy;
	LONG maxSpd_num, maxSpd_deno;
	LONG ovSpd_num, ovSpd_deno;
	LONG pitch_num, pitch_deno;

/* ���[�V�����T�C�N���Z�b�g */
	mtPrm->BasePrm.MotionCycle = iniPrm->Cycle;
//	iniPrm->SpdDiv = MG_DEFAULT_SPDDIV;				/* <S006> *//* S06C /

/* ���x�̒P�ʕϊ��萔�̌v�Z[ref/�ʐM����]->[ref/s] */
	sx = 0;
	mtPrm->BasePrm.fspdRefKx = MlibPcalKxgain(1, 4000000, iniPrm->Cycle, &sx, 24);	

/* �g���N�̒P�ʕϊ��萔�̌v�Z[24bit/MAX]->[%] */
	sx = 0;
	mtPrm->BasePrm.trqKx = MlibPcalKxgain(1, iniPrm->MaxTrq, MAX_TRQ24, &sx, 24);	

	mPrmCalcPow(1, iniPrm->MaxSpdExp, &maxSpd_num, &maxSpd_deno);
	mPrmCalcPow(1, iniPrm->OverSpdExp, &ovSpd_num, &ovSpd_deno);
	
/* ���x�̒P�ʕϊ��萔�̌v�Z[30bit/MAX]->[24bit/OS] */
	/* buf = spd * maxSpd/(ovSpd*64) */
	/* buf = spd * maxSpd * (maxSpd_num/maxSpd_deno) / (ovSpd*(ovSpd_num/ovSpd_deno)*64) */
	sx = sy = 0;
	kx = MlibPcalKxmulx(iniPrm->MaxSpd, maxSpd_num, ovSpd_deno, &sx);
	ky = MlibPcalKxmulx(iniPrm->OverSpd*64, ovSpd_num, maxSpd_deno, &sy);
	mtPrm->BasePrm.maxToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
	
/* ���x�̒P�ʕϊ��萔�̌v�Z[24bit/OS]->[30bit/MAX] */
	mtPrm->BasePrm.osToMaxKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);

/* ���x�̒P�ʕϊ��萔�̌v�Z[�w�ߒP��/s]->[24bit/OS] */
/* ���x�̒P�ʕϊ��萔�̌v�Z[24bit/OS]->[�w�ߒP��/s] */
	if( iniPrm->LinearMotor != FALSE )
	{
		sx = sy = 0;
		mPrmCalcPow(1, iniPrm->ScalePitchExp-9, &pitch_num, &pitch_deno);
		kx = MlibPcalKxmulx(iniPrm->ScalePitch, iniPrm->EGearB, MAX_SPD24, &sx);
		kx = MlibPcalKxmulx(kx, ovSpd_deno, pitch_num, &sx);
		ky = MlibPcalKxmulx(iniPrm->EGearA, iniPrm->OverSpd, iniPrm->PgPuls, &sy);  
		ky = MlibPcalKxmulx(ky, pitch_deno, ovSpd_num, &sy);                                     
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);
	}
	else
	{
		sx = sy = 0;
		kx = MlibPcalKxmulx(60*ovSpd_deno, iniPrm->EGearB, MAX_SPD24, &sx);
		ky = MlibPcalKxmulx(iniPrm->EGearA, iniPrm->OverSpd*ovSpd_num, iniPrm->PgPuls, &sy);
		mtPrm->BasePrm.refToOsKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);
		mtPrm->BasePrm.osToRefKx = MlibPcalKxdivx(ky, sy, kx, sx, 24);
	}

/* ���x�̒P�ʕϊ��萔�̌v�Z[**.*** ref/�������]��[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx(1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx((1 << iniPrm->SpdDiv), iniPrm->Cycle, 1, &sy);
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	
	mtPrm->BasePrm.PosingSpdMax = MlibMulgain32(0x40000000, mtPrm->BasePrm.PosingSpdInvKx);
/* POSING����\�Z�o --OverSpeed���x���������_�ʒu�Ƃ��� */
	ovSpd_num = MlibMulgain32(MAX_SPD24, mtPrm->BasePrm.osToRefKx);	/* Over Speed [ref/s] */
	for(sx = MG_SPDDIV_RANGE; sx > 0; sx--)
	{
		maxSpd_num = mtPrm->BasePrm.PosingSpdMax >> sx;
		if(ovSpd_num < maxSpd_num)
		{
			break;
		}
	}
	maxSpd_deno = iniPrm->SpdDiv + sx;	/* �����_�r�b�g�ʒu */

/* �������萔�̌v�Z[10000 x ref/s^2]��[**.*** ref/�������^2] */
	/* buf = rate * 2^vel_shift * 10000 / (4000000[.25us]/cyc[.25us])^2 */
	/* buf = rate * 2^vel_shift * cyc[.25us]^2 / (40000)^2 */
	sx = 0;
	kx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 40000, &sx, 0);
	mtPrm->BasePrm.PosingRateKx = MlibPcalKxgain(kx, iniPrm->Cycle, 40000, &sx, 24);
	
/* ���x�P�ʕϊ��萔�̌v�Z[ref/s]��[**.*** ref/�������] */
	/* buf = spd * 2^vel_shift / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * 2^vel_shift * cyc[.25us] / 4000000[.25us] */
	sx = 0;
	mtPrm->BasePrm.PosingSpdKx = MlibPcalKxgain((1 << maxSpd_deno), iniPrm->Cycle, 4000000, &sx, 24);
	
/* ���x�̒P�ʕϊ��萔�̌v�Z[**.*** ref/�������]��[ref/s] */
	/* buf = spd * 4000000[.25us] / (2^vel_shift * cyc[.25us]) */
	sx = sy = 0;
	kx = MlibPcalKxmulx(1, 4000000, 1, &sx);
	ky = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, 1, &sy);
	mtPrm->BasePrm.PosingSpdInvKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	
	mtPrm->BasePrm.mgSpdMax = MlibMulgain32(0x40000000, mtPrm->BasePrm.PosingSpdInvKx);

/* MG_BASE_PRM�Z�b�g */
	mtPrm->BasePrm.vel_sft_exp = iniPrm->SpdDiv;
	mtPrm->BasePrm.vel_shift = maxSpd_deno;
	mtPrm->BasePrm.vel_shift_s = (MG_VREMBIT_MAX - maxSpd_deno);
	mtPrm->BasePrm.vel_max = 1 << (31 - maxSpd_deno);
	mtPrm->BasePrm.seg_num = iniPrm->SegNum;

	sx = 0;
	kx = MlibPcalKxgain(mtPrm->BasePrm.PosingSpdMax, iniPrm->Cycle, 4000000, &sx, 24);
	mtPrm->BasePrm.seg_max_ref = MlibMulgain32(iniPrm->SegNum, kx);

//	mtPrm->BasePrm.FullcCtrl = iniPrm->FullcCtrl;	/* �ٸ۰���ސ��� */ /* ���g�p	*/
//	mtPrm->BasePrm.Linear = iniPrm->Linear;		/* ���j�A�g�p */ /* ���g�p	*/
//	mtPrm->BasePrm.ot_zstop = iniPrm->ot_zstop;/* ���g�p	*/

/* ACC_TIME�p�����x�P�ʕϊ��萔�̌v�Z[(ref/s)/(0.1ms)]��[**.*** ref/�������^2] <Rev2.01>*/
	sx = sy = 0;
	kx = MlibPcalKxmulx((1 << maxSpd_deno), iniPrm->Cycle, iniPrm->Cycle, &sx);
	ky = MlibPcalKxmulx(40000, 40000, 1, &sy);
	mtPrm->BasePrm.taccRateKx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/* ACC_TIME�p�����x�P�ʕϊ��萔�̌v�Z[(2^7*ref/s)/(0.1ms)]��[**.*** ref/�������^2] <Rev2.01>*/
	sy = 0;
	ky = MlibPcalKxmulx(40000, 40000, (1<<7), &sy);
	mtPrm->BasePrm.taccRate7Kx = MlibPcalKxdivx(kx, sx, ky, sy, 24);	

/* TFF/TLIM/ACC_TIME�g�p�I�� <Rev2.01>*/
	mtPrm->BasePrm.trefMode = (UCHAR)iniPrm->trefMode;
	mtPrm->BasePrm.TrefModePos = ((iniPrm->OptEnable&0x00F0) != 0) ? iniPrm->trefMode : 0;
	mtPrm->BasePrm.TLimModePos = ((iniPrm->OptEnable&0x0F00) == 0) ? mtPrm->BasePrm.TrefModePos : 0;
	mtPrm->BasePrm.SetCmdAccRate = ((iniPrm->OptEnable&0x0F00) != 0) ? &MPrmCalcAccTimeRate : NULL;

/* �ʒu���֘APRM�Z�b�g */
	mtPrm->PsnPrm.absPG = iniPrm->absPG;		/* ��޿PG�g�p */
	mtPrm->PsnPrm.TdlyIn = iniPrm->dlyTime;
	mtPrm->PsnPrm.SegNum = iniPrm->SvCycle;

/* Option�w�ߊ��t��` */
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

static VOID mPrmCalcPow(LONG data, LONG exp, LONG* num, LONG* deno)
{
	if(exp > 0)
	{
		while((exp--) > 0)
			data *= 10;
		*num = data;
		*deno = 1;
	}
	else if(exp < 0)
	{
		exp = (ULONG)MlibLABS(exp);
		while((exp--) > 0)
			data *= 10;
		*num = 1;
		*deno = data;
	}
	else
	{
		*num = data;
		*deno = 1;
	}
}


/****************************************************************************
 �����x1�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetAccRate1(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate1 = data;
	return 0;
}

/****************************************************************************
 �����x2�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetAccRate2(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->accrate2 = data;
	return 0;
}

/****************************************************************************
 �����x�؂�ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetAccAltSpd(MT_PRM* prm, MG_PRM* mgPrm, LONG data)
{
	mgPrm->altspd_a = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}

/****************************************************************************
 �����x1�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetDecRate1(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
		data = 0x3FFFFFFF;
	mgPrm->decrate1 = data;
	return 0;
}

/****************************************************************************
 �����x2�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetDecRate2(MT_PRM* prm, MG_PRM* mgPrm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	mgPrm->decrate2 = data;
	return 0;
}

/****************************************************************************
 �����x��ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetDecAltSpd(MT_PRM* prm, MG_PRM* mgPrm, LONG data)
{
	mgPrm->altspd_d = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}

/****************************************************************************
 ����~�p�����x1�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetStopDecRate1(MT_PRM* prm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	prm->BasePrm.StopPrm.decrate1 = data;
	return 0;
}

/****************************************************************************
 ����~�p�����x2�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetStopDecRate2(MT_PRM* prm, LONG rate)
{
	LONG data;
	data = MlibMulgain32(rate, prm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	prm->BasePrm.StopPrm.decrate2 = data;
	return 0;
}

/****************************************************************************
 ����~�p�����x��ւ����x�ݒ�(MG�p)
****************************************************************************/
LONG MPrmSetStopDecAltSpd(MT_PRM* prm, LONG data)
{
	prm->BasePrm.StopPrm.altspd_d = MlibMulgain32(data, prm->BasePrm.PosingSpdKx);
	return 0;
}


/****************************************************************************
 �w���֐��������o�C�A�X�ݒ�(AccFilter)
****************************************************************************/
LONG MPrmSetExpFilBias(MT_PRM* prm, ACF_PRM* acfPrm, LONG bias)
{
	LONG kx, sx=0;

	/* buf = spd / (4000000[.25us]/cyc[.25us]) */
	/* buf = spd * cyc[.25us] / 4000000[.25us] */
	kx = MlibPcalKxgain(1, prm->BasePrm.MotionCycle, 4000000, &sx, 24);
	acfPrm->expbias = MlibMulgain32(bias, kx);
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
LONG MPrmSetExpFilTb(MT_PRM* prm, ACF_PRM* acfPrm, ULONG exptb)
{
	LONG buf;

	/* exptb = 2^vel_shift * (cyc /(exptb*4 + cyc)) */
	buf = ((1 << prm->BasePrm.vel_sft_exp) * prm->BasePrm.MotionCycle);	/* 2^vel_shift * cyc */
	buf = buf / ((exptb*400) + prm->BasePrm.MotionCycle);

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
LONG MPrmSetMaFilTb(MT_PRM* prm, ACF_PRM* acfPrm, ULONG matb)
{
	LONG	tb, i;
	LONG	nTb, iTb;
	MATB_PRM maPrm;

	tb = matb * 400;	/* [0.1ms]->[0.25us] */
	if(tb >= prm->BasePrm.MotionCycle)
	{/* ���萔������T�C�N���ȏ�̏ꍇ */
		nTb = (tb / prm->BasePrm.MotionCycle);	/* �ړ����ω񐔌v�Z */
		iTb = (USHORT)(nTb / ACF_MAX_SZ);
		if(iTb > 0)
		{
			for(i = 1; i < 5; i++)
			{
/* <V744>		if((1<<i) >= iTb) break;	*/
				if((1<<i) > iTb)
				{
				 	break;
				}
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
 ���_���A�����̐ݒ�(Motion Control)
 �����Fdata=TRUE�F�������AFALSE:������
****************************************************************************/
LONG MPrmSetZretDir(MT_PRM* prm, BOOL data)
{
	if(data)
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
****************************************************************************/
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
 �������w�ߗp �����x�ݒ�(Motion Control) <Rev2.01>
 �����FTacc[0.1ms], Tspd[ref/s]
****************************************************************************/
VOID MPrmCalcAccTimeRate(MT_PRM* prm, LONG Tspd, LONG Tacc, LONG Tdec)
{
	ULONG spd;
	LONG rate,accrate;

	spd = (ULONG)MlibLABS(Tspd);
	if(Tacc == 0)
	{
		Tacc = 1;
	}
	
	if(spd < 0x1000000)
	{
		rate = (spd << 7) / Tacc;
		rate = MlibMulgain32(rate, prm->BasePrm.taccRate7Kx);
	}
	else
	{
		rate = (spd + (Tacc>>1)) / Tacc;
		rate = MlibMulgain32(rate, prm->BasePrm.taccRateKx);
	}
	accrate = MlibLIMITUL(rate, 0x3FFFFFFF, 1);

	prm->CmdRate.accrate2 = accrate;
	prm->CmdRate.decrate2 = accrate;
}
