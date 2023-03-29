/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RegEdit.c : MEMOBUS Register Editor (Fn300) Sample Source									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function:																						*/
/*			1)Monitor Register 																		*/
/*			2)Edit Register 																		*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.20  K.Ozaki																*/
/*		Rev.1.10 : 2013.09.26  K.Ozaki 	<S00C> Add Trace Area(0x4000-0x7FFF) 						*/
/*		Rev.1.20 : 2013.12.05  K.Ozaki 	<S073> Revised for Array Data 								*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"		/* Kernel API */
#include	"LcdOp.h"		/* LCD Operator */
#include	"Monitor.h"		/* Un Monitor */
#include	"Parameter.h"	/* Parameter */
#include 	"Operation.h"	/* Fn Function */
#include 	"Information.h"	/* Information */
#include	"Memory.h"		/* Memory Trace */

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
//<S073> #define OBJ_WRITE( obj, idx, data, hmsg ) ((obj)->Write( (obj), (idx), (data), hmsg ))
#define OBJ_WRITE( obj, idx, subidx, data, hmsg ) ((obj)->WriteS( (obj), (idx), (subidx), (data), hmsg ))
//<S073> #define OBJ_READ( obj, idx, pdata, hmsg ) ((obj)->Read( (obj), (idx), (pdata), MCHK_REG_READ, hmsg ))
#define OBJ_READ( obj, idx, subidx, pdata, hmsg ) ((obj)->ReadS( (obj), (idx), (subidx), (pdata), MCHK_REG_READ, hmsg ))
#define OBJ_SEARCH( obj, num, base, hmsg ) ((obj)->SearchB( (obj), (num), (base), hmsg ))

#define CHK_OBJ_ISLONG( obj, idx, msg ) ( ( (void *)( obj ) == NULL ) ? FALSE \
										: (obj)->isLong( obj, idx, msg) ) 
#define GET_TRC_INDX( objidx )			((( objidx >= 4) && ( objidx < 8 )) ? ((objidx-4) << 12) : 0)

/****************************************************************************************************/
/*																									*/
/*		Variable Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _FnRegEditV {
	INT		State;					/* Sequence State */
	INT		LPos;					/* Line Position */
	INT		DPos;					/* Digit Position */
	UINT	ObjIdx[4];				/* Index of Object Table */
	UINT	RegIdx[4];				/* Index of Register Table */
	UINT	RegSubIdx[4];			/* Sub Index of Register Table <S073> */
	UINT	RegData[4];				/* Register Data */
	int		Reserved[(256/4)-1-1-4-4-4];	/* Reserved: All functions can use ram variables at least 256 bytes. */
} REGEDITV;

enum _FnRegEditState {
	REGEDIT_ADDREDIT = 0,
	REGEDIT_DATAEDIT,
};

/****************************************************************************************************/
/*																									*/
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
COBJECT * const CRegObjTbl[16] = {
	(COBJECT *)CPARAMX, /* 0: Parameter (Non Volatile) */
	(COBJECT *)CPARAMX, /* 1: Parameter (Temporal) */
	(COBJECT *)COPEREG,	/* 2: Operation */
	NULL,				/* 3: Not Defined */
	
	(COBJECT *)CTRCMEM,	/* 4: Data Trace */
	(COBJECT *)CTRCMEM,	/* 5: Data Trace */
	(COBJECT *)CTRCMEM,	/* 6: Data Trace */
	(COBJECT *)CTRCMEM,	/* 7: Data Trace */

	NULL,				/* 8: Not Defined */
	NULL,				/* 9: Not Defined */
	NULL,				/* A: Not Defined */
	NULL,				/* B: Not Defined */

	(COBJECT *)CINFREG,	/* C: Information Register */
	NULL,				/* D: Not Defined */
	(COBJECT *)CUNREGX,	/* E: Monitor Register */
	NULL,				/* F: Not Defined */
};

const UINT RegEditStartDisp[4] = {
	0x0100,				
	0x2001,
	0x2000,
	0xC000,
};


/****************************************************************************************************/
/*																									*/
/*		Function List																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunRegEditBegin(FUNMSG *Fmsg); 				/* Fn300 Register Editor Display Mode Begin		*/
LONG FunRegEditMain(FUNMSG *Fmsg);					/* Fn300 Register Editor Display Mode Main		*/
void FunRegEditEnd(FUNMSG *Fmsg);					/* Fn300 Register Editor Display Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG LpxLcdRegEditExe(FUNMSG *Fmsg);				/* LCD Operator Fn000 Execution 				*/
LONG LpxLcdHandleKeys(FUNMSG *Fmsg);				/* LCD Operator Input Key Handler				*/
/*--------------------------------------------------------------------------------------------------*/
#if  SVFSEL_PANELOP_USE == 0
#define LpxPnlRegEditExe( x ) (0)
#else
LONG LpxPnlRegEditExe(FUNMSG *Fmsg);				/* Panel Operator Fn000 Execution 				*/
#endif

/****************************************************************************************************/
/*																									*/
/*		Fn300: Register Editor Mode 																*/
/*																									*/
/****************************************************************************************************/
LONG FunRegEditMain(FUNMSG *Fmsg)
{
MSGIF *hMsg = Fmsg->Super;
/*--------------------------------------------------------------------------------------------------*/
	if( KPI_CHK_LopHmsg( hMsg ) )							/* LCD Operator */
	{
		return LpxLcdRegEditExe( Fmsg );
	} 
/*--------------------------------------------------------------------------------------------------*/
	else if ( KPI_CHK_PopHmsg( hMsg ) )						/* Panel Operator */
	{
		return 1 /* TODO: LpxPnlEditExe( Fmsg ) */;
	}
	return 1;//<S0C7>
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		Fn300: Register Editor Begin																*/
/*																									*/
/****************************************************************************************************/
LONG FunRegEditBegin(FUNMSG *Fmsg)
{
LONG rc = OK;
REGEDITV *pv = (REGEDITV *)(Fmsg->pVar);							/* get Variable Pointer */
int i;
/*--------------------------------------------------------------------------------------------------*/
	pv->DPos = 0;													/* Reset Digit Position */
	pv->LPos = 0;													/* Reset Line Position */
/*--------------------------------------------------------------------------------------------------*/
	for ( i=0; i<4; i++ )
	{
		pv->ObjIdx[i] = (RegEditStartDisp[i] >> 12) & 0x0F;			/* Load initial Obj. index */
		pv->RegIdx[i] = OBJ_SEARCH( CRegObjTbl[ pv->ObjIdx[i] ] 	/* Load initial Obj. index */
					, RegEditStartDisp[i] & 0x0FFF, 0, Fmsg->Super );
	}
/*--------------------------------------------------------------------------------------------------*/
	if( rc != OK ) { return (rc); }
/*--------------------------------------------------------------------------------------------------*/
	return (rc);
}


/****************************************************************************************************/
/*																									*/
/*		Fn300: Register Editor End																	*/
/*																									*/
/****************************************************************************************************/
void FunRegEditEnd(FUNMSG *Fmsg)
{
	;
}

/****************************************************************************************************/
/*																									*/
/*		Fn300 Register Editor Display																*/
/*																									*/
/****************************************************************************************************/
LONG LpxLcdRegEditExe(FUNMSG *Fmsg)
{
LONG i;
REGEDITV *pv = (REGEDITV *)(Fmsg->pVar);						/* get Variable Pointer */
CHAR *ResBuf = Fmsg->pTxBuf;									/* set response buffer */
LONG rc = 0;													/* 0: continue, 1: end */
COBJECT *pObj;
LONG wkx;
LONG subx;														/* <S073> */
LONG Editting;
/*--------------------------------------------------------------------------------------------------*/
/*	Handle Key Data																					*/
/*--------------------------------------------------------------------------------------------------*/
	rc = LpxLcdHandleKeys( Fmsg );
/*--------------------------------------------------------------------------------------------------*/
/*	Set Register Editor Display Data																*/
/*--------------------------------------------------------------------------------------------------*/
	RpxLcdClrTxBuf( ResBuf ); 									/* clear Tx buffer */
//	RpxLcdSetStr(0, 7, (UCHAR *)"-REG EDIT-", ResBuf);			/* TITLE */		/* <S154> */
	RpxLcdSetStr(0, 9, (UCHAR *)"REG EDIT", ResBuf);			/* TITLE */		/* <S154> */
/*--------------------------------------------------------------------------------------------------*/
	for(i = 0; i < 4; i++)
	{
		/*------------------------------------------------------------------------------------------*/
		pObj = CRegObjTbl[ pv->ObjIdx[i] ];
		Editting = ((pv->State == REGEDIT_DATAEDIT) && (pv->LPos == i) );
		if (  Editting )
		{
			wkx = pObj->GetNumber( pObj, pv->RegIdx[i], Fmsg->Super );			/* wkx = Reg. Number*/
			wkx += pv->RegSubIdx[i];											/* <S073> */
		}
		else if ( pObj != NULL )
		{ 
			wkx = pv->RegIdx[i];												/* wkx = Reg. Index */
			wkx |= GET_TRC_INDX( pv->ObjIdx[ i ] );								/* for Trace */
//<S073>			OBJ_READ( pObj, wkx, &(pv->RegData[i]), Fmsg->Super ); 				/* Read Register */
			subx = pv->RegSubIdx[i];
			OBJ_READ( pObj, wkx, subx, &(pv->RegData[i]), Fmsg->Super ); 		/* <S073> Read Register */
			wkx = pObj->GetNumber( pObj, pv->RegIdx[i], Fmsg->Super );			/* wkx = Reg. Number*/
			wkx += pv->RegSubIdx[i];											/* <S073> */
		}
		else
		{
			pv->RegData[i] = 0;													/* Reg. Data = 0 */
			wkx = 0;															/* Reg. Number = 0 */
		}
		/*------------------------------------------------------------------------------------------*/
		RpxLcdSetStr((USHORT)(i + 1), 0, (UCHAR *)"#", ResBuf); 				/* set "#"*/
		/*------------------------------------------------------------------------------------------*/
		RpxLcdSetValHex((USHORT)(i + 1), 1, pv->ObjIdx[i], 1, ResBuf); 			/* set ObjIdx */
		RpxLcdSetValHex((USHORT)(i + 1), 2, wkx, 3, ResBuf); 					/* set Number */
		/*------------------------------------------------------------------------------------------*/
		RpxLcdSetStr((USHORT)(i + 1), 5, (UCHAR *)"=0x", ResBuf); 				/* set "=0x"*/
		/*------------------------------------------------------------------------------------------*/
		{
		int Digit;
			Digit = ( (CHK_OBJ_ISLONG( pObj, pv->RegIdx[i], Fmsg->Super ) ) ) ? 8 : 4;
			RpxLcdSetValHex((USHORT)(i + 1), 8, pv->RegData[i], Digit, ResBuf); /* set Data */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Set Blink 																						*/
/*--------------------------------------------------------------------------------------------------*/
	switch ( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case REGEDIT_ADDREDIT:
/*--------------------------------------------------------------------------------------------------*/
			RpxLcdSetBlnk( 1+pv->LPos, 4-(pv->DPos), 1, ResBuf );
			break;
/*--------------------------------------------------------------------------------------------------*/
	case REGEDIT_DATAEDIT:
/*--------------------------------------------------------------------------------------------------*/
			{
			int Digit;
				pObj = CRegObjTbl[ pv->ObjIdx[ pv->LPos ] ];
				Digit = ( CHK_OBJ_ISLONG( pObj, pv->RegIdx[pv->LPos], Fmsg->Super ) ) ? 8 : 4; 
				RpxLcdSetBlnk( 1+pv->LPos, 7+Digit-(pv->DPos), 1, ResBuf );
			}
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
LONG LpxLcdHandleKeys(FUNMSG *Fmsg)
{
LONG wkx;
LONG subx;														/* <S073> */
LONG CurNumber;
LONG rc = 0;													/* 0: continue, 1: end */
REGEDITV *pv = (REGEDITV *)(Fmsg->pVar);						/* get Variable Pointer */
//CHAR *ResBuf = Fmsg->pTxBuf;									/* set response buffer *//*<S0C7>*/
OPKEYFLG OpKeyMk = *(OPKEYFLG *)(Fmsg->pKeyMk);					/* get key make flag */
/*--------------------------------------------------------------------------------------------------*/
COBJECT *pCurObj = CRegObjTbl[ pv->ObjIdx[pv->LPos] ];			/* Current Slected Object */
/*--------------------------------------------------------------------------------------------------*/
	CurNumber = (( pCurObj == NULL ) ? 0 : 						/* Current Reg. Number */
			pCurObj->GetNumber( pCurObj, pv->RegIdx[ pv->LPos ], Fmsg->Super ));
	CurNumber += pv->RegSubIdx[ pv->LPos ];						/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
	switch ( pv->State )
	{
/*--------------------------------------------------------------------------------------------------*/
	case REGEDIT_ADDREDIT:
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			rc = 1;														/* end */
		}
		else if ( OpKeyMk.up )
		{
			if ( pv->DPos < 3 )
			{
				if ( pCurObj != NULL )
				{
					wkx = CurNumber + (0x01 << (pv->DPos*4));			/* Set search number */
					subx = pv->RegSubIdx[ pv->LPos ];					/* <S073> */
					if ( wkx >= 0x1000 ) { wkx = 0xFFF; }				/* Limit number */
					pv->RegIdx[ pv->LPos ] = pCurObj->SearchBS( pCurObj,/* <S073> Forward Search */
									wkx,								/* Search Number */
									pv->RegIdx[ pv->LPos ],				/* Base Index */
									subx,								/* <S073> Sub Index */
									&(pv->RegSubIdx[ pv->LPos ]),		/* <S073> Sub Index */
									Fmsg->Super );						/* Message Interface */
				}
			}
			else
			{
				if( pv->ObjIdx[ pv->LPos ] < 0x0F ) 
				{ 
					pv->ObjIdx[ pv->LPos ]++; 
					pv->RegIdx[ pv->LPos ] = 0;
					pv->RegData[ pv->LPos ] = 0;
				}
			}
		}
		else if ( OpKeyMk.down )
		{
			if ( pv->DPos < 3 )
			{
				if ( pCurObj != NULL )
				{
					wkx = CurNumber - (0x01 << (pv->DPos*4));			/* Set search number */
					subx = pv->RegSubIdx[ pv->LPos ];					/* <S073> */
					if ( wkx < 0 ) { wkx = 0x000; }						/* Limit number */
					pv->RegIdx[ pv->LPos ] = pCurObj->SearchBS( pCurObj,/* <S073> Backward Search */
									wkx,								/* Search Number */
									pv->RegIdx[ pv->LPos ],				/* Base Index */
									subx,								/* <S073> Sub Index */
									&(pv->RegSubIdx[ pv->LPos ]),		/* <S073> Sub Index */
									Fmsg->Super );						/* Message Interface */
				}
			}
			else
			{
				if( pv->ObjIdx[ pv->LPos ] > 0x00 ) 
				{ 
					pv->ObjIdx[ pv->LPos ]--; 
					pv->RegIdx[ pv->LPos ] = 0;
					pv->RegData[ pv->LPos ] = 0;
				}
			}
		}
		else if ( OpKeyMk.shup )
		{
			if ( pv->DPos < 3 ) { pv->DPos++; }
		}
		else if ( OpKeyMk.shdown )
		{
			if ( pv->DPos > 0 ) { pv->DPos--; }
		}
		else if ( OpKeyMk.data ) 
		{
			if( CRegObjTbl[ pv->ObjIdx[pv->LPos] ] != NULL )
			{
				pv->State = REGEDIT_DATAEDIT;
				pv->DPos = 0;
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
	case REGEDIT_DATAEDIT:
/*--------------------------------------------------------------------------------------------------*/
		if( OpKeyMk.mode )
		{
			rc = 1;													/* end */
		}
		else if ( OpKeyMk.up )
		{
			if( ((pv->RegData[ pv->LPos ] >> (4*pv->DPos)) & 0x0F) != 0x0F )
			{
				pv->RegData[ pv->LPos ] = pv->RegData[ pv->LPos ] + ( 1 << (4*pv->DPos) );
			}
		}
		else if ( OpKeyMk.down )
		{
			if( ((pv->RegData[ pv->LPos ] >> (4*pv->DPos)) & 0x0F) != 0x00 )
			{
				pv->RegData[ pv->LPos ] = pv->RegData[ pv->LPos ] - ( 1 << (4*pv->DPos) );
			}
		}
		else if ( OpKeyMk.shup ) 
		{
			int MaxDigit = 
				( ( CHK_OBJ_ISLONG( pCurObj, pv->RegIdx[ pv->LPos ], Fmsg->Super ) ) ? 8 : 4);
			if ( pv->DPos < (MaxDigit-1) ) { pv->DPos++; }
		}
		else if ( OpKeyMk.shdown )
		{
			if ( pv->DPos > 0 ) { pv->DPos--; }
		}
		else if ( OpKeyMk.data )
		{
			wkx = pv->RegIdx[ pv->LPos ];
			wkx |= GET_TRC_INDX( pv->ObjIdx[ pv->LPos ] );						/* for Trace */
//<S073>	OBJ_WRITE( pCurObj, wkx, pv->RegData[ pv->LPos ], Fmsg->Super );
			subx = pv->RegSubIdx[ pv->LPos ];										/* <S073> */
			OBJ_WRITE( pCurObj, wkx, subx, pv->RegData[ pv->LPos ], Fmsg->Super );	/* <S073> */
			pv->State = REGEDIT_ADDREDIT;
			pv->DPos = 0;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:
/*--------------------------------------------------------------------------------------------------*/
		rc = 1;
		break;//<S0C7>
	}
	return (rc);
}

/***************************************** end of file **********************************************/
