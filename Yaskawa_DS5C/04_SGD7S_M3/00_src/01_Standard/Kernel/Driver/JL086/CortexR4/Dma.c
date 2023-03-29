/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Dma.c : DMA Interface Definition File														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : JL086 DMA Interface.																	*/
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
/*		Rev.1.00 : 2013.07.21  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "config.h"
#include "Uart.h"
#include "Dma.h"

/****************************************************************************************************/
/*																									*/
/*		Common Definitions																			*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		CHSTAT Bit Mapping Definition																*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
#define EN (1<<0)
#define RQST (1<<1)
#define TACT (1<<2)
#define SUS (1<<3)
/*--------------------------------------------------------------------------------------------------*/
#define ER (1<<4)
#define END (1<<5)
#define TC (1<<6)
#define SR (1<<7)
/*--------------------------------------------------------------------------------------------------*/
#define DL (1<<8)
#define DW (1<<9)
#define DER (1<<10)
#define MODE (1<<11)
/*--------------------------------------------------------------------------------------------------*/
#define INTM (1<<16)
//#define DMARQM (1<<17)
//#define SWPRQ (1<<18)
//#define DNUM (1<<24)

/*--------------------------------------------------------------------------------------------------*/
/*		CHCTRL Bit Mapping Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define SETEN  (1<<0)
#define CLREN  (1<<1)
#define STG    (1<<2)
#define SWRST  (1<<3)
/*--------------------------------------------------------------------------------------------------*/
#define CLRRQ  (1<<4)
#define CLREND (1<<5)
#define CLRTC  (1<<6)
//#define CLRDER (1<<7)
/*--------------------------------------------------------------------------------------------------*/
#define SETSUS (1<<8)
#define CLRSUS (1<<9)
/*--------------------------------------------------------------------------------------------------*/
//#define SETREN (1<<12)
//#define SETSSWPRQ (1<<14)
/*--------------------------------------------------------------------------------------------------*/
#define SETINTM (1<<16)
#define CLRINTM (1<<17)
/*--------------------------------------------------------------------------------------------------*/
//#define SETDMARQM (1<<18)
//#define CLRDMARQM (1<<19)

/*--------------------------------------------------------------------------------------------------*/
/*		CHCFG Bit Mapping Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define SEL0 (1<<0)
#define REGD (1<<3)
/*--------------------------------------------------------------------------------------------------*/
#define LEN (1<<4)
#define HEN (1<<5)
#define LVL (1<<6)
/*--------------------------------------------------------------------------------------------------*/
#define AM0 (1<<8)
//#define DRRP (1<<11)
/*--------------------------------------------------------------------------------------------------*/
//#define SDS0 (1<<12)
//#define SDS3 (1<<15)
/*--------------------------------------------------------------------------------------------------*/
#define DDS0 (1<<16)
#define DDS3 (1<<19)
/*--------------------------------------------------------------------------------------------------*/
#define SAD (1<<20)
#define DAD (1<<21)
#define TM (1<<22)
#define WONLY (1<<23)
/*--------------------------------------------------------------------------------------------------*/
#define DEM  (1<<24)
#define TCM  (1<<25)
#define DIM  (1<<26)
#define SBE  (1<<27)
/*--------------------------------------------------------------------------------------------------*/
#define RSEL (1<<28)
#define RSW  (1<<29)
#define REN  (1<<30)
#define DMA  (1<<31)



/****************************************************************************************************/
/*																									*/
/*		DMA Procedures for UART 																	*/ 
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define LPX_STOP_DMA( base, ch ) {            \
tDMA_REG *ureg;                               \
UINT	data;                                 \
		ureg = (base);                        \
		ureg = AXIDMA_CH ##ch## _REGBASE;     \
		DMA_REG_WR( CHCTRL, CLREN );          \
		do {                                  \
			DMA_REG_RD( CHSTAT, data );       \
		} while ( data & (TACT | EN) );       \
		DMA_REG_WR( CHCTRL, SWRST );          \
}
void	KpxSifStopRxDma0(void) { LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 0 ); }
void	KpxSifStopTxDma0(void) { LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 1 ); }
void	KpxSifStopRxDma1(void) { do{ /*dummy*/; }while(0); }
void	KpxSifStopTxDma1(void) { do{ /*dummy*/; }while(0); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_RSTRXDMA( ch ) {                             \
tUART_REG *ureg = (ch == 0) ? CPU_UART_REG0 : CPU_UART_REG1; \
		ureg->FDR = (1<<4);                                  \
}
#define	LPX_SIF_SETX_RxDMA( base, ch, sa, da, cnt ) { \
tDMA_REG *ureg;                                       \
		ureg = (base);                                \
		ureg = AXIDMA_CH ##ch## _REGBASE;             \
		DMA_REG_WR( N0SA0, sa );                      \
		DMA_REG_WR( N0DA0, da );                      \
		DMA_REG_WR( N0TB0, cnt );                     \
}
void	KpxSifSetRxDma0(UINT SAdr, UINT DAdr, LONG Cnt) {
		LPX_SIF_SETX_RxDMA( AXIDMA_UNIT0_REGBASE, 0, SAdr, DAdr, Cnt ); 
}
void	KpxSifSetRxDma1(UINT SAdr, UINT DAdr, LONG Cnt) { do{ /*dummy*/; }while(0); }
#define	LPX_SIF_SETX_TxDMA( base, ch, sa, da, cnt ) { \
tDMA_REG *ureg;                                       \
		ureg = (base);                                \
		ureg = AXIDMA_CH ##ch## _REGBASE;             \
		DMA_REG_WR( N0SA0, sa );                      \
		DMA_REG_WR( N0DA0, da );                      \
		DMA_REG_WR( N0TB0, (cnt) );                   \
}
void	KpxSifSetTxDma0(UINT SAdr, UINT DAdr, LONG Cnt) { 
		LPX_SIF_SETX_TxDMA( AXIDMA_UNIT0_REGBASE, 1, SAdr, DAdr, Cnt ); 
}
void	KpxSifSetTxDma1(UINT SAdr, UINT DAdr, LONG Cnt) { do{ /*dummy*/; }while(0); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_START_DMA( base, ch ) {                        \
tDMA_REG *ureg;                                            \
ULONG wk;                                                  \
		ureg = (base);                                     \
		ureg = AXIDMA_CH ##ch## _REGBASE;                  \
		__nop(); \
		__nop(); \
		DMA_REG_WR( CHCTRL, SWRST );                       \
		__nop(); \
		__nop(); \
		do {                                               \
			DMA_REG_WR( CHCTRL, SETEN );                   \
			DMA_REG_RD( CHSTAT, wk );                      \
		} while ( (wk & EN) ^ (EN) /*(wk & (TACT | EN ))^(TACT | EN) */ );       \
}
#define LPX_START_TXDMA( base, ch ) {                      \
tDMA_REG *ureg;                                            \
ULONG wk;                                                  \
		ureg = (base);                                     \
		ureg = AXIDMA_CH ##ch## _REGBASE;                  \
		DMA_REG_WR( CHCTRL, CLRRQ | CLREND | CLRTC );  /* @@ */ \
		__nop(); \
		__nop(); \
		DMA_REG_WR( CHCTRL, SWRST );                       \
		__nop(); \
		__nop(); \
		do {                                               \
			DMA_REG_WR( CHCTRL, SETEN );                   \
			DMA_REG_RD( CHSTAT, wk );                      \
		} while ( (wk & (TACT | EN ))^(TACT | EN) );       \
}
void	KpxSifStartRxDma0(void) { LPX_START_DMA( AXIDMA_UNIT0_REGBASE, 0 ); }
void	KpxSifStartTxDma0(void) { LPX_START_TXDMA( AXIDMA_UNIT0_REGBASE, 1 ); /*LPX_START_DMA( AXIDMA_UNIT0_REGBASE, 1 );*/ }
void	KpxSifStartRxDma1(void) { do{ /* dummy */; } while(0); }
void	KpxSifStartTxDma1(void) { do{ /* dummy */; } while(0); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_CHK_DMA_TC( base, ch ) {      \
tDMA_REG *ureg;                               \
UINT	data;                                 \
		ureg = (base);                        \
		ureg = AXIDMA_CH ##ch## _REGBASE;     \
		DMA_REG_RD( CHSTAT, data );           \
		return ( data & TC );                 \
}
LONG	KpxSifChkRxDma0Tc(void) { LPX_SIF_CHK_DMA_TC( AXIDMA_UNIT0_REGBASE, 0 ); }
LONG	KpxSifChkTxDma0Tc(void) { LPX_SIF_CHK_DMA_TC( AXIDMA_UNIT0_REGBASE, 1 ); }
LONG	KpxSifChkRxDma1Tc(void) { do{ /* dummy */ return (0); } while(0); }
LONG	KpxSifChkTxDma1Tc(void) { do{ /* dummy */ return (0); } while(0); }
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Initialize DMA																				*/
/*																									*/
/****************************************************************************************************/
void	KpxInitCpuDMA( void )
{
/*--------------------------------------------------------------------------------------------------*/
#define LPX_FIX_PR( base )   {            /* Fix DMA Priority */ \
tDMA_REG *ureg;					          /* DMA register base addr */ \
		ureg = (base);  \
		DMA_REG_WR( DCTRL, 0 );  \
}
#define LPX_SET_UARTRXCFG( base, ch ) {   /* Set UART RX DMA Configuration */    \
tDMA_REG *ureg;				              /* DMA register base addr */           \
UINT	data;				              /* working data */                     \
		ureg = (base);                    /* set UNIT0 register base */          \
		ureg = AXIDMA_CH ##ch## _REGBASE; /* set CHANNEL0 register base */       \
		data =  SAD |  				      /* Fixed Source Address */             \
				((0x01)<<8) |		      /* Level Mode (check) */               \
				((0x06)<<4) | 		      /* Hi Level (check) */                 \
				0x05 ; 				      /* UART1 Rx */                         \
		DMA_REG_WR( CHCFG, data );	      /* set UNIT0/Ch0 CHCFG */              \
		/*DMA_REG_WR( CHITVL, 0x64 );*/	      /* set CHANNEL Interval (check) */ \
}

#define LPX_SET_UARTTXCFG( base, ch ) {   /* Set UART RX DMA Configuration */ \
tDMA_REG *ureg;				              /* DMA register base addr */        \
UINT	data;				              /* working data */                  \
		ureg = (base);			          /* set UNIT0 register base */       \
		ureg = AXIDMA_CH ##ch## _REGBASE; /* set CHANNEL1 register base */    \
		data =  DAD |  					  /* Fixed Destination Address */     \
				((0x01)<<8) |			  /* Level Mode (check) */            \
				((0x06)<<4) | 			  /* Hi Level (check) */              \
				0x04 ; 					  /* UART1 Tx */                      \
		DMA_REG_WR( CHCFG, data );		  /* set UNIT0/Ch1 CHCFG */           \
		/* DMA_REG_WR( CHITVL, 0x64 ); */ /* set CHANNEL Interval (check) */  \
}
/*--------------------------------------------------------------------------------------------------*/
/*		全てのチャネルを動作禁止																	*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 0 );   /* Stop AXI DMA Unit0/CH0 */
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 1 );   /* Stop AXI DMA Unit0/CH1 */
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 2 );   /* Stop AXI DMA Unit0/CH2 */
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 3 );   /* Stop AXI DMA Unit0/CH3 */
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		各チャネルの優先順位を固定(	CH0->CH1->CH2->CH3 )											*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_FIX_PR( AXIDMA_UNIT0_REGBASE );        /* AXI DMA Unit0 */
/*--------------------------------------------------------------------------------------------------*/
/*		ハードウェア転送要求( Set Channel Configuration Register )									*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_SET_UARTRXCFG( AXIDMA_UNIT0_REGBASE, 0 ); /* AXI DMA Unit0 Channel0 */
		LPX_SET_UARTTXCFG( AXIDMA_UNIT0_REGBASE, 1 ); /* AXI DMA Unit0 Channel1 */
/*--------------------------------------------------------------------------------------------------*/
		/*
		 * 後は、シリアル通信インターフェイス(Uart.c)で初期化
		 */
/*--------------------------------------------------------------------------------------------------*/
/* 		DMA動作許可(UNIT0/CH0,CH1)																	*/
/*--------------------------------------------------------------------------------------------------*/
//		LPX_START_DMA( AXIDMA_UNIT0_REGBASE, 0 ); /* Start DMA Unit0/Channel0 */
//		LPX_START_DMA( AXIDMA_UNIT0_REGBASE, 1 ); /* Start DMA Unit0/Channel1 */
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		Stop DMA 																					*/
/*																									*/
/****************************************************************************************************/
void	KpxStopCpuDMA( void )
{
/*--------------------------------------------------------------------------------------------------*/
/*		全てのチャネルを動作禁止																	*/
/*--------------------------------------------------------------------------------------------------*/
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 0 );   /* Stop AXI DMA Unit0/CH0 */
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 1 );   /* Stop AXI DMA Unit0/CH1 */
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 2 );   /* Stop AXI DMA Unit0/CH2 */
		LPX_STOP_DMA( AXIDMA_UNIT0_REGBASE, 3 );   /* Stop AXI DMA Unit0/CH3 */
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/*--------------------------------------------------------------------------------------------------*/
