/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ResVibCtrl.c : �U�����o�E���U����p	���W���[����`											*/
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
/*	Note	:	����	2010.11.11	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MLib.h"
#include "ResVibCtrl.h"
#include "BaseControlPrmCalc.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	ResVibATypeResVibInitialize( RESVIB *ResVib );											*/
/*	LONG	ResVibATypeResVibCtrl( RESVIB *ResVib, LONG MotSpd, LONG TrqRef );						*/
/*--------------------------------------------------------------------------------------------------*/
/*	void	DetVibObsInitialize( DETVIB *DetVib );													*/
/*	void	DetVibSpeedObserver( DETVIB *DetVib, LONG SpdFbki, LONG TrqRefMon );					*/
/*	void	DetVibSetSpeedError( DETVIB *DetVib, BOOL TuningLess, BOOL TuningRun );					*/
/*	LONG	DetVibGetSpdErrPeak( DETVIB *DetVib );													*/
/*	LONG	DetVibGetVibCheckLevel( DETVIB *DetVib );												*/
/*	void	DetVibResetMaxMinSpeedError( DETVIB *DetVib );											*/
/*	void	DetVibResetMaxMinSpeedErrorFn( DETVIB *DetVib );										*/
/*	LONG	DetVibGetSpdErrPeakFn( DETVIB *DetVib );												*/
/*--------------------------------------------------------------------------------------------------*/
/*	void	DetVibInitialize( DETVIB *DetVib );														*/
/*	LONG	DetVibDetectVibration( DETVIB *DetVib );												*/
/*	void	DetVibResetRequest( DETVIB *DetVib );													*/
/*	LONG	DetVibDetectLowFreqVibration( DETVIB *DetVib );											*/
/*	void	DetVibLowFreqResetRequest( DETVIB *DetVib );											*/
/*--------------------------------------------------------------------------------------------------*/
/*	void	DetVibTraceInitialize( DETVIB *DetVib );												*/
/*	void	DetVibTraceRuntimeService( DETVIB *DetVib );											*/
/*	UCHAR	DetVibTraceGetStatus( DETVIB *DetVib );													*/
/*	SHORT*	DetVibTraceGetDataAdr( DETVIB *DetVib );												*/
/*	USHORT	DetVibTraceGetIndex( DETVIB *DetVib );													*/
/*	void	DetVibTraceStartReq( DETVIB *DetVib );													*/
/*	void	DetVibTraceStopReq( DETVIB *DetVib );													*/
/*	void	DetVibTraceSetCondition( DETVIB *DetVib, USHORT TraceNum,								*/
/*															UCHAR TraceInt, SHORT TrigerLvl );		*/
/****************************************************************************************************/
extern void __EI( void );									/* ���荞�݋��ݒ�						*/
extern void __DI( void );									/* ���荞�݋֎~�ݒ�						*/



#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		���U����ϐ�������																			*/
/*																									*/
/****************************************************************************************************/
void	ResVibATypeResVibInitialize( RESVIB *ResVib )
{
	MlibResetLongMemory( &ResVib->var, sizeof(ResVib->var)/4 );
	return;
}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */


/* 2014/04/01MD�́A���LA�^���U������ϲ�ۉ��Z���Ɉړ��@*/
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		A�^���U���䉉�Z																				*/
/*																									*/
/*	�T�v:	1�����x�I�u�U�[�o���\�����C���葬�x��FB���x���琧�U�M�����Z�o����						*/
/*			���U�M���Ƀ_���s���O�Q�C�����悶�đ��xFB��␳����										*/
/*			SGDX�ł͋@�\OFF�����I�u�U�[�o�����s����B(�A�i���O���j�^���L��)							*/
/*																									*/
/* arg		: LONG	MotSpd				:	���x�t�B�[�h�o�b�N	[2^24/OvrSpd]						*/
/*			: LONG	TrqRef				:	�g���N�w��			[2^24/MaxTrq]						*/
/* out		:																						*/
/* g-out	: LONG	SpdFBCx				:	���xFB�⏞			[2^24/OvrSpd]						*/
/*																									*/
/****************************************************************************************************/
LONG	ResVibATypeResVibCtrl( RESVIB *ResVib, LONG MotSpd, LONG TrqRef )
{
	LONG	u1, u2, u3;//<S03E>
	LONG	x1, x2;

	u1 = MotSpd - ResVib->var.SpdObs;
#if (FLOAT_USE==TRUE)
	x1 = u1 * ResVib->ResVibPrm.Ks;
	x2 = TrqRef * ResVib->ResVibPrm.Kj;

	ResVib->var.SpdObs = MlibLimitul( (ResVib->var.SpdObs + x1 + x2), 0x1000000, -0x1000000 );

	u2 = MlibHpfilter1( u1, ResVib->ResVibPrm.Khpf, &ResVib->var.Hpftmp );
	ResVib->var.dSpdLpfo = MlibLpfilter1( u2, ResVib->ResVibPrm.Klpf, ResVib->var.dSpdLpfo );

	ResVib->var.dSpdCmp = ResVib->var.dSpdLpfo * ResVib->ResVibPrm.Knd;
#else
	x1 = MlibMulgainNolim( u1, ResVib->ResVibPrm.Ks );
	x2 = MlibMulgain( TrqRef, ResVib->ResVibPrm.Kj );

	ResVib->var.SpdObs = MlibLimitul( (ResVib->var.SpdObs + x1 + x2), 0x1000000, -0x1000000 );

	u2 = MlibHpfilter1( u1, ResVib->ResVibPrm.Khpf, &ResVib->var.Hpftmp );
	ResVib->var.dSpdLpfo = MlibLpfilter1( u2, ResVib->ResVibPrm.Klpf, ResVib->var.dSpdLpfo );

	u3 = MlibHpfilter1( ResVib->var.dSpdLpfo, ResVib->ResVibPrm.Khpf1, &ResVib->var.Hpftmp1 );//<S03E>

	//ResVib->var.dSpdCmp = MlibMulgain( ResVib->var.dSpdLpfo, ResVib->ResVibPrm.Knd );//<S03E>
	ResVib->var.dSpdCmp = MlibMulgain( ResVib->var.dSpdLpfo, ResVib->ResVibPrm.Knd ) + MlibMulgain( u3, ResVib->ResVibPrm.Knd1 );//<S03E>
#endif /* FLOAT_USE */

	/* ���j�^�p�ϐ��o�� */
	ResVib->var.MonResVib  = ResVib->var.dSpdLpfo;

	return	ResVib->var.dSpdCmp;
}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */


#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		���U����I���X�C�b�`(Pn10B.3)																*/
/*																									*/
/****************************************************************************************************/
BOOL	ResVibCalculatePrmSW( RESVIB *ResVib, ULONG avibsw, BOOL TuningLess, ASICS *SvAsicRegs )
{
	LONG	wk;
	BOOL	PrmErr;
	PrmErr = FALSE;

	if( TuningLess == FALSE )
	{
		wk = avibsw & 0x0F;
		switch( wk )
		{
		case 0x00: /* ���U���䖳�� */
			/* �b�菈�� ASIC��MREG_RVCTRL�̊��蓖�Ă��Ȃ��̂ŁA�ۗ� */
			ResVib->AresVibSetting = FALSE;
//			SvAsicRegs->AsicMicroReg->MREG_RVCTRL = FALSE;
			break;
		case 0x01: /* �`�^���U����L�� */
			/* �b�菈�� ASIC��MREG_RVCTRL�̊��蓖�Ă��Ȃ��̂ŁA�ۗ� */
			ResVib->AresVibSetting = TRUE;
//			SvAsicRegs->AsicMicroReg->MREG_RVCTRL = TRUE;
			break;
		default :
			PrmErr = TRUE;
			break;
		}
	}
	else
	{
		/* �������X�L������A�^���U���� */
		/* �b�菈�� ASIC��MREG_RVCTRL�̊��蓖�Ă��Ȃ��̂ŁA�ۗ� */
		ResVib->AresVibSetting = FALSE;
//		SvAsicRegs->AsicMicroReg->MREG_RVCTRL = FALSE;
	}

	return	PrmErr;
}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */



#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		���U����p�����[�^�v�Z																		*/
/*																									*/
/****************************************************************************************************/
void	ResVibCalculatePrm( RESVIB *ResVib, RESVIB_CFG_PRM *Prm, KSGAIN Kvx, LONG ScanTimeNs, ASICS *SvAsicRegs )
{
	RESVIB_PRM	wrkp;
	LONG		kx, sx;
	LONG		wk, wk2;

/*--------------------------------------------------------------------------------------------------*/
/*		���U�I�u�U�[�o�Q�C��(���U���g��[0.1Hz]) : ResVib.P.Ks										*/
/*																									*/
/*			= ResFreq[0.1Hz]/2																		*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Ks = BpxNorObsGainCal( (Prm->avibfrq >> 1) ,ScanTimeNs);

/*--------------------------------------------------------------------------------------------------*/
/*		���U�I�u�U�[�o�C�i�[�V���Q�C��([%]) : ResVib.P.Kj											*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Kj = BpxNorObsJGainCal( Kvx, Prm->jrate, Prm->avibgn , ScanTimeNs );

/*--------------------------------------------------------------------------------------------------*/
/*		���U���x�_���s���O�Q�C��([%]) : ResVib.P.Knd												*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Knd = MlibScalKxgain( Prm->avibdamp, 1, 100, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		���U���[�p�X�t�B���^�Q�C��(���U���g��[0.1Hz],���萔�␳[0.01ms]) : ResVib.P.Klpf			*/
/*																									*/
/*									         2 * 1000000											*/
/*			�t�B���^���萔[0.01ms] = ----------------------------- + adj[0.01ms]					*/
/*			 					  	       2*PAI * ResFreq[0.1Hz]									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 20000000,1, Prm->avibfrq, &sx, 0 );
	wk = MlibPcalKxgain( kx, C10POW7, C2PAIE7, &sx, 24 );

	/* �t�B���^���萔�����p���v�Z���� */
	wk2 = MlibKsgain2Long( wk );

	/* �������ʉ��Z */
	wk = ((LONG)Prm->avibfil1 * 10) + wk2;
	if( wk < 0 )
	{ /* �������~�b�g */
		wk = 0;
	}
//	wrkp.Klpf = MlibPcalKf1gain( wk, (ScanTimeNs/1000), 0 );
	wrkp.Klpf = MlibPcalKf1gain( wk, ((ScanTimeNs+500)/1000), 0 );

/*--------------------------------------------------------------------------------------------------*/
/*		���U�n�C�p�X�t�B���^�Q�C��(���U���g��[0.1Hz],���萔�␳[0.01ms]) : ResVib.P.Khpf			*/
/*																									*/
/*									         2 * 100000												*/
/*			�t�B���^���萔[0.01ms] = ----------------------------- + adj[0.01ms]					*/
/*			 					  	       2*PAI * ResFreq[Hz]										*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	wk = (LONG)((SHORT)Prm->avibfil2 * 10) + wk2;
	if( wk < 0 )
	{ /* �������~�b�g */
		wk = 0;
	}
//	wrkp.Khpf = MlibPcalKf1gain( wk, (ScanTimeNs/1000), 0 );
	wrkp.Khpf = MlibPcalKf1gain( wk, ((ScanTimeNs+500)/1000), 0 );
	wrkp.Khpf1 = MlibPcalKf1gain( (LONG)Prm->avibfrq2, ((ScanTimeNs+500)/1000), 1 );//<S03E>
	wrkp.Knd1 = MlibScalKxgain( Prm->avibdamp2, 1, 100, NULL, 24 );//<S03E>

/*--------------------------------------------------------------------------------------------------*/
/*		Set Parameters																				*/
/*--------------------------------------------------------------------------------------------------*/
	KPI_DI();		/* <S03E> Disable Interrupt	*/
	ResVib->ResVibPrm = wrkp;//<S03E>
	KPI_EI();		/*<S03E>  Enable Interrupt		*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		�}�C�N���h�e���W�X�^�ւ̏�������															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/* �b�菈���@AISC��MREG_RVKS�̊��蓖�Ă��Ȃ����߁A�ۗ� */
	MpReg = &(SvAsicRegs->AsicMicroReg->MREG_RVKS);
	tbuf[0] = (USHORT)(wrkp.Ks);
	tbuf[1] = (USHORT)(wrkp.Ks >> 16);
	tbuf[2] = (USHORT)(wrkp.Kj);
	tbuf[3] = (USHORT)(wrkp.Kj >> 16);
	tbuf[4] = (USHORT)(wrkp.Knd);
	tbuf[5] = (USHORT)(wrkp.Knd >> 16);
	tbuf[6] = (USHORT)(wrkp.Klpf);
	tbuf[7] = (USHORT)(wrkp.Klpf >> 16);
	tbuf[8] = (USHORT)(wrkp.Khpf);
	tbuf[9] = (USHORT)(wrkp.Khpf >> 16);
	MicroTranslatePrmReq( SvAsicRegs, tbuf, MpReg, 10 );
#endif
/*--------------------------------------------------------------------------------------------------*/

	return;

}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�U�����o���Z�ϐ�������																		*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsInitialize( DETVIB *DetVib )
{
	MlibResetLongMemory( &(DetVib->DetVibObs.var), sizeof(DetVib->DetVibObs.var)/4 );
	return;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�U�����o���Z																				*/
/*																									*/
/****************************************************************************************************/
void	DetVibSpeedObserver( DETVIB *DetVib, LONG SpdFbki, LONG TrqRefMon )
{
	DETVIBOBS	*DetVibObs;
	LONG		x1, x2, u1;
	LONG		SpdErr;

	DetVibObs = &(DetVib->DetVibObs);

	/* ���ω�����(2Scan) */
	DetVibObs->var.AvSpdFbk = ( SpdFbki + DetVibObs->var.AvSpdFbk ) >> 1;

/*--------------------------------------------------------------------------------------------------*/
/*		���x�I�u�U�[�o���Z																			*/
/*--------------------------------------------------------------------------------------------------*/
	u1 = DetVibObs->var.AvSpdFbk - DetVibObs->var.SpdObs;

	x1 = MlibIntegral( u1, DetVibObs->conf.Ks, DetVibObs->var.Ivar64 );
	u1 = x1 + (u1 << 1);

#if (FLOAT_USE==TRUE)
	x1 = ( (float)u1 * DetVibObs->conf.Ks);
	x2 = TrqRefMon * DetVibObs->conf.Kj;
#else
	x1 = MlibMulgainNolim( u1, DetVibObs->conf.Ks );
	x2 = MlibMulgain( TrqRefMon, DetVibObs->conf.Kj );
#endif /* FLOAT_USE */

	DetVibObs->var.SpdObs = MlibLimitul( (DetVibObs->var.SpdObs + x1 + x2), 0x1000000, -0x1000000 );

/*--------------------------------------------------------------------------------------------------*/
/*		����l�ۑ�																					*/
/*--------------------------------------------------------------------------------------------------*/
	SpdErr = DetVibObs->var.AvSpdFbk - DetVibObs->var.SpdObs;
	DetVibObs->var.AvSpdErr = ( DetVibObs->var.LastSpdErr + SpdErr ) >> 1;
	DetVibObs->var.LastSpdErr = SpdErr;
	DetVibObs->var.AvSpdFbk = SpdFbki;
	DetVibObs->var.TrcObsAvSpdErr = DetVibObs->var.AvSpdErr;
	DetVibObs->var.AvSpdErrbfFil = DetVibObs->var.AvSpdErr;

	return;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		���U���o�p�ő�E�ŏ����x�΍��ݒ菈��														*/
/*																									*/
/****************************************************************************************************/
void	DetVibSetSpeedError( DETVIB *DetVib, BOOL TuningLess, BOOL TuningRun )
{
	DETVIBOBS	*DetVibObs;
	DetVibObs = &(DetVib->DetVibObs);

/*--------------------------------------------------------------------------------------------------*/
/*		�U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g����													*/
/*--------------------------------------------------------------------------------------------------*/
	if( DetVib->DetVibObs.var.SpdErrClrReq == TRUE )
	{
		DetVib->DetVibObs.var.MaxSpdErr = 0;
		DetVib->DetVibObs.var.MinSpdErr = 0;
		DetVib->DetVibObs.var.SpdErrClrReq = FALSE;
	}
	if( DetVib->DetVibObs.var.SpdErrFnClrReq == TRUE )
	{
		DetVib->DetVibObs.var.MaxSpdErrFn = 0;
		DetVib->DetVibObs.var.MinSpdErrFn = 0;
		DetVib->DetVibObs.var.SpdErrFnClrReq = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�U��(���U)�`�F�b�N�p --> ScanC:LpxCheckVibration( ), �`���[�j���O�֌W						*/
/*--------------------------------------------------------------------------------------------------*/
	if( TuningRun == TRUE )
	{
		DetVibObs->var.AvSpdErr = MlibHpfilter1( DetVibObs->var.AvSpdErr,
												 DetVibObs->conf.Khpf,
												 &(DetVibObs->var.Hpftmp) );
	}
	/*------------------------------------------------------------------------------------------*/
	/*	�������X���䎞�U�����o���x������														*/
	/*------------------------------------------------------------------------------------------*/
	if( TuningLess == TRUE )
	{
		DetVibObs->var.AvSpdErr = MlibHpfilter1( DetVibObs->var.AvSpdErr,
												 DetVibObs->conf.Khpf,
												 &(DetVibObs->var.Hpftmp) );
	}
/*--------------------------------------------------------------------------------------------------*/
	if( DetVibObs->var.AvSpdErr > DetVibObs->var.MaxSpdErr )
	{
		DetVibObs->var.MaxSpdErr = DetVibObs->var.AvSpdErr;
	}
	if( DetVibObs->var.AvSpdErr < DetVibObs->var.MinSpdErr )
	{
		DetVibObs->var.MinSpdErr = DetVibObs->var.AvSpdErr;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�����p�����[�^�`���[�j���O�A���[���p														*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObs->var.AvSpdErrLpfOut =
		MlibLpfilter1( DetVibObs->var.AvSpdErrbfFil, DetVibObs->conf.Klpf, DetVibObs->var.AvSpdErrLpfOut );
/*--------------------------------------------------------------------------------------------------*/
	if( DetVibObs->var.AvSpdErrLpfOut > DetVibObs->var.MaxSpdErrFn )
	{
		DetVibObs->var.MaxSpdErrFn = DetVibObs->var.AvSpdErrLpfOut;
	}
	if( DetVibObs->var.AvSpdErrLpfOut < DetVibObs->var.MinSpdErrFn )
	{
		DetVibObs->var.MinSpdErrFn = DetVibObs->var.AvSpdErrLpfOut;
	}
	return;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		���x�΍��� Peak to Peak �擾�pAPI															*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibGetSpdErrPeak( DETVIB *DetVib )
{
	return	(DetVib->DetVibObs.var.MaxSpdErr - DetVib->DetVibObs.var.MinSpdErr);
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�U�����o���x�� �擾�pAPI																	*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibGetVibCheckLevel( DETVIB *DetVib )
{
	return	(DetVib->conf.VibCheckLevel);
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



/****************************************************************************************************/
/*																									*/
/*		�U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g	Called from Round, ScanC							*/
/*																									*/
/****************************************************************************************************/
void	DetVibResetMaxMinSpeedError( DETVIB *DetVib )
{
	DetVib->DetVibObs.var.SpdErrClrReq = TRUE;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		�U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g (for Fn)		Called from Round, ScanC				*/
/*																									*/
/****************************************************************************************************/
void	DetVibResetMaxMinSpeedErrorFn( DETVIB *DetVib )
{
	DetVib->DetVibObs.var.SpdErrFnClrReq = TRUE;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��� Peak to Peak �擾�pAPI (for Fn)													*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibGetSpdErrPeakFn( DETVIB *DetVib )
{
	return	(DetVib->DetVibObs.var.MaxSpdErrFn - DetVib->DetVibObs.var.MinSpdErrFn);
}



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�T�[�{�����ԊĎ��ϐ�����������															*/
/*																									*/
/****************************************************************************************************/
void	DetVibInitialize( DETVIB *DetVib )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�����ԊĎ��p�S�ϐ��[���N���A														*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(DetVib->var), sizeof( DetVib->var )/4  );

	return;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�T�[�{�U��(���U)�`�F�b�N����																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{����̔��U���Ď����A�T�[�{����̖\���𖢑R�ɖh�~����B							*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibDetectVibration( DETVIB *DetVib )
{
	LONG	Vib;
	LONG	VibStatus;
	VibStatus = DETECT_VIB_NOT_DETECT;

	/* Peak to Peak�ŐU����Ԃ����o */
	Vib = DetVib->DetVibObs.var.MaxSpdErr - DetVib->DetVibObs.var.MinSpdErr;

	if( Vib > DetVib->conf.VibCheckLevel )
	{
		if( DetVib->conf.VibCheckSel == 1 )
		{
//			KPI_WARNINGSET( WRN_VIB );					/* A.911 : �U��(���U)���[�j���O				*/
			VibStatus |= DETECT_VIB_WARNING;
		}
		else if( DetVib->conf.VibCheckSel == 2 )
		{
//			KPI_ALARMSET( ALM_VIB );					/* A.520 : �U��(���U)�A���[��				*/
//			KPI_WARNINGCLR( WRN_VIB );					/* A.911 : �U��(���U)���[�j���O				*/
			VibStatus |= DETECT_VIB_ALARM;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		�A�h�o���X�g�I�[�g�`���[�j���O�֌W															*/
/*--------------------------------------------------------------------------------------------------*/
/*			�A�h�o���X�g�I�[�g�`���[�j���O���s���̓��Z�b�g���Ȃ�									*/
/*			�A���[�����x���𒴂�����Ƃ肠�������Z�b�g 												*/
/*--------------------------------------------------------------------------------------------------*/
		/* �A���[�����x���𒴂����烊�Z�b�g */
		DetVibResetMaxMinSpeedError( DetVib );
		/* �U�����o�t���O�Z�b�g */
		DetVib->var.VibDetFlg = TRUE;
	}
	else
	{
//		KPI_WARNINGCLR( WRN_VIB );						/* A.911 : �U��(���U)���[�j���O				*/
		/* �U�����o�t���O�N���A */
		DetVib->var.VibDetFlg = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*			�I�[�g�`���[�j���O�A���[��(A.521)���o����												*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/* A.521����(�U���A���[�����x���~�Q�{)���������ꂽ��A�J�E���g�X�^�[�g����						*/
	/*----------------------------------------------------------------------------------------------*/
	if( Vib > (DetVib->conf.VibCheckLevel << 1) )
	{
		DetVib->var.AatAlmCntFlg = TRUE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* AT�A���[�����o�p�o�ߎ��ԃJ�E���g�C���N�������g												*/
	/*----------------------------------------------------------------------------------------------*/
	if( DetVib->var.AatAlmCntFlg == TRUE )
	{
		DetVib->var.AatAlmCnt++;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* AT�A���[�����o�p�o�ߎ��ԃJ�E���g 50ms�㌟�o����												*/
	/*		A.521��������������Ă�����A���̂܂܏������s											*/
	/*		A.521��������������Ă��Ȃ�������A�J�E���g�N���A										*/
	/*----------------------------------------------------------------------------------------------*/
	if( DetVib->var.AatAlmCnt == KPI_SCANC_MS( 50 ) )
	{
		if( Vib <= (DetVib->conf.VibCheckLevel << 1) )
		{
			DetVib->var.AatAlmCntFlg = FALSE;
			DetVib->var.AatAlmCnt = 0;
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/* AT�A���[�����o�p�o�ߎ��ԃJ�E���g 100ms�㌟�o����												*/
	/*		A.521�����𖞂�����Ă�����A�A���[��													*/
	/*		A.521��������������Ă��Ȃ�������A�J�E���^�N���A										*/
	/*----------------------------------------------------------------------------------------------*/
	else if( DetVib->var.AatAlmCnt == KPI_SCANC_MS( 100 ) )
	{
		DetVib->var.AatAlmCntFlg = FALSE;
		DetVib->var.AatAlmCnt = 0;
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* 2010.11.15 Y.Oka SGDV�ɂ�A.521�o�͂��R�����g�A�E�g����Ă��邽�߁A�폜 */
		if( Vib > (Gprm.VibCheckLevel<<1) )
		{
			if(BoutV.f.InertiaTuning)
			{
				KPI_ALARMSET( ALM_AT );			/* A.521 : AT�A���[��						*/
			}
		}
#endif /* #if 0 */
	}

	return	VibStatus;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�T�[�{�U��(���U)�`�F�b�N���Z�b�g����														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{�I�t���ɐU���`�F�b�N�p�ϐ��̃N���A���s���B										*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	DetVibResetRequest( DETVIB *DetVib )
{
	DetVib->var.VibDetFlg = FALSE;
	DetVib->var.AatAlmCntFlg = FALSE;
	DetVib->var.AatAlmCnt = 0;
	DetVibResetMaxMinSpeedError( DetVib );
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�T�[�{���䔭�U(����g)�Ď�����																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �ʒu���[�v�ɂ�锭�U�Ȃǒ���g�̃T�[�{����̔��U���Ď����A								*/
/*			�T�[�{����̖\���𖢑R�ɖh�~����B														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibDetectLowFreqVibration( DETVIB *DetVib )
{

	LONG	Vib;
	LONG	VibStatus;
	VibStatus = DETECT_VIB_NOT_DETECT;

	/* Peak to Peak�ŐU����Ԃ����o(�E�V�t�g�̓I�[�o�[�t���[�΍�) */
	DetVib->var.SumSpdErr += (DetVib->DetVibObs.var.MaxSpdErrFn - DetVib->DetVibObs.var.MinSpdErrFn) >> 3;
	DetVib->var.SumSpdErr = MlibLimitul( DetVib->var.SumSpdErr, 0x70000000, 0 );
	Vib = DetVib->var.SumSpdErr;

	/*----------------------------------------------------------------------------------------------*/
	/*	����������																					*/
	/*----------------------------------------------------------------------------------------------*/
	/* �U�����x���̃N���A (2ms���ɃN���A) */
	DetVibResetMaxMinSpeedErrorFn( DetVib );
	/* ����g�U�����o�t���O�N���A */
	DetVib->var.LowFreqVibDetFlg = FALSE;

	DetVib->var.LowFreqVibAlmCnt++;
	/*----------------------------------------------------------------------------------------------*/
	/* �A���[�����o�p�o�ߎ��ԃJ�E���g 500ms�㌟�o����												*/
	/*		�U�����o�����𖞂�����Ă�����A�A���[��												*/
	/*		�U�����o��������������Ă��Ȃ�������A�J�E���^�N���A									*/
	/*	���j���o���Ԃ�ύX����ꍇ�͐ώZ�l�̃I�[�o�[�t���[�ɒ���									*/
	/*----------------------------------------------------------------------------------------------*/
	if( DetVib->var.LowFreqVibAlmCnt == KPI_SCANC_MS( 500 ) )
	{
		DetVib->var.LowFreqVibAlmCnt = 0;
		DetVib->var.SumSpdErr = 0;						/* �ώZ�l�N���A								*/
		/*------------------------------------------------------------------------------------------*/
		if( Vib > DetVib->conf.JstVibCheckLevel )
		{
			/* 2010.11.15 Y.Oka TuningLess�͌Ăь��Ŋm�F */
//			if( Gprm.f.TuningLess )						/* �������X�L���H							*/
//			{
//				KPI_ALARMSET( ALM_AT );					/* A.521 : AT�A���[��						*/
				VibStatus = DETECT_VIB_ALARM;
//			}
			DetVib->var.LowFreqVibDetFlg = TRUE;		/* ����g�U�����o							*/
		}
	}
	return	VibStatus;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		�T�[�{���䔭�U(����g)�`�F�b�N���Z�b�g����													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �T�[�{�I�t���ɔ��U(����g)�`�F�b�N�p�ϐ��̃N���A���s���B								*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	DetVibLowFreqResetRequest( DETVIB *DetVib )
{
	DetVib->var.LowFreqVibDetFlg = FALSE;				/* ����g�U�����o�t���O�N���A				*/
	DetVib->var.LowFreqVibAlmCnt = 0;					/* �U���o�ߎ��ԃJ�E���^�N���A				*/
	DetVib->var.SumSpdErr = 0;							/* �ώZ�l�N���A								*/
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X����������																	*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceInitialize( DETVIB *DetVib )
{
	MlibResetLongMemory( &(DetVib->DetVibTrc), sizeof(DetVib->DetVibTrc)/4 );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X����(for Tuning Library)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : Tuning Library ����̗v���ɉ����āA���x�΍��̃g���[�X�����{����B						*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceRuntimeService( DETVIB *DetVib )
{
	LONG		Data;
	DETVIBTRC	*DetVibTrc;
	DetVibTrc = &(DetVib->DetVibTrc);

/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X���s�w�߃`�F�b�N																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( DetVibTrc->TrcCmd != DETVIBTRC_TRCEXE )
	{
		DetVibTrc->TrcIndx = 0;								/* Reset Index							*/
		DetVibTrc->TrcSamp = 1000;							/* Reset TrcSamp for quick start		*/
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�g���[�X�T���v�����O����																	*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibTrc->TrcSamp++;
	if( ++DetVibTrc->TrcSamp < DetVibTrc->TrcInt )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�f�[�^�g���[�X�g���K���o����																*/
/*--------------------------------------------------------------------------------------------------*/
	if( DetVibTrc->TrcIndx == 0 )
	{
		/* 2^24 --> 2^14(16384) */
		Data = ( (DetVib->DetVibObs.var.TrcObsAvSpdErr) >> 10 );
		/* Limit Trace Data */
		Data = MlibLIMITUL( Data, 16384, -16384 );

		if( MlibAbs32( Data ) >= DetVibTrc->TrcTrgLvl )
		{ /* �g���K���x�����������ꍇ */
			/* Store Trace Data to TrcBuf */
			DetVibTrc->TrcBufA[DetVibTrc->TrcIndx] = (SHORT)Data;
			DetVibTrc->TrcIndx++;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		�f�[�^�g���[�X���s����																		*/
/*--------------------------------------------------------------------------------------------------*/
	else if( DetVibTrc->TrcIndx < DETVIBTRC_BUFMAX )
	{ /* Check Index for safety */
		/* 2^24 --> 2^14(16384) */
		Data = ((DetVib->DetVibObs.var.TrcObsAvSpdErr) >> 10);
		/* Limit Trace Data */
		Data = MlibLIMITUL( Data, 16384, -16384 );
		/* Store Trace Data to TrcBuf */
		DetVibTrc->TrcBufA[DetVibTrc->TrcIndx] = (SHORT)Data;
	/*----------------------------------------------------------------------------------------------*/
		if( ++DetVibTrc->TrcIndx < DetVibTrc->TrcNum )
		{
			/* Reset Sampling Counter */
			DetVibTrc->TrcSamp = 0;
		}
	/*----------------------------------------------------------------------------------------------*/
		else
		{ /* Data Trace End */
			DetVibTrc->TrcIndx = 0;								/* Reset Index						*/
			DetVibTrc->TrcSamp = 1000;							/* Reset TrcSamp for quick start	*/
			DetVibTrc->TrcCmd  = DETVIBTRC_TRCEND;				/* Set Trace End to TrcCmd			*/
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X ��Ԏ擾API																*/
/*																									*/
/****************************************************************************************************/
UCHAR	DetVibTraceGetStatus( DETVIB *DetVib )
{
	return	DetVib->DetVibTrc.TrcCmd;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X �f�[�^�o�b�t�@�A�h���X�擾API												*/
/*																									*/
/****************************************************************************************************/
SHORT*	DetVibTraceGetDataAdr( DETVIB *DetVib )
{
	return	&(DetVib->DetVibTrc.TrcBufA[0]);
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X �C���f�b�N�X�擾API														*/
/*																									*/
/****************************************************************************************************/
USHORT	DetVibTraceGetIndex( DETVIB *DetVib )
{
	return	DetVib->DetVibTrc.TrcIndx;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X�J�n�v��API																	*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceStartReq( DETVIB *DetVib )
{
	DetVib->DetVibTrc.TrcCmd = DETVIBTRC_TRCEXE;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X��~�v��API																	*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceStopReq( DETVIB *DetVib )
{
	DetVib->DetVibTrc.TrcCmd = DETVIBTRC_NOCMD;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		���x�΍��g���[�X�f�[�^�ݒ�API																*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceSetCondition( DETVIB *DetVib, USHORT TraceNum, UCHAR TraceInt, SHORT TrigerLvl )
{
	DetVib->DetVibTrc.TrcNum = TraceNum;
	DetVib->DetVibTrc.TrcInt = TraceInt;
	DetVib->DetVibTrc.TrcTrgLvl = TrigerLvl;

	return;
}






/****************************************************************************************************/
/*																									*/
/*		�U�����o���Z�p�p�����[�^�v�Z(�����l)														*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateInitPrm( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�U�����o�p�I�u�U�[�o�Q�C���A���[�p�X�t�B���^�Q�C���A�n�C�p�X�t�B���^�Q�C���ݒ�				*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObsCalculateGains( DetVibObs, VIBOBSGN, DETVIBLP, 0, ScanTimeNs );

/*--------------------------------------------------------------------------------------------------*/
/*		�U�����o�p�I�u�U�[�o�C�i�[�V���Q�C���ݒ�													*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObsCalculateJGain( DetVibObs, Kvx, jrate, ScanTimeNs );

	return;
}


/****************************************************************************************************/
/*																									*/
/*		�U�����o���Z�p�p�����[�^�v�Z	Called from Round, ScanC									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�U�����o�p�I�u�U�[�o�Q�C���A���[�p�X�t�B���^�Q�C���A�n�C�p�X�t�B���^�Q�C���̐ݒ���s���B	*/
/*			Ks   : �U�����o�p�I�u�U�[�o�Q�C��(80Hz �Œ�)											*/
/*			Klpf : �����p���U�����o�p���[�p�X�t�B���^�Q�C��(3183us = 50Hz �Œ�)						*/
/*			Khpf : AAT�U�����o�p�n�C�p�X�t�B���^�Q�C��(�����l�̓t�B���^�Ȃ�)						*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateGains( DETVIBOBS *DetVibObs, LONG ObsGain, LONG Lpf, LONG Hpf, LONG ScanTimeNs )
{
	/* �U�����o�p�I�u�U�[�o�Q�C�� : Ks */
	DetVibObs->conf.Ks = BpxNorObsGainCal( ObsGain, ScanTimeNs );

	/* �����p���U�����o�p���[�p�X�t�B���^�Q�C�� : Klpf */
	DetVibObs->conf.Klpf = MlibPcalKf1gain( Lpf, ScanTimeNs/1000, 0 );

	/* AAT�U�����o�p�n�C�p�X�t�B���^�Q�C�� : Khpf */
	DetVibObs->conf.Khpf = MlibPcalKf1gain( Hpf, ScanTimeNs/1000, 0 );

	return;

}


/****************************************************************************************************/
/*																									*/
/*		�U�����o���Z�p�C�i�[�V���p�����[�^�v�Z (Called from XlsPrmXXXX.c)							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�U�����o�p�I�u�U�[�o�C�i�[�V���Q�C���̐ݒ���s���B											*/
/*			Kj	 : �U�����o�p�I�u�U�[�o�C�i�[�V���Q�C��(100% �Œ�)									*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateJGain( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs )
{
	/* �U�����o�p�I�u�U�[�o�C�i�[�V���Q�C�� : Kj */
	DetVibObs->conf.Kj = BpxNorObsJGainCal( Kvx, jrate, 100, ScanTimeNs );
	return;
}


/****************************************************************************************************/
/*																									*/
/*		�U�����o���Z�p�p�����[�^�����v�Z (for AAT)	Called from Round								*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateInitPrmAAT( DETVIBOBS *DetVibObs, BOOL TuningLess, LONG ScanTimeNs )
{
/*--------------------------------------------------------------------------------------------------*/
/*		�������X��������HPF�Ȃ�																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* Iprm.f.TuningLess */
	if( TuningLess == FALSE)
	{ /* �������X���� */
		/* �n�C�p�X�t�B���^���� */
		DetVibObsCalculateGains( DetVibObs, VIBOBSGN, DETVIBLP, 0, ScanTimeNs );
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		�������X�L������HPF����																		*/
/*--------------------------------------------------------------------------------------------------*/
		/* �n�C�p�X�t�B���^�L�� */
		DetVibObsCalculateGains( DetVibObs, VIBOBSGN, DETVIBLP, DETVIBHP, ScanTimeNs );
	}
	return;
}

/***************************************** end of file **********************************************/
