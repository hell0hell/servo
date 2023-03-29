#ifndef __FN_SET_TUNE_LESS_LVL_H /* �ēǂݍ��ݖh�~�p */
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
//#define	DAT_GAINLEVEL_MAX			4		/* �������X�Q�C�����x���ő�l						*//* <S1C7> */
#define		DAT_GAINLEVEL_MAX			7		/* �������X�Q�C�����x���ő�l						*//* <S1C7> */
#define		DAT_GAINLEVEL_MIN			0		/* �������X�Q�C�����x���ŏ��l						*/
#define		DAT_JRATLEVEL_MAX			2		/* �������X�C�i�[�V�����x���ő�l					*/
#define		DAT_JRATLEVEL_MIN			0		/* �������X�C�i�[�V�����x���ŏ��l					*/
/*--------------------------------------------------------------------------------------------------*/
#define		DAT_SEQ_INIT				0		/* ��������											*/
#define		DAT_SEQ_TUNING				1		/* �Q�C�����x����������								*/
#define		DAT_SEQ_END					10		/* �I������											*/


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
