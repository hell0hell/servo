/*****************************************************************************
Description		:Fn207 V-Monitor Mode
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunVibMonitorBegin();					:Fn207 V-Monitor Mode Begin
	FunVibMonitorMain();					:Fn207 V-Monitor Mode Display
	FunVibMonitorEnd();						:Fn207 V-Monitor Mode End

	-- Locals --

	LpxVibMonitorExe();						:Fn207 V-Monitor Mode Execution(Digital Operator)
	FunVibMonInitDisplay();					:Fn207 V-Monitor Mode Init Display
	FunVibMonRunDisplay();					:Fn207 V-Monitor Mode Run Display
	FunVibMonResultDisplay();				:Fn207 V-Monitor Mode Result Display
	FunVibMonStoreDisplay();				:Fn207 V-Monitor Mode Store Display

	LpxPnlVibMonitorExe();					:Fn207 V-Monitor Mode Execution(Panel Operator)
	FunPnlVibMonRunDisplay();				:Fn207 V-Monitor Mode Run Display(Panel)
	FunPnlVibMonResultDisplay();			:Fn207 V-Monitor Mode Result Display (Panel)
	FunPnlVibMonInitDisplay();				:Fn207 V-Monitor Mode Init Display (Panel)

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
ZH.QIN		2013.06.07		created
K.Liu		2013.12.09		Modify
*****************************************************************************/

#include	"FunVMonitor.h"

/* defines */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
#define	OPESTATUS_REG		(0x000F)		/* Operation Display Status Register	*/
#define VMONCOMPLETE_REG	(0x0017)		/* VMON Complete Mode Register		*/
#define VMON_FREQ_FIRST		(0x0400)		/* Vmon First Freq Register*/
#define VMON_FREQ_SECOND	(0x0402)		/* Vmon Second Freq Register*/
#define VMON_FREQ_THERD		(0x0404)		/* Vmon Third Freq Register*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE				(0x0000U) 		/* Normal Mode */
#define FNVMON_MODE					(0x1207U) 		/* V-Monitor Mode */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define VMON_STOP 			(0x0)
#define VMON_EXEC 			(0x1)
#define VMON_WRITEPRM		(0x7)
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			0L				/*Continue				*/
#define FUN_END					1L				/*End					*/
/*--------------------------------------------------------------------------------------------------*/

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))  /* <S073> */
/*--------------------------------------------------------------------------------------------------*/
enum VMON_CMP_STS {
	VMON_END_STS		= 0,				/* 完了													*/
	VMON_RUN_STS,							/* 未完了(実行中)										*/
	VMON_ERR_STS,							/* 検出不可												*/
	VMON_STOP_STS							/* 検出停止												*/
};
/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunVibMonitorBegin(FUNMSG *Fmsg);				/* Fn207 V-Monitor Mode Begin				*/
LONG FunVibMonitorMain(FUNMSG *Fmsg);				/* Fn207 V-Monitor Mode Display				*/
void FunVibMonitorEnd(FUNMSG *Fmsg);				/* Fn207 V-Monitor Mode End					*/

/* Locals */
static LONG LpxVibMonitorExe(FUNMSG *Fmsg);					/* Fn207 V-Monitor Mode Exe			*/
static void	FunVibMonInitDisplay( FUNMSG *Fmsg );			/*Fn207 V-Monitor Mode Init Display	*/
static void FunVibMonRunDisplay( FUNMSG *Fmsg );			/*Fn207 V-Monitor Mode Run Display	*/
static void FunVibMonResultDisplay( FUNMSG *Fmsg );			/*Fn207 V-Monitor Mode Result Display*/
static void FunVibMonStoreDisplay( FUNMSG *Fmsg );			/*Fn207 V-Monitor Mode Store Display*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlVibMonitorExe( x ) (0)
#define	FunPnlVibMonRunDisplay( x )	()
#define	FunPnlVibMonResultDisplay( x )	()
#define	FunPnlVibMonInitDisplay( x )	()
#else
LONG LpxPnlVibMonitorExe(FUNMSG *hMsg);					/* Panel Operator Fn207 Execution 		*/
static void FunPnlVibMonRunDisplay( FUNMSG *Fmsg );			/* Fn207 V-Monitor Mode Run Display(Panel) */
static void FunPnlVibMonResultDisplay( FUNMSG *Fmsg );		/* Fn207 V-Monitor Mode Result Display (Panel)*/
static void	FunPnlVibMonInitDisplay( FUNMSG *Fmsg );		/* Fn207 V-Monitor Mode Init Display (Panel) */
#endif

/*****************************************************************************
  Description: Fn207 V-Monitor Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunVibMonitorMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
LONG rc;
rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )							/* LCD Operator */
	{
		rc = LpxVibMonitorExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc =  LpxPnlVibMonitorExe( Fmsg );
	}
	else
	{
		/*do nothing	*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}

/*****************************************************************************
  Description: Fn207 V-Monitor Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: PrgJogBegin Success, Other: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunVibMonitorBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNVMONV  *pv = (FNVMONV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = VMON_SEQ_INIT;
	pv->var.VibmState = VMON_STOP_STS;
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
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (Operation Read)		*/
					VMONCOMPLETE_REG,
					0,
					&(pv->var.VibmStateIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (Freq[0] Read)		*/
				VMON_FREQ_FIRST,
				0,
				&(pv->var.FreqIdx[0]),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (Freq[1] Read)		*/
			VMON_FREQ_SECOND,
			0,
			&(pv->var.FreqIdx[1]),
			MCHK_REG_READ,
			Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,					/* Search Register (Freq[2] Read)		*/
			VMON_FREQ_THERD,
			0,
			&(pv->var.FreqIdx[2]),
			MCHK_REG_READ,
			Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Command)	*/
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
	if( ResFlag == TRUE )
	{
/*--------------------------------------------------------------------------------------------------*/
/* Set V-Monitor Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNVMON_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn207 V-Monitor Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunVibMonitorEnd(FUNMSG *Fmsg)
{
FNVMONV  *pv = (FNVMONV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, VMON_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}

/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Execution(Digital Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxVibMonitorExe(FUNMSG *Fmsg)
{
FNVMONV *pv = (FNVMONV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
//OPKEYFLG OpKeyLevel = *(OPKEYFLG *)(Fmsg->pKeyLvl);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

	STSREG_READX(pv->var.VibmStateIdx, (ULONG *)(&pv->var.VibmState), MCHK_REG_READ, Hmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		V-Monitor (Fn207) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
		case VMON_SEQ_INIT:						/* 振動レベル検出開始待ち						*/
			/* fall through */
		case VMON_SEQ_DETERR:					/* 周波数解析エラー								*/
			if (OpKeyMake.mode == TRUE )
			{
				pv->var.State = VMON_SEQ_END;
			}
//			else if (OpKeyLevel.data == TRUE )
			else if (OpKeyMake.data == TRUE )
			{
				pv->var.State = VMON_SEQ_DETECT;
				KlibRstLongTimer( &(pv->var.TimeOutCtr) );
				OPEREG_WRITE( pv->var.OpeSetIdx, VMON_EXEC, Hmsg );
			}
			else
			{
				/*do nothing */
			}
			FunVibMonInitDisplay( Fmsg );
			break;
		case VMON_SEQ_DETECT:					/* 周波数解析中					*/
			if (KlibGetLongTimerMs( pv->var.TimeOutCtr ) >= 8000U )  /* 8sec経過	*/
			{
				pv->var.State = VMON_SEQ_DETERR;
			}

			if (pv->var.VibmState == VMON_STS_ERR)
			{
				BlinkFlag->error = TRUE;
				pv->var.State = VMON_SEQ_DETERR;
			}
			else if (pv->var.VibmState == VMON_END_STS)
			{
				pv->var.State = VMON_SEQ_DISPLAY;
			}
			else
			{
				/* do nothing*/
			}

			if (OpKeyMake.mode == TRUE )
			{
				pv->var.State = VMON_SEQ_END;
			}
			else if (OpKeyMake.data == TRUE )
			{
				pv->var.State = VMON_SEQ_INIT;
				OPEREG_WRITE( pv->var.OpeSetIdx, VMON_STOP, Hmsg );
			}
			else
			{
				/*do nothing	*/
			}
			FunVibMonRunDisplay( Fmsg );
			break;
		case VMON_SEQ_DISPLAY:				/* 周波数解析結果(フィルタ設定待ち)				*/
			STSREG_READX(pv->var.FreqIdx[0], &(pv->var.Freq[0]), MCHK_REG_READ, Hmsg);
			STSREG_READX(pv->var.FreqIdx[1], &(pv->var.Freq[1]), MCHK_REG_READ, Hmsg);
			STSREG_READX(pv->var.FreqIdx[2], &(pv->var.Freq[2]), MCHK_REG_READ, Hmsg);
			if (OpKeyMake.mode == TRUE )
			{
				pv->var.State = VMON_SEQ_END;
			}
			else if( OpKeyMake.data == TRUE )
			{
				if ((pv->var.VibmState == VMON_END_STS) && ( pv->var.Freq[0] != 0U ))
				{
					pv->var.State = VMON_SEQ_CALC;
					OPEREG_WRITE( pv->var.OpeSetIdx, VMON_WRITEPRM, Hmsg );
				}
			}
			else
			{
				/*do nothing	*/
			}
			FunVibMonResultDisplay( Fmsg );
			break;
		case VMON_SEQ_CALC:						/* フィルタ設定中								*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal  == 1 )
			{
				BlinkFlag->done = TRUE;
			}
			else if( pv->var.OperatorStsVal  == -1 )
			{
				BlinkFlag->error = TRUE;
			}
			else
			{
				/* Do Nothing */
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );

			FunVibMonStoreDisplay( Fmsg );
			KlibRstLongTimer( &(pv->var.TimeOutCtr) );
			pv->var.State = VMON_SEQ_STORE;
			break;
		case VMON_SEQ_STORE:					/* フィルタ保存中								*/
			FunVibMonStoreDisplay( Fmsg );
			if (KlibGetLongTimerMs( pv->var.TimeOutCtr ) >= 1000U )
			{/* 1[s]経過 */
				pv->var.State = VMON_SEQ_DISPLAY;
			}
			break;
		case VMON_SEQ_END:						/* 終了処理										*/
			FunVibMonitorEnd( Fmsg );
			RpxLcdClrLineBlnk( 1U, ResBuf );
			rc = FUN_END;
			break;
	}
	return (rc);
}

/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Init Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunVibMonInitDisplay( FUNMSG *Fmsg )
{
	FNVMONV *pv = (FNVMONV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 									/* clear Tx buffer	*/
//	RpxLcdSetStr(0U, 6U, (UCHAR *)"-V-MONITOR-", ResBuf);		/* set title 		*//* <S154> */
	RpxLcdSetStr(0U, 8U, (UCHAR *)"V-MONITOR", ResBuf);			/* set title 		*//* <S154> */
	if (pv->var.VibmState == VMON_STS_ERR)
	{
		RpxLcdSetStr(1U, 0U, (UCHAR *)"NO MONITOR       ", ResBuf);
	}
	else
	{
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Measure          ", ResBuf);
		RpxLcdClrLineBlnk( 1U, ResBuf );
	}
	RpxLcdSetStr(2U, 0U, (UCHAR *)"F1=----          ", ResBuf);
	RpxLcdSetStr(3U, 0U, (UCHAR *)"F2=----          ", ResBuf);
	RpxLcdSetStr(4U, 0U, (UCHAR *)"F3=----          ", ResBuf);
}
/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Run Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunVibMonRunDisplay( FUNMSG *Fmsg )
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	RpxLcdSetBlnk( 1U, 0U, 17U, ResBuf );
}
/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Result Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunVibMonResultDisplay( FUNMSG *Fmsg )
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	FNVMONV *pv = (FNVMONV *)(Fmsg->pVar);
	RpxLcdSetStr(1U, 0U, (UCHAR *)"Measure          ", ResBuf);
	RpxLcdClrLineBlnk( 1U, ResBuf );
	if ( pv->var.Freq[0] != 0U )
	{
		RpxLcdSetStr( 2U, 0U, (UCHAR *)"F1=      [Hz] ", ResBuf );
		RpxLcdSetDecPnt( 2U, 4U, pv->var.Freq[0] , 4U, (USHORT)FALSE, 0, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 2U, 0U, (UCHAR *)"F1=----       ", ResBuf );
	}
	if ( pv->var.Freq[1] != 0U )
	{
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"F2=      [Hz] ", ResBuf );
		RpxLcdSetDecPnt( 3U, 4U, pv->var.Freq[1], 4U, (USHORT)FALSE, 0, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 3U, 0U, (UCHAR *)"F2=----       ", ResBuf );
	}
	if ( pv->var.Freq[2] != 0U )
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"F3=      [Hz] ", ResBuf );
		RpxLcdSetDecPnt( 4U, 4U, pv->var.Freq[2], 4U, (USHORT)FALSE, 0, ResBuf );
	}
	else
	{
		RpxLcdSetStr( 4U, 0U, (UCHAR *)"F3=----       ", ResBuf );
	}
}
/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Store Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunVibMonStoreDisplay( FUNMSG *Fmsg )
{
	UCHAR *ResBuf = Fmsg->pTxBuf;
	RpxLcdSetStr(1U, 0U, (UCHAR *)"SETTING DONE     ", ResBuf);
	RpxLcdClrLineBlnk( 1U, ResBuf );
}


#if SVFSEL_PANELOP_USE	== 1
/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Execution (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxPnlVibMonitorExe(FUNMSG *Fmsg)
{
FNVMONV *pv = (FNVMONV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
//OPKEYFLG OpKeyLevel = *(OPKEYFLG *)(Fmsg->pKeyLvl);
BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

	STSREG_READX(pv->var.VibmStateIdx, (ULONG *)(&pv->var.VibmState), MCHK_REG_READ, Hmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		V-Monitor (Fn207) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
		case VMON_SEQ_INIT:						/* 振動レベル検出開始待ち						*/
			/* fall through */
		case VMON_SEQ_DETERR:					/* 周波数解析エラー								*/
			if (OpKeyMake.data == TRUE )
			{
				pv->var.State = VMON_SEQ_END;
			}
//			else if (OpKeyLevel.mode == TRUE )
			else if (OpKeyMake.mode == TRUE )
			{
				pv->var.State = VMON_SEQ_DETECT;
				KlibRstLongTimer( &(pv->var.TimeOutCtr) );
				OPEREG_WRITE( pv->var.OpeSetIdx, VMON_EXEC, Hmsg );
			}
			else
			{
				/*do nothing */
			}
			FunPnlVibMonInitDisplay( Fmsg );
			break;
		case VMON_SEQ_DETECT:					/* 周波数解析中									*/
			if (KlibGetLongTimerMs( pv->var.TimeOutCtr ) >= 8000U )  /* 8sec経過	*/
			{
				pv->var.State = VMON_SEQ_DETERR;
			}

			if (pv->var.VibmState == VMON_STS_ERR)
			{
				BlinkFlag->error = TRUE;
				pv->var.State = VMON_SEQ_DETERR;
			}
			else if (pv->var.VibmState == VMON_END_STS)
			{
				pv->var.State = VMON_SEQ_DISPLAY;
			}
			else
			{
				/* do nothing*/
			}
			if (OpKeyMake.data == TRUE )
			{
				pv->var.State = VMON_SEQ_END;
			}
			else if (OpKeyMake.mode == TRUE )
			{
				pv->var.State = VMON_SEQ_INIT;
				OPEREG_WRITE( pv->var.OpeSetIdx, VMON_STOP, Hmsg );
			}
			else
			{
				/*do nothing	*/
			}
			FunPnlVibMonRunDisplay( Fmsg );
			break;
		case VMON_SEQ_DISPLAY:				/* 周波数解析結果(フィルタ設定待ち)				*/
			STSREG_READX(pv->var.FreqIdx[0], &(pv->var.Freq[0]), MCHK_REG_READ, Hmsg);
			if ( OpKeyMake.data == TRUE )
			{
				pv->var.State = VMON_SEQ_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				if ((pv->var.VibmState == VMON_END_STS) && ( pv->var.Freq[0] != 0U ))
				{
					pv->var.State = VMON_SEQ_CALC;
					OPEREG_WRITE( pv->var.OpeSetIdx, VMON_WRITEPRM, Hmsg );
				}
			}
			else
			{
				/* do nothing	*/
			}
			FunPnlVibMonResultDisplay( Fmsg );
			break;
		case VMON_SEQ_CALC:						/* フィルタ設定中								*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal  == 1 )
			{
				BlinkFlag->done = TRUE;
			}
			else if( pv->var.OperatorStsVal  == -1 )
			{
				BlinkFlag->error = TRUE;
			}
			else
			{
				/* Do Nothing */
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );

			KlibRstLongTimer( &(pv->var.TimeOutCtr) );
			pv->var.State = VMON_SEQ_STORE;
			break;
		case VMON_SEQ_STORE:					/* フィルタ保存中								*/
			if (KlibGetLongTimerMs( pv->var.TimeOutCtr ) >= 1000U )
			{/* 1[s]経過 */
				pv->var.State = VMON_SEQ_DISPLAY;
			}
			break;
		case VMON_SEQ_END:						/* 終了処理										*/
			FunVibMonitorEnd( Fmsg );
			rc = FUN_END;
			break;
	}
	return (rc);
}


/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Run Display(Panel)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunPnlVibMonRunDisplay( FUNMSG *Fmsg )
{
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	RpxLedSetBlnk( DspData, 0x1F );
}


/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Result Display (Panel)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunPnlVibMonResultDisplay( FUNMSG *Fmsg )
{
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	FNVMONV *pv = (FNVMONV *)(Fmsg->pVar);
	RpxLedSetBlnk( DspData, 0x00 );

	if ( pv->var.Freq[0] != 0 )
	{
		RpxLedSetStr( (UCHAR *)"F", DspData, 4U, 1U );
		RpxLedSetValDec( (USHORT)pv->var.Freq[0], DspData, 3U, 4U, TRUE, 0U );
	}
	else
	{
		RpxLedSetStr( (UCHAR *)"F----", DspData, 4U, 5U );
	}
}

/*****************************************************************************
  Description:	Fn207 V-Monitor Mode Init Display (Panel)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	FunPnlVibMonInitDisplay( FUNMSG *Fmsg )
{
	FNVMONV *pv = (FNVMONV *)(Fmsg->pVar);
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	/*--------------------------------------------------------------------------------------------------*/
	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	RpxLedSetBlnk( DspData, 0x00 );

	/* パネル表示データ作成	*/
	if ( pv->var.VibmState == VMON_STS_ERR )
	{
		RpxLedSetStr((UCHAR *)"F----", DspData, 4U, 5U);
	}
	else
	{
		RpxLedSetStr((UCHAR *)"F    ", DspData, 4U, 5U);
	}
}
#endif	/*	#if SVFSEL_PANELOP_USE	== 1	*/
/***************************************** end of file **********************************************/

