/****************************************************************************
  Description   : JOG Operation(Fn100, Fn101, Fn102)	<S050>
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : Mercury

  Create Date   : 2013-11-19

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "AlarmManager.h"


/****************************************************************************
 Fn100, Fn101, Fn102: Entry function of TEST MODE 1,2,3
****************************************************************************/
PRM_RSLT FnEnterTetMod( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn100: Execute function of Force ACON Mode 
****************************************************************************/
PRM_RSLT FnExecuteTetMod1(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	errRes = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case 0:
		switch( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			Axis->PowerManager->MainPowerChecker.conf.TestMode1 = TRUE;
			ALMSetMask( Axis->AlmManager, ALM_WIR, TRUE );
			FunExe->State = 1;
			break;
		default:
			break;
		}
		break;

	case 1:	/* 終了処理		*/
		FunExe->HoldFnMode = FALSE;				/* Fnモード自動終了なし */
		break;

	default:
		break;
	}
	return errRes;
}


/****************************************************************************
 Fn101: Execute function of Regeneration Invalidity Mode
****************************************************************************/
PRM_RSLT FnExecuteTetMod2(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	errRes = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case 0:
		switch( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			Axis->PowerManager->MainPowerChecker.conf.TestMode2 = TRUE;
			FunExe->State = 1;
			break;
		default:
			break;
		}
		break;

	case 1:	/* 終了処理		*/
		FunExe->HoldFnMode = FALSE;				/* Fnモード自動終了なし */
		break;

	default:
		break;
	}
	return errRes;
}


/****************************************************************************
 Fn102: Execute function of Overload Mask Mode
****************************************************************************/
PRM_RSLT FnExecuteTetMod3(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	errRes = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case 0:
		switch( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			Axis->PowerManager->MainPowerChecker.conf.TestMode3 = TRUE;
			ALMSetMask( Axis->AlmManager, ALM_OLF1, TRUE );
			ALMSetMask( Axis->AlmManager, ALM_OLF1, TRUE );
			FunExe->State = 1;
			break;
		default:
			break;
		}
		break;

	case 1:	/* 終了処理		*/
		FunExe->HoldFnMode = FALSE;				/* Fnモード自動終了なし */
		break;

	default:
		break;
	}
	return errRes;
}



/******************************* End of File *******************************/
