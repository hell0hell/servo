/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PrmFileDownload.c : Parameter File Download Sample Source									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Parameter File Upload (0x200F)														*/
/*			2)Parameter File Download 																*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.09.26  K.Ozaki	<S00C>														*/
/*		Rev.1.10 : 2013.10.24  K.Ozaki	<S031>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"					/* Kernel API */
#include "LcdOp.h"					/* LCD Operator */
#include "Monitor.h"				/* Un Monitor */
#include "Function.h"				/* Fn Function */
#include "MercuryGlobals.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "DataTraceManager.h"		/* @@ */
#include "Memory.h"					/* <S031> */


/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _FnPrmDownloadV{
	int		Reserved[(256/4)];	/* Reserved: All functions can use ram variables at least 256 bytes. */
} FNPRMDLDV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static USHORT *LpxGetTrcBufAddr( void *hUsr );							/* <S031> */
static void LpxFreeTrcBufAddr( void *hUsr );							/* <S031> */
static LONG LpxServoPrmFileRwChkCmd(FUNEXE *FunExe, PRM_RSLT *rc);

/****************************************************************************************************/
/*																									*/
/*		Register Access Function																	*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/* 0x200F: Entry of Servo Parameter Upload for Register IF											*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegServoPrmFileReadBegin(FUNEXE *FunExe, void *Axis)
{
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;

	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)						/* check servo on state */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	if( FnCmnCtrl->f.PrmCopy /* <S031>: check parameter copy state */ )
	{
		return PRM_RSLT_NOACCRIGHT_ERR;
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/* 0x200F: Execution of Servo Parameter Upload for Register IF										*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegServoPrmFileReadExe(FUNEXE *FunExe, void *Axis)
{
PRM_RSLT rc = PRM_RSLT_CONTINUE;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
//FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;
/*--------------------------------------------------------------------------------------------------*/
	switch ( FunExe->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:																/* no operation */
/*--------------------------------------------------------------------------------------------------*/
		if( LpxServoPrmFileRwChkCmd( FunExe, &rc ) )
		{
			if( KriRequestReadAllEeprom( pAxRsc->pRwAllMng, LpxGetTrcBufAddr( Axis /* <S031> TODO: */ ) ) == OK )
			{
				FunExe->State = 1;
				rc = PRM_RSLT_CONTINUE;
			}
			else
			{
				LpxFreeTrcBufAddr( Axis  );							/* <S031> TODO */
				FunExe->State = 2;
				rc = PRM_RSLT_CONDITION_ERR;						/* TODO: エラー番号確認 */
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:			/* execution state */
/*--------------------------------------------------------------------------------------------------*/
		if( KriChkDoneReadAllEeprom( pAxRsc->pRwAllMng ) == FALSE )
		{
			rc = PRM_RSLT_CONTINUE;
		}
		else
		{
			LpxFreeTrcBufAddr( Axis );								/* <S031> TODO */
			FunExe->State = 0;
			rc = PRM_RSLT_SUCCESS;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:			/* error state */
/*--------------------------------------------------------------------------------------------------*/
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}



/****************************************************************************************************/
/*																									*/
/* 0x200F: End of Servo Parameter Upload for Register IF											*/
/*																									*/
/****************************************************************************************************/
void RegServoPrmFileReadEnd(FUNEXE *FunExe, void *Axis)
{
//AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
//FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;
	/* 終了処理	*/
	LpxFreeTrcBufAddr( Axis );					/* <S031> TODO */
}


/****************************************************************************************************/
/*																									*/
/* 0x2010: Entry of Servo Parameter Download for Register IF										*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegServoPrmFileWriteBegin(FUNEXE *FunExe, void *Axis)
{
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;

	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)						/* check servo on state */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	if( FnCmnCtrl->f.PrmCopy /* <S031>: check parameter copy state */ )
	{
		return PRM_RSLT_NOACCRIGHT_ERR;
	}

	FnCmnCtrl->KnlMode = KPI_KMD_PARAMCPY; 								/* <S031> Executing Parameter Copy */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*																									*/
/* 0x2010: Execution of Servo Parameter Download for Register IF									*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT RegServoPrmFileWriteExe(FUNEXE *FunExe, void *Axis)
{
PRM_RSLT rc = PRM_RSLT_CONTINUE;
AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
//FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;
/*--------------------------------------------------------------------------------------------------*/
	switch ( FunExe->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:																/* no operation */
/*--------------------------------------------------------------------------------------------------*/
		if( LpxServoPrmFileRwChkCmd( FunExe, &rc ) )
		{
			if( KriRequestWriteAllEeprom( pAxRsc->pRwAllMng, LpxGetTrcBufAddr( Axis /* <S031> TODO */ ) ) == OK )
			{
				FunExe->State = 1;
				rc = PRM_RSLT_CONTINUE;
			}
			else
			{
				LpxFreeTrcBufAddr( Axis );							/* <S031> TODO */
				FunExe->State = 2;
				rc = PRM_RSLT_CONDITION_ERR;						/* TODO: エラー番号確認 */
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:			/* execution state */
/*--------------------------------------------------------------------------------------------------*/
		if( KriChkDoneWriteAllEeprom( pAxRsc->pRwAllMng ) == FALSE )
		{
			rc = PRM_RSLT_CONTINUE;
		}
		else
		{
			LpxFreeTrcBufAddr( Axis );								/* <S031> TODO */
			FunExe->State = 0;
			ALMSetServoAlarm( pAxRsc->AlmManager, WRN_BOOT );		/* <S0AA> A.941 */
			rc = PRM_RSLT_SUCCESS;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:			/* error state */
/*--------------------------------------------------------------------------------------------------*/
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/* 0x2010: End of Servo Parameter Download for Register IF											*/
/*																									*/
/****************************************************************************************************/
void RegServoPrmFileWriteEnd(FUNEXE *FunExe, void *Axis)
{
//AXIS_HANDLE *pAxRsc = (AXIS_HANDLE *)Axis;
//FUN_CMN_CONTROL *FnCmnCtrl = pAxRsc->FnCmnCtrl;
	/* 終了処理	*/
	LpxFreeTrcBufAddr( Axis );						/* <S031> TODO */
}


/****************************************************************************************************/
/*																									*/
/* 		get trace buffer																			*/
/*																									*/
/****************************************************************************************************/
USHORT *LpxGetTrcBufAddr( void *hUsr )
{
	return CTRCMEM->GetBuffer( CTRCMEM, hUsr );					/* <S031> get buffer */
#if 0
	if( pAxRsc->TrcHndl->TrcExeV.TrcCmd == TRCCMD_NOCMD )
	{
		return (USHORT *)(pAxRsc->TraceBuffer);
	}
	else
	{
		return NULL;
	}
#endif
}

/****************************************************************************************************/
/*																									*/
/* 		free trace buffer																			*/
/*																									*/
/****************************************************************************************************/
void LpxFreeTrcBufAddr( void *hUsr )
{
	CTRCMEM->FreeBuffer( CTRCMEM, hUsr );
}

/****************************************************************************************************/
/*																									*/
/*		Check Parameter File Read/Write Command														*/
/*																									*/
/****************************************************************************************************/
LONG LpxServoPrmFileRwChkCmd(FUNEXE *FunExe, PRM_RSLT *rc)
{
LONG rv = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	switch ( FunExe->CmdCode )
	{
	/*----------------------------------------------------------------------------------------------*/
	case FCMD_EXEC:											/* execution */
	/*----------------------------------------------------------------------------------------------*/
		/* TODO: */
		*rc = PRM_RSLT_SUCCESS;
		rv = TRUE;
		break;
	/*----------------------------------------------------------------------------------------------*/
	case FCMD_UP:											/* UP,正転,正方向		*/
	case FCMD_DOWN:											/* DOWN,逆転,逆方向		*/
	case FCMD_SV:											/* サーボオン/オフ		*/
	case FCMD_CH1:											/* CH1,U相選択			*/
	case FCMD_CH2:											/* CH2,V相選択			*/
	case FCMD_EWRT:											/* EEPROM書込み処理		*/
	case FCMD_MTUP:											/* 操作モード維持(正転)	*/
	case FCMD_MTDOWN:										/* 操作モード維持(逆転)	*/
	/*----------------------------------------------------------------------------------------------*/
		*rc = PRM_RSLT_CONDITION_ERR;						/* error */
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:												/* other command */
	/*----------------------------------------------------------------------------------------------*/
		*rc = PRM_RSLT_CONTINUE;						/* error */
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rv);
}


