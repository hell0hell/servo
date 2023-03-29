/****************************************************************************************************/
/*																									*/
/*																									*/
/*		CheckAlarm.h : アラーム監視処理定義															*/
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
#ifndef CHECK_ALARM_H
#define CHECK_ALARM_H


#include "MLib.h"
#include "EncData.h"
#include "AlarmManager.h"
#include "SftDiagFunc.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダアラーム用定義																	*/
/*--------------------------------------------------------------------------------------------------*/
#define PG_OH_LEVEL				90							/* ＰＧオーバヒート温度 [℃]			*/

/*--------------------------------------------------------------------------------------------------*/
/*		リニア用定義																				*/
/*--------------------------------------------------------------------------------------------------*/
#define	POSDATALMT				0x70000000					/* 位置データオーバ閾値[指令単位]		*/

/*--------------------------------------------------------------------------------------------------*/
/*		モータ暴走検出用定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define MOTSPD_OS_10	1677721		/* OS速度の1/10													*/


/*--------------------------------------------------------------------------------------------------*/
/*		暴走監視用構造体定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_OVRRUN {
	LONG	OvrChkCnt;						/* 暴走監視 : 監視時間カウント							*/
	LONG	OvrAlmCnt;						/* 暴走監視 : アラームカウント							*/
	LONG	OvrTrqRef;						/* 暴走監視 : 前回トルク指令							*/
	LONG	OvrAbsMotSpd;					/* 暴走監視 : 前回モータ速度絶対値						*/
	LONG	OvrPeakMotSpd;					/* 暴走監視 : モータ速度最大値							*/
	LONG	OvrTrqLevel;					/* 暴走監視 : トルクレベル								*/
	LONG	OvrSpdLevel;					/* 暴走監視 : 速度レベル								*/
} CHECK_OVRRUN;


/*--------------------------------------------------------------------------------------------------*/
/*		アンプ＆モータ過負荷チェック用データ定義													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct OLPRM {					/* 過負荷チェック基本パラメータ								*/
	LONG	Tmid;							/* 中間電流OL時間(ディレーティング含む)	[0.1s]			*/
	LONG	Tmax;							/* 最大電流OL時間(ディレーティング含む)	[0.1s]			*/
	LONG	Ibse;							/* ベース電流(ディレーティング含む)		[15000/Imax]	*/
	LONG	Imid;							/* 中間OL電流(ディレーティング含む)		[15000/Imax]	*/
	LONG	Imax;							/* 最大OL電流(ディレーティング含む)		[15000/Imax]	*/
} OLPRM;


/*--------------------------------------------------------------------------------------------------*/
/*		過負荷チェック用構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
#if 1	/* <S154> */
typedef	struct _CHECK_OLP {					/* 過負荷チェック用パラメータ定義						*/
		LONG	Ibsqrmid;					/* 連続過負荷ベース電流の２乗							*/
		LONG	Ibsqrmax;					/* 瞬時過負荷ベース電流の２乗							*/
		LONG	AlmLvlmid;					/* 連続過負荷アラームレベル								*/
		LONG	WrnLvlmid;					/* 連続過負荷ワーニングレベル							*/
		LONG	AlmLvlmax;					/* 瞬時過負荷アラームレベル								*/
		LONG	WrnLvlmax;					/* 瞬時過負荷ワーニングレベル							*/
		KSGAIN	OlLvlmidGain;				/* 連続過負荷検出レベルゲイン[低速OL対応]				*/
		KSGAIN	OlLvlmaxGain;				/* 連続過負荷検出レベルゲイン[低速OL対応]				*/
		KSGAIN	CoolRateGain;				/* 自走空冷係数(リニア用)								*/
} CHECK_OLP;/* <S154> */

/* <S154> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		過負荷チェック用構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_OL {
	struct	{							/* 過負荷チェック用パラメータ定義						*/
		CHECK_OLP	NormalArea;			/* 通常区間過負荷チェック用パラメータ					*/
		CHECK_OLP	LowSpdArea;			/* 低速区間過負荷チェック用パラメータ					*/
		CHECK_OLP	LockArea;			/* ロック区間過負荷チェック用パラメータ					*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* 過負荷チェック用変数定義									*/
		LONG	WrnInfo;					/* ワーニング情報										*/
		LONG	SumImid;					/* 連続過負荷用２乗電流積算値							*/
		LONG	SumImax;					/* 瞬時過負荷用２乗電流積算値							*/
	} var;
} CHECK_OL;
#else	/* <S154> */
typedef	struct CHECK_OL {
	struct	{							/* 過負荷チェック用パラメータ定義							*/
		LONG	Ibsqrmid;					/* 連続過負荷ベース電流の２乗							*/
		LONG	Ibsqrmax;					/* 瞬時過負荷ベース電流の２乗							*/
		LONG	AlmLvlmid;					/* 連続過負荷アラームレベル								*/
		LONG	WrnLvlmid;					/* 連続過負荷ワーニングレベル							*/
		LONG	AlmLvlmax;					/* 瞬時過負荷アラームレベル								*/
		LONG	WrnLvlmax;					/* 瞬時過負荷ワーニングレベル							*/
		KSGAIN	OlLvlmidGain;				/* 連続過負荷検出レベルゲイン[低速OL対応]				*/
		KSGAIN	OlLvlmaxGain;				/* 連続過負荷検出レベルゲイン[低速OL対応]				*/
		KSGAIN	CoolRateGain;				/* 自走空冷係数(リニア用)								*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* 過負荷チェック用変数定義									*/
		LONG	WrnInfo;					/* ワーニング情報										*/
		LONG	SumImid;					/* 連続過負荷用２乗電流積算値							*/
		LONG	SumImax;					/* 瞬時過負荷用２乗電流積算値							*/
	} var;
} CHECK_OL;
#endif	/* <S154> */


/*--------------------------------------------------------------------------------------------------*/
/*		DB過負荷チェック用構造体定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_DBOL {
	struct	{							/* DB過負荷チェック用定数定義								*/
		BOOL	DbOHeatCheck;				/* 温度上昇によるＤＢ過負荷検出処理の有無選択			*/
		LONG	Kdbpower;					/* ＤＢ過負荷 : 電力演算ゲイン							*/
		LONG	DbolPowMax; 				/* ＤＢ過負荷 : 電力最大値								*/
		LONG	DbolPowBase; 				/* ＤＢ過負荷 : 電力ベース								*/
		LONG	DbolSpdLevel;				/* ＤＢ過負荷 : 速度レベル								*/
		LONG	DbolAlmLevel;				/* ＤＢ過負荷 : アラームレベル							*/
		LONG	Kundbpower;					/* ＤＢ抵抗消費電力演算ゲイン(for Un00B)				*/
		LONG	KundbpowerCycle;			/* ＤＢ抵抗消費電力演算ゲイン(for Un144)				*/
		LONG	DbOHeatAlmLvl;				/* ＤＢ過負荷(温度上昇)：アラームレベル					*/
		LONG	DbOHeatWrnLvl;				/* ＤＢ過負荷(温度上昇)：ワーニングレベル				*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* DB過負荷チェック用変数定義								*/
		BOOL	DbOHeatWarn;				/* ＤＢ過負荷(温度上昇)ワーニング検出中フラグ			*/
		LONG	DbolPowSum;					/* ＤＢ過負荷 : 電力積算値								*/
		LONG	DbPowerSum;					/* ＤＢ平均値 : 電力積算値								*/
		LONG	DbMeanCycle;				/* ＤＢ平均値 : 電力積算回数							*/
		LONG	DbHeat;						/* ＤＢ過負荷(温度上昇)検出用積算値						*/
		LONG	UnDbPower;					/* Un144  : ＤＢ消費電力値 			[%]					*/
		LONG	UnMeanDbPower;				/* Un00B  : ＤＢ消費電力平均値 		[%]					*/
	} var;
} CHECK_DBOL;

/* <S154> Start */
/************************************************************************************************/
/*																								*/
/*		アンプ低速過負荷チェック用変数定義																*/
/*																								*/
/************************************************************************************************/
/************************************************************************************************/
/*		アンプ低速過負荷：ＳＧＤＳ方式																	*/
/************************************************************************************************/
typedef	struct _LOWOLPRM{							/* 低速過負荷チェック基本パラメータ					*/
		OLPRM	AmpLock;						/* ロック区間用基本パラメータ							*/
		OLPRM	AmpLow;							/* 低速区間用基本パラメータ							*/
} LOWOLPRM;
/*----------------------------------------------------------------------------------------------*/
typedef	struct _LOWOLCHKP{							/* 低速過負荷チェック用パラメータ					*/
		struct {
		LONG	LowSpdOLchg;					/* 低速ＯＬ変曲点速度[低速区間]	[2^24/OvrSpd]	*/
		LONG	LockSpdOLchg;					/* 低速ＯＬ変曲点速度[ロック区間]	[2^24/OvrSpd]	*/
		LONG	LowSpdOffsetImid;				/* 低速ＯＬ変曲点ＯＬ電流[低速区間] [15000/Imax]		*/
		LONG	LowSpdOffsetTmax;				/* 低速ＯＬ変曲点ＯＬ時間[低速区間] [0.1s]			*/
		LONG	TransOLImid;					/* 低速ＯＬ中間ＯＬ電流変換係数 						*/
		LONG	TransOLTmax;					/* 低速ＯＬ最大ＯＬ時間変換係数 						*/
		UCHAR	LowSpdOLChkEnable;				/* 低速過負荷チェック実行フラグ						*/
		CHAR	Dummy1;							/* for alignment								*/
		SHORT	Dummy2;							/* for alignment 								*/
		} conf;
} LOWOLCHKP;

typedef	struct _CHECK_LOWOL{							/* 低速過負荷チェック用構造体定義			*/
	LOWOLPRM	LowOLPrm;								/* 低速過負荷チェック基本パラメータ				*/
	LOWOLCHKP	LowOLChkP;								/* 低速過負荷チェック用パラメータ				*/
	/* <S1F5> Start */
	struct {
		BOOL	BaseCurrentDerateEnable;				/* 過負荷検出ベース電流ディレーティング機能有効 	*/
	}conf;
	/* <S1F5> End */
} CHECK_LOWOL;
/* <S154> End */

#if 0	/*<S1F6>*/
/*--------------------------------------------------------------------------------------------------*/
/*		セーフティ入力信号タイミング異常チェック用構造体定義										*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_HWBB_TIMING {
	struct	{							/* セーフティ入力信号タイミング異常チェック用定数定義		*/
		LONG	MaxBbonTimeDiff;			/* 入力信号時間差監視：最大許容時間						*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* セーフティ入力信号タイミング異常チェック用変数定義		*/
		LONG	BbonTimeDiffCnt;			/* セーフティ入力信号タイミング異常時間カウント			*/
	} var;
} CHECK_HWBB_TIMING;
#endif	//#if 0	/*<S1F6>*/


/*--------------------------------------------------------------------------------------------------*/
/*		位置偏差過大チェック用構造体定義															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_OVRPOSERR {
	LONG		PerAlmLevel;				/* 位置偏差過大アラームレベル				[指令単位]	*/
	LONG		PerWrnLevel;				/* 位置偏差過大ワーニングレベル				[指令単位]	*/
	LONG		SvonPerAlmLevel;			/* サーボオン時位置偏差過大アラームレベル	[指令単位]	*/
	LONG		SvonPerWrnLevel;			/* サーボオン時位置偏差過大ワーニングレベル	[指令単位]	*/
} CHECK_OVRPOSERR;

typedef	struct CHECK_FENCPOSERR {
	LONG	FencPosLast;					/* セミフル偏差過大検出：フルク前回位置					*/
	LONG	MencPosLast;					/* セミフル偏差過大検出：セミク前回位置					*/
	LONG	ExtPGRem;						/* セミフル偏差過大検出：								*/
	LONG	MtPGRem;						/* セミフル偏差過大検出：								*/
	LONG	FencPosOneTurn;					/* セミフル偏差過大検出：フルク１回転前位置				*/
	ULONG	FSOFAlmCnt;						/* セミフル偏差過大検出：検出フィルタ					*/
	PERRA	sfperra;						/* 位置偏差Ａ構造体 (ｾﾐ-ﾌﾙ)偏差計算用					*/

	EGEAR	Egear;							/* ｾﾐギヤ構造体											*/
	EGEAR	sfgear;							/* ｾﾐ/ﾌﾙ変換ギヤ構造体									*/
	ULONG	fpgoverLevel;					/* モータ-負荷位置間偏差過大検出レベル		[指令単位]	*/
	LONG	fpgcutratelv;					/* フルクローズ1回転当たりの乗算値				   [%]	*/
	LONG	fcpgpls;						/* 外付けスケールピッチ数					  [P/Rev]	*/
	USHORT	fpgcutrate;						/* Pn52A : フルクローズ1回転当たりの乗算値				*/
} CHECK_FENCPOSERR;

/*--------------------------------------------------------------------------------------------------*/
/*		アラーム検出処理用構造体定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_ALARM {
//	ENVMON				EnvMon;				/* 設置環境モニタ用変数									*//* <S03A> *//* <S0ED> */
//	CHECK_OH			OverHeat;			/* オーバーヒートチェック用変数							*//* <S03A> *//* <S0ED> */
	CHECK_OL			MotOL;				/* モータ過負荷チェック用変数							*/
	CHECK_OL			AmpOL;				/* アンプ過負荷チェック用変数							*/
	CHECK_OVRRUN		OvrRun;				/* 暴走検出用変数										*/
	CHECK_DBOL			DbOL;				/* DB過負荷チェック用変数								*/
	CHECK_HWBB_TIMING	HwbbTiming;			/* HWBB入力信号タイミング異常チェック用変数				*/
	CHECK_OVRPOSERR		OvrPosErr;			/* 位置偏差過大チェック用変数							*/
	CHECK_FENCPOSERR	OvrFPosErr;			/* セミフル位置偏差過大チェック用変数					*/
	CHECK_LOWOL			AmpLowOL;			/* アンプ低速過負荷用変数定義							*/ /* <S154>  */
//	CHECK_RUSH			*pRush;				/* 突入防止抵抗過負荷検出用変数							*/
//	CHECK_REGOL			*pRegOL;			/* 回生過負荷チェック用変数へのポインタ(ユニット共通)	*/
//	CHECK_REGENE		*pRegChk;			/* 回生異常チェック用変数へのポインタ(ユニット共通)		*/
	ULONG				OSCount;			/* OS状態連続カウンタ									*/
	LONG				Gup1Timer;			/* A.b33検出用タイマ									*/
	USHORT 				faultState;			/* Fault status of Servo ASIC 							*/

} CHECK_ALARM;




/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	AdetInitCheckAlarm( CHECK_ALARM *pChkAlm, ULONG AlmOLInfo );
USHORT	AdetCheckAsicHwAlarm( CHECK_ALARM *pChkAlm, ASICS *SvAsic,
										BOOL EncDisable, ALMMAN_HANDLE *AlmMngr );
void	AdetCheckOCAlarmDetections( ALMMAN_HANDLE *AlmMngr, ASICS *SvAsic, LONG axisNo, BOOL BeSts );	/* <S126> *//* <S137> */
void	AdetCheckMotorOverCurrent( ALMMAN_HANDLE *AlmMngr, ASICS *SvAsic, BOOL BeSts, BOOL BaseEnableSts );/*<S1F5>*/
void	AdetCheckSencAlarm( ALMMAN_HANDLE *AlmMngr, MENCV *MencV, USHORT RxAlmCode );					/* <S03A> */
void	AdetCheckSencBaseTimer(MENCV *MencV, ALMMAN_HANDLE *AlmMngr );
void	AdetCheckSencOverHeat( MENCV *MencV, ALMMAN_HANDLE *AlmMngr );
void	AdetCheckOverSpeed( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *pChkAlm, LONG MotSpd, BPRMDAT *Bprm );
/*--------------------------------------------------------------------------------------------------*/
void	AdetCheckDivOverSpeed( ALMMAN_HANDLE *AlmMngr, LONG MotSpd, LONG DivOvrSpd );				/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
//void	AdetCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,							  /* <S03A> *//* <S0ED> */
//														SHORT CtrlBoadTemp, SHORT PowerBoadTemp );		/* <S0ED> */
//void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt, USHORT PowerBoadErrCnt );/* <S0DE> *//* <S0ED> */
//void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp );							  /* <S03A> *//* <S0ED> */

//void	AdetCheckOverLoadAmpMot( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *CheckAlarm,
//								LONG IdRefMon, LONG IqRefMon, LONG AbsMotSpd, BOOL EncConnect );				/* <S154> */
BOOL	AdetCheckOverLoadAmpMot( ALMMAN_HANDLE *AlmMngr, CHECK_ALARM *CheckAlarm,
								LONG IdRefMon, LONG IqRefMon, LONG AbsMotSpd, BOOL EncConnect );				/* <S154> */

void	AdetCheckOLAmpMot( LONG AbsMotSpd, LONG Ixsqr, CHECK_OL *ol_ptr, ALMMAN_HANDLE *AlmMngr , UCHAR Mode);	/* <S154> */

void	AdetCheckDbrakeOverLoad( ALMMAN_HANDLE *AlmMngr, CHECK_DBOL *DbOL, BOOL DbOn, LONG AbsMotSpd );
void	AdetCheckRippleWrn( ALMMAN_HANDLE *AlmMngr, BOOL RippleWrnState );									/* <S1FD> */
BOOL	AdetCheckBbonSignal( CHECK_HWBB_TIMING *pHwbbTmg, BOOL LackOneBbon );
//BOOL	AdetCheckGateDriveDefect( DET_HWBB_STS *pDetHwbbSts );
void	AdetCheckMotorOverrun( ALMMAN_HANDLE *AlmMngr, CHECK_OVRRUN *OvrRun,
								 LONG MotSpd, LONG TrqRefMon, LONG BaseEnableSts, BOOL MotTypeLinear );
void	AdetCheckSvonPerrOverFlow( ALMMAN_HANDLE *AlmMngr, CHECK_OVRPOSERR *OvrPosErr, LONG PositionError );
void	AdetCheckPerrOverFlow( ALMMAN_HANDLE *AlmMngr, CHECK_OVRPOSERR *OvrPosErr,
										BOOL PosCtrlMode, LONG PositionError, BOOL SvonSpdLmt );
void AdetCheckFullcEncoderAlarm(CHECK_FENCPOSERR *ChkFposErr, MENCV *FencV,
                            	BOOL PosCtrlMode, LONG MencPos, LONG FencPos, ALMMAN_HANDLE *AlmMngr );
//<S1F5>BOOL	AdetTestPulseDiagnostic( BOOL DpInState, BOOL DpInLastState, BOOL CheckEnable, BOOL Bbon );					/* <S0F1> *//*<S1DB>*/





#endif /* CHECK_ALARM_H */
/***************************************** end of file **********************************************/
