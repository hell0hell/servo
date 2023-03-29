/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Dma.h : DMA Interface Definition File														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	‹@ ”\ : DMA Common Definitions.																	*/
/*																									*/
/*			1. 																						*/
/*			2. 																						*/
/*			3. 																						*/
/*			4. 																						*/
/*			5. 																						*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.07.19  K.Ozaki		##Cortex-R4##											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef			___TST_DMA___
#define			___TST_DMA___

/****************************************************************************************************/
/*																									*/
/*		DMA Interface Definition																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define CPU_DMA_N_CH 		(4)				            /* Channel Number */
/*--------------------------------------------------------------------------------------------------*/
#define CPU_AXIDMA_REGBASE   (CONFIG_AXIDMA_REGBASE)	/* DMA Register BaseAddr */
#define CPU_AXIDMA_REGOFFSET (COMPILE_ERROR)			/* DMA Register BaseOffset */
#define DMA_UNIT_OFFSET     (0x400)						/* DMA Unit OFFSET */
#define DMA_CHANNEL_OFFSET  (0x40)						/* DMA Channel OFFSET */
#define DMA_CHANNEL_OFFSET2 (0x20)						/* DMA Channel OFFSET for Skip Register */

/*--------------------------------------------------------------------------------------------------*/
/** DMA Register Mapping 																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	VUINT32 N0SA0;								/**< 0x00:Next0 Source Address Register */
	VUINT32 N0DA0;								/**< 0x04:Next0 Destination Address Register */
	VUINT32 N0TB0;								/**< 0x08:Next0 Transaction Byte Register */
	VUINT32 N0SA1;								/**< 0x0C:Next1 Source Address Register */
	VUINT32 N0DA1;								/**< 0x10:Next1 Destination Address Register */
	VUINT32 N0TB1;								/**< 0x14:Next1 Transaction Byte Register */
	VUINT32 CRSA;								/**< 0x18:Current Source Address Register */
	VUINT32 CRDA;								/**< 0x1C:Current Destination Address Register */
	VUINT32 CRTB;								/**< 0x20:Current Transaction Byte Register */
	VUINT32 CHSTAT;								/**< 0x24:Channel Status Register */
	VUINT32 CHCTRL;								/**< 0x28:Channel Control Register */
	VUINT32 CHCFG;								/**< 0x2c:Channel Configuration Register */
	VUINT32 CHITVL;								/**< 0x30:Channel Interval Register */
	VUINT32 CHEXT;								/**< 0x34:Channel Extension Register */
	VUINT32 NXLA;								/**< 0x38:Next Link Address Register*/
	VUINT32 CRLA;								/**< 0x3C:Next Current Link Address Register*/
	VUINT32	Dummy1[(0x200-0x40)/4];				/**< 0x40-0x1FF: Dummy */
	VUINT32 Dummy2;								/**< 0x200:Dummy */
	VUINT32 Dummy3;								/**< 0x204:Dummy */
	VUINT32 Dummy4;								/**< 0x208:Dummy */
	VUINT32 Dummy5;								/**< 0x20C:Dummy */
	VUINT32 Dummy6[(0x300-0x210)/4];			/**< 0x210-0x2FF: Dummy */
	VUINT32 DCTRL;								/**< 0x300:DMAC Control Register */
	VUINT32 Dummy7;								/**< 0x304:Dummy */
	VUINT32 Dummy8;								/**< 0x308:Dummy */
	VUINT32 Dummy9;								/**< 0x30C:Dummy */
	VUINT32 DST_EN;								/**< 0x310:DMAC Enable Status Register */
	VUINT32 DST_ER;								/**< 0x314:DMAC Error Status Register */
	VUINT32 DST_END;							/**< 0x318:DMAC End Status Register */
	VUINT32 DST_TC;								/**< 0x31C:DMAC Terminal Count Register */
	VUINT32 DST_SUS;							/**< 0x320:DMAC Suspend Status Register */
} tDMA_REG;


/*--------------------------------------------------------------------------------------------------*/
/*		Register Base Address Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define AXIDMA_UNIT0_REGBASE (tDMA_REG *)( CPU_AXIDMA_REGBASE + (DMA_UNIT_OFFSET * 0))

#define AXIDMA_CH0_REGBASE (tDMA_REG *)((UINT)(ureg) + (DMA_CHANNEL_OFFSET * 0))
#define AXIDMA_CH1_REGBASE (tDMA_REG *)((UINT)(ureg) + (DMA_CHANNEL_OFFSET * 1))
#define AXIDMA_CH2_REGBASE (tDMA_REG *)((UINT)(ureg) + (DMA_CHANNEL_OFFSET * 2))
#define AXIDMA_CH3_REGBASE (tDMA_REG *)((UINT)(ureg) + (DMA_CHANNEL_OFFSET * 3))

/*--------------------------------------------------------------------------------------------------*/
/*		Register Access Macro Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define DMA_REG_WR(reg, data) (ureg)->##reg = (data)
#define DMA_REG_RD(reg, data) (data) = (ureg)->##reg


/*--------------------------------------------------------------------------------------------------*/
/*  	Dma.c : MICROSSP Cpu DMA I/F Driver															*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitCpuDMA(void);					          /* Initialize DMA							*/
void	KpxStopCpuDMA(void);					          /* Stop All DMA							*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxSifStopRxDma0(void);					          /* Stop UART Rx DMA */
void	KpxSifStopTxDma0(void);                           /* Stop UART Tx DMA */
void	KpxSifStopRxDma1(void);                           /* Stop UART Rx DMA */
void	KpxSifStopTxDma1(void);                           /* Stop UART Tx DMA */
/*--------------------------------------------------------------------------------------------------*/
void	KpxSifSetRxDma0(UINT SAdr, UINT DAdr, LONG Cnt);  /* Set UART Rx DMA */
void	KpxSifSetTxDma0(UINT SAdr, UINT DAdr, LONG Cnt);  /* Set UART Tx DMA */
void	KpxSifSetRxDma1(UINT SAdr, UINT DAdr, LONG Cnt);  /* Set UART Rx DMA */
void	KpxSifSetTxDma1(UINT SAdr, UINT DAdr, LONG Cnt);  /* Set UART Tx DMA */
/*--------------------------------------------------------------------------------------------------*/
void	KpxSifStartRxDma0(void);                          /* Start UART Rx DMA */
void	KpxSifStartTxDma0(void);                          /* Start UART Tx DMA */
void	KpxSifStartRxDma1(void);                          /* Start UART Rx DMA */
void	KpxSifStartTxDma1(void);                          /* Start UART Tx DMA */
/*--------------------------------------------------------------------------------------------------*/
LONG	KpxSifChkRxDma0Tc(void);                          /* Check Rx DMA Transaction Completion */
LONG	KpxSifChkTxDma0Tc(void);                          /* Check Tx DMA Transaction Completion */
LONG	KpxSifChkRxDma1Tc(void);                          /* Check Rx DMA Transaction Completion */
LONG	KpxSifChkTxDma1Tc(void);                          /* Check Tx DMA Transaction Completion */

#endif
