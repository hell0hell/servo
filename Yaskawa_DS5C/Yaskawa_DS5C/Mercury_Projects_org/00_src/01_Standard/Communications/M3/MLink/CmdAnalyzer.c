/****************************************************************************
Description	: MECHATROLINK Command Analyzer
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.04.xx    created
I.M.   2010.10.xx    renewal

****************************************************************************/
#include "Basedef.h"
#include "KLib.h"
#include "CmdAnalyzer.h"
#include "MtApi.h"
#include "ComManager.h"
#include "ComAlmMngr.h"
#include "CmdTable.h"
#include "SysApi.h"
#include "MLib.h"


/* タイムアウト処理 */
static	INT32 cmdCancelProc( MLNET_HANDLE *hNetData, const CMD_DEF_TBL *pTbl );



/****************************************************************************/
/* 概要		:	CMD AnaLyzerの初期化を行う									*/
/****************************************************************************/
/* 引数		:	CMD_HNDL* hdl	CMD ANALYZER の	ハンドラ					*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdInit( CMD_HNDL *pCmdHdl )
{
	/* グローバルデータをゼロクリア */
	MlibResetByteMemory( pCmdHdl, sizeof(CMD_HNDL) );

	pCmdHdl->cmdInfo.profileType = M3STD_PROFILE;
	pCmdHdl->cmdInfo.cmdRdy = TRUE;

	/* 受信コマンドのコマンド定義テーブル（のポインタ）を設定 */
	CmdSetCMDTablePointer(M3STD_PROFILE, 
						  (void**)&pCmdHdl->CmdIdxTbl, 
						  (void**)&pCmdHdl->cmdTblBasePtr, 
						  (void**)&pCmdHdl->exCmdTblBasePtr);
}

/****************************************************************************/
/* 概要		:	メインコマンド解析用バッファの更新とコマンド解析実行要求の	*/
/*				作成を行う。												*/
/****************************************************************************/
/* 引数		:	CMD_HNDL* pCmdHdl		CMD ANALYZER の	ハンドラ			*/
/*				COM_HNDL* pComHdl		通信マネージャのハンドラ			*/
/*				BOOL rsp_set			メインコマンド応答完了フラグ		*/
/* 戻り値	:	= TRUE  : コマンド解析用バッファ更新						*/
/*				= FALSE : コマンド解析用バッファ未更新						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdUpdateData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL rsp_set )
{
	ULONG *cmd_wk;
	ULONG *ana_wk;

	cmd_wk = (ULONG*)pComHdl->CmdBufPtr;
	ana_wk = (ULONG*)( &pCmdHdl->cmdInfo.cmdBuf );

	if( (pCmdHdl->cmdInfo.cmdRdy && rsp_set) || ((UCHAR)(*cmd_wk) == DISCONNECT_CMD) )
	{ /* CMD解析用バッファ更新 */
		*( ana_wk + 0 ) = *( cmd_wk + 0 );
		*( ana_wk + 1 ) = *( cmd_wk + 1 );
		*( ana_wk + 2 ) = *( cmd_wk + 2 );
		*( ana_wk + 3 ) = *( cmd_wk + 3 );
		*( ana_wk + 4 ) = *( cmd_wk + 4 );
		*( ana_wk + 5 ) = *( cmd_wk + 5 );
		*( ana_wk + 6 ) = *( cmd_wk + 6 );
		*( ana_wk + 7 ) = *( cmd_wk + 7 );
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************/
/* 概要		:	サブコマンド解析用バッファの更新とコマンド解析実行要求の	*/
/*				作成を行う。												*/
/****************************************************************************/
/* 引数		:	CMD_HNDL* pCmdHdl		CMD ANALYZER の	ハンドラ			*/
/*				COM_HNDL* pComHdl		通信マネージャのハンドラ			*/
/*				BOOL ex_rsp_set			サブコマンド応答完了フラグ			*/
/* 戻り値	:	= TRUE  : コマンド解析用バッファ更新						*/
/*				= FALSE : コマンド解析用バッファ未更新						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdUpdateExData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL ex_rsp_set )
{
	ULONG *cmd_wk;
	ULONG *ana_wk;

	cmd_wk = (ULONG*)( pComHdl->CmdBufPtr + pComHdl->exCmdOfs );
	ana_wk = (ULONG*)( &pCmdHdl->exCmdInfo.cmdBuf );

	if( pCmdHdl->exCmdInfo.cmdRdy && ex_rsp_set )
	{ /* SUB CMD解析用バッファ更新 */
		*( ana_wk + 0 ) = *( cmd_wk + 0 );
		*( ana_wk + 1 ) = *( cmd_wk + 1 );
		*( ana_wk + 2 ) = *( cmd_wk + 2 );
		*( ana_wk + 3 ) = *( cmd_wk + 3 );
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************/
/* 概要		:	メインコマンドの解析・実行を行う。							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdAnalyze( MLNET_HANDLE *hNetData )
{
	INT32 ret;
	LONG cmd;							/* コマンドコード */
	LONG phs;							/* 通信フェーズ */
	CMD_HNDL *pCmdHdl;					/* コマンドハンドルへのポインタ */
	ALM_HNDL *pAlmHdl;					/* コマンドハンドルへのポインタ */
	CMD_DEF_TBL *cmd_tbl_ptr;			/* コマンド定義テーブル */

	/* 呼び出し回数が多いため、アドレス位置を予め計算しておく */
	pCmdHdl = &hNetData->cmd_hdl;
	pAlmHdl = &hNetData->alm_hdl;

	if( pCmdHdl->cmdInfo.enableCmdAna == FALSE )
	{/* メインコマンド実行処理要求なし */
		return;
	}

	/* コマンドコードの取得 */
	cmd = (LONG)pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;

	/* 受信コマンドのコマンド定義テーブル（のポインタ）を抽出 */
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* コマンド解析実行結果クリア */
	pCmdHdl->cmdInfo.ResultCmdAna = ~CMD_FUNC_NORMAL_END;

	/* フェーズチェック */
	phs = hNetData->com_hdl.Phase;
	if( phs >= cmd_tbl_ptr->availablePhase )
	{ /* フェーズチェック OK */
		if( cmd_tbl_ptr->chkCmdLock & pCmdHdl->exCmdInfo.busy )
		{ /* コマンド干渉している場合 */
			pCmdHdl->cmdInfo.rsltBuf = pCmdHdl->cmdInfo.cmdBuf;
			ret = CMD_CMDLOCK_ERR;
		}
		else
		{
			/* コマンド解析実行処理 */
			ret = cmdCancelProc( hNetData, cmd_tbl_ptr );
			if( ret == CMD_FUNC_NORMAL_END )
			{
				ret = cmd_tbl_ptr->cmdFuncPtr( hNetData );
			}
		}
	}
	else
	{
		if( phs > 1 )
		{ /* 解析結果バッファへCMDバッファのデータをコピー */
			pCmdHdl->cmdInfo.rsltBuf = pCmdHdl->cmdInfo.cmdBuf;
			if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
			{ /* M2互換プロファイルのときは、A.95Aを返す */
				ret = CMD_PHASE_ERR_M2;
			}
			else
			{ /* 上記以外はA.97Aを返す */
				ret = CMD_PHASE_ERR_M3;
			}
		}
		else
		{ /* Phase = 1 の時は、何もしない */
			pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = 0x00;
			ret = CMD_FUNC_NORMAL_END;
		}
	}

	pCmdHdl->cmdInfo.crtCmd = (LONG)pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	pCmdHdl->cmdInfo.busy   = cmd_tbl_ptr->chkCmdLock;
	if( ret == CMD_FUNC_NORMAL_END )
	{ /* 正常終了 */
		pCmdHdl->cmdInfo.cmdRdy = TRUE;
		pCmdHdl->cmdInfo.enableCmdAna = FALSE;
	}
	else if( ret == CMD_FUNC_CONTINUE )
	{ /* 継続実行中 */
		pCmdHdl->cmdInfo.cmdRdy = FALSE;
	}
	else
	{ /* 異常終了 */
//		AlmSetCode( pAlmHdl, hNetData->com_hdl.my_ax_number, ret );
		AlmSetCmdAlmCode( pAlmHdl, ret, FALSE );
		pCmdHdl->cmdInfo.CmdAlm = TRUE;
		pCmdHdl->cmdInfo.warng  = TRUE;					/* メインコマンド解析での異常を記憶 */
		pCmdHdl->cmdInfo.cmdRdy = TRUE;
		pCmdHdl->cmdInfo.enableCmdAna = FALSE;
	}
	/* コマンド解析正常終了チェック */
	if( pCmdHdl->cmdInfo.ResultCmdAna == CMD_FUNC_NORMAL_END )
	{
		pCmdHdl->cmdInfo.CmdAlm = FALSE;
	}
}


/****************************************************************************/
/* 概要		:	サブコマンドの解析・実行を行う。							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdAnalyzeEx( MLNET_HANDLE *hNetData )
{
	INT32 ret;
	LONG cmd;								/* コマンドコード */
	LONG phs;								/* 通信フェーズ */
	LONG idx;
	CMD_HNDL *pCmdHdl;						/* コマンドハンドルへのポインタ */
	ALM_HNDL *pAlmHdl;						/* コマンドハンドルへのポインタ */
	EX_CMD_DEF_TBL *ex_cmd_tbl_ptr;			/* サブコマンド定義テーブル */

	/* 呼び出し回数が多いため、アドレス位置を予め計算しておく */
	pCmdHdl = &hNetData->cmd_hdl;
	pAlmHdl = &hNetData->alm_hdl;

	if( pCmdHdl->exCmdInfo.enableCmdAna == FALSE )
	{ /* サブコマンド実行処理要求なし */
		return;
	}

	/* コマンドコードの取得 */
	cmd = (LONG)pCmdHdl->exCmdInfo.cmdBuf.stdData.mon.cmd;

	/* 受信コマンドのサブコマンド定義テーブル（のポインタ）を抽出 */
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* サブコマンド未使用の場合は終了 */
	if( pCmdHdl->useExCmd == FALSE )
	{
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = 0x00;
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
		return;
	}

	/* コマンド解析実行結果クリア */
	pCmdHdl->exCmdInfo.ResultCmdAna = ~CMD_FUNC_NORMAL_END;

	/* 通信フェーズ取得 */
	phs = hNetData->com_hdl.Phase;
	if( phs >= ex_cmd_tbl_ptr->availablePhase )
	{ /* フェーズチェック OK */
		if( ex_cmd_tbl_ptr->chkCmdLock & pCmdHdl->cmdInfo.busy )
		{ /* コマンド干渉している場合 */
			pCmdHdl->exCmdInfo.rsltBuf = pCmdHdl->exCmdInfo.cmdBuf;
			ret = CMD_CMDLOCK_ERR;
		}
		else
		{
			/* サブコマンド解析実処理 */
			ret = ex_cmd_tbl_ptr->exCmdFuncPtr( hNetData );
		}
	}
	else
	{
		if( phs > 1 )
		{ /* 解析結果バッファへCMDバッファのデータをコピー */
			pCmdHdl->exCmdInfo.rsltBuf = pCmdHdl->exCmdInfo.cmdBuf;
			if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
			{ /* M2互換プロファイルのときは、A.95Aを返す */
				ret = CMD_PHASE_ERR_M2;
			}
			else
			{ /* 上記以外はA.97Aを返す */
				ret = CMD_PHASE_ERR_M3;
			}
		}
		else
		{ /* Phase = 1 の時は、何もしない*/
			pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = 0x00;
			ret = CMD_FUNC_NORMAL_END;
		}
	}

	if( ret == CMD_FUNC_NORMAL_END )
	{ /* 正常終了 */
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
	}
	else if( ret == CMD_FUNC_CONTINUE )
	{ /* 継続実行中 */
		pCmdHdl->exCmdInfo.cmdRdy = FALSE;
		pCmdHdl->exCmdInfo.busy   = TRUE;
	}
	else
	{ /* 異常終了 */
		/* ret = コマンドコード(３バイト) + アラームＩＤ（１，２バイト）とする */
//		AlmSetCode( pAlmHdl, hNetData->com_hdl.my_ax_number, ret );
		AlmSetCmdAlmCode( pAlmHdl, ret, TRUE );
		pCmdHdl->exCmdInfo.CmdAlm = TRUE;
		pCmdHdl->exCmdInfo.warng  = TRUE;	/* サブコマンド解析での異常はワーニングのみ */
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
	}
	/* コマンド解析正常終了チェック */
	if( pCmdHdl->exCmdInfo.ResultCmdAna == CMD_FUNC_NORMAL_END )
	{
		pCmdHdl->exCmdInfo.CmdAlm = FALSE;
	}
}



/****************************************************************************/
/* 概要		:	タイムアウト処理を行う。									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*				INT32 ret					コマンド解析実行処理の戻り値	*/
/* 戻り値	:	正常終了		:	CMD_FUNC_NORMAL_END						*/
/* 				タイムアウト	:	CMD_TIME_OUT_ERR						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
static	INT32 cmdCancelProc( MLNET_HANDLE *hNetData, const CMD_DEF_TBL *pTbl )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	UCHAR		cmd;							/* コマンドコード */ /* <S0EC> */
	INT32		rc;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	rc			= CMD_FUNC_NORMAL_END;

	/* コマンドコードの取得 */
	cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	if( pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd != cmd )
	{
		/* コマンド切り替わり→ラッチ要求フラグのクリア[M2互換プロファイル] */
		if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
		{
			MciCancelLatch( pMtHdl, MOTION_LTMD, 0 );
		}
		
		/* ラッチ機能付きコマンドからの切り替わり→ラッチ要求フラグのクリア[M3標準プロファイル] */
		if( (pCmdHdl->cmdInfo.profileType == M3STD_PROFILE) &&
			(pMtHdl->latch[0].Mode) > MODAL_LTMD )
		{
			MciCancelLatch( pMtHdl, MOTION_LTMD, 0 );
			pCmdHdl->cmdInfo.reLatchReq = TRUE;
		}
	}

	/* タイムアウトチェック */
	if( (pCmdHdl->cmdInfo.cmdRdy) || (pTbl->timeOut == 0) )
	{ /* CmdRdy=1 or タイムアウトチェックなし */
		KlibRstLongTimer( &pCmdHdl->cmdInfo.busyCnt );
		return rc;
	}
	if( KlibGetLongTimerMs(pCmdHdl->cmdInfo.busyCnt) > pTbl->timeOut ) /* unit:[ms] */
	{ /* !コマンドタイムアウト! */
		if( cmd == 0x31 )
		{ /* SV_ON */
			MciSetServoOn( pMtHdl, FALSE, TRUE );
			rc = CMD_TIME_OUT_ERR;
		}
		else if( cmd == 0x23 )
		{ /* SENS_ON */
			MciSetSensOn( pMtHdl, FALSE );
			rc = CMD_TIME_OUT_ERR;
		}
		else
		{ /* その他のコマンドのときは何もしない */
			KlibRstLongTimer( &pCmdHdl->cmdInfo.busyCnt );
			rc = CMD_FUNC_NORMAL_END;
		}
		AlmSetCode( &hNetData->alm_hdl, hNetData->com_hdl.my_ax_number, CMD_TIME_OUT_ERR );
	}
	return rc;
}



/****************************************************************************/
/* 概要		:		プロファイルタイプをチェックし、						*/
/*					WDT,サブコマンドのオフセット位置、コマンド定義テーブルの*/
/*					ベースポインタの設定を行う。							*/
/****************************************************************************/
/* 引数		:	CMD_HNDL* pCmdHdl		CMD ANALYZER のハンドル				*/
/*				COM_HNDL* pComHdl		通信マネージャのハンドラ			*/
/*				LONG tdata_size			伝送バイト数						*/
/* 戻り値	:	CMD_FUNC_NORMAL_END	：	正常終了							*/
/* 			:	CMD_PROFILE_DETECT	：	正常終了（プロファイルタイプ決定)	*/
/* 			:	CMD_PROFILE_UNKNOWN	：	プロファイルタイプ Unknown			*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdSetProfileType( MLNET_HANDLE *hNetData, LONG tdata_size )
{
	CMD_HNDL *pCmdHdl;
	COM_HNDL *pComHdl;
	INT32 ret;
	UCHAR profile;					/* プロファイルタイプ */

	/* ローカル変数の初期値設定 */
	pCmdHdl	= &( hNetData->cmd_hdl );
	pComHdl	= &( hNetData->com_hdl );
//	ret		= CMD_FUNC_NORMAL_END;

	/*--------------------------------------------------------------------*/
	/* Phase=1でCONNECTコマンドを受信した場合にベースポインタの設定を実行 */
	/*--------------------------------------------------------------------*/
//	if( (pCmdHdl->cmdInfo.cmdBuf.stdData.connect.cmd == 0x0E) )
	{
		/* プロファイルチェック */
		profile = pCmdHdl->cmdInfo.cmdBuf.stdData.connect.profile_type;
		if( profile == M3STD_PROFILE )
		{
			/* プロファイルタイプ設定 */
			pCmdHdl->cmdInfo.profileType = profile;
			/* WDTオフセット位置設定 */
			ComSetWdtOfs( pComHdl, M3STD_WDT_OFS, M3STD_WDT_OFS );
			/* サブコマンドオフセット位置設定 */
			ComSetExCmdOfs( pComHdl, M3STD_EX_CMD_OFS, M3STD_EX_CMD_OFS );

			MtcSetMtInfoByProfileType( &hNetData->mt_hdl, pCmdHdl->cmdInfo.profileType );
			ret = CMD_PROFILE_DETECT;
		}
		else if( profile == M2_PROFILE )
		{
			/* プロファイルタイプ設定 */
			pCmdHdl->cmdInfo.profileType = profile;
			/* WDTオフセット位置設定 */
			ComSetWdtOfs( pComHdl, M2_WDT_OFS, M2_WDT_OFS );
			/* サブコマンドオフセット位置設定 */
			ComSetExCmdOfs( pComHdl, M2_EX_CMD_OFS, M2_EX_CMD_OFS );

			MtcSetMtInfoByProfileType( &hNetData->mt_hdl, pCmdHdl->cmdInfo.profileType );
			ret = CMD_PROFILE_DETECT;
		}
		else
		{ /* 未知のプロファイルに対しては現在の設定維持 */
			/* 初期化時にM3標準プロファイルの情報をセットしている */
			ret = CMD_PROFILE_UNKNOWN;
		}

		/* 受信コマンドのコマンド定義テーブル（のポインタ）を設定 */
		CmdSetCMDTablePointer(profile, 
							  (void**)&pCmdHdl->CmdIdxTbl, 
							  (void**)&pCmdHdl->cmdTblBasePtr, 
							  (void**)&pCmdHdl->exCmdTblBasePtr);

		/* CMD_ALM自動クリアモード設定 */
		AlmSetCmdAlmClearMode( &(hNetData->alm_hdl), 
				pCmdHdl->cmdInfo.profileType, (hNetData->reg_hdl.Prm)->comCtrl);
	}
	return ret;
}



/****************************************************************************/
/* 概要		：	コネクション切断処理										*/
/*				フェーズダウン処理、モーション無効処理を実行する			*/
/****************************************************************************/
/* 引数		：	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	CMD_FUNC_NORMAL_END	：	正常終了							*/
/* 			：	CMD_FUNC_CONTINUE	：	処理実行中							*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdRunDisConnect( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	INT32		ret, result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	result		= CMD_FUNC_CONTINUE;

	/* 通信フェーズチェック */
	switch( pCmdHdl->cmd_seq )
	{
	case REQ_PHSDOWN_SEQ:		/* フェーズダウン要求 */
		pCmdHdl->useExCmd = FALSE;	/* サブコマンド不可状態へ */
		ComReqPhaseDown( pComHdl, 1 );
		pCmdHdl->cmd_seq = SV_OFF_SEQ;
//	/*	break;  ↓↓下へ↓↓*/

	case SV_OFF_SEQ:			/* サーボオフ指令 */
		if( MciSetServoOn( &hNetData->mt_hdl, FALSE, TRUE ) == MCI_SUCCESS )
		{
			hNetData->mt_hdl.svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
			pCmdHdl->cmd_seq = MT_DISABLE_SEQ;
		}
		break;

	case MT_DISABLE_SEQ:		/* モーション無効 */
		pCmdHdl->cmdInfo.lstSvCmdCtrl = 0;	/* LT_REQの前回値クリア */
		ret = MciResetMotion( &hNetData->mt_hdl );
		if( ret != MCI_WAIT_COMP )
		{
			pCmdHdl->cmd_seq = (ret == MCI_SUCCESS) ? 
								PHASE_CHK_SEQ : SV_OFF_SEQ;
		}
		break;

	case PHASE_CHK_SEQ:			/* フェーズダウンチェック */
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret != COM_FUNC_CONTINUE )
		{
			pCmdHdl->cmd_seq = SOFT_RESET_SEQ;
		}
		break;

	case SOFT_RESET_SEQ:		/* システムリセット処理実行 */
		ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_SYSRST, NULL);
		if( ret == REG_CONTINUE ) 
		{ /* ソフトリセット処理終了 */
			pCmdHdl->cmd_seq = WAIT_SOFT_RESET_SEQ;
		}
		break;

	case WAIT_SOFT_RESET_SEQ:	/* システムリセット待ち */
		if(0 >= RegCheckCommandState(&hNetData->reg_hdl))
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			result = CMD_FUNC_NORMAL_END;
		}
		break;

	default:
		result = CMD_FUNC_NORMAL_END;
		break;
	}

	return result;
}



/****************************************************************************/
/* 概要		：	通信リセット要求処理										*/
/*				通常運転時にTMCフレームを受信したら、コネクション切断処理	*/
/*				の実行を要求する											*/
/****************************************************************************/
/* 引数		：	CMD_HNDL* hdl	CMD ANALYZER の	ハンドル					*/
/* 戻り値	：	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
LONG CmdReqComReset( MLNET_HANDLE *hNetData )
{
	if( hNetData->com_hdl.Phase > 0 )
	{ 
		if( hNetData->com_hdl.Phase >= 2 )
		{/* 通信フェーズ≧2の場合は通信リセット要求を実行 */
			hNetData->cmd_hdl.cmd_seq = REQ_PHSDOWN_SEQ;
		}

		if( CmdRunDisConnect( hNetData ) == CMD_FUNC_NORMAL_END )
		{
			hNetData->com_hdl.Phase = 0;
		}
	}
	else
	{ /* 通信フェーズ<2の場合はすでに通信リセットされているのでPhase=0にする */
		hNetData->com_hdl.Phase = 0;
	}

	return hNetData->com_hdl.Phase;
}



