/****************************************************************************
Description	: USB function Server for MEMBUS protocol
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  :
	-- APIs --
	MbusUsb_Initialize()	: Initialize the USB function Server
	MbusUsb_Server()		: Runtime process of the MEMOBUS USB Server
	MbusUsb_PollOutIn()		: ポーリングによるUSB送受信監視制御処理

	-- Locals --
	MbusUsb_ChkMyMsg()		: 自局アドレスのチェック

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.05.10		created

****************************************************************************/
#include "Basedef.h"
#include "arm.h"
#include "UsbFunctionIF.h"
#include "KLib.h"
#include "MLib.h"
#include "usb_f.h"

//debug -->
#if 1
const UCHAR	MsgReadSvSerial[] = {0x01,0x40,0x00,0x03,0xc8,0x08,0x00,0x08,0x3a,0x67};
#define	LEN_MSGREADSVSERIAL		sizeof(MsgReadSvSerial)
const UCHAR	ResReadSvSerial[] = {0x01,0x40,0x00,0x03,0x00,0x08,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x30,0x00,0x7f,0xfa};
#define	LEN_RESREADSVSERIAL		sizeof(ResReadSvSerial)
const UCHAR	MsgReadMaxRegSizeA[] = {0x01,0x40,0x00,0x41,0x00,0x03,0x10,0x10};
#define	LEN_MSGREADMAXREGSIZEA	sizeof(MsgReadMaxRegSizeA)
const UCHAR	ResReadMaxRegSizeA[] = {0x01,0x40,0x00,0x41,0x00,0x03,0x00,0x1b,0x4c,0x07};
#define	LEN_RESREADMAXREGSIZEA	sizeof(ResReadMaxRegSizeA)
const UCHAR	MsgReadMaxRegSizeB[] = {0x01,0x40,0x00,0x41,0x00,0x0d,0x91,0xd4};
#define	LEN_MSGREADMAXREGSIZEB	sizeof(MsgReadMaxRegSizeB)
const UCHAR	ResReadMaxRegSizeB[] = {0x01,0x40,0x00,0x41,0x00,0x0d,0x00,0x1b,0x2d,0xc4};
#define	LEN_RESREADMAXREGSIZEB	sizeof(ResReadMaxRegSizeB)
const UCHAR	MsgReadMaxRegSizeC[] = {0x01,0x40,0x00,0x41,0x00,0x10,0x51,0xdd};
#define	LEN_MSGREADMAXREGSIZEC	sizeof(MsgReadMaxRegSizeC)
const UCHAR	ResReadMaxRegSizeC[] = {0x01,0x40,0x00,0x41,0x00,0x10,0x00,0x15,0x3c,0x06};
#define	LEN_RESREADMAXREGSIZEC	sizeof(ResReadMaxRegSizeC)
const UCHAR	MsgReadMaxRegSizeD[] = {0x01,0x40,0x00,0x41,0x00,0x0e,0xd1,0xd5};
#define	LEN_MSGREADMAXREGSIZED	sizeof(MsgReadMaxRegSizeD)
const UCHAR	ResReadMaxRegSizeD[] = {0x01,0x40,0x00,0x41,0x00,0x0e,0x00,0x17,0xdd,0xc1};
#define	LEN_RESREADMAXREGSIZED	sizeof(ResReadMaxRegSizeD)
#endif
//debug <--


/****************************************************************************/
/*		PROTOTYPE															*/
/****************************************************************************/
/* Locals */
static ULONG	MbusUsb_ChkMyMsg(MEMOBUS_IF *MbusIf);		/* 自局アドレスチェック		*/

/***************************************************************************
Description: Initialize the USB Memobus Server
----------------------------------------------------------------------------
Parameters:
	USB_MBUS_SERVER		*pUsbMbusServer	:Pointer to this module handle
	MEMOBUS_ADDR_MAP	*pAxisCfg		:Axis Address configuration
	ULONG				MyAddress		:my address
Return:
	0					: Success.
****************************************************************************/
INT32 MbusUsb_Initialize(USB_MBUS_SERVER *pUsbMbusServer,
						 MEMOBUS_ADDR_MAP *pAxisCfg, ULONG MyAddress)
{
/* FIXME:MyAddressの扱い等が不明確 */
#if 1	/* debug */
	MyAddress = 1;
#endif
	/* ワーク領域初期化 */
	MlibResetByteMemory(pUsbMbusServer, sizeof(USB_MBUS_SERVER));

	/* MEMOBUS I/F 初期化 */
	MbusFuncInitialize(&(pUsbMbusServer->MbusIf), pAxisCfg, MyAddress);

	/* USBファンクションデバイス初期化 */
	UsbFuncInitializeDriver(MyAddress);

	/* 送受信監視シーケンス状態設定 */
	pUsbMbusServer->State = USB_STATE_INITIAL1;

	return 0;
}


/***************************************************************************
Description: Runtime process of the USB Memobus Server
----------------------------------------------------------------------------
Parameters:
	USB_MBUS_SERVER		*pUsbMbusServer	:Pointer to this module handle
Return: none
****************************************************************************/
void MbusUsb_Server(USB_MBUS_SERVER *pUsbMbusServer)
{
	/* 送信開始待ち */
	if(pUsbMbusServer->State == USB_STATE_WAIT_START_IN1)
	{
/* FIXME:MbusFuncMsgProcedure()利用時の仕様不明確分あり */
#if 0	/* 暫定対応(ここの処理を正しく作成する必要がある) */
		/* メッセージ解析＆実行処理 */
		MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
#else
		switch( pUsbMbusServer->MbusIf.RcvbLength )
		{
		case LEN_MSGREADSVSERIAL:		// レジスタ内容読み出し
			if( (pUsbMbusServer->MbusIf.RxBuf[1] == 0x40) && (pUsbMbusServer->MbusIf.RxBuf[3] == 0x03) )
			{
				// サーボパックシリアルNo.を返すが、返信データに問題があるようでMBCommanderでエラーとなっている様子
				memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadSvSerial, LEN_RESREADSVSERIAL);
				pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADSVSERIAL;
			}
			else
			{
				/* メッセージ解析＆実行処理 */
				MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
			}
			break;
		case LEN_MSGREADMAXREGSIZEA:	// 8バイト
			// 最大サイズ読み出し(16ビットアドレス用)
			if( (pUsbMbusServer->MbusIf.RxBuf[1] == 0x40) && (pUsbMbusServer->MbusIf.RxBuf[3] == 0x41) )
			{
				if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x03 )		// レジスタ内容読み出し
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeA, LEN_RESREADMAXREGSIZEA);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZEA;
				}
				else if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x0d )	// レジスタの非連続読み出し
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeB, LEN_RESREADMAXREGSIZEB);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZEB;
				}
				else if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x10 )	// 複数レジスタへの書き込み
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeC, LEN_RESREADMAXREGSIZEC);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZEC;
				}
				else if( pUsbMbusServer->MbusIf.RxBuf[5] == 0x0e )	// レジスタの非連続読み出し
				{
					memcpy(pUsbMbusServer->MbusIf.TxBuf, ResReadMaxRegSizeD, LEN_RESREADMAXREGSIZED);
					pUsbMbusServer->MbusIf.TrnbLength = LEN_RESREADMAXREGSIZED;
				}
			}
//tanaka			else if( pUsbMbusServer->MbusIf.RxBuf[1] == 0x08 )		// ループバック
//tanaka			{
//tanaka				memcpy(pUsbMbusServer->MbusIf.TxBuf, pUsbMbusServer->MbusIf.RxBuf, LEN_MSGREADMAXREGSIZEA);
//tanaka				pUsbMbusServer->MbusIf.TrnbLength = LEN_MSGREADMAXREGSIZEA;
//tanaka			}
			else
			{
				/* メッセージ解析＆実行処理 */
				MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
			}
			break;
		default:
			/* メッセージ解析＆実行処理 */
			MbusFuncMsgProcedure( &(pUsbMbusServer->MbusIf) );
			break;
		}
#endif
//		MlibSetCRC16(pUsbMbusServer->MbusIf.TxBuf, pUsbMbusServer->MbusIf.TrnbLength);
//		pUsbMbusServer->MbusIf.TrnbLength += 2;

		/* バルク転送フラグの制御が必要！！(BlkInSkip, BlkInStart) */
		/* 現時点ではIN開始を設定 */
		pUsbMbusServer->f.BlkInStart = TRUE;

		/* Roundからの送信開始状態に移行 */
		pUsbMbusServer->State = USB_STATE_WAIT_START_IN2;
	}
}



/***************************************************************************
Description: Runtime process of the USB Memobus Server
----------------------------------------------------------------------------
Parameters:
	USB_MBUS_SERVER		*pUsbMbusServer	:Pointer to this module handle
Return: none
****************************************************************************/
void MbusUsb_PollOutIn(USB_MBUS_SERVER *pUsbMbusServer)
{
	ULONG	rc = 0;

	UsbFuncPollBulkOutIn();	// 仮置き

	/*----------------------------------------------------------------------------------------------*/
	/*		ＵＳＢによるMEMOBUS通信　送受信監視シーケンス											*/
	/*----------------------------------------------------------------------------------------------*/
	switch( pUsbMbusServer->State ){
		/*------------------------------------------------------------------------------------------*/
		/*	初期ステップ																			*/
		/*		・USBドライバの送受信FIFOを全クリアする												*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_INITIAL1:
			UsbFuncResetBuffer( );
//*			if( ReadFuncLmtflg() == FALSE )	/* 機能制限フラグが立っていなければ状態を進める			*/
			{								/* フラグが立っているときはFIFOのリセットのみ			*/
				pUsbMbusServer->State = USB_STATE_INITIAL2;
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	初期ステップ２																			*/
		/*		・USBドライバの状態をチェックし通信可能状態に入ったことを確認して、Endpoint2のOUT	*/
		/*		  データを受けるように要求する。要求後、OUTパケット受信完了処理で完了を待つ。		*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_INITIAL2:
			/*--------------------------------------------------------------------------------------*/
			/*	USBドライバに対してUSBポートが通信可能か問合せを行い、通信可能ならMEMOBUS通信の		*/
			/*	準備を行う。																		*/
			/*--------------------------------------------------------------------------------------*/
			rc = UsbFuncPortConfiguration();
			if( rc == TRUE )
			{
				/*----------------------------------------------------------------------------------*/
				/*	USBドライバに対してデータを受信するように要求									*/
				/*		ドライバが正常に受信開始処理を実行(rc==TRUE)したらOUT受信完了待ちへ遷移		*/
				/*----------------------------------------------------------------------------------*/
				rc = UsbFuncStartBkout( pUsbMbusServer->MbusIf.RxBuf, ((USHORT)MSG_MAXBUFSZB -1 ));
				if( rc == TRUE )
				{
					pUsbMbusServer->State = USB_STATE_WAIT_OUT_END;		/* OUT受信完了待ち処理に遷移	*/
				}
			}
			pUsbMbusServer->f.BlkInSkip = FALSE;						/* 無応答指令フラグクリア		*/
			pUsbMbusServer->f.BlkInStart = FALSE;						/* 応答指令フラグをクリア		*/
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	MEMOBUSデータ受信完了待ち処理															*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_WAIT_OUT_END:
			rc = UsbFuncPortConfiguration();							/* USB有効?				*/
			if( rc == FALSE )
			{
				pUsbMbusServer->State = USB_STATE_INITIAL1;				/* 初期ステップへ遷移	*/
			}
			else
			{
				rc = UsbFuncChkRcvComp();					/* 全データ受信完了問合せ			*/
				if( rc == BLKTRNS_END )						/* 全データ受信完了？				*/
				{
					/*------------------------------------------------------------------------------*/
					/*	受信完了時の処理															*/
					/*		メッセージのチェック													*/
					/*			・自局宛チェック( ｱﾄﾞﾚｽ"0"の場合ﾌﾞﾛｰﾄﾞｷｬｽﾄであるため自局と判定)		*/
					/*		※CRCチェックはアプリケーションで行う。									*/
					/*------------------------------------------------------------------------------*/
					pUsbMbusServer->MbusIf.RcvbLength = UsbFuncGetRcvSize();/* 受信完了バイト数を得る			*/
					rc = MbusUsb_ChkMyMsg(&(pUsbMbusServer->MbusIf));
					if( rc == TRUE )
					{
						pUsbMbusServer->State = USB_STATE_WAIT_START_IN1;	/* Adr=0 or 自局宛:送信開始待ちへ	*/
					}
					else
					{
						pUsbMbusServer->State = USB_STATE_INITIAL1;			/* 自局宛以外は受信開始へ戻る		*/
					}
				}
			}
			break;
		/*------------------------------------------------------------------------------------------*/
		/*	送信開始待ち処理																		*/
		/*		アプリケーションからの送信開始要求を待つ											*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_WAIT_START_IN1:
			break;
		case USB_STATE_WAIT_START_IN2:
			rc = UsbFuncPortConfiguration();							/* USB有効?				*/
			if( rc == FALSE )
			{
				pUsbMbusServer->State = USB_STATE_INITIAL1;				/* 初期ステップへ遷移	*/
			}
			/*----------------------------------------------------------------------------------*/
			/*	無応答の場合は送信ステップを行わず受信待ちへ戻る								*/
			/*----------------------------------------------------------------------------------*/
			else
			{
				if( pUsbMbusServer->f.BlkInSkip == TRUE )
				{
					pUsbMbusServer->State = USB_STATE_INITIAL1;			/* 初期ステップへ遷移	*/
				}
				else if( pUsbMbusServer->f.BlkInStart == TRUE )
				{
					/*--------------------------------------------------------------------------*/
					/*	USBドライバへ送信バッファを送信データ長だけ送信するように要求			*/
					/*--------------------------------------------------------------------------*/
					rc = UsbFuncStartBkin( pUsbMbusServer->MbusIf.TxBuf, pUsbMbusServer->MbusIf.TrnbLength );
					if( rc == TRUE )
					{
						pUsbMbusServer->State = USB_STATE_WAIT_IN_END;	/* 全データ送信完了待ちへ遷移	*/
					}
					else
					{
						pUsbMbusServer->State = USB_STATE_INITIAL1;		/* 初期ステップへ遷移			*/
					}
				}
				else
				{
					/* 処理なし */
				}
			}
			break;

		/*------------------------------------------------------------------------------------------*/
		/*	全データ送信完了待ち処理																*/
		/*		USBドライバ送信完了を待つ															*/
		/*------------------------------------------------------------------------------------------*/
		case USB_STATE_WAIT_IN_END:
			rc = UsbFuncPortConfiguration();							/* USB有効?					*/
			if( rc == FALSE )
			{
				pUsbMbusServer->State = USB_STATE_INITIAL1;				/* 初期ステップへ遷移		*/
			}
			else
			{
				rc = UsbFuncChkTrnsComp();								/* 全データ送信完了問合せ	*/
				/*------------------------------------------------------------------------------*/
				/*	送信完了チェック															*/
				/*------------------------------------------------------------------------------*/
				if( rc == BLKTRNS_END )
				{
					pUsbMbusServer->State = USB_STATE_INITIAL1;			/* 初期ステップへ遷移		*/
				}
			}
			break;

		/*------------------------------------------------------------------------------------------*/
		/*	その他（ここには来ない）※保険															*/
		/*------------------------------------------------------------------------------------------*/
		default :
			break;

	}

/*--------------------------------------------------------------------------------------------------*/
/*	送受信シーケンスおわり																			*/
/*--------------------------------------------------------------------------------------------------*/

	return;

}

/**************************** Start of Local Functions **********************************************/
/****************************************************************************************************/
/*																									*/
/*		自局宛メッセージのチェック																	*/
/*																									*/
/****************************************************************************************************/
/***************************************************************************
Description: 自局アドレスチェック
----------------------------------------------------------------------------
Parameters:
	MEMOBUS_IF			*MbusIfPtr	:Pointer to this module handle
Return: none
****************************************************************************/
static ULONG	MbusUsb_ChkMyMsg(MEMOBUS_IF *MbusIfPtr)
{
	/*----------------------------------------------------------------------------------------------*/
	/*		今回受信したメッセージのアドレス部=="0" or ==自局アドレスの場合TRUEを返す				*/
	/*----------------------------------------------------------------------------------------------*/
	if(( MbusIfPtr->RxBuf[0] == 0x00 )||( MbusIfPtr->RxBuf[0] == MbusIfPtr->NodeAddress ))
	{
		return( TRUE );					/* 自局宛であることを返す								*/
	}
	else
	{
		return( FALSE );				/* 自局宛ではないことを返す								*/
	}

}
/**************************** End of Local Functions ************************************************/


/* End of File */
