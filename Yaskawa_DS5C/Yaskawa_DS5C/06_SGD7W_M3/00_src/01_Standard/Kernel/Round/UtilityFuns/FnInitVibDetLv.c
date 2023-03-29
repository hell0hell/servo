/****************************************************************************
  Description   : Initialize Vibration Detect Level (Fn01B)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.31   created

****************************************************************************/
#include "FnInitVibDetLvl.h"
#include "MercuryGlobals.h"
#include "MLib.h"
#include "KLib.h"
#include "PnPrmListTbl.h"


/****************************************************************************************************/
/*																									*/
/*		Fn01B : Initialize Vibration Detect Level													*/
/*																									*/
/****************************************************************************************************/
/* -------- Fn01B Initialize Threshhold Level for Vibration Detect Begin	-------- */
PRM_RSLT	FnEnterInitVibDetLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/* モータレステストモードの場合は入らずに終了 *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnモード自動終了なし */
	FunExe->HoldFnMode = TRUE;

	return(rc);
}


/* -------- Fn01B Initialize Threshhold Level for Vibration Detect End	-------- */
void	FnLeaveInitVibDetLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	/* Fnモード自動終了設定 */
	FunExe->HoldFnMode = FALSE;

	return;
}


/* -------- Fn01B Initialize Threshhold Level for Vibration Detect Execution	-------- */
PRM_RSLT	FnExecuteInitVibDetLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	LONG			lwk0;
	PRM_RSLT		lrc;
	REG_ACC_T		AccessPrm;
	INIVIBDET		*IniVibDet;
	BPRMDAT			*Bprm;
	REGIF_CONFIG_T	*RegCfg;
	FUN_CMN_CONTROL		*FnCmnCtrl;		/* <S085> */

	IniVibDet = Axis->IniVibDet;
	Bprm = Axis->UniPrms->Bprm;
	RegCfg = Axis->RegManager->hPnReg;
	rc = PRM_RSLT_SUCCESS;
	FnCmnCtrl = Axis->FnCmnCtrl;		/* <S085> */

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch( FunExe->State )
	{
	case VIBDETLVL_SEQ_INIT:	/* 検出開始待ち処理	*/
		switch( FunExe->CmdCode )
		{
		case FCMD_STOP:			/* 終了				*/
			FunExe->State = VIBDETLVL_SEQ_END;
			break;

		case FCMD_EXEC:			/* 検出開始		*/
			IniVibDet->var.DetSpdActive = FALSE;
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			FunExe->State = VIBDETLVL_SEQ_DETECT;
			break;

		case FCMD_UP:
		case FCMD_DOWN:
		case FCMD_SV:
		case FCMD_CH1:
		case FCMD_CH2: 
		case FCMD_EWRT:
		case FCMD_MTUP:
		case FCMD_MTDOWN: 
			rc = PRM_RSLT_CMD_ERR;
			break;

		default:
			break;
		}
		break;

	case VIBDETLVL_SEQ_DETECT:
		switch( FunExe->CmdCode )
		{
		case FCMD_STOP:			/* 検出終了＆書込 */
			rc = PRM_RSLT_CONTINUE;
			if( MlibAbs32( Axis->UnMonitor->UnMotSpdx ) > VIBDET_LVL )
			{
				IniVibDet->var.DetSpdActive = TRUE;
			}

			if( IniVibDet->var.DetSpdActive == TRUE )
			{
#if (FLOAT_USE==TRUE)
				lwk0 = DetVibGetSpdErrPeak(Axis->DetVib) * Bprm->Kmonspd;
#else
				lwk0 = MlibMulgainSx48( DetVibGetSpdErrPeak(Axis->DetVib), Bprm->Kmonspd );
#endif /* FLOAT_USE */

				/* 振動検出レベル書込み */
				if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
				{
//					lrc = RpiPrmWriteRamForFn( &pndef_vibnorm_l, (ULONG)lwk0, pfexe->App.Hmsg );
					lrc = TunePrmWriteRam( &AccessPrm, &pndef_vibnorm_l, (ULONG)lwk0, 2 );
				}
				else
				{
//					lrc = RpiPrmWriteRamForFn( &pndef_vibnorm, (ULONG)lwk0, pfexe->App.Hmsg );
					lrc = TunePrmWriteRam( &AccessPrm, &pndef_vibnorm, (ULONG)lwk0, 2 );
				}
				if( lrc != PRM_RSLT_SUCCESS )
				{
					FnCmnCtrl->OperatorSts = -1;		/* <S085> */
					FunExe->State = VIBDETLVL_SEQ_ERROR;
				}

				/* EEPROM更新 */
//				lrc = RpiFunPrmNvmemUpdate( &pndef_vibnorm );
				lrc = PrmStoreValueToEeprom( RegCfg, &pndef_vibnorm, 0 );
				if( lrc != PRM_RSLT_SUCCESS )
				{
					FnCmnCtrl->OperatorSts = -1;		/* <S085> */
					FunExe->State = VIBDETLVL_SEQ_ERROR;
				}
				else
				{
					FnCmnCtrl->OperatorSts = 1;			/* <S085> */
					FunExe->State = VIBDETLVL_SEQ_STORE;
				}
			}
			else
			{
				FnCmnCtrl->OperatorSts = -1;			/* <S085> */
				rc = PRM_RSLT_CONDITION_ERR;
			}
			break;

		case FCMD_NONE:
			if( MlibAbs32( Axis->UnMonitor->UnMotSpdx ) > VIBDET_LVL )
			{
				IniVibDet->var.DetSpdActive = TRUE;
			}
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case VIBDETLVL_SEQ_STORE:
		rc = PRM_RSLT_CONTINUE;
		FunExe->State = VIBDETLVL_SEQ_END;
		break;

	case VIBDETLVL_SEQ_ERROR:
		rc = PRM_RSLT_CONTINUE;
		FunExe->State = VIBDETLVL_SEQ_END;
		break;

	case VIBDETLVL_SEQ_END:		/* 終了処理		*/
		FnLeaveInitVibDetLevel( FunExe, Axis );
		break;

	default:
		break;
	}
	return rc;
}



/****************************************************************************************************/
