/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Sqo.h : Sequence output signal header file													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	Function : 																						*/
/*		1. Translate Sequence Output Signal Logics													*/
/*		2. Create Output Signal (POUT,ZONE,INPOS)													*/
/*																									*/
/*																									*/
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
/*																									*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		SQO Definitions 																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		SQO Type Definitions 																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  SQO signals                                                                                     */
/*--------------------------------------------------------------------------------------------------*/
typedef UINT SQO_SIGS;
#define SQO_SIGS_INPOS	(1<<0)    /* bit0: /INPOS */
#define SQO_SIGS_POUT0	(1<<1)    /* bit1: /POUT0 */
#define SQO_SIGS_POUT1	(1<<2)    /* bit2: /POUT1 */
#define SQO_SIGS_POUT2	(1<<3)    /* bit3: /POUT2 */
/*--------------------------------------------------------------------------------------------------*/
#define SQO_SIGS_POUT3	(1<<4)    /* bit4: /POUT3 */
#define SQO_SIGS_POUT4	(1<<5)    /* bit5: /POUT4 */
#define SQO_SIGS_POUT5	(1<<6)    /* bit6: /POUT5 */
#define SQO_SIGS_POUT6	(1<<7)    /* bit7: /POUT6 */
/*--------------------------------------------------------------------------------------------------*/
#define SQO_SIGS_POUT7	(1<<8)    /* bit8: /POUT7 */
#define SQO_SIGS_WRN	(1<<9)    /* bit9: /WRN */
#define SQO_SIGS_BK		(1<<10)   /* bit10: /BK */
#define SQO_SIGS_SRDY	(1<<11)   /* bit11: /SRDY */
/*--------------------------------------------------------------------------------------------------*/
#define SQO_SIGS_CLT	(1<<12)   /* bit12: /CLT */
#define SQO_SIGS_TGON	(1<<13)   /* bit13: /TGON */
#define SQO_SIGS_NEAR	(1<<14)   /* bit14: /NEAR */
#define SQO_SIGS_VCMP	(1<<15)   /* bit14: /VCMP */
/*--------------------------------------------------------------------------------------------------*/
#define SQO_SIGS_BUSY	(1<<16)   /* bit16: @@ Anlg /BUSY */
#define SQO_SIGS_POSRDY	(1<<17)   /* bit17: @@ Anlg /POSRDY */
#define SQO_SIGS_PCO	(1<<18)   /* bit18: @@ Anlg /PCO */
/*--------------------------------------------------------------------------------------------------*/
#define SQO_SIGS_VLT	(1<<18)   /* bit19: @@ Anlg /VLT */
#define SQO_SIGS_PSELA	(1<<19)   /* bit20: @@ Anlg /PSELA */
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		SQO Signal Logic									  										*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_SQO_LGC
{
	SQO_LGC_DEFAULT,      /* 0: default */
	SQO_LGC_OPPOSITE,     /* 1: opposite */
	SQO_LGC_NONACTIVE,    /* 2: alwais non active */
} SQO_LGC;

/*--------------------------------------------------------------------------------------------------*/
/*  Programable output settings                                                                     */
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_SQO_POUT
{
	SQO_POUT_NONACTIVE,
	SQO_POUT_ACTIVE,
	SQO_POUT_ZONE,
	SQO_POUT_PREVIOUS
} SQO_POUT;

/*--------------------------------------------------------------------------------------------------*/
/*		SQO Variable																			 	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _sqov {
/*--------------------------------------------------------------------------------------------------*/
	POUTS Pouts;                        /* Programable output signals */
/*--------------------------------------------------------------------------------------------------*/
	struct _sqo_map {
		UINT	InPos      :4;			/* /INPOS */
		UINT	Pout0      :4;			/* /POUT0 */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pout1      :4;			/* /POUT1 */
		UINT	Pout2      :4;			/* /POUT2 */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pout3      :4;			/* /POUT3 */
		UINT	Pout4      :4;			/* /POUT4 */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pout5      :4;			/* /POUT5 */
		UINT	Pout6      :4;			/* /POUT6 */
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pout7      :4;			/* /POUT6 */
		UINT	Wrn        :4;			/* /WARN */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Bk         :4;			/* /BK */
		UINT	Srdy       :4;			/* /S-RDY */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Clt        :4;			/* /CLT */
		UINT	Tgon       :4;			/* /TGON */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Near       :4;			/* /NEAR */
		UINT	Vcmp       :4;			/* /VCMP */
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Busy       :4;			/* @@ Anlg /BUSY */
		UINT	Posrdy     :4;			/* @@ Anlg /POSRDY */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pco        :4;			/* @@ Anlg PCO */
		UINT	Vlt        :4;			/* @@ Anlg /VLT    */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Psela      :4;			/* @@ Anlg /PSELA */
		UINT	Reserved   :4;			/* @@ Anlg /Reserved    */
	} Map;
/*--------------------------------------------------------------------------------------------------*/
	UCHAR ZoneIdx;						/* Zone table index */
	UCHAR Reserve01;				
	UCHAR Reserve02;			
	UCHAR Reserve03;
/*--------------------------------------------------------------------------------------------------*/
	SQO_SIGS 	Sigs;					/* Output Signals */
	SQO_SIGS 	RegSigs;				/* Output Signals for Register I/F */
	SQO_SIGS 	ActvLgcs;				/* Active Logics */
	SQO_SIGS 	NonActvMask;			/* None Active Sginal Mask */
/*--------------------------------------------------------------------------------------------------*/
	UINT	(*MakeInpos)( LONG, LONG ); /* Make Inpos Signal <S00C>									*/
/*--------------------------------------------------------------------------------------------------*/
} SQOV;


#ifdef	__ghs__
#pragma ghs startzda
#endif
extern SQOV	SqoV;								/* SQO variable										*/
#ifdef	__ghs__
#pragma ghs endzda
#endif
