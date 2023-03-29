/****************************************************************************
Description	: UART Transmission Server for MEMBUS protocol
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	MbusUart_Initialize()	: Initialize the UART Memobus Server
	MbusUart_Server()		: Runtime process of the UART Memobus Server

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.10.18     created

****************************************************************************/
#include "Basedef.h"
#include "UartMsgIF.h"
#include "KLib.h"
#include "MLib.h"

#if !(defined _VC_DEBUG_)
#include "Uart.h"
#endif


#define UART_1CHAR_TIMEOUT		100			/* 100ms */
#define UART_NEXTMSG_TIMEOUT	10000		/* 10s */
#define UART_MIN_SIZE			6

#define UART_COMPRM	(LCR_DBIT8|LCR_STOP1|LCR_PARENB|LCR_EVNPAR)


/***************************************************************************
Description: Initialize the UART Memobus Server
----------------------------------------------------------------------------
Parameters:
	UART_MBUS_SERVER	*UartServer	:Pointer to this module handle
	MEMOBUS_ADDR_MAP	*AxisCfg	:Pointer to Axis Address configuration
	LONG				myAddress	:my address
Return:
	0					: Success.
	Other				: error.
****************************************************************************/
#if 0	/*<S150>*/
INT32 MbusUart_Initialize(UART_MBUS_SERVER *UartServer,
						 MEMOBUS_ADDR_MAP *AxisCfg, LONG myAddress)
{
#if !(defined _VC_DEBUG_)
	MlibResetByteMemory(UartServer, sizeof(UART_MBUS_SERVER));

	MbusFuncInitialize(&UartServer->MbusIf, AxisCfg, myAddress);

//	InitializeUART(kBaud38400, UART_COMPRM);

//	UartServer->State = UART_STATE_WAIT_RX;
#endif

	return 0;
}
#endif


#if 0 /* Žb’è */

/***************************************************************************
Description: Runtime process of the UART Memobus Server
----------------------------------------------------------------------------
Parameters:
	UART_MBUS_SERVER	*UartServer	:Pointer to this module handle
Return: none
****************************************************************************/
void MbusUart_PollTrans(UART_MBUS_SERVER *UartServer)
{
#if !(defined _VC_DEBUG_)
	LONG	len, rem_sz;
	UCHAR	*bufptr;

	switch(UartServer->State)
	{
	case UART_STATE_INIT:
		break;

	case UART_STATE_WAIT_RX:
	case UART_STATE_RECEIVE:
	/* Check received message available */
		rem_sz = MSG_MAXBUFSZB - UartServer->MbusIf.RcvbLength;
		bufptr = &UartServer->MbusIf.RxBuf[UartServer->MbusIf.RcvbLength];
		len = ReadUARTFifo(bufptr, rem_sz);
		if(len > 0)
		{
			if( ((rem_sz-len) < 0) || 
				(UartServer->MbusIf.RxBuf[0] != (UCHAR)UartServer->MbusIf.NodeAddress) )
			{/* buffer size over or unmatch to my address */
				UartServer->MbusIf.RcvbLength = 0;
			}
			else
			{
				UartServer->MbusIf.RcvbLength += (USHORT)len;
			}

			KlibRstLongTimer(&UartServer->RecvTime);
			UartServer->State = UART_STATE_RECEIVE;
		}
		else
		{
			if(UART_1CHAR_TIMEOUT <= KlibGetLongTimerMs(UartServer->RecvTime))
			{
				KlibRstLongTimer(&UartServer->RecvTime);

				if(UART_MIN_SIZE < UartServer->MbusIf.RcvbLength)
				{/* valid packet */
					if(FALSE != MlibChkCRC16(UartServer->MbusIf.RxBuf, UartServer->MbusIf.RcvbLength-2)) 
					{
						WriteUARTFlush();
						TerminateUART();
						UartServer->TxCnt = 0;
						UartServer->State = UART_STATE_WAIT_TX;
					}
					else
					{
						UartServer->State = UART_STATE_WAIT_RX;
						UartServer->MbusIf.RcvbLength = 0;
					}
				}
				else
				{/* invalid packet(short packet or crc error) */
					UartServer->State = UART_STATE_WAIT_RX;
					UartServer->MbusIf.RcvbLength = 0;
				}
			}
		}
		break;

	case UART_STATE_WAIT_TX:
		break;

	case UART_STATE_SEND:
		rem_sz = UartServer->MbusIf.TrnbLength - UartServer->TxCnt;
		bufptr = &UartServer->MbusIf.TxBuf[UartServer->TxCnt];

		len = WriteUARTFifo(bufptr, rem_sz);
		UartServer->TxCnt += len;
		if(rem_sz <= len)
		{
			UartServer->State = UART_STATE_WAIT_RX;
			KlibRstLongTimer(&UartServer->RecvTime);
			UartServer->MbusIf.RcvbLength = 0;
		}
		break;

	default:
		break;
	}
#endif
}



void MbusUart_Server(UART_MBUS_SERVER *UartServer)
{
	if(UartServer->State == UART_STATE_WAIT_TX)
	{
		MbusFuncMsgProcedure( &UartServer->MbusIf );

		MlibSetCRC16(UartServer->MbusIf.TxBuf, UartServer->MbusIf.TrnbLength);
		UartServer->MbusIf.TrnbLength += 2;

		UartServer->State = UART_STATE_SEND;
	}
}

#endif


#ifdef _VC_DEBUG_
INT32 MbusDebug_Initialize(UART_MBUS_SERVER *UartServer,
						 MEMOBUS_ADDR_MAP *AxisCfg, LONG myAddress)
{
	MbusFuncInitialize(&UartServer->MbusIf, AxisCfg, myAddress);
	return 0;
}


void MbusDebug_Server(UART_MBUS_SERVER *UartServer)
{
	MbusFuncMsgProcedure( &UartServer->MbusIf );
}
#endif



/* End of File */
