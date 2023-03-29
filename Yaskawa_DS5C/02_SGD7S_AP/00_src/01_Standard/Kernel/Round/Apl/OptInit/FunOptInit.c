/*****************************************************************************
Description		:Fn014 Option Card Detection Clear Mode
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	:
	-- APIs --
	FunOptCardDetClearBegin();						:Fn014 Option Card Detection Clear Mode Begin
	FunOptCardDetClearMain();						:Fn014 Option Card Detection Clear Mode Display
	FunOptCardDetClearEnd();						:Fn014 Option Card Detection Clear Mode End
	-- Locals --
	Digital Operator:
	LpxLcdOptCardDetClearExe();						:Fn014 Option Card Detection Clear Mode Execute
	FunOptCardDetClearDisplay();					:Fn014 Option Card Detect Clear Display Data
	FunLcdOptCardDisplay();							:Fn014 LCD Option Card Display
	FunOptCardDetClearInit();						:Fn014 Option Card Detect Clear Initial

	Panel Operator:
	LpxPnlOptCardDetClearExe();						:Fn014 Option Card Detection Clear Mode Execute
	FunPnlOptCardDetClearDisplay();					:Fn014 Option Card Detection Clear Panel Display Data
-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
L.CHEN		2013.06.07		created

*****************************************************************************/

#include	"FunOptInit.h"	/* Servo Operation */
#include	"Monitor.h"

/* defines */
/*--------------------------------------------------------------------------------------------------*/
/* 待ち時間	2000ms 																				*/
/*--------------------------------------------------------------------------------------------------*/
//#define WAITTM_ONE_SENCOND			(1000U)/* <S0BE> */
#define WAITTM_ONE_SENCOND			(2000U)/* <S0BE> */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
#define PRMOPDETSW_REG		(0x0E09)		/* オプションカード対応制限スイッチレジスタ スタブ定数	*/
#define OPTCARDNO_REG		(0x00D0)		/* 0x20D0 : オプションカード選択レジスタ スタブ定数		*/
#define	OPTCARDINFO_REG		(0x051A)		/* 0xE51A : オプションカード検出情報 	*/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode																					*/
/*--------------------------------------------------------------------------------------------------*/
#define FNNORMAL_MODE				(0x0000U) 		/* Normal Mode		*/
#define FNOPTCARDDETCLEAR_MODE		(0x1014U)		/*Option Card Detection Clear Mode		*/

#define OPTIONCARDCLEAR				(0x0001U)		/* オプションカードの検出結果クリア		*/
/*--------------------------------------------------------------------------------------------------*/
#define FUN_CONTINUE				0L				/* Continue				*/
#define FUN_END						1L				/* End					*/
/*--------------------------------------------------------------------------------------------------*/
/* Option Clear Command 																			*/
/*--------------------------------------------------------------------------------------------------*/
#define		OPTCLR_BIT_COMMAND		0x0001U		/* COMMAND Mode			*/
#define		OPTCLR_BIT_SAFE			0x0002U		/* SAFE Mode			*/
#define		OPTCLR_BIT_FB			0x0004U		/* FB Mode				*/
#define		OPTCLR_BIT_ALL			0x0007U		/* ALL Mode				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))

#define STSREG_READX( idx, pResData, CheckType, hmsg )   (COPEREG->Read( COPEREG, (idx), (pResData),\
																					(CheckType), hmsg ))   /* <S073> */
#define UNREG_READX( idx, pResData, CheckType, hmsg )    (CUNREGX->Read( (COBJECT *)(CUNREGX), (idx),\
															 	 	 (pResData), (CheckType), hmsg ))	/*<S054>*/  /* <S073> */
/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/* APIs */
LONG FunOptCardDetClearBegin(FUNMSG *Fmsg);				/* Fn014 Option Card Detection Clear Mode Begin			*/
LONG FunOptCardDetClearMain(FUNMSG *Fmsg);				/* Fn014 Option Card Detection Clear Mode Main			*/
void FunOptCardDetClearEnd(FUNMSG *Fmsg);				/* Fn014 Option Card Detection Clear Mode End			*/

/* Locals */
static LONG LpxLcdOptCardDetClearExe(FUNMSG *Fmsg);				/* Fn014 Option Card Detection Clear Mode Exe		*/
static void	FunOptCardDetClearDisplay( FUNMSG *Fmsg );			/* Fn014 Option Card Detection Clear Display Data	*/
static void FunLcdOptCardDisplay( USHORT Line, USHORT Type, UCHAR *ResBuf );	/* Fn014 LCD Option Card Display	*/
static void FunOptCardDetClearInit( FUNMSG *Fmsg );				/* Fn014 Option Card Detection Clear initia			*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlOptCardDetClearExe( x ) (0)
#else
LONG LpxPnlOptCardDetClearExe(FUNMSG *hMsg);					/* Fn014 Option Card Detection Clear Mode Execute	(Panel Operator)	*/
static void	FunPnlOptCardDetClearDisplay( FUNMSG *Fmsg ); 		/* Fn014 Option Card Detection Clear Panel Display Data					*/
#endif

/*******************************************************************************
  Description: Fn014 Option Card Detection Clear Main
--------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end 
--------------------------------------------------------------------------------
  Note:

*********************************************************************************/
LONG FunOptCardDetClearMain(FUNMSG *Fmsg)
{
	MSGIF *hMsg;
	LONG rc;

	hMsg = Fmsg->Super;
	rc = FUN_CONTINUE;

/*------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) == TRUE)						/* LCD Operator */
	{
		rc = LpxLcdOptCardDetClearExe( Fmsg );
	}
/*------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) == TRUE)				/* Panel Operator	*/
	{
		rc = LpxPnlOptCardDetClearExe( Fmsg );
	}
	else
	{
		/*do nothing		*/	
	}
/*------------------------------------------------------------------------------*/
	return rc;
}

/*********************************************************************************
  Description: Fn014 Option Card Detection Clear Mode Begin
----------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			OK: PrgJogBegin Success, Other: Failure
----------------------------------------------------------------------------------
  Note:

*********************************************************************************/
LONG FunOptCardDetClearBegin(FUNMSG *Fmsg)
{
	LONG	rc;
	FNOPTCARDDETCLEARV  *pv;
	MSGIF *Hmsg;
	BOOL ResFlag;

	ResFlag = TRUE;
	rc = OK;
	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);
	Hmsg = Fmsg->Super;
/*------------------------------------------------------------------------------*/
	pv->var.State = OPTCLR_SEQ_INIT;
//	FunOptCardDetClearInit(Fmsg);		/* <S0BE>:暫定 */
/*------------------------------------------------------------------------------*/
/*	Search Register																*/
/*------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 		/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->var.OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK )
	{
		ResFlag = FALSE;
	}
/*------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,	/* Search Register (Operation Command)	*/
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
/*------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = COPEREG->Search( COPEREG,		/* Search Register (Option Card NO)	*/
				OPTCARDNO_REG,
				0,
				&(pv->var.OptCardNoIdx),
				MCHK_REG_READ,
				Hmsg);
		if( rc != OK )
		{
			ResFlag = FALSE;
		}
	}
/*------------------------------------------------------------------------------*/
/* Monitor Table Search														*/
/*------------------------------------------------------------------------------*/
	if ( ResFlag == TRUE )
	{
		rc = CUNREGX->Search( (COBJECT *)CUNREGX,		/* Search Register (0xE51A : オプションカード検出情報)*/
				OPTCARDINFO_REG,
				0,
				&(pv->var.OptCardInfoIdx),
				MCHK_REG_READ,
				Hmsg);
	}

/*------------------------------------------------------------------------------*/
/* Fn014 Option Card Detection Clear initial			 						*/
/*------------------------------------------------------------------------------*//* <S0BE>:暫定 */
	FunOptCardDetClearInit(Fmsg);

/*------------------------------------------------------------------------------*/
/* Set  Option Card Detection Clear Mode to Mode Register 						*/
/*------------------------------------------------------------------------------*/
	if( ResFlag == TRUE )
	{
		rc = OPEREG_WRITE( pv->var.OpeModeIdx, FNOPTCARDDETCLEAR_MODE, Hmsg );
	}
/*----------------------------------------------------------------------------*/
	return (rc);
}

/*****************************************************************************
  Description:	Fn014 Option Card Detection Clear Mode End
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void FunOptCardDetClearEnd(FUNMSG *Fmsg)
{
	FNOPTCARDDETCLEARV  *pv;
	MSGIF *Hmsg;

	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);
	Hmsg = Fmsg->Super;

/*---------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->var.OpeModeIdx, FNNORMAL_MODE, Hmsg );
}

/*****************************************************************************
  Description: Fn014 Option Card Detection Clear initial
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunOptCardDetClearInit( FUNMSG *Fmsg )
{
	FNOPTCARDDETCLEARV  *pv;
	MSGIF *Hmsg;

	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);
	Hmsg = Fmsg->Super;

	UNREG_READX(pv->var.OptCardInfoIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg);
//	if ( (pv->var.ResData & 0x000F)== 0x0001 )
	if( (pv->var.ResData & 0x000F) != 0x0000 )/* <S0BE> */
	{
		pv->var.SelMaxNo = 1;
		pv->var.Connected = (SHORT)OPTCLR_BIT_COMMAND;
		pv->var.CardNoMax = OPTCLR_CARD_COMMAND;
		pv->var.CardNoMin = OPTCLR_CARD_COMMAND;
	}

//	if ( (pv->var.ResData & 0x00F0)== 0x0010 )
	if( (pv->var.ResData & 0x00F0) != 0x0000 )/* <S0BE> */
	{
		pv->var.SelMaxNo++;
		pv->var.Connected |= (SHORT)OPTCLR_BIT_SAFE;
		if ( pv->var.CardNoMin == OPTCLR_CARD_NOT )
		{
			pv->var.CardNoMin = OPTCLR_CARD_SAFE;
		}
		pv->var.CardNoMax = OPTCLR_CARD_SAFE;
	}

//	if ( (pv->var.ResData & 0x0F00)== 0x0100 )
	if ( (pv->var.ResData & 0x0F00)!= 0x0000 )/* <S0BE> */
	{
		pv->var.SelMaxNo++;
		pv->var.Connected |= (SHORT)OPTCLR_BIT_FB;
		if ( pv->var.CardNoMin == OPTCLR_CARD_NOT )
		{
			pv->var.CardNoMin = OPTCLR_CARD_FB;
		}
		pv->var.CardNoMax = OPTCLR_CARD_FB;
	}

	pv->var.CardNo = pv->var.CardNoMin;
	return;
}


/*****************************************************************************
  Description:	Fn014 LCD Option Card Display
------------------------------------------------------------------------------
  Parameters:
	Line:			開始行位置
	Type:			Option Card mode
	ResBuf:			応答バッファアドレス
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void FunLcdOptCardDisplay( USHORT Line, USHORT Type, UCHAR *ResBuf )
{
	switch ( Type )
	{
		case OPTCLR_BIT_COMMAND :		/* COMMAND Mode						*/
			RpxLcdSetStr( Line, 3U, (UCHAR *)"Command Opt", ResBuf );
			break;

		case OPTCLR_BIT_SAFE :			/* SAFE Mode						*/
			RpxLcdSetStr( Line, 3U, (UCHAR *)"Safety Opt", ResBuf );
			break;

		case OPTCLR_BIT_FB :			/* FB Mode							*/
			RpxLcdSetStr( Line, 3U, (UCHAR *)"Feedback Opt", ResBuf );
			break;

		case OPTCLR_BIT_ALL :			/* ALL Mode							*/
			RpxLcdSetStr( Line,      3U, (UCHAR *)"Command Opt",  ResBuf );
			RpxLcdSetStr( Line + 1U, 3U, (UCHAR *)"Safety Opt",   ResBuf );
			RpxLcdSetStr( Line + 2U, 3U, (UCHAR *)"Feedback Opt", ResBuf );
			break;

		default :
			break;
	}
}

/*****************************************************************************
  Description:	Fn014 Option Card Detection Clear Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

******************************************************************************/
static void	FunOptCardDetClearDisplay( FUNMSG *Fmsg )
{
	FNOPTCARDDETCLEARV *pv;
	UCHAR *ResBuf;

	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);
	ResBuf = Fmsg->pTxBuf;
	/*-----------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 							/* clear Tx buffer	*/
	/* set title */
//	RpxLcdSetStr( 0U, 7U, (UCHAR *)"-Opt Init-", ResBuf );		/* <S154> */
	RpxLcdSetStr( 0U, 9U, (UCHAR *)"Opt Init", ResBuf );		/* <S154> */

	switch ( pv->var.State )
	{
		case OPTCLR_SEQ_SEL_OP :/* OptCardDetClear Select Operation Mode	*/
			switch ( pv->var.Connected )
			{
				case OPTCLR_BIT_COMMAND_DISPLAY :	/* Only Display COMMAND Mode	*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_COMMAND, ResBuf );
					break;

				case OPTCLR_BIT_SAFE_DISPLAY :		/* Only Display SAFE Mode		*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_SAFE, ResBuf );
					break;

				case OPTCLR_BIT_COMMAND_SAFE_DISPLAY :	/* Display COMMAND And SAFE Mode	*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_COMMAND, ResBuf );
					RpxLcdSetStr( 2U, 0U, (UCHAR *)"02:", ResBuf );
					FunLcdOptCardDisplay( 2U, OPTCLR_BIT_SAFE, ResBuf );
					break;

				case OPTCLR_BIT_FB_DISPLAY :		/* Only Display FB Mode				*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_FB, ResBuf );
					break;

				case OPTCLR_BIT_COMMAND_FB_DISPLAY :	/* Display COMMAND And FB Mode	*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_COMMAND, ResBuf );
					RpxLcdSetStr( 2U, 0U, (UCHAR *)"02:", ResBuf );
					FunLcdOptCardDisplay( 2U, OPTCLR_BIT_FB, ResBuf );
					break;

				case OPTCLR_BIT_SAFE_FB_DISPLAY :	/* Display SAFE And FB Mode		*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_SAFE, ResBuf );
					RpxLcdSetStr( 2U, 0U, (UCHAR *)"02:", ResBuf );
					FunLcdOptCardDisplay( 2U, OPTCLR_BIT_FB, ResBuf );
					break;

				case OPTCLR_BIT_ALL_DISPLAY :		/* Display All Mode				*/
					RpxLcdSetStr( 1U, 0U, (UCHAR *)"01:", ResBuf );
					RpxLcdSetStr( 2U, 0U, (UCHAR *)"02:", ResBuf );
					RpxLcdSetStr( 3U, 0U, (UCHAR *)"03:", ResBuf );
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_ALL, ResBuf );
					break;

				case OPTCLR_BIT_NONE_DISPLAY :
					RpxLcdSetStr( 2U, 0U, (UCHAR *)"NO_NEED_OPT_INIT", ResBuf );		/* <S0BE>:暫定 */
					RpxLcdSetStr( 3U, 02U, (UCHAR *)"Return:[SET]", ResBuf );			/* <S0BE>:暫定 */
					/* fall through */
				default :
					break;
			}
			RpxLcdSetBlnk( (USHORT)(pv->var.LPos), 0U, 17U, ResBuf );
			break;

		case OPTCLR_SEQ_REQWAIT :		/* OptCardDetClear Wait Respons		*/
			switch ( pv->var.CardNo )
			{
				case OPTCLR_CARD_COMMAND :	/* COMMAND Mode				*/
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_COMMAND, ResBuf );
					break;

				case OPTCLR_CARD_SAFE :		/* SAFE Mode				*/
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_SAFE, ResBuf );
					break;

				case OPTCLR_CARD_FB :		/* FB Mode					*/
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_FB, ResBuf );
					break;

				default :
					break;
			}
			RpxLcdSetStr( 2U, 4U, (UCHAR *)"Initialize", ResBuf );
			RpxLcdSetStr( 3U, 2U, (UCHAR *)"Start :[DATA]", ResBuf );
			RpxLcdSetStr( 4U, 2U, (UCHAR *)"Return:[SET]", ResBuf );
			break;

		case OPTCLR_SEQ_CLEAR :		/* OptCardDetClear Clear Operation		*/
		/* fall through */
		case OPTCLR_SEQ_WAIT :		/* OptCardDetClear Sequence Wait		*/
			switch ( pv->var.CardNo )
			{
				case OPTCLR_CARD_COMMAND :		/* COMMAND Mode				*/
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_COMMAND, ResBuf );
					break;

				case OPTCLR_CARD_SAFE :			/* SAFE Mode				*/
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_SAFE, ResBuf );
					break;

				case OPTCLR_CARD_FB :			/* FB Mode					*/
					FunLcdOptCardDisplay( 1U, OPTCLR_BIT_FB, ResBuf );
					break;

				default :
					break;
			}
			RpxLcdSetStr( 2U, 4U, (UCHAR *)"Initialize", ResBuf );
			break;

		case OPTCLR_SEQ_END :					/* OptCardDetClear End		*/
		/* fall through */
		default :
			break;
	}
	return;
}

/*****************************************************************************
  Description:	Fn014 Option Card Detection Clear Mode Execution(Digital Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxLcdOptCardDetClearExe(FUNMSG *Fmsg)
{
	/* 変数の定義		*/
	FNOPTCARDDETCLEARV *pv;
	OPKEYFLG OpKeyMake;
	LONG rc;
	MSGIF *Hmsg;
	BLINKFLG *BlinkFlag;

	/* 変数の初期化		*/
	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);
	OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	rc = FUN_CONTINUE;								/* 0: continue, 1: end */
	Hmsg = Fmsg->Super;
	BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);

	if( OpKeyMake.mode == TRUE )
	{
		pv->var.State = OPTCLR_SEQ_END;
	}

	switch ( pv->var.State )
	{
/*--------------------------------------------------------------------------*/
		case OPTCLR_SEQ_INIT:		/* OptCardDetClear Initial				*/
/*--------------------------------------------------------------------------*/
			pv->var.State = OPTCLR_SEQ_SEL_OP;
			pv->var.LPos = 1U;
			pv->var.SelNo = 1;
			break;
/*--------------------------------------------------------------------------*/
		case OPTCLR_SEQ_SEL_OP:	/* OptCardDetClear Select Operation Mode	*/
/*--------------------------------------------------------------------------*/
			if( OpKeyMake.down == TRUE )
			{
				if ( pv->var.SelNo < pv->var.SelMaxNo )
				{
					pv->var.SelNo++;
				}
				else
				{
					pv->var.SelNo = 1;
				}
				pv->var.LPos = (UCHAR)(pv->var.SelNo);
			}
			else if( OpKeyMake.up == TRUE )
			{
				if ( pv->var.SelNo > 1 )
				{
					pv->var.SelNo--;
				}
				else
				{
					pv->var.SelNo = pv->var.SelMaxNo;
				}
				pv->var.LPos = (UCHAR)(pv->var.SelNo);

			}
			else if( OpKeyMake.data == TRUE )
			{
				switch ( pv->var.Connected )
				{
					case OPTCLR_CARD_COMMAND_CONNECT_ONLY :		/* Only Connect COMMAD Mode		*/
						/* fall through */
					case OPTCLR_CARD_COMMAND_SAFE_CONNECT :		/* Connect COMMAND And SAFE Mode	*/
						/* fall through */
					case OPTCLR_CARD_ALL_CONNECT :				/* Connect All Mode					*/
						pv->var.CardNo = pv->var.SelNo;
						pv->var.State = OPTCLR_SEQ_REQWAIT;		/* <S0BE> */
						break;

					case OPTCLR_CARD_SAFE_CONNECT_ONLY :		/* Only Connect SAFE Mode			*/
						/* fall through */
					case OPTCLR_CARD_SAFE_FB_CONNECT :			/* Connect SAFE And FB Mode			*/
						pv->var.CardNo = pv->var.SelNo + 1;
						pv->var.State = OPTCLR_SEQ_REQWAIT;		/* <S0BE> */
						break;

					case OPTCLR_CARD_FB_CONNECT_ONLY :			/* Only Connect FB Mode				*/
						pv->var.CardNo = OPTCLR_CARD_FB;
						pv->var.State = OPTCLR_SEQ_REQWAIT;		/* <S0BE> */
						break;

					case OPTCLR_CARD_COMMAND_FB_CONNECT :		/* Connect COMMAND And FB Mode		*/
						if ( pv->var.SelNo == OPTCLR_CARD_COMMAND )
						{
							pv->var.CardNo = OPTCLR_CARD_COMMAND;
						}
						else
						{
							pv->var.CardNo = OPTCLR_CARD_FB;
						}
						pv->var.State = OPTCLR_SEQ_REQWAIT;		/* <S0BE> */
						break;

					case OPTCLR_CARD_NONE_CONNECT :				/* Connect Nothing					*/
						/* fall through */
					default :
						pv->var.State = OPTCLR_SEQ_NOPE;		/* <S0BE> */
						break;
				}
//				pv->var.State = OPTCLR_SEQ_REQWAIT;	/* <S0BE> */
			}
			else
			{
				/* do nothing */
			}

			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_REQWAIT:		/* OptCardDetClear Wait Response		*/
/*------------------------------------------------------------------------------*/
			if( OpKeyMake.data == TRUE )
			{
				pv->var.State = OPTCLR_SEQ_CLEAR;
				OPEREG_WRITE( pv->var.OptCardNoIdx, pv->var.CardNo, Hmsg );
				OPEREG_WRITE( pv->var.OpeSetIdx, OPTIONCARDCLEAR, Hmsg );
			}
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_CLEAR:		/* OptCardDetClear Clear Operation			*/
/*------------------------------------------------------------------------------*/
			/*TODO : done 表示操作処理	*/
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResData == FNNORMAL_MODE )
			{
				KlibRstLongTimer( &pv->var.LongTimer );
				BlinkFlag->done = TRUE;
				pv->var.State = OPTCLR_SEQ_WAIT;
			}
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_NOPE:		/* OptCardDetClear No Operation				*/
/*------------------------------------------------------------------------------*//* <S0BE> */
			KlibRstLongTimer( &pv->var.LongTimer );
			BlinkFlag->no_op = TRUE;
			pv->var.State = OPTCLR_SEQ_WAIT;
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_WAIT :		/* OptCardDetClear Sequence Wait			*/
/*------------------------------------------------------------------------------*/
			if ( KlibGetLongTimerMs( pv->var.LongTimer ) > WAITTM_ONE_SENCOND )
			{
				pv->var.State = OPTCLR_SEQ_SEL_OP;
			}
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_END :	/* OptCardDetClear End							*/
/*------------------------------------------------------------------------------*/
			FunOptCardDetClearEnd(Fmsg);
			rc =FUN_END;
			break;

		default:
				break;
	}
	/* 表示データ作成		*/
	FunOptCardDetClearDisplay( Fmsg );

	return rc;

}
#if SVFSEL_PANELOP_USE == 1
/*****************************************************************************
  Description:	Fn014 Option Card Detection Clear Panel Display Data
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	None
------------------------------------------------------------------------------
  Note:

******************************************************************************/
static void	FunPnlOptCardDetClearDisplay( FUNMSG *Fmsg )
{
	FNOPTCARDDETCLEARV *pv;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */

	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);

	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	switch ( pv->var.State )
	{
		case OPTCLR_SEQ_SEL_OP :
			switch ( pv->var.Connected )
			{
				case OPTCLR_BIT_SAFE_DISPLAY :
					/*fall through */
				case OPTCLR_BIT_COMMAND_SAFE_DISPLAY :
					RpxLedSetStr((UCHAR *)"o.safe", DspData, 4U, 6U);
					break;

				case OPTCLR_BIT_FB_DISPLAY :
					/*fall through */
				case OPTCLR_BIT_COMMAND_FB_DISPLAY :
					RpxLedSetStr((UCHAR *)"o.feed", DspData, 4U, 6U);
					break;

				case OPTCLR_BIT_SAFE_FB_DISPLAY :
					/*fall through */
				case OPTCLR_BIT_ALL_DISPLAY :
					if ( pv->var.SelNo < pv->var.SelMaxNo )
					{

						RpxLedSetStr((UCHAR *)"o.safe", DspData, 4U, 6U);
					}
					else
					{
						RpxLedSetStr((UCHAR *)"o.feed", DspData, 4U, 6U);
					}
					break;

				case OPTCLR_BIT_NONE_DISPLAY :
					/*fall through */
				case OPTCLR_BIT_COMMAND_DISPLAY :
					/*fall through */
				default :
					break;
			}
			break;

		case OPTCLR_SEQ_REQWAIT :
		case OPTCLR_SEQ_CLEAR :
		case OPTCLR_SEQ_WAIT :
			RpxLedSetStr((UCHAR *)"o.InIt", DspData, 4U, 6U);
			break;

		case OPTCLR_SEQ_END :
		default :
			break;
	}
}

/*****************************************************************************
  Description:	Fn014 Option Card Detection Clear Mode Execution (Panel Operator)
------------------------------------------------------------------------------
  Parameters:
	Fmsg;			Operator Function Message
  Return:
	LONG			0: continue, 1: end
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static LONG LpxPnlOptCardDetClearExe(FUNMSG *Fmsg)
{
	/* 変数の定義		*/
	FNOPTCARDDETCLEARV *pv;
	OPKEYFLG OpKeyMake;
	LONG rc;
	MSGIF *Hmsg;
	BLINKFLG *BlinkFlag;

	/* 変数の初期化		*/
	pv = (FNOPTCARDDETCLEARV *)(Fmsg->pVar);
	OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);
	rc = FUN_CONTINUE;								/* 0: continue, 1: end */
	Hmsg = Fmsg->Super;
	BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);

	if( OpKeyMake.data == TRUE )
	{
		pv->var.State = OPTCLR_SEQ_END;
	}

	switch ( pv->var.State )
	{
/*--------------------------------------------------------------------------*/
		case OPTCLR_SEQ_INIT:		/* OptCardDetClear Initial				*/
/*--------------------------------------------------------------------------*/
			pv->var.State = OPTCLR_SEQ_SEL_OP;
			pv->var.LPos = 1U;
			pv->var.SelNo = 1;
			break;
/*--------------------------------------------------------------------------*/
		case OPTCLR_SEQ_SEL_OP:	/* OptCardDetClear Select Operation Mode	*/
/*--------------------------------------------------------------------------*/
			if( OpKeyMake.down == TRUE )
			{
				if ( pv->var.SelNo < pv->var.SelMaxNo )
				{
					pv->var.SelNo++;
				}
				else
				{
					pv->var.SelNo = 1;
				}
				pv->var.LPos = (UCHAR)(pv->var.SelNo);
			}
			else if( OpKeyMake.up == TRUE )
			{
				if ( pv->var.SelNo > 1 )
				{
					pv->var.SelNo--;
				}
				else
				{
					pv->var.SelNo = pv->var.SelMaxNo;
				}
				pv->var.LPos = (UCHAR)(pv->var.SelNo);
			}
			else if( OpKeyMake.mode == TRUE )
			{
				switch ( pv->var.Connected )
				{
					case OPTCLR_CARD_COMMAND_CONNECT_ONLY :		/* Only Connect COMMAD Mode		*/
						/* fall through */
					case OPTCLR_CARD_COMMAND_SAFE_CONNECT :		/* Connect COMMAND And SAFE Mode	*/
						/* fall through */
					case OPTCLR_CARD_ALL_CONNECT :				/* Connect All Mode					*/
						pv->var.CardNo = pv->var.SelNo;
						break;

					case OPTCLR_CARD_SAFE_CONNECT_ONLY :		/* Only Connect SAFE Mode			*/
						/* fall through */
					case OPTCLR_CARD_SAFE_FB_CONNECT :			/* Connect SAFE And FB Mode			*/
						pv->var.CardNo = pv->var.SelNo + 1;
						break;

					case OPTCLR_CARD_FB_CONNECT_ONLY :			/* Only Connect FB Mode				*/
						pv->var.CardNo = OPTCLR_CARD_FB;
						break;

					case OPTCLR_CARD_COMMAND_FB_CONNECT :		/* Connect COMMAND And FB Mode		*/
						if ( pv->var.SelNo == OPTCLR_CARD_COMMAND )
						{
							pv->var.CardNo = OPTCLR_CARD_COMMAND;
						}
						else
						{
							pv->var.CardNo = OPTCLR_CARD_FB;
						}
						break;

					case OPTCLR_CARD_NONE_CONNECT :				/* Connect Nothing					*/
						/* fall through */
					default :
						break;
				}
				pv->var.State = OPTCLR_SEQ_REQWAIT;
			}
			else
			{
				/* do nothing */
			}

			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_REQWAIT:		/* OptCardDetClear Wait Response		*/
/*------------------------------------------------------------------------------*/
			if( OpKeyMake.mode == TRUE )
			{
				pv->var.State = OPTCLR_SEQ_CLEAR;
				OPEREG_WRITE( pv->var.OptCardNoIdx, pv->var.CardNo, Hmsg );
				OPEREG_WRITE( pv->var.OpeSetIdx, OPTIONCARDCLEAR, Hmsg );
			}
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_CLEAR:		/* OptCardDetClear Clear Operation			*/
/*------------------------------------------------------------------------------*/
			/*TODO : done 表示操作処理	*/
			STSREG_READX( pv->var.OpeModeIdx, &(pv->var.ResData), MCHK_REG_READ, Hmsg );
			if( pv->var.ResData == FNNORMAL_MODE )
			{
				KlibRstLongTimer( &pv->var.LongTimer );
				BlinkFlag->done = TRUE;
				pv->var.State = OPTCLR_SEQ_WAIT;
			}
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_WAIT :		/* OptCardDetClear Sequence Wait			*/
/*------------------------------------------------------------------------------*/
			if ( KlibGetLongTimerMs( pv->var.LongTimer ) > WAITTM_ONE_SENCOND )
			{
				pv->var.State = OPTCLR_SEQ_SEL_OP;
			}
			break;
/*------------------------------------------------------------------------------*/
		case OPTCLR_SEQ_END :	/* OptCardDetClear End							*/
/*------------------------------------------------------------------------------*/
			FunOptCardDetClearEnd(Fmsg);
			rc =FUN_END;
			break;

		default:
				break;
	}
	/* 表示データ作成		*/
	FunPnlOptCardDetClearDisplay( Fmsg );

	return rc;

}
#endif


/***************************************** end of file ***********************/

