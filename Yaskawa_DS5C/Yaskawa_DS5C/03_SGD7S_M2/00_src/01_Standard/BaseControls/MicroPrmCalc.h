/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MicroScanAIf.c : μプログラムScanA I/F処理定義												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : μプログラムのScanAとCPU間のI/F処理														*/
/*																									*/
/*		μプログラム－CPU間のデータ授受を行う。														*/
/*																									*/
/*																									*/
/*																									*/
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
#ifndef MICRO_PRM_CALC_H
#define MICRO_PRM_CALC_H


#include "JL086ServoIpReg.h"
#include "Bprm.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define		MICRO_TR_READY			0		/* マイクロプログラム応答(READY状態)					*/
#define		MICRO_TR_SIZE_SHORT		1		/* マイクロパラメータ一括転送用書き込みデータサイズ		*/
#define		MICRO_TR_SIZE_LONG		2		/* マイクロパラメータ一括転送用書き込みデータサイズ		*/
#define		MICRO_TR_DATA_MAX		32		/* マイクロパラメータ一括転送用書き込みデータ数[WORD]	*/
#define		MICRO_TR_DATA_ERROR		-1		/* マイクロパラメータ一括転送エラーステータス			*/
#define		MICRO_TR_SUCCESS		0		/* マイクロパラメータ一括転送データ設定完了				*/
#define		MICRO_ADDRESS_CHANGE	0x0fff	/* マイクロアドレス変更用								*/
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
/*		弱め界磁制御用変数定義																		*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct WEAKENFIELD {
	struct {						/* 弱め界磁用定数データ定義										*/
		KSGAIN	KVdcAsic;				/* 主回路検出値換算ゲイン [Admax] ⇒ [8192/Vdc]				*/
		KSGAIN	KpvBeforeDiv;			/* 電圧フィードバックループゲイン計算用定数(速度で割る前)	*/
		KSGAIN	KivBeforeDiv;			/* 電圧フィードバックループ積分ゲイン計算用定数(速度で割る前)*/
		LONG	IdrefLimTerm1;			/* d軸電流指令リミット値計算用定数(速度^2で割る前)			*/
		LONG	IdrefLimTerm23;			/* d軸電流指令リミット値計算用定数(速度に関係のない項)		*/
		LONG	IdrefLimTerm4;			/* d軸電流指令リミット値計算用定数(速度で割る前)			*/
		SHORT	V1Max;					/* 電圧指令制限レベル										*/
		SHORT	dummy1;					/* for alignment											*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {						/* 弱め界磁用変数データ定義										*/
		LONG	Kpv;					/* 電圧フィードバックループゲイン(速度で割った後)			*/
		LONG	Kiv;					/* 電圧フィードバックループ積分ゲイン(速度で割った後)		*/
		LONG	VdcSum;					/* 主回路電圧積算値											*/
		SHORT	IdrefLim;				/* d軸電流指令リミット値									*/
		SHORT	KpvL;					/* Kpvの下位16bit											*/
		SHORT	KpvH;					/* Kpvの上位16bit											*/
		SHORT	KivL;					/* Kivの下位16bit											*/
		SHORT	KivH;					/* Kivの上位16bit											*/
		SHORT	VdcAsic;				/* 主回路電圧平均値											*/
		SHORT	VdcAvgCnt;				/* 主回路電圧平均回数										*/
		SHORT	VdcSumCnt;				/* 主回路電圧積算回数										*/
	} var;
} WEAKENFIELD;


/*--------------------------------------------------------------------------------------------------*/
/*		マイクロ電圧補償ゲイン機能用変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct MPKVVCOMP {
	struct {						/* マイクロ電圧補償ゲイン機能用定数データ定義					*/
		LONG	CompLevel;				/* 主回路電圧補正レベル										*/
		LONG	KdMpKvv;				/* Calculate Gain for MpKvv									*/
		LONG	KfMpKvv;				/* Filter Gain for MpKvv									*/
		BOOL	MpKvvFlg;				/* 主回路電圧補正機能選択	[0:OFF,1:ON]					*/
		USHORT	vrevgn;					/* 電圧補正ゲイン											*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {						/* マイクロ電圧補償ゲイン機能用変数データ定義					*/
		SHORT	MpKvv;					/* 電圧補償ゲイン指令			[8192/VdcBase]				*/
		SHORT	Dummy;					/* Dummy for Alignment										*/
	} var;
} MPKVVCOMP;


/* Input Parameter of "KpxPcalMicroProgram()" */
typedef struct MAICRO_CFGPRM {
	USHORT	imaxs;					/* PnE15 : サーボパック最大電流									*/
	USHORT	ignd;					/* PnE20 : 電流ループゲイン(D)									*/
	USHORT	ignq;					/* PnE21 : 電流ループゲイン(Q)									*/
	USHORT	kid;					/* PnE22 : 電流ループ積分時定数(D)								*/
	USHORT	kiq;					/* PnE23 : 電流ループ積分時定数(Q)								*/
	USHORT	limintd;				/* PnE24 : 電流ループ積分制限値(D)								*/
	USHORT	limintq;				/* PnE25 : 電流ループ積分制限値(Q)								*/
	USHORT	ifil;					/* PnE26 : 電流ループフィルタ時定数								*/
	USHORT	shant;					/* PnE28 : 電流検出ゲイン										*/
	USHORT	pwmf;					/* PnE2C : ＰＷＭ周波数											*/
	USHORT	dq_sw;					/* PnE2F : ＤＱ用スイッチ										*/
	USHORT	vrevgn;					/* PnE2A : 電圧補正ゲイン										*/
	USHORT	lmdata;					/* PnE2B : 電圧制限値											*/
	DBYTEX	odt_c;					/* PnE2D : 下位:オンディレイ時間, 上位:オンディレイ補償定数		*/
	DBYTEX	odlv_slp;				/* PnE2E : 下位:オンディレイ補償変更レベル, 上位:補償傾き		*/
	USHORT	motresq;				/* PnE3B : モータ共振抑制用ノッチフィルタQ値					*/
	USHORT	fltrange;				/* PnE3F : モータ過電流検出フィルタ回数							*/	/* <S1F5> */
	USHORT	zadjiu;					/* PnE50 : 電流検出ゼロ調(U相)									*/
	USHORT	zadjiv;					/* PnE51 : 電流検出ゼロ調(V相)									*/
	DBYTEX	gadjiu_v;				/* PnE52 : 電流検出ゲイン調(U,V相)								*/
	USHORT	pnvoltgn;				/* PnE70 : P-N電圧検出レベル									*/
	USHORT	pwmf2;					/* PnEA5 : 静音モードPWM周波数									*/
	USHORT	ilimlvl;				/* PnEA6 : キャリア周波数切り替え電流閾値						*/
	USHORT	pwmf3;					/* PnEA7 : 第2PWM周波数											*/
	USHORT	idlim;					/* PnEB2 : 電圧フィードバック積分リミット						*/
	DBYTEX	oslv_pol;				/* PnF06 : ＯＳ検出レベル, ポール数								*/
	USHORT	irat;					/* PnF09 : 定格電流												*/
	USHORT	imax;					/* PnF0A : 瞬時最大電流											*/
	USHORT	motresfrq;				/* PnF1C : モータ共振抑制用ノッチフィルタ周波数					*/
	USHORT	flg_wf;					/* PnF1E : フラグ												*/
	MFUNCF	motfuncf;				/* PnF24 : モータ機能フラグ2									*//*モータキャリア周波数切り替え<S117>*/
	USHORT	mot_pwmf;				/* PnF25 : モータPWM周波数										*//*モータキャリア周波数切り替え<S117>*/
#if 0	/* キャリア周波数の統一は行なわないため不要<S11E> */
	BOOL	motCarrUse;				/* モータキャリア(PnF25)有効 	[0:OFF,1:ON]					*//*モータキャリア周波数切り替え<S117>*/
#endif		/*#if 0	 キャリア周波数の統一は行なわないため不要<S11E> */
	SHORT	NormCarrCnt;			/* 通常キャリアカウント値		[-]								*//*モータキャリア周波数切り替え<S117>*/
	SHORT	HighCarrCnt;			/* 高キャリアカウント値			[-]								*//*モータキャリア周波数切り替え<S117>*/
} MAICRO_CFGPRM;


/* Input Parameter of "PcalMicroDqAcrGain()"  */
typedef struct DQACR_GAIN_PRM {
	USHORT	ignd;					/* PnE20 : 電流ループゲイン(D)									*/
	USHORT	ignq;					/* PnE21 : 電流ループゲイン(Q)									*/
	USHORT	kid;					/* PnE22 : 電流ループ積分時定数(D)								*/
	USHORT	kiq;					/* PnE23 : 電流ループ積分時定数(Q)								*/
	USHORT	ifil;					/* PnE26 : 電流ループフィルタ時定数								*/
	USHORT	ifil2;					/* PnEA4 : 静音モード電流ループフィルタ時定数					*/
} DQACR_GAIN_PRM;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/

LONG	KpxPcalMicroProgram( MAICRO_CFGPRM *uCfgPrm, BPRMDAT *Bprm,
							 ASICS *SvAsicRegs, WEAKENFIELD* WeakenField );
LONG	KpxMotorOCLevel( MAICRO_CFGPRM *uCfgPrm, ASICS *SvAsicRegs, MENCPRM *MencP );	/*<S1F5>*/
void	KpiPcalMicroNotchFilter2( ASICS *SvAsicRegs, LONG hz, LONG qx, LONG dx, SHORT *MpReg );
LONG	KpiChangePwmFrequency( ULONG PwmFx, USHORT pwmf, USHORT pwmf3, USHORT flg_wf );
void	KpiManageMicroTorqueFilter( VUSHORT *mreg_csw_ptr, USHORT *CswData, USHORT FilSel, LONG FilOn );
LONG	LpxSetPrmToASIC( SHORT *adr, LONG prm, BOOL limchk );
void	PcalMicroDqAcrGain(	ASIC_U_IF_STR	*pAsicMicroReg,	/* 制御変数ポインタ						*/
							DQACR_GAIN_PRM	*Prm,			/* パラメータデータポインタ				*/
							BPRMDAT			*Bprm,			/* ベースパラメータポインタ 			*/
							ULONG			Loophz,			/* 速度ループゲイン			[0.1Hz]		*/
							ULONG			ratio);			/* ゲインバランス			[%]			*/

LONG	PcalDqAcrKp( LONG Gain, LONG Derate, BPRMDAT *Bprm, LONG axis_type );
LONG	PcalDqAcrKi( LONG Ti, LONG Kp, KSGAIN MotL, BPRMDAT *Bprm );
LONG	PcalDqAcrFilter( LONG Tf );
#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
LONG	MicroTranslatePrmReq( ASICS *SvAsicRegs, void *Data, VUSHORT *Address, LONG Size );
#endif


void PcmKvvIniPrmCalc( MPKVVCOMP *MpKvvComp, BPRMDAT *Bprm,
					   USHORT kvvfil, USHORT pnvoltgn, USHORT vrevgn, BOOL enable );
void	PcmWFPrmCalc( WEAKENFIELD *wf_ptr, LONG MotSpd, LONG DcVolt );		/* <S024> */
void	PcmMicroKvvCalc( MPKVVCOMP *MpKvvComp, LONG DcVolt, USHORT UvLevel );
void	PcalVFBKp( WEAKENFIELD *WeakenField, ASICS *SvAsicRegs, BPRMDAT *Bprm, USHORT kv, USHORT tv );
void	PcalVFBKi( WEAKENFIELD *WeakenField, ASICS *SvAsicRegs, BPRMDAT *Bprm, USHORT kv);
void	PcalIdrefLimTerm1( WEAKENFIELD *WeakenField, ASICS *SvAsicRegs, BPRMDAT *Bprm, LONG v1max, LONG vmaxid);
void	PcalIdrefLimTerm234(WEAKENFIELD* weaken_ptr, ASICS *SvAsicRegs, BPRMDAT* Bprm, DBYTEX oslv_pol, USHORT pnvoltgn);


#endif /* MICRO_PRM_CALC_H */
/***************************************** end of file **********************************************/
