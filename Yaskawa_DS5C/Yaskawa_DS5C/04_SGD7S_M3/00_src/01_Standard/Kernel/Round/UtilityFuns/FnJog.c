/****************************************************************************
  Description   : JOG Operation(Fn002, Fn003, Fn004)
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
#include "KLib.h"


/****************************************************************************
		DEFINES
****************************************************************************/
#define	ZSRCH_DRV			0x00
#define	ZSRCH_END			0x01
#define	ZSRCH_ERR			0x02

/****************************************************************************
 Fn002: Entry function of JOG operation
****************************************************************************/
PRM_RSLT FnEnterJOGoperation(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	FunExe->HoldFnMode = TRUE;/* Fnモード自動終了なし */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn002: Execute function of JOG operation
****************************************************************************/
PRM_RSLT FnExecuteJOGoperation(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	MENCV			*MencV;
	ALMMAN_HANDLE	*AlmManager;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	LONG			JogSpdP1;

	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	MencV = Axis->MencV;
	AlmManager = Axis->AlmManager;
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	errRes = PRM_RSLT_SUCCESS;

	if(FunExe->State == 0)
	{/* サーボOFF中	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_SV:	/* サーボON	*/
			if( (SeqCtrlOut->MainPowerOn == FALSE)
				|| (SeqCtrlOut->HwbbSts != FALSE)
				|| (AlmManager->AlmReady == FALSE) )
			{
				errRes = PRM_RSLT_CONDITION_ERR;
			}
			else
			{
				FnCmnCtrl->FnSvonReq = TRUE;
			}
			break;

		case FCMD_UP:	/* 正転	*/
		case FCMD_DOWN:	/* 逆転	*/
		default:
			break;
		}

		FnCmnCtrl->JogSpeed = 0;
		if(SeqCtrlOut->BaseEnableReq != FALSE)
		{
			FunExe->State = 1;
		}
	}
	else
	{/* サーボON中	*/
		JogSpdP1 = (MencV->AxisMotType == MOTTYPE_LINEAR) ?
					Prm->jogspd_l : Prm->jogspd;
		if(MencV->LowSpdMotor == FALSE)
		{
			JogSpdP1 = (10 * JogSpdP1);
		}

		switch(FunExe->CmdCode)
		{
		case FCMD_STOP:	/* 停止	*/
			FnCmnCtrl->JogSpeed = 0;
			break;
		case FCMD_UP:	/* 正転	*/
			FnCmnCtrl->JogSpeed = BprmSpeedLevelCal(Bprm, JogSpdP1, 0);
			KlibRstLongTimer(&FunExe->LongTimer);
			break;

		case FCMD_DOWN:	/* 逆転	*/
			FnCmnCtrl->JogSpeed = -BprmSpeedLevelCal(Bprm, JogSpdP1, 0);
			KlibRstLongTimer(&FunExe->LongTimer);
			break;

		case FCMD_SV:	/* サーボオフ	*/
			FnCmnCtrl->FnSvonReq = FALSE;
			break;
		default:
			if(KlibGetLongTimerMs(FunExe->LongTimer)
				> (ULONG)(FnCmnCtrl->SafeStopTime*1000))
			{
				FnCmnCtrl->JogSpeed = 0;
			}
			break;
		}

		if(SeqCtrlOut->BaseEnableReq == FALSE)
		{
			FunExe->State = 0;
		}
	}

	return errRes;
}


/****************************************************************************
 Fn002: Leave function of JOG operation
****************************************************************************/
void FnLeaveJOGoperation(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
//	SEQ_CTRL_OUT	*SeqCtrlOut;

	FnCmnCtrl = Axis->FnCmnCtrl;

//	while(SeqCtrlOut->BaseEnableReq != FALSE);

	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->FnSvonReq = FALSE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->FnSvControl = FALSE;
}


/****************************************************************************
 Fn003: Entry function of Origin Point Search
****************************************************************************/
PRM_RSLT FunEnterOriginSearch(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_ZSRCH;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	/* Fnモード自動終了なし */
	FunExe->HoldFnMode = TRUE;

	/* レジスタ設定(原点サーチ開始)	*/
	FnCmnCtrl->OpeReg.ZsrchState = ZSRCH_DRV;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn003: Execute function origin Point Search
****************************************************************************/
PRM_RSLT FunExecuteOriginSearch(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	ZX_CTRL_HNDL	*ZeroClampCtrl;
	ALMMAN_HANDLE	*AlmManager;

	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	ZeroClampCtrl = &Axis->BaseControls->ZeroClampCtrl;
	AlmManager = Axis->AlmManager;

	errRes = PRM_RSLT_SUCCESS;

	switch(FunExe->State)
	{
		case 0:	/* 初期化処理	*/
			FunExe->State = 1;		/* 処理ステートの移行	*/
			break;

		case 1:	/* サーボOFF中	*/
			FnCmnCtrl->JogSpeed = 0;	/* サーボオフ中は常に速度指令=0とする */

			switch(FunExe->CmdCode)
			{
				case FCMD_SV:		/* サーボON	*/
					if( (SeqCtrlOut->MainPowerOn == FALSE)
						|| (SeqCtrlOut->HwbbSts != FALSE)
						|| (AlmManager->AlmReady == FALSE) )
					{
						errRes = PRM_RSLT_CONDITION_ERR;
					}
					else
					{
						FnCmnCtrl->FnSvonReq = TRUE;
					}
					break;
				case FCMD_EXEC: case FCMD_CH1: case FCMD_CH2:
				case FCMD_EWRT: case FCMD_MTUP: case FCMD_MTDOWN:
					errRes = PRM_RSLT_CONDITION_ERR;
					break;
				default:
					break;
			}
			/* サーボオンで処理ステートの移行処理 */
			if(SeqCtrlOut->BaseEnableReq != FALSE)
			{
				FunExe->State = 2;
			}
			break;
		case 2:	/* サーボON中	*/
			/* 原点サーチ完了判断	*/
			if(FnCmnCtrl->FnCtrlMcmd == CTRLMODE_ZSRCH
			&& ZeroClampCtrl->var.OrgSrchFinish == TRUE)
			{
				FunExe->State = 3;
				break;
			}

			switch(FunExe->CmdCode)
			{
				//case FCMD_NONE:
				case FCMD_STOP:
					FnCmnCtrl->JogSpeed = 0;					/* 原点サーチ運転速度ゼロ	*/
					break;

				case FCMD_UP:		/* 正転	*/
					FnCmnCtrl->JogSpeed = ZeroClampCtrl->conf.OrgSrchSpeed;	/* 原点サーチ速度 */
					KlibRstLongTimer(&FunExe->LongTimer);
					break;
				case FCMD_DOWN:		/* 逆転	*/
					FnCmnCtrl->JogSpeed = -ZeroClampCtrl->conf.OrgSrchSpeed;	/* 原点サーチ速度 */
					KlibRstLongTimer(&FunExe->LongTimer);
					break;
				case FCMD_SV:		/* サーボオフ	*/
					FnCmnCtrl->FnSvonReq = FALSE;
					break;
				case FCMD_EXEC: case FCMD_CH1: case FCMD_CH2:
				case FCMD_EWRT: case FCMD_MTUP: case FCMD_MTDOWN:
					errRes = PRM_RSLT_CONDITION_ERR;
					break;
				default:
					if(KlibGetLongTimerMs(FunExe->LongTimer)
						> (ULONG)(FnCmnCtrl->SafeStopTime*1000))
					{
						FnCmnCtrl->JogSpeed = 0;
					}
					break;
			}
			/* サーボオフで処理ステートの移行処理 */
			if(SeqCtrlOut->BaseEnableReq == FALSE)
			{
				FunExe->State = 1;
			}
			break;
		case 3:	/* サーチ完了(サーボオン状態)	*/
			switch(FunExe->CmdCode)
			{
				//case FCMD_NONE:
				case FCMD_STOP:
					break;//120716_kira
				case FCMD_SV:
					FnCmnCtrl->FnSvonReq = FALSE;
					break;
				case FCMD_EXEC: case FCMD_CH1: case FCMD_CH2:
				case FCMD_EWRT: case FCMD_MTUP: case FCMD_MTDOWN:
					errRes = PRM_RSLT_CONDITION_ERR;
					break;
				default:
					break;
			}
			/* サーボオフで処理ステートの移行処理 */
			if(SeqCtrlOut->BaseEnableReq == FALSE)
			{
				FunExe->State = 1;
			}
			break;//<S0C7>
		default:
			break;
	}

	if( (SeqCtrlOut->MainPowerOn == FALSE)
		|| (SeqCtrlOut->HwbbSts != FALSE)
		|| (AlmManager->AlmReady == FALSE) )
	{
		FnCmnCtrl->OpeReg.ZsrchState = ZSRCH_ERR;	/* レジスタ設定 (アラーム発生/HWBB状態/主回路遮断) */
	}
	else if(FnCmnCtrl->FnCtrlMcmd == CTRLMODE_ZSRCH
		 && ZeroClampCtrl->var.OrgSrchFinish == TRUE)
	{
		FnCmnCtrl->OpeReg.ZsrchState = ZSRCH_END;	/* レジスタ設定 (原点サーチ完了) */
	}
	else
	{
		FnCmnCtrl->OpeReg.ZsrchState = ZSRCH_DRV;
	}

	return(errRes);
}

/****************************************************************************
 Fn003: Leave function of Origin Point Search
****************************************************************************/
void FunLeaveOriginSearch(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
//	SEQ_CTRL_OUT	*SeqCtrlOut;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* ラウンド制御中のSEN信号ON要求をI/Oの状態で設定する */
	//RoutV.f.SensOnReq = (CinV.f.Sv.Sen == TRUE) ? TRUE : FALSE ;

	//while(SeqCtrlOut->BaseEnableReq != FALSE);

	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->FnSvonReq = FALSE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->FnSvControl = FALSE;
}

/******************************* End of File *******************************/
