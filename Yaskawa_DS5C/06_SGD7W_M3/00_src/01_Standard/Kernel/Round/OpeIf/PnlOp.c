/*****************************************************************************
Description		: パネルオペレータ処理モジュール
-----------------------------------------------------------------------------
Author		: SHCORE
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
	PnlOpDisplayLedData();	  			:7segLED表示データセット
	PnlOpInitialize();					:Panel Op 関連初期化
	LedChkTest();						:7segLED表示確認試験処理	<S0CB>

	-- Locals --
	PnlOpSetLedMode();					:7segLED 状態セット

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.05.20		Rev1.0
Y.Oka		2013.12.19		Rev2.0

*****************************************************************************/
#include "PnlOp.h"
#include "KLib.h"
#include "HwDeviceIf.h"

/****************************************************************************************************/
/*																									*/
/*		LEDオペレータ表示Bit定義																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Panel Operator LED Codes																			*/
/*																									*/
/* (digit location)        (7seg location)															*/
/*    bit:0																							*/
/*       --					_																		*/
/*    5 |  | 1             |_|																		*/
/*       -- 6              |_|																		*/
/*    4 |  | 2      led[?]: 0																		*/
/*       -- * 7																						*/
/*       3  logic			0:ON / 1:OFF															*/
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_B0		0x01
#define	LEDCODE_B1		0x02
#define	LEDCODE_B2		0x04
#define	LEDCODE_B3		0x08
#define	LEDCODE_B4		0x10
#define	LEDCODE_B5		0x20
#define	LEDCODE_B6		0x40
#define	LEDCODE_B7		0x80

/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_ZERO	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_1		LEDCODE_B1|LEDCODE_B2
#define	LEDCODE_2		LEDCODE_B0|LEDCODE_B1|LEDCODE_B3|LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_3		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B6
#define	LEDCODE_4		LEDCODE_B1|LEDCODE_B2|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_5		LEDCODE_B0|LEDCODE_B2|LEDCODE_B3|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_6		LEDCODE_B0|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_7		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B5
#define	LEDCODE_8		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_9		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_A		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_B		LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_C		LEDCODE_B0|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_D		LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_E		LEDCODE_B0|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_F		LEDCODE_B0|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_G		LEDCODE_B0|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_H		LEDCODE_B1|LEDCODE_B2|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_sH		LEDCODE_B2|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_J		LEDCODE_B1|LEDCODE_B2|LEDCODE_B3
#define	LEDCODE_L		LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_N		LEDCODE_B2|LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_sO		LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_P		LEDCODE_B0|LEDCODE_B1|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_Q		LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_R		LEDCODE_B4|LEDCODE_B6
#define	LEDCODE_S		LEDCODE_B0|LEDCODE_B2|LEDCODE_B3|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_T		LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_U		LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5
#define	LEDCODE_sU		LEDCODE_B2|LEDCODE_B3|LEDCODE_B4
#define	LEDCODE_X		LEDCODE_B1|LEDCODE_B2|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6
#define	LEDCODE_Y		LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B5|LEDCODE_B6
/*--------------------------------------------------------------------------------------------------*/
#define	LEDCODE_A_DOT	LEDCODE_A|LEDCODE_B7
#define	LEDCODE_B_DOT	LEDCODE_B|LEDCODE_B7
#define	LEDCODE_T_DOT	LEDCODE_T|LEDCODE_B7
#define	LEDCODE_MINUS	LEDCODE_B6
#define	LEDCODE_POINT	LEDCODE_B7
#define	LEDCODE_BLANK	0x00U



const UCHAR ledcode[] =
{ /*    0,            1,            2,            3,           4,             5     */
	LEDCODE_ZERO, LEDCODE_1,    LEDCODE_2,    LEDCODE_3,    LEDCODE_4,    LEDCODE_5,
  /*    6,            7             8,            9,            A,            b     */
	LEDCODE_6,    LEDCODE_7,    LEDCODE_8,    LEDCODE_9,    LEDCODE_A,    LEDCODE_B, 
  /*    C,            d,            E,            F                                 */
	LEDCODE_C,    LEDCODE_D,    LEDCODE_E,    LEDCODE_F };

/* <S0CB> Start */
/* プッシュボタン・7LED-LED確認モード用 7LED表示定義 */
#define	PATTERN0	LEDCODE_BLANK																			/* 全LED消灯		*/
#define	PATTERN1	LEDCODE_B0																				/* Bit0 点灯		*/
#define	PATTERN2	LEDCODE_B0|LEDCODE_B1																	/* Bit0 - Bit1 点灯	*/
#define	PATTERN3	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2														/* Bit0 - Bit2 点灯	*/
#define	PATTERN4	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3												/* Bit0 - Bit3 点灯	*/
#define	PATTERN5	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4									/* Bit0 - Bit4 点灯	*/
#define	PATTERN6	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5						/* Bit0 - Bit5 点灯	*/
#define	PATTERN7	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6			/* Bit0 - Bit6 点灯	*/
#define	PATTERN8	LEDCODE_B0|LEDCODE_B1|LEDCODE_B2|LEDCODE_B3|LEDCODE_B4|LEDCODE_B5|LEDCODE_B6|LEDCODE_B7	/* Bit0 - Bit7 点灯	*/

const UCHAR LedLightPatternTbl[] =
{	/*	0,			1,			2,			3,			*/
		PATTERN1,	PATTERN2,	PATTERN3,	PATTERN4,
	/*	4,			5,			6,			7			*/
		PATTERN5,	PATTERN6,	PATTERN7,	PATTERN8,

	/*	8,			9,			10,			11,			*/
		PATTERN7,	PATTERN6,	PATTERN5,	PATTERN4,
	/*	12,			13,			14,			15			*/
		PATTERN3,	PATTERN2,	PATTERN1,	PATTERN0
};
/* <S0CB> End */

/****************************************************************************************************/
/*		static variables															    			*/
/****************************************************************************************************/
//static PNLOP_LED_DATA_HANDLE     PnlOpLedDataHndl;		/* ネットワーク用パネオペ構造体			*//* <S0A1> */


/****************************************************************************************************/
/*		PROTOTYPE																	    			*/
/****************************************************************************************************/
/* APIs */
/* Panel Op 関連初期化 */
//void PnlOpInitialize( PNLOP_DATA_HANDLE *hLedData );
/* 7segLED表示データセット */
//BOOL PnlOpDisplayLedData( PNLOP_DATA_HANDLE *hLedData, ALMMAN_HANDLE  *hAlmManager );
/* 7segLED 表示コード取得用API */
//UCHAR PnlOpGetLedCode( PNLOP_DATA_HANDLE *hLedData );
/* 7egLED 状態表示情報設定用API */
//void PnlOpSetLedStatus( PNLOP_DATA_HANDLE *hLedData, UCHAR ServoSts )


/* Locals */
/* 7segLED 状態セット */
static UCHAR PnlOpSetLedMode( PNLOP_DATA_HANDLE *hLedData );
static UCHAR PnlOpSetStsBits( PNLOP_DATA_HANDLE *hLedData );


/*****************************************************************************
  Description: Panel Op 関連初期化
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void PnlOpInitialize( PNLOP_DATA_HANDLE *hLedData )
{
	LONG i;
	UCHAR* pBuf;

	pBuf = (UCHAR*)hLedData;

	for(i = 0L; i < (LONG)(sizeof(PNLOP_DATA_HANDLE)); i++)
	{
		*(pBuf+i) = 0U;
	}

	KlibRstLongTimer(&hLedData->var.SysTime);
	KlibRstLongTimer(&hLedData->var.SysTime2);

	hLedData->var.status = (USHORT)(STS_DEN);
	hLedData->var.cnt = 0U;
	hLedData->var.cnt_limt = 0U;
	hLedData->var.timer[0] = 10U;

	return;
}

/*****************************************************************************
  Description: 7segLED表示データセット
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
	hAlmManager		アラーム情報
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
BOOL PnlOpDisplayLedData( PNLOP_DATA_HANDLE *hLedData, ALMMAN_HANDLE *hAlmManager )
{
	BOOL	ret;
	ret = FALSE;

	hLedData->var.alm_code = (USHORT)ALMGetAlarmInfo(hAlmManager);

	if(KlibGetLongTimerMs(hLedData->var.SysTime) > hLedData->var.timer[hLedData->var.cnt])
	{
		KlibRstLongTimer(&hLedData->var.SysTime);
		KlibRstLongTimer(&hLedData->var.SysTime2);

		/* 状態セット */
		hLedData->var.led[0] = PnlOpSetStsBits( hLedData );
		hLedData->var.mode = PnlOpSetLedMode( hLedData );

		hLedData->var.cnt++;
		if(hLedData->var.cnt > hLedData->var.cnt_limt)
		{
			/* カウンタリセット */
			hLedData->var.cnt = 0U;
		}
		hLedData->var.code = ~hLedData->var.led[hLedData->var.cnt];

		/* 表示更新 */
//		HALdrv_Net7SegLedControl( code ); /* <S0A1> */
		ret = TRUE;
	}
	else
	{
		if((hLedData->var.cnt == 0U) && (KlibGetLongTimerMs(hLedData->var.SysTime2) > 20U))
		{
			KlibRstLongTimer(&hLedData->var.SysTime2);

			/* 状態セット */
			hLedData->var.led[0] = PnlOpSetStsBits( hLedData );
			hLedData->var.code = ~hLedData->var.led[0];

			/* 表示更新 */
//			HALdrv_Net7SegLedControl( code ); /* <S0A1> */
			ret = TRUE;
		}
	}

	return	ret;
}

/*****************************************************************************
  Description: 7segLED 状態セット
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
  Return:
	UCHAR			: Servo Mode
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static UCHAR PnlOpSetLedMode( PNLOP_DATA_HANDLE *hLedData )
{
	UCHAR led[2];
	UCHAR cnt = 0;

	/* アラーム状態 */
	if(hLedData->var.alm_code != 0U) /* alarm */
	{
		if(hLedData->var.alm_code != hLedData->var.lst_alm)
		{
			hLedData->var.timer[0] = 700U;
			hLedData->var.timer[1] = 200U; hLedData->var.led[1] = LEDCODE_BLANK;
			hLedData->var.timer[2] = 350U; hLedData->var.led[2] = LEDCODE_A_DOT;
			hLedData->var.timer[3] = 100U; hLedData->var.led[3] = LEDCODE_BLANK;
			hLedData->var.timer[4] = 350U; hLedData->var.led[4] = ledcode[(hLedData->var.alm_code >> 8U) & 0xFU];
			hLedData->var.timer[5] = 100U; hLedData->var.led[5] = LEDCODE_BLANK;
			hLedData->var.timer[6] = 350U; hLedData->var.led[6] = ledcode[(hLedData->var.alm_code >> 4U) & 0xFU];
			hLedData->var.timer[7] = 100U; hLedData->var.led[7] = LEDCODE_BLANK;
			hLedData->var.timer[8] = 350U; hLedData->var.led[8] = ledcode[hLedData->var.alm_code & 0xFU];
			hLedData->var.timer[9] = 100U; hLedData->var.led[9] = LEDCODE_BLANK;

			hLedData->var.cnt = 0U;
			hLedData->var.cnt_limt = 9U;
			hLedData->var.lst_alm = hLedData->var.alm_code;
		}
		return	ALARM_MODE;
	}
	hLedData->var.lst_alm = hLedData->var.alm_code; /* <S0A4> */

/* ↓<S0A1> */

	/* HBB状態 */
	if(hLedData->var.status & STS_HBB)
	{
		hLedData->var.timer[0] = 700U;
		hLedData->var.timer[1] = 200U; hLedData->var.led[1] = LEDCODE_BLANK;
		hLedData->var.timer[2] = 250U; hLedData->var.led[2] = LEDCODE_H;
		hLedData->var.timer[3] = 100U; hLedData->var.led[3] = LEDCODE_BLANK;
		hLedData->var.timer[4] = 250U; hLedData->var.led[4] = LEDCODE_B;
		hLedData->var.timer[5] = 100U; hLedData->var.led[5] = LEDCODE_BLANK;
		hLedData->var.timer[6] = 250U; hLedData->var.led[6] = LEDCODE_B_DOT;
		hLedData->var.timer[7] = 100U; hLedData->var.led[7] = LEDCODE_BLANK;
		hLedData->var.cnt_limt = 7;
		return	HBB_MODE;
	}
	
		/* <S1F5> Start */
	if( hLedData->var.status & STS_FSTP )		 /* FSTP */
	{
		hLedData->var.timer[0] = 700U;
		hLedData->var.timer[1] = 200U;hLedData->var.led[1] = LEDCODE_BLANK;
		hLedData->var.timer[2] = 350U;hLedData->var.led[2] = LEDCODE_F;
		hLedData->var.timer[3] = 100U;hLedData->var.led[3] = LEDCODE_BLANK;
		hLedData->var.timer[4] = 350U;hLedData->var.led[4] = LEDCODE_S;
		hLedData->var.timer[5] = 100U;hLedData->var.led[5] = LEDCODE_BLANK;
		hLedData->var.timer[6] = 350U;hLedData->var.led[6] = LEDCODE_T;
		hLedData->var.timer[7] = 100U;hLedData->var.led[7] = LEDCODE_BLANK;
		hLedData->var.timer[8] = 350U;hLedData->var.led[8] = LEDCODE_P;
		hLedData->var.timer[9] = 100U;hLedData->var.led[9] = LEDCODE_BLANK;
		hLedData->var.cnt_limt = 9U;
		return FSTP_MODE;
	}
	/* <S1F5> End */

#if 0 /* Safety現状未対応 *//* <S0A1> */
#if ( CCSW_SFTID == SFTID_EXIST )											/*<V889>Add:Start*/
	/* ACT状態 */
	if(hLedData->var.status & STS_ACT)
	{
		hLedData->var.timer[0] = 700U;
		hLedData->var.timer[1] = 200U; hLedData->var.led[1] = LEDCODE_BLANK;
		hLedData->var.timer[2] = 250U; hLedData->var.led[2] = LEDCODE_A;
		hLedData->var.timer[3] = 100U; hLedData->var.led[3] = LEDCODE_BLANK;
		hLedData->var.timer[4] = 250U; hLedData->var.led[4] = LEDCODE_C;
		hLedData->var.timer[5] = 100U; hLedData->var.led[5] = LEDCODE_BLANK;
		hLedData->var.timer[6] = 250U; hLedData->var.led[6] = LEDCODE_T;
		hLedData->var.timer[7] = 100U; hLedData->var.led[7] = LEDCODE_BLANK;
		hLedData->var.cnt_limt = 7;
		return	ACT_MODE;
	}
#endif																		/*<V889>Add:End*/
#endif

	/* OT状態 */
	if(hLedData->var.status & STS_P_OT)	/* P-OT */
		led[cnt++] = LEDCODE_P;
	if(hLedData->var.status & STS_N_OT)	/* N-OT */
		led[cnt++] = LEDCODE_N;
	if(cnt > 0)
	{
		hLedData->var.timer[0] = 500U;
		hLedData->var.timer[1] = 250U; hLedData->var.led[1] = led[0];
		hLedData->var.timer[2] = 250U; hLedData->var.led[2] = led[1];
		hLedData->var.cnt_limt = cnt;
		return	OT_MODE;
	}

#if 0 /* Safety現状未対応 *//* <S0A1> */
#if ( CCSW_SFTID == SFTID_EXIST )											/*<V889>Add:Start*/
	/* SFON状態 */
	if(!(hLedData->var.status & STS_SAFE) && (hLedData->var.status & STS_SFON))
	{
		hLedData->var.timer[0] = 250U;
		hLedData->var.timer[1] = 250U; hLedData->var.led[1] = hLedData->var.led[0] | 0x20;
		hLedData->var.cnt_limt = 1;
		return SAFE_MODE;
	}
#endif
#endif

/* ↑<S0A1> */

	/* 通常運転状態 */
	hLedData->var.cnt = 0U;
	hLedData->var.cnt_limt = 0U;
	hLedData->var.timer[0] = 10U;
	return	NORMAL_MODE;

}



/****************************************************************************/
/*																			*/
/*		PnlOpSetStsBits() : 7segLED 状態セット								*/
/*																			*/
/****************************************************************************/
static UCHAR PnlOpSetStsBits( PNLOP_DATA_HANDLE *hLedData )
{
	UCHAR sts = 0;

	sts |= ((hLedData->var.status & STS_TGON) != 0)		? LEDCODE_B0 : 0;
	sts |= ((hLedData->var.status & STS_SV_ON) == 0)	? LEDCODE_B6 : 0;
	sts |= ((hLedData->var.status & STS_DEN) == 0)		? LEDCODE_B3 : 0;

#if 0 /* Safety現状未対応 */
#if ( CCSW_SFTID == SFTID_EXIST )
	sts |= ((hLedData->var.status & STS_SFON) != 0)		? LEDCODE_B5 : 0;
	sts |= ((hLedData->var.status & STS_SAFE) != 0)		? LEDCODE_B5 : 0;
#endif
#endif

	sts |= LEDCODE_B7;	/* D.P LEDは常に点灯 */

	return	sts;
}



/*****************************************************************************
  Description: 7segLED 表示コード取得用API
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
  Return:
	UCHAR			: Servo Mode
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
UCHAR PnlOpGetLedCode( PNLOP_DATA_HANDLE *hLedData )
{
	return (hLedData->var.code);
}



/*****************************************************************************
  Description: 7segLED 状態表示情報設定用API
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
  Return:
	UCHAR			: Servo Mode
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void PnlOpSetLedStatus( PNLOP_DATA_HANDLE *hLedData, USHORT ServoSts )
{
	hLedData->var.status = ServoSts;
	return;
}



/*****************************************************************************
  Description: パネオペ CONNECT LED状態表示情報取得用API
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
  Return:
	UCHAR			: Servo Mode
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
BOOL PnlOpGetConnectStatus( PNLOP_DATA_HANDLE *hLedData )
{
	return	(hLedData->var.ConnectSts);
}



/*****************************************************************************
  Description: パネオペ CONNECT LED 状態表示情報設定用API
------------------------------------------------------------------------------
  Parameters:
	hLedData:		パネオペ制御用ハンドル
	ConnectSts:		M-III通信CONNECT状態(いずれか1軸でもCONNECT中であればTRUE)
  Return:
	UCHAR			: Servo Mode
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void PnlOpSetConnectStatus( PNLOP_DATA_HANDLE *hLedData, BOOL ConnectSts )
{
	hLedData->var.ConnectSts = ConnectSts;
	return;
}

/* <S0CB> Start */
/*****************************************************************************
  Description: 7segLED表示確認試験処理
------------------------------------------------------------------------------
  Parameters:
	LightIntervalTime;	7segLED点灯時間間隔
  Return:
	none
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
//void LedChkTest( ULONG LightIntervalTime )
BOOL LedChkTest(  PNLOP_DATA_HANDLE *hLedData, ULONG LightIntervalTime )
{
	UCHAR code;
	ULONG IntervalTimeMs;
	BOOL	ret;

//	PNLOP_LED_DATA_HANDLE *hLedData;
//	hLedData = &PnlOpLedDataHndl;

	hLedData->var.cnt_limt = 15U;				/* カウンタ最大値設定 */
	IntervalTimeMs = LightIntervalTime * 100;	/* 点灯間隔時間設定値が100[ms]単位である為、[ms]単位へ変更 */
	ret = FALSE;


	if(KlibGetLongTimerMs(hLedData->var.SysTime) > hLedData->var.timer[hLedData->var.cnt])
	{
		KlibRstLongTimer(&hLedData->var.SysTime);
		KlibRstLongTimer(&hLedData->var.SysTime2);

		/* 表示パターンセット */
		hLedData->var.led[hLedData->var.cnt] = LedLightPatternTbl[hLedData->var.cnt];

		/* 点灯間隔時間設定 */
		hLedData->var.timer[hLedData->var.cnt] = IntervalTimeMs;

		/* カウンタ更新 */
		hLedData->var.cnt++;

		if(hLedData->var.cnt > hLedData->var.cnt_limt)
		{
			/* カウンタリセット */
			hLedData->var.cnt = 0U;
		}
		hLedData->var.code = ~hLedData->var.led[hLedData->var.cnt];

		/* 表示更新 */
//		HALdrv_Net7SegLedControl( code );
		ret = TRUE;
		
	}
	else
	{
		if((hLedData->var.cnt == 0U) && (KlibGetLongTimerMs(hLedData->var.SysTime2) > 20U))
		{
			KlibRstLongTimer(&hLedData->var.SysTime2);
			hLedData->var.led[0] = PATTERN1;		/* パターン1を表示 */
			hLedData->var.code = ~hLedData->var.led[0];

			/* 表示更新 */
//			HALdrv_Net7SegLedControl( code );
			ret = TRUE;

		}
	}
	return	ret;
}
/* <S0CB> End */

/***************************************** end of file **********************************************/
