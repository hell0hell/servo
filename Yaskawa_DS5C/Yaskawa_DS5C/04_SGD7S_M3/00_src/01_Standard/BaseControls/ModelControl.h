#ifndef MODEL_CONTROL_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ModelControl.h : _CNggNt@Xû®fÇ]§äÖAè`					*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	Å	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MfcHandler.h"
#include "Bprm.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
//#define	MFCONEMASSCYCLE		KPI_SBCYCLENS			/* ÌlebZ»²¸ÙÀ²Ñ(ScanB))		[ns]	*/
#define	MFCTWOMASSCYCLE(CycleTimeNs)	(CycleTimeNs << 1)	/* ñµ«lebZ»²¸ÙÀ²Ñ(ScanB~2)	[ns]	*/
#define	C10POW9				1000000000				/* 10^9											*/
#define	E9C2PAI2			25330296				/* 10^9 /(2*PAI)^2								*/

/*--------------------------------------------------------------------------------------------------*/
#define SGDV_TYPE		0x00						/* SGDV^Cv									*/
#define SGDX_TYPE		0x01						/* SGDXÝ·[h								*/
/*--------------------------------------------------------------------------------------------------*/
#define ONE_MASS		0x01						/* Ìnf									*/
#define TWO_MASS1		0x02						/* Qµ«nnfP							*/
#define TWO_MASS2		0x03						/* Qµ«nnfQ							*/
#define ONE_BASE		0x04						/* Ìn{@äf							*/


typedef struct MFC_CFG_PRM {
	USHORT	mdlsw;				/* Pn140 : fÇ]§äÖAXCb`								*/
	USHORT	mdlgn;				/* Pn141 : fÇ]§äQC [0.1/s]								*/
	USHORT	mdlzeta;			/* Pn142 : fÇ]§äQCâ³									*/
	USHORT	mdlptff;			/* Pn143 : fÇ]§äoCAX(³]ûü)							*/
	USHORT	mdlntff;			/* Pn144 : fÇ]§äoCAX(t]ûü)							*/
	USHORT	mdlantfrq;			/* Pn145 : U®}§Püg`										*/
	USHORT	mdlresfrq;			/* Pn146 : U®}§Püga										*/
	USHORT	mdlvff;				/* Pn147 : fÇ]§ä¬xtB[htH[hâ					*/
	USHORT	mdlgn2;				/* Pn148 : æQfÇ]§äQC [0.1/s]							*/
	USHORT	mdlzeta2;			/* Pn149 : æQfÇ]§äQCâ³								*/
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
