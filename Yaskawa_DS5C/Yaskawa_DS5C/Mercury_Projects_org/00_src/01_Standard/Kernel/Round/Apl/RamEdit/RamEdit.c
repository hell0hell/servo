/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RamEdit.c : RAA Editor (Fn103) Sample Source												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Lcd Operator RAM Edit Function														*/
/*			2)Panel Operator RAM Edit Function														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.0.10 : 2013.10.08  K.Ozaki	<S012>														*/
/*		Rev.0.20 : 2013.10.09  K.Ozaki	<S013>	Bug fix, Panel Operator								*/
/*		Rev.0.30 : 2014.02.18  K.Ozaki	<S18B>	Save Ram Address									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include	"Memory.h"		/* Memory Trace */

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define CPU_RAMSADR 0x67FA0000				/* @@: CPUの内蔵RAM先頭アドレスを初期値とする */

/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _FnRamEditV {
	INT		State;					/* Sequence State */
	INT		LPos;					/* Line Position */
	INT		DPos;					/* Digit Position */
	UINT	Addr[4];				/* RAM Address */
	UINT	Data[4];				/* RAM Data */
	int		Reserved[(256/4)-1-1-1-1-4-4];	/* Reserved: All functions can use ram variables at least 256 bytes. */
} RAMEDITV;

enum _FnLcdRamEditState {
	RAMEDIT_ADDREDIT = 0,
	RAMEDIT_DATAEDIT,
};

enum _FnPnlRamEditState {			/* <S013> */
	RAMEDITPNL_ADDRHIEDIT = 0,
	RAMEDITPNL_ADDRLOEDIT,
	RAMEDITPNL_DATALOEDIT,
	RAMEDITPNL_DATAHIEDIT,
};

typedef struct _FnRamEditS {		/* <S18B> */
	UINT	InitAddr[4];			/* Initial RAM Address */
} RAMEDITS;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
const UINT RamEditStartAddr[4] = {
	CPU_RAMSADR,				
	CPU_RAMSADR,
	CPU_RAMSADR,
	CPU_RAMSADR,
};

/****************************************************************************************************/
/*																									*/
/*		Valiable Definition																			*/
/*																									*/
/****************************************************************************************************/
RAMEDITS	RamEditS;				/* <S18B> */

/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunRamEditBegin(FUNMSG *Fmsg); 				/* Fn103 RAM Editor Display Mode Begin			*/
LONG FunRamEditMain(FUNMSG *Fmsg);					/* Fn103 RAM Editor Display Mode Main			*/
void FunRamEditEnd(FUNMSG *Fmsg);					/* Fn103 RAM Editor Display Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxLcdRamEditExe(FUNMSG *Fmsg);			/* LCD Operator Fn103 Execution 				*/
static LONG LpxLcdRamEditHandleKeys(FUNMSG *Fmsg);	/* LCD Operator Input Key Handler				*/
/*--------------------------------------------------------------------------------------------------*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlRamEditExe( x ) (0)
#define LpxPnlRamEditHandleKeys (0)
#else
static LONG LpxPnlRamEditExe(FUNMSG *Fmsg);			/* Panel Operator Fn103 Execution 				*/
static LONG LpxPnlRamEditHandleKeys(FUNMSG *Fmsg);	/* Panel Operator Input Key Handler	<S013>		*/
#endif
/*--------------------------------------------------------------------------------------------------*/
static UINT LpxRamEditAddrDigitInc( RAMEDITV *pv );	/* Increment ram address digit <S013>			*/
static UINT LpxRamEditAddrDigitDec( RAMEDITV *pv );	/* Decrement ram address digit <S013>			*/

/****************************************************************************************************/
/*																									*/
/*		Fn103: RAM Editor Mode 																		*/
/*																									*/
/****************************************************************************************************/
LONG FunRamEditMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdRamEditExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlRamEditExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (1);												/* 0: continue 1: end */
}

/****************************************************************************************************/
/*																									*/
/*		Fn103: RAM Editor Begin																		*/
/*																									*/
/****************************************************************************************************/
LONG FunRamEditBegin(FUNMSG *Fmsg)
{
LONG rc = OK;
RAMEDITV *pv = (RAMEDITV *)(Fmsg->pVar);							/* get Variable Pointer */
MSGIF *hMsg = Fmsg->Super;
int i;
/*--------------------------------------------------------------------------------------------------*/
/*	Set Initial Digit, Line Position <S013>															*/
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )									/* LCD Operator */
	{
		pv->DPos = 0;												/* Reset Digit Position */
		pv->LPos = 3;												/* Reset Line Position */
	}
	else if ( KPI_CHK_PopHmsg( hMsg ) )								/* Panel Operator */
	{
		pv->DPos = 4;												/* Reset Digit Position */
		pv->LPos = 0;												/* Reset Line Position */
	}
/*--------------------------------------------------------------------------------------------------*/
	for ( i=0; i<4; i++ )
	{
		if( KpiCheckReadMemAddr( RamEditS.InitAddr[i], 1 ) != MEMDEF_READ_NG )
		{
			pv->Addr[i] = RamEditS.InitAddr[i];						/* set start address */
		}
		else
		{
			pv->Addr[i] = RamEditStartAddr[i];						/* set start address */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn103: RAM Editor End																		*/
/*																									*/
/****************************************************************************************************/
void FunRamEditEnd(FUNMSG *Fmsg)
{
int i;
RAMEDITV *pv = (RAMEDITV *)(Fmsg->pVar);							/* get Variable Pointer */
	for( i = 0; i < 4; i++ )
	{
		RamEditS.InitAddr[i] = pv->Addr[i];							/* <S18B> save start address */
	}
}

/****************************************************************************************************/
/*																									*/
/*		Fn103 RAM Editor Display 																	*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdRamEditExe(FUNMSG *Fmsg)
{
LONG i;
RAMEDITV *pv = (RAMEDITV *)(Fmsg->pVar);						/* get Variable Pointer */
UCHAR *ResBuf = Fmsg->pTxBuf;									/* set response buffer */
LONG rc = 0;													/* 0: continue, 1: end */
//LONG wkx;
LONG Editting;
LONG isLong;
/*--------------------------------------------------------------------------------------------------*/
/*	Handle Key Data																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = LpxLcdRamEditHandleKeys( Fmsg );
/*--------------------------------------------------------------------------------------------------*/
/*	Set Register Editor Display Data																*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 									/* clear Tx buffer */
//	RpxLcdSetStr(0, 7, (UCHAR *)"-RAM Edit-", ResBuf);			/* TITLE */		/* <S154> */
	RpxLcdSetStr(0, 9, (UCHAR *)"RAM Edit", ResBuf);			/* TITLE */		/* <S154> */
/*--------------------------------------------------------------------------------------------------*/
/*	Set Display Data																				*/
/*--------------------------------------------------------------------------------------------------*/
	for(i = 0; i < 4; i++)
	{
		/*------------------------------------------------------------------------------------------*/
		/*	Read Ram/Rom Value 																		*/
		/*------------------------------------------------------------------------------------------*/
		if ( KpiCheckReadMemAddr( pv->Addr[i], 1 /* @@ chk word size */) != MEMDEF_READ_NG )
		{
			Editting = ( (pv->State == RAMEDIT_DATAEDIT) && (pv->LPos == i) );
			isLong = ( KpiGetMemAccessSize( pv->Addr[i] ) == 4 );
			if ( ! Editting )
			{
				if ( isLong )
				{
					pv->Data[i] = *(ULONG *)(pv->Addr[i]);						/* get data */
				}
				else
				{
					pv->Data[i] = *(USHORT *)(pv->Addr[i]);						/* get data */
				}
			}
			/*--------------------------------------------------------------------------------------*/
			/*	Set Display Data	<S013>															*/
			/*--------------------------------------------------------------------------------------*/
			if ( isLong )
			{
				RpxLcdSetValHex(1+i, 11-2, (ULONG)pv->Data[i], 8, ResBuf );		/* set long data */
			}
			else
			{
				RpxLcdSetValHex(1+i, 11-2, (USHORT)pv->Data[i], 4, ResBuf );	/* set short data */
			}
		}
		else
		{
			RpxLcdSetStr(1+i, 11-2, (UCHAR *)"----", ResBuf );					/* set display data */
		}
		/*------------------------------------------------------------------------------------------*/
		RpxLcdSetValHex(1+i, 2-2, pv->Addr[i], 8, ResBuf );						/* set address */
		RpxLcdSetStr(1+i, 10-2, (UCHAR *)"=", ResBuf );							/* set equal */
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Set Blink																						*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( pv->State ) 										
	{
	case RAMEDIT_ADDREDIT:
		RpxLcdSetBlnk( 1+pv->LPos, (USHORT)(9-2 - pv->DPos), 1, ResBuf );
		break;
	case RAMEDIT_DATAEDIT:
		isLong = ( KpiGetMemAccessSize( pv->Addr[ pv->LPos ] ) == 4 );					/* <S013> */
		RpxLcdSetBlnk( 1+pv->LPos, 14-2-(pv->DPos) + ((isLong) ? 4 : 0), 1, ResBuf );	/* <S013> */
		break;
	default:
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}



/****************************************************************************************************/
/*																									*/
/*	LCD Operator Input Key Handler																	*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdRamEditHandleKeys(FUNMSG *Fmsg)
{
//UINT wkx;
LONG rc = 0;													/* 0: continue, 1: end */
RAMEDITV *pv = (RAMEDITV *)(Fmsg->pVar);						/* get Variable Pointer */
//CHAR *ResBuf = Fmsg->pTxBuf;									/* set response buffer */
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
/*--------------------------------------------------------------------------------------------------*/
	switch ( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case RAMEDIT_ADDREDIT:
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			rc = 1;														/* end */
		}
		else if ( OpKeyMk.up )
		{
			pv->Addr[ pv->LPos ] = LpxRamEditAddrDigitInc( pv );		/* set address */
		}
		else if ( OpKeyMk.down )
		{
			pv->Addr[ pv->LPos ] = LpxRamEditAddrDigitDec( pv );		/* set address */
		}
		else if ( OpKeyMk.shup )
		{
//			if ( pv->DPos < 7 ) { pv->DPos++; }
			pv->DPos = (pv->DPos+1) & 0x07;		/* <S013> rotate */
		}
		else if ( OpKeyMk.shdown )
		{
//			if ( pv->DPos > 0 ) { pv->DPos--; }
			pv->DPos = (pv->DPos-1) & 0x07;		/* <S013> rotate */
		}
		else if ( OpKeyMk.data ) 
		{
			if ( KpiCheckWriteMemAddr( pv->Addr[ pv->LPos ], 1 ) != MEMDEF_WRITE_NG )
			{
				pv->DPos = 0;
				pv->State = RAMEDIT_DATAEDIT;						/* set data edit mode */
			}
			else
			{
				((BLINKFLG *)(Fmsg->pBlnkKick))->no_op = 1;			/* No Operation */
			}
		}
		else if ( OpKeyMk.scroll )
		{
			pv->LPos = (( pv->LPos - 1 ) & 0x03);
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case RAMEDIT_DATAEDIT:
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			rc = 1;													/* end */
		}
		else if ( OpKeyMk.up )
		{
			if( ((pv->Data[ pv->LPos ] >> (4*pv->DPos)) & 0x0F) != 0x0F )
			{
				pv->Data[ pv->LPos ] = pv->Data[ pv->LPos ] + ( 1 << (4*pv->DPos) );
			}
		}
		else if ( OpKeyMk.down )
		{
			if( ((pv->Data[ pv->LPos ] >> (4*pv->DPos)) & 0x0F) != 0x00 )
			{
				pv->Data[ pv->LPos ] = pv->Data[ pv->LPos ] - ( 1 << (4*pv->DPos) );
			}
		}
		else if ( OpKeyMk.shup ) 
		{
			int MaxDigit = (( KpiGetMemAccessSize( pv->Addr[ pv->LPos ] ) == 4 ) ? 8 : 4);
//			if ( pv->DPos < (MaxDigit-1) ) { pv->DPos++; }
			pv->DPos = (pv->DPos+1) & (MaxDigit-1);			/* <S013> rotate */
		}
		else if ( OpKeyMk.shdown )
		{
			int MaxDigit = (( KpiGetMemAccessSize( pv->Addr[ pv->LPos ] ) == 4 ) ? 8 : 4);
//			if ( pv->DPos > 0 ) { pv->DPos--; }
			pv->DPos = (pv->DPos-1) & (MaxDigit-1);			/* <S013> rotate */
		}
		else if ( OpKeyMk.data )
		{
			/*--------------------------------------------------------------------------------------*/
			/* write data to specified memory addres 												*/
			/*--------------------------------------------------------------------------------------*/
			if( KpiGetMemAccessSize( pv->Addr[ pv->LPos ] ) == 4 )
			{
				*((ULONG *)pv->Addr[ pv->LPos ]) = (ULONG)(pv->Data[ pv->LPos ]);
			}
			else
			{
				*((USHORT *)pv->Addr[ pv->LPos ]) = (USHORT)(pv->Data[ pv->LPos ]);
			}
			/*--------------------------------------------------------------------------------------*/
			pv->State = RAMEDIT_ADDREDIT;
			pv->DPos = 0;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:
/*--------------------------------------------------------------------------------------------------*/
		rc = 1;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	if( rc != 0 ) { FunRamEditEnd( Fmsg ); }						/* <S18B> */
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn103 RAM Editor Display for Panel Operator	<S013>											*/
/*																									*/
/****************************************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
LONG LpxPnlRamEditExe(FUNMSG *Fmsg)
{
LONG isLong;
LONG Editting;
RAMEDITV *pv = (RAMEDITV *)(Fmsg->pVar);						/* get Variable Pointer */
//MSGIF *Hmsg = Fmsg->Super;										/* message interface */
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
//OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
//OPKEYFLG OpKeyLvl = *(OPKEYFLG *)(Fmsg->pKeyLvl);				/* get key level flag */
LONG rc = 0;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
/*	Handle Key Data																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = LpxPnlRamEditHandleKeys( Fmsg );
/*--------------------------------------------------------------------------------------------------*/
/*	Read Ram/Rom Value 																				*/
/*--------------------------------------------------------------------------------------------------*/
	if ( KpiCheckReadMemAddr( pv->Addr[0], 1 ) != MEMDEF_READ_NG )
	{
		Editting = ( (pv->State == RAMEDITPNL_DATALOEDIT) || (pv->State == RAMEDITPNL_DATAHIEDIT) );
		isLong = ( KpiGetMemAccessSize( pv->Addr[0] ) == 4 );
		if ( ! Editting )
		{
			if ( isLong )
			{
				pv->Data[0] = *(ULONG *)(pv->Addr[0]);						/* get data */
			}
			else
			{
				pv->Data[0] = *(USHORT *)(pv->Addr[0]);						/* get data */
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* 		Make Panel Display Data																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
		case RAMEDITPNL_ADDRHIEDIT:
			/* led="E.xxxx"	*/
			RpxLedSetStr( (UCHAR *)"E.", DspData, 4, 1);
			RpxLedSetValHex( (USHORT)(pv->Addr[ 0 ] >> 16), DspData, 3, 4);
			break;

		case RAMEDITPNL_ADDRLOEDIT:
			/* led="A.xxxx"	*/
			RpxLedSetStr( (UCHAR *)"A.", DspData, 4, 1);
			RpxLedSetValHex( (USHORT)(pv->Addr[ 0 ] & 0x0000FFFF), DspData, 3, 4);
			break;

		case RAMEDITPNL_DATALOEDIT:
			/* led="d.xxxx"	*/
			RpxLedSetStr((UCHAR *)"d.", DspData, 4, 1);
			RpxLedSetValHex( (pv->Data[ 0 ] & 0x0000FFFF), DspData, 3, 4);
			break;

		case RAMEDITPNL_DATAHIEDIT:
			/* Mercury: led="H.xxxx"	*/
			RpxLedSetStr((UCHAR *)"H.", DspData, 4, 1);
			RpxLedSetValHex( (pv->Data[ 0 ] >> 16), DspData, 3, 4);
			break;/*<S0C7>*/

		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*	Set Blink																						*/
/*--------------------------------------------------------------------------------------------------*/
	switch( pv->State )
	{
	/*----------------------------------------------------------------------------------------------*/
		case RAMEDITPNL_ADDRHIEDIT:			/* edit Hi address */
		case RAMEDITPNL_ADDRLOEDIT:			/* edit Lo address */
	/*----------------------------------------------------------------------------------------------*/
			RpxLedSetBlnk( DspData, 0x00);
			RpxLedSetBlnk( DspData, 
						(UCHAR)(1 << (pv->DPos - (( pv->State == RAMEDITPNL_ADDRHIEDIT ) ? 4 : 0))));
			break;

	/*----------------------------------------------------------------------------------------------*/
		case RAMEDITPNL_DATALOEDIT:			/* edit Lo data */
		case RAMEDITPNL_DATAHIEDIT:			/* edit Hi data */
	/*----------------------------------------------------------------------------------------------*/
			RpxLedSetBlnk( DspData, 0x00);
			if( KpiCheckWriteMemAddr( pv->Addr[ 0 ], 1) != MEMDEF_WRITE_NG )
			{
				RpxLedSetBlnk( DspData, 
						(UCHAR)(1 << (pv->DPos - (( pv->State == RAMEDITPNL_DATAHIEDIT ) ? 4 : 0))));
			}
			else 
			{
				RpxLedSetBlnk( DspData, 0x00);
				RpxLedSetStr( (UCHAR *)"d.----", DspData, 4, 5);
			}
			break;

	/*----------------------------------------------------------------------------------------------*/
		default:
	/*----------------------------------------------------------------------------------------------*/
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif

/****************************************************************************************************/
/*																									*/
/*	Panel Operator Input Key Handler	<S013>														*/
/*																									*/
/****************************************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
LONG LpxPnlRamEditHandleKeys(FUNMSG *Fmsg)
{
//UINT wkx;
LONG isLong;
LONG rc = 0;													/* 0: continue, 1: end */
RAMEDITV *pv = (RAMEDITV *)(Fmsg->pVar);						/* get Variable Pointer */
//LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
/*--------------------------------------------------------------------------------------------------*/
	switch ( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case RAMEDITPNL_ADDRHIEDIT:									/* hi address edit */
	case RAMEDITPNL_ADDRLOEDIT:									/* lo address edit */
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.data )
		{
			rc = 1;												/* end */
		}
		else if( OpKeyMk.up )
		{
			pv->Addr[ 0 ] = LpxRamEditAddrDigitInc( pv );		/* increment address */
		}
		else if( OpKeyMk.down )
		{
			pv->Addr[ 0 ] = LpxRamEditAddrDigitDec( pv );		/* decrement address */
		}
		else if( OpKeyMk.shup )
		{
			if( pv->DPos < (7- ((pv->State == RAMEDITPNL_ADDRHIEDIT) ? 0 : 4)) )
			{
				pv->DPos ++;									/* shift up */
			}
			else
			{
				pv->DPos = (4- ((pv->State == RAMEDITPNL_ADDRHIEDIT) ? 0 : 4));
			}
		}
		else if( OpKeyMk.mode )
		{
			pv->State = (pv->State == RAMEDITPNL_ADDRHIEDIT) 
					? RAMEDITPNL_ADDRLOEDIT : RAMEDITPNL_DATALOEDIT;
			pv->DPos = 0;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case RAMEDITPNL_DATALOEDIT:										/* lo data edit */
	case RAMEDITPNL_DATAHIEDIT:										/* hi data edit */
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.data )
		{
			if ( KpiCheckWriteMemAddr( pv->Addr[ 0 ], 1 ) != MEMDEF_WRITE_NG )
			{
				/*----------------------------------------------------------------------------------*/
				/* write ram data 																	*/
				/*----------------------------------------------------------------------------------*/
				if( KpiGetMemAccessSize( pv->Addr[ 0 ] ) == 4 )
				{
					*((ULONG *)pv->Addr[ 0 ]) = (ULONG)(pv->Data[ 0 ]);
				}
				else
				{
					*((USHORT *)pv->Addr[ 0 ]) = (USHORT)(pv->Data[ 0 ]);
				}
			}
			rc = 1;													/* end */
		}
		else if( OpKeyMk.up )
		{
			if( ((pv->Data[ pv->LPos ] >> (4*pv->DPos)) & 0x0F) != 0x0F )
			{
				pv->Data[ pv->LPos ] = pv->Data[ pv->LPos ] + ( 1 << (4*pv->DPos) );
			}
		}
		else if( OpKeyMk.down )
		{
			if( ((pv->Data[ pv->LPos ] >> (4*pv->DPos)) & 0x0F) != 0x00 )
			{
				pv->Data[ pv->LPos ] = pv->Data[ pv->LPos ] - ( 1 << (4*pv->DPos) );
			}
		}
		else if( OpKeyMk.shup )
		{
			if( pv->DPos < (7- ((pv->State == RAMEDITPNL_DATAHIEDIT) ? 0 : 4)) )
			{
				pv->DPos ++;										/* shift up */
			}
			else
			{
				pv->DPos = (4- ((pv->State == RAMEDITPNL_DATAHIEDIT) ? 0 : 4));
			}
		}
		else if( OpKeyMk.mode )
		{
			if ( pv->State == RAMEDITPNL_DATALOEDIT )
			{
				isLong = ( KpiGetMemAccessSize( pv->Addr[0] ) == 4 );
				pv->State = (( isLong ) ? RAMEDITPNL_DATAHIEDIT : RAMEDITPNL_ADDRHIEDIT);
				pv->DPos = 4;
			}
			else
			{
				pv->State = RAMEDITPNL_ADDRHIEDIT;
				pv->DPos = 4;
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:
/*--------------------------------------------------------------------------------------------------*/
		rc = 1;
		break;
	}
	return (rc);
}
#endif


/****************************************************************************************************/
/*																									*/
/* 		Increment ram address digit <S013>															*/
/*																									*/
/****************************************************************************************************/
UINT LpxRamEditAddrDigitInc( RAMEDITV *pv )
{
UINT wkx;

	wkx = pv->Addr[ pv->LPos ];									/* wkx = current addr */
	wkx += ((ULONG)0x01 << ((pv->DPos) * 4));					/* increment address */
	if ( wkx < pv->Addr[ pv->LPos ] )
	{
		wkx = 0xFFFFFFFF;										/* overflow : limit address */
	}

	return (KpiFindMemNextAddress( wkx, pv->Addr[ pv->LPos ] ));	/* return address */
}

/****************************************************************************************************/
/*																									*/
/*		Decrement ram address digit <S013>															*/
/*																									*/
/****************************************************************************************************/
UINT LpxRamEditAddrDigitDec( RAMEDITV *pv )
{
UINT wkx;

	wkx = pv->Addr[ pv->LPos ];									/* wkx = current addr */
	wkx -= ((ULONG)0x01 << ((pv->DPos) * 4));					/* decrement address */
	if ( wkx > pv->Addr[ pv->LPos ] )
	{
		wkx = 0x0;												/* underflow : limit address */
	}

	return (KpiFindMemNextAddress( wkx, pv->Addr[ pv->LPos ] )); /* set address */
}

/***************************************** end of file **********************************************/
