/****************************************************************************
Description	: MECHATROLINK Command Analyzer
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
Y.Oka  2010.04.xx    created
I.M.   2010.10.xx    renewal

****************************************************************************/
#include "Basedef.h"
#include "KLib.h"
#include "CmdAnalyzer.h"
#include "MtApi.h"
#include "ComManager.h"
#include "ComAlmMngr.h"
#include "CmdTable.h"
#include "SysApi.h"
#include "MLib.h"


/* �^�C���A�E�g���� */
static	INT32 cmdCancelProc( MLNET_HANDLE *hNetData, const CMD_DEF_TBL *pTbl );



/****************************************************************************/
/* �T�v		:	CMD AnaLyzer�̏��������s��									*/
/****************************************************************************/
/* ����		:	CMD_HNDL* hdl	CMD ANALYZER ��	�n���h��					*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdInit( CMD_HNDL *pCmdHdl )
{
	/* �O���[�o���f�[�^���[���N���A */
	MlibResetByteMemory( pCmdHdl, sizeof(CMD_HNDL) );

	pCmdHdl->cmdInfo.profileType = M3STD_PROFILE;
	pCmdHdl->cmdInfo.cmdRdy = TRUE;

	/* ��M�R�}���h�̃R�}���h��`�e�[�u���i�̃|�C���^�j��ݒ� */
	CmdSetCMDTablePointer(M3STD_PROFILE, 
						  (void**)&pCmdHdl->CmdIdxTbl, 
						  (void**)&pCmdHdl->cmdTblBasePtr, 
						  (void**)&pCmdHdl->exCmdTblBasePtr);
}

/****************************************************************************/
/* �T�v		:	���C���R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v����	*/
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
INT32 CmdUpdateData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL rsp_set )
{
	ULONG *cmd_wk;
	ULONG *ana_wk;

	cmd_wk = (ULONG*)pComHdl->CmdBufPtr;
	ana_wk = (ULONG*)( &pCmdHdl->cmdInfo.cmdBuf );

	if( (pCmdHdl->cmdInfo.cmdRdy && rsp_set) || ((UCHAR)(*cmd_wk) == DISCONNECT_CMD) )
	{ /* CMD��͗p�o�b�t�@�X�V */
		*( ana_wk + 0 ) = *( cmd_wk + 0 );
		*( ana_wk + 1 ) = *( cmd_wk + 1 );
		*( ana_wk + 2 ) = *( cmd_wk + 2 );
		*( ana_wk + 3 ) = *( cmd_wk + 3 );
		*( ana_wk + 4 ) = *( cmd_wk + 4 );
		*( ana_wk + 5 ) = *( cmd_wk + 5 );
		*( ana_wk + 6 ) = *( cmd_wk + 6 );
		*( ana_wk + 7 ) = *( cmd_wk + 7 );
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************/
/* �T�v		:	�T�u�R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v����	*/
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
INT32 CmdUpdateExData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL ex_rsp_set )
{
	ULONG *cmd_wk;
	ULONG *ana_wk;

	cmd_wk = (ULONG*)( pComHdl->CmdBufPtr + pComHdl->exCmdOfs );
	ana_wk = (ULONG*)( &pCmdHdl->exCmdInfo.cmdBuf );

	if( pCmdHdl->exCmdInfo.cmdRdy && ex_rsp_set )
	{ /* SUB CMD��͗p�o�b�t�@�X�V */
		*( ana_wk + 0 ) = *( cmd_wk + 0 );
		*( ana_wk + 1 ) = *( cmd_wk + 1 );
		*( ana_wk + 2 ) = *( cmd_wk + 2 );
		*( ana_wk + 3 ) = *( cmd_wk + 3 );
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************/
/* �T�v		:	���C���R�}���h�̉�́E���s���s���B							*/
/****************************************************************************/
/* ����		:	MLNET_HANDLE* hNetData		�����p�n���h�������\����		*/
/* �߂�l	:	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
VOID CmdAnalyze( MLNET_HANDLE *hNetData )
{
	INT32 ret;
	LONG cmd;							/* �R�}���h�R�[�h */
	LONG phs;							/* �ʐM�t�F�[�Y */
	CMD_HNDL *pCmdHdl;					/* �R�}���h�n���h���ւ̃|�C���^ */
	ALM_HNDL *pAlmHdl;					/* �R�}���h�n���h���ւ̃|�C���^ */
	CMD_DEF_TBL *cmd_tbl_ptr;			/* �R�}���h��`�e�[�u�� */

	/* �Ăяo���񐔂��������߁A�A�h���X�ʒu��\�ߌv�Z���Ă��� */
	pCmdHdl = &hNetData->cmd_hdl;
	pAlmHdl = &hNetData->alm_hdl;

	if( pCmdHdl->cmdInfo.enableCmdAna == FALSE )
	{/* ���C���R�}���h���s�����v���Ȃ� */
		return;
	}

	/* �R�}���h�R�[�h�̎擾 */
	cmd = (LONG)pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;

	/* ��M�R�}���h�̃R�}���h��`�e�[�u���i�̃|�C���^�j�𒊏o */
	cmd_tbl_ptr = &(pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]]);

	/* �R�}���h��͎��s���ʃN���A */
	pCmdHdl->cmdInfo.ResultCmdAna = ~CMD_FUNC_NORMAL_END;

	/* �t�F�[�Y�`�F�b�N */
	phs = hNetData->com_hdl.Phase;
	if( phs >= cmd_tbl_ptr->availablePhase )
	{ /* �t�F�[�Y�`�F�b�N OK */
		if( cmd_tbl_ptr->chkCmdLock & pCmdHdl->exCmdInfo.busy )
		{ /* �R�}���h�����Ă���ꍇ */
			pCmdHdl->cmdInfo.rsltBuf = pCmdHdl->cmdInfo.cmdBuf;
			ret = CMD_CMDLOCK_ERR;
		}
		else
		{
			/* �R�}���h��͎��s���� */
			ret = cmdCancelProc( hNetData, cmd_tbl_ptr );
			if( ret == CMD_FUNC_NORMAL_END )
			{
				ret = cmd_tbl_ptr->cmdFuncPtr( hNetData );
			}
		}
	}
	else
	{
		if( phs > 1 )
		{ /* ��͌��ʃo�b�t�@��CMD�o�b�t�@�̃f�[�^���R�s�[ */
			pCmdHdl->cmdInfo.rsltBuf = pCmdHdl->cmdInfo.cmdBuf;
			if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
			{ /* M2�݊��v���t�@�C���̂Ƃ��́AA.95A��Ԃ� */
				ret = CMD_PHASE_ERR_M2;
			}
			else
			{ /* ��L�ȊO��A.97A��Ԃ� */
				ret = CMD_PHASE_ERR_M3;
			}
		}
		else
		{ /* Phase = 1 �̎��́A�������Ȃ� */
			pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd = 0x00;
			ret = CMD_FUNC_NORMAL_END;
		}
	}

	pCmdHdl->cmdInfo.crtCmd = (LONG)pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	pCmdHdl->cmdInfo.busy   = cmd_tbl_ptr->chkCmdLock;
	if( ret == CMD_FUNC_NORMAL_END )
	{ /* ����I�� */
		pCmdHdl->cmdInfo.cmdRdy = TRUE;
		pCmdHdl->cmdInfo.enableCmdAna = FALSE;
	}
	else if( ret == CMD_FUNC_CONTINUE )
	{ /* �p�����s�� */
		pCmdHdl->cmdInfo.cmdRdy = FALSE;
	}
	else
	{ /* �ُ�I�� */
//		AlmSetCode( pAlmHdl, hNetData->com_hdl.my_ax_number, ret );
		AlmSetCmdAlmCode( pAlmHdl, ret, FALSE );
		pCmdHdl->cmdInfo.CmdAlm = TRUE;
		pCmdHdl->cmdInfo.warng  = TRUE;					/* ���C���R�}���h��͂łُ̈���L�� */
		pCmdHdl->cmdInfo.cmdRdy = TRUE;
		pCmdHdl->cmdInfo.enableCmdAna = FALSE;
	}
	/* �R�}���h��͐���I���`�F�b�N */
	if( pCmdHdl->cmdInfo.ResultCmdAna == CMD_FUNC_NORMAL_END )
	{
		pCmdHdl->cmdInfo.CmdAlm = FALSE;
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
	cmd = (LONG)pCmdHdl->exCmdInfo.cmdBuf.stdData.mon.cmd;

	/* ��M�R�}���h�̃T�u�R�}���h��`�e�[�u���i�̃|�C���^�j�𒊏o */
	idx = pCmdHdl->cmdTblBasePtr[pCmdHdl->CmdIdxTbl[cmd]].exCmdTblIdx;
	ex_cmd_tbl_ptr = &(pCmdHdl->exCmdTblBasePtr[idx]);

	/* �T�u�R�}���h���g�p�̏ꍇ�͏I�� */
	if( pCmdHdl->useExCmd == FALSE )
	{
		pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = 0x00;
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
		return;
	}

	/* �R�}���h��͎��s���ʃN���A */
	pCmdHdl->exCmdInfo.ResultCmdAna = ~CMD_FUNC_NORMAL_END;

	/* �ʐM�t�F�[�Y�擾 */
	phs = hNetData->com_hdl.Phase;
	if( phs >= ex_cmd_tbl_ptr->availablePhase )
	{ /* �t�F�[�Y�`�F�b�N OK */
		if( ex_cmd_tbl_ptr->chkCmdLock & pCmdHdl->cmdInfo.busy )
		{ /* �R�}���h�����Ă���ꍇ */
			pCmdHdl->exCmdInfo.rsltBuf = pCmdHdl->exCmdInfo.cmdBuf;
			ret = CMD_CMDLOCK_ERR;
		}
		else
		{
			/* �T�u�R�}���h��͎����� */
			ret = ex_cmd_tbl_ptr->exCmdFuncPtr( hNetData );
		}
	}
	else
	{
		if( phs > 1 )
		{ /* ��͌��ʃo�b�t�@��CMD�o�b�t�@�̃f�[�^���R�s�[ */
			pCmdHdl->exCmdInfo.rsltBuf = pCmdHdl->exCmdInfo.cmdBuf;
			if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
			{ /* M2�݊��v���t�@�C���̂Ƃ��́AA.95A��Ԃ� */
				ret = CMD_PHASE_ERR_M2;
			}
			else
			{ /* ��L�ȊO��A.97A��Ԃ� */
				ret = CMD_PHASE_ERR_M3;
			}
		}
		else
		{ /* Phase = 1 �̎��́A�������Ȃ�*/
			pCmdHdl->exCmdInfo.rsltBuf.stdData.mon.cmd = 0x00;
			ret = CMD_FUNC_NORMAL_END;
		}
	}

	if( ret == CMD_FUNC_NORMAL_END )
	{ /* ����I�� */
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
	}
	else if( ret == CMD_FUNC_CONTINUE )
	{ /* �p�����s�� */
		pCmdHdl->exCmdInfo.cmdRdy = FALSE;
		pCmdHdl->exCmdInfo.busy   = TRUE;
	}
	else
	{ /* �ُ�I�� */
		/* ret = �R�}���h�R�[�h(�R�o�C�g) + �A���[���h�c�i�P�C�Q�o�C�g�j�Ƃ��� */
//		AlmSetCode( pAlmHdl, hNetData->com_hdl.my_ax_number, ret );
		AlmSetCmdAlmCode( pAlmHdl, ret, TRUE );
		pCmdHdl->exCmdInfo.CmdAlm = TRUE;
		pCmdHdl->exCmdInfo.warng  = TRUE;	/* �T�u�R�}���h��͂łُ̈�̓��[�j���O�̂� */
		pCmdHdl->exCmdInfo.cmdRdy = TRUE;
		pCmdHdl->exCmdInfo.busy   = FALSE;
		pCmdHdl->exCmdInfo.enableCmdAna = FALSE;
	}
	/* �R�}���h��͐���I���`�F�b�N */
	if( pCmdHdl->exCmdInfo.ResultCmdAna == CMD_FUNC_NORMAL_END )
	{
		pCmdHdl->exCmdInfo.CmdAlm = FALSE;
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
	UCHAR		cmd;							/* �R�}���h�R�[�h */ /* <S0EC> */
	INT32		rc;

	/* ���[�J���ϐ��̏����� */
	pCmdHdl		= &hNetData->cmd_hdl;
	pMtHdl		= &hNetData->mt_hdl;
	rc			= CMD_FUNC_NORMAL_END;

	/* �R�}���h�R�[�h�̎擾 */
	cmd = pCmdHdl->cmdInfo.cmdBuf.stdData.nop.cmd;
	if( pCmdHdl->cmdInfo.rsltBuf.stdData.nop.cmd != cmd )
	{
		/* �R�}���h�؂�ւ�聨���b�`�v���t���O�̃N���A[M2�݊��v���t�@�C��] */
		if( pCmdHdl->cmdInfo.profileType == M2_PROFILE )
		{
			MciCancelLatch( pMtHdl, MOTION_LTMD, 0 );
		}
		
		/* ���b�`�@�\�t���R�}���h����̐؂�ւ�聨���b�`�v���t���O�̃N���A[M3�W���v���t�@�C��] */
		if( (pCmdHdl->cmdInfo.profileType == M3STD_PROFILE) &&
			(pMtHdl->latch[0].Mode) > MODAL_LTMD )
		{
			MciCancelLatch( pMtHdl, MOTION_LTMD, 0 );
			pCmdHdl->cmdInfo.reLatchReq = TRUE;
		}
	}

	/* �^�C���A�E�g�`�F�b�N */
	if( (pCmdHdl->cmdInfo.cmdRdy) || (pTbl->timeOut == 0) )
	{ /* CmdRdy=1 or �^�C���A�E�g�`�F�b�N�Ȃ� */
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
		AlmSetCode( &hNetData->alm_hdl, hNetData->com_hdl.my_ax_number, CMD_TIME_OUT_ERR );
	}
	return rc;
}



/****************************************************************************/
/* �T�v		:		�v���t�@�C���^�C�v���`�F�b�N���A						*/
/*					WDT,�T�u�R�}���h�̃I�t�Z�b�g�ʒu�A�R�}���h��`�e�[�u����*/
/*					�x�[�X�|�C���^�̐ݒ���s���B							*/
/****************************************************************************/
/* ����		:	CMD_HNDL* pCmdHdl		CMD ANALYZER �̃n���h��				*/
/*				COM_HNDL* pComHdl		�ʐM�}�l�[�W���̃n���h��			*/
/*				LONG tdata_size			�`���o�C�g��						*/
/* �߂�l	:	CMD_FUNC_NORMAL_END	�F	����I��							*/
/* 			:	CMD_PROFILE_DETECT	�F	����I���i�v���t�@�C���^�C�v����)	*/
/* 			:	CMD_PROFILE_UNKNOWN	�F	�v���t�@�C���^�C�v Unknown			*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
INT32 CmdSetProfileType( MLNET_HANDLE *hNetData, LONG tdata_size )
{
	CMD_HNDL *pCmdHdl;
	COM_HNDL *pComHdl;
	INT32 ret;
	UCHAR profile;					/* �v���t�@�C���^�C�v */

	/* ���[�J���ϐ��̏����l�ݒ� */
	pCmdHdl	= &( hNetData->cmd_hdl );
	pComHdl	= &( hNetData->com_hdl );
//	ret		= CMD_FUNC_NORMAL_END;

	/*--------------------------------------------------------------------*/
	/* Phase=1��CONNECT�R�}���h����M�����ꍇ�Ƀx�[�X�|�C���^�̐ݒ�����s */
	/*--------------------------------------------------------------------*/
//	if( (pCmdHdl->cmdInfo.cmdBuf.stdData.connect.cmd == 0x0E) )
	{
		/* �v���t�@�C���`�F�b�N */
		profile = pCmdHdl->cmdInfo.cmdBuf.stdData.connect.profile_type;
		if( profile == M3STD_PROFILE )
		{
			/* �v���t�@�C���^�C�v�ݒ� */
			pCmdHdl->cmdInfo.profileType = profile;
			/* WDT�I�t�Z�b�g�ʒu�ݒ� */
			ComSetWdtOfs( pComHdl, M3STD_WDT_OFS, M3STD_WDT_OFS );
			/* �T�u�R�}���h�I�t�Z�b�g�ʒu�ݒ� */
			ComSetExCmdOfs( pComHdl, M3STD_EX_CMD_OFS, M3STD_EX_CMD_OFS );

			MtcSetMtInfoByProfileType( &hNetData->mt_hdl, pCmdHdl->cmdInfo.profileType );
			ret = CMD_PROFILE_DETECT;
		}
		else if( profile == M2_PROFILE )
		{
			/* �v���t�@�C���^�C�v�ݒ� */
			pCmdHdl->cmdInfo.profileType = profile;
			/* WDT�I�t�Z�b�g�ʒu�ݒ� */
			ComSetWdtOfs( pComHdl, M2_WDT_OFS, M2_WDT_OFS );
			/* �T�u�R�}���h�I�t�Z�b�g�ʒu�ݒ� */
			ComSetExCmdOfs( pComHdl, M2_EX_CMD_OFS, M2_EX_CMD_OFS );

			MtcSetMtInfoByProfileType( &hNetData->mt_hdl, pCmdHdl->cmdInfo.profileType );
			ret = CMD_PROFILE_DETECT;
		}
		else
		{ /* ���m�̃v���t�@�C���ɑ΂��Ă͌��݂̐ݒ�ێ� */
			/* ����������M3�W���v���t�@�C���̏����Z�b�g���Ă��� */
			ret = CMD_PROFILE_UNKNOWN;
		}

		/* ��M�R�}���h�̃R�}���h��`�e�[�u���i�̃|�C���^�j��ݒ� */
		CmdSetCMDTablePointer(profile, 
							  (void**)&pCmdHdl->CmdIdxTbl, 
							  (void**)&pCmdHdl->cmdTblBasePtr, 
							  (void**)&pCmdHdl->exCmdTblBasePtr);

		/* CMD_ALM�����N���A���[�h�ݒ� */
		AlmSetCmdAlmClearMode( &(hNetData->alm_hdl), 
				pCmdHdl->cmdInfo.profileType, (hNetData->reg_hdl.Prm)->comCtrl);
	}
	return ret;
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
	case REQ_PHSDOWN_SEQ:		/* �t�F�[�Y�_�E���v�� */
		pCmdHdl->useExCmd = FALSE;	/* �T�u�R�}���h�s��Ԃ� */
		ComReqPhaseDown( pComHdl, 1 );
		pCmdHdl->cmd_seq = SV_OFF_SEQ;
//	/*	break;  �������ց���*/

	case SV_OFF_SEQ:			/* �T�[�{�I�t�w�� */
		if( MciSetServoOn( &hNetData->mt_hdl, FALSE, TRUE ) == MCI_SUCCESS )
		{
			hNetData->mt_hdl.svCmd->CmdSeqBit &= ~(BRK_RELEASE_BIT);
			pCmdHdl->cmd_seq = MT_DISABLE_SEQ;
		}
		break;

	case MT_DISABLE_SEQ:		/* ���[�V�������� */
		pCmdHdl->cmdInfo.lstSvCmdCtrl = 0;	/* LT_REQ�̑O��l�N���A */
		ret = MciResetMotion( &hNetData->mt_hdl );
		if( ret != MCI_WAIT_COMP )
		{
			pCmdHdl->cmd_seq = (ret == MCI_SUCCESS) ? 
								PHASE_CHK_SEQ : SV_OFF_SEQ;
		}
		break;

	case PHASE_CHK_SEQ:			/* �t�F�[�Y�_�E���`�F�b�N */
		ret = (pComHdl->reqPhase != 0) ? COM_FUNC_CONTINUE : pComHdl->PhUpSts;
		if( ret != COM_FUNC_CONTINUE )
		{
			pCmdHdl->cmd_seq = SOFT_RESET_SEQ;
		}
		break;

	case SOFT_RESET_SEQ:		/* �V�X�e�����Z�b�g�������s */
		ret = RegRequestCommand(&hNetData->reg_hdl, REG_CMD_SYSRST, NULL);
		if( ret == REG_CONTINUE ) 
		{ /* �\�t�g���Z�b�g�����I�� */
			pCmdHdl->cmd_seq = WAIT_SOFT_RESET_SEQ;
		}
		break;

	case WAIT_SOFT_RESET_SEQ:	/* �V�X�e�����Z�b�g�҂� */
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



/****************************************************************************/
/* �T�v		�F	�ʐM���Z�b�g�v������										*/
/*				�ʏ�^�]����TMC�t���[������M������A�R�l�N�V�����ؒf����	*/
/*				�̎��s��v������											*/
/****************************************************************************/
/* ����		�F	CMD_HNDL* hdl	CMD ANALYZER ��	�n���h��					*/
/* �߂�l	�F	�Ȃ�														*/
/****************************************************************************/
/* ���ŗ���	:																*/
/*	Ver1.00																	*/
/****************************************************************************/
LONG CmdReqComReset( MLNET_HANDLE *hNetData )
{
	if( hNetData->com_hdl.Phase > 0 )
	{ 
		if( hNetData->com_hdl.Phase >= 2 )
		{/* �ʐM�t�F�[�Y��2�̏ꍇ�͒ʐM���Z�b�g�v�������s */
			hNetData->cmd_hdl.cmd_seq = REQ_PHSDOWN_SEQ;
		}

		if( CmdRunDisConnect( hNetData ) == CMD_FUNC_NORMAL_END )
		{
			hNetData->com_hdl.Phase = 0;
		}
	}
	else
	{ /* �ʐM�t�F�[�Y<2�̏ꍇ�͂��łɒʐM���Z�b�g����Ă���̂�Phase=0�ɂ��� */
		hNetData->com_hdl.Phase = 0;
	}

	return hNetData->com_hdl.Phase;
}



