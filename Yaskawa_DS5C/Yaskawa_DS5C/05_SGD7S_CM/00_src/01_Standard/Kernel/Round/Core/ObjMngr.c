/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ObjMngr.c : Object Resource Management														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Read Object Data																			*/
/*																									*/
/*		2) Search Obeject 																			*/
/*																									*/
/*		3) Write Object Data																		*/
/*																									*/
/*		4) Check Object Access Level																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.08  K.Ozaki																*/
/*		Rev.1.10 : 2013.09.26  K.Ozaki	<S00C>														*/
/*		Rev.1.20 : 2013.12.05  K.Ozaki	<S073>														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Object.h"
#include "LcdOp.h"		/* <S113> */

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxInitObjManager( void );			/* Initialize Function 									*/
void	RpxPrstObjManager( void );			/* Parameter Reset Function 							*/
/*--------------------------------------------------------------------------------------------------*/
//LONG	RpxObjRead(							/* Read Object Data										*/
//		COBJECT *this,						/* this Pointer 										*/
//		COBJDEF	*pObjDef,					/* Object Definition Pointer							*/
//		ULONG	*pResData,					/* Pointer to the Read Data								*/
//		ULONG	CheckType,					/* Error Check Type										*/
//		MSGIF 	*hMsg );					/* Message Handle										*/
/*--------------------------------------------------------------------------------------------------*/
//LONG	RpxObjReadIdx(						/* Read Object Data ( Index Input )						*/
LONG	RpxObjRead(							/* <S073> Read Object Data ( Index Input )				*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	Index,						/* Object Table Index									*/
		ULONG	*pResData,					/* Pointer to the Object Data							*/
		ULONG	CheckType,					/* Error Check Type										*/
		MSGIF   *hMsg );					/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjReadS(						/* <S073> Read Object Data with Sub Index				*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	Index,						/* Object Table Index									*/
		LONG	SubIndex,					/* Object Table Sub Index								*/
		ULONG	*pResData,					/* Pointer to the Read Data								*/
		ULONG	CheckType,					/* Error Check Type										*/
		MSGIF 	*hMsg );					/* Message Handle										*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjSearch(						/* Search Object Index (Binary Search)					*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Object Search Base Index								*/
		LONG	*pFindIdx,					/* Pointer to the Find Index							*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		);				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjSearchSub(					/* <S073> Search Object with Sub Index (Binary Search)	*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Object Search Base Index								*/
		LONG	*pFindIdx,					/* Pointer to the Find Index							*/
		LONG	*pFindSubIdx,				/* Pointer to the Find Sub Index						*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		);				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjSearchIdx(					/* Search Object Index (Linear Search)					*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Base Index											*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjSearchIdxSub(					/* <S073> Search Object Index (Linear Search)			*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Base Index											*/
		LONG	BaseSubIdx,					/* Base Sub Index										*/
		LONG	*pFindSubIdx,				/* Pointer to the Find Sub Index						*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjReadArray(					/* Read Array Object ( by using Index ) 				*/
		COBJECT *this,						/* this Pointer 										*/
		LONG Index,							/* Index of the Object 									*/
		LONG ArrayIndex,					/* Index of the Array 									*/
		LONG ReadSize,						/* Size of the Array 									*/
		ULONG *pResData,					/* Pointer to the Read Value 							*/
		ULONG CheckType,					/* Error Check Type 									*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjWrite(						/* Write Object 										*/
		COBJECT *this,						/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		ULONG Data,							/* Write Value 											*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjWriteS(						/* <S073> Write Object 									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		LONG SubIndex,  					/* Sub Index of the Object Defitnion 					*/
		ULONG Data,							/* Write Value 											*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjWriteArray(					/* <S073> Write Object 									*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		LONG ArrayIndex,					/* Index of the Array 									*/
		LONG WriteSize,						/* Size of the Array 									*/
		ULONG *pWrData,						/* Pointer to the Write Value 							*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjGetNumber(					/* Write Object			 								*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjChkLong(						/* Check Object is Long or not							*/
		COBJECT *this,						/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg );						/* Message Interface 									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxCheckObjAttrMdef(			/* Check Object Attribute 								*/
		COBJDEF	*pObjDef,					/* Object Definition									*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxSearchObjTblIdx(				/* Search Index from Object Table 						*/
		COBJTBL *ObjTbl,					/* Object Definition Table								*/
		LONG	Size,						/* Table Size											*/
		LONG	Number,						/* Object Number										*/
		LONG	*pFindIdx		);			/* Search Result Index 									*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxSearchObjTblNextIdx(			/* Search the Next Index from the Start Index			*/
		COBJTBL	*pObjTbl,					/* Object Definition Table								*/
		LONG	EntNum,						/* Table Entry Number									*/
		LONG	ObjNo,						/* Object Number										*/
		LONG	StIdx,						/* Start Index											*/
		HMSGIF	Hmsg);						/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxObjLimitCheckPdef(			/* Check Parameter Limit								*/
		COBJDEF *pObjDef,					/* Pointer to the ObjDef								*/
		ULONG	Data			);			/* Check Data											*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxObjWrite(					/* Write Object			 								*/
		COBJDEF *pObjDef,					/* Pointer to the Object Definition						*/
		ULONG	PrmData,					/* Write value											*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxObjWriteS(					/* <S073> Write Object with Array Index					*/
		COBJDEF *pObjDef,					/* Pointer to the Object Definition						*/
		LONG	ArrayIndex,					/* Index of the Array 									*/
		ULONG	PrmData,					/* Write value											*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
static LONG LpxCheckObjAttrIsArray( 		/* <S073> Search Array									*/
		COBJDEF *pObjDef, 					/* Pointer to the Object Definition						*/
		LONG SrchNumber, 					/* Search Number 										*/
		LONG ObjNumber );					/* Object Number 										*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxSearchObjTblIdxSub(			/* <S073> Search Object Table Index						*/
		COBJTBL *pObjTbl,					/* Object Definition Table								*/
		LONG	Size,						/* Table Size											*/
		LONG	Number,						/* Object Number										*/
		LONG	*pFindIdx,					/* Search Result Index 									*/
		LONG	*pFindSubIdx	);			/* Search Result Sub Index 								*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxSearchObjTblNextIdxSub(		/* <S073> Search the Next Index with Sub Index		 	*/
		COBJTBL	*pObjTbl,					/* Object Definition Table								*/
		LONG	Size,						/* Table Size 											*/
		LONG	ObjNo,						/* Object Number										*/
		LONG	StIdx,						/* Start Index											*/
		LONG	StSubIdx,					/* Start Sub Index										*/
		LONG	*pFindSubIdx,				/* Search Result Sub Index 								*/
		HMSGIF	Hmsg);						/* Message Interface									*/



/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Get Object number						 													*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_OBJNUMBER( tbl, idx )   ((tbl)[(idx)].Number)

/*--------------------------------------------------------------------------------------------------*/
/*		Check the parameter is long or not		 													*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_OBJ_ISLONG( pndef )   (((pndef)->DataLength) == 4 ? TRUE : FALSE)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Max Sub Index																			*/
/*--------------------------------------------------------------------------------------------------*/
#define Lpx_GetMaxSubIdx( tbl, idx )  /* <S073> */\
			( (GET_OBJDEF( tbl, idx )->DataLength * GET_OBJDEF( tbl, idx )->NumberOfArray)>>1)

/*--------------------------------------------------------------------------------------------------*/
/*		:Pn00B.0対応																				*/
/*--------------------------------------------------------------------------------------------------*/
#define SETUP_PRM	0x40	/* <S113> */
#define TUNING_PRM	0x80	/* <S113> */

/****************************************************************************************************/
/*																									*/
/*		Valiable Definition																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/






/****************************************************************************************************/
/*																									*/
/*		Initialize Function																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitObjManager( void )
{
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Parameter Reset Function																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstObjManager( void )
{
		return;
}



/****************************************************************************************************/
/*																									*/
/*		Object Data Read Function (DefPtr Input)													*/
/*																									*/
/****************************************************************************************************/
#if 0 /* <S073> */
LONG	RpxObjRead(
		COBJECT *this,						/* this Pointer 										*/
		COBJDEF	*pObjDef,					/* Object Definition Pointer							*/
		ULONG	*pResData,					/* Pointer to the Read Data								*/
		ULONG	CheckType,					/* Error Check Type										*/
		MSGIF 	*hMsg )						/* Message Handle										*/
{
LONG	rc;
/*--------------------------------------------------------------------------------------------------*/
/*		Check Object Attribute																		*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxCheckObjAttrMdef( pObjDef, CheckType, hMsg );
		if( rc != OK ){ return( rc );}
/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Data																				*/
/*--------------------------------------------------------------------------------------------------*/
		GET_OBJDATA( pObjDef, 0, pResData, hMsg );
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}
#endif


/****************************************************************************************************/
/*																									*/
/*		Read Object Data (Index Input)																*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjRead(
		COBJECT *this,						/* this Pointer 										*/
		LONG	Index,						/* Object Table Index									*/
		ULONG	*pResData,					/* Pointer to the Read Data								*/
		ULONG	CheckType,					/* Error Check Type										*/
		MSGIF   *hMsg )						/* Message Interface 									*/
{
LONG	rc;
COBJTBL *pObjTbl = GET_OBJTBL( this );						/* get object table */
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );			/* get object definition */
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= this->TblEntNum )
		{
			return ( MERR_NO );								/* Object Number Error					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Attribute (Access Level, Special Use, etc)											*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxCheckObjAttrMdef( pObjDef, CheckType, hMsg );
		if( rc != OK ){ return( rc );}
/*--------------------------------------------------------------------------------------------------*/
/*		Read Object Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		GET_OBJDATA( pObjDef, 0, pResData, hMsg );
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Read Object Data (Index Input)														*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjReadS(
		COBJECT *this,						/* this Pointer 										*/
		LONG	Index,						/* Object Table Index									*/
		LONG	SubIndex,					/* Object Table Sub Index								*/
		ULONG	*pResData,					/* Pointer to the Read Data								*/
		ULONG	CheckType,					/* Error Check Type										*/
		MSGIF   *hMsg )						/* Message Interface 									*/
{
LONG	rc;
LONG	l;
COBJTBL *pObjTbl = GET_OBJTBL( this );						/* get object table */
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );			/* get object definition */
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= this->TblEntNum )
		{
			return ( MERR_NO );								/* Object Number Error					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Attribute (Access Level, Special Use, etc)											*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxCheckObjAttrMdef( pObjDef, CheckType, hMsg );
		if( rc != OK ){ return( rc );}
/*--------------------------------------------------------------------------------------------------*/
/*		Read Object Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		l = CHK_OBJ_ISLONG( pObjDef );							/* TODO: byte araay */
		GET_OBJDATA( pObjDef, SubIndex>>l, pResData, hMsg );
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		Read Array Object Data (Index Input)														*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjReadArray(
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,							/* Index of the Object 									*/
		LONG ArrayIndex,					/* Index of the Array 									*/
		LONG ReadSize,						/* Size of the Array 									*/
		ULONG *pResData,					/* Pointer to the Read Value 							*/
		ULONG CheckType,					/* Error Check Type 									*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
LONG 	i;
LONG	rc;
COBJTBL *pObjTbl = GET_OBJTBL( this );						/* get object table */
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );			/* get object definition */
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= this->TblEntNum )
		{
			return ( MERR_NO );								/* Object Number Error					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Attribute (Access Level, Special Use, etc)											*/
/*--------------------------------------------------------------------------------------------------*/
		rc = LpxCheckObjAttrMdef( pObjDef, CheckType, hMsg );
		if( rc != OK ){ return( rc );}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Array Size 																			*/
/*--------------------------------------------------------------------------------------------------*/
		if ( ( ArrayIndex + ReadSize ) > pObjDef->NumberOfArray )	/* @@ check */
		{
			return ( MERR_NO );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Read Object Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < ReadSize ; i++ ) 
		{
			GET_OBJDATA( pObjDef, (ArrayIndex + i), &(pResData[i]), hMsg );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}



/****************************************************************************************************/
/*																									*/
/*		Search Object Index From Object Table														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	: Search Object Index by using binary search.													*/
/*																									*/
/* 		Found:     return = OK, *pFindIdx = Index													*/
/* 		Not Found: return = NG, *pFindIdx = 0														*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjSearch(						/* Search Object Index									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Object Search Base Index								*/
		LONG	*pFindIdx,					/* Pointer to the Find Index							*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		)				/* Message Interface									*/
{
COBJTBL *pObjTbl = GET_OBJTBL( this );
/*--------------------------------------------------------------------------------------------------*/
	if ( LpxSearchObjTblIdx( pObjTbl, this->TblEntNum, SrchNo, pFindIdx ) == OK ) 
	{
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( *pFindIdx >= this->TblEntNum )
		{
			;									/* Monitor Number Error */
		}
		else 
/*--------------------------------------------------------------------------------------------------*/
		if ( LpxCheckObjAttrMdef( GET_OBJDEF( pObjTbl, *pFindIdx ), CheckType, Hmsg ) == OK ) 
		{
			return (OK);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	*pFindIdx = 0; 
	return (NG); 								/* return (MERR_NO); */
}


/****************************************************************************************************/
/*																									*/
/*		<S073> Search Object Index From Object Table												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	: Search Object Index by using binary search.													*/
/*																									*/
/* 		Found:     return = OK, *pFindIdx = Index													*/
/* 		Not Found: return = NG, *pFindIdx = 0														*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjSearchSub(					/* <S073> Search Object with Sub Index (Binary Search)	*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Object Search Base Index								*/
		LONG	*pFindIdx,					/* Pointer to the Find Index							*/
		LONG	*pFindSubIdx,				/* Pointer to the Find Sub Index						*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		)				/* Message Interface									*/
{
COBJTBL *pObjTbl = GET_OBJTBL( this );
/*--------------------------------------------------------------------------------------------------*/
	if ( LpxSearchObjTblIdxSub( pObjTbl, this->TblEntNum, SrchNo, pFindIdx, pFindSubIdx ) == OK ) 
	{
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( *pFindIdx >= this->TblEntNum )
		{
			;									/* Monitor Number Error */
		}
		else 
/*--------------------------------------------------------------------------------------------------*/
		if ( LpxCheckObjAttrMdef( GET_OBJDEF( pObjTbl, *pFindIdx ), CheckType, Hmsg ) == OK ) 
		{
			return (OK);
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	*pFindIdx = 0; 
	*pFindSubIdx = 0;
	return (NG); 								/* return (MERR_NO); */
}

/****************************************************************************************************/
/*																									*/
/*		Search Index of Object Table		 														*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjSearchIdx(					/* Return: Find Index									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Search Number										*/
		LONG	BaseIdx,					/* Start Index											*/
		HMSGIF	Hmsg		)				/* Message Interface									*/
{
COBJTBL *pTbl = GET_OBJTBL( this );
LONG	EntNum = this->TblEntNum;
LONG	MaxNum = this->GetNumber( this, EntNum-1, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*	@@ Limit Search Number																			*/
/*--------------------------------------------------------------------------------------------------*/
	SrchNo = ((0xF000 & (SrchNo)) == 0xF000) ? 0 : SrchNo;
	SrchNo = ( SrchNo > MaxNum ) ? MaxNum : SrchNo;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Next Index																				*/
/*--------------------------------------------------------------------------------------------------*/
	return LpxSearchObjTblNextIdx( pTbl, EntNum, SrchNo, BaseIdx, Hmsg );
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Search Index of Object Table		 													*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjSearchIdxSub(					/* Return: Find Index									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Search Number										*/
		LONG	BaseIdx,					/* Start Index											*/
		LONG	BaseSubIdx,					/* Start Sub Index										*/
		LONG	*pFindSubIdx,				/* Pointer to the Find Sub Index						*/
		HMSGIF	Hmsg		)				/* Message Interface									*/
{
COBJTBL *pTbl = GET_OBJTBL( this );
LONG	EntNum = this->TblEntNum;
LONG	MaxNum = this->GetNumber( this, EntNum-1, Hmsg );
/*--------------------------------------------------------------------------------------------------*/
/*	@@ Limit Search Number																			*/
/*--------------------------------------------------------------------------------------------------*/
	SrchNo = ((0xF000 & (SrchNo)) == 0xF000) ? 0 : SrchNo;
	SrchNo = ( SrchNo > MaxNum ) ? MaxNum : SrchNo;
/*--------------------------------------------------------------------------------------------------*/
/*	Search Next Index, Next Sub Index																*/
/*--------------------------------------------------------------------------------------------------*/
	return LpxSearchObjTblNextIdxSub( pTbl, EntNum, SrchNo, BaseIdx, BaseSubIdx, pFindSubIdx, Hmsg );
}


/****************************************************************************************************/
/*																									*/
/*		Search Index of Object Table																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSearchObjTblIdx(					/* Search Object Table Index							*/
		COBJTBL *pObjTbl,					/* Object Definition Table								*/
		LONG	Size,						/* Table Size											*/
		LONG	Number,						/* Object Number										*/
		LONG	*pFindIdx		)			/* Search Result Index 									*/
{
LONG is;													/* start index */
LONG ie;													/* end index */
LONG in;													/* intermediate index */
/*--------------------------------------------------------------------------------------------------*/
	is=0;
	ie=Size-1;
/*--------------------------------------------------------------------------------------------------*/
	while (TRUE)  											/* binary search */
	{
		if (ie < is) { return (NG); }
		in = (is+ie)>>1;
		if (Number == GET_OBJNUMBER( pObjTbl, in ) ){ *pFindIdx = in; return (OK); }
		if (Number < GET_OBJNUMBER( pObjTbl, in ) ){ ie = in-1; }
		else { is = in+1; }
	}
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Search Index and Sub Index of Object Table											*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSearchObjTblIdxSub(				/* Search Object Table Index							*/
		COBJTBL *pObjTbl,					/* Object Definition Table								*/
		LONG	Size,						/* Table Size											*/
		LONG	Number,						/* Object Number										*/
		LONG	*pFindIdx,					/* Search Result Index 									*/
		LONG	*pFindSubIdx	)			/* Search Result Sub Index 								*/
{
LONG is;													/* start index */
LONG ie;													/* end index */
LONG in;													/* intermediate index */
/*--------------------------------------------------------------------------------------------------*/
	is=0;
	ie=Size-1;
/*--------------------------------------------------------------------------------------------------*/
	while (TRUE)  											/* binary search */
	{
		in = (is+ie)>>1;									/* <S073> */
		if (ie < is) { break; }
//<S073>		in = (is+ie)>>1;
		if (Number == GET_OBJNUMBER( pObjTbl, in ) ){ *pFindIdx = in; *pFindSubIdx = 0; return (OK); }
		if (Number < GET_OBJNUMBER( pObjTbl, in ) ){ ie = in-1; }
		else { is = in+1; }
	}
/*--------------------------------------------------------------------------------------------------*/
/*	Check Array																						*/
/*--------------------------------------------------------------------------------------------------*/
	if ( LpxCheckObjAttrIsArray( GET_OBJDEF( pObjTbl, in ), Number, GET_OBJNUMBER( pObjTbl, in ) ) )
	{
		*pFindIdx = in;
		*pFindSubIdx = Number - GET_OBJNUMBER( pObjTbl, in );
		return (OK);
	}
/*--------------------------------------------------------------------------------------------------*/
	return (NG);
/*--------------------------------------------------------------------------------------------------*/
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Search Index of Object Table ( if not found, then find next index ) 					*/
/*																									*/
/****************************************************************************************************/
LONG LpxCheckObjAttrIsArray( COBJDEF *pObjDef, LONG SrchNumber, LONG ObjNumber )
{
LONG narray = pObjDef->NumberOfArray; 
LONG dsize = pObjDef->DataLength;
LONG l = CHK_OBJ_ISLONG( pObjDef );
/*------------------------------------------------------------------------------------------*/
	if ( narray == 0 )
	{
		return FALSE;							/* Not Array */
	}
	else if (( SrchNumber >=  ObjNumber ) &&
			( SrchNumber <= ( ObjNumber + ((dsize * (narray - 1)) >> 1) ) ))
	{
		//return TRUE;							/* Find */
		return (!(SrchNumber & l));				/* Check Long Object, Odd Srch Number */
	}
/*------------------------------------------------------------------------------------------*/
	else
	{
		return FALSE;
	}
}


/****************************************************************************************************/
/*																									*/
/*		Search Index of Object Table ( if not found, then find next index ) 						*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSearchObjTblNextIdx(				/* Search Index of Object Table							*/
		COBJTBL	*pObjTbl,					/* Object Table											*/
		LONG	EntNum,						/* Table Entry Number									*/
		LONG	ObjNo,						/* Object Number										*/
		LONG	StIdx,						/* Start Index											*/
		HMSGIF	Hmsg)						/* Message Interface									*/
{
#define UNSEARCH_FORWARD (1)
#define UNSEARCH_BACKWARD (-1)
LONG idx;
LONG rtIdx;    /* return index */
LONG dir;      /* Search direction */
/*--------------------------------------------------------------------------------------------------*/
	rtIdx = StIdx;
/*--------------------------------------------------------------------------------------------------*/
	if ( ObjNo >= GET_OBJNUMBER( pObjTbl, StIdx ) )
	{ /* forward search */
		dir = UNSEARCH_FORWARD;
	}
	else
	{ /* backword search */
		dir = UNSEARCH_BACKWARD;
	}
/*--------------------------------------------------------------------------------------------------*/
	for ( idx = StIdx; (idx < EntNum) && (idx >= 0); idx = idx + dir )
	{
		if ( LpxCheckObjAttrMdef( GET_OBJDEF( pObjTbl, idx), MCHK_OPE_READ, Hmsg ) != OK ) { continue; }
		/*------------------------------------------------------------------------------------------*/
		rtIdx = idx; /* visible index */
		if ( (dir == UNSEARCH_FORWARD ) && ( GET_OBJNUMBER( pObjTbl, idx ) >= ObjNo ) ){ break; }
		else if ( (dir == UNSEARCH_BACKWARD ) && ( GET_OBJNUMBER( pObjTbl, idx ) <= ObjNo ) ){ break; }
		/*------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return (rtIdx);
}

/****************************************************************************************************/
/*																									*/
/*		Check Object Attribute																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxCheckObjAttrMdef(
		COBJDEF	*pObjDef,					/* Object Definition									*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		)				/* Message Interface									*/
{
/*--------------------------------------------------------------------------------------------------*/
		if( pObjDef == NULL ) { return (MERR_NO); }						/* Number Error 			*/
/*--------------------------------------------------------------------------------------------------*/
/*		Check AccessLevel																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( (CheckType & MCHK_ACCS_LVL) && (Hmsg != NULL) )
		{
			if(	(pObjDef->AccessLevel) > Hmsg->AccessLvl )
			{
				return( MERR_LEVEL );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Special Use																			*/
/*--------------------------------------------------------------------------------------------------*/
/* <S113> Start :処理変更の為､コメントアウト */
#if 0
		if( CheckType & MCHK_SP_USE )
		{
			if( 0 /* TODO CHK_UNSPUSE() */)
			{
				return( MERR_CHKSPUSE );
			}
		}
#endif
/* <S113> End : 処理変更の為､コメントアウト */

/* <S113> Start */
		if( (CheckType & MCHK_SP_USE) && (Hmsg != NULL) )
		{
			if( (Hmsg->AccessLvl == ACCLVL_USER1) )	/* <S11A> */
			{										/* <S11A> */
				if(Hmsg->OpeDspMode == OPEDSP_SETUPPRM)
				{/* セットアップパラメータモード */
					if( (pObjDef->SpUse & 0xF0) == TUNING_PRM )	/* TUNINGパラメータであった場合は表示しない。 */
					{
						return( MERR_CHKSPUSE );
					}
				}
			}/* <S11A> */
		}
/* <S113> End */

/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		Write Object Method																			*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjWrite(						/* Write Object 										*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		ULONG Data,							/* Write Value 											*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
LONG	i;
LONG	rc;
COBJTBL *pObjTbl = GET_OBJTBL( this );
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );
/*--------------------------------------------------------------------------------------------------*/
/*		Check Data Range																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( LpxObjLimitCheckPdef( pObjDef, Data ) != OK )
		{
			return( OERR_RANGE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Write the Object																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( 1 )
		{
			rc = LpxObjWrite( pObjDef, Data, hMsg );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( rc );
	
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Write Object Method																	*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjWriteS(						/* Write Object 										*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		LONG SubIndex,  					/* Sub Index of the Object Defitnion 					*/
		ULONG Data,							/* Write Value 											*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
LONG	i;
LONG	l;
LONG	rc;
COBJTBL *pObjTbl = GET_OBJTBL( this );
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );
/*--------------------------------------------------------------------------------------------------*/
/*		Check Data Range																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( LpxObjLimitCheckPdef( pObjDef, Data ) != OK )
		{
			return( OERR_RANGE );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Write the Object																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( 1 )
		{
			l = CHK_OBJ_ISLONG( pObjDef );								/* TODO: byte array */
			rc = LpxObjWriteS( pObjDef, SubIndex>>l, Data, hMsg );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( rc );
	
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Write Object Method																	*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjWriteArray(					/* Write Object 										*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		LONG ArrayIndex,					/* Index of the Array 									*/
		LONG WriteSize,						/* Size of the Array 									*/
		ULONG *pWrData,						/* Pointer to the Write Value 							*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
LONG	i;
LONG	rc;
COBJTBL *pObjTbl = GET_OBJTBL( this );
COBJDEF	*pObjDef = GET_OBJDEF( pObjTbl, Index );
/*--------------------------------------------------------------------------------------------------*/
/*		Check Index																					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Index >= this->TblEntNum )
		{
			return ( MERR_NO );								/* Object Number Error					*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Attribute (Access Level, Special Use, etc)											*/
/*--------------------------------------------------------------------------------------------------*/
//@@		rc = LpxCheckObjAttrMdef( pObjDef, CheckType, hMsg );
//@@		if( rc != OK ){ return( rc );}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Array Size 																			*/
/*--------------------------------------------------------------------------------------------------*/
		if ( ( ArrayIndex + WriteSize ) > pObjDef->NumberOfArray )	/* @@ check */
		{
			return ( MERR_NO );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Write Object Data																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i = 0; i < WriteSize; i++ )
		{
			if ( CHK_OBJ_ISLONG( pObjDef ) )
			{
				if( LpxObjLimitCheckPdef( pObjDef, pWrData[i] ) != OK )		/* check data range */
				{
					return( OERR_RANGE );
				}
				rc = LpxObjWriteS( pObjDef, ArrayIndex, pWrData[i], hMsg );	/* write the object */
			}
			else
			{
				if( LpxObjLimitCheckPdef( pObjDef, ((USHORT *)pWrData)[i] ) != OK )		/* check data range */
				{
					return( OERR_RANGE );
				}
				rc = LpxObjWriteS( pObjDef, ArrayIndex, ((USHORT *)pWrData)[i], hMsg );	/* write the object */
			}
			if ( rc != OK ) { break; }
		}
/*--------------------------------------------------------------------------------------------------*/
		return( rc );
}


/****************************************************************************************************/
/*																									*/
/*		Get Object Number																			*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjGetNumber(					/* Write Object			 								*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
COBJTBL *pTbl = (COBJTBL *)this->pTbl;
	if( Index >= this->TblEntNum )
	{
		Index = this->TblEntNum - 1;
	}
	return (pTbl[Index].Number);
}


/****************************************************************************************************/
/*																									*/
/*		Get Object Number																			*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjChkLong(						/* Check Object is Long									*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg )						/* Message Interface 									*/
{
COBJTBL *pTbl = (COBJTBL *)this->pTbl;
	if( Index >= this->TblEntNum )
	{
		Index = this->TblEntNum - 1;
	}
	return (((pTbl[Index].Attribute)->DataLength == 4) ? TRUE : FALSE);
}


/****************************************************************************************************/
/*																									*/
/*		Upper and lower limit check																	*/
/*																									*/
/****************************************************************************************************/
LONG	LpxObjLimitCheckPdef(				/* Check Parameter Limit								*/
		COBJDEF *pObjDef,					/* Pointer to the ObjDef								*/
		ULONG	Data			)			/* Check Data											*/
{
ULONG	i;
/*--------------------------------------------------------------------------------------------------*/
/*		Nibble Parameter																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( GET_OBJBASE( pObjDef ) == OBJDEF_BASENBL ) 
		{
			ULONG	mask = 0x0F;
			ULONG	imax = ( CHK_OBJ_ISLONG( pObjDef ) )? 8 : 4;
		/*------------------------------------------------------------------------------------------*/
			for( i=0; i < imax; i++ )
			{
				if( (Data & mask) < (pObjDef->LowerLimit & mask) ){ return(NG);}
				if( (Data & mask) > (pObjDef->UpperLimit & mask) ){ return(NG);}
				mask = (mask << 4);
			}
			return(OK);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Data range check：Signed																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( CHK_OBJ_SIGNED( pObjDef ) == OBJDEF_W_SIGN )
		{
			if( (LONG)Data > (LONG)pObjDef->UpperLimit ){ return( NG );}
			if( (LONG)Data < (LONG)pObjDef->LowerLimit ){ return( NG );}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Data range check：Unsigned																	*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			if( (ULONG)Data > (ULONG)pObjDef->UpperLimit ){ return( NG );}
			if( (ULONG)Data < (ULONG)pObjDef->LowerLimit ){ return( NG );}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		Write Parameter to RAM 																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxObjWrite(						/* Write Object			 								*/
		COBJDEF *pObjDef,					/* Pointer to the Object Definition						*/
		ULONG	PrmData,					/* Write value											*/
		HMSGIF	Hmsg			)			/* Message Interface									*/
{
LONG	rc;
ULONG	PrmSave;
ULONG	ErrFlag = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		Update Parameter RAM Area																	*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_OBJ_SETVAR( pObjDef, PrmData, 0, Hmsg, PrmSave );

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算実行処理																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		注意：パラメータ計算エラーが起きた場合の処理												*/
/*		エンコーダ通信異常・エンコーダ通信位置データ加速度異常・エンコーダ通信タイマ異常・			*/
/*		エンコーダエコーバック異常アラーム発生時は計算エラー処理しないこと。(SGDSより)				*/
/*--------------------------------------------------------------------------------------------------*/
		KriResetParamError( );									/* Reset Error Flag					*/
		LPX_OBJ_CALC( pObjDef, 0, rc, Hmsg ); 					/* Calculate Parameter				*/
		/*
		 *
		 * TODO:
		 * エンコーダ通信異常時の計算エラー回避処理
		 * (INGRAMではやってないので、いらない？)
		 *
		 */
		if( (/* (Alm.PrmErrInfo.ErrFlag == TRUE) ||*/ (rc != TRUE)) /* && !Alm.AlmAttr.f.NoPceRepair */)
		{
			ErrFlag = 1;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算エラーが起きた場合の処理														*/
/*--------------------------------------------------------------------------------------------------*/
		if( ErrFlag != 0 )
		{
			LPX_OBJ_RESTORE( pObjDef, 0, Hmsg, PrmSave );
		/*------------------------------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				LPX_OBJ_CALC( pObjDef, 0, rc, Hmsg ); 			/* Recalc Parameter					*/
			}
			return( OERR_CALC );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/*		<S073> Write Parameter to RAM 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxObjWriteS(						/* Write Object with Array Index						*/
		COBJDEF *pObjDef,					/* Pointer to the Object Definition						*/
		LONG	ArrayIndex,					/* Index of the Array 									*/
		ULONG	PrmData,					/* Write value											*/
		HMSGIF	Hmsg			)			/* Message Interface									*/
{
LONG	rc;
ULONG	PrmSave;
ULONG	ErrFlag = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		Update Parameter RAM Area																	*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_OBJ_SETVAR( pObjDef, PrmData, ArrayIndex, Hmsg, PrmSave );

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算実行処理																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		注意：パラメータ計算エラーが起きた場合の処理												*/
/*		エンコーダ通信異常・エンコーダ通信位置データ加速度異常・エンコーダ通信タイマ異常・			*/
/*		エンコーダエコーバック異常アラーム発生時は計算エラー処理しないこと。(SGDSより)				*/
/*--------------------------------------------------------------------------------------------------*/
		KriResetParamError( );									/* Reset Error Flag					*/
		LPX_OBJ_CALC( pObjDef, ArrayIndex, rc, Hmsg ); 			/* Calculate Parameter				*/
		/*
		 *
		 * TODO:
		 * エンコーダ通信異常時の計算エラー回避処理
		 * (INGRAMではやってないので、いらない？)
		 *
		 */
		if( (/* (Alm.PrmErrInfo.ErrFlag == TRUE) ||*/ (rc != TRUE)) /* && !Alm.AlmAttr.f.NoPceRepair */)
		{
			ErrFlag = 1;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算エラーが起きた場合の処理														*/
/*--------------------------------------------------------------------------------------------------*/
		if( ErrFlag != 0 )
		{
			LPX_OBJ_RESTORE( pObjDef, ArrayIndex, Hmsg, PrmSave );
		/*------------------------------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				LPX_OBJ_CALC( pObjDef, ArrayIndex, rc, Hmsg ); 			/* Recalc Parameter			*/
			}
			return( OERR_CALC );
		}
/*--------------------------------------------------------------------------------------------------*/
		return( OK );
}

/****************************************************************************************************/
/*																									*/
/* 		<S073>  Search the Next Index with Sub Index		 										*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSearchObjTblNextIdxSub(			/* Search Index of Object Table							*/
		COBJTBL	*pObjTbl,					/* Object Definition Table								*/
		LONG	EntNum,						/* Table Size 											*/
		LONG	SrchNo,						/* Search Number										*/
		LONG	StIdx,						/* Start Index											*/
		LONG	StSubIdx,					/* Start Sub Index										*/
		LONG	*pFindSubIdx,				/* Search Result Sub Index 								*/
		HMSGIF	Hmsg)						/* Message Interface									*/
{
LONG idx;
LONG l;
LONG rtIdx;    /* return index */
LONG subidx;   /* Sub index */
LONG rtSubIdx; /* return Sub index */
LONG dir;      /* Search direction */
/*--------------------------------------------------------------------------------------------------*/
	rtIdx = StIdx;
	rtSubIdx = StSubIdx;
/*--------------------------------------------------------------------------------------------------*/
	if ( SrchNo >= (GET_OBJNUMBER( pObjTbl, StIdx ) + StSubIdx) )
	{ /* forward search */
		dir = UNSEARCH_FORWARD;
	}
	else
	{ /* backword search */
		dir = UNSEARCH_BACKWARD;
	}
/*--------------------------------------------------------------------------------------------------*/
	for ( idx = StIdx; (idx < EntNum) && (idx >= 0); idx = idx + dir )
	{
		if ( LpxCheckObjAttrMdef( GET_OBJDEF( pObjTbl, idx), MCHK_OPE_READ, Hmsg ) != OK ) { continue; }
		/*------------------------------------------------------------------------------------------*/
		rtIdx = idx; /* visible index */
#if 1	/* TODO: LONG */
		/*------------------------------------------------------------------------------------------*/
		/*	Find visible sub index 																	*/
		/*------------------------------------------------------------------------------------------*/
		rtSubIdx = 0;
		l = CHK_OBJ_ISLONG( GET_OBJDEF( pObjTbl, idx ) );					/* TODO: byte araay */
		subidx = ( dir == UNSEARCH_FORWARD ) ? 0 : (Lpx_GetMaxSubIdx( pObjTbl, idx) - (1<<l));
		subidx = ((Lpx_GetMaxSubIdx( pObjTbl, idx) == 0) ? (-1) : subidx);
		/*------------------------------------------------------------------------------------------*/
		for ( /* subidx = StSubIdx */; 
			(subidx < Lpx_GetMaxSubIdx( pObjTbl, idx)) && (subidx >= 0 ); 
			  subidx += (dir<<l) )
		{
			/*--------------------------------------------------------------------------------------*/
			/*	Check Sub Index																		*/
			/*--------------------------------------------------------------------------------------*/
//			rtSubIdx = (( LpxCheckObjAttrIsArray( GET_OBJDEF( pObjTbl, idx), 
//						GET_OBJNUMBER( pObjTbl, idx ) + subidx,
//						GET_OBJNUMBER( pObjTbl, idx) ) ) ? subidx : rtSubIdx );
			/*--------------------------------------------------------------------------------------*/
			/*	Check End Condition																	*/
			/*--------------------------------------------------------------------------------------*/
			if ( (dir == UNSEARCH_FORWARD ) 
					&& (( GET_OBJNUMBER( pObjTbl, idx ) + subidx ) >= SrchNo ) )
			{ 
				rtSubIdx = subidx;
				break; 
			}
			else if ( (dir == UNSEARCH_BACKWARD ) 
					&& (( GET_OBJNUMBER( pObjTbl, idx ) + subidx ) <= SrchNo ) )
			{ 
				rtSubIdx = subidx;
				break; 
			}
		}
#endif
		/*------------------------------------------------------------------------------------------*/
		/*	Check End Condition																		*/
		/*------------------------------------------------------------------------------------------*/
		if ( (dir == UNSEARCH_FORWARD ) 
				&& (( GET_OBJNUMBER( pObjTbl, idx ) + rtSubIdx ) >= SrchNo ) )
		{ 
			break; 
		}
		else if ( (dir == UNSEARCH_BACKWARD ) 
				&& (( GET_OBJNUMBER( pObjTbl, idx ) + rtSubIdx ) <= SrchNo ) )
		{
			break; 
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	*pFindSubIdx = rtSubIdx;								/* set sub index */
/*--------------------------------------------------------------------------------------------------*/
	return (rtIdx);
}

/***************************************** end of file **********************************************/
