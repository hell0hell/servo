/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsTrcDef.h : トレース定義ヘッダファイル													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. トレース定義構造体																		*/
/*																									*/
/*		2. トレース定義テーブルExtern宣言															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2010.02.12 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"




/****************************************************************************************************/
/*																									*/
/*		トレース定義構造体																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		数値トレース定義構造体																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct  {
//		UCHAR	SelNo;								/* 選択No										*/	/* <S17A> */
		USHORT	SelNo;								/* 選択No										*/	/* <S17A> */
//		UCHAR	VxType;								/* 変数タイプ									*/
		UCHAR	VxSftL;								/* トレース変数左シフト							*/
		UCHAR	VxSftR;								/* トレース変数右シフト							*/
		BOOL	AvrgSel;							/* トレース平均化選択:TRUE⇒平均化する			*/	
		LONG	(*GetVarData)( void* Axis );		/* トレース変数アドレス取得関数					*/
		LONG	(*GetVarGain)( void* Axis );		/* トレース変数のゲイン計算関数					*/
} TVXDEF;
/*--------------------------------------------------------------------------------------------------*/
#define XTRCDEF_TxSHORT			0x02				/* Type：SHORT									*/
/*--------------------------------------------------------------------------------------------------*/
/*		ビットトレース定義構造体																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct  {
		UCHAR	SelNo;								/* 選択No										*/
//		UCHAR	BitNo;								/* ビットNo.									*/
		BOOL	(*GetBitData)( void* Axis );		/* トレース変数アドレス取得関数					*/
} TBXDEF;
/*--------------------------------------------------------------------------------------------------*/
/*		TVXDEF/TBXDEF Version																		*/
/*--------------------------------------------------------------------------------------------------*/
#define TVXDEF_VERSION			0x0001				/* TVXDEF Version								*/
#define TBXDEF_VERSION			0x0001				/* TBXDEF Version								*/
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		トレース定義テーブルExtern宣言																*/
/*																									*/
/****************************************************************************************************/
extern  const TVXDEF TrcVarTbl[];					/* 数値トレース定義テーブル						*/
extern  const TBXDEF TrcBitTbl[];					/* ビットトレース定義テーブル					*/
extern  const USHORT TrcVarTblEntNum;				/* 数値トレース定義テーブル登録個数				*/
extern  const USHORT TrcBitTblEntNum;				/* ビットトレース定義テーブル登録個数			*/
//extern  const TBXDEF TrcBitTblUser[];				/* ビットトレース定義テーブル					*/
/*--------------------------------------------------------------------------------------------------*/
extern  const USHORT TrcVarTblUserEntNum;			/* 数値トレース定義テーブル登録個数				*/
extern  const USHORT TrcBitTblUserEntNum;			/* ビットトレース定義テーブル登録個数			*/
/*--------------------------------------------------------------------------------------------------*/
/*		Define Entry Number																			*/
/*--------------------------------------------------------------------------------------------------*/
#if		OPENSW_TRCDEF == 0
#define TRCVARTBL_ENTNUM	TrcVarTblEntNum
#define TRCBITTBL_ENTNUM	TrcBitTblEntNum
#else
#define TRCVARTBL_ENTNUM	(TrcVarTblEntNum + TrcVarTblUserEntNum)
#define TRCBITTBL_ENTNUM	(TrcBitTblEntNum + TrcBitTblUserEntNum)
#endif
/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/
