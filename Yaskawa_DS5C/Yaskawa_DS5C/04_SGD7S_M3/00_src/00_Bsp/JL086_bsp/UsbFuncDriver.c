/****************************************************************************
Description	: USB function driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	-- APIs --
	UsbFuncInitializeDriver()		: USB�h���C�o����������
	UsbFuncChkAvailable()			: USB�g�p�\���菈��	<S138>
	 - ���荞�݊֘A -
	UsbFuncIntIntu2f()				: INTU2F ���荞�ݏ���
	UsbFuncIntIntu2fepc()			: INTU2FEPC ���荞�ݏ���
	 - �R���g���[���]���֘A -
	UsbFuncTaskControl()			: �R���g���[���]���^�X�N����
	 - �o���N�]���֘A���� -
	UsbFuncPollBulkOutIn()			: USB �o���N�]�����䏈��
	UsbFuncResetBuffer()			: USB Endpoint1/2 ����M�o�b�t�@�N���A����
	 - USB MEMOBUS�T�[�o�[�p�֘A -
	UsbFuncStartBkout()				: USB Endpoint2 ��M�J�n(BLUK OUT)����
	UsbFuncStartBkin()				: USB Endpoint1 ���M�J�n(BLUK IN)����
	UsbFuncPortConfiguration()		: USB�|�[�g��Ԓʒm�T�[�r�X����
	UsbFuncGetRcvSize()				: USB Endpoint2 ��M�o�C�g���擾����
	UsbFuncChkRcvComp()				: �o���N�]��(OUT)��Ԓʒm�T�[�r�X����
	UsbFuncChkTrnsComp()			: �o���N�]��(IN)��Ԓʒm�T�[�r�X����

	-- Locals --
	VendorRequest_Callback()		: �x���_�[���N�G�X�g�R�[���o�b�N����
	UsbFuncInitializeInfo()			: USB�t�@���N�V������񏉊�������
	UsbFuncClearFlag()				: �t���O�E�N���A����
	UsbFuncSetMaxpacket()			: Max Packet Size �ݒ菈��
	UsbFuncGetInfoAddress()			: USB���̈�̃A�h���X�擾����
	UsbFuncCheckPlugInout()			: �ڑ���Ԋm�F����
	UsbFuncCheckSetupReceive()		: SETUP�g�[�N����M�m�F����
	 - EPn�]���֘A -
	UsbFuncSetStallEPn()			: EPn �X�g�[����������
	UsbFuncOutDataSplitCopy()		: OUT�f�[�^�����R�s�[���� <S182>
	UsbFuncInDataSplitCopy()		: IN�f�[�^�����R�s�[����	 <S182>
	 - �R���g���[���]���֘A -
	UsbFuncWriteEP0()				: EP0 ���M����
	UsbFuncSetStallEP0()			: EP0 �X�g�[����������
	UsbFuncSetNakEP0()				: EP0 NAK��������
	UsbFuncClearNakEP0()			: EP0 NAK��������
	UsbFuncSendNullEP0()			: EP0 NULL���M����
	control_nodata()				: EP0 No Data����
	control_read()					: EP0 Control Read����
	control_write()					: EP0 Control Write����
	decode_request()				: EP0 ���N�G�X�g�f�R�[�h����
	bmRequestType_standard()		: EP0 �X�^���_�[�h���N�G�X�g����
	std_req_clear_feature()			: EP0 CLEAR_FEATURE����
	Clear_Feature_EP()				: EP0 CLEAR_FEATURE����( TARGET = EP )
	std_req_get_configuration()		: EP0 GET_CONFIGURATION����
	std_req_get_descriptor()		: EP0 GET_DESCRIPTOR����
	Get_Descriptor_Device()			: EP0 GET_DESCRIPTOR DEVICE����
	Get_Descriptor_Configuration()	: EP0 GET_DESCRIPTOR CONFIGURATION����
	Get_Descriptor_String()			: EP0 GET_DESCRIPTOR STRING����
	Get_Descriptor_Qualifier()		: EP0 GET_DESCRIPTOR DEVICE QUALIFIER����
	Get_Descriptor_OtherSpeed()		: EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION����
	std_req_get_interface()			: EP0 GET_INTERFACE����
	std_req_get_status()			: EP0 GET_STATUS����
	Get_Status_Device()				: EP0 GET_STATUS DEVICE����
	Get_Status_EP()					: EP0 GET_STATUS ENDPOINT����
	std_req_set_address()			: EP0 SET_ADDRESS����
	std_req_set_configuration()		: EP0 SET_CONFIGURATION����
	std_req_set_descriptor()		: EP0 SET_DESCRIPTOR����
	std_req_set_feature()			: EP0 SET_FEATURE����
	Set_Feature_Device()			: EP0 SET_FEATURE����( TARGET = DEVICE )
	Set_Feature_EP()				: EP0 SET_FEATURE����( TARGET = EP )
	std_req_set_interface()			: EP0 SET_INTERFACE����
	std_req_synch_frame()			: EP0 SYNCH_FRAME����
	Check_USB_DIR_EPn()				: EP������USB_DIR_OUT/IN�ϊ�����
	udev_get_DeviceState()			: �f�o�C�X�X�e�[�g�擾����
	info_get_CurrConfigDesc()		: info �J�����g�R���t�B�M�����[�V�����f�X�N���v�^�̎擾����
	info_find_InterfaceDesc()		: info �C���^�[�t�F�[�X�f�X�N���v�^�̎擾����
	EPn_get_STALL()					: EPn STALL��Ԏ擾����

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
#define BULK_MAXPACKET_SIZE_HS	 512					/* High Speed���o���N�]���ő�T�C�Y		*/
#define BULK_MAXPACKET_SIZE_FS	  64					/* Full Speed���o���N�]���ő�T�C�Y		*/
#define INT_MAXPACKET_SIZE_HS	1024					/* High Speed�����荞�ݓ]���ő�T�C�Y	*/
#define INT_MAXPACKET_SIZE_FS	  64					/* Full Speed�����荞�ݓ]���ő�T�C�Y	*/

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
#define UDEV_ST_IDLE		BIT0						/* Idle(Attached)�X�e�[�g	*/
#define UDEV_ST_DEFAULT		BIT1						/* Default�X�e�[�g			*/
#define UDEV_ST_ADDRESS		BIT2						/* Address�X�e�[�g			*/
#define UDEV_ST_CONFIGURED	BIT3						/* Configured�X�e�[�g		*/
#define UDEV_ST_SUSPENDED	BIT4						/* Suspend�X�e�[�g			*/

/* U2F FLAG	*/
#define U2F_ENABLE			1							/* ����						*/
#define U2F_DISABLE			0							/* �֎~						*/

/* ����M�f�[�^�R�s�[���@1�X�L�����ł̃R�s�[�T�C�Y */ /*<S182>*/
#define SCAN_COPY_SIZE		64							/* 1�X�L�����ł̃R�s�[�T�C�Y(4*n)(�ύX�֎~)	*/

/*===========================================================================*/
/* globals in this file														 */
/*===========================================================================*/
static USB_FUNCTION_IF	UsbfIf;							/* �h���C�o�����f�[�^					*/
static BOOL				UsbFunctionAvailable;			/* USB�g�p�\����t���O				*//* <S138> */

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
/*	FIXME:�f�o�b�O�p���u����(����m�肳����K�v�L��) */
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
/*		GET_DESCRIPTOR String �ɂĕԐM����String�f�B�X�N���v�^��`									*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		����h�c																					*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR LanguageIdStringDescriptor[]=
{
4,				/* �����ި�������̻���(byte)														*/
0x03,			/* �ި������������(0x03�Œ�)														*/
0x09,0x04		/* 0x0409 : ID=�Č�																	*/
};

/*--------------------------------------------------------------------------------------------------*/
/*		������																						*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR ImanufacturerStringDescriptor[]=
{
58,		/* �����ި�������̻���(byte) */
0x03,	/* �ި������������(0x03�Œ�) */
/* ������ */
/* YASKAWA ELECTRIC CORPORATION */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'Y', 0 ,'A', 0 ,'S', 0 ,'K', 0 ,'A', 0 ,'W', 0 ,'A', 0 ,' ', 0 ,
	'E', 0 ,'L', 0 ,'E', 0 ,'C', 0 ,'T', 0 ,'R', 0 ,'I', 0 ,'C', 0 ,
	' ', 0 ,'C', 0 ,'O', 0 ,'R', 0 ,'P', 0 ,'O', 0 ,'R', 0 ,'A', 0 ,
	'T', 0 ,'I', 0 ,'O', 0 ,'N', 0
};

/*--------------------------------------------------------------------------------------------------*/
/*		���i																						*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IproductStringDescriptor[]=
{
42,		/* �����ި�������̻���(byte) */
0x03,	/* �ި������������(0x03�Œ�) */
/* ������ */
/* YASKAWA SIGMA SERIES */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'Y', 0 ,'A', 0 ,'S', 0 ,'K', 0 ,'A', 0 ,'W', 0 ,'A', 0 ,' ', 0 ,
	'S', 0 ,'I', 0 ,'G', 0 ,'M', 0 ,'A', 0 ,' ', 0 ,'S', 0 ,'E', 0 ,
	'R', 0 ,'I', 0 ,'E', 0 ,'S', 0
};
/*--------------------------------------------------------------------------------------------------*/
/*		�V���A���ԍ�																				*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IserialNumberStringDescriptor[]=
{
34,		/* �����ި�������̻���(byte) */
0x03,	/* �ި������������(0x03�Œ�) */
/* ������ */
/* 0000000000000000 (16��׸�) */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,
	'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0 ,'0', 0
};
/*--------------------------------------------------------------------------------------------------*/
/*		�R���t�B�O���[�V����																		*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IconfigurationStringDescriptor[]=
{
54,	/* �����ި�������̻��� (byte) */
0x03,	/* �ި������������(0x03�Œ�) */
/* ������ */
/* Sigma ACservo Basic Config */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'S', 0 ,'i', 0 ,'g', 0 ,'m', 0 ,'a', 0 ,' ', 0 ,'A', 0 ,'C', 0 ,
	's', 0 ,'e', 0 ,'r', 0 ,'v', 0 ,'o', 0 ,' ', 0 ,'B', 0 ,'a', 0 ,
	's', 0 ,'i', 0 ,'c', 0 ,' ', 0 ,'C', 0 ,'o', 0 ,'n', 0 ,'f', 0 ,
	'i', 0 ,'g', 0
};
/*--------------------------------------------------------------------------------------------------*/
/*		�C���^�[�t�F�C�X																			*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR IinterfaceStringDescriptor[]=
{
24,		/* �����ި�������̻���(byte) */
0x03,	/* �ި������������(0x03�Œ�) */
/* ������ */
/* 2 Endpoints */
/*   0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
	'2', 0 ,' ', 0 ,'E', 0 ,'n', 0 ,'d', 0 ,'p', 0 ,'o', 0 ,'i', 0 ,
	'n', 0 ,'t', 0 ,'s', 0
};

/*--------------------------------------------------------------------------------------------------*/
/*		USB�t�@���N�V�����p���荞�ݏ����o�^�e�[�u��													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /*<S146> ���荞�ݐݒ�͂��Ȃ��Ȃ������ߍ폜 */
/*	FIXME:�������x����INTC_PRL_LVL7�ŗǂ����H */
const tINTC_REGIST	intc_regist_table_u2f[2] =
{
	{INTC_IDX_U2F_INT,		UsbFuncIntIntu2f,		INTC_LEVEL_HI,	INTC_PRL_LVL7, 0},	/* INTU2F ���荞�ݏ����p	*/
	{INTC_IDX_U2F_EPC_INT,	UsbFuncIntIntu2fepc,	INTC_LEVEL_HI,	INTC_PRL_LVL7, 0}	/* INTU2FEPC ���荞�ݏ���	*/
};
#endif

/*===========================================================================*/
/* PROTOTYPE																 */
/*===========================================================================*/
/* APIs	*/

/* Locals	*/
static	ER_RET	VendorRequest_Callback( DEVICE_REQUEST* pDeviceRequest );	/* �x���_�[���N�G�X�g�R�[���o�b�N����	*/
static	void	UsbFuncInitializeInfo( U2F_INFO *pUsbfInfo );				/* USB�t�@���N�V������񏉊�������		*/
static	void	UsbFuncClearFlag( USB_FLAG *pUsbfFlag );					/* �t���O�E�N���A����					*/
static	void	UsbFuncSetMaxpacket ( void );								/* Max Packet Size �ݒ菈��				*/
static	USB_FUNCTION_IF*	UsbFuncGetInfoAddress( void );					/* USB���̈�̃A�h���X�擾����		*/
static	UINT	UsbFuncCheckPlugInout( void );								/* USB�ڑ���Ԋm�F����					*/
static	UINT	UsbFuncCheckSetupReceive( void );							/* SETUP�g�[�N����M�m�F����			*/

/* EPn�]���֘A	*/
static	void	UsbFuncSetStallEPn( UINT num );								/* EPn �X�g�[����������					*/
static	void	UsbFuncOutDataSplitCopy( USB_FUNCTION_IF *pUsbfIf );		/* OUT�f�[�^�����R�s�[����					*/ /*<S182>*/
static	void	UsbFuncInDataSplitCopy( USB_FUNCTION_IF *pUsbfIf );			/* IN�f�[�^�����R�s�[����					*/ /*<S182>*/

/* �R���g���[���]���֘A	*/
static	INT		UsbFuncWriteEP0( VUCHAR* pBuf, INT size );					/* EP0 ���M����							*/
static	void	UsbFuncSetStallEP0( void );									/* EP0 �X�g�[����������					*/
#if 0	/* �����_�ł͎g�p���Ȃ����� <S150>*/
static	void	UsbFuncSetNakEP0( void );									/* EP0 NAK��������						*/
static	void	UsbFuncClearNakEP0( void );									/* EP0 NAK��������						*/
static	ER_RET	UsbFuncSendNullEP0( void );									/* EP0 NULL���M����						*/
#endif

/*-------------------------------------*/
/* ���N�G�X�g�E�^�C�v�ʏ���			   */
/*-------------------------------------*/
static	ER_RET	control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 No Data����			*/
static	ER_RET	control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 Control Read����		*/
static	ER_RET	control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 Control Write����	*/

/*-------------------------------------*/
/* �X�^���_�[�h�E���N�G�X�g����		   */
/*-------------------------------------*/
static	ER_RET	decode_request( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 ���N�G�X�g�f�R�[�h���� */

static	ER_RET	bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 �X�^���_�[�h���N�G�X�g���� */
/* --- CLEAR_FEATURE     ( 1) [No data] --- */
static	ER_RET	std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest );								/* EP0 CLEAR_FEATURE���� */
static	ER_RET	Clear_Feature_EP( UINT num );															/* EP0 CLEAR_FEATURE����( TARGET = EP ) */
/* --- GET_CONFIGURATION ( 8) [Read]    --- */
static	ER_RET	std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_CONFIGURATION���� */
/* --- GET_DESCRIPTOR    ( 6) [Read]    --- */
static	ER_RET	std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR���� */
static	ER_RET	Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR DEVICE���� */
static	ER_RET	Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 GET_DESCRIPTOR CONFIGURATION���� */
static	ER_RET	Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR STRING���� */
static	ER_RET	Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR DEVICE QUALIFIER���� */
static	ER_RET	Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION���� */
/* --- GET_INTERFACE     (10) [Read]    --- */
static	ER_RET	std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_INTERFACE���� */
/* --- GET_STATUS        ( 0) [Read]    --- */
static	ER_RET	std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS���� */
static	ER_RET	Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS DEVICE���� */
static	ER_RET	Get_Status_EP( DEVICE_REQUEST* pDeviceRequest );										/* EP0 GET_STATUS ENDPOINT���� */
/* --- SET_ADDRESS       ( 5) [No data] --- */
static	ER_RET	std_req_set_address( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SET_ADDRESS���� */
/* --- SET_CONFIGURATION ( 9) [No data] --- */
static	ER_RET	std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 SET_CONFIGURATION���� */
/* --- SET_DESCRIPTOR    ( 7) [Write]   --- */
static	ER_RET	std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest );								/* EP0 SET_DESCRIPTOR���� */
/* --- SET_FEATURE       ( 3) [No data] --- */
static	ER_RET	std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 SET_FEATURE���� */
static	ER_RET	Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo );				/* EP0 SET_FEATURE����( TARGET = DEVICE ) */
static	ER_RET	Set_Feature_EP( UINT num );																/* EP0 SET_FEATURE����( TARGET = EP ) */
/* --- SET_INTERFACE     (11) [No data] --- */
static	ER_RET	std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 SET_INTERFACE���� */
/* --- SYNC_FRAME        (12) [No data] --- */
static	ER_RET	std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SYNCH_FRAME���� */

static	ER_RET	Check_USB_DIR_EPn( UINT num, UINT8 direction );											/* EP������USB_DIR_OUT/IN�ϊ����� */
static	UINT32	udev_get_DeviceState( void );															/* �f�o�C�X�X�e�[�g�擾���� */
static	DESC_CONFIG*			info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo );							/* info �J�����g�R���t�B�M�����[�V�����f�X�N���v�^�̎擾���� */
static	INTERFACE_DESCRIPTOR*	info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo );			/* info �C���^�[�t�F�[�X�f�X�N���v�^�̎擾���� */
static	INT						EPn_get_STALL( UINT num );												/* EPn STALL��Ԏ擾���� */


/*===========================================================================*/
/* PROGRAM																	 */
/*===========================================================================*/

/***************************************************************************
  Description: USB�h���C�o����������
----------------------------------------------------------------------------
  Parameters:
	MyAddress;			���ǃA�h���X
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
void	UsbFuncInitializeDriver( ULONG MyAddress )
{
	UINT32			state;					/* ��ԑ���p					*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^			*/
	USB_FLAG		*pUsbfFlag;				/* USB�t���O�̈�|�C���^		*/
//	INT				i;						/* ���[�v�J�E���^				*//*<S0C7>*/
	ULONG			StartTime;				/* �^�C�}�p���[�N				*//*<S138>*/

	UsbFunctionAvailable = FALSE;									/* USB�g�p�s�ݒ�(Rev.A��Ή�) *//*<S138>*/

	pUsbfIf = UsbFuncGetInfoAddress();								/* USB���̈�̃A�h���X�擾	*/

	MlibResetLongMemory( pUsbfIf, sizeof( USB_FUNCTION_IF )/4  );	/* �Ǘ��������N���A				*/

	pUsbfIf->UsbfInfo.MyAddress = MyAddress;						/* ���ǃA�h���X�ݒ�				*/

	pUsbfFlag = &(pUsbfIf->UsbfFlag);
	UsbFuncClearFlag( &(pUsbfIf->UsbfFlag) );						/* �t���O�N���A					*/

	UsbFuncInitializeInfo( &(pUsbfIf->UsbfInfo) );					/* �N���X��`������				*/

	/*----------------------------------------------------------------------------------------------*/
	/*		EPC, PLL ���Z�b�g����																	*/
	/*----------------------------------------------------------------------------------------------*/
	ARMPF_EPCTR &= ~(D_SYS_EPC_RST | D_SYS_PLL_RST);				/* ���Z�b�g���� */

	/*----------------------------------------------------------------------------------------------*/
	/*		System Bus-EPC Bridge �����ݒ�															*/
	/*----------------------------------------------------------------------------------------------*/
	ARMPF_SYSSCTR   = D_SYS_WAIT_MODE;								/* �X���[�u�̃E�G�C�g����w��	*/
#if 1	/* XXX:DMA�]���g�p���͗L���ɂ��� */ /* <S026> �������w������̂��ߗL���� */
	ARMPF_SYSMCTR   = 0x00000034;									/* DMA�]�����̃A�[�r�g���[�V����(bit 31)�F���E���h���r�� */
#endif
#if 0	/* <S150> VBUS���荞�݂��g�p���Ȃ�(�|�[�����O��) */
	ARMPF_SYSBINTEN = D_SYS_VBUS_INT;								/* VBUS���荞�݋���						*/
#endif

	/*----------------------------------------------------------------------------------------------*/
	/*		EPC �����ݒ�																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* --- PLL_LOCK�҂� --- */
	KlibRstLongTimer(&StartTime);		/* �v���J�n *//*<S138>*/
	while ((ARMPF_EPCTR & D_SYS_PLL_LOCK) == 0)
	{
		/* !!!!!!!! ToDo PLL LOCK�҂��ُ펞���� !!!!!!!! */
// <S138> >>>>>
		if( KlibGetLongTimerUs(StartTime) > 2000 )
		{
			return;
		}
		KlibWaitus(100);
// <<<<< <S138>
	}

	UsbFunctionAvailable = TRUE;							/* USB�g�p�\�ݒ�(Rev.A��Ή�) *//*<S138>*/

	/* --- ���荞�ݐݒ� --- */
	/* USB���䃌�W�X�^	*/
#if 1	/* XXX:High Speed ���p���́A�������L���ɂ��� */
	ARMPF_USB_CONTROL   = D_USB_INT_SEL | D_USB_SOF_RCV;			/* INTU2FEPC���荞�݂����x�����荞�݂ɐݒ�	*/
															/* SOF ��M�G���[�������Ɏ������J�o���@�\���g�p	*/
#else
	ARMPF_USB_CONTROL   = D_USB_INT_SEL | D_USB_SOF_RCV | D_USB_CONSTFS;	/* ��L �{ FULL Speed�Œ�	*/
#endif
	/* USB���荞�݋����W�X�^	*/
#if 0	/* <S150> ���荞�݂��g�p�����A�|�[�����O�ɕύX */
	ARMPF_USB_INT_ENA   = D_USB_USB_RST_EN    |					/* ���荞�݋��F�o�X�E���Z�b�g			*/
					D_USB_SPEED_MODE_EN |					/*             �F�o�X�E�X�s�[�h�ύX		*/
					D_USB_EP0_EN        |					/*             �FEP0 ���荞��			*/
					D_USB_EP1_EN        |					/*             �FEP1 ���荞��			*/
					D_USB_EP2_EN;							/*             �FEP2 ���荞��			*/
#endif

	/* --- Max Packet�ݒ� --- */
	UsbFuncSetMaxpacket();

	/* --- �eEP �o�b�t�@�E�N���A --- */
	/* EPn���䃌�W�X�^	*/
	ARMPF_EP0_CONTROL   = D_EP0_BCLR;								/* EP0 : �o�b�t�@�E�N���A				*/
	ARMPF_EP1_CONTROL   = D_EPn_BCLR;								/* EP1 : Bulk-in,  �o�b�t�@�E�N���A		*/
	ARMPF_EP2_CONTROL   = D_EPn_BCLR | D_EPn_DIR0;				/* EP2 : Bulk-out, �o�b�t�@�E�N���A		*/

	/* --- �eEP ���荞�݋��� --- */
	/* EPn���荞�݋����W�X�^	*/
#if 0	/* <S150> ���荞�݂��g�p�����A�|�[�����O�ɕύX	*/
	ARMPF_EP0_INT_ENA   = D_EP0_SETUP_EN;							/* EP0 : ���荞�݋��FSETUP_INT					*/
#endif
#if 0	/* XXX:�]���������荞�݂𗘗p����ꍇ�ɗL���ɂ���i�L���ɂ��鍀�ڂ͍Č������K�v�j */
	ARMPF_EP1_INT_ENA   = D_EPn_IN_EN | D_EPn_IN_END_EN;			/* EP1 : ���荞�݋��FEP1_IN_INT, EP1_IN_END_INT	*/
															/*	D_EPn_IN_EN		: IN�]���������荞�݋���		*/
															/*	D_EPn_IN_END_EN	: IN DMA�]���������荞�݋���	*/
	ARMPF_EP2_INT_ENA   = D_EPn_OUT_EN | D_EPn_OUT_END_EN;		/* EP2 : ���荞�݋��FEP2_OUT_INT, EP2_OUT_END_INT	*/
															/*	D_EPn_OUT_EN	: OUT�]���������荞�݋���		*/
															/*	D_EPn_OUT_END_EN: OUT DMA�]���������荞�݋���	*/
#endif

	/* --- �eEP �L���� --- */
	ARMPF_EP1_CONTROL  |= D_EPn_EN;								/* EP1 : �L����		*/
	ARMPF_EP2_CONTROL  |= D_EPn_EN;								/* EP2 : �L����		*/

	/*=========================================*/
	/* �v���O�C���^�A�E�g���� (�N������Ԋm�F) */
	/*=========================================*/
	if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL)		/* VBUS���x���m�F	*/
	{
		/* ====== �v���O�C������ ======	*/
		state       =  ARMPF_USB_CONTROL;							/* USB���䃌�W�X�^�l�擾						*/
		state      &= ~D_USB_CONNECTB;						/* �iFunction PHY �ւ�USB �M����L���ɂ���j	*/
		state      |=  D_USB_PUE2;							/* �iD+�M����Pull-up ����j						*/
		ARMPF_USB_CONTROL =  state;								/* ��L����𓯎��ɔ��f							*/
		pUsbfFlag->UsbPlugStatus = U2F_PLUG_IN;				/* �v���O��Ԃ��v���O�C���ɍX�V					*/
	}
	else
	{
		/* ====== �v���O�A�E�g���� ====== */
		state       =  ARMPF_USB_CONTROL;							/* USB���䃌�W�X�^�l�擾						*/
		state      |=  D_USB_CONNECTB;						/* �iFunction PHY �ւ�USB �M���𖳌��ɂ���j	*/
		state      &= ~D_USB_PUE2;							/* �iD+�M����Pull-up ���Ȃ��j					*/
		ARMPF_USB_CONTROL =  state;								/* ��L����𓯎��ɔ��f							*/
		pUsbfFlag->UsbPlugStatus = U2F_PLUG_OUT;			/* �v���O��Ԃ��v���O�A�E�g�ɍX�V				*/
	}

#if 0	/* Interrupt.c�ł܂Ƃ߂Đݒ肷�邽�ߕs�v	*/
	/*=========================================*/
	/* ���荞�ݐݒ�							   */
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
  Description: USB�g�p�\���菈��
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	TRUE;			�g�p�\
	FALSE;			�g�p�֎~
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
	APIs	���荞�݊֘A
****************************************************************************/

/***************************************************************************
  Description: INTU2F ���荞�ݏ���
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
	UINT32			state;					/* ��ԑ���p				*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^		*/
	USB_FLAG		*pUsbfFlag;				/* USB�t���O�̈�|�C���^	*/

//<S138> >>>>>
	/* Rev.A��Ή� */
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}
//<<<<< <S138>

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB���̈�̃A�h���X�擾	*/
	pUsbfFlag = &(pUsbfIf->UsbfFlag);

	/*=========================================*/
	/* �v���O�C���^�A�E�g���� (VBUS)		   */
	/*=========================================*/
	if ((ARMPF_SYSBINT & D_SYS_VBUS_INT) == D_SYS_VBUS_INT)
	{
		ARMPF_SYSBINT |= D_SYS_VBUS_INT;								/* ���荞�ݗv���N���A	*/
		if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL)		/* VBUS���x���m�F		*/
		{
			/* ====== �v���O�C������ ====== */
			state       =  ARMPF_USB_CONTROL;							/* USB���䃌�W�X�^�l�擾						*/
			state      &= ~D_USB_CONNECTB;						/* �iFunction PHY �ւ�USB �M����L���ɂ���j	*/
			state      |=  D_USB_PUE2;							/* �iD+�M����Pull-up ����j						*/
			ARMPF_USB_CONTROL =  state;								/* ��L����𓯎��ɔ��f							*/
			pUsbfFlag->UsbPlugStatus = U2F_PLUG_IN;				/* �v���O��Ԃ��v���O�C���ɍX�V					*/
		}
		else
		{
			/* ====== �v���O�A�E�g���� ====== */
			state       =  ARMPF_USB_CONTROL;							/* USB���䃌�W�X�^�l�擾						*/
			state      |=  D_USB_CONNECTB;						/* �iFunction PHY �ւ�USB �M���𖳌��ɂ���j	*/
			state      &= ~D_USB_PUE2;							/* �iD+�M����Pull-up ���Ȃ��j					*/
			state      &= ~D_USB_DEFAULT;						/* �i�G���h�|�C���g0�𖳌��ɂ���j				*/
			state      &= ~D_USB_CONF;							/* �i�G���h�|�C���g0�ȊO�𖳌��ɂ���j			*/
			ARMPF_USB_CONTROL =  state;								/* ��L����𓯎��ɔ��f							*/
			pUsbfFlag->UsbPlugStatus = U2F_PLUG_OUT;			/* �v���O��Ԃ��v���O�A�E�g�ɍX�V				*/

			UsbFuncClearFlag( pUsbfFlag );						/* �t���O�E�N���A								*/
		}
	}

#if 0	/* XXX:�T���v���\�[�X�ŃR�����g�Ƃ��đ��� */
	intc_clear_eiint( IDX_INTU2F );								/* INTU2F �v���N���A	*/
#endif
}

/***************************************************************************
  Description: INTU2FEPC ���荞�ݏ���
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
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^		*/
	USB_FLAG		*pUsbfFlag;				/* USB�t���O�̈�|�C���^	*/

//<S138> >>>>>
	/* Rev.A��Ή� */
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}
//<<<<< <S138>

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB���̈�̃A�h���X�擾	*/
	pUsbfFlag = &(pUsbfIf->UsbfFlag);

	/*=========================================*/
	/* �o�X�E���Z�b�g ����					   */
	/*=========================================*/
	if ((ARMPF_USB_INT_STA & D_USB_USB_RST_INT) == D_USB_USB_RST_INT)
	{
		ARMPF_USB_INT_STA = ~D_USB_USB_RST_INT;						/* ���荞�ݗv���N���A		*/
		/* --- Max Packet�ݒ� --- */
		UsbFuncSetMaxpacket();
		/* --- EP �o�b�t�@�E�N���A --- */
		ARMPF_EP0_CONTROL   |= D_EP0_BCLR;							/* EP0 : �o�b�t�@�E�N���A	*/
		ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							/* EP1 : �o�b�t�@�E�N���A	*/
		ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							/* EP2 : �o�b�t�@�E�N���A	*/
		/* --- �t���O�E�N���A --- */
		UsbFuncClearFlag( pUsbfFlag );							/* �t���O�E�N���A			*/
	}

	/*=========================================*/
	/* USB�o�X�E�X�s�[�h�ύX ����			   */
	/*=========================================*/
	if ((ARMPF_USB_INT_STA & D_USB_SPEED_MODE_INT) == D_USB_SPEED_MODE_INT)
	{
		ARMPF_USB_INT_STA = ~D_USB_SPEED_MODE_INT;					/* ���荞�ݗv���N���A		*/
		/* --- Max Packet�ݒ� --- */
		UsbFuncSetMaxpacket();
	}

	/*=========================================*/
	/* EP0 ����								   */
	/*=========================================*/
	/* ---  SETUP�f�[�^��M --- */
	if ((ARMPF_EP0_STATUS & D_EP0_SETUP_INT) == D_EP0_SETUP_INT)
	{
		ARMPF_EP0_STATUS = ~D_EP0_SETUP_INT;							/* ���荞�ݗv���N���A		*/
		pUsbfFlag->UsbSetupReceive++;
	}
#if 0	/* Bulk�f�[�^�]���Ŋ��荞�݂͎g�p���Ă��Ȃ����� */
	/*=========================================*/
	/* EP1 ����								   */
	/*=========================================*/
	/* ---  Bulk-In �f�[�^���M --- */
	if ((ARMPF_EP1_STATUS & D_EPn_IN_INT) == D_EPn_IN_INT) {
		ARMPF_EP1_STATUS = ~D_EPn_IN_INT;								/* ���荞�ݗv���N���A		*/
		pUsbfFlag->UsbEp1InCount++;
	}
	/* ---  Bulk-In DMA�]���I�� --- */
	if ((ARMPF_EP1_STATUS & D_EPn_IN_END_INT) == D_EPn_IN_END_INT) {
		ARMPF_EP1_STATUS = ~D_EPn_IN_END_INT;							/* ���荞�ݗv���N���A		*/
	}
	/*=========================================*/
	/* EP2 ����								   */
	/*=========================================*/
	/* ---  Bulk-Out �f�[�^��M --- */
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_INT) == D_EPn_OUT_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_INT;							/* ���荞�ݗv���N���A		*/
		pUsbfFlag->UsbEp2OutCount++;
	}
	/* ---  Bulk-Out DMA�]���I�� --- */
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_END_INT) == D_EPn_OUT_END_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_END_INT;						/* ���荞�ݗv���N���A		*/
	}
#endif

#if 0	/* XXX:�T���v���\�[�X�ŃR�����g�Ƃ��đ��� */
	intc_clear_eiint( IDX_INTU2FEPC );							/* INTU2FEPC �v���N���A	*/
#endif
}


/***************************************************************************
	APIs	�R���g���[���]���֘A
****************************************************************************/

/***************************************************************************
  Description: �R���g���[���]���^�X�N����
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
	ER_RET			result;					/* ����							*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^			*/
	U2F_INFO		*pUsbfInfo;				/* USB�f�o�C�X���̈�|�C���^	*/
	DEVICE_REQUEST	*pDeviceRequest;		/* �f�o�C�X���N�G�X�g�|�C���^	*/

//<S138> >>>>>
	/* USB�g�p�s�̏ꍇ�͑��I��(Rev.A��Ή�) */
	if( UsbFuncChkAvailable() == FALSE )
	{
		return;
	}
//<<<<< <S138>

	/* USB���̈�A�h���X�擾	*/
	pUsbfIf = UsbFuncGetInfoAddress();
	/* USB�f�o�C�X���̈�A�h���X�擾	*/
	pUsbfInfo = &(pUsbfIf->UsbfInfo);
	/* �f�o�C�X���N�G�X�g�A�h���X�擾	*/
	pDeviceRequest = &(pUsbfIf->UsbfInfo.usb.request);

	/*=========================================*/
	/* EP0 ���� (SETUP�f�[�^��M)			   */
	/*=========================================*/
	if (pUsbfIf->UsbfFlag.UsbSetupReceive != pUsbfIf->UsbfFlag.UsbSetupSave)
	{
		/* �����r�p�ɕۑ����� */
		pUsbfIf->UsbfFlag.UsbSetupSave = pUsbfIf->UsbfFlag.UsbSetupReceive;
		/* ------ �f�o�C�X�E���N�G�X�g ���[�h ------ */
		pDeviceRequest->bmRequestType = (UINT8) ((ARMPF_SETUP_DATA0 >>  0) & 0xff  );
		pDeviceRequest->bRequest      = (UINT8) ((ARMPF_SETUP_DATA0 >>  8) & 0xff  );
		pDeviceRequest->wValue        = (UINT16)((ARMPF_SETUP_DATA0 >> 16) & 0xffff);
		pDeviceRequest->wIndex        = (UINT16)((ARMPF_SETUP_DATA1 >>  0) & 0xffff);
		pDeviceRequest->wLength       = (UINT16)((ARMPF_SETUP_DATA1 >> 16) & 0xffff);

		/* ------ ���N�G�X�g�E�^�C�v���� ------ */
		if( pDeviceRequest->wLength == 0x0000 )
		{	/* �f�[�^�E�X�e�[�W�̃o�C�g����0�̂Ƃ�		*/
			/*  -> STATUS������				*/
			result = control_nodata( pDeviceRequest, pUsbfInfo );
		}
		else
		{	/* �f�[�^�E�X�e�[�W�̃o�C�g����0�ł͂Ȃ��C	*/
			if( (pDeviceRequest->bmRequestType & USB_DIR_IN) == USB_DIR_IN )
			{	/* �f�[�^�]�������� Host=>Dev �̂Ƃ��C	*/
				/*  -> DATA IN & STATUS������	*/
				result = control_read( pDeviceRequest, pUsbfInfo );
			}
			else
			{	/* Dev=>Host �̂Ƃ��C					*/
				/*  -> DATA OUT & STATUS������	*/
				result = control_write( pDeviceRequest, pUsbfInfo );
			}
		}
		/* ------ �G���[���� --- */
		if (result != ER_OK)
		{
			/* ToDo:�R���g���[���]���ŃG���[���̋��ʏ���������΂����ɋL�q���� */
		}
	}
}

/***************************************************************************
	APIs	�o���N�]���֘A
****************************************************************************/

/***************************************************************************
  Description: USB �o���N�]�����䏈���iScan�����ɂ�銄���ݗv���̃|�[�����O�����j
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	�y�@�\�z
		�e���荞�ݏ�Ԃ������ɊĎ����Ċ����ݗv���������ɏ������s���B
			�Ď��Ώۊ��荞�݁F�L�ڏ��Ԃǂ���Ƀ`�F�b�N���Ă���
				(1) OUT�f�[�^��M����(ARMPF_EP2_STATUS & D_EPn_OUT_INT)
				(2) OUT�f�[�^NULL�p�P�b�g��M����(ARMPF_EP2_STATUS & D_EPn_OUT_NULL_INT)
				(3) IN�g�[�N����NAK����(ARMPF_EP1_STATUS & D_EPn_IN_NAK_ERR_INT)
	�y���Ӂz(SGDV�p�̃R�����g�@�Ƃ肠�����ύX�Ȃ��Ŏc���Ă���)
		���̊֐��́A�ʐM�}�l�[�W���Ɠ������͏�ʃ^�X�N�ɂĎ��s�����邱�ƁB
		�ʐM�}�l�[�W���Ɠ����^�X�N�Ŏ��s������ꍇ�ʐM�}�l�[�W���̑O�Ŏ��s������ƌ������ǂ��B
----------------------------------------------------------------------------
	�y����Ώہz
		���̃��W���[���Ő��䂷��G���h�|�C���g�͈ȉ��̒ʂ�B
			Bulk out: Endpoint2
			Bulk in : Endpoint1
	�y����M�J�n����(�Q�l)�z
		�ʐM�}�l�[�W������]���J�n�w�߂��ȉ��̊֐��Ŏ󂯂�B
			Bulk out: UsbFuncStartBkout()
			Bulk in : UsbFuncStartBkin()
		�J�n�w�ߎ���CPU��USBF���`�F�b�N���ăh���C�o�X�e�[�^�X���ȉ��̂悤�ɐݒ肷��B
			Bulk out: ��M�\ -> UsbfIf.Ep1Status = BLKTRNS_ENABLE
					  ��M�s�� -> UsbfIf.Ep1Status = BLKTRNS_DISABLE
			Bulk in : ���M�\ -> UsbfIf.Ep2Status = BLKTRNS_ENABLE
					  ���M�s�� -> UsbfIf.Ep2Status = BLKTRNS_DISABLE

****************************************************************************/
/*<S182> �f�[�^�R�s�[�𕡐��X�L�����ɕ�������悤�ɕύX�i�ύX���������ߊ֐����ɗ����Ȃ��j  */
void	UsbFuncPollBulkOutIn( void )
{
	UINT32			MaxPacketSize;				/* �ő�p�P�b�g�T�C�Y			*/
	USB_FUNCTION_IF	*pUsbfIf;					/* USB���̈�|�C���^			*/

	pUsbfIf = UsbFuncGetInfoAddress();			/* USB���̈�̃A�h���X�擾	*/

	/*----------------------------------------------------------------------------------------------*/
	/*	�o���N�]�������ݗv���t���O�̃`�F�b�N														*/
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*	���ӁI�I																					*/
	/*																								*/
	/*	�F�{������ScanC�Ȃǂ̒ᑬ������^�X�N�Ń|�[�����O����ꍇ��OUT�f�[�^���_�u���o�b�t�@����	*/
	/*	  �ɗ��܂�P�[�X����������悤�ɂȂ�A�ȉ��̕ύX���K�v�ƂȂ�܂��I�I						*/
	/*																								*/
	/*	�P�DEP2��M����IN�������ꍇ��OUT�f�[�^�����ׂēǂ݂��������Ƃ��m�F���Ă����M�I���Ƃ���	*/
	/*----------------------------------------------------------------------------------------------*/

	/*------------------------------------------------------------------------------------------*/
	/*	�ڍחv���`�F�b�N(1-1)	: OUT�f�[�^��M����												*/
	/*------------------------------------------------------------------------------------------*/
	if( (pUsbfIf->OutSplitCopy != TRUE) && (pUsbfIf->InSplitCopy != TRUE) )
	if( (ARMPF_EP2_STATUS & D_EPn_OUT_INT) == D_EPn_OUT_INT )	/* OUT�f�[�^��M����			*/
	{
		ARMPF_EP2_STATUS &= ~D_EPn_OUT_INT;						/* �����ݗv���N���A				*/
		if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )				/* ��M���`�F�b�N				*/
		{
																/* ��M���ۑ�					*/
			pUsbfIf->OutPacketSize = ARMPF_EP2_LEN_DCNT & D_EPn_LDATA;
			pUsbfIf->OutCopyRem = pUsbfIf->OutPacketSize;		/* �R�s�[�c�T�C�Y�ۑ�			*/
			/*------------------------------------------------------------------------------*/
			/*	��M�T�C�Y�I�[�o�[�`�F�b�N													*/
			/*------------------------------------------------------------------------------*/
			if( (pUsbfIf->OutLength + pUsbfIf->OutPacketSize) > pUsbfIf->OutLengthReq )
			{
				/*--------------------------------------------------------------------------*/
				/*	������ް�����M�������M�ޯ̧�̻��ނ��ނ��邽�߃f�[�^�j�����Ċ���	*/
				/*	�A�v���P�[�V�������x���ŃG���[���������s���Ă��炤�B					*/
				/*--------------------------------------------------------------------------*/
				ARMPF_EP2_CONTROL |= D_EPn_ONAK;				/* EP2��NAK�����݂̂ɐݒ�		*/
				pUsbfIf->Ep2Status = BLKTRNS_STALL;				/* STALL�����������Ƃ��Z�b�g	*/
			}
			else
			{
				/*--------------------------------------------------------------------------*/
				/*	�I�[�o���Ȃ��ꍇ�A������M����											*/
				/*--------------------------------------------------------------------------*/
				UsbFuncOutDataSplitCopy( pUsbfIf );
			}
		}
	}
	/*------------------------------------------------------------------------------------------*/
	/*	�ڍחv���`�F�b�N(1-2)	: OUT�f�[�^�����R�s�[��											*/
	/*------------------------------------------------------------------------------------------*/
	if( pUsbfIf->OutSplitCopy == TRUE )
	{
		if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )
		{
			/*--------------------------------------------------------------------------*/
			/*	������M�̑��������{����												*/
			/*--------------------------------------------------------------------------*/
			UsbFuncOutDataSplitCopy( pUsbfIf );
		}
	}

	/*--------------------------------------------------------------------------------------*/
	/*	�ڍחv���`�F�b�N(2)	: OUT�f�[�^NULL�p�P�b�g��M����									*/
	/*		OUT�f�[�^���}�b�N�X�E�p�P�b�g�T�C�Y�̐����{�̎��A								*/
	/*		�f�[�^�I���F���̂��ߍŌ��NULL���o���z�X�g�̂���								*/
	/*		NULL���Ď����f�[�^��M����NULL���������M�����Ƃ���							*/
	/*--------------------------------------------------------------------------------------*/
	if( pUsbfIf->OutSplitCopy != TRUE )
	if( (ARMPF_EP2_STATUS & D_EPn_OUT_NULL_INT) == D_EPn_OUT_NULL_INT )
	{
		ARMPF_EP2_STATUS &= ~D_EPn_OUT_NULL_INT;		/* BKO1NL�����ݗv���N���A			*/
		if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )		/* ��M�����H						*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;			/* EP2��NAK�����݂̂ɐݒ�			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;			/* ��M�������Z�b�g					*/
		}
	}

	/*--------------------------------------------------------------------------------------*/
	/*	�ڍחv���`�F�b�N(3-1)	: IN�g�[�N����NAK����										*/
	/*		�ʏ�OUT�f�[�^��AIN�g�[�N��������B												*/
	/*		�A��OUT�f�[�^���}�b�N�X�E�p�P�b�g�T�C�Y�̐����{�̏ꍇ�A�z�X�g��					*/
	/*		����Ă�OUT�f�[�^�̍Ō��NULL���o�����A�����Ȃ�IN�g�[�N�����o���B				*/
	/*		�]���āAEndpoint1�����ł͂Ȃ�Endpoint2�̏�Ԃ��`�F�b�N��OUT�f�[�^��M�ҋ@����	*/
	/*		IN��������OUT�f�[�^��M������IN�g�[�N����M��������s���čs���B					*/
	/*--------------------------------------------------------------------------------------*/
	if( (pUsbfIf->OutSplitCopy != TRUE) && (pUsbfIf->InSplitCopy != TRUE) )
	if( (ARMPF_EP1_STATUS & D_EPn_IN_NAK_ERR_INT) == D_EPn_IN_NAK_ERR_INT )
	{
		ARMPF_EP1_STATUS &= ~D_EPn_IN_NAK_ERR_INT;		/* �t���O�N���A						*/
		if( pUsbfIf->Ep1Status == BLKTRNS_ENABLE )		/* ���M���`�F�b�N					*/
		{
			/*------------------------------------------------------------------------------*/
			/*	���M�f�[�^�����݋��`�F�b�N												*/
			/*------------------------------------------------------------------------------*/
			if( (ARMPF_EP1_STATUS & D_EPn_IN_EMPTY) == D_EPn_IN_EMPTY )	/* EP1��IN EMPTY�̏ꍇ������OK		*/
			{
				/* �����f�[�^���M���� */
				ARMPF_EP1_CONTROL &= ~D_EPn_AUTO;

				/* EP1�̍ő�p�P�b�g�T�C�Y�擾 */
				MaxPacketSize = ARMPF_EP1_PCKT_ADRS & D_EPn_MPKT;

				/* ����p�P�b�g�T�C�Y��ݒ� */
				if( pUsbfIf->InLengthRem > MaxPacketSize )
				{
					pUsbfIf->InCopyRem = MaxPacketSize;
				}
				else
				{
					pUsbfIf->InCopyRem = pUsbfIf->InLengthRem;
				}

				/*--------------------------------------------------------------------------*/
				/*	�������M�J�n															*/
				/*--------------------------------------------------------------------------*/
				UsbFuncInDataSplitCopy( pUsbfIf );
			}
		}
		else if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )	/* EP2��M����IN���������M����	*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;			/* EP2��NAK�����݂̂ɐݒ�			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;			/* ��M�������Z�b�g					*/
		}
	}

	/*------------------------------------------------------------------------------------------*/
	/*	�ڍחv���`�F�b�N(3-2)	: IN�f�[�^�����R�s�[��											*/
	/*------------------------------------------------------------------------------------------*/
	if( pUsbfIf->InSplitCopy == TRUE )
	{
		if( pUsbfIf->Ep1Status == BLKTRNS_ENABLE )		/* ���M���`�F�b�N					*/
		{
			/*--------------------------------------------------------------------------*/
			/*	������M�̑��������{����												*/
			/*--------------------------------------------------------------------------*/
			UsbFuncInDataSplitCopy( pUsbfIf );
		}
		else if( pUsbfIf->Ep2Status == BLKTRNS_ENABLE )	/* EP2��M����IN���������M����	*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;			/* EP2��NAK�����݂̂ɐݒ�			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;			/* ��M�������Z�b�g					*/
		}
	}
	return;
}


/***************************************************************************
  Description: USB Endpoint1/2 ����M�o�b�t�@�N���A����
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
	USB_FUNCTION_IF	*pUsbfIf;						/* USB���̈�|�C���^			*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB���̈�̃A�h���X�擾	*/

	ARMPF_EP2_CONTROL |= D_EPn_ONAK;						/* Endpoint2��NAK�݂̂̉����ɂ���			*/
	pUsbfIf->Ep1Status = BLKTRNS_DISABLE;			/* EP1�̏�Ԃ𑗐M�֎~�ɐݒ�				*/
	pUsbfIf->Ep2Status = BLKTRNS_DISABLE;			/* EP2�̏�Ԃ𑗐M�֎~�ɐݒ�				*/
	/*------------------------------------------------------------------------------------------*/
	/*	��M(OUT) �o�b�t�@�̃N���A																*/
	/*------------------------------------------------------------------------------------------*/
	ARMPF_EP2_CONTROL |= D_EPn_BCLR;
	/*------------------------------------------------------------------------------------------*/
	/*	���M(IN) �o�b�t�@�̃N���A																*/
	/*------------------------------------------------------------------------------------------*/
	ARMPF_EP1_CONTROL |= D_EPn_BCLR;

	return;
}

/***************************************************************************
	APIs	USB MEMOBUS�T�[�o�[�p�����֘A
****************************************************************************/

/***************************************************************************
  Description: USB Endpoint2 ��M�J�n(BLUK OUT)����
----------------------------------------------------------------------------
  Parameters:
	Buffer;			��M�f�[�^�i�[��擪�A�h���X
	OutLength;		��M�f�[�^�i�[��o�b�t�@�T�C�Y
  Return:
	TRUE;			��M����
	FALSE;			��M�֎~
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncStartBkout( UCHAR *Buffer ,USHORT OutLength )
{
	ULONG			rc;								/* �߂�l						*/
	USB_FUNCTION_IF	*pUsbfIf;						/* USB���̈�|�C���^			*/

	rc = FALSE;										/* �����l : FALSE				*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB���̈�̃A�h���X�擾	*/

	/*----------------------------------------------------------------------------------------------*/
	/*	EP2�g�p�\���`�F�b�N���� :: �C���^�[�t�F�C�X��Configutation�ς݂̏ꍇ�Ɏg�p�\			*/
	/*----------------------------------------------------------------------------------------------*/
	if( (ARMPF_USB_STATUS & D_USB_CONF) == D_USB_CONF )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP2�g�p�\������																		*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->OutLengthReq = OutLength;			/* ��M�����T�C�Y���L��					*/
		pUsbfIf->OutLength = 0;						/* ��M�ς݃o�C�g����������					*/
		pUsbfIf->OutBufPtr = Buffer;				/* ��M�f�[�^�i�[��|�C���^���L��			*/
		/*------------------------------------------------------------------------------------------*/
		/*	EP2�o�b�t�@�̃N���A																		*/
		/*------------------------------------------------------------------------------------------*/
		ARMPF_EP2_CONTROL |= D_EPn_BCLR;
		/*------------------------------------------------------------------------------------------*/
		/*	�����R�s�[�t���O������	<S182>														*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->OutSplitCopy = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	�X�e�[�^�X����M�\��ԂɃZ�b�g														*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->Ep2Status = BLKTRNS_ENABLE;		/* EP2�̏�Ԃ���M���ɐݒ�				*/
		ARMPF_EP2_CONTROL &= ~D_EPn_ONAK;			/* EP2��NAK����������						*/

		rc = TRUE;
	}
	else
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP2�g�p�s�̏ꍇ�A�g�p�֎~�ɂ��ă��^�[������											*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->Ep2Status = BLKTRNS_DISABLE;		/* EP2�̏�Ԃ���M�֎~�ɐݒ�				*/
		rc = FALSE;
	}
	return( rc );
}

/***************************************************************************
  Description: USB Endpoint1 ���M�J�n(BLUK IN)����
----------------------------------------------------------------------------
  Parameters:
	Buffer;			���M�f�[�^�i�[��擪�A�h���X
	InLength;		���M�f�[�^�T�C�Y
  Return:
	TRUE;			���M����
	FALSE;			���M�֎~
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncStartBkin( UCHAR *Buffer ,USHORT InLength )
{
	ULONG			rc;								/* �߂�l						*/
	USB_FUNCTION_IF	*pUsbfIf;						/* USB���̈�|�C���^			*/

	rc = FALSE;										/* �����l : FALSE				*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB���̈�̃A�h���X�擾	*/

	/*----------------------------------------------------------------------------------------------*/
	/*	EP1�g�p�\���`�F�b�N���� :: �C���^�[�t�F�C�X��Configutation�ς݂̏ꍇ�Ɏg�p�\			*/
	/*----------------------------------------------------------------------------------------------*/
	if( (ARMPF_USB_STATUS & D_USB_CONF) == D_USB_CONF )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP1�g�p�\������																		*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->InLengthRem = InLength;			/* ���M�f�[�^�c��T�C�Y��������				*/
		pUsbfIf->InLength = 0;						/* ���M�ς݃o�C�g����������					*/
		pUsbfIf->InBufPtr = Buffer;					/* ��M�f�[�^�i�[����L��					*/
		/*------------------------------------------------------------------------------------------*/
		/*	EP1�o�b�t�@�̃N���A																		*/
		/*------------------------------------------------------------------------------------------*/
		ARMPF_EP1_CONTROL |= D_EPn_BCLR;
		/*------------------------------------------------------------------------------------------*/
		/*	�����R�s�[�t���O������	<S182>														*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->InSplitCopy = FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	�X�e�[�^�X�𑗐M�\��ԂɃZ�b�g														*/
		/*------------------------------------------------------------------------------------------*/
		ARMPF_EP2_STATUS &= ~D_EPn_OUT_NAK_ERR_INT;		/* EP2 OUT�g�[�N����M��ԃN���A			*/
		pUsbfIf->Ep1Status = BLKTRNS_ENABLE;		/* EP1�̏�Ԃ𑗐M���ɐݒ�				*/

		rc = TRUE;
	}
	else
	{
		/*------------------------------------------------------------------------------------------*/
		/*	EP1�g�p�s��DMA�ݒ肹���Ƀ^�[������													*/
		/*------------------------------------------------------------------------------------------*/
		pUsbfIf->Ep1Status = BLKTRNS_DISABLE;		/* EP1�̏�Ԃ𑗐M�֎~�ɐݒ�				*/
		rc = FALSE;
	}
	return( rc );
}

/***************************************************************************
  Description: USB�|�[�g��Ԓʒm�T�[�r�X����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	TRUE;			�A�v���P�[�V�����w�ʐM�\
	FALSE;			�A�v���P�[�V�����w�ʐM�s��
----------------------------------------------------------------------------
  Note:
	�A�v���P�[�V�����w�ʐM���\�ł��邩��ԐM���܂��B
	�EUSB�ł́ASET_CONFIGURATION Val = 1 ��Host��Device�Ɏw�߂����Endpoint0�ȊO��(�{�h��
	  �C�o�ł�1��2)���g�p�\�ɂȂ�܂��B�A�v���P�[�V�����ʐM��EP1��EP2���g�p���܂��B
****************************************************************************/
ULONG	UsbFuncPortConfiguration( void )
{
	ULONG	rc;								/* ���֐��߂�l				*/
	UINT32	ret;							/* ���֐��߂�l				*/

	rc = FALSE;								/* �����l : FALSE			*/

	/* �f�o�C�X�X�e�[�g�擾	*/
	ret = udev_get_DeviceState();
	if( ret == UDEV_ST_CONFIGURED)
	{	/* Configured�X�e�[�g���I�����Ă���	*/
		rc = TRUE;
	}

	return( rc );
}

/***************************************************************************
  Description: USB Endpoint2 ��M�o�C�g���擾����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	��M�f�[�^�o�C�g�T�C�Y
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
USHORT	UsbFuncGetRcvSize( void )
{
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^		*/

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB���̈�A�h���X�擾	*/
	
	return( pUsbfIf->OutLength );			/* ��M�f�[�^�o�C�g�T�C�Y	*/
}

/***************************************************************************
  Description: �o���N�]��(OUT)��Ԓʒm�T�[�r�X����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	�o���N�]��(OUT)���
	BLKTRNS_DISABLE;		�o���N�]���֎~
	BLKTRNS_ENABLE;			�o���N�]������
	BLKTRNS_END;			�o���N�]������
	BLKTRNS_STALL;			�o���N�]���ɂĈُ픭����STALL����
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncChkRcvComp( void )
{
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^		*/

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB���̈�A�h���X�擾	*/

	return( (ULONG)(pUsbfIf->Ep2Status) );	/* �o���N�]��(OUT)���		*/
}

/***************************************************************************
  Description: �o���N�]��(IN)��Ԓʒm�T�[�r�X����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	�o���N�]��(IN)���
	BLKTRNS_ENABLE;			�o���N�]������
	BLKTRNS_END;			�o���N�]���I��
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
ULONG	UsbFuncChkTrnsComp( void )
{
	ULONG			rc;						/* �߂�l					*/
	USB_FUNCTION_IF	*pUsbfIf;				/* USB���̈�|�C���^		*/

	rc = BLKTRNS_ENABLE;					/* �����l : BLKTRNS_ENABLE	*/

	pUsbfIf = UsbFuncGetInfoAddress();		/* USB���̈�A�h���X�擾	*/

	/* ���M�o�b�t�@����ɂȂ����瑗�M����*/
	if( (pUsbfIf->Ep1Status == BLKTRNS_END) 
		&& ((ARMPF_EP1_STATUS & D_EPn_IN_EMPTY) == D_EPn_IN_EMPTY) )
	{
		rc = BLKTRNS_END;
	}
	/* ���M�����҂���ԂŁAOUT�g�[�N������M�����ꍇ�͋����I�� */
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
  Description:�x���_�[���N�G�X�g�R�[���o�b�N����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;			�f�o�C�X���N�G�X�g
  Return:
	ER_OK;					����
	���̑�;					���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET VendorRequest_Callback( DEVICE_REQUEST* pDeviceRequest )
{
	UINT32			data;						/* Max Logical Unit Number	*/
	ER_RET			result;						/* ����						*/
	USB_FUNCTION_IF	*pUsbfIf;					/* USB���̈�|�C���^		*/

	/* �߂�l������			*/
	result = ER_SYS;

	/* USB���̈�A�h���X�擾	*/
	pUsbfIf = UsbFuncGetInfoAddress();

	/* Configured�X�e�[�g�I���m�F	*/
	if (pUsbfIf->UsbfInfo.usb.curr_config == 0) {
		result = ER_SYS;
		return result;
	}

	/* ���N�G�X�g����	*/
	switch( pDeviceRequest->bRequest )
	{
		/* Get own address	*/
		case USB_VENDER_GET_OWN_ADDRESS:
			/* �p�����[�^�E�`�F�b�N		*/
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

			/* ���ǃA�h���X���M	*/
			data = 0;
			data |= pUsbfIf->UsbfInfo.MyAddress;
			UsbFuncWriteEP0( (UCHAR*)&data, pDeviceRequest->wLength );

			result = ER_OK;
			break;

		/* ���`���N�G�X�g����	*/
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: USB�t�@���N�V������񏉊�������
----------------------------------------------------------------------------
  Parameters:
	pUsbfInfo;				USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void	UsbFuncInitializeInfo( U2F_INFO *pUsbfInfo )
{
	/* Descriptor�ݒ�	*/
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
	pUsbfInfo->dev.ep_num							= U2F_EPn_MAX;				/* ���p����EP��									*/
#if 0	/* XXX:�T���v���\�[�X�ŃR�����g�A�E�g����Ă��� */
	pUsbfInfo->dev.ep_dir							= DIR_OUT_EP2;				/* EPn����(OUT:1,IN:0) Bit0=EP1 - Bit14=EP15	*/
#endif
	pUsbfInfo->cb_class_req							= 0;						/* Class Request�R�[���o�b�N�֐�(�Ȃ�)			*/
	pUsbfInfo->cb_vendor_req						= VendorRequest_Callback;	/* Vendor Request�R�[���o�b�N�֐�				*/

	return;
}

/***************************************************************************
  Description: �t���O�E�N���A����
----------------------------------------------------------------------------
  Parameters:
	pUsbfFlag;				USB�t���O�ւ̃|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncClearFlag( USB_FLAG *pUsbfFlag )
{
	pUsbfFlag->UsbSetupReceive = 0;						/* SETUP�g�[�N����M��	*/
	pUsbfFlag->UsbSetupSave = 0;						/* ��L�ۑ��l				*/
}

/***************************************************************************
  Description: Max Packet Size �ݒ菈��
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
	UINT32	bulk_size;								/* �o���N�]���T�C�Y�p	*/
	UINT32	interrupt_size;							/* ���荞�ݓ]���T�C�Y�p	*/
 
	/*---------------------------------*/
	/* USB�o�X�E�X�s�[�h�m�F		   */
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
	/* Max Packet Size �ݒ�			   */
	/*---------------------------------*/
														/* EPn : BaseAdr  MaxPkt(HS/FS �{�h���C�o�ł͋K�i��̍ő�l��ݒ�)	*/
														/* EP0 : 0x0000        64 byte x 2��								*/
	ARMPF_EP1_PCKT_ADRS = 0x00200000 | bulk_size;				/* EP1 : 0x0020    512/64 byte x 2��	--   Bulk    �]�� --		*/
	ARMPF_EP2_PCKT_ADRS = 0x01200000 | bulk_size;				/* EP2 : 0x0120    512/64 byte x 2��	--   Bulk    �]�� --		*/
	ARMPF_EP3_PCKT_ADRS = 0x02200000 | interrupt_size;		/* EP3 : 0x0220   1024/64 byte x 1��	-- Interrupt �]�� --		*/
														/*  BaseAdr��1���[�h(4�o�C�g)�P��									*/
														/*		EP0�� 64(byte) x 2 = 128(byte) = 64(word) = 0x0020 ���		*/
														/*		EP1��BaseAdr�� 0x0000+0x0020 = 0x0020 �ƂȂ�				*/
}

/***************************************************************************
  Description: USB���̈�̃A�h���X�擾����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	USB���̈�̃A�h���X
----------------------------------------------------------------------------
  Note:
	USB���̈�̃A�h���X�͖{�֐��𗘗p���Ď擾���邱��
****************************************************************************/
static USB_FUNCTION_IF*	UsbFuncGetInfoAddress( void )
{
	return &UsbfIf;
}

/***************************************************************************
  Description: �ڑ���Ԋm�F����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	U2F_PLUG_OUT;		USB Host�Ɛڑ�����Ă��Ȃ��Ƃ�
	U2F_PLUG_IN;		USB Host�Ɛڑ�����Ă���Ƃ�
****************************************************************************/
static UINT UsbFuncCheckPlugInout( void )
{
	USB_FUNCTION_IF	*pUsbfIf;						/* USB���̈�|�C���^			*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB���̈�̃A�h���X�擾	*/

	return pUsbfIf->UsbfFlag.UsbPlugStatus;			/* �ڑ���Ԃ�Ԃ�				*/
}

/***************************************************************************
  Description: SETUP�g�[�N����M�m�F����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	none
----------------------------------------------------------------------------
  Note:
	0;		SETUP�g�[�N������M
	0�ȊO;	SETUP�g�[�N����M
****************************************************************************/
static UINT UsbFuncCheckSetupReceive( void )
{
	USB_FUNCTION_IF	*pUsbfIf;						/* USB���̈�|�C���^			*/

	pUsbfIf = UsbFuncGetInfoAddress();				/* USB���̈�̃A�h���X�擾	*/

	/* SETUP�g�[�N����M��Ԃ�Ԃ�	*/
	return (pUsbfIf->UsbfFlag.UsbSetupReceive - pUsbfIf->UsbfFlag.UsbSetupSave);
}

/***************************************************************************
	Locals	EPn�]���֘A
****************************************************************************/

/***************************************************************************
  Description: EPn �X�g�[����������
----------------------------------------------------------------------------
  Parameters:
	num;			EP�ԍ�
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncSetStallEPn( UINT num )
{
	/* --- IN/OUT ���f (���䃌�W�X�^�̓]�������Ŋm�F 0:IN, 1:OUT ) --- */
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		/* --- BULK IN --- */
		/* IN�g�[�N����STALL���� */
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_ISTL;
	} else {
		/* --- BULK OUT --- */
		/* OUT/PING�g�[�N����STALL�����A����і{�ݒ�̗L���� */
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= (D_EPn_OSTL_EN | D_EPn_OSTL);
	}
}


/* <S182> >>>>> */
/***************************************************************************
  Description: OUT�f�[�^�����R�s�[����
----------------------------------------------------------------------------
  Parameters:
	pUsbfIf;			USB���̈�|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncOutDataSplitCopy( USB_FUNCTION_IF *pUsbfIf )
{
	INT				i;							/* ���[�v�J�E���^				*/
	USHORT			strind;						/* ����M�o�b�t�@�������ʒu		*/
	INT				pktlen;						/* ��M�p�P�b�g�o�C�g�T�C�Y		*/
	UINT32			TmpWordData;				/* ���[�h�f�[�^�e���|����		*/
	UINT32			MaxPacketSize;				/* �ő�p�P�b�g�T�C�Y			*/
	UINT32			CopySize;					/* �R�s�[�T�C�Y					*/

	CopySize = SCAN_COPY_SIZE;

	strind = pUsbfIf->OutLength;				/* �����M�f�[�^�擪index		*/
												/* ���񏈗������M���ݒ�		*/
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
	pUsbfIf->OutLength += (USHORT)pktlen;		/* �����M�f�[�^�Ō��index	*/

	/*--------------------------------------------------------------------------*/
	/*	��M����																*/
	/*--------------------------------------------------------------------------*/
	while( strind < pUsbfIf->OutLength )
	{
		TmpWordData = ARMPF_EP2_READ;			/* 4�o�C�g�f�[�^�擾			*/
		for( i = 0; i < 4; i++ )
		{
			/* 1�o�C�g����4�o�C�g�����o�b�t�@�Ɋi�[���� */
			pUsbfIf->OutBufPtr[strind] = (UINT8)(TmpWordData >> (8*i));
			strind++;
			/* �[��(1�`3)�o�C�g�̃`�F�b�N	*/
			if( strind >= pUsbfIf->OutLength )
			{
				break;
			}
		}
	}
	/*--------------------------------------------------------------------------*/
	/*	��M��������															*/
	/*--------------------------------------------------------------------------*/
	/* �����M��(�}�b�N�X�E�p�P�b�g�T�C�Y�܂�)��S�ăR�s�[���Ă���Ί��� */
	if( pUsbfIf->OutCopyRem == 0 )
	{
		/* EP2�̃}�b�N�X�E�p�P�b�g�T�C�Y�擾 */
		MaxPacketSize = ARMPF_EP2_PCKT_ADRS & D_EPn_MPKT;

		if( pUsbfIf->OutPacketSize < MaxPacketSize )	/* ������߹�Ă��ő�T�C�Y�����Ȃ犮��	*/
		{
			ARMPF_EP2_CONTROL |= D_EPn_ONAK;		/* EP2��NAK�����݂̂ɐݒ�			*/
			pUsbfIf->Ep2Status = BLKTRNS_END;		/* ��M�������Z�b�g					*/
		}
		pUsbfIf->OutSplitCopy = FALSE;				/* �����R�s�[�t���O������			*/
	}
	else
	{
		pUsbfIf->OutSplitCopy = TRUE;				/* �����R�s�[�t���O��ݒ�			*/
	}
}


/***************************************************************************
  Description: IN�f�[�^�����R�s�[����
----------------------------------------------------------------------------
  Parameters:
	pUsbfIf;			USB���̈�|�C���^
  Return:
	none
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static void UsbFuncInDataSplitCopy( USB_FUNCTION_IF *pUsbfIf )
{
	INT				i;							/* ���[�v�J�E���^				*/
	USHORT			strind;						/* ����M�o�b�t�@�������ʒu		*/
	UINT32			TmpWordData;				/* ���[�h�f�[�^�e���|����		*/
	UINT32			TmpFlag;					/* ���䃌�W�X�^�l�ҏW�p			*/
	UINT32			CopySize;					/* �R�s�[�T�C�Y					*/
	UCHAR			endflag;					/* �I���t���O					*/

	endflag = FALSE;							/* �����l : FALSE				*/

	/* EP1�̃R�s�[�T�C�Y�Đݒ� */
	CopySize = SCAN_COPY_SIZE;

	if( pUsbfIf->InLengthRem >= CopySize )
	{
		/*----------------------------------------------------------------------*/
		/*	�c�T�C�Y��EP1�̍ő�p�P�b�g�T�C�Y�ȏ�̏ꍇ							*/
		/*----------------------------------------------------------------------*/
		strind = pUsbfIf->InLength;				/* ���񑗐M�f�[�^�擪index		*/
		pUsbfIf->InLength += CopySize;			/* ���񑗐M�f�[�^�擪index		*/
		pUsbfIf->InLengthRem -= CopySize;		/* �c�T�C�Y�v�Z					*/
		pUsbfIf->InCopyRem -= CopySize;			/* ����p�P�b�g�̎c�T�C�Y�v�Z	*/
	}
	else
	{
		/*----------------------------------------------------------------------*/
		/*	�c�T�C�Y��EP1�̍ő�p�P�b�g�T�C�Y�����̏ꍇ�iNULL���܂�)			*/
		/*----------------------------------------------------------------------*/
		strind = pUsbfIf->InLength;				/* ���񑗐M�f�[�^�擪index		*/
		pUsbfIf->InLength += pUsbfIf->InLengthRem;	/* ���񑗐M�f�[�^�擪index	*/
		pUsbfIf->InCopyRem = 0;					/* ����p�P�b�g�̎c�T�C�Y		*/
		endflag = TRUE;							/* ����ő��M�������邱�Ƃ��L��	*/
	}
	/*--------------------------------------------------------------------------*/
	/*	�f�[�^���M����															*/
	/*		��NULL�̏ꍇ�͂P���for���[�v���Ȃ�(�������܂Ȃ�)					*/
	/*--------------------------------------------------------------------------*/
	/* ���M�f�[�^�̐ݒ� */
	for( ; (strind + sizeof(UINT32) - 1) < pUsbfIf->InLength ; strind += 4 )
	{
		/* ���[�h(4�o�C�g)�f�[�^���쐬���ď������� */
		TmpWordData   = (UINT32)pUsbfIf->InBufPtr[strind+0];
		TmpWordData  |= (UINT32)pUsbfIf->InBufPtr[strind+1] << 8;
		TmpWordData  |= (UINT32)pUsbfIf->InBufPtr[strind+2] << 16;
		TmpWordData  |= (UINT32)pUsbfIf->InBufPtr[strind+3] << 24;
		/* �o�b�t�@�������� */
		ARMPF_EP1_WRITE = TmpWordData;
	}

	/* �f�[�^���M�^�C�~���O */
	if( pUsbfIf->InCopyRem == 0 )
	{
		/* --- �f�[�^���M������ --- */
		TmpFlag  = ARMPF_EP1_CONTROL;						/* EP1���䃌�W�X�^�l�擾		*/
		TmpFlag |= D_EPn_DEND;						/* ���M���ݒ�ǉ�				*/

		/* �[���o�C�g����(endflag=TRUE���A���M�p�C���f�b�N�X�����M�T�C�Y������) */
		if( (endflag == TRUE) && (strind < pUsbfIf->InLength) )
		{
			/* �[���o�C�g�Z�o(0�`3) : 1�`3�̎����������	*/
			pUsbfIf->InLengthRem &= (BIT1 | BIT0);

			TmpWordData = 0;						/* �f�[�^0�N���A				*/

			/* �[���f�[�^�̐ݒ�	*/
			for (i = 0; i < pUsbfIf->InLengthRem; i++) {
				TmpWordData |= (UINT32)pUsbfIf->InBufPtr[strind+i] << (8*i);
			}

			/* --- �o�b�t�@�������� ---	*/
			ARMPF_EP1_WRITE = TmpWordData;

			/* --- �[���o�C�g���̐ݒ� --- */
			TmpFlag |= ( ((UINT32)pUsbfIf->InLengthRem) << 5 ) & ( D_EP0_DW );
		}

		/* �f�[�^���M */
		ARMPF_EP1_CONTROL |= TmpFlag;

		/*--------------------------------------------------------------------------*/
		/*	���M�����Z�b�g															*/
		/*--------------------------------------------------------------------------*/
		if( endflag == TRUE )
		{
			pUsbfIf->Ep1Status = BLKTRNS_END;	/* ���M�������Z�b�g					*/
		}
		pUsbfIf->InSplitCopy = FALSE;			/* �����R�s�[�t���O����				*/
	}
	else
	{
		pUsbfIf->InSplitCopy = TRUE;			/* �����R�s�[�t���O�ݒ�				*/
	}
}
/* <<<<< <S182> */

/***************************************************************************
	Locals	�R���g���[���]���֘A
****************************************************************************/

/***************************************************************************
  Description: EP0 ���M����
----------------------------------------------------------------------------
  Parameters:
	pBuf;				���M�f�[�^�i�[�̈�
	size;				���M�T�C�Y
  Return:
	0�ȏ�̒l�̂Ƃ�;	���M�����o�C�g��
	���̒l�̂Ƃ�;		�G���[�����ɂ�鋭���I��
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static INT UsbFuncWriteEP0( VUCHAR* pBuf, INT size )
{
	INT	iRemainSize;						/* �c��f�[�^��					*/

	UINT32	TmpData;						/* �������݃f�[�^�쐬�p���[�N	*/
	UINT32	temp;							/* ���䃌�W�X�^�l�ҏW�p���[�N	*/
	INT		i;								/* ���[�v�J�E���^				*/

	/* �c��f�[�^��	*/
	iRemainSize = size;

	/* �c��f�[�^������΃��[�v	*/
	while (iRemainSize > 0)
	{
		/*-----------------------------*/
		/* �o�b�t�@�E�G���v�e�B�m�F	   */
		/*-----------------------------*/
		/* �o�b�t�@����ɂȂ�܂ŃE�G�C�g	*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
		{
			/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return ER_SYS;
			}
		}

		/*-----------------------------*/
		/* �o�b�t�@��������			   */
		/*  (4byte�P��, Max:64byte)	   */
		/*-----------------------------*/
		for (i = 0; ((i < 64) && (iRemainSize >= 4)); i += 4, iRemainSize -= 4)
		{
			/* --- �f�[�^���� --- */
			TmpData  = (UINT32)*pBuf++;
			TmpData |= (UINT32)*pBuf++ <<  8;
			TmpData |= (UINT32)*pBuf++ << 16;
			TmpData |= (UINT32)*pBuf++ << 24;
			/* --- �o�b�t�@�������� --- */
			ARMPF_EP0_WRITE = TmpData;
		}
		/* --- �f�[�^���M������ --- */
		temp  = ARMPF_EP0_CONTROL;										/* CONTROL���W�X�^ ���[�h	*/
		temp |= D_EP0_DEND;											/* ���M����					*/

		/*-----------------------------*/
		/* �[���o�C�g(3byte�ȉ�)����   */
		/*-----------------------------*/
		if ((i < 64) && (iRemainSize > 0) && (iRemainSize < 4))
		{
			/* --- �[���f�[�^���� --- */
			TmpData = 0;
			for (i = 0; i < iRemainSize; i++)
			{
				TmpData |= (UINT32)*pBuf++ << (8*i);
			}
			/* --- �o�b�t�@�������� --- */
			ARMPF_EP0_WRITE = TmpData;
			/* --- �f�[�^���M���� --- */
			temp |= ( ((UINT32)iRemainSize) << 5 ) & ( D_EP0_DW );	/* �[���o�C�g�̐ݒ�			*/
			iRemainSize = 0;
		}
		/*-----------------------------*/
		/* �f�[�^���M����			   */
		/*-----------------------------*/
		ARMPF_EP0_CONTROL = temp;											/* CONTROL���W�X�^ ���C�g	*/
	}

	return size;
}

/***************************************************************************
  Description: EP0 �X�g�[����������
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
	ARMPF_EP0_CONTROL |= D_EP0_STL;				/* �X�g�[�������ݒ�			*/
}

#if 0	/* �����_�ł͎g�p���Ȃ����� <S150>*/
/***************************************************************************
  Description: EP0 NAK��������
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
	UINT32 temp;								/* �ݒ�ҏW�p���[�N				*/

	temp  = ARMPF_EP0_CONTROL;						/* EP0���䃌�W�X�^�l�ǂݏo��	*/
	temp |= D_EP0_INAK_EN;						/* EP0_INAK�������ݗL����		*/
	temp |= (UINT32)(D_EP0_INAK | D_EP0_ONAK);	/* IN/OUT/PING�g�[�N���ւ�		*/
												/* ����NAK�����ݒ�				*/
	ARMPF_EP0_CONTROL = temp;							/* EP0���䃌�W�X�^�֏�������	*/
}

/***************************************************************************
  Description: EP0 NAK��������
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
	UINT32 temp;								/* �ݒ�ҏW�p���[�N				*/

	temp  = ARMPF_EP0_CONTROL;						/* EP0���䃌�W�X�^�l�ǂݏo��	*/
	temp |= D_EP0_INAK_EN;						/* EP0_INAK�������ݗL����		*/
	temp &= ~(UINT32)(D_EP0_INAK | D_EP0_ONAK);	/* IN/OUT/PING�g�[�N���ւ�		*/
												/* ����NAK���������ݒ�			*/
	ARMPF_EP0_CONTROL = temp;							/* EP0���䃌�W�X�^�֏�������	*/
}

/***************************************************************************
  Description: EP0 NULL���M����
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
	/* �o�b�t�@�E�G���v�e�B�m�F	   */
	/*-----------------------------*/
	/* �o�b�t�@����ɂȂ�܂ŃE�G�C�g	*/
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return ER_SYS;
		}
	}

	/*-----------------------------*/
	/* �f�[�^���M����			   */
	/*-----------------------------*/
	ARMPF_EP0_CONTROL |= D_EP0_DEND;					/* ���M����					*/

	return ER_OK;
}
#endif

/***************************************************************************
  Description: EP0 No Data����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32	temp;					/* �ݒ�ҏW�p���[�N		*/
	ER_RET	result;					/* ����					*/

	/* �߂�l������	*/
	result = ER_SYS;

	/*=========================================*/
	/* �f�R�[�h����							   */
	/*=========================================*/
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result )
	{
		UsbFuncSetStallEP0();
		return result;
	}

	/*=========================================*/
	/* �X�e�[�^�X�E�X�e�[�W�ڍs�҂�			   */
	/*=========================================*/
	/* �X�e�[�^�X�E�X�e�[�W�Ɉڍs����܂ŃE�G�C�g */
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT)
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;							/* �v���N���A */

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{															/* �o�b�t�@����łȂ��Ƃ��C			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> �o�b�t�@����ɂ���B			*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
		{
			/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* IN�g�[�N������NAK����������			   */
	/*=========================================*/
	temp = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = temp;

	/*=========================================*/
	/* �X�e�[�^�X�X�e�[�W					   */
	/*=========================================*/
	/* NULL�f�[�^���M */
	/* ------ �o�b�t�@�E�G���v�e�B�m�F ------ */
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/* �o�b�t�@����ɂȂ�܂ŃE�G�C�g */
	{
		/*/ --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}

	/* ------ �f�[�^���M���� ------ */
	temp  = ARMPF_EP0_CONTROL;									/* CONTROL���W�X�^ ���[�h	*/
	temp |= D_EP0_DEND;										/* ���M����					*/
	ARMPF_EP0_CONTROL = temp;										/* CONTROL���W�X�^ ���C�g	*/

	/*----------------------------------------*/
	/* �X�e�[�^�X�X�e�[�W�I���m�F			  */
	/*----------------------------------------*/
	/* �X�e�[�^�X�E�X�e�[�W���I������܂ŃE�G�C�g */
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT)
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;						/* �v���N���A */

	return ER_OK;
}


/***************************************************************************
  Description: EP0 Control Read����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32	temp;					/* �ݒ�ҏW�p���[�N		*/
	ER_RET	result;					/* ����					*/
	UINT32	tcnt;					/* �^�C���A�E�g�J�E���^	<S1FE> */

	/* �߂�l������	*/
	result = ER_SYS;

	/*=========================================*/
	/* �o�b�t�@�E�G���v�e�B�m�F				   */
	/*=========================================*/
	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{															/* �o�b�t�@����łȂ��Ƃ��C			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> �o�b�t�@����ɂ���B			*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
		{
			/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* IN�g�[�N������NAK����������			   */
	/*=========================================*/
	temp = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = temp;

	/*=========================================*/
	/* �f�R�[�h����							   */
	/*=========================================*/
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	/*=========================================*/
	/* �X�e�[�^�X�E�X�e�[�W�ڍs�҂�			   */
	/*=========================================*/
	/* �X�e�[�^�X�E�X�e�[�W�Ɉڍs����܂ŃE�G�C�g */
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT)
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;								/* �v���N���A */

	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)
	{																/* �o�b�t�@����łȂ��Ƃ��C			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									/*  -> �o�b�t�@����ɂ���B			*/
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
		{
			/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* OUT�g�[�N������NAK����������			   */
	/*=========================================*/
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;

	/*----------------------------------------*/
	/* �X�e�[�^�X�X�e�[�W					  */
	/*----------------------------------------*/
	/* NULL�f�[�^��M */
	while((ARMPF_EP0_STATUS & D_EP0_OUT_NULL_INT) != D_EP0_OUT_NULL_INT)	/* NULL�f�[�^����M����܂ŃE�G�C�g */
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_OUT_NULL_INT;								/* �v���N���A */

	/*----------------------------------------*/
	/* �X�e�[�^�X�X�e�[�W�I���m�F			  */
	/*----------------------------------------*/
	/* �X�e�[�^�X�E�X�e�[�W���I������܂ŃE�G�C�g */
	KlibRstLongTimer(&tcnt);										/* �^�C�}�J�E���g�X�^�[�g <S1FE> */
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT)
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
		if( UsbFuncCheckSetupReceive() != 0 )
		{
			return result;
		}
		if( KlibGetLongTimerMs(tcnt) > 1000  )
		{/* 1s�ȏ㑱���ƃ^�C���A�E�g <S1FE> */
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;								/* �v���N���A */

	return ER_OK;
}


/***************************************************************************
  Description: EP0 Control Write����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32	temp;					/* �ݒ�ҏW�p���[�N		*/
	ER_RET	result;					/* ����					*/

	/* �߂�l������	*/
	result = ER_SYS;

	/*=========================================*/
	/* �o�b�t�@�E�G���v�e�B�m�F				   */
	/*=========================================*/
	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)
	{																/* �o�b�t�@����łȂ��Ƃ��C			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									/*  -> �o�b�t�@����ɂ���B			*/
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
		{
			/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* OUT�g�[�N������NAK����������			   */
	/*=========================================*/
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;

	/*=========================================*/
	/* �f�R�[�h����							   */
	/*=========================================*/
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result )
	{
		UsbFuncSetStallEP0();
		return result;
	}

	/*=========================================*/
	/* �X�e�[�^�X�E�X�e�[�W�ڍs�҂�			   */
	/*=========================================*/
	/* �X�e�[�^�X�E�X�e�[�W�Ɉڍs����܂ŃE�G�C�g */
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT)
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;							/* �v���N���A */

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
	{															/* �o�b�t�@����łȂ��Ƃ��C			*/
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> �o�b�t�@����ɂ���B			*/
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
		{
			/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
			{
				return result;
			}
		}
	}

	/*=========================================*/
	/* IN�g�[�N������NAK����������			   */
	/*=========================================*/
	temp = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = temp;

	/*----------------------------------------*/
	/* �X�e�[�^�X�X�e�[�W					  */
	/*----------------------------------------*/
	ARMPF_EP0_CONTROL |= D_EP0_DEND;

	/*----------------------------------------*/
	/* �X�e�[�^�X�X�e�[�W�I���m�F			  */
	/*----------------------------------------*/
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT)
	/* �X�e�[�^�X�E�X�e�[�W���I������܂ŃE�G�C�g */
	{
		/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
		{
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;							/* �v���N���A */

	return ER_OK;
}


/*=============================================================================*/
/* USB STANDARD REQUEST														   */
/*=============================================================================*/

/***************************************************************************
  Description: EP0 ���N�G�X�g�f�R�[�h����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET decode_request(DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET	result;					/* ����					*/

	/* �߂�l������	*/
	result = ER_SYS;

	/* �R���g���[���]���^�C�v���� */
	switch( pDeviceRequest->bmRequestType & USB_REQUEST )
	{
		/*-----------------------------*/
		/* �W���f�o�C�X�E���N�G�X�g	   */
		/*-----------------------------*/
		case USB_REQUEST_STANDARD:
			result = bmRequestType_standard( pDeviceRequest, pUsbfInfo );
			break;

		/*-----------------------------*/
		/* �N���X�E���N�G�X�g		   */
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
		/* �x���_�ŗL���N�G�X�g		   */
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
		/* ���̑�					   */
		/*-----------------------------*/
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 �X�^���_�[�h���N�G�X�g����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UCHAR	bRequest;			/* ���N�G�X�g�R�[�h	*/
	ER_RET	result;				/* ����				*/

	/* �ϐ������� */
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
  Description: EP0 CLEAR_FEATURE����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest )
{
	UINT8	recipient;				/* �����^�[�Q�b�g		*/
	UINT8	direction;				/* ���o�͕���			*/
	UINT16	selector;				/* �Z���N�^�l			*/
	UINT16	wIndex;					/* �C���f�b�N�X�l		*/
	UINT16	endpoint;				/* �G���h�|�C���g�ԍ�	*/
	ER_RET	result;					/* ����					*/
	USB_FUNCTION_IF	*pUsbfIf;		/* USB���̈�|�C���^	*/
	USB_INFO		*pUsbInfo;		/* USB���|�C���^		*/

	/* �ϐ������� */
	pUsbfIf = UsbFuncGetInfoAddress();
	pUsbInfo = &(pUsbfIf->UsbfInfo.usb);

	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	selector  = pDeviceRequest->wValue;
	wIndex    = pDeviceRequest->wIndex;
	endpoint  = wIndex & 0xFF;

	result = ER_SYS;

	/*---------------------------------*/
	/* �p�����[�^�E�`�F�b�N(�萔�ӏ�)  */
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
					/* �����[�g�E�F�C�N�A�b�v�֎~�ݒ� */
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
					/* �o�b�t�@�G���v�e�B�m�F */
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
					{															/* �o�b�t�@����łȂ��Ƃ��C			*/
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> �o�b�t�@����ɂ���B			*/
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
						{
							/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
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
  Description: EP0 CLEAR_FEATURE����( TARGET = EP )
----------------------------------------------------------------------------
  Parameters:
	num;				EP�ԍ�
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Clear_Feature_EP( UINT num )
{
	UINT32	temp;					/* �ݒ�ҏW�p���[�N		*/
	UINT32	temp2;					/* �ݒ�m�F�p���[�N		*/

	/* Parameter�m�F */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* EP Enable�m�F */
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN)
	{
		return ER_SYS;
	}

	/* EPn Clear Feature */
	if( (udev_get_DeviceState() & UDEV_ST_CONFIGURED) != 0 )
	{
		/* Buffer Clear */
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_BCLR;													/* ����M�o�b�t�@�E�N���A */
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 )
		{	/* IN */
			/* PID������ */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_IPIDCLR;											/* ���MPID�N���A */
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			/* �o�b�t�@�G���v�e�B�m�F */
			while( ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) ||
			       ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_DATA ) == D_EPn_IN_DATA ) )
			{	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B */
				/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
				if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT)
				{
					return ER_SYS;
				}
			}
			/* EP STALL���� */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp &= ~(D_EPn_ISTL);											/* IN�g�[�N�� STALL���� */
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		}
		else
		{	/* OUT */
			/* PID������ */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OPIDCLR;											/* ��MPID�N���A */
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			/* EP STALL���� */
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OSTL_EN;											/* EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���	*/
			temp &= ~(D_EPn_OSTL);											/* OUT/PING�g�[�N�� STALL����				*/
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
  Description: EP0 GET_CONFIGURATION����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET	result;					/* ����					*/

	/* �ϐ������� */
	result = ER_SYS;

	if( ( 0x0000 != pDeviceRequest->wValue  ) ||
	    ( 0x0000 != pDeviceRequest->wIndex  ) ||
	    ( 0x0001 != pDeviceRequest->wLength ) ||
	    ( USB_DIR != pDeviceRequest->bmRequestType ) )
	{
		return ER_SYS;
	}

	/* �R���t�B�M�����[�V�����l�́u1�v�̂݃T�|�[�g */
	result = UsbFuncWriteEP0( (UCHAR*)&(pUsbfInfo->usb.curr_config), 1 );
	if( result < 0 )
	{
		return result;
	}
	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_DESCRIPTOR����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	wValue;					/* �l					*/
	UINT16	wIndex;					/* �C���f�b�N�X�l		*/
	UINT16	descriptor_type;		/* �f�B�X�N���v�^�^�C�v	*/
	ER_RET	result;					/* ����					*/

	/* �ϐ������� */
	wValue			= pDeviceRequest->wValue;
	wIndex			= pDeviceRequest->wIndex;
	descriptor_type	= (wValue >> 8);

	result			= ER_SYS;

	/* ���N�G�X�g�^�C�v�̊m�F */
	if( USB_DIR != pDeviceRequest->bmRequestType )
	{
		return ER_SYS;
	}

	/* �f�B�X�N���v�^�^�C�v���ɏ������s�� */
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
  Description: EP0 GET_DESCRIPTOR DEVICE����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void*	pData;					/* DEVICE�f�B�X�N���v�^�̐擪�A�h���X	*/
	INT		size;					/* DEVICE�f�B�X�N���v�^�̃f�[�^�T�C�Y	*/
	ER_RET	result;					/* ����									*/

	/* �ϐ������� */
	size = 0;
	result = ER_SYS;

	/* DEVICE�f�B�X�N���v�^�̕ԋp */
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
  Description: EP0 GET_DESCRIPTOR CONFIGURATION����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATION�f�B�X�N���v�^�̐擪�A�h���X	*/
	INT				size;			/* CONFIGURATION�f�B�X�N���v�^�̃f�[�^�T�C�Y	*/
	ER_RET			result;			/* ����											*/

	/* �ϐ������� */
	size	= 0;
	result	= ER_SYS;

	/* CONFIGURATION�f�B�X�N���v�^�̕ԋp */
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

	/* ���M */
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
  Description: EP0 GET_DESCRIPTOR STRING����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void*	pData;					/* STRING�f�B�X�N���v�^�̐擪�A�h���X	*/
	INT		size;					/* STRING�f�B�X�N���v�^�̃f�[�^�T�C�Y	*/
	ER_RET	result;					/* ����									*/
	INT		string_num;				/* STRING�ԍ�							*/

	/* �ϐ������� */
	size	= 0;
	result	= ER_SYS;

	/* ���N�G�X�g���ꂽSTRING�ԍ� */
	string_num = (INT)(pDeviceRequest->wValue & 0xff);

	/* Not Support INDEX */
	if( ( string_num < 0 ) || ( STRIDX_MAX <= string_num ) )
	{
		return ER_SYS;
	}

	pData = (void*)(pUsbfInfo->desc.pString[string_num]);
	size  = (INT)(((STRING_DESCRIPTOR*)pData)->bLength);

	/* �f�[�^�T�C�Y���v���T�C�Y���傫���ꍇ */
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
  Description: EP0 GET_DESCRIPTOR DEVICE QUALIFIER����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void*	pData;					/* DEVICE QUALIFIER�f�B�X�N���v�^�̐擪�A�h���X	*/
	INT		size;					/* DEVICE QUALIFIER�f�B�X�N���v�^�̃f�[�^�T�C�Y	*/
	ER_RET	result;					/* ����											*/

	/* �ϐ������� */
	size	= 0;
	result	= ER_SYS;

	/* DEVICE QUALIFIER�f�B�X�N���v�^�̕ԋp */
	pData  = (void*)(pUsbfInfo->desc.pQualifier);
	size   = (INT)(((DEVICE_DESCRIPTOR*)pData)->bLength);

	/* �f�[�^�T�C�Y���v���T�C�Y���傫���ꍇ */
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
  Description: EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATION�f�B�X�N���v�^�̐擪�A�h���X	*/
	DESC_CONFIG		ConfigDescWork;	/* CONFIGURATION�f�B�X�N���v�^�R�s�[�p���[�N	*/
	INT				size;			/* CONFIGURATION�f�B�X�N���v�^�̃f�[�^�T�C�Y	*/
	ER_RET			result;			/* ����											*/

	/* �ϐ������� */
	size	= 0;
	result	= ER_SYS;

	/* CONFIGURATION�f�B�X�N���v�^�̕ԋp */
	if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE )
	{
		/* ����HighSpeed(Full Speed�f�X�N���v�^��ԋp) */
		pConfigDesc = pUsbfInfo->desc.pConf_fs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}
	else
	{
		/* ����FullSpeed�iHigh Speed�f�X�N���v�^��ԋp�j*/
		pConfigDesc = pUsbfInfo->desc.pConf_hs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}

	/* �f�[�^���R�s�[���āA�f�B�X�N���v�^�^�C�v��ύX���� */
	memcpy(&ConfigDescWork, pConfigDesc, sizeof(DESC_CONFIG));
	ConfigDescWork.conf_desc.bDescriptorType = (UINT8)0x07;

	/* �f�[�^�T�C�Y���v���T�C�Y���傫���ꍇ */
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
  Description: EP0 GET_INTERFACE����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR*	pIfDesc;	/* INTERFACE�f�B�X�N���v�^�̐擪�A�h���X	*/
	UINT8					data;		/* �ԋp�f�[�^���[�N							*/
	ER_RET					result;		/* ����										*/

	/* �ϐ������� */
	pIfDesc	= NULL;
	data	= (UINT8)0;
	result	= ER_SYS;

	/* ���N�G�X�g���e�`�F�b�N */
	if( ( 0x0000      != pDeviceRequest->wValue )	||
		( 0x0001      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x81 != pDeviceRequest->bmRequestType  ) )
	{
		return ER_SYS;
	}

	/* �f�o�C�X��Ԃ�CONFIGURED���m�F */
	if( UDEV_ST_CONFIGURED == udev_get_DeviceState() )
	{
		/* INTERFACE�f�B�X�N���v�^������ */
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
  Description: EP0 GET_STATUS����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT8	recipient;				/* �����^�[�Q�b�g	*/
	UINT8	direction;				/* ���o�͕���		*/
	ER_RET  result;					/* ����				*/

	/* �ϐ������� */
	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	result = ER_SYS;

	/* ���N�G�X�g���e�`�F�b�N */
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
  Description: EP0 GET_STATUS DEVICE����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	data;					/* �ԋp�f�[�^�ҏW�p		*/
	ER_RET	result;					/* ����					*/

	/* �ϐ������� */
	data	= 0;
	result	= ER_SYS;

	/* bmAttributes��SelfPowerd(D6)�r�b�g�𑗐M�f�[�^��D0�� */
	data = ( (UINT16)(pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) >> 6 ) & (UINT16)BIT0;

	/* �����[�g�E�F�C�N�A�b�v��D1�� */
	if( U2F_ENABLE == pUsbfInfo->usb.flag_RemoteWakeup )
	{
		data |= (UINT8)BIT1;
	}
	/* STATUS�f�[�^�̕ԋp */
	result = UsbFuncWriteEP0( (UCHAR*)&data, 2 );
	if( result < 0 )
	{
		return result;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 GET_STATUS ENDPOINT����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Get_Status_EP( DEVICE_REQUEST* pDeviceRequest )
{
	UINT	num;					/* EP�ԍ�				*/
	UINT8	direction;				/* ����					*/
	INT		status;					/* STALL���			*/
	UINT16	data;					/* �ԋp�f�[�^�ҏW�p		*/
	ER_RET	result;					/* ����					*/
	UINT16	wIndex;					/* �C���f�b�N�X�l		*/
	UINT	endpoint;				/* �G���h�|�C���g�ԍ�	*/

	/* �ϐ������� */
	num			= (UINT)( (pDeviceRequest->wIndex) & ~(UINT32)USB_DIR );
	direction	= (UINT8)(pDeviceRequest->wIndex & USB_DIR);
	data		= 0;
	result		= ER_SYS;
	wIndex		= pDeviceRequest->wIndex;
	endpoint	= (UINT)(wIndex & 0xFF);

	/* �G���h�|�C���g�ԍ��`�F�b�N */
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
			/* �����`�F�b�N */
			if( ER_OK != Check_USB_DIR_EPn( num, direction ) )
			{
				return ER_SYS;
			}

			status = EPn_get_STALL( num );	/* STALL��Ԏ擾 */
			if( status < 0 )
			{
				return ER_SYS;
			}
			if( 0 == status )
			{
				data = 0;			/* EPn��STALL���Ă��Ȃ�	*/
			}
			else
			{
				data = 1;			/* EPn��STALL���Ă���	*/
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
  Description: EP0 SET_ADDRESS����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_address( DEVICE_REQUEST* pDeviceRequest )
{
	UINT16	wValue;					/* �A�h���X�l			*/

	/* �ϐ������� */
	wValue = pDeviceRequest->wValue;

	/* ���N�G�X�g���e�`�F�b�N */
	if( ( pDeviceRequest->bmRequestType != (UINT8)0x00 )	||
		( pDeviceRequest->wIndex != 0x0000 )		||
		( pDeviceRequest->wLength != 0x0000 ) )
	{
		return ER_SYS;
	}

	/* �A�h���X�l�`�F�b�N */
	if( wValue != ( wValue & 0x007F ) )
	{
		return ER_SYS;
	}

	ARMPF_USB_ADDRESS = (UINT32)wValue << 16;

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SET_CONFIGURATION����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	ConfigValue;			/* CONFIGURATION�ԍ�	*/
	UINT32	tmp;					/* �ݒ�ҏW�p���[�N		*/
	ER_RET	result;					/* ����					*/

	/* �ϐ������� */
	ConfigValue	= pDeviceRequest->wValue & 0x00ff;
	result		= ER_SYS;

	/* ���N�G�X�g���e�`�F�b�N */
	if( ( 0x0000      != pDeviceRequest->wIndex )	||
		( 0x0000      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x00 != pDeviceRequest->bmRequestType  ) )
	{
		return ER_SYS;
	}

	/* CONFIGURATION�ԍ����̏������s�� */
	switch( ConfigValue )
	{
		case 0:
			/* Address�X�e�[�g�ɖ߂� */
			pUsbfInfo->usb.curr_config = 0;
			ARMPF_USB_CONTROL &= ~D_USB_CONF;

			/* --- EP �o�b�t�@�E�N���A --- */
			ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							/* EP1 : �o�b�t�@�E�N���A		*/
			ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							/* EP2 : �o�b�t�@�E�N���A		*/
			/* --- PID������ --- */
			ARMPF_EP1_CONTROL |= D_EPn_IPIDCLR;							/* EP1 : ���MPID�N���A			*/
			ARMPF_EP2_CONTROL |= D_EPn_OPIDCLR;							/* EP2 : ��MPID�N���A			*/
			/* --- EP STALL���� --- */
			tmp = ARMPF_EP1_CONTROL;
			tmp &= ~(D_EPn_ISTL);									/* EP1 : IN�g�[�N�� STALL����	*/
			ARMPF_EP1_CONTROL = tmp;

			tmp = ARMPF_EP2_CONTROL;
			tmp |= D_EPn_OSTL_EN;									/* EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���	*/
			tmp &= ~(D_EPn_OSTL);									/* EP2 : OUT/PING�g�[�N�� STALL����			*/
			ARMPF_EP2_CONTROL = tmp;

			result = ER_OK;
			break;
		case 1:
			/* Configured�X�e�[�g */
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
			/* �T�|�[�g�O��0��ݒ� */
			pUsbfInfo->usb.curr_config = 0;
			result = ER_SYS;
			break;
	}
	return result;
}

/***************************************************************************
  Description: EP0 SET_DESCRIPTOR����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest )
{
	/* TODO: SET_DESCRIPTOR�ɑΉ�����ꍇ�͏�����ǉ�����(�����STALL��������) */
/*	return ER_NOTSUP; */
	return ER_SYS;
}

/***************************************************************************
  Description: EP0 SET_FEATURE����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT8	recipient;				/* �����^�[�Q�b�g		*/
	UINT8	direction;				/* ���o�͕���			*/
	UINT16	selector;				/* �Z���N�^�l			*/
	UINT16	wIndex;					/* �C���f�b�N�X�l		*/
	UINT	endpoint;				/* �G���h�|�C���g�ԍ�	*/
	ER_RET	result;					/* ����					*/
	UINT	num;					/* �ԍ�					*/
	UINT8	dir;					/* ����					*/

	/* �ϐ������� */
	recipient	= (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction	= (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	selector	= pDeviceRequest->wValue;
	wIndex		= pDeviceRequest->wIndex;
	endpoint	= wIndex & 0xFF;
	result		= ER_SYS;

	/* ���N�G�X�g���e�`�F�b�N */
	if( ( 0x0000 != pDeviceRequest->wLength ) ||
		( USB_DIR_OUT != direction ) )
	{
		return ER_SYS;
	}

	/* �����^�[�Q�b�g���ɏ������s�� */
	switch( recipient )
	{
		case USB_TARGET_DEVICE:
			result = Set_Feature_Device( selector, wIndex, pUsbfInfo );
			break;
		case USB_TARGET_INTERFACE:
			result = ER_SYS;
			break;
		case USB_TARGET_ENDPOINT:
			/* �C���f�b�N�X�l�`�F�b�N */
			if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
				 ( 0x0000 != ( wIndex & 0x0070 ) ) )
			{
				return ER_SYS;
			}
			/* �Z���N�^�l�`�F�b�N */
			if( USB_FEATURE_ENDPOINT_HALT == selector )
			{
				/* �G���h�|�C���g�ԍ��`�F�b�N */
				if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) )
				{
					/* EP0 */
					/* �o�b�t�@�G���v�e�B�m�F */
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)
					{															/* �o�b�t�@����łȂ��Ƃ��C			*/
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;								/*  -> �o�b�t�@����ɂ���B			*/
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY)	/*  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B	*/
						{
							/* --- �P�[�u���������ꂽ�Ƃ��͋����I�� --- */
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

					/* �����`�F�b�N */
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
  Description: EP0 SET_FEATURE����( TARGET = DEVICE )
----------------------------------------------------------------------------
  Parameters:
	selector;			USB STD FEATURE Slectors
	wIndex;				�C���f�b�N�X�l
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo )
{
	ER_RET	result;					/* ����					*/
	UINT32	temp;					/* �ݒ�ҏW�p���[�N		*/
	UINT32	tst_mode;				/* �ݒ�ҏW�p���[�N		*/

	/* �ϐ������� */
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
					/* �����[�g�E�F�C�N�A�b�v�t���O�̍X�V */
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
				temp     = ARMPF_USB_CONTROL;									/* CONTROL���W�X�^�̑ޔ�	*/
				tst_mode = (UINT32)wIndex;								/* wIndex��UINT32��			*/
				tst_mode = ( tst_mode << 16 ) & D_USB_USBTESTMODE;		/* TSTMODE�r�b�g�փV�t�g	*/
				temp &= ~(UINT32)D_USB_USBTESTMODE;						/* TSTMODE�̈����U�N���A	*/
				temp |= tst_mode;										/* �ݒ�l					*/

				/* ���W�X�^�֏����� */
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
  Description: EP0 SET_FEATURE����( TARGET = EP )
----------------------------------------------------------------------------
  Parameters:
	num;				EP�ԍ�
  Return:
	ER_OK; 				����
	ER_PARAM;			�p�����[�^�G���[
	ER_SYS;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Set_Feature_EP( UINT num )
{
	/* Parameter�m�F */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* EP Enable�m�F */
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN)
	{
		return ER_SYS;
	}

	/* EPn Clear Feature */
	if( ( udev_get_DeviceState() & UDEV_ST_CONFIGURED ) != 0 )
	{
		/* EP STALL�ݒ� */
		UsbFuncSetStallEPn( num );
	}
	else
	{
		return ER_SYS;
	}

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SET_INTERFACE����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR*	pIfDesc;	/* INTERFACE�f�B�X�N���v�^�̐擪�A�h���X	*/

	/* �ϐ������� */
	pIfDesc = NULL;

	/* ���N�G�X�g���e�`�F�b�N */
	if( ( 0x0000      != pDeviceRequest->wLength )	||
	    ( 0x0000      != pDeviceRequest->wValue  )	||			/* Alternate Setting��Ή��̂��߁CwValue�ł͂��� */
	    ( (UINT8)0x01 != pDeviceRequest->bmRequestType  ) )
	{
		return ER_SYS;
	}

	/* �R���t�B�O���[�V�����ς݃`�F�b�N */
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
	/* �w���Interface�ԍ��Ńf�o�C�X�̐ݒ�����{����ꍇ	 */
	/* �������牺�ɒǉ�										 */
	/*-------------------------------------------------------*/
	/* pIfDesc                = �C���^�[�t�F�[�X�f�X�N���v�^	*/
	/* pDeviceRequest->wIndex = �C���^�[�t�F�[�X�ԍ�			*/
	/* pDeviceRequest->wValue = ��֐ݒ�l						*/

	return ER_OK;
}

/***************************************************************************
  Description: EP0 SYNCH_FRAME����
----------------------------------------------------------------------------
  Parameters:
	pDeviceRequest;		�f�o�C�X���N�G�X�g
  Return:
	ER_OK; 				����
	���̑�;				���s
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest )
{
	/* TODO: SYNCH_FRAME�ɑΉ�����ꍇ�͏�����ǉ�����(�����STALL��������) */
/*	return ER_NOTSUP; */
	return ER_SYS;
}

/***************************************************************************
  Description: EP������USB_DIR_OUT/IN�ϊ�����
----------------------------------------------------------------------------
  Parameters:
	num;				EP�ԍ�
	direction;			��r����
  Return:
	ER_OK; 				direction�Ɠ���
	ER_SYS;				direction�ƈႤ���G���[
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static ER_RET Check_USB_DIR_EPn( UINT num, UINT8 direction )
{
	UINT8	dir;					/* �����`�F�b�N�p���[�N	*/

	/* EPn�ԍ��m�F */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* �����`�F�b�N */
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 )
	{	/* OUT */
		dir = USB_DIR_OUT;
	}
	else
	{	/* IN */
		dir = USB_DIR_IN;
	}

	/* direction�Ɣ�r */
	if( dir != direction )
	{
		return ER_SYS;
	}

	return ER_OK;
}

/***************************************************************************
  Description: �f�o�C�X�X�e�[�g�擾����
----------------------------------------------------------------------------
  Parameters:
	none
  Return:
	UDEV_ST_IDLE; 		�A�C�h���X�e�[�g
	UDEV_ST_DEFAULT;	�f�t�H���g�X�e�[�g
	UDEV_ST_ADDRESS;	�A�h���X�X�e�[�g
	UDEV_ST_CONFIGURED;	�R���t�B�M���A�[�h�X�e�[�g
	UDEV_ST_SUSPENDED;	�T�X�y���h�X�e�[�g
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static UINT32 udev_get_DeviceState( void )
{
	UINT32	epctr;					/* EPC���Ӑ��䃌�W�X�^�l	*/
	UINT32	state;					/* USB�X�e�[�^�X���W�X�^�l	*/

	/* EPC���Ӑ��䃌�W�X�^�l�擾 */
	epctr	= ARMPF_EPCTR;

	if( 0 == (epctr & D_SYS_VBUS_LEVEL ) )
	{
		/* Idle(Attached)�X�e�[�g */
		return UDEV_ST_IDLE;
	}

	if( 0 == (epctr & D_SYS_PLL_LOCK ) )
	{
		/* Suspend�X�e�[�g */
		return UDEV_ST_SUSPENDED;
	}

	/* USB�X�e�[�^�X���W�X�^�l�擾 */
	state = ARMPF_USB_STATUS;

	if( 0 == ( state & ( D_USB_CONF_ST | D_USB_DEFAULT ) ) )
	{
		return UDEV_ST_IDLE;
	}

	if( ( state & D_USB_SPND_OUT ) != 0 )
	{
		/* Suspend�X�e�[�g */
		return UDEV_ST_SUSPENDED;
	}

	if( ( state & D_USB_CONF_ST ) != 0 )
	{
		/* Configured�X�e�[�g */
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
				/* Default�X�e�[�g */
				return UDEV_ST_DEFAULT;
			}
		}
	}

	return UDEV_ST_IDLE;
}

/***************************************************************************
  Description: info �J�����g�R���t�B�M�����[�V�����f�X�N���v�^�̎擾����
----------------------------------------------------------------------------
  Parameters:
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	�J�����g�R���t�B�M�����[�V�����f�X�N���v�^
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static DESC_CONFIG* info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATION�f�B�X�N���v�^�̐擪�A�h���X	*/

	/* �ϐ������� */
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
		/* �����Ȃ� */
	}
	return pConfigDesc;
}

/***************************************************************************
  Description: info �C���^�[�t�F�[�X�f�X�N���v�^�̎擾����
----------------------------------------------------------------------------
  Parameters:
	index;				�C���^�[�t�F�[�X�ԍ�
	pUsbfInfo;			USB�f�o�C�X���̈�ւ̃|�C���^
  Return:
	�C���^�[�t�F�[�X�f�X�N���v�^
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static INTERFACE_DESCRIPTOR* info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc;	/* CONFIGURATION�f�B�X�N���v�^�̐擪�A�h���X	*/
	UINT			NumIf;			/* 	*/

	/* �ϐ������� */
	pConfigDesc = NULL;

	/* �C���^�[�t�F�[�X�ԍ��`�F�b�N */
	if( U2F_IF_MAX <= index )
	{
		return NULL;
	}

	pConfigDesc = info_get_CurrConfigDesc( pUsbfInfo );
	if( NULL == pConfigDesc )
	{
		return NULL;
	}

	/* �C���^�[�t�F�[�X�̌��� */
	for( NumIf = 0; NumIf <= (UINT)(pConfigDesc->conf_desc.bNumInterfaces); NumIf++ )
	{
		/* �w��C���^�[�t�F�[�X�ԍ��ƈ�v���邩�`�F�b�N */
		if( index == (UINT16)(pConfigDesc->if_desc[NumIf].bInterfaceNumber) )
		{
			/* found InterfaceDescriptor */
			return (INTERFACE_DESCRIPTOR*)(&pConfigDesc->if_desc[NumIf]);
		}
	}

	return NULL;	/* Not Found. */
}

/***************************************************************************
  Description: EPn STALL��Ԏ擾����
----------------------------------------------------------------------------
  Parameters:
	num;				EP�ԍ�
  Return:
	0;					STALL���Ă��Ȃ�
	1;					STALL���Ă���
	ER_PARAM;			�p�����[�^�G���[
----------------------------------------------------------------------------
  Note:
	none
****************************************************************************/
static INT EPn_get_STALL( UINT num )
{
	UINT32 status;

	/* Parameter�m�F */
	if(( num < 1 ) || ( num > 9))
	{
		return ER_PARAM;
	}

	/* EP Enable�m�F */
	status = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
	if( ( status & D_EPn_EN ) == 0 )
	{
		return ER_PARAM;
	}

	/* Halt�iSTALL�j�m�F */
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 )
	{	/* OUT */
		/* STALL�`�F�b�N */
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
		/* STALL�`�F�b�N */
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
