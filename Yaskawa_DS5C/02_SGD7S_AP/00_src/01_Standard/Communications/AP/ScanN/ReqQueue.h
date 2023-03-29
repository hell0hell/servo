/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ReqQueue.h : Indexer Request Queue Header File												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. ReqQueue Constant Definition																*/
/*																									*/
/*		2. ReqQueue Macro    Definition																*/
/*																									*/
/*		3. ReqQueue Variable Extern Declaration														*/
/*																									*/
/*		4. ReqQueue Function Prototype Definition													*/
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
/*		Constant Definition																			*/
/*																									*/
/****************************************************************************************************/
#define PGM_REQQUE (&(RqV[REQQUE_MODULES_PGM]))
#define ZONE_REQQUE (&(RqV[REQQUE_MODULES_ZONE]))
#define JSPD_REQQUE (&(RqV[REQQUE_MODULES_JSPD]))
#define SQI_REQQUE (&(RqV[REQQUE_MODULES_SQI]))

/*--------------------------------------------------------------------------------------------------*/
/*		using modules																				*/
/*--------------------------------------------------------------------------------------------------*/
#define N_REQQUEUE (2)		/* number of request queue */
enum _ReqQueueUseModules 
{
	REQQUE_MODULES_PGM = 0,				/* 0: PGM */
	REQQUE_MODULES_ZONE,				/* 1: ZONE */
	REQQUE_MODULES_JSPD,				/* 2: JSPD */
	REQQUE_MODULES_SQI,					/* 3: SQI */
	REQQUE_MODULES_CNT
};
/*--------------------------------------------------------------------------------------------------*/
/*		Typdefs 																					*/
/*--------------------------------------------------------------------------------------------------*/
typedef int REQID;				/* request queue ID */
#define REQID_OVERFLOW (-1)
#define REQID_NOP      (-1)

/*--------------------------------------------------------------------------------------------------*/
/*		Common Request 																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _REQ
{
	REQ_STORE,
	REQ_INIT
} REQ;

/*--------------------------------------------------------------------------------------------------*/
/*		SQI Request 																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _SQIREQ
{
	SQIREQ_SETSIGS,
} SQIREQ;

/*--------------------------------------------------------------------------------------------------*/
/*		Common Answer 																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum _ANS
{
	ANS_SPACE,
	ANS_BUSY,
	ANS_SUCCESS,
	ANS_FAIL
} ANS;

/*--------------------------------------------------------------------------------------------------*/
/*		Request Queue Struct 																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _RequestAnswer
{
	volatile UCHAR Req;
	volatile UCHAR Ans;
	volatile UINT Arg;
} REQANS;

typedef struct _RequestQueue
{
	REQANS v[ N_REQQUEUE ];
	UCHAR RdIdx;
	UCHAR WrIdx;
	UCHAR Reserve0;
	UCHAR Reserve1;
} REQQUE;

extern REQQUE RqV[];

