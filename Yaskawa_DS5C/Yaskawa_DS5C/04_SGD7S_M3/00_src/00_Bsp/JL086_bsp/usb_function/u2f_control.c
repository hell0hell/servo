/**
 *******************************************************************************
 * @file  u2f_control.c
 * @brief USB request functions for usb function
 * @attention 
 * @note 
 * 
 * $Rev:: 338                        $
 * $Date:: 2012-02-20 21:33:53 +0900#$
 *******************************************************************************
 */
/*
 *------------------------------------------------------------------------------
 * Notice 
 *  This is a sample program. 
 *  Renesas Electronics assumes no responsibility for any losses incurred. 
 *
 * Copyright(C) 2011 Renesas Electronics Corporation 
 * Copyright(C) 2011 Renesas Micro Systems Co., Ltd. 
 *------------------------------------------------------------------------------
 */

/*============================================================================*/
/* I N C L U D E                                                              */
/*============================================================================*/
#include "Basedef.h"
#include "errcodes.h"
#include "arm.h"				//@
#include "ARMPF_USB_FUNC.h"
#include "nbpfahb32vic128.h"	//@
#include "usb_f.h"
#include "u2f_desc.h"

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
/// USB Device State
typedef INT					UDEV_ST;
#define UDEV_ST_IDLE		BIT0
#define UDEV_ST_DEFAULT		BIT1
#define UDEV_ST_ADDRESS		BIT2
#define UDEV_ST_CONFIGURED	BIT3
#define UDEV_ST_SUSPENDED	BIT4

// U2F FLAG
#define U2F_ENABLE			1
#define U2F_DISABLE			0

//#define NULL				0


/*===========================================================================*/
/* S T R U C T                                                               */
/*===========================================================================*/

/*============================================================================*/
/* V A R I A B L E                                                            */
/*============================================================================*/

/*============================================================================*/
/* S T A T I C  F U N C                                                       */
/*============================================================================*/
//-------------------------------------
// ���N�G�X�g�E�^�C�v�ʏ���
//-------------------------------------
static	ER_RET	control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 No Data���� */
static	ER_RET	control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 Control Read���� */
static	ER_RET	control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 Control Write���� */

//-------------------------------------
// �X�^���_�[�h�E���N�G�X�g����
//-------------------------------------
static	ER_RET	decode_request( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 ���N�G�X�g�f�R�[�h���� */

static	ER_RET	bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 �X�^���_�[�h���N�G�X�g���� */
// --- CLEAR_FEATURE     ( 1) [No data] ---
static	ER_RET	std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest );								/* EP0 CLEAR_FEATURE���� */
static	ER_RET	Clear_Feature_EP( UINT num );															/* EP0 CLEAR_FEATURE����( TARGET = EP ) */
// --- GET_CONFIGURATION ( 8) [Read]    ---
static	ER_RET	std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_CONFIGURATION���� */
// --- GET_DESCRIPTOR    ( 6) [Read]    ---
static	ER_RET	std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR���� */
static	ER_RET	Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR DEVICE���� */
static	ER_RET	Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 GET_DESCRIPTOR CONFIGURATION���� */
static	ER_RET	Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR STRING���� */
static	ER_RET	Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR DEVICE QUALIFIER���� */
static	ER_RET	Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION���� */
// --- GET_INTERFACE     (10) [Read]    ---
static	ER_RET	std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_INTERFACE���� */
// --- GET_STATUS        ( 0) [Read]    ---
static	ER_RET	std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS���� */
static	ER_RET	Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS DEVICE���� */
static	ER_RET	Get_Status_EP( DEVICE_REQUEST* pDeviceRequest );										/* EP0 GET_STATUS ENDPOINT���� */
// --- SET_ADDRESS       ( 5) [No data] ---
static	ER_RET	std_req_set_address( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SET_ADDRESS���� */
// --- SET_CONFIGURATION ( 9) [No data] ---
static	ER_RET	std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 SET_CONFIGURATION���� */
// --- SET_DESCRIPTOR    ( 7) [Write]   ---
static	ER_RET	std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest );								/* EP0 SET_DESCRIPTOR���� */
// --- SET_FEATURE       ( 3) [No data] ---
static	ER_RET	std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 SET_FEATURE���� */
static	ER_RET	Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo );									/* EP0 SET_FEATURE����( TARGET = DEVICE ) */
static	ER_RET	Set_Feature_EP( UINT num );																/* EP0 SET_FEATURE����( TARGET = EP ) */
// --- SET_INTERFACE     (11) [No data] ---
static	ER_RET	std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 SET_INTERFACE���� */
// --- SYNC_FRAME        (12) [No data] ---
static	ER_RET	std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SYNCH_FRAME���� */

static	ER_RET	Check_USB_DIR_EPn( UINT num, UINT8 direction );											/* EP������USB_DIR_OUT/IN�ϊ����� */
static	UINT32	udev_get_DeviceState( void );															/* �f�o�C�X�X�e�[�g�擾 */
static	DESC_CONFIG*			info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo );							/* info �J�����g�R���t�B�M�����[�V�����f�X�N���v�^�̎擾 */
static	INTERFACE_DESCRIPTOR*	info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo );			/* info �C���^�[�t�F�[�X�f�X�N���v�^�̎擾 */
static	INT						EPn_get_STALL( UINT num );												/* EPn STALL��Ԏ擾�֐� */

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/

/**
 *******************************************************************************
  @brief �R���g���[���]���^�X�N����
  @param  �Ȃ�
  @retval �Ȃ�
 *******************************************************************************
*/
void UsbFuncTaskControl( void )
{
	ER_RET	result;
	USB_FUNCTION_IF	*pUsbIf;
	U2F_INFO		*pUsbfInfo;
	DEVICE_REQUEST	*pDeviceRequest;

	pUsbIf = UsbFuncGetInfoAddress();
	pUsbfInfo = &(pUsbIf->UsbfInfo);
	pDeviceRequest = &(pUsbIf->UsbfInfo.usb.request);

	//=========================================
	// EP0 ���� (SETUP�f�[�^��M)
	//=========================================
	if (pUsbIf->UsbfFlag.UsbSetupReceive == 1) {
		pUsbIf->UsbfFlag.UsbSetupReceive = 0;
		// ------ �f�o�C�X�E���N�G�X�g ���[�h ------
		pDeviceRequest->bmRequestType = (UINT8) ((ARMPF_SETUP_DATA0 >>  0) & 0xff  );
		pDeviceRequest->bRequest      = (UINT8) ((ARMPF_SETUP_DATA0 >>  8) & 0xff  );
		pDeviceRequest->wValue        = (UINT16)((ARMPF_SETUP_DATA0 >> 16) & 0xffff);
		pDeviceRequest->wIndex        = (UINT16)((ARMPF_SETUP_DATA1 >>  0) & 0xffff);
		pDeviceRequest->wLength       = (UINT16)((ARMPF_SETUP_DATA1 >> 16) & 0xffff);

		// ------ ���N�G�X�g�E�^�C�v���� ------
		if( pDeviceRequest->wLength == 0x0000 ) {								// �f�[�^�E�X�e�[�W�̃o�C�g����0�̂Ƃ�
			result = control_nodata( pDeviceRequest, pUsbfInfo );				//  -> STATUS������
		} else {																// �f�[�^�E�X�e�[�W�̃o�C�g����0�ł͂Ȃ��C
			if( (pDeviceRequest->bmRequestType & USB_DIR_IN) == USB_DIR_IN ) {	// �f�[�^�]�������� Host=>Dev �̂Ƃ��C
				result = control_read( pDeviceRequest, pUsbfInfo );				//  -> DATA IN & STATUS������
			} else {															// Dev=>Host �̂Ƃ��C
				result = control_write( pDeviceRequest, pUsbfInfo );			//  -> DATA OUT & STATUS������
			}
		}
		// ------ �G���[���� ---
		if (result != ER_OK) {
			// ToDo
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		�t�r�a�|�[�g��Ԓʒm�T�[�r�X�֐�															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		�A�v���P�[�V�����w�ʐM���\�ł��邩��ԐM���܂��B											*/
/*			�EUSB�ł́ASET_CONFIGURATION Val = 1 ��Host��Device�Ɏw�߂����Endpoint0�ȊO��(�{�h��	*/
/*			  �C�o�ł�1��2)���g�p�\�ɂȂ�܂��B�A�v���P�[�V�����ʐM��EP1��EP2���g�p���܂��B		*/
/*																									*/
/****************************************************************************************************/
ULONG	UsbFuncPortConfiguration( void )
{
	ULONG rc = FALSE;
	UINT32	ret;

	ret = udev_get_DeviceState();
	if( ret == UDEV_ST_CONFIGURED)
	{
		rc = TRUE;
	}

	return( rc );

}

/**
 ******************************************************************************
  @brief EP0 No Data����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32 data;
	UINT32 temp;

	ER_RET	 result	= ER_SYS;

	//=========================================
	// �f�R�[�h����
	//=========================================
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	//=========================================
	// �X�e�[�^�X�E�X�e�[�W�ڍs�҂�
	//=========================================
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT) {	// �X�e�[�^�X�E�X�e�[�W�Ɉڍs����܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;									// �v���N���A

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {				// �o�b�t�@����łȂ��Ƃ��C
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;										//  -> �o�b�t�@����ɂ���B
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {		//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// IN�g�[�N������NAK����������
	//=========================================
	data = ARMPF_EP0_CONTROL;
	data |= D_EP0_INAK_EN;
	data &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = data;

	//=========================================
	// �X�e�[�^�X�X�e�[�W
	//=========================================
	// NULL�f�[�^���M
	// ------ �o�b�t�@�E�G���v�e�B�m�F ------
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  �o�b�t�@����ɂȂ�܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}

	// ------ �f�[�^���M���� ------
	temp  = ARMPF_EP0_CONTROL;											// CONTROL���W�X�^ ���[�h
	temp |= D_EP0_DEND;												// ���M����
	ARMPF_EP0_CONTROL = temp;												// CONTROL���W�X�^ ���C�g

	//----------------------------------------
	// �X�e�[�^�X�X�e�[�W�I���m�F
	//----------------------------------------
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT) {	// �X�e�[�^�X�E�X�e�[�W���I������܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;									// �v���N���A

	return ER_OK;
}


/**
 *******************************************************************************
  @brief EP0 Control Read����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 *******************************************************************************
*/
static ER_RET control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32 data;

	ER_RET result = ER_SYS;

	//=========================================
	// �o�b�t�@�E�G���v�e�B�m�F
	//=========================================
	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {				// �o�b�t�@����łȂ��Ƃ��C
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;										//  -> �o�b�t�@����ɂ���B
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {		//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// IN�g�[�N������NAK����������
	//=========================================
	data = ARMPF_EP0_CONTROL;
	data |= D_EP0_INAK_EN;
	data &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = data;

	//=========================================
	// �f�R�[�h����
	//=========================================
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	//=========================================
	// �X�e�[�^�X�E�X�e�[�W�ڍs�҂�
	//=========================================
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT) {	// �X�e�[�^�X�E�X�e�[�W�Ɉڍs����܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;									// �v���N���A

	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {				// �o�b�t�@����łȂ��Ƃ��C
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;										//  -> �o�b�t�@����ɂ���B
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {		//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// OUT�g�[�N������NAK����������
	//=========================================
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;

	//----------------------------------------
	// �X�e�[�^�X�X�e�[�W
	//----------------------------------------
	// NULL�f�[�^��M
	while((ARMPF_EP0_STATUS & D_EP0_OUT_NULL_INT) != D_EP0_OUT_NULL_INT) {	// NULL�f�[�^����M����܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_OUT_NULL_INT;									// �v���N���A

	//----------------------------------------
	// �X�e�[�^�X�X�e�[�W�I���m�F
	//----------------------------------------
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT) {		// �X�e�[�^�X�E�X�e�[�W���I������܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ���ASETUP�g�[�N������M�����Ƃ��͋����I�� ---
		if( (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) || (UsbFuncCheckSetupReceive() != 0) ){
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;									// �v���N���A

	return ER_OK;
}


/**
 *******************************************************************************
  @brief EP0 Control Write����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 *******************************************************************************
*/
static ER_RET control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;
	UINT32 data;

	//=========================================
	// �o�b�t�@�E�G���v�e�B�m�F
	//=========================================
	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {			// �o�b�t�@����łȂ��Ƃ��C
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> �o�b�t�@����ɂ���B
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// OUT�g�[�N������NAK����������
	//=========================================
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;


	//=========================================
	// �f�R�[�h����
	//=========================================
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	//=========================================
	// �X�e�[�^�X�E�X�e�[�W�ڍs�҂�
	//=========================================
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT) {	// �X�e�[�^�X�E�X�e�[�W�Ɉڍs����܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;									// �v���N���A

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {			// �o�b�t�@����łȂ��Ƃ��C
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> �o�b�t�@����ɂ���B
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
			// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// IN�g�[�N������NAK����������
	//=========================================
	data = ARMPF_EP0_CONTROL;
	data |= D_EP0_INAK_EN;
	data &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = data;

	//----------------------------------------
	// �X�e�[�^�X�X�e�[�W
	//----------------------------------------
	ARMPF_EP0_CONTROL |= D_EP0_DEND;

	//----------------------------------------
	// �X�e�[�^�X�X�e�[�W�I���m�F
	//----------------------------------------
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT) {	// �X�e�[�^�X�E�X�e�[�W���I������܂ŃE�G�C�g
		// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;									// �v���N���A

	return ER_OK;
}




//=============================================================================
// USB STANDARD REQUEST
//=============================================================================

/**
 ******************************************************************************
  @brief EP0 ���N�G�X�g�f�R�[�h����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET decode_request(DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;

	// �R���g���[���]���^�C�v����
	switch( pDeviceRequest->bmRequestType & USB_REQUEST )
	{
		//-----------------------------
		// �W���f�o�C�X�E���N�G�X�g
		//-----------------------------
		case USB_REQUEST_STANDARD:
			result = bmRequestType_standard( pDeviceRequest, pUsbfInfo );
			break;

		//-----------------------------
		// �N���X�E���N�G�X�g
		//-----------------------------
		case USB_REQUEST_CLASS:
			if( pUsbfInfo->cb_class_req ) {
				result = pUsbfInfo->cb_class_req( pDeviceRequest );
			} else {
				result = ER_SYS;
			}
			break;

		//-----------------------------
		// �x���_�ŗL���N�G�X�g
		//-----------------------------
		case USB_REQUEST_VENDOR:
			if( pUsbfInfo->cb_vendor_req ) {
				result = pUsbfInfo->cb_vendor_req( pDeviceRequest );
			} else {
				result = ER_SYS;
			}
			break;

		//-----------------------------
		// ���̑�
		//-----------------------------
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/**
 ******************************************************************************
  @brief EP0 �X�^���_�[�h���N�G�X�g����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UCHAR bRequest = pDeviceRequest->bRequest;
	ER_RET result = ER_SYS;

	switch( bRequest ) {
		case USB_STD_GET_STATUS:							// GET STATUS
			result = std_req_get_status( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_CLEAR_FEATURE:							// CLEAR FEATURE
			result = std_req_clear_feature( pDeviceRequest );
			break;
		case USB_STD_SET_FEATURE:							// SET FEATURE
			result = std_req_set_feature( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_ADDRESS:							// SET ADDRESS
			result = std_req_set_address( pDeviceRequest );
			break;
		case USB_STD_GET_DESCRIPTOR:						// GET DESCRIPTOR
			result = std_req_get_descriptor( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_DESCRIPTOR:						// SET DESCRIPTOR
			result = std_req_set_descriptor( pDeviceRequest );
			break;
		case USB_STD_GET_CONFIGURATION:						// GET CONFIGURATION
			result = std_req_get_configuration( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_CONFIGURATION:						// SET CONFIGURATION
			result = std_req_set_configuration( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_GET_INTERFACE:							// GET INTERFACE
			result = std_req_get_interface( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SET_INTERFACE:							// SET INTERFACE
			result = std_req_set_interface( pDeviceRequest, pUsbfInfo );
			break;
		case USB_STD_SYNCH_FRAME:							// SYNC FRAME
			result = std_req_synch_frame( pDeviceRequest );
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}


/**
 ******************************************************************************
  @brief EP0 CLEAR_FEATURE����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest )
{
	UINT8	recipient;
	UINT8	direction;
	UINT16	selector;
	UINT16	wIndex;
	UINT16	endpoint;

	ER_RET  result;

	USB_FUNCTION_IF	*pUsbIf;
	USB_INFO		*pUsbInfo;

	pUsbIf = UsbFuncGetInfoAddress();
	pUsbInfo = &(pUsbIf->UsbfInfo.usb);

	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	selector  = pDeviceRequest->wValue;
	wIndex    = pDeviceRequest->wIndex;
	endpoint  = wIndex & 0xFF;

	result = ER_SYS;


	//---------------------------------
	// �p�����[�^�E�`�F�b�N(�萔�ӏ�)
	//---------------------------------
	if( ( 0x0000 != pDeviceRequest->wLength ) ||
		( USB_DIR_OUT != direction ) ) {
		return ER_SYS;
	}

	switch( recipient ) {
		case USB_TARGET_DEVICE:
			if( 0x0000 == wIndex ) {
				if( USB_FEATURE_DEVICE_REMOTE_WAKEUP == selector ) {
					// �����[�g�E�F�C�N�A�b�v�֎~�ݒ�
					pUsbInfo->flag_RemoteWakeup = U2F_DISABLE;
					result = ER_OK;
				} else {
					result = ER_SYS;
				}
			} else {
				result = ER_SYS;
			}
			break;
		case USB_TARGET_INTERFACE:
			result = ER_SYS;
			break;
		case USB_TARGET_ENDPOINT:

			if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
				 ( 0x0000 != ( wIndex & 0x0070 ) ) ) {
				return ER_SYS;
			}

			if( USB_FEATURE_ENDPOINT_HALT == selector ) {
				if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) ) {
					// EP0
					// �o�b�t�@�G���v�e�B�m�F
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {			// �o�b�t�@����łȂ��Ƃ��C
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> �o�b�t�@����ɂ���B
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
							// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
							if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
								return ER_SYS;
							}
						}
					}

					result = ER_OK;
				} else {
					// Other EP
					if( ER_OK != Clear_Feature_EP( endpoint & ~((UINT)USB_DIR)) ) {
						return ER_SYS;
					}
				}
				result = ER_OK;
			} else {
				return ER_SYS;
			}
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/**
 ******************************************************************************
  @brief EP0 CLEAR_FEATURE����( TARGET = EP )
  @param  num    -- EP�ԍ�
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Clear_Feature_EP( UINT num )
{
	UINT32 temp;
	UINT32 temp2;

	// Parameter�m�F
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// EP Enable�m�F
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN) {
		return ER_SYS;
	}

	// EPn Clear Feature
	if( (udev_get_DeviceState() & UDEV_ST_CONFIGURED) != 0 ) {
		// Buffer Clear
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_BCLR;													// ����M�o�b�t�@�E�N���A
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {  // IN
			// PID������
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_IPIDCLR;											// ���MPID�N���A
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			// �o�b�t�@�G���v�e�B�m�F
			while( ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) ||
			       ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_DATA ) == D_EPn_IN_DATA ) ) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
				// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
				if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
					return ER_SYS;
				}
			}
			// EP STALL����
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp &= ~(D_EPn_ISTL);											// IN�g�[�N�� STALL����
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		} else {
			// PID������
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OPIDCLR;											// ��MPID�N���A
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			// EP STALL����
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OSTL_EN;											// EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���
			temp &= ~(D_EPn_OSTL);											// OUT/PING�g�[�N�� STALL����
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		}

#if 0
		// EP STALL����
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_OSTL_EN;										// EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���
		temp &= ~(D_EPn_OSTL | D_EPn_ISTL);							// IN/OUT/PING�g�[�N�� STALL����
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		// PID������ & Buffer Clear
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= (D_EPn_OPIDCLR | D_EPn_IPIDCLR | D_EPn_BCLR);		// ����MPID�N���A�C�o�b�t�@�E�N���A
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {  // IN
//			// Check IN Transfer Buffer Clear
//			// �o�b�t�@�G���v�e�B�m�F
			while( ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) ||
			       ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_DATA ) == D_EPn_IN_DATA ) ) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
			}
//			while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
//			}
		}
#endif
	} else {
		return ER_SYS;
	}

	temp2 = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
	temp = *((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20));
	if ((temp2 | temp) != 0) {
		__nop();
	}

	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 GET_CONFIGURATION����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;

	if( ( 0x0000 != pDeviceRequest->wValue  ) ||
	    ( 0x0000 != pDeviceRequest->wIndex  ) ||
	    ( 0x0001 != pDeviceRequest->wLength ) ||
	    ( USB_DIR != pDeviceRequest->bmRequestType ) ) {
		return ER_SYS;
	}

	// �R���t�B�M�����[�V�����l�́u1�v�̂݃T�|�[�g
	result = UsbFuncWriteEP0( (UCHAR*)&(pUsbfInfo->usb.curr_config), 1 );
	if( result < 0 ) {
		return result;
	}
	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16	wValue  = pDeviceRequest->wValue;
	UINT16	wIndex	= pDeviceRequest->wIndex;
	UINT16	descriptor_type = (wValue >> 8);

	ER_RET	result = ER_SYS;

	if( USB_DIR != pDeviceRequest->bmRequestType ) {
		return ER_SYS;
	}

	switch( descriptor_type ) {
		case USB_DESC_TYPE_DEVICE:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) ) {
				// Device Descriptor
				result = Get_Descriptor_Device( pDeviceRequest, pUsbfInfo );
			} else {
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_CONFIGURATION:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) ) {
				// Configuration Descriptor
				result = Get_Descriptor_Configuration( pDeviceRequest, pUsbfInfo );
			} else {
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_STRING:
			// String Descriptor
			result = Get_Descriptor_String( pDeviceRequest, pUsbfInfo );
			break;
		case USB_DESC_TYPE_DEVICE_QUALIFIER:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) ) {
				// Device Qualifier Descriptor
				result = Get_Descriptor_Qualifier( pDeviceRequest, pUsbfInfo );
			} else {
				return ER_SYS;
			}
			break;
		case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
			if( ( 0x0000 == ( wValue & 0x00ff ) ) && ( 0x0000 == wIndex ) ) {
				// Qther Speed Configuration Descriptor
				result = Get_Descriptor_OtherSpeed( pDeviceRequest, pUsbfInfo );
			} else {
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

/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR DEVICE����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void* pData;
	INT size = 0;
	ER_RET result = ER_SYS;

	// DEVICE�f�B�X�N���v�^�̕ԋp
	pData  = (void*)(pUsbfInfo->desc.pDevice);
	size   = (INT)((DEVICE_DESCRIPTOR*)pData)->bLength;
	if (size > (INT)pDeviceRequest->wLength) {
		size = (INT)pDeviceRequest->wLength;
	}
	result = UsbFuncWriteEP0( (UCHAR*)pData, size );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR CONFIGURATION����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG* pConfigDesc;
	INT size = 0;
	ER_RET result = ER_SYS;

	// CONFIGURATION�f�B�X�N���v�^�̕ԋp
	if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE ) {
		// High Speed
		pConfigDesc = pUsbfInfo->desc.pConf_hs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	} else {
		// Full Speed
		pConfigDesc = pUsbfInfo->desc.pConf_fs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}

	// Configuration Descriptor Type
	pConfigDesc->conf_desc.bDescriptorType = (UINT8)0x02;

	// ���M
	if (size > (INT)pDeviceRequest->wLength) {
		size = (INT)pDeviceRequest->wLength;
	}
	result = UsbFuncWriteEP0( (UCHAR*)pConfigDesc, size );

	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR STRING����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void* pData;
	INT size = 0;
	ER_RET result = ER_SYS;

	// ���N�G�X�g���ꂽSTRING�ԍ�
	INT string_num = (INT)(pDeviceRequest->wValue & 0xff);

	// Not Support INDEX
	if( ( string_num < 0 ) || ( STRIDX_MAX <= string_num ) ) {
		return ER_SYS;
	}

	pData = (void*)(pUsbfInfo->desc.pString[string_num]);
	size  = (INT)(((STRING_DESCRIPTOR*)pData)->bLength);

	if (size > (INT)pDeviceRequest->wLength) {
		size = (INT)pDeviceRequest->wLength;
	}
	result = UsbFuncWriteEP0( (UCHAR*)pData, size );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR DEVICE QUALIFIER����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void* pData;
	INT size = 0;
	ER_RET result = ER_SYS;

	// DEVICE QUALIFIER�f�B�X�N���v�^�̕ԋp
	pData  = (void*)(pUsbfInfo->desc.pQualifier);
	size   = (INT)(((DEVICE_DESCRIPTOR*)pData)->bLength);

	if (size > (INT)pDeviceRequest->wLength) {
		size = (INT)pDeviceRequest->wLength;
	}

	result = UsbFuncWriteEP0( pData, size );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG* pConfigDesc;
	INT size = 0;
	ER_RET result = ER_SYS;

	// CONFIGURATION�f�B�X�N���v�^�̕ԋp
	if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE ) {
		// ����HighSpeed(Full Speed�f�X�N���v�^��ԋp)
		pConfigDesc = pUsbfInfo->desc.pConf_fs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	} else {
		// ����FullSpeed�iHigh Speed�f�X�N���v�^��ԋp�j
		pConfigDesc = pUsbfInfo->desc.pConf_hs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	}

	// Other Speed Configuration Descriptor Type
	pConfigDesc->conf_desc.bDescriptorType = (UINT8)0x07;

	if (size > (INT)pDeviceRequest->wLength) {
		size = (INT)pDeviceRequest->wLength;
	}

	result = UsbFuncWriteEP0( (UCHAR*)pConfigDesc, size );

	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 GET_INTERFACE����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR* pIfDesc = NULL;
	UINT8 data = (UINT8)0;
	ER_RET result = ER_SYS;

	if( ( 0x0000      != pDeviceRequest->wValue )	||
		( 0x0001      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x81 != pDeviceRequest->bmRequestType  ) ) {
		return ER_SYS;
	}

	if( UDEV_ST_CONFIGURED == udev_get_DeviceState() ) {
		pIfDesc = info_find_InterfaceDesc( pDeviceRequest->wIndex, pUsbfInfo );
		if( pIfDesc != NULL ) {
			data = pIfDesc->bAlternateSetting;
			result = UsbFuncWriteEP0( (UCHAR*)&data, 1 );
			if( result < 0 ) {
				return ER_SYS;
			} else {
				return ER_OK;
			}
		}
	}

	return ER_SYS;
}


/**
 ******************************************************************************
  @brief EP0 GET_STATUS����
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT8	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	UINT8	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	ER_RET  result = ER_SYS;

	if( ( 0x0000 != pDeviceRequest->wValue )	||
		( 0x0002 != pDeviceRequest->wLength )	||
		( USB_DIR_IN != direction ) ) {
		return ER_SYS;
	}

	switch( recipient ) {
		case USB_TARGET_DEVICE:
			if( 0x0000 == pDeviceRequest->wIndex ) {
				result = Get_Status_Device( pDeviceRequest, pUsbfInfo );
			} else {
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


/**
 ******************************************************************************
  @brief EP0 GET_STATUS DEVICE����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16 data = 0;
	ER_RET result = ER_SYS;


	// bmAttributes��SelfPowerd(D6)�r�b�g�𑗐M�f�[�^��D0��
	data = ( (UINT16)(pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) >> 6 ) & (UINT16)BIT0;


	// �����[�g�E�F�C�N�A�b�v��D1��
	if( U2F_ENABLE == pUsbfInfo->usb.flag_RemoteWakeup ) {
		data |= (UINT8)BIT1;
	}
	// STATUS�f�[�^�̕ԋp
	result = UsbFuncWriteEP0( (UCHAR*)&data, 2 );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 GET_STATUS ENDPOINT����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Get_Status_EP( DEVICE_REQUEST* pDeviceRequest )
{
	UINT	num			= (UINT)( (pDeviceRequest->wIndex) & ~(UINT32)USB_DIR );	// EP�ԍ�
	UINT8	direction	= (UINT8)(pDeviceRequest->wIndex & USB_DIR);					// ����
	INT		status;																		// STALL���
	UINT16	data		= 0;
	ER_RET result		= ER_SYS;
	UINT16	wIndex		= pDeviceRequest->wIndex;
	UINT	endpoint	= (UINT)(wIndex & 0xFF);

	if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
		 ( 0x0000 != ( wIndex & 0x0070 ) ) ) {
		return ER_SYS;
	}

	if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) ) {	// wIndex = EP�ԍ�
		// EP0
		data = 0;
	} else {
		// Other EP
		if( UDEV_ST_CONFIGURED == udev_get_DeviceState() ) {
			// �����`�F�b�N
			if( ER_OK != Check_USB_DIR_EPn( num, direction ) ) {
				return ER_SYS;
			}

			status = EPn_get_STALL( num );	// STALL��Ԏ擾

			if( status < 0 ) {
				return ER_SYS;
			}
			if( 0 == status ) {
				data = 0;			// EPn��STALL���Ă��Ȃ�
			} else {
				data = 1;			// EPn��STALL���Ă���
			}
		} else {
			return ER_SYS;
		}
	}

	result = UsbFuncWriteEP0( (UCHAR*)&data, 2 );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 SET_ADDRESS����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_set_address( DEVICE_REQUEST* pDeviceRequest )
{
	UINT16 wValue = pDeviceRequest->wValue;

	if( ( pDeviceRequest->bmRequestType != (UINT8)0x00 )	||
		( pDeviceRequest->wIndex != 0x0000 )		||
		( pDeviceRequest->wLength != 0x0000 ) ) {
		return ER_SYS;
	}

	if( wValue != ( wValue & 0x007F ) ) {
		return ER_SYS;
	}

	ARMPF_USB_ADDRESS = wValue << 16;

	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 SET_CONFIGURATION����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16 ConfigValue = pDeviceRequest->wValue & 0x00ff;		// CONFIGURATION�ԍ�
	UINT32 tmp;

	ER_RET result = ER_SYS;

	if( ( 0x0000      != pDeviceRequest->wIndex )	||
		( 0x0000      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x00 != pDeviceRequest->bmRequestType  ) ) {
		return ER_SYS;
	}

	switch( ConfigValue ) {
		case 0:
			// Address�X�e�[�g�ɖ߂�
			pUsbfInfo->usb.curr_config = 0;
			ARMPF_USB_CONTROL &= ~D_USB_CONF;

			// --- EP �o�b�t�@�E�N���A ---
			ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							// EP1 : �o�b�t�@�E�N���A
			ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							// EP2 : �o�b�t�@�E�N���A
			// --- PID������ ---
			ARMPF_EP1_CONTROL |= D_EPn_IPIDCLR;							// EP1 : ���MPID�N���A
			ARMPF_EP2_CONTROL |= D_EPn_OPIDCLR;							// EP2 : ��MPID�N���A
			// --- EP STALL���� ---
			tmp = ARMPF_EP1_CONTROL;
			tmp &= ~(D_EPn_ISTL);									// EP1 : IN�g�[�N�� STALL����
			ARMPF_EP1_CONTROL = tmp;

			tmp = ARMPF_EP2_CONTROL;
			tmp |= D_EPn_OSTL_EN;									// EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���
			tmp &= ~(D_EPn_OSTL);									// EP2 : OUT/PING�g�[�N�� STALL����
			ARMPF_EP2_CONTROL = tmp;

			result = ER_OK;
			break;
		case 1:
			// Configured�X�e�[�g
			// Set Current Configuration Number
			if (ConfigValue > (UINT16)(pUsbfInfo->desc.pDevice->bNumConfigurations)) {
				return ER_SYS;
			}
			pUsbfInfo->usb.curr_config = ConfigValue;
			ARMPF_USB_CONTROL |= D_USB_CONF;
			result = ER_OK;
			break;
		default:
			// �T�|�[�g�O��0��ݒ�
			pUsbfInfo->usb.curr_config = 0;
			result = ER_SYS;
			break;
	}
	return result;
}


/**
 ******************************************************************************
  @brief EP0 SET_DESCRIPTOR����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest )
{
	// TODO
//	return ER_NOTSUP;
	return ER_SYS;
}



/**
 ******************************************************************************
  @brief EP0 SET_FEATURE����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT8	recipient = (UINT8)(pDeviceRequest->bmRequestType & USB_TARGET);
	UINT8	direction = (UINT8)(pDeviceRequest->bmRequestType & USB_DIR);
	UINT16	selector  = pDeviceRequest->wValue;
	UINT16	wIndex    = pDeviceRequest->wIndex;
	UINT	endpoint  = wIndex & 0xFF;

	ER_RET	result = ER_SYS;

	if( ( 0x0000 != pDeviceRequest->wLength ) ||
		( USB_DIR_OUT != direction ) ) {
		return ER_SYS;
	}

	switch( recipient ) {
		case USB_TARGET_DEVICE:
			result = Set_Feature_Device( selector, wIndex, pUsbfInfo );
			break;
		case USB_TARGET_INTERFACE:
			result = ER_SYS;
			break;
		case USB_TARGET_ENDPOINT:

			if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
				 ( 0x0000 != ( wIndex & 0x0070 ) ) ) {
				return ER_SYS;
			}

			if( USB_FEATURE_ENDPOINT_HALT == selector ) {
				if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) ) {
					// EP0
					// �o�b�t�@�G���v�e�B�m�F
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {			// �o�b�t�@����łȂ��Ƃ��C
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> �o�b�t�@����ɂ���B
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> ���ۂɋ�ɂȂ�܂ŃE�G�C�g�B
							// --- �P�[�u���������ꂽ�Ƃ��͋����I�� ---
							if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
								return ER_SYS;
							}
						}
					}
				} else {
					// Other EP
					UINT	num = endpoint & ~(UINT)USB_DIR;
					UINT8	dir = (UINT8)(endpoint & USB_DIR);

					// �����`�F�b�N
					if( ER_OK != Check_USB_DIR_EPn( num, dir ) ) {
						return ER_SYS;
					}

					if( ER_OK != Set_Feature_EP( num ) ) {
						return ER_SYS;
					}
				}
				result = ER_OK;
			} else {
				return ER_SYS;
			}
			break;
		default:
			result = ER_SYS;
			break;
	}
	return result;
}


/**
 ******************************************************************************
  @brief EP0 SET_FEATURE����( TARGET = DEVICE )
  @param  pReg      -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo     -- ���[�U���
  @param  selector  -- USB STD FEATURE Slectors
  @param  wIndex    --
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;


	switch( selector ) {
		case USB_FEATURE_DEVICE_REMOTE_WAKEUP:
			if( 0x0000 == wIndex ) {
				if( (UINT8)0 != ( (pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) & (UINT8)BIT5 ) ) {		// BIT5:Remote Wakeup bit
					// �����[�g�E�F�C�N�A�b�v�t���O�̍X�V
					pUsbfInfo->usb.flag_RemoteWakeup = U2F_ENABLE;
					result = ER_OK;
				} else {
					result = ER_SYS;
				}
			} else {
				return ER_SYS;
			}
			break;
		case USB_FEATURE_TST_MODE:
			if( wIndex <= 0x0005 ) {
				UINT32 temp		= 0;
				UINT32 tst_mode = 0;

				temp     = ARMPF_USB_CONTROL;									// CONTROL���W�X�^�̑ޔ�
				tst_mode = (UINT32)wIndex;								// wIndex��UINT32��
				tst_mode = ( tst_mode << 16 ) & D_USB_USBTESTMODE;		// TSTMODE�r�b�g�փV�t�g
				temp &= ~(UINT32)D_USB_USBTESTMODE;						// TSTMODE�̈����U�N���A
				temp |= tst_mode;										// �ݒ�l

				// ���W�X�^�֏�����
				ARMPF_USB_CONTROL = temp;
				result = ER_OK;
			} else {
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


/**
 ******************************************************************************
  @brief EP0 SET_FEATURE����( TARGET = EP )
  @param  pReg  -- ���W�X�^�x�[�X�A�h���X
  @param  num   -- EP�ԍ�
  @retval ER_OK     -- ����
  @retval ER_PARAM  -- �p�����[�^�G���[
 ******************************************************************************
*/
static ER_RET Set_Feature_EP( UINT num )
{
//	UINT32 temp;

	// Parameter�m�F
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// EP Enable�m�F
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN) {
		return ER_SYS;
	}

	// EPn Clear Feature
	if( ( udev_get_DeviceState() & UDEV_ST_CONFIGURED ) != 0 ) {
		// EP STALL�ݒ�
		UsbFuncSetStallEPn( num );

//		u2f_set_Istall_EPn( num );
//		u2f_set_Ostall_EPn( num );

//		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
//		temp |= D_EPn_OSTL_EN;										// EPn_OSTL �r�b�g�ւ̏������݂�L���ɂ���
//		temp |= (D_EPn_OSTL | D_EPn_ISTL);							// IN/OUT/PING�g�[�N�� STALL�ݒ�
//		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
	} else {
		return ER_SYS;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 SET_INTERFACE����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo           -- ���[�U���
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR* pIfDesc = NULL;

	if( ( 0x0000      != pDeviceRequest->wLength )	||
	    ( 0x0000      != pDeviceRequest->wValue  )	||			// Alternate Setting��Ή��̂��߁CwValue�ł͂���
	    ( (UINT8)0x01 != pDeviceRequest->bmRequestType  ) ) {
		return ER_SYS;
	}

	if( UDEV_ST_CONFIGURED == udev_get_DeviceState() ) {
		pIfDesc = info_find_InterfaceDesc( pDeviceRequest->wIndex, pUsbfInfo );
		if( NULL == pIfDesc ) {
			return ER_SYS;
		}
	} else {
		return ER_SYS;
	}

	//-------------------------------------------------------
	// �w���Interface�ԍ��Ńf�o�C�X�̐ݒ�����{����ꍇ
	// �������牺�ɒǉ�
	//-------------------------------------------------------
	// pIfDesc                = �C���^�[�t�F�[�X�f�X�N���v�^
	// pDeviceRequest->wIndex = �C���^�[�t�F�[�X�ԍ�
	// pDeviceRequest->wValue = ��֐ݒ�l
	//

	return ER_OK;
}
/**
 ******************************************************************************
  @brief EP0 SYNCH_FRAME����
  @param  pReg            -- ���W�X�^�x�[�X�A�h���X
  @param  pDeviceRequest  -- �f�o�C�X���N�G�X�g
  @retval ER_OK   -- ����
  @retval ���̑�  -- ���s
 ******************************************************************************
*/
static ER_RET std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest )
{
	// TODO
//	return ER_NOTSUP;
	return ER_SYS;
}


/**
 ******************************************************************************
  @brief EP������USB_DIR_OUT/IN�ϊ�����
  @param  pReg       -- ���W�X�^�x�[�X�A�h���X
  @param  num        -- EP�ԍ�
  @param  direction  -- ��r����
  @retval ER_OK   -- direction�Ɠ���
  @retval ER_SYS  -- direction�ƈႤ���G���[
 ******************************************************************************
*/
static ER_RET Check_USB_DIR_EPn( UINT num, UINT8 direction )
{
	UINT8 dir;

	// EPn�ԍ��m�F
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// �����`�F�b�N
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 ) {  // OUT
		dir = USB_DIR_OUT;
	} else {
		dir = USB_DIR_IN;
	}

	// direction�Ɣ�r
	if( dir != direction ) {
		return ER_SYS;
	}

	return ER_OK;
}



/**
 ******************************************************************************
  @brief �f�o�C�X�X�e�[�g�擾
  @param  pReg  -- ���W�X�^�x�[�X�A�h���X
  @retval UDEV_ST_IDLE        -- �A�C�h���X�e�[�g
  @retval UDEV_ST_DEFAULT     -- �f�t�H���g�X�e�[�g
  @retval UDEV_ST_ADDRESS     -- �A�h���X�X�e�[�g
  @retval UDEV_ST_CONFIGURED  -- �R���t�B�M���A�[�h�X�e�[�g
  @retval UDEV_ST_SUSPENDED   -- �T�X�y���h�X�e�[�g
 ******************************************************************************
*/
static UINT32 udev_get_DeviceState( void )
{
	UINT32  epctr	= ARMPF_EPCTR;
	UINT32  state;

	if( 0 == (epctr & D_SYS_VBUS_LEVEL ) ) {
		// Idle(Attached)�X�e�[�g
		return UDEV_ST_IDLE;
	}

	if( 0 == (epctr & D_SYS_PLL_LOCK ) ) {
		// Suspend�X�e�[�g
		return UDEV_ST_SUSPENDED;
	}

	state = ARMPF_USB_STATUS;

	if( 0 == ( state & ( D_USB_CONF_ST | D_USB_DEFAULT ) ) ) {
		return UDEV_ST_IDLE;
	}

	if( ( state & D_USB_SPND_OUT ) != 0 ) {
		// Suspend�X�e�[�g
		return UDEV_ST_SUSPENDED;
	}

	if( ( state & D_USB_CONF_ST ) != 0 ) {
		// Configured�X�e�[�g
		return UDEV_ST_CONFIGURED;
	} else {
		if( (ARMPF_USB_ADDRESS & D_USB_USB_ADDR) == D_USB_USB_ADDR ) {
			// Address state
			return UDEV_ST_ADDRESS;
		} else {
			if( ( state & D_USB_DEFAULT ) != 0 ) {
				// Default�X�e�[�g
				return UDEV_ST_DEFAULT;
			}
		}
	}

	return UDEV_ST_IDLE;
}

/**
 ******************************************************************************
  @brief info �J�����g�R���t�B�M�����[�V�����f�X�N���v�^�̎擾
  @param  pReg   -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo  -- ���[�U���
  @retval �J�����g�R���t�B�M�����[�V�����f�X�N���v�^
 ******************************************************************************
*/
static DESC_CONFIG* info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG* pConfigDesc = NULL;

	if( pUsbfInfo->usb.curr_config != 0 ) {
		if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE ) {
			pConfigDesc = pUsbfInfo->desc.pConf_hs;
		} else {
			pConfigDesc = pUsbfInfo->desc.pConf_fs;
		}
	} else {
	}
	return pConfigDesc;
}


/**
 ******************************************************************************
  @brief info �C���^�[�t�F�[�X�f�X�N���v�^�̎擾
  @param  pReg   -- ���W�X�^�x�[�X�A�h���X
  @param  pInfo  -- ���[�U���
  @param  index  -- �C���^�[�t�F�[�X�ԍ�
  @retval �C���^�[�t�F�[�X�f�X�N���v�^
 ******************************************************************************
*/
static INTERFACE_DESCRIPTOR* info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG*	pConfigDesc = NULL;
	UINT NumIf;

	if( U2F_IF_MAX <= index ) {
		return NULL;
	}

	pConfigDesc = info_get_CurrConfigDesc( pUsbfInfo );
	if( NULL == pConfigDesc ) {
		return NULL;
	}

	for( NumIf = 0; NumIf <= (UINT)(pConfigDesc->conf_desc.bNumInterfaces); NumIf++ ) {
		if( index == (UINT16)(pConfigDesc->if_desc[NumIf].bInterfaceNumber) ) {
			// found InterfaceDescriptor
			return (INTERFACE_DESCRIPTOR*)(&pConfigDesc->if_desc[NumIf]);
		}
	}

	return NULL;	// Not Found.
}


/**
 ******************************************************************************
  @brief EPn STALL��Ԏ擾�֐�
  @param  pReg  -- ���W�X�^�x�[�X�A�h���X
  @param  num   -- EP�ԍ�
  @retval 0         -- STALL���Ă��Ȃ�
  @retval 1         -- STALL���Ă���
  @retval ER_PARAM  -- �p�����[�^�G���[
 ******************************************************************************
*/
static INT EPn_get_STALL( UINT num )
{
	UINT32 status;

	// Parameter�m�F
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// EP Enable�m�F
	status = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
	if( ( status & D_EPn_EN ) == 0 ) {
		return ER_PARAM;
	}

	 // Halt�iSTALL�j�m�F
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 ) {  // OUT
		if( ( status & ( D_EPn_OSTL ) ) != 0 ) {
			return 1;
		} else {
			return 0;
		}
	} else {
		if( ( status & ( D_EPn_ISTL ) ) != 0 ) {
			return 1;
		} else {
			return 0;
		}
	}
}


