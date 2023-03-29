#ifndef RIPPLE_COMP_H
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		RippleComp.h : リップル補償関連処理定義														*/
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
#include "SerialEncIf.h"	/*<S1FD>*/
#include "Bprm.h"			/*<S1FD>*/
//#include "MercuryGlobals.h"
/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
LONG	RippleCompensation( RPLCMP *RippleCmp, USHORT MechaPhase, USHORT MotPhase, BOOL BaseEnable);
void	RippleCompChkThresholdVal( RPLCMP *RippleCmp, USHORT CtrlMode, LONG SpdRef,
																	LONG SpdFb, BOOL TuneLessCtrl );
BOOL	CalVelRippleCompE( RPLCMP *RippleCmp, BPRMDAT *Bprm, USHORT RippleCompGainE, 
							USHORT *RippleCompCycleE, SHORT *RippleCompAmplitudeE, USHORT *RippleCompPhaseE);
ULONG	ChkMotorSerialNo( UINT AxisNo, MENCV *MencV );
void	FnRegCalVelRippleCompE( BPRMDAT *Bprm, RPLCMP *RippleCmp );
BOOL	RippleCompGetRippleWrnState( RPLCMP *RippleCmp );/*<S1FD>*/

#if 0/* TODO : 速度リップル補償 機械角補正処理(将来用としてコメントアウト) */
BOOL	CalVelRippleCompM( RPLCMP *RippleCmp, BPRMDAT *Bprm, USHORT RippleCompGainM, 
							USHORT *RippleCompCycleM, SHORT *RippleCompAmplitudeM, USHORT *RippleCompPhaseM);

void	FnRegCalVelRippleCompM( AXIS_HANDLE *Axis );
#endif/* TODO : 速度リップル補償 機械角補正処理(将来用としてコメントアウト) */

//LONG	RippleCompGetMonOutSel( RPLCMP_PRM *RippleCmpP );	/* モニタ出力選択 *//* @@現状未対応 */



#define RIPPLE_COMP_H
#endif /* RIPPLE_COMP_H */
/***************************************** end of file **********************************************/
