#ifndef GAIN_CHANGE_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		GainChange.h : �Q�C���؂�ւ��֘A������`													*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "GselHandler.h"
#include "MfcHandler.h"
#include "BaseControls.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Gain Select Data Definition																*/
/*--------------------------------------------------------------------------------------------------*/
#define	SVCDEF_NETGSEL_SETNUM	2					/* �Q�C���؂�ւ��Ή��o���N�Z�b�g����`			*/
#define	GAIN1					0x400000			/* �L���Q�C�����j�^�p							*/
#define	GAIN2					0x800000			/* �L���Q�C�����j�^�p							*/



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
//<S09B> void	GselInitAutoGainChange( ATGSEL *pAtGselA, ATGSEL *pAtGselB );
void	GselInitServoGainChange( GAIN_CHNG_HNDL *hGainChg );
LONG	GselDetGainChngTiming( BOOL CoinSignal, BOOL NearSignal, BOOL RefZSignal, LONG Condition );
//<S09B> void	GselAutoGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG triger, CTRL_CMD_PRM *CtrlCmdPrm );
//<S09B> void	GselManualGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG GselNum, BOOL TuningLess, 
//				CTRL_CMD_PRM *CtrlCmdPrm );
void	GselManualGainChange( BASE_CTRL_HNDL *BaseControls );				/* <S09B> */
void	GselCalculatePrm( GAIN_CHNG_HNDL *hGainChg, USHORT gnswwait1, USHORT gnswwait2, LONG ScanTime );
BOOL	GselGet_AtGselStatus( GAIN_CHNG_HNDL *hGainChg );

#if 0 /* 2010.11.10 Y.Oka �����Q�C���؂�ւ����̃X���[�W���O���Ή��̂��߃R�����g�A�E�g */
void	PcalAtGseldKsSet( GAIN_CHNG_HNDL *hGainChg, PRMDATA *Prm, LONG ScanTime );
void	PcalAtGseldKviSet( GAIN_CHNG_HNDL *hGainChg, PRMDATA *Prm, LONG ScanTime );
void	PcalAtGseldKpSet( GAIN_CHNG_HNDL *hGainChg, PRMDATA *Prm, LONG ScanTime );
void	PcalAtGseldKlpfSet( AXIS_HANDLE *Axis, LONG ScanTime );
KSGAIN	DeltaGainCalc( KSGAIN prev_gn, KSGAIN next_gn, ULONG time, LONG ScanTime );
KSGAIN	DeltaFilGnCalc( ATGSEL *AtGsel, KSGAIN GselTrqFilMin, ULONG time, LONG ScanTime );
#endif /* #if 0 */


/*--------------------------------------------------------------------------------------------------*/
/*		Auto Gain Change (AutoGainChange.c) 														*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_ATGAINCHANGE_USE	== 1
void	GselInitAutoGainChange( ATGSEL *pAtGsel, 		/* Initialize Auto Gain Change				*/
			GSELGAINS 	*prevGain,						/* prevGain 								*/
			GSELGAINS 	*nextGain,						/* nextGain 								*/
			GSELDOBS	*prevGainDobs,					/* prevGain(Dobs) 							*/
			GSELDOBS	*nextGainDobs );				/* nextGain(Dobs) 							*/
/*--------------------------------------------------------------------------------------------------*/
void	GselAutoGainChange( 							/* Auto Gain Change Main Function			*/
			BASE_CTRL_HNDL *BaseControls, 				/*											*/
			LONG trigger );								/*											*/
/*--------------------------------------------------------------------------------------------------*/
void	GselRstAutoGainChange( 							/* Reset Auto Gain Change 					*/
			GAIN_CHNG_HNDL	*pGainChange );				/*											*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAutoGainChange( ATGSEL *pAtGsel, 			/* Auto Gain Change Parameter Calculation	*/
				LONG swtime, 							/* Pn131/Pn132[ms] 							*/
				LONG dlytime, 							/* Pn135/Pn136[ms] 							*/
				LONG svcycleus );						/* Execute Cycle [us]						*/
/*--------------------------------------------------------------------------------------------------*/
void	PcalAtGseldKpSet( 								/* �����Q�C���ؑ֎��̇�Kp�Q�C���v�Z			*/
				ATGSEL *pAtGsel, 
				GSELGAINS *dstGain );
void	PcalAtGseldKviSet( 								/* �����Q�C���ؑ֎��̇�Kv,��Kvi�Q�C���v�Z	*/
				ATGSEL *pAtGsel, 
				GSELGAINS *dstGain );
void	PcalAtGseldKlpfSet( 							/* �����Q�C���ؑ֎��̇�Klpf�Q�C���v�Z		*/
				ATGSEL *pAtGsel, 
				GSELGAINS *dstGain );
void	PcalAtGseldKsSet( 								/* �����Q�C���ؑ֎��̇�Ks,Ksj�Q�C���v�Z		*/
				ATGSEL *pAtGsel, 
				GSELDOBS *dstGain );
/*--------------------------------------------------------------------------------------------------*/
#else
#define GselInitAutoGainChange( x1, x2, x3, x4, x5 )	{;}
#define	GselAutoGainChange( x1, x2 )					{;}
#define	GselRstAutoGainChange( x1 )						{;}
#define	PcalAutoGainChange( x1, x2, x3, x4 )			{;}
#define PcalAtGseldKpSet( x1, x2 )						{;}
#define	PcalAtGseldKviSet( x1, x2 )						{;}
#define	PcalAtGseldKlpfSet( x1, x2 )					{;}
#define	PcalAtGseldKsSet( x1, x2 )						{;}
#endif


#define GAIN_CHANGE_H
#endif /* GAIN_CHANGE_H */
/***************************************** end of file **********************************************/
