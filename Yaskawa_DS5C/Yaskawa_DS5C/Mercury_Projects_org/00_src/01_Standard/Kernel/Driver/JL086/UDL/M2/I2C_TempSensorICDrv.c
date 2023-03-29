/****************************************************************************************************
Description	:	I/F Definision by I2C for Temperature Sensor IC(S-5851AAA-M6T1)	<TempSensorIC>
				I2C�p���x�Z���TIC I/F��`
-----------------------------------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	TempICI2Cdrv_Initialize()		: Initialize the temperature sensor IC
	TempICI2Cdrv_Read_Temperature()	: Store temperature data in buffer
	TempICI2Cdrv_Read()				: Read 16bit data from temperature sensor IC (S-5851AAA-M6T1)
	TempICI2Cdrv_Write()			: Write 8bit data to tenmpareture sensor IC (S-5851AAA-M6T1)

���x�Z���TIC�ǂݏo���^�������݃f�[�^ ( S-5851AAA-M6T1 )

1) To send Device address
		   7  6  5  4  3  2  1  0
-------------------------------------
I2CDATA	   1  0  0  1 A2 A1 A0 R/W
EPRCNT	   0x00DC (9clock)
-------------------------------------
R/W  : 0 = Write, 1 = Read
IC�@ : A2�`0 = 010
IC�A : A2�`0 = 111

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



extern USHORT		DebugBuf1[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf2[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf3[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern USHORT		DebugBuf4[100];		/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern INT			DebugCount;				/* �f�o�b�O�p											 *//* <TempSensorIC> */
extern INT 			Debug_i;				/* wait�p												 *//* <TempSensorIC> */
extern INT 			Debug_j;				/* 														 *//* <TempSensorIC> */



/****************************************************************************************************
Description: Initialize the temperature sensor IC (S-5851AAA-M6T1)
			���x�Z���TIC ������

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

	/* �m�F��rslt�̃`�F�b�N�����ׂ������ǂƂ肠�����Ȃ��ŁB*/
	/* �m�F��read�����ׂ������ǂƂ肠�����Ȃ��ŁB*/

}

/****************************************************************************************************
Description:  Store temperature data in buffer
			���x�Z���TIC ���x���W�X�^�ǂݏo��

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
		�f�o�b�O�̂��߁A�o�b�t�@�Ɋi�[����
----------------------------------------------------------------------------------------------------*/
	if( Debug_j < 500 )	/* 2013.05.28<tanaka21> */
	{	/* 2ms * 500 = 1sec ���Ɉ��ǂ�*/
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
		DebugBuf3[ (UINT)(DebugCount) ] = data1 >> 8;	/* ����4bit�������P�ʊ��Z [deg]*/
		DebugBuf4[ (UINT)(DebugCount) ] = data2 >> 8;	/* ����4bit�������P�ʊ��Z [deg]*/
		DebugCount++;
		if( DebugCount == 100 )	/* 2013.05.28<tanaka21> */
		{
			DebugCount = 0;
		}
		Debug_j = 0;
	}
	
	/* �m�F��rslt�̃`�F�b�N�����ׂ������ǂƂ肠�����Ȃ��ŁB*/

}

/****************************************************************************************************
Description: Read 16bit data from temperature sensor IC (S-5851AAA-M6T1)
			���x�Z���TIC ���[�h(16bit)�f�[�^�ǂݏo��

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

	/* ���[�J���ϐ��̏����� */
	poll_cnt = 0;

	/* for debug */
//	pAsicHwReg->AREG_HPDO &= ~(0x01);					/* Write Protect ����						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
/*	�X�^�[�g�R���f�B�V����																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STRT_HOLDTIME;	/* I2C���p�A�z�[���h���� 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_LOW;		/* SDA��Low�ɐݒ�							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* �N���b�N = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�X���[�u�A�h���X���M																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* �_�~�[���C�g */
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_CLK_385K;		/* �N���b�N���g�� 385KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_TEMPIC_WR_DEV_ADR | (device << 1));	/* 1 0 0 1 A2 A1 A0 W : A2,A1,A0 = device	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;			/* �N���b�N = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACK���Ԃ��Ă���܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�|�C���^���W�X�^�w��(���M)																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �A�h���X�������� */
	pAsicHwReg->AREG_I2CDATA = ( (USHORT)eaddr & 0x0001 );/* 0000 000*	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* �N���b�N = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACK���Ԃ��Ă���܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	
/*--------------------------------------------------------------------------------------------------*/
/*	�X�^�[�g�R���f�B�V����																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STRT_HOLDTIME;	/* I2C���p�A�z�[���h���� 1.0us				*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_LOW;		/* SDA��Low�ɐݒ�							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* �N���b�N = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* Wait */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	
/*--------------------------------------------------------------------------------------------------*/
/*	�X���[�u�A�h���X���M																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_CLK_385K;		/* �N���b�N���g�� 385KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_TEMPIC_RD_DEV_ADR | (device << 1));	/* 1 0 0 1 A2 A1 A0 R : A2,A1,A0 = device	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;			/* �N���b�N = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACK���Ԃ��Ă���܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�Ǐo���f�[�^��M																				*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_I2CDATA = KPX_E0_TERMINAL_HIGH_Z;	/* �O���[�qE0��Hi-Z��Ԃɂ���				*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_8CLK;	/* �N���b�N = 8clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
/*--------------------------------------------------------------------------------------------------*/
	data = (pAsicHwReg->AREG_I2CDI << 8);

//	if( size == EEPROM_2UNIT )
	{ /* 2�o�C�g�Ǐo���v���̏ꍇ */
	/*----------------------------------------------------------------------------------------------*/
	/*	�A�N�m���b�W���M																			*/
	/*----------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_I2CDATA = KPX_EEP_ACK;			/* ACK										*/
		pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;		/* �N���b�N = 1clock 					*/
		Dummy = pAsicHwReg->AREG_EPRSET;				/* Dummy Read								*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
			KlibWaitus(I2C_WAIT_TICK);	
			++poll_cnt;
			if(poll_cnt >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}

	/*----------------------------------------------------------------------------------------------*/
	/*	�Ǐo���f�[�^��M																			*/
	/*----------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_I2CDATA = KPX_E0_TERMINAL_HIGH_Z;	/* �O���[�qE0��Hi-Z��Ԃɂ���			*/
		pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_8CLK;	/* �N���b�N = 8clock				*/
		Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read							*/
		while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
		{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
			KlibWaitus(I2C_WAIT_TICK);	
			++poll_cnt;
			if(poll_cnt >= I2C_ACCESS_TIMEOUT)
			{	return EEPROM_I2C_TIMEOUT_ERR;}		
		}
	/*----------------------------------------------------------------------------------------------*/
		data |= pAsicHwReg->AREG_I2CDI;
	}
/*--------------------------------------------------------------------------------------------------*/
/*	No ACK���M																						*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_I2CDATA = KPX_EEP_NCK;				/* NO ACK 									*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;			/* �N���b�N = 1clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�X�g�b�v�R���f�B�V����																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STOP_SETUPTIME;		/* 0.3us setuptime							*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_HIGH;			/* SDA��Hi�ɐݒ�							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;			/* �N���b�N = 1clock 					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

	/* for debug */
//	pAsicHwReg->AREG_HPDO |= 0x01;						/* Write Protect �ݒ�						*/
	/* for debug */

	*rdVal = data;

/*--------------------------------------------------------------------------------------------------*/
	return( EEPROM_I2C_SUCCESS );
/*--------------------------------------------------------------------------------------------------*/
}


/****************************************************************************************************
Description: Write 8bit data to tenmpareture sensor IC (S-5851AAA-M6T1)
			 ���x�Z���TIC 8bit�f�[�^��������

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

	/* ���[�J���ϐ��̏����� */
	poll_cnt = 0;

	/* for debug */
//	pAsicHwReg->AREG_HPDO &= ~(0x01);					/* Write Protect ����						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/
/*	�X�^�[�g�R���f�B�V����																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STRT_HOLDTIME;	/* I2C���p�A�z�[���h���� 1.0us				*//*?*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_LOW;		/* SDA��Low�ɐݒ�							*//*?*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* �N���b�N�� = 1clock 						*//*?*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� (�Ȃ�while�g���̂���??? by aba)*/
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�X���[�u�A�h���X���M																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_CLK_385K;		/* �N���b�N���g�� 385KHz 				*/
	pAsicHwReg->AREG_I2CDATA = (USHORT)(KPX_TEMPIC_WR_DEV_ADR | (device << 1));	/* 1 0 0 1 A2 A1 A0 W : A2,A1,A0 = device	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* �N���b�N = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACK���Ԃ��Ă���܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�|�C���^���W�X�^�w��(���M)																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �A�h���X�������� */
	pAsicHwReg->AREG_I2CDATA = ( (USHORT)eaddr & 0x0001 );/* 0000 000*	*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* �N���b�N = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	 /* ACK���Ԃ��Ă���܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}

/*--------------------------------------------------------------------------------------------------*/
/*	�����݃f�[�^���M																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* �f�[�^�������� */
	pAsicHwReg->AREG_I2CDATA = data;
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_9CLK;	/* �N���b�N = 9clock					*/
	Dummy = pAsicHwReg->AREG_EPRSET;					/* Dummy Read								*/
	while( (pAsicHwReg->AREG_EPRSET & 0x8000) == 0 )
	{	 /* ASIC�̃X�e�[�^�X���ʐM�����ɂȂ�܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);	
		if((++poll_cnt) >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}
	while( (pAsicHwReg->AREG_I2CDI & 0x0001) == 1 )
	{	  /* ACK���Ԃ��Ă���܂ő҂� */
		KlibWaitus(I2C_WAIT_TICK);
		++poll_cnt;
		if(poll_cnt >= I2C_ACCESS_TIMEOUT)
		{	return EEPROM_I2C_TIMEOUT_ERR;}		
	}


/*--------------------------------------------------------------------------------------------------*/
/*	�X�g�b�v�R���f�B�V����																			*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_EPRSET = KPX_TEMPIC_STOP_SETUPTIME;		/* �Z�b�g�A�b�v���� 0.3us					*/
	pAsicHwReg->AREG_I2CDATA = KPX_TEMPIC_SDA_HIGH;		/* SDA��Hi�ɐݒ�							*/
	pAsicHwReg->AREG_EPRCNT = KPX_TEMPIC_CLKCNT_1CLK;	/* �N���b�N = 1clock					*/
/*--------------------------------------------------------------------------------------------------*/
	/* �������ݎ��ԕ�WAIT */
	KlibWaitms( 5 );

	/* for debug */
//	pAsicHwReg->AREG_HPDO |= 0x01;						/* Write Protect �ݒ�						*/
	/* for debug */

/*--------------------------------------------------------------------------------------------------*/

	return EEPROM_I2C_SUCCESS;
}

