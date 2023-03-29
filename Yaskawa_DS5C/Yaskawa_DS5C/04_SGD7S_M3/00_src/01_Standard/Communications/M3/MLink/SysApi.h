/****************************************************************************
Description	: MECHATROLINK System Api定義
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef SYS_API_H_

#include "Alarm.h"
#include "RegMngr.h"


/* デバイス情報アクセス構造体 */
typedef struct DEV_DATA_ {
	ULONG		type;				/* デバイスタイプ */
	ULONG		ofst;				/* 読み出しオフセット[byte] */
	ULONG		size;				/* 読み出しサイズ[byte] */
	DWORDX		*pBuf;				/* デバイス情報格納先 */
} DEV_DATA;


/* 通信制御周期変更によるパラメータ再計算API用構造体 */
typedef struct CYC_CHG_PRM_ {
	USHORT		scanCyc;			/* 通信制御周期[0.25us] */
	USHORT		intCyc;				/* 伝送周期[0.25us] */
	USHORT		aplCyc;				/* 通信周期[x伝送周期] */
	SHORT		dlyTime;			/* To→Tiまでの時間 */
} CYC_CHG_PRM;


/* デバイス情報取得API デバイスコード */
#define	DEV_SV_MODEL			0x00			/* サーボパック型式 */
#define	DEV_SV_VER				0x02			/* サーボパックVer */
#define	DEV_ENC_MODEL			0x10			/* エンコーダ型式 */
#define	DEV_ENC_VER				0x12			/* エンコーダVer */
#define	DEV_MTR_MODEL			0x20			/* モータ型式 */
#define	DEV_COM_MODEL			0x50			/* 通信IF型式 */
#define	DEV_COM_VER				0x52			/* 通信IFVer */
#define	DEV_FBPG_MODEL			0x30			/* 外部モータ型式 */
#define	DEV_FBPG_VER			0x32			/* 外部エンコーダVer */
#define	DEV_SAFETY_MODEL		0x60			/* Safety Option型式 */
#define	DEV_SAFETY_VER			0x62			/* Safety OptionVer */
#define	DEV_FBOP_MODEL			0x70			/* Feedback Option型式 */
#define	DEV_FBOP_VER			0x72			/* Feedback OptionVer */

// ↓  M3Proto 暫定仕様
#define DEV_VENDER_ID			0x01			/* ベンダーIDコード */
#define DEV_CODE				0x02			/* デバイスコード */
#define DEV_VER					0x03			/* デバイスバージョン */
#define DEV_PROFILETYPE01		0x10			/* プロファイルタイプ１ */
#define DEV_PROFILETYPE02		0x12			/* プロファイルタイプ２ */
#define DEV_MIN_TCYC			0x16			/* 伝送周期最小値 */
#define DEV_TCYC_GRANULARITY	0x18			/* 伝送周期刻み */
#define DEV_MIN_APLCYC			0x19			/* 通信周期最小値 */
#define DEV_TRBYTE_LIST			0x1B			/* 伝送バイト数（リスト） */
#define DEV_TRBYTE				0x1C			/* 伝送バイト数(現在値) */

/* レジスタアクセスタイプ */
#define REG_NORMAL				0x0				/* 通常レジスタアクセス */
#define REG_CANCEL				0x1				/* レジスタアクセスCancel */

/* アラーム操作APIリターンコード */
#define ALM_SUCCESS				ERR_NONE		/* 正常終了 */
#define ALM_CONTINUE			ERR_WAIT_COMP	/* 継続中 */
#define ALM_CNDTN_ERR			ERR_CMD_CNDTN	/* 実行条件異常 */

/* レジスタアクセスAPIリターンコード */
#define REG_SUCCESS				ERR_NONE		/* 正常終了 */
#define REG_CONTINUE			ERR_WAIT_COMP	/* 継続中 */
#define REG_NO_ERR				ERR_PRM_NO		/* レジスタ番号異常 */
#define REG_SIZE_ERR			ERR_DATA_SZ		/* レジスタサイズ異常 */
#define REG_DATA_ERR			ERR_DATA_SET	/* レジスタデータ範囲異常 */
#define REG_CNDTN_ERR			ERR_CMD_CNDTN	/* 実行条件異常 */
#define REG_TYPE_ERR			ERR_DATA_SET	/* タイプ異常 */


/* REG Managerの初期化処理 */
VOID RegInit( REG_HNDL *hReg, MT_PRM *mtPrm, 
				COM_NET_PRM *netPrm, PRMDATA *pprm_ptr,  void *hRegMngr);
/* 初期パラメータの一括ロード */
INT32 RegLoadDefaultVal( REG_HNDL *hReg, UNI_PRM_STR *uni_prm_ptr );
/* 初期(オフライン)パラメータの一括計算 */
INT32 RegCalcDefaultVal( REG_HNDL *hReg );
/*  初期パラメータから共通パラメータの計算を実施関数	*//* <S027> */
INT32 RegSetComDefaultVal( REG_HNDL *hReg, UNI_PRM_STR *pUniPrm );

/* レジスタ操作バックグランド処理 */
VOID RegRunBackgrndProc( REG_HNDL *hReg );

INT32 RegRequestCommand( REG_HNDL *hReg, REG_CMD_TYPE type, REG_DATA *pData );
INT32 RegCheckCommandState( REG_HNDL *hReg );

/* 通信情報パラメータ更新API */
VOID RegSetComPrmMon( REG_HNDL *hReg, USHORT adr, USHORT byte );
/* 通信データモニタ更新 *//* <S0BD> */
VOID RegSetComMonData(REG_HNDL* hdl, USHORT* pCmd, USHORT* pRsp, UCHAR prfType);
/* 通信ASICエラー情報取得処理 *//* <S0BD> */
VOID RegSetComErrData(REG_HNDL* hdl, USHORT cnt, ULONG ests);




#define SYS_API_H_
#endif /* SYS_API_H_ */

