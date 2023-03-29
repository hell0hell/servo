#ifndef MODEL_CONTROL_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ModelControl.h : �_�C���N�g�g���N���t�@�����X�������f���Ǐ]����֘A������`					*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MfcHandler.h"
#include "Bprm.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
//#define	MFCONEMASSCYCLE		KPI_SBCYCLENS			/* ���̂l�e�b���Z�������(ScanB))		[ns]	*/
#define	MFCTWOMASSCYCLE(CycleTimeNs)	(CycleTimeNs << 1)	/* �񊵐��l�e�b���Z�������(ScanB�~2)	[ns]	*/
#define	C10POW9				1000000000				/* 10^9											*/
#define	E9C2PAI2			25330296				/* 10^9 /(2*PAI)^2								*/

/*--------------------------------------------------------------------------------------------------*/
#define SGDV_TYPE		0x00						/* SGDV�^�C�v									*/
#define SGDX_TYPE		0x01						/* SGDX�݊����[�h								*/
/*--------------------------------------------------------------------------------------------------*/
#define ONE_MASS		0x01						/* ���̌n���f��									*/
#define TWO_MASS1		0x02						/* �Q�����n�n���f���P							*/
#define TWO_MASS2		0x03						/* �Q�����n�n���f���Q							*/
#define ONE_BASE		0x04						/* ���̌n�{�@�䃂�f��							*/


typedef struct MFC_CFG_PRM {
	USHORT	mdlsw;				/* Pn140 : ���f���Ǐ]����֘A�X�C�b�`								*/
	USHORT	mdlgn;				/* Pn141 : ���f���Ǐ]����Q�C�� [0.1/s]								*/
	USHORT	mdlzeta;			/* Pn142 : ���f���Ǐ]����Q�C���␳									*/
	USHORT	mdlptff;			/* Pn143 : ���f���Ǐ]����o�C�A�X(���]����)							*/
	USHORT	mdlntff;			/* Pn144 : ���f���Ǐ]����o�C�A�X(�t�]����)							*/
	USHORT	mdlantfrq;			/* Pn145 : �U���}���P���g���`										*/
	USHORT	mdlresfrq;			/* Pn146 : �U���}���P���g���a										*/
	USHORT	mdlvff;				/* Pn147 : ���f���Ǐ]���䑬�x�t�B�[�h�t�H���[�h�⏞					*/
	USHORT	mdlgn2;				/* Pn148 : ��Q���f���Ǐ]����Q�C�� [0.1/s]							*/
	USHORT	mdlzeta2;			/* Pn149 : ��Q���f���Ǐ]����Q�C���␳								*/
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
