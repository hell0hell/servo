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
/* ���ʃ��j�^ defines */
#define POS_CTRL 		1
#define SEL_APOS 		0
#define SEL_MPOS 		1
#define SEL_ALM_DTL		8
#define SEL_CMN1		0xC
#define SEL_CMN2		0xD

/* ���ʃ��j�^ SV_STAT */
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
/* �T�v		:	�X�e�[�^�X����												*/
/****************************************************************************/
/* ����		:	CMD_HNDL* pCmdHdl			CMD ANALYZER �̃n���h��			*/
/*			:	ALM_HNDL* pAlmHdl			ALM Manager �̃n���h��			*/
/*			:	CMD_LINK_DATA* rslt_wk		��͌��ʃo�b�t�@�̃|�C���^		*/
/* �߂�l	:	CMD_STAT �r�b�g���											*/
/****************************************************************************/
USHORT stdRspMakeCMD_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, USHORT sts )
{
	/* ��͌��ʃo�b�t�@��CMD_CTRL���R�s�[ */
	sts = sts & RSP_CLR_CMDSTAT_BITS;

	/* bit0 (D_ALM) */
	sts |= (pAlmHdl->M3StdAlm.DevAlm << RSP_DEV_ALM_BIT_NO);
	/* bit1 (D_WAR) */
	sts |= (pAlmHdl->M3StdAlm.DevWrn << RSP_DEV_WRN_BIT_NO);
	/* bit2 (CMDRDY) */
	sts |= (pCmdHdl->cmdInfo.cmdRdy << RSP_CMD_READY_BIT_NO);
	/* bit3 (ALMCLR) */
	sts |= (pCmdHdl->cmdInfo.almClrCmp << RSP_ALMCLR_CMP_BIT_NO);
	/* bit8�`11 (CMD_ALM) */
	sts |= (USHORT)(pAlmHdl->CmdAlmCode << RSP_CMD_ALM_BIT_NO);
	/* bit12�`15(COMM_ALM) */
	sts |= (USHORT)(pAlmHdl->CommAlmCode << RSP_COMM_ALM_BIT_NO);

	return sts;
}



/****************************************************************************/
/* �T�v		:	�T�[�{�R�}���h�X�e�[�^�X�t�B�[���h����						*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	CMD_LINK_DATA* rslt_wk		��͌��ʃo�b�t�@�̃|�C���^		*/
/* �߂�l	:	SVCMD_STAT �r�b�g���										*/
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

	/* ���[�J���ϐ��̏����� */
 	pRspHdl		= &hNetData->rsp_hdl;
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;

	/* ��͌��ʃo�b�t�@��CMD_CTRL���R�s�[ */
	sts.RspSvCmdStat = rslt_wk->stdData.mt.svcmd_ctrl & 0xFFFF0000;

	/*----------------------------------------*/
	/* �T�[�{�R�}���h�X�e�[�^�X�t�B�[���h�ݒ� */
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

	/* bit2�`3(Reserve) */
	/* bit4�`7(ACCFIL) */
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

	/* bit14�`15 (Reserve) */

	return sts.RspSvCmdStat;

}


/****************************************************************************/
/* �T�v		:	�T�[�{�R�}���h���o�͐M�����j�^����							*/
/****************************************************************************/
/* ����		:	MT_HNDL *pMtHdl												*/
/* �߂�l	:	SVCMD_IO �r�b�g���											*/
/****************************************************************************/
static	ULONG stdRspMakeSVCMD_IO( MT_HNDL *pMtHdl )
{
	RSP_SVCMD_IO	sts;					/* SVCMD_IO�r�b�g��� */
	PSN_HNDL		*psn_hdl_ptr;
	ULONG			RspCtrlBit,RspIoBit;
	BOOL			bitwk;

/* <S0AC>start */
	UCHAR	ucMonitorBit;		/* SI0�`SI6�̑S�r�b�g�ݒ肷��܂Ń��[�v */
	UCHAR	ucTmp;				/* Hi�� ON=1�ɂāASI0�`SI6�擾���� */
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

#if 0 /* 2011.01.05 Y.Oka PsnUpdateFBData()�ł�Pset,Near�쐬�����ǉ��ɂ�茩���� */
	/* bit12(DEN), bit14(PSET) */
	pos_err	= MlibLABS( psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos );
	if( psn_hdl_ptr->Den )
	{	
		sts.RspCmdIoBit |= RSP_CMD_DEN_BIT;
		if(pos_err <= psn_hdl_ptr->Prm->psetWidth)
		{ /* DEN=1 && (Tpos-Apos)<=�ʒu���ߊ����� */
			sts.RspCmdIoBit |= RSP_CMD_PSET_BIT;
		}
	}

	/* bit13(NEAR) */
	if( pos_err <= psn_hdl_ptr->Prm->nearWidth )
	{ /* (Tpos-Apos)<=NEAR�� */
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
	bitwk = ~(RspCtrlBit >> MOTOR_MOVING_BITNO) & 1;	/* <S028> : �o�͐M�������]���Ă����̂ŏC�� */
	sts.RspCmdIoBit |= (bitwk << RSP_CMD_ZSPD_BITNO);

	/* bit20�`23(Reserve) */
	/* bit24�`31(Vender Specific I/O) */

// #if 0/* <S0AC>start */
	/*------------------------------------------------------------------------------*/
	/* IO_STS1 24bit��																*/
	/* Pn860�`Pn866��SI0�`SI6�����肠�����Ă����ꍇ�ɁA�o�̓r�b�g���ݒ�			*/
	/* 2���\�����́ASI01�`SI06/SI10�`SI16�̊���t�����o�͂���						*/
	/* Safety�ɑΉ������ꍇ�́Awk�Ƀf�[�^��OR�ݒ肷��								*/
	/*------------------------------------------------------------------------------*/

	/* wk�̏����� */
	wk = 0;

	if( pMtHdl->mtPrm.InSigMonM3.bitsMsk )
	{ // 1data��enable�łȂ����IO OFF�ɂďI��
//		for( ucMonitorBit=0; ucMonitorBit < 7; ucMonitorBit++ )
		for( ucMonitorBit=0; ucMonitorBit < INPUT_SIGMON_BIT_NUM; ucMonitorBit++ )/* <S1C0> */
		{
			/* ���ꂼ���IO��Pn�ݒ���C�l�[�u���̎�����INPUT�f�[�^���擾���� */
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
			/* ���ꂼ���IO��Pn�ݒ���C�l�[�u���̎�����OUTPUT�f�[�^���擾���� */
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
/* �T�v		:	�T�u�R�}���h�X�e�[�^�X����									*/
/****************************************************************************/
/* ����		:	CMD_HNDL* pCmdHdl			CMD Manager�n���h��				*/
/*			:	ALM_HNDL* pAlmHdl			ALM Manager�n���h��				*/
/*			:	CMD_LINK_EXDATA* rslt_wk	��͌��ʃo�b�t�@�̃|�C���^		*/
/* �߂�l	:	SUB_STAT �r�b�g���											*/
/****************************************************************************/
ULONG stdRspMakeSUB_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, ULONG sts )
{
	ULONG	ex_cmd_alm;				/* SUBCMD_ALM */
	ULONG	sub_stat;
	
	sub_stat = sts & RSP_SUBCMD_CLR_BITS_MSK;

	/* bit2 (SUBCMDRDY) */
	sub_stat |= (pCmdHdl->exCmdInfo.cmdRdy << RSP_SUBCMD_READY_BIT_NO);

	/* bit8�`11 (SUBCMD_ALM) */
	ex_cmd_alm  = pAlmHdl->ExCmdAlmCode & 0x0F;
	sub_stat |= (ex_cmd_alm << RSP_SUBCMD_ALM_BIT_NO);

	return sub_stat;
}


/****************************************************************************/
/*					 ���C���R�}���h���X�|���X�쐬����						*/
/****************************************************************************/
/****************************************************************************/
/* �T�v		:	�V�X�e���R�}���h���X�|���X����						(??H)	*/
/****************************************************************************/
/* �Ή��R�}���h : ����`/MEM_WR/											*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_SYS_CMD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->dword[4] = rslt_wk->dword[4];
	rsp_buf->dword[5] = rslt_wk->dword[5];
	rsp_buf->dword[6] = rslt_wk->dword[6];
	rsp_buf->dword[7] = rslt_wk->dword[7];
}

/****************************************************************************/
/* �T�v		:	ID_RD�R�}���h���X�|���X����							(03H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_ID_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
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
/* �T�v		:	MEM_RD�R�}���h���X�|���X����						(1DH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_MEM_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
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
/* �T�v		:	NOP,SYNC_SET,DISCONNECT ���X�|���X����	(00/0D/0FH)			*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
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
/* �T�v		:	�b�n�m�e�h�f�R�}���h���X�|���X����					(04H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_CONFIG( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
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
/* �T�v		:	�`�k�l�QRD�R�}���h���X�|���X����					(05H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_ALM_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
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
/* �T�v		:	�`�k�l�Q�b�k�q�R�}���h���X�|���X����				(06H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_ALM_CLR( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
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
/* �T�v		:	�b�n�m�m�d�b�s�R�}���h���X�|���X����				(0EH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_CONNECT( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
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
/* �T�v		:	�o�n�r�Q�r�d�s�R�}���h���X�|���X����				(20H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_POS_SET( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_HNDL		*pCmdHdl;
	CMD_LINK_DATA	*rslt_wk;
	LONG			sel_mon1, sel_mon2, sel_mon3;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	rslt_wk		= &( pCmdHdl->cmdInfo.rsltBuf );

	/* �X�e�[�^�X�ݒ� */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( &hNetData->mt_hdl );

	/* POS_SET_MOD, POS_DATA �R�s�[ */
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->dword[4] = rslt_wk->dword[4];

	/* ���j�^�R�[�h1,2,3���o */
	/* SEL_MON1�𒊏o */
	sel_mon1 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16 ) & 0xF;
	/* SEL_MON2�𒊏o */
	sel_mon2 = ( (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16) & 0xF0 ) >> 4;
	/* SEL_MON3�𒊏o */
	sel_mon3 = ( pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 24 ) & 0xF;

	/* ���j�^�f�[�^1,2,3�ݒ� */
	rsp_buf->stdData.mt.mon1 = stdRspGetMonitorData( hNetData, sel_mon1 );
	rsp_buf->stdData.mt.mon2 = stdRspGetMonitorData( hNetData, sel_mon2 );
	rsp_buf->stdData.mt.mon3 = stdRspGetMonitorData( hNetData, sel_mon3 );
}

/****************************************************************************/
/* �T�v		:	SVPRM_RD�R�}���h���X�|���X����						(40H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_SVPRM_RD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk		= &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* �X�e�[�^�X�ݒ� */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( &hNetData->mt_hdl );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
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
/* �T�v		:	SVPRM_WR�R�}���h���X�|���X����						(41H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_SVPRM( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	CMD_LINK_DATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk		= &( hNetData->cmd_hdl.cmdInfo.rsltBuf );

	/* �X�e�[�^�X�ݒ� */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( &hNetData->mt_hdl );

	/* CMD Analyzer�̉�͌��ʃo�b�t�@�̃f�[�^��RSP�i�[�p�o�b�t�@�֓\��t�� */
	rsp_buf->dword[3] = rslt_wk->dword[3];
	rsp_buf->dword[4] = rslt_wk->dword[4];
	rsp_buf->dword[5] = rslt_wk->dword[5];
	rsp_buf->dword[6] = rslt_wk->dword[6];
	rsp_buf->dword[7] = rslt_wk->dword[7];
}


/****************************************************************************/
/* �T�v		:	���j�^�L��R�}���h���X�|���X����					(**H)	*/
/*				BRK_ON/OFF, SENS_ON/OFF, HOLD, LTMOD_ON/OFF, SMON, SV_ON/OFF*/
/*				INTERPORATE, POSING, FEED, LATCH, EX_POSING, ZRET, VELCTRL	*/
/*				TRQCTRL, SVCTRL												*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdRsp_MON_CMD( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	MT_HNDL			*pMtHdl;
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	CMD_LINK_DATA	*rslt_wk;
	UCHAR			sel_mon1, sel_mon2, sel_mon3;	/* ���j�^�I����� */

	/* ���[�J���ϐ��̏����� */
	pMtHdl		= &hNetData->mt_hdl;
	pRspHdl		= &hNetData->rsp_hdl;
	pCmdHdl		= &hNetData->cmd_hdl;
	rslt_wk		= &( pCmdHdl->cmdInfo.rsltBuf );

	/* �X�e�[�^�X�ݒ� */
	rsp_buf->stdData.mt.svCmdStat = stdRspMakeSVCMD_STAT( hNetData, rslt_wk );
	rsp_buf->stdData.mt.svCmdIo   = stdRspMakeSVCMD_IO( pMtHdl );

	/* ���j�^�R�[�h1,2,3���o */
	/* SEL_MON1�𒊏o */
	sel_mon1 = (UCHAR)( (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16) & 0xF );
	/* SEL_MON2�𒊏o */
	sel_mon2 = (UCHAR)( ((pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 16) & 0xF0) >> 4 );
	/* SEL_MON3�𒊏o */
	sel_mon3 = (UCHAR)( (pCmdHdl->cmdInfo.rsltBuf.stdData.mt.svcmd_ctrl >> 24) & 0xF );

	/* �Œ胂�j�^�f�[�^�ݒ� */
	rsp_buf->stdData.mt.cprm_mon1 = 
		stdRspGetMonitorData( hNetData, pRspHdl->cPrmMon[0] );
	rsp_buf->stdData.mt.cprm_mon2 = 
		stdRspGetMonitorData( hNetData, pRspHdl->cPrmMon[1] );
	/* ���j�^�f�[�^1,2,3�ݒ� */
	rsp_buf->stdData.mt.mon1 = stdRspGetMonitorData( hNetData, sel_mon1 );
	rsp_buf->stdData.mt.mon2 = stdRspGetMonitorData( hNetData, sel_mon2 );
	rsp_buf->stdData.mt.mon3 = stdRspGetMonitorData( hNetData, sel_mon3 );
}


/****************************************************************************/
/* �T�v		:	���j�^�f�[�^�擾����										*/
/*				SEL_MON �Őݒ肳�ꂽ�f�[�^���擾����						*/
/****************************************************************************/
/* ����		:	COM_HNDL *pComHdl											*/
/*			:	MT_HNDL *pMtHdl												*/
/*			:	ALM_HNDL *pAlmHdl											*/
/*			:	UCHAR sel_mon		���j�^�I�����							*/
/* �߂�l	:	���j�^�f�[�^												*/
/****************************************************************************/
static	LONG stdRspGetMonitorData( MLNET_HANDLE *hNetData, LONG sel_mon )
{
	MT_HNDL		*pMtHdl;
	LONG		*ltSelR;
	LONG		m3_selmon1;
	LONG		m3_selmon2;
	LONG		ret;

	/* ���[�J���ϐ��̏����� */
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
	{/* ���ʃ��j�^��SV_STAT���́A���낢��ȃ��W���[���̏����Q�Ƃ���̂ł����ō쐬 */
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
/*					 �T�u�R�}���h���X�|���X�쐬����							*/
/****************************************************************************/
/****************************************************************************/
/* �T�v		:	�V�X�e���R�}���h���X�|���X����						(??H)	*/
/****************************************************************************/
/* �Ή��R�}���h : ����`/MEM_WR/SVPRM_WR									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData			�����p�n���h�������\����	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_SYS_CMD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->dword[1];
	rsp_buf->dword[2] = rslt_wk->dword[2];
	rsp_buf->dword[3] = rslt_wk->dword[3];
}


/****************************************************************************/
/* �T�v		:	NOP�R�}���h���X�|���X����							(00H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_NOP( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	rsp_buf->dword[1] = 0;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}

/****************************************************************************/
/* �T�v		:	�`�k�l�QRD�R�}���h���X�|���X����					(05H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_ALM_RD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA	*rslt_wk;

	/* ���[�J���ϐ��̏����� */
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
/* �T�v		:	ALM_CLR�R�}���h���X�|���X����						(06H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_ALM_CLR( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ���[�J���ϐ��̏����� */
	rslt_wk = &( hNetData->cmd_hdl.exCmdInfo.rsltBuf );

	rsp_buf->dword[1] = rslt_wk->stdData.alm.mode;
	rsp_buf->dword[2] = 0;
	rsp_buf->dword[3] = 0;
}


/****************************************************************************/
/* �T�v		:	SMON �R�}���h���X�|���X����									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData			�����p�n���h�������\����	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_MON_CMD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_HNDL	*pCmdHdl;
	LONG		sel_mon4,sel_mon5,sel_mon6;	/* ���j�^�I����� */

	/* ���[�J���ϐ��̏����� */
	pCmdHdl	= &hNetData->cmd_hdl;

	/* ���j�^�R�[�h4,5,6���o */
	/* SEL_MON4�𒊏o */
	sel_mon4 = (pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl1 & 0xF0) >> 4;
	/* SEL_MON5�𒊏o */
	sel_mon5 = pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl2 & 0xF;
	/* SEL_MON6�𒊏o */
	sel_mon6 = (pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.sub_ctrl2 & 0xF0) >> 4;

	/* ���j�^4,5,6�f�[�^�擾 */
	rsp_buf->stdData.mon.mon4 = stdRspGetMonitorData( hNetData, sel_mon4 );
	rsp_buf->stdData.mon.mon5 = stdRspGetMonitorData( hNetData, sel_mon5 );
	rsp_buf->stdData.mon.mon6 = stdRspGetMonitorData( hNetData, sel_mon6 );

}

/****************************************************************************/
/* �T�v		:	ME_RD�R�}���h���X�|���X����							(1DH)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData			�����p�n���h�������\����	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_MEM_RD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ���[�J���ϐ��̏����� */
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
/* �T�v		:	SVPRM_RD�R�}���h���X�|���X����						(40H)	*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData			�����p�n���h�������\����	*/
/*			:	RSP_LINK_EXDATA* 	rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
VOID stdExRsp_SVPRM_RD( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *rsp_buf )
{
	CMD_LINK_EXDATA		*rslt_wk;

	/* ���[�J���ϐ��̏����� */
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



