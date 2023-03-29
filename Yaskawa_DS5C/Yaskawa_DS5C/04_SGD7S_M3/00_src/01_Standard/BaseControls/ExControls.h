/****************************************************************************************************/
/*																									*/
/*																									*/
/*		�O���I�u�U�[�o��`�w�b�_�[�t�@�C��															*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2013.08.17	Y.Oka	For Mercury												*/
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
	DOBS_NOCHANGE		= (LONG)0x00L,			/* �O���I�u�U�[�o�����̗L���؂�ւ��Ȃ�				*/
	DOBSCHNG_INV2ACT	= (LONG)0x01L,			/* �O���I�u�U�[�o�����˗L���؂�ւ�					*/
	DOBSCHNG_ACT2INV	= (LONG)0x02L,			/* �O���I�u�U�[�o�L���˖����؂�ւ�					*/
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
/* �������X�g�p/�s�g�p�ݒ�擾�pAPI */
__inline	BOOL	TuneLessGetTuneLessUse( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->conf.TuningLessUse);
}
/* �������X��Ԏ擾�pAPI */
__inline	BOOL	TuneLessGetTuneLessSts( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->var.TuneLessAct);
}
/* �������X�O���I�u�U�[�o�ɂ��g���N�⏞�l�擾�pAPI */
__inline	LONG	TuneLessGetDisTrqCmp( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->Drcomp.var.DisTrqCmp);
}
/* �������X�O���I�u�U�[�o�ɂ�鐄��O���擾�pAPI */
__inline	LONG	TuneLessGetDisTrq( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->Drcomp.var.DisTrq);
}
/* �������X�O���I�u�U�[�o�ɂ��g���N�w�߃t�B���^�ϕ��������v���擾�pAPI */
__inline	BOOL	TuneLessGetTFilClrReq( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->Drcomp.var.TrqFilClrReq);
}
/* �������X�ʑ��⏞���x�I�u�U�[�o�ɂ�鐄�葬�x�擾�pAPI */
__inline	LONG	TuneLessGetSpdObsFbk( TUNELESS_CTRL *TuneLessCtrl )
{
	return (TuneLessCtrl->EhVobs.var.EhSpdObs);
}
/* �������X���������v������API */
__inline	void	TuneLessClearForceInvldReq( TUNELESS_CTRL *TuneLessCtrl )
{
	TuneLessCtrl->var.TuneLessInvldReq = FALSE;
}
/* �������X���������v���ݒ�API */
__inline	void	TuneLessSetForceInvldReq( TUNELESS_CTRL *TuneLessCtrl )
{
	TuneLessCtrl->var.TuneLessAct = FALSE;
	TuneLessCtrl->var.TuneLessInvldReq = TRUE;
}
/* �������X��Ԏ擾�pAPI */
__inline	BOOL	TuneLessGetTuneLessModeEx( TUNELESS_CTRL *TuneLessCtrl ) /* <S1C7> */
{
	return (TuneLessCtrl->conf.TuningLessEx);
}


/****************************************************************************************************/
/*		for Disturb Observer																		*/
/****************************************************************************************************/
/* �O���I�u�U�[�o��Ԏ擾�pAPI */
__inline	BOOL	DobsGetObserverSts( DOBS_CTRL *DobsCtrl )
{
	return (DobsCtrl->var.DobsAct);
}
/* �O���I�u�U�[�o�ɂ��g���N�⏞�l�擾�pAPI */
__inline	LONG	DobsGetDisTrqCmp( DOBS_CTRL *DobsCtrl )
{
	return (DobsCtrl->var.DisTrqCmp);
}
/* �O���I�u�U�[�o�ɂ��؂�ւ���Ԏ擾�pAPI */
__inline	UCHAR	DobsGetExchangeSts( DOBS_CTRL *DobsCtrl )
{
	return (DobsCtrl->var.ExchangeSts);
}


/****************************************************************************************************/
/*		for Eh Velocity Observer																	*/
/****************************************************************************************************/
/* �������X�g�p/�s�g�p�ݒ�擾�pAPI */
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
