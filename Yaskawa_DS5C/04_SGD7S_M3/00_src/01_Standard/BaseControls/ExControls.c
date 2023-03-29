/****************************************************************************************************/
/*																									*/
/*																									*/
/*		�g�����䉉�Z�֐���`																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			1. �������X���䉉�Z																		*/
/*			2. �O���I�u�U�[�o(���C�⏞)���Z															*/
/*			3. �ʑ��⏞���x�I�u�U�[�o���Z															*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2013.08.17	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "ExControls.h"
#include "BaseControlPrmCalc.h"


/****************************************************************************************************/
/*		Local Function Definition																	*/
/****************************************************************************************************/
/* �������X���o�X�g�⏞�퉉�Z */
//static	void	tuneLessRobustCompenSater( TUNELESS_DRCOMP *Drcomp, LONG MotSpd, LONG TrqRef );
static void	tuneLessRobustCompenSater( void *pdrcmp, LONG MotSpd, LONG TrqRef );	/* <S1A5> */
/* �������X�ʑ��⏞���x�I�u�U�[�o���Z */
static void tuneLessEhSpeedObserver( TUNELESS_EHVOBS *EhVobs, LONG MotSpd, LONG TrqRef );

/* <S1A5> �Đ݌v�ŊO���I�u�U�[�o			*/
static void	tuneLessRobustCompensatorEx( void *pprm, LONG MotSpd, LONG TrqRef );
/* <S1A5> �Q���t�B���^���Z(�o�ꎟ�ϊ�) 		*/
static LONG	tuneLessLowPassfilter2Ex( LONG u, LONG k[5], LONG z[4] );



/****************************************************************************************************/
/*																									*/
/*		�������X�ϐ�������																			*/
/*																									*/
/****************************************************************************************************/
void	TuneLessCtrlInitialize( TUNELESS_CTRL *TuneLessCtrl )
{
	/* �������X�p�O���I�u�U�[�o�ϐ������� */
	MlibResetLongMemory( &(TuneLessCtrl->Drcomp.var), sizeof(TuneLessCtrl->Drcomp.var)>>2 );
	/* �������X�p�ڑ��⏞���x�I�u�U�[�o�ϐ������� */
	MlibResetLongMemory( &(TuneLessCtrl->EhVobs.var), sizeof(TuneLessCtrl->EhVobs.var)>>2 );
	/* �������X�ϐ������� */
	MlibResetLongMemory( &(TuneLessCtrl->var), sizeof(TuneLessCtrl->var)>>2 );
	/* <S1A5> */
	TuneLessInitRobustCompensator( TuneLessCtrl, &(TuneLessCtrl->Drcomp) );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�������X����																				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v :	�������X�p���o�X�g�⏞�킨��шʑ��⏞���x�I�u�U�[�o���R�[������						*/
/*																									*/
/*	arg		: LONG	MotSpd					: ���[�^���x				[2^24/OvrSpd]				*/
/*			  LONG	TrqRefi					: �t�B���^�O�g���N�w��		[2^24/MaxTrq]				*/
/*			  LONG	TrqRef					: �g���N�w��				[2^24/MaxTrq]				*/
/*	out		: 																						*/
/*	g-out	: LONG	AoutV.TrqFBCx 			: �g���N�e�a�⏞			[2^24/MaxTrq]				*/
/*			  LONG	AoutV.SpdFbki			: ���x�t�B�[�h�o�b�N		[2^24/OvrSpd]				*/
/*			  LONG	BoutV.AmonDisTrq		: �O���g���N				[2^24/MaxTrq]				*/
/*			  LONG	BoutV.AmonDisTrqCmp 	: �⏞�g���N				[2^24/MaxTrq]				*/
/*			  LONG	BoutV.AmonSpdObs		: ���葬�x					[2^24/OvrSpd]				*/
/*																									*/
/****************************************************************************************************/
void	TuningLessCtrl( TUNELESS_CTRL *TuneLessCtrl, KSGAIN MotSpd, KSGAIN TrqRefi, KSGAIN TrqRef )			/*<S00A>*/
{
	TUNELESS_DRCOMP		*Drcomp;
	TUNELESS_EHVOBS		*EhVobs;

	Drcomp = &(TuneLessCtrl->Drcomp);
	EhVobs = &(TuneLessCtrl->EhVobs);

/*--------------------------------------------------------------------------------------------------*/
/*		�������X�p���xFB�t�B���^																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	TuneLessCtrl->var.SpdFbFilo = FlibLpfilter1( MotSpd, Drcomp->conf.KVfbFil, TuneLessCtrl->var.SpdFbFilo );
#else
	TuneLessCtrl->var.SpdFbFilo = MlibLpfilter1( MotSpd, Drcomp->conf.KVfbFil, TuneLessCtrl->var.SpdFbFilo );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		���o�X�g�⏞�퉉�Z																			*/
/*--------------------------------------------------------------------------------------------------*/
//<S1A5>	tuneLessRobustCompenSater( Drcomp, TuneLessCtrl->var.SpdFbFilo, TrqRef );
		Drcomp->conf.RobustCompensator( Drcomp, TuneLessCtrl->var.SpdFbFilo, TrqRef );	/* <S1A5> */

/*--------------------------------------------------------------------------------------------------*/
/*		�ʑ��⏞���x�I�u�U�[�o���Z																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->var.TuneLessAct == FALSE)
	{ /* �������X���� */
		/* �������X�ł�JL��������Ȃ��̂Ńg���N�w�߃[���ő��x�𐄒� */
		tuneLessEhSpeedObserver( EhVobs, MotSpd, 0 );
	}
	else
	{
		tuneLessEhSpeedObserver( EhVobs, MotSpd, TrqRefi );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�������X�L��/������Ԑݒ�																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->var.TuneLessAct == TRUE)
	{ /* �������X�L���� */
		TuneLessCtrl->Drcomp.var.CurrentStatus = TRUE;		/* �������X�L����Ԑݒ�					*/
	}
	else
	{ /* �������X������ */
		TuneLessCtrl->Drcomp.var.CurrentStatus = FALSE;		/* �������X������Ԑݒ�					*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�@�\OFF/ON����������																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (TuneLessCtrl->Drcomp.var.CurrentStatus == FALSE)		/* ���񖳌�							*/
		&& (TuneLessCtrl->Drcomp.var.LastStatus == TRUE) )		/* �O��L��							*/
	{ /* �������X�L���˖��� */
		/* �g���N�w�߃t�B���^�ϕ��������v�� */
		Drcomp->var.TrqFilClrReq = TRUE;
	}
	else if( (TuneLessCtrl->Drcomp.var.CurrentStatus == TRUE)	/* ����L��							*/
		&& (TuneLessCtrl->Drcomp.var.LastStatus == FALSE) )		/* �O�񖳌�							*/
	{ /* �������X�����˗L�� */
		/* �g���N�w�߃t�B���^�ϕ��������v�� */
		Drcomp->var.TrqFilClrReq = TRUE;
	}
	else
	{ /* �������X�L���˗L�� or �����˖��� */
		/* �g���N�w�߃t�B���^�ϕ��������v���Ȃ� */
		Drcomp->var.TrqFilClrReq = FALSE;
	}

	/* �O�񒲐����X�@�\��ԍX�V */
	TuneLessCtrl->Drcomp.var.LastStatus = TuneLessCtrl->Drcomp.var.CurrentStatus;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�������X���o�X�g�⏞�퉉�Z(SGDV�݊�����)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v :	�g���N�w�߂���э������x�Ɋ�Â��ϓ��g���N�𐄒肵�A									*/
/*			�g���N�w�߂ɑ����ĐV���ȃg���N�w�߂Ƃ���B												*/
/*				Tr1 = Tr0 + Kf * LPF{Tr0 - LPF(J*d��m/dt)}											*/
/*																									*/
/*	arg		: LONG	MotSpd					: ���x�t�B�[�h�o�b�N		[2^24/OvrSpd]				*/
/*			  LONG	TrqRef					: �g���N�w��				[2^24/MaxTrq]				*/
/*	out		: 																						*/
/*	g-out																							*/
/*																									*/
/****************************************************************************************************/
//<S1A5> static	void	tuneLessRobustCompenSater( TUNELESS_DRCOMP *Drcomp, LONG MotSpd, LONG TrqRef )
void	tuneLessRobustCompenSater( void *pdrcmp, LONG MotSpd, LONG TrqRef )	/* <S1A5> */
{
TUNELESS_DRCOMP *Drcomp = (TUNELESS_DRCOMP *)pdrcmp;
#if (FLOAT_USE==TRUE)
	float	fwk;


	Drcomp->var.VelLpfo2 = FlibLpfilter1( MotSpd, Drcomp->conf.Krlpf2, Drcomp->var.VelLpfo2 );
	Drcomp->var.VelLpfo1 = FlibLpfilter1( Drcomp->var.VelLpfo2, Drcomp->conf.Krlpf1, Drcomp->var.VelLpfo1 );

	fwk = Drcomp->var.VelLpfo2 - Drcomp->var.VelLpfo1;
	fwk = fwk * Drcomp->conf.Krsj;

	/* �g���N�⏞�l[2^24/MaxTrq] */
	Drcomp->var.DisTrq = FlibLimitul( (fwk - Drcomp->var.TrqLpfo_old), 0x1000000, -0x1000000 );
	fwk = Drcomp->var.DisTrq * Drcomp->conf.Krd;

	/* �t�B���^��g���N�⏞�l[2^24/MaxTrq] */
	Drcomp->var.DisTrqCmp = FlibLpfilter1( fwk, Drcomp->conf.KCmpOutlpf, Drcomp->var.DisTrqCmp );

	Drcomp->var.TrqLpfo_old = Drcomp->var.TrqLpfo;
	Drcomp->var.TrqLpfo = FlibLpfilter1( TrqRef, Drcomp->conf.Krlpf1, Drcomp->var.TrqLpfo );

	return;
#else
	LONG	lwk;

	Drcomp->var.VelLpfo2 = MlibLpfilter1( MotSpd, Drcomp->conf.Krlpf2, Drcomp->var.VelLpfo2 );
	Drcomp->var.VelLpfo1 = MlibLpfilter1( Drcomp->var.VelLpfo2, Drcomp->conf.Krlpf1, Drcomp->var.VelLpfo1 );

	lwk = Drcomp->var.VelLpfo2 - Drcomp->var.VelLpfo1;
	lwk = MlibMulgain29( lwk, Drcomp->conf.Krsj );

	/* �g���N�⏞�l[2^24/MaxTrq] */
	Drcomp->var.DisTrq = MlibLimitul( (lwk - Drcomp->var.TrqLpfo_old), 0x1000000, -0x1000000 );
	lwk = MlibMulgainNolim( Drcomp->var.DisTrq, Drcomp->conf.Krd );

	/* �t�B���^��g���N�⏞�l[2^24/MaxTrq] */
	Drcomp->var.DisTrqCmp = MlibLpfilter1( lwk, Drcomp->conf.KCmpOutlpf, Drcomp->var.DisTrqCmp );

	Drcomp->var.TrqLpfo_old = Drcomp->var.TrqLpfo;
	Drcomp->var.TrqLpfo = MlibLpfilter1( TrqRef, Drcomp->conf.Krlpf1, Drcomp->var.TrqLpfo );

	return;
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		�������X�ʑ��⏞���x�I�u�U�[�o���Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v :	�ʑ��⏞���x�I�u�U�[�o�̐��葬�x�v�Z���s���B											*/
/*			�g���N�t�B���^���萔���P�O[�ʂ�]�ȉ��̏ꍇ�C���U���݂ɂȂ��Ă��܂����߁C				*/
/*			�Ƃ肠�����C�����l���Q�O[�ʂ�]�Ƃ���B													*/
/*			�������C���̉����l�́C���x�I�u�U�[�o�̂����s���Z�o�݂̂ɓK�p�����B					*/
/*			���������āC�g���N�t�B���^���̓p�����[�^�ݒ�l�ʂ�ɂȂ�B								*/
/*																									*/
/*	arg			: MotSpd					: �������x				[2^24/nos]						*/
/*				  TrqRef					: �g���N�w��			[2^24/Tqmax]					*/
/*	out			: EhSpdObs					: ���葬�x				[2^24/nos]						*/
/* 	g-out		: 																					*/
/*																									*/
/****************************************************************************************************/
static	void	tuneLessEhSpeedObserver( TUNELESS_EHVOBS *EhVobs, LONG MotSpd, LONG TrqRef )
{
#if (FLOAT_USE==TRUE)
	float	x1, x2, x3, x4, u1;

	/*----------------------------------------------------------------------------------------------*/
	/*		�ʑ��⏞���x�I�u�U�[�o���Z 																*/
	/*----------------------------------------------------------------------------------------------*/
	u1 = EhVobs->var.EhSpdObs + EhVobs->var.Wkbf2;
	EhVobs->var.Wkbf1 = FlibLpfilter1( u1, EhVobs->conf.Lpf, EhVobs->var.Wkbf1 );

	u1 = MotSpd - EhVobs->var.Wkbf1;

	x1 = u1 * EhVobs->conf.Tf;
	x2 = x1 * EhVobs->conf.Kstf1;
	EhVobs->var.Wkbf2 = x2 - u1;

	x2 = x1 * EhVobs->conf.Kstf2;
	x3 = FlibIntegral( x1, EhVobs->conf.Kstf3, EhVobs->var.Ivar64 );
	x4 = TrqRef * EhVobs->conf.Kj;

	EhVobs->var.EhSpdObs = FlibLimitul( (EhVobs->var.EhSpdObs + x2 + x3 + x4), 0x1000000, -0x1000000 );

	return;
#else
	LONG	x1, x2, x3, x4, u1;

	/*----------------------------------------------------------------------------------------------*/
	/*		�ʑ��⏞���x�I�u�U�[�o���Z 																*/
	/*----------------------------------------------------------------------------------------------*/
	u1 = EhVobs->var.EhSpdObs + EhVobs->var.Wkbf2;
	EhVobs->var.Wkbf1 = MlibLpfilter1( u1, EhVobs->conf.Lpf, EhVobs->var.Wkbf1 );

	u1 = MotSpd - EhVobs->var.Wkbf1;

	x1 = MlibMulgain32( u1 , EhVobs->conf.Tf );
	x2 = MlibMulgainNolim( x1 , EhVobs->conf.Kstf1 );
	EhVobs->var.Wkbf2 = x2 - u1;

	x2 = MlibMulgainNolim( x1 , EhVobs->conf.Kstf2 );
	x3 = MlibIntegral( x1, EhVobs->conf.Kstf3, EhVobs->var.Ivar64 );
	x4 = MlibMulgain( TrqRef, EhVobs->conf.Kj );

	EhVobs->var.EhSpdObs = MlibLimitul( (EhVobs->var.EhSpdObs + x2 + x3 + x4), 0x1000000, -0x1000000 );

	return;
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		�O���I�u�U�[�o�ϐ�������																	*/
/*																									*/
/****************************************************************************************************/
//<S09B> void	DisturbObserverInitialize( DOBS_CTRL *DobsCtrl )
void	DisturbObserverInitialize( DOBS_CTRL *DobsCtrl, GSELDOBS *pActGains )
{
	/* �O���I�u�U�[�o�ϐ������� */
	MlibResetLongMemory( &(DobsCtrl->var), sizeof(DobsCtrl->var)>>2 );
	/* �p�����[�^���������͑�1�Q�C����ݒ� */
//<S09B>	DobsCtrl->DobsPrm[0] = DobsCtrl->DobsPrm[1];
	DobsCtrl->pDobsPrm = pActGains;			/* <S09B> */
	/* �O���I�u�U�[�o�L���E�����ݒ� */
///*<S00A>*/	DobsCtrl->var.DobsAct = DobsCtrl->var.DobsAct;
	DobsCtrl->var.DobsAct = 0;			/*@@TODO�������������� <S00A>*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		���̃��f���Q���O���I�u�U�[�o���Z															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v :	�Q���O���I�u�U�[�o���\�����C�O���g���N���Z�o����										*/
/*			�I�u�U�[�o���Z�͋@�\OFF�������s����B(�A�i���O���j�^���L��)								*/
/*			�@�\OFF->ON�ő��x�΍��𒲐����ؑ֎��̃V���b�N��ጸ����									*/
/*																									*/
/*	arg		: LONG	MotSpd				:	���x�t�B�[�h�o�b�N	[2^24/OvrSpd]						*/
/*			  LONG	TrqRef				:	�g���N�w��			[2^24/MaxTrq]						*/
/*	out		: 																						*/
/*	g-out	: LONG	AoutV.TrqFBCx 		:	�g���N�e�a�⏞		[2^24/MaxTrq]						*/
/*			  LONG	BoutV.AmonDisTrq	:	�O���g���N			[2^24/MaxTrq]						*/
/*			  LONG	BoutV.AmonDisTrqCmp :	�⏞�g���N			[2^24/MaxTrq]						*/
/*			  LONG	BoutV.AmonDobSpd	:	�I�u�U�[�o���葬�x	[2^24/OvrSpd]						*/
/*																									*/
/****************************************************************************************************/
void	DisturbObserver( DOBS_CTRL *DobsCtrl, KSGAIN MotSpd, KSGAIN TrqRef )				/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	u1, u2;
	float	x1, x2;

	u1 = MotSpd - DobsCtrl->var.SpdObs;

	u2 = FlibIntegral( u1, DobsCtrl->DobsPrm[0].Ks, DobsCtrl->var.Ivar64 );
	u1 = u2 + (u1 * 2);

	x1 = u1 * DobsCtrl->DobsPrm[0].Ks;
	x2 = TrqRef * DobsCtrl->DobsPrm[0].Kj;

	DobsCtrl->var.SpdObs = FlibLimitul( (DobsCtrl->var.SpdObs + x1 + x2), 0x1000000, -0x1000000 );

	DobsCtrl->var.DisTrq = u2 * DobsCtrl->DobsPrm[0].Ksj;

	u2 = DobsCtrl->var.DisTrq;			/*<V043>*/

	DobsCtrl->var.DTrqLpfo = FlibLpfilter1( u2, DobsCtrl->DobsPrm[0].Klpf, DobsCtrl->var.DTrqLpfo );

	DobsCtrl->var.DisTrqCmp = DobsCtrl->var.DTrqLpfo * DobsCtrl->DobsPrm[0].Kd;

	if( DobsCtrl->var.DobsAct == TRUE )
	{ /* �⏞�L���� */
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LpxDobsLatchForAnalogfb(DobsCtrl->var.DTrqLpfo);
#endif
		DobsCtrl->var.CurrentStatus = TRUE;								/* �O���I�u�U�[�o�L�����				*/
	}
	else
	{ /* �⏞������ */
		DobsCtrl->var.CurrentStatus = FALSE;								/* �O���I�u�U�[�o�������				*/
	}

#else	
	LONG	u1, u2;
	LONG	x1, x2;
	GSELDOBS *pDobsPrm = DobsCtrl->pDobsPrm;						/* <S09B> */

	u1 = MotSpd - DobsCtrl->var.SpdObs;

	u2 = MlibIntegral( u1, pDobsPrm->Ks, DobsCtrl->var.Ivar64 );	/* <S09B> */
	u1 = u2 + (u1 << 1);

	x1 = MlibMulgainNolim( u1, pDobsPrm->Ks );						/* <S09B> */
	x2 = MlibMulgain( TrqRef, pDobsPrm->Kj );						/* <S09B> */

	DobsCtrl->var.SpdObs = MlibLimitul( (DobsCtrl->var.SpdObs + x1 + x2), 0x1000000, -0x1000000 );

	DobsCtrl->var.DisTrq = MlibMulgain( u2, pDobsPrm->Ksj );		/* <S09B> */

	u2 = DobsCtrl->var.DisTrq;			/*<V043>*/

	DobsCtrl->var.DTrqLpfo = MlibLpfilter1( u2, pDobsPrm->Klpf, DobsCtrl->var.DTrqLpfo );	/* <S09B> */

	DobsCtrl->var.DisTrqCmp = MlibMulgainNolim( DobsCtrl->var.DTrqLpfo, pDobsPrm->Kd );	/* <S09B> */

	if( DobsCtrl->var.DobsAct == TRUE )
	{ /* �⏞�L���� */
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LpxDobsLatchForAnalogfb(DobsCtrl->var.DTrqLpfo);
#endif
		DobsCtrl->var.CurrentStatus = TRUE;								/* �O���I�u�U�[�o�L�����				*/
	}
	else
	{ /* �⏞������ */
		DobsCtrl->var.CurrentStatus = FALSE;								/* �O���I�u�U�[�o�������				*/
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*	���x�΍��ϕ������� & �g���N�w�߃t�B���^�ώZ�l����������											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	�O���I�u�U�[�o�@�\�����̗L���ؑւ����Ƀg���N��A���I�ɐؑւ��邽�ߐώZ�l�̏��������s��			*/
/*																									*/
/*	�@���x�΍��ϕ��̏�����																			*/
/*		I�����g���N���O���⏞�g���N���������������o�͂���悤�ɐώZ�l��������						*/
/*		�g���N�⏞ OFF��ON���F																		*/
/*			�u���x�ϕ����䂩��̃g���N�{�g���N�⏞���O�񑬓x����g���N�v�ƂȂ�K�v�����邩��		*/
/*			���x�ϕ��g���N�����x�ϕ��g���N�|�g���N�⏞�ŏ���������B�g���N�⏞�̓}�C�i�X������		*/
/*			���x����g���N�Ɖ��Z�����																*/
/*																									*/
/*		�g���N�⏞ ON��OFF���F																		*/
/*			�u���x�ϕ����䂩��̃g���N���O��g���N�⏞�{�O�񑬓x����g���N�v�ƂȂ�K�v�����邩��	*/
/*			���x�ϕ��g���N�����x�ϕ��g���N�{�g���N�⏞�ŏ���������B								*/
/*																									*/
/*		���ӁjIntegral()�֐����̎l�̌ܓ������̂��߁C���������ɂ́~�Q���K�v							*/
/*																									*/
/*	�A�g���N�w�߃t�B���^�ϕ��l�̏�����																*/
/*		�g���N�w�߃��[�p�X�t�B���^�̐ώZ�l���g���N�⏞�����Z����O�̑��x���䃋�[�v�ɂ��g���N��	*/
/*		����������B																				*/
/*																									*/
/*	����L�̏����͐��䃋�[�v�ϐ���ύX���邽�߁A�{���W���[���̊O���Ŏ��{����B						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( (DobsCtrl->var.CurrentStatus == TRUE)				/* ����O���I�u�U�[�o�L��				*/
		&& (DobsCtrl->var.LastStatus == FALSE))				/* �O��O���I�u�U�[�o����				*/
	{ /* �O���I�u�U�[�o�����˗L�� */
		DobsCtrl->var.ExchangeSts = DOBSCHNG_INV2ACT;
	}
	else if((DobsCtrl->var.CurrentStatus == FALSE)			/* ����O���I�u�U�[�o����				*/
		&& (DobsCtrl->var.LastStatus == TRUE))				/* �O��O���I�u�U�[�o�L��				*/
	{ /* �O���I�u�U�[�o�L���˖��� */
		DobsCtrl->var.ExchangeSts = DOBSCHNG_ACT2INV;
	}
	else
	{
		DobsCtrl->var.ExchangeSts = DOBS_NOCHANGE;
	}

	/* �O��O���I�u�U�[�o�@�\��� */
	DobsCtrl->var.LastStatus = DobsCtrl->var.CurrentStatus;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�ʑ��⏞���x�I�u�U�[�o�ϐ�������															*/
/*																									*/
/****************************************************************************************************/
void	EhSpeedObserverInitialize( EHVOBS_CTRL *EhVobsCtrl )
{
	MlibResetLongMemory( &(EhVobsCtrl->var), sizeof(EhVobsCtrl->var)>>2 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		�ʑ��⏞���x�I�u�U�[�o���Z																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�T�v :	�ʑ��⏞���x�I�u�U�[�o�̐��葬�x�v�Z���s���B											*/
/*			�g���N�t�B���^���萔��10��s]�ȉ��Ƃ���Ɣ��U�C���ƂȂ邽�߉����l��20[��s]�Ƃ���B		*/
/*			�������A���̉����l�͑��x�I�u�U�[�o��KsTf�Z�o�݂̂ɓK�p�����B							*/
/*			���������āA�g���N�t�B���^���̓p�����[�^�ݒ�l�ʂ�ɂȂ�B								*/
/*																									*/
/*	arg			: MotSpd				�������x			[2^24/nos]								*/
/*				  TrqRef				�g���N�w��			[2^24/Tqmax]							*/
/*	out			: EhVobs.V.EhSpdObs		���葬�x			[2^24/nos]								*/
/* 	g-out		: 																					*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	EhSpeedObserver( EHVOBS_CTRL *EhVobsCtrl, KSGAIN MotSpd, KSGAIN TrqRef )				/*<S00A>*/
{
#if (FLOAT_USE==TRUE)
	float	x1, x2, x3, x4, u1;

	/*----------------------------------------------------------------------------------------------*/
	/*	�I�u�U�[�o�ւ̃g���N�w�ߓ��͂Ƀ��[�p�X�t�B���^������										*/
	/*----------------------------------------------------------------------------------------------*/
	EhVobsCtrl->var.TrqRefLpfo = FlibLpfilter1( TrqRef,
												EhVobsCtrl->EhVobsPrm.KTrqReflpf,
												EhVobsCtrl->var.TrqRefLpfo );

	/*----------------------------------------------------------------------------------------------*/
	/*		�ʑ��⏞���x�I�u�U�[�o���Z 																*/
	/*----------------------------------------------------------------------------------------------*/
	u1 = EhVobsCtrl->var.EhSpdObs + EhVobsCtrl->var.Wkbf2;

	EhVobsCtrl->var.Wkbf1 = FlibLpfilter1( u1, EhVobsCtrl->EhVobsPrm.Lpf, EhVobsCtrl->var.Wkbf1 );

	u1 = MotSpd - EhVobsCtrl->var.Wkbf1;

	x1 = u1 * EhVobsCtrl->EhVobsPrm.Tf;

	x2 = x1 * EhVobsCtrl->EhVobsPrm.Kstf1;

	EhVobsCtrl->var.Wkbf2 = x2 - u1;

	x2 = x1 * EhVobsCtrl->EhVobsPrm.Kstf2 ;

	x3 = FlibIntegral( x1, EhVobsCtrl->EhVobsPrm.Kstf3, EhVobsCtrl->var.Ivar64 );

	x4 = EhVobsCtrl->var.TrqRefLpfo * EhVobsCtrl->EhVobsPrm.Kj;

	EhVobsCtrl->var.AccSum = EhVobsCtrl->var.AccSum + (x2 + x3 + x4);
	EhVobsCtrl->var.EhSpdObs = FlibLimitul( EhVobsCtrl->var.AccSum, 0x1000000, -0x1000000 );//���ڐ����ɒu������

#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���j�^�p�ϐ��o��																		*/
	/*----------------------------------------------------------------------------------------------*/
	BoutV.AmonSpdObs = NORMALIZEINT(EhVobsCtrl->var.EhSpdObs);
#endif

	return( EhVobsCtrl->var.EhSpdObs );

#else
	
	LONG	x1, x2, x3, x4, u1;

	/*----------------------------------------------------------------------------------------------*/
	/*	�I�u�U�[�o�ւ̃g���N�w�ߓ��͂Ƀ��[�p�X�t�B���^������										*/
	/*----------------------------------------------------------------------------------------------*/
	EhVobsCtrl->var.TrqRefLpfo = MlibLpfilter1( TrqRef,
												EhVobsCtrl->EhVobsPrm.KTrqReflpf,
												EhVobsCtrl->var.TrqRefLpfo );

	/*----------------------------------------------------------------------------------------------*/
	/*		�ʑ��⏞���x�I�u�U�[�o���Z 																*/
	/*----------------------------------------------------------------------------------------------*/
	u1 = EhVobsCtrl->var.EhSpdObs + EhVobsCtrl->var.Wkbf2;

	EhVobsCtrl->var.Wkbf1 = MlibLpfilter1( u1, EhVobsCtrl->EhVobsPrm.Lpf, EhVobsCtrl->var.Wkbf1 );

	u1 = MotSpd - EhVobsCtrl->var.Wkbf1;

	x1 = MlibMulgain32( u1 , EhVobsCtrl->EhVobsPrm.Tf);

	x2 = MlibMulgainNolim( x1 , EhVobsCtrl->EhVobsPrm.Kstf1 );

	EhVobsCtrl->var.Wkbf2 = x2 - u1;

	x2 = MlibMulgainNolim( x1 , EhVobsCtrl->EhVobsPrm.Kstf2 );

	x3 = MlibIntegral( x1, EhVobsCtrl->EhVobsPrm.Kstf3, EhVobsCtrl->var.Ivar64 );

	x4 = MlibMulgain( EhVobsCtrl->var.TrqRefLpfo, EhVobsCtrl->EhVobsPrm.Kj );

	EhVobsCtrl->var.AccSum = EhVobsCtrl->var.AccSum + (x2 + x3 + x4);
	EhVobsCtrl->var.EhSpdObs = MlibLimitul( EhVobsCtrl->var.AccSum, 0x1000000, -0x1000000 );

#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		���j�^�p�ϐ��o��																		*/
	/*----------------------------------------------------------------------------------------------*/
	BoutV.AmonSpdObs = EhVobsCtrl->var.EhSpdObs;
#endif

	return( EhVobsCtrl->var.EhSpdObs );

#endif /* FLOAT_USE */
}

/*START <S0CD> */
void InitParamFriction( BASE_LOOPCTRLS *BaseLoops  )
{
	BaseLoops->BaseCtrls->FrictionM.TrqGrvLast = BaseLoops->BaseCtrls->FrictionM.GrvTrq;
	BaseLoops->BaseCtrls->FrictionM.TrqCLast = 0;
	BaseLoops->BaseCtrls->FrictionM.InitEnable = FALSE;

}
/*END <S0CD>*/

/*START <S0CD>*/
//#if( FRICTRQ_COMP != 0 )
LONG	LpxMotorFricComp(  BOOL BaseEnable, BASE_LOOPCTRLS *BaseLoops )
{
	LONG Trqfric;
	LONG TrqGravity;
	LONG TrqCoulomb;
	LONG TrqViscous;
	LONG sx;
	LONG kx;
	LONG ky;
	LONG kz;

	/* ���������ϐ��C��<S191> */
	TrqGravity = 0;
	TrqCoulomb = 0;
	TrqViscous = 0;

	if(BaseEnable == FALSE)
		return 0;

	if((BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo > -BaseLoops->BaseCtrls->FrictionM.SpdHys) && (BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo < BaseLoops->BaseCtrls->FrictionM.SpdHys ))
	{
		TrqGravity = BaseLoops->BaseCtrls->FrictionM.TrqGrvLast;
		TrqCoulomb = BaseLoops->BaseCtrls->FrictionM.TrqCLast;
  	    TrqViscous = 0;

	}
	if(BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo >= BaseLoops->BaseCtrls->FrictionM.SpdHys)
	{
		sx = 0;

		TrqGravity = BaseLoops->BaseCtrls->FrictionM.GrvTrq;
		BaseLoops->BaseCtrls->FrictionM.TrqGrvLast = BaseLoops->BaseCtrls->FrictionM.GrvTrq;
		TrqCoulomb = BaseLoops->BaseCtrls->FrictionM.PosCTrq;
		BaseLoops->BaseCtrls->FrictionM.TrqCLast = BaseLoops->BaseCtrls->FrictionM.PosCTrq;

		ky = MlibPcalKxkskx(600000000,BaseLoops->Bprm->RatSpd,C2PAIE7,&sx,-24);
		kz = BprmSpeedLevelCal(BaseLoops->Bprm, ky*10,0);
		kx = MlibPcalKxgain(BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo,1,kz,&sx, 0);
		TrqViscous = MlibPcalKxgain(kx, BaseLoops->BaseCtrls->FrictionM.VisTrq, 1, &sx, -24);
	}
	if(BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo <= -BaseLoops->BaseCtrls->FrictionM.SpdHys)
	{
		sx = 0;
		TrqGravity = 0;
		BaseLoops->BaseCtrls->FrictionM.TrqGrvLast = 0;
		TrqCoulomb = BaseLoops->BaseCtrls->FrictionM.NegCTrq;
		BaseLoops->BaseCtrls->FrictionM.TrqCLast = BaseLoops->BaseCtrls->FrictionM.NegCTrq;

		ky = MlibPcalKxkskx(600000000,BaseLoops->Bprm->RatSpd,C2PAIE7,&sx,-24);
		kz = BprmSpeedLevelCal(BaseLoops->Bprm, ky*10,0);
		kx = MlibPcalKxgain(BaseLoops->BaseCtrls->CtrlCmdMngr.SpdRefo,1,kz,&sx, 0);
		TrqViscous = MlibPcalKxgain(kx, BaseLoops->BaseCtrls->FrictionM.VisTrq, 1, &sx, -24);
	}

	Trqfric = TrqGravity + TrqViscous + TrqCoulomb;
	return Trqfric;
}

//#endif

/****************************************************************************************************/
/*																									*/
/*		<S1A5> �Đ݌v�ŊO���I�u�U�[�o																*/
/*		�������Xstep3																				*/
/*																									*/
/*	�T�v:	�g���N�w�߂���э������x�Ɋ�Â��ϓ��g���N�𐄒肵�A									*/
/*			�g���N�w�߂ɑ����ĐV���ȃg���N�w�߂Ƃ���B												*/
/*				Tr1 = Tr0 + Kf * LPF{Tr0 - LPF(J*d��m/dt)}											*/
/*																									*/
/* arg		: LONG	MotSpd				:	���x�t�B�[�h�o�b�N	[2^24/OvrSpd]						*/
/*			  LONG	TrqRef				:	�g���N�w��			[2^24/MaxTrq]						*/
/* out		: 																						*/
/* g-out																							*/
/*																									*/
/****************************************************************************************************/
void	tuneLessRobustCompensatorEx( void *pprm, LONG MotSpd, LONG TrqRef )
{
	LONG	lwk1, lwk2;
//	LONG	sx1;
	TUNELESS_DRCOMP *pdrcmp = (TUNELESS_DRCOMP *)pprm;
	TL2PRM *pTl2 = pdrcmp->conf.pTl2;

	/*----------------------------------------------------------------------------------------------*/
	/* �g���N�w�ߑ��p�X�̉��Z 																		*/
	/*----------------------------------------------------------------------------------------------*/
	lwk1 = tuneLessLowPassfilter2Ex( TrqRef, pTl2->KrlpfCur, pdrcmp->var.Krlpftmp1 );

	lwk1 = tuneLessLowPassfilter2Ex( lwk1, pTl2->KrlpfDisObs1, pdrcmp->var.Krlpftmp2 );


	/*----------------------------------------------------------------------------------------------*/
	/* �t�B�[�h�o�b�N���x���p�X�̉��Z 																*/
	/*----------------------------------------------------------------------------------------------*/
	pdrcmp->var.VelLpfo1 = MlibLpfilter1( MotSpd, pTl2->KrlpfResSup, pdrcmp->var.VelLpfo1 );

	lwk2 = MotSpd - pdrcmp->var.VelLpfo1;

	lwk2 = lwk2 >> 2;

	lwk2 = pdrcmp->var.VelLpfo1 + lwk2;

	lwk2 = tuneLessLowPassfilter2Ex( lwk2, pTl2->KrlpfDisObs2, pdrcmp->var.Krlpftmp3 );

	lwk2 = MlibMulgain29( lwk2, pdrcmp->conf.Krsj );


	/*----------------------------------------------------------------------------------------------*/
	/* ����O���g���N���Z 																			*/
	/*----------------------------------------------------------------------------------------------*/
	pdrcmp->var.DisTrq = MlibLimitul( (lwk2 - lwk1), 0x1000000, -0x1000000 );

	lwk1 = MlibMulgainNolim( pdrcmp->var.DisTrq, pdrcmp->conf.Krd );

	pdrcmp->var.DisTrqCmp = MlibLpfilter1( lwk1, pdrcmp->conf.KCmpOutlpf, pdrcmp->var.DisTrqCmp );
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		<S1A5> �Q���t�B���^(�o�ꎟ�ϊ�)																*/
/*		�������Xstep3�p																				*/
/*																									*/
/****************************************************************************************************/
static LONG tuneLessLowPassfilter2Ex( LONG u, LONG k[5], LONG z[4] )
{
LONG	x[6];

		x[0] = MlibMulgain29( u,    k[0] );
		x[1] = MlibMulgain29( z[0], k[1] );
		x[2] = MlibMulgain29( z[1], k[2] );
		x[3] = MlibMulgain29( z[2], k[3] );
		x[4] = MlibMulgain29( z[3], k[4] );
		x[5] = x[0] + x[1] + x[2] + x[3] - x[4];
/*--------------------------------------------------------------------------------------------------*/
		z[3] = z[2];
		z[2] = x[5];
		z[1] = z[0];
		z[0] = u;

		return( x[5] );

}


/*END <S0CD>*/

/****************************************************************************************************/
/*																									*/
/*		<S1A5> ���o�X�g�⏞��I��																	*/
/*		�������Xstep3�p																				*/
/*																									*/
/****************************************************************************************************/
void	TuneLessInitRobustCompensator( TUNELESS_CTRL *TuneLessCtrl, TUNELESS_DRCOMP	*Drcomp )
{
	Drcomp->conf.pTl2 = &(TuneLessCtrl->TuneLessPrm.Tl2Prm);
	if ( TuneLessCtrl->conf.TuningLessEx == FALSE )
	{
		Drcomp->conf.RobustCompensator = tuneLessRobustCompenSater;
	}
	else
	{
		Drcomp->conf.RobustCompensator = tuneLessRobustCompensatorEx;
	}
}

/***************************************** end of file **********************************************/
