/****************************************************************************
Description	: EEPROM Access Manager
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	EepdevInitialize()				: Initialize the Eeprom device

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#include "Basedef.h"
#include "EepromIF.h"

#define SIZE_BLOCK_A	EEP_MAP_HEADER_INFO_LENGTH/2

/* Default value of the Eeprom Header Information on IFA board */
const EEP_HEADER_INFO_T EepDefaultInfoValue = {
	SIZE_BLOCK_A,	//SizeA
	EEP_FORMAT_ID,	//FormatID;
	0,				//VendorID;
	0x02230001,		//ProductID;
	EEP_IFBOARD_ID,	//Board ID;
	EEP_BOARD_DATE,	//Date;
	"SGDV-MDA02E8M3A",
	"",
	0,				//Checksum
	0,				//SizeB
};

#if 0/* <S04E> */
const EEP_NOCHKSUM_CMN_T EepCmnDefaultValue = {
    {0x0020,0xb529,0x0001, 0x0000},
    0xC0A80101,
    0,
    0xFFFFFF00,
    "CSTR-IFAM3LA",
    {0},
    0,
    0,
    0,
    0,
};/* for IFA */
#endif

/* Default parameters on MBB module */
#define EEP_MBB_MAX_ID			3
#define EEP_MBB_PRM_ENTRY		2
const USHORT EepDefaultMBBParams[EEP_MBB_PRM_ENTRY][EEP_MBB_MAX_ID] = {
/* 0xE5B */   		{ 	0xffff,0xffff,0xffff 	},
/* reserve(0xE7C) */  { 18520,18520,18520 	},
/* reserve(削除) *///  { (USHORT)-18520,(USHORT)-18520,(USHORT)-18520 	},
};
const ULONG EepDefaultMBBParamsSize = sizeof(EepDefaultMBBParams)/EEP_MBB_MAX_ID;


#ifdef	CSW_CSTR_MBB_TYPE			/* for MBB board */
/* Default value of the Eeprom Header Information on COA module */
const EEP_HEADER_INFO_T EepDefaultMBBInfo[EEP_MBB_MAX_ID] = {
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_MBB,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x2104,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTM-BB40AA\0A",//CSTR-MBB04AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_MBB,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x2106,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTM-BB60AA\0A",//CSTR-MBB06AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_MBB,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x2108,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTM-BB80AA\0A",//CSTR-MBB08AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
};
#else			/* for MBA board */
/* Default value of the Eeprom Header Information on COA module */
const EEP_HEADER_INFO_T EepDefaultMBBInfo[EEP_MBB_MAX_ID] = {
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_MBB,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x2104,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-MBA04AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_MBB,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x2106,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-MBA06AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_MBB,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x2108,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-MBA08AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
};
#endif



/*------------------------------------------------------------------------------*/
/* ＜＜チェックサムの注意点＞＞													*/
/*																				*/
/* ■MBB rev.B																	*/
/* MBBに搭載したEepromから0xE5B、0xE7Cを読み出してチェックサムの計算をします。	*/
/*																				*/
/* ■MBAもしくはMBB rev.A														*/
/* Eepromを搭載していないので、ダミー変数としてPnE5C、PnE7Dを読み出して			*/
/* チェックサムの計算をします。													*/
/*																				*/
/*------------------------------------------------------------------------------*/
/* ＜＜PnE5B、PnE5C、PnE7C、PnE7Dのパラメータについて＞＞						*/
/* ■MBB rev.B（Eeprom搭載）													*/
/* PnE5B：Eepromへ読み書き														*/
/* PnE5C：変更不可（読み出しのみ）												*/
/* PnE7C：Eepromへ読み書き														*/
/* PnE7D：変更不可（読み出しのみ）												*/
/*																				*/
/* ■MBAもしくはMBB rev.A（Eeprom非搭載）										*/
/* PnE5B：RAMのみ変更可能														*/
/*        電源を再投入すると初期値（PrmTblIngram.cのDefault value）に戻ります。 */
/* PnE5C：変更不可（読み出しのみ）												*/
/* PnE7C：RAMのみ変更可能。														*/
/*        電源を再投入すると初期値（PrmTblIngram.cのDefault value）に戻ります。	*/
/* PnE7D：変更不可（読み出しのみ）												*/
/*																				*/
/*------------------------------------------------------------------------------*/
/* Default parameters on COA module */
#define EEP_COA_MAX_ID			5
#define EEP_COA_PRM_ENTRY		19
const USHORT EepDefaultCOAParams[EEP_COA_PRM_ENTRY][EEP_COA_MAX_ID] = {
/* 0x630 */   { 	1500, 	4000, 	8000, 	2000, 	1700, 	}, 			
/* 0xE0C */   { 	0x2302, 	0x2302, 	0x2302, 	0x2302, 	0x2302, 	},
/* 0xE1C */   { 	0, 	0, 	0, 	0, 	0, 	},
/* 予約(0xE5C) */   { 	0, 	0, 	0, 	0, 	0, 	},
/* 0xE70 */   { 	500, 	500, 	500, 	500, 	500, 	},
/* 0xE71 */   { 	425, 	425, 	425, 	425, 	410, 	},
/* 0xE72 */   { 	400, 	400, 	400, 	400, 	385, 	},
/* 0xE73 */   { 	170, 	170, 	170, 	170, 	170, 	},
/* 0xE74 */   { 	0, 	0, 	0, 	0, 	0, 	},
/* 0xE78 */   { 	10, 	10, 	10, 	10, 	10, 	},
/* 0xE79 */   { 	0, 	0, 	0, 	0, 	0, 	},
/* 0xE7A */   { 	0x55, 	0x55, 	0x55, 	0x55, 	0x55, 	},
/* 0xE7B */   { 	200, 	200, 	200, 	200, 	200, 	},
/* 予約(0xE7D) */   { 	18664, 	18664, 	18664, 	18520, 	18520, 	},
/* 0xE80 */   { 	15238, 	15238, 	15238, 	15242, 	15242, 	},
/* 0xE81 */   { 	3487, 	3487, 	3487, 	3487, 	3487, 	},
/* 0xE82 */   { 	8408, 	8408, 	8408, 	8408, 	8408, 	},
/* 0xE83 */   { 	10000, 	10000, 	10000, 	10000, 	10000, 	},
/* 0xE84 */   { 	5000, 	5000, 	5000, 	5000, 	5000, 	},
};



const ULONG EepDefaultCOAParamsSize = sizeof(EepDefaultCOAParams)/EEP_COA_MAX_ID;




/* Default value of the Eeprom Header Information on COA module */
const EEP_HEADER_INFO_T EepDefaultCOAInfo[EEP_COA_MAX_ID] = {
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_COA,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x0101,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-COA01AA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_COA,	//FormatID;
		0,				//VendorID;
		1,				//ProductID;
		0x0104,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-COA04AA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_COA,	//FormatID;
		0,				//VendorID;
		2,				//ProductID;
		0x0108,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-COA08AA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_COA,	//FormatID;
		0,				//VendorID;
		3,				//ProductID;
		0x0202,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTC-BO20AA", //CSTR-COB02AA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_COA,	//FormatID;
		0,				//VendorID;
		4,				//ProductID;
		0x0302,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTC-CO20AA", //CSTR-COC02AA
		"",
		0,				//Checksum
		0,				//SizeB
	},
};



/* Default parameters on SDA module */
#define EEP_SDA_ADJPRM_IDX			{22, 23, 24}
#define EEP_SDA_MAX_ID				8
#define EEP_SDA_PRM_ENTRY			41
const USHORT EepDefaultSDAParams[EEP_SDA_PRM_ENTRY][EEP_SDA_MAX_ID] = {
/* Pn601 */   { 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0 },
/* PnE11 */   { 	0x01A5, 	0x0101, 	0x0102, 	0x0104, 	0x0105, 	0x0108, 	0x0110, 	0x0115 },
/* PnE12 */   { 	50, 	100, 	200, 	400, 	500, 	750, 	1000, 	1500 },
/* PnE14 */   { 	9, 	13, 	23, 	40, 	54, 	78, 	107, 	164 },
/* PnE15 */   { 	30, 	41, 	82, 	132, 	156, 	239, 	240, 	396 },
/* PnE17 */   { 	0x1473, 	0x1473, 	0x1773, 	0x1473, 	0x1473, 	0x1473, 	0x1473, 	0x1473 },
/* PnE18 */   { 	0x0301, 	0x0301, 	0x0301, 	0x0301, 	0x050A, 	0x050A, 	0x050A, 	0x050A },
#ifdef	CSW_CSTR_MBB_TYPE								/* for SDB board */
/* PnE1B */   { 	0, 	0, 	0, 	0, 	33000, 	33000, 	33000, 	33000 },
#else													/* for SDA board */
/* PnE1B */   { 	0, 	0, 	0, 	0, 	2500, 	2500, 	2500, 	2500 },
#endif
/* PnE1D */   { 	1000, 	1000, 	1000, 	1000, 	60, 	40, 	30, 	33 },
/* PnE20 */   { 	1800, 	1800, 	1800, 	1800, 	1800, 	1800, 	900, 	900 },
/* PnE21 */   { 	1800, 	1800, 	1800, 	1800, 	1800, 	1800, 	900, 	900 },
/* PnE22 */   { 	600, 	600, 	600, 	600, 	600, 	600, 	600, 	600 },
/* PnE23 */   { 	600, 	600, 	600, 	600, 	600, 	600, 	600, 	600 },
/* PnE26 */   { 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0 },
/* PnE28 */   { 	6989, 	6508, 	6298, 	6758, 	6390, 	6730, 	6758, 	6843 },
/* PnE2C */   { 	10667, 	10667, 	10667, 	10667, 	10667, 	10667, 	8000, 	4000 },
/* PnE2D */   { 	0x1E1E, 	0x1E1E, 	0x1E1E, 	0x1E1E, 	0x1E1E, 	0x1E1E, 	0x1E1E, 	0x3232 },
/* PnE2E */   { 	0x0F64, 	0x0F64, 	0x0A64, 	0x0564, 	0x0A64, 	0x0A64, 	0x0864, 	0x0864 },
/* PnE2F */   { 	0x0D0A, 	0x0D0A, 	0x0D0A, 	0x0D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A },
/* PnE45 */   { 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000 },
/* PnE46 */   { 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000 },
/* PnE47 */   { 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000 },
/* PnE50 */   { 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF },
/* PnE51 */   { 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF },
/* PnE52 */   { 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF, 	0xFFFF },
/* PnE90 */   { 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000 },
/* PnE91 */   { 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000, 	0x0000 },
/* PnE93 */   { 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0 },
/* PnE94 */   { 	0, 	0, 	0, 	0, 	0, 	0, 	0, 	0 },
/* PnEA4 */   { 	100, 	100, 	100, 	100, 	100, 	100, 	100, 	100 },
/* PnEA6 */   { 	30, 	30, 	30, 	30, 	30, 	30, 	30, 	30 },
/* PnEA7 */   { 	10000, 	10000, 	10000, 	10000, 	10000, 	10000, 	0, 	0 },
/* PnEB3 */   { 	115, 	115, 	115, 	115, 	127, 	115, 	127, 	127 },
/* PnEDA */   { 	6000, 	6000, 	6000, 	6000, 	6000, 	6000, 	6000, 	6000 },
/* PnEDB */   { 	100, 	100, 	100, 	100, 	100, 	100, 	50, 	50 },
/* PnEDC */   { 	10, 	10, 	10, 	10, 	10, 	10, 	10, 	10 },
/* PnEDD */   { 	10, 	10, 	10, 	10, 	10, 	10, 	10, 	10 },
/* 0xEE8 */   { 	0x2D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A, 	0x2D0A },
/* 0xEE9 */   { 	127, 	127, 	127, 	127, 	127, 	127, 	127, 	127 },
/* 0xEEA */   { 	6000, 	6000, 	6000, 	6000, 	6000, 	6000, 	6000, 	6000 },
/* 0xEEB */   { 	100, 	100, 	100, 	100, 	100, 	100, 	100, 	100 },
};
const ULONG EepDefaultSDAParamsSize = sizeof(EepDefaultSDAParams)/EEP_SDA_MAX_ID;

#ifdef	CSW_CSTR_MBB_TYPE	/* for SDB board */
/* Default value of the Eeprom Header Information on SDB board */
const EEP_HEADER_INFO_T EepDefaultSDAInfo[EEP_SDA_MAX_ID] = {
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x3001,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BD7RA0AA",//CSTR-SDBR70AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		1,				//ProductID;
		0x3002,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BD9RA0AA",//CSTR-SDBR90AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		2,				//ProductID;
		0x3003,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BDR1A6AA",//CSTR-SDB1R6AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		3,				//ProductID;
		0x3004,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BDR2A8AA",//CSTR-SDB2R8AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		4,				//ProductID;
		0x3005,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BDR3A8AA",//CSTR-SDB3R8AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		5,				//ProductID;
		0x3006,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BDR5A5AA",//CSTR-SDB5R5AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		6,				//ProductID;
		0x3007,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BDR7A6AA",//CSTR-SDB7R6AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		7,				//ProductID;
		0x3008,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"SCRTS-BD21A0AA",//CSTR-SDB120AAA
		"",
		0,				//Checksum
		0,				//SizeB
	},
};
#else	/* for SDA board */
/* Default value of the Eeprom Header Information on SDA board */
const EEP_HEADER_INFO_T EepDefaultSDAInfo[EEP_SDA_MAX_ID] = {
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		0,				//ProductID;
		0x3001,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDAR70AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		1,				//ProductID;
		0x3002,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDAR90AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		2,				//ProductID;
		0x3003,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDA1R6AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		3,				//ProductID;
		0x3004,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDA2R8AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		4,				//ProductID;
		0x3005,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDA3R8AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		5,				//ProductID;
		0x3006,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDA5R5AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		SIZE_BLOCK_A,	//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		6,				//ProductID;
		0x3007,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDA7R6AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
	{
		0,				//SizeA
		EEP_FORMAT_ID_SDA,	//FormatID;
		0,				//VendorID;
		7,				//ProductID;
		0x3008,			//Board ID;
		EEP_BOARD_DATE,	//Date;
		"CSTR-SDA120AAA",
		"",
		0,				//Checksum
		0,				//SizeB
	},
};
#endif

const UINT8 EepDefaultSDA_WrSkipIdx[] = EEP_SDA_ADJPRM_IDX;



/***************************************************************************
Description: Check a parameter to be skipped in parameter restoring.

Parameters:
Return:
	Error
****************************************************************************/
static BOOL EepCfg_IsWriteSkipParameter(UINT wrIndex, const UINT8 *SkipIdxArray, UINT ArrayNum)
{
	UINT i;
	for(i = 0; i < ArrayNum; i++)
	{
		if(wrIndex == SkipIdxArray[i])
		{
			return TRUE;
		}
	}
	return FALSE;
}


# if 0
/***************************************************************************
Description: Initialize the SDA Eeprom parameters

Parameters:
	EEP_DEVICE_HANDLE *hEepDev	: Pointer to EEPROM device handle
	USHORT vadr					: Virtual EEPROM top address
	UINT ampType				: SDA ID number
Return:
	Error
****************************************************************************/
void EepCfg_InitAmpParameters(EEP_DEVICE_HANDLE *hEepDev, USHORT vadr, UINT ampType)
{
	UINT   i;
	USHORT address;
	USHORT checksum;
	USHORT sizeB;
	USHORT offset;
	USHORT value;

	EEP_HEADER_INFO_T eep_header;

	if((ampType == 0) || (ampType > EEP_SDA_MAX_ID))
	{
		return;
	}

	offset = sizeof(eep_header.Checksum) + sizeof(eep_header.SizeBlockB);
	sizeB = (USHORT)((EepDefaultSDAParamsSize + offset)/2);

	// Write the header information
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_SDA, vadr,
						   (USHORT*)&EepDefaultSDAInfo[ampType-1],
						   sizeof(EEP_HEADER_INFO_T)/2);

	// Write the default parameter values
	checksum = sizeB;
	address = vadr + EEP_MAP_HEADER_INFO_LENGTH + offset;
	for(i = 0; i < EEP_SDA_PRM_ENTRY; i++)
	{
		if(FALSE != EepCfg_IsWriteSkipParameter(i,
												EepDefaultSDA_WrSkipIdx,
												sizeof(EepDefaultSDA_WrSkipIdx)))
		{
			EepdevReadValues(hEepDev, PRMDEF_EEPROM_SDA, address + (i*2), (USHORT*)&value, 1);
		}
		else
		{
			value = EepDefaultSDAParams[i][ampType-1];
			EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_SDA, address + (i*2), (USHORT*)&value, 1);
		}

		checksum += value;
	}

	// Write a checksum value
	checksum = (USHORT)(-checksum);
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_SDA,
		(vadr + EEP_MAP_HEADER_INFO_LENGTH), (USHORT*)&checksum, 1);
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_SDA,
		(vadr + EEP_MAP_HEADER_INFO_LENGTH + sizeof(eep_header.Checksum)), (USHORT*)&sizeB, 1);

}

/***************************************************************************
Description: Initialize the COA Eeprom parameters

Parameters:
	EEP_DEVICE_HANDLE *hEepDev	: Pointer to EEPROM device handle
	USHORT vadr					: Virtual EEPROM top address
	UINT ampType				: COA ID number
Return:
	none
****************************************************************************/
void EepCfg_InitConvParameters(EEP_DEVICE_HANDLE *hEepDev, USHORT vadr, UINT ampType)
{
	UINT   i;
	USHORT address;
	USHORT checksum;
	USHORT sizeB;
	USHORT offset;
	USHORT value;

	EEP_HEADER_INFO_T eep_header;

	if((ampType == 0) || (ampType > EEP_COA_MAX_ID))
	{
		return;
	}

	offset = sizeof(eep_header.Checksum) + sizeof(eep_header.SizeBlockB);
	sizeB = (USHORT)((EepDefaultCOAParamsSize + offset)/2);

	// Write the header information
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_COA, vadr,
						   (USHORT*)&EepDefaultCOAInfo[ampType-1],
						   sizeof(EEP_HEADER_INFO_T)/2);

	// Write the default parameter values
	checksum = sizeB;
	address = vadr + EEP_MAP_HEADER_INFO_LENGTH + offset;
	for(i = 0; i < EEP_COA_PRM_ENTRY; i++)
	{
		value = EepDefaultCOAParams[i][ampType-1];
		EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_COA, address + (i*2), (USHORT*)&value, 1);

		checksum += EepDefaultCOAParams[i][ampType-1];
	}

	// Write a checksum value
	checksum = (USHORT)(-checksum);
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_COA,
		(vadr + EEP_MAP_HEADER_INFO_LENGTH), (USHORT*)&checksum, 1);
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_COA,
		(vadr + EEP_MAP_HEADER_INFO_LENGTH + sizeof(eep_header.Checksum)), (USHORT*)&sizeB, 1);
}



/***************************************************************************
Description: Initialize the MBB Eeprom parameters

Parameters:
	EEP_DEVICE_HANDLE *hEepDev	: Pointer to EEPROM device handle
	USHORT vadr					: Virtual EEPROM top address
	UINT ampType				: MBB ID number
Return:
	none
****************************************************************************/
void EepCfg_InitMBBParameters(EEP_DEVICE_HANDLE *hEepDev, USHORT vadr, UINT mbbType)
{
	UINT   i;
	USHORT address;
	USHORT checksum;
	USHORT sizeB;
	USHORT offset;
	USHORT value;

	EEP_HEADER_INFO_T eep_header;

	if((mbbType == 0) || (mbbType > EEP_MBB_MAX_ID))
	{
		return;
	}

	offset = sizeof(eep_header.Checksum) + sizeof(eep_header.SizeBlockB);
	sizeB = (USHORT)((EepDefaultMBBParamsSize + offset)/2);

	// Write the header information
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_MBB, vadr,
						   (USHORT*)&EepDefaultMBBInfo[mbbType-1],
						   sizeof(EEP_HEADER_INFO_T)/2);

	// Write the default parameter values
	checksum = sizeB;
	address = vadr + EEP_MAP_HEADER_INFO_LENGTH + offset;
	for(i = 0; i < EEP_MBB_PRM_ENTRY; i++)
	{
		value = EepDefaultMBBParams[i][mbbType-1];
		EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_MBB, address + (i*2), (USHORT*)&value, 1);

		checksum += EepDefaultMBBParams[i][mbbType-1];
	}

	// Write a checksum value
	checksum = (USHORT)(-checksum);
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_MBB,
		(vadr + EEP_MAP_HEADER_INFO_LENGTH), (USHORT*)&checksum, 1);
	EepdevWriteNoSumValues(hEepDev, PRMDEF_EEPROM_MBB,
		(vadr + EEP_MAP_HEADER_INFO_LENGTH + sizeof(eep_header.Checksum)), (USHORT*)&sizeB, 1);
}
#endif

/* End of File */
