/************************************************************************************************************/
/*																											*/
/*																											*/
/*		SFlash.c : SoC(JL-086)シリアルFlashメモリコントローラ関連処理										*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		シリアルFlashメモリコントローラ関連処理ソースファイル												*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.29  Y.Tanaka		初版															*/
/*																											*/
/************************************************************************************************************/
#include "Basedef.h"
#include "ARMPF_SROM.h"
#include "SFlash.h"

// Initialize
void	SerialRomInit( void )
{
    SFMEMC_Init();
}

// Write Enable
void	SerialRomWriteEnable( void )
{
	SFM_Transaction_open();
    SFM_PutByte( 0x06 );
    SFM_Transaction_close();
}

// Write Disable
void	SerialRomWriteDisable( void )
{
    SFM_Transaction_open();
    SFM_PutByte( 0x04 );
    SFM_Transaction_close();
}

// Read Identification
LONG SerialRomReadID( void )
{
    LONG id;

    SFM_Transaction_open();
    SFM_PutByte( 0x9F );
    id = SFM_GetByte();
    id <<= 8;
    id |= SFM_GetByte();
    id <<= 8;
    id |= SFM_GetByte();
    SFM_Transaction_close();
    return( id );
}

// Read Status Register
LONG SerialRomReadStatusRegister( void )
{
    LONG id;

    SFM_Transaction_open();
    SFM_PutByte( 0x05 );
    id = SFM_GetByte();
    SFM_Transaction_close();
    return( id );
}

void SerialRomWaitReady( void )
{
    SFM_Transaction_open();
    SFM_PutByte( 0x05 );
    while( SFM_GetByte() & 1 ) {};
    SFM_Transaction_close();
}

// Bulk Erase
void	SerialRomChipErase( void )
{
    SFM_Transaction_open();
    SFM_PutByte( 0x06 );					// write enable
    SFM_Transaction_open();
    SFM_PutByte( 0xC7 );					// bulk erase
	SFM_Transaction_close();
}

// Sector Erase
void	SerialRomSectorErase( ULONG lAddr )
{
	SFM_Transaction_open();
    SFM_PutByte( 0x06 );					// write enable
    SFM_Transaction_open();
    SFM_PutByte( 0xD8 );					// sector erase
    SFM_PutByte( ( lAddr >> 16 ) & 0xff );	// ...address(page)
    SFM_PutByte( ( lAddr >>  8 ) & 0xff );
    SFM_PutByte( ( lAddr       ) & 0xff );
    SFM_Transaction_close();
}

// Check Status		/* tanaka21 add 2013.07.09 */
UCHAR	SFlashChkStatus( void )
{
	UCHAR	sts;
	
	SFM_Transaction_open();
	SFM_PutByte( 0x05 );					// read status
	sts = SFM_GetByte();		/* 0/1 - FIN/BUSY */
	SFM_Transaction_close();
	
	return sts;
}

// Page Program
void  SerialRomPageProgram( ULONG lAddr, ULONG *pBuff, LONG nSize )
{
	LONG	i;
	SFM_Transaction_open();
    SFM_PutByte( 0x06 );					// write enable
    SFM_Transaction_open();
    SFM_PutByte( 0x02 );					// page program
    SFM_PutByte( ( lAddr >> 16 ) & 0xff );	// ...address(page)
    SFM_PutByte( ( lAddr >>  8 ) & 0xff );
    SFM_PutByte( ( lAddr       ) & 0xff );
	for( i = 0 ; i < nSize ; i++ ){
		SFM_PutByte( ( pBuff[i] & 0xFF));			// ...data
		SFM_PutByte( ((pBuff[i]>> 8) & 0xFF) );			// ...data
		SFM_PutByte( ((pBuff[i]>>16) & 0xFF) );			// ...data
		SFM_PutByte( ((pBuff[i]>>24) & 0xFF) );			// ...data
	}
	SFM_Transaction_close();
}

// Read Data Bytes
void	SerialRomReadDataBytes( ULONG lAddr, UCHAR *pBuff, LONG nSize )
{
	LONG	i;
	SFM_Transaction_open();
    SFM_PutByte( 0x03 );					// normal read
    SFM_PutByte( ( lAddr >> 16 ) & 0xff );	// ...address
    SFM_PutByte( ( lAddr >>  8 ) & 0xff );
    SFM_PutByte( ( lAddr       ) & 0xff );
	for( i = 0 ; i < nSize ; i++ )
	{
		*(pBuff++) = SFM_GetByte();		// ...data
	}
    SFM_Transaction_close();
}

void	SerialRomReadDataBytes4( ULONG lAddr, ULONG *pBuff, LONG nSize )
{
	LONG i;
	SFM_Transaction_open();
    SFM_PutByte( 0x03 );					// normal read
    SFM_PutByte( ( lAddr >> 16 ) & 0xff );	// ...address
    SFM_PutByte( ( lAddr >> 8  ) & 0xff );
    SFM_PutByte(   lAddr         & 0xff );
	for( i = 0 ; i < nSize * 4 ; i++ )
	{
		*(pBuff++) = SFM_GetByte();		// ...data
	}
    SFM_Transaction_close();
}

// Read Data Bytes(Verify)
LONG SerialRomCheckDataBytes( ULONG lAddr, UCHAR *pBuff, LONG nSize )
{
    LONG nRet;
	LONG	i;
    SFM_Transaction_open();
    SFM_PutByte( 0x03 );						// normal read
    SFM_PutByte( ( lAddr >> 16 ) & 0xff );	// ... address
    SFM_PutByte( ( lAddr >> 8  ) & 0xff );
    SFM_PutByte(   lAddr         & 0xff );
	nRet = OK;
	for( i = 0 ; i < nSize ; i++ )
	{
		if( *(pBuff++) != SFM_GetByte() )
		{
			nRet = NG;	// ...check data
		}
	}
    SFM_Transaction_close();
    return( nRet );
}

// Deep Power-down
void	SerialRomDeepPowerDown( void )
{
    SFM_Transaction_open();
    SFM_PutByte( 0xB9 );		// deep power down
    SFM_Transaction_close();
}

// Release from Deep Power-down
void	SerialRomReleaseDeepPowerDown( void )
{
    SFM_Transaction_open();
    SFM_PutByte( 0xAB );		// release
    SFM_Transaction_close();
}

// WREN			Write Enable					0000 0110	0x06	0 0 0
// WRDI			Write Disable					0000 0100	0x04	0 0 0
// RDID			Read Identification				1001 1111	0x9F	0 0 1 to 3
// RDSR			Read Status Register			0000 0101	0x05	0 0 1 to ∞
// WRSR			Write Status Register			0000 0001	0x01	0 0 1
// READ			Read Data Bytes					0000 0011	0x03	3 0 1 to ∞
// FAST_READ	Read Data Bytes at Higher Speed	0000 1011	0x0B	3 1 1 to ∞
// PP			Page Program					0000 0010	0x02	3 0 1 to 256
// SE			Sector Erase					1101 1000	0xD8	3 0 0
// BE			Bulk Erase						1100 0111	0xC7	0 0 0
// DP			Deep Power-down					1011 1001	0xB9	0 0 0
// RES			Release from Deep Power-down,
// 				and Read Electronic Signature	1010 1011	0xAB	0 3 1 to ∞
// 				Release from Deep Power-down						0 0 0
/*-------end of file----------------------------------------------------------------------------------------*/
