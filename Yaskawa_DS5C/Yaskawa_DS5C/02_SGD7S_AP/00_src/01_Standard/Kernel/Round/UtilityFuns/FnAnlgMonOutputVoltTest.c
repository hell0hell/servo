/****************************************************************************
  Description   : �A�i���O���j�^�o�͓d���m�F�������[�h�iFn3004�j	<S0FD>
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
#define	TRUE	1					/* ���s�\ */
#define	FALSE	0					/* ���s�s�\ */

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

	FnCmnCtrl->KnlMode = KPI_KMD_ANLGMONVOLTTEST;	/* �A�i���O���j�^�o�͓d�������� */

	FunExe->HoldFnMode = TRUE;						/* Fn���[�h�����I���Ȃ� */
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

	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		rc = PRM_RSLT_CONTINUE;
	}
	else
	{
		/* �A�i���O���j�^�o�͓d���m�F�������t���O = True */
		FnCmnCtrl->AnlgMonVoltTstFlag = TRUE;

		/*	�����ۂ̏�����ScanB�Ŏ��s�����uBpxOutputAnalogMonitor():�A�i���O���j�^�o�͏����v�ɂĖ��X�L�������s�����B	*/
		/* 	�@(�ʏ�̃A�i���O���j�^�o�͏����͖{�⏕�@�\���I������܂Ŏ��s����܂���B)										*/

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

	/* �A�i���O���j�^�o�͓d���m�F�������t���O = False */
	FnCmnCtrl->AnlgMonVoltTstFlag = FALSE;

	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}
