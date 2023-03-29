/****************************************************************************************************/
/*																									*/
/*																									*/
/*		Bprm.h : サーボ制御ベースパラメータ構造体定義												*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_BPRM_H_
#define	_BPRM_H_


#include "alarm.h"
#include "EncData.h"

/*---------------------------------------------------------------------------*/
/*		Un Monitor Average time												 */
/*---------------------------------------------------------------------------*/
#define SVCDEF_UNSPD_SCMEANNUM	((64 + KPI_SCCYCLEMS - 1) / KPI_SCCYCLEMS)
#define SVCDEF_UNSPD_SCMEANTMMS	( KPI_SCCYCLEMS * SVCDEF_UNSPD_SCMEANNUM )

/*---------------------------------------------------------------------------*/
/*		Normalize Multiplier Factor											 */
/*---------------------------------------------------------------------------*/
#define NORM_TRQ_TO_IQ			3840000		/* TrqRef(2^24) --> IqRef(15000) */
#define NORM_IQ_TO_TRQ			73300775	/* IqRef(15000) --> TrqRef(2^24) */

/****************************************************************************************************/
/*																									*/
/*		基本パラメータ定義																			*/
/*																									*/
/****************************************************************************************************/
typedef	struct BPRMDAT
{									/* Comment							Rotary		Linear		*/
	BOOL	RvsDir;					/* 逆回転モード												*/
	LONG	DirSign;				/* 1:正回転モード, -1:逆回転モード							*/
	LONG	MotSel;					/* モータ選択												*//* <S04D> */
	LONG	EncSel;					/* エンコーダ選択											*//* <S04D> */
	BOOL	FencUse;				/* フルクエンコーダ使用フラグ								*/
	BOOL	FencType;				/* フルクエンコーダ使用可能AMP								*//* <S04D> */
	BOOL	PsFulcSelect;			/* 完全フルクタイプ											*//* <S04D> */
	BOOL	SencFbOptUse;			/* Use Feedback Option Module Encoder for Semi-close control*//* <S04D> */
	BOOL	LinearMotorType;		/* Motor Type Rotary or Linear								*/
/*----------------------------------------------------------------------------------------------*/
	LONG	FbPulse;				/* Feedback PulseNo (Menc/Fenc)		[p/rev]		[p/360deg]	*/
	LONG	MotPulse;				/* Motor Encoder PulseNo			[p/rev]		[p/360deg]	*/
	LONG	XfbPulse1;				/* Feedback PulseNo for I/F			[p/rev]		[p/mm]		*/
	LONG	XfbPulse2;				/* Feedback PulseNo for I/F			[p/rev]		[p/pitch]	*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Kfbpls;					/* FB Pulse Factor (Menc/Fenc)		[rad/p]		[m/p]		*/
	KSGAIN	Kmotpls;				/* Motor Encoder Pulse Factor		[rad/p]		[m/p]		*/
	KSGAIN	KmotplsFC;				/* Motor Encoder Pulse Factor		[rad/p]		[m/p]		*/
	KSGAIN	Keangle;				/* Electric Angle Trans Factor		[Erad/Mrad]	[Erad/m]	*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	RatSpd;					/* RatedSpeed						[rad/sec]	[m/sec]		*/
	KSGAIN	OvrSpd;					/* OverSpeed						[rad/sec]	[m/sec]		*/
//	LONG	MaxAcc;	(現状未使用)	/* Max.Acceleration					[rad/s^2]	[m/s^2]		*/
	KSGAIN	MaxTrq;					/* Max.Torque						[Nm]		[N]			*/
	KSGAIN	MaxCur;					/* Max.Current						[A]			[A]			*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Vdc;					/* DC Voltage						[V]			[V]			*/
	KSGAIN	MotW;					/* Motor Power						[W]			[W]			*/
	KSGAIN	MotA;					/* Motor Rated Current				[A]			[A]			*/
	KSGAIN	MotR;					/* Motor Resistance					[ohm]		[ohm]		*/
	KSGAIN	MotLd;					/* Motor d-axis Inductance			[H]			[H]			*/
	KSGAIN	MotLq;					/* Motor q-axis Inductance			[H]			[H]			*/
	KSGAIN	MotLdb;					/* Motor Inductance for DbAlmChk	[H]			[H]			*/
	KSGAIN	MotEmf;					/* Motor EMF Constant				[Vrms/r/s]	[Vrms/m/s]	*/
	KSGAIN	Jmot;					/* Motor Rotor Inertia				[kg*m^2]	[kg]		*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	OvrPspd;				/* Pulse Speed at OvrSpd			[p/sec]     [p/sec]		*/
	KSGAIN	MaxPspd;				/* Pulse Speed at MaxSpd			[p/sec]     [p/sec]		*/
	KSGAIN	Knorspd;				/* Normalized Speed Trans Factor	[r/min,mm/s --> NorSpd]	*/
	KSGAIN	NorspdConv;				/* Normalized Speed Trans Factor	[r/min,mm/s --> NorSpd]	*/
	KSGAIN	KMotSpdConv;			/* 速度換算係数		<V053>			[0.1r/min,0.1mm/s->p/ms]*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	ExpSpd;					/* Motor Speed   Exponent			[-]			[-]			*/
	KSGAIN	ExpTrq;					/* Motor Torque  Exponent			[-]			[-]			*/
	KSGAIN	ExpMotW;				/* Motor Power   Exponent			[-]			[-]			*/
	KSGAIN	ExpJmot;				/* Motor Inertia Exponent			[-]			[-]			*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Kmottrq;				/* Kmottrq = MotMaxTrq / MaxTrq		[-]			[-]			*/
	USHORT	PerMaxTrq;				/* Max.  Torque Percent				[%]			[%]			*/
	USHORT	PerOvrSpd;				/* Over  Speed Percent				[0.01%]		[0.01%]		*/
	LONG	NorOvrSpd;				/* Over  Speed Normalized			[2^24]		[2^24]		*/
#if (FLOAT_USE==TRUE)
	float	pNorOvrSpd;				/* Over  Speed Normalized(正側)		[100.0]		[100.0]		*/
	float	nNorOvrSpd;				/* Over  Speed Normalized(負側)		[-100.0]	[-100.0]	*/
#endif /* FLOAT_USE */
	LONG	NorMaxSpd;				/* Max.  Speed Normalized			[2^24/OS]	[2^24/OS]	*/
	LONG	NorRatSpd;				/* Rated Speed Normalized			[2^24/OS]	[2^24/OS]	*/
	LONG    NorSuspSpd;             /* Suspend Speed Normalized			[2^24/OS]	[2^24/OS]   <S201>*/
	ULONG	OSCountMax;				/* OS状態連続カウンタ上限値			[-]			[-]			*/
/*----------------------------------------------------------------------------------------------*/
	LONG	AmpMotWatRatio;			/* (AmpWat  / MotWat ) * 2^16		[-]			[-]			*/
	LONG	AmpMotImaxRatio;		/* (AmpImax / MotImax) * 2^16		[-]			[-]			*/
/*----------------------------------------------------------------------------------------------*/
	LONG	CompVolt;				/* 主回路電圧補正下限値				<V116>					*/
/*----------------------------------------------------------------------------------------------*/
	UCHAR	AmpInVcode;				/* Amp Input  Volt Code				[-]			[-]			*/
	UCHAR	AmpOutVcode;			/* Amp Output Volt Code				[-]			[-]			*/
	UCHAR	AmpDcinType;			/* Amp DC Input Type (AC:0, DC:1)	[-]			[-]			*/
	UCHAR	AmpOut2VType;			/* Amp 倍電圧仕様(TRUE/FALSE)		[-]			[-]			*/
	USHORT	AmpUnVolt;				/* Amp Volt for UnXXX Monitor		[V]			[V]			*/
/*----------------------------------------------------------------------------------------------*/
	UCHAR	MotVcode;				/* Motor Volt Code					[-]			[-]			*/
	UCHAR	MaxVel;					/* Max Velocity						[100r/min]	[100mm/s]	*/
	USHORT	MotUnVolt;				/* Motor Volt For UnXXX Monitor		[V]			[V]			*/
	USHORT	VdetMax;				/* Amp Volt detect at max input		[data/MaxV] [data/MaxV] */
	USHORT	VdetAdMax;				/* Amp Volt AD detect at max input	[data/MaxV] [data/MaxV] */
/*----------------------------------------------------------------------------------------------*/
	BOOL	CurrentLoopTiSw;		/* 電流ループ積分時定数切替スイッチ							*/
	UCHAR	MotTypeCode;			/* Motor Type Code					[-]			[-]			*/

/*----------------------------------------------------------------------------------------------*/
	LONG	SvCycleUs;				/* サーボ制御周期 [us]										*/
	LONG	SvCycleNs;				/* サーボ制御周期 [ns]										*/
	LONG	MPFCycleUs_SC;			/* 磁極検出制御周期(速度・トルク制御) [us]					*/
	LONG	MPFCycleUs_MR;			/* 磁極検出制御周期(メイン処理) [us]						*/
/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	float	Kposlvl;				/* Position Level Cal. Factor	[CmdPulse --> FbkPulse]		*/
	float	Kspdlvl;				/* Speed    Level Cal. Factor	[0.1r/min,mm/s --> NorSpd]	*/
	float	Ktrqlvl;				/* Torque   Level Cal. Factor	[0.01% --> 2^24/MaxTrq]		*/
	float	Ktrqlvls;				/* Torque   Level Cal. Factor	[0.01% --> 15000/MaxTrq]	*/
/*----------------------------------------------------------------------------------------------*/
	float	Kmonorspd;				/* Normal. Speed Trans Factor	[r/min,mm/s --> NorSpd]		*/
	float	Knortrq;				/* Normal. Torque Trans Factor	[%RatedTorque --> NorTrq]	*/
	float	Kmonspd;				/* Monitor Speed Trans Factor	[NorSpd  --> r/min,mm/s]	*/
	float	Kmontrq;				/* Monitor Torque Trans Factor	[NorTrq  --> %RatedTorque]	*/
	float	Kmoncrnt;				/* Monitor Current Trans Factor	[NorCurrent->%RatedCurrent]	*/
	float	Kmonpfbkspd;			/* Monitor PfbkSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	float	Kmonpcmdspd;			/* Monitor PcmdSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	float	Kmonpcmdspd2ms;			/* Monitor PcmdSpd Trans Factor [p/2ms   --> r/min,mm/s]	*/
	float	Knorpulsspd;			/* Monitor PulsSpd Trans Factor [p/ScanB --> 2^24/OvrSpd]	*/
/*----------------------------------------------------------------------------------------------*/
	float	KmotspdA;				/* ScanA Motor Speed Cal. Gain	(KIOPから移動)				*/
	float	KmotspdB;				/* ScanB Motor Speed Cal. Gain	(KIOPから移動)				*/
	float	KmotspdC;				/* ScanC Motor Speed Cal. Gain	(KIOPから移動)				*/
/*----------------------------------------------------------------------------------------------*/
	float	Kcurload;				/* 実効トルク(電流)演算ゲイン(ScanC変数から移動)			*/
/*----------------------------------------------------------------------------------------------*/
	BOOL	SpdDetMaFil;			/* モータ速度検出用移動平均フィルタ有効/無効選択			*/
	float	SpdDetectUnit;			/* 速度検出分解能	[0.1r/min/pulse],[0.1mm/s/pulse]		*/
	float	SpdFbLpassFil;			/* モータ静音用速度ＦＢLPF設定	[0.01ms]		<V423>		*/
/*----------------------------------------------------------------------------------------------*/
	float	MonScalePitch;			/* Scale Pitch for Monitor				[pm]	MENCVから	*/
	float	MonScaleExp;			/* Scale Pitch Exponent for Monitor		[-]		MENCVから	*/
/*----------------------------------------------------------------------------------------------*/
	float	KConvLoadSpd;			/* 負荷速度変換係数	[pls/ScanA] -> [2^24/OvrSpd]			*/
/*----------------------------------------------------------------------------------------------*/
	float	Kvrefad;				/* 速度指令AD演算ゲイン										*/
#else
	LONG	Kposlvl;				/* Position Level Cal. Factor	[CmdPulse --> FbkPulse]		*/
	LONG	Kspdlvl;				/* Speed    Level Cal. Factor	[0.1r/min,mm/s --> NorSpd]	*/
	LONG	Ktrqlvl;				/* Torque   Level Cal. Factor	[0.01% --> 2^24/MaxTrq]		*/
	LONG	Ktrqlvls;				/* Torque   Level Cal. Factor	[0.01% --> 15000/MaxTrq]	*/
/*----------------------------------------------------------------------------------------------*/
	LONG	Kmonorspd;				/* Normal. Speed Trans Factor	[r/min,mm/s --> NorSpd]		*/
	LONG	Knortrq;				/* Normal. Torque Trans Factor	[%RatedTorque --> NorTrq]	*/
	LONG	Kmonspd;				/* Monitor Speed Trans Factor	[NorSpd  --> r/min,mm/s]	*/
	LONG	Kmontrq;				/* Monitor Torque Trans Factor	[NorTrq  --> %RatedTorque]	*/
	LONG	Kmoncrnt;				/* Monitor Current Trans Factor	[NorCurrent->%RatedCurrent]	*/
	LONG	Kmonpfbkspd;			/* Monitor PfbkSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	LONG	Kmonpcmdspd;			/* Monitor PcmdSpd Trans Factor [p/64ms  --> r/min,mm/s]	*/
	LONG	Kmonpcmdspd2ms;			/* Monitor PcmdSpd Trans Factor [p/2ms   --> r/min,mm/s]	*/
	LONG	Knorpulsspd;			/* Monitor PulsSpd Trans Factor [p/ScanB --> 2^24/OvrSpd]	*/
/*----------------------------------------------------------------------------------------------*/
	LONG	KmotspdA;				/* ScanA Motor Speed Cal. Gain	(KIOPから移動)				*/
	LONG	KmotspdB;				/* ScanB Motor Speed Cal. Gain	(KIOPから移動)				*/
	LONG	KmotspdC;				/* ScanC Motor Speed Cal. Gain	(KIOPから移動)				*/
/*----------------------------------------------------------------------------------------------*/
	LONG	Kcurload;				/* 実効トルク(電流)演算ゲイン(ScanC変数から移動)			*/
/*----------------------------------------------------------------------------------------------*/
	BOOL	SpdDetMaFil;			/* モータ速度検出用移動平均フィルタ有効/無効選択			*/
	LONG	SpdDetectUnit;			/* 速度検出分解能	[0.1r/min/pulse],[0.1mm/s/pulse]		*/
	LONG	SpdFbLpassFil;			/* モータ静音用速度ＦＢLPF設定	[0.01ms]		<V423>		*/
/*----------------------------------------------------------------------------------------------*/
	ULONG	MonScalePitch;			/* Scale Pitch for Monitor				[pm]	MENCVから	*/
	LONG	MonScaleExp;			/* Scale Pitch Exponent for Monitor		[-]		MENCVから	*/
/*----------------------------------------------------------------------------------------------*/
	LONG	KConvLoadSpd;			/* 負荷速度変換係数	[pls/ScanA] -> [2^24/OvrSpd]			*/
/*----------------------------------------------------------------------------------------------*/
	LONG	Kvrefad;				/* 速度指令AD演算ゲイン										*/
#endif /* FLOAT_USE */
/*----------------------------------------------------------------------------------------------*/
/*		中間パラメータ定義																		*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN	Kpx;					/* Position Control Gain									*/
	KSGAIN	Kvx;					/* Speed    Control Gain									*/
	KSGAIN	Kpvx;					/* Position-Speed Control Gain for MFC & Predictive Control	*/
	KSGAIN	Kmotspd;				/* Motor Speed Cal. Gain  [pulse/sec] --> [2^24/OvrSpd]		*/
	KSGAIN	KmotspdFC;				/* Motor Speed Cal. Gain  [pulse/sec] --> [2^24/OvrSpd]		*/
/*----------------------------------------------------------------------------------------------*/
/*	AP <S00C>																					*/
/*----------------------------------------------------------------------------------------------*/
	KSGAIN  TrefonLevel;			/* Torque Reference Input ON Level [2^24/MaxTrq]			*/
	ULONG	OsAlmLvl;				/* Osアラームレベル */	/* <S1D0> */
} BPRMDAT;

typedef struct BPRM_INICFG {
	ULONG	ratb2l;		/* Pn20E : 電子ギア比(分子)									*/
	ULONG	rata2l;		/* Pn210 : 電子ギア比(分母)									*/
	USHORT	maxvel_l;	/* Pn385 : モータ最高速度(リニア)							*/
	DBYTEX	styp;		/* PnE11 : 下位:サーボパック形式コード, 上位:入力電圧コード	*/
	USHORT	ampw;		/* PnE12 : サーボパック容量									*/
	USHORT	imaxs;		/* PnE15 : サーボパック最大電流								*/
	USHORT	dq_sw2;		/* PnE39 : 電流制御スイッチ2								*/
	DBYTEX	ratmt2_os;	/* PnE62 : 下位:過負荷検出中間時間2, 上位:OS検出レベル		*/
	USHORT	vrefgn;		/* Pn300 : 速度指令入力ゲイン								*/
	USHORT	gadjv;		/* PnE55 : 速度指令ゲイン調									*/
} BPRM_INICFG;

typedef enum BPRM_ERR_ {
	BPRM_NOALM		= 0,				/* アラームなし								*/
	BPRM_NOTSUPPORT	= 0x00000001,		/* A.051 : 製品未サポートアラーム			*/
	BPRM_VELPRM		= 0x00000002,		/* A.550 : 最高速度設定異常					*/
} BPRM_ERR;

/****************************************************************************************************/
/*	基本パラメータ計算メイン */
/****************************************************************************************************/
void BprmSetServoCycle( BPRMDAT *Bprm, LONG SACycleNs, LONG SBCycleNs );
LONG BprmCalcBaseParameters( BPRMDAT *Bprm, MENCPRM *MencP, BPRM_INICFG *CfgPrm, MENCV *MencV, MENCV *FencV );

LONG	BprmTorqueLevelCal( BPRMDAT *Brpm, LONG Torque, LONG LvlSel );
KSGAIN	BprmSpeedLevelCal( BPRMDAT *Brpm, KSGAIN Speed, KSGAIN LmtSel );		/*<S00A>*/
LONG	BprmPositionLevelCal( BPRMDAT *Brpm, LONG Position );


#endif	/* _BPRM_H_ */
/***************************************** end of file **********************************************/
