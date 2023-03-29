/****************************************************************************
Description	: MECHATROLINK Position Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef POSITION_H_

#include "MtPRM.h"


/****************************************************************************
 Defines
****************************************************************************/
#define SOFT_LIMIT_P		0x1
#define SOFT_LIMIT_N		0x2

#define SOFT_LIMIT_PBIT		0x0
#define SOFT_LIMIT_NBIT		0x1

//#define LT_NUM			2		/* ラッチ本数 */
#define LT_NUM				3		/* ラッチ本数 */	/* <S056>:ラッチ本数を変更 */


/****************************************************************************
 Structure of the Position Handle
****************************************************************************/
typedef struct PSN_HNDL_ {
	LONG		Tpos;				/* 目標位置 */
	LONG		Ipos;				/* 指令位置（フィルタ前） */
	LONG		Pos;				/* 指令位置（フィルタ後） */
	LONG		Mpos;				/* FB位置入力タイミングでの指令位置 */
	LONG		Apos;				/* FB位置 */
	LONG		Perr;				/* 偏差 */
	LONG		Lpos[LT_NUM];		/* ラッチ位置 */
	LONG		LstLpos[LT_NUM];	/* 前回ラッチ位置 */
	LONG		Fbspd;				/* FB速度 */
	LONG		Pgcnt;				/* 目標位置 */
	LONG		Dpos;				/* 今回出力移動距離 */
	LONG		lstDpos;			/* 前回出力移動距離 */
	LONG		PosOfs;				/* 共通モニタ POS_OFST */
	LONG		InitPgPos64[2];		/* 初期エンコーダ位置（64bit）*/
	BOOL		Den;				/* 指令出力完了 */
	BOOL		Pset;				/* 位置決め完了信号 */
	BOOL		Near;				/* Near信号 */
	BOOL		RefEnable;			/* リファレンス点有効 */
	BOOL		PosEnable;			/* 座標位置有効 */
	LONG		SoftLimit;			/* ソフトリミット状態 */
	PSN_PRM		*Prm;				/* Potionパラメータ */
} PSN_HNDL;



/****************************************************************************
 API's
****************************************************************************/
/* PSN_HNDLの初期化処理 */
VOID PsnInit( PSN_HNDL *hdl, PSN_PRM *prm );
/* FBデータ作成処理 */
VOID PsnUpdateFBData( PSN_HNDL *hdl, LONG pgcnt, LONG PosErr );
/* PSETおよびNEAR信号作成処理	*//* <S0AE> */
VOID PsnSetPsetState( PSN_HNDL *hdl );
/* TPOS設定処理(絶対値) */
LONG PsnSetTPOS( PSN_HNDL *hdl, LONG tpos );
/* TPOS設定処理(相対距離) */
LONG PsnSetTPOSDist( PSN_HNDL *hdl, LONG dpos );
/* 速度・トルク制御時の位置合わせ処理 */
VOID PsnResetCmdPosAll( PSN_HNDL *hdl, LONG err );
/* ソフトリミット状態チェック処理(APOS) */
LONG PsnCheckSoftLimitFB( PSN_HNDL *hdl, BOOL *p_ot, BOOL *n_ot );
/* 座標系設定 */
VOID PsnSetCordinate( PSN_HNDL *hdl, LONG mode, LONG pos );
/* 座標系リセット */
VOID PsnResetApos( PSN_HNDL *hdl, BOOL abs );
/* LPOS取得 */
BOOL PsnMakeLpos( PSN_HNDL *hdl, LONG ltcnt, LONG i );



#define POSITION_H_
#endif /* POSITION_H_ */



