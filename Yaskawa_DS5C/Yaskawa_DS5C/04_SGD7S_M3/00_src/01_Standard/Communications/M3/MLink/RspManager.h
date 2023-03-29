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


/* RSP Managerの初期化処理 */
VOID RspInit( RSP_HNDL *hdl );
/* メインコマンドに対するRSPデータ作成処理 */
VOID RspMakeData( MLNET_HANDLE *hNetData, RSP_LINK_DATA *rsp_buf );
/* サブコマンドに対するRSPデータ作成処理 */
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA *ex_rsp_buf );

VOID RspSetcPrmMon(RSP_HNDL *hdl, ULONG data, UCHAR num);




#define RSP_MANAGER_H_
#endif /* RSP_MANAGER_H_ */


