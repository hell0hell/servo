#ifndef SCANB_MAIN_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanBMain.h : ScanBÉÅÉCÉìèàóùíËã`															*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	èâî≈	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MercuryGlobals.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
//void	KpxInputScanB( AXIS_HANDLE* );
void	BpiScanBMain( AXIS_HANDLE*, LONG ax_no );/* <S1C0>ÅFé≤í«â¡ */
//void	KpxOutputScanB( AXIS_HANDLE* );
//void	BpxOutputScanC( AXIS_HANDLE* );

void	InitScanBData( AXIS_HANDLE* );
//void	PrstScanBData( AXIS_HANDLE* );

//void PdetServoOnMake( SERVO_CONTROL_IF*, FUN_CMN_CONTROL*, MENCV*, BOOL );	/* <S21C> */
ALMID_T PdetServoOnMake( SERVO_CONTROL_IF*, FUN_CMN_CONTROL*, MENCV*, AXIS_HANDLE * );	/* <S226> */
BOOL    AlmClearReq( ALMMAN_HANDLE *, ALMID_T );										/* <S226> */

#define SCANB_MAIN_H
#endif /* SCANB_MAIN_H */
/***************************************** end of file **********************************************/
