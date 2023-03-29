#ifndef __FN_ATYPE_VIB_TUNE_H /* 再読み込み防止用 */
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




/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct AVIBTUNE
{
	struct {
		USHORT	AvibMode;			/* 0x20A0 : A型制振機能調整モード設定							*/
		USHORT	AvibState;			/* 0x20A1 : A型制振機能調整完了									*/
		USHORT	AvibFreq;			/* 0x20A2 : A型制振機能調整周波数								*/
		USHORT	AvibGain;			/* 0x20A3 : A型制振機能調整ゲイン								*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		SHORT	FftStat;			/* 周波数解析ステータス											*/
		SHORT	dummy;				/* for Alignment												*/
		BOOL	FftReq;				/* 周波数解析要求												*/
		BOOL	Match;				/* 表示と動作があっているか？									*/
		LONG	VibDetLvl;			/* 振動検出レベル												*/
		ULONG	TimeOutCtr;			/* タイムアウト検出用カウンタ									*/
	} var;
} AVIBTUNE;



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum AVIB_SEQ_STS {
	AVIB_SEQ_INIT		= 0,		/* 初期処理														*/
	AVIB_SEQ_MODESEL,				/* 自動/手動モード選択											*/
	AVIB_SEQ_TRGWAIT,				/* 振動検出トリガ待ち											*/
	AVIB_SEQ_MEASURE,				/* 周波数解析中													*/
	AVIB_SEQ_COMP,					/* 周波数解析完了(フィルタ設定待ち)								*/
	AVIB_SEQ_SET,					/* フィルタ設定中												*/
	AVIB_SEQ_FREQTUNE,				/* 周波数調整													*/
	AVIB_SEQ_GAINTUNE,				/* ダンピングゲイン調整											*/
	AVIB_SEQ_END,					/* 終了処理														*/
	AVIB_SEQ_NOOPWAIT,				/* No Operation 表示ウェイト									*/
};
/*--------------------------------------------------------------------------------------------------*/
enum AVIB_CMP_STS {
	AVIB_STS_NONCOMP	= 0,		/* 振動検出完了：検出未完了										*/
	AVIB_STS_COMP,					/* 振動検出完了：検出完了										*/
	AVIB_STS_TIMEOUT,				/* 振動検出完了：検出不可(タイムアウト)							*/
	AVIB_STS_RANGEOV,				/* 振動検出完了：検出不可(周波数範囲外)							*/
	AVIB_STS_NONVIB,				/* 振動検出完了：検出不可(周波数特定不可)						*/
};
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_FRQ_MIN			100		/* 検出周波数下限 100Hz									*/
#define		AVIB_FRQ_MAX			1000	/* 検出周波数上限 1000Hz								*/
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_MODE_MIN			0		/* モード下限											*/
#define		AVIB_MODE_MAX			1		/* モード上限											*/
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_GTUN_MAX			2		/* ゲイン調整最大桁										*/
#define		AVIB_FTUN_MAX			4		/* 周波数調整最大桁										*/
#define		AVIB_TUN_MIN			1		/* ゲイン/周波数調整最小桁								*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetAtypeVibTuningMode( AVIBTUNE *AvibTune, USHORT Mode );
PRM_RSLT	RpiFunSetAtypeVibTuningFrequency( AVIBTUNE *AvibTune, USHORT Frq );
PRM_RSLT	RpiFunSetAtypeVibTuningGain( AVIBTUNE *AvibTune, USHORT Gain );
void		RpiRegSetAtypeVibTuningFrequency( AVIBTUNE *AvibTune, USHORT Frq );
void		RpiRegSetAtypeVibTuningGain( AVIBTUNE *AvibTune, USHORT Gain );



/****************************************************************************************************/
#define __FN_ATYPE_VIB_TUNE_H
#endif /* __FN_ATYPE_VIB_TUNE_H */
