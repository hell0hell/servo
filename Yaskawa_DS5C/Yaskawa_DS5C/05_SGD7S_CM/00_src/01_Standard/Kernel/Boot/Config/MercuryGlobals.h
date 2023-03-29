#ifndef GROBAL_DATA_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MercuryGlobals.h : グローバルデータ定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.05.11	Y.Oka															*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#if 0 /* 2012.08.186 Y.Oka Make Mercury Base Software */
#include "BaseControls.h"
#include "SequenceControls.h"
#include "BaseEnableSequence.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "MpfHandler.h"
#include "PdetABSScale.h"
#include "AutoNotchSet.h"
#include "TuneLib.h"
#include "FnMfcTuning.h"
#include "FnVibMonitor.h"
#include "FnInitVibDetLvl.h"
#include "FnOnePrmTuning.h"
#include "FnAtypeVibTune.h"
#include "FnEasyFFT.h"
#include "DataTraceManager.h"
#include "AdvancedAutoTuning.h"
#include "NetworkIf.h"
#else /* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "NetworkIf.h"
#include "HwDeviceIf.h"
#include "BaseControls.h"
#include "SequenceIo.h"
#include "BaseEnableSequence.h"
#include "FunManager.h"
#include "FnMfcTuning.h"
#include "FnVibMonitor.h"
#include "FnInitVibDetLvl.h"
#include "FnOnePrmTuning.h"
#include "FnAtypeVibTune.h"
#include "FnEasyFFT.h"
#include "FnSysConfig.h" /* <S0BE> */
#include "AutoNotchSet.h"
#include "AdvancedAutoTuning.h"
#include "PdetABSScale.h"
#include "JL086ServoIpHw.h"
#include "EepromIF.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"
#include "AlarmManager.h"
#include "OverheatManager.h"
#include "Pls.h"									/* <S00C> Command Pulse Counter Driver			*/
#include "Adc.h"									/* <S00C> A/D Converter Driver					*/
#include "I2cDriver.h"									/* <S00C> EEPROM Driver							*/
#include "XlsAmonDef.h"		/* <S016>仮措置 */
#include "SvidDef.h"		/* <S031> 後で消す */
#include "DetectTemperature.h"	/* <S03A> */
#include "SvOptCardIf.h"							/* <S04D> Servo Option Card If					*/
#include "PnlOp.h"
#include "LifeTimePredictionApi.h"	/* <S154> */
#include "AlmLatch.h"				/* <S154> */
#include "MotorLessTestStr.h"						/* <S1B7> Motor Less Test Mode					*/
#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */


/****************************************************************************************************/
/*		STRUCT DEFINITION(ユニット単位)																*/
/****************************************************************************************************/
/* 処理時間計測用 */
typedef struct _TASK_MEASURE_TIME {
	ULONG	SA_maxTime;		/* ScanA最大処理時間 */
	ULONG	SA_crtTime;		/* ScanA現在処理時間 */
	ULONG	SA_sumTime;		/* ScanA2ms累積処理時間	<S043> */
	ULONG	SAinB_exeTime;		/* ScanB実行中のScanA実行カウント	<S043> */
	ULONG	SAinC_exeTime;		/* ScanC実行中のScanA実行カウント	<S043> */
	ULONG	SB_maxTime;		/* ScanB最大処理時間 */
	ULONG	SB_crtTime;		/* ScanB現在処理時間 */
	ULONG	SB_sumTime;		/* ScanB2ms累積処理時間	<S043> */
	ULONG	SB_exeflg;		/* ScanB実行中フラグ	<S043> */
	ULONG	SBinC_exeTime;		/* ScanC実行中のScanB実行カウント	<S043> */
	ULONG	SC_maxTime;		/* ScanC最大処理時間 */
	ULONG	SC_crtTime;		/* ScanC現在処理時間 */
	ULONG	SC_exeflg;		/* ScanC実行中フラグ	<S043> */
	ULONG	crtTimePer2ms;	/* 2msあたりのScanA,ScanB,ScanC占有処理時間 */
	ULONG	maxTimePer2ms;	/* 2msあたりのScanA,ScanB,ScanC占有最大処理時間 */
	ULONG	ScanSumTime;	/* work */

	ULONG	SCtask_counter;
	ULONG	SCtask_wakeup_time;
	ULONG	SBcountInMS;
	ULONG	SBCount;
	ULONG	SBCountInc;
	ULONG	SACount;		/* ScanA実行回数	<S043> */
} TASK_MEASURE_TIME;

#define	SCANA_EXE_CNT_IN_2MS	(2000000/KPI_SACYCLENS)		/* 2ms中のScanA実行回数	<S043> */
#define	SCANB_EXE_CNT_IN_2MS	(2000000/KPI_SBCYCLENS)		/* 2ms中のScanB実行回数	<S043> */
#define	SCANC_EXE_CNT_IN_2MS	(2/KPI_SCCYCLEMS)			/* 2ms中のScanC実行回数	<S043> */

/* Unit ID Information */
typedef struct MODULE_ID_INFO {
	EEP_HEADER_INFO_T	*IfInfo;		/* IF ID情報 */
	EEP_HEADER_INFO_T	*CoaInfo;		/* COA ID情報 */
	EEP_HEADER_INFO_T	*MbuInfo;		/* MBU ID情報 */
	EEP_HEADER_INFO_T	SdaInfo;		/* SDA ID情報 */
//<S053>	HW_AXIS_MAP			*AsicNoMap;		/* 軸番号とJL077ASIC番頭とのマップ情報  */
} MODULE_ID_INFO;


/****************************************************************************************************/
/*		STRUCT DEFINITION(ユニット内軸間インタフェース)		モータキャリア周波数切り替え対応<S117>								*/
/****************************************************************************************************/
/*  */
#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )
typedef struct AXIS_IF {
	SHORT	pwm_ax[MAX_AXIS_NUMBER];		/* 各軸の設定PWMキャリア周波数							*/
	SHORT	pwm_unit;						/* 全体の設定PWMキャリア周波数							*/
}AXIS_IF;
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */



/****************************************************************************************************/
/*		STRUCT DEFINITION(軸単位)																	*/
/****************************************************************************************************/
typedef	struct AXIS_HANDLE {
	LONG				SlotNo;				/* 軸番号(=MB基板スロット番号)							*/
	LONG				AxisNo;				/* 軸アドレス(論理軸番号)								*/

	/* Hardware abstraction layer */
	MODULE_ID_INFO		*ModuleID;			/* Module ID information */
	MENCV				*MencV;				/* モータエンコーダデータへのポインタ					*/
	MENCV				*FencV;				/* フルクエンコーダデータ								*/
	ASICS				*SvAsicRegs;		/* ASICアドレステーブルへのポインタ						*/
	SHAL_LATCH_STS_ADR	*LtStsAdr;			/* EXTラッチ信号用 										*/
//	OH_INFO				*OhInfo;			/* 基板温度検出用 										*//* <S03A> */
	PLSIFV 				*pPlsIfV;			/* <S00C> Command Pulse Interface						*/
	CADC				*pAdc;				/* <S00C> A/D Converter Interface						*/
	RWALLMD 			*pRwAllMng;			/* <S00C> Parameter Download							*/
	RWPDLMD 			*pRwPdlMng;			/* <S031> Lcd Parameter Copy							*/

	/* ベースサーボ制御用データ(ScanA) */
	BASE_LOOPCTRLS		*BaseLoops;			/* 制御ループ演算用データ								*/
	CTRL_LOOP_OUT		*CtrlLoopOut;		/* ScanA出力データ										*/

	/* ベースサーボ制御用データ(ScanB) */
	BASE_CTRL_HNDL		*BaseControls;		/* 基本制御用共有データ									*/
	SERVO_CONTROL_IF	*SvControlIf;		/* M3 I/Fデータ											*/
	BASE_CTRL_OUT		*BaseCtrlOut;		/* ScanB出力データ										*/
	DETVIB				*DetVib;			/* 振動検出用変数										*/
	USHORT				*TableRefBuffer;	/* テーブル運転指令バッファへのポインタ					*/
	
	MOTSPDMAFIL			*MotSpdMafil;		/* モータ速度検出用移動平均演算用変数					*/

	PDET_ABSCONTROL		*PdetAbsSeq;		/* ABSスケール磁極検出用変数							*/
	MPFIND				*MpFind;			/* 磁極検出動作用変数									*/
	
	/* サーボシーケンス制御用データ(ScanC) */
	SEQ_IO_HNDL			*IoSignals;			/* シーケンスI/O */
	SEQ_CTRL_OUT		*SeqCtrlOut;		/* ScanC出力データ										*/
	BE_SEQ_HNDL			*BeSequence;		/* Base Enable Sequence Handle							*/
	CHECK_ALARM			*CheckAlarm;		/* アラーム検出処理用構造体								*/
	SEQ_MOT_STS			*SeqMotSts;			/* ScanCにて取得・作成するモータ状態					*/
	REMVIBFREQ			*RemVibFreq;		/* 残留振動周波数モニタ用データ							*/
	CALC_WATTAGE		*CalcWattage;		/* <S020> 消費電力演算用データ(軸毎)					*/
	ENVMON				*EnvMonitor;		/* 設置環境モニタデータ(軸毎)							*//* <S0ED> */
	POWER_MONITOR		*PowerMonitor;		/* <S020> 消費電力モニタ変数(ユニット共通)				*/
	POW_MNG_HNDL		*PowerManager;		/* 電源管理用データ(ユニット共通)						*/
	CHECK_OH			*OverHeat;			/* オーバーヒートチェック用変数(ユニット共通)			*//* <S0ED> */
	LIFETIME_HANDLE		*hLifeTime;			/* 寿命予測機能											*//* <S154> */
	DBLIFETIME_HADNLE	*hDBLifeTime;		/* DB寿命予測機能										*//* <S180> */
	ALMLATCH_HNDL		*hAlmLatchHndl;		/* アラームラッチ用データ								*//* <S154> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	PNLOP_DATA_HANDLE	*PnlOpLedDataHndl;	/* ネットワーク用パネオペ構造体							*//* <S0A1> */
#endif

	/* Network */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* AnalogIFも下記構造体を利用するように変更する？			*/
	NETIF_AXIS_HANDLE	*NetAxisIf;			/* Network Interface Handle for each axis 				*/
	NETIF_SYS_HANDLE	*NetNodeIf;			/* Network Interface Handle for node device 			*/
#endif
	NETMSG_IF			*NetMsgIf;			/* Network Messages IF									*/

	/* オプションモジュール	*//* <S04D> */
	OPTCARD_HNDL		*OptCardHdl;		/* オプションモジュール構造体へのポインタ				*/
	
	/* パラメータデータ */
	REG_MANAGER_HANDLE	*RegManager;		/* Register Manager */
	UNI_PRM_STR			*UniPrms;			/* パラメータハンドルへのポインタ						*/
	ALMMAN_HANDLE		*AlmManager;		/* アラーム制御用構造体									*/

	/* Fn機能データ */
	FUN_CMN_CONTROL		*FnCmnCtrl;			/* Fn機能処理用データへのポインタ						*/
	FUN_MANAGER			*FnManager;			/* Fn Managerハンドル									*/
	FUN_AXCOMMON		*FnAxCommons;		/* Fn Manager用ユニット共通ハンドル						*/

	/* Utility Functions */
	ANOTCHSEQ			*ANotchSeq;			/* 自動ノッチシーケンス用構造体							*/
	AUTONOTCH			*ANotch;			/* 自動ノッチ設定用構造体								*/
	FFTANALYZE			*FftAna;			/* 振動周波数解析(Online FFT)用構造体					*/
	MFCTUNE				*MfcTune;			/* モデルゲイン調整用構造体								*/
	VIBMON				*VibMon;			/* オンライン振動モニタ用構造体							*/
	INIVIBDET			*IniVibDet;			/* 振動検出レベル初期化用構造体							*/
	ADATHNDL			*AdatHndl;			/* Advanced Auto Tuning	Handle							*/
	ONEPRMTUNE			*OnePrmTune;		/* ワンパラメータチューニング用構造体					*/
	AVIBTUNE			*AvibTune;			/* A型制振制御調整用構造体								*/
	EASYFFT				*EasyFFT;			/* Easy FFT用構造体										*/
	OPTCLR				*OptClr;			/* オプションカード検出クリア用構造体					*//* <S0BE> */

	TEMP_MANAGER		*TempManager;		/* 温度検出データへのポインタ							*//* <S03A> */
	ANLG_UN_MON_DATA	*UnMonitor;			/* Amon/Unモニタデータへのポインタ						*/
	TASK_MEASURE_TIME	*TaskProcessTime;	/* 処理時間モニタ用 */

#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )			/* モータキャリア周波数切り替え対応<S117> */
	AXIS_IF				*AxisIf;			/* ユニット内軸間インタフェース構造体			*/
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) */
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */

	/* データトレース関連 *///kira_101027
	TRCHNDL				*TrcHndl;			/* データトレースハンドル */
	USHORT				*TraceBuffer;		/* データトレースバッファへのポインタ */

	/* アナログモニタ関連 *//* <S016> 仮措置 */
	AMONOUT				*AmonOut;			/* アナログモニタ出力情報へのポインタ	*/
	UCHAR				*ScanB250us;		/* ScanB 250us フラグ				*/

	/* モータレステストモード関連	*//* <S1B7> */
	MOTLESSTST			*MotorLessTest;		/* モータレステストIF定義へのポインタ					*/

#if 0 /* 2012.08.06 Y.Oka Make Mercury Base Software */
	/* アプリケーション */
	TRIGIO_INFO			*TrigInfo;			/* TrigIO制御用データ 									*/
#endif /* 2012.08.06 Y.Oka Make Mercury Base Software */

#if defined(_VC_DEBUG_) /* 2012.08.23 Y.Oka Make Mercury Base Software */
	UCHAR				*pDummyEeprom;
#endif /* #if defined(_VC_DEBUG_) */ /* 2012.08.23 Y.Oka Make Mercury Base Software */
	
	OPACCSV 			*pAccessOp;			/* <S166> Operator Access Control */

} AXIS_HANDLE;


void* GetAxisHandle(UINT axis_number);
AXIS_HANDLE *InitializeAxisGlobals( void );


extern	LONG	NumberOfAxes;				/* 軸数設定						*/
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
extern	LONG	ScanAStartCtr;						/* ScanA処理カウンタ							*/
extern	LONG	ScanBStartCtr;						/* ScanB処理カウンタ							*/
extern	LONG	ScanCStartCtr;						/* ScanC処理カウンタ							*/
#endif
extern DWORDX	ReadDummy;					/* <S074> Dummy Variable for Read */

#define GROBAL_DATA_H
#endif /* GROBAL_DATA_H */

//#define _INFO_REG_CHECK_	/* 機種情報レジスタ用コンパイルスイッチ */	/* <S061> */


/***************************************** end of file **********************************************/
