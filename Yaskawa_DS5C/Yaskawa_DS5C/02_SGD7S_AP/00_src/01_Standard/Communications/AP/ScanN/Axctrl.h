/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Axctrl.h : Indexer Motion Control header file												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		1. Motion Control		 																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.08.28  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		Axctrl Definitions 																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Axctrl Type Definitions 																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Axctrl Sv Bit Signals																	 	*/
/*--------------------------------------------------------------------------------------------------*/
typedef UINT AXCTRL_SVBITS;
#define AXCTRL_SVBITS_SVHANDSHAKEMISS	(0x00800000)
#define AXCTRL_SVBITS_SVWDCMISS			(0x00400000)
#define AXCTRL_SVBITS_REGDISTSHORT		(0x00200000)
#define AXCTRL_SVBITS_SVALM				(0x00100000)
#define AXCTRL_SVBITS_SVWRN				(0x00080000)
#define AXCTRL_SVBITS_NEAR				(0x00040000)
#define AXCTRL_SVBITS_INPOS				(0x00020000)
#define AXCTRL_SVBITS_DEN				(0x00010000)
#define AXCTRL_SVBITS_SVDEN				(0x00008000)		
#define AXCTRL_SVBITS_SAFETY			(0x00000100)
#define AXCTRL_SVBITS_TGON				(0x00000080)
#define AXCTRL_SVBITS_MAIN				(0x00000040)
#define AXCTRL_SVBITS_POT				(0x00000020)
#define AXCTRL_SVBITS_NOT				(0x00000010)
#define AXCTRL_SVBITS_PLS				(0x00000008)
#define AXCTRL_SVBITS_NLS				(0x00000004)
#define AXCTRL_SVBITS_HOMEDONE			(0x00000002)
#define AXCTRL_SVBITS_BB				(0x00000001)

/*--------------------------------------------------------------------------------------------------*/
/*		Axctrl Variable																			 	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _axctrlv {
/*--------------------------------------------------------------------------------------------------*/
	UCHAR Reserve0;						/* Reserved */
	UCHAR Reserve1;						/* Reserved */
	UCHAR Reserve2;						/* Reserved */
	UCHAR Reserve3;						/* Reserved */
/*--------------------------------------------------------------------------------------------------*/
//	long  InPosWidth;					/* /INPOS width [ref] */
} AXCTRLV;

#ifdef	__ghs__
#pragma ghs startzda
#endif
extern AXCTRLV AxctrlV;
#ifdef	__ghs__
#pragma ghs endzda
#endif
