/****************************************************************************
  Description   : �v�b�V���{�^���E7SEG-LED�m�F���[�h�iFn3000�j
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
/* �A�i���O���j�^�o�͂̃}�j���A���[�������@�ő�l/�ŏ��l */
#define	ZADJAM_MAX			127
#define	ZADJAM_MIN			-128

/* �A�i���O���j�^�o�͂̃}�j���A���Q�C�������@�ő�l/�ŏ��l */
#define	GADJAM_MAX			127
#define	GADJAM_MIN			-128


#define	OK	1					/* ���s�\ */
#define	NG	0					/* ���s�s�\ */


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

	/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
//	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
//	{
//		return PRM_RSLT_CONDITION_ERR;
//	}

	FnCmnCtrl->KnlMode = KPI_KMD_PBLEDTEST;		/* �v�b�V���{�^���E7SEG-LED�m�F��  */
	FunExe->HoldFnMode = TRUE;					/* Fn���[�h�����I���Ȃ� */
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

	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		rc = PRM_RSLT_CONTINUE;
	}
	else
	{
		/*	���s�v����������A�v�b�V���{�^��&7SegLed�m�F�t���O��True�Ƃ���B
			���ۂ̏�����ScanC�́uSyscSequenceCommon()�v�Ŏ��s����B(�ʏ��LED�o�͏����͎��s���Ȃ�)
		*/
		FnCmnCtrl->PbAndLedChkTestFlag = 0x01;


	/* AP�` �v�b�V���{�^��&7SegLed�J�n�t���O��False�Ƃ���B*/
	/* ���ۂ̏�����ScanC�́uPnlStatusDispMode()�v�Ŏ��s����B(�ʏ��LED�o�͏����͎��s���Ȃ�) */

		FnCmnCtrl->AnlgLedChkInitFlag = 0x00;	

		/* AP�` �v�b�V���{�^��&7SegLed�m�F �J�E���^ �N���A*/
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

	/* �v�b�V���{�^��&7SegLed�m�F�t���O��False�Ƃ���B*/
	FnCmnCtrl->PbAndLedChkTestFlag = 0x00;

	/* AP�` �v�b�V���{�^��&7SegLed�J�n�t���O��False�Ƃ���B*/
	FnCmnCtrl->AnlgLedChkInitFlag = 0x00;

	/* AP�` �v�b�V���{�^��&7SegLed�m�F �J�E���^ �N���A*/
	FnCmnCtrl->AnlgLedChkCnt = 0x00;

	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}
