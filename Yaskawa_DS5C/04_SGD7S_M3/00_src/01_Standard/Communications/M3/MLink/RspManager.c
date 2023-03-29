/****************************************************************************
Description	: MECHATROLINK Response Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "RspManager.h"
#include "CmdAnalyzer.h"
#include "M3StdRspProc.h"
#include "MtApi.h"
#include "CmdTable.h"
#include "MLib.h"





/****************************************************************************/
/* 概要		:	RSP Managerの初期化を行う									*/
/****************************************************************************/
/* 引数		:	RSP_HNDL* hdl												*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspInit( RSP_HNDL *hdl )
{
	/* グローバルデータをゼロクリア */
	MlibResetByteMemory( hdl,  sizeof(RSP_HNDL) );
	hdl->enableLtMon[0] = FALSE;	/* ラッチモニタ更新OFF */
	hdl->enableLtMon[1] = FALSE;	/* ラッチモニタ更新OFF */
}




/****************************************************************************/
/* 概要		:	メインコマンドに対するRSPデータの作成を行う					*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rspbuf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeData( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
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
	cmd = pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd;
	rsp_buf->stdData.mt.cmd = cmd;
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* ラッチ状態チェック */
	pRspHdl->last_L_CMP[0] = MciGetLatchStatus( pMtHdl, &pRspHdl->enableLtMon[0], 0 );
	pRspHdl->last_L_CMP[1] = MciGetLatchStatus( pMtHdl, &pRspHdl->enableLtMon[1], 1 );

	/* CMD定義テーブルに登録されているコマンド毎のRSP作成処理関数を実行 */
	cmd_tbl_ptr->rspFuncPtr( hNetData, rsp_buf );

	/* ステータス作成・貼り付け */
	if( pCmdHdl->cmdInfo.profileType == M3STD_PROFILE )
	{
		rsp_buf->stdData.mt.cmdStat = stdRspMakeCMD_STAT( pCmdHdl, pAlmHdl, 
										pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd_ctrl );
	}
#ifdef ML2_PROFILE_SUPP
	else if( prof == M2_PROFILE )
	{
		/* アラームコード貼り付け（メインコマンドのみ） */
		rsp_buf->m2Data.mt.alarm = ( (USHORT)pAlmHdl->errCode >> 4 );	/* 下位１桁切り捨て */
		/* ステータス作成・貼り付け */
		rsp_buf->m2Data.mt.status = m2RspMakeStatus( hNetData );
	}
#endif

	pRspHdl->rspSetDone = pCmdHdl->cmdInfo.cmdRdy;

}


/****************************************************************************/
/* 概要		:	サブコマンドに対するRSPデータの作成を行う					*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData			多軸用ハンドル統括構造体	*/
/*			:	RSP_LINK_EXDATA* ex_rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *ex_rsp_buf )
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

	/* レスポンス用メインコマンド保存バッファのコマンドコードから、
						受信コマンドのCMD定義テーブル（のポインタ）を抽出 */
	cmd = pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd;
//	ex_rsp_buf->stdData.mon.cmd = cmd;
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* CMD定義テーブルに登録されているコマンド毎のRSP作成処理関数を実行 */
	ex_cmd_tbl_ptr->exRspFuncPtr( hNetData, ex_rsp_buf );

	/* ステータス作成・貼り付け */
	if( pCmdHdl->cmdInfo.profileType == M3STD_PROFILE )
	{
		ex_rsp_buf->dword[0] = stdRspMakeSUB_STAT(pCmdHdl, pAlmHdl, 
												  pCmdHdl->exCmdInfo.rsltBuf.dword[0]);
	}
#ifdef ML2_PROFILE_SUPP
	else if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
	{
		ex_rsp_buf->m2Data.mon.cmd = cmd;
		ex_rsp_buf->m2Data.mon.status = m2RspMakeExStatus( pCmdHdl, pAlmHdl );
	}
#endif	

	/* respSetDone = CmdRdy */
	pRspHdl->exRspSetDone = pCmdHdl->exCmdInfo.cmdRdy;
}

/****************************************************************************/
/* 概要		：CPRM_SEL_MON 取得処理											*/
/****************************************************************************/
/* 引数		：	RSP_HNDL *hdl												*/
/*			：	ULONG 	 data		CPRM_SEL_MONの値						*/
/*			：	UCHAR 	 num		モニタ選択番号 [0or1]					*/
/* 戻り値	：	なし														*/
/****************************************************************************/
VOID RspSetcPrmMon(RSP_HNDL *hdl, ULONG data, UCHAR num)
{
	hdl->cPrmMon[num] = data;
}



