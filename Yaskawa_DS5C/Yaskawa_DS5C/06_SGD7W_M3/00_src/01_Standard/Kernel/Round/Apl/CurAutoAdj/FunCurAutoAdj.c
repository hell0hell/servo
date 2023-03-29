/*****************************************************************************
Description		: Fn00E Motor Current Offset Auto Adjustment Mode
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunCurrentAutoAdjBegin();		:Fn00E Motor Current Offset Auto Adjustment Mode Begin
	FunCurrentAutoAdjMain();		:Fn00E Motor Current Offset Auto Adjustment Mode Display
	FunCurrentAutoAdjEnd();			:Fn00E Motor Current Offset Auto Adjustment Mode End

	-- Locals --
	LpxCurrentAutoAdjExe();			:Fn00E Motor Current Offset Auto Adjustment Mode Execution(Digital Operator)
	LpxPnlCurrentAutoAdjExe();		:Fn00E Motor Current Offset Auto Adjustment Mode Execution(Panel Operator)

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.07		created
K.Liu		2013.12.09		Modify
*****************************************************************************/

#include	"FunCurAutoAdj.h"

/* defines */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE				(0x0000U) 		/* Normal Mode */
#define FNCURAUTOADJ_MODE			(0x100EU)		/* Motor Current Offset Auto Adjustment Mode 	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define CURAUTOADJ_EXE 		(0x1)				/*Motor Current Offset Auto Adjustment Mode execute	*/
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			0L					/*Continue				*/
#define FUN_END					1L					/*End					*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg )) /* <S073> */



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunCurrentAutoAdjBegin(FUNMSG *Fmsg);			/* Fn00E Motor Current Offset Auto Adjustment Mode Begin	*/
LONG FunCurrentAutoAdjMain(FUNMSG *Fmsg);			/* Fn00E Motor Current Offset Auto Adjustment Mode Display	*/
void FunCurrentAutoAdjEnd(FUNMSG *Fmsg);			/* Fn00E Motor Current Offset Auto Adjustment Mode End		*/

/* Locals */
static LONG LpxCurrentAutoAdjExe(FUNMSG *Fmsg);		/* Fn00E Motor Current Offset Auto Adjustment Mode Exe		*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlCurrentAutoAdjExe( x ) (1)
#else
LONG LpxPnlCurrentAutoAdjExe(FUNMSG *hMsg);			/* Panel Operator Fn00E Execution 				*/
#endif


/*****************************************************************************
  Description: Fn00E Motor Current Offset Auto Adjustment Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunCurrentAutoAdjMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxCurrentAutoAdjExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )					/* Panel Operator */
	{
		rc =  LpxPnlCurrentAutoAdjExe( Fmsg );
	}
	else
	{
		/*do nothing	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn00E Motor Current Offset Auto Adjustment Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunCurrentAutoAdjBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
FNCURAUTOADJV  *pv = (FNCURAUTOADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNCURAUTOADJ_IDLE;
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
/* Set Motor Current Offset Auto Adjustment Mode to Mode Register 									*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNCURAUTOADJ_MODE, Hmsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &pv->var.DispTime );						/* Panel Operator 					*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn00E Motor Current Offset Auto Adjustment Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunCurrentAutoAdjEnd(FUNMSG *Fmsg)
{
FNCURAUTOADJV  *pv = (FNCURAUTOADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}

/*****************************************************************************
  Description:	Fn00E Motor Current Offset Auto Adjustment Mode Execution(Digital Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxCurrentAutoAdjExe(FUNMSG *Fmsg)
{
FNCURAUTOADJV *pv = (FNCURAUTOADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;										/* TODO: Œã‚ÅÁ‚·		*/
SEQ_CTRL_OUT	*SeqCtrlOut;
BASE_CTRL_HNDL		*BaseControls;

FUN_MANAGER *fnp;
fnp = pAxRsc->FnManager;

SeqCtrlOut = pAxRsc->SeqCtrlOut;
BaseControls = pAxRsc->BaseControls;

/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf ); 								/* clear Tx buffer */
		/* set title */
		RpxLcdSetStr( 1U, 0U, (UCHAR *)"Auto Offset-ADJ", ResBuf );
		RpxLcdSetStr( 2U, 1U, (UCHAR *)"of Motor Current", ResBuf );
		/* Contents		*/
		RpxLcdSetStr( 3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf );
		RpxLcdSetStr( 4U, 2U, (UCHAR *)"Return:[SET]", ResBuf );
/*--------------------------------------------------------------------------------------------------*/
/*		Motor Current Offset Auto Adjustment (Fn00E) Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNCURAUTOADJ_IDLE: 										/* Idle State 				*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNCURAUTOADJ_END;
			}
			else if( OpKeyMake.data == TRUE )
			{
				if( (SeqCtrlOut->MainPowerOn == FALSE)
					|| (SeqCtrlOut->HwbbSts != FALSE)
					|| (SeqCtrlOut->MotStop == FALSE)
					|| (BaseControls->BeComplete == TRUE) )
				{
					BlinkFlag->no_op = TRUE;
				}
				else
				{
				pv->var.State = FNCURAUTOADJ_EXEC;
					FnManagerSto = *fnp;
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, CURAUTOADJ_EXE, Hmsg ) == OK )
					{
						BlinkFlag->done = TRUE;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
			}
			else
			{
				/*do nothing	*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURAUTOADJ_EXEC: 										/* 	Exec State				*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( ( pv->var.ResData )  == FNNORMAL_MODE)
			{
				if(*((UINT *)&OpBlnkFlg) != 0) break;
				*fnp = FnManagerSto;
				pv->var.State = FNCURAUTOADJ_IDLE;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURAUTOADJ_END: 											/* 	End State				*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentAutoAdjEnd( Fmsg );
			fnp->cFunMode[FUN_CH1] = NULL;
			fnp->FunExeState[FUN_CH1].CmdCode = FCMD_NONE;
			fnp->FunExeState[FUN_CH1].State = 0;
			fnp->AxCommon->modeLock = FALSE;
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		if (FNCURAUTOADJ_EXEC == pv->var.State)
		{
			RpxLcdSetBlnk( 1U, 0U, 15U, ResBuf );
			RpxLcdSetBlnk( 2U, 0U, 17U, ResBuf );
		}
		else
		{
			RpxLcdClrLineBlnk( 1U, ResBuf );
			RpxLcdClrLineBlnk( 2U, ResBuf );
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn00E Motor Current Offset Auto Adjustment Mode Execution (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if SVFSEL_PANELOP_USE == 1
static LONG LpxPnlCurrentAutoAdjExe(FUNMSG *Fmsg)
{
FNCURAUTOADJV *pv = (FNCURAUTOADJV *)(Fmsg->pVar);					/* get Variable Pointer */
MSGIF *Hmsg = Fmsg->Super;										/* message interface */
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;											/* 0: continue, 1: end */
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;								/* TODO: Œã‚ÅÁ‚· */
SEQ_CTRL_OUT	*SeqCtrlOut;
BASE_CTRL_HNDL		*BaseControls;

FUN_MANAGER *fnp;
fnp = pAxRsc->FnManager;

SeqCtrlOut = pAxRsc->SeqCtrlOut;
BaseControls = pAxRsc->BaseControls;

/*--------------------------------------------------------------------------------------------------*/
/*		Motor Current Offset Auto Adjustment (Fn00E) Sequence										*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNCURAUTOADJ_IDLE: 										/* Idle State 				*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNCURAUTOADJ_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				if( (SeqCtrlOut->MainPowerOn == FALSE)
					|| (SeqCtrlOut->HwbbSts != FALSE)
					|| (SeqCtrlOut->MotStop == FALSE)
					|| (BaseControls->BeComplete == TRUE) )
				{
					BlinkFlag->no_op = TRUE;
			}
			else
			{
					pv->var.State = FNCURAUTOADJ_EXEC;
					FnManagerSto = *fnp;
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, CURAUTOADJ_EXE, Hmsg ) == OK )
					{
						BlinkFlag->done = TRUE;
					}
					else
					{
						BlinkFlag->error = TRUE;
					}
				}
			}
			else
			{
				/*do nothing	*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURAUTOADJ_EXEC: 										/* 	Exec State				*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( ( pv->var.ResData )  == FNNORMAL_MODE)
			{
				if( *((UINT *)&OpBlnkFlg ) != 0) break;
				*fnp = FnManagerSto;
				pv->var.State = FNCURAUTOADJ_IDLE;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURAUTOADJ_END: 											/* 	End State				*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentAutoAdjEnd( Fmsg );
			fnp->cFunMode[FUN_CH1] = NULL;
			fnp->FunExeState[FUN_CH1].CmdCode = FCMD_NONE;
			fnp->FunExeState[FUN_CH1].State = 0;
			fnp->AxCommon->modeLock = FALSE;
			rc = FUN_END;
			break;
		default:
			break;
		}

		/* ALL Clear	*/
		MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

		RpxLedSetStr((UCHAR *)"Cur_o", DspData, 4U, 5U);
		/* Hard-wired Baseblock Display <V068> */
		if( CHK_HBB() /* CoutV.f.Sv.HwbbState */ )
		{
			if(((KlibGetLongTimerMs(pv->var.DispTime)/100) & 0x04) == 0 )
			{
				RpxLedSetStr((UCHAR *)". Hbb", DspData, 4U, 5U);
			}
		}

	return (rc);
}

#endif
/***************************************** end of file **********************************************/

