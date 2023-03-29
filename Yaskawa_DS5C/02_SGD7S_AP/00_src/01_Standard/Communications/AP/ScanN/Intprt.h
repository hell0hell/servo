/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Intprt.h : Indexer Program Table Interpreter header file									*/
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
/*		Intprt Definitions 																			*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Intprt Type Definitions 																	*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Interpreter Request Types                                                                        */
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_INTPRT_REQ
{
	INTPRT_REQ_NOP,
	INTPRT_REQ_PAUSE,
	INTPRT_REQ_RUN,
	INTPRT_REQ_CANCEL
} INTPRT_REQ;

/*--------------------------------------------------------------------------------------------------*/
/* Interpreter State                                                                                */
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_INTPRT_STATE
{
	INTPRT_STATE_CANCEL,            /* 0: cancel state */
	INTPRT_STATE_RUN,               /* 1: run state */
	INTPRT_STATE_LOOP,              /* 2: loop state */
	INTPRT_STATE_MOV,               /* 3: move state */
//	INTPRT_STATE_MOV_BUSY,          /* 4: move busy state */
	INTPRT_STATE_EVFLG,             /* 5: checking event flag state */
	INTPRT_STATE_EVTIME,            /* 6: checking event time state */
	INTPRT_STATE_LOOP_END,          /* 7: loop end state */
//	INTPRT_STATE_MOV_SKIP,          /* 8: move skip state */
	INTPRT_STATE_MOV_STANDBY,       /* 9: move stand by state */
	INTPRT_STATE_PAUSE,             /* A: pause state */
//	INTPRT_STATE_PAUSE_STANBY,      /* B: pause stand by state */
//	INTPRT_STATE_SKIP,              /* C: skip state */
//	INTPRT_STATE_SKIP_STANBY,       /* D: skip standby state */
//	INTPRT_STATE_CANCEL_SKIP        /* E: cancel skip state */
} INTPRT_STATE;

/*--------------------------------------------------------------------------------------------------*/
/*		Intprt Variable																			 	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _intprtv {
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR Seq;							/* Interpreter sequence step */
	CHAR  Next;							/* Program table next step */
	UCHAR EvSel;						/* Event SEl0-SEl7 */
	UCHAR Reserve0;						/* Reserve */
/*--------------------------------------------------------------------------------------------------*/
	long PgmStep;						/* Program table current step */
/*--------------------------------------------------------------------------------------------------*/
	long Acc;                           /* Interpreter current acceleration [ref/cycle^2] */
	long Dec;                           /* Interpreter current deceleration [ref/cycle^2] */
	long Tlim;							/* Interpreter current Torque Limit [%] */
/*--------------------------------------------------------------------------------------------------*/
	ULONG tEvTime;                      /* Event Time [ScanN count] */
	ULONG tEvTimeProgress;				/* Event time progress count [ScanN count] */
/*--------------------------------------------------------------------------------------------------*/
	long nLoop;							/* Loops */
	long nLoopProgress;                 /* Loop counter */
/*--------------------------------------------------------------------------------------------------*/
	PGMDEF *pCrPgm;                     /* Current Program */
	PGMDEF *pPrPgm;                     /* Previous Program */
/*--------------------------------------------------------------------------------------------------*/
	UINT (*isEvTriggered)( struct _intprtv *pv ); /* Event trigger check function */
/*--------------------------------------------------------------------------------------------------*/
} INTPRTV;

#ifdef	__ghs__
#pragma ghs startzda
#endif
extern INTPRTV IntprtV;
#ifdef	__ghs__
#pragma ghs endzda
#endif
