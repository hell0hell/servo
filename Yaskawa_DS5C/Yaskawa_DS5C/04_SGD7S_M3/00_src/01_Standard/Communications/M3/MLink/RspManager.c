/****************************************************************************
Description	: MECHATROLINK Response Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "RspManager.h"
#include "CmdAnalyzer.h"
#include "M3StdRspProc.h"
#include "MtApi.h"
#include "CmdTable.h"
#include "MLib.h"





/****************************************************************************/
/* �T�v		:	RSP Manager�̏��������s��									*/
/****************************************************************************/
/* ����		:	RSP_HNDL* hdl												*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspInit( RSP_HNDL *hdl )
{
	/* �O���[�o���f�[�^���[���N���A */
	MlibResetByteMemory( hdl,  sizeof(RSP_HNDL) );
	hdl->enableLtMon[0] = FALSE;	/* ���b�`���j�^�X�VOFF */
	hdl->enableLtMon[1] = FALSE;	/* ���b�`���j�^�X�VOFF */
}




/****************************************************************************/
/* �T�v		:	���C���R�}���h�ɑ΂���RSP�f�[�^�̍쐬���s��					*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			:	RSP_LINK_DATA* 	rspbuf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeData( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf )
{
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	MT_HNDL			*pMtHdl;
	ALM_HNDL		*pAlmHdl;
	UCHAR			cmd;
	CMD_DEF_TBL		*cmd_tbl_ptr;

	/* ���[�J���ϐ��̏����� */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pMtHdl	= &hNetData->mt_hdl;
	pAlmHdl	= &hNetData->alm_hdl;

	/* ���X�|���X�p���C���R�}���h�ۑ��o�b�t�@�̃R�}���h�R�[�h����A
							��M�R�}���h��CMD��`�e�[�u��(�̃|�C���^)�𒊏o */
	cmd = pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd;
	rsp_buf->stdData.mt.cmd = cmd;
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* ���b�`��ԃ`�F�b�N */
	pRspHdl->last_L_CMP[0] = MciGetLatchStatus( pMtHdl, &pRspHdl->enableLtMon[0], 0 );
	pRspHdl->last_L_CMP[1] = MciGetLatchStatus( pMtHdl, &pRspHdl->enableLtMon[1], 1 );

	/* CMD��`�e�[�u���ɓo�^����Ă���R�}���h����RSP�쐬�����֐������s */
	cmd_tbl_ptr->rspFuncPtr( hNetData, rsp_buf );

	/* �X�e�[�^�X�쐬�E�\��t�� */
	if( pCmdHdl->cmdInfo.profileType == M3STD_PROFILE )
	{
		rsp_buf->stdData.mt.cmdStat = stdRspMakeCMD_STAT( pCmdHdl, pAlmHdl, 
										pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd_ctrl );
	}
#ifdef ML2_PROFILE_SUPP
	else if( prof == M2_PROFILE )
	{
		/* �A���[���R�[�h�\��t���i���C���R�}���h�̂݁j */
		rsp_buf->m2Data.mt.alarm = ( (USHORT)pAlmHdl->errCode >> 4 );	/* ���ʂP���؂�̂� */
		/* �X�e�[�^�X�쐬�E�\��t�� */
		rsp_buf->m2Data.mt.status = m2RspMakeStatus( hNetData );
	}
#endif

	pRspHdl->rspSetDone = pCmdHdl->cmdInfo.cmdRdy;

}


/****************************************************************************/
/* �T�v		:	�T�u�R�}���h�ɑ΂���RSP�f�[�^�̍쐬���s��					*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData			�����p�n���h�������\����	*/
/*			:	RSP_LINK_EXDATA* ex_rsp_buf		RSP�i�[�p�o�b�t�@�|�C���^	*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *ex_rsp_buf )
{
	RSP_HNDL		*pRspHdl;
	CMD_HNDL		*pCmdHdl;
	ALM_HNDL		*pAlmHdl;
	UCHAR			cmd;
	LONG			idx;
	EX_CMD_DEF_TBL	*ex_cmd_tbl_ptr;

	/* ���[�J���ϐ��̏����� */
	pRspHdl	= &hNetData->rsp_hdl;
	pCmdHdl	= &hNetData->cmd_hdl;
	pAlmHdl	= &hNetData->alm_hdl;

	/* ���X�|���X�p���C���R�}���h�ۑ��o�b�t�@�̃R�}���h�R�[�h����A
						��M�R�}���h��CMD��`�e�[�u���i�̃|�C���^�j�𒊏o */
	cmd = pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd;
//	ex_rsp_buf->stdData.mon.cmd = cmd;
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* CMD��`�e�[�u���ɓo�^����Ă���R�}���h����RSP�쐬�����֐������s */
	ex_cmd_tbl_ptr->exRspFuncPtr( hNetData, ex_rsp_buf );

	/* �X�e�[�^�X�쐬�E�\��t�� */
	if( pCmdHdl->cmdInfo.profileType == M3STD_PROFILE )
	{
		ex_rsp_buf->dword[0] = stdRspMakeSUB_STAT(pCmdHdl, pAlmHdl, 
												  pCmdHdl->exCmdInfo.rsltBuf.dword[0]);
	}
#ifdef ML2_PROFILE_SUPP
	else if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
	{
		ex_rsp_buf->m2Data.mon.cmd = cmd;
		ex_rsp_buf->m2Data.mon.status = m2RspMakeExStatus( pCmdHdl, pAlmHdl );
	}
#endif	

	/* respSetDone = CmdRdy */
	pRspHdl->exRspSetDone = pCmdHdl->exCmdInfo.cmdRdy;
}

/****************************************************************************/
/* �T�v		�FCPRM_SEL_MON �擾����											*/
/****************************************************************************/
/* ����		�F	RSP_HNDL *hdl												*/
/*			�F	ULONG 	 data		CPRM_SEL_MON�̒l						*/
/*			�F	UCHAR 	 num		���j�^�I��ԍ� [0or1]					*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
VOID RspSetcPrmMon(RSP_HNDL *hdl, ULONG data, UCHAR num)
{
	hdl->cPrmMon[num] = data;
}



