/**
 *******************************************************************************
 * @file  u2f_main.c
 * @brief program main for usb function
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
//#include "arm.h"				//@		/*<S008>*/
//#include "nbpfahb32vic128.h"	//@		/*<S008>*/
#include "ARMPF_USB_FUNC.h"
#include "usb_f.h"
#include "u2f_desc.h"

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
// --- Max Packet Size ---
#define BULK_MAXPACKET_SIZE_HS	 512
#define BULK_MAXPACKET_SIZE_FS	  64
#define INT_MAXPACKET_SIZE_HS	1024
#define INT_MAXPACKET_SIZE_FS	  64


/*===========================================================================*/
/* S T R U C T                                                               */
/*===========================================================================*/

/*============================================================================*/
/* V A R I A B L E                                                            */
/*============================================================================*/
volatile UINT32			f_connect = U2F_PLUG_OUT;
volatile UINT32			f_setup = 0;
volatile UINT32			f_ep2_out = 0;
volatile UINT32			f_ep1_dmaend = 0;
volatile UINT32			f_ep2_dmaend = 0;

/*============================================================================*/
/* S T A T I C  F U N C                                                       */
/*============================================================================*/
static void set_maxpacket ( void );
static void clr_flag ( void );

/*============================================================================*/
/* P R O G R A M                                                              */
/*============================================================================*/
/**
 *******************************************************************************
  @brief USB Function ドライバ初期化
  @param  なし
  @retval なし
 *******************************************************************************
*/
void u2f_init( void )
{
	UINT32	state;

	clr_flag();												// フラグ・クリア

//	init_u2f_control();

	//=========================================
	// EPC,PLL リセット解除
	//=========================================
	ARMPF_EPCTR &= ~(D_SYS_EPC_RST | D_SYS_PLL_RST);				// リセット解除

	//=========================================
	// System Bus-EPC Bridge 初期設定
	//=========================================
	ARMPF_SYSSCTR   = D_SYS_WAIT_MODE;							// スレーブのウエイト動作指定
	ARMPF_SYSMCTR   = 0x00000034;									// DMA転送時のアービトレーション：ラウンドロビン
	ARMPF_SYSBINTEN = D_SYS_VBUS_INT;								// VBUS割り込み許可

	//=========================================
	// EPC 初期設定
	//=========================================
	// --- PLL_LOCK待ち ---
	while ((ARMPF_EPCTR & D_SYS_PLL_LOCK) == 0) {					// PLL_LOCK待ち
		// !!!!!!!! ToDo 異常時処理 !!!!!!!!
	}

	// --- 割り込み設定 ---
	ARMPF_USB_CONTROL   = D_USB_INT_SEL | D_USB_SOF_RCV;			// INTU2FEPC割り込みをレベル割り込みに設定
															// SOF 受信エラー発生時に自動リカバリ機能を使用
	ARMPF_USB_INT_ENA   = D_USB_USB_RST_EN    |					// 割り込み許可：バス・リセット
					D_USB_SPEED_MODE_EN |					//             ：バス・スピード変更
					D_USB_EP0_EN        |					//             ：EP0 割り込み
					D_USB_EP1_EN        |					//             ：EP1 割り込み
					D_USB_EP2_EN;							//             ：EP2 割り込み

	// --- Max Packet設定 ---
	set_maxpacket();

	// --- 各EP バッファ・クリア ---
	ARMPF_EP0_CONTROL   = D_EP0_BCLR;								// EP0 : バッファ・クリア
//	ARMPF_EP1_CONTROL   = D_EPn_BCLR | D_EPn_AUTO;				// EP1 : Bulk-in,  バッファ・クリア, 自動データ送信有効
	ARMPF_EP1_CONTROL   = D_EPn_BCLR;								// EP1 : Bulk-in,  バッファ・クリア
	ARMPF_EP2_CONTROL   = D_EPn_BCLR | D_EPn_DIR0;				// EP2 : Bulk-out, バッファ・クリア

	// --- 各EP 割り込み許可 ---
	ARMPF_EP0_INT_ENA   = D_EP0_SETUP_EN;							// EP0 : 割り込み許可：SETUP_INT
	ARMPF_EP1_INT_ENA   = D_EPn_IN_END_EN;						// EP1 : 割り込み許可：EP1_IN_END_INT
	ARMPF_EP2_INT_ENA   = D_EPn_OUT_EN | D_EPn_OUT_END_EN;		// EP2 : 割り込み許可：EP2_OUT_INT, EP2_OUT_END

	// --- 各EP 有効化 ---
	ARMPF_EP1_CONTROL  |= D_EPn_EN;								// EP1 : 有効化
	ARMPF_EP2_CONTROL  |= D_EPn_EN;								// EP2 : 有効化

	//=========================================
	// プラグイン／アウト処理 (起動時状態確認)
	//=========================================
	if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL)	// VBUSレベル確認
	{
		// ====== プラグイン処理 ======
		state       =  ARMPF_USB_CONTROL;							//
		state      &= ~D_USB_CONNECTB;						// （Function PHY へのUSB 信号を有効にする）
		state      |=  D_USB_PUE2;							// （D+信号をPull-up する）
		ARMPF_USB_CONTROL =  state;								// 上記操作を同時に反映
		f_connect = U2F_PLUG_IN;
	}
	else
	{
		// ====== プラグアウト処理 ======
		state       =  ARMPF_USB_CONTROL;							//
		state      |=  D_USB_CONNECTB;						// （Function PHY へのUSB 信号を無効にする）
		state      &= ~D_USB_PUE2;							// （D+信号をPull-up しない）
		ARMPF_USB_CONTROL =  state;								// 上記操作を同時に反映
		f_connect = U2F_PLUG_OUT;
	}

	//=========================================
	// 割り込み設定
	//=========================================
//@	intc_set_handler( IDX_INTU2F, u2f_int_intu2f );			// 割り込みハンドラ登録
//@	intc_set_prl( IDX_INTU2F, PRIORITY_LVL0 );				// 割り込み優先レベル	LEVEL0
//@	intc_clear_eiint( IDX_INTU2F );							// 割り込みクリア
//@	intc_enable_eiint( IDX_INTU2F );						// 割り込み許可
//@
//@	intc_set_handler( IDX_INTU2FEPC, u2f_int_intu2fepc );	// 割り込みハンドラ登録
//@	intc_set_prl( IDX_INTU2FEPC, PRIORITY_LVL0 );			// 割り込み優先レベル	LEVEL0
//@	intc_clear_eiint( IDX_INTU2FEPC );						// 割り込みクリア
//@	intc_enable_eiint( IDX_INTU2FEPC );						// 割り込み許可
}


/**
 *******************************************************************************
  @brief INTU2F 割り込み処理
  @param  なし
  @retval なし
 *******************************************************************************
*/
void u2f_int_intu2f ( void )
{
	UINT32	state;

	//=========================================
	// プラグイン／アウト処理 (VBUS)
	//=========================================
	if ((ARMPF_SYSBINT & D_SYS_VBUS_INT) == D_SYS_VBUS_INT) {
		ARMPF_SYSBINT |= D_SYS_VBUS_INT;								// 割り込み要因クリア
		if ((ARMPF_EPCTR & D_SYS_VBUS_LEVEL) == D_SYS_VBUS_LEVEL) {	// VBUSレベル確認
			// ====== プラグイン処理 ======
			state       =  ARMPF_USB_CONTROL;							//
			state      &= ~D_USB_CONNECTB;						// （Function PHY へのUSB 信号を有効にする）
			state      |=  D_USB_PUE2;							// （D+信号をPull-up する）
			ARMPF_USB_CONTROL =  state;								// 上記操作を同時に反映
			f_connect = U2F_PLUG_IN;
		} else {												//
			// ====== プラグアウト処理 ======
			state       =  ARMPF_USB_CONTROL;							//
			state      |=  D_USB_CONNECTB;						// （Function PHY へのUSB 信号を無効にする）
			state      &= ~D_USB_PUE2;							// （D+信号をPull-up しない）
			state      &= ~D_USB_DEFAULT;						// （エンドポイント0を無効にする）
			state      &= ~D_USB_CONF;							// （エンドポイント0以外を無効にする）
			ARMPF_USB_CONTROL =  state;								// 上記操作を同時に反映
			f_connect = U2F_PLUG_OUT;
			clr_flag();											// フラグ・クリア
		}
	}

//@	intc_clear_eiint( IDX_INTU2F );								// INTU2F 要因クリア

}

/**
 *******************************************************************************
  @brief INTU2FEPC 割り込み処理
  @param  なし
  @retval なし
 *******************************************************************************
*/
void u2f_int_intu2fepc ( void )
{
	//=========================================
	// バス・リセット 処理
	//=========================================
	if ((ARMPF_USB_INT_STA & D_USB_USB_RST_INT) == D_USB_USB_RST_INT) {
		ARMPF_USB_INT_STA = ~D_USB_USB_RST_INT;						// 割り込み要因クリア
		// --- Max Packet設定 ---
		set_maxpacket();
		// --- EP バッファ・クリア ---
		ARMPF_EP0_CONTROL   |= D_EP0_BCLR;							// EP0 : バッファ・クリア
		ARMPF_EP1_CONTROL   |= D_EPn_BCLR;							// EP1 : バッファ・クリア
		ARMPF_EP2_CONTROL   |= D_EPn_BCLR;							// EP2 : バッファ・クリア
		// --- フラグ・クリア ---
		clr_flag();												// フラグ・クリア
	}
	//=========================================
	// USBバス・スピード変更 処理
	//=========================================
	if ((ARMPF_USB_INT_STA & D_USB_SPEED_MODE_INT) == D_USB_SPEED_MODE_INT) {
		ARMPF_USB_INT_STA = ~D_USB_SPEED_MODE_INT;					// 割り込み要因クリア
		// --- Max Packet設定 ---
		set_maxpacket();
	}

	//=========================================
	// EP0 処理
	//=========================================
	// ---  SETUPデータ受信 ---
	if ((ARMPF_EP0_STATUS & D_EP0_SETUP_INT) == D_EP0_SETUP_INT) {
		ARMPF_EP0_STATUS = ~D_EP0_SETUP_INT;							// 割り込み要因クリア
		f_setup = 1;
	}
	//=========================================
	// EP1 処理
	//=========================================
	// ---  Bulk-In DMA転送終了 ---
	if ((ARMPF_EP1_STATUS & D_EPn_IN_END_INT) == D_EPn_IN_END_INT) {
		ARMPF_EP1_STATUS = ~D_EPn_IN_END_INT;						// 割り込み要因クリア
		f_ep1_dmaend = 1;
	}
	//=========================================
	// EP2 処理
	//=========================================
	// ---  Bulk-Out データ受信 ---
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_INT) == D_EPn_OUT_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_INT;							// 割り込み要因クリア
		f_ep2_out++;
	}
	// ---  Bulk-Out DMA転送終了 ---
	if ((ARMPF_EP2_STATUS & D_EPn_OUT_END_INT) == D_EPn_OUT_END_INT) {
		ARMPF_EP2_STATUS = ~D_EPn_OUT_END_INT;						// 割り込み要因クリア
		f_ep2_dmaend = 1;
	}

//@	intc_clear_eiint( IDX_INTU2FEPC );							// INTU2FEPC 要因クリア
}

/**
 ******************************************************************************
  @brief EP0 送信
  @param  pBuf      -- 送信データ格納領域
  @param  size      -- 送信サイズ
  @retval 0以上の値のとき -- 送信したバイト数
  @retval 負の値のとき    -- エラー発生による強制終了
 ******************************************************************************
*/
INT u2f_write_EP0( VUCHAR* pBuf, INT size )
{
	INT	iSendLength		= size;				// 送信データ長
	INT	iRemainSize		= 0;				// 残りデータ長

	UINT32	TmpData;
	UINT32	temp;
	INT		i;

	// 残りデータ長
	iRemainSize = iSendLength;

	while (iRemainSize > 0) {
		//-----------------------------
		// バッファ・エンプティ確認
		//-----------------------------
		while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  バッファが空になるまでウエイト
			// --- ケーブルが抜かれたときは強制終了 ---
			if (f_connect == U2F_PLUG_OUT) {
				return ER_SYS;
			}
		}

		//-----------------------------
		// バッファ書き込み
		//  (4byte単位, Max:64byte)
		//-----------------------------
		for (i = 0; ((i < 64) && (iRemainSize >= 4)); i += 4, iRemainSize -= 4) {
			// --- データ生成 ---
			TmpData  = (UINT32)*pBuf++;
			TmpData |= (UINT32)*pBuf++ <<  8;
			TmpData |= (UINT32)*pBuf++ << 16;
			TmpData |= (UINT32)*pBuf++ << 24;
			// --- バッファ書き込み ---
			ARMPF_EP0_WRITE = TmpData;
		}
		// --- データ送信許可準備 ---
		temp  = ARMPF_EP0_CONTROL;											// CONTROLレジスタ リード
		temp |= D_EP0_DEND;												// 送信許可

		//-----------------------------
		// 端数バイト(3byte以下)処理
		//-----------------------------
		if ((i < 64) && (iRemainSize > 0) && (iRemainSize < 4)) {
			// --- 端数データ生成 ---
			TmpData = 0;
			for (i = 0; i < iRemainSize; i++) {
				TmpData |= (UINT32)*pBuf++ << (8*i);
			}
			// --- バッファ書き込み ---
			ARMPF_EP0_WRITE = TmpData;
			// --- データ送信許可 ---
			temp |= ( ((UINT32)iRemainSize) << 5 ) & ( D_EP0_DW );			// 端数バイトの設定
			iRemainSize = 0;
		}
		//-----------------------------
		// データ送信許可
		//-----------------------------
		ARMPF_EP0_CONTROL = temp;												// CONTROLレジスタ ライト
	}

	return size;
}


/**
 ******************************************************************************
  @brief EPn 受信 (PIO転送)
  @param  num       -- EP番号
  @param  pBuf      -- 受信データ格納領域
  @param  size      -- 受信要求サイズ
  @retval 0以上の値のとき -- 受信したバイト数
  @retval 負の値のとき    -- エラー発生による強制終了
 ******************************************************************************
*/

INT u2f_read_EPn( UINT num, VUCHAR* pBuf, INT size )
{
	INT i;
	INT cnt;

	INT	iReadLength;							// 受信データ長
	INT	iRemainSize;							// 残りデータ長
	UINT32	TmpData;

	// 残りデータ長
	iRemainSize = size;
	cnt = 0;

	while (iRemainSize > 0) {
		// --- 受信バイト数確認 ---
		iReadLength = *((VUINT32*)(EPn_LEN_DCNT_ADR + (num - 1) * 0x20)) & 0x3ff;

		// --- 受信待ちタイムアウト判断(受信待ち状態がSOF10回カウント分続いたとき) ---
		if (iReadLength == 0) {
			if ((ARMPF_USB_INT_STA & D_USB_SOF_INT) == D_USB_SOF_INT) {
				ARMPF_USB_INT_STA &= ~D_USB_SOF_INT;					// SOF受信情報クリア
				if (cnt++ >= 10) {
					break;
				}
			}
		} else {
			cnt = 0;
		}

		// --- データ受信 ---
		while (iReadLength > 0) {
			TmpData = *((VUINT32*)(EPn_READ_ADR + (num - 1) * 0x20));
			for (i = 0; ((i < 4) && (iReadLength > 0)); i++, iReadLength--, iRemainSize--) {
				*pBuf++ = (UINT8)(TmpData >> (8*i));
			}
		}
		// --- ケーブルが抜かれたときは強制終了 ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}

//	__DI();
	f_ep2_out--;
//	__EI();

	return (size - iRemainSize);
}

/**
 ******************************************************************************
  @brief EPn 受信 (DMA転送)
  @param  num       -- EP番号
  @param  pBuf      -- 受信データ格納領域
  @param  size      -- 受信サイズ
  @retval 0以上の値のとき -- 送信したバイト数
  @retval 負の値のとき    -- エラー発生による強制終了
 ******************************************************************************
*/

INT u2f_read_EPn_DMA( UINT num, VUCHAR* pBuf, INT size )
{
	UINT32	MaxPacketSize;
//	UINT32	LastPacketSize;
	UINT32	TransPacketNum;

	//=================================
	// 転送パケット数算出
	//=================================
	// --- EPnのマックス・パケット・サイズ取得 ---
	MaxPacketSize  = (*((VUINT32*)(EPn_PCKT_ADRS_ADR + (num - 1) * 0x20)) & 0x0000ffff);	// EPnのマックス・パケット・サイズ取得
	// --- 転送パケット数算出---
	TransPacketNum = size / MaxPacketSize;													// 転送パケット数算出
	// --- 最大パケット数制限 & 最終パケット転送サイズ算出 ---
	if (TransPacketNum > 256) {																// 転送パケット数が256より大きいとき(H/W制限)，
		// --- 最大パケット数オーバ ---
		TransPacketNum = 256;																//  -> 転送パケット数を256に制限
//		LastPacketSize = MaxPacketSize;														//  -> 最終パケットでの転送サイズも0に設定
	} else {																				// 転送パケット数が256以下のときで，
		if ((TransPacketNum > 0) && ((size % MaxPacketSize) == 0)) {						// フル・パケットであれば，
			// --- フル・パケット ---
//			LastPacketSize = MaxPacketSize;													//  -> 最終パケット・サイズをマックス・パケットに設定
		} else {																			// ショート・パケットであれば，
			// --- ショート・パケット ---
//			LastPacketSize = ((size % MaxPacketSize) & 0x7fc);								//  -> 最終パケット転送サイズ算出
		}
	}

	//=================================
	// Bridge側 DMA転送初期設定
	//=================================
	// --- マックス・パケット・サイズ設定 ---
	*((VUINT32*)(EPnDCR2_ADR + (num - 1) * 0x10)) = MaxPacketSize;
	// --- 転送先アドレス設定 ---
	*((VUINT32*)(EPnTADR_ADR + (num - 1) * 0x10)) = (UINT32)pBuf;

	// --- 転送方向 設定 ---
	*((VUINT32*)(EPnDCR1_ADR + (num - 1) * 0x10)) = D_SYS_EPn_DIR0;

	//=================================
	// EPC側 DMA転送初期設定
	//=================================
	*((VUINT32*)(EPn_DMA_CTRL_ADR + (num - 1) * 0x20)) = (D_EPn_STOP_MODE | 0x00000101);

	//=================================
	// データ受信待ち
	//=================================
	while (f_ep2_out == 0) {
		// --- ケーブルが抜かれたときは強制終了 ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}
//	__DI();
	f_ep2_out--;
//	__EI();

	//=================================
	// Bridge側 DMA転送開始処理
	//=================================
	// --- 転送パケット数, 転送方向, DMA転送許可 設定 ---
	//  （転送パケット数が256のときは，転送パケット数を0x00と設定）
	*((VUINT32*)(EPnDCR1_ADR + (num - 1) * 0x10)) = (((TransPacketNum & 0xff) << 16) | D_SYS_EPn_DIR0 | D_SYS_EPn_REQEN);

	//=================================
	// EPC側 DMA転送開始処理
	//=================================
	// --- 転送パケット数 設定 ---
	*((VUINT32*)(EPn_LEN_DCNT_ADR + (num - 1) * 0x20)) = (TransPacketNum << 16);
	// EPC バースト設定 & DMA転送許可
	*((VUINT32*)(EPn_DMA_CTRL_ADR + (num - 1) * 0x20)) |= (D_EPn_BURST_SET | D_EPn_DMA_EN);

	//=================================
	// DMA転送終了待ち
	//=================================
	while ((f_ep2_dmaend & 1) != 1) {
		// --- ケーブルが抜かれたときは強制終了 ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}
	f_ep2_dmaend = 0;
//	__DI();
	f_ep2_out = 0;
//	__EI();

	return size;
}


/**
 ******************************************************************************
  @brief EPn 送信 (PIO転送)
  @param  num       -- EP番号
  @param  pBuf      -- 送信データ格納領域
  @param  size      -- 送信サイズ
  @retval 0以上の値のとき -- 送信したバイト数
  @retval 負の値のとき    -- エラー発生による強制終了
 ******************************************************************************
*/

INT u2f_write_EPn( UINT num, VUCHAR* pBuf, INT size )
{
	INT	iRemainSize;						// 残りデータ長

	UINT32	MaxPacketSize;
	UINT32	TmpData;
	UINT32	temp;
	INT		i;

	// 残りデータ長
	iRemainSize = size;

	// --- 自動データ送信無効 ---
	*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) &= ~D_EPn_AUTO;

	// --- EPnのマックス・パケット・サイズ取得 ---
	MaxPacketSize  = (*((VUINT32*)(EPn_PCKT_ADRS_ADR + (num - 1) * 0x20)) & 0x0000ffff);	// EPnのマックス・パケット・サイズ取得

	while (iRemainSize > 0) {
		//-----------------------------
		// バッファ・エンプティ確認
		//-----------------------------
		while((*((VUINT32*)(EPn_STATUS_ADR + (num - 1) * 0x20)) & D_EPn_IN_EMPTY) != D_EPn_IN_EMPTY) {	//  バッファが空になるまでウエイト
			// --- ケーブルが抜かれたときは強制終了 ---
			if (f_connect == U2F_PLUG_OUT) {
				return ER_SYS;
			}
		}

		//-----------------------------
		// バッファ書き込み
		//  (4byte単位, Max:Max Packet Size)
		//-----------------------------
		for (i = 0; ((i < MaxPacketSize) && (iRemainSize >= 4)); i += 4, iRemainSize -= 4) {
			// --- データ生成 ---
			TmpData  = (UINT32)*pBuf++;
			TmpData |= (UINT32)*pBuf++ <<  8;
			TmpData |= (UINT32)*pBuf++ << 16;
			TmpData |= (UINT32)*pBuf++ << 24;
			// --- バッファ書き込み ---
			*((VUINT32*)(EPn_WRITE_ADR + (num - 1) * 0x20)) = TmpData;
		}
		// --- データ送信許可準備 ---
		temp  = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));											// CONTROLレジスタ リード
		temp |= D_EPn_DEND;												// 送信許可

		//-----------------------------
		// 端数バイト(3byte以下)処理
		//-----------------------------
		if ((i < MaxPacketSize) && (iRemainSize > 0) && (iRemainSize < 4)) {
			// --- 端数データ生成 ---
			TmpData = 0;
			for (i = 0; i < iRemainSize; i++) {
				TmpData |= (UINT32)*pBuf++ << (8*i);
			}
			// --- バッファ書き込み ---
			*((VUINT32*)(EPn_WRITE_ADR + (num - 1) * 0x20)) = TmpData;
			// --- データ送信許可 ---
			temp |= ( ((UINT32)iRemainSize) << 5 ) & ( D_EP0_DW );			// 端数バイトの設定
			iRemainSize = 0;
		}
		//-----------------------------
		// データ送信許可
		//-----------------------------
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;										// CONTROLレジスタ ライト
	}

	return size;
}

/**
 ******************************************************************************
  @brief EPn 送信 (DMA転送)
  @param  num       -- EP番号
  @param  pBuf      -- 送信データ格納領域
  @param  size      -- 送信サイズ
  @retval 0以上の値のとき -- 送信したバイト数
  @retval 負の値のとき    -- エラー発生による強制終了
 ******************************************************************************
*/

INT u2f_write_EPn_DMA( UINT num, VUCHAR* pBuf, INT size )
{
	UINT32	MaxPacketSize, LastPacketSize;
	UINT32	TransPacketNum;

	// --- 自動データ送信有効 ---
	*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_AUTO;

	//=================================
	// 転送パケット数算出
	//=================================
	// --- EPnのマックス・パケット・サイズ取得 ---
	MaxPacketSize  = (*((VUINT32*)(EPn_PCKT_ADRS_ADR + (num - 1) * 0x20)) & 0x0000ffff);	// EPnのマックス・パケット・サイズ取得
	// --- 転送パケット数算出---
	TransPacketNum = size / MaxPacketSize;													// 転送パケット数算出
	// --- 最大パケット数制限 & 最終パケット転送サイズ算出 ---
	if (TransPacketNum > 256)
	{																// 転送パケット数が256より大きいとき(H/W制限)，
		// --- 最大パケット数オーバ ---
		TransPacketNum = 256;																//  -> 転送パケット数を256に制限
		LastPacketSize = MaxPacketSize;														//  -> 最終パケットでの転送サイズも0に設定
	}
	else
	{																				// 転送パケット数が256以下のときで，
		if ((TransPacketNum > 0) && ((size % MaxPacketSize) == 0))
		{// フル・パケットであれば，
			// --- フル・パケット ---
			LastPacketSize = MaxPacketSize;													//  -> 最終パケット・サイズをマックス・パケットに設定
		}
		else
		{// ショート・パケットであれば，
			// --- ショート・パケット ---
			LastPacketSize = ((size % MaxPacketSize) & 0x7fc);								//  -> 最終パケット転送サイズ算出
		}
	}

	//=================================
	// Bridge側 DMA転送初期設定
	//=================================
	// --- マックス・パケット & 最終パケット転送サイズ設定 ---
	*((VUINT32*)(EPnDCR2_ADR + (num - 1) * 0x10)) = ((LastPacketSize << 16) | MaxPacketSize);
	// --- 転送先アドレス設定 ---
	*((VUINT32*)(EPnTADR_ADR + (num - 1) * 0x10)) = (UINT32)pBuf;

	// --- 転送パケット数, 転送方向, DMA転送許可 設定 ---
	//  （転送パケット数が256のときは，転送パケット数を0x00と設定）
	*((VUINT32*)(EPnDCR1_ADR + (num - 1) * 0x10)) = (((TransPacketNum & 0xff) << 16) | D_SYS_EPn_REQEN);

	//=================================
	// EPC側 DMA転送初期設定（転送開始）
	//=================================
	// --- 転送パケット数 設定 ---
	*((VUINT32*)(EPn_LEN_DCNT_ADR + (num - 1) * 0x20)) = (TransPacketNum << 16);
	// EPC バースト設定 & DMA転送許可
	*((VUINT32*)(EPn_DMA_CTRL_ADR + (num - 1) * 0x20)) = (D_EPn_BURST_SET | D_EPn_DMA_EN | 0x00000101);

	//=================================
	// DMA転送終了待ち
	//=================================
	while ((f_ep1_dmaend & 1) != 1)
	{
		// --- ケーブルが抜かれたときは強制終了 ---
		if
		(f_connect == U2F_PLUG_OUT)
		{
			return ER_SYS;
		}
	}
	f_ep1_dmaend = 0;

	return size;
}

/**
 ******************************************************************************
  @brief EP0 ストール応答
  @param  なし
  @retval なし
 ******************************************************************************
*/

void u2f_set_stall_EP0( void )
{
	ARMPF_EP0_CONTROL |= D_EP0_STL;
}

/**
 ******************************************************************************
  @brief EPn ストール応答
  @param  num       -- EP番号
  @retval なし
 ******************************************************************************
*/

void u2f_set_stall_EPn( UINT num )
{
	// --- IN/OUT 判断 ---
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		// --- BULK IN ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_ISTL;
	} else {
		// --- BULK OUT ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= (D_EPn_OSTL_EN | D_EPn_OSTL);
	}
}

/**
 ******************************************************************************
  @brief EPn ストール解除
  @param  num       -- EP番号
  @retval なし
 ******************************************************************************
*/

void u2f_clear_stall_EPn( UINT num )
{
	UINT32	temp;

	// --- IN/OUT 判断 ---
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		// --- BULK IN ---
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp &= ~(D_EPn_ISTL);											// EPn : INトークン STALL解除
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;

	} else {
		// --- BULK OUT ---
		temp = *((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20));
		temp |= D_EPn_OSTL_EN;											// EPn_OSTL ビットへの書き込みを有効にする
		temp &= ~(D_EPn_OSTL);											// EPn : OUT/PINGトークン STALL解除
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) = temp;
	}
}

/**
 ******************************************************************************
  @brief EPn バッファ・クリア
  @param  num       -- EP番号
  @retval なし
 ******************************************************************************
*/

void u2f_clear_buf_EPn( UINT num )
{
	*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_BCLR;			// EPn : バッファ・クリア
}

/**
 ******************************************************************************
  @brief EPn PIDクリア
  @param  num       -- EP番号
  @retval なし
 ******************************************************************************
*/

void u2f_clear_PID_EPn( UINT num )
{
	// --- IN/OUT 判断 ---
	if( (*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) & D_EPn_DIR0) != D_EPn_DIR0 ) {
		// --- BULK IN ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_IPIDCLR;		// EPn : 送信PIDクリア
	} else {
		// --- BULK OUT ---
		*((VUINT32*)(EPn_CONTROL_ADR + (num - 1) * 0x20)) |= D_EPn_OPIDCLR;		// EPn : 受信PIDクリア
	}
}

/**
 ******************************************************************************
  @brief EP0 NAK応答
  @param  なし
  @retval なし
 ******************************************************************************
*/

void u2f_set_nak_EP0( void )
{
	UINT32 temp;

	temp  = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp |= (UINT32)(D_EP0_INAK | D_EP0_ONAK);
	ARMPF_EP0_CONTROL = temp;
}

/**
 ******************************************************************************
  @brief EP0 NAK解除
  @param  なし
  @retval なし
 ******************************************************************************
*/

void u2f_clear_nak_EP0( void )
{
	UINT32 temp;

	temp  = ARMPF_EP0_CONTROL;
	temp |= D_EP0_INAK_EN;
	temp &= ~(UINT32)(D_EP0_INAK | D_EP0_ONAK);
	ARMPF_EP0_CONTROL = temp;
}


/**
 ******************************************************************************
  @brief EP0 NULL送信
  @param  なし
  @retval なし
 ******************************************************************************
*/

ER_RET u2f_send_null_EP0( void )
{
	//-----------------------------
	// バッファ・エンプティ確認
	//-----------------------------
	while((ARMPF_EP0_STATUS & D_EP0_IN_EMPTY) != D_EP0_IN_EMPTY) {	//  バッファが空になるまでウエイト
		// --- ケーブルが抜かれたときは強制終了 ---
		if (f_connect == U2F_PLUG_OUT) {
			return ER_SYS;
		}
	}

	//-----------------------------
	// データ送信許可
	//-----------------------------
	ARMPF_EP0_CONTROL |= D_EP0_DEND;									// 送信許可

	return ER_OK;
}

/**
 ******************************************************************************
  @brief USB接続状態確認
  @param  なし
  @retval U2F_PLUG_OUT -- USB Hostと接続されていないとき
  @retval U2F_PLUG_IN  -- USB Hostと接続されているとき
 ******************************************************************************
*/

UINT u2f_check_plug_inout( void )
{
	return f_connect;
}

/**
 ******************************************************************************
  @brief Max Packet Size 設定
  @param  なし
  @retval なし
 ******************************************************************************
*/

static void set_maxpacket ( void )
{
	UINT32	bulk_size, interrupt_size;

	//---------------------------------
	// USBバス・スピード確認
	//---------------------------------
	if ( (ARMPF_USB_STATUS & D_USB_SPEED_MODE) == D_USB_SPEED_MODE ) {
		// High Speed
		bulk_size      = BULK_MAXPACKET_SIZE_HS;
		interrupt_size = INT_MAXPACKET_SIZE_HS;
	} else {
		// Full Speed
		bulk_size      = BULK_MAXPACKET_SIZE_FS;
		interrupt_size = INT_MAXPACKET_SIZE_FS;
	}

	//---------------------------------
	// Max Packet Size 設定
	//---------------------------------
														// EPn : BaseAdr  MaxPkt(HS/FS 本ドライバでは規格上の最大値を設定)
														// EP0 : 0x0000        64 byte x 2面
	ARMPF_EP1_PCKT_ADRS = 0x00200000 | bulk_size;				// EP1 : 0x0020    512/64 byte x 2面	--   Bulk    転送 --
	ARMPF_EP2_PCKT_ADRS = 0x01200000 | bulk_size;				// EP2 : 0x0120    512/64 byte x 2面	--   Bulk    転送 --
	ARMPF_EP3_PCKT_ADRS = 0x02200000 | interrupt_size;		// EP3 : 0x0220   1024/64 byte x 1面	-- Interrupt 転送 --
}

/**
 ******************************************************************************
  @brief フラグ・クリア
  @param  なし
  @retval なし
 ******************************************************************************
*/
static void clr_flag ( void )
{
	f_setup = 0;
	f_ep2_out = 0;
	f_ep1_dmaend = 0;
	f_ep2_dmaend = 0;
}
