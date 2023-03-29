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
// リクエスト・タイプ別処理
//-------------------------------------
static	ER_RET	control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 No Data処理 */
static	ER_RET	control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 Control Read処理 */
static	ER_RET	control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 Control Write処理 */

//-------------------------------------
// スタンダード・リクエスト処理
//-------------------------------------
static	ER_RET	decode_request( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );					/* EP0 リクエストデコード処理 */

static	ER_RET	bmRequestType_standard(  DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 スタンダードリクエスト処理 */
// --- CLEAR_FEATURE     ( 1) [No data] ---
static	ER_RET	std_req_clear_feature( DEVICE_REQUEST* pDeviceRequest );								/* EP0 CLEAR_FEATURE処理 */
static	ER_RET	Clear_Feature_EP( UINT num );															/* EP0 CLEAR_FEATURE処理( TARGET = EP ) */
// --- GET_CONFIGURATION ( 8) [Read]    ---
static	ER_RET	std_req_get_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_CONFIGURATION処理 */
// --- GET_DESCRIPTOR    ( 6) [Read]    ---
static	ER_RET	std_req_get_descriptor( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR処理 */
static	ER_RET	Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR DEVICE処理 */
static	ER_RET	Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );	/* EP0 GET_DESCRIPTOR CONFIGURATION処理 */
static	ER_RET	Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_DESCRIPTOR STRING処理 */
static	ER_RET	Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR DEVICE QUALIFIER処理 */
static	ER_RET	Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION処理 */
// --- GET_INTERFACE     (10) [Read]    ---
static	ER_RET	std_req_get_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 GET_INTERFACE処理 */
// --- GET_STATUS        ( 0) [Read]    ---
static	ER_RET	std_req_get_status( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS処理 */
static	ER_RET	Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 GET_STATUS DEVICE処理 */
static	ER_RET	Get_Status_EP( DEVICE_REQUEST* pDeviceRequest );										/* EP0 GET_STATUS ENDPOINT処理 */
// --- SET_ADDRESS       ( 5) [No data] ---
static	ER_RET	std_req_set_address( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SET_ADDRESS処理 */
// --- SET_CONFIGURATION ( 9) [No data] ---
static	ER_RET	std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );		/* EP0 SET_CONFIGURATION処理 */
// --- SET_DESCRIPTOR    ( 7) [Write]   ---
static	ER_RET	std_req_set_descriptor( DEVICE_REQUEST* pDeviceRequest );								/* EP0 SET_DESCRIPTOR処理 */
// --- SET_FEATURE       ( 3) [No data] ---
static	ER_RET	std_req_set_feature( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );				/* EP0 SET_FEATURE処理 */
static	ER_RET	Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo );									/* EP0 SET_FEATURE処理( TARGET = DEVICE ) */
static	ER_RET	Set_Feature_EP( UINT num );																/* EP0 SET_FEATURE処理( TARGET = EP ) */
// --- SET_INTERFACE     (11) [No data] ---
static	ER_RET	std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo );			/* EP0 SET_INTERFACE処理 */
// --- SYNC_FRAME        (12) [No data] ---
static	ER_RET	std_req_synch_frame( DEVICE_REQUEST* pDeviceRequest );									/* EP0 SYNCH_FRAME処理 */

static	ER_RET	Check_USB_DIR_EPn( UINT num, UINT8 direction );											/* EP方向のUSB_DIR_OUT/IN変換処理 */
static	UINT32	udev_get_DeviceState( void );															/* デバイスステート取得 */
static	DESC_CONFIG*			info_get_CurrConfigDesc( U2F_INFO* pUsbfInfo );							/* info カレントコンフィギュレーションデスクリプタの取得 */
static	INTERFACE_DESCRIPTOR*	info_find_InterfaceDesc( UINT16 index, U2F_INFO* pUsbfInfo );			/* info インターフェースデスクリプタの取得 */
static	INT						EPn_get_STALL( UINT num );												/* EPn STALL状態取得関数 */

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/

/**
 *******************************************************************************
  @brief コントロール転送タスク処理
  @param  なし
  @retval なし
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
	// EP0 処理 (SETUPデータ受信)
	//=========================================
	if (pUsbIf->UsbfFlag.UsbSetupReceive == 1) {
		pUsbIf->UsbfFlag.UsbSetupReceive = 0;
		// ------ デバイス・リクエスト リード ------
		pDeviceRequest->bmRequestType = (UINT8) ((ARMPF_SETUP_DATA0 >>  0) & 0xff  );
		pDeviceRequest->bRequest      = (UINT8) ((ARMPF_SETUP_DATA0 >>  8) & 0xff  );
		pDeviceRequest->wValue        = (UINT16)((ARMPF_SETUP_DATA0 >> 16) & 0xffff);
		pDeviceRequest->wIndex        = (UINT16)((ARMPF_SETUP_DATA1 >>  0) & 0xffff);
		pDeviceRequest->wLength       = (UINT16)((ARMPF_SETUP_DATA1 >> 16) & 0xffff);

		// ------ リクエスト・タイプ判別 ------
		if( pDeviceRequest->wLength == 0x0000 ) {								// データ・ステージのバイト数が0のとき
			result = control_nodata( pDeviceRequest, pUsbfInfo );				//  -> STATUS処理へ
		} else {																// データ・ステージのバイト数が0ではなく，
			if( (pDeviceRequest->bmRequestType & USB_DIR_IN) == USB_DIR_IN ) {	// データ転送方向が Host=>Dev のとき，
				result = control_read( pDeviceRequest, pUsbfInfo );				//  -> DATA IN & STATUS処理へ
			} else {															// Dev=>Host のとき，
				result = control_write( pDeviceRequest, pUsbfInfo );			//  -> DATA OUT & STATUS処理へ
			}
		}
		// ------ エラー処理 ---
		if (result != ER_OK) {
			// ToDo
		}
	}
}

/****************************************************************************************************/
/*																									*/
/*		ＵＳＢポート状態通知サービス関数															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		アプリケーション層通信が可能であるかを返信します。											*/
/*			・USBでは、SET_CONFIGURATION Val = 1 をHostがDeviceに指令するとEndpoint0以外の(本ドラ	*/
/*			  イバでは1と2)が使用可能になります。アプリケーション通信はEP1とEP2を使用します。		*/
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
  @brief EP0 No Data処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET control_nodata( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32 data;
	UINT32 temp;

	ER_RET	 result	= ER_SYS;

	//=========================================
	// デコード処理
	//=========================================
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	//=========================================
	// ステータス・ステージ移行待ち
	//=========================================
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT) {	// ステータス・ステージに移行するまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;									// 要因クリア

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {				// バッファが空でないとき，
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;										//  -> バッファを空にする。
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {		//  -> 実際に空になるまでウエイト。
			// --- ケーブルが抜かれたときは強制終了 ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// INトークン強制NAK応答を解除
	//=========================================
	data = ARMPF_EP0_CONTROL;
	data |= D_EP0_INAK_EN;
	data &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = data;

	//=========================================
	// ステータスステージ
	//=========================================
	// NULLデータ送信
	// ------ バッファ・エンプティ確認 ------
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  バッファが空になるまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}

	// ------ データ送信許可 ------
	temp  = ARMPF_EP0_CONTROL;											// CONTROLレジスタ リード
	temp |= D_EP0_DEND;												// 送信許可
	ARMPF_EP0_CONTROL = temp;												// CONTROLレジスタ ライト

	//----------------------------------------
	// ステータスステージ終了確認
	//----------------------------------------
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT) {	// ステータス・ステージが終了するまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;									// 要因クリア

	return ER_OK;
}


/**
 *******************************************************************************
  @brief EP0 Control Read処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 *******************************************************************************
*/
static ER_RET control_read( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT32 data;

	ER_RET result = ER_SYS;

	//=========================================
	// バッファ・エンプティ確認
	//=========================================
	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {				// バッファが空でないとき，
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;										//  -> バッファを空にする。
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {		//  -> 実際に空になるまでウエイト。
			// --- ケーブルが抜かれたときは強制終了 ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// INトークン強制NAK応答を解除
	//=========================================
	data = ARMPF_EP0_CONTROL;
	data |= D_EP0_INAK_EN;
	data &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = data;

	//=========================================
	// デコード処理
	//=========================================
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	//=========================================
	// ステータス・ステージ移行待ち
	//=========================================
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT) {	// ステータス・ステージに移行するまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;									// 要因クリア

	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {				// バッファが空でないとき，
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;										//  -> バッファを空にする。
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {		//  -> 実際に空になるまでウエイト。
			// --- ケーブルが抜かれたときは強制終了 ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// OUTトークン強制NAK応答を解除
	//=========================================
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;

	//----------------------------------------
	// ステータスステージ
	//----------------------------------------
	// NULLデータ受信
	while((ARMPF_EP0_STATUS & D_EP0_OUT_NULL_INT) != D_EP0_OUT_NULL_INT) {	// NULLデータを受信するまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_OUT_NULL_INT;									// 要因クリア

	//----------------------------------------
	// ステータスステージ終了確認
	//----------------------------------------
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT) {		// ステータス・ステージが終了するまでウエイト
		// --- ケーブルが抜かれたか、SETUPトークンを受信したときは強制終了 ---
		if( (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) || (UsbFuncCheckSetupReceive() != 0) ){
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;									// 要因クリア

	return ER_OK;
}


/**
 *******************************************************************************
  @brief EP0 Control Write処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 *******************************************************************************
*/
static ER_RET control_write( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;
	UINT32 data;

	//=========================================
	// バッファ・エンプティ確認
	//=========================================
	if((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {			// バッファが空でないとき，
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> バッファを空にする。
		while((ARMPF_EP0_STATUS & D_EP0_OUT_EMPTY) != D_EP0_OUT_EMPTY) {	//  -> 実際に空になるまでウエイト。
			// --- ケーブルが抜かれたときは強制終了 ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// OUTトークン強制NAK応答を解除
	//=========================================
	ARMPF_EP0_CONTROL &= ~(UINT32)D_EP0_ONAK;


	//=========================================
	// デコード処理
	//=========================================
	result = decode_request( pDeviceRequest, pUsbfInfo );
	if( ER_OK != result ) {
		UsbFuncSetStallEP0();
		return result;
	}

	//=========================================
	// ステータス・ステージ移行待ち
	//=========================================
	while((ARMPF_EP0_STATUS & D_EP0_STG_START_INT) != D_EP0_STG_START_INT) {	// ステータス・ステージに移行するまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_START_INT;									// 要因クリア

	if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {			// バッファが空でないとき，
		ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> バッファを空にする。
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> 実際に空になるまでウエイト。
			// --- ケーブルが抜かれたときは強制終了 ---
			if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
				return result;
			}
		}
	}

	//=========================================
	// INトークン強制NAK応答を解除
	//=========================================
	data = ARMPF_EP0_CONTROL;
	data |= D_EP0_INAK_EN;
	data &= ~(UINT32)D_EP0_INAK;
	ARMPF_EP0_CONTROL = data;

	//----------------------------------------
	// ステータスステージ
	//----------------------------------------
	ARMPF_EP0_CONTROL |= D_EP0_DEND;

	//----------------------------------------
	// ステータスステージ終了確認
	//----------------------------------------
	while((ARMPF_EP0_STATUS & D_EP0_STG_END_INT) != D_EP0_STG_END_INT) {	// ステータス・ステージが終了するまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
			return result;
		}
	}
	ARMPF_EP0_STATUS = ~D_EP0_STG_END_INT;									// 要因クリア

	return ER_OK;
}




//=============================================================================
// USB STANDARD REQUEST
//=============================================================================

/**
 ******************************************************************************
  @brief EP0 リクエストデコード処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET decode_request(DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;

	// コントロール転送タイプ判別
	switch( pDeviceRequest->bmRequestType & USB_REQUEST )
	{
		//-----------------------------
		// 標準デバイス・リクエスト
		//-----------------------------
		case USB_REQUEST_STANDARD:
			result = bmRequestType_standard( pDeviceRequest, pUsbfInfo );
			break;

		//-----------------------------
		// クラス・リクエスト
		//-----------------------------
		case USB_REQUEST_CLASS:
			if( pUsbfInfo->cb_class_req ) {
				result = pUsbfInfo->cb_class_req( pDeviceRequest );
			} else {
				result = ER_SYS;
			}
			break;

		//-----------------------------
		// ベンダ固有リクエスト
		//-----------------------------
		case USB_REQUEST_VENDOR:
			if( pUsbfInfo->cb_vendor_req ) {
				result = pUsbfInfo->cb_vendor_req( pDeviceRequest );
			} else {
				result = ER_SYS;
			}
			break;

		//-----------------------------
		// その他
		//-----------------------------
		default:
			result = ER_SYS;
			break;
	}
	return result;
}

/**
 ******************************************************************************
  @brief EP0 スタンダードリクエスト処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP0 CLEAR_FEATURE処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
	// パラメータ・チェック(定数箇所)
	//---------------------------------
	if( ( 0x0000 != pDeviceRequest->wLength ) ||
		( USB_DIR_OUT != direction ) ) {
		return ER_SYS;
	}

	switch( recipient ) {
		case USB_TARGET_DEVICE:
			if( 0x0000 == wIndex ) {
				if( USB_FEATURE_DEVICE_REMOTE_WAKEUP == selector ) {
					// リモートウェイクアップ禁止設定
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
					// バッファエンプティ確認
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {			// バッファが空でないとき，
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> バッファを空にする。
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> 実際に空になるまでウエイト。
							// --- ケーブルが抜かれたときは強制終了 ---
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
  @brief EP0 CLEAR_FEATURE処理( TARGET = EP )
  @param  num    -- EP番号
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Clear_Feature_EP( UINT num )
{
	UINT32 temp;
	UINT32 temp2;

	// Parameter確認
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// EP Enable確認
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN) {
		return ER_SYS;
	}

	// EPn Clear Feature
	if( (udev_get_DeviceState() & UDEV_ST_CONFIGURED) != 0 ) {
		// Buffer Clear
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_BCLR;													// 送受信バッファ・クリア
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {  // IN
			// PID初期化
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_IPIDCLR;											// 送信PIDクリア
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			// バッファエンプティ確認
			while( ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) ||
			       ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_DATA ) == D_EPn_IN_DATA ) ) {	//  -> 実際に空になるまでウエイト。
				// --- ケーブルが抜かれたときは強制終了 ---
				if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
					return ER_SYS;
				}
			}
			// EP STALL解除
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp &= ~(D_EPn_ISTL);											// INトークン STALL解除
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		} else {
			// PID初期化
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OPIDCLR;											// 受信PIDクリア
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
			// EP STALL解除
			temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
			temp |= D_EPn_OSTL_EN;											// EPn_OSTL ビットへの書き込みを有効にする
			temp &= ~(D_EPn_OSTL);											// OUT/PINGトークン STALL解除
			*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
		}

#if 0
		// EP STALL解除
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_OSTL_EN;										// EPn_OSTL ビットへの書き込みを有効にする
		temp &= ~(D_EPn_OSTL | D_EPn_ISTL);							// IN/OUT/PINGトークン STALL解除
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		// PID初期化 & Buffer Clear
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= (D_EPn_OPIDCLR | D_EPn_IPIDCLR | D_EPn_BCLR);		// 送受信PIDクリア，バッファ・クリア
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

		if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {  // IN
//			// Check IN Transfer Buffer Clear
//			// バッファエンプティ確認
			while( ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) ||
			       ((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_DATA ) == D_EPn_IN_DATA ) ) {	//  -> 実際に空になるまでウエイト。
			}
//			while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> 実際に空になるまでウエイト。
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
  @brief EP0 GET_CONFIGURATION処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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

	// コンフィギュレーション値は「1」のみサポート
	result = UsbFuncWriteEP0( (UCHAR*)&(pUsbfInfo->usb.curr_config), 1 );
	if( result < 0 ) {
		return result;
	}
	return ER_OK;
}

/**
 ******************************************************************************
  @brief EP0 GET_DESCRIPTOR処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP0 GET_DESCRIPTOR DEVICE処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Descriptor_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void* pData;
	INT size = 0;
	ER_RET result = ER_SYS;

	// DEVICEディスクリプタの返却
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
  @brief EP0 GET_DESCRIPTOR CONFIGURATION処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Descriptor_Configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG* pConfigDesc;
	INT size = 0;
	ER_RET result = ER_SYS;

	// CONFIGURATIONディスクリプタの返却
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

	// 送信
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
  @brief EP0 GET_DESCRIPTOR STRING処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Descriptor_String( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void* pData;
	INT size = 0;
	ER_RET result = ER_SYS;

	// リクエストされたSTRING番号
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
  @brief EP0 GET_DESCRIPTOR DEVICE QUALIFIER処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Descriptor_Qualifier( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	void* pData;
	INT size = 0;
	ER_RET result = ER_SYS;

	// DEVICE QUALIFIERディスクリプタの返却
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
  @brief EP0 GET_DESCRIPTOR OTHER_SPEED_CONFIGURATION処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Descriptor_OtherSpeed( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	DESC_CONFIG* pConfigDesc;
	INT size = 0;
	ER_RET result = ER_SYS;

	// CONFIGURATIONディスクリプタの返却
	if( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE ) {
		// 現在HighSpeed(Full Speedデスクリプタを返却)
		pConfigDesc = pUsbfInfo->desc.pConf_fs;
		size = (INT)((pConfigDesc->conf_desc.wTotalLength[1] << 16) + pConfigDesc->conf_desc.wTotalLength[0]);
	} else {
		// 現在FullSpeed（High Speedデスクリプタを返却）
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
  @brief EP0 GET_INTERFACE処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP0 GET_STATUS処理
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP0 GET_STATUS DEVICE処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Status_Device( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16 data = 0;
	ER_RET result = ER_SYS;


	// bmAttributesのSelfPowerd(D6)ビットを送信データのD0へ
	data = ( (UINT16)(pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) >> 6 ) & (UINT16)BIT0;


	// リモートウェイクアップはD1へ
	if( U2F_ENABLE == pUsbfInfo->usb.flag_RemoteWakeup ) {
		data |= (UINT8)BIT1;
	}
	// STATUSデータの返却
	result = UsbFuncWriteEP0( (UCHAR*)&data, 2 );
	if( result < 0 ) {
		return result;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 GET_STATUS ENDPOINT処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Get_Status_EP( DEVICE_REQUEST* pDeviceRequest )
{
	UINT	num			= (UINT)( (pDeviceRequest->wIndex) & ~(UINT32)USB_DIR );	// EP番号
	UINT8	direction	= (UINT8)(pDeviceRequest->wIndex & USB_DIR);					// 方向
	INT		status;																		// STALL状態
	UINT16	data		= 0;
	ER_RET result		= ER_SYS;
	UINT16	wIndex		= pDeviceRequest->wIndex;
	UINT	endpoint	= (UINT)(wIndex & 0xFF);

	if ( ( 0x0000 != ( wIndex & 0xFF00 ) )   ||
		 ( 0x0000 != ( wIndex & 0x0070 ) ) ) {
		return ER_SYS;
	}

	if( ( 0x0000 == endpoint ) || ( 0x0080 == endpoint ) ) {	// wIndex = EP番号
		// EP0
		data = 0;
	} else {
		// Other EP
		if( UDEV_ST_CONFIGURED == udev_get_DeviceState() ) {
			// 方向チェック
			if( ER_OK != Check_USB_DIR_EPn( num, direction ) ) {
				return ER_SYS;
			}

			status = EPn_get_STALL( num );	// STALL状態取得

			if( status < 0 ) {
				return ER_SYS;
			}
			if( 0 == status ) {
				data = 0;			// EPnはSTALLしていない
			} else {
				data = 1;			// EPnはSTALLしている
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
  @brief EP0 SET_ADDRESS処理
  @param  pReg            -- レジスタベースアドレス
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP0 SET_CONFIGURATION処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET std_req_set_configuration( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	UINT16 ConfigValue = pDeviceRequest->wValue & 0x00ff;		// CONFIGURATION番号
	UINT32 tmp;

	ER_RET result = ER_SYS;

	if( ( 0x0000      != pDeviceRequest->wIndex )	||
		( 0x0000      != pDeviceRequest->wLength )	||
	    ( (UINT8)0x00 != pDeviceRequest->bmRequestType  ) ) {
		return ER_SYS;
	}

	switch( ConfigValue ) {
		case 0:
			// Addressステートに戻る
			pUsbfInfo->usb.curr_config = 0;
			ARMPF_USB_CONTROL &= ~D_USB_CONF;

			// --- EP バッファ・クリア ---
			ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							// EP1 : バッファ・クリア
			ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							// EP2 : バッファ・クリア
			// --- PID初期化 ---
			ARMPF_EP1_CONTROL |= D_EPn_IPIDCLR;							// EP1 : 送信PIDクリア
			ARMPF_EP2_CONTROL |= D_EPn_OPIDCLR;							// EP2 : 受信PIDクリア
			// --- EP STALL解除 ---
			tmp = ARMPF_EP1_CONTROL;
			tmp &= ~(D_EPn_ISTL);									// EP1 : INトークン STALL解除
			ARMPF_EP1_CONTROL = tmp;

			tmp = ARMPF_EP2_CONTROL;
			tmp |= D_EPn_OSTL_EN;									// EPn_OSTL ビットへの書き込みを有効にする
			tmp &= ~(D_EPn_OSTL);									// EP2 : OUT/PINGトークン STALL解除
			ARMPF_EP2_CONTROL = tmp;

			result = ER_OK;
			break;
		case 1:
			// Configuredステート
			// Set Current Configuration Number
			if (ConfigValue > (UINT16)(pUsbfInfo->desc.pDevice->bNumConfigurations)) {
				return ER_SYS;
			}
			pUsbfInfo->usb.curr_config = ConfigValue;
			ARMPF_USB_CONTROL |= D_USB_CONF;
			result = ER_OK;
			break;
		default:
			// サポート外は0を設定
			pUsbfInfo->usb.curr_config = 0;
			result = ER_SYS;
			break;
	}
	return result;
}


/**
 ******************************************************************************
  @brief EP0 SET_DESCRIPTOR処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP0 SET_FEATURE処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
					// バッファエンプティ確認
					if((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {			// バッファが空でないとき，
						ARMPF_EP0_CONTROL |= D_EP0_BCLR;									//  -> バッファを空にする。
						while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  -> 実際に空になるまでウエイト。
							// --- ケーブルが抜かれたときは強制終了 ---
							if (UsbFuncCheckPlugInout() == U2F_PLUG_OUT) {
								return ER_SYS;
							}
						}
					}
				} else {
					// Other EP
					UINT	num = endpoint & ~(UINT)USB_DIR;
					UINT8	dir = (UINT8)(endpoint & USB_DIR);

					// 方向チェック
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
  @brief EP0 SET_FEATURE処理( TARGET = DEVICE )
  @param  pReg      -- レジスタベースアドレス
  @param  pInfo     -- ユーザ情報
  @param  selector  -- USB STD FEATURE Slectors
  @param  wIndex    --
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET Set_Feature_Device( UINT16 selector, UINT16 wIndex, U2F_INFO* pUsbfInfo )
{
	ER_RET result = ER_SYS;


	switch( selector ) {
		case USB_FEATURE_DEVICE_REMOTE_WAKEUP:
			if( 0x0000 == wIndex ) {
				if( (UINT8)0 != ( (pUsbfInfo->desc.pConf_hs->conf_desc.bmAttributes) & (UINT8)BIT5 ) ) {		// BIT5:Remote Wakeup bit
					// リモートウェイクアップフラグの更新
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

				temp     = ARMPF_USB_CONTROL;									// CONTROLレジスタの退避
				tst_mode = (UINT32)wIndex;								// wIndexをUINT32へ
				tst_mode = ( tst_mode << 16 ) & D_USB_USBTESTMODE;		// TSTMODEビットへシフト
				temp &= ~(UINT32)D_USB_USBTESTMODE;						// TSTMODE領域を一旦クリア
				temp |= tst_mode;										// 設定値

				// レジスタへ書込み
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
  @brief EP0 SET_FEATURE処理( TARGET = EP )
  @param  pReg  -- レジスタベースアドレス
  @param  num   -- EP番号
  @retval ER_OK     -- 成功
  @retval ER_PARAM  -- パラメータエラー
 ******************************************************************************
*/
static ER_RET Set_Feature_EP( UINT num )
{
//	UINT32 temp;

	// Parameter確認
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// EP Enable確認
	if ( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_EN) != D_EPn_EN) {
		return ER_SYS;
	}

	// EPn Clear Feature
	if( ( udev_get_DeviceState() & UDEV_ST_CONFIGURED ) != 0 ) {
		// EP STALL設定
		UsbFuncSetStallEPn( num );

//		u2f_set_Istall_EPn( num );
//		u2f_set_Ostall_EPn( num );

//		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
//		temp |= D_EPn_OSTL_EN;										// EPn_OSTL ビットへの書き込みを有効にする
//		temp |= (D_EPn_OSTL | D_EPn_ISTL);							// IN/OUT/PINGトークン STALL設定
//		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
	} else {
		return ER_SYS;
	}

	return ER_OK;
}


/**
 ******************************************************************************
  @brief EP0 SET_INTERFACE処理
  @param  pReg            -- レジスタベースアドレス
  @param  pInfo           -- ユーザ情報
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
 ******************************************************************************
*/
static ER_RET std_req_set_interface( DEVICE_REQUEST* pDeviceRequest, U2F_INFO* pUsbfInfo )
{
	INTERFACE_DESCRIPTOR* pIfDesc = NULL;

	if( ( 0x0000      != pDeviceRequest->wLength )	||
	    ( 0x0000      != pDeviceRequest->wValue  )	||			// Alternate Setting非対応のため，wValueではじく
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
	// 指定のInterface番号でデバイスの設定を実施する場合
	// ここから下に追加
	//-------------------------------------------------------
	// pIfDesc                = インターフェースデスクリプタ
	// pDeviceRequest->wIndex = インターフェース番号
	// pDeviceRequest->wValue = 代替設定値
	//

	return ER_OK;
}
/**
 ******************************************************************************
  @brief EP0 SYNCH_FRAME処理
  @param  pReg            -- レジスタベースアドレス
  @param  pDeviceRequest  -- デバイスリクエスト
  @retval ER_OK   -- 成功
  @retval その他  -- 失敗
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
  @brief EP方向のUSB_DIR_OUT/IN変換処理
  @param  pReg       -- レジスタベースアドレス
  @param  num        -- EP番号
  @param  direction  -- 比較方向
  @retval ER_OK   -- directionと同じ
  @retval ER_SYS  -- directionと違うかエラー
 ******************************************************************************
*/
static ER_RET Check_USB_DIR_EPn( UINT num, UINT8 direction )
{
	UINT8 dir;

	// EPn番号確認
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// 方向チェック
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) == D_EPn_DIR0 ) {  // OUT
		dir = USB_DIR_OUT;
	} else {
		dir = USB_DIR_IN;
	}

	// directionと比較
	if( dir != direction ) {
		return ER_SYS;
	}

	return ER_OK;
}



/**
 ******************************************************************************
  @brief デバイスステート取得
  @param  pReg  -- レジスタベースアドレス
  @retval UDEV_ST_IDLE        -- アイドルステート
  @retval UDEV_ST_DEFAULT     -- デフォルトステート
  @retval UDEV_ST_ADDRESS     -- アドレスステート
  @retval UDEV_ST_CONFIGURED  -- コンフィギュアードステート
  @retval UDEV_ST_SUSPENDED   -- サスペンドステート
 ******************************************************************************
*/
static UINT32 udev_get_DeviceState( void )
{
	UINT32  epctr	= ARMPF_EPCTR;
	UINT32  state;

	if( 0 == (epctr & D_SYS_VBUS_LEVEL ) ) {
		// Idle(Attached)ステート
		return UDEV_ST_IDLE;
	}

	if( 0 == (epctr & D_SYS_PLL_LOCK ) ) {
		// Suspendステート
		return UDEV_ST_SUSPENDED;
	}

	state = ARMPF_USB_STATUS;

	if( 0 == ( state & ( D_USB_CONF_ST | D_USB_DEFAULT ) ) ) {
		return UDEV_ST_IDLE;
	}

	if( ( state & D_USB_SPND_OUT ) != 0 ) {
		// Suspendステート
		return UDEV_ST_SUSPENDED;
	}

	if( ( state & D_USB_CONF_ST ) != 0 ) {
		// Configuredステート
		return UDEV_ST_CONFIGURED;
	} else {
		if( (ARMPF_USB_ADDRESS & D_USB_USB_ADDR) == D_USB_USB_ADDR ) {
			// Address state
			return UDEV_ST_ADDRESS;
		} else {
			if( ( state & D_USB_DEFAULT ) != 0 ) {
				// Defaultステート
				return UDEV_ST_DEFAULT;
			}
		}
	}

	return UDEV_ST_IDLE;
}

/**
 ******************************************************************************
  @brief info カレントコンフィギュレーションデスクリプタの取得
  @param  pReg   -- レジスタベースアドレス
  @param  pInfo  -- ユーザ情報
  @retval カレントコンフィギュレーションデスクリプタ
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
  @brief info インターフェースデスクリプタの取得
  @param  pReg   -- レジスタベースアドレス
  @param  pInfo  -- ユーザ情報
  @param  index  -- インターフェース番号
  @retval インターフェースデスクリプタ
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
  @brief EPn STALL状態取得関数
  @param  pReg  -- レジスタベースアドレス
  @param  num   -- EP番号
  @retval 0         -- STALLしていない
  @retval 1         -- STALLしている
  @retval ER_PARAM  -- パラメータエラー
 ******************************************************************************
*/
static INT EPn_get_STALL( UINT num )
{
	UINT32 status;

	// Parameter確認
	if(( num < 1 ) || ( num > 9)) {
		return ER_PARAM;
	}

	// EP Enable確認
	status = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
	if( ( status & D_EPn_EN ) == 0 ) {
		return ER_PARAM;
	}

	 // Halt（STALL）確認
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


