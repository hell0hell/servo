/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Object.h : Object Resource Management														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Object Data Type Definition																*/
/*																									*/
/*		2) Object Resource Macro Definition															*/
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
/*		Rev.1.00 : 2013.5.08  K.Ozaki																*/
/*		Rev.1.10 : 2013.9.26  K.Ozaki	<S00C>														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___OBJECT___
#define  ___OBJECT___
#include "KnlApi.h"
#include "PrmAccessIf.h" 			

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef const PRM_ATTR_T COBJDEF;
typedef const PRMTBL COBJTBL;

/*--------------------------------------------------------------------------------------------------*/
/*		Size Conversion			 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define _OBJ_LONG(ptr, sign) ((sign) ? *(LONG *)(ptr) : *(ULONG *)(ptr))
#define _OBJ_SHORT(ptr, sign) ((sign) ? *(SHORT *)(ptr) : *(USHORT *)(ptr))
#define _OBJ_CHAR(ptr, sign) ((sign) ? *(CHAR *)(ptr) : *(UCHAR *)(ptr))

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Table Pointer																	*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_OBJTBL( p )             (COBJTBL *)((p)->pTbl)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_OBJDEF( p, idx )		(((COBJTBL *)(p))[(idx)].Attribute)

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Number																			*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Get Object Data																				*/
/*--------------------------------------------------------------------------------------------------*/
#define GET_OBJDATA( odef, idx, pval, hmsg )  {  /* TODO: hmsg消す */                               \
		if( NULL != (odef)->GetRamPtrCallback )                                                     \
		{                                                                                           \
			void *ptr;                                                                              \
			ptr = (odef)->GetRamPtrCallback( (idx), hmsg->pAxRsc );                                 \
			*(pval) =                                                                               \
				((((odef)->DataLength) == 4) ? _OBJ_LONG( ptr, (odef)->Sign ) :                     \
         		((((odef)->DataLength) == 2) ? _OBJ_SHORT( ptr, (odef)->Sign ) :                    \
			    ((((odef)->DataLength) == 1) ? _OBJ_CHAR( ptr, (odef)->Sign ) : 0)));               \
		}                                                                                           \
		else                                                                                        \
		{                                                                                           \
			*(pval) = (odef)->DefaultValue;                                                         \
			if( NULL != (odef)->PrmCalCallback )                                                    \
			{                                                                                       \
				(odef)->PrmCalCallback( PRM_ACCCMD_READ, (idx), hmsg->pAxRsc, (pval) );             \
			}                                                                                       \
		}                                                                                           \
}

/*--------------------------------------------------------------------------------------------------*/
/*		Write Object (Parameter)<S073>																*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_OBJ_SETVAR( pndef, data, aidx, hmsg, save ) {                                         \
		if( NULL != (pndef)->GetRamPtrCallback )                                                    \
		{                                                                                           \
		void *ptr = (pndef)->GetRamPtrCallback( (aidx)/* Array Index */, hmsg->pAxRsc );            \
				switch (((pndef)->DataLength) )                                                     \
				{                                                                                   \
				case 4:                                                                             \
					(save) = _OBJ_LONG( ptr, (pndef)->Sign );                                       \
					*(ULONG *)(ptr) = (data);                                                       \
					break;                                                                          \
				case 2:                                                                             \
					(save) = _OBJ_SHORT( ptr, (pndef)->Sign );                                      \
					*(USHORT *)(ptr) = (data);                                                      \
					break;                                                                          \
				default:                                                                            \
					(save) = _OBJ_CHAR( ptr, (pndef)->Sign );                                       \
					*(UCHAR *)(ptr) = (data);                                                       \
					break;                                                                          \
				}                                                                                   \
		}                                                                                           \
}

/*--------------------------------------------------------------------------------------------------*/
/*		Restore Object (Parameter) <S073>															*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_OBJ_RESTORE( pndef, aidx, hmsg, save ) {                                              	\
		if( NULL != (pndef)->GetRamPtrCallback )                                                    \
		{                                                                                           \
		void *ptr = (pndef)->GetRamPtrCallback( (aidx)/* Array Index */, hmsg->pAxRsc );            \
				switch (((pndef)->DataLength) )                                                     \
				{                                                                                   \
				case 4:                                                                             \
					*(ULONG *)(ptr) = (save);                                                       \
					break;                                                                          \
				case 2:                                                                             \
					*(USHORT *)(ptr) = (save);                                                      \
					break;                                                                          \
				default:                                                                            \
					*(UCHAR *)(ptr) = (save);                                                       \
					break;                                                                          \
				}                                                                                   \
		}                                                                                           \
}

/*--------------------------------------------------------------------------------------------------*/
/*		 Object (Parameter) Calculation <S073> 														*/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_OBJ_CALC( pndef, aidx, rc, hmsg ) {                                                     \
		if ( NULL != (pndef)->PrmCalCallback )                                                      \
		{                                                                                           \
			(rc) = ((((pndef)->PrmCalCallback)( PRM_ACCCMD_WRITE,                                   \
						(aidx) ,	                                                                \
						/*hmsg->pAxRsc, &PrmData ) >= 0) ? TRUE : FALSE );*/                        \
						hmsg->pAxRsc, (LONG*)&PrmData ) >= 0) ? TRUE : FALSE );                     \
		}                                                                                           \
		else                                                                                        \
		{                                                                                           \
			(rc) = TRUE;                                                                            \
		}                                                                                           \
}



/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _CObject{
	USHORT	TblEntNum;						/* Object Table Entry Number							*/
	USHORT  Reserved;						/* Reserved 											*/
/*--------------------------------------------------------------------------------------------------*/
	void 	*pTbl;							/* Pointer to the Object Table							*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>	LONG	(*Read)(  				/* Read Method ( by using DefPtr ) 						*/
//				struct _CObject *this,		/* this Pointer 										*/
//				COBJDEF *pObjDef,  			/* Pointer to the Parameter Defitnion 					*/
//				ULONG *pResData,			/* Pointer to the Read Value 							*/
//				ULONG CheckType,			/* Error Check Type 									*/
//				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Read)(						/* <S073> Read Method ( by using Index ) 				*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadS)(						/* <S073> Read Method ( by using Index ) 				*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				LONG SubIndex,				/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Search)(						/* Index Search Method									*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchB)(						/* Index Search Method ( by using Base Index ) 			*/
				struct _CObject *this,		/* this Pointer 										*/
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
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*isLong)(						/* Check Specified Object is Long or Short				*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchS)(						/* <S073> Index Search with Sub Index 					*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				LONG	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchBS)(					/* <S073> Next Index Search Method  					*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG 	SrchNo,				/* Number of Un Monitor 								*/
				LONG 	BaseIdx,			/* Start Index of Searching 							*/
				LONG 	BaseSubIdx,			/* Start Sub Index of Searching 						*/
				LONG 	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				MSGIF 	*hMsg );			/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteArray)(					/* <S073> Write Array Data Method 						*/
				struct _CObject *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG WriteSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
} COBJECT;

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
LONG	RpxObjRead(							/* <S073> Read Object Data ( Index Input )				*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	Index,						/* Object Table Index									*/
		ULONG	*pResData,					/* Pointer to the Read Data								*/
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
LONG	RpxObjSearch(						/* Search Object Index									*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Start Index											*/
		LONG	*pFindIdx,					/* Pointer to the Search Index							*/
		ULONG	CheckType,					/* Error Check Type										*/
		HMSGIF	Hmsg		);				/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjSearchIdx(					/* モニタテーブルのインデックス検索処理					*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* モニタ番号											*/
		LONG	BaseIdx,					/* 検索開始インデックス									*/
		HMSGIF	Hmsg			);			/* メッセージIFハンドル									*/
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
LONG	RpxObjGetNumber(					/* Write Object			 								*/
		COBJECT *this,						/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjChkLong(						/* Check Object is Long or not							*/
		COBJECT *this,						/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		MSGIF *hMsg );						/* Message Interface 									*/
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
LONG	RpxObjSearchIdxSub(					/* <S073> Search Object Index (Linear Search)			*/
		COBJECT *this,						/* this Pointer 										*/
		LONG	SrchNo,						/* Object Search Number									*/
		LONG	BaseIdx,					/* Base Index											*/
		LONG	BaseSubIdx,					/* Base Sub Index										*/
		LONG	*pFindSubIdx,				/* Pointer to the Find Sub Index						*/
		HMSGIF	Hmsg			);			/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
LONG	RpxObjWriteArray(					/* <S073> Write Object 									*/
		struct _CObject *this,				/* this Pointer 										*/
		LONG Index,  						/* Index of the Object Defitnion 						*/
		LONG ArrayIndex,					/* Index of the Array 									*/
		LONG WriteSize,						/* Size of the Array 									*/
		ULONG *pWrData,						/* Pointer to the Write Value 							*/
		MSGIF *hMsg );						/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Ｕｎモニタ管理ＡＰＩ定義(<-RndApi.h)														*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Servo Monitor Check Flag																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	MCHK_NONE			0x00
#define	MCHK_ACCS_LVL		0x01
#define	MCHK_SP_USE			0x10
/*--------------------------------------------------------------------------------------------------*/
#define MCHK_REG_READ		MCHK_SP_USE
#define MCHK_OPE_READ		MCHK_ACCS_LVL | MCHK_SP_USE
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		モニタ管理ＩＦ定義(<-RndRpx.h)																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Servo Monitor Object Return Code															*/
/*--------------------------------------------------------------------------------------------------*/
#define	MERR_NO			0x0001
#define	MERR_LEVEL		0x0004
#define	MERR_CHKSPUSE	0x0005

/*--------------------------------------------------------------------------------------------------*/
/*		Display Base																				*/
/*--------------------------------------------------------------------------------------------------*/
#define OBJDEF_BASENBL			(PRMDEF_BASENBL)				/* Attribute：Nibble 				*/
#define OBJDEF_BASEDEC			(PRMDEF_BASEDEC)				/* Attribute：Decimal				*/
#define OBJDEF_BASEHEX			(PRMDEF_BASEHEX)				/* Attribute：Hexadecimal			*/
#define OBJDEF_BASEBIT			(PRMDEF_BASEBIT)				/* Attribute：Bit					*/

/*--------------------------------------------------------------------------------------------------*/
/*		signed or not signed																		*/
/*--------------------------------------------------------------------------------------------------*/
#define OBJDEF_W_SIGN			(TRUE)			/* Signed											*/
#define OBJDEF_NOSIGN			(FALSE)			/* Unsigned											*/

/*--------------------------------------------------------------------------------------------------*/
/*		Parameter Object Return Code																*/
/*--------------------------------------------------------------------------------------------------*/
#define	OERR_NO					0x0001
#define	OERR_DFIT				0x0002
#define	OERR_SIZE				0x0003
#define	OERR_LEVEL				0x0004
#define	OERR_RONLY				0x0005
#define	OERR_MTR_TYPE			0x0006
#define	OERR_AMP_TYPE			0x0007
#define	OERR_CALC				0x0008
#define	OERR_RANGE				0x0009
#define	OERR_FOUND				0x000A
#define OERR_INH				0x000B
#define OERR_PRMCOPY			0x000C
#define OERR_WRITE_HANDLE		0x000D
#define	OERR_OPEDSP				0x000E

/*--------------------------------------------------------------------------------------------------*/
/*		Get Parameter Base		 																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	GET_OBJBASE( pndef )		((pndef)->FormatType) 


/*--------------------------------------------------------------------------------------------------*/
/*		Check the parameter is signed or not	 													*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_OBJ_SIGNED( pndef )   ( (pndef)->Sign )

/*--------------------------------------------------------------------------------------------------*/
/*		Check the parameter is Arrayed or not	 													*/
/*--------------------------------------------------------------------------------------------------*/
#define CHK_OBJ_ISARRAY( pndef )  (( (pndef)->NumberOfArray) )

#endif
/***************************************** end of file **********************************************/
