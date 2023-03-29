/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント          */
/*                                                                          */
/*            各コマンドレスポンス作成処理 									*/
/*																			*/
/****************************************************************************/
/* ＩＦ定義関数 :                                                           */
/*																			*/
/*                                                                          */
/****************************************************************************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/

/* includes */
#include "Basedef.h"
#include "M2Handler.h"
#include "ComAlmMngr.h"
#include "SysApi.h"
#include "MtApi.h"
#include "MLib.h"

/* forward declarations (local function) */
static	VOID rspADJ_sigma2_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf);
static	VOID rspADJ_normal_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf);
static	VOID rspADJ_other_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf);

/****************************************************************************/
/* 概要		：ステータス処理												*/
/****************************************************************************/
/* 引数		：	RSP_HNDL *hdl												*/
/* 戻り値	：	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
USHORT rspMakeStatus(RSP_HNDL *pRspHdl, CMD_HNDL *pCmdHdl, 
											MT_HNDL *pMtHdl, ALM_HNDL *pAlmHdl)
{
	PSN_HNDL	*psn_hdl_ptr;
	ULONG		RspSeqBit,	RspCtrlBit;
	BOOL		bitwk;
	USHORT		sts;
	UCHAR		wk;

	/* ローカル変数設定	*/
	psn_hdl_ptr	= &pMtHdl->psnHdl;
	RspSeqBit = pMtHdl->svRsp->RspSeqBit;
	RspCtrlBit = pMtHdl->svRsp->RspCtrl.RspCtrlBit;

	sts = 0;
	/*---- D0(ALM) ----------------------------------------------------------*/
	sts |= pAlmHdl->almSts << RSP_DEV_ALM_BIT_NO;
	
	/*---- D1(WARNG) --------------------------------------------------------*/
	sts |= pAlmHdl->wrnSts << RSP_DEV_WRN_BIT_NO;
	
	/*---- D2(CMDRDY) -------------------------------------------------------*/
	sts |= pCmdHdl->cmdInfo.cmdRdy << RSP_CMD_READY_BIT_NO;
	
	/*---- D3(SVON) ---------------------------------------------------------*/
	bitwk = (RspSeqBit >> SERVO_ON_COMP_BITNO) & 1;
	sts |= bitwk << RSP_SVON_BIT_NO;
	
	/*---- D4(PON) ----------------------------------------------------------*/
	bitwk = (RspSeqBit >> MAIN_POWER_ON_BITNO) & 1;
	sts |= bitwk << RSP_POWER_ON_BIT_NO;
	
	/*---- D5(MLOCK) --------------------------------------------------------*//* 未対応	*/
	sts |= FALSE << RSP_MLOCK_BIT_NO;

	/*---- D6(ZPOINT) -------------------------------------------------------*/
	if( (psn_hdl_ptr->RefEnable != FALSE)
		&& ((ULONG)MlibLABS(psn_hdl_ptr->Apos) <= psn_hdl_ptr->Prm->zPointWidth) )
	{
		sts |=  RSP_ZPOINT_BIT;
    }
    
	/*---- D9(T_LIM) --------------------------------------------------------*/
	bitwk = (RspCtrlBit >> TRQ_CLAMP_BITNO) & 1;
	sts |= bitwk << RSP_TRQ_LIM_BIT_NO;
	
	/*---- D10(L_CMP) -------------------------------------------------------*/
	sts |= pRspHdl->last_L_CMP << RSP_LCMP_BIT_NO /* <S04A> L_CMPビット番号間違い RSP_TRQ_LIM_BIT_NO */;
	
	/*---- D10(P-SOT) -------------------------------------------------------*/
	bitwk = psn_hdl_ptr->SoftLimit & 1;
	sts |= (bitwk << RSP_P_SOFT_LIM_BIT_NO);

	/*---- D11(N-SOT) -------------------------------------------------------*/
	bitwk = (psn_hdl_ptr->SoftLimit >> 1) & 1;
	sts |= (bitwk << RSP_N_SOFT_LIM_BIT_NO);

	/*---- 制御モード依存 ---------------------------------------------------*/
	if (pMtHdl->cmdData.CtrlMode == MT_POS_CTRLMD)
	{/* 位置制御 */
		/*---- D7(PSET) -----------------------------------------------------*/
		sts |= psn_hdl_ptr->Pset << RSP_PSET_BIT_NO;
		/*---- D8(DEN) ------------------------------------------------------*/
		sts |= psn_hdl_ptr->Den << RSP_DEN_ON_BIT_NO;
		/*---- D11(NEAR) ----------------------------------------------------*/
		sts |= psn_hdl_ptr->Near << RSP_NEAR_BIT_NO;
	}
	else if (pMtHdl->cmdData.CtrlMode == MT_VEL_CTRLMD)
	{/* 速度制御 */
		/*---- D7(V_CMP) ----------------------------------------------------*/
		bitwk = (RspCtrlBit >> REACH_SPD_BITNO) & 1;
		sts |= bitwk << RSP_PSET_BIT_NO;
		/*---- D8(ZSPD) -----------------------------------------------------*/
		bitwk = ~(RspCtrlBit >> MOTOR_MOVING_BITNO) & 1;/* <S06B>:出力信号を反転 */
		sts |= bitwk << RSP_DEN_ON_BIT_NO;
		/*---- D11(NEAR) ----------------------------------------------------*/
		sts |= psn_hdl_ptr->Near << RSP_NEAR_BIT_NO;
	}
	else
	{/* トルク制御 */
		/*---- D7(PSET) -----------------------------------------------------*/
		sts |= psn_hdl_ptr->Pset << RSP_PSET_BIT_NO;
		/*---- D8(DEN) ------------------------------------------------------*/
		sts |= psn_hdl_ptr->Den << RSP_DEN_ON_BIT_NO;
		/*---- D11(V_LIM) ---------------------------------------------------*/
		bitwk = (RspCtrlBit >> SPD_CLAMP_BITNO) & 1;
		sts |= bitwk << RSP_NEAR_BIT_NO;
	}


	return sts;
}

/****************************************************************************/
/* 概要		：サブステータス処理											*/
/****************************************************************************/
/* 引数		：	RSP_HNDL *hdl												*/
/* 戻り値	：	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
UCHAR rspMakeExStatus(CMD_HNDL *cmdHdl, ALM_HNDL *almHdl)
{
	UCHAR	sts;
	
	/* ローカル変数をクリア	*/
	sts = 0;
	
	cmdHdl->exCmdInfo.warng &= almHdl->wrnSts;
	sts |= cmdHdl->exCmdInfo.warng << RSP_SUBCMD_WRN_BIT_NO;			/* D1(WARNG)	*/
	sts |= cmdHdl->exCmdInfo.cmdRdy << RSP_SUBCMD_READY_BIT_NO;		/* D2(CMDRDY)	*/

	return sts;
}

/****************************************************************************/
/* 概要		：システムコマンドレスポンス処理						(??H)	*/
/****************************************************************************/
/* 対応コマンド：	未定義/PRM_WR/PPRM_WR/ADJ								*/
/****************************************************************************/
/* 引数		：	MLNET_HANDLE *hNetData										*/
/*			：	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	：	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID rsp_SYS_CMD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
}

/****************************************************************************/
/* 概要		：NOP,CONFIG,SYNC_SET,DISCONNECT レスポンス処理	(04/0D/0FH)		*/
/****************************************************************************/
VOID rsp_NOP(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}

/****************************************************************************/
/* 概要		：ＰＲＭ＿ＲＤコマンドレスポンス処理					(01H)	*/
/****************************************************************************/
VOID rsp_PRM_RD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{/* パラメータ読み込み完了	*/
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->word[2] = rslt_wk->word[2];					/* NO 	*/
		rsp_buf->byte[6] = rslt_wk->byte[6];					/* SIZE	*/
		rsp_buf->byte[7] = 0;
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}
}

/****************************************************************************/
/* 概要		：ＩＤ＿ＲＤコマンドレスポンス処理						(03H)	*/
/****************************************************************************/
VOID rsp_ID_RD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	
	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{/* パラメータ読み込み完了	*/
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->byte[4] = rslt_wk->byte[4];			/* DEVICE_CODE	*/
		rsp_buf->byte[5] = rslt_wk->byte[5];			/* OFFSET		*/
		rsp_buf->byte[6] = rslt_wk->byte[6];			/* SIZE			*/
		rsp_buf->byte[7] = 0;
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}

}

/****************************************************************************/
/* 概要		：ＡＬＭ＿ＲＤコマンドレスポンス処理					(05H)	*/
/****************************************************************************/
VOID rsp_ALM_RD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */

	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{/* パラメータ読み込み完了	*/
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->byte[4] = rslt_wk->byte[4];			/* ALM_RD_MOD	*/
		rsp_buf->byte[5] = 0;
		rsp_buf->word[3] = 0;
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}

}
/****************************************************************************/
/* 概要		：ＡＬＭ＿ＣＬＲコマンドレスポンス処理					(06H)	*/
/****************************************************************************/
VOID rsp_ALM_CLR(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT	iomon;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	

	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->byte[4] = rslt_wk->alm.mode;

	/*---------------------------------------------------------------*/
	/*IO_MONフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
    if((hNetData->cmd_hdl.cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)      
    {
	     /* I/O入力信号状態 */		
		iomon = (MciGetInPortSignals(&hNetData->mt_hdl) << 12);
	    iomon |= MciGetInputSignals(&hNetData->mt_hdl);
	    UtilSetWData(rsp_buf->mt.io_mon, iomon);
     }
	/*---------------------------------------------------------------*/
}

/****************************************************************************/
/* 概要		：ＣＯＮＮＥＣＴコマンドレスポンス処理					(0EH)	*/
/****************************************************************************/
VOID rsp_CONNECT(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->byte[4] = rslt_wk->connect.ver;
	rsp_buf->byte[5] = rslt_wk->connect.com_mod;
	rsp_buf->byte[6] = rslt_wk->connect.com_time;
}

/****************************************************************************/
/* 概要		：ＰＯＳ＿ＳＥＴコマンドレスポンス処理					(20H)	*/
/****************************************************************************/
VOID rsp_POS_SET(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT	iomon;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	

	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->byte[4] = rslt_wk->pos_set.ps_cmd;
	rsp_buf->byte[5] = rslt_wk->pos_set.pos[0];
	rsp_buf->byte[6] = rslt_wk->pos_set.pos[1];
	rsp_buf->byte[7] = rslt_wk->pos_set.pos[2];
	rsp_buf->byte[8] = rslt_wk->pos_set.pos[3];

	/*---------------------------------------------------------------*/
	/*IO_MONフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
    if((hNetData->cmd_hdl.cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
    {
	     /* I/O入力信号状態 */		
		iomon = (MciGetInPortSignals(&hNetData->mt_hdl) << 12);
	    iomon |= MciGetInputSignals(&hNetData->mt_hdl);
	    UtilSetWData(rsp_buf->mt.io_mon, iomon);
     }
	/*---------------------------------------------------------------*/
}

/****************************************************************************/
/* 概要		：ＡＤＪコマンドレスポンス処理							(3EH)	*/
/****************************************************************************/
VOID rsp_ADJ(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	UCHAR  subcode;
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	subcode = rslt_wk->adj2.subcode;

	if(subcode == 0x00)
	{/* Σ－Ⅱ互換モードの場合 */
		rspADJ_sigma2_mode(hNetData, rsp_buf);
	}
	else if(subcode == 0x01)
	{/* 通常モードの場合 */
		rspADJ_normal_mode(hNetData, rsp_buf);
	}
	else
	{/* OPTION/IO_MON拡張モード、自動試験モード	*/
		rspADJ_other_mode(hNetData, rsp_buf);
	}
}

/*--- Σ－Ⅱ互換モードの場合 ------------------------------------------------*/
VOID rspADJ_sigma2_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT ccmd;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	ccmd = rslt_wk->adj2.ccmd;
	
	if( (ccmd == 0)
		&&(hNetData->cmd_hdl.cmdInfo.cmdRdy == FALSE) )
	{/* CCMD = 0(読み出し) && CmdRdy = Flase :→ 読み込み中	*/
		rsp_buf->dword[1] = 0;
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
		rsp_buf->byte[4] = rslt_wk->adj2.ccmd;
		rsp_buf->byte[5] = rslt_wk->adj2.caddress[0];
		rsp_buf->byte[6] = rslt_wk->adj2.caddress[1];
	}
	else
	{
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
		rsp_buf->byte[4] = rslt_wk->adj2.ccmd;
		rsp_buf->byte[5] = rslt_wk->adj2.caddress[0];
		rsp_buf->byte[6] = rslt_wk->adj2.caddress[1];
		rsp_buf->byte[7] = rslt_wk->adj2.cdata[0];
		rsp_buf->byte[8] = rslt_wk->adj2.cdata[1];
	}
}
/*--- 通常モードの場合 ------------------------------------------------------*/
VOID rspADJ_normal_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT ccmd;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	ccmd = rslt_wk->adj.ccmd;
	
	if( (ccmd == 0x03)
		&&(hNetData->cmd_hdl.cmdInfo.cmdRdy == FALSE) )
	{/* CCMD = 3(読み出し) && CmdRdy = Flase :→ 読み込み中 */
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->word[4]  = rslt_wk->word[4];
		rsp_buf->word[5]  = rslt_wk->word[5];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
		rsp_buf->byte[14] = 0;
	}
}
/*--- OPTION/IO_MON拡張モード、自動試験モード -------------------------------*/
VOID rspADJ_other_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->byte[14] = 0;
	
}


/****************************************************************************/
/* 概要		：モニタ有りコマンドレスポンス処理						(**H)	*/
/*				BRK_ON/OFF, SENS_ON/OFF, HOLD, LTMOD_ON/OFF, SMON, SV_ON/OFF*/
/*				INTERPORATE, POSING, FEED, LATCH, EX_POSING, ZRET, VELCTRL	*/
/*				TRQCTRL, SVCTRL												*/
/****************************************************************************/
VOID rsp_MON_CMD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	RSP_HNDL	*pRspHdl;
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT		iomon;
	UCHAR		crtlMode;			/* 現在制御モード*/
	UCHAR		sel_mon1,sel_mon2;	/* モニタ選択情報 */

	/* ローカル変数の初期化 */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;

	/* 現在制御モード取得 */
	crtlMode = pMtHdl->cmdData.CtrlMode;

	/* モニタコード1抽出 */
	sel_mon1 = pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 & 0x0F;		/* SEL_MON1を抽出 */

	/* モニタコード2抽出 */
	if (pRspHdl->enableLtMon)
	{	/* ラッチ完了状態: モニタ2にLPOS */
		sel_mon2 = LPOS_MON;
	}
	else
	{	/* モニタコード2抽出 */
		sel_mon2 = (pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 & 0x0F0) >> 4;	/* D4～D7のSEL_MON2を抽出 */
	}

	/* モニタデータ1/2 */
	rsp_buf->mt.mon1 = MciGetMonitorData(pMtHdl, sel_mon1, crtlMode);
	rsp_buf->mt.mon2 = MciGetMonitorData(pMtHdl, sel_mon2, crtlMode);

	/* モニタ選択 */
	rsp_buf->mt.sel_mon12 = (sel_mon1 | (sel_mon2 << 4));

     /* I/O入力信号状態 */		
	iomon = (MciGetInPortSignals(&hNetData->mt_hdl) << 12);
    iomon |= MciGetInputSignals(&hNetData->mt_hdl);
    UtilSetWData(rsp_buf->mt.io_mon, iomon);
}

/****************************************************************************/
/* 概要		：システムコマンドレスポンス処理						(??H)	*/
/****************************************************************************/
/* 対応コマンド：	未定義/PRM_RD/PRM_WR/PPRM_WR/ALM_RD/					*/
/****************************************************************************/
/* 引数		：	MLNET_HANDLE *hNetData										*/
/*			：	RSP_LINK_EXDATA* 	ex_rsp_buf	RSP格納用バッファポインタ	*/
/* 戻り値	：	なし														*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID exRsp_SYS_CMD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->word[1] = rslt_wk->word[1];
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
}
/****************************************************************************/
/* 概要		：ＮＯＰコマンドレスポンス処理							(00H)	*/
/****************************************************************************/
VOID exRsp_NOP(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	rsp_buf->word[1] = 0;
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}
/****************************************************************************/
/* 概要		：ＰＲＭ＿ＲＤコマンドレスポンス処理					(01H)	*/
/****************************************************************************/
VOID exRsp_PRM_RD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	CMD_LINK_EXDATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{/* パラメータ読み込み完了	*/
		rsp_buf->word[1] = rslt_wk->word[1];
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->word[1] = rslt_wk->word[1];					/* NO 	*/
		rsp_buf->byte[4] = rslt_wk->byte[4];					/* SIZE	*/
		rsp_buf->byte[5] = 0;
		rsp_buf->word[3] = 0;
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}
}

/****************************************************************************/
/* 概要		：ＡＬＭ＿ＲＤコマンドレスポンス処理					(05H)	*/
/****************************************************************************/
VOID exRsp_ALM_RD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	CMD_LINK_EXDATA* rslt_wk;
	
	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );
	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */

	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{/* パラメータ読み込み完了	*/
		rsp_buf->word[1] = rslt_wk->word[1];
		rsp_buf->dword[1] = rslt_wk->dword[1];
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->byte[2] = rslt_wk->byte[2];			/* ALM_RD_MOD	*/
		rsp_buf->dword[1] = 0;
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}

}

/****************************************************************************/
/* 概要		：LTMOD_ON/LTMOd_OFF/SMON コマンドレスポンス処理				*/
/****************************************************************************/
VOID exRsp_MON_CMD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	RSP_HNDL	*pRspHdl;
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG		mon3,mon4;			/* モニタ３，４ */
	UCHAR		crtlMode;			/* 現在制御モード*/
	UCHAR		sel_mon3,sel_mon4;	/* モニタ選択情報 */

	/* ローカル変数の初期化 */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;

	/* 現在制御モード取得 */
	crtlMode = pMtHdl->cmdData.CtrlMode;

	/* モニタコード3/4抽出 */
	sel_mon3 = pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 & 0x0F;			/* SEL_MON3を抽出 */
	sel_mon4 = (pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 & 0x0F0) >> 4;	/* SEL_MON4を抽出 */

	/* モニタ3/4データ取得 */
	mon3 = MciGetMonitorData(pMtHdl, sel_mon3, crtlMode);
	mon4 = MciGetMonitorData(pMtHdl, sel_mon4, crtlMode);

	/* モニタ選択 */
	rsp_buf->mon.sel_mon = (sel_mon3 | (sel_mon4 << 4));

	/* モニタ3/4 */
	UtilSetDWData(rsp_buf->mon.mon3, mon3);
	UtilSetDWData(rsp_buf->mon.mon4, mon4);

	/* EX_STATUS */
	UtilSetWData(rsp_buf->mon.exSts, pMtHdl->latch.SeqSts.wdCnt);
}
