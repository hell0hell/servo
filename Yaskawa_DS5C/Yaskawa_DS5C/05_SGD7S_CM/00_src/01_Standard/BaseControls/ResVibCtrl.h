#ifndef	_RES_VIB_CTRL_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ResVibCtrl.h : 振動検出・制振制御用ヘッダーファイル											*/
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
/*	Note	:	初版	2010.11.11	Y.Oka	for INGRAM												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "JL086ServoIpReg.h"


/****************************************************************************************************/
/*																									*/
/*		定数定義																					*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		for AType Restrain Vibration																*/
/*--------------------------------------------------------------------------------------------------*/
//#define	ARESCYCLE			KPI_SBCYCLENS		/* A型制振制御演算サイクルタイム(ScanB) [ns]		*/
//#define	C10POW7				10000000			/* 10^7												*/
//#define	C10POW9				1000000000			/* 10^9												*/
//#define	C2PAIE7				62831853			/* (2*PAI) * 10^7									*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Detect Vibration																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	DETECT_VIB_NOT_DETECT	0x0000			/* 振動検出ワーニング出力							*/
#define	DETECT_VIB_WARNING		0x0001			/* 振動検出ワーニング出力							*/
#define	DETECT_VIB_ALARM		0x0010			/* 振動検出アラーム出力								*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Detect Vibration Trace																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	DETVIBTRC_BUFMAX		200				/* 最大トレースバッファ個数							*/
#define DETVIBTRC_NOCMD			0x00			/* トレース指令無し									*/
#define	DETVIBTRC_TRCEXE		0x10			/* トレース実行指令									*/
#define	DETVIBTRC_TRCEND		0x1F			/* トレース実行終了応答								*/

/*--------------------------------------------------------------------------------------------------*/
/*		for Vibration Detect Observer																*/
/*--------------------------------------------------------------------------------------------------*/
//#define	DETVIBCYCLENS	KPI_SBCYCLENS	/* 振動検出演算サイクルタイム(ScanB)[ns]					*/
//#define	DETVIBCYCLEUS	KPI_SBCYCLEUS	/* 振動検出演算サイクルタイム(ScanB)[us]					*/
#define	VIBOBSGN		800				/* 振動検出用オブザーバゲイン(80Hz)							*/
#define	DETVIBLP		3183			/* ワンパラ振動検出用ローパスフィルタ(3183us = 50Hz)		*/
#define	DETVIBHP		320				/* 発振検出用ハイパスフィルタ：0.32[ms]=500[Hz]				*/


/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		振動検出用速度オブザーバ変数定義															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DETVIBOBS {					/* 振動検出用速度オブザーバパラメータ＆変数定義			*/
	struct	{								/* 振動検出用速度オブザーバパラメータ定義				*/
		KSGAIN	Ks;								/* 振動検出用オブザーバゲイン						*/
		LONG	Kj;								/* 振動検出用オブザーバイナーシャゲイン				*/
		LONG	Klpf;							/* 振動検出用ローパスフィルタゲイン					*/
		LONG	Khpf;							/* 振動検出用ハイパスフィルタゲイン					*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {								/* 振動検出用速度オブザーバ変数データ定義				*/
		LONG	Ivar64[2];						/* 積分演算用										*/
		LONG	AccSum;							/* 加速度積算値(速度推定値)							*/
		LONG	SpdObs;							/* 速度推定値										*/
		LONG	AvSpdFbk;						/* 2回平均速度										*/
		LONG	LastSpdErr;						/* 速度偏差前回値									*/
		LONG	AvSpdErr;						/* 2回平均速度偏差									*/
		LONG	AvSpdErrbfFil;					/* フィルタ処理前速度偏差							*/
		LONG	AvSpdErrLpfOut;					/* 2回平均速度偏差(ローパスフィルタ出力)			*/
		LONG	Hpftmp;							/* ハイパスフィルタ演算用							*/
		/*------------------------------------------------------------------------------------------*/
		LONG	TrcObsAvSpdErr;					/* Trace  : 2回平均速度偏差(自動ノッチ用トレース)	*/
		BOOL	SpdErrClrReq;					/* Alarm  : 振動検出用差速度初期化要求フラグ		*/
		LONG	MaxSpdErr;						/* Alarm  : 振動検出用差速度最大値	[2^24/OvrSpd]	*/
		LONG	MinSpdErr;						/* Alarm  : 振動検出用差速度最小値	[2^24/OvrSpd]	*/
		BOOL	SpdErrFnClrReq;					/* Fn用   : 振動検出用差速度初期化要求フラグ		*/
		LONG	MaxSpdErrFn;					/* Fn用   : 振動検出用差速度最大値	[2^24/OvrSpd]	*/
		LONG	MinSpdErrFn;					/* Fn用   : 振動検出用差速度最小値	[2^24/OvrSpd]	*/
		LONG	UnMaxSpdErr;					/* Un10C  : 振動検出用差速度最大値	[2^24/OvrSpd]	*/
		LONG	UnMinSpdErr;					/* Un10C  : 振動検出用差速度最小値	[2^24/OvrSpd]	*/
		/*------------------------------------------------------------------------------------------*/
	} var;
} DETVIBOBS;


/*--------------------------------------------------------------------------------------------------*/
/*		速度オブザーバによる速度偏差トレース用データ定義											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DETVIBTRC {					/* 速度オブザーバによる速度偏差トレース用データ定義		*/
		UCHAR	TrcCmd;							/* トレース実行指令(指令／応答)						*/
		UCHAR	TrcInt;							/* トレース間隔指定									*/
		USHORT	TrcNum;							/* トレース回数指定									*/
//		LONG	*TrcVarA;						/* トレース変数指定A [2^24/正規化値]				*/
	/*----------------------------------------------------------------------------------------------*/
		USHORT	TrcSamp;						/* トレース間隔制御用								*/
		USHORT	TrcIndx;						/* トレースデータインデクス							*/
		SHORT	TrcBufA[DETVIBTRC_BUFMAX];		/* トレースデータバッファA [16384/正規化値]			*/
		SHORT	TrcTrgLvl;						/* トレーストリガレベル								*/
} DETVIBTRC;


/*--------------------------------------------------------------------------------------------------*/
/*		発振監視用データ定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DETVIB {
	DETVIBTRC	DetVibTrc;						/* 速度オブザーバによる速度偏差トレース用変数		*/
	DETVIBOBS	DetVibObs;						/* 振動検出用速度オブザーバ変数						*/
	struct	{									/* 発振監視用パラメータ定義							*/
		LONG	VibCheckLevel;						/* 振動検出レベル [2^24/OvrSpd]					*/
		LONG	JstVibCheckLevel;					/* イナーシャ同定時振動検出レベル [2^27/OvrSpd]	*/
		LONG	VibCheckSel;						/* 振動検出選択									*/
	} conf;
	struct	{									/* 発振監視用変数定義								*/
		BOOL	VibDetFlg;							/* 振動検出フラグ								*/
		BOOL	LowFreqVibDetFlg;					/* 低周波振動検出フラグ							*/
		BOOL	AatAlmCntFlg;						/* AATアラーム検出用経過時間カウントフラグ		*/
		ULONG	AatAlmCnt;							/* AATアラーム検出用経過時間カウント			*/
		ULONG	LowFreqVibAlmCnt;					/* 低周波振動アラーム検出用経過時間カウント		*/
		LONG	SumSpdErr;							/* 低周波振動検出用差速度積算値					*/
	} var;
} DETVIB;


/*--------------------------------------------------------------------------------------------------*/
/*		制振制御演算用定数定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct RESVIBP {						/* 制振制御用パラメータ定義							*/
	LONG	Ks;										/* 制振オブザーバゲイン							*/
	LONG	Kj;										/* 制振オブザーバイナーシャゲイン				*/
	LONG	Knd;									/* 制振速度ダンピングゲイン						*/
	LONG	Klpf;									/* 制振ローパスフィルタゲイン					*/
	LONG	Khpf;									/* 制振ハイパスフィルタゲイン					*/
	LONG	Khpf1;								    /* <S03E> 3慣性制振制御ハイパスフィルタゲイン			*/
	LONG	Knd1;									/* <S03E> 3慣性制振制御ダンピングゲイン2				*/
} RESVIB_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		制振制御演算用変数定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct RESVIB {							/* 制振制御用パラメータ＆変数定義					*/
		RESVIB_PRM	ResVibPrm;						/* 制振制御用パラメータ							*/
		BOOL		AresVibSetting;					/* A型制振制御設定								*/
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* 制振制御用変数データ定義							*/
		LONG		Ivar64[2];						/* 積分演算用									*/
		LONG		SpdObs;							/* 速度推定値									*/
		LONG		Hpftmp;							/* ハイパスフィルタ演算用						*/
		LONG		dSpdLpfo;						/* ローパスフィルタ出力速度振動成分				*/
		LONG		dSpdCmp;						/* 制振補償値		[2^24/OvrSpd]				*/
		LONG		MonResVib;						/* 制振モニタ信号	[2^24/OvrSpd]				*/
		LONG	    Hpftmp1;						/* <S03E> 3慣性制振制御ハイパスフィルタ演算用			*/
	} var;
} RESVIB;

/*--------------------------------------------------------------------------------------------------*/
/*		制振制御入力パラメータ	for ResVibCalculatePrm()											*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct RESVIB_CFG_PRM {
	USHORT	jrate;								/* Pn103 : 慣性モーメント比							*/
	USHORT	avibfrq;							/* Pn161 : Ａ型制振周波数							*/
	USHORT	avibgn;								/* Pn162 : Ａ型制振ゲイン補正						*/
	USHORT	avibdamp;							/* Pn163 : Ａ型制振ダンピングゲイン					*/
	SHORT	avibfil1;							/* Pn164 : Ａ型制振フィルタ時定数１補正				*/
	SHORT	avibfil2;							/* Pn165 : Ａ型制振フィルタ時定数２補正				*/
	USHORT	avibdamp2;							/* <S03E> 　 Pn166 : 　3 慣性制振ダンピングゲイン			*/
	USHORT	avibfrq2;							/* <S03E> 　Pn167 :　3 慣性制振周波数				*/
} RESVIB_CFG_PRM;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	ResVibATypeResVibInitialize( RESVIB *ResVib );
LONG	ResVibATypeResVibCtrl( RESVIB *ResVib, LONG MotSpd, LONG TrqRef );

BOOL	ResVibCalculatePrmSW( RESVIB *ResVib, ULONG avibsw, BOOL TuningLess, ASICS *SvAsicRegs );
void	ResVibCalculatePrm( RESVIB *ResVib, RESVIB_CFG_PRM *Prm, KSGAIN Kvx, LONG ScanTimeNs, ASICS *SvAsicRegs );

void	DetVibObsInitialize( DETVIB *DetVib );
void	DetVibSpeedObserver( DETVIB *DetVib, LONG SpdFbki, LONG TrqRefMon );
void	DetVibSetSpeedError( DETVIB *DetVib, BOOL TuningLess, BOOL TuningRun );
LONG	DetVibGetSpdErrPeak( DETVIB *DetVib );
LONG	DetVibGetVibCheckLevel( DETVIB *DetVib );
void	DetVibResetMaxMinSpeedError( DETVIB *DetVib );
void	DetVibResetMaxMinSpeedErrorFn( DETVIB *DetVib );
LONG	DetVibGetSpdErrPeakFn( DETVIB *DetVib );

void	DetVibInitialize( DETVIB *DetVib );
LONG	DetVibDetectVibration( DETVIB *DetVib );
void	DetVibResetRequest( DETVIB *DetVib );
LONG	DetVibDetectLowFreqVibration( DETVIB *DetVib );
void	DetVibLowFreqResetRequest( DETVIB *DetVib );

void	DetVibTraceInitialize( DETVIB *DetVib );
void	DetVibTraceRuntimeService( DETVIB *DetVib );
UCHAR	DetVibTraceGetStatus( DETVIB *DetVib );
SHORT*	DetVibTraceGetDataAdr( DETVIB *DetVib );
USHORT	DetVibTraceGetIndex( DETVIB *DetVib );
void	DetVibTraceStartReq( DETVIB *DetVib );
void	DetVibTraceStopReq( DETVIB *DetVib );
void	DetVibTraceSetCondition( DETVIB *DetVib, USHORT TraceNum, UCHAR TraceInt, SHORT TrigerLvl );

void	DetVibObsCalculateInitPrm( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs );
void	DetVibObsCalculateGains( DETVIBOBS *DetVibObs, LONG ObsGain, LONG Lpf, LONG Hpf, LONG ScanTimeNs );
void	DetVibObsCalculateJGain( DETVIBOBS *DetVibObs, KSGAIN Kvx, ULONG jrate, LONG ScanTimeNs );
void	DetVibObsCalculateInitPrmAAT( DETVIBOBS *DetVibObs, BOOL TuningLess, LONG ScanTimeNs );


#define _RES_VIB_CTRL_H /* 再読み込み防止用 */
#endif /* _RES_VIB_CTRL_H */
/***************************************** end of file **********************************************/
