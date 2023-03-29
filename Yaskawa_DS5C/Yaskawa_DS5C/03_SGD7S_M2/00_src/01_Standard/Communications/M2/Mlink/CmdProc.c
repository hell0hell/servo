/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント          */
/*                                                                          */
/*            各コマンド解析処理											*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	加速度指令追加									*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*******************  Yaskawa Electric Corporation **************************/

/* includes */
#include "Basedef.h"
#include "M2Handler.h"
#include "ComAlmMngr.h"
#include "ComManager.h"
#include "SysApi.h"
#include "MtApi.h"
#include "CmdAnalyzer.h"
#include "MLib.h"

/* ID_RD */
#define ID_RD_MAX_SIZE			8				/* 最大読み出しサイズ	*/

/* ALM_RD */
#define ALM_RD_MOD0				0				/* 現在発生中の異常・警告の状態 */
#define ALM_RD_MOD1				1				/* 異常発生状況履歴 */
#define ALM_RD_MOD2				2				/* 現在発生中の異常・警告の詳細情報 */
#define ALM_RD_MOD3				3				/* 異常発生状況履歴の詳細情報 */

/* CONFIG */
#define CONFIG_MOD0				0				/* パラメータ再計算 */
#define CONFIG_MOD1				1				/* EEPROMへの一括書き込み */

/* ALM_CLR */
#define ALM_CLR_MOD0			0				/* 現在発生中の異常・警告の状態クリア */
#define ALM_CLR_MOD1			1				/* 異常発生状況履歴クリア */

/* MEM_RD */
#define MEM_RD_MAX_SIZE			20				/* 最大読み出しサイズ(メインコマンド) */
#define EX_MEM_RD_MAX_SIZE		4				/* 最大読み出しサイズ(サブコマンド)	 */

/* MEM_WR */
#define MEM_WR_MAX_SIZE			20				/* 最大書き込みサイズ(メインコマンド) */
#define EX_MEM_WR_MAX_SIZE		4				/* 最大書き込みサイズ(サブコマンド)	 */

/* PRM_RD/PRM_WR/PPRM_WR */
#define RAM_OFFSET 				0x1000			/* RAM書き込み用オフセット */
#define EEPROM_OFFSET			0				/* EEPROM書き込み用オフセット */

/* 仮想メモリ */
#define	VMEM_VEN_MIN			0x80000000		/* ベンダースペシフィック領域 開始アドレス	*/
#define	VMEM_VEN_MAX			0x8001FFFF		/* ベンダースペシフィック領域 終了アドレス	*/

/* LTMOD_ON/OFF */
#define MODAL_LATCH			0		/* モーダルラッチ要求 */
#define	MOTION_LATCH		1		/* モーションラッチ要求 */

/* MOTION CMD */
#define	CMD_MAXTRQ			0x40000	/* コマンドのＴＲＱ指令のＭＡＸ値  2^14*/

/* SVCTRL */
#define	SVCTRL_HOLD			0		/* モーション選択 HOLD */
#define	SVCTRL_INTERPOLATE	1		/* モーション選択 INTERPOLATE */
#define	SVCTRL_FEED			2		/* モーション選択 FEED */
#define	SVCTRL_POSING		3		/* モーション選択 POSING */

#define SVCTRL_SON_SEL		1		/* シーケンス信号 SON */
#define SVCTRL_BRK_SEL		2		/* シーケンス信号 BRK */
#define SVCTRL_SEN_SEL		4		/* シーケンス信号 SEN */
#define SVCTRL_ACLR_SEL		8		/* シーケンス信号 ACLR */

/* forward declarations (local function) */
static INT32 cmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst);
static INT32 cmdAlmRdProc(ALM_HNDL* alm_hdl, UCHAR alm_rd_mod, UCHAR idx, UCHAR* alm_data);
static INT32 exCmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst);
static INT32 cmdChkPrmWr(CMD_LINK_DATA* cmd_buf,CMD_LINK_DATA* rslt_buf);
static INT32 cmdChkConnectCmd(MLNET_HANDLE *hNetData, LONG *reqphs, LONG *dtmod, LONG *com_tim);
static INT32 cmdChkExPrmWr(CMD_LINK_EXDATA* cmd_buf,CMD_LINK_EXDATA* rslt_buf);
static INT32 cmdSvCtrlMotionSelectProc( MLNET_HANDLE *hNetData, CHAR sel_mt);
static INT32 cmdADJ_normal_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl );
static INT32 cmdADJ_sigma2_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl );
static INT32 cmdADJ_tester_mode(CMD_HNDL* hdl);
static INT32 cmdADJ_opIoCmd_mode(CMD_HNDL* hdl);


/****************************************************************************/
/*								メインコマンド								*/
/****************************************************************************/

/****************************************************************************/
/* 概要		：未定義コマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_WARNG6			コマンド警告６						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{
	/* 異常終了を返す */
	hNetData->cmd_hdl.cmdInfo.rsltBuf = hNetData->cmd_hdl.cmdInfo.cmdBuf;
	return CMD_WARNG6;
}

/****************************************************************************/
/* 概要		:	未サポートコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	=z CMD_WARNG2			コマンド警告２						*/
/****************************************************************************/
INT32 cmd_NO_SUPP( MLNET_HANDLE *hNetData )
{
	/* 異常終了を返す */
	hNetData->cmd_hdl.cmdInfo.rsltBuf = hNetData->cmd_hdl.cmdInfo.cmdBuf;
	return CMD_WARNG2;
}

/****************************************************************************/
/* 概要		：ＮＯＰコマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_NOP( MLNET_HANDLE *hNetData )
{
	hNetData->cmd_hdl.cmdInfo.rsltBuf.nop.cmd = hNetData->cmd_hdl.cmdInfo.cmdBuf.nop.cmd;
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		：ＰＲＭ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl		コマンド軸情報							*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG1		データ設定警告１	（Ａ．９４Ａ）	*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= CMD_WARNG1			コマンド警告１		（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_PRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR  		cmd;								/* コマンドコード 		*/
	USHORT 		no;  								/* パラメータ番号 		*/
	UCHAR  		size;								/* パラメータサイズ 	*/
	INT32  		ret  = CMD_FUNC_NORMAL_END;
	
	/* ローカル変数の初期化 */
	pCmdHdl = &hNetData->cmd_hdl;
	
	cmd = pCmdHdl->cmdInfo.cmdBuf.prm.cmd;
	no  = pCmdHdl->cmdInfo.cmdBuf.prm.no;
	size = pCmdHdl->cmdInfo.cmdBuf.prm.size;

	/* 実行条件チェック */
	if(  (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
	     && ((pCmdHdl->cmdInfo.rsltBuf.prm.cmd != cmd) 
	     || (pCmdHdl->cmdInfo.rsltBuf.prm.no != no) 
	     || (pCmdHdl->cmdInfo.rsltBuf.prm.size != size)) )
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* レジスタアクセス用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = size;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.prm.data;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.prm.data);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->cmdInfo.RegData);
		}

		/* 解析結果をバッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.prm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.prm.no   = no;
		pCmdHdl->cmdInfo.rsltBuf.prm.size = size;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
		}
	}
	return ret;
}

/****************************************************************************/
/* 概要		：ＰＲＭ＿ＷＲコマンド解析処理									*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl			コマンド軸情報						*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG1		データ設定警告１（Ａ．９４Ａ）		*/
/*			:	= DATA_SET_WARNG2		データ設定警告２（Ａ．９４Ｂ）		*/
/*			:	= DATA_SET_WARNG3		データ設定警告３（Ａ．９４Ｃ）		*/
/*			:	= DATA_SET_WARNG4		データ設定警告４（Ａ．９４Ｄ）		*/
/*			:	= CMD_WARNG1			コマンド警告１	（Ａ．９５Ａ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_PRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = RAM_OFFSET;
	INT32  ret;

	ret = cmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl ,ofst);
	return ret;
}

/****************************************************************************/
/* 概要		：パラメータ書き込み実行処理									*/
/****************************************************************************/
static  INT32 cmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst)
{
	USHORT no = cmdHdl->cmdInfo.cmdBuf.prm.no;			/* パラメータ番号 */
	UCHAR  size = cmdHdl->cmdInfo.cmdBuf.prm.size;		/* パラメータサイズ */
	INT32  ret  = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (cmdHdl->cmdInfo.cmdRdy != FALSE)
		&& cmdChkPrmWr(&cmdHdl->cmdInfo.cmdBuf, &cmdHdl->cmdInfo.rsltBuf))
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* レジスタアクセス用データ設定 */
			cmdHdl->cmdInfo.RegData.AccessSize = size;
			cmdHdl->cmdInfo.RegData.Length = 1;
			cmdHdl->cmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ( (ofst) <<1 );
			cmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			cmdHdl->cmdInfo.RegData.pBuf = (LONG*)cmdHdl->cmdInfo.cmdBuf.prm.data;
			cmdHdl->cmdInfo.RegData.BufferSize = sizeof(cmdHdl->cmdInfo.cmdBuf.prm.data);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(regHdl, REG_CMD_PRMWR, &cmdHdl->cmdInfo.RegData);
		}
		/* 解析結果をバッファへコピー */
		cmdHdl->cmdInfo.rsltBuf.prm.cmd  = cmdHdl->cmdInfo.cmdBuf.prm.cmd;
		cmdHdl->cmdInfo.rsltBuf.prm.no   = no;
		cmdHdl->cmdInfo.rsltBuf.prm.size = size;
		/* ８byte分コピー(先頭アドレスは、奇数アドレスであるため、DWORDの書き込みはできない) */
		UtilSetLLData(cmdHdl->cmdInfo.rsltBuf.prm.data, cmdHdl->cmdInfo.cmdBuf.prm.data);
	}
	else
	{
		if(cmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(regHdl);
		}
	}
	
	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		：パラメータ書き込み実行条件チェック処理						*/
/****************************************************************************/
static INT32 cmdChkPrmWr(CMD_LINK_DATA* cmd_buf, CMD_LINK_DATA* rslt_buf)
{
	UCHAR i, sz;

	if(rslt_buf->prm.cmd != cmd_buf->prm.cmd)	return TRUE;
	if(rslt_buf->prm.no != cmd_buf->prm.no)		return TRUE;
	if(rslt_buf->prm.size != cmd_buf->prm.size)	return TRUE;

	sz = (cmd_buf->prm.size >= 8) ? 8: cmd_buf->prm.size;
	for(i = 0; i < sz; i++)
	{
		if(cmd_buf->prm.data[i] != rslt_buf->prm.data[i])
			return TRUE;
	}

	return FALSE;
}

/****************************************************************************/
/* 概要		:	ＩＤ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= DATA_SET_WARNG2		データ設定警告２（Ａ．９４Ｂ）		*/
/*			:	= DATA_SET_WARNG4		データ設定警告４（Ａ．９４Ｄ）		*/
/****************************************************************************/
INT32 cmd_ID_RD( MLNET_HANDLE *hNetData )
{
	UCHAR		cmd, dev_cod, offset, size;/* <S155> *//* <S**1> */
	LONG		code;/* <S**1> */
	INT32		ret;
	CMD_HNDL	*pCmdHdl;

	/* ローカル変数の初期化 */
	ret	= CMD_FUNC_NORMAL_END;
	
	pCmdHdl = &hNetData->cmd_hdl;
	
	cmd		= pCmdHdl->cmdInfo.cmdBuf.id_rd.cmd;
	dev_cod = pCmdHdl->cmdInfo.cmdBuf.id_rd.dev_cod;
	offset   = pCmdHdl->cmdInfo.cmdBuf.id_rd.offset;
	size    = pCmdHdl->cmdInfo.cmdBuf.id_rd.size;
	
	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->cmdInfo.rsltBuf.id_rd.cmd != cmd) 
		 || (pCmdHdl->cmdInfo.rsltBuf.id_rd.dev_cod != dev_cod)
		  || (pCmdHdl->cmdInfo.rsltBuf.id_rd.offset  != offset)
		   || (pCmdHdl->cmdInfo.rsltBuf.id_rd.size    != size)) )
	{
		if( size <= ID_RD_MAX_SIZE )
		{
			/* レジスタアクセス用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = 1;
			pCmdHdl->cmdInfo.RegData.Length = (LONG)size;
//			pCmdHdl->cmdInfo.RegData.Number = (dev_cod*4) + ofset;/* <S155> */
			code = ( (LONG)(dev_cod & 0xF0) << 1 ) + ( (LONG)(dev_cod & 0x0F) << 3);/* <S155> *//* <S**1> */
			pCmdHdl->cmdInfo.RegData.Number = (code << 1);	/* <S155> *//* <S**1> */
			pCmdHdl->cmdInfo.RegData.offset = (LONG)offset;		/* <S**1> */
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.id_rd.id_info;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.id_rd.id_info);

			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_IDRD, &pCmdHdl->cmdInfo.RegData);
		}
		else
		{
			MlibResetByteMemory( pCmdHdl->cmdInfo.rsltBuf.id_rd.id_info, ID_RD_MAX_SIZE );
			ret = DATA_SET_WARNG4;
		}

		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.id_rd.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.id_rd.dev_cod = dev_cod;
		pCmdHdl->cmdInfo.rsltBuf.id_rd.offset  = offset;
		pCmdHdl->cmdInfo.rsltBuf.id_rd.size    = size;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
		}
	}
	
	return ret;
}

/****************************************************************************/
/* 概要		：ＣＯＮＦＩＧコマンド解析処理									*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl			コマンド軸情報						*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_CONFIG( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	LONG		cmd;
	INT32		result;
	LONG		cmd_switch_time;

	/* ローカル変数の初期化 */
	pCmdHdl			= &hNetData->cmd_hdl;
	cmd 			= pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	result			= CMD_FUNC_NORMAL_END;
	cmd_switch_time = FALSE;

/* 実行条件チェック */
	if(pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd)
	{
		pCmdHdl->cmd_seq = SV_OFF_SEQ;	/* コマンド切り替わりで処理開始 */
		cmd_switch_time = TRUE;
	}
	
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE) || (cmd_switch_time == TRUE) )
	{
		result	= CMD_FUNC_CONTINUE;
		/* CONFIGシーケンス*/
		if(pCmdHdl->cmd_seq == SV_OFF_SEQ)
		{
			if(MCI_SUCCESS == MciSetServoOn(&hNetData->mt_hdl, FALSE, TRUE))
			{
				/* BKオン */
				(hNetData->mt_hdl.svCmd)->CmdSeqBit &= ~(BRK_RELEASE_BIT);
				pCmdHdl->cmd_seq = PRM_RE_CALC_SEQ;
			}
		}
		else if(pCmdHdl->cmd_seq == PRM_RE_CALC_SEQ)
		{
			/* レジスタコマンド処理要求発行API	*/
			if( REG_CONTINUE == RegRequestCommand(&hNetData->reg_hdl, REG_CMD_RECALC, NULL) ) 
			{ /* リセット処理終了 */
				pCmdHdl->cmdInfo.chkOpIoCmd = pCmdHdl->cmdInfo.cmdCdataBuf;		/* ADJ(SUBCODE=10H)のCDATAを取得 */
				pCmdHdl->cmd_seq = MT_ENABLE_SEQ;
			}
		}
		else
		{
//			if(0 >= RegCheckCommandState(&hNetData->reg_hdl)) /* <S0EC> */
			result = RegCheckCommandState(&hNetData->reg_hdl);
			if(result != REG_CONTINUE)
			{
				pCmdHdl->cmd_seq = IDOL_SEQ;
//				result = CMD_FUNC_NORMAL_END; /* <S0EC> */
			}					
		}
	
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.nop.cmd = cmd;
	}
	
	return result;
}

/****************************************************************************/
/* 概要		：ＡＬＭ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl			コマンド軸情報						*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd, alm_rd_mod, alm_idx;
	UCHAR		*alm_data;
	INT32 		ret;
	
	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;

	cmd			= pCmdHdl->cmdInfo.cmdBuf.alm.cmd;		/* コマンドコード 			*/
	alm_rd_mod	= pCmdHdl->cmdInfo.cmdBuf.alm.mode;     /* アラーム読み出しモード	*/
	alm_idx		= pCmdHdl->cmdInfo.cmdBuf.alm.data[0];  /* アラームインデックス番号 */
    alm_data	= &pCmdHdl->cmdInfo.rsltBuf.alm.data[0];/* アラームデータ			*/

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE)
		|| (pCmdHdl->cmdInfo.rsltBuf.alm.cmd != cmd) 
		|| (pCmdHdl->cmdInfo.rsltBuf.alm.mode != alm_rd_mod) 
	    || ((alm_rd_mod == ALM_RD_MOD2) && (*alm_data != alm_idx))
	    || ((alm_rd_mod == ALM_RD_MOD3) && (*alm_data != alm_idx)) )
	{
		ret = cmdAlmRdProc(&hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data);

	/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.alm.mode = alm_rd_mod;
	}

	return ret;
}


/****************************************************************************/
/* 概要		：アラーム読み出しＡＰＩ実行処理								*/
/****************************************************************************/
static INT32	cmdAlmRdProc(ALM_HNDL* alm_hdl, UCHAR alm_rd_mod, UCHAR idx, UCHAR* alm_data)
{
	INT32 ret;

	switch(alm_rd_mod)
	{
	case ALM_RD_MOD0:	/* 現在発生アラームリード */
		ret = AlmGetCrtBufAll(alm_hdl, alm_data);
		break;
	case ALM_RD_MOD1:	/* アラーム履歴リード */
		ret = AlmGetHistoryAll(alm_hdl, alm_data);
		break;
	case ALM_RD_MOD2:	/* 現在発生アラームバッファ詳細情報リード */
		*alm_data = idx;
		*(alm_data+7) = 0;
		*(alm_data+8) = 0;
		*(alm_data+9) = 0;
		alm_data++;
		ret = AlmGetCrtBuf(alm_hdl, idx, alm_data);
		break;
	case ALM_RD_MOD3:	/* 過去発生アラーム履歴詳細情報リード */
		*alm_data = idx;
		*(alm_data+7) = 0;
		*(alm_data+8) = 0;
		*(alm_data+9) = 0;
		alm_data++;
		ret = AlmGetHistory(alm_hdl, idx, alm_data);
		break;
	default:
		ret = DATA_SET_WARNG2;
		break;
	}
	return ret;
}

/****************************************************************************/
/* 概要		：ＡＬＭ＿ＣＬＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl			コマンド軸情報						*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ALM_CLR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_clr_mod;
	USHORT 		option;								/* オプション 			*/
	INT32		ret;


	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;
	
	cmd = pCmdHdl->cmdInfo.cmdBuf.alm.cmd;			/* コマンドコード		*/
	alm_clr_mod = pCmdHdl->cmdInfo.cmdBuf.alm.mode;	/* アラームクリアモード */
	ret = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
	   &&((pCmdHdl->cmdInfo.rsltBuf.alm.cmd  != cmd)
	    || (pCmdHdl->cmdInfo.rsltBuf.alm.mode != alm_clr_mod)) )
	{
		if( alm_clr_mod == ALM_CLR_MOD0 )
		{
			ret = AlmSetClearReq( &hNetData->alm_hdl, TRUE );
		}
		else if( alm_clr_mod == ALM_CLR_MOD1 )
		{
			AlmRequestAlarmHistoryClear( &hNetData->alm_hdl );
			ret = CMD_FUNC_CONTINUE;
		}
		else
		{
			ret = DATA_SET_WARNG2;
		}
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.alm.mode = alm_clr_mod;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			if( alm_clr_mod == ALM_CLR_MOD0 )
			{
				ret = AlmSetClearReq( &hNetData->alm_hdl, FALSE );
			}
			else /* if( alm_clr_mod == ALM_CLR_MOD1 )	*/
			{
				ret = AlmGetAlarmHistoryClearStatus( &hNetData->alm_hdl );
			}
		}
	}

	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(&hNetData->mt_hdl, option);
	}
	/*---------------------------------------------------------------*/

	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		:	ＳＹＮＣ＿ＳＥＴコマンド解析処理							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_SYNC_FAULT			Ｍ－Ⅱ同期失敗（Ａ．Ｅ５１）	*/
/*			:	= CMD_COM_ERR				Ｍ－Ⅱ通信異常（Ａ．Ｅ６０）	*/
/*			:   = CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 cmd_SYNC_SET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	MT_HNDL		*pMtHdl;
	UCHAR		cmd;
	INT32		ret, result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	result		= CMD_FUNC_CONTINUE;

	cmd	= pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd) && (pComHdl->Phase == 2) )
	{
		pCmdHdl->cmd_seq = SV_OFF_SEQ;	/* コマンド切替り && Ph=2で処理開始 */
	}

	switch(pCmdHdl->cmd_seq)
	{
	case SV_OFF_SEQ :		/* サーボオフ指令 */
		if( MciSetServoOn( pMtHdl, FALSE, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = MT_DISABLE_SEQ;
		}
		break;
	case MT_DISABLE_SEQ :	/* ブレーキオン・モーション無効 */
		if( MciEnableState( pMtHdl, FALSE ) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = REQ_PHSUP_SEQ;
		}
		break;
	case REQ_PHSUP_SEQ :	/* フェーズアップ要求 */
		ret = ComReqPhaseUp( pComHdl, 3, 
				pComHdl->NetPrm.tType, pComHdl->NetPrm.aplCycle );
		pCmdHdl->cmd_seq = (ret == COM_FUNC_NORMAL_END) ? 
							PHASE_CHK_SEQ : FAIL_END_SEQ;
		break;
	case PHASE_CHK_SEQ :	/* フェーズアップチェック */
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret == CMD_FUNC_NORMAL_END )
		{ /* フェーズ移行処理が正常終了したら */
			if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
			{ /* モーション有効 */
				pCmdHdl->cmd_seq = IDOL_SEQ;
				result = CMD_FUNC_NORMAL_END;
			}
		}
		else if(ret != CMD_FUNC_CONTINUE)
		{ /* フェーズ移行処理が異常終了したら */
			pCmdHdl->cmd_seq = FAIL_END_SEQ;
		}
		break;
	case FAIL_END_SEQ :	/* シーケンス失敗 */
		if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			result = CMD_SYNC_FAULT;
		}
		break;
	default:
		result = CMD_FUNC_NORMAL_END;
		break;
	}

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.nop.cmd = cmd;

	return result;
}


/****************************************************************************/
/* 概要		:	ＣＯＮＮＥＣＴコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/*			:   = CMD_TCYC_ERROR			伝送周期設定異常（Ａ．Ｅ４０）	*/
/*			:	= CMD_SYNC_FAULT			Ｍ－Ⅱ同期失敗（Ａ．Ｅ５１）	*/
/****************************************************************************/
INT32 cmd_CONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	MT_HNDL		*pMtHdl;
	CYC_CHG_PRM	*cycPrm;
	LONG		com_tim, dtmod, reqphs;
	INT32		ret, rc;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	com_tim		= pCmdHdl->cmdInfo.cmdBuf.connect.com_time;		/* 通信周期 */
	rc			= CMD_FUNC_CONTINUE;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy) && (pComHdl->Phase == 1) )
	{ /* IDOL状態 && Phase=1 で処理開始 */
		pCmdHdl->cmd_seq = REQ_PHSUP_SEQ;
	}

	switch(pCmdHdl->cmd_seq)
	{
	case REQ_PHSUP_SEQ:	/* フェーズアップ要求 */
		ret = cmdChkConnectCmd( hNetData, &reqphs, &dtmod, &com_tim);
		if( ret == CMD_FUNC_NORMAL_END )
		{ /* コマンド正常時 */
			if( COM_FUNC_NORMAL_END == 
					ComReqPhaseUp(pComHdl, reqphs, dtmod, com_tim) )
			{ /* 正常なら次のシーケンスへ、失敗時はリトライ */
				pCmdHdl->cmd_seq = PHASE_CHK_SEQ;
			}
		}
		else if( ret != CMD_FUNC_CONTINUE )
		{ /* 異常時 */
			rc = ret;
			pCmdHdl->cmd_seq = IDOL_SEQ;
		}
		break;
	case PHASE_CHK_SEQ:	/* フェーズアップチェック */
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret == COM_FUNC_NORMAL_END )
		{
			pCmdHdl->cmd_seq = PRM_RE_CALC_SEQ;
		}
		else if( ret != COM_FUNC_CONTINUE )
		{ /* 異常終了 */
			if( pComHdl->Phase <= 1 )
			{ /* フェーズ１ならエラーコードをセット */
				rc = ret;
				pCmdHdl->cmd_seq = IDOL_SEQ;
			}
			else
			{
				pCmdHdl->cmd_seq = FAIL_END_SEQ;
			}
		}
		break;
	case PRM_RE_CALC_SEQ:	/* パラメータ再計算実行 */
		cycPrm = (CYC_CHG_PRM*)pCmdHdl->cmdInfo.RegWkBuf;
		cycPrm->dlyTime = 0;
		cycPrm->scanCyc = (USHORT)CFG_TIO_BASE_TIME;
		cycPrm->intCyc = pComHdl->NetPrm.tCycle;
		cycPrm->aplCyc = (USHORT)pComHdl->NetPrm.aplCycle;
		pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.RegWkBuf;
		
			/* レジスタコマンド処理要求発行API	*/
		RegRequestCommand(&hNetData->reg_hdl, REG_CMD_CHGCYC, &pCmdHdl->cmdInfo.RegData);
		pCmdHdl->cmd_seq = PRM_ACCESS_SEQ;
		break;
	case PRM_ACCESS_SEQ:	/* パラメータ再計算実行完了まち */
		if(0 >= RegCheckCommandState(&hNetData->reg_hdl))
		{	pCmdHdl->cmd_seq = MT_ENABLE_SEQ;}
		break;
	case MT_ENABLE_SEQ:		/* モーション有効 */
		if( MciEnableState( pMtHdl, TRUE ) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			rc = CMD_FUNC_NORMAL_END;
		}
		break;
	case FAIL_END_SEQ:		/* シーケンス失敗 */
		if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			rc = CMD_SYNC_FAULT;
		}
		break;
	default:
		rc = CMD_FUNC_NORMAL_END;
		break;
	}
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.connect.cmd =
					pCmdHdl->cmdInfo.cmdBuf.connect.cmd;
	pCmdHdl->cmdInfo.rsltBuf.connect.ver =
					pCmdHdl->cmdInfo.cmdBuf.connect.ver;
	pCmdHdl->cmdInfo.rsltBuf.connect.com_mod =
					pCmdHdl->cmdInfo.cmdBuf.connect.com_mod;
	pCmdHdl->cmdInfo.rsltBuf.connect.com_time =
					pCmdHdl->cmdInfo.cmdBuf.connect.com_time;

	return rc;
}


/****************************************************************************/
/* 概要		：ＣＯＮＮＥＣＴコマンドデータチェック処理						*/
/****************************************************************************/
static INT32 cmdChkConnectCmd(MLNET_HANDLE *hNetData, LONG* reqphs, LONG* dtmod, LONG* com_tim)
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	UCHAR		ver, com_mod;
	ULONG		t_spd, t_byte;/* <S052>:USHORT -> ULONG */
	BOOL		sv_on;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;
	pComHdl	= &hNetData->com_hdl;
	ver     = pCmdHdl->cmdInfo.cmdBuf.connect.ver;		/* バージョン		*/
	com_mod = pCmdHdl->cmdInfo.cmdBuf.connect.com_mod;	/* 通信モード		*/
	t_spd	= (ULONG)pComHdl->NetPrm.tSpeed;			/* 通信速度 4/10	*/
	t_byte	= (ULONG)pComHdl->NetPrm.dataSize;			/* 伝送バイト数		*/
	sv_on	= hNetData->mt_hdl.svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK;/* サーボオン状態 0:OFF,1:ON */
	ret		= CMD_FUNC_NORMAL_END;

/* 実行条件チェック */
	if(sv_on != 0)
	{
		return  CMD_WARNG1;
	}

	if(ver == 0x10)
	{/* MECHATROLINK-I	*/
		if(t_spd != 4)
		{/* 通信速度不整合	*/
			return  DATA_SET_WARNG2;
		}
		if(*com_tim % 2)
		{/* 通信周期異常	*/
			return  DATA_SET_WARNG2;
		}
		if((*com_tim < 2) || (*com_tim > 32))
		{/* 通信周期異常	*/
			return  DATA_SET_WARNG2;
		}
		/* 通信周期を伝送周期単位[2ms単位]に変換 */
		*com_tim = *com_tim >> 1;
		/* サブコマンド使用／未使用設定 */
		pCmdHdl->useExCmd = FALSE;
		/* 要求フェーズ設定 */
		if((com_mod & 0x03) == 0x2)
		{
			*reqphs = 3;
		}
		else
		{
			*reqphs = 2;
		
		}
	}
	else if(ver == 0x21)
	{/* MECHATROLINK-II	*/
		if(t_spd != 10)
		{/* 通信速度不整合	*/
			return  DATA_SET_WARNG2;
		}
		if((com_mod & 0x80) && (t_byte == 17))
		{/* SubCmd,伝送ﾊﾞｲﾄ数 不整合	*/
			return  DATA_SET_WARNG2;
		}
		if((*com_tim < 1) || (*com_tim > 32))
		{/* 通信周期異常	*/
			return  DATA_SET_WARNG2;
		}
		/* サブコマンド使用／未使用設定 */
		pCmdHdl->useExCmd = (com_mod >> 7) & 0x1;
		/* 要求フェーズ設定 */
		if((com_mod & 0x02) == 0x2)
		{
			*reqphs = 3;
		}
		else
		{
			*reqphs = 2;
		}
	}
	else
	{
		return  DATA_SET_WARNG2;
	}

	/* 伝送モード設定 */
	com_mod = (com_mod >> 2) & 0x3;
	if(com_mod == 0x1)
	{/* 01b: 連送モード */
		if(*com_tim < 2)
		{
		 	return DATA_SET_WARNG2;	/* 2倍未満ならNG */
		}
		*dtmod = CONSECUTIVE_TRANSFER;
	}
	else if(com_mod == 0x2)
	{/* 10b:複合モード */
		return  DATA_SET_WARNG2;
	}
	else
	{/* 00b,11b: 単送モード */
		*dtmod = SINGLE_TRANSFER;
	}

	return ret;
}

/****************************************************************************/
/* 概要		：ＤＩＳＣＯＮＮＥＣＴコマンド解析処理							*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl			コマンド軸情報						*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_DISCONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	UCHAR		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;
	pComHdl	= &hNetData->com_hdl;
	cmd     = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;		/* コマンドコード */
	result	= CMD_FUNC_CONTINUE;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd)
		&& ((pComHdl->Phase > 1) || (pComHdl->reqPhase > 1)) )
	{/* コマンド切り替わり && (現在Phase2以上 or PhaseUP中) */
		pCmdHdl->cmd_seq = REQ_PHSDOWN_SEQ;
	}
	
	/* DISCONNECT処理実行 */
	result = CmdRunDisConnect(hNetData);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.nop.cmd = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;

	return result;
}

/****************************************************************************/
/* 概要		：ＰＰＲＭ＿ＷＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl			コマンド軸情報						*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG1			データ設定警告１（Ａ．９４Ａ）	*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= DATA_SET_WARNG3			データ設定警告３（Ａ．９４Ｃ）	*/
/*			:	= DATA_SET_WARNG4			データ設定警告４（Ａ．９４Ｄ）	*/
/*			:	= CMD_WARNG1				コマンド警告１	（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_PPRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = EEPROM_OFFSET;
	INT32  ret;

	ret = cmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl, ofst);
	return ret;
}

/****************************************************************************/
/* 概要		：ＰＯＳ＿ＳＥＴコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_POS_SET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	UCHAR 		ps_cmd;									/* 座標設定モード */
	BOOL 		refe;									/* レファレンス点 */
	LONG		pos_data;								/* 設定位置 */
	INT32		ret;
	USHORT		option;									/* オプション */

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	ps_cmd = pCmdHdl->cmdInfo.cmdBuf.pos_set.ps_cmd;
	refe   = (BOOL)(ps_cmd & 0x80) >> 7;
	pos_data = UtilGetDWData(pCmdHdl->cmdInfo.cmdBuf.pos_set.pos);	/* Longﾃﾞｰﾀにする。	*/

	if((ps_cmd & 0xf) == 0x03)
	{/* 座標系設定=APOSの場合 */
		ret = MciSetCordinatePos(pMtHdl, pos_data, refe);		/* 座標系設定 */
	}
	else
	{/* 座標系設定!=APOSの場合 */
		ret = DATA_SET_WARNG2;
	}

	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.pos_set.cmd    = pCmdHdl->cmdInfo.cmdBuf.pos_set.cmd;
	pCmdHdl->cmdInfo.rsltBuf.pos_set.ps_cmd = ps_cmd;
	UtilSetDWData(pCmdHdl->cmdInfo.rsltBuf.pos_set.pos, pos_data);	/* 奇数アドレスにコピー	*/

	return ret;
}

/****************************************************************************/
/* 概要		：ＢＲＫ＿ＯＮコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_BRK_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT		option;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* 実行条件チェック */
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd) 
	{/* ＢＫ指令オン */
		pMtHdl->svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
	}
	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		：ＢＲＫ＿ＯＦＦコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_BRK_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT 		option;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 実行条件チェック */
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd) 
	{/* ＢＫ指令オフ */
		pMtHdl->svCmd->CmdSeqBit |= BRK_RELEASE_BIT;
	}
	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		:	ＳＥＮＳ＿ＯＮコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= CMD_WARNG1			コマンド警告１						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SENS_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32  		ret;
	USHORT 		option;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;


	/* センサオン指令セット */
	ret = MciSetSensOn(pMtHdl, TRUE);

	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return ret;
}

/****************************************************************************/
/* 概要		：ＳＥＮＳ＿ＯＦＦコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SENS_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT	 	option;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	ret			= CMD_FUNC_NORMAL_END;

	/* センサオフ指令セット */
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd)
	{
		ret = MciSetSensOn(pMtHdl, FALSE);
	}
	
	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return ret;
}

/****************************************************************************/
/* 概要		：ＨＯＬＤコマンド解析処理								(25H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_HOLD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MCI_CMD 	cmd;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	ret			= CMD_FUNC_NORMAL_END;


	/* 各データを抽出 */
	cmd.HoldMod   = (UCHAR)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* Holdモードセット */
	cmd.TrqLimitP = CMD_MAXTRQ;								/* TRQ LIMITセット  */
	cmd.TrqLimitN = CMD_MAXTRQ;								/* TRQ LIMITセット  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Optionセット     */

	/* ＨＯＬＤコマンドＡＰＩ関数実行 */
	MciSetHoldCmd(&hNetData->mt_hdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd       = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		：ＬＴＭＯＤ＿ＯＮコマンド解析処理								*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl		コマンド軸情報							*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= LTMOD_ON_CMD_WARNG4	コマンド警告４（Ａ．９５Ｄ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_LTMOD_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG  lt_sgn;
	UCHAR lt_mod;
	INT32  ret;
	USHORT option;										/* オプション		 */

	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	lt_sgn = (LONG)pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;
	lt_mod = (UCHAR)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;
	
	/* ラッチ要求 */
	ret = MciRequestMdlLatch(pMtHdl, lt_mod, lt_sgn);

	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return ret;
}

/****************************************************************************/
/* 概要		：ＬＴＭＯＤ＿ＯＦＦコマンド解析処理							*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl		コマンド軸情報							*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= LTMOD_ON_CMD_WARNG4	コマンド警告４（Ａ．９５Ｄ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_LTMOD_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32  ret;
	USHORT option;											/* オプション 	*/

	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	/* ラッチキャンセル */
	ret = MciCancelLatch(pMtHdl, MODAL_LTMD);
	
	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	
	return ret;
}

/****************************************************************************/
/* 概要		：ＳＭＯＮコマンド解析処理										*/
/****************************************************************************/
/* 引数		;	CMD_HNDL* hdl		コマンド軸情報							*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT 		option;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{
		/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		：ＳＶ＿ＯＮコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SV_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT 		option;
	BOOL		reset;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	ret		= CMD_FUNC_NORMAL_END;
	

	/* サーボオン指令セット */
	reset = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	ret = MciSetServoOn(pMtHdl, TRUE, reset);
	
	if(ret != MCI_CNDTN_ERR)
	{	/* 異常終了でなければ、オプション指令セット */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return ret;
}

/****************************************************************************/
/* 概要		：ＳＶ＿ＯＦＦコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SV_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	USHORT		option;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* サーボオフ指令セット */
	ret = MciSetServoOn(pMtHdl, FALSE, TRUE);

	/*---------------------------------------------------------------*/
	/*Optionフィールド有効(YEA依頼の対応)                            */
	/*---------------------------------------------------------------*/
	if((pCmdHdl->cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
	{/* オプション指令抽出 */
		option = pCmdHdl->cmdInfo.cmdBuf.mt.option;
		MciSetOptionCmd(pMtHdl, option);
	}
	/*---------------------------------------------------------------*/

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return ret;
}


/****************************************************************************/
/* 概要		：ＩＮＴＥＲＰＯＬＡＴＥコマンド解析処理				(34H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	IPOLATE_CMD_WARNING1						*/
/*				データ異常	:	IPOLATE_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_INTERPOLATE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;		/* TPOSセット       */
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* VFF/速度指令 [ref/s]/[OS/2^24] */
	cmd.Trq       = (LONG)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = cmd.Trq;				/* TLIMセット		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Optionセット     */

	/* 通信ワーニング時の補正処理 */
	if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)
		&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
	{/* 通信ワーニング && 現在モーション=INTERPOLATE */
		cmd.TPos += pMtHdl->outData.DposIn;
	}

	/* ＩＮＴＥＲＰＯＬＡＴＥコマンドＡＰＩ関数実行 */
	result = MciSetInterpolateCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＰＯＳＩＮＧコマンド解析処理							(35H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	POSING_CMD_WARNING1							*/
/*				データ異常	:	POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* 各データを抽出 */
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* TPOSセット       */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TSPDセット       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;			/* TLIMセット		*/
	cmd.Trq       = 0;									/* TFFセット		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Optionセット     */
	cmd.RstReq    = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	cmd.AccRate   = (LONG)buf;							/* ACC_TIMEセット	*/
	cmd.EnableRate = TRUE;								/* ACC_TIME有効		*/

	/* ＰＯＳＩＮＧコマンドＡＰＩ関数実行 */
	result = MciSetPosingCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＦＥＥＤコマンド解析処理								(36H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	FEED_CMD_WARNING1							*/
/*				データ異常	:	FEED_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_FEED( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* 各データを抽出 */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TSPDセット       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;			/* TLIMセット		*/
	cmd.Trq       = 0;									/* TFFセット		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Optionセット     */
	cmd.RstReq    = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	cmd.AccRate   = (LONG)buf;							/* ACC_TIMEセット */
	cmd.EnableRate = FALSE;								/* ACC_TIME有効 */

	/* ＦＥＥＤコマンドＡＰＩ関数実行 */
	result = MciSetFeedCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＬＡＴＣＨコマンド解析処理							(38H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	LATCH_CMD_WARNING1							*/
/*				ﾃﾞｰﾀ異常	:	LATCH_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_LATCH( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.LatchSgnl = pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;/* LT_SGNセット	*/
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;		/* TPOSセット 	*/
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* VFF/速度指令 [ref/s]/[OS/2^24] */
	cmd.Trq       = (LONG)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = cmd.Trq;				/* TLIMセット	*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Optionセット */

	/* 通信ワーニング時の補正処理 */
	if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)
		&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
	{/* 通信ワーニング && 現在モーション=INTERPOLATE */
		cmd.TPos += pMtHdl->outData.DposIn;
	}

	/* ＬＡＴＣＨコマンドＡＰＩ関数実行 */
	result = MciSetLatchCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＥＸ＿ＰＯＳＩＮＧコマンド解析処理					(39H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	EX_POSING_CMD_WARNING1						*/
/*				ﾃﾞｰﾀ異常	:	EX_POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_EX_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.LatchSgnl = pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;/* LT_SGNセット */
	cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* TPOSセット       */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TSPDセット       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;			/* TLIMセット		*/
	cmd.Trq       = 0;									/* TFFセット		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Optionセット     */
	cmd.RstReq    = (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd);
	cmd.AccRate   = (LONG)buf;							/* ACC_TIMEセット 	*/
	cmd.EnableRate = TRUE;								/* ACC_TIME有効 	*/

	/* ＥＸ＿ＰＯＳＩＮＧコマンドＡＰＩ関数実行 */
	result = MciSetExPosingCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＺＲＥＴコマンド解析処理								(3AH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	ZRET_CMD_WARNING1							*/
/*				ﾃﾞｰﾀ異常	:	ZRET_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ZRET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	USHORT		buf;
	INT32		result;
	
	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.LatchSgnl = pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn & 0x3;/* LT_SGNセット     */
	cmd.TSpeed    = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* TSPDセット       */
	buf           = (USHORT)UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.mt.iocmd);
	cmd.TrqLimitP = cmd.TrqLimitN = (ULONG)buf;				/* TLIMセット		*/
	cmd.Trq       = 0;										/* TFFセット		*/
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Optionセット     */
	cmd.RstReq    = pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	cmd.AccRate   = (LONG)buf;								/* ACC_TIMEセット */
	cmd.EnableRate = FALSE;									/* ACC_TIME有効 */

	/* ＺＲＥＴコマンドＡＰＩ関数実行 */
	result = MciSetHomingCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＶＥＬＣＴＲＬコマンド解析処理						(3CH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	VELCTRL_CMD_WARNING1						*/
/*				ﾃﾞｰﾀ異常	:	VELCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_VELCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;
	
	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* VFF/速度指令 [ref/s]/[OS/2^30] */
	cmd.TrqLimitP = (ULONG)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;			/* TRQ LIMITセット  */
	cmd.TrqLimitN = (ULONG)(pCmdHdl->cmdInfo.cmdBuf.mt.pos1 >> 16);	/* TRQ LIMITセット  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Optionセット     */
	cmd.Trq       = (LONG)cmd.TrqLimitP;				/* TFFセット */

	/* ＶＥＬＣＴＲＬコマンドＡＰＩ関数実行 */
	result = MciSetVelCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＴＲＱＣＴＲＬコマンド解析処理						(3DH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	TRQCTRL_CMD_WARNING1						*/
/*				ﾃﾞｰﾀ異常	:	TRQCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_TRQCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;
	
	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	/* 各データを抽出 */
	cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* VLIMセット       */
	cmd.Trq       = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* TQREFセット      */
	cmd.TrqLimitP = CMD_MAXTRQ;							/* TRQ LIMITセット  */
	cmd.TrqLimitN = CMD_MAXTRQ;							/* TRQ LIMITセット  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;	/* Optionセット     */

	/* ＴＲＱＣＴＲＬコマンドＡＰＩ関数実行 */
	result = MciSetTrqCmd(pMtHdl, &cmd);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;

	return result;
}

/****************************************************************************/
/* 概要		：ＳＶＣＴＲＬコマンド解析処理							(3FH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	：	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*																			*/
/*	SUBCTRL : d0-d1 L_SGN	ラッチ信号選択	0:C相, 1:EXT1, 2:EXT2, 3:EXT3	*/
/*			  d2    SET_L	ラッチ指定										*/
/*			  d3    RESERVE													*/
/*			  d4-d6 MOTION	モーション選択	0:HOLD, 1:INTERPORATE			*/
/*											2,FEED, 3:POSING				*/
/*			  d7    RESERVE													*/
/*																			*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_SVCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG		subctrl;	/* サブコントロール */
	CHAR		selmotion;	/* モーション選択   */
	UCHAR		sq_cmd;		/* シーケンス信号   */
	UCHAR		last_sq_cmd;
	INT32		result;
	
	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	result 		= CMD_FUNC_NORMAL_END;
	
	/* 各データを抽出 */
	subctrl = (LONG)pCmdHdl->cmdInfo.cmdBuf.mt.lt_sgn;
	selmotion = (subctrl >> 4) & 0x7;
	sq_cmd = pCmdHdl->cmdInfo.cmdBuf.mt.iocmd[0];

/*--------------------------------------------------------------------------*/
/*	モーション信号処理				               		          			*/
/*--------------------------------------------------------------------------*/
	result = cmdSvCtrlMotionSelectProc(hNetData, selmotion);

/*--------------------------------------------------------------------------*/
/*	ラッチ信号処理				                             				*/
/*--------------------------------------------------------------------------*/
	if(subctrl & 0x04)
	{/*	ラッチリクエスト	*/
		MciRequestMtLatch(pMtHdl, MOTION_MDL_LTMD, subctrl & 0x3);
	}
	else
	{/* ラッチキャンセル	*/
		MciCancelLatch(pMtHdl, MOTION_MDL_LTMD);
	}
	
/*--------------------------------------------------------------------------*/
/*	シーケンス信号処理			                             				*/
/*--------------------------------------------------------------------------*/
	if(pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd)
	{
		last_sq_cmd = 0;
	}
	else
	{/* 	*/
		last_sq_cmd = pCmdHdl->cmdInfo.rsltBuf.mt.iocmd[0];
	}
	
	/*----	ブレーキオン信号	----------------*/
	if( (pCmdHdl->cmdInfo.rsltBuf.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.mt.cmd)
			||((last_sq_cmd & SVCTRL_BRK_SEL) != (sq_cmd & SVCTRL_BRK_SEL)))/* 実行条件チェック */
	{
		if(sq_cmd & SVCTRL_BRK_SEL)
		{/* ＢＫ指令オン */
			pMtHdl->svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
		}
		else
		{/* ＢＫ指令オフ */
			pMtHdl->svCmd->CmdSeqBit |= BRK_RELEASE_BIT;
		}
	}
	
	/*----	センサオン信号	--------------------*/
	MciSetSensOn(pMtHdl, sq_cmd & SVCTRL_SEN_SEL);

	/*----	アラームクリア信号	----------------*/
	if(!(last_sq_cmd & SVCTRL_ACLR_SEL))
	{
		AlmSetClearReq(&hNetData->alm_hdl, (sq_cmd & SVCTRL_ACLR_SEL));
	}

	/*----	サーボオン信号	--------------------*/
	if(!(sq_cmd & SVCTRL_SON_SEL))
	{/* サーボOFF */	
		MciSetServoOn(pMtHdl, FALSE, TRUE);
	}
	else if(!(last_sq_cmd & SVCTRL_SON_SEL))
	{/* 立ち上がりエッジでサーボON */
		if(MciSetServoOn(pMtHdl, TRUE, TRUE) != MCI_EMGBB_ERR)
		{
			sq_cmd &= (~SVCTRL_SON_SEL);	/* Safety Stop以外は、常にエッジにする */
		}
	}

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.mt.cmd;
	pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 = pCmdHdl->cmdInfo.cmdBuf.mt.sel_mon12;
	pCmdHdl->cmdInfo.rsltBuf.mt.iocmd[0] = sq_cmd;

	return result;
}

/****************************************************************************/
/*						モーション信号処理									*/
/****************************************************************************/
static INT32 cmdSvCtrlMotionSelectProc( MLNET_HANDLE *hNetData, CHAR sel_mt)
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		rc;

	/* 各処理共通データ設定 */
	cmd.Trq       = 0;									/* TFFセット */
	cmd.TrqLimitP = CMD_MAXTRQ;							/* TRQ LIMITセット  */
	cmd.TrqLimitN = CMD_MAXTRQ;							/* TRQ LIMITセット  */
	cmd.Option    = pCmdHdl->cmdInfo.cmdBuf.mt.option;		/* Optionセット     */
	cmd.EnableRate = FALSE;								/* ACC_TIME無効 */

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	
	switch (sel_mt)
	{/* サーボオン状態のチェックは各APIで実施 */
	/*----	HOLD	--------------------------------------------------------*/
		case SVCTRL_HOLD :
			cmd.HoldMod = (UCHAR)pCmdHdl->cmdInfo.cmdBuf.mt.pos1;/* Holdモード       */
			rc = MciSetHoldCmd(pMtHdl, &cmd);
			break;
	
	/*----	INTERPOLATE	----------------------------------------------------*/
		case SVCTRL_INTERPOLATE :
			if(hNetData->com_hdl.Phase != 3)					/* フェーズチェック 				*/
			{
				return CMD_WARNG1;
			}
			cmd.TPos      = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;	/* TPOSセット  						*/
			cmd.Velocity  = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;	/* VFF/速度指令 [ref/s]/[OS/2^24]	*/
			if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)/* 通信ワーニング時の補正処理 		*/
					&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
			{/* 通信ワーニング && 現在モーション=INTERPOLATE */
				cmd.TPos += pMtHdl->outData.DposIn;
			}
			rc = MciSetInterpolateCmd(pMtHdl, &cmd);
			break;
		
	/*----	FEED	--------------------------------------------------------*/
		case SVCTRL_FEED :
			cmd.TSpeed   = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* TSPDセット       */
			rc = MciSetFeedCmd(pMtHdl, &cmd);
			break;
			
	/*----	POSING	--------------------------------------------------------*/
		case SVCTRL_POSING :
			cmd.TPos     = pCmdHdl->cmdInfo.cmdBuf.mt.pos1;		/* TPOSセット       */
			cmd.TSpeed   = pCmdHdl->cmdInfo.cmdBuf.mt.pos2;		/* TSPDセット       */
			rc = MciSetPosingCmd(pMtHdl, &cmd);
			break;
			
	/*----	default	--------------------------------------------------------*/
		default :
			rc = DATA_SET_WARNG2;
			break;
	}
	
	if(rc != MCI_CNDTN_ERR)
	{
		return rc;	/* 条件異常はマスク、それ以外はエラー */
	}
	
	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* 概要		：ＡＤＪコマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG1		データ設定警告１	（Ａ．９４Ａ）	*/
/*			:	= DATA_SET_WARNG2		データ設定警告２	（Ａ．９４Ｂ）	*/
/*			:	= DATA_SET_WARNG3		データ設定警告３	（Ａ．９４Ｃ）	*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= CMD_WARNG1			コマンド警告１		（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_ADJ( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	REG_HNDL	*pRegHdl;
	INT32  		ret;
	UCHAR		sub_code;
	
	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pRegHdl		= &hNetData->reg_hdl;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj.subcode;
	
	switch(sub_code)
	{
	case 0x00:
		ret = cmdADJ_sigma2_mode(pCmdHdl, pRegHdl);
		break;
	case 0x01:
		ret = cmdADJ_normal_mode(pCmdHdl, pRegHdl);
		break;
	case 0x0E:
		ret = cmdADJ_tester_mode(pCmdHdl);
		break;
	case 0x10:
		ret = cmdADJ_opIoCmd_mode(pCmdHdl); /* OPTION/IO_MONフィールド使用可能コマンド拡張モード */
		break;
	default:
		ret = DATA_SET_WARNG2;
		break;						   /*<V624>Add*/
	}

	return ret;
}

/****************************************************************************/
/* 概要		：ＡＤＪ（OPTION/IO_MONフィールド使用可能コマンド拡張モード)    */
/****************************************************************************/
static INT32 cmdADJ_opIoCmd_mode( CMD_HNDL *pCmdHdl )
{
	UCHAR  cmd;
	UCHAR  sub_code;
	USHORT ccmd;
	USHORT cdata;
	INT32  ret;

	/* ローカル変数の初期化 */
	cmd      = pCmdHdl->cmdInfo.cmdBuf.adj.cmd;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj.subcode;
	ccmd     = pCmdHdl->cmdInfo.cmdBuf.adj.ccmd;
	cdata    = pCmdHdl->cmdInfo.cmdBuf.adj.caddress;	/* CDATA指令抽出 */
	ret  = CMD_FUNC_NORMAL_END;                    		/* 正常終了		 */


	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != TRUE)                     /* CMDRDY = FALSE */
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.cmd != cmd)          /* 前回受信CMDコード != 今回受信CMDコード */
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.subcode != sub_code) /* 前回受信SUB_CODE  != 今回受信SUB_CODE 	*/
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.ccmd != ccmd)        /* 前回受信CCMD      != 今回受信CCMD		*/
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.caddress != cdata) ) /* 前回受信CDATA     != 今回受信CDATA 	*/
	{
	    /* 解析結果をバッファへコピー */
	    pCmdHdl->cmdInfo.rsltBuf.adj.cmd      = cmd;
	    pCmdHdl->cmdInfo.rsltBuf.adj.subcode  = sub_code;
	    pCmdHdl->cmdInfo.rsltBuf.adj.ccmd     = ccmd;

	    switch(ccmd)
	    {
	    case 0x0000:                                      /* データ参照 */
			pCmdHdl->cmdInfo.rsltBuf.adj.caddress = pCmdHdl->cmdInfo.chkOpIoCmd;
			return ret;
//			break;<S0C7>
	    case 0x0001:                                      /* データ設定 */
			if(cdata & 0xFFFE)
			{
		    	ret = DATA_SET_WARNG2;
			}
			else
			{
		    	pCmdHdl->cmdInfo.cmdCdataBuf = cdata;         /* cdata指令をバッファへコピー */
			}
			break;
	    default:
			ret = DATA_SET_WARNG2;
			break;
	    }
	    /* 解析結果をバッファへコピー */
	    pCmdHdl->cmdInfo.rsltBuf.adj.caddress = cdata;
	}
	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		：ＡＤＪ（Σ－Ⅱ互換モード）									*/
/****************************************************************************/
static INT32 cmdADJ_sigma2_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl)
{
	REG_DATA reg_data;									/* レジスタアクセス情報 */
	USHORT cdata[4]={0};
	UCHAR  cmd;
	UCHAR  sub_code;
	UCHAR  ccmd;
	USHORT cadrs;
	USHORT lcadrs;
	USHORT lcdata;
	INT32  ret;

	/* ローカル変数の初期化 */
	cdata[0] = UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.adj2.cdata);
	cmd      = pCmdHdl->cmdInfo.cmdBuf.adj2.cmd;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj2.subcode;
	ccmd     = pCmdHdl->cmdInfo.cmdBuf.adj2.ccmd;
	cadrs    = UtilGetWData(pCmdHdl->cmdInfo.cmdBuf.adj2.caddress);
	lcadrs   = UtilGetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.caddress);
	lcdata   = UtilGetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.cdata);
	ret  = CMD_FUNC_NORMAL_END;
	
	/* 実行条件チェック */
	if(  (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ( (pCmdHdl->cmdInfo.rsltBuf.adj2.cmd != cmd) 
	    || (pCmdHdl->cmdInfo.rsltBuf.adj2.subcode != sub_code) 
		|| (pCmdHdl->cmdInfo.adjWrk != ccmd)
		|| (lcadrs != cadrs) 
		|| ((ccmd == 1) && (lcdata != cdata[0])) )	 )
	{
		/* 解析結果をバッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.adj2.cmd     = cmd;
		pCmdHdl->cmdInfo.rsltBuf.adj2.subcode = sub_code;
		pCmdHdl->cmdInfo.rsltBuf.adj2.ccmd    = ccmd;
		pCmdHdl->cmdInfo.adjWrk = ccmd;
		UtilSetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.caddress, cadrs);

		if(ccmd == 0)
		{	/* データ参照 */
			pCmdHdl->cmdInfo.RegData.AccessSize = 2;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.adj2.cdata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.adj2.cdata);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_ADJ_RD, &pCmdHdl->cmdInfo.RegData);
		}
		else if(ccmd == 1)
		{	/* データ設定 */
			/* レジスタアクセス用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = 2;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
//			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)&cdata[0];
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)&pCmdHdl->cmdInfo.cmdBuf.adj2.cdata;/* <S0D4> */
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.cmdBuf.adj2.cdata);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);

			/* 解析結果をバッファへコピー */
			UtilSetDWData(pCmdHdl->cmdInfo.rsltBuf.adj.cdata, cdata[0]);
		}
		else
		{
			return DATA_SET_WARNG2;
		}
#if 0	/* <S0D4> */	
		UtilSetWData(pCmdHdl->cmdInfo.rsltBuf.adj2.cdata, cdata[0]);
		if((ret != REG_CONTINUE) && (ret != REG_SUCCESS))
		{/* エラー時はエラー情報をセット */
			pCmdHdl->cmdInfo.rsltBuf.adj2.ccmd = ccmd;
		}
#endif
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(pRegHdl);	
			if((ret != REG_CONTINUE) && (ret != REG_SUCCESS))	/* <S0D4> */
			{/* エラー時はエラー情報をセット */
				pCmdHdl->cmdInfo.rsltBuf.adj2.ccmd = pRegHdl->accErrCode;
			}
		}
	}

	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		：ＡＤＪ（通常モード）											*/
/****************************************************************************/
static INT32 cmdADJ_normal_mode( CMD_HNDL *pCmdHdl,  REG_HNDL *pRegHdl)
{
	REG_DATA reg_data;									/* レジスタアクセス情報 */
	ULONG  cdata[2]={0};
	UCHAR  cmd;
	UCHAR  sub_code;
	USHORT ccmd;
	USHORT cadrs;
	USHORT csize;
	ULONG  lcdata;
	INT32  ret;
	
	/* ローカル変数の初期化 */
	cdata[0] = UtilGetDWData(pCmdHdl->cmdInfo.cmdBuf.adj.cdata);
	cmd      = pCmdHdl->cmdInfo.cmdBuf.adj.cmd;
	sub_code = pCmdHdl->cmdInfo.cmdBuf.adj.subcode;
	ccmd     = pCmdHdl->cmdInfo.cmdBuf.adj.ccmd;
	cadrs    = pCmdHdl->cmdInfo.cmdBuf.adj.caddress;
	csize    = pCmdHdl->cmdInfo.cmdBuf.adj.csize;
	lcdata   = UtilGetDWData(pCmdHdl->cmdInfo.rsltBuf.adj.cdata);
	ret  = CMD_FUNC_NORMAL_END;


	/* 実行条件チェック */
	if(  (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ( (pCmdHdl->cmdInfo.rsltBuf.adj.cmd != cmd) 
	    || (pCmdHdl->cmdInfo.rsltBuf.adj.subcode != sub_code) 
		|| (pCmdHdl->cmdInfo.rsltBuf.adj.ccmd != ccmd)
		|| (pCmdHdl->cmdInfo.rsltBuf.adj.caddress != cadrs)
		|| (pCmdHdl->cmdInfo.adjWrk != csize)
		|| ((ccmd == 4) && (lcdata != cdata[0])) )	 )
	{
		if(ccmd == 3)
		{/* データ参照 */
			/* レジスタアクセス用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = 0;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.adj.cdata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.cmdBuf.adj.cdata);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_ADJ_RD, &pCmdHdl->cmdInfo.RegData);

		}
		else if(ccmd == 4)
		{/* データ設定 */
			/* レジスタアクセス用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = csize;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = VMEM_VEN_MIN + (cadrs<<1);
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
//			pCmdHdl->cmdInfo.RegData.pBuf = &cdata[0];
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)&pCmdHdl->cmdInfo.cmdBuf.adj.cdata;/* <S0D4> */
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(pRegHdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);

			/* 解析結果をバッファへコピー */
			UtilSetDWData(pCmdHdl->cmdInfo.rsltBuf.adj.cdata, cdata[0]);
		}
		else
		{
			return DATA_SET_WARNG2;
		}

		/* 解析結果をバッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.adj.cmd      = cmd;
		pCmdHdl->cmdInfo.rsltBuf.adj.subcode  = sub_code;
		pCmdHdl->cmdInfo.rsltBuf.adj.ccmd     = ccmd;
		pCmdHdl->cmdInfo.rsltBuf.adj.caddress = cadrs;
		pCmdHdl->cmdInfo.rsltBuf.adj.csize 	  = csize;
		pCmdHdl->cmdInfo.adjWrk = csize;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(pRegHdl);
			if((ret != REG_CONTINUE) && (ret != REG_SUCCESS))	/* <S0D4> */
			{/* エラー時はエラー情報をセット */
				pCmdHdl->cmdInfo.rsltBuf.adj.csize = pRegHdl->accErrCode;
			}
		}
	}
	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		：ＡＤＪ（テスターモード）										*/
/****************************************************************************/
//#include "HardSw.h"
static INT32 cmdADJ_tester_mode( CMD_HNDL *hdl )
{
#if 0
	UCHAR  cmd;     
    UCHAR  sub_code;
    UCHAR  LedCode;
    UCHAR  sw[3];
    
	cmd      = hdl->cmdInfo.cmdBuf.adj.cmd;
	sub_code = hdl->cmdInfo.cmdBuf.adj.subcode;
	LedCode  = hdl->cmdInfo.cmdBuf.byte[8];

	HwTestSWFunc(LedCode, &sw[0], &sw[2], &sw[1]);

	hdl->cmdInfo.rsltBuf.adj.cmd      = cmd;
	hdl->cmdInfo.rsltBuf.adj.subcode  = sub_code;
	hdl->cmdInfo.rsltBuf.byte[4] = sw[0];
	hdl->cmdInfo.rsltBuf.byte[5] = sw[1];
	hdl->cmdInfo.rsltBuf.byte[6] = sw[2];
#endif
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		：ＭＥＭ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 cmd_MEM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	LONG   adrs;
	UCHAR  size;
	UCHAR  *p, i;
	INT32  ret;

	/* ローカル変数の初期化 */
	pCmdHdl = &hNetData->cmd_hdl;
	adrs  = UtilGetDWData(&pCmdHdl->cmdInfo.cmdBuf.byte[4]);
	size  = pCmdHdl->cmdInfo.cmdBuf.byte[8];
	ret  = CMD_FUNC_NORMAL_END;
  
	if(size > 6)	
	{	/* データサイズ異常 */
		ret = DATA_SET_WARNG4;
	}
	else
	{
		p = (UCHAR*)adrs;
		for(i = 0;i < size; i++)
		{
			pCmdHdl->cmdInfo.rsltBuf.byte[9+i] = *(p+i);
		}
		UtilMemSet(&pCmdHdl->cmdInfo.rsltBuf.byte[9+size], 0, 6-size);
	}

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.nop.cmd = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	UtilSetDWData(&pCmdHdl->cmdInfo.rsltBuf.byte[4], adrs);
	pCmdHdl->cmdInfo.rsltBuf.byte[8] = size;

	/* 戻り値設定 */
	return ret;

}

/****************************************************************************/
/*								サブコマンド								*/
/****************************************************************************/

/****************************************************************************/
/* 概要		：未定義コマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_WARNG1				コマンド警告１					*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{	/* 異常終了を返す */
	hNetData->cmd_hdl.exCmdInfo.rsltBuf = hNetData->cmd_hdl.exCmdInfo.cmdBuf;
	return CMD_WARNG6;
}
INT32 exCmd_NO_SUPP( MLNET_HANDLE *hNetData )
{	/* 異常終了を返す */
	hNetData->cmd_hdl.exCmdInfo.rsltBuf = hNetData->cmd_hdl.exCmdInfo.cmdBuf;	  /*コーディングミス修正*//*<V594>*/
	return CMD_WARNG2;
}

/****************************************************************************/
/* 概要		：ＮＯＰコマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_NOP( MLNET_HANDLE *hNetData )
{
	/* ＣＭＤデータを解析結果バッファへコピー */
	hNetData->cmd_hdl.exCmdInfo.rsltBuf.mon.cmd = hNetData->cmd_hdl.exCmdInfo.cmdBuf.mon.cmd;
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		：ＰＲＭ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG1		データ設定警告１	（Ａ．９４Ａ）	*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= CMD_WARNG1			コマンド警告１		（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_PRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR  		cmd;								/* コマンドコード 		*/
	USHORT 		no;  								/* パラメータ番号 		*/
	UCHAR  		size;								/* パラメータサイズ 	*/
	USHORT 		data[4] = {0};						/* パラメータデータ 	*/
	REG_DATA 	reg_data;							/* レジスタアクセス情報 */
	INT32  		ret  = CMD_FUNC_NORMAL_END;
	
	/* ローカル変数の初期化 */
	pCmdHdl = &hNetData->cmd_hdl;
	
	cmd = pCmdHdl->exCmdInfo.cmdBuf.prm.cmd;
	no  = pCmdHdl->exCmdInfo.cmdBuf.prm.no;
	size = pCmdHdl->exCmdInfo.cmdBuf.prm.size;

	/* 実行条件チェック */
	if(  (pCmdHdl->exCmdInfo.cmdRdy != FALSE) 
	     && ((pCmdHdl->exCmdInfo.rsltBuf.prm.cmd != cmd) 
	     || (pCmdHdl->exCmdInfo.rsltBuf.prm.no != no) 
	     || (pCmdHdl->exCmdInfo.rsltBuf.prm.size != size)) )
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* レジスタアクセス用データ設定 */
			pCmdHdl->exCmdInfo.RegData.AccessSize = size;
			pCmdHdl->exCmdInfo.RegData.Length = 1;
			pCmdHdl->exCmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);
			pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->exCmdInfo.RegData.pBuf = (LONG*)pCmdHdl->exCmdInfo.rsltBuf.prm.data;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.prm.data);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->exCmdInfo.RegData);
		}
		/* 解析結果をバッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.prm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.prm.no   = no;
		pCmdHdl->exCmdInfo.rsltBuf.prm.size = size;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
		}
	}

	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		：ＰＲＭ＿ＷＲコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG1		データ設定警告１（Ａ．９４Ａ）		*/
/*			:	= DATA_SET_WARNG2		データ設定警告２（Ａ．９４Ｂ）		*/
/*			:	= DATA_SET_WARNG3		データ設定警告３（Ａ．９４Ｃ）		*/
/*			:	= DATA_SET_WARNG4		データ設定警告４（Ａ．９４Ｄ）		*/
/*			:	= CMD_WARNG1			コマンド警告１	（Ａ．９５Ａ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_PRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = RAM_OFFSET;
	INT32  ret;

	ret = exCmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl ,ofst);
	return ret;
}

/****************************************************************************/
/* 概要		：パラメータ書き込み実行処理									*/
/****************************************************************************/
static  INT32 exCmdPrmWrProc(CMD_HNDL* cmdHdl, REG_HNDL* regHdl, USHORT ofst)
{
	USHORT no   = cmdHdl->exCmdInfo.cmdBuf.prm.no;		/* パラメータ番号 */
	UCHAR  size = cmdHdl->exCmdInfo.cmdBuf.prm.size;	/* パラメータサイズ */
	USHORT data[4] = {0};								/* パラメータデータ */
	INT32  ret  = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (cmdHdl->exCmdInfo.cmdRdy != FALSE)
		&& cmdChkExPrmWr(&cmdHdl->exCmdInfo.cmdBuf, &cmdHdl->exCmdInfo.rsltBuf) )
	{
		if(size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* レジスタアクセス用データ設定 */
			cmdHdl->exCmdInfo.RegData.AccessSize = size;
			cmdHdl->exCmdInfo.RegData.Length = 1;
			cmdHdl->exCmdInfo.RegData.Number = (no<<1) + VMEM_VEN_MIN + ( (ofst) <<1 );
			cmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			cmdHdl->exCmdInfo.RegData.pBuf = (LONG*)cmdHdl->exCmdInfo.cmdBuf.prm.data;
			cmdHdl->exCmdInfo.RegData.BufferSize = sizeof(cmdHdl->exCmdInfo.cmdBuf.prm.data);
	
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(regHdl, REG_CMD_PRMWR, &cmdHdl->exCmdInfo.RegData);
		}
		/* 解析結果をバッファへコピー */
		cmdHdl->exCmdInfo.rsltBuf.prm.cmd  = cmdHdl->exCmdInfo.cmdBuf.prm.cmd;
		cmdHdl->exCmdInfo.rsltBuf.prm.no   = no;
		cmdHdl->exCmdInfo.rsltBuf.prm.size = size;
		/* ８byte分コピー(先頭アドレスは、奇数アドレスであるため、DWORDの書き込みはできない) */
		UtilSetLLData(cmdHdl->exCmdInfo.rsltBuf.prm.data, cmdHdl->exCmdInfo.cmdBuf.prm.data);
	}
	else
	{
		if(cmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(regHdl);
		}
	}

	/* 戻り値設定 */
	return ret;
}

/****************************************************************************/
/* 概要		：パラメータ書き込み実行条件チェック処理						*/
/****************************************************************************/
static INT32 cmdChkExPrmWr(CMD_LINK_EXDATA* cmd_buf, CMD_LINK_EXDATA* rslt_buf)
{
	UCHAR i, sz;

	if(rslt_buf->prm.cmd != cmd_buf->prm.cmd)	return TRUE;
	if(rslt_buf->prm.no != cmd_buf->prm.no)		return TRUE;
	if(rslt_buf->prm.size != cmd_buf->prm.size)	return TRUE;

	sz = (cmd_buf->prm.size >= 8) ? 8: cmd_buf->prm.size;
	for(i = 0; i < sz; i++)
	{
		if(cmd_buf->prm.data[i] != rslt_buf->prm.data[i])
			return TRUE;
	}

	return FALSE;
}

/****************************************************************************/
/* 概要		：ＡＬＭ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd, alm_rd_mod, alm_idx;
	UCHAR		*alm_data;
	INT32  		ret;
	
	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	
	cmd        = pCmdHdl->exCmdInfo.cmdBuf.alm.cmd;			/* コマンドコード			*/
	alm_rd_mod = pCmdHdl->exCmdInfo.cmdBuf.alm.mode;		/* アラーム読み出しモード	*/
	alm_idx    = pCmdHdl->exCmdInfo.cmdBuf.alm.data[0];		/* アラームインデックス番号	*/
	alm_data  = &pCmdHdl->exCmdInfo.rsltBuf.alm.data[0];	/* アラームデータ			*/

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy == FALSE) 
	    || (pCmdHdl->exCmdInfo.rsltBuf.alm.cmd != cmd)
	    || (pCmdHdl->exCmdInfo.rsltBuf.alm.mode != alm_rd_mod) 
	    || ((alm_rd_mod == ALM_RD_MOD2) && (pCmdHdl->exCmdInfo.rsltBuf.alm.data[0] != alm_idx)) 
	    || ((alm_rd_mod == ALM_RD_MOD3) && (pCmdHdl->exCmdInfo.rsltBuf.alm.data[0] != alm_idx)) )
	{
		ret = cmdAlmRdProc(&hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data);

		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.alm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.alm.mode = alm_rd_mod;
	}

	return ret;
}

/****************************************************************************/
/* 概要		：ＰＰＲＭ＿ＷＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG1		データ設定警告１（Ａ．９４Ａ）		*/
/*			:	= DATA_SET_WARNG2		データ設定警告２（Ａ．９４Ｂ）		*/
/*			:	= DATA_SET_WARNG3		データ設定警告３（Ａ．９４Ｃ）		*/
/*			:	= DATA_SET_WARNG4		データ設定警告４（Ａ．９４Ｄ）		*/
/*			:	= CMD_WARNG1			コマンド警告１	（Ａ．９５Ａ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_PPRM_WR( MLNET_HANDLE *hNetData )
{
	USHORT ofst = EEPROM_OFFSET;
	INT32  ret;

	ret = exCmdPrmWrProc(&hNetData->cmd_hdl, &hNetData->reg_hdl ,ofst);
	return ret;
}

/****************************************************************************/
/* 概要		：ＬＴＭＯＤ＿ＯＮコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= LTMOD_ON_CMD_WARNG4	コマンド警告４（Ａ．９５Ｄ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_LTMOD_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG  lt_sgn;
	UCHAR lt_mod;
	INT32  ret;

	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	lt_sgn = (LONG)pCmdHdl->exCmdInfo.cmdBuf.mon.lt_sgn & 0x03;
	lt_mod = (UCHAR)pCmdHdl->exCmdInfo.cmdBuf.mon.mon3[0];

	/* ラッチ要求 */
	ret = MciRequestMdlLatch(pMtHdl, lt_mod, lt_sgn);

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;
	pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 = pCmdHdl->exCmdInfo.cmdBuf.mon.sel_mon34;
	
	return ret;
}

/****************************************************************************/
/* 概要		：ＬＴＭＯＤ＿ＯＦＦコマンド解析処理							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= LTMOD_ON_CMD_WARNG4	コマンド警告４（Ａ．９５Ｄ）		*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_LTMOD_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32  ret;

	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	
	/* ラッチキャンセル */
	ret = MciCancelLatch(pMtHdl, MODAL_LTMD);
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;
	pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 = pCmdHdl->exCmdInfo.cmdBuf.mon.sel_mon34;
	
	return ret;
}

/****************************************************************************/
/* 概要		：ＳＭＯＮコマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/****************************************************************************/
/* 改版履歴	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 exCmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl		= &hNetData->cmd_hdl;
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;
	pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 = pCmdHdl->exCmdInfo.cmdBuf.mon.sel_mon34;

	return CMD_FUNC_NORMAL_END;
}
