/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Sqi.h : Local Header File for ScanN Software												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Translate Sequence Input Signal Logics													*/
/*		2. Sequence Input Signal State transition													*/
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
/*		SQI Definitions 																			*/
/*																									*/
/****************************************************************************************************/
//#define SQI_REQQUEMAX 10

/****************************************************************************************************/
/*																									*/
/*		SQI Type Definitions 																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		SQI Request Types									  										*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_SQI_REQ
{
	SQI_REQ_NOP,
	SQI_REQ_SVOFF,
	SQI_REQ_SVON,
	SQI_REQ_PAUSE,
	SQI_REQ_RUN,
	SQI_REQ_CANCEL,
	SQI_REQ_HOLD,
	SQI_REQ_SKIP,
	SQI_REQ_JOGP,
	SQI_REQ_JOGN,
	SQI_REQ_HOME,
	SQI_REQ_ARES
} SQI_REQ;

/*--------------------------------------------------------------------------------------------------*/
/*		SQI Signal Logic									  										*/
/*--------------------------------------------------------------------------------------------------*/
typedef enum enum_SQI_LGC
{
	SQI_LGC_DEFAULT,      /* 0: default */
	SQI_LGC_OPPOSITE,     /* 1: opposite */
	SQI_LGC_ACTIVE,       /* 2: always active */
	SQI_LGC_NONACTIVE     /* 3: alwais non active */
} SQI_LGC;

/*--------------------------------------------------------------------------------------------------*/
/*		SQI Signals																				 	*/
/*--------------------------------------------------------------------------------------------------*/
typedef UINT SQI_SIGS;
#define SQI_SIGS_MODE01		(0x001)
#define SQI_SIGS_STARTHOME	(0x002)
#define SQI_SIGS_PGMRESJOGP	(0x004)
#define SQI_SIGS_SEL0JOGN	(0x008)
#define SQI_SIGS_SEL1JOG0	(0x010)
#define SQI_SIGS_SEL2JOG1	(0x020)
#define SQI_SIGS_SEL3JOG2	(0x040)
#define SQI_SIGS_SEL4JOG3	(0x080)
#define SQI_SIGS_SEL5JOG4	(0x100)
#define SQI_SIGS_SEL6JOG5	(0x200)
#define SQI_SIGS_SEL7JOG6	(0x400)
#define SQI_SIGS_PDET		(0x800)		/* <S21C> */
#define SQI_SIGS_SON		(0x1000)
#define SQI_SIGS_ALMRST		(0x2000)
#define SQI_SIGS_DEC		(0x4000)
#define SQI_SIGS_RGRT		(0x8000)
#define SQI_SIGS_POT		(0x10000)
#define SQI_SIGS_NOT		(0x20000)
#define SQI_SIGS_PCL		(0x40000)
#define SQI_SIGS_NCL		(0x80000)
#define SQI_SIGS_CLR		(0x100000) /* @@ Anlg */
#define SQI_SIGS_PCON	    (0x200000) /* @@ Anlg */
#define SQI_SIGS_ZCLAMP     (0x400000) /* @@ Anlg */
#define SQI_SIGS_INHIBIT    (0x800000) /* @@ Anlg */
#define SQI_SIGS_PSEL       (0x1000000) /* @@ Anlg */
/* <S03D> Start */
#define SQI_SIGS_SPDD       (0x2000000)		/* @@ Anlg */
#define SQI_SIGS_SPDA       (0x4000000)		/* @@ Anlg */
#define SQI_SIGS_SPDB       (0x8000000)		/* @@ Anlg */
#define SQI_SIGS_CSEL       (0x10000000)	/* @@ Anlg */
/* <S03D> End */
#define SQI_SIGS_GSEL 		(0x20000000)	/* <S09B> */
#define SQI_SIGS_SEN 		(0x40000000)	/* <S168> */
#define SQI_SIGS_FSTP 		(0x80000000)	/* <S1F5> */

enum _SQI_BITNO {
	/*----------------------------------------------------------------------------------------------*/
	/*	Bit0-7																						*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_MODE01	= 0,   //(0x000001)
	SQI_BNO_STARTHOME,     //(0x000002)
	SQI_BNO_PGMRESJOGP,    //(0x000004)
	SQI_BNO_SEL0JOGN,      //(0x000008)
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_SEL1JOG0,      //(0x000010)
	SQI_BNO_SEL2JOG1,      //(0x000020)
	SQI_BNO_SEL3JOG2,      //(0x000040)
	SQI_BNO_SEL4JOG3,      //(0x000080)
	/*----------------------------------------------------------------------------------------------*/
	/*	Bit8-15																						*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_SEL5JOG4,      //(0x000100)
	SQI_BNO_SEL6JOG5,      //(0x000200)
	SQI_BNO_SEL7JOG6,      //(0x000400)
//	SQI_BNO_RESERVED,      //(0x000800)
	SQI_BNO_PDET,          //(0x000800)	 <S21C> 
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_SON,           //(0x001000)
	SQI_BNO_ALMRST,        //(0x002000)
	SQI_BNO_DEC,           //(0x004000)
	SQI_BNO_RGRT,          //(0x008000)
	/*----------------------------------------------------------------------------------------------*/
	/*	Bit16-23																					*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_POT,           //(0x010000)
	SQI_BNO_NOT,           //(0x020000)
	SQI_BNO_PCL,           //(0x040000)
	SQI_BNO_NCL,           //(0x080000)
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_CLR,           //(0x100000)	@@ Anlg
	SQI_BNO_PCON,          //(0x200000) @@ Anlg
	SQI_BNO_ZCLAMP,        //(0x400000) @@ Anlg
	SQI_BNO_INHIBIT,       //(0x800000) @@ Anlg
	/*----------------------------------------------------------------------------------------------*/
	/*	Bit24-31																					*/
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_PSEL,          //(0x1000000) @@ Anlg
/* <S03D> Start */
	SQI_BNO_SPDD,			//(0x2000000:Bit25) @@ Anlg
	SQI_BNO_SPDA,			//(0x4000000:Bit26) @@ Anlg
	SQI_BNO_SPDB,			//(0x8000000:Bit27) @@ Anlg
	/*----------------------------------------------------------------------------------------------*/
	SQI_BNO_CSEL,			//(0x10000000:Bit28) @@ Anlg
/* <S03D> End */
	SQI_BNO_GSEL,			//(0x20000000:Bit29) <S09B> 
	SQI_BNO_SEN,  	        //(0x40000000:Bit30) <S168>
	SQI_BNO_FSTP, 		//(X080000000:bit31) <ShOCRE><S1F5>
};

/*--------------------------------------------------------------------------------------------------*/
/*		SQI Variable																			 	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _sqiv {
/*--------------------------------------------------------------------------------------------------*/
	struct _sqi_lgc {					/* Sequence input signal logics */
		UINT	Mode01     :2;			/* /MODE01 */
		UINT	StartHome  :2;			/* /START-HOME */
		UINT	PgmresJogp :2;			/* /PGMRES-JOGP */
		UINT	Sel0JogN   :2;			/* /SEL0-JOGN */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Sel1Jog0   :2;			/* /SEL1-JOG0 */
		UINT	Sel2Jog1   :2;			/* /SEL2-JOG1 */
		UINT	Sel3Jog2   :2;			/* /SEL3-JOG2 */
		UINT	Sel4Jog3   :2;			/* /SEL4-JOG3 */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Sel5Jog4   :2;			/* /SEL5-JOG4 */
		UINT	Sel6Jog5   :2;			/* /SEL6-JOG5 */
		UINT	Sel7Jog6   :2;			/* /SEL7-JOG6 */
//		UINT	Reserve0   :2;			/* reserved */
		UINT	Pdet	   :2;          /* /P-DET(Pn50D.3) <S21C> */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Son        :2;			/* /S-ON */
		UINT	Ares       :2;			/* /ALM-RST */
		UINT	Dec        :2;			/* /DEC */
		UINT	Rgrt       :2;			/* /RGRT */
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pot        :2;          /* P-OT */
		UINT	Not        :2;          /* N-OT */
		UINT	Pcl        :2;          /* /P-CL */
		UINT	Ncl        :2;          /* /N-CL */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Clr        :2;          /* /Clr */
		UINT	Pcon       :2;          /* /P-CON */
		UINT	Zclamp     :2;          /* /ZCLAMP */
		UINT	Inhibit    :2;          /* /INHIBIT */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Psel       :2;          /* /Psel */
		UINT	SpdD       :2;          /* /SPD-D */
		UINT	SpdA       :2;          /* /SPD-A */
		UINT	SpdB       :2;          /* /SPD-B */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Csel       :2;          /* /C-SEL */
		UINT	Gsel	   :2;			/* /G-SEL <S09B> */
		UINT	Sen		   :2;			/* /SEN <S168> */
		UINT	Fstp	   :2;			/* /FSTP <S1F5> */
//		UINT	ReserveX   :2;          /* reserved */
	/*--------------------------------------------------------------------------------------------------*/
	} Lgc;
/*--------------------------------------------------------------------------------------------------*/
	struct _sqi_map {					/* signal maps to convert HW signal to logical signal */
		UINT	Mode01     :4;			/* /MODE01 */
		UINT	StartHome  :4;			/* /START-HOME */
	/*----------------------------------------------------------------------------------------------*/
		UINT	PgmresJogp :4;			/* /PGMRES-JOGP */
		UINT	Sel0JogN   :4;			/* /SEL0-JOGN */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Sel1Jog0   :4;			/* /SEL1-JOG0 */
		UINT	Sel2Jog1   :4;			/* /SEL2-JOG1 */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Sel3Jog2   :4;			/* /SEL3-JOG2 */
		UINT	Sel4Jog3   :4;			/* /SEL4-JOG3 */
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Sel5Jog4   :4;			/* /SEL5-JOG4 */
		UINT	Sel6Jog5   :4;			/* /SEL6-JOG5 */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Sel7Jog6   :4;			/* /SEL7-JOG6 */
//		UINT	Reserve0   :4;			/* reserved */
		UINT	Pdet	   :4;          /* /P-DET(Pn50D.3) <S21C> */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Son        :4;			/* /S-ON */
		UINT	Ares       :4;			/* /ALM-RST */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Dec        :4;			/* /DEC */
		UINT	Rgrt       :4;			/* /RGRT */
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pot        :4;          /* P-OT */
		UINT	Not        :4;          /* P-OT */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Pcl        :4;          /* /P-CL */
		UINT	Ncl        :4;          /* /P-cl */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Clr        :4;          /* /Clr */
		UINT	Pcon       :4;          /* /P-CON */
	/*----------------------------------------------------------------------------------------------*/
		UINT	Zclamp     :4;          /* /ZCLAMP */
		UINT	Inhibit    :4;          /* /INHIBIT */
	/*----------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
		UINT	Psel       :4;          /* /Psel */
		UINT	SpdD       :4;          /* /SPD-D */
	/*----------------------------------------------------------------------------------------------*/
		UINT	SpdA       :4;          /* /SPD-A */
		UINT	SpdB       :4;          /* /SPD-B */
	/*--------------------------------------------------------------------------------------------------*/
		UINT	Csel       :4;          /* /C-SEL */
		UINT	Gsel       :4;          /* /G-SEL */
	/*--------------------------------------------------------------------------------------------------*/
		UINT	Sen        :4;          /* /SEN <S168> */
		UINT	Fstp       :4;			/* /FSTP <S1F5> */
//		UINT	ReserveX   :4;          /* reserved */
	} Map;
/*--------------------------------------------------------------------------------------------------*/
	UCHAR SqiSeq;						/* SQI sequence step */
	UCHAR PgmSeq;						/* Program table mode sequence step */
	UCHAR JogSeq;						/* JOG/HOMING mode sequence step */
	UCHAR RegSeq;						/* Register Input Signal sequence step */
/*--------------------------------------------------------------------------------------------------*/
	UCHAR SIMode;						/* PnB90.0: Signal Input Mode */
	UCHAR AnMode;						/* PnB90.1: Analog Input Mode */
	UCHAR Reserve1;
	UCHAR Reserve2;
/*--------------------------------------------------------------------------------------------------*/
	SQI_SIGS Sigs;						/* Current Signals */
	SQI_SIGS LastSigs;					/* Last Signals */
	SQI_SIGS ActvMask;					/* Active Signal Masks */
	SQI_SIGS NonActvMask;				/* None Active Signal Masks */
/*--------------------------------------------------------------------------------------------------*/
	SQI_SIGS ActvLgcs;					/* Active Logics */
/*--------------------------------------------------------------------------------------------------*/
	LONG Tlim;							/* Torque limit [%] */
/*--------------------------------------------------------------------------------------------------*/
	SQI_SIGS PhsSig;					/* Physical Input Signals for Monitor */
	SQI_SIGS RegSig;					/* 0xAAA8: Input Signals for Register I/F */
	LONG SIStopTime;					/* 0xAAA6: Signal Input Stopping Time[ScanN count] */
	LONG SIStopTimeCnt;					/* Signal Input Clear Counter [ScanN count] */
/*--------------------------------------------------------------------------------------------------*/
	LONG SonRefTimeCnt;					/* Servo on signal Input Time Counter [ScanN count] */
/*--------------------------------------------------------------------------------------------------*/
	LONG DelayAlmToCoin;				/* Perr Clear Delay Timer [ScanN count] */
} SQIV;

/****************************************************************************************************/
/*																									*/
/*		Sqi Sequence Definitions																	*/
/*																									*/
/****************************************************************************************************/
enum _SQSTEP_SQI {
	SQSQI_INITIAL,		/* 0: Initial step */
	SQSQI_BB,			/* 1: Base block state */
//<S090>	SQSQI_WAITSVON,		/* 2: wait servo on state */
	SQSQI_RUN,			/* 3: Run state */
//<S090>	SQSQI_ZSTOP,		/* 4: Emergency stop state */ 
//<S090>	SQSQI_WAITALMRES,	/* 5: wait alerm clear state */ 
};

enum _SQSTEP_JOG {
	SQJOG_INIT,			/* 0: Jog/homing initial step */
	SQJOG_JOGP,			/* 1: JOGP state */
	SQJOG_JOGN,			/* 2: JOGN state */
	SQJOG_HOMING,		/* 3: HOMING state */
	SQJOG_PAUSE,		/* 4: pause state */
};

enum _SQSTEP_PGM {
	SQPGM_CANCEL,		/* 0: Program table drive cancel state */
	SQPGM_RUN,			/* 1: Program table drive run state */
	SQPGM_PAUSE,		/* 2: Program table drive pause state */
	SQPGM_HOMING,		/* 3: @@ Anlg HOMING state */
	SQPGM_HOMPAUSE,		/* 4: @@ Anlg HOMING pause state */
};

enum _SQSTEP_REG {
	SQREG_WAIT_SIGIN,	/* 0: wait input signal state */
	SQREG_INPUT_SIGNAL,	/* 1: input signal state */
};

/****************************************************************************************************/
/*																									*/
/*		Sqi Register I/F Definitions																*/
/*																									*/
/****************************************************************************************************/
enum _SQI_SIMODE {
	SQI_SIMODE_HWONLY = 0,
	SQI_SIMODE_REGONLY,
	SQI_SIMODE_HWREG,
};

enum _SQI_SIMODE_MinMax{
	SQI_SIMODE_MIN = SQI_SIMODE_HWONLY,
	SQI_SIMODE_MAX = SQI_SIMODE_REGONLY,
};

enum _SQI_CLRTIME_MinMax{
	SQI_CLRTIME_MIN = 0,
	SQI_CLRTIME_MAX = 10000,
};

/****************************************************************************************************/
/*																									*/
/*		Sqi Additional Definitions																	*/
/*																									*/
/****************************************************************************************************/
enum _SQI_ASSIGNMODE {
	SQI_ASSIGN_DEFAULT = 0,	
	SQI_ASSIGN_PRM,	
};

/****************************************************************************************************/
#ifdef	__ghs__
#pragma ghs startzda
#endif
extern struct _sqiv SqiV;
#ifdef	__ghs__
#pragma ghs endzda
#endif

extern const UCHAR CSqiAssignTbl[];

