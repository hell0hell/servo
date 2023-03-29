/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanN.h : Local Header File for ScanN Software												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. ScanN Constant Definition																*/
/*																									*/
/*		2. ScanN Macro    Definition																*/
/*																									*/
/*		3. ScanN Variable Extern Declaration														*/
/*																									*/
/*		4. ScanN Function Prototype Definition														*/
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
#define		IXX_SCANN
#include	"KnlApi.h"								/* Kernel api */
#include 	"Pgm.h"									/* program table */
#include 	"Zone.h"								/* zone table */
#include 	"Jspd.h"								/* jog speed table */
#include 	"Sqi.h"									/* sequence input signal */
#include 	"Sqo.h"									/* sequence output signal */
#include 	"Axctrl.h"								/* motion control */
#include 	"Intprt.h"								/* interpreter */
#include 	"XlsPrmSgdtsIxOpt.h"					/* Option parameter IDs	*/
#include	"AlmMngrIx.h"							/* Indexer alarm manager */
#include	"ReqQueue.h"							/* Indexer request queue */

/****************************************************************************************************/
/*																									*/
/*		ScanN Macro Definitions																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Bit Operation																					*/
/*--------------------------------------------------------------------------------------------------*/
#define IXX_BITSET( bits, bno, istrue ) ((bits) = (istrue) ? ((bits) | (1<<(bno)))  /* bit set */ \
				                        : ((bits) &= ~(1<<(bno)))) 
#define IXX_BITCHK( bits, bno ) ((bits) & (1<<(bno)))                               /* bit check */

/*--------------------------------------------------------------------------------------------------*/
/*	Sv-Option Interface																				*/
/*--------------------------------------------------------------------------------------------------*/
#define IXX_GET_HWPCMD( svif )      ((svif)->CycRsp.Mon4)
#define IXX_GET_HWSIN( svif )       ((svif)->CycRsp.Mon5)
#define IXX_GET_HWAD1( svif )       ((svif)->CycRsp.Mon6)
#define IXX_GET_HWAD2( svif )       ((svif)->CycRsp.Mon7)
#define IXX_GET_PERCLRPOS( svif )   ((svif)->CycRsp.Mon8)
#define IXX_GET_SERVOONCOMP( ninv ) (IXX_BITCHK((ninv)->Seq, SERVO_ON_COMP_BITNO))
/*--------------------------------------------------------------------------------------------------*/
#define IXX_GET_CLR( svif )       (IXX_GET_HWSIN( svif ) & 0x8000)				/* @@ 後で消す */
#define IXX_GET_SEN( svif )       (IXX_GET_HWSIN( svif ) & 0x0080)				/* <S00C> */

/****************************************************************************************************/
/*																									*/
/*		ScanN Common Definitions																	*/
/*																									*/
/****************************************************************************************************/
#define 	INT_MAX (0x7FFFFFFF)					/* 暫定！！ 2147483647 *///<S0C7>表記をHEXで統一
#define 	INT_MIN (0x80000000)					/* 暫定！！-2147483648 *///<S0C7>literal treated as "long long"
#include 	"ServoIf.h"								/* Servo if (暫定！！) */
#include 	"Motion.h"								/* Motion control */
#include	"ScanNiov.h"							/* ScanN variables definition */
/*--------------------------------------------------------------------------------------------------*/
#define		SCANN_DEFTRQ  (0x1000000)               /* Default Torque Limit (maxtrq)                */
/*--------------------------------------------------------------------------------------------------*/
#define		SCANN_N_AXIS	(MAX_AXIS_NUMBER)		/* TODO: Multi Axis	*/

typedef enum enum_LINERCIRCLE
{
	LINERCIRCLE_LINER,				/* 0: Liner coordinate */
	LINERCIRCLE_CIRCLE_SHORT,		/* 1: Circle coordinate ( short ) */
	LINERCIRCLE_CIRCLE_P,			/* 2: Circle coordinate ( positive ) */
	LINERCIRCLE_CIRCLE_N			/* 3: Circle coordinate ( negative ) */
} LINERCIRCLE;

typedef enum enum_HOMEMETHOD
{
	HOMEMETHOD_NOTUSE,
	HOMEMETHOD_DEC_C,
	HOMEMETHOD_DEC,
	HOMEMETHOD_C,
	HOMEMETHOD_PUSH
} HOMEMETHOD;

typedef enum enum_INPOUTSEL			/* <S00C> */
{
	INPOUTSEL_PWIDTH,
	INPOUTSEL_PWIDTH_FILTER,
	INPOUTSEL_PWIDTH_DEN,
} INPOUTSEL;

/****************************************************************************************************/
/*																									*/
/*		ScanN Common Struct Definitions 															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Message payload format 1                                                                         */
/*--------------------------------------------------------------------------------------------------*/
struct _fmt1{
	int arg1;
	int arg2;
	int arg3;
	int arg4;
	void *ptr1;
	void *ptr2;
	void *ptr3;
};

/*--------------------------------------------------------------------------------------------------*/
/*  Message channels                                                                                */
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_CHANNEL
{
	CHANNEL_NULL,        /* 0: NULL */
	CHANNEL_SQI,         /* 1: SQI */
	CHANNEL_ANLG,        /* 2: @@ Anlg */
	CHANNEL_INTPRT,      /* 3: INTPRT */
	CHANNEL_SERCM,       /* 4: SERCM */
	CHANNEL_PCCM,        /* 6: PCCM  */
	CHANNEL_DGOP,        /* 7: DGOP */
	CHANNEL_AXCTRL,      /* 8: AXCTRL */
} _CHANNEL;

/*--------------------------------------------------------------------------------------------------*/
/* Message token                                                                                    */
/*--------------------------------------------------------------------------------------------------*/
struct _token{
	/*----------------------------------------------------------------------------------------------*/
	MSGIF    *Super;		/* Super Class (Message Interface) */
	OPPRMDAT *pPrm;			/* Option Parameters */
	/*----------------------------------------------------------------------------------------------*/
	_CHANNEL Channel;       /* message channel */
	union {
	/*----------------------------------------------------------------------------------------------*/
		struct _fmt1 mes1;  /* SQI/INTPRT: common message format 1 */
	/*----------------------------------------------------------------------------------------------*/
		MT_CMD	CmdData;    /* Axctrl: Motion Command Data Format  */
	/*----------------------------------------------------------------------------------------------*/
	} fmt;
};
typedef struct _token NMSG;

/*--------------------------------------------------------------------------------------------------*/
/* Position token                                                                                   */
/*--------------------------------------------------------------------------------------------------*/
struct _pos {
	struct {
		UCHAR LinearCircle;				/* Linear or Circle Coordinate */
		UCHAR bSotUse;                  /* Use soft limit or not */
		UCHAR bTPosLimEnable;			/* soft limit is enable or not */
		UCHAR Reserve2;
	}f;
/*--------------------------------------------------------------------------------------------------*/
	long Apos;                          /* Current Motor Position [external ref] */
	long Pos;                           /* Current Reference Position [external ref] */
	long Tpos;                          /* Target Position [external ref] */
	long Tdst;                          /* Target Distance [external ref] */
/*--------------------------------------------------------------------------------------------------*/
	long AposRem;                       /* Current Motor Position for Egear */
	long PosRem;                        /* Current Reference Position for Egear */
	long TdstRem;						/* Target Distance Remainder for Egear */
/*--------------------------------------------------------------------------------------------------*/
	long Rpos; 							/* Registration target position [external ref] */
	long Rdst;                          /* Registration target distance [external ref] */
/*--------------------------------------------------------------------------------------------------*/
	long RposRem; 						/* Registration target position Remainder for Egear */
	long RdstRem;                       /* Registration target distance Remainder for Egear */
/*--------------------------------------------------------------------------------------------------*/
	long HomePos;                       /* Incremental encorder -> Origin Position
										 * Absolute encoder -> Origin Position Offset [ref] */
/*--------------------------------------------------------------------------------------------------*/
	long PosMax;                        /* Position Max[ref] */
	long PosMin;                        /* Position Min[ref] */
	long PosWidth;						/* Position Width [ref] */
	long PosLimP;						/* Positive Position Limit [ref] */
	long PosLimN;						/* Negative Position Limit [ref] */
/*--------------------------------------------------------------------------------------------------*/
	long PcCmZSetPos;					/* Pccm */
/*--------------------------------------------------------------------------------------------------*/
	void (*SetPos)(struct _pos *pPos, long pgcnt, long pos, long *dstpos );   /* set position */
	void (*SetDst)(struct _pos *pPos, long tpos, long *tdst );                /* set distance */
};

/****************************************************************************************************/
/*																									*/
/*		ScanN Common Function Prototypes 															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		ScanN.c : ScanN main function 																*/
/*--------------------------------------------------------------------------------------------------*/
void	NpiScanNMain( SV_OPTION_IF *psvif );			     /* ScanN main function					*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxSetOriginPos( struct _pos *pNpos, long homepos ); /* set origin position                 */

/*--------------------------------------------------------------------------------------------------*/
/*		ScanNio.c : ScanN Input and Output 															*/
/*--------------------------------------------------------------------------------------------------*/
void	NpxInputMain( 											/* ScanN inuput main function 		*/
			SV_OPTION_IF *psvif,								/* SV-OPTION Interface				*/
			NINV *pninv, 										/* ScanN input valiables			*/
			struct _pos *pNpos );								/* ScanN position 					*/
void	NpxOutputMain( 											/* ScanN output main function		*/
			SV_OPTION_IF *psvif, 								/* SV-OPTION Interface				*/
			NOUTV *pnoutv );									/* ScanN output valiable			*/
void	NpxInitInOutProc( 										/* ScanN I/O init main function   	*/
			SV_OPTION_IF *psvif,								/* SV-OPTION Interface				*/
			NINV *pninv, 										/* ScanN input valiables			*/
			NOUTV *pnoutv );									/* ScanN output valiables			*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxMakeMonitor( long idx, long rem, USHORT *RegBuf );  	/* set indexer monitor 				*/
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		Define ScanN Indexer Option Function Prototypes 											*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Sqi.c : Sequence Input Signal and statet transition  										*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxSqiMain( 										/* Sqi Main Function 					*/
			SQIV *psqiv, 									/* Sqi Valiable							*/
			JSPDT *pJspdCache, 								/* Jog Speed Table Cache				*/
			struct _token *nmsg );							/* ScanN Message						*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxInitSqi( 										/* Initialize Sqi 						*/
			SQIV *psqiv,  									/* Sqi Variable 						*/
			OPPRMDAT *pPrm );								/* the Parameters						*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxSqiSetLgcs( SQIV *psqiv, OPPRMDAT *pprm );		/* Set sequence input signal logics 	*/ 
void	IxxSqiSetMaps( SQIV *psqiv, OPPRMDAT *pPrm );		/* Set sequence input signal maps	 	*/ 
UINT	IxxSqiGetSigs( void );								/* Get Signals                          */

/*--------------------------------------------------------------------------------------------------*/
/*		Intprt.c : Program Table Interpreter		 												*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_INTPRT_USE == 1
void	IxxIntprtMain( INTPRTV *pInt, 
				PGMT *pPgmCache, 
				struct _token *nmsg,
				struct _pos *pNpos );                       /* Intprt main 							*/
void	IxxInitIntprt( INTPRTV *pInt );						/* Initialize Interpreter				*/
#else
#define IxxIntprtMain( x1, x2, x3, x4 )  {;}
#define	IxxInitIntprt( x1 ) {;}

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Axctrl.c : Indexer Motion Control (which uses M-2 Motion Component) 						*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlMain( MTHNDL *mthdl, struct _token *nmsg );	/* Indexer Motion Control Main 		*/
void	IxxInitAxctrl( AXCTRLV *pax );			                /* Initialize Indexer Motion		*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqFollowUp( MT_CMD *mtcmd );                /* follow up request                  */
void	IxxAxctrlReqSkip( MT_CMD *mtcmd, MT_HOLD_MODE mode ); /* skip request                       */
void	IxxAxctrlReqHold( MT_CMD *mtcmd );                    /* hold request                       */
void	IxxAxctrlReqFeed( MT_CMD *mtcmd,                      /* Feed request                       */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec );                                   /* deceleration [ref/cycle^2]         */
void	IxxAxctrlReqHoming( MT_CMD *mtcmd, long tspeed, MT_LATCH_SGNL sgnl );  /* Homing request    */
void	IxxAxctrlReqZstop( MT_CMD *mtcmd );                   /* Zero stop request                  */
void	IxxAxctrlReqPosing( MT_CMD *mtcmd,                    /* Posing request                     */
				long tdst,                                    /* target position [ref]              */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec );                                   /* deceleration [ref/cycle^2]         */
void	IxxAxctrlReqExPosing( MT_CMD *mtcmd,                  /* Ex Posing request                  */
				long tdst,                                    /* target position [ref]              */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long rdst,                                    /* registration desitination [ref]    */
				long rspd,                                    /* registration speed [ref/s]         */
				MT_LATCH_SGNL sgnl );
void	IxxAxctrlReqExFeed( MT_CMD *mtcmd,                    /* Ex Feed request                    */
				long tspeed,                                  /* target speed [ref/s]               */
				long acc,                                     /* acceleration [ref/cycle^2]         */
				long dec,                                     /* deceleration [ref/cycle^2]         */
				long rdst,                                    /* registration desitination [ref]    */
				long rspd,                                    /* registration speed [ref/s]         */
				MT_LATCH_SGNL sgnl );
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqResume( MT_CMD *mtcmd, long tspd );       /* Resume request                     */
/*--------------------------------------------------------------------------------------------------*/
void	IxxAxctrlReqInterpolate( MT_CMD *mtcmd,               /* @@ Interpolate request             */
				long tdst,                                    /* target distance [ref]              */
				long vff,                                     /* velocity feed forwared[2^24/OS]    */
				long tff );                                   /* torque feed forwared[2^24/MaxTrq]  */
void	IxxAxctrlReqVelctrl( MT_CMD *cmdData,                 /* @@ Velctrl request                 */
				long tvelo,                                   /* target velocity [2^24/OS]          */
				long tff );                                   /* torque feed forwared[2^24/MaxTrq]	*/
/*--------------------------------------------------------------------------------------------------*/
/*		Sqo.c : Sequence Output Signal  															*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxSqoMain( SQOV *psqov, ZONET *pZoneCache,     /* Make sequence output signal 				*/
		struct _pos *pNpos );
void	IxxInitSqo( SQOV *psqov, OPPRMDAT *pprm );		/* Initialize Sqo 							*/
void	IxxSqoSetLgcs( SQOV *psqov, OPPRMDAT *pprm );	/* Set sequence Output signal logics 		*/ 

/*--------------------------------------------------------------------------------------------------*/
/*		Pccm.c : SC Message Manager  																*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxMsgManager( void );					/* Indexer SC Message Manager						*/
void	IxxInitMsg( void );						/* Initialize Indexer SC Message Manager			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Disp.c : Create Display 	 																*/
/*--------------------------------------------------------------------------------------------------*/
void	IxxDispMain( void );					/* Indexer Disp Main								*/
void	IxxInitDisp( void );					/* Initialize Disp 									*/

/*--------------------------------------------------------------------------------------------------*/
/*		Pgm.c : Program table reading/writing/loading 												*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_PGM_USE == 1
void	IxxInitPgm( PGMV *ppgmv );			                /* Initialize PGM						*/
UINT	IxxPgmInitLoad( PGMT *pPgmCache );		            /* Initial load for PGM					*/
void	IxxPgmSequence( PGMV *ppgmv, PGMT *pPgmCache );		/* Program Table Store/Init sequence    */
/*--------------------------------------------------------------------------------------------------*/
void	IxxPgmGetPart( PGMT *pPgmCache, long idx, long num, USHORT *RegBuf );     /* get Pgm part   */
long	IxxPgmSetPart( PGMT *pPgmCache, long idx, long num, USHORT *RegBuf );     /* set Pgm part   */
/*--------------------------------------------------------------------------------------------------*/
#else
#define	IxxInitPgm( x1 ) { ;}		                        /* Initialize PGM						*/
#define	IxxPgmInitLoad( x1 ) (0)	                        /* Initial load for PGM					*/
#define IxxPgmSequence( x1, x2 )  { ;}                      /* Program Table Store/Init sequence    */

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Zone.c : Zone table reading/writing/loading 												*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_ZONE_USE == 1
void	IxxInitZone( ZONEV *pzonev );			            /* Initialize Zone						*/
UINT	IxxZoneInitLoad( ZONET *pZoneCache );	            /* Initial load for Zone Table			*/
void	IxxZoneSequence( ZONEV *pZone, ZONET *pZoneCache ); /* Zone Table Store/Init Sequence       */
int		IxxZoneGetIdx( long pos, ZONET *pZoneCache );       /* get zone index                       */
/*--------------------------------------------------------------------------------------------------*/
void	IxxZoneGetPart( ZONET *pZoneCache, long idx, long num, USHORT *RegBuf ); /* get zone part   */
long	IxxZoneSetPart( ZONET *pZoneCache, long idx, long num, USHORT *RegBuf ); /* set zone part   */
/*--------------------------------------------------------------------------------------------------*/
#else
#define	IxxInitZone( x1 ) { ;}			                    /* Initialize Zone						*/
#define IxxZoneInitLoad( x1 ) (0)	                        /* Initial load for Zone Table			*/
#define	IxxZoneSequence( x1, x2  ) { ;}                     /* Zone Table Store/Init Sequence       */
#define IxxZoneGetIdx( x1, x2 )    (0)                      /* get zone index                       */

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Jspd.c : Jog speed table reading/writing/loading 											*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_JSPD_USE == 1
void	IxxInitJspd( JSPDV *pjspv );			             /* Initialize Jspd						*/
UINT	IxxJspdInitLoad( JSPDT *pJspdCache );	             /* Initial load for Jspd Table			*/
void	IxxJspdSequence( JSPDV *pjspv, JSPDT *pJspdCache );  /* Jspd Table Store/Init sequence      */
/*--------------------------------------------------------------------------------------------------*/
void	IxxJspdGetPart( JSPDT *pJspdCache, long Idx, long num, USHORT *RegBuf ); /* get jspd part   */
long	IxxJspdSetPart( JSPDT *pJspdCache, long Idx, long num, USHORT *RegBuf ); /* set jspd part   */
/*--------------------------------------------------------------------------------------------------*/
#else
#define	IxxInitJspd( x1 )	{ ;}				             /* Initialize Jspd						*/
#define	IxxJspdInitLoad( x1 ) (0)				             /* Initial load for Jspd Table			*/
#define	IxxJspdSequence( x1, x2 )  { ;}                      /* Jspd Table Store/Init sequence      */
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		AlmMngrIx.c : Indexer Alarm Manager															*/
/*--------------------------------------------------------------------------------------------------*/
#if 	SVFSEL_INDEXER_ALMMNGR == 1
void	IxxInitAlmMngr( OPALMV *pOpAlm );	/* Initialize Indexer Alarm Manager 					*/
void	IxxAlmManager( OPALMV *pOpAlm );	/* Indexer Alarm Manager Main Function          		*/
#else
#define IxxInitAlmMngr( pOpAlm );
#define IxxAlmManager( pOpAlm );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		ReqQueue.c : Indexer Request Queue															*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_REQEUES_USE == 1
void NpiInitReqQueues( void );					/* Initialize Request Queues						*/
REQID NpxGetReq( 								/* Get Request (return: Request Queue ID) 			*/
				REQQUE* pReqQ, 					/* Request Queue Pointer							*/
				REQ *pDst, 						/* Request Value Pointer							*/
				UINT *pArg );					/* Argument Value Pointer							*/
void NpxSetAns( 								/* Set Answer 										*/
				REQQUE *pReqQ,  				/* Request Queue Pointer							*/
				REQID ReqID, 					/* Request Queue ID									*/
				ANS Ans );						/* Answer 											*/
REQID NpiSetReq( 								/* Set Request (return: Request Queue ID)			*/
				REQQUE *pReqQ, 					/* Request Queue Pointer							*/
				REQ Req, 						/* Request											*/
				UINT Arg );						/* Argument											*/
ANS NpiGetAns( 									/* Get Answer from Request Queue					*/
				REQQUE *pReqQ,					/* Request Queue Pointer							*/
				REQID ReqID );					/* Request Queue ID									*/
#else
#define NpiInitReqQueues( ) {;}
#endif
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		Anlg.c : Analog Command Input Module														*/
/*--------------------------------------------------------------------------------------------------*/
#include "Anlg.h"

extern struct _pos Npos;                    /* ScanN position                                       */
extern MSGIF NetMsg;	                    /* TODO: 後で消す */


/*--------------------------------------------------------------------------------------------------*/
typedef struct _ANLG_AXIS_RSC_ 					/* <S00C>											*/
{
/*--------------------------------------------------------------------------------------------------*/
	NINV	NinV;								/* ScanN Input  Variables							*/
	NOUTV	NoutV;								/* ScanN Output Variables							*/
	struct _token Nmsg;                         /* ScanN message token (Motion)                     */
	struct _pos Npos;                           /* ScanN position                                   */
	OPPRMDAT OpPrm;								/* Option Parameter									*/
/*--------------------------------------------------------------------------------------------------*/
	SQIV	SqiV;								/* SQI variable 									*/
	SQOV	SqoV;								/* SQO variable										*/
	AXCTRLV AxctrlV;							/* Axctrl variable                                  */
	ANLGV	AnlgV;								/* Anlg variable									*/
/*--------------------------------------------------------------------------------------------------*/
	MTHNDL	MtHdl;								/* Motion handle i									*/
} ANLG_AXIS_RSC_T;

