/****************************************************************************/
/*																			*/
/*																			*/
/*		ACC FILTER ヘッダファイル											*/
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
#ifndef		ACC_FIL_H_
#define		ACC_FIL_H_

#include "ComCfg.h"
//#include "MtPRM.h"

/* defines */
#ifdef ACC_FIL_DISABLE
 #define ACF_BUF_SZ		1
#else
 #define REAL_BUF_SZ	(512000*4)/(SCAN_N_DEFCYCLE*CFG_TIO_BASE_TIME)
 #if (REAL_BUF_SZ > ACF_MAX_SZ)
  #define ACF_BUF_SZ	ACF_MAX_SZ
 #else
  #define ACF_BUF_SZ	REAL_BUF_SZ
 #endif
#endif

#define ACF_SW_NOFIL		0
#define ACF_SW_EXPFIL		1
#define ACF_SW_MAFIL		2


/* typedefs: struct */
typedef struct ACF_MAFIL_ {
	LONG		maSum;		/* 移動平均フィルタサム */
	LONG		maRem;		/* 移動平均フィルタ余り */
	LONG*		maBuf;		/* 移動平均フィルタバッファ */
	USHORT		maTb;		/* 移動平均フィルタ時定数 */
	USHORT		maBufPtr;	/* 移動平均フィルタバッファポインタ */
	USHORT		maBufCnt;	/* 移動平均フィルタ出力時間 */
} ACF_MAFIL;

typedef struct ACF_IMAFIL_ {
	LONG		inPosSum;	/* 入力移動量積算 */
	LONG		dPos;		/* 移動量増分値 */
	LONG		lstPos;		/* 前回指令入力移動量 */
	LONG		dPosSum;	/* 補間距離積算 */
	LONG		iRem;		/* 移動平均フィルタ余り */
	UCHAR		iTbSft;		/* 移動平均フィルタ時定数 */
	UCHAR		iTb;		/* 移動平均フィルタ時定数 */
	UCHAR		iCnt;		/* 補間回数カウンタ*/
} ACF_IMAFIL;


typedef struct ACFHNDL_ {
	UCHAR		filSw;		/* 現在フィルタタイプ */
	BOOL		filComp;	/* フィルタ払い出し完了 */
	USHORT		expRem;		/* 指数関数フィルタバッファ余り */
	LONG		expBuf;		/* 指数関数フィルタバッファ */
	LONG		expBias;	/* 指数関数フィルタバイアス速度 */
	LONG		expTb;		/* 指数関数フィルタ時定数 */
	LONG		maBuf[ACF_BUF_SZ];/* 移動平均フィルタバッファ */
	ULONG		maFilTb;	/* 移動平均フィルタ時定数 */
	ACF_MAFIL	maFil;		/* 移動平均フィルタ */
	ACF_IMAFIL	maIFil;		/* 移動平均補間フィルタ */
	ACF_PRM*	Prm;		/* パラメータ */
	MC_BASE_PRM* bPrm;		/* 基本パラメータ */
} ACFHNDL;

/* forward declarations */
/* Public functions */
#ifdef ACC_FIL_DISABLE
#define AcfInit(hdl, bprm, prm)			(;)
#define AcfGenerate(hdl, spd, sel, den)	(;)
#define AcfClear(hdl)					(;)
#else
extern VOID AcfInit(ACFHNDL* hdl, MC_BASE_PRM* bPrm, ACF_PRM* prm);
extern VOID AcfGenerate(ACFHNDL* hdl, LONG* spd, UCHAR sel, BOOL* den);
extern VOID AcfClear(ACFHNDL* hdl);
#endif

/* 操作関数マクロ定義 */
#define AcfAttachPRM(hdl, prm)			{(hdl)->Prm = prm;}

#endif /* ACC_FIL_H_ */
