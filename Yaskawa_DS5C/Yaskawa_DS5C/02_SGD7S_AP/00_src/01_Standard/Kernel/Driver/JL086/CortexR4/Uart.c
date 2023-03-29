/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Uart.c : CPU Uart Interface Driver 															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : JL086 Uart Interface																	*/
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
/*		Rev.1.00 : 2013.07.19  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include	"config.h"
#include	"Basedef.h"
#include	"MLib.h"
#include	"Uart.h"
#include	"Dma.h"
#include 	"FirmDownload.h"		/* <S041> */

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Communication Variable																*/
/*--------------------------------------------------------------------------------------------------*/
CSIFCOMPORT SifComPorts[CPU_SIF_N_CH];			/* Serial Com Port Register Set						*/
CSIFCOMPORT *SIFCOM0;							/* Serial Com Port Pointer(0)						*/
CSIFCOMPORT *SIFCOM1;							/* Serial Com Port Pointer(1)						*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Macro Definitions																			*/ 
/*																									*/
/****************************************************************************************************/
#define CPU_SIF_RXCHKF0 (CPU_UART_REG0->LSR)
#define CPU_SIF_RXCHKF1 (CPU_UART_REG1->LSR)
#define CPU_SIF_RXBUF0 (VUCHAR *)&(CPU_UART_REG0->RBRTHR)
#define CPU_SIF_RXBUF1 (VUCHAR *)&(CPU_UART_REG1->RBRTHR)
#define CPU_SIF_TXCHKF0 (CPU_UART_REG0->LSR)
#define CPU_SIF_TXCHKF1 (CPU_UART_REG1->LSR)
#define CPU_SIF_TXBUF0 (VUCHAR *)&(CPU_UART_REG0->RBRTHR)
#define CPU_SIF_TXBUF1 (VUCHAR *)&(CPU_UART_REG1->RBRTHR)

/*--------------------------------------------------------------------------------------------------*/
#define UART_REG_WR(reg, data) ((VUCHAR)((ureg)->##reg) = (UCHAR)(data))
#define UART_REG_RD(reg, data) ((data) = (VUCHAR)((ureg)->##reg))

/****************************************************************************************************/
/*																									*/
/*		シリアル通信ポート用プロシージャ															*/ 
/*																									*/
/****************************************************************************************************/
#define LPX_SIF_RSTRXDMA( ch ) {                             \
tUART_REG *ureg = (ch == 0) ? CPU_UART_REG0 : CPU_UART_REG1; \
		ureg->FDR = (1<<4);                                  \
}
#define	LPX_SIF_RXENABLE( ch )  {                 \
		tUART_REG *ureg;                          \
		ULONG wk;                                 \
		ureg = CPU_UART_REG##ch;                  \
		UART_REG_WR(FDR, (0x10));                 \
		UART_REG_RD(IER, wk);                     \
		UART_REG_WR(IER, (wk | 0x5) );            \
}
void	KpxEnableRx0(void) { LPX_SIF_RXENABLE( 0 ); }
void	KpxEnableRx1(void) { LPX_SIF_RXENABLE( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_RXDISABLE( ch ) {      \
		tUART_REG *ureg;               \
		ULONG wk;                      \
		ureg = CPU_UART_REG##ch;       \
		UART_REG_RD(IER, wk);          \
		UART_REG_WR(IER, wk & ~(0x5)); \
}
void	KpxDisableRx0(void) { LPX_SIF_RXDISABLE( 0 ); }
void	KpxDisableRx1(void) { LPX_SIF_RXDISABLE( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_RSTRXSTS( ch ) {       \
		tUART_REG *ureg;               \
		ULONG wk;                      \
		ureg = CPU_UART_REG##ch;       \
		UART_REG_RD(LSR, wk);          \
}
void	KpxRstRxSts0(void) { LPX_SIF_RSTRXSTS( 0 ); }
void	KpxRstRxSts1(void) { LPX_SIF_RSTRXSTS( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define CFG_TXEN_PORTADR   (VUINT *)(0xEFFF8010)		/* 実際は、GPIO4 */
#define CFG_SIF0_TXE( x )  { /* Set UART Tx Line Enable or Disable */ \
		if( x == 1 ) { *CFG_TXEN_PORTADR |= (1<<2); }                 \
		if( x == 0 ) { *CFG_TXEN_PORTADR &= ~(1<<2); }                \
}
#define CFG_SIF1_TXE( x )  {;}		/* Set UART Tx Line Enable or Disable */
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_TXENABLE( ch ) {     \
		tUART_REG *ureg;             \
		ULONG wk;                    \
		ureg = CPU_UART_REG##ch;     \
		UART_REG_WR(FDR, (1<<6));    /* reset DMA */ \
		UART_REG_RD(IER, wk);        \
		UART_REG_WR(IER, wk | 0x2);  \
}
void	KpxTxEnable0(void) { CFG_SIF0_TXE( 1 ); LPX_SIF_TXENABLE( 0 ); }
void	KpxTxEnable1(void) { CFG_SIF1_TXE( 1 ); LPX_SIF_TXENABLE( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_TXDISABLE( ch ) {      \
		tUART_REG *ureg;               \
		ULONG wk;                      \
		ureg = CPU_UART_REG##ch;       \
		UART_REG_RD(IER, wk);          \
		UART_REG_WR(IER, wk & ~(0x2)); \
}
void	KpxTxDisable0(void) { LPX_SIF_TXDISABLE( 0 ); CFG_SIF0_TXE( 0 ); }
void	KpxTxDisable1(void) { LPX_SIF_TXDISABLE( 1 ); CFG_SIF0_TXE( 0 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_CHK_RXERROR( ch ) {   \
		tUART_REG *ureg;              \
		ULONG wk;                     \
		ureg = CPU_UART_REG##ch;      \
		UART_REG_RD(IIRFCR, wk);      \
		return ( (wk&0x0F) == 0x06 ); \
}
LONG	KpxChkRxError0(void) { LPX_SIF_CHK_RXERROR( 0 );  }
LONG	KpxChkRxError1(void) { LPX_SIF_CHK_RXERROR( 1 );  }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_RESET( ch ) do{;}while(0)  /* TODO: */
void	KpxResetSerial0(void)  { LPX_SIF_RESET( 0 ); }
void	KpxResetSerial1(void)  { LPX_SIF_RESET( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_SETBRT( ch ) do{;}while(0) /* TODO: */
void	KpxSetBrt0(CSIFCOMPORT *this, LONG bps) { LPX_SIF_SETBRT( 0 ); }
void	KpxSetBrt1(CSIFCOMPORT *this, LONG bps) { LPX_SIF_SETBRT( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_STXSWTRG( ch )  {    \
		tUART_REG *ureg;         \
		ureg = CPU_UART_REG##ch; \
		UART_REG_WR(FDR, (1<<6));\
}
void	KpxStxSwTrg0(void){ LPX_STXSWTRG( 0 );}
void	KpxStxSwTrg1(void){ LPX_STXSWTRG( 1 );}
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_CHK_RXCTO( ch ) {   \
		tUART_REG *ureg;            \
		ULONG wk;                   \
		ureg = CPU_UART_REG##ch;    \
		UART_REG_RD( IIRFCR, wk );  \
		return (( wk & 0x0F ) == 0x0C); \
}
LONG	KpxSifChkChrRxTimeout0(void){ LPX_SIF_CHK_RXCTO( 0 ); }
LONG	KpxSifChkChrRxTimeout1(void){ LPX_SIF_CHK_RXCTO( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_CHK_RXRDY( ch ) {   \
		tUART_REG *ureg;            \
		ULONG wk;                   \
		ureg = CPU_UART_REG##ch;    \
		UART_REG_RD( IIRFCR, wk );  \
		return (( wk & 0x0F ) == 0x04); \
}
LONG	KpxSifChkRxReady0(void){ LPX_SIF_CHK_RXRDY( 0 ); }
LONG	KpxSifChkRxReady1(void){ LPX_SIF_CHK_RXRDY( 1 ); }
/*--------------------------------------------------------------------------------------------------*/
#define LPX_SIF_CHK_TXEMPTY( ch ) {   \
		tUART_REG *ureg;            \
		ULONG wk;                   \
		ureg = CPU_UART_REG##ch;    \
		UART_REG_RD( IIRFCR, wk );  \
		return (( wk & 0x0F ) == 0x02); \
}
LONG	KpxSifChkTxEmpty0(void){ LPX_SIF_CHK_TXEMPTY( 0 ); }
LONG	KpxSifChkTxEmpty1(void){ LPX_SIF_CHK_TXEMPTY( 1 ); }
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		シリアルI/Fの初期化																			*/
/*																									*/
/****************************************************************************************************/
void	KpxInitCpuSerialIF(				/* シリアルI/Fの初期化										*/
		ULONG	ch,						/* IF Channel : 0, 1, 2										*/
		ULONG	bps,					/* Baud Rate  : xxxxx [bit/sec]								*/
		ULONG	DataLen,				/* DataLength : 5/6/7/8/9									*/
		ULONG	Parity,					/* Parity : None(0)/Odd(1)/Even(2)							*/
		ULONG	StopBit)				/* StopBit : 1/2/3/4										*/
{
tUART_REG *ureg;  						/* control register address */
ULONG	wk;
USHORT	divisor;
UCHAR	modex1 = 0;
CSIFCOMPORT *com;
/*--------------------------------------------------------------------------------------------------*/
/* 		set register base address 																	*/
/*--------------------------------------------------------------------------------------------------*/
		ureg = (tUART_REG*)((ULONG)CONFIG_UART_REGBASE	+ ((ULONG)CONFIG_UART_REGOFFSET * ch));	
/*--------------------------------------------------------------------------------------------------*/
/*		Disable FIFO 																				*/
/*--------------------------------------------------------------------------------------------------*/
		UART_REG_WR( IIRFCR, 0 );								
/*--------------------------------------------------------------------------------------------------*/
/*		Set LCR to divisor latch mode																*/
/*--------------------------------------------------------------------------------------------------*/
		do {
			UART_REG_WR(LCR, 0x80 );
			UART_REG_RD(LCR, wk);
		} while ( (wk & 0x80) != 0x80 );
/*--------------------------------------------------------------------------------------------------*/
/*		Set Value Cal.																				*/
/*--------------------------------------------------------------------------------------------------*/
/*		divisor = fx/(bps)/16			fx : APBCLK/4												*/
/*--------------------------------------------------------------------------------------------------*/
		switch( bps )/* 切替転送速度 <S041> */
		{
			case 9600:		/* 9600 bps */
				divisor = 0x66;		/* 9.6K bpsへ切替 */
				break;
			case 14400:		/* 14400 bps */
				divisor = 0x44;		/* 14.4K bpsへ切替 */
				break;
			case 19200:		/* 19200K bps */
				divisor = 0x33;		/* 19.2K bpsへ切替 */
				break;
			case 38400:		/* 38400 bps */
				divisor = 0x19;		/* 38.4K bpsへ切替 */
				break;
			case 57600:		/* 57600 bps	*/
				divisor = 0x11;		/* 57.6K bpsへ切替 */
				break;
			case 115200:	/* 115200 bps		*/
				divisor = 0x08;		/* 115.2K bpsへ切替 */
				break;
			default:		/* 上記以外の転送速度時は計算する　*/
				divisor = (CONFIG_CLKFREQ_UART*1000)/(bps)/16; /* divisor = (62500/4)*10^3/(19200)/16 = 50.86 */
				break;
		}
		UART_REG_WR(RBRTHR, (divisor >> 0) & 0xFF );
		UART_REG_WR(IER, (divisor >> 8) & 0xFF );
		SysDl.NowBps = bps;	/* 現在のＢＰＳ値の保存 <S041> */
/*--------------------------------------------------------------------------------------------------*/
/*		Cal. SetValue for Mode Register																*/
/*--------------------------------------------------------------------------------------------------*/
		if( StopBit == 1 ){ ; }									/* Stop bit(1)  SBL=0/ESBL=0		*/
		if( StopBit == 2 ){ modex1 |= 0x04; }					/* Stop bit(2)  SBL=1/ESBL=0		*/
		else { ; }												/* Not Supported 					*/
/*--------------------------------------------------------------------------------------------------*/
		if( Parity == 0 ){ ; }									/* Parity None(0) PEN=0/P=0 		*/
		if( Parity == 1 ){ modex1 |= 0x08; }					/* Parity Odd(1) PEN=1/P=1			*/
		if( Parity == 2 ){ modex1 |= 0x18; }					/* Parity Even(2) PEN=1/P=0			*/
/*--------------------------------------------------------------------------------------------------*/
		if( DataLen == 5 ){ modex1 |= 0x00; }					/* DataLen (5) L=000				*/
		if( DataLen == 6 ){ modex1 |= 0x01; }					/* DataLen (6) L=001				*/
		if( DataLen == 7 ){ modex1 |= 0x02; }					/* DataLen (7) L=010				*/
		if( DataLen == 8 ){ modex1 |= 0x03; }					/* DataLen (8) L=011				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Set LCR to normal mode																		*/
/*--------------------------------------------------------------------------------------------------*/
		do {
			UART_REG_WR(LCR, modex1);
			UART_REG_RD(LCR, wk);
		} while ( (wk) != modex1 );

/*--------------------------------------------------------------------------------------------------*/
/*		Disable Interrupt 																			*/
/*--------------------------------------------------------------------------------------------------*/
		UART_REG_WR( IER, 0 );

/*--------------------------------------------------------------------------------------------------*/
/*		Enable FIFO	 																				*/
/*--------------------------------------------------------------------------------------------------*/
//		UART_REG_WR( IIRFCR,  0xC7 );							/* Reset & Enable FIFO */
		UART_REG_WR( IIRFCR,  0x47 );							/* Reset & Enable FIFO */

/*--------------------------------------------------------------------------------------------------*/
/*		Create Com Port 																			*/
/*--------------------------------------------------------------------------------------------------*/
		com = &(SifComPorts[ch]);
		MlibResetLongMemory( com, sizeof( CSIFCOMPORT )/4 );	/* Reset All Data to Zero			*/
		SIFCOM0 = ((ch == 0) ? com : SIFCOM0);
		SIFCOM1 = ((ch == 1) ? com : SIFCOM1);

/*--------------------------------------------------------------------------------------------------*/
		switch( ch )
		{
/*--------------------------------------------------------------------------------------------------*/
		case 0:													/* UART Ch:0						*/
/*--------------------------------------------------------------------------------------------------*/
			com->RXCHKF = &(CPU_SIF_RXCHKF0);                   /* bit3-0=0100B:received data */
			com->RXBUF = CPU_SIF_RXBUF0;
//			com->RXSTS = &(CPU_SIF_RXSTS0); TODO:
			com->RXENABLE = KpxEnableRx0;
			com->RXDISABLE = KpxDisableRx0;
			com->RSTRXSTS = KpxRstRxSts0;
			com->TXCHKF = &(CPU_SIF_TXCHKF0);
			com->TXBUF = CPU_SIF_TXBUF0;
			com->TXENABLE = KpxTxEnable0;
			com->TXDISABLE = KpxTxDisable0;
			com->CHK_RXERR = KpxChkRxError0;
			com->RESET = KpxResetSerial0;
//			com->BRT = 19200;                       /* Initial baud rate */
			com->BRT = bps;                       	/* Initial baud rate *//*<S041>*/
//			com->BRT = 38400;                       /* Initial baud rate */
			com->SETBRT = KpxSetBrt0; //TODO:
			com->SWTRG_TxDMA = KpxStxSwTrg0;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case 1:													/* UART Ch:1						*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
			com->RXCHKF = &(CPU_SIF_RXCHKF1);
			com->RXBUF = CPU_SIF_RXBUF1;
//			com->RXSTS = &(CPU_SIF_RXSTS1); TODO:
			com->RXENABLE = KpxEnableRx1;
			com->RXDISABLE = KpxDisableRx1;
			com->RSTRXSTS = KpxRstRxSts1;
			com->TXCHKF = &(CPU_SIF_TXCHKF1);
			com->TXBUF = CPU_SIF_TXBUF1;
			com->TXENABLE = KpxTxEnable1;
			com->TXDISABLE = KpxTxDisable1;
			com->CHK_RXERR = KpxChkRxError1;
			com->RESET = KpxResetSerial1;
			com->BRT = 19200;                       /* Initial baud rate */
			com->SETBRT = KpxSetBrt1; //TODO:
			com->SWTRG_TxDMA = KpxStxSwTrg1;
#endif
			break;
/*--------------------------------------------------------------------------------------------------*/
		default:
			break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Initialize DMA																				*/
/*--------------------------------------------------------------------------------------------------*/
#define SET_DMA_CNTREG( base, ch, dst ) { \
tDMA_REG *ureg;                           \
		ureg = (base);                    \
		ureg = AXIDMA_CH ##ch## _REGBASE; \
        dst = &(ureg->CRTB);              \
}
		if( 1 )
		{
			switch( ch )
			{
		/*------------------------------------------------------------------------------------------*/
			case 0:												/* UART Ch:0						*/
		/*------------------------------------------------------------------------------------------*/
				SET_DMA_CNTREG( AXIDMA_UNIT0_REGBASE, 0, com->RxDMA_CNT );
				SET_DMA_CNTREG( AXIDMA_UNIT0_REGBASE, 1, com->TxDMA_CNT );
				com->STOP_RxDMA = KpxSifStopRxDma0;
				com->STOP_TxDMA = KpxSifStopTxDma0;
				com->SETX_RxDMA = KpxSifSetRxDma0;
				com->SETX_TxDMA = KpxSifSetTxDma0;
				com->START_RxDMA = KpxSifStartRxDma0;
				com->START_TxDMA = KpxSifStartTxDma0;
				com->CHK_RxDMA_TC = KpxSifChkRxDma0Tc;
				com->CHK_TxDMA_TC = KpxSifChkTxDma0Tc;
				com->CHK_TIMEOUT = KpxSifChkChrRxTimeout0;
				com->CHK_RXRDY = KpxSifChkRxReady0;
				com->CHK_TXEMPTY = KpxSifChkTxEmpty0;
				break;
		/*------------------------------------------------------------------------------------------*/
			case 1:												/* UART Ch:1						*/
		/*------------------------------------------------------------------------------------------*/
#if 0
				com->RxDMA_CNT = NULL; /* dummy */
				com->TxDMA_CNT = NULL; /* dummy */
				com->STOP_RxDMA = KpxSifStopRxDma1;
				com->STOP_TxDMA = KpxSifStopTxDma1;
				com->SETX_RxDMA = KpxSifSetRxDma1;
				com->SETX_TxDMA = KpxSifSetTxDma1;
				com->START_RxDMA = KpxSifStartRxDma1;
				com->START_TxDMA = KpxSifStartTxDma1;
				com->CHK_RxDMA_TC = KpxSifChkRxDma1Tc;
				com->CHK_TxDMA_TC = KpxSifChkTxDma1Tc;
				com->CHK_TIMEOUT = KpxSifChkChrRxTimeout1;
				com->CHK_RXRDY = KpxSifChkRxReady1;
				com->CHK_TXEMPTY = KpxSifChkTxEmpty1;
#endif
				break;
		/*------------------------------------------------------------------------------------------*/
			default:
				break;
			}
		}
/*--------------------------------------------------------------------------------------------------*/
		else
		{
//			LPX_SIF_STOP_DMA( 4 );	/* stop DMA channel 4 */
//			LPX_SIF_STOP_DMA( 5 );	/* stop DMA channel 5 */
//			LPX_SIF_STOP_DMA( 6 );	/* stop DMA channel 6 */
//			LPX_SIF_STOP_DMA( 7 );	/* stop DMA channel 7 */
		}
/*--------------------------------------------------------------------------------------------------*/

		return;
}

