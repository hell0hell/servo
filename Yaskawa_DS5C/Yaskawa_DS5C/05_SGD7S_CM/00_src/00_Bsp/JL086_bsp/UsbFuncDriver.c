/****************************************************************************
Description	: USB function driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	-- APIs --
	UsbFuncInitializeDriver()		: USBドライバ初期化処理
	UsbFuncChkAvailable()			: USB使用可能判定処理	<S138>
	 - 割り込み関連 -
	UsbFuncIntIntu2f()				: INTU2F 割り込み処理
	UsbFuncIntIntu2fepc()			: INTU2FEPC 割り込み処理
	 - コントロール転送関連 -
	UsbFuncTaskControl()			: コントロール転送タスク処理
	 - バルク転送関連処理 -
	UsbFuncPollBulkOutIn()			: USB バルク転送制御処理
	UsbFuncResetBuffer()			: USB Endpoint1/2 送受信バッファクリア処理
	 - USB MEMOBUSサーバー用関連 -
	UsbFuncStartBkout()				: USB Endpoint2 受信開始(BLUK OUT)処理
	UsbFuncStartBkin()				: USB Endpoint1 送信開始(BLUK IN)処理
	UsbFuncPortConfiguration()		: USBポート状態通知サービス処理
	UsbFuncGetRcvSize()				: USB Endpoint2 受信バイト数取得処理
	UsbFuncChkRcvComp()				: バルク転送(OUT)状態通知サービス処理
	UsbFuncChkTrnsComp()			: バルク転送(IN)状態通知サービス処理

	-- Locals --
	VendorRequest_Callback()		: ベンダーリクエストコールバック処理
	UsbFuncInitializeInfo()			: USBファンクション情報初期化処理
	UsbFuncClearFlag()				: フラグ・クリア処理
	UsbFuncSetMaxpacket()			: Max Packet Size 設定処理
	UsbFuncGetInfoAddress()			: USB情報領域のアドレス取得処理
	UsbFuncCheckPlugInout()			: 接続状態確認処理
	UsbFuncCheckSetupReceive()		: SETUPトークン受信確認処理
	 - EPn転送関連 -
	UsbFuncSetStallEPn()			: EPn ストール応答処理
	UsbFuncOutDataSplitCopy()		: OUTデータ分割コピー処理 <S182>
	UsbFuncInDataSplitCopy()		: INデータ分割コピー処理	 <S182>
	 - コントロール転送関連 -
	UsbFuncWriteEP0()				: EP0 送信処理
	UsbFuncSetStallEP0()			: EP0 ストール応答処理
	UsbFuncSetNakEP0()				: EP0 NAK応答処理
	UsbFuncClearNakEP0()			: EP0 NAK解除処理
	UsbFuncSendNullEP0()			: EP0 NULL送信処理
	control_nodata()				: EP0 No Data処理
	control_read()					: EP0 Control Read処理
	control_write()					: EP0 Control Write処理
	decode_request()				: EP0 リクエストデコード処理
	bmRequestType_standard()		: EP0 スタンダードリクエスト処理
	std_req_clear_feature()			: EP0 CLEAR_FEATURE処理
	Clear_Feature_EP()				: EP0 CLEAR_FEATURE処理( TARGET = EP )
	std_req_get_configuration()		: EP0 GET_CONFIGURATION処理
	std_req_get_descriptor()		: EP0 GET_DESCRIPTOR処理
	Get_Descriptor_Device()			: EP0 GET_DESCRIPTOR DEVICE処理
	Get_Descriptor_Configuration()	: EP0 GET_DESCRIPTOR CONFIGURATION処理
	Get_Descriptor_String()			: EP0 GET_DESCRIPTOR STRING処理
	Get_Descriptor_Qualifier()		: EP0 GET_DESCRIPTOR DEVICE QUALIFIER処理
	Get_Descriptor_OtherSpeed()		: EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION処理
	std_req_get_interface()			: EP0 GET_INTERFACE処理
	std_req_get_status()			: EP0 GET_STATUS処理
	Get_Status_Device()				: EP0 GET_STATUS DEVICE処理
	Get_Status_EP()					: EP0 GET_STATUS ENDPOINT処理
	std_req_set_address()			: EP0 SET_ADDRESS処理
	std_req_set_configuration()		: EP0 SET_CONFIGURATION処理
	std_req_set_descriptor()		: EP0 SET_DESCRIPTOR処理
	std_req_set_feature()			: EP0 SET_FEATURE処理
	Set_Feature_Device()			: EP0 SET_FEATURE処理( TARGET = DEVICE )
	Set_Feature_EP()				: EP0 SET_FEATURE処理( TARGET = EP )
	std_req_set_interface()			: EP0 SET_INTERFACE処理
	std_req_synch_frame()			: EP0 SYNCH_FRAME処理
	Check_USB_DIR_EPn()				: EP方向のUSB_DIR_OUT/IN変換処理
	udev_get_DeviceState()			: デバイスステート取得処理
	info_get_CurrConfigDesc()		: info カレントコンフィギュレーションデスクリプタの取得処理
	info_find_InterfaceDesc()		: info インターフェースデスクリプタの取得処理
	EPn_get_STALL()					: EPn STALL状態取得処理

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.05.12		created

****************************************************************************/
#include "Basedef.h"
#include "config.h"
//<S146>#include "nbpfahb32vic128.h"
#include "ARMPF_USB_FUNC.h"
#include "usb_f.h"
#include "u2f_desc.h"
#include "MLib.h"
#include "Klib.h"/*<S0C7>*/
#ifdef	_MERCURY_PLATFORM_		/*<S190>*/
#ifndef NULL
	#define NULL 0
#endif
#endif	/* _MERCURY_PLATFORM_ */
/*===========================================================================*/
/* defines																	 */
/*===========================================================================*/
/* Max Packet Size (High Speed, Full Speed) */
#define BULK_MAXPACKET_SIZE_HS	 512					/* High Speed時バルク転送最大サイズ		*/
#define BULK_MAXPACKET_SIZE_FS	  64					/* Full Speed時バルク転送最大サイズ		*/
#define INT_MAXPACKET_SIZE_HS	1024					/* High Speed時割り込み転送最大サイズ	*/
#define INT_MAXPACKET_SIZE_FS	  64					/* Full Speed時割り込み転送最大サイズ	*/

/* Packet Size (High Speed, Full Speed) */
#define PACKET_SIZE_FULLSPEED_H		((UINT8)0x00)		/* FS Packet Size :  64byte	*/
#define PACKET_SIZE_FULLSPEED_L		((UINT8)0x40)		/*							*/
#define PACKET_SIZE_HIGHSPEED_H		((UINT8)0x02)		/* HS Packet Size : 512byte	*/
#define PACKET_SIZE_HIGHSPEED_L		((UINT8)0x00)		/*							*/

#define USB_VENDOR_ID_H				((UINT8)0x13)		/* Vendor ID = Yaskawa Electric(0x137C)	*/
#define USB_VENDOR_ID_L				((UINT8)0x7C)		/*										*/
#define USB_PRODUCT_ID_H			((UINT8)0x02)		/* Product ID (0x0250)		*/
#define USB_PRODUCT_ID_L			((UINT8)0x50)		/*							*/
#define USB_RELEASE_NUM_H			((UINT8)0x01)		/* Release Number (0x0100)	*/
#define USB_RELEASE_NUM_L			((UINT8)0x00)		/*							*/
#define USB_VERSION_H				((UINT8)0x02)		/* USB2.0					*/
#define USB_VERSION_L				((UINT8)0x00)		/*							*/

#define USB_INTERFACE_CLASS			((UCHAR)0x00)		/* bInterfaceClass    (none)			*/
#define USB_INTERFACE_SUB_CLASS		((UCHAR)0x00)		/* bInterfaceSubClass (none)			*/
#define USB_INTERFACE_PROTOCOL		((UCHAR)0xFF)		/* bInterfaceProtocol (Vendor original)	*/

/* USB Device State	*/
#define UDEV_ST_IDLE		BIT0						/* Idle(Attached)ステート	*/
#define UDEV_ST_DEFAULT		BIT1						/* Defaultステート			*/
#define UDEV_ST_ADDRESS		BIT2						/* Addressステート			*/
#define UDEV_ST_CONFIGURED	BIT3						/* Configuredステート		*/
#define UDEV_ST_SUSPENDED	BIT4						/* Suspendステート			*/

/* U2F FLAG	*/
#define U2F_ENABLE			1							/* 許可						*/
#define U2F_DISABLE			0							/* 禁止						*/

/* 送受信データコピー時　1スキャンでのコピーサイズ */ /*<S182>*/
#define SCAN_COPY_SIZE		64							/* 1スキャンでのコピーサイズ(4*n)(変更禁止)	*/

/*===========================================================================*/
/* globals in this file														 */
/*===========================================================================*/
static USB_FUNCTION_IF	UsbfIf;							/* ドライバ内部データ					*/
static BOOL				UsbFunctionAvailable;			/* USB使用可能判定フラグ				*//* <S138> */

/*===========================================================================*/
/* const datas																 */
/*===========================================================================*/
/*-----------------------------------------------*/
/* Device Descriptor							 */
/*-----------------------------------------------*/
const DEVICE_DESCRIPTOR UsbFunctionDeviceDescriptor = {
	(UCHAR)0x12,								/* bLength(18)				*/
	(UCHAR)USB_DESC_TYPE_DEVICE,				/* bDescriptor(1)			*/
	{
		(UINT8)USB_VERSION_L,						/* bcdUSB(0x0200)		*/
		(UINT8)USB_VERSION_H,						/* bcdUSB(0x0200)		*/
	},
	(UCHAR)0x00,								/* bDeviceClass				*/
	(UCHAR)0x00,								/* bDeviceSubClass			*/
	(UCHAR)0x00,								/* bDeviceProtocol			*/
	(UCHAR)0x40,								/* bMaxPacketSize0(64)		*/
	{
		(UINT8)USB_VENDOR_ID_L,						/* idVendor				*/
		(UINT8)USB_VENDOR_ID_H,						/* idVendor				*/
	},
	{
		(UINT8)USB_PRODUCT_ID_L,					/* idProduct			*/
		(UINT8)USB_PRODUCT_ID_H,					/* idProduct			*/
	},
	{
		(UINT8)USB_RELEASE_NUM_L,					/* bcdDevice			*/
		(UINT8)USB_RELEASE_NUM_H,					/* bcdDevice			*/
	},
	(UCHAR)STRIDX_MANUFACTURER,					/* iManufacturer			*/
	(UCHAR)STRIDX_PRODUCT,						/* iProduct					*/
/*	FIXME:デバッグ用処置あり(後日確定させる必要有り) */
//	(UCHAR)STRIDX_SERIALNUMBER,					/* iSerialNumber			*/
	(UCHAR)0x00,					/* iSerialNumber			*//* for debug tanaka21	*/
	(UCHAR)0x01,								/* bNumConfigurations		*/
};

/*-----------------------------------------------*/
/* Qualifier Descriptor							 */
/*-----------------------------------------------*/
const DEVICE_QUALIFIER_DESCRIPTOR UsbFunctionQualifierDescriptor = {
	(UCHAR)0x0a,								/* bLength;					*/
	(UCHAR)USB_DESC_TYPE_DEVICE_QUALIFIER,		/* bDescriptorType;			*/
	{
		(UINT8)USB_VERSION_L,						/* bcdUSB(0x0200)		*/
		(UINT8)USB_VERSION_H,						/* bcdUSB(0x0200)		*/
	},
	(UCHAR)0x00,								/* bDeviceClass;			*/
	(UCHAR)0x00,								/* bDeviceSubClass;			*/
	(UCHAR)0x00,								/* bDeviceProtocol;			*/
	(UCHAR)0x40,								/* bMaxPacketSize0;			*/
	(UCHAR)0x01,								/* bNumConfigurations;		*/
	(UCHAR)0x00,								/* bReserved;				*/
};

/*-----------------------------------------------*/
/* Configuration Descriptor (FS)				 */
/*-----------------------------------------------*/
const DESC_CONFIG UsbFunctionConfigurationDescriptorFS = {
	/* Configuration	*/
	{
		(UCHAR)0x09,							/* bLength(9)				*/
		(UCHAR)USB_DESC_TYPE_CONFIGURATION,		/* bDescriptor(2)			*/
		{
			(UINT8)0x20,							/* wTotalLength(32)		*/
			(UINT8)0x00,							/* wTotalLength			*/
		},
		(UCHAR)0x01,							/* bNumInterfaces			*/
		(UCHAR)0x01,							/* bConfigurationValue		*/
		(UCHAR)STRIDX_CONFIGURATION,			/* iConfiguration			*/
		(UCHAR)0xc0,							/* bmAttributes				*/
		(UCHAR)0x00,							/* MaxPower					*/
	},
	/* Interface	*/
	{
		/* Interface[0]	*/
		{
			(UCHAR)0x09,							/* bLength(9)					*/
			(UCHAR)USB_DESC_TYPE_INTERFACE,			/* bDescriptorType				*/
			(UCHAR)0x00,							/* bInterfaceNumber				*/
			(UCHAR)0x00,							/* bAlternateSetting			*/
			(UCHAR)0x02,							/* bNumEndpoints				*/
			(UCHAR)USB_INTERFACE_CLASS,				/* bInterfaceClass				*/
			(UCHAR)USB_INTERFACE_SUB_CLASS,			/* bInterfaceSubClass			*/
			(UCHAR)USB_INTERFACE_PROTOCOL,			/* bInterfaceProtocol			*/
			(UCHAR)STRIDX_INTERFACE,				/* iInterface					*/
			/* Endpoint	*/
			{
				{
				/* EP1	*/
					(UCHAR)0x07,						/* bLength(7)					*/
					USB_DESC_TYPE_ENDPOINT,				/* bDescriptorType				*/
					(UCHAR)0x81,						/* bEndpointAddress (EP1 IN)	*/
					(UCHAR)0x02,						/* bmAttributes (BULK)			*/
					{
						(UINT8)PACKET_SIZE_FULLSPEED_L,		/* wMaxPacketSize			*/
						(UINT8)PACKET_SIZE_FULLSPEED_H,		/* wMaxPacketSize			*/
					},
					(UCHAR)0x00,						/* bInterval					*/
				},
				/* EP2	*/
				{
					(UCHAR)0x07,						/* bLength(7)					*/
					USB_DESC_TYPE_ENDPOINT,				/* bDescriptorType				*/
					(UCHAR)0x02,						/* bEndpointAddress (EP2 OUT)	*/
					(UCHAR)0x02,						/* bmAttributes (BULK)			*/
					{
						(UINT8)PACKET_SIZE_FULLSPEED_L,		/* wMaxPacketSize			*/
						(UINT8)PACKET_SIZE_FULLSPEED_H,		/* wMaxPacketSize			*/
					},
					(UCHAR)0x00,						/* bInterval					*/
				},
			},
		},
	},
};

/*-----------------------------------------------*/
/* Configuration Descriptor (HS)				 */
/*-----------------------------------------------*/
const DESC_CONFIG UsbFunctionConfigurationDescriptorHS = {
	/* Configuration	*/
	{
		(UCHAR)0x09,							/* bLength(9)							*/
		(UCHAR)USB_DESC_TYPE_CONFIGURATION,		/* bDescriptor(2)						*/
		{
			(UINT8)0x20,							/* wTotalLength(32)					*/
			(UINT8)0x00,							/* wTotalLength						*/
		},
		(UCHAR)0x01,							/* bNumInterfaces						*/
		(UCHAR)0x01,							/* bConfigurationValue					*/
		(UCHAR)STRIDX_CONFIGURATION,			/* iConfiguration						*/
		(UCHAR)0xc0,							/* bmAttributes							*/
		(UCHAR)0x00,							/* MaxPower								*/
	},
	/* Interface	*/
	{
		{
			(UCHAR)0x09,							/* bLength(9)							*/
			(UCHAR)USB_DESC_TYPE_INTERFACE,			/* bDescriptorType						*/
			(UCHAR)0x00,							/* bInterfaceNumber						*/
			(UCHAR)0x00,							/* bAlternateSetting					*/
			(UCHAR)0x02,							/* bNumEndpoints						*/
			(UCHAR)USB_INTERFACE_CLASS,				/* bInterfaceClass						*/
			(UCHAR)USB_INTERFACE_SUB_CLASS,			/* bInterfaceSubClass 					*/
			(UCHAR)USB_INTERFACE_PROTOCOL,			/* bInterfaceProtocol					*/
			(UCHAR)STRIDX_INTERFACE,				/* iInterface							*/
			/* Endpoint	*/
			{
				{
				/* EP1	*/
					(UCHAR)0x07,						/* bLength(7)					*/
					USB_DESC_TYPE_ENDPOINT,				/* bDescriptorType				*/
					(UCHAR)0x81,						/* bEndpointAddress (EP1 IN)	*/
					(UCHAR)0x02,						/* bmAttributes (BULK)			*/
					{
						(UINT8)PACKET_SIZE_HIGHSPEED_L,		/* wMaxPacketSize			*/
						(UINT8)PACKET_SIZE_HIGHSPEED_H,		/* wMaxPacketSize			*/
					},
					(UCHAR)0x00,						/* bInterval					*/
				},
				/* EP2	*/
				{
					(UCHAR)0x07,						/* bLength(7)					*/
					USB_DESC_TYPE_ENDPOINT,				/* bDescriptorType				*/
					(UCHAR)0x02,						/* bEndpointAddress (EP2 OUT)	*/
					(UCHAR)0x02,						/* bmAttributes (BULK)			*/
					{
						(UINT8)PACKET_SIZE_HIGHSPEED_L,		/* wMaxPacketSize			*/
						(UINT8)PACKET_SIZE_HIGHSPEED_H,		/* wMaxPacketSize			*/
					},
					(UCHAR)0x00,						/* bInterval					*/
				},
			},
		},
	},
};

/****************************************************************************************************/
/*																									*/
/*		GET_DESCRIPTOR String にて返信するStringディスクリプタ定義									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		言語ＩＤ																					*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR LanguageIdStringDescriptor[]=
{
4,				/* このﾃﾞｨｽｸﾘﾌﾟﾀのｻｲｽﾞ(byte)														*/
0x03,			/* ﾃﾞｨｽｸﾘﾌﾟﾀのﾀｲﾌﾟ(0x03固定)														*/
0x09,0x04		/* 0x0409 : ID=米語																	*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		製造者																						*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR ImanufacturerStringDescriptor[]=
{
58,		/* このﾃﾞｨｽｸﾘﾌﾟﾀのｻｲｽﾞ(byte) */
0x03,	/* ﾃﾞｨｽｸﾘﾌﾟﾀのﾀｲﾌﾟ(0x03固定) */
/* 文字列 */
/* YASKAWA ELECTRIC CORPORATION */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'Y', 0 ,'A', 0 ,'S', 0 ,'K', 0 ,'A', 0 ,'W', 0 ,'A', 0 ,' ', 0 ,
	'E', 0 ,'L', 0 ,'E', 0 ,'C', 0 ,'T', 0 ,'R', 0 ,'I', 0 ,'C', 0 ,
	' ', 0 ,'C', 0 ,'O', 0 ,'R', 0 ,'P', 0 ,'O', 0 ,'R', 0 ,'A', 0 ,
	'T', 0 ,'I', 0 ,'O', 0 ,'N', 0
};

/*--------------------------------------------------------------------------------------------------*/
/*		製品																						*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IproductStringDescriptor[]=
{
42,		/* このﾃﾞｨｽｸﾘﾌﾟﾀのｻｲｽﾞ(byte) */
0x03,	/* ﾃﾞｨｽｸﾘﾌﾟﾀのﾀｲﾌﾟ(0x03固定) */
/* 文字列 */
/* YASKAWA SIGMA SERIES */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'Y', 0 ,'A', 0 ,'S', 0 ,'K', 0 ,'A', 0 ,'W', 0 ,'A', 0 ,' ', 0 ,
	'S', 0 ,'I', 0 ,'G', 0 ,'M', 0 ,'A', 0 ,' ', 0 ,'S', 0 ,'E', 0 ,
	'R', 0 ,'I', 0 ,'E', 0 ,'S', 0
};
/*--------------------------------------------------------------------------------------------------*/
/*		シリアル番号																				*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IserialNumberStringDescriptor[]=
{
34,		/* このﾃﾞｨｽｸﾘﾌﾟﾀのｻｲｽﾞ(byte) */
0x03,	/* ﾃﾞｨｽｸﾘﾌﾟﾀのﾀｲﾌﾟ(0x03固定) */
/* 文字列 */
/* 0000000000000000 (16ｷｬﾗｸﾀ) */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,
	'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0
};
/*--------------------------------------------------------------------------------------------------*/
/*		コンフィグレーション																		*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IconfigurationStringDescriptor[]=
{
54,	/* このﾃﾞｨｽｸﾘﾌﾟﾀのｻｲｽﾞ (byte) */
0x03,	/* ﾃﾞｨｽｸﾘﾌﾟﾀのﾀｲﾌﾟ(0x03固定) */
/* 文字列 */
/* Sigma ACservo Basic Config */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'S', 0 ,'i', 0 ,'g', 0 ,'m', 0 ,'a', 0 ,' ', 0 ,'A', 0 ,'C', 0 ,
	's', 0 ,'e', 0 ,'r', 0 ,'v', 0 ,'o', 0 ,' ', 0 ,'B', 0 ,'a', 0 ,
	's', 0 ,'i', 0 ,'c', 0 ,' ', 0 ,'C', 0 ,'o', 0 ,'n', 0 ,'f', 0 ,
	'i', 0 ,'g', 0
};
/*--------------------------------------------------------------------------------------------------*/
/*		インターフェイス																			*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IinterfaceStringDescriptor[]=
{
24,		/* このﾃﾞｨｽｸﾘﾌﾟﾀのｻｲｽﾞ(byte) */
0x03,	/* ﾃﾞｨｽｸﾘﾌﾟﾀのﾀｲﾌﾟ(0x03固定) */
/* 文字列 */
/* 2 Endpoints */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'2', 0 ,' ', 0 ,'E', 0 ,'n', 0 ,'d', 0 ,'p', 0 ,'o', 0 ,'i', 0 ,
	'n', 0 ,'t', 0 ,'s', 0
};

/*--------------------------------------------------------------------------------------------------*/
/*		USBファンクション用割り込み処理登録テーブル													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /*<S146> 割り込み設定はしなくなったため削除 */
/*	FIXME:割込レベルはINTC_PRL_LVL7で良いか？ */
const tINTC_REGIST	intc_regist_table_u2f[2] =
{
	{INTC_IDX_U2F_INT,		UsbFuncIntIntu2f,		INTC_LEVEL_HI,	INTC_PRL_LVL7, 0},	/* INTU2F 割り込み処理用	*/
	{INTC_IDX_U2F_EPC_INT,	UsbFuncIntIntu2fepc,	INTC_LEVEL_HI,	INTC_PRL_LVL7, 0}	/* INTU2FEPC 割り込み処理	*/
};
#endif

/*===========================================================================*/
/* PROTOTYPE																 */
/*===========================================================================*/
/* APIs	*/

/* Locals	*/
static	ER_RET	VendorRequest_Callback( DEVICE_REQUEST* pDeviceRequest );	/* ベンダーリクエストコールバック処理	*/
static	void	UsbFuncInitializeInfo( U2F_INFO *pUsbfInfo );				/* USBファンクション情報初期化処理		*/
static	void	UsbFuncClearFlag( USB_FLAG *pUsbfFlag );					/* フラグ・クリア処理					*/
static	void	UsbFuncSetMaxpacket ( void );								/* Max Packet Size 設定処理				*/
static	USB_FUNCTION_IF*	UsbFuncGetInfoAddress( void );					/* USB情報領域のアドレス取得処理		*/
static	UINT	UsbFuncCheckPlugInout( void );								/* USB接続状態確認処理					*/
static	UINT	UsbFuncCheckSetupReceive( void );							/* SETUPトークン受信確認処理			*/

/* EPn転送関連	*/
static	void	UsbFuncSetStallEPn( UINT num );								/* EPn ストール応答処理					*/
static	void	UsbFuncOutDataSplitCopy( USB_FUNCTION_IF *pUsbfIf );		/* OUTデータ分割コピー処理					*/ /*<S182>*/
static	void	UsbFuncInDataSplitCopy( USB_FUNCTION_IF *pUsbfIf );			/* INデータ分割コピー処理					*/ /*<S182>*/

/* コントロール転送関連	*/
static	INT		UsbFuncWriteEP0( VUCHAR* pBuf, INT size );					/* EP0 送信処理							*/
static	void	UsbFuncSetStallEP0( void );									/* EP0 ストール応答処理					*/
#if 0	/* 現時点では使用しないため <S150>*/
static	void	UsbFuncSetNakEP0( void );									/* EP0 NAK応答処理						*/
static	void	UsbFuncClearNakEP0( void );									/* EP0 NAK解除処理						*/
static	ER_RET	UsbFuncSendNullEP0( void );									/* EP0 NULL送信処理						*/
#endif

/*-------------------------------------*/
/* リクエスト・タイプ別処理			   */
/*-------------------------------------*/
static	ER_RET	control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 No Data処理			*/
static	ER_RET	control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 Control Read処理		*/
static	ER_RET	control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 Control Write処理	*/

/*-------------------------------------*/
/* スタンダード・リクエスト処理		   */
/*-------------------------------------*/
static	ER_RET	decode_request( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 リクエストデコード処理 */

static	ER_RET	bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 スタンダードリクエスト処理 */
/* --- CLEAR_FEATURE     ( 1) [No data] --- */
static	ER_RET	std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest );								/* EP0 CLEAR_FEATURE処理 */
static	ER_RET	Clear_Feature_EP( UINT num );															/* EP0 CLEAR_FEATURE処理( TARGET = EP ) */
/* --- GET_CONFIGURATION ( 8) [Read]    --- */
static	ER_RET	std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_CONFIGURATION処理 */
/* --- GET_DESCRIPTOR    ( 6) [Read]    --- */
static	ER_RET	std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR処理 */
static	ER_RET	Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR DEVICE処理 */
static	ER_RET	Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 GET_DESCRIPTOR CONFIGURATION処理 */
static	ER_RET	Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR STRING処理 */
static	ER_RET	Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR DEVICE QUALIFIER処理 */
static	ER_RET	Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION処理 */
/* --- GET_INTERFACE     (10) [Read]    --- */
static	ER_RET	std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_INTERFACE処理 */
/* --- GET_STATUS        ( 0) [Read]    --- */
static	ER_RET	std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS処理 */
static	ER_RET	Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS DEVICE処理 */
static	ER_RET	Get_Status_EP( DEVICE_REQUEST* pDeviceRequest );										/* EP0 GET_STATUS ENDPOINT処理 */
/* --- SET_ADDRESS       ( 5) [No data] --- */
static	ER_RET	std_req_set_address( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SET_ADDRESS処理 */
/* --- SET_CONFIGURATION ( 9) [No data] --- */
static	ER_RET	std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 SET_CONFIGURATION処理 */
/* --- SET_DESCRIPTOR    ( 7) [Write]   --- */
static	ER_RET	std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest );								/* EP0 SET_DESCRIPTOR処理 */
/* --- SET_FEATURE       ( 3) [No data] --- */
static	ER_RET	std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 SET_FEATURE処理 */
static	ER_RET	Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo );				/* EP0 SET_FEATURE処理( TARGET = DEVICE ) */
static	ER_RET	Set_Feature_EP( UINT num );																/* EP0 SET_FEATURE処理( TARGET = EP ) */
/* --- SET_INTERFACE     (11) [No data] --- */
static	ER_RET	std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 SET_INTERFACE処理 */
/* --- SYNC_FRAME        (12) [No data] --- */
static	ER_RET	std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SYNCH_FRAME処理 */

static	ER_RET	Check_USB_DIR_EPn( UINT num, UINT8 direction );											/* EP方向のUSB_DIR_OUT/IN変換処理 */
static	UINT32	udev_get_DeviceState( void );															/* デバイスステート取得処理 */
static	DESC_CONFIG*			info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo );							/* info カレントコンフィギュレーションデスクリプタの取得処理 */
static	INTERFACE_DESCRIPTOR*	info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo );			/* info インターフェースデスクリプタの取得処理 */
static	INT						EPn_get_STALL( UINT num );												/* EPn STALL状態取得処理 */


/*===========================================================================*/
/* PROGRAM																	 */
/*===========================================================================*/

/***************************************************************************
  Description: USBドライバ初期化処理
----------------------------------------------------------------------------
  Parameters:
	MyAddress;			自局アドレス
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
void	UsbFuncInitializeDriver( ULONG MyAddress )
{
	UINT32			state;					/* 状態操作用					*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ			*/
	USB_FLAG		*pUsbfFlag;				/* USBフラグ領域ポインタ		*/
//	INT				i;						/* ループカウンタ				*//*<S0C7>*/
	ULONG			StartTime;				/* タイマ用ワーク				*//*<S138>*/

	UsbFunctionAvailable = FALSE;									/* USB使用不可設定(Rev.A基板対応) *//*<S138>*/

	pUsbfIf = UsbFuncGetInfoAddress();								/* USB情報領域のアドレス取得	*/

	MlibResetLongMemory( pUsbfIf, sizeof( USB_FUNCTION_IF )/4  );	/* 管理メモリクリア				*/

	pUsbfIf->UsbfInfo.MyAddress = MyAddress;						/* 自局アドレス設定				*/

	pUsbfFlag = &(pUsbfIf->UsbfFlag);
	UsbFuncClearFlag( &(pUsbfIf->UsbfFlag) );						/* フラグクリア					*/

	UsbFuncInitializeInfo( &(pUsbfIf->UsbfInfo) );					/* クラス定義初期化				*/

	/*----------------------------------------------------------------------------------------------*/
	/*		EPC, PLL リセット解除																	*/
	/*----------------------------------------------------------------------------------------------*/
	ARMPF_EPCTR &= ~(D_SYS_EPC_RST | D_SYS_PLL_RST);				/* リセット解除 */

	/*----------------------------------------------------------------------------------------------*/
	/*		System Bus-EPC Bridge 初期設定															*/
	/*----------------------------------------------------------------------------------------------*/
	ARMPF_SYSSCTR   = D_SYS_WAIT_MODE;								/* スレーブのウエイト動作指定	*/
#if 1	/* XXX:DMA転送使用時は有効にする */ /* <S026> 初期化指示ありのため有効化 */
	ARMPF_SYSMCTR   = 0x00000034;									/* DMA転送時のアービトレーション(bit 31)：ラウンドロビン */
#endif
#if 0	/* <S150> VBUS割り込みを使用しない(ポーリング化) */
	ARMPF_SYSBINTEN = D_SYS_VBUS_INT;								/* VBUS割り込み許可						*/
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		EPC 初期設定																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* --- PLL_LOCK待ち --- */
	KlibRstLongTimer(&StartTime);		/* 計時開始 *//*<S138>*/
	while ((ARMPF_EPCTR & D_SYS_PLL_LOCK) == 0)
	{
		/* !!!!!!!! ToDo PLL LOCK待ち異常時処理 !!!!!!!! */
// <S138> >>>>>
		if( KlibGetLongTimerUs(StartTime) > 2000 )
		{
			return;
		}
		KlibWaitus(100);
// <<<<< <S138>
	}

	UsbFunctionAvailable = TRUE;							/* USB使用可能設定(Rev.A基板対応) *//*<S138>*/

	/* --- 割り込み設定 --- */
	/* USB制御レジスタ	*/
#if 1	/* XXX:High Speed 利用時は、こちらを有効にする */
	ARMPF_USB_CONTROL   = D_USB_INT_SEL | D_USB_SOF_RCV;			/* INTU2FEPC割り込みをレベル割り込みに設定	*/
															/* SOF 受信エラー発生時に自動リカバリ機能を使用	*/
#else
	ARMPF_USB_CONTROL   = D_USB_INT_SEL | D_USB_SOF_RCV | D_USB_CONSTFS;	/* 上記 ＋ FULL Speed固定	*/
#endif
	/* USB割り込み許可レジスタ	*/
#if 0	/* <S150> 割り込みを使用せず、ポーリングに変更 */
	ARMPF_USB_INT_ENA   = D_USB_USB_RST_EN    |					/* 割り込み許可：バス・リセット			*/
					D_USB_SPEED_MODE_EN |					/*             ：バス・スピード変更		*/
					D_USB_EP0_EN        |					/*             ：EP0 割り込み			*/
					D_USB_EP1_EN        |					/*             ：EP1 割り込み			*/
					D_USB_EP2_EN;							/*             ：EP2 割り込み			*/
#endif

	/* --- Max Packet設定 --- */
	UsbFuncSetMaxpacket();

	/* --- 各EP バッファ・クリア --- */
	/* EPn制御レジスタ	*/
	ARMPF_EP0_CONTROL   = D_EP0_BCLR;								/* EP0 : バッファ・クリア				*/
	ARMPF_EP1_CONTROL   = D_EPn_BCLR;								/* EP1 : Bulk-in,  バッファ・クリア		*/
	ARMPF_EP2_CONTROL   = D_EPn_BCLR | D_EPn_DIR0;				/* EP2 : Bulk-out, バッファ・クリア		*/

	/* --- 各EP 割り込み許可 --- */
	/* EPn割り込み許可レジスタ	*/
#if 0	/* <S150> 割り込みを使用せず、ポーリングに変更	*/
	ARMPF_EP0_INT_ENA   = D_EP0_SETUP_EN;							/* EP0 : 割り込み許可：SETUP_INT					*/
#endif
#if 0	/* XXX:転送完了割り込みを利用する場合に有効にする（有効にする項目は再検討が必要） */
	ARMPF_EP1_INT_ENA   = D_EPn_IN_EN | D_EPn_IN_END_EN;			/* EP1 : 割り込み許可：EP1_IN_INT, EP1_IN_END_INT	*/
															/*	D_EPn_IN_EN		: IN転送完了割り込み許可		*/
															/*	D_EPn_IN_END_EN	: IN DMA転送完了割り込み許可	*/
	ARMPF_EP2_INT_ENA   = D_EPn_OUT_EN | D_EPn_OUT_END_EN;		/* EP2 : 割り込み許可：EP2_OUT_INT, EP2_OUT_END_INT	*/
															/*	D_EPn_OUT_EN	: OUT転送完了割り込み許可		*/
															/*	D_EPn_OUT_END_EN: OUT DMA転送完了割り込み許可	*/
#endif

	/* --- 各EP 有効化 --- */
	ARMPF_EP1_CONTROL  |= D_EPn_EN;								/* EP1 : 有効化		*/
	ARMPF_EP2_CONTROL  |= D_EPn_EN;								/* EP2 : 有効化		*/

	/*=========================================*/
	/* プラグイン／アウト処理 (起動時状態確認) */
	/*=========================================*/
	if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL)		/* VBUSレベル確認	*/
	{
		/* ====== プラグイン処理 ======	*/
		state       =  ARMPF_USB_CONTROL;							/* USB制御レジスタ値取得						*/
		state      &= ~D_USB_CONNECTB;						/* （Function PHY へのUSB 信号を有効にする）	*/
		state      |=  D_USB_PUE2;							/* （D+信号をPull-up する）						*/
		ARMPF_USB_CONTROL =  state;								/* 上記操作を同時に反映							*/
		pUsbfFlag->UsbPlugStatus = U2F_PLUG_IN;				/* プラグ状態をプラグインに更新					*/
	}
	else
	{
		/* ====== プラグアウト処理 ====== */
		state       =  ARMPF_USB_CONTROL;							/* USB制御レジスタ値取得						*/
		state      |=  D_USB_CONNECTB;						/* （Function PHY へのUSB 信号を無効にする）	*/
		state      &= ~D_USB_PUE2;							/* （D+信号をPull-up しない）					*/
		ARMPF_USB_CONTROL =  state;								/* 上記操作を同時に反映							*/
		pUsbfFlag->UsbPlugStatus = U2F_PLUG_OUT;			/* プラグ状態をプラグアウトに更新				*/
	}

#if 0	/* Interrupt.cでまとめて設定するため不要	*/
	/*=========================================*/
	/* 割り込み設定							   */
	/*=========================================*/
	for( i = 0; i < (INT)(sizeof(intc_regist_table_u2f)/sizeof(tINTC_REGIST)); i++ )
	{
		intc_set_handler((tINTC_REGIST*)&intc_regist_table_u2f[i]);
		intc_enable_interrupt(intc_regist_table_u2f[i].index);
	}
#endif

		return;
}

//<S138> >>>>>
/***************************************************************************
  Description: USB使用可能判定処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	TRUE;			使用可能
	FALSE;			使用禁止
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
BOOL	UsbFuncChkAvailable( void )
{
	return UsbFunctionAvailable;
}
//<<<<< <S138>

/***************************************************************************
	APIs	割り込み関連
****************************************************************************/

/***************************************************************************
  Description: INTU2F 割り込み処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
void UsbFuncIntIntu2f( void )
{
	UINT32			state;					/* 状態操作用				*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ		*/
	USB_FLAG		*pUsbfFlag;				/* USBフラグ領域ポインタ	*/

//<S138> >>>>>
	/* Rev.A基板対応 */
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}
//<<<<< <S138>

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB情報領域のアドレス取得	*/
	pUsbfFlag = &(pUsbfIf->UsbfFlag);

	/*=========================================*/
	/* プラグイン／アウト処理 (VBUS)		   */
	/*=========================================*/
	if ((ARMPF_SYSBINT & D_SYS_VBUS_INT) == D_SYS_VBUS_INT)
	{
		ARMPF_SYSBINT |= D_SYS_VBUS_INT;								/* 割り込み要因クリア	*/
		if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL)		/* VBUSレベル確認		*/
		{
			/* ====== プラグイン処理 ====== */
			state       =  ARMPF_USB_CONTROL;							/* USB制御レジスタ値取得						*/
			state      &= ~D_USB_CONNECTB;						/* （Function PHY へのUSB 信号を有効にする）	*/
			state      |=  D_USB_PUE2;							/* （D+信号をPull-up する）						*/
			ARMPF_USB_CONTROL =  state;								/* 上記操作を同時に反映							*/
			pUsbfFlag->UsbPlugStatus = U2F_PLUG_IN;				/* プラグ状態をプラグインに更新					*/
		}
		else
		{
			/* ====== プラグアウト処理 ====== */
			state       =  ARMPF_USB_CONTROL;							/* USB制御レジスタ値取得						*/
			state      |=  D_USB_CONNECTB;						/* （Function PHY へのUSB 信号を無効にする）	*/
			state      &= ~D_USB_PUE2;							/* （D+信号をPull-up しない）					*/
			state      &= ~D_USB_DEFAULT;						/* （エンドポイント0を無効にする）				*/
			state      &= ~D_USB_CONF;							/* （エンドポイント0以外を無効にする）			*/
			ARMPF_USB_CONTROL =  state;								/* 上記操作を同時に反映							*/
			pUsbfFlag->UsbPlugStatus = U2F_PLUG_OUT;			/* プラグ状態をプラグアウトに更新				*/

			UsbFuncClearFlag( pUsbfFlag );						/* フラグ・クリア								*/
		}
	}

#if 0	/* XXX:サンプルソースでコメントとして存在 */
	intc_clear_eiint( IDX_INTU2F );								/* INTU2F 要因クリア	*/
#endif
}

/***************************************************************************
  Description: INTU2FEPC 割り込み処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
void UsbFuncIntIntu2fepc ( void )
{
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ		*/
	USB_FLAG		*pUsbfFlag;				/* USBフラグ領域ポインタ	*/

//<S138> >>>>>
	/* Rev.A基板対応 */
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}
//<<<<< <S138>

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB情報領域のアドレス取得	*/
	pUsbfFlag = &(pUsbfIf->UsbfFlag);

	/*=========================================*/
	/* バス・リセット 処理					   */
	/*=========================================*/
	if ((ARMPF_USB_INT_STA & D_USB_USB_RST_INT) == D_USB_USB_RST_INT)
	{
		ARMPF_USB_INT_STA = ~D_USB_USB_RST_INT;						/* 割り込み要因クリア		*/
		/* --- Max Packet設定 --- */
		UsbFuncSetMaxpacket();
		/* --- EP バッファ・クリア --- */
		ARMPF_EP0_CONTROL   |= D_EP0_BCLR;							/* EP0 : バッファ・クリア	*/
		ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							/* EP1 : バッファ・クリア	*/
		ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							/* EP2 : バッファ・クリア	*/
		/* --- フラグ・クリア --- */
		UsbFuncClearFlag( pUsbfFlag );							/* フラグ・クリア			*/
	}

	/*=========================================*/
	/* USBバス・スピード変更 処理			   */
	/*=========================================*/
	if ((ARMPF_USB_INT_STA & D_USB_SPEED_MODE_INT) == D_USB_SPEED_MODE_INT)
	{
		ARMPF_USB_INT_STA = ~D_USB_SPEED_MODE_INT;					/* 割り込み要因クリア		*/
		/* --- Max Packet設定 --- */
		UsbFuncSetMaxpacket();
	}

	/*=========================================*/
	/* EP0 処理								   */
	/*=========================================*/
	/* ---  SETUPデータ受信 --- */
	if ((ARMPF_EP0_STATUS & D_EP0_SETUP_INT) == D_EP0_SETUP_INT)
	{
		ARMPF_EP0_STATUS = ~D_EP0_SETUP_INT;							/* 割り込み要因クリア		*/
		pUsbfFlag->UsbSetupReceive++;
	}
#if 0	/* Bulkデータ転送で割り込みは使用していないため */
	/*=========================================*/
	/* EP1 処理								   */
	/*=========================================*/
	/* ---  Bulk-In データ送信 --- */
	if ((ARMPF_EP1_STATUS & D_EPn_IN_INT) == D_EPn_IN_INT) {
		ARMPF_EP1_STATUS = ~D_EPn_IN_INT;								/* 割り込み要因クリア		*/
		pUsbfFlag->UsbEp1InCount++;
	}
	/* ---  Bulk-In DMA転送終了 --- */
	if ((ARMPF_EP1_STATUS & D_EPn_IN_END_INT) == D_EPn_IN_END_INT) {
		ARMPF_EP1_STATUS = ~D_EPn_IN_END_INT;							/* 割り込み要因クリア		*/
	}
	/*=========================================*/
	/* EP2 処理								   */
	/*=========================================*/
	/* ---  Bulk-Out データ受信 --- */
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_INT) == D_EPn_OUT_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_INT;							/* 割り込み要因クリア		*/
		pUsbfFlag->UsbEp2OutCount++;
	}
	/* ---  Bulk-Out DMA転送終了 --- */
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_END_INT) == D_EPn_OUT_END_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_END_INT;						/* 割り込み要因クリア		*/
	}
#endif

#if 0	/* XXX:サンプルソースでコメントとして存在 */
	intc_clear_eiint( IDX_INTU2FEPC );							/* INTU2FEPC 要因クリア	*/
#endif
}


/***************************************************************************
	APIs	コントロール転送関連
****************************************************************************/

/***************************************************************************
  Description: コントロール転送タスク処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
void UsbFuncTaskControl( void )
{
	ER_RET			result;					/* 結果							*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ			*/
	U2F_INFO		*pUsbfInfo;				/* USBデバイス情報領域ポインタ	*/
	DEVICE_REQUEST	*pDeviceRequest;		/* デバイスリクエストポインタ	*/

//<S138> >>>>>
	/* USB使用不可の場合は即終了(Rev.A基板対応) */
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}
//<<<<< <S138>

	/* USB情報領域アドレス取得	*/
	pUsbfIf = UsbFuncGetInfoAddress();
	/* USBデバイス情報領域アドレス取得	*/
	pUsbfInfo = &(pUsbfIf->UsbfInfo);
	/* デバイスリクエストアドレス取得	*/
	pDeviceRequest = &(pUsbfIf->UsbfInfo.usb.request);

	/*=========================================*/
	/* EP0 処理 (SETUPデータ受信)			   */
	/*=========================================*/
	if (pUsbfIf->UsbfFlag.UsbSetupReceive != pUsbfIf->UsbfFlag.UsbSetupSave)
	{
		/* 次回比較用に保存する */
		pUsbfIf->UsbfFlag.UsbSetupSave = pUsbfIf->UsbfFlag.UsbSetupReceive;
		/* ------ デバイス・リクエスト リード ------ */
		pDeviceRequest->bmRequestType = (UINT8) ((ARMPF_SETUP_DATA0 >>  0) & 0xff  );
		pDeviceRequest->bRequest      = (UINT8) ((ARMPF_SETUP_DATA0 >>  8) & 0xff  );
		pDeviceRequest->wValue        = (UINT16)((ARMPF_SETUP_DATA0 >> 16) & 0xffff);
		pDeviceRequest->wIndex        = (UINT16)((ARMPF_SETUP_DATA1 >>  0) & 0xffff);
		pDeviceRequest->wLength       = (UINT16)((ARMPF_SETUP_DATA1 >> 16) & 0xffff);

		/* ------ リクエスト・タイプ判別 ------ */
		if( pDeviceRequest->wLength == 0x0000 )
		{	/* データ・ステージのバイト数が0のとき		*/
			/*  -> STATUS処理へ				*/
			result = control_nodata( pDeviceRequest, pUsbfInfo );
		}
		else
		{	/* データ・ステージのバイト数が0ではなく，	*/
			if( (pDeviceRequest->bmRequestType & USB_DIR_IN) == USB_DIR_IN )
			{	/* データ転送方向が Host=>Dev のとき，	*/
				/*  -> DATA IN & STATUS処理へ	*/
				result = control_read( pDeviceRequest, pUsbfInfo );
			}
			else
			{	/* Dev=>Host のとき，					*/
				/*  -> DATA OUT & STATUS処理へ	*/
				result = control_write( pDeviceRequest, pUsbfInfo );
			}
		}
		/* ------ エラー処理 --- */
		if (result != ER_OK)
		{
			/* ToDo:コントロール転送でエラー時の共通処理があればここに記述する */
		}
	}
}

/***************************************************************************
	APIs	バルク転送関連
****************************************************************************/

/***************************************************************************
  Description: USB バルク転送制御処理（Scan処理による割込み要因のポーリング処理）
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	【機能】
		各割り込み状態を定周期に監視して割込み要因発生時に処理を行う。
			監視対象割り込み：記載順番どおりにチェックしていく
				(1) OUTデータ受信完了(ARMPF_EP2_STATUS & D_EPn_OUT_INT)
				(2) OUTデータNULLパケット受信完了(ARMPF_EP2_STATUS & D_EPn_OUT_NULL_INT)
				(3) INトークンにNAK応答(ARMPF_EP1_STATUS & D_EPn_IN_NAK_ERR_INT)
	【注意】(SGDV用のコメント　とりあえず変更なしで残している)
		この関数は、通信マネージャと同等又は上位タスクにて実行させること。
		通信マネージャと同じタスクで実行させる場合通信マネージャの前で実行させると効率が良い。
----------------------------------------------------------------------------
	【制御対象】
		このモジュールで制御するエンドポイントは以下の通り。
			Bulk out: Endpoint2
			Bulk in : Endpoint1
	【送受信開始処理(参考)】
		通信マネージャから転送開始指令を以下の関数で受ける。
			Bulk out: UsbFuncStartBkout()
			Bulk in : UsbFuncStartBkin()
		開始指令時にCPUのUSBFをチェックしてドライバステータスを以下のように設定する。
			Bulk out: 受信可能 -> UsbfIf.Ep1Status = BLKTRNS_ENABLE
					  受信不可 -> UsbfIf.Ep1Status = BLKTRNS_DISABLE
			Bulk in : 送信可能 -> UsbfIf.Ep2Status = BLKTRNS_ENABLE
					  送信不可 -> UsbfIf.Ep2Status = BLKTRNS_DISABLE

****************************************************************************/
/*<S182> データコピーを複数スキャンに分割するように変更（変更部多いため関数内に履歴なし）  */
void	UsbFuncPollBulkOutIn( void )
{
	UINT32			MaxPacketSize;				/* 最大パケットサイズ			*/
	USB_FUNCTION_IF	*pUsbfIf;					/* USB情報領域ポインタ			*/

	pUsbfIf = UsbFuncGetInfoAddress();			/* USB情報領域のアドレス取得	*/

	/*----------------------------------------------------------------------------------------------*/
	/*	バルク転送割込み要因フラグのチェック														*/
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*	注意！！																					*/
	/*																								*/
	/*	：本処理をScanCなどの低速定周期タスクでポーリングする場合はOUTデータがダブルバッファ両面	*/
	/*	  に溜まるケースが発生するようになり、以下の変更が必要となります！！						*/
	/*																								*/
	/*	１．EP2受信中にINが来た場合もOUTデータをすべて読みだしたことを確認してから受信終了とする	*/
	/*----------------------------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------------------------*/
	/*	詳細要因チェック(1-1)	: OUTデータ受信完了												*/
	/*------------------------------------------------------------------------------------------*/
	if( (pUsbfIf->OutSplitCopy != TRUE) && (pUsbfIf->InSplitCopy != TRUE) )
	if( (ARMPF_EP2_STATUS & D_EPn_OUT_INT) == D_EPn_OUT_INT )	/* OUTデータ受信完了			*/
	{
		ARMPF_EP2_STATUS &= ~D_EPn_OUT_INT;						/* 割込み要因クリア				*/
		if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )				/* 受信許可チェック				*/
		{
																/* 受信長保存					*/
			pUsbfIf->OutPacketSize = ARMPF_EP2_LEN_DCNT & D_EPn_LDATA;
			pUsbfIf->OutCopyRem = pUsbfIf->OutPacketSize;		/* コピー残サイズ保存			*/
			/*------------------------------------------------------------------------------*/
			/*	受信サイズオーバーチェック													*/
			/*------------------------------------------------------------------------------*/
			if( (pUsbfIf->OutLength + pUsbfIf->OutPacketSize) > pUsbfIf->OutLengthReq )
			{
				/*--------------------------------------------------------------------------*/
				/*	今回のﾃﾞｰﾀを受信したら受信ﾊﾞｯﾌｧのｻｲｽﾞをｵｰﾊﾞするためデータ破棄して完了	*/
				/*	アプリケーションレベルでエラー処理を実行してもらう。					*/
				/*--------------------------------------------------------------------------*/
				ARMPF_EP2_CONTROL |= D_EPn_ONAK;				/* EP2をNAK応答のみに設定		*/
				pUsbfIf->Ep2Status = BLKTRNS_STALL;				/* STALL応答したことをセット	*/
			}
			else
			{
				/*--------------------------------------------------------------------------*/
				/*	オーバしない場合、分割受信する											*/
				/*--------------------------------------------------------------------------*/
				UsbFuncOutDataSplitCopy( pUsbfIf );
			}
		}
	}
	/*------------------------------------------------------------------------------------------*/
	/*	詳細要因チェック(1-2)	: OUTデータ分割コピー中											*/
	/*------------------------------------------------------------------------------------------*/
	if( pUsbfIf->OutSplitCopy == TRUE )
	{
		if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )
		{
			/*--------------------------------------------------------------------------*/
			/*	分割受信の続きを実施する												*/
			/*--------------------------------------------------------------------------*/
			UsbFuncOutDataSplitCopy( pUsbfIf );
		}
	}

	/*--------------------------------------------------------------------------------------*/
	/*	詳細要因チェック(2)	: OUTデータNULLパケット受信完了									*/
	/*		OUTデータがマックス・パケットサイズの整数倍の時、								*/
	/*		データ終了認識のため最後にNULLを出すホストのため								*/
	/*		NULLを監視しデータ受信中にNULLが来たら受信完了とする							*/
	/*--------------------------------------------------------------------------------------*/
	if( pUsbfIf->OutSplitCopy != TRUE )
	if( (ARMPF_EP2_STATUS & D_EPn_OUT_NULL_INT) == D_EPn_OUT_NULL_INT )
	{
		ARMPF_EP2_STATUS &= ~D_EPn_OUT_NULL_INT;		/* BKO1NL割込み要因クリア			*/
		if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )		/* 受信許可中？						*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;			/* EP2をNAK応答のみに設定			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;			/* 受信完了をセット					*/
		}
	}

	/*--------------------------------------------------------------------------------------*/
	/*	詳細要因チェック(3-1)	: INトークンにNAK応答										*/
	/*		通常OUTデータ後、INトークンが来る。												*/
	/*		但しOUTデータがマックス・パケットサイズの整数倍の場合、ホストに					*/
	/*		よってはOUTデータの最後にNULLを出さず、いきなりINトークンを出す。				*/
	/*		従って、Endpoint1だけではなくEndpoint2の状態もチェックしOUTデータ受信待機中に	*/
	/*		INが来たらOUTデータ受信完了とINトークン受信処理を並行して行う。					*/
	/*--------------------------------------------------------------------------------------*/
	if( (pUsbfIf->OutSplitCopy != TRUE) && (pUsbfIf->InSplitCopy != TRUE) )
	if( (ARMPF_EP1_STATUS & D_EPn_IN_NAK_ERR_INT) == D_EPn_IN_NAK_ERR_INT )
	{
		ARMPF_EP1_STATUS &= ~D_EPn_IN_NAK_ERR_INT;		/* フラグクリア						*/
		if( pUsbfIf->Ep1Status == BLKTRNS_ENABLE )		/* 送信許可チェック					*/
		{
			/*------------------------------------------------------------------------------*/
			/*	送信データ書込み許可チェック												*/
			/*------------------------------------------------------------------------------*/
			if( (ARMPF_EP1_STATUS & D_EPn_IN_EMPTY) == D_EPn_IN_EMPTY )	/* EP1のIN EMPTYの場合書込みOK		*/
			{
				/* 自動データ送信無効 */
				ARMPF_EP1_CONTROL &= ~D_EPn_AUTO;

				/* EP1の最大パケットサイズ取得 */
				MaxPacketSize = ARMPF_EP1_PCKT_ADRS & D_EPn_MPKT;

				/* 今回パケットサイズを設定 */
				if( pUsbfIf->InLengthRem > MaxPacketSize )
				{
					pUsbfIf->InCopyRem = MaxPacketSize;
				}
				else
				{
					pUsbfIf->InCopyRem = pUsbfIf->InLengthRem;
				}

				/*--------------------------------------------------------------------------*/
				/*	分割送信開始															*/
				/*--------------------------------------------------------------------------*/
				UsbFuncInDataSplitCopy( pUsbfIf );
			}
		}
		else if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )	/* EP2受信中にINが来たら受信完了	*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;			/* EP2をNAK応答のみに設定			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;			/* 受信完了をセット					*/
		}
	}

	/*------------------------------------------------------------------------------------------*/
	/*	詳細要因チェック(3-2)	: INデータ分割コピー中											*/
	/*------------------------------------------------------------------------------------------*/
	if( pUsbfIf->InSplitCopy == TRUE )
	{
		if( pUsbfIf->Ep1Status == BLKTRNS_ENABLE )		/* 送信許可チェック					*/
		{
			/*--------------------------------------------------------------------------*/
			/*	分割受信の続きを実施する												*/
			/*--------------------------------------------------------------------------*/
			UsbFuncInDataSplitCopy( pUsbfIf );
		}
		else if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )	/* EP2受信中にINが来たら受信完了	*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;			/* EP2をNAK応答のみに設定			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;			/* 受信完了をセット					*/
		}
	}
	return;
}


/***************************************************************************
  Description: USB Endpoint1/2 送受信バッファクリア処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
void	UsbFuncResetBuffer( void )
{
	USB_FUNCTION_IF	*pUsbfIf;						/* USB情報領域ポインタ			*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB情報領域のアドレス取得	*/

	ARMPF_EP2_CONTROL |= D_EPn_ONAK;						/* Endpoint2をNAKのみの応答にする			*/
	pUsbfIf->Ep1Status = BLKTRNS_DISABLE;			/* EP1の状態を送信禁止に設定				*/
	pUsbfIf->Ep2Status = BLKTRNS_DISABLE;			/* EP2の状態を送信禁止に設定				*/
	/*------------------------------------------------------------------------------------------*/
	/*	受信(OUT) バッファのクリア																*/
	/*------------------------------------------------------------------------------------------*/
	ARMPF_EP2_CONTROL |= D_EPn_BCLR;
	/*------------------------------------------------------------------------------------------*/
	/*	送信(IN) バッファのクリア																*/
	/*------------------------------------------------------------------------------------------*/
	ARMPF_EP1_CONTROL |= D_EPn_BCLR;

	return;
}

/***************************************************************************
	APIs	USB MEMOBUSサーバー用処理関連
****************************************************************************/

/***************************************************************************
  Description: USB Endpoint2 受信開始(BLUK OUT)処理
----------------------------------------------------------------------------
  Parameters:
	Buffer;			受信データ格納先先頭アドレス
	OutLength;		受信データ格納先バッファサイズ
  Return:
	TRUE;			受信許可
	FALSE;			受信禁止
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncStartBkout( UCHAR *Buffer ,USHORT OutLength )
{
	ULONG			rc;								/* 戻り値						*/
	USB_FUNCTION_IF	*pUsbfIf;						/* USB情報領域ポインタ			*/

	rc = FALSE;										/* 初期値 : FALSE				*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB情報領域のアドレス取得	*/

	/*----------------------------------------------------------------------------------------------*/
	/*	EP2使用可能かチェックする :: インターフェイスがConfigutation済みの場合に使用可能			*/
	/*----------------------------------------------------------------------------------------------*/
	if( (ARMPF_USB_STATUS & D_USB_CONF) == D_USB_CONF )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP2使用可能時処理																		*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->OutLengthReq = OutLength;			/* 受信長許可サイズを記憶					*/
		pUsbfIf->OutLength = 0;						/* 受信済みバイト数を初期化					*/
		pUsbfIf->OutBufPtr = Buffer;				/* 受信データ格納先ポインタを記憶			*/
		/*------------------------------------------------------------------------------------------*/
		/*	EP2バッファのクリア																		*/
		/*------------------------------------------------------------------------------------------*/
		ARMPF_EP2_CONTROL |= D_EPn_BCLR;
		/*------------------------------------------------------------------------------------------*/
		/*	分割コピーフラグを解除	<S182>														*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->OutSplitCopy = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	ステータスを受信可能状態にセット														*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->Ep2Status = BLKTRNS_ENABLE;		/* EP2の状態を受信許可に設定				*/
		ARMPF_EP2_CONTROL &= ~D_EPn_ONAK;			/* EP2のNAK応答を解除						*/

		rc = TRUE;
	}
	else
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP2使用不可の場合、使用禁止にしてリターンする											*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->Ep2Status = BLKTRNS_DISABLE;		/* EP2の状態を受信禁止に設定				*/
		rc = FALSE;
	}
	return( rc );
}

/***************************************************************************
  Description: USB Endpoint1 送信開始(BLUK IN)処理
----------------------------------------------------------------------------
  Parameters:
	Buffer;			送信データ格納先先頭アドレス
	InLength;		送信データサイズ
  Return:
	TRUE;			送信許可
	FALSE;			送信禁止
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncStartBkin( UCHAR *Buffer ,USHORT InLength )
{
	ULONG			rc;								/* 戻り値						*/
	USB_FUNCTION_IF	*pUsbfIf;						/* USB情報領域ポインタ			*/

	rc = FALSE;										/* 初期値 : FALSE				*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB情報領域のアドレス取得	*/

	/*----------------------------------------------------------------------------------------------*/
	/*	EP1使用可能かチェックする :: インターフェイスがConfigutation済みの場合に使用可能			*/
	/*----------------------------------------------------------------------------------------------*/
	if( (ARMPF_USB_STATUS & D_USB_CONF) == D_USB_CONF )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP1使用可能時処理																		*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->InLengthRem = InLength;			/* 送信データ残りサイズを初期化				*/
		pUsbfIf->InLength = 0;						/* 送信済みバイト数を初期化					*/
		pUsbfIf->InBufPtr = Buffer;					/* 受信データ格納先を記憶					*/
		/*------------------------------------------------------------------------------------------*/
		/*	EP1バッファのクリア																		*/
		/*------------------------------------------------------------------------------------------*/
		ARMPF_EP1_CONTROL |= D_EPn_BCLR;
		/*------------------------------------------------------------------------------------------*/
		/*	分割コピーフラグを解除	<S182>														*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->InSplitCopy = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	ステータスを送信可能状態にセット														*/
		/*------------------------------------------------------------------------------------------*/
		ARMPF_EP2_STATUS &= ~D_EPn_OUT_NAK_ERR_INT;		/* EP2 OUTトークン受信状態クリア			*/
		pUsbfIf->Ep1Status = BLKTRNS_ENABLE;		/* EP1の状態を送信許可に設定				*/

		rc = TRUE;
	}
	else
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP1使用不可のDMA設定せずにターンする													*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->Ep1Status = BLKTRNS_DISABLE;		/* EP1の状態を送信禁止に設定				*/
		rc = FALSE;
	}
	return( rc );
}

/***************************************************************************
  Description: USBポート状態通知サービス処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	TRUE;			アプリケーション層通信可能
	FALSE;			アプリケーション層通信不可
----------------------------------------------------------------------------
  Note:
	アプリケーション層通信が可能であるかを返信します。
	・USBでは、SET_CONFIGURATION Val = 1 をHostがDeviceに指令するとEndpoint0以外の(本ドラ
	  イバでは1と2)が使用可能になります。アプリケーション通信はEP1とEP2を使用します。
****************************************************************************/
ULONG	UsbFuncPortConfiguration( void )
{
	ULONG	rc;								/* 自関数戻り値				*/
	UINT32	ret;							/* 他関数戻り値				*/

	rc = FALSE;								/* 初期値 : FALSE			*/

	/* デバイスステート取得	*/
	ret = udev_get_DeviceState();
	if( ret == UDEV_ST_CONFIGURED)
	{	/* Configuredステートが終了している	*/
		rc = TRUE;
	}

	return( rc );
}

/***************************************************************************
  Description: USB Endpoint2 受信バイト数取得処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	受信データバイトサイズ
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
USHORT	UsbFuncGetRcvSize( void )
{
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ		*/

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB情報領域アドレス取得	*/
	
	return( pUsbfIf->OutLength );			/* 受信データバイトサイズ	*/
}

/***************************************************************************
  Description: バルク転送(OUT)状態通知サービス処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	バルク転送(OUT)状態
	BLKTRNS_DISABLE;		バルク転送禁止
	BLKTRNS_ENABLE;			バルク転送許可
	BLKTRNS_END;			バルク転送完了
	BLKTRNS_STALL;			バルク転送にて異常発生しSTALLした
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncChkRcvComp( void )
{
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ		*/

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB情報領域アドレス取得	*/

	return( (ULONG)(pUsbfIf->Ep2Status) );	/* バルク転送(OUT)状態		*/
}

/***************************************************************************
  Description: バルク転送(IN)状態通知サービス処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	バルク転送(IN)状態
	BLKTRNS_ENABLE;			バルク転送許可
	BLKTRNS_END;			バルク転送終了
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncChkTrnsComp( void )
{
	ULONG			rc;						/* 戻り値					*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB情報領域ポインタ		*/

	rc = BLKTRNS_ENABLE;					/* 初期値 : BLKTRNS_ENABLE	*/

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB情報領域アドレス取得	*/

	/* 送信バッファが空になったら送信完了*/
	if( (pUsbfIf->Ep1Status == BLKTRNS_END) 
		&& ((ARMPF_EP1_STATUS & D_EPn_IN_EMPTY) == D_EPn_IN_EMPTY) )
	{
		rc = BLKTRNS_END;
	}
	/* 送信完了待ち状態で、OUTトークンを受信した場合は強制終了 */
	if( (ARMPF_EP2_STATUS & D_EPn_OUT_NAK_ERR_INT) == D_EPn_OUT_NAK_ERR_INT )
	{
		rc = BLKTRNS_END;
	}
	return( rc );
}


/***************************************************************************
	Locals
****************************************************************************/

/***************************************************************************
  Description:ベンダーリクエストコールバック処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;			デバイスリクエスト
  Return:
	ER_OK;					成功
	その他;					失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET VendorRequest_Callback( DEVICE_REQUEST* pDeviceRequest )
{
	UINT32			data;						/* Max Logical Unit Number	*/
	ER_RET			result;						/* 結果						*/
	USB_FUNCTION_IF	*pUsbfIf;					/* USB情報領域ポインタ		*/

	/* 戻り値初期化			*/
	result = ER_SYS;

	/* USB情報領域アドレス取得	*/
	pUsbfIf = UsbFuncGetInfoAddress();

	/* Configuredステート終了確認	*/
	if (pUsbfIf->UsbfInfo.usb.curr_config == 0) {
		result = ER_SYS;
		return result;
	}

	/* リクエスト識別	*/
	switch( pDeviceRequest->bRequest )
	{
		/* Get own address	*/
		case USB_VENDER_GET_OWN_ADDRESS:
			/* パラメータ・チェック		*/
			if ( ( ( pDeviceRequest->bmRequestType != 0xc0   ) &&
				   ( pDeviceRequest->bmRequestType != 0xc3   ) ) ||
			     ( pDeviceRequest->wValue        != 0x0000     ) ||
			     ( pDeviceRequest->wIndex        != 0x0000     ) ||
			     ( ( pDeviceRequest->wLength       != 0x0001 ) &&		/* <S072> tanaka21 */
			      ( pDeviceRequest->wLength       != 0x0004 ) &&
			       ( pDeviceRequest->wLength       != 0x0002 ) )
			   )
			{
				result = ER_SYS;
				break;
			}

			/* 自局アドレス送信	*/
			data = 0;
			data |= pUsbfIf->UsbfInfo.MyAddress;
			UsbFuncWriteEP0( (UCHAR*)&data, pDeviceRequest->wLength );

			result = ER_OK;
			break;

		/* 非定義リクエスト処理	*/
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: USBファンクション情報初期化処理
----------------------------------------------------------------------------
  Parameters:
	pUsbfInfo;				USBデバイス情報領域へのポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void	UsbFuncInitializeInfo( U2F_INFO *pUsbfInfo )
{
	/* Descriptor設定	*/
	pUsbfInfo->desc.pDevice							= (DEVICE_DESCRIPTOR*)&UsbFunctionDeviceDescriptor;
	pUsbfInfo->desc.pQualifier						= (DEVICE_QUALIFIER_DESCRIPTOR*)&UsbFunctionQualifierDescriptor;
	pUsbfInfo->desc.pConf_fs						= (DESC_CONFIG*)&UsbFunctionConfigurationDescriptorFS;
	pUsbfInfo->desc.pConf_hs						= (DESC_CONFIG*)&UsbFunctionConfigurationDescriptorHS;
	pUsbfInfo->desc.pString[STRIDX_LANG]			= (STRING_DESCRIPTOR*)LanguageIdStringDescriptor;
	pUsbfInfo->desc.pString[STRIDX_MANUFACTURER]	= (STRING_DESCRIPTOR*)ImanufacturerStringDescriptor;
	pUsbfInfo->desc.pString[STRIDX_PRODUCT]			= (STRING_DESCRIPTOR*)IproductStringDescriptor;
	pUsbfInfo->desc.pString[STRIDX_SERIALNUMBER]	= (STRING_DESCRIPTOR*)IserialNumberStringDescriptor;
	pUsbfInfo->desc.pString[STRIDX_CONFIGURATION]	= (STRING_DESCRIPTOR*)IconfigurationStringDescriptor;
	pUsbfInfo->desc.pString[STRIDX_INTERFACE]		= (STRING_DESCRIPTOR*)IinterfaceStringDescriptor;
	pUsbfInfo->dev.ep_num							= U2F_EPn_MAX;				/* 利用するEP数									*/
#if 0	/* XXX:サンプルソースでコメントアウトされていた */
	pUsbfInfo->dev.ep_dir							= DIR_OUT_EP2;				/* EPn方向(OUT:1,IN:0) Bit0=EP1 - Bit14=EP15	*/
#endif
	pUsbfInfo->cb_class_req							= 0;						/* Class Requestコールバック関数(なし)			*/
	pUsbfInfo->cb_vendor_req						= VendorRequest_Callback;	/* Vendor Requestコールバック関数				*/

	return;
}

/***************************************************************************
  Description: フラグ・クリア処理
----------------------------------------------------------------------------
  Parameters:
	pUsbfFlag;				USBフラグへのポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncClearFlag( USB_FLAG *pUsbfFlag )
{
	pUsbfFlag->UsbSetupReceive = 0;						/* SETUPトークン受信回数	*/
	pUsbfFlag->UsbSetupSave = 0;						/* 上記保存値				*/
}

/***************************************************************************
  Description: Max Packet Size 設定処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncSetMaxpacket ( void )
{
	UINT32	bulk_size;								/* バルク転送サイズ用	*/
	UINT32	interrupt_size;							/* 割り込み転送サイズ用	*/
 
	/*---------------------------------*/
	/* USBバス・スピード確認		   */
	/*---------------------------------*/
	if ( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE )
	{
		/* High Speed	*/
		bulk_size      = BULK_MAXPACKET_SIZE_HS;
		interrupt_size = INT_MAXPACKET_SIZE_HS;
	}
	else
	{
		/* Full Speed	*/
		bulk_size      = BULK_MAXPACKET_SIZE_FS;
		interrupt_size = INT_MAXPACKET_SIZE_FS;
	}

	/*---------------------------------*/
	/* Max Packet Size 設定			   */
	/*---------------------------------*/
														/* EPn : BaseAdr  MaxPkt(HS/FS 本ドライバでは規格上の最大値を設定)	*/
														/* EP0 : 0x0000        64 byte x 2面								*/
	ARMPF_EP1_PCKT_ADRS = 0x00200000 | bulk_size;				/* EP1 : 0x0020    512/64 byte x 2面	--   Bulk    転送 --		*/
	ARMPF_EP2_PCKT_ADRS = 0x01200000 | bulk_size;				/* EP2 : 0x0120    512/64 byte x 2面	--   Bulk    転送 --		*/
	ARMPF_EP3_PCKT_ADRS = 0x02200000 | interrupt_size;		/* EP3 : 0x0220   1024/64 byte x 1面	-- Interrupt 転送 --		*/
														/*  BaseAdrは1ワード(4バイト)単位									*/
														/*		EP0は 64(byte) x 2 = 128(byte) = 64(word) = 0x0020 より		*/
														/*		EP1のBaseAdrは 0x0000+0x0020 = 0x0020 となる				*/
}

/***************************************************************************
  Description: USB情報領域のアドレス取得処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	USB情報領域のアドレス
----------------------------------------------------------------------------
  Note:
	USB情報領域のアドレスは本関数を利用して取得すること
****************************************************************************/
static USB_FUNCTION_IF*	UsbFuncGetInfoAddress( void )
{
	return &UsbfIf;
}

/***************************************************************************
  Description: 接続状態確認処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	U2F_PLUG_OUT;		USB Hostと接続されていないとき
	U2F_PLUG_IN;		USB Hostと接続されているとき
****************************************************************************/
static UINT UsbFuncCheckPlugInout( void )
{
	USB_FUNCTION_IF	*pUsbfIf;						/* USB情報領域ポインタ			*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB情報領域のアドレス取得	*/

	return pUsbfIf->UsbfFlag.UsbPlugStatus;			/* 接続状態を返す				*/
}

/***************************************************************************
  Description: SETUPトークン受信確認処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	0;		SETUPトークン未受信
	0以外;	SETUPトークン受信
****************************************************************************/
static UINT UsbFuncCheckSetupReceive( void )
{
	USB_FUNCTION_IF	*pUsbfIf;						/* USB情報領域ポインタ			*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB情報領域のアドレス取得	*/

	/* SETUPトークン受信状態を返す	*/
	return (pUsbfIf->UsbfFlag.UsbSetupReceive - pUsbfIf->UsbfFlag.UsbSetupSave);
}

/***************************************************************************
	Locals	EPn転送関連
****************************************************************************/

/***************************************************************************
  Description: EPn ストール応答処理
----------------------------------------------------------------------------
  Parameters:
	num;			EP番号
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncSetStallEPn( UINT num )
{
	/* --- IN/OUT 判断 (制御レジスタの転送方向で確認 0:IN, 1:OUT ) --- */
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		/* --- BULK IN --- */
		/* INトークンにSTALL応答 */
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_ISTL;
	} else {
		/* --- BULK OUT --- */
		/* OUT/PINGトークンにSTALL応答、および本設定の有効化 */
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= (D_EPn_OSTL_EN | D_EPn_OSTL);
	}
}


/* <S182> >>>>> */
/***************************************************************************
  Description: OUTデータ分割コピー処理
----------------------------------------------------------------------------
  Parameters:
	pUsbfIf;			USB情報領域ポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncOutDataSplitCopy( USB_FUNCTION_IF *pUsbfIf )
{
	INT				i;							/* ループカウンタ				*/
	USHORT			strind;						/* 送受信バッファ内処理位置		*/
	INT				pktlen;						/* 受信パケットバイトサイズ		*/
	UINT32			TmpWordData;				/* ワードデータテンポラリ		*/
	UINT32			MaxPacketSize;				/* 最大パケットサイズ			*/
	UINT32			CopySize;					/* コピーサイズ					*/

	CopySize = SCAN_COPY_SIZE;

	strind = pUsbfIf->OutLength;				/* 今回受信データ先頭index		*/
												/* 今回処理する受信長設定		*/
	if( pUsbfIf->OutCopyRem >= CopySize )
	{
		pktlen = CopySize;
		pUsbfIf->OutCopyRem -= CopySize;
	}
	else
	{
		pktlen = pUsbfIf->OutCopyRem;
		pUsbfIf->OutCopyRem = 0;
	}
	pUsbfIf->OutLength += (USHORT)pktlen;		/* 今回受信データ最後尾index	*/

	/*--------------------------------------------------------------------------*/
	/*	受信する																*/
	/*--------------------------------------------------------------------------*/
	while( strind < pUsbfIf->OutLength )
	{
		TmpWordData = ARMPF_EP2_READ;			/* 4バイトデータ取得			*/
		for( i = 0; i < 4; i++ )
		{
			/* 1バイトずつ4バイト分をバッファに格納する */
			pUsbfIf->OutBufPtr[strind] = (UINT8)(TmpWordData >> (8*i));
			strind++;
			/* 端数(1～3)バイトのチェック	*/
			if( strind >= pUsbfIf->OutLength )
			{
				break;
			}
		}
	}
	/*--------------------------------------------------------------------------*/
	/*	受信完了判定															*/
	/*--------------------------------------------------------------------------*/
	/* 今回受信分(マックス・パケットサイズまで)を全てコピーしていれば完了 */
	if( pUsbfIf->OutCopyRem == 0 )
	{
		/* EP2のマックス・パケットサイズ取得 */
		MaxPacketSize = ARMPF_EP2_PCKT_ADRS & D_EPn_MPKT;

		if( pUsbfIf->OutPacketSize < MaxPacketSize )	/* 今回のﾊﾟｹｯﾄが最大サイズ未満なら完了	*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;		/* EP2をNAK応答のみに設定			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;		/* 受信完了をセット					*/
		}
		pUsbfIf->OutSplitCopy = FALSE;				/* 分割コピーフラグを解除			*/
	}
	else
	{
		pUsbfIf->OutSplitCopy = TRUE;				/* 分割コピーフラグを設定			*/
	}
}


/***************************************************************************
  Description: INデータ分割コピー処理
----------------------------------------------------------------------------
  Parameters:
	pUsbfIf;			USB情報領域ポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncInDataSplitCopy( USB_FUNCTION_IF *pUsbfIf )
{
	INT				i;							/* ループカウンタ				*/
	USHORT			strind;						/* 送受信バッファ内処理位置		*/
	UINT32			TmpWordData;				/* ワードデータテンポラリ		*/
	UINT32			TmpFlag;					/* 制御レジスタ値編集用			*/
	UINT32			CopySize;					/* コピーサイズ					*/
	UCHAR			endflag;					/* 終了フラグ					*/

	endflag = FALSE;							/* 初期値 : FALSE				*/

	/* EP1のコピーサイズ再設定 */
	CopySize = SCAN_COPY_SIZE;

	if( pUsbfIf->InLengthRem >= CopySize )
	{
		/*----------------------------------------------------------------------*/
		/*	残サイズがEP1の最大パケットサイズ以上の場合							*/
		/*----------------------------------------------------------------------*/
		strind = pUsbfIf->InLength;				/* 今回送信データ先頭index		*/
		pUsbfIf->InLength += CopySize;			/* 次回送信データ先頭index		*/
		pUsbfIf->InLengthRem -= CopySize;		/* 残サイズ計算					*/
		pUsbfIf->InCopyRem -= CopySize;			/* 今回パケットの残サイズ計算	*/
	}
	else
	{
		/*----------------------------------------------------------------------*/
		/*	残サイズがEP1の最大パケットサイズ未満の場合（NULLも含む)			*/
		/*----------------------------------------------------------------------*/
		strind = pUsbfIf->InLength;				/* 今回送信データ先頭index		*/
		pUsbfIf->InLength += pUsbfIf->InLengthRem;	/* 次回送信データ先頭index	*/
		pUsbfIf->InCopyRem = 0;					/* 今回パケットの残サイズ		*/
		endflag = TRUE;							/* 今回で送信完了することを記憶	*/
	}
	/*--------------------------------------------------------------------------*/
	/*	データ送信処理															*/
	/*		※NULLの場合は１回もforループしない(書き込まない)					*/
	/*--------------------------------------------------------------------------*/
	/* 送信データの設定 */
	for( ; (strind + sizeof(UINT32) - 1) < pUsbfIf->InLength ; strind += 4 )
	{
		/* ワード(4バイト)データを作成して書き込み */
		TmpWordData   = (UINT32)pUsbfIf->InBufPtr[strind+0];
		TmpWordData  |= (UINT32)pUsbfIf->InBufPtr[strind+1] << 8;
		TmpWordData  |= (UINT32)pUsbfIf->InBufPtr[strind+2] << 16;
		TmpWordData  |= (UINT32)pUsbfIf->InBufPtr[strind+3] << 24;
		/* バッファ書き込み */
		ARMPF_EP1_WRITE = TmpWordData;
	}

	/* データ送信タイミング */
	if( pUsbfIf->InCopyRem == 0 )
	{
		/* --- データ送信許可準備 --- */
		TmpFlag  = ARMPF_EP1_CONTROL;						/* EP1制御レジスタ値取得		*/
		TmpFlag |= D_EPn_DEND;						/* 送信許可設定追加				*/

		/* 端数バイト処理(endflag=TRUEかつ、送信用インデックスが送信サイズ未満時) */
		if( (endflag == TRUE) && (strind < pUsbfIf->InLength) )
		{
			/* 端数バイト算出(0～3) : 1～3の時処理される	*/
			pUsbfIf->InLengthRem &= (BIT1 | BIT0);

			TmpWordData = 0;						/* データ0クリア				*/

			/* 端数データの設定	*/
			for (i = 0; i < pUsbfIf->InLengthRem; i++) {
				TmpWordData |= (UINT32)pUsbfIf->InBufPtr[strind+i] << (8*i);
			}

			/* --- バッファ書き込み ---	*/
			ARMPF_EP1_WRITE = TmpWordData;

			/* --- 端数バイト数の設定 --- */
			TmpFlag |= ( ((UINT32)pUsbfIf->InLengthRem) << 5 ) & ( D_EP0_DW );
		}

		/* データ送信 */
		ARMPF_EP1_CONTROL |= TmpFlag;

		/*--------------------------------------------------------------------------*/
		/*	送信完了セット															*/
		/*--------------------------------------------------------------------------*/
		if( endflag == TRUE )
		{
			pUsbfIf->Ep1Status = BLKTRNS_END;	/* 送信完了をセット					*/
		}
		pUsbfIf->InSplitCopy = FALSE;			/* 分割コピーフラグ解除				*/
	}
	else
	{
		pUsbfIf->InSplitCopy = TRUE;			/* 分割コピーフラグ設定				*/
	}
}
/* <<<<< <S182> */

/***************************************************************************
	Locals	コントロール転送関連
****************************************************************************/

/***************************************************************************
  Description: EP0 送信処理
----------------------------------------------------------------------------
  Parameters:
	pBuf;				送信データ格納領域
	size;				送信サイズ
  Return:
	0以上の値のとき;	送信したバイト数
	負の値のとき;		エラー発生による強制終了
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static INT UsbFuncWriteEP0( VUCHAR* pBuf, INT size )
{
	INT	iRemainSize;						/* 残りデータ長					*/

	UINT32	TmpData;						/* 書き込みデータ作成用ワーク	*/
	UINT32	temp;							/* 制御レジスタ値編集用ワーク	*/
	INT		i;								/* ループカウンタ				*/

	/* 残りデータ長	*/
	iRemainSize = size;

	/* 残りデータがあればループ	*/
	while (iRemainSize > 0)
	{
		/*-----------------------------*/
		/* バッファ・エンプティ確認	   */
		/*-----------------------------*/
		/* バッファが空になるまでウエイト	*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
		{
			/* --- ケーブルが抜かれたときは強制終了 --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return ER_SYS;
			}
		}

		/*-----------------------------*/
		/* バッファ書き込み			   */
		/*  (4byte単位, Max:64byte)	   */
		/*-----------------------------*/
		for (i = 0; ((i < 64) && (iRemainSize >= 4)); i += 4, iRemainSize -= 4)
		{
			/* --- データ生成 --- */
			TmpData  = (UINT32)*pBuf++;
			TmpData |= (UINT32)*pBuf++ <<  8;
			TmpData |= (UINT32)*pBuf++ << 16;
			TmpData |= (UINT32)*pBuf++ << 24;
			/* --- バッファ書き込み --- */
			ARMPF_EP0_WRITE = TmpData;
		}
		/* --- データ送信許可準備 --- */
		temp  = ARMPF_EP0_CONTROL;										/* CONTROLレジスタ リード	*/
		temp |= D_EP0_DEND;											/* 送信許可					*/

		/*-----------------------------*/
		/* 端数バイト(3byte以下)処理   */
		/*-----------------------------*/
		if ((i < 64) && (iRemainSize > 0) && (iRemainSize < 4))
		{
			/* --- 端数データ生成 --- */
			TmpData = 0;
			for (i = 0; i < iRemainSize; i++)
			{
				TmpData |= (UINT32)*pBuf++ << (8*i);
			}
			/* --- バッファ書き込み --- */
			ARMPF_EP0_WRITE = TmpData;
			/* --- データ送信許可 --- */
			temp |= ( ((UINT32)iRemainSize) << 5 ) & ( D_EP0_DW );	/* 端数バイトの設定			*/
			iRemainSize = 0;
		}
		/*-----------------------------*/
		/* データ送信許可			   */
		/*-----------------------------*/
		ARMPF_EP0_CONTROL = temp;											/* CONTROLレジスタ ライト	*/
	}

	return size;
}

/***************************************************************************
  Description: EP0 ストール応答処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncSetStallEP0( void )
{
	ARMPF_EP0_CONTROL |= D_EP0_STL;				/* ストール応答設定			*/
}

#if 0	/* 現時点では使用しないため <S150>*/
/***************************************************************************
  Description: EP0 NAK応答処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncSetNakEP0( void )
{
	UINT32 temp;								/* 設定編集用ワーク				*/

	temp  = ARMPF_EP0_CONTROL;						/* EP0制御レジスタ値読み出し	*/
	temp |= D_EP0_INAK_EN;						/* EP0_INAK書き込み有効化		*/
	temp |= (UINT32)(D_EP0_INAK | D_EP0_ONAK);	/* IN/OUT/PINGトークンへの		*/
												/* 強制NAK応答設定				*/
	ARMPF_EP0_CONTROL = temp;							/* EP0制御レジスタへ書き込み	*/
}

/***************************************************************************
  Description: EP0 NAK解除処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncClearNakEP0( void )
{
	UINT32 temp;								/* 設定編集用ワーク				*/

	temp  = ARMPF_EP0_CONTROL;						/* EP0制御レジスタ値読み出し	*/
	temp |= D_EP0_INAK_EN;						/* EP0_INAK書き込み有効化		*/
	temp &= ~(UINT32)(D_EP0_INAK | D_EP0_ONAK);	/* IN/OUT/PINGトークンへの		*/
												/* 強制NAK応答解除設定			*/
	ARMPF_EP0_CONTROL = temp;							/* EP0制御レジスタへ書き込み	*/
}

/***************************************************************************
  Description: EP0 NULL送信処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET UsbFuncSendNullEP0( void )
{
	/*-----------------------------*/
	/* バッファ・エンプティ確認	   */
	/*-----------------------------*/
	/* バッファが空になるまでウエイト	*/
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return ER_SYS;
		}
	}

	/*-----------------------------*/
	/* データ送信許可			   */
	/*-----------------------------*/
	ARMPF_EP0_CONTROL |= D_EP0_DEND;					/* 送信許可					*/

	return ER_OK;
}
#endif

/***************************************************************************
  Description: EP0 No Data処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32	temp;					/* 設定編集用ワーク		*/
	ER_RET	result;					/* 結果					*/

	/* 戻り値初期化	*/
	result = ER_SYS;

	/*=========================================*/
	/* デコード処理							   */
	/*=========================================*/
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result )
	{
		UsbFuncSetStallEP0();
		return result;
	}

	/*=========================================*/
	/* ステータス・ステージ移行待ち			   */
	/*=========================================*/
	/* ステータス・ステージに移行するまでウエイト */
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT)
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;							/* 要因クリア */

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{															/* バッファが空でないとき，			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> バッファを空にする。			*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
		{
			/* --- ケーブルが抜かれたときは強制終了 --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* INトークン強制NAK応答を解除			   */
	/*=========================================*/
	temp = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = temp;

	/*=========================================*/
	/* ステータスステージ					   */
	/*=========================================*/
	/* NULLデータ送信 */
	/* ------ バッファ・エンプティ確認 ------ */
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/* バッファが空になるまでウエイト */
	{
		/*/ --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}

	/* ------ データ送信許可 ------ */
	temp  = ARMPF_EP0_CONTROL;									/* CONTROLレジスタ リード	*/
	temp |= D_EP0_DEND;										/* 送信許可					*/
	ARMPF_EP0_CONTROL = temp;										/* CONTROLレジスタ ライト	*/

	/*----------------------------------------*/
	/* ステータスステージ終了確認			  */
	/*----------------------------------------*/
	/* ステータス・ステージが終了するまでウエイト */
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT)
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;						/* 要因クリア */

	return ER_OK;
}


/***************************************************************************
  Description: EP0 Control Read処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32	temp;					/* 設定編集用ワーク		*/
	ER_RET	result;					/* 結果					*/
	UINT32	tcnt;					/* タイムアウトカウンタ	<S1FE> */

	/* 戻り値初期化	*/
	result = ER_SYS;

	/*=========================================*/
	/* バッファ・エンプティ確認				   */
	/*=========================================*/
	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{															/* バッファが空でないとき，			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> バッファを空にする。			*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
		{
			/* --- ケーブルが抜かれたときは強制終了 --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* INトークン強制NAK応答を解除			   */
	/*=========================================*/
	temp = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = temp;

	/*=========================================*/
	/* デコード処理							   */
	/*=========================================*/
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	/*=========================================*/
	/* ステータス・ステージ移行待ち			   */
	/*=========================================*/
	/* ステータス・ステージに移行するまでウエイト */
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT)
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;								/* 要因クリア */

	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)
	{																/* バッファが空でないとき，			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									/*  -> バッファを空にする。			*/
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
		{
			/* --- ケーブルが抜かれたときは強制終了 --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* OUTトークン強制NAK応答を解除			   */
	/*=========================================*/
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;

	/*----------------------------------------*/
	/* ステータスステージ					  */
	/*----------------------------------------*/
	/* NULLデータ受信 */
	while((ARMPF_EP0_STATUS & D_EP0_OUT_NULL_INT) != D_EP0_OUT_NULL_INT)	/* NULLデータを受信するまでウエイト */
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_OUT_NULL_INT;								/* 要因クリア */

	/*----------------------------------------*/
	/* ステータスステージ終了確認			  */
	/*----------------------------------------*/
	/* ステータス・ステージが終了するまでウエイト */
	KlibRstLongTimer(&tcnt);										/* タイマカウントスタート <S1FE> */
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT)
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
		if( UsbFuncCheckSetupReceive() != 0 )
		{
			return result;
		}
		if( KlibGetLongTimerMs(tcnt) > 1000  )
		{/* 1s以上続くとタイムアウト <S1FE> */
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;								/* 要因クリア */

	return ER_OK;
}


/***************************************************************************
  Description: EP0 Control Write処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32	temp;					/* 設定編集用ワーク		*/
	ER_RET	result;					/* 結果					*/

	/* 戻り値初期化	*/
	result = ER_SYS;

	/*=========================================*/
	/* バッファ・エンプティ確認				   */
	/*=========================================*/
	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)
	{																/* バッファが空でないとき，			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									/*  -> バッファを空にする。			*/
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
		{
			/* --- ケーブルが抜かれたときは強制終了 --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* OUTトークン強制NAK応答を解除			   */
	/*=========================================*/
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;

	/*=========================================*/
	/* デコード処理							   */
	/*=========================================*/
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result )
	{
		UsbFuncSetStallEP0();
		return result;
	}

	/*=========================================*/
	/* ステータス・ステージ移行待ち			   */
	/*=========================================*/
	/* ステータス・ステージに移行するまでウエイト */
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT)
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;							/* 要因クリア */

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{															/* バッファが空でないとき，			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> バッファを空にする。			*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
		{
			/* --- ケーブルが抜かれたときは強制終了 --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* INトークン強制NAK応答を解除			   */
	/*=========================================*/
	temp = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = temp;

	/*----------------------------------------*/
	/* ステータスステージ					  */
	/*----------------------------------------*/
	ARMPF_EP0_CONTROL |= D_EP0_DEND;

	/*----------------------------------------*/
	/* ステータスステージ終了確認			  */
	/*----------------------------------------*/
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT)
	/* ステータス・ステージが終了するまでウエイト */
	{
		/* --- ケーブルが抜かれたときは強制終了 --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;							/* 要因クリア */

	return ER_OK;
}


/*=============================================================================*/
/* USB STANDARD REQUEST														   */
/*=============================================================================*/

/***************************************************************************
  Description: EP0 リクエストデコード処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET decode_request(DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET	result;					/* 結果					*/

	/* 戻り値初期化	*/
	result = ER_SYS;

	/* コントロール転送タイプ判別 */
	switch( pDeviceRequest->bmRequestType & USB_REQUEST )
	{
		/*-----------------------------*/
		/* 標準デバイス・リクエスト	   */
		/*-----------------------------*/
		case USB_REQUEST_STANDARD:
			result = bmRequestType_standard( pDeviceRequest, pUsbfInfo );
			break;

		/*-----------------------------*/
		/* クラス・リクエスト		   */
		/*-----------------------------*/
		case USB_REQUEST_CLASS:
			if( pUsbfInfo->cb_class_req )
			{
				result = pUsbfInfo->cb_class_req( pDeviceRequest );
			}
			else
			{
				result = ER_SYS;
			}
			break;

		/*-----------------------------*/
		/* ベンダ固有リクエスト		   */
		/*-----------------------------*/
		case USB_REQUEST_VENDOR:
			if( pUsbfInfo->cb_vendor_req )
			{
				result = pUsbfInfo->cb_vendor_req( pDeviceRequest );
			}
			else
			{
				result = ER_SYS;
			}
			break;

		/*-----------------------------*/
		/* その他					   */
		/*-----------------------------*/
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 スタンダードリクエスト処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UCHAR	bRequest;			/* リクエストコード	*/
	ER_RET	result;				/* 結果				*/

	/* 変数初期化 */
	bRequest = pDeviceRequest->bRequest;
	result = ER_SYS;

	switch( bRequest ) {
		case USB_STD_GET_STATUS:							/* GET STATUS			*/
			result = std_req_get_status( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_CLEAR_FEATURE:							/* CLEAR FEATURE		*/
			result = std_req_clear_feature( pDeviceRequest );
			break;
		case USB_STD_SET_FEATURE:							/* SET FEATURE			*/
			result = std_req_set_feature( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_ADDRESS:							/* SET ADDRESS			*/
			result = std_req_set_address( pDeviceRequest );
			break;
		case USB_STD_GET_DESCRIPTOR:						/* GET DESCRIPTOR		*/
			result = std_req_get_descriptor( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_DESCRIPTOR:						/* SET DESCRIPTOR		*/
			result = std_req_set_descriptor( pDeviceRequest );
			break;
		case USB_STD_GET_CONFIGURATION:						/* GET CONFIGURATION	*/
			result = std_req_get_configuration( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_CONFIGURATION:						/* SET CONFIGURATION	*/
			result = std_req_set_configuration( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_GET_INTERFACE:							/* GET INTERFACE		*/
			result = std_req_get_interface( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_INTERFACE:							/* SET INTERFACE		*/
			result = std_req_set_interface( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SYNCH_FRAME:							/* SYNC FRAME			*/
			result = std_req_synch_frame( pDeviceRequest );
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}


/***************************************************************************
  Description: EP0 CLEAR_FEATURE処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest )
{
	UINT8	recipient;				/* 処理ターゲット		*/
	UINT8	direction;				/* 入出力方向			*/
	UINT16	selector;				/* セレクタ値			*/
	UINT16	wIndex;					/* インデックス値		*/
	UINT16	endpoint;				/* エンドポイント番号	*/
	ER_RET	result;					/* 結果					*/
	USB_FUNCTION_IF	*pUsbfIf;		/* USB情報領域ポインタ	*/
	USB_INFO		*pUsbInfo;		/* USB情報ポインタ		*/

	/* 変数初期化 */
	pUsbfIf = UsbFuncGetInfoAddress();
	pUsbInfo = &(pUsbfIf->UsbfInfo.usb);

	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	selector  = pDeviceRequest->wValue;
	wIndex    = pDeviceRequest->wIndex;
	endpoint  = wIndex & 0xFF;

	result = ER_SYS;

	/*---------------------------------*/
	/* パラメータ・チェック(定数箇所)  */
	/*---------------------------------*/
	if( ( 0x0000 != pDeviceRequest->wLength ) ||
		( USB_DIR_OUT != direction ) )
	{
		return ER_SYS;
	}

	switch( recipient )
	{
		case USB_TARGET_DEVICE:
			if( 0x0000 == wIndex )
			{
				if( USB_FEATURE_DEVICE_REMOTE_WAKEUP == selector )
				{
					/* リモートウェイクアップ禁止設定 */
					pUsbInfo->flag_RemoteWakeup = U2F_DISABLE;
					result = ER_OK;
				}
				else
				{
					result = ER_SYS;
				}
			}
			else
			{
				result = ER_SYS;
			}
			break;
		case USB_TARGET_INTERFACE:
			result = ER_SYS;
			break;
		case USB_TARGET_ENDPOINT:
			if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
				 ( 0x0000 != ( wIndex & 0x0070 ) ) )
			{
				return ER_SYS;
			}

			if( USB_FEATURE_ENDPOINT_HALT == selector )
			{
				if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) )
				{
					/* EP0 */
					/* バッファエンプティ確認 */
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
					{															/* バッファが空でないとき，			*/
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> バッファを空にする。			*/
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
						{
							/* --- ケーブルが抜かれたときは強制終了 --- */
							if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
							{
								return ER_SYS;
							}
						}
					}
					result = ER_OK;
				}
				else
				{
					/* Other EP */
					if( ER_OK != Clear_Feature_EP( endpoint & ~((UINT)USB_DIR)) )
					{
						return ER_SYS;
					}
				}
				result = ER_OK;
			}
			else
			{
				return ER_SYS;
			}
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 CLEAR_FEATURE処理( TARGET = EP )
----------------------------------------------------------------------------
  Parameters:
	num;				EP番号
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Clear_Feature_EP( UINT num )
{
	UINT32	temp;					/* 設定編集用ワーク		*/
	UINT32	temp2;					/* 設定確認用ワーク		*/

	/* Parameter確認 */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* EP Enable確認 */
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN)
	{
		return ER_SYS;
	}

	/* EPn Clear Feature */
	if( (udev_get_DeviceState() & UDEV_ST_CONFIGURED) != 0 )
	{
		/* Buffer Clear */
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_BCLR;													/* 送受信バッファ・クリア */
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 )
		{	/* IN */
			/* PID初期化 */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_IPIDCLR;											/* 送信PIDクリア */
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			/* バッファエンプティ確認 */
			while( ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) ||
			       ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_DATA ) == D_EPn_IN_DATA ) )
			{	/*  -> 実際に空になるまでウエイト。 */
				/* --- ケーブルが抜かれたときは強制終了 --- */
				if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
				{
					return ER_SYS;
				}
			}
			/* EP STALL解除 */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp &= ~(D_EPn_ISTL);											/* INトークン STALL解除 */
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		}
		else
		{	/* OUT */
			/* PID初期化 */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OPIDCLR;											/* 受信PIDクリア */
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			/* EP STALL解除 */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OSTL_EN;											/* EPn_OSTL ビットへの書き込みを有効にする	*/
			temp &= ~(D_EPn_OSTL);											/* OUT/PINGトークン STALL解除				*/
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		}
	}
	else
	{
		return ER_SYS;
	}

	temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
	temp2 = *((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20));
	if ((temp | temp2) != 0)
	{
		__nop();
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_CONFIGURATION処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET	result;					/* 結果					*/

	/* 変数初期化 */
	result = ER_SYS;

	if( ( 0x0000 != pDeviceRequest->wValue  ) ||
	    ( 0x0000 != pDeviceRequest->wIndex  ) ||
	    ( 0x0001 != pDeviceRequest->wLength ) ||
	    ( USB_DIR != pDeviceRequest->bmRequestType ) )
	{
		return ER_SYS;
	}

	/* コンフィギュレーション値は「1」のみサポート */
	result = UsbFuncWriteEP0( (UCHAR*)&(pUsbfInfo->usb.curr_config), 1 );
	if( result < 0 )
	{
		return result;
	}
	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	wValue;					/* 値					*/
	UINT16	wIndex;					/* インデックス値		*/
	UINT16	descriptor_type;		/* ディスクリプタタイプ	*/
	ER_RET	result;					/* 結果					*/

	/* 変数初期化 */
	wValue			= pDeviceRequest->wValue;
	wIndex			= pDeviceRequest->wIndex;
	descriptor_type	= (wValue >> 8);

	result			= ER_SYS;

	/* リクエストタイプの確認 */
	if( USB_DIR != pDeviceRequest->bmRequestType )
	{
		return ER_SYS;
	}

	/* ディスクリプタタイプ毎に処理を行う */
	switch( descriptor_type )
	{
		case USB_DESC_TYPE_DEVICE:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) )
			{
				/* Device Descriptor */
				result = Get_Descriptor_Device( pDeviceRequest, pUsbfInfo );
			}
			else
			{
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_CONFIGURATION:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) )
			{
				/* Configuration Descriptor */
				result = Get_Descriptor_Configuration( pDeviceRequest, pUsbfInfo );
			}
			else
			{
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_STRING:
			/* String Descriptor */
			result = Get_Descriptor_String( pDeviceRequest, pUsbfInfo );
			break;
		case USB_DESC_TYPE_DEVICE_QUALIFIER:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) )
			{
				/* Device Qualifier Descriptor */
				result = Get_Descriptor_Qualifier( pDeviceRequest, pUsbfInfo );
			}
			else
			{
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) )
			{
				/* Qther Speed Configuration Descriptor */
				result = Get_Descriptor_OtherSpeed( pDeviceRequest, pUsbfInfo );
			}
			else
			{
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_INTERFACE_POWER:
			result = ER_SYS;
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR DEVICE処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void*	pData;					/* DEVICEディスクリプタの先頭アドレス	*/
	INT		size;					/* DEVICEディスクリプタのデータサイズ	*/
	ER_RET	result;					/* 結果									*/

	/* 変数初期化 */
	size = 0;
	result = ER_SYS;

	/* DEVICEディスクリプタの返却 */
	pData  = (void*)(pUsbfInfo->desc.pDevice);
	size   = (INT)((DEVICE_DESCRIPTOR*)pData)->bLength;
	if (size > (INT)pDeviceRequest->wLength)
	{
		size = (INT)pDeviceRequest->wLength;
	}
	result = UsbFuncWriteEP0( (UCHAR*)pData, size );
	if( result < 0 )
	{
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR CONFIGURATION処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATIONディスクリプタの先頭アドレス	*/
	INT				size;			/* CONFIGURATIONディスクリプタのデータサイズ	*/
	ER_RET			result;			/* 結果											*/

	/* 変数初期化 */
	size	= 0;
	result	= ER_SYS;

	/* CONFIGURATIONディスクリプタの返却 */
	if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE )
	{
		/* High Speed */
		pConfigDesc = pUsbfInfo->desc.pConf_hs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}
	else
	{
		/* Full Speed */
		pConfigDesc = pUsbfInfo->desc.pConf_fs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}

	/* 送信 */
	if (size > (INT)pDeviceRequest->wLength)
	{
		size = (INT)pDeviceRequest->wLength;
	}
	result = UsbFuncWriteEP0( (UCHAR*)pConfigDesc, size );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR STRING処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void*	pData;					/* STRINGディスクリプタの先頭アドレス	*/
	INT		size;					/* STRINGディスクリプタのデータサイズ	*/
	ER_RET	result;					/* 結果									*/
	INT		string_num;				/* STRING番号							*/

	/* 変数初期化 */
	size	= 0;
	result	= ER_SYS;

	/* リクエストされたSTRING番号 */
	string_num = (INT)(pDeviceRequest->wValue & 0xff);

	/* Not Support INDEX */
	if( ( string_num < 0 ) || ( STRIDX_MAX <= string_num ) )
	{
		return ER_SYS;
	}

	pData = (void*)(pUsbfInfo->desc.pString[string_num]);
	size  = (INT)(((STRING_DESCRIPTOR*)pData)->bLength);

	/* データサイズが要求サイズより大きい場合 */
	if (size > (INT)pDeviceRequest->wLength)
	{
		size = (INT)pDeviceRequest->wLength;
	}
	result = UsbFuncWriteEP0( (UCHAR*)pData, size );
	if( result < 0 )
	{
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR DEVICE QUALIFIER処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void*	pData;					/* DEVICE QUALIFIERディスクリプタの先頭アドレス	*/
	INT		size;					/* DEVICE QUALIFIERディスクリプタのデータサイズ	*/
	ER_RET	result;					/* 結果											*/

	/* 変数初期化 */
	size	= 0;
	result	= ER_SYS;

	/* DEVICE QUALIFIERディスクリプタの返却 */
	pData  = (void*)(pUsbfInfo->desc.pQualifier);
	size   = (INT)(((DEVICE_DESCRIPTOR*)pData)->bLength);

	/* データサイズが要求サイズより大きい場合 */
	if (size > (INT)pDeviceRequest->wLength)
	{
		size = (INT)pDeviceRequest->wLength;
	}

	result = UsbFuncWriteEP0( pData, size );
	if( result < 0 )
	{
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATIONディスクリプタの先頭アドレス	*/
	DESC_CONFIG		ConfigDescWork;	/* CONFIGURATIONディスクリプタコピー用ワーク	*/
	INT				size;			/* CONFIGURATIONディスクリプタのデータサイズ	*/
	ER_RET			result;			/* 結果											*/

	/* 変数初期化 */
	size	= 0;
	result	= ER_SYS;

	/* CONFIGURATIONディスクリプタの返却 */
	if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE )
	{
		/* 現在HighSpeed(Full Speedデスクリプタを返却) */
		pConfigDesc = pUsbfInfo->desc.pConf_fs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}
	else
	{
		/* 現在FullSpeed（High Speedデスクリプタを返却）*/
		pConfigDesc = pUsbfInfo->desc.pConf_hs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}

	/* データをコピーして、ディスクリプタタイプを変更する */
	memcpy(&ConfigDescWork, pConfigDesc, sizeof(DESC_CONFIG));
	ConfigDescWork.conf_desc.bDescriptorType = (UINT8)0x07;

	/* データサイズが要求サイズより大きい場合 */
	if (size > (INT)pDeviceRequest->wLength)
	{
		size = (INT)pDeviceRequest->wLength;
	}

	result = UsbFuncWriteEP0( (UCHAR*)&ConfigDescWork, size );
	if( result < 0 )
	{
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_INTERFACE処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR*	pIfDesc;	/* INTERFACEディスクリプタの先頭アドレス	*/
	UINT8					data;		/* 返却データワーク							*/
	ER_RET					result;		/* 結果										*/

	/* 変数初期化 */
	pIfDesc	= NULL;
	data	= (UINT8)0;
	result	= ER_SYS;

	/* リクエスト内容チェック */
	if( ( 0x0000      != pDeviceRequest->wValue )	||
		( 0x0001      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x81 != pDeviceRequest->bmRequestType  ) )
	{
		return ER_SYS;
	}

	/* デバイス状態がCONFIGUREDか確認 */
	if( UDEV_ST_CONFIGURED == udev_get_DeviceState() )
	{
		/* INTERFACEディスクリプタを検索 */
		pIfDesc = info_find_InterfaceDesc( pDeviceRequest->wIndex, pUsbfInfo );
		if( pIfDesc != NULL )
		{
			data = pIfDesc->bAlternateSetting;
			result = UsbFuncWriteEP0( (UCHAR*)&data, 1 );
			if( result < 0 )
			{
				return ER_SYS;
			}
			else
			{
				return ER_OK;
			}
		}
	}

	return ER_SYS;
}

/***************************************************************************
  Description: EP0 GET_STATUS処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT8	recipient;				/* 処理ターゲット	*/
	UINT8	direction;				/* 入出力方向		*/
	ER_RET  result;					/* 結果				*/

	/* 変数初期化 */
	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	result = ER_SYS;

	/* リクエスト内容チェック */
	if( ( 0x0000 != pDeviceRequest->wValue )	||
		( 0x0002 != pDeviceRequest->wLength )	||
		( USB_DIR_IN != direction ) )
	{
		return ER_SYS;
	}

	switch( recipient )
	{
		case USB_TARGET_DEVICE:
			if( 0x0000 == pDeviceRequest->wIndex )
			{
				result = Get_Status_Device( pDeviceRequest, pUsbfInfo );
			}
			else
			{
				return ER_SYS;
			}
			break;
		case USB_TARGET_INTERFACE:
			result = ER_SYS;
			break;
		case USB_TARGET_ENDPOINT:
			result = Get_Status_EP( pDeviceRequest );
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 GET_STATUS DEVICE処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	data;					/* 返却データ編集用		*/
	ER_RET	result;					/* 結果					*/

	/* 変数初期化 */
	data	= 0;
	result	= ER_SYS;

	/* bmAttributesのSelfPowerd(D6)ビットを送信データのD0へ */
	data = ( (UINT16)(pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) >> 6 ) & (UINT16)BIT0;

	/* リモートウェイクアップはD1へ */
	if( U2F_ENABLE == pUsbfInfo->usb.flag_RemoteWakeup )
	{
		data |= (UINT8)BIT1;
	}
	/* STATUSデータの返却 */
	result = UsbFuncWriteEP0( (UCHAR*)&data, 2 );
	if( result < 0 )
	{
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_STATUS ENDPOINT処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Status_EP( DEVICE_REQUEST* pDeviceRequest )
{
	UINT	num;					/* EP番号				*/
	UINT8	direction;				/* 方向					*/
	INT		status;					/* STALL状態			*/
	UINT16	data;					/* 返却データ編集用		*/
	ER_RET	result;					/* 結果					*/
	UINT16	wIndex;					/* インデックス値		*/
	UINT	endpoint;				/* エンドポイント番号	*/

	/* 変数初期化 */
	num			= (UINT)( (pDeviceRequest->wIndex) & ~(UINT32)USB_DIR );
	direction	= (UINT8)(pDeviceRequest->wIndex & USB_DIR);
	data		= 0;
	result		= ER_SYS;
	wIndex		= pDeviceRequest->wIndex;
	endpoint	= (UINT)(wIndex & 0xFF);

	/* エンドポイント番号チェック */
	if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
		 ( 0x0000 != ( wIndex & 0x0070 ) ) )
	{
		return ER_SYS;
	}

	if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) )
	{
		/* EP0 */
		data = 0;
	}
	else
	{
		/* Other EP */
		if( UDEV_ST_CONFIGURED == udev_get_DeviceState() )
		{
			/* 方向チェック */
			if( ER_OK != Check_USB_DIR_EPn( num, direction ) )
			{
				return ER_SYS;
			}

			status = EPn_get_STALL( num );	/* STALL状態取得 */
			if( status < 0 )
			{
				return ER_SYS;
			}
			if( 0 == status )
			{
				data = 0;			/* EPnはSTALLしていない	*/
			}
			else
			{
				data = 1;			/* EPnはSTALLしている	*/
			}
		}
		else
		{
			return ER_SYS;
		}
	}

	result = UsbFuncWriteEP0( (UCHAR*)&data, 2 );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SET_ADDRESS処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_address( DEVICE_REQUEST* pDeviceRequest )
{
	UINT16	wValue;					/* アドレス値			*/

	/* 変数初期化 */
	wValue = pDeviceRequest->wValue;

	/* リクエスト内容チェック */
	if( ( pDeviceRequest->bmRequestType != (UINT8)0x00 )	||
		( pDeviceRequest->wIndex != 0x0000 )		||
		( pDeviceRequest->wLength != 0x0000 ) )
	{
		return ER_SYS;
	}

	/* アドレス値チェック */
	if( wValue != ( wValue & 0x007F ) )
	{
		return ER_SYS;
	}

	ARMPF_USB_ADDRESS = (UINT32)wValue << 16;

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SET_CONFIGURATION処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	ConfigValue;			/* CONFIGURATION番号	*/
	UINT32	tmp;					/* 設定編集用ワーク		*/
	ER_RET	result;					/* 結果					*/

	/* 変数初期化 */
	ConfigValue	= pDeviceRequest->wValue & 0x00ff;
	result		= ER_SYS;

	/* リクエスト内容チェック */
	if( ( 0x0000      != pDeviceRequest->wIndex )	||
		( 0x0000      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x00 != pDeviceRequest->bmRequestType  ) )
	{
		return ER_SYS;
	}

	/* CONFIGURATION番号毎の処理を行う */
	switch( ConfigValue )
	{
		case 0:
			/* Addressステートに戻る */
			pUsbfInfo->usb.curr_config = 0;
			ARMPF_USB_CONTROL &= ~D_USB_CONF;

			/* --- EP バッファ・クリア --- */
			ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							/* EP1 : バッファ・クリア		*/
			ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							/* EP2 : バッファ・クリア		*/
			/* --- PID初期化 --- */
			ARMPF_EP1_CONTROL |= D_EPn_IPIDCLR;							/* EP1 : 送信PIDクリア			*/
			ARMPF_EP2_CONTROL |= D_EPn_OPIDCLR;							/* EP2 : 受信PIDクリア			*/
			/* --- EP STALL解除 --- */
			tmp = ARMPF_EP1_CONTROL;
			tmp &= ~(D_EPn_ISTL);									/* EP1 : INトークン STALL解除	*/
			ARMPF_EP1_CONTROL = tmp;

			tmp = ARMPF_EP2_CONTROL;
			tmp |= D_EPn_OSTL_EN;									/* EPn_OSTL ビットへの書き込みを有効にする	*/
			tmp &= ~(D_EPn_OSTL);									/* EP2 : OUT/PINGトークン STALL解除			*/
			ARMPF_EP2_CONTROL = tmp;

			result = ER_OK;
			break;
		case 1:
			/* Configuredステート */
			/* Set Current Configuration Number */
			if (ConfigValue > (UINT16)(pUsbfInfo->desc.pDevice->bNumConfigurations))
			{
				return ER_SYS;
			}
			pUsbfInfo->usb.curr_config = ConfigValue;
			ARMPF_USB_CONTROL |= D_USB_CONF;
			result = ER_OK;
			break;
		default:
			/* サポート外は0を設定 */
			pUsbfInfo->usb.curr_config = 0;
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 SET_DESCRIPTOR処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest )
{
	/* TODO: SET_DESCRIPTORに対応する場合は処理を追加する(現状はSTALL応答する) */
/*	return ER_NOTSUP; */
	return ER_SYS;
}

/***************************************************************************
  Description: EP0 SET_FEATURE処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT8	recipient;				/* 処理ターゲット		*/
	UINT8	direction;				/* 入出力方向			*/
	UINT16	selector;				/* セレクタ値			*/
	UINT16	wIndex;					/* インデックス値		*/
	UINT	endpoint;				/* エンドポイント番号	*/
	ER_RET	result;					/* 結果					*/
	UINT	num;					/* 番号					*/
	UINT8	dir;					/* 方向					*/

	/* 変数初期化 */
	recipient	= (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction	= (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	selector	= pDeviceRequest->wValue;
	wIndex		= pDeviceRequest->wIndex;
	endpoint	= wIndex & 0xFF;
	result		= ER_SYS;

	/* リクエスト内容チェック */
	if( ( 0x0000 != pDeviceRequest->wLength ) ||
		( USB_DIR_OUT != direction ) )
	{
		return ER_SYS;
	}

	/* 処理ターゲット毎に処理を行う */
	switch( recipient )
	{
		case USB_TARGET_DEVICE:
			result = Set_Feature_Device( selector, wIndex, pUsbfInfo );
			break;
		case USB_TARGET_INTERFACE:
			result = ER_SYS;
			break;
		case USB_TARGET_ENDPOINT:
			/* インデックス値チェック */
			if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
				 ( 0x0000 != ( wIndex & 0x0070 ) ) )
			{
				return ER_SYS;
			}
			/* セレクタ値チェック */
			if( USB_FEATURE_ENDPOINT_HALT == selector )
			{
				/* エンドポイント番号チェック */
				if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) )
				{
					/* EP0 */
					/* バッファエンプティ確認 */
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
					{															/* バッファが空でないとき，			*/
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> バッファを空にする。			*/
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> 実際に空になるまでウエイト。	*/
						{
							/* --- ケーブルが抜かれたときは強制終了 --- */
							if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
							{
								return ER_SYS;
							}
						}
					}
				}
				else
				{
					/* Other EP */
					num = endpoint & ~(UINT)USB_DIR;
					dir = (UINT8)(endpoint & USB_DIR);

					/* 方向チェック */
					if( ER_OK != Check_USB_DIR_EPn( num, dir ) )
					{
						return ER_SYS;
					}

					if( ER_OK != Set_Feature_EP( num ) )
					{
						return ER_SYS;
					}
				}
				result = ER_OK;
			}
			else
			{
				return ER_SYS;
			}
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 SET_FEATURE処理( TARGET = DEVICE )
----------------------------------------------------------------------------
  Parameters:
	selector;			USB STD FEATURE Slectors
	wIndex;				インデックス値
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo )
{
	ER_RET	result;					/* 結果					*/
	UINT32	temp;					/* 設定編集用ワーク		*/
	UINT32	tst_mode;				/* 設定編集用ワーク		*/

	/* 変数初期化 */
	result = ER_SYS;
	temp = 0;
	tst_mode = 0;

	switch( selector )
	{
		case USB_FEATURE_DEVICE_REMOTE_WAKEUP:
			if( 0x0000 == wIndex )
			{
				if( (UINT8)0 != ( (pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) & (UINT8)BIT5 ) )
				{	/* BIT5:Remote Wakeup bit */
					/* リモートウェイクアップフラグの更新 */
					pUsbfInfo->usb.flag_RemoteWakeup = U2F_ENABLE;
					result = ER_OK;
				}
				else
				{
					result = ER_SYS;
				}
			}
			else
			{
				return ER_SYS;
			}
			break;
		case USB_FEATURE_TST_MODE:
			if( wIndex <= 0x0005 )
			{
				temp     = ARMPF_USB_CONTROL;									/* CONTROLレジスタの退避	*/
				tst_mode = (UINT32)wIndex;								/* wIndexをUINT32へ			*/
				tst_mode = ( tst_mode << 16 ) & D_USB_USBTESTMODE;		/* TSTMODEビットへシフト	*/
				temp &= ~(UINT32)D_USB_USBTESTMODE;						/* TSTMODE領域を一旦クリア	*/
				temp |= tst_mode;										/* 設定値					*/

				/* レジスタへ書込み */
				ARMPF_USB_CONTROL = temp;
				result = ER_OK;
			}
			else
			{
				result = ER_SYS;
			}
			break;
		case USB_FEATURE_ENDPOINT_HALT:
			result = ER_SYS;
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 SET_FEATURE処理( TARGET = EP )
----------------------------------------------------------------------------
  Parameters:
	num;				EP番号
  Return:
	ER_OK; 				成功
	ER_PARAM;			パラメータエラー
	ER_SYS;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Set_Feature_EP( UINT num )
{
	/* Parameter確認 */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* EP Enable確認 */
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN)
	{
		return ER_SYS;
	}

	/* EPn Clear Feature */
	if( ( udev_get_DeviceState() & UDEV_ST_CONFIGURED ) != 0 )
	{
		/* EP STALL設定 */
		UsbFuncSetStallEPn( num );
	}
	else
	{
		return ER_SYS;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SET_INTERFACE処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR*	pIfDesc;	/* INTERFACEディスクリプタの先頭アドレス	*/

	/* 変数初期化 */
	pIfDesc = NULL;

	/* リクエスト内容チェック */
	if( ( 0x0000      != pDeviceRequest->wLength )	||
	    ( 0x0000      != pDeviceRequest->wValue  )	||			/* Alternate Setting非対応のため，wValueではじく */
	    ( (UINT8)0x01 != pDeviceRequest->bmRequestType  ) )
	{
		return ER_SYS;
	}

	/* コンフィグレーション済みチェック */
	if( UDEV_ST_CONFIGURED == udev_get_DeviceState() )
	{
		pIfDesc = info_find_InterfaceDesc( pDeviceRequest->wIndex, pUsbfInfo );
		if( NULL == pIfDesc )
		{
			return ER_SYS;
		}
	}
	else
	{
		return ER_SYS;
	}

	/*-------------------------------------------------------*/
	/* 指定のInterface番号でデバイスの設定を実施する場合	 */
	/* ここから下に追加										 */
	/*-------------------------------------------------------*/
	/* pIfDesc                = インターフェースデスクリプタ	*/
	/* pDeviceRequest->wIndex = インターフェース番号			*/
	/* pDeviceRequest->wValue = 代替設定値						*/

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SYNCH_FRAME処理
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		デバイスリクエスト
  Return:
	ER_OK; 				成功
	その他;				失敗
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest )
{
	/* TODO: SYNCH_FRAMEに対応する場合は処理を追加する(現状はSTALL応答する) */
/*	return ER_NOTSUP; */
	return ER_SYS;
}

/***************************************************************************
  Description: EP方向のUSB_DIR_OUT/IN変換処理
----------------------------------------------------------------------------
  Parameters:
	num;				EP番号
	direction;			比較方向
  Return:
	ER_OK; 				directionと同じ
	ER_SYS;				directionと違うかエラー
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Check_USB_DIR_EPn( UINT num, UINT8 direction )
{
	UINT8	dir;					/* 方向チェック用ワーク	*/

	/* EPn番号確認 */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* 方向チェック */
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 )
	{	/* OUT */
		dir = USB_DIR_OUT;
	}
	else
	{	/* IN */
		dir = USB_DIR_IN;
	}

	/* directionと比較 */
	if( dir != direction )
	{
		return ER_SYS;
	}

	return ER_OK;
}

/***************************************************************************
  Description: デバイスステート取得処理
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	UDEV_ST_IDLE; 		アイドルステート
	UDEV_ST_DEFAULT;	デフォルトステート
	UDEV_ST_ADDRESS;	アドレスステート
	UDEV_ST_CONFIGURED;	コンフィギュアードステート
	UDEV_ST_SUSPENDED;	サスペンドステート
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static UINT32 udev_get_DeviceState( void )
{
	UINT32	epctr;					/* EPC周辺制御レジスタ値	*/
	UINT32	state;					/* USBステータスレジスタ値	*/

	/* EPC周辺制御レジスタ値取得 */
	epctr	= ARMPF_EPCTR;

	if( 0 == (epctr & D_SYS_VBUS_LEVEL ) )
	{
		/* Idle(Attached)ステート */
		return UDEV_ST_IDLE;
	}

	if( 0 == (epctr & D_SYS_PLL_LOCK ) )
	{
		/* Suspendステート */
		return UDEV_ST_SUSPENDED;
	}

	/* USBステータスレジスタ値取得 */
	state = ARMPF_USB_STATUS;

	if( 0 == ( state & ( D_USB_CONF_ST | D_USB_DEFAULT ) ) )
	{
		return UDEV_ST_IDLE;
	}

	if( ( state & D_USB_SPND_OUT ) != 0 )
	{
		/* Suspendステート */
		return UDEV_ST_SUSPENDED;
	}

	if( ( state & D_USB_CONF_ST ) != 0 )
	{
		/* Configuredステート */
		return UDEV_ST_CONFIGURED;
	}
	else
	{
		if( (ARMPF_USB_ADDRESS & D_USB_USB_ADDR) == D_USB_USB_ADDR )
		{
			/* Address state */
			return UDEV_ST_ADDRESS;
		}
		else
		{
			if( ( state & D_USB_DEFAULT ) != 0 )
			{
				/* Defaultステート */
				return UDEV_ST_DEFAULT;
			}
		}
	}

	return UDEV_ST_IDLE;
}

/***************************************************************************
  Description: info カレントコンフィギュレーションデスクリプタの取得処理
----------------------------------------------------------------------------
  Parameters:
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	カレントコンフィギュレーションデスクリプタ
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static DESC_CONFIG* info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATIONディスクリプタの先頭アドレス	*/

	/* 変数初期化 */
	pConfigDesc = NULL;

	if( pUsbfInfo->usb.curr_config != 0 )
	{
		if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE )
		{	/* High Speed */
			pConfigDesc = pUsbfInfo->desc.pConf_hs;
		}
		else
		{	/* Full Speed */
			pConfigDesc = pUsbfInfo->desc.pConf_fs;
		}
	}
	else
	{
		/* 処理なし */
	}
	return pConfigDesc;
}

/***************************************************************************
  Description: info インターフェースデスクリプタの取得処理
----------------------------------------------------------------------------
  Parameters:
	index;				インターフェース番号
	pUsbfInfo;			USBデバイス情報領域へのポインタ
  Return:
	インターフェースデスクリプタ
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static INTERFACE_DESCRIPTOR* info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATIONディスクリプタの先頭アドレス	*/
	UINT			NumIf;			/* 	*/

	/* 変数初期化 */
	pConfigDesc = NULL;

	/* インターフェース番号チェック */
	if( U2F_IF_MAX <= index )
	{
		return NULL;
	}

	pConfigDesc = info_get_CurrConfigDesc( pUsbfInfo );
	if( NULL == pConfigDesc )
	{
		return NULL;
	}

	/* インターフェースの検索 */
	for( NumIf = 0; NumIf <= (UINT)(pConfigDesc->conf_desc.bNumInterfaces); NumIf++ )
	{
		/* 指定インターフェース番号と一致するかチェック */
		if( index == (UINT16)(pConfigDesc->if_desc[NumIf].bInterfaceNumber) )
		{
			/* found InterfaceDescriptor */
			return (INTERFACE_DESCRIPTOR*)(&pConfigDesc->if_desc[NumIf]);
		}
	}

	return NULL;	/* Not Found. */
}

/***************************************************************************
  Description: EPn STALL状態取得処理
----------------------------------------------------------------------------
  Parameters:
	num;				EP番号
  Return:
	0;					STALLしていない
	1;					STALLしている
	ER_PARAM;			パラメータエラー
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static INT EPn_get_STALL( UINT num )
{
	UINT32 status;

	/* Parameter確認 */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* EP Enable確認 */
	status = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
	if( ( status & D_EPn_EN ) == 0 )
	{
		return ER_PARAM;
	}

	/* Halt（STALL）確認 */
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 )
	{	/* OUT */
		/* STALLチェック */
		if( ( status & ( D_EPn_OSTL ) ) != 0 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{	/* IN */
		/* STALLチェック */
		if( ( status & ( D_EPn_ISTL ) ) != 0 )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
}

/***************************************** end of file **********************************************/
