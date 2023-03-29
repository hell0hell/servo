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

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fn�T�[�{�^�]���l�� */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	FunExe->HoldFnMode = TRUE;/* Fn���[�h�����I���Ȃ� */

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
	{/* �T�[�{OFF��	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_SV:	/* �T�[�{ON	*/
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

		case FCMD_UP:	/* ���]	*/
		case FCMD_DOWN:	/* �t�]	*/
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
	{/* �T�[�{ON��	*/
		JogSpdP1 = (MencV->AxisMotType == MOTTYPE_LINEAR) ?
					Prm->jogspd_l : Prm->jogspd;
		if(MencV->LowSpdMotor == FALSE)
		{
			JogSpdP1 = (10 * JogSpdP1);
		}

		switch(FunExe->CmdCode)
		{
		case FCMD_STOP:	/* ��~	*/
			FnCmnCtrl->JogSpeed = 0;
			break;
		case FCMD_UP:	/* ���]	*/
			FnCmnCtrl->JogSpeed = BprmSpeedLevelCal(Bprm, JogSpdP1, 0);
			KlibRstLongTimer(&FunExe->LongTimer);
			break;

		case FCMD_DOWN:	/* �t�]	*/
			FnCmnCtrl->JogSpeed = -BprmSpeedLevelCal(Bprm, JogSpdP1, 0);
			KlibRstLongTimer(&FunExe->LongTimer);
			break;

		case FCMD_SV:	/* �T�[�{�I�t	*/
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

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fn�T�[�{�^�]���l�� */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_ZSRCH;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	/* Fn���[�h�����I���Ȃ� */
	FunExe->HoldFnMode = TRUE;

	/* ���W�X�^�ݒ�(���_�T�[�`�J�n)	*/
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
		case 0:	/* ����������	*/
			FunExe->State = 1;		/* �����X�e�[�g�̈ڍs	*/
			break;

		case 1:	/* �T�[�{OFF��	*/
			FnCmnCtrl->JogSpeed = 0;	/* �T�[�{�I�t���͏�ɑ��x�w��=0�Ƃ��� */

			switch(FunExe->CmdCode)
			{
				case FCMD_SV:		/* �T�[�{ON	*/
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
			/* �T�[�{�I���ŏ����X�e�[�g�̈ڍs���� */
			if(SeqCtrlOut->BaseEnableReq != FALSE)
			{
				FunExe->State = 2;
			}
			break;
		case 2:	/* �T�[�{ON��	*/
			/* ���_�T�[�`�������f	*/
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
					FnCmnCtrl->JogSpeed = 0;					/* ���_�T�[�`�^�]���x�[��	*/
					break;

				case FCMD_UP:		/* ���]	*/
					FnCmnCtrl->JogSpeed = ZeroClampCtrl->conf.OrgSrchSpeed;	/* ���_�T�[�`���x */
					KlibRstLongTimer(&FunExe->LongTimer);
					break;
				case FCMD_DOWN:		/* �t�]	*/
					FnCmnCtrl->JogSpeed = -ZeroClampCtrl->conf.OrgSrchSpeed;	/* ���_�T�[�`���x */
					KlibRstLongTimer(&FunExe->LongTimer);
					break;
				case FCMD_SV:		/* �T�[�{�I�t	*/
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
			/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs���� */
			if(SeqCtrlOut->BaseEnableReq == FALSE)
			{
				FunExe->State = 1;
			}
			break;
		case 3:	/* �T�[�`����(�T�[�{�I�����)	*/
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
			/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs���� */
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
		FnCmnCtrl->OpeReg.ZsrchState = ZSRCH_ERR;	/* ���W�X�^�ݒ� (�A���[������/HWBB���/���H�Ւf) */
	}
	else if(FnCmnCtrl->FnCtrlMcmd == CTRLMODE_ZSRCH
		 && ZeroClampCtrl->var.OrgSrchFinish == TRUE)
	{
		FnCmnCtrl->OpeReg.ZsrchState = ZSRCH_END;	/* ���W�X�^�ݒ� (���_�T�[�`����) */
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

	/* ���E���h���䒆��SEN�M��ON�v����I/O�̏�ԂŐݒ肷�� */
	//RoutV.f.SensOnReq = (CinV.f.Sv.Sen == TRUE) ? TRUE : FALSE ;

	//while(SeqCtrlOut->BaseEnableReq != FALSE);

	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->FnSvonReq = FALSE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;
	FnCmnCtrl->FnSvControl = FALSE;
}

/******************************* End of File *******************************/
