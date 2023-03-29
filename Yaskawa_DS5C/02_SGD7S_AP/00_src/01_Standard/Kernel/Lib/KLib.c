/****************************************************************************
Description	: Kernel System Library
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
//#include <INTEGRITY.h>
#include "Basedef.h"
#include "KLib.h"
#ifndef _MERCURY_PLATFORM_	/* 暗号化フォーマット対応 <S146> */
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"
#endif

/* 2012.08.19 Y.Oka Make Mercury Base Software */
#ifdef _VC_DEBUG_
#include <stdio.h>
#include <time.h>
#endif
/* 2012.08.19 Y.Oka Make Mercury Base Software */

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
#include "nbpfapbtm32.h"
#endif

#define KLIB_EEPDISABLE_TIME		100		/* 100ms */

/* globals */
#pragma arm section rwdata = "Globals", zidata = "Globals"
static LONG		StartUpTimer;
static ULONG	Timer100msCounter;
static ULONG	PowerOnTimer;
static LONG		EepDisableTimer;

static ULONG	KlibScanCounter;
static ULONG	KlibTimerValue;

/* ===>> delete <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0
static ASICS	*pSvAsicRegs;
#endif
/* <<=== delete <TimerAPI> 2013.08.23 K.Sakamoto */
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
//static UCHAR	IntScanBWaitCounter;/* 未使用 */
//static UCHAR	IntScanCWaitCounter;/* 未使用 */
static UCHAR 	IntScanBTimingCounter;
static UCHAR 	IntScanCTimingCounter;
#endif

#pragma arm section rwdata, zidata


/****************************************************************************
	Initialize the Klib variables
****************************************************************************/
void KlibInitialize( void )
{
	ITmr_InitTimer();
}

/****************************************************************************
	Initialize the Time Stamp values
****************************************************************************/
void KlibInitializeTimeStamp( ULONG time )
{
	Timer100msCounter = time;
	PowerOnTimer = 0;
	StartUpTimer = 0;
	EepDisableTimer = KLIB_EEPDISABLE_TIME;
}

/****************************************************************************
	Count up the Time Stamp value(Called from cyclic scan task)
****************************************************************************/
void KlibCountTimeStamp( ULONG tick_in_msec, BOOL PowerOn )
{
	/* タイムスタンプカウンタ更新 */
	if(PowerOn != FALSE)		/* Power On時のみ更新 */
	{
		PowerOnTimer += tick_in_msec;
		if(PowerOnTimer >= 100)
		{
			Timer100msCounter++;
			PowerOnTimer = 0;
		}
		EepDisableTimer = 0;
	}
	else
	{
		if(EepDisableTimer < KLIB_EEPDISABLE_TIME)
		{	EepDisableTimer += tick_in_msec;}
	}

	/* メインタイマ(制御電源投入からのタイマ[ms], Max.1193H/49D)*/
	if( StartUpTimer < 0xFFFF0000 )	
	{
		StartUpTimer += tick_in_msec;
	}
}

/****************************************************************************
	Get a time stamp value
****************************************************************************/
ULONG KlibGetTimeStamp( void )
{
	return Timer100msCounter;
}

/****************************************************************************
	Get a power on time after start up 
****************************************************************************/
LONG KlibGetTimeAfterPowerOn( void )
{
	return StartUpTimer;
}

/****************************************************************************
	Get the power off time state 
****************************************************************************/
LONG KlibGetMainPowerOffTimer( void )
{
	return EepDisableTimer;
}


/****************************************************************************
	[ns]単位ウェイト 
****************************************************************************/
void KlibWaitns( LONG ns )
{
/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
	ULONG	StTime;		/* 開始時間	*/
	ULONG	CuTime;		/* 現在時間	*/
	ULONG	Count;		/* カウント値	*/

	/* 開始カウント取得 */
	StTime = ITmr_GetTimerCount();

	/* 正の値の時にウェイトを実行する */
	if( ns > 0 )
	{
		/* 指定時間をカウント値に変換 */
		Count = ITmr_ConvNsec2count( ns );
		/* 時間待ち処理 */
		do
		{
			/* 現在カウント取得 */
			CuTime = ITmr_GetTimerCount();
		} while((CuTime - StTime) < Count);
	}
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

	return;
}

/****************************************************************************
	[μs]単位ウェイト 
****************************************************************************/
void KlibWaitus( LONG us )
{

	ULONG	StTime;		/* 開始時間	*/
	ULONG	CuTime;		/* 現在時間	*/
	ULONG	Count;		/* カウント値	*/
	LONG	TimeUsec;	/* 待ち時間	*/

	/* 開始カウント取得 */
	StTime = ITmr_GetTimerCount();

	/* 正の値の時にウェイトを実行する */
	if( us > 0 )
	{
		/* 待ち時間保存 */
		TimeUsec = us;

		/* 最大時間(usec)超え分待ち処理*/
		ITmr_WaitExceedTimeUsec( &TimeUsec, &StTime );

		/* 指定時間をカウント値に変換 */
		Count = ITmr_ConvUsec2count( TimeUsec );

		/* 時間待ち処理 */
		do
		{
			/* 現在カウント取得 */
			CuTime = ITmr_GetTimerCount();

		} while((CuTime - StTime) < Count);
	}

	return;
}

/****************************************************************************
	[ms]単位ウェイト 
****************************************************************************/
void KlibWaitms( LONG ms )
{

	ULONG	StTime;		/* 開始時間	*/
	ULONG	CuTime;		/* 現在時間	*/
	ULONG	Count;		/* カウント値	*/
	LONG	TimeMsec;	/* 待ち時間	*/

	/* 開始カウント取得 */
	StTime = ITmr_GetTimerCount();

	/* 正の値の時にウェイトを実行する */
	if( ms > 0 )
	{
		/* 待ち時間保存 */
		TimeMsec = ms;

		/* 最大時間(msec)超え分待ち処理*/
		ITmr_WaitExceedTimeMsec( &TimeMsec, &StTime );

		/* 指定時間をカウント値に変換 */
		Count = ITmr_ConvMsec2count( TimeMsec );

		/* 時間待ち処理 */
		do
		{
			/* 現在カウント取得 */
			CuTime = ITmr_GetTimerCount();

		} while((CuTime - StTime) < Count);
	}
}

/****************************************************************************
	ロングタイマカウンタリセット						<S00E>
****************************************************************************/
void KlibRstLongCounter( ULONG *timer )
{
	*timer = KlibTimerValue;
}

/****************************************************************************
	ロングタイマ値の取得 [counter]				<S00E>
****************************************************************************/
ULONG KlibGetLongCounter( ULONG timer )
{
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0	/*<S00B>*//* 処理復活<S043> */
	return (ULONG)(KlibTimerValue - timer);
#else
	ULONG	CuCount;	/* 現在カウント		*/
	ULONG	Dcount;		/* カウント差分		*/

	/* 現在カウント取得 */
	CuCount = ITmr_GetTimerCount();

	/* 差分値計算　*/
	Dcount = CuCount - timer;

	/* カウンタ値を返す */
	return Dcount;
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	ロングタイマリセット
****************************************************************************/
void KlibRstLongTimer( ULONG *timer )
{
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0	/*<S00B>*//*コメント解除<S00E>*/
	*timer = KlibTimerValue;
#else
	*timer = ITmr_GetTimerCount();
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	ロングタイマ値の取得 [ms] 					<S00E>
****************************************************************************/
ULONG KlibGetLongTimerMs( ULONG timer )
{
/* ===>> change <TimerAPI> 2013.08.23 K.Sakamoto */
#if 0	/*<S00B>*//*コメント解除<S00E>*/
	return (ULONG)(KlibTimerValue - timer);
#else
	ULONG	CuCount;	/* 現在カウント		*/
	ULONG	Dcount;		/* カウント差分		*/
	ULONG	TimeMsec;	/* 経過時間(msec)	*/

	/* 現在カウント取得 */
	CuCount = ITmr_GetTimerCount();

	/* 差分値計算　*/
	Dcount = CuCount - timer;

	/* カウント->時間(msec)変換 */
	TimeMsec = ITmr_ConvCount2msec( Dcount );

	/* 時間(msec)を返す */
	return TimeMsec;
#endif
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	ロングタイマ値の取得 [us]						<S00E>
****************************************************************************/
ULONG KlibGetLongTimerUs( ULONG timer )
{
	ULONG	CuCount;	/* 現在カウント		*/
	ULONG	Dcount;		/* カウント差分		*/
	ULONG	TimeUsec;	/* 経過時間(usec)	*/

	/* 現在カウント取得 */
	CuCount = ITmr_GetTimerCount();

	/* 差分値計算　*/
	Dcount = CuCount - timer;

	/* カウント->時間(usec)変換 */
	TimeUsec = ITmr_ConvCount2usec( Dcount );

	/* 時間(usec)を返す */
	return TimeUsec;
/* <<=== change <TimerAPI> 2013.08.23 K.Sakamoto */
}

/****************************************************************************
	ロングタイマ値カウント [ms] 
	ScanB 125usなら、divShift=3, 250usなら、divShift=2
****************************************************************************/
void KlibCountLongTimer( ULONG CountInMS )
{
	KlibScanCounter++;
	if(KlibScanCounter >= CountInMS)
	{
		KlibScanCounter = 0;
		KlibTimerValue ++;
	}

}

/****************************************************************************
	システムクロックタイマ値取得(32bit Clock Countで1秒)
****************************************************************************/
 ULONG KlibGetClockTime(void)
{
//	Time t;
//	GetClockTime( HighResClock, &t);
//	return t.Fraction;
	ULONG t;
#ifdef _VC_DEBUG_ /* 2012.08.19 Y.Oka Make Mercury Base Software */
	t = clock();
#elif defined(_TEST_BOARD_) /* 2012.08.19 Y.Oka Make Mercury Base Software */
	timer_ioctl( 0, TIMER_IOCTL_GET_COUNT, (VOID *)&t);
#else
	t = 0;
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software */
	return t;
}

/****************************************************************************
	システムクロック→[ns]変換
****************************************************************************/
ULONG KlibClockToTime(ULONG clock_diff)
{
//	float time_in_ns;
//	time_in_ns = (float)clock_diff * (float)(1000000000.0/4294967296.0);
//	return (ULONG)time_in_ns; /* [ns] */
	ULONG time_in_ns;
#ifdef _VC_DEBUG_ /* 2012.08.19 Y.Oka Make Mercury Base Software */
	time_in_ns = clock_diff * 1000000;	/* clock_diff[ms] */
#else /* _VC_DEBUG_ */ /* 2012.08.19 Y.Oka Make Mercury Base Software */
//	time_in_ns = clock_diff * 20;	/* 1000/50MHz */
	time_in_ns = clock_diff * 32;	/* 1000/31.25MHz　SoCのタイマに合わせる　<S043> */
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software */
	return (ULONG)time_in_ns; /* [ns] */
}


/****************************************************************************
	ScanB, ScanC 割り込みタイミング初期化
****************************************************************************/
void InitializeInterruptTiming( void )
{
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
//	IntScanBWaitCounter = 0;/* 未使用 */
//	IntScanCWaitCounter = 0;/* 未使用 */
	IntScanBTimingCounter = INT_TIMING_ScanB;
	IntScanCTimingCounter = INT_TIMING_ScanC;
#endif
	return;
}


/****************************************************************************
	割り込みタイミング取得		※ScanA, ScanBからCall
							※戻り値 ： TRUE：割り込みタイミング
****************************************************************************/
UCHAR GetInterruptTiming( UCHAR IntName )
{
	UCHAR	ret;
	ret = FALSE;
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)

	if( IntName == INTERRUPT_ScanB )
	{
		if( (IntScanBTimingCounter & INT_TIMING_ScanB) == INT_TIMING_ScanB )
		{
			ret = TRUE;
		}
		IntScanBTimingCounter++;
	}
	else if( IntName == INTERRUPT_ScanC )
	{
		if( (IntScanCTimingCounter & INT_TIMING_ScanC) == INT_TIMING_ScanC )
		{
			ret = TRUE;
		}
		IntScanCTimingCounter++;
	}
	else
	{
		/* DO NOTHING */
	}
#endif
	return ret;
}



/****************************************************************************************************/
/*																									*/
/*		致命的な異常時のエラー処理																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		機能概要 : 割込み禁止にしてベースブロック状態でプログラムを停止する。						*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	KpiSystemError( void )
{

/*--------------------------------------------------------------------------------------------------*/
/*		プログラム停止																				*/
/*--------------------------------------------------------------------------------------------------*/
	for( ; ; )											/* プログラム停止無限ループ					*/
	{													/*											*/
		;												/*											*/
	}													/*											*/
/*--------------------------------------------------------------------------------------------------*/

}

/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
/****************************************************************************
	インターバル・タイマ初期化
****************************************************************************/
void	ITmr_InitTimer( void )
{
	/* コンペア・レジスタ設定 */
	ITMR_CMD = ITMR_CMD_SET;

	/* コントロール・レジスタ設定 */
	ITMR_TMCD = ITMR_TMCD_SET;
}

/****************************************************************************
	タイマ・カウンタ・レジスタ値取得
****************************************************************************/
ULONG	ITmr_GetTimerCount( void )
{
	ULONG	Count;		/* カウント値	*/

	/* タイマ・カウンタ・レジスタ値取得 */
	Count = ITMR_TMD;

	return Count;
}

/****************************************************************************
	時間(nsec)->カウント変換
****************************************************************************/
ULONG	ITmr_ConvNsec2count( LONG TimeNsec )
{
	ULONG	Count;		/* カウント値	*/

	/* カウント値計算 */
	Count = (ULONG)TimeNsec / ITMR_NSEC_PER_COUNT;

	return Count;
}

/****************************************************************************
	最大時間(usec)超え分wait
****************************************************************************/
void	ITmr_WaitExceedTimeUsec( LONG *TimeUsec, ULONG *StTimer )
{
	ULONG	CuTimer;	/* 現在カウント値	*/
	ULONG	Dcount;		/* カウント差分	*/
	ULONG	LoopMax;	/* ループ回数		*/

	/* ループ回数取得 */
	LoopMax = (ULONG)(*TimeUsec) >> ITMR_USEC_SHIFT;

	/* 残時間を設定 */
	*TimeUsec = *TimeUsec & ITMR_USEC_MASK;

	/* ループ実行 */
	for( ; LoopMax > 0; LoopMax-- )
	{
		/* ウェイト処理 */
		do
		{
			/* 現在カウント値取得 */
			CuTimer = ITmr_GetTimerCount();

			/* カウント差分計算 */
			Dcount = CuTimer - *StTimer;
		} while( Dcount < ITMR_USEC_FIX_COUNT );

		/* 開始カウント更新 */
		*StTimer = *StTimer + ITMR_USEC_FIX_COUNT;
	}
}

/****************************************************************************
	時間(usec)->カウント変換
****************************************************************************/
ULONG	ITmr_ConvUsec2count( LONG TimeUsec )
{
	LONG	WaitTime;	/* 指定時間	*/
	ULONG	Count;		/* カウント値	*/

	/* 変数初期化 */
	WaitTime = TimeUsec;

	/* 最大値を超えているかチェック */
	if( WaitTime > ITMR_USEC_MAX )
	{
		WaitTime = ITMR_USEC_MAX;	/* 強制的に最大値		*/
	}

	/* カウント値計算 */
	Count = ((ULONG)WaitTime * ITMR_USEC_MUL_VAL) / ITMR_USEC_DIV_VAL;

	return Count;
}

/****************************************************************************
	最大時間(msec)超え分wait
****************************************************************************/
void	ITmr_WaitExceedTimeMsec( LONG *TimeMsec, ULONG *StTimer )
{
	ULONG	CuTimer;	/* 現在カウント値	*/
	ULONG	Dcount;		/* カウント差分	*/
	ULONG	LoopMax;	/* ループ回数		*/

	/* ループ回数取得 */
	LoopMax = (ULONG)(*TimeMsec) >> ITMR_MSEC_SHIFT;

	/* 残時間を設定 */
	*TimeMsec = *TimeMsec & ITMR_MSEC_MASK;

	/* ループ実行 */
	for( ; LoopMax > 0; LoopMax-- )
	{
		/* ウェイト処理 */
		do
		{
			/* 現在カウント値取得 */
			CuTimer = ITmr_GetTimerCount();

			/* カウント差分計算 */
			Dcount = CuTimer - *StTimer;
		} while( Dcount < ITMR_MSEC_FIX_COUNT );

		/* 開始カウント更新 */
		*StTimer = *StTimer + ITMR_MSEC_FIX_COUNT;
	}
}

/****************************************************************************
	時間(msec)->カウント変換
****************************************************************************/
ULONG	ITmr_ConvMsec2count( LONG TimeMsec )
{
	LONG	WaitTime;	/* 指定時間	*/
	ULONG	Count;		/* カウント値	*/

	/* 変数初期化 */
	WaitTime = TimeMsec;

	/* 最大値を超えているかチェック */
	if( WaitTime > ITMR_MSEC_MAX )
	{
		WaitTime = ITMR_MSEC_MAX;	/* 強制的に最大値		*/
	}

	/* カウント値計算 */
	Count = (ULONG)WaitTime * ITMR_MSEC_MUL_VAL;

	return Count;
}

/****************************************************************************
	カウント->時間(msec)変換
****************************************************************************/
ULONG	ITmr_ConvCount2msec( ULONG Count )
{
	ULONG	TimeMsec;	/* 経過時間(msec)	*/

	/* 経過時間計算 */
	TimeMsec = Count / ITMR_MSEC_MUL_VAL;

	return TimeMsec;
}
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

/****************************************************************************
	カウント->時間(usec)変換
****************************************************************************/
ULONG	ITmr_ConvCount2usec( ULONG Count )
{
	ULONG	TimeUsec;	/* 経過時間(usec)	*/

	/* 経過時間計算 */
	TimeUsec = Count / ITMR_USEC_MUL_VAL;

	return TimeUsec;
}


