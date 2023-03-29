/*****************************************************************************
Description	:Fn020 Scale Origin Setting
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunScaleOrgSetMain();		:Fn020 Scale Origin Setting Mode Display
	FunScaleOrgSetBegin();		:Fn020 Scale Origin Setting Modde Begin
	FunScaleOrgSetEnd();			:Fn020 Scale Origin Setting Mode End

	-- Locals --
	LpxLcdScaleOrgSetExe();		:LCD Operator Fn020 Execution
	FunScaleOrgSetDisplay();	:Fn020 Scale Origin Setting Display( LCD Display )
	LpxPnlScaleOrgSetExe();		:Panel Operator Fn020 Execution
	FunPnlScaleOrgSetDisplay();	:Fn020 Scale Origin Setting Display( Panel Display )

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.06.07		created
K.Liu		2013.08.29		Add Panel Operator Function
K.Liu		2013.12.09		Modify
*****************************************************************************/
#include "FunSOrigSet.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1				(0x0000)			/* 1st Operation Mode Register 		*/
#define OPESET_REG1					(0x0001)			/* 1st Operation Mode Set Register	*/
#define ORGSETST_REG				(0x0033)			/* Origin Set State					*/
#define	OPESTATUS_REG				(0x000F)			/* Operation Display Status Register	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNSCALEORGSET_MODE			(0x1020U) 			/* Scale Origin Setting Mode 		*/
#define FNNORMAL_MODE				(0x0000U)			/* Normal Mode						*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define SCALEORGSETCMD_EXE	 		(0x0001)
#define SCALEORGSETCMD_PROTPROC		(0x0002)			/*	Protection Processing 1					*/
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE				0					/* Continue */
#define FUN_END						1					/* End 		*/

/*--------------------------------------------------------------------------------------------------*/
#define ORGSET_MAX					4U
/*--------------------------------------------------------------------------------------------------*/
/* Fn020 Scale Origin Setting State																	*/
/*--------------------------------------------------------------------------------------------------*/
#define ORGSET_CONTINUE				0U					/* Origin Setting Continue 					*/
#define ORGSET_COMPLETED			1U					/* Origin Setting Completed 				*/
/*--------------------------------------------------------------------------------------------------*/
/*	 Operator Display State																			*/
/*--------------------------------------------------------------------------------------------------*/
enum SCALE_ORG_DISPLAY_ST
{
	FUNSOSET_OPDSP_NONE,
	FUNSOSET_OPDSP_BLINK,
	FUNSOSET_OPDSP_DONE,
	FUNSOSET_OPDSP_NOOP,
	FUNSOSET_OPDSP_ERROR
};

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunScaleOrgSetBegin(FUNMSG *Fmsg); 			/* Fn020 Scale Origin Setting Mode Begin		*/
LONG FunScaleOrgSetMain(FUNMSG *Fmsg);				/* Fn020 Scale Origin Setting Mode Display		*/
void FunScaleOrgSetEnd(FUNMSG *Fmsg);				/* Fn020 Scale Origin Setting Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdScaleOrgSetExe(FUNMSG *Fmsg);						/* LCD Operator Fn020 Execution */
static void FunScaleOrgSetDisplay( FUNMSG *Fmsg, UCHAR OpDspReq );	/* Fn020 Display 				*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlScaleOrgSetExe( x ) (1)
#define FunPnlScaleOrgSetPnlDisplay( x, y )	( )
#else
LONG LpxPnlScaleOrgSetExe(FUNMSG *hMsg);						/* Panel Operator Fn020 Execution 	*/
static void FunPnlScaleOrgSetDisplay( FUNMSG *Fmsg, UCHAR OpDspReq );	/* Panel Display */
#endif

#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData), \
																		(CheckType), hmsg ))  /* <S073> */

/*****************************************************************************
  Description: Fn020 Scale Origin Setting Mode Display
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: Continue, 1: End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunScaleOrgSetMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc = 0;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == (ULONG)(TRUE) )					/* LCD Operator */
	{
		rc = LpxLcdScaleOrgSetExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )						/* Panel Operator */
	{
		rc = LpxPnlScaleOrgSetExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		/* Do Nothing */
	}
/*--------------------------------------------------------------------------------------------------*/
	return rc;
}


/*****************************************************************************
  Description: Fn020 Scale Origin Setting Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
LONG FunScaleOrgSetBegin(FUNMSG *Fmsg)
{
	LONG	rc = OK;
	BOOL ResFlag = TRUE;
	FNSORIGSETV  *pv = (FNSORIGSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNSORIGSETST_ORGSET1;
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
	if ( ResFlag == TRUE)
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
	if ( ResFlag == TRUE)
	{
			rc = COPEREG->Search( COPEREG,						/* Search Register (Operation Read)	*/
					ORGSETST_REG,
					0,
					&(pv->var.OpeReadIdx),
					MCHK_REG_READ,
					Hmsg);
			if( rc != OK )
			{
				ResFlag = FALSE;
			}
	}
	if ( ResFlag == TRUE)
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
/*--------------------------------------------------------------------------------------------------*/
/* Set Fn020 Scale Origin Setting Mode to Mode Register 											*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE)
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNSCALEORGSET_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
//	if ( rc == OK )
//	{	/*	 保護処理 	*/
//		OPEREG_WRITE( pv->var.OpeSetIdx, SCALEORGSETCMD_PROTPROC, Hmsg );
//	}

	return (rc);
}


/*****************************************************************************
  Description: Fn020 Scale Origin Setting Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunScaleOrgSetEnd(FUNMSG *Fmsg)
{
	FNSORIGSETV  *pv = (FNSORIGSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn020 Scale Origin Setting Mode Execution( LCD Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdScaleOrgSetExe(FUNMSG *Fmsg)
{
	FNSORIGSETV *pv = (FNSORIGSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	UCHAR	OpDspReq = (UCHAR)FUNSOSET_OPDSP_NONE;
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*			Scale Origin Setting(Fn020) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNSORIGSETST_ORGSET1: 	/* Fall Through */					/* Origin Setting Select	*/
		case FNSORIGSETST_ORGSET2:	/* Fall Through */
		case FNSORIGSETST_ORGSET3:	/* Fall Through */
		case FNSORIGSETST_ORGSET4:	/* Fall Through */
		case FNSORIGSETST_ORGSET5:
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNSORIGSETST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				if(pv->var.State < ORGSET_MAX)
				{
					pv->var.State++;
				}

				if ( pv->var.State == ORGSET_MAX )
				{	/*	 保護処理 	*/
					OPEREG_WRITE( pv->var.OpeSetIdx, SCALEORGSETCMD_PROTPROC, Hmsg );
				}
			}
			else if ( OpKeyMake.data == TRUE )
			{
				if(pv->var.State != ORGSET_MAX)				/* Disable Execution	*/
				{
					OpDspReq = (UCHAR)FUNSOSET_OPDSP_NOOP;
				}
				else										/* Enable Execution		*/
				{
					if ( OK == OPEREG_WRITE( pv->var.OpeSetIdx, SCALEORGSETCMD_EXE, Hmsg ))
					{
						OpDspReq = (UCHAR)FUNSOSET_OPDSP_BLINK;
						pv->var.State = FNSORIGSETST_WAIT;
					}
					else
					{
						OpDspReq = (UCHAR)FUNSOSET_OPDSP_ERROR;
						pv->var.State = FNSORIGSETST_ORGSET1;
					}

				}
			}
			else
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_NOOP;
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNSORIGSETST_WAIT: 						/* Scale Origin Setting Wait For Completing	*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal  == 1 )
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_DONE;
				pv->var.State = FNSORIGSETST_ORGSET1;
			}
			else if( pv->var.OperatorStsVal  == -1 )
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_ERROR;
				pv->var.State = FNSORIGSETST_ORGSET1;
			}
			else
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_BLINK;
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );

			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNSORIGSETST_END: 													/*	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunScaleOrgSetEnd( Fmsg );
			rc = FUN_END;
			break;

		default:
			break;
		}

	/*	Fn020 Scale Origin Setting Display	*/
	FunScaleOrgSetDisplay( Fmsg, OpDspReq );
	return (rc);
}


/*****************************************************************************
  Description: Fn020 Scale Origin Setting Display( LCD Display )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
	OpDspReq;	Display Mode
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunScaleOrgSetDisplay( FUNMSG *Fmsg, UCHAR OpDspReq )
{
	FNSORIGSETV *pv = (FNSORIGSETV *)(Fmsg->pVar);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	UCHAR	OrgsetStep;

	/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 												/* clear Tx buffer		*/
	RpxLcdSetStr(2U, 1U, (UCHAR *)"Scale Origin Set", ResBuf);				/* set title			*/

	/* ORGSET?のフェーズ番号の決定	*/
	OrgsetStep = (pv->var.State >= 5)?	5U : ( pv->var.State + 1L ) ;

	if (OpDspReq == (UCHAR)FUNSOSET_OPDSP_NOOP)
	{
		BlinkFlag->no_op = TRUE;
	}
	else if (OpDspReq == (UCHAR)FUNSOSET_OPDSP_DONE)
	{
		BlinkFlag->done = TRUE;
	}
	else if (OpDspReq == (UCHAR)FUNSOSET_OPDSP_ERROR)
	{
		BlinkFlag->error = TRUE;
	}
	else
	{
		/* Do Nothing */
	}

	/* Contents		*/
	if(OpDspReq != (UCHAR)FUNSOSET_OPDSP_BLINK)
	{
		RpxLcdSetStr(4U, 5U, (UCHAR *)"ORGSET", ResBuf);
		RpxLcdSetDecPnt(4U, 11U, (ULONG)OrgsetStep, 1U, (USHORT)(FALSE), 0, ResBuf);
	}
	else
	{
		RpxLcdSetStr(4U, 5U, (UCHAR *)"       ", ResBuf);
	}

	/* 実行中状態表示	*/
	if(OpDspReq == (UCHAR)FUNSOSET_OPDSP_BLINK)
	{
		RpxLcdSetBlnk(2U,1U,16U,ResBuf);
	}
	else
	{
		RpxLcdClrLineBlnk(2U,ResBuf);
	}
	return ;
}

/*****************************************************************************
  Description: Fn020 Scale Origin Setting Mode Execution( Panel Operator )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlScaleOrgSetExe(FUNMSG *Fmsg)
{
	FNSORIGSETV *pv = (FNSORIGSETV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	UCHAR	OpDspReq = (UCHAR)FUNSOSET_OPDSP_NONE;
	LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end */

/*--------------------------------------------------------------------------------------------------*/
/*			Scale Origin Setting(Fn020) Sequence													*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->var.State )
	{
/*--------------------------------------------------------------------------------------------------*/
		case FNSORIGSETST_ORGSET1: 	/* Fall Through */					/* Origin Setting Select	*/
		case FNSORIGSETST_ORGSET2:	/* Fall Through */
		case FNSORIGSETST_ORGSET3:	/* Fall Through */
		case FNSORIGSETST_ORGSET4:	/* Fall Through */
		case FNSORIGSETST_ORGSET5:
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNSORIGSETST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				if(pv->var.State < ORGSET_MAX)
				{
					pv->var.State++;
				}
			}
			else if ( OpKeyMake.mode == TRUE )
			{
				if(pv->var.State != ORGSET_MAX)				/* Disable Execution	*/
				{
					OpDspReq = (UCHAR)FUNSOSET_OPDSP_NOOP;
				}
				else										/* Enable Execution		*/
				{
					if ( OK == OPEREG_WRITE( pv->var.OpeSetIdx, SCALEORGSETCMD_EXE, Hmsg ))
					{
						pv->var.State = FNSORIGSETST_WAIT;
					}
					else
					{
						OpDspReq = (UCHAR)FUNSOSET_OPDSP_ERROR;
						pv->var.State = FNSORIGSETST_ORGSET1;
					}

				}
			}
			else
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_NOOP;
			}
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNSORIGSETST_WAIT: 						/* Scale Origin Setting Wait For Completing	*/
/*--------------------------------------------------------------------------------------------------*/
			STSREG_READX(pv->var.OperatorStsIdx, (ULONG*)&(pv->var.OperatorStsVal), MCHK_REG_READ, Hmsg);
			if( pv->var.OperatorStsVal  == 1 )
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_DONE;
				pv->var.State = FNSORIGSETST_ORGSET1;
			}
			else if( pv->var.OperatorStsVal  == -1 )
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_ERROR;
				pv->var.State = FNSORIGSETST_ORGSET1;
			}
			else
			{
				OpDspReq = (UCHAR)FUNSOSET_OPDSP_BLINK;
			}
			OPEREG_WRITE( pv->var.OperatorStsIdx, 0, Hmsg );
			break;

/*--------------------------------------------------------------------------------------------------*/
		case FNSORIGSETST_END: 													/*	End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunScaleOrgSetEnd( Fmsg );
			rc = FUN_END;
			break;

		default:
			break;
		}

	/*	Fn020 Scale Origin Setting Display	( Panel Operator Display ) */
	FunPnlScaleOrgSetDisplay( Fmsg, OpDspReq );
	return (rc);
}


/*****************************************************************************
  Description: Fn020 Scale Origin Setting Display( Panel Operator Display )
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
	OpDspReq;	Display Mode
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunPnlScaleOrgSetDisplay( FUNMSG *Fmsg, UCHAR OpDspReq )
{
	FNSORIGSETV *pv = (FNSORIGSETV *)(Fmsg->pVar);
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
	UCHAR	OrgsetStep;

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/
	/*--------------------------------------------------------------------------------------------------*/
	/* ORGSET?のフェーズ番号の決定	*/
	OrgsetStep = (pv->var.State >= 5)?	5U : ( pv->var.State + 1L ) ;

	if (OpDspReq == (UCHAR)FUNSOSET_OPDSP_NOOP)
	{
		BlinkFlag->no_op = TRUE;
	}
	else if (OpDspReq == (UCHAR)FUNSOSET_OPDSP_DONE)
	{
		BlinkFlag->done = TRUE;
	}
	else if (OpDspReq == (UCHAR)FUNSOSET_OPDSP_ERROR)
	{
		BlinkFlag->error = TRUE;
	}
	else
	{
		/* Do Nothing */
	}

	/*led="ORGSET "*/
	RpxLedSetStr((UCHAR *)"OSET", DspData, 4U, 4U);
	RpxLedSetValDec((USHORT)OrgsetStep, DspData, 0U, 1U, FALSE, 0U);

	return ;
}
#endif	/* #if  SVFSEL_PANELOP_USE == 1 */


/***************************************** end of file **********************************************/
