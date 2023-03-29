/****************************************************************************
  Description   : Callback functions for Monitor Register
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-10-20

****************************************************************************/
#include "KnlApi.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "HwDeviceIf.h"
#include "JL086ServoIpHw.h"

/****************************************************************************************************/
/*		Un123 : “à•”“ü—ÍM†ƒ‚ƒjƒ^																	*/
/****************************************************************************************************/
PRM_RSLT uncal_InternalInputSignal3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
LONG rv;
PLSIFV *pPls = &(LPlsIfV[ Axis->AxisNo ]); /*KPX_GET_PLSIF( Axis->AxisNo );*/
#define UMONSETBIT( sig, bit )  ((sig)? (1<<bit) : 0x0000)

	/**
	 * @@ bit0: PULS signal
	 *    bit1: SIGN signal
	 *    bit2: CLR signal
	 *    bit3: PSEL signal
	 */
	 rv = UMONSETBIT(  KPX_CHECK_PCMD_PULS(), 0 );
	 rv |= UMONSETBIT( KPX_CHECK_PCMD_SIGN(), 1 );
	 rv |= UMONSETBIT( KPX_CHECK_PCMD_CLR() , 2 );
//	 rv |= UMONSETBIT( BinV.f.Sv.Psel, 3 );

	*pValue = rv;
	return PRM_RSLT_SUCCESS;
}

/******************************* End of File *******************************/
