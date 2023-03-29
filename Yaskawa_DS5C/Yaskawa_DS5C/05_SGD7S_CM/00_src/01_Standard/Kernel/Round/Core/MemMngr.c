/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MemMngr.c : Memory Management																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1) Read Memory Object Data																	*/
/*																									*/
/*		2) Search Memory Object 																	*/
/*																									*/
/*		3) Write Memory Object Data																	*/
/*																									*/
/*		4) Check Memory Object Access Level															*/
/*																									*/
/*		5) Check Memory Read/Write Access Check 													*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.09.26  K.Ozaki	<S00C>														*/
/*		Rev.1.10 : 2013.10.08  K.Ozaki	<S012>	Ram Editor											*/
/*		Rev.1.20 : 2013.10.24  K.Ozaki	<S031>	GetBuffer/FreeBuffer								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Memory.h"

#define MEMMNGR_DEBUG (0)			/* 1: debug 0:normal */

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		Public Function																				*/
/*--------------------------------------------------------------------------------------------------*/
void	RpxInitMemManager( void *pBuf, long size );	/* Initialize Function 						*/
void	RpxPrstMemManager( void );					/* Parameter Reset Function 				*/
/*--------------------------------------------------------------------------------------------------*/
LONG	KpiCheckReadMemAddr( ULONG MemAddr, ULONG WordNum );		/* Memory Read Address Check	*/
LONG	KpiCheckWriteMemAddr( ULONG MemAddr, ULONG WordNum );		/* Memory Write Address Check 	*/
/*--------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------*/
/*		Private Function																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG	LpxMemRead(  						/* Read Memory		 									*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,						/* Monitor Table Index 									*/
			ULONG *pResData,				/* Pointer to the Read Value 							*/
			ULONG CheckType,				/* Error Check Type 									*/
			MSGIF *hMsg );					/* Message Interface 									*/
LONG	LpxMemReadS(  						/* <S073> Read Memory		 							*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,						/* Monitor Table Index 									*/
			LONG SubIndex,					/* Monitor Table Index 									*/
			ULONG *pResData,				/* Pointer to the Read Value 							*/
			ULONG CheckType,				/* Error Check Type 									*/
			MSGIF *hMsg );					/* Message Interface 									*/
LONG	LpxMemSearch(						/* Index Search Method									*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG	SrchNo,					/* Search Number										*/
			LONG	BaseIdx,				/* Search Base Index									*/
			LONG	*pFindIdx,				/* Pointer to the Found Index							*/
			ULONG	CheckType,				/* Error Check Type										*/
			MSGIF	*hMsg			);		/* Message Interface									*/
LONG	LpxMemSearchS(						/* Index Search Method	<S073>							*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG	SrchNo,					/* Search Number										*/
			LONG	BaseIdx,				/* Search Base Index									*/
			LONG	*pFindIdx,				/* Pointer to the Found Index							*/
			LONG	*pFindSubIdx,			/* Pointer to the Found Index							*/
			ULONG	CheckType,				/* Error Check Type										*/
			MSGIF	*hMsg			);		/* Message Interface									*/
LONG	LpxMemSearchIdx(					/* Index Search Method ( by using Base Index ) 			*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG SrchNo,					/* Number of Un Monitor 								*/
			LONG BaseIdx,					/* Start Index of Searching 							*/
			MSGIF *hMsg );					/* Message Interface 									*/
LONG	LpxMemSearchIdxS(					/* Index Search Method ( by using Base Index ) 			*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG SrchNo,					/* Number of Un Monitor 								*/
			LONG BaseIdx,					/* Start Index of Searching 							*/
			LONG *pFindSubIdx,				/* Pointer to the Found Index							*/
			MSGIF *hMsg );					/* Message Interface 									*/
LONG	LpxMemWrite(  						/* Write Method 										*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,  					/* Index of the Object Definition 						*/
			ULONG Data,						/* Write Value 											*/
			MSGIF *hMsg );					/* Message Interface 									*/
LONG	LpxMemWriteS(  					/* <S073> Write Method 									*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,  					/* Index of the Object Definition 						*/
			LONG SubIndex,  				/* Index of the Object Definition 						*/
			ULONG Data,						/* Write Value 											*/
			MSGIF *hMsg );					/* Message Interface 									*/
LONG	LpxMemReadArray(					/* Read Array Object ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
LONG	LpxMemWriteArray(					/* Write Array Object ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
//				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg );				/* Message Interface 									*/
LONG	LpxMemGetNumber(					/* Get Object Number from Object Index					*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Definition 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
LONG	LpxMemChkSize(						/* Check Object Data Size is Long or not				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Definition 						*/
				MSGIF *hMsg );				/* Message Interface 									*/
/*--------------------------------------------------------------------------------------------------*/
void*	LpxMemGetBuffer(					/* <S031> Get Work Memory for the User					*/
				struct _CMemory *this,		/* this pointer											*/
				void *hUser );				/* pointer to the user 									*/
void	LpxMemFreeBuffer(					/* <S031> Free Work Memory from the User				*/
				struct _CMemory *this,		/* this pointer											*/
				void *hUser );				/* pointer to the user 									*/



/****************************************************************************************************/
/*																									*/
/*		Private Macro Definition																	*/
/*																									*/
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		Valiable Definition																			*/
/*																									*/
/****************************************************************************************************/
CMEMORY LMemory[1];
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Initialize Function																			*/
/*																									*/
/****************************************************************************************************/
void	RpxInitMemManager( void *pBuf, long size )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Set	Table Entry Number																		*/
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->TblEntNum = (USHORT)0;
		CTRCMEM->pTbl = pBuf;							/* @@ 後で消す */
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->Read = LpxMemRead;
//		CTRCMEM->ReadX = LpxMemReadIdx;
		CTRCMEM->ReadS = LpxMemReadS;					/* <S073> */
		CTRCMEM->Search = LpxMemSearch;
		CTRCMEM->SearchB = LpxMemSearchIdx;
		CTRCMEM->ReadArray = LpxMemReadArray;
		CTRCMEM->Write = LpxMemWrite;
		CTRCMEM->WriteS = LpxMemWriteS;					/* <S073> */
		CTRCMEM->GetNumber = LpxMemGetNumber;
		CTRCMEM->isLong = LpxMemChkSize;
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->SearchS = LpxMemSearchS;				/* <S073> */
		CTRCMEM->SearchBS = LpxMemSearchIdxS;			/* <S073> */
		CTRCMEM->WriteArray = LpxMemWriteArray;			/* <S073> */
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->AdrsMsk = size-1;
/*--------------------------------------------------------------------------------------------------*/
		CTRCMEM->GetBuffer = LpxMemGetBuffer;			/* <S031> */
		CTRCMEM->FreeBuffer = LpxMemFreeBuffer;			/* <S031> */
		CTRCMEM->pWrkBufUser = NULL;					/* <S031> */
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Parameter Reset Function																	*/
/*																									*/
/****************************************************************************************************/
void	RpxPrstMemManager( void )
{
		return;
}

/****************************************************************************************************/
/*																									*/
/*		Read Memory ( not supported ) 																*/
/*																									*/
/****************************************************************************************************/
#if 0
LONG	LpxMemRead(  						/* Read Memory		 									*/
			struct _CMemory *this,			/* this Pointer 										*/
			void *pObjDef,  				/* Pointer to the Parameter Definition 					*/
			ULONG *pResData,				/* Pointer to the Read Value 							*/
			ULONG CheckType,				/* Error Check Type 									*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
	return ( OK );
}
#endif


/****************************************************************************************************/
/*																									*/
/*		Read Memory 				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemRead(						/* Read Method ( by using Index ) 						*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,						/* Monitor Table Index 									*/
			ULONG *pResData,				/* Pointer to the Read Value 							*/
			ULONG CheckType,				/* Error Check Type 									*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
USHORT *pBuf = (USHORT *)this->pTbl;				/* @@ */
	*pResData = pBuf[Index & (this->AdrsMsk)];
	return ( OK );
}

/****************************************************************************************************/
/*																									*/
/*		Read Memory 				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemReadS(						/* Read Method ( by using Index ) 						*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,						/* Monitor Table Index 									*/
			LONG SubIndex,					/* Monitor Table Index 									*/
			ULONG *pResData,				/* Pointer to the Read Value 							*/
			ULONG CheckType,				/* Error Check Type 									*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
	return LpxMemRead( this, Index, pResData, CheckType, hMsg );
}


/****************************************************************************************************/
/*																									*/
/*		Search Memory 				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemSearch(						/* Index Search Method									*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG	SrchNo,					/* Search Number										*/
			LONG	BaseIdx,				/* Search Base Index									*/
			LONG	*pFindIdx,				/* Pointer to the Found Index							*/
			ULONG	CheckType,				/* Error Check Type										*/
			MSGIF	*hMsg			)		/* Message Interface									*/
{
	*pFindIdx = SrchNo;
	return ( OK );
}

/****************************************************************************************************/
/*																									*/
/*		Search Memory <S073>		 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemSearchS(						/* Index Search Method									*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG	SrchNo,					/* Search Number										*/
			LONG	BaseIdx,				/* Search Base Index									*/
			LONG	*pFindIdx,				/* Pointer to the Found Index							*/
			LONG	*pFindSubIdx,			/* Pointer to the Found Index							*/
			ULONG	CheckType,				/* Error Check Type										*/
			MSGIF	*hMsg			)		/* Message Interface									*/
{
	*pFindIdx = SrchNo;
	*pFindSubIdx = 0;
	return ( OK );
}


/****************************************************************************************************/
/*																									*/
/*		Search Index 				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemSearchIdx(					/* Index Search Method ( by using Base Index ) 			*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG SrchNo,					/* Number of Un Monitor 								*/
			LONG BaseIdx,					/* Start Index of Searching 							*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
	return (SrchNo);
}

/****************************************************************************************************/
/*																									*/
/*		Search Index 				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemSearchIdxS(					/* Index Search Method ( by using Base Index ) 			*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG SrchNo,					/* Number of Un Monitor 								*/
			LONG BaseIdx,					/* Start Index of Searching 							*/
			LONG	*pFindSubIdx,			/* Pointer to the Found Sub Index						*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
	*pFindSubIdx = 0;
	return (SrchNo);
}


/****************************************************************************************************/
/*																									*/
/*		Write Memory  				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemWrite(  						/* Write Method 										*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,  					/* Index of the Object Defitnion 						*/
			ULONG Data,						/* Write Value 											*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
USHORT *pBuf = (USHORT *)this->pTbl;				/* @@ */
	pBuf[Index & (this->AdrsMsk)] = (USHORT)Data;
	return (OK);
}

/****************************************************************************************************/
/*																									*/
/*		Write Memory  				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemWriteS(  						/* Write Method 										*/
			struct _CMemory *this,			/* this Pointer 										*/
			LONG Index,  					/* Index of the Object Defitnion 						*/
			LONG SubIndex,  				/* Index of the Object Defitnion 						*/
			ULONG Data,						/* Write Value 											*/
			MSGIF *hMsg )					/* Message Interface 									*/
{
	return LpxMemWrite( this, Index, Data, hMsg );
}


/****************************************************************************************************/
/*																									*/
/*		Read Array   				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemReadArray(					/* Read Array Object ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pResData,			/* Pointer to the Read Value 							*/
				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
LONG i;
LONG base= Index + ArrayIndex;
USHORT *pBuf = (USHORT *)this->pTbl;				/* @@ */
	for( i=0; i < ReadSize; i++ )
	{
		pResData[i] = pBuf[(base+i) & (this->AdrsMsk)];
	}
	return (OK);
}


/****************************************************************************************************/
/*																									*/
/*		Write Array   				 																*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemWriteArray(					/* Read Array Object ( by using Index ) 				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,					/* Index of the Object 									*/
				LONG ArrayIndex,			/* Index of the Array 									*/
				LONG ReadSize,				/* Size of the Array 									*/
				ULONG *pWrData,				/* Pointer to the Write Value 							*/
//				ULONG CheckType,			/* Error Check Type 									*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
LONG i;
LONG base= Index + ArrayIndex;
USHORT *pBuf = (USHORT *)this->pTbl;				/* @@ */
	for( i=0; i < ReadSize; i++ )
	{
		pBuf[(base+i) & (this->AdrsMsk)] = (USHORT)pWrData[i];
	}
	return (OK);
}

/****************************************************************************************************/
/*																									*/
/*	 Get Object Number from Object Index															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemGetNumber(					/* Get Object Number from Object Index					*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Definition 						*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
	return Index;
}

/****************************************************************************************************/
/*																									*/
/*	 Get Object Number from Object Index															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxMemChkSize(						/* Check Object Data Size is Long or not				*/
				struct _CMemory *this,		/* this Pointer 										*/
				LONG Index,  				/* Index of the Object Definition 						*/
				MSGIF *hMsg )				/* Message Interface 									*/
{
	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		メモリ読出しアドレスチェック	<S012>														*/
/*		Memory Read Address Check																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : メモリ読出し時のメモリアドレスのチェックを行う。										*/
/*																									*/
/*	用 途 : 1) 補助機能(Fn103)	: RAM Editor														*/
/*			2) メッセージ処理	: メモリ読出し														*/
/*																									*/
/*	備 考 : 1) アナログモニタ：変数アドレス指定は別管理												*/
/*			2) データトレース：トレース対象アドレス指定は別管理										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	KpiCheckReadMemAddr( ULONG MemAddr, ULONG WordNum )
{
#define CPU_MEMADRMSK 0xFFFFFFFF
ULONG	i;
ULONG	EndAddr;
		if ( MEMMNGR_DEBUG != 0 ){ return MEMDEF_CPURAM; }

/*--------------------------------------------------------------------------------------------------*/
/*		Memory Address : Get Effective Address														*/
/*--------------------------------------------------------------------------------------------------*/
		MemAddr = MemAddr & CPU_MEMADRMSK;
		EndAddr = MemAddr + (2*WordNum) - 1;
		if( EndAddr <= MemAddr ){ return( MEMDEF_READ_NG );}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Read Memory																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MEMDEFTBL_ENTNUM	/* zantei MemMngP.MemDefTblEntNum */; i++ )
		{
			if( (MemDefTbl[i].MemBgnAddr <= MemAddr) && (EndAddr <= MemDefTbl[i].MemEndAddr) )
			{
				if( MemDefTbl[i].Attr.Read == 0 )
				{
					return( MEMDEF_READ_NG );
				}
//Mercury		else if( MemDefTbl[i].AxisNo > MemMngP.AxisNumber )
//				{
//					return( MEMDEF_READ_NG );
//				}
				else
				{
					return( MemDefTbl[i].MemDefID );
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( MEMDEF_READ_NG );
}

/****************************************************************************************************/
/*																									*/
/*		メモリ書込みアドレスチェック	<S012>														*/
/* 		Memory Write Address Check 																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : メモリ書込み時のメモリアドレスのチェックを行う。										*/
/*																									*/
/*	用 途 : 1) 補助機能(Fn103)	: RAM Editor														*/
/*			2) メッセージ処理	: メモリ書込み														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG	KpiCheckWriteMemAddr( ULONG MemAddr, ULONG WordNum )
{
ULONG	i;
ULONG	EndAddr;
		
		if ( MEMMNGR_DEBUG != 0 ){ return MEMDEF_CPURAM; }
/*--------------------------------------------------------------------------------------------------*/
/*		Memory Address : Get Effective Address														*/
/*--------------------------------------------------------------------------------------------------*/
		MemAddr = MemAddr & CPU_MEMADRMSK;
		EndAddr = MemAddr + (2*WordNum) - 1;
		if( EndAddr <= MemAddr ){ return( MEMDEF_WRITE_NG );}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Write Memory																			*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MEMDEFTBL_ENTNUM	/* zantei MemMngP.MemDefTblEntNum */; i++ )
		{
			if( (MemDefTbl[i].MemBgnAddr <= MemAddr) && (EndAddr <= MemDefTbl[i].MemEndAddr) )
			{
				if( MemDefTbl[i].Attr.Write == 0 )
				{
					return( MEMDEF_WRITE_NG );
				}
//Mercury		else if( MemDefTbl[i].AxisNo > MemMngP.AxisNumber )
//				{
//					return( MEMDEF_WRITE_NG );
//				}
				else
				{
					return( MemDefTbl[i].MemDefID );
				}
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( MEMDEF_WRITE_NG );
}


/****************************************************************************************************/
/*																									*/
/* 		Get Memory Access Size  <S012>																*/
/*																									*/
/****************************************************************************************************/
LONG	KpiGetMemAccessSize( ULONG MemAddr )
{
ULONG	i;
/*--------------------------------------------------------------------------------------------------*/
/*		Memory Address : Get Effective Address														*/
/*--------------------------------------------------------------------------------------------------*/
		MemAddr = MemAddr & CPU_MEMADRMSK;
/*--------------------------------------------------------------------------------------------------*/
/*		Find Memory	Access Size																		*/
/*--------------------------------------------------------------------------------------------------*/
		for( i=0; i < MEMDEFTBL_ENTNUM; i++ )
		{
			if( (MemDefTbl[i].MemBgnAddr <= MemAddr) && (MemAddr <= MemDefTbl[i].MemEndAddr) )
			{
				return( MemDefTbl[i].AccessSize );
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		return( 2 );
}


/****************************************************************************************************/
/*																									*/
/* 		Find Next Accesable Memory Address  <S012>													*/
/*		return: Found Address																		*/
/*																									*/
/****************************************************************************************************/
ULONG	KpiFindMemNextAddress( ULONG SrchAddr, ULONG BaseAddr )
{
#define MEM_FORWARD (1)
#define MEM_BACKWARD (-1)
LONG	i;
ULONG	Dir;
LONG	Area = -1;
/*--------------------------------------------------------------------------------------------------*/
/*		Memory Address : Get Effective Address														*/
/*--------------------------------------------------------------------------------------------------*/
		SrchAddr = SrchAddr & CPU_MEMADRMSK;
/*--------------------------------------------------------------------------------------------------*/
/*		Set Direction																				*/
/*--------------------------------------------------------------------------------------------------*/
		Dir = ( ( SrchAddr >= BaseAddr ) ? MEM_FORWARD : MEM_BACKWARD );
/*--------------------------------------------------------------------------------------------------*/
/*		Find Memory	Address and Size																*/
/*--------------------------------------------------------------------------------------------------*/
		if ( Dir == MEM_FORWARD )
		{
		/*------------------------------------------------------------------------------------------*/
		/*		Forward Search 																		*/
		/*------------------------------------------------------------------------------------------*/
			for( i=0; i < MEMDEFTBL_ENTNUM; i++ )
			{
				if( (MemDefTbl[i].MemBgnAddr <= SrchAddr) && (SrchAddr <= MemDefTbl[i].MemEndAddr) )
				{
					Area = i;
					break;
				}
				else if ( (i != (MEMDEFTBL_ENTNUM-1)) && ( SrchAddr < MemDefTbl[i+1].MemBgnAddr ) )
				{
					SrchAddr = MemDefTbl[i+1].MemBgnAddr;
					Area = i+1;
					break;
				}
			}
		/*------------------------------------------------------------------------------------------*/
			if ( Area < 0 )
			{
				if ( MemDefTbl[MEMDEFTBL_ENTNUM-1].AccessSize == 4 )
				{
					return (((MemDefTbl[MEMDEFTBL_ENTNUM-1].MemEndAddr)>>2)<<2);	/* not found */
				}
				else 
				{
					return (((MemDefTbl[MEMDEFTBL_ENTNUM-1].MemEndAddr)>>1)<<1);	/* not found */
				}
			}
		}
		else
		{
		/*------------------------------------------------------------------------------------------*/
		/*		Backword Search 																	*/
		/*------------------------------------------------------------------------------------------*/
			for( i=(MEMDEFTBL_ENTNUM-1); i >= 0 ; i-- )
			{
				if( (MemDefTbl[i].MemBgnAddr <= SrchAddr) && (SrchAddr <= MemDefTbl[i].MemEndAddr) )
				{
					Area = i;
					break;
				}
				else if (( i != 0 ) && ( SrchAddr > MemDefTbl[i-1].MemEndAddr ))
				{
					SrchAddr = MemDefTbl[i-1].MemEndAddr;
					Area = i-1;
					break;
				}
			}
		/*------------------------------------------------------------------------------------------*/
			if ( Area < 0 )
			{
				return (MemDefTbl[0].MemBgnAddr);				/* not found */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
//		if ( Area < 0 )
//		{
//			return (BaseAddr);									/* not found */
//		}
/*--------------------------------------------------------------------------------------------------*/
/*		Set Next Address																			*/
/*--------------------------------------------------------------------------------------------------*/
		if ( MemDefTbl[Area].AccessSize == 4 )
		{
			while (SrchAddr & 0x03){ SrchAddr += Dir; }			/* long access */
		}
		else if ( MemDefTbl[Area].AccessSize == 2 )
		{
			while (SrchAddr & 0x01){ SrchAddr += Dir; }			/* short access */
		}
		else
		{
			;	/* ??? */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Check Area Over 																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MemDefTbl[Area].MemBgnAddr <= SrchAddr) && (SrchAddr <= MemDefTbl[Area].MemEndAddr) )
		{
			return (SrchAddr);
		}
		else if ( Dir == MEM_FORWARD )
		{
			/*--------------------------------------------------------------------------------------*/
			/* 	Area Over : Find Next Address														*/
			/*--------------------------------------------------------------------------------------*/
			if ( ( Area + 1 ) < MEMDEFTBL_ENTNUM )
			{
				return MemDefTbl[Area+1].MemBgnAddr;
			}
			else
			{
				return (BaseAddr);									/* not found */
			}
		}
		else 
		{
			/*--------------------------------------------------------------------------------------*/
			/* 	Area Over : Find Next Address														*/
			/*--------------------------------------------------------------------------------------*/
			if ( Area > 0 )
			{
				return MemDefTbl[Area-1].MemEndAddr;
			}
			else
			{
				return (BaseAddr);									/* not found */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
//		return( BaseAddr );//<S0C7>
}


/****************************************************************************************************/
/*																									*/
/* 		Get Work Buffer for the User  <S031>														*/
/*		return: normal/buffer address	error/null													*/
/*																									*/
/****************************************************************************************************/
void*	LpxMemGetBuffer(					/* @@ Get Work Memory for the User						*/
				struct _CMemory *this,		/* this pointer											*/
				void *hUser )				/* pointer to the user 									*/
{
/*--------------------------------------------------------------------------------------------------*/
/*		Check pTrcBufUser																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_DI( );
		if( this->pWrkBufUser == NULL )
		{
			this->pWrkBufUser = hUser;
		}
		else if( this->pWrkBufUser != hUser )
		{
			KPI_EI( );
			return( NULL );
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Return with pTrcBuf																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_EI( );
		return( (void*)(this->pTbl) /* @@ check */);
}

/****************************************************************************************************/
/*																									*/
/* 		Free Work Buffer from the User  <S031>														*/
/*																									*/
/****************************************************************************************************/
void	LpxMemFreeBuffer(					/* @@ Free Work Memory from the User					*/
				struct _CMemory *this,		/* this pointer											*/
				void *hUser )				/* pointer to the user 									*/
{
	if( this->pWrkBufUser == hUser )
	{
		this->pWrkBufUser = NULL;
	}	
}

/***************************************** end of file **********************************************/
