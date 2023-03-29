/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Pls.h : Command Pulse Counter Definition File												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	    Command Pulse Counter Definitions.															*/
/*																									*/
/*			1. 																						*/
/*			2. 																						*/
/*			3. 																						*/
/*			4. 																						*/
/*			5. 																						*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2013.08.20  K.Ozaki																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef			___TST_PLS___
#define			___TST_PLS___

/****************************************************************************************************/
/*																									*/
/*		Pulse Counter Struct Definition																*/
/*																									*/
/****************************************************************************************************/
typedef struct _PLSIFP{				/**< Pulse Command HW Initial Parameters						*/
		UCHAR	PerClrEdgeMode;		/* Use CLR Signal Edge for Perr clear 							*/
		UCHAR	PerClrMode;			/*  															*/
		UCHAR	PerClrSigSel;		/* 																*/
		UCHAR	PerClrLvlSel;		/* 																*/
		UCHAR	PerClrMecha;		/* 1: Machine Position Base/ 0: Controler Position Base			*/
		UCHAR	CmdOpenCollector;	/* <S00C> Filter Flag for Open Collector Signal Usage 			*/
		SHORT	EdgeClr_DisableTime;/* Pn240: Edge Clear Disable Time Count [ScanB count]			*/
} PLSIFP;
typedef struct _PLSIFV{				/**< Pulse Command HW Interface Variable						*/
/*--------------------------------------------------------------------------------------------------*/
	PLSIFP P;						/**< Pulse Command HW Initial Parameters						*/
/*--------------------------------------------------------------------------------------------------*/
	struct {						/**< Pulse Command HW Counter Flags								*/
		UINT	TrcPcmdInon		:1; /* Trace : Pulse Command Input On								*/
		UINT	TrcPcmdSign		:1; /* Trace : Pulse Command Input Sign								*/
		UINT	PerClrSignal	:1; /* Position Error Clear Signal									*/
		UINT	PerClrEdge		:1; /* Position Error Clear Signal Edge Detected					*/
		/*------------------------------------------------------------------------------------------*/
		UINT	ClrEnableFlg 	:1;	/* Reference Latch End State of Position Edge Clear				*/
		UINT	EdgeClrUnmatch  :1; /* Perr Edge Clear Unmatch Status								*/
		UINT	EdgeDetected	:1; /* Clear Signal Edge Detection 									*/
		UINT	Reserve0		:1; /* Reserved					  									*/
		/*------------------------------------------------------------------------------------------*/
		UINT	ReserveX		:24;/* Reserved														*/
	} f;
/*--------------------------------------------------------------------------------------------------*/
	VUINT32	*CMDLDL;				/**< Pointer to the Command Pulse Counter Load Latch Value		*/
	VUINT32	*POCSET;				/**< Pointer to the Command Pulse Counter Current Value			*/
	VUINT32	*CMDCLML;				/**< Pointer to the Command Pulse Counter Latch Value			*/
	VUINT32	*TIM;					/**< Pointer to the Latch Timer Current Value					*/
	VUINT32	*SFBLT;					/**< Pointer to the Latched Feedback Position 					*/
	void (*LATCH_PULSCNTR)( void );	/**< Pointer to the Function to Latch Command Pulse Counter		*/
	LONG (*CHK_LATCHTIMER)(			/**< Pointer to the Function to Check Latch Timer				*/
			SHORT *pTim );			/**< Pointer to the Timer Variable								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	PcmdHwCntr;				/* Pulse Command HW Counter										*/
	SHORT	LastPcmdHwCntr;			/* Pulse Command HW Counter Last Scan							*/
	SHORT	dPcmdInputHwCntr;		/* Latch Delta Pulse Command of Position Egde Clear 			*/
	SHORT	dPcmdHwCntr;			/* Delta Pulse Command HW Counter								*/
	SHORT	ClrPcmdPastCntr;		/* 																*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	EdgeClr_DisableCnt;		/* Perr Clear Edge Disable Count [ScanB Count]					*/
	SHORT	ClrLatchRefPos;			/* Latch Pulse Command of Position Edge Clear					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	PerClrEdgeTime;			/* Perr Clear Edge Time											*/
	SHORT	LatchBaseTimeA;			/* ScanA Captured Latch Base Time [timer count:4Mhz]			*/
	SHORT	LatchBaseTimeB;			/* ScanB Captured Latch Base Time [timer count:4Mhz]			*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	PcmdRingCntr;			/* Pulse Command Ring  Counter [pulse]							*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	ClrFbPos;				/* latched feedback position for clr signal [pulse]				*/
	LONG	ClrFbPosSft;			/* latched feedback position for clr signal [pulse]				*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	MposSftR;				/* @@ Motor Position Shift Number								*/
	LONG	RvsDir;					/* @@ Reverse Direction	for Latch								*/
/*--------------------------------------------------------------------------------------------------*/
} PLSIFV;

/*--------------------------------------------------------------------------------------------------*/
/*		Command Pulse Counter Definition															*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		カウンタラッチ(CMD,FB,FC)																	*/
/*		Command Pulse Counter Access Macro Definition												*/
/*--------------------------------------------------------------------------------------------------*/
//#define	KPI_READ_PCMDCNTR( ch )		(SHORT)(*((LPlsIfV[ch]).CMDLDL))
#define	KPI_READ_PCMDCNTR( )				(SHORT)(*(pPls->CMDLDL))
//TODO: #define	KPI_READ_PFBCNTR( ch )		ASIC_RD( AREG_FBCLT )
//TODO: #define	KPI_READ_PFCCNTR( ch )		ASIC_RD( AREG_FCLDL )

/*--------------------------------------------------------------------------------------------------*/
/*		指令パルス入力信号読出し : モニタ用															*/
/*		Reading Command Pulse Signal : for Monitor													*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_READ_PCMD_STATUS( x )	{ x = (*(pPls->POCSET)); }
#define	KPX_CHECK_PCMD_CADAT( x )	( (x & 0x1000)? TRUE : FALSE )
#define	KPX_CHECK_PCMD_CBDAT( x )	( (x & 0x2000)? TRUE : FALSE )
#define	KPX_CHECK_PCMD_CCDAT( x )   ( (x & 0x4000)? TRUE : FALSE )

/*--------------------------------------------------------------------------------------------------*/
/*		指令パルス入力信号読出し : Unモニタ用														*/
/*--------------------------------------------------------------------------------------------------*/
#if 	SVFSEL_CMDPLS_USE == 1
#define	KPX_CHECK_PCMD_PULS( )      ( (pPls->f.TrcPcmdInon)? TRUE : FALSE )
#define	KPX_CHECK_PCMD_SIGN( )	    ( (pPls->f.TrcPcmdSign)? TRUE : FALSE )
#define	KPX_CHECK_PCMD_CLR( )       ( (pPls->f.PerClrSignal)? TRUE : FALSE )
#define	KPX_CHECK_PCMD_CLREDGE( ch )  ( ( LPlsIfV[(ch)].f.PerClrEdge ) ? TRUE : FALSE )

#else
#define	KPX_CHECK_PCMD_PULS( )      ( FALSE )
#define	KPX_CHECK_PCMD_SIGN( )	    ( FALSE )
#define	KPX_CHECK_PCMD_CLR( )       ( FALSE )
#define	KPX_CHECK_PCMD_CLREDGE( ch )  ( FALSE )

#endif


/*--------------------------------------------------------------------------------------------------*/
/*		偏差クリア信号読出し : モニタ用																*/
/*		Read Perr Clear Signal : for monitor														*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_READ_PERCLRSIGNAL( ch )	    ((*(LPlsIfV[ch].POCSET) & 0x4000) ? TRUE : FALSE)


/****************************************************************************************************/
/*																									*/
/*		ＣＰＵ：ラッチ位置補正演算マクロ定義														*/
/*																									*/
/****************************************************************************************************/
#define	CPU_LBTMCNTUS	(4)									/* Base Count per us [count/us]			*/
//#define KPI_SBCYCLEUS	(125)								/* @@ ScanB cycle time [us]				*///<S0C7>
/*--------------------------------------------------------------------------------------------------*/
#define	PFBKCOMPDELAY	((CPU_LBTMCNTUS * 491) / 10)		/* パルスＦＢ検出遅れ					*/
#define	PFBKPOUTDELAY	((CPU_LBTMCNTUS * 625) / 10)		/* 分周パルス出力遅れ					*/
#define	PFBKCOMPGAINX	((0x7FFFFFFF/(KPI_SBCYCLEUS*CPU_LBTMCNTUS))<<1)
/*--------------------------------------------------------------------------------------------------*/
/*		Position Command Compensation																*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_PREFCOMPEN( dPos, LatchTime )\
		MlibMulhigh32( dPos * (SHORT)(pPls->LatchBaseTimeB - LatchTime), PFBKCOMPGAINX )
/*--------------------------------------------------------------------------------------------------*/
/*		Position Feedback Compensation for Machine Base												*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_PFBKCOMPEN( dPos, LatchTime )\
		MlibMulhigh32( dPos * (SHORT)(pPls->LatchBaseTimeB - LatchTime - PFBKCOMPDELAY), PFBKCOMPGAINX )
/*--------------------------------------------------------------------------------------------------*/
/*		Position Feedback Compensation for Host Controller Base										*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPI_PFBKCOMPENHC( dPos, LatchTime )\
		MlibMulhigh32( dPos * (SHORT)(pPls->LatchBaseTimeB - LatchTime + PFBKPOUTDELAY), PFBKCOMPGAINX )
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		パルス指令フィルタ選択：オープンコレクタ信号用フィルタ選択									*/
/*--------------------------------------------------------------------------------------------------*/
#define CFG_FILSEL_PORTADR   (VUINT *)(0xEFFF8010)		/* 実際は、GPIO4 */
#define	KPI_PCMDFIL_ON( )			{ *CFG_FILSEL_PORTADR |= (1<<5); }
#define	KPI_PCMDFIL_OFF( )			{ *CFG_FILSEL_PORTADR &= ~(1<<5); }

/****************************************************************************************************/
/*																									*/
/*		Pulse Counter Interface Definition															*/
/*																									*/
/****************************************************************************************************/
#if 	SVFSEL_CMDPLS_USE == 1
#define PLS_N_CH	(2)								/* Pulse Counter Channel Number					*/
#define PLS_CH0_USE (1)								/* Pulse Counter Channel:0 Use					*/
#define PLS_CH1_USE (0)								/* Pulse Counter Channel:1 Not use				*/
#else
/* nothing */
#endif


/*--------------------------------------------------------------------------------------------------*/
/*		Function Prototypes																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 	SVFSEL_CMDPLS_USE == 1

void	KpxPlsScanAService( PLSIFV *pPls );	/* Command Pulse Manager(ScanA)							*/
void	KpxPlsScanBService( PLSIFV *pPls );	/* Command Pulse Manager(ScanB)							*/
#define	KpxPlsScanCService( x )	{ ;}		/* Command Pulse Manager(ScanC)							*/
void	KpxInitPls( void *ax ); 			/* Initialize Command Pulse Interface 					*/
void 	KpiPlsPerClrLatchPosition(			/* Input PERR Clear Latched Position 					*/
			PLSIFV *pPls, 					/* 														*/
			LONG PosFbki,					/* Feedback Position 									*/
			LONG MotAngle,					/* Motor Angle 	 										*/
			LONG *pClrPos );				/* Latched Position 									*/

#else

#define	KpxPlsScanAService( x )	{ ;}
#define	KpxPlsScanBService( x )	{ ;}
#define	KpxPlsScanCService( x )	{ ;}
#define	KpxInitPls( x )			{ ;}
#define	KpiPlsPerClrLatchPosition( x1, x2, x3, x4 ) { ;}

#endif

/*--------------------------------------------------------------------------------------------------*/
/*		Variables																					*/
/*--------------------------------------------------------------------------------------------------*/
//#if 	SVFSEL_CMDPLS_USE == 1
//#define CCMDPLSX (&(LPlsIfV[0]))		/* Command Pulse Interface CH:0								*/
//#define CCMDPLSY (&(LPlsIfV[1]))		/* Command Pulse Interface CH:1								*/
//extern 	PLSIFV LPlsIfV[PLS_N_CH];		/* Command Pulse Interface Variable							*/
//#else
//#define CCMDPLSX (NULL)					/* Command Pulse Interface CH:0								*/
//#define CCMDPLSY (NULL)					/* Command Pulse Interface CH:1								*/
//#endif
//
//#define KPX_GET_PLSIF( ch )			(( ch == 0 ) ? CCMDPLSX : CCMDPLSY )
#endif
