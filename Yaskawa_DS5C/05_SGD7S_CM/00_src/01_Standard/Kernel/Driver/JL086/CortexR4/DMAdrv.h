/****************************************************************************
Description	: DMA Driver
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#ifndef __DMA_DRV_H 
#define __DMA_DRV_H

/* defines */
#define DMA_MAX_CH				4
#define DMA_LINK_OFS			MAX_AXIS_NUMBER

#define DMA_SVDEV_CH			2				/* DMA channel number */
#define DMA_SVDEV_OUT_LINK		0				/* Link Descriptor Index number */
#define DMA_SVDEV_IN_LINK		DMA_LINK_OFS	/* Link Descriptor Index number */

#define DMA_MLDEV_CH			3						/* DMA channel number */
#define DMA_MLDEV_OUT_LINK(ch)	(DMA_LINK_OFS*(2+ch))	/* Link Descriptor Index number */
#define DMA_MLDEV_IN_LINK(ch)	(DMA_LINK_OFS*(4+ch))	/* Link Descriptor Index number */

#define DMA_STATUS_MASK			((0xB4000000 >> (DMA_SVDEV_CH*8)) | (0xB4000000 >> (DMA_MLDEV_CH*8)))

/* for DMA transfers */
typedef struct DMA_ADDR_MAP {
	void	*SrcAddr;
	void	*DstAddr;
	UINT	Length;
	UINT	NextLink;
} DMA_ADDR_MAP;

typedef struct DMA_SHARED_MEMORY {
	ULONG	uDmaInBuffer[MAX_AXIS_NUMBER][32];
	ULONG	uDmaOutBuffer[MAX_AXIS_NUMBER][32];
	ULONG	mlDmaInBuffer[MAX_AXIS_NUMBER][16];
	ULONG	mlDmaOutBuffer[MAX_AXIS_NUMBER][16];
} DMA_SHARED_MEMORY;


#ifdef __cplusplus
extern	"C"	{
#endif

LONG DMAdrvInitialize(UINT ch);
LONG DMAdrvSetLinkDescriptor(DMA_ADDR_MAP *AddrMap, UINT MapNum, UINT LinkNumber);
LONG DMAdrvSetM3ExLinkDescInfo(ULONG RcvTglBufAddrOfst, 
								ULONG RcvLinkNumber0, ULONG RcvLinkNumber1);

void DMAdrvEnableM3Transfer(BOOL enable);

void DMAdrvStartMemoryTransfer(UINT ch, UINT LinkNumber);
BOOL DMAdrvWaitForTransferEnd(UINT ch, UINT Timeout);
void DMAdrvClearErrorStatus(void);

void DMAdrvLockCacheRegion(UINT CacheType, void* address, UINT Length);


#ifdef __cplusplus
}
#endif

#endif //__DMA_DRV_H 

