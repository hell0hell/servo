/****************************************************************************
Description	: MECHATROLINK Task Interface Procedure
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef _ML_TASK_MNGR_H
#define _ML_TASK_MNGR_H

#include "Basedef.h"

/****************************************************************************
 API's
****************************************************************************/
#if 0
VOID OpiInitialize( VOID *axRsc );

VOID OpiCalcInitParam( VOID *axRsc );
VOID OpiInitDevice( VOID *axRsc, VOID *DeviceAddress );
VOID OpiStartDevice( VOID *axRsc, VOID *DeviceAddress );

VOID OpiRoundProcedure(VOID *axRsc);
//VOID OpiIntSync( VOID *axRsc );
BOOL OpiInputProcess( VOID *axRsc, BOOL IntStatus );
VOID OpiRunTimeScanProcess( void *AxisRsc, BOOL irqSts );
VOID OpiOutputProcess( VOID *axRsc, BOOL IntStatus );
VOID OpiSoftReset( VOID *axRsc );
#endif

#endif /* _ML_TASK_MNGR_H */


