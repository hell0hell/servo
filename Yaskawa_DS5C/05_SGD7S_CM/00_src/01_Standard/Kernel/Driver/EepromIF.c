/****************************************************************************************************/
/*
 * Description	: EEPROM Access Manager
 * ----------------------------------------------------------------------------
 * Author        : Yaskawa Electric Corp.,
 * Copyright (c) 2013 All Rights Reserved
 * 
 * Project       : 
 * Functions	  :
 * EepdevInitialize()				: Initialize the Eeprom device
 * EepdevWriteValues()				: Write values to Eeprom
 * EepdevReadValues()				: Read values from Eeprom
 * EepdevCheckSumValue()			: Check whether the stored values are valid
 * EepdevPutQueue()				: Put a write value into a queue
 * EepdevWriteQuePackets()			: Write the queue values to Eeprom
 * EepdevCreateQueue()				: Write the queue values to Eeprom
 * ----------------------------------------------------------------------------
 * Changes		:
 * Name   Date          Description
 * ----------------------------------------------------------------------------
 *        2013.7.30     created
 *
 */
/****************************************************************************************************/
#include "KnlApi.h"
#include "EepromIF.h"

#ifdef _VC_DEBUG_ /* 2012.08.22 Y.Oka Make Mercury Base Software */
#include <stdio.h>
#endif /* _VC_DEBUG_ */ /* 2012.08.22 Y.Oka Make Mercury Base Software */


/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
#define		CPU_WRKBUFSADR		(void *)0xE8000000		/* TODO: 後で消す							*/
/* forward declarations of the local function */
//-void	KpxWriteEepromEnable( void );					/* EEPROM Write Enable						*//* <*S04E> :Rename */
//-void	KpxWriteEepromDisable( void );					/* EEPROM Write Disable						*//* <*S04E> :Rename */
void	KpxWriteEepromEnable( ULONG eeprom_no );		/* EEPROM Write Enable						*//* <*S04E> :Rename */
void	KpxWriteEepromDisable( ULONG eeprom_no );		/* EEPROM Write Disable						*//* <*S04E> :Rename */

/****************************************************************************************************/
/*																									*/
/*		EEPROM-I2C: Write EEPROM Enable																*/
/*																									*/
/****************************************************************************************************//* <*S04E> :Rename */
void	KpxWriteEepromEnable( ULONG eeprom_no )
{
//-	CFG_EEPROM_WRITE_ENABLE( );			/* EEPROM Write Enable						*/
	/* EEPROMの２軸対応 */ /* <S1AC> */
	if(eeprom_no == 0)
	{
		CFG_EEPROM1_WRITE_ENABLE();	/* EEPROM1 Write Enable						*/
	}else
	{
		CFG_EEPROM2_WRITE_ENABLE();	/* EEPROM2 Write Enable						*/
	}
	return;
}

/****************************************************************************************************/
/*																									*/
/*		EEPROM-I2C: Write EEPROM Disable															*/
/*																									*/
/****************************************************************************************************//* <*S04E> :Rename */
void	KpxWriteEepromDisable(  ULONG eeprom_no )
{
//-		CFG_EEPROM_WRITE_DISABLE( );	/* EEPROM Write Disable		*/
	/* EEPROMの２軸対応 */ /* <S1AC> */
	if(eeprom_no == 0)
	{
		CFG_EEPROM1_WRITE_DISABLE();	/* EEPROM1 Write Disable	*/
	}else
	{
		CFG_EEPROM2_WRITE_DISABLE();	/* EEPROM2 Write Disable	*/
	}
	return;
}

/****************************************************************************************************/
/*																									*/
/*		Debug1																						*/
/*																									*/
/****************************************************************************************************/
void	LpxTestEeprom( void )
{
int i;
USHORT	*bufx = (USHORT*)CPU_WRKBUFSADR;
//	KpxWriteEepromEnable( );/* <*S04E> :Rename *//* <S0E4> */
//	for( i=0; i< EEPSIZE_TOTAL; i++ ) { KpxWriteEepromWord( i, (USHORT)0, WAIT ); }  /* <S003> *//* <S097> */
//	for( i=0; i< EEPSIZE_TOTAL; i++ ) { *bufx++ = KpxReadEepromWord( i, WAIT ); }
//	for( i=0; i< EEPSIZE_TOTAL*2; i+=2 ) { EepI2Cdrv_WriteEepromWord( i, (USHORT)0 ); }
//	for( i=0; i< EEPSIZE_TOTAL*2; i+=2 ) { EepI2Cdrv_ReadEepromWord( i, bufx++ ); }
//	KpxWriteEepromDisable( );/* <*S04E> :Rename *//* <S0E4> */
}

/****************************************************************************************************/
/*																									*/
/*		Debug2																						*/
/*																									*/
/****************************************************************************************************/
#if 0
void	LpxTestEeprom2( void )
{
int i;
USHORT	*bufx = (USHORT*)CPU_WRKBUFSADR;
	GetEepromSemaphore( WAIT );			/* Get  EEPROM Semaphore						*/
	WriteEepromEnable( );
	for( i=0; i< EEPSIZE_TOTAL; i++ ) 
	{ 
		do { ; } while ( KpxWriteEepromWordNoWait( i, (USHORT *)&i ) != 0 );
	}
	for( i=0; i< EEPSIZE_TOTAL; i++, bufx++ )
	{ 
		do { ; } while ( KpxReadEepromWordNoWait( i, bufx ) != 0 );
	}
	WriteEepromDisable( );
	FreeEepromSemaphore( );				/* Free EEPROM Semaphore						*/
}
#endif
/****************************************************************************************************/
/*
 * Description: Initialize the Eeprom device manager
 *
 * Parameters:
 * none
 * Return:
 * Error
 *
 */
/****************************************************************************************************/
LONG EepdevInitialize( void *SdaAsicPtr, void *CoaAsicPtr, RWALLMD *pRwAllMng, RWPDLMD *pRwPdlMng )
{
	//LONG	rslt;

	//hEepDev->pvI2CDevice = SdaAsicPtr;
	//hEepDev->pvI2CCoADevice = CoaAsicPtr;
	//rslt = SPIEepDrvInitialize( &hEepDev->pvSPIDevice );
	//return rslt;
	if( 0 ){ LpxTestEeprom( ); }							/* TODO:Debug後に削除					*/
/*--------------------------------------------------------------------------------------------------*/
	KpxInitDownloadEeprom( pRwAllMng );						/* <S00C> 								*/
	KpxInitLcdPdlEeprom( pRwPdlMng );						/* <S031> 								*/
/*--------------------------------------------------------------------------------------------------*/
	return TRUE;
}

/***************************************************************************
Description: Read the stored value from the EEPROM

Parameters:
	hEepDev		: Pointer to EEPROM device handle
	devID		: EEPROM Device ID(0: SPI(IF), 1: I2C(SDA)
	address		: Eeprom Address in bytes
	rdValue		: Pointer to the buffer for reading
	rdWordSz	: reading size in words
Return:
	Error
****************************************************************************/
LONG EepdevReadValues(UINT devId, USHORT address, USHORT *rdValue, USHORT rdWordSz)
{
	USHORT	eepAddress;
	LONG	rdCnt, byteSz;
	LONG	rslt = I2C_DRV_SUCCESS;
	ULONG	eepNumber;						/* <S0E4> */

/* 2013.08.21 Y.Oka @@Excel定義表からも削除する */
//	if(devId == PRMDEF_EEPROM_IF)
//	{	/* Read from IF SPI EEPROM device */
//		eepAddress = address;
//	}
	/* EEPROMアドレス & EEPROM番号設定 *//* <S0E4> */
	eepAddress = GET_EEP_PHYSICAL_ADDRESS( address );
	eepNumber = GET_EEP_MEMORY_NUMBER( address );


	for(rdCnt = 0; rdCnt < rdWordSz; rdCnt++)
	{
//		rslt = EepI2Cdrv_ReadEepromWord((ASIC_HW_IF_STR*)hEepDev->pvI2CDevice, axis, eepAddress, rdValue);
//		if(rslt != 0){	break;}
//		*rdValue = KpxReadEepromWord( eepAddress, WAIT ); /* <S097> */
		rslt = EepI2Cdrv_ReadEepromWord( eepAddress, rdValue, eepNumber );
		if(rslt != I2C_DRV_SUCCESS){ break; }
		eepAddress += 2;
		rdValue++;
	}

	return rslt;
}



/***************************************************************************
Description: Write the value to the EEPROM

Parameters:
	devID		: EEPROM Device ID(0: SPI(IF), 1: I2C(SDA)
	address		: Eeprom Address in bytes
	wrValue		: Pointer to the buffer for writing
	wrWordSz	: writing size in words
Return:
	Error
Remarks:
    page size
****************************************************************************/
LONG EepdevWriteNoSumValues(UINT devId, USHORT address, USHORT *wrValue, USHORT wrWordSz)
{
	USHORT	eepAddress;
	LONG	wrCnt, byteSz;
	LONG	rslt = 0;
	ULONG	eepNumber;						/* <S0E4> */

/* 2013.08.21 Y.Oka @@Excel定義表からも削除する */
//	if(devId == PRMDEF_EEPROM_IF)
//	{	/* Read from IF SPI EEPROM device */
//		eepAddress = address;
//	}
	/* EEPROMアドレス & EEPROM番号設定 *//* <S0E4> */
	eepAddress = GET_EEP_PHYSICAL_ADDRESS( address );
	eepNumber = GET_EEP_MEMORY_NUMBER( address );


//-	KpxWriteEepromEnable(  );			/* Write EEPROM Enable	*//* <*S04E> */
	KpxWriteEepromEnable(eepNumber);	/* Write EEPROM1 or 2 Enable	*//* <S1AC> */
	for(wrCnt = 0; wrCnt < wrWordSz; wrCnt++)
	{
//		KpxWriteEepromWord( eepAddress, wrValue[wrCnt], WAIT ); /* <S097> */
		rslt = EepI2Cdrv_WriteEepromWord( eepAddress, wrValue[wrCnt], eepNumber );
		if(rslt != 0){	break;}
		eepAddress += 2;
	}
//-	KpxWriteEepromDisable( );			/* Write EEPROM Disable	*//* <*S04E> */
	KpxWriteEepromDisable(eepNumber);	/* Write EEPROM1 or 2 Disable	*//* <S1AC> */

	return rslt;
}



/***************************************************************************
Description: Write the value to the EEPROM

Parameters:
	USHORT address	: Eeprom Address in bytes
	USHORT *wrValue	: Pointer to the buffer for writing
	USHORT wrWordSz	: writing size in bytes
Return:
	LONG			: 0:書き込み成功, 1:変更なし, 0以下:Error
****************************************************************************/
LONG EepdevWriteParameters(UINT devId, USHORT address, USHORT *wrValue, USHORT wrWordSz)
{
	LONG	rslt, err;
	USHORT	cnt;
	USHORT	checksum;
	USHORT	wr_addr;
	USHORT	rdValue;

	rslt = 0;
	checksum = 0;

	/* Write new values */
	for(cnt = 0; cnt < wrWordSz; cnt++)
	{
		wr_addr = address + (cnt * 2);
		/* Read a stored value */
		err = EepdevReadValues(devId, wr_addr, &rdValue, 1);
		if(err != 0)
		{// error !!
			rslt = err;
			checksum = 0;
			break;
		}

		/* Verify and write a new value */
		if(rdValue != wrValue[cnt])
		{
			err = EepdevWriteNoSumValues(devId, wr_addr, &wrValue[cnt], 1);
			if(err != 0)
			{// error !!
				rslt = err;
				checksum = 0;
				break;
			}
		}

		/* add a new sum value */
		checksum += (USHORT)(rdValue - wrValue[cnt]);
	}

	/* Write a new checksum value *//* @@SGD7W未対応 */
	if(checksum != 0)
	{
//		wr_addr = (address & EEP_CHECKSUM_ADDRESS_MASK); /* <S003> */
		if( (address & EEP_CHECKSUM_ADDRESS_MASK) == EEP_CMNPRM_ADDRESS )
		{ /* Axis Common Parameter */
			wr_addr = EEP_CMNPRM_CHKSUM_ADR;
		}
		else
		{
//			wr_addr = EEP_AXPRM_CHKSUM_ADR; /* <S0E4> */
			wr_addr = (address & EEP_MAP_AXIS_MASK) + EEP_AXPRM_CHKSUM_ADR; /* <S0E4> */
		}

		if(devId != PRMDEF_EEPROM_IF)
		{	wr_addr += EEP_MAP_HEADER_INFO_LENGTH;}

		rslt = EepdevReadValues(devId, wr_addr, &rdValue, 1);
		if(rslt == 0)
		{
			checksum += rdValue;
			rslt = EepdevWriteNoSumValues(devId, wr_addr, &checksum, 1);
		}
	}
	else /* <S097> */
	{
		rslt = 1;
	}

	return rslt;
}


/***************************************************************************
Description: Initialize the Eeprom queue

Parameters:
	*hEepQue	: Pointor to the Que handle
	*QueBuf		: Pointor to the pool buffer
	MaxQueSize	: Number of max. queues
Return:
	none
****************************************************************************/
void EepdevCreateQueue(EEP_QUE_HANDLE *hEepQue,
						EEP_QUE_BUFFER *QueBuf, UINT MaxQueSize)
{
	hEepQue->maxQueSize = MaxQueSize;
	hEepQue->QueIdx     = 0;
	hEepQue->nextQueIdx = 0;
	hEepQue->QueBuffer  = QueBuf;
}


/***************************************************************************
Description: Send a que packet

Parameters:
	*hEepQue	: Pointor to the Que handle
	address		: Eeprom Address in bytes
	wrValue		: Pointer to the buffer for writing
	wrWordSz	: writing size in words
Return:
	none
****************************************************************************/
LONG EepdevPutQueue(EEP_QUE_HANDLE *hEepQue, UINT devID,
					USHORT address, ULONG wrValue, USHORT wrWordSz)
{
	UINT	queCnt;
	UINT	queIdx;

	queCnt = (hEepQue->nextQueIdx + 1) - hEepQue->QueIdx;
	if(queCnt > hEepQue->maxQueSize)
	{
		return -1;
	}

	queIdx = hEepQue->nextQueIdx;
	hEepQue->QueBuffer[queIdx].devID    = devID;
	hEepQue->QueBuffer[queIdx].address  = address;
	hEepQue->QueBuffer[queIdx].wrWordSz = wrWordSz;
	hEepQue->QueBuffer[queIdx].wrValue  = wrValue;

	++ queIdx;
	hEepQue->nextQueIdx = (queIdx >= hEepQue->maxQueSize) ? 0 : queIdx;
	return 0;
}

/***************************************************************************
Description: Flush the queue value

Parameters:
	*hEepQue	: Pointor to the Que handle
	maxWriteNum	: Number of max. queues
Return:
	none
****************************************************************************/
void EepdevWriteQuedValues(EEP_QUE_HANDLE *hEepQue, UINT maxWriteNum)
{
	UINT	queIdx;
	UINT	writeCnt;
	EEP_QUE_BUFFER	QueBuf;

	writeCnt = 0;
	queIdx = hEepQue->QueIdx;

	while ((hEepQue->nextQueIdx != queIdx) && (writeCnt < maxWriteNum))
	{
		QueBuf = hEepQue->QueBuffer[queIdx];

#ifdef __BIG_ENDIAN__
		if(QueBuf.wrWordSz == 1)
		{
			QueBuf.wrValue = QueBuf.wrValue << 16;
		}
#endif
		EepdevWriteNoSumValues(QueBuf.devID, QueBuf.address,
							   (USHORT*)&(QueBuf.wrValue), QueBuf.wrWordSz);

		queIdx++;
		if(queIdx >= hEepQue->maxQueSize)
		{
			queIdx = 0;
		}

		hEepQue->QueIdx = queIdx;
		writeCnt++;
	}
}



#ifdef _VC_DEBUG_ /* 2012.08.22 Y.Oka Make Mercury Base Software */
/***************************************************************************
Description: Read all parameters from Parameter file for VC_DEBUG

Parameters:
	USHORT *RdBuffer	: Pointer to the buffer for reading
	USHORT rdSize		: reading size in bytes
Return:
	Error
****************************************************************************/
LONG DbgEepAllRead( void *RdBuffer, USHORT rdSize )
{
	FILE	*fp;
	errno_t	error;

	/* バイナリ書き込み読み込みモードでファイルをオープン */
	if( error = fopen_s(&fp, "Prm.bin", "rb") != 0 ){
		return -1;
	}

	/* 書き込んだデータを読み込んでみる */
	fread( RdBuffer, sizeof(UCHAR), rdSize, fp);

	/* File Close */
	fclose( fp );

	return 0;
}
#endif /* _VC_DEBUG_ */ /* 2012.08.22 Y.Oka Make Mercury Base Software */

#if defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
/***************************************************************************
Description: Read all parameters from Parameter file for VC_DEBUG

Parameters:
	USHORT *RdBuffer	: Pointer to the buffer for reading
	USHORT rdSize		: reading size in bytes
Return:
	Error
****************************************************************************/
LONG DbgEepReadValues(UCHAR *EepBuf, USHORT address, UCHAR *rdValue, USHORT rdWordSz)
{
	USHORT	eepAddress;
	LONG	rdCnt;

	if( address > DEBUG_EEPROM_SIZE )
	{
		return -1;
	}

	eepAddress = address + DEBUG_HEADER_OFST;
	for(rdCnt = 0; rdCnt < rdWordSz; rdCnt++)
	{
		*rdValue = EepBuf[eepAddress];
		*(rdValue+1) = EepBuf[eepAddress+1];
		eepAddress += 2;
		rdValue += 2;
	}
	return 0;
}
#endif /* #if defined(_VC_DEBUG_) */ /* 2012.08.22 Y.Oka Make Mercury Base Software */



/* <S18F> */
/***************************************************************************
Description: Read the stored value from the EEPROM

Parameters:
	address		: Eeprom Address in bytes
	rdValue		: Pointer to the buffer for reading
	rdWordSz	: reading size in words
Return:
	Error
****************************************************************************/
LONG KriReadEepromData( USHORT address, USHORT *rdValue, USHORT rdWordSz )
{
	USHORT	eepAddress;
	ULONG	eepNumber;
	LONG	rdCnt;
	LONG	byteSz;
	LONG	rslt = I2C_DRV_SUCCESS;

	/* EEPROMアドレス & EEPROM番号設定 */
	eepAddress = GET_EEP_PHYSICAL_ADDRESS( address );
	eepNumber  = GET_EEP_MEMORY_NUMBER( address );

	for( rdCnt = 0 ; rdCnt < rdWordSz ; rdCnt++ )
	{
		rslt = EepI2Cdrv_ReadEepromWord( eepAddress, rdValue, eepNumber );
		if( rslt != I2C_DRV_SUCCESS )
		{
			break;
		}
		eepAddress += 2;
		rdValue++;
	}

	return( rslt );
}



/* <S18F> */
/***************************************************************************
Description: Write the value to the EEPROM

Parameters:
	address		: Eeprom Address in bytes
	wrValue		: Pointer to the buffer for writing
	wrWordSz	: writing size in words
Return:
	Error
****************************************************************************/
LONG KriWriteEepromData( USHORT address, USHORT *wrValue, USHORT wrWordSz )
{
	USHORT	eepAddress;
	ULONG	eepNumber;
	LONG	wrCnt;
	LONG	byteSz;
	LONG	rslt = I2C_DRV_SUCCESS;

	/* EEPROMアドレス & EEPROM番号設定 */
	eepAddress = GET_EEP_PHYSICAL_ADDRESS( address );
	eepNumber  = GET_EEP_MEMORY_NUMBER( address );

//-	KpxWriteEepromEnable( );			/* Write EEPROM Enable	*/
	KpxWriteEepromEnable(eepNumber);	/* Write EEPROM1 or 2 Enable	*//* <S1AC> */
	for( wrCnt = 0 ; wrCnt < wrWordSz ; wrCnt++ )
	{
		rslt = EepI2Cdrv_WriteEepromWord( eepAddress, wrValue[wrCnt], eepNumber );
		if( rslt != I2C_DRV_SUCCESS )
		{
			break;
		}
		eepAddress += 2;
	}
//-	KpxWriteEepromDisable( );			/* Write EEPROM Disable	*/
	KpxWriteEepromDisable(eepNumber);	/* Write EEPROM1 or 2 Disable	*//* <S1AC> */

	return( rslt );
}



/***************************************** end of file **********************************************/
