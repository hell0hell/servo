/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControls.h : Local Header File for ScanB Software										*/
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
#ifndef	_BASE_CONTROLS_H_
#define	_BASE_CONTROLS_H_


#include "KLib.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "EncData.h"
#include "JL086ServoIpHw.h"
#include "BaseCmnStruct.h"
#include "CtrlLoopHandler.h"
#include "GselHandler.h"
#include "MfcHandler.h"
#include "PcmdFilter.h" /* <S065> */
#include "ResVibCtrl.h"
#include "TableDrive.h"
#include "PnPrmStruct.h"
#include "Bprm.h"
#include "MicroPrmCalc.h"
#include "ProgramJog.h"
#include "JatOffLine.h"
#include "CtrlPrmTransducer.h"
#include "Pls.h"
#include "MotorLessTestStr.h"	/* <S1B7> */

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Bit No. Definition for LatchSel (ULONG LatchSel)											*/
/*--------------------------------------------------------------------------------------------------*/
#define	C_PHASE_LATCH		0x01

/* <S14B> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		速度リップル補償機能の有効化しきい値条件													*/
/*--------------------------------------------------------------------------------------------------*/
#define	CMD_SPD		0x00
#define	FB_SPD		0x01
/* <S14B> End */

/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		新調整レス用パラメータ変数定義 <S1A5> 														*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _TL2PRM {
	LONG		KrlpfResSup;		/* 機械共振抑制フィルタゲイン									*/
	LONG		KrlpfDisObs1[5];	/* 外乱オブザーバ2次フィルタゲイン(バンドパスフィルタ用)		*/
	LONG		KrlpfDisObs2[5];	/* 外乱オブザーバ2次フィルタゲイン(ハイパスフィルタ用)			*/
	LONG		KrCurfreq;			/* 電流ループ近似モデル周波数									*/
	LONG		KrCurZeta1;			/* 電流ループ近似モデル減衰係数1								*/
	LONG		KrCurZeta2;			/* 電流ループ近似モデル減衰係数2								*/
	KSGAIN		KrlpfCur[5];		/* 電流ループ近似モデル2次フィルタゲイン						*/
} TL2PRM;

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス用パラメータ変数定義																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_PRM {
	USHORT		kv;				/* 速度ループゲイン									[0.1Hz]			*/
	USHORT		kv2;			/* 補正後速度ループゲイン							[0.1Hz]			*/
	USHORT		jrate;			/* 負荷慣性モーメント/質量比						[%]				*/
	USHORT		Ti;				/* 速度ループ積分時定数								[0.01ms]		*/
	USHORT		kp;				/* 位置ループゲイン									[0.1/s]			*/
	USHORT		klpf;			/* トルク補償後トルク指令フィルタ時定数				[0.01ms]		*/
	USHORT		f1;				/* 外乱オブザーバローパスフィルタ1周波数			[0.1Hz]			*/
	USHORT		f1Comp;			/* 補正後外乱オブザーバローパスフィルタ1周波数		[0.1Hz]			*/
	USHORT		f2;				/* 外乱オブザーバローパスフィルタ2周波数			[0.1Hz]			*/
	USHORT		kj_dist;		/* 外乱オブザーバイナーシャゲイン					[%]				*/
	USHORT		kj_dist2;		/* 補正後外乱オブザーバイナーシャゲイン				[%]				*/
	USHORT		kd_dist;		/* 外乱オブザーバ係数								[%]				*/
	USHORT		ks_vobs;		/* 位相補償速度オブザーバゲイン						[Hz]			*/
	USHORT		kj_vobs;		/* 位相補償速度オブザーバイナーシャゲイン			[%]				*/
	USHORT		kj_vobs2;		/* 補正後位相補償速度オブザーバイナーシャゲイン		[%]				*/
	USHORT		vobsLpf;		/* 位相補償速度オブザーバLPF時定数					[0.01ms]		*/
	USHORT		Stiff;			/* ロバスト性減低率									[%]				*/
	USHORT		F1rate;			/* 外乱オブザーバフィルタ周波数1低減率				[%]				*/
	LONG		vffPhCmpFrq1;	/* 位相補償フィルタ周波数1							[0.1Hz]			*/
	LONG		vffPhCmpFrq2;	/* 位相補償フィルタ周波数2							[0.1Hz]			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0							/* <S1A5> */
	LONG		KrlpfResSup;	/* 新調整レス 機械共振抑制フィルタゲイン							*/
	LONG		KrlpfDisObs1[5];/* 新調整レス 外乱オブザーバ2次フィルタゲイン(バンドパスフィルタ用)	*/
	LONG		KrlpfDisObs2[5];/* 新調整レス 外乱オブザーバ2次フィルタゲイン(ハイパスフィルタ用)	*/
	LONG		KrCurfreq;		/* 新調整レス 電流ループ近似モデル周波数							*/
	LONG		KrCurZeta1;		/* 新調整レス 電流ループ近似モデル減衰係数1							*/
	LONG		KrCurZeta2;		/* 新調整レス 電流ループ近似モデル減衰係数2							*/
	KSGAIN		KrlpfCur[5];	/* 新調整レス 電流ループ近似モデル2次フィルタゲイン					*//*<S00A>*/
#endif
	TL2PRM		Tl2Prm;			/* <S1A5> 															*/
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス用ロバスト補償器演算用変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TUNELESS_DRCOM {				/* ロバスト補償器用パラメータ定義						*/
	struct {								/* 調整レス用レジスタ定義								*/
#if (FLOAT_USE==TRUE)
		float		KVfbFil;					/* 調整レス用速度ＦＢフィルタゲイン					*/
		float		KCmpOutlpf;					/* ロバスト補償器トルク補償ローパスフィルタゲイン	*/
		float		Krlpf1;						/* ロバスト補償器ローパスフィルタゲイン1			*/
		float		Krlpf2;						/* ロバスト補償器ローパスフィルタゲイン2			*/
		float		Krd;						/* ロバスト補償トルク係数							*/
		float		Krsj;						/* ロバスト補償器トルク補正ゲイン					*/
#else
		LONG		KVfbFil;					/* 調整レス用速度ＦＢフィルタゲイン					*/
		LONG		KCmpOutlpf;					/* ロバスト補償器トルク補償ローパスフィルタゲイン	*/
		LONG		Krlpf1;						/* ロバスト補償器ローパスフィルタゲイン1			*/
		LONG		Krlpf2;						/* ロバスト補償器ローパスフィルタゲイン2			*/
		LONG		Krd;						/* ロバスト補償トルク係数							*/
		LONG		Krsj;						/* ロバスト補償器トルク補正ゲイン					*/
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	TL2PRM	*pTl2;						/* Tuningless Parameter <S1A5> 								*/
	/*----------------------------------------------------------------------------------------------*/
	void	(*RobustCompensator)		/* Tuneingless Robust Compensator <S1A5> 					*/
			( void *pdrcmp, 			/* 															*/
			  LONG motspd, 				/* 															*/
			  LONG trqref );			/* 															*/
	/*----------------------------------------------------------------------------------------------*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 調整レス用レジスタ定義								*/
		LONG		DisTrq;						/* 外乱トルク推定値									*/
		LONG		DisTrqCmp;					/* 外乱トルク補償値			[2^24/MaxTrq]			*/
		LONG		TrqLpfo;					/* ロバスト補償器ローパスフィルタ出力値				*/
		LONG		TrqLpfo_old;				/* ロバスト補償器前回ローパスフィルタ出力値			*/
		LONG		VelLpfo1;					/* ロバスト補償器差分速度ローパスフィルタ出力値1	*/
		LONG		VelLpfo2;					/* ロバスト補償器差分速度ローパスフィルタ出力値2	*/
		BOOL		TrqFilClrReq;				/* トルク指令フィルタ積分初期化要求					*/
		BOOL		CurrentStatus;				/* 有効/無効切り替え判定用現在状態フラグ			*/
		BOOL		LastStatus;					/* 有効/無効切り替え判定用前回状態フラグ			*/
	/*----------------------------------------------------------------------------------------------*/
		LONG		Krlpftmp1[4];				/* <S1A5> 2次LPFフィルタ内部演算用変数				*/
		LONG		Krlpftmp2[4];				/* <S1A5> 2次LPFフィルタ内部演算用変数				*/
		LONG		Krlpftmp3[4];				/* <S1A5> 2次LPFフィルタ内部演算用変数				*/
	/*----------------------------------------------------------------------------------------------*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_DRCOMP;

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス用位相補償速度オブザーバ変数定義													*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct TUNELESS_EHVOBS {			/* 位相補償速度オブザーバ用パラメータ定義				*/
	struct {								/* 調整レス用レジスタ定義								*/
#if (FLOAT_USE==TRUE)
		float		Kstf1;						/* 位相補償VOBSゲイン1								*/
		float		Kstf2;						/* 位相補償VOBSゲイン2								*/
		float		Kstf3;						/* 位相補償VOBSゲイン3								*/
		float		Kj;							/* 位相補償VOBS位相補償ゲイン						*/
		float		Lpf;						/* 位相補償VOBSローパスフィルタゲイン (LPF + Pn401）*/
		float		Tf;							/* 位相補償VOBSローパスフィルタ時定数				*/
#else
		LONG		Kstf1;						/* 位相補償VOBSゲイン1								*/
		LONG		Kstf2;						/* 位相補償VOBSゲイン2								*/
		LONG		Kstf3;						/* 位相補償VOBSゲイン3								*/
		LONG		Kj;							/* 位相補償VOBS位相補償ゲイン						*/
		LONG		Lpf;						/* 位相補償VOBSローパスフィルタゲイン (LPF + Pn401）*/
		LONG		Tf;							/* 位相補償VOBSローパスフィルタ時定数				*/
#endif /* FLOAT_USE */
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 調整レス用レジスタ定義								*/
		KSGAIN		Ivar64[2];					/* 積分演算用										*//*<S00A>*/
		LONG		EhSpdObs;					/* 速度推定値										*/
		LONG		Wkbf1;						/* ローパスフィルタ出力 							*/
		LONG		Wkbf2;						/* 位相補償VOBS演算用ワーク変数						*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_EHVOBS;

/*--------------------------------------------------------------------------------------------------*/
/*		調整レス制御用構造体定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct TUNELESS_CTRL {
	TUNELESS_PRM		TuneLessPrm;		/* 調整レス用ゲインパラメータ構造体						*/
	TUNELESS_DRCOMP		Drcomp;				/* 調整レス用ロバスト補償演算変数						*/
	TUNELESS_EHVOBS		EhVobs;				/* 調整レス用位相補償速度OBS演算変数					*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 調整レス用レジスタ定義								*/
		USHORT			DatGnLevel;				/* 0x20E0 : DATゲインレべル							*/
		USHORT			DatJratLevel;			/* 0x20E1 : DAT負荷レべル							*/
	} OpeReg;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 調整レス用パラメータ定義								*/
		BOOL			TuningLessUse;			/* 調整レス使用/不使用設定							*/
		BOOL			TuningLessEx;			/* 新調整レス設定									*/
		BOOL			TuningLessVComp;		/* 調整レス位相進み補償設定							*/
		LONG			TuningLessType;			/* 調整レス処理タイプ								*/
		LONG			TuningLessMode;			/* 調整レスモード									*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 調整レス用変数定義									*/
		LONG			SpdFbFilo;				/* 速度FBフィルタ出力								*/
		BOOL			TuneLessAct;			/* 調整レス要求										*/
		BOOL			TuneLessVCompReq;		/* 調整レス位相進み補償要求							*/
		BOOL			TuneLessInvldReq;		/* 調整レス強制無効要求								*/
		BOOL			TuningFuncRun;			/* Fn200実行中フラグ								*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} TUNELESS_CTRL;



/*--------------------------------------------------------------------------------------------------*/
/*		外乱オブザーバ用変数定義									<V221>							*/
/*--------------------------------------------------------------------------------------------------*/
#if 0		/* <S09B> */
typedef	struct DOBS_PRM {					/* 外乱オブザーバ用パラメータ定義						*/
	KSGAIN				Ks;						/* 外乱オブザーバゲイン								*/
	KSGAIN				Kj;						/* 外乱オブザーバイナーシャゲイン					*/
	KSGAIN				Ksj;					/* 外乱オブザーバトルクゲイン						*/
	KSGAIN				Kd;						/* 外乱トルク係数									*/
	KSGAIN				Klpf;					/* 外乱オブザーバローパスフィルタゲイン				*/
} DOBS_PRM;
#else
typedef GSELDOBS DOBS_PRM;
#endif

typedef	struct DOBS_CTRL {					/* 外乱オブザーバ用パラメータ＆変数定義					*/
//<S09B>	DOBS_PRM			DobsPrm[3];				/* 外乱オブザーバ用パラメータ						*/
	DOBS_PRM			*pDobsPrm;				/* 外乱オブザーバ用パラメータ <S09B>				*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 外乱オブザーバ用変数データ定義						*/
		BOOL			DobsAct;				/* 外乱オブザーバ実行要求							*/
		KSGAIN			Ivar64[2];				/* 積分演算用										*//*<S00A>*/
		LONG			SpdObs;					/* 速度推定値										*/
		LONG			DisTrq;					/* 外乱トルク推定値									*/
		LONG			Hpftmp;					/* ハイパスフィルタ演算用							*/
		LONG			DTrqLpfo;				/* ローパスフィルタ出力外乱トルク推定値				*/
		LONG			DisTrqCmp;				/* 外乱トルク補償値			[2^24/MaxTrq]			*/
		UCHAR			CurrentStatus;			/* 有効/無効切り替え判定用現在状態フラグ			*/
		UCHAR			LastStatus;				/* 有効/無効切り替え判定用前回状態フラグ			*/
		UCHAR			ExchangeSts;			/* 機能有効・無効切り替え状態(ショック軽減用)		*/
		UCHAR			Dummy;					/* for Alignment									*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
} DOBS_CTRL;



/*--------------------------------------------------------------------------------------------------*/
/*		位相補償速度オブザーバ(VOBS)変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct EHVOBS_PRM {					/* 位相補償速度オブザーバ用パラメータ定義				*/
#if (FLOAT_USE==TRUE)
	float				Kstf1;					/* 位相補償VOBSゲイン1(Pn127)						*/
	float				Kstf2;					/* 位相補償VOBSゲイン2(Pn127)						*/
	float				Kstf3;					/* 位相補償VOBSゲイン3(Pn127)						*/
	float				Kj;						/* 位相補償VOBS位相補償ゲイン（Pn128）				*/
	float				Lpf;					/* 位相補償VOBSローパスフィルタゲイン(Pn13B+Pn401)	*/
	float				Tf;						/* 位相補償VOBSローパスフィルタ時定数				*/
	float				KTrqReflpf;				/* VOBSトルク指令ローパスフィルタゲイン				*/
#else
	
	LONG				Kstf1;					/* 位相補償VOBSゲイン1(Pn127)						*/
	LONG				Kstf2;					/* 位相補償VOBSゲイン2(Pn127)						*/
	LONG				Kstf3;					/* 位相補償VOBSゲイン3(Pn127)						*/
	LONG				Kj;						/* 位相補償VOBS位相補償ゲイン（Pn128）				*/
	LONG				Lpf;					/* 位相補償VOBSローパスフィルタゲイン(Pn13B+Pn401)	*/
	LONG				Tf;						/* 位相補償VOBSローパスフィルタ時定数				*/
	LONG				KTrqReflpf;				/* VOBSトルク指令ローパスフィルタゲイン				*/
#endif /* FLOAT_USE */
} EHVOBS_PRM;

typedef	struct EHVOBS_CTRL {				/* 位相補償速度オブザーバ用パラメータ＆変数定義			*/
	EHVOBS_PRM			EhVobsPrm;				/* 位相補償速度オブザーバ用パラメータ				*/
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 外乱オブザーバ用変数データ定義						*/
		BOOL			EhVobsUse;				/* 位相補償速度オブザーバ有効/無効選択				*/
	} conf;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* 位相補償速度オブザーバ用変数データ定義				*/
		KSGAIN			Ivar64[2];				/* 積分演算用										*//*<S00A>*/
		LONG			AccSum;					/* 加速度積算値(速度推定値)							*/
		LONG			EhSpdObs;				/* 速度推定値										*/
		LONG			Wkbf1;					/* ローパスフィルタ出力 							*/
		LONG			Wkbf2;					/* 位相補償VOBS演算用ワーク変数						*/
		LONG			TrqRefLpfo;				/* VOBSトルク指令フィルタ推定値						*/
	} var;
} EHVOBS_CTRL;

/* <S14B> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		速度リップル補償用変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	RPLCMP_ORDER_MECHA	12				/* 速度リップル補償最大次数(機械角)					*/
#define	RPLCMP_ORDER_ELE	6				/* 速度リップル補償最大次数(電気角)					*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct RPLCMP_PRM_MECHA {		/* 速度リップル補償パラメータ定義(機械角)				*/
	LONG	CompGain;							/* 速度リップル補償ゲイン			[%]				*/
	USHORT	CompFrq[RPLCMP_ORDER_MECHA];		/* 速度リップル補償成分				[-]				*/
	LONG	CompAmpValue[RPLCMP_ORDER_MECHA];	/* 速度リップル補償振幅				[15000/MaxTrq]	*/
	USHORT	PhaseOfset[RPLCMP_ORDER_MECHA];		/* 速度リップル補償位相				[65536/360deg]	*/
}RPLCMP_PRM_MECHA;

typedef	struct RPLCMP_PRM_ELE {			/* 速度リップル補償パラメータ定義(電気角)				*/
	LONG	CompGain;							/* 速度リップル補償ゲイン			[%]				*/
	USHORT	CompFrq[RPLCMP_ORDER_ELE];			/* 速度リップル補償成分				[-]				*/
	LONG	CompAmpValue[RPLCMP_ORDER_ELE];		/* 速度リップル補償振幅				[15000/MaxTrq]	*/
	USHORT	PhaseOfset[RPLCMP_ORDER_ELE];		/* 速度リップル補償位相				[65536/360deg]	*/
}RPLCMP_PRM_ELE;

typedef	struct RPLCMP_PRM {				/* 速度リップル補償パラメータ定義						*/
	RPLCMP_PRM_MECHA	RplCmpPrmMecha;				/* 速度リップル補償パラメータ（機械角）			*/
	RPLCMP_PRM_ELE		RplCmpPrmEle;				/* 速度リップル補償パラメータ（電気角）			*/
	/*----------------------------------------------------------------------------------------------*/
	BOOL	RplCmpUse;								/* 速度リップル補償使用/不使用設定				*/
	BOOL	RippleWrnMode;							/* 速度リップル補償情報不一致ワーニング検出モード(0:検出する,1:検出しない)*//* <S1FD> */
	BOOL	RippleLstWrnMode;						/* 速度リップル補償情報不一致ワーニング検出モード前回値(200Dh用)*//* <S1FD> */
//	BOOL	RplCmpMode6f;							/* 速度リップル補償モード選択(1:1f～6f/0:1f～3f)*/
//	BOOL	RplCmpDobsMon;							/* モニタ出力選択(外乱OBSと共用)				*/
//	USHORT	RplSpAnalyzeMode;						/* 速度リップルスペクトル解析モード選択			*/
	USHORT	RplCompGainEle;							/* 速度リップル補償ゲイン(電気角)				*/	/* <S14B> */
	USHORT	RplCompCycleEle[RPLCMP_ORDER_ELE];		/* 速度リップル補償成分周期(電気角)				*/	/* <S14B> */
	SHORT	RplCompAmplitudeEle[RPLCMP_ORDER_ELE];	/* 速度リップル補償成分振幅(電気角)				*/	/* <S14B> */
	USHORT	RplCompPhaseEle[RPLCMP_ORDER_ELE];		/* 速度リップル補償成分位相(電気角)				*/	/* <S14B> */
} RPLCMP_PRM;
/*--------------------------------------------------------------------------------------------------*/
typedef	struct {						/* 速度リップル補償用パラメータ＆変数定義				*/
	RPLCMP_PRM			RplCmpPrm;					/* 速度リップル補償用パラメータ					*/
/*--------------------------------------------------------------------------------------------------*/
	struct {										/* 速度リップル補償用変数データ定義				*/
		LONG			TrqCompAll;					/* 速度リップル補償トルク		   [2^24/MaxTrq]*/
		BOOL			RippleWrnState;				/* 速度リップル補償情報ワーニング状態(0:A.942発生させる,1:正常)	*/	/* <S1FD> */
		BOOL			RippleCmpExe;				/* 速度リップル補償機能の有効化しきい値条件確認 */
//		LONG			TrqComp[RPLCMP_ORDER];		/* 速度リップル補償トルク(次数成分)[2^24/MaxTrq]*/
	} var;
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RippleCmpState;			/* 速度リップル補償情報書込み確認(00H:完了,01H:未完了,02H:失敗)	*/
	BOOL	VelRipplePhaseReady;	/* 速度リップル補償用 磁極検出完了フラグ						*/
	LONG	RippleTrqCompMon;		/* 速度リップル補正トルク(モニタ用)	[2^24/MaxTrq]				*/
	BOOL	RippleCompMon;			/* 速度リップル補償中信号(Un110 bit23:0=補正無効,1=補正中)		*/
	USHORT	RippleCmpThresholdSel;	/* 速度リップル補償機能の有効化しきい値条件選択					*/
	LONG	TrqRefBeforeComp;		/* 補正前トルク指令(Trace・AnaMon用)[2^24/MaxTrq]				*/
	LONG	VelRippleCompSpd;		/* 速度リップル補償有効速度[min-1]								*/
} RPLCMP;
/* <S14B> End */

/*--------------------------------------------------------------------------------------------------*/
/*		制御モード切替定義																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct CTRL_CHANGE {
	LONG	dPosFbki;				/* 位置増分値(前回応答時からの増分値)	[pulse]					*/
	LONG	CmdErr;					/* 位置偏差補正値 [指令単位]									*/
	LONG	SpdFbki_l;				/* モータ速度(前回値)					[2^24/OvrSpd]			*/
	LONG	SpdFbki_a;				/* モータ速度(前回と今回の平均値)		[2^24/OvrSpd]			*/
} CTRL_CHANGE;


/*--------------------------------------------------------------------------------------------------*/
/*		ScanB用ノード共通変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct SERVO_CONTROL_IF {
	SV_OPTION_IF	CmdOptionIf;			/* SV-OP I/Fハンドルへのポインタ						*/
	BOOL			CmdReady;				/* 指令受付可能フラグ */

	CMD_CTRLBITS	CmdCtrl;				/* 制御指令												*/
	LONG			CmdSeqBit;				/* シーケンス制御指令									*/
	LONG			CmdLtBit;				/* ラッチ要求											*/
	RSP_CTRLBITS	RspCtrl;				/* 制御状態 */
	LONG			RspSigBit;				/* シーケンス信号										*/
	LONG			RspLtBit;				/* ラッチ状態											*/

	BOOL			LtReqCphs;
	BOOL			LtReqExt1;
	BOOL			LtReqExt2;
	BOOL			LtReqExt3;
	BOOL			ExtLtEnable[3];
	SHAL_LATCH_STS	ExtLtSts[3];

	LONG			MonCphaLpos[2];			/* C相ラッチ位置	[指令単位]							*/
	LONG			MonExt1Lpos[2];			/* EXT1ラッチ位置	[指令単位]							*/
	LONG			MonExt2Lpos[2];			/* EXT2ラッチ位置	[指令単位]							*/
	LONG			MonExt3Lpos[2];			/* EXT3ラッチ位置	[指令単位]							*/

//	CTRL_CHANGE		CtrlChange;				/* 制御切替変数											*/
//	LONG			VirtualPositionError[2];/* 仮想位置偏差		[指令単位]	(通信応答タイミング)	*/
//	LONG			FbPositionFB[2];		/* FB位置			[FB単位]	(通信応答タイミング)	*/

	ULONG			NetSout;				/* 通信I/F出力信号指令									*/
	LONG			NetCtrlMcmd;			/* 制御モード(通信からの指令)							*/
	LONG			NetTrqFFC;				/* トルクFF指令		[2^24/MaxTrq]						*/
	LONG			NetTrqRef;				/* トルク指令		[2^24/MaxTrq]						*/
	LONG			NetSpdLmt;				/* 速度制限			[2^24/OvrSpd]						*/
	LONG			NetSpdRef;				/* 速度指令			[2^24/OvrSpd]						*/
	LONG			NetSpdFFC;				/* 速度ＦＦ指令		[2^24/OvrSpd]						*/
	LONG			NetPosRef;				/* 位置指令			[指令単位]							*/

	ULONG			NetIfMonInSignal1;		/* SIポートetc入力信号状態 for Network	単軸アンプ		*//* <S0AC> *//* <S1C0>:rename */
	ULONG			NetIfMonInSignal2;		/* SIポートetc入力信号状態 for Network	多軸アンプ		*//* <S1C0> */

	LONG			*MonRamTable[0x50];	/* 拡張モニタ用変数格納テーブル					*//* <S1D7>：要素数変更 */
	struct {								/* 拡張モニタ選択										*/
		LONG		MonSel1;				/* 拡張モニタ選択1										*/
		LONG		MonSel2;				/* 拡張モニタ選択2										*/
		LONG		MonSel3;				/* 拡張モニタ選択3										*/
		LONG		MonSel4;				/* 拡張モニタ選択4										*/
		LONG		MonSel5;				/* 拡張モニタ選択5										*/
		LONG		MonSel6;				/* 拡張モニタ選択6										*/
		LONG		MonSel7;				/* 拡張モニタ選択7										*/
		LONG		MonSel8;				/* 拡張モニタ選択8										*/
	} MonSel;
} SERVO_CONTROL_IF;


typedef	struct SETTLINGTIME {			/* 整定時間関連処理用変数定義								*/
	BOOL	StlgCalcReq;					/* 整定時間算出中フラグ									*/
	BOOL	StlgLastCoinSts;				/* COIN信号前回値										*/
	BOOL	StlgCoinLatch;					/* COINラッチ状態										*/
	ULONG	ProcessTimer;					/* 整定時間測定処理実行タイミング検出用タイマ			*/
	LONG	StlgLastdPcmd;					/* 差分位置指令前回値									*/
	LONG	StlgTimeCntr;					/* 整定時間計測用カウンタ								*/
	LONG	TmpStlgTime;					/* 整定時間カウンタバッファ								*/
	LONG	RsltStlgTime;					/* 整定時間演算結果										*/
	LONG	OverErrDir;						/* オーバーシュートする偏差の符号						*/
	LONG	OverShoot;						/* オーバーシュート量測定用			[指令単位]			*/
	LONG	UnitChngGain;					/* 単位変換用ゲイン[cyc]⇒[1us]							*/
	LONG	UnMaxOverShoot;					/* Un106 : 最大オーバーシュート量	[指令単位]			*/
	LONG	UnCoinOffTimer;					/* Un10B : 位置決め完了失敗時間積算	[ms]				*/
	LONG	OvsErrDetLvl;					/* 許容オーバーシュート量			[指令単位]			*/
	LONG	LastdPcmda;						/* 前回位置指令増分値[指令単位]							*/
} SETTLINGTIME;


typedef	struct UV_TLIM_DATA {
	struct {							/* 不足電圧ワーニング時トルク制限パラメータ定義				*/
		BOOL			UvTrqLmtCtrl;		/* 不足電圧ワーニング時トルク制限制御選択				*/
		LONG			UvTLimSetting;		/* 主回路電圧降下時トルク制限値			[2^24/MaxTrq]	*/
		LONG			DeltaUvTrqLmt;		/* 主回路電圧降下時トルク制限復帰トルク	[2^24/MaxTrq]	*/
		LONG			UvTrqLmtOffFil;		/* 主回路電圧降下時トルク制限解除時間	[ScanB回数]		*/
		SHORT			Dummy;				/* for Alignment										*/
	} conf;
	struct {							/* 不足電圧ワーニング時トルク制限変数定義					*/
		LONG			UvTrqLmt;			/* 不足電圧時のトルク制限				[2^24/MaxTrq]	*/
		ULONG			UvTrqLmtWait;		/* 不足電圧時のトルク制限解除ウェイトカウンタ			*/
	} var;
} UV_TLIM_DATA;


typedef	struct TRQ_LMT_DATA {
	struct {							/* トルク制限パラメータ定義									*/
		BOOL			TrefTrqLmt;			/* トルク指令を外部トルク制限として使用					*/
		BOOL			TrefTrqLmtCL;		/* トルク指令を外部トルク制限として使用(P-CL,N-CL有効時)*/
#if (FLOAT_USE==TRUE)
		float			EstopTrqLmt;		/* 非常停止トルク					[2^24/MaxTrq]		*/
		float			FwIntTrqLmt;		/* 正転内部トルク制限				[2^24/MaxTrq]		*/
		float			RvIntTrqLmt;		/* 逆転内部トルク制限				[2^24/MaxTrq]		*/
		float			FwExtTrqLmt;		/* 正転外部トルク制限				[2^24/MaxTrq]		*/
		float			RvExtTrqLmt;		/* 逆転外部トルク制限				[2^24/MaxTrq]		*/
#else
		LONG			EstopTrqLmt;		/* 非常停止トルク					[2^24/MaxTrq]		*/
		LONG			FwIntTrqLmt;		/* 正転内部トルク制限				[2^24/MaxTrq]		*/
		LONG			RvIntTrqLmt;		/* 逆転内部トルク制限				[2^24/MaxTrq]		*/
		LONG			FwExtTrqLmt;		/* 正転外部トルク制限				[2^24/MaxTrq]		*/
		LONG			RvExtTrqLmt;		/* 逆転外部トルク制限				[2^24/MaxTrq]		*/
#endif /* FLOAT_USE */
	} conf;
	struct {							/* トルク制限変数定義										*/
		BOOL			PclSignal;			/* PCL Signal											*/
		BOOL			NclSignal;			/* NCL Signal											*/
#if (FLOAT_USE==TRUE)
		float			FwTrqLmt;			/* Forward Torque Limit(入力)		[2^24/MaxTrq]		*/
		float			RvTrqLmt;			/* Reverse Torque Limit(入力)		[2^24/MaxTrq]		*/
//		float			PosTrqLmt;			/* Positive Torque Limit(Network)	[2^24/MaxTrq]		*/
//		float			NegTrqLmt;			/* Negative Torque Limit(Network)	[2^24/MaxTrq]		*/
		float			PosTrqLmtOut;		/* Positive Torque Limit(出力)		[2^24/MaxTrq]		*/
		float			NegTrqLmtOut;		/* Negative Torque Limit(出力)		[2^24/MaxTrq]		*/
#else
		LONG			FwTrqLmt;			/* Forward Torque Limit(入力)		[2^24/MaxTrq]		*/
		LONG			RvTrqLmt;			/* Reverse Torque Limit(入力)		[2^24/MaxTrq]		*/
//		LONG			PosTrqLmt;			/* Positive Torque Limit(Network)	[2^24/MaxTrq]		*/
//		LONG			NegTrqLmt;			/* Negative Torque Limit(Network)	[2^24/MaxTrq]		*/
		LONG			PosTrqLmtOut;		/* Positive Torque Limit(出力)		[2^24/MaxTrq]		*/
		LONG			NegTrqLmtOut;		/* Negative Torque Limit(出力)		[2^24/MaxTrq]		*/
#endif /* FLOAT_USE */
	} var;
	UV_TLIM_DATA		UvTrqLimits;		/* 不足電圧ワーニング時トルク制限用データ				*/
} TRQ_LMT_DATA;


typedef	struct BASE_MOT_STS {
//	BOOL				MotCposOk;			/* C相通過完了フラグ									*/
	BOOL				MotCphPass;			/* C相通過フラグ(1scan)									*/
	LONG				MposFromCpos;		/* CPOSからのモータ位置		 [+/-(PulseNo/2)],[LM:LONG]	*/
	LONG				MotSpd;				/* モータ速度							[2^24/OvrSpd]	*/
	LONG				FencSpd;			/* モータ速度(ﾌﾙｸ)						[2^24/OvrSpd]	*//* <S04D> */
	LONG				MotPos;				/* モータ位置(ScanB積算値)				[32bitRingPulse]*/
	LONG				dMotPos;			/* Delta Motor Position					[pulse/ScanB]	*/
	BOOL				FencCphPass;		/* C相通過フラグ(1scan)(ﾌﾙｸ)							*//* <S04D> */
	LONG				FposFromCpos;		/* CPOSからのモータ位置(ﾌﾙｸ) [+/-(PulseNo/2)],[LM:LONG]	*//* <S04D> */
	LONG				FencPos;			/* モータ位置(ScanB積算値)(ﾌﾙｸ)			[32bitRingPulse]*//* <S04D> */
	LONG				dFencPos;			/* Delta Motor Position	(ﾌﾙｸ)			[pulse/ScanB]	*//* <S04D> */
//	LONG				MotPosB;			/* ScanB取得モータ位置(PGデータ)		[pulse]			*//* <S04D>：未使用	*/
	LONG				MotSpdSumB;			/* Motor Speed Sum(ScanB周期)							*/
	LONG				MotSpdCntB;			/* Motor Speed Cnt(ScanB周期)							*/
//	LONG				MotSpdForOS;		/* Motor Speed OS異常検出用				[2^24/OvrSpd]	*//* <S04D>：未使用	*/
	LONG				MotSpdForSeq;		/* Motor Speed シーケンス用(補償なし)	[2^24/OvrSpd]	*//* <S0E5> */
//	SHORT				MotSpdForMp;		/* μProgram EMF補償計算用モータ速度	[15000/OvrSpd]	*//* <S04D>：未使用	*/
	SHORT				Dummy;				/* for Alignment */
} BASE_MOT_STS;


typedef	struct PCMP_CALC_DATA {
	KSGAIN				Kpcmpd;				/* 位相補償(遅れ)演算用ゲイン1(μプログラム用)			*/
	KSGAIN				Kpcmpd2;			/* 位相補償(遅れ)演算用ゲイン2(μプログラム用)			*/
	KSGAIN				Kpcmpsp1;			/* 位相補償(速度)演算用ゲイン1(μプログラム用)			*/
	KSGAIN				KpcmpdItplt;		/* 位相補間量演算係数(μプログラム用)					*/
	LONG				PcmpSpd1;			/* 位相補償1開始速度									*/
} PCMP_CALC_DATA;


typedef	struct POS_CTRL_STS {
	BOOL				RefZSignal;			/* Reference Zero Signal								*/
	BOOL				NearSignal;			/* Near Signal											*/
	BOOL				CoinSignal;			/* Coin Signal											*/
	BOOL				DenSignal;			/* Den Signal											*/
} POS_CTRL_STS;


typedef	struct BASE_CTRL_HNDL {
	MENCV				*MencV;				/* モータエンコーダデータへのポインタ			 		*//* <S004> */
	MENCV				*FencV;				/* フルクエンコーダデータへのポインタ			 		*//* <S04D> */
	CTRL_CMD_MNG		CtrlCmdMngr;		/* 制御指令管理データ									*/
	CTRL_CMD_PRM		CtrlCmdPrm;			/* 制御指令用パラメータ									*/
	BASE_MOT_STS		MotSts;				/* ScanBにて取得・作成するモータ状態					*/
	CTRLMD_SET			CtrlModeSet;		/* 制御モード関連データ									*/
	POS_MNG_HNDL		PosManager;			/* 位置管理用データハンドル								*/
	POS_LOOP_CTRL		FcPosLoop;			/* フルク用位置ループ制御データ 						*/
	POS_CTRL_STS		PosCtrlSts;			/* 位置制御ステータス(DEN/COIN/NEAR)					*/
	TRQ_LMT_DATA		TrqLimitData;		/* トルク制限関連データ									*/

//	MOTSPDMAFIL			MotSpdMafil;		/* モータ速度検出用移動平均演算用変数					*/
	SETTLINGTIME		SettlingTime;		/* 整定時間計測関連処理用構造体							*/

	ZX_CTRL_HNDL		ZeroClampCtrl;		/* ゼロクランプ制御演算用構造体							*/
	PCMDFIL				PcmdFil;			/* 位置指令フィルタ用変数								*//* <S065> */
	MAFIL				VFFcmdFil;			/* 速度フィードフォワード指令フィルタ用変数				*//* <S1D0> */
	MAFIL				TFFcmdFil;			/* トルクフィードフォワード指令フィルタ用変数			*//* <S1D0> */
	RESVIB				ResVib;				/* 振動抑制用変数										*/
	VIBSUPFIL			VibSupFil;			/* 振動抑制フィルタ用変数								*/
	MODE_SW				ModeSwData;			/* モードSW用変数										*/
	GAIN_CHNG_HNDL		GainChange;			/* ゲイン切り替え用構造体								*/
	MFCTRL				MFControl;			/* モデル追従制御用変数									*/
	TUNELESS_CTRL		TuneLessCtrl;		/* 調整レス制御用データ									*/
	DOBS_CTRL			DobsCtrl;			/* 外乱オブザーバ制御用データ							*/
	EHVOBS_CTRL			EhVobsCtrl;			/* 位相補償速度オブザーバ制御用データ					*/
	PCMP_CALC_DATA		PcmpCalcData;		/* 位相補償演算用データ									*/
	TBLDRIVE			TblDrive;			/* テーブル運転用構造体									*/
	PJOGHNDL			PJogHdl;			/* プログラムJOG用構造体								*/
	JATHNDL				JatHdl;				/* オフライン慣性モーメント推定							*/

	BOOL				BeComplete;			/* ベースイネーブル完了状態								*/
//	BOOL				PdetCtrlReq;		/* 磁極検出制御要求フラグ								*/
	BOOL				CLatchClrReq;		/* C Phase Position Latch Clear Request to μScanA		*/
	BOOL				BBRefClrReq;		/* Servo Control Ref. Clear Request from ScanC			*/
//	BOOL				OverTravel;			/* OT状態(OTシーケンス処理出力) from ScanC				*/
//	BOOL				PhaseReady;			/* Motor Phase Ready (Phase Found Finish)				*/
	BOOL				VrefInSts;			/* 速度指令入力中フラグ									*//* <S0A1> */
	BOOL				TrefInSts;			/* トルク指令入力中フラグ								*//* <S0A1> */
	LONG				GselNum;			/* マニュアルゲイン切替用ゲイン番号						*/

	WEAKENFIELD			WeakenField;		/* 弱め界磁制御演算用変数								*/
	MPKVVCOMP			MpKvvComp;			/* マイクロ電圧補償ゲイン機能用変数						*/

	TRANSDUCE_CTRL		TransduceCtrl;		/* 制御用パラメータ一括転送用変数						*/
/*--------------------------------------------------------------------------------------------------*/
	BOOL				PerClrEdge;			/* @@ Position Error Clear Signal Edge					*/
	LONG				ClrFbPos;			/* @@ Position Error Clear Latched FB Position[Pulse]	*/
	LONG				MonPerrLpos[2];		/* @@ Position Error Clear Latched FB Position[Ref]		*/
/*--------------------------------------------------------------------------------------------------*/
	FRICTION			FrictionM;			/* <S0CD> */
	LONG				TrqMotFricComp;		/* <S0CD> */
/*--------------------------------------------------------------------------------------------------*/
	BOOL				OverTravel;			/* <S18B> */
/*--------------------------------------------------------------------------------------------------*/
	LONG				AmondPosCmd;		/* <S1B6> 位置指令差分				[pulse/125us]		*/
	
} BASE_CTRL_HNDL;


/*--------------------------------------------------------------------------------------------------*/
/*		位置制御演算用変数定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* 位置制御用定数＆変数定義							*/
		struct	{								/* 位置制御用定数データ定義							*/
#if (FLOAT_USE==TRUE)
//		LONG	Kpi;								/* 位置制御比例積分ゲイン						*/
		float	Kpd;								/* 位置制御比例微分ゲイン						*/
		float	VelBias;							/* 位置制御速度バイアス							*/
		float	BiasAddLevel;						/* 速度バイアス加算幅		[Pulse, mm]			*/
//		LONG	KdPff;								/* 位置制御位置増分値ＦＦゲイン					*/
//		LONG	KdPffFil;							/* 位置制御位置増分値ＦＦフィルタゲイン			*/
#else
//		LONG	Kpi;								/* 位置制御比例積分ゲイン						*/
		LONG	Kpd;								/* 位置制御比例微分ゲイン						*/
		LONG	VelBias;							/* 位置制御速度バイアス							*/
		LONG	BiasAddLevel;						/* 速度バイアス加算幅		[Pulse, mm]			*/
//		LONG	KdPff;								/* 位置制御位置増分値ＦＦゲイン					*/
//		LONG	KdPffFil;							/* 位置制御位置増分値ＦＦフィルタゲイン			*/
#endif /* FLOAT_USE */
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* 位置制御用変数データ定義							*/
#if (FLOAT_USE==TRUE)
		float	SpdRef;								/* 位置制御出力									*/
		float	PerIvar64[2];						/* 位置偏差積分演算用							*/
		float	PacOut;								/* 比例制御出力									*/
		float	IacOut;								/* 積分制御出力									*/
		float	DacOut;								/* 微分制御出力									*/
		float	LastPacOut;							/* 前回比例制御出力								*/
		float	dPFfFilo;							/* 位置増分値ＦＦフィルタ出力					*/
#else
		LONG	SpdRef;								/* 位置制御出力									*/
		LONG	PerIvar64[2];						/* 位置偏差積分演算用							*/
		LONG	PacOut;								/* 比例制御出力									*/
		LONG	IacOut;								/* 積分制御出力									*/
		LONG	DacOut;								/* 微分制御出力									*/
		LONG	LastPacOut;							/* 前回比例制御出力								*/
		LONG	dPFfFilo;							/* 位置増分値ＦＦフィルタ出力					*/
#endif /* FLOAT_USE */
		} V;
} POSCTRL;
/*--------------------------------------------------------------------------------------------------*/
/*		速度制御演算用変数定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* 速度制御用定数＆変数定義							*/
		struct	{								/* 速度制御用定数データ定義							*/
//		LONG	IPSpdControl;
//		LONG	SpdIctrlKeep;
//		LONG	P_I;								/* 速度制御ＰＩ制御割合							*/
//		LONG	SpdSfsAcc;							/* ソフトスタート加速量							*/
//		LONG	SpdSfsDec;							/* ソフトスタート減速量							*/
//		LONG	KVrfFil;							/* 速度指令フィルタゲイン						*/
//		LONG	KVfbFil;							/* 速度ＦＢフィルタゲイン						*/
		KSGAIN	KVintegFil;							/* 速度積分クリアフィルタゲイン					*//*<S00A>*/
//		LONG	SpdRefPhFil;						/* 位相進み用ＬＰＦ(速度指令用)					*//*<V108>*/
//		LONG	SpdRefPhFilGn;						/* 位相進み補償ゲイン(速度指令用)				*//*<V108>*/
////		LONG	SpdFFPhFil;							/* 位相進み用ＬＰＦ(速度ＦＦ用)					*//*<V108>*/
////		LONG	SpdFFPhFilGn;						/* 位相進み補償ゲイン(速度ＦＦ用)				*//*<V108>*/
//		LONG	SvonSpdLmtLevel;
//		LONG	SvonSpdLmtValue;
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* 速度制御変数データ定義							*/
#if (FLOAT_USE==TRUE)
		float	TrqRef;								/* 速度制御出力									*/
		float	SpdFbFilo;							/* 速度ＦＢフィルタ出力							*/
		float	PacOut;								/* 比例制御出力									*/
		float	IacOut;								/* 積分制御出力									*/
		LONG	Ivar64[2];							/* 積分制御演算用								*/
		float	SpdRefPhLpfo;						/* 位相進み用ＬＰＦ出力(速度指令用)				*//*<V108>*/
		float	SpdRefPhHpfo;						/* 位相進み用ＨＰＦ出力(速度指令用)				*//*<V108>*/
		float	SpdRefPhHpfTmp;						/* 位相進み用ＨＰＦ演算用(速度指令用)			*//*<V108>*/

		float	SpdFFPhLpfo;						/* 位相進み用ＬＰＦ出力(速度ＦＦ用)				*//*<V108>*/
		float	SpdFFPhHpfo;						/* 位相進み用ＨＰＦ出力(速度ＦＦ用)				*//*<V108>*/
		float	SpdFFPhHpfTmp;						/* 位相進み用ＨＰＦ演算用(速度ＦＦ用)			*//*<V108>*/
#else
		LONG	TrqRef;								/* 速度制御出力									*/
		LONG	SpdFbFilo;							/* 速度ＦＢフィルタ出力							*/
		LONG	PacOut;								/* 比例制御出力									*/
		LONG	IacOut;								/* 積分制御出力									*/
		LONG	Ivar64[2];							/* 積分制御演算用								*/
		LONG	SpdRefPhLpfo;						/* 位相進み用ＬＰＦ出力(速度指令用)				*//*<V108>*/
		LONG	SpdRefPhHpfo;						/* 位相進み用ＨＰＦ出力(速度指令用)				*//*<V108>*/
		LONG	SpdRefPhHpfTmp;						/* 位相進み用ＨＰＦ演算用(速度指令用)			*//*<V108>*/
		LONG	SpdFFPhLpfo;						/* 位相進み用ＬＰＦ出力(速度ＦＦ用)				*//*<V108>*/
		LONG	SpdFFPhHpfo;						/* 位相進み用ＨＰＦ出力(速度ＦＦ用)				*//*<V108>*/
		LONG	SpdFFPhHpfTmp;						/* 位相進み用ＨＰＦ演算用(速度ＦＦ用)			*//*<V108>*/
#endif /* FLOAT_USE */

		UCHAR	SpdRefFilClrCmd;					/* 速度指令位相進み補償フィルタ初期化コマンド	*//*<V392>*/
		UCHAR	SpdFFFilClrCmd;						/* 速度ＦＦ位相進み補償フィルタ初期化コマンド	*//*<V392>*/
		BOOL	SvonSpdLmtFlag;
		BOOL	LastSvonFlag;
		} V;
} SPDCTRL;
/*--------------------------------------------------------------------------------------------------*/
/*		トルク制御演算用変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
//typedef	struct	{								/* トルク制御用定数＆変数定義						*/
//		struct	{								/* トルク制御用定数データ定義						*/
//		LONG	OverTrqLoopGain[3];					/* 速度制限用ループゲイン						*/
//		LONG	OverTrqIntegGain[3];				/* 速度制限用積分ゲイン							*/
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LONG	Kp;									/* アナログF/B比例ゲイン						*/
		LONG	Kvi;								/* アナログF/B積分ゲイン						*/
		LONG	KdFF;								/* アナログF/BループFF							*/
		LONG	AnlgfbLimit;						/* アナログF/B積分リミット値					*/
		LONG	Bias;								/* アナログF/Bバイアス							*/
		USHORT	OverCntVal;							/* アナログF/B過大検出時間カウント値			*/
		USHORT	AnlgfbPolarSw;						/* アナログF/B極性選択							*/
		LONG	KIntegPreset;						/* アナログF/B積分プリセット係数				*/
#endif
//		} P;
	/*----------------------------------------------------------------------------------------------*/
//		struct {								/* トルク制御用変数データ定義						*/
//		LONG	SpdLimit;							/* トルク制御時の速度制限値						*/
//		LONG	OverTrqIntegBuf[2];
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LONG	TrqRefx;							/* アナログF/B推力指令							*/
		LONG	PacOut;								/* 比例制御出力									*/
		LONG	IacOut;								/* 積分制御出力									*/
		LONG	Ivar64[2];							/* 積分制御演算用								*/
		LONG	AnlgFbMon;							/* アナログF/B値モニタ[100%]					*/
		USHORT	OverCnt;							/* アナログF/B過大検出時間カウンタ				*/
		UCHAR	AnlgfbSts_l;						/* 前回アナログF/B状態							*/
		UCHAR	AnlgfbSts_ll;						/* 前々回アナログF/B状態						*/
		LONG	DTrqLpfoLatch;						/* 外乱推定値ラッチ								*/
		UCHAR	SensorContact;						/* アナログFBセンサ接触							*/
		UCHAR	InitDone;							/* トルク制御への切替え時の初期化を２度しないよう追加 */
#endif
//		} V;
//} TRQCTRL;
/*--------------------------------------------------------------------------------------------------*/
/*		トルクフィルタ演算用変数定義																*/
/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ : ScanAとScanBで個別																*/
/*		演算用変数 : ScanAとScanBで共有(制御モード切替時のトルク指令連続性確保のため)				*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* トルクフィルタ用定数＆変数定義					*/
		struct {								/* ScanA : トルクフィルタ用定数定義					*/
#if (FLOAT_USE==TRUE)
		float	Klpf;								/* １次遅れフィルタゲイン						*/
		float	Knxf[3];							/* ノッチフィルタゲイン							*/
		float	Klpf2[4];							/* ２次遅れフィルタゲイン						*/
		float	Ktrflpf;							/* T-REFフィルタ								*//* <V102> */
#else
		LONG	Klpf;								/* １次遅れフィルタゲイン						*/
		LONG	Knxf[3];							/* ノッチフィルタゲイン							*/
		LONG	Klpf2[4];							/* ２次遅れフィルタゲイン						*/
		LONG	Ktrflpf;							/* T-REFフィルタ								*//* <V102> */
#endif /* FLOAT_USE */
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		LONG	Kafblpf;							/* アナログF/Bフィルタ							*/
#endif
		} A;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* ScanB : トルクフィルタ用定数定義					*/
#if (FLOAT_USE==TRUE)
		float	Knxf[3];							/* ノッチフィルタゲイン							*/
		float	Klpf2[4];							/* ２次遅れフィルタゲイン						*/
		float	Ktrflpf;							/* T-REFフィルタ								*//* <V102> */
		float	Klpf3;								/* １次遅れフィルタゲイン(ﾄﾙｸ補償後)			*//*<V371>*/
#else
		LONG	Knxf[3];							/* ノッチフィルタゲイン							*/
		LONG	Klpf2[4];							/* ２次遅れフィルタゲイン						*/
		LONG	Ktrflpf;							/* T-REFフィルタ								*//* <V102> */
		LONG	Klpf3;								/* １次遅れフィルタゲイン(ﾄﾙｸ補償後)			*//*<V371>*/
#endif /* FLOAT_USE */
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* トルクフィルタ用変数データ定義					*/
#if (FLOAT_USE==TRUE)
		float	FilOut;								/* トルクフィルタ出力							*/
		float	FilOut3;							/* トルクフィルタ出力(ﾄﾙｸ補償後)				*//*<V371>*/
		float	NxFil[3];							/* ノッチフィルタ演算用							*/
		float	LpFil2[4];							/* ２次遅れフィルタ演算用						*/
		UCHAR	TrqFilClrCmd;						/* トルク指令フィルタ積分初期化コマンド			*/
		UCHAR	TrqFilDisable;						/* トルク指令フィルタ無効						*//* <V181> */
//		UCHAR	TrqInputPos;						/* トルク外乱指令入力位置選択					*//* <S0B7> */
#else
		LONG	FilOut;								/* トルクフィルタ出力							*/
		LONG	FilOut3;							/* トルクフィルタ出力(ﾄﾙｸ補償後)				*//*<V371>*/
		LONG	NxFil[3];							/* ノッチフィルタ演算用							*/
		LONG	LpFil2[4];							/* ２次遅れフィルタ演算用						*/
		UCHAR	TrqFilClrCmd;						/* トルク指令フィルタ積分初期化コマンド			*/
		UCHAR	TrqFilDisable;						/* トルク指令フィルタ無効						*//* <V181> */
//		UCHAR	TrqInputPos;						/* トルク外乱指令入力位置選択					*//* <S0B7> */
#endif /* FLOAT_USE */
		} V;
} TRQFIL;
/*--------------------------------------------------------------------------------------------------*/
/*		ゼロ速停止＆ゼロクランプ制御用変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* ゼロ速制御用定数＆変数定義						*/
		struct	{								/* ゼロ速制御用定数データ定義						*/
#if (FLOAT_USE==TRUE)
		float	Kp;									/* 位置制御比例ゲイン							*/
		float	SpdLimit;							/* 速度制御リミット値							*/
#else
		LONG	Kp;									/* 位置制御比例ゲイン							*/
		LONG	SpdLimit;							/* 速度制御リミット値							*/
#endif /* FLOAT_USE */
		} P;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* ゼロ速制御用変数データ定義						*/
		KSGAIN	PosError;							/* 位置偏差										*//*<S00A>*/
		} V;
} ZXCTRL;

/*--------------------------------------------------------------------------------------------------*/
/*		キャリア周波数切り替え用変数定義															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* キャリア周波数切り替え用定数＆変数定義			*/
		struct	{								/* キャリア周波数切り替え用定数データ定義			*/
		BOOL	CarrChange;							/* キャリア周波数切り替えフラグ [0:OFF,1:ON]	*/
		LONG	IqLimitA;							/* キャリア切り替え電流閾値A	[2^24/MaxTrq]	*/
		LONG	IqLimitB;							/* キャリア切り替え電流閾値B	[2^24/MaxTrq]	*/
		LONG	VelLimitA;							/* キャリア切り替え速度閾値A	[2^24/OvrSpd]	*/
		LONG	VelLimitB;							/* キャリア切り替え速度閾値B	[2^24/OvrSpd]	*/
		SHORT	NormCarrCnt;						/* 通常キャリアカウント値		[-]				*/
		SHORT	HighCarrCnt;						/* 切り替え高キャリアカウント値	[-]				*/
		BOOL	MotCarrUse;							/* モータキャリア(PnF25)有効 	[0:OFF,1:ON]	*//* モータキャリア周波数切り替え<S117> */
		SHORT	CarrCnt;							/* 現在キャリアカウント値		[-]				*//* モータキャリア周波数切り替え<S117> */
		} conf;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* キャリア周波数切り替え用変数データ定義			*/
		BOOL	CarrState;							/* 現在のキャリア設定							*/
		} var;										/*	(0:低キャリア, 1:高キャリア)				*/
} CARRCHNG;


/*--------------------------------------------------------------------------------------------------*/
/*		キャリア周波数切り替え用変数定義															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* キャリア周波数切り替え用定数＆変数定義			*/
		struct	{								/* キャリア周波数切り替え用定数データ定義			*/
#if (FLOAT_USE==TRUE)
		float	Kpcmpd;								/* Pcmpd Calculate Gain1						*/
		float	Kpcmpd2;							/* Pcmpd Calculate Gain2						*/
		float	Kpcmpsp1;							/* Pcmps Calculate Gain1						*/
		float	PcmpSpd1;							/* Pcmps Calculate Speed1						*/
		float	KpcmpdItplt;						/* 位相補間量演算係数							*/
#else
		LONG	Kpcmpd;								/* Pcmpd Calculate Gain1						*/
		LONG	Kpcmpd2;							/* Pcmpd Calculate Gain2						*/
		LONG	Kpcmpsp1;							/* Pcmps Calculate Gain1						*/
		LONG	PcmpSpd1;							/* Pcmps Calculate Speed1						*/
		LONG	KpcmpdItplt;						/* 位相補間量演算係数							*/
#endif /* FLOAT_USE */
		} conf;
	/*----------------------------------------------------------------------------------------------*/
		struct {								/* キャリア周波数切り替え用変数データ定義			*/
#if (FLOAT_USE==TRUE)
		float	Pcmpd;								/* 位相補償(遅れ補償1)			[65536/360deg]	*/
		float	Pcmpd2;								/* 位相補償(遅れ補償2)			[65536/360deg]	*/
		float	Pcmps;								/* 位相補償(速度補償計)			[65536/360deg]	*/
		float	Pcmps1;								/* 位相補償(速度補償1)			[65536/360deg]	*/
		float	Pcmps2;								/* 位相補償(速度補償2)			[65536/360deg]	*/
		float	PcmpdItplt;							/* 位相補間量									*/
#else
		SHORT	Pcmpd;								/* 位相補償(遅れ補償1)			[65536/360deg]	*/
		SHORT	Pcmpd2;								/* 位相補償(遅れ補償2)			[65536/360deg]	*/
		SHORT	Pcmps;								/* 位相補償(速度補償計)			[65536/360deg]	*/
		SHORT	Pcmps1;								/* 位相補償(速度補償1)			[65536/360deg]	*/
		SHORT	Pcmps2;								/* 位相補償(速度補償2)			[65536/360deg]	*/
		SHORT	PcmpdItplt;							/* 位相補間量									*/
#endif /* FLOAT_USE */
		} var;
} PHASECMP;


/*--------------------------------------------------------------------------------------------------*/
/*		for Base Loop																				*/
/*--------------------------------------------------------------------------------------------------*/
/* ★構造体再検討必要★ */
typedef struct BASE_LOOPCTRLS {
	MENCV			*MencV;
	MENCV			*FencV;																			/* <S04D> */
	BPRMDAT			*Bprm;
	MOTSPDMAFIL		*MotSpdMafil;		/* モータ速度検出用移動平均演算用変数						*/
	BASE_CTRL_HNDL	*BaseCtrls;
	GSELGAINS		*GselGains;
	ASIC_HW_IF_STR	*pAsicHwReg;		/* Asic H/W レジスタアドレス								*//* <S004> */
	ALMMAN_HANDLE	*AlmManager;		/* <S018> */
/* <S1B7> >>>>> */
	MOTLESSTST		*MotorLessTest;		/* モータレステストIF定義									*/
	void			(*SencReadPosition)(MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
										ALMMAN_HANDLE *AlmManager, BOOL BeComplete,
										void *BaseLoops);
	LONG 			(*FencReadPosition)(MENCV *FencV,
										ALMMAN_HANDLE *AlmManager, BOOL BeComplete,
										void *BaseLoops);
/* <<<<< <S1B7> */
	/*----------------------------------------------------------------------------------------------*/
	POSCTRL			PosCtrl;			/* 位置制御用変数											*/
	SPDCTRL			SpdCtrl;			/* 速度制御用変数											*/
//	TRQCTRL			TrqCtrl;			/* トルク制御用変数											*/
	TRQFIL			TrqFil;				/* トルクフィルタ用変数										*/
	ZXCTRL			ZxCtrl;				/* ゼロ速停止＆ゼロクランプ制御用変数						*/
	/*----------------------------------------------------------------------------------------------*/
	CARRCHNG		CarrerChange;		/* キャリア周波数切り替え用変数								*/
	PHASECMP		PhaseComp;			/* 位相補償用変数											*/
	VFBTIMBASE		Vfbtimbase;			/* ベースタイマによるFB速度補正用変数						*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float			MotPos;
	float			MotSpd;
	float			dMotPos;
	float			LastMotPos;
#else
	LONG			MotPos;
	LONG			MotSpd;
	LONG			dMotPos;
	LONG			LastMotPos;
#endif /* FLOAT_USE */
/*Full Closed Encoder Define Start	*//* <S04D> */
#if (FLOAT_USE==TRUE)
	float			FencPos;
	float			FencSpd;
	float			dFencPos;
	float			LastFencPos;
#else
	LONG			FencPos;
	LONG			FencSpd;
	LONG			dFencPos;
	LONG			LastFencPos;
#endif /* FLOAT_USE */
/*Full Closed Encoder Define End	*/
	/*----------------------------------------------------------------------------------------------*/
	USHORT			CtrlMode;			/* 基本ループ制御モード										*/
	USHORT			LastCtrlMode;		/* 前回基本ループ制御モード									*//* <S007> */
	USHORT			ZCtrlMode;			/* ゼロ速制御／ゼロクランプ制御時							*/
	USHORT			Dummy;
	/*----------------------------------------------------------------------------------------------*/
	LONG 			CmdCtrlBit;
	BOOL			PConActFlg;			/* P制御状態確認フラグ										*//* <S064> */
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float			dPcmda;				/* 位置指令増分値入力　[指令単位]							*/
	float			dPosRefi;			/* 位置制御：位置指令差分入力		[Pulse/Scan]			*/
	float			NextdPcmda;			/* 位置制御：次回位置指令増分値入力	[指令単位]				*/
	float			NextdPosRefi;		/* 位置制御：次回位置指令差分入力	[Pulse/Scan]			*/
	float			dPosFbki;			/* 位置制御：位置ＦＢ差分入力		[Pulse/Scan]			*/
	PMAFV			pmafvar;
	float 			pmafbuf[4];
	float			SpdRefp;			/* 位置制御：速度指令出力			[2^24/OvrSpd]			*/
	float			SpdRefi;			/* 速度制御：速度指令入力			[2^24/OvrSpd]			*/
	float			SpdRefo;			/* 速度制御：速度指令ＳＦＳ出力		[2^24/OvrSpd]			*/
	float			SpdRefSum;			/* 速度制御：指令速度(Ref+FF)		[2^24/OvrSpd]			*/
	float			SpdFbki;			/* 速度制御：速度ＦＢ入力			[2^24/OvrSpd]			*/
	float			SpdObsFbki;			/* 速度制御：速度ＦＢ入力(OBS後)	[2^24/OvrSpd]			*/
	float			TrqRefs;			/* 速度制御：トルク指令出力			[2^24/MaxTrq]			*/
	float			SpdFFCx;			/* 拡張制御：速度ＦＦ補償			[2^24/OvrSpd]			*/
	float			SpdFBCx;			/* 拡張制御：速度ＦＢ補償			[2^24/OvrSpd]			*/
	float			TrqFFCx;			/* 拡張制御：トルクＦＦ補償			[2^24/MaxTrq]			*/
	float			TrqFBCx;			/* 拡張制御：トルクＦＢ補償			[2^24/MaxTrq]			*/
	float			TrqRefi;			/* トルクフィルタ：トルク指令入力	[2^24/MaxTrq]			*/
	float			TrqRefo;			/* トルクフィルタ：トルク指令出力	[2^24/MaxTrq]			*/
	float			TrqRefo_l;			/* トルクフィルタ：トルク指令出力	[2^24/MaxTrq]			*/
	float			TrqRefo_a;			/* トルクフィルタ：トルク指令出力	[2^24/MaxTrq]			*/
	float			TrqRefoLimit;		/* トルク制限後トルク指令			[2^24/MaxTrq]			*/
	float			TrqRefoComp;		/* 補正後トルク指令出力				[2^24/MaxTrq]			*/
	float			DisTrqRef3;			/* トルク外乱指令入力				[2^24/MaxTrq]			*/
	float			MotSpd_l;			/* Motor Speed						[2^24/OvrSpd]			*/
	float			MotSpd_a;			/* Motor Speed						[2^24/OvrSpd]			*/
#else
	LONG			dPcmda;				/* 位置指令増分値入力　[指令単位]							*/
	LONG			dPosRefi;			/* 位置制御：位置指令差分入力		[Pulse/Scan]			*/
	LONG			NextdPcmda;			/* 位置制御：次回位置指令増分値入力	[指令単位]				*/
	LONG			NextdPosRefi;		/* 位置制御：次回位置指令差分入力	[Pulse/Scan]			*/
	LONG			dPosFbki;			/* 位置制御：位置ＦＢ差分入力		[Pulse/Scan]			*/
	PMAFV			pmafvar;
	LONG 			pmafbuf[4];
	LONG			SpdRefp;			/* 位置制御：速度指令出力			[2^24/OvrSpd]			*/
	LONG			SpdRefi;			/* 速度制御：速度指令入力			[2^24/OvrSpd]			*/
	LONG			SpdRefo;			/* 速度制御：速度指令ＳＦＳ出力		[2^24/OvrSpd]			*/
	LONG			SpdRefSum;			/* 速度制御：指令速度(Ref+FF)		[2^24/OvrSpd]			*/
	LONG			SpdFbki;			/* 速度制御：速度ＦＢ入力			[2^24/OvrSpd]			*/
	LONG			SpdObsFbki;			/* 速度制御：速度ＦＢ入力(OBS後)	[2^24/OvrSpd]			*/
	LONG			TrqRefs;			/* 速度制御：トルク指令出力			[2^24/MaxTrq]			*/
	LONG			SpdFFCx;			/* 拡張制御：速度ＦＦ補償			[2^24/OvrSpd]			*/
	LONG			SpdFBCx;			/* 拡張制御：速度ＦＢ補償			[2^24/OvrSpd]			*/
	LONG			TrqFFCx;			/* 拡張制御：トルクＦＦ補償			[2^24/MaxTrq]			*/
	LONG			TrqFBCx;			/* 拡張制御：トルクＦＢ補償			[2^24/MaxTrq]			*/
	LONG			TrqRefi;			/* トルクフィルタ：トルク指令入力	[2^24/MaxTrq]			*/
	LONG			TrqRefo;			/* トルクフィルタ：トルク指令出力	[2^24/MaxTrq]			*/
	LONG			TrqRefo_l;			/* トルクフィルタ：トルク指令出力	[2^24/MaxTrq]			*/
	LONG			TrqRefo_a;			/* トルクフィルタ：トルク指令出力	[2^24/MaxTrq]			*/
	LONG			TrqRefoLimit;		/* トルク制限後トルク指令			[2^24/MaxTrq]			*/
	LONG			TrqRefoComp;		/* 補正後トルク指令出力				[2^24/MaxTrq]			*/
	LONG			DisTrqRef3;			/* トルク外乱指令入力				[2^24/MaxTrq]			*/
	LONG			MotSpd_l;			/* Motor Speed						[2^24/OvrSpd]			*/
	LONG			MotSpd_a;			/* Motor Speed						[2^24/OvrSpd]			*/
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	SHORT			IdRef;				/* d軸電流指令						[15000/Imax]			*/
	SHORT			IqRef;				/* q軸電流指令						[15000/Imax]			*/
	SHORT			IqDist;				/* q軸外乱トルク					[15000/MaxTrq]			*/
	BOOL			TrqClamp;
	/*----------------------------------------------------------------------------------------------*/
	SHORT			PowMonVq;			/* 消費電力モニタ演算用q軸電圧								*//* <S020> */
	SHORT			PowMonIqd;			/* 消費電力モニタ演算用q軸電流								*//* <S020> */
	SHORT			PowMonVd;			/* 消費電力モニタ演算用d軸電圧								*//* <S020> */
	SHORT			PowMonIdd;			/* 消費電力モニタ演算用d軸電流								*//* <S020> */
	/*----------------------------------------------------------------------------------------------*/
/*Full Closed Encoder Define Start	*//* <S04D> */
#if (FLOAT_USE==TRUE)
	float			FencSpd_l;			/* 前回Load Speed					[2^24/OvrSpd]			*/
	float			FencSpd_a;			/* 平均Load Speed					[2^24/OvrSpd]			*/
	float			SpdObsFbki_l;		/* 前回Load Speed					[2^24/OvrSpd]			*/
	float			SpdObsFbki_a;		/* 平均Load Speed					[2^24/OvrSpd]			*/
#else
	LONG			FencSpd_l;			/* 前回Load Speed					[2^24/OvrSpd]			*/
	LONG			FencSpd_a;			/* 平均Load Speed					[2^24/OvrSpd]			*/
	LONG			SpdObsFbki_l;		/* 前回Load Speed					[2^24/OvrSpd]			*/
	LONG			SpdObsFbki_a;		/* 平均Load Speed					[2^24/OvrSpd]			*/
	/*----------------------------------------------------------------------------------------------*/
#endif /* FLOAT_USE */
/*Full Closed Encoder Define End	*/
	struct {
		UCHAR	PastInputClrEdge; 		/* AP専用フラグ!!											*/
		UCHAR	PerClrEdge;		 		/* AP専用フラグ!!											*/
		UCHAR	Reserved1;		 		/* AP専用フラグ!!											*/
		UCHAR	Reserved2;		 		/* AP専用フラグ!!											*/
	} f;
	/*----------------------------------------------------------------------------------------------*/
	LONG			TRefAd;				/* Torque Reference [AD value] <S051> 						*/

	RPLCMP	RippleCmp;				/* リップル補償用変数										*//* <S14B> */
} BASE_LOOPCTRLS;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* Baseloops */
void	SysScanAInit( BASE_LOOPCTRLS *BaseLoops, MENCV *MencV, MENCV *FencV, BPRMDAT *Bprm, MOTSPDMAFIL *MotSpdMafil,
		 	 BASE_CTRL_HNDL *BaseCtrls, GSELGAINS *GselGains,ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE 	*AlmManager, /* <S04D>:引数追加 */
		 	 MOTLESSTST *MotorLessTest );	/* <S1B7>:引数追加 */
void	KpxInputScanA( BASE_LOOPCTRLS *BaseLoops );/*<S0C5>*/
ALMID_T	KpxOutputScanA( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut, ASICS *SvAsicRegs, PRMDATA *Prm );
//void	SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl, BASE_CTRL_OUT *BaseCtrlOut );
void	SysInputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, BASE_CTRL_HNDL *pBaseCtrl,
													BASE_CTRL_OUT *BaseCtrlOut, CMD_CTRLBITS *CmdCtrl );/* <S050> */
void 	SysOutputScanAProcessData( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut ); /* 2012.12.27 Y.Oka 暫定対応 */
void	ApxYecServoControl( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut ); /* <S0B8> */


/* BaseControls */
void BpiYecServoControl( BASE_LOOPCTRLS*, SERVO_CONTROL_IF*, CTRL_LOOP_OUT*, FUN_CMN_CONTROL*, BPRMDAT* );/* <S04D> */
void	BpiMpoleFindMakePosCmd( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
													FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm);/* <S04D> */
void BpiTorqueLimitControl( TRQ_LMT_DATA*, BASE_CTRL_HNDL*,	BOOL, ALMMAN_HANDLE*); /* <S145> */
void BpiChangeSpdDetMaFil( MOTSPDMAFIL*, LONG num );		/*<S02F>*/
LONG BpiBasePositionControl( POS_LOOP_CTRL *PosLoop, CTRL_CMD_MNG *CtrlCmd,
								 POS_MNG_HNDL *PosManager, KSGAIN Kp, KSGAIN Kpi );
void BpiInputMotorPosition(BASE_CTRL_HNDL *BaseControls, ASIC_U_IF_STR *pAsicMicroReg,
							BPRMDAT *Bprm, PRMDATA *Prm, CTRL_LOOP_OUT *CtrlLoopOut);/* <S04D>:引数整理 *//* <S086> */
void BpiOutputNetStatus(BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                        	CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, BPRMDAT *Bprm, PLSIFV *pPls);/* <S04D>:引数追加 */
void BpiInputControlCMD( BASE_CTRL_HNDL *BaseControls, SERVO_CONTROL_IF *SvControlIf,
                         CTRL_LOOP_OUT *CtrlLoopOut, SEQ_CTRL_OUT *SeqCtrlOut, USHORT uwd_l, LONG ax_no);/* <S1C0>：引数追加 */

void InitBasePositionControl( POSCTRL *PosCtrl );	/* <S14C> */

#endif	/* _BASE_CONTROLS_H_*/
/***************************************** end of file **********************************************/

