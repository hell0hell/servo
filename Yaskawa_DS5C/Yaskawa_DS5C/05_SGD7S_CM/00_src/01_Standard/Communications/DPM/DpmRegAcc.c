/****************************************************************************/
/*																			*/
/*		DPM Non-Cyclic data Control ソースファイル							*/
/*																			*/
/****************************************************************************/
/*																			*/
/*	Target model: SigmaV Command-Option	ServoDrive							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	History:																*/
/*	2007.9.12	created														*/
/*																			*/
/*																			*/
/************** Copyright(c) Yaskawa Electric Corporation *******************/

#define	KPI_IOREG
#define	DPM_CTRL_MODULE


/* includes */
#include "KnlStd.h"
#include "Dpm.h"
#include "NetMsgDrv.h"


/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif

/* Public functions */
/* Link operation API */
//void DpmLinkRegAcc(DPMCTRLIF *pDpmCtl);
/* CS Message operation API */
//LONG DpmReqRecvCSMsg(DPMCTRLIF *pDpmCtl);
//LONG DpmGetRecvCSMsgSts(DPMCTRLIF *pDpmCtl, USHORT *rcvWSz, USHORT *rcvBSz);
//void DpmReadCSMsgData(DPMCTRLIF *pDpmCtl, USHORT *rcvData, USHORT rdSz);
//void DpmWriteCSMsgData(DPMCTRLIF *pDpmCtl, USHORT *sndData, USHORT wrSz);
//LONG DpmReqSendCSMsg(DPMCTRLIF *pDpmCtl);
/* DegOp. operation API */
//LONG DpmGetDegOpState(DPMCTRLIF *pDpmCtl);
//LONG DpmReqDegOpPrmRead(DPMCTRLIF *pDpmCtl, USHORT cmd, USHORT no);
//LONG DpmReqDegOpPrmWrite(DPMCTRLIF *pDpmCtl, USHORT no, USHORT size, LONG data);
//LONG DpmGetDegOpPrmValue(DPMCTRLIF *pDpmCtl, USHORT *no, DEGOP_ATTR *Attr, LONG *data);
/* SC Message. operation API */
//LONG DpmGetSCMsgState(DPMCTRLIF *pDpmCtl);
//LONG DpmReqSCMsgTrans(DPMCTRLIF *pDpmCtl, USHORT *pMsgBuf, LONG bSz);
//LONG DpmGetSCMsgRspData(DPMCTRLIF *pDpmCtl, USHORT *pMsgBuf, LONG *bSz);

/* Private functions */
static USHORT dpmMakeChecksum(void* pData, LONG wSz);
static NONCYC_STATE dpmSetLinkError(LONG err, DOLNK_STRUCT *Cmd, DOLNK_STRUCT *Rsp);

#ifdef __cplusplus
}
#endif


/****************************************************************************
****************************************************************************/
/* LINK Data Exchange Functions */
/****************************************************************************
****************************************************************************/

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/****************************************************************************
 チェックサム作成
****************************************************************************/
static USHORT dpmMakeChecksum(void* pData, LONG wSz)
{
	USHORT sum = 0;
	USHORT* pBuf = (USHORT*)pData;
	LONG i;
	for(i = 0; i < wSz; i++)
	{
		sum += pBuf[i];
	}
	return (~sum+1);
}

/****************************************************************************
 LINKコマンドエラー設定
****************************************************************************/
static NONCYC_STATE dpmSetLinkError(LONG err, DOLNK_STRUCT *Cmd, DOLNK_STRUCT *Rsp)
{
	NONCYC_STATE rc = Enum_ReqCompState;

	if(err == REGIF_CONTINUE)
	{/* 継続中 */
		rc = Enum_WaitCompState;
	}
	else
	{/* 処理完了 */
		if(err != REGIF_COMPLETE)
		{/* 異常終了 */
			Cmd->Cmd |= 0x80;
			Cmd->Len = 2;
			Cmd->Data = err;
		}
		*Rsp = *Cmd;
		Rsp->Chksum = dpmMakeChecksum(&Rsp->Cmd, DOLNK_MKSUM_WSZ);
	}
	return rc;
}

/****************************************************************************
 DPM LINKコマンド実行処理
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		none
****************************************************************************/
void DpmLinkRegAcc(DPMCTRLIF *pDpmCtl)
{
	LONG			length;
	LONG			rc;
	NONCYC_STATE	state = pDpmCtl->hndshk.LnkCtrl;

	switch (state)
	{
	case Enum_RecvReqState:		/* 処理要求あり */
	/* Command 取得 & サムチェック */
		pDpmCtl->lnkCmd = pDpmCtl->pLink->Cmd;
		if(dpmMakeChecksum(&pDpmCtl->lnkCmd, DOLNK_CHKSUM_WSZ) != 0)
		{/* Checksum error */
			pDpmCtl->hndshk.LnkCtrl = dpmSetLinkError(0xF,
										 &pDpmCtl->lnkCmd, &pDpmCtl->pLink->Rsp);
		}
		else
		{/* Checksum OK */
		/* レジスタ処理 */
			length = (LONG)pDpmCtl->lnkCmd.Len;
			if(pDpmCtl->lnkCmd.Cmd == 0x04)
			{/* レジスタライト */
				if((((ULONG)length&0x1) != 0) || (length == 0))/* size = 奇数 or size = 0 */
				{	rc = REGIF_DANUMERR;}
				else
				{
					rc = OpiWriteRegister(pDpmCtl->pSvOptIf->Regif,(LONG)pDpmCtl->lnkCmd.Address,
											 (length/2), (USHORT*)&pDpmCtl->lnkCmd.Data);
				}
				pDpmCtl->hndshk.LnkCtrl = dpmSetLinkError(rc,
										&pDpmCtl->lnkCmd, &pDpmCtl->pLink->Rsp);
			}
			else if(pDpmCtl->lnkCmd.Cmd == 0x03)
			{/* レジスタリード */
				rc = OpiReadRegister(pDpmCtl->pSvOptIf->Regif,
						(LONG)pDpmCtl->lnkCmd.Address, &length, (USHORT*)&pDpmCtl->lnkCmd.Data);

				pDpmCtl->lnkCmd.Len = (USHORT)(length * 2);
				pDpmCtl->hndshk.LnkCtrl = dpmSetLinkError(rc, 
											&pDpmCtl->lnkCmd, &pDpmCtl->pLink->Rsp);
			}
			else
			{/* コマンド異常 */
				pDpmCtl->hndshk.LnkCtrl = dpmSetLinkError(REGIF_CMDERR,
										 &pDpmCtl->lnkCmd, &pDpmCtl->pLink->Rsp);
			}
		}
		break;

	case Enum_WaitCompState:	/* 継続中の場合 */
		rc = OpiGetRegWrBusySts(pDpmCtl->pSvOptIf->Regif);
		pDpmCtl->hndshk.LnkCtrl = dpmSetLinkError(rc, &pDpmCtl->lnkCmd, &pDpmCtl->pLink->Rsp);
		break;

	default:
		break;
	}

}

#ifdef	__ghs__
#pragma ghs section text=default
#endif



/****************************************************************************
****************************************************************************/
/* CS Message Data Exchange Functions(M2ifMbus.c用ドライバ) */
/****************************************************************************
****************************************************************************/

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	CSメッセージ受信開始要求セット	<DPM_V103>
-----------------------------------------------------------------------------
paramter:	pDpmCtl	- pointer to DPM resouce data
return;		ND_SUCCESS - normal end
*****************************************************************************/
LONG DpmReqRecvCSMsg(DPMCTRLIF *pDpmCtl)
{
	return ND_SUCCESS;
}

/*****************************************************************************
	CSメッセージ受信完了チェック
-----------------------------------------------------------------------------
paramter:	pDpmCtl	- pointer to DPM resouce data
			rcvSz	- pointer of received message size data
return;		ND_RECV_BUSY - no message received
			ND_SUCCESS	 - message received
*****************************************************************************/
LONG DpmGetRecvCSMsgSts(DPMCTRLIF *pDpmCtl, USHORT *rcvWSz, USHORT *rcvBSz)
{
	USHORT bSz;
	LONG rc = ND_SUCCESS;

	*rcvWSz = 0;
	*rcvBSz = 0;
	if(pDpmCtl->hndshk.CSCtrl != Enum_RecvReqState)
	{/* 要求なし */
		rc = ND_RECV_BUSY;
	}
	else
	{/* 要求あり */
		pDpmCtl->pCSMsg->Rsp[0] = pDpmCtl->pCSMsg->Cmd[0];
		if((pDpmCtl->pCSMsg->Cmd[0]&0xF) != MSG_FMTID_MBUS)
		{/* プロトコルIDエラー */
			pDpmCtl->pCSMsg->Rsp[0] = MSG_FMTID_ERR;
			pDpmCtl->pCSMsg->Rsp[1] = 0;
			pDpmCtl->hndshk.CSCtrl = Enum_ReqCompState;
			rc = ND_RECVMSG_ERR;
		}
		else
		{/* プロトコルIDOK */
			bSz = pDpmCtl->pCSMsg->Cmd[1];
			*rcvWSz = (bSz >> 1) + (bSz & 0x1);	/* Set receive size */
			*rcvBSz = bSz;
			pDpmCtl->hndshk.CSCtrl = Enum_WaitCompState;/* Set Busy Status */
		}
	}
	return rc;
}


/*****************************************************************************
	CSメッセージデータリード
-----------------------------------------------------------------------------
paramter:	pDpmCtl	- pointer to DPM resouce data
			rcvData	- pointer to message data
			rdSz	- size of message data
return;		none
*****************************************************************************/
void DpmReadCSMsgData(DPMCTRLIF *pDpmCtl, USHORT *rcvData, USHORT rdSz)
{
	LONG	i, maxSz;

	rcvData[0] = MSG_FMTID_MBUS;

	maxSz = (DPM_MSGDATA_BSZ-4)/sizeof(USHORT);
	if(rdSz > maxSz )
	{	rdSz = (USHORT)maxSz;}	/* 最大サイズでクランプ */

	for(i = 1; i <= rdSz; i++)
	{
		rcvData[i] = pDpmCtl->pCSMsg->Cmd[i+1];
	}
}

/*****************************************************************************
	CSメッセージデータライト
-----------------------------------------------------------------------------
paramter:	pDpmCtl	- pointer to DPM resouce data
			sndData	- pointer to message data
			wrSz	- size of message data
return;		none
*****************************************************************************/
void DpmWriteCSMsgData(DPMCTRLIF *pDpmCtl, USHORT *sndData, USHORT wrSz)
{
	LONG i;

	pDpmCtl->pCSMsg->Rsp[1] = (USHORT)(wrSz - 2);
	wrSz = (wrSz >> 1) + (wrSz & 0x1);

	for(i = 1; i <= (LONG)((DPM_MSGDATA_BSZ-4) / sizeof(USHORT)); i++)
	{
		if(i < wrSz)
		{	pDpmCtl->pCSMsg->Rsp[i+1] = sndData[i];} /* wrSz分DPMへデータコピー */
		else
		{	pDpmCtl->pCSMsg->Rsp[i+1] = 0;}			/* 残りは0 */
	}
}

/*****************************************************************************
	CSメッセージ送信開始要求セット
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		ND_SEND_BUSY	- sequece error
			ND_SUCCESS		- normal end
*****************************************************************************/
LONG DpmReqSendCSMsg(DPMCTRLIF *pDpmCtl)
{
	LONG rc = ND_SUCCESS;
/* Check CS Message Request */
	if(pDpmCtl->hndshk.CSCtrl != Enum_WaitCompState)
	{	rc = ND_SEND_BUSY;	/* 処理中でない場合はエラー */}
	else
	{	pDpmCtl->hndshk.CSCtrl = Enum_ReqCompState;}

	return rc;
}

/*****************************************************************************
	CSメッセージ送信キャンセル	<DPM_V103>
-----------------------------------------------------------------------------
paramter:	pDpmCtl	- pointer to DPM resouce data
return;		ND_SUCCESS - normal end
*****************************************************************************/
LONG DpmSendMsgAbort(DPMCTRLIF *pDpmCtl)
{
	LONG i;

	if((pDpmCtl->hndshk.CSCtrl == Enum_RecvReqState)
		|| (pDpmCtl->hndshk.CSCtrl == Enum_WaitCompState))
	{/* 未応答状態 */
		for(i = 2; i < (LONG)(DPM_MSGDATA_BSZ / sizeof(USHORT)); i++)
		{	pDpmCtl->pCSMsg->Rsp[i] = 0;}			/* Set all data 0 */

		pDpmCtl->pCSMsg->Rsp[1] = 0;
		pDpmCtl->hndshk.CSCtrl = Enum_ReqCompState;
	}

	return ND_SUCCESS;
}


#ifdef	__ghs__
#pragma ghs section text=default
#endif


/****************************************************************************
****************************************************************************/
/* DegiOp. Data Exchange Functions */
/****************************************************************************
****************************************************************************/

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	デジオペ状態取得
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		DEGOP_ENABLE	- ready state
			DEGOP_BUSY		- not ready state
*****************************************************************************/
LONG DpmGetDegOpState(DPMCTRLIF *pDpmCtl, DEGOP_PRMRNG *pPrmRange)
{
	ULONG	Ulim, Llim;
	LONG rc = DEGOP_BUSY;

	if( (pDpmCtl->hndshk.DOCtrl == Enum_ReqCompState)
		|| (pDpmCtl->hndshk.DOCtrl == Enum_IdolState) )
	{
		Ulim = (pDpmCtl->pSvOptIf->OptSetting.MaxOpPnNo) & (0xFFF);
		Llim = (pDpmCtl->pSvOptIf->OptSetting.MinOpPnNo) & (0xFFF);

		if( (Ulim < Llim)
			|| (Ulim > OPT_PNNO_UPRLMT)
			|| (Llim < OPT_PNNO_LWRLMT) )
		{
			rc = DEGOP_REQ_ERR;
		}
		else
		{
			pPrmRange->UprLmt = Ulim;
			pPrmRange->LwrLmt = Llim;
			rc = DEGOP_ENABLE;
		}
	}
	else 
	{
		if( pDpmCtl->hndshk.DOCtrl == Enum_DisableState )
		{
			rc = DEGOP_REQ_ERR;
		}
	}

	return rc;
}

/*****************************************************************************
	デジオペパラメータリード要求発行
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			cmd				- Key Event(cmd=2:up-key, else:down-key)
			no				- Parameter No.
return;		DEGOP_REQ_ERR	- request error
			DEGOP_SUCCESS	- normal end
*****************************************************************************/
LONG DpmReqDegOpPrmRead(DPMCTRLIF *pDpmCtl, USHORT cmd, USHORT no)
{
	LONG rc;
	DOLNK_STRUCT degCmd;

	if( (pDpmCtl->hndshk.DOCtrl != Enum_ReqCompState)
		&& (pDpmCtl->hndshk.DOCtrl != Enum_IdolState) )
	{/* 応答完了 or IDOL状態以外はエラー */
		rc = DEGOP_REQ_ERR;
	}
	else
	{/* 応答完了 or IDOL状態 */
	/* コマンドライト */
		cmd = (cmd == DEGOP_READ_UP)? DEGOP_READ_UP: DEGOP_READ_DOWN;
		degCmd.Chksum = ~(cmd+no)+1;
		degCmd.Cmd = cmd;
		degCmd.Address = no;
		degCmd.Len = 0;
		degCmd.Data = 0;
		degCmd.dummy_c = 0;
		degCmd.dummy_e = 0;
		pDpmCtl->pDeg->Cmd = degCmd;
	/* Handshake更新要求発行 */
		pDpmCtl->hndshk.DOCtrl = Enum_RecvReqState;
		rc = DEGOP_SUCCESS;
	}
	return rc;
}

/*****************************************************************************
	デジオペパラメータライト要求発行
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			no				- Parameter No.
			size			- Parameter size.
			data			- Parameter data.
return;		DEGOP_REQ_ERR	- request error
			DEGOP_SUCCESS	- normal end
*****************************************************************************/
LONG DpmReqDegOpPrmWrite(DPMCTRLIF *pDpmCtl, USHORT no, USHORT size, LONG data)
{
	LONG rc;
	DOLNK_STRUCT degCmd;

	if( (pDpmCtl->hndshk.DOCtrl != Enum_ReqCompState)
		&& (pDpmCtl->hndshk.DOCtrl != Enum_IdolState) )
	{/* 応答完了 or IDOL状態以外はエラー */
		rc = DEGOP_REQ_ERR;
	}
	else
	{/* 応答完了 or IDOL状態 */
	/* コマンドライト */
		degCmd.Cmd = DEGOP_WRITE;
		degCmd.Address = no;
		degCmd.Len = size;
		degCmd.Data = data;
		degCmd.dummy_c = 0;
		degCmd.dummy_e = 0;
		degCmd.Chksum = dpmMakeChecksum(&degCmd.Cmd, DOLNK_MKSUM_WSZ);
		pDpmCtl->pDeg->Cmd = degCmd;
	/* Handshake更新要求発行 */
		pDpmCtl->hndshk.DOCtrl = Enum_RecvReqState;
		rc = DEGOP_SUCCESS;
	}

	return rc;
}

/*****************************************************************************
	デジオペパラメータデータ取得
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			no				- pointer to Parameter No.
			Attr			- pointer to Parameter attritbute.
			data			- pointer to Parameter data.
return;		DEGOP_REQ_ERR	- request error
			DEGOP_BUSY		- busy state
			DEGOP_SUCCESS	- normal end
*****************************************************************************/
LONG DpmGetDegOpPrmValue(DPMCTRLIF *pDpmCtl, USHORT *no, DEGOP_ATTR *Attr, LONG *data)
{
	DOLNK_STRUCT degRsp;
	LONG rc = DEGOP_SUCCESS;
/* 処理中チェック */
	if( (pDpmCtl->hndshk.DOCtrl == Enum_WaitCompState)
		|| (pDpmCtl->hndshk.DOCtrl == Enum_RecvReqState) )
	{/* 応答待ち状態 */
		rc = DEGOP_BUSY;
	}
	else
	{/* 応答完了状態 */
		degRsp = pDpmCtl->pDeg->Rsp;
		if(dpmMakeChecksum(&degRsp, DOLNK_CHKSUM_WSZ) != 0)
		{	/* チェックサムエラー */
			rc = DEGOP_RCV_ERR;
		}
		else
		{/* チェックサムOK */
			if((degRsp.Cmd & 0x80) != 0)
			{/* 実行エラー */
				rc = DEGOP_RCV_ERR;
			}
			else
			{/* 正常応答 */
				*no = degRsp.Address;
				*((USHORT*)Attr) = degRsp.Len;
				*data = degRsp.Data;
			}
		}
	}
	return rc;
}
/*------------- <V685> Add Begin -------------------------------------------*/
/*****************************************************************************
	デジオペ補助機能用コマンド発行
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			cmd				- command
			idx				- function list top index
			data			- command data (list num, key, 0)
return;		DEGOP_REQ_ERR	- request error
			DEGOP_SUCCESS	- normal end
*****************************************************************************/
LONG DpmReqDegOpFn(DPMCTRLIF *pDpmCtl, USHORT cmd, USHORT idx, USHORT data)
{
	LONG rc;
	DOLNK_STRUCT degCmd;

	if( (pDpmCtl->hndshk.DOCtrl != Enum_ReqCompState)
		&& (pDpmCtl->hndshk.DOCtrl != Enum_IdolState) )
	{/* 応答完了 or IDOL状態以外はエラー */
		rc = DEGOP_REQ_ERR;
	}
	else
	{/* 応答完了 or IDOL状態 */
	/* コマンドライト */
		degCmd.Cmd = cmd;
		degCmd.Address = idx;
//<V689>		degCmd.Len = 2;
		degCmd.Len = (cmd == DEGOP_FN_END)? 0: 2;	/*<V689>*/
		degCmd.Data = data;
		degCmd.dummy_c = 0;
		degCmd.dummy_e = 0;
		degCmd.Chksum = dpmMakeChecksum(&degCmd.Cmd, DOLNK_MKSUM_WSZ);
		pDpmCtl->pDeg->Cmd = degCmd;
	/* Handshake更新要求発行 */
		pDpmCtl->hndshk.DOCtrl = Enum_RecvReqState;
		rc = DEGOP_SUCCESS;
	}

	return rc;
}

/*****************************************************************************
	デジオペ補助機能用レスポンス取得
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			pDgopRsp		- pointer to response data
			pDgopExRsp		- pointer to extension response data
return;		DEGOP_RCV_ERR	- receive error
			DEGOP_BUSY		- busy state
			DEGOP_SUCCESS	- normal end
*****************************************************************************/
LONG DpmGetDegOpFn(DPMCTRLIF *pDpmCtl, DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp)
{
	DOLNK_STRUCT degRsp;
	LONG rc = DEGOP_SUCCESS;
/* 処理中チェック */
	if( (pDpmCtl->hndshk.DOCtrl == Enum_WaitCompState)
		|| (pDpmCtl->hndshk.DOCtrl == Enum_RecvReqState) )
	{/* 応答待ち状態 */
		rc = DEGOP_BUSY;
	}
	else
	{/* 応答完了状態 */
		degRsp = pDpmCtl->pDeg->Rsp;
		pDgopRsp->Cmd = (degRsp.Cmd & ~(0x80));
		if(dpmMakeChecksum(&degRsp, DOLNK_CHKSUM_WSZ) != 0)
		{	/* チェックサムエラー */
			rc = DEGOP_RCV_ERR;
		}
		else
		{/* チェックサムOK */
			if((degRsp.Cmd & 0x80) != 0)
			{/* 実行エラー */
				rc = DEGOP_RCV_ERR;
			}
			else
			{/* 正常応答 */
				pDgopRsp->Data.us = (USHORT)degRsp.Data;
				pDgopRsp->Len = (USHORT)(degRsp.Data>>16);
				*pDgopExRsp = *pDpmCtl->pDegEx;
			}
		}
	}
	return rc;
}
/*------------- <V685> Add End ---------------------------------------------*/
#ifdef	__ghs__
#pragma ghs section text=default
#endif

/****************************************************************************
****************************************************************************/
/* SC Message Data Exchange Functions */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	SC Message状態取得
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		SCMSG_ENABLE	- ready state
			SCMSG_BUSY		- not ready state
*****************************************************************************/
LONG DpmGetSCMsgState(DPMCTRLIF *pDpmCtl)
{
	LONG rc = SCMSG_BUSY;
	if( (pDpmCtl->hndshk.SCCtrl == Enum_ReqCompState)
		|| (pDpmCtl->hndshk.SCCtrl == Enum_IdolState) )
	{
		rc = SCMSG_ENABLE;
	}
	return rc;
}

/*****************************************************************************
	SC Messageデータ転送要求発行
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			pMsgBuf			- pointer to message data
			bSz				- size of message data
return;		SCMSG_REQ_ERR	- request error
			SCMSG_SUCCESS	- normal end
*****************************************************************************/
LONG DpmReqSCMsgTrans(DPMCTRLIF *pDpmCtl, USHORT *pMsgBuf, LONG bSz)
{
	LONG i, wSz, rc;

	if( (pDpmCtl->hndshk.SCCtrl != Enum_ReqCompState)
		&& (pDpmCtl->hndshk.SCCtrl != Enum_IdolState) )
	{/* 応答完了 or IDOL状態以外はエラー */
		rc = SCMSG_REQ_ERR;
	}
	else
	{/* 応答完了 or IDOL状態 */
		if(bSz > (DPM_MSGDATA_BSZ-4))
		{	bSz = (DPM_MSGDATA_BSZ-4);}
		/* コマンドライト */
		pDpmCtl->pSCMsg->Cmd[0] = MSG_FMTID_MBUS;
		pDpmCtl->pSCMsg->Cmd[1] = (USHORT)bSz;
		wSz = (LONG)((ULONG)(bSz + 1) / sizeof(USHORT));
		for(i = 0; i < wSz; i++)
		{
			pDpmCtl->pSCMsg->Cmd[i+2] = pMsgBuf[i];
		}
		/* Handshake更新要求発行 */
		pDpmCtl->hndshk.SCCtrl = Enum_RecvReqState;
		rc = SCMSG_SUCCESS;
	}
	return rc;
}

/*****************************************************************************
	SC Message応答データ取得
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
			pMsgBuf			- pointer to message data
			bSz				- size of message data
return;		SCMSG_REQ_ERR	- request error
			SCMSG_BUSY		- busy state
			SCMSG_SUCCESS	- normal end
*****************************************************************************/
LONG DpmGetSCMsgRspData(DPMCTRLIF *pDpmCtl, USHORT *pMsgBuf, LONG *bSz)
{
	LONG i, wSz, rc;

	*bSz = 0;
	if( (pDpmCtl->hndshk.SCCtrl == Enum_WaitCompState)
		|| (pDpmCtl->hndshk.SCCtrl == Enum_RecvReqState) )
	{/* 応答待ち状態 */
		rc = SCMSG_BUSY;
	}
	else
	{/* 応答完了状態 */
		*bSz = (LONG)pDpmCtl->pSCMsg->Rsp[1];
//		if( (*bSz == 0) || (*bSz > (DPM_MSGDATA_BSZ-4)) )
		if( (*bSz < MSG_MINDATA_BSZ) || (*bSz > (DPM_MSGDATA_BSZ-4)) )
		{/* 無応答エラー or データサイズエラー*/
			rc = SCMSG_RCV_ERR;
		}
		else
		{/* 応答データあり */
			wSz = (LONG)((ULONG)(*bSz + 1) / sizeof(USHORT));
			for(i = 0; i < wSz; i++)
			{
				pMsgBuf[i] = pDpmCtl->pSCMsg->Rsp[i+2];
			}
			rc = SCMSG_SUCCESS;
		}
	}
	return rc;
}



#ifdef	__ghs__
#pragma ghs section text=default
#endif
