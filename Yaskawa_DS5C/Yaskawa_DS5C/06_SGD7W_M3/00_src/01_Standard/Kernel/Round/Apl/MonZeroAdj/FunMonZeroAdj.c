/*****************************************************************************
Description		: Fn00C : Analog Monitor Offset Manual Adjustment
-----------------------------------------------------------------------------
Author		: S154
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunAnlgMonOffManAdjMain();				:Fn00C : Analog Monitor Offset Manual Adjustment mode Main
	FunAnlgMonOffManAdjBegin();				:Fn00C : Analog Monitor Offset Manual Adjustment mode Begin
	FunAnlgMonOffManAdjEnd();				:Fn00C : Analog Monitor Offset Manual Adjustment mode End

	-- Locals --
	FunAnlgMonOffManAdjDisplay();			:Fn00C Analog Monitor Offset Manual Adjustment Display Data
	LpxLcdAnlgMonOffManAdjExe();			:LCD Operator Fn00C Execution
	LpxPnlAnlgMonOffManAdjExe();			:PANEL Operator Fn00C Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.10.11		created
T.Masuo		2013.12.25		不具合修正No. A196 対応	<S0A9>
*****************************************************************************/

#include	"FunMonZeroAdj.h"

/*--------------------------------------------------------------------------------------------------*/
/* extern宣言 																						*/
/*--------------------------------------------------------------------------------------------------*/
extern MSGIF LopMsg;					/* LcdOpe：Message I/F */	/* <S0A9> Add */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1			(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1				(0x0001)		/* 1st Operation Mode Set Register */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE			(0x0000U)		/* Normal Mode	*/
#define FNMONZEROADJ_MODE		(0x100CU)		/* Analog Monitor Offset Manual Adjustment Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define MONZEROADJ_UP			(0x2)			/* Up */
#define MONZEROADJ_DOWN			(0x3)			/* Down */
#define MONZEROADJ_CH1ADJ		(0x5)			/* Ch1 Select*/
#define MONZEROADJ_CH2ADJ		(0x6)			/* Ch2 Select*/

/*--------------------------------------------------------------------------------------------------*/
/* Un Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define UNMONZEROCH1ADJ_REG1	(0x050D)		/* 0x50D:dummy */
#define UNMONZEROCH2ADJ_REG1	(0x050E)		/* 0x50E:dummy */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define PNDEF_B_PRM6_REG1		(0x006)			/* 0x006: dummy 機能選択応用スイッチ6 */
#define PNDEF_B_PRM7_REG1		(0x007)			/* 0x007: dummy 機能選択応用スイッチ7 */

/*--------------------------------------------------------------------------------------------------*/
/* Fn00C Analog Monitor Offset Manual Adjustment 													*/
/*--------------------------------------------------------------------------------------------------*/
#define ZADJAM_MAX				(127)			/* adjustment range min		*/
#define ZADJAM_MIN				(-128)			/* adjustment range max		*/

/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE			(0L)			/*Continue	*/
#define FUN_END					(1L)			/*End		*/

/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )		(COPEREG->Write( COPEREG, (idx), (data), hmsg ))
#define UNREG_WRITE( idx, data, hmsg )   (CUNREGX->Write( (COBJECT *)CUNREGX, (idx), (data), hmsg ))
#define UNREG_READX( idx, pResData, CheckType, hmsg )	(CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
																	(pResData), (CheckType), hmsg ))   /* <S073> */
#define PRMREG_READX( idx, pResData, CheckType, hmsg )	(CPARAMX->Read( (COBJECT *)(CPARAMX),(idx),\
																	(pResData), (CheckType), hmsg ))   /* <S073> */

/*--------------------------------------------------------------------------------------------------*/
/* アナログモニタ表示切替用モニタ番号テーブル														*/
/*--------------------------------------------------------------------------------------------------*/
const USHORT fun_anlg_un_tbl[] =
{
	0x000,
	0x001,
	0x002,
	0x008,
	0x008,
	0x007,
	0x000,
	0x000,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF,
	0xFFF
};

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAnlgMonOffManAdjBegin(FUNMSG *Fmsg);			/* Fn00C Analog Monitor Offset Manual Adjustment Mode Begin	*/
LONG FunAnlgMonOffManAdjMain(FUNMSG *Fmsg);				/* Fn00C Analog Monitor Offset Manual Adjustment Mode Main	*/
void FunAnlgMonOffManAdjEnd(FUNMSG *Fmsg);				/* Fn00C Analog Monitor Offset Manual Adjustment Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static void FunAnlgMonOffManAdjDisplay(FUNMSG *Fmsg);	/* AnlgMonOffManAdj Display Data			*/
static LONG LpxLcdAnlgMonOffManAdjExe(FUNMSG *Fmsg);	/* LCD Operator Fn00C Execution 			*/

#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlAnlgMonOffManAdjExe( x ) (1)
#else
static LONG LpxPnlAnlgMonOffManAdjExe(FUNMSG *hMsg);	/* Panel Operator Fn00C Execution 			*/
#endif

/*****************************************************************************
  Description: Fn00C Analog Monitor Offset Manual Adjustment Mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgMonOffManAdjMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdAnlgMonOffManAdjExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlAnlgMonOffManAdjExe( Fmsg );
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
  Description: Fn00C Analog Monitor Offset Manual Adjustment Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgMonOffManAdjBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNMONZEROADJHV  *pv = (FNMONZEROADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;

/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNMONZEROADJST_CH1;
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
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,			/* Search Register (Un Ch1 Read)			*/
					UNMONZEROCH1ADJ_REG1,
					0,
					&(pv->var.UnReadIdxCh1),
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
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,			/* Search Register (Un Ch2 Read)			*/
					UNMONZEROCH2ADJ_REG1,
					0,
					&(pv->var.UnReadIdxCh2),
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
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm スイッチ6)		*/
				PNDEF_B_PRM6_REG1,
				0,
				&(pv->var.PrmReadIdxPrm6),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm スイッチ7)		*/
				PNDEF_B_PRM7_REG1,
				0,
				&(pv->var.PrmReadIdxPrm7),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/* Set MonZeroAdj Mode to Mode Register 															*/
/*--------------------------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNMONZEROADJ_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn00C Analog Monitor Offset Manual Adjustment Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunAnlgMonOffManAdjEnd(FUNMSG *Fmsg)
{
FNMONZEROADJHV  *pv = (FNMONZEROADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn00C Analog Monitor Offset Manual Adjustment Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static void FunAnlgMonOffManAdjDisplay(FUNMSG *Fmsg)
{
	USHORT wk_brm;
	USHORT wk_un_no;
	USHORT wk_idx;
	ULONG mondata_tmp;
	CMONTBL *mondef_tmp;
	LONG temp;
	FNMONZEROADJHV *pv = (FNMONZEROADJHV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
	CMONDEF	*pMonDef; 							/* MONDEF	*pMonDef; */	/* <S0A9> */

	/* Clear	*/
	RpxLcdClrTxBuf( ResBuf );
	/* TITLE		*/
//	RpxLcdSetStr( 0U, 7U, (UCHAR *)"-Zero ADJ-", ResBuf );			/* <S154> */
	RpxLcdSetStr( 0U, 9U, (UCHAR *)"Zero ADJ", ResBuf );			/* <S154> */
	/* Contents		*/
	RpxLcdSetStr( 1U, 0U, (UCHAR *)"CH1=", ResBuf );
	RpxLcdSetStr( 2U, 0U, (UCHAR *)"CH2=", ResBuf );
	RpxLcdSetStr( 3U, 0U, (UCHAR *)"Un   =", ResBuf );
	RpxLcdSetStr( 4U, 0U, (UCHAR *)"Un   =", ResBuf );
//	RpxLcdSetSignedDecData( 1U, 4U, pv->var.ResCh1Data, 5U, (USHORT)FALSE, 0, ResBuf );		/* <S0A9> Delete */
//	RpxLcdSetSignedDecData( 2U, 4U, pv->var.ResCh2Data, 5U, (USHORT)FALSE, 0, ResBuf );		/* <S0A9> Delete */
	RpxLcdSetSignedDecData( 1U, 4U, pv->var.DispAdiData1, 5U, (USHORT)FALSE, 0, ResBuf );	/* <S0A9> Add */
	RpxLcdSetSignedDecData( 2U, 4U, pv->var.DispAdiData2, 5U, (USHORT)FALSE, 0, ResBuf );	/* <S0A9> Add */

	/* Un表示	*/
	/* アナログモニタ1に該当するモニタ選択番号取得	*/
	PRMREG_READX( pv->var.PrmReadIdxPrm6, &(pv->var.ResPrm6Data), MCHK_REG_READ, Hmsg );
	wk_brm = (USHORT)( pv->var.ResPrm6Data & 0x0000000f );
	wk_un_no = fun_anlg_un_tbl[wk_brm];
	temp = CUNREGX->Search( (COBJECT *)CUNREGX, (LONG)wk_un_no, 0, (LONG *)&(wk_idx), MCHK_ACCS_LVL, Hmsg );
	if( ( wk_un_no != 0xFFF ) && ( temp == OK ) )
	{
		RpxLcdSetDecPnt( 3U, 2U, (ULONG)wk_un_no, 3U, (USHORT)FALSE, 0, ResBuf );
		mondef_tmp = (CMONTBL *)GET_OBJTBL( (COBJECT *)(CUNREGX) );
		UNREG_READX( (LONG)temp, (ULONG *)( &(mondata_tmp) ), MCHK_ACCS_LVL, Hmsg );

		/* <S0A9> Delete Start */
		#if 0
			RpxLcdSetPnUnData( 3U, 6U, mondata_tmp, mondef_tmp->Attribute->DataLength,
					mondef_tmp->Attribute->FormatType, mondef_tmp->Attribute->Sign,
					mondef_tmp->Attribute->Decimals, ResBuf );
		#endif
		/* <S0A9> Delete End */

		/* <S0A9> Add Start */
		/* Unモニタ定義取得 */
		pMonDef = GET_UNMONDEF( CUNMONX->pUnMonTbl, wk_un_no );

		/* Ldata <-- モニタ値	*/
		CUNMONX->Read( (COBJECT *)CUNMONX, wk_un_no, &mondata_tmp, MCHK_ACCS_LVL, &LopMsg );

		/* 表示するUnモニタの値を取得 */
		RpxLcdSetPnUnData(
		3U, 											/* 開始行位置(最上行が0)								*/
		6U,												/* 開始行位置(最上行が0)								*/
		mondata_tmp,									/* 表示データ											*/
		CUNMONX->isLong( CUNMONX, wk_un_no, &LopMsg),	/* ロングパラメータ時TRUE								*/
		CHK_UNBASE( pMonDef ),							/* 底(表示属性)											*/
		pMonDef->Sign, 									/* 符号有無(表示属性)									*/
		CHK_UNDECIMALS( pMonDef ),						/* 小数点以下桁数(表示属性)								*/
		ResBuf											/* 応答バッファアドレス									*/
		);
		/* <S0A9> Add End */
	}
	else
	{ /* 選択したＵｎ番号に対応する表示データが無い場合			*/
		RpxLcdSetStr( 3U, 2U, (UCHAR *)"---", ResBuf );
		RpxLcdSetStr( 3U, 6U, (UCHAR *)"****  ", ResBuf );
	}
	/* アナログモニタ2に該当するモニタ選択番号取得	*/
	PRMREG_READX( pv->var.PrmReadIdxPrm7, &(pv->var.ResPrm7Data), MCHK_REG_READ, Hmsg );
	wk_brm = (USHORT)( pv->var.ResPrm7Data & 0x0000000f );
	wk_un_no = fun_anlg_un_tbl[wk_brm];
	temp = CUNREGX->Search( (COBJECT *)CUNREGX, (LONG)wk_un_no, 0, (LONG *)&(wk_idx), MCHK_ACCS_LVL, Hmsg );
	if( ( wk_un_no != 0xFFF ) && ( temp == OK ) )
	{
		RpxLcdSetDecPnt( 4U, 2U, (ULONG)wk_un_no, 3U, (USHORT)FALSE, 0, ResBuf );
		mondef_tmp = (CMONTBL *)GET_OBJTBL( (COBJECT *)(CUNREGX) );
		UNREG_READX( (LONG)temp, (ULONG *)( &(mondata_tmp) ), MCHK_ACCS_LVL, Hmsg );
		/* <S0A9> Delete Start */
		#if 0
			RpxLcdSetPnUnData( 4U, 6U, mondata_tmp, mondef_tmp->Attribute->DataLength,
					mondef_tmp->Attribute->FormatType, mondef_tmp->Attribute->Sign,
					mondef_tmp->Attribute->Decimals, ResBuf );
		#endif
		/* <S0A9> Delete End */

		/* <S0A9> Add Start */
		/* Unモニタ定義取得 */
		pMonDef = GET_UNMONDEF( CUNMONX->pUnMonTbl, wk_un_no );

		/* Ldata <-- モニタ値	*/
		CUNMONX->Read( (COBJECT *)CUNMONX, wk_un_no, &mondata_tmp, MCHK_ACCS_LVL, &LopMsg );

		/* 表示するUnモニタの値を取得 */
		RpxLcdSetPnUnData(
		4U, 											/* 開始行位置(最上行が0)								*/
		6U,												/* 開始行位置(最上行が0)								*/
		mondata_tmp,									/* 表示データ											*/
		CUNMONX->isLong( CUNMONX, wk_un_no, &LopMsg),	/* ロングパラメータ時TRUE								*/
		CHK_UNBASE( pMonDef ),							/* 底(表示属性)											*/
		pMonDef->Sign, 									/* 符号有無(表示属性)									*/
		CHK_UNDECIMALS( pMonDef ),						/* 小数点以下桁数(表示属性)								*/
		ResBuf											/* 応答バッファアドレス									*/
		);
		/* <S0A9> Add End */

	}
	else
	{ /* 選択したＵｎ番号に対応する表示データが無い場合			*/
		RpxLcdSetStr( 4U, 2U, (UCHAR *)"---", ResBuf );
		RpxLcdSetStr( 4U, 6U, (UCHAR *)"****  ", ResBuf );
	}
	/* 点滅処理	*/
	if( pv->var.State == FNMONZEROADJST_CH1ADJ )
	{
		RpxLcdClrLineBlnk( 2U, ResBuf );
		RpxLcdSetBlnk( 1U, 9U, 1U, ResBuf );
	}
	else if( pv->var.State == FNMONZEROADJST_CH2ADJ )
	{
		RpxLcdClrLineBlnk( 1U, ResBuf );
		RpxLcdSetBlnk( 2U, 9U, 1U, ResBuf );
	}
}


/*****************************************************************************
  Description: LCD Operator Fn00C Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdAnlgMonOffManAdjExe(FUNMSG *Fmsg)
{
FNMONZEROADJHV *pv = (FNMONZEROADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/

/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	CMN_PRMDATA	*Cprm = pAxRsc->UniPrms->Cprm;

	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			mnaz_Ch1_tmp;
	LONG			mnaz_Ch2_tmp;
/* <S0A9> : Add End */

/*--------------------------------------------------------------------------------------------------*/
/*		MonZeroAdj (Fn00C) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH1: 											/* Ch1 State 			*/
/*--------------------------------------------------------------------------------------------------*/
/* <S0A9> : Delete Start */
#if 0
			UNREG_READX( pv->var.UnReadIdxCh1, (ULONG *)( &(pv->var.ResCh1Data) ), MCHK_REG_READ, Hmsg );
			UNREG_READX( pv->var.UnReadIdxCh2, (ULONG *)( &(pv->var.ResCh2Data) ), MCHK_REG_READ, Hmsg );
#endif
/* <S0A9> : Delete End */
			pv->var.State = FNMONZEROADJST_CH1ADJ;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH1ADJ: 										/* Ch1 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONZEROADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
			/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 ++;	/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(下位byte) */
				mnaz_Ch1_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);

				if(mnaz_Ch1_tmp < ZADJAM_MAX)
				{
					mnaz_Ch1_tmp += 1;
				}

				/* PnE58:モニタ1,2ゼロ調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.l = (UCHAR)mnaz_Ch1_tmp;

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh1Data < ZADJAM_MAX )
					{
						pv->var.ResCh1Data++;
						UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 --;		/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(下位byte) */
				mnaz_Ch1_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);

				if(mnaz_Ch1_tmp > ZADJAM_MIN)
				{
					mnaz_Ch1_tmp -= 1;
				}

				/* PnE58:モニタ1,2ゼロ調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.l = (UCHAR)mnaz_Ch1_tmp;

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh1Data > ZADJAM_MIN )
					{
						pv->var.ResCh1Data--;
						UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */

			}
			else if ( OpKeyMake.scroll == TRUE )
			{
//				OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_CH2ADJ, Hmsg );	/* <S0A9> : Delete */
				pv->var.State = FNMONZEROADJST_CH2ADJ;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;

				/* PnE58:モニタ1,2ゼロ調をEepRomへ書き込み */			/* <S0A9> */
				PrmStoreValueToEeprom( RegCfg, &pndef_mnaz0_1, 0 );		/* <S0A9> */
				/* アナログモニタ出力オフセット計算(CH1)	*/	/* <S1B9> */
				PcalAnalogMonitorOffset( pAxRsc, 0 );
				/* アナログモニタ出力処理(CH1&CH2)	*/	/* <S1B9> */
				BpxOutputAnalogMonitor( pAxRsc );
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH2ADJ: 										/* Ch2 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONZEROADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 ++;		/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(上位byte) */
				mnaz_Ch2_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);

				if(mnaz_Ch2_tmp < ZADJAM_MAX)
				{
					mnaz_Ch2_tmp += 1;
				}

				/* PnE58:モニタ1,2ゼロ調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.h = (UCHAR)mnaz_Ch2_tmp;
				/* <S0A9> : Add End */

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh2Data < ZADJAM_MAX )
					{
						pv->var.ResCh2Data++;
						UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 --;		/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(上位byte) */
				mnaz_Ch2_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);

				if(mnaz_Ch2_tmp > ZADJAM_MIN)
				{
					mnaz_Ch2_tmp -= 1;
				}

				/* PnE58:モニタ1,2ゼロ調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.h = (UCHAR)mnaz_Ch2_tmp;
				/* <S0A9> : Add End */

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh2Data > ZADJAM_MIN )
					{
						pv->var.ResCh2Data--;
						UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				//OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_CH1ADJ, Hmsg );	/* <S0A9> : Delete */
				pv->var.State = FNMONZEROADJST_CH1ADJ;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;

				/* PnE58:モニタ1,2ゼロ調をEepRomへ書き込み */			/* <S0A9> */
				PrmStoreValueToEeprom( RegCfg, &pndef_mnaz0_1, 0 );		/* <S0A9> */
				/* アナログモニタ出力オフセット計算(CH2)	*/	/* <S1B9> */
				PcalAnalogMonitorOffset( pAxRsc, 1 );
				/* アナログモニタ出力処理(CH1&CH2)	*/	/* <S1B9> */
				BpxOutputAnalogMonitor( pAxRsc );
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunAnlgMonOffManAdjEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		pv->var.DispAdiData1 = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);		/* <S142> */
		pv->var.DispAdiData2 = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);		/* <S142> */
		FunAnlgMonOffManAdjDisplay( Fmsg );
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn00C Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlAnlgMonOffManAdjExe(FUNMSG *Fmsg)
{
FNMONZEROADJHV *pv = (FNMONZEROADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end		*/
/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	CMN_PRMDATA	*Cprm = pAxRsc->UniPrms->Cprm;
	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			mnaz_Ch1_tmp;
	LONG			mnaz_Ch2_tmp;
/* <S0A9> : Add End */
/*--------------------------------------------------------------------------------------------------*/
/*		MonZeroAdj (Fn00C) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH1: 											/* Ch1 State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONZEROADJST_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONZEROADJST_CH2;
			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONZEROADJST_CH1ADJ;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH2: 											/* Ch2 State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONZEROADJST_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONZEROADJST_CH1;
			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONZEROADJST_CH2ADJ;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH1ADJ: 										/* Ch1 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONZEROADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{

			/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 ++;	/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(下位byte) */
				mnaz_Ch1_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);

				if(mnaz_Ch1_tmp < ZADJAM_MAX)
				{
					mnaz_Ch1_tmp += 1;
				}

				/* PnE58:モニタ1,2ゼロ調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.l = (UCHAR)mnaz_Ch1_tmp;

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh1Data < ZADJAM_MAX )
					{
						pv->var.ResCh1Data++;
						UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */

			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 --;		/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(下位byte) */
				mnaz_Ch1_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);

				if(mnaz_Ch1_tmp > ZADJAM_MIN)
				{
					mnaz_Ch1_tmp -= 1;
				}

				/* PnE58:モニタ1,2ゼロ調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.l = (UCHAR)mnaz_Ch1_tmp;
				/* <S0A9> : Add End */

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh1Data > ZADJAM_MIN )
					{
						pv->var.ResCh1Data--;
						UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONZEROADJST_CH1;
			}
			/* <S0A9> Add Start */
			else if ( OpKeyMake.mode == TRUE )
			{
				/* PnE58:モニタ1,2ゼロ調をEepRomへ書き込み */
				PrmStoreValueToEeprom( RegCfg, &pndef_mnaz0_1, 0 );
			}
			/* <S0A9> Add End */
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_CH2ADJ: 										/* Ch2 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONZEROADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{
				/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 ++;		/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(上位byte) */
				mnaz_Ch2_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);

				if(mnaz_Ch2_tmp < ZADJAM_MAX)
				{
					mnaz_Ch2_tmp += 1;
				}

				/* PnE58:モニタ1,2ゼロ調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.h = (UCHAR)mnaz_Ch2_tmp;
				/* <S0A9> : Add End */

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh2Data < ZADJAM_MAX )
					{
						pv->var.ResCh2Data++;
						UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{
				/* <S0A9> : Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 --;		/* <S142> */

				/* PnE58:モニタ1,2ゼロ調(上位byte) */
				mnaz_Ch2_tmp = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);

				if(mnaz_Ch2_tmp > ZADJAM_MIN)
				{
					mnaz_Ch2_tmp -= 1;
				}

				/* PnE58:モニタ1,2ゼロ調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnaz0_1.b.h = (UCHAR)mnaz_Ch2_tmp;
				/* <S0A9> : Add End */

				/* <S0A9> : Delete Start */
				#if 0
					if( pv->var.ResCh2Data > ZADJAM_MIN )
					{
						pv->var.ResCh2Data--;
						UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
					}
					OPEREG_WRITE( pv->var.OpeSetIdx, MONZEROADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONZEROADJST_CH2;
			}
			/* <S0A9> Add Start */
			else if ( OpKeyMake.mode == TRUE )
			{
				/* PnE58:モニタ1,2ゼロ調をEepRomへ書き込み */
				PrmStoreValueToEeprom( RegCfg, &pndef_mnaz0_1, 0 );
			}
			/* <S0A9> Add End */
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONZEROADJST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunAnlgMonOffManAdjEnd( Fmsg );
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
		case FNMONZEROADJST_CH1:
			RpxLedSetStr( (UCHAR *)"Ch1_O", DspData, 4U, 5U );
			break;
		case FNMONZEROADJST_CH2:
			RpxLedSetStr( (UCHAR *)"Ch2_O", DspData, 4U, 5U );
			break;
		case FNMONZEROADJST_CH1ADJ:
			//RpxLedDispMonDecWord( (USHORT)( pv->var.ResCh1Data & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			pv->var.DispAdiData1 = (LONG)(CHAR)((Cprm->mnaz0_1.b.l) & 0xFF);		/* <S142> */
			RpxLedDispMonDecWord( (USHORT)( pv->var.DispAdiData1 & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Add */

			break;
		case FNMONZEROADJST_CH2ADJ:
			//RpxLedDispMonDecWord( (USHORT)( pv->var.ResCh1Data & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			pv->var.DispAdiData2 = (LONG)(CHAR)((Cprm->mnaz0_1.b.h) & 0xFF);		/* <S142> */
			RpxLedDispMonDecWord( (USHORT)( pv->var.DispAdiData2 & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Add */

			break;
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif


/***************************************** end of file **********************************************/
