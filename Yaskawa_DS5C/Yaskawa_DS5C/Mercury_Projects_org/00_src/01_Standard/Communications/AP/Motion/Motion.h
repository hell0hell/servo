/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Motion.h : Motion Module Common Header file 												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Motion 共通ヘッダファイル																	*/
/*																									*/
/*		1. Global Function Prototypes ( MtXXX.h )													*/
/*																									*/
/*		2. Global Data Definition ( MtXXX.h )														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.09.05  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef		_MOTION_H_
#define		_MOTION_H_

#include "KnlApi.h"					/* Kernel API */

typedef SV_CYC_CMD CYC_CMD;			/* 暫定 */
typedef SV_CYC_RSP CYC_RSP;			/* 暫定 */
typedef UINT RSP_SEQBITS;			/* 暫定 */
typedef UINT RSP_LTBITS;			/* 暫定 */
typedef UINT CMD_SEQBITS;			/* 暫定 */
typedef UINT CMD_LTBITS;			/* 暫定 */

/****************************************************************************************************/
/*																									*/
/*	Grobal Definitions																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* defines : Control Mode 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define MT_NO_CTRLMD	(0)		/* 制御無効モード */
#define MT_POS_CTRLMD	(1)		/* 位置制御モード */
#define MT_VEL_CTRLMD	(2)		/* 速度制御モード */
#define MT_TRQ_CTRLMD	(3)		/* トルク制御モード */

/*--------------------------------------------------------------------------------------------------*/
/* defines : ラッチモード(API設定用) */
/*--------------------------------------------------------------------------------------------------*/
#define MOTION_LTMD			1	/* モーションラッチモード */
#define MOTION_MDL_LTMD		2	/* モーションラッチモード(SVCTRL) */
#define MODAL_LTMD			3	/* モーダルラッチモード(Normal) */

/*--------------------------------------------------------------------------------------------------*/
/* defines : ラッチモード(内部処理用) */
/*--------------------------------------------------------------------------------------------------*/
#define INTERPOL_LTMD		4	/* モーションラッチラッチモード(INTERPOLATE) */
#define EXPOSING_LTMD		5	/* モーションラッチラッチモード(EXPOSING) */
#define ZRET_LTMD			6	/* モーションラッチラッチモード(ZRET) */
//#define ZRET_LOCK_LTMD		7	/* モーションラッチラッチモード(ZRET中) */
#define NONE_LTMD			0	/* ラッチモードなし */

/*--------------------------------------------------------------------------------------------------*/
/* defines : Soft Limit                                                                             */
/*--------------------------------------------------------------------------------------------------*/
#define SOFT_LIMIT_P	0x1
#define SOFT_LIMIT_N	0x2

/*--------------------------------------------------------------------------------------------------*/
/* enums : Motion Types																				*/
/*--------------------------------------------------------------------------------------------------*/
enum MOTIONTYPE {
	MTYPE_NONE=0,	/* 0:モーション停止中 */ 
	MTYPE_POSING,	/* 1:位置決め */ 
	MTYPE_EXPOSING,	/* 2:外部位置決め */ 
	MTYPE_HOMING,	/* 3:原点復帰 */
	MTYPE_FEED,		/* 4:定速送り */
	MTYPE_EXFEED,	/* 5:外部信号定速送り */
	MTYPE_HOLD,		/* 6:停止 */
	MTYPE_INTERPOL,	/* 7:補間送り */ 
	MTYPE_VELCTRL,	/* 8:速度制御 */
	MTYPE_TRQCTRL,	/* 9:トルク制御 */
	MTYPE_FOLLOW,	/* A:フォローアップ */ 
};

/*--------------------------------------------------------------------------------------------------*/
/* enums : Hold Mode																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _MT_HOLD_MODE {
	MT_HOLD_SLOWSTOP = 0,
	MT_HOLD_FASTSTOP = 1,
	MT_HOLD_EMGSTOP  = 2,
} MT_HOLD_MODE;

/*--------------------------------------------------------------------------------------------------*/
/* enums : Latch Signal																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _MT_LATCH_SGNL {
	MT_LATCH_CPHASE = 0,
	MT_LATCH_EXT1,
	MT_LATCH_EXT2,
	MT_LATCH_EXT3,
}  MT_LATCH_SGNL;


/****************************************************************************************************/
/*																									*/
/*  Motion Parameter Definitions 																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* モーション動作機能SW： 無効にする場合はコメントにする											*/
/*--------------------------------------------------------------------------------------------------*/
//#define PRM_BANK_ENABLE		/* バンク切り替え有効SW */
#define ACC_FIL_DISABLE	    /* フィルタ無効SW */
#define POSING_1RATEONLY	/* POSING 1段目無効SW */

/*--------------------------------------------------------------------------------------------------*/
#define MAX_TRQ24			0x1000000	/* MAXTRQ */
#define MAX_SPD24			0x1000000	/* MAX速度 */
#define DISTANCE_MAX		0x7FFFFFFF	/* 移動距離MAX */
#define ACF_MAX_SZ			512			/* 移動平均フィルタバッファ数 */
#define FOLLOWUP_MAX_REF	32767		/* FollowUp時最大移動量[ref/cyc] */
#define MG_VREMBIT_MAX		24			/* MotionGenerator余り速度演算MAXビット数 */
#define MG_SPDDIV_RANGE		4			/* MotionGenerator少数点位置自動設定ビット範囲 */


/****************************************************************************************************/
/*																									*/
/*		Type Definitions																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Motion Command Structure(MtApi.c)																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MCI_CMD_ {
	LONG		TPos;		/* 目標位置[ref] */
	LONG		TSpeed;		/* 目標速度[ref/s] */
	LONG		Velocity;	/* VFF/速度指令 [ref/s],[2^30/MAX] */
	LONG		Trq;		/* トルク指令[MAX_TRQ/2^30] */
	ULONG		TrqLimitP;	/* 正側トルク制限[MAX_TRQ/2^30] */
	ULONG		TrqLimitN;	/* 負側トルク制限[MAX_TRQ/2^30] */
	LONG		AccRate;	/* 加速定数<Rev2.01> */
	LONG		DecRate;	/* 減速定数<Rev2.01> */
	ULONG		Option;		/* オプション指令 */
	UCHAR		HoldMod;	/* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	UCHAR		LatchSgnl;	/* ラッチ信号選択 0:C相,n:EXTn */
	BOOL		EnableRate;	/* 加速度指令有効<Rev2.01> */
	BOOL		RstReq;		/* モーションリセット要求 */
} MCI_CMD;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Command Structure(MtCtrl.c)																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MT_CMD_ {
	LONG	TPos;		    /* 目標位置[ref] */
	LONG	TSpeed;		    /* 目標速度[ref/s] */
	LONG	Velocity;	    /* VFF[re/s], 速度指令[2^30/MAX] */
	LONG	Trq;		    /* トルク指令[MAX_TRQ/2^30] */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	Mtype;		    /* モーションタイプ */
	UCHAR	bHold;	    	/* HOLD指令 */
	UCHAR	HoldMod;	    /* 停止方法選択 0: 減速停止1,1:急速停止,2減速停止2 */
	UCHAR	LatchSgnl;	    /* ラッチ信号選択 0:C相,n:EXTn */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	AccFilSel;	    /* フィルタ選択 0,3: なし, 1:指数,2:移動平均 */
	UCHAR	CtrlMode;	    /* 制御モード */
	UCHAR	CmdRstReq;	    /* コマンドリセット要求 */
	UCHAR	MtEenable;	    /* モーション有効 */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ServoOn;	    /* サーボON指令 */
	UCHAR	SensOn;		    /* センサON指令 */
	UCHAR	SvonLock;	    /* サーボON Lock */
	UCHAR	CmdRateOn;	    /* 加減速指令有効SW<Rev2.01> */
/*--------------------------------------------------------------------------------------------------*/
	LONG	Rdst;	        /* Registration distination offset [ref] */
	LONG	Rspd;		    /* Registration speed [ref/min] */
/*--------------------------------------------------------------------------------------------------*/
	LONG	TspdKx;			/* Speed Gain [spdref] -> [ref/cycle]									*/
//	LONG	Tlim;			/* ﾄﾙｸﾘﾐｯﾄ[%] */
} MT_CMD;

/*--------------------------------------------------------------------------------------------------*/
/*  Latch Command Structure																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MT_LATCH_ {
	UCHAR	Req;		/* ラッチ要求 */
	UCHAR	Cmp;		/* ラッチ完了 */
	UCHAR	Mode;		/* ラッチモード */
	UCHAR	Sgnl;		/* 選択信号 */

	UCHAR	LtDisable;	/* ラッチ無効 */
	UCHAR	SeqReq;		/* 連続ラッチ要求 */
	UCHAR	SeqCnt;		/* 連続ラッチシーケンスカウンタ */
	union {				/* 連続ラッチステータス */
		USHORT	wdCnt;
		UCHAR	cnt[2];	/* [0]:連続回数カウント,[1]:シーケンスカウント */
	}SeqSts;
} MT_LATCH;


/*--------------------------------------------------------------------------------------------------*/
/*  Motion Generator Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MG_PRM_ {
	LONG	accrate1;			/* 加速定数１ ***.*[ref/cyc^2] */
	LONG	accrate2;			/* 加速定数２ ***.*[ref/cyc^2] */
	LONG	altspd_a;			/* 加速定数切替速度 ***.*[ref/cyc] */
	LONG	decrate1;			/* 減速定数１ ***.*[ref/cyc^2] */
	LONG	decrate2;			/* 減速定数２ ***.*[ref/cyc^2] */
	LONG	altspd_d;			/* 加速定数切替速度 ***.*[ref/cyc] */
} MG_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Control Base Parameter																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MC_BASE_PRM_ {
	LONG	MotionCycle;		/* モーション制御周期[0.25us] */
	BOOL	FullcCtrl;			/* Fullc使用 */
	BOOL	Linear;				/* リニア使用 */
	BOOL	ot_zstop;			/* OT時ｾﾞﾛｸﾗﾝﾌﾟ */
	UCHAR	trefMode;			/* 速度制御時のトルク指令使用選択 */
	UCHAR	TrefModePos;		/* 位置制御時のトルク指令使用選択(INTERPOLATE系) */
	UCHAR	TLimModePos;		/* 位置制御時のトルク制限使用選択(POSING系) */

	LONG	PosingRateKx;		/* [10000ref/s^s]->[***.*ref/cyc^cyc]加減速度変換ノーマライズ定数 */
	LONG	PosingRateKxIx;		/* TODO:[(1000ref/min)/msec]->[***.*ref/cyc^cyc]加減速度変換ノーマライズ定数 */
	LONG	PosingSpdKx;		/* [ref/s]->[***.*ref/cyc]速度変換ノーマライズ定数 */
	LONG	PosingSpdKxIx;		/* TODO:[1000ref/min]->[***.*ref/cyc]速度変換ノーマライズ定数 */
	LONG	PosingSpdInvKx;		/* [***.*ref/cyc]->[ref/s]速度変換ノーマライズ定数 */
	LONG	RefToRpmKxIx;		/* TODO:[1000ref/min]->[rpm]速度変換ノーマライズ定数 */
	LONG	fspdRefKx;			/* [ref/通信周期]->[ref/s]速度変換ノーマライズ定数 */
	LONG	refToOsKx;			/* [指令単位/s]->[24bit/OS]速度変換ノーマライズ定数 */
	LONG	osToRefKx;			/* [24bit/OS]->[指令単位/s]速度変換ノーマライズ定数 */
	LONG	maxToOsKx;			/* [30bit/MAX]->[24bit/OS]速度変換ノーマライズ定数 */
	LONG	osToMaxKx;			/* [24bit/OS]->[30bit/MAX]速度変換ノーマライズ定数 */
	LONG	trqKx;				/* [24bit/MAX]->[%]ノーマライズ定数 */
	LONG	invTrqKx;			/* [%]->[24bit/MAX]ノーマライズ定数 */
	LONG	taccRateKx;			/* [(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	LONG	taccRate7Kx;		/* [2^7*(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	VOID	(*SetCmdAccRate)();/* 加速度指令計算関数ポインタ<Rev2.01> */

	UCHAR	vel_sft_exp;		/* 速度分解能小数点ビット数ACCFIL用 [bit] */
	UCHAR	vel_shift;			/* 速度分解能小数点ビット数MG用 [bit] */
	UCHAR	vel_shift_s;		/* 速度分解能少数切捨ビット数 [bit] */
	LONG	vel_max;			/* 最大速度(=2^(31-vel_shift))[ref/cyc] */
	LONG	seg_num;			/* セグメント分割回数 */
	ULONG	seg_max_ref;		/* セグメント入力最大 */
	ULONG	PosingSpdMax;		/* POSING最大速度 */
	ULONG	mgSpdMax;			/* MG内POSING最大速度 */
	MG_PRM	StopPrm;			/* 非常停止用減速パラメータ */
} MC_BASE_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Position Parameter																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct PSN_PRM_ {
	BOOL	sotEnableP;			/* 正転側ソフトリミット有効 */
	BOOL	sotEnableN;			/* 逆転側ソフトリミット有効 */
	BOOL	tposLimitEnable;	/* 指令ソフトリミット有効 */
	BOOL	absPG;				/* ｱﾌﾞｿPG使用 */
	LONG	absOfsset;			/* 絶対値PGｵﾌｾｯﾄ値(Pn803) */
	LONG	softLimit_P;		/* 正転側ソフトリミット値 */
	LONG	softLimit_N;		/* 逆転側ソフトリミット値 */
	LONG	ltRangeP;			/* Pn820:正側ラッチゾーン */
	LONG	ltRangeN;			/* Pn822:負側ラッチゾーン */
	ULONG	zPointWidth;		/* 原点位置範囲 */
	ULONG	psetWidth;			/* Near範囲 */
	ULONG	nearWidth;			/* Pset範囲 */
	SHORT	TdlyIn;				/* MPOS補正時間 */
	USHORT	SegNum;				/* 分割回数(モーション周期/サーボ周期) */
} PSN_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Sequence Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MT_SEQ_PRM_ {
	LONG	zretsp1;			/* ZRET ｱﾌﾟﾛｰﾁ速度1 [ref/sec -> ref/cycle] */
	LONG	zretsp2;			/* ZRET ｱﾌﾟﾛｰﾁ速度2 [ref/sec -> ref/cycle] */
	LONG	zret_dist;			/* ZRET最終走行距離 [ref] */
	LONG	ex_posing_dist;		/* EX_POSING最終走行距離 */
	ULONG	rampTimeOut;		/* 減速停止タイムアウト時間[モーション周期] */
	ULONG	tlimTimeCnt1;		/* 押し付け原点復帰ﾄﾙｸ制限検出時間1 */
	ULONG	tlimTimeCnt2;		/* 押し付け原点復帰ﾄﾙｸ制限検出時間2 */
	LONG	zret_dir;			/* 原点復帰方向 */
	LONG	zretOSLevel;		/* zret overspeed alarm level [2^24/MaxSpd] */
} MT_SEQ_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Acc filter Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union ACC_MATB_PRM_ {
	ULONG	lTb;		/* 移動平均回数(Total) */
	USHORT	wTb[2];		/* high word:移動平均回数, low word:移動平均補間回数  */
} MATB_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Acc filter Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct ACCFIL_PRM_ {
	LONG	expbias;			/* 指数加減速バイアス */
	LONG	exptb;				/* 指数加減速時定数 */
	MATB_PRM mafil;				/* 移動平均回数 */
} ACF_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Option Map Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct OPTION_MAP_ {
	UCHAR	accfilBit;			/* 加減速フィルタ選択ビット位置 */
	UCHAR	accfilBitMsk;		/* 加減速フィルタ選択ビットマスク */
	UCHAR	gselBit;			/* ゲイン選択ビット位置 */
	UCHAR	gselBitMsk;			/* ゲイン選択ビットマスク */
	UCHAR	vppiBit;			/* P_PI選択ビット位置 */
	UCHAR	vppiBitMsk;			/* P_PI選択ビットマスク */
	UCHAR	ppiclrBit;			/* 位置積分クリアビット位置 */
	UCHAR	ppiclrBitMsk;		/* 位置積分ビットマスク */
	UCHAR	pclBit;				/* P_CLビット位置 */
	UCHAR	pclBitMsk;			/* P_CLビットマスク */
	UCHAR	nclBit;				/* N_CLビット位置 */
	UCHAR	nclBitMsk;			/* N_CLビットマスク */
	UCHAR	outpBit;			/* OutSignalビット位置 */
	UCHAR	outpBitMsk;			/* OutSignalビットマスク */
	UCHAR	bankBit;			/* BankSelビット位置 */
	UCHAR	bankBitMsk;			/* BankSelビットマスク */
	UCHAR	ltBit;				/* LtDisableビット位置 */
	UCHAR	ltBitMsk;			/* LtDisableビットマスク */
} OPTION_MAP;

/*--------------------------------------------------------------------------------------------------*/
/*  Input Signal Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct INSIG_MAP_ {
	UCHAR	bitNo[8];
	USHORT	bitsMsk;
} INSIG_MAP;

/*--------------------------------------------------------------------------------------------------*/
/*  Sequencial Latch Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct LTSEQ_PRM_ {
	USHORT	seqNum;				/* シーケンス信号数 */
	USHORT	loopCnt;			/* 連続回数設定 */
	UCHAR	ltSgnl[8];			/* ラッチ信号 */
} LTSEQ_PRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion generator inout																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MG_INOUT_ {
	UCHAR	Mtype;			/* Motion Type */
	UCHAR	LastMtype;		/* Last Motion Type */
	USHORT	Ctrl;			/* Motion制御Option */
/*--------------------------------------------------------------------------------------------------*/
	LONG	DposIn;			/* 指令移動量[ref] */
	//LONG	Tspd;			/* POSING用目標速度[ref/s] */
	LONG	Tspd;			/* POSING用目標速度[ref/cycle] (暫定) */
	LONG	Vref;			/* 速度指令[30bit/MAX] */
	LONG	Tref;			/* トルク指令[30bit/MAX] */
/*--------------------------------------------------------------------------------------------------*/
	LONG	DposOut;		/* 出力移動量[ref](Vel->Pos) */
	LONG	VrefOut;		/* 出力速度指令[24bit/OS] */
	LONG	TrefOut;		/* 出力トルク指令[24bit/MAX] */
} MG_INOUT;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion generator handle																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MGHNDL_ {
	BOOL			cMinusDir;	/* 現在払い出し方向 */
	BOOL			tMinusDir;	/* 現在の指令位置から目標位置の方向 */
	ULONG			dpos0;		/* DPOSの少数部 for POSINGの指令速度（少数部）の積算 */
	LONG			dposRem;	/* 伝送周期移動量(dpos)余り         */
	LONG			Cspd;		/* 現在指令速度 */
	MC_BASE_PRM*	basePrm;	/* MG基本パラメータ */
	MG_PRM*			prm;		/* Motionパラメータ */
} MGHNDL;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Parameter																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MTPRM_ {
	MC_BASE_PRM BasePrm;		/* 基本パラメータ */
	MG_PRM		MgPrm;			/* MGモーションパラメータ */
	MG_PRM		CmdRate;		/* 加減速指令<Rev2.01> */
	PSN_PRM		PsnPrm;			/* PositionManagerパラメータ */
	MT_SEQ_PRM	MtSeqPrm;		/* Motion指令作成用パラメータ */
	//ACF_PRM		AccFilPrm;		/* 位置指令フィルタパラメータ */
	//OPTION_MAP	OptMap;			/* OPTION指令ビット位置設定パラメータ */
	//INSIG_MAP	InSigMap;		/* Input Signal0-7割付 */
#ifdef PRM_BANK_ENABLE
	//MTBANK		Bnk;			/* Motion PRM Bankデータ */
#endif
	//LTSEQ_PRM	LtPrm;
} MTPRM;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Initial Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MTPRM_INI_ {
	LONG	EGearA;			/* 電子ギアA(分母) */
	LONG	EGearB;			/* 電子ギアB(分子) */
	LONG	PgPuls;			/* PGパルス数[puls/rev],[puls/pitch] */
	LONG	MaxTrq;			/* MAXトルク[%] */
	LONG	OverSpd;		/* OS速度[rpm],[mm/s] */
	LONG	MaxSpd;			/* MAX速度[rpm],[mm/s] */
	LONG	ScalePitch;		/* スケールピッチ[mm/pitch] */
	SHORT	OverSpdExp;		/* OS速度指数 */
	SHORT	MaxSpdExp;		/* MAX速度指数 */
	SHORT	ScalePitchExp;	/* スケールピッチ指数 */
	USHORT	trefMode;		/* トルク制御Option */
	BOOL	ot_zstop;		/* OT時ｾﾞﾛｸﾗﾝﾌﾟ */
	BOOL	absPG;			/* ｱﾌﾞｿPG使用 */
	BOOL	FullcCtrl;		/* ﾌﾙｸﾛｰｽﾞﾄﾞ制御 */
	BOOL	Linear;			/* リニア使用 */

	USHORT	Cycle;			/* モーション制御周期[0.25us] */
	USHORT	SpdDiv;			/* 分解能 */
	ULONG	SegNum;			/* 補間送りセグメント回数 */
	USHORT	OptEnable;		/* Option割付有効 */
	USHORT	OptMap[5];		/* Option割付定義 */
	USHORT	OutSigEnable;	/* Option割付出力信号有効/無効 */
	SHORT	dlyTime;		/* 指令出力からFB入力までの時間 */
	USHORT	SvCycle;		/* サーボ周期(モーション周期/サーボ周期) */
/*--------------------------------------------------------------------------------------------------*/
	LONG	TspdGain;		/* 目標速度ゲイン */
/*--------------------------------------------------------------------------------------------------*/
} MTPRM_INI;

/*--------------------------------------------------------------------------------------------------*/
/*  Position Handle Structure																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct PSNHNDL_ {
	LONG		Tpos;		/* 目標位置 */
	LONG		Ipos;		/* 指令位置（フィルタ前） */
	LONG		Pos;		/* 指令位置（フィルタ後） */
	LONG		Mpos;		/* FB位置入力タイミングでの指令位置 */
	LONG		Apos;		/* FB位置 */
	LONG		Perr;		/* 偏差 */
	LONG		TlimPerr; /* ﾄﾙｸﾘﾐｯﾄ時の位置偏差 */
	LONG		Lpos;		/* ラッチ位置 */
	LONG		LstLpos;	/* 前回ラッチ位置 */
	LONG		Fbspd;		/* FB速度 */
//	LONG		FbSpdx;		/* FB速度[2ms平均] */
//	LONG		FbSpdSum;	/* FB速度[2ms平均]用Sum値 */
	LONG		Pgcnt;		/* 目標位置 */
	LONG		Dpos;		/* 今回出力移動距離 */
	LONG		lstDpos;	/* 前回出力移動距離 */
	BOOL		Den;		/* 指令出力完了 */
	BOOL		RefEnable;	/* リファレンス点有効 */
	BOOL		PosEnable;	/* 座標位置有効 */
	UCHAR		SoftLimit;	/* ソフトリミット状態 */
//	UCHAR		FbSpdSumCnt;/* FB速度[2ms平均]用ｶｳﾝﾀ */	
	PSN_PRM*	Prm;		/* Potionパラメータ */
} PSNHNDL;

/*--------------------------------------------------------------------------------------------------*/
/*  Motion Handle Structure																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct MTHNDL_ {
	UCHAR		seqState;	/* 指令生成シーケンス状態 */
	UCHAR		otState;	/* OT状態 */
//#if defined ( CCSW_SFTID ) && ( CCSW_SFTID == SFTID_EXIST )								/*<V717>Add:Start*/
//	UCHAR		amState;	/* AM状態 */
//#endif															/*<V717>Add:End  */
	UCHAR		mtState;	/* モーション状態 */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR		crtMtype;	/* 現在モーションタイプ */
	UCHAR		ctrlMode;	/* 現在制御モード */
	UCHAR		tlimSig[2]; /* トルクリミットフィルタ */
/*--------------------------------------------------------------------------------------------------*/
	LONG		ZretFspdSum;     /* feedback speed sum (for homing)                                 */
	LONG		ZretFspdSumCnt;  /* feedback speed sum count (for homing)                           */
/*--------------------------------------------------------------------------------------------------*/
	ULONG		timer;		/* RampStop用タイマ */
	LONG		segCnt;		/* モーション払い出し回数カウンタ */
	ULONG		tlimCnt;	/* トルクリミットタイマカウンタ */
/*--------------------------------------------------------------------------------------------------*/
	MT_CMD		cmdData;	/* モーション指令データ */
	MG_INOUT	outData;	/* モーション出力データ */
	MT_LATCH	latch;		/* ラッチ */
	MGHNDL		mtgHdl;		/* MotionGeneratorハンドル */
	PSNHNDL		psnHdl;		/* Positionハンドル */
	//ACFHNDL		acfHdl;		/* AccFilterハンドル */
	MTPRM		mtPrm;		/* MotionPRMハンドル */
/*--------------------------------------------------------------------------------------------------*/
	struct {
		MTPRM_INI	mt;		/* モーション初期パラメータデータ */
	} iniPrm;				/* 初期パラメータロードデータ */
/*--------------------------------------------------------------------------------------------------*/
	CYC_CMD*	svCmd;		/* Servo If Cyclic Command Data*/
	CYC_RSP*	svRsp;		/* Servo If Cyclic Response Data*/
} MTHNDL;

/****************************************************************************************************/
/*																									*/
/*		Grobal Function Prototypes																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  MtCtrl.c: Motion Control																		*/
/*--------------------------------------------------------------------------------------------------*/
extern VOID MtcInit(MTHNDL* hdl, SV_OPTION_IF* svData);				/* initialize motion */
extern VOID MtcOutputData(MTHNDL* hdl, SV_OPTION_IF *svData);		/* output motion command */
extern VOID MtcInputData(MTHNDL* hdl, SV_OPTION_IF *svData);		/* input motion data */
extern VOID MtcCreateInCmd(MTHNDL* hdl);							/* create motion in command */
void	MtcSetHomeMethod(long prm);							        /* set homing method */
/*--------------------------------------------------------------------------------------------------*/
/*  MtGenerator.c: Motion Generator																	*/
/*--------------------------------------------------------------------------------------------------*/
extern BOOL MtgGenerate(MGHNDL* hdl, MG_INOUT* inOut);				/* generate motion */
/*--------------------------------------------------------------------------------------------------*/
/*  MtPRM.c: Motion Parameter																		*/
/*--------------------------------------------------------------------------------------------------*/
LONG MPrmCalcInit(MTPRM* prm, MTPRM_INI* iniPrm);					/* calculate motion parameters  */
LONG MPrmSetZretDir(MTPRM* prm, BOOL data);							/* set homing dir               */
LONG MPrmSetZretSpd1(MTPRM* prm, LONG data);						/* set homing approach spd      */
LONG MPrmSetZretSpd2(MTPRM* prm, LONG data);						/* set homing creap spd         */
LONG MPrmSetZretDist(MTPRM* prm, LONG data);						/* set homing distination       */
LONG MPrmSetZretOSLvl(MTPRM* prm, LONG data);						/* set homing overspeed level   */
LONG MPrmSetSoftLimitFlag(MTPRM* prm, USHORT data);					/* sot enable or disable 		*/
LONG MPrmSetSoftLimitP(MTPRM* prm, LONG data);						/* set positive sot 			*/
LONG MPrmSetSoftLimitN(MTPRM* prm, LONG data);						/* set negative sot 			*/
LONG MPrmSetTlimTimeOut1(MTPRM* prm, LONG data);					/* set homing timeout1          */
LONG MPrmSetTlimTimeOut2(MTPRM* prm, LONG data);					/* set homing timeout2          */
/*--------------------------------------------------------------------------------------------------*/
/*  Position.c: Position Manager																	*/
/*--------------------------------------------------------------------------------------------------*/
extern VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm);
extern VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt);
extern BOOL PsnGetPsetSts(PSNHNDL* hdl);
extern BOOL PsnGetNearSts(PSNHNDL* hdl);
extern BOOL PsnGetZPointSts(PSNHNDL* hdl);
extern LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos);
extern LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos);
extern VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err);
extern UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos);
extern UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot);
extern VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos);
extern VOID PsnResetApos(PSNHNDL* hdl, BOOL abs);
extern BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt);
#define	PsnSetRefPoint(hdl, enable)	((hdl)->RefEnable = enable)
#define	PsnResetIPOS(hdl)			((hdl)->Ipos = (hdl)->Pos)
#define	PsnSetTPOSNoLimit(hdl,dpos)	((hdl)->Tpos = dpos + (hdl)->Ipos)
#define	PsnSetPOS(hdl, dpos)		{(hdl)->Pos += dpos;\
									 (hdl)->lstDpos = (hdl)->Dpos;\
									 (hdl)->Dpos = dpos;}
#define	PsnSetIPOS(hdl, dpos)		((hdl)->Ipos += dpos)
#define	PsnSetDenSts(hdl, den)		((hdl)->Den = den)
#define	PsnGetDenSts(hdl)			((hdl)->Den)
#define	PsnGetTPOS(hdl)				((hdl)->Tpos)
#define	PsnGetIPOS(hdl)				((hdl)->Ipos)
#define	PsnGetPOS(hdl)				((hdl)->Pos)
#define	PsnGetMPOS(hdl)				((hdl)->Mpos)
#define	PsnGetLPOS(hdl)				((hdl)->Lpos)
#define	PsnGetLstLPOS(hdl)			((hdl)->LstLpos)
#define	PsnGetAPOS(hdl)				((hdl)->Apos)
#define	PsnGetPERR(hdl)				((hdl)->Perr)
#define	PsnGetSOTSts(hdl)			((hdl)->SoftLimit)
#define	PsnGetFBSPD(hdl)			((hdl)->Fbspd)
#define	PsnAttachPRM(hdl, prm)		((hdl)->Prm = prm)
/*--------------------------------------------------------------------------------------------------*/
/*  MtApi.c: Motion API																				*/
/*--------------------------------------------------------------------------------------------------*/
extern INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd);
extern VOID MciSetOptionCmd(MTHNDL* hdl, ULONG option);

extern INT32 MciEnableState(MTHNDL* hdl, BOOL enable);
extern INT32 MciResetMotion(MTHNDL* hdl);
extern INT32 MciSetServoOn(MTHNDL* hdl, BOOL enable, BOOL reset);
extern INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable);

extern INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
extern INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode);
extern INT32 MciSetCordinatePos(MTHNDL* hdl, LONG data, BOOL ref);

extern VOID MciOutputSignals(MTHNDL* hdl, USHORT signl);
extern VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable);
/*--------------------------------------------------------------------------------------------------*/
extern UCHAR MciGetMType(MTHNDL* hdl);
extern BOOL MciGetServoOnStatus(MTHNDL* hdl);
extern BOOL MciGetSensOnStatus(MTHNDL* hdl);
extern UCHAR MciGetCtrlMode(MTHNDL* hdl);
extern LONG MciGetMonitorData(MTHNDL* hdl, USHORT sel, UCHAR unit);
extern BOOL MciGetDenStatus(MTHNDL* hdl);
extern BOOL MciGetLatchStatus(MTHNDL* hdl ,UCHAR *ltEdge);
extern USHORT MciGetLatchSeqStatus(MTHNDL* hdl);

extern BOOL MciGetInPosStatus(MTHNDL* hdl);
extern BOOL MciGetNearStatus(MTHNDL* hdl);
extern BOOL MciGetZpointStatus(MTHNDL* hdl);
extern UCHAR MciGetSoftLimitStatus(MTHNDL* hdl);
extern LONG MciGetOutputCmdDist(MTHNDL* hdl);

//extern BOOL MciGetZspdStatus(MTHNDL* hdl);
//extern BOOL MciGetMainPowerStatus(MTHNDL* hdl);
extern BOOL MciGetVlimitStatus(MTHNDL* hdl);
extern BOOL MciGetTrqLimitStatus(MTHNDL* hdl);
extern BOOL MciGetVcmpStatus(MTHNDL* hdl);

extern USHORT MciGetInputSignals(MTHNDL* hdl);
extern USHORT MciGetInPortSignals(MTHNDL* hdl);

#if defined( CCSW_SFTID  ) && ( CCSW_SFTID == SFTID_EXIST )						/*<V717>Add:Start*/
extern USHORT MciGetSomStatus(void);
#endif													/*<V717>Add:End  */

/*--------------------------------------------------------------------------------------------------*/
//#if defined(__ghs__)
//#pragma ghs startzda
//#elif !defined(WIN32)
//#pragma section sidata begin
//#endif
extern MTHNDL	MtHdl;							/* motion handle */
//extern void	(*pIxxAxctrlReqHoming)( MT_CMD *mtcmd, long tspeed,
//									 MT_LATCH_SGNL sgnl );    /* Pointer Homing request    			*/
//#if defined(__ghs__)
//#pragma ghs endzda
//#elif !defined(WIN32)
//#pragma section sidata end
//#endif

#endif /* _MOTION_H_ */
