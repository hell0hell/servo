/****************************************************************************
Description	: Definition for MECHATROLINK Response Maker
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#ifndef RSP_FUNCDEF_H_

//#include "M3Handler.h"
#include "CmdHandler.h"
#include "ComAlmMngr.h"

/****************************************************************************
 Response functions
****************************************************************************/
/* �X�e�[�^�X�쐬�֐���` */
/* �X�e�[�^�X���� */
USHORT stdRspMakeCMD_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, USHORT sts );
/* �T�u�R�}���h�X�e�[�^�X���� */
ULONG stdRspMakeSUB_STAT( CMD_HNDL *pCmdHdl, ALM_HNDL *pAlmHdl, ULONG sts );

/* ���X�|���X�쐬�֐���` */
/* ����`/MEM_WR�R�}���h���X�|���X���� */
VOID stdRsp_SYS_CMD( void *hNetData, void *rsp_buf );
/* ID_RD�R�}���h���X�|���X���� (03H) */
VOID stdRsp_ID_RD( void *hNetData, void *rsp_buf );
/* NOP,SYNC_SET,DISCONNECT�R�}���h���X�|���X���� (00/0D/0FH) */
VOID stdRsp_NOP( void *hNetData, void *rsp_buf );
/* CONFIG�R�}���h���X�|���X���� (04H) */
VOID stdRsp_CONFIG( void *hNetData, void *rsp_buf );
/* ALM_RD�R�}���h���X�|���X���� (06H) */
VOID stdRsp_ALM_RD( void *hNetData, void *rsp_buf );
/* ALM_CLR�R�}���h���X�|���X���� (06H) */
VOID stdRsp_ALM_CLR( void *hNetData, void *rsp_buf );
/* CONNECT�R�}���h���X�|���X���� (0EH) */
VOID stdRsp_CONNECT( void *hNetData, void *rsp_buf );
/* POS_SET�R�}���h���X�|���X���� (20H) */
VOID stdRsp_POS_SET( void *hNetData, void *rsp_buf );
/* MEM_RD�R�}���h���X�|���X���� (1DH) */
VOID stdRsp_MEM_RD( void *hNetData, void *rsp_buf );
/* SVPRM_RD�R�}���h���X�|���X���� (40H) */
VOID stdRsp_SVPRM_RD( void *hNetData, void *rsp_buf );
/* SVPRM_WR�R�}���h���X�|���X���� (41H) */
VOID stdRsp_SVPRM( void *hNetData, void *rsp_buf );
/* ���j�^�L��R�}���h���X�|���X���� (**H) */
VOID stdRsp_MON_CMD( void *hNetData, void *rsp_buf );

/* �T�u�R�}���h  ���X�|���X�쐬�֐���` */
/* ����`/ALM_RD/MER_WR/SVPRM_WR�R�}���h���X�|���X���� */
VOID stdExRsp_SYS_CMD( void *hNetData, void *rsp_buf );
/* NOP�R�}���h���X�|���X���� (00H) */
VOID stdExRsp_NOP( void *hNetData, void *rsp_buf );
/* ALM_RD�R�}���h���X�|���X���� (06H) */
VOID stdExRsp_ALM_RD( void *hNetData, void *rsp_buf );
/* ALM_CLR�R�}���h���X�|���X���� (06H) */
VOID stdExRsp_ALM_CLR( void *hNetData, void *rsp_buf );
/* SMON �R�}���h���X�|���X���� */
VOID stdExRsp_MON_CMD( void *hNetData, void *rsp_buf );
/* SVPRM_RD�R�}���h���X�|���X���� */
VOID stdExRsp_SVPRM_RD( void *hNetData, void *rsp_buf );
/* MEM_RD�R�}���h���X�|���X���� */
VOID stdExRsp_MEM_RD( void *hNetData, void *rsp_buf );



#define RSP_FUNCDEF_H_
#endif /* RSP_FUNCDEF_H_ */


