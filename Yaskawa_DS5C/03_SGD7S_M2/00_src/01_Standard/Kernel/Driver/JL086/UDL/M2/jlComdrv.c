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
	data.byte[0] = ctrl;		/* CTRL�R�[�h�Z�b�g */
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
	CH_IDLE_S=0,	/* IDLE��� */
	CH_W_SYNCT,		/* ��M�]�������҂���� */
	CH_W_DATA,		/* �f�[�^��M�҂���� */
	CH_W_SYNCR,		/* ���M�]�������҂���� */
	CH_W_POLL,		/* �f�[�^�]������� */
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
 ���b�Z�[�W�f�[�^��������(�O���[�o�����荞�ݏ���:JL076�p)
******************************************************************************
 ����:	rcvSts	- Message��M���(0:�ُ�, 0�ȊO:����)
		pRegRxD	- ASIC���b�Z�[�W��M�o�b�t�@�A�h���X
		pRegTxD	- ASIC���b�Z�[�W���M�o�b�t�@�A�h���X
*****************************************************************************/
void Mldrv_ExchangeMsgCtrl(MLMSG_INFO* pInfo, USHORT rcvSts, UCHAR* pRegRxD, UCHAR* pRegTxD)
{
	ULONG reg;

	pInfo->Rx_CTRLd = TRUE;
	if(pInfo->ABT_R != 0)
	{/* ABT�v������ */
		MLLSI_WR_REG(pRegTxD, pInfo->ABT_R);
		pInfo->Rx_CTRL = 0;
		pInfo->ABT_R = 0;
	}
	else if(rcvSts != 0)
	{/* ��M����Ȃ�,����CTRL�R�[�h�X�V */
		reg = MLLSI_RD_REG(pRegRxD);
		pInfo->Rx_CTRL = reg & MCTRL_C1C2_CM_MSK;
	}
}

/*****************************************************************************
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ������C��
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
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ���(IDLE���)
*****************************************************************************/
UCHAR mldrv_IDLE(MLMSG_INFO* pInfo)
{
	ULONG req, val;

	if(pInfo->ABT_R) return CH_IDLE_S;

	if(pInfo->MRX_R)
	{/* ��M�v������ */
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
	{/* ���M�v������ */
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
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ���(��M�v���҂����)
*****************************************************************************/
UCHAR mldrv_WSYNCT(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req;
	UCHAR rc;
	
	/* ���[�J���ϐ�������	*/
	rc = CH_W_SYNCT;
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	ctrl = pInfo->Rx_CTRL & MCTRL_SDEND_MSK;
	
	if(req & MCTRL_RE)
	{/* �]�������w�薢��M */
		if(pInfo->Rx_CTRL == MCTRL_ABORT)
		{/* Abort��M */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		if(ctrl == MCTRL_SD0_END0)
		{/* �f�[�^�����Ă���ăZ�b�g */
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
	{/* �]�������w���M */
		pInfo->SeqNo = 0;
		pInfo->RxSize = 0;
		req = MCTRL_SD1_END0 | MCTRL_RE | MCTRL_CM | (pInfo->C2Sel << MCTRL_C1C2_BIT);
		MLLSI_WR_REG(pInfo->pRegMCTRL, req);
		rc = CH_W_DATA;
	}
	return rc;
}

/*****************************************************************************
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ���(�f�[�^��M���)
*****************************************************************************/
UCHAR mldrv_WDATA(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req;

	/* ���[�J���ϐ�������	*/
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	
	if(req & MCTRL_RE)
	{/* ����M��� */
		if(pInfo->Rx_CTRL == MCTRL_ABORT)
		{/* Abort��M */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		ctrl = pInfo->Rx_CTRL & MCTRL_SDEND_MSK;
		if(ctrl == MCTRL_SD1_END1)
		{/* ��M�����FMCCTRL���W�X�^��������->�����M */
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
	else				/* ��M������� */
	{/* �f�[�^�ۑ� */
		ctrl = MLLSI_RD_REG(pInfo->pRegMBD);
		Mldrv_read_lnk(pInfo->pRegMBD, &pInfo->pRxBuf[pInfo->RxSize], pInfo->pktSz);
		pInfo->RxSize += pInfo->pktSz;
	/* �f�[�^END�`�F�b�N */
		if((ctrl & MCTRL_SDEND_MSK) == MCTRL_SD1_END1)
		{/* ��M���� */
			mldrv_SetError(pInfo, ML_FUNC_SUCCESS, MCTRL_SD0_END1 | MCTRL_RE);
			return CH_IDLE_S;
		}
	/* �����M�o�b�t�@�e�ʃ`�F�b�N */
		if(pInfo->RxSize > ML_MSGBUF_SZ)
		{/* �o�b�t�@�I�[�o */
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
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ���(���M�v���҂����)
*****************************************************************************/
UCHAR mldrv_WSYNCR(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req, end;
	UCHAR rc;

	/* ���[�J���ϐ�������	*/
	rc = CH_W_SYNCR;
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	ctrl = pInfo->Rx_CTRL & MCTRL_SDEND_MSK;
	
	if(req & MCTRL_RE)
	{/* �]�������w�薢��M */
		if(pInfo->Rx_CTRL == MCTRL_ABORT)
		{/* Abort��M */
			mldrv_SetError(pInfo, ML_ABORT_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
		if(ctrl == MCTRL_SD0_END1)
		{/* �f�[�^�����Ă���ăZ�b�g */
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
	{/* �]�������w���M */
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
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ���(�f�[�^���M���)
*****************************************************************************/
UCHAR mldrv_WPOLL(MLMSG_INFO* pInfo)
{
	ULONG ctrl, req, end;
	USHORT sz;

	/* ���[�J���ϐ�������	*/
	req = MLLSI_RD_REG(pInfo->pRegMCTRL);
	ctrl = pInfo->Rx_CTRL;
	
	if(req & MCTRL_TE)
	{/* �����M��� */
		if(ctrl == MCTRL_ABORT)
		{/* Abort��M */
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
		{/* END�̂ݕs��v�FMCCTRL���W�X�^��������->���񑗐M */
			if((req ^ ctrl) & MCTRL_END)
			{
				MLLSI_WR_REG(pInfo->pRegMCTRL, 0);
				req = MCTRL_TE | MCTRL_SD | MCTRL_CM | (ctrl&MCTRL_END)
					| (pInfo->C2Sel << MCTRL_C1C2_BIT) | pInfo->SeqNo;
				MLLSI_WR_REG(pInfo->pRegMCTRL, req);
			}
		}
	}
	else				/* 1�p�P�b�g���M���� */
	{/* ���M�����`�F�b�N */
		pInfo->TxCnt += pInfo->pktSz;
		if(pInfo->TxSize <= pInfo->TxCnt)
		{/* ���M���� */
			mldrv_SetError(pInfo, ML_FUNC_SUCCESS, MCTRL_SD0_END0 | MCTRL_RE);
			return CH_IDLE_S;
		}
	/* �o�b�t�@�e�ʃ`�F�b�N */
		sz = pInfo->TxCnt + pInfo->pktSz;
		if(sz > ML_MSGBUF_SZ)
		{/* �o�b�t�@�I�[�o */
			mldrv_SetError(pInfo, ML_TXBUF_ERR, MCTRL_ABORT);
			return CH_IDLE_S;
		}
	/* ���񑗐M�f�[�^�X�V */
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
 �}���`�p�P�b�g���b�Z�[�W��ԑJ�ڏ���(�G���[�Z�b�g����)
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




