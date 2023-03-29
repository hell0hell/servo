/****************************************************************************************************/
/*																									*/
/*																									*/
/*		HwAccess.c : INGRAM用H/Wアクセス関数定義													*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.06.08	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "HwDeviceIf.h"
//#include "INTEGRITY.h" //#include "mp_api.h"
#include "MLib.h"
#include "KLib.h"
#ifndef _MERCURY_PLATFORM_	/*<S1A2>*/
#include "JL086ServoIpHw.h"
#endif	/*_MERCURY_PLATFORM_*/	/*<S1A2>*/
#include "Interrupt.h"


/* M-III 通信設定(DipSW/RotarySWによる) */
#ifndef _TEST_BOARD_ /* 本基板用 */

	#define DATA_SIZE_MSK			0x00003000	/* M-III Data Size設定ビットマスク (Dip-SW) */
	#define DATA_SIZE_BIT_SFT		12			/* M-III Data Size設定シフト数  (Dip-SW) */
	#define STATION_ADDRESS_MSK		0x00000FF0	/* M-III Address設定ビットマスク (Rotary-SW1,2) */
	#define STATION_ADDRESS_SFT		4			/* M-III Address設定シフト数  (Rotary-SW1,2) */
	#define ROTALY_SW1_MSK			0x000000F0	/* RotalySW1状態取得用ビットマスク					*/
	#define ROTALY_SW1_SFT			4
	#define ROTALY_SW2_MSK			0x00000F00	/* RotalySW2状態取得用ビットマスク					*/
	#define ROTALY_SW2_SFT			8
	#define DIP_SW_MSK				0x0000F000	/* DipSW状態取得用ビットマスク */
	#define DIP_SW_SFT				12			/* DipSW状態取得用シフト数 */
	#define IN0_REGISTER_DSW		0			/* DIPスイッチ入力レジスタ DipSW					*/

#else /* 以下削除予定 */
	#define DATA_SIZE_MSK			0x0003	/* IN0 DIP入力レジスタ DipSW1 M-III Data Size 設定		*/
	#define DATA_SIZE_BIT_SFT		4		/* IN0 DIP入力レジスタ DipSW1 M-III Data Size 設定		*/
	#define IN0_REGISTER_DSW		0		/* IN0 DIPスイッチ入力レジスタ DipSW1					*/
	#define IN1_REGISTER_RSW		1		/* IN1 ロータリスイッチ入力レジスタ R-SW1,2				*/
#endif

/* ---futami---> */
#define	OUT7_REGISTER				0x0007	/* OUT7 WDTトリガ設定レジスタ							*/
#define	OUT8_REGISTER				0x0008	/* OUT8 WDTクリア設定レジスタ							*/
/* <---futami--- */

/* ---futami (R)BOOT Version Check---> */
/* #define LOCALBUS_FLASH_BASE 	0xfc000000 */
//BOOT基板無し
//#define	BOOT_VERSION_OFS			0x03e00000
//#define	BOOTSTRAP_VERSION_OFS		0x03e00008
//BOOT基板有り
#define	BOOT_VERSION_OFS			0x01e00000
#define	BOOTSTRAP_VERSION_OFS		0x01e00008
/* <---futami (R)BOOT Version Check--- */


/*--------------------------------------------------------------------------------------------------*/
/*		シリアル入出力データ取得用構造体定義														*/
/*--------------------------------------------------------------------------------------------------*/
/* 2012.09.06 Y.Oka for Mercury base */
typedef	struct SERIAL_INPUT {
	HWSIN			SerialSignalIn;			/* Serial Input Data(フィルタ後) */
	HWSIN			UnInSignal;				/* Serial Input Data(BinK.UnInSignal相当) */
	ULONG			SignalFilBuf[2];		/* Input Signal with Bit Filter */
} SERIAL_INPUT;

typedef	struct SERIAL_OUTPUT {
	HWSOUT			RegeneSigOut;			/* Output Regene Signal */
	HWSOUT			RelaySigOut;			/* Output RelayOn Signal */
	HWSOUT			AlarmSigOut;			/* Output Alarm Signal *//* <S04B> */
	HWSOUT			LedOutput;				/* Output 7seg LED */
//	HWSOUT			BrakeSigOut;			/* Output Brake Signal *//* <S04B> */
	HWSOUT			LayoutSigOut;			/* Output Layout Signal *//* <S04B> */
//	VBOOL			SignalForceOut;			/* Enable FBA Force Output Signals */	/* <S0CB> */
	HWSOUT			SignalForceOut;			/* Enable FBA Force Output Signals */	/* <S0CB>  */
	HWSOUT			SerialSignalOut;		/* Serial Output Data for Monitor */
/*--------------------------------------------------------------------------------------------------*/
	HWSOUT			SignalScanBOut;			/* <S045> M2M3: Sequencial Output Signal */
	HWSOUT			SignalScanCOut;			/* <S045> M2M3: Sequencial Output Signal */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR			PnlLedOut[5];			/* @@ AP: Panel Operator LEDs */
	UCHAR			PnlLedIdx;				/* @@ AP: Panel Operator LED Index */
	USHORT			OutSigs;				/* @@ AP: Output Signals */
} SERIAL_OUTPUT;




/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
/*	void HALdrv_StartAdConvert( void );																*/
/*	LONG HALdrv_GetDcVoltFromAd( void );															*/
/*	void HALdrv_BrakeControl( LONG ax_no, BOOL sts );												*/
/*	void HALdrv_RegeneControl( BOOL sts );															*/
/*	void HALdrv_RelayControl( BOOL sts );															*/
/*	void HALdrv_seg_disp( LONG seg_data );															*/
/*	ULONG HALdrv_GetSerialIOData( ASIC_HW_IF_STR *pAsicHwReg, ULONG *pInputData, ULONG *pFilterBuf );*/
/*	void HALdrv_SetSerialIOData( ASIC_HW_IF_STR *pAsicHwReg, ULONG SetData );						*/
/*	LONG HALdrv_GetTransmissionByte( void );														*/
/*	LONG HALdrv_GetStationAddress( void );															*/
/****************************************************************************************************/
#ifdef	__ghs__
#pragma ghs section bss=".datacache"
/*--------------------------------------------------------------------------------------------------*/
#endif

/* Check Post Output */
static VULONG			*HALdrv_CheckPost;

static SERIAL_INPUT		SerialInput;
static SERIAL_OUTPUT	SerialOutput;

//static IODevice			IOPldRegDev; /* 不使用 <Oka02> */
//static IODevice			IOPldHscanDev; /* 不使用 <Oka02> */


#if 0 /* 2012.09.07 Y.Oka for Mercury Base (only INGRAM) */
static VSHORT			*HALdrv_ConvCurrent;	/* コンバータ過負荷検出用ADアドレスへのポインタ */

/* ---futami (R)BOOT Version Check---> */
static VULONG			*HALdrv_NorflashAddress;
/* <---futami (R)BOOT Version Check--- */
#endif

#ifdef _VC_DEBUG_
static ULONG			DummyRegAddress;
#endif

#ifdef	__ghs__
#pragma ghs section bss=default
/*--------------------------------------------------------------------------------------------------*/
#endif



#ifndef _MERCURY_PLATFORM_	/*<S1A2>*/
/****************************************************************************/
/* 概要：5Vトレラントスタンバイ解除処理		<S01F>							*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 解説																		*/
/*	5Vトレラント：JL-086起動前に5Vが入力されるのを防ぐための機能。			*/
/*																			*/
/*	5Vトレラントがスタンバイ状態の場合、UDL側のOV、OC、OC2検出が機能しない。*/
/*	初期化処理にて実行する。												*/
/****************************************************************************/
void HALdrv_Clear5VTBStandby( void )
{
	ULONG	*CpuPort;
//	CpuPort = (ULONG*)BUF_5T_CTRL_ADR;
	CpuPort = (ULONG*)ARMPF_SCU_5TBUF_EN;//<S0C7>重複定義。ARMPF_SCU.hに定義してある変数を使用する。

//	*CpuPort &= ~(OV_CNT | OC1_CNT | OC2_CNT | SYNCL_CNT);
	*CpuPort &= ~((1<<OV_CNT) | (1<<OC1_CNT) | (1<<OC2_CNT) | (1<<SYNCL_CNT));/*<S132>*/
	return;
}



/****************************************************************************/
/* 概要：主回路電圧検出用A/D変換開始処理									*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_StartAdConvert( void )
{
#if defined(_VC_DEBUG_)
	// ;
#elif defined(_TEST_BOARD_) /* @@削除予定 */
	DET_DCVOLT_START( );

#else /* 本基板 */
	// 不要
#endif

	return;
}


/****************************************************************************/
/* 概要：主回路電圧値取得処理												*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	主回路DC電圧(P-N間電圧)										*/
/****************************************************************************/
LONG HALdrv_GetDcVoltFromAd( void )
{
#if defined(_VC_DEBUG_)
	LONG	voltage;
	voltage = 10445;
#elif defined(_TEST_BOARD_) /* @@削除予定 */
	SHORT	voltage;
	voltage = GET_PN_DC_VOLT( );

#else /* 本基板 */
	LONG	voltage;
	voltage = GET_PN_DC_VOLT( );

/* for debug */
#ifdef	_BOARD_DEBUG_
	DetectVoltAd = voltage;
/* for debug */
#endif /* _BOARD_DEBUG_ */

#endif

	return voltage;
}


#if 0 /* 2013.11.17 Y.Oka 信号割り付け対応済みのため不要 */
/****************************************************************************/
/* 概要：ブレーキ制御用データ設定処理										*/
/****************************************************************************/
/* 引数		:	BOOL sts		ブレーキ制御ステータス ON/OFF				*/
/*			:	LONG ax_no		設定軸番号									*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_BrakeControl( LONG ax_no, BOOL sts )
{
#if 0
	ULONG out;
	out = SerialOutput.BrakeSigOut.dw;
	out &= ~(1 << ax_no);	/* Clear */
	out |= (sts << ax_no);	/* Set */
	SerialOutput.BrakeSigOut.dw = out;
#else
	/* ブレーキ未対応 */
#endif
}
#endif


/****************************************************************************/
/* 概要：FBA I/O出力データ設定処理											*/
/****************************************************************************/
/* 引数		:	outSignal		信号出力データ								*/
/*			:	mask			信号出力データ有効ビット					*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/****************************************************************************/
/*	【Σ7-S】						【Σ7-W】								*/
/*		┌───┬─────┐			┌───┬───────┐			*/
/*		│Bit	│ 信号名	│			│Bit	│	信号名		│			*/
/*		├───┼─────┤			├───┼───────┤			*/
/*		│	0	│	ALM		│			│	0	│	ALARM0		│			*/
/*		│	1	│	SO1		│			│	1	│	ALARM1		│			*/
/*		│	2	│	SO2		│			│	2	│	SO1(=OUT2)	│			*/
/*		│	3	│	SO3		│			│	3	│	SO2(=OUT3)	│			*/
/*		│	4	│	ALO1	│			│	4	│	SO3(=OUT4)	│			*/
/*		│	5	│	ALO2	│			│	5	│	SO4(=OUT5)	│			*/
/*		│	6	│	ALO3	│			│	6	│	SO5(=OUT6)	│			*/
/*		│	7	│	PCO		│			├───┼───────┤			*/
/*		├───┼─────┤			│	7	│	予約		│			*/
/*		│	8	│	予約	│			│	|	│				│			*/
/*		│	|	│			│			│	15	│				│			*/
/*		│	15	│			│			└───┴───────┘			*/
/*		└───┴─────┘												*/
/*																			*/
/*	※出力信号設定データ(42H:0x80004086/40H:0x2043)の出力させる信号bitを	*/
/*	　Onすると、出力信号モニタ(42H:0x8001C00E/40H:0xE007)の該当信号bitが	*/
/*	　Onする。(出力信号設定データがOffで出力信号モニタOffとなる)			*/
/*																			*/
/****************************************************************************/
USHORT HALdrv_WriteForceOutputSignals( ULONG outSignal, BOOL fout )	/* <S1AB> : 強制出力仕様変更の為、処理関数内の処理変更 */
{
	USHORT ret;

	if(fout)
	{	/* 強制出力設定あり */
#if CSW_BOARD_TYPE != SGD7W_TYPE
		/*--------------------------------------------------------------------------------------------------*/
		/*		SGD7S																						*/
		/*--------------------------------------------------------------------------------------------------*/
		KPI_SOUT_ALARM0( SerialOutput.SignalForceOut, outSignal & 0x01 );			/* bit0 : ALM */
		KPI_SOUT_SO1( SerialOutput.SignalForceOut, ((outSignal & 0x02) ^ 0x02) );	/* bit1 : SO1 */
		KPI_SOUT_SO2( SerialOutput.SignalForceOut, ((outSignal & 0x04) ^ 0x04) );	/* bit2 : SO2 */
		KPI_SOUT_SO3( SerialOutput.SignalForceOut, ((outSignal & 0x08) ^ 0x08) );	/* bit3 : SO3 */
	#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
		/* AP形のみ */
		KPI_SOUT_SO4( SerialOutput.SignalForceOut, ((outSignal & 0x10) ^ 0x10) );	/* bit4 : SO4(ALO1) */
		KPI_SOUT_SO5( SerialOutput.SignalForceOut, ((outSignal & 0x20) ^ 0x20) );	/* bit5 : SO5(ALO2) */
		KPI_SOUT_SO6( SerialOutput.SignalForceOut, ((outSignal & 0x40) ^ 0x40) );	/* bit6 : SO6(ALO3) */
	#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */
		/* 「bit7 : 分周Ｃ相設定」は本関数呼び元である「FnExecuteForceSignalOut()」にて実行 */

#else
		/*--------------------------------------------------------------------------------------------------*/
		/*		SGD7W																						*/
		/*--------------------------------------------------------------------------------------------------*/
		KPI_SOUT_ALARM0( SerialOutput.SignalForceOut, outSignal & 0x01 );			/* bit0 : ALARM0		*/
		KPI_SOUT_ALARM1( SerialOutput.SignalForceOut, outSignal & 0x02 );			/* bit1 : ALARM1		*/
		KPI_SOUT_SO1( SerialOutput.SignalForceOut, ((outSignal & 0x04) ^ 0x04) );	/* bit2 : SO1(=OUT2)	*/
		KPI_SOUT_SO2( SerialOutput.SignalForceOut, ((outSignal & 0x08) ^ 0x08) );	/* bit3 : SO2(=OUT3)	*/
		KPI_SOUT_SO3( SerialOutput.SignalForceOut, ((outSignal & 0x10) ^ 0x10) );	/* bit4 : SO3(=OUT4)	*/
		KPI_SOUT_SO4( SerialOutput.SignalForceOut, ((outSignal & 0x20) ^ 0x20) );	/* bit5 : SO4(=OUT5)	*/
		KPI_SOUT_SO5( SerialOutput.SignalForceOut, ((outSignal & 0x40) ^ 0x40) );	/* bit6 : SO5(=OUT6)	*/
#endif /*CSW_BOARD_TYPE != SGD7W_TYPE */

		/* ＨＷ出力信号に反映 */
		ret = (USHORT)(SerialOutput.SignalForceOut.dw);
	}
	else
	{/* 強制出力設定なし */
		ret = 0x0000;
	}
	return (ret);
}


/* <S1AB> Delete Start */
/*	下記仕様変更の為、コメントアウト。										*/
/*		①Σ7Sの強制出力の仕様をΣVとを合わせる。							*/
/*		②Σ7Wの強制出力の仕様変更(並びを変更)								*/
#if 0
//void HALdrv_WriteForceOutputSignals( ULONG outSignal, BOOL fout )	/* <S0CB> */
USHORT HALdrv_WriteForceOutputSignals( ULONG outSignal, BOOL fout )	/* <S0CB> */
{
	USHORT ret;/* <S0CB> */

#if 0
	SerialOutput.SignalForceOut = fout;
	/* Output Signals to FBA */
	SerialOutput.SoutRegAddress[0] = (USHORT)(outSignal);
	SerialOutput.SoutRegAddress[1] = (USHORT)(outSignal >> 16);
#else
	/* 強制出力未対応 */
/* <S0CB>	Start */
/* 強制出力設定 */
	if(fout)
	{
#if CSW_BOARD_TYPE != SGD7W_TYPE	/* <S114> */
/*--------------------------------------------------------------------------------------------------*/
/*		SGD7S																						*/
/*--------------------------------------------------------------------------------------------------*/
		/* 強制出力実行 */
		KPI_SOUT_SO1( SerialOutput.SignalForceOut, outSignal & 0x01 );		/* SO1 */
		KPI_SOUT_SO2( SerialOutput.SignalForceOut, outSignal & 0x02 );		/* SO2 */
		KPI_SOUT_SO3( SerialOutput.SignalForceOut, outSignal & 0x04 );		/* SO3 */
	#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
		KPI_SOUT_SO4( SerialOutput.SignalForceOut, outSignal & 0x08 );		/* SO4 */
		KPI_SOUT_SO5( SerialOutput.SignalForceOut, outSignal & 0x10 );		/* SO5 */
		KPI_SOUT_SO6( SerialOutput.SignalForceOut, outSignal & 0x20 );		/* SO6 */
	#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

		/* 「SO7 : 分周Ｃ相設定」は本関数呼び元である「FnExecuteForceSignalOut()」にて実行 */

		KPI_SOUT_ALARM0( SerialOutput.SignalForceOut, outSignal & 0x80 );	/* SO8:ALM */	/* <S114> */

/* <S114> Start */
#else
/*--------------------------------------------------------------------------------------------------*/
/*		SGD7W																						*/
/*--------------------------------------------------------------------------------------------------*/
/* <S114> Delete Start : 暫定処理 ⇒ 仕様書に合わせたよりへ変更する為、コメントアウト */
//#ifndef	CSW_SGD7W_TYPE		/* ALM0,ALM1出力対応<S104> */
//		KPI_SOUT_ALARM0( SerialOutput.SignalForceOut, outSignal & 0x80 );	/* SO8:ALM */
//#else
//		/* @@zantei */
//		KPI_SOUT_ALARM0( SerialOutput.SignalForceOut, outSignal & 0x80 );	/* SO8:ALM */
//		KPI_SOUT_ALARM1( SerialOutput.SignalForceOut, outSignal & 0x100 );	/* SO8:ALM */
//#endif
/* <S114> Delete End : 暫定処理 ⇒ 仕様書に合わせたよりへ変更する為、コメントアウト */
		KPI_SOUT_SO1( SerialOutput.SignalForceOut, outSignal & 0x01 );		/* SO1(=OUT2)	*/
		KPI_SOUT_SO2( SerialOutput.SignalForceOut, outSignal & 0x02 );		/* SO2(=OUT3)	*/
		KPI_SOUT_SO3( SerialOutput.SignalForceOut, outSignal & 0x04 );		/* SO3(=OUT4)	*/
		KPI_SOUT_SO4( SerialOutput.SignalForceOut, outSignal & 0x08 );		/* SO4(=OUT5)	*/
		KPI_SOUT_SO5( SerialOutput.SignalForceOut, outSignal & 0x10 );		/* SO5(=OUT6)	*/
		KPI_SOUT_ALARM0( SerialOutput.SignalForceOut, outSignal & 0x20 );	/* ALARM0		*/
		KPI_SOUT_ALARM1( SerialOutput.SignalForceOut, outSignal & 0x40 );	/* ALARM1		*/
#endif	/* <S114> End */

		/* ＨＷ出力信号に反映 */
		//FnCmnCtrl->ForceOutSignalsHW = SerialOutput.SignalForceOut;
		ret = (USHORT)(SerialOutput.SignalForceOut.dw);
	}
	else
	{
		//SerialOutput.SignalForceOut.dw = 0x0000;
		ret = 0x0000;
	}
	return (ret);
/* <S0CB>	End */
#endif
}
#endif	/* <S1AB> Delete End */

/****************************************************************************/
/* 概要：回生トランジスタ制御用データ設定処理								*/
/****************************************************************************/
/* 引数		:	BOOL sts		回生トランジスタ制御ステータス ON/OFF		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_RegeneControl( BOOL sts )
{
	SerialOutput.RegeneSigOut.flag.RegenOn = sts;
}

BOOL HALdrv_GetRegeneControlState( void )
{
	BOOL sts;
//	sts = (0 != (SerialOutput.SerialSignalOut.dw & 0x0002)) ? FALSE : TRUE;		/* <S0EF> */
	sts = (0 != (SerialOutput.SerialSignalOut.dw & REGON_STS)) ? FALSE : TRUE;	/* <S0EF> */
	return sts;
}



#if 0 /* <S0A2> */
/****************************************************************************/
/* 概要：突防リレー制御用データ設定処理										*/
/****************************************************************************/
/* 引数		:	BOOL sts		突防リレー制御ステータス ON/OFF				*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_RelayControl( BOOL sts )
{
	SerialOutput.RelaySigOut.flag.RelayOn = sts;
}
#else /* <S0A2> */
/****************************************************************************/
/* 概要：突防リレー制御用データ設定処理										*/
/****************************************************************************/
/* 引数		:	BOOL sts			突防リレー制御ステータス ON/OFF			*/
/*			:	void *AsicHwReg		UDL Access Address for DB Control		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_RelayControl( void *AsicHwReg, BOOL sts )
{
#if CSW_BOARD_TYPE == SGD7S_TYPE
	/* 同期シリアルにて突防リレー制御を行う */
	SerialOutput.RelaySigOut.flag.RelayOn = sts;
#else /* CSW_PWBORARD_TYPE == SGD7W_TYPE */
	/* UDL DB Control にて突防リレー制御を行う */
	SHalDrv_RelayControl( AsicHwReg, sts );
#endif
}
#endif /* <S0A2> */

/****************************************************************************/
/* 概要：突防リレー状態取得													*/
/****************************************************************************/
/* 引数		:	BOOL sts		突防リレー制御ステータス ON/OFF				*/
/* 戻り値	:	なし														*/
/****************************************************************************/
BOOL HALdrv_GetRelayOnAck( void )
{
	BOOL err;
	err = SerialOutput.RelaySigOut.flag.RelayOn;
	return (err & 0x1);
}

/****************************************************************************/
/* 概要：同期シリアルI/Oデータ初期化処理									*/
/****************************************************************************/
/* 引数		:	void* SIRegs		Register address of the SI signals		*/
/*				void* SORegs		Register address of the SO signals		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
const UCHAR IniPnlOut[5] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF /* <S153> All Blank */ };
void HALdrv_InitSerialIO( void )
{
//	INT i;//<S0C7>
/*--------------------------------------------------------------------------------------------------*/
	/*  @@ AP: Panel Operator */
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &SerialOutput, sizeof( SERIAL_OUTPUT )/4 );	/* <S03C> */

	/* <S153> Initialize Panel Buffer */
	MlibCopyByteMemory( &(SerialOutput.PnlLedOut[0]), &(IniPnlOut[0]), sizeof( IniPnlOut ) );
/*--------------------------------------------------------------------------------------------------*/

	/* Initialize Serial Input Buffer */
	SerialInput.SerialSignalIn.dw = 0;

	/* Initialize Serial Output Data */
//	SerialOutput.BrakeSigOut.flag.SO1 = FALSE; 		/* Brake状態 */
	SerialOutput.RegeneSigOut.flag.RegenOn = FALSE;	/* Regene OFF */
	SerialOutput.RelaySigOut.flag.RelayOn = FALSE;	/* Relay ON */
//	SerialOutput.LedOutput.dw = 0x00000000;			/* LED 全点灯 *//* <S0A1> */
	SerialOutput.LedOutput.dw = LED_INIT_DISPLAY;	/* LED 全消灯 *//* <S0A1> */
}

#if(PRODUCT_SERIES_SEVEN == FALSE)    /* <S09F> */
/****************************************************************************/
/* 概要：同期シリアルデータ入力サービス										*/
/****************************************************************************/
/* 引数					*/
/*Function declaration	*/
static ULONG SerialtoGPIOconvert(ULONG);
typedef	union Calculate_GPIO
{
	struct {
			UINT	bit0  :1;
			UINT	bit1  :1;
			UINT	bit2  :1;
			UINT	bit3  :1;
			UINT	bit4  :1;
			UINT	bit5  :1;
			UINT	bit6  :1;
			UINT	bit7  :1;
			/*----------------*/
			UINT	bit8  :1;
			UINT	bit9  :1;
			UINT	bit10 :1;
			UINT	bit11 :1;
			UINT	bit12 :1;
			UINT	bit13 :1;
			UINT	bit14 :1;
			UINT	bit15 :1;
			/*----------------*/
			UINT	bit16  :1;
			UINT	bit17  :1;
			UINT	bit18  :1;
			UINT	bit19  :1;
			UINT	bit20  :1;
			UINT	bit21  :1;
			UINT	bit22  :1;
			UINT	bit23  :1;
			/*----------------*/
			UINT	bit24  :1;
			UINT	bit25  :1;
			UINT	bit26  :1;
			UINT	bit27  :1;
			UINT	bit28  :1;
			UINT	bit29  :1;
			UINT	bit30  :1;
			UINT	bit31  :1;
		}Calculate_bit;
  ULONG  Value;
}Calculate_GPIO;
/****************************************************************************/
/* 概要：		Serial Data to GPIO convert						                */
/****************************************************************************/
/* 引数	: 	  Data_in													    */
/* 戻り値	:	  Data_Out	                                                    */
/****************************************************************************/
static ULONG SerialtoGPIOconvert(ULONG Data_in)
{
#define	bit0_Value		0x01    /* bit00 */
#define	bit1_Value		0x02    /* bit01 */
#define	bit2_Value		0x04	/* bit02 */
#define	bit3_Value		0x08	/* bit03 */
#define	bit4_Value		0x10	/* bit04 */
#define	bit5_Value		0x20	/* bit05 */
#define	bit6_Value		0x40	/* bit06 */
#define	bit7_Value		0x80	/* bit07 */

ULONG    GPIO_Read_H;/*Used to read the GPIO register values*/
ULONG    GPIO_Read_L;
ULONG    Data_Out;
union Calculate_GPIO  Calculate_flag;

	GPIO_Read_H = GPIO_Inputdata_P4();
	GPIO_Read_L = GPIO_Inputdata_P3();
	GPIO_Read_H = GPIO_Read_H & 0x000000FF;/*Take last 8 BIT data*/
	GPIO_Read_L = GPIO_Read_L & 0x000000FF;/*Take last 8 BIT data*/

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
    Calculate_flag.Calculate_bit.bit31 = (GPIO_Read_L & bit3_Value)>>3;/*Pin40  /S-ON*/
	Calculate_flag.Calculate_bit.bit30 = (GPIO_Read_L & bit4_Value)>>4;/*Pin42  P-OT*/
    Calculate_flag.Calculate_bit.bit29 = (GPIO_Read_L & bit5_Value)>>5;/*Pin43  N-OT*/
    Calculate_flag.Calculate_bit.bit28 = (GPIO_Read_L & bit6_Value)>>6;/*Pin41  /P-CON*/
	Calculate_flag.Calculate_bit.bit27 = (GPIO_Read_L & bit7_Value)>>7;/*Pin44  /ALM-RST*/
	Calculate_flag.Calculate_bit.bit26 = 0;
	Calculate_flag.Calculate_bit.bit25 = 0;
	Calculate_flag.Calculate_bit.bit24 =  (GPIO_Read_H & bit2_Value)>>2;/*Regene Alarm*/
	Calculate_flag.Calculate_bit.bit23 = (GPIO_Read_L & bit0_Value)>>0;/*FAN Alarm*/
	Calculate_flag.Calculate_bit.bit22 = (GPIO_Read_L & bit2_Value)>>2;/*Acon2  */
	Calculate_flag.Calculate_bit.bit21 = (GPIO_Read_L & bit1_Value)>>1;/*Acon1  */

	Calculate_flag.Calculate_bit.bit20 = (GPIO_Read_H & bit1_Value)>>1;/*SEN    */
	Calculate_flag.Calculate_bit.bit19 = (GPIO_Read_H & bit7_Value)>>7;/*BTsw14    */
	Calculate_flag.Calculate_bit.bit18 =  (GPIO_Read_H & bit6_Value)>>6;/*BTsw13    */
	Calculate_flag.Calculate_bit.bit17 =  (GPIO_Read_H & bit4_Value)>>4;/*BTsw12    */
	Calculate_flag.Calculate_bit.bit16 = (GPIO_Read_H & bit3_Value)>>3;/*BTsw11    */
#endif

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	Calculate_flag.Calculate_bit.bit31 =  0;
	Calculate_flag.Calculate_bit.bit30 = (GPIO_Read_L & bit4_Value)>>4;/*Pin7  */
	Calculate_flag.Calculate_bit.bit29 = (GPIO_Read_L & bit5_Value)>>5;/*Pin8  */
	Calculate_flag.Calculate_bit.bit28 = (GPIO_Read_L & bit6_Value)>>6;/*Pin9  */
    Calculate_flag.Calculate_bit.bit27 = (GPIO_Read_L & bit7_Value)>>7;/*Pin10  */

	Calculate_flag.Calculate_bit.bit26 = 0;
	Calculate_flag.Calculate_bit.bit25 = 0;

	Calculate_flag.Calculate_bit.bit24=  (GPIO_Read_H & bit2_Value)>>2;/*Regene Alarm*/
	Calculate_flag.Calculate_bit.bit23 = (GPIO_Read_L & bit0_Value)>>0;/*FAN Alarm*/
	Calculate_flag.Calculate_bit.bit22 = (GPIO_Read_L & bit2_Value)>>2;/*Acon2  */
	Calculate_flag.Calculate_bit.bit21 = (GPIO_Read_L & bit1_Value)>>1;/*Acon1  */

	Calculate_flag.Calculate_bit.bit20 = 0;
	Calculate_flag.Calculate_bit.bit19 = 0;
	Calculate_flag.Calculate_bit.bit18 = 0;
	Calculate_flag.Calculate_bit.bit17 = 0;
	Calculate_flag.Calculate_bit.bit16 = 0;

	Calculate_flag.Calculate_bit.bit15 = (Data_in)>>31;/*D-SW4*/
	Calculate_flag.Calculate_bit.bit14 = (Data_in)>>30;/*D-SW3  */
	Calculate_flag.Calculate_bit.bit13 = (Data_in)>>29;/*D-SW2  */
	Calculate_flag.Calculate_bit.bit12 = (Data_in)>>28;/*D-SW1    */
	Calculate_flag.Calculate_bit.bit11 = (Data_in)>>27;/*R-SW01    */
	Calculate_flag.Calculate_bit.bit10 = (Data_in)>>26;/*R-SW02    */
	Calculate_flag.Calculate_bit.bit9  = (Data_in)>>25;/*R-SW03    */
	Calculate_flag.Calculate_bit.bit8  = (Data_in)>>24;/*R-SW04    */

	Calculate_flag.Calculate_bit.bit7 = 0;
	Calculate_flag.Calculate_bit.bit6 = 0;
	Calculate_flag.Calculate_bit.bit5 = 0;
	Calculate_flag.Calculate_bit.bit4 = 0;
	Calculate_flag.Calculate_bit.bit3 = 0;
	Calculate_flag.Calculate_bit.bit2 = 0;
	Calculate_flag.Calculate_bit.bit1 = 0;
	Calculate_flag.Calculate_bit.bit0 = 0;

#endif

	//Calculate_flag.Value= Calculate_flag.Value&0xFFFF0000;/*Remove low, Save high*/
	//Data_in=Data_in&0x0000FFFF;/*Remove high, Save low*/
	//Data_Out = Calculate_flag.Value&Data_in;/*2 Data Merge*/
    Data_Out = Calculate_flag.Value;
    return Data_Out;
}
#endif

/****************************************************************************/
/* 概要：同期シリアルデータ入力サービス										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	ULONG					取得データ(2回読みフィルタ後)		*/
/****************************************************************************/
void HALdrv_InputSerialIO( void )
{
	LONG	lwk0;

	/* Read Input Signals */
	lwk0 = HW_SIN_GET( );
#if(PRODUCT_SERIES_SEVEN == FALSE)
	GPIO_InputDirection_PM3();/*GPIO 0  TO  GPIO 7==input*/
	GPIO_InputDirection_PM4();/*GPIO 10 TO  GPIO 17==input*/
	lwk0=SerialtoGPIOconvert(lwk0);
#endif
	/* Bit Filter */
	SerialInput.SignalFilBuf[0] =
			(((SerialInput.SignalFilBuf[1] | lwk0) & SerialInput.SignalFilBuf[0] )
			| (SerialInput.SignalFilBuf[1] & lwk0));
	SerialInput.SignalFilBuf[1] = lwk0;

	SerialInput.SerialSignalIn.dw = SerialInput.SignalFilBuf[0] ^ KPX_HWSINXOR;
	SerialInput.UnInSignal.dw = SerialInput.SignalFilBuf[0]; /* <XXX> */


}

/****************************************************************************/
/* Set Sequence Output Signal (後で消す)									*/
/****************************************************************************/
void HALdrv_SetSOut( int axis, UINT x )
{

#if 0 /* <S1C0> */
	if( axis == 0 )
	{
		KPI_SOUT_SO1( SerialOutput.LayoutSigOut, x & 0x01 );
		KPI_SOUT_SO2( SerialOutput.LayoutSigOut, x & 0x02 );
		KPI_SOUT_SO3( SerialOutput.LayoutSigOut, x & 0x04 );
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
		KPI_SOUT_SO4( SerialOutput.LayoutSigOut, x & 0x08 ); /* <S04B> */
		KPI_SOUT_SO5( SerialOutput.LayoutSigOut, x & 0x10 ); /* <S04B> */
		KPI_SOUT_SO6( SerialOutput.LayoutSigOut, x & 0x20 ); /* <S04B> */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

	}
	else
	{
		//KPI_SOUT_SO1( SerialOutput2.LayoutSigOut, x & 0x01 );
		//KPI_SOUT_SO2( SerialOutput2.LayoutSigOut, x & 0x02 );
		//KPI_SOUT_SO3( SerialOutput2.LayoutSigOut, x & 0x04 );
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE /* <S04B> */
		//KPI_SOUT_SO4( SerialOutput2.LayoutSigOut, x & 0x08 ); /* <S04B> */
		//KPI_SOUT_SO5( SerialOutput2.LayoutSigOut, x & 0x10 ); /* <S04B> */
		//KPI_SOUT_SO6( SerialOutput2.LayoutSigOut, x & 0x20 ); /* <S04B> */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */
	}
#endif/* <S1C0> */

HWSOUT	hwsout;

	hwsout.dw = 0;
	if( axis == 0 )
	{
		KPI_SOUT_SO1( hwsout, x & 0x01 );
		KPI_SOUT_SO2( hwsout, x & 0x02 );
		KPI_SOUT_SO3( hwsout, x & 0x04 );
		
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
		KPI_SOUT_SO4( hwsout, x & 0x08 );
		KPI_SOUT_SO5( hwsout, x & 0x10 );
		KPI_SOUT_SO6( hwsout, x & 0x20 );
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
		KPI_SOUT_SO4( hwsout, x & 0x08 );
		KPI_SOUT_SO5( hwsout, x & 0x10 );
#endif /*	SGD7W（2軸対応）のみ	*/
		SerialOutput.LayoutSigOut.dw = hwsout.dw;
	}
	else
	{
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
		KPI_SOUT_SO1( hwsout, x & 0x01 );
		KPI_SOUT_SO2( hwsout, x & 0x02 );
		KPI_SOUT_SO3( hwsout, x & 0x04 );
		KPI_SOUT_SO4( hwsout, x & 0x08 );
		KPI_SOUT_SO5( hwsout, x & 0x10 );
		SerialOutput.LayoutSigOut.dw |= hwsout.dw;
#endif /*	SGD7W（2軸対応）のみ	*/
	
	
	}

}

/****************************************************************************/
/* <S045> Set Sequence Output Signal (ScanC)								*/
/****************************************************************************/
void HALdrv_SetSOutC( int axis, HWSOUT hwsout )
{
	if( axis == 0 )
	{
		SerialOutput.SignalScanCOut = hwsout;
	}
	else
	{
		/* SGD7W IO入出力対応<S118> */
		SerialOutput.SignalScanCOut.dw |= hwsout.dw;
	}
}

/****************************************************************************/
/* <S045> Set Sequence Output Signal (ScanB)								*/
/****************************************************************************/
void HALdrv_SetSOutB( int axis, HWSOUT hwsout )
{
	if( axis == 0 )
	{
		SerialOutput.SignalScanBOut = hwsout;
	}
	else
	{
		/* SGD7W IO入出力対応<S118> */
		SerialOutput.SignalScanBOut.dw |= hwsout.dw;
	}
}

/****************************************************************************/
/* Set Alarm Output Signal (後で消す)										*/
/****************************************************************************/
void HALdrv_SetAlmOut( int axis, UINT x )
{
	if( axis == 0 )
	{
//		KPI_SOUT_ALARM( SerialOutput.RelaySigOut, x ); /* <S04B> */
		KPI_SOUT_ALARM0( SerialOutput.AlarmSigOut, x ); /* <S04B> *//* ALM0,ALM1出力対応<S104> */
	}
	else
	{
		/* TODO: */
		KPI_SOUT_ALARM1( SerialOutput.AlarmSigOut, x ); /* ALM0,ALM1出力対応<S104> */
	}
}

/****************************************************************************/
/* 概要：同期シリアル出力データ送信処理										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_OutputSerialIO( void )
{
	ULONG outx;

	/*----------------------------------------------------------------------*/
	/*		Set Output Data													*/
	/*----------------------------------------------------------------------*/
	/* Make Setting Data */
#ifndef	CSW_SGD7W_TYPE		/* ALM0,ALM1出力対応<S104> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)		/* AP以外 */
	outx = ( SerialOutput.LayoutSigOut.dw			/* <S04B> */
			| SerialOutput.RelaySigOut.dw
			| SerialOutput.RegeneSigOut.dw
			| SerialOutput.AlarmSigOut.dw			/* <S04B> */
			| SerialOutput.LedOutput.dw 
			| SerialOutput.SignalScanBOut.dw 		/* <S045> */
			| SerialOutput.SignalScanCOut.dw 		/* <S045> */
			);
#else											/* AP */
	outx = ( SerialOutput.LayoutSigOut.dw			/* <S04B> */
			| SerialOutput.RelaySigOut.dw
			| SerialOutput.RegeneSigOut.dw 
			| SerialOutput.AlarmSigOut.dw			/* <S04B> */
			| SerialOutput.SignalScanBOut.dw 		/* <S081> */
			| SerialOutput.SignalScanCOut.dw 		/* <S04B> */
			| (0x0800 << SerialOutput.PnlLedIdx )
			| (SerialOutput.PnlLedOut[ 4-SerialOutput.PnlLedIdx ] << 16) );
#endif
#else		/* #ifndef	CSW_SGD7W_TYPE	ALM0,ALM1出力対応<S104> */
	outx = ( SerialOutput.LayoutSigOut.dw			/* <S04B> */
			| SerialOutput.RelaySigOut.dw
			| SerialOutput.RegeneSigOut.dw
			| SerialOutput.AlarmSigOut.dw			/* <S04B> */
			| SerialOutput.LedOutput.dw
			| SerialOutput.SignalScanBOut.dw 		/* <S045> */
			| SerialOutput.SignalScanCOut.dw 		/* <S045> */
			);
#endif		/* #ifndef	CSW_SGD7W_TYPE	ALM0,ALM1出力対応<S104> */

//	outx = SerialOutput.RelaySigOut.dw;
//	outx = OutputData;

#if 0 /* 2013.09.26 Y.Oka 回生トランジスタ指令チェックの論理が反対となるため、反転処理の後へ移動 */ /* <S00D> */
	/* モニタ情報設定 */
	SerialOutput.SerialSignalOut.dw = outx;
#endif /* <S00D> */

	/* Lo Activeのため設定データをXOR */
#if	(CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)	/* APのみ  *//*<S138>*/
	if( TRUE == UsbFuncChkAvailable() )
	{/* RevC基板以降 */
		outx = outx ^ KPX_HWSOUTXOR;
	}
	else
	{/* RevA基板 */
		outx = outx ^ (KPX_HWSOUTXOR ^ 0x0000F800 );	/* 7segの反転ビット対応 */
	}
#else	/* #if	(CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) APのみ  <S138> */
	outx = outx ^ KPX_HWSOUTXOR;
#endif		/* #if	(CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) APのみ  <S138> */


	/* 2013.09.26 Y.Oka 回生トランジスタ指令チェックの論理が反対となるため、反転処理の後へ移動 */ /* <S00D> */
	/* モニタ情報設定 */
	SerialOutput.SerialSignalOut.dw = outx;

#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
	/* 分周出力形態の選択信号(ﾗｲﾝﾄﾞﾗｲﾊﾞ出力ｲﾈｰﾌﾞﾙ) Plsel=ON */
	outx |= BIT3;
#endif
	/* Output Signals */
	HW_SOUT_SET( outx );
}

/*--------------------------------------------------------------------------*/
/**
 *
 * @@ AP:
 * Test Panel Operator Led Test
 *
 */
/*--------------------------------------------------------------------------*/
void HALdrv_TstSetPnlLedC( UCHAR *LedCodes )
{
int Cntr;
	for( Cntr = 5-1; Cntr >= 0 ; Cntr -- )
	{
		SerialOutput.PnlLedOut[ Cntr ] = (LedCodes[ Cntr ] ^ 0xFF);	/* @@ */
	}
}

void HALdrv_TstOutputPnlLedC( void )
{
LONG ScanCcnt;
UINT PnlIdx;
UINT PnlLed;
UINT SoutX;
	/*----------------------------------------------------------------------*/
	/* timer <- ScanC Count, update PnlLedIdx on every 4ms 					*/
	/*----------------------------------------------------------------------*/
	ScanCcnt = CpiGetScanCCnt();
	PnlIdx = (( ScanCcnt >> 1 ) % 5);
	/*----------------------------------------------------------------------*/
	SerialOutput.PnlLedIdx = PnlIdx;
	/*----------------------------------------------------------------------*/
	if( 0 )
	{
		PnlLed = (1 << (( ScanCcnt >> 9 ) % 6) ^ 0xFF);
		SerialOutput.PnlLedOut[ PnlIdx ] = (UCHAR)PnlLed;
	}
	if ( 0 ) 
	{
	/*----------------------------------------------------------------------*
	 *	SO Signal Test
	 *     Panel SW1->SO1
	 *     Panel SW2->SO2
	 *     Panel SW3->SO3
	 *     Panel SW4->SO4
	 *----------------------------------------------------------------------*/
	UINT tstx = 1;		/* 1: SO1-4, 0:SO5-8 */
		SoutX = ((SerialInput.SerialSignalIn.dw >> 16) & 0x0F);
		SoutX = (( tstx ) ? SoutX : ((SoutX<<4)&0x03));
		SerialOutput.OutSigs = SoutX;
	}
}

#ifndef _TEST_BOARD_ /* 本基板処理 */

#if 0 /* <S0A2> */
/****************************************************************************/
/* 概要：7SEG LED出力 for Network Servo										*/
/****************************************************************************/
/* 引数		:	ULONG outData : 7SEG出力データ								*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_Net7SegLedControl( UCHAR outData )
{
	SerialOutput.LedOutput.dw = outData << NET_7SEGOUT_BIT_SFT;
}
#endif

#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE /* <S0A2> */
/****************************************************************************/
/* 概要：7SEG LED出力 for Network Servo										*/
/****************************************************************************/
/* 引数		:	UCHAR outData : 軸番号指定									*/
/*			:	ULONG outData : 7SEG出力データ								*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_Net7SegLedControl( UCHAR axis, UCHAR outData ) /* <S0A2> */
{
	if( axis == 0 )
	{
		/* Clear axis0 Data *//* <S0E4> */
		SerialOutput.LedOutput.dw &= ~(0x000000FF << NET_7SEG1OUT_BIT_SFT);
		/* Set axis0 Data */
		SerialOutput.LedOutput.dw |= outData << NET_7SEG1OUT_BIT_SFT;
	}
	else
	{
		/* Clear axis0 Data *//* <S0E4> */
		SerialOutput.LedOutput.dw &= ~(0x000000FF << NET_7SEG2OUT_BIT_SFT);
		/* Set axis0 Data */
		SerialOutput.LedOutput.dw |= outData << NET_7SEG2OUT_BIT_SFT;
	}
}



/****************************************************************************/
/* 概要：パネオペ LED出力 for Network Servo									*/
/****************************************************************************/
/* 引数		:	UCHAR outData : 軸番号指定									*/
/*			:	ULONG outData : 7SEG出力データ								*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_NetConLedControl( BOOL ledOut ) /* <S0A1> */
{
#if CSW_NETIF_TYPE == NETIF_M3_TYPE
	if( ledOut == TRUE )
	{
//		SerialOutput.LedOutput.dw |= ledOut << CON_LED_BIT_SFT;/* <S0B2> */
		SerialOutput.LedOutput.dw &= ~(1 << CON_LED_BIT_SFT);
	}
	else
	{
//		SerialOutput.LedOutput.dw &= ~(ledOut << CON_LED_BIT_SFT);/* <S0B2> */
		SerialOutput.LedOutput.dw |= 1 << CON_LED_BIT_SFT;
	}
#endif
	return;
}
#endif/* <S0A2> */


#else /* _TEST_BOARD_ 削除予定 */

//static USHORT PortOut_7seg;	//for debug tanaka21 不要
//static USHORT ScanCCnt;	//for debug tanaka21 不要
/****************************************************************************/
/* 概要：同期シリアル出力データ送信処理(ScanC) for debug tanaka21			*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_OutputSerialIO_ScanC( void )
{
ULONG timer;
HWSOUT	outx;
	KlibRstLongTimer( &timer );
	outx.dw = (1 << (16 + (( timer >> 9 ) % 6)));
	SerialOutput.LedOutput.dw &= ~((0x3F)<<16);		/* Clear 7Seg */
	SerialOutput.LedOutput.dw |= (outx.dw);			/* Set 7Seg */
}
#endif



/****************************************************************************/
/* 概要：シーケンス入力信号読み出し処理										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	非同期シリアル読み出しデータ(2回読みフィルタ後)				*/
/****************************************************************************/
HWSIN HALdrv_ReadInputSignals( void )
{
//<S045>	return	SerialInput.SerialSignalIn;
HWSIN hw;
	hw.dw = ((SerialInput.SerialSignalIn.dw & KPI_SINANDBITS) | KPI_SINSETBITS);	/* <S045> */
	return hw;
}

#if 0
/* ↓はNG                                                       for Un005 */
HWSIN HALdrv_ReadMonInputSignals( void )
{
	return	SerialInput.UnInSignal;
}
#else
/* for Un005 */
ULONG HALdrv_ReadMonInputSignals( LONG Axis )
{
HWSIN hwsin;
ULONG dw = 0;
#if CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
 	hwsin.dw = (( Axis == 0 )  ? SerialInput.UnInSignal.dw : 0/*SerialInput2.UnInSignal.dw*/);
	dw = (KPX_MON_HWSIN00( hwsin ) ? (dw | (1<<0)) : dw );
	dw = (KPX_MON_HWSIN01( hwsin ) ? (dw | (1<<1)) : dw );
	dw = (KPX_MON_HWSIN02( hwsin ) ? (dw | (1<<2)) : dw );
	dw = (KPX_MON_HWSIN03( hwsin ) ? (dw | (1<<3)) : dw );
	dw = (KPX_MON_HWSIN04( hwsin ) ? (dw | (1<<4)) : dw );
	dw = (KPX_MON_HWSIN05( hwsin ) ? (dw | (1<<5)) : dw );
	dw = (KPX_MON_HWSIN06( hwsin ) ? (dw | (1<<6)) : dw );
	dw = (KPX_MON_HWSIN07( hwsin ) ? (dw | (1<<7)) : dw );
/*--------------------------------------------------------------------------------------------------*/
#elif CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ　　 SGD7W IO入出力対応<S118>	*/
//-	hwsin.dw = (( Axis == 0 )  ? SerialInput.UnInSignal.dw : 0/*SerialInput2.UnInSignal.dw*/);
	if(Axis == 0)
	{	/*	1軸目	*/
		hwsin.dw = SerialInput.UnInSignal.dw;
		dw = (KPX_MON_HWSIN00( hwsin ) ? (dw | (1<<0)) : dw );
		dw = (KPX_MON_HWSIN01( hwsin ) ? (dw | (1<<1)) : dw );
		dw = (KPX_MON_HWSIN02( hwsin ) ? (dw | (1<<2)) : dw );
		dw = (KPX_MON_HWSIN03( hwsin ) ? (dw | (1<<3)) : dw );
		dw = (KPX_MON_HWSIN04( hwsin ) ? (dw | (1<<4)) : dw );
		dw = (KPX_MON_HWSIN05( hwsin ) ? (dw | (1<<5)) : dw );
//		dw = (KPX_MON_HWSIN06( hwsin ) ? (dw | (1<<6)) : dw );
//		dw = (KPX_MON_HWSIN07( hwsin ) ? (dw | (1<<7)) : dw );
	}else
	{	/*	2軸目	*/
		hwsin.dw = SerialInput.UnInSignal.dw;
//		dw = (KPX_MON_HWSIN00( hwsin ) ? (dw | (1<<0)) : dw );
//		dw = (KPX_MON_HWSIN01( hwsin ) ? (dw | (1<<1)) : dw );
//		dw = (KPX_MON_HWSIN02( hwsin ) ? (dw | (1<<2)) : dw );
//		dw = (KPX_MON_HWSIN03( hwsin ) ? (dw | (1<<3)) : dw );
//		dw = (KPX_MON_HWSIN04( hwsin ) ? (dw | (1<<4)) : dw );
//		dw = (KPX_MON_HWSIN05( hwsin ) ? (dw | (1<<5)) : dw );
		dw = (KPX_MON_HWSIN06( hwsin ) ? (dw | (1<<0)) : dw );
		dw = (KPX_MON_HWSIN07( hwsin ) ? (dw | (1<<1)) : dw );
		dw = (KPX_MON_HWSIN08( hwsin ) ? (dw | (1<<2)) : dw );
		dw = (KPX_MON_HWSIN09( hwsin ) ? (dw | (1<<3)) : dw );
		dw = (KPX_MON_HWSIN10( hwsin ) ? (dw | (1<<4)) : dw );
		dw = (KPX_MON_HWSIN11( hwsin ) ? (dw | (1<<5)) : dw );
	}
/*--------------------------------------------------------------------------------------------------*/
#endif
/*--------------------------------------------------------------------------------------------------*/
	return dw;
}
#endif

/****************************************************************************/
/* 概要：シーケンス出力信号読み出し処理										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	非同期シリアル出力データ(2回読みフィルタ後)					*/
/****************************************************************************/
HWSOUT HALdrv_ReadOuputSignals( void )
{
	return	SerialOutput.SerialSignalOut;
}
/****************************************************************************/
/* 概要：シーケンス出力信号読み出し処理										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:																*/
/****************************************************************************/
ULONG	HALdrv_ReadMonOutputSignals( LONG ax )
{
	HWSOUT hwsout;
	ULONG dw = 0;
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	SGD7W IO入出力対応<S118>	*/
	if(ax == 0)
	{	/*	1軸目	*/
/*--------------------------------------------------------------------------------------------------*/
//		hwsout.dw = (( ax == 0 )  ? SerialOutput.SerialSignalOut.dw : 0/* Zantei */ );
	 	hwsout.dw = SerialOutput.SerialSignalOut.dw;
/*--------------------------------------------------------------------------------------------------*/
//		dw = (KPX_MON_HWSOUT00( hwsout ) ? (dw | (1<<0)) : dw );
//		dw = (KPX_MON_HWSOUT01( hwsout ) ? (dw | (1<<1)) : dw );
//		dw = (KPX_MON_HWSOUT02( hwsout ) ? (dw | (1<<2)) : dw );
//		dw = (KPX_MON_HWSOUT03( hwsout ) ? (dw | (1<<3)) : dw );
//		dw = (KPX_MON_HWSOUT04( hwsout ) ? (dw | (1<<4)) : dw );
//		dw = (KPX_MON_HWSOUT05( hwsout ) ? (dw | (1<<5)) : dw );
//		dw = (KPX_MON_HWSOUT06( hwsout ) ? (dw | (1<<6)) : dw );
//		dw = (KPX_MON_HWSOUT07( hwsout ) ? (dw | (1<<7)) : dw );
		dw = (KPX_MON_HWSOUT00( hwsout ) ? (dw | (1<<0)) : dw );
		dw = (KPX_MON_HWSOUT02( hwsout ) ? (dw | (1<<1)) : dw );
		dw = (KPX_MON_HWSOUT04( hwsout ) ? (dw | (1<<2)) : dw );
/*--------------------------------------------------------------------------------------------------*/
	}
	else
	{	/*	2軸目	*/
/*--------------------------------------------------------------------------------------------------*/
	 	hwsout.dw = SerialOutput.SerialSignalOut.dw;
/*--------------------------------------------------------------------------------------------------*/
//		dw = (KPX_MON_HWSOUT00( hwsout ) ? (dw | (1<<0)) : dw );
//		dw = (KPX_MON_HWSOUT01( hwsout ) ? (dw | (1<<1)) : dw );
//		dw = (KPX_MON_HWSOUT02( hwsout ) ? (dw | (1<<2)) : dw );
//		dw = (KPX_MON_HWSOUT03( hwsout ) ? (dw | (1<<3)) : dw );
//		dw = (KPX_MON_HWSOUT04( hwsout ) ? (dw | (1<<4)) : dw );
//		dw = (KPX_MON_HWSOUT05( hwsout ) ? (dw | (1<<5)) : dw );
//		dw = (KPX_MON_HWSOUT06( hwsout ) ? (dw | (1<<6)) : dw );
//		dw = (KPX_MON_HWSOUT07( hwsout ) ? (dw | (1<<7)) : dw );
		dw = (KPX_MON_HWSOUT01( hwsout ) ? (dw | (1<<0)) : dw );
		dw = (KPX_MON_HWSOUT03( hwsout ) ? (dw | (1<<1)) : dw );
		dw = (KPX_MON_HWSOUT05( hwsout ) ? (dw | (1<<2)) : dw );
/*--------------------------------------------------------------------------------------------------*/
	}

#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
 	hwsout.dw = (( ax == 0 )  ? SerialOutput.SerialSignalOut.dw : 0/* Zantei */ );
/*--------------------------------------------------------------------------------------------------*/
	dw = (KPX_MON_HWSOUT00( hwsout ) ? (dw | (1<<0)) : dw );
	dw = (KPX_MON_HWSOUT01( hwsout ) ? (dw | (1<<1)) : dw );
	dw = (KPX_MON_HWSOUT02( hwsout ) ? (dw | (1<<2)) : dw );
	dw = (KPX_MON_HWSOUT03( hwsout ) ? (dw | (1<<3)) : dw );
	dw = (KPX_MON_HWSOUT04( hwsout ) ? (dw | (1<<4)) : dw );
	dw = (KPX_MON_HWSOUT05( hwsout ) ? (dw | (1<<5)) : dw );
	dw = (KPX_MON_HWSOUT06( hwsout ) ? (dw | (1<<6)) : dw );
	dw = (KPX_MON_HWSOUT07( hwsout ) ? (dw | (1<<7)) : dw );
/*--------------------------------------------------------------------------------------------------*/
#endif
		return (dw);
}

/****************************************************************************/
/* 概要：シーケンス入力信号読み出し処理										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:																*/
/****************************************************************************//* <S1C0> */
ULONG HALdrv_ReadMonAllInputSignals( void )
{
HWSIN hwsin;
ULONG dw = 0;
 	hwsin.dw = SerialInput.UnInSignal.dw;
	dw = (KPX_MON_HWSIN00( hwsin ) ? (dw | (1<<0)) : dw );
	dw = (KPX_MON_HWSIN01( hwsin ) ? (dw | (1<<1)) : dw );
	dw = (KPX_MON_HWSIN02( hwsin ) ? (dw | (1<<2)) : dw );
	dw = (KPX_MON_HWSIN03( hwsin ) ? (dw | (1<<3)) : dw );
	dw = (KPX_MON_HWSIN04( hwsin ) ? (dw | (1<<4)) : dw );
	dw = (KPX_MON_HWSIN05( hwsin ) ? (dw | (1<<5)) : dw );
	dw = (KPX_MON_HWSIN06( hwsin ) ? (dw | (1<<6)) : dw );
	dw = (KPX_MON_HWSIN07( hwsin ) ? (dw | (1<<7)) : dw );
	dw = (KPX_MON_HWSIN08( hwsin ) ? (dw | (1<<8)) : dw );
	dw = (KPX_MON_HWSIN09( hwsin ) ? (dw | (1<<9)) : dw );
	dw = (KPX_MON_HWSIN10( hwsin ) ? (dw | (1<<10)) : dw );
	dw = (KPX_MON_HWSIN11( hwsin ) ? (dw | (1<<11)) : dw );
	
/*--------------------------------------------------------------------------------------------------*/
	return dw;
}

/****************************************************************************/
/* 概要：シーケンス出力信号読み出し処理										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:																*/
/****************************************************************************/
ULONG	HALdrv_ReadMonAllOutputSignals( void )
{
	HWSOUT hwsout;
	ULONG dw = 0;
 	hwsout.dw = SerialOutput.SerialSignalOut.dw;
/*--------------------------------------------------------------------------------------------------*/
	dw = (KPX_MON_HWSOUT00( hwsout ) ? (dw | (1<<0)) : dw );
	dw = (KPX_MON_HWSOUT01( hwsout ) ? (dw | (1<<1)) : dw );
	dw = (KPX_MON_HWSOUT02( hwsout ) ? (dw | (1<<2)) : dw );
	dw = (KPX_MON_HWSOUT03( hwsout ) ? (dw | (1<<3)) : dw );
	dw = (KPX_MON_HWSOUT04( hwsout ) ? (dw | (1<<4)) : dw );
	dw = (KPX_MON_HWSOUT05( hwsout ) ? (dw | (1<<5)) : dw );
	dw = (KPX_MON_HWSOUT06( hwsout ) ? (dw | (1<<6)) : dw );
	dw = (KPX_MON_HWSOUT07( hwsout ) ? (dw | (1<<7)) : dw );

		return (dw);
}

/****************************************************************************/
/* 概要：RSW/DSW用I/Oデバイス初期化											*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_InitIODevices( void )
{
	/* シリアルI/O初期化 */
	HALdrv_InitSerialIO();
}

/****************************************************************************/
/* 概要：H/W同期Enable														*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_EnableExtSyncInterrupt( void *pAsicHwReg )
{
	SHalDrv_SetEnableComsyncsel(pAsicHwReg);
}

/****************************************************************************/
/* 概要：H/W同期Disable														*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_DisableExtSyncInterrupt( void *pAsicHwReg )
{
	SHalDrv_SetDisableComsyncsel(pAsicHwReg);
}

/****************************************************************************/
/* 概要：H/W同期状態取得													*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	TRUE ：非同期、 FALSE：同期									*/
/* <S052>:	ｿｰｽ整理実施(処理を明確に)										*/
/****************************************************************************/
BOOL HALdrv_GetExtUnSyncStatus( void )
{
	LONG int_chek;
	BOOL ret, ret1,ret2;
	
	/* TRUE:割込みあり、FALSE:割込みなし	*/
	int_chek = INTUDL1;
	ret1 = GetIntUDLStatus(int_chek);	/* 1:INTUDL1 - ScanB */
	int_chek = INTUDL2;
	ret2 = GetIntUDLStatus(int_chek);   /* 2:INTUDL2 - ScanC */

	/* TRUE ：非同期、 FALSE：同期　*/
	if( (ret1 != FALSE)&&(ret2 != FALSE) )
	{
		ret = FALSE;
	}
	else
	{
		ret = TRUE;
	}
	return ret;
}

/****************************************************************************/
/* 概要：H/W割込み状態取得													*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	TRUE ：非同期、 FALSE：同期									*/
/****************************************************************************/
BOOL HALdrv_GetIntUDLStatus( UCHAR IntName, VOID* AsicHwReg)
{
	BOOL ret;
	
	if( IntName == INTERRUPT_IntSync )
	{
#if (CSW_NETIF_TYPE != NETIF_M2_TYPE)
		ret = GetIntUDLStatus( INTUDL_ML );		/* 1:INTUDL4,7 - IntSync */
#else
		ret = SHalDrv_Ack_MEHCATROLINK2Interrupt(AsicHwReg);
#endif
	}
	else if( IntName == INTERRUPT_ScanA )
	{
		ret = GetIntUDLStatus( INTUDL0 );		/* 5:INTUDL0 - ScanA */
	}
	else if( IntName == INTERRUPT_ScanB )
	{
		ret = GetIntUDLStatus( INTUDL1 );		/* 6:INTUDL1 - ScanB */
	}
	else if( IntName == INTERRUPT_ScanC )
	{
		ret = GetIntUDLStatus( INTUDL2 );		/* 7:INTUDL2 - ScanC */
	}
	else
	{
		ret = FALSE;			/* 未初期化変数修正<S191> */
	}
	
	return ret;
}
/****************************************************************************/
/* 概要：H/W割込みクリア要求												*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	TRUE ：非同期、 FALSE：同期									*/
/****************************************************************************/
VOID HALdrv_ReqIntUDLStatusClear( UCHAR IntName )
{

	if( IntName == INTERRUPT_IntSync )
	{
#if  (CSW_NETIF_TYPE != NETIF_M2_TYPE)
		ReqIntUDLStatusClear( INTUDL_ML );		/* 1:INTUDL4,7 - IntSync 		*/
#else
		;										/* M2は、割込み確認で自動ｸﾘｱ	*/
#endif
	}
	else if( IntName == INTERRUPT_ScanA )
	{
		ReqIntUDLStatusClear( INTUDL0 );		/* 5:INTUDL0 - ScanA			*/
	}
	else if( IntName == INTERRUPT_ScanB )
	{
		ReqIntUDLStatusClear( INTUDL1 );		/* 6:INTUDL1 - ScanB 			*/
	}
	else if( IntName == INTERRUPT_ScanC )
	{
		ReqIntUDLStatusClear( INTUDL2 );		/* 7:INTUDL2 - ScanC 			*/
	}

	return;
}


#if 0 /* 不使用 <S1D2> */
/****************************************************************************/
/* 概要：Self Reset処理														*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_OutputSoftwareResetSignal( void )
{
	WriteIODeviceRegister( IOPldRegDev, 0x001F, 0xA5A5 );
}
#endif



#if 0 /* 不使用 <S1D2> */
/****************************************************************************/
/* 概要：サーボASIC同期設定													*/
/****************************************************************************/
/* 引数		:																*/
/*	BaseCycle	:	ScanB周期[ns]											*/
/*	SyncCycle	:	サーボASIC同期周期[ScanB周期](0：同期無効)				*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_EnableServoAsicSync(UINT BaseCycle, UINT SyncCycle )
{
	ULONG cycle;
	cycle = (ULONG)((BaseCycle/KPI_SACYCLENS) - 1);

	MP_SetMHINTTimer(IOPldRegDev, 1);	/* MHINTタイマに１分周をセット */
	MP_ResetHINT(IOPldRegDev);			/* HINTタイマー同期開始 */

	/* Set synchronization cycle to "pldHscanInt" */
#ifndef	CSW_CSTR_MBB_TYPE	// for MBA board
	WriteIODeviceRegister( IOPldHscanDev, 0, SyncCycle );
#else						// for MBB board
	WriteIODeviceRegister( IOPldHscanDev, 1, SyncCycle );
#endif
	/* Set HINT cycle to "pldDev" */
	WriteIODeviceRegister( IOPldRegDev, 0x0010, 0x8000UL | cycle);
}
#endif



/****************************************************************************/
/* 概要：伝送バイト数(ディップSW設定)取得処理								*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	伝送バイト数設定値											*/
/****************************************************************************/
UCHAR HALdrv_GetTransmissionByte( void )
{
	UCHAR	DataSize;
	ULONG	switch_sts;

#ifndef _TEST_BOARD_ /* 本基板用 */

	/* Get DipSW and RotarySW Status */
#if(PRODUCT_SERIES_SEVEN == FALSE) /* <S09F> */
    ULONG   lwk0;
    lwk0=HW_SIN_GET( );
	GPIO_InputDirection_PM3();/*GPIO 0  TO  GPIO 7==input*/
	GPIO_InputDirection_PM4();/*GPIO 10 TO  GPIO 17==input*/
	lwk0=SerialtoGPIOconvert(lwk0);

	switch_sts = ~(lwk0);
	switch_sts = (switch_sts & DATA_SIZE_MSK) >> DATA_SIZE_BIT_SFT;
	DataSize = (UCHAR)((switch_sts + 1) * DATA_SIZE_16BYTE);
#else
	switch_sts = ~(HW_SIN_GET( ));
	switch_sts = (switch_sts & DATA_SIZE_MSK) >> DATA_SIZE_BIT_SFT;
	DataSize = (UCHAR)((switch_sts + 1) * DATA_SIZE_16BYTE);
#endif
#else /* _TEST_BOARD_ */ /* 以下削除予定 */
	/* Get DipSW and RotarySW Status */
	ReadIODeviceRegister(IOPldRegDev, IN0_REGISTER_DSW, (Value *)&switch_sts );

/* 暫定処理 本基板に合わせて修正する必要あり	HWSINを使うべき */
	switch_sts = (~switch_sts) >> DATA_SIZE_BIT_SFT;
	switch ( switch_sts & DATA_SIZE_MSK )
	{
	case 0x00: /* bit0:OFF, bit1:OFF */
		DataSize = (UCHAR)DATA_SIZE_16BYTE;	break;
	case 0x01: /* bit0:ON, bit1:OFF */
		DataSize = (UCHAR)DATA_SIZE_32BYTE;	break;
	case 0x02: /* bit0:OFF, bit1:ON */
		DataSize = (UCHAR)DATA_SIZE_48BYTE;	break;
	case 0x03: /* bit0:ON, bit1:ON */
		DataSize = (UCHAR)DATA_SIZE_64BYTE;	break;
	default: /* 本ケースはなし */
		DataSize = (UCHAR)DATA_SIZE_48BYTE;	break;
	}
#endif

	return DataSize;
}

/****************************************************************************/
/* 概要：ディップSW設定取得処理												*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	伝送バイト数設定値											*/
/****************************************************************************/
ULONG HALdrv_GetDipSwitchSetting( void )
{
	ULONG	switch_sts;
	/* Get DipSW and RotarySW Status */
	switch_sts = SerialInput.SerialSignalIn.dw;
	switch_sts = (switch_sts & DIP_SW_MSK) >> DIP_SW_SFT;
	return ( switch_sts );
}

/****************************************************************************/
/* 概要：局アドレス(ロータリSW設定)取得処理									*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	局アドレス設定値											*/
/****************************************************************************/
UCHAR HALdrv_GetStationAddress( void )
{
	UCHAR	Address;
	ULONG	switch_sts;

#ifndef _TEST_BOARD_ /* 本基板用 */
#if(PRODUCT_SERIES_SEVEN == FALSE) /* <S09F> */
    ULONG   lwk0;
    lwk0 = HW_SIN_GET( );
	GPIO_InputDirection_PM3();/*GPIO 0  TO  GPIO 7==input*/
	GPIO_InputDirection_PM4();/*GPIO 10 TO  GPIO 17==input*/
	lwk0 = SerialtoGPIOconvert(lwk0);
	/* Get DipSW and RotarySW Status */
	switch_sts = lwk0;
#else
	switch_sts = HW_SIN_GET( );
#endif
//	Address = (UCHAR)((switch_sts & STATION_ADDRESS_MSK) >> STATION_ADDRESS_SFT);
	/* HWSIN 4-7:SW1,8-11:SW2	→	Address 下位:SW2,上位:SW1					*/
	Address = (UCHAR)((switch_sts & ROTALY_SW1_MSK)
						|((switch_sts & ROTALY_SW2_MSK) >> ROTALY_SW2_SFT));

#else /* _TEST_BOARD_ */ /* 削除予定 */
	/* Get DipSW and RotarySW Status */
	ReadIODeviceRegister(IOPldRegDev, IN1_REGISTER_RSW, (Value *)&switch_sts );
/* 暫定処理 本基板に合わせて修正する必要あり	HWSINを使うべき */
	Address = (UCHAR)(switch_sts);
#endif

	return Address;
}


/****************************************************************************/
/* 概要：ハード依存ネットワークパラメータ取得処理							*/
/****************************************************************************/
/* 引数		:	伝送バイト数、伝送速度、局アドレス							*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_GetNetworkHardPrm(UCHAR *tByte, UCHAR *tSpeed, UCHAR *tAddress)
{
	UCHAR	dsw,rsw;
	ULONG	switch_sts;

#if(PRODUCT_SERIES_SEVEN == FALSE) /* <S09F> */
    ULONG   lwk0;
    lwk0 = HW_SIN_GET( );
	GPIO_InputDirection_PM3();/*GPIO 0  TO  GPIO 7==input*/
	GPIO_InputDirection_PM4();/*GPIO 10 TO  GPIO 17==input*/
	lwk0 = SerialtoGPIOconvert(lwk0);
	/* Get DipSW and RotarySW Status */
	switch_sts = lwk0;
#else
	switch_sts = HW_SIN_GET( );
	/* 暫定処理 本基板に合わせて修正する必要あり	HWSINを使うべき */
#endif
	dsw = (UCHAR)(~(switch_sts & 0x0000F000) >> 12);
	/* 伝送速度	*/
	if(dsw & M2_TRANSMISSION_RATE_MASK )
	{/* 伝送速度 10Mbps	*/
		*tSpeed = TRANS_RATE_10MBPS;
	}
	else
	{/* 伝送速度 4Mbps	*/
		*tSpeed = TRANS_RATE_4MBPS;
	}

	/* 伝送バイト数	*/
	if(dsw & M2_TRANSMISSION_BYTE_MASK )
	{/* 伝送バイト数 32Byte	*/
		*tByte = DATA_SIZE_32BYTE;
	}
	else
	{/* 伝送バイト数 17Byte	*/
		*tByte = DATA_SIZE_17BYTE;
	}

	/* 自局アドレス	*/
	rsw = (UCHAR)((switch_sts & 0x00000F00) >> 8);
	if(dsw & M2_TRANSMISSION_ADDRESS_MASK )
	{/* ロータリーSW＋50H	*/
		*tAddress =  (UCHAR)(rsw) + TRANS_ADR_OFFSET_50H;
	}
	else
	{/* ロータリーSW＋40H	*/
		*tAddress =  (UCHAR)(rsw) + TRANS_ADR_OFFSET_40H;
	}
}



#if 0 /* 不使用 <S1D2> */
/****************************************************************************/
/* 概要：LED ON/OFF処理														*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_OutputLED(INT LedOutputs )
{
	LedOutputs = (LedOutputs ^ LED_OUTPUT_MASK) & LED_OUTPUT_MASK;
	WriteIODeviceRegister( IOPldRegDev, 0, LedOutputs );
}
#endif



/****************************************************************************/
/* 概要：同期シリアルI/Oデータ初期化処理									*/
/****************************************************************************/
/* 引数		:	void* ChkPostRegs	Register address of the SI signals		*/
/*				LONG Setting		Micro ScanA Output Selection			*/
/* 戻り値	:	なし														*/
/****************************************************************************/
#ifndef _VC_DEBUG_
#define DEBUG_PORT_REGADDR			0xEFFF8010
#define DEBUG_PORT_MODE_REGADDR		0xEFFF8110
#else
#define DEBUG_PORT_REGADDR			&(DummyRegAddress)
#define DEBUG_PORT_MODE_REGADDR		&(DummyRegAddress)
#endif
void HALdrv_InitCheckPost( void )
{
	VULONG*	DebugPortModeRegAddr;

	HALdrv_CheckPost = (VULONG*)DEBUG_PORT_REGADDR;
	DebugPortModeRegAddr = (VULONG*)DEBUG_PORT_MODE_REGADDR;

	/* Initialize Port Output */
//-	*HALdrv_CheckPost = 0x00000008;			/* <S106> */ /*SW_RSTのI/Oポート初期化は行わない*/
	 /* P40 - P47 Ports Output Data Setting *//* <S1AC> */
	*HALdrv_CheckPost = 0x0000000B;	/* P40(EEP0_WP)=P41(EEP1_WP)=H(プロテクトへ) */

	/* Set P4 Signal to Output Port */
	*DebugPortModeRegAddr = 0x00000000; /* P40 - P47 Ports are Output Setting */

	return;
}

/****************************************************************************/
/* 概要：セーフティ機能　内部信号異常検出用パルス出力処理		<S0F1>		*/
/*       4ms周期パルス出力(ScanC処理)										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_ReverceTestPls( void )
{
	*HALdrv_CheckPost ^= (1<< SIL3_TEST_PLS);
	return;
}

/****************************************************************************/
/* 概要：セーフティ機能　内部信号異常検出用パルス出力アドレス取得処理		<S1F6>		*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	内部信号異常検出用パルス出力アドレス									*/
/****************************************************************************/
VULONG *HALdrv_GetCheckPostAddr( void )
{
	return HALdrv_CheckPost;
}


/*******************************************************************************
*  Function    : HALdrv_GpioOutput
*  Description : Gpio出力設定
*  Format      :
*  Parameter   :
*  Return      :
*  Update History
*    Date        Author         Description of Change
*   ----------  --------------  ---------------------
*   2010.05.11  Y.Oka           Created
*******************************************************************************/
void HALdrv_GpioOutput( BOOL sts, LONG IoNo )
{
	/* ---futami---> */
//	HALdrv_CheckPost[IoNo] = sts;
	/* <---futami--- */
}


/****************************************************************************/
/* 概要：IF基板 Debug port出力処理											*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_OutputDebugSP(UINT bit_no)
{
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
	UCHAR	SftNo;
	SftNo = bit_no + 2; /* Offset to P32 */
	*HALdrv_CheckPost |= (1 << SftNo);
#elif !defined(_VC_DEBUG_)
	*HALdrv_CheckPost |= (1 << bit_no);
#endif
	return;
}

void HALdrv_ClearDebugSP(UINT bit_no)
{
#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
	UCHAR	SftNo;
	SftNo = bit_no + 2; /* Offset to P32 */
	*HALdrv_CheckPost &= ~(1 << SftNo);
#elif !defined(_VC_DEBUG_)
	*HALdrv_CheckPost &= ~(1 << bit_no);
#endif
	return;
}
#endif	/*_MERCURY_PLATFORM_*/	/*<S1A2>*/


#if 0 /* 2013.07.14 Y.Oka Mercury用処理に変更必要 */
/****************************************************************************/
/* 概要：基板ID読み出し														*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
ULONG HALdrv_ReadBoardID( HW_BOARDID_T Id )
{
	ULONG	idval, tmp;

	switch(Id)
	{
	case HW_MB_MODULE:
		idval = (ULONG)SerialInput.SinRegAddress[1];
		break;

	case HW_FBB_MODULE:
		tmp = SerialOutput.IdReadReqOut;
		SerialOutput.IdReadReqOut = tmp | BIT15;
		KlibWaitus( 500 );
		idval = (ULONG)SerialInput.SinRegAddress[1];
		SerialOutput.IdReadReqOut = tmp;
		break;

	default:
		idval = 0;
		break;
	}
	return idval;
}
#endif
/*<S1A2>*/
/****************************************************************************/
/* 概要：基板ID読み出し														*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	ボードID (BOARD_ID_xxx)										*/
/****************************************************************************/
ULONG HALdrv_ReadBoardID( void )
{
	ULONG	idval;

	/* 基板IDを取得し、連番(0～)に補正して返す */
	idval = *((ULONG *)GPIO_BOARD_ID_ADDRESS) & GPIO_BOARD_ID_MASK;
	idval >>= BOARD_ID_SHT_NUM;

	return idval;
}


#if 0 /* 2013.11.17 Y.Oka OH処理はMDと異なるため削除 */
/* ---futami---> */
/****************************************************************************/
/* 概要：OH_LED制御用データ設定処理											*/
/****************************************************************************/
/* 引数		:	BOOL sts		OH_LED制御ステータス ON/OFF					*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_OhLedControl( BOOL sts )
{
#if 0 /* 2012.09.07 Y.Oka for Mercury Base (only INGRAM) */
	/* COA Output Signal for ScanC											*/
	SerialOutput.RelaySigOut.flag.OhLed = sts;
#endif
}
#endif


/* <---futami--- */

/* ---futami---> */
/****************************************************************************/
/* 概要：IF基板WDTトリガ設定処理											*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_KickIfWdt( void )
{
#if 0 /* 2012.09.07 Y.Oka for Mercury Base (only INGRAM) */
	WriteIODeviceRegister(IOPldRegDev, OUT7_REGISTER, 0xffff );
#endif
}

/* <---futami--- */

/* ---futami (R)S/W Version Check---> */
/****************************************************************************/
/* 概要：IF基板Flash先頭アドレス取得										*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_GetNorflashRegister( void *RegisterAddress )
{
#if 0 /* 2012.09.07 Y.Oka for Mercury Base (only INGRAM) */
	HALdrv_NorflashAddress = (VULONG*)RegisterAddress;
#endif
}


/****************************************************************************/
/* 概要：(R)Bootsoft Version / Bootstrap Version取得						*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
ULONG HALdrv_GetBootVersion( UINT Index, BOOT_VERSION Val )
{
#if 0 /* 2012.09.07 Y.Oka for Mercury Base (only INGRAM) */

	VULONG	*DataPtr;
	
	switch( Val )
	{
		case R_BOOTSOFT_VERSION:
			DataPtr = (VULONG*)( (UCHAR*)HALdrv_NorflashAddress + BOOT_VERSION_OFS + Index * 4 );
			break;
			
		case R_BOOTSTRAP_VERSION:
			DataPtr = (VULONG*)( (UCHAR*)HALdrv_NorflashAddress + BOOTSTRAP_VERSION_OFS + Index * 4 );
			break;
	}
	return *DataPtr;
#else
	return 0;
#endif
}


/* <---futami (R)S/W Version Check--- */



/****************************************************************************/
/* 概要：M-IIIμプログラムのダウンロード選択(2軸一体用)		<S0E4>			*/
/****************************************************************************/
/* 引数		:	M-IIIμプログラムダウンロード選択	TRUE：あり、FALSE：なし	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_SetM3PrgSel( BOOL sel )
{
	ULONG	*CpuPort;
	CpuPort = (ULONG*)PRGSEL_CTRL_ADR;

	if( sel == TRUE )
	{
		*CpuPort |= PRGSEL;
	}
	else
	{
		; //処理なし
	}
	return;
}


/* <S198> */
/****************************************************************************/
/* 概要：強制出力モード用マスク処理											*/
/****************************************************************************/
/* 引数		:	なし														*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void HALdrv_ForceOutSignalsMask( void )
{
	SerialOutput.RelaySigOut.dw    &= 0xF00F;
	SerialOutput.RegeneSigOut.dw   &= 0xF00F;
	SerialOutput.LayoutSigOut.dw   &= 0xF00F;
	SerialOutput.AlarmSigOut.dw    &= 0xF00F;
	SerialOutput.SignalScanCOut.dw &= 0xF00F;
}

/****************************************************************************************************/
/*																									*/
/*		<S1B5> Bit Number Conversion																*/
/*																									*/
/****************************************************************************************************/
LONG 	HALdrv_GetPsBitNumber( LONG Pin )
{
LONG	Bit;
/*--------------------------------------------------------------------------------------------------*/
	switch( Pin )
	{
	/*----------------------------------------------------------------------------------------------*/
	case KPI_SI0PINNO : 	Bit = KPI_SI0BITNO; 	break;
	case KPI_SI1PINNO : 	Bit = KPI_SI1BITNO; 	break;
	case KPI_SI2PINNO : 	Bit = KPI_SI2BITNO; 	break;
	case KPI_SI3PINNO : 	Bit = KPI_SI3BITNO; 	break;
	case KPI_SI4PINNO : 	Bit = KPI_SI4BITNO; 	break;
	case KPI_SI5PINNO : 	Bit = KPI_SI5BITNO; 	break;
	/*----------------------------------------------------------------------------------------------*/
	case KPI_SI6PINNO : 	Bit = KPI_SI6BITNO; 	break;
	case KPI_SI7PINNO : 	Bit = KPI_SI7BITNO; 	break;
	case KPI_SI8PINNO : 	Bit = KPI_SI8BITNO; 	break;
	case KPI_SI9PINNO : 	Bit = KPI_SI9BITNO; 	break;
	case KPI_SI10PINNO : 	Bit = KPI_SI10BITNO; 	break;
	case KPI_SI11PINNO : 	Bit = KPI_SI11BITNO; 	break;
	/*----------------------------------------------------------------------------------------------*/
	default: 				Bit = KPI_PINNO_NA; 	break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return Bit;
}


/****************************************************************************************************/
/*																									*/
/*		<S1B5> Bit Number Conversion																*/
/*																									*/
/****************************************************************************************************/
LONG 	HALdrv_GetSpBitNumber( LONG Pin )
{
LONG Bit;
/*--------------------------------------------------------------------------------------------------*/
	switch( Pin )
	{
	/*----------------------------------------------------------------------------------------------*/
	case KPI_SO1PINNO : 	Bit = KPI_SO1BITNO; 	break;
	case KPI_SO2PINNO : 	Bit = KPI_SO2BITNO; 	break;
	case KPI_SO3PINNO : 	Bit = KPI_SO3BITNO; 	break;
	case KPI_SO4PINNO : 	Bit = KPI_SO4BITNO; 	break;
	case KPI_SO5PINNO : 	Bit = KPI_SO5BITNO; 	break;
	case KPI_SO6PINNO : 	Bit = KPI_SO6BITNO; 	break;
	/*----------------------------------------------------------------------------------------------*/
	default: 				Bit = KPI_PINNO_NA; 	break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return Bit;
}

/***************************************** end of file **********************************************/
