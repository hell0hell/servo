/****************************************************************************************************/
/*																									*/
/*																									*/
/*		拡張制御演算関数定義																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			1. 調整レス制御演算																		*/
/*			2. 外乱オブザーバ(摩擦補償)演算															*/
/*			3. 位相補償速度オブザーバ演算															*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2013.08.17	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "ExControls.h"
#include "BaseControlPrmCalc.h"


/****************************************************************************************************/
/*		Local Function Definition																	*/
/****************************************************************************************************/
/* 調整レスロバスト補償器演算 */
//static	void	tuneLessRobustCompenSater( TUNELESS_DRCOMP *Drcomp, LONG MotSpd, LONG TrqRef );
static void	tuneLessRobustCompenSater( void *pdrcmp, LONG MotSpd, LONG TrqRef );	/* <S1A5> */
/* 調整レス位相補償速度オブザーバ演算 */
static void tuneLessEhSpeedObserver( TUNELESS_EHVOBS *EhVobs, LONG MotSpd, LONG TrqRef );

/* <S1A5> 再設計版外乱オブザーバ			*/
static void	tuneLessRobustCompensatorEx( void *pprm, LONG MotSpd, LONG TrqRef );
/* <S1A5> ２次フィルタ演算(双一次変換) 		*/
static LONG	tuneLessLowPassfilter2Ex( LONG u, LONG k[5], LONG z[4] );



/****************************************************************************************************/
/*																									*/
/*		調整レス変数初期化																			*/
/*																									*/
/****************************************************************************************************/
void	TuneLessCtrlInitialize( TUNELESS_CTRL *TuneLessCtrl )
{
	/* 調整レス用外乱オブザーバ変数初期化 */
	MlibResetLongMemory( &(TuneLessCtrl->Drcomp.var), sizeof(TuneLessCtrl->Drcomp.var)>>2 );
	/* 調整レス用移送補償速度オブザーバ変数初期化 */
	MlibResetLongMemory( &(TuneLessCtrl->EhVobs.var), sizeof(TuneLessCtrl->EhVobs.var)>>2 );
	/* 調整レス変数初期化 */
	MlibResetLongMemory( &(TuneLessCtrl->var), sizeof(TuneLessCtrl->var)>>2 );
	/* <S1A5> */
	TuneLessInitRobustCompensator( TuneLessCtrl, &(TuneLessCtrl->Drcomp) );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		調整レス制御																				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要 :	調整レス用ロバスト補償器および位相補償速度オブザーバをコールする						*/
/*																									*/
/*	arg		: LONG	MotSpd					: モータ速度				[2^24/OvrSpd]				*/
/*			  LONG	TrqRefi					: フィルタ前トルク指令		[2^24/MaxTrq]				*/
/*			  LONG	TrqRef					: トルク指令				[2^24/MaxTrq]				*/
/*	out		: 																						*/
/*	g-out	: LONG	AoutV.TrqFBCx 			: トルクＦＢ補償			[2^24/MaxTrq]				*/
/*			  LONG	AoutV.SpdFbki			: 速度フィードバック		[2^24/OvrSpd]				*/
/*			  LONG	BoutV.AmonDisTrq		: 外乱トルク				[2^24/MaxTrq]				*/
/*			  LONG	BoutV.AmonDisTrqCmp 	: 補償トルク				[2^24/MaxTrq]				*/
/*			  LONG	BoutV.AmonSpdObs		: 推定速度					[2^24/OvrSpd]				*/
/*																									*/
/****************************************************************************************************/
void	TuningLessCtrl( TUNELESS_CTRL *TuneLessCtrl, KSGAIN MotSpd, KSGAIN TrqRefi, KSGAIN TrqRef )			/*<S00A>*/
{
	TUNELESS_DRCOMP		*Drcomp;
	TUNELESS_EHVOBS		*EhVobs;

	Drcomp = &(TuneLessCtrl->Drcomp);
	EhVobs = &(TuneLessCtrl->EhVobs);

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス用速度FBフィルタ																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	TuneLessCtrl->var.SpdFbFilo = FlibLpfilter1( MotSpd, Drcomp->conf.KVfbFil, TuneLessCtrl->var.SpdFbFilo );
#else
	TuneLessCtrl->var.SpdFbFilo = MlibLpfilter1( MotSpd, Drcomp->conf.KVfbFil, TuneLessCtrl->var.SpdFbFilo );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ロバスト補償器演算																			*/
/*--------------------------------------------------------------------------------------------------*/
//<S1A5>	tuneLessRobustCompenSater( Drcomp, TuneLessCtrl->var.SpdFbFilo, TrqRef );
		Drcomp->conf.RobustCompensator( Drcomp, TuneLessCtrl->var.SpdFbFilo, TrqRef );	/* <S1A5> */

/*--------------------------------------------------------------------------------------------------*/
/*		位相補償速度オブザーバ演算																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->var.TuneLessAct == FALSE)
	{ /* 調整レス無効 */
		/* 調整レスではJLが分からないのでトルク指令ゼロで速度を推定 */
		tuneLessEhSpeedObserver( EhVobs, MotSpd, 0 );
	}
	else
	{
		tuneLessEhSpeedObserver( EhVobs, MotSpd, TrqRefi );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス有効/無効状態設定																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( TuneLessCtrl->var.TuneLessAct == TRUE)
	{ /* 調整レス有効時 */
		TuneLessCtrl->Drcomp.var.CurrentStatus = TRUE;		/* 調整レス有効状態設定					*/
	}
	else
	{ /* 調整レス無効時 */
		TuneLessCtrl->Drcomp.var.CurrentStatus = FALSE;		/* 調整レス無効状態設定					*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		機能OFF/ON初期化処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (TuneLessCtrl->Drcomp.var.CurrentStatus == FALSE)		/* 今回無効							*/
		&& (TuneLessCtrl->Drcomp.var.LastStatus == TRUE) )		/* 前回有効							*/
	{ /* 調整レス有効⇒無効 */
		/* トルク指令フィルタ積分初期化要求 */
		Drcomp->var.TrqFilClrReq = TRUE;
	}
	else if( (TuneLessCtrl->Drcomp.var.CurrentStatus == TRUE)	/* 今回有効							*/
		&& (TuneLessCtrl->Drcomp.var.LastStatus == FALSE) )		/* 前回無効							*/
	{ /* 調整レス無効⇒有効 */
		/* トルク指令フィルタ積分初期化要求 */
		Drcomp->var.TrqFilClrReq = TRUE;
	}
	else
	{ /* 調整レス有効⇒有効 or 無効⇒無効 */
		/* トルク指令フィルタ積分初期化要求なし */
		Drcomp->var.TrqFilClrReq = FALSE;
	}

	/* 前回調整レス機能状態更新 */
	TuneLessCtrl->Drcomp.var.LastStatus = TuneLessCtrl->Drcomp.var.CurrentStatus;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		調整レスロバスト補償器演算(SGDV互換方式)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要 :	トルク指令および差分速度に基づき変動トルクを推定し、									*/
/*			トルク指令に足して新たなトルク指令とする。												*/
/*				Tr1 = Tr0 + Kf * LPF{Tr0 - LPF(J*dωm/dt)}											*/
/*																									*/
/*	arg		: LONG	MotSpd					: 速度フィードバック		[2^24/OvrSpd]				*/
/*			  LONG	TrqRef					: トルク指令				[2^24/MaxTrq]				*/
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

	/* トルク補償値[2^24/MaxTrq] */
	Drcomp->var.DisTrq = FlibLimitul( (fwk - Drcomp->var.TrqLpfo_old), 0x1000000, -0x1000000 );
	fwk = Drcomp->var.DisTrq * Drcomp->conf.Krd;

	/* フィルタ後トルク補償値[2^24/MaxTrq] */
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

	/* トルク補償値[2^24/MaxTrq] */
	Drcomp->var.DisTrq = MlibLimitul( (lwk - Drcomp->var.TrqLpfo_old), 0x1000000, -0x1000000 );
	lwk = MlibMulgainNolim( Drcomp->var.DisTrq, Drcomp->conf.Krd );

	/* フィルタ後トルク補償値[2^24/MaxTrq] */
	Drcomp->var.DisTrqCmp = MlibLpfilter1( lwk, Drcomp->conf.KCmpOutlpf, Drcomp->var.DisTrqCmp );

	Drcomp->var.TrqLpfo_old = Drcomp->var.TrqLpfo;
	Drcomp->var.TrqLpfo = MlibLpfilter1( TrqRef, Drcomp->conf.Krlpf1, Drcomp->var.TrqLpfo );

	return;
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		調整レス位相補償速度オブザーバ演算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要 :	位相補償速度オブザーバの推定速度計算を行う。											*/
/*			トルクフィルタ時定数が１０[μｓ]以下の場合，発振ぎみになってしまうため，				*/
/*			とりあえず，下限値を２０[μｓ]とする。													*/
/*			ただし，この下限値は，速度オブザーバのｋｓＴｆ算出のみに適用される。					*/
/*			したがって，トルクフィルタ部はパラメータ設定値通りになる。								*/
/*																									*/
/*	arg			: MotSpd					: 差分速度				[2^24/nos]						*/
/*				  TrqRef					: トルク指令			[2^24/Tqmax]					*/
/*	out			: EhSpdObs					: 推定速度				[2^24/nos]						*/
/* 	g-out		: 																					*/
/*																									*/
/****************************************************************************************************/
static	void	tuneLessEhSpeedObserver( TUNELESS_EHVOBS *EhVobs, LONG MotSpd, LONG TrqRef )
{
#if (FLOAT_USE==TRUE)
	float	x1, x2, x3, x4, u1;

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバ演算 																*/
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
	/*		位相補償速度オブザーバ演算 																*/
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
/*		外乱オブザーバ変数初期化																	*/
/*																									*/
/****************************************************************************************************/
//<S09B> void	DisturbObserverInitialize( DOBS_CTRL *DobsCtrl )
void	DisturbObserverInitialize( DOBS_CTRL *DobsCtrl, GSELDOBS *pActGains )
{
	/* 外乱オブザーバ変数初期化 */
	MlibResetLongMemory( &(DobsCtrl->var), sizeof(DobsCtrl->var)>>2 );
	/* パラメータ初期化時は第1ゲインを設定 */
//<S09B>	DobsCtrl->DobsPrm[0] = DobsCtrl->DobsPrm[1];
	DobsCtrl->pDobsPrm = pActGains;			/* <S09B> */
	/* 外乱オブザーバ有効・無効設定 */
///*<S00A>*/	DobsCtrl->var.DobsAct = DobsCtrl->var.DobsAct;
	DobsCtrl->var.DobsAct = 0;			/*@@TODO初期化を見直す <S00A>*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		剛体モデル２次外乱オブザーバ演算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要 :	２次外乱オブザーバを構成し，外乱トルクを算出する										*/
/*			オブザーバ演算は機能OFF時も実行する。(アナログモニタも有効)								*/
/*			機能OFF->ONで速度偏差を調整し切替時のショックを低減する									*/
/*																									*/
/*	arg		: LONG	MotSpd				:	速度フィードバック	[2^24/OvrSpd]						*/
/*			  LONG	TrqRef				:	トルク指令			[2^24/MaxTrq]						*/
/*	out		: 																						*/
/*	g-out	: LONG	AoutV.TrqFBCx 		:	トルクＦＢ補償		[2^24/MaxTrq]						*/
/*			  LONG	BoutV.AmonDisTrq	:	外乱トルク			[2^24/MaxTrq]						*/
/*			  LONG	BoutV.AmonDisTrqCmp :	補償トルク			[2^24/MaxTrq]						*/
/*			  LONG	BoutV.AmonDobSpd	:	オブザーバ推定速度	[2^24/OvrSpd]						*/
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
	{ /* 補償有効時 */
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LpxDobsLatchForAnalogfb(DobsCtrl->var.DTrqLpfo);
#endif
		DobsCtrl->var.CurrentStatus = TRUE;								/* 外乱オブザーバ有効状態				*/
	}
	else
	{ /* 補償無効時 */
		DobsCtrl->var.CurrentStatus = FALSE;								/* 外乱オブザーバ無効状態				*/
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
	{ /* 補償有効時 */
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LpxDobsLatchForAnalogfb(DobsCtrl->var.DTrqLpfo);
#endif
		DobsCtrl->var.CurrentStatus = TRUE;								/* 外乱オブザーバ有効状態				*/
	}
	else
	{ /* 補償無効時 */
		DobsCtrl->var.CurrentStatus = FALSE;								/* 外乱オブザーバ無効状態				*/
	}
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*	速度偏差積分初期化 & トルク指令フィルタ積算値初期化処理											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*	外乱オブザーバ機能無効⇔有効切替え時にトルクを連続的に切替えるため積算値の初期化を行う			*/
/*																									*/
/*	①速度偏差積分の初期化																			*/
/*		I成分トルクを外乱補償トルク相当を加味した出力するように積算値を初期化						*/
/*		トルク補償 OFF→ON時：																		*/
/*			「速度積分制御からのトルク＋トルク補償＝前回速度制御トルク」となる必要があるから		*/
/*			速度積分トルク＝速度積分トルク－トルク補償で初期化する。トルク補償はマイナス符号で		*/
/*			速度制御トルクと加算される																*/
/*																									*/
/*		トルク補償 ON→OFF時：																		*/
/*			「速度積分制御からのトルク＝前回トルク補償＋前回速度制御トルク」となる必要があるから	*/
/*			速度積分トルク＝速度積分トルク＋トルク補償で初期化する。								*/
/*																									*/
/*		注意）Integral()関数内の四捨五入処理のため，初期化時には×２が必要							*/
/*																									*/
/*	②トルク指令フィルタ積分値の初期化																*/
/*		トルク指令ローパスフィルタの積算値をトルク補償を加算する前の速度制御ループによるトルクで	*/
/*		初期化する。																				*/
/*																									*/
/*	※上記の処理は制御ループ変数を変更するため、本モジュールの外側で実施する。						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( (DobsCtrl->var.CurrentStatus == TRUE)				/* 今回外乱オブザーバ有効				*/
		&& (DobsCtrl->var.LastStatus == FALSE))				/* 前回外乱オブザーバ無効				*/
	{ /* 外乱オブザーバ無効⇒有効 */
		DobsCtrl->var.ExchangeSts = DOBSCHNG_INV2ACT;
	}
	else if((DobsCtrl->var.CurrentStatus == FALSE)			/* 今回外乱オブザーバ無効				*/
		&& (DobsCtrl->var.LastStatus == TRUE))				/* 前回外乱オブザーバ有効				*/
	{ /* 外乱オブザーバ有効⇒無効 */
		DobsCtrl->var.ExchangeSts = DOBSCHNG_ACT2INV;
	}
	else
	{
		DobsCtrl->var.ExchangeSts = DOBS_NOCHANGE;
	}

	/* 前回外乱オブザーバ機能状態 */
	DobsCtrl->var.LastStatus = DobsCtrl->var.CurrentStatus;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		位相補償速度オブザーバ変数初期化															*/
/*																									*/
/****************************************************************************************************/
void	EhSpeedObserverInitialize( EHVOBS_CTRL *EhVobsCtrl )
{
	MlibResetLongMemory( &(EhVobsCtrl->var), sizeof(EhVobsCtrl->var)>>2 );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		位相補償速度オブザーバ演算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要 :	位相補償速度オブザーバの推定速度計算を行う。											*/
/*			トルクフィルタ時定数を10μs]以下とすると発振気味となるため下限値を20[μs]とする。		*/
/*			ただし、この下限値は速度オブザーバのKsTf算出のみに適用される。							*/
/*			したがって、トルクフィルタ部はパラメータ設定値通りになる。								*/
/*																									*/
/*	arg			: MotSpd				差分速度			[2^24/nos]								*/
/*				  TrqRef				トルク指令			[2^24/Tqmax]							*/
/*	out			: EhVobs.V.EhSpdObs		推定速度			[2^24/nos]								*/
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
	/*	オブザーバへのトルク指令入力にローパスフィルタを入れる										*/
	/*----------------------------------------------------------------------------------------------*/
	EhVobsCtrl->var.TrqRefLpfo = FlibLpfilter1( TrqRef,
												EhVobsCtrl->EhVobsPrm.KTrqReflpf,
												EhVobsCtrl->var.TrqRefLpfo );

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバ演算 																*/
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
	EhVobsCtrl->var.EhSpdObs = FlibLimitul( EhVobsCtrl->var.AccSum, 0x1000000, -0x1000000 );//直接数字に置き換え

#if 0
	/*----------------------------------------------------------------------------------------------*/
	/*		モニタ用変数出力																		*/
	/*----------------------------------------------------------------------------------------------*/
	BoutV.AmonSpdObs = NORMALIZEINT(EhVobsCtrl->var.EhSpdObs);
#endif

	return( EhVobsCtrl->var.EhSpdObs );

#else
	
	LONG	x1, x2, x3, x4, u1;

	/*----------------------------------------------------------------------------------------------*/
	/*	オブザーバへのトルク指令入力にローパスフィルタを入れる										*/
	/*----------------------------------------------------------------------------------------------*/
	EhVobsCtrl->var.TrqRefLpfo = MlibLpfilter1( TrqRef,
												EhVobsCtrl->EhVobsPrm.KTrqReflpf,
												EhVobsCtrl->var.TrqRefLpfo );

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバ演算 																*/
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
	/*		モニタ用変数出力																		*/
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

	/* 未初期化変数修正<S191> */
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
/*		<S1A5> 再設計版外乱オブザーバ																*/
/*		調整レスstep3																				*/
/*																									*/
/*	概要:	トルク指令および差分速度に基づき変動トルクを推定し、									*/
/*			トルク指令に足して新たなトルク指令とする。												*/
/*				Tr1 = Tr0 + Kf * LPF{Tr0 - LPF(J*dωm/dt)}											*/
/*																									*/
/* arg		: LONG	MotSpd				:	速度フィードバック	[2^24/OvrSpd]						*/
/*			  LONG	TrqRef				:	トルク指令			[2^24/MaxTrq]						*/
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
	/* トルク指令側パスの演算 																		*/
	/*----------------------------------------------------------------------------------------------*/
	lwk1 = tuneLessLowPassfilter2Ex( TrqRef, pTl2->KrlpfCur, pdrcmp->var.Krlpftmp1 );

	lwk1 = tuneLessLowPassfilter2Ex( lwk1, pTl2->KrlpfDisObs1, pdrcmp->var.Krlpftmp2 );


	/*----------------------------------------------------------------------------------------------*/
	/* フィードバック速度側パスの演算 																*/
	/*----------------------------------------------------------------------------------------------*/
	pdrcmp->var.VelLpfo1 = MlibLpfilter1( MotSpd, pTl2->KrlpfResSup, pdrcmp->var.VelLpfo1 );

	lwk2 = MotSpd - pdrcmp->var.VelLpfo1;

	lwk2 = lwk2 >> 2;

	lwk2 = pdrcmp->var.VelLpfo1 + lwk2;

	lwk2 = tuneLessLowPassfilter2Ex( lwk2, pTl2->KrlpfDisObs2, pdrcmp->var.Krlpftmp3 );

	lwk2 = MlibMulgain29( lwk2, pdrcmp->conf.Krsj );


	/*----------------------------------------------------------------------------------------------*/
	/* 推定外乱トルク演算 																			*/
	/*----------------------------------------------------------------------------------------------*/
	pdrcmp->var.DisTrq = MlibLimitul( (lwk2 - lwk1), 0x1000000, -0x1000000 );

	lwk1 = MlibMulgainNolim( pdrcmp->var.DisTrq, pdrcmp->conf.Krd );

	pdrcmp->var.DisTrqCmp = MlibLpfilter1( lwk1, pdrcmp->conf.KCmpOutlpf, pdrcmp->var.DisTrqCmp );
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		<S1A5> ２次フィルタ(双一次変換)																*/
/*		調整レスstep3用																				*/
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
/*		<S1A5> ロバスト補償器選択																	*/
/*		調整レスstep3用																				*/
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
