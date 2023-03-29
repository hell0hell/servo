/****************************************************************************
  Description   : Motor Parameter Downloading(200BH, 200CH)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	I.M.   2011.03.12     created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "KLib.h"
#include "SerialEncIf.h"




/****************************************************************************
 200BH: Entry function of Motor Parameter Uploading
****************************************************************************/
PRM_RSLT FnEnterMotorParameterRead(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MOTLESSTST			*MotorLessTest;	/* モータレステストモードIF定義		*//* <S1B7> */

	FnCmnCtrl = Axis->FnCmnCtrl;
	MotorLessTest = Axis->MotorLessTest;									/* <S1B7> */

	/* サーボオン中は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* モータレステストモード判定 *//* <S1B7> */
	if( (MotorLessTest->MotorLessTestMode == TRUE)
		&& (MotorLessTest->MotorLessTestDatas.SemiEncConnect == FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	FunExe->HoldFnMode = TRUE;	/* Do not escape Fn mode */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 200BH: Execute function of Motor Parameter Uploading
****************************************************************************/
PRM_RSLT FnExecuteMotorParameterRead(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	MENCV			*MencV;
	SENC_ERROR		SenErr;

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = Axis->MencV;

	switch(FunExe->State)
	{
	case 0:	/* 実行要求待ち(完了) */
	case 2:	/* 前回実行失敗 */
		switch(FunExe->CmdCode)
		{
		case FCMD_EXEC:	/* 実行	*/
			if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
			{
				errRes = PRM_RSLT_CONDITION_ERR;
			}
			else if((MencV->MotPrmAddress + MencV->MotPrmSize) > 160)
			{
				errRes = PRM_RSLT_RWACC_ERR;
			}
			else
			{
				FunExe->State = 1;
				errRes = PRM_RSLT_CONTINUE;
			}
			break;

		case FCMD_STOP:
		case FCMD_NONE:
			errRes = PRM_RSLT_SUCCESS;
			break;
		default:
			errRes = PRM_RSLT_CONDITION_ERR;
			break;
		}
		break;

	case 1:	/* Upload処理 */
		SenErr = SencReadMotorParameter(MencV,
		                                MencV->MotPrmAddress,
		                                MencV->MotPrmSize,
		                                &Axis->TraceBuffer[MencV->MotPrmAddress/2]);
		if(SenErr == SENCERR_BUSY)
		{
			errRes = PRM_RSLT_CONTINUE;
		}
		else if(SenErr == SENCERR_SUCCESS)
		{/* 正常終了 */
			FunExe->State = 0;
			errRes = PRM_RSLT_SUCCESS;
		}
		else
		{
			errRes = PRM_RSLT_RWACC_ERR;
			FunExe->State = 2;
		}
		break;

	default:
		errRes = PRM_RSLT_SUCCESS;
		break;
	}

	return errRes;
}

/****************************************************************************
 200BH: Leave function of Motor Parameter Uploading
****************************************************************************/
void FnLeaveMotorParameterRead(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	MENCV			*MencV;

	MencV = Axis->MencV;
	FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */

	if(FunExe->State == 1)
	{
		while(SENCERR_BUSY == SencReadMotorParameter(MencV,
		                    		                 MencV->MotPrmAddress,
		                    		                 MencV->MotPrmSize,
		                    		                 &Axis->TraceBuffer[MencV->MotPrmAddress/2]));
	}
}




/****************************************************************************
 200CH: Entry function of Motor Parameter Downloading
****************************************************************************/
PRM_RSLT FnEnterMotorParameterWrite(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MENCV				*MencV;
	MOTLESSTST			*MotorLessTest;	/* モータレステストモードIF定義		*//* <S1B7> */

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = Axis->MencV;
	MotorLessTest = Axis->MotorLessTest;									/* <S1B7> */

	/* サーボオン中は入らずに終了	*/
	if((FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		|| (MencV->ScaleWriteEnable == FALSE))
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* モータレステストモード判定 *//* <S1B7> */
	if( (MotorLessTest->MotorLessTestMode == TRUE)
		&& (MotorLessTest->MotorLessTestDatas.SemiEncConnect == FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	FunExe->HoldFnMode = TRUE;	/* Do not escape Fn mode */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 200CH: Execute function of Motor Parameter Downloading
****************************************************************************/
PRM_RSLT FnExecuteMotorParameterWrite(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	MENCV			*MencV;
	SENC_ERROR		SenErr;

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = Axis->MencV;

	switch(FunExe->State)
	{
	case 0:	/* 実行要求待ち(完了) */
	case 2:	/* 前回実行失敗 */
		switch(FunExe->CmdCode)
		{
		case FCMD_EXEC:	/* 実行	*/
			if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
			{
				errRes = PRM_RSLT_CONDITION_ERR;
			}
			else if((MencV->MotPrmAddress + MencV->MotPrmSize) > 160)
			{
				errRes = PRM_RSLT_RWACC_ERR;
			}
			else
			{
				FunExe->State = 1;
				errRes = PRM_RSLT_CONTINUE;
			}
			break;

		case FCMD_STOP:
		case FCMD_NONE:
			errRes = PRM_RSLT_SUCCESS;
			break;
		default:
			errRes = PRM_RSLT_CONDITION_ERR;
			break;
		}
		break;

	case 1:	/* Upload処理 */
		SenErr = SencWriteMotorParameter(MencV,
		                                MencV->MotPrmAddress,
		                                MencV->MotPrmSize,
		                                &Axis->TraceBuffer[(MencV->MotPrmAddress/2)]);
		if(SenErr == SENCERR_BUSY)
		{
			errRes = PRM_RSLT_CONTINUE;
		}
		else if(SenErr == SENCERR_SUCCESS)
		{/* 正常終了 */
			FunExe->State = 0;
			errRes = PRM_RSLT_SUCCESS;
		}
		else
		{
			errRes = PRM_RSLT_RWACC_ERR;
			FunExe->State = 2;
		}
		break;

	default:
		errRes = PRM_RSLT_SUCCESS;
		break;
	}

	return errRes;
}

/****************************************************************************
 200CH: Leave function of Motor Parameter Downloading
****************************************************************************/
void FnLeaveMotorParameterWrite(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	MENCV			*MencV;

	MencV = Axis->MencV;
	FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */

	if(FunExe->State == 1)
	{
		while(SENCERR_BUSY == SencWriteMotorParameter(MencV,
		                    		                 MencV->MotPrmAddress,
		                    		                 MencV->MotPrmSize,
		                    		                 &Axis->TraceBuffer[(MencV->MotPrmAddress/2)]));
	}
}


/******************************* End of File *******************************/
