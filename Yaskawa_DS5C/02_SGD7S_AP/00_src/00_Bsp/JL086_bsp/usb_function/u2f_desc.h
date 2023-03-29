/****************************************************************************
Description	: USB function driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  : none

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.05.12		created
Y.Tanaka	2014.03.17		メンバ変数のvolatile化		<S1FE>

****************************************************************************/
#if !defined(U2F_DESC_H__)
#define U2F_DESC_H__

/*===========================================================================*/
/* I N C L U D E                                                             */
/*===========================================================================*/

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
/* <S150> */
/* type of error code */
typedef int ER_RET;							/**< Function return type will return the error code */

#define	U2F_EPn_MAX								((UINT16)2)
#define U2F_IF_MAX								((UINT16)1)


#define LANG_ID_ENGLISH							((UINT16)0x0409)	/* Language ID = English */
enum {
	STRIDX_LANG = 0,		/* Language Code		*/
	STRIDX_MANUFACTURER,	/* 製造元				*/
	STRIDX_PRODUCT,			/* 製品名				*/
	STRIDX_SERIALNUMBER,	/* シリアル番号			*/
	STRIDX_CONFIGURATION,	/* コンフィグレーション	*/
	STRIDX_INTERFACE,		/* インターフェイス		*/
	STRIDX_MAX
};

/* bmRequestType: bit7 方向 */
#define USB_DIR									((UINT8)0x80)
#define USB_DIR_OUT								((UINT8)0x00)
#define USB_DIR_IN								((UINT8)0x80)

/* bmRequestType: bit6-5 リクエストタイプ */
#define USB_REQUEST								((UINT8)0x60)
#define USB_REQUEST_STANDARD					((UINT8)0x00)
#define USB_REQUEST_CLASS						((UINT8)0x20)
#define USB_REQUEST_VENDOR						((UINT8)0x40)

/* bmRequestType: bit4-0 ターゲットタイプ */
#define USB_TARGET								((UINT8)0x1f)
#define USB_TARGET_DEVICE						((UINT8)0x00)
#define USB_TARGET_INTERFACE					((UINT8)0x01)
#define USB_TARGET_ENDPOINT						((UINT8)0x02)
#define USB_TARGET_OTHER						((UINT8)0x03)

/* bRequest: USB標準リクエスト */
#define USB_STD_GET_STATUS						((UINT8)0)
#define USB_STD_CLEAR_FEATURE					((UINT8)1)
#define USB_STD_SET_FEATURE						((UINT8)3)
#define USB_STD_SET_ADDRESS						((UINT8)5)
#define USB_STD_GET_DESCRIPTOR					((UINT8)6)
#define USB_STD_SET_DESCRIPTOR					((UINT8)7)
#define USB_STD_GET_CONFIGURATION				((UINT8)8)
#define USB_STD_SET_CONFIGURATION				((UINT8)9)
#define USB_STD_GET_INTERFACE					((UINT8)10)
#define USB_STD_SET_INTERFACE					((UINT8)11)
#define USB_STD_SYNCH_FRAME						((UINT8)12)

/* bRequest: クラスリクエスト */
#define USB_CLASS_BULK_ONLY_MASS_STORAGE_RESET	((UINT8)0xff)
#define USB_CLASS_GET_MAX_LUN					((UINT8)0xfe)

/* bRequest: ベンダーリクエスト */
#define USB_VENDER_GET_OWN_ADDRESS				((UINT8)1)

/* USBディスクリプタタイプ */
#define USB_DESC_TYPE_DEVICE					((UINT16)0x0001)
#define USB_DESC_TYPE_CONFIGURATION				((UINT16)0x0002)
#define USB_DESC_TYPE_STRING					((UINT16)0x0003)
#define USB_DESC_TYPE_INTERFACE					((UINT16)0x0004)
#define USB_DESC_TYPE_ENDPOINT					((UINT16)0x0005)
#define USB_DESC_TYPE_DEVICE_QUALIFIER			((UINT16)0x0006)
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION	((UINT16)0x0007)
#define USB_DESC_TYPE_INTERFACE_POWER			((UINT16)0x0008)

/* USB STD FEATURE Slectors */
#define USB_FEATURE_DEVICE_REMOTE_WAKEUP		((UINT16)0x0001)
#define USB_FEATURE_ENDPOINT_HALT				((UINT16)0x0000)
#define USB_FEATURE_TST_MODE					((UINT16)0x0002)

/* USB DESCRIPTOR Byte Length */
#define USB_DESC_LENGTH_CONFIG					((UINT16)9)
#define USB_DESC_LENGTH_INTERFACE				((UINT16)9)
#define USB_DESC_LENGTH_EP						((UINT16)7)


/*===========================================================================*/
/* S T R U C T                                                               */
/*===========================================================================*/
/*-----------------------------*/
/* USBデバイスリクエスト構造体 */
/*-----------------------------*/
typedef struct _DEVICE_REQUEST
{
	VUINT8	bmRequestType;
	VUINT8	bRequest;
	VUINT16	wValue;
	VUINT16	wIndex;
	VUINT16	wLength;
} DEVICE_REQUEST;

/*-----------------------------*/
/* STD Descriptor			   */
/*-----------------------------*/
/* Device Descriptor */
typedef struct _DEVICE_DESCRIPTOR
{
	VUINT8	bLength;
	VUINT8	bDescriptorType;
	VUINT8	bcdUSB[2];
	VUINT8	bDeviceClass;
	VUINT8	bDeviceSubClass;
	VUINT8	bDeviceProtocol;
	VUINT8	bMaxPacketSize0;
	VUINT8	idVendor[2];
	VUINT8	idProduct[2];
	VUINT8	bcdDevice[2];
	VUINT8	iManufacturer;
	VUINT8	iProduct;
	VUINT8	iSerialNumber;
	VUINT8	bNumConfigurations;
} DEVICE_DESCRIPTOR;

/* Device Qualifier Descriptor */
typedef struct _DEVICE_QUALIFIER_DESCRIPTOR
{
	VUINT8	bLength;
	VUINT8	bDescriptorType;
	VUINT8	bcdUSB[2];
	VUINT8	bDeviceClass;
	VUINT8	bDeviceSubClass;
	VUINT8	bDeviceProtocol;
	VUINT8	bMaxPacketSize0;
	VUINT8	bNumConfigurations;
	VUINT8	Reserved;
} DEVICE_QUALIFIER_DESCRIPTOR;

/* Configuration Descriptor / Other Speed Configuration Descriptor */
typedef struct _CONFIGURATION_DESCRIPTOR
{
	VUINT8	bLength;
	VUINT8	bDescriptorType;
	VUINT8	wTotalLength[2];
	VUINT8	bNumInterfaces;
	VUINT8	bConfigurationValue;
	VUINT8	iConfiguration;
	VUINT8	bmAttributes;
	VUINT8	bMaxPower;
} CONFIGURATION_DESCRIPTOR;

/* Endpoint Descriptor */
typedef struct _ENDPOINT_DESCRIPTOR
{
	VUINT8	bLength;
	VUINT8	bDescriptorType;
	VUINT8	bEndpointAddress;
	VUINT8	bmAttributes;
	VUINT8	wMaxPacketSize[2];
	VUINT8	bInterval;
} ENDPOINT_DESCRIPTOR;

/* String Descriptor */
typedef struct _STRING_DESCRIPTOR
{
	VUINT8	bLength;
	VUINT8	bDescriptorType;
	VUINT8	bString[1];			/* dummy */
} STRING_DESCRIPTOR;

/* Interface Descriptor */
typedef struct _INTERFACE_DESCRIPTOR
{
	VUINT8	bLength;
	VUINT8	bDescriptorType;
	VUINT8	bInterfaceNumber;
	VUINT8	bAlternateSetting;
	VUINT8	bNumEndpoints;
	VUINT8	bInterfaceClass;
	VUINT8	bInterfaceSubClass;
	VUINT8	bInterfaceProtocol;
	VUINT8	iInterface;
	ENDPOINT_DESCRIPTOR ep_desc[U2F_EPn_MAX];
} INTERFACE_DESCRIPTOR;

/* Get Descriptor Configuration時に利用 */
typedef struct _DESC_CONFIG
{
	CONFIGURATION_DESCRIPTOR	conf_desc;
	INTERFACE_DESCRIPTOR		if_desc[U2F_IF_MAX];
} DESC_CONFIG;

/* 対応する全Descriptor */
typedef struct _DESCRIPTORS
{
	DEVICE_DESCRIPTOR			*pDevice;
	DEVICE_QUALIFIER_DESCRIPTOR	*pQualifier;
	DESC_CONFIG					*pConf_fs;
	DESC_CONFIG					*pConf_hs;
	STRING_DESCRIPTOR			*pString[STRIDX_MAX];
} DESCRIPTORS;

/*===========================================================================*/
/* S T R U C T                                                               */
/*===========================================================================*/

/* DEV INFORMATION */
typedef struct _DEV_INFO
{
	VINT		ep_num;						/* 利用するEP数									*/
	VUINT32	ep_dir;						/* EPn方向(OUT:1,IN:0) Bit0=EP1 - Bit14=EP15	*/
	VUINT32	ep_mpk[15];					/* EPn MAXパケットサイズ						*/
} DEV_INFO;

/* USB INFORMATION ( Read Only ) */
typedef VUINT32	U2F_CANCEL;
typedef struct _USB_INFO
{
	VUINT32			curr_config;		/* Current Configuration Number		*/
	VUINT32			flag_RemoteWakeup;	/* RemoteWakeup Flag				*/
										/*		(U2F_ENABLE=許可，U2F_DISABLE=禁止) */
	VUINT32			flag_CancelAll;		/* Operation Cancel Flag			*/
										/*		(U2F_ENABLE=キャンセル，U2F_DISABLE=通常）*/
	U2F_CANCEL		flag_CancelEPn;		/* Operation Cancel EPn Flag		*/
										/*		(bit0-14, 1B:キャンセル, 0B:通常）*/
	VUINT32			flag_Connect;		/* USB Plug IN/OUT Flag				*/
										/*		(U2F_PLUG_OUT=プラグイン, U2F_PLUG_OUT=プラグアウト)	*/
	VUINT32			flag_Setup;			/* SETUP data receive Flag			*/
										/*		(SETUP_RECIEVED=SETUP受信, SETUP_NOT_RECIEVED=SETUP未受信) */
	VUINT32			int_en_AHB;			/* AHB interrupt enable				*/
	VUINT32			int_en_USB;			/* USB interrupt enable				*/
	VUINT32			int_en_EP0;			/* EP0 interrupt enable				*/
	VUINT32			int_en_EPnI;		/* EPn IN  interrupt enable			*/
	VUINT32			int_en_EPnO;		/* EPn OUT interrupt enable			*/
	DEVICE_REQUEST	request;			/* DeviceRequest					*/
} USB_INFO;

typedef struct _USB_FLAG
{
	VUINT8			UsbPlugStatus;		/* USB Plug IN/OUT Flag				*/
										/*		(U2F_PLUG_OUT=プラグイン, U2F_PLUG_OUT=プラグアウト)	*/
	VUINT8			UsbSetupReceive;	/* SETUP data receive count			*/
	VUINT8			UsbSetupSave;		/* SETUP UsbSetupRecieve Saved		*/
	VUINT8			UsbEp1InCount;		/* EP1 IN count						*/
	VUINT8			UsbEp2OutCount;		/* EP2 OUT count					*/
} USB_FLAG;

typedef ER_RET (*URQ_Callback)(DEVICE_REQUEST* pDeviceRequest);
/* U2F INFORMATION */
typedef struct _U2F_INFO
{
	DESCRIPTORS		desc;				/* Descriptor						*/
	DEV_INFO		dev;				/* Device Information				*/
	USB_INFO		usb;				/* USB Information					*/
	USB_FLAG		usb_f;				/* USB Flag							*/
	URQ_Callback	cb_class_req;		/* Class request callback function	*/
	URQ_Callback	cb_vendor_req;		/* Vendor request callback function	*/
	VULONG			MyAddress;			/* My Address						*/
} U2F_INFO;

typedef struct _USB_FUNCTION_IF
{
	U2F_INFO		UsbfInfo;			/* USBファンクション情報			*/
	VUCHAR			Ep1Status;			/* Endpoint1(OUT用)状態				*/
	VUCHAR			Ep2Status;			/* Endpoint2( IN用)状態				*/
	VUCHAR			*OutBufPtr;			/* 受信バッファポインタ				*/
	VUCHAR			*InBufPtr;			/* 送信バッファポインタ				*/
	VUSHORT			OutLengthReq;		/* 受信要求バイト数					*/
	VUSHORT			OutLength;			/* 受信済みバイト数					*/
	VUSHORT			InLengthRem;		/* 送信残りバイト数					*/
	VUSHORT			InLength;			/* 送信済みバイト数					*/
	USB_FLAG		UsbfFlag;			/* 各種状態							*/
/* <S182> >>>>> */
	VUINT32			OutPacketSize;		/* 受信パケットサイズ(保存用)		*/
	VUINT32			OutCopyRem;			/* 受信データ残サイズ				*/
	BOOL			OutSplitCopy;		/* 受信分割コピーフラグ				*/
	VUINT32			InCopyRem;			/* 送信データ残サイズ				*/
	BOOL			InSplitCopy;		/* 送信分割コピーフラグ				*/
/* <<<<< <S182> */
} USB_FUNCTION_IF;

#endif /* U2F_DESC_H__ */
/*-------end of file----------------------------------------------------------------------------------------*/
