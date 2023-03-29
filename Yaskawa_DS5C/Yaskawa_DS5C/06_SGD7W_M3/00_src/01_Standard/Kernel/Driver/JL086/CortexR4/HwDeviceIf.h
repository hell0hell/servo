/****************************************************************************************************/
/*																									*/
/*																									*/
/*		HwDeviceIf.h : Σ-7S,W用H/Wアクセス定義														*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.04.12	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*				Ver2.00		2013.07.14 Y.Oka Changed for Mercury									*/
/*																									*/
/****************************************************************************************************/
#ifndef _HW_DEVICE_IF_H
#define _HW_DEVICE_IF_H

#include "ARMPF_SCU.h"//<S0C7>

/* defines for Board Memory map information */
#ifndef _TEST_BOARD_
#define HW_SVASIC_ADDRESS 			0xC0000000		/* JL086 Servo UDL Address */
#define HW_M3UDL_ADDRESS 			0xD0000000		/* JL086 M3 UDL Address *//*<S008>*/
#define HW_SRAM_ADDRESS 			0xE0000000		/* JL086 netX100(SRAM) Address *//*<S008>*/

#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)	/* MECATROLINK- M2	*/
#define HW_NETLSI_BASE_ADDRESS		HW_SVASIC_ADDRESS		/* JL086 Servo UDL Address */
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)	/*  MECATROLINK-3	*/
#define HW_NETLSI_BASE_ADDRESS		HW_M3UDL_ADDRESS		/* JL086 M3 UDL Address *//*<S008>*/
#elif (CSW_NETIF_TYPE == NETIF_CM_TYPE)	/*  Command Option	*//*<S008>*/
#define HW_NETLSI_BASE_ADDRESS		HW_SRAM_ADDRESS			/* JL086 netX100(SRAM) Address *//*<S008>*/
#else
#define HW_NETLSI_BASE_ADDRESS		(NULL) 					/* <S00C> */
#endif


#else /* _TEST_BOARD_ */

/* 評価ボード用定義 */ /* @@最終的には削除 */
#define HW_SVASIC_ADDRESS 			0x08000000		/* JL076 Servo ASIC Address for Axis0 */
//#define HW_SVASIC_ADDRESS 			0x09000000		/* JL076 Servo ASIC Address for Axis1 */
#define JL086_SVIP_OFS				0x01000000		/* 1軸毎のASICレジスタオフセット数	*/

#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)	/* MECATROLINK- M2	*/
#define HW_NETLSI_BASE_ADDRESS		0x08000000		/* JL076 Base Address							*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)	/*  MECATROLINK-3	*/
#define HW_NETLSI_BASE_ADDRESS		0x10000000		/* JL-100 Base Address							*/
#endif

#endif /* _TEST_BOARD_ */

/* <S1A2> >>>>> ボードID取得関連定義 */
#define GPIO_BOARD_ID_ADDRESS		0xEFFF8500		/* ボードID取得用GPIOアドレス					*/
#define GPIO_BOARD_ID_MASK			0x0000000E		/* ボードID部マスクパターン						*/
#define BOARD_ID_SHT_NUM			1				/* 右シフト数(0からの連番とするため)			*/
typedef enum BOARD_ID_T {	/* ボードID(0からの連番) */
	BOARD_ID_S_AP,					/* S アナログパルス形		*/
	BOARD_ID_S_OPT,					/* S 指令オプション取付形	*/
	BOARD_ID_S_M3,					/* S MECHATROLINK-III通信形	*/
	BOARD_ID_S_M2,					/* S MECHATROLINK-II通信形	*/
	BOARD_ID_S_ECAT,				/* S EtherCAT通信形			*/
	BOARD_ID_W_M3,					/* W MECHATROLINK-III通信形	*/
	BOARF_ID_RSV1,					/* 予約						*/
	BOARF_ID_RSV2					/* 予約						*/
} BOARD_ID_T;
/* <<<<< <S1A2> */

/* Check Post Setting Address */
#define	CHECK_POST_ADDRESS			0x90100
#define CHKPOST_SETTING_ADDRESS		0x04

#ifndef _MERCURY_PLATFORM_	/*<S146>*/
/* Axis Mapping Setting Address */
#define HW_AXIS_MAP_ADDERSS_OFS		0x00090400
typedef struct HW_AXIS_MAP{
	USHORT	asicNo[MAX_AXIS_NUMBER];
	void	*asicAddr[MAX_AXIS_NUMBER];
	LONG	asicNum;
} HW_AXIS_MAP;
#endif	/* _MERCURY_PLATFORM_ */


/* M-II/M-III 通信設定(DipSW/RotarySWによる) */
#define DATA_SIZE_16BYTE		16			/* 伝送バイト数 16byte									*/
#define DATA_SIZE_17BYTE		17			/* 伝送バイト数 17byte									*/
#define DATA_SIZE_32BYTE		32			/* 伝送バイト数 32byte									*/
#define DATA_SIZE_48BYTE		48			/* 伝送バイト数 48byte									*/
#define DATA_SIZE_64BYTE		64			/* 伝送バイト数 64byte									*/

#define TRANS_RATE_4MBPS		4			/* MECHATROLINK-I  伝送速度								*/
#define TRANS_RATE_10MBPS		10			/* MECHATROLINK-II 伝送速度								*/

#define TRANS_ADR_OFFSET_40H	0x40		/* 局アドレスオフセット(40H) for M2						*/
#define TRANS_ADR_OFFSET_50H	0x50		/* 局アドレスオフセット(50H) for M2						*/
#define LOWER_INVALID_ADDRESS	0x02		/* 無効アドレス設定値(0x02以下は無効)					*/
#define UPPER_INVALID_ADDRESS	0xF0		/* 無効アドレス設定値(0x02以上は無効)					*/



#define	M2_TRANSMISSION_RATE_MASK		0x01
#define	M2_TRANSMISSION_BYTE_MASK		0x02
#define	M2_TRANSMISSION_ADDRESS_MASK	0x04



/* LED 制御設定 */
typedef enum LED_BIT_T {
	LED_RDY_BITNO = 0,
	LED_ERR_BITNO,
	LED_CNCT_BITNO,
	LED_CMERR_BITNO,
	LED_LINK1_BITNO,
	LED_LINK2_BITNO,
	LED_RESERVE1_BITNO,
	LED_LINKSRC_BITNO,
}LED_BIT_T;
#define LED_DEFAULT_OUT		(1<<LED_ERR_BITNO)
#define LED_OUTPUT_MASK		0x0000000F

/* ---futami---> */
/* 出荷試験用テストモード設定 */
typedef enum TEST_MODE {
	TEST_NONE = 0,
	TEST_IF_WDTKICK,
	TEST_ASIC_WDTSTOP,
	TEST_ASIC_WDTKICK,
	TEST_RELAY_ON,
	TEST_RELAY_OFF,
/* <S0CB>	Start */
	TEST_REGENE_ON,
	TEST_REGENE_OFF,
	TEST_DB_ON,
	TEST_DB_OFF,
	TEST_GATE_DRIVE_ON,
	TEST_GATE_DRIVE_OFF,
	TEST_GATE_DRIVE_ON_OFF_START,	/* <S19F> */
	TEST_GATE_DRIVE_ON_OFF_STOP,	/* <S19F> */
/* <S0CB>	End */
}TEST_MODE;
/* <---futami--- */



/* 5V Tolerant Buffer Standby Control <S01F> */
//#define BUF_5T_CTRL_ADR	0xEFF8602C	//<S0C7>重複定義。ARMPF_SCU.hに定義してある変数を使用する。
//#define OV_CNT			BIT0		//<S0C7>
//#define OC1_CNT			BIT1		//<S0C7>
//#define OC2_CNT			BIT2		//<S0C7>
//#define SYNCL_CNT			BIT3		//<S0C7>

#if(PRODUCT_SERIES_SEVEN == FALSE)    /* <S09F> */
#define  SigmaM_GPIO
#ifdef   SigmaM_GPIO
#define GPIO_P3_Output              (VLONG *)(0xEFFF800C)/*GPIO 0 to GPIO 7*/
#define GPIO_P4_Output              (VLONG *)(0xEFFF8010)/*GPIO 10 to GPIO 17*/
#define GPIO_PM3_Direction_Select   (VLONG *)(0xEFFF810C)/*GPIO 0 to GPIO 7*/
#define GPIO_PM4_Direction_Select   (VLONG *)(0xEFFF8110)/*GPIO 10 to GPIO 17*/


#define	GPIO_InputDirection_PM3( )	{ *GPIO_PM3_Direction_Select |= 0x000000FF; }   /* Select_Gpio_input		*/
#define	GPIO_OutputDirection_PM3( )	{ *GPIO_PM3_Direction_Select |= 0x00000000; }   /* Select_Gpio_output		*/
#define	GPIO_InputDirection_PM4( )	{ *GPIO_PM4_Direction_Select |= 0x000000DE; }   /* Select_Gpio_input		*/
#define	GPIO_OutputDirection_PM4( )	{ *GPIO_PM4_Direction_Select |= 0x00000000; }   /* Select_Gpio_output		*/
#define GPIO_Inputdata_P3()         ( *(VLONG*)(0xEFFF850C))
#define GPIO_Inputdata_P4()         ( *(VLONG*)(0xEFFF8510))

#define GPIO_OutputData_P3_High          {*GPIO_P3_Output  |= 0x000000FF;}
#define GPIO_OutputData_P3_low           {*GPIO_P3_Output  |= 0x00000000;}
#define GPIO_OutputData_P4_High          {*GPIO_P4_Output  |= 0x000000FF;}
#define GPIO_OutputData_P4_low           {*GPIO_P4_Output  |= 0x00000000;}

#define HW_SOUT_SET_H(out)		     *(VULONG*)(0xC0000000 + 0x0000000CUL) = (ULONG)(out)

#define HW_SOUT_Serial_H(out)		 *(VULONG*)(0xC0000000 + 0x000000E4UL) = (ULONG)(out)

#define  HW_SIN_GET_H()             ( *(VLONG*)(0xC00000E4))
#endif
#endif
/* <S04D> ：Start*/
/* Check GPIO Signal Define	*/
#define GPIO0_PORT_REGADDR			0xEFFF850C	/* GPIO00 ～ GPIO07	*/
#define GPIO1_PORT_REGADDR			0xEFFF8510	/* GPIO10 ～ GPIO17	*/

typedef enum GPIO0_PORT_BIT {
	GPIO00 = 0,
	GPIO01,
	GPIO02,
	GPIO03,
	GPIO04,
	GPIO05,
	GPIO06,
	GPIO07,
}GPIO0_PORT_BIT;

typedef enum GPIO1_PORT_BIT {
	GPIO10 = 0,
	GPIO11,
	GPIO12,
	GPIO13,
	GPIO14,
	GPIO15,
	GPIO16,
	GPIO17,
}GPIO1_PORT_BIT;

#define HALDRV_GET_GPIO00	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO00))? FALSE:TRUE
#define HALDRV_GET_GPIO01	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO01))? FALSE:TRUE
#define HALDRV_GET_GPIO02	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO02))? FALSE:TRUE
#define HALDRV_GET_GPIO03	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO03))? FALSE:TRUE
#define HALDRV_GET_GPIO04	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO04))? FALSE:TRUE
#define HALDRV_GET_GPIO05	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO05))? FALSE:TRUE
#define HALDRV_GET_GPIO06	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO06))? FALSE:TRUE
#define HALDRV_GET_GPIO07	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO07))? FALSE:TRUE

#define HALDRV_GET_GPIO10	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO10))? FALSE:TRUE
#define HALDRV_GET_GPIO11	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO11))? FALSE:TRUE
#define HALDRV_GET_GPIO12	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO12))? FALSE:TRUE
#define HALDRV_GET_GPIO13	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO13))? FALSE:TRUE
#define HALDRV_GET_GPIO14	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO14))? FALSE:TRUE
#define HALDRV_GET_GPIO15	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO15))? FALSE:TRUE
#define HALDRV_GET_GPIO16	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO16))? FALSE:TRUE
#define HALDRV_GET_GPIO17	((*(UCHAR*)GPIO0_PORT_REGADDR) & (1<<GPIO17))? FALSE:TRUE

/* <S04D> ：End */


/* M-III PRGSEL Setting *//* <S0E4> */
#define PRGSEL_CTRL_ADR		0xEFF8604C
#define PRGSEL				BIT0

#if CSW_BOARD_TYPE == SGD7S_TYPE /* <S0A2> */
/****************************************************************************************************/
/*																									*/
/*		ハードウェア入力信号処理定義																*/
/*																									*/
/****************************************************************************************************/
#ifdef __BIG_ENDIAN__
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef _TEST_BOARD_ /* @@最終的には削除 */

#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	#define	KPX_HWSINXOR		0x01E00000				/* Input Signal Reverse Bits Data			*/
	#define	KPX_HWSOUTXOR		0xFFFFFFFF				/* Output Signal Reverse Bits Data			*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	#define	KPX_HWSINXOR		0x01E0F000				/* Input Signal Reverse Bits Data			*/
	#define	KPX_HWSOUTXOR		0xFFFFFFFF				/* Output Signal Reverse Bits Data			*/
#else /* NETIF_M2_TYPE / NETIF_CM_TYPE */
	#define	KPX_HWSINXOR		0x01E0F000				/* Input Signal Reverse Bits Data			*/
	#define	KPX_HWSOUTXOR		0xFFFFFFFF				/* Output Signal Reverse Bits Data			*/
#endif /* IF TYPE */

#else /* _TEST_BOARD_ */ /* @@最終的には以下削除 */
#define	KPX_HWSINXOR			0xFFFFFFFF				/* Input Signal Reverse Bits Data			*/
#define	KPX_HWSOUTXOR			0xFFFFFFFF				/* Output Signal Reverse Bits Data			*/
#endif /* _TEST_BOARD_ */ /* @@最終的には削除 */

/*--------------------------------------------------------------------------------------------------*/
/*		シリアル入力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	SI0				:1;			/* 31 : 入力信号0 (A/P:CN1-40pin, Network:CN1-13pin)	*/
		UINT	SI1				:1;			/* 30 : 入力信号1 (A/P:CN1-42pin, Network:CN1-7pin)		*/
		UINT	SI2				:1;			/* 29 : 入力信号2 (A/P:CN1-43pin, Network:CN1-8pin)		*/
		UINT	SI3				:1;			/* 28 : 入力信号3 (A/P:CN1-41pin, Network:CN1-9pin)		*/
		UINT	SI4				:1;			/* 27 : 入力信号4 (A/P:CN1-44pin, Network:CN1-10pin)	*/
		UINT	SI5				:1;			/* 26 : 入力信号5 (A/P:CN1-45pin, Network:CN1-11pin)	*/
		UINT	SI6				:1;			/* 25 : 入力信号6 (A/P:CN1-46pin, Network:CN1-12pin)	*/
		UINT	RegeneAlm		:1;			/* 24 : *Regene Alarm									*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	FanAlm			:1;			/* 23 : *FAN Alarm										*/
		UINT	Acon2			:1;			/* 22 : *AC電源入力2									*/
		UINT	Acon1			:1;			/* 21 : *AC電源入力1									*/
		UINT	SEN				:1;			/* 20 : SEN Signal (CN1-4)		(Only A/P)				*/
		UINT	BTsw11			:1;			/* 19 : POP:B-SW1				(Only A/P)				*/
		UINT	BTsw12			:1;			/* 18 : POP:B-SW2				(Only A/P)				*/
		UINT	BTsw13			:1;			/* 17 : POP:B-SW3				(Only A/P)				*/
		UINT	BTsw14			:1;			/* 16 : POP:B-SW4				(Only A/P)				*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	DPsw14			:1;			/* 15 : *Dip SW(4接点:LSB)		(Only Network)			*/
		UINT	DPsw13			:1;			/* 14 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw12			:1;			/* 13 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw11			:1;			/* 12 : *Dip SW(4接点:MSB)		(Only Network)			*/
		UINT	RTsw21			:1;			/* 11 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw22			:1;			/* 10 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw23			:1;			/*  9 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw24			:1;			/*  8 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	RTsw11			:1;			/*  7 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw12			:1;			/*  6 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw13			:1;			/*  5 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw14			:1;			/*  4 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	Reserve4		:1;			/*  3 : Reserve											*/
		UINT	Reserve2		:1;			/*  2 : Reserve											*/
		UINT	Reserve1		:1;			/*  1 : Reserve											*/
		UINT	Reserve0		:1;			/*  0 : Reserve											*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/
} HWSIN;

#else /*  __LITTLE_ENDIAN__ */

typedef	union HWSIN {					/* Serial Input Signal										*/
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	#define	KPX_HWSINXOR		0x01E00000				/* Input Signal Reverse Bits Data			*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	#define	KPX_HWSINXOR		0x01E0F000				/* Input Signal Reverse Bits Data			*/
#else /* NETIF_M2_TYPE / NETIF_CM_TYPE */
	#define	KPX_HWSINXOR		0x01E0F000				/* Input Signal Reverse Bits Data			*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		<S045> Input Mapping Data and Input Macro Definition										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_SI0BITNO		(31)
#define	KPI_SI1BITNO		(30)
#define	KPI_SI2BITNO		(29)
#define	KPI_SI3BITNO		(28)
#define	KPI_SI4BITNO		(27)
#define	KPI_SI5BITNO		(26)
#define	KPI_SI6BITNO		(25)
#define KPI_SI7BITNO		(-1)	/*	not available */
#define KPI_SI8BITNO		(-2)	/*	not available */
#define KPI_SI9BITNO		(-3)	/*	not available */
#define KPI_SI10BITNO		(-4)	/*	not available */
#define KPI_SI11BITNO		(-5)	/*	not available */
#define KPI_SINHIBITNO		(1)
#define KPI_SINLOBITNO		(0)
#define KPI_SINANDBITS		(0xFFFFFFF0)
#define KPI_SINSETBITS		(0x00000002)
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル入力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	Reserve0		:1;			/*  0 : Reserve											*/
		UINT	Reserve1		:1;			/*  1 : Reserve											*/
		UINT	Reserve2		:1;			/*  2 : Reserve											*/
		UINT	Reserve3		:1;			/*  3 : Reserve											*/
		UINT	RTsw14			:1;			/*  4 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw13			:1;			/*  5 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw12			:1;			/*  6 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw11			:1;			/*  7 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	RTsw24			:1;			/*  8 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw23			:1;			/*  9 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw22			:1;			/* 10 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw21			:1;			/* 11 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	DPsw11			:1;			/* 12 : *Dip SW(4接点:MSB)		(Only Network)			*/
		UINT	DPsw12			:1;			/* 13 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw13			:1;			/* 14 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw14			:1;			/* 15 : *Dip SW(4接点:LSB)		(Only Network)			*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	BTsw11			:1;			/* 16 : POP:B-SW4				(Only A/P)				*/
		UINT	BTsw12			:1;			/* 17 : POP:B-SW3				(Only A/P)				*/
		UINT	BTsw13			:1;			/* 18 : POP:B-SW2				(Only A/P)				*/
		UINT	BTsw14			:1;			/* 19 : POP:B-SW1				(Only A/P)				*/
		UINT	SEN				:1;			/* 20 : SEN Signal (CN1-4)		(Only A/P)				*/
		UINT	Acon1			:1;			/* 21 : *AC電源入力1									*/
		UINT	Acon2			:1;			/* 22 : *AC電源入力2									*/
		UINT	FanAlm			:1;			/* 23 : *FAN Alarm										*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	RegeneAlm		:1;			/* 24 : *Regene Alarm									*/
		UINT	IN6				:1;			/* 25 : 入力信号6 (A/P:CN1-46pin, Network:CN1-12pin)	*/
		UINT	IN5				:1;			/* 26 : 入力信号5 (A/P:CN1-45pin, Network:CN1-11pin)	*/
		UINT	IN4				:1;			/* 27 : 入力信号4 (A/P:CN1-44pin, Network:CN1-10pin)	*/
		UINT	IN3				:1;			/* 28 : 入力信号3 (A/P:CN1-41pin, Network:CN1-9pin)		*/
		UINT	IN2				:1;			/* 29 : 入力信号2 (A/P:CN1-43pin, Network:CN1-8pin)		*/
		UINT	IN1				:1;			/* 30 : 入力信号1 (A/P:CN1-42pin, Network:CN1-7pin)		*/
		UINT	IN0				:1;			/* 31 : 入力信号0 (A/P:CN1-40pin, Network:CN1-13pin)	*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/

} HWSIN;

#endif /* __BIG_ENDIAN__ */



/*--------------------------------------------------------------------------------------------------*/
/*		シリアル出力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef __BIG_ENDIAN__
typedef	union HWSOUT {					/* Serial Output Signal										*/
	struct {							/* ビット信号定義											*/
		UINT	Reserved31		:1;			/* 31 : Reserve											*/
		UINT	Reserved30		:1;			/* 30 : Reserve											*/
		UINT	Reserved29		:1;			/* 29 : Reserve											*/
		UINT	Reserved28		:1;			/* 28 : Reserve											*/
		UINT	Reserved27		:1;			/* 27 : Reserve											*/
		UINT	Reserved26		:1;			/* 26 : Reserve											*/
		UINT	Reserved25		:1;			/* 25 : Reserve											*/
		UINT	Reserved24		:1;			/* 24 : Reserve											*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved23		:1;			/* 23 : POP (A/P:B-DPK, Network:-)						*/
		UINT	Reserved22		:1;			/* 22 : POP (A/P:B-G, Network:-)						*/
		UINT	Reserved21		:1;			/* 21 : POP (A/P:B-F, Network:-)						*/
		UINT	Reserved20		:1;			/* 20 : POP (A/P:B-E, Network:-)						*/
		UINT	Reserved19		:1;			/* 19 : POP (A/P:B-D, Network:-)						*/
		UINT	Reserved18		:1;			/* 18 : POP (A/P:B-C, Network:-)						*/
		UINT	Reserved17		:1;			/* 17 : POP (A/P:B-B, Network:-)						*/
		UINT	Reserved16		:1;			/* 16 : POP (A/P:B-A, Network:-)						*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved15		:1;			/* 15 : POP (A/P:B-LD5, Network:B-DPK)					*/
		UINT	Reserved14		:1;			/* 14 : POP (A/P:B-LD4, Network:B-G)					*/
		UINT	Reserved13		:1;			/* 13 : POP (A/P:B-LD3, Network:B-F)					*/
		UINT	Reserved12		:1;			/* 12 : POP (A/P:B-LD2, Network:B-E)					*/
		UINT	Reserved11		:1;			/* 11 : POP (A/P:B-LD1, Network:B-D)					*/
		UINT	SO7				:1;			/* 10 : 出力信号7/POP (A/P:CN1-39pin, Network:B-C)		*/
		UINT	SO6				:1;			/*  9 : 出力信号6/POP (A/P:CN1-38pin, Network:B-B)		*/
		UINT	SO5				:1;			/*  8 : 出力信号5/POP (A/P:CN1-37pin, Network:B-A)		*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	SO4				:1;			/*  7 : 出力信号4 (A/P:CN1-29,30pin, Network:CN1-25,26)	*/
		UINT	SO3				:1;			/*  6 : 出力信号3 (A/P:CN1-27,28pin, Network:CN1-23,24)	*/
		UINT	SO2				:1;			/*  5 : 出力信号2 (A/P:CN1-31,32pin, Network:CN1-3,4)	*/
		UINT	SO1				:1;			/*  4 : 出力信号1 (A/P:CN1-25,26pin, Network:CN1-1,2)	*/
		UINT	Plsel			:1;			/*  3 : PLSEL											*/
		UINT	LedOut			:1;			/*  2 : CM:COM_RDY, M3:CON_LED, M2,A/P:-				*/
		UINT	RegenOn			:1;			/*  1 : REGON											*/
		UINT	RelayOn			:1;			/*  0 : RLYON											*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/
} HWSOUT;

#else /* __LITTLE_ENDIAN__ */

typedef	union HWSOUT {					/* Serial Output Signal										*/
#ifndef _TEST_BOARD_ /* @@最終的には削除 */
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	/* APもM2/M3に合わせた<S081> 																	*/
// #ifndef _IF_BOARD_REVC_	/* <S063> *//*<S138>*/
//	#define	KPX_HWSOUTXOR		0x000000D3				/* <S081> Output Signal Reverse Bits Data	*/
//#else	//#ifndef _IF_BOARD_REVC_	/* <S063> *//*<S138>*/
#if(PRODUCT_SERIES_SEVEN==FALSE)                        /* <S0AB>  */
	#define	KPX_HWSOUTXOR		0x0000F803				/* <S081> Output Signal Reverse Bits Data <S09F>	*/
#else
/* <S1AB> : SO4～SO6の出力信号反転を廃止 */
//#define	KPX_HWSOUTXOR	0x0000F8D3				/* <S081> Output Signal Reverse Bits Data <S063>			*/	/* <S1AA> */
#define	KPX_HWSOUTXOR		0x0000FFD3				/* Output Signal Reverse Bits Data */	/* <S1AA> */
#endif
//#endif	//#ifndef _IF_BOARD_REVC_	/* <S063> *//*<S138>*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	#define	KPX_HWSOUTXOR		0x000000D3				/* <S045> Output Signal Reverse Bits Data	*/
#else /* NETIF_M2_TYPE / NETIF_CM_TYPE */
	#define	KPX_HWSOUTXOR		0x000000D3				/* <S045> Output Signal Reverse Bits Data	*/
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		<S045> Output Mapping Data and Output Macro Definition										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_SO1BITNO		(6)
#define	KPI_SO2BITNO		(7)
#define	KPI_SO3BITNO		(4)
#define	KPI_SO4BITNO		(8)		/* <S04B> */
#define	KPI_SO5BITNO		(9)		/* <S04B> */
#define	KPI_SO6BITNO		(10)	/* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル出力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	RelayOn			:1;			/*  0 : RLYON											*/
		UINT	RegenOn			:1;			/*  1 : REGON											*/
		UINT	LedOut			:1;			/*  2 : CM:COM_RDY, M3:CON_LED, M2,A/P:-				*/
		UINT	Plsel			:1;			/*  3 : PLSEL											*/
		UINT	OUT1			:1;			/*  4 : 出力信号1 (A/P:CN1-29,30pin, Network:CN1-1,2)	*/
		UINT	OUT2			:1;			/*  5 : 出力信号2 (A/P:CN1-31,32pin, Network:CN1-3,4)	*/
		UINT	OUT3			:1;			/*  6 : 出力信号3 (A/P:CN1-25,26pin, Network:CN1-23,24)	*/
		UINT	OUT4			:1;			/*  7 : 出力信号4 (A/P:CN1-27,28pin, Network:CN1-25,26)	*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	OUT5			:1;			/*  8 : 出力信号5/ALMCD1 (A/P:CN1-37pin, Network:B-A)	*//* <S04B> */
		UINT	OUT6			:1;			/*  9 : 出力信号6/ALMCD2 (A/P:CN1-38pin, Network:B-B)	*//* <S04B> */
		UINT	OUT7			:1;			/* 10 : 出力信号7/ALMCD3 (A/P:CN1-39pin, Network:B-C)	*//* <S04B> */
		UINT	Reserved11		:1;			/* 11 : POP (A/P:B-LD1, Network:B-D)					*/
		UINT	Reserved12		:1;			/* 12 : POP (A/P:B-LD2, Network:B-E)					*/
		UINT	Reserved13		:1;			/* 13 : POP (A/P:B-LD3, Network:B-F)					*/
		UINT	Reserved14		:1;			/* 14 : POP (A/P:B-LD4, Network:B-G)					*/
		UINT	Reserved15		:1;			/* 15 : POP (A/P:B-LD5, Network:B-DPK)					*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved16		:1;			/* 16 : POP (A/P:B-A, Network:-)						*/
		UINT	Reserved17		:1;			/* 17 : POP (A/P:B-B, Network:-)						*/
		UINT	Reserved18		:1;			/* 18 : POP (A/P:B-C, Network:-)						*/
		UINT	Reserved19		:1;			/* 19 : POP (A/P:B-D, Network:-)						*/
		UINT	Reserved20		:1;			/* 20 : POP (A/P:B-E, Network:-)						*/
		UINT	Reserved21		:1;			/* 21 : POP (A/P:B-F, Network:-)						*/
		UINT	Reserved22		:1;			/* 22 : POP (A/P:B-G, Network:-)						*/
		UINT	Reserved23		:1;			/* 23 : POP (A/P:B-DPK, Network:-)						*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved24		:1;			/* 24 : Reserve											*/
		UINT	Reserved25		:1;			/* 25 : Reserve											*/
		UINT	Reserved26		:1;			/* 26 : Reserve											*/
		UINT	Reserved27		:1;			/* 27 : Reserve											*/
		UINT	Reserved28		:1;			/* 28 : Reserve											*/
		UINT	Reserved29		:1;			/* 29 : Reserve											*/
		UINT	Reserved30		:1;			/* 30 : Reserve											*/
		UINT	Reserved31		:1;			/* 31 : Reserve											*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/

#else /* _TEST_BOARD_ */ /* @@最終的には以下削除 */

/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_HWSOUTXOR			0xFFFFFFFF				/* Output Signal Reverse Bits Data			*/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル出力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	RelayOn			:1;			/*  0 : RLYON											*/
		UINT	Reserved01		:1;			/*  1 : Reserve											*/
		UINT	NetConnect		:1;			/*  2 : CON_LED											*/
		UINT	Plsel			:1;			/*  3 : PLSEL											*/
		UINT	SO1				:1;			/*  4 : CN1-  pin										*/
		UINT	SO2				:1;			/*  5 : CN1-  pin										*/
		UINT	SO3				:1;			/*  6 : CN1-  pin										*/
		UINT	SO4				:1;			/*  7 : CN1-  pin										*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	AlmCd1			:1;			/*  8 : Reserve											*/
		UINT	AlmCd2			:1;			/*  9 : Reserve											*/
		UINT	AlmCd3			:1;			/* 10 : Reserve											*/
		UINT	Reserved11		:1;			/* 11 : Reserve											*/
		UINT	Reserved12		:1;			/* 12 : Reserve											*/
		UINT	Reserved13		:1;			/* 13 : Reserve											*/
		UINT	Reserved14		:1;			/* 14 : Reserve											*/
		UINT	Reserved15		:1;			/* 15 : Reserve											*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved16		:1;			/* 16 : Reserve											*/
		UINT	Reserved17		:1;			/* 17 : Reserve											*/
		UINT	Reserved18		:1;			/* 18 : Reserve											*/
		UINT	Reserved19		:1;			/* 19 : Reserve											*/
		UINT	Reserved20		:1;			/* 20 : Reserve											*/
		UINT	Reserved21		:1;			/* 21 : Reserve											*/
		UINT	Reserved22		:1;			/* 22 : Reserve											*/
		UINT	Reserved23		:1;			/* 23 : Reserve											*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved24		:1;			/* 24 : Reserve											*/
		UINT	Reserved25		:1;			/* 25 : Reserve											*/
		UINT	Reserved26		:1;			/* 26 : Reserve											*/
		UINT	Reserved27		:1;			/* 27 : Reserve											*/
		UINT	Reserved28		:1;			/* 28 : Reserve											*/
		UINT	Reserved29		:1;			/* 29 : Reserve											*/
		UINT	Reserved30		:1;			/* 30 : Reserve											*/
		UINT	Reserved31		:1;			/* 31 : Reserve											*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/

#endif /* _TEST_BOARD_ */

} HWSOUT;
#endif /* __BIG_ENDIAN__ */


#else /* CSW_BOARD_TYPE == SGD7W_TYPE *//* <S0A2> */

/****************************************************************************************************/
/*																									*/
/*		ハードウェア入力信号処理定義																*/
/*																									*/
/****************************************************************************************************/
#ifdef __BIG_ENDIAN__
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
	#define	KPX_HWSINXOR		0x01E0F000				/* Input Signal Reverse Bits Data			*/
	#define	KPX_HWSOUTXOR		0xFFFFFFFF				/* Output Signal Reverse Bits Data			*/

/*--------------------------------------------------------------------------------------------------*/
/*		シリアル入力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	IN0				:1;			/* 31 : 入力信号01 (CN1-3pin)							*/
		UINT	IN1				:1;			/* 30 : 入力信号02 (CN1-4pin)							*/
		UINT	IN2				:1;			/* 29 : 入力信号03 (CN1-5pin)							*/
		UINT	IN3				:1;			/* 28 : 入力信号04 (CN1-6pin)							*/
		UINT	IN4				:1;			/* 27 : 入力信号05 (CN1-7pin)							*/
		UINT	IN5				:1;			/* 26 : 入力信号06 (CN1-8pin)							*/
		UINT	SI6				:1;			/* 25 : 入力信号6 (A/P:CN1-46pin, Network:CN1-12pin)	*/
		UINT	IN7				:1;			/* 24 : 入力信号12 (CN1-10pin)							*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	IN8				:1;			/* 23 : 入力信号13 (CN1-11pin)							*/
		UINT	IN9				:1;			/* 22 : 入力信号14 (CN1-12pin)							*/
		UINT	IN10			:1;			/* 21 : 入力信号15 (CN1-13pin)							*/
		UINT	IN11			:1;			/* 20 : 入力信号16 (CN1-14pin)							*/
		UINT	RegeneAlm		:1;			/* 19 : *Regene Alarm									*/
		UINT	FanAlm			:1;			/* 18 : *FAN Alarm										*/
		UINT	Acon2			:1;			/* 17 : *AC電源入力2									*/
		UINT	Acon1			:1;			/* 16 : *AC電源入力1									*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	DPsw14			:1;			/* 15 : *Dip SW(4接点:LSB)		(Only Network)			*/
		UINT	DPsw13			:1;			/* 14 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw12			:1;			/* 13 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw11			:1;			/* 12 : *Dip SW(4接点:MSB)		(Only Network)			*/
		UINT	RTsw21			:1;			/* 11 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw22			:1;			/* 10 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw23			:1;			/*  9 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw24			:1;			/*  8 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	RTsw11			:1;			/*  7 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw12			:1;			/*  6 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw13			:1;			/*  5 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw14			:1;			/*  4 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	Reserve4		:1;			/*  3 : Reserve											*/
		UINT	Reserve2		:1;			/*  2 : Reserve											*/
		UINT	Reserve1		:1;			/*  1 : Reserve											*/
		UINT	Reserve0		:1;			/*  0 : Reserve											*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/
} HWSIN;

#else /*  __LITTLE_ENDIAN__ */

typedef	union HWSIN {					/* Serial Input Signal										*/
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
	#define	KPX_HWSINXOR		0x000FF000				/* Input Signal Reverse Bits Data			*/
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル入力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	Reserve0		:1;			/*  0 : Reserve											*/
		UINT	Reserve1		:1;			/*  1 : Reserve											*/
		UINT	Reserve2		:1;			/*  2 : Reserve											*/
		UINT	Reserve3		:1;			/*  3 : Reserve											*/
		UINT	RTsw14			:1;			/*  4 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw13			:1;			/*  5 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw12			:1;			/*  6 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
		UINT	RTsw11			:1;			/*  7 : Rotaly SW1(16進SW:0-F)	(Only M-III)			*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	RTsw24			:1;			/*  8 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw23			:1;			/*  9 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw22			:1;			/* 10 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	RTsw21			:1;			/* 11 : Rotaly SW2(16進SW:0-F)	(Only Network)			*/
		UINT	DPsw11			:1;			/* 12 : *Dip SW(4接点:MSB)		(Only Network)			*/
		UINT	DPsw12			:1;			/* 13 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw13			:1;			/* 14 : *Dip SW(4接点)			(Only Network)			*/
		UINT	DPsw14			:1;			/* 15 : *Dip SW(4接点:LSB)		(Only Network)			*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Acon1			:1;			/* 16 : *AC電源入力1									*/
		UINT	Acon2			:1;			/* 17 : *AC電源入力2									*/
		UINT	FanAlm			:1;			/* 18 : *FAN Alarm										*/
		UINT	RegeneAlm		:1;			/* 19 : *Regene Alarm									*/
		UINT	IN11			:1;			/* 20 : 入力信号16 (CN1-14pin)							*/
		UINT	IN10			:1;			/* 21 : 入力信号15 (CN1-13pin)							*/
		UINT	IN9				:1;			/* 22 : 入力信号14 (CN1-12pin)							*/
		UINT	IN8				:1;			/* 23 : 入力信号13 (CN1-11pin)							*/
		/*------------------------------------------------------------------------------------------*/
		UINT	IN7				:1;			/* 24 : 入力信号12 (CN1-10pin)							*/
		UINT	IN6				:1;			/* 25 : 入力信号11 (CN1-9pin)							*/
		UINT	IN5				:1;			/* 26 : 入力信号06 (CN1-8pin)							*/
		UINT	IN4				:1;			/* 27 : 入力信号05 (CN1-7pin)							*/
		UINT	IN3				:1;			/* 28 : 入力信号04 (CN1-6pin)							*/
		UINT	IN2				:1;			/* 29 : 入力信号03 (CN1-5pin)							*/
		UINT	IN1				:1;			/* 30 : 入力信号02 (CN1-4pin)							*/
		UINT	IN0				:1;			/* 31 : 入力信号01 (CN1-3pin)							*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/

} HWSIN;

#endif /* __BIG_ENDIAN__ */



/*--------------------------------------------------------------------------------------------------*/
/*		シリアル出力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef __BIG_ENDIAN__
typedef	union HWSOUT {					/* Serial Output Signal										*/
	struct {							/* ビット信号定義											*/
		UINT	B_DPK1			:1;			/* 31 : 7SEG_1:B_DPK									*/
		UINT	B_G1			:1;			/* 30 : 7SEG_1:B_G										*/
		UINT	B_F1			:1;			/* 29 : 7SEG_1:B_F										*/
		UINT	B_E1			:1;			/* 28 : 7SEG_1:B_E										*/
		UINT	B_D1			:1;			/* 27 : 7SEG_1:B_D										*/
		UINT	B_C1			:1;			/* 26 : 7SEG_1:B_C										*/
		UINT	B_B1			:1;			/* 25 : 7SEG_1:B_B										*/
		UINT	B_A1			:1;			/* 24 : 7SEG_1:B_A										*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	B_DPK2			:1;			/* 23 : 7SEG_2:B_DPK									*/
		UINT	B_G2			:1;			/* 22 : 7SEG_2:B_G										*/
		UINT	B_F2			:1;			/* 21 : 7SEG_2:B_F										*/
		UINT	B_E2			:1;			/* 20 : 7SEG_2:B_E										*/
		UINT	B_D2			:1;			/* 19 : 7SEG_2:B_D										*/
		UINT	B_C2			:1;			/* 18 : 7SEG_2:B_C										*/
		UINT	B_B2			:1;			/* 17 : 7SEG_2:B_B										*/
		UINT	B_A2			:1;			/* 16 : 7SEG_2:B_A										*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Reserved5		:1;			/* 15 : Reserver										*/
		UINT	Reserved4		:1;			/* 14 : Reserver										*/
		UINT	Reserved3		:1;			/* 13 : Reserver										*/
		UINT	Reserved2		:1;			/* 12 : Reserver										*/
		UINT	Reserved1		:1;			/* 11 : Reserver										*/
		UINT	OUT7			:1;			/* 10 : 出力信号7 (CN1-31,32)							*/
		UINT	OUT6			:1;			/*  9 : 出力信号6 (CN1-29,30)							*/
		UINT	OUT5			:1;			/*  8 : 出力信号5 (CN1-27,28)							*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	OUT4			:1;			/*  7 : 出力信号4 (CN1-25,26)							*/
		UINT	OUT3			:1;			/*  6 : 出力信号3 (CN1-23,24)							*/
		UINT	OUT2			:1;			/*  5 : 出力信号2 (CN1-21,22)							*/
		UINT	OUT1			:1;			/*  4 : 出力信号1 (CN1-19,20)							*/
		UINT	Plsel			:1;			/*  3 : PLSEL											*/
		UINT	LedOut			:1;			/*  2 : CON_LED											*/
		UINT	RegenOn			:1;			/*  1 : REGON											*/
		UINT	RelayOn			:1;			/*  0 : RLYON											*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/
} HWSOUT;

#else /* __LITTLE_ENDIAN__ */

typedef	union HWSOUT {					/* Serial Output Signal										*/
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定													*/
/*--------------------------------------------------------------------------------------------------*/
//	#define	KPX_HWSOUTXOR		0x00000005				/* Output Signal Reverse Bits Data			*//* <S129> */

#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	SGD7W IO入出力対応<S118>	*/
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定								<S129>					*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_HWSOUTXOR		0x00000F85				/* Output Signal Reverse Bits Data			*/
/*--------------------------------------------------------------------------------------------------*/
/*	<S045> Input Mapping Data and Input Macro Definition										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_SI0BITNO		(31)	/*	CN1( 3Pin):SI01（デフォルト割当：P-OT0）［1軸目用］	*/
#define	KPI_SI1BITNO		(30)	/*	CN1( 4Pin):SI02（デフォルト割当：N-OT0）［1軸目用］	*/
#define	KPI_SI2BITNO		(29)	/*	CN1( 5Pin):SI03（デフォルト割当：DEC0  ）［1軸目用］	*/
#define	KPI_SI3BITNO		(28)	/*	CN1( 6Pin):SI04（デフォルト割当：EXT01）［1軸目用］	*/
#define	KPI_SI4BITNO		(27)	/*	CN1( 7Pin):SI05（デフォルト割当：EXT02）［1軸目用］	*/
#define	KPI_SI5BITNO		(26)	/*	CN1( 8Pin):SI06（デフォルト割当：EXT03）［1軸目用］	*/
#define	KPI_SI6BITNO		(25)	/*  CN1( 9Pin):SI07（デフォルト割当：P-OT1）［2軸目用］	*/
#define KPI_SI7BITNO		(24)	/*	CN1(10Pin):SI08（デフォルト割当：N-OT1）［2軸目用］	*/
#define KPI_SI8BITNO		(23)	/*	CN1(11Pin):SI09（デフォルト割当：DEC1  ）［2軸目用］	*/
#define KPI_SI9BITNO		(22)	/*	CN1(12Pin):SI10（デフォルト割当：EXT11）［2軸目用］	*/
#define KPI_SI10BITNO		(21)	/*	CN1(13Pin):SI11（デフォルト割当：EXT12）［2軸目用］	*/
#define KPI_SI11BITNO		(20)	/*	CN1(14Pin):SI12（デフォルト割当：EXT13）［2軸目用］	*/
#define KPI_SINHIBITNO		(1)
#define KPI_SINLOBITNO		(0)
#define KPI_SINANDBITS		(0xFFFFFFF0)
#define KPI_SINSETBITS		(0x00000002)
/*--------------------------------------------------------------------------------------------------*/
/*	<S045> Output Mapping Data and Output Macro Definition										*/
/*--------------------------------------------------------------------------------------------------*/
#if 0

#define	KPI_SO0BITNO		(5)	/*	OUT0 ALM0       [CN1-19,20]	*/
#define	KPI_SO1BITNO		(6)	/*	OUT1 ALM1       [CN1-21,22]	*/
#define	KPI_SO2BITNO		(7)	/*	OUT2 SO1(OUT1_5)[CN1-23,24]	*/
#define	KPI_SO3BITNO		(8)	/*	OUT3 SO2(OUT2_5)[CN1-25,26]	*/
#define	KPI_SO4BITNO		(9)	/*	OUT4 SO3(OUT3_5)[CN1-27,28]	*/
#define	KPI_SO5BITNO		(10)/*	OUT5 SO4(OUT4_5)[CN1-29,30]	*/
#define	KPI_SO6BITNO		(11)/*	OUT6 SO5(OUT5_5)[CN1-31,32]	*/
#endif
#define	KPI_SO1BITNO		(7)	/*	OUT2 SO1(OUT1_5)[CN1-23,24]	*/
#define	KPI_SO2BITNO		(8)	/*	OUT2 SO2(OUT1_5)[CN1-25,26]	*/
#define	KPI_SO3BITNO		(9)	/*	OUT3 SO3(OUT2_5)[CN1-27,28]	*/
#define	KPI_SO4BITNO		(10)/*	OUT4 SO4(OUT3_5)[CN1-29,30]	*/
#define	KPI_SO5BITNO		(11)/*	OUT5 SO5(OUT4_5)[CN1-31,32]	*/
#define	KPI_SO6BITNO		(-1)/* not available */

#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア入出力信号論理反転ビット指定								<S129>					*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_HWSOUTXOR		0x00000005				/* Output Signal Reverse Bits Data			*/
/*--------------------------------------------------------------------------------------------------*/
/*	<S045> Input Mapping Data and Input Macro Definition										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_SI0BITNO		(31)
#define	KPI_SI1BITNO		(30)
#define	KPI_SI2BITNO		(29)
#define	KPI_SI3BITNO		(28)
#define	KPI_SI4BITNO		(27)
#define	KPI_SI5BITNO		(26)
#define	KPI_SI6BITNO		(25)
#define KPI_SINHIBITNO		(1)
#define KPI_SINLOBITNO		(0)
#define KPI_SINANDBITS		(0xFFFFFFF0)
#define KPI_SINSETBITS		(0x00000002)
/*--------------------------------------------------------------------------------------------------*/
/*	<S045> Output Mapping Data and Output Macro Definition										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_SO1BITNO		(6)
#define	KPI_SO2BITNO		(7)
#define	KPI_SO3BITNO		(4)
#define	KPI_SO4BITNO		(8)		/* <S04B> */
#define	KPI_SO5BITNO		(9)		/* <S04B> */
#define	KPI_SO6BITNO		(10)	/* <S04B> */
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		シリアル出力信号定義																		*/
/*--------------------------------------------------------------------------------------------------*/
	struct {							/* ビット信号定義											*/
		UINT	RelayOn			:1;			/*  0 : RLYON											*/
		UINT	Reserved0		:1;			/*  1 : Reserver										*/
		UINT	RegenOn			:1;			/*  2 : REGON											*/
		UINT	LedOut			:1;			/*  3 : CON_LED											*/
		UINT	Plsel			:1;			/*  4 : PLSEL											*/
		UINT	OUT0			:1;			/*  5 : 出力信号1 (CN1-19,20, ALM0+,ALM0-)	<S104>			*/
		UINT	OUT1			:1;			/*  6 : 出力信号2 (CN1-21,22, ALM1+,ALM1-)	<S104>			*/
		UINT	OUT2			:1;			/*  7 : 出力信号3 (CN1-23,24)		<S104>					*/
		UINT	OUT3			:1;			/*  8 : 出力信号4 (CN1-25,26)		<S104>					*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	OUT4			:1;			/*  9 : 出力信号5 (CN1-27,28)		<S104>					*/
		UINT	OUT5			:1;			/* 10 : 出力信号6 (CN1-29,30)		<S104>					*/
		UINT	OUT6			:1;			/* 11 : 出力信号7 (CN1-31,32)		<S104>					*/
		UINT	Reserved1		:1;			/* 12 : Reserver										*/
		UINT	Reserved2		:1;			/* 13 : Reserver										*/
		UINT	Reserved3		:1;			/* 14 : Reserver										*/
		UINT	Reserved4		:1;			/* 15 : Reserver										*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	B_A2			:1;			/* 16 : 7SEG_2:B_A										*/
		UINT	B_B2			:1;			/* 17 : 7SEG_2:B_B										*/
		UINT	B_C2			:1;			/* 18 : 7SEG_2:B_C										*/
		UINT	B_D2			:1;			/* 19 : 7SEG_2:B_D										*/
		UINT	B_E2			:1;			/* 20 : 7SEG_2:B_E										*/
		UINT	B_F2			:1;			/* 21 : 7SEG_2:B_F										*/
		UINT	B_G2			:1;			/* 22 : 7SEG_2:B_G										*/
		UINT	B_DPK2			:1;			/* 23 : 7SEG_2:B_DPK									*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	B_A1			:1;			/* 24 : 7SEG_1:B_A										*/
		UINT	B_B1			:1;			/* 25 : 7SEG_1:B_B										*/
		UINT	B_C1			:1;			/* 26 : 7SEG_1:B_C										*/
		UINT	B_D1			:1;			/* 27 : 7SEG_1:B_D										*/
		UINT	B_E1			:1;			/* 28 : 7SEG_1:B_E										*/
		UINT	B_F1			:1;			/* 29 : 7SEG_1:B_F										*/
		UINT	B_G1			:1;			/* 30 : 7SEG_1:B_G										*/
		UINT	B_DPK1			:1;			/* 31 : 7SEG_1:B_DPK									*/
	}flag;
	/*----------------------------------------------------------------------------------------------*/
	ULONG	dw;								/* For ULONG Access										*/

} HWSOUT;
#endif /* __BIG_ENDIAN__ */

#endif /* CSW_BOARD_TYPE == SGD7S_TYPE *//* <S0A2> */



/*--------------------------------------------------------------------------------------------------*/
/*		信号入出力マクロ定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef _VC_DEBUG_
#ifndef _TEST_BOARD_ /* @@最終的には削除 */
/* for Serial I/O */
#define HW_SIN_GET()			(*(VLONG*)(HW_SVASIC_ADDRESS + 0x000000E0UL))
//#define HW_SOUT_SET(out)		*(VUSHORT*)(HW_SVASIC_ADDRESS + 0x00000008UL) = (ULONG)(out)
#define HW_SOUT_SET(out)		*(VULONG*)(HW_SVASIC_ADDRESS + 0x00000008UL) = (ULONG)(out)		/* @@ chk */
#define KPX_AREG_IOSTAT_INIOUT	(0xFF000000 ^ KPX_HWSOUTXOR)	/* 定数は正論理で指定				*/

#else /* _TEST_BOARD_ */ /* @@最終的には以下削除 */
#define HW_SIN_GET()			{(*(VUSHORT*)(HW_SVASIC_ADDRESS + 0x000070UL)							\
								 | (*(VUSHORT*)(HW_SVASIC_ADDRESS + 0x000072UL)^ 0xffff) << 16);}
#define HW_SOUT_SET(out)		{*(VUSHORT*)(HW_SVASIC_ADDRESS + 0x000004UL) = (USHORT)((out)>>0);		\
								 *(VUSHORT*)(HW_SVASIC_ADDRESS + 0x000006UL) = (USHORT)((out)>>16);}
#define KPX_AREG_IOSTAT1_INIOUT	(0xFFF0 ^ KPX_HWSOUTXOR)	/* 定数は正論理で指定				*/

#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
	#define KPX_AREG_IOSTAT2_INIOUT	(0x00BF)					/* <S153> パネルオペレータ初期値"-"		*/
#else
	#define KPX_AREG_IOSTAT2_INIOUT	(0x0000)					/* パネルオペレータ用(接続無し)		*/
#endif

#endif /* _TEST_BOARD_ */

#else /* VC_DEBUG_ */
/* for Serial I/O */
#define HW_SIN_GET()			0x00000000UL
#define HW_SOUT_SET(out)		{;}
#define KPX_AREG_IOSTAT_INIOUT	0xFFFFFFFFUL
#endif /* VC_DEBUG_ */


/* for Panel 7Segment LED *//* <S0A1> *//* <S0A2> */
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
	#define LED_INIT_DISPLAY		0x00FF0800 /* B-LD1 all OFF */
	#define REGON_STS				0x02 /* <S0EF> */
#else /* CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE */
	#if CSW_BOARD_TYPE == SGD7S_TYPE
		#define NET_7SEG1OUT_BIT_SFT	8
		#define NET_7SEG2OUT_BIT_SFT	8			/* 実際は不使用 */
		#define CON_LED_BIT_SFT			2
		#define LED_INIT_DISPLAY		0x0000FF04	/* 7Seg and CON_LED all OFF */
		#define REGON_STS				0x02 /* <S0EF> */
	#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
//		#define NET_7SEG1OUT_BIT_SFT	24	/* <S10B> */
//		#define NET_7SEG2OUT_BIT_SFT	16	/* <S10B> */
		#define NET_7SEG1OUT_BIT_SFT	16	/* <S10B> */
		#define NET_7SEG2OUT_BIT_SFT	24	/* <S10B> */
		#define CON_LED_BIT_SFT			3
//		#define LED_INIT_DISPLAY		0x00FFFF08	/* 7Seg and CON_LED all OFF */	/* <S114> */
		#define LED_INIT_DISPLAY		0xFFFFF008	/* 7Seg and CON_LED all OFF */	/* <S114> */
		#define REGON_STS				0x04 /* <S0EF> */
	#endif /* CSW_BOARD_TYPE == SGD7S_TYPE */
#endif /* CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE */

/*--------------------------------------------------------------------------------------------------*/
/*		シーケンス信号出力マクロ定義																*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef	CSW_SGD7W_TYPE		/* ALM0,ALM1出力対応<S104> */
#define	KPI_SOUT_ALARM0( x, y )		{ x.flag.OUT2 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_ALARM1( x, y )		{ ;}
#define	KPI_SOUT_SO1( x, y )		{ x.flag.OUT3 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO2( x, y )		{ x.flag.OUT4 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO3( x, y )		{ x.flag.OUT1 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO4( x, y )		{ x.flag.OUT5 = ((y) ? TRUE : FALSE) ;} /* <S04B> */
#define	KPI_SOUT_SO5( x, y )		{ x.flag.OUT6 = ((y) ? TRUE : FALSE) ;} /* <S04B> */
#define	KPI_SOUT_SO6( x, y )		{ x.flag.OUT7 = ((y) ? TRUE : FALSE) ;} /* <S04B> */
#define	KPI_SOUT_ALMCODE( x, y )	{ x.dw |= ((y ^ 0x07) << KPI_SOUTACBITNO);\
									  x.dw |= KPX_SOUTPUTBITX( ((y>>0) & 0x01), Iprm.Omap.Alo1 );\
									  x.dw |= KPX_SOUTPUTBITX( ((y>>1) & 0x01), Iprm.Omap.Alo2 );\
									  x.dw |= KPX_SOUTPUTBITX( ((y>>2) & 0x01), Iprm.Omap.Alo3 );}
#else	/* #ifndef CSW_SGD7W_TYPE	ALM0,ALM1出力対応<S104> */
#define	KPI_SOUT_ALARM0( x, y )		{ x.flag.OUT0 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_ALARM1( x, y )		{ x.flag.OUT1 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO1( x, y )		{ x.flag.OUT2 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO2( x, y )		{ x.flag.OUT3 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO3( x, y )		{ x.flag.OUT4 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO4( x, y )		{ x.flag.OUT5 = ((y) ? TRUE : FALSE) ;}
#define	KPI_SOUT_SO5( x, y )		{ x.flag.OUT6 = ((y) ? TRUE : FALSE) ;}
#endif	/* #ifndef CSW_SGD7W_TYPE	ALM0,ALM1出力対応<S104> */

/****************************************************************************************************/
/*																									*/
/*		P-N電圧検出用定義																			*/
/*																									*/
/****************************************************************************************************/
/* 主回路電圧検出用A/Dアドレス */
#ifndef _TEST_BOARD_
#define	GET_PN_DC_VOLT()	(*(VLONG*)(HW_SVASIC_ADDRESS + 0x0350UL))
#else	/* TEST BOARD */
#define	GET_PN_DC_VOLT()	(*(VUSHORT*)(HW_SVASIC_ADDRESS + 0x00EEUL))
#define	DET_DCVOLT_START()	*(VUSHORT*)(HW_SVASIC_ADDRESS + 0x00EEUL) = (USHORT)(0)
#endif



/****************************************************************************************************/
/*																									*/
/*		モニタ用ハードウェア信号定義			SGD7W IO入出力対応<S118>							*/
/*																									*/
/****************************************************************************************************/
#ifndef _TEST_BOARD_	/* 仮対応 */
/*--------------------------------------------------------------------------------------------------*/
/*		Un005 : 入力信号モニタ [-]																	*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
/*		SGD7S																						*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
#define	KPX_MON_HWSIN00( x )		x.flag.IN0
#define	KPX_MON_HWSIN01( x )		x.flag.IN1
#define	KPX_MON_HWSIN02( x )		x.flag.IN2
#define	KPX_MON_HWSIN03( x )		x.flag.IN3
#define	KPX_MON_HWSIN04( x )		x.flag.IN4
#define	KPX_MON_HWSIN05( x )		x.flag.IN5
#define	KPX_MON_HWSIN06( x )		x.flag.IN6
#define	KPX_MON_HWSIN07( x )		0
#define	KPX_MON_HWSIN08( x )		0
#define	KPX_MON_HWSIN09( x )		0
#define	KPX_MON_HWSIN10( x )		0
#define	KPX_MON_HWSIN11( x )		0
#else	/* #if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */
#define	KPX_MON_HWSIN00( x )		x.flag.IN0
#define	KPX_MON_HWSIN01( x )		x.flag.IN3
#define	KPX_MON_HWSIN02( x )		x.flag.IN1
#define	KPX_MON_HWSIN03( x )		x.flag.IN2
#define	KPX_MON_HWSIN04( x )		x.flag.IN4
#define	KPX_MON_HWSIN05( x )		x.flag.IN5
#define	KPX_MON_HWSIN06( x )		x.flag.IN6
#define	KPX_MON_HWSIN07( x )		x.flag.SEN
#define	KPX_MON_HWSIN08( x )		0
#define	KPX_MON_HWSIN09( x )		0
#define	KPX_MON_HWSIN10( x )		0
#define	KPX_MON_HWSIN11( x )		0
#endif	/* #if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */
/*--------------------------------------------------------------------------------------------------*/
#elif CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
/*		SGD7W																						*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_MON_HWSIN00( x )		x.flag.IN0
#define	KPX_MON_HWSIN01( x )		x.flag.IN1
#define	KPX_MON_HWSIN02( x )		x.flag.IN2
#define	KPX_MON_HWSIN03( x )		x.flag.IN3
#define	KPX_MON_HWSIN04( x )		x.flag.IN4
#define	KPX_MON_HWSIN05( x )		x.flag.IN5
#define	KPX_MON_HWSIN06( x )		x.flag.IN6
#define	KPX_MON_HWSIN07( x )		x.flag.IN7
#define	KPX_MON_HWSIN08( x )		x.flag.IN8
#define	KPX_MON_HWSIN09( x )		x.flag.IN9
#define	KPX_MON_HWSIN10( x )		x.flag.IN10
#define	KPX_MON_HWSIN11( x )		x.flag.IN11
#endif	/* #if CSW_BOARD_TYPE == SGD7S_TYPE		SGD7S（1軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Un006 : 出力信号モニタ [-]																	*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
/*		SGD7S																						*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
#define	KPX_MON_HWSOUT00( x )		x.flag.OUT2 /* AlmOut */
#define	KPX_MON_HWSOUT01( x )		x.flag.OUT3 /* SO1 */
#define	KPX_MON_HWSOUT02( x )		x.flag.OUT4 /* SO2 */
#define	KPX_MON_HWSOUT03( x )		x.flag.OUT1 /* SO3 */
#define	KPX_MON_HWSOUT04( x )		0
#define	KPX_MON_HWSOUT05( x )		0
#define	KPX_MON_HWSOUT06( x )		0
#define	KPX_MON_HWSOUT07( x )		0
#else	/* #if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */
#define	KPX_MON_HWSOUT00( x )		x.flag.OUT2 /* AlmOut */
#define	KPX_MON_HWSOUT01( x )		x.flag.OUT3 /* SO1 */
#define	KPX_MON_HWSOUT02( x )		x.flag.OUT4 /* SO2 */
#define	KPX_MON_HWSOUT03( x )		x.flag.OUT1 /* SO3 */
#define	KPX_MON_HWSOUT04( x )		x.flag.OUT5 /* SO4 *//* <S04B> */
#define	KPX_MON_HWSOUT05( x )		x.flag.OUT6 /* SO5 *//* <S04B> */
#define	KPX_MON_HWSOUT06( x )		x.flag.OUT7 /* SO6 *//* <S04B> */
#define	KPX_MON_HWSOUT07( x )		0
#endif	/* #if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */
/*--------------------------------------------------------------------------------------------------*/
#elif CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
/*--------------------------------------------------------------------------------------------------*/
/*		SGD7W																						*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/* <S1C0>：DEL */
#define	KPX_MON_HWSOUT00( x )		x.flag.OUT2 /* S01(CN1-23,24) */
#define	KPX_MON_HWSOUT01( x )		x.flag.OUT3 /* SO2(CN1-25,26) */
#define	KPX_MON_HWSOUT02( x )		x.flag.OUT4 /* SO3(CN1-27,28)*/
#define	KPX_MON_HWSOUT03( x )		x.flag.OUT5 /* SO4(CN1-29,30) */
#define	KPX_MON_HWSOUT04( x )		0
#define	KPX_MON_HWSOUT05( x )		0
#define	KPX_MON_HWSOUT06( x )		0
#define	KPX_MON_HWSOUT07( x )		0
#endif

#define	KPX_MON_HWSOUT00( x )		x.flag.OUT0 /* ALM0	(CN1-19,20)	*/
#define	KPX_MON_HWSOUT01( x )		x.flag.OUT1	/* ALM1	(CN1-21,22)	*/
#define	KPX_MON_HWSOUT02( x )		x.flag.OUT2 /* S01	(CN1-23,24) */
#define	KPX_MON_HWSOUT03( x )		x.flag.OUT3 /* SO2	(CN1-25,26) */
#define	KPX_MON_HWSOUT04( x )		x.flag.OUT4 /* SO3	(CN1-27,28) */
#define	KPX_MON_HWSOUT05( x )		x.flag.OUT5 /* SO4	(CN1-29,30) */
#define	KPX_MON_HWSOUT06( x )		x.flag.OUT6 /* SO5	(CN1-29,30) */
#define	KPX_MON_HWSOUT07( x )		0

#endif	/* #if CSW_BOARD_TYPE == SGD7S_TYPE		SGD7S（1軸対応）のみ	 */
/*--------------------------------------------------------------------------------------------------*/
/*		Un111 : 内部信号モニタ [-]																	*/
/*--------------------------------------------------------------------------------------------------*/
//#define	KPI_MON_HWSOUT_RELAYON		0
//#define	KPI_MON_HWSOUT_REGENON		0
/*--------------------------------------------------------------------------------------------------*/
#else	/* TEST BOARD */
/*--------------------------------------------------------------------------------------------------*/
/*		Un005 : 入力信号モニタ [-]																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_MON_HWSIN00( x )		x.flag.SI0
#define	KPX_MON_HWSIN01( x )		x.flag.SI1
#define	KPX_MON_HWSIN02( x )		x.flag.SI2
#define	KPX_MON_HWSIN03( x )		x.flag.SI3
#define	KPX_MON_HWSIN04( x )		x.flag.SI4
#define	KPX_MON_HWSIN05( x )		x.flag.SI5
#define	KPX_MON_HWSIN06( x )		x.flag.SI6
#define	KPX_MON_HWSIN07( x )		x.flag.SI7
/*--------------------------------------------------------------------------------------------------*/
/*		Un006 : 出力信号モニタ [-]																	*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_MON_HWSOUT00( x )		x.flag.AlmOut
#define	KPX_MON_HWSOUT01( x )		x.flag.SO1
#define	KPX_MON_HWSOUT02( x )		x.flag.SO2
#define	KPX_MON_HWSOUT03( x )		x.flag.SO3
#define	KPX_MON_HWSOUT04( x )		x.flag.AlmCd1
#define	KPX_MON_HWSOUT05( x )		x.flag.AlmCd2
#define	KPX_MON_HWSOUT06( x )		x.flag.AlmCd3
#define	KPX_MON_HWSOUT07( x )		0
/*--------------------------------------------------------------------------------------------------*/
/*		Un111 : 内部信号モニタ [-]																	*/
/*--------------------------------------------------------------------------------------------------*/
//#define	KPI_MON_HWSOUT_RELAYON		0
//#define	KPI_MON_HWSOUT_REGENON		0
/*--------------------------------------------------------------------------------------------------*/
#endif


/****************************************************************************************************/
/*																									*/
/*		<S1B5> Parallel -> Serial Bit Number Conversion												*/
/*																									*/
/****************************************************************************************************/
#define KPI_PINNO_NA (-1)		/* not available */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)

	/*----------------------------------------------------------------------------------------------*/
	/*		for SGD7S-AP Cn1(50pin)																	*/
	/*----------------------------------------------------------------------------------------------*/
	#define KPI_SI0PINNO (40)		/* SI0: S-ON */
	#define KPI_SI1PINNO (42)		/* SI1: P-OT */
	#define KPI_SI2PINNO (43)		/* SI2: N-OT */
	#define KPI_SI3PINNO (41)		/* SI3: P-CON */
	#define KPI_SI4PINNO (44)		/* SI4: ALMRST */
	#define KPI_SI5PINNO (45)		/* SI5: P-CL */
	/*----------------------------------------------------------------------------------------------*/
	#define KPI_SI6PINNO (46)		/* SI6: N-CL */
	#define KPI_SI7PINNO (-1)		/* Not Available */
	#define KPI_SI8PINNO (-2)		/* Not Available */
	#define KPI_SI9PINNO (-3)		/* Not Available */
	#define KPI_SI10PINNO (-4)		/* Not Available */
	#define KPI_SI11PINNO (-5)		/* Not Available */
	/*----------------------------------------------------------------------------------------------*/

#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) || (CSW_NETIF_TYPE == NETIF_CM_TYPE)

	#if CSW_BOARD_TYPE == SGD7S_TYPE

		/*------------------------------------------------------------------------------------------*/
		/*		for SGD7S Cn1(26 pin) 																*/
		/*------------------------------------------------------------------------------------------*/
		#define KPI_SI0PINNO (7)
		#define KPI_SI1PINNO (8)
		#define KPI_SI2PINNO (9)
		#define KPI_SI3PINNO (10)
		#define KPI_SI4PINNO (11)
		#define KPI_SI5PINNO (12)
		/*------------------------------------------------------------------------------------------*/
		#define KPI_SI6PINNO (-1)		/* Not available */
		#define KPI_SI7PINNO (-2)		/* Not available */
		#define KPI_SI8PINNO (-3)		/* Not available */
		#define KPI_SI9PINNO (-4)		/* Not available */
		#define KPI_SI10PINNO (-5)		/* Not available */
		#define KPI_SI11PINNO (-6)		/* Not available */
		/*------------------------------------------------------------------------------------------*/
	#elif CSW_BOARD_TYPE == SGD7W_TYPE

		/*------------------------------------------------------------------------------------------*/
		/*		for SGD7W Cn1(36 pin) 																*/
		/*------------------------------------------------------------------------------------------*/
		#define KPI_SI0PINNO (3)
		#define KPI_SI1PINNO (4)
		#define KPI_SI2PINNO (5)
		#define KPI_SI3PINNO (6)
		#define KPI_SI4PINNO (7)
		#define KPI_SI5PINNO (8)
		/*------------------------------------------------------------------------------------------*/
		#define KPI_SI6PINNO (9)
		#define KPI_SI7PINNO (10)
		#define KPI_SI8PINNO (11)
		#define KPI_SI9PINNO (12)
		#define KPI_SI10PINNO (13)
		#define KPI_SI11PINNO (14)
		/*------------------------------------------------------------------------------------------*/
	#endif

#else
	compile_error!!
#endif


/****************************************************************************************************/
/*																									*/
/*		<S1B5> Serial -> Parallel Bit Number Conversion												*/
/*																									*/
/****************************************************************************************************/
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)

	/*----------------------------------------------------------------------------------------------*/
	/*		for SGD7S-AP Cn1(50pin)																	*/
	/*----------------------------------------------------------------------------------------------*/
	#define KPI_SO1PINNO (25)		/* SO1: COIN/VCMP */
	#define KPI_SO2PINNO (27)		/* SO2: TGON */
	#define KPI_SO3PINNO (29)		/* SO3: S-RDY */
	#define KPI_SO4PINNO (-1)		/* Not available */
	#define KPI_SO5PINNO (-2)		/* Not available */
	#define KPI_SO6PINNO (-3)		/* Not available */
	/*----------------------------------------------------------------------------------------------*/

#elif (CSW_NETIF_TYPE == NETIF_M2_TYPE) || (CSW_NETIF_TYPE == NETIF_M3_TYPE) || (CSW_NETIF_TYPE == NETIF_CM_TYPE)

	#if CSW_BOARD_TYPE == SGD7S_TYPE

		/*------------------------------------------------------------------------------------------*/
		/*		for SGD7S Cn1(26 pin) 																*/
		/*------------------------------------------------------------------------------------------*/
		#define KPI_SO1PINNO (1)
		#define KPI_SO2PINNO (23)
		#define KPI_SO3PINNO (25)
		#define KPI_SO4PINNO (-1)		/* Not available */
		#define KPI_SO5PINNO (-2)		/* Not available */
		#define KPI_SO6PINNO (-3)		/* Not available */

	#elif CSW_BOARD_TYPE == SGD7W_TYPE

		/*------------------------------------------------------------------------------------------*/
		/*		for SGD7W Cn1(36 pin) 																*/
		/*------------------------------------------------------------------------------------------*/
		#define KPI_SO1PINNO (23)
		#define KPI_SO2PINNO (25)
		#define KPI_SO3PINNO (27)
		#define KPI_SO4PINNO (29)
		#define KPI_SO5PINNO (31)
		#define KPI_SO6PINNO (-1)		/* Not available */
	#endif

#else
	compile_error!!
#endif

/****************************************************************************************************/
/*																									*/
/*		<S1D6> Latch Definition																		*/
/*																									*/
/****************************************************************************************************/
#if CSW_BOARD_TYPE == SGD7S_TYPE
	/* for SGD7S-M2/SGD7S-M3 */
	#define	KPI_EXT1_LOWLEVEL  (0x04)
	#define	KPI_EXT2_LOWLEVEL  (0x05)
	#define	KPI_EXT3_LOWLEVEL  (0x06)
	#define	KPI_EXT1_HIGHLEVEL (0x0D)
	#define	KPI_EXT2_HIGHLEVEL (0x0E)
	#define	KPI_EXT3_HIGHLEVEL (0x0F)

#elif CSW_BOARD_TYPE == SGD7W_TYPE
	/* for SGD7W-M3 */
	#define	KPI_EXT1_LOWLEVEL  (0x03)
	#define	KPI_EXT2_LOWLEVEL  (0x04)
	#define	KPI_EXT3_LOWLEVEL  (0x05)
	#define	KPI_EXT1_HIGHLEVEL (0x0C)
	#define	KPI_EXT2_HIGHLEVEL (0x0D)
	#define	KPI_EXT3_HIGHLEVEL (0x0E)

#endif

/****************************************************************************************************/
/*		HAL API's																					*/
/****************************************************************************************************/
/* for I/O controls */
void HALdrv_InitSerialIO( void );
void HALdrv_InputSerialIO( void );
void HALdrv_OutputSerialIO( void );

#ifndef _TEST_BOARD_ /* 本基板用 */

//void HALdrv_Net7SegLedControl( UCHAR outData ); /* <S0A2> */
void HALdrv_Net7SegLedControl( UCHAR axis, UCHAR outData );
void HALdrv_NetConLedControl( BOOL ledOut );

#else /* _TEST_BOARD_ 削除予定 */
void HALdrv_OutputSerialIO_ScanC( void );
#endif


/* for Sequence I/O */
HWSIN HALdrv_ReadInputSignals( void );
ULONG HALdrv_ReadMonInputSignals( LONG axis );
void HALdrv_BrakeControl( LONG ax_no, BOOL sts );
//void HALdrv_WriteForceOutputSignals( ULONG outSignal, BOOL fout );	/* <S0CB> */
USHORT HALdrv_WriteForceOutputSignals( ULONG outSignal, BOOL fout );	/* <S0CB> */
HWSOUT HALdrv_ReadOuputSignals( void );

LONG 	HALdrv_GetPsBitNumber( LONG Pin );								/* <S1B5> */
LONG 	HALdrv_GetSpBitNumber( LONG Pin );								/* <S1B5> */

ULONG HALdrv_ReadMonAllInputSignals( void );				/* <S1C0> */
ULONG	HALdrv_ReadMonAllOutputSignals( void );				/* <S1C0> */

#if 0 /* Only INGRAM */
typedef enum HW_BOARDID_t {
	HW_IF_MODULE, HW_MB_MODULE, HW_FBB_MODULE, HW_COA_MODULE, HW_SDA_MODULE,
} HW_BOARDID_T;
ULONG HALdrv_ReadBoardID( HW_BOARDID_T Id );
#endif
ULONG HALdrv_ReadBoardID( void );	/*<S1A2>*/

/* for H/W Control I/O */
void HALdrv_RegeneControl( BOOL sts );
BOOL HALdrv_GetRegeneControlState( void );
//void HALdrv_RelayControl( BOOL sts ); /* <S0A2> */
void HALdrv_RelayControl( void *AsicHwReg, BOOL sts );
BOOL HALdrv_GetRelayOnAck( void );

/* for 5V Tolerant Buffer Standby Control <S01F> */
void HALdrv_Clear5VTBStandby( void );

/* for P-N voltage detections */
void HALdrv_StartAdConvert( void );
LONG HALdrv_GetDcVoltFromAd( void );

/* for getting RSW/DSW signals */
void HALdrv_InitIODevices( void );
UCHAR HALdrv_GetTransmissionByte( void );
UCHAR HALdrv_GetStationAddress( void );
ULONG HALdrv_GetDipSwitchSetting( void );

/* for IF LED signals */
//void HALdrv_OutputLED( INT LedOutputs ); /* 不使用 <Oka02> */

/* for H/W synchronization */
void HALdrv_EnableExtSyncInterrupt( void *pAsicHwReg );
void HALdrv_DisableExtSyncInterrupt( void *pAsicHwReg );
BOOL HALdrv_GetExtUnSyncStatus( void );
BOOL HALdrv_GetIntUDLStatus( UCHAR IntName, VOID* AsicHwReg);
VOID HALdrv_ReqIntUDLStatusClear( UCHAR IntName );
//void HALdrv_EnableServoAsicSync(UINT BaseCycle, UINT SyncCycle ); /* 不使用 <Oka02> */
//void HALdrv_OutputSoftwareResetSignal( void ); /* 不使用 <Oka02> */

/* for Check Post Output */
void HALdrv_InitCheckPost( void );
void HALdrv_GpioOutput( BOOL sts, LONG IoNo );
void HALdrv_OutputDebugSP(UINT bit_no);
void HALdrv_ClearDebugSP(UINT bit_no);
void HALdrv_ReverceTestPls( void );
VULONG *HALdrv_GetCheckPostAddr( void );	/*<S1F6>*/

/* for OH_LED Control */
void HALdrv_OhLedControl( BOOL sts );

/* for Check IF WDTERR */
void HALdrv_KickIfWdt( void );

/* ---futami (R)S/W Version Check---> */
typedef enum  BOOT_VERSION{
	R_BOOTSOFT_VERSION = 0,
	R_BOOTSTRAP_VERSION
}BOOT_VERSION;

void HALdrv_GetNorflashRegister( void *RegisterAddress );
ULONG HALdrv_GetBootVersion( UINT Index, BOOT_VERSION Val );
/* <---futami (R)S/W Version Check--- */

/* for M-III PRGSEL Setting *//* <S0E4> */
void HALdrv_SetM3PrgSel( BOOL sel );

void HALdrv_ForceOutSignalsMask( void );	/* <S198> */

#endif	// _HW_DEVICE_IF_H
/***************************************** end of file **********************************************/
