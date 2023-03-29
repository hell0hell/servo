/*****************************************************************************
Description		: Init. Threshhold Level for Vibration Detect
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunVibDetLvSetMain();					:Fn01B Init. Threshhold Level for Vibration Detect mode Display
	FunVibDetLvSetBegin();					:Fn01B Init. Threshhold Level for Vibration Detect mode Begin
	FunVibDetLvSetEnd();					:Fn01B Init. Threshhold Level for Vibration Detect mode End

	-- Locals --
	LpxLcdVibDetLvSetExe();					:LCD Operator Fn01B Execution
	LpxPnlVibDetLvSetExe();					:PANEL Operator Fn01B Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.07.01		created
SL.Xue		2013.12.10		DONE、ERROR点滅判定処理を修正した

*****************************************************************************/

#include	"FunVibLvlInit.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define	OPESTATUS_REG			(0x000F)		/* Operation Display Status Register	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000)		/* Normal Mode	*/
#define FNVIBLVLINIT_MODE		(0x101B) 		/* Init. Threshhold Level for Vibration Detect Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command	 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define VIBLVLINITCMD_EXE		(0x0001)		/* 検出開始 */
#define VIBLVLINITCMD_ENDWIRTE	(0x0000)		/* 検出終了＆書き込み */

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )    (COPEREG->Read( COPEREG, (idx), (pResData), \
																	(CheckType), hmsg ))

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunVibDetLvSetBegin(FUNMSG *Fmsg); 		/* Fn01B Init. Threshhold Level for Vibration Detect Mode Begin	*/
LONG FunVibDetLvSetMain(FUNMSG *Fmsg);			/* Fn01B Init. Threshhold Level for Vibration Detect Mode Main	*/
void FunVibDetLvSetEnd(FUNMSG *Fmsg);			/* Fn01B Init. Threshhold Level for Vibration Detect Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdVibDetLvSetExe(FUNMSG *Fmsg);			/* LCD Operator Fn01B Execution 			*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlVibDetLvSetExe( x ) (1)
#else
static LONG LpxPnlVibDetLvSetExe(FUNMSG *hMsg);			/* Panel Operator Fn01B Execution 			*/
#endif

/*****************************************************************************
  Description: Fn01B Init. Threshhold Level for Vibration Detect Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunVibDetLvSetMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdVibDetLvSetExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlVibDetLvSetExe( Fmsg );
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
  Description: Fn01B Init. Threshhold Level for Vibration Detect mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Other: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunVibDetLvSetBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNVIBLVLINITV  *pv = (FNVIBLVLINITV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = VIBDET_SEQ_INIT;
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
	rc = COPEREG->Search( COPEREG,							/* Search Register (Operation 0x00F)	*/
					OPESTATUS_REG,
					0,
					&(pv->var.OperatorStsIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set VibLvlInit Mode to Mode Register 															*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNVIBLVLINIT_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn01B Init. Threshhold Level for Vibration Detect Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunVibDetLvSetEnd(FUNMSG *Fmsg)
{
FNVIBLVLINITV  *pv = (FNVIBLVLINITV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: LCD Operator Fn01B Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdVibDetLvSetExe(FUNMSG *Fmsg)
{
FNVIBLVLINITV *pv = (FNVIBLVLINITV *)(Fmsg->pVar);
UCHAR *ResBuf = Fmsg->pTxBuf;
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		RpxLcdClrTxBuf( ResBuf );
		/* TITLE */
		RpxLcdSetStr(1U, 1U, (UCHAR *)"Vibration Detect", ResBuf );
		RpxLcdSetStr(2U, 4U, (UCHAR *)"Level Init", ResBuf );
		/* Contents */
		RpxLcdSetStr(3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf );
		RpxLcdSetStr(4U, 2U, (UCHAR *)"Return:[SET]", ResBuf );
		RpxLcdClrLineBlnk(4U, ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		VibLvlInit (Fn01b) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_INIT: 													/* Initial State 	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VIBDET_SEQ_END;
			}
			else if( OpKeyMake.data == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, VIBLVLINITCMD_EXE, Hmsg );
				pv->var.State = VIBDET_SEQ_DETECT;
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_DETECT: 												/* detect state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = VIBDET_SEQ_END;
			}
			else if( OpKeyMake.data == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, VIBLVLINITCMD_ENDWIRTE, Hmsg );
				if( pv->var.OperatorStsVal == 1 )
				{
					pBlinkFlg->done = TRUE;
				pv->var.State = VIBDET_SEQ_STORE;
			}
				else if( pv->var.OperatorStsVal == -1 )
				{
					pBlinkFlg->error = TRUE;
					pv->var.State = VIBDET_SEQ_ERROR;
				}
				else
				{
					/*do nothing*/
				}
				KpiRstLongTimer( &( pv->var.LongTimer ) );
			}
			else
			{
				/*do nothing*/
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );
			RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
			RpxLcdSetStr( 4U, 0U, (UCHAR *)"    Init         ", ResBuf );
			/* Blink Set	*/
			RpxLcdSetBlnk( 4U, 0U, 17U, ResBuf );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_STORE: 													/* store state		*/
/*--------------------------------------------------------------------------------------------------*/
			RpxLcdSetStr( 3U, 0U, (UCHAR *)"                 ", ResBuf );
			RpxLcdSetStr( 4U, 0U, (UCHAR *)"    Done         ", ResBuf );
			/* Blink Set	*/
			RpxLcdSetBlnk( 4U, 0U, 17U, ResBuf );
			if ( KpiGetLongTimer( &( pv->var.LongTimer ) ) >= 1000U )
			{
				pv->var.State = VIBDET_SEQ_INIT;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_ERROR: 													/* error state		*/
/*--------------------------------------------------------------------------------------------------*/
			if ( KpiGetLongTimer( &( pv->var.LongTimer ) ) >= 1000U )
			{
				pv->var.State = VIBDET_SEQ_INIT;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_END: 													/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunVibDetLvSetEnd( Fmsg );
			RpxLcdClrLineBlnk( 4U, ResBuf );
			rc = FUN_END;
			break;
		default:
			break;
		}
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn01B Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlVibDetLvSetExe(FUNMSG *Fmsg)
{
FNVIBLVLINITV *pv = (FNVIBLVLINITV *)(Fmsg->pVar);
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer */
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		MlibResetByteMemory( DspData, sizeof(LEDDSP) );
		/* パネル表示データ作成	*/
		/*led="d.InIt"*/
		RpxLedSetStr( (UCHAR *)"d.InIt", DspData, 4U, 6U );
		RpxLedSetBlnk( DspData, 0x00U );
/*--------------------------------------------------------------------------------------------------*/
/*		VibLvlInit (Fn01b) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_INIT: 													/* Initial State 	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = VIBDET_SEQ_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, VIBLVLINITCMD_EXE, Hmsg );
				pv->var.State = VIBDET_SEQ_DETECT;
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_DETECT: 												/* detect state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = VIBDET_SEQ_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, VIBLVLINITCMD_ENDWIRTE, Hmsg );
				if( pv->var.OperatorStsVal == 1 )
				{
					pBlinkFlg->done = TRUE;
				pv->var.State = VIBDET_SEQ_STORE;
			}
				else if( pv->var.OperatorStsVal == -1 )
				{
					pBlinkFlg->error = TRUE;
					pv->var.State = VIBDET_SEQ_ERROR;
				}
				else
				{
					/*do nothing*/
				}
				KpiRstLongTimer( &( pv->var.LongTimer ) );
			}
			else
			{
				/*do nothing*/
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );
			RpxLedSetBlnk( DspData, 0x1FU );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_STORE: 													/* store state		*/
/*--------------------------------------------------------------------------------------------------*/
			if ( KpiGetLongTimer( &( pv->var.LongTimer ) ) >= 1000U )
			{
				pv->var.State = VIBDET_SEQ_INIT;
			}
			RpxLedSetBlnk( DspData, 0x00U );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_ERROR: 													/* error state		*/
/*--------------------------------------------------------------------------------------------------*/
			if ( KpiGetLongTimer( &( pv->var.LongTimer ) ) >= 1000U )
			{
				pv->var.State = VIBDET_SEQ_INIT;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case VIBDET_SEQ_END: 													/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunVibDetLvSetEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
