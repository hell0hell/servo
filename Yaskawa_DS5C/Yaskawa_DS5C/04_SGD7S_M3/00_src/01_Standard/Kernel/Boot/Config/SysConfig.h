/****************************************************************************
  Description   : Parameter Initializing Module for INGRAM
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
  ------------------------------------------------------------------------
	I.M.   2011.02.01     re-created

****************************************************************************/
#ifndef _SYS_PRM_CONFIG_H_
#define _SYS_PRM_CONFIG_H_


#include "MercuryGlobals.h"
#include "ModelControl.h"


/****************************************************************************/
/*		PROTOTYPE															*/
/****************************************************************************/
void SysResetAxisParameters( AXIS_HANDLE *AxisRsc );
void SysInitModuleConfigInfo(AXIS_HANDLE *AxisRsc,
							EEP_NOCHKSUM_CMN_T *EepIdInfo, BOOL AutoConfig);
// Iprm
void IprmcalMainAxis( AXIS_HANDLE *Axis );
void IprmcalMainCommon( AXIS_HANDLE *Axis );
void LpxPcalKioManager( AXIS_HANDLE *Axis );
void PcalCopyMFCCfgParam(MFC_CFG_PRM *MfcCfgPrm, const PRMDATA *Prm);



#endif /* _SYS_PRM_CONFIG_H_ */

/*** end of file ***/
