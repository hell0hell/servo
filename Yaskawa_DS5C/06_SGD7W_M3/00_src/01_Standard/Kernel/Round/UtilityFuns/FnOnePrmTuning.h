#ifndef __FN_ONE_PRM_TUNE_H /* 再読み込み防止用 */
/****************************************************************************
  Description   : One Parameter Tuning (Fn203)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.10   created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct ONEPRMV
{
	struct {
		USHORT	SpatMode;				/* 0x2050 : 1パラメータチューニングモード					*/
		USHORT	SpatLevel;				/* 0x2051 : 1パラメータチューニングレベル					*/
		USHORT	SpatFFLevel;			/* 0x2052 : 1パラチューニングＦＦレベル						*/
		USHORT	SpatLvlChgState;		/* 0x2053 : 1パラチューニングレベル変更完了					*/
		USHORT	SpatANotchState;		/* 0x2054 : 1パラチューニング自動ノッチ完了					*/
		USHORT	SpatAresState;			/* 0x2055 : 1パラチューニング自動Ａ型制振完了				*/
		USHORT	OnePrmFilType;			/* 0x2095 : 1パラチューニングフィルタタイプ(AATと共通)		*/
		USHORT	Dummy;					/* for Alignment											*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		BOOL	TunRun;					/* チューニング実行中										*/
		BOOL	TunSel;					/* FB/FFチューニング選択									*/
		BOOL	FftWait;				/* 自動ノッチ実行待ち										*/
		BOOL	NotchChk;				/* 自動ノッチ設定後応答チェック								*/
		BOOL	FftReq;					/* 周波数解析要求											*/
		BOOL	ModeUnmatch;			/* チューニングモードアンマッチ								*/
/*--------------------------------------------------------------------------------------------------*/
		ULONG	FftWaitTimer;			/* 自動ノッチ実行タイマ										*/
		ULONG	FfWaitTimer;			/* FF設定待ちタイマ											*/
		ULONG	NotchChkTimer;			/* ノッチチェックタイマ										*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	ModeMin;				/* チューニングモード設定最小値								*/
		UCHAR	ModeMax;				/* チューニングモード設定最大値								*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	DigMax;					/* チューニングレベル移動最大桁								*/
		UCHAR	Dummy1;					/* for Alignment											*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	FfSetting;				/* FFチューニング設定										*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	Jrate;					/* 慣性モーメント/重量比設定								*/
/*--------------------------------------------------------------------------------------------------*/
		USHORT	OrgKv;					/* 変更前のKv												*/
		USHORT	OrgMdlgn;				/* 変更前のKpm												*/
		USHORT	OrgNf1;					/* 変更前の第1ノッチフィルタ周波数							*/
		USHORT	OrgNf2;					/* 変更前の第2ノッチフィルタ周波数							*/
		USHORT	OrgNfQ1;				/* 変更前の第1ノッチフィルタQ値								*/
		USHORT	OrgNfQ2;				/* 変更前の第2ノッチフィルタQ値								*/
		USHORT	OrgAResf;				/* 変更前のＡ型制振周波数									*/
		USHORT	OrgAResDGn;				/* 変更前のＡ型制振ダンピングゲイン							*/
/*--------------------------------------------------------------------------------------------------*/
		SHORT	FftStat;				/* 周波数解析ステータス										*/
		SHORT	Dummy2;					/* for Alignment											*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	ANotchSetStat;			/* 自動ノッチ設定結果状況									*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	ANotchStat;				/* 自動ノッチ設定実行状況									*/
		UCHAR	ANotch1Stat;			/* 自動ノッチ１段目ノッチ設定結果							*/
		UCHAR	ANotch2Stat;			/* 自動ノッチ２段目ノッチ設定結果							*/
		UCHAR	ANotchPreStat;			/* 自動ノッチ直前ノッチ設定									*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	AResSetStat;			/* 自動Ａ型設定結果状況										*/
/*--------------------------------------------------------------------------------------------------*/
		UCHAR	AResStat;				/* 自動Ａ型設定実行状況										*/
		UCHAR	ARes1Stat;				/* 自動Ａ型設定結果											*/
/*--------------------------------------------------------------------------------------------------*/
	} var;
} ONEPRMTUNE;


typedef	struct TUNEGAIN
{
	USHORT	loophz;					/* Pn100:速度ループゲイン										*/
	USHORT	pitime;					/* Pn101:速度ループ積分時定数									*/
	USHORT	posgn;					/* Pn102:位置ループゲイン										*/
	USHORT	trqfil11;				/* Pn401:トルクフィルタ時定数									*/
	USHORT	mdlgn;					/* Pn141:モデルループゲイン										*/
	USHORT	dobgn;					/* Pn121:摩擦補償ゲイン											*/
	USHORT	dlpfil;					/* Pn124:摩擦補償周波数補正										*/
	USHORT	ntchq1;					/* Pn40A:第1ノッチフィルタQ値									*/
	USHORT	ntchq2;					/* Pn40D:第2ノッチフィルタQ値									*/
} TUNEGAIN;


typedef	struct TUNECALV
{
	ULONG		PIRate;
	ULONG		KpZeta;
	ULONG		Ti;
	TUNEGAIN	Gain;				/* チューニングゲイン											*/
} TUNECALV;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum OPTUNE_SEQ_STS {
	OPTUNE_SEQ_INIT				= 0,		/* 初期処理												*/
	OPTUNE_SEQ_JSET,						/* 慣性モーメント比設定処理								*/
	OPTUNE_SEQ_MODESEL,						/* モード設定処理										*/
	OPTUNE_SEQ_FILSEL,						/* フィルタタイプ設定処理								*/
	OPTUNE_SEQ_PREDISP,						/* 調整前ゲイン表示処理									*/
	OPTUNE_SEQ_TUNING,						/* 1パラ調整処理										*/
	OPTUNE_SEQ_VIBSEL,						/* 振動抑制処理											*/
	OPTUNE_SEQ_POSTDISP,					/* 調整後ゲイン表示処理									*/
	OPTUNE_SEQ_NOTCHERR,					/* ノッチ検出エラー処理									*/
	OPTUNE_SEQ_END,							/* 終了処理												*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_FLAG_SEL {
	OPTUNE_FLAG_FBSEL			= 0,		/* フィードバックゲイン選択								*/
	OPTUNE_FLAG_FFSEL,						/* フィードフォワードゲイン選択							*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_ARES_DETVIB_STS {
	OPTUNE_STS_ARES_NOVIBDET	= 0,		/* Ａ型制振完了ステータス：振動未検出					*/
	OPTUNE_STS_ARES_DETECT,					/* Ａ型制振完了ステータス：振動検出中					*/
	OPTUNE_STS_ARES_NOENABLE,				/* Ａ型制振完了ステータス：自動設定不可					*/
	OPTUNE_STS_ARES_DET_ERR,				/* Ａ型制振完了ステータス：検出エラー					*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_ARES_SET_STS {
	OPTUNE_STS_ARES_INIT		= 0,		/* Ａ型制振設定初期										*/
	OPTUNE_STS_ARES_OK,						/* Ａ型制振設定正常完了									*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_ARES_COMP_STS {
	OPTUNE_STS_ARES_NODETECT	= 0,		/* Ａ型制振完了ステータス：未実施						*/
	OPTUNE_STS_ARES_COMP,					/* Ａ型制振完了ステータス：設定完了						*/
	OPTUNE_STS_ARES_NG,						/* Ａ型制振完了ステータス：設定失敗						*/
};
/*--------------------------------------------------------------------------------------------------*/
enum OPTUNE_FF_STS {
	OPTUNE_STS_FFCOMP			= 0,		/* FFレベル変更完了状態：変更完了/イニシャル			*/
	OPTUNE_STS_FFWAIT,						/* FFレベル変更完了状態：変更待ち						*/
	OPTUNE_STS_FFTMOUT,						/* FFレベル変更完了状態：変更失敗(タイムアウト)			*/
};
/*--------------------------------------------------------------------------------------------------*/


#define		OPTUNE_JTUN_MAX		5			/* 慣性モーメント/重量比調整最大桁						*/
#define		OPTUNE_MFC_ACTIVE	0x0001		/* 剛体系設定(Pn140)									*/
#define		OPTUNE_MODE_MIN		0			/* 1パラモード下限値									*/
#define		OPTUNE_MODE_MAX		3			/* 1パラモード上限値									*/
#define		TUNE_MAX_TI			3000		/* Ti上限値												*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
void		OnePrmReSetTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT loophz, USHORT posgn, BOOL IPSpdControl );
PRM_RSLT	RpiFunSetOnePrmTuningMode( ONEPRMTUNE *OnePrmTune, USHORT Mode );
PRM_RSLT	RpiFunSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl );
PRM_RSLT	RpiFunSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl );
void		RpiRegSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain );
void		RpiRegSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain );



/****************************************************************************************************/
#define __FN_ONE_PRM_TUNE_H
#endif /* __FN_ONE_PRM_TUNE_H */
