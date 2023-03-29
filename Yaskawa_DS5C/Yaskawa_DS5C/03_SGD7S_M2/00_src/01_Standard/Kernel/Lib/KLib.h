#ifndef WAIT_FUNC_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		WainFunc.h : タイマ/ウェイト系処理定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"

#ifdef _TEST_BOARD_
#define FREERUN_TIMER				0
#define TIMER_CLOCK_PER_1US			25
#endif

#define KLIB_EEPDISABLE_TIME		100		/* 100ms */

/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
/* インターバル・タイマレジスタ */
#define ITMR_TMD				(*((volatile unsigned long*)(0xEFFF1000)))	/* タイマ・カウンタ・レジスタ		*/
#define ITMR_CMD				(*((volatile unsigned long*)(0xEFFF1004)))	/* コンペア・レジスタ			*/
#define ITMR_TMCD				(*((volatile unsigned long*)(0xEFFF1008)))	/* コントロール・レジスタ		*/
/* 設定値 */
#define ITMR_CMD_SET			0xFFFFFFFF		/* コンペア・レジスタ設定値(0,1,..,0xFFFFFFFE,0xFFFFFFFF,0,1,..となる設定)	*/
#define ITMR_TMCD_SET			0x00000003		/* コントロール・レジスタ設定値(INCLOCKの2分周,動作許可)						*/
/* nsec計算用 */
#define ITMR_NSEC_PER_COUNT		32UL			/* 1カウントあたりの時間(nsec)	*/
/* usec計算用(設定を一部のみ修正すると、正しく動作しないので注意すること) */
#define ITMR_USEC_MAX			0x020C49BA		/* waitで演算可能な最大時間(usec) 34,359,738 usec	*/
												/* 0x02000000 を固定カウント値でのループ条件値とする		*/
#define ITMR_USEC_SHIFT			25				/* 固定カウント値でのループが必要か判定するためのシフト数	*/
#define ITMR_USEC_MASK			0x01FFFFFF		/* 指定時間からループ回数を取り除くためのマスクパターン		*/
#define ITMR_USEC_FIX_COUNT		0x3E800000		/* 固定カウント値((0x02000000 * 125) / 4)			*/
#define ITMR_USEC_MUL_VAL		125UL			/* 乗数(本来は1,000だが、1,000/8=125)				*/
#define ITMR_USEC_DIV_VAL		4UL				/* 除数(本来は32だが、32/8=4)						*/
/* msec計算用(設定を一部のみ修正すると、正しく動作しないので注意すること) */
#define ITMR_MSEC_MAX			0x000218DE		/* waitで演算可能な最大時間(msec) 137,438 msec	*/
												/* 0x00020000 を固定カウント値でのループ条件値とする		*/
#define ITMR_MSEC_SHIFT			17				/* 固定カウント値でのループが必要か判定するためのシフト数	*/
#define ITMR_MSEC_MASK			0x0001FFFF		/* 指定時間からループ回数を取り除くためのマスクパターン		*/
#define ITMR_MSEC_FIX_COUNT		0xF4240000		/* 固定カウント値(0x00020000 * 31,250)				*/
#define ITMR_MSEC_MUL_VAL		31250UL			/* 乗数(本来は1,000,000 だが、1,000,000/32=31,250)	*/
#define ITMR_MSEC_DIV_VAL		1				/* 除数(本来は32だが、32/32=1)						*/
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

enum	INTERRUPT_NAME {
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE	/* AP以外 */
	INTERRUPT_IntSync = 1,
//	CoE_SRST = 3,		/*<S008>*/
	OP_SWRST = 3,							/* <S106> */
	SIL3_TEST_PLS	= 4,					/* <S0F1> */
	INTERRUPT_ScanA	= 5 ,
	INTERRUPT_ScanB = 6,
	INTERRUPT_ScanC = 7,
#else
//	DebugPort1		= 1,					/* 後で消す */
//	DebugPort2		= 7,					/* 後で消す */
	INTERRUPT_ScanA	= 1,
	OP_SWRST 		= 3,					/* <S106> */
	SIL3_TEST_PLS	= 4,					/* <S0F1> */
	INTERRUPT_ScanB = 7,
	INTERRUPT_IntSync,						/* DebugPortは使用しないこと!!!!!!!!! */
	INTERRUPT_ScanC,						/* DebugPortは使用しないこと!!!!!!!!! */
#endif
};

enum	INTERRUPT_TIMING {
	INT_TIMING_ScanB	= 0x01,
	INT_TIMING_ScanC	= 0x0F,
};


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
void	KlibInitialize( void );								/* Klib初期化	*/
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */

void	KlibWaitns( LONG ns );
void	KlibWaitus( LONG us );
void	KlibWaitms( LONG ms );

void	KlibRstLongCounter( ULONG *timer );			/* 差分カウンタリセット	<S00E>	*/
ULONG	KlibGetLongCounter( ULONG timer );			/* カウンタ計時	<S00E>	*/
void	KlibRstLongTimer( ULONG *timer );			/* タイマーリセット	<S00E>	*/
ULONG	KlibGetLongTimerMs( ULONG timer );			/* タイマ[ms]計時	<S00E>	*/
ULONG	KlibGetLongTimerUs( ULONG timer );			/* タイマ[ms]計時	<S00E>	*/
void	KlibCountLongTimer( ULONG divShift );		/* カウンタ処理	<S00E>	*/

void KlibInitializeTimeStamp( ULONG time );
void KlibCountTimeStamp( ULONG tick_in_msec, BOOL PowerOn );
LONG KlibGetTimeAfterPowerOn( void );
ULONG KlibGetTimeStamp( void );
LONG KlibGetMainPowerOffTimer( void );

ULONG KlibClockToTime(ULONG clock_diff);
ULONG KlibGetClockTime(void);

void InitializeInterruptTiming( void );
UCHAR GetInterruptTiming( UCHAR IntName );

/* ===>> add <TimerAPI> 2013.08.23 K.Sakamoto */
void	ITmr_InitTimer( void );										/* インターバル・タイマ初期化		*/
ULONG	ITmr_GetTimerCount( void );									/* タイマ・カウンタ・レジスタ値取得	*/
ULONG	ITmr_ConvNsec2count( LONG TimeNsec );						/* 時間(nsec)->カウント変換	*/
void	ITmr_WaitExceedTimeUsec( LONG *TimeUsec, ULONG *StTimer );	/* 最大時間(usec)超え分wait	*/
ULONG	ITmr_ConvUsec2count( LONG TimeUsec );						/* 時間(usec)->カウント変換	*/
void	ITmr_WaitExceedTimeMsec( LONG *TimeMsec, ULONG *StTimer );	/* 最大時間(msec)超え分wait	*/
ULONG	ITmr_ConvMsec2count( LONG TimeMsec );						/* 時間(msec)->カウント変換	*/
ULONG	ITmr_ConvCount2msec( ULONG Count );							/* カウント->時間(msec)変換	*/
ULONG	ITmr_ConvCount2usec( ULONG Count );							/* カウント->時間(usec)変換	*/
/* <<=== add <TimerAPI> 2013.08.23 K.Sakamoto */


/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SystemError.h : システムエラー処理定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	KpiSystemError( void );

#define WAIT_FUNC_H
#endif /* WAIT_FUNC_H */
/***************************************** end of file **********************************************/
