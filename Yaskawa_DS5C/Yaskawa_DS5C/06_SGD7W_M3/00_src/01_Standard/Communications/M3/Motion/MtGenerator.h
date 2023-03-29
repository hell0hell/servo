/****************************************************************************
Description	: MECHATROLINK Motion Generator
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef MOTOIN_GEN_H_

#include "MtPRM.h"

/****************************************************************************
 Defines
****************************************************************************/
#define MG_HOLD_CTRL1	0	/* 減速停止1(通常パラメータ使用) */
#define MG_HOLD_CTRL2	1	/* 減速停止2(停止用パラメータ使用) */

#define VELREM_BIT_MSK	((1<<MG_VREMBIT_MAX)-1)
#define VELREM_CARRYUP	(1<<MG_VREMBIT_MAX)

/* motoin type */
enum MG_MTYPE {
	MNONE = 0,			/* 0:NOPモーション */
	MINTERPOLATE,		/* 1:INTERPOLATE */
	MPOSING,			/* 2:POSING */ 
	MHOLD,				/* 3:HOLD */
	MFOLLOW,			/* 4:FOLLOW UP */
	MVELCTRL,			/* 5:VELCTRL */
	MVELCTRL_WITHACCR, 	/* 6:VELCTRL with Accr */
	MTRQCTRL,			/* 7:TRQCTRL */
	MSPOSING,			/* 8:SPOSING */
	MSHOLD,				/* 9:SHOLD */	
};


/* SPOSING動作モード */
#define SPOSING_INI_MODE		0	/* SPOSING初期化モード 			*/
#define SPOSING_ACC_MODE		1	/* SPOSING加速、定速モード 		*/
#define SPOSING_DEC_MODE1		2	/* SPOSING減速モード			*/
#define SPOSING_DEC_MODE2		3	/* SPOSING減速モード			*/

/* S字加減速比率 */
#define S_RATIO_MAX	50
#define S_RATIO_MIN  0

/****************************************************************************
 Structure of the Motion Generator
****************************************************************************/
typedef struct MG_POSWK_ {
	LONG	err_dist;			/* 残距離（指令位置から目標位置までの距離） ****.[ref] */
	ULONG	err_dist0;			/* 残距離（小数部）（指令位置から目標位置までの距離） .**[ref] */
	ULONG	sd_dist0;			/* 減速距離（小数部）（現在指令速度から停止するまでの減速距離） .**[ref] */
	LONG	sd_dist;			/* 減速距離（整数部）（現在指令速度から停止するまでの減速距離） ****[ref] */
	LONG	sd_rem;				/* 減速時端数（減速距離と残距離の差で発生する払い出し端数） ***.*[ref/cyc] */
	LONG	spdrem1;			/* １段目減速時の速度端数払い出し ***.*[ref/cyc] */
	LONG	spdrem2;			/* ２段目減速時の速度端数払い出し ***.*[ref/cyc] */
	LONG	spd;				/* 現在指令速度 ***.*[ref/cyc] */
	LONG	tspd;				/* 目標速度 ***.*[ref/cyc] */
	LONG	lspd;				/* 前回指令速度 ***.*[ref/cyc] */
	LONG	bspd;				/* 指令出力速度(dpos) ***.*[ref/cyc] */
} MG_POSWK;


//SPOSING用ワーク
typedef struct MG_S_POSWK_ {
	LONG	ErrorDistance;		/* 残距離（指令位置から目標位置までの距離） ****.[ref] */
	ULONG	ErrorDistance0;		/* 残距離（小数部）（指令位置から目標位置までの距離） .**[ref] */
	LONG	StopDistance;		/* 減速距離（整数部）（現在指令速度から停止するまでの減速距離） ****[ref] */
	ULONG	StopDistance0;		/* 減速距離（小数部）（現在指令速度から停止するまでの減速距離） .**[ref] */
//	LONG	StopDistanceRem;	/* 減速時端数（減速距離と残距離の差で発生する払い出し端数） ***.*[ref/cyc] */
	LONG	CurrentSpd;			/* 現在指令速度 ***.*[ref/cyc] */
	LONG	TargetSpd;			/* 目標速度 ***.*[ref/cyc] */
	LONG	LastSpd;			/* 前回指令速度 ***.*[ref/cyc] */
	LONG	Segment;			/* 指令出力速度(dpos) ***.*[ref/cyc] */
} MG_S_POSWK;


typedef struct MG_INOUT_ {
	LONG	Mtype;				/* Motion Type */
	LONG	Mtype_cancel;		/* Motion Type(コマンドキャンセル前) */	
	LONG	CtrlModeLst;		/* 前回制御モード */
	LONG	Ctrl;				/* Motion制御Option */
	LONG	DposIn;				/* 指令移動量[ref] */
	LONG	Tspd;				/* POSING用目標速度[ref/s] */
	LONG	Vref;				/* 速度指令[30bit/MAX] */
	LONG	Tref;				/* トルク指令[30bit/MAX] */
	LONG	FbSpeed;			/* FB速度[24bit/OS](制御切替時の指令用) */

	LONG	DposOut;			/* 出力移動量[ref](Vel->Pos) */
	LONG	VrefOut;			/* 出力速度指令[24bit/OS] */
	LONG	TrefOut;			/* 出力トルク指令[24bit/MAX] */
} MG_INOUT;


/* SPOSING用変数 */
typedef struct MG_SPOSING_ {
	LONG	State;				/* SPOSING状態変数						*/
	LONG	LastDposIn;			/* SPOSING前回残距離[ref]				*/
	LONG	LastTargetSpd;		/* SPOSING前回目標速度[ref/cyc]			*/
	LONG	LastTargetPosition;	/* SPOSING前回目標位置[ref/cyc]			*/

	LONG	CurrentSratio;		/* S字加減速時間比率[%]				*/
	LONG	InitAcc;			/* SPOSING初期化加速度 ***.*[ref/cyc^2]	*/
	LONG	InitDec;			/* SPOSING初期化減速度 ***.*[ref/cyc^2]	*/
	LONG	dJerkAcc;			/* Jerk(加速) ***.*[ref/cyc^3]			*/
	LONG	dJerkDec;			/* Jerk(減速) ***.*[ref/cyc^3]			*/
	LONG	JerkAccRate;		/* SPOSING加速度 ***.*[ref/cyc^2]		*/
	LONG	JerkDecRate;		/* SPOSING減速度 ***.*[ref/cyc^2]		*/
	LONG	CurrentAccRate;		/* 加速度指令 ***.*[ref/cyc^2]			*/
	LONG	CurrentDecRate;		/* 減速度指令 ***.*[ref/cyc^2]			*/
	LONG	AltSpd_a1;			/* 加速切り替え速度1 ***.*[ref/cyc]		*/
	LONG	AltSpd_a2;			/* 加速切り替え速度2 ***.*[ref/cyc]		*/
	LONG	AltSpd_d1;			/* 減速切り替え速度1 ***.*[ref/cyc]		*/
	LONG	AltSpd_d2;			/* 減速切り替え速度2 ***.*[ref/cyc]		*/
	LONG	InitSpdAcc;			/* 加速開始速度 ***.*[ref/cyc]			*/
	LONG	InitSpdDec;			/* 減速開始速度 ***.*[ref/cyc]			*/
	LONG	EndSpdDec;			/* 減速終了速度 ***.*[ref/cyc]			*/
	LONG	TargetSpd;			/* 目標速度 ***.*[ref/cyc]				*/

	LONG	SpdRemAcc;			/* 加速時速度端数 ***.*[ref/cyc]		*/
	LONG	SpdRemDec;			/* 減速時速度端数 ***.*[ref/cyc]	    */
	LONG	dSpdRemAcc;			/* 速度端数分割速度(加速) ***.*[ref/cyc]*/
	LONG	dSpdRemDec;			/* 速度端数分割速度(減速) ***.*[ref/cyc]*/

	LONG	StopDistRem;		/* 残距離端数 ***.*[ref]				*/
	LONG	dStopDistRem;		/* 残距離端数分割量 ***.*[ref/cyc]		*/
	LONG	jStopDistRem;		/* 残距離端数増加量	***.*[ref/cyc]		*/
	LONG	TmpDecVal;			/* 減速端数分割処理のための一時変数		*/
	LONG	DecCnt;				/* 減速端数分割処理のための減速回数		*/
	LONG	rStopDistRem;		/* 減速端数分割したさらに余り			*/
	LONG	rdStopDistRem;		/* さらに余りの分割量					*/

	LONG	LastSegment;		/* 前回セグメント(モニタ用)[ref/cyc^2]	*/
	LONG	AccMonitor;			/* 実加速度(モニタ用)[ref/cyc^2]	*/
} MG_SPOSING;


typedef struct MG_HNDL_ {
	BOOL			cMinusDir;		/* 現在払い出し方向 */
	BOOL			tMinusDir;		/* 現在の指令位置から目標位置の方向 */
	ULONG			dpos0;			/* DPOSの少数部 for POSINGの指令速度（少数部）の積算 */
	LONG			dposRem;		/* 伝送周期移動量(dpos)余り         */
	LONG			Cspd;			/* 現在指令速度 */
	LONG			VrefSum[2];		/* 速度指令積算値[24bit/OS](速度制御) */
	MC_BASE_PRM		*basePrm;		/* MG基本パラメータ */
	MG_PRM			*prm;			/* Motionパラメータ */
	MG_SPOSING		sposing;		/* SPOSING用パラメータ 			*/
} MG_HNDL;

/****************************************************************************
 API's
****************************************************************************/
/* MG_HNDLの初期化処理 */
VOID MtgInit( MG_HNDL *hdl, MC_BASE_PRM *bPrm, MG_PRM *prm );
/* モーション生成メイン処理 */
BOOL MtgGenerate(MG_HNDL* hdl, MG_INOUT* inOut);
/* 減速距離取得 */
LONG MtgGetDctDistance( MG_HNDL *hdl, LONG spd, LONG ctrl );
/* 減速距離取得(S字) */
LONG MtgGetDctSpDistance( MG_HNDL *hdl, LONG spd, LONG ctrl );
/* トルク/速度制御時 現在指令速度設定 */
LONG MtgSetCspd( MG_HNDL *hdl, LONG spd );
/* SPOSINGモーション処理 */
BOOL MtgSposing( MG_HNDL *hdl, MG_INOUT *inOut );
/* SHOLDモーション処理 */
BOOL MtgShold( MG_HNDL *hdl, MG_INOUT *inOut );
/* S字減速距離計算 */
BOOL mtgSpCalDecPrm(MG_SPOSING* sposing, MC_BASE_PRM* bPrm, BOOL ChkReverse,
							LONG CurrentSpd, LONG *StopDistance, ULONG *StopDistance0 );
/* SPOSING 加減速、S字パラメータセット */
void mtgSpSetPrm(MG_SPOSING* sposing, MG_PRM* prm, LONG TargetSpd);


#define MOTOIN_GEN_H_
#endif /* MOTOIN_GEN_H_ */


