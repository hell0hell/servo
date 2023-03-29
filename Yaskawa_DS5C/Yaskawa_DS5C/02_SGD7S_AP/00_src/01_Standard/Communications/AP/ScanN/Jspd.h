/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Jspd.h : Jog Speed Table Header File 														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Jspd Constant Definition																	*/
/*																									*/
/*		2. Jspd Struct Definition																	*/
/*																									*/
/*		3. Jspd Variable Extern Declaration															*/
/*																									*/
/*		4. Local Function Prototype Definition														*/
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
/*		Jspd Common Definitions																		*/
/*																									*/
/****************************************************************************************************/
#define JSPD_N_JSPD    (16)                   /* Maximum Jog Speed Table Number                     */

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Version Definition																			*/
/*--------------------------------------------------------------------------------------------------*/
#define VERCHK_JSPD (0x0002)                  /* TODO: Versionä«óùédólåàÇﬂ */

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Speed Store/Init sequence                                                                  */
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_JSPD_SEQ
{
	JSPD_SEQ_NOP,            /* 0: no operation */
	JSPD_SEQ_STORE,          /* 1: store state */
	JSPD_SEQ_STORE_ERS,      /* 2: store erase state */
	JSPD_SEQ_STORE_PGM,      /* 3: store program state */
	JSPD_SEQ_INIT,           /* 4: init state */
	JSPD_SEQ_INIT_ERS,       /* 5: init erase state */
	JSPD_SEQ_INIT_PGM,       /* 6: init program state */
	JSPD_SEQ_INIT_CACHE      /* 7: init cache state */
} JSPD_SEQ;

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Speed Min/Max                                                                              */
/*--------------------------------------------------------------------------------------------------*/
enum enum_JSPD_MinMax {
	JSPD_MIN = 1,
	JSPD_MAX = 99999999,
};

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Speed Initial Load Error code                                                              */
/*--------------------------------------------------------------------------------------------------*/
typedef UINT JSPD_INITLOAD;
#define JSPD_INITLOAD_SUMWRONG	    (1<<0)                          /* bit0: sumcheck error */
#define JSPD_INITLOAD_INVALID	    (1<<1)                          /* bit1: exceeded value */
#define JSPD_INITLOAD_VERUNMATCH	(1<<2)                          /* bit2: version unmatch */

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Speed Error code                                                             				*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_JSPD_RUNTIMEERR
{
	JSPD_RUNTIMEERR_NORMAL
} JSPD_RUNTIMEERR;

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Speed Table Definition                                                       				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _jspddef
{
	long Jspd;                    /* jog speed */
} JSPDDEF;

typedef struct _jspdt
{
	USHORT CkSum;                 /* Jog Speed table check sum */
	USHORT Ver;                   /* Jog Speed table version */
	JSPDDEF Row[ JSPD_N_JSPD ];   /* Jog Speed data rows */
} JSPDT;

/*--------------------------------------------------------------------------------------------------*/
/*	Jspd Speed Variable Definition                                                       			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _jspdv
{
	UCHAR Seq;                   /* sequence step */
	UCHAR Idx;                   /* index */
	CHAR ReqId;                  /* Jspd request id */
	CHAR FlReqId;                /* flash driver request id */
} JSPDV;

/*--------------------------------------------------------------------------------------------------*/

#ifdef	__ghs__
#pragma ghs startzda
#pragma ghs section zbss = ".TBLCACHE"
#endif
extern JSPDT JspdCache;								/* Jog Speed Table Cache                        */
#ifdef	__ghs__
#pragma ghs section zbss = default
#pragma ghs endzda
#endif
