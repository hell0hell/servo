/****************************************************************************/
/*																			*/
/*																			*/
/*		NetWortk Driver ヘッダファイル										*/
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
/*	【改版履歴】															*/
/*		Ver1.00																*/
/*																			*/
/*																			*/
/****************************************************************************/
#ifndef		NET_DRIVER_H_
#define		NET_DRIVER_H_

/* includes */
#include "jlComdrv.h"
#include "NetMsgDrv.h"


/****************************************************************************/
/*		DEFINES																*/
/****************************************************************************/
/* Error code definitions */
#define ND_SUCCESS				(0)			/* 正常終了 */
#define ND_INITVAL_ERR			(-1)		/* パラメータ設定エラー */
#define ND_SYNC_ERR				(-2)		/* 不正同期割り込みエラー */
#define ND_ASICSYS_ERR			(-3)		/* JL080SYSTEM設定エラー */
#define ND_RECV_ERR				(-4)		/* Receiveエラー */


/****************************************************************************/
/*		STRUCT DEFINITION													*/
/****************************************************************************/
/* 通信パラメータ構造体 */
typedef struct ND_PARAM_ {
	USHORT	TCycle;			/* 伝送周期      [0.25us] */
	USHORT	TSlot;			/* タイムスロット[0.25us] */
	UCHAR	NodeAddress;	/* 自局アドレス */
	UCHAR	DataSize;		/* 伝送バイト数 */
	UCHAR	TSpeed;			/* 伝送速度 4Mbps, 10Mbps, 100Mbps */
} ND_PARAM;





/****************************************************************************/
/*		API's																*/
/****************************************************************************/
/* Globals: private */
INT32	NdResetDevice(ND_PARAM* nd_prm);
void		NdExchangeMsgPkt(void);
void		NdExchangeMsgPktAsync(void);


/* forward declarations */
INT32 NdInit(ND_PARAM* nd_prm, void *DeviceAddress);
INT32 NdReceiveGblInfo(USHORT* t_cyc,USHORT* t_slot);
INT32 NdGetDevStatus(ULONG *ests);
INT32 NdReceiveCMD(LONG ch, ULONG *pBuf);
void NdSendRSP( UINT st_no, ULONG *Buffer );
void NdControlMsgPkt(void);

LONG NdReqRecvMsg(UCHAR ch);
#if (ND_MSGDRV_VER > 0x100)
LONG NdGetRecvMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz);
#else
LONG NdGetRecvMsgSts(UCHAR ch, USHORT *rcvSz);
#endif
void NdReadMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz);
void NdWriteMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz);
LONG NdReqSendMsg(UCHAR ch);
LONG NdGetSendMsgSts(UCHAR ch);

ND_PARAM* NdGetNdParamPtr( VOID );
void NdResetAsic( void *DeviceAddress ); /* <S14E> */

#endif /* NET_DRIVER_H_ */

