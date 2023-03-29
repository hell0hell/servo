/****************************************************************************************************/
/*																									*/
/*																									*/
/*		外乱オブザーバ定義ヘッダーファイル															*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2013.08.17	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#ifndef	_DISTURB_OBSERVER_H
#define _DISTURB_OBSERVER_H


#include "BaseControls.h"






/****************************************************************************************************/
/*																									*/
/*		DEFINES																						*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/


/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
enum DOBS_CHANGE_STS{
	DOBS_NOCHANGE		= (LONG)0x00L,			/* 外乱オブザーバ無効⇔有効切り替えなし				*/
	DOBSCHNG_INV2ACT	= (LONG)0x01L,			/* 外乱オブザーバ無効⇒有効切り替え					*/
	DOBSCHNG_ACT2INV	= (LONG)0x02L,			/* 外乱オブザーバ有効⇒無効切り替え					*/
};

/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		PROTOTYPE																					*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Globals																						*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
void	TuneLessCtrlInitialize( TUNELESS_CTRL *TuneLessCtrl );
void	TuningLessCtrl( TUNELESS_CTRL *TuneLessCtrl, KSGAIN MotSpd, KSGAIN TrqRefi, KSGAIN TrqRef );			/*<S00A>*/

/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
void	DisturbObserverInitialize( DOBS_CTRL *DobsCtrl, GSELDOBS *pActGains );			/* <S09B> */
void	DisturbObserver( DOBS_CTRL *DobsCtrl, KSGAIN MotSpd, KSGAIN TrqRef );				/*<S00A>*/

/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
void	EhSpeedObserverInitialize( EHVOBS_CTRL *EhVobsCtrl );
LONG	EhSpeedObserver( EHVOBS_CTRL *EhVobsCtrl, KSGAIN MotSpd, KSGAIN TrqRef );				/*<S00A>*/
void	InitParamFriction( BASE_LOOPCTRLS *BaseLoops );/*<S0CD>*/
LONG	LpxMotorFricComp( BOOL BaseEnable, BASE_LOOPCTRLS *BaseLoops );/*<S0CD>*/

/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
/* 調整レス使用/不使用設定取得用API */
__inline	BOOL	TuneLessGetTuneLessUse( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->conf.TuningLessUse);
}
/* 調整レス状態取得用API */
__inline	BOOL	TuneLessGetTuneLessSts( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->var.TuneLessAct);
}
/* 調整レス外乱オブザーバによるトルク補償値取得用API */
__inline	LONG	TuneLessGetDisTrqCmp( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->Drcomp.var.DisTrqCmp);
}
/* 調整レス外乱オブザーバによる推定外乱取得用API */
__inline	LONG	TuneLessGetDisTrq( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->Drcomp.var.DisTrq);
}
/* 調整レス外乱オブザーバによるトルク指令フィルタ積分初期化要求取得用API */
__inline	BOOL	TuneLessGetTFilClrReq( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->Drcomp.var.TrqFilClrReq);
}
/* 調整レス位相補償速度オブザーバによる推定速度取得用API */
__inline	LONG	TuneLessGetSpdObsFbk( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->EhVobs.var.EhSpdObs);
}
/* 調整レス強制無効要求解除API */
__inline	void	TuneLessClearForceInvldReq( TUNELESS_CTRL *TuneLessCtrl )
{
	TuneLessCtrl->var.TuneLessInvldReq = FALSE;
}
/* 調整レス強制無効要求設定API */
__inline	void	TuneLessSetForceInvldReq( TUNELESS_CTRL *TuneLessCtrl )
{
	TuneLessCtrl->var.TuneLessAct = FALSE;
	TuneLessCtrl->var.TuneLessInvldReq = TRUE;
}
/* 調整レス状態取得用API */
__inline	BOOL	TuneLessGetTuneLessModeEx( TUNELESS_CTRL *TuneLessCtrl ) /* <S1C7> */
{
	return (TuneLessCtrl->conf.TuningLessEx);
}


/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
/* 外乱オブザーバ状態取得用API */
__inline	BOOL	DobsGetObserverSts( DOBS_CTRL *DobsCtrl )
{
	return (DobsCtrl->var.DobsAct);
}
/* 外乱オブザーバによるトルク補償値取得用API */
__inline	LONG	DobsGetDisTrqCmp( DOBS_CTRL *DobsCtrl )
{
	return (DobsCtrl->var.DisTrqCmp);
}
/* 外乱オブザーバによる切り替え状態取得用API */
__inline	UCHAR	DobsGetExchangeSts( DOBS_CTRL *DobsCtrl )
{
	return (DobsCtrl->var.ExchangeSts);
}


/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
/* 調整レス使用/不使用設定取得用API */
__inline	BOOL	EhVobsGetObserverUse( EHVOBS_CTRL *EhVobsCtrl )
{
	return (EhVobsCtrl->conf.EhVobsUse);
}


/****************************************************************************************************/
/*																									*/
/*		EXTERN																						*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		for Tuning Less Control																		*/
/****************************************************************************************************/
extern	BOOL	TuneLessGetTuneLessSts( TUNELESS_CTRL *TuneLessCtrl );
extern	LONG	TuneLessGetSpdObsFbk( TUNELESS_CTRL *TuneLessCtrl );
extern	LONG	TuneLessGetDisTrqCmp( TUNELESS_CTRL *TuneLessCtrl );
extern	LONG	TuneLessGetDisTrq( TUNELESS_CTRL *TuneLessCtrl );
extern	BOOL	TuneLessGetTFilClrReq( TUNELESS_CTRL *TuneLessCtrl );
extern	void	TuneLessSetForceInvldReq( TUNELESS_CTRL *TuneLessCtrl );
extern	void	TuneLessClearForceInvldReq( TUNELESS_CTRL *TuneLessCtrl );
/* <S1A5> */
extern  void	TuneLessInitRobustCompensator( 	TUNELESS_CTRL *TuneLessCtrl, 
												TUNELESS_DRCOMP	*Drcomp );	

/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
extern	BOOL	DobsGetObserverSts( DOBS_CTRL *DobsCtrl );
extern	LONG	DobsGetDisTrqCmp( DOBS_CTRL *DobsCtrl );
extern	UCHAR	DobsGetExchangeSts( DOBS_CTRL *DobsCtrl );



/***************************************** end of file **********************************************/
#endif /* _DISTURB_OBSERVER_H */
