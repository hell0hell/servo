/****************************************************************************
  Description   : Callback functions for Network ID Register
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corp., Copyright (c) 2010
                  All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-10-20

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "NetDriver.h"



/* ---------------------------------------------------------------------------------------------- */
/* Get the serial number by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)((ArrayIdx < 16) ?
				(pAxis->ModuleID->IfInfo)->SerialNumber[ArrayIdx] : 0);
	return PRM_RSLT_SUCCESS;	
}


/* ---------------------------------------------------------------------------------------------- */
/* Get the transmission cycle by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadTByteSetting(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)
{
	ND_PARAM* ndprm;

	ndprm = NdGetNdParamPtr();
	switch(ndprm->DataSize)
	{
	case  8: *pValue = 0x00000001; break;
	case 16: *pValue = 0x00000002; break;
	case 32: *pValue = 0x00000004; break;
	case 48: *pValue = 0x00000008; break;
	case 64: *pValue = 0x00000010; break;
	default: *pValue = 0x00000000; break;
	}
	return PRM_RSLT_SUCCESS;	

}

/* ---------------------------------------------------------------------------------------------- */
/* Get the ProductName by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)( (ArrayIdx < 32)?/* ArrayIdx‚ª32ˆÈã‚É‚È‚é‚±‚Æ‚Í‚È‚¢‚ªB*/
					(pAxis->ModuleID->IfInfo)->ProductName[ArrayIdx] : 0 );
	return PRM_RSLT_SUCCESS;	
}

/* ---------------------------------------------------------------------------------------------- */
/* Get the model of the motor by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadMotModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)( (ArrayIdx < MOT_ENC_MODEL_STR)?
					(pAxis->MencV)->MotInfo.Model[ArrayIdx] : 0 );
	return PRM_RSLT_SUCCESS;	
}


/* Get the software version of the encoder by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadEncVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S224> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)(pAxis->UniPrms->Prm)->MencP.encver;
	return PRM_RSLT_SUCCESS;	
}

/* ---------------------------------------------------------------------------------------------- */
/* Get the model of the exterior encoder by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadExEncModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)( (ArrayIdx < MOT_ENC_MODEL_STR)?
					(pAxis->FencV)->EncInfo.Model[ArrayIdx] : 0 );
	return PRM_RSLT_SUCCESS;
}


/* Get the software version of the exterior encoder by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadExEncVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)(pAxis->OptCardHdl)->FbkOpt.conf.EncVer;
	return PRM_RSLT_SUCCESS;	
}

/* ---------------------------------------------------------------------------------------------- */
/* Get the model of the SaftyOption by M3 command 'ID_RD' */
PRM_RSLT NetworkIdSOptionModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = 0;
	return PRM_RSLT_SUCCESS;
}


/* Get the software version of the SaftyOption by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadSOptionVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = 0;
	return PRM_RSLT_SUCCESS;	
}


/* ---------------------------------------------------------------------------------------------- */
/* Get the model of the Feedback Option by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadFbOptionModel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)( (ArrayIdx < 24)?
					(pAxis->OptCardHdl)->FbkOpt.conf.OpInfo.ModelStr[ArrayIdx]: 0 );
	return PRM_RSLT_SUCCESS;
}


/* Get the software version of the Feedback Option by M3 command 'ID_RD' */
PRM_RSLT NetworkIdReadFbOptionVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue)/* <S**1> */
{
	AXIS_HANDLE	*pAxis;

	pAxis = (AXIS_HANDLE*)(Axis);
	*pValue = (LONG)(pAxis->OptCardHdl)->FbkOpt.conf.OpInfo.SoftVer;
	return PRM_RSLT_SUCCESS;	
}

/******************************* End of File *******************************/
