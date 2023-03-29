/****************************************************************************
  Description   : Fn005, Fn006, Fn014, Fn030, Parameter Recalculation.
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-11-18

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "SysConfig.h"
#include "MtApi.h"
#include "KLib.h"


/****************************************************************************
 Fn005: Entry function of User Parameter Initialization
****************************************************************************/
PRM_RSLT FnEnterPrmInit(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn005: Execute function of User Parameter Initialization
****************************************************************************/
PRM_RSLT FnExecutePrmInit(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	REGIF_CONFIG_T	*hPnReg;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	hPnReg = Axis->RegManager->hPnReg;
	FnCmnCtrl = Axis->FnCmnCtrl;

	// Write the default parameters to EEPROM
	if(FunExe->State == 0)
	{
		/* サーボオン中の場合は入らずに終了	*/
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}

		/* 実行要求がくるまで、待ち	*/
		if(FunExe->CmdCode == FCMD_EXEC)
		{
			PrmStoreSystemDefaultValues(hPnReg, TRUE, ACCLVL_USER2);
#if 0/* <S04E> */
			/* Restore IP address */
			EepdevWriteNoSumValues(hPnReg->hEepDev, 0, EEP_NOSUMCMN_ADDRESS(EEP_IPADDR_OFS),
			                       (USHORT*)&EepCmnDefaultValue.IPAddress, 2);
			/* Restore sub net mask */
			EepdevWriteNoSumValues(hPnReg->hEepDev, 0, EEP_NOSUMCMN_ADDRESS(EEP_NETMASK_OFS),
			                       (USHORT*)&EepCmnDefaultValue.NetMask, 2);
			/* Restore gateway address */
			EepdevWriteNoSumValues(hPnReg->hEepDev, 0, EEP_NOSUMCMN_ADDRESS(EEP_GATEWAY_OFS),
			                       (USHORT*)&EepCmnDefaultValue.GateWay, 2);
#endif
			FunExe->State = 1;
		}
		errRes = PRM_RSLT_CONTINUE;
	}
	else if(FunExe->State == 1)
	{
		PrmStoreSystemDefaultValues(hPnReg, FALSE, ACCLVL_USER2);
		FunExe->State = 2;
//		Axis->SeqCtrlData->RndReady = FALSE;	/* 初期化実行後、サーボオン不可 */
		Axis->BeSequence->SvonEnable = FALSE;	/* 初期化実行後、サーボオン不可 */
		errRes = PRM_RSLT_SUCCESS;
	}
	else
	{
		errRes = PRM_RSLT_SUCCESS;
	}

	return errRes;
}



/****************************************************************************
 Fn006: Alarm Traceback Data Clear
****************************************************************************/
PRM_RSLT FnExecuteTracebackClear(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	ALMSetAlarmHistResetRequest(Axis->AlmManager);

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn014 : Entry function of Option Card Detection Clear
****************************************************************************/
PRM_RSLT FnEnterModuleInfoReset(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;
#if 0
	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}
#endif
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn014 : Execute function of Option Card Detection Clear
****************************************************************************//* <S0BE>：全面変更 */
PRM_RSLT FnExecuteModuleInfoReset(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	REGIF_CONFIG_T	*hPnReg;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	hPnReg = Axis->RegManager->hPnReg;
	FnCmnCtrl = Axis->FnCmnCtrl;

	// Write the default parameters to EEPROM
	if(FunExe->State == 0)
	{
		/*------------------------------------------------------------------*/
		/*	Read EEPROM Option ID											*/
		/*------------------------------------------------------------------*/
		EepdevReadValues(PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_OPTID_OFS),
							(USHORT*)&Axis->OptClr->var.CmdOptId, EEP_OPTID_SIZE);

		
		FunExe->State = 1;
		errRes = PRM_RSLT_CONTINUE;
	}
	else if(FunExe->State == 1)
	{
		/* 実行要求がくるまで、待ち	*/
		if(FunExe->CmdCode == FCMD_EXEC)
		{
			/* 現状の設定値読込み	*/
			switch ( Axis->OptClr->OpeReg.OptCardNo )
			{
				case OPTCLR_CARD_COMMAND:
					Axis->OptClr->var.CmdOptId = CMDOP_ID_NONE;
					break;

				case OPTCLR_CARD_SAFE:
					Axis->OptClr->var.SftOptId = SFTOP_ID_NONE;
					break;

				case OPTCLR_CARD_FB:
					Axis->OptClr->var.FbkOptId = FBKOP_ID_NONE;
					break;

				default :
					break;
			}
		
			/*------------------------------------------------------------------*/
			/*	Write EEPROM Option ID											*/
			/*------------------------------------------------------------------*/
			EepdevWriteNoSumValues(PRMDEF_EEPROM_IF, EEP_NOSUMCMN_ADDRESS(EEP_OPTID_OFS),
								(USHORT*)&Axis->OptClr->var.CmdOptId, EEP_OPTID_SIZE);
			FunExe->State = 2;
			errRes = PRM_RSLT_SUCCESS;
		}
		else
		{
			errRes = PRM_RSLT_CONTINUE;
		}

	}
	else
	{
		errRes = PRM_RSLT_SUCCESS;
	}

	return errRes;
}


/****************************************************************************
 2001H : Alarm Reset
****************************************************************************/
PRM_RSLT FnExecuteAlarmResetting(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	ALMSetAlarmResetRequest(Axis->AlmManager);

	return PRM_RSLT_SUCCESS;
}


/*****************************************************************************
  Description: 2003H Command Pulse Counter Clear (only Register IF) End	<S054>
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		STRUCT DEFINITION(軸単位)
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunPlsCntClrEnd(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FunExe->HoldFnMode = FALSE;				/* Fnモード自動終了設定 */
}


/*****************************************************************************
  Description: 2003H Command Pulse Counter Clear (only Register IF) Begin
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		STRUCT DEFINITION(軸単位)
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT FunPlsCntClrBegin(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */
	return PRM_RSLT_SUCCESS;
}


/*****************************************************************************
  Description: 2003H Command Pulse Counter Clear (only Register IF) Execution	<S054>
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		STRUCT DEFINITION(軸単位)
  Return:
	rc;			戻り値
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT FunPlsCntClrExe(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	ANLG_UN_MON_DATA	*UnMonitor;
	PRM_RSLT rc;

	UnMonitor = Axis->UnMonitor;
	rc = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case 0:
		switch( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			UnMonitor->UnPcmdCntr = 0;		/* 指令パルスカウンタ（Un00C）クリア	*/
			FunExe->State = 1;				/* 自動終了	*/
			break;
		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;
	case 1:									/* 終了処理	*/
		FunPlsCntClrEnd( FunExe, Axis );
		break;
	default:
		break;
	}
	return( rc );
}


/*****************************************************************************
  Description: 2004H Feedback Pulse Counter Clear (only Register IF) End	<S054>
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		STRUCT DEFINITION(軸単位)
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunFbkPlsCntEnd(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FunExe->HoldFnMode = FALSE;				/* Fnモード自動終了設定 */
}


/*****************************************************************************
  Description: 2004H Feedback Pulse Counter Clear (only Register IF Begin
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		STRUCT DEFINITION(軸単位)
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT FunFbkPlsCntBegin(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */
	return PRM_RSLT_SUCCESS;
}


/*****************************************************************************
  Description: 2004H Feedback Pulse Counter Clear (only Register IF) Execution	<S054>
------------------------------------------------------------------------------
  Parameters:
	FunExe;		補助機能アプリ側で参照/操作できるメンバ
	Axis;		STRUCT DEFINITION(軸単位)
  Return:
	rc;			戻り値
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT FunFbkPlsCntExe(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	/* スタブとする */
//	LONG UnPfbkCntr2;		/* <S144> */
//	LONG RemCntr2;			/* <S144> */

	ANLG_UN_MON_DATA	*UnMonitor;
	PRM_RSLT rc;

	UnMonitor = Axis->UnMonitor;
	rc = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case 0:
		switch( FunExe->CmdCode )
		{
		case FCMD_EXEC:
			/* フィードバックパルスカウンタ（Un00D）クリア	*/
			UnMonitor->UnPfbkCntr = 0;
			/* フィードバックパルスカウンタ（Un013）クリア	*/
//			UnPfbkCntr2 = 0;
//			RemCntr2 = 0;
			UnMonitor->UnPfbkCntr2 = 0;		/* <S144> */
			UnMonitor->RemCntr2 = 0;		/* <S144> */
			FunExe->State = 1;				/* 自動終了	*/
			break;
		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;
	case 1:									/* 終了処理	*/
		FunFbkPlsCntEnd( FunExe, Axis );
		break;
	default:
		break;
	}
	return( rc );
}


/****************************************************************************
 2008H: Entry function of Parameter Recalculation
****************************************************************************/
PRM_RSLT FnEnterPrmRecalc(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 2008H: Execute function of Parameter Recalculation
****************************************************************************/
PRM_RSLT FnExecutePrmRecalc(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
#if ((CSW_NETIF_TYPE == NETIF_M3_TYPE)||(CSW_NETIF_TYPE == NETIF_M2_TYPE))/* <S158> */
	MLNET_HANDLE	*NetAxisIf;

	NetAxisIf = Axis->NetAxisIf;
#endif
	errRes = PRM_RSLT_CONTINUE;
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	switch(FunExe->State)
	{
	case 0:	/* 実行要求待ち */
		if(FunExe->CmdCode == FCMD_EXEC)
		{
			FunExe->State = 1;
		}
		break;

	case 1:	/* Recalculate parameters */
		/* Reset MLINK Motion */
#if ((CSW_NETIF_TYPE == NETIF_M3_TYPE)||(CSW_NETIF_TYPE == NETIF_M2_TYPE))/* <S158> */
		if(MCI_WAIT_COMP != MciResetMotion(&NetAxisIf->mt_hdl))
		{
			// Recalculate parameters
			SysResetAxisParameters(Axis);

			MciEnableState(&NetAxisIf->mt_hdl, TRUE);
			FunExe->State = 2;
		}
#else
			// Recalculate parameters
			SysResetAxisParameters(Axis);
			FunExe->State = 2;
#endif
		break;

	case 2:	/* Enable MLINK Motion */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
		if(MCI_SUCCESS == MciEnableState(&NetAxisIf->mt_hdl, TRUE))
#endif
		{
			FunExe->State = 0;
			errRes = PRM_RSLT_SUCCESS;
		}
		break;

	default:
		break;
	}

	return errRes;
}


/****************************************************************************
 0x2009: Entry function of Store Parameters
****************************************************************************/
PRM_RSLT FnEnterStoreParameters(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 0x2009: Execute function of Store Parameters
****************************************************************************/
PRM_RSLT FnExecuteStoreParameters(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	REGIF_CONFIG_T	*hPnReg;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	hPnReg = Axis->RegManager->hPnReg;
	FnCmnCtrl = Axis->FnCmnCtrl;
	errRes = PRM_RSLT_CONTINUE;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode == FCMD_EXEC)
	{
		errRes = PrmStoreAllValuesToEeprom(hPnReg);
	}

	return errRes;
}




/****************************************************************************
 Fn030 : Entry function of Software Reset Mode
****************************************************************************/
PRM_RSLT FnEnterSoftwareReset(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	LONG				no;

	/* サーボオン中の場合は入らずに終了	*/
	for(no = 0; no < NumberOfAxes; no++)
	{
		FnCmnCtrl = ((AXIS_HANDLE*)GetAxisHandle(no))->FnCmnCtrl;
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn030 : Execute function of Software Reset Mode
****************************************************************************/
PRM_RSLT FnExecuteSoftwareReset(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	ULONG			rdTime, timeStamp;
	LONG			no;
	BE_SEQ_HNDL		*BeSequence;			/* <S076> */

	BeSequence = Axis->BeSequence;			/* <S076> */

	/* サーボオン中の場合は入らずに終了	*/
	for(no = 0; no < NumberOfAxes; no++)
	{
		FnCmnCtrl = ((AXIS_HANDLE*)GetAxisHandle(no))->FnCmnCtrl;
		if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
	}

	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* Save Time stamp value to EEPROM */
	rdTime = timeStamp = KlibGetTimeStamp();
	EepdevReadValues( 0,
					  //EEP_NOSUM_ADDRESS(0, EEP_TIMESTAMP_OFS),					/* <S04E>:DELL */
					  EEP_NOSUMCMN_ADDRESS(EEP_TIMESTAMP_OFS),                      /* <S04E>:ADD */
					  (USHORT*)&rdTime, 2 );
	if(timeStamp != rdTime)
	{
		EepdevWriteNoSumValues( 0,
								//EEP_NOSUM_ADDRESS(0, EEP_TIMESTAMP_OFS),			/* <S04E>:DELL */
								EEP_NOSUMCMN_ADDRESS(EEP_TIMESTAMP_OFS),            /* <S04E>:ADD */
								(USHORT*)&timeStamp,
								2 );
	}
//	KlibWaitms(20);

#if 0 /* 2012.08.19 Y.Oka Make Mercury Base Software SyncNet用処理？ */
	/* Finalization for Option module */
	OpiSoftReset(Axis);
#endif /* 2012.08.19 Y.Oka Make Mercury Base Software */

	BeSequence->SoftResetReq = TRUE;		/* soft reset req on <S076> */

	/* Outputs a reset signal */
//	HALdrv_OutputSoftwareResetSignal(); /* 不使用 <Oka02> */


	return PRM_RSLT_SUCCESS;
}



/****************************************************************************
 2011H : Entry function of SDA Parameter Initialization
****************************************************************************/
PRM_RSLT FnEnterSDAPrmInit(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 2011H : Execute function of SDA/COA Parameter Initialization
****************************************************************************/
PRM_RSLT FnExecuteSDAPrmInit(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
//	REGIF_CONFIG_T	*hPnReg;
//	UINT			code;

//	hPnReg = Axis->RegManager->hPnReg;
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* 実行要求がくるまで、待ち	*/
	if((FunExe->CmdCode == FCMD_STOP)
		|| (FunExe->CmdCode == FCMD_NONE))
	{
		return PRM_RSLT_CONTINUE;
	}

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}
#if 0
	code = (UINT)FunExe->CmdCode;
	if(code < 0x100)
	{
		// Write the default parameters to SDA EEPROM
		EepCfg_InitAmpParameters(hPnReg->hEepDev,
								 hPnReg->EepromBlockAddress,
								 code);
	}
	else if(code > 0x200)
	{
		code = code & 0x0F;
		// Write the default parameters to MBB EEPROM
		EepCfg_InitMBBParameters(hPnReg->hEepDev, 0, code);
	}
	else
	{
		code = code & 0x0F;
		// Write the default parameters to COA EEPROM
		EepCfg_InitConvParameters(hPnReg->hEepDev, 0, code);
	}
#endif
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 200EH : Enter function of HW OutPut Signal By Force
****************************************************************************/
PRM_RSLT FnEnterForceSignalOut(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
//	HWSOUT				OutSig;		/* <S1FB> */
	ULONG				OutSig;		/* <S1FB> */

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
//	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;			/* <S1FB> */

	FunExe->HoldFnMode = TRUE;/* Fnモード自動終了なし */

//	OutSig = HALdrv_ReadOuputSignals();				/* <S1FB> */
//	FnCmnCtrl->ForceOutSignals = (USHORT)OutSig.dw;	/* <S1FB> */
	OutSig = HALdrv_ReadMonAllOutputSignals();		/* <S1FB> */
	FnCmnCtrl->ForceOutSignals = (USHORT)OutSig;	/* <S1FB> */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 200EH : Execute function of HW OutPut Signal By Force
****************************************************************************/
PRM_RSLT FnExecuteForceSignalOut(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SENCX 		*Enc;							/* <S0CB> */
	ASIC_HW_IF_STR *pAsicHwReg;					/* <S1FB> */


	FnCmnCtrl = Axis->FnCmnCtrl;
	Enc = &Axis->BaseLoops->MencV->EncIf;		/* <S0CB> */
	pAsicHwReg = Axis->SvAsicRegs->AsicHwReg;	/* <S1FB> */


	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode == FCMD_EXEC)
	{
		FnCmnCtrl->OutModeFlg = TRUE;	/* 自動出力モードフラグ = True */	/* <S0CB> */

#ifndef HW_FBA_IO_AXIS
		/* enable the force output signals */
		//HALdrv_WriteForceOutputSignals(FnCmnCtrl->ForceOutSignals, TRUE);									/* <S0CB> */
		//HALdrv_WriteForceOutputSignals(Axis, TRUE);														/* <S0CB> */
		FnCmnCtrl->ForceOutSignalsHW = HALdrv_WriteForceOutputSignals(FnCmnCtrl->ForceOutSignals, TRUE);	/* <S0CB> */
#else
		/* enable the force output signals */
		HALdrv_WriteForceOutputSignalsAx(FnCmnCtrl->ForceOutSignals, TRUE,
										 &(Axis->IoSignals->SerialOutputAx), Axis->SvAsicRegs->AsicHwReg);
#endif

#if CSW_BOARD_TYPE != SGD7W_TYPE	/* <S114> : SO7(分周Ｃ相設定)の強制出力はSGD7Sのみ有効です。 */
/* <S0CB> Start */
		/*--- 強制出力設定 bit7 : 分周Ｃ相 ---*/
//		if( FnCmnCtrl->ForceOutSignals & 0x0040 )	/* <S1AB> */
//		if( FnCmnCtrl->ForceOutSignals & 0x0080 )	/* <S1AB> */
		if( (FnCmnCtrl->ForceOutSignals & 0x0080) == 0x0080 )	/* <S204> */
		{
			/* パルス変換C相マスクON設定 */
			Enc->SencSetReg->AREG_WDT1SET |= BIT8;

			/* 分周C相 常時On */
			pAsicHwReg->AREG_DIVSET |= BIT15;	/* <S1FB> */

			/* 通常Ｃ相出力選択 */
			pAsicHwReg->AREG_DIVSET &= 0xfffff3ff;	/* <S204> */
		}
		else
		{
//			/* パルス変換C相マスクOFF設定 */
//			Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;	/* <S204> */

			/* 分周C相 常時Off */
			pAsicHwReg->AREG_DIVSET &= ~BIT15;		/* <S1FB> */

			/* パルス変換C相マスクON設定 */
			Enc->SencSetReg->AREG_WDT1SET |= BIT8;	/* <S204> */

			/* 通常Ｃ相出力選択 */
			pAsicHwReg->AREG_DIVSET &= 0xfffff3ff;	/* <S204> */
		}
/* <S0CB> End */
#endif	/* <S114> : SO7(分周Ｃ相設定)の強制出力はSGD7Sのみ有効です。 */

		FunExe->State = 1;
	}

	return PRM_RSLT_SUCCESS;

}

/****************************************************************************
 200EH : Leave function of HW OutPut Signal By Force
****************************************************************************/
void     FnLeaveForceSignalOut(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	MENCV			*MencV;			/* <S204> */
	ASIC_HW_IF_STR	*pAsicHwReg;	/* <S204> */
	SENCX 			*Enc;			/* <S204> */

	FnCmnCtrl = Axis->FnCmnCtrl;
	MencV = Axis->MencV;						/* <S204> */
	pAsicHwReg = Axis->SvAsicRegs->AsicHwReg;	/* <S204> */
	Enc = &Axis->BaseLoops->MencV->EncIf;		/* <S0CB> */

/* <S204> Start */
	/* 分周C相 常時Off */
	pAsicHwReg->AREG_DIVSET &= ~BIT15;

	/*----------------------------------------------------------------------*/
	/*  分周モード設定														*/
	/*----------------------------------------------------------------------*/
	switch( MencV->DivInfo.DivModeStatus & 0x0F )
	{
		case DIV_MODE_NORMAL:	/* 分周スルーモード時の設定値 */
			pAsicHwReg->AREG_DIVSET &= 0xfffff3ff;
			break ;
		case DIV_MODE_DCC:	/* 分周モード1時の設定値 */
			/* 分周Ｃ相出力選択１ */
			/* DIVSET bit8 = On : データパルス変換出力, DIVSET bit10-11 = 1 : 分周C相 */
			pAsicHwReg->AREG_DIVSET |= (BIT10 | BIT8);
			break ;
		case DIV_MODE_DCC2:/* 分周モード2時の設定値 */
			/* 分周Ｃ相出力選択２ */
			/* DIVSET bit8 = On : データパルス変換出力, DIVSET bit10-11 = 2 : 分周C相2 */
			pAsicHwReg->AREG_DIVSET |= (BIT11 | BIT8);
			break ;
		default :
			/*--- 処理なし(ここにはこない) ---*/
			break ;
	}

	/*----------------------------------------------------------------------*/
	/*  分周C相マスク設定													*/
	/*----------------------------------------------------------------------*/
	switch( MencV->DivInfo.DivCMaskStatus & 0x0F )
	{
		case DIV_CMASK_OFF:
			/* パルス変換C相マスクOFF設定 */
			Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;
			break ;
		case DIV_CMASK_ON:
			/* パルス変換C相マスクON設定 */
			Enc->SencSetReg->AREG_WDT1SET |= BIT8;
			break ;
		default :
			/*--- 処理なし(ここにはこない) ---*/
			break ;
	}
/* <S204> End */

	FnCmnCtrl->OutModeFlg = FALSE;	/* 自動出力モードフラグ = False */	/* <S0CB> */

#ifndef HW_FBA_IO_AXIS
	/* disable the force output signals */
//	HALdrv_WriteForceOutputSignals(FnCmnCtrl->ForceOutSignals, FALSE);									/* <S0CB> */
	FnCmnCtrl->ForceOutSignalsHW = HALdrv_WriteForceOutputSignals(FnCmnCtrl->ForceOutSignals, FALSE);	/* <S0CB> */
#else
	/* disable the force output signals */
	HALdrv_WriteForceOutputSignalsAx(FnCmnCtrl->ForceOutSignals, FALSE,
									 &(Axis->IoSignals->SerialOutputAx), Axis->SvAsicRegs->AsicHwReg);
#endif
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSvControl = FALSE;
}

/****************************************************************************
 1090H: Encoder data alarm reset
****************************************************************************/
PRM_RSLT FnExecuteEncDataAlmClear(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
#if 0/* TODO:SigmaV_MD独自仕様？ *///<S0C7>
	USHORT wk;
	wk = 0;
	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* Clear EEPROM Encoder data alarm	*/
	EepdevWriteNoSumValues( Axis->AlmManager->EepDevice, 0,
							EEP_NOSUM_ADDRESS(Axis->AlmManager->myAsicNo, EEP_INIALM_OFS),
							&wk,
							1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/******************************* End of File *******************************/
