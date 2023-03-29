/****************************************************************************
Description	: Network Interface modules
----------------------------------------------------------------------------
File name		: NetworkIf.h			<S008>

Author			: Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project			: INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.12.18     created

****************************************************************************/
#ifndef __NETWORK_IF_H 
#define __NETWORK_IF_H


#include "MemobusIF.h"
#include "UartMsgIF.h"
#include "UsbFunctionIF.h"	/* <USB-FUNCTION> 2013.05.12 K.Sakamoto */


/****************************************************************************
	Settings according to CSW_NETIF_TYPE
****************************************************************************/
/*--------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)					/* MECATROLINK- M2	*/
/*--------------------------------------------------------------------------*/
#include "M2TaskIfMain.h"
#include "M2Handler.h"
#include "NetworkIdRegFunc.h"
#include "MtApi.h"

#ifdef CSW_ETHERNET_ENABLE
#include "EthernetIF.h"
#endif

typedef MLNET_HANDLE 		NETIF_AXIS_HANDLE;
typedef ML_NODE_COMMONS 	NETIF_SYS_HANDLE;

typedef struct NETMSG_IF {
	UART_MBUS_SERVER	UartMsg;			/* UART Messages IF			*/
	USB_MBUS_SERVER		UsbMbusServer;		/* USB Server */ /* <USB-FUNCTION> 2013.05.18 K.Sakamoto */
#ifdef CSW_ETHERNET_ENABLE
	ETH_UDP_SERVER		EthUdpServer;		/* Ethernet UDP Server		*/
#endif
} NETMSG_IF;

BOOL OpiIntSync( VOID *axRsc, BOOL IntStatus );
#define OpiOutputProcess(a,b)

#define NetworkIdRegTable			M2IdRegTable
#define NetworkIdRegTableEntNum		M2IdRegTableEntNum



/*--------------------------------------------------------------------------*/
#elif (CSW_NETIF_TYPE == NETIF_M3_TYPE)					/*  MECATROLINK-3	*/
/*--------------------------------------------------------------------------*/
#include "M3TaskIfMain.h"
#include "M3MsgIF.h"
#include "M3Handler.h"
#include "NetworkIdRegFunc.h"
#include "MtApi.h"

#ifdef CSW_ETHERNET_ENABLE
#include "EthernetIF.h"
#endif

typedef MLNET_HANDLE 		NETIF_AXIS_HANDLE;
typedef ML_NODE_COMMONS 	NETIF_SYS_HANDLE;

typedef struct NETMSG_IF {
	UART_MBUS_SERVER	UartMsg;			/* UART Messages IF										*/
	M3MSGIFV			M3MsgC1V;			/* C1メッセージ通信データ								*/
	M3MSGIFV			M3MsgC2V;			/* C2メッセージ通信データ								*/
	USB_MBUS_SERVER		UsbMbusServer;		/* USB Server */ /* <USB-FUNCTION> 2013.05.18 K.Sakamoto */
#ifdef CSW_ETHERNET_ENABLE
	ETH_UDP_SERVER		EthUdpServer;		/* Ethernet UDP Server									*/
#endif
} NETMSG_IF;

BOOL OpiIntSync( VOID *axRsc, BOOL IntStatus );
#define OpiOutputProcess(a,b)

#define NetworkIdRegTable			M3IdRegTable
#define NetworkIdRegTableEntNum		M3IdRegTableEntNum

/*--------------------------------------------------------------------------*/
#elif (CSW_NETIF_TYPE == NETIF_ECAT_TYPE)						/* EtherCAT	*/
/*--------------------------------------------------------------------------*/
#include "AP_Resources.h"
	#include "MtApi.h"

#define CSW_ETHERNET_ENABLE

#include "EthernetIF.h"

typedef COE_AXIS_RSC_T 	NETIF_AXIS_HANDLE;
typedef COE_SYS_RSC_T 	NETIF_SYS_HANDLE;

typedef struct NETMSG_IF {
	UART_MBUS_SERVER	UartMsg;			/* UART Messages IF										*/
	USB_MBUS_SERVER		UsbMbusServer;		/* USB Server */ /* <USB-FUNCTION> 2013.05.18 K.Sakamoto */
#ifdef CSW_ETHERNET_ENABLE
	ETH_UDP_SERVER		EthUdpServer;		/* Ethernet UDP Server		*/
#endif
} NETMSG_IF;


#define OpiIntSync(a,b)
void OpiOutputProcess( void *axRsc, BOOL IntStatus );



/*--------------------------------------------------------------------------*/
#elif CSW_NETIF_TYPE == NETIF_SYNQNET_TYPE						/* SYNQNET	*/
/*--------------------------------------------------------------------------*/
#include "SynqNetTaskIfMain.h"

typedef SYNQNET_AXIS_HANDLE 	NETIF_AXIS_HANDLE;
typedef SYNQNET_NODE_COMMONS 	NETIF_SYS_HANDLE;

typedef struct NETMSG_IF {
	UART_MBUS_SERVER	UartMsg;			/* UART Messages IF										*/
	USB_MBUS_SERVER		UsbMbusServer;		/* USB Server */ /* <USB-FUNCTION> 2013.05.18 K.Sakamoto */
} NETMSG_IF;



/*-------------------------------------------------------------------------*/
#elif CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE						/* Anlgpls	*/
/*-------------------------------------------------------------------------*/
#ifdef CSW_ETHERNET_ENABLE
#include "EthernetIF.h"
#endif

typedef struct _NETMSG_IF {
	UART_MBUS_SERVER	UartMsg;			/* UART Messages IF		*/
	USB_MBUS_SERVER		UsbMbusServer;		/* USB Server */ /* <USB-FUNCTION> 2013.05.18 K.Sakamoto */
#ifdef CSW_ETHERNET_ENABLE
	ETH_UDP_SERVER		EthUdpServer;		/* Ethernet UDP Server	*/
#endif
} NETMSG_IF;



/*--------------------------------------------------------------------------*/
#else															/* no IF	*/
/*--------------------------------------------------------------------------*/

//no defined network interface  //stamp

#endif



/* MECHATROLINK */
void OpiInitialize( void *axRsc ) ;
void OpiCalcInitParam( void *axRsc );
void OpiInitDevice( void *axRsc, void *DeviceAddress );
void OpiStartDevice( void *axRsc, void *DeviceAddress );
BOOL OpiRoundProcedure(void *axRsc);
BOOL OpiInputProcess( void *axRsc, BOOL IntStatus );
void OpiRunTimeScanProcess( void *AxisRsc, BOOL irqSts );
UINT OpiScanCProcess(void *AxisRsc);
void OpiSoftReset( void *axRsc );
BOOL OpiCheckStableState( void *axRsc );	/*<S1DB>*/
void OpiResetSyncExe( void *axRsc );	/*<S1DB>*/


/* Message */
//void MsgIfInitialize(NETMSG_IF *NetMsgIf, MEMOBUS_ADDR_MAP *AddrMap,
//	                 ULONG AddrMapNum, ULONG NodeAddr, ULONG portNo);
void MsgIfInitialize(NETMSG_IF *NetMsgIf, MEMOBUS_ADDR_MAP *AddrMap, 
										ULONG AddrMapNum, ULONG NodeAddr);	/* <S10E> */
void MsgIfPollTxRx(NETMSG_IF *NetMsgIf);
void MsgIfPollScanB(NETMSG_IF *NetMsgIf);	/* <USB-FUNCTION> 2013.05.12 K.Sakamoto */
void MsgIfBackgroundProdedure(NETMSG_IF *NetMsgIf);



#endif //__NETWORK_IF_H 
/***************************************** end of file **********************************************/
