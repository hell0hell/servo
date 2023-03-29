/************************************************************************************************************/
/*																											*/
/*																											*/
/*		ARMPF_SCU.h : MercurySoC ペリフェラル SCUヘッダ														*/
/*																											*/
/*																											*/
/************************************************************************************************************/
/*																											*/
/*																											*/
/*		システムコントロールユニット(SCU)関連ヘッダファイル													*/
/*																											*/
/*																											*/
/************** Copyright (C) Yaskawa Electric Corporation **************************************************/
/*																											*/
/*		Rev.1.00 : 2013.07.30  Y.Tanaka		初版															*/
/*																											*/
/************************************************************************************************************/
#include	"ARMPF.h"


#ifndef	_ARMPF_SCU_H_
#define	_ARMPF_SCU_H_

#define	ARMPF_SCU_BASE	0xEFF86000

/*----------------------------------------------------------------------------------------------------------*/
/*																											*/
/*		SCUレジスタ設定値																					*/
/*																											*/
/*----------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------*/
/*		SCU register																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	ARMPF_SCU_DEVICE_ID		(ARMPF_SCU_BASE + 0x000)		/* デバイスIDコードレジスタ					*/
#define	ARMPF_SCU_SOFT_RST		(ARMPF_SCU_BASE + 0x004)		/* ソフトリセットレジスタ					*/
#define	ARMPF_SCU_CPU_HALT		(ARMPF_SCU_BASE + 0x008)		/* PCIexブート制御レジスタ					*/
#define	ARMPF_SCU_CLK_EN		(ARMPF_SCU_BASE + 0x00C)		/* クロック制御用レジスタ					*/
#define	ARMPF_SCU_CLK_SEL		(ARMPF_SCU_BASE + 0x010)		/* クロック切り替えレジスタ					*/
#define	ARMPF_SCU_MODE			(ARMPF_SCU_BASE + 0x014)		/* モード確認用レジスタ						*/
#define	ARMPF_SCU_REMAP			(ARMPF_SCU_BASE + 0x018)		/* リマップ機能制御レジスタ					*/
#define	ARMPF_SCU_PCIE_INT		(ARMPF_SCU_BASE + 0x01C)		/* PCIexエンドポイント割込みレジスタ		*/
#define	ARMPF_SCU_PCIE_INTMSK	(ARMPF_SCU_BASE + 0x020)		/* PCIexエンドポイント割込みマスクレジスタ	*/
#define	ARMPF_SCU_PCIE_INTSEL	(ARMPF_SCU_BASE + 0x024)		/* PCIex割込みUDL-IF選択レジスタ			*/
#define	ARMPF_SCU_WDT_MODE		(ARMPF_SCU_BASE + 0x028)		/* UDL-WDTリセット制御レジスタ				*/
#define	ARMPF_SCU_5TBUF_EN		(ARMPF_SCU_BASE + 0x02C)		/* 5Vトレラントバッファスタンバイ制御レジスタ	*/
#define	ARMPF_SCU_RESERVE0		(ARMPF_SCU_BASE + 0x030)		/* reserved									*/
#define	ARMPF_SCU_NMI_SET		(ARMPF_SCU_BASE + 0x034)		/* NMI設定レジスタ							*/
#define	ARMPF_SCU_INTSYS1_EN	(ARMPF_SCU_BASE + 0x038)		/* 周辺回路割込み入力同期/非同期設定1レジスタ	*/
#define	ARMPF_SCU_INTSYS2_EN	(ARMPF_SCU_BASE + 0x03C)		/* 周辺回路割込み入力同期/非同期設定2レジスタ	*/
#define	ARMPF_SCU_VICIFSYN_EN	(ARMPF_SCU_BASE + 0x040)		/* CPU-VIC間の同期/非同期設定レジスタ		*/
#define	ARMPF_SCU_PCIE_CLKSEL	(ARMPF_SCU_BASE + 0x044)		/* PCIexクロック選択レジスタ				*/
#define	ARMPF_SCU_RESET_STATUS	(ARMPF_SCU_BASE + 0x048)		/* リセットステータスレジスタ				*/
#define	ARMPF_SCU_UDLCNT		(ARMPF_SCU_BASE + 0x04C)		/* M3-UDLPRGSEL設定レジスタ					*/
#define	ARMPF_SCU_RESERVE1		(ARMPF_SCU_BASE + 0x050)		/* reserved									*/
#define	ARMPF_SCU_CPUSS_TRIGINT	(ARMPF_SCU_BASE + 0x054)		/* CTIモジュールからのクロストリガ割り込み要因レジスタ	*/
#define	ARMPF_SCU_GBETHER_CONT	(ARMPF_SCU_BASE + 0x058)		/* GbEtherモード設定レジスタ				*/
#define	ARMPF_SCU_UDLINT_CONT	(ARMPF_SCU_BASE + 0x05C)		/* PCIexに対するUDL割込み選択レジスタ		*/
#define	ARMPF_SCU_UDLINT_STATUS	(ARMPF_SCU_BASE + 0x060)		/* UDL割込み要因レジスタ					*/

#define	ARMPF_SCU_APBREG15		(0xEFFF027C)					/* I2C動作制御レジスタ(ES3以降有効)		<S152>			*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_DVICE_ID																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	JL086ID				0x086A								/* Device ID								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_SOFT_RST																						*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SOFT_RESET			0									/* ソフトリセット実行						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CPU_HALT 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	HALT_OFF			1									/* HALT解除									*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CLK_EN 	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	UDL_NET_CLK			0									/* M3UDLクロック							*/
#define	UDL_SERVO_CLK		1									/* ServoUDLクロック							*/
#define	ADC_CLK				2									/* ADCクロック								*/
#define	GBETHER_CLK			3									/* GbEtherクロック							*/
#define	DDR3_CLK			4									/* DDR3クロック								*/
#define	USB_CLK				5									/* USBクロック								*/
#define	BUSOUT_CLK			6									/* BUSCLK_OUTクロック						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CLK_SEL 	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	CLK_MODE1		0										/* モード1									*/
#define	CLK_MODE2		1										/* モード2									*/
#define	CLK_MODE3		2										/* モード3									*/
#define	CLK_MODE4		3										/* モード4									*/
#define	I2C_CKSEL		0										/* I2Cクロック周波数切り替え				*/
																/* モード1：357Kbps							*/
																/* モード2：384Kbps							*/
																/* モード3：400Kbps							*/
																/* モード4：設定禁止						*/
#define	DDR3_CKSEL		2										/* DDR3クロック周波数切り替え				*/
																/* モード1：600Mbps							*/
																/* モード2：800Mbps							*/
																/* モード3：1200Mbps						*/
																/* モード4：設定禁止						*/
#define	OUT_CKSEL		4										/* 外部バスクロック周波数切り替え			*/
																/* モード1：BUSCLK(125MHz)					*/
																/* モード2：BUSCLK/2						*/
																/* モード3：BUSCLK/4						*/
																/* モード4：BUSCLK/8						*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_MODE		 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	MOVE_MODE		0										/* 動作モード								*/
#define	VECTOR_MODE		4										/* VECTORモード								*/
#define	CPU_CLK_MODE	6										/* CPUクロック周波数選択					*/
#define	OSC_MODE		7										/* OSCバッファモード						*/
#define	MODE_VNDMD		0										/* ルネサステストモード						*/
#define	MODE_RSV0		1										/* リザーブモード							*/
#define	MODE_RSV1		2										/* リザーブモード							*/
#define	MODE_TMD3		3										/* シミュレーションモード(TMD3)				*/
#define	MODE_TMD4		4										/* シミュレーションモード(TMD4)				*/
#define	MODE_DBGMD		5										/* UDL_Servoデバッグモード					*/
#define	MODE_MDBGMD		6										/* UDL_M3デバッグモード						*/
#define	MODE_NORMAL		7										/* ノーマルモード							*/
#define	BOOT_PROM		0										/* パラレルROMブート(LOWVEC)				*/
#define	BOOT_PCI		1										/* PCIexブート(HIVEC)						*/
#define	BOOT_SROM		2										/* シリアルROMブート(HIVEC)					*/
#define	BOOT_M3			3										/* M3ブート(HIVEC)							*/
#define	CPUCLK_250		0										/* CPU Clock 250MHz							*/
#define	CPUCLK_500		1										/* CPU Clock 500MHz							*/
#define	OSC_XTOFF		0										/* OSCバッファ未使用						*/
#define	OSC_X25MD		1										/* X25OSC使用								*/
#define	OSC_X30MD		2										/* X30OSC使用								*/
#define	OSC_XTON		3										/* 発振子利用								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_REMAP	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	REMAP_OFF		0										/* リマップ無効								*/
#define	REMAP_ON		1										/* リマップ有効								*/
#define	DDR_REMAP		0										/* DDR REMAP								*/
#define	AXI_REMAP		1										/* AXI-RAM REMAP(read only)					*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_PCIE_INT, SSPRO7_PCIE_INTMSK																	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	PCIE_INTA		0										/* INTA_EP割込み発行/マスク					*/
#define	PCIE_INTB		1										/* INTB_EP割込み発行/マスク					*/
#define	PCIE_INTC		2										/* INTC_EP割込み発行/マスク					*/
#define	PCIE_INTD		3										/* INTD_EP割込み発行/マスク					*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_PCIE_INTSEL 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	PCIE_INTSEL		0										/* PCIex割込み選択レジスタ					*/
#define	PCIE_INTMODE	4										/* 割込み信号タイミング制御レジスタ			*/
#define	PCIE_TLPEND_INT	0										/* PCIE_TLPEND_INTをUDLに接続				*/
#define	PCIEX_GPO_0		1										/* PCIEX_GPO_0をUDLに接続					*/
#define	PCIE_INTA_RC	2										/* PCIE_INTA_RCをUDLに接続					*/
#define	PCIE_INTB_RC	3										/* PCIE_INTB_RCをUDLに接続					*/
#define	PCIE_INTC_RC	4										/* PCIE_INTC_RCをUDLに接続					*/
#define	PCIE_INTD_RC	5										/* PCIE_INTD_RCをUDLに接続					*/
#define	PCIE_INTMSI_RC	6										/* PCIE_INTMSI_RCをUDLに接続				*/
#define	PCIE_MSG_INT_RC	7										/* PCIE_MSG_INT_RCをUDLに接続				*/
#define	INT_8CLK_PLS	0										/* ACLKの8クロック幅で出力					*/
#define	INT_LVL01		1										/* レベル出力								*/
#define	INT_3CLK_PLS	2										/* ACLKの3クロック幅で出力					*/
#define	INT_LVL11		3										/* レベル出力								*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_WDT_MODE	 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_5TBUF_EN	 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	OV_CNT		0											/* OV端子スタンバイ制御レジスタ				*/
#define	OC1_CNT		1											/* OC1端子スタンバイ制御レジスタ			*/
#define	OC2_CNT		2											/* OC2端子スタンバイ制御レジスタ			*/
#define	SYNCL_CNT	3											/* SUNCL端子スタンバイ制御レジスタ			*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_NMI_SET	 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_INTSYS1_EN 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_INTSYS2_EN 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_VICIFSYN_EN 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_PCIE_CLKSEL 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_RESET_STATUS																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SW_RST_STS		1										/* ソフトウェアリセット						*/
#define	WDT_RST_STS		2										/* UDL WDTリセット							*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_UDLCNT	 																					*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_CPUSS_TRIGINT																				*/
/*----------------------------------------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_GBETHER_CONT																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	GB_PHYMODE		0										/* GbEther通信モード設定					*/
#define	GB_SPEED		4										/* 通信速度設定								*/
#define	GB_RMIIMODE		8										/* RMII転送時の転送モード設定				*/
#define	GB_CLKSEL		12										/* RGMII転送時のクロック制御設定			*/
#define	GB_PHY_MII		0x0										/* MIIモード								*/
#define	GB_PHY_RMII		0x1										/* RMII通常モード							*/
#define	GB_PHY_RMIIRX	0x9										/* RMII送信クロックRX入力モード				*/
#define	GB_PHY_GMII		0xA										/* GMIIモード								*/
#define	GB_PHY_RGMII	0xB										/* RGMIIモード								*/
#define	GB_SPD_10M		0										/* 通信速度10Mbps							*/
#define	GB_SPD_100M		1										/* 通信速度100Mbps							*/
#define	GB_SPD_1G		2										/* 通信速度1Gbps							*/



/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_UDLINT_CONT 																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	PCI_EP_A_SEL	0										/* PCI EP割込みAに出力する割込み選択		*/
#define	PCI_EP_B_SEL	4										/* PCI EP割込みBに出力する割込み選択		*/
#define	PCI_EP_C_SEL	8										/* PCI EP割込みCに出力する割込み選択		*/
#define	PCI_EP_D_SEL	12										/* PCI EP割込みDに出力する割込み選択		*/
#define	UDL_INTA		0x0										/* UDL_INTA(Servo)を出力					*/
#define	UDL_INTB		0x1										/* UDL_INTB(Servo)を出力					*/
#define	UDL_INTC		0x2										/* UDL_INTC(Servo)を出力					*/
#define	UDL_INTN		0x3										/* UDL_INTN(Servo)を出力					*/
#define	UDL_INT1		0x4										/* UDL_INT1(Servo)を出力					*/
#define	UDL_INT2		0x5										/* UDL_INT2(Servo)を出力					*/
#define	UDL_M3INT0		0x6										/* UDL-M3_INT0(Network)を出力 				*/
#define	UDL_M3INT1		0x7										/* UDL-M3_INT1(Network)を出力 				*/
#define	UDL_ALLSERVO	0x8										/* Servo割り込みのすべての OR 出力 			*/
#define	UDL_ALLM3		0x9										/* M3UDL割り込みのすべての OR 出力 			*/

/*----------------------------------------------------------------------------------------------------------*/
/*		SSPRO7_UDLINT_STATUS																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	UDL_INTA_OM		0x01									/* UDL_INTA(Servo)を出力					*/
#define	UDL_INTB_OM		0x02									/* UDL_INTB(Servo)を出力					*/
#define	UDL_INTC_OM		0x04									/* UDL_INTC(Servo)を出力					*/
#define	UDL_INTN_OM		0x08									/* UDL_INTN(Servo)を出力					*/
#define	UDL_INT1_OM		0x10									/* UDL_INT1(Servo)を出力					*/
#define	UDL_INT2_OM		0x20									/* UDL_INT2(Servo)を出力					*/
#define	UDL_M3INT0_OM	0x40									/* UDL-M3_INT0(Network)を出力 				*/
#define	UDL_M3INT1_OM	0x80									/* UDL-M3_INT1(Network)を出力 				*/


/*----------------------------------------------------------------------------------------------------------*/
/*		APBREG15	<S152>																					*/
/*----------------------------------------------------------------------------------------------------------*/
#define	I2C_LOW			12										/* */
#define	LOW_DISABLE		0										/* Low検出無効(ES2と同じ) */
#define	LOW_ENAABLE		1										/* Low検出有効 */
/*----------------------------------------------------------------------------------------------------------*/
#define	I2C_RATE		8										/* */
#define	RATE_D1			0									/* 00:D1(ES2と同じ) */
#define	RATE_D2			1									/* 01:D2 */
#define	RATE_D3			2									/* 10:D3 */
#define	RATE_D4			3									/* 11:D4(for High speed mode */
/*----------------------------------------------------------------------------------------------------------*/
#define	I2C_CK			0										/* */
#define	CLK_738			0x00									/* 7.38MHz */
#define	CLK_750			0x01									/* 7.50MHz */
#define	CLK_762			0x02									/* 7.62MHz */
#define	CLK_774			0x03									/* 7.74MHz */
#define	CLK_787			0x04									/* 7.87MHz */
#define	CLK_800			0x05									/* 8.00MHz */
#define	CLK_814			0x06									/* 8.14MHz */
#define	CLK_828			0x07									/* 8.28MHz */
#define	CLK_842			0x08									/* 8.42MHz */
#define	CLK_857			0x09									/* 8.57MHz */
#define	CLK_873			0x0A									/* 8.73MHz */
#define	CLK_889			0x0B									/* 8.89MHz */
#define	CLK_906			0x0C									/* 9.06MHz */
#define	CLK_923			0x0D									/* 9.23MHz */
#define	CLK_941			0x0E									/* 9.41MHz */
#define	CLK_960			0x0F									/* 9.60MHz */
/*----------------------------------------------------------------------------------------------------------*/



/*----------------------------------------------------------------------------------------------------------*/
/*		API MACRO																							*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_GET_ID()		(ARMPF_REG_READ( ARMPF_SCU_DEVICE_ID ) & 0xFFFF)			/* JL-086ID取得				*//* <S119>ES4以降対応 *//* コード整理<S191> */
#define	SCU_GET_VER()		(ARMPF_REG_READ( ARMPF_SCU_DEVICE_ID ) & 0xFFFF) >> 16 )	/* JL-086 ES version取得	*//* <S191>ES4以降対応 */
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SOFTRESET()		ARMPF_REG_WRITE( ARMPF_SCU_SOFT_RST, SOFT_RESET )	/* JL-086ソフトリセット		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_CPU_HALT()		ARMPF_REG_WRITE( ARMPF_SCU_CPU_HALT, HALT_OFF )		/* PCIブート時CPUHALT解除	*/
/*----------------------------------------------------------------------------------------------------------*/
#if 1
#define	SCU_CLK_START( x )	ARMPF_REG_BITCLR( ARMPF_SCU_CLK_EN, x )				/* モジュールクロック供給	*/
#define	SCU_CLK_STOP( x )	ARMPF_REG_BITSET( ARMPF_SCU_CLK_EN, x )				/* モジュールクロック停止	*/
#else
#define	SCU_CLK_START( x )					/* モジュールクロック供給	*/
#define	SCU_CLK_STOP( x )					/* モジュールクロック停止	*/
#endif
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_CLK_SEL( x, mode )	ARMPF_REG_VALSET( ARMPF_SCU_CLK_SEL, (mode << x) )	/* モジュールクロック選択	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_GET_MODE( mode )	( ARMPF_REG_READ(ARMPF_SCU_MODE) >> mode )			/* 動作モード取得			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_REMAP( ram )	ARMPF_REG_WRITE( ARMPF_SCU_REMAP, (REMAP_ON << ram) )	/* リマップ実行			*/
#define	SCU_GET_REMAP( ram )	( ARMPF_REG_READ(ARMPF_SCU_REMAP) >> ram )		/* リマップ状態取得			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_PCIEINT( x )	ARMPF_REG_BITSET( ARMPF_SCU_PCIE_INT, x )		/* PCIINT発行				*/
#define	SCU_CLR_PCIEINT( x )	ARMPF_REG_BITCLR( ARMPF_SCU_PCIE_INT, x )		/* PCIINTクリア				*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_MASK_PCIEINT( x )	ARMPF_REG_BITSET( ARMPF_SCU_PCIE_INTMSK, x )	/* PCIINTマスク				*/
#define	SCU_MASKCLR_PCIEINT( x )	ARMPF_REG_BITCLR( ARMPF_SCU_PCIE_INTMSK, x )	/* PCIINTマスク無効		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_PCIE_INTSEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_PCIE_INTSEL, (x << PCIE_INTSEL) )	/* PCIex割込み選択レジスタセット	*/
#define	SCU_SET_PCIE_INTMODE( x )	ARMPF_REG_VALSET( ARMPF_SCU_PCIE_INTSEL, (x << PCIE_INTMODE) )	/* 割込み信号タイミングセット	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_ENABLE_WDT_RST()	ARMPF_REG_BITSET( ARMPF_SCU_WDT_MODE, 0 )		/* UDL_WDTリセット有効		*/
#define	SCU_DISABLE_WDT_RST()	ARMPF_REG_BITCLR( ARMPF_SCU_WDT_MODE, 0 )		/* UDL_WDTリセット無効		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_5TBUF_EN( x )	ARMPF_REG_BITSET( ARMPF_SCU_5TBUF_EN, x )		/* 5Vトレラントバッファスタンバイ	*/
#define	SCU_CLR_5TBUF_EN( x )	ARMPF_REG_BITCLR( ARMPF_SCU_5TBUF_EN, x )		/* 5Vトレラントバッファ無効			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_ENABLE_FIQ_MASK()	ARMPF_REG_BITSET( ARMPF_SCU_NMI_SET, 0 )		/* FIQマスク有効			*/
#define	SCU_DISABLE_FIQ_MASK()	ARMPF_REG_BITCLR( ARMPF_SCU_NMI_SET, 0 )		/* FIQマスク無効			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_INTSYS1_EN( x )		ARMPF_REG_WRITE( ARMPF_SCU_INTSYS1_EN, x )		/* 周辺回路割込み入力同期/非同期設定1	*/
#define	SCU_INTSYS2_EN( x )		ARMPF_REG_WRITE( ARMPF_SCU_INTSYS2_EN, x )		/* 周辺回路割込み入力同期/非同期設定2	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_VICIFSYN()		ARMPF_REG_BITSET( ARMPF_SCU_VICIFSYN_EN, 0 )	/* CPU-VIC間の同期設定		*/
#define	SCU_CLR_VICIFSYN()		ARMPF_REG_BITCLR( ARMPF_SCU_VICIFSYN_EN, 0 )	/* CPU-VIC間の非同期設定	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_PCIECLK_IN()		ARMPF_REG_BITSET( ARMPF_SCU_PCIE_CLKSEL, 0 )	/* PCIe内部クロック供給		*/
#define	SCU_PCIECLK_OUT()		ARMPF_REG_BITCLR( ARMPF_SCU_PCIE_CLKSEL, 0 )	/* PCIe外部差動クロック入力	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_READ_RST_STATUS()	ARMPF_REG_READ(ARMPF_SCU_RESET_STATUS)			/* リセットステータスリード	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_UDLM3_PRGSEL()	ARMPF_REG_WRITE( ARMPF_SCU_UDLCNT, 1 )			/* M3UDL PRGSEL有効			*/
#define	SCU_CLR_UDLM3_PRGSEL()	ARMPF_REG_WRITE( ARMPF_SCU_UDLCNT, 0 )			/* M3UDL PRGSEL無効			*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_READ_CPUSS_TRIGINT()	ARMPF_REG_READ(ARMPF_SCU_CPUSS_TRIGINT)		/* クロストリガ割込み要因リード	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_GB_PHYMODE( x )	ARMPF_REG_VALSET( ARMPF_SCU_GBETHER_CONT, (x << GB_PHYMODE) )	/* GbEther通信モード設定		*/
#define	SCU_SET_GB_SPEED( x )	ARMPF_REG_VALSET( ARMPF_SCU_GBETHER_CONT, (x << GB_SPEED) )		/* 通信速度設定					*/
#define	SCU_SET_GB_HALF_DUPLX()	ARMPF_REG_BITSET( ARMPF_SCU_GBETHER_CONT, GB_RMIIMODE )			/* RMII転送時半二重転送			*/
#define	SCU_SET_GB_FULL_DUPLX()	ARMPF_REG_BITCLR( ARMPF_SCU_GBETHER_CONT, GB_RMIIMODE )			/* RMII転送時全二重転送			*/
#define	SCU_SET_GB_CLK_REVPHASE()	ARMPF_REG_BITSET( ARMPF_SCU_GBETHER_CONT, GB_CLKSEL )		/* RGMII転送時クロック逆位相	*/
#define	SCU_SET_GB_CLK_COPHASE()	ARMPF_REG_BITCLR( ARMPF_SCU_GBETHER_CONT, GB_CLKSEL )		/* RGMII転送時クロック同位相	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_PCI_EP_A_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_A_SEL) )	/* PCI EP割込みAに出力する割込み設定		*/
#define	SCU_PCI_EP_B_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_B_SEL) )	/* PCI EP割込みBに出力する割込み設定		*/
#define	SCU_PCI_EP_C_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_C_SEL) )	/* PCI EP割込みCに出力する割込み設定		*/
#define	SCU_PCI_EP_D_SEL( x )	ARMPF_REG_VALSET( ARMPF_SCU_UDLINT_CONT, (x << PCI_EP_D_SEL) )	/* PCI EP割込みDに出力する割込み設定		*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_READ_UDLINT_STATUS()	ARMPF_REG_READ(ARMPF_SCU_UDLINT_STATUS)		/* UDL INT割込み要因リード	*/
/*----------------------------------------------------------------------------------------------------------*/
#define	SCU_SET_APBREG15( a, b, c )	ARMPF_REG_VALSET( ARMPF_SCU_APBREG15, ( (a)<<(I2C_LOW) | (b)<<(I2C_RATE) | (c)<<(I2C_CK) ) )		/* <S152> */

#endif
/*-------end of file----------------------------------------------------------------------------------------*/
