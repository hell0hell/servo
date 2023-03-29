/****************************************************************************************************
Description	:	I/F Definision by I2C for Temperature Sensor IC(S-5851AAA-M6T1)	<TempSensorIC>
				I2C用温度センサIC I/F定義
-----------------------------------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	TempICI2Cdrv_Initialize()		: Initialize the temperature sensor IC
	TempICI2Cdrv_Read_Temperature()	: Store temperature data in buffer
	TempICI2Cdrv_Read()				: Read 16bit data from temperature sensor IC (S-5851AAA-M6T1)
	TempICI2Cdrv_Write()			: Write 8bit data to tenmpareture sensor IC (S-5851AAA-M6T1)

温度センサIC読み出し／書き込みデータ ( S-5851AAA-M6T1 )

1) To send Device address
		   7  6  5  4  3  2  1  0
-------------------------------------
I2CDATA	   1  0  0  1 A2 A1 A0 R/W
EPRCNT	   0x00DC (9clock)
-------------------------------------
R/W  : 0 = Write, 1 = Read
IC① : A2～0 = 010
IC② : A2～0 = 111

1) To write data to Pointer-Register
		   7  6  5  4  3  2  1  0
-------------------------------------
I2CDATA	   0  0  0  0  0  0  0 P0
EPRCNT	   0x00DC (9clock)
-------------------------------------
P0 : 0 = Temperature-Register
	 1 = Configration-Register


1) To write data to Configration-Register
		   7  6  5  4  3  2  1  0
-------------------------------------
I2CDATA	  OS  *  *  *  *  *  * SD
EPRCNT	   0x00DC (9clock)
-------------------------------------
OS : 1 = Shutdown mode
SD : 1 = One-Shot mode

1) Set Read Command to I2CDATA,
		 F  E  D  C  B  A  9  8  7  6  5  4  3  2  1  0
--------------------------------------------------------
I2CDI0-7 1  1  1  1 A9 A8 A7 A6 A5 A4 A3 A2 A1 A0  0  ACK
EPRCNT   0x008A	(29clock)
--------------------------------------------------------

-----------------------------------------------------------------------------------------------------
Changes		:
Name			Date			Description
-----------------------------------------------------------------------------------------------------
A.Hashimoto		2013.5.20		created

*****************************************************************************************************/
#include "Basedef.h"
#include "I2C_TempSensorICDrv.h"


/****************************************************************************************************
		Temperature sensor IC I/F Definition for BR24L32W	I2C I/F type
*****************************************************************************************************/
//



extern USHORT		DebugBuf1[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf2[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf3[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern USHORT		DebugBuf4[100];		/* デバッグ用											 *//* <TempSensorIC> */
extern INT			DebugCount;				/* デバッグ用											 *//* <TempSensorIC> */
extern INT 			Debug_i;				/* wait用												 *//* <TempSensorIC> */
extern INT 			Debug_j;				/* 														 *//* <TempSensorIC> */



/****************************************************************************************************
Description: Initialize the temperature sensor IC (S-5851AAA-M6T1)
			温度センサIC 初期化

Parameters:
	
Return:
	Error
*****************************************************************************************************/
void TempICI2Cdrv_Initialize(ASIC_HW_IF_STR *pAsicHwReg)
{

	LONG		device;						/* Device address (A2,A1,A0) of Temperature Sensor IC	*/
	ULONG		eaddr;						/* Pointer-Register address of Temperature Sensor IC	*/
	UCHAR		data;						/* 8 bit data to be written								*/
	LONG		rslt;						/* result												*/
	
	
	/* write data to IC1 Configration-Register*/
	device = KPX_TEMPIC_ADR1;
	eaddr = KPX_TEMPIC_REG_CONFIG;
	data = 0x0000;										/* OS=0FF SD=OFF							*/
	rslt = TempICI2Cdrv_Write(pAsicHwReg, device, eaddr, data );
	
	if(rslt == 1)
	{
		Debug_i++;
	}
	
	/* write data to IC2 Configration-Register*/
	device = KPX_TEMPIC_ADR2;
	eaddr = KPX_TEMPIC_REG_CONFIG;
	data = 0x0000;										/* OS=0FF SD=OFF							*/
	rslt = TempICI2Cdrv_Write(pAsicHwReg, device, eaddr, data );
	
	if(rslt == 1)
	{
		Debug_i++;
	}

	/* 確認でrsltのチェック入れるべきだけどとりあえずなしで。*/
	/* 確認でread入れるべきだけどとりあえずなしで。*/

}

/****************************************************************************************************
Description:  Store temperature data in buffer
			温度センサIC 温度レジスタ読み出し

Parameters:
	ASIC_HW_IF_STR 	*pAsicHwReg : Pointer to the ASIC hardware register
Return:
	Error
*****************************************************************************************************/
void TempICI2Cdrv_Read_Temperature(ASIC_HW_IF_STR *pAsicHwReg)
{

	LONG 		device;		/* Device address (A2,A1,A0) of Temperature Sensor IC					*/
	ULONG		eaddr;		/* Pointer-Register address of Temperature Sensor IC					*/
	LONG		rslt;		/* result																*/
	USHORT		data1;		/* read data															*/
	USHORT		data2;		/* read data															*/
	
	
/*---------------------------------------------------------------------------------------------------
		デバッグのため、バッファに格納する
----------------------------------------------------------------------------------------------------*/
	if( Debug_j < 500 )	/* 2013.05.28<tanaka21> */
	{	/* 2ms * 500 = 1sec 毎に一回読む*/
		Debug_j++;
	}
	else
	{
		/* Raed data from IC1 Temperature-Register	*/
		device = KPX_TEMPIC_ADR1;
		eaddr = KPX_TEMPIC_REG_TEMP;
		rslt = TempICI2Cdrv_Read(pAsicHwReg, device, eaddr, &data1);
		
		if(rslt == 1)
		{
			Debug_i++;
		}

		/* Raed data from IC2 Temperature-Register	*/
		device = KPX_TEMPIC_ADR2;
		eaddr = KPX_TEMPIC_REG_TEMP;
		rslt = TempICI2Cdrv_Read(pAsicHwReg, device, eaddr, &data2);

		if(rslt == 1)
		{
			Debug_i++;
		}

		DebugBuf1[ (UINT)(DebugCount) ] = data1;
		DebugBuf2[ (UINT)(DebugCount) ] = data2;
		DebugBuf3[ (UINT)(DebugCount) ] = data1 >> 8;	/* 下位4bit無視＆単位換算 [deg]*/
		DebugBuf4[ (UINT)(DebugCount) ] = data2 >> 8;	/* 下位4bit無視＆単位換算 [deg]*/
		DebugCount++;
		if( DebugCount == 100 )	/* 2013.05.28<tanaka21> */
		{
			DebugCount = 0;
		}
		Debug_j = 0;
	}
	
	/* 確認でrsltのチェック入れるべきだけどとりあえずなしで。*/

}

/****************************************************************************************************
Description: Read 16bit data from temperature sensor IC (S-5851AAA-M6T1)
			温度センサIC ワード(16bit)データ読み出し

Parameters:
	ASIC_HW_IF_STR	*pAsicHwReg	: Pointer to the ASIC hardware register
	LONG			device		: Device address (A2,A1,A0) of Temperature Sensor IC
	ULONG			eaddr		: Pointer-Register address of Temperature Sensor IC
	USHORT			*rdVal		: Pointer to the buffer for reading
	
Return:
	Error
*****************************************************************************************************/
LONG TempICI2Cdrv_Read(ASIC_HW_IF_STR *pAsicHwReg, LONG device, ULONG eaddr, USHORT *rdVal)
{

	USHORT			data;
	VUSHORT			Dummy;
	ULONG			poll_cnt;

	/* ローカル変数の初期化 */
	poll_cnt = 0;

	/* for debug */
//	pAsicHwReg->AREG_HPDO &= ~(0x01);					/* Write Protect 解除						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
/*	スタートコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STRT_HOLDTIME;	/* I2C利用、ホールド時間 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_LOW;		/* SDAをLowに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* クロック = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	スレーブアドレス送信																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ダミーライト */
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_CLK_385K;		/* クロック周波数 385KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_TEMPIC_WR_DEV_ADR | (device << 1));	/* 1 0 0 1 A2 A1 A0 W : A2,A1,A0 = device	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;			/* クロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACKが返ってくるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	ポインタレジスタ指定(送信)																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* アドレス書き込み */
	pAsicHwReg->AREG_I2CDATA = ( (USHORT)eaddr & 0x0001 );/* 0000 000*	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* クロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACKが返ってくるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	
/*--------------------------------------------------------------------------------------------------*/
/*	スタートコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STRT_HOLDTIME;	/* I2C利用、ホールド時間 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_LOW;		/* SDAをLowに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* クロック = 1clock 					*/
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
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_CLK_385K;		/* クロック周波数 385KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_TEMPIC_RD_DEV_ADR | (device << 1));	/* 1 0 0 1 A2 A1 A0 R : A2,A1,A0 = device	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;			/* クロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACKが返ってくるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	読出しデータ受信																				*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_I2CDATA = KPX_E0_TERMINAL_HIGH_Z;	/* 外部端子E0をHi-Z状態にする				*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_8CLK;	/* クロック = 8clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
/*--------------------------------------------------------------------------------------------------*/
	data = (pAsicHwReg->AREG_I2CDI << 8);

//	if( size == EEPROM_2UNIT )
	{ /* 2バイト読出し要求の場合 */
	/*----------------------------------------------------------------------------------------------*/
	/*	アクノリッジ送信																			*/
	/*----------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_I2CDATA = KPX_EEP_ACK;			/* ACK										*/
		pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;		/* クロック = 1clock 					*/
		Dummy = pAsicHwReg->AREG_EPRSET;				/* Dummy Read								*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* ASICのステータスが通信完了になるまで待つ */
			KlibWaitus(I2C_WAIT_TICK);	
			++poll_cnt;
			if(poll_cnt >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}

	/*----------------------------------------------------------------------------------------------*/
	/*	読出しデータ受信																			*/
	/*----------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_I2CDATA = KPX_E0_TERMINAL_HIGH_Z;	/* 外部端子E0をHi-Z状態にする			*/
		pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_8CLK;	/* クロック = 8clock				*/
		Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read							*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* ASICのステータスが通信完了になるまで待つ */
			KlibWaitus(I2C_WAIT_TICK);	
			++poll_cnt;
			if(poll_cnt >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}
	/*----------------------------------------------------------------------------------------------*/
		data |= pAsicHwReg->AREG_I2CDI;
	}
/*--------------------------------------------------------------------------------------------------*/
/*	No ACK送信																						*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_I2CDATA = KPX_EEP_NCK;				/* NO ACK 									*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;			/* クロック = 1clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	ストップコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STOP_SETUPTIME;		/* 0.3us setuptime							*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_HIGH;			/* SDAをHiに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;			/* クロック = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

	/* for debug */
//	pAsicHwReg->AREG_HPDO |= 0x01;						/* Write Protect 設定						*/
	/* for debug */

	*rdVal = data;

/*--------------------------------------------------------------------------------------------------*/
	return( EEPROM_I2C_SUCCESS );
/*--------------------------------------------------------------------------------------------------*/
}


/****************************************************************************************************
Description: Write 8bit data to tenmpareture sensor IC (S-5851AAA-M6T1)
			 温度センサIC 8bitデータ書き込み

Parameters:
	ASIC_HW_IF_STR *pAsicHwReg	: Pointer to the ASIC hardware register
	LONG 			device		: Device address (A2,A1,A0) of Temperature Sensor IC 
	ULONG 			eaddr		: Pointer-Register address of Temperature Sensor IC
	UCHAR 			data		: 8 bit data to be written
Return:
	Error
*****************************************************************************************************/
LONG TempICI2Cdrv_Write( ASIC_HW_IF_STR *pAsicHwReg, LONG device, ULONG eaddr, UCHAR data )
{

	VUSHORT			Dummy;
	ULONG			poll_cnt;		/* Time Stamp Unit [ms]				*/

	/* ローカル変数の初期化 */
	poll_cnt = 0;

	/* for debug */
//	pAsicHwReg->AREG_HPDO &= ~(0x01);					/* Write Protect 解除						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
/*	スタートコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STRT_HOLDTIME;	/* I2C利用、ホールド時間 1.0us				*//*?*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_LOW;		/* SDAをLowに設定							*//*?*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* クロック数 = 1clock 						*//*?*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ (なせwhile使うのかな??? by aba)*/
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}
	}

/*--------------------------------------------------------------------------------------------------*/
/*	スレーブアドレス送信																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_CLK_385K;		/* クロック周波数 385KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_TEMPIC_WR_DEV_ADR | (device << 1));	/* 1 0 0 1 A2 A1 A0 W : A2,A1,A0 = device	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* クロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACKが返ってくるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	ポインタレジスタ指定(送信)																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* アドレス書き込み */
	pAsicHwReg->AREG_I2CDATA = ( (USHORT)eaddr & 0x0001 );/* 0000 000*	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* クロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACKが返ってくるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	書込みデータ送信																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* データ書き込み */
	pAsicHwReg->AREG_I2CDATA = data;
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* クロック = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASICのステータスが通信完了になるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	  /* ACKが返ってくるまで待つ */
		KlibWaitus(I2C_WAIT_TICK);
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}


/*--------------------------------------------------------------------------------------------------*/
/*	ストップコンディション																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STOP_SETUPTIME;		/* セットアップ時間 0.3us					*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_HIGH;		/* SDAをHiに設定							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* クロック = 1clock					*/
/*--------------------------------------------------------------------------------------------------*/
	/* 書き込み時間分WAIT */
	KlibWaitms( 5 );

	/* for debug */
//	pAsicHwReg->AREG_HPDO |= 0x01;						/* Write Protect 設定						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/

	return EEPROM_I2C_SUCCESS;
}

