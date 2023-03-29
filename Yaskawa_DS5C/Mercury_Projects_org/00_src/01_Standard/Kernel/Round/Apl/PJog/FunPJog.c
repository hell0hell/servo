/*****************************************************************************
Description		: Program JOG module
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunPrgJogMain();					:Fn004 Program Jog mode Display
	FunPrgJogBegin();					:Fn004 Program Jog mode Begin
	FunPrgJogEnd();						:Fn004 Program Jog mode End

	-- Locals --
	LpxLcdPrgJogExe();					:LCD Operator Fn004 Execution
	LpxPnlPrgJogExe();					:Panel Operator Fn004 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.07		created
K.Liu		2013.08.28  	Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/

#include	"FunPJog.h"
#include	"LcdOp.h"		/* LCD Operator */
#include	"Parameter.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register 		*/
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register	*/
#define PJOGCOMPLETE_REG	(0x0031)		/* ÉvÉçÉOÉâÉÄÇiÇnÇfâ^ì]äÆóπämîF		*/
#define PRMPJOGSW_REG		(0x0530)		/* ÉvÉçÉOÉâÉÄJOG â^ì]ä÷åWÉXÉCÉbÉ`		*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNPRGJOG_MODE			(0x1004) 			/* PrgJog Mode */
#define FNNORMAL_MODE			(0x0000U)			/* Normal Mode	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define PROGRAMJOGCMD_STOP 			(0x0)
#define PROGRAMJOGCMD_RUN 			(0x1)
#define PROGRAMJOGCMD_FORWARD 		(0x2)
#define PROGRAMJOGCMD_REVERSE 		(0x3)
#define PROGRAMJOGCMD_SVONOFF 		(0x4)

/*--------------------------------------------------------------------------------------------------*/
/*		 Program JOG State																			*/
/*--------------------------------------------------------------------------------------------------*/
#define PJOG_INIT_ABORT			0U									/* Initial or Abort */
#define PJOG_COMPLETE			2U									/* Program Jog Complete 	*/
#define PJOG_ERRABORT			3U									/* Program Jog Abort	*/
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE				0			/* Continue */
#define FUN_END						1			/* End */
#define END_BLINK_TIME				1000		/* 1 ïb */
/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunPrgJogBegin(FUNMSG *Fmsg); 						/* Fn004 Program Jog mode Begin				*/
LONG FunPrgJogMain(FUNMSG *Fmsg);						/* Fn004 Program Jog mode Display			*/
void FunPrgJogEnd(FUNMSG *Fmsg);						/* Fn004 Program Jog mode End				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdPrgJogExe(FUNMSG *Fmsg);						/* LCD Operator Fn004 Execution 	*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlPrgJogExe( x ) (1)
#else
LONG LpxPnlPrgJogExe(FUNMSG *Fmsg);								/* Panel Operator Fn004 Execution 	*/
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))				/*<S054>*/
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), \
																	(pResData),	(CheckType), hmsg ))	/*<S054>*/  /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))	/*<S054>*/  /* <S073> */

/*****************************************************************************
  Description: Fn004 Program Jog mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunPrgJogMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;

	rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator		*/
	{
		rc = LpxLcdPrgJogExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == (TRUE) )				/* Panel Operator	*/
	{
		rc = LpxPnlPrgJogExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		/*	Do Nothing	*/
	}

	return rc;
}


/*****************************************************************************
  Description: Fn004 Program Jog mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunPrgJogBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;
	FNPROGRAMJOGV  *pv = (FNPROGRAMJOGV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNPROGRAMJOGST_INIT;
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
					PJOGCOMPLETE_REG,
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
	if ( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,	/* Search Register (ÉvÉçÉOÉâÉÄJOG â^ì]ä÷åWÉXÉCÉbÉ`)*/
					PRMPJOGSW_REG,
					0,
					&(pv->var.PrmPJogSwIdx),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* Set Program Jog Mode to Mode Register 															*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNPRGJOG_MODE, Hmsg );
		KlibRstLongTimer( &(pv->var.DispTime) );						/* Panel Operator 			*/
	}
/*--------------------------------------------------------------------------------------------------*/
//	KpiRstLongTimer( &(pv->var.DispTime) );						/* Panel Operator 					*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn004 Program Jog mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunPrgJogEnd(FUNMSG *Fmsg)
{
	FNPROGRAMJOGV  *pv = (FNPROGRAMJOGV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_STOP, Hmsg );
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn004 Program Jog mode Execution (LCD Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdPrgJogExe(FUNMSG *Fmsg)
{
	FNPROGRAMJOGV *pv = (FNPROGRAMJOGV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	LONG  IgnrBlnk = FALSE;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;											/* TODO: å„Ç≈è¡Ç∑		*/
	LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 												/* clear Tx buffer		*/
//	RpxLcdSetStr(0U, 8U, (UCHAR *)"-PRG JOG-", ResBuf );					/* set title 			*//* <S154> */
	RpxLcdSetStr(0U, 10U, (UCHAR *)"PRG JOG", ResBuf );						/* set title 			*//* <S154> */
/*--------------------------------------------------------------------------------------------------*/
/*		PrgJog (Fn004) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_INIT: 										/* Initial State 			*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.SafeStopCmd = 0;									/* <S198> */
			pv->var.State = FNPROGRAMJOGST_SVOFF;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_SVOFF: 										/* Servo off state			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNPROGRAMJOGST_END;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_SVONOFF, Hmsg ) != OK )	/* servo on */
				{
					BlinkFlag->no_op = TRUE;	/* no_opÅ@ï\é¶ëÄçÏèàóù*/
				}
			}
			else
			{
				/* Do Nothing */
			}

			if ( CHK_SVON() == TRUE )
			{
				PRMREG_READX(pv->var.PrmPJogSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
				pv->var.RevDir = ( (pv->var.ResData % 2) > 0 )? TRUE : FALSE;
				pv->var.State = FNPROGRAMJOGST_SVON;
			}
			else
			{
				/* edit parameter */
				LcdPnUnEditSub( LCDPMDSP_PJOG, Fmsg->pKeyMk, Fmsg->pBlnkKick );
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_SVON: 											/*	Servo on state 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNPROGRAMJOGST_END;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_SVONOFF, Hmsg );
			}
			else 
			{
				if ( (pv->var.RevDir == FALSE) && (OpKeyMake.up == TRUE) )			/* Program JOGâ^ì]äJén(ê≥ì])	*/
				{
					pv->var.SafeStopCmd = PROGRAMJOGCMD_FORWARD;					/* <S198> */
					OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_FORWARD, Hmsg );
					pv->var.State = FNPROGRAMJOGST_RUN;
					
				}
				else if ( (pv->var.RevDir == TRUE) && (OpKeyMake.down == TRUE) )	/* Program JOGâ^ì]äJén(ãtì])	*/
				{
					pv->var.SafeStopCmd = PROGRAMJOGCMD_REVERSE;					/* <S198> */
					OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_REVERSE, Hmsg );
					pv->var.State = FNPROGRAMJOGST_RUN;
				}
				else
				{
					/* Do Nothing */
				}
			}

			if ( CHK_SVON() == FALSE)
			{
				pv->var.State = FNPROGRAMJOGST_SVOFF; 								/* servo off */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_RUN: 											/* 		â^ì]íÜ			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNPROGRAMJOGST_END;
			}
			else if ( OpKeyMake.svon == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_SVONOFF, Hmsg );
			}
			else 
			{
				/* Do Nothing */
				OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );	/* <S198> */
			}
			
			if ( CHK_SVON() == FALSE)
			{
				pv->var.State = FNPROGRAMJOGST_SVOFF; 								/* servo off */
			}

			STSREG_READX(pv->var.OpeReadIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			if( ( pv->var.ResData == PJOG_INIT_ABORT ) || ( pv->var.ResData == PJOG_COMPLETE )
					|| ( pv->var.ResData == PJOG_ERRABORT ) )
			{
				pv->var.State = FNPROGRAMJOGST_STOP;
				KlibRstLongTimer(&pv->var.LongTime);
				if( pv->var.ResData == PJOG_COMPLETE )
				{
					BlinkFlag->end = TRUE;
				}
				else if ( pv->var.ResData == PJOG_ERRABORT )
				{
					BlinkFlag->error = TRUE;
				}
				else
				{
					/* Do Nothing */
				}
			}
			
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_STOP: 											/* 	â^ì]èIóπ(å„èàóù)	*/
/*--------------------------------------------------------------------------------------------------*/
			if( KlibGetLongTimerMs( pv->var.LongTime ) > END_BLINK_TIME )
			{
				pv->var.State = FNPROGRAMJOGST_SVON;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunPrgJogEnd( Fmsg );
			rc = FUN_END;
			break;
		default: 
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
		IgnrBlnk = ( FNPROGRAMJOGST_SVON == pv->var.State );					
		LcdMkDspDataPnUnLines(LCDPMDSP_PJOG, IgnrBlnk, ResBuf ); 				/* make Pn/Un lines */
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn004 Program Jog mode Execution (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlPrgJogExe(FUNMSG *Fmsg)
{
	FNPROGRAMJOGV *pv = (FNPROGRAMJOGV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;											/* TODO: å„Ç≈è¡Ç∑		*/
	LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*		PrgJog (Fn004) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_INIT: 										/* Initial State 			*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.State = FNPROGRAMJOGST_SVOFF;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_SVOFF: 										/* Servo off state			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNPROGRAMJOGST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_SVONOFF, Hmsg ) != OK )	/* servo on */
				{
					BlinkFlag->no_op = TRUE;	/* no_opÅ@ï\é¶ëÄçÏèàóù*/
				}
			}
			else
			{
				/* Do Nothing  */
			}

			if ( CHK_SVON() == TRUE)
			{
				PRMREG_READX(pv->var.PrmPJogSwIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
				pv->var.RevDir = ( (pv->var.ResData % 2) > 0 )? TRUE : FALSE;
				pv->var.State = FNPROGRAMJOGST_SVON; 								/* servo off */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_SVON: 											/*	Servo on state 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNPROGRAMJOGST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_SVONOFF, Hmsg );
			}
			else
			{
				if ( (pv->var.RevDir == FALSE) && (OpKeyMake.up == TRUE) )			/* Program JOGâ^ì]äJén(ê≥ì])	*/
				{
					pv->var.SafeStopCmd = PROGRAMJOGCMD_FORWARD;					/* <S1A7> */
					OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_FORWARD, Hmsg );
					pv->var.State = FNPROGRAMJOGST_RUN;

				}
				else if ( (pv->var.RevDir == TRUE) && (OpKeyMake.down == TRUE) )	/* Program JOGâ^ì]äJén(ãtì])	*/
				{
					pv->var.SafeStopCmd = PROGRAMJOGCMD_REVERSE;					/* <S1A7> */
					OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_REVERSE, Hmsg );
					pv->var.State = FNPROGRAMJOGST_RUN;
				}
				else
				{
					/* Do Nothing */
				}
			}

			if ( CHK_SVON() == FALSE)
			{
				pv->var.State = FNPROGRAMJOGST_SVOFF; 								/* servo off */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_RUN: 											/* 		â^ì]íÜ			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNPROGRAMJOGST_END;
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, PROGRAMJOGCMD_SVONOFF, Hmsg );
			}
			else
			{
				/* Do Nothing */
				OPEREG_WRITE( pv->var.OpeSetIdx, pv->var.SafeStopCmd, Hmsg );	/* <S1A7> */
			}

			if ( CHK_SVON() == FALSE)
			{
				pv->var.State = FNPROGRAMJOGST_SVOFF; 								/* servo off */
			}

			STSREG_READX(pv->var.OpeReadIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
			if( ( pv->var.ResData == PJOG_INIT_ABORT ) || ( pv->var.ResData == PJOG_COMPLETE )
					|| ( pv->var.ResData == PJOG_ERRABORT ) )
			{
				pv->var.State = FNPROGRAMJOGST_STOP;
				KlibRstLongTimer(&pv->var.LongTime);
				if( pv->var.ResData == PJOG_COMPLETE )
				{
					BlinkFlag->end = TRUE;
				}
				else if ( pv->var.ResData == PJOG_ERRABORT )
				{
					BlinkFlag->error = TRUE;
				}
				else
				{
					/* Do Nothing */
				}
			}

			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_STOP: 											/* 	â^ì]èIóπ(å„èàóù)	*/
/*--------------------------------------------------------------------------------------------------*/
			if( KlibGetLongTimerMs( pv->var.LongTime ) > END_BLINK_TIME )
			{
				pv->var.State = FNPROGRAMJOGST_SVON;
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNPROGRAMJOGST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunPrgJogEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Make Display Data																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

		RpxLedSetStr( (UCHAR *)".PJOG", DspData, 4U, 5U );
		/*------------------------------------------------------------------------------------------*/
		/*
		 * Hard-wired Baseblock Display <V068>
		 *
		 *     set "Hbb" to the panel once every 100*4 msec. (SGDV-AP specification)
		 *
		 */
		/*------------------------------------------------------------------------------------------*/
		if( CHK_HBB() == TRUE/* CoutV.f.Sv.HwbbState */ )
		{
			if( !( (KpiGetLongTimer( &(pv->var.DispTime) )/100U) & 0x04U) )
			{
				RpxLedSetStr( (UCHAR *)". Hbb", DspData, 4U, 5U );	/* set HBB */
			}
		}
		RpxLedSetStatusBits( DspData, Hmsg );						/* set panel status bits */
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif	/* #if  SVFSEL_PANELOP_USE == 1 */


/***************************************** end of file **********************************************/
