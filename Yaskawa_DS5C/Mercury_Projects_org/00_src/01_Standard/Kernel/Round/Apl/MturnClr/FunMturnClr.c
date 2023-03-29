/*****************************************************************************
Description		: Multi Turn Data Reset & Encoder Alarm Reset
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunMultiTurnResetMain();			:Fn008 Multi Turn Data Reset & Encoder Alarm Reset mode Display
	FunMultiTurnResetBegin();			:Fn008 Multi Turn Data Reset & Encoder Alarm Reset mode Begin
	FunMultiTurnResetEnd();				:Fn008 Multi Turn Data Reset & Encoder Alarm Reset mode End

	-- Locals --
	LpxLcdMultiTurnResetExe();			:LCD Operator Fn008 Execution
	LpxPnlMultiTurnResetExe();			:PANEL Operator Fn008 Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.09		created
SL.Xue		2013.12.10		DONE、ERROR点滅判定処理を修正した

*****************************************************************************/
#include	"FunMturnClr.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */
#define	OPESTATUS_REG			(0x000F)		/* Operation Display Status Register	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define FNMTURNCLR_MODE			(0x1008U) 		/* MturnClr Mode */
#define FNNORMAL_MODE			(0x0000U)		/* Normal mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define MTURNCLRST_EXE 			(0x1)			/* MturnClr Execute */
#define MTURNCLRST_PROTECT 		(0x2)			/* MturnClr Protect */

/*--------------------------------------------------------------------------------------------------*/
/* Un Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define UNALMCODE_REG1			(0x050A)		/* 0x50A: 現在のアラーム状態 */

/*--------------------------------------------------------------------------------------------------*/
/* Un Command 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define MTURNCLRCSTS_ERROR		(0x0C80U) 		/* A.C80：PGクリア異常 （クリア失敗） */
#define MTURNCLRCSTS_SUCCESS	(0x0000U)		/* 正常＇アラームなし （クリア成功） */

/*--------------------------------------------------------------------------------------------------*/
/* Multi Turn Data Reset & Encoder Alarm Reset 														*/
/*--------------------------------------------------------------------------------------------------*/
#define MTNOPNUM_MAX			(0x05U)			/* Multi Turn Data Reset & Encoder Alarm Reset	*/

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )    (COPEREG->Read( COPEREG, (idx), (pResData), \
																	(CheckType), hmsg ))
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))	/*<S054>*/ /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMultiTurnResetBegin(FUNMSG *Fmsg); 				/* Fn008 MturnClr Mode Begin				*/
LONG FunMultiTurnResetMain(FUNMSG *Fmsg);				/* Fn008 MturnClr Mode Main					*/
void FunMultiTurnResetEnd(FUNMSG *Fmsg);				/* Fn008 MturnClr Mode End					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdMultiTurnResetExe(FUNMSG *Fmsg);		/* LCD Operator Fn008 Execution 			*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlMultiTurnResetExe( x ) (1)
#else
static LONG LpxPnlMultiTurnResetExe(FUNMSG *hMsg);		/* Panel Operator Fn008 Execution 			*/
#endif

/*****************************************************************************
  Description: Fn008 MturnClr Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunMultiTurnResetMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdMultiTurnResetExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlMultiTurnResetExe( Fmsg );
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
  Description: Fn008 MturnClr Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunMultiTurnResetBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNMTURNCLRV  *pv = (FNMTURNCLRV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNMTURNCLRST_INIT;
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
/* Set MturnClr Mode to Mode Register 																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNMTURNCLR_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn008 MturnClr Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunMultiTurnResetEnd(FUNMSG *Fmsg)
{
FNMTURNCLRV  *pv = (FNMTURNCLRV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: LCD Operator Fn008 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdMultiTurnResetExe(FUNMSG *Fmsg)
{
FNMTURNCLRV *pv = (FNMTURNCLRV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
ASIC_HW_IF_STR	*AsicHwReg = Hmsg->pAxRsc->SvAsicRegs->AsicHwReg;		/* <S169> */
/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		RpxLcdClrTxBuf( ResBuf );
		/* set title */
//		RpxLcdSetStr(2U, 1U, (UCHAR *)"-Multiturn Clear-", ResBuf );	/* <S154> */
		RpxLcdSetStr(2U, 1U, (UCHAR *)"Multiturn Clear", ResBuf );		/* <S154> */
		RpxLcdSetStr(4U, 5U, (UCHAR *)"PGCL", ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		MturnClr (Fn008) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_INIT: 											/* Initial State		*/
/*--------------------------------------------------------------------------------------------------*/
			OPEREG_WRITE( pv->var.OpeSetIdx, MTURNCLRST_PROTECT, Hmsg );
			pv->var.Cnt = 1;
			pv->var.State = FNMTURNCLRST_IDLE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_IDLE: 											/* start state			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMTURNCLRST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				if( pv->var.Cnt < MTNOPNUM_MAX )
				{
					pv->var.Cnt++;
				}
			}
			else if( OpKeyMake.data == TRUE )
			{
				//if( pv->var.Cnt != MTNOPNUM_MAX )
				if(( pv->var.Cnt != MTNOPNUM_MAX )||( SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0 ))	/* <S169> */
				{
					pBlinkFlg->no_op = TRUE;
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, MTURNCLRST_EXE, Hmsg );
					pv->var.State = FNMTURNCLRST_MULTITURN;
				}
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_MULTITURN: 										/* Multiturn state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal == 1 )
			{
				pBlinkFlg->done = TRUE;
				pv->var.State = FNMTURNCLRST_IDLE;
			}
			else if( pv->var.OperatorStsVal == -1 )
			{
				pBlinkFlg->error = TRUE;
				pv->var.State = FNMTURNCLRST_IDLE;
			}
			else
			{
				/* Do Nothing */
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_END: 												/* End state			*/
/*--------------------------------------------------------------------------------------------------*/
			FunMultiTurnResetEnd( Fmsg );
			rc = FUN_END;
			break;
		default: 
			break;
		}
		RpxLcdSetDecPnt(4U, 9U, pv->var.Cnt, 1U, (USHORT)FALSE, 0, ResBuf);
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn008 Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlMultiTurnResetExe(FUNMSG *Fmsg)
{
FNMTURNCLRV *pv = (FNMTURNCLRV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer */
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
ASIC_HW_IF_STR	*AsicHwReg = Hmsg->pAxRsc->SvAsicRegs->AsicHwReg;		/* <S169> */
/*--------------------------------------------------------------------------------------------------*/
/*		MturnClr (Fn008) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_INIT: 											/* Initial State		*/
/*--------------------------------------------------------------------------------------------------*/
			OPEREG_WRITE( pv->var.OpeSetIdx, MTURNCLRST_PROTECT, Hmsg );
			pv->var.Cnt = 1;
			pv->var.State = FNMTURNCLRST_IDLE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_IDLE: 											/* Servo start state	*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMTURNCLRST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				if( pv->var.Cnt < MTNOPNUM_MAX )
				{
					pv->var.Cnt++;
				}
			}
			else if( OpKeyMake.mode == TRUE )
			{
				//if( pv->var.Cnt != MTNOPNUM_MAX )
				if(( pv->var.Cnt != MTNOPNUM_MAX )||( SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0 ))	/* <S169> */
				{
					pBlinkFlg->no_op = TRUE;
				}
				else
				{
					OPEREG_WRITE( pv->var.OpeSetIdx, MTURNCLRST_EXE, Hmsg );
					pv->var.State = FNMTURNCLRST_MULTITURN;
				}
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_MULTITURN: 										/* Multiturn state		*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal == 1 )
			{
				pBlinkFlg->done = TRUE;
				pv->var.State = FNMTURNCLRST_IDLE;
			}
			else if( pv->var.OperatorStsVal == -1 )
			{
				pBlinkFlg->error = TRUE;
				pv->var.State = FNMTURNCLRST_IDLE;
			}
			else
			{
				/* Do Nothing */
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMTURNCLRST_END: 												/* End state			*/
/*--------------------------------------------------------------------------------------------------*/
			FunMultiTurnResetEnd( Fmsg );
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
		/*led="PGCL "*/
		RpxLedSetStr( (UCHAR *)"PGCL", DspData, 4U, 4U );
		RpxLedSetValDec( (USHORT)pv->var.Cnt, DspData, 0U, 1U, (USHORT)FALSE, 0U );
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
