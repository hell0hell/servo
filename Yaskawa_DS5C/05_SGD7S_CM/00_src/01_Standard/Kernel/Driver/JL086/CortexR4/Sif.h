/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Sif.h : Serial Interface Definition File													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	機 能 : Serial Interface Common Definitions.													*/
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
/*		Rev.1.00 : 2013.04.11  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef			___TST_SIF___
#define			___TST_SIF___

/****************************************************************************************************/
/*																									*/
/*		Serial Communication Interface Definition													*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#define SIF_TX_BUFSIZE		(512)			/* Serial IF send buffer size							*//* @@CHECK */
#define SIF_RX_BUFSIZE		(512) 			/* Serial IF receive buffer size						*//* @@CHECK */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Serial Communication Class																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _CSIF{
/*--------------------------------------------------------------------------------------------------*/
	void	*Com;							/* Communication port									*/
//<S150>	void	*ComMbus42;						/* UART 42H解析用 @@USB同様後で消す						*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	AxisNumber;						/* axis number											*/
	UCHAR	SqStep;							/* sequence step										*/
	UCHAR	ErrCnt;							/* serial I/F error counter								*/
	UCHAR	ErrCod;							/* serial I/F error code								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RxCmdLen;						/* command message length [byte]						*/
	USHORT	RxCmdCnt;						/* receive message count								*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RxDmaScnt;						/* RxDMA start count									*/
	USHORT	RxDmaRcnt;						/* RxDMA saved count									*/
/*--------------------------------------------------------------------------------------------------*/
	ULONG	RxTimer;						/* Receive timer										*/
	ULONG	StxTimer;						/* Transmit timer										*/
/*--------------------------------------------------------------------------------------------------*/
	volatile USHORT	ReqStx;					/* staart Tx request									*/
	volatile USHORT	TxResLen;				/* response message length [byte]						*/
	volatile USHORT	TxResCnt;				/* response message count								*/
	UCHAR	* volatile pTxRes;				/* response buffer pointer								*/
/*--------------------------------------------------------------------------------------------------*/
	void	(*DoRxTx)(struct _CSIF *this);	/* Exchange Serial Data									*/
	LONG	(*ChkCmd)(						/* Check Serial Command Data Receiving					*/
				struct _CSIF *this,			/* this pointer											*/
				USHORT *pCmdLen );			/* Pointer to the command message length				*/
	LONG	(*StxRes)(						/* Start Serial Response Data Sending					*/
				struct _CSIF *this,			/* this pointer											*/
				UCHAR *ResTxBuf,			/* pointer to the response buffer						*/
				LONG   ResTxLen );			/* response message length								*/
/*--------------------------------------------------------------------------------------------------*/
//	UCHAR	RxBuf[SIF_RX_BUFSIZE];			/* receive data buffer									*/
//	UCHAR	TxBuf[SIF_TX_BUFSIZE];			/* transmit data buffer									*/
	UCHAR	*RxBuf;							/* receive data buffer									*/
	UCHAR	*TxBuf;							/* transmit data buffer									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	RcvBufSize;						/* rcv buf size 										*/
	USHORT	SndBufSize;						/* send buf size 										*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	*pBrtPrm;						/* pointer to the serial baudrate parameter             */
/*--------------------------------------------------------------------------------------------------*/
} CSIF;
/*--------------------------------------------------------------------------------------------------*/

#endif
