/****************************************************************************
Description	: MECHATROLINK System Api定義
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef SYS_API_H_

#include "Alarm.h"
#include "RegMngr.h"

/* 通信制御周期変更によるパラメータ再計算API用構造体 */
typedef struct CYC_CHG_PRM_ {
	USHORT		scanCyc;			/* 通信制御周期[0.25us] */
	USHORT		intCyc;				/* 伝送周期[0.25us] */
	USHORT		aplCyc;				/* 通信周期[x伝送周期] */
	SHORT		dlyTime;			/* To→Tiまでの時間 */
} CYC_CHG_PRM;


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
/* レジスタ操作バックグランド処理 */
VOID RegRunBackgrndProc( REG_HNDL *hReg );

INT32 RegRequestCommand( REG_HNDL *hReg, REG_CMD_TYPE type, REG_DATA *pData );
INT32 RegCheckCommandState( REG_HNDL *hReg );

/* 通信情報パラメータ更新API */
VOID RegSetComPrmMon( REG_HNDL *hReg, USHORT adr, USHORT byte );
/* 通信データモニタ更新 */
VOID RegSetComMonData( REG_HNDL *hReg, USHORT *pCmd, USHORT *pRsp );/* <S0BD> */
/* 通信ASICエラー情報取得処理 */
VOID RegSetComErrData( REG_HNDL *hReg, USHORT cnt, ULONG ests );




#define SYS_API_H_
#endif /* SYS_API_H_ */

