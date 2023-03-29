/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ResVibCtrl.c : 振動検出・制振制御用	モジュール定義											*/
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
/*	Note	:	初版	2010.11.11	Y.Oka	For INGRAM												*/
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
extern void __EI( void );									/* 割り込み許可設定						*/
extern void __DI( void );									/* 割り込み禁止設定						*/



#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		制振制御変数初期化																			*/
/*																									*/
/****************************************************************************************************/
void	ResVibATypeResVibInitialize( RESVIB *ResVib )
{
	MlibResetLongMemory( &ResVib->var, sizeof(ResVib->var)/4 );
	return;
}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */


/* 2014/04/01MDは、下記A型制振処理をﾏｲｸﾛ演算部に移動　*/
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		A型制振制御演算																				*/
/*																									*/
/*	概要:	1次速度オブザーバを構成し，推定速度とFB速度から制振信号を算出する						*/
/*			制振信号にダンピングゲインを乗じて速度FBを補正する										*/
/*			SGDXでは機能OFF時もオブザーバを実行する。(アナログモニタも有効)							*/
/*																									*/
/* arg		: LONG	MotSpd				:	速度フィードバック	[2^24/OvrSpd]						*/
/*			: LONG	TrqRef				:	トルク指令			[2^24/MaxTrq]						*/
/* out		:																						*/
/* g-out	: LONG	SpdFBCx				:	速度FB補償			[2^24/OvrSpd]						*/
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

	/* モニタ用変数出力 */
	ResVib->var.MonResVib  = ResVib->var.dSpdLpfo;

	return	ResVib->var.dSpdCmp;
}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */


#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		制振制御選択スイッチ(Pn10B.3)																*/
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
		case 0x00: /* 制振制御無効 */
			/* 暫定処理 ASICにMREG_RVCTRLの割り当てがないので、保留 */
			ResVib->AresVibSetting = FALSE;
//			SvAsicRegs->AsicMicroReg->MREG_RVCTRL = FALSE;
			break;
		case 0x01: /* Ａ型制振制御有効 */
			/* 暫定処理 ASICにMREG_RVCTRLの割り当てがないので、保留 */
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
		/* 調整レス有効時はA型制振無効 */
		/* 暫定処理 ASICにMREG_RVCTRLの割り当てがないので、保留 */
		ResVib->AresVibSetting = FALSE;
//		SvAsicRegs->AsicMicroReg->MREG_RVCTRL = FALSE;
	}

	return	PrmErr;
}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */



#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		制振制御パラメータ計算																		*/
/*																									*/
/****************************************************************************************************/
void	ResVibCalculatePrm( RESVIB *ResVib, RESVIB_CFG_PRM *Prm, KSGAIN Kvx, LONG ScanTimeNs, ASICS *SvAsicRegs )
{
	RESVIB_PRM	wrkp;
	LONG		kx, sx;
	LONG		wk, wk2;

/*--------------------------------------------------------------------------------------------------*/
/*		制振オブザーバゲイン(制振周波数[0.1Hz]) : ResVib.P.Ks										*/
/*																									*/
/*			= ResFreq[0.1Hz]/2																		*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Ks = BpxNorObsGainCal( (Prm->avibfrq >> 1) ,ScanTimeNs);

/*--------------------------------------------------------------------------------------------------*/
/*		制振オブザーバイナーシャゲイン([%]) : ResVib.P.Kj											*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Kj = BpxNorObsJGainCal( Kvx, Prm->jrate, Prm->avibgn , ScanTimeNs );

/*--------------------------------------------------------------------------------------------------*/
/*		制振速度ダンピングゲイン([%]) : ResVib.P.Knd												*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.Knd = MlibScalKxgain( Prm->avibdamp, 1, 100, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		制振ローパスフィルタゲイン(制振周波数[0.1Hz],時定数補正[0.01ms]) : ResVib.P.Klpf			*/
/*																									*/
/*									         2 * 1000000											*/
/*			フィルタ時定数[0.01ms] = ----------------------------- + adj[0.01ms]					*/
/*			 					  	       2*PAI * ResFreq[0.1Hz]									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 20000000,1, Prm->avibfrq, &sx, 0 );
	wk = MlibPcalKxgain( kx, C10POW7, C2PAIE7, &sx, 24 );

	/* フィルタ時定数ワンパラ計算結果 */
	wk2 = MlibKsgain2Long( wk );

	/* 微調整量加算 */
	wk = ((LONG)Prm->avibfil1 * 10) + wk2;
	if( wk < 0 )
	{ /* 下限リミット */
		wk = 0;
	}
//	wrkp.Klpf = MlibPcalKf1gain( wk, (ScanTimeNs/1000), 0 );
	wrkp.Klpf = MlibPcalKf1gain( wk, ((ScanTimeNs+500)/1000), 0 );

/*--------------------------------------------------------------------------------------------------*/
/*		制振ハイパスフィルタゲイン(制振周波数[0.1Hz],時定数補正[0.01ms]) : ResVib.P.Khpf			*/
/*																									*/
/*									         2 * 100000												*/
/*			フィルタ時定数[0.01ms] = ----------------------------- + adj[0.01ms]					*/
/*			 					  	       2*PAI * ResFreq[Hz]										*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	wk = (LONG)((SHORT)Prm->avibfil2 * 10) + wk2;
	if( wk < 0 )
	{ /* 下限リミット */
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
/*		マイクロＩＦレジスタへの書き込み															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/* 暫定処理　AISCにMREG_RVKSの割り当てがないため、保留 */
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
/*		振動検出演算変数初期化																		*/
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
/*		振動検出演算																				*/
/*																									*/
/****************************************************************************************************/
void	DetVibSpeedObserver( DETVIB *DetVib, LONG SpdFbki, LONG TrqRefMon )
{
	DETVIBOBS	*DetVibObs;
	LONG		x1, x2, u1;
	LONG		SpdErr;

	DetVibObs = &(DetVib->DetVibObs);

	/* 平均化処理(2Scan) */
	DetVibObs->var.AvSpdFbk = ( SpdFbki + DetVibObs->var.AvSpdFbk ) >> 1;

/*--------------------------------------------------------------------------------------------------*/
/*		速度オブザーバ演算																			*/
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
/*		今回値保存																					*/
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
/*		発振検出用最大・最小速度偏差設定処理														*/
/*																									*/
/****************************************************************************************************/
void	DetVibSetSpeedError( DETVIB *DetVib, BOOL TuningLess, BOOL TuningRun )
{
	DETVIBOBS	*DetVibObs;
	DetVibObs = &(DetVib->DetVibObs);

/*--------------------------------------------------------------------------------------------------*/
/*		振動検出用差速度最大値・最小値リセット処理													*/
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
/*		振動(発振)チェック用 --> ScanC:LpxCheckVibration( ), チューニング関係						*/
/*--------------------------------------------------------------------------------------------------*/
	if( TuningRun == TRUE )
	{
		DetVibObs->var.AvSpdErr = MlibHpfilter1( DetVibObs->var.AvSpdErr,
												 DetVibObs->conf.Khpf,
												 &(DetVibObs->var.Hpftmp) );
	}
	/*------------------------------------------------------------------------------------------*/
	/*	調整レス制御時振動検出レベル処理														*/
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
/*		ワンパラメータチューニングアラーム用														*/
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
/*		速度偏差の Peak to Peak 取得用API															*/
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
/*		振動検出レベル 取得用API																	*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibGetVibCheckLevel( DETVIB *DetVib )
{
	return	(DetVib->conf.VibCheckLevel);
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



/****************************************************************************************************/
/*																									*/
/*		振動検出用差速度最大値・最小値リセット	Called from Round, ScanC							*/
/*																									*/
/****************************************************************************************************/
void	DetVibResetMaxMinSpeedError( DETVIB *DetVib )
{
	DetVib->DetVibObs.var.SpdErrClrReq = TRUE;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		振動検出用差速度最大値・最小値リセット (for Fn)		Called from Round, ScanC				*/
/*																									*/
/****************************************************************************************************/
void	DetVibResetMaxMinSpeedErrorFn( DETVIB *DetVib )
{
	DetVib->DetVibObs.var.SpdErrFnClrReq = TRUE;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差の Peak to Peak 取得用API (for Fn)													*/
/*																									*/
/****************************************************************************************************/
LONG	DetVibGetSpdErrPeakFn( DETVIB *DetVib )
{
	return	(DetVib->DetVibObs.var.MaxSpdErrFn - DetVib->DetVibObs.var.MinSpdErrFn);
}



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		サーボ制御状態監視変数初期化処理															*/
/*																									*/
/****************************************************************************************************/
void	DetVibInitialize( DETVIB *DetVib )
{
/*--------------------------------------------------------------------------------------------------*/
/*		サーボ制御状態監視用全変数ゼロクリア														*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(DetVib->var), sizeof( DetVib->var )/4  );

	return;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		サーボ振動(発振)チェック処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボ制御の発振を監視し、サーボ制御の暴走を未然に防止する。							*/
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

	/* Peak to Peakで振動状態を検出 */
	Vib = DetVib->DetVibObs.var.MaxSpdErr - DetVib->DetVibObs.var.MinSpdErr;

	if( Vib > DetVib->conf.VibCheckLevel )
	{
		if( DetVib->conf.VibCheckSel == 1 )
		{
//			KPI_WARNINGSET( WRN_VIB );					/* A.911 : 振動(発振)ワーニング				*/
			VibStatus |= DETECT_VIB_WARNING;
		}
		else if( DetVib->conf.VibCheckSel == 2 )
		{
//			KPI_ALARMSET( ALM_VIB );					/* A.520 : 振動(発振)アラーム				*/
//			KPI_WARNINGCLR( WRN_VIB );					/* A.911 : 振動(発振)ワーニング				*/
			VibStatus |= DETECT_VIB_ALARM;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		アドバンストオートチューニング関係															*/
/*--------------------------------------------------------------------------------------------------*/
/*			アドバンストオートチューニング実行中はリセットしない									*/
/*			アラームレベルを超えたらとりあえずリセット 												*/
/*--------------------------------------------------------------------------------------------------*/
		/* アラームレベルを超えたらリセット */
		DetVibResetMaxMinSpeedError( DetVib );
		/* 振動検出フラグセット */
		DetVib->var.VibDetFlg = TRUE;
	}
	else
	{
//		KPI_WARNINGCLR( WRN_VIB );						/* A.911 : 振動(発振)ワーニング				*/
		/* 振動検出フラグクリア */
		DetVib->var.VibDetFlg = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*			オートチューニングアラーム(A.521)検出処理												*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/* A.521条件(振動アラームレベル×２倍)が満たされたら、カウントスタート許可						*/
	/*----------------------------------------------------------------------------------------------*/
	if( Vib > (DetVib->conf.VibCheckLevel << 1) )
	{
		DetVib->var.AatAlmCntFlg = TRUE;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* ATアラーム検出用経過時間カウントインクリメント												*/
	/*----------------------------------------------------------------------------------------------*/
	if( DetVib->var.AatAlmCntFlg == TRUE )
	{
		DetVib->var.AatAlmCnt++;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* ATアラーム検出用経過時間カウント 50ms後検出処理												*/
	/*		A.521条件が満たされていたら、そのまま処理続行											*/
	/*		A.521条件が満たされていなかったら、カウントクリア										*/
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
	/* ATアラーム検出用経過時間カウント 100ms後検出処理												*/
	/*		A.521条件を満たされていたら、アラーム													*/
	/*		A.521条件が満たされていなかったら、カウンタクリア										*/
	/*----------------------------------------------------------------------------------------------*/
	else if( DetVib->var.AatAlmCnt == KPI_SCANC_MS( 100 ) )
	{
		DetVib->var.AatAlmCntFlg = FALSE;
		DetVib->var.AatAlmCnt = 0;
	/*----------------------------------------------------------------------------------------------*/
#if 0 /* 2010.11.15 Y.Oka SGDVにてA.521出力がコメントアウトされているため、削除 */
		if( Vib > (Gprm.VibCheckLevel<<1) )
		{
			if(BoutV.f.InertiaTuning)
			{
				KPI_ALARMSET( ALM_AT );			/* A.521 : ATアラーム						*/
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
/*		サーボ振動(発振)チェックリセット処理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボオフ時に振動チェック用変数のクリアを行う。										*/
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
/*		サーボ制御発振(低周波)監視処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 位置ループによる発振など低周波のサーボ制御の発振を監視し、								*/
/*			サーボ制御の暴走を未然に防止する。														*/
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

	/* Peak to Peakで振動状態を検出(右シフトはオーバーフロー対策) */
	DetVib->var.SumSpdErr += (DetVib->DetVibObs.var.MaxSpdErrFn - DetVib->DetVibObs.var.MinSpdErrFn) >> 3;
	DetVib->var.SumSpdErr = MlibLimitul( DetVib->var.SumSpdErr, 0x70000000, 0 );
	Vib = DetVib->var.SumSpdErr;

	/*----------------------------------------------------------------------------------------------*/
	/*	初期化処理																					*/
	/*----------------------------------------------------------------------------------------------*/
	/* 振動レベルのクリア (2ms毎にクリア) */
	DetVibResetMaxMinSpeedErrorFn( DetVib );
	/* 低周波振動検出フラグクリア */
	DetVib->var.LowFreqVibDetFlg = FALSE;

	DetVib->var.LowFreqVibAlmCnt++;
	/*----------------------------------------------------------------------------------------------*/
	/* アラーム検出用経過時間カウント 500ms後検出処理												*/
	/*		振動検出条件を満たされていたら、アラーム												*/
	/*		振動検出条件が満たされていなかったら、カウンタクリア									*/
	/*	注）検出時間を変更する場合は積算値のオーバーフローに注意									*/
	/*----------------------------------------------------------------------------------------------*/
	if( DetVib->var.LowFreqVibAlmCnt == KPI_SCANC_MS( 500 ) )
	{
		DetVib->var.LowFreqVibAlmCnt = 0;
		DetVib->var.SumSpdErr = 0;						/* 積算値クリア								*/
		/*------------------------------------------------------------------------------------------*/
		if( Vib > DetVib->conf.JstVibCheckLevel )
		{
			/* 2010.11.15 Y.Oka TuningLessは呼び元で確認 */
//			if( Gprm.f.TuningLess )						/* 調整レス有効？							*/
//			{
//				KPI_ALARMSET( ALM_AT );					/* A.521 : ATアラーム						*/
				VibStatus = DETECT_VIB_ALARM;
//			}
			DetVib->var.LowFreqVibDetFlg = TRUE;		/* 低周波振動検出							*/
		}
	}
	return	VibStatus;
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



#if( CSW_SVCF_VIB_DETECT_USE != 0 )
/****************************************************************************************************/
/*																									*/
/*		サーボ制御発振(低周波)チェックリセット処理													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボオフ時に発振(低周波)チェック用変数のクリアを行う。								*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	DetVibLowFreqResetRequest( DETVIB *DetVib )
{
	DetVib->var.LowFreqVibDetFlg = FALSE;				/* 低周波振動検出フラグクリア				*/
	DetVib->var.LowFreqVibAlmCnt = 0;					/* 振動経過時間カウンタクリア				*/
	DetVib->var.SumSpdErr = 0;							/* 積算値クリア								*/
}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレース初期化処理																	*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceInitialize( DETVIB *DetVib )
{
	MlibResetLongMemory( &(DetVib->DetVibTrc), sizeof(DetVib->DetVibTrc)/4 );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレース処理(for Tuning Library)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : Tuning Library からの要求に応じて、速度偏差のトレースを実施する。						*/
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
/*		トレース実行指令チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( DetVibTrc->TrcCmd != DETVIBTRC_TRCEXE )
	{
		DetVibTrc->TrcIndx = 0;								/* Reset Index							*/
		DetVibTrc->TrcSamp = 1000;							/* Reset TrcSamp for quick start		*/
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		トレースサンプリング制御																	*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibTrc->TrcSamp++;
	if( ++DetVibTrc->TrcSamp < DetVibTrc->TrcInt )
	{
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		データトレーストリガ検出処理																*/
/*--------------------------------------------------------------------------------------------------*/
	if( DetVibTrc->TrcIndx == 0 )
	{
		/* 2^24 --> 2^14(16384) */
		Data = ( (DetVib->DetVibObs.var.TrcObsAvSpdErr) >> 10 );
		/* Limit Trace Data */
		Data = MlibLIMITUL( Data, 16384, -16384 );

		if( MlibAbs32( Data ) >= DetVibTrc->TrcTrgLvl )
		{ /* トリガレベルを上回った場合 */
			/* Store Trace Data to TrcBuf */
			DetVibTrc->TrcBufA[DetVibTrc->TrcIndx] = (SHORT)Data;
			DetVibTrc->TrcIndx++;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		データトレース実行処理																		*/
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
/*		速度偏差トレース 状態取得API																*/
/*																									*/
/****************************************************************************************************/
UCHAR	DetVibTraceGetStatus( DETVIB *DetVib )
{
	return	DetVib->DetVibTrc.TrcCmd;
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレース データバッファアドレス取得API												*/
/*																									*/
/****************************************************************************************************/
SHORT*	DetVibTraceGetDataAdr( DETVIB *DetVib )
{
	return	&(DetVib->DetVibTrc.TrcBufA[0]);
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレース インデックス取得API														*/
/*																									*/
/****************************************************************************************************/
USHORT	DetVibTraceGetIndex( DETVIB *DetVib )
{
	return	DetVib->DetVibTrc.TrcIndx;
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレース開始要求API																	*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceStartReq( DETVIB *DetVib )
{
	DetVib->DetVibTrc.TrcCmd = DETVIBTRC_TRCEXE;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレース停止要求API																	*/
/*																									*/
/****************************************************************************************************/
void	DetVibTraceStopReq( DETVIB *DetVib )
{
	DetVib->DetVibTrc.TrcCmd = DETVIBTRC_NOCMD;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		速度偏差トレースデータ設定API																*/
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
/*		振動検出演算用パラメータ計算(初期値)														*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateInitPrm( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs )
{
/*--------------------------------------------------------------------------------------------------*/
/*		振動検出用オブザーバゲイン、ローパスフィルタゲイン、ハイパスフィルタゲイン設定				*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObsCalculateGains( DetVibObs, VIBOBSGN, DETVIBLP, 0, ScanTimeNs );

/*--------------------------------------------------------------------------------------------------*/
/*		振動検出用オブザーバイナーシャゲイン設定													*/
/*--------------------------------------------------------------------------------------------------*/
	DetVibObsCalculateJGain( DetVibObs, Kvx, jrate, ScanTimeNs );

	return;
}


/****************************************************************************************************/
/*																									*/
/*		振動検出演算用パラメータ計算	Called from Round, ScanC									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		振動検出用オブザーバゲイン、ローパスフィルタゲイン、ハイパスフィルタゲインの設定を行う。	*/
/*			Ks   : 振動検出用オブザーバゲイン(80Hz 固定)											*/
/*			Klpf : ワンパラ振動検出用ローパスフィルタゲイン(3183us = 50Hz 固定)						*/
/*			Khpf : AAT振動検出用ハイパスフィルタゲイン(初期値はフィルタなし)						*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateGains( DETVIBOBS *DetVibObs, LONG ObsGain, LONG Lpf, LONG Hpf, LONG ScanTimeNs )
{
	/* 振動検出用オブザーバゲイン : Ks */
	DetVibObs->conf.Ks = BpxNorObsGainCal( ObsGain, ScanTimeNs );

	/* ワンパラ振動検出用ローパスフィルタゲイン : Klpf */
	DetVibObs->conf.Klpf = MlibPcalKf1gain( Lpf, ScanTimeNs/1000, 0 );

	/* AAT振動検出用ハイパスフィルタゲイン : Khpf */
	DetVibObs->conf.Khpf = MlibPcalKf1gain( Hpf, ScanTimeNs/1000, 0 );

	return;

}


/****************************************************************************************************/
/*																									*/
/*		振動検出演算用イナーシャパラメータ計算 (Called from XlsPrmXXXX.c)							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		振動検出用オブザーバイナーシャゲインの設定を行う。											*/
/*			Kj	 : 振動検出用オブザーバイナーシャゲイン(100% 固定)									*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateJGain( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs )
{
	/* 振動検出用オブザーバイナーシャゲイン : Kj */
	DetVibObs->conf.Kj = BpxNorObsJGainCal( Kvx, jrate, 100, ScanTimeNs );
	return;
}


/****************************************************************************************************/
/*																									*/
/*		振動検出演算用パラメータ初期計算 (for AAT)	Called from Round								*/
/*																									*/
/****************************************************************************************************/
void	DetVibObsCalculateInitPrmAAT( DETVIBOBS *DetVibObs, BOOL TuningLess, LONG ScanTimeNs )
{
/*--------------------------------------------------------------------------------------------------*/
/*		調整レス無効時はHPFなし																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* Iprm.f.TuningLess */
	if( TuningLess == FALSE)
	{ /* 調整レス無効 */
		/* ハイパスフィルタ無効 */
		DetVibObsCalculateGains( DetVibObs, VIBOBSGN, DETVIBLP, 0, ScanTimeNs );
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		調整レス有効時はHPFあり																		*/
/*--------------------------------------------------------------------------------------------------*/
		/* ハイパスフィルタ有効 */
		DetVibObsCalculateGains( DetVibObs, VIBOBSGN, DETVIBLP, DETVIBHP, ScanTimeNs );
	}
	return;
}

/***************************************** end of file **********************************************/
