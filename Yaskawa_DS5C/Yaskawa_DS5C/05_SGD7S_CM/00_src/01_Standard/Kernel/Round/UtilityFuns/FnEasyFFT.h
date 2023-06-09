#ifndef __FN_EASY_FFT_H /* 再読み込み防止用 */
/****************************************************************************
  Description   : A Type Vibration Tuning (Fn204)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.15   created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct EASYFFT {
	struct {
		USHORT	EzFftMode;				/* 0x2060 : EasyFFTモード									*/
		USHORT	EzFftState;				/* 0x2061 : EasyFFT完了確認									*/
		USHORT	EzFftTrq;				/* 0x2062 : EasyFFTトルク振幅								*/
		USHORT	EzFftDetf;				/* 0x2410 : EasyFFT検出周波数								*/
		USHORT	EzFftFil1;				/* 0x2411 : EasyFFTフィルタ周波数１							*/
		USHORT	EzFftFil2;				/* 0x2412 : EasyFFTフィルタ周波数２							*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		BOOL	UseHpFil;				/* ハイパスフィルタ使用										*/
/*--------------------------------------------------------------------------------------------------*/
		LONG	KnormFrq;				/* 周波数単位変換ゲイン										*/
		ULONG	Tolerance;				/* Step毎のバラツキ許容値 = 100[Hz]							*/
		USHORT	UnitMaxFreq;			/* 単位換算用検出最大周波数[Hz]								*/
		USHORT	RetUnitShift;			/* 単位逆変換用シフト数										*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	ReTryMethod;			/* ノッチ検出リトライ方法									*/
		USHORT	ReTryMaxCount;			/* ノッチ検出リトライ数										*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	*TblBufAddr;			/* テーブル運転指令バッファアドレス							*/
		USHORT	*TrcBufAddr;			/* トレースバッファアドレス									*/
		USHORT	Dummy;					/* for Alignment											*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		BOOL	Zoom;					/* ズーム													*/
		BOOL	DrvDir;					/* 運転方向													*/
		BOOL	Drived;					/* 運転実施済												*/
		BOOL	SameDir;				/* 同方向運転												*/
		BOOL	AnalyzeState;			/* 異常検出													*/
		BOOL	SwpDir;					/* 掃引方向 増大(0)/減少(1)									*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	SeqPtr;					/* シーケンスポインタ										*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	NotchSw;				/* ノッチスイッチ											*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	FrqLowerLmt;			/* ノッチ検出・発振検出周波数の下限 単位[Hz]				*/
		USHORT	FrqSwpMin;				/* 掃引周波数の最小値 単位[Hz]								*/
		USHORT	FrqSwpMax;				/* 掃引周波数の最大値 単位[Hz]								*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	OrgFrqLowerLmt;			/* ノッチ検出・発振検出周波数の下限のバックアップ			*/
		USHORT	OrgFrqSwpMin;			/* 掃引周波数の最小値のバックアップ							*/
		USHORT	OrgFrqSwpMax;			/* 掃引周波数の最大値のバックアップ							*/
/*--------------------------------------------------------------------------------------------------*/
		ULONG	DetFrq0;				/* 絶対値の最大値である周波数値(ノッチ周波数）				*/
		USHORT	SpcPeak0;				/* 絶対値の最大値である速度FB/実トルク値（絶対値）			*/
		USHORT	DetState0;				/* 															*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	PosDrvCount;			/* 正方向運転回数											*/
		USHORT	NegDrvCount;			/* 逆方向運転回数											*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	Step;					/* STEP状態													*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	StepReTryCnt;			/* リトライ・カウンター:STEPの是非							*/
		USHORT	AvrReTryCnt;			/* リトライ・カウンター:正逆平均の是非						*/
		USHORT	DetReTryCnt;			/* リトライ・カウンター:検出の是非							*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	DetCount;				/* 検出結果カウンタ											*/
		USHORT	DetState[ 4 ];			/* 検出結果(状態)											*/
		ULONG	DetFrq[ 4 ];			/* 検出結果(周波数)											*/
		ULONG	DetAvrFrq[ 2 ];			/* 検出平均結果(周波数)										*/
/*--------------------------------------------------------------------------------------------------*/
		ULONG	FinalDetFrq;			/* 最終結果周波数											*/
/*--------------------------------------------------------------------------------------------------*/
		LONG	KTrqRef;				/* 掃引トルク演算ゲイン										*/
/*--------------------------------------------------------------------------------------------------*/
		LONG	KHpFilter;				/* ハイパスフィルタゲイン									*/
/*--------------------------------------------------------------------------------------------------*/
		LONG	DeltaFrqRes;			/* 応答処理用・掃引周波数の増分/減分値						*/
		LONG	DeltaFrqRef;			/* 指令作成用・掃引周波数の増分/減分値						*/
	} var;
} EASYFFT;



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum EZFFT_SEQ_STS {
	EZFFT_SEQ_INIT		= 0,		/* 初期処理														*/
	EZFFT_SEQ_TRQSET,				/* 掃引トルク設定												*/
	EZFFT_SEQ_READY,				/* 運転準備														*/
	EZFFT_SEQ_DRIVE,				/* 運転動作中													*/
	EZFFT_SEQ_RESULT,				/* 運転完了(フィルタ設定待ち)									*/
	EZFFT_SEQ_END,					/* 終了処理														*/
};
/*--------------------------------------------------------------------------------------------------*/
enum EZFFT_LSEQ_STS {
	EZFFT_LSEQ_DRV		= 0,		/* 運転中														*/
	EZFFT_LSEQ_WAITSTOP,			/* 運転停止待ち													*/
	EZFFT_LSEQ_DETFRQ,				/* 周波数解析中													*/
	EZFFT_LSEQ_AVRFRQ,				/* 正逆差平均化													*/
	EZFFT_LSEQ_STPEND,				/* 運転動作完了													*/
	EZFFT_LSEQ_AVRSTP,				/* 複数回試行の平均化											*/
	EZFFT_LSEQ_ZOOMREQ,				/* 周波数解析結果周辺での再試行要求								*/
	EZFFT_LSEQ_ERR,					/* 異常終了														*/
	EZFFT_LSEQ_WARN,				/* ワーニング終了												*/
	EZFFT_LSEQ_DONE,				/* 正常終了														*/
};
/*--------------------------------------------------------------------------------------------------*/
enum EZFFT_CMP_STS {
	EZFFT_STS_END		= 0,		/* 完了															*/
	EZFFT_STS_RUN,					/* 未完了														*/
	EZFFT_STS_DETNG,				/* ピーク値検出不能												*/
	EZFFT_STS_ERR,					/* 異常終了														*/
};
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_SIZ_TRACE			1024	/* トレースサイズ										*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_DRV_FORWARD		0		/* テーブル運転方向：＋									*/
#define		EZFFT_DRV_REVERSE		1		/* テーブル運転方向：−									*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_ANASTS_OK			0		/* 周波数解析結果OK										*/
#define		EZFFT_ANASTS_NG			1		/* 周波数解析結果NG										*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_STEP_1			0		/* 周波数解析step1										*/
#define		EZFFT_STEP_2			1		/* 周波数解析step2										*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_ANAOBJ_REF		0		/* 周波数解析対象：指令									*/
#define		EZFFT_ANAOBJ_RES		1		/* 周波数解析対象：応答									*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_FRQ_L2H			0		/* トルク掃印方向：低⇒高								*/
#define		EZFFT_FRQ_H2L			1		/* トルク掃印方向：高⇒低								*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_RETURN_UNIT		125		/* 単位逆変換用定数										*/
#define		EZFFT_DEVLVL_DIR		250		/* 正逆運転時のバラツキ許容値 [0.1%]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		EZFFT_CLOSE_MANUAL		4		/* 位置制御 リトライ回数 0回 1動作のみ(片側運転)		*/
#define		EZFFT_ONESTEP_RETRY_0	5		/* 周波数解析結果周辺での再試行なし、リトライ回数 0回	*/
#define		EZFFT_ONESTEP_RETRY_1	6		/* 周波数解析結果周辺での再試行なし、リトライ回数 1回	*/
#define		EZFFT_OPEN_MANUAL		7		/* トルク制御 リトライ回数 0回 1動作のみ(片側運転)		*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void		EasyFftInitialize( EASYFFT *EasyFFT, USHORT *TableBuf, USHORT *TraceBuf, UNI_PRM_STR *prm_hndl_ptr );
PRM_RSLT	RpiFunSetEasyFftTrqLevel( EASYFFT *EasyFFT, USHORT Lvl );



/****************************************************************************************************/
#define __FN_EASY_FFT_H
#endif /* __FN_EASY_FFT_H */
