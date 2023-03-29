/****************************************************************************
Description	: MECHATROLINK Command Analyzer
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef CMD_ANALYZER_H_

#include "M3Handler.h"

/****************************************************************************
 API's 
****************************************************************************/
/* �֐��߂�l��` */
#define CMD_FUNC_NORMAL_END		0x0
#define CMD_FUNC_CONTINUE		0x1
#define CMD_CMDLOCK_ERR			ERR_SUBCMD
#define CMD_PHASE_ERR_M2		ERR_CMD_CNDTN
#define CMD_PHASE_ERR_M3		ERR_COM_PHASE
#define CMD_TIME_OUT_ERR		ERR_CMD_FAIL
#define CMD_PROFILE_UNKNOWN		ERR_DATA_SET
#define CMD_PROFILE_DETECT		1

#define DISCONNECT_CMD			0x0F			/* DISCONNECT �R�}���h�R�[�h */


/* CMD AnaLyzer�̏��������� */
VOID CmdInit( CMD_HNDL *hdl );
/* ���C���R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v������ */
INT32 CmdUpdateData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL rsp_set );
/* �T�u�R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v������ */
INT32 CmdUpdateExData( CMD_HNDL *pCmdHdl, COM_HNDL *pComHdl, BOOL ex_rsp_set );
/* ���C���R�}���h�̉�́E���s���� */
VOID CmdAnalyze( MLNET_HANDLE *hNetData );
/* �T�u�R�}���h�̉�́E���s���� */
VOID CmdAnalyzeEx( MLNET_HANDLE *hNetData );
/* �v���t�@�C���^�C�v�`�F�b�N�A�x�[�X�|�C���^�̐ݒ菈�� */
INT32 CmdSetProfileType( MLNET_HANDLE *hNetData, LONG tdata_size );
/* �R�l�N�V�����ؒf���� */
INT32 CmdRunDisConnect( MLNET_HANDLE *hNetData );
/* �ʐM���Z�b�g�v������ */
LONG CmdReqComReset( MLNET_HANDLE *hNetData );



#define CMD_ANALYZER_H_
#endif /* CMD_ANALYZER_H_ */


