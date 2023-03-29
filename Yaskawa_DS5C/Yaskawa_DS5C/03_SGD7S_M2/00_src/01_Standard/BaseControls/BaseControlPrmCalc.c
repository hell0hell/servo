/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControlPrmCalc.c : BaseControl�p�p�����[�^�v�Z�֐���`									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.11.15	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "BaseControlPrmCalc.h"
#include "MicroPrmCalc.h"



/****************************************************************************************************/
/*																									*/
/*		���x����p�����[�^�v�Z																		*/
/*																									*/
/****************************************************************************************************/
void	PcalBaseSpeedControl( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, BASE_SPDLOOP_PRM *Prm, LONG GselNo )
{
	GSELGAINS			*GselGains;
	TUNELESS_PRM		*TunLessPrm;
	CTRL_CMD_PRM		*CtrlCmdPrm;
#if (FLOAT_USE==TRUE)
	float				jrate;
	float				Loophz;
	float				Pitime;
	float				kx, sx;
#else
	ULONG				jrate;
	ULONG				Loophz;
	ULONG				Pitime;
	LONG				kx, sx;
#endif /* FLOAT_USE */


	CtrlCmdPrm	= &(BaseCtrlData->CtrlCmdPrm);
	GselGains	= &(BaseCtrlData->GainChange.GselGains[GselNo]);
	jrate		= Prm->jrate;

	if( GselNo == 1 )
	{ /* ��1�Q�C���v�Z�� */
		Loophz = Prm->loophz;
		Pitime = Prm->pitime;
	}
	else if( GselNo == 2 )
	{ /* ��2�Q�C���v�Z�� */
		Loophz = Prm->loophz2;
		Pitime = Prm->pitime2;
	}
	else if( GselNo == 5 )
	{ /* ��5(�������X)�Q�C���v�Z�� */
		TunLessPrm = &(BaseCtrlData->TuneLessCtrl.TuneLessPrm);
		Loophz = TunLessPrm->kv2;
		Pitime = TunLessPrm->Ti;
		jrate = TunLessPrm->jrate;
	}
	else
	{ /* �ُ펞�͑��Q�C�� */
		Loophz = Prm->loophz;
		Pitime = Prm->pitime;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	PI����^I-P����Q�C���p�����[�^�v�Z														*/
	/*------------------------------------------------------------------------------------------*/
	if( BaseCtrlData->CtrlCmdPrm.IPSpdControl )				/* I-P����					*/
	{
#if (FLOAT_USE==TRUE)
		CtrlCmdPrm->PI_rate = (float)( 100 - Prm->ipg_rwt.b.l ) / 100.0f;
#else
		CtrlCmdPrm->PI_rate = MlibScalKxgain( 100 - Prm->ipg_rwt.b.l, 1, 100, NULL, 24 );
#endif

#if (FLOAT_USE==TRUE)
		GselGains->Kv = 1.0f;
		GselGains->Kvi = GselGains->Kv * (float)KPI_MBCYCLEUS / ( 10.0f * Pitime );
		GselGains->Kv2 = Bprm->Kvx * (( 100.0f + jrate ) * Loophz ) / 1000.0f;
		GselGains->Kv2Inv = 1000.0f / Bprm->Kvx / (( 100.0f + jrate ) * Loophz ); //modoshi
#else
		GselGains->Kv = 1;
		GselGains->Kvi = MlibScalKskxkx( GselGains->Kv, KPI_MBCYCLEUS, 10*Pitime, NULL, 24 );
		GselGains->Kv2 = MlibScalKskxkx( Bprm->Kvx, (100+jrate)*Loophz,  1000, NULL, 24 );
		kx = MlibScalKxkxks( 1000, 1, Bprm->Kvx, &sx, 0 );
		GselGains->Kv2Inv = MlibPcalKxgain( kx, 1, (100+jrate)*Loophz, &sx, 24 );
#endif
	}
	else															/* PI����					*/
	{
#if (FLOAT_USE==TRUE)
		CtrlCmdPrm->PI_rate = 1.0f;
#else
		CtrlCmdPrm->PI_rate = 1;
#endif

#if (FLOAT_USE==TRUE)
		GselGains->Kv = Bprm->Kvx * (( 100.0f + jrate ) * Loophz ) / 1000.0f;
		GselGains->Kvi = GselGains->Kv * (float)KPI_MBCYCLEUS / ( 10.0f * Pitime );
		GselGains->Kv2 = 1.0;
		GselGains->Kv2Inv = 1.0;
#else
		GselGains->Kv =  MlibScalKskxkx( Bprm->Kvx, (100+jrate)*Loophz, 1000, NULL, 24 );
		GselGains->Kvi = MlibScalKskxkx( GselGains->Kv, KPI_MBCYCLEUS, 10*Pitime, NULL, 24 );
		GselGains->Kv2 = 1;
		GselGains->Kv2Inv = 1;
#endif
	}
}



/****************************************************************************************************/
/*																									*/
/*		�ʒu������Q�C���p�����[�^�v�Z															*/
/*																									*/
/****************************************************************************************************/
void	PcalBasePositionPControl( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, EGEAR *Egear, ULONG Posgn, LONG GselNo )
{
	GSELGAINS	*GselGains;
#if (FLOAT_USE==TRUE)
	float		fw;
#else
	LONG		kx, sx;
#endif

	GselGains = &(BaseCtrlData->GainChange.GselGains[GselNo]);





#if (FLOAT_USE==TRUE)
	GselGains->Kp = Bprm->Kpx * Posgn / 10.0F;
	GselGains->KpInv = 10000000.0F / ( Posgn * (float)KPI_MBCYCLEUS );
#else
	GselGains->Kp = MlibScalKskxkx( Bprm->Kpx, Posgn, 10, NULL, 24 );
	GselGains->KpInv = MlibScalKxgain( 1000000, 10, Posgn*KPI_MBCYCLEUS, NULL, -24 );
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*	���z�ʒu�΍��v�Z�p�Q�C��																	*/
	/*																								*/
	/*		BinK.MotSpd[2^24/OvrSpd]-->���z�ʒu�΍�[�w�ߒP��]�ɕϊ����邽�߂̃Q�C��					*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = 10.0F / Posgn;
	fw = fw / Bprm->Kpx;
	GselGains->VirtualPosKp = fw * (float)Egear->a / (float)Egear->b;
#else
	kx = MlibScalKxgain( 1, 10, Posgn,    &sx, 0);
	kx = MlibPcalKxkxks( kx, 1, Bprm->Kpx, &sx, 0);
	GselGains->VirtualPosKp = MlibPcalKxgain( kx, Egear->a, Egear->b, &sx, 24 );
#endif

}



/****************************************************************************************************/
/*																									*/
/*		�[���N�����v�ʒu����Q�C���v�Z																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	�@ �\ : �[���N�����v����p�̈ʒu����Q�C���̌v�Z���s���B										*/
/*																									*/
/*	�� �� : ���[�^�ʒu����ł���A�t���N���[�Y�h����̓T�|�[�g���Ă��Ȃ��̂Œ��ӂ��邱�ƁB			*/
/*																									*/
/*			Bprm.Kfbpls	 : �e�a�p���X�ϊ��W��[rad/pulse]    --- �t���N�Ή�							*/
/*			Bprm.Kmotpls : ���[�^�p���X�ϊ��W��[rad/pulse]	--- �t���N�Ƃ͓Ɨ�						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	PcalZeroPositionControl( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, LONG Posgn, LONG GselNo )
{
#if (FLOAT_USE==TRUE)
	float	fw;

	GSELGAINS	*GselGains;

	GselGains	= &(BaseCtrlData->GainChange.GselGains[GselNo]);

	fw = (float)0x01000000 * Bprm->Kmotpls / Bprm->OvrSpd;
	fw = fw * Posgn / 10.0F;
	GselGains->ZcKp = fw;
#else
	LONG		kx, sx;
	GSELGAINS	*GselGains;

	GselGains	= &(BaseCtrlData->GainChange.GselGains[GselNo]);

	sx = 0;
	kx = MlibScalKxksks( 0x01000000, Bprm->Kmotpls, Bprm->OvrSpd, &sx,  0 );
	kx = MlibPcalKxgain( kx,         Posgn,        10,          &sx, 24 );
	GselGains->ZcKp = kx;
#endif
}


/****************************************************************************************************/
/*																									*/
/*		�g���N���䎞���x�����ϕ��Q�C���v�Z															*/
/*																									*/
/****************************************************************************************************/
void	PcalSpeedLimitGain( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, USHORT jrate, ULONG Loophz, LONG GselNo )
{
	ULONG			SpdLimGain;
	LONG			SpdLimIntegGain;
	CTRL_CMD_PRM	*CtrlCmdPrm;

	CtrlCmdPrm	= &(BaseCtrlData->CtrlCmdPrm);

	/*------------------------------------------------------------------------------------------*/
	/*	���x���[�v�Q�C����10.0Hz�����̏ꍇ�́A���x�����p���[�v�Q�C����10.0Hz�ɂ���				*/
	/*------------------------------------------------------------------------------------------*/
	if( Loophz < 100 )
	{
		SpdLimGain      = 100;
		SpdLimIntegGain = 32000;
	}
	else if( Loophz < 200 )
	{
		SpdLimGain      = Loophz;
		SpdLimIntegGain = 3200000 / Loophz;
	}
	else
	{
		SpdLimGain      = Loophz;
		SpdLimIntegGain = 16000;
	}
#if (FLOAT_USE==TRUE)
	/* ���x�����p ���[�v�Q�C���̌v�Z */
	CtrlCmdPrm->OverTrqLoopGain[GselNo] = 
			Bprm->Kvx * (float)(( 100 + jrate ) * SpdLimGain ) / 1000.0f;
	/* ���x�����p �ϕ��Q�C���̌v�Z *//* �ϕ����萔��16.00ms�Œ� */
	CtrlCmdPrm->OverTrqIntegGain[GselNo] = 
			CtrlCmdPrm->OverTrqLoopGain[GselNo] * (float)Bprm->SvCycleUs / (float)SpdLimIntegGain;
#else
	/* ���x�����p ���[�v�Q�C���̌v�Z */
	CtrlCmdPrm->OverTrqLoopGain[GselNo] =
			MlibScalKskxkx( Bprm->Kvx, (100+ jrate)*SpdLimGain, 1000, NULL, 24 );
	/* ���x�����p �ϕ��Q�C���̌v�Z *//* �ϕ����萔��16.00ms�Œ� */
	CtrlCmdPrm->OverTrqIntegGain[GselNo] =
			MlibScalKskxkx( CtrlCmdPrm->OverTrqLoopGain[GselNo], Bprm->SvCycleUs, SpdLimIntegGain, NULL, 24 );
#endif /* FLOAT_USE */

}


/****************************************************************************************************/
/*																									*/
/*		�g���N�t�B���^�F���[�p�X�t�B���^�p�����[�^�v�Z												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�⑫�F�g���N���䃂�[�h���̃g���N�t�B���^�́A��P�g���N�t�B���^�̂ݑΉ�						*/
/*																									*/
/****************************************************************************************************/
void	PcalBaseTrqLpassFilter( TRQFIL *TrqFilData, BASE_CTRL_HNDL *BaseCtrlData,
														LONG trqfil11, LONG trqfil12, LONG GselNo )
{
	LONG			cyc_time;
	GSELGAINS		*GselGains;
//	CTRL_CMD_PRM	*CtrlCmdPrm;//<S0C7>

	GselGains	= &(BaseCtrlData->GainChange.GselGains[GselNo]);
//	CtrlCmdPrm	= &(BaseCtrlData->CtrlCmdPrm);//<S0C7>
	cyc_time	= KPI_MBCYCLEUS;
#if (FLOAT_USE==TRUE)
	TrqFilData->A.Klpf = FlibPcalKf1gain( trqfil11 * 10, cyc_time, 0 ); 
	GselGains->Klpf = FlibPcalKf1gain( trqfil12 * 10, cyc_time, 0 );
#else
	TrqFilData->A.Klpf = MlibPcalKf1gain( 10 * trqfil11, cyc_time, 0 );
	GselGains->Klpf = MlibPcalKf1gain( 10 * trqfil12, cyc_time, 0 );
#endif /* FLOAT_USE */
}


/****************************************************************************************************/
/*																									*/
/*		�g���N�t�B���^�F�Q�����[�p�X�t�B���^�p�����[�^�v�Z											*/
/*																									*/
/****************************************************************************************************/
/*		MlibLpfilter2( )�͍폜����Ă���g�p�ł��Ȃ��BMlibNxfilter2( )�ő�p����B					*/
/*		kn = 0 �Ƃ��邱�ƂŁA�Q�����[�p�X�t�B���^�ɂȂ�B(kn:�m�b�`�^�񎟂k�o�e�ؑւ��W�� [0.001])	*/
/****************************************************************************************************/
/*		5000Hz�Ή��̂��ߑo�ꎟ�ϊ��ɕύX����B														*/
/****************************************************************************************************/
void	PcalBaseTrqLpassFilter2( TRQFIL *TrqFilData, BASE_CTRL_HNDL *BaseCtrlData,
																		USHORT secfil, USHORT secq )
{
#if (FLOAT_USE==TRUE)
	float			hz;
	float			qx;
#else
	LONG			hz;
	LONG			qx;
#endif /* FLOAT_USE */

//	CTRL_CMD_PRM	*CtrlCmdPrm;//<S0C7>

//	CtrlCmdPrm	= &(BaseCtrlData->CtrlCmdPrm);//<S0C7>

	hz = secfil;				/* ���g�� [Hz]									*/
	qx = 10 * secq;				/* �p�萔 [0.001]								*/
#if (FLOAT_USE==TRUE)
	f_PcalLowfilter2( hz, qx, KPI_SACYCLEUS, TrqFilData->A.Klpf2 );
	f_PcalLowfilter2( hz, qx, KPI_SACYCLEUS, TrqFilData->P.Klpf2 );
#else
	PcalLowfilter2( hz, qx, KPI_SACYCLEUS, TrqFilData->A.Klpf2 );
	PcalLowfilter2( hz, qx, KPI_SACYCLEUS, TrqFilData->P.Klpf2 );
#endif /* FLOAT_USE */
}

#if (FLOAT_USE==TRUE)
/****************************************************************************************************/
/*																									*/
/*		�Q�����[�p�X�t�B���^�p�����[�^�v�Z(�o�ꎟ�ϊ�)												*/
/*																									*/
/****************************************************************************************************/
void	f_PcalLowfilter2(

		float	hz,					/* �t�B���^���g��		[Hz]									*/
		float	qx,					/* �N�H���e�B�t�@�N�^	[-]									*/
		float	ts,					/* ���Z����				[sec]									*/
		float	kn[4] )				/* �v�Z���ʊi�[													*/
{
float	ka,kb,kc;
float	kx,ky;

	/*----------------------------------------------------------------------------------------------*/
	/*	 ���g���␳																					*/
	/*----------------------------------------------------------------------------------------------*/
		if ( hz > 2000 )
		{
			hz = ( hz * (3.0f * hz - 1894.0f) + 11788000.0f) / 10000.0f;
		}

		kc = (2*PAI*hz)*(2*PAI*hz)*ts*ts;						/* wn*wn*ts*ts					*/

		kb = 2*(2*PAI*hz)*ts/qx;									/* 2*wn*ts/qx					*/

		kx = 4 + (2*PAI*hz)*(2*PAI*hz)*ts*ts;					/* 4 + wn*wn*ts*ts				*/
		ky = 2*(4 - (2*PAI*hz)*(2*PAI*hz)*ts*ts);				/* 2*(4 - wn*wn*ts*ts)			*/

		ka = kx + kb;												/* 4 + 2*wn*ts/qx + wn*wn*ts*ts	*/
		kb = kx - kb;												/* 4 - 2*wn*ts/qx + wn*wn*ts*ts	*/

		kc = hz*hz*ts*ts/773019.0f;

		kb = hz * ts / qx * 2*2*PAI/1000.0f;

		kx = 4 + kc;
		ky = 4 - kc;

		ka = kx + kb;
		kb = kx - kb;


		kn[0] = kc / ka;											/* kc / ka						*/
		kn[1] = ky / ka;											/* ky / ka						*/
		kn[2] = kb / ka;											/* kb / ka						*/
		kn[3] = 2 * kn[0];											/* 2 * K[0]						*/

	return;
}
#else
/****************************************************************************************************/
/*																									*/
/*		�Q�����[�p�X�t�B���^�p�����[�^�v�Z(�o�ꎟ�ϊ�)												*/
/*																									*/
/****************************************************************************************************/
LONG	PcalLowfilter2(
		LONG	hz,					/* �t�B���^���g��		[Hz]									*/
		LONG	qx,					/* �N�H���e�B�t�@�N�^	[0.001]									*/
		LONG	ts,					/* ���Z����				[us]									*/
		KSGAIN	kn[4] )				/* �v�Z���ʊi�[													*/
{
	LONG	ka,sa,kb,sb,kc,sc;
	LONG	kx,sx,ky,sy = 0;

	sa = 0;
	sx = 0;
	sy = 0;

	/*----------------------------------------------------------------------------------------------*/
	/*	 ���g���␳																					*/
	/*----------------------------------------------------------------------------------------------*/
	if( hz > 2000 )
	{
		hz = (LONG)( (SHORT)hz * (SHORT)(3 * hz - 1894) + 11788000) / 10000;
	}

	sc = 15;
	kc = MlibPcalKxgain( hz*hz,  ts*ts,  773019,   &sc, 0 );		/* wn*wn*ts*ts					*/
																	/* 773019=1000000^2/(2��^2)/2^15*/
	sb = 0;
	kb = MlibPcalKxgain( hz,     ts,     qx,       &sb, 0 );
	kb = MlibPcalKxgain( kb,     125664, 10000000, &sb, 0 );		/* 2*wn*ts/qx					*/
																	/* 125664 = 2*2��*10000			*/

	kx = MlibPcalKxaddx( 4<<24,  24,  kc,  sc,  &sx );				/* 4 + wn*wn*ts*ts				*/
	ky = MlibPcalKxsubx( 4<<24,  24,  kc,  sc,  &sy );				/* 2*(4 - wn*wn*ts*ts)			*/
	sy = sy - 1;
	ka = MlibPcalKxaddx( kx,     sx,  kb,  sb,  &sa );				/* 4 + 2*wn*ts/qx + wn*wn*ts*ts	*/
	kb = MlibPcalKxsubx( kx,     sx,  kb,  sb,  &sb );				/* 4 - 2*wn*ts/qx + wn*wn*ts*ts	*/

	kn[0] = MlibPcalKxdivx( kc,  sc,  ka,  sa,  24  );				/* {kc,sc}/{ka,sa}				*/
	kn[1] = MlibPcalKxdivx( ky,  sy,  ka,  sa,  24  );				/* {ky,sy}/{ka,sa}				*/
	kn[2] = MlibPcalKxdivx( kb,  sb,  ka,  sa,  24  );				/* {kb,sb}/{ka,sa}				*/
	kn[3] = MlibPcalKxdivx( kc,  (sc - 1),  ka,  sa,  24  );		/*  2 * K[0]					*/

	return( 0 );
}
#endif /* FLOAT_USE */

/****************************************************************************************************/
/*																									*/
/*		�g���N�t�B���^(�g���N�⏞��)�F���[�p�X�t�B���^�p�����[�^�v�Z								*/
/*																									*/
/****************************************************************************************************/
KSGAIN	PcalBaseTrqLpassFilter3( LONG Trqfil, LONG ScanTime )
{
#if (FLOAT_USE==TRUE)
	return FlibPcalKf1gain( Trqfil * 10, ScanTime, 0 );
#else
	return MlibPcalKf1gain( 10*Trqfil, ScanTime, 0 );
#endif /* FLOAT_USE */
}


/****************************************************************************************************/
/*																									*/
/*		�ʒu�w�߃t�B�[�h�t�H���[�h�Q�C���v�Z														*/
/*																									*/
/****************************************************************************************************/
KSGAIN	PcaldPosRefFFGain( KSGAIN Kpx, LONG ffgn, LONG ScanTime )
{

	KSGAIN	KdPff;
#if (FLOAT_USE==TRUE)
	float	fw;
	fw = Kpx * 1000000.0F / (float)ScanTime;
	KdPff = fw * (float)ffgn / 100.0F;

#else
	LONG	kx;
	LONG	sx;
	kx = MlibScalKskxkx( Kpx, 1000000, ScanTime, &sx, 0 );
	KdPff = MlibPcalKxgain( kx, ffgn, 100, &sx, 24 );
#endif

	return	KdPff;
}

#if (FLOAT_USE==TRUE) 
/****************************************************************************************************/
/*																									*/
/*		���K���I�u�U�[�o�Q�C���v�Z : ObsGain [0.1Hz] --> NormalizedObsGain [rad/ts] 					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					  ObsGain * 2*PAI * CycleNs[ns]													*/
/*				x = -------------------------------													*/
/*							 10 * 10^9																*/
/*																									*/
/****************************************************************************************************/
float	f_BpxNorObsGainCal( float ObsGain , float scantime)
{

	float x;

	x = ObsGain * 2.0f * PAI * scantime / 10.0f/1000000000.0f/100.0f;

	return( x );
}
#endif /* FLOAT_USE */
/****************************************************************************************************/
/*																									*/
/*		���K���I�u�U�[�o�Q�C���v�Z : ObsGain [0.1Hz] --> NormalizedObsGain [rad/ts] 					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					  ObsGain * 2*PAI * CycleNs[ns]													*/
/*				x = -------------------------------													*/
/*							 10 * 10^9																*/
/*																									*/
/****************************************************************************************************/
LONG	BpxNorObsGainCal( LONG ObsGain , LONG scantime)
{
	LONG	x, sx;
	sx = 0;

	x = MlibScalKxgain( ObsGain, C2PAIE7, (C10POW7 * 10), &sx, 0 );
	x = MlibPcalKxgain( x, scantime, C10POW9, &sx, 24 );
	return( x );
}



#if (FLOAT_USE==TRUE) 
/****************************************************************************************************/
/*																									*/
/*		���K���I�u�U�[�o�C�i�[�V���Q�C���v�Z : ObsJGain [%] --> NormalizedObsJGain 					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      MaxTrq * 100 * CycleNs[ns] * ObsJGain[%]									*/
/*				x = ------------------------------------------------								*/
/*					  OvrSpd * Jmot * (100+Prm.jrate) * 10^9 * 100									*/
/*																									*/
/*					  2*PAI * 100 * CycleNs * ObsJGain												*/
/*				  = --------------------------------------											*/
/*					  Kvx * (100+Prm.jrate) * 10^9 * 100											*/
/*																									*/
/****************************************************************************************************/
float	f_BpxNorObsJGainCal( KSGAIN Kvx, float jrate, float ObsJGain , float scantime )
{

	float x;
	//float kx, ky;

	//kx = (ObsJGain * scantime * C2PAIE7);
	//ky = Kvx * (float)C10POW7;
	//ky = ky * (float)C10POW9 / (float)(100+jrate);
	//x = kx / ky; //modoshi

	x = 2 * PAI * scantime * ObsJGain / (Kvx * (100.0f+jrate) * 1000000000.0f);

	return( x );

}
#endif /* FLOAT_USE */
/****************************************************************************************************/
/*																									*/
/*		���K���I�u�U�[�o�C�i�[�V���Q�C���v�Z : ObsJGain [%] --> NormalizedObsJGain 					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					      MaxTrq * 100 * CycleNs[ns] * ObsJGain[%]									*/
/*				x = ------------------------------------------------								*/
/*					  OvrSpd * Jmot * (100+Prm.jrate) * 10^9 * 100									*/
/*																									*/
/*					  2*PAI * 100 * CycleNs * ObsJGain												*/
/*				  = --------------------------------------											*/
/*					  Kvx * (100+Prm.jrate) * 10^9 * 100											*/
/*																									*/
/****************************************************************************************************/
LONG	BpxNorObsJGainCal( KSGAIN Kvx, LONG jrate, LONG ObsJGain , LONG scantime )
{
	LONG	x;
	LONG	kx,sx;
	LONG	ky,sy;

	sy = 0;
	sx = 0;

	kx = MlibPcalKxmulx( ObsJGain, scantime, C2PAIE7, &sx );
	ky = MlibScalKskxkx( Kvx, C10POW7, 1, &sy, 0 );
	ky = MlibPcalKxmulx( ky, C10POW9, (100+jrate), &sy );
	x = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

	return( x );

}



/***************************************** end of file **********************************************/
