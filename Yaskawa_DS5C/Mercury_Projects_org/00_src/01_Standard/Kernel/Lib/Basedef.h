/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Basedef.h : Basic Data Type Definition Header File											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Basic Data Type Definition																*/
/*																									*/
/*		2. Basic Constant Definition																*/
/*																									*/
/*		3. Basic Macro    Definition																*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2002.10.25  T.Taniguchi															*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef	_BASEDEF_H 
#define	_BASEDEF_H 

/*--------------------------------------------------------------------------------------------------*/
/*		Configurations of the build environment														*/
/*--------------------------------------------------------------------------------------------------*/
#if defined( __PowerPC )
#ifndef __BIG_ENDIAN__
#define __BIG_ENDIAN__
#endif
#elif defined( WIN32 )
#define	_VC_DEBUG_	
#endif

/****************************************************************************************************/
/*																									*/
/*		Basic Constant Definition					<S00A>										*/
/*																									*/
/****************************************************************************************************/
#define		OK					0
#define		NG					-1						/* floader.hの定義と合わせる				*/
/*--------------------------------------------------------------------------------------------------*/
#define		TRUE				1
#define		FALSE				0
/*--------------------------------------------------------------------------------------------------*/
#define		PAI				3.141592654f				/* π										*/
//#define		INTGRAL_NORMAXVAL	100.0				/* 正規化値									*/
#define		INTGRAL_NORMAXVAL	0x02000000				/* 正規化値									*/
/*--------------------------------------------------------------------------------------------------*/
#define		LONG_MAX			2147483647				/* Max Value of Signed LONG 				*//* <S020> */

/****************************************************************************************************/
/*																									*/
/*		Basic Data Type Definition																	*/
/*																									*/
/****************************************************************************************************/
#ifndef	_VC_DEBUG_ 	/* 2012.08.24 Y.Oka Make Mercury Base Software */
typedef signed char				CHAR;
typedef unsigned char			UCHAR;
typedef short					SHORT;
typedef unsigned short			USHORT;
typedef long					LONG;
typedef unsigned long			ULONG;
typedef int						INT;
typedef unsigned int			UINT;
/*--------------------------------------------------------------------------------------------------*/
typedef volatile CHAR			VCHAR;
typedef volatile UCHAR			VUCHAR;
typedef volatile SHORT			VSHORT;
typedef volatile USHORT			VUSHORT;
typedef volatile LONG			VLONG;
typedef volatile ULONG			VULONG;
typedef volatile INT			VINT;
typedef volatile UINT			VUINT;
/*--------------------------------------------------------------------------------------------------*/
//typedef enum BOOL{ FALSE=0,TRUE=1}	BOOL;	
typedef int						BOOL;	
typedef signed char				INT8;
typedef unsigned char			UINT8;
typedef short					INT16;
typedef unsigned short			UINT16;
typedef long					INT32;	
typedef unsigned long			UINT32;	
typedef long long				INT64;	
typedef unsigned long long		UINT64;	
/*--------------------------------------------------------------------------------------------------*/
typedef volatile BOOL			VBOOL;	
typedef volatile INT8			VINT8;
typedef volatile UINT8			VUINT8;
typedef volatile INT16			VINT16;
typedef volatile UINT16			VUINT16;
typedef volatile INT32			VINT32;	
typedef volatile UINT32			VUINT32;	
typedef volatile INT64			VINT64;	
typedef volatile UINT64			VUINT64;	
/*--------------------------------------------------------------------------------------------------*/
typedef	const UCHAR				CUCHAR;
typedef	const USHORT			CUSHORT;
typedef	const ULONG				CULONG;
/*--------------------------------------------------------------------------------------------------*/
typedef	long					KMGAIN;				/* {k,s} Type for Mulgain()						*/
//typedef	long					KSGAIN;				/* {k,s} Type for PcalKsksks()					*//*<S00A>*/
/*--------------------------------------------------------------------------------------------------*/
#define VOID					void

#else	/* _VC_DEBUG_ */	/* 2012.08.24 Y.Oka Make Mercury Base Software */

typedef char 					CHAR;
typedef unsigned char			UCHAR;
typedef short					SHORT;
typedef unsigned short			USHORT;
typedef long					LONG;
typedef unsigned long			ULONG;
typedef int						INT;
typedef unsigned int			UINT;
/*--------------------------------------------------------------------------------------------------*/
typedef volatile CHAR			VCHAR;
typedef volatile UCHAR			VUCHAR;
typedef volatile SHORT			VSHORT;
typedef volatile USHORT			VUSHORT;
typedef volatile LONG			VLONG;
typedef volatile ULONG			VULONG;
typedef volatile INT			VINT;
typedef volatile UINT			VUINT;
/*--------------------------------------------------------------------------------------------------*/
//typedef enum BOOL{ FALSE=0,TRUE=1}	BOOL;	
typedef int						BOOL;	
typedef signed char				INT8;
typedef unsigned char			UINT8;
typedef short					INT16;
typedef unsigned short			UINT16;
typedef signed int				INT32;	
typedef unsigned int			UINT32;	
typedef long long				INT64;	
typedef unsigned long long		UINT64;	
/*--------------------------------------------------------------------------------------------------*/
typedef volatile BOOL			VBOOL;	
typedef volatile INT8			VINT8;
typedef volatile UINT8			VUINT8;
typedef volatile INT16			VINT16;
typedef volatile UINT16			VUINT16;
typedef volatile INT32			VINT32;	
typedef volatile UINT32			VUINT32;	
typedef volatile INT64			VINT64;	
typedef volatile UINT64			VUINT64;	
/*--------------------------------------------------------------------------------------------------*/
typedef	const UCHAR				CUCHAR;
typedef	const USHORT			CUSHORT;
typedef	const ULONG				CULONG;
/*--------------------------------------------------------------------------------------------------*/
//typedef	long					KMGAIN;				/* {k,s} Type for Mulgain()						*//*<S00A>*/
//typedef	long					KSGAIN;				/* {k,s} Type for PcalKsksks()					*//*<S00A>*/
/*--------------------------------------------------------------------------------------------------*/
#define VOID					void	

#endif	/* _VC_DEBUG_ */	/* 2012.08.24 Y.Oka Make Mercury Base Software */
/*--------------------------------------------------------------------------------------------------*/
/*		float使用/不使用 スイッチ	<S00A>																*/
/*--------------------------------------------------------------------------------------------------*/
#define	    FLOAT_USE			FALSE /* 擬似浮動小数点処理  TRUE=有効 / FALSE=無効		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
typedef	float					KSGAIN;				/* Floating Data								*/
#endif
#if (FLOAT_USE==FALSE)
typedef	LONG					KSGAIN;				/* {k,s} Type Floating Data						*/
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/


#ifndef		NULL
#define		NULL			((void *)0)
#endif


/****************************************************************************************************/
/*																									*/
/*		Basic Structure type																		*/
/*																									*/
/****************************************************************************************************/
#ifdef __BIG_ENDIAN__
/*--------------------------------------------------------------------------------------------------*/
/*		Double Byte Data Type Definition															*/
/*--------------------------------------------------------------------------------------------------*/
typedef union DBYTEX
{
	USHORT	w;										/* Word											*/
	struct { 
		UCHAR h; 
		UCHAR l;
	} b;											/* Low Byte / High Byte							*/
} DBYTEX;

/*--------------------------------------------------------------------------------------------------*/
/*		Double Word Data Type Definition															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union DWORDX
{
	ULONG	dw;										/* Double Word									*/
	LONG	Long;									/* signed   long								*/
	ULONG	Ulong;									/* unsigned long								*/
	struct {
		USHORT h; 
		USHORT l;
	} w;											/* Low Word / High Word							*/
	struct { 
		UCHAR hh; 
		UCHAR hl; 
		UCHAR lh; 
		UCHAR ll;
	} b;											/* Low Byte / High Byte					*/
} DWORDX;

/*--------------------------------------------------------------------------------------------------*/
/*		８ビット構成データ用構造体																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union UDWORD
{
/*--------------------------------------------------------------------------------------------------*/
	ULONG	w;										/* Double Word									*/
/*--------------------------------------------------------------------------------------------------*/
	struct {
		USHORT h; 
		USHORT l;
	} hw;											/* Low Word / High Word							*/
/*--------------------------------------------------------------------------------------------------*/
    struct { 
		UCHAR d[2]; 
		UCHAR h; 
		UCHAR l;
	} b;											/* Low Byte / High Byte							*/
/*--------------------------------------------------------------------------------------------------*/
} UDWORD;


#else  /* Little Endian */

/*--------------------------------------------------------------------------------------------------*/
/*		Double Byte Data Type Definition															*/
/*--------------------------------------------------------------------------------------------------*/
typedef union DBYTEX
{
    USHORT	w;										/* Word											*/
    struct { 
		UCHAR l; 
		UCHAR h;
	} b;											/* Low Byte / High Byte							*/
} DBYTEX;

/*--------------------------------------------------------------------------------------------------*/
/*		Double Word Data Type Definition															*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union DWORDX
{
	ULONG	dw;										/* Double Word									*/
	LONG	Long;									/* signed   long								*/
	ULONG	Ulong;									/* unsigned long								*/
	struct {
		USHORT l; 
		USHORT h;
	} w;											/* Low Word / High Word							*/
    struct {
		UCHAR ll; 
		UCHAR lh; 
		UCHAR hl; 
		UCHAR hh;
	} b;											/* Low Byte / High Byte							*/
} DWORDX;

/*--------------------------------------------------------------------------------------------------*/
/*		８ビット構成データ用構造体																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	union UDWORD
{
	ULONG	w;										/* Double Word									*/
	struct { 
		USHORT l; 
		USHORT h;
	} hw;											/* Low Word / High Word							*/
    struct { 
		UCHAR l; 
		UCHAR h;
	} b;											/* Low Byte / High Byte							*/
} UDWORD;

#endif /* __BIG_ENDIAN__ */



/****************************************************************************************************/
/*		Basic Structure type for Micro Program														*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Data Type Definition																		*/
/****************************************************************************************************/
//typedef __int64 INT64;							/* 64bit Integer									*/
/*--------------------------------------------------------------------------------------------------*/
typedef union {									/* Double Word Register								*/
		INT64	dl;								/* 64bit Register									*/
		LONG	l[2];							/* Long Register									*/
		ULONG	ul[2];							/* Long Register									*/
		SHORT	s[4];							/* Short Register									*/
		USHORT	us[4];							/* Short Register									*/
} DLREG;
/*--------------------------------------------------------------------------------------------------*/
typedef union {									/* Double Word Register								*/
		LONG	l;								/* Long Register									*/
		ULONG	ul;								/* Long Register									*/
		SHORT	s[2];							/* Short Register									*/
		USHORT	us[2];							/* Short Register									*/
} DWREG;
/*--------------------------------------------------------------------------------------------------*/
typedef struct {								/* Constant Data Register							*/
		DWREG	Zero;							/* Zero(0) Register									*/
		DWREG	PlusOne;						/* PlusOne(1) Register								*/
		DWREG	MinusOne;						/* MinusOne(-1) Register							*/
} CREG;
/*--------------------------------------------------------------------------------------------------*/
typedef	const SHORT CSHORT;
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Basic Constant Definition																	*/
/*																									*/
/****************************************************************************************************/
#define		OK				 0
#define		NG				 -1							/* floader.hの定義と合わせる				*/
/*--------------------------------------------------------------------------------------------------*/
#define		TRUE			 1
#define		FALSE			 0
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		Basic Macro Definition																		*/
/*																									*/
/****************************************************************************************************/
#define	LONGOF( l, h )	((LONG)((USHORT)(l) + ((SHORT)(h) << 16)))
#define	ULONGOF( l, h )	((ULONG)((USHORT)(l) + ((ULONG)(h) << 16)))



/****************************************************************************************************/
/*																									*/
/*			Bit Data Definition																		*/
/*																									*/
/****************************************************************************************************/
#define	BIT0	0x0001
#define	BIT1	0x0002
#define	BIT2	0x0004
#define	BIT3	0x0008
#define	BIT4	0x0010
#define	BIT5	0x0020
#define	BIT6	0x0040
#define	BIT7	0x0080
#define	BIT8	0x0100
#define	BIT9	0x0200
#define	BIT10	0x0400
#define	BIT11	0x0800
#define	BIT12	0x1000
#define	BIT13	0x2000
#define	BIT14	0x4000
#define	BIT15	0x8000

/* --> <USB-FUNCTION> 2013.05.13 K.Sakamoto */
#define	BIT16	0x00010000
#define	BIT17	0x00020000
#define	BIT18	0x00040000
#define	BIT19	0x00080000
#define	BIT20	0x00100000
#define	BIT21	0x00200000
#define	BIT22	0x00400000
#define	BIT23	0x00800000
#define	BIT24	0x01000000
#define	BIT25	0x02000000
#define	BIT26	0x04000000
#define	BIT27	0x08000000
#define	BIT28	0x10000000
#define	BIT29	0x20000000
#define	BIT30	0x40000000
#define	BIT31	0x80000000
/* <-- <USB-FUNCTION> 2013.05.13 K.Sakamoto */


#ifndef _MERCURY_PLATFORM_	/* 暗号化フォーマット対応 <S146> */
#include "SystemSetting.h"
//#include "SystemError.h"
#endif


#endif
/***************************************** end of file **********************************************/
