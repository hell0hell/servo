/****************************************************************************
Description	: MECHATROLINK Response Maker (Standard Servo Profile)
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
#include "SysApi.h"
#include "MtApi.h"
#include "MLib.h"
//#include "M3StdRspProc.h"



/****************************************************************************
 Defines for Response function
****************************************************************************/
/* 共通モニタ defines */
#define POS_CTRL 		1
#define SEL_APOS 		0
#define SEL_MPOS 		1
#define SEL_ALM_DTL		8
#define SEL_CMN1		0xC
#define SEL_CMN2		0xD

/* 共通モニタ SV_STAT */
#define PHASE_MASK		0xFF
#define CTRLMODE_MASK	0xFF
#define LTRDY1_MASK		1
#define LTRDY2_MASK		1
#define LTSEL1R_MASK	3
#define LTSEL2R_MASK	3
#define CTRLMODE_MAP	8
#define LTRDY1_MAP		24
#define LTRDY2_MAP		25
#define LTSEL1R_MAP		26
#define LTSEL2R_MAP		28



static	LONG stdRspGetMonitorData( MLNET_HANDLE *hNetData, LONG sel_mon );
static	ULONG stdRspMakeSVCMD_STAT( MLNET_HANDLE *hNetData, CMD_LINK_DATA *rslt_wk );
static	ULONG stdRspMakeSVCMD_IO( MT_HNDL *pMtHdl );



/****************************************************************************/
/* 概要		:	ステータス処理												*/
/****************************************************************************/
/* 引数		:	CMD_HNDL* pCmdHdl			CMD ANALYZER のハンドル			*/
/*			:	ALM_HNDL* pAlmHdl			ALM Manager のハンドル			*/
/*			:	CMD_LINK_DATA* rslt_wk		解析結果バッファのポインタ		*/
/* 戻り値	:	CMD_STAT ビット情報											*/
/****************************************************************************/
USHORT stdRspMakeCMD_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, USHORT sts )
{
	/* 解析結果バッファのCMD_CTRLをコピー */
	sts = sts & RSP_CLR_CMDSTAT_BITS;

	/* bit0 (D_ALM) */
	sts |= (pAlmHdl->M3StdAlm.DevAlm << RSP_DEV_ALM_BIT_NO);
	/* bit1 (D_WAR) */
	sts |= (pAlmHdl->M3StdAlm.DevWrn << RSP_DEV_WRN_BIT_NO);
	/* bit2 (CMDRDY) */
	sts |= (pCmdHdl->cmdInfo.cmdRdy << RSP_CMD_READY_BIT_NO);
	/* bit3 (ALMCLR) */
	sts |= (pCmdHdl->cmdInfo.almClrCmp << RSP_ALMCLR_CMP_BIT_NO);
	/* bit8～11 (CMD_ALM) */
	sts |= (USHORT)(pAlmHdl->CmdAlmCode << RSP_CMD_ALM_BIT_NO);
	/* bit12～15(COMM_ALM) */
	sts |= (USHORT)(pAlmHdl->CommAlmCode << RSP_COMM_ALM_BIT_NO);

	return sts;
}



/****************************************************************************/
/* 概要		:	サーボコマンドステータスフィールド処理						*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	CMD_LINK_DATA* rslt_wk		解析結果バッファのポインタ		*/
/* 戻り値	:	SVCMD_STAT ビット情報										*/
/****************************************************************************/
static	ULONG stdRspMakeSVCMD_STAT( MLNET_HANDLE *hNetData, CMD_LINK_DATA *rslt_wk )
{
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	MT_HNDL			*pMtHdl;
	RSP_SVCMD_STAT	sts;
	BOOL			wk;
	LONG			RspSeqBit;
	BOOL			zspd;

	/* ローカル変数の初期化 */
 	pRspHdl		= &hNetData->rsp_hdl;
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* 解析結果バッファのCMD_CTRLをコピー */
	sts.RspSvCmdStat = rslt_wk->stdData.mt.svcmd_ctrl & 0xFFFF0000;

	/*----------------------------------------*/
	/* サーボコマンドステータスフィールド設定 */
	/*----------------------------------------*/
	zspd = ~(pMtHdl->svRsp->RspCtrl.RspCtrlBit >> MOTOR_MOVING_BITNO) & 1;

	/* bit0(CMD_PAUSE_CMP) */
	wk = pCmdHdl->cmdInfo.cmdPauseEnable & zspd;
	sts.RspSvCmdStat |= (wk << RSP_CMD_PAUSE_CMP_BITNO);

	/* bit1(CMD_CANCEL_CMP) */
	if (pMtHdl->cmdData.CtrlMode == MT_POS_CTRLMD)
	{	wk = ( pCmdHdl->cmdInfo.cmdCancelEnable & pMtHdl->psnHdl.Den );}
	else
	{	wk = ( pCmdHdl->cmdInfo.cmdCancelEnable & zspd );}
	sts.RspSvCmdStat |= (wk << RSP_CMD_CANCEL_CMP_BITNO);

	/* bit2～3(Reserve) */
	/* bit4～7(ACCFIL) */
	sts.RspSvCmdStat |= (pMtHdl->acfHdl.filSw << RSP_CMD_ACC_FILNO);

	/* bit8(L_CMP1) */
	sts.RspSvCmdStat |= (pRspHdl->last_L_CMP[0] << RSP_CMD_LCOMP1_BITNO);

	/* bit9(L_CMP2) */
	sts.RspSvCmdStat |= (pRspHdl->last_L_CMP[1] << RSP_CMD_LCOMP2_BITNO);

	/* bit10(POS_RDY) */
	if( (pMtHdl->mtState >= MSTAT_READY) 
		&& (0 != (pMtHdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK))
		&& (FALSE == hNetData->alm_hdl.PosDataNG) )
	{
		sts.RspSvCmdStat |= (1 << RSP_CMD_POS_READY_BITNO);
	}

	/* bit11(PON) */
	RspSeqBit = pMtHdl->svRsp->RspSeqBit;
	wk = (RspSeqBit >> MAIN_POWER_ON_BITNO) & 1;
	sts.RspSvCmdStat |= (wk << RSP_CMD_POWER_ON_BITNO);

	/* bit12(M_RDY) */
	wk = (RspSeqBit >> MOTOR_READY_BITNO) & 1;
	sts.RspSvCmdStat |= (wk << RSP_CMD_MOTOR_READY_BITNO);

	/* bit13(SV_ON) */
	wk = (RspSeqBit >> SERVO_ON_COMP_BITNO) & 1;
	sts.RspSvCmdStat |= (wk << RSP_CMD_SERVO_ON_BITNO);

	/* bit14～15 (Reserve) */

	return sts.RspSvCmdStat;

}


/****************************************************************************/
/* 概要		:	サーボコマンド入出力信号モニタ処理							*/
/****************************************************************************/
/* 引数		:	MT_HNDL *pMtHdl												*/
/* 戻り値	:	SVCMD_IO ビット情報											*/
/****************************************************************************/
static	ULONG stdRspMakeSVCMD_IO( MT_HNDL *pMtHdl )
{
	RSP_SVCMD_IO	sts;					/* SVCMD_IOビット情報 */
	PSN_HNDL		*psn_hdl_ptr;
	ULONG			RspCtrlBit,RspIoBit;
	BOOL			bitwk;

/* <S0AC>start */
	UCHAR	ucMonitorBit;		/* SI0～SI6の全ビット設定するまでループ */
	UCHAR	ucTmp;				/* Hi側 ON=1にて、SI0～SI6取得する */
	UCHAR	wk;
/* <S0AC>end */	
	
	psn_hdl_ptr		= &pMtHdl->psnHdl;

	RspIoBit = pMtHdl->svRsp->RspSigBit;
	/* bit1(DEC) */
	sts.RspCmdIoBit = ((RspIoBit >> DEC_SIG_BITNO) & 1) << RSP_CMD_DEC_BITNO;
	/* bit2(POT) */
	sts.RspCmdIoBit |= (((RspIoBit >> P_OT_SIG_BITNO) & 1) << RSP_CMD_POT_BITNO);
	/* bit3(NOT) */
	sts.RspCmdIoBit |= (((RspIoBit >> N_OT_SIG_BITNO) & 1) << RSP_CMD_NOT_BITNO);
	/* bit4(EXT1)-bit6(EXT3) */
	sts.RspCmdIoBit |= (((RspIoBit >> EXT1_SIG_BITNO) & 0x7) << RSP_CMD_EXT1_BITNO);
	/* bit7(ESTP) */
	sts.RspCmdIoBit |= (((RspIoBit >> HWBB_SIG_BITNO) & 3) << RSP_CMD_ESTOP_BITNO);

	/* bit8(Reserve) */

	/* bit9(BRK_ON) */
	bitwk = (~(pMtHdl->svRsp->RspSeqBit) >> BRK_RELEASING_BITNO) & 0x01;
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_BRK_ON_BITNO);

	/* bit10(P-SOT) */
	bitwk = psn_hdl_ptr->SoftLimit & 1;
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_PSOFT_LMT_BITNO);

	/* bit11(N-SOT) */
	bitwk = (psn_hdl_ptr->SoftLimit >> 1) & 1;
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_NSOFT_LMT_BITNO);

#if 0 /* 2011.01.05 Y.Oka PsnUpdateFBData()でのPset,Near作成処理追加により見直し */
	/* bit12(DEN), bit14(PSET) */
	pos_err	= MlibLABS( psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos );
	if( psn_hdl_ptr->Den )
	{	
		sts.RspCmdIoBit |= RSP_CMD_DEN_BIT;
		if(pos_err <= psn_hdl_ptr->Prm->psetWidth)
		{ /* DEN=1 && (Tpos-Apos)<=位置決め完了幅 */
			sts.RspCmdIoBit |= RSP_CMD_PSET_BIT;
		}
	}

	/* bit13(NEAR) */
	if( pos_err <= psn_hdl_ptr->Prm->nearWidth )
	{ /* (Tpos-Apos)<=NEAR幅 */
		sts.RspCmdIoBit |= RSP_CMD_NEAR_BIT;
	}
#endif /* #if 0 */

	/* bit12(DEN) */
	sts.RspCmdIoBit |= (psn_hdl_ptr->Den * RSP_CMD_DEN_BIT);

	/* bit13(NEAR) */
	sts.RspCmdIoBit |= (psn_hdl_ptr->Near * RSP_CMD_NEAR_BIT);

	/* bit14(PSET) */
	sts.RspCmdIoBit |= (psn_hdl_ptr->Pset * RSP_CMD_PSET_BIT);

	/* bit15(ZPOINT) */
	if( (psn_hdl_ptr->RefEnable != FALSE) 
		&& ((ULONG)MlibLABS(psn_hdl_ptr->Apos) <= psn_hdl_ptr->Prm->zPointWidth) )
	{
		sts.RspCmdIoBit |= RSP_CMD_ZPOINT_BIT;
	}

	/* bit16(T_LIM) */
	RspCtrlBit = pMtHdl->svRsp->RspCtrl.RspCtrlBit;
	bitwk = (RspCtrlBit >> TRQ_CLAMP_BITNO) & 1;
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_TRQ_LMT_BITNO);
	
	/* bit17(V_LIM) */
	bitwk = (RspCtrlBit >> SPD_CLAMP_BITNO) & 1;
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_VEL_LMT_BITNO);

	/* bit18(V_CMP) */
	bitwk = (RspCtrlBit >> REACH_SPD_BITNO) & 1;
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_VEL_CMP_BITNO);

	/* bit19(ZSPD) */
	bitwk = ~(RspCtrlBit >> MOTOR_MOVING_BITNO) & 1;	/* <S028> : 出力信号が反転していたので修正 */
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_ZSPD_BITNO);

	/* bit20～23(Reserve) */
	/* bit24～31(Vender Specific I/O) */

// #if 0/* <S0AC>start */
	/*------------------------------------------------------------------------------*/
	/* IO_STS1 24bit目																*/
	/* Pn860～Pn866にSI0～SI6が割りあたっていた場合に、出力ビット先を設定			*/
	/* 2軸構成時は、SI01～SI06/SI10～SI16の割り付けを出力する						*/
	/* Safetyに対応した場合は、wkにデータをOR設定する								*/
	/*------------------------------------------------------------------------------*/

	/* wkの初期化 */
	wk = 0;

	if( pMtHdl->mtPrm.InSigMonM3.bitsMsk )
	{ // 1dataもenableでなければIO OFFにて終了
//		for( ucMonitorBit=0; ucMonitorBit < 7; ucMonitorBit++ )
		for( ucMonitorBit=0; ucMonitorBit < INPUT_SIGMON_BIT_NUM; ucMonitorBit++ )/* <S1C0> */
		{
			/* それぞれのIOがPn設定よりイネーブルの時だけINPUTデータを取得する */
			if( (pMtHdl->mtPrm.InSigMonM3.bitsMsk >> ucMonitorBit) & 0x01 )
			{
				ucTmp = ((~(pMtHdl->svRsp->Mon7)) >> ucMonitorBit) & 0x0001;
				wk |= ucTmp << pMtHdl->mtPrm.InSigMonM3.bitNo[ucMonitorBit];
			}
		}
	}

//#if( CCSW_SFTID == SFTID_EXIST )/* <S1C0> *//* <S1F9> */
	if(pMtHdl->mtPrm.OutSigMonM3.bitsMsk)
	{
		for( ucMonitorBit=0; ucMonitorBit < OUTPUT_SIGMON_BIT_NUM; ucMonitorBit++ )
		{
			/* それぞれのIOがPn設定よりイネーブルの時だけOUTPUTデータを取得する */
			if( (pMtHdl->mtPrm.OutSigMonM3.bitsMsk >> ucMonitorBit) & 0x01 )
			{
				ucTmp = (UCHAR) ( ((~(pMtHdl->svRsp->Mon7 >> OUTPUT_SIGMON_BIT_SFT)) >> ucMonitorBit) & 0x0001);
				wk |= ucTmp << pMtHdl->mtPrm.OutSigMonM3.bitNo[ucMonitorBit];
			}
		}
	}
//#endif/* <S1F9> */

	sts.RspCmdIoBit |= (wk << RSP_CMD_IOSTS1_BITNO);
	
// #endif/* <S0AC>end */

	return sts.RspCmdIoBit;
}


/****************************************************************************/
/* 概要		:	サブコマンドステータス処理									*/
/****************************************************************************/
/* 引数		:	CMD_HNDL* pCmdHdl			CMD Managerハンドル				*/
/*			:	ALM_HNDL* pAlmHdl			ALM Managerハンドル				*/
/*			:	CMD_LINK_EXDATA* rslt_wk	解析結果バッファのポインタ		*/
/* 戻り値	:	SUB_STAT ビット情報											*/
/****************************************************************************/
ULONG stdRspMakeSUB_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, ULONG sts )
{
	ULONG	ex_cmd_alm;				/* SUBCMD_ALM */
	ULONG	sub_stat;
	
	sub_stat = sts & RSP_SUBCMD_CLR_BITS_MSK;

	/* bit2 (SUBCMDRDY) */
	sub_stat |= (pCmdHdl->exCmdInfo.cmdRdy << RSP_SUBCMD_READY_BIT_NO);

	/* bit8～11 (SUBCMD_ALM) */
	ex_cmd_alm  = pAlmHdl->ExCmdAlmCode & 0x0F;
	sub_stat |= (ex_cmd_alm << RSP_SUBCMD_ALM_BIT_NO);

	return sub_stat;
}


/****************************************************************************/
/*					 メインコマンドレスポンス作成処理						*/
/****************************************************************************/
/****************************************************************************/
/* 概要		:	システムコマンドレスポンス処理						(??H)	*/
/****************************************************************************/
/* 対応コマンド : 未定義/MEM_WR/											*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_SYS_CMD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->dword[4] = rslt_wk->dword[4];
	rsp_buf->dword[5] = rslt_wk->dword[5];
	rsp_buf->dword[6] = rslt_wk->dword[6];
	rsp_buf->dword[7] = rslt_wk->dword[7];
}

/****************************************************************************/
/* 概要		:	ID_RDコマンドレスポンス処理							(03H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_ID_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[1] = rslt_wk->dword[1];
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
		rsp_buf->dword[4] = rslt_wk->dword[4];
		rsp_buf->dword[5] = rslt_wk->dword[5];
		rsp_buf->dword[6] = rslt_wk->dword[6];
		rsp_buf->dword[7] = rslt_wk->dword[7];
	}
	else
	{
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
		rsp_buf->dword[4] = 0;
		rsp_buf->dword[5] = 0;
		rsp_buf->dword[6] = 0;
		rsp_buf->dword[7] = 0;
	}
}

/****************************************************************************/
/* 概要		:	MEM_RDコマンドレスポンス処理						(1DH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_MEM_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[3] = rslt_wk->dword[3];
		rsp_buf->dword[4] = rslt_wk->dword[4];
		rsp_buf->dword[5] = rslt_wk->dword[5];
		rsp_buf->dword[6] = rslt_wk->dword[6];
		rsp_buf->dword[7] = rslt_wk->dword[7];
	}
	else
	{
		rsp_buf->dword[3] = 0;
		rsp_buf->dword[4] = 0;
		rsp_buf->dword[5] = 0;
		rsp_buf->dword[6] = 0;
		rsp_buf->dword[7] = 0;
	}
}

/****************************************************************************/
/* 概要		:	NOP,SYNC_SET,DISCONNECT レスポンス処理	(00/0D/0FH)			*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_NOP( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->dword[4] = 0;
	rsp_buf->dword[5] = 0;
	rsp_buf->dword[6] = 0;
	rsp_buf->dword[7] = 0;
}


/****************************************************************************/
/* 概要		:	ＣＯＮＦＩＧコマンドレスポンス処理					(04H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_CONFIG( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->stdData.config.config_mod;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->dword[4] = 0;
	rsp_buf->dword[5] = 0;
	rsp_buf->dword[6] = 0;
	rsp_buf->dword[7] = 0;
}

/****************************************************************************/
/* 概要		:	ＡＬＭ＿RDコマンドレスポンス処理					(05H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_ALM_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[1] = rslt_wk->dword[1];

	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[2] = rslt_wk->stdData.alm.data[0]
						  | (rslt_wk->stdData.alm.data[1] << 16);
		rsp_buf->dword[3] = rslt_wk->stdData.alm.data[2]
						  | (rslt_wk->stdData.alm.data[3] << 16);
		rsp_buf->dword[4] = rslt_wk->stdData.alm.data[4]
						  | (rslt_wk->stdData.alm.data[5] << 16);
		rsp_buf->dword[5] = rslt_wk->stdData.alm.data[6]
						  | (rslt_wk->stdData.alm.data[7] << 16);
		rsp_buf->dword[6] = rslt_wk->stdData.alm.data[8]
						  | (rslt_wk->stdData.alm.data[9] << 16);
		rsp_buf->dword[7] = 0;
	}
	else
	{
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
		rsp_buf->dword[4] = 0;
		rsp_buf->dword[5] = 0;
		rsp_buf->dword[6] = 0;
		rsp_buf->dword[7] = 0;
	}
}

/****************************************************************************/
/* 概要		:	ＡＬＭ＿ＣＬＲコマンドレスポンス処理				(06H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_ALM_CLR( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->stdData.alm.mode;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->dword[4] = 0;
	rsp_buf->dword[5] = 0;
	rsp_buf->dword[6] = 0;
	rsp_buf->dword[7] = 0;
}


/****************************************************************************/
/* 概要		:	ＣＯＮＮＥＣＴコマンドレスポンス処理				(0EH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_CONNECT( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->dword[4] = 0;
	rsp_buf->dword[5] = 0;
	rsp_buf->dword[6] = 0;
	rsp_buf->dword[7] = 0;
	rsp_buf->dword[1] = (  rslt_wk->stdData.connect.ver
						| (rslt_wk->stdData.connect.com_mod << 8)
						| (rslt_wk->stdData.connect.com_time << 16)
						| (rslt_wk->stdData.connect.profile_type << 24) );
}


/****************************************************************************/
/* 概要		:	ＰＯＳ＿ＳＥＴコマンドレスポンス処理				(20H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_POS_SET( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_HNDL		*pCmdHdl;
	CMD_LINK_DATA	*rslt_wk;
	LONG			sel_mon1, sel_mon2, sel_mon3;

	/* ローカル変数の初期化 */
	pCmdHdl		= &hNetData->cmd_hdl;
	rslt_wk		= &( pCmdHdl->cmdInfo.rsltBuf );

	/* ステータス設定 */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( &hNetData->mt_hdl );

	/* POS_SET_MOD, POS_DATA コピー */
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->dword[4] = rslt_wk->dword[4];

	/* モニタコード1,2,3抽出 */
	/* SEL_MON1を抽出 */
	sel_mon1 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16 ) & 0xF;
	/* SEL_MON2を抽出 */
	sel_mon2 = ( (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16) & 0xF0 ) >> 4;
	/* SEL_MON3を抽出 */
	sel_mon3 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 24 ) & 0xF;

	/* モニタデータ1,2,3設定 */
	rsp_buf->stdData.mt.mon1 = stdRspGetMonitorData( hNetData, sel_mon1 );
	rsp_buf->stdData.mt.mon2 = stdRspGetMonitorData( hNetData, sel_mon2 );
	rsp_buf->stdData.mt.mon3 = stdRspGetMonitorData( hNetData, sel_mon3 );
}

/****************************************************************************/
/* 概要		:	SVPRM_RDコマンドレスポンス処理						(40H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_SVPRM_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk		= &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* ステータス設定 */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( &hNetData->mt_hdl );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[3] = rslt_wk->dword[3];
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[4] = rslt_wk->dword[4];
		rsp_buf->dword[5] = rslt_wk->dword[5];
		rsp_buf->dword[6] = rslt_wk->dword[6];
		rsp_buf->dword[7] = rslt_wk->dword[7];
	}
	else
	{
		rsp_buf->dword[4] = 0;
		rsp_buf->dword[5] = 0;
		rsp_buf->dword[6] = 0;
		rsp_buf->dword[7] = 0;
	}
}

/****************************************************************************/
/* 概要		:	SVPRM_WRコマンドレスポンス処理						(41H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_SVPRM( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk		= &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* ステータス設定 */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( &hNetData->mt_hdl );

	/* CMD Analyzerの解析結果バッファのデータをRSP格納用バッファへ貼り付け */
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->dword[4] = rslt_wk->dword[4];
	rsp_buf->dword[5] = rslt_wk->dword[5];
	rsp_buf->dword[6] = rslt_wk->dword[6];
	rsp_buf->dword[7] = rslt_wk->dword[7];
}


/****************************************************************************/
/* 概要		:	モニタ有りコマンドレスポンス処理					(**H)	*/
/*				BRK_ON/OFF, SENS_ON/OFF, HOLD, LTMOD_ON/OFF, SMON, SV_ON/OFF*/
/*				INTERPORATE, POSING, FEED, LATCH, EX_POSING, ZRET, VELCTRL	*/
/*				TRQCTRL, SVCTRL												*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdRsp_MON_CMD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	MT_HNDL			*pMtHdl;
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	CMD_LINK_DATA	*rslt_wk;
	UCHAR			sel_mon1, sel_mon2, sel_mon3;	/* モニタ選択情報 */

	/* ローカル変数の初期化 */
	pMtHdl		= &hNetData->mt_hdl;
	pRspHdl		= &hNetData->rsp_hdl;
	pCmdHdl		= &hNetData->cmd_hdl;
	rslt_wk		= &( pCmdHdl->cmdInfo.rsltBuf );

	/* ステータス設定 */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( pMtHdl );

	/* モニタコード1,2,3抽出 */
	/* SEL_MON1を抽出 */
	sel_mon1 = (UCHAR)( (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16) & 0xF );
	/* SEL_MON2を抽出 */
	sel_mon2 = (UCHAR)( ((pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16) & 0xF0) >> 4 );
	/* SEL_MON3を抽出 */
	sel_mon3 = (UCHAR)( (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 24) & 0xF );

	/* 固定モニタデータ設定 */
	rsp_buf->stdData.mt.cprm_mon1 = 
		stdRspGetMonitorData( hNetData, pRspHdl->cPrmMon[0] );
	rsp_buf->stdData.mt.cprm_mon2 = 
		stdRspGetMonitorData( hNetData, pRspHdl->cPrmMon[1] );
	/* モニタデータ1,2,3設定 */
	rsp_buf->stdData.mt.mon1 = stdRspGetMonitorData( hNetData, sel_mon1 );
	rsp_buf->stdData.mt.mon2 = stdRspGetMonitorData( hNetData, sel_mon2 );
	rsp_buf->stdData.mt.mon3 = stdRspGetMonitorData( hNetData, sel_mon3 );
}


/****************************************************************************/
/* 概要		:	モニタデータ取得処理										*/
/*				SEL_MON で設定されたデータを取得する						*/
/****************************************************************************/
/* 引数		:	COM_HNDL *pComHdl											*/
/*			:	MT_HNDL *pMtHdl												*/
/*			:	ALM_HNDL *pAlmHdl											*/
/*			:	UCHAR sel_mon		モニタ選択情報							*/
/* 戻り値	:	モニタデータ												*/
/****************************************************************************/
static	LONG stdRspGetMonitorData( MLNET_HANDLE *hNetData, LONG sel_mon )
{
	MT_HNDL		*pMtHdl;
	LONG		*ltSelR;
	LONG		m3_selmon1;
	LONG		m3_selmon2;
	LONG		ret;

	/* ローカル変数の初期化 */
	pMtHdl		= &hNetData->mt_hdl;
	ltSelR		= hNetData->cmd_hdl.cmdInfo.ltSelR;
	m3_selmon1	= pMtHdl->mtPrm.BasePrm.m3_selMon1;
	m3_selmon2	= pMtHdl->mtPrm.BasePrm.m3_selMon2;

	if( sel_mon == SEL_ALM_DTL )
	{
		ret = hNetData->alm_hdl.errCode;
	}
	else if( ((sel_mon == SEL_CMN1) && (m3_selmon1 == SV_STAT_MON))
	    	|| ((sel_mon == SEL_CMN2) && (m3_selmon2 == SV_STAT_MON)) )
	{/* 共通モニタのSV_STAT情報は、いろいろなモジュールの情報を参照するのでここで作成 */
		ret   = ( hNetData->com_hdl.Phase & PHASE_MASK );
		ret  |= ( (pMtHdl->cmdData.CtrlMode-1) & CTRLMODE_MASK ) << CTRLMODE_MAP;
		ret  |= ( MciGetLtRdy(pMtHdl, 0) & LTRDY1_MASK ) << LTRDY1_MAP;
		ret  |= ( MciGetLtRdy(pMtHdl, 1) & LTRDY2_MASK ) << LTRDY2_MAP;
		ret  |= ( ltSelR[0] & LTSEL1R_MASK ) << LTSEL1R_MAP;
		ret  |= ( ltSelR[1] & LTSEL2R_MASK ) << LTSEL2R_MAP;
	}
	else
	{
		ret = MciGetM3StdMonitorData( pMtHdl, sel_mon );
	}
	return ret;
}


/****************************************************************************/
/*					 サブコマンドレスポンス作成処理							*/
/****************************************************************************/
/****************************************************************************/
/* 概要		:	システムコマンドレスポンス処理						(??H)	*/
/****************************************************************************/
/* 対応コマンド : 未定義/MEM_WR/SVPRM_WR									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData			多軸用ハンドル統括構造体	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_SYS_CMD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
}


/****************************************************************************/
/* 概要		:	NOPコマンドレスポンス処理							(00H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_NOP( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}

/****************************************************************************/
/* 概要		:	ＡＬＭ＿RDコマンドレスポンス処理					(05H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP格納用バッファポインタ		*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_ALM_RD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA	*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->stdData.alm.mode;

	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[2] = rslt_wk->stdData.alm.data[0]
						  | (rslt_wk->stdData.alm.data[1] << 16);
		rsp_buf->dword[3] = rslt_wk->stdData.alm.data[2]
						  | (rslt_wk->stdData.alm.data[3] << 16);
	}
	else
	{
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}
}

/****************************************************************************/
/* 概要		:	ALM_CLRコマンドレスポンス処理						(06H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData		多軸用ハンドル統括構造体		*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_ALM_CLR( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->stdData.alm.mode;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}


/****************************************************************************/
/* 概要		:	SMON コマンドレスポンス処理									*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData			多軸用ハンドル統括構造体	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_MON_CMD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_HNDL	*pCmdHdl;
	LONG		sel_mon4,sel_mon5,sel_mon6;	/* モニタ選択情報 */

	/* ローカル変数の初期化 */
	pCmdHdl	= &hNetData->cmd_hdl;

	/* モニタコード4,5,6抽出 */
	/* SEL_MON4を抽出 */
	sel_mon4 = (pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl1 & 0xF0) >> 4;
	/* SEL_MON5を抽出 */
	sel_mon5 = pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl2 & 0xF;
	/* SEL_MON6を抽出 */
	sel_mon6 = (pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl2 & 0xF0) >> 4;

	/* モニタ4,5,6データ取得 */
	rsp_buf->stdData.mon.mon4 = stdRspGetMonitorData( hNetData, sel_mon4 );
	rsp_buf->stdData.mon.mon5 = stdRspGetMonitorData( hNetData, sel_mon5 );
	rsp_buf->stdData.mon.mon6 = stdRspGetMonitorData( hNetData, sel_mon6 );

}

/****************************************************************************/
/* 概要		:	ME_RDコマンドレスポンス処理							(1DH)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData			多軸用ハンドル統括構造体	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_MEM_RD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->dword[3] = 0;
	}
}

/****************************************************************************/
/* 概要		:	SVPRM_RDコマンドレスポンス処理						(40H)	*/
/****************************************************************************/
/* 引数		:	MLNET_HANDLE* hNetData			多軸用ハンドル統括構造体	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP格納用バッファポインタ	*/
/* 戻り値	:	なし														*/
/****************************************************************************/
VOID stdExRsp_SVPRM_RD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ローカル変数の初期化 */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->dword[1];
	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{
		rsp_buf->dword[2] = rslt_wk->dword[2];
		rsp_buf->dword[3] = rslt_wk->dword[3];
	}
	else
	{
		rsp_buf->dword[2] = 0;
		rsp_buf->dword[3] = 0;
	}
}



