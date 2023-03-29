#ifndef __BACKLASH_COMP_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BacklashComp.h : バックラッシ補正機能	モジュール定義										*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2011.04.05	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "Bprm.h"
#include "MLib.h"

/****************************************************************************************************/
/*																									*/
/*		Structure Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		バックラッシ補正用変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct	{								/* バックラッシ補正用定数＆変数定義					*/
	struct {									/* バックラッシ補正用定数データ定義					*/
		BOOL	BacklashDir;					/* バックラッシ補正方向選択設定						*/
		LONG	CompPulse;							/* バックラッシ補正量			[Pulse]			*/
		LONG	Kfil;								/* バックラッシフィルタゲイン					*/
		LONG	KfilTmp;							/* バックラッシフィルタゲイン演算用				*/
		LONG	MaxCompPulse;						/* バックラッシ補正量制限値		[Pulse]			*/
		ULONG	MaxCompRef;							/* バックラッシ補正量制限値		[指令単位]		*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* バックラッシ補正用変数データ定義					*/
		LONG	BlshCmpAmountPls;					/* バックラッシ補正量差分値						*/
		LONG	Filo;								/* バックラッシ補正フィルタ出力					*/
		LONG	LastFilo;							/* 前回バックラッシ補正フィルタ出力				*/
		LONG	Cmppls;								/* バックラッシ補正量			[Pulse]			*/
		LONG	Cmpgrrem;							/* 電子ギア変換後バックラッシ補正量余り			*/
		LONG	Cmppos;								/* バックラッシ補正位置			[Pulse]			*/
		LONG	remx;								/* バックラッシ補正フィルタ計算余り				*/
	} var;
} BLSHCMP;


/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define		BLSHCOMP_UPLIM		500000				/* バックラッシ補正量最大値						*/
#define		FOR_BACKLASH		0x00				/* 正転側にバックラッシ補正						*/
#define		REV_BACKLASH		0x01				/* 逆転側にバックラッシ補正						*/


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void	BlshCompensateBacklash( BLSHCMP *BlshCmp, EGEAR *Egear, LONG *dPosRef, LONG *dPcmd, BOOL *RefZ, BOOL BlshActive );
void	BlshCalcMaxBlshCompPulse( BLSHCMP *BlshCmp, EGEAR *Egear, BPRMDAT *Bprm );
void	BlshCalcCompensateAmount( BLSHCMP *BlshCmp, EGEAR *Egear, LONG blcm2l );
void	BlshCalcCompensateTime( BLSHCMP *BlshCmp, LONG SvCycleUs, USHORT blfiltime );
void	BlshInvaildCompensate( BLSHCMP *BlshCmp );
void	BlshInitializeCompensate( BLSHCMP *BlshCmp );



/****************************************************************************************************/
#define __BACKLASH_COMP_H
#endif /* __BACKLASH_COMP_H */
