/*****************************************************************************
Description		: Origin Point Search
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunOriginSearchMain();					:Fn003 Origin Point Search mode Display
	FunOriginSearchBegin();					:Fn003 Origin Point Search mode Begin
	FunOriginSearchEnd();					:Fn003 Origin Point Search mode End

	-- Locals --
	LpxLcdOriginSearchExe();				:LCD Operator Fn003 Execution
	LpxPnlOriginSearchExe();				:PANEL Operator Fn003 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.07		created

*****************************************************************************/

#include	"FunZSearch.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define OPEZSEARCH_REG1			(0x030)			/* ZSearch end */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000U)		/* Normal Mode	*/
#define FNZSEARCH_MODE			(0x1003U) 		/* Origin Point Search Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define ZSEARCHCMD_STOP 		(0x0)			/* Stop */
#define ZSEARCHSTS_END			(0x1)			/* End */
#define ZSEARCHCMD_FORWARD 		(0x2)			/* Forward */
#define ZSEARCHCMD_REVERSE 		(0x3)			/* Reverse */
#define ZSEARCHCMD_SVONOFF 		(0x4)			/* On And Off Replace*/

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )    (COPEREG->Read( COPEREG, (idx), (pResData), \
																	(CheckType), hmsg ))  /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunOriginSearchBegin(FUNMSG *Fmsg); 				/* Fn003 Origin Point Search Mode Begin		*/
LONG FunOriginSearchMain(FUNMSG *Fmsg);					/* Fn003 Origin Point Search Mode Main		*/
void FunOriginSearchEnd(FUNMSG *Fmsg);					/* Fn003 Origin Point Search Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdOriginSearchExe(FUNMSG *Fmsg);		/* LCD Operator Fn003 Execution 			*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlOriginSearchExe( x ) (1)
#else
static LONG LpxPnlOriginSearchExe(FUNMSG *hMsg);		/* Panel Operator Fn003 Execution 			*/
#endif

/*****************************************************************************
  Description: Fn003 ZSearch Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunOriginSearchMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdOriginSearchExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlOriginSearchExe( Fmsg );
	}
/*--------------------------------------------------------------------------*/
	else
	{
		/*	Do Nothing	*/
	}
/*--------------------------------------------------------------------------*/
	return rc;
}


/*****************************************************************************
  Description: Fn003 ZSearch Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunOriginSearchBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNZSEARCHV  *pv = (FNZSEARCHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNZSEARCHST_INIT;
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
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
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
	if ( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Read)		*/
					OPEZSEARCH_REG1,
					0,
					&(pv->var.OpeReadIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set ZSearch Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNZSEARCH_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &( pv->var.DispTime ) );						/* Panel Operator				*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn003 ZSearch Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunOriginSearchEnd(FUNMSG *Fmsg)
{
FNZSEARCHV  *pv = (FNZSEARCHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: LCD Operator Fn003 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdOriginSearchExe(FUNMSG *Fmsg)
{
FNZSEARCHV *pv = (FNZSEARCHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
LONG  IgnrBlnk = FALSE;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
OPKEYFLG OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;											/* TODO: 後で消す		*/
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
		/* Clear	*/
		RpxLcdClrTxBuf( ResBuf );
		/* TITLE	*/
		if( pv->var.State == FNZSEARCHST_SEARCHEND )
		{
//			RpxLcdSetStr(0U, 7U, (UCHAR *)"-Complete-", ResBuf );		/* <S154> */
			RpxLcdSetStr(0U, 9U, (UCHAR *)"Complete", ResBuf );			/* <S154> */
			RpxLcdSetBlnk( 0U, 7U, 10U, ResBuf );
		}
		else
		{
//			RpxLcdSetStr(0U, 7U, (UCHAR *)"-Z-Search-", ResBuf );		/* <S154> */
			RpxLcdSetStr(0U, 9U, (UCHAR *)"Z-Search", ResBuf );			/* <S154> */
			RpxLcdClrLineBlnk( 0U, ResBuf );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ZSearch (Fn003) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_INIT: 												/* Initial State		*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.State = FNZSEARCHST_SVOFF;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_SVOFF: 											/* Servo off state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNZSEARCHST_END;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				if( ( CHK_ALARM() ) || ( CHK_HBB() ) || ( !CHK_CHG() ) )
				{
					pBlinkFlg->no_op = TRUE;
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_SVONOFF, Hmsg );
				}
			}
			else
			{
				/* edit parameter */
				LcdPnUnEditSub( LCDPMDSP_ZSRCH, Fmsg->pKeyMk, Fmsg->pBlnkKick );
			}
			if ( CHK_SVON() == TRUE )
			{
				pv->var.State = FNZSEARCHST_SVON;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_SVON: 												/* Servo on state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpeReadIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResData == (UINT)ZSEARCHSTS_END )
			{
				pv->var.State = FNZSEARCHST_SEARCHEND;
			}
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNZSEARCHST_END;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_SVONOFF, Hmsg );
			}
			else
			{
				if ( OpKeyLvl.up == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_FORWARD, Hmsg );
				}
				else if ( OpKeyLvl.down == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_REVERSE, Hmsg );
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_STOP, Hmsg );
				}
			}
			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNZSEARCHST_SVOFF;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_SEARCHEND: 										/* Searchend state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNZSEARCHST_END;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_SVONOFF, Hmsg );
			}
			else
			{
				/* do nothing */
			}
			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNZSEARCHST_SVOFF;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_END: 												/* End state			*/
/*--------------------------------------------------------------------------------------------------*/
			FunOriginSearchEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		IgnrBlnk = ( FNZSEARCHST_SVON == pv->var.State );
		LcdMkDspDataPnUnLines( LCDPMDSP_ZSRCH, IgnrBlnk, ResBuf );			/* make Pn/Un lines		*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn003 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlOriginSearchExe(FUNMSG *Fmsg)
{
FNZSEARCHV *pv = (FNZSEARCHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
OPKEYFLG OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;											/* TODO: 後で消す		*/
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
/*		ZSearch (Fn003) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_INIT: 												/* Initial State		*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.State = FNZSEARCHST_SVOFF;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_SVOFF: 											/* Servo off state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNZSEARCHST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				if( ( CHK_ALARM() ) || ( CHK_HBB() ) || ( !CHK_CHG() ) )
				{
					pBlinkFlg->no_op = TRUE;
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_SVONOFF, Hmsg );
				}
			}
			else
			{
				/* edit parameter */
				/* LcdPnUnEditSub( LCDPMDSP_ZSRCH, Fmsg->pKeyMk, Fmsg->pBlnkKick ); */
			}
			if ( CHK_SVON() == TRUE )
			{
				pv->var.State = FNZSEARCHST_SVON;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_SVON: 												/* Servo on state 		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX( pv->var.OpeReadIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResData == (UINT)ZSEARCHSTS_END )
			{
				pv->var.State = FNZSEARCHST_SEARCHEND;
			}
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNZSEARCHST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_SVONOFF, Hmsg );
			}
			else
			{
				if ( OpKeyLvl.up == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_FORWARD, Hmsg );
				}
				else if ( OpKeyLvl.down == TRUE )
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_REVERSE, Hmsg );
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_STOP, Hmsg );
				}
			}
			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNZSEARCHST_SVOFF;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_SEARCHEND: 										/* Searchend state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNZSEARCHST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, ZSEARCHCMD_SVONOFF, Hmsg );
			}
			else
			{
				/* do nothing */
			}
			if ( CHK_SVON() == FALSE )
			{
				pv->var.State = FNZSEARCHST_SVOFF;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNZSEARCHST_END: 												/* End state			*/
/*--------------------------------------------------------------------------------------------------*/
			FunOriginSearchEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		MlibResetByteMemory( DspData, sizeof(LEDDSP) );
		/* パネル表示データ作成	*/
		RpxLedSetStr( (UCHAR *)". CSr", DspData, 4U, 5U );
		if( pv->var.State == FNZSEARCHST_SEARCHEND )
		{
			RpxLedSetBlnk( DspData, 0x07U );
		}
		else
		{
			RpxLedSetBlnk( DspData, 0x00U );
		}
		/* Hard-wired Baseblock Display <V068> */
		if( CHK_HBB() )
		{
			if( !( ( KpiGetLongTimer( &( pv->var.DispTime ) ) / 100U ) & 0x04U ) )
			{
				RpxLedSetStr( (UCHAR *)". Hbb", DspData, 4U, 5U );
			}
		}
		RpxLedSetStatusBits( DspData, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
