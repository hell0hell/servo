/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント          */
/*                                                                          */
/*            レスポンス作成処理	RSP Manager 							*/
/*																			*/
/****************************************************************************/
/* ＩＦ定義関数 :                                                           */
/*																			*/
/* VOID RspInit()				RSP Managerの初期化							*/
/* VOID RspMakeData()			メインコマンドに対するRSPデータの作成		*/
/* VOID RspMakeExData()			サブコマンドに対するRSPデータの作成			*/
/*                                                                          */
/****************************************************************************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/

/* includes */
#include "Basedef.h"
#include "RspManager.h"
#include "CmdAnalyzer.h"
#include "RspProc.h"
#include "MtApi.h"
#include "CmdTable.h"
#include "MLib.h"


/****************************************************************************/
/* 概要		：RSP Managerの初期化を行う										*/
/****************************************************************************/
/* 引数		：RSP_HNDL *hdl													*/
/* 戻り値	：なし															*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspInit(RSP_HNDL *rspHdl)
{
	/* グローバルデータをゼロクリア */
	MlibResetByteMemory( rspHdl,  sizeof(RSP_HNDL) );
	
	/* ラッチモニタ更新OFF 			*/
	rspHdl->enableLtMon = FALSE;
}

/****************************************************************************/
/* 概要		：メインコマンドに対するRSPデータの作成を行う					*/
/****************************************************************************/
/* 引数		：	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			：RSP_LINK_DATA* 	rspbuf		RSP格納用バッファポインタ		*/
/* 戻り値	：なし															*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeData(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	MT_HNDL			*pMtHdl;
	ALM_HNDL		*pAlmHdl;
	UCHAR			cmd;
	CMD_DEF_TBL		*cmd_tbl_ptr;

	/* ローカル変数の初期化 */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	pAlmHdl	= &hNetData->alm_hdl;

	/* レスポンス用メインコマンド保存バッファのコマンドコードから、
							受信コマンドのCMD定義テーブル(のポインタ)を抽出 */
	cmd = pCmdHdl->cmdInfo.rsltBuf.nop.cmd;
	rsp_buf->mt.cmd = cmd;
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* ラッチ状態チェック */
	pRspHdl->last_L_CMP = MciGetLatchStatus(pMtHdl, &pRspHdl->enableLtMon);

	/* CMD定義テーブルに登録されているコマンド毎のRSP作成処理関数を実行 */
	cmd_tbl_ptr->rspFuncPtr(hNetData, rsp_buf);

	/* アラームコード貼り付け（メインコマンドのみ） */
	rsp_buf->mt.alarm = ( (USHORT)pAlmHdl->errCode >> 4 );	/* 下位１桁切り捨て */

	/* ステータス作成・貼り付け */
	rsp_buf->mt.status = rspMakeStatus(pRspHdl, pCmdHdl, pMtHdl, pAlmHdl);

	/* respSetDone = CmdRdy */
	pRspHdl->rspSetDone = pCmdHdl->cmdInfo.cmdRdy;
}

/****************************************************************************/
/* 概要		：サブコマンドに対するRSPデータの作成を行う						*/
/****************************************************************************/
/* 引数		：	MLNET_HANDLE* hNetData			多軸用ハンドル統括構造体	*/
/* 戻り値	：なし															*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* ex_rsp_buf)
{
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	ALM_HNDL		*pAlmHdl;
	UCHAR			cmd;
	LONG			idx;
	EX_CMD_DEF_TBL	*ex_cmd_tbl_ptr;

	/* ローカル変数の初期化 */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pAlmHdl	= &hNetData->alm_hdl;

	/* レスポンス用メインコマンド保存バッファのコマンドコードから、受信コマ
	   ンドのCMD定義テーブル（のポインタ）を抽出 */
	cmd = pCmdHdl->exCmdInfo.rsltBuf.mon.cmd;
	ex_rsp_buf->mon.cmd = cmd;
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* CMD定義テーブルに登録されているコマンド毎のRSP作成処理関数を実行 */
	ex_cmd_tbl_ptr->exRspFuncPtr(hNetData,ex_rsp_buf);

	/* ステータス作成・貼り付け */
	ex_rsp_buf->mon.status = rspMakeExStatus(pCmdHdl, pAlmHdl);

	/* respSetDone = CmdRdy */
	pRspHdl->exRspSetDone =  pCmdHdl->exCmdInfo.cmdRdy;

}

