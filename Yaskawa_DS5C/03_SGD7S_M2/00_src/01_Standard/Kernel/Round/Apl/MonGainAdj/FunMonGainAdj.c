/*****************************************************************************
Description		: Fn00D : Analog Monitor Gain Manual Adjustment
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	FunAnlgMonGainManAdjMain();				:Fn00D : Analog Monitor Gain Manual Adjustment mode Main
	FunAnlgMonGainManAdjBegin();			:Fn00D : Analog Monitor Gain Manual Adjustment mode Begin
	FunAnlgMonGainManAdjEnd();				:Fn00D : Analog Monitor Gain Manual Adjustment mode End

	-- Locals --
	FunAnlgMonGainManAdjDisplay();			:Fn00D Analog Monitor Gain Manual Adjustment Display Data
	LpxLcdAnlgMonGainManAdjExe();			:LCD Operator Fn00D Execution
	LpxPnlAnlgMonGainManAdjExe();			:PANEL Operator Fn00D Execution

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
SL.Xue		2013.10.12		created
T.Masuo		2013.12.27		不具合修正No. A196 対応	<S0A9>
*****************************************************************************/

#include	"FunMonGainAdj.h"

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
#define FNMONGAINADJ_MODE		(0x100DU)		/* Analog Monitor Gain Manual Adjustment Mode */

/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define MONGAINADJ_UP			(0x2)			/* Up */
#define MONGAINADJ_DOWN			(0x3)			/* Down */
#define MONGAINADJ_CH1ADJ		(0x5)			/* Ch1 Select*/
#define MONGAINADJ_CH2ADJ		(0x6)			/* Ch2 Select*/

/*--------------------------------------------------------------------------------------------------*/
/* Un Register 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define UNMONGAINCH1ADJ_REG1	(0x050D)		/* 0x50F:dummy */
#define UNMONGAINCH2ADJ_REG1	(0x050E)		/* 0x510:dummy */

/*--------------------------------------------------------------------------------------------------*/
/* Prm Register 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define PNDEF_B_PRM6_REG1		(0x006)			/* 0x006: dummy 機能選択応用スイッチ6 */
#define PNDEF_B_PRM7_REG1		(0x007)			/* 0x007: dummy 機能選択応用スイッチ7 */

/*--------------------------------------------------------------------------------------------------*/
/* Fn00D Analog Monitor Gain Manual Adjustment 														*/
/*--------------------------------------------------------------------------------------------------*/
#define GADJAM_MAX				(127)			/* adjustment range min		*/
#define GADJAM_MIN				(-128)			/* adjustment range max		*/

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
const USHORT fun_anlg_un_gain_tbl[] =
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
LONG FunAnlgMonGainManAdjBegin(FUNMSG *Fmsg);			/* Fn00D : Analog Monitor Gain Manual Adjustment Mode Begin	*/
LONG FunAnlgMonGainManAdjMain(FUNMSG *Fmsg);			/* Fn00D : Analog Monitor Gain Manual Adjustment Mode Main	*/
void FunAnlgMonGainManAdjEnd(FUNMSG *Fmsg);				/* Fn00D : Analog Monitor Gain Manual Adjustment Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static void FunAnlgMonGainManAdjDisplay(FUNMSG *Fmsg);	/* AnlgMonOffManAdj Display Data			*/
static LONG LpxLcdAnlgMonGainManAdjExe(FUNMSG *Fmsg);	/* LCD Operator Fn00D Execution 			*/

#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlAnlgMonGainManAdjExe( x ) (1)
#else
static LONG LpxPnlAnlgMonGainManAdjExe(FUNMSG *hMsg);	/* Panel Operator Fn00D Execution 			*/
#endif

/*****************************************************************************
  Description: Fn00D Analog Monitor Gain Manual Adjustment mode Main
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		0: continue, 1: end 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgMonGainManAdjMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
	LONG rc;
	rc = FUN_CONTINUE;
/*--------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE )					/* LCD Operator */
	{
		rc = LpxLcdAnlgMonGainManAdjExe( Fmsg );
	} 
/*--------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE )				/*Panel Operator*/
	{
		rc = LpxPnlAnlgMonGainManAdjExe( Fmsg );
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
  Description: Fn00D Analog Monitor Gain Manual Adjustment Mode Begin
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		OK: Success, Others: Failure 
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
LONG FunAnlgMonGainManAdjBegin(FUNMSG *Fmsg)
{
LONG	rc = OK;
BOOL ResFlag = TRUE;
FNMONGAINADJHV  *pv = (FNMONGAINADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->var.State = FNMONGAINADJST_CH1;
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
					UNMONGAINCH1ADJ_REG1,
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
					UNMONGAINCH2ADJ_REG1,
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm スイッチ6)			*/
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
		rc = CPARAMX->Search( (COBJECT *)CPARAMX,			/* Search Register (Prm スイッチ7)			*/
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
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNMONGAINADJ_MODE, Hmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/*****************************************************************************
  Description: Fn00D Analog Monitor Gain Manual Adjustment Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
void FunAnlgMonGainManAdjEnd(FUNMSG *Fmsg)
{
FNMONGAINADJHV  *pv = (FNMONGAINADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}


/*****************************************************************************
  Description: Fn00D Analog Monitor Gain Manual Adjustment Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static void FunAnlgMonGainManAdjDisplay(FUNMSG *Fmsg)
{
	USHORT wk_brm;
	USHORT wk_un_no;
	USHORT wk_idx;
	ULONG mondata_tmp;
	CMONTBL *mondef_tmp;
	LONG temp;
	FNMONGAINADJHV *pv = (FNMONGAINADJHV *)(Fmsg->pVar);
	UCHAR *ResBuf = Fmsg->pTxBuf;
	MSGIF *Hmsg = Fmsg->Super;
	CMONDEF	*pMonDef; 							/* MONDEF	*pMonDef; */	/* <S0A9> */

	/* Clear	*/
	RpxLcdClrTxBuf( ResBuf );
	/* TITLE		*/
//	RpxLcdSetStr( 0U, 7U, (UCHAR *)"-Gain ADJ-", ResBuf );			/* <S154> */
	RpxLcdSetStr( 0U, 9U, (UCHAR *)"Gain ADJ", ResBuf );			/* <S154> */
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
	wk_un_no = fun_anlg_un_gain_tbl[wk_brm];
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
	wk_un_no = fun_anlg_un_gain_tbl[wk_brm];
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
	if( pv->var.State == FNMONGAINADJST_CH1ADJ )
	{
		RpxLcdClrLineBlnk( 2U, ResBuf );
		RpxLcdSetBlnk( 1U, 9U, 1U, ResBuf );
	}
	else if( pv->var.State == FNMONGAINADJST_CH2ADJ )
	{
		RpxLcdClrLineBlnk( 1U, ResBuf );
		RpxLcdSetBlnk( 2U, 9U, 1U, ResBuf );
	}
}


/*****************************************************************************
  Description: LCD Operator Fn00D Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
static LONG LpxLcdAnlgMonGainManAdjExe(FUNMSG *Fmsg)
{
FNMONGAINADJHV *pv = (FNMONGAINADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
BLINKFLG *pBlinkFlg = (BLINKFLG *)(Fmsg->pBlnkKick);
LONG rc = FUN_CONTINUE;														/* 0: continue, 1: end	*/
/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	CMN_PRMDATA	*Cprm = pAxRsc->UniPrms->Cprm;

	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			mnag_Ch1_tmp;
	LONG			mnag_Ch2_tmp;
/* <S0A9> : Add End */

/*--------------------------------------------------------------------------------------------------*/
/*		MonGainAdj (Fn00D) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH1: 											/* Ch1 State 			*/
/*--------------------------------------------------------------------------------------------------*/
/* <S0A9> : Delete Start */
#if 0
			UNREG_READX( pv->var.UnReadIdxCh1, (ULONG *)( &(pv->var.ResCh1Data) ), MCHK_REG_READ, Hmsg );
			UNREG_READX( pv->var.UnReadIdxCh2, (ULONG *)( &(pv->var.ResCh2Data) ), MCHK_REG_READ, Hmsg );
#endif
/* <S0A9> : Delete End */
			pv->var.State = FNMONGAINADJST_CH1ADJ;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH1ADJ: 										/* Ch1 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONGAINADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 ++;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(下位byte) */
				mnag_Ch1_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);

				if(mnag_Ch1_tmp < GADJAM_MAX)
				{
					mnag_Ch1_tmp += 1;
				}

				/* PnE59:モニタ1,2ゲイン調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.l = (UCHAR)mnag_Ch1_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh1Data < GADJAM_MAX )
				{
					pv->var.ResCh1Data++;
				UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 --;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(下位byte) */
				mnag_Ch1_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);

				if(mnag_Ch1_tmp > GADJAM_MIN)
				{
					mnag_Ch1_tmp -= 1;
				}

				/* PnE59:モニタ1,2ゲイン調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.l = (UCHAR)mnag_Ch1_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh1Data > GADJAM_MIN )
				{
					pv->var.ResCh1Data--;
					UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_CH2ADJ, Hmsg );
				pv->var.State = FNMONGAINADJST_CH2ADJ;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;

				/* PnE59:モニタ1,2ゲイン調をEepRomへ書き込み */			/* <S0A9> */
				PrmStoreValueToEeprom( RegCfg, &pndef_mnag0_1, 0 );		/* <S0A9> */

			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH2ADJ: 										/* Ch2 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONGAINADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 ++;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(上位byte) */
				mnag_Ch2_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);

				if(mnag_Ch2_tmp < GADJAM_MAX)
				{
					mnag_Ch2_tmp += 1;
				}

				/* PnE59:モニタ1,2ゲイン調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.h = (UCHAR)mnag_Ch2_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh2Data < GADJAM_MAX )
				{
					pv->var.ResCh2Data++;
					UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 --;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(上位byte) */
				mnag_Ch2_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);

				if(mnag_Ch2_tmp > GADJAM_MIN)
				{
					mnag_Ch2_tmp -= 1;
				}

				/* PnE59:モニタ1,2ゲイン調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.h = (UCHAR)mnag_Ch2_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh2Data > GADJAM_MIN )
				{
					pv->var.ResCh2Data--;
					UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.scroll == TRUE )
			{
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_CH1ADJ, Hmsg );
				pv->var.State = FNMONGAINADJST_CH1ADJ;
			}
			else if ( OpKeyMake.data == TRUE )
			{
				pBlinkFlg->done = TRUE;

				/* PnE59:モニタ1,2ゲイン調をEepRomへ書き込み */			/* <S0A9> */
				PrmStoreValueToEeprom( RegCfg, &pndef_mnag0_1, 0 );		/* <S0A9> */
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunAnlgMonGainManAdjEnd( Fmsg );
			rc = FUN_END;
			break;
		default:
			break;
		}
		pv->var.DispAdiData1 = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);		/* <S142> */
		pv->var.DispAdiData2 = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);		/* <S142> */
		FunAnlgMonGainManAdjDisplay( Fmsg );
	return (rc);
}


/*****************************************************************************
  Description: PANEL Operator Fn00D Execution
------------------------------------------------------------------------------
  Parameters:
	Fmsg;		Operator Function Message
  Return:
	LONG;		FUN_CONTINUE: Execution Continue, FUN_END: Execution End
------------------------------------------------------------------------------
  Note:
	
*****************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
static LONG LpxPnlAnlgMonGainManAdjExe(FUNMSG *Fmsg)
{
FNMONGAINADJHV *pv = (FNMONGAINADJHV *)(Fmsg->pVar);
MSGIF *Hmsg = Fmsg->Super;
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);		/* set display data buffer	*/
OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
LONG rc = FUN_CONTINUE;													/* 0: continue, 1: end		*/
/* <S0A9> : Add Start */
	AXIS_HANDLE *pAxRsc = Hmsg->pAxRsc;
	CMN_PRMDATA	*Cprm = pAxRsc->UniPrms->Cprm;

	REGIF_CONFIG_T	*RegCfg = pAxRsc->RegManager->hPnReg;
	LONG			mnag_Ch1_tmp;
	LONG			mnag_Ch2_tmp;
/* <S0A9> : Add End */

/*--------------------------------------------------------------------------------------------------*/
/*		MonGainAdj (Fn00D) Sequence																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->var.State )
		{
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH1: 											/* Ch1 State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONGAINADJST_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONGAINADJST_CH2;
			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONGAINADJST_CH1ADJ;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH2: 											/* Ch2 State 			*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONGAINADJST_END;
			}
			else if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = FNMONGAINADJST_CH1;
			}
			else if( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONGAINADJST_CH2ADJ;
			}
			else
			{
				/* do nothing */
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH1ADJ: 										/* Ch1 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONGAINADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 ++;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(下位byte) */
				mnag_Ch1_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);

				if(mnag_Ch1_tmp < GADJAM_MAX)
				{
					mnag_Ch1_tmp += 1;
				}

				/* PnE59:モニタ1,2ゲイン調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.l = (UCHAR)mnag_Ch1_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh1Data < GADJAM_MAX )
				{
					pv->var.ResCh1Data++;
					UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData1 --;	/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(下位byte) */
				mnag_Ch1_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);

				if(mnag_Ch1_tmp > GADJAM_MIN)
				{
					mnag_Ch1_tmp -= 1;
				}

				/* PnE59:モニタ1,2ゲイン調(下位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.l = (UCHAR)mnag_Ch1_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh1Data > GADJAM_MIN )
				{
					pv->var.ResCh1Data--;
					UNREG_WRITE( pv->var.UnReadIdxCh1, pv->var.ResCh1Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONGAINADJST_CH1;
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_CH2ADJ: 										/* Ch2 Adj State 		*/
/*--------------------------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = FNMONGAINADJST_END;
			}
			else if ( OpKeyMake.up == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 ++;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(上位byte) */
				mnag_Ch2_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);

				if(mnag_Ch2_tmp < GADJAM_MAX)
				{
					mnag_Ch2_tmp += 1;
				}

				/* PnE59:モニタ1,2ゲイン調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.h = (UCHAR)mnag_Ch2_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh2Data < GADJAM_MAX )
				{
					pv->var.ResCh2Data++;
					UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_UP, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.down == TRUE )
			{

		/* <S0A9> Add Start */
				/* 表示データ更新 */
//				pv->var.DispAdiData2 --;		/* <S142> */

				/* PnE59:モニタ1,2ゲイン調(上位byte) */
				mnag_Ch2_tmp = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);

				if(mnag_Ch2_tmp > GADJAM_MIN)
				{
					mnag_Ch2_tmp -= 1;
				}

				/* PnE59:モニタ1,2ゲイン調(上位byte)に更新した値をRAMへ書き込み */
				Cprm->mnag0_1.b.h = (UCHAR)mnag_Ch2_tmp;
		/* <S0A9> Add End */

				/* <S0A9> : Delete Start */
				#if 0
				if( pv->var.ResCh2Data > GADJAM_MIN )
				{
					pv->var.ResCh2Data--;
					UNREG_WRITE( pv->var.UnReadIdxCh2, pv->var.ResCh2Data, Hmsg );
				}
				OPEREG_WRITE( pv->var.OpeSetIdx, MONGAINADJ_DOWN, Hmsg );
				#endif
				/* <S0A9> : Delete End */
			}
			else if ( OpKeyMake.shup == TRUE )
			{
				pv->var.State = FNMONGAINADJST_CH2;
			}
			else
			{
				/*do nothing*/
			}
			break;
/*--------------------------------------------------------------------------------------------------*/
		case FNMONGAINADJST_END: 											/* End state 			*/
/*--------------------------------------------------------------------------------------------------*/
			FunAnlgMonGainManAdjEnd( Fmsg );
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
		case FNMONGAINADJST_CH1:
			RpxLedSetStr( (UCHAR *)"Ch1_G", DspData, 4U, 5U );
			break;
		case FNMONGAINADJST_CH2:
			RpxLedSetStr( (UCHAR *)"Ch2_G", DspData, 4U, 5U );
			break;
		case FNMONGAINADJST_CH1ADJ:
			//RpxLedDispMonDecWord( (USHORT)( pv->var.ResCh1Data & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			pv->var.DispAdiData1 = (LONG)(CHAR)((Cprm->mnag0_1.b.l) & 0xFF);		/* <S142> */
			RpxLedDispMonDecWord( (USHORT)( pv->var.DispAdiData1 & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Add */
			break;
		case FNMONGAINADJST_CH2ADJ:
			//RpxLedDispMonDecWord( (USHORT)( pv->var.ResCh1Data & 0xFFFF ), DspData, 0U, DSPDEF_W_SIGN );	/* <S0A9> Delete */
			pv->var.DispAdiData2 = (LONG)(CHAR)((Cprm->mnag0_1.b.h) & 0xFF);		/* <S142> */
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
