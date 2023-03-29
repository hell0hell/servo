/****************************************************************************/
/*                                                                          */
/* Module : Common header of ML2 Driver module                              */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/* History:                                                                 */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*******************  2006 Yaskawa Electric Corporation *********************/
#ifndef		JLCOM_DRV_H_
#define		JLCOM_DRV_H_

#include "BaseDef.h"

/* Message buffer size */
#define ML_MSGBUF_SZ		512		/* 480+32 */

/* Error code */
#define ML_FUNC_SUCCESS		(0)
#define ML_RAMCHECK_ERR		(-1)
#define ML_RECV_ERR			(-2)
#define ML_ABORT_ERR		(-3)
#define ML_HNDSHK_ERR		(-4)
#define ML_RXBUF_ERR		(-5)
#define ML_TXBUF_ERR		(-6)

/* M-2 CTRL Header */
#define MCTRL_SD			0x00000080UL
#define MCTRL_END			0x00000040UL
#define MCTRL_C1C2			0x00000020UL
#define MCTRL_CM			0x00000010UL
#define MCTRL_RE			0x00000100UL
#define MCTRL_TE			0x00000200UL

#define MCTRL_C1C2_BIT		5

#define MCTRL_C1C2_CM_MSK	0x000000CFUL
#define MCTRL_ENDCLR		0x000000BFUL
#define MCTRL_SNMSK			0x0000000FUL
#define MCTRL_SNCLR			0x000000F0UL
#define MCTRL_ABORT			(MCTRL_END | MCTRL_SNMSK)

#define MCTRL_SDEND_MSK		0x000000C0UL
#define MCTRL_SD0_END0		0x00000000UL
#define MCTRL_SD0_END1		MCTRL_END
#define MCTRL_SD1_END0		MCTRL_SD
#define MCTRL_SD1_END1		(MCTRL_SD | MCTRL_END)

/* ASIC access Interface macro */
#define MLLSI_RD_REG(pJL)		*((VULONG*)(pJL))
#define MLLSI_WR_REG(pJL, data)	*((VULONG*)(pJL)) = (ULONG)(data)


/* typedefs of struct type */
typedef union _MLWD_DATA {
	USHORT wd;
	UCHAR  byte[2];
} MLWD_DATA;

typedef union _MLDWD_DATA {
	ULONG 	dwd;
	USHORT	wd[2];
	UCHAR	byte[4];
} MLDWD_DATA;

typedef struct tagMLMSG_INFO {
	UCHAR	C2Sel;						/* 0: C1, 1: C2								*/
	UCHAR	State;						/* ���No									*/
	SHORT	Error;						/* Error���								*/
	UCHAR	MRX_R;						/* ��M�v��									*/
	UCHAR	MTX_R;						/* ���M�v��									*/

	UCHAR	SeqNo;						/* Seq No. ���Ғl							*/
	UCHAR	pktSz;						/* 1 packet size							*/
	SHORT	RxSize;						/* ��M�σo�C�g��							*/
	SHORT	TxSize;						/* ���M�o�C�g��								*/
	SHORT	TxCnt;						/* ���M�σo�C�g��							*/
	ULONG	Rx_CTRLd;					/* ��MCTRL�f�[�^(ASIC���l)					*/
	ULONG	Rx_CTRL;					/* ��MCTRL�f�[�^							*/
	ULONG	ABT_R;						/* �A�{�[�g���M�v��							*/
	UCHAR	pRxBuf[ML_MSGBUF_SZ];		/* ��M�f�[�^�o�b�t�@						*/
	UCHAR	*pTxBuf;					/* ���M�f�[�^�o�b�t�@�|�C���^				*/

	UCHAR	*pRegMCTRL;					/* ASIC���b�Z�[�W���䃌�W�X�^�ւ̃|�C���^	*/
	UCHAR	*pRegMBD;					/* ASIC����M�o�b�t�@���W�X�^�ւ̃|�C���^	*/
	MLDWD_DATA	RegTMC;					/* ���M���b�Z�[�W�f�[�^						*/
} MLMSG_INFO;


#ifdef __cplusplus
extern	"C"	{
#endif
extern int Mldrv_ramcheck( UCHAR *pJL, USHORT chk_data, USHORT size);
extern void Mldrv_read_lnk(UCHAR *pJL, UCHAR *pBuf, UINT size);
extern void Mldrv_write_lnk(UCHAR *pJL, UCHAR *pBuf, UINT size, UCHAR ctrl);
extern void Mldrv_get_comtime(UCHAR *pJL, USHORT *t_cyc, USHORT *t_slt);

extern void Mldrv_ExchangeMsg(MLMSG_INFO* pInfo, USHORT rcvSts, UCHAR* pRegRxD, UCHAR* pRegTxD);
extern void Mldrv_ExchangeMsgCtrl(MLMSG_INFO* pInfo, USHORT rcvSts, UCHAR* pRegRxD, UCHAR* pRegTxD);
extern void Mldrv_ControlMsg(MLMSG_INFO* pInfo);

#ifdef __cplusplus
}
#endif


#endif /* JLCOM_DRV_H_ */
