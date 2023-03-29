/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControlPrmCalc.h : BaseControl用パラメータ計算ヘッダーファイル							*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.09.28	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_BASECTRL_PRM_CALC_H
#define _BASECTRL_PRM_CALC_H


#include "BaseControls.h"


/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
typedef struct BASE_SPDLOOP_PRM {
	USHORT	jrate;
	USHORT	loophz;					/* Pn100 : 速度ループゲイン										*/
	USHORT	pitime;					/* Pn101 : 速度ループ積分時定数									*/
	USHORT	loophz2;				/* Pn104 : 第２速度ループゲイン									*/
	USHORT	pitime2;				/* Pn105 : 第２速度ループ積分時定数								*/
	DBYTEX	ipg_rwt;				/* PnE44 : 下位:i-P制御割合, 上位:暴走検出トルク				*/
} BASE_SPDLOOP_PRM;

void	PcalBaseSpeedControl(
			BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, BASE_SPDLOOP_PRM *Prm, LONG GselNo );

void	PcalBasePositionPControl(
			BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, EGEAR *Egear, ULONG Posgn, LONG GselNo );

void	PcalZeroPositionControl(
			BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, LONG Posgn, LONG GselNo );

void	PcalSpeedLimitGain(
			BASE_CTRL_HNDL *BaseCtrlData, BPRMDAT *Bprm, USHORT jrate, ULONG Loophz, LONG GselNo );

void	PcalBaseTrqLpassFilter(
			TRQFIL *TrqFilData, BASE_CTRL_HNDL *BaseCtrlData, LONG trqfil11, LONG trqfil12, LONG GselNo );

void	PcalBaseTrqLpassFilter2(
			TRQFIL *TrqFilData, BASE_CTRL_HNDL *BaseCtrlData, USHORT secfil, USHORT secq );

LONG	PcalLowfilter2( LONG hz, LONG qx, LONG ts, KSGAIN kn[4] );
#if (FLOAT_USE==TRUE)
void	f_PcalLowfilter2(								/* ２次ローパスフィルタパラメータ計算			*/
			float	hz,									/* フィルタ周波数		[Hz]				*/
			float	qx,									/* クォリティファクタ	[-]				*/
			float	ts,									/* 演算周期				[sec]				*/
			float	kn[4] );							/* 計算結果格納								*/
#endif /* FLOAT_USE */
KSGAIN	PcalBaseTrqLpassFilter3( LONG Trqfil, LONG ScanTime );

KSGAIN	PcaldPosRefFFGain( KSGAIN Kpx, LONG ffgn, LONG ScanTime );

#if (FLOAT_USE==TRUE)
float	f_BpxNorObsGainCal( float ObsGain , float scantime );
float	f_BpxNorObsJGainCal( KSGAIN Kvx, float jrate, float ObsJGain , float scantime );
#endif /* FLOAT_USE */
LONG	BpxNorObsGainCal( LONG ObsGain , LONG scantime );
LONG	BpxNorObsJGainCal( KSGAIN Kvx, LONG jrate, LONG ObsJGain , LONG scantime );



/***************************************** end of file **********************************************/
#endif /* _BASECTRL_PRM_CALC_H */
