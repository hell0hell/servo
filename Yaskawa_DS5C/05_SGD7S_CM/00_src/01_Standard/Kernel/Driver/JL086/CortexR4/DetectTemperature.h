#ifndef __DETECT_TEMPERATURE_
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		DetectTemp.h : Temperature Detect IC Access Driver						<S03A>				*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		機 能 : I2C/Temperature IC Device Driver for S-5851AAA-M6T1									*/
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
#define	DETECT_TEMP_TIMING		100			/* 温度検出周期[ms]										*/
											/* Σ-7では温度ICが2点あるため、同一ICへのアクセスは	*/
											/* DETECT_TEMP_TIMING * 2[ms]となる。					*/
#define	TEMP_IC_NUM				2			/* 温度検出ICの点数										*/

enum TEMP_DEVICE_ID {						/* 温度検出IC切り替え用ID								*/
	CTRL_BOARD_TEMPIC = 0,
	POWER_BOARD_TEMPIC,
};

/****************************************************************************************************/
/*																									*/
/*		STRUCT DEFINITION																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		温度監視用定義																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _TEMP_MANAGER {
	struct {								/* 温度監視用変数定義									*/
		SHORT		CtrlBoardTemp;				/* 制御基板現在温度[℃]								*/
		SHORT		PowerBoardTemp;				/* パワー基板現在温度[℃]							*/
		SHORT		CtrlBoardTempMax;			/* 制御基板最高温度[℃]								*/
		SHORT		PowerBoardTempMax;			/* パワー基板最高温度[℃]							*/
		USHORT		CtrlBoardI2CErrCnt;			/* 制御基板通信エラー連続回数						*//* <S0DE> */
		USHORT		PowerBoardI2CErrCnt;		/* パワー基板通信エラー連続回数						*//* <S0DE> */
	/*----------------------------------------------------------------------------------------------*/
		USHORT		DeviceID;					/* 制御基板IC/パワー基板IC							*/
		ULONG		DetTempTimer;				/* 温度検出タイミング用タイマ						*/
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
