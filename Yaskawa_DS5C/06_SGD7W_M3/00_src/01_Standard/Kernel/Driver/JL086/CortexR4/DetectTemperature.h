#ifndef __DETECT_TEMPERATURE_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		DetectTemp.h : Temperature Detect IC Access Driver						<S03A>				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		�@ �\ : I2C/Temperature IC Device Driver for S-5851AAA-M6T1									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Author   : Yaskawa Electric Corp.,															*/
/*					Copyright (c) 2013 All Rights Reserved											*/
/*																									*/
/*		Project  : Mercury																			*/
/*																									*/
/*		Rev.1.00 : 2013.10.27  Y.Oka																*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"


/****************************************************************************************************/
/*																									*/
/*		DEFINES																						*/
/*																									*/
/****************************************************************************************************/
#define	DETECT_TEMP_TIMING		100			/* ���x���o����[ms]										*/
											/* ��-7�ł͉��xIC��2�_���邽�߁A����IC�ւ̃A�N�Z�X��	*/
											/* DETECT_TEMP_TIMING * 2[ms]�ƂȂ�B					*/
#define	TEMP_IC_NUM				2			/* ���x���oIC�̓_��										*/

enum TEMP_DEVICE_ID {						/* ���x���oIC�؂�ւ��pID								*/
	CTRL_BOARD_TEMPIC = 0,
	POWER_BOARD_TEMPIC,
};

/****************************************************************************************************/
/*																									*/
/*		STRUCT DEFINITION																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		���x�Ď��p��`																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _TEMP_MANAGER {
	struct {								/* ���x�Ď��p�ϐ���`									*/
		SHORT		CtrlBoardTemp;				/* �������݉��x[��]								*/
		SHORT		PowerBoardTemp;				/* �p���[����݉��x[��]							*/
		SHORT		CtrlBoardTempMax;			/* �����ō����x[��]								*/
		SHORT		PowerBoardTempMax;			/* �p���[��ō����x[��]							*/
		USHORT		CtrlBoardI2CErrCnt;			/* �����ʐM�G���[�A����						*//* <S0DE> */
		USHORT		PowerBoardI2CErrCnt;		/* �p���[��ʐM�G���[�A����						*//* <S0DE> */
	/*----------------------------------------------------------------------------------------------*/
		USHORT		DeviceID;					/* ������IC/�p���[���IC							*/
		ULONG		DetTempTimer;				/* ���x���o�^�C�~���O�p�^�C�}						*/
	} var;
} TEMP_MANAGER;



#if	(CSW_DETECT_TEMPERATURE == TRUE)
/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
LONG	DetTempInitialize( TEMP_MANAGER *TempManager );
LONG	DetTempManager( TEMP_MANAGER *TempManager );

/****************************************************************************************************/
/*																									*/
/*		API Function																				*/
/*																									*/
/****************************************************************************************************/
SHORT DetTempGetInternalTemperature( TEMP_MANAGER *TempManager );
SHORT DetTempGetCtrlBoardTemperature( TEMP_MANAGER *TempManager );
SHORT DetTempGetPowerBoardTemperature( TEMP_MANAGER *TempManager );
USHORT DetTempGetCtrlBoardErrorCount( TEMP_MANAGER *TempManager );									  /* <S0DE> */
USHORT DetTempGetPowerBoardErrorCount( TEMP_MANAGER *TempManager );									  /* <S0DE> */


#else /* (CSW_DETECT_TEMPERATURE == TRUE) */

LONG	DetTempInitialize( TEMP_MANAGER *TempManager ){ };
LONG	DetTempManager( TEMP_MANAGER *TempManager ){ };
SHORT DetTempGetInternalTemperature( TEMP_MANAGER *TempManager ){ return 0 };
SHORT DetTempGetCtrlBoardTemperature( TEMP_MANAGER *TempManager ){ return 0 };
SHORT DetTempGetPowerBoardTemperature( TEMP_MANAGER *TempManager ){ return 0 };
USHORT DetTempGetCtrlBoardErrorCount( TEMP_MANAGER *TempManager ){ return 0 };						  /* <S0DE> */
USHORT DetTempGetPowerBoardErrorCount( TEMP_MANAGER *TempManager ){ return 0 };						  /* <S0DE> */
#endif /* (CSW_DETECT_TEMPERATURE == TRUE) */



#define __DETECT_TEMPERATURE_
#endif /* __DETECT_TEMPERATURE_ */
/***************************************** end of file **********************************************/
