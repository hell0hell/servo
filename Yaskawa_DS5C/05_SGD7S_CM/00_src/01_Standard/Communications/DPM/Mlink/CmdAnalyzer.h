/****************************************************************************/
/*																			*/
/*																			*/
/*		CMD Analyzer ヘッダファイル											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		CMD_ANALYZER_H_
#define		CMD_ANALYZER_H_

/* includes */
#include "M2Handler.h"

/* defines */
/* 関数戻り値定義 */
#define CMD_FUNC_NORMAL_END	ERR_NONE
#define CMD_FUNC_CONTINUE	ERR_WAIT_COMP
#define CMD_CMDLOCK_ERR		ERR_SUBCMD
#define CMD_PHASE_ERR		ERR_CMD_CNDTN
#define CMD_TIME_OUT_ERR	ERR_CMD_FAIL

#define DISCONNECT_CMD			0x0F			/* DISCONNECT コマンドコード */

/* CMD AnaLyzerの初期化処理 */
VOID CmdInit(CMD_HNDL* pCmdHdl);
/* メインコマンド解析用バッファの更新とコマンド解析実行要求処理 */
INT32 CmdUpdateData(CMD_HNDL* pCmdHdl, COM_HNDL* pComHdl, BOOL rsp_set);
/* サブコマンド解析用バッファの更新とコマンド解析実行要求処理 */
INT32 CmdUpdateExData(CMD_HNDL* pCmdHdl, COM_HNDL* pComHdl, BOOL ex_rsp_set);
/* メインコマンドの解析・実行処理 */
VOID CmdAnalyze( MLNET_HANDLE *hNetData );
/* サブコマンドの解析・実行処理 */
VOID CmdAnalyzeEx( MLNET_HANDLE *hNetData );

#endif /* CMD_ANALYZER_H_ */
