/*****************************************************************************
Description		: Current Offset Manual Adjustment
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunCurrentManAdjMain();					:Fn00F Current Offset Manual Adjustment mode Display
	FunCurrentManAdjBegin();				:Fn00F Current Offset Manual Adjustment mode Begin
	FunCurrentManAdjEnd();					:Fn00F Current Offset Manual Adjustment mode End

	-- Locals --
	LpxLcdCurrentManAdjExe();				:LCD Operator Fn00F Execution
	LpxPnlCurrentManAdjExe();				:PANEL Operator Fn00F Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.06.09		created
SL.Xue		2013.12.09		up、downについての処理を修正した
T.Masuo		2013.12.27		不具合修正No. A257 対応	<S0A9>
*****************************************************************************/
#include	"FunCurManuAdj.h"


/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNCURMANUADJ_MODE		(0x100FU) 		/* CurManuAdj Mode */
#define FNNORMAL_MODE			(0x0000U)		/* normal mode	*/

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define CURMANUADJCMD_UP 		(0x2)			/* Up Adjustment */
#define CURMANUADJCMD_DOWN 		(0x3)			/* Down Adjustment */
#define CURMANUADJCMD_U 		(0x5)			/* U Select */
#define CURMANUADJCMD_V	 		(0x6)			/* V Select */

/*--------------------------------------------------------------------------------------------------*/
/* Un Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define UNCURUPHZADJ_REG1		(0x0511)		/* 0x511:（U相）Electric Data */
#define UNCURVPHZADJ_REG1		(0x0512)		/* 0x512:（V相）Electric Data */

/*--------------------------------------------------------------------------------------------------*/
/* Fn00F Current Offset Manual Adjustment															*/
/*--------------------------------------------------------------------------------------------------*/
#define ZADJI_MAX				(511)			/* adjustment range min		*/
#define ZADJI_MIN				(-512)			/* adjustment range max		*/

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define UNREG_WRITE( idx, data, hmsg )   (CUNREGX->Write( (COBJECT *)CUNREGX, (idx), (data), hmsg ))
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))  /* <S073> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunCurrentManAdjBegin(FUNMSG *Fmsg); 				/* Fn00F CurManuAdj Mode Begin				*/
LONG FunCurrentManAdjMain(FUNMSG *Fmsg);				/* Fn00F CurManuAdj Mode Main				*/
void FunCurrentManAdjEnd(FUNMSG *Fmsg);					/* Fn00F CurManuAdj Mode End				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdCurrentManAdjExe(FUNMSG *Fmsg);		/* LCD Operator Fn00F Execution 			*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlCurrentManAdjExe( x ) (1)
#else
static LONG LpxPnlCurrentManAdjExe(FUNMSG *hMsg);		/* Panel Operator Fn00F Execution 			*/
#endif

/*****************************************************************************
  Description: Fn00F CurManuAdj Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunCurrentManAdjMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdCurrentManAdjExe( Fmsg );
	}
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlCurrentManAdjExe( Fmsg );
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
  Description: Fn00F CurManuAdj Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunCurrentManAdjBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNCURMANUADJV  *pv = (FNCURMANUADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNCURMANUADJST_U;
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
/* <S0A9> : Delete Start */
#if 0
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,					/* Search Register (Un Read)	*/
					UNCURUPHZADJ_REG1,
					0,
					&(pv->var.UnReadIdxU),
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
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,					/* Search Register (Un Read)	*/
					UNCURVPHZADJ_REG1,
					0,
					&(pv->var.UnReadIdxV),
					MCHK_REG_READ,
					Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
#endif
/* <S0A9> : Delete End */

/*--------------------------------------------------------------------------------------------------*/
/* Set CurManuAdj Mode to Mode Register 															*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNCURMANUADJ_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn00F CurManuAdj Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunCurrentManAdjEnd(FUNMSG *Fmsg)
{
FNCURMANUADJV  *pv = (FNCURMANUADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn00F Current Offset Manual Adjustment Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunCurrentManualAdjustDisplay( FUNMSG *Fmsg )
{
	FNCURMANUADJV *pv = (FNCURMANUADJV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA *Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			zadjiu_tmp;
	LONG			zadjiv_tmp;
/* <S0A9> : Add End */


/* <S0A9> : Delete Start */
#if 0
	UNREG_READX( pv->var.UnReadIdxU, (ULONG *)( &(pv->var.ResZadjiuData) ), MCHK_REG_READ, Hmsg );
	UNREG_READX( pv->var.UnReadIdxV, (ULONG *)( &(pv->var.ResZadjivData) ), MCHK_REG_READ, Hmsg );
#endif
/* <S0A9> : Delete End */

	/* Contents		*/
	RpxLcdSetStr( 3U, 2U, (UCHAR *)"ZADJIU=", ResBuf );
	RpxLcdSetStr( 4U, 2U, (UCHAR *)"ZADJIV=", ResBuf );
/* <S0A9> : Delete Start */
#if 0
	RpxLcdSetSignedDecData( 3U, 9U, pv->var.ResZadjiuData, 5U, (USHORT)FALSE, 0, ResBuf );
	RpxLcdSetSignedDecData( 4U, 9U, pv->var.ResZadjivData, 5U, (USHORT)FALSE, 0, ResBuf );
#endif
/* <S0A9> : Delete End */

/* <S0A9> : Add Start */

				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				pv->var.DispZadjiuData = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				pv->var.DispZadjivData = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

	RpxLcdSetSignedDecData( 3U, 9U, pv->var.DispZadjiuData, 5U, (USHORT)FALSE, 0, ResBuf );
	RpxLcdSetSignedDecData( 4U, 9U, pv->var.DispZadjivData, 5U, (USHORT)FALSE, 0, ResBuf );
/* <S0A9> : Add End */

	if( pv->var.State == FNCURMANUADJST_ADJU )
	{
		RpxLcdClrLineBlnk( 4U, ResBuf );
		RpxLcdSetBlnk( 3U, 14U, 1U, ResBuf );
	}
	else if( pv->var.State == FNCURMANUADJST_ADJV )
	{
		RpxLcdClrLineBlnk( 3U, ResBuf );
		RpxLcdSetBlnk( 4U, 14U, 1U, ResBuf );
	}
	else
	{
		/* do nothing*/
	}
}


/*****************************************************************************
  Description: LCD Operator Fn00F Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdCurrentManAdjExe(FUNMSG *Fmsg)
{
FNCURMANUADJV *pv = (FNCURMANUADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
UCHAR *ResBuf = Fmsg->pTxBuf;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA *Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			zadjiu_tmp;
	LONG			zadjiv_tmp;
/* <S0A9> : Add End */

/*--------------------------------------------------------------------------------------------------*/
		/* clear Tx buffer */
		RpxLcdClrTxBuf( ResBuf );
		/* set title */
		RpxLcdSetStr(1U, 0U, (UCHAR *)"Manual Offset-ADJ", ResBuf);
		RpxLcdSetStr(2U, 1U, (UCHAR *)"of Motor Current", ResBuf);
/*--------------------------------------------------------------------------------------------------*/
/*		CurManuAdj (Fn00F) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_U: 													/*  U state			*/
/*--------------------------------------------------------------------------------------------------*/
			pv->var.State = FNCURMANUADJST_ADJU;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_ADJU: 												/* AdjU state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNCURMANUADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_UP, Hmsg );	/* <S0A9> Delete */

				/* <S0A9> : Add Start */
				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp < ZADJI_MAX)
				{
					zadjiu_tmp ++;
				}

				/* PnE50:電流検出ゼロ調(U相)に更新した値をRAMへ書き込み */
				Prm->zadjiu = (SHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
				pv->var.DispZadjiuData = zadjiu_tmp;
				/* <S0A9> : Add End */

			}
			else if ( OpKeyMake.down == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_DOWN, Hmsg );	/* <S0A9> Delete */

			/* <S0A9> : Add Start */
				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp > ZADJI_MIN)
				{
					zadjiu_tmp --;
				}
				/* PnE50:電流検出ゼロ調(U相)に更新した値をRAMへ書き込み */
				Prm->zadjiu = (USHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
				pv->var.DispZadjiuData = zadjiu_tmp;
			/* <S0A9> : Add End */
			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_V, Hmsg );	/* <S0A9> Delete */
				pv->var.State = FNCURMANUADJST_ADJV;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;
				/*  PnE50:電流検出ゼロ調(U相)をEepRomへ書き込み */		/* <S0A9> */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjiu, 0 );		/* <S0A9> */
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_ADJV: 												/* AdjV state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNCURMANUADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_UP, Hmsg );	/* <S0A9> Delete */

			/* <S0A9> : Add Start */
				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

				if(zadjiv_tmp < ZADJI_MAX)
				{
					zadjiv_tmp ++;
				}
				/* PnE51:電流検出ゼロ調(V相)に更新した値をRAMへ書き込み */
				Prm->zadjiv = (SHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
				pv->var.DispZadjivData = zadjiv_tmp;
			/* <S0A9> : Add End */
			}
			else if ( OpKeyMake.down == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_DOWN, Hmsg );	/* <S0A9> Delete */

			/* <S0A9> : Add Start */
				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

				if(zadjiv_tmp > ZADJI_MIN)
				{
					zadjiv_tmp --;
				}
				/* PnE51:電流検出ゼロ調(U相)に更新した値をRAMへ書き込み */
				Prm->zadjiv = (USHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
				pv->var.DispZadjivData = zadjiv_tmp;
			/* <S0A9> : Add End */

			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_U, Hmsg );	/* <S0A9> Delete */
				pv->var.State = FNCURMANUADJST_ADJU;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;
				/* PnE51:電流検出ゼロ調(V相)をEepRomへ書き込み */		/* <S0A9> */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjiv, 0 );		/* <S0A9> */
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_END: 												/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentManAdjEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		FunCurrentManualAdjustDisplay( Fmsg );
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn00F Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlCurrentManAdjExe(FUNMSG *Fmsg)
{
FNCURMANUADJV *pv = (FNCURMANUADJV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	PRMDATA *Prm = pAxRsc->UniPrms->Prm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			zadjiu_tmp;
	LONG			zadjiv_tmp;
/* <S0A9> : Add End */

/*--------------------------------------------------------------------------------------------------*/
/*		CurManuAdj (Fn00F) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_U: 												/* Panel U state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNCURMANUADJST_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNCURMANUADJST_V;
			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNCURMANUADJST_ADJU;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_V: 												/* Panel V state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNCURMANUADJST_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNCURMANUADJST_U;
			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNCURMANUADJST_ADJV;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_ADJU: 											/* Panel AdjU state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNCURMANUADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_UP, Hmsg );	/* <S0A9> Delete */
				/* <S0A9> : Add Start */
				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp < ZADJI_MAX)
				{
					zadjiu_tmp ++;
				}

				/* PnE50:電流検出ゼロ調(U相)に更新した値をRAMへ書き込み */
				Prm->zadjiu = (SHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
//				pv->var.DispZadjiuData = zadjiu_tmp;		/* <S142> */
				/* <S0A9> : Add End */

			}
			else if ( OpKeyMake.down == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_DOWN, Hmsg );	/* <S0A9> Delete */

			/* <S0A9> : Add Start */
				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp > ZADJI_MIN)
				{
					zadjiu_tmp --;
				}

				/* PnE50:電流検出ゼロ調(U相)に更新した値をRAMへ書き込み */
				Prm->zadjiu = (USHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
//				pv->var.DispZadjiuData = zadjiu_tmp;		/* <S142> */
			/* <S0A9> : Add End */

			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNCURMANUADJST_U;
			}
			/* <S0A9> Add Start */
			else if ( OpKeyMake.mode == TRUE )
			{
				/*  PnE50:電流検出ゼロ調(U相)をEepRomへ書き込み */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjiu, 0 );
			}
			/* <S0A9> Add End */
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_ADJV: 											/* Panel AdjV state		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNCURMANUADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_UP, Hmsg );	/* <S0A9> Delete */

			/* <S0A9> : Add Start */
				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

				if(zadjiv_tmp < ZADJI_MAX)
				{
					zadjiv_tmp ++;
				}
				/* PnE51:電流検出ゼロ調(V相)に更新した値をRAMへ書き込み */
				Prm->zadjiv = (SHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
//				pv->var.DispZadjivData = zadjiv_tmp;		/* <S142> */
			/* <S0A9> : Add End */

			}
			else if ( OpKeyMake.down == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, CURMANUADJCMD_DOWN, Hmsg );	/* <S0A9> Delete */
			/* <S0A9> : Add Start */
				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

				if(zadjiv_tmp > ZADJI_MIN)
				{
					zadjiv_tmp --;
				}
				/* PnE51:電流検出ゼロ調(U相)に更新した値をRAMへ書き込み */
				Prm->zadjiv = (USHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, pAxRsc, 0 );	/* <S1B6> */

				/* 表示データ更新 */
//				pv->var.DispZadjivData = zadjiv_tmp;		/* <S142> */
			/* <S0A9> : Add End */

			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNCURMANUADJST_V;
			}
			/* <S0A9> Add Start */
			else if ( OpKeyMake.mode == TRUE )
			{
				/* PnE51:電流検出ゼロ調(V相)をEepRomへ書き込み */
				PrmStoreValueToEeprom( RegCfg, &pndef_zadjiv, 0 );
			}
			/* <S0A9> Add End */
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNCURMANUADJST_END: 												/* End state		*/
/*--------------------------------------------------------------------------------------------------*/
			FunCurrentManAdjEnd( Fmsg );
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
		switch( pv->var.State )
		{
		case FNCURMANUADJST_U:
			RpxLedSetStr( (UCHAR *)"Cu1_o", DspData, 4U, 5U );
			break;
		case FNCURMANUADJST_V:
			RpxLedSetStr( (UCHAR *)"Cu2_o", DspData, 4U, 5U );
			break;
		case FNCURMANUADJST_ADJU:
			//RpxLedDispMonDecWord( (USHORT)( pv->var.ResZadjiuData & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			pv->var.DispZadjiuData = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);										/* <S142> */
			RpxLedDispMonDecWord( (USHORT)( pv->var.DispZadjiuData & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Add */
			break;
		case FNCURMANUADJST_ADJV:
			//RpxLedDispMonDecWord( (USHORT)( pv->var.ResZadjivData & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			pv->var.DispZadjivData = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);										/* <S142> */
			RpxLedDispMonDecWord( (USHORT)( pv->var.DispZadjivData & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
