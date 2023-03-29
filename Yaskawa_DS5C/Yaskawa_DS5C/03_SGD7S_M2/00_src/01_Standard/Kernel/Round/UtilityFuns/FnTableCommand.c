/****************************************************************************
  Description   : Table Command Drive Mode (only Register IF) (2007H)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.18   created

****************************************************************************/
#include "FnTableCommand.h"
#include "MercuryGlobals.h"
#include "TableDrive.h"
#include "BaseControls.h"
#include "ExControls.h"


/****************************************************************************************************/
/*																									*/
/*		Table Command Drive Mode (only Register IF) (2007H)											*/
/*																									*/
/****************************************************************************************************/
/* -------- Local Function Prototype Definition -------- */



/* -------- 2007H Table Command (only Register IF) Begin	-------- */
PRM_RSLT	FnEnterTableCommandMode( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI�� */
	if( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		return( PRM_RSLT_CONDITION_ERR );
	}

	/* Fn�T�[�{�^�]���l�� */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->JogSpeed = 0;

	/* Fn���[�h�����I���Ȃ� */
	FunExe->HoldFnMode = TRUE;

	/* �X�e�[�^�X�F�C�j�V�A���ݒ� <S212> */
	Axis->BaseControls->TblDrive.OpeReg.TblState = 0;

	return(rc);
}


/* -------- 2007H Table Command (only Register IF) End	-------- */
void	FnLeaveTableCommandMode( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	TBLDRIVE		*TblDrive;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	TblDrive = &(Axis->BaseControls->TblDrive);
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* Fn�T�[�{�^�]�w�ߐݒ�(NOCMD) */
	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->FnSvonReq = FALSE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;

	/* �T�[�{�I�t����܂őҋ@ */
	while( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		;
	}

	/* �e�[�u���^�]�X�g�b�v */
	TblDriveSetStopRequest( TblDrive );

	/* Fn�T�[�{�^�]����� */
	FnCmnCtrl->FnSvControl = FALSE;

	/* �������X���������v������ */
	TuneLessClearForceInvldReq( &Axis->BaseControls->TuneLessCtrl );

	/* Fn���[�h�����I�� */
	FunExe->HoldFnMode = FALSE;
}

/* -------- 2007H Table Command (only Register IF) Exe	-------- */
PRM_RSLT	FnExecuteTableCommandMode( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	TBLDRIVE		*TblDrive;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	TblDrive = &(Axis->BaseControls->TblDrive);
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

	switch( FunExe->State )
	{
	case 0:	/* ���������� */
		/* �e�[�u���^�]�������� */
		rc = TblDrivePrepareProc( TblDrive );
		if( rc == PRM_RSLT_SUCCESS )
		{
			/* �e�[�u���^�]���� */
			FunExe->State = 1;

			/* ���䃂�[�h�ݒ� */
			FnCmnCtrl->FnCtrlMcmd = TblDrive->conf.CtrlMode;
			FnCmnCtrl->FnSensOnReq = TRUE;

			/* �������X�������� */
			if( TblDrive->conf.TuneLessDisable )
			{
				/* �������X���������v�� */
				TuneLessSetForceInvldReq( &(Axis->BaseControls->TuneLessCtrl) );
			}
		}
		else
		{
			FunExe->State = 4;	/* �I������	*/
		}
		break;

	case 1:	/* �T�[�{�I�t�� */
		switch( FunExe->CmdCode )
		{
			case FCMD_NONE:		/* �R�}���h�Ȃ� */
				break;

			case FCMD_SV:		/* �T�[�{�I���w�� */
//<S0E5>		if( (!(ALMGetAlarmInfo(Axis->AlmManager)) && !(Axis->SeqCtrlOut->HwbbSts) && (Axis->SeqCtrlOut->MainPowerOn) )
//<S108>		if( (((Axis->AlmManager)->Status.AlmFlag != FALSE) && !(Axis->SeqCtrlOut->HwbbSts) && (Axis->SeqCtrlOut->MainPowerOn) )	/* <S0E5> */
				if( (((Axis->AlmManager)->Status.AlmFlag == FALSE) && !(Axis->SeqCtrlOut->HwbbSts) && (Axis->SeqCtrlOut->MainPowerOn) )	/* <S0E5> */
					&& ((TblDrive->var.TblCmd == TBLCMD_TABLESTART) || (TblDrive->var.TblCmd == TBLCMD_TBLDRV_END)) )
				{ /* �e�[�u���^�]�J�n�����𖞂����ꍇ */
					FnCmnCtrl->FnSvonReq = TRUE;
				}
				else
				{
					rc = PRM_RSLT_CONDITION_ERR;
				}
				break;

			default:			/* ����ȊO�̃R�}���h *//* <S191> */
//				rc = PRM_RSLT_CMD_ERR;
				break;
		}
		/* �T�[�{�I���ŏ����X�e�[�g�̈ڍs����	*/
		if( Axis->SeqCtrlOut->BaseEnableReq == TRUE )
		{
			FunExe->State = 2;
			/* ���߃R�[�h�ێ��N���A */
			TblDrive->var.KeepTblMode = FALSE;
		}
		break;

	case 2:	/* �T�[�{�I���� */
		switch( FunExe->CmdCode )
		{
		case FCMD_NONE:		/* �R�}���h�Ȃ� */
			break;

		case FCMD_UP:		/* ���] */
		case FCMD_MTUP:		/* ���]�F���샂�[�h�ێ� */
			rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_FOR );
			if( rc == PRM_RSLT_SUCCESS )
			{
				/* �e�[�u���^�]���� */
				FunExe->State = 3;
				if( FunExe->CmdCode == FCMD_MTUP )
				{ /* ���샂�[�h�ێ� */
					TblDrive->var.KeepTblMode = TRUE;
				}
			}
			else
			{
				/* �I������ */
				FunExe->State = 4;
			}
			break;

		case FCMD_DOWN:		/* �t�]					*/
		case FCMD_MTDOWN:	/* �t�]�F���샂�[�h�ێ�	*/
			rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_REV );
			if( rc == PRM_RSLT_SUCCESS )
			{
				/* �e�[�u���^�]���� */
				FunExe->State = 3;
				if( FunExe->CmdCode == FCMD_MTDOWN )
				{ /* ���샂�[�h�ێ� */
					TblDrive->var.KeepTblMode = TRUE;
				}
			}
			else
			{
				/* �I������ */
				FunExe->State = 4;
			}
			break;

		case FCMD_SV:		/* �T�[�{�I�t */
			FnCmnCtrl->FnSvonReq = FALSE;
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs����(�O���v���ł̃T�[�{�I�t���܂ނ��߁A�T�[�{�I���v�����N���A)	*/
		if( Axis->SeqCtrlOut->BaseEnableReq != TRUE )
		{
			FunExe->State = 1;
			FnCmnCtrl->FnSvonReq = FALSE;
		}
		break;

	case 3:	/* �e�[�u���^�]�� */
		/* �e�[�u���^�]�I���m�F */
		if( (TblDrive->var.TblCmd == TBLCMD_TBLDRV_END) || (TblDrive->var.TblCmd == TBLCMD_TBLDRV_ERR) )
		{
			if( TblDrive->var.KeepTblMode == TRUE )
			{ /* �e�[�u���^�]���[�h�p���v������̏ꍇ */
				/* �T�[�{�I���� */
				FunExe->State = 2;
				TblDrive->var.KeepTblMode = FALSE;
			}
			else
			{
				FunExe->State = 4;	/* �e�[�u���^�]�I�� */
			}
			/* �e�[�u���^�]���X�e�[�g���甲���� */
		}
		else
		{
			switch( FunExe->CmdCode )
			{
			case FCMD_UP:				/* ���]					*/
			case FCMD_DOWN:				/* �t�]					*/
			case FCMD_MTUP:				/* ���]�F���샂�[�h�ێ�	*/
			case FCMD_MTDOWN:			/* �t�]�F���샂�[�h�ێ�	*/
				break;

			case FCMD_SV:				/* �T�[�{�I�t			*/
				if( TblDrive->var.KeepTblMode == TRUE )
				{ /* �e�[�u���^�]�p���v������̏ꍇ */
					/* �e�[�u���^�]�X�g�b�v */
					TblDriveSetStopRequest( TblDrive );
					FnCmnCtrl->FnSvonReq = FALSE;
				}
				else
				{
					/* �e�[�u���^�]�I��	*/
					FunExe->State = 5;
				}
				break;

			default:
				rc = PRM_RSLT_CMD_ERR;
				break;
			}
			/* �T�[�{�I�t�ŏ����X�e�[�g�̈ڍs���� */
			/* �O���v���ł̃T�[�{�I�t���܂ނ��߁A�T�[�{�I���v�����N���A */
			if( Axis->SeqCtrlOut->BaseEnableReq != TRUE )
			{
				FunExe->State = 1;
				FnCmnCtrl->FnSvonReq = FALSE;
			}
		}
		break;

	case 4:	/* �I������ */
		FnLeaveTableCommandMode( FunExe, Axis );
		break;

	case 5:
		FnCmnCtrl->FnSvonReq = FALSE;
		if( (TblDrive->var.TblCmd == TBLCMD_TBLDRV_END)
			|| (TblDrive->var.TblCmd == TBLCMD_TBLDRV_ERR) )
		{
			/* �e�[�u���^�]�I�� */
			FunExe->State = 4;
		}
		break;

	default:
		break;
	}
	return(rc);
}





/****************************************************************************************************/
