/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RemVibFreq.c : 残留振動周波数計算処理定義													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 残留振動周波数計算処理																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2011.02.01	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MLib.h"
#include "KLib.h"
#include "RemVibFreq.h"



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	RemVibChkCondition( REMVIBFREQ *RemVibFreq, BOOL PosCtrlMode );							*/
/*	void	RemVibCalculateFreq( REMVIBFREQ *RemVibFreq, LONG CtrlPosErr,							*/
/*																BOOL CoinSts, BOOL RefzSts );		*/
/****************************************************************************************************/

/****************************************************************************************************/
/*		API																							*/
/****************************************************************************************************/
/*	LONG	RemVibGetRemVibFreq( REMVIBFREQ *RemVibFreq );											*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		残留振動周波数算出初期化処理																*/
/*																									*/
/****************************************************************************************************/
void	RemVibChkCondition( REMVIBFREQ *RemVibFreq, BOOL PosCtrlMode )
{
	if( PosCtrlMode != TRUE )
	{ /* 位置制御モード以外 */
		/* 強制終了(周波数計算なし) */
		RemVibFreq->var.State = RVM_INIT;
	}
	return;
}


/****************************************************************************************************/
/*																									*/
/*		残留振動周波数算出																			*/
/*																									*/
/*		概要: 位置指令払い出し完了後の残留振動周波数を算出する。									*/
/*			　残留振動周波数は、位置指令払い出し完了後のオーバーシュート，							*/
/*			　アンダーシュート量の各々2回のピーク値を求め、オーバーシュート間、						*/
/*			　アンダーシュート間の時間の平均値を求め、周波数変換して残留振動とする。				*/
/*			　周波数計算は，10個のピークまたは，500ms間ピークが無い場合，終了する。					*/
/*			　計算中に位置指令が入力された場合は，計算可能な場合は，周波数計算を実行し，			*/
/*			　計算不可の場合は，初期化処理へ戻る。													*/
/*																									*/
/****************************************************************************************************/
void	RemVibCalculateFreq( REMVIBFREQ *RemVibFreq, LONG CtrlPosErr, BOOL CoinSts, BOOL RefzSts )
{
	LONG	lwk;
	ULONG	ulwk;
	ULONG	Sumtime;
	ULONG	tmpdiff;

	/*----------------------------------------------------------------------------------------------*/
	/*	位置偏差フィルタ処理(位置偏差のノイズカット)												*/
	/*----------------------------------------------------------------------------------------------*/
	lwk = CtrlPosErr;								/* 位置アンプ偏差		[pulse]					*/
	RemVibFreq->var.PosErrFilOut2 = MlibLpfilter1( lwk,
												   RemVibFreq->conf.RemVibFil,
												   RemVibFreq->var.PosErrFilOut2 );

	RemVibFreq->var.PosErrFilOut = MlibLpfilter1( RemVibFreq->var.PosErrFilOut2,
												  RemVibFreq->conf.RemVibFil,
												  RemVibFreq->var.PosErrFilOut );

	switch( RemVibFreq->var.State )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	初期化																						*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_INIT:
		/*------------------------------------------------------------------------------------------*/
		/*	残留振動周波数算出関連データクリア 														*/
		/*------------------------------------------------------------------------------------------*/
		MlibResetLongMemory( &(RemVibFreq->var), sizeof( RemVibFreq->var )>>2 );
		RemVibFreq->var.State = RVM_COIN;
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	位置決め完了待ち																			*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_COIN:
		if( (RefzSts == TRUE) && (CoinSts == TRUE) )
		{ /* 位置指令払い出し完了 && 位置決め完了 */
			RemVibFreq->mon.UnRvibFrequency = 0;				/* Un表示クリア	<S0DA>				*/
			/*--------------------------------------------------------------------------------------*/
			/*	位置偏差方向判別				 													*/
			/*		※モータが停止している場合は，偏差が増加しないのでこのシーケンスに留まる		*/
			/*--------------------------------------------------------------------------------------*/
			if( (RemVibFreq->var.LastPosErr - RemVibFreq->var.PosErrFilOut) < 0 )
			{ /* 偏差が＋方向に増加 */
				RemVibFreq->var.Dir = 1;
				RemVibFreq->var.State = RVM_PEAKCHK_F;
			}
			else if( (RemVibFreq->var.LastPosErr - RemVibFreq->var.PosErrFilOut) > 0 )
			{ /* 偏差が－方向に増加 */
				RemVibFreq->var.Dir = -1;
				RemVibFreq->var.State = RVM_PEAKCHK_F;
			}
			/* オーバーシュートタイマカウンタ初期化 */
			RemVibFreq->var.PeakCnt = 0;
			/* オーバーシュート量保存 */
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			/* ピーク検出タイムアウトのため、最小検出周波数の半分のカウンタ値となる */
			KlibRstLongTimer( &RemVibFreq->var.TimeOutCtr );
			/* 残留振動時間測定用タイマ(ScanC周期)初期化 */
			KlibRstLongTimer( &RemVibFreq->var.PeakCntTimer );
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	ピーク検出(振動周波数検出前)																*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_PEAKCHK_F:
		/*------------------------------------------------------------------------------------------*/
		/*	位置指令判別＆タイムアウト処理															*/
		/*------------------------------------------------------------------------------------------*/
		if( (RefzSts == FALSE)
			|| (KlibGetLongTimerMs(RemVibFreq->var.TimeOutCtr) >= REMVIB_TIMEOUT) )
		{ /* 位置指令払い出し中 || タイムアウト時 */
			RemVibFreq->var.State = RVM_INIT;					/* 強制終了(周波数計算なし)			*/
			break;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	ピーク更新処理																			*/
		/*		オーバーシュート量が同方向に増加している場合にピークのカウンタを更新する			*/
		/*------------------------------------------------------------------------------------------*/
		if( (RemVibFreq->var.Dir * (RemVibFreq->var.PosErrFilOut - RemVibFreq->var.PeakValue)) > 0 )
		{ /* ピーク更新中 */
			/* オーバーシュート量最大値更新 */
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			/* オーバーシュート最大値タイマカウンタ(ScanC周期)更新 */
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		/*------------------------------------------------------------------------------------------*/
		/*	ピーク反転判別																			*/
		/*------------------------------------------------------------------------------------------*/
		if( ((RemVibFreq->var.Dir * (RemVibFreq->var.PeakValue - RemVibFreq->var.PosErrFilOut))
			> RemVibFreq->conf.RemVibDetWidth) )
		{ /* ピーク反転 */
			KlibRstLongTimer( &RemVibFreq->var.TimeOutCtr );	/* タイムアウトカウンタ更新			*/

			/*--------------------------------------------------------------------------------------*/
			/*	ピーク反転4回までは，振動周波数計算しない。											*/
			/*	(実際にはピーク反転3回で計算可能だが，初回の反転はノイズの影響があるため無視する)	*/
			/*--------------------------------------------------------------------------------------*/
			if( RemVibFreq->var.RevCnt >= 3 )
			{ /* 4回目のピーク -> 周波数計算 */
				/* 同方向ピークの差 */
				lwk = (LONG)RemVibFreq->var.PeakCnt - (LONG)RemVibFreq->var.LLastpeakCnt;
				/* 最小検出周波数でクランプ */
				tmpdiff = MlibLimitul( lwk, REMVIB_MINDET, 0 );

				Sumtime = tmpdiff * REMVIB_DENO_OFFSET;
				RemVibFreq->var.MeanDeno =  REMVIB_DENO_OFFSET;

				/* 平均値計算 */
				ulwk = Sumtime / RemVibFreq->var.MeanDeno;
				/* 最小検出周波数でクランプ */
				RemVibFreq->var.FreqCnt = MlibLimitul( ulwk, REMVIB_MINDET, 2 );

				/* 振動周波数検出開始へ */
				RemVibFreq->var.State = RVM_PEAKCHK_L;

			}
			/* ピークデータ更新 */
			RemVibFreq->var.LLastpeakCnt = RemVibFreq->var.LastPeakCnt;
			RemVibFreq->var.LastPeakCnt = RemVibFreq->var.PeakCnt;
			RemVibFreq->var.RevCnt++;
			RemVibFreq->var.Dir *= -1;
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	ピーク検出(振動周波数検出開始)																*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_PEAKCHK_L:
		/*------------------------------------------------------------------------------------------*/
		/*	位置指令判別＆タイムアウト処理															*/
		/*------------------------------------------------------------------------------------------*/
		if( (RefzSts == FALSE)
			|| (KlibGetLongTimerMs( RemVibFreq->var.TimeOutCtr ) >= REMVIB_TIMEOUT) )
		{ /* 位置指令払い出し中 || タイムアウト時 */
			RemVibFreq->var.State = RVM_COMP;					/* 強制終了(周波数計算へ)			*/
			break;
		}
		/*------------------------------------------------------------------------------------------*/
		/*	ピーク更新処理																			*/
		/*		オーバーシュート量が同方向に増加している場合にピークのカウンタを更新する			*/
		/*------------------------------------------------------------------------------------------*/
		if( (RemVibFreq->var.Dir * (RemVibFreq->var.PosErrFilOut - RemVibFreq->var.PeakValue)) > 0 )
		{	/* ピーク更新中 */
			/* オーバーシュート量最大値更新 */
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			/* オーバーシュート最大値タイマカウンタ(ScanC周期)更新 */
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		/*------------------------------------------------------------------------------------------*/
		/*	ピーク反転判別																			*/
		/*------------------------------------------------------------------------------------------*/
		if( ((RemVibFreq->var.Dir * (RemVibFreq->var.PeakValue - RemVibFreq->var.PosErrFilOut))
			> RemVibFreq->conf.RemVibDetWidth) )
		{ /* ピーク反転　*/
			KlibRstLongTimer( &RemVibFreq->var.TimeOutCtr );	/* タイムアウトカウンタ更新			*/

			if (RemVibFreq->var.RevCnt >= 9)
			{	/* ピークが10以上の場合 */
				RemVibFreq->var.State = RVM_COMP;				/* 強制終了(周波数計算へ)			*/
			}
			/* 同方向ピークの差 */
			lwk = (LONG)RemVibFreq->var.PeakCnt - (LONG)RemVibFreq->var.LLastpeakCnt;
			/* 最小検出周波数でｸﾗﾝﾌﾟ */
			tmpdiff = MlibLimitul( lwk, REMVIB_MINDET, 0 );

			/*--------------------------------------------------------------------------------------*/
			/*	平均値計算																			*/
			/*--------------------------------------------------------------------------------------*/
			/*	分子遂次計算																		*/
			/*		SUM = Mean(n-1):前回平均値 * MeanDeno(n-1):前回分母 * r:重み + X(n):今回値		*/
			/*																						*/
			/*		注）最小検出周波数：0.8Hz，重み1.2，オフセット1000倍で10回の遂次計算で			*/
			/*			以下の式の分子は0xB23E7ECになりLONGサイズ以内。								*/
			/*--------------------------------------------------------------------------------------*/
			Sumtime =
				((RemVibFreq->var.FreqCnt * RemVibFreq->var.MeanDeno * REMVIB_WEIGHT_NUME) / REMVIB_WEIGHT_DENO)
				+ (tmpdiff * REMVIB_DENO_OFFSET);

			/*--------------------------------------------------------------------------------------*/
			/*	分母遂次計算																		*/
			/*		SUM = MeanDeno(n-1):前回分母 * r:重み + 1										*/
			/*																						*/
			/*		注）重み1.2，オフセット1000倍で10回の遂次計算でSUMは25959になるので				*/
			/*			65536を超えることはない。													*/
			/*--------------------------------------------------------------------------------------*/
			RemVibFreq->var.MeanDeno =
				((RemVibFreq->var.MeanDeno * REMVIB_WEIGHT_NUME) / REMVIB_WEIGHT_DENO) + REMVIB_DENO_OFFSET;

			/* 平均値計算 */
			ulwk = Sumtime / RemVibFreq->var.MeanDeno;
			/* 最小検出周波数でクランプ*/
			RemVibFreq->var.FreqCnt = MlibLimitul( ulwk, REMVIB_MINDET, 2 );

			/* ピークデータ更新 */
			RemVibFreq->var.LLastpeakCnt = RemVibFreq->var.LastPeakCnt;
			RemVibFreq->var.LastPeakCnt = RemVibFreq->var.PeakCnt;
			RemVibFreq->var.RevCnt++;
			RemVibFreq->var.Dir *= -1;
			RemVibFreq->var.PeakValue = RemVibFreq->var.PosErrFilOut;
			RemVibFreq->var.PeakCnt = ( KlibGetLongTimerMs(RemVibFreq->var.PeakCntTimer) / REMVIB_CYCLEMS );
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	/*	振動周波数計算																				*/
	/*----------------------------------------------------------------------------------------------*/
	case RVM_COMP:
			if( (RemVibFreq->var.FreqCnt <= REMVIB_MINOUT) && (RemVibFreq->var.FreqCnt > 0) )
			{
				/* 周波数計算 [0.1Hz] */
				RemVibFreq->mon.UnRvibFrequency = 10000 /(RemVibFreq->var.FreqCnt * KPI_SCCYCLEMS);	/* <S0DA> */
			}
			else
			{
				/* 最小出力周波数未満は0.0Hzとする */
				RemVibFreq->mon.UnRvibFrequency = 0;	/* <S0DA> */
			}
			if( RefzSts == FALSE )
			{ /* 位置指令払い出し中 */
				/* 初期化処理へ */
				RemVibFreq->var.State = RVM_INIT;
			}
		break;
	default:
		break;
	}

	RemVibFreq->var.LastPosErr = RemVibFreq->var.PosErrFilOut;
	return;
}


/****************************************************************************************************/
/*																									*/
/*		残留振動周波数取得用API																		*/
/*																									*/
/****************************************************************************************************/
LONG	RemVibGetRemVibFreq( REMVIBFREQ *RemVibFreq )
{
	return	RemVibFreq->mon.UnRvibFrequency;		/* <S0DA> */
}

/****************************************************************************************************/
/*																									*/
/*		残留振動周波数クリア処理 																	*/
/*																									*/
/****************************************************************************************************/
void	RemVibRsetRemVibrationFreq( REMVIBFREQ *RemVibFreq )
{
	RemVibFreq->mon.UnRvibFrequency = 0;			/* 残留振動周波数モニタクリア <S0DA>			*/
	RemVibFreq->var.FreqCnt = 0;					/* 残留振動周波数カウンタクリア					*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		残留振動周波数モニタパラメータ計算															*/
/*																									*/
/****************************************************************************************************/
void	RemVibCalculateMonitorPrm( REMVIBFREQ *RemVibFreq, EGEAR *Egear,
    	                           USHORT remdetw, LONG CoinLevel, LONG ScanTime )
{
	LONG		kx, sx;

	sx = 0;
	kx = MlibScalKxgain( CoinLevel, remdetw, 1000, &sx, 0 );
	kx = MlibPcalKxgain( kx, Egear->b, Egear->a, &sx,  -24 );

	/* 最小検出幅でリミット */
	RemVibFreq->conf.RemVibDetWidth = MlibLimitul( kx, 0x1000000, REMVIB_MINDETWIDTH );
	/* 位置偏差のノイズカット用 */
	RemVibFreq->conf.RemVibFil = MlibPcalKf1gain( REMVIB_FILFREQ, ScanTime, 1 );
}



/***************************************** end of file **********************************************/
