/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LcdPdl.h : Lcd Operator Parameter Download Management										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Lcd Operator Parameter Download Type Definition											*/
/*																									*/
/*		2) Lcd Operator Parameter Download Macro Definition											*/
/*																									*/
/*		3) 																							*/
/*																									*/
/*		4) 																							*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.10.24  K.Ozaki	<S031>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___LCDPDL___
#define  ___LCDPDL___
#include "Object.h"

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
#if 0
struct _CLcdPdl{
	USHORT	TblEntNum;						/* Parameter Download Table Entry Number				*/
	USHORT  Reserved;						/* Reserved 											*/
/*--------------------------------------------------------------------------------------------------*/
	void 	*pTbl;							/* Pointer to the Parameter Download Table				*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Read)(  						/* ( not supported ) 									*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				void *pObjDef,  			/* Pointer to the Parameter Defitnion 					*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadX)(						/* Read Method ( by using Index ) 						*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Search)(						/* Index Search Method									*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchB)(						/* Index Search Method ( by using Base Index ) 			*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadArray)(					/* Read Array Object ( by using Index ) 				*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Write)(  						/* Write Method 										*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				ULONG Data,					/* Write Value 											*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*GetNumber)(					/* Get Object Number from Object Index					*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*isLong)(						/* Check Object Size is Long or Not						*/
				struct _CLcdPdl *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
} ;
#endif

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
#if 0
extern struct _CLcdPdl LLcdPdl[];
#define CLCDPDL (&LcdPdl[0])
#endif


/****************************************************************************************************/
/*																									*/
/*		Parameter Download Table Extern																*/
/*																									*/
/****************************************************************************************************/
extern  const PDLTBL LcdPdlTbl[];				/* Lcd Operator Parameter Download Table			*/
extern  const USHORT LcdPdlTblEntNum;			/* Entry Number of Parameter Download Table			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Define Entry Number																			*/
/*--------------------------------------------------------------------------------------------------*/
#define LCDPDLTBL_ENTNUM	LcdPdlTblEntNum

#endif
/***************************************** end of file **********************************************/
