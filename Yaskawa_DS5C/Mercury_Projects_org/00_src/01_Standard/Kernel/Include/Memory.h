/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Memory.h : Memory Object Resource Management												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Memory Data Type Definition																*/
/*																									*/
/*		2) Memory Resource Macro Definition															*/
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
/*		Rev.1.00 : 2013.09.26  K.Ozaki	<S00C>														*/
/*		Rev.1.01 : 2013.10.08  K.Ozaki	<S012>	Ram Editor 											*/
/*		Rev.1.02 : 2013.10.24  K.Ozaki	<S031>	Parameter Download 									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef ___MEMORY___
#define  ___MEMORY___
#include "Object.h"

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		��������`�\����																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		��������`�\����																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct	{
		struct	{								/* ������`											*/
		UINT	Read     :1;					/* Read����											*/
		UINT	Write    :1;					/* Write����										*/
		UINT	IchkWRV  :1;					/* �����f�f�FWrite/Read/Verify						*/
		} Attr;									/* ����������										*/
	/*----------------------------------------------------------------------------------------------*/
		ULONG	MemBgnAddr;						/* �������擪�A�h���X								*/
		ULONG	MemEndAddr;						/* �������ŏI�A�h���X								*/
//Mercury		USHORT	AxisNo;					/* ���ԍ�(AxisNo > 0�F����p������)					*/
		USHORT 	AccessSize;						/* �������A�N�Z�X�T�C�Y[byte]						*/
		USHORT	MemDefID;						/* ��������`�h�c									*/
} MEMDEF;
typedef const MEMDEF CMEMDEF;					/* const ��������`�\����							*/
/*--------------------------------------------------------------------------------------------------*/
/*		MEMDEF Version																				*/
/*--------------------------------------------------------------------------------------------------*/
#define MEMDEF_VERSION			0x0001			/* MEMDEF Version									*/


/****************************************************************************************************/
/*																									*/
/*		��������`�h�c																				*/
/*																									*/
/****************************************************************************************************/
#define MEMDEF_CPUROM			0x0001			/* CPU ROM											*/
#define MEMDEF_CPURAM			0x0002			/* CPU RAM											*/
#define MEMDEF_CPUIO			0x0003			/* CPU I/O											*/
#define MEMDEF_AREG				0x0004			/* ASIC H/W Register								*/
#define MEMDEF_MREG				0x0005			/* ASIC Micro Register								*/
#define MEMDEF_EXTROM			0x0006			/* External ROM										*/
#define MEMDEF_EXTRAM			0x0007			/* External RAM										*/
#define MEMDEF_DPRAM			0x0008			/* Dual Port RAM									*/
#define MEMDEF_OTHER			0x0009			/* Other Memory										*/
#define MEMDEF_READ_NG			0x0000			/* Read Memory NG									*/
#define MEMDEF_WRITE_NG			0x0000			/* Write Memory NG									*/
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Struct Definition																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _CMemory{
	USHORT	TblEntNum;						/* ( not used ) Object Table Entry Number				*/
	USHORT  Reserved;						/* Reserved 											*/
/*--------------------------------------------------------------------------------------------------*/
	void 	*pTbl;							/* ( not used ) Pointer to the Object Table				*/
/*--------------------------------------------------------------------------------------------------*/
//<S073>	LONG	(*Read)(  				/* ( not supported ) 									*/
//				struct _CMemory *this,		/* this Pointer 										*/
//				void *pObjDef,  			/* Pointer to the Parameter Defitnion 					*/
//				ULONG *pResData,			/* Pointer to the Read Value 							*/
//				ULONG CheckType,			/* Error Check Type 									*/
//				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Read)(						/* <S073> Read Method ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadS)(						/* <S073> Read Method ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Monitor Table Index 									*/
				LONG SubIndex,				/* Monitor Table Index 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Search)(						/* Index Search Method									*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchB)(						/* Index Search Method ( by using Base Index ) 			*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG SrchNo,				/* Number of Un Monitor 								*/
				LONG BaseIdx,				/* Start Index of Searching 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*ReadArray)(					/* Read Array Object ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*Write)(  						/* Write Method 										*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				ULONG Data,					/* Write Value 											*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteS)(  					/* <S073> Write Method 									*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG SubIndex,  			/* Index of the Object Defitnion 						*/
				ULONG Data,					/* Write Value 											*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*GetNumber)(					/* Get Object Number from Object Index					*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*isLong)(						/* Check Object Size is Long or Not						*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchS)(						/* <S073> Index Search with Sub Index 					*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG	SrchNo,				/* Search Number										*/
				LONG	BaseIdx,			/* Search Base Index									*/
				LONG	*pFindIdx,			/* Pointer to the Found Index							*/
				LONG	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				ULONG	CheckType,			/* Error Check Type										*/
				MSGIF	*hMsg			);	/* Message Interface									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*SearchBS)(					/* <S073> Next Index Search Method  					*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG 	SrchNo,				/* Number of Un Monitor 								*/
				LONG 	BaseIdx,			/* Start Index of Searching 							*/
				LONG 	BaseSubIdx,			/* Start Sub Index of Searching 						*/
				LONG 	*pFindSubIdx,		/* Pointer to the Found Sub Index						*/
				MSGIF 	*hMsg );			/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	(*WriteArray)(					/* @@ Write Array Data Method 							*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Defitnion 						*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG WriteSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	AdrsMsk;						/* @@ Memory Address Mask								*/
/*--------------------------------------------------------------------------------------------------*/
	void*	(*GetBuffer)(					/* <S031> Get Work Memory for the User					*/
				struct _CMemory *this,		/* this pointer											*/
				void *hUser );				/* pointer to the user 									*/
/*--------------------------------------------------------------------------------------------------*/
	void	(*FreeBuffer)(					/* <S031> Free Work Memory from the User				*/
				struct _CMemory *this,		/* this pointer											*/
				void *hUser );				/* pointer to the user 									*/
/*--------------------------------------------------------------------------------------------------*/
	void	*pWrkBufUser;					/* <S031> Pointer to the Work Buffer User				*/
} CMEMORY;

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		MemMngr.c	<S013>																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	KpiCheckReadMemAddr( ULONG MemAddr, ULONG WordNum );  /* Memory Read Address Check <S012>	*/
LONG	KpiCheckWriteMemAddr( ULONG MemAddr, ULONG WordNum ); /* Memory Write Address Check <S012>	*/ 
LONG	KpiGetMemAccessSize( ULONG MemAddr ); 				  /* Get Memory Access Size  <S012>		*/
ULONG	KpiFindMemNextAddress( ULONG SrchAddr, 		/* Find Next Accesable Memory Address  <S012>	*/
				ULONG BaseAddr ); 

extern CMEMORY LMemory[];
#define CTRCMEM (&LMemory[0])
#define TRCMEMSIZE (0x2000 * 2)			/* @@  Trace memory size ( short ) */


/****************************************************************************************************/
/*																									*/
/*		��������`�e�[�u��Extern�錾																*/
/*																									*/
/****************************************************************************************************/
extern  const MEMDEF MemDefTbl[];				/* ��������`�e�[�u��								*/
extern  const USHORT MemDefTblEntNum;			/* ��������`�e�[�u���o�^��						*/

/*--------------------------------------------------------------------------------------------------*/
/*		Define Entry Number																			*/
/*--------------------------------------------------------------------------------------------------*/
#define MEMDEFTBL_ENTNUM	MemDefTblEntNum

#endif
/***************************************** end of file **********************************************/
