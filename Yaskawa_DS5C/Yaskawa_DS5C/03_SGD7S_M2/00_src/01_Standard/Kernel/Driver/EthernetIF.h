/****************************************************************************
Description	: Ethernet UDP Server for MEMBUS protocol
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created

****************************************************************************/
#ifndef __ETHERNET_IF_H 
#define __ETHERNET_IF_H

#include "MemobusIF.h"

/* defines */
#define ETH_DEFAULT_IP		0xC0A80100	/* 192.168.1.0 */


/* definition of API error code */
enum ETH_UDP_ERROR {
	ETH_UDP_SUCCESS			= (LONG)0x00000000L,
	ETH_UDP_ERROR_OPEN		= (LONG)0x80000001L,
	ETH_UDP_ERROR_BIND		= (LONG)0x80000002L,
	ETH_UDP_ERROR_FIONBIO	= (LONG)0x80000003L,
};

/* definition of Ethernet process state */
typedef enum ETH_UDP_STATE_ {
	ETH_UDP_STATE_INIT = 0,
	ETH_UDP_STATE_WAIT_RCV,
	ETH_UDP_STATE_WAIT_NEXTRCV,
} ETH_UDP_STATE;

/* Structure of Ethernet I/F handle */
typedef struct ETH_UDP_SERVER_ {
	MEMOBUS_IF		MbusIf;			/* MEMOBUS function Interface */
	ETH_UDP_STATE	State;			/* Communication state */
	INT				Socket;			/* socket descriptor */
	ULONG			RecvTime;		/* the time from the last received packet */
} ETH_UDP_SERVER;


typedef struct ETH_PARAMS_ {
  INT		IP_Address;
  INT		DefGateway;
  INT		SubnetMask;
  USHORT	PortNo;
} ETH_PARAMS;


#ifdef __cplusplus
extern	"C"	{
#endif

/* API's */
INT32 MbusUDP_Initialize(ETH_UDP_SERVER *EthUdpServer,
						 MEMOBUS_ADDR_MAP *AxisCfg, ETH_PARAMS *ethParam);

INT32 MbusUDP_Reset(ETH_UDP_SERVER *EthUdpServer,ETH_PARAMS *ethParam); 

void MbusUDP_Server(ETH_UDP_SERVER *EthUdpServer);


#ifdef __cplusplus
}
#endif



#endif //__ETHERNET_IF_H 

