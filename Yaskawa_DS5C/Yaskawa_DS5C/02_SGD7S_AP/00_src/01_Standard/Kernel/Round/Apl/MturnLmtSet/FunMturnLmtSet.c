/*****************************************************************************
Description		: Multi Turn Limit Setting 
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunMultiTurnLimitSetMain();			:Fn013 Multi Turn Limit Setting Mode Display
	FunMultiTurnLimitSetBegin();		:Fn013 Multi Turn Limit Setting Modde Begin
	FunMultiTurnLimitSetEnd();			:Fn013 Multi Turn Limit Setting Mode End

	-- Locals --
	LpxLcdMultiTurnLimitSetExe();		:LCD Operator Fn013 Execution
	LpxPnlMultiTurnLimitSetExe();		:Panel Operator Fn013 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.07		created
K.Liu		2013.08.29		Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/

#include	"FunMturnLmtSet.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register 		*/
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register	*/

#define	OPESTATUS_REG		(0x000F)		/* Operation Display Status Register	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNMTURNLMTSET_MODE			(0x1013) 			/* Multi Turn Limit Setting Mode */
#define FNNORMAL_MODE				(0x0000U)			/* Normal Mode	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define MTURNLMTSETCMD_EXE	 		(0x0001)
#define MTURNLMTSETCMD_PROTPROC		(0x0002)			/*	Protection Processing	*/
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
LONG FunMultiTurnLimitSetBegin(FUNMSG *Fmsg); 		/* Fn013 Multi Turn Limit Setting Mode Begin	*/
LONG FunMultiTurnLimitSetMain(FUNMSG *Fmsg);		/* Fn013 Multi Turn Limit Setting Mode Display	*/
void FunMultiTurnLimitSetEnd(FUNMSG *Fmsg);			/* Fn013 Multi Turn Limit Setting Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdMultiTurnLimitSetExe(FUNMSG *Fmsg);			/* LCD Operator Fn013 Execution 	*/


#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlMultiTurnLimitSetExe( x ) ( 1 )
#else
LONG LpxPnlMultiTurnLimitSetExe(FUNMSG *hMsg);					/* Panel Operator Fn013 Execution 	*/
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), \
																	(pResData), (CheckType), hmsg ))  /* <S073> */


/*****************************************************************************
  Description: Fn013 Multi Turn Limit Setting Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunMultiTurnLimitSetMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;

	rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (UINT)(TRUE) )					/* LCD Operator */
	{
		rc = LpxLcdMultiTurnLimitSetExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/* Panel Operator */
	{
		rc = LpxPnlMultiTurnLimitSetExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		/*	Do Nothing	*/
	}
	
	return rc;
}


/*****************************************************************************
  Description: Fn013 Multi Turn Limit Setting Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunMultiTurnLimitSetBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	FNMTURNLMTSETV  *pv = (FNMTURNLMTSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNMTURNLMTSETST_INIT;
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
			rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Command)	*/
						OPESTATUS_REG,
						0,
						&(pv->var.OperatorStsIdx),
						MCHK_REG_READ,
						Hmsg);
			if( rc == OK )
			{
/*--------------------------------------------------------------------------------------------------*/
/* Set Multi Turn Limit Setting Mode to Mode Register 												*/
/*--------------------------------------------------------------------------------------------------*/
				rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNMTURNLMTSET_MODE, Hmsg );
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn013 Multi Turn Limit Setting Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunMultiTurnLimitSetEnd(FUNMSG *Fmsg)
{
	FNMTURNLMTSETV  *pv = (FNMTURNLMTSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn013 Multi Turn Limit Setting Mode Execution( LCD Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdMultiTurnLimitSetExe(FUNMSG *Fmsg)
{
	FNMTURNLMTSETV *pv = (FNMTURNLMTSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 												/* clear Tx buffer		*/
	/* set title	*/
	RpxLcdSetStr(1U, 1U, (UCHAR *)"Multiturn Limit", ResBuf);
	RpxLcdSetStr(2U, 7U, (UCHAR *)"Set", ResBuf);
	/* Set Contents	*/
	RpxLcdSetStr(3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf);
	RpxLcdSetStr(4U, 2U, (UCHAR *)"Return:[SET]", ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		Multi Turn Limit Setting(Fn013) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_INIT: 										/* Initial State 			*/
/*--------------------------------------------------------------------------------------------------*/
			OPEREG_WRITE( pv->var.OpeSetIdx, MTURNLMTSETCMD_PROTPROC, Hmsg );
			pv->var.State = FNMTURNLMTSETST_IDLE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_IDLE: 								/* Multi Turn Limit Setting Idle	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMTURNLMTSETST_END;
			}
			else if ( OpKeyMake.data == TRUE )		/* Multi Turn Limit Setting Execution	*/
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, MTURNLMTSETCMD_EXE, Hmsg );
				pv->var.State = FNMTURNLMTSETST_EXECUTING;
			}
			else
			{
				/* Do Nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_EXECUTING: 					/* Multi Turn Limit Setting Executing	*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal  == 1 )
			{
				BlinkFlag->done = TRUE;
				pv->var.State = FNMTURNLMTSETST_IDLE;
			}
			else if( pv->var.OperatorStsVal  == -1 )
			{
				BlinkFlag->error = TRUE;
				pv->var.State = FNMTURNLMTSETST_IDLE;
			}
			else
			{
				/* Do Nothing */
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );

			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_END: 												/*		End state	*/
/*--------------------------------------------------------------------------------------------------*/
			FunMultiTurnLimitSetEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}

	return (rc);
}

/*****************************************************************************
  Description: Fn013 Multi Turn Limit Setting Mode Execution( Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlMultiTurnLimitSetExe(FUNMSG *Fmsg)
{
	FNMTURNLMTSETV *pv = (FNMTURNLMTSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Multi Turn Limit Setting(Fn013) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_INIT: 										/* Initial State 			*/
/*--------------------------------------------------------------------------------------------------*/
			OPEREG_WRITE( pv->var.OpeSetIdx, MTURNLMTSETCMD_PROTPROC, Hmsg );	
			pv->var.State = FNMTURNLMTSETST_IDLE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_IDLE: 								/* Multi Turn Limit Setting Idle	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMTURNLMTSETST_END;
			}
			else if ( OpKeyMake.mode == TRUE )		/* Multi Turn Limit Setting Execution	*/
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, MTURNLMTSETCMD_EXE, Hmsg );
				pv->var.State = FNMTURNLMTSETST_EXECUTING;
			}
			else
			{
				/* Do Nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_EXECUTING: 					/* Multi Turn Limit Setting Executing	*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal  == 1 )
			{
				BlinkFlag->done = TRUE;
				pv->var.State = FNMTURNLMTSETST_IDLE;
			}
			else if( pv->var.OperatorStsVal  == -1 )
			{
				BlinkFlag->error = TRUE;
				pv->var.State = FNMTURNLMTSETST_IDLE;
			}
			else
			{
				/* Do Nothing */
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNLMTSETST_END: 												/*		End state	*/
/*--------------------------------------------------------------------------------------------------*/
			FunMultiTurnLimitSetEnd( Fmsg );
			rc = FUN_END;
			break;
		default: 
			break;
		}

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/
	/* パネル表示データ作成	*/
	/*led="PGSEt"*/
	RpxLedSetStr((UCHAR *)"PGSEt", DspData, 4U, 5U );

	return (rc);
}

#endif	/* #if  SVFSEL_PANELOP_USE == 1 */






/***************************************** end of file **********************************************/
