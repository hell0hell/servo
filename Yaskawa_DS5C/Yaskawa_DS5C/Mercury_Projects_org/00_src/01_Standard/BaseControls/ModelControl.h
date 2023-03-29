#ifndef MODEL_CONTROL_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ModelControl.h : ダイレクトトルクリファレンス方式モデル追従制御関連処理定義					*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MfcHandler.h"
#include "Bprm.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
//#define	MFCONEMASSCYCLE		KPI_SBCYCLENS			/* 剛体ＭＦＣ演算ｻｲｸﾙﾀｲﾑ(ScanB))		[ns]	*/
#define	MFCTWOMASSCYCLE(CycleTimeNs)	(CycleTimeNs << 1)	/* 二慣性ＭＦＣ演算ｻｲｸﾙﾀｲﾑ(ScanB×2)	[ns]	*/
#define	C10POW9				1000000000				/* 10^9											*/
#define	E9C2PAI2			25330296				/* 10^9 /(2*PAI)^2								*/

/*--------------------------------------------------------------------------------------------------*/
#define SGDV_TYPE		0x00						/* SGDVタイプ									*/
#define SGDX_TYPE		0x01						/* SGDX互換モード								*/
/*--------------------------------------------------------------------------------------------------*/
#define ONE_MASS		0x01						/* 剛体系モデル									*/
#define TWO_MASS1		0x02						/* ２慣性系系モデル１							*/
#define TWO_MASS2		0x03						/* ２慣性系系モデル２							*/
#define ONE_BASE		0x04						/* 剛体系＋機台モデル							*/


typedef struct MFC_CFG_PRM {
	USHORT	mdlsw;				/* Pn140 : モデル追従制御関連スイッチ								*/
	USHORT	mdlgn;				/* Pn141 : モデル追従制御ゲイン [0.1/s]								*/
	USHORT	mdlzeta;			/* Pn142 : モデル追従制御ゲイン補正									*/
	USHORT	mdlptff;			/* Pn143 : モデル追従制御バイアス(正転方向)							*/
	USHORT	mdlntff;			/* Pn144 : モデル追従制御バイアス(逆転方向)							*/
	USHORT	mdlantfrq;			/* Pn145 : 振動抑制１周波数Ａ										*/
	USHORT	mdlresfrq;			/* Pn146 : 振動抑制１周波数Ｂ										*/
	USHORT	mdlvff;				/* Pn147 : モデル追従制御速度フィードフォワード補償					*/
	USHORT	mdlgn2;				/* Pn148 : 第２モデル追従制御ゲイン [0.1/s]							*/
	USHORT	mdlzeta2;			/* Pn149 : 第２モデル追従制御ゲイン補正								*/
} MFC_CFG_PRM;


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	MfcInitModelControl( MFCTRL *MFControl );
LONG	MfcControlMain( MFCTRL *MFControl, LONG dPcmd, LONG *SpdFFCx,  LONG *TrqFFCx );

/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
SHORT	MfcGetParamEnableSts( MFCTRL *MFControl );
void	MfcSetMfcCtrlSwitch( MFCTRL *MFControl, BOOL Switch );

BOOL	MfcCalculateInitPrm( MFCTRL *MFControl, MFC_CFG_PRM *MfcCfgPrm,
    	                     USHORT jrate, BPRMDAT *Bprm, USHORT mfctype );
BOOL	MfcCalculatePrmSW( MFCTRL *MFControl, MFC_CFG_PRM *MfcCfgPrm, USHORT jrate, BPRMDAT *Bprm );
void	MfcCalculatePrm( MFCTRL *MFControl, MFC_CFG_PRM	*MfcCfgPrm,
    	                 USHORT jrate, BPRMDAT *Bprm, LONG cfric, LONG GselNo );


#define MODEL_CONTROL_H
#endif /* MODEL_CONTROL_H */
/***************************************** end of file **********************************************/
