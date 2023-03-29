/****************************************************************************/
/*																			*/
/*																			*/
/*		RSP Manager ÉwÉbÉ_ÉtÉ@ÉCÉã											*/
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
/*	Åyâ¸î≈óöóÅz															*/
/*		Ver1.00																*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		RSP_MANAGER_H_
#define		RSP_MANAGER_H_

#include "M2Handler.h"

/* forward declarations */
VOID RspInit(RSP_HNDL *rspHdl);
VOID RspMakeData( MLNET_HANDLE *hNetData, RSP_LINK_DATA* rsp_buf);
VOID RspMakeExData( MLNET_HANDLE *hNetData, RSP_LINK_EXDATA* ex_rsp_buf);

USHORT rspMakeStatus(RSP_HNDL *pRspHdl, CMD_HNDL *pCmdHdl, MT_HNDL *pMtHdl, ALM_HNDL *pAlmHdl);
UCHAR rspMakeExStatus(CMD_HNDL *cmdHdl, ALM_HNDL *almHdl);


#endif /* RSP_MANAGER_H_ */
