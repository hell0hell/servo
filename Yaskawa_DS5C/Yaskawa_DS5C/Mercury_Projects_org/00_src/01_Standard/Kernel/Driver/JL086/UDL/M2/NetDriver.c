/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK サーボ通信ＩＦアプリ 通信コンポーネント		    */
/*                                                                          */
/*            JL-076 通信ドライバ	JL-076 NetWortk Driver					*/
/*																			*/
/****************************************************************************/
/* 定義関数 :                                                               */
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*		Ver1.01		JL-052不具合対策										*/
/*                  レジスタEC0にISRS=5をセット(WR0*からPRETXを出力し、		*/
/*                  この期間はH/WでラインレシーバをDisableする)	<V580>		*/
/*                                                                          */
/*******************  2006 Yaskawa Electric Corporation *********************/

/* includes */
#include "ComCfg.h"
#include "NetDriver.h"
//#include "NetMsgDrv.h"
#include "jlComdrv.h"
#include "jl077def.h"


typedef struct tagMLDRV_CHINFO {
	UCHAR*		hif_reg_top;	/* JL086 Servo UDL Address 				*/
	MLMSG_INFO	msgInfo[2];		/* メッセージCHInfo 					*/
	ND_PARAM	nd_prm;			/* 通信パラメータ						*/
	USHORT		errSts;			/* エラーステータス						*/

	VUSHORT		*pRegSTS;		/* ASICステータスレジスタへのポインタ	*/
	UCHAR		*pRegRxD;		/* ASIC受信バッファへのポインタ			*/
	UCHAR		*pRegTxD;		/* ASIC送信バッファへのポインタ			*/
	UCHAR		*pRegSFD;		/* ASICGBL受信バッファへのポインタ		*/
	
} MLDRV_CHINFO;

/* M2通信割り込み使用定義 */
#define JL076_DEF_SET	(JL076_MD1_SYNCLOE | JL076_MD1_MLTST | JL076_MD1_MLTENA | JL076_MD1_MLRENA)

/* Local variables */
MLDRV_CHINFO mlCh;


static INT32 nd_jl076_Init(ND_PARAM* nd_prm);



/*****************************************************************************
*****************************************************************************/
/*	ASIC初期化 */
/*****************************************************************************
*****************************************************************************/
INT32 NdInit( ND_PARAM *nd_prm, void *DeviceAddress )
{
	INT32	rc,i;			/* return code */
	UCHAR *pBuf = (UCHAR*)&mlCh;

	for(i = 0; i < sizeof(MLDRV_CHINFO); i++)
	{
		*(pBuf++) = 0;
	}
	
	/* Set ASIC Address  */
	mlCh.hif_reg_top = (UCHAR*)DeviceAddress;
	
	mlCh.msgInfo[0].C2Sel = 0;
	mlCh.msgInfo[0].pktSz = ((nd_prm->DataSize <= 17) ? 16: 30);
	mlCh.msgInfo[0].RegTMC.dwd = MCTRL_CM | MCTRL_SD1_END1;
	mlCh.msgInfo[1].C2Sel = 1;
	mlCh.msgInfo[1].pktSz = mlCh.msgInfo[0].pktSz;
	mlCh.msgInfo[1].RegTMC.dwd = MCTRL_CM | MCTRL_SD1_END1 | MCTRL_C1C2;

	mlCh.pRegSTS = (VUSHORT*)(mlCh.hif_reg_top + JL076_MLSTAT_ADR);
	mlCh.pRegRxD = (UCHAR*)(mlCh.hif_reg_top + JL076_RC_ADR);
	mlCh.pRegTxD = (UCHAR*)(mlCh.hif_reg_top + JL076_TC_ADR);
	mlCh.pRegSFD = (UCHAR*)(mlCh.hif_reg_top + JL076_GR_ADR);
	mlCh.msgInfo[0].pRegMCTRL = (UCHAR*)(mlCh.hif_reg_top + JL076_CTRL_C1_ADR);
	mlCh.msgInfo[0].pRegMBD = (UCHAR*)(mlCh.hif_reg_top + JL076_MB_C1_ADR);
	mlCh.msgInfo[1].pRegMCTRL = (UCHAR*)(mlCh.hif_reg_top + JL076_CTRL_C2_ADR);
	mlCh.msgInfo[1].pRegMBD = (UCHAR*)(mlCh.hif_reg_top + JL076_MB_C2_ADR);

	rc = nd_jl076_Init(nd_prm);
	
	/* ND_PARAMの値を保存する。	*/
	mlCh.nd_prm.TCycle = nd_prm->TCycle;
	mlCh.nd_prm.TSlot = nd_prm->TSlot;
	mlCh.nd_prm.NodeAddress = nd_prm->NodeAddress;
	mlCh.nd_prm.DataSize = nd_prm->DataSize;
	mlCh.nd_prm.TSpeed = nd_prm->TSpeed;
	
	return rc;
}
/****************************************************************************/
/* 概要：JL-076の初期化を行う。												*/
/****************************************************************************/
/* 引数		：prm ASIC設定パラメータ										*/
/* 戻り値	： = ND_SUCCESS 			正常終了							*/
/*   		:  = ND_INITVAL_ERR        パラメータ設定異常                   */
/****************************************************************************/
static INT32 nd_jl076_Init(ND_PARAM* nd_prm)
{
	UCHAR*	pJL;		/* JL-076 register top address. */
	USHORT	prm;		/* prm register data */
	LONG	t_cyc;

/* Set ASIC Address  */
	pJL = mlCh.hif_reg_top;
	
/* Network parameter setting */
	if(nd_prm->TSpeed == 4)	/* MECHATROLINK-1 mode */
	{
		prm = JL076_PRM_4MHZ | JL076_PRM_17BYTE;	/* prm : 4MHz, 17byte */
		nd_prm->TSlot =  500;	/* channel cycle : 125[us] */
		nd_prm->TCycle = 8000;	/* transmission cycle time : 2000[us] */
	}
	else if(nd_prm->TSpeed == 10)	/* MECHATROLINK-2 mode */
	{
		if(nd_prm->DataSize == 32)			/* 32byte mode */
		{
			prm = JL076_PRM_10MHZ | JL076_PRM_32BYTE;	/* prm : 10MHz, 32byte */
			nd_prm->TSlot = 360;		/* channel cycle : tslot=90[us] */
		}
		else if(nd_prm->DataSize == 17)	/* 17byte mode */
		{
			prm = JL076_PRM_10MHZ | JL076_PRM_17BYTE;	/* prm : 10MHz, 17byte */
			nd_prm->TSlot = 240;		/* channel cycle : tslot=60[us] */
		}
		else	/* parameter setting error */
		{
			return ND_INITVAL_ERR;
		}
	}
	else	/* parameter setting error */
	{
		return ND_INITVAL_ERR;
	}

	mlCh.errSts = 0;
	
/*----  for Axis1	------------------------------------*/
	/* Stop Communication */
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR, 0);

	/* Initilization of JL-076 */
	JL076_WR_DWREG(pJL + JL076_MLSET2_ADR, prm | nd_prm->NodeAddress);

	/* Set initial message ctrl data */
	JL076_WR_DWREG(pJL + JL076_TM_C1_ADR, (VULONG)mlCh.msgInfo[0].RegTMC.dwd);
	JL076_WR_DWREG(pJL + JL076_TM_C2_ADR, (VULONG)mlCh.msgInfo[1].RegTMC.dwd);
	MLLSI_WR_REG(mlCh.msgInfo[0].pRegMCTRL, mlCh.msgInfo[0].RegTMC.dwd);
	MLLSI_WR_REG(mlCh.msgInfo[1].pRegMCTRL, mlCh.msgInfo[1].RegTMC.dwd);

	/* Enable Communication */
	t_cyc = (nd_prm->TCycle * nd_prm->TSpeed) / 4;
	JL076_WR_DWREG(pJL + JL076_MLSY_ADR, t_cyc);
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR, 
			(JL076_DEF_SET | JL076_MD1_INTFREE | ((t_cyc>>16)&1)) );

#if 0
/*----  for Axis2	------------------------------------*/
	/* Stop Communication */
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR + 0x1000, 0);

	/* Initilization of JL-076 */
	JL076_WR_DWREG(pJL + JL076_MLSET2_ADR + 0x1000, prm | nd_prm->NodeAddress + 1);

	/* Set initial message ctrl data */
	JL076_WR_DWREG(pJL + JL076_TM_C1_ADR + 0x1000, (VULONG)mlCh.msgInfo[0].RegTMC.dwd);
	JL076_WR_DWREG(pJL + JL076_TM_C2_ADR + 0x1000, (VULONG)mlCh.msgInfo[1].RegTMC.dwd);
	MLLSI_WR_REG(mlCh.msgInfo[0].pRegMCTRL + 0x1000, mlCh.msgInfo[0].RegTMC.dwd);
	MLLSI_WR_REG(mlCh.msgInfo[1].pRegMCTRL + 0x1000, mlCh.msgInfo[1].RegTMC.dwd);

	/* Enable Communication */
	JL076_WR_DWREG(pJL + JL076_MLSY_ADR + 0x1000, t_cyc);
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR + 0x1000, 
			(JL076_DEF_SET | JL076_MD1_INTFREE | ((t_cyc>>16)&1)) );
#endif
	return ND_SUCCESS;
}


/********************************************************************************/
/*																				*/
/*	関数名 :	NdSendRSP()														*/
/*																				*/
/*	機能概要:																	*/
/*		応答データ送信処理（同期通信）											*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	引数 :																		*/
/*		INT ch					チャンネル番号									*/
/*		ULONG* lnk_rbuff		送信データの送信バッファ						*/
/*	戻り値 :																	*/
/*		= ND_SUCCESS			正常終了										*/
/*		= 上記以外				応答データ書き込み失敗							*/
/*																				*/
/********************************************************************************/
void NdSendRSP( UINT st_no, ULONG *Buffer )
{

//		Mldrv_write_lnk(mlCh.pRegTxD[st_no], (UCHAR*)Buffer, 30, 0x01)
		Mldrv_write_lnk(mlCh.pRegTxD, (UCHAR*)Buffer, 30, 0x01);
}
/****************************************************************************/
/*	ASIC設定リセット(伝送周期再設定) */
/*****************************************************************************
引数：	nd_prm	・・・通信パラメータ設定
戻り値：rc < 0	・・・エラー
		rc = 0	・・・正常
*****************************************************************************/
INT32 NdResetDevice(ND_PARAM* nd_prm)
{
	UCHAR*	pJL;	/* JL-076 register top address. */
	LONG	t_cyc;

/* Reset Mode2 register in MECHATROLINK-1 mode */
	if(nd_prm->TSpeed == 4)
	{
		return ND_SUCCESS;
	}
/* Set JL-076 register top address */
	pJL = mlCh.hif_reg_top;

/*----  for Axis1	------------------------------------*/	
	/* Reset Mode2 register(Timer Stop) */
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR, JL076_DEF_SET);
	/* Reset Transmission cycle time */
	t_cyc = (nd_prm->TCycle * nd_prm->TSpeed) / 4;
	JL076_WR_DWREG(pJL + JL076_MLSY_ADR, t_cyc);
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR, 
			(JL076_DEF_SET | JL076_MD1_INTFREE | ((t_cyc>>16)&1)) );

#if 0
/*----  for Axis2	------------------------------------*/
	/* Reset Mode2 register(Timer Stop) */
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR + 0x1000, JL076_DEF_SET);
	/* Reset Transmission cycle time */
	JL076_WR_DWREG(pJL + JL076_MLSY_ADR + 0x1000, t_cyc);
	JL076_WR_DWREG(pJL + JL076_MLSET1_ADR + 0x1000, 
			(JL076_DEF_SET | JL076_MD1_INTFREE | ((t_cyc>>16)&1)) );
#endif


	/* ND_PARAMの値を保存する。	*/
	mlCh.nd_prm.TCycle = nd_prm->TCycle;
	mlCh.nd_prm.TSlot = nd_prm->TSlot;
	mlCh.nd_prm.NodeAddress = nd_prm->NodeAddress;
	mlCh.nd_prm.DataSize = nd_prm->DataSize;
	mlCh.nd_prm.TSpeed = nd_prm->TSpeed;

	return ND_SUCCESS;
}

/*****************************************************************************
	メッセージ送受信(グローバル割り込み処理)
*****************************************************************************/
void NdExchangeMsgPkt(void)
{
	Mldrv_ExchangeMsgCtrl(&mlCh.msgInfo[1],
							mlCh.errSts & JL076_C2ERR_STS, 
							(UCHAR*)(mlCh.hif_reg_top + JL076_RM_C2_ADR),
							(UCHAR*)(mlCh.hif_reg_top + JL076_TM_C2_ADR));

	Mldrv_ExchangeMsgCtrl(&mlCh.msgInfo[0], 
							mlCh.errSts & JL076_C1ERR_STS, 
							(UCHAR*)(mlCh.hif_reg_top + JL076_RM_C1_ADR),
							(UCHAR*)(mlCh.hif_reg_top + JL076_TM_C1_ADR));
}
/*****************************************************************************
	メッセージ送受信(非同期アクセス用処理)
*****************************************************************************/
void NdExchangeMsgPktAsync(void)
{
	USHORT errSts;
	
	errSts = (USHORT)MLLSI_RD_REG(mlCh.pRegSTS);
	errSts = (errSts >> 4) & errSts;	/* 前回伝送時と直値の受信状態をAND  */

	Mldrv_ExchangeMsgCtrl(&mlCh.msgInfo[1],
							errSts & JL076_C2ERR_STS, 
							(UCHAR*)(mlCh.hif_reg_top + JL076_RM_C2_ADR),
							(UCHAR*)(mlCh.hif_reg_top + JL076_TM_C2_ADR));

	Mldrv_ExchangeMsgCtrl(&mlCh.msgInfo[0], 
							errSts & JL076_C1ERR_STS, 
							(UCHAR*)(mlCh.hif_reg_top + JL076_RM_C1_ADR),
							(UCHAR*)(mlCh.hif_reg_top + JL076_TM_C1_ADR));
}

/****************************************************************************/
/*	サイクリックデータ受信 */
/*****************************************************************************
引数：	ch		・・・受信slot(Reserve)
		pBuf	・・・受信データ格納先
戻り値：rc < 0	・・・受信エラー
		rc > 0	・・・受信完了
*****************************************************************************/
INT32 NdReceiveCMD(LONG ch, ULONG *pBuf)
{
	UCHAR dummy[32];

/* Check received status */
	if((mlCh.errSts & JL076_MYERR_STS) != 0)
	{/* Receiving OK */
		Mldrv_read_lnk(mlCh.pRegRxD, (UCHAR*)pBuf, 30);
		return ND_SUCCESS;
	}
/* Receiving error */
	Mldrv_read_lnk(mlCh.pRegRxD, dummy, 30);
	return ND_RECV_ERR;
}

/****************************************************************************/
/* 概要：グローバルデータ（伝送周期、タイムスロット）の取得を行う。			*/
/****************************************************************************/
/* 引数		：USHORT* t_cyc		伝送周期[0.25us]							*/
/*			：USHORT* t_slot	タイムスロット[0.25us]						*/
/* 戻り値	：= 0 正常終了													*/
/****************************************************************************/
INT32 NdReceiveGblInfo(USHORT* t_cyc,USHORT* t_slot)
{
	USHORT dmmy1,dmmy2;
/* 	*/
	if(mlCh.nd_prm.TSpeed == 4)
	{/* MECHATROLINK-1 mode */
		*t_cyc = mlCh.nd_prm.TCycle;
		*t_slot = mlCh.nd_prm.TSlot;
		return ND_SUCCESS;
	}
	else
	{/* MECHATROLINK-2 mode */
		/* Check received status */
		if((mlCh.errSts & JL076_GBLERR_STS) != 0)
		{/* Receiving OK */
			Mldrv_get_comtime(mlCh.pRegSFD, t_cyc, t_slot);
			return ND_SUCCESS;
		}
		/* Receiving error */
		Mldrv_get_comtime(mlCh.pRegSFD, &dmmy1, &dmmy2);
		return ND_RECV_ERR;
	}
}

/*****************************************************************************
	ASICステータスの取得(グローバル割り込みの最初にコール)
*****************************************************************************/
INT32 NdGetDevStatus(ULONG *ests)
{
/* Save Error Status */
	*ests = 0;
	mlCh.errSts = (USHORT)MLLSI_RD_REG(mlCh.pRegSTS);
	return ND_SUCCESS;
}

/*****************************************************************************
	マルチパケットメッセージ分割・組立メイン制御
*****************************************************************************/
void NdControlMsgPkt(void)
{
	Mldrv_ControlMsg(&mlCh.msgInfo[0]);
	Mldrv_ControlMsg(&mlCh.msgInfo[1]);
}

/*****************************************************************************
	メッセージ受信開始要求セット
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
		ND_RECV_BUSY	・・・受信中(要求発行失敗)
		ND_SUCCESS		・・・正常終了
*****************************************************************************/
LONG NdReqRecvMsg(UCHAR ch)
{
	MLMSG_INFO	*pInfo;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];

	if(pInfo->MRX_R != 0)	/* receiving now */
	{	return ND_RECV_BUSY;}

/* Request transmit message */
	pInfo->Error = 0;
	pInfo->MRX_R = TRUE;

	return ND_SUCCESS;
}

/*****************************************************************************
	メッセージ受信完了チェック
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
		rcvWSz	・・・受信メッセージワードサイズ
		rcvBSz	・・・受信メッセージバイトサイズ（常に0）
	戻り値：
		ND_RECV_BUSY	・・・受信中, or 受信待ち状態
		ND_SUCCESS		・・・受信完了
		それ以外		・・・受信エラー(ハンドシェイクエラー含)
*****************************************************************************/
#if (ND_MSGDRV_VER > 0x100)
LONG NdGetRecvMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz)
{
	MLMSG_INFO	*pInfo;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];
	*rcvWSz = 0;
	*rcvBSz = 0;

	if((pInfo->MRX_R != 0) || (pInfo->State != 0))	/* receiving now */
	{	return ND_RECV_BUSY;}
	if(pInfo->Error != ML_FUNC_SUCCESS)	/* receive error */
	{	return ND_RECVMSG_ERR;}

/* Set receive size */
	*rcvWSz = pInfo->RxSize >> 1;
	return ND_SUCCESS;
}
#else
LONG NdGetRecvMsgSts(UCHAR ch, USHORT *rcvSz)
{
	MLMSG_INFO	*pInfo;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];
	*rcvSz = 0;

	if((pInfo->MRX_R != 0) || (pInfo->State != 0))	/* receiving now */
	{	return ND_RECV_BUSY;}
	if(pInfo->Error != ML_FUNC_SUCCESS)	/* receive error */
	{	return ND_RECVMSG_ERR;}

/* Set receive size */
	*rcvSz = pInfo->RxSize >> 1;
	return ND_SUCCESS;
}
#endif
/*****************************************************************************
	メッセージデータリード
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
		rcvData	・・・受信データ格納先
		rdSz	・・・リードワード数
*****************************************************************************/
void NdReadMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz)
{
	MLMSG_INFO	*pInfo;
	USHORT* pBuf;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];
	if(!pInfo->MRX_R)
	{
		pBuf = (USHORT*)pInfo->pRxBuf;
		while((rdSz--) > 0)
		{
			*rcvData = *pBuf;
			rcvData++;
			pBuf++;
		}
	}
}

/*****************************************************************************
	メッセージデータライト
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
		sndData	・・・送信データ格納先
		wrSz	・・・ライトバイト数
*****************************************************************************/
void NdWriteMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz)
{
	MLMSG_INFO	*pInfo;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];
	if(!pInfo->MTX_R)
	{
		pInfo->pTxBuf = (UCHAR*)sndData;
#if (ND_MSGDRV_VER > 0x100)
		pInfo->TxSize = wrSz;
#else
		pInfo->TxSize = wrSz << 1;
#endif
	}
}

/*****************************************************************************
	メッセージ送信開始要求セット
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
		ND_SEND_BUSY	・・・送信中(要求発行失敗)
		ND_SUCCESS		・・・正常終了
*****************************************************************************/
LONG NdReqSendMsg(UCHAR ch)
{
	MLMSG_INFO	*pInfo;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];
	if(pInfo->MTX_R != 0)	/* receiving now */
	{	return ND_SEND_BUSY;}

/* Request transmit message */
	pInfo->Error = 0;
	pInfo->MTX_R = TRUE;

	return ND_SUCCESS;
}

/*****************************************************************************
	メッセージ送信完了チェック
	引数：
		ch = 1	・・・C1メッセージCH		ch != 1	・・・C2メッセージCH
	戻り値：
		ND_SEND_BUSY	・・・送信中
		ND_SUCCESS		・・・送信完了
		それ以外		・・・送信エラー(ハンドシェイクエラー含)
*****************************************************************************/
LONG NdGetSendMsgSts(UCHAR ch)
{
	MLMSG_INFO	*pInfo;

	pInfo = (ch == 1) ? &mlCh.msgInfo[0]: &mlCh.msgInfo[1];
	if((pInfo->MTX_R != 0) || (pInfo->State != 0))	/* receiving now */
	{	return ND_SEND_BUSY;}

	if(pInfo->Error != ML_FUNC_SUCCESS)	/* transmitting error */
	{	return ND_SENDMSG_ERR;}

	return ND_SUCCESS;
}


#ifdef _MSG_DEBUG
#define _MSG_DEBUG
#pragma section sidata begin
USHORT msgbuf[128];
USHORT msgSts[2];
#pragma section sidata end
VOID MsgTest(UCHAR i)
{
	USHORT sz;
	LONG rc;

	switch(msgSts[i])
	{
	case 0:
		if(NdReqRecvMsg(i) == ND_SUCCESS)
			msgSts[i] = 1;
		break;
	case 1:
		rc = NdGetRecvMsgSts(i, &sz);
		if(rc == ND_SUCCESS)
		{
			NdReadMsgData(i, msgbuf, sz);
			NdWriteMsgData(i, msgbuf, sz);
			if(NdReqSendMsg(i) != ND_SUCCESS)
			{	msgSts[i] = 0;}
			else
			{	msgSts[i] = 2;}
		}
		else if(rc != ND_RECV_BUSY)
		{
			msgSts[i] = 0;
		}
		break;
	case 2:
		if(NdGetSendMsgSts(i) != ND_SEND_BUSY)
		{
			msgSts[i] = 0;
		}
		break;

	default:break;
	}
}
#endif /* _MSG_DEBUG */

/*****************************************************************************
	通信パラメータ取得
	引数：
	戻り値：*ND_PRAM	//kira_for_MEMOBUS_Message
*****************************************************************************/
ND_PARAM* NdGetNdParamPtr( VOID )
{
	return (&mlCh.nd_prm);

}

/*****************************************************************************
	通信ASICリセット (通信停止) <S14E>
	引数：DeviceAddress 通信ASIC先頭アドレス
	戻り値：
*****************************************************************************/
void NdResetAsic( void *DeviceAddress )
{
	/* Stop Communication */
	*(ULONG*)((ULONG)DeviceAddress + JL076_MLSET1_ADR) = 0;
	return;
}
