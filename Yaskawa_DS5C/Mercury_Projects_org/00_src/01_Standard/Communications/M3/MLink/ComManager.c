/****************************************************************************
Description	: MECHATROLINK Communication Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

Functions     :
 ComInit()				:COM Manager初期化処理		
 ComInitNetwork()		:通信デバイスの初期化処理		
 ComReqPhaseUp()		:通信Phase遷移(Phaseアップ)要求処理
 ComReqPhaseDown()		:通信Phase遷移要求(Phaseダウン)処理		
 ComRunPhaseChange()	:通信Phase遷移実行処理				
 ComReceiveCmd()		:自局受信ステータス取得処理		
 ComSendRsp()			:自局データの送信(ＡＳＩＣへのライト)処理		
 ComSetRspBuffer()		:スレーブWDCカウントアップ、送信用RSPバッファポインタ切替え処理	
 ComGenerateAplCycle()	:通信周期サイクルの生成処理				
 ComCheckNetError()		:通信エラー(受信エラー、WDCエラー)の検出処理	
 ComSetWdtOfs()			:WDTオフセット位置設定処理				
 ComSetExCmdOfs()		:サブコマンドオフセット位置設定処理	
 ComSetFcsErrSts()		:FCSエラー状態セット処理					

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created 

****************************************************************************/
#include "Basedef.h"
#include "KLib.h"
#include "MLib.h"
#include "ComManager.h"
#include "ComCfg.h"

/* Local functions */
/* mWDCのエッジ検出処理	*/			
//static VOID comDetectEdegeOfmWdc( COM_HNDL *pComHdl, INT32 RecvErr );
static VOID comDetectEdegeOfmWdc( COM_HNDL *pComHdl );
/* 通信Phase１への遷移実行処理 */
static	INT32	comRunPhaseDown( COM_HNDL *hdl );
/* 通信Phase２，３への遷移実行処理 */
static	INT32	comRunPhaseUp( COM_HNDL *hdl );
/* 伝送周期,通信周期の範囲チェック処理 */
static	INT32	comChkCycle( LONG t_cyc, LONG com_tim );
/* 伝送周期が通信定義テーブルに登録されているかのチェック処理 */
//static	INT32	comGetCfgTbl( USHORT t_cyc, CFG_TIM_TBL *cfg_tbl_ptr );
/* WDCエラーの検出処理 */
static	LONG comChkWdtErr( UCHAR mWdc, UCHAR *lmWdc, BOOL mask );
/* 受信エラーの検出処理 */
static	LONG comChkRecvErr( LONG errCtr, LONG dtmod, LONG rcvErr, LONG lrcvErr );


/****************************************************************************/
/* 概要		:	COM Manager初期化を行う										*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl					通信マネージャのハンドラ	*/
/*			:	LONG	ax_no					軸No.						*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID ComInit( COM_HNDL *hdl, LONG ax_no )
{
	/* グローバルデータをゼロクリア */
	MlibResetByteMemory( hdl, sizeof(COM_HNDL) );

	/* 自局軸番号セット */
	hdl->my_ax_number = ax_no+1;

	/* バッファポインタセット */
	hdl->CmdBufPtr = hdl->CmdBuffer;
	hdl->RspBufPtr = hdl->RspBuffer;

	/* WDTオフセット位置 */
	hdl->wdtOfs = M3STD_WDT_OFS;
	/* サブコマンドオフセット位置 */
	hdl->exCmdOfs = M3STD_EX_CMD_OFS;

	/* 通信モード初期設定 */
	hdl->NetPrm.aplCycle = 1;
	hdl->NetPrm.tType = SINGLE_TRANSFER;
	hdl->transMode = SINGLE_TRANSFER;

	/* Phase初期設定 */
	hdl->Phase = 0;
	
}

/****************************************************************************/
/* 概要		：通信モードパラメータの設定を行う								*/
/****************************************************************************/
/* 引数		：	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値	：なし															*/
/****************************************************************************//* <S07A> */
VOID ComNetPrmSet( COM_HNDL *hdl, ND_PARAM	nd_prm )
{
	/* 通信モード初期設定 */
	hdl->NetPrm.dataSize = nd_prm.DataSize;
	hdl->NetPrm.nodeAddress = nd_prm.NodeAddress;
}

/****************************************************************************/
/* 概要		:	通信Phase遷移要求（Phaseアップ）を行う。					*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/*				UCHAR phs ：遷移先Phase(1,2,3)								*/
/*		    	UCHAR mode：≠1 単送、=1 連送（phs=1の場合）				*/
/*				USHORT apl_cyc 			： 通信周期 [unit: 伝送周期の倍率]	*/
/* 戻り値 	: = COM_PHS_STATE_ERROR  	: フェーズ移行状態異常				*/
/*		  	   = COM_FUNC_NORMAL_END	: 正常終了							*/
/****************************************************************************/
INT32 ComReqPhaseUp( COM_HNDL *hdl, LONG phs, LONG mode, LONG aplCyc, BOOL AsyncMode )
{
	/* Phase移行中は、エラー */
	if( hdl->reqPhase != 0 )
	{
		return COM_PHASE_ERROR;
	}
	/* Phaseが既にあがっている場合は、正常終了 */
	if( phs <= hdl->Phase )
	{
		return COM_FUNC_NORMAL_END;
	}

	if( AsyncMode == FALSE )
	{ /* 同期通信モード */
		/* Phase Up要求 */
		hdl->NetPrm.tType    = mode;
		hdl->NetPrm.aplCycle = aplCyc;
		hdl->stateOfPhUp = PH_CHK_COM_CYCLE;	/* 通信周期チェック */

		KlibRstLongTimer(&hdl->startTime);
		hdl->PhUpSts = 0;
		hdl->reqPhase = phs;
	}
	else
	{ /* 非同期通信モード(非同期通信時は Phase2 → Phase3 への移行なし) */
		hdl->NetPrm.tType    = SINGLE_TRANSFER;		/* 単送設定をする(未使用) */
		hdl->NetPrm.aplCycle = 1;
		hdl->Phase = 2;								/* 非同期時はPhase2へ移行 */
	}

	return COM_FUNC_NORMAL_END;
}



/****************************************************************************/
/* 概要		:	通信Phase遷移要求（Phaseダウン）を行う。					*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 			:	LONG phs		遷移先Phase(1,2,3)							*/
/* 戻り値	:	= COM_PHASE_ERROR  	: 遷移先異常							*/
/*				= COM_FUNC_NORMAL_END 	: 正常終了							*/
/****************************************************************************/
INT32 ComReqPhaseDown( COM_HNDL *hdl, LONG phs )
{
	/* 移行先Phaseの設定(より低いPhaseへ) */
	if( (hdl->reqPhase != 0) && (hdl->reqPhase < phs) )
	{
		phs = hdl->reqPhase;
	}

	/* 遷移先Phase = 現在Phase時は、そのまま終了 */
	if( (hdl->reqPhase == 0) && (phs == hdl->Phase) )
	{
		return COM_FUNC_NORMAL_END;
	}

	/* 遷移先Phase > 現在Phaseの場合は、エラー */
	if( phs > hdl->Phase )
	{
		return COM_PHASE_ERROR;
	}

	/* Phase Down要求 */
	if( phs == 1 )
	{
		hdl->NetPrm.aplCycle = 1;
		hdl->NetPrm.tType = SINGLE_TRANSFER;
		hdl->recvErrChk = FALSE;			/* 通信エラー検出無効 */
		hdl->Phase = phs;					/* Phase 1セット */
	}
	else if( phs == 2 )
	{
		hdl->Phase = phs;					/* Phase 2セット */
	}

	hdl->stateOfSync = DISABLE_SYNC;		/* 通信周期同期無効状態へ */
	hdl->transMode = SINGLE_TRANSFER;		/* SINGLEモード */
	hdl->wdcErrChk = FALSE;					/* WDCエラー検出無効 */

	return COM_FUNC_NORMAL_END;
}



/****************************************************************************/
/* 概要：通信Phase遷移実行処理を行う。										*/
/****************************************************************************/
/* 引数		：	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値	：	なし														*/
/****************************************************************************/
VOID ComRunPhaseChange( COM_HNDL *hdl )
{
	INT32 rc;

	rc = 0;

	if( hdl->reqPhase != 0 )
	{ /* フェーズ移行要求ありの場合 */
		switch( hdl->reqPhase )
		{
		case 1:	/* Phase1への移行 */
			rc = comRunPhaseDown( hdl );
			break;
		case 2:	/* Phase2への移行 */
		case 3:	/* Phase3への移行 */
			rc = comRunPhaseUp( hdl );
			break;
		default:
			hdl->stateOfPhUp = PH_IDOL;
			hdl->reqPhase = 0;
			hdl->PhUpSts = COM_PHASE_ERROR;
			break;
		}

		/*完了確認 */
		if( rc != COM_FUNC_CONTINUE )
		{
			hdl->reqPhase = 0;
			hdl->stateOfPhUp = PH_IDOL;
		}
	}
}



/****************************************************************************/
/* 概要：通信Phase１への遷移実行処理を行う。								*/
/****************************************************************************/
/* 引数		：	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値	：	フェーズ移行状態											*/
/*				= COM_FUNC_NORMAL_END	: 正常終了							*/
/*				= COM_FUNC_CONTINUE		: 継続実行中						*/
/****************************************************************************/
static	INT32 comRunPhaseDown( COM_HNDL *hdl )
{
	hdl->NetPrm.aplCycle = 1;
	hdl->NetPrm.tType = SINGLE_TRANSFER;
	hdl->transMode = SINGLE_TRANSFER;
	hdl->stateOfSync = DISABLE_SYNC;
	hdl->PhUpSts = 0;
	hdl->Phase = 1;

	return COM_FUNC_NORMAL_END;
}



/****************************************************************************/
/* 概要：通信Phase２，３への遷移実行処理を行う。							*/
/****************************************************************************/
/* 引数		：	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値	：	フェーズ移行状態											*/
/*				= COM_FUNC_NORMAL_END	: 終了								*/
/*				= COM_FUNC_CONTINUE		: 継続実行中						*/
/****************************************************************************/
static	INT32 comRunPhaseUp( COM_HNDL *hdl )
{
	INT32 sts, rc;
	USHORT t_cyc;

	switch( hdl->stateOfPhUp )
	{
	case PH_CHK_COM_CYCLE:		/* 伝送周期, 通信周期チェック */
		t_cyc = NdGetTransmissionCycle();
		sts = comChkCycle( t_cyc, hdl->NetPrm.aplCycle );
		if( sts != COM_FUNC_NORMAL_END )
		{ /* 伝送周期, 通信周期設定 異常 */
			hdl->PhUpSts = sts;									/* エラーコードセット */
			hdl->Phase = 1;										/* Phase 1セット */
			hdl->NetPrm.aplCycle = 1;							/* 通信周期=伝送周期セット */
			rc = sts;
		}
		else
		{
			if( hdl->reqPhase == 3 )
			{ /* Phase3への移行時 */
				hdl->stateOfSync = START_COMCYC; 				/* WDC同期開始 */
				hdl->stateOfPhUp = PH_SYNC_COMCYC;				/* 通信周期タイミング検出状態へ */
				rc = COM_FUNC_CONTINUE;
			}
			else
			{ /* Phase2への移行時 */
				hdl->stateOfSync = DISABLE_SYNC;				/* 通信周期同期解除 */
				hdl->transMode = SINGLE_TRANSFER;				/* Single転送モード */
				hdl->recvErrChk = TRUE;							/* 通信エラー検出有効 */
				hdl->Phase = hdl->reqPhase;						/* Phaseセット */
				rc = COM_FUNC_NORMAL_END;
			}
		}
		break;

	case PH_SYNC_COMCYC:	/* 通信周期タイミング検出 */
		if( hdl->stateOfSync == ENA_WDCCHK )
		{ /* 通信周期タイミング検出完了 */
			hdl->recvErrChk = TRUE;								/* 通信エラー検出有効 */
			hdl->Phase = hdl->reqPhase;							/* Phaseセット */
			rc = COM_FUNC_NORMAL_END;
		}
		else
		{
			rc = COM_FUNC_CONTINUE;
			if( KlibGetLongTimerMs(hdl->startTime) > PH_UP_TIMEOUT ) /* unit:[ms] */
			{ /* タイムアウト時 */
				hdl->stateOfSync = DISABLE_SYNC;				/* 通信周期同期解除 */
				hdl->transMode = SINGLE_TRANSFER;				/* Single転送モード */
				hdl->recvErrChk = TRUE;							/* 通信エラー検出有効 */

				hdl->PhUpSts = COM_WDTOUT_ERR;
				hdl->Phase = 2;
				rc = COM_WDTOUT_ERR;
			}
		}
		break;

	default:
		rc = COM_FUNC_NORMAL_END;
		break;
	}

	return rc;
}



/****************************************************************************/
/* 概要：伝送周期,通信周期の範囲チェックを行う。							*/
/****************************************************************************/
/* 引数		：t_cyc	：伝送周期[0.25us]										*/
/* 引数		：com_cyc	：通信周期[伝送周期]								*/
/* 戻り値	：= COM_FUNC_NORMAL_END	：伝送周期正常							*/
/*			：= COM_TCYC_ERR		：伝送周期異常							*/
/****************************************************************************/
static	INT32 comChkCycle( LONG t_cyc, LONG com_tim )
{
	ULONG com_cyc;
	const CFG_TIM_TBL *pTbl;

	pTbl = &CfgTimTbl[0];

	/* 伝送周期を通信定義テーブルでチェック */
	while( pTbl->SuptCycle != 0xFFFF )
	{
		if( pTbl->SuptCycle == t_cyc )
		{ /* サポート伝送周期 */
			com_cyc = t_cyc * com_tim;
			if( com_cyc > MAX_COM_APL_CYC )
			{ /* 最大値オーバ */
				return COM_APLCYC_ERR;
			}
			if( com_cyc % (pTbl->SvIfDataCyc*CFG_TIO_BASE_TIME) )
			{ /* ScanNの整数倍か */
				return COM_APLCYC_ERR;
			}

			return COM_FUNC_NORMAL_END;
		}
		pTbl++;
	}
	return COM_TCYC_ERR;
}


#if 0
/****************************************************************************/
/* 概要：伝送周期が通信定義テーブルに登録されているかをチェックする。		*/
/****************************************************************************/
/* 引数		：t_cyc	 ：グローバルデータから取得した伝送周期[0.25us]			*/
/*			：pCfgtbl：通信定義テーブル(データ参照用)						*/
/* 戻り値	：= COM_FUNC_NORMAL_END	：伝送周期正常							*/
/*			：= COM_TCYC_ERR		：伝送周期異常							*/
/****************************************************************************/
static	INT32 comGetCfgTbl( USHORT t_cyc, CFG_TIM_TBL *cfg_tbl_ptr )
{
	INT32 ret;
	const CFG_TIM_TBL *tbl_ptr;

	/* ローカル変数の初期化 */
	ret = COM_TCYC_ERR;
	tbl_ptr = &CfgTimTbl[0];

	/* 伝送周期を通信定義テーブルでチェック */
	while( tbl_ptr->SuptCycle != 0xFFFF )
	{
		if( tbl_ptr->SuptCycle == t_cyc )
		{
			*cfg_tbl_ptr = *tbl_ptr;
			ret = COM_FUNC_NORMAL_END;
			break;
		}
		tbl_ptr++;
	}
	return ret;
}
#endif


#if 0
/****************************************************************************************/
/* 概要：自局受信ステータスを取得し、正常受信していれば自局データの受信を行う。			*/
/****************************************************************************************/
/* 引数		：	MLNET_HANDLE* hNetData					多軸用ハンドル統括構造体		*/
/*				JL100_CPU_EXCHNG_DATA* exchng_data_ptr	多軸用ノード共通変数構造体		*/
/* 戻り値	：  = COM_FUNC_NORMAL_END		:			正常受信						*/
/*				= COM_FUNC_ERROR_END		:			受信異常						*/
/* エラーステータス																		*/
/*				= COM_RECV_ERR				:			指令データの受信異常			*/
/*				= COM_RECV_GBL_ERR			:			グローバルデータの受信異常		*/
/*				= COM_RECV_SYNC_ERR			:			同期通信異常					*/
/****************************************************************************************/
INT32 ComReceiveCmd( COM_HNDL *hComMngr )
{
	INT32	rc;

	/* 前回自局受信状態の保存 */
	hComMngr->LrecvErr = hComMngr->RecvErr;

	/* 指令データの受信処理(戻り値は受信異常軸数) */
	if(NdReceiveCMD( hComMngr->my_ax_number, hComMngr->CmdBufPtr ) > 0)
	{/* 自局ﾃﾞｰﾀ正常受信	*/
		hComMngr->RecvErr = FALSE;
		rc = COM_FUNC_NORMAL_END;
	}
	else
	{
		hComMngr->RecvErr = TRUE;
		rc = COM_RECV_ERR;
	}
	
	return rc;
}

/****************************************************************************************/
/* 概要：自局データの送信（ＡＳＩＣへのライト）を行う。									*/
/****************************************************************************************/
/* 引数		:	COM_HNDL* hdl							通信マネージャのハンドラ		*/
/* 戻り値   :	なし																	*/
/****************************************************************************************/
VOID ComSendRsp( COM_HNDL *hComMngr )
{
	NdSendRSP( hComMngr->my_ax_number, hComMngr->RspBufPtr );
}
#endif

/****************************************************************************/
/* 概要：スレーブＷＤＣをカウントアップし、送信用ＲＳＰバッファポインタを	*/
/*		 切替る。															*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値   :	なし														*/
/****************************************************************************/
VOID ComSetRspBuffer( COM_HNDL *pComHdl )
{
	UCHAR	*sWdt;

	sWdt = ( (UCHAR*)pComHdl->RspBufPtr + pComHdl->wdtOfs );

	/* スレーブＷＤＣをカウントアップ */
	pComHdl->sWdc = (UCHAR)(pComHdl->sWdc + 1);
	*sWdt = (UCHAR)( (pComHdl->sWdc << 4) | (pComHdl->mWdc & 0xF) );

}

/****************************************************************************/
/* 概要：レスポンスデータ作成タイミング取得									*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl					通信マネージャのハンドラ	*/
/* 戻り値   :	FALSE/TRUE													*/
/****************************************************************************/
BOOL ComCheckRspTiming( COM_HNDL *pComHdl )
{
	BOOL		rsp_timing;
	INT			apl_cycle;
	INT			rsp_time;
	const INT	rspTimTbl[DTMOD_NUM][5] =
	{  /* 1  2  3  4  5   */
		{ 0, 1, 0, 1, 2 },  /* single mode */
	    { 0, 1, 1, 3, 0 },  /* dual mode */
	    { 0, 0, 2, 0, 1 }   /* dual&single mode */
	};

	apl_cycle = pComHdl->NetPrm.aplCycle-1;
	rsp_time = ((apl_cycle < 5) ?
				rspTimTbl[pComHdl->transMode][apl_cycle] :
				rspTimTbl[pComHdl->transMode][4] + (apl_cycle - 4));

	rsp_timing = (pComHdl->comCycCnt == rsp_time) ? TRUE: FALSE;
	
	return rsp_timing;
}

/****************************************************************************/
/* 概要：通信周期サイクルの生成を行う。										*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl					通信マネージャのハンドラ	*/
/* 戻り値   :	なし														*/
/****************************************************************************/
//VOID ComGenerateAplCycle( COM_HNDL *pComHdl, INT32 RecvErr )
VOID ComGenerateAplCycle( COM_HNDL *pComHdl )	/* ｺﾏﾝﾄﾞの受信をIntSyncで処理を実施しているため、RecvErrは構造体に格納済み	*/
{
	/* 通信周期カウンタをカウントアップ */
	pComHdl->comCycCnt++;

	if( pComHdl->comCycCnt >= pComHdl->NetPrm.aplCycle )
	{
		/* 通信周期カウンタは通信周期設定 - 1 でリセット */
		pComHdl->comCycCnt = 0;
	}

	/* マスタＷＤＣのエッジ検出処理 */
//	comDetectEdegeOfmWdc( pComHdl, RecvErr );
	comDetectEdegeOfmWdc( pComHdl );/* ｺﾏﾝﾄﾞの受信をIntSyncで処理を実施しているため、RecvErrは構造体に格納済み	*/
}


/****************************************************************************/
/* 概要：mWDCのエッジ検出処理を行う。										*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値   :	なし														*/
/****************************************************************************/
static VOID comDetectEdegeOfmWdc( COM_HNDL *pComHdl )
{
	LONG wdchk;

	switch( pComHdl->stateOfSync )
	{
	case ENA_WDCCHK:	/* WDCエラー検出有効(通常状態) */
		break;

	case START_COMCYC:	/* 通信同期開始 */
		wdchk = comChkWdtErr( *((UCHAR*)pComHdl->CmdBufPtr + pComHdl->wdtOfs),
//										&pComHdl->mWdc, pComHdl->NetPrm.wdtErrMask );			/* <S0D5> */
										&pComHdl->mWdcForSync, pComHdl->NetPrm.wdtErrMask );	/* <S0D5> */;
		if( !pComHdl->LrecvErr && !pComHdl->RecvErr && (wdchk == 0) )
		{ /* 前回伝送周期、今回伝送周期で正常受信且つＷＤＣのエッジが検出された場合 */
			if( pComHdl->NetPrm.tType == SINGLE_TRANSFER )
			{
				/* 単送 */
				pComHdl->comCycCnt = 0;
			}
			else
			{
				/* 連送 */
				pComHdl->comCycCnt = pComHdl->NetPrm.aplCycle - 1;
			}

			pComHdl->stateOfSync = READY_COMCYC;
			pComHdl->wdChkCnt = 0;
		}
		break;

	case READY_COMCYC:	/* 通信同期確認 */
		wdchk = comChkWdtErr( *((UCHAR*)pComHdl->CmdBufPtr + pComHdl->wdtOfs),
								&pComHdl->mWdcForSync, pComHdl->NetPrm.wdtErrMask );
		if( !pComHdl->LrecvErr && !pComHdl->RecvErr && (wdchk == 0) )
		{ /* 前回伝送周期、今回伝送周期で正常受信且つＷＤＣのエッジが検出された場合 */
			if( (pComHdl->NetPrm.tType == SINGLE_TRANSFER) && (pComHdl->comCycCnt == 0) )
			{ /* 単送 */
				if( (++pComHdl->wdChkCnt) >= 4 )
				{ /* 4回連続正常 */
					pComHdl->stateOfSync = START_WDCCHK;
					pComHdl->transMode = SINGLE_TRANSFER;
					pComHdl->wdChkCnt = 0;
				}
			}
			else if( (pComHdl->NetPrm.tType == CONSECUTIVE_TRANSFER) 
				&&   (pComHdl->comCycCnt == (pComHdl->NetPrm.aplCycle - 1)) )
			{ /* 連送 */
				if( (++pComHdl->wdChkCnt) >= 4 )
				{ /* 4回連続正常 */
					pComHdl->stateOfSync = START_WDCCHK;
					pComHdl->transMode = CONSECUTIVE_TRANSFER;
					pComHdl->wdChkCnt = 0;
				}
			}
			else
			{ /* タイミングが違った場合は一つ前の状態に戻る */
				if( !pComHdl->NetPrm.wdtErrMask )
				{ /* WDCマスクでない場合のみ、リトライ */
					pComHdl->stateOfSync = START_COMCYC;
				}
			}
		}
		break;

	case START_WDCCHK:	/* WDCエラー検出開始 */
		if( pComHdl->comCycCnt == 0 )
		{
			/* mWDC保存 */
//			pComHdl->mWdc = *((UCHAR*)pComHdl->CmdBufPtr + pComHdl->wdtOfs);
	
			pComHdl->wdChkCnt++;
			if( pComHdl->wdChkCnt >= 2 )
			{ /* タスク同期が安定するのを２周期待つ */
				pComHdl->stateOfSync = ENA_WDCCHK;
				pComHdl->wdcErrChk = TRUE;
			}
		}
		break;

	case DISABLE_SYNC:	/* 通信同期無効 */
		pComHdl->wdcErrChk = FALSE;
		break;

	default : break;
	}

}


/****************************************************************************/
/* 概要		:	通信エラー（受信エラー、ＷＤＣエラー）の検出を行う。		*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 戻り値   : = COM_FUNC_NORMAL_END	正常受信								*/
/*			: = COM_RECV_ERR		受信エラー								*/
/*			: = COM_FCS_ERR			FCSエラー								*/
/*          : = COM_RECV_WARNG		受信ワーニング							*/
/*          : = COM_FCS_WARNG		FCSワーニング							*/
/*          : = COM_WDC_ERR			WDCエラー								*/
/****************************************************************************/
INT32 ComCheckNetError( COM_HNDL *pComHdl )/* ｺﾏﾝﾄﾞの受信をIntSyncで処理を実施しているため、RecvErrは構造体に格納済み	*/
{
	UCHAR	crtCmd;				/* 受信データのメインコマンド */
	INT32	ret;
	LONG	wdchk;

	/* ローカル変数の初期化 */
	ret = COM_FUNC_NORMAL_END;

	/* 受信データのメインコマンド取得 */
	crtCmd = (UCHAR)( pComHdl->CmdBufPtr[0] & 0xff );
	if( crtCmd == CRT_CMD_DISCONNECT )
	{ /* 受信データのメインコマンド取得 == DISCONNECT */
		return ret;
	}

	/* 受信エラーチェック */
	pComHdl->recvErrCnt = comChkRecvErr( pComHdl->recvErrCnt, pComHdl->transMode, pComHdl->RecvErr, pComHdl->LrecvErr );
	if( pComHdl->recvErrCnt == 0 )
	{ /* 正常受信 */
		wdchk = comChkWdtErr( *((UCHAR*)pComHdl->CmdBufPtr+pComHdl->wdtOfs), &pComHdl->mWdc, pComHdl->NetPrm.wdtErrMask );
		if( (wdchk != 0) && pComHdl->wdcErrChk )
		{ /* WDTエラー && WDCエラー検出有効*/
			ComReqPhaseDown( pComHdl, 2 );
			ret = COM_WDC_ERR;
		}
	}
	else if( pComHdl->recvErrCnt == COM_RECV_ERR_LIMIT )
	{ /* 連続受信異常アラーム */
		pComHdl->mWdc = (UCHAR)(pComHdl->mWdc + 1);			/* 通信異常マスク用に mWdt+1 補正 */
		pComHdl->recvErrCntNum = (USHORT)(pComHdl->recvErrCntNum + 1);	/* エラーカウントアップ */

		if( !pComHdl->NetPrm.recvErrMask && pComHdl->recvErrChk )
		{
			ComReqPhaseDown( pComHdl, 2 );
//			if( pComHdl->RecvErr != 0 )
			{
				ret = COM_RECV_ERR;
			}
		}
	}
	else
	{ /* 受信異常ワーニング */
		pComHdl->mWdc = (UCHAR)(pComHdl->mWdc + 1);			/* 通信異常マスク用に mWdt+1 補正 */
		pComHdl->recvErrCntNum = (USHORT)(pComHdl->recvErrCntNum + 1);	/* エラーカウントアップ */

		if( !pComHdl->NetPrm.recvWrnMask && pComHdl->recvErrChk )
		{
//			if( pComHdl->RecvErr != 0 )
			{
				ret = COM_RECV_WARNG;
			}
		}
	}
	return ret;
}



/****************************************************************************/
/* 概要		:	受信エラーの検出を行う。									*/
/****************************************************************************/
/* 引数		:	LONG errCnt					受信エラーカウント値			*/
/*			:	LONG dtmod					転送方式						*/
/*			:	BOOL rcvErr					通信異常状態					*/
/*			:	BOOL lrcvErr				前回通信異常状態				*/
/* 戻り値   :	= エラーカウント(errCnt)									*/
/****************************************************************************/
static	LONG comChkRecvErr( LONG errCtr, LONG dtmod, LONG rcvErr, LONG lrcvErr )
{
	/* 受信正常 */
	if( rcvErr == 0 )
	{
		return 0;
	}

	/* 受信異常 */
	if( dtmod == CONSECUTIVE_TRANSFER )
	{ /* 連送 */
		if( lrcvErr != 0 )
		{
			errCtr = COM_RECV_ERR_LIMIT;
		}
		else
		{
			errCtr = 0;
		}
	}
	else
	{ /* 単送 */
		errCtr++;
		if( errCtr >= COM_RECV_ERR_LIMIT )
		{
			errCtr = COM_RECV_ERR_LIMIT;		/* 通信異常 */
		}
	}
	return errCtr;
}



/****************************************************************************/
/* 概要		:	ＷＤＣエラーの検出を行う。									*/
/****************************************************************************/
/* 引数		:	LONG  mWdc		マスタWDC									*/
/*			:	LONG* lmWdc		前回マスタWDCポインタ						*/
/*			:	BOOL  mask		WDCマスク設定								*/
/* 戻り値	:	= 0				正常					    				*/
/*			:	!= 0			ＷＤＣエラー								*/
/****************************************************************************/
static	LONG comChkWdtErr( UCHAR mWdc, UCHAR *lmWdc, BOOL mask )
{
	LONG ret;

	/* ローカル変数の初期化 */
	ret = 0;

	if( !mask )
	{
		ret = ( mWdc - (*lmWdc + 1) ) & 0xF;
	}

	*lmWdc = mWdc;
	return ret;
}


/****************************************************************************/
/* 概要：WDTオフセット位置を設定する										*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/*				LONG tdata_size	伝送バイト数								*/
/*				UCHAR wdtOfs	WDTオフセット位置（バイト）					*/
/* 戻り値   : = COM_FUNC_NORMAL_END	正常受信								*/
/* 			  = COM_WDT_OFS_ERR		設定処理失敗							*/
/****************************************************************************/
INT32 ComSetWdtOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG wdtOfs )
{
	INT32 ret;

	if( wdtOfs <= tdata_size )
	{
		pComHdl->wdtOfs = wdtOfs;
		ret = COM_FUNC_NORMAL_END;
	}
	else
	{
		ret = COM_WDT_OFS_ERR;
	}
	return ret;
}



/****************************************************************************/
/* 概要：サブコマンドオフセット位置を設定する								*/
/****************************************************************************/
/* 引数		:	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 引数		:	UCHAR exCmdOfs	サブコマンドオフセット位置（バイト）		*/
/* 戻り値   : = COM_FUNC_NORMAL_END	正常受信								*/
/* 			  = COM_EXCMD_OFS_ERR	設定処理失敗							*/
/****************************************************************************/
INT32 ComSetExCmdOfs( COM_HNDL *pComHdl, LONG tdata_size, LONG exCmdOfs )
{
	INT32 ret;

	if( exCmdOfs <= tdata_size )
	{
		pComHdl->exCmdOfs = exCmdOfs;
		ret = COM_FUNC_NORMAL_END;
	}
	else
	{
		ret = COM_EXCMD_OFS_ERR;
	}
	return ret;
}


#if 0
/****************************************************************************/
/* 概要：FCSエラー状態セット												*/
/****************************************************************************/
/* 引数		：	COM_HNDL* hdl	通信マネージャのハンドラ					*/
/* 引数		：	なし														*/
/* 戻り値   :   なし														*/
/****************************************************************************/
VOID ComSetFcsErrSts( COM_HNDL *hdl )
{
	/* 2010.02.08 Y.Oka SGDV不具合修正反映 */
//	hdl->LrecvErr = hdl->RecvErr;	/* 前回値を保存 */
	hdl->RecvErr = RECV_FCS_ERR;	/* FCS異常状態を設定 */
}
#endif



