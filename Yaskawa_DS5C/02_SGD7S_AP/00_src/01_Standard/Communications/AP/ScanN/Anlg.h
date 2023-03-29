/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Anlg.h : Analog Reference Input Module Header File											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Anlg Variable Definition																	*/
/*		2. Anlg Macro Definition																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.05.23  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Anlg Definitions																			*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------
 * Anlg Module Reqeust 
 * NOP: 
 * 		do nothing. (Motion:Skip)
 *
 * VELCTRL: 
 *      Velocity Control
 *      input: arg2( analog A/D value )
 *      Motion: Velctrl
 *
 * TRQCTRL: 
 *      Torque Control 
 *      input: arg3( analog A/D value )
 *      Motion:TrqCtrl
 *
 * INTERPOL:
 *      Position Control 
 *      input: arg4 ( analog pulse counter )
 *      Motion:Interpolate
 *
 * INTVELCTRL: 
 *      Velocity Control
 *      input: arg2( Internal Velocity value:Pn301, Pn302, Pn303 )
 *      Motion: Velctrl
 
 *--------------------------------------------------------------------------------------------------*/
enum enum_ANLG_REQ
{
	ANLG_REQ_NOP,					/* do nothing */
	ANLG_REQ_VELCTRL,				/* Analog Voltage Input (VELCTRL) */
	ANLG_REQ_TRQCTRL,				/* Analog Voltage Input (TRQCTRL) */
	ANLG_REQ_INTERPOL,				/* Analog Pulse Input (INTERPOLATE) */
	ANLG_REQ_INTEVELCTRL,			/* Analog Internal Velocity Input (VELCTRL) */	/* <S03D> */
};

/*--------------------------------------------------------------------------------------------------
 * Anlg Module Request Set Macro
 *
 *--------------------------------------------------------------------------------------------------
 *  -Anlg Module N-Message Format (fmt1)
 *     arg1: request (see below)
 *     arg2: Analog Voltage A/D value(for velctrl)
 *     arg3: Analog Voltage A/D value(for trqctrl)
 *     arg4: Analog Pulse Counter(for interpolate)
 *
 *--------------------------------------------------------------------------------------------------*/
#define IXX_ANLG_REQX( req, ad1, ad2, pcntr, nmsg ) {         \
		(nmsg)->Channel = CHANNEL_ANLG;  /* update channel */ \
		(nmsg)->fmt.mes1.arg1 = (req);   /* request */        \
		(nmsg)->fmt.mes1.arg2 = (ad1);   /* AD1 */            \
		(nmsg)->fmt.mes1.arg3 = (ad2);   /* AD2 */            \
		(nmsg)->fmt.mes1.arg4 = (pcntr); /* pulse counter */  \
	}                                                         \

/****************************************************************************************************/
/*																									*/
/*		Macro Definitions																			*/
/*																									*/
/****************************************************************************************************/
typedef struct _AnlgV {							/* Analog Input Valiable							*/
	LONG LastPcmdCntr;							/* Last Input Pulse Counter							*/
	LONG PcmdRem;								/* Pulse Counter Remainder							*/
/*--------------------------------------------------------------------------------------------------*/
	KSGAIN VrefKx;								/* Vref A/D Gain [A/D -> 2^24/OVRSPD]				*/
	KSGAIN TrefKxP;								/* Positive Tref A/D Gain [A/D -> 2^24/MAXTRQ] 		*/
	KSGAIN TrefKxN;								/* Negative Tref A/D Gain [A/D -> 2^24/MAXTRQ] 		*/
	SHORT MulFrq;								/* Pn218:  											*/
	SHORT Reserved;								/* Reserve 											*/
/*--------------------------------------------------------------------------------------------------*/
	struct {									/* Analog Input Flags								*/
		UCHAR	VrefSpdFF;						/* use vrefad as velocity feed forward 				*/
		UCHAR	TrefTrqFF;						/* use trefad as torque feed forward 				*/
		UCHAR	Reserved01;						/* reserve 											*/
		UCHAR	Reserved02;						/* reserve											*/
	}f;
/*--------------------------------------------------------------------------------------------------*/
	LONG Ktreflpf;								/* TREF lopass filter gain							*/
	LONG TrqRefAd;								/* Torque Reference [2^24/MaxTrq]					*/
	LONG Kvreflpf;								/* VREF lopass filter gain							*/
	LONG SpdRefAd;								/* Velocity Reference [2^24/OVRSPD]					*/
/*--------------------------------------------------------------------------------------------------*/
	LONG Tlim;									/* Torque limit [2^24/MaxTrq]						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	InterSpdLevel[4];					/* Internal Velocity Level [2^24/OvrSpd]			*/	/* <S03D> */
} ANLGV;


/****************************************************************************************************/
/*																									*/
/*		Anlg Function Prototypes																	*/
/*																									*/
/****************************************************************************************************/
void	IxxAnlgMain( 									/* Anlg Main Function						*/
				ANLGV* pAnlg,							/* Analog variables							*/
				struct _token *nmsg );					/* Message token							*/
void	IxxInitAnlg( ANLGV *pAnlg );					/* Initialize Analog 						*/
/*--------------------------------------------------------------------------------------------------*/

extern ANLGV AnlgV;										/*											*/
