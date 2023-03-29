/*****************************************************************************
Description		:Fn005 ParamInit Mode 
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunUserParamInitBegin();					:Fn005 ParamInit Mode Begin
	FunUserParamInitMain();						:Fn005 ParamInit Mode Display
	FunUserParamInitEnd();						:Fn005 ParamInit Mode End

	-- Locals --
	LpxUserParamInitExe();						:Fn005 ParamInit Mode Execution(Digital Operator)
	LpxPnlUserParamInitExe();					:Fn005 ParamInit Mode Execution (Panel Operator)

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.07		created
LIU.K		2013.12.09		Modify
*****************************************************************************/

#include	"FunPrmInit.h"

/* defines */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE		(0x0000U) 		/* Normal Mode		*/
#define FNPRMINIT_MODE		(0x1005U) 		/* ParamInit Mode	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PRMINITCMD_EXE 		(0x1)			/*ParamInit Mode execute	*/
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			0L				/*Continue				*/
#define FUN_END					1L				/*End					*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))  /* <S073> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunUserParamInitBegin(FUNMSG *Fmsg);				/* Fn005 ParamInit Mode Begin				*/
LONG FunUserParamInitMain(FUNMSG *Fmsg);				/* Fn005 ParamInit Mode Display				*/
void FunUserParamInitEnd(FUNMSG *Fmsg);					/* Fn005 ParamInit Mode End					*/

/* Locals */
static LONG LpxUserParamInitExe(FUNMSG *Fmsg);			/* Fn005 ParamInit Mode Execution(Digital Operator)			*/
//static LONG LpxPnlUserParamInitExe(FUNMSG *Fmsg);		/* Fn005 ParamInit Mode Execution (Panel Operator)			*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlUserParamInitExe( x ) (1)
#else
LONG LpxPnlUserParamInitExe(FUNMSG *hMsg);					/* Fn005 ParamInit Mode Execution (Panel Operator)		*/
#endif


/*****************************************************************************
  Description: Fn005 ParamInit Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunUserParamInitMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxUserParamInitExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )					/* Panel Operator */
	{
		rc = LpxPnlUserParamInitExe( Fmsg );
	}
	else
	{
		/*do nothing	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn005 ParamInit Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunUserParamInitBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
FNPRMINITV  *pv = (FNPRMINITV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNPRMINIT_IDLE;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->var.OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc == OK )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Command)	*/
				OPESET_REG1,
				0,
				&(pv->var.OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
		if (rc == OK)
		{
/*--------------------------------------------------------------------------------------------------*/
/* Set ParamInit Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNPRMINIT_MODE, Hmsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn005 ParamInit Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunUserParamInitEnd(FUNMSG *Fmsg)
{
FNPRMINITV  *pv = (FNPRMINITV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}

/*****************************************************************************
  Description:	Fn005 ParamInit Mode Execution (Digital Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxUserParamInitExe(FUNMSG *Fmsg)
{
FNPRMINITV *pv = (FNPRMINITV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: Œã‚ÅÁ‚·		*/
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf ); 									/* clear Tx buffer	*/
		RpxLcdSetStr( 1U, 1U, (UCHAR *)"Parameter Init", ResBuf );	/* set title 		*/
		RpxLcdSetStr( 2U, 2U, (UCHAR *)"Start :[DATA]", ResBuf );	/* Contents			*/
		RpxLcdSetStr( 3U, 2U, (UCHAR *)"Return:[SET]", ResBuf );	/* Contents			*/
/*--------------------------------------------------------------------------------------------------*/
/*		ParamInit (Fn005) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNPRMINIT_IDLE: 										/* Idle State	 				*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNPRMINIT_END;
			}
			else if( OpKeyMake.data == TRUE )
			{
				if ( pAxRsc->FnCmnCtrl->f.PrmCopy != TRUE)
				{
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, PRMINITCMD_EXE, Hmsg ) == OK )
					{
						pv->var.State = FNPRMINIT_EXEC;
					}
				}
			}
			else
			{
				/*do nothing	*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPRMINIT_EXEC: 									/* 	Exec	State					*/
/*--------------------------------------------------------------------------------------------------*/
//			BlinkFlag->done = TRUE;
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( (pv->var.ResData)  == FNNORMAL_MODE)
			{
				BlinkFlag->done = TRUE;
				pv->var.State = FNPRMINIT_IDLE;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPRMINIT_END: 									/*  End		State					*/
/*--------------------------------------------------------------------------------------------------*/
			FunUserParamInitEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		if (FNPRMINIT_EXEC == pv->var.State)
		{
			RpxLcdSetBlnk( 1U, 1U, 14U, ResBuf );
		}
		else
		{
			RpxLcdClrLineBlnk( 1U, ResBuf );
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn005 ParamInit Mode Execution (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/

#if SVFSEL_PANELOP_USE == 1
static LONG LpxPnlUserParamInitExe(FUNMSG *Fmsg)
{
FNPRMINITV *pv = (FNPRMINITV *)(Fmsg->pVar);					/* get Variable Pointer */
MSGIF *Hmsg = Fmsg->Super;										/* message interface */
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
LONG rc = FUN_CONTINUE;											/* 0: continue, 1: end */
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: Œã‚ÅÁ‚·		*/

/*--------------------------------------------------------------------------------------------------*/
/*		ParamInit (Fn005) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
	/*--------------------------------------------------------------------------------------------------*/
	case FNPRMINIT_IDLE: 										/* Idle State	 				*/
	/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMake.data == TRUE )
		{
			pv->var.State = FNPRMINIT_END;
		}
		else if( OpKeyMake.mode == TRUE )
		{
			if ( pAxRsc->FnCmnCtrl->f.PrmCopy != TRUE)
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, PRMINITCMD_EXE, Hmsg ) == OK )
				{
					pv->var.State = FNPRMINIT_EXEC;
				}
			}
		}
		else
		{
			/*do nothing	*/
		}
//		RpxLedSetStr((UCHAR *)"P.InIt", DspData, 4U, 6U);
		break;
	/*--------------------------------------------------------------------------------------------------*/
	case FNPRMINIT_EXEC: 											/* 	Exec	State					*/
	/*--------------------------------------------------------------------------------------------------*/
//		BlinkFlag->done = TRUE;
		STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
		if( (pv->var.ResData)  == FNNORMAL_MODE)
		{
			BlinkFlag->done = TRUE;
			pv->var.State = FNPRMINIT_IDLE;
		}
		break;
	/*--------------------------------------------------------------------------------------------------*/
	case FNPRMINIT_END: 											/*  End		State					*/
	/*--------------------------------------------------------------------------------------------------*/
		FunUserParamInitEnd( Fmsg );
		rc = FUN_END;
		break;
		default:
		break;
	}

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	RpxLedSetStr((UCHAR *)"P.InIt", DspData, 4U, 6U);

	return (rc);
}


#endif



/***************************************** end of file **********************************************/

