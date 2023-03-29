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

****************************************************************************/
#ifndef USB_F_H__
#define USB_F_H__

/*===========================================================================*/
/* I N C L U D E                                                             */
/*===========================================================================*/
#include "Basedef.h"
//<S150>#include "errcodes.h"	/* 暗号化フォーマット対応 <S146> */
#include "u2f_desc.h"
//<S146>#include "errcodes.h"

/*===========================================================================*/
/* D E F I N E                                                               */
/*===========================================================================*/
#define U2F_PLUG_OUT    0
#define U2F_PLUG_IN     1

/* <S150> */
/* define of error code */
#define ER_OK      ((ER_RET)0)				/**< Error code for Normal end (no error) */
#define ER_NG      ((ER_RET)-1)				/**< Error code for Abnormal end (error) */
#define ER_SYS     ((ER_RET)(2 * ER_NG))	/**< Error code for Undefined error */
#define ER_PARAM   ((ER_RET)(3 * ER_NG))	/**< Error code for Invalid parameter */
#define ER_NOTYET  ((ER_RET)(4 * ER_NG))	/**< Error code for Incomplete processing */
#define ER_NOMEM   ((ER_RET)(5 * ER_NG))	/**< Error code for Out of memory */
#define ER_BUSY    ((ER_RET)(6 * ER_NG))	/**< Error code for Busy */
#define ER_INVAL   ((ER_RET)(7 * ER_NG))	/**< Error code for Invalid state */
#define ER_TIMEOUT ((ER_RET)(8 * ER_NG))	/**< Error code for Timeout occurs */

/*==========================================================================*/
/* E X T E R N   V A R I A B L E                                            */
/*==========================================================================*/

/*===========================================================================*/
/* P R O T O T Y P E                                                         */
/*===========================================================================*/
/* UsbFuncDriver.c */
/* --- 初期化 --- */
void	UsbFuncInitializeDriver( ULONG MyAddress );	/* USB Function ドライバ初期化					*/
BOOL	UsbFuncChkAvailable( void );				/* USB使用可能判定処理							*//*<S138>*/
/* --- 割り込み --- */
void    UsbFuncIntIntu2f ( void );					/* INTU2F 割り込み処理							*/
void    UsbFuncIntIntu2fepc ( void );				/* INTU2FEPC 割り込み処理						*/
/* --- コントロール転送 --- */
void	UsbFuncTaskControl( void );					/* コントロール転送タスク処理					*//*<S1A2>*/
/* --- バルク転送タスク --- */
void	UsbFuncPollBulkOutIn( void );				/* バルク転送処理メイン(ScanBから実行可)		*/

ULONG	UsbFuncStartBkout( UCHAR *Buffer ,USHORT );	/* バルク転送(OUT)開始要求関数					*/
ULONG	UsbFuncStartBkin( UCHAR *Buffer ,USHORT );	/* バルク転送(IN) 開始要求関数					*/
USHORT	UsbFuncGetRcvSize( void );					/* 受信完了時の受信バイト数取得					*/
ULONG	UsbFuncChkRcvComp( void );					/* バルク転送(OUT)状態チェック					*/
ULONG	UsbFuncChkTrnsComp( void );					/* バルク転送(IN)状態チェック					*/
void	UsbFuncResetBuffer( void );					/* バルク転送用バッファのクリア処理				*/

ULONG	UsbFuncPortConfiguration( void );			/* <S150> */

/*--------------------------------------------------------------------------------------------------*/
/*	エンドポイント1or2状態定義																		*/
/*--------------------------------------------------------------------------------------------------*/
#define	BLKTRNS_DISABLE			0x00		/* バルク転送禁止										*/
#define	BLKTRNS_ENABLE			0x01		/* バルク転送許可										*/
#define	BLKTRNS_ACTIVE			0x02		/* バルク転送動作中										*/
#define	BLKTRNS_END				0x03		/* バルク転送完了										*/
#define	BLKTRNS_STALL			0x10		/* バルク転送にて異常発生しSTALLした					*/

/****************************************************************************************************/
/*																									*/
/*		INTUSB0Bによるコントロール転送処理															*/
/*																									*/
/****************************************************************************************************/
#define	REQ_ANA					0		/* CPUDEC割込みによるデータリクエスト解析処理				*/
#define	EP0_INTAKEN				1		/* EP0IN割込みによるＩＮトークン処理						*/
#define	EP0_INTAKEN_CMP			2		/* EP0INDT割込みによるＩＮトークン完了処理					*/

#endif // USB_F_H__
/*-------end of file----------------------------------------------------------------------------------------*/
