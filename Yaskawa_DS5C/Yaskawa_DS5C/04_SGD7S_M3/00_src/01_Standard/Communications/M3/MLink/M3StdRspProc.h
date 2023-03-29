/****************************************************************************
Description	: Definition for MECHATROLINK Response Maker
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef RSP_FUNCDEF_H_

//#include "M3Handler.h"
#include "CmdHandler.h"
#include "ComAlmMngr.h"

/****************************************************************************
 Response functions
****************************************************************************/
/* ステータス作成関数定義 */
/* ステータス処理 */
USHORT stdRspMakeCMD_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, USHORT sts );
/* サブコマンドステータス処理 */
ULONG stdRspMakeSUB_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, ULONG sts );

/* レスポンス作成関数定義 */
/* 未定義/MEM_WRコマンドレスポンス処理 */
VOID stdRsp_SYS_CMD( void *hNetData, void *rsp_buf );
/* ID_RDコマンドレスポンス処理 (03H) */
VOID stdRsp_ID_RD( void *hNetData, void *rsp_buf );
/* NOP,SYNC_SET,DISCONNECTコマンドレスポンス処理 (00/0D/0FH) */
VOID stdRsp_NOP( void *hNetData, void *rsp_buf );
/* CONFIGコマンドレスポンス処理 (04H) */
VOID stdRsp_CONFIG( void *hNetData, void *rsp_buf );
/* ALM_RDコマンドレスポンス処理 (06H) */
VOID stdRsp_ALM_RD( void *hNetData, void *rsp_buf );
/* ALM_CLRコマンドレスポンス処理 (06H) */
VOID stdRsp_ALM_CLR( void *hNetData, void *rsp_buf );
/* CONNECTコマンドレスポンス処理 (0EH) */
VOID stdRsp_CONNECT( void *hNetData, void *rsp_buf );
/* POS_SETコマンドレスポンス処理 (20H) */
VOID stdRsp_POS_SET( void *hNetData, void *rsp_buf );
/* MEM_RDコマンドレスポンス処理 (1DH) */
VOID stdRsp_MEM_RD( void *hNetData, void *rsp_buf );
/* SVPRM_RDコマンドレスポンス処理 (40H) */
VOID stdRsp_SVPRM_RD( void *hNetData, void *rsp_buf );
/* SVPRM_WRコマンドレスポンス処理 (41H) */
VOID stdRsp_SVPRM( void *hNetData, void *rsp_buf );
/* モニタ有りコマンドレスポンス処理 (**H) */
VOID stdRsp_MON_CMD( void *hNetData, void *rsp_buf );

/* サブコマンド  レスポンス作成関数定義 */
/* 未定義/ALM_RD/MER_WR/SVPRM_WRコマンドレスポンス処理 */
VOID stdExRsp_SYS_CMD( void *hNetData, void *rsp_buf );
/* NOPコマンドレスポンス処理 (00H) */
VOID stdExRsp_NOP( void *hNetData, void *rsp_buf );
/* ALM_RDコマンドレスポンス処理 (06H) */
VOID stdExRsp_ALM_RD( void *hNetData, void *rsp_buf );
/* ALM_CLRコマンドレスポンス処理 (06H) */
VOID stdExRsp_ALM_CLR( void *hNetData, void *rsp_buf );
/* SMON コマンドレスポンス処理 */
VOID stdExRsp_MON_CMD( void *hNetData, void *rsp_buf );
/* SVPRM_RDコマンドレスポンス処理 */
VOID stdExRsp_SVPRM_RD( void *hNetData, void *rsp_buf );
/* MEM_RDコマンドレスポンス処理 */
VOID stdExRsp_MEM_RD( void *hNetData, void *rsp_buf );



#define RSP_FUNCDEF_H_
#endif /* RSP_FUNCDEF_H_ */


