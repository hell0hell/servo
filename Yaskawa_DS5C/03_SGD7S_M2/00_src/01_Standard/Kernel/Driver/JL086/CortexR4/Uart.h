/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Uart.h : Uart Interface Definition File														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : JL086 Uart Common Definitions.															*/
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
#ifndef			___TST_UART___
#define			___TST_UART___
#include 		"Sif.h"							/* Serial Interface									*/

/*--------------------------------------------------------------------------------------------------*/
/** UART Register Mapping 																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	VUINT32 RBRTHR;								/**< R:Recieve Buffer Register / W:Transmitt Holding Register */
	VUINT32 IER;								/**< Interrupt Enable Register */
	VUINT32 IIRFCR;								/**< R:Interrupt Indicate Register / W:FIFO Control Register */
	VUINT32 LCR;								/**< Line Control Register */
	VUINT32 MCR;								/**< Modem Control Register */
	VUINT32 LSR;								/**< Line Status Register */
	VUINT32 MSR;								/**< Modem Status Register */
	VUINT32 SCR;								/**< SCratch pad Register */
	VUINT32 _rsv020H[(0x07C-0x020)/4];			/**< (reserved area) */
	VUINT32 USR;								/**< Uart Status Register */
	VUINT32 _rsv080H[(0x100-0x080)/4];			/**< (reserved area) */
	VUINT32 FDR;								/**< Fifo Dma control Register */
	VUINT32 _rsv104H[(0x200-0x104)/4];			/**< (reserved area) */
} tUART_REG;

/*--------------------------------------------------------------------------------------------------*/
/*  	Register Base Address Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define CPU_UART_REG0 ((tUART_REG *)(CONFIG_UART_REGBASE + CONFIG_UART_REGOFFSET*0))
#define CPU_UART_REG1 ((tUART_REG *)(CONFIG_UART_REGBASE + CONFIG_UART_REGOFFSET*1))

/****************************************************************************************************/
/*																									*/
/*		シリアル通信ＩＦ定義																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define CPU_SIF_N_CH 		(1)				/* Channel Number */
/*--------------------------------------------------------------------------------------------------*/
#ifndef _VC_DEBUG_
#define UART0_PORT_USE		(1)				/* UART0: use */
#define UART1_PORT_USE		(0)				/* UART1: not use */
#else
#define UART0_PORT_USE		(0)				/* UART0: not use */
#define UART1_PORT_USE		(0)				/* UART1: not use */
#endif
/*--------------------------------------------------------------------------------------------------*/
#define	COM0_UART			(0)				/* UART Channel 0 */
#define	COM1_UART			(1)				/* UART Channel 1 */
/*--------------------------------------------------------------------------------------------------*/
#define SIF_N_CH			(CPU_SIF_N_CH)	/* Serial IF channel number								*/
/*--------------------------------------------------------------------------------------------------*/
#define	REQSTX_STARTTX		0x01			/* 応答送信開始指令										*/
#define	REQSTX_NORESPONSE	0x02			/* 無応答指令(MsgErr)									*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		ＩＦ関数・マクロ定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_CHK_OPMSG( fc )	((0x60 <= fc) && (fc <= 0x64))		/* Check Operator Message			*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_INIT_UART( ch, AxisAddr ) {           \
		KpxInitCpuSerialIF( ch, 19200, 8, 2, 1 ); \
		KpxInitSifMsgManager( COM ## ch ##_UART,  \
						      SIFCOM ## ch,       \
						      AxisAddr );         \
        }

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Port Definition																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _SifComPort {
/*--------------------------------------------------------------------------------------------------*/
	void	(*CREATE)(struct _SifComPort *this, 			/* Create Serial I/F                    */
					int portno,								/* Port No								*/
					int use_dma);							/* 1: use dma  0: not use dma           */
/*--------------------------------------------------------------------------------------------------*/
	volatile UINT *RXCHKF;									/* Serial Receive Check Flag Reg.		*/
	volatile UCHAR *RXBUF;									/* Serial Receive Data Buffer			*/
//	volatile UCHAR *RXSTS;									/* Serial Receive Status Reg.			*/
	void	(*RXENABLE)(void);								/* Serial Receive Enable				*/
	void	(*RXDISABLE)(void);								/* Serial Receive Disable				*/
	void	(*RSTRXSTS)(void);								/* Serial Reset Rx Status				*/
/*--------------------------------------------------------------------------------------------------*/
	volatile UINT *TXCHKF;									/* Serial Transmit Check Flag Reg.		*/
	volatile UCHAR *TXBUF;									/* Serial Transmit Data Buffer			*/
	//volatile UINT *TXSTS;									/* Serial Transmit Status Reg.			*/
	void	(*TXENABLE)(void);								/* Serial Transmit Enable				*/
	void	(*TXDISABLE)(void);								/* Serial Transmit Disable				*/
	//void	(*RSTTXSTS)(void);								/* Serial Reset Tx Status				*/
/*--------------------------------------------------------------------------------------------------*/
//	volatile USHORT *RxDMA_CNT;								/* RxDMA Count Reg.						*/
//	volatile USHORT *TxDMA_CNT;								/* TxDMA Count Reg.						*/
	volatile UINT *RxDMA_CNT;								/* RxDMA Count Reg.						*/
	volatile UINT *TxDMA_CNT;								/* TxDMA Count Reg.						*/
	void	(*STOP_RxDMA)(void);							/* Stop Sif RxDMA						*/
	void	(*STOP_TxDMA)(void);							/* Stop Sif TxDMA						*/
	void	(*SETX_RxDMA)(UINT SAdr, UINT DAdr, LONG Cnt);	/* Set addr and count of RxDMA			*/
	void	(*SETX_TxDMA)(UINT SAdr, UINT DAdr, LONG Cnt);	/* Set addr and count of TxDMA			*/
	void	(*START_RxDMA)(void);							/* Start Sif RxDMA						*/
	void	(*START_TxDMA)(void);							/* Start Sif TxDMA						*/
	LONG	(*CHK_RxDMA_TC)(void);							/* Check RxDMA Completion				*/
	LONG	(*CHK_TxDMA_TC)(void);							/* Check TxDMA Completion				*/
/*--------------------------------------------------------------------------------------------------*/
	void	(*SWTRG_TxDMA)(void);							/* Start Tx( software trigger ) 		*/
	LONG	(*CHK_TIMEOUT)(void);							/* Check Rx Charactor Timeout 	 		*/
	LONG	(*CHK_RXRDY)(void);								/* Check Rx Detect Trigger Level 		*/
	LONG	(*CHK_TXEMPTY)(void);							/* Check Tx Register Empty 				*/
/*--------------------------------------------------------------------------------------------------*/
//	volatile USHORT *RxDMA_STS;								/* *RxDMA Status Reg.                   */
//	volatile USHORT *TxDMA_STS;								/* *RxDMA Status Reg.                   */
//	USHORT	RxErrCnt;                                       /* *Receive Error Count                 */
//	USHORT  RxErrSts;                                       /* *Receive Error Status                */
//	USHORT  RxErrData;                                      /* *Receive Data on Error State         */
//	USHORT  Reserve;                                        /* *Reserved                            */
/*--------------------------------------------------------------------------------------------------*/
	LONG 	(*CHK_RXERR)(void);								/* *Check Serial Rx Error               */
	void 	(*RESET)(void);									/* *Reset Serial Interface              */
/*--------------------------------------------------------------------------------------------------*/
	LONG	BRT;											/* *Current Serial I/F baud rate        */
	void	(*SETBRT)(struct _SifComPort *this, LONG brt);	/* *Set Serial I/F baud rate            */
/*--------------------------------------------------------------------------------------------------*/
} CSIFCOMPORT;

/*--------------------------------------------------------------------------------------------------*/
/*  	Uart.c : MICROSSP Cpu Serial I/F Driver														*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxInitCpuSerialIF(							/* Initialize Serial I/F						*/
		ULONG	ch,									/* IF Channel : 0, 1, 2							*/
		ULONG	bps,								/* Baud Rate  : xxxxx [bit/sec]					*/
		ULONG	DataLen,							/* DataLength : 5/6/7/8/9						*/
		ULONG	Parity,								/* Parity : None(0)/Odd(1)/Even(2)				*/
		ULONG	StopBit);							/* StopBit : 1/2/3/4							*/

/*--------------------------------------------------------------------------------------------------*/
/*  	SifMbus.c : Serial Message Handler 															*/
/*--------------------------------------------------------------------------------------------------*/
void	KpxSifMsgManager( void );					/* Serial Message Handler Main Function			*/
void	KpxInitSifMsgManager( 						/* Serial Message Handler Initialize			*/
				LONG ch,							/* UART channel number (0,1,2)					*/
				CSIFCOMPORT *Com,					/* UART Com port 								*/
				LONG Axis);							/* Axis Number									*/
//<S150>				void *pUartMbusServer);				/* Zantei USB同様後で消す						*/

/*--------------------------------------------------------------------------------------------------*/
/*		変数定義																					*/
/*--------------------------------------------------------------------------------------------------*/
extern CSIFCOMPORT *SIFCOM0;						/* UART Port0									*/
extern CSIFCOMPORT *SIFCOM1;						/* UART Port1									*/
/*--------------------------------------------------------------------------------------------------*/
extern	CSIF		LSif[SIF_N_CH];					/* Serial Interface Instance					*/

#endif
