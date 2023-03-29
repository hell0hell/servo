/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g		    */
/*                                                                          */
/*            JL-076 �ʐM�h���C�o	JL-076 NetWortk Driver					*/
/*																			*/
/****************************************************************************/
/* ��`�֐� :                                                               */
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	�y���ŗ����z															*/
/*		Ver1.00																*/
/*		Ver1.01		JL-052�s��΍�										*/
/*                  ���W�X�^EC0��ISRS=5���Z�b�g(WR0*����PRETX���o�͂��A		*/
/*                  ���̊��Ԃ�H/W�Ń��C�����V�[�o��Disable����)	<V580>		*/
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
	MLMSG_INFO	msgInfo[2];		/* ���b�Z�[�WCHInfo 					*/
	ND_PARAM	nd_prm;			/* �ʐM�p�����[�^						*/
	USHORT		errSts;			/* �G���[�X�e�[�^�X						*/

	VUSHORT		*pRegSTS;		/* ASIC�X�e�[�^�X���W�X�^�ւ̃|�C���^	*/
	UCHAR		*pRegRxD;		/* ASIC��M�o�b�t�@�ւ̃|�C���^			*/
	UCHAR		*pRegTxD;		/* ASIC���M�o�b�t�@�ւ̃|�C���^			*/
	UCHAR		*pRegSFD;		/* ASICGBL��M�o�b�t�@�ւ̃|�C���^		*/
	
} MLDRV_CHINFO;

/* M2�ʐM���荞�ݎg�p��` */
#define JL076_DEF_SET	(JL076_MD1_SYNCLOE | JL076_MD1_MLTST | JL076_MD1_MLTENA | JL076_MD1_MLRENA)

/* Local variables */
MLDRV_CHINFO mlCh;


static INT32 nd_jl076_Init(ND_PARAM* nd_prm);



/*****************************************************************************
*****************************************************************************/
/*	ASIC������ */
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
	
	/* ND_PARAM�̒l��ۑ�����B	*/
	mlCh.nd_prm.TCycle = nd_prm->TCycle;
	mlCh.nd_prm.TSlot = nd_prm->TSlot;
	mlCh.nd_prm.NodeAddress = nd_prm->NodeAddress;
	mlCh.nd_prm.DataSize = nd_prm->DataSize;
	mlCh.nd_prm.TSpeed = nd_prm->TSpeed;
	
	return rc;
}
/****************************************************************************/
/* �T�v�FJL-076�̏��������s���B												*/
/****************************************************************************/
/* ����		�Fprm ASIC�ݒ�p�����[�^										*/
/* �߂�l	�F = ND_SUCCESS 			����I��							*/
/*   		:  = ND_INITVAL_ERR        �p�����[�^�ݒ�ُ�                   */
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
/*	�֐��� :	NdSendRSP()														*/
/*																				*/
/*	�@�\�T�v:																	*/
/*		�����f�[�^���M�����i�����ʐM�j											*/
/*																				*/
/*==============================================================================*/
/*																				*/
/*	���� :																		*/
/*		INT ch					�`�����l���ԍ�									*/
/*		ULONG* lnk_rbuff		���M�f�[�^�̑��M�o�b�t�@						*/
/*	�߂�l :																	*/
/*		= ND_SUCCESS			����I��										*/
/*		= ��L�ȊO				�����f�[�^�������ݎ��s							*/
/*																				*/
/********************************************************************************/
void NdSendRSP( UINT st_no, ULONG *Buffer )
{

//		Mldrv_write_lnk(mlCh.pRegTxD[st_no], (UCHAR*)Buffer, 30, 0x01)
		Mldrv_write_lnk(mlCh.pRegTxD, (UCHAR*)Buffer, 30, 0x01);
}
/****************************************************************************/
/*	ASIC�ݒ胊�Z�b�g(�`�������Đݒ�) */
/*****************************************************************************
�����F	nd_prm	�E�E�E�ʐM�p�����[�^�ݒ�
�߂�l�Frc < 0	�E�E�E�G���[
		rc = 0	�E�E�E����
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


	/* ND_PARAM�̒l��ۑ�����B	*/
	mlCh.nd_prm.TCycle = nd_prm->TCycle;
	mlCh.nd_prm.TSlot = nd_prm->TSlot;
	mlCh.nd_prm.NodeAddress = nd_prm->NodeAddress;
	mlCh.nd_prm.DataSize = nd_prm->DataSize;
	mlCh.nd_prm.TSpeed = nd_prm->TSpeed;

	return ND_SUCCESS;
}

/*****************************************************************************
	���b�Z�[�W����M(�O���[�o�����荞�ݏ���)
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
	���b�Z�[�W����M(�񓯊��A�N�Z�X�p����)
*****************************************************************************/
void NdExchangeMsgPktAsync(void)
{
	USHORT errSts;
	
	errSts = (USHORT)MLLSI_RD_REG(mlCh.pRegSTS);
	errSts = (errSts >> 4) & errSts;	/* �O��`�����ƒ��l�̎�M��Ԃ�AND  */

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
/*	�T�C�N���b�N�f�[�^��M */
/*****************************************************************************
�����F	ch		�E�E�E��Mslot(Reserve)
		pBuf	�E�E�E��M�f�[�^�i�[��
�߂�l�Frc < 0	�E�E�E��M�G���[
		rc > 0	�E�E�E��M����
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
/* �T�v�F�O���[�o���f�[�^�i�`�������A�^�C���X���b�g�j�̎擾���s���B			*/
/****************************************************************************/
/* ����		�FUSHORT* t_cyc		�`������[0.25us]							*/
/*			�FUSHORT* t_slot	�^�C���X���b�g[0.25us]						*/
/* �߂�l	�F= 0 ����I��													*/
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
	ASIC�X�e�[�^�X�̎擾(�O���[�o�����荞�݂̍ŏ��ɃR�[��)
*****************************************************************************/
INT32 NdGetDevStatus(ULONG *ests)
{
/* Save Error Status */
	*ests = 0;
	mlCh.errSts = (USHORT)MLLSI_RD_REG(mlCh.pRegSTS);
	return ND_SUCCESS;
}

/*****************************************************************************
	�}���`�p�P�b�g���b�Z�[�W�����E�g�����C������
*****************************************************************************/
void NdControlMsgPkt(void)
{
	Mldrv_ControlMsg(&mlCh.msgInfo[0]);
	Mldrv_ControlMsg(&mlCh.msgInfo[1]);
}

/*****************************************************************************
	���b�Z�[�W��M�J�n�v���Z�b�g
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
		ND_RECV_BUSY	�E�E�E��M��(�v�����s���s)
		ND_SUCCESS		�E�E�E����I��
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
	���b�Z�[�W��M�����`�F�b�N
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
		rcvWSz	�E�E�E��M���b�Z�[�W���[�h�T�C�Y
		rcvBSz	�E�E�E��M���b�Z�[�W�o�C�g�T�C�Y�i���0�j
	�߂�l�F
		ND_RECV_BUSY	�E�E�E��M��, or ��M�҂����
		ND_SUCCESS		�E�E�E��M����
		����ȊO		�E�E�E��M�G���[(�n���h�V�F�C�N�G���[��)
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
	���b�Z�[�W�f�[�^���[�h
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
		rcvData	�E�E�E��M�f�[�^�i�[��
		rdSz	�E�E�E���[�h���[�h��
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
	���b�Z�[�W�f�[�^���C�g
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
		sndData	�E�E�E���M�f�[�^�i�[��
		wrSz	�E�E�E���C�g�o�C�g��
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
	���b�Z�[�W���M�J�n�v���Z�b�g
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
		ND_SEND_BUSY	�E�E�E���M��(�v�����s���s)
		ND_SUCCESS		�E�E�E����I��
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
	���b�Z�[�W���M�����`�F�b�N
	�����F
		ch = 1	�E�E�EC1���b�Z�[�WCH		ch != 1	�E�E�EC2���b�Z�[�WCH
	�߂�l�F
		ND_SEND_BUSY	�E�E�E���M��
		ND_SUCCESS		�E�E�E���M����
		����ȊO		�E�E�E���M�G���[(�n���h�V�F�C�N�G���[��)
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
	�ʐM�p�����[�^�擾
	�����F
	�߂�l�F*ND_PRAM	//kira_for_MEMOBUS_Message
*****************************************************************************/
ND_PARAM* NdGetNdParamPtr( VOID )
{
	return (&mlCh.nd_prm);

}

/*****************************************************************************
	�ʐMASIC���Z�b�g (�ʐM��~) <S14E>
	�����FDeviceAddress �ʐMASIC�擪�A�h���X
	�߂�l�F
*****************************************************************************/
void NdResetAsic( void *DeviceAddress )
{
	/* Stop Communication */
	*(ULONG*)((ULONG)DeviceAddress + JL076_MLSET1_ADR) = 0;
	return;
}
