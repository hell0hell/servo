/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Eeprom.c : EEPROM Device Driver																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : I2C for EEPROM/Temperature IC Device Driver for SII S-24C32C							*/
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
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.07.25  K.Ozaki																*/
/*		Rev.2.00 : 2013.12.17  Y.Oka <S097>															*/
/*																									*/
/****************************************************************************************************/
#include	"KnlApi.h"



/****************************************************************************************************/
/*																									*/
/*		Function Prototypes																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
LONG	I2CdrvInitCpuI2CBusIF( void );			/* I2Cバス I/Fの初期化								*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	eepI2Cdrv_SoftReset( void );
static LONG	i2Cdrv_Disable( IIC_REG *ireg );
static LONG	i2Cdrv_Enable( IIC_REG *ireg );



/****************************************************************************************************/
/*																									*/
/*		Initialize I2C Bus Interface (JL086)														*/
/*																									*/
/****************************************************************************************************/
LONG	I2CdrvInitCpuI2CBusIF( void )
{
/*--------------------------------------------------------------------------------------------------*/
	LONG		wkx;											/* work 							*/
	LONG		ret = I2C_DRV_SUCCESS;							/* Return Value						*/
	IIC_REG		*ireg = I2C_CH0_REGBASE;						/* I2C Ch. 0 						*/
	LONG		fast_mode = 1;									/* zantei							*/
	LONG		RefClkKhz = 9600;								/* 9.6Mbps							*/
	VUINT8		*SCU_REG = (void *)0xEFF86010;					/* I2C Source Clock Register		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Disable I2C Communication																	*/
/*--------------------------------------------------------------------------------------------------*/
	ret = i2Cdrv_Disable( ireg );
	if( I2C_DRV_SUCCESS != ret )
	{ /* Detect Time Out */
		return	ret;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Set I2C Clock		 																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( fast_mode )
	{
//		wkx = *SCU_REG;											/* I2C Clock Select (Fxx/24) 		*//* I2Cの初期化はJL086Core.cに統一 <S152> */
//		*SCU_REG = ((wkx & 0x3C) | 0x02);						/* 10B: Mode3 (9.6Mbps)				*//* I2Cの初期化はJL086Core.cに統一 <S152> */
		I2C_REG_WR( ireg->IICCL, SMC | DFC );					/* 9600(Kbps)/24=400(Kbps)			*/
	}
	else
	{
		if ( (2000 <= RefClkKhz) && (RefClkKhz <= 4190) )
		{
			I2C_REG_WR( ireg->IICCL, DFC );						/* I2C Clock Select (Fxx/44) 		*/
		}
		else
		{
			I2C_REG_WR( ireg->IICCL, DFC | CL0);				/* I2C Clock Select (Fxx/86) 		*/
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Set I2C Configuration																		*/
/*--------------------------------------------------------------------------------------------------*/
	I2C_REG_WR( ireg->IICF, STCEN | IICRSV );					/* 通信許可 & 通信予約禁止			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Enable I2C Communication																	*/
/*--------------------------------------------------------------------------------------------------*/
	ret = i2Cdrv_Enable( ireg );
	if( I2C_DRV_SUCCESS != ret )
	{ /* Detect Time Out */
		return	ret;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Soft Reset EEPROM( Reset EEPROM device )													*/
/*		※最初にSTOPを送らないとうまくいかないようなので送っておく 									*/
/*--------------------------------------------------------------------------------------------------*/
	ret = eepI2Cdrv_SoftReset( );
	if( I2C_DRV_SUCCESS != ret )
	{ /* Detect Time Out */
		return	ret;
	}

/*--------------------------------------------------------------------------------------------------*/
	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		I2C Bus Interrupt Handler																	*/
/*		1)割込発生条件: STOPの送信、またはデータの9clk目を送信										*/
/*																									*/
/****************************************************************************************************/
void IntCpuI2CBusIF( void )
{
//UINT	IntI2cTest;
//UINT	IntI2cSts;
/*--------------------------------------------------------------------------------------------------*/
//struct _IIC_REG_ *ireg = I2C_CH0_REGBASE;						/* I2C Ch. 0 						*/
//		IntI2cTest++;
//		I2C_REG_RD( IICS, IntI2cSts );							/* Check IICS 						*/
/*--------------------------------------------------------------------------------------------------*/
		return;
}



/****************************************************************************************************/
/*																									*/
/*		EEPROM-I2C : Write EEPROM Word																*/
/*																									*/
/****************************************************************************************************/
//LONG	EepI2Cdrv_WriteEepromWord( ULONG EepAdr, USHORT WrData ) /* <S0E4> */
LONG	EepI2Cdrv_WriteEepromWord( ULONG EepAdr, USHORT WrData, ULONG EepromNo )
{
	LONG	i;
	ULONG	time;
	ULONG	NackCnt = 0;
	ULONG	SlvAddr;

/*--------------------------------------------------------------------------------------------------*/
/*		Check and Set Slave Address												<S0E4>				*/
/*--------------------------------------------------------------------------------------------------*/
	if( EepromNo == 0 )
	{
		SlvAddr = KPX_I2CEEP1_DEV_ADR_WR;
	}
	else
	{
		SlvAddr = KPX_I2CEEP2_DEV_ADR_WR;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		I2C Set Stop Condition																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_STOP();
/*--------------------------------------------------------------------------------------------------*/
/*		Wait Stop Condition End																		*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_STOP_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		Write Start																					*/
/*--------------------------------------------------------------------------------------------------*/
//	SlvAddr = KPX_I2CEEP1_DEV_ADR_WR;  /* <S0E4> */
	KPX_I2C_START( );										/* Send Start Condition	and 1st byte	*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_START_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		Device/Eeprom Address																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( SlvAddr ); 										/* Write Slave Address to Register	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Wait Transmit Finish																		*/
/*--------------------------------------------------------------------------------------------------*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM High Address																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( CFG_EEPROM_EEPADR_OUTH == 1 )							/* Check EEPROM High Address		*/
	{
		KPX_I2C_TX( EepAdr >> 8 );								/* Write EepAdr Low  to TxBuf		*/
//		KlibRstLongTimer( &time ); /* <S0A8> */
//		do {
		for( i=0; i < 50; i++ )
		{
			KlibWaitus( 1 );										/* Wait 1 us						*/
			if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
			{
				break;
			}
		} //while( ! KPX_I2C_CHK_TX_END( ) );
	}															/*									*/
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM Low Address																			*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( EepAdr );										/* Write EepAdr Low to TxBuf　		*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Write EEPROM Low Data																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( WrData );										/* Write Data Low to TxBuf			*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Write EEPROM High Data																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( (WrData)>>8 );									/* Write Data High to TxBuf			*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Write End																					*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_STOP( );											/* Write End						*/

/*--------------------------------------------------------------------------------------------------*/
/*		Check Nack Count																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( NackCnt > 0 )
	{
//		I2cEepRwMng.WrNackCnt += (USHORT)NackCnt;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Wait Write EEPROM Data																		*/
/*--------------------------------------------------------------------------------------------------*/
	KlibWaitms( 10 );

	return	I2C_DRV_SUCCESS;
}

/****************************************************************************************************/
/*																									*/
/*		EEPROM-I2C : Read EEPROM Word																*/
/*							 : Using Sequencial Read												*/
/*																									*/
/****************************************************************************************************/
//LONG	EepI2Cdrv_ReadEepromWord( ULONG EepAdr, USHORT *RdData )  /* <S0E4> */
LONG	EepI2Cdrv_ReadEepromWord( ULONG EepAdr, USHORT *RdData, ULONG EepromNo )
{
	LONG	i;
	ULONG	time;
	ULONG	SlvAddr;
	ULONG	NackCnt = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Stop Condition																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_STOP( );											/* Stop Condition 					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Wait Stop Condition End																		*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_STOP_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		Read Start																<S0E4>				*/
/*--------------------------------------------------------------------------------------------------*/
	if( EepromNo == 0 )
	{
		SlvAddr = KPX_I2CEEP1_DEV_ADR_WR;						/* Set TxMode						*/
	}
	else
	{
		SlvAddr = KPX_I2CEEP2_DEV_ADR_WR;						/* Set TxMode						*/
	}
	KPX_I2C_START( );											/* Start Condition					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Wait Start Condition Finish																	*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_START_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		Device/Eeprom Address																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( SlvAddr ); 										/* Write Slave Address to Register	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Wait Transmit Finish																		*/
/*--------------------------------------------------------------------------------------------------*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM High Address																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( CFG_EEPROM_EEPADR_OUTH == 1 )							/* Check EEPROM High Address		*/
	{
		KPX_I2C_TX( EepAdr >> 8 );								/* Write EepAdr Low  to TxBuf		*/
//		KlibRstLongTimer( &time ); /* <S0A8> */
//		do {
		for( i=0; i < 50; i++ )
		{
			KlibWaitus( 1 );									/* Wait 1 us						*/
			if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
			{
				break;
			}
		} //while( ! KPX_I2C_CHK_TX_END( ) );
	}															/*									*/
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM Low Address																			*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( EepAdr );										/* Write EepAdr Low to TxBuf　		*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Read EEPROM Data														<S0E4>				*/
/*--------------------------------------------------------------------------------------------------*/
	if( EepromNo == 0 )
	{
		SlvAddr = KPX_I2CEEP1_DEV_ADR_RD;						/* Write Addr to TxBuf				*/
	}
	else
	{
		SlvAddr = KPX_I2CEEP2_DEV_ADR_RD;						/* Write Addr to TxBuf				*/
	}
	KPX_I2C_START( );											/* Start Condition					*/
/*--------------------------------------------------------------------------------------------------*/
/*		Wait Start Condition Finish																	*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do { /* <S0A8> */
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_START_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		Send Slave Address																			*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( SlvAddr );
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_TX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Read Low Byte Data 																			*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_RX( );												/* Start receiving (1st data)		*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_RX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_RX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/
	*RdData = KPX_I2C_GetRdData;

/*--------------------------------------------------------------------------------------------------*/
/*		Read High Byte Data																			*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_RX( );										/* Start receiving (2nd data)				*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KPX_I2C_CHK_RX_END() ) /* <S0A8> */
		{
			break;
		}
	} //while( ! KPX_I2C_CHK_RX_END( ) );
	KPX_I2C_NackCnt( NackCnt );									/* CountUp NackCnt If Nack			*/
	*RdData |= (KPX_I2C_GetRdData << 8 );						/* Read High Byte Data from RxBuf	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Read End																					*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_STOP( );											/* Stop Condition					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Check Nack Count																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( NackCnt > 0 )
	{
//		I2cEepRwMng.RdNackCnt += (USHORT)NackCnt;
	}
/*--------------------------------------------------------------------------------------------------*/
	return	I2C_DRV_SUCCESS;
}



/****************************************************************************************************/
/*																									*/
/*		Soft Rest EEPROM API																		*/
/*																									*/
/****************************************************************************************************/
static LONG	eepI2Cdrv_SoftReset( void )
{
	LONG	i;
	ULONG	time;

/*--------------------------------------------------------------------------------------------------*/
/*		I2C set start condition																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_START();

/*--------------------------------------------------------------------------------------------------*/
/*		Wait Start Condition Finish																	*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_START_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		Send Dummy Clock																			*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_TX( 0xFF );

/*--------------------------------------------------------------------------------------------------*/
/*		Wait Transmit Finish																		*/
/*--------------------------------------------------------------------------------------------------*/
//	KlibRstLongTimer( &time ); /* <S0A8> */
//	do {
	for( i=0; i < 50; i++ )
	{
		KlibWaitus( 1 );										/* Wait 1 us						*/
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} //while( ! KPX_I2C_CHK_TX_END( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		I2C Set Start Condition																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_START();

/*--------------------------------------------------------------------------------------------------*/
/*		wait Start Condition Finish																	*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_START_END_CHK( ) );

/*--------------------------------------------------------------------------------------------------*/
/*		I2C Set Stop Condition																		*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_I2C_STOP();

/*--------------------------------------------------------------------------------------------------*/
/*		Wait Stop Condition End																		*/
/*--------------------------------------------------------------------------------------------------*/
	KlibRstLongTimer( &time );
	do {
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			return I2C_TIMEOUT_ERR;
		}
	} while( ! KPX_I2C_STOP_END_CHK( ) );


	KlibWaitus(10); /* 念のため */
	return I2C_DRV_SUCCESS;
}



/****************************************************************************************************/
/*																									*/
/*		I2CDriver Disable API																		*/
/*																									*/
/****************************************************************************************************/
static LONG	i2Cdrv_Disable( IIC_REG *ireg )
{
	LONG	ret;
	UINT	wkx;
	ULONG	time;

	ret = I2C_DRV_SUCCESS;
	KlibRstLongTimer( &time );

	I2C_REG_WR( ireg->IICACT, 0 ); 					/* set I2C disable */
	do {
		I2C_REG_RD( ireg->IICACT, wkx );
		KlibWaitus( 1 );							/* wait until I2C is disable */
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			ret = I2C_TIMEOUT_ERR;
			break;
		}
	} while( wkx != 0 );

	return	ret;
}



/****************************************************************************************************/
/*																									*/
/*		I2CDriver Enable API																		*/
/*																									*/
/****************************************************************************************************/
static LONG	i2Cdrv_Enable( IIC_REG *ireg )
{
	LONG	ret;
	UINT	wkx;
	ULONG	time;

	ret = I2C_DRV_SUCCESS;
	KlibRstLongTimer( &time );

	I2C_REG_WR( ireg->IICACT, IICE ); 				/* set I2C enable */

	do {
		I2C_REG_RD( ireg->IICACT, wkx );
		KlibWaitus( 1 );							/* wait until I2C is enable */
		if( KlibGetLongTimerMs(time) > I2C_DRV_TIMEOUT )
		{
			ret = I2C_TIMEOUT_ERR;
			break;
		}
	} while( (wkx & IICE)^IICE );

	return	ret;
}



/***************************************** end of file **********************************************/
