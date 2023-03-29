/****************************************************************************************************/
/*																									*/
/*																									*/
/*		EepromIf.c : I2C用EEPROM I/F定義定義														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : EEPROMへのアクセス処理																	*/
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
/*	Note	:	初版	2010.05.25	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "I2C_EepromDrv.h"

/****************************************************************************************************/
/*																									*/
/*		ＥＥＰＲＯＭインタフェイス定義																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		EEPROM読み出し／書き込みデータ ( S-29530A:2kbyte / S-29630A:4kbyte )						*/
/*																									*/
/*			1) Set Read Command to EPRLW,EPRHW,EPRCNT												*/
/*						 F  E  D  C  B  A  9  8  7  6  5  4  3  2  1  0								*/
/*				--------------------------------------------------------							*/
/*				EPRLW	 1  1  1  1 A9 A8 A7 A6 A5 A4 A3 A2 A1 A0  0  1		S-29530A:2kbyte			*/
/*				EPRHW	 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1								*/
/*				EPRCNT   0x008A	(29clock)															*/
/*				--------------------------------------------------------							*/
/*				EPRLW	 1  1 AA A9 A8 A7 A6 A5 A4 A3 A2 A1 A0 AX  0  1		S-29630A:4kbyte			*/
/*				EPRHW	 1  1  1  1  1  1  1  1  1  1  1  1  1  1  1  1								*/
/*				EPRCNT   0x0082	(31clock)															*/
/*																									*/
/*			2) Set Write Command to EPRLW,EPRHW,EPRCNT												*/
/*						  F  E  D  C  B  A  9  8  7  6  5  4  3  2  1  0							*/
/*				---------------------------------------------------------							*/
/*				EPRLW	 D3 D2 D1 D0 A9 A8 A7 A6 A5 A4 A3 A2 A1 A0  1  0	S-29530A:2kbyte			*/
/*				EPRHW	  1  1  1  1 DF DE DD DC DB DA D9 D8 D7 D6 D5 D4							*/
/*				EPRCNT   0x008A (29clock)															*/
/*				---------------------------------------------------------							*/
/*				EPRLW	 D1 D0 AA A9 A8 A7 A6 A5 A4 A3 A2 A1 A0 AX  1  0	S-29630A:4kbyte			*/
/*				EPRHW	  1  1 DF DE DD DC DB DA D9 D8 D7 D6 D5 D4 D3 D2							*/
/*				EPRCNT   0x0082	(31clock)															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		EEPROM I/F Definition for BR24L32W	I2C I/F type									<V629>	*/
/*--------------------------------------------------------------------------------------------------*/
//#define	KPX_STRT_HOLDTIME				0x18B0			/* FAST-MODE: 0.6us+400ns=1us				*/
//#define	KPX_STOP_SETUPTIME				0x18E8			/* FAST-MODE: 0.6us-300ns=0.3us				*/
#define	KPX_STRT_HOLDTIME				0x0868			/* FAST-MODE: 0.6us+400ns=1us				*/
#define	KPX_STOP_SETUPTIME				0x08A0			/* FAST-MODE: 0.6us-300ns=0.3us				*/
#define	KPX_EPRCLK_100K					0x1A38
#define	KPX_EPRCLK_400K					0x1ACE			/* FAST-MODE: 400KHz						*/
//#define	KPX_EPRCLK_350K					0x1AC7			/* FAST-MODE: 350KHz						*/
#define	KPX_EPRCLK_HALFCLK				0x00FE
#define	KPX_EPRCLK_1CLK					0x00FC
#define	KPX_EPRCLK_8CLK					0x00E0
#define	KPX_EPRCLK_9CLK					0x00DC
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_WAIT_DEV_ADR				0x00A0
#define	KPX_WR_DEV_ADR					0x00A0
#define	KPX_RD_DEV_ADR					0x00A1
#define	KPX_SDA_LOW						0x0000
#define	KPX_SDA_HIGH					0x0080
#define	KPX_E0_TERMINAL_HIGH_Z			0x00FF
#define	KPX_EEP_ACK						0x0000
#define	KPX_EEP_NCK						0x0080



/****************************************************************************************************/
/*																									*/
/*		ＥＥＰＲＯＭワードデータ読み出し	（ランダムリード）										*/
/*																									*/
/****************************************************************************************************/
LONG EepI2Cdrv_ReadEepromWord(ASIC_HW_IF_STR *pAsicHwReg, LONG axis, ULONG eaddr, USHORT *rdVal)
{
#if 0 /* 2012.08.18 Y.Oka Make Mercury Base Software */

	USHORT			Data;
	VUSHORT			Dummy;
	ULONG			poll_cnt;

	/* ローカル変数の初期化 */
	poll_cnt = 0;

	/* for debug */
	pAsicHwReg->AREG_HPDO &= ~(0x01);					/* Write Protect 解除						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
/*	スタートコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_STRT_HOLDTIME;		/* I2C利用、ホールド時間 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_SDA_LOW;				/* SDAをLowに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;			/* EEPROMクロック = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	スレーブアドレス送信																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ダミーライト */
	pAsicHwReg->AREG_EPRSET = KPX_EPRCLK_400K;			/* EEPROMクロック周波数 400KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_WR_DEV_ADR | (axis << 1));	/* 1 0 1 0 A2 A1 A0 W *  A2,A1,A0 = axis	*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;			/* EEPROMクロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	書込みアドレス送信																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* 下位アドレス書き込み */
	pAsicHwReg->AREG_I2CDATA = ( (USHORT)eaddr & 0x00FF );
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;			/* EEPROMクロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	スタートコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_STRT_HOLDTIME;		/* I2C利用、ホールド時間 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_SDA_LOW;				/* SDAをLowに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;			/* EEPROMクロック = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	スレーブアドレス送信																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_EPRCLK_400K;			/* EEPROMクロック周波数 400KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_RD_DEV_ADR | (axis << 1));	/* 1 0 1 0 A2 A1 A0 R *  A2,A1,A0 = axis	*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;			/* EEPROMクロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	読出しデータ受信																				*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_I2CDATA = KPX_E0_TERMINAL_HIGH_Z;	/* 外部端子E0をHi-Z状態にする				*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_8CLK;			/* EEPROMクロック = 8clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
/*--------------------------------------------------------------------------------------------------*/
	Data = pAsicHwReg->AREG_I2CDI;

//	if( size == EEPROM_2UNIT )
	{ /* 2バイト読出し要求の場合 */
	/*----------------------------------------------------------------------------------------------*/
	/*	アクノリッジ送信																			*/
	/*----------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_I2CDATA = KPX_EEP_ACK;			/* ACK										*/
		pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;		/* EEPROMクロック = 1clock 					*/
		Dummy = pAsicHwReg->AREG_EPRSET;				/* Dummy Read								*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* Wait */
			KlibWaitus(I2C_WAIT_TICK);	
			if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}

	/*----------------------------------------------------------------------------------------------*/
	/*	読出しデータ受信																			*/
	/*----------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_I2CDATA = KPX_E0_TERMINAL_HIGH_Z;	/* 外部端子E0をHi-Z状態にする			*/
		pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_8CLK;			/* EEPROMクロック = 8clock				*/
		Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read							*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* Wait */
			KlibWaitus(I2C_WAIT_TICK);	
			if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}
	/*----------------------------------------------------------------------------------------------*/
		Data |= (pAsicHwReg->AREG_I2CDI << 8);
	}
/*--------------------------------------------------------------------------------------------------*/
/*	No ACK送信																						*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_I2CDATA = KPX_EEP_NCK;				/* NO ACK 									*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;			/* EEPROMクロック = 1clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	ストップコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_STOP_SETUPTIME;		/* 0.3us setuptime							*/
	pAsicHwReg->AREG_I2CDATA = KPX_SDA_HIGH;			/* SDAをHiに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;			/* EEPROMクロック = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

	/* for debug */
	pAsicHwReg->AREG_HPDO |= 0x01;						/* Write Protect 設定						*/
	/* for debug */

	*rdVal = Data;

#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */

/*--------------------------------------------------------------------------------------------------*/
	return( EEPROM_I2C_SUCCESS );
/*--------------------------------------------------------------------------------------------------*/
}



/****************************************************************************************************/
/*																									*/
/*		ＥＥＰＲＯＭワードデータ書き込み															*/
/*																									*/
/****************************************************************************************************/
LONG EepI2Cdrv_WriteEepromWord( ASIC_HW_IF_STR *pAsicHwReg, LONG axis, ULONG eaddr, USHORT wdata )
{
#if 0 /* 2012.08.18 Y.Oka Make Mercury Base Software */

	VUSHORT			Dummy;
	ULONG			poll_cnt;

	/* ローカル変数の初期化 */
	poll_cnt = 0;

	/* for debug */
	pAsicHwReg->AREG_HPDO &= ~(0x01);					/* Write Protect 解除						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
/*	スタートコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_STRT_HOLDTIME;		/* I2C利用、ホールド時間 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_SDA_LOW;				/* SDAをLowに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;			/* EEPROMクロック = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	スレーブアドレス送信																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_EPRCLK_400K;			/* EEPROMクロック周波数 400KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_WR_DEV_ADR | (axis << 1));	/* 1 0 1 0 A2 A1 A0 W *  A2,A1,A0 = axis	*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;			/* EEPROMクロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	書込みアドレス送信																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* アドレス書き込み */
	pAsicHwReg->AREG_I2CDATA = ( (USHORT)eaddr & 0x00FF );
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;			/* EEPROMクロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	書込みデータ送信																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* 下位バイトのデータ書き込み */
	pAsicHwReg->AREG_I2CDATA = ( wdata & 0x00FF );
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;			/* EEPROMクロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

//	if( size == EEPROM_2UNIT )
	{ /* 2バイト書き込み要求の場合 */
		/* 上位バイトのデータ書き込み */
		pAsicHwReg->AREG_I2CDATA = ( (wdata & 0xFF00) >> 8 );
		pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_9CLK;		/* EEPROMクロック = 9clock					*/
		Dummy = pAsicHwReg->AREG_EPRSET;				/* Dummy Read								*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* Wait */
			KlibWaitus(I2C_WAIT_TICK);	
			if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}
		while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
		{	 /* Wait */
			KlibWaitus(I2C_WAIT_TICK);	
			if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*	ストップコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_STOP_SETUPTIME;		/* セットアップ時間 0.3us					*/
	pAsicHwReg->AREG_I2CDATA = KPX_SDA_HIGH;			/* SDAをHiに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_EPRCLK_1CLK;			/* EEPROMクロック = 1clock					*/
/*--------------------------------------------------------------------------------------------------*/
	/* 書き込み時間分WAIT */
	KlibWaitms( 5 );

	/* for debug */
	pAsicHwReg->AREG_HPDO |= 0x01;						/* Write Protect 設定						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */

	return EEPROM_I2C_SUCCESS;
}

#ifdef EEPROM_DEBUG
void I2C_test(void)
{
	LONG	i;
	USHORT	data;
	for( i = 0; i <= 0x00FE; i+=2 )
	{
		data = (i>>1) + (i<<7);
		printf( "Write Address = %x, Write Data = %x\n", i, data );
		WriteEepromWord( 0, i, 1, data );
		WaitEepromWriteComp( 0 );
		data = ReadEepromWord( 0, i, 1 );
		WaitEepromReadComp( );
		printf( "Read Address = %x, Read Data = %x\n\n", i, data );
	}
	for( i = 0; i <= 0x00FF; i+=2 )
	{
		data = ReadEepromWord( 0, i, 1 );
		WaitEepromReadComp( );
		printf( "Read Address = %x, Read Data = %x\n", i, data );
	}
}
#endif



/***************************************** end of file **********************************************/
