#ifndef __FN_SET_TUNE_LESS_LVL_H /* 再読み込み防止用 */
/****************************************************************************
  Description   : Set Tuning Less Level (Fn200)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.31   created

****************************************************************************/
#include "Basedef.h"
#include "BaseControls.h"
#include "PrmAccessIf.h"



/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
//#define	DAT_GAINLEVEL_MAX			4		/* 調整レスゲインレベル最大値						*//* <S1C7> */
#define		DAT_GAINLEVEL_MAX			7		/* 調整レスゲインレベル最大値						*//* <S1C7> */
#define		DAT_GAINLEVEL_MIN			0		/* 調整レスゲインレベル最小値						*/
#define		DAT_JRATLEVEL_MAX			2		/* 調整レスイナーシャレベル最大値					*/
#define		DAT_JRATLEVEL_MIN			0		/* 調整レスイナーシャレベル最小値					*/
/*--------------------------------------------------------------------------------------------------*/
#define		DAT_SEQ_INIT				0		/* 初期処理											*/
#define		DAT_SEQ_TUNING				1		/* ゲインレベル調整処理								*/
#define		DAT_SEQ_END					10		/* 終了処理											*/


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
void		RpiRegSetTuneLessLevel( ULONG Gain, TUNELESS_CTRL *TuneLessCtrl );
PRM_RSLT	RpiFunSetDATGainLevel( TUNELESS_CTRL *TuneLessCtrl, USHORT Level );
PRM_RSLT	RpiFunSetDATJratLevel( TUNELESS_CTRL *TuneLessCtrl, USHORT Level );



/****************************************************************************************************/
#define __FN_SET_TUNE_LESS_LVL_H
#endif /* __FN_SET_TUNE_LESS_LVL_H */
