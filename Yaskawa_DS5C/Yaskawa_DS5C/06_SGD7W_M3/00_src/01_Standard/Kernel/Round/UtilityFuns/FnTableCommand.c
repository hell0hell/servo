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

	/* サーボオン中の場合は入らずに終了 */
	if( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		return( PRM_RSLT_CONDITION_ERR );
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->JogSpeed = 0;

	/* Fnモード自動終了なし */
	FunExe->HoldFnMode = TRUE;

	/* ステータス：イニシアル設定 <S212> */
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

	/* Fnサーボ運転指令設定(NOCMD) */
	FnCmnCtrl->JogSpeed = 0;
	FnCmnCtrl->FnSvonReq = FALSE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_NOCMD;
	FnCmnCtrl->FnSensOnReq = FALSE;

	/* サーボオフするまで待機 */
	while( TRUE == Axis->SeqCtrlOut->BaseEnableReq )
	{
		;
	}

	/* テーブル運転ストップ */
	TblDriveSetStopRequest( TblDrive );

	/* Fnサーボ運転権解放 */
	FnCmnCtrl->FnSvControl = FALSE;

	/* 調整レス強制無効要求解除 */
	TuneLessClearForceInvldReq( &Axis->BaseControls->TuneLessCtrl );

	/* Fnモード自動終了 */
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
	case 0:	/* 初期化処理 */
		/* テーブル運転準備処理 */
		rc = TblDrivePrepareProc( TblDrive );
		if( rc == PRM_RSLT_SUCCESS )
		{
			/* テーブル運転処理 */
			FunExe->State = 1;

			/* 制御モード設定 */
			FnCmnCtrl->FnCtrlMcmd = TblDrive->conf.CtrlMode;
			FnCmnCtrl->FnSensOnReq = TRUE;

			/* 調整レス無効判別 */
			if( TblDrive->conf.TuneLessDisable )
			{
				/* 調整レス強制無効要求 */
				TuneLessSetForceInvldReq( &(Axis->BaseControls->TuneLessCtrl) );
			}
		}
		else
		{
			FunExe->State = 4;	/* 終了処理	*/
		}
		break;

	case 1:	/* サーボオフ中 */
		switch( FunExe->CmdCode )
		{
			case FCMD_NONE:		/* コマンドなし */
				break;

			case FCMD_SV:		/* サーボオン指令 */
//<S0E5>		if( (!(ALMGetAlarmInfo(Axis->AlmManager)) && !(Axis->SeqCtrlOut->HwbbSts) && (Axis->SeqCtrlOut->MainPowerOn) )
//<S108>		if( (((Axis->AlmManager)->Status.AlmFlag != FALSE) && !(Axis->SeqCtrlOut->HwbbSts) && (Axis->SeqCtrlOut->MainPowerOn) )	/* <S0E5> */
				if( (((Axis->AlmManager)->Status.AlmFlag == FALSE) && !(Axis->SeqCtrlOut->HwbbSts) && (Axis->SeqCtrlOut->MainPowerOn) )	/* <S0E5> */
					&& ((TblDrive->var.TblCmd == TBLCMD_TABLESTART) || (TblDrive->var.TblCmd == TBLCMD_TBLDRV_END)) )
				{ /* テーブル運転開始条件を満たす場合 */
					FnCmnCtrl->FnSvonReq = TRUE;
				}
				else
				{
					rc = PRM_RSLT_CONDITION_ERR;
				}
				break;

			default:			/* それ以外のコマンド *//* <S191> */
//				rc = PRM_RSLT_CMD_ERR;
				break;
		}
		/* サーボオンで処理ステートの移行処理	*/
		if( Axis->SeqCtrlOut->BaseEnableReq == TRUE )
		{
			FunExe->State = 2;
			/* 命令コード保持クリア */
			TblDrive->var.KeepTblMode = FALSE;
		}
		break;

	case 2:	/* サーボオン中 */
		switch( FunExe->CmdCode )
		{
		case FCMD_NONE:		/* コマンドなし */
			break;

		case FCMD_UP:		/* 正転 */
		case FCMD_MTUP:		/* 正転：操作モード維持 */
			rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_FOR );
			if( rc == PRM_RSLT_SUCCESS )
			{
				/* テーブル運転処理 */
				FunExe->State = 3;
				if( FunExe->CmdCode == FCMD_MTUP )
				{ /* 操作モード維持 */
					TblDrive->var.KeepTblMode = TRUE;
				}
			}
			else
			{
				/* 終了処理 */
				FunExe->State = 4;
			}
			break;

		case FCMD_DOWN:		/* 逆転					*/
		case FCMD_MTDOWN:	/* 逆転：操作モード維持	*/
			rc = TblDriveStartRequest( TblDrive, TBLCMD_DRV_REV );
			if( rc == PRM_RSLT_SUCCESS )
			{
				/* テーブル運転処理 */
				FunExe->State = 3;
				if( FunExe->CmdCode == FCMD_MTDOWN )
				{ /* 操作モード維持 */
					TblDrive->var.KeepTblMode = TRUE;
				}
			}
			else
			{
				/* 終了処理 */
				FunExe->State = 4;
			}
			break;

		case FCMD_SV:		/* サーボオフ */
			FnCmnCtrl->FnSvonReq = FALSE;
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		/* サーボオフで処理ステートの移行処理(外部要因でのサーボオフを含むため、サーボオン要求をクリア)	*/
		if( Axis->SeqCtrlOut->BaseEnableReq != TRUE )
		{
			FunExe->State = 1;
			FnCmnCtrl->FnSvonReq = FALSE;
		}
		break;

	case 3:	/* テーブル運転中 */
		/* テーブル運転終了確認 */
		if( (TblDrive->var.TblCmd == TBLCMD_TBLDRV_END) || (TblDrive->var.TblCmd == TBLCMD_TBLDRV_ERR) )
		{
			if( TblDrive->var.KeepTblMode == TRUE )
			{ /* テーブル運転モード継続要求ありの場合 */
				/* サーボオン中 */
				FunExe->State = 2;
				TblDrive->var.KeepTblMode = FALSE;
			}
			else
			{
				FunExe->State = 4;	/* テーブル運転終了 */
			}
			/* テーブル運転中ステートから抜ける */
		}
		else
		{
			switch( FunExe->CmdCode )
			{
			case FCMD_UP:				/* 正転					*/
			case FCMD_DOWN:				/* 逆転					*/
			case FCMD_MTUP:				/* 正転：操作モード維持	*/
			case FCMD_MTDOWN:			/* 逆転：操作モード維持	*/
				break;

			case FCMD_SV:				/* サーボオフ			*/
				if( TblDrive->var.KeepTblMode == TRUE )
				{ /* テーブル運転継続要求ありの場合 */
					/* テーブル運転ストップ */
					TblDriveSetStopRequest( TblDrive );
					FnCmnCtrl->FnSvonReq = FALSE;
				}
				else
				{
					/* テーブル運転終了	*/
					FunExe->State = 5;
				}
				break;

			default:
				rc = PRM_RSLT_CMD_ERR;
				break;
			}
			/* サーボオフで処理ステートの移行処理 */
			/* 外部要因でのサーボオフを含むため、サーボオン要求をクリア */
			if( Axis->SeqCtrlOut->BaseEnableReq != TRUE )
			{
				FunExe->State = 1;
				FnCmnCtrl->FnSvonReq = FALSE;
			}
		}
		break;

	case 4:	/* 終了処理 */
		FnLeaveTableCommandMode( FunExe, Axis );
		break;

	case 5:
		FnCmnCtrl->FnSvonReq = FALSE;
		if( (TblDrive->var.TblCmd == TBLCMD_TBLDRV_END)
			|| (TblDrive->var.TblCmd == TBLCMD_TBLDRV_ERR) )
		{
			/* テーブル運転終了 */
			FunExe->State = 4;
		}
		break;

	default:
		break;
	}
	return(rc);
}





/****************************************************************************************************/
