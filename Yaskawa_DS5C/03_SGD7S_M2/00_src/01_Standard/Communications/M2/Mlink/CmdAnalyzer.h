/****************************************************************************/
/*																			*/
/*																			*/
/*		CMD Analyzer �w�b�_�t�@�C��											*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*	�y���ŗ����z															*/
/*		Ver1.00																*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		CMD_ANALYZER_H_
#define		CMD_ANALYZER_H_

/* includes */
#include "M2Handler.h"

/* defines */
/* �֐��߂�l��` */
#define CMD_FUNC_NORMAL_END	ERR_NONE
#define CMD_FUNC_CONTINUE	ERR_WAIT_COMP
#define CMD_CMDLOCK_ERR		ERR_SUBCMD
#define CMD_PHASE_ERR		ERR_CMD_CNDTN
#define CMD_TIME_OUT_ERR	ERR_CMD_FAIL

#define DISCONNECT_CMD			0x0F			/* DISCONNECT �R�}���h�R�[�h */

/* CMD AnaLyzer�̏��������� */
VOID CmdInit(CMD_HNDL* pCmdHdl);
/* ���C���R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v������ */
INT32 CmdUpdateData(CMD_HNDL* pCmdHdl, COM_HNDL* pComHdl, BOOL rsp_set);
/* �T�u�R�}���h��͗p�o�b�t�@�̍X�V�ƃR�}���h��͎��s�v������ */
INT32 CmdUpdateExData(CMD_HNDL* pCmdHdl, COM_HNDL* pComHdl, BOOL ex_rsp_set);
/* ���C���R�}���h�̉�́E���s���� */
VOID CmdAnalyze( MLNET_HANDLE *hNetData );
/* �T�u�R�}���h�̉�́E���s���� */
VOID CmdAnalyzeEx( MLNET_HANDLE *hNetData );

#endif /* CMD_ANALYZER_H_ */
