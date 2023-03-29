/****************************************************************************
Description	: EEPROM Access Interface
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
#ifndef __EEPDEV_API_H
#define __EEPDEV_API_H


/* 2012.08.08 Y.Oka Make Mercury Base Software */
//#include <INTEGRITY.h>
/* 2012.08.08 Y.Oka Make Mercury Base Software */
#include "I2cDriver.h"				/* <S00C> 後で消す */

#if !defined(CSW_NETIF_SYNQNET_TYPE)
#define EEP_SDACOA_ON_IF
#endif



#if defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
/* EEPROM MemoryMap Information (暫定) */
#define DEBUG_EEPROM_SIZE			0x1300		// Byte
#define DEBUG_HEADER_OFST			0x14		// Byte
#endif /* #if defined(_VC_DEBUG_) */ /* 2012.08.22 Y.Oka Make Mercury Base Software */



/* defines */
#define EEP_FORMAT_ID				0x0026U		// Ver 0.34
#define EEP_FORMAT_ID_SDA			0x0006U		// Ver 0.06
#define EEP_FORMAT_ID_COA			0x0003U		// Ver 0.03
#define EEP_FORMAT_ID_MBB			0x0002U		// Ver 0.02

#define EEP_BOARD_DATE				0x0B04		// 2011.04 : Manifacuring date
#define EEP_IFBOARD_ID				0x1006		// M3L:0x1006, M3H:0x1010, MPL:0x1106, MPH:0x1110/


enum EEP_DEVICE_ID_T {
	PRMDEF_EEPROM_IF = 0,
	PRMDEF_EEPROM_SDA,
	PRMDEF_EEPROM_COA,
	PRMDEF_EEPROM_MBB,
	PRMDEF_NO_EEPROM,
};


#define EEP_SDA_MEM_AXIS_ID_BIT			11			// EEP_MAP_BLOCK_LENGTHのbit長
#define EEP_SDA_MEM_AXIS_ID_MASK		0xFF

#define EEP_MAP_HEADER_INFO_ADDRESS		0x0000U
#define EEP_MAP_HEADER_INFO_LENGTH		0x0040U

/* Do not change this address, the kernel module reads this value at start-up. */
#define EEP_NOSUM_CMN_BLOCK_ADDRESS		0x0040U	/* Top Address of the no-sum value for axis common *//* <S04E> */
#define EEP_NOSUM_CMN_BLOCK_LENGTH		0x0040U

#define EEP_NOSUM_BLOCK_ADDRESS			0x0080U	/* Top Address of the no-sum value for axises *//* <S04E> */
#define EEP_NOSUM_BLOCK_LENGTH			0x0180U

/* 2012.08.08 Y.Oka Make Mercury Base Software */
#define EEP_CMNPRM_ADDRESS				0x0200U	/* Top Address of the user common parameters */
#define EEP_AXPRM_TOP_ADDRESS			0x0300U	/* Top Address of the user axis parameters */
#define EEP_CMNPRM_CHKSUM_ADR			0x0200U	/* Top Address of the user common parameters */
#define EEP_AXPRM_CHKSUM_ADR			0x0300U	/* Top Address of the user axis parameters */
//#define EEP_CMNPRM_ADDRESS			0x0800U	/* Top Address of the user common parameters */
//#define EEP_AXPRM_TOP_ADDRESS			0x1000U	/* Top Address of the user axis parameters */
/* 2012.08.08 Y.Oka Make Mercury Base Software */

#define EEP_MAP_BLOCK_LENGTH			0x0800U	/* a block length of the user paraeters */
#define EEP_MAP_AXIS_OFFSET				0x1000U	/* axis offset of each user paraeters */	/* <S0E4> */
#define EEP_MAP_SDABLOCK_LENGTH			0x0100U	/* a block length of the SDA paraeters */
#define EEP_CHECKSUM_ADDRESS_MASK		0xFF00U	/* checksum address mask */
#define EEP_MAP_SDA_OFFSET				0x0700U	/* offset address of the SDA paraeters */

#define EEP_MAP_AXIS_MASK				0xF000U	/* axis no mask for detect eeprom number */	/* <S0E4> */
#define EEP_MAP_AXIS_SFT				12U		/* EEP_MAP_AXIS_OFFSETのbit長 */			/* <S0E4> */
#define EEP_MAP_PADR_MASK				0x0FFFU	/* physical address mask */					/* <S0E4> */

#define EEP_MAX_QUEUES				256


/* physical address and eeprom number of the Axis data *//* <S0E4> */
#define GET_EEP_MEMORY_NUMBER(adr)		(((adr)&(EEP_MAP_AXIS_MASK)) >> (EEP_MAP_AXIS_SFT))
#define GET_EEP_PHYSICAL_ADDRESS(adr)	((adr)&(EEP_MAP_PADR_MASK))

/* physical address of the each no-checksum data */
//#define EEP_NOSUM_ADDRESS(no, ofs)	(EEP_NOSUM_BLOCK_ADDRESS+(EEP_NOSUM_BLOCK_LENGTH*(no))+(ofs))	/* <S0E4> */
#define EEP_NOSUM_ADDRESS(no, ofs)	(EEP_NOSUM_BLOCK_ADDRESS+(EEP_MAP_AXIS_OFFSET*(no))+(ofs))			/* <S0E4S0E4> */
#define EEP_NOSUMCMN_ADDRESS(ofs)	(EEP_NOSUM_CMN_BLOCK_ADDRESS+(ofs))

#if 0/* Different specifications */											/* <S04E>：SigamV-MD仕様→Sigma7 */
/* Offset address of the no-checksum informations */
enum EEP_NOCHKSUM_OFS {
	EEP_OLINFO_OFS = 0,
	EEP_ALMINFO_OFS = 2,
	EEP_ALMCODE_OFS = 4,
	EEP_ALMTIME_OFS = 24,
	EEP_TIMESTAMP_OFS = 64,
	EEP_ENGINFO_OFS = 68,
	EEP_INIALM_OFS = 84,
};
/* Structure of the no-checksum informations for each Axis */
typedef struct _EEP_NOCHKSUM_VAL_T {
	USHORT	OLInfo;
	USHORT	AlarmInfo;
	USHORT	AlarmCode[10];
	ULONG	AlarmTime[10];
	ULONG	TimeStamp;
	UCHAR	EngInfo[16];
} EEP_NOCHKSUM_VAL_T;
#else
/* Offset address of the no-checksum informations */
enum EEP_NOCHKSUM_OFS {
	EEP_OLINFO_OFS = 0x00,
	EEP_ALMINFO_OFS = 0x02,
	EEP_ALMCODE_OFS = 0x04,
	EEP_ALMTIME_OFS = 0x18,
	EEP_ENGINFO_OFS = 0x40,
	EEP_MOTORSN_OFS = 0x52,	/* <S14B> */
//	TODO:前回接続したﾓｰﾀ種別 = 0x50,	SGDVではどの処理？
//	TODO:前回接続したシリアルNo = 0x52,	SGDVではどの処理？
	EEP_ALMMON_OFS = 0x62,
	EEP_LCDINF_OFS = 0xA2,
//	TODO:アラーム発生時の通信コマンド = 0x110,	ﾊﾟﾗﾒｰﾀ(Pn890～)と処理が重複する。SGDVではどの処理？
//	TODO:アラーム発生時の通信レスポンス = 0x140,	ﾊﾟﾗﾒｰﾀ(Pn8A8～)と処理が重複する。SGDVではどの処理？
};
/* Structure of the no-checksum informations for each Axis */
typedef struct _EEP_NOCHKSUM_VAL_T {
	USHORT	OLInfo;
	USHORT	AlarmInfo;
	USHORT	AlarmCode[10];
	ULONG	AlarmTime[10];
	UCHAR	EngInfo[16];
	USHORT	Dummy;	//TODO
	USHORT	Dummy2;	//TODO
	USHORT	AlarmMon[32];
	USHORT	LcdInfo[55];
	//TODO
} EEP_NOCHKSUM_VAL_T;
#endif											/* <S04E>：SigamV-MD仕様→Sigma7 */

#if 0/* Different specifications */											/* <S04E>：SigamV-MD仕様→Sigma7 */
/* Offset address of the no-checksum informations for the device unit */
enum EEP_NOCHKSUM_CMN_OFS {
	EEP_MACADDR_OFS = 0,	/* Do not change this address, the kernel module reads this value at start-up. */
	EEP_IPADDR_OFS = 8,		/* Do not change this address, the kernel module reads this value at start-up. */
	EEP_GATEWAY_OFS = 12,	/* Do not change this address, the kernel module reads this value at start-up. */
	EEP_NETMASK_OFS = 16,	/* Do not change this address, the kernel module reads this value at start-up. */
	EEP_HOSTNAME_OFS = 20,	/* Do not change this address, the kernel module reads this value at start-up. */

	EEP_SDAWAT_OFS = 52,
	EEP_CNVID_OFS = 68,
	EEP_MBID_OFS = 70,
	EEP_IOID_OFS = 72,
	EEP_CNVOLINFO_OFS = 74,
};
/* Structure of the no-checksum informations for the device unit */
typedef struct _EEP_NOCHKSUM_CMN_T {
	USHORT	MacAddress[4];
    ULONG	IPAddress;
    ULONG	GateWay;
    ULONG	NetMask;
    CHAR	HostName[32];

    USHORT	SdaWatt[8];
	USHORT	ConverterID;
	USHORT	MountBaseID;
	USHORT	IOmoduleID;
	USHORT	CnvOLInfo;
} EEP_NOCHKSUM_CMN_T;
#else
/* Offset address of the no-checksum informations for the device unit */
enum EEP_NOCHKSUM_CMN_OFS {
	EEP_TIMESTAMP_OFS = 0x00,
	EEP_OPTHST_OFS = 0x04,
	EEP_OPTID_OFS = 0x06,
	EEP_BOOTINF_OFS = 0x0C,
	EEP_AVRINTERTMPR_OFS = 0x16,		/* <S154> */
	EEP_RESERVED_CMN_OFS = 0x18,		/* <S154> */
};
/* Structure of the no-checksum informations for the device unit */
typedef struct _EEP_NOCHKSUM_CMN_T {
	ULONG	TimeStamp;
	USHORT	OptHst;
	USHORT	OptID[3];
	USHORT	BootInf[5];
	SHORT	AvrInterTmpr;			/* <S154> */
	USHORT	reserved;/* [20] */		/* <S154> */
} EEP_NOCHKSUM_CMN_T;
#endif											/* <S04E>：SigamV-MD仕様→Sigma7 */

#define EEP_OPTID_SIZE 3	/* In Word(SHORT)	*//* <S04E> */


/* Structure of EEPROM Block headder */
typedef struct _EEP_HEADER_INFO_T {
	USHORT	SizeBlockA;
	USHORT	FormatID;
	ULONG	VendorID;
	ULONG	ProductID;
	USHORT	BoardID;
	USHORT	Data;
	CHAR	ProductName[32];
	CHAR	SerialNumber[16];
	USHORT	Checksum;
	USHORT	SizeBlockB;
} EEP_HEADER_INFO_T;



/* Structure of EEPROM Que buffer */
typedef struct _EEP_QUE_BUFFER {
	UINT			devID;
	USHORT			address;
	USHORT			wrWordSz;
	ULONG			wrValue;
} EEP_QUE_BUFFER;

/* Structure of EEPROM Que handle */
typedef struct _EEP_QUE_HANDLE {
	UINT			maxQueSize;
	UINT			QueIdx;
	UINT			nextQueIdx;
	EEP_QUE_BUFFER	*QueBuffer;
} EEP_QUE_HANDLE;

#if 0  /* 不使用 <S1D2> */
/* Structure of EEPROM Que handle */
typedef struct _EEP_DEVICE_HANDLE {
	void		*pvI2CDevice;	/* Eeprom Device of SDA */
	void		*pvI2CCoADevice;/* Eeprom Device of COA */
	IODevice	pvSPIDevice;	/* Eeprom Device of IF */
} EEP_DEVICE_HANDLE;
#endif

extern const EEP_HEADER_INFO_T EepDefaultInfoValue;
extern const EEP_NOCHKSUM_CMN_T EepCmnDefaultValue;


#ifdef __cplusplus
extern	"C"	{
#endif

/* API's */
LONG EepdevInitialize(void *SdaAsicPtr, void *CoaAsicPtr, RWALLMD *pRwAllMng, RWPDLMD *pRwPdlMng); /* <S031> */

LONG EepdevReadValues(UINT devId, USHORT address, USHORT *rdValue, USHORT rdWordSz);

LONG EepdevWriteNoSumValues(UINT devId, USHORT address, USHORT *wrValue, USHORT wrWordSz);

LONG EepdevWriteParameters(UINT devId, USHORT address, USHORT *wrValue, USHORT wrWordSz);

void EepdevCreateQueue(EEP_QUE_HANDLE *hEepQue, EEP_QUE_BUFFER *QueBuf, UINT MaxQueSize);

LONG EepdevPutQueue(EEP_QUE_HANDLE *hEepQue, UINT devID,
					USHORT address, ULONG wrValue, USHORT wrWordSz);

void EepdevWriteQuedValues(EEP_QUE_HANDLE *hEepQue, UINT maxWriteNum);

LONG EepdevCopyEachAxis( USHORT SrcAxis, USHORT DstAxis ); /* ダウンローダのΣ-7W対応までの暫定 *//* <S0E4> */
//void EepCfg_InitAmpParameters(EEP_DEVICE_HANDLE *hEepDev, USHORT vadr, UINT ampType);
//void EepCfg_InitConvParameters(EEP_DEVICE_HANDLE *hEepDev, USHORT vadr, UINT ampType);
//void EepCfg_InitMBBParameters(EEP_DEVICE_HANDLE *hEepDev, USHORT vadr, UINT mbbType);

LONG KriReadEepromData( USHORT address, USHORT *rdValue, USHORT rdWordSz );			/* <S18F> */
LONG KriWriteEepromData( USHORT address, USHORT *wrValue, USHORT wrWordSz );		/* <S18F> */


#if defined(_VC_DEBUG_) /* 2012.08.22 Y.Oka Make Mercury Base Software */
LONG DbgEepAllRead( void *RdBuffer, USHORT rdSize );
LONG DbgEepReadValues( UCHAR *EepBuf, USHORT address, UCHAR *rdValue, USHORT rdWordSz );
#endif /* _VC_DEBUG_ */ /* 2012.08.22 Y.Oka Make Mercury Base Software */


#ifdef __cplusplus
}
#endif



/***************************************** end of file **********************************************/
#endif //__EEPDEV_API_H

