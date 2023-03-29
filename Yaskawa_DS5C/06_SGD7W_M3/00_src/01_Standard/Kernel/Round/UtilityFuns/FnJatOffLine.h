#ifndef _FN_JAT_OFFLINE_H
#define _FN_JAT_OFFLINE_H
/****************************************************************************
Description	: Jrate OffLine Function
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
T.kira  2011.03.01    created

*****************************************************************************/
#include "Basedef.h"
#include "JatOffLine.h"
#include "PrmAccessIf.h"
#include "UnitPrmStruct.h"
#include "GainChange.h"
/****************************************************************************************************/
/*																									*/
/*	定数定義										            									*/
/*																									*/
/****************************************************************************************************/
/*ScanBの制御周期は動的に変わるので、引数でもらう													*/
//#define	JMDLCALCYCNS	KPI_SBCYCLENS				/* モデル計算サイクル[ns]	ScanB)			*/
#define	JRATCALCYCUS	(KPI_SCCYCLEMS*1000)		/* イナーシャ計算サイクル[us]	(ScanC)			*/
#define	JRATVERRFIL		1000						/* 同定エラー用速度偏差フィルタ周波数[0.1Hz]	*/
#define	JATFIL			100000						/* イナーシャ更新フィルタ時定数：100000[us]		*/
#define	JATVFFGN		100							/* イナーシャ同定中速度FFゲイン：100[%]			*/

/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
SHORT CpiInitJatOffLine( JATHNDL *JatHdl, UNI_PRM_STR *UniPrms, REG_MANAGER_HANDLE *RegManager, 
			GAIN_CHNG_HNDL GainChange, SHORT InitSw, LONG distance, UCHAR CtrlMode, LONG SvCycleNs);
void CpiEndJatOffLine( JATHNDL *JatHdl, REG_ACC_T *AccessPrm );
CHAR CpiGetJatExeErrStatus( JATHNDL	*JatHdl );	
CHAR CpiGetJatErrStatus( JATHNDL *JatHdl );		
CHAR CpiGetJatCompStatus( JATHNDL *JatHdl );		
void CpiSetEstimateJrat( JATHNDL *JatHdl, REG_ACC_T *AccessPrm );

#endif //_FN_JAT_OFFLINE_H 
/***************************************** end of file **********************************************/
