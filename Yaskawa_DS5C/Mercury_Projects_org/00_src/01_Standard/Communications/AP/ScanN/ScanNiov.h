/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ScanNiov.h : Local Header File for ScanN Software											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Local Constant Definition																*/
/*																									*/
/*		2. Local Macro    Definition																*/
/*																									*/
/*		3. Local Variable Extern Declaration														*/
/*																									*/
/*		4. Local Function Prototype Definition														*/
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
#ifndef __SCANN_IOV__
#define __SCANN_IOV__


/****************************************************************************************************/
/*																									*/
/*		Define ScanN Input/Output Variables 														*/
/*																									*/
/****************************************************************************************************/

/*--------------------------------------------------------------------------------------------------*/
/*		ScanN Input Variable Definition																*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _NINV
{
/*--------------------------------------------------------------------------------------------------*/
	struct {
		UINT BB        :1;					/* Base Block */
		UINT HOMEDONE  :1;					/* Homing done */
		UINT NLS       :1;					/* Negative soft limit */
		UINT PLS       :1;					/* Positive soft limit */
		UINT NOT       :1;					/* Negative OT */
		UINT POT       :1;					/* Positive OT */
		UINT MAIN      :1;					/* Main power on */
		UINT TGON      :1;					/* TGON */
/*--------------------------------------------------------------------------------------------------*/
		UINT Safety    :1;                  /* Safety */
		UINT SVDEN     :1;                  /* SVDEN */
		UINT DEN       :1;                  /* DEN */
		UINT INPOS     :1;                  /* Inpos */
		UINT NEAR      :1;                  /* Near */
		UINT SVWRN     :1;                  /* servo warning */
/*--------------------------------------------------------------------------------------------------*/
		UINT SVALM     :1;                  /* Servo alerm */
		UINT REGDISTSHORT :1;               /* Registration distination */
		UINT SVWDCMISS :1;                  /* Watch dog count error */
		UINT SVHANDSHAKEMISS :1;            /* Handshake error */
/*--------------------------------------------------------------------------------------------------*/
		UINT PGCPHASE   :1;                 /* @@ Anlg */
		UINT CLR		:1;					/* @@ Anlg Mercury */
		UINT SENI		:1;					/* @@ Cn1-4 SENI <S00C> <S168> */
	} NrmBits;
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* Initial Parameters 									*/
	UCHAR	PerClrMode;						/* 位置偏差クリア動作選択設定 							*/
	UCHAR	PerClrSigSel;					/* 位置偏差クリア信号形態選択設定 						*/
	UCHAR	PerClrLvlSel;					/* 位置偏差クリア信号レベル選択設定 					*/
	UCHAR	AssignMode;						/* Pn50A.0: Input Signal Assign Mode 					*/
	UCHAR	AbsUse;							/* Using Absolute Encoder <S00C>						*/
	//UCHAR	Reserved0;
	UCHAR	CtrlMode;						/* 制御方式(制御モード)					*/	/* <S162>  	*/
	UCHAR	Reserved1;
	UCHAR	Reserved2;
	} P;
/*--------------------------------------------------------------------------------------------------*/
	ULONG Hwsin;							/* Hardware input signals (SI0-SI6) */
/*--------------------------------------------------------------------------------------------------*/
	RSP_CTRLBITS	Ctrl;					/* Control state (dbg) */
	RSP_SEQBITS 	Seq;					/* Sequence control state */
	RSP_LTBITS		Lt;						/* Latch state (dbg) */
/*--------------------------------------------------------------------------------------------------*/
	LONG    Apos;                           /* M2 Absolute position [internal ref] */
	LONG    LastApos;                       /* M2 last Absolute position [internal ref] */
	LONG    Pos;                            /* M2 refenrece position [internal ref] */
	LONG    LastPos;                        /* M2 last reference position [internal ref] */
/*--------------------------------------------------------------------------------------------------*/
	LONG	Ad1;							/* @@ Anlg HW A/D value1 */
	LONG	Ad2;							/* @@ Anlg HW A/D value2 */
	LONG	PcmdRingCntr;					/* @@ Anlg HW Command Input Ring Counter [pulse] */
/*--------------------------------------------------------------------------------------------------*/
} NINV;

/*--------------------------------------------------------------------------------------------------*/
/*		ScanN Output Variable Definition															*/
/*--------------------------------------------------------------------------------------------------*/
enum _ScanNExeMode {
	EMODE_NOCTRL,                           /* 0: No control */
	EMODE_JOGHOMING,                        /* 1: Jog/Homing Mode */
	EMODE_PGM,                              /* 2: Program Mode */
	EMODE_ANLG,                             /* 3: Analog Mode */
};
typedef	struct _NOUTV
{
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	ExeMode;					    /* indexer module execution mode */
	UCHAR	HomeUse;						/* use Homing or not */
	UCHAR	HomeMethod;					    /* homing method */
	UCHAR	CtrlModeA;						/* @@ Anlg Reference Input Mode */
	UCHAR	CtrlModeB;						/* @@ Anlg Reference Input Mode */			/* <S03D> */
	UCHAR	CtrlModeSel;					/* @@ Anlg Reference Input Mode Select*/	/* <S03D> */
/*--------------------------------------------------------------------------------------------------*/
	struct {								/* Initial Parameters 									*/
	UCHAR	CoinOut;						/* /COIN出力タイミング選択設定 							*/
	UCHAR	OppositeMode;					/* Pn512 Opposite Output (b0:SO1,b1:SO2,b2:SO3)			*/
	UCHAR	Reserved1;						/* 														*/
	UCHAR	Reserved2;						/* 														*/
	} P;
/*--------------------------------------------------------------------------------------------------*/
	ULONG	AlmBits;						/* Indexer Alerm Bits */
	LONG	HomePos;						/* Homing Position [ref] */
	LONG	HomeSpd;						/* Homing Spead [1000ref/min] */
	LONG	HomeTlim;						/* Homing Torque [2^24/MaxTrq] */
/*--------------------------------------------------------------------------------------------------*/
	LONG	Acc;							/* Default acceleration [ref/cycle^2] */
	LONG	Dec;							/* Default deceleration [ref/cycle^2] */
/*--------------------------------------------------------------------------------------------------*/
	UINT	Sigs;							/* Output signals ( SO1-SO7 ) */
	ULONG	XorMask;						/* Output signal Mask Info. *//* <S04B> */
/*--------------------------------------------------------------------------------------------------*/
	CMD_CTRLBITS	Ctrl;					/* Control request (dbg) */
	CMD_SEQBITS     Seq;					/* Sequence control command from scanN (dbg) */
	CMD_LTBITS		Lt;						/* Latch request (dbg) */
/*--------------------------------------------------------------------------------------------------*/
	POUTS	Pouts;							/* Pout settings  */
/*--------------------------------------------------------------------------------------------------*/
	ULONG	ErrCode;                        /* Current error code */
	ULONG	LatestErrCode;                  /* Latest error code */
} NOUTV;


/*--------------------------------------------------------------------------------------------------*/
/*		ScanN Input/Output Variable Extern															*/
/*--------------------------------------------------------------------------------------------------*/
extern	NINV	NinV;						/* ScanN Input Variables								*/
extern	NOUTV	NoutV;						/* ScanN Output Variables								*/
/*--------------------------------------------------------------------------------------------------*/
#endif
