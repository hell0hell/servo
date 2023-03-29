/*****************************************************************************
Description		: Alarm Traceback Data Clear (Fn006)	
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunAlarmTraceClearBegin();					:Fn006 Alarm Traceback Data Clear Begin
	FunAlarmTraceClearMain();					:Fn006 Alarm Traceback Data Clear Mode Display
	FunAlarmTraceClearEnd();					:Fn006 Alarm Traceback Data Clear End

	-- Locals --
	LpxLcdAlarmTraceClearExe();					:Fn006 Alarm Traceback Data Clear Execution
	LpxPnlAlarmTraceClearExe();					:Fn006 Alarm Traceback Data Clear Execution( Panel Operator)

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.07		created
K.Liu		2013.08.28		Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/

#include "FunAlmHistClr.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register 		*/
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNALMHISTCLR_MODE			(0x1006) 		/* Alarm Traceback Data Clear Mode 				*/
#define FNNORMAL_MODE				(0x0000U)		/* Normal Mode									*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define ALMHISTCLRCMD_EXE 			(0x0001)
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE				0			/* Continue */
#define FUN_END						1			/* End */


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAlarmTraceClearBegin(FUNMSG *Fmsg); 	/* Fn006 Alarm Traceback Data Clear Begin			*/
LONG FunAlarmTraceClearMain(FUNMSG *Fmsg);		/* Fn006 Alarm Traceback Data Clear mode Display	*/
void FunAlarmTraceClearEnd(FUNMSG *Fmsg);		/* Fn006 Alarm Traceback Data Clear End				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdAlarmTraceClearExe(FUNMSG *Fmsg);					/* LCD Operator Fn006 Execution */

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlAlarmTraceClearExe( x ) (1)
#else
LONG LpxPnlAlarmTraceClearExe(FUNMSG *Fmsg);				/* Panel Operator Fn006 Execution 		*/
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg )) /* <S073> */


/*****************************************************************************
  Description: Fn006 Alarm Traceback Data Clear mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAlarmTraceClearMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;

	rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator */
	{
		rc = LpxLcdAlarmTraceClearExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/* Panel Operator */
	{
		rc = LpxPnlAlarmTraceClearExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		/*	Do Nothing	*/
	}
	
	return rc;
}


/*****************************************************************************
  Description: Fn006 Alarm Traceback Data Clear mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAlarmTraceClearBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	FNALMHISTCLRV  *pv = (FNALMHISTCLRV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNALMHISTCLRST_EXE;
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
/*--------------------------------------------------------------------------------------------------*/
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Command)	*/
					OPESET_REG1,
					0,
					&(pv->var.OpeSetIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc == OK )
		{
/*--------------------------------------------------------------------------------------------------*/
/* Set Jog Mode to Mode Register 																	*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNALMHISTCLR_MODE, Hmsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}



/*****************************************************************************
  Description: Fn006 Alarm Traceback Data Clear mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunAlarmTraceClearEnd(FUNMSG *Fmsg)
{
	FNALMHISTCLRV  *pv = (FNALMHISTCLRV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn006 Alarm Traceback Data Clear mode Execution ( LCD Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdAlarmTraceClearExe(FUNMSG *Fmsg)
{
	FNALMHISTCLRV *pv = (FNALMHISTCLRV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 												/* clear Tx buffer		*/
	/* set title	*/
	RpxLcdSetStr(1U, 1U, (UCHAR *)"Alarm History", ResBuf);
	RpxLcdSetStr(2U, 6U, (UCHAR *)"Data Clear", ResBuf);
	RpxLcdSetStr(3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf);
	RpxLcdSetStr(4U, 2U, (UCHAR *)"Return:[SET]", ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		Alarm Traceback Data Clear(Fn006) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNALMHISTCLRST_EXE: 								/* アラーム履歴の消去を実行します	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNALMHISTCLRST_END;
			}
			else if ( OpKeyMake.data == TRUE )		/* Alarm Traceback Data Clear Execution	*/
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, ALMHISTCLRCMD_EXE, Hmsg ) == OK )
				{
				BlinkFlag->done = TRUE;
				}
			}
			else
			{ 											
				/* Do Nothing */
			}
			
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNALMHISTCLRST_END: 												/* 	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunAlarmTraceClearEnd( Fmsg );
			rc = FUN_END;
			break;
		default: 
			break;
		}

	return (rc);
}

/*****************************************************************************
  Description: Fn006 Alarm Traceback Data Clear mode Execution ( Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlAlarmTraceClearExe(FUNMSG *Fmsg)
{
	FNALMHISTCLRV *pv = (FNALMHISTCLRV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*		Alarm Traceback Data Clear(Fn006) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNALMHISTCLRST_EXE: 								/* アラーム履歴の消去を実行します	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNALMHISTCLRST_END;
			}
			else if ( OpKeyMake.mode == TRUE )		/* Alarm Traceback Data Clear Execution	*/
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, ALMHISTCLRCMD_EXE, Hmsg ) == OK )
				{
				BlinkFlag->done = TRUE;
				}
			}
			else
			{
				/* Do Nothing */
			}

			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNALMHISTCLRST_END: 												/* 	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunAlarmTraceClearEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/
	/* パネル表示データ作成	*/
	/*led="trClr"*/
	RpxLedSetStr((UCHAR *)"trClr", DspData, 4U, 5U );

	return (rc);
}
#endif	/* #if  SVFSEL_PANELOP_USE == 1 */


/***************************************** end of file **********************************************/
