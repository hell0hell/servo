#ifndef	_PCMD_FILTER_H /* 再読み込み防止用 */
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PcmdFilter.h : 位置指令フィルタ演算用ヘッダーファイル										*/
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
/*	Note	:	初版	2010.11.11	Y.Oka	for INGRAM												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "Mlib.h"


/****************************************************************************************************/
/*																									*/
/*		定数定義																					*/
/*																									*/
/****************************************************************************************************/
#define MAFIL_BUFNUM			256						/* 移動平均フィルタバッファ個数				*/
#define	SUMX_LIM				0x3FFFFFFF				/* ExpFil.V.Pexfvar.sumx：オーバフロー閾値	*/
#define	MAXOMEGA2(AvfCycleNs)	(127323954/AvfCycleNs)	/* 127323954 = 10^9 * 0.8 / (2*PAI)			*/
														/*						周波数2最大値[Hz]	*/


/****************************************************************************************************/
/*																									*/
/*		Variables Definition																		*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		指数加減速フィルタ用パラメータ変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _EXPFIL {					/* 指数加減速フィルタ用パラメータ＆変数定義				*/
	struct	{								/* 指数加減速フィルタ用パラメータ						*/
		LONG	Kf[2];							/* 指数加減速フィルタゲイン							*/
		LONG	Pbias[2];						/* 指数加減速フィルタバイアス						*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* 指数加減速フィルタ用変数データ						*/
		LONG	Filo;							/* フィルタ出力										*/
		PEXFV	Pexfvar;						/* 指数加減速フィルタ演算用変数構造体				*/
		BOOL	RefZStatus;						/* 払い出し完了状態									*/
	} var;
} EXPFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		移動平均フィルタ用パラメータ変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _MAFIL {						/* 移動平均フィルタ用パラメータ＆変数定義				*/
	struct	{								/* 移動平均フィルタ用パラメータ							*/
		LONG	PmafNum[2];						/* 移動平均回数										*/
		UCHAR	ImafSft[2];						/* 補間移動平均回数(2^Sft)							*/
		UCHAR	SpareByte[2];					/* 予備												*/
		/*----------------------------------------------------------------------------------------------*//*<S0C3>*/
		LONG	PmafNumSec[2];						/* 移動平均2回数										*/
		UCHAR	ImafSftSec[2];						/* 補間移動平均2回数(2^Sft)							*/
		UCHAR	SpareByteSec[2];					/* 予備2												*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct	{								/* 移動平均フィルタ用変数データ							*/
		LONG	PcmdInX;						/* 長時間補間移動平均フィルタ演算用					*/
		LONG	PcmdInY;						/* 長時間補間移動平均フィルタ演算用					*/
		LONG	Index;							/* 補間移動平均フィルタ演算用						*/
		LONG	Filo;							/* フィルタ出力										*/
		BOOL	RefZStatus;						/* 払い出し完了状態									*/
		PMAFV	Pmafvar;						/* 移動平均フィルタ演算用変数構造体					*/
		PIMFV	Pimfvar;						/* 補間移動平均フィルタ演算用変数構造体				*/
		LONG	Pmafbuf[ MAFIL_BUFNUM ];		/* 移動平均フィルタ演算用バッファ					*/
		/*----------------------------------------------------------------------------------------------*//*<S0C3>*/
		LONG	PcmdInXSec;						/* 長時間補間移動平均フィルタ2演算用					*/
		LONG	PcmdInYSec;						/* 長時間補間移動平均フィルタ2演算用					*/
		LONG	IndexSec;							/* 補間移動平均フィルタ2演算用						*/
		LONG	FiloSec;							/* フィルタ2出力										*/
		HIGHFV	PmafvarSec;						/* 移動平均フィルタ2演算用変数構造体					*/
		HIMFV	PimfvarSec;						/* 補間移動平均フィルタ2演算用変数構造体				*/
		LONG	PmafbufSec[ MAFIL_BUFNUM ];		/* 移動平均フィルタ2演算用バッファ					*/
	} var;
} MAFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		位置指令フィルタ用変数定義																	*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct _PCMDFIL {					/* 位置指令フィルタ演算用パラメータ＆変数定義			*/
	EXPFIL	ExpFil;								/* 加減速フィルタ用変数								*/
	MAFIL	MaFil;								/* 移動平均フィルタ用変数							*/
} PCMDFIL;


/*--------------------------------------------------------------------------------------------------*/
/*		振動抑制フィルタ用パラメータ変数定義														*/
/*--------------------------------------------------------------------------------------------------*/
typedef	struct VIBSUPFILPRM {					/* 振動抑制フィルタ用パラメータ定義					*/
	LONG			Kff1;							/* 振動抑制フィルタゲイン1						*/
	LONG			Kff2;							/* 振動抑制フィルタゲイン2						*/
	LONG			Kff3;							/* 振動抑制フィルタゲイン3						*/
	BOOL			enable;							/* 振動抑制フィルタ機能選択						*/
} VIBSUPFILPRM;

typedef	struct VIBSUPFIL {						/* 振動抑制フィルタ用パラメータ＆変数定義			*/
	struct {									/* 振動抑制フィルタ用パラメータ定義					*/
		VIBSUPFILPRM	VibSupPrm;					/* 振動抑制フィルタ用パラメータ					*/
		VIBSUPFILPRM	Pexe;						/* 実行用振動抑制フィルタ用パラメータ			*/
		BOOL			VibSupSetting;				/* 振動抑制フィルタ設定							*/
	} conf;
	/*----------------------------------------------------------------------------------------------*/
	struct {									/* 振動抑制フィルタ用変数データ定義					*/
		LONG			rem1;						/* 振動抑制フィルタ演算用						*/
		LONG			rem2;						/* 振動抑制フィルタ演算用						*/
		LONG			rem3;						/* 振動抑制フィルタ演算用						*/
		LONG			wkbf1;						/* 振動抑制フィルタ演算用						*/
		LONG			wkbf2;						/* 振動抑制フィルタ演算用						*/
		LONG			FilioErr[2];				/* 振動抑制フィルタ入出力間偏差					*/
		LONG			Buf;						/* 振動抑制フィルタ溜まり						*/
		LONG			Filo;						/* 振動抑制フィルタ出力							*/
	} var;
} VIBSUPFIL;



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	PcmdFilInitialize( PCMDFIL *PcmdFil, VIBSUPFIL *VibSupFil );
void	FFCmdFilInitialize( MAFIL *pMaFil );									/* <S1D0> */
LONG	FFcmdFilMoveAverageFilter( MAFIL *pMaFil, LONG FFcmdIn, BOOL FilStop );	/* <S1D0> */
LONG	PcmdFilRuntimeService( PCMDFIL *PcmdFil, LONG PcmdIn, LONG FilRest, BOOL FilStop );
/*--------------------------------------------------------------------------------------------------*/
void	PcmdFilCalculatePrmExpFilter( EXPFIL *ExpFil, USHORT PrmTime, KSGAIN KMotSpdConv,
													 UCHAR MaxVel, LONG SvCycleUs, LONG Flag );
void	PcmdFilCalculatePrmExpFilBias( EXPFIL *ExpFil, USHORT expbias1,
												LONG Egear_a, LONG Egear_b, LONG SvCycleUs );
void	PcmdFilCalculatePrmMoveAvergeFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad,
		                                      LONG SvCycleUs, LONG Flag );/*<S0C3>*/
void    PcmdFilCalculatePrmHighFilter( MAFIL *MaFil, USHORT PrmTime, USHORT Highrad,
		                                      LONG SvCycleUs, LONG Flag );/*<S0C3>*/
/*--------------------------------------------------------------------------------------------------*/
void	PcmdFilSetRefZStatus( PCMDFIL *PcmdFil, BOOL status );
BOOL	PcmdFilGetRefZStatus( PCMDFIL *PcmdFil );
/*--------------------------------------------------------------------------------------------------*/
LONG	PcmdFilVibSupFilter( VIBSUPFIL *VibSupFil, LONG dPcmd, BOOL *RefZSignal );
BOOL	PcmdFilCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw,
												ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs );
void	PcmdFilCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil, LONG ScanTimeNs );


#define _PCMD_FILTER_H /* 再読み込み防止用 */
#endif /* _PCMD_FILTER_H */
/***************************************** end of file **********************************************/
