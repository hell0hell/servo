/****************************************************************************/
/*																			*/
/*																			*/
/*		NetWortk Message Driver �w�b�_�t�@�C��								*/
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
/*		Ver1.01	���b�Z�[�W�h���C�oAPI�����ύX								*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		NET_MSGDRIVER_H_
#define		NET_MSGDRIVER_H_

/* includes */

#ifdef	MCCSW_SGDVM2
#define ND_MSGDRV_VER		0x100
#else
#define ND_MSGDRV_VER		0x101
#endif

/* defines */
#define ND_RECV_BUSY			(2)		/* MSG��M�ҋ@��� */
#define ND_SEND_BUSY			(1)		/* MSG���M����� */
#define ND_SUCCESS				(0)		/* ����I�� */
#define ND_RECVMSG_ERR			(-1)	/* MSG��M�G���[ */
#define ND_RECVADR_ERR			(-2)	/* MSG��M�A�h���X�G���[ */
#define ND_SENDMSG_ERR			(-3)	/* MSG���M�G���[ */


/* forward declarations */
#ifdef __cplusplus
extern	"C"	{
#endif

#if defined(__ghs__)
#pragma ghs startzda
#elif !defined(_WIN32)
#pragma section sidata begin
#endif

extern LONG	(*NdiReqRecvMsg)(UCHAR);					/* Request Op-MSG Recv 	*/
#if (ND_MSGDRV_VER > 0x100)
extern LONG	(*NdiGetRecvMsgSts)(UCHAR, USHORT *, USHORT *);	/* Get Op-MSG Recv Sts	*/
extern LONG	(*NdiSendMsgAbort)(UCHAR);						/* Abort message */
#else
extern LONG	(*NdiGetRecvMsgSts)(UCHAR, USHORT *);		/* Get Op-MSG Recv Sts	*/
#define NdiSendMsgAbort(ch)								/* Abort message */
#endif
extern void	(*NdiReadMsgData)(UCHAR, USHORT *, USHORT);	/* Get Op-MSG Recv Data	*/
extern void	(*NdiWriteMsgData)(UCHAR, USHORT *, USHORT);/* Set Op-MSG Send Data	*/
extern LONG	(*NdiReqSendMsg)(UCHAR);					/* Request Op-MSG Send 	*/
extern LONG	(*NdiGetSendMsgSts)(UCHAR);					/* Get Op-MSG Send Sts	*/



#if defined(__ghs__)
#pragma ghs endzda
#elif !defined(_WIN32)
#pragma section sidata end
#endif


#ifdef __cplusplus
}
#endif


#endif /* NET_MSGDRIVER_H_ */
