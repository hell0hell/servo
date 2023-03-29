/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Zone.h : Zone Table Header File 															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Zone Constant Definition																	*/
/*																									*/
/*		2. Zone Struct Definition																	*/
/*																									*/
/*		3. Zone Variable Extern Declaration															*/
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
/*		Zone Common Definitions																		*/
/*																									*/
/****************************************************************************************************/
#define ZONE_N_ZONE    (32)                   /* Maximum Zone Table Number                          */

/*--------------------------------------------------------------------------------------------------*/
/*	ZONE Version Definition																			*/
/*--------------------------------------------------------------------------------------------------*/
#define VERCHK_ZONE (0x0002)                  /* TODO: Versionä«óùédólåàÇﬂ */

/*--------------------------------------------------------------------------------------------------*/
/* Zone table sequence                                                                              */
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_ZONE_SEQ
{
	ZONE_SEQ_NOP,            /* 0: no operation */
	ZONE_SEQ_STORE,          /* 1: store state */
	ZONE_SEQ_STORE_ERS,      /* 2: store erase state */
	ZONE_SEQ_STORE_PGM,      /* 3: store program state */
	ZONE_SEQ_INIT,           /* 4: init state */
	ZONE_SEQ_INIT_ERS,       /* 5: init erase state */
	ZONE_SEQ_INIT_PGM,       /* 6: init program state */
	ZONE_SEQ_INIT_CACHE      /* 7: init cache state */
} ZONE_SEQ;

/*--------------------------------------------------------------------------------------------------*/
/* Zone table minimum/max                                                                           */
/*--------------------------------------------------------------------------------------------------*/
typedef struct struct_ZONE_MINMAX
{
	int Min;
	int Max;
} ZONE_MINMAX;

enum enum_ZONE_MinMax {
	ZONE_MIN = -99999999,
	ZONE_MAX =  99999999
};
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*  Zone Table Initial load return code                                                             */
/*--------------------------------------------------------------------------------------------------*/
typedef UINT ZONE_INITLOAD;
#define ZONE_INITLOAD_SUMWRONG	 (1<<0)                             /* bit0: sumcheck error */
#define ZONE_INITLOAD_INVALID	 (1<<1)                             /* bit1: exceeded value */
#define ZONE_INITLOAD_VERUNMATCH (1<<2)                             /* bit2: version unmatch */

/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_ZONE_RUNTIMEERR
{
	ZONE_RUNTIMEERR_NORMAL
} ZONE_RUNTIMEERR;
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*																									*/
/*		Zone Struct Definitions																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*  Zone Table	                                                                                    */
/*--------------------------------------------------------------------------------------------------*/
typedef struct _zone {
	long ZoneP;                               /* Positive position */
	long ZoneN;                               /* Negative position */
} ZONEDEF;

typedef struct _zonet {
	USHORT CkSum;                             /* Zone table checksum */
	USHORT Ver;                               /* Zone table version */
	ZONEDEF Row[ ZONE_N_ZONE ];               /* Zone table data rows */
} ZONET;

/*--------------------------------------------------------------------------------------------------*/
/*  Zone Table Variable                                                                             */
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ZONEV
{
	UCHAR Seq;                                /* zone table store/init sequence step */
	UCHAR Idx;                                /* Zone index */
	CHAR ReqId;                               /* zone table request id */
	CHAR FlReqId;                             /* Flash driver request id */
} ZONEV;


#ifdef	__ghs__
#pragma ghs startzda
#pragma ghs section zbss = ".TBLCACHE"
#endif
extern ZONET ZoneCache;							/* Zone Table Cache                                 */
#ifdef	__ghs__
#pragma ghs section zbss = default
#pragma ghs endzda
#endif
