#ifndef __TUNE_LIBRARY_H 
#define __TUNE_LIBRARY_H
/****************************************************************************
Description	: Tuning Function Library
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.11.16    created

*****************************************************************************/
#include "Basedef.h"
#include "PrmAccessIf.h"
#include "PnPrmStruct.h"
#include "ResVibCtrl.h"



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		for Tuning Library																			*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_FREQ_SEARCHINIT	0		/* 周波数解析処理正常終了							*/
#define		TUNE_FREQ_SEARCHEND		1		/* 周波数解析処理正常終了							*/
#define		TUNE_FREQ_SEARCH		2		/* 周波数解析処理中									*/
#define		TUNE_FREQ_SEARCHERR		-1		/* 周波数解析処理異常終了							*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_MDLZETA			1000	/* モデルループゲイン補正 				[0.1%]		*/
#define		TUNE_MFCKTFF			1000	/* モデルトルクフィードフォワード補償	[0.1%]		*/
#define		TUNE_MFCKVFF			1000	/* モデル速度フィードフォワード補償 	[0.1%]		*/
#define		TUNE_MFCKVFF2			900		/* モデル速度フィードフォワード補償２	[0.1%]		*/
#define		TUNE_DTRQGN				85		/* 推定外乱係数     = 85[%]							*/
#define		TUNE_DOBJGN				100		/* イナーシャ補正   = 100[%]						*/
#define		TUNE_DOBGNRATIO			100		/* オブザーバゲイン = 100[%]						*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_PI_TIKV			636620	/* PIバランスのTi/Kv  636620 = 4 *1000000 / 2π		*/
#define		TUNE_IP_TIKV			318310	/* IPバランスのTi/Kv  318310 = 2 *1000000 / 2π		*/
#define		TUNE_IP_KPTI			250000	/* IPバランスのKp/Ti  ζ=1.0, Kp[1/s] = 0.25 / Ti[s]*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_ANOTCH_DECA		10		/* 0.1Hz -> 1Hzの単位変換							*/
#define		TUNE_ANOTCH_Q_MIN		50		/* 自動ノッチのQ値下限								*/
#define		TUNE_ANOTCH_Q_MAX		300		/* 自動ノッチのQ値上限 3.00							*/
#define		TUNE_ANOTCH_Q_MAX_DET	500		/* 上限検出用の自動ノッチのQ値上限 5.00				*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_MAX_DLPFIL			4000	/* Kv連動外乱オブザーバローパスの上限 400.0Hz		*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_RESFRQ_KV			100		/* Kv補正用共振周波数			[0.1Hz]				*/
#define		TUNE_ANTFRQ_KV			100		/* Kv補正用反共振周波数			[0.1Hz]				*/
#define		TUNE_ANTFRQ_NTQ1		100		/* ノッチQ値計算用反共振周波数	[0.1Hz]				*/
#define		TUNE_ANTFRQ_NTQ2		100		/* ノッチQ値計算用反共振周波数	[0.1Hz]				*/
/*--------------------------------------------------------------------------------------------------*/
#define		AVIB_FRQ_MIN			100		/* A型制振検出周波数下限 100Hz						*/
#define		AVIB_FRQ_MAX			1000	/* A型制振検出周波数上限 1000Hz						*/
#define 		AVIBDAMPGN				180		/* A型制振制御調整ダンピングゲイン設定値 150%		*/
/*--------------------------------------------------------------------------------------------------*/
#define		TUNE_ANOTCH_MINKV		600		/* ノッチフィルタON最低Kv		[0.1Hz]				*/
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Auto Notch																				*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NOTCH_INIT		0x00	/* ノッチ設定初期								*/
#define		OPTUNE_STS_NOTCH1_OK		0x01	/* 1段目ノッチ設定正常完了						*/
#define		OPTUNE_STS_NOTCH2_OK		0x02	/* 2段目ノッチ設定正常完了						*/
#define		OPTUNE_STS_NOTCHQ1_ERR		0x04	/* 1段目ノッチQ値異常							*/
#define		OPTUNE_STS_NOTCHQ2_ERR		0x08	/* 2段目ノッチQ値異常							*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NT_NODETECT		0		/* ノッチ完了ステータス：振動未検出				*/
#define		OPTUNE_STS_NT_DETECT		1		/* ノッチ完了ステータス：振動検出中				*/
#define		OPTUNE_STS_NT_NOENABLE		2		/* ノッチ完了ステータス：自動設定不可			*/
#define		OPTUNE_STS_NT_DET_ERR		3		/* ノッチ完了ステータス：検出エラー				*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NT1_NODETECT		0		/* ノッチ完了ステータス：1段目未実施			*/
#define		OPTUNE_STS_NT1_COMP			1		/* ノッチ完了ステータス：1段目設定完了			*/
#define		OPTUNE_STS_NT1_NG			2		/* ノッチ完了ステータス：1段目設定失敗			*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_STS_NT2_NODETECT		0		/* ノッチ完了ステータス：2段目未実施			*/
#define		OPTUNE_STS_NT2_COMP			1		/* ノッチ完了ステータス：2段目設定完了			*/
#define		OPTUNE_STS_NT2_NG			2		/* ノッチ完了ステータス：2段目設定失敗			*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTUNE_PSTS_NODETECT		0		/* ノッチ完了ステータス：直前ノッチ未実施		*/
#define		OPTUNE_PSTS_NT1_COMP		1		/* ノッチ完了ステータス：直前ノッチ1段目		*/
#define		OPTUNE_PSTS_NT2_COMP		2		/* ノッチ完了ステータス：直前ノッチ2段目		*/


/*--------------------------------------------------------------------------------------------------*/
/*		for Online Frequency Analysis																*/
/*--------------------------------------------------------------------------------------------------*/
#define	PEAK_DETECT_NUM		5					/* スペクトラムピーク周波数検出個数				*/
#define	WORK_AREA_NUM		200					/* ワーク用エリア数 [(Fmax-Fmin)/Sno]とすること	*/
#define	REGULAR_SFT			16384				/* 正規化量：スペクトラム算出時のOverFlow対策	*/
#define 	MINIMAM_FREQ		240					/* 検出周波数下限値[Hz]							*/
#define	MINIMAM_SPECTRUM	3000				/* スペクトルピーク下限値						*/
#define	MINIMAM_SPECTRUM2	30000				/* 調整レス自動ノッチ用スペクトルピーク下限値	*/
#define	ONFFT_OK_LMT		20					/* 周波数解析閾値 [倍]							*/
#define	ONFFT_OK_LMT2		50					/* 調整レス自動ノッチ周波数解析閾値[倍]			*/
//#define	SAMPLETIME		4000					/* 1 / (データサンプル周期(sec)×2)[Hz]			*/
#define	SAMPLETIME(SBScanTimeUs) (500000/SBScanTimeUs)	/* 1/(データサンプル周期(sec)×2)[Hz]	*/




/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		自動ノッチ設定用定数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct AUTONOTCH_PRM {
	USHORT			KvrateTf;		/* 自動Tf計算のカットオフのKvとの比					[倍]		*/
	USHORT			KvrateTfonNt;	/* ノッチON時の自動Tf計算のカットオフのKvとの比		[倍]		*/
	USHORT			KvrateQ;		/* ノッチQ値計算のカットオフのKvとの比				[倍]		*/
	USHORT			J1Tf;			/* 自動Tf計算のモータ側慣性モーメント比				[%]			*/
	USHORT			J1TfonNt;		/* ノッチON時の自動Tf計算のモータ側慣性モーメント比	[%]			*/
	USHORT			J1Q;			/* ノッチON時のQ値計算のモータ側慣性モーメント比	[%]			*/
	USHORT			TfMin;			/* 自動Tf計算最小設定値								[0.01ms]	*/
	USHORT			NotchQMin;		/* ノッチQ値最小設定値								[-]			*/
} AUTONOTCH_PRM;



/*--------------------------------------------------------------------------------------------------*/
/*		自動ノッチ設定用変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct AUTONOTCH_DATA {
	struct {						/* 振動周波数解析用定数データ定義								*/
		USHORT			freq;			/* 自動ノッチ周波数											*/
		USHORT			KvGain;			/* Tf, ノッチQ値算出用ゲイン								*/
		USHORT			Jrate;			/* Tf, ノッチQ値算出用負荷慣性モーメント					*/
	} conf;
	struct {						/* 振動周波数解析用変数データ定義								*/
		USHORT			NotchSw;		/* 自動ノッチ設定選択										*/
		USHORT			TrqFil;			/* トルクフィルタ設定値										*/
		USHORT			NotchFil1;		/* 1段目ノッチフィルタ周波数設定値							*/
		USHORT			NotchFil2;		/* 2段目ノッチフィルタ周波数設定値							*/
		USHORT			NotchQ1;		/* 1段目ノッチフィルタQ値設定値								*/
		USHORT			NotchQ2;		/* 2段目ノッチフィルタQ値設定値								*/
	} var;
} AUTONOTCH_DATA;


/*--------------------------------------------------------------------------------------------------*/
/*		自動ノッチ設定用構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct AUTONOTCH {
	AUTONOTCH_PRM	ANotchPrm;			/* 自動ノッチ設定用定数										*/
	AUTONOTCH_DATA	ANotchData;			/* 自動ノッチ設定用変数										*/
} AUTONOTCH;



/*--------------------------------------------------------------------------------------------------*/
/*		オンライン振動モニタ用構造体定義(レジスタ用)												*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FRQANA {
	USHORT		Freq;					/* 解析周波数												*/
	USHORT		Amp;					/* 解析振幅													*/
} FRQANA;


/*--------------------------------------------------------------------------------------------------*/
/*		周波数解析結果格納用構造体定義(レジスタ用)													*/
/*--------------------------------------------------------------------------------------------------*/
/* オートチューニングエリア情報：2400H - 25FFH */
typedef	struct FFTANA_RSLT {
	FRQANA		VibmRf[3];				/* 0x2400 : オンライン振動モニタ共振周波数1					*/
										/* 0x2401 : オンライン振動モニタ共振周波数1ピーク値			*/
										/* 0x2402 : オンライン振動モニタ共振周波数2					*/
										/* 0x2403 : オンライン振動モニタ共振周波数2ピーク値			*/
										/* 0x2404 : オンライン振動モニタ共振周波数3					*/
										/* 0x2405 : オンライン振動モニタ共振周波数3ピーク値			*/
	FRQANA		VibmAf;					/* 0x2406 : オンライン振動モニタ反共振周波数1				*/
										/* 0x2407 : オンライン振動モニタ反共振周波数1ピーク値		*/
} FFTANA_RSLT;


/*--------------------------------------------------------------------------------------------------*/
/*		振動周波数解析 定数構造体定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FFTANA_PRM {				/* 振動周波数解析用パラメータ定義						*/
	LONG		Spectrumval;				/* 振動解析合否判定スペクトラム補正値					*/
	LONG		Spectrumval2;				/* 調整レス振動解析合否判定スペクトラム補正値			*/
	BOOL		ActiveFFtOn;				/* 強制周波数解析要求フラグ								*/
	BOOL		TuneLessSts;				/* 調整レスオート有効・無効フラグ						*/
	LONG		SumpleNum;					/* データサンプル数										*/
	LONG		AnaMaxFreq;					/* 解析最大周波数										*/
	LONG		AnaMinFreq;					/* 解析最小周波数										*/

	LONG		SampleTime;					/* 1 / (データサンプル周期(sec)×2) [Hz]				*/
} FFTANA_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		振動周波数解析  変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct FFTANALYZE {				/* 振動周波数解析用パラメータ＆変数定義					*/
	FFTANA_RSLT	FftAnaRslt;					/* 振動周波数解析結果格納用								*/
	FFTANA_PRM	FftAnaPrm;					/* 振動周波数解析用パラメータ							*/
	/*----------------------------------------------------------------------------------------------*/
	struct {								/* 振動周波数解析用変数データ定義						*/
		ULONG	swork_max[PEAK_DETECT_NUM];		/* スペクトラムピークデータ							*/
		ULONG	freq_max[PEAK_DETECT_NUM];		/* 周波数ピークデータ								*/
		ULONG	swork_min;						/* スペクトラム下限データ							*/
		ULONG	freq_min;						/* 周波数下限データ									*/
		LONG	Reals[WORK_AREA_NUM];			/* RealPart計算ワーク用								*/
		LONG	Images[WORK_AREA_NUM];			/* ImagePart計算ワーク用							*/
	} var;
} FFTANALYZE;


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		for Tuning Library																			*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT	TunePrmWriteRam( REG_ACC_T *AccessPrm, CPRM_ATTR_T *PrmAttr, LONG Data, LONG Size );
USHORT	TunePrmLimitUnsigned( ULONG Input, CPRM_ATTR_T *PrmAttr );
SHORT	TunePrmLimitSigned( LONG Input, CPRM_ATTR_T *PrmAttr );
USHORT	TuneKvFuncTi( USHORT Kv, BOOL IPSpdControl );
USHORT	TuneKvFuncKp( USHORT Kv, USHORT Ti, BOOL IPSpdControl );
USHORT	TuneKvFuncDobsgn( USHORT Kv );
SHORT	TuneKvFuncDobLpFil( USHORT Kv, USHORT Dobsgn );
SHORT	TuneNotchQ( AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, USHORT kv, USHORT jrate, UCHAR sel, PRMDATA *Prm );
void	TuneGetNotchQSetting( AUTONOTCH *ANotch, USHORT *NotchQ1, USHORT *NotchQ2, USHORT *TrqFil );
USHORT	TuneNotchModKv( USHORT kv, USHORT jrate, USHORT j1rate );
SHORT	TuneNotchQset( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, USHORT Antfreq, USHORT *autoQ );
USHORT	TuneNotchTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate, USHORT Nfreq, USHORT autoQ_Q1 );
USHORT	TuneOnePrmTfcalc( AUTONOTCH_PRM *ANotchPrm, USHORT kv, USHORT jrate );
SHORT	TuneNotchQset2( USHORT freq, USHORT Nfreq, USHORT *autoQ );
void	TuneFilGainCalbyFilType( AUTONOTCH *ANotch, UCHAR Type );
//void	TuneMfcActive( PRMDATA *Prm );
//void	TuneMfcActiveStMode( PRMDATA *Prm );
//void	TuneDobsActive( PRMDATA *Prm );
//SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm );
void	TuneMfcActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );										/*<S09E>*/
void	TuneMfcActiveStMode( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );								/*<S09E*/
void	TuneDobsActive( PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );									/*<S09E>*/
SHORT	TuneATypeResSet( USHORT freq, USHORT auton_sw, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );	/*<S09E>*/
SHORT	TuneNotchFilterSetting( AUTONOTCH *ANotch, PRMDATA *Prm, REGIF_CONFIG_T *RegCfg );
SHORT	TuneVibrationTrace( DETVIB *DetVib, FFTANALYZE *FftAna, UCHAR TraceRequest, SHORT TrgLvl, BOOL TuneLessSts );
USHORT	TuneMfcKpmCalcAntFreq( USHORT AntFreq );
void	TuneSetNotchFilSettingData( AUTONOTCH *ANotch, USHORT freq, USHORT KvGain, USHORT Jrate );


/*--------------------------------------------------------------------------------------------------*/
/*		for Online Frequency Analysis																*/
/*--------------------------------------------------------------------------------------------------*/
void	FftCalculateInitPrm( FFTANALYZE *FftAna, LONG SampleTimeUs );
SHORT	FftAnalyzeExecute( FFTANALYZE *FftAna, SHORT *TraceData );
void	FftSetAnaCondition( FFTANALYZE *FftAna, LONG SumpleNum, LONG AnaMaxFreq, LONG AnaMinFreq, BOOL TuneLessSts );
USHORT	FftGetAnaRsltVibmRf( FFTANALYZE *FftAna );
void	FftSetForceAnaReq( FFTANALYZE *FftAna, BOOL Setting );
BOOL	FftGetForceAnaReq( FFTANALYZE *FftAna );



/****************************************************************************************************/
/*																									*/
/*		Extern Definition																			*/
/*																									*/
/****************************************************************************************************/
extern	const	AUTONOTCH_PRM	AutoNotchTbl[ 4 ];



#endif //__TUNE_LIBRARY_H 
/***************************************** end of file **********************************************/
