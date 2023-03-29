/****************************************************************************/
/*																			*/
/*		Command Option Module SC-UART functions								*/
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
#ifndef		SC_UART_DRV_H_
#define		SC_UART_DRV_H_

/* Public functions */
LONG MLOpiGetSCMsgState(void);
LONG MLOpiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz);
LONG MLOpiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz);
void MLOpiCancelSCMsgReq(void);

void OpxInitSCSerialMsg( void );
void OpxSCSifMsgManager( void );
void OpxEnableSCSif( USHORT data );

#endif		/* SC_UART_DRV_H_ */
