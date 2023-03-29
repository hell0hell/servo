/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g          */
/*                                                                          */
/*            ���X�|���X�쐬����	RSP Manager 							*/
/*																			*/
/****************************************************************************/
/* �h�e��`�֐� :                                                           */
/*																			*/
/* VOID RspInit()				RSP Manager�̏�����							*/
/* VOID RspMakeData()			���C���R�}���h�ɑ΂���RSP�f�[�^�̍쐬		*/
/* VOID RspMakeExData()			�T�u�R�}���h�ɑ΂���RSP�f�[�^�̍쐬			*/
/*                                                                          */
/****************************************************************************/
/*	�y���ŗ����z															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/

/* includes */
#include "Basedef.h"
#include "RspManager.h"
#include "CmdAnalyzer.h"
#include "RspProc.h"
#include "MtApi.h"
#include "CmdTable.h"
#include "MLib.h"


/****************************************************************************/
/* �T�v		�FRSP Manager�̏��������s��										*/
/****************************************************************************/
/* ����		�FRSP_HNDL *hdl													*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspInit(RSP_HNDL *rspHdl)
{
	/* �O���[�o���f�[�^���[���N���A */
	MlibResetByteMemory( rspHdl,  sizeof(RSP_HNDL) );
	
	/* ���b�`���j�^�X�VOFF 			*/
	rspHdl->enableLtMon = FALSE;
}

/****************************************************************************/
/* �T�v		�F���C���R�}���h�ɑ΂���RSP�f�[�^�̍쐬���s��					*/
/****************************************************************************/
/* ����		�F	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*			�FRSP_LINK_DATA* 	rspbuf		RSP�i�[�p�o�b�t�@�|�C���^		*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeData(MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf)
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
	cmd = pCmdHdl->cmdInfo.rsltBuf.nop.cmd;
	rsp_buf->mt.cmd = cmd;
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* ���b�`��ԃ`�F�b�N */
	pRspHdl->last_L_CMP = MciGetLatchStatus(pMtHdl, &pRspHdl->enableLtMon);

	/* CMD��`�e�[�u���ɓo�^����Ă���R�}���h����RSP�쐬�����֐������s */
	cmd_tbl_ptr->rspFuncPtr(hNetData, rsp_buf);

	/* �A���[���R�[�h�\��t���i���C���R�}���h�̂݁j */
	rsp_buf->mt.alarm = ( (USHORT)pAlmHdl->errCode >> 4 );	/* ���ʂP���؂�̂� */

	/* �X�e�[�^�X�쐬�E�\��t�� */
	rsp_buf->mt.status = rspMakeStatus(pRspHdl, pCmdHdl, pMtHdl, pAlmHdl);

	/* respSetDone = CmdRdy */
	pRspHdl->rspSetDone = pCmdHdl->cmdInfo.cmdRdy;
}

/****************************************************************************/
/* �T�v		�F�T�u�R�}���h�ɑ΂���RSP�f�[�^�̍쐬���s��						*/
/****************************************************************************/
/* ����		�F	MLNET_HANDLE* hNetData			�����p�n���h�������\����	*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* ex_rsp_buf)
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

	/* ���X�|���X�p���C���R�}���h�ۑ��o�b�t�@�̃R�}���h�R�[�h����A��M�R�}
	   ���h��CMD��`�e�[�u���i�̃|�C���^�j�𒊏o */
	cmd = pCmdHdl->exCmdInfo.rsltBuf.mon.cmd;
	ex_rsp_buf->mon.cmd = cmd;
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* CMD��`�e�[�u���ɓo�^����Ă���R�}���h����RSP�쐬�����֐������s */
	ex_cmd_tbl_ptr->exRspFuncPtr(hNetData,ex_rsp_buf);

	/* �X�e�[�^�X�쐬�E�\��t�� */
	ex_rsp_buf->mon.status = rspMakeExStatus(pCmdHdl, pAlmHdl);

	/* respSetDone = CmdRdy */
	pRspHdl->exRspSetDone =  pCmdHdl->exCmdInfo.cmdRdy;

}

