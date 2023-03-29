/****************************************************************************************************/
/*																									*/
/*																									*/
/*		EepromIf.h : I2CópEEPROM I/FíËã`															*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	èâî≈	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef EEPROM_IF_H

#include "Basedef.h"
#include "JL086ServoIpReg.h"
#include "KLib.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define		EEPROM_2UNIT				1
#define		EEPROM_I2C_TIMEOUT_ERR		-124
#define		EEPROM_I2C_SUCCESS			0

#define		I2C_WAIT_TICK				100		// [us]
#define		I2C_ACCESS_TIMEOUT			200		// timeout[us] =I2C_WAIT_TICK x I2C_POLL_TIMEOUT

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
LONG EepI2Cdrv_ReadEepromWord(ASIC_HW_IF_STR *pAsicHwReg, LONG axis, ULONG eaddr, USHORT *rdVal);
LONG EepI2Cdrv_WriteEepromWord( ASIC_HW_IF_STR *pAsicHwReg, LONG axis, ULONG eaddr, USHORT wdata );
//void	WaitEepromReadComp( void );
//void	WaitEepromWriteComp( LONG axis );



#define EEPROM_IF_H
#endif /* EEPROM_IF_H */
/***************************************** end of file **********************************************/
