/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BacklashComp.c : �o�b�N���b�V�␳�@�\	���W���[����`										*/
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
/*	Note	:	����	2011.04.05	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "BacklashComp.h"



#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�o�b�N���b�V�␳����																		*/
/*																									*/
/****************************************************************************************************/
void	BlshCompensateBacklash( BLSHCMP *BlshCmp, EGEAR *Egear,
									LONG *dPosRef, LONG *dPcmd, BOOL *RefZ, BOOL BlshActive )
{
	LONG	dAmountPls;
	LONG	work;		/* <S1D8> */

	/*------------------------------------------------------------------------------------------*/
	/*		�x�[�X�u���b�N���ȂǁF�␳�Ȃ��̏�Ԃɖ߂�(���[�^�ʒu�ɖ߂��j						*/
	/*------------------------------------------------------------------------------------------*/
	if( BlshActive == FALSE )
	{
		*dPosRef -= BlshCmp->var.LastFilo;
#if (FLOAT_USE==TRUE)
		*dPcmd += FlibEgearRvscnv( -(BlshCmp->var.LastFilo), Egear, &BlshCmp->var.Cmpgrrem );
#else
		*dPcmd += MlibEgearRvscnv( -(BlshCmp->var.LastFilo), Egear, &BlshCmp->var.Cmpgrrem );
#endif /* FLOAT_USE */
		BlshCmp->var.BlshCmpAmountPls = -(BlshCmp->var.LastFilo);
		work = BlshCmp->var.BlshCmpAmountPls;								/* <S1D8> */
		MlibResetLongMemory( &BlshCmp->var, sizeof(BlshCmp->var)/4 );
		BlshCmp->var.BlshCmpAmountPls = work;			/* �o�b�N���b�V�␳�ʍ����l�́A�ΏۊO	*//* <S1D8> */

	}
	/*------------------------------------------------------------------------------------------*/
	/*		�x�[�X�C�l�[�u�����F�o�b�N���b�V�␳�������Z										*/
	/*------------------------------------------------------------------------------------------*/
	else
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�o�b�N���b�V�␳�������ʁA�␳�ʐݒ�												*/
		/*--------------------------------------------------------------------------------------*/
		if( *dPosRef != 0 )
		{
			if( (*dPosRef > 0) && (BlshCmp->conf.BacklashDir == FOR_BACKLASH) )
			{
				BlshCmp->var.Cmppos = BlshCmp->conf.CompPulse;
			}
			else if( (*dPosRef < 0) && (BlshCmp->conf.BacklashDir == REV_BACKLASH) )
			{
				BlshCmp->var.Cmppos = -BlshCmp->conf.CompPulse;
			}
			else
			{
				BlshCmp->var.Cmppos = 0;
			}
		}
		/*--------------------------------------------------------------------------------------*/
		/*	�o�b�N���b�V�␳�ʃt�B���^����														*/
		/*--------------------------------------------------------------------------------------*/
		dAmountPls = MlibMulkxBiasrem( BlshCmp->var.Cmppos - BlshCmp->var.LastFilo,
									   BlshCmp->conf.Kfil,
									   &BlshCmp->var.remx,
									   0 );
		BlshCmp->var.Filo = BlshCmp->var.LastFilo + dAmountPls;		/* ���݂܂ł̕␳��			*/
		BlshCmp->var.LastFilo = BlshCmp->var.Filo;					/* �t�B���^�o�͑O��l�X�V	*/
		/*--------------------------------------------------------------------------------------*/
		/*	�o�b�N���b�V�␳�ʂ̎w�߉��Z����													*/
		/*		*dPosRef�F�p���X�P�ʂ̈ʒu�w�߂ɉ��Z�A*dPcmd�F�w�ߒP�ʂ̈ʒu�w�߂ɉ��Z			*/
		/*--------------------------------------------------------------------------------------*/
		*dPosRef += dAmountPls;
#if (FLOAT_USE==TRUE)
		*dPcmd += FlibEgearRvscnv( dAmountPls, Egear, &BlshCmp->var.Cmpgrrem );
#else
		*dPcmd += MlibEgearRvscnv( dAmountPls, Egear, &BlshCmp->var.Cmpgrrem );
#endif /* FLOAT_USE */
		/*--------------------------------------------------------------------------------------*/
		/*	FB�ʒu��񃂃j�^�����p�o�b�N���b�V�␳�ʍ����l�X�V									*/
		/*		BoutV.BlshCmpAmountPls�F�p���X�P�ʂ�FB�ʒu�����l����̌��Z�p					*/
		/*--------------------------------------------------------------------------------------*/
		BlshCmp->var.BlshCmpAmountPls = dAmountPls;
	}
	/*------------------------------------------------------------------------------------------*/
	/*	�ʒu�w�ߕ����o�������iDEN�M���j�X�V														*/
	/*		�␳�ʂ����Z�����w��!=0 or �␳�ʃt�B���^�o�b�t�@!=0�FDEN�M��=FALSE(����)			*/
	/*------------------------------------------------------------------------------------------*/
	if( (*dPosRef!=0) || ((BlshCmp->var.Cmppos - BlshCmp->var.LastFilo) != 0) )
	{
		*RefZ = FALSE;
	}
	else
	{
		/* ��L�ȊO�̏ꍇ�A�␳�t�B���^�Q�C���̃p�����[�^�L�� */
		BlshCmp->conf.Kfil = BlshCmp->conf.KfilTmp;
	}

	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�o�b�N���b�V�␳ �␳�ʐ����l�v�Z															*/
/*																									*/
/****************************************************************************************************/
void	BlshCalcMaxBlshCompPulse( BLSHCMP *BlshCmp, EGEAR *Egear, BPRMDAT *Bprm )
{
	/*----------------------------------------------------------------------------------------------*/
	/*	2�~���[�^�ő呬�x�p���X�v�Z�iMaxCompPulse�j�F�p���X�P��/sec���p���X�P��/ScanB[us]			*/
	/*		Bprm.MaxPspd�F[Pulse/sec]																*/
	/*----------------------------------------------------------------------------------------------*/
	BlshCmp->conf.MaxCompPulse = MlibScalKxkskx( Bprm->SvCycleUs, Bprm->MaxPspd, 500000, NULL, -30 );
//	blcm2_UpLim = ULONGOF( pndef_blcm2l.UpperLimit, pndef_blcm2h.UpperLimit );

	/*----------------------------------------------------------------------------------------------*/
	/*	�o�b�N���b�V�␳�ʐ����l�v�Z�i���j�^�p�j   �F��ٽ�P��/ScanB[us]��0.1�w�ߒP��/ScanB[us]		*/
	/*----------------------------------------------------------------------------------------------*/
	BlshCmp->conf.MaxCompRef =
		(ULONG)MlibPcalKxgain( BlshCmp->conf.MaxCompPulse*10, Egear->a, Egear->b, NULL, -30 );
	BlshCmp->conf.MaxCompRef =
		( BlshCmp->conf.MaxCompRef > BLSHCOMP_UPLIM )? BLSHCOMP_UPLIM : BlshCmp->conf.MaxCompRef;

	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�o�b�N���b�V�␳ �␳�ʁiPn231�j�p�����[�^�v�Z												*/
/*																									*/
/****************************************************************************************************/
void	BlshCalcCompensateAmount( BLSHCMP *BlshCmp, EGEAR *Egear, LONG blcm2l )
{
	LONG	lwk;
	LONG	kx;
	LONG	sx;

	/*----------------------------------------------------------------------------------------------*/
	/*	Pn231�APn232�v�Z�iPrmCompPulse�j�F�w�ߒP�ʁ��p���X�P��										*/
	/*----------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( blcm2l, 1, 10, &sx, 0 );
	lwk = MlibPcalKxgain( kx, Egear->b, Egear->a, &sx, -30 );

	/*----------------------------------------------------------------------------------------------*/
	/*	�␳�ʃp���X���~�b�g�����iBlshCmp->conf.CompPulse�j											*/
	/*		�␳�ʂ����[�^�ő呬�x��2�{�̃p���X�ȏ�ł���΁A���[�^�ő呬�x��2�{�̃p���X�ɂ���		*/
	/*----------------------------------------------------------------------------------------------*/
	BlshCmp->conf.CompPulse = MlibLimitul(lwk, BlshCmp->conf.MaxCompPulse, -BlshCmp->conf.MaxCompPulse);
	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�o�b�N���b�V�␳ �␳���萔�iPn233�j�p�����[�^�v�Z											*/
/*																									*/
/****************************************************************************************************/
void	BlshCalcCompensateTime( BLSHCMP *BlshCmp, LONG SvCycleUs, USHORT blfiltime )
{
	BlshCmp->conf.KfilTmp = MlibScalKxgain( SvCycleUs, 1, (SvCycleUs + (blfiltime * 10)), NULL, 24 );
	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�o�b�N���b�V�␳ �␳��Ԗ�������															*/
/*																									*/
/****************************************************************************************************/
void	BlshInvaildCompensate( BLSHCMP *BlshCmp )
{
	LONG work;							/* <S1D8> */

	/* �␳��Ԗ��������FAPOS=FB�ʒu�ɂ��� */
	BlshCmp->var.BlshCmpAmountPls = -BlshCmp->var.LastFilo;
	/* �␳�t�B���^�Q�C���̃p�����[�^�L�� */
	BlshCmp->conf.Kfil = BlshCmp->conf.KfilTmp;
	/* �o�b�N���b�V�␳�ϐ������� */
	work = BlshCmp->var.BlshCmpAmountPls;							/* <S1D8> */
	MlibResetLongMemory( &BlshCmp->var, sizeof(BlshCmp->var)/4 );			
	BlshCmp->var.BlshCmpAmountPls = work;			/* �o�b�N���b�V�␳�ʍ����l�́A�ΏۊO	*/	/* <S1D8> */
	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		�o�b�N���b�V�␳ �␳�ʏ���������															*/
/*																									*/
/****************************************************************************************************/
void	BlshInitializeCompensate( BLSHCMP *BlshCmp )
{
	/* �␳�t�B���^�Q�C���̃p�����[�^�L�� */
	BlshCmp->conf.Kfil = BlshCmp->conf.KfilTmp;
	/* �o�b�N���b�V�␳�ϐ������� */
	MlibResetLongMemory( &BlshCmp->var, sizeof(BlshCmp->var)/4 );
	return;
}
#endif



/***************************************** end of file **********************************************/
