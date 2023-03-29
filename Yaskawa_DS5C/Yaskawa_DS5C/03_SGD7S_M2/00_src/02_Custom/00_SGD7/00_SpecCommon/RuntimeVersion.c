/****************************************************************************
Description	: Definitions for ROM Information
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2011 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.12.22     created

****************************************************************************/
#include "Basedef.h"
#include "FileDownload.h"
#include "indsecinfo.h"


extern UINT32 __ghs_romblocksize;


const ROM_BLOCK_HDRINFO RomHeaderInfo =
{
 	/* Magic key */
 	"YSFR",
 	/* Header size */
	sizeof(ROM_BLOCK_HDRINFO),
 	/* type */
	ROMINF_TYPE_FLUSH_BIN,
	/* Number of Blocks */
	1,
 	/* ROM size */
	(ULONG)&__ghs_romblocksize,//0x3C0000,
 	/* crc code */
	0,

 	/* Major Version */
	(USHORT)(SERVO_FIRM_VER>>16),
 	/* Minor Version */
	(USHORT)SERVO_FIRM_VER,
	/* ROM start address */
	(ULONG)&RomHeaderInfo,//0x01C00000,
 	/* Sector Address */
	(ULONG)&__secinfo,
 	/* far jump Address */
	0,
};
