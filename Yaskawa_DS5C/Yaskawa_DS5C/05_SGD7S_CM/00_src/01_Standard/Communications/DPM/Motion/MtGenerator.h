/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Generator ヘッダファイル										*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		MOTOIN_GEN_H_

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
	MNONE = 0,		/* 0:NOPモーション */
	MINTERPOLATE,	/* 1:INTERPOLATE */
	MPOSING,		/* 2:POSING */ 
	MHOLD,			/* 3:HOLD */
	MFOLLOW,		/* 4:FOLLOW UP */
	MVELCTRL,		/* 5:VELCTRL */
	MTRQCTRL,		/* 6:TRQCTRL */
};


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

typedef struct MG_INOUT_ {
	LONG	Mtype;			/* Motion Type */
	LONG	Ctrl;			/* Motion制御Option */
	LONG	DposIn;			/* 指令移動量[ref] */
	LONG	Tspd;			/* POSING用目標速度[ref/s] */
	LONG	Vref;			/* 速度指令[30bit/MAX] */
	LONG	Tref;			/* トルク指令[30bit/MAX] */

	LONG	DposOut;		/* 出力移動量[ref](Vel->Pos) */
	LONG	VrefOut;		/* 出力速度指令[24bit/OS] */
	LONG	TrefOut;		/* 出力トルク指令[24bit/MAX] */
} MG_INOUT;


typedef struct MG_HNDL_ {
	BOOL			cMinusDir;	/* 現在払い出し方向 */
	BOOL			tMinusDir;	/* 現在の指令位置から目標位置の方向 */
	ULONG			dpos0;		/* DPOSの少数部 for POSINGの指令速度（少数部）の積算 */
	LONG			dposRem;	/* 伝送周期移動量(dpos)余り         */
	LONG			Cspd;		/* 現在指令速度 */
	MC_BASE_PRM*	basePrm;	/* MG基本パラメータ */
	MG_PRM*			prm;		/* Motionパラメータ */
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
/* トルク/速度制御時 現在指令速度設定 */
LONG MtgSetCspd( MG_HNDL *hdl, LONG spd );


#define		MOTOIN_GEN_H_
#endif /* MOTOIN_GEN_H_ */
