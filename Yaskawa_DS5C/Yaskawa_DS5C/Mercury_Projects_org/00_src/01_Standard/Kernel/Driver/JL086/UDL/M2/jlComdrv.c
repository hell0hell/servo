/****************************************************************************/
/*                                                                          */
/* Module : JL-076/052 access interface                                     */
/*                                                                          */
/****************************************************************************/
/* Functions :                                                              */
/*                                                                          */
/* Mldrv_ramcheck()       : Read/Write Check on the Internal RAM            */
/* Mldrv_read_lnk()       : Read a packet of cyclic data                    */
/* Mldrv_write_lnk()      : Write a packet of cyclic data                   */
/* Mldrv_get_comtime()    : Read the transmission cycle from global frame   */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* History:                                                                 */
/*                                                                          */
/*                                                                          */
/*******************  2006 Yaskawa Electric Corporation *********************/

/* includes */
#include "Basedef.h"
#include "jlComdrv.h"

/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif
int Mldrv_ramcheck( UCHAR *pJL, USHORT chk_data, USHORT size);
void Mldrv_read_lnk(UCHAR *pJL, UCHAR *pBuf, UINT size);
void Mldrv_write_lnk(UCHAR *pJL, UCHAR *pBuf, UINT size, UCHAR ctrl);
void Mldrv_get_comtime(UCHAR *pJL, USHORT *t_cyc, USHORT *t_slt);
void Mldrv_write_lnk052(UCHAR *pJL, UCHAR *pBuf, UINT size, UCHAR ctrl);	/* <V581> */

void Mldrv_ExchangeMsgCtrl(MLMSG_INFO* pInfo, USHORT rcvSts, UCHAR* pRegRxD, UCHAR* pRegTxD);
void Mldrv_ControlMsg(MLMSG_INFO* pInfo);

UCHAR mldrv_IDLE(MLMSG_INFO* pInfo);
UCHAR mldrv_WSYNCT(MLMSG_INFO* pInfo);
UCHAR mldrv_WDATA(MLMSG_INFO* pInfo);
UCHAR mldrv_WSYNCR(MLMSG_INFO* pInfo);
UCHAR mldrv_WPOLL(MLMSG_INFO* pInfo);
void mldrv_SetError(MLMSG_INFO* pInfo, SHORT code, ULONG mctrl);

#ifdef __cplusplus
}
#endif


/*****************************************************************************
*****************************************************************************/
/*	Link Functions */
/*****************************************************************************
*****************************************************************************/

/****************************************************************************/
/* Function:                                                                */
/* Mldrv_ramcheck()      : Read/Write Check on the Internal RAM             */
/*                                                                          */
/* Return code :                                                            */
/*   ML_FUNC_SUCCESS    : Normal End                                        */
/*   ML_RAMCHECK_ERR    : Internal RAM check error                          */
/*                                                                          */
/* Parameter :                                                              */
/*   pJL : ASIC RAM address   					                            */
/*   chk_data : check data to read and write                                */
/*                                                                          */
/* Note :                                                                   */
/****************************************************************************/
int Mldrv_ramcheck(UCHAR *pJL, USHORT chk_data, USHORT size)
{
	VUSHORT	*pVal;		/* Host I/F Top address */
	USHORT	rd_data;	/* Read data */
	int		i;			/* loop counter */

/* Write data */
	pVal = (VUSHORT*)pJL;
	for(i = 0; i < size; i++, pVal++)
	{
		if((i & 0x1) && (chk_data != 0))	/* odd address */
			MLLSI_WR_REG(pVal, ~chk_data);
		else	/* even address */
			MLLSI_WR_REG(pVal, chk_data);
	}

/* Read and verify writed data */
	pVal = (VUSHORT*)pJL;
	for(i = 0; i < size; i++, pVal++)
	{
		rd_data = MLLSI_RD_REG(pVal);
		if((i & 0x1) && (chk_data != 0))	/* odd address */
		{
			rd_data = ~rd_data;
			if(rd_data != chk_data)
				return ML_RAMCHECK_ERR;
		}
		else	/* even address */
		{
			if(rd_data != chk_data)
				return ML_RAMCHECK_ERR;
		}
	}

	return ML_FUNC_SUCCESS;
}

/****************************************************************************/
/* Function:                                                                */
/* Mldrv_read_lnk()      : Read a packet of cyclic data                     */
/*                                                                          */
/* Parameter :                                                              */
/*   pJL : ASIC Rx register top address                                     */
/*   pBuf: Receiving buffer                                                 */
/*   size: Receiving buffer size in word                                    */
/*                                                                          */
/* Note :                                                                   */
/****************************************************************************/
void Mldrv_read_lnk(UCHAR *pJL, UCHAR *pBuf, UINT size)
{
	VULONG		*pVal;	/* Host I/F address */
	MLDWD_DATA	data;

	pVal = (VULONG*)pJL;
/* Read a received packet */
	data.dwd = MLLSI_RD_REG(pVal);
	*pBuf = data.byte[1];
	*(pBuf+1) = data.byte[2]; *(pBuf+2) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+1);
	*(pBuf+3) = data.byte[0]; *(pBuf+4) = data.byte[1];
	*(pBuf+5) = data.byte[2]; *(pBuf+6) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+2);
	*(pBuf+7) = data.byte[0]; *(pBuf+8) = data.byte[1];
	*(pBuf+9) = data.byte[2]; *(pBuf+10) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+3);
	*(pBuf+11) = data.byte[0]; *(pBuf+12) = data.byte[1];
	*(pBuf+13) = data.byte[2]; *(pBuf+14) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+4);
	*(pBuf+15) = data.byte[0]; 
	
	if(size <= 17) return;
	*(pBuf+16) = data.byte[1];
	*(pBuf+17) = data.byte[2]; *(pBuf+18) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+5);
	*(pBuf+19) = data.byte[0]; *(pBuf+20) = data.byte[1];
	*(pBuf+21) = data.byte[2]; *(pBuf+22) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+6);
	*(pBuf+23) = data.byte[0]; *(pBuf+24) = data.byte[1];
	*(pBuf+25) = data.byte[2]; *(pBuf+26) = data.byte[3];
	data.dwd = MLLSI_RD_REG(pVal+7);
	*(pBuf+27) = data.byte[0]; *(pBuf+28) = data.byte[1];
	*(pBuf+29) = data.byte[2];

}

/****************************************************************************/
/* Function:                                                                */
/* Mldrv_write_lnk()      : Write a packet of cyclic data                   */
/*                                                                          */
/* Parameter :                                                              */
/*   pJL : ASIC Tx register top address                                     */
/*   pBuf: transmitting buffer                                              */
/*   size: transmitting buffer size in word                                 */
/*                                                                          */
/* Note :                                                                   */
/****************************************************************************/
void Mldrv_write_lnk(UCHAR *pJL, UCHAR *pBuf, UINT size, UCHAR ctrl)
{
	VULONG			*pVal;	/* Host I/F Top address */
	MLDWD_DATA		data;

	pVal = (VULONG*)pJL;
/* Write a received packet */
	data.byte[0] = ctrl;		/* CTRLコードセット */
	data.byte[1] = *pBuf;
	data.byte[2] = *(pBuf+1);	data.byte[3] = *(pBuf+2);
	MLLSI_WR_REG(pVal, data.dwd);
	data.byte[0] = *(pBuf+3);	data.byte[1] = *(pBuf+4);
	data.byte[2] = *(pBuf+5);	data.byte[3] = *(pBuf+6);
	MLLSI_WR_REG(pVal+1, data.dwd);
	data.byte[0] = *(pBuf+7);	data.byte[1] = *(pBuf+8);
	data.byte[2] = *(pBuf+9);	data.byte[3] = *(pBuf+10);
	MLLSI_WR_REG(pVal+2, data.dwd);
	data.byte[0] = *(pBuf+11);	data.byte[1] = *(pBuf+12);
	data.byte[2] = *(pBuf+13);	data.byte[3] = *(pBuf+14);
	MLLSI_WR_REG(pVal+3, data.dwd);
	data.byte[0] = *(pBuf+15);	data.byte[1] = *(pBuf+16);
	data.byte[2] = *(pBuf+17);	data.byte[3] = *(pBuf+18);
	MLLSI_WR_REG(pVal+4, data.dwd);
	
	if(size <= 17) return;
	data.byte[0] = *(pBuf+19);	data.byte[1] = *(pBuf+20);
	data.byte[2] = *(pBuf+21);	data.byte[3] = *(pBuf+22);
	MLLSI_WR_REG(pVal+5, data.dwd);
	data.byte[0] = *(pBuf+23);	data.byte[1] = *(pBuf+24);
	data.byte[2] = *(pBuf+25);	data.byte[3] = *(pBuf+26);
	MLLSI_WR_REG(pVal+6, data.dwd);
	data.byte[0] = *(pBuf+27);	data.byte[1] = *(pBuf+28);
	data.byte[2] = *(pBuf+29);	data.byte[3] = 0;
	MLLSI_WR_REG(pVal+7, data.dwd);;	
}

/****************************************************************************/
/* Function:                                                                */
/* Mldrv_get_comtime()   :  Read the transmission cycle from global frame   */
/*                                                                          */
/* Parameter :                                                              */
/*   pJL   : Global Buffer register top address                             */
/*   t_cyc : transmission cycle[0.25us]                                     */
/*   t_slt : time slot[0.25us]                                              */
/*                                                                          */
/* Note :                                                                   */
/****************************************************************************/
void Mldrv_get_comtime(UCHAR *pJL, USHORT *t_cyc, USHORT *t_slt)
{
	MLDWD_DATA	reg1, reg2;

/* Read the transmission cycle */
	reg1.dwd = MLLSI_RD_REG(pJL);
	reg2.dwd = MLLSI_RD_REG(pJL + 4);

	*t_cyc = ((USHORT)reg1.byte[1] | (USHORT)(reg1.byte[2] << 8));
	*t_slt = ((USHORT)reg1.byte[3] | (USHORT)(reg2.byte[0] << 8));
}



/*****************************************************************************
*****************************************************************************/
/*	Message Functions */
/*****************************************************************************
*****************************************************************************/

/* Message State machine */
enum MSG_STATE {
	CH_IDLE_S=0,	/* IDLE状態 */
	CH_W_SYNCT,		/* 受信転送方向待ち状態 */
	CH_W_DATA,		/* データ受信待ち状態 */
	CH_W_SYNCR,		/* 送信転送方向待ち状態 */
	CH_W_POLL,		/* データ転送中状態 */
};
/* motion state machine function table */
typedef UCHAR (*MSGFUNC)(MLMSG_INFO* pInfo);
const MSGFUNC mldrv_State[] = {
	mldrv_IDLE,
	mldrv_WSYNCT,
	mldrv_WDATA,
	mldrv_WSYNCR,
	mldrv_WPOLL,
};



/*****************************************************************************
 メッセージデータ交換処理(グローバル割り込み処理:JL076用)
******************************************************************************
 引数:	rcvSts	- Message受信状態(0:異常, 0以外:正常)
		pRegRxD	- ASICメッセージ受信バッファアドレス
		pRegTxD	- ASICメッセージ送信バッファアドレス
*****************************************************************************/
void Mldrv_ExchangeMsgCtrl(MLMSG_INFO* pInfo, USHORT rcvSts, UCHAR* pRegRxD, UCHAR* pRegTxD)
{
	ULONG reg;

	pInfo->Rx_CTRLd = TRUE;
	if(pInfo->ABT_R != 0)
	{/* ABT要求あり */
		MLLSI_WR_REG(pRegTxD, pInfo->ABT_R);
		pInfo->Rx_CTRL = 0;
		pInfo->ABT_R = 0;
	}
	else if(rcvSts != 0)
	{/* 受信正常なら,内部CTRLコード更新 */
		reg = MLLSI_RD_REG(pRegRxD);
		pInfo->Rx_CTRL = reg & MCTRL_C1C2_CM_MSK;
	}
}

/*****************************************************************************
 マルチパケットメッセージ状態遷移処理メイン
*****************************************************************************/
void Mldrv_ControlMsg(MLMSG_INFO* pInfo)
{
	if(pInfo->Rx_CTRLd != 0)
	{
		pInfo->State = mldrv_State[pInfo->State](pInfo);
		pInfo->Rx_CTRLd = 0;
	}
}



/*****************************************************************************
 マルチパケットメッセージ状態遷移処理(IDLE状態)
*****************************************************************************/
UCHAR mldrv_IDLE(MLMSG_INFO* pInfo)
{
	ULONG req, val;

	if(pInfo->ABT_R) return CH_IDLE_S;

	if(pInfo->MRX_R)
	{/* 受信要求あり */
		val = (USHORT)MLLSI_RD_REG(pInfo->pRegMCTRL);
		if((val & MCTRL_C1C2_CM_MSK) != MCTRL_SD0_END0)
		{
			MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
			req = MCTRL_SD0_END0 | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
			MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		}
		return CH_W_SYNCT;
	}
	if(pInfo->MTX_R)
	{/* 送信要求あり */
		val = (USHORT)MLLSI_RD_REG(pInfo->pRegMCTRL);
		if((val & MCTRL_C1C2_CM_MSK) != MCTRL_SD0_END1)
		{
			MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
			req = MCTRL_SD0_END1 | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
			MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		}
		return CH_W_SYNCR;
	}
	return CH_IDLE_S;
}

/*****************************************************************************
 マルチパケットメッセージ状態遷移処理(受信要求待ち状態)
*****************************************************************************/
UCHAR mldrv_WSYNCT(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req;
	UCHAR rc;
	
	/* ローカル変数初期化	*/
	rc = CH_W_SYNCT;
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	ctrl = pInfo->Rx_CTRL & MCTRL_SDEND_MSK;
	
	if(req & MCTRL_RE)
	{/* 転送方向指定未受信 */
		if(pInfo->Rx_CTRL == MCTRL_ABORT)
		{/* Abort受信 */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		if(ctrl == MCTRL_SD0_END0)
		{/* データがきてたら再セット */
			MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
			req = pInfo->Rx_CTRL | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
			MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		}
		else if(ctrl == MCTRL_SD0_END1)
		{/* Handshake Error */
			mldrv_SetError(pInfo, ML_HNDSHK_ERR, MCTRL_SD0_END0 | MCTRL_RE);
			rc = CH_IDLE_S;
		}
	}
	else
	{/* 転送方向指定受信 */
		pInfo->SeqNo = 0;
		pInfo->RxSize = 0;
		req = MCTRL_SD1_END0 | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
		MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		rc = CH_W_DATA;
	}
	return rc;
}

/*****************************************************************************
 マルチパケットメッセージ状態遷移処理(データ受信状態)
*****************************************************************************/
UCHAR mldrv_WDATA(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req;

	/* ローカル変数初期化	*/
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	
	if(req & MCTRL_RE)
	{/* 未受信状態 */
		if(pInfo->Rx_CTRL == MCTRL_ABORT)
		{/* Abort受信 */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		ctrl = pInfo->Rx_CTRL & MCTRL_SDEND_MSK;
		if(ctrl == MCTRL_SD1_END1)
		{/* 受信完了：MCCTRLレジスタ書き換え->次回受信 */
			MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
			req = MCTRL_SD1_END1 | MCTRL_RE | MCTRL_CM
					| (pInfo->C2Sel << MCTRL_C1C2_BIT) | pInfo->SeqNo;
			MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		}
		else if(ctrl == MCTRL_SD0_END1)
		{/* Handshake Error */
			mldrv_SetError(pInfo, ML_HNDSHK_ERR, MCTRL_SD0_END0 | MCTRL_RE);
			return CH_IDLE_S;
		}
	}
	else				/* 受信完了状態 */
	{/* データ保存 */
		ctrl = MLLSI_RD_REG(pInfo->pRegMBD);
		Mldrv_read_lnk(pInfo->pRegMBD, &pInfo->pRxBuf[pInfo->RxSize], pInfo->pktSz);
		pInfo->RxSize += pInfo->pktSz;
	/* データENDチェック */
		if((ctrl & MCTRL_SDEND_MSK) == MCTRL_SD1_END1)
		{/* 受信完了 */
			mldrv_SetError(pInfo, ML_FUNC_SUCCESS, MCTRL_SD0_END1 | MCTRL_RE);
			return CH_IDLE_S;
		}
	/* 次回受信バッファ容量チェック */
		if(pInfo->RxSize > ML_MSGBUF_SZ)
		{/* バッファオーバ */
			mldrv_SetError(pInfo, ML_RXBUF_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		pInfo->SeqNo = (pInfo->SeqNo+1) & 0xF;
		req = MCTRL_SD1_END0 | MCTRL_RE | MCTRL_CM
				| (pInfo->C2Sel << MCTRL_C1C2_BIT) | pInfo->SeqNo;
		MLLSI_WR_REG(pInfo->pRegMCTRL, req);
	}
	return CH_W_DATA;
}

/*****************************************************************************
 マルチパケットメッセージ状態遷移処理(送信要求待ち状態)
*****************************************************************************/
UCHAR mldrv_WSYNCR(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req, end;
	UCHAR rc;

	/* ローカル変数初期化	*/
	rc = CH_W_SYNCR;
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	ctrl = pInfo->Rx_CTRL & MCTRL_SDEND_MSK;
	
	if(req & MCTRL_RE)
	{/* 転送方向指定未受信 */
		if(pInfo->Rx_CTRL == MCTRL_ABORT)
		{/* Abort受信 */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		if(ctrl == MCTRL_SD0_END1)
		{/* データがきてたら再セット */
			MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
			req = pInfo->Rx_CTRL | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
			MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		}
		else if(ctrl == MCTRL_SD0_END0)
		{/* Handshake Error */
			mldrv_SetError(pInfo, ML_HNDSHK_ERR, MCTRL_SD0_END0 | MCTRL_RE);
			rc = CH_IDLE_S;
		}
	}
	else
	{/* 転送方向指定受信 */
		pInfo->SeqNo = 0;
		pInfo->TxCnt = 0;
		end = ((pInfo->TxSize - pInfo->pktSz) <= 0) ? MCTRL_END: 0;
		req = MCTRL_TE | MCTRL_SD | MCTRL_CM | end
					| (pInfo->C2Sel << MCTRL_C1C2_BIT) | pInfo->SeqNo;
		Mldrv_write_lnk(pInfo->pRegMBD, pInfo->pTxBuf, pInfo->pktSz, (UCHAR)req);
		MLLSI_WR_REG(pInfo->pRegMCTRL, (req | MCTRL_END));
		rc = CH_W_POLL;
	}

	return rc;
}

/*****************************************************************************
 マルチパケットメッセージ状態遷移処理(データ送信状態)
*****************************************************************************/
UCHAR mldrv_WPOLL(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req, end;
	USHORT sz;

	/* ローカル変数初期化	*/
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	ctrl = pInfo->Rx_CTRL;
	
	if(req & MCTRL_TE)
	{/* 未送信状態 */
		if(ctrl == MCTRL_ABORT)
		{/* Abort受信 */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		if((ctrl & MCTRL_SDEND_MSK) == MCTRL_SD0_END0)
		{/* Handshake Error */
			mldrv_SetError(pInfo, ML_HNDSHK_ERR, MCTRL_SD0_END0 | MCTRL_RE);
			return CH_IDLE_S;
		}
		if(((ctrl & MCTRL_SD) != 0) &&
			 ((ctrl & MCTRL_SNMSK) == pInfo->SeqNo))
		{/* ENDのみ不一致：MCCTRLレジスタ書き換え->次回送信 */
			if((req ^ ctrl) & MCTRL_END)
			{
				MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
				req = MCTRL_TE | MCTRL_SD | MCTRL_CM | (ctrl&MCTRL_END)
					| (pInfo->C2Sel << MCTRL_C1C2_BIT) | pInfo->SeqNo;
				MLLSI_WR_REG(pInfo->pRegMCTRL, req);
			}
		}
	}
	else				/* 1パケット送信完了 */
	{/* 送信完了チェック */
		pInfo->TxCnt += pInfo->pktSz;
		if(pInfo->TxSize <= pInfo->TxCnt)
		{/* 送信完了 */
			mldrv_SetError(pInfo, ML_FUNC_SUCCESS, MCTRL_SD0_END0 | MCTRL_RE);
			return CH_IDLE_S;
		}
	/* バッファ容量チェック */
		sz = pInfo->TxCnt + pInfo->pktSz;
		if(sz > ML_MSGBUF_SZ)
		{/* バッファオーバ */
			mldrv_SetError(pInfo, ML_TXBUF_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
	/* 次回送信データ更新 */
		pInfo->SeqNo = (pInfo->SeqNo+1) & 0xF;
		ctrl = MCTRL_TE | MCTRL_SD | MCTRL_CM
					| (pInfo->C2Sel << MCTRL_C1C2_BIT) | pInfo->SeqNo;
		end = (pInfo->TxSize <= sz) ? MCTRL_END: 0;
		Mldrv_write_lnk(pInfo->pRegMBD, 
			&pInfo->pTxBuf[pInfo->TxCnt], pInfo->pktSz, (UCHAR)(ctrl | end));
		MLLSI_WR_REG(pInfo->pRegMCTRL, (ctrl | (req & MCTRL_END)));
	}
	return CH_W_POLL;
	
}

/*****************************************************************************
 マルチパケットメッセージ状態遷移処理(エラーセット処理)
*****************************************************************************/
void mldrv_SetError(MLMSG_INFO* pInfo, SHORT code, ULONG mctrl)
{
	MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
	if(mctrl == MCTRL_ABORT)
	{
		pInfo->ABT_R = MCTRL_ABORT | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
		mctrl = MCTRL_SD0_END0 | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
	}
	else
	{
		mctrl = mctrl | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
	}
	MLLSI_WR_REG(pInfo->pRegMCTRL, mctrl);
	pInfo->Error = code;
	pInfo->MRX_R = pInfo->MTX_R = FALSE;
}




