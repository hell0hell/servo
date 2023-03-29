/****************************************************************************
Description	: Function list of Network Identification Registers
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#ifndef __NETWORK_ID_REG_FUNC_H 
#define __NETWORK_ID_REG_FUNC_H

#include "PrmAccessIf.h"

#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)					/* MECATROLINK- M2	*/
extern const PRMTBL M2IdRegTable[];
extern const ULONG M2IdRegTableEntNum;

#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)					/*  MECATROLINK-3	*/
extern const PRMTBL M3IdRegTable[];
extern const ULONG M3IdRegTableEntNum;

#endif

PRM_RSLT NetworkIdReadSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT NetworkIdReadTByteSetting(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT NetworkIdReadProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdReadMotModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdReadEncVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S224> */
PRM_RSLT NetworkIdReadExEncModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdReadExEncVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdSOptionModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdReadSOptionVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdReadFbOptionModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */
PRM_RSLT NetworkIdReadFbOptionVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S**1> */

#endif //__NETWORK_ID_REG_FUNC_H 

