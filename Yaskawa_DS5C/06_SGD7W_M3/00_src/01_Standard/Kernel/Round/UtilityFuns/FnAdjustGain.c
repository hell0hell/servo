/****************************************************************************
  Description   : ���������@�@A/D �Q�C���������[�h�iFn3001, Fn3002,Fn3003�j
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


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/


/****************************************************************************/
/*		PROTOTYPE															*/
/****************************************************************************/


/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************
 Fn3001: Entry function of Adjust Speed Ref.A/D Gain
****************************************************************************/
PRM_RSLT FnEnterAdjustVrefGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{

	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
//	FnCmnCtrl->KnlMode = KPI_KMD_ADJGAIN;		/* A/D �Q�C��������  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S142> */
//	FunExe->HoldFnMode = TRUE;					/* Fn���[�h�����I���Ȃ� */	/* <S0EB> */
	rc = PRM_RSLT_SUCCESS;
#else											/* Analog IF �ȊO */
	rc = PRM_RSLT_SUCCESS;
#endif
	return (rc);
}

/****************************************************************************
 Fn3001: Execute function of Adjust Speed Ref.A/D Gain
****************************************************************************/
PRM_RSLT FnExecuteAdjustVrefGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT				rc;
	CMN_PRMDATA				*Cprm;
	PRMDATA					*Prm;
	FUN_CMN_CONTROL			*FnCmnCtrl;
	LONG					Adj_Result;

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		rc = PRM_RSLT_CONTINUE;
	}
	else
	{
#if( SVFSEL_ADC_USE==1 )       /* <S19A> */
		/* ���s�v����������A�u���x�w�߂`�c�Q�C�������v�����s */
		Adj_Result = TciAdjustVrefGain( Axis );
#endif
		FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S0EB> */

		if( Adj_Result == KPI_ADC_ADJOK )
		{
			rc = PRM_RSLT_SUCCESS;			/* ����I�� */
		}
		else
		{
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
		}
	}
#else											/* Analog IF �ȊO */
	rc = PRM_RSLT_SUCCESS;
#endif
	return(rc);

}

#if 0	/* <S0EB> Delete:�����Œ����@�\���I��������ׁA�{�������R�����g�A�E�g����B */
/****************************************************************************
 Fn3001: End function of Adjust Speed Ref.A/D Gain
****************************************************************************/
void FnLeaveAdjustVrefGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */
#endif
}
#endif	/* <S0EB> Delete:�����Œ����@�\���I��������ׁA�{�������R�����g�A�E�g����B */

/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************
 Fn3002: Entry function of Adjust Torque Positive Ref.A/D Gain
****************************************************************************/
PRM_RSLT FnEnterAdjustTrefPGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
//	FnCmnCtrl->KnlMode = KPI_KMD_ADJGAIN;		/* A/D �Q�C��������  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S142> */
//	FunExe->HoldFnMode = TRUE;					/* Fn���[�h�����I���Ȃ� */	/* <S0EB> */
	rc = PRM_RSLT_SUCCESS;
#else											/* Analog IF �ȊO */
	rc = PRM_RSLT_SUCCESS;
#endif
	return (rc);
}

/****************************************************************************
 Fn3002: Execute function of Adjust Torque Positive Ref.A/D Gain
****************************************************************************/
PRM_RSLT FnExecuteAdjustTrefPGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT				rc;
	CMN_PRMDATA				*Cprm;
	PRMDATA					*Prm;
	FUN_CMN_CONTROL			*FnCmnCtrl;
	LONG					Adj_Result;

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		rc = PRM_RSLT_CONTINUE;
	}
	else
	{
		/* ���s�v����������A�u�g���N�w�߂`�c�����Q�C�������v�����s */
#if( SVFSEL_ADC_USE==1 )     /* <S19A> */
		Adj_Result = TciAdjustTrefPGain( Axis );
#endif
		FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S0EB> */

		if( Adj_Result == KPI_ADC_ADJOK )
		{
			rc = PRM_RSLT_SUCCESS;			/* ����I�� */
		}
		else
		{
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
		}
	}
#else											/* Analog IF �ȊO */
	rc = PRM_RSLT_SUCCESS;
#endif
	return(rc);

}

#if 0	/* <S0EB> Delete:�����Œ����@�\���I��������ׁA�{�������R�����g�A�E�g����B */
/****************************************************************************
 Fn3002: End function of Adjust Torque Positive Ref.A/D Gain
****************************************************************************/
void FnLeaveAdjustTrefPGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */
#endif
}
#endif	/* <S0EB> Delete:�����Œ����@�\���I��������ׁA�{�������R�����g�A�E�g����B */
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************
 Fn3003: Entry function of Adjust Torque Negative Ref.A/D Gain
****************************************************************************/
PRM_RSLT FnEnterAdjustTrefNGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT rc;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */

//	FnCmnCtrl->KnlMode = KPI_KMD_ADJGAIN;		/* A/D �Q�C��������  */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S142> */
//	FunExe->HoldFnMode = TRUE;					/* Fn���[�h�����I���Ȃ� */	/* <S0EB> */
	rc = PRM_RSLT_SUCCESS;
#else											/* Analog IF �ȊO */
	rc = PRM_RSLT_SUCCESS;
#endif

	return (rc);
}

/****************************************************************************
 Fn3003: Execute function of Adjust Torque Negative Ref.A/D Gain
****************************************************************************/
PRM_RSLT FnExecuteAdjustTrefNGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT				rc;
	CMN_PRMDATA				*Cprm;
	PRMDATA					*Prm;
	FUN_CMN_CONTROL			*FnCmnCtrl;
	LONG					Adj_Result;

	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;
	FnCmnCtrl = Axis->FnCmnCtrl;
	rc = PRM_RSLT_SUCCESS;

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
	/* ���s�v��������܂ŁA�҂�	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		rc = PRM_RSLT_CONTINUE;
	}
	else
	{
		/* ���s�v����������A�u�g���N�w�߂`�c�����Q�C�������v�����s */
#if( SVFSEL_ADC_USE==1 )      /* <S19A> */
		Adj_Result = TciAdjustTrefNGain( Axis );
#endif
		FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */	/* <S0EB> */

		if( Adj_Result == KPI_ADC_ADJOK )
		{
			rc = PRM_RSLT_SUCCESS;			/* ����I�� */
		}
		else
		{
			rc = PRM_RSLT_CONDITION_ERR;	/* �����G���[ */
		}
	}
#else										/* Analog IF �ȊO */
	rc = PRM_RSLT_SUCCESS;
#endif
	return(rc);

}

#if 0	/* <S0EB> Delete:�����Œ����@�\���I��������ׁA�{�������R�����g�A�E�g����B */
/****************************************************************************
 Fn3003: End function of Adjust Torque Negative Ref.A/D Gain
****************************************************************************/
void FnLeaveAdjustTrefNGain(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)		/* Analog IF */
	/* �I������	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;		/* normal mode */
#endif
}
#endif	/* <S0EB> Delete:�����Œ����@�\���I��������ׁA�{�������R�����g�A�E�g����B */
