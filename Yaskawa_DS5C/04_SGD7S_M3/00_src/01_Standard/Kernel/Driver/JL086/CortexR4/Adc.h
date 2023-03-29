/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Adc.h : A/D Converter Definition File														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	    A/D Converter Definitions.																	*/
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
/*		Rev.1.00 : 2013.08.14  K.Ozaki																*/
/*		Rev.1.10 : 2013.10.01  K.Ozaki			<S00C>												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef			___TST_ADC___
#define			___TST_ADC___

#include "SystemSetting.h"
/****************************************************************************************************/
/*																									*/
/*		A/D Converter Register Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/** ADC Register Mapping 																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct {
	VUINT32 ADM0;								/**< 0x0000 A/D converter mode register 0 			*/
	VUINT32 ADM1;								/**< 0x0004 A/D converter mode register 1 			*/
	VUINT32 ADM2;								/**< 0x0008 A/D converter mode register 2 			*/
	VUINT32 ADM3;								/**< 0x000C A/D converter mode register 3 			*/
/*--------------------------------------------------------------------------------------------------*/
	VUINT32 Reserved0[(0x20-0x10)/4];			/**< 0x0010 - 0x001F reserved						*/
/*--------------------------------------------------------------------------------------------------*/
	VUINT32 ADINT;								/**< 0x0020 A/D converter interrupt control register*/
	VUINT32 ADSTS;								/**< 0x0024 A/D converter status register			*/
	VUINT32 ADIVC;								/**< 0x0028 A/D converter clock division register	*/
	VUINT32 Reserved1;							/**< 0x002C reserved								*/
/*--------------------------------------------------------------------------------------------------*/
	VUINT32	ADCR0;								/**< 0x0030 A/D convert result register 0			*/
	VUINT32	ADCR1;								/**< 0x0034 A/D convert result register 1			*/
	VUINT32	ADCR2;								/**< 0x0038 A/D convert result register 2			*/
	VUINT32	ADCR3;								/**< 0x003C A/D convert result register 2			*/
/*--------------------------------------------------------------------------------------------------*/
	VUINT32	ADCR4;								/**< 0x0040 A/D convert result register 4			*/
	VUINT32	ADCR5;								/**< 0x0044 A/D convert result register 5			*/
	VUINT32	ADCR6;								/**< 0x0048 A/D convert result register 6			*/
	VUINT32	ADCR7;								/**< 0x004C A/D convert result register 7			*/
/*--------------------------------------------------------------------------------------------------*/
	VUINT32	ADCR8;								/**< 0x0050 A/D convert result register 8			*/
	VUINT32	ADCR9;								/**< 0x0054 A/D convert result register 9			*/
	VUINT32	ADCRA;								/**< 0x0058 A/D convert result register A			*/
	VUINT32	ADCRB;								/**< 0x005C A/D convert result register B			*/
/*--------------------------------------------------------------------------------------------------*/
} tADC_REG;

/*--------------------------------------------------------------------------------------------------*/
/*  	Register Base Address Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define CONFIG_ADC_REGBASE (unsigned int)(0xEFFF9000)			/* @@ */
#define CONFIG_ADC_REGOFFSET (0x60)								/* @@ */
#define CPU_ADC_REG0 (tADC_REG *)(CONFIG_ADC_REGBASE + CONFIG_ADC_REGOFFSET*0)

/*--------------------------------------------------------------------------------------------------*/
/*		Register Access Macro Definition															*/
/*--------------------------------------------------------------------------------------------------*/
#define ADC_REG_WR( reg, data ) (areg)->##reg = (data)
#define ADC_REG_RD( reg, data ) (data) = (areg)->##reg

/*--------------------------------------------------------------------------------------------------*/
/* ADC Software Reset 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_ADC_RESET( ) 	{;}							/* TODO: 									*/

/*--------------------------------------------------------------------------------------------------*/
/* ADC Initialize 	 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_INIT_CPUAD( )		{;}						/* TODO: 									*/

/*--------------------------------------------------------------------------------------------------*/
/* START ADC  	 																					*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_START_CPUAD( )		{;}						/* TODO: 									*/

/*--------------------------------------------------------------------------------------------------*/
/* Check ADC Interrupt 																				*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_CHK_CPUADINTF( ) ( (areg->ADSTS & 0x03) == 0x03 )
//#define KPX_CLR_CPUADINTF( ) { do{ ADC_REG_WR( ADSTS, 0x03 ); ADC_REG_RD( ADSTS, sts ); } while ((sts&0x03)); }
#define KPX_CLR_CPUADINTF( ) { ADC_REG_WR( ADSTS, 0x03 ); }

/*--------------------------------------------------------------------------------------------------*/
/* Read CPU ADC		 																				*/
/*--------------------------------------------------------------------------------------------------*/
//#define KPX_START_CPUAD()	(ADM0 = CPU_ADM0SET + 0x80)		/* CPU AD Start Command					*/
#define KPX_READ_CPUAD0( )	( (USHORT)((CPU_ADC_REG0)->ADCR0) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD1( )	( (USHORT)((CPU_ADC_REG0)->ADCR1) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD2( )	( (USHORT)((CPU_ADC_REG0)->ADCR2) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD3( )	( (USHORT)((CPU_ADC_REG0)->ADCR3) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD4( )	( (USHORT)((CPU_ADC_REG0)->ADCR4) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD5( )	( (USHORT)((CPU_ADC_REG0)->ADCR5) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD6( )	( (USHORT)((CPU_ADC_REG0)->ADCR6) )		/* CPU AD Value Read			*/
#define KPX_READ_CPUAD7( )	( (USHORT)((CPU_ADC_REG0)->ADCR7) )		/* CPU AD Value Read			*/

/*--------------------------------------------------------------------------------------------------*/
/*		指令用ＡＤコントロール																		*/
/*--------------------------------------------------------------------------------------------------*/
#define KPX_REFAD_LATCH( )			(((ASIC_HW_IF_STR *)_UDL_REG_ADDR_)->AREG_ADSET1) |= (1<<14)
//#define	KPX_REFAD_SYNC( )			ASIC_WR( AREG_ADSYNC, 0x0000 )

//#if	( SVFSEL_VREFAD_24BIT != 0 )	/* @SKLT@ */
//#define KPX_REFAD_READAD0( )		(((SHORT)ASIC_RD(AREG_ACULH)<<16) + (USHORT)ASIC_RD(AREG_ACUL))
//#define KPX_REFAD_READAD1( )		(((SHORT)ASIC_RD(AREG_ACVLH)<<16) + (USHORT)ASIC_RD(AREG_ACVLL))
//#else
//#define KPX_REFAD_READAD0( )		((SHORT)ASIC_RD(AREG_ACUL))
//#define KPX_REFAD_READAD1( )		((SHORT)ASIC_RD(AREG_ACVLL))
//#endif

/****************************************************************************************************/
/*																									*/
/*		ＣＰＵ：トルク指令ＡＤ切り替えハードウェア定義												*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		トルク指令ＡＤ切替えマクロ定義																*/
/*		※注意!! GPIOのbit演算は割り込み禁止が必要!!												*/
/*--------------------------------------------------------------------------------------------------*/
#define CFG_ADSEL_PORTADR   (VUINT *)(0xEFFF8010)		/* 実際は、GPIO4 */
#define	KPX_SET_ADSEL_TREF( )		{ *CFG_ADSEL_PORTADR &= ~(1<<6); }  /* Select CPUAD Tref		*/
#define	KPX_SET_ADSEL_VREF( )		{ *CFG_ADSEL_PORTADR |= (1<<6); }   /* Select CPUAD Vref		*/
/*--------------------------------------------------------------------------------------------------*/
#define	KPX_ADSEL_TREF				(0)							/* CPU AD Tref Select(LO)			*/
#define	KPX_ADSEL_VREF				(1)							/* CPU AD Vref Select(HI)			*/
#define	KPX_READ_ADSEL( )			((*CFG_ADSEL_PORTADR & (1<<6)) == 0)? KPX_ADSEL_TREF : KPX_ADSEL_VREF;
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*																									*/
/*		A/D Converter Interface Definition															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
#if 	SVFSEL_ADC_USE == 1
#define ADC_N_CH (2) 						/* Number of A/D converter								*/
#define ADC_CH0_USE (1)						/* A/D converter Channel:0 Use							*/
#define ADC_CH1_USE (0)						/* A/D converter Channel:1 Not Use						*/
#else
/* nothing */
#endif
/*--------------------------------------------------------------------------------------------------*/
#if 	SVFSEL_ADC_USE == 1
#define ADC_VREFAD_USE (1)					/* 1: Use Vref Ad */
#define ADC_TREFAD_USE (1)					/* 1: Use Tref Ad */
#else
#define ADC_VREFAD_USE (0)					/* 0: Not Use Vref Ad */
#define ADC_TREFAD_USE (0)					/* 0: Not Use Tref Ad */
#endif

enum {
	ADC0_VREFAD = 0,						/* ADC0: VREF 											*/
	ADC1_TREFAD,							/* ADC1: TREF 											*/
//	ADC3_OHI,								/* @@@ ADC3: Over heat ? 								*//* <S0CB> Rename */
	ADC3_OHIAD,								/* Axis0 Over heat		  								*//* <S0CB> Rename */
	ADC4_OHIAD,								/* Axis2 Over heat	 									*//* <S0E1> */
	ADCX_NUM								/* number of ADCs */
};
/*--------------------------------------------------------------------------------------------------*/
enum _ADCMODE{
	ADCMD_NORMAL, 							/* ADC Normal Mode */
	ADCMD_ADJREFAD,							/* ADC Adjust Mode */
};
/*--------------------------------------------------------------------------------------------------*/
struct	_ADCPRM {							/* AD Converter Parameter								*/
	LONG	ZadjVelo;						/* Zero Adjust Variable (*2^7 Velocity) 				*/
	SHORT	ZadjTrqP;						/* Zero Adjust Variable (Torque) 						*/
	SHORT	ZadjTrqN;						/* Zero Adjust Variable (Torque) 						*/
	LONG	KvadchkP;						/* 速度指令ＡＤチェック正側演算ゲイン					*/
	LONG	KvadchkN;						/* 速度指令ＡＤチェック負側演算ゲイン					*/
	LONG	VrefAdAlmLevel;					/* 速度指令ＡＤチェックアラームレベル					*/
};
typedef struct _ADCPRM ADCPRM;

/*--------------------------------------------------------------------------------------------------*/
struct	_ADCFLG {							/* AD Converter Flags									*/
	UCHAR	TrefAdReadOk;					/* Tref AD Read OK Flag for KpiInputTrqRefAd()			*/
	UCHAR	VrefAdErrChk;					/* Vref AD Error Check Flag								*/
	UCHAR	AdjustError;					/* Adjust Error Flag									*/
	UCHAR	Reserve2;						/*														*/
};
typedef struct _ADCFLG ADCFLG;


typedef struct _ADC {
	LONG	Var[ADCX_NUM];					/* A/D Variable (sign extended 32bit variable)			*/ 
/*--------------------------------------------------------------------------------------------------*/
	ADCPRM	P;								/* A/D converter Parameters								*/
	ADCFLG	f;								/* A/D converter Flags									*/
/*--------------------------------------------------------------------------------------------------*/
	UCHAR	Mode;							/* A/D converter mode 									*/
	UCHAR	VrefAdDchkCnt;					/* Vref AD Data  Check Counter							*/
	UCHAR	Reserved0;						/*														*/
	UCHAR	Reserved1;						/*														*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	TrefAdinP;						/* Tref Positive AD Input Data							*/
	LONG	TrefAdinN;						/* Tref Negative AD Input Data							*/
/*--------------------------------------------------------------------------------------------------*/
	void	*hAlm;							/* @@ Alarm Manager!!(後で消す)							*/
	void	*pPrm;							/* @@ Parameter (後で消す)								*/
	void	*pSot;							/* @@ Sequence control out (後で消す)					*/
} CADC;

#if(PRODUCT_SERIES_SEVEN==TRUE)                /* <S09F> */
/*--------------------------------------------------------------------------------------------------*/
/*		Function Prototypes																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 	(SVFSEL_ADC_USE == 1)
void	KpxAdcScanAService( CADC *pAdc );	/* A/D Converter ScanA Service							*/
void	KpxAdcScanBService( CADC *pAdc );	/* A/D Converter ScanB Service							*/
void	KpxAdcScanCService( CADC *pAdc );	/* A/D Converter ScanC Service							*/
void	KpxInitAdc( void *ax ); 			/* Initialize A/D Converter Interface 					*/

#elif (SVFSEL_ONLY_OH_ADC_USE == 1)			/* <S0E1>:SVFSEL_ONLY_OH_ADC_USE追加					*/
#define KpxAdcScanAService( x )			{;}
#define KpxAdcScanBService( x )			{;}
void	KpxAdcScanCService( CADC *pAdc );	/* A/D Converter ScanC Service							*/
void	KpxInitAdc( void *ax ); 			/* Initialize A/D Converter Interface 					*/

#else
#define KpxAdcScanAService( x )			{;}	
#define KpxAdcScanBService( x )			{;}	
#define KpxAdcScanCService( x )			{;}	
#define KpxInitAdc( x )					{;}

#endif
#else
//#if 	SVFSEL_ADC_USE == 1                   /* <S09F> */
void	KpxAdcScanAService( CADC *pAdc );	/* A/D Converter ScanA Service							*/
void	KpxAdcScanBService( CADC *pAdc );	/* A/D Converter ScanB Service							*/
void	KpxAdcScanCService( CADC *pAdc );	/* A/D Converter ScanC Service							*/
void	KpxInitAdc( void *ax ); 			/* Initialize A/D Converter Interface 					*/

//#else                                      /* <S09F> */
//#define KpxAdcScanAService( x )			{;}	 /* <S09F> */
//#define KpxAdcScanBService( x )			{;}	 /* <S09F> */
//#define KpxAdcScanCService( x )			{;}	 /* <S09F> */
//#define KpxInitAdc( x )					{;} /* <S09F> */

//#endif                                      /* <S09F> */
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Variables																					*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*	Parameter Calculation																			*/
/*--------------------------------------------------------------------------------------------------*/
#define CPCALADC_TREFP (15091)
#define CPCALADC_TREFN (14299)
KSGAIN	KpiPcalSpdRefAdGain(  				/* Velocity Reference AD gain caluculation 				*/
	LONG	RatSpd,							/* rated speed [rad/s], [m/s] 							*/
	LONG	OvrSpd,							/* overed speed [rad/s], [m/s] 							*/
	LONG	Vrefgn,							/* Pn300: velocity input gain [0.01V/rated speed 		*/
	LONG	Gadjv );						/* PnE55: velocity gain adjust variable					*/
KSGAIN	KpiPcalTrqRefAdGain( 				/* Torque Reference AD gain calculation					*/
	LONG	MaxTrq, 						/* MaxTrq : 最大トルク[%]								*/
	LONG	Trefgn, 						/* Trefgn : トルク指令入力ゲイン[0.1V/定格トルク]		*/
	LONG	Gadjtp, 						/* Gadjtp : 正側トルク指令ゲイン調 [1/256]				*/
	LONG	Ref );							/* Ref.Voltage: TrefP:15091, TrefN:14299				*/

/*--------------------------------------------------------------------------------------------------*/
/*	ADC Adjust 																						*/
/*--------------------------------------------------------------------------------------------------*/
#define KPI_ADC_NOTADJ	(0)					/* 調整実施条件不成立 									*/
#define KPI_ADC_ADJOK	(1)					/* 調整完了 											*/
#define KPI_ADC_ADJERR	(-1)				/* 調整失敗 											*/
#if(PRODUCT_SERIES_SEVEN==TRUE)                                           /* <S09F> */
#if 	SVFSEL_ADC_USE == 1
LONG	KpiAdjustRefAdZero( CADC *pAdc );	/* Zero Adjust of specified ADC							*/
#else
#define KpiAdjustRefAdZero( x )  (KPI_ADC_NOTADJ) 
#endif
/*--------------------------------------------------------------------------------------------------*/
#else
//#if 	SVFSEL_ADC_USE == 1               /* <S09F> */
LONG	KpiAdjustRefAdZero( CADC *pAdc );	/* Zero Adjust of specified ADC							*/
//#else                                   /* <S09F> */
//#define KpiAdjustRefAdZero( x )  (KPI_ADC_NOTADJ)  /* <S09F> */
//#endif                                       /* <S09F> */
/*--------------------------------------------------------------------------------------------------*/

#endif

/*--------------------------------------------------------------------------------------------------*/
/*	Overheat voltage value conversion factor														*/
/*--------------------------------------------------------------------------------------------------*/
/* 以下の関係から[V/ADC変換値]の平均を求め、KSGAIN形式(ただし、SX<24となること)で定義する。			*/
/*	--------------------------------------------------------------------------------------------	*/
/* 																									*/
/* 	入力電圧値[V]	ADC変換値[DEC] ([HEX])															*/
/* 		 5.00[V]	3723[DEC] (E8B[HEX])															*/
/* 		 4.00[V]	2978[DEC] (BA2[HEX])															*/
/* 		 3.00[V]	2234[DEC] (8BA[HEX])															*/
/* 		 2.00[V]	1489[DEC] (5D1[HEX])															*/
/* 		 1.00[V]	 745[DEC] (2E9[HEX])															*/
/* 		 0.00[V]	   0[DEC] (  0[HEX])															*/
/* 																									*/
/*--------------------------------------------------------------------------------------------------*/
#define OH_ADC2VOL	0x182260DF

#endif
