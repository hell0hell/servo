#ifndef	_POWER_MANAGER_H_ /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PowerManager.h : サーボ電源管理処理定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.04.19	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Bprm.h"
#include "BaseCmnStruct.h"
#include "JL086ServoIpReg.h"


/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		主回路監視用構造体定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_MAIN_POWER {
	struct	{							/* 主回路電源監視用パラメータ定義							*/
		ULONG	AcoffDetctTime;				/* AC Power Off Detect Time								*/
		BOOL	Dcbus;						/* DC電源入力選択(Pn001.2)								*/
		BOOL	Ac3Phase;					/* 電源入力3相(PnE0C.1,2)								*/
		BOOL	DetectAcOn2;				/* 電源欠相検出方式(PnE0C.3)							*/
		LONG	RlyDbWay;					/* 突入/DB制御方法選択設定(PnE0C.4,5)					*/
		ULONG	UvFilter;					/* UV検出フィルタ時定数(PnE74)							*/
		LONG	OvdetAlmFilter;				/* 過電圧アラームフィルタ(PnE7A.2,3)	(1=250us)		*/
		ULONG	PowerChargeWaitTime;		/* DC充電完了待ち時間	<S053>							*/
		ULONG	PowerReadyWaitTime;			/* パワーレディ待ち時間	<S053>							*/
		ULONG	DischCheckVlt;				/* 急速放電確認閾値電圧									*//* <S1C6> *//* <S1D1> */
		ULONG	DischCompVlt;				/* 急速放電継続閾値電圧									*//* <S1C6> *//* <S1D1> */
		ULONG	MainDischActTime;			/* メイン急速放電時間(充電時間と同一)					*//* <S1C6> */
		USHORT	UvLevel;					/* 不足電圧(UV)検出レベル								*/
		USHORT	UvWrnLevel;					/* 不足電圧(UV)ワーニング検出レベル						*/
		USHORT	OvLevel;					/* 回生使用時の過電圧(OV)検出レベル						*/
		USHORT	NonRegOvLevel;				/* 回生不使用時の過電圧(OV)検出レベル					*//* <S0F7> */
		USHORT	TestDcLevel;				/* 自動試験機モード：主回路電圧検出補正時調整基準電圧	*/
		BOOL	TestMode1;					/* 自動試験機モード：電源管理							*/
		BOOL	TestMode2;					/* 自動試験機モード：回生異常検出なし					*/
		BOOL	TestMode3;					/* 自動試験機モード：ＤＢ停止時下側ＴｒをＯＮしない(INGRAMでは不使用)	*/
		BOOL	TestMode4;					/* 自動試験機モード：ASIC WDTをkickしない				*/
		BOOL	TestMode5;					/* 自動試験機モード：主回路遮断中、突防リレーを強制ON/OFF	*/
		BOOL	TestMode6;					/* 自動試験機モード：回生制御を強制ON/OFF				*//* <S0CB> */
		BOOL	TestMode7;					/* 自動試験機モード：DB制御を強制ON/OFF					*//* <S0CB> */
		BOOL	TestMode8;					/* 自動試験機モード：ゲートドライブ信号を強制ON/OFF		*//* <S0CB> */
		BOOL	TestMode9;					/* 自動試験機モード：ゲートドライブ信号をScanAにてON/OFF*//* <S19F> */
		BOOL	GateDriveSigTestFlag;		/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA)*//* <S19F> */
		BOOL	PreWarningSw;				/* 予防保全ワーニング選択								*//* <S154> */
		BOOL	DcDischarge;				/* DC電源入力時の急速放電選択							*//* <S21D> */
		BOOL	CapaLinkFlag;				/* コンデンサバンク接続フラグ								*//* <S21D> */
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{							/* 主回路電源監視用パラメータ定義							*/
		ULONG	AconWaitTimer;				/* Initial Acon Wait Timer								*/
		ULONG	AcoffChkTimer;				/* AC Power Off Check Timer								*/
		ULONG	DconChkTimer;				/* DC Power On  Check Timer								*/
		ULONG	DischChkTimer;				/* Discharge Check Timer								*/
		ULONG	DischSeqNo;					/* Discharge Sequence Number							*//* <S1C6> */
		ULONG	DischActTimer;				/* Discharge Active Time Counter						*//* <S1C6> */
		ULONG	DischStartVolt;				/* Discharge Start Timing Voltage						*//* <S1C6> */
		ULONG	OpenChkTimer[2];			/* Open Phase Check Timer								*/
		ULONG	UvChkTimer;					/* Under Voltage Check Timer							*/
		ULONG	WireAlmDetWait;				/* Remain DC Power Check Timer							*/
		BOOL	DetOpenAcOn1;				/* ACON1 signal			(Used by Detect Open-Phase)		*/
		BOOL	DetOpenAcOn2;				/* ACON2 signal			(Used by Detect Open-Phase)		*/
		BOOL	Alm330Lt;					/* A.330 in DC mode Detect Flag							*/
		LONG	AcOn1OffCount;				/* ACON1 Off Count Timer(Used by Detect Open-Phase)		*/
		LONG	AcOn2OffCount;				/* ACON2 Off Conut Timer(Used by Detect Open-Phase)		*/
		LONG	AconDetectCount;			/* Acon Active Detect Count in DC mode					*/
		BOOL	PreSigOut;					/* 予防保全出力信号										*//* <S154> */
		/* <S21D> Start */
		BOOL	ClinkOut;					/* CLINK Output Signal									*/
		BOOL	CmconOut;					/* CMCON Output Signal									*/
		ULONG	ConvInSigDischErrChkTimer;	/* Converter I/O Signal Discharge Error Check Timer		*/
		LONG	DcVoltOld;					/* DC Volt Before Discharge								*/
		ULONG	ClinkAnsErrChkTimer;		/* Capacitor Bank Link Answer Back Error Check Timer	*/
		ULONG	CmcAnsErrChkTimer;			/* Capacitor Bank MC Answer Back Error Check Timer		*/
		ULONG	ClinkAnsTimer;				/* Clink Answer Check Timer								*/
		BOOL	WrnExRegInhibit;			/* WRN_EXREG Inhibit Flag								*/
		BOOL	CapaChargeOk;				/* Capacitor Bank Charge OK Flag						*/
		/* <S21D> End */
	} var;
/*--------------------------------------------------------------------------------------------------*/
} CHECK_MAIN_POWER;


/*--------------------------------------------------------------------------------------------------*/
/*		DC電源電圧検出用変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct DET_DC_VOLT {
	struct	{							/* DC電源電圧検出用定数定義									*/
		UCHAR	VdetMode;					/* 主回路母線電圧検出方式(PnE0C.8,9)					*/
		USHORT	VdetMaxVolt;				/* Amp Volt detect at max input		[data/MaxV]			*/
		LONG	VdetAdNorm;					/* 主回路検出AD変換結果正規化定数(PnE70)				*/
		LONG	VdetAdFil;					/* 主回路検出ADフィルタゲイン							*/
		LONG	VdcdetAlmFilter;			/* 主回路検出異常アラームフィルタ(PnE7A.0,1) (1=250us)	*/
		BOOL	VdetPrmOk;					/* 主回路検出関連パラメータ設定(1:OK, 0:NG)				*/
		LONG	VdetAdadj;					/* PnE5B : 下位:P-N電圧検出レベル微調					*/
		LONG	Kdcvolt;					/* 主回路検出値換算ゲイン　[Admax] ⇒ [V]				*/
	} conf;
	LONG	DcAdFili;						/* DC Volt Ad read filter output						*/
	LONG	DcAdFilo[3];					/* DC Volt Ad read filter output						*/
	LONG	DcVoltx;						/* DC Volt												*/
	ULONG	DcvErrCnt;						/* DC Volt Error Check Counter							*/
	ULONG	DcvWaitCnt;						/* DC Volt Error Check Wait Counter						*/
	LONG	VdcdetAlmCnt;					/* 主回路検出アラーム検出フィルタ	(1=250us)			*/
	LONG	OvdetAlmCnt;					/* 過電圧アラーム検出フィルタ		(1=250us)			*/
} DET_DC_VOLT;


/*--------------------------------------------------------------------------------------------------*/
/*		電源ステータス構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POWER_STATUS {
	BOOL			Acon;					/* AC電源検出状態										*/
	BOOL			Acon1Sig;				/* コンバータ出力信号状態1								*/
	BOOL			Acon1Sig_l;				/* コンバータ出力信号状態1(前回値)						*/
	BOOL			Acon2Sig;				/* コンバータ出力信号状態2(現状未使用)					*/
	BOOL			ChargeOk;				/* DC充電完了状態										*/
	BOOL			Discharge;				/* DC急速放電要求										*/
	BOOL			PowerOn;				/* 主電源ON状態(パワーレディ待ち時間考慮)				*/
	BOOL			PowerOnEep;				/* 主電源ON状態(EEPROM管理用)							*/
	LONG			DcVolt;					/* P-N電圧検出値										*/
	BOOL			NegPumpReq;		        /* Negative Pump On Control Request to Kernel<S201>*/
	BOOL			AconDetectReady;	    /* AC ON Signal Detect Ready Flag(FALSE:Not Ready, TRUE:Ready)<S201>*/
	/* <S21D> Start */
	BOOL			ClinkAnsSig;			/* コンデンサバンク接続監視信号							*/
	BOOL			CmcAnsSig;				/* 突防リレー状態監視信号								*/
	BOOL			ConvRdySig;				/* コンバータレディ信号									*/
	BOOL			ConvMconSig;			/* 突入電流防止用リレー信号								*/
	/* <S21D> End */
} POWER_STATUS;

/*--------------------------------------------------------------------------------------------------*/
/*		中間電位異常検出構造体定義 <S053>															*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct PCOV_ERR_DETECT {
	struct {							/* 中間電位異常検出用パラメータ定義							*/
		BOOL		PcovEna;				/* 中間電位異常検出有効									*/
		ULONG		PcovErrCntLmt;			/* 中間電位異常検出時定数(カウント値)					*/
	} conf;
	struct {							/* 中間電位異常検出用変数定義								*/
		BOOL		Pcov;					/* 中間電位異常信号(正論理)								*/
		ULONG		PcovErrCnt;				/* 中間電位異常カウント									*/
	} var;
} PCOV_ERR_DETECT;


/*--------------------------------------------------------------------------------------------------*/
/*		突入防止抵抗過負荷チェック用構造体定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_RUSH {
	LONG	PowerOnChk;						/* 突入抵抗過負荷 : Last PowerOn						*/
	LONG	PowerOnCnt[2];					/* 突入抵抗過負荷 : Off-->On Counter					*/
	ULONG	PowerOnChkCycle;				/* 突入抵抗過負荷 : Check Cycle Counter					*/
} CHECK_RUSH;


/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷チェック用構造体定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_REGOL {
	struct	{								/* 回生過負荷チェック用定数定義							*/
		LONG	Krgpower;						/* 回生過負荷 : 電力演算ゲイン						*/
		LONG	RgolPowBase;					/* 回生過負荷 : 電力ベース							*/
		LONG	RgolAlmLevel;					/* 回生過負荷 : アラームレベル						*/
		LONG	Kunrgpower;						/* 回生消費電力演算ゲイン(for Un00A)				*/
		LONG	KunrgpowerCycle;				/* 回生消費電力演算ゲイン(for Un143)				*/
		LONG	RgSumSel;						/* 回生過負荷検出用初期値割合選択(PnE02.6,7)		*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct	{								/* 回生過負荷チェック用変数定義							*/
		LONG	RgolPowSum;						/* 回生過負荷 : 電力積算値							*/
		BOOL	LastRgolDetect;					/* 回生過負荷 : 前回起動時の回生過負荷検出フラグ	*/
		BOOL	RgPowUp;						/* 回生過負荷 : 回生電力増加フラグ					*/
		LONG	RgPowerSum;						/* 回生平均値 : 電力積算値							*/
		LONG	RgMeanCycle;					/* 回生平均値 : 電力積算回数						*/
	} var;
} CHECK_REGOL;


/*--------------------------------------------------------------------------------------------------*/
/*		回生異常チェック用構造体定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_REGENE {
	LONG	RegeneErrCnt;					/* 回生異常チェックカウント								*/
	BOOL	RegeAlmSignal;					/* 回生異常アラームフラグ								*/
} CHECK_REGENE;


/*--------------------------------------------------------------------------------------------------*/
/*		回生制御用構造体定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct REGENE_CTRL {
	struct	{								/* 回生制御用定数定義									*/
		ULONG			ResistorCapa;			/* 回生抵抗容量(Pn600)								*/
		BOOL			RegRxIn;				/* 回生抵抗内蔵(PnE0C.0)							*/
		LONG			RegStartFilter;			/* 回生動作開始フィルタ(PnE7A.4,5)	(1=250us)		*/
		LONG			RegStopFilter;			/* 回生動作開始フィルタ(PnE7A.6,7)	(1=250us)		*/
		USHORT			RegOnLevel;				/* 回生Tr ONレベル									*/
		USHORT			RegOffLevel;			/* 回生Tr OFFレベル									*/
		USHORT			RegSyn;					/* 回生同期機能の選択								*//* <S21D> */
		USHORT			RegSynWaitTime;			/* 回生同期時の回生ON待ち時間(PnE9B)				*//* <S21D> */
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	LONG			RegStartCnt;			/* 回生動作開始待ちカウンタ								*/
	LONG			RegStopCnt;				/* 回生動作終了待ちカウンタ								*/
	LONG			RegOnCount[2];			/* 回生トランジスタON時間 0:ScanB更新, 1:ScanC更新		*/
	LONG			dRegOnCount;			/* 差分回生トランジスタON時間			[Count/ScanC]	*/
	BOOL			RegOn;					/* 回生トランジスタON要求								*/
	BOOL			RegAlmSig;				/* 回生異常検出信号										*/
	BOOL			RegAlmMotStop;			/* 回生過負荷アラーム発生後モータ停止検出フラグ			*/
	CHECK_REGOL		RegOL;					/* 回生過負荷チェック用変数へのポインタ(ユニット共通)	*/
	CHECK_REGENE	RegChk;					/* 回生異常チェック用変数(ユニット共通)					*/
	LONG			UnMeanRgPower;			/* 回生消費電力平均値	(ユニット共通：モニタ用)	[%]			*//* <S123> */
} REGENE_CTRL;


/*--------------------------------------------------------------------------------------------------*/
/*		回生制御設定用パラメータ構造体定義															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct REGCFG_PRM {
	USHORT			regres;					/* Pn600 : 回生抵抗容量									*/
	USHORT			exreger;				/* Pn603 : 回生抵抗値(デフォルト)						*/
	USHORT			reger;					/* PnE1A : 回生抵抗値									*/
	USHORT			wre;					/* PnE1C : 回生抵抗容量(デフォルト)						*/
	USHORT			regonlvl;				/* PnE72 : 回生オンレベル								*/
	USHORT			ConvW;					/* PnE7F : コンバータ容量(システム)						*/
} REGCFG_PRM;


/*--------------------------------------------------------------------------------------------------*/
/*		サーボパック内蔵ファン停止検出用構造体定義	<S0A3>											*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CHECK_FANLOCK {
	struct	{							/* 内蔵ファン停止検出用定数定義								*/
		USHORT	FanLockDetectTime;		/* ファン停止検出時間(連続)									*/
		ULONG	FanLockAlmDetectTime;	/* サーボオン時内蔵ファン停止アラーム検出時間(設定値)		*/	/* <S219> */
	} conf;
	struct	{							/* 内蔵ファン停止検出用変数定義								*/
		USHORT	FanLockStartCnt;		/* ファン停止検出開始時間カウント							*/
		USHORT	FanLockTimeCnt;			/* ファン停止検出時間カウント								*/
		BOOL	FanLockWrnFlag;			/* ファン停止ワーニング発生フラグ							*/	/* <S219> */
		ULONG	WrnStartTime;			/* 累積稼働時間[100ms](ファン停止ワーニング発生時)			*/	/* <S219> */
		ULONG	FanLockWrnElapsedTime;	/* サーボオン時内蔵ファン停止アラーム検出残り時間			*/	/* <S219> */
	} var;
} CHECK_FANLOCK;


/*--------------------------------------------------------------------------------------------------*/
/*		サーボ電源管理用構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POW_MNG_HNDL {
	CHECK_MAIN_POWER	MainPowerChecker;			/* 主回路監視用変数(ユニット共通)				*/
	DET_DC_VOLT			DetectDcVolt;				/* DC電源電圧検出用データ(ユニット共通)			*/
	POWER_STATUS		PowerSts;					/* 電源状態(ユニット共通)						*/
	REGENE_CTRL			RegeneCtrl;					/* 回生トランジスタ制御用データ(ユニット共通)	*/
	CHECK_RUSH			Rush;						/* 突入防止抵抗過負荷検出用変数(ユニット共通)	*/
	PCOV_ERR_DETECT		PcovErrDetect;				/* 中間電位異常検出用変数(ユニット共通) <S053>	*/
	CHECK_FANLOCK		FanLock;					/* サーボパック内蔵ファン停止検出用変数			*//* <S0A3> */
} POW_MNG_HNDL;



/*--------------------------------------------------------------------------------------------------*/
/*		消費電力演算用構造体定義	<S020>															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CALC_WATTAGE {
	struct	{								/* 消費電力演算用定数定義								*/
		float			CurrentNorm1;			/* 正規化値1(あとで相応しい名前を検討すること)		*/
		float			CurrentNorm2;			/* 正規化値2(あとで相応しい名前を検討すること)		*/
		float			CurrentNorm3;			/* 正規化値3(あとで相応しい名前を検討すること)		*/
		USHORT			Imax;					/* 最大電流値(アンプとモータで小さい方)				*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* 消費電力演算用定数定義								*/
		float			PowerConsumption;		/* 瞬時消費電力						[W]				*/
		float			PowerConsumption_a;		/* 瞬時消費電力						[W]				*//* <S039> */
	} var;

} CALC_WATTAGE;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct POWER_MONITOR {
	struct	{								/* 消費電力演算用定数定義								*/
		ULONG			TimerCnt1s;				/* 1s計測用カウンタ									*/
		ULONG			TimerCnt10s;			/* 10s計測用カウンタ								*/
		float			CumulativeEnergy;		/* 1秒毎の累積消費電力量			[Wh]			*/
		float			PonCumulativeEnergy;	/* 電源投入からの累積消費電力量		[Wh]			*/
		LONG			UnConsumedPower;		/* Un032 : 消費電力					[W]				*/
		LONG			UnEnergyConsumption;	/* Un033 : 消費電力量 				[Wh]			*/
		LONG			UnCumulativeEnergyI;	/* Un034 : 累積消費電力量(整数部)	[Wh]			*/
		LONG			UnCumulativeEnergyD;	/* Un035 : 累積消費電力量(小数部)	[0.001Wh]		*//* <S0A5> */
	} var;
} POWER_MONITOR;



/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		AC電源欠相検出処理用																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	ACOPEN_DETECT_WAIT			KPI_SCANC_MS( 1000 )		/* 欠相検出時間[ms]					*/

/*--------------------------------------------------------------------------------------------------*/
/*		DC電源監視用																				*/
/*		<S053>以下パラメータを参照する処理に変更。													*/
/*				PnE88: DC充電完了待ち時間[ms]														*/
/*				PnE89: パワーレディ待ち時間[ms]														*/
/*--------------------------------------------------------------------------------------------------*/
//#define	POWCHARG_WAIT				KPI_SCANC_MS( 180 )		/* DC充電完了待ち時間 : 180ms		*/
//#define	POWREADY_WAIT				KPI_SCANC_MS( 240 )		/* パワーレディ待ち時間 : 240ms		*/
/* <S21D> Start */
#define CONVINSIGDISCHERRCHK_TIME	KPI_SCANC_MS( 1000 )	/* A.331（電源監視入力信号異常）急速放電異常検出時間：1s	*/
#define CLINKANSERRCHK_TIME			KPI_SCANC_MS( 500 )		/* A.332（コンデンサバンク接続アンサー異常）検出時間：500ms	*/
#define CMCANSERRCHK_TIME			KPI_SCANC_MS( 500 )		/* A.333（コンデンサバンクMCアンサー異常）検出時間：500ms	*/
#define CMCON_WAIT					KPI_SCANC_MS( 10000 )	/* /CMCON 信号出力待ち時間：10000ms		*/
#define DCOFF_FILTER				100U					/* DC瞬停保持時間フィルタ：100ms		*/
/* <S21D> End */
/*--------------------------------------------------------------------------------------------------*/
/*		DC電源急速放電処理用									<S1C6>								*/
/*--------------------------------------------------------------------------------------------------*/
#define DCVOLT60V					30							/* DcVolt が 60V超					*/
#define DCVOLT10V					5							/* A.331検出用電圧値：10V			*//* <S21D> */
#define DISCH_CHK_TIME				20							/* 急速放電確認時間[ms](x3回)		*/
#define DISCH_CHK_VOLT				10							/* 急速放電確認閾値電圧[V](x3回)	*/
enum {
	DISCH_INIT = 0,		/* 急速放電シーケンス初期状態 												*/
	DISCH_CHK1ST,		/* 急速放電確認第1回 														*/
	DISCH_CHK2ND,		/* 急速放電確認第2回 														*/
	DISCH_CHK3RD,		/* 急速放電確認第3回 														*/
	DISCH_ACTIVE,		/* 急速放電実行中 															*/
	DISCH_FINISH,		/* 急速放電完了		 														*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		DC電源電圧検出用定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	DETECT_DC_NO_OPERATION		0x00

/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷検出用定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_REGOL_NO_OPERATION		0x0000						/* アラームなし(処理不要)			*/
#define	CHK_REGOL_SET_ALARM			0x0001						/* 全軸アラーム出力					*/
#define	CHK_REGOL_SET_WARNING		0x0010						/* 全軸ワーニング出力				*/
#define	CHK_REGOL_CLR_WARNING		0x0100						/* 全軸ワーニングクリア				*/
/* <S21D> Start */
#define REGSYNOFF					0U				/* 回生同期機能を使用しません						*/
#define PNREGSYN					1U				/* PN母線共通システムにおいて回生同期機能を使用します	*/
#define PNEXREG						2U				/* PN母線共通システムにて外部回生機能を利用します		*/
/* <S21D> End */

/*--------------------------------------------------------------------------------------------------*/
/*		突入防止抵抗過負荷検出用定義																*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_RUSH_NO_OPERATION		0x0000						/* アラームなし(処理不要)			*/
#define	CHK_RUSH_SET_ALARM			0x0001						/* 全軸アラーム出力					*/

/*--------------------------------------------------------------------------------------------------*/
/*		回生異常検出用定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_REGENE_NO_OPERATION		0x0000						/* アラームなし(処理不要)			*/
#define	CHK_REGENE_SET_ALARM		0x0001						/* 全軸アラーム出力					*/

/*--------------------------------------------------------------------------------------------------*/
/*		サーボパック内蔵ファン停止検出用定義	<S0A3>												*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_FANLOCK_NO_OPERATION	0x0000						/* アラームなし(処理不要)			*/
#define	CHK_FANLOCK_SET_ALARM		0x0001						/* 全軸アラーム出力					*/
#define	CHK_FANLOCK_SET_WARNING		0x0010						/* 全軸ワーニング出力				*/

/*--------------------------------------------------------------------------------------------------*/
/*		回生過負荷検出用初期値割合選択定義															*/
/*--------------------------------------------------------------------------------------------------*/
#define REGPOWSUM_RATE95PER			0x00						/* 回生過負荷アラームレベルの95％	*/
#define REGPOWSUM_RATE50PER			0x01						/* 回生過負荷アラームレベルの50％	*/

/*--------------------------------------------------------------------------------------------------*/
/*		主回路母線電圧検出方式																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	VDET_NONE					0x00		/* ソフトウェアによる検出なし(SGDM/SGDHタイプ)		*/
#define	VDET_PWM_FIX_FOLLOW			0x01		/* PWM-コンパレータ方式 : 固定値(1)による追従		*/
#define	VDET_PWM_VAR_FOLLOW			0x02		/* PWM-コンパレータ方式 : 可変値(1-3)による追従		*/
#define	VDET_AD						0x03		/* A/D変換方式										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_VDET_PWM_MAX			255							/* 8bit PWM	(2^n - 1)				*/
#define	KPX_DCVOLT_ADMAX			1023

/*--------------------------------------------------------------------------------------------------*/
/*		突入/DB制御方法定義																			*/
/*--------------------------------------------------------------------------------------------------*/
#define DBCTRL_RLY_COMMON			0x00		/* 突入＋ＤＢ：リレー共通							*/
#define DBCTRL_TRIACK_SEPARATE		0x01		/* 突入：リレー；ＤＢ：サイリスタ					*/
#define DBCTRL_RLY_SEPARATE			0x02		/* 突入：リレー；ＤＢ：リレー						*/
#define DBCTRL_TRIACK_COMMON		0x03		/* 突入＋ＤＢ:リレー＋サイリスタ					*/
#define WAIT_RLY_DB					20			/* RLYON出力後DBON出力までのウェイト時間[us] 		*//* <S20E> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
//void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts );							  /* <S0F6> */
void	PcmPowerManager( POW_MNG_HNDL *hPowerManager, BOOL MotStopSts, BOOL BeSts );				  /* <S0F6> */
void	PcmRegeneControl( REGENE_CTRL *pRegCtrl, POWER_STATUS *PowSts, BOOL MotStop );
LONG	PcmDetectDcVolt( DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts );
//LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc, POWER_STATUS *PowSts ); /* <S0F7> */
LONG	PcmCheckOverVoltage( CHECK_MAIN_POWER *PowerChecker, DET_DC_VOLT *pDetectDc,
														POWER_STATUS *PowSts, REGENE_CTRL *RegCtrl ); /* <S0F7> */
LONG	PcmCheckRegeneOverLoad( REGENE_CTRL *pRegCtrl, ULONG *AlmOLInfo, ANLG_UN_MON_DATA *MonData );
void	SetRegeneOverLoadSts( LONG RegOlSts );
BOOL	PcmCheckRushRxOverLoad( CHECK_RUSH *pRush, POWER_STATUS *pPowerSts );
BOOL	PcmCheckRegeneAlarm( REGENE_CTRL *pRegCtrl, BOOL PowerOn );
LONG	PcmCheckFanLock( CHECK_FANLOCK *FanLock, BOOL FanStop, BOOL BbSts );						  /* <S0A3> */
void	PcmInitPowerManager( POW_MNG_HNDL *hPowerManager, ULONG AlmOLInfo );
void	PcmCountRegeneOnTime( POW_MNG_HNDL *hPowerManager );
void	PcmInputPowerStatus( POW_MNG_HNDL *hPowerManager, BOOL Acon1, BOOL RegeneAlm );

//void	IprmcalRegenePowerGain( REGENE_CTRL *RegeneCtrl, BPRMDAT *Bprm, USHORT regonlvl, USHORT	reger, USHORT ConvW);
void	IprmcalRegenePowerGain( REGENE_CTRL *RegeneCtrl, BPRMDAT *Bprm, REGCFG_PRM *RegPrm );
BOOL	PcalRegeneOverLoadLevel( REGENE_CTRL *RegeneCtrl,USHORT regres, USHORT wre, USHORT ConvW );
void	PcmCalculateVdetAdFil( DET_DC_VOLT *pDetectDc, USHORT vdetadfil, LONG SvCycleUs );

void	PcmRlyDbControlProcess( ASIC_HW_IF_STR *pAsicHwReg, POW_MNG_HNDL *PowerManager,
									BOOL BbSts, BOOL DbOn, BOOL KnlMode, BOOL MotorLessTestMode );	  /* <S20E> */
LONG	PcmGetDcVoltage( POWER_STATUS *PowSts );													  /* <S020> */

void	PrmCalcPowerMonitor( CALC_WATTAGE *CalcWattage, USHORT AmpImax, USHORT MotImax );			  /* <S020> */
void	InitializePowerMonitor( POWER_MONITOR *PowMon );											  /* <S020> */
void	CalcPowerMoniter( CALC_WATTAGE *CalcWattage, SHORT PowMonVq, SHORT PowMonIqd,				  /* <S020> *//* <S039> */
							SHORT PowMonVd, SHORT PowMonIdd, LONG DcVolt, BOOL BaseEnable );
void	SetPowerMoniter( POWER_MONITOR *PowMon, float PowerConsumption );							  /* <S020> */
void	ResetPowerConsumption( CALC_WATTAGE *CalcWattage );											  /* <S020> */
float	GetPowerConsumption( CALC_WATTAGE *CalcWattage );											  /* <S020> */

#define	_POWER_MANAGER_H_
#endif	/* _POWER_MANAGER_H_ */
/***************************************** end of file **********************************************/
