/****************************************************************************
Description	: Network Interface modules
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.12.18     created

****************************************************************************/
#include "Basedef.h"
#include "NetworkIf.h"
#include "HwDeviceIf.h"
#include "EepromIF.h"



//void MsgIfInitialize(NETMSG_IF *NetMsgIf, MEMOBUS_ADDR_MAP *AddrMap, 		  /* <S10E> */
//	                 ULONG AddrMapNum, ULONG NodeAddr, ULONG portNo)
void MsgIfInitialize(NETMSG_IF *NetMsgIf, MEMOBUS_ADDR_MAP *AddrMap, 
										ULONG AddrMapNum, ULONG NodeAddr)
{
	/* Ethernet UDP message initialize */
#ifdef CSW_ETHERNET_ENABLE
	ETH_PARAMS		ethParam;

	/* Ethernet message initialize */
	dsw = HALdrv_GetDipSwitchSetting();
	if(0 != (dsw & DSW_EINIT_BIT))/* E-INIT=ON */
	{
		ethParam.IP_Address = ETH_DEFAULT_IP | HALdrv_GetStationAddress();
	}
	else
	{
		EepdevReadValues( (AddrMap->hRegMngr->hPnReg)->hEepDev, 0,
		                  EEP_NOSUMCMN_ADDRESS(EEP_IPADDR_OFS),
		                  (USHORT*)&ethParam.IP_Address, 2 );
	}
	ethParam.PortNo = portNo;
	MbusUDP_Initialize(&NetMsgIf->EthUdpServer, AddrMap, &ethParam);
#endif


#if 0	/* <S150> */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	/* C1 message initialize */
	M3MsgIfInit( &NetMsgIf->M3MsgC1V, AddrMap, COM2_OPT1 );
	/* C2 message initialize */
	M3MsgIfInit( &NetMsgIf->M3MsgC2V, AddrMap, COM3_OPT2 );
#endif
#endif	/* <S150> */

#if 0	/* <S150> */
//#if 0	// <USB-FUNCTION> 暫定コメントアウト → コメントアウト解除<S001>
#ifdef _VC_DEBUG_
	/* Uart message initialize */
	MbusDebug_Initialize(&NetMsgIf->UartMsg, AddrMap, NodeAddr);
#else
	/* Uart message initialize */
	MbusUart_Initialize(&NetMsgIf->UartMsg, AddrMap, NodeAddr);
#endif
//#endif 	// <USB-FUNCTION> 暫定コメントアウト → コメントアウト解除<S001>
#endif	/* <S150> */

#if 0	/* <S150> */
/* --> <USB-FUNCTION> 2013.05.12 K.Sakamoto */
//#ifdef _USB_FUNC_USE_/*<S138>*/
	/* USB message initialize*/
	MbusUsb_Initialize(&NetMsgIf->UsbMbusServer, AddrMap, NodeAddr);
//#endif	//#ifdef _USB_FUNC_USE_/*<S138>*/
/* <-- <USB-FUNCTION> 2013.05.12 K.Sakamoto */
#endif	/* <S150> */
}



void MsgIfPollTxRx(NETMSG_IF *NetMsgIf)
{
#if 0	// <USB-FUNCTION> 暫定コメントアウト
	/* UART Polling procedure */
	MbusUart_PollTrans(&NetMsgIf->UartMsg);
#endif	// <USB-FUNCTION> 暫定コメントアウト
}

/* --> <USB-FUNCTION> 2013.05.12 K.Sakamoto */
#if 0	/* <S150> */
//#ifdef _USB_FUNC_USE_/*<S138>*/
/* スキャンB用ポーリング処理 */
void MsgIfPollScanB(NETMSG_IF *NetMsgIf)
{
	/* USBファンクション バルク転送用ポーリング処理 */
	MbusUsb_PollOutIn(&(NetMsgIf->UsbMbusServer));
}
//#endif	//#ifdef _USB_FUNC_USE_/*<S138>*/
/* <-- <USB-FUNCTION> 2013.05.12 K.Sakamoto */
#endif	/* <S150> */

void MsgIfBackgroundProdedure(NETMSG_IF *NetMsgIf)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	M3MsgIfManager(&NetMsgIf->M3MsgC1V);	/* C1 Msg */
	M3MsgIfManager(&NetMsgIf->M3MsgC2V);	/* C2 Msg */
#endif

#ifdef CSW_ETHERNET_ENABLE
	MbusUDP_Server(&NetMsgIf->EthUdpServer);
#endif

#if 0	// <USB-FUNCTION> 暫定コメントアウト
#ifdef _VC_DEBUG_
	MbusDebug_Server(&NetMsgIf->UartMsg);
#else
	MbusUart_Server(&NetMsgIf->UartMsg);
#endif	/* UART Polling procedure */
#endif	// <USB-FUNCTION> 暫定コメントアウト

#if 0	/* <S072> */
/* --> <USB-FUNCTION> 2013.05.18 K.Sakamoto */
//#ifdef _USB_FUNC_USE_/*<S138>*/
	MbusUsb_Server(&NetMsgIf->UsbMbusServer);
//#endif	//#ifdef _USB_FUNC_USE_/*<S138>*/
/* <-- <USB-FUNCTION> 2013.05.18 K.Sakamoto */
#endif
}




