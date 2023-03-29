/****************************************************************************************************/
/*																									*/
/*																									*/
/*		TestMode.c : Test Mode Function (Fn100, Fn101, Fn102)		<S050>							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Execute Test Mode (Fn100, Fn101, Fn102)												*/
/*			2)Test Mode Display																		*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.10.20  Y.Oka																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include 	"Function.h"	/* Fn Function */
#include	"Operation.h"	/* Servo Operation */

/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
/**
 * Fn100, Fn101, Fn102 Mode Variable
 */
typedef struct _FnTstModV {
	LONG	State;				/* Fn100, Fn101, Fn102  Execution State								*///<S0C7>型を修正
	LONG	OpeModeIdx;			/* Operation Mode Register Index									*///<S0C7>
	LONG	OpeSetIdx;			/* Operation Set Register Index										*///<S0C7>
	ULONG	DispTime;			/* Display Time (Panel Operator)									*///<S0C7>
	LONG	Reserved[(256/4)-4];/* Reserved: All functions can use ram variables at least 256 bytes. *///<S0C7>
} FNTSTMODV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Operation Register 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OPEMODE_REG1		(0x0000)		/* 1st Operation Mode Register */
#define OPESET_REG1			(0x0001)		/* 1st Operation Mode Set Register */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Mode	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define FNTEST_ACON_MODE	(0x1100) 		/* Force ACON Mode */
#define FNTEST_REGOFF_MODE	(0x1101)		/* Regeneration Invalidity Mode */
#define FNTEST_OLMSK_MODE	(0x1102)		/* Overload Mask Mode */
/*--------------------------------------------------------------------------------------------------*/
/* Operation Command 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define TSTMODE_EXE			(0x1)
#define FUN_CONTINUE		(0L)			/*Continue	*/	/* <S206> */
#define FUN_END				(1L)			/*End		*/	/* <S206> */
/*--------------------------------------------------------------------------------------------------*/
#define OPEREG_WRITE( idx, data, hmsg )   (COPEREG->Write( COPEREG, (idx), (data), hmsg ))

/*--------------------------------------------------------------------------------------------------*/
enum _enumFnJogState
{
	TESTMODE_EXE = 0,
	TESTMODE_WAIT,
	TESTMODE_END
};
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
/* Force ACON Mode																					*/
LONG FunTstMod1Begin(FUNMSG *Fmsg); 					/* Fn100 Mode Begin							*/
LONG FunTstMod1Main(FUNMSG *Fmsg);						/* Fn100 Mode Main							*/

/* Regeneration Invalidity Mode																		*/
LONG FunTstMod2Begin(FUNMSG *Fmsg); 					/* Fn101 Mode Begin							*/
LONG FunTstMod2Main(FUNMSG *Fmsg);						/* Fn101 Mode Main							*/

/* Overload Mask Mode																				*/
LONG FunTstMod3Begin(FUNMSG *Fmsg); 					/* Fn102 Mode Begin							*/
LONG FunTstMod3Main(FUNMSG *Fmsg);						/* Fn102 Mode Main							*/

/* Commons */
void FunTstModEnd(FUNMSG *Fmsg);						/* Fn100, Fn101, Fn102 Mode End				*/


/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG LpxLcdTstMod1Exe(FUNMSG *hMsg);					/* LCD Operator Fn100 Execution 			*/
LONG LpxLcdTstMod2Exe(FUNMSG *hMsg);					/* LCD Operator Fn101 Execution 			*/
LONG LpxLcdTstMod3Exe(FUNMSG *hMsg);					/* LCD Operator Fn102 Execution 			*/

#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlTstMode1Exe( x ) (1)
#define LpxPnlTstMode2Exe( x ) (1)
#define LpxPnlTstMode3Exe( x ) (1)
#else
/* パネオペ未対応 → パネオペ対応 */	/* <S206> */
//#define LpxPnlTstMode1Exe( x ) (1)	/* <S206> */
//#define LpxPnlTstMode2Exe( x ) (1)	/* <S206> */
//#define LpxPnlTstMode3Exe( x ) (1)	/* <S206> */
LONG LpxPnlTstMode1Exe(FUNMSG *Fmsg);	/* <S206> */
LONG LpxPnlTstMode2Exe(FUNMSG *Fmsg);	/* <S206> */
LONG LpxPnlTstMode3Exe(FUNMSG *Fmsg);	/* <S206> */
#endif

/****************************************************************************************************/
/*																									*/
/*		Fn100 Mode Display																			*/
/*																									*/
/****************************************************************************************************/
LONG FunTstMod1Main(FUNMSG *Fmsg)
{
	MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdTstMod1Exe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlTstMode1Exe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return TRUE;
}


/****************************************************************************************************/
/*																									*/
/*		Fn100 Mode Begin																			*/
/*																									*/
/****************************************************************************************************/
LONG FunTstMod1Begin(FUNMSG *Fmsg)
{
	LONG		rc = OK;
	FNTSTMODV	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF		*Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->State = TESTMODE_EXE;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG,							/* Search Register (Operation Command)	*/
				OPESET_REG1,
				0,
				&(pv->OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
/* Set Fn100 Mode to Mode Register 																	*/
/*--------------------------------------------------------------------------------------------------*/
	rc = OPEREG_WRITE( pv->OpeModeIdx, FNTEST_ACON_MODE, Hmsg );
	if ( rc != OK ){ return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &(pv->DispTime) );						/* Panel Operator 						*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn100 Mode Execution (LCD Operator)															*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdTstMod1Exe(FUNMSG *Fmsg)
{
	FNTSTMODV 	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF 		*Hmsg = Fmsg->Super;
	UCHAR 		*ResBuf = Fmsg->pTxBuf;//<S0C7>
//	LONG  		IgnrBlnk = FALSE;//<S0C7>
	OPKEYFLG 	OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);
//	OPKEYFLG 	OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);//<S0C7>
	LONG 		rc = 0;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 											/* clear Tx buffer			*/
//	RpxLcdSetStr(0, 9, (UCHAR *)"-Test 1-", ResBuf );					/* set title				*/		/* <S154> */
//	RpxLcdSetStr(0, 11, (UCHAR *)"Test 1", ResBuf );					/* set title				*/		/* <S154> *//* <S207> */
//	RpxLcdSetStr(1, 5, (UCHAR *)"-AC ON-", ResBuf );					/* set title				*/		/* <S207> */
//	RpxLcdSetStr(2, 3, (UCHAR *)"-Activated!-", ResBuf );				/* set title				*/		/* <S207> */
	RpxLcdSetStr(0, 9, (UCHAR *)"-Test 1-", ResBuf );					/* set title				*/		/* <S207> */
	RpxLcdSetStr(1, 5, (UCHAR *)"AC ON", ResBuf );						/* set title				*/		/* <S207> */
	RpxLcdSetStr(2, 3, (UCHAR *)"Activated!", ResBuf );					/* set title				*/		/* <S207> */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetStr(3, 0, (UCHAR *)"                 ", ResBuf);			/* set contents				*/
	RpxLcdSetStr(4, 2, (UCHAR *)"Return:[SET]", ResBuf);				/* set contents				*/
/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn100) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, TSTMODE_EXE, Hmsg );
		pv->State = TESTMODE_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			pv->State = TESTMODE_END;
		}
		else
		{
			; //処理なし
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunJogEnd( Fmsg );
		rc = 1;
		break;
	default:
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/* <S206> Start */
/****************************************************************************************************/
/*																									*/
/*		Fn100 Mode Execution (Panel Operator)														*/
/*																									*/
/****************************************************************************************************/
#if SVFSEL_PANELOP_USE == 1
LONG LpxPnlTstMode1Exe(FUNMSG *Fmsg)
{
	FNTSTMODV 	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);	/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;												/* 0: continue, 1: end	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn100) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pv->State )
		{
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, TSTMODE_EXE, Hmsg );
		pv->State = TESTMODE_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMake.mode == TRUE )
		{
			pv->State = TESTMODE_END;
		}
		else
		{
			; //処理なし
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunJogEnd( Fmsg );
		rc = FUN_END;
		break;
	default:
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	/* パネル表示データ作成	*/
	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	/* led = "tESt1" */
	RpxLedSetStr((UCHAR *)"tESt1", DspData, 4U, 5U);
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif
/* <S206> End */

/****************************************************************************************************/
/*																									*/
/*		Fn101 Mode Display																			*/
/*																									*/
/****************************************************************************************************/
LONG FunTstMod2Main(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdTstMod2Exe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlTstMode2Exe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return TRUE;
}


/****************************************************************************************************/
/*																									*/
/*		Fn101 Mode Begin																			*/
/*																									*/
/****************************************************************************************************/
LONG FunTstMod2Begin(FUNMSG *Fmsg)
{
LONG		rc = OK;
FNTSTMODV	*pv = (FNTSTMODV *)(Fmsg->pVar);
MSGIF		*Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->State = TESTMODE_EXE;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG,							/* Search Register (Operation Command)	*/
				OPESET_REG1,
				0,
				&(pv->OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
/* Set Fn101 Mode to Mode Register 																	*/
/*--------------------------------------------------------------------------------------------------*/
	rc = OPEREG_WRITE( pv->OpeModeIdx, FNTEST_REGOFF_MODE, Hmsg );
	if ( rc != OK ){ return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &(pv->DispTime) );						/* Panel Operator 						*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn101 Mode Execution (LCD Operator)															*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdTstMod2Exe(FUNMSG *Fmsg)
{
	FNTSTMODV	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF		*Hmsg = Fmsg->Super;
	UCHAR		*ResBuf = Fmsg->pTxBuf;//<S0C7>
//	LONG		IgnrBlnk = FALSE;//<S0C7>
	OPKEYFLG	OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);
//	OPKEYFLG	OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);//<S0C7>
	LONG		rc = 0;												/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 										/* clear Tx buffer				*/
//	RpxLcdSetStr(0, 9, (UCHAR *)"-Test 2-", ResBuf );				/* set title					*/		/* <S154> */
//	RpxLcdSetStr(0, 11, (UCHAR *)"Test 2", ResBuf );				/* set title					*/		/* <S154> */	/* <S207> */
//	RpxLcdSetStr(1, 5, (UCHAR *)"-Regeneration-", ResBuf );			/* set title					*/
//	RpxLcdSetStr(1, 1, (UCHAR *)"-Regeneration -", ResBuf );		/* set title					*/		/* <S1B8> */	/* <S207> */
//	RpxLcdSetStr(2, 3, (UCHAR *)"-Turned Off!-", ResBuf );			/* set title					*/
//	RpxLcdSetStr(2, 2, (UCHAR *)"-Turned Off!-", ResBuf );			/* set title					*/		/* <S1B8> */	/* <S207> */
	RpxLcdSetStr(0, 9, (UCHAR *)"-Test 2-", ResBuf );				/* set title					*/		/* <S207> */
	RpxLcdSetStr(1, 2, (UCHAR *)"Regeneration", ResBuf );			/* set title					*/		/* <S207> */
	RpxLcdSetStr(2, 3, (UCHAR *)"Turned Off!", ResBuf );			/* set title					*/		/* <S207> */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetStr(3, 0, (UCHAR *)"                 ", ResBuf);		/* set contents					*/
	RpxLcdSetStr(4, 2, (UCHAR *)"Return:[SET]", ResBuf);			/* set contents					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn101) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, TSTMODE_EXE, Hmsg );
		pv->State = TESTMODE_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			pv->State = TESTMODE_END;
		}
		else
		{
			; //処理なし
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunJogEnd( Fmsg );
		rc = 1;
		break;
	default:
		break;
	}
	/*----------------------------------------------------------------------------------------------*/
	return (rc);
}

/* <S206> Start */
/****************************************************************************************************/
/*																									*/
/*		Fn101 Mode Execution (Panel Operator)														*/
/*																									*/
/****************************************************************************************************/
#if SVFSEL_PANELOP_USE == 1
LONG LpxPnlTstMode2Exe(FUNMSG *Fmsg)
{
	FNTSTMODV 	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);	/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;												/* 0: continue, 1: end	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn101) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, TSTMODE_EXE, Hmsg );
		pv->State = TESTMODE_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMake.mode == TRUE )
		{
			pv->State = TESTMODE_END;
		}
		else
		{
			; //処理なし
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunJogEnd( Fmsg );
		rc = 1;
		break;
	default:
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	/* パネル表示データ作成	*/
	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	/* led = "tESt2" */
	RpxLedSetStr((UCHAR *)"tESt2", DspData, 4U, 5U);
/*--------------------------------------------------------------------------------------------------*/

	return (rc);
}
#endif
/* <S206> End */

/****************************************************************************************************/
/*																									*/
/*		Fn102 Mode Display																			*/
/*																									*/
/****************************************************************************************************/
LONG FunTstMod3Main(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdTstMod3Exe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlTstMode3Exe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return TRUE;
}


/****************************************************************************************************/
/*																									*/
/*		Fn102 Mode Begin																			*/
/*																									*/
/****************************************************************************************************/
LONG FunTstMod3Begin(FUNMSG *Fmsg)
{
LONG		rc = OK;
FNTSTMODV	*pv = (FNTSTMODV *)(Fmsg->pVar);
MSGIF		*Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	pv->State = TESTMODE_EXE;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Register																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG, 							/* Search Register (Operation Mode)		*/
				OPEMODE_REG1,
				0,
				&(pv->OpeModeIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	rc = COPEREG->Search( COPEREG,							/* Search Register (Operation Command)	*/
				OPESET_REG1,
				0,
				&(pv->OpeSetIdx),
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
/* Set Fn102 Mode to Mode Register 																	*/
/*--------------------------------------------------------------------------------------------------*/
	rc = OPEREG_WRITE( pv->OpeModeIdx, FNTEST_OLMSK_MODE, Hmsg );
	if ( rc != OK ){ return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	KpiRstLongTimer( &(pv->DispTime) );						/* Panel Operator 						*/
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn102 Mode Execution (LCD Operator)															*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdTstMod3Exe(FUNMSG *Fmsg)
{
	FNTSTMODV	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF		*Hmsg = Fmsg->Super;
	UCHAR		*ResBuf = Fmsg->pTxBuf;//<S0C7>
//	LONG		IgnrBlnk = FALSE;//<S0C7>
	OPKEYFLG	OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);
//	OPKEYFLG	OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);//<S0C7>
	LONG		rc = 0;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 										/* clear Tx buffer				*/
//	RpxLcdSetStr(0, 9, (UCHAR *)"-Test 3-", ResBuf );				/* set title					*/		/* <S154> */
//	RpxLcdSetStr(0, 11, (UCHAR *)"Test 3", ResBuf );				/* set title					*/		/* <S154> */	/* <S207> */
//	RpxLcdSetStr(1, 5, (UCHAR *)"-Overload-", ResBuf );				/* set title					*/
//	RpxLcdSetStr(1, 3, (UCHAR *)"-Overload -", ResBuf );			/* set title					*/		/* <S1B8> */	/* <S207> */
//	RpxLcdSetStr(2, 3, (UCHAR *)"-Masked!-", ResBuf );				/* set title					*/
//	RpxLcdSetStr(2, 4, (UCHAR *)"-Masked!-", ResBuf );				/* set title					*/		/* <S207> */
	RpxLcdSetStr(0, 9, (UCHAR *)"-Test 3-", ResBuf );				/* set title					*/		/* <S207> */
	RpxLcdSetStr(1, 4, (UCHAR *)"Overload", ResBuf );				/* set title					*/		/* <S207> */
	RpxLcdSetStr(2, 5, (UCHAR *)"Masked!", ResBuf );				/* set title					*/		/* <S207> */
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetStr(3, 0, (UCHAR *)"                 ", ResBuf);		/* set contents					*/
	RpxLcdSetStr(4, 2, (UCHAR *)"Return:[SET]", ResBuf);			/* set contents					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn102) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, TSTMODE_EXE, Hmsg );
		pv->State = TESTMODE_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			pv->State = TESTMODE_END;
		}
		else
		{
			; //処理なし
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunJogEnd( Fmsg );
		rc = 1;
		break;
	default:
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/* <S206> Start */
/****************************************************************************************************/
/*																									*/
/*		Fn102 Mode Execution (Panel Operator)														*/
/*																									*/
/****************************************************************************************************/
#if SVFSEL_PANELOP_USE == 1
LONG LpxPnlTstMode3Exe(FUNMSG *Fmsg)
{
	FNTSTMODV 	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF *Hmsg = Fmsg->Super;
	LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);	/* set display data buffer */
	OPKEYFLG OpKeyMake = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
	BLINKFLG *BlinkFlag = (BLINKFLG *)(Fmsg->pBlnkKick);
	LONG rc = FUN_CONTINUE;												/* 0: continue, 1: end	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Jog (Fn102) Sequence																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_EXE:													/* Execute State			*/
/*--------------------------------------------------------------------------------------------------*/
		OPEREG_WRITE( pv->OpeSetIdx, TSTMODE_EXE, Hmsg );
		pv->State = TESTMODE_WAIT;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_WAIT:													/* Wate state				*/
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMake.mode == TRUE )
		{
			pv->State = TESTMODE_END;
		}
		else
		{
			; //処理なし
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case TESTMODE_END: 													/* End state				*/
/*--------------------------------------------------------------------------------------------------*/
		FunJogEnd( Fmsg );
		rc = 1;
		break;
	default:
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	/* パネル表示データ作成	*/
	/* ALL Clear	*/
	MlibResetByteMemory( DspData, sizeof(LEDDSP) );	 					/* clear Tx buffer		*/

	/* led = "tESt3" */
	RpxLedSetStr((UCHAR *)"tESt3", DspData, 4U, 5U);
/*--------------------------------------------------------------------------------------------------*/

	return (rc);
}
#endif
/* <S206> End */

/****************************************************************************************************/
/*																									*/
/*		Fn100, Fn101, Fn102 Mode End																*/
/*																									*/
/****************************************************************************************************/
void FunTstModEnd(FUNMSG *Fmsg)
{
	FNTSTMODV	*pv = (FNTSTMODV *)(Fmsg->pVar);
	MSGIF		*Hmsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	OPEREG_WRITE( pv->OpeModeIdx, 0, Hmsg );
}



/***************************************** end of file **********************************************/
