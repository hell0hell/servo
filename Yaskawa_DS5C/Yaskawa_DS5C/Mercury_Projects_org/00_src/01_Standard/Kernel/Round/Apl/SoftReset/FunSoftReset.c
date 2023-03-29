/*****************************************************************************
Description		:Fn030 Soft Reset Mode 
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunSftRstBegin();						:Fn030 Soft Reset Mode Begin
	FunSftRstMain();						:Fn030 Soft Reset Mode Display
	FunSftRstEnd();							:Fn030 Soft Reset Mode End
	-- Locals --
	LpxSftRstExe();							:Fn030 Soft Reset Mode Execution(Digital Operator)
	LpxPnlSftRstExe();						:Fn030 Soft Reset Mode Execution(Panel Operator)

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.07		created
K.Liu		2013.12.09		Modify
*****************************************************************************/

#include	"FunSoftReset.h"		/* Kernel API */
/* defines */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE		(0x0000U) 		/* Normal Mode */
#define FNSOFTRESET_MODE	(0x2006U)		/*Soft Reset Mode  Mode	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define SOFTRESET_EXE 		(0x1)
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			0L				/*Continue				*/
#define FUN_END					1L				/*End					*/
#define MTNOPNUM_MAX		5U				/*MAX  Number			*/
#define MTNOPNUM_MIN		1U				/*MIN  Number			*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunSftRstBegin(FUNMSG *Fmsg);				/* Fn030 Soft Reset Mode Begin						*/
LONG FunSftRstMain(FUNMSG *Fmsg);				/* Fn030 Soft Reset Mode Display					*/
void FunSftRstEnd(FUNMSG *Fmsg);				/* Fn030 Soft Reset Mode End						*/

/* Locals */
static LONG LpxSftRstExe(FUNMSG *Fmsg);			/* Fn030 Soft Reset Mode Execution					*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlSftRstExe( x ) (1)
#else
LONG LpxPnlSftRstExe(FUNMSG *hMsg);					/* Panel Operator Fn030 Execution 				*/
#endif

/*****************************************************************************
  Description: Fn030 Soft Reset Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunSftRstMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
	/* ここでは１軸目(0)固定 *//* <S19D> */
	hMsg->pAxRsc = (AXIS_HANDLE*)GetAxisHandle(0);
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxSftRstExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc = LpxPnlSftRstExe( Fmsg );
	}
	else
	{
		/*do nothing	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn030 Soft Reset Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg			Operator Function Message
  Return:
  	LONG			OK: Soft Reset Success, Other: Failure
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunSftRstBegin(FUNMSG *Fmsg)
{
LONG	no;		/* <S1CC> */
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNSOFTRESETV  *pv = (FNSOFTRESETV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/* ここでは１軸目(0)固定 *//* <S19D> */
//AXIS_HANDLE	*pAxRsc = Hmsg->pAxRsc;							/* <S166> 								*/
AXIS_HANDLE	*pAxRsc;
	pAxRsc = (AXIS_HANDLE*)GetAxisHandle(0);
	Hmsg->pAxRsc = pAxRsc;	/* <S19D> */
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNSOFTRESET_INIT;
/*--------------------------------------------------------------------------------------------------*/
/*	<S166><S1CC> Check Base Enable																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* 1軸目 or 2軸目サーボオン中の場合は入らずに終了	*/ /* <S1CC> */
	for(no = 0; no < NumberOfAxes; no++)
	{
		pAxRsc = (AXIS_HANDLE*)GetAxisHandle(no);
		if( ! CHK_BB() ) { return (NG) ; }
	}
	/* ポインタを1軸目へ戻す */
	pAxRsc = (AXIS_HANDLE*)GetAxisHandle(0);
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->var.OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK )
	{
		ResFlag = FALSE;
	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Command)	*/
				OPESET_REG1,
				0,
				&(pv->var.OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	/*--------------------------------------------------------------------------------------------------*/
	/* Set Soft Reset Mode to Mode Register 															*/
	/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
//<S166>		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNSOFTRESET_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn030 Soft Reset Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunSftRstEnd(FUNMSG *Fmsg)
{
FNSOFTRESETV  *pv = (FNSOFTRESETV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
	/* ここでは１軸目(0)固定 *//* <S19D> */
	Hmsg->pAxRsc = (AXIS_HANDLE*)GetAxisHandle(0);
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}

/*****************************************************************************
  Description:	Fn030 Soft Reset Mode Execution(Digital Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxSftRstExe(FUNMSG *Fmsg)
{
FNSOFTRESETV *pv = (FNSOFTRESETV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
		RpxLcdClrTxBuf( ResBuf ); 								/* clear Tx buffer */
		/* set title */
		RpxLcdSetStr(2U, 1U, (UCHAR *)"Software Reset", ResBuf);
		/* Contents		*/
		RpxLcdSetStr(4U, 5U, (UCHAR *)"RESET", ResBuf);
		RpxLcdSetDecPnt(4U, 10U, pv->var.SelNo, 1U, (ULONG)FALSE, 0, ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		Soft Reset Mode Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNSOFTRESET_INIT: 										/* Initial State 				*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.SelNo = MTNOPNUM_MIN;
			pv->var.State = FNSOFTRESET_EXEC;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNSOFTRESET_EXEC: 									/* 	Exec	State					*/
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNSOFTRESET_END;
			}
			else if (OpKeyMake.up == TRUE )
			{
				if (pv->var.SelNo < MTNOPNUM_MAX)
				{
					pv->var.SelNo ++;
				}
			}
			else if ( OpKeyMake.data == TRUE )
			{
				if ( pv->var.SelNo != MTNOPNUM_MAX )
				{
					BlinkFlag->no_op = TRUE;
				}
				else
				{
//<S166>			if ( OPEREG_WRITE( pv->var.OpeSetIdx, SOFTRESET_EXE, Hmsg ) == OK )
					if ( ( OPEREG_WRITE( pv->var.OpeModeIdx, FNSOFTRESET_MODE, Hmsg ) == OK ) &&
						( OPEREG_WRITE( pv->var.OpeSetIdx, SOFTRESET_EXE, Hmsg ) == OK ) )		/* <S166> */
					{
						pv->var.State = FNSOFTRESET_END;
					}
					else
					{
						BlinkFlag->no_op = TRUE;
					}
				}
			}
			else
			{
					/*do nothing		*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNSOFTRESET_END: 									/*  End		State					*/
/*--------------------------------------------------------------------------------------------------*/
			FunSftRstEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn030 Soft Reset Mode Execution (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/

#if SVFSEL_PANELOP_USE == 1
static LONG LpxPnlSftRstExe(FUNMSG *Fmsg)
{
FNSOFTRESETV *pv = (FNSOFTRESETV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
/*		Soft Reset Mode Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNSOFTRESET_INIT: 										/* Initial State 				*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.SelNo = MTNOPNUM_MIN;
			pv->var.State = FNSOFTRESET_EXEC;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNSOFTRESET_EXEC: 									/* 	Exec	State					*/
/*--------------------------------------------------------------------------------------------------*/
			if (OpKeyMake.data == TRUE )
			{
				pv->var.State = FNSOFTRESET_END;
			}
			else if (OpKeyMake.up == TRUE )
			{
				if (pv->var.SelNo < MTNOPNUM_MAX)
				{
					pv->var.SelNo ++;
				}
			}
			else if ( OpKeyMake.mode == TRUE  )
			{
				if ( pv->var.SelNo != MTNOPNUM_MAX )
				{
					BlinkFlag->no_op = TRUE;
				}
				else
				{
					if ( OPEREG_WRITE( pv->var.OpeSetIdx, SOFTRESET_EXE, Hmsg ) == OK )
					{
						pv->var.State = FNSOFTRESET_END;
					}
					else
					{
						BlinkFlag->no_op = TRUE;
					}
				}
			}
			else
			{
					/*do nothing		*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNSOFTRESET_END: 									/*  End		State					*/
/*--------------------------------------------------------------------------------------------------*/
			FunSftRstEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	RpxLedSetStr((UCHAR *)"SrSt", DspData, 4U, 4U);
	RpxLedSetValDec((USHORT)pv->var.SelNo, DspData, 0U, 1U, FALSE, 0U);

	return (rc);
}
#endif
/***************************************** end of file **********************************************/

