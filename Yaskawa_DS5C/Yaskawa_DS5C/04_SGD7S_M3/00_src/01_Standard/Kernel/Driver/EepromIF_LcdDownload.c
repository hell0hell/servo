/****************************************************************************************************/
/*																									*/
/*																									*/
/*		EepromIF_LcdDownload.c : Lcd Operator Parameter Download 									*/
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
/*		Rev.1.00 : 2013.10.24  K.Ozaki		<S031>													*/
/*		Rev.2.00 : 2013.12.18  Y.Oka		<S097>													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "KnlApi.h"
#include "LcdPdl.h"

#define EEPADR_BYTE 			/* EEPROMデバイストライバのアドレスがBYTEの場合(Mercury) */
#ifdef EEPADR_BYTE 
	#define ADDRSFT (1)
#else
	#define ADDRSFT (0)
#endif

/****************************************************************************************************/
/*																									*/
/*		Macro Definition																			*/
/*																									*/
/****************************************************************************************************/
#define EEPADR_AXCMN_CHKSUM  (0x100<<ADDRSFT)			/* word address 							*/
#define EEPADR_AXPRM_CHKSUM  (0x180<<ADDRSFT)			/* word address 							*/
#define LpxGetEepromSemaphore( WAIT ) { ;}				/* TODO: Get EepromSemaphore				*/
#define LpxFreeEepromSemaphore( ) { ;}					/* TODO: Free EepromSemaphore				*/
#define LPX_CHK_RWPDL_NOREQ( RwReq )	( (RwReq == 0) || ((RwReq & 0x00FF) != 0) )

/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		APIs																						*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitDownloadEeprom( RWALLMD *pRwAllMng );	/* Initialize Download EEPROM				*//* <S0E4> */
LONG	KpxLcdPdlEeprom( RWPDLMD *pRwPdlMng );			/* Download EEPROM							*/
LONG	KriRequestReadLcdPdlEeprom( RWPDLMD *pRwPdlMng, USHORT *ReadBuf, void *PdlTbl, LONG TblEntNum );
LONG	KriRequestWriteLcdPdlEeprom( RWPDLMD *pRwPdlMng, USHORT *WriteBuf, void *PdlTbl, LONG TblEntNum );
LONG	KriChkDoneReadLcdPdlEeprom( RWPDLMD *pRwPdlMng );
LONG	KriChkDoneWriteLcdPdlEeprom( RWPDLMD *pRwPdlMng );

/*--------------------------------------------------------------------------------------------------*/
/*		Local Function 																				*/
/*--------------------------------------------------------------------------------------------------*/
//static void	LpxReadAllEepromData( RWALLMD *pRwAllMng );	/* Read  All Eeprom Data				*//* <S0E4> */
//static void	LpxWriteAllEepromData( RWALLMD *pRwAllMng );/* Write All Eeprom Data				*//* <S0E4> */
static LONG	LpxReadLcdPdlData( RWPDLMD *pRwPdlMng );	/* Read  All Eeprom Data					*/
static LONG	LpxWriteLcdPdlData( RWPDLMD *pRwPdlMng );	/* Write All Eeprom Data					*/



/****************************************************************************************************/
/*																									*/
/*		Initiazlie Download EEPROM																	*/
/*																									*/
/****************************************************************************************************/
void	KpxInitLcdPdlEeprom( RWPDLMD *pRwPdlMng )
{
	MlibResetLongMemory( pRwPdlMng, sizeof( RWPDLMD )/4 );	/* Reset Memory */
}


/****************************************************************************************************/
/*																									*/
/*		EEPROM Download Manager																		*/
/*																									*/
/****************************************************************************************************/
LONG	KpxLcdPdlEeprom( RWPDLMD *pRwPdlMng )
{
LONG	ret = I2C_DRV_SUCCESS;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Read/Write Request																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( pRwPdlMng->RwReq == RWPDL_REQ_READ )
		{
			ret = LpxReadLcdPdlData( pRwPdlMng );
		}
		else if( pRwPdlMng->RwReq == RWPDL_REQ_WRITE )
		{
			ret = LpxWriteLcdPdlData( pRwPdlMng );
		}
/*--------------------------------------------------------------------------------------------------*/
		return	ret;
}


/****************************************************************************************************/
/*																									*/
/*		Read All EEPROM Data																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxReadLcdPdlData( RWPDLMD *pRwPdlMng )
{
LONG	i;
LONG	ret;
USHORT 	EepAdr;
ULONG 	EepNo;
const PDLTBL *pPdlTbl = (const PDLTBL *)pRwPdlMng->PdlTbl;
ret = I2C_DRV_SUCCESS;

/*--------------------------------------------------------------------------------------------------*/
/*		Read All EEPROM Procedure																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pRwPdlMng->RwStep )
		{
		/*------------------------------------------------------------------------------------------*/
			case 0:	/* Initial Step																	*/
		/*------------------------------------------------------------------------------------------*/
					pRwPdlMng->RwBuf[1] = pPdlTbl[1].PnNo;								/* Total Number */
					pRwPdlMng->RwBuf[2] = pPdlTbl[2].PnNo;								/* Header Size */
					pRwPdlMng->ChkSumX = pRwPdlMng->RwBuf[1] + pRwPdlMng->RwBuf[2];		/* Set CheckSum */
					pRwPdlMng->RwCnt  = 3;
					pRwPdlMng->RwStep = 1;
					break;
		/*------------------------------------------------------------------------------------------*/
			case 1:	/* Read Download Parameter from EEPROM											*/
		/*------------------------------------------------------------------------------------------*/
					KpiRstLongTimer( &pRwPdlMng->ChkTimer );	/* Reset Timer						*/
					LpxGetEepromSemaphore( WAIT );				/* Get EepromSemaphore				*/
				/*----------------------------------------------------------------------------------*/
					for(  ; pRwPdlMng->RwCnt < pRwPdlMng->TblEntNum; (pRwPdlMng->RwCnt)++ )
					{
						if( KpiGetLongTimer( &pRwPdlMng->ChkTimer ) > 100 ){ break;}	/* 			*/
				/*----------------------------------------------------------------------------------*/
						i = pRwPdlMng->RwCnt;
						EepAdr = GET_EEP_PHYSICAL_ADDRESS(pPdlTbl[i].EepAddr);						  /* <S0E4> */
						EepNo  = GET_EEP_MEMORY_NUMBER(pPdlTbl[i].EepAddr);							  /* <S0E4> */
				/*----------------------------------------------------------------------------------*/
						if ( EepAdr < EEPSIZE_TOTAL )
						{
//							ret = EepI2Cdrv_ReadEepromWord( EepAdr<<ADDRSFT, &pRwPdlMng->RwBuf[i] );   /* <S0E4> */
							ret = EepI2Cdrv_ReadEepromWord( EepAdr<<ADDRSFT, &pRwPdlMng->RwBuf[i], EepNo );
						}
						else
						{
							pRwPdlMng->RwBuf[i] = 0;
						}
				/*----------------------------------------------------------------------------------*/
						pRwPdlMng->ChkSumX += pRwPdlMng->RwBuf[i];
					}
				/*----------------------------------------------------------------------------------*/
					LpxFreeEepromSemaphore( );					/* Free EepromSemaphore				*/
					if( pRwPdlMng->RwCnt >= pRwPdlMng->TblEntNum )
					{
						pRwPdlMng->RwBuf[0] = -(pRwPdlMng->ChkSumX);
						pRwPdlMng->RwStep = 2;
					}
					break;
		/*------------------------------------------------------------------------------------------*/
			case 2:	/* Write End Procedure															*/
		/*------------------------------------------------------------------------------------------*/
					pRwPdlMng->RwReq  = RWPDL_REQ_READOK;
					pRwPdlMng->RwStep = 0;
					break;
		/*------------------------------------------------------------------------------------------*/
			default: /* Not Used																	*/
		/*------------------------------------------------------------------------------------------*/
					break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return	ret;
}


/****************************************************************************************************/
/*																									*/
/*		Write All EEPROM Data																		*/
/*																									*/
/****************************************************************************************************/
LONG	LpxWriteLcdPdlData( RWPDLMD *pRwPdlMng )
{
LONG		i;
LONG		ret;
USHORT		EepAdr;
ULONG		EepNo;
USHORT		SumX;
USHORT		wkx;
const PDLTBL *pPdlTbl = (const PDLTBL *)pRwPdlMng->PdlTbl;
ret = I2C_DRV_SUCCESS;

/*--------------------------------------------------------------------------------------------------*/
/*		Write All EEPROM Procedure																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( pRwPdlMng->RwStep )
		{
		/*------------------------------------------------------------------------------------------*/
			case 0:	/* Initial Step																	*/
		/*------------------------------------------------------------------------------------------*/
		/* 			Calculate checksum 																*/
		/*------------------------------------------------------------------------------------------*/
					for( i = 0, SumX=0; i < pRwPdlMng->RwBuf[1]; i++ ) 	
					{
						SumX += pRwPdlMng->RwBuf[i];
					}
					if (  SumX != 0 )
					{
						pRwPdlMng->RwReq  = RWPDL_REQ_WRITENG;
						pRwPdlMng->RwStep = 3;
						break;
					}
		/*------------------------------------------------------------------------------------------*/
					LpxGetEepromSemaphore( WAIT );				/* Get EepromSemaphore				*/
					ret = EepI2Cdrv_ReadEepromWord( EEPADR_AXCMN_CHKSUM, &pRwPdlMng->ChkSumX, 0 );	  /* <S0E4> */
					ret = EepI2Cdrv_ReadEepromWord( EEPADR_AXPRM_CHKSUM, &pRwPdlMng->ChkSumY, 0 );	  /* <S0E4> */
//<S14A>TODO: SGD7W		ret = EepI2Cdrv_ReadEepromWord( EEPADR_AXCMN_CHKSUM, &pRwPdlMng->ChkSumX, 1 );	  /* <S0E4> */
//<S14A>TODO: SGD7W		ret = EepI2Cdrv_ReadEepromWord( EEPADR_AXPRM_CHKSUM, &pRwPdlMng->ChkSumY, 1 );	  /* <S0E4> */
					LpxFreeEepromSemaphore( );					/* Free EepromSemaphore				*/
		/*------------------------------------------------------------------------------------------*/
					pRwPdlMng->TblEntNum = 						/* @@ Limit Table Entry Number 		*/
						MlibMIN( pRwPdlMng->RwBuf[1], pRwPdlMng->TblEntNum ); 
		/*------------------------------------------------------------------------------------------*/
					pRwPdlMng->RwCnt  = 0;						/* Reset R/W Count					*/
					pRwPdlMng->RwStep = 1;						/* Goto Write All Step				*/
					pRwPdlMng->EepWrDisable = TRUE;				/* EEPROM Write Disable				*/
					break;
		/*------------------------------------------------------------------------------------------*/
			case 1:	/* Write All EEPROM																*/
		/*------------------------------------------------------------------------------------------*/
					KpiRstLongTimer( &pRwPdlMng->ChkTimer );	/* Reset Timer						*/
					LpxGetEepromSemaphore( WAIT );				/* Get EepromSemaphore				*/
//-					KpxWriteEepromEnable( );					/* Write EEPROM Enable				*/
				/*----------------------------------------------------------------------------------*/
					for(  ; pRwPdlMng->RwCnt < pRwPdlMng->TblEntNum ; (pRwPdlMng->RwCnt)++ )
					{
						i = pRwPdlMng->RwCnt;
						EepAdr = pPdlTbl[i].EepAddr;
						if ( pPdlTbl[i].pPrmDef == NULL ) { continue; }
				/*----------------------------------------------------------------------------------*/
				/*		Write Download Parameter													*/
				/*----------------------------------------------------------------------------------*/
						EepAdr = GET_EEP_PHYSICAL_ADDRESS(pPdlTbl[i].EepAddr);						  /* <S0E4> */
						EepNo  = GET_EEP_MEMORY_NUMBER(pPdlTbl[i].EepAddr);							  /* <S0E4> */
//						ret = EepI2Cdrv_ReadEepromWord( EepAdr<<ADDRSFT, &wkx );
						ret = EepI2Cdrv_ReadEepromWord( EepAdr<<ADDRSFT, &wkx, EepNo );				  /* <S0E4> */
						SumX = pRwPdlMng->RwBuf[i] - wkx;
						if ( SumX )
						{
							if ( pPdlTbl[i].pPrmDef->AxisCommon )
							{
								pRwPdlMng->ChkSumX -= SumX;
							}
							else
							{
								pRwPdlMng->ChkSumY -= SumX;
							}

//							ret = EepI2Cdrv_WriteEepromWord( EepAdr<<ADDRSFT, pRwPdlMng->RwBuf[i] ); 	/* <S0E4> */
							KpxWriteEepromEnable(EepNo);	/* Write EEPROM1 or 2 Enable	*//* <S1AC> */
							ret = EepI2Cdrv_WriteEepromWord( EepAdr<<ADDRSFT, pRwPdlMng->RwBuf[i], EepNo );
							KpxWriteEepromDisable(EepNo);	/* Write EEPROM1 or 2 Disable	*//* <S1AC> */
						}
				/*----------------------------------------------------------------------------------*/
						if( KpiGetLongTimer( &pRwPdlMng->ChkTimer ) > 100 ){ break;}
					}
				/*----------------------------------------------------------------------------------*/
//-					KpxWriteEepromDisable( );					/* Write EEPROM Disable				*/
					LpxFreeEepromSemaphore( );					/* Free EepromSemaphore				*/
					if( pRwPdlMng->RwCnt >= pRwPdlMng->TblEntNum )
					{
						pRwPdlMng->RwStep = 2;					/* Goto Write End Step				*/
					}
					break;
		/*------------------------------------------------------------------------------------------*/
			case 2:	/* Write End Procedure															*/
		/*------------------------------------------------------------------------------------------*/
					LpxGetEepromSemaphore( WAIT );				/* Get EepromSemaphore				*/
//-					KpxWriteEepromEnable( );					/* Write EEPROM Enable				*/
					KpxWriteEepromEnable(0);		/* Write EEPROM1 Enable	*//* <S1AC> */
					ret = EepI2Cdrv_WriteEepromWord( EEPADR_AXCMN_CHKSUM, pRwPdlMng->ChkSumX, 0 );	  /* <S0E4> */
					ret = EepI2Cdrv_WriteEepromWord( EEPADR_AXPRM_CHKSUM, pRwPdlMng->ChkSumY, 0 );	  /* <S0E4> */
//<S14A> TODO: SGD7W	ret = EepI2Cdrv_WriteEepromWord( EEPADR_AXCMN_CHKSUM, pRwPdlMng->ChkSumX, 1 );	  /* <S0E4> */
//<S14A> TODO: SGD7W	ret = EepI2Cdrv_WriteEepromWord( EEPADR_AXPRM_CHKSUM, pRwPdlMng->ChkSumY, 1 );	  /* <S0E4> */
//-					KpxWriteEepromDisable( );					/* Write EEPROM Disable				*/
					KpxWriteEepromDisable(0);					/* Write EEPROM1 Disable *//* <S1AC> */
					LpxFreeEepromSemaphore( );					/* Free EepromSemaphore				*/
		/*------------------------------------------------------------------------------------------*/
					pRwPdlMng->RwReq  = RWPDL_REQ_WRITEOK;
					pRwPdlMng->RwStep = 0;
					break;
		/*------------------------------------------------------------------------------------------*/
			default: /* Not Used																	*/
		/*------------------------------------------------------------------------------------------*/
					break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return	ret;
}


/****************************************************************************************************/
/*																									*/
/*		Request Read All Eeprom Data																*/
/*																									*/
/****************************************************************************************************/
LONG	KriRequestReadLcdPdlEeprom( RWPDLMD *pRwPdlMng, USHORT *ReadBuf, void *PdlTbl, LONG TblEntNum )
{
		if( ReadBuf == NULL )
		{
			return( NG );
		}
		else if( LPX_CHK_RWPDL_NOREQ( pRwPdlMng->RwReq ) )
		{
			pRwPdlMng->RwBuf = ReadBuf;
			pRwPdlMng->RwReq = RWPDL_REQ_READ;
			pRwPdlMng->PdlTbl = PdlTbl;
			pRwPdlMng->TblEntNum = TblEntNum;
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
LONG	KriChkDoneReadLcdPdlEeprom( RWPDLMD *pRwPdlMng )
{
		if( pRwPdlMng->RwReq == RWPDL_REQ_READOK ){ return( TRUE );}
		if( pRwPdlMng->RwReq == RWPDL_REQ_READNG ){ return( TRUE );}
		return( FALSE );
}


/****************************************************************************************************/
/*																									*/
/*		Request Write All Eeprom Data																*/
/*																									*/
/****************************************************************************************************/
LONG	KriRequestWriteLcdPdlEeprom( RWPDLMD *pRwPdlMng, USHORT *WriteBuf, void *PdlTbl, LONG TblEntNum )
{
		if( WriteBuf == NULL )
		{
			return( NG );
		}
		else if( LPX_CHK_RWPDL_NOREQ( pRwPdlMng->RwReq ) )
		{
			pRwPdlMng->RwBuf = WriteBuf;
			pRwPdlMng->RwReq = RWPDL_REQ_WRITE;
			pRwPdlMng->PdlTbl = PdlTbl;
			pRwPdlMng->TblEntNum = TblEntNum;
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
LONG	KriChkDoneWriteLcdPdlEeprom( RWPDLMD *pRwPdlMng )
{
		if( pRwPdlMng->RwReq == RWPDL_REQ_WRITEOK ){ return( TRUE );}
		if( pRwPdlMng->RwReq == RWPDL_REQ_WRITENG ){ return( TRUE );}
		return( FALSE );
}


/***************************************** end of file **********************************************/
