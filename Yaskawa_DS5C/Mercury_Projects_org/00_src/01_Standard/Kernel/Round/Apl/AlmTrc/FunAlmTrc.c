/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FunAlmTrc.c : Alarm Traceback Display Function (Fn000) Sample Source						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Execute Alarm Trace Mode(Fn000)														*/
/*			2)Alarm Mode Display																	*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.15  K.Ozaki																*/
/*		Rev.1.01 : 2013.07.24  K.Ozaki	Bug Fix														*/
/*		Rev.1.02 : 2013.08.10  K.Ozaki	Add Panel Operator Function									*/
/*		Rev.1.03 : 2013.10.24  K.Ozaki	Remove Compile Warning 										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"				/* Kernel API */
#include	"LcdOp.h"				/* LCD Operator */
#include	"Monitor.h"				/* Un Monitor */
#include 	"Function.h"			/* Fn Function */

/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _FnAlmTrcV {
	int		LPos;					/* Line Position */
	int		DPos;					/* Digit Position */
	LONG	AlmTrcIdx;				/* Index of Alarm Trace Register */
	LONG	AlmTstampIdx;			/* Index of Alarm Trace Timestamp Register */
	int		Reserved[(256/4)-4];	/* Reserved: All functions can use ram variables at least 256 bytes. */
} FNALMTRCV;

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define	ALMTRCDISP_MAX		(10)					/*  */
#define	ALMPNLCHNG_MAX		(4)						/*  */
#define ALMTRC_REGNO		(0x500)					/* Alarm Trace Register Number */
#define ALMTSTAMP_REGNO		(0x52A)					/* Alarm Trace Timestamp Register Number */


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAlmTrcBegin(FUNMSG *Fmsg); 					/* Fn000 Alarm Trace Data Display Mode Begin	*/
LONG FunAlmTrcMain(FUNMSG *Fmsg);					/* Fn000 Alarm Trace Data Display Mode Main		*/
void FunAlmTrcEnd(FUNMSG *Fmsg);					/* Fn000 Alarm Trace Data Display Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG LpxLcdAlmTrcExe(FUNMSG *hMsg);					/* LCD Operator Fn000 Execution 				*/
#if  SVFSEL_PANELOP_USE != 1
#define LpxPnlAlmTrcExe( x ) (1)
#else
LONG LpxPnlAlmTrcExe(FUNMSG *hMsg);					/* Panel Operator Fn000 Execution 				*/
#endif

/****************************************************************************************************/
/*																									*/
/*		Fn000: Alarm Traceback Data Display: Normal mode 											*/
/*																									*/
/****************************************************************************************************/
LONG FunAlmTrcMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdAlmTrcExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return LpxPnlAlmTrcExe( Fmsg );
	}
/*--------------------------------------------------------------------------------------------------*/
	return (1);												/* End */
}

/****************************************************************************************************/
/*																									*/
/*		Fn000: Alarm Trace Data Display Begin														*/
/*																									*/
/****************************************************************************************************/
LONG FunAlmTrcBegin(FUNMSG *Fmsg)
{
LONG rc = NG;
MSGIF *Hmsg = Fmsg->Super;									/* get MsgIf */
FNALMTRCV *pv = (FNALMTRCV *)(Fmsg->pVar);					/* get Variable Pointer */
/*--------------------------------------------------------------------------------------------------*/
	pv->DPos = 0;											/* Reset Digit Position */
	pv->LPos = 0;											/* Reset Line Position */
/*--------------------------------------------------------------------------------------------------*/
	rc = CUNREGX->Search( (COBJECT *)CUNREGX, 				/* Search Register (Alarm Trace) */
				ALMTRC_REGNO, 
				0, 
				&(pv->AlmTrcIdx), 
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	rc = CUNREGX->Search( (COBJECT *)CUNREGX, 				/* Search Register (timestamp) */
				ALMTSTAMP_REGNO, 
				0, 
				&(pv->AlmTstampIdx), 
				MCHK_REG_READ,
				Hmsg);
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn000: Alarm Trace Data Display End															*/
/*																									*/
/****************************************************************************************************/
void FunAlmTrcEnd(FUNMSG *Fmsg)
{
	;
}

/****************************************************************************************************/
/*																									*/
/*		Fn000 Alarm Trace Data Display (LCD Operator)												*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdAlmTrcExe(FUNMSG *Fmsg)
{
LONG i;
ULONG AlmTrc[ALMTRCDISP_MAX];									/* alarm traceback data */
ULONG AlmTstamp[ALMTRCDISP_MAX];								/* alarm timestamp data */
FNALMTRCV *pv = (FNALMTRCV *)(Fmsg->pVar);						/* get Variable Pointer */
UCHAR *ResBuf = Fmsg->pTxBuf;									/* set response buffer */
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
LONG rc = 0;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
/*	Get Alarm Code & Time Stamp																		*/
/*--------------------------------------------------------------------------------------------------*/
	CUNREGX->ReadArray( (COBJECT *)CUNREGX, 
					pv->AlmTrcIdx,								/* Object Index */
					0,											/* Array Index */
					ALMTRCDISP_MAX,								/* Read Size */
					AlmTrc,										/* Read Data(pointer */
					MCHK_REG_READ,								/* Check Type */
					Fmsg->Super );
	CUNREGX->ReadArray( (COBJECT *)CUNREGX, 
					pv->AlmTstampIdx,							/* Object Index */
					0,											/* Array Index */
					ALMTRCDISP_MAX,								/* Read Size */
					AlmTstamp,									/* Read Data(pointer */
					MCHK_REG_READ,								/* Check Type */
					Fmsg->Super );
/*--------------------------------------------------------------------------------------------------*/
/*	Handle Key Data																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( OpKeyMk.mode )
	{
		rc = 1;													/* end */
	}
	else if ( OpKeyMk.up )
	{
		if(pv->LPos > 0)
		{
			pv->LPos--;
		}
		else if(pv->LPos == 0)
		{
			pv->LPos = ALMTRCDISP_MAX - 1;
		}
	}
	else if ( OpKeyMk.down )
	{
		if( pv->LPos < (ALMTRCDISP_MAX - 1) )
		{
			pv->LPos++;
		}
		else if( pv->LPos == (ALMTRCDISP_MAX - 1) )
		{
			pv->LPos = 0;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Set Alarm Trace Display Data																	*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 									/* clear Tx buffer */
//	RpxLcdSetStr(0, 10, (UCHAR *)"-ALARM-", ResBuf);			/* TITLE *//* <S154> */
	RpxLcdSetStr(0, 12, (UCHAR *)"ALARM", ResBuf);				/* TITLE *//* <S154> */
/*--------------------------------------------------------------------------------------------------*/
	for(i = 0; i < 4; i++)
	{
		long almidx = pv->LPos + i;
		if( almidx > (ALMTRCDISP_MAX - 1) )
		{ 
			almidx -= ALMTRCDISP_MAX; 
		}
		/*------------------------------------------------------------------------------------------*/
		RpxLcdSetDecPnt((USHORT)(i + 1), 0, (ULONG)almidx, 1, FALSE, 0, ResBuf); 	/* No	*/
		RpxLcdSetStr((USHORT)(i + 1), 1, (UCHAR *)": ", ResBuf); 					/* ": "*/
		/*------------------------------------------------------------------------------------------*/
		if( AlmTrc[almidx] != 0 )
		{
			RpxLcdSetValHex((USHORT)(i + 1), 3, AlmTrc[almidx], 3, ResBuf);
			RpxLcdSetStr((USHORT)(i + 1), 6, (UCHAR *)" ", ResBuf);
			RpxLcdSetDecPnt((USHORT)(i + 1), 7, AlmTstamp[almidx], 10, FALSE, 0, ResBuf);
		} 
		else 
		{
			RpxLcdSetStr((USHORT)(i + 1), 3, (UCHAR *)"---           ", ResBuf);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdSetBlnk(1, 0, 2, ResBuf); 												/* set blink */
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}

/****************************************************************************************************/
/*																									*/
/*		Fn000 Alarm Trace Data Display (Panel Operator)												*/
/*																									*/
/****************************************************************************************************/
#if  SVFSEL_PANELOP_USE == 1
LONG LpxPnlAlmTrcExe(FUNMSG *Fmsg)
{
LONG AlmCnt;													/* number of alarms */
ULONG AlmTrc[ALMTRCDISP_MAX];									/* alarm traceback data */
ULONG AlmTstamp[ALMTRCDISP_MAX];								/* alarm timestamp data */
FNALMTRCV *pv = (FNALMTRCV *)(Fmsg->pVar);						/* get Variable Pointer */
LEDDSP *DspData = (LEDDSP *)&(Fmsg->pTxBuf[PNLRSP_LEDCODE_IDX]);/* set display data buffer */
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
LONG rc = 0;													/* 0: continue, 1: end */
/*--------------------------------------------------------------------------------------------------*/
/*	Get Alarm Code & Time Stamp																		*/
/*--------------------------------------------------------------------------------------------------*/
	CUNREGX->ReadArray( (COBJECT *)CUNREGX, 
					pv->AlmTrcIdx,					/* Object Index */
					0,								/* Array Index */
					ALMTRCDISP_MAX,					/* Read Size */
					AlmTrc,							/* Read Data(pointer */
					MCHK_REG_READ,					/* Check Type */
					Fmsg->Super );
	CUNREGX->ReadArray( (COBJECT *)CUNREGX, 
					pv->AlmTstampIdx,				/* Object Index */
					0,								/* Array Index */
					ALMTRCDISP_MAX,					/* Read Size */
					AlmTstamp,						/* Read Data(pointer */
					MCHK_REG_READ,					/* Check Type */
					Fmsg->Super );
/*--------------------------------------------------------------------------------------------------*/
/*	Get Alarm Count																					*/
/*--------------------------------------------------------------------------------------------------*/
	for ( AlmCnt = ALMTRCDISP_MAX; AlmCnt > 0; AlmCnt-- ) { 
		if( AlmTrc[AlmCnt - 1] != 0 ){ break; } 
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Handle Key Data																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( OpKeyMk.data )
	{
		rc = 1;										/* end */
	}
	else if ( OpKeyMk.up )
	{
		if(pv->LPos > 0)
		{
			pv->LPos--;
		}
		else if(pv->LPos == 0)
		{
			pv->LPos = ALMTRCDISP_MAX - 1;
		}
	}
	else if ( OpKeyMk.down )
	{
		if( pv->LPos < (ALMTRCDISP_MAX - 1) )
		{
			pv->LPos++;
		}
		else if( pv->LPos == (ALMTRCDISP_MAX - 1) )
		{
			pv->LPos = 0;
		}
	}
	else if ( OpKeyMk.shup )
	{
		if( pv->DPos < (ALMPNLCHNG_MAX - 1) )
		{
			pv->DPos++;
		}
		else
		{
			pv->DPos = 0;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Set Alarm Trace Display Data																	*/
/*--------------------------------------------------------------------------------------------------*/
	if ( pv->DPos == 0 )
	{
		/*------------------------------------------------------------------------------------------*/
		/*
		 * Display Alarm Code
		 *
		 */
		/*------------------------------------------------------------------------------------------*/
		RpxLedSetBlnk( DspData, 0x00 );
		RpxLedSetValDec( pv->LPos*10, DspData, 4, 2, FALSE, 1 );
		RpxLedSetStr( (UCHAR *)" ", DspData, 3, 1 );
		if( pv->LPos < AlmCnt )
		{
			RpxLedSetValHex( AlmTrc[pv->LPos], DspData, 2, 3 );
		}
		else
		{
			RpxLedSetStr( (UCHAR *)"---", DspData, 2, 3 );
		}
	}
	else
	{
		/*------------------------------------------------------------------------------------------*/
		/*
		 * Display Time Stamp
		 *
		 */
		/*------------------------------------------------------------------------------------------*/
		if( pv->LPos < AlmCnt )
		{
			RpxLedDispMonDecLong(AlmTstamp[pv->LPos], DspData, (pv->DPos - 1), 0, DSPDEF_NOSIGN);
		}
		else 
		{
			switch( pv->DPos )
			{
				case 1:
					RpxLedSetStr((UCHAR *)"_", DspData, 4, 1);
					RpxLedSetStr((UCHAR *)"----", DspData, 3, 4);
					break;
				case 2:
					RpxLedSetStr((UCHAR *)"-", DspData, 4, 1);
					RpxLedSetStr((UCHAR *)"----", DspData, 3, 4);
					break;
				case 3:
					RpxLedSetStr((UCHAR *)"~", DspData, 4, 1);
					RpxLedSetStr((UCHAR *)"  --", DspData, 3, 4);
					break;
				default :
					RpxLedSetStr((UCHAR *)"_", DspData, 4, 1);
					RpxLedSetStr((UCHAR *)"----", DspData, 3, 4);
					break;
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}
#endif

/***************************************** end of file **********************************************/
