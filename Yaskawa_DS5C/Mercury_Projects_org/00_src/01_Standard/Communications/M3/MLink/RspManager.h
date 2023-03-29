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
#ifndef RSP_MANAGER_H_

#include "M3Handler.h"


/* RSP Manager�̏��������� */
VOID RspInit( RSP_HNDL *hdl );
/* ���C���R�}���h�ɑ΂���RSP�f�[�^�쐬���� */
VOID RspMakeData( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf );
/* �T�u�R�}���h�ɑ΂���RSP�f�[�^�쐬���� */
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *ex_rsp_buf );

VOID RspSetcPrmMon(RSP_HNDL *hdl, ULONG data, UCHAR num);




#define RSP_MANAGER_H_
#endif /* RSP_MANAGER_H_ */


