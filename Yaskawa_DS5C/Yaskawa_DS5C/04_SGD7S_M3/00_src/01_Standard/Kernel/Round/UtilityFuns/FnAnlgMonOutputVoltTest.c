/****************************************************************************
  Description   : アナログモニタ出力電圧確認試験モード（Fn3004）	<S0FD>
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2013 All Rights Reserved

  Project       : Mercury

  Create Date   : 2014-01-23

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
#define	TRUE	1					/* 実行可能 */
#define	FALSE	0					/* 実行不可能 */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/

/****************************************************************************
 Fn3004: Entry function of Analog Monitor Output Volt Check Test
****************************************************************************/
PRM_RSLT FnEnterAnlgMonOutputVoltTest(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
	FnCmnCtrl = Axis->FnCmnCtrl;

	FnCmnCtrl->KnlMode = KPI_KMD_ANLGMONVOLTTEST;	/* アナログモニタ出力電圧試験中 */

	FunExe->HoldFnMode = TRUE;						/* Fnモード自動終了なし */
	rc = PRM_RSLT_SUCCESS;

	return (rc);
}

/****************************************************************************
 Fn3004: Execute function of Analog Monitor Output Volt Check Test
****************************************************************************/
PRM_RSLT FnExecuteAnlgMonOutputVoltTest(FUNEXE *FunExe, AXIS_HANDLE *Axis)
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
		/* アナログモニタ出力電圧確認試験中フラグ = True */
		FnCmnCtrl->AnlgMonVoltTstFlag = TRUE;

		/*	★実際の処理はScanBで実行される「BpxOutputAnalogMonitor():アナログモニタ出力処理」にて毎スキャン実行される。	*/
		/* 	　(通常のアナログモニタ出力処理は本補助機能を終了するまで実行されません。)										*/

		rc = PRM_RSLT_SUCCESS;
	}

	return(rc);

}

/****************************************************************************
 Fn3004: End function of Analog Monitor Output Volt Check Test
****************************************************************************/
void FnLeaveAnlgMonOutputVoltTest(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* アナログモニタ出力電圧確認試験中フラグ = False */
	FnCmnCtrl->AnlgMonVoltTstFlag = FALSE;

	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}
