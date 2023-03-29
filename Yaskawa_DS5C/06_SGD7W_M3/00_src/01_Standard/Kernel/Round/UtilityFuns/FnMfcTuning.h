#ifndef __FN_MFC_TUNE_H /* 再読み込み防止用 */
/****************************************************************************
  Description   : Mfc with Vibration Suppression Tuning (Fn205)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.25   created

****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"



/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
typedef	struct MFCTUNE {
	struct {
		USHORT	MdlVibFreq;					/* 0x20B0 : 振動抑制付きMFC調整周波数					*/
		USHORT	MdlVibState;				/* 0x20B1 : 振動抑制付きMFC調整完了						*/
		USHORT	MdlVibFreq2;				/* 0x20B2 : 振動抑制機能チューニング周波数2				*/
		USHORT	MdlVibFil2;					/* 0x20B3 : 振動抑制機能チューニング周波数2補正			*/
		USHORT	MdlVibState2;				/* 0x20B4 : 振動抑制機能設定2完了確認					*/
		USHORT	Dummy;						/* For Alignment										*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {
		BOOL	DispMatch;					/* 表示と動作があっているか？							*/
		ULONG	FfWaitCntr;					/* 待ち時間経過カウンタ									*/
		USHORT	DetFreq;					/* 検出周波数											*/
		USHORT	OrgMdlgn;					/* 変更前のKpm											*/
		USHORT	OrgAntFrq;					/* 変更前のfa											*/
		USHORT	OrgResFrq;					/* 変更前のfr											*/
		USHORT	OrgMdlSw;					/* 変更前のスイッチ										*/
		USHORT	Dummy;						/* For Alignment										*/
	} var;
} MFCTUNE;



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
enum VIBMFC_SEQ_STS {
	VIBMFC_SEQ_INIT		= 0,				/* 初期処理												*/
	VIBMFC_SEQ_MEASURE,						/* 振動検出中											*/
	VIBMFC_SEQ_ADJUST,						/* 振動周波数微調整										*/
	VIBMFC_SEQ_SETREQ,						/* 周波数(振動抑制付きモデル追従制御)設定				*/
	VIBMFC_SEQ_WAITMFC,						/* 周波数設定有効待ち(モデル追従制御は既に有効)			*/
	VIBMFC_SEQ_WAITNORM,					/* 振動抑制付きモデル追従制御設定有効待ち				*/
	VIBMFC_SEQ_SETCOMP,						/* 設定完了												*/
	VIBMFC_SEQ_END,							/* 終了処理												*/
	VIBMFC_SEQ_NOOPWAIT,					/* No Operation 表示ウェイト							*/
};
/*--------------------------------------------------------------------------------------------------*/
enum VIBMFC_CMP_STS {
	VIBMFC_STS_COMP		= 0,				/* 完了ステータス：設定完了/イニシャル					*/
	VIBMFC_STS_WAIT,						/* 完了ステータス：設定待ち								*/
	VIBMFC_STS_TMOUT,						/* 完了ステータス：設定失敗(タイムアウト)				*/
};
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_TUN_MAX			3		/* ゲイン調整最大桁										*/
#define		VIBMFC_TUN_MIN			1		/* ゲイン/周波数調整最小桁								*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_DET_FRQ_MIN		10		/* Fn023内の残留振動検出周波数下限 1.0Hz				*/
#define		VIBMFC_DET_FRQ_MAX		1000	/* Fn023内の残留振動検出周波数上限 100.0Hz				*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_ONE_MASS			0x01	/* 剛体系モデル											*/
#define		VIBMFC_TWO_MASS1		0x02	/* ２慣性系系モデル１									*/
#define		VIBMFC_TWO_MASS2		0x03	/* ２慣性系系モデル２									*/
#define		VIBMFC_ONE_BASE			0x04	/* 剛体系＋機台モデル									*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_PRM_ACTIVE		0x0011	/* 剛体系＋機台モデル設定(Pn140)						*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBMFC_FF_WAITTMOUT		8000	/* FFレベル変更タイムアウト：8[sec]						*/



/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT	RpiMfcTuningSetFreq( MFCTUNE *MfcTune, USHORT Frq );
PRM_RSLT	RpiMfcTuningSetFreq2( MFCTUNE *MfcTune, USHORT Frq );
PRM_RSLT	RpiMfcTuningSetFreq2Comp( MFCTUNE *MfcTune, USHORT Fil );
void		RpiRegSetVibMfcTuningFrequency( MFCTUNE *MfcTune, USHORT Frq );
void		RpiRegSetMdlVibFreq2( MFCTUNE *MfcTune, USHORT Frq2 );
void		RpiRegSetMdlVibFil2( MFCTUNE *MfcTune, USHORT Fil );



/****************************************************************************************************/
#define __FN_MFC_TUNE_H
#endif /* __FN_MFC_TUNE_H */
