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
/*	�萔��`										            									*/
/*																									*/
/****************************************************************************************************/
/*ScanB�̐�������͓��I�ɕς��̂ŁA�����ł��炤													*/
//#define	JMDLCALCYCNS	KPI_SBCYCLENS				/* ���f���v�Z�T�C�N��[ns]	ScanB)			*/
#define	JRATCALCYCUS	(KPI_SCCYCLEMS*1000)		/* �C�i�[�V���v�Z�T�C�N��[us]	(ScanC)			*/
#define	JRATVERRFIL		1000						/* ����G���[�p���x�΍��t�B���^���g��[0.1Hz]	*/
#define	JATFIL			100000						/* �C�i�[�V���X�V�t�B���^���萔�F100000[us]		*/
#define	JATVFFGN		100							/* �C�i�[�V�����蒆���xFF�Q�C���F100[%]			*/

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
