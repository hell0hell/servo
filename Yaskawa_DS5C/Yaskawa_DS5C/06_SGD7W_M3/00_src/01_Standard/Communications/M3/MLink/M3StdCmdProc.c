/****************************************************************************
Description	: MECHATROLINK Command Analyzation (Standard Servo Profile)
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "M3Handler.h"
#include "ComAlmMngr.h"
#include "ComManager.h"
#include "SysApi.h"
#include "MtApi.h"
#include "CmdAnalyzer.h"
#include "MLib.h"
//#include "M3StdCmdProc.h"

/****************************************************************************
	Defines for Command function
****************************************************************************/
/* ID_RD */
#define ID_RD_MAX_SIZE			24				/* 最大読み出しサイズ	*/
#define	EVT_ID_RD_MAX_SIZE		8				/* 最大読み出しサイズ(イベントドリブン通信) */

/* ALM_RD */
#define ALM_RD_MOD0				0				/* 現在発生中の異常・警告の状態 */
#define ALM_RD_MOD1				1				/* 異常発生状況履歴 */
#define ALM_RD_MOD2				2				/* 現在発生中の異常・警告の詳細情報 */
#define ALM_RD_MOD3				3				/* 異常発生状況履歴の詳細情報 */
#define ALM_RD_NUM				12				/* アラーム読み出し個数(メインコマンド) */
#define EX_ALM_RD_NUM			4				/* アラーム読み出し個数(サブコマンド) */

/* CONFIG */
#define CONFIG_MOD0				0				/* パラメータ再計算 */
#define CONFIG_MOD1				1				/* EEPROMへの一括書き込み */
#define CONFIG_MOD2				2				/* パラメータ初期化 */

/* ALM_CLR */
#define ALM_CLR_MOD0			0				/* 現在発生中の異常・警告の状態クリア */
#define ALM_CLR_MOD1			1				/* 異常発生状況履歴クリア */

/* MEM_RD */
#define MEM_RD_MAX_SIZE			20				/* 最大読み出しサイズ(メインコマンド) */
#define EX_MEM_RD_MAX_SIZE		4				/* 最大読み出しサイズ(サブコマンド)	 */
#define EVT_MEM_RD_MAX_SIZE		4				/* 最大読み出しサイズ(イベントドリブン通信)	*/

/* MEM_WR */
#define MEM_WR_MAX_SIZE			20				/* 最大書き込みサイズ(メインコマンド) */
#define EX_MEM_WR_MAX_SIZE		4				/* 最大書き込みサイズ(サブコマンド)	 */

/* PRM_RD/PRM_WR/PPRM_WR */
#define RAM_OFFSET 				0x1000			/* RAM書き込み用オフセット */
#define EEPROM_OFFSET			0				/* EEPROM書き込み用オフセット */
#define COMM_PRM_OFFSET			0xA00			/* 共通パラメータ用オフセット */

/* INTERPOLATE～TRQCTRL */
#define CMD_MAX_TLIM			MT_MAXTRQ_30	/* TLIM最大値 */
#define CMD_MAX_VLIM			MT_OSVEL_30		/* VLIM最大値 */
#define CMD_MAX_ACCR			MT_MAX_ACCR		/* ACCR,DECR[ref/cyc^2] 最大値 */
#define CMD_MAX_ACCR_VEL		MT_MAX_ACCR_VEL	/* ACCR,DECR[ref/cyc^2] 最大値(VELCTRLコマンド時) */

/* 仮想メモリ */
#define VMEM_ID_MAX				0x0000FFFF		/* ID領域				終了アドレス	*/
#define VMEM_COM_MIN			0x00010000		/* 共通パラメータ		開始アドレス	*/
#define VMEM_COM_MAX			0x0001FFFF		/* 共通パラメータ		終了アドレス	*/
#define	VMEM_VEN_MIN			0x80000000		/* ベンダースペシフィック領域 開始アドレス	*/
#define	VMEM_VEN_MAX			0x8001FFFF		/* ベンダースペシフィック領域 終了アドレス	*/

#define	ONLY_C_PHASE_LATCH		0x00			/* LT_SGNL C相ラッチのみ */

/* SPOSING S字加減速比率定義 */
#define S_RATIO1	50			/* S字加減速時間比率1(%) */
#define S_RATIO2	25			/* S字加減速時間比率2(%) */

/* POSINGモード選択 */
#define POSING_SEL  0x0000
#define SPOSING_SEL 0x0001

/************ ビット指令 ************/
/* CMD_CTRL */
#define CMD_CTRL_ALM_CLR_MAP	3
#define CMD_CTRL_CMD_ID_MAP		6

#define CMD_CTRL_ALM_CLR_MASK	0x1
#define CMD_CTRL_CMD_ID_MASK	0x3

/* SVCMD_CTRL */
#define SVCMD_CTRL_CMD_PAUSE_MAP	0
#define SVCMD_CTRL_CMD_CANCEL_MAP	1
#define SVCMD_CTRL_STOP_MODE_MAP	2
#define SVCMD_CTRL_ACCFIL_MAP		4
#define SVCMD_CTRL_LT_REQ1_MAP		8
#define SVCMD_CTRL_LT_REQ2_MAP		9
#define SVCMD_CTRL_LT_SEL1_MAP		10
#define SVCMD_CTRL_LT_SEL2_MAP		12
#define SVCMD_CTRL_SEL_MON1_MAP		16
#define SVCMD_CTRL_SEL_MON2_MAP		20
#define SVCMD_CTRL_SEL_MON3_MAP		24

#define SVCMD_CTRL_CMD_PAUSE_MASK	0x1
#define SVCMD_CTRL_CMD_CANCEL_MASK	0x1
#define SVCMD_CTRL_STOP_MODE_MASK	0x3
#define SVCMD_CTRL_ACCFIL_MASK		0x3
#define SVCMD_CTRL_LT_REQ1_MASK		0x1
#define SVCMD_CTRL_LT_REQ2_MASK		0x1
#define SVCMD_CTRL_LT_SEL1_MASK		0x3
#define SVCMD_CTRL_LT_SEL2_MASK		0x3
#define SVCMD_CTRL_SEL_MON1_MASK	0xF
#define SVCMD_CTRL_SEL_MON2_MASK	0xF
#define SVCMD_CTRL_SEL_MON3_MASK	0xF

#define SVCMD_CTRL_ACK_MASK			0x0FFF0033


/* SVCMD_IO */
#define SVCMD_IO_V_CLMP_MAP			3
#define SVCMD_IO_V_PPI_MAP			4
#define SVCMD_IO_P_PPI_MAP			5
#define SVCMD_IO_P_CL_MAP			6
#define SVCMD_IO_N_CL_MAP			7
#define SVCMD_IO_G_SEL_MAP			8
//#define SVCMD_IO_BANK_SEL_MAP		16	/* <S1C0> :Delete */
//#define SVCMD_IO_NET_TRG_MAP		25/* <S012> */	/* <S0CB> Delete */
#define SVCMD_IO_NET_TRG_MAP		31/* <S0CB> */
//#define SVCMD_IO_VENDER_IO2_MAP	20	/* <S1C0> :Delete */
//#define SVCMD_IO_VENDER_IO3_MAP	24	/* <S1C0> :Delete */
//#define SVCMD_IO_TFF_FIL_INV_MAP	26/* <S1D0> */
//#define SVCMD_IO_VFF_FIL_INV_MAP	27/* <S1D0> */
//#define SVCMD_IO_VENDER_IO4_MAP	28	/* <S1C0> :Delete */

#define SVCMD_IO_V_CLMP_MASK		0x1
#define SVCMD_IO_V_PPI_MASK			0x1
#define SVCMD_IO_P_PPI_MASK			0x1
#define SVCMD_IO_P_CL_MASK			0x1
#define SVCMD_IO_N_CL_MASK			0x1
#define SVCMD_IO_G_SEL_MASK			0xF
#define SVCMD_IO_NET_TRG_MASK		0x1/* <S012> */

#define SVCMD_IO_BANK_SEL_MASK		0xF			/* BANK_SEL */
#define SVCMD_IO_SO1_MASK			0x1			/* IO出力設定機能 */
#define SVCMD_IO_SO2_MASK			0x2			/* IO出力設定機能 */
#define SVCMD_IO_SO3_MASK			0x4			/* IO出力設定機能 */
#define SVCMD_IO_VENDER_IO3_MASK	0xF
#define SVCMD_IO_VENDER_IO4_MASK	0xF


/* SUB_CTRL */
//#define SUB_CTRL_SUB_CMD_ID_MAP	6
#define SUB_CTRL_SELMON4_MAP		12
#define SUB_CTRL_SELMON5_MAP		16
#define SUB_CTRL_SELMON6_MAP		20

//#define SUB_CTRL_SUB_CMD_ID_MASK	0x3
#define SUB_CTRL_SELMON4_MASK		0xF
#define SUB_CTRL_SELMON5_MASK		0xF
#define SUB_CTRL_SELMON6_MASK		0xF

/*************************************/

/* SVCMD_CTRL処理の戻り値 */
#define CMD_FUNC_PAUSE_REQ			1
#define CMD_FUNC_CANCEL_REQ			2

/* モーションラッチコマンドコード */
#define CMD_CODE_EX_FEED			0x37
#define CMD_CODE_EX_POSING			0x39
#define CMD_CODE_ZRET				0x3A

/* ZRET MODE */
#define ZRET_MODE_TYPE_MAP			0
#define ZRET_MODE_TYPE_MASK			0xF
#define ZRET_MODE_HOME_DIR_MAP		7
#define ZRET_MODE_HOME_DIR_MASK		1

#define ZRET_MODE_TYPE_LATCH_MODE			0
#define ZRET_MODE_TYPE_LATCH_AND_DEC_MODE	1


/* Local functions */
static VOID stdCmdCmdCtrlProc( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, MT_HNDL *pMtHdl );
static VOID stdCmdSvCmdCtrlProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl, BOOL enable );
static VOID stdCmdSvCmdIoProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl );
static VOID stdCmdSubCtrlProc( CMD_HNDL *hdl );
static INT32 stdCmdAlmRdProc( ALM_HNDL *alm_hdl, USHORT alm_rd_mod, USHORT idx, USHORT *alm_data, LONG read_num );
static INT32 stdCmdChkConnectCmd( MLNET_HANDLE *hNetData, LONG *reqphs, 
								  LONG *dtmod, LONG *com_tim, BOOL AsyncMode );
//static INT32 cmdMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData );
//static INT32 cmdMulgain32WithClampAccTime( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )




/****************************************************************************/
/*								ビット指令処理								*/
/****************************************************************************/
/****************************************************************************/
/* 概要		:	コマンド制御(CMD_CTRL)処理									*/
/****************************************************************************/
/* 引数		:	CMD_HNDL *hdl												*/
/*			:	ALM_HNDL *pAlmHdl											*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static	VOID stdCmdCmdCtrlProc( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, MT_HNDL *pMtHdl )
{
	LONG	cmd_ctrl;
	LONG	almClr, lstAlmClr;
	INT32	ret;

	cmd_ctrl = pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;

	/* ALM_CLR *//* エッジでアラームクリア */
	almClr = ( cmd_ctrl >> CMD_CTRL_ALM_CLR_MAP ) & CMD_CTRL_ALM_CLR_MASK;
	if( almClr )
	{
		lstAlmClr = ( pCmdHdl->cmdInfo.lstCmdCtrl >> CMD_CTRL_ALM_CLR_MAP ) & CMD_CTRL_ALM_CLR_MASK;
		ret = AlmSetClearReq( pAlmHdl, (BOOL)(almClr != lstAlmClr) );
		
		/* ALM_CLRステータス作成 */
		pCmdHdl->cmdInfo.almClrCmp = (BOOL)( (ret==ALM_SUCCESS) && almClr );
	}
	else
	{
		/* ALM_CLRステータス作成 */
		pCmdHdl->cmdInfo.almClrCmp = FALSE;
	}
	/* CMD_CTRLの保存 */
	pCmdHdl->cmdInfo.lstCmdCtrl = cmd_ctrl;
	
	/* CMD_ID を解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd_ctrl
		 = (USHORT)( cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP) );
}


/****************************************************************************/
/* 概要		:	サーボコマンド制御フィールド(SVCMD_CTRL)処理				*/
/****************************************************************************/
/* 引数		:	CMD_HNDL *hdl												*/
/*			:	MT_HNDL *pMtHdl												*/
/* 			:	BOOL enable		TRUE/FALSE									*/
/*								CMD_CANCEL,CMD_PAUSE を実行する  = TRUE		*/
/*								CMD_CANCEL,CMD_PAUSE を実行しない= FALSE	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static VOID stdCmdSvCmdCtrlProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl, BOOL enable )
{
	MCI_CMD		cmd;
	ULONG		svcmd_ctrl;
	ULONG		lstsvcmd_ctrl;										/* svcmd_ctrl 前回値 */
	ULONG		cmdPause, cmdCancel;								/* CMD_PAUSE, CMD_CANCEL指令 */
	LONG		ltSel1, ltSel2;										/* ラッチ選択 */
	LONG		ltReq1, ltReq2;										/* ラッチ要求 */
	UCHAR		cmdCode;											/* コマンドコード */

	/* ローカル変数の初期化 */
	svcmd_ctrl		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	lstsvcmd_ctrl	= pCmdHdl->cmdInfo.lstSvCmdCtrl;
	cmdCode			= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;


	/*-----------------------------------------------------------------*/
	/* 					CMD_CANCEL, CMD_PAUSE			 			   */
	/*-----------------------------------------------------------------*/
	/* POSING,FEED,EX_FEED,EX_POSING,ZRET,VELCTRL のときのみ処理を行う */
	/*-----------------------------------------------------------------*/
	if(enable)
	{ /* CMD_CANCEL,CMD_PAUSE が有効なコマンドの場合の処理 */
		/* CMD_PAUSE, CMD_CANCEL のビット指令取得 */
		cmdPause  = ( svcmd_ctrl & (1<<SVCMD_CTRL_CMD_PAUSE_MAP) );
		cmdCancel = ( svcmd_ctrl & (1<<SVCMD_CTRL_CMD_CANCEL_MAP) );

		/*--------------------------------------*/
		/*				CMD_CANCEL				*/
		/*--------------------------------------*/
		if( cmdCancel )
		{ /* CMD_CANCEL オン */
			/* モーション指令データを設定 */
			cmd.HoldMod = (svcmd_ctrl >> SVCMD_CTRL_STOP_MODE_MAP) & SVCMD_CTRL_STOP_MODE_MASK;

			/* CMD_CANCEL API関数実行 */
			MciSetCmdCancel( pMtHdl, &cmd );
			pCmdHdl->cmdInfo.cmdCancelEnable = TRUE;
		}
		else
		{
			/* CMD_CANCEL = FALSE の場合 フラグをクリアする */
			pCmdHdl->cmdInfo.cmdCancelEnable = FALSE;

			/*--------------------------------------*/
			/*				CMD_PAUSE				*/
			/*--------------------------------------*/
			if( cmdPause )
			{ /* CMD_PAUSE オン */
				/* モーション指令データを設定 */
				cmd.HoldMod = (svcmd_ctrl >> SVCMD_CTRL_STOP_MODE_MAP) & SVCMD_CTRL_STOP_MODE_MASK;

				/* 原点復帰アプローチ速度をゼロに設定 */
				MciClearZretSpd(pMtHdl);

				/* CMD_PAUSE API関数実行 */
				MciSetCmdPause(pMtHdl, &cmd);
				pCmdHdl->cmdInfo.cmdPauseEnable = TRUE;
			}
			else
			{
				/* 原点復帰アプローチ速度を復帰 */
				MciReturnZretSpd(pMtHdl);

				/* CMD_PAUSE = FALSE の場合 フラグをクリアする */
				pCmdHdl->cmdInfo.cmdPauseEnable = FALSE;
			}
		}
	}
	else
	{/* CMD_PAUSE, CMD_CANCEL 未使用の場合は有効フラグをクリアする */
		pCmdHdl->cmdInfo.cmdPauseEnable = FALSE;
		pCmdHdl->cmdInfo.cmdCancelEnable = FALSE;
	}

	/*-------------------*/
	/* 		ACCFIL		 */
	/*-------------------*/
	pMtHdl->cmdData.AccFilSel = (svcmd_ctrl >> SVCMD_CTRL_ACCFIL_MAP) & SVCMD_CTRL_ACCFIL_MASK;

	/*--------------------------------------------*/
	/* LT_REQ1,LT_SEL1 */  /* エッジでラッチ1要求 */
	/*--------------------------------------------*/
	ltSel1 = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;
	ltReq1 = (svcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ1_MAP)) ;

	if( (cmdCode != CMD_CODE_EX_FEED) && (cmdCode != CMD_CODE_EX_POSING) && (cmdCode != CMD_CODE_ZRET) )
	{/* ラッチコマンド以外のコマンドの場合にモーダルラッチ要求を行う */
	 /* ラッチコマンドはモーションＡＰＩでラッチ要求を行う */
		if( 0 != ltReq1 )
		{
			if( (0 == (lstsvcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ1_MAP)) )
				|| (pCmdHdl->cmdInfo.reLatchReq != FALSE) )
			{
				MciRequestMdlLatch( pMtHdl, 0, ltSel1, 0);
				pCmdHdl->cmdInfo.reLatchReq = FALSE;
			}
		}
	}
	else
	{/* ラッチコマンドの場合は、モーションAPIでラッチ要求を行うので"FALSE"とする */
		pCmdHdl->cmdInfo.reLatchReq = FALSE;
	}
	if( 0 == ltReq1 )
	{/* ラッチ要求なしの場合は、ラッチ要求クリア */
		MciCancelLatch( pMtHdl, MOTION_LTMD, 0 );
		MciCancelLatch( pMtHdl, MODAL_LTMD, 0 );
		pCmdHdl->cmdInfo.reLatchReq = FALSE;
	}

	/*--------------------------------------------*/
	/* LT_REQ2,LT_SEL2  */ /* エッジでラッチ2要求 */
	/*--------------------------------------------*/
	ltSel2 = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL2_MAP) & SVCMD_CTRL_LT_SEL2_MASK;
	ltReq2 = (svcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ2_MAP)) ;

	if( 0 != ltReq2 )
	{/* モーダルラッチ要求を行う （ラッチコマンドはLT_REQ2を利用しない） */
		if( 0 == (lstsvcmd_ctrl & (1 << SVCMD_CTRL_LT_REQ2_MAP)))
		{
			if( pMtHdl->mtPrm.LtPrm.seqNum == 0 )
			{/* 通常ラッチ */	
				MciRequestMdlLatch( pMtHdl, 0, ltSel2, 1 );
			}
			else
			{/* 連続ラッチ有効 */
				MciRequestMdlLatch( pMtHdl, 1, ltSel2, 1 );
			}
		}
	}
	else 
	{/* ラッチ要求なしの場合は、ラッチ要求クリア */
		MciCancelLatch( pMtHdl, MOTION_LTMD, 1 );
		MciCancelLatch( pMtHdl, MODAL_LTMD, 1 );
	}

	/* ラッチ信号選択情報の保存（共通モニタ用） */
	pCmdHdl->cmdInfo.ltSelR[0] = ltSel1;
	pCmdHdl->cmdInfo.ltSelR[1] = ltSel2;

	/* SVCMD_CTRL情報の保存 */
	pCmdHdl->cmdInfo.lstSvCmdCtrl = svcmd_ctrl;

	/* ACCFIL, SEL_MON1～3 を解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl = svcmd_ctrl & SVCMD_CTRL_ACK_MASK;
}


/****************************************************************************/
/* 概要		:	サーボコマンド入出力信号(SVCMD_IO)処理						*/
/****************************************************************************/
/* 引数		:	CMD_HNDL *hdl												*/
/*			:	MT_HNDL *pMtHdl												*/
/*			:	REG_HNDL *pRegHdl											*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static VOID stdCmdSvCmdIoProc( CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl )
{
	ULONG	svcmd_io, CmdSeq;
	ULONG	bitVal;	
	USHORT	CmdCtrl;

	svcmd_io = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_io;

	///* V_CLMP */	/* 未サポート */
	//if( pMtHdl->cmdData.CtrlMode == MT_TRQ_CTRLMD )
	//{
	//	bitVal = (UCHAR)( (svcmd_io >> SVCMD_IO_V_CLMP_MAP) & SVCMD_IO_V_CLMP_MASK );
	//	MciSetEnbVelClmpSignal( pMtHdl, bitVal );
	//}

	/* Reset CmdCtrlBit Data *//* <S021> */
	CmdCtrl = pMtHdl->svCmd->CmdCtrl.CmdCtrlBit
			  & (~(ENBPCTRL_BIT | CLRPOSINTG_BIT | TRACE_TRG_BIT));


	/* V_PPI */
//	CmdCtrl = pMtHdl->svCmd->CmdCtrl.CmdCtrlBit & (~(ENBPCTRL_BIT | CLRPOSINTG_BIT));	/* <S013> */
	bitVal = ( svcmd_io >> SVCMD_IO_V_PPI_MAP ) & 1;
	CmdCtrl |= (USHORT)(bitVal << ENBPCTRL_BITNO);

	/* P_PPI */
	bitVal = ( svcmd_io >> SVCMD_IO_P_PPI_MAP ) & 1;
	CmdCtrl |= (USHORT)(bitVal << CLRPOSINTG_BITNO);

#if CSW_NET_TRG_USE == TRUE		/* <S012> */
	/* NET_TRG *//* <S012> */
	bitVal = ( svcmd_io >> SVCMD_IO_NET_TRG_MAP ) & SVCMD_IO_NET_TRG_MASK;
	CmdCtrl |= (USHORT)(bitVal << TRACE_TRIGGER_BITNO);
#endif		/* <S012> */

	pMtHdl->svCmd->CmdCtrl.CmdCtrlBit = CmdCtrl;

	/* P_CL/N_CL */
	CmdSeq = pMtHdl->svCmd->CmdSeqBit & (~(ENB_EXTPTRQLMT_BIT | ENB_EXTNTRQLMT_BIT));
	bitVal = ( svcmd_io >> SVCMD_IO_P_CL_MAP ) & 3;
	CmdSeq |= (ULONG)(bitVal << ENB_EXTPTRQLMT_BITNO);

	pMtHdl->svCmd->CmdSeqBit = CmdSeq;

	/* G_SEL */
	bitVal = ( svcmd_io >> SVCMD_IO_G_SEL_MAP ) & SVCMD_IO_G_SEL_MASK;
	pMtHdl->svCmd->CmdCtrl.SelBank = (UCHAR)bitVal;

#if 0/* VendorSpecificsI/O任意割り付け対応	*//* <S1C0>:Start */
	/* BANK_SEL 16bit目*//* <S080> */
	bitVal = ( svcmd_io >> SVCMD_IO_BANK_SEL_MAP ) & SVCMD_IO_BANK_SEL_MASK;
	if( bitVal < pMtHdl->mtPrm.Bnk.bankNum )
	{
		pMtHdl->mtPrm.Bnk.cmdSel = bitVal;
	}

	///* SO1, SO2, SO3 *//* <S14D>:以下、コメントアウトから解除 */
	if( pMtHdl->mtPrm.ProfPrm.OutSigEnable != FALSE)
	{ /* Pn50E,50F,510=0のときのみ実行 */
		bitVal  = ( svcmd_io >> SVCMD_IO_VENDER_IO2_MAP ) & SVCMD_IO_SO1_MASK;
		bitVal |= ( svcmd_io >> SVCMD_IO_VENDER_IO2_MAP ) & SVCMD_IO_SO2_MASK;
		bitVal |= ( svcmd_io >> SVCMD_IO_VENDER_IO2_MAP ) & SVCMD_IO_SO3_MASK;
		pMtHdl->svCmd->DOSignal = bitVal;
	}
#else
	/* BANK_SEL 16bit目*//* <S080> */
	bitVal = ( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_BANK].bitNo )
									 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_BANK].bitsMsk;
	if( bitVal < pMtHdl->mtPrm.Bnk.bankNum )
	{
		pMtHdl->mtPrm.Bnk.cmdSel = bitVal;
	}

	///* SO1, SO2, SO3 *//* <S14D>:以下、コメントアウトから解除 */
	if( pMtHdl->mtPrm.ProfPrm.OutSigEnable != FALSE)
	{ /* Pn50E,50F,510=0のときのみ実行 */
		bitVal  = ( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO1].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO1].bitsMsk;
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO2].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO2].bitsMsk) << 1);
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO3].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO3].bitsMsk) << 2);
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ	*/
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO4].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO4].bitsMsk) << 3);
		bitVal |= ((( svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO5].bitNo )
								 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SO5].bitsMsk) << 4);
#endif/*	SGD7W（2軸対応）のみ	*/

		pMtHdl->svCmd->DOSignal = bitVal;
	}
#endif/* VendorSpecificsI/O任意割り付け対応	*//* <S1C0>:End */
/* <S1D0> Start */
/* TFF_FIL_INV */
//	bitVal = (BOOL)(svcmd_io >> SVCMD_IO_TFF_FIL_INV_MAP) & SVCMD_IO_TFF_FIL_INV_MASK;		/* <VFF_TFF_MAFIL> */
//	pMtHdl->svCmd->CmdCtrl.tffFilSts = bitVal;

/* VFF_FIL_INV */
//	bitVal = (BOOL)(svcmd_io >> SVCMD_IO_VFF_FIL_INV_MAP) & SVCMD_IO_VFF_FIL_INV_MASK;		/* <VFF_TFF_MAFIL> */
//	pMtHdl->svCmd->CmdCtrl.vffFilSts = bitVal;
/* <S1D0> End */
}


/****************************************************************************/
/* 概要		:	サブコマンド制御(SUB_CTRL)処理								*/
/****************************************************************************/
/* 引数		:	CMD_HNDL *hdl												*/
/* 戻り値	:	なし														*/
/****************************************************************************/
static VOID stdCmdSubCtrlProc( CMD_HNDL *hdl )
{
	/*----------------------------------------*/
	/*  SELMON4～6 を解析結果バッファへコピー */
	/*----------------------------------------*/
	/* SELMON4 */
	hdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl1 = 
			(UCHAR)( hdl->exCmdInfo.cmdBuf.stdData.mon.sub_ctrl1 & (SUB_CTRL_SELMON4_MASK << 4));

	/* SELMON5, SELMON6 */
	hdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl2 = 
				hdl->exCmdInfo.cmdBuf.stdData.mon.sub_ctrl2;
}


/****************************************************************************/
/*								メインコマンド								*/
/****************************************************************************/
/****************************************************************************/
/* 概要		:	未定義コマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_WARNG6			コマンド警告６						*/
/****************************************************************************/
INT32 stdCmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* 異常終了を返す */
	pCmdHdl->cmdInfo.rsltBuf.stdData = pCmdHdl->cmdInfo.cmdBuf.stdData;
	return CMD_WARNG6;
}


/****************************************************************************/
/* 概要		:	未サポートコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_WARNG2			コマンド警告２						*/
/****************************************************************************/
INT32 stdCmd_NO_SUPP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* 異常終了を返す */
	pCmdHdl->cmdInfo.rsltBuf.stdData = pCmdHdl->cmdInfo.cmdBuf.stdData;
	return CMD_WARNG2;
}


/****************************************************************************/
/* 概要		:	ＮＯＰコマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
INT32 stdCmd_NOP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );
	/* 解析結果をバッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;
	return CMD_FUNC_NORMAL_END;
}

/****************************************************************************/
/* 概要		:	ＩＤ＿ＲＤコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= DATA_SET_WARNG2		データ設定警告２（Ａ．９４Ｂ）		*/
/*			:	= DATA_SET_WARNG4		データ設定警告４（Ａ．９４Ｄ）		*/
/****************************************************************************/
INT32 stdCmd_ID_RD( MLNET_HANDLE *hNetData )
{
	UCHAR		cmd, id_code, offset;
	USHORT		size;
	INT32		ret;
	CMD_HNDL	*pCmdHdl;

	/* ローカル変数の初期化 */
	pCmdHdl = &hNetData->cmd_hdl;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	ret	= CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.cmd;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.cmd != cmd) 
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[1] != pCmdHdl->cmdInfo.cmdBuf.dword[1])) ) 
	{
		id_code = pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.id_code;
		offset   = pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.offset;
		size    = pCmdHdl->cmdInfo.cmdBuf.stdData.id_rd.size;

		if( size <= ID_RD_MAX_SIZE )
		{
			/* デバイス情報読み出し用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = 1;
			pCmdHdl->cmdInfo.RegData.Length = (LONG)size;
			pCmdHdl->cmdInfo.RegData.Number = (LONG)(id_code*4);	/* <S**1> */
			pCmdHdl->cmdInfo.RegData.offset = (LONG)offset;			/* <S**1> */
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = (LONG*)pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.id_info;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.id_info);
			
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_IDRD, &pCmdHdl->cmdInfo.RegData);
		}
		else
		{
			MlibResetByteMemory( pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.id_info, ID_RD_MAX_SIZE );
			ret = DATA_SET_WARNG4;
		}

		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.id_rd.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.dword[1] = pCmdHdl->cmdInfo.cmdBuf.dword[1];

		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	ＣＯＮＦＩＧコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/****************************************************************************/
INT32 stdCmd_CONFIG( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
//	COM_HNDL	*pComHdl;
	LONG		cmd, config_mod;
	LONG		cmd_switch_time;
	BOOL		sv_on;/* <S06D> */
	INT32		result;

#if (CSW_DEBUG_PRINT_ENABLE == TRUE)
	printf( "called CONFIG command\n" );
#endif

	/* ローカル変数の初期化 */
	pCmdHdl			= &hNetData->cmd_hdl;
//	pComHdl			= &hNetData->com_hdl;
	cmd				= pCmdHdl->cmdInfo.cmdBuf.stdData.config.cmd;
	config_mod		= pCmdHdl->cmdInfo.cmdBuf.stdData.config.config_mod;
	result			= CMD_FUNC_NORMAL_END;
	cmd_switch_time = FALSE;

	sv_on			= hNetData->mt_hdl.svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK;/* サーボオン状態 *//* <S06D> */

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.rsltBuf.stdData.config.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.config.cmd) 
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.config.config_mod != config_mod) )
	{
		pCmdHdl->cmd_seq = SV_OFF_SEQ;	/* コマンド切り替わりで処理開始 */
		cmd_switch_time = TRUE;
	}

	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE) || (cmd_switch_time != FALSE) )
	{ /* CMDRDY=FALSE or コマンドの切り替わりで実行 */
		result = CMD_FUNC_CONTINUE;
		if( sv_on == FALSE )/* <S06D>:sv_onを追加 *//* <S0B3> */
		{
			if( config_mod == CONFIG_MOD0 )
			{ /* パラメータ再計算 */
				if(pCmdHdl->cmd_seq == SV_OFF_SEQ)
				{
					pCmdHdl->cmdInfo.lstSvCmdCtrl = 0;	/* LT_REQの前回値クリア */
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
						pCmdHdl->cmd_seq = MT_ENABLE_SEQ;
					}
				}
				else
				{
					result = RegCheckCommandState(&hNetData->reg_hdl);
					if(result != REG_CONTINUE)
					{
						pCmdHdl->cmd_seq = IDOL_SEQ;
//						result = CMD_FUNC_NORMAL_END; /* <S0EC> */
					}					
				}
			}
			else if( config_mod == CONFIG_MOD2 )
			{ /* パラメータ初期化 */
				if(pCmdHdl->cmd_seq == SV_OFF_SEQ)
				{
					pCmdHdl->cmdInfo.lstSvCmdCtrl = 0;	/* LT_REQの前回値クリア */
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
					if( REG_CONTINUE == RegRequestCommand(&hNetData->reg_hdl, REG_CMD_INITPRM, NULL) ) 
					{ /* リセット処理終了 */
						pCmdHdl->cmd_seq = MT_ENABLE_SEQ;
					}
				}
				else
				{
					result = RegCheckCommandState(&hNetData->reg_hdl);
					if(result != REG_CONTINUE)
					{
						pCmdHdl->cmd_seq = IDOL_SEQ;
//						result = CMD_FUNC_NORMAL_END; /* <S0EC> */
					}					
				}
			}
			else
				{/* mode範囲外	*/
				result = DATA_SET_WARNG2;
			}
		}
		else/* <S0B3> */
		{/* サーボオン中	*/
			result = CMD_WARNG1;
		}
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.config.cmd = (UCHAR)cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.config.config_mod = (UCHAR)config_mod;

		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = result;
	}

#if (CSW_DEBUG_PRINT_ENABLE == TRUE)
	if( result == CMD_FUNC_NORMAL_END )
	{
		printf( "CONFIG command normal end\n\n" );
	}
#endif

	return result;
}


/****************************************************************************/
/* 概要		:	ＡＬＭ＿ＲＤコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/****************************************************************************/
INT32 stdCmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_rd_mod, alm_idx;
	USHORT		*alm_data;
	INT32		ret;
	
	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd			= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.cmd;
	alm_rd_mod	= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.mode;
	alm_idx		= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.index;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE)
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd != cmd) 
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode != alm_rd_mod) 
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.index != alm_idx) )
	{
		/* AlmTblMax = 10 */
		alm_data = &pCmdHdl->cmdInfo.rsltBuf.stdData.alm.data[0];
		ret = stdCmdAlmRdProc( &hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data, ALM_RD_NUM );

		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode = alm_rd_mod;
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.index = alm_idx;
		
		/* コマンド解析実行結果設定 *//* <S14D> */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	アラーム読み出しＡＰＩ実行処理								*/
/****************************************************************************/
static	INT32 stdCmdAlmRdProc( ALM_HNDL *alm_hdl, USHORT alm_rd_mod, USHORT idx, 
											USHORT *alm_data, LONG read_num )
{
	INT32 ret;

	if( alm_rd_mod == ALM_RD_MOD0 )
	{ /* 現在発生アラームリード */
		ret = AlmGetCrtDetailBufAll( alm_hdl, alm_data, read_num );
	}
	else if( alm_rd_mod == ALM_RD_MOD1 )
	{ /* アラーム履歴リード */
		ret = AlmGetHistoryDetailAll( alm_hdl, alm_data, read_num );
	}
	else
	{
		ret = DATA_SET_WARNG2;
	}
	return ret;
}


/****************************************************************************/
/* 概要		:	ＡＬＭ＿ＣＬＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdCmd_ALM_CLR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL				*pCmdHdl;
	UCHAR					cmd;			
	USHORT					alm_clr_mod;	
	INT32					ret;

	/* ローカル変数の初期化 */
	ret		= CMD_FUNC_NORMAL_END;
	pCmdHdl	= &hNetData->cmd_hdl;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd			= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.cmd;
	alm_clr_mod	= pCmdHdl->cmdInfo.cmdBuf.stdData.alm.mode;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
	    && ((pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd  != cmd) 
	    || (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode != alm_clr_mod)) )
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
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.alm.mode = alm_clr_mod;

		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			if( alm_clr_mod == ALM_CLR_MOD0 )
			{
				ret = AlmSetClearReq( &hNetData->alm_hdl, FALSE );
			}
			else //if( alm_clr_mod == ALM_CLR_MOD1 )
			{
				ret = AlmGetAlarmHistoryClearStatus( &hNetData->alm_hdl );
			}
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

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
INT32 stdCmd_SYNC_SET( MLNET_HANDLE *hNetData )
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

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );

	cmd	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd != cmd) && (pComHdl->Phase == 2) )
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
				pComHdl->NetPrm.tType, pComHdl->NetPrm.aplCycle, NdGetComMode() );
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
				pCmdHdl->cmdInfo.ResultCmdAna = result;/* <S14D> */
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
			pCmdHdl->cmdInfo.ResultCmdAna = result;/* <S14D> */
		}
		break;
	default:
		result = CMD_FUNC_NORMAL_END;
		break;
	}

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = cmd;
//	/* コマンド解析実行結果設定 */
//	pCmdHdl->cmdInfo.ResultCmdAna = result;/* <S14D> */

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
INT32 stdCmd_CONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	MT_HNDL		*pMtHdl;
	CYC_CHG_PRM	*cycPrm;
	LONG		com_tim, dtmod, reqphs;
	INT32		ret, rc;
	BOOL		AsyncMode;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	com_tim		= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_time;		/* 通信周期 */
	rc			= CMD_FUNC_CONTINUE;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	
	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy) && (pComHdl->Phase == 1) )
	{ /* IDOL状態 && Phase=1 で処理開始 */
		pCmdHdl->cmd_seq = REQ_PHSUP_SEQ;
	}

	switch(pCmdHdl->cmd_seq)
	{
	case REQ_PHSUP_SEQ:	/* フェーズアップ要求 */
		AsyncMode = (BOOL)NdGetComMode();
		ret = stdCmdChkConnectCmd( hNetData, &reqphs, &dtmod, &com_tim, AsyncMode);
		if( ret == CMD_FUNC_NORMAL_END )
		{ /* コマンド正常時 */

			/* プロファイルタイプ設定処理 */
			CmdSetProfileType( hNetData, 0);

			if( COM_FUNC_NORMAL_END == 
					ComReqPhaseUp(pComHdl, reqphs, dtmod, com_tim, AsyncMode) )
			{ /* 正常なら次のシーケンスへ、失敗時はリトライ */
				pCmdHdl->cmd_seq = PHASE_CHK_SEQ;
			}
		}
		else if( ret != CMD_FUNC_CONTINUE )
		{ /* 異常時 */
			rc = ret;
			pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
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
				pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
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
		cycPrm->intCyc = NdGetTransmissionCycle();
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
			pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
		}
		break;
	case FAIL_END_SEQ:		/* シーケンス失敗 */
		if( MciEnableState(pMtHdl, TRUE) == MCI_SUCCESS )
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			rc = CMD_SYNC_FAULT;
			pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */
		}
		break;
	default:
		rc = CMD_FUNC_NORMAL_END;
		break;
	}
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.cmd =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.cmd;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.ver =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.ver;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.com_mod =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_mod;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.com_time =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_time;
	pCmdHdl->cmdInfo.rsltBuf.stdData.connect.profile_type =
					pCmdHdl->cmdInfo.cmdBuf.stdData.connect.profile_type;

	/* コマンド解析実行結果設定 */
//	pCmdHdl->cmdInfo.ResultCmdAna = rc;/* <S14D> */

	return rc;
}


/****************************************************************************/
/* 概要		:	ＣＯＮＮＥＣＴコマンドデータチェック処理					*/
/****************************************************************************/
static INT32 stdCmdChkConnectCmd( MLNET_HANDLE *hNetData, LONG *reqphs, 
								  LONG *dtmod, LONG *com_tim, BOOL AsyncMode )
{
	CMD_HNDL	*pCmdHdl;
	LONG		ver, com_mod, profile;
	LONG		t_byte;		/* <S07A> */
	BOOL		sv_on;		/* <S07A> */
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	ver			= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.ver;				/* バージョン */
	com_mod		= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.com_mod;			/* 通信モード */
	profile		= pCmdHdl->cmdInfo.cmdBuf.stdData.connect.profile_type;		/* プロファイルタイプ */
	t_byte		= hNetData->com_hdl.NetPrm.dataSize;						/* 伝送バイト数 */		/* <S07A> */
	sv_on		= hNetData->mt_hdl.svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK; /* サーボオン状態 */	/* <S07A> */
	ret			= CMD_FUNC_NORMAL_END;

	/* 実行条件チェック *//*<S07A1> */
	if( sv_on != FALSE )
	{
		return  CMD_WARNG1;
	}

	if( ver != 0x30 )
	{/*	未サポートVer	*/
		return  DATA_SET_WARNG2;
	}

	/* Profile設定 */
	switch(profile)
	{
	case M3STD_PROFILE:
		if(AsyncMode != FALSE)
		{
			return  DATA_SET_WARNG2;
		}
		if( (com_mod & 0x80)&&(t_byte != 48) )/* <S07A> */
		{
			return  DATA_SET_WARNG2;
		}
		break;

	case EVTID_PROFILE:
		if(AsyncMode == FALSE)
		{
			ret = DATA_SET_WARNG2;
		}

		pCmdHdl->useExCmd = FALSE;
		*reqphs = 2;
		return ret;

	case M2_PROFILE:
	default:
		return  DATA_SET_WARNG2;
	}

	/* サブコマンド使用／未使用設定 */
	pCmdHdl->useExCmd = (com_mod >> 7) & 0x1;

	/* 要求フェーズ設定 */
	*reqphs = ((com_mod & 0x02) == 0x2) ? 3 : 2;

	/* 伝送モード設定 */
	com_mod = ( com_mod >> 2 ) & 0x3;
	if( com_mod == 0x0 )
	{ /* 00b: 単送モード */
		if( (*com_tim < 1) /*|| (com_tim > 32)*/ )
		{
			return DATA_SET_WARNG2;
		}
		*dtmod = SINGLE_TRANSFER;
	}
	else if( com_mod == 0x1 )
	{ /* 01b: 連送モード */
		if( *com_tim < 2 )
		{/* 2倍未満ならNG */
			return DATA_SET_WARNG2;
		}
		*dtmod = CONSECUTIVE_TRANSFER;
	}
	else
	{ /* 10b,11b :予約 */
		return DATA_SET_WARNG2;
	}
	return ret;
}


/****************************************************************************/
/* 概要		:	ＤＩＳＣＯＮＮＥＣＴコマンド解析処理						*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdCmd_DISCONNECT( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	UCHAR		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;
	pComHdl	= &hNetData->com_hdl;
	cmd		= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
//	result	= CMD_FUNC_CONTINUE;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd != cmd)
		&& ((pComHdl->Phase > 1) || (pComHdl->reqPhase > 1)) )
	{
		/* コマンド切り替わり && (現在Phase2以上 or PhaseUP中) */
		pCmdHdl->cmd_seq = REQ_PHSDOWN_SEQ;
	}

	/* DISCONNECT処理実行 */
	result = CmdRunDisConnect( hNetData );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＭＥＭ＿ＲＤコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= REG_SIZE_ERR			データサイズ警告	（Ａ．９４Ｄ）	*/
/*			:	= REG_NO_ERR			アドレス異常		（Ａ．９４Ａ）	*/
/****************************************************************************/
INT32 stdCmd_MEM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	LONG		mode, type;
	LONG		adrs;
	LONG		size, accSize, buf;/* <S14D> */
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd	 = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.address;
	ret	 = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address != adrs)
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[1] != pCmdHdl->cmdInfo.cmdBuf.dword[1])) )
	{
		size = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* アクセスするデータサイズ取得 */
		if ( (mode != 1) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;		/* A.94B データ設定異常			*/
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* デバイス情報読み出し用データ設定 */
/* <S14D> */
//			pCmdHdl->cmdInfo.RegData.AccessSize = 1 << (type-1);
//			pCmdHdl->cmdInfo.RegData.Length = size;
			accSize = 1 << (type-1);
			if((adrs >= VMEM_COM_MIN) && (adrs <= VMEM_COM_MAX))
			{/* 共通パラメータ領域	*/
				buf = accSize * size;
				pCmdHdl->cmdInfo.RegData.AccessSize = 4;
				pCmdHdl->cmdInfo.RegData.Length = buf >> 2;	/* bufは、4の整数倍("accSize < 4"でなければ)	*/
				if( accSize < 4 )
				{/* 共通パラメータ領域は、アクセスサイズは4のみ	*/
					ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
				}
			}
			else if(adrs >= VMEM_VEN_MIN)
			{/* ベンダー領域	*/
				buf = accSize * size;
				pCmdHdl->cmdInfo.RegData.AccessSize = 2;
				pCmdHdl->cmdInfo.RegData.Length = buf >> 1;	/* bufは、2の整数倍("accSize < 2"でなければ)	*/
				if( accSize < 2 )
 				{/* ベンダは、アクセスサイズは2,4のみ	*/
					ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
				}
			}
			else if(adrs <= VMEM_ID_MAX)
			{
				buf = accSize * size;
				pCmdHdl->cmdInfo.RegData.AccessSize = 1;
				pCmdHdl->cmdInfo.RegData.Length = buf;
			}
			else
			{
				pCmdHdl->cmdInfo.RegData.AccessSize = accSize;
				pCmdHdl->cmdInfo.RegData.Length = size;
			}
/* <S14D> */
			if(ret == CMD_FUNC_NORMAL_END)
			{
				pCmdHdl->cmdInfo.RegData.Number = adrs;
				pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
				pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata;
				pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata);
			
				/* レジスタコマンド処理要求発行API	*/
				ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->cmdInfo.RegData);
			}
		}
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->cmdInfo.rsltBuf.dword[1] = pCmdHdl->cmdInfo.cmdBuf.dword[1];
		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}
	return ret;
}


/****************************************************************************/
/* 概要		:	ＭＥＭ＿ＷＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= REG_SIZE_ERR			データサイズ警告	（Ａ．９４Ｄ）	*/
/*			:	= REG_NO_ERR			アドレス異常		（Ａ．９４Ａ）	*/
/*			:	= ERR_CMD_CNDTN			コマンド警告(条件外)（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdCmd_MEM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	ULONG		adrs;
	LONG		size;
	LONG		mode, type;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl			= &hNetData->cmd_hdl;

	/* コマンド制御(CMD_CTRL)解析処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, &hNetData->mt_hdl );

	cmd	 = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.address;
	ret  = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address != adrs) 
		||	(pCmdHdl->cmdInfo.rsltBuf.dword[1] != pCmdHdl->cmdInfo.cmdBuf.dword[1])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[0] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[0])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[1] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[1])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[2] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[2])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[3] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[3])
		||	(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[4] != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[4])) )
	{
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->cmdInfo.rsltBuf.dword[1] = pCmdHdl->cmdInfo.cmdBuf.dword[1];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[0] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[0];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[1] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[1];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[2] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[2];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[3] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[3];
		pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata[4] = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.ldata[4];

		size = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->cmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* アクセスするデータサイズ取得 */
		if( (mode < 1) || (mode > 2) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;			/* A.94B データ設定異常 */
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* デバイス情報読み出し用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = 1 << (type-1);
			pCmdHdl->cmdInfo.RegData.Length = size;
			pCmdHdl->cmdInfo.RegData.Number = adrs;
			pCmdHdl->cmdInfo.RegData.EepWrite = (mode-1);
			pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.mem.ldata);
			
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);
		}
		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}
	return ret;
}


/****************************************************************************/
/* 概要		:	ＰＯＳ＿ＳＥＴコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/****************************************************************************/
INT32 stdCmd_POS_SET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	ULONG		pos_set_mod;								/* 座標設定モード */
	UCHAR		refe;										/* レファレンス点 */
	LONG		pos_data;									/* 設定位置 */
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	pos_set_mod	= pCmdHdl->cmdInfo.cmdBuf.stdData.pos_set.pos_set_mod;
	refe		= (UCHAR)( (pos_set_mod & 0x80) >> 7 );
	pos_data	= pCmdHdl->cmdInfo.cmdBuf.stdData.pos_set.pos_data;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	if( (pos_set_mod & 0xf) == 0x00 )
	{	/* 座標系設定=APOSの場合 */
		ret = MciSetCordinatePos( pMtHdl, pos_data, refe, TRUE );
	}
	else
	{	/* 座標系設定!=APOSの場合 */
		ret = DATA_SET_WARNG2;
	}

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.pos_set.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.pos_set.cmd;
	pCmdHdl->cmdInfo.rsltBuf.stdData.pos_set.pos_set_mod = pos_set_mod;
	pCmdHdl->cmdInfo.rsltBuf.stdData.pos_set.pos_data = pos_data;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = ret;

	return ret;
}


/****************************************************************************/
/* 概要		:	ＢＲＫ＿ＯＮコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
INT32 stdCmd_BRK_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＢＫ指令オン */
	pMtHdl->svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* 概要		:	ＢＲＫ＿ＯＦＦコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
INT32 stdCmd_BRK_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＢＫ指令オフ */
	pMtHdl->svCmd->CmdSeqBit |= BRK_RELEASE_BIT;
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

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
INT32 stdCmd_SENS_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	ret			= CMD_FUNC_NORMAL_END;/* <S14D> */

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy == FALSE)
		|| (pCmdHdl->cmdInfo.rsltBuf.stdData.alm.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mem.cmd) )/* <S14D> */
	{
	/* センサオン指令セット */
		ret = MciSetSensOn( pMtHdl, TRUE );
		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;/* <S14D> */
	}

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	return ret;
}


/****************************************************************************/
/* 概要		:	ＳＥＮＳ＿ＯＦＦコマンド解析処理							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/****************************************************************************/
INT32 stdCmd_SENS_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	ret			= CMD_FUNC_NORMAL_END;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* センサオフ指令セット */
	if(pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd)
	{
		ret = MciSetSensOn( pMtHdl, FALSE );
		
		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	return ret;
}


/****************************************************************************/
/* 概要		:	ＳＭＯＮコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/****************************************************************************/
INT32 stdCmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;

	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* 概要		:	ＳＶ＿ＯＮコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdCmd_SV_ON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL				*pCmdHdl;
	MT_HNDL					*pMtHdl;
	BOOL					reset;
	INT32					ret;

	/* ローカル変数の初期化 */
	pCmdHdl			= &hNetData->cmd_hdl;
	pMtHdl			= &hNetData->mt_hdl;
//	ret				= CMD_FUNC_NORMAL_END;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	reset = (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd);
	ret = MciSetServoOn( pMtHdl, TRUE, reset );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = ret;

	return ret;
}


/****************************************************************************/
/* 概要		:	ＳＶ＿ＯＦＦコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/****************************************************************************/
INT32 stdCmd_SV_OFF( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* サーボオフ指令セット */
	ret = MciSetServoOn( pMtHdl, FALSE, TRUE );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = ret;

	return ret;
}


/****************************************************************************/
/* 概要		:	ＩＮＴＥＲＰＯＬＡＴＥコマンド解析処理				(34H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	IPOLATE_CMD_WARNING1						*/
/*				データ異常	:	IPOLATE_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_INTERPOLATE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.TPos		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.Velocity	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.Trq			= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.TrqLimitP	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* 通信ワーニング時の補正処理 */
	if( (hNetData->com_hdl.recvErrCnt == COM_RCV_WRN_STS)
		&& (pMtHdl->crtMtype == MTYPE_INTERPOL) )
	{/* 通信ワーニング && 現在モーション=INTERPOLATE */
		cmd.TPos += pMtHdl->outData.DposIn;
	}

	/* ＩＮＴＥＲＰＯＬＡＴＥコマンドＡＰＩ関数実行 */
	result = MciSetInterpolateCmd( pMtHdl, &cmd );

	/* ビット指令解析処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＳＰＯＳＩＮＧコマンド解析処理						(C0H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	POSING_CMD_WARNING1							*/
/*				データ異常	:	POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_SPOSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;
	BOOL		SratioBit;
	ULONG		svcmd_io;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.TPos	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;
	svcmd_io = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_io;	

	/* 実行条件チェック *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/* S字区間比率(svcmd_io：bit24→1:50%、0:25%)	*/
		SratioBit	= ((svcmd_io >> pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SRATIO].bitNo )
									 & pMtHdl->mtPrm.SvCmdIoOutMap[VENDOR_IOBIT_SRATIO].bitsMsk);
									 
		/* S字時間比率初期化 */
		if( hNetData->reg_hdl.Prm->posing_mode == 0 )
		{/* Pn8FF=0のとき */
			if( SratioBit == 1 )
			{
				cmd.Sratio = S_RATIO1;/* S字加減速時間比率1(%) */
			}
			else
			{
				cmd.Sratio = S_RATIO2;/* S字加減速時間比率2(%) */
			}
		}
		else
		{/* Pn8FF!=0のとき */
			cmd.Sratio = hNetData->reg_hdl.Prm->posing_mode;
		}
		/*------------------*/
		/* 加減速タイプ設定 *//* <S004> */
		/*------------------*/
		if( (cmd.AccRate == 0) && (cmd.DecRate == 0) )
		{/* 加減速度がともにゼロの場合は機器パラメータの加減速度を使う */
			cmd.RateType  = RATE_PRM;					/* 加減速度指令タイプセット */
		}
		else
		{/* 上記以外は通信で指令された加減速度を使う */
			cmd.RateType  = RATE_CMD;					/* 加減速度指令タイプセット */
		}
	
		/* SPOSINGコマンドAPI関数実行 */
		result = MciSetSposingCmd( pMtHdl, &cmd );

	}
	else
	{
		result = DATA_SET_WARNG2;
	}
	
	/* ビット指令解析処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＰＯＳＩＮＧコマンド解析処理						(35H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	POSING_CMD_WARNING1							*/
/*				データ異常	:	POSING_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* SPOSINGモード(Pn8FFで切り換え:0→POSING、0≠SPOSING：) */
//	if( hNetData->reg_hdl.Prm->posing_mode == SPOSING_SEL )
	if( hNetData->reg_hdl.Prm->posing_mode > 0 )
	{
		return stdCmd_SPOSING( hNetData );
	}

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.TPos	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* 実行条件チェック *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/*------------------*/
		/* 加減速タイプ設定 */
		/*------------------*/
		if( (cmd.AccRate == 0) && (cmd.DecRate == 0) )
		{/* 加減速度がともにゼロの場合は機器パラメータの加減速度を使う */
			cmd.RateType  = RATE_PRM;					/* 加減速度指令タイプセット */
		}
		else
		{/* 上記以外は通信で指令された加減速度を使う */
			cmd.RateType  = RATE_CMD;					/* 加減速度指令タイプセット */
		}
	
		/* ＰＯＳＩＮＧコマンドＡＰＩ関数実行 */
		result = MciSetPosingCmd( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* ビット指令解析処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＦＥＥＤコマンド解析処理							(36H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	FEED_CMD_WARNING1							*/
/*				データ異常	:	FEED_CMD_DATA_SET_WARNING2					*/
/****************************************************************************/
INT32 stdCmd_FEED( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* 実行条件チェック *//* <S06D> *//* <S07B>:TSPDのチェックを実施しないように変更 */
	if( ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/*------------------*/
		/* 加減速タイプ設定 *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* 加減速度がともにゼロの場合は機器パラメータの加減速度を使う */
			cmd.RateType  = RATE_PRM;					/* 加減速度指令タイプセット */
		}
		else
		{/* 上記以外は通信で指令された加減速度を使う */
			cmd.RateType  = RATE_CMD;					/* 加減速度指令タイプセット */
		}
		
		/* ＦＥＥＤコマンドＡＰＩ関数実行 */
		result = MciSetFeedCmd( pMtHdl, &cmd );

	}
	else
	{
		result = DATA_SET_WARNG2;
	}
	/* ビット指令解析処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＥＸ＿ＦＥＥＤコマンド解析処理						(37H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	EX_FEED_CMD_WARNING1						*/
/*				データ異常	:	EX_FEED_CMD_DATA_SET_WARNING2				*/
/*				ﾗｯﾁ条件異常	:	LATCH_DATA_SET_WARNG5						*/
/****************************************************************************/
INT32 stdCmd_EX_FEED( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result, wk1, wk2;
	LONG		cmd_ctrl;
	ULONG		svcmd_ctrl;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	svcmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	cmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;

		/* 各データを抽出 */
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* 実行条件チェック *//* <S06D> *//* <S07B>:TSPDのチェックを実施しないように変更 */
	if( ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		cmd.MtLatchReq  = (svcmd_ctrl >> SVCMD_CTRL_LT_REQ1_MAP) & 1;	/* ラッチ要求セット */
		cmd.LatchSgnl   = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;	/* LT_SGNセット */
	
		wk1 = ( (pCmdHdl->cmdInfo.lstCmdCtrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP) ) 
								!= (cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) );
		wk2 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd );
		cmd.RstReq    = ( wk1 || wk2 );							/* モーションリセット要求セット */

		/*------------------*/
		/* 加減速タイプ設定 *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* 加減速度がともにゼロの場合は機器パラメータの加減速度を使う */
			cmd.RateType  = RATE_PRM;					/* 加減速度指令タイプセット */
		}
		else
		{/* 上記以外は通信で指令された加減速度を使う */
			cmd.RateType  = RATE_CMD;					/* 加減速度指令タイプセット */
		}
		
		/* ＥＸ＿ＦＥＥＤコマンドＡＰＩ関数実行 */
		result = MciSetExFeedCmd( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* ビット指令解析処理 */
	/* lstCmdCtrlを指令データ作成処理で利用するため、stdCmdCmdCtrlProc()をここで処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＥＸ＿ＰＯＳＩＮＧコマンド解析処理					(39H)	*/
/****************************************************************************/
/* 引数		:	CMD_HNDL *hdl												*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	EX_POSING_CMD_WARNING1						*/
/*				データ異常	:	EX_POSING_CMD_DATA_SET_WARNING2				*/
/*				ﾗｯﾁ条件異常	:	LATCH_DATA_SET_WARNG5						*/
/****************************************************************************/
INT32 stdCmd_EX_POSING( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	ULONG		svcmd_ctrl;
	ULONG		cmd_ctrl;
	INT32		result, wk1, wk2;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	svcmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	cmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;
	cmd.TPos	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* 実行条件チェック *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		cmd.MtLatchReq  = (svcmd_ctrl >> SVCMD_CTRL_LT_REQ1_MAP) & 1;	/* ラッチ要求セット */
		cmd.LatchSgnl   = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;	/* LT_SGNセット */
	
		wk1 = ( (pCmdHdl->cmdInfo.lstCmdCtrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP) ) 
								!= (cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) );
		wk2 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd );
		cmd.RstReq = ( wk1 || wk2 );							/* モーションリセット要求セット */
	
		/*------------------*/
		/* 加減速タイプ設定 *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* 加減速度がともにゼロの場合は機器パラメータの加減速度を使う */
			cmd.RateType  = RATE_PRM;					/* 加減速度指令タイプセット */
		}
		else
		{/* 上記以外は通信で指令された加減速度を使う */
			cmd.RateType  = RATE_CMD;					/* 加減速度指令タイプセット */
		}
			
		/* ＥＸ＿ＰＯＳＩＮＧコマンドＡＰＩ関数実行 */
		result = MciSetExPosingCmd( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* ビット指令解析処理 */
	/* lstCmdCtrlを指令データ作成処理で利用するため、stdCmdCmdCtrlProc()をここで処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＺＲＥＴコマンド解析処理							(3AH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	ZRET_CMD_WARNING1							*/
/*				ﾃﾞｰﾀ異常	:	ZRET_CMD_DATA_SET_WARNING2					*/
/*				ﾗｯﾁ条件異常	:	LATCH_DATA_SET_WARNG5						*/
/****************************************************************************/
INT32 stdCmd_ZRET( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result, wk1, wk2;
	LONG		cmd_ctrl;
	UCHAR		type;
	ULONG		svcmd_ctrl, mode;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	svcmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.svcmd_ctrl;
	cmd_ctrl	= pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd_ctrl;
	mode		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	type		= (UCHAR)( (mode >> ZRET_MODE_TYPE_MAP) & ZRET_MODE_TYPE_MASK );/* 原点復帰タイプ */

	cmd.MtLatchReq  = (svcmd_ctrl >> SVCMD_CTRL_LT_REQ1_MAP) & 1;	/* ラッチ要求セット */
	cmd.LatchSgnl   = (svcmd_ctrl >> SVCMD_CTRL_LT_SEL1_MAP) & SVCMD_CTRL_LT_SEL1_MASK;	/* LT_SGNセット */
	cmd.TSpeed	= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;

	/* 実行条件チェック *//* <S06D> */
	if( ((ULONG)cmd.TSpeed <= 0x7FFFFFFF)
	    && ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		wk1 = ( (pCmdHdl->cmdInfo.lstCmdCtrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) 
								!= (cmd_ctrl & (CMD_CTRL_CMD_ID_MASK << CMD_CTRL_CMD_ID_MAP)) );
		wk2 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd != pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd );
		cmd.RstReq    = ( wk1 || wk2 );							/* モーションリセット要求セット */
		cmd.HomeDir   = (BOOL)( (mode >> ZRET_MODE_HOME_DIR_MAP) & ZRET_MODE_HOME_DIR_MASK );			/* HOME DIR セット */
	
		/*------------------*/
		/* 加減速タイプ設定 *//* <S004> */
		/*------------------*/
		if((cmd.AccRate == 0) && (cmd.DecRate == 0))
		{/* 加減速度がともにゼロの場合は機器パラメータの加減速度を使う */
			cmd.RateType  = RATE_PRM;					/* 加減速度指令タイプセット */
		}
		else
		{/* 上記以外は通信で指令された加減速度を使う */
			cmd.RateType  = RATE_CMD;					/* 加減速度指令タイプセット */
		}
		/*--------------------*/
		/* 原点復帰タイプ設定 */
		/*--------------------*/
		if( cmd.RstReq )
		{ /* コマンド切り替わり時に原点復帰タイプを取得 */
		  /* ZRET実行中は原点復帰モードを切り替えない */
			pCmdHdl->cmdInfo.homingType = type;	/* 原点復帰タイプを取得 */
		}
	
		/*--------------------*/
		/* 原点復帰API実行    */
		/*--------------------*/
		if( pCmdHdl->cmdInfo.homingType == ZRET_MODE_TYPE_LATCH_MODE )
		{
			/* ＺＲＥＴコマンドＡＰＩ関数（DECなしモード）実行 */
			result = MciSetHomingCmd( pMtHdl, &cmd );
		}
		else
		{
			/* ＺＲＥＴコマンドＡＰＩ関数（DECありモード）実行 */
			result = MciSetHomingCmdWithDec( pMtHdl, &cmd );
		}
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* ビット指令解析処理 */
	/* lstCmdCtrlを指令データ作成処理で利用するため、stdCmdCmdCtrlProc()をここで処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＶＥＬＣＴＲＬコマンド解析処理						(3CH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	VELCTRL_CMD_WARNING1						*/
/*				データ異常	:	VELCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_VELCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.Trq		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;
	cmd.Velocity= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;
	cmd.AccRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data3;
	cmd.DecRate	= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data4;
	cmd.TrqLimitP= (ULONG)pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data5;
	cmd.RateType  = RATE_CMD;										/* 加減速度指令タイプセット *//* <S004> */

	/* 実行条件チェック *//* <S06D> */
	if( ((cmd.AccRate <= 0x7FFFFFFF)||(cmd.AccRate == 0xFFFFFFFF))
	    && ((cmd.DecRate <= 0x7FFFFFFF)||(cmd.DecRate == 0xFFFFFFFF)) )
	{
		/* ＶＥＬＣＴＲＬコマンドＡＰＩ関数実行 */
		result = MciSetVelCmdWithAccr( pMtHdl, &cmd );
	}
	else
	{
		result = DATA_SET_WARNG2;
	}

	/* ビット指令解析処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, TRUE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );	

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＴＲＱＣＴＲＬコマンド解析処理						(3DH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	正常終了	:	CMD_FUNC_NORMAL_END							*/
/*				条件異常	:	TRQCTRL_CMD_WARNING1						*/
/*				ﾃﾞｰﾀ異常	:	TRQCTRL_CMD_DATA_SET_WARNING2				*/
/****************************************************************************/
INT32 stdCmd_TRQCTRL( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	MCI_CMD		cmd;
	INT32		result;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 各データを抽出 */
	cmd.Velocity= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data1;/* TQREFセット		*/
	cmd.Trq		= pCmdHdl->cmdInfo.cmdBuf.stdData.mt.data2;/* VLIMセット 		*/

	/* ＴＲＱＣＴＲＬコマンドＡＰＩ関数実行 */
	result = MciSetTrqCmd( pMtHdl, &cmd );

	/* ビット指令解析処理 */
	/* stdCmdSvCmdCtrlProc()で Mtype, CtrlMode を利用するため、ここで処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->cmdInfo.rsltBuf.stdData.mt.cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.mt.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->cmdInfo.ResultCmdAna = result;

	return result;
}


/****************************************************************************/
/* 概要		:	ＳＶＰＲＭ＿ＲＤコマンド解析処理							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG1		データ設定警告１	（Ａ．９４Ａ）	*/
/*			:	= DATA_SET_WARNG2		データ設定警告２	（Ａ．９４Ｂ）	*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= CMD_WARNG1			コマンド警告１		（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdCmd_SVPRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	UCHAR		cmd;
	ULONG		no,chekNo;/* <S06D>：chekNo追加 */
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	ret	 = CMD_FUNC_NORMAL_END;
	cmd	 = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.cmd;	/* コマンドコード */

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[3] != pCmdHdl->cmdInfo.cmdBuf.dword[3])) ) 
	{

		switch(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* 共通パラメータ(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<2);							/* <S06D>：Dell */
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 				/* <S06D>：Dell */
			chekNo = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>：Add */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add */
			break;
		case 0x10:/* 機器パラメータ(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1);							/* <S06D>：Dell */
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);        							/* <S06D>：Dell */
			chekNo = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>：Add */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add */
			break;
		default:
			chekNo = 0;/* <S14D> */
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}

		/* パラメータ番号、サイズチェック *//* <S06D>：Add */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* レジスタアクセス用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = no;
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata);

			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->cmdInfo.RegData);
		}
		/* 解析結果をバッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.dword[3] = pCmdHdl->cmdInfo.cmdBuf.dword[3];

		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	ＳＶＰＲＭ＿ＷＲコマンド解析処理							*/
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
INT32 stdCmd_SVPRM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	ULONG		no,chekNo;/* <S06D>：chekNo追加 */
	INT32		ret;
	UCHAR		cmd;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ビット指令解析処理 */
	/* コマンド制御(CMD_CTRL)処理 */
	stdCmdCmdCtrlProc( pCmdHdl, &hNetData->alm_hdl, pMtHdl );
	/* サーボコマンド制御フィールド(SVCMD_CTRL)処理 */
	stdCmdSvCmdCtrlProc( pCmdHdl, pMtHdl, FALSE );
	/* サーボコマンド入出力信号(SVCMD_IO)処理 */
	stdCmdSvCmdIoProc( pCmdHdl, pMtHdl );

	ret	 = CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.cmd;

	/* 実行条件チェック */
	if( (pCmdHdl->cmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->cmdInfo.rsltBuf.dword[3] != pCmdHdl->cmdInfo.cmdBuf.dword[3]) 
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[0] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[0])
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[1] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[1])
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[2] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[2])
		||  (pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[3] != pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[3])) )
	{
		switch(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* 共通パラメータ(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<2);							/* <S06D>：Dell */
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 				/* <S06D>：Dell */
			chekNo = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>：Add */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add */
			break;
		case 0x01:/* 共通パラメータ(EEPROM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<2);							/* <S06D>：Dell */
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET)<<1);   							/* <S06D>：Dell */
			chekNo = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>：Add */
			no = VMEM_VEN_MIN + ((chekNo)<<1);  										/* <S06D>：Add */
			break;
		case 0x10:/* 機器パラメータ(RAM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1);							/* <S06D>：Dell */
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1);        							/* <S06D>：Dell */
			chekNo = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>：Add */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add */
			break;
		case 0x11:/* 機器パラメータ(EEPROM) */
//			no = (pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no<<1);							/* <S06D>：Dell */
//			no = no + VMEM_VEN_MIN;                            							/* <S06D>：Dell */
			chekNo = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>：Add */
			no = VMEM_VEN_MIN + ((chekNo)<<1); 											/* <S06D>：Add */
			break;
		default:
			chekNo = 0;/* <S14D> */
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		
		/* パラメータ番号、サイズチェック *//* <S06D>：Add */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* デバイス情報読み出し用データ設定 */
			pCmdHdl->cmdInfo.RegData.AccessSize = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->cmdInfo.RegData.Length = 1;
			pCmdHdl->cmdInfo.RegData.Number = no;
			pCmdHdl->cmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->cmdInfo.RegData.pBuf = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata;
			pCmdHdl->cmdInfo.RegData.BufferSize = sizeof(pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata);

			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->cmdInfo.RegData);
		}

		/* 解析結果をバッファへコピー */
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->cmdInfo.rsltBuf.dword[3] = pCmdHdl->cmdInfo.cmdBuf.dword[3];

		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[0] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[0];
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[1] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[1];
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[2] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[2];
		pCmdHdl->cmdInfo.rsltBuf.stdData.svprm.ldata[3] = pCmdHdl->cmdInfo.cmdBuf.stdData.svprm.ldata[3];

		/* コマンド解析実行結果設定 */
		pCmdHdl->cmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->cmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->cmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/*								サブコマンド								*/
/****************************************************************************/
/****************************************************************************/
/* 概要		:	未定義コマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_WARNG6				コマンド警告６					*/
/****************************************************************************/
INT32 stdExCmd_NO_DEFINE( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* 異常終了を返す */
	pCmdHdl->exCmdInfo.rsltBuf.stdData = pCmdHdl->exCmdInfo.cmdBuf.stdData;
	return CMD_WARNG6;
}


/****************************************************************************/
/* 概要		:	未サポートコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_WARNG2				コマンド警告２					*/
/****************************************************************************/
INT32 stdExCmd_NO_SUPP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* 異常終了を返す */
	pCmdHdl->exCmdInfo.rsltBuf.stdData = pCmdHdl->exCmdInfo.cmdBuf.stdData;
	return CMD_WARNG2;
}


/****************************************************************************/
/* 概要		:	ＮＯＰコマンド解析処理										*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/****************************************************************************/
INT32 stdExCmd_NOP( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	pCmdHdl = &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.stdData.mon.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->exCmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* 概要		:	ＡＬＭ＿ＲＤコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdExCmd_ALM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_rd_mod, alm_idx;
	USHORT		*alm_data;
	INT32		ret;

	/* ローカル変数の初期化 */
	ret			= CMD_FUNC_NORMAL_END;
	pCmdHdl		= &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd			= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.cmd;		/* コマンドコード */
	alm_rd_mod	= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.mode;		/* アラーム読み出しモード */
	alm_idx		= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.index;		/* アラームインデックス番号 */

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy == FALSE) 
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd   != cmd)
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode  != alm_rd_mod)
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.index != alm_idx) )
	{
		alm_data = &pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.data[0];	
		ret = stdCmdAlmRdProc( &hNetData->alm_hdl, alm_rd_mod, alm_idx, alm_data, EX_ALM_RD_NUM );

		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode = alm_rd_mod;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.index = alm_idx;

		/* コマンド解析実行結果設定 */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	ＡＬＭ＿ＣＬＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END		正常終了						*/
/*			:	= CMD_FUNC_CONTINUE			継続実行中						*/
/*			:	= DATA_SET_WARNG2			データ設定警告２（Ａ．９４Ｂ）	*/
/*			:	= CMD_WARNG1				コマンド警告１（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdExCmd_ALM_CLR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	USHORT		alm_clr_mod;
	INT32		ret;

	/* ローカル変数の初期化 */
	ret			= CMD_FUNC_NORMAL_END;
	pCmdHdl		= &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd			= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.cmd;
	alm_clr_mod	= pCmdHdl->exCmdInfo.cmdBuf.stdData.alm.mode;

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)
	    && ((pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd  != cmd) 
	    || (pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode != alm_clr_mod)) )
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
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.alm.mode = alm_clr_mod;

		/* コマンド解析実行結果設定 */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			if( alm_clr_mod == ALM_CLR_MOD0 )
			{
				ret = AlmSetClearReq( &hNetData->alm_hdl, FALSE );
			}
			else //if( alm_clr_mod == ALM_CLR_MOD1 )
			{
				ret = AlmGetAlarmHistoryClearStatus( &hNetData->alm_hdl );
			}
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	ＭＥＭ＿ＲＤコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= REG_SIZE_ERR			データサイズ警告	（Ａ．９４Ｄ）	*/
/*			:	= REG_NO_ERR			アドレス異常		（Ａ．９４Ａ）	*/
/****************************************************************************/
INT32 stdExCmd_MEM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	LONG		mode, type;
	LONG		adrs;
	LONG		size, accSize, buf;/* <S14D> */
	UCHAR		cmd;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd	 = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.address;
	ret	 = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||  (pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address != adrs)
		||  (pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])) )
	{
		size = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* アクセスするデータサイズ取得 */
		if ( (mode != 1) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;		/* A.94B データ設定異常			*/
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{			/* デバイス情報読み出し用データ設定 */
/* <S14D> */
//			pCmdHdl->exCmdInfo.RegData.AccessSize = 1 << (type-1);
//			pCmdHdl->exCmdInfo.RegData.Length = size;
			accSize = 1 << (type-1);
			if((adrs >= VMEM_COM_MIN) && (adrs <= VMEM_COM_MAX))
			{/* 共通パラメータ領域	*/
				buf = accSize * size;
				pCmdHdl->exCmdInfo.RegData.AccessSize = 4;
				pCmdHdl->exCmdInfo.RegData.Length = buf >> 2;	/* bufは、4の整数倍("accSize < 4"でなければ)	*/
				if( accSize < 4 )
				{/* 共通パラメータ領域は、アクセスサイズは4のみ	*/
					ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
				}
			}
			else if(adrs >= VMEM_VEN_MIN)
			{/* ベンダー領域	*/
				buf = accSize * size;
				pCmdHdl->exCmdInfo.RegData.AccessSize = 2;
				pCmdHdl->exCmdInfo.RegData.Length = buf >> 1;	/* bufは、2の整数倍("accSize < 2"でなければ)	*/
				if( accSize < 2 )
 				{/* ベンダは、アクセスサイズは2,4のみ	*/
					ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
				}
			}
			else if(adrs <= VMEM_ID_MAX)
			{
				buf = accSize * size;
				pCmdHdl->exCmdInfo.RegData.AccessSize = 1;
				pCmdHdl->exCmdInfo.RegData.Length = buf;
			}
			else
			{
				pCmdHdl->exCmdInfo.RegData.AccessSize = accSize;
				pCmdHdl->exCmdInfo.RegData.Length = size;
			}
/* <S14D> */
			if(ret == CMD_FUNC_NORMAL_END)
			{
				pCmdHdl->exCmdInfo.RegData.Number = adrs;
				pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
				pCmdHdl->exCmdInfo.RegData.pBuf = &pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata;
				pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata);
				
				/* レジスタコマンド処理要求発行API	*/
				ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->exCmdInfo.RegData);
			}
		}
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];
		/* コマンド解析実行結果設定 */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	ＭＥＭ＿ＷＲコマンド解析処理								*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= REG_SIZE_ERR			データサイズ警告	（Ａ．９４Ｄ）	*/
/*			:	= REG_NO_ERR			アドレス異常		（Ａ．９４Ａ）	*/
/****************************************************************************/
INT32 stdExCmd_MEM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	ULONG		adrs;
	LONG		size;
	LONG		mode, type;
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl			= &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );

	cmd	 = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.cmd;	
	adrs = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.address;
	ret  = CMD_FUNC_NORMAL_END;

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)/* <S1F8> */
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd != cmd)
		||	(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address != adrs) 
		||	(pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])
		||	(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata != pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.ldata)) )
	{
		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.cmd = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.address = adrs;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.ldata;

		size = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.size;
		mode = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode >> 4;
		type = pCmdHdl->exCmdInfo.cmdBuf.stdData.mem.mode & 0x0F;
		/* アクセスするデータサイズ取得 */
		if( (mode < 1) || (mode > 2) || (type < 1) || (type >= 4) )
		{
			ret = DATA_SET_WARNG2;			/* A.94B データ設定異常 */
		}
		else if(size == 0)/* <S14D> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		else
		{
			/* デバイス情報読み出し用データ設定 */
			pCmdHdl->exCmdInfo.RegData.AccessSize = 1 << (type-1);
			pCmdHdl->exCmdInfo.RegData.Length = size;
			pCmdHdl->exCmdInfo.RegData.Number = adrs;
			pCmdHdl->exCmdInfo.RegData.EepWrite = (mode-1);
			pCmdHdl->exCmdInfo.RegData.pBuf = &pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.stdData.mem.ldata);
			
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->exCmdInfo.RegData);
		}
		/* コマンド解析実行結果設定 */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}
	return ret;
}


/****************************************************************************/
/* 概要		:	ＳＭＯＮコマンド解析処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/****************************************************************************/
INT32 stdExCmd_SMON( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;

	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;
	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );
	/* ＣＭＤデータを解析結果バッファへコピー */
	pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = pCmdHdl->exCmdInfo.cmdBuf.stdData.mon.cmd;

	/* コマンド解析実行結果設定 */
	pCmdHdl->exCmdInfo.ResultCmdAna = CMD_FUNC_NORMAL_END;

	return CMD_FUNC_NORMAL_END;
}


/****************************************************************************/
/* 概要		:	ＳＶＰＲＭ＿ＲＤコマンド解析処理							*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/* 戻り値	:	= CMD_FUNC_NORMAL_END	正常終了							*/
/*			:	= CMD_FUNC_CONTINUE		継続実行中							*/
/*			:	= DATA_SET_WARNG1		データ設定警告１	（Ａ．９４Ａ）	*/
/*			:	= DATA_SET_WARNG2		データ設定警告２	（Ａ．９４Ｂ）	*/
/*			:	= DATA_SET_WARNG4		データ設定警告４	（Ａ．９４Ｄ）	*/
/*			:	= CMD_WARNG1			コマンド警告１		（Ａ．９５Ａ）	*/
/****************************************************************************/
INT32 stdExCmd_SVPRM_RD( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	UCHAR		cmd;
	ULONG		no,chekNo;/* <S06D>：chekNo追加 *//* <S1C1> */
	INT32		ret;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );

	ret	= CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.cmd;

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE) 
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])) ) 
	{

		switch(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* 共通パラメータ(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<2);
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 
			chekNo = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;	/* <S06D>：Add *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add *//* <S1C1> */
			break;
		case 0x10:/* 機器パラメータ(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1);
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1); 
			chekNo = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>：Add *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add *//* <S1C1> */
			break;
		default:
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> *//* <S1C0> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		/* パラメータ番号、サイズチェック *//* <S06D>：Add *//* <S1C1> */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* デバイス情報読み出し用データ設定 */
			pCmdHdl->exCmdInfo.RegData.AccessSize = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->exCmdInfo.RegData.Length = 1;
			pCmdHdl->exCmdInfo.RegData.Number = no;
			pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->exCmdInfo.RegData.pBuf = pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata);
			
			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMRD, &pCmdHdl->exCmdInfo.RegData);
		}

		/* ＣＭＤデータを解析結果バッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];

		/* コマンド解析実行結果設定 */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}


/****************************************************************************/
/* 概要		:	ＳＶＰＲＭ＿ＷＲコマンド解析処理							*/
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
INT32 stdExCmd_SVPRM_WR( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	ULONG		no,chekNo;/* <S06D>：chekNo追加 *//* <S1C1> */
	INT32		ret;
	UCHAR		cmd;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;

	/* サブコマンド制御(SUB_CTRL)処理 */
	stdCmdSubCtrlProc( pCmdHdl );

	ret	= CMD_FUNC_NORMAL_END;
	cmd	= pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.cmd;

	/* 実行条件チェック */
	if( (pCmdHdl->exCmdInfo.cmdRdy != FALSE)
		&& ((pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd != cmd) 
		||  (pCmdHdl->exCmdInfo.rsltBuf.dword[1] != pCmdHdl->exCmdInfo.cmdBuf.dword[1])
		||  (pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[0] != pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[0])
		||  (pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[1] != pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[1])) )
	{
		switch(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.mode)
		{
		case 0x00:/* 共通パラメータ(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<2);
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET+RAM_OFFSET)<<1); 
			chekNo = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;		/* <S06D>：Add *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add *//* <S1C1> */		
			break;
		case 0x01:/* 共通パラメータ(EEPROM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<2);
//			no = no + VMEM_VEN_MIN + ((COMM_PRM_OFFSET)<<1); 
			chekNo = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1) + COMM_PRM_OFFSET;		/* <S06D>：Add *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo)<<1); 						/* <S06D>：Add *//* <S1C1> */
			break;
		case 0x10:/* 機器パラメータ(RAM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1);
//			no = no + VMEM_VEN_MIN + ((RAM_OFFSET)<<1); 
			chekNo = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>：Add *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo + RAM_OFFSET)<<1); 							/* <S06D>：Add *//* <S1C1> */
			break;
		case 0x11:/* 機器パラメータ(EEPROM) */
//			no = (pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no<<1);
//			no = no + VMEM_VEN_MIN;
			chekNo = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.no;							/* <S06D>：Add *//* <S1C1> */
			no = VMEM_VEN_MIN + ((chekNo)<<1); 											/* <S06D>：Add *//* <S1C1> */
			break;
		default:
			ret = DATA_SET_WARNG2;
			break;
		}
		if(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size == 0)/* <S16C> *//* <S1C1> */
		{
			ret = DATA_SET_WARNG4;			/* A.94D データ設定警告４ */
		}
		/* パラメータ番号、サイズチェック *//* <S06D>：Add *//* <S1C1> */
		if(chekNo >= 0x1000)
		{
			ret = DATA_SET_WARNG1;
		}

		if( ret == CMD_FUNC_NORMAL_END )
		{
			/* デバイス情報読み出し用データ設定 */
			pCmdHdl->exCmdInfo.RegData.AccessSize = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.size;
			pCmdHdl->exCmdInfo.RegData.Length = 1;
			pCmdHdl->exCmdInfo.RegData.Number = no;
			pCmdHdl->exCmdInfo.RegData.EepWrite = FALSE;
			pCmdHdl->exCmdInfo.RegData.pBuf = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata;
			pCmdHdl->exCmdInfo.RegData.BufferSize = sizeof(pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata);

			/* レジスタコマンド処理要求発行API	*/
			ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_PRMWR, &pCmdHdl->exCmdInfo.RegData);
		}

		/* 解析結果をバッファへコピー */
		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.cmd  = cmd;
		pCmdHdl->exCmdInfo.rsltBuf.dword[1] = pCmdHdl->exCmdInfo.cmdBuf.dword[1];

		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[0] = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[0];
		pCmdHdl->exCmdInfo.rsltBuf.stdData.svprm.ldata[1] = pCmdHdl->exCmdInfo.cmdBuf.stdData.svprm.ldata[1];

		/* コマンド解析実行結果設定 */
		pCmdHdl->exCmdInfo.ResultCmdAna = ret;
	}
	else
	{
		if(pCmdHdl->exCmdInfo.cmdRdy == FALSE)
		{
			ret = RegCheckCommandState(&hNetData->reg_hdl);
			pCmdHdl->exCmdInfo.ResultCmdAna = ret;
		}
	}

	return ret;
}

#if 0
/****************************************************************************/
/* 概要		：指令データのクランプ機能付きゲイン乗算						*/
/****************************************************************************/
/* ULONG inData			入力値（指令データ）								*/
/* LONG  maxData		最大値												*/
/* LONG  kx				ノーマライズ定数									*/
/* LONG* outData		出力値（計算結果）									*/
/****************************************************************************/
/* ret = CMD_FUNC_NORMAL_END  正常終了										*/
/* ret = DATA_SET_WARNG6      データ設定警告(データクランプ動作)			*/
/****************************************************************************/
static INT32 cmdMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )
{
	INT32 ret;

	ret = CMD_FUNC_NORMAL_END;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF のときは通信データを指令 */
		*outData = MlibMulgain32( (LONG)inData, kx );
	}
	else if( inData == 0xFFFFFFFF )
	{/* inData = 0xFFFFFFFF のときは最大値を指令 */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF のときは inData = 0x7FFFFFFF を指令 */
		*outData = MlibMulgain32( 0x7FFFFFFF, kx );
		ret = DATA_SET_WARNG6;
	}
	return ret;
}
#endif

#if 0 /* 現状未対応 */
/****************************************************************************/
/* 概要		:	加速時間のクランプ機能付きゲイン乗算 浮動少数版				*/
/****************************************************************************/
/* ULONG inData			入力値（指令データ）								*/
/* LONG  maxData		最大値												*/
/* LONG  kx				ノーマライズ定数									*/
/* LONG* outData		出力値（計算結果）									*/
/****************************************************************************/
/* ret = CMD_FUNC_NORMAL_END  正常終了										*/
/* ret = DATA_SET_WARNG6      データ設定警告(データクランプ動作)			*/
/****************************************************************************/
static INT32 cmdMulgain32WithClampAccTime_f( ULONG inData, LONG maxData, KSGAIN kx, LONG *outData )
{
	INT32 ret;

	ret = CMD_FUNC_NORMAL_END;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF のときは通信データを指令 */
		*outData = MlibDiv6432_f( (double)kx, (double)1000, (double)inData );			/* ACCRセット */
	}
	else if( inData == 0x0 )
	{/* inData = 0x0 のときは最大値を指令 */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF のときは inData = 0x7FFFFFFF を指令 */
		*outData = MlibDiv6432_f( (double)kx, (double)1000, (double)0x7FFFFFFF );		/* ACCRセット */
		ret = DATA_SET_WARNG6;
	}
	return ret;
}

/****************************************************************************/
/* 概要		：加速時間のクランプ機能付きゲイン乗算							*/
/****************************************************************************/
/* ULONG inData			入力値（指令データ）								*/
/* LONG  maxData		最大値												*/
/* LONG  kx				ノーマライズ定数									*/
/* LONG* outData		出力値（計算結果）									*/
/****************************************************************************/
/* ret = CMD_FUNC_NORMAL_END  正常終了										*/
/* ret = DATA_SET_WARNG6      データ設定警告(データクランプ動作)			*/
/****************************************************************************/
static INT32 cmdMulgain32WithClampAccTime( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )
{
	INT32 ret;
	
	ret = CMD_FUNC_NORMAL_END;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF のときは通信データを指令 */
		*outData = MlibDiv6432( kx, 1000, inData );	/* ACCRセット */
	}
	else if( inData == 0x0 )
	{/* inData = 0x0 のときは最大値を指令 */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF のときは inData = 0x7FFFFFFF を指令 */
		*outData = MlibDiv6432( kx, 1000, 0x7FFFFFFF );	/* ACCRセット */
		ret = DATA_SET_WARNG6;
	}
	return ret;
}

#endif /* #if 0 */




