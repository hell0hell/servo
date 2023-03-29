/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MprgStruct.h : Mercury Micro Program Registers definition									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2013.06.27 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SvIpAccess.h"



/****************************************************************************************************/
/*																									*/
/*		defines for Micro program																	*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		ServoIP Memory Maps																			*/
/****************************************************************************************************/
#define SVIP_TOP_ADR	0x600		/* H/Wレジスタ先頭アドレス										*/
#define SVIP_AXIS_SIZE	0x100		/* 1軸当たりの使用領域											*/


/****************************************************************************************************/
/*		degree value																				*/
/****************************************************************************************************/
#define PI2			16384			/*  90deg														*/
#define PI23		21845			/* 120deg														*/


/****************************************************************************************************/
/*		AMPTYPE : 回転型/リニア型選択																*/
/****************************************************************************************************/
#define ROTARY		0x0000			/* 回転型														*/
#define LINEAR		0x0001			/* リニア型														*/


/****************************************************************************************************/
/*		PAOSEQCMD : PaoSeqMcmd Interface Definition													*/
/****************************************************************************************************/
#define PAONOCMD	0x0000			/* No Command              (Host CPU:KPX_PAOSEQ_NOCMD)			*/
#define PAOPLSOUT	0x0020			/* IncPulse Output Request (Host CPU:KPX_IPLSOUT_REQUEST)		*/


/****************************************************************************************************/
/*		POSETCMD : パルス出力回路初期化要求　(絶対値授受シーケンスのパルス出力回路初期化用)			*/
/****************************************************************************************************/
#define POSETNOCMD	0x0000			/* 要求なし														*/
#define POSETCMD00	0x0001			/* パルス変換位置に0000hを強制セット要求						*/
#define POSETCMDFF	0x0002			/* パルス変換位置にFFFFhを強制セット要求						*/


/****************************************************************************************************/
/*		CPORT read data																				*/
/****************************************************************************************************/
//;--				0x0001			/* bit.0 : --Reserve--											*/
//;--				0x0002			/* bit.1 : --Reserve--											*/
//;--				0x0004			/* bit.2 : --Reserve--											*/
#define RLOCK		0x0008			/* bit.3 : Transer register lock status							*/
#define BB			0x0010			/* bit.4 : Base block bit										*/


/****************************************************************************************************/
/*		status set																					*/
/****************************************************************************************************/
//;--				0x0001			/* bit.0 : --Reserve--											*/
//;--				0x0002			/* bit.1 : --Reserve--											*/
//;--				0x0004			/* bit.2 : --Reserve--											*/
#define DLIM		0x0008			/* bit.3 : D-axis integration limit status						*/
#define QLIM		0x0008			/* bit.3 : Q-axis integration limit status						*/
//;--				0x0010			/* bit.4 : --Reserve--											*/
#define TLIM		0x0020			/* bit.5 : Torque Limit status									*/
#define IHOLD		0x0040			/* bit.6 : Anti-windup test										*/


/****************************************************************************************************/
/*		status clear																				*/
/****************************************************************************************************/
//;--				0x0fffe			/* bit.0 : --Reserve--											*/
//;--				0x0fffd			/* bit.1 : --Reserve--											*/
//;--				0x0fffb			/* bit.2 : --Reserve--											*/
#define DLIMI		0x0fff7			/* bit.3 : D-axis integration limit status						*/
#define QLIMI		0x0fff7			/* bit.3 : Q-axis integration limit status						*/
//;--				0x0ffef			/* bit.4 : --Reserve--											*/
#define TLIMI		0x0ffdf			/* bit.5 : Torque Limit status									*/
#define IHOLDI		0x0ffbf			/* bit.6 : Anti-windup test										*/


/****************************************************************************************************/
/*		CSW (Control Select Switch )																*/
/****************************************************************************************************/
#define ICLR		0x0001			/* bit.0  積分リミット時処理：ホールド(0)／クリア(1)			*/
#define ISEL		0x0002			/* bit.1  Ld/Lq補償：電流FB(0)／電流指令(1)						*/
//;--				0x0004			/* bit.2  --Reserve--											*/
#define OBSSEL		0x0008			/* bit.3  Current Observer Select bit							*/
#define F1DSABL		0x0010			/* bit.4  Notch filter1 Disable									*/
#define F2DSABL		0x0020			/* bit.5  Notch filter2 Disable									*/
#define LPFDSABL	0x0040			/* bit.6  Low Pass filter1 Disable								*/
#define LPFCDSABL	0x0080			/* bit.7  Low Pass filter2 Disable								*/
#define OVMSEL1		0x0100			/* bit.8  if bit8,9 = 01 then OVMOD1							*/
#define OVMSEL2		0x0200			/* bit.9  if bit8,9 = 10 or 11 then OVMOD2  					*/
#define OVMMOD		0x0400			/* bit.10 modulation modification enable						*/
#define V_FB		0x0800			/* bit.11 電圧FB方式弱め界磁選択								*/
#define DIDTSEL		0x1000			/* bit.12 L di/dt cmp select(1:di/dt,0:def)						*/
#define V_FB2		0x2000			/* bit.13 電圧FB方式弱め界磁選択2								*/
#define INT_ST		0x4000			/* bit.14 飽和時積分停止										*/
#define F3DSABL		0x8000			/* bit.15 Notch filter3 Disable									*/


/****************************************************************************************************/
/*		ビットパラメータ ( 初期化時セット CPU→ASIC )												*/
/****************************************************************************************************/
#define UPGDIVOUT	0x0001			/* bit.0  マイクロ分周機能使用選択								*/
//;--				0x0002			/* bit.1  --Reserve--											*/
//;--				0x0004			/* bit.2  --Reserve--											*/
//;--				0x0008			/* bit.3  --Reserve--											*/
//;--				0x0010			/* bit.4  --Reserve--											*/
//;--				0x0020			/* bit.5  --Reserve--											*/
//;--				0x0040			/* bit.6  --Reserve--											*/
//;--				0x0080			/* bit.7  --Reserve--											*/
//;--				0x0100			/* bit.8  --Reserve--											*/
//;--				0x0200			/* bit.9  --Reserve--											*/
//;--				0x0400			/* bit.10 --Reserve--											*/
//;--				0x0800			/* bit.11 --Reserve--											*/
//;--				0x1000			/* bit.12 --Reserve--											*/
//;--				0x2000			/* bit.13 --Reserve--											*/
//;--				0x4000			/* bit.14 --Reserve--											*/
//;--				0x8000			/* bit.15 --Reserve--											*/


/****************************************************************************************************/
/*		ビットデータ ( 初期化時セット CPU→ASIC、スキャン処理 ASIC→CPU )							*/
/****************************************************************************************************/
#define SERXERR		0x0001			/* bit.0  Senc Receive Error									*/
#define ACCCHKENA	0x0002			/* bit.1  Encoder Acc. Check Enable Flag						*/
//;--				0x0004			/* bit.2  --Reserve--											*/
//;--				0x0008			/* bit.3  --Reserve--											*/
//;--				0x0010			/* bit.4  --Reserve--											*/
//;--				0x0020			/* bit.5  --Reserve--											*/
//;--				0x0040			/* bit.6  --Reserve--											*/
//;--				0x0080			/* bit.7  --Reserve--											*/
//;--				0x0100			/* bit.8  --Reserve--											*/
//;--				0x0200			/* bit.9  --Reserve--											*/
//;--				0x0400			/* bit.10 --Reserve--											*/
//;--				0x0800			/* bit.11 --Reserve--											*/
//;--				0x1000			/* bit.12 --Reserve--											*/
//;--				0x2000			/* bit.13 --Reserve--											*/
//;--				0x4000			/* bit.14 --Reserve--											*/
//;--				0x8000			/* bit.15 --Reserve--											*/



/****************************************************************************************************/
/*																									*/
/*		Work Register : WREGBASE address = 08007000h												*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		32 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ACRV {
	DWREG	IdIntgl;			/* 0x08007000 : ACRd Integral										*/
	DWREG	IqIntgl;			/* 0x08007004 : ACRq Integral										*/
	DWREG	VdFil;				/* 0x08007008 : ACRd Filter Output									*/
	DWREG	VqFil;				/* 0x0800700C : ACRq Filter Output									*/
} ACRV;

/*--------------------------------------------------------------------------------------------------*/
/*		16 bit registers																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _STSFLG {
	SHORT	BbSetW;				/* 0x08007010 : soft BB set(BBSET)									*/
	SHORT	FltStsW;			/* 0x08007012 : fault status(FLTIN)									*/
	SHORT	CtrlStsRW;			/* 0x08007014 : controll flag/status(CTSTR,CTSTW)					*/
	SHORT	CNTSTS;				/* 0x08007016 : counter status(FBCSTS)								*/
	SHORT	FccStsMon;			/* 0x08007018 : full closed counter status(FCCST)					*/
	SHORT	IntglFlg;			/* 0x0800701A : 積分飽和フラグ										*/
} STSFLG;

/*--------------------------------------------------------------------------------------------------*/
/*		voltage FF																					*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _VCMPV {
	SHORT	LdC;				/* 0x0800701C : Ld  * omega											*/
	SHORT	LqC;				/* 0x0800701E : Lq  * omega											*/
	SHORT	MagC;				/* 0x08007020 : Phi * omega											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdOut;				/* 0x08007022 : Vd output voltage									*/
	SHORT	VqOut;				/* 0x08007024 : Vq output voltage									*/
	SHORT	VdComp;				/* 0x08007026 : Vd compensation data								*/
	SHORT	VqComp;				/* 0x08007028 : Vq compensation data								*/
	SHORT	VuOut;				/* 0x0800702A : vu output data (to PWM)								*/
	SHORT	VvOut;				/* 0x0800702C : vv output data (to PWM)								*/
	SHORT	VwOut;				/* 0x0800702E : vw output data (to PWM)								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdRef;				/* 0x08007030 : vdref												*/
	SHORT	VqRef;				/* 0x08007032 : vqref												*/
	SHORT	Vmax2;				/* 0x08007034 : 電圧指令ベクトル最大値(8192*1.27)					*/
	SHORT	V12;				/* 0x08007036 : 電圧指令ベクトル(√(VdOut^2+VqOut^2))				*/
} VCMPV;

/*--------------------------------------------------------------------------------------------------*/
/*		trigonometric function value																*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _SINTBL {
	SHORT	SinT;				/* 0x08007038 : sin theta											*/
	SHORT	CosT;				/* 0x0800703A : cos theta											*/
	SHORT	SinT2;				/* 0x0800703C : sin(theta + 2pi/3)									*/
	SHORT	CosT2;				/* 0x0800703E : cos(theta + 2pi/3)									*/
	SHORT	SinT3;				/* 0x08007040 : sin(theta - 2pi/3)									*/
	SHORT	CosT3;				/* 0x08007042 : cos(theta - 2pi/3)									*/
} SINTBL;

/*--------------------------------------------------------------------------------------------------*/
/*		A/D Stop Error detect																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ADSTOP {
	SHORT	ADRst;				/* 0x08007044 : 													*/
	SHORT	ADERRCNT;			/* 0x08007046 : ASIC AD Error Counter								*/
	SHORT	IRIUP;				/* 0x08007048 : Previous IRIU										*/
	SHORT	IRIUCNT;			/* 0x0800704A : same IRIU counter									*/
	SHORT	IRIVP;				/* 0x0800704C : Previous IRIV										*/
	SHORT	IRIVCNT;			/* 0x0800704E : same IRIV counter									*/
} ADSTOP;

/*--------------------------------------------------------------------------------------------------*/
/*		CPU I/F Data																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ADINV {
	SHORT	IuOffsetIn;			/* 0x08007050 : A/D transfer offset for iu							*/
	SHORT	IvOffsetIn;			/* 0x08007052 : A/D transfer offset for iv							*/
	SHORT	KcuIn;				/* 0x08007054 : A/D transfer gain for iu							*/
	SHORT	KcvIn;				/* 0x08007056 : A/D transfer gain for iv							*/
	SHORT	IdIn;				/* 0x08007058 : Id reference										*/
	SHORT	IqIn;				/* 0x0800705A : Iq reference										*/
	SHORT	PhaseHIn;			/* 0x0800705C : PHA input											*/
	SHORT	VelIn;				/* 0x0800705E : VEL input											*/
/*--------------------------------------------------------------------------------------------------*/
/*		voltage compensation																		*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	VdRefIn;			/* 0x08007060 : vdref input											*/
	SHORT	VqRefIn;			/* 0x08007062 : vqref input											*/
/*--------------------------------------------------------------------------------------------------*/
/*		Torque Limit																				*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	TLimPIn;			/* 0x08007064 : +tlim												*/
	SHORT	TLimMIn;			/* 0x08007066 : -tlim												*/
} ADINV;

/*--------------------------------------------------------------------------------------------------*/
/*		static variable																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADP {
	SHORT	Kcu;				/* 0x08007068 : A/D transfer gain for iu							*/
	SHORT	Kcv;				/* 0x0800706A : A/D transfer gain for iv							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for over modulation																			*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kmod;				/* 0x0800706C : Over modulation gain/offset							*/
	SHORT	Kvv;				/* 0x0800706E : AVR													*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kc;					/* 0x08007070 : Current conversion Gain								*/
	SHORT	Ld;					/* 0x08007072 : d axis Inductance									*/
	SHORT	Lq;					/* 0x08007074 : q axis Inductance									*/
	SHORT	Mag;				/* 0x08007076 : Magnetic flux (Phi)									*/
	SHORT	KdP;				/* 0x08007078 : d axis propotion gain (PI control)					*/
	SHORT	KqP;				/* 0x0800707A : q axis propotion gain (PI control)					*/
	SHORT	KdI;				/* 0x0800707C : d axis integral time (gain)							*/
	SHORT	KqI;				/* 0x0800707E : q axis integral time (gain)							*/
	SHORT	VdLim;				/* 0x08007080 : d axis integral limit								*/
	SHORT	VqLim;				/* 0x08007082 : q axis integral limit								*/
	SHORT	KvvIn;				/* 0x08007084 : Voltage Compasation gain							*/
	SHORT	OnDelayLvl;			/* 0x08007086 : On delay change level								*/
	SHORT	Tfil;				/* 0x08007088 : Filter time constant								*/
	SHORT	Vmax;				/* 0x0800708A : voltage limit data (Vmax^2)							*/
	SHORT	OnDelayComp;		/* 0x0800708C : On-delay compensation								*/
	SHORT	CtrlSw;				/* 0x0800708E : Control Flag										*/
	SHORT	CrFreq;				/* 0x08007090 : carrier freq reference(change while BB)				*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch1																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf11;				/* 0x08007092 : 													*/
	SHORT	Kf12;				/* 0x08007094 : 													*/
	SHORT	Kf13;				/* 0x08007096 : 													*/
	SHORT	Kf14;				/* 0x08007098 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch2																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf21;				/* 0x0800709A : 													*/
	SHORT	Kf22;				/* 0x0800709C : 													*/
	SHORT	Kf23;				/* 0x0800709E : 													*/
	SHORT	Kf24;				/* 0x080070A0 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		notch3																						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Kf31;				/* 0x080070A2 : 													*/
	SHORT	Kf32;				/* 0x080070A4 : 													*/
	SHORT	Kf33;				/* 0x080070A6 : 													*/
	SHORT	Kf34;				/* 0x080070A8 : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		LPF																							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	TLpf;				/* 0x080070AA : LPF1 gain											*/
	SHORT	TLpf2;				/* 0x080070AC : LPF2 gain											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	MotResist;			/* 0x080070AE : Moter resistance(r1/I_BASE*V_BASE)					*/
	SHORT	OnDelaySlope;		/* 0x080070B0 : Dead-time comp. gain								*/
	SHORT	L_dIdt;				/* 0x080070B2 : 													*/
	SHORT	FccRst;				/* 0x080070B4 : FCCST Reset(for TEST)								*/
	SHORT	Dummy;				/* 0x080070B6 : for Alignment										*/
} INTADP;

/*--------------------------------------------------------------------------------------------------*/
/*		Current Loop Control Data																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADV {
	SHORT	IuOffset;			/* 0x080070B8 : A/D transfer offset for iu							*/
	SHORT	IvOffset;			/* 0x080070BA : A/D transfer offset for iv							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IuInData;			/* 0x080070BC : iu input data (from A/D)							*/
	SHORT	IvInData;			/* 0x080070BE : iv input data (from A/D)							*/
	SHORT	IdInData;			/* 0x080070C0 : Id Input											*/
	SHORT	IqInData;			/* 0x080070C2 : Iq Input											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	CrFreqW;			/* 0x080070C4 : Carrier freq Now									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IuOut;				/* 0x080070C6 : Iu reference(for dead-time cmp)						*/
	SHORT	IvOut;				/* 0x080070C8 : Iv reference(for dead-time cmp)						*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	V1;					/* 0x080070CA : V1( = SQRT(VdOut^2+VqOut^2) )						*/
	SHORT	Vcent;				/* 0x080070CC : Central voltage										*/
	SHORT	Dummy1;				/* 0x080070CE : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		for LPF																						*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut1Lpf;			/* 0x080070D0 : LPF1 Output(Low)									*/
	DWREG	IqOut2Lpf;			/* 0x080070D4 : LPF2 Output(Low)									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqRef;				/* 0x080070D8 : Iq Reference after limit							*/
	SHORT	TLimP;				/* 0x080070DA : +tlim												*/
	SHORT	TLimM;				/* 0x080070DC : -tlim												*/
/*--------------------------------------------------------------------------------------------------*/
/*		for monitor																					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqMon;				/* 0x080070DE : IQ monitor											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IdDataP;			/* 0x080070E0 : 													*/
	SHORT	IqDataP;			/* 0x080070E2 : 													*/
	SHORT	KEangle;			/* 0x080070E4 : 機械角→電気角変換係数								*/
	SHORT	Dummy2;				/* 0x080070E6 : for Alignment										*/
	DWREG	IdLfil;				/* 0x080070E8 : L(di/dt)フィルタ									*/
	DWREG	IqLfil;				/* 0x080070EC : 													*/
/*--------------------------------------------------------------------------------------------------*/
/*		for axis q monitor																			*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IqDistIn;			/* 0x080070F0 : q軸外乱トルク入力									*/
	SHORT	IqDist;				/* 0x080070F2 : q軸外乱トルク										*/
	SHORT	IqMonFil;			/* 0x080070F4 : q軸電流指令モニタ(フィルタ後)						*/
	SHORT	IqOfRef;			/* 0x080070F6 : q軸電流指令(外乱トルク加算後)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter1																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut1L;			/* 0x080070F8 : filter1 output(Low)									*/
	DWREG	IqOut1PL;			/* 0x080070FC : filter1 output 1delay(Low)							*/
	DWREG	IqOut1PPL;			/* 0x080070100 : filter1 output 2delay(Low)							*/
	DWREG	IqIn1PL;			/* 0x08007104 : filter1 input 1delay(Low)							*/
	DWREG	IqIn1PPL;			/* 0x08007108 : filter1 input 2delay(Low)							*/
	SHORT	IQO1;				/* 0x0800710C : filter1 output(short type)							*/
	SHORT	Dummy3;				/* 0x0800710E : for Alignment										*/
	DWREG	IqOut1BufL;			/* 0x08007110 : filter1 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter2																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut2L;			/* 0x08007114 : filter2 output(Low)									*/
	DWREG	IqOut2PL;			/* 0x08007118 : filter2 output 1delay(Low)							*/
	DWREG	IqOut2PPL;			/* 0x0800711C : filter2 output 2delay(Low)							*/
	DWREG	IqIn2PL;			/* 0x08007120 : filter2 input 1delay(Low)							*/
	DWREG	IqIn2PPL;			/* 0x08007124 : filter2 input 2delay(Low)							*/
	SHORT	IQO2;				/* 0x08007128 : filter2 output(short type)							*/
	SHORT	Dummy4;				/* 0x0800712A : for Alignment										*/
	DWREG	IqOut2BufL;			/* 0x0800712C : filter2 output buffer(Low)							*/
/*--------------------------------------------------------------------------------------------------*/
/*		for notch filter3																			*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IqOut3L;			/* 0x08007130 : filter3 output(Low)									*/
	DWREG	IqOut3PL;			/* 0x08007134 : filter3 output 1delay(Low)							*/
	DWREG	IqOut3PPL;			/* 0x08007138 : filter3 output 2delay(Low)							*/
	DWREG	IqIn3PL;			/* 0x0800713C : filter3 input 1delay(Low)							*/
	DWREG	IqIn3PPL;			/* 0x08007140 : filter3 input 2delay(Low)							*/
	SHORT	IqOut3;				/* 0x08007144 : filter3 output(short type)							*/
	SHORT	Dummy5;				/* 0x08007146 : for Alignment										*/
	DWREG	IqOut3BufL;			/* 0x08007148 : filter3 output buffer(Low)							*/
} INTADV;

/*--------------------------------------------------------------------------------------------------*/
/*		Work Register for INT_AD																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INTADWK {
	SHORT	swk0;				/* 0x0800714C : 電流制御用16bitワークレジスタ0						*/
	SHORT	swk1;				/* 0x0800714E : 電流制御用16bitワークレジスタ1						*/
	SHORT	swk2;				/* 0x08007150 : 電流制御用16bitワークレジスタ2						*/
	SHORT	swk3;				/* 0x08007152 : 電流制御用16bitワークレジスタ3						*/
	SHORT	swk4;				/* 0x08007154 : 電流制御用16bitワークレジスタ4						*/
	SHORT	swk5;				/* 0x08007156 : 電流制御用16bitワークレジスタ5						*/
	SHORT	swk6;				/* 0x08007158 : 電流制御用16bitワークレジスタ6						*/
	SHORT	swk7;				/* 0x0800715A : 電流制御用16bitワークレジスタ7						*/
	SHORT	swk8;				/* 0x0800715C : 電流制御用16bitワークレジスタ8						*/
	SHORT	Dummy;				/* 0x0800715E : for Alignment										*/
	CSHORT*	pCtbl;				/* 0x08007160 : 電流制御用テーブルポインタ用ワークレジスタ			*/
	LONG	lwk0;				/* 0x08007164 : 電流制御用32bitワークレジスタ0						*/
	LONG	lwk1;				/* 0x08007168 : 電流制御用32bitワークレジスタ1						*/
	LONG	lwk2;				/* 0x0800716C : 電流制御用32bitワークレジスタ2						*/
	LONG	lwk4;				/* 0x08007170 : 電流制御用32bitワークレジスタ4						*/
	LONG	lwk6;				/* 0x08007174 : 電流制御用32bitワークレジスタ6						*/
	LONG	lwk8;				/* 0x08007178 : 電流制御用32bitワークレジスタ8						*/
	struct {
		USHORT	Low;			/* 0x0800717C : 引数下位16bit値										*/
		USHORT	High;			/* 0x0800717E : 引数上位16bit値										*/
		USHORT	uswk0;			/* 0x08007180 : 平方根演算用16bitワークレジスタ0					*/
		USHORT	uswk1;			/* 0x08007182 : 平方根演算用16bitワークレジスタ1					*/
		USHORT	uswk3;			/* 0x08007184 : 平方根演算用16bitワークレジスタ3					*/
		USHORT	uswk4;			/* 0x08007186 : 平方根演算用16bitワークレジスタ4					*/
		USHORT	uswk5;			/* 0x08007188 : 平方根演算用16bitワークレジスタ5					*/
		USHORT	uswk6;			/* 0x0800718A : 平方根演算用16bitワークレジスタ6					*/
		ULONG	ulwk0;			/* 0x0800718C : 平方根演算用32bitワークレジスタ0					*/
		ULONG	ulwk2;			/* 0x08007190 : 平方根演算用32bitワークレジスタ2					*/
		DWREG	tmp0;			/* 0x08007194 : 平方根演算用16/32bitワークレジスタ0					*/
	} sqrtwk;
/*--------------------------------------------------------------------------------------------------*/
} INTADWK;

/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DOBSP {
	SHORT	TsPerL;				/* 0x08007198 : Ts/L ( オブザーバ用パラメータ )						*/
	SHORT	Gobs;				/* 0x0800719A : g ( オブザーバの極 )								*/
	SHORT	RLTs;				/* 0x0800719C : 1-R･Ts/L ( オブザーバ用パラメータ )					*/
	SHORT	FilObsGain;			/* 0x0800719E : フィルタゲイン										*/
} DOBSP;

/*--------------------------------------------------------------------------------------------------*/
/*		for Current Observer																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DOBSV {
	DWREG	LpfIlq;				/* 0x080071A0 : q軸ローパスフィルタ変数								*/
	DWREG	HpfIlq;				/* 0x080071A4 : q軸ハイパスフィルタ変数								*/
	SHORT	IqObsOut;			/* 0x080071A8 : q軸オブザーバ出力									*/
	SHORT	IqObsFreq;			/* 0x080071AA : q軸振動成分											*/
	DWREG	LpfIld;				/* 0x080071AC : d軸ローパスフィルタ変数								*/
	DWREG	HpfIld;				/* 0x080071B0 : d軸ハイパスフィルタ変数								*/
	SHORT	IdObsOut;			/* 0x080071B4 : d軸オブザーバ出力									*/
	SHORT	IdObsFreq;			/* 0x080071B6 : d軸振動成分											*/
	SHORT	DmpGain;			/* 0x080071B8 : 電流オブザーバダンピングゲイン						*/
	SHORT	Dummy;				/* 0x080071BA : for Alignment										*/
} DOBSV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Phase Interpolate																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _PHASEV {
	SHORT	PHAL;				/* 0x080071BC : Phase(Low)											*/
	SHORT	PhaseH;				/* 0x080071BE : Phase(High)											*/
	SHORT	PhaseIp;			/* 0x080071C0 : 位相補間量											*/
	SHORT	PhaseIpF;			/* 0x080071C2 : 位相補間フラグ(0：しない、1：する)					*/
	SHORT	PhaseIpIn;			/* 0x080071C4 : 位相補間量(CPU→ASIC受け渡し用)						*/
	SHORT	PhaseIpFIn;			/* 0x080071C6 : 位相補間フラグ(CPU→ASIC受け渡し用)					*/
} PHASEV;

/*--------------------------------------------------------------------------------------------------*/
/*		for Encoder IF																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _ENCIFV {
	SHORT	AmpType;			/* 0x080071C8 : Rotary Type or Linear Type							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0x080071CA : for Alignment										*/
	DWREG	RcvPosX0;			/* 0x080071CC : Motor Encoder Position(受信位置)					*/
	DWREG	RcvPosX1;			/* 0x080071D0 : Motor Encoder Position(受信位置)					*/
	DWREG	RcvPosX2;			/* 0x080071D4 : Motor Encoder Position(受信位置)					*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	MotPosSftR;			/* 0x080071D8 : Shift Data for Motor Position Cal.					*/
	SHORT	MotPosSftX;			/* 0x080071DA : Shift Data for Motor Position Cal.					*/
	SHORT	MotPosSign;			/* 0x080071DC : Sign  Data for Motor Position Cal.					*/
	SHORT	DivOutSft;			/* 0x080071DE : 分周出力シフト数									*/
	DWREG	AccErrLv;			/* 0x080071E0 : Motor Encoder Acc. Error Check Low					*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	DivPls;				/* 0x080071E4 : 分周出力パルス										*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	DivOutGain;			/* 0x080071E8 : 分周出力ゲイン(リニア)								*/
	DWREG	DivPos;				/* 0x080071EC : 分周出力パルス(リニア)								*/
	DWREG	DivPlsRem;			/* 0x080071F0 : 分周出力パルス余り(リニア)							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	SPGFail;			/* 0x080071F4 : S-PG受信失敗許容回数								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	BitIprm;			/* 0x080071F6 : Bit Initial Parameter								*/
	SHORT	BitData;			/* 0x080071F8 : Bit Data											*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IncPlsReq;			/* 0x080071FA : PAO IncrePulse Output Request						*/
	SHORT	PAOSeqCmd;			/* 0x080071FC : PAO Serial Output Sequence							*/
	SHORT	PlsOSetCmd;			/* 0x080071FE : パルス出力回路初期化要求							*/
} ENCIFV;

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力関連：HostCPU --> Asic 定周期転送用											*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _DIVPLSV {
	SHORT	IncPlsReqIn;		/* 0x08007200 : PAO IncrePulse Output Request						*/
	SHORT	PAOSeqCmdIn;		/* 0x08007202 : PAO Serial Output Sequence							*/
	SHORT	PlsOSetCmdIn;		/* 0x08007204 : パルス出力回路初期化要求							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	DivSetIn;			/* 0x08007206 : 分周機能設定入力									*/
	SHORT	PoSet1In;			/* 0x08007208 : パルス変換原点補正1									*/
	SHORT	PoSet2In;			/* 0x0800720A : パルス変換原点補正2									*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	IncInitPlsIn;		/* 0x0800720C : 初期インクレパルス									*/
	DWREG	IncInitRemIn;		/* 0x08007210 : 初期インクレパルス余り								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	AccCntClrReq;		/* 0x08007214 : 加速度チェック開始カウントクリア要求				*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0x08007216 : for Alignment										*/
/*--------------------------------------------------------------------------------------------------*/
/*		Register for Library																		*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	Argu0;				/* 0x08007218 : Argument0											*/
	DWREG	Argu1;				/* 0x0800721C : Argument1											*/
	SHORT	Argu2;				/* 0x08007220 : Argument2											*/
	SHORT	Argu2H;				/* 0x08007222 : Argument2 High Word									*/
	DWREG	Ret0;				/* 0x08007224 : Return Register										*/
	DWREG	Kx;					/* 0x08007228 : kx													*/
	DWREG	Sx;					/* 0x0800722C : sx													*/
	DWREG	Iu0;				/* 0x08007230 : iu[0]												*/
	SHORT	Iu1;				/* 0x08007234 : iu[1]												*/
	SHORT	Iu1H;				/* 0x08007236 : iu[1] High Word										*/
} DIVPLSV;

/*--------------------------------------------------------------------------------------------------*/
/*		INT_ENC用ワーク																				*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _IENCWK {
	SHORT	EncWk0;				/* 0x08007238 : Encoder Interrup Work0 Lo							*/
	SHORT	EncWk0H;			/* 0x0800723A : Encoder Interrup Work0 Hi							*/
	SHORT	EncWk1;				/* 0x0800723C : Encoder Interrup Work1 Lo							*/
	SHORT	EncWk1H;			/* 0x0800723E : Encoder Interrup Work2 Hi							*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	RxPos;				/* 0x08007240 : Encoder Position Data Low							*/
	SHORT	RxFlg0;				/* 0x08007244 : エンコーダ受信チェックフラグ						*/
	SHORT	Dummy;				/* 0x08007246 : for Alignment										*/
} IENCWK;

/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス更新処理(@INT_HOST)用ワーク														*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _IHOSTWK {
	USHORT	Divuswk;			/* 0x08007248 : 追加(16/32bit兼用で使っていたワークレジスタの分離)	*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0x0800724A : for Alignment										*/
	LONG	DivWk0;				/* 0x0800724C : 分周パルス更新処理用 Work0							*/
	LONG	DivWk1;				/* 0x08007250 : 分周パルス更新処理用 Work1							*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	RxPos0;				/* 0x08007254 : Encoder Position Data Low							*/
	LONG	RcvPosX;			/* 0x08007258 : Motor Encoder Position Low							*/
	LONG	LastRcvPosX;		/* 0x0800725C : Last Motor Encoder Position Low						*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	DMotPos;			/* 0x08007260 : Delta Motor Position Low							*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	EncMstErrCnt;		/* 0x08007264 : Encoder Mst. Error Counter							*/
	SHORT	AccChkCnt;			/* 0x08007266 : 													*/
	LONG	MotAcc;				/* 0x08007268 : Motor Acc.											*/
/*--------------------------------------------------------------------------------------------------*/
	LONG	IncInitPls;			/* 0x0800726C : 初期インクレパルス出力								*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	DivSetW;			/* 0x08007270 : 分周機能設定										*/
	SHORT	PoSet1W;			/* 0x08007272 : パルス変換原点補正1									*/
	SHORT	PoSet2W;			/* 0x08007274 : パルス変換原点補正2									*/
/*--------------------------------------------------------------------------------------------------*/
	USHORT	uswk;				/* 0x08007276 : ホスト割込み用16bitワークレジスタ0					*/
	SHORT	swk0;				/* 0x08007278 : ホスト割込み用16bitワークレジスタ1					*/
	SHORT	swk1;				/* 0x0800727A : ホスト割込み用16bitワークレジスタ1					*/
	LONG	lwk1;				/* 0x0800727C : ホスト割込み用32bitワークレジスタ0					*/
	LONG	lwk2;				/* 0x08007280 : ホスト割込み用32bitワークレジスタ1					*/
	LONG	lwk3;				/* 0x08007284 : ホスト割込み用32bitワークレジスタ2					*/
} IHOSTWK;

/*--------------------------------------------------------------------------------------------------*/
/*		Register for Library																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _IPFBWK {
	LONG	lswk1;				/* 0x08007288 : wk1 for Library										*/
	LONG	lswk2;				/* 0x0800728C : wk2 for Library										*/
	LONG	lswk3;				/* 0x08007290 : wk3 for Library										*/
	LONG	lswk4;				/* 0x08007294 : wk4 for Library										*/
	LONG	lswk10;				/* 0x08007298 : wk10 for Library									*/
	LONG	lswk11;				/* 0x0800729C : wk11 for Library									*/
	DLREG	dlwk;				/* 0x080072A0 : 64bit計算用ワークレジスタ							*/
} IPFBWK;

/*--------------------------------------------------------------------------------------------------*/
/*		for Weaken Field																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _WEAKFV {
	SHORT	WfKpVLIn;			/* 0x080072A8 : 電圧FB比例ゲイン(下位16bit)							*/
	SHORT	WfKpVHIn;			/* 0x080072AA : 電圧FB比例ゲイン(上位16bit)							*/
	SHORT	WfKiVLIn;			/* 0x080072AC : 電圧FB積分ゲイン(下位16bit)							*/
	SHORT	WfKiVHIn;			/* 0x080072AE : 電圧FB積分ゲイン(上位16bit)							*/
	SHORT	WfV1MaxIn;			/* 0x080072B0 : 電圧指令制限レベル									*/
	SHORT	WfIdRefLimIn;		/* 0x080072B2 : d軸電流指令リミット									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	WfIntegLim;			/* 0x080072B4 : 電圧FB積分リミット									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	IdOut;				/* 0x080072B6 : Id reference										*/
	SHORT	IqOut;				/* 0x080072B8 : Iq reference										*/
	SHORT	Vel;				/* 0x080072BA : Velocity (omega)									*/
/*--------------------------------------------------------------------------------------------------*/
	DWREG	WfKpV;				/* 0x080072BC : 電圧FB比例ゲイン(下位16bit)							*/
	DWREG	WfKiV;				/* 0x080072C0 : 電圧FB積分ゲイン(下位16bit)							*/
	SHORT	WfV1Max;			/* 0x080072C4 : 電圧指令制限レベル									*/
	SHORT	WfIdRefLim;			/* 0x080072C6 : d軸電流指令リミット									*/
	SHORT	WfVqMax;			/* 0x080072C8 : q軸電圧指令リミット									*/
/*--------------------------------------------------------------------------------------------------*/
	SHORT	Dummy;				/* 0x080072CA : for Alignment										*/
	DWREG	WfIntgl;			/* 0x080072CC : 電圧FB積分値										*/
	SHORT	WfVdRef;			/* 0x080072D0 : d軸電圧指令											*/
	SHORT	WfVqRef;			/* 0x080072D2 : q軸電圧指令											*/
} WEAKFV;

/*--------------------------------------------------------------------------------------------------*/
/*		Version define register																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _VER_INFO {
	SHORT	MswVer;				/* 0x080072D4 : ソフトバージョン情報								*/
	SHORT	TstVer;				/* 0x080072D6 : テストバージョン情報								*/
	SHORT	YspVer;				/* 0x080072D8 : Ｙ仕様バージョン情報								*/
	SHORT	Dummy;				/* 0x080072DA : for Alignment										*/
} VER_INFO;

/*--------------------------------------------------------------------------------------------------*/
/*		Axis information register																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _AXIS_INFO {
	LONG	AxisNum;			/* 0x080072DC : 制御軸数											*/
} AXIS_INFO;

/*--------------------------------------------------------------------------------------------------*/
/*		Common work register																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _COMWORKS {
	SHORT	WREG82;				/* 0x080072E0 : Work  Register 82									*/
	SHORT	WREG83;				/* 0x080072E2 : Work  Register 83									*/
	SHORT	WREG84;				/* 0x080072E4 : Work  Register 84									*/
	SHORT	WREG85;				/* 0x080072E6 : Work  Register 85									*/
	SHORT	WREG86;				/* 0x080072E8 : Work  Register 86									*/
	SHORT	WREG87;				/* 0x080072EA : Work  Register 87									*/
	SHORT	WREG88;				/* 0x080072EC : Work  Register 88									*/
	SHORT	WREG89;				/* 0x080072EE : Work  Register 89									*/
	SHORT	WREG95;				/* 0x080072F0 : Work  Register 95									*/
	SHORT	WREG100;			/* 0x080072F2 : Work  Register 100									*/
	SHORT	WREG101;			/* 0x080072F4 : Work  Register 101									*/
	SHORT	WREG102;			/* 0x080072F6 : Work  Register 102									*/
	SHORT	WREG103;			/* 0x080072F8 : Work  Register 103									*/
	SHORT	WREG104;			/* 0x080072FA : Work  Register 104									*/
	SHORT	WREG109;			/* 0x080072FC : Work  Register 109									*/
	SHORT	Dummy;				/* 0x080072FE : for Alignment										*/
} COMWORKS;

typedef struct _HOST_WOKS {
	DWREG	HTMP0;				/* 0x08007300 : HOST Interrupt Temp0								*/
	DWREG	HTMP2;				/* 0x08007304 : HOST Interrupt Temp2								*/
	DWREG	HTMP4;				/* 0x08007308 : HOST Interrupt Temp4								*/
	DWREG	HTMP6;				/* 0x0800730C : HOST Interrupt Temp6								*/
	SHORT	HTMP7;				/* 0x08007310 : HOST Interrupt Temp7								*/
	SHORT	HTMP8;				/* 0x08007312 : HOST Interrupt Temp8								*/
} HOST_WOKS;

typedef struct _ROUND_WORKS {
	SHORT	WREG140;			/* 0x08007314 : Work Register 140									*/
	SHORT	WREG141;			/* 0x08007316 : Work Register 141									*/
	SHORT	WREG142;			/* 0x08007318 : Work Register 142									*/
	SHORT	WREG143;			/* 0x0800731A : Work Register 143									*/
	SHORT	WREG144;			/* 0x0800731C : Work Register 144									*/
	SHORT	WREG145;			/* 0x0800731E : Work Register 145									*/
	SHORT	WREG146;			/* 0x08007320 : Work Register 146									*/
	SHORT	WREG147;			/* 0x08007322 : Work Register 147									*/
	SHORT	WREG148;			/* 0x08007324 : Work Register 148									*/
	SHORT	WREG149;			/* 0x08007326 : Work Register 149									*/
} ROUND_WORKS;

typedef struct _AD_WORKS {
	DWREG	TMP0;				/* 0x08007328 : AD Interrupt Temp0									*/
	DWREG	TMP2;				/* 0x0800732C : AD Interrupt Temp2									*/
	DWREG	TMP4;				/* 0x08007330 : AD Interrupt Temp4									*/
	DWREG	TMP6;				/* 0x08007334 : AD Interrupt Temp6									*/
	DWREG	TMP8;				/* 0x08007338 : AD Interrupt Temp8									*/
} AD_WORKS;

/*--------------------------------------------------------------------------------------------------*/
/*		初期化処理用ワーク																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef struct _INITWK {
	SHORT	IN_WK0;				/* 0x0800733C : 初期化処理用 Work0 Lo								*/
	SHORT	IN_WK0H;			/* 0x0800733E : 初期化処理用 Work0 Hi								*/
	SHORT	IN_WK1;				/* 0x08007340 : 初期化処理用 Work1 Lo								*/
	SHORT	IN_WK1H;			/* 0x08007342 : 初期化処理用 Work1 Hi								*/
} INITWK;




/****************************************************************************************************/
/*		Axis Control Structure definition															*/
/****************************************************************************************************/
typedef struct {
	ACRV			AcrV;		/* 電流制御用変数													*/
	STSFLG			StsFlg;		/* ステータスフラグ													*/
	VCMPV			VcmpV;		/* 電圧補償演算用変数												*/
	SINTBL			SinTbl;		/* sinテーブル														*/
	ADSTOP			AdStop;		/* 電流制御停止エラー検出用構造体									*/
	ADINV			AdinV;		/* ホストCPU受け渡し変数											*/
	INTADP			IntAdP;		/* 電流割込みパラメータ												*/
	INTADV			IntAdV;		/* 電流割込み変数													*/
	INTADWK			IntAdwk;	/* 電流割込みワーク													*/
	DOBSP			DobsP;		/* d軸オブザーバ用パラメータ										*/
	DOBSV			DobsV;		/* d軸オブザーバ用変数												*/
	PHASEV			PhaseV;		/* 位相演算用変数													*/
	ENCIFV			EncIfV;		/* エンコーダインタフェース変数										*/
	DIVPLSV			DivPlsV;	/* 分周パルス用変数													*/
	IENCWK			IEncWk;		/* エンコーダ割込みワーク											*/
	IHOSTWK			IHostWk;	/* ホスト割込みワーク												*/
	IPFBWK			IPfbwk;		/* 余り付き位置FB計算												*/
	WEAKFV			WeakFV;		/* 弱め界磁用変数													*/
	SHORT			Dummy[274];	/* Dummy buffer for Axis offset										*/
	SVIP_READ_REG	*SvIpRegR;	/* Servo IP H/W Read Register Address								*/
	SVIP_WRITE_REG	*SvIpRegW;	/* Servo IP H/W Write Register Address								*/
} MICRO_AXIS_HANDLE;

extern	MICRO_AXIS_HANDLE	AxisHdl[2];
extern	LONG		ZEROR;
extern	LONG		ONER;
extern	LONG		NONER;
extern	LONG		True;
extern	LONG		False;
extern	VER_INFO	VerInfo;
extern	AXIS_INFO	AxisInfo;



/*--------------------------------------------------------------------------------------------------*/







/***************************************** end of file **********************************************/
