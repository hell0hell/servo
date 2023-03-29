/****************************************************************************
Description	: MECHATROLINK Command Analyzer
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef CMD_ANALYZER_H_

#include "M3Handler.h"

/****************************************************************************
 API's 
****************************************************************************/
/* 関数戻り値定義 */
#define CMD_FUNC_NORMAL_END		0x0
#define CMD_FUNC_CONTINUE		0x1
#define CMD_CMDLOCK_ERR			ERR_SUBCMD
#define CMD_PHASE_ERR_M2		ERR_CMD_CNDTN
#define CMD_PHASE_ERR_M3		ERR_COM_PHASE
#define CMD_TIME_OUT_ERR		ERR_CMD_FAIL
#define CMD_PROFILE_UNKNOWN		ERR_DATA_SET
#define CMD_PROFILE_DETECT		1

#define DISCONNECT_CMD			0x0F			/* DISCONNECT コマンドコード */


/* CMD AnaLyzerの初期化処理 */
VOID CmdInit( CMD_HNDL *hdl );
/* メインコマンド解析用バッファの更新とコマンド解析実行要求処理 */
INT32 CmdUpdateData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL rsp_set );
/* サブコマンド解析用バッファの更新とコマンド解析実行要求処理 */
INT32 CmdUpdateExData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL ex_rsp_set );
/* メインコマンドの解析・実行処理 */
VOID CmdAnalyze( MLNET_HANDLE *hNetData );
/* サブコマンドの解析・実行処理 */
VOID CmdAnalyzeEx( MLNET_HANDLE *hNetData );
/* プロファイルタイプチェック、ベースポインタの設定処理 */
INT32 CmdSetProfileType( MLNET_HANDLE *hNetData, LONG tdata_size );
/* コネクション切断処理 */
INT32 CmdRunDisConnect( MLNET_HANDLE *hNetData );
/* 通信リセット要求処理 */
LONG CmdReqComReset( MLNET_HANDLE *hNetData );



#define CMD_ANALYZER_H_
#endif /* CMD_ANALYZER_H_ */


