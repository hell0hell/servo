/****************************************************************************
  Description   : プッシュボタン・7SEG-LED確認モード（Fn3000）
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2013 All Rights Reserved

  Project       : Mercury

  Create Date   : 2013-12-xx

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"

#include "PnPrmListTbl.h"
#include "KLib.h"
#include "PnlOp.h"
#include "HwDeviceIf.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/* アナログモニタ出力のマニュアルゼロ調整　最大値/最小値 */
#define	ZADJAM_MAX			127
#define	ZADJAM_MIN			-128

/* アナログモニタ出力のマニュアルゲイン調整　最大値/最小値 */
#define	GADJAM_MAX			127
#define	GADJAM_MIN			-128


#define	OK	1					/* 実行可能 */
#define	NG	0					/* 実行不可能 */


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/



/****************************************************************************
 Fn3000: Entry function of 7 SegLed And Push Button Check Test
****************************************************************************/
PRM_RSLT FnEnterSevenSegLedChkTest(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
//	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
//	{
//		return PRM_RSLT_CONDITION_ERR;
//	}

	FnCmnCtrl->KnlMode = KPI_KMD_PBLEDTEST;		/* プッシュボタン・7SEG-LED確認中  */
	FunExe->HoldFnMode = TRUE;					/* Fnモード自動終了なし */
	rc = PRM_RSLT_SUCCESS;

	return (rc);
}

/****************************************************************************
 Fn3000: Execute function of 7 SegLed And Push Button Check Test
****************************************************************************/
PRM_RSLT FnExecuteSevenSegLedChkTest(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT				rc;
	CMN_PRMDATA				*Cprm;
	PRMDATA					*Prm;
	FUN_CMN_CONTROL			*FnCmnCtrl;

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		rc = PRM_RSLT_CONTINUE;
	}
	else
	{
		/*	実行要求が来たら、プッシュボタン&7SegLed確認フラグをTrueとする。
			実際の処理はScanCの「SyscSequenceCommon()」で実行する。(通常のLED出力処理は実行しない)
		*/
		FnCmnCtrl->PbAndLedChkTestFlag = 0x01;


	/* AP形 プッシュボタン&7SegLed開始フラグをFalseとする。*/
	/* 実際の処理はScanCの「PnlStatusDispMode()」で実行する。(通常のLED出力処理は実行しない) */

		FnCmnCtrl->AnlgLedChkInitFlag = 0x00;	

		/* AP形 プッシュボタン&7SegLed確認 カウンタ クリア*/
		FnCmnCtrl->AnlgLedChkCnt = 0x00;

		rc = PRM_RSLT_SUCCESS;
	}
//		return PRM_RSLT_CONTINUE;
	return(rc);

}

/****************************************************************************
 Fn3000: End function of 7 SegLed And Push Button Check Test
****************************************************************************/
void FnLeaveSevenSegLedChkTest(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* プッシュボタン&7SegLed確認フラグをFalseとする。*/
	FnCmnCtrl->PbAndLedChkTestFlag = 0x00;

	/* AP形 プッシュボタン&7SegLed開始フラグをFalseとする。*/
	FnCmnCtrl->AnlgLedChkInitFlag = 0x00;

	/* AP形 プッシュボタン&7SegLed確認 カウンタ クリア*/
	FnCmnCtrl->AnlgLedChkCnt = 0x00;

	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}
