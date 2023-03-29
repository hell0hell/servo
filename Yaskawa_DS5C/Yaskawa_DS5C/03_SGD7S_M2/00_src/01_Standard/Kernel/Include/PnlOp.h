/*****************************************************************************
Description		: データトレース定義グローバルヘッダファイル
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.05.20		Rev1.0
Y.Oka		2013.12.19		Rev2.0

*****************************************************************************/
#ifndef _PNLOP_API_H
#define _PNLOP_API_H

#include "basedef.h"
#include "AlarmManager.h"



/* @@現状アナログ/パルス用処理未対応 2013.07.17 Y.Oka  */
/****************************************************************************************************/
/*																									*/
/*		パネオペ情報作成処理定義																	*/
/*																									*/
/****************************************************************************************************/
typedef enum _PNLOP_LED_STS_BITNO
{	/* Enum of Panel Op Led status Bit No */
	LED_POT_BNO		= 0,
	LED_NOT_BNO		= 1,
	LED_SVON_BNO	= 3,
	LED_TGON_BNO	= 7,
	LED_DEN_BNO		= 8,
	LED_FSTP_BNO	= 9,	/* <S1F5> */
	LED_HBB_BNO		= 10,
#if 0 /* Safety現状未対応 *//* <S0A1> */
#if ( CCSW_SFTID == SFTID_EXIST )
	LED_SFON_BNO	= 11,
	LED_SAFE_BNO	= 12,
	LED_ACT_BNO		= 13,
#endif
#endif
//	LED_CNCT_BNO	= 15,
}PNLOP_LED_STS_BITNO;

/* ↓<S0A1> */
#define STS_P_OT			(1 << LED_POT_BNO)
#define STS_N_OT			(1 << LED_NOT_BNO)
#define STS_TGON			(1 << LED_TGON_BNO)
#define STS_SV_ON			(1 << LED_SVON_BNO)
#define STS_CONNECT			(1 << LED_CNCT_BNO)
#define STS_DEN				(1 << LED_DEN_BNO)
#define STS_HBB				(1 << LED_HBB_BNO)
#define STS_FSTP			(1 <<LED_FSTP_BNO)  /* <S1F5> */
#if (CSW_NETIF_TYPE == NETIF_CM_TYPE)
	#define STS_P_LS		(1 << LED_PLS_BNO)
	#define STS_N_LS		(1 << LED_NLS_BNO)
	#define STS_OP_DISABLE	(1 << LED_OP_DISABLE_BNO)
#endif
#if 0 /* Safety現状未対応 *//* <S0A1> */
#if ( CCSW_SFTID == SFTID_EXIST )
	#define STS_SFON		(1 << LED_SFON_BNO)
	#define STS_SAFE		(1 << LED_SAFE_BNO)
	#define STS_ACT			(1 << LED_ACT_BNO)
#endif
#endif
/* ↑<S0A1> */


/*****************************************************************************
 表示モード定義
*****************************************************************************/
typedef enum _PNLOP_DISP_MODE {
	NORMAL_MODE	= 0,
	ALARM_MODE,
	OT_MODE,			/* <S0A1> */
	HBB_MODE,			/* <S0A1> */
	LS_MODE,			/* <S0A1> */
	BOOT_MODE,			/* <S0A1> */
	FSTP_MODE,			/* <S1F5> */
#if 0 /* Safety現状未対応 *//* <S0A1> */
#if ( CCSW_SFTID == SFTID_EXIST )
	SAFE_MODE,
	ACT_MODE,
#endif
#endif

}PNLOP_DISP_MODE;//<S300>


/*****************************************************************************
 7SegLED用表示定義
*****************************************************************************/
typedef struct _PNLOP_DATA_HANDLE
{	/* (M3) Structure of Panel Op LED Data Handle */
	struct {
		ULONG	SysTime;
		ULONG	SysTime2;
		BOOL	ConnectSts;
		USHORT	status;
		USHORT	lst_alm;
		UCHAR 	code;
		UCHAR	cnt;
		UCHAR	cnt_limt;
		UCHAR	enable;
		UCHAR	led[16];
		USHORT	timer[16];
		USHORT	mode;
		USHORT	alm_code;
	}var;
} PNLOP_DATA_HANDLE;



/*****************************************************************************
 Global Functions Prototype
*****************************************************************************/
void	PnlOpInitialize( PNLOP_DATA_HANDLE *hLedData );
BOOL	PnlOpDisplayLedData( PNLOP_DATA_HANDLE *hLedData, ALMMAN_HANDLE *hAlmManager );
UCHAR	PnlOpGetLedCode( PNLOP_DATA_HANDLE *hLedData );
void	PnlOpSetLedStatus( PNLOP_DATA_HANDLE *hLedData, USHORT ServoSts );
BOOL	PnlOpGetConnectStatus( PNLOP_DATA_HANDLE *hLedData );
void	PnlOpSetConnectStatus( PNLOP_DATA_HANDLE *hLedData, BOOL ConnectSts );

/*-------------------------------------------End Of File  -----------------------------------------------*/
#endif	/* _PNLOP_API_H */
