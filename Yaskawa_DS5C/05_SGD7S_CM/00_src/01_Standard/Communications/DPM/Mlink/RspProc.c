/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g          */
/*                                                                          */
/*            �e�R�}���h���X�|���X�쐬���� 									*/
/*																			*/
/****************************************************************************/
/* �h�e��`�֐� :                                                           */
/*																			*/
/*                                                                          */
/****************************************************************************/
/*	�y���ŗ����z															*/
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
/* �T�v		�F�X�e�[�^�X����												*/
/****************************************************************************/
/* ����		�F	RSP_HNDL *hdl												*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
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

	/* ���[�J���ϐ��ݒ�	*/
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
	
	/*---- D5(MLOCK) --------------------------------------------------------*//* ���Ή�	*/
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
	sts |= pRspHdl->last_L_CMP << RSP_LCMP_BIT_NO /* <S04A> L_CMP�r�b�g�ԍ��ԈႢ RSP_TRQ_LIM_BIT_NO */;
	
	/*---- D10(P-SOT) -------------------------------------------------------*/
	bitwk = psn_hdl_ptr->SoftLimit & 1;
	sts |= (bitwk << RSP_P_SOFT_LIM_BIT_NO);

	/*---- D11(N-SOT) -------------------------------------------------------*/
	bitwk = (psn_hdl_ptr->SoftLimit >> 1) & 1;
	sts |= (bitwk << RSP_N_SOFT_LIM_BIT_NO);

	/*---- ���䃂�[�h�ˑ� ---------------------------------------------------*/
	if (pMtHdl->cmdData.CtrlMode == MT_POS_CTRLMD)
	{/* �ʒu���� */
		/*---- D7(PSET) -----------------------------------------------------*/
		sts |= psn_hdl_ptr->Pset << RSP_PSET_BIT_NO;
		/*---- D8(DEN) ------------------------------------------------------*/
		sts |= psn_hdl_ptr->Den << RSP_DEN_ON_BIT_NO;
		/*---- D11(NEAR) ----------------------------------------------------*/
		sts |= psn_hdl_ptr->Near << RSP_NEAR_BIT_NO;
	}
	else if (pMtHdl->cmdData.CtrlMode == MT_VEL_CTRLMD)
	{/* ���x���� */
		/*---- D7(V_CMP) ----------------------------------------------------*/
		bitwk = (RspCtrlBit >> REACH_SPD_BITNO) & 1;
		sts |= bitwk << RSP_PSET_BIT_NO;
		/*---- D8(ZSPD) -----------------------------------------------------*/
		bitwk = ~(RspCtrlBit >> MOTOR_MOVING_BITNO) & 1;/* <S06B>:�o�͐M���𔽓] */
		sts |= bitwk << RSP_DEN_ON_BIT_NO;
		/*---- D11(NEAR) ----------------------------------------------------*/
		sts |= psn_hdl_ptr->Near << RSP_NEAR_BIT_NO;
	}
	else
	{/* �g���N���� */
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
/* �T�v		�F�T�u�X�e�[�^�X����											*/
/****************************************************************************/
/* ����		�F	RSP_HNDL *hdl												*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
UCHAR rspMakeExStatus(CMD_HNDL *cmdHdl, ALM_HNDL *almHdl)
{
	UCHAR	sts;
	
	/* ���[�J���ϐ����N���A	*/
	sts = 0;
	
	cmdHdl->exCmdInfo.warng &= almHdl->wrnSts;
	sts |= cmdHdl->exCmdInfo.warng << RSP_SUBCMD_WRN_BIT_NO;			/* D1(WARNG)	*/
	sts |= cmdHdl->exCmdInfo.cmdRdy << RSP_SUBCMD_READY_BIT_NO;		/* D2(CMDRDY)	*/

	return sts;
}

/****************************************************************************/
/* �T�v		�F�V�X�e���R�}���h���X�|���X����						(??H)	*/
/****************************************************************************/
/* �Ή��R�}���h�F	����`/PRM_WR/PPRM_WR/ADJ								*/
/****************************************************************************/
/* ����		�F	MLNET_HANDLE *hNetData										*/
/*			�F	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID rsp_SYS_CMD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
}

/****************************************************************************/
/* �T�v		�FNOP,CONFIG,SYNC_SET,DISCONNECT ���X�|���X����	(04/0D/0FH)		*/
/****************************************************************************/
VOID rsp_NOP(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}

/****************************************************************************/
/* �T�v		�F�o�q�l�Q�q�c�R�}���h���X�|���X����					(01H)	*/
/****************************************************************************/
VOID rsp_PRM_RD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{/* �p�����[�^�ǂݍ��݊���	*/
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
/* �T�v		�F�h�c�Q�q�c�R�}���h���X�|���X����						(03H)	*/
/****************************************************************************/
VOID rsp_ID_RD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	
	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{/* �p�����[�^�ǂݍ��݊���	*/
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
/* �T�v		�F�`�k�l�Q�q�c�R�}���h���X�|���X����					(05H)	*/
/****************************************************************************/
VOID rsp_ALM_RD(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */

	if(hNetData->cmd_hdl.cmdInfo.cmdRdy != FALSE)
	{/* �p�����[�^�ǂݍ��݊���	*/
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
/* �T�v		�F�`�k�l�Q�b�k�q�R�}���h���X�|���X����					(06H)	*/
/****************************************************************************/
VOID rsp_ALM_CLR(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT	iomon;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	

	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->byte[4] = rslt_wk->alm.mode;

	/*---------------------------------------------------------------*/
	/*IO_MON�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
    if((hNetData->cmd_hdl.cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)      
    {
	     /* I/O���͐M����� */		
		iomon = (MciGetInPortSignals(&hNetData->mt_hdl) << 12);
	    iomon |= MciGetInputSignals(&hNetData->mt_hdl);
	    UtilSetWData(rsp_buf->mt.io_mon, iomon);
     }
	/*---------------------------------------------------------------*/
}

/****************************************************************************/
/* �T�v		�F�b�n�m�m�d�b�s�R�}���h���X�|���X����					(0EH)	*/
/****************************************************************************/
VOID rsp_CONNECT(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->byte[4] = rslt_wk->connect.ver;
	rsp_buf->byte[5] = rslt_wk->connect.com_mod;
	rsp_buf->byte[6] = rslt_wk->connect.com_time;
}

/****************************************************************************/
/* �T�v		�F�o�n�r�Q�r�d�s�R�}���h���X�|���X����					(20H)	*/
/****************************************************************************/
VOID rsp_POS_SET(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT	iomon;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	

	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
	rsp_buf->byte[4] = rslt_wk->pos_set.ps_cmd;
	rsp_buf->byte[5] = rslt_wk->pos_set.pos[0];
	rsp_buf->byte[6] = rslt_wk->pos_set.pos[1];
	rsp_buf->byte[7] = rslt_wk->pos_set.pos[2];
	rsp_buf->byte[8] = rslt_wk->pos_set.pos[3];

	/*---------------------------------------------------------------*/
	/*IO_MON�t�B�[���h�L��(YEA�˗��̑Ή�)                            */
	/*---------------------------------------------------------------*/
    if((hNetData->cmd_hdl.cmdInfo.chkOpIoCmd & 0x0001) == 0x0001)
    {
	     /* I/O���͐M����� */		
		iomon = (MciGetInPortSignals(&hNetData->mt_hdl) << 12);
	    iomon |= MciGetInputSignals(&hNetData->mt_hdl);
	    UtilSetWData(rsp_buf->mt.io_mon, iomon);
     }
	/*---------------------------------------------------------------*/
}

/****************************************************************************/
/* �T�v		�F�`�c�i�R�}���h���X�|���X����							(3EH)	*/
/****************************************************************************/
VOID rsp_ADJ(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	UCHAR  subcode;
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	subcode = rslt_wk->adj2.subcode;

	if(subcode == 0x00)
	{/* ���|�U�݊����[�h�̏ꍇ */
		rspADJ_sigma2_mode(hNetData, rsp_buf);
	}
	else if(subcode == 0x01)
	{/* �ʏ탂�[�h�̏ꍇ */
		rspADJ_normal_mode(hNetData, rsp_buf);
	}
	else
	{/* OPTION/IO_MON�g�����[�h�A�����������[�h	*/
		rspADJ_other_mode(hNetData, rsp_buf);
	}
}

/*--- ���|�U�݊����[�h�̏ꍇ ------------------------------------------------*/
VOID rspADJ_sigma2_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT ccmd;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	ccmd = rslt_wk->adj2.ccmd;
	
	if( (ccmd == 0)
		&&(hNetData->cmd_hdl.cmdInfo.cmdRdy == FALSE) )
	{/* CCMD = 0(�ǂݏo��) && CmdRdy = Flase :�� �ǂݍ��ݒ�	*/
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
/*--- �ʏ탂�[�h�̏ꍇ ------------------------------------------------------*/
VOID rspADJ_normal_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	USHORT ccmd;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	ccmd = rslt_wk->adj.ccmd;
	
	if( (ccmd == 0x03)
		&&(hNetData->cmd_hdl.cmdInfo.cmdRdy == FALSE) )
	{/* CCMD = 3(�ǂݏo��) && CmdRdy = Flase :�� �ǂݍ��ݒ� */
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
/*--- OPTION/IO_MON�g�����[�h�A�����������[�h -------------------------------*/
VOID rspADJ_other_mode(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
{
	CMD_LINK_DATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );
	
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->byte[14] = 0;
	
}


/****************************************************************************/
/* �T�v		�F���j�^�L��R�}���h���X�|���X����						(**H)	*/
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
	UCHAR		crtlMode;			/* ���ݐ��䃂�[�h*/
	UCHAR		sel_mon1,sel_mon2;	/* ���j�^�I����� */

	/* ���[�J���ϐ��̏����� */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;

	/* ���ݐ��䃂�[�h�擾 */
	crtlMode = pMtHdl->cmdData.CtrlMode;

	/* ���j�^�R�[�h1���o */
	sel_mon1 = pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 & 0x0F;		/* SEL_MON1�𒊏o */

	/* ���j�^�R�[�h2���o */
	if (pRspHdl->enableLtMon)
	{	/* ���b�`�������: ���j�^2��LPOS */
		sel_mon2 = LPOS_MON;
	}
	else
	{	/* ���j�^�R�[�h2���o */
		sel_mon2 = (pCmdHdl->cmdInfo.rsltBuf.mt.sel_mon12 & 0x0F0) >> 4;	/* D4�`D7��SEL_MON2�𒊏o */
	}

	/* ���j�^�f�[�^1/2 */
	rsp_buf->mt.mon1 = MciGetMonitorData(pMtHdl, sel_mon1, crtlMode);
	rsp_buf->mt.mon2 = MciGetMonitorData(pMtHdl, sel_mon2, crtlMode);

	/* ���j�^�I�� */
	rsp_buf->mt.sel_mon12 = (sel_mon1 | (sel_mon2 << 4));

     /* I/O���͐M����� */		
	iomon = (MciGetInPortSignals(&hNetData->mt_hdl) << 12);
    iomon |= MciGetInputSignals(&hNetData->mt_hdl);
    UtilSetWData(rsp_buf->mt.io_mon, iomon);
}

/****************************************************************************/
/* �T�v		�F�V�X�e���R�}���h���X�|���X����						(??H)	*/
/****************************************************************************/
/* �Ή��R�}���h�F	����`/PRM_RD/PRM_WR/PPRM_WR/ALM_RD/					*/
/****************************************************************************/
/* ����		�F	MLNET_HANDLE *hNetData										*/
/*			�F	RSP_LINK_EXDATA* 	ex_rsp_buf	RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID exRsp_SYS_CMD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->word[1] = rslt_wk->word[1];
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
}
/****************************************************************************/
/* �T�v		�F�m�n�o�R�}���h���X�|���X����							(00H)	*/
/****************************************************************************/
VOID exRsp_NOP(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	rsp_buf->word[1] = 0;
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}
/****************************************************************************/
/* �T�v		�F�o�q�l�Q�q�c�R�}���h���X�|���X����					(01H)	*/
/****************************************************************************/
VOID exRsp_PRM_RD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	CMD_LINK_EXDATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{/* �p�����[�^�ǂݍ��݊���	*/
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
/* �T�v		�F�`�k�l�Q�q�c�R�}���h���X�|���X����					(05H)	*/
/****************************************************************************/
VOID exRsp_ALM_RD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	CMD_LINK_EXDATA* rslt_wk;
	
	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );
	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */

	if(hNetData->cmd_hdl.exCmdInfo.cmdRdy != FALSE)
	{/* �p�����[�^�ǂݍ��݊���	*/
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
/* �T�v		�FLTMOD_ON/LTMOd_OFF/SMON �R�}���h���X�|���X����				*/
/****************************************************************************/
VOID exRsp_MON_CMD(MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* rsp_buf)
{
	RSP_HNDL	*pRspHdl;
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	LONG		mon3,mon4;			/* ���j�^�R�C�S */
	UCHAR		crtlMode;			/* ���ݐ��䃂�[�h*/
	UCHAR		sel_mon3,sel_mon4;	/* ���j�^�I����� */

	/* ���[�J���ϐ��̏����� */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;

	/* ���ݐ��䃂�[�h�擾 */
	crtlMode = pMtHdl->cmdData.CtrlMode;

	/* ���j�^�R�[�h3/4���o */
	sel_mon3 = pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 & 0x0F;			/* SEL_MON3�𒊏o */
	sel_mon4 = (pCmdHdl->exCmdInfo.rsltBuf.mon.sel_mon34 & 0x0F0) >> 4;	/* SEL_MON4�𒊏o */

	/* ���j�^3/4�f�[�^�擾 */
	mon3 = MciGetMonitorData(pMtHdl, sel_mon3, crtlMode);
	mon4 = MciGetMonitorData(pMtHdl, sel_mon4, crtlMode);

	/* ���j�^�I�� */
	rsp_buf->mon.sel_mon = (sel_mon3 | (sel_mon4 << 4));

	/* ���j�^3/4 */
	UtilSetDWData(rsp_buf->mon.mon3, mon3);
	UtilSetDWData(rsp_buf->mon.mon4, mon4);

	/* EX_STATUS */
	UtilSetWData(rsp_buf->mon.exSts, pMtHdl->latch.SeqSts.wdCnt);
}
