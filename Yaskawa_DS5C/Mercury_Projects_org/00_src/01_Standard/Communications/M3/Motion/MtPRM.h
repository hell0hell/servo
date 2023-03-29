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
#ifndef MOTOIN_PRM_H_

#include "UnitPrmStruct.h"


/****************************************************************************/
/* モーション動作機能SW： 無効にする場合はコメントにする					*/
/****************************************************************************/
#define BASE_SHIFT					9			/* べき乗演算用基本シフト数 */

#define MAX_TRQ24					0x1000000		/* MAXTRQ */
#define MAX_SPD24					0x1000000		/* MAX速度 */
#define DISTANCE_MAX				0x7FFFFFFF		/* 移動距離MAX */
#define ACF_MAX_SZ					512				/* 移動平均フィルタバッファ数 */
#define FOLLOWUP_MAX_REF			32767			/* FollowUp時最大移動量[ref/cyc] */
#define MG_VREMBIT_MAX				24				/* MotionGenerator余り速度演算MAXビット数 */
#define MG_SPDDIV_RANGE				6				/* MotionGenerator少数点位置自動設定ビット範囲 */
#define MG_DEFAULT_SPDDIV			11				/* MotionGenerator少数点ビット数(250us時) */

#define ZRET_DIST_DIR_IS_ABSOLUTE	0				/* 原点復帰最終走行距離 絶対方向指定 */
#define ZRET_DIST_DIR_IS_RELATIVE	1				/* 原点復帰最終走行距離 相対方向指定 */
#define ZRET_LATCH_IS_CANCEL		0				/* ZRETコマンド中のコマンドの切り替わりによるラッチキャンセル動作の有効 */
#define ZRET_LATCH_IS_NOCANCEL		1				/* ZRETコマンド中のコマンドの切り替わりによるラッチキャンセル動作の無効 */

#define BANK_NUM_MAX	16							/* 最大バンク数 */
#define BANK_MEM_MAX	15							/* 最大メンバ数 */


/****************************************************************************/
/* Vendor Specifics I/O割り付け定義											*/
/****************************************************************************//* <S1C0> */
//#define VENDOR_IOPRM_NUM	4						/* SVCMD_IO(出力)VendorSpecificsI/O割り付けパラメータ数		*/
#define VENDOR_IOBIT_NUM	(VENDOR_IOPRM_NUM * 4)	/* SVCMD_IO(出力)VendorSpecificsI/O割り付け対象ビット数		*/
#define VENDOR_IOBIT_OFFSET	15						/* SVCMD_IO(出力)VendorSpecificsI/Oのオフセット量			*/

#define VENDOR_IOBIT_BANK	0						/* SVCMD_IO(出力)VendorSpecificsI/O.Bank					*/
#define VENDOR_IOBIT_SO1	1						/* SVCMD_IO(出力)VendorSpecificsI/O.SO1						*/
#define VENDOR_IOBIT_SO2	2						/* SVCMD_IO(出力)VendorSpecificsI/O.SO2						*/
#define VENDOR_IOBIT_SO3	3						/* SVCMD_IO(出力)VendorSpecificsI/O.SO3						*/
#define VENDOR_IOBIT_SO4	4						/* SVCMD_IO(出力)VendorSpecificsI/O.SO4	(for SGD7W)			*/
#define VENDOR_IOBIT_SO5	5						/* SVCMD_IO(出力)VendorSpecificsI/O.SO5	(for SGD7W)			*/
#define VENDOR_IOBIT_JSEL	6						/* SVCMD_IO(出力)VendorSpecificsI/O.JSEL					*/
#define VENDOR_IOBIT_SRATIO	8						/* SVCMD_IO(出力)VendorSpecificsI/O.SRATIO					*/
#define VENDOR_IOBIT_LTREQ1	9						/* SVCMD_IO(出力)VendorSpecificsI/O.LT_REQ_EX1 (for FT)		*/
#define VENDOR_IOBIT_LTREQ2	10						/* SVCMD_IO(出力)VendorSpecificsI/O.LT_REQ_EX2 (for FT)		*/
#define VENDOR_IOBIT_LTREQ3	11						/* SVCMD_IO(出力)VendorSpecificsI/O.LT_REQ_EX3 (for FT)		*/


/****************************************************************************
 Structure of the Motion Parameters
****************************************************************************/
typedef struct MG_PRM_ {
	LONG	accrate1;				/* 加速定数１ ***.*[ref/cyc^2] */
	LONG	accrate2;				/* 加速定数２ ***.*[ref/cyc^2] */
	LONG	altspd_a;				/* 加速定数切替速度 ***.*[ref/cyc] */
	LONG	decrate1;				/* 減速定数１ ***.*[ref/cyc^2] */
	LONG	decrate2;				/* 減速定数２ ***.*[ref/cyc^2] */
	LONG	altspd_d;				/* 加速定数切替速度 ***.*[ref/cyc] */
	LONG	Sratio;					/* S字加減速比率(SPOSING用)[%] */
	
	LONG	accr_vel;				/* 加速度(速度指令用) ***.*[ref/cyc^2] */
	LONG	decr_vel;				/* 減速度(速度指令用) ***.*[ref/cyc^2] */
} MG_PRM;


typedef struct M3CONV_UNIT_ {
	LONG	m3SpdSelUnit;			/* 速度単位選択		0～4							*/
	LONG	m3AccSelUnit;			/* 加速度単位選択	0～1							*/
	LONG	m3TrqSelUnit;			/* トルク単位選択	0～2							*/
	LONG	m3PosSelUnit;			/* 位置		変換なし								*/

	KSGAIN	m3SpdRefToUnit;			/* m3SpdSelUnit=0: 変換なし										*/
									/* m3SpdSelUnit=1: 指令単位/sec			--> 指令単位/min		*/
									/* m3SpdSelUnit=2: 指令単位/sec			--> 定格速度に対する%	*/
									/* m3SpdSelUnit=3: 指令単位/sec			--> RPM					*/
									/* m3SpdSelUnit=4: 指令単位/sec			--> 最大速度/40000000h	*/
	KSGAIN	m3SpdUnitToRef;			/* m3SpdSelUnit=0: 変換なし										*/
									/* m3SpdSelUnit=1: 指令単位/min			--> 指令単位/sec		*/
									/* m3SpdSelUnit=2: 定格速度に対する%	--> 指令単位/sec		*/
									/* m3SpdSelUnit=3: RPM					--> 指令単位/sec		*/
									/* m3SpdSelUnit=4: 最大速度/40000000h	--> 指令単位/sec		*/
	KSGAIN	m3SpdRpmToUnit;			/* m3SpdSelUnit=0: RPM					--> 指令単位/sec		*/
									/* m3SpdSelUnit=1: RPM					--> 指令単位/min		*/
									/* m3SpdSelUnit=2: RPM					--> 定格速度に対する%	*/
									/* m3SpdSelUnit=3: 変換なし										*/
									/* m3SpdSelUnit=4: RPM					--> 最大速度/40000000h	*/
	KSGAIN	m3SpdUnitToMax;			/* m3SpdSelUnit=0: 指令単位/sec			--> 最大速度/40000000h	*/
									/* m3SpdSelUnit=1: 指令単位/min			--> 最大速度/40000000h	*/
									/* m3SpdSelUnit=2: 定格速度に対する%	--> 最大速度/40000000h	*/
									/* m3SpdSelUnit=3: RPM					--> 最大速度/40000000h	*/
									/* m3SpdSelUnit=4: 変換なし										*/
	KSGAIN	m3SpdMaxToUnit;			/* m3SpdSelUnit=0: 最大速度/40000000h	--> 指令単位/sec		*/
									/* m3SpdSelUnit=1: 最大速度/40000000h	--> 指令単位/min		*/
									/* m3SpdSelUnit=2: 最大速度/40000000h	--> 定格速度に対する%	*/
									/* m3SpdSelUnit=3: 最大速度/40000000h	--> RPM					*/
									/* m3SpdSelUnit=4: 変換なし										*/
	KSGAIN	m3Spd24OsToUnit;		/* m3SpdSelUnit=0: 24BitOs				--> 指令単位/sec		*/
									/* m3SpdSelUnit=1: 24BitOs				--> 指令単位/min		*/
									/* m3SpdSelUnit=2: 24BitOs				--> 定格速度に対する%	*/
									/* m3SpdSelUnit=3: 24BitOs				--> RPM					*/
									/* m3SpdSelUnit=4: 24BitOs				--> 最大速度/40000000h	*/

	KSGAIN	m3PosUnitToRef;			/* m3PosSelUnit=0: 変換なし										*/
	KSGAIN	m3PosRefToUnit;			/* m3PosSelUnit=0: 変換なし										*/

	KSGAIN	m3AccUnitToCyc2;		/* m3AccSelUnit=0: 指令単位/sec^2	 --> **.*** 指令単位/cyc^2	*/
									/* m3AccSelUnit=1: 加速時間(ms)		 --> 加速度(指令単位/sec^2)	*/
	KSGAIN	m3AccUnitToOs;			/* m3AccSelUnit=0: 加速度(ref/sec^2) --> OS速度/cyc				*/
									/* m3AccSelUnit=1: 加速時間(ms)		 --> OS速度/cyc				*/

	KSGAIN	m3TrqMaxToUnit;			/* m3TrqSelUnit=0: 最大トルク/40000000h --> Nm(N)				*/
									/* m3TrqSelUnit=1: 最大トルク/40000000h --> 定格トルクに対する%	*/
									/* m3TrqSelUnit=2: 変換なし										*/
	KSGAIN	m3TrqUnitToMax;			/* m3TrqSelUnit=0: Nm(N)			   --> 最大トルク/40000000h	*/
									/* m3TrqSelUnit=1: 定格トルクに対する% --> 最大トルク/40000000h	*/
									/* m3TrqSelUnit=2: 変換なし										*/
	KSGAIN	m3ReftoRpm;				/* 指令単位/s → RPM											*/
	KSGAIN	m3RpmtoRef;				/* RPM → 指令単位/s											*/
} M3CONV_UNIT;


typedef struct MC_BASE_PRM_ {
	LONG	MotionCycle;			/* モーション制御周期[0.25us] */
//	LONG	trefMode;				/* 速度制御時のトルク指令使用選択 */
	LONG	TrefModePos;			/* 位置制御時のトルク指令使用選択(INTERPOLATE系) */
//	LONG	TLimModePos;			/* 位置制御時のトルク制限使用選択(POSING系) */
	LONG	vLimModeTrq;			/* トルク制御時の速度制限使用選択 */

	KSGAIN	PosingRateKx;			/* [10000ref/s^s]->[***.*ref/cyc^cyc]加減速度変換ノーマライズ定数 */
	KSGAIN	PosingSpdKx;			/* [ref/s]->[***.*ref/cyc]速度変換ノーマライズ定数 */
	KSGAIN	PosingSpdInvKx;			/* [***.*ref/cyc]->[ref/s]速度変換ノーマライズ定数 */
	KSGAIN	fspdRefKx;				/* [ref/通信周期]->[ref/s]速度変換ノーマライズ定数 */
	KSGAIN	refToOsKx;				/* [指令単位/s]->[24bit/OS]速度変換ノーマライズ定数 */
	KSGAIN	osToRefKx;				/* [24bit/OS]->[指令単位/s]速度変換ノーマライズ定数 */
	KSGAIN	maxToOsKx;				/* [30bit/MAX]->[24bit/OS]速度変換ノーマライズ定数 */
	KSGAIN	osToMaxKx;				/* [24bit/OS]->[30bit/MAX]速度変換ノーマライズ定数 */
	KSGAIN	trqKx;					/* [24bit/MAX]->[%]ノーマライズ定数 */
//	KSGAIN	taccRateKx;				/* [(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
//	KSGAIN	taccRate7Kx;			/* [2^7*(ref/s)/(0.1ms)]->***.*[ref/cyc^2]<Rev2.01> */
	VOID	(*SetCmdAccRate)();		/* 加速度指令計算関数ポインタ<Rev2.01> */

	LONG	vel_sft_exp;			/* 速度分解能小数点ビット数ACCFIL用 [bit] */
	LONG	vel_shift;				/* 速度分解能小数点ビット数MG用 [bit] */
	LONG	vel_shift_s;			/* 速度分解能少数切捨ビット数 [bit] */
	LONG	vel_max;				/* 最大速度(=2^(31-vel_shift))[ref/cyc] */
	LONG	seg_num;				/* セグメント分割回数 */
	ULONG	seg_max_ref;			/* セグメント入力最大 */
	ULONG	PosingSpdMax;			/* POSING最大速度 */
	ULONG	mgSpdMax;				/* MG内POSING最大速度 */
	ULONG	osVel;					/* OS速度[ref/s] */
	MG_PRM	StopPrm;				/* 非常停止用減速パラメータ */

	M3CONV_UNIT m3ConvUnit;			/* 単位変換	*/
	LONG		m3_selMon1;			/* 共通モニタ選択1 */
	LONG		m3_selMon2;			/* 共通モニタ選択2 */
	LONG	opMon[2];				/* オプションモニタ */
} MC_BASE_PRM;


typedef struct PSN_PRM_ {
	BOOL	sotEnableP;				/* 正転側ソフトリミット有効 */
	BOOL	sotEnableN;				/* 逆転側ソフトリミット有効 */
	BOOL	tposLimitEnable;		/* 指令ソフトリミット有効 */
	BOOL	absPG;					/* ｱﾌﾞｿPG使用 */
	LONG	absOfsset;				/* 絶対値PGｵﾌｾｯﾄ値(Pn803) */
	LONG	softLimit_P;			/* 正転側ソフトリミット値 */
	LONG	softLimit_N;			/* 逆転側ソフトリミット値 */
	LONG	ltRangeP;				/* Pn820:正側ラッチゾーン */
	LONG	ltRangeN;				/* Pn822:負側ラッチゾーン */
	ULONG	zPointWidth;			/* 原点位置範囲 */
	ULONG	psetWidth;				/* Pset範囲 */
	ULONG	nearWidth;				/* Near範囲 */
	LONG	TdlyIn;					/* MPOS補正時間 */
	LONG	SegNum;					/* 分割回数(モーション周期/サーボ周期) */
} PSN_PRM;


typedef struct MT_SEQ_PRM_ {
	LONG	zretsp1;				/* ZRET ｱﾌﾟﾛｰﾁ速度1 [ref/sec] */
	LONG	zretsp2;				/* ZRET ｱﾌﾟﾛｰﾁ速度2 [ref/sec] */
	LONG	zret_dist;				/* ZRET最終走行距離 */
	LONG	ex_posing_dist;			/* EX_POSING最終走行距離 */
	ULONG	rampTimeOut;			/* 減速停止タイムアウト時間[モーション周期] */
	LONG	zret_dir;				/* 原点復帰方向 */
	LONG	zretsp1_org;			/* ZRET ｱﾌﾟﾛｰﾁ速度1（パラメータ設定値）[ref/sec] */
	LONG	zretsp2_org;			/* ZRET ｱﾌﾟﾛｰﾁ速度2（パラメータ設定値）[ref/sec] */
} MT_SEQ_PRM;


typedef	union ACC_MATB_PRM_ {
	ULONG	lTb;					/* 移動平均回数(Total) */
	USHORT	wTb[2];					/* high word:移動平均回数, low word:移動平均補間回数  */
} MATB_PRM;


typedef struct ACCFIL_PRM_ {
	LONG	expbias;				/* 指数加減速バイアス */
	LONG	exptb;					/* 指数加減速時定数 */
	MATB_PRM mafil;					/* 移動平均回数 */
} ACF_PRM;


typedef struct OPTION_MAP_ {
	LONG	accfilBit;				/* 加減速フィルタ選択ビット位置 */
	LONG	accfilBitMsk;			/* 加減速フィルタ選択ビットマスク */
	LONG	gselBit;				/* ゲイン選択ビット位置 */
	LONG	gselBitMsk;				/* ゲイン選択ビットマスク */
	LONG	vppiBit;				/* P_PI選択ビット位置 */
	LONG	vppiBitMsk;				/* P_PI選択ビットマスク */
	LONG	ppiclrBit;				/* 位置積分クリアビット位置 */
	LONG	ppiclrBitMsk;			/* 位置積分ビットマスク */
	LONG	pclBit;					/* P_CLビット位置 */
	LONG	pclBitMsk;				/* P_CLビットマスク */
	LONG	nclBit;					/* N_CLビット位置 */
	LONG	nclBitMsk;				/* N_CLビットマスク */
	LONG	outpBit;				/* OutSignalビット位置 */
	LONG	outpBitMsk;				/* OutSignalビットマスク */
	LONG	bankBit;				/* BankSelビット位置 */
	LONG	bankBitMsk;				/* BankSelビットマスク */
	LONG	ltBit;					/* LtDisableビット位置 */
	LONG	ltBitMsk;				/* LtDisableビットマスク */
} OPTION_MAP;


typedef struct INSIG_MAP_ {
	LONG	bitNo[8];
	LONG	bitsMsk;
} INSIG_MAP;


typedef struct INSIG_M3_MAP_ {
	LONG	bitNo[15];				/* IO1-6入力ビット位置 */
	LONG	bitsMsk;				/* 拡張予定 (14bit必要) */
}INSIG_M3_MAP;

#if ( CCSW_SFTID == SFTID_EXIST )
typedef struct OUTSIG_M3_MAP_ {
	UCHAR	bitNo[16];			/* Out1-16出力力ビット位置 */
	ULONG	bitsMsk;			/* 下位18bitを使用する */
}OUTSIG_M3_MAP;
#endif

typedef struct SVCMD_IO_OUT_MAP_ {/* <S1C0> */
	UCHAR	bitNo;					/* SVCMD_IO(出力)16-31ビット位置 */
	UCHAR	bitsMsk;				/* マスク */
}SVCMD_IO_OUT_MAP;

typedef struct LTSEQ_PRM_ {
	LONG	seqNum;					/* シーケンス信号数 */
	LONG	loopCnt;				/* 連続回数設定 */
	LONG	ltSgnl[8];				/* ラッチ信号 */
} LTSEQ_PRM;


typedef struct PROF_PRM_ {
	LONG	zretDistDir;			/* 原点復帰最終走行距離方向 */
	LONG	zretLtCancel;			/* ZRETコマンド中のコマンドの切り替わりによるラッチキャンセル動作の有効／無効 */
	BOOL	initPgPos64Enable;		/* 初期エンコーダ位置（64bit）の有効／無効 */

	LONG	OutSigEnable;			/* Option割付出力信号有効/無効 *//* <S14D> */
} PROF_PRM;


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
	MC_BASE_PRM BasePrm;			/* 基本パラメータ */
	MG_PRM		MgPrm;				/* MGモーションパラメータ */
	MG_PRM		CmdRate;			/* 加減速指令<Rev2.01> */
	PSN_PRM		PsnPrm;				/* PositionManagerパラメータ */
	MT_SEQ_PRM	MtSeqPrm;			/* Motion指令作成用パラメータ */
	ACF_PRM		AccFilPrm;			/* 位置指令フィルタパラメータ */
	OPTION_MAP	OptMap;				/* OPTION指令ビット位置設定パラメータ */
	INSIG_MAP	InSigMap;			/* Input Signal0-7割付 */
	INSIG_M3_MAP InSigMonM3;		/* Input Signal0-7割付&セーフティオプション SVCMD_IO monitor用 */
	SVCMD_IO_OUT_MAP SvCmdIoOutMap[VENDOR_IOBIT_NUM];	/* SVCMD_IO(出力)任意割り付け用 *//* <S1C0> */
	MTBANK		Bnk;				/* Motion PRM Bankデータ */
	LTSEQ_PRM	LtPrm;
	PROF_PRM	ProfPrm;			/* プロファイルに依存したパラメータ */
#if ( CCSW_SFTID == SFTID_EXIST )											/*<V889>Add:Start*/
	OUTSIG_M3_MAP OutSigMonM3;	/* Output Signal0-7割付&セーフティオプション SVCMD_IO monitor用 */
#endif																		/*<V889>Add:End*/
} MT_PRM;


typedef struct MTPRM_INI_ {
	LONG	EGearA;					/* 電子ギアA(分母) */
	LONG	EGearB;					/* 電子ギアB(分子) */
	LONG	PgPuls;					/* PGパルス数[puls/rev],[puls/pitch] */
	LONG	MaxTrq;					/* MAXトルク[%] */
	LONG	OverSpd;				/* OS速度[rpm],[mm/s] */
	LONG	MaxSpd;					/* MAX速度[rpm],[mm/s] */
	LONG	ScalePitch;				/* スケールピッチ[mm/pitch] */
	LONG	OverSpdExp;				/* OS速度指数 */
	LONG	MaxSpdExp;				/* MAX速度指数 */
	LONG	ScalePitchExp;			/* スケールピッチ指数 */
	LONG	trefMode;				/* トルク制御Option */
	BOOL	absPG;					/* アブソPG使用 */
	BOOL	FullcCtrl;				/* ﾌﾙｸﾛｰｽﾞﾄﾞ制御 */
	BOOL	LinearMotor;			/* 回転 or リニア */

	LONG	Cycle;					/* モーション制御周期[0.25us] */
	LONG	SpdDiv;					/* 分解能 */
	ULONG	SegNum;					/* 補間送りセグメント回数 */
	LONG	OptEnable;				/* Option割付有効 */
	LONG	OptMap[5];				/* Option割付定義 */
	LONG	OutSigEnable;			/* Option割付出力信号有効/無効 */
	LONG	dlyTime;				/* 指令出力からFB入力までの時間 */
	USHORT	SvCycle;				/* サーボ周期(モーション周期/サーボ周期) */
	
	LONG	RatTrqExp;				/* モータトルク指数 */
	LONG	RatSpd;					/* 定格速度[rpm, mm/s] */
	LONG	RatTrq;					/* 定格トルク[Nm/N] */
} MTPRM_INI;



/****************************************************************************
 API's
****************************************************************************/
LONG MPrmCalcInit(MT_PRM *prm, MTPRM_INI *iniPrm, PRMDATA *pprm_ptr );
VOID MPrmCalcSvCmdIo(MT_PRM *mtPrm, PRMDATA *Prm );/* <S1C0> */
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

void MPrmSetInSigMonitor(MT_PRM *prm, USHORT data, UCHAR IoNum);
#if ( CCSW_SFTID == SFTID_EXIST )
void MPrmSetOutSigMonitor(MT_PRM* prm, USHORT data, UCHAR IoNum);
#endif




#endif /* MOTOIN_PRM_H_ */
#define MOTOIN_PRM_H_



