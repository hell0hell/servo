/*****************************************************************************
Description		: Built-in FAN Lifetime Reset (Fn110)	
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunBFanLtRstBegin();				:Fn110 Built-in FAN Lifetime Reset Begin
	FunBFanLtRstMain();					:Fn110 Built-in FAN Lifetime Reset Mode Display
	FunBFanLtRstEnd();					:Fn110 Built-in FAN Lifetime Reset End

	-- Locals --
	LpxLcdBFanLtRstExe();				:Fn110 Built-in FAN Lifetime Reset Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2014.03.05		created

*****************************************************************************/

#include "FunBFanLtRst.h"

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register 		*/
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNBFANLTRST_MODE			(0x2014U) 		/* Built-in FAN Lifetime Reset Mode 			*/
#define FNNORMAL_MODE				(0x0000U)		/* Normal Mode									*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define BFANLTRSTCMD_EXE 			(0x0001)
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
LONG FunBFanLtRstBegin(FUNMSG *Fmsg); 			/* Fn110 Built-in FAN Lifetime Reset Begin			*/
LONG FunBFanLtRstMain(FUNMSG *Fmsg);			/* Fn110 Built-in FAN Lifetime Reset mode Display	*/
void FunBFanLtRstEnd(FUNMSG *Fmsg);				/* Fn110 Built-in FAN Lifetime Reset End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdBFanLtRstExe(FUNMSG *Fmsg);					/* LCD Operator Fn110 Execution 	*/


#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																	(CheckType), hmsg )) /* <S073>	*/


/*****************************************************************************
  Description: Fn110 Built-in FAN Lifetime Reset mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunBFanLtRstMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;

	rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator */
	{
		rc = LpxLcdBFanLtRstExe( Fmsg );
	} 
	
	return rc;
}


/*****************************************************************************
  Description: Fn110 Built-in FAN Lifetime Reset mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunBFanLtRstBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	FNBFANLTRSTV  *pv = (FNBFANLTRSTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNBFANRSTST_EXE;
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
/* Set Built-in FAN Lifetime Reset Mode to Mode Register 											*/
/*--------------------------------------------------------------------------------------------------*/
			rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNBFANLTRST_MODE, Hmsg );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}



/*****************************************************************************
  Description: Fn110 Built-in FAN Lifetime Reset mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunBFanLtRstEnd(FUNMSG *Fmsg)
{
	FNBFANLTRSTV  *pv = (FNBFANLTRSTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn110 Built-in FAN Lifetime Reset mode Execution ( LCD Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdBFanLtRstExe(FUNMSG *Fmsg)
{
	FNBFANLTRSTV *pv = (FNBFANLTRSTV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	UCHAR *ResBuf = Fmsg->pTxBuf;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end	*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 												/* clear Tx buffer		*/
	/* set title	*/
	RpxLcdSetStr(1U, 1U, (UCHAR *)"Built-in FAN", ResBuf);
	RpxLcdSetStr(2U, 2U, (UCHAR *)"Lifetime Reset", ResBuf);
	RpxLcdSetStr(3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf);
	RpxLcdSetStr(4U, 2U, (UCHAR *)"Return:[SET]", ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		Built-in FAN Lifetime Reset(Fn110) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNBFANRSTST_EXE: 							/* Built-in FAN Lifetime Reset Execution	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNBFANRSTST_END;
			}
			else if ( OpKeyMake.data == TRUE )		/* Alarm Traceback Data Clear Execution	*/
			{
				if ( OPEREG_WRITE( pv->var.OpeSetIdx, BFANLTRSTCMD_EXE, Hmsg ) == OK )
				{
					BlinkFlag->done = TRUE;
				}
				else
				{
					BlinkFlag->error = TRUE;
				}
			}
			else
			{ 											
				/* Do Nothing */
			}
			
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNBFANRSTST_END: 													/* 	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunBFanLtRstEnd( Fmsg );
			rc = FUN_END;
			break;
		default: 
			break;
		}

	return (rc);
}


/***************************************** end of file **********************************************/
