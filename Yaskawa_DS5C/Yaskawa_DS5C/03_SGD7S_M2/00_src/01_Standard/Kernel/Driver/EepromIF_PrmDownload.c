/****************************************************************************************************/
/*																									*/
/*																									*/
/*		EepromIF_PrmDownload.c : EEPROM Parameter Download 											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		Function:																					*/
/*			1.EEPROM Parameter Upload 																*/
/*			2.EEPROM Parameter Download 															*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.09.26  K.Ozaki		<S00C>													*/
/*		Rev.1.10 : 2013.10.02  K.Ozaki 		Bug Fix	<S00F>											*/
/*		Rev.1.1x : 2014.01.08  K.Ozaki 		SGD7W <S0E4>											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"

/****************************************************************************************************/
/*																									*/
/*		マクロ定義																					*/
/*																									*/
/****************************************************************************************************/
#define EEPADR_BYTE 			/* EEPROMデバイストライバのアドレスがBYTEの場合(Mercury)<S00F> */
#ifdef EEPADR_BYTE 
	#define ADDRSFT (1)
#else
	#define ADDRSFT (0)
#endif

#ifdef EEPSIZE_TOTAL									/* <S0E4> */
	#undef EEPSIZE_TOTAL
	#define EEPSIZE_TOTAL (MAX_AXIS_NUMBER * (0x800))	/* 2K word * Axis Number */
#endif

/****************************************************************************************************/
/*																									*/
/*		ＥＥＰＲＯＭ管理用データ定義																*/
/*																									*/
/****************************************************************************************************/
//<S00F> static	RWALLMD	RwAllEepMng;			/* Read/Write All EEPROM Management Data			*/

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		External Function (EepromIf.c)																*/
/*--------------------------------------------------------------------------------------------------*/
extern void	LpxWriteEepromEnable( void );				/* TODO: 後で消す EEPROM Write Enable		*//* <S0E4> */
extern void	LpxWriteEepromDisable( void );				/* TODO: 後で消す EEPROM Write Disable		*//* <S0E4> */

/*--------------------------------------------------------------------------------------------------*/
/*		APIs																						*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitDownloadEeprom( RWALLMD *pRwAllMng );	/* Initialize Download EEPROM				*/
void	KpxDownloadEeprom( RWALLMD *pRwAllMng );		/* Download EEPROM							*/
LONG	KriRequestReadAllEeprom( RWALLMD *pRwAllMng, USHORT *ReadBuf );
LONG	KriRequestWriteAllEeprom( RWALLMD *pRwAllMng, USHORT *WriteBuf );
LONG	KriChkDoneReadAllEeprom( RWALLMD *pRwAllMng );
LONG	KriChkDoneWriteAllEeprom( RWALLMD *pRwAllMng );

/*--------------------------------------------------------------------------------------------------*/
/*		Local Function 																				*/
/*--------------------------------------------------------------------------------------------------*/
static void	LpxReadAllEepromData( RWALLMD *pRwAllMng );	/* Read  All Eeprom Data					*/
static void	LpxWriteAllEepromData( RWALLMD *pRwAllMng );/* Write All Eeprom Data					*/

#define LpxGetEepromSemaphore( WAIT ) { ;}				/* TODO: Get EepromSemaphore				*/
#define LpxFreeEepromSemaphore( ) { ;}					/* TODO: Free EepromSemaphore				*/


/****************************************************************************************************/
/*																									*/
/*		Initiazlie Download EEPROM																	*/
/*																									*/
/****************************************************************************************************/
void	KpxInitDownloadEeprom( RWALLMD *pRwAllMng )
{
	MlibResetLongMemory( pRwAllMng, sizeof( RWALLMD )/4 );	/* Reset Memory */
}


/****************************************************************************************************/
/*																									*/
/*		EEPROM Download Manager																		*/
/*																									*/
/****************************************************************************************************/
void	KpxDownloadEeprom( RWALLMD *pRwAllMng )
{
/*--------------------------------------------------------------------------------------------------*/
/*		Check Read/Write Request																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( pRwAllMng->RwReq == RWALL_REQ_READ )
		{
			LpxReadAllEepromData( pRwAllMng );
		}
		else if( pRwAllMng->RwReq == RWALL_REQ_WRITE )
		{
			LpxWriteAllEepromData( pRwAllMng );
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Read All EEPROM Data																		*/
/*																									*/
/****************************************************************************************************/
void	LpxReadAllEepromData( RWALLMD *pRwAllMng )
{
LONG	i;
ULONG	ReadAdr;
ULONG	EepNo;
/*--------------------------------------------------------------------------------------------------*/
/*		Read All EEPROM Procedure																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pRwAllMng->RwStep )
		{
		/*------------------------------------------------------------------------------------------*/
			case 0:	/* Initial Step																	*/
		/*------------------------------------------------------------------------------------------*/
					pRwAllMng->RwCnt  = 0;
					pRwAllMng->RwStep = 1;
					break;
		/*------------------------------------------------------------------------------------------*/
			case 1:	/* Read All EEPROM																*/
		/*------------------------------------------------------------------------------------------*/
					KpiRstLongTimer( &pRwAllMng->ChkTimer );	/* Reset Timer						*/
					LpxGetEepromSemaphore( WAIT );				/* Get EepromSemaphore				*/
				/*----------------------------------------------------------------------------------*/
					for(  ; pRwAllMng->RwCnt < EEPSIZE_TOTAL; pRwAllMng->RwCnt++ )
					{
						i = pRwAllMng->RwCnt;
//						pRwAllMng->RwBuf[i] = KpxReadEepromWord( i<<ADDRSFT, WAIT );				  /* <S00F> *//* <S097> */

						ReadAdr = GET_EEP_PHYSICAL_ADDRESS(i<<ADDRSFT);								  /* <S0E4> */
						EepNo = GET_EEP_MEMORY_NUMBER(i<<ADDRSFT);									  /* <S0E4> */
//						pRwAllMng->RwBuf[i] = KpxReadEepromWord( ReadAdr, EepNo, WAIT );	 		  /* <S0E4> */

//						EepI2Cdrv_ReadEepromWord( i<<ADDRSFT, &pRwAllMng->RwBuf[i] );				  /* <S0E4> */
						EepI2Cdrv_ReadEepromWord( ReadAdr, &pRwAllMng->RwBuf[i], EepNo );			  /* <S0E4> */
						if( KpiGetLongTimer( &pRwAllMng->ChkTimer ) > 100 ){ break;}
					}
				/*----------------------------------------------------------------------------------*/
					LpxFreeEepromSemaphore( );					/* Free EepromSemaphore				*/
					if( pRwAllMng->RwCnt >= EEPSIZE_TOTAL )
					{
						pRwAllMng->RwStep = 2;
					}
					break;
		/*------------------------------------------------------------------------------------------*/
			case 2:	/* Write End Procedure															*/
		/*------------------------------------------------------------------------------------------*/
					pRwAllMng->RwReq  = RWALL_REQ_READOK;
					pRwAllMng->RwStep = 0;
					break;
		/*------------------------------------------------------------------------------------------*/
			default: /* Not Used																	*/
		/*------------------------------------------------------------------------------------------*/
					break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Write All EEPROM Data																		*/
/*																									*/
/****************************************************************************************************/
void	LpxWriteAllEepromData( RWALLMD *pRwAllMng )
{
LONG	i;
ULONG	WriteAdr;
ULONG	EepNo;
/*--------------------------------------------------------------------------------------------------*/
/*		Write All EEPROM Procedure																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pRwAllMng->RwStep )
		{
		/*------------------------------------------------------------------------------------------*/
			case 0:	/* Initial Step																	*/
		/*------------------------------------------------------------------------------------------*/
					pRwAllMng->RwCnt  = 0;						/* Reset R/W Count					*/
					pRwAllMng->RwStep = 1;						/* Goto Write All Step				*/
					pRwAllMng->EepWrDisable = TRUE;				/* EEPROM Write Disable				*/
					break;
		/*------------------------------------------------------------------------------------------*/
			case 1:	/* Write All EEPROM																*/
		/*------------------------------------------------------------------------------------------*/
					KpiRstLongTimer( &pRwAllMng->ChkTimer );	/* Reset Timer						*/
					LpxGetEepromSemaphore( WAIT );				/* Get EepromSemaphore				*/
//-					KpxWriteEepromEnable( );					/* Write EEPROM Enable				*//* <S04E> :Rename */
				/*----------------------------------------------------------------------------------*/
					for(  ; pRwAllMng->RwCnt < EEPSIZE_TOTAL; pRwAllMng->RwCnt++ )
					{
						i = pRwAllMng->RwCnt;
//						KpxWriteEepromWord( i<<ADDRSFT, pRwAllMng->RwBuf[i], WAIT );				 /* <S00F> *//* <S097> */

						WriteAdr = GET_EEP_PHYSICAL_ADDRESS(i<<ADDRSFT);							  /* <S0E4> */
						EepNo = GET_EEP_MEMORY_NUMBER(i<<ADDRSFT);									  /* <S0E4> */
//						KpxWriteEepromWord( WriteAdr, pRwAllMng->RwBuf[i], EepNo, WAIT );			  /* <S0E4> */

//						EepI2Cdrv_WriteEepromWord( i<<ADDRSFT, pRwAllMng->RwBuf[i] );				  /* <S0E4> */
						KpxWriteEepromEnable(EepNo);	/* Write EEPROM1 or 2 Enable	*//* <S1AC> */
						EepI2Cdrv_WriteEepromWord( WriteAdr, pRwAllMng->RwBuf[i], EepNo );			  /* <S0E4> */
						KpxWriteEepromDisable(EepNo);	/* Write EEPROM1 or 2 Disable	*//* <S1AC> */
						if( KpiGetLongTimer( &pRwAllMng->ChkTimer ) > 100 ){ break;} 
					}
				/*----------------------------------------------------------------------------------*/
//-					KpxWriteEepromDisable( );					/* Write EEPROM Disable				*//* <S04E> :Rename */
					LpxFreeEepromSemaphore( );					/* Free EepromSemaphore				*/
					if( pRwAllMng->RwCnt >= EEPSIZE_TOTAL )
					{
						pRwAllMng->RwStep = 2;					/* Goto Write End Step				*/
					}
					break;
		/*------------------------------------------------------------------------------------------*/
			case 2:	/* Write End Procedure															*/
		/*------------------------------------------------------------------------------------------*/
					pRwAllMng->RwReq  = RWALL_REQ_WRITEOK;
					pRwAllMng->RwStep = 0;
					break;
		/*------------------------------------------------------------------------------------------*/
			default: /* Not Used																	*/
		/*------------------------------------------------------------------------------------------*/
					break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		Request Read All Eeprom Data																*/
/*																									*/
/****************************************************************************************************/
LONG	KriRequestReadAllEeprom( RWALLMD *pRwAllMng, USHORT *ReadBuf )
{
		if( ReadBuf == NULL )
		{
			return( NG );
		}
		else if( LPX_CHK_RWALL_NOREQ( pRwAllMng->RwReq ) )
		{
			pRwAllMng->RwBuf = ReadBuf;
			pRwAllMng->RwReq = RWALL_REQ_READ;
			return( OK );
		}
		else
		{
			return( NG );
		}
}


/****************************************************************************************************/
/*																									*/
/*		Check Done Read All Eeprom Data																*/
/*																									*/
/****************************************************************************************************/
LONG	KriChkDoneReadAllEeprom( RWALLMD *pRwAllMng )
{
		if( pRwAllMng->RwReq == RWALL_REQ_READOK ){ return( TRUE );}
		if( pRwAllMng->RwReq == RWALL_REQ_READNG ){ return( TRUE );}
		return( FALSE );
}


/****************************************************************************************************/
/*																									*/
/*		Request Write All Eeprom Data																*/
/*																									*/
/****************************************************************************************************/
LONG	KriRequestWriteAllEeprom( RWALLMD *pRwAllMng, USHORT *WriteBuf )
{
		if( WriteBuf == NULL )
		{
			return( NG );
		}
		else if( LPX_CHK_RWALL_NOREQ( pRwAllMng->RwReq ) )
		{
			pRwAllMng->RwBuf = WriteBuf;
			pRwAllMng->RwReq = RWALL_REQ_WRITE;
			return( OK );
		}
		else
		{
			return( NG );
		}
}


/****************************************************************************************************/
/*																									*/
/*		Check Done Write All Eeprom Data															*/
/*																									*/
/****************************************************************************************************/
LONG	KriChkDoneWriteAllEeprom( RWALLMD *pRwAllMng )
{
		if( pRwAllMng->RwReq == RWALL_REQ_WRITEOK ){ return( TRUE );}
		if( pRwAllMng->RwReq == RWALL_REQ_WRITENG ){ return( TRUE );}
		return( FALSE );
}


/***************************************** end of file **********************************************/
