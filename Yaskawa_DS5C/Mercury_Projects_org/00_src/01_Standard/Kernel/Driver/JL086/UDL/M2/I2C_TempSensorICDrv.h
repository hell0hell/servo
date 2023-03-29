/****************************************************************************************************
Description	:	I/F Definision by I2C for Temperature Sensor IC(S-5851AAA-M6T1)
				I2C用温度センサIC I/F定義
-----------------------------------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :

-----------------------------------------------------------------------------------------------------
Changes		:
Name			Date			Description
-----------------------------------------------------------------------------------------------------
A.Hashimoto		2013.5.20		created

*****************************************************************************************************/
#ifndef I2C_TEMPSENSORICDRV_H

#include "Basedef.h"
#include "JL086ServoIpReg.h"
#include "KLib.h"

/****************************************************************************************************
		DEFINES
*****************************************************************************************************/
//#define		EEPROM_2UNIT				1
#define		EEPROM_I2C_TIMEOUT_ERR		-124
#define		EEPROM_I2C_SUCCESS			0

#define		I2C_WAIT_TICK				1		// [us]
#define		I2C_ACCESS_TIMEOUT			200		// timeout[us] =I2C_WAIT_TICK x I2C_POLL_TIMEOUT

#define	KPX_TEMPIC_STRT_HOLDTIME		0x0868			/* 0.6us+400ns=1us							*//* ?*/
#define	KPX_TEMPIC_STOP_SETUPTIME		0x08A0			/* 0.6us-300ns=0.3us						*//* ?*/
#define	KPX_TEMPIC_CLK_278K				0x1AB8			/* clock freqency 278KHz (3.6us)	スタート/ストップ時		*/
#define	KPX_TEMPIC_CLK_385K				0x1ACC			/* clock freqency 385KHz (2.6us)	データ通信時			*/
#define	KPX_TEMPIC_CLK_400K				0x1ACE			/* clock freqency 400KHz (2.5us)	MAX						*/
#define	KPX_TEMPIC_CLKCNT_HALFCLK		0x00FE			/* EPRCNT:0.5clk							*/
#define	KPX_TEMPIC_CLKCNT_1CLK			0x00FC			/* EPRCNT:1clk								*/
#define	KPX_TEMPIC_CLKCNT_8CLK			0x00E0			/* EPRCNT:8clk								*/
#define	KPX_TEMPIC_CLKCNT_9CLK			0x00DC			/* EPRCNT:9clk								*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_TEMPIC_WR_DEV_ADR			0x0090			/* 1  0  0  1 A2 A1 A0 W : Write=0,A=0		*/
#define	KPX_TEMPIC_RD_DEV_ADR			0x0091			/* 1  0  0  1 A2 A1 A0 R : Read=1,A=0		*/
#define	KPX_TEMPIC_SDA_LOW				0x0000			/* MSB = 0									*/
#define	KPX_TEMPIC_SDA_HIGH				0x0080			/* MSB = 1									*/
#define	KPX_TEMPIC_ADR1					0x0002			/* A2 A1 A0 : IC1 = 010 					*/
#define	KPX_TEMPIC_ADR2					0x0007			/* A2 A1 A0 : IC2 = 111 					*/
#define	KPX_TEMPIC_REG_TEMP				0x0000			/* Temperature-Register (Read only)			*/
#define	KPX_TEMPIC_REG_CONFIG			0x0001			/* Configration-Register (R/W)				*/
#define	KPX_E0_TERMINAL_HIGH_Z			0x00FF			/* ?										*/
#define	KPX_EEP_ACK						0x0000			/* MSB = 0, 読み出し時のACK送信用			*/
#define	KPX_EEP_NCK						0x0080			/* MSB = 1, 読み出し時のNCK送信用			*/



/****************************************************************************************************
		PROTOTYPE
*****************************************************************************************************/
/*: Initialize the temperature sensor IC*/
void TempICI2Cdrv_Initialize(ASIC_HW_IF_STR *pAsicHwReg);

/* Store temperature data in buffer*/
void TempICI2Cdrv_Read_Temperature(ASIC_HW_IF_STR *pAsicHwReg);

/* Read 16bit data from temperature sensor IC (S-5851AAA-M6T1)*/
LONG TempICI2Cdrv_Read(ASIC_HW_IF_STR *pAsicHwReg, LONG device, ULONG eaddr, USHORT *rdVal);

/* Write 8bit data to tenmpareture sensor IC (S-5851AAA-M6T1)*/
LONG TempICI2Cdrv_Write( ASIC_HW_IF_STR *pAsicHwReg, LONG device, ULONG eaddr, UCHAR data );



#define I2C_TEMPSENSORICDRV_H
#endif /* I2C_TEMPSENSORICDRV_H */
/***************************************** end of file **********************************************/
