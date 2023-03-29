/****************************************************************************/
/*																			*/
/*		Command Option Module Task functions								*/
/*																			*/
/****************************************************************************/
/*																			*/
/*	Target model: SigmaV Command-Option	ServoDrive							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	History:																*/
/*	2007.9.12	created														*/
/*																			*/
/*																			*/
/************** Copyright(c) Yaskawa Electric Corporation *******************/
#ifndef		CMOPTIF_TASKS_H_
#define		CMOPTIF_TASKS_H_

/* M-2 SC Message. operation API */
#include "SC_UartDrv.h"

/* external functions */
/* M-2 Task functions */
void MLOpiInitialize(void);
void MLOpiCalcInitParam(void);
void MLOpiInitDevice(void);
void MLOpiStartDevice(void);
void MLOpiIntSync(void);
void MLOpiPreCtrlProcedure(void);
void MLOpiPostCtrlProcedure(void);
void MLOpiScanNProcedure(void);
void MLOpiScanCProcedure(void);
void MLOpiRoundProcedure(void);

/* DPM Task functions */
void DPiInitialize(void);
void DPiStartDevice(void);
void DPiIntSync(void);
void DPiPreCtrlProcedure(void);
void DPiPostCtrlProcedure(void);
void DPiScanCProcedure(void);
void DPiRoundProcedure(void);
/* DPM CS message functions */
LONG DPiReqRecvCSMsg(UCHAR ch);
LONG DPiGetRecvCSMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz);
void DPiReadCSMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz);
void DPiWriteCSMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz);
LONG DPiReqSendCSMsg(UCHAR ch);
LONG DpiGetSendCSMsgSts(UCHAR ch);
LONG DpiSendMsgAbort(UCHAR ch);
/* DPM DegOp. operation API */
LONG DPiGetDegOpState(DEGOP_PRMRNG *pPrmRange);
LONG DPiReqDegOpPrmRead(USHORT cmd, USHORT no);
LONG DPiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data);
LONG DPiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data);
/*------------- <V685> Add Begin -------------------------------------------*/
USHORT DpiGetDegOpNumOfFn(void);
LONG DpiReqDegOpFn(USHORT cmd, USHORT idx, USHORT data);
LONG DpiGetDegOpFn(DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp);
/*------------- <V685> Add End ---------------------------------------------*/
/* DPM SC Message. operation API */
LONG DPiGetSCMsgState(void);
LONG DPiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz);
LONG DPiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz);
void DPiCancelSCMsgReq(void);

void	ApxAinputNetIf( void );	/* 62.5 Spcecial */

#endif		/* DPM_OPT_H_ */
