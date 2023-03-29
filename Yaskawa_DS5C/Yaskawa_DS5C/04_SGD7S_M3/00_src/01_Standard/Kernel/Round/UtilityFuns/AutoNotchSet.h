#ifndef __AUTO_NOTCH_SET_H 
#define __AUTO_NOTCH_SET_H
/****************************************************************************
Description	: Auto Notch Filter Parameter Setting Function
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.11.17    created

*****************************************************************************/
#include "Basedef.h"
#include "TuneLib.h"
#include "ResVibCtrl.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define		ANOTCH_MIN_SET_FREQ			400		/* 自動ノッチ最小設定周波数	[Hz]					*/
#define		ANOTCH_MIN_SET_FREQ2		200		/* 強制検出時自動ノッチ最小設定周波数	[Hz]		*/
#define		ANOTCH_NOTCH_SET_CHK_TIME	3000	/* ノッチ設定後待ち時間		[ms]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		NOTCH_COMP_MAX_FREQ			800		/* ノッチ周波数補正最大周波数	[Hz]				*/
#define		NOTCH_COMP_RATIO			170		/* ノッチ周波数補正割合			[%]					*/
/*--------------------------------------------------------------------------------------------------*/
#define		NOTCH_BASIC_VIB_LEVEL		600		/* 振動検出レベル(回転)			[0.1min-1]			*/
#define		NOTCH_BASIC_VIB_LEVEL_L		200		/* 振動検出レベル(ﾘﾆｱ)			[0.1mm/s]			*/
#define		NOTCH_FILTYPE				2		/* 自動ノッチ中のフィルタタイプ タイプ2固定			*/
#define		NOTCH_FILTYPE2				3		/* 2000Hz以上の自動ノッチ中フィルタタイプ タイプ3	*/
#define		FREQDETUNIT					20		/* 周波数解析検出周波数分解能						*/
#define		ANOTCH_FRAT_106( data )		(((data)*17)>>4)	/* 106.25%								*/




/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		自動ノッチシーケンス用 定数・変数データ定義													*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ANOTCHSEQ {
	struct {								/* 自動ノッチシーケンス用レジスタ定義					*/
		USHORT		SpatANotchState;			/* 0x2054 : 自動ノッチステータス					*/
	} OpeReg;
	struct {								/* 自動ノッチシーケンス用定数定義						*/
		USHORT		kv;							/* 自動ノッチ計算用速度ループゲイン		[0.1Hz]		*/
		USHORT		jrate;						/* 自動ノッチ計算用負荷慣性モーメント/質量比[%]		*/
		BOOL		EepromWriteProtect;			/* EEPROM書き込み禁止フラグ							*/
		BOOL		BaseEnableSts;				/* サーボON/OFF状態フラグ							*/
		BOOL		TuneLessSts;				/* 調整レス有効・無効フラグ							*/
		BOOL		OverTravelSts;				/* P-OT/N-OT状態フラグ								*/
		LONG		VibCheckLevelForTunLess ;	/* 自動ノッチ用振動検出レベル			[2^24/OS]	*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 自動ノッチシーケンス用変数定義						*/
		BOOL		FftReq;						/* 周波数解析要求									*/
		BOOL		NotchChk;					/* ノッチチェック中									*/
		ULONG		NotchChkTimer;				/* ノッチチェックタイマ(振動検出待ち)				*/
		SHORT		FftStat;					/* 周波数解析ステータス								*/
		SHORT		ANotchSetStat;				/* 自動ノッチ設定結果状況							*/
		UCHAR		ANotchStat;					/* 自動ノッチ設定実行状況							*/
		UCHAR		ANotch1Stat;				/* 自動ノッチ１段目ノッチ設定結果					*/
		UCHAR		ANotch2Stat;				/* 自動ノッチ２段目ノッチ設定結果					*/
		UCHAR		ANotchPreStat;				/* 自動ノッチ直前ノッチ設定							*/
	} var;
} ANOTCHSEQ;



/*--------------------------------------------------------------------------------------------------*/
/*		自動ノッチシーケンスコール用 ポインタ構造体定義												*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ANOTCHSEQCTRL {
	ANOTCHSEQ			*ANotchSeq;			/* 自動ノッチシーケンス用構造体							*/
	AUTONOTCH			*ANotch;			/* 自動ノッチ設定用構造体								*/
	FFTANALYZE			*FftAna;			/* 振動周波数解析(Online FFT)用構造体					*/
	DETVIB				*DetVib;			/* 振動検出用構造体										*/
	PRMDATA				*Prm;				/* パラメータデータ構造体								*/
	REGIF_CONFIG_T		*RegCfg;			/* レジスタIFコンフィグレーション用構造体				*/
} ANOTCHSEQCTRL;



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchFilterSetting( ANOTCHSEQCTRL *ANotchCtrl );
void	AutoNotchInitialize( ANOTCHSEQ *ANotchSeq );
void	AutoNotchCalculatePrm( ANOTCHSEQ *ANotchSeq, USHORT kv, USHORT jrate);
void	AutoNotchCalculateInitPrm( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, LONG AmpType, UNI_PRM_STR *UniPrm );
void	RpiResetStatusANotch1( ANOTCHSEQ *ANotchSeq );
void	RpiResetStatusANotch2( ANOTCHSEQ *ANotchSeq );
UCHAR	RpiGetANotchPreStat( ANOTCHSEQ *ANotchSeq );
USHORT	RpiGetANotchSetStat( ANOTCHSEQ *ANotchSeq );
void	RpiSetRegSpatANotchState( ANOTCHSEQ *ANotchSeq );
void	RpiANotchFftReqSet( ANOTCHSEQ *ANotchSeq, FFTANALYZE *FftAna );
void	RpiANotchTrqFilCalc( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );
void	RpiANotchDisableReqSet( ANOTCHSEQ *ANotchSeq );
void	AutoNotchSetCondition( ANOTCHSEQ *ANotchSeq, BOOL EepromWriteProtect, 
										BOOL BaseEnableSts, BOOL TuneLessSts, BOOL OverTravelSts );


#endif //__AUTO_NOTCH_SET_H 
/***************************************** end of file **********************************************/
