/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AlmMngrIx.h : Indexer Alarm Module Header File												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. AlmMngr Constant Definition																*/
/*																									*/
/*		2. AlmMngr Macro    Definition																*/
/*																									*/
/*		3. AlmMngr Variable Extern Declaration														*/
/*																									*/
/*		4. AlmMngr Function Prototype Definition													*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.11.14  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		AlmMngrIx Common Definitions																*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Indexer alerm bits definitions 																*/
/*--------------------------------------------------------------------------------------------------*/
typedef UINT _ALMBITS;
/*--------------------------------------------------------------------------------------------------*/
#define ALMBITS_INSIGTIMEOUT		(0x20000000)	/* Sigma-S */
#define ALMBITS_ZRETOVERSPEED		(0x10000000)	/* Sigma-S */
#define ALMBITS_ZRETUNCOMPLETE		(0x08000000)	/* Sigma-S */
#define ALMBITS_STARTUPERR			(0x04000000)
#define ALMBITS_RUNTIMEERR			(0x02000000)
#define ALMBITS_SOFTVERUNMATCH		(0x01000000)
/*--------------------------------------------------------------------------------------------------*/
#define ALMBITS_PRMSUMWRONG			(0x00800000)
#define ALMBITS_PRMVERUNMATCH		(0x00400000)
#define ALMBITS_PRMVALEXCEED		(0x00200000)
#define ALMBITS_SVHANDSHAKEMISS		(0x00100000)
/*--------------------------------------------------------------------------------------------------*/
#define ALMBITS_SVWDCMISS			(0x00080000)
#define ALMBITS_PGMSUMWRONG			(0x00040000)
#define ALMBITS_PGMVERUNMATCH		(0x00020000)
#define ALMBITS_PGMVALEXCEED		(0x00010000)
/*--------------------------------------------------------------------------------------------------*/
#define ALMBITS_ZONESUMWRONG		(0x00000080)
#define ALMBITS_ZONEVERUNMATCH		(0x00000040)
#define ALMBITS_ZONEVALEXCEED		(0x00000020)
#define ALMBITS_JSPDSUMWRONG		(0x00000010)
/*--------------------------------------------------------------------------------------------------*/
#define ALMBITS_JSPDVERUNMATCH		(0x00000008)
#define ALMBITS_JSPDVALEXCEED		(0x00000004)
#define ALMBITS_SVALM				(0x00000002)
#define ALMBITS_REGDISTSHORT		(0x00000001)


/*--------------------------------------------------------------------------------------------------*/
/*  Alarm Set APIs                                                                                  */
/*--------------------------------------------------------------------------------------------------*/
#define IXX_ALARMSET( almbit )	{NoutV.AlmBits |= (almbit);}
#define IXX_ALARMCLR( almbit )	{NoutV.AlmBits &= ~(almbit);}

/*--------------------------------------------------------------------------------------------------*/
/*		Indexer Error Code Definition(Žb’è) 														*/
/*--------------------------------------------------------------------------------------------------*/
typedef UINT NSAPP_ERRCODE;
#define ERRCODE_PGMSTOREFAIL		(0xe41)	//E41E
#define ERRCODE_ZONESTOREFAIL		(0xe42)	//E42E
#define ERRCODE_JSPDSTOREFAIL		(0xe43)	//E43E
#define ERRCODE_PGMDIRTY			(0xe44)	//E44E
#define ERRCODE_ZONEDIRTY			(0xe45)	//E45E
#define ERRCODE_JSPDDIRTY			(0xe46)	//E46E
#define ERRCODE_SERCMRXOVERFLOW		(0xe47)	//E47E
#define ERRCODE_SERCMPE				(0xe48)	//E48E
#define ERRCODE_SERCMFE				(0xe49)	//E49E
#define ERRCODE_SERCMOVE			(0xe4a)	//E4AE
#define ERRCODE_POTREJECT			(0xe4b)	//E4BE
#define ERRCODE_NOTREJECT			(0xe4c)	//E4CE
#define ERRCODE_PLSREJECT			(0xe4d)	//E4DE
#define ERRCODE_NLSREJECT			(0xe4e)	//E4EE
#define ERRCODE_POSEXCEED			(0xe4f)	//E4FE
#define ERRCODE_POSNOTYET			(0xe51)	//E51E
#define ERRCODE_RDSTNOTYET			(0xe52)	//E52E
#define ERRCODE_POSINTERFERED		(0xe53)	//E53E
#define ERRCODE_SVONNOTYET			(0xe54)	//E54E
#define ERRCODE_SVONFAIL			(0xe55)	//E55E
#define ERRCODE_SERCMSN				(0xe56)	//E56E
#define ERRCODE_UNKNOWN				(0xe57)	//E57E
#define ERRCODE_INVALID				(0xe58)	//E58E
#define ERRCODE_SVCMFAIL			(0xe59)	//E59E
#define ERRCODE_SVONREJECT			(0xe5a)	//E5AE
#define ERRCODE_ALMREJECT			(0xe5b)	//E5BE
#define ERRCODE_MAINOFFREJECT		(0xe5c)	//E5CE
#define ERRCODE_HOMENOTUSE			(0xe5d)	//E5DE
#define ERRCODE_PGMRUNNING			(0xe5e)	//E5EE
#define ERRCODE_SESINTERFERED		(0xe5f)	//E5FE
#define ERRCODE_PGUNMATCH			(0xe61)	//E61E
#define ERRCODE_NOTALMCC			(0xe62)	//E62E
#define ERRCODE_ASTOP				(0xe63)	//E63E

/*--------------------------------------------------------------------------------------------------*/
/*  Error Set APIs                                                                                  */
/*--------------------------------------------------------------------------------------------------*/
#ifdef IXX_ROUND
	#define IXX_ERRORSET( code )     (RoutV.ErrCode = (code))      /* set indexer error */
	#define IXX_ERRORRESET()         (RoutV.ErrCode = (0))         /* reset indexer error */
#else
	#define IXX_ERRORSET( code )     (NoutV.ErrCode = (code))      /* set indexer error */
	#define IXX_ERRORRESET()         (NoutV.ErrCode = (0))         /* reset indexer error */
#endif
#define IXX_ERRORCHK()                                             /* check current indexer error */\
								((NoutV.ErrCode) ? (NoutV.ErrCode) : (0 /* TODO: RoutV.ErrCode */))   
/*--------------------------------------------------------------------------------------------------*/
#define IXX_ERRORLATCH( x )      ((x) = (IXX_ERRORCHK()) ? IXX_ERRORCHK() : (x))  /* latch error */

/****************************************************************************************************/
/*																									*/
/*		AlmMngrIx Struct Definitions 																*/
/*																									*/
/****************************************************************************************************/
typedef struct _OpAlmV {
	UCHAR WrnSeq;			/* warning sequence step */
	UCHAR Reserve;			/* reserved */
	USHORT WrnTimerCnt;		/* warning clear timer count */
} OPALMV;

extern OPALMV OpAlmV;
