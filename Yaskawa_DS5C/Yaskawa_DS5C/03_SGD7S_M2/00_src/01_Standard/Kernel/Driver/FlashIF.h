/****************************************************************************
  Description   : FlashIF.h
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : Mercury

  Create Date   :

****************************************************************************/
#ifndef _SELAL_FLUSH_TEST_H_
#define _SELAL_FLUSH_TEST_H_

#include "Basedef.h"
#include "SFlash.h"

/* defines */
#define SF_BASE_ADDR 		0xFFE00000
#define SF_TEST_SECTOR_NO	0
#define SF_BUFSIZE1		256
#define SF_BUFSIZE2		512

/* prototypes */
void	InitSFlashSoC( void );				/* �{��SerialFlash������		*/
UCHAR	SFlashModeChgSoC( UCHAR SPI, UCHAR Read, UCHAR PreFetch, UCHAR Com );		/* �{��SerialFlash���[�h�ύX		*/
LONG	SFlashEraseSectorSoC( ULONG EraseAddr );	/* �{��SerialFlash�Z�N�^�[����	*/
LONG	SFlashWritePageSoC( ULONG WriteAddr, ULONG *WriteBuf );		/* �{��SerialFlash�y�[�W���C�g	*/
LONG	SFlashReadByteSoC( ULONG ReadAddr, UCHAR *ReadBuf );		/* �{��SerialFlash�o�C�g���[�h	*/
void SF_Test1(void);				/* Test1 */

/* with timer */
ULONG SF_Test_Read256byte_t(void);
void SF_Test_Write256byte_t(void);
void SF_SectorErase_t(void);

/* on ScanA */
void SF_SectorErase_s(void);
void SF_Write256byte_s(void);


#endif /* _SELAL_FLUSH_TEST_H_ */
/******************************* End of File *******************************/
