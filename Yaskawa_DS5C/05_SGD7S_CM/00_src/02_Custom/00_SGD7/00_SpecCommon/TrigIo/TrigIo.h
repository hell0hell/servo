/****************************************************************************
Description	: Trigger I/O functions
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2012 All Rights Reserved

Project       : SGDZ-BS63/80

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
H.N.   2012.03.05     created

****************************************************************************/
#ifndef	_TRIGIO_H
#define	_TRIGIO_H

#include "IngramGlobals.h"
#include "PrmAccessIf.h"
/* function prototype */

/* Trigger I/O パラメータ・レジスタ計算関数 */
PRM_RSLT pncal_trigIoTbl1_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl1_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl2_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl2_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl3_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl3_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl4_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl4_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl5_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl5_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl6_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl6_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl7_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl7_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl8_axisNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT pncal_trigIoTbl8_onTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);


PRM_RSLT fnCalcTriggerIOStsMon(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTriggerIOControl(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

/* Trigger I/O アプリケーション処理 */
void TrigIOSetDelayTime(AXIS_HANDLE	*Axis);
void TrigIOInitialize(AXIS_HANDLE *Axis);

#endif	//	_TRIGIO_H
