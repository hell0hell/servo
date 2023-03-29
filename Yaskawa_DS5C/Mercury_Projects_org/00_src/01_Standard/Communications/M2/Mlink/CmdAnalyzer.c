/****************************************************************************/
/*                                                                          */
/*            MECHATROLINK �T�[�{�ʐM�h�e�A�v�� �ʐM�R���|�[�l���g          */
/*                                                                          */
/*            �R�}���h��͏���	CMD Analyzer 								*/
/*																			*/
/****************************************************************************/
/* �h�e��`�֐� :                                                           */
/*																			*/
/* VOID CmdInit()					CMD AnaLyzer�̏�����					*/
/* INT32 CmdUpdateData()			���C���R�}���h��͗p�o�b�t�@�̍X�V		*/
/* INT32 CmdUpdateExData()			�T�u�R�}���h��͗p�o�b�t�@�̍X�V		*/
/* VOID CmdAnalyze()				���C���R�}���h�̉�́E���s				*/
/* VOID CmdAnalyzeEx()				�T�u�R�}���h�̉�́E���s				*/
/*                                                                          */
/****************************************************************************/
/*	�y���ŗ����z															*/
/*		Ver1.00																*/
/*                                                                          */
/*******************  2003 Yaskawa Electric Corporation *********************/

/* includes */
#include "Basedef.h"
#include "KLib.h"
#include "CmdAnalyzer.h"
#include "MtApi.h"
#include "ComManager.h"
#include "ComAlmMngr.h"
#include "CmdTable.h"
#include "SysApi.h"
#include "MLib.h"

/* local forward declarations */
static	INT32 cmdCancelProc( MLNET_HANDLE *hNetData, const CMD_DEF_TBL *pTbl );

/****************************************************************************/
/* �T�v		�FCMD AnaLyzer�̏��������s��									*/
/****************************************************************************/
/* ����		�F	CMD_HNDL* hdl	CMD ANALYZER ��	�n���h��					*/
/*				COM_HNDL* com_hdl	�ʐM�}�l�W���[�̃n���h��				*/
/*				MTHNDL* mt_hdl		���[�V�����}�l�[�W���̃n���h��			*/
/*				REGHNDL* reg_hdl	���W�X�^�}�l�[�W���̃n���h��			*/
/*				ALMHNDL* alm_hdl	�A���[���}�l�[�W���̃n���h��			*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdInit(CMD_HNDL* pCmdHdl)
{
	/* �O���[�o���f�[�^���[���N���A */
	MlibResetByteMemory( pCmdHdl, sizeof(CMD_HNDL) );
	
	/* �R�}���hRdy�̏����l�ݒ� */
	pCmdHdl->cmdInfo.cmdRdy = TRUE;

	/* ��M�R�}���h�̃R�}���h��`�e�[�u���i�̃|�C���^�j��ݒ� */
	CmdSetCMDTablePointer((void**)&pCmdHdl->CmdIdxTbl, 
						  (void**)&pCmdHdl->cmdTblBasePtr, 
						  (void**)&pCmdHdl->exCmdTblBasePtr);
}

/****************************************************************************/
/* �T�v		�F���C���R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v����	*/
/*				�쐬���s���B												*/
/****************************************************************************/
/* ����		:	CMD_HNDL* pCmdHdl		CMD ANALYZER ��	�n���h��			*/
/*				COM_HNDL* pComHdl		�ʐM�}�l�[�W���̃n���h��			*/
/*				BOOL rsp_set			���C���R�}���h���������t���O		*/
/* �߂�l	:	= TRUE  : �R�}���h��͗p�o�b�t�@�X�V						*/
/*				= FALSE : �R�}���h��͗p�o�b�t�@���X�V						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdUpdateData(CMD_HNDL* pCmdHdl, COM_HNDL* pComHdl, BOOL rsp_set)
{
	ULONG* cmd_wk;
	ULONG* ana_wk;
	
	cmd_wk = (ULONG*)pComHdl->CmdBufPtr;
	ana_wk = (ULONG*)(&pCmdHdl->cmdInfo.cmdBuf);

	if( (pCmdHdl->cmdInfo.cmdRdy && rsp_set) || ((UCHAR)(*cmd_wk) == DISCONNECT_CMD) )
	{/* CMD��͗p�o�b�t�@�X�V */
		*(ana_wk+0) = *(cmd_wk+0);
		*(ana_wk+1) = *(cmd_wk+1);
		*(ana_wk+2) = *(cmd_wk+2);
		*(ana_wk+3) = *(cmd_wk+3);
		return TRUE;
	}

	return FALSE;
}

/****************************************************************************/
/* �T�v		�F�T�u�R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v����		*/
/*				�쐬���s���B												*/
/****************************************************************************/
/* ����		:	CMD_HNDL* pCmdHdl		CMD ANALYZER ��	�n���h��			*/
/*				COM_HNDL* pComHdl		�ʐM�}�l�[�W���̃n���h��			*/
/*				BOOL ex_rsp_set			�T�u�R�}���h���������t���O			*/
/* �߂�l	:	= TRUE  : �R�}���h��͗p�o�b�t�@�X�V						*/
/*				= FALSE : �R�}���h��͗p�o�b�t�@���X�V						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdUpdateExData(CMD_HNDL* pCmdHdl, COM_HNDL* pComHdl, BOOL ex_rsp_set)
{
	ULONG* cmd_wk;
	ULONG* ana_wk;
	
	cmd_wk = (ULONG*)(pComHdl->CmdBufPtr + pComHdl->exCmdOfs);
	ana_wk = (ULONG*)(&pCmdHdl->exCmdInfo.cmdBuf);

	if(pCmdHdl->exCmdInfo.cmdRdy && ex_rsp_set)
	{/* SUB CMD��͗p�o�b�t�@�X�V */
		*(ana_wk+0) = *(cmd_wk+0);
		*(ana_wk+1) = *(cmd_wk+1);
		*(ana_wk+2) = *(cmd_wk+2);
		*(ana_wk+3) = *(cmd_wk+3);
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************/
/* �T�v		�F���C���R�}���h�̉�́E���s���s���B							*/
/****************************************************************************/
/* ����		�FMLNET_HANDLE* hNetData	�����p�n���h�������\����			*/
/* �߂�l	�F�Ȃ�															*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdAnalyze(MLNET_HANDLE* hNetData)
{
	INT32 ret;
	LONG  cmd;								/* �R�}���h�R�[�h				*/
	LONG  phs;								/* �ʐM�t�F�[�Y					*/
	CMD_HNDL *pCmdHdl;						/* �R�}���h�n���h���ւ̃|�C���^	*/
	ALM_HNDL *pAlmHdl;						/* �A���[���n���h���ւ̃|�C���^	*/
	CMD_DEF_TBL *cmd_tbl_ptr;				/* �R�}���h��`�e�[�u��			*/

	/* �Ăяo���񐔂��������߁A�A�h���X�ʒu��\�ߌv�Z���Ă��� */
	pCmdHdl = &hNetData->cmd_hdl;
	pAlmHdl = &hNetData->alm_hdl;

	if( pCmdHdl->cmdInfo.enableCmdAna == FALSE )
	{/* ���C���R�}���h���s�����v���Ȃ� */
		return;
	}

	/* �R�}���h�R�[�h�̎擾 */
	cmd = (LONG)pCmdHdl->cmdInfo.cmdBuf.nop.cmd;

	/* ��M�R�}���h�̃R�}���h��`�e�[�u���i�̃|�C���^�j�𒊏o */
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* �A�N�e�B�u���[�h�����v���t���O�̐ݒ� */
//#if ( CCSW_SFTID == SFTID_EXIST )												/*<V730>Add:Start*/
//	SviSetActModOn(GetMtHndl(0)->svCmd, cmd_tbl_ptr->activeModeFlg);
//#endif																			/*<V730>Add:End  */

	/* �t�F�[�Y�`�F�b�N */
	phs = hNetData->com_hdl.Phase;
	if(phs >= cmd_tbl_ptr->availablePhase)
	{/* �t�F�[�Y�`�F�b�N OK */
		if(cmd_tbl_ptr->chkCmdLock & pCmdHdl->exCmdInfo.busy)
		{	/* �R�}���h�����Ă���ꍇ */
			pCmdHdl->cmdInfo.rsltBuf = pCmdHdl->cmdInfo.cmdBuf;
			ret = CMD_CMDLOCK_ERR;
		}
		else
		{	/* �R�}���h��͎��s���� */
			ret = cmdCancelProc(hNetData, cmd_tbl_ptr);
			if(ret == CMD_FUNC_NORMAL_END)
			{
				ret = cmd_tbl_ptr->cmdFuncPtr(pCmdHdl);
			}
		}
	}
	else
	{
		if(phs > 1)
		{	/* ��͌��ʃo�b�t�@��CMD�o�b�t�@�̃f�[�^���R�s�[ */
			pCmdHdl->cmdInfo.rsltBuf = pCmdHdl->cmdInfo.cmdBuf;
			ret = CMD_PHASE_ERR;
		}
		else
		{/* Phase = 1 �̎��́A�������Ȃ�*/
			pCmdHdl->cmdInfo.rsltBuf.nop.cmd = 0x00;
			ret = CMD_FUNC_NORMAL_END;
		}
	}

	pCmdHdl->cmdInfo.crtCmd = (LONG)pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	pCmdHdl->cmdInfo.busy   = cmd_tbl_ptr->chkCmdLock;
	if(ret == CMD_FUNC_NORMAL_END)
	{/* ����I�� */
		pCmdHdl->cmdInfo.cmdRdy  = TRUE;
		pCmdHdl->cmdInfo.enableCmdAna = FALSE;
	}
	else if(ret == CMD_FUNC_CONTINUE)
	{/* �p�����s�� */
		pCmdHdl->cmdInfo.cmdRdy = FALSE;
	}
	else
	{/* �ُ�I�� */
		AlmSetCode( pAlmHdl, hNetData->com_hdl.my_ax_number, ret );
		pCmdHdl->cmdInfo.cmdRdy  = TRUE;
		pCmdHdl->cmdInfo.enableCmdAna = FALSE;
	}
}

/****************************************************************************/
/* �T�v		:	�T�u�R�}���h�̉�́E���s���s���B							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdAnalyzeEx( MLNET_HANDLE *hNetData )
{
	INT32 ret;
	LONG cmd;								/* �R�}���h�R�[�h */
	LONG phs;								/* �ʐM�t�F�[�Y */
	LONG idx;
	CMD_HNDL *pCmdHdl;						/* �R�}���h�n���h���ւ̃|�C���^ */
	ALM_HNDL *pAlmHdl;						/* �R�}���h�n���h���ւ̃|�C���^ */
	EX_CMD_DEF_TBL *ex_cmd_tbl_ptr;			/* �T�u�R�}���h��`�e�[�u�� */

	/* �Ăяo���񐔂��������߁A�A�h���X�ʒu��\�ߌv�Z���Ă��� */
	pCmdHdl = &hNetData->cmd_hdl;
	pAlmHdl = &hNetData->alm_hdl;

	if( pCmdHdl->exCmdInfo.enableCmdAna == FALSE )
	{ /* �T�u�R�}���h���s�����v���Ȃ� */
		return;
	}

	/* �R�}���h�R�[�h�̎擾 */
	cmd = (LONG)pCmdHdl->exCmdInfo.cmdBuf.mon.cmd;

	/* ��M�R�}���h�̃T�u�R�}���h��`�e�[�u���i�̃|�C���^�j�𒊏o */
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* �T�u�R�}���h���g�p�̏ꍇ�͏I�� */
	if( pCmdHdl->useExCmd == FALSE )
	{
		pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = 0x00;
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
		return;
	}
	
	/* �ʐM�t�F�[�Y�擾 */
	phs = hNetData->com_hdl.Phase;
	if(phs >= ex_cmd_tbl_ptr->availablePhase)
	{/* �t�F�[�Y�`�F�b�N OK */
		if(ex_cmd_tbl_ptr->chkCmdLock & pCmdHdl->cmdInfo.busy)
		{/* �R�}���h�����Ă���ꍇ */
			pCmdHdl->exCmdInfo.rsltBuf = pCmdHdl->exCmdInfo.cmdBuf;
			ret = CMD_CMDLOCK_ERR;
		}
		else
		{/* �R�}���h��͎����� */
			ret = ex_cmd_tbl_ptr->exCmdFuncPtr(pCmdHdl);
		}
	}
	else
	{
		if(phs > 1)
		{/* ��͌��ʃo�b�t�@��CMD�o�b�t�@�̃f�[�^���R�s�[ */
			pCmdHdl->exCmdInfo.rsltBuf = pCmdHdl->exCmdInfo.cmdBuf;
			ret = CMD_PHASE_ERR;
		}
		else
		{/* Phase = 1 �̎��́A�������Ȃ�*/
			pCmdHdl->exCmdInfo.rsltBuf.mon.cmd = 0x00;
			ret = CMD_FUNC_NORMAL_END;
		}
	}

	if(ret == CMD_FUNC_NORMAL_END)
	{/* ����I�� */
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
	}
	else if(ret == CMD_FUNC_CONTINUE)
	{/* �p�����s�� */
		pCmdHdl->exCmdInfo.cmdRdy = FALSE;
		pCmdHdl->exCmdInfo.busy   = TRUE;/* ���C��-�T�u���`�F�b�N�ݒ� */ //ex_cmd_tbl_ptr->chkCmdLock;
	}
	else
	{/* �ُ�I�� */
		/* ret = �R�}���h�R�[�h(�R�o�C�g) + �A���[���h�c�i�P�C�Q�o�C�g�j�Ƃ��� */
		AlmSetCode( pAlmHdl, hNetData->com_hdl.my_ax_number,ret );
		pCmdHdl->exCmdInfo.warng = TRUE;	/* �T�u�R�}���h��͂łُ̈�̓��[�j���O�̂� */
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
	}
}


/****************************************************************************/
/* �T�v		:	�^�C���A�E�g�������s���B									*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/*				INT32 ret					�R�}���h��͎��s�����̖߂�l	*/
/* �߂�l	:	����I��		:	CMD_FUNC_NORMAL_END						*/
/* 				�^�C���A�E�g	:	CMD_TIME_OUT_ERR						*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
static	INT32 cmdCancelProc( MLNET_HANDLE *hNetData, const CMD_DEF_TBL *pTbl )
{
	CMD_HNDL	*pCmdHdl;
	MT_HNDL		*pMtHdl;
	UCHAR		cmd;							/* �R�}���h�R�[�h *//* <S0EC> */
	INT32		rc;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	rc			= CMD_FUNC_NORMAL_END;

	/* �R�}���h�R�[�h�̎擾 */
	cmd = pCmdHdl->cmdInfo.cmdBuf.nop.cmd;
	if( pCmdHdl->cmdInfo.rsltBuf.nop.cmd != cmd )
	{/* �R�}���h�؂�ւ�聨���b�`�v���t���O�̃N���A */
		MciCancelLatch(pMtHdl, MOTION_LTMD);
	}

/* �^�C���A�E�g�`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy) || (pTbl->timeOut == 0) )
	{/* CmdRdy=1 or �^�C���A�E�g�`�F�b�N�Ȃ� */
		KlibRstLongTimer( &pCmdHdl->cmdInfo.busyCnt );
		return rc;
	}
	if( KlibGetLongTimerMs(pCmdHdl->cmdInfo.busyCnt) > pTbl->timeOut ) /* unit:[ms] */
	{ /* !�R�}���h�^�C���A�E�g! */
		if( cmd == 0x31 )
		{ /* SV_ON */
			MciSetServoOn( pMtHdl, FALSE, TRUE );
			rc = CMD_TIME_OUT_ERR;
		}
		else if( cmd == 0x23 )
		{ /* SENS_ON */
			MciSetSensOn( pMtHdl, FALSE );
			rc = CMD_TIME_OUT_ERR;
		}
		else
		{ /* ���̑��̃R�}���h�̂Ƃ��͉������Ȃ� */
			KlibRstLongTimer( &pCmdHdl->cmdInfo.busyCnt );
			rc = CMD_FUNC_NORMAL_END;
		}
		AlmSetCode( &hNetData->alm_hdl, hNetData->com_hdl.my_ax_number,  CMD_TIME_OUT_ERR );
	}
	return rc;
}

/****************************************************************************/
/* �T�v		�F	�R�l�N�V�����ؒf����										*/
/*				�t�F�[�Y�_�E�������A���[�V�����������������s����			*/
/****************************************************************************/
/* ����		�F	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	�F	CMD_FUNC_NORMAL_END	�F	����I��							*/
/* 			�F	CMD_FUNC_CONTINUE	�F	�������s��							*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdRunDisConnect( MLNET_HANDLE *hNetData )
{
	CMD_HNDL	*pCmdHdl;
	COM_HNDL	*pComHdl;
	INT32		ret, result;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pComHdl		= &hNetData->com_hdl;
	result		= CMD_FUNC_CONTINUE;

	/* �ʐM�t�F�[�Y�`�F�b�N */
	switch( pCmdHdl->cmd_seq )
	{
	case REQ_PHSDOWN_SEQ:		/*---- �t�F�[�Y�_�E���v�� ----*/
		pCmdHdl->useExCmd = FALSE;	/* �T�u�R�}���h�s��Ԃ� */
		ComReqPhaseDown( pComHdl, 1 );
		pCmdHdl->cmd_seq = SV_OFF_SEQ;
//	/*	break;  �������ց���*/

	case SV_OFF_SEQ:			/*---- �T�[�{�I�t�w�� --------*/
		if( MciSetServoOn( &hNetData->mt_hdl, FALSE, TRUE ) == MCI_SUCCESS )
		{
			hNetData->mt_hdl.svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
			pCmdHdl->cmd_seq = MT_DISABLE_SEQ;
		}
		break;

	case MT_DISABLE_SEQ:		/*---- ���[�V�������� --------*/
		ret = MciResetMotion( &hNetData->mt_hdl );
		if( ret != MCI_WAIT_COMP )
		{
			pCmdHdl->cmd_seq = (ret == MCI_SUCCESS) ? 
								PHASE_CHK_SEQ : SV_OFF_SEQ;
		}
		
		pCmdHdl->cmdInfo.chkOpIoCmd = 0x0000;   /* OPTION/IO_MON�t�B�[���h�g�p�\�R�}���h�g������ *//*<V624>*/
		pCmdHdl->cmdInfo.cmdCdataBuf = 0x0000;	/* ADJ(SUBCODE=10H)��CDATA�̃o�b�t�@�������� *//*<V624>*/

		break;

	case PHASE_CHK_SEQ:			/*---- �t�F�[�Y�_�E���`�F�b�N ----*/
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret != COM_FUNC_CONTINUE )
		{
			pCmdHdl->cmd_seq = SOFT_RESET_SEQ;
		}
		break;

	case SOFT_RESET_SEQ:		/*---- �V�X�e�����Z�b�g�������s ----*/
		ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_SYSRST, NULL);
		if( ret == REG_CONTINUE ) 
		{ /* �\�t�g���Z�b�g�����I�� */
			pCmdHdl->cmd_seq = WAIT_SOFT_RESET_SEQ;
		}
		break;

	case WAIT_SOFT_RESET_SEQ:	/*---- �V�X�e�����Z�b�g�҂� --------*/
		if(0 >= RegCheckCommandState(&hNetData->reg_hdl))
		{
			pCmdHdl->cmd_seq = IDOL_SEQ;
			result = CMD_FUNC_NORMAL_END;
		}
		break;

	default:
		result = CMD_FUNC_NORMAL_END;
		break;
	}

	return result;
}

