/****************************************************************************
  Description   : Absolute Scale Utility functions(Fn020, Fn080, Fn081)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
  ------------------------------------------------------------------------
	I.M.   2011.02.01     created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "SerialEncIf.h"
#include "KLib.h"
#include "PdetABSScale.h"
#include "PnPrmListTbl.h"


/****************************************************************************
  Description: Enable Motor phase Write function
-----------------------------------------------------------------------------
  Parameters:
	PDET_CONTROL *PdtCtrl	: Pointer to the PDET_CONTROL handle
	MENCV *MencV			: Pointer to the Encoder handle
	BOOL enable				: TRUE=Enable, FALSE=Disable
  Return:
	TRUE  - Request is accepted.
	FALSE - Request can not accepted.
*****************************************************************************/
BOOL PdetEnablePhaseOffsetWrite(PDET_ABSCONTROL *PdtCtrl, MENCV *MencV, BOOL enable)
{
	BOOL wr_req;

	wr_req = FALSE;

	if( (MencV->AbsoEncType != MENC_INCRE_TYPE)
		&& (MencV->withPoleSensor == FALSE)
		&& ((MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS)
			|| (MencV->SerialConvType == SCONVTYPE_ENDAT_ABS)) )
	{
		if( (enable != FALSE)
			&& ((PdtCtrl->SeqStep == PDETSEQ_IDOL)
				|| (PdtCtrl->SeqStep == PDETSEQ_ERROR)
				|| (PdtCtrl->SeqStep == PDETSEQ_PHASE_WR_READY)) )
		{
			MencV->PhaseReady = FALSE;
			PdtCtrl->SeqStep = PDETSEQ_PHASE_WR_READY;
			wr_req = TRUE;
		}
	}
	else
	{
		PdtCtrl->SeqStep = PDETSEQ_IDOL;
	}

//	PdtCtrl->PhaseWrStartReq = wr_req;
	return wr_req;
}



#pragma arm section code = "CALL_SCANB" /* <S1D2> */
/****************************************************************************
  Description: Request to start motor phase write sequence
-----------------------------------------------------------------------------
  Parameters:
	PDET_CONTROL *PdtCtrl	: Pointer to the PDET_CONTROL handle
	MENCV *MencV			: Pointer to the Encoder handle
	LONG PhaseOffset		: Phase offset
  Return:
	TRUE  - write sequence was already completed.
	FALSE - write sequence is executing now.
*****************************************************************************/
BOOL PdetStartPhaseOffsetWrite(PDET_ABSCONTROL *PdtCtrl, MENCV *MencV, LONG PhaseOffset)
{
	BOOL comp;
	switch(PdtCtrl->SeqStep)
	{
	case PDETSEQ_PHASE_WR_READY:
		/* Stop position read of ScanA */
		MencV->EncDisable = TRUE;
		/* Start Phase Offset Write sequence */
		PdtCtrl->SeqStep = PDETSEQ_WAIT_BASEBLOCK;
		MencV->MpfPhaseOffset = PhaseOffset;
		comp = FALSE;
		break;

	case PDETSEQ_IDOL:
		comp = TRUE;
		PdtCtrl->SeqStep = PDETSEQ_FINISHED;
		break;
	case PDETSEQ_FINISHED:
	case PDETSEQ_ERROR:
		comp = TRUE;
		break;

	default:
		comp = FALSE;
		break;
	}
	return comp;
}
#pragma arm section code


/****************************************************************************
  Description: Check to start motor phase write sequence
-----------------------------------------------------------------------------
  Parameters:
	PDET_CONTROL *PdtCtrl	: Pointer to the PDET_CONTROL handle
	MENCV *MencV			: Pointer to the Encoder handle
  Return:
	TRUE  - ready.
	FALSE - not ready.
*****************************************************************************/
BOOL PdetCheckPdetCondition(PDET_ABSCONTROL *PdtCtrl, MENCV *MencV)
{
	if( (MencV->AbsoEncType != MENC_INCRE_TYPE)
		&& (MencV->withPoleSensor == FALSE)
		&& (MencV->ScaleWriteEnable != FALSE) )
	{
		return PdetGetPhaseOffsetWriteReq(PdtCtrl);
	}

	return TRUE;
}


/****************************************************************************
  Description: Get the request state of motor phase write sequence
-----------------------------------------------------------------------------
  Parameters:
	PDET_CONTROL *PdtCtrl	: Pointer to the PDET_CONTROL handle
  Return:
	TRUE  - Request is accepted.
	FALSE - Request is not accepted.
*****************************************************************************/
BOOL PdetGetPhaseOffsetWriteReq(PDET_ABSCONTROL *PdtCtrl)
{
	BOOL req;
	switch(PdtCtrl->SeqStep)
	{
	case PDETSEQ_IDOL:
	case PDETSEQ_FINISHED:
	case PDETSEQ_ERROR:
		req = FALSE;
		break;
	default:
		req = TRUE;
		break;
	}
	return req;
}


/****************************************************************************
  Description: Motor Phase Offset Write sequence for Absolute Scale
-----------------------------------------------------------------------------
  Parameters:
	PDET_CONTROL *PdtCtrl	: Pointer to the PDET_CONTROL handle
	MENCV *MencV			: Pointer to the Encoder handle
	ASIC_HW_IF_STR	*AsicHwReg	: Pointer to JL-077 register
	PREGIF_CONFIG_T	hPnReg	: Pointer to Parameter access handle
  Return:
	TRUE - error state(A.C90)
	FALSE - no error state
*****************************************************************************/
BOOL PdetWriteMotorPhaseSequence(PDET_ABSCONTROL	*PdtCtrl,
								 MENCV				*MencV,
								 ASIC_HW_IF_STR		*AsicHwReg,
								 PREGIF_CONFIG_T	hPnReg)
{
	BOOL		alm;
	LONG		syssw3;
	SENC_ERROR	err;
	REG_ACC_T	AccessPrm;

	alm = FALSE;

	switch(PdtCtrl->SeqStep)
	{
	case PDETSEQ_PHASE_WR_READY:
		break;

	case PDETSEQ_WAIT_BASEBLOCK:
		if(SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) != 0)
		{
			PdtCtrl->SeqStep = PDETSEQ_WRITE_PHASE_OFS;
		}
		break;

	case PDETSEQ_WRITE_PHASE_OFS:
		err = SencWriteMotorPhaseOffset(MencV,
										(USHORT)MencV->MpfPhaseOffset);
		if(err != SENCERR_BUSY)
		{
			/* Re-start position read of ScanA */
			MencV->EncDisable = FALSE;

			if(err != SENCERR_SUCCESS)
			{/* error */
				PdtCtrl->SeqStep = PDETSEQ_ERROR;
				alm = TRUE;
			}
			else
			{/* success */

				AccessPrm.hRegCfg = hPnReg;
				AccessPrm.ArrayIdx = 0;
				AccessPrm.IfHandle = NULL;
				AccessPrm.EepWrite = TRUE;

				/* Write Phase offset to PnE8D */
				AccessPrm.cPrmDef = &pndef_phsofst;
				PrmWriteValue(&AccessPrm, &MencV->MpfPhaseOffset, 2);

				/* Write Phase offset to PnF1D */
				AccessPrm.cPrmDef = &pndef_phscmpM2;
				PrmWriteValue(&AccessPrm, &MencV->MpfPhaseOffset, 2);

				/* Write the complete flag to PnE0D.5 */
				AccessPrm.cPrmDef = &pndef_syssw3;
				PrmReadValue(&AccessPrm, &syssw3, 2);
				syssw3 = syssw3 | 0x0020;
				PrmWriteValue(&AccessPrm, &syssw3, 2);

				PdtCtrl->SeqStep = PDETSEQ_FINISHED;
			}
		}
		break;

	default:
		break;
	}

	return alm;
}




/****************************************************************************
 Fn0080: Entry function of Magnetic Pole Adjust
****************************************************************************/
PRM_RSLT FnEnterMagneticPoleDetection(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MENCV				*MencV;
	SEQ_CTRL_OUT		*SeqCtrlOut;

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = Axis->MencV;
	SeqCtrlOut = Axis->SeqCtrlOut;

	/* <S215>：条件見直し */
	if( (SeqCtrlOut->MainPowerOn == FALSE)
		|| (SeqCtrlOut->HwbbSts != FALSE)
		|| ((Axis->AlmManager->AlmReady == FALSE)
			&& (FALSE != ALMGetOtherAlarmChk(Axis->AlmManager, ALM_PHASEANGLE)))
//		|| (SeqCtrlOut->SvRdyPhaseIgnore == FALSE)
		|| (MencV->withPoleSensor != FALSE)
		|| (MencV->EncDisable != FALSE)
		|| (FnCmnCtrl->State.FnSvCtrlDisable != FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* <S20F>:Fnサーボ運転権獲得の前に処理を実施する様に変更	 */
	/* モータレステストモードの場合は入らずに終了 *//* <S1B7>	 */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->forcedAlmReady = TRUE;/* <S215> */

	FunExe->HoldFnMode = TRUE;	/* Do not escape Fn mode */

	PdetEnablePhaseOffsetWrite(Axis->PdetAbsSeq, MencV, TRUE);

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
  Fn0080: Execute function of Magnetic Pole Adjust
****************************************************************************/
PRM_RSLT FnExecuteMagneticPoleDetection(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	MENCV			*MencV;
	PDET_ABSCONTROL	*PdetCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	PRMDATA			*Prm;

	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	MencV = Axis->MencV;
	PdetCtrl = Axis->PdetAbsSeq;
	Prm = Axis->UniPrms->Prm;
	errRes = PRM_RSLT_SUCCESS;

#if 0
	/* 実行条件チェック */
	if( (SeqCtrlOut->MainPowerOn == FALSE)
		|| (SeqCtrlOut->HwbbSts != FALSE)
		|| (Axis->AlmManager->AlmReady == FALSE)
		|| (PdetCtrl->SeqStep == PDETSEQ_ERROR) )
	{
		PdetCtrl->SeqStep = PDETSEQ_ERROR;
		FnCmnCtrl->PdetReq = FALSE;
		FunExe->State = 0;
		return PRM_RSLT_CONDITION_ERR;
	}
#endif

	switch(FunExe->State)
	{
	case 0:	/* 実行要求待ち	*/
		if(FunExe->CmdCode == FCMD_EXEC)
		{
			/* 実行条件チェック *//* <S215>：条件見直し */
			if( (SeqCtrlOut->MainPowerOn == FALSE)
				|| (SeqCtrlOut->HwbbSts != FALSE)
				|| (SeqCtrlOut->SvRdyPhaseIgnore == FALSE)
				|| (PdetCtrl->SeqStep == PDETSEQ_ERROR) )
			{
				PdetCtrl->SeqStep = PDETSEQ_ERROR;
				FnCmnCtrl->PdetReq = FALSE;
				FunExe->State = 0;
				return PRM_RSLT_CONDITION_ERR;
			}
			
			PdetCtrl->SeqStep = PDETSEQ_IDOL;
			if(FALSE != PdetEnablePhaseOffsetWrite( PdetCtrl, MencV, TRUE ))
			{	/* Clear Pdet complete flag */
				Prm->syssw3 = Prm->syssw3 & (~0x0020);
				PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_syssw3, 0);
			}

			FnCmnCtrl->PdetReq = TRUE;
			FunExe->State = 1;
			errRes = PRM_RSLT_CONTINUE;
		}
		break;

	case 1:	/* 実行完了待ち */
		if(PdetCtrl->SeqStep == PDETSEQ_FINISHED)
		{/* Finished */
			FnCmnCtrl->PdetReq = FALSE;
			FunExe->State = 3;
		}
		else
		{
			if(FunExe->CmdCode == FCMD_STOP)
			{/* 中断 */
				FunExe->State =
					(PdetCtrl->SeqStep == PDETSEQ_WRITE_PHASE_OFS) ? 2 : 0;
			}
			errRes = PRM_RSLT_CONTINUE;
		}
		break;

	case 2:	/* 中断による実行完了待ち */
		if((PdetCtrl->SeqStep == PDETSEQ_FINISHED)
			|| (PdetCtrl->SeqStep == PDETSEQ_ERROR))
		{
			FnCmnCtrl->PdetReq = FALSE;
			FunExe->State = 0;
		}
		break;

	default:
		break;
	}

	return errRes;
}


/****************************************************************************
  Fn0080: Leave function of Magnetic Pole Adjust
****************************************************************************/
void FnLeaveMagneticPoleDetection(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;
	FnCmnCtrl->PdetReq = FALSE;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->FnSvControl = FALSE;
	FnCmnCtrl->forcedAlmReady = FALSE;/* <S215> */
}


/****************************************************************************
 Fn081: Entry function of Magnetic Pole Copy
****************************************************************************/
PRM_RSLT FnEnterMagneticPoleCopy(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MENCV				*MencV;

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = Axis->MencV;

			/* サーボオン中、Mitutoyoスケールでない場合は入らずに終了	*/
	if( (FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		|| (MencV->SerialConvType != SCONVTYPE_MITUTOYO_ABS)
		|| (MencV->EncDisable != FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->JogSpeed = 0;

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
  Fn081: Execute function of Magnetic Pole Copy
****************************************************************************/
PRM_RSLT FnExecuteMagneticPoleCopy(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	MENCV			*MencV;
	PRMDATA			*Prm;
	REG_ACC_T		AccessPrm;
	LONG			syssw3;

	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	MencV = Axis->MencV;
	Prm = Axis->UniPrms->Prm;

	MencV->EncDisable = TRUE;

	AccessPrm.hRegCfg  = Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.IfHandle = NULL;
	AccessPrm.EepWrite = TRUE;

	/* PnF1D --> PnE8D */
	/* 2012.08.18 Y.Oka Make Mercury Base Software */
	MencV->MpfPhaseOffset =	(USHORT)Prm->MencP.phscmpM2;
	Prm->phsofst          = (USHORT)MencV->MpfPhaseOffset;
	/* 2012.08.18 Y.Oka Make Mercury Base Software */

	/* Write Phase offset to PnE8D */
	AccessPrm.cPrmDef = &pndef_phsofst;
	PrmWriteValue(&AccessPrm, &MencV->MpfPhaseOffset, 2);

	/* Write the complete flag to PnE0D.5=1 */
	AccessPrm.cPrmDef = &pndef_syssw3;
	PrmReadValue(&AccessPrm, &syssw3, 2);
	syssw3 = syssw3 | 0x0020;
	PrmWriteValue(&AccessPrm, &syssw3, 2);

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
  Fn081: Leave function of Magnetic Pole Copy
****************************************************************************/
void FnLeaveMagneticPoleCopy(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;
	FnCmnCtrl->FnSvControl = FALSE;
}


/****************************************************************************
 Fn0020: Entry function of Scale Origin Setting
****************************************************************************/
PRM_RSLT FnEnterScaleZeroSetting(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MENCV				*MencV;
	MOTLESSTST			*MotorLessTest;	/* モータレステストモードIF定義		*//* <S1B7> */

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = ((Axis->UniPrms->Bprm)->FencUse != FALSE)
			? Axis->FencV: Axis->MencV;
	MotorLessTest = Axis->MotorLessTest;									/* <S1B7> */

	/* モータレステストモード判定 *//* <S1B7> */
	if( (MotorLessTest->MotorLessTestMode == TRUE)
		&& (MotorLessTest->MotorLessTestDatas.SemiEncConnect == FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

			/* サーボオン中, インクレ使用、Mitutoyoスケールでない場合は入らずに終了	*/
	if( (FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		|| (MencV->AbsoEncType == MENC_INCRE_TYPE)
		|| (MencV->SerialConvType != SCONVTYPE_MITUTOYO_ABS)
		|| (MencV->EncDisable != FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->JogSpeed = 0;

	FunExe->HoldFnMode = TRUE;	/* Do not escape Fn mode */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
  Fn0020: Execute function of Scale Origin Setting
****************************************************************************/
PRM_RSLT FnExecuteScaleZeroSetting(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	MENCV			*MencV;
	SENC_ERROR		SenErr;
	REG_ACC_T		AccessPrm;
	LONG			syssw3;

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = ((Axis->UniPrms->Bprm)->FencUse != FALSE)
			? Axis->FencV: Axis->MencV;
	errRes = PRM_RSLT_SUCCESS;

	switch(FunExe->State)
	{
	case 0:	/* 保護処理	*/
		if(FunExe->CmdCode == FCMD_UP)
		{
			FunExe->State = 1;
		}
		break;

	case 1:	/* 実行要求待ち	*/
		if(FunExe->CmdCode == FCMD_EXEC)
		{
			MencV->EncDisable = TRUE;

			AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
			AccessPrm.ArrayIdx = 0;
			AccessPrm.IfHandle = NULL;
			AccessPrm.EepWrite = TRUE;

			/* Write the complete flag to PnE0D.5=0 */
			AccessPrm.cPrmDef = &pndef_syssw3;
			PrmReadValue(&AccessPrm, &syssw3, 2);
			syssw3 = syssw3 & 0xFFCF;
			PrmWriteValue(&AccessPrm, &syssw3, 2);

			FunExe->State = 2;
			errRes = PRM_RSLT_CONTINUE;
		}
		break;

	case 2:	/* 実行完了待ち */
		SenErr = SencResetScaleZeroPosition(MencV);
		if(SenErr == SENCERR_BUSY)
		{/* still busy  */
			errRes = PRM_RSLT_CONTINUE;
			FnCmnCtrl->OperatorSts = 0;		/* <S085> */
		}
		else
		{
			if(SenErr != SENCERR_SUCCESS)
			{/* error */
				errRes = PRM_RSLT_CALC_ERR;
				FnCmnCtrl->OperatorSts = -1;		/* <S085> */
			}
			else
			{/* success */
				AccessPrm.hRegCfg = Axis->RegManager->hPnReg;
				AccessPrm.ArrayIdx = 0;
				AccessPrm.IfHandle = NULL;
				AccessPrm.EepWrite = TRUE;

				/* Write the complete flag to PnE0D.5=1 */
				AccessPrm.cPrmDef = &pndef_syssw3;
				PrmReadValue(&AccessPrm, &syssw3, 2);
				syssw3 = syssw3 | 0x0010;
				PrmWriteValue(&AccessPrm, &syssw3, 2);
				FnCmnCtrl->OperatorSts = 1;		/* <S085> */
			}

			FunExe->State = 0;
			FunExe->HoldFnMode = FALSE;	/* Escape Fn mode */
			FnCmnCtrl->FnSvControl = FALSE;
		}
		break;

	default:
		break;
	}

	return errRes;
}


/****************************************************************************
  Fn0020: Leave function of Scale Origin Setting
****************************************************************************/
void FnLeaveScaleZeroSetting(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	MENCV			*MencV;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	MencV = ((Axis->UniPrms->Bprm)->FencUse != FALSE)
			? Axis->FencV: Axis->MencV;
	FnCmnCtrl = Axis->FnCmnCtrl;
	FnCmnCtrl->FnSvControl = FALSE;

	if(FunExe->State == 2)
	{
		while(SENCERR_BUSY == SencResetScaleZeroPosition(MencV));
	}
}


/******************************* End of File *******************************/
