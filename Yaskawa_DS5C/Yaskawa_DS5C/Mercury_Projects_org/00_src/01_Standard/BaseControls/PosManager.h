#ifndef POS_MANAGER_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PosManager.h : 位置管理関連処理定義															*/
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
#include "BaseControls.h"

/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Coin Signal Output Select Definition														*/
/*--------------------------------------------------------------------------------------------------*/
#define POSERR			0x00						/* 位置偏差で/COIN出力							*/
#define POSERR_REFOUT	0x01						/* 位置偏差と位置指令フィルタ出力で/COIN出力	*/
#define POSERR_REFIN	0x02						/* 位置偏差と位置指令入力で/COIN出力			*/



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
void	PosMngInitPositionManager( BASE_CTRL_HNDL *pBaseCtrl );
LONG	PosMngNetPosCmdManager( BASE_LOOPCTRLS *BaseLoops, SERVO_CONTROL_IF *SvControlIf,
														FUN_CMN_CONTROL *FnCmnCtrl, BPRMDAT *Bprm );/* <S04D> */
void	PosMngSenOnReCalcAbsoPos(MENCV *MencV, APOSRG *EncPos,
    	                         EGEAR *Egear, DBYTEX bit_dp, USHORT limmlt, BOOL RvsDir );
void	PosMngSenOnReCalcAbsoScalePos(MENCV *MencV, APOSRG *EncPos, EGEAR *Egear, BOOL RvsDir);
void	PosMngLatchNetFbkPos( BASE_CTRL_HNDL *pBaseCtrl, SERVO_CONTROL_IF *SvControlIf, BPRMDAT *Bprm );/* <S04D> */

//BOOL	PosMngCalculatePrmVibSupFilSW( VIBSUPFIL *VibSupFil, ULONG mdlsw, ULONG ff_feq, ULONG ff_fil, LONG ScanTimeNs ); /* <S065> */
//void	PosMngCalculatePrmVibSupFil( VIBSUPFIL *VibSupFil, ULONG ff_frq, ULONG ff_fil, LONG ScanTimeNs ); /* <S065> */
void	PosMngResponseManager( BASE_CTRL_HNDL *BaseControls );

void	StlgInitSettlingTime( SETTLINGTIME *SettlingTime, LONG CycleTimeUs );
void	StlgCalculateSettlingTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts );
void	StlgMakeMaxOverShoot( SETTLINGTIME *SettlingTime, POS_MNG_HNDL *PosManager, BOOL PosCtrlMode );
void	StlgMakeCoinOffTime( SETTLINGTIME *SettlingTime, LONG dPcmda, BOOL CoinSts, BOOL PosCtrlMode );
void	StlgCalculateOverShootErrLevel( SETTLINGTIME *SettlingTime, LONG CoinLevel, LONG ovserrdetlvl );


/****************************************************************************************************/
/*		API Function																				*/
/****************************************************************************************************/
void	BpiEgearFuncSwitch( POS_MNG_HNDL *PosManager, BOOL Switch );
void	BpiPcmdFilSwitch( POS_MNG_HNDL *PosManager, BOOL Switch );
void	BpiRsetMaxOverShoot( SETTLINGTIME *SettlingTime );
void	BpiRsetCoinOffTime( SETTLINGTIME *SettlingTime );
LONG	BpiGetCoinOffTime( SETTLINGTIME *SettlingTime );
LONG	BpiGetMaxOverShoot( SETTLINGTIME *SettlingTime );
LONG	BpiGetCoinLatchSts( SETTLINGTIME *SettlingTime );
LONG	BpiGetSettlingTime( SETTLINGTIME *SettlingTime );

#define POS_MANAGER_H
#endif /* POS_MANAGER_H */
/***************************************** end of file **********************************************/
