/****************************************************************************************************/
/*																									*/
/*																									*/
/*		調整レス用パラメータ計算ヘッダーファイル													*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.09.28	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_TUNELESS_PRM_CALC_H
#define _TUNELESS_PRM_CALC_H


#include "BaseControls.h"
#include "BaseControlPrmCalc.h"
#include "ResVibCtrl.h"
#include "AutoNotchSet.h"



/****************************************************************************************************/
/*																									*/
/*		STRUCT DEFINITION																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		調整レスレベル定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_GAIN_TBL {
	USHORT		Kv;				/* 速度ループゲイン									[0.1Hz]			*/
	USHORT		F1rate;			/* オブザーバゲイン比率								[%]				*/
} TUNELESS_GAIN_TBL;
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_JRATE_TBL {
	USHORT		Jrate;			/* 負荷慣性モーメント/質量比						[%]				*/
	USHORT		dummy;			/* for alignment													*/
} TUNELESS_JRATE_TBL;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		DEFINES																						*/
/*																									*/
/****************************************************************************************************/
#define		PRM_SETTING_OK			0			/* パラメータ設定値判定用(正常)						*/
#define		PRM_SETTING_NG			-1			/* パラメータ設定値判定用(異常)						*/
#define		PRMERR_TUNELESS_LVL		0x01		/* Pn170 設定異常									*/
#define		PRMERR_TUNELESS_TYPE	0x02		/* Pn14F 設定異常									*/


/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
/* 調整レスゲイン算出用 */
#define		TULESCYCLE			KPI_SACYCLENS		/* 調整レス演算サイクルタイム(ScanA)		[ns]*/
#define		TULESCYCLEUS		((TULESCYCLE + 500)/1000)	/* 調整レス演算サイクルタイム		[us]*/
/*--------------------------------------------------------------------------------------------------*/
#define		TLSPDDET13BIT		(KPI_SACOUNTMS*600000/8192)			/* 13bitエンコーダ速度検出分解能			*/
#define		TLSPDDET200UM		(KPI_SACOUNTMS*10000/256*200/1000)	/* スケールピッチ200umの速度検出分解能		*/
/*--------------------------------------------------------------------------------------------------*/
#define		TLESSGLVLMAX		10				/* 調整レスゲインレベル最大値						*/
#define		TLESSJLVLMAX		5				/* 調整レスイナーシャレベル最大値					*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNELESSJRAT		30				/* 調整レスJL目標安定範囲				[倍]		*/
#define		DISOBSFSTD			6000			/* 調整レスRobust補償器周波数1：(基準)[0.1Hz]		*/
#define		TUNELESS_STIFFSTD	100				/* 調整レスRobust性強度	100%(基準)					*/
/*--------------------------------------------------------------------------------------------------*/
//#define	DISOBSF11			TuneLessGain	/* 調整レスRobust補償器周波数1：ロバスト性規定(騒音対策)[0.1Hz]	*/
#define		DISOBSF12			3000			/* 調整レスRobust補償器周波数1：ロバスト性規定(騒音対策)[0.1Hz]	*/
#define		DISOBSF21			8000			/* 調整レスRobust補償器周波数2：ノイズカット用(標準)[0.1Hz]		*/
#define		DISOBSF22			8000			/* 調整レスRobust補償器周波数2：ノイズカット用(騒音対策)[0.1Hz]	*/
#define		DISOBSF1MIN			1500			/* 調整レスRobust補償器周波数1最小値[0.1Hz]			*/
/*--------------------------------------------------------------------------------------------------*/
#define		VOBSLPFGNHZ			(DISOBSFSTD/TUNELESSJRAT)	/* 調整レス速度オブザーバLPF計算周波数[0.1Hz]		*/
/*--------------------------------------------------------------------------------------------------*/
#define		DISOBSKJ			100				/* 調整レス外乱オブザーバ係数ゲイン		[%]			*/
#define		DISOBSKD			99				/* 調整レス外乱オブザーバ係数			[%]			*/
#define		VOBSKS				60				/* 調整レス速度オブザーバゲイン			[Hz]		*/
#define		VOBSKJ				100				/* 調整レス速度オブザーバ係数ゲイン		[%]			*/
#define		VPHASEF2			10000			/* 調整レス用位相補償フィルタ周波数2	[0.1Hz]		*/
#define		VPHASEFULIM			1				/* 調整レス用位相補償フィルタ周波数下限値[0.1Hz]	*/
/*--------------------------------------------------------------------------------------------------*/
#define		EHVOBSTFULIM		2				/* 調整レス位相補償速度OBS LpFil時定数下限値[10us]	*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_IP_KPTI		250000			/* IPバランスのKp/Ti ζ=1.0, Kp[1/s] = 0.25 / Ti[s]	*/
#define		TUNLESS_TREFFIL		10				/* トルク補償後トルク指令フィルタ時定数[0.01ms]		*/
/*--------------------------------------------------------------------------------------------------*/
//#define	TUNELESS_STIFF1		TunLessStiff	/* 調整レスロバスト性強度	(標準)					*/
#define		TUNELESS_STIFF2		50				/* 調整レスロバスト性強度2	50%(安定性重視)			*/
#define		TUNELESS_STIFF3		30				/* 調整レスロバスト性強度3	30%(安定性重視)			*/
/*--------------------------------------------------------------------------------------------------*/
#define		VIBOBS_KS			800				/* 振動検出オブザーバKsゲイン			[0.1Hz]		*/
/*--------------------------------------------------------------------------------------------------*/
//#define	DAT_GAINLEVEL_MAX	4				/* 調整レスゲインレベル最大値						*//* <S1C7> */
#define		DAT_GAINLEVEL_MAX	7				/* 調整レスゲインレベル最大値						*//* <S1C7> */
#define		DAT_GAINLEVEL_MIN	0				/* 調整レスゲインレベル最小値						*/
#define		DAT_JRATLEVEL_MAX	2				/* 調整レスイナーシャレベル最大値					*/
#define		DAT_JRATLEVEL_MIN	0				/* 調整レスイナーシャレベル最小値					*/
/*--------------------------------------------------------------------------------------------------*/
#define		TLEX_DISOBSFRQ		4000			/* 新調整レス外乱オブザーバ周波数					*/
#define		TLEX_DISOBSKD		99				/* 新調整レス外乱オブザーバ係数						*/
#define		TLEX_VOBSLPFGAIN	200				/* 新調整レス位相補償速度オブザーバゲイン			*//* <S1C7> */
//#define	TLEX_VOBSLPFGAIN	150				/* 新調整レス位相補償速度オブザーバゲイン			*//* <S1C7> */
#define		TLEX_VPHASEF1		300				/* 新調整レス位相補償フィルタ周波数1				*/
#define		TLEX_VPHASEF2		600				/* 新調整レス位相補償フィルタ周波数2				*/
#define		TLEX_VOBSKS			120				/* 新調整レス位相補償速度オブザーバゲイン			*//* <S1C7> */
//#define	TLEX_VOBSKS			160				/* 新調整レス位相補償速度オブザーバゲイン			*//* <S1C7> */
#define		TLEX_TRQLPFIL		8				/* 新調整レストルクフィルタ時定数					*/
#define		TLEX_BNDPASSFILZN	10000			/* 新調整バンドパスフィルタ減衰係数					*/
#define		TLEX_RESSUPFIL		2000			/* 新調整レス 機械共振抑制フィルタ時定数			*/

/* 調整レスモード設定用 */
enum TUNELESS_MODE_SELECT{
	TLMODE_NORMAL		= (LONG)0x00L,			/* TuningLessMode NORMAL							*/
	TLMODE_CORELESS		= (LONG)0x01L,			/* TuningLessMode for CoreLess Linear				*/
	TLMODE_STABLE_L		= (LONG)0x10L,			/* TuningLessMode Stable Form for Linear			*/
	TLMODE_STABLE_R		= (LONG)0x20L,			/* TuningLessMode Stable Form for Rotary			*/
	TLMODE_SILENT		= (LONG)0x40L,			/* TuningLessMode Silent Form						*/
	TLMODE_SPDDETCHNG	= (LONG)0x100L,			/* TuningLessMode Speed Detect Change Request		*/
};


/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
/* 外乱オブザーバ演算サイクルタイム(速度ループと同一周期)[ns] */
#define	DOBSCYCLE		KPI_SACYCLENS


/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
/* 位相補償速度オブザーバ演算サイクルタイム(速度ループと同一周期)[us] */
#define	EHSOBSCYCLE		KPI_SACYCLEUS


/****************************************************************************************************/
/*																									*/
/*		STRUCTS																						*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Tuning Less Structures for API function Parameters											*/
/****************************************************************************************************/
typedef struct _TUNELESS_CFGPRM {
	USHORT		gnmode;					/* Pn10B : ゲイン関係応用スイッチ							*/
	USHORT		mfctype;				/* Pn14F : 制御関連スイッチ									*/
	USHORT		avibsw;					/* Pn160 : 制振制御関連スイッチ								*/
	USHORT		DatLevel;				/* Pn170 : ダイナミックオートチューニング関連スイッチ		*/
	USHORT		tfuncsw;				/* Pn408 : トルク関係機能スイッチ							*/
	USHORT		flg_wf;					/* PnF1E : フラグ											*/
} TUNELESS_CFGPRM;

typedef struct _TUNELESS_SETPRM {
	USHORT		DatLevel;				/* Pn170 : ダイナミックオートチューニング関連スイッチ		*/
	USHORT		trqfil11;				/* Pn401 : 1段目第1トルク指令フィルタ時定数					*/
	USHORT		ampw;					/* PnE12 : サーボパック容量									*/
	USHORT		ignq;					/* PnE21 : 電流ループゲイン(Q)								*/
	USHORT		kiq;					/* PnE23 : 電流ループ積分時定数(Q)							*/
	USHORT		fbfil2;					/* PnEA8 : 第2速度F/Bフィルタ時定数							*/
	USHORT		TuneLessGain;			/* PnEDA : 調整レス外乱オブザーバゲイン						*/
	USHORT		TuneLessStiff;			/* PnEDB : 調整レスイナーシャ補正ゲイン						*/
	USHORT		TuneLessTrqLpf;			/* PnEDC : 調整レス外乱トルクローパスフィルタ時定数			*/
	USHORT		TuneLessVfbLpf;			/* PnEDD : 調整レス速度FBローパスフィルタ時定数				*/
	MENCPRM		*MencP;					/* PnF00 - PnF1E : エンコーダパラメータ						*/
	DETVIB		*DetVib;				/* 発振監視用データ定義										*/
	ANOTCHSEQ 	*ANotchSeq;				/* 自動ノッチ用データ定義									*/
} TUNELESS_SETPRM;


/****************************************************************************************************/
/*		Disturb Observer Structures for API function Parameters										*/
/****************************************************************************************************/
typedef struct _DOBS_CFGPRM {
	USHORT		jrate;					/* Pn103 : 慣性モーメント比									*/
	USHORT		loophz;					/* Pn100 : 速度ループゲイン									*/
	USHORT		loophz2;				/* Pn104 : 第2速度ループゲイン								*/
	USHORT		dobgn;					/* Pn121 : 摩擦補償ゲイン									*/
	USHORT		dobgn2;					/* Pn122 : 第2摩擦補償ゲイン								*/
	USHORT		dtrqgn;					/* Pn123 : 摩擦補償係数										*/
	SHORT		dlpfil;					/* Pn124 : 摩擦補償周波数補正								*/
	USHORT		dobjgn;					/* Pn125 : 摩擦補償ゲイン補正								*/
} DOBS_CFGPRM;


/****************************************************************************************************/
/*		Eh Velocity Observer Structures for API function Parameters									*/
/****************************************************************************************************/
typedef struct _EHVOBS_CFGPRM {
	USHORT		jrate;					/* Pn103 : 慣性モーメント比									*/
	USHORT		evobgn;					/* Pn127 : 速度オブザーバゲイン								*/
	USHORT		evobjgn;				/* Pn128 : 速度オブザーバ位置補償ゲイン						*/
	USHORT		trqfil11;				/* Pn401 : 1段目第1トルク指令フィルタ時定数					*/
} EHVOBS_CFGPRM;



/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
//VOID	TuneLessCalculatePrm( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm, TUNELESS_SETPRM *Prm );		  /* <S01D> */
BOOL	TuneLessCalculatePrm( BASE_LOOPCTRLS *BaseLoops, BPRMDAT *Bprm,								  /* <S066> */
								TUNELESS_SETPRM *Prm, BASE_SPDLOOP_PRM *SpdCtrlPrm );
LONG	TuneLessCalculateInitPrm( BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm,
								  MENCV *MencV, ASICS *SvAsicRegs, TUNELESS_CFGPRM *CfgPrm );
BOOL	TuneLessSetTrqFil3( BOOL TuningLess, BOOL TuningLessEx, ULONG flg_wf,
							BOOL TuningLessLowGainMotor );

/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
LONG	DobsCalculatePrmSW( BOOL *DobsAct, BOOL TuningLess, USHORT DobsSel );
VOID	DobsCalculatePrm( DOBS_PRM *DobsPrm, ASICS *SvAsicRegs,
						  BPRMDAT *Bprm, DOBS_CFGPRM *CfgPrm, LONG GselNo );

/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
BOOL	EhVobsCalculatePrmSW( BOOL *EhVobsUse, BOOL TuningLess, USHORT EhVobsSel );					  /* <S066> */
BOOL	EhVobsCalculatePrm( EHVOBS_CTRL *EhVobsCtrl, BPRMDAT *Bprm,									  /* <S066> */
							EHVOBS_CFGPRM *CfgPrm, TRANSDUCE_CTRL *TransduceCtrl );


/****************************************************************************************************/
/*																									*/
/*		EXTERN																						*/
/*																									*/
/****************************************************************************************************/
extern	void	RpiRegSetTuneLessLevel( ULONG Gain, TUNELESS_CTRL *TuneLessCtrl );




/***************************************** end of file **********************************************/
#endif /* _TUNELESS_PRM_CALC_H */
