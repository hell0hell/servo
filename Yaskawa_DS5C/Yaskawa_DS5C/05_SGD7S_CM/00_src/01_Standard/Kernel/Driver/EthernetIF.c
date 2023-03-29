/****************************************************************************
Description	: Ethernet UDP Server for MEMBUS protocol
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	MbusUDP_Initialize()	: Initialize the Ethernet UDP Server
	MbusUDP_Server()		: Runtime process of the MEMOBUS UDP Server
	MbusUDP_Reset()			: Close socket and Re-Initialize the UDP Server

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created

****************************************************************************/
#include <INTEGRITY.h>
#include "Basedef.h"
#ifndef _VC_DEBUG_
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#endif
#include "EthernetIF.h"
#include "KLib.h"


//#define _ETH_DEBUG_LOOP_BACK_TEST

#ifdef ETHERNET_ONLINE_RESET
/* message queue command  */
enum REQUEST_CODE {
	EVENT_NONE = 0,
	EVENT_ETHER_RESET = 2,
	EVENT_END_TASK = 4,
};

/* Structure of the message packet  */
typedef	struct QUE_PACKET {
	/* packet header */
	volatile enum REQUEST_CODE	CmdCode;
	int					DataLength;

	/* each packet data */
	int					buffer[2];
} QUE_PACKET;



/***************************************************************************
Description: Reset the Ethernet Interface
----------------------------------------------------------------------------
Parameters:
Return:      none
****************************************************************************/
static void Ether_Restart(dip_t *dip, BOOL dhcpEnable)
{
    MessageQueue	queue;
	QUE_PACKET		*pkt;
	IODevice		EtherDev;
	ULONG			cnt;
//	Time T = {5, 0}; /* 1 seconds */

	/* Check DHCP Client function	*/
    if( !dhcpEnable )
    {/* DHCP not use	*/
		if(Success == RequestResource((Object *)&EtherDev,
										"EtherDev","!systempassword"))
        {
			WriteIODeviceStatus(EtherDev,
					IODEV_ETHER_SET_DYNAMIC_IP, (void*)dip, sizeof(dip_t));
        }
    }
    else
    {/* DHCP use	*/
        /* DHCP START */
    }
	/* GET MAC ADDRESS */
//	ReadIODeviceStatus(EtherDev, IODEV_ETHER_ADDR, &mac_addr[0], 6);

    /* Initialize Message queues */
//	CreateMessageQueue(1, sizeof(QUE_PACKET), MESSAGE_QUEUE_SHARED, &ackQue);
//	RegisterMessageQueue(ackQue, "SvQue");

	OpenRegisteredMessageQueue("KnlEventQue", &queue);
	AllocateMessageQueueBuffer(queue, &((void*)pkt));

	/* Requeset to restart */
	pkt->CmdCode = EVENT_ETHER_RESET;
	pkt->DataLength = 0;
	SendOnMessageQueue(queue, pkt);

	/* wait for completed */
//	TimedReceiveOnMessageQueue(ackQue, &T, &((void*)pkt));
	cnt = 0;
	while(pkt->CmdCode != (1+EVENT_ETHER_RESET))
	{
		KlibWaitus(1000);
		if((++cnt) > 200)
		{	break;}
	}

	FreeMessageQueueBuffer(queue, (void**)&pkt);
//	CloseMessageQueue(ackQue);

}
#endif //ETHERNET_ONLINE_RESET


/***************************************************************************
Description: Initialize the Ethernet UDP Server
----------------------------------------------------------------------------
Parameters:
	ETH_UDP_SERVER		*EthUdpServer	:Pointer to this module handle
	REG_MANAGER_HANDLE	*hRegMngr		:Pointer to Register Manager handle
	UINT32				IP_Address		:IP Address
	UINT16				PortNo			:Port number
Return:
	ETH_UDP_SUCCESS(0)	: Success.
	Other				: The function failed and returns enum ETH_UDP_ERROR.
****************************************************************************/
INT32 MbusUDP_Initialize(ETH_UDP_SERVER *EthUdpServer,
						 MEMOBUS_ADDR_MAP *AxisCfg, ETH_PARAMS *ethParam)
{
	INT					Socket;
	CHAR				ioctl_flg;
	struct sockaddr_in	mySocketInfo;

	memset(EthUdpServer, 0, sizeof(ETH_UDP_SERVER));
	EthUdpServer->State = ETH_UDP_STATE_INIT;

	/* Set Socket Information. */
	memset(&mySocketInfo, 0, sizeof(mySocketInfo));
	mySocketInfo.sin_addr.s_addr = htonl(ethParam->IP_Address);
	mySocketInfo.sin_family = AF_INET;
	mySocketInfo.sin_port = htons(ethParam->PortNo);

#ifdef ETHERNET_ONLINE_RESET
	{
	dip_t dip;
	/* Reset Ethernet Network configuration */
    dip.devno = 0;
    dip.my_ip = mySocketInfo.sin_addr.s_addr;
    dip.df_gw = htonl(ethParam->DefGateway);
    dip.mask = htonl(ethParam->SubnetMask);
	Ether_Restart(&dip, FALSE);
	}
#endif //ETHERNET_ONLINE_RESET

	/* Memobus IF Initialize */
	MbusFuncInitialize(&EthUdpServer->MbusIf, AxisCfg,
						(LONG)(ethParam->IP_Address&0xFF));

	/* Create a socket */
	Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if(Socket < 0)
	{
		return ETH_UDP_ERROR_OPEN;
	}

	/* Bind a socket */
	if(0 > bind(Socket, (struct sockaddr *)&mySocketInfo, sizeof(mySocketInfo)))
	{
		/* Retry to assign system local address. */
		mySocketInfo.sin_addr.s_addr = INADDR_ANY;
		if(0 > bind(Socket, (struct sockaddr *)&mySocketInfo, sizeof(mySocketInfo)))
		{
			return ETH_UDP_ERROR_BIND;
		}
	}

	/* Nonblocking a socket */
	ioctl_flg = 1;
	if(0 > ioctl(Socket, FIONBIO, &ioctl_flg))
	{
		return ETH_UDP_ERROR_FIONBIO;
	}

	EthUdpServer->State = ETH_UDP_STATE_WAIT_RCV;
	EthUdpServer->Socket = Socket;

	return ETH_UDP_SUCCESS;
}


/***************************************************************************
Description: Close socket and Re-Initialize the UDP Server
----------------------------------------------------------------------------
Parameters:
	ETH_UDP_SERVER		*EthUdpServer	:Pointer to this module handle
	REG_MANAGER_HANDLE	*hRegMngr		:Pointer to Register Manager handle
	UINT32				IP_Address		:IP Address
	UINT16				PortNo			:Port number
Return:
	ETH_UDP_SUCCESS(0)	: Success.
	Other				: The function failed and returns enum ETH_UDP_ERROR.
****************************************************************************/
INT32 MbusUDP_Reset(ETH_UDP_SERVER *EthUdpServer, ETH_PARAMS *ethParam)
{
#if 0
	close(EthUdpServer->Socket);
	return MbusUDP_Initialize(EthUdpServer,
				EthUdpServer->MbusIf.hRegMngr, ethParam);
#endif

	return ETH_UDP_SUCCESS;
}


/***************************************************************************
Description: Runtime process of the MEMOBUS UDP Server
----------------------------------------------------------------------------
Parameters:
	ETH_UDP_SERVER		*EthUdpServer	:Pointer to this module handle
Return: none
****************************************************************************/
void MbusUDP_Server(ETH_UDP_SERVER *EthUdpServer)
{
	fd_set				fdsetRead;
	struct sockaddr_in	fromInfo;
	INT					Socket;
	struct timeval		timeOut;
	INT					msgsize, fromlen;

	if(EthUdpServer->State == ETH_UDP_STATE_INIT)
	{
		return;
	}

	/* Set the read file descriptor */
	Socket = EthUdpServer->Socket;
	FD_ZERO((&fdsetRead));
	FD_SET(Socket, (&fdsetRead));

	/* Set timeout value to 0 millisecond. */
	timeOut.tv_sec  = 0;
	timeOut.tv_usec = 0;
	select(FD_SETSIZE, &fdsetRead, NULL, NULL, &timeOut);

	/* Check received message available */
	if(FD_ISSET(Socket, &fdsetRead))
	{
		/* Receive a UDP packet. */
		fromlen = sizeof(fromInfo);
		msgsize = recvfrom(Socket, EthUdpServer->MbusIf.RxBuf,
					MSG_MAXBUFSZB, 0, (struct sockaddr *)&fromInfo, &fromlen);

		if(msgsize > 0)
		{
#ifdef _ETH_DEBUG_LOOP_BACK_TEST
			memcpy(EthUdpServer->MbusIf.TxBuf, EthUdpServer->MbusIf.RxBuf, msgsize);
#else
			/* Execute MEMOBUS function. */
			EthUdpServer->MbusIf.RcvbLength = msgsize;
			msgsize = (INT)MbusFuncMsgProcedure(&EthUdpServer->MbusIf);
#endif
			/* Send a UDP packet. */
			sendto(Socket, EthUdpServer->MbusIf.TxBuf,
					msgsize, 0, (const struct sockaddr *)&fromInfo, fromlen);

			/* Save a current time. */
			KlibRstLongTimer(&EthUdpServer->RecvTime);
			EthUdpServer->State = ETH_UDP_STATE_WAIT_NEXTRCV;
		}
	}
	else
	{
		if( (EthUdpServer->State == ETH_UDP_STATE_WAIT_NEXTRCV)
			&& (20000 < KlibGetLongTimerMs(EthUdpServer->RecvTime)) )
		{
			/* Clear my execution right of Utility functions. */

			EthUdpServer->State = ETH_UDP_STATE_WAIT_RCV;
		}
	}
}



/* End of File */
