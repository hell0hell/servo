/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Monitor.h : Monitor Definition Header File													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Un Monitor Access Class Definition														*/
/*																									*/
/*		2) Monitor Register Access Class Definition													*/
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
/*		Rev.1.00 : 2013.4.26  K.Ozaki																*/
/*		Rev.1.10 : 2013.8.9   K.Ozaki																*/
/*		Rev.1.20 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___MONITOR___
#define  ___MONITOR___
#include "KnlApi.h"
#include "Object.h"

/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* 			ONLY IN MERCURY 																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef COBJDEF CMONDEF;						/* Un Monitor Definition */
typedef COBJTBL CMONTBL;						/* Un Monitor Table */

/*--------------------------------------------------------------------------------------------------*/
/*		Get UnMon Table Pointer																		*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_UNMONTBL( obj )      (CMONTBL *)(((COBJECT *)(obj))->pTbl)

/*--------------------------------------------------------------------------------------------------*/
/*		Get UnMon Definition 																		*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_UNMONDEF( tbl, idx ) (((CMONTBL *)tbl)[(idx)].Attribute)

/*--------------------------------------------------------------------------------------------------*/
/*		Check Un Monitor Clear is Enable															*/
/*--------------------------------------------------------------------------------------------------*/
#define	CHK_UNCLEAR_ENABLE(undef)    ((undef)->ClrVar /* TODO */)

/*--------------------------------------------------------------------------------------------------*/
/*		GET Un Monitor Special Use 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_UNSPUSE(undef)           ((undef)->SpUse )

/*--------------------------------------------------------------------------------------------------*/
/*		Check Un Monitor is signed or not	 														*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_UN_SIGNED( undef )   ( (undef)->Sign )

/*--------------------------------------------------------------------------------------------------*/
/*		Get Un Monitor Base 	  																	*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_UNBASE( undef )     ((undef)->FormatType)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Un Monitor Decimals   																	*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_UNDECIMALS( undef ) ((undef)->Decimals)

/*--------------------------------------------------------------------------------------------------*/
#define MONDEF_MAX_UnNO  0xFFF


/*--------------------------------------------------------------------------------------------------*/
/*		Get UnMonitor Variable Address for Single/Multi Drive										*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef CFG_GET_AXIS_VAR_ADR
#define LPX_GET_UNVAR_ADR( pVar )		(pVar)
#else
#define LPX_GET_UNVAR_ADR( pVar )		CFG_GET_AXIS_VAR_ADR( (pVar) )
#endif
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Un Monitor Access Class Definition															*/
/*																									*/
/****************************************************************************************************/
typedef struct _CMonitor{					/* Un Monitor Access Class Definiton					*/
	USHORT	UnTblEntNum;					/* Un Monitor Table Entry Number(Protected)				*/
	USHORT  Reserved;						/* Reserved(Protected) 									*/
/*--------------------------------------------------------------------------------------------------*/
	CMONTBL *pUnMonTbl;						/* Pointer to the Un Monitor Table(Protected)			*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>	LONG	(*Read)(  				/* Read Method ( by using DefPtr ) 						*/
//				COBJECT *this,				/* Pointer to this Object 								*/
//				CMONDEF *pUnDef,  			/* Pointer to the Monitor Defitnion 					*/
//				ULONG *pResData,			/* Pointer to the Read Value 							*/
//				ULONG CheckType,			/* Error Check Type 									*/
//				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Read)(						/* <S073> Read Method ( by using Index ) 				*/
				COBJECT *this,				/* Pointer to this Object 								*/
				LONG Index,					/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadS)(						/* <S073> Read Method ( by using Index ) 				*/
				COBJECT *this,				/* Pointer to this Object 								*/
				LONG Index,					/* Monitor Table Index 									*/
				LONG SubIndex,				/* Monitor Table Sub Index 								*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Search)(						/* Index Search Method									*/
				COBJECT *this,				/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Start Index											*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchB)(						/* Index Search Method ( by using Base Index ) 			*/
				COBJECT *this,				/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadArray)(					/* Read Array Object ( by using Index ) 				*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Write)(  						/* Write Method 										*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				ULONG Data,					/* Write Value 											*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteS)(  					/* <S073> Write Method 									*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG SubIndex,  			/* Index of the Object Defitnion 						*/
				ULONG Data,					/* Write Value 											*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*GetNumber)(					/* Get Object Number from Object Index					*/
				struct _CMonitor *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*isLong)(						/* Check Object is Long or Not 							*/
				struct _CMonitor *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchS)(						/* <S073> Index Search witch Sub Index 					*/
				struct _CMonitor *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				LONG	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchBS)(					/* <S073> Next Index Search Method  					*/
				struct _CMonitor *this,		/* this Pointer 										*/
				LONG 	SrchNo,				/* Number of Un Monitor 								*/
				LONG 	BaseIdx,			/* Start Index of Searching 							*/
				LONG 	BaseSubIdx,			/* Start Sub Index of Searching 						*/
				LONG 	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				MSGIF 	*hMsg );			/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteArray)(					/* <S073> Write Array Data Method 						*/
				struct _CMonitor *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG WriteSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
/*	extended																						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Clear)(						/* Clear Monitor 										*/
				COBJECT *this,				/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
} CMONITOR;

/*--------------------------------------------------------------------------------------------------*/
extern const PRMTBL UnRegTable[];			/* Un Register Table */
extern const ULONG UnRegTableEntNum;		/* Un Register Table Entry Number */
extern const PRMTBL UnMonTable[];			/* Un Monitor Table */
extern const ULONG UnMonTableEntNum;		/* Un Monitor Table Entry Number */
/*--------------------------------------------------------------------------------------------------*/
#define UNMONTBL_ENTNUM   (UnMonTableEntNum)
#define UNMONTBL_PTR      (UnMonTable)
#define UNREGTBL_ENTNUM   (UnRegTableEntNum)
#define UNREGTBL_PTR      (UnRegTable)
/*--------------------------------------------------------------------------------------------------*/
#define MONITOR_NUMBERX (2)
extern CMONITOR LMonitor[];					/* Monitor Access Instance (0:UnMonTbl, 1:UnRegTbl)		*/
#define CUNMONX (&LMonitor[0])				/* The Un Monitor Access Class 							*/
#define CUNREGX (&LMonitor[1])				/* The Un Register Access Class 						*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitMonManager( void );			/* Monitor Manager Initialize							*/
void	RpxPrstMonManager( void );			/* Monitor Manager Parameter Reset						*/

#endif
/***************************************** end of file **********************************************/
