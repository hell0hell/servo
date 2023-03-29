/****************************************************************************
Description	: MECHATROLINK Position Filter
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef ACC_FIL_H_

#include "MtPRM.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define REAL_BUF_SZ	(512000 * 4) / (SCAN_N_DEFCYCLE * CFG_TIO_BASE_TIME)
#if (REAL_BUF_SZ > ACF_MAX_SZ)
	#define ACF_BUF_SZ	ACF_MAX_SZ
#else
	#define ACF_BUF_SZ	REAL_BUF_SZ
#endif

#define ACF_SW_NOFIL		0
#define ACF_SW_EXPFIL		1
#define ACF_SW_MAFIL		2



/****************************************************************************************************/
/*		STRUCT DEFINITION																			*/
/****************************************************************************************************/
typedef struct ACF_MAFIL_ {
	LONG		maSum;				/* 移動平均フィルタサム */
	LONG		maRem;				/* 移動平均フィルタ余り */
	LONG		*maBuf;				/* 移動平均フィルタバッファ */
	USHORT		maTb;				/* 移動平均フィルタ時定数 */
	USHORT		maBufPtr;			/* 移動平均フィルタバッファポインタ */
	USHORT		maBufCnt;			/* 移動平均フィルタ出力時間 */
	USHORT		dummy;				/* 4byte Alignment */
} ACF_MAFIL;


typedef struct ACF_IMAFIL_ {
	LONG		inPosSum;			/* 入力移動量積算 */
	LONG		dPos;				/* 移動量増分値 */
	LONG		lstPos;				/* 前回指令入力移動量 */
	LONG		dPosSum;			/* 補間距離積算 */
	LONG		iRem;				/* 移動平均フィルタ余り */
	UCHAR		iTbSft;				/* 移動平均フィルタ時定数 */
	UCHAR		iTb;				/* 移動平均フィルタ時定数 */
	UCHAR		iCnt;				/* 補間回数カウンタ */
	UCHAR		dummy;				/* 4byte Alignment */
} ACF_IMAFIL;


typedef struct ACF_HNDL_ {
	UCHAR		filSw;				/* 現在フィルタタイプ */
	UCHAR		dummy1[3];			/* 4byte Alignment */
	BOOL		filComp;			/* フィルタ払い出し完了 */
	USHORT		expRem;				/* 指数関数フィルタバッファ余り */
	USHORT		dummy2;				/* 4byte Alignment */
	LONG		expBuf;				/* 指数関数フィルタバッファ */
	LONG		expBias;			/* 指数関数フィルタバイアス速度 */
	LONG		expTb;				/* 指数関数フィルタ時定数 */
	LONG		maBuf[ACF_BUF_SZ];	/* 移動平均フィルタバッファ */
	ULONG		maFilTb;			/* 移動平均フィルタ時定数 */
	ACF_MAFIL	maFil;				/* 移動平均フィルタ */
	ACF_IMAFIL	maIFil;				/* 移動平均補間フィルタ */
	ACF_PRM		*Prm;				/* パラメータ */
	MC_BASE_PRM *bPrm;				/* 基本パラメータ */
} ACF_HNDL;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* ACF_HNDLの初期化処理 */
VOID AcfInit( ACF_HNDL *hdl, MC_BASE_PRM *bPrm, ACF_PRM *prm );
/* フィルタ生成処理 */
VOID AcfGenerate( ACF_HNDL *hdl, LONG *spd, LONG sel, BOOL *den );
/* フィルタクリア処理 */
VOID AcfClear( ACF_HNDL *hdl );



#define ACC_FIL_H_
#endif /* ACC_FIL_H_ */



