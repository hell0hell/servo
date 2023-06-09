/****************************************************************************
Description	: MECHATROLINK Motion Paramter
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef		MOTOIN_PRM_H_

#include "UnitPrmStruct.h"

/****************************************************************************/
/* モーション動作機能SW： 無効にする場合はコメントにする					*/
/****************************************************************************/
#define PRM_BANK_ENABLE		/* バンク切り替え有効SW */
//#define ACC_FIL_DISABLE	/* フィルタ無効SW */
//#define POSING_1RATEONLY	/* POSING 1段目無効SW */

#define MAX_TRQ24			0x1000000	/* MAXTRQ */
#define MAX_SPD24			0x1000000	/* MAX速度 */
#define DISTANCE_MAX		0x7FFFFFFF	/* 移動距離MAX */
#define ACF_MAX_SZ			512			/* 移動平均フィルタバッファ数 */
#define FOLLOWUP_MAX_REF	32767		/* FollowUp時最大移動量[ref/cyc] */
#define MG_VREMBIT_MAX		24			/* MotionGenerator余り速度演算MAXビット数 */
#define MG_SPDDIV_RANGE		6			/* MotionGenerator少数点位置自動設定ビット範囲 */
#define MG_DEFAULT_SPDDIV	11			/* MotionGenerator少数点ビット数(250us時) *//* <S006> */


#define BANK_NUM_MAX		16	/* 最大バンク数 */
#define BANK_MEM_MAX		15	/* 最大メンバ数 */

/****************************************************************************
 Structure of the Motion Parameters
****************************************************************************/
typedef struct MG_PRM_ {
	LONG	accrate1;			/* 加速定数１ ***.*[ref/cyc^2] */
	LONG	accrate2;			/* 加速定数２ ***.*[ref/cyc^2] */
	LONG	altspd_a;			/* 加速定数切替速度 ***.*[ref/cyc] */
	LONG	decrate1;			/* 減速定数１ ***.*[ref/cyc^2] */
	LONG	decrate2;			/* 減速定数２ ***.*[ref/cyc^2] */
	LONG	altspd_d;			/* 加速定数切替速度 ***.*[ref/cyc] */
} MG_PRM;

typedef struct MC_BASE_PRM_ {
	LONG	MotionCycle;		/* モーション制御周期[0.25us] */
//	BOOL	FullcCtrl;			/* Fullc使用 */
//	BOOL	Linear;				/* リニア使用 */
//	BOOL	ot_zstop;			/* OT時ｾﾞﾛｸﾗﾝﾌﾟ */
	LONG   	trefMode;			/* 速度制御時のトルク指令使用選択 */
	LONG   	TrefModePos;		/* 位置制御時のトルク指令使用選択(INTERPOLATE系) */
	LONG   	TLimModePos;		/* 位置制御時のトルク制限使用選択(POSING系) */

	KSGAIN	PosingRateKx;		/* [10000ref/s^s]->[***.*ref/cyc^cyc]加減速度変換ノーマライズ定数 */
	KSGAIN	PosingSpdKx;		/* [ref/s]->[***.*ref/cyc]速度変換ノーマライズ定数 */
	KSGAIN	PosingSpdInvKx;		/* [***.*ref/cyc]->[ref/s]速度変換ノーマライズ定数 */
	KSGAIN	fspdRefKx;			/* [ref/通信周期]->[ref/s]速度変換ノーマライズ定数 */
	KSGAIN	refToOsKx;			/* [指令単位/s]->[24bit/OS]速度変換ノーマライズ定数 */
	KSGAIN	osToRefKx;			/* [24bit/OS]->[指令単位/s]速度変換ノーマライズ定数 */
	KSGAIN	maxToOsKx;			/* [30bit/MAX]->[24bit/OS]速度変換ノーマライズ定数 */
	KSGAIN	osToMaxKx;			/* [24bit/OS]->[30bit/MAX]速度変換ノーマライズ定数 */
	KSGAIN	trqKx;				/* [24bit/MAX]->[%]ノーマライズ定数 */
	KSGAIN	taccRateKx;			/* [(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	KSGAIN	taccRate7Kx;		/* [2^7*(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	VOID	(*SetCmdAccRate)();/* 加速度指令計算関数ポインタ<Rev2.01> */

	LONG	vel_sft_exp;		/* 速度分解能小数点ビット数ACCFIL用 [bit] */
	LONG	vel_shift;			/* 速度分解能小数点ビット数MG用 [bit] */
	LONG	vel_shift_s;		/* 速度分解能少数切捨ビット数 [bit] */
	LONG	vel_max;			/* 最大速度(=2^(31-vel_shift))[ref/cyc] */
	LONG	seg_num;			/* セグメント分割回数 */
	ULONG	seg_max_ref;		/* セグメント入力最大 */
	ULONG	PosingSpdMax;		/* POSING最大速度 */
	ULONG	mgSpdMax;			/* MG内POSING最大速度 */
	MG_PRM	StopPrm;			/* 非常停止用減速パラメータ */

	LONG	opMon[2];			/* オプションモニタ *//* <S004> */
} MC_BASE_PRM;

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
	ULONG	psetWidth;			/* Pset範囲 */
	ULONG	nearWidth;			/* Near範囲 */
	ULONG	TdlyIn;				/* MPOS補正時間 */
	ULONG	SegNum;				/* 分割回数(モーション周期/サーボ周期) */
} PSN_PRM;

typedef struct MT_SEQ_PRM_ {
	LONG	zretsp1;			/* ZRET ｱﾌﾟﾛｰﾁ速度1 [ref/sec] */
	LONG	zretsp2;			/* ZRET ｱﾌﾟﾛｰﾁ速度2 [ref/sec] */
	LONG	zret_dist;			/* ZRET最終走行距離 */
	LONG	ex_posing_dist;		/* EX_POSING最終走行距離 */
	ULONG	rampTimeOut;		/* 減速停止タイムアウト時間[モーション周期] */
	LONG	zret_dir;			/* 原点復帰方向 */
} MT_SEQ_PRM;

typedef	union ACC_MATB_PRM_ {
	ULONG	lTb;		/* 移動平均回数(Total) */
	USHORT	wTb[2];		/* high word:移動平均回数, low word:移動平均補間回数  */
} MATB_PRM;

typedef struct ACCFIL_PRM_ {
	LONG	expbias;			/* 指数加減速バイアス */
	LONG	exptb;				/* 指数加減速時定数 */
	MATB_PRM mafil;				/* 移動平均回数 */
} ACF_PRM;

typedef struct OPTION_MAP_ {
	LONG	accfilBit;			/* 加減速フィルタ選択ビット位置 */
	LONG	accfilBitMsk;		/* 加減速フィルタ選択ビットマスク */
	LONG	gselBit;			/* ゲイン選択ビット位置 */
	LONG	gselBitMsk;			/* ゲイン選択ビットマスク */
	LONG	vppiBit;			/* P_PI選択ビット位置 */
	LONG	vppiBitMsk;			/* P_PI選択ビットマスク */
	LONG	ppiclrBit;			/* 位置積分クリアビット位置 */
	LONG	ppiclrBitMsk;		/* 位置積分ビットマスク */
	LONG	pclBit;				/* P_CLビット位置 */
	LONG	pclBitMsk;			/* P_CLビットマスク */
	LONG	nclBit;				/* N_CLビット位置 */
	LONG	nclBitMsk;			/* N_CLビットマスク */
	LONG	outpBit;			/* OutSignalビット位置 */
	LONG	outpBitMsk;			/* OutSignalビットマスク */
	LONG	bankBit;			/* BankSelビット位置 */
	LONG	bankBitMsk;			/* BankSelビットマスク */
	LONG	ltBit;				/* LtDisableビット位置 */
	LONG	ltBitMsk;			/* LtDisableビットマスク */
} OPTION_MAP;

typedef struct INSIG_MAP_ {
	LONG	bitNo[8];
	LONG	bitsMsk;
} INSIG_MAP;

typedef struct LTSEQ_PRM_ {
	LONG	seqNum;				/* シーケンス信号数 */
	LONG	loopCnt;			/* 連続回数設定 */
	LONG	ltSgnl[8];			/* ラッチ信号 */
} LTSEQ_PRM;


typedef struct tagMTBANK {
	LONG		bankNum;				/* バンク数 */
	LONG		memNum;					/* メンバ数 */
	LONG		bankSel;				/* バンクセレクタ現在値 */
	LONG		cmdSel;					/* バンクセレクタ指令値 */
	ULONG		enaIdxBits;				/* 割付table */
	struct {
		LONG	idx;					/* 割付メンバ定義テーブルへのIdx */
		LONG	highWd;					/* 4byteメンバ時のHigh Word指定 */
	}memAttr[BANK_MEM_MAX];				/* メンバ割付テーブル */
	MG_PRM		mgPrm[BANK_NUM_MAX];	/* MG用バンクデータバッファ */
	ACF_PRM		acfPrm[BANK_NUM_MAX];	/* AccFilter用バンクデータバッファ */
} MTBANK;


typedef struct MT_PRM_ {
	MC_BASE_PRM BasePrm;		/* 基本パラメータ */
	MG_PRM		MgPrm;			/* MGモーションパラメータ */
	MG_PRM		CmdRate;		/* 加減速指令<Rev2.01> */
	PSN_PRM		PsnPrm;			/* PositionManagerパラメータ */
	MT_SEQ_PRM	MtSeqPrm;		/* Motion指令作成用パラメータ */
	ACF_PRM		AccFilPrm;		/* 位置指令フィルタパラメータ */
	OPTION_MAP	OptMap;			/* OPTION指令ビット位置設定パラメータ */
	INSIG_MAP	InSigMap;		/* Input Signal0-7割付 */
#ifdef PRM_BANK_ENABLE
	MTBANK		Bnk;			/* Motion PRM Bankデータ */
#endif
	LTSEQ_PRM	LtPrm;
} MT_PRM;


typedef struct MTPRM_INI_ {
	LONG	EGearA;			/* 電子ギアA(分母) */
	LONG	EGearB;			/* 電子ギアB(分子) */
	LONG	PgPuls;			/* PGパルス数[puls/rev],[puls/pitch] */
	LONG	MaxTrq;			/* MAXトルク[%] */
	LONG	OverSpd;		/* OS速度[rpm],[mm/s] */
	LONG	MaxSpd;			/* MAX速度[rpm],[mm/s] */
	LONG	ScalePitch;		/* スケールピッチ[mm/pitch] */
	LONG	OverSpdExp;		/* OS速度指数 */
	LONG	MaxSpdExp;		/* MAX速度指数 */
	LONG	ScalePitchExp;	/* スケールピッチ指数 */
	LONG	trefMode;		/* トルク制御Option */
//	BOOL	ot_zstop;		/* OT時ｾﾞﾛｸﾗﾝﾌﾟ */
	BOOL	absPG;			/* ｱﾌﾞｿPG使用 */
	BOOL	FullcCtrl;		/* ﾌﾙｸﾛｰｽﾞﾄﾞ制御 */
	BOOL	LinearMotor;	/* リニア使用 */

	LONG	Cycle;			/* モーション制御周期[0.25us] */
	LONG	SpdDiv;			/* 分解能 */
	ULONG	SegNum;			/* 補間送りセグメント回数 */
	LONG	OptEnable;		/* Option割付有効 */
	LONG	OptMap[5];		/* Option割付定義 */
	LONG	OutSigEnable;	/* Option割付出力信号有効/無効 */
	LONG	dlyTime;		/* 指令出力からFB入力までの時間 */
	USHORT	SvCycle;		/* サーボ周期(モーション周期/サーボ周期) */
} MTPRM_INI;


/****************************************************************************
 API's
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *prm, MTPRM_INI *iniPrm, PRMDATA *pprm_ptr );
LONG MPrmSetZretDir( MT_PRM *prm, BOOL data );
VOID MPrmCalcAccTimeRate(MT_PRM *prm, LONG Tspd, LONG Tacc, LONG Tdec);
LONG MPrmSetAccAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data);
LONG MPrmSetDecAltSpd(MT_PRM *prm, MG_PRM *mgPrm, LONG data);
LONG MPrmSetExpFilBias(MT_PRM *prm, ACF_PRM *acfPrm, LONG bias);
LONG MPrmSetExpFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG exptb);
LONG MPrmSetMaFilTb(MT_PRM *prm, ACF_PRM *acfPrm, ULONG matb);
LONG MPrmSetInSigMap(MT_PRM* prm, LONG data);

LONG MPrmSetAccRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);
LONG MPrmSetAccRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);
LONG MPrmSetDecRate1(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);
LONG MPrmSetDecRate2(MT_PRM *prm, MG_PRM *mgPrm, LONG rate);


/* 操作関数マクロ定義 */

#define		MOTOIN_PRM_H_
#endif /* MOTOIN_PRM_H_ */

