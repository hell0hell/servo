/****************************************************************************
Description	: UART Transmission Server for MEMBUS protocol
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.10.18     created

****************************************************************************/
#ifndef __UART_MBUS_IF_H 
#define __UART_MBUS_IF_H

#include "MemobusIF.h"


/* definition of transmission process state */
typedef enum UART_TRANS_STATE_ {
	UART_STATE_INIT = 0,
	UART_STATE_WAIT_RX,
	UART_STATE_RECEIVE,
	UART_STATE_WAIT_TX,
	UART_STATE_SEND,
} UART_TRANS_STATE;

/* Structure of Ethernet I/F handle */
typedef struct UART_MBUS_SERVER_ {
	MEMOBUS_IF			MbusIf;			/* MEMOBUS function Interface */
	UART_TRANS_STATE	State;			/* Communication state */
	ULONG				RecvTime;		/* the time from the last received packet */
	ULONG				TxCnt;			/*  */
} UART_MBUS_SERVER;



#ifdef __cplusplus
extern	"C"	{
#endif

/* API's */
INT32 MbusUart_Initialize(UART_MBUS_SERVER *UartServer,
						 MEMOBUS_ADDR_MAP *AxisCfg, LONG myAddress);
void MbusUart_PollTrans(UART_MBUS_SERVER *UartServer);
void MbusUart_Server(UART_MBUS_SERVER *UartServer);

#ifdef _VC_DEBUG_
INT32 MbusDebug_Initialize(UART_MBUS_SERVER *UartServer,
						 MEMOBUS_ADDR_MAP *AxisCfg, LONG myAddress);
void MbusDebug_Server(UART_MBUS_SERVER *UartServer);
#endif

#ifdef __cplusplus
}
#endif



#endif //__UART_MBUS_IF_H 

