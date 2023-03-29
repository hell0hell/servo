/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FunListDef.c : �⏕�@�\��`																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. �⏕�@�\��`																				*/
/*																									*/
/*		2. �⏕�@�\�Ǘ��e�[�u��																		*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		2014.03.18 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "FunListDef.h"
#include "PrmAccessIf.h"



/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\��`																				*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Fn000 : Alarm Traceback Data Display: Normal mode											*/
/****************************************************************************************************/
CFUNDEF	fndef_fn000 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Alm History",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunAlmTrcBegin,
/* Pointer to the Operator Main		*/	FunAlmTrcMain,
/* Pointer to the Operator End		*/	FunAlmTrcEnd,
};


/****************************************************************************************************/
/*		Fn002 : JOG																					*/
/****************************************************************************************************/
CFUNDEF	fndef_fn002 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"JOG        ",
/* �J�n�����֐��|�C���^				*/	FnEnterJOGoperation,
/* ���s�����֐��|�C���^				*/	FnExecuteJOGoperation,
/* �I�������֐��|�C���^				*/	FnLeaveJOGoperation,
/* Pointer to the Operator Begin	*/	FunJogBegin,
/* Pointer to the Operator Main		*/	FunJogMain,
/* Pointer to the Operator End		*/	FunJogEnd,
};


/****************************************************************************************************/
/*		Fn003 : Origin Point Search																	*/
/****************************************************************************************************/
CFUNDEF	fndef_fn003 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Z-Search   ",
/* �J�n�����֐��|�C���^				*/	FunEnterOriginSearch,
/* ���s�����֐��|�C���^				*/	FunExecuteOriginSearch,
/* �I�������֐��|�C���^				*/	FunLeaveOriginSearch,
/* Pointer to the Operator Begin	*/	FunOriginSearchBegin,
/* Pointer to the Operator Main		*/	FunOriginSearchMain,
/* Pointer to the Operator End		*/	FunOriginSearchEnd,
};


/****************************************************************************************************/
/*		Fn004 : Program JOG																			*/
/****************************************************************************************************/
CFUNDEF	fndef_fn004 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Program JOG",
/* �J�n�����֐��|�C���^				*/	FnEnterPrgJog,
/* ���s�����֐��|�C���^				*/	FnExecutePrgJog,
/* �I�������֐��|�C���^				*/	FnLeavePrgJog,
/* Pointer to the Operator Begin	*/	FunPrgJogBegin,
/* Pointer to the Operator Main		*/	FunPrgJogMain,
/* Pointer to the Operator End		*/	FunPrgJogEnd,
};


/****************************************************************************************************/
/*		Fn005 : User Parameter Initialization														*/
/****************************************************************************************************/
CFUNDEF	fndef_fn005 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Prm Init   ",
/* �J�n�����֐��|�C���^				*/	FnEnterPrmInit,
/* ���s�����֐��|�C���^				*/	FnExecutePrmInit,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunUserParamInitBegin,
/* Pointer to the Operator Main		*/	FunUserParamInitMain,
/* Pointer to the Operator End		*/	FunUserParamInitEnd,
};


/****************************************************************************************************/
/*		Fn006 : Alarm Traceback Data Clear															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn006 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"AlmHist Clr",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	FnExecuteTracebackClear,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunAlarmTraceClearBegin,
/* Pointer to the Operator Main		*/	FunAlarmTraceClearMain,
/* Pointer to the Operator End		*/	FunAlarmTraceClearEnd,
};


/****************************************************************************************************/
/*		Fn008 : Multi Turn Data Reset & Encoder Alarm Reset											*/
/****************************************************************************************************/
CFUNDEF	fndef_fn008 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Mturn Clr  ",
/* �J�n�����֐��|�C���^				*/	FnEnterMultiTurnReset,
/* ���s�����֐��|�C���^				*/	FnExecuteMultiTurnReset,
/* �I�������֐��|�C���^				*/	FnLeaveMultiTurnReset,
/* Pointer to the Operator Begin	*/	FunMultiTurnResetBegin,
/* Pointer to the Operator Main		*/	FunMultiTurnResetMain,
/* Pointer to the Operator End		*/	FunMultiTurnResetEnd,
};


/****************************************************************************************************/
/*		Fn009 : Analog Reference Offset Auto Adjustment												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn009 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Ref Adj    ",
/* �J�n�����֐��|�C���^				*/	FnEnterAnlgRefOffsetAutoAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteAnlgRefOffsetAutoAdj,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunAnlgRefAutoAdjBegin,
/* Pointer to the Operator Main		*/	FunAnlgRefAutoAdjMain,
/* Pointer to the Operator End		*/	FunAnlgRefAutoAdjEnd,
};


/****************************************************************************************************/
/*		Fn00A : Velocity Reference Manual Adjustment												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn00A = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Vel Adj    ",
/* �J�n�����֐��|�C���^				*/	FnEnterAnlgVelocityRefManualAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteAnlgVelocityRefManualAdj,
/* �I�������֐��|�C���^				*/	FnLeaveAnlgVelocityRefManualAdj,
/* Pointer to the Operator Begin	*/	FunAnlgVelRefManAdjBegin,
/* Pointer to the Operator Main		*/	FunAnlgVelRefManAdjMain,
/* Pointer to the Operator End		*/	FunAnlgVelRefManAdjEnd,
};


/****************************************************************************************************/
/*		Fn00B : Torque Reference Manual Adjustment													*/
/****************************************************************************************************/
CFUNDEF	fndef_fn00B = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Trq Adj    ",
/* �J�n�����֐��|�C���^				*/	FnEnterAnlgTorqueRefManualAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteAnlgTorqueRefManualAdj,
/* �I�������֐��|�C���^				*/	FnLeaveAnlgTorqueRefManualAdj,
/* Pointer to the Operator Begin	*/	FunAnlgTrqRefManAdjBegin,
/* Pointer to the Operator Main		*/	FunAnlgTrqRefManAdjMain,
/* Pointer to the Operator End		*/	FunAnlgTrqRefManAdjEnd,
};


/****************************************************************************************************/
/*		Fn00C : Analog Monitor Offset Manual Adjustment												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn00C = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"MonZero Adj",
/* �J�n�����֐��|�C���^				*/	FnEnterAnlgMonOffsetsManualAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteAnlgMonOffsetsManualAdj,
/* �I�������֐��|�C���^				*/	FnLeaveAnlgMonOffsetsManualAdj,
/* Pointer to the Operator Begin	*/	FunAnlgMonOffManAdjBegin,
/* Pointer to the Operator Main		*/	FunAnlgMonOffManAdjMain,
/* Pointer to the Operator End		*/	FunAnlgMonOffManAdjEnd,
};


/****************************************************************************************************/
/*		Fn00D : Analog Monitor Gain Manual Adjustment												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn00D = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"MonGain Adj",
/* �J�n�����֐��|�C���^				*/	FnEnterAnlgMonGainManualAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteAnlgMonGainManualAdj,
/* �I�������֐��|�C���^				*/	FnLeaveAnlgMonGainManualAdj,
/* Pointer to the Operator Begin	*/	FunAnlgMonGainManAdjBegin,
/* Pointer to the Operator Main		*/	FunAnlgMonGainManAdjMain,
/* Pointer to the Operator End		*/	FunAnlgMonGainManAdjEnd,
};


/****************************************************************************************************/
/*		Fn00E : Current Offset Auto Adjustment														*/
/****************************************************************************************************/
CFUNDEF	fndef_fn00E = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Cur AutoAdj",
/* �J�n�����֐��|�C���^				*/	FnEnterMotorCurrentAutoAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteMotorCurrentAutoAdj,
/* �I�������֐��|�C���^				*/	FnLeaveMotorCurrentAutoAdj,
/* Pointer to the Operator Begin	*/	FunCurrentAutoAdjBegin,
/* Pointer to the Operator Main		*/	FunCurrentAutoAdjMain,
/* Pointer to the Operator End		*/	FunCurrentAutoAdjEnd,
};


/****************************************************************************************************/
/*		Fn00F : Current Offset Manual Adjustment													*/
/****************************************************************************************************/
CFUNDEF	fndef_fn00F = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Cur ManuAdj",
/* �J�n�����֐��|�C���^				*/	FnEnterMotorCurrentManualAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteMotorCurrentManualAdj,
/* �I�������֐��|�C���^				*/	FnLeaveMotorCurrentManualAdj,
/* Pointer to the Operator Begin	*/	FunCurrentManAdjBegin,
/* Pointer to the Operator Main		*/	FunCurrentManAdjMain,
/* Pointer to the Operator End		*/	FunCurrentManAdjEnd,
};


/****************************************************************************************************/
/*		Fn010 : Rewriting Inhibition Processing Display												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn010 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Prm Protect",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunAccessSetBegin,
/* Pointer to the Operator Main		*/	FunAccessSetMain,
/* Pointer to the Operator End		*/	FunAccessSetEnd,
};


/****************************************************************************************************/
/*		Fn011 : Motot Type & Y Specification Display												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn011 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"MotorInfo  ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunMotorTypeBegin,
/* Pointer to the Operator Main		*/	FunMotorTypeMain,
/* Pointer to the Operator End		*/	FunMotorTypeEnd,
};


/****************************************************************************************************/
/*		Fn012 : Software Version Display															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn012 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Soft Ver   ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunSoftVersionBegin,
/* Pointer to the Operator Main		*/	FunSoftVersionMain,
/* Pointer to the Operator End		*/	FunSoftVersionEnd,
};


/****************************************************************************************************/
/*		Fn013 : Multi Turn Limit Setting															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn013 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"MturnLmtSet",
/* �J�n�����֐��|�C���^				*/	FnEnterMultiTurnLimitSetting,
/* ���s�����֐��|�C���^				*/	FnExecuteMultiTurnLimitSetting,
/* �I�������֐��|�C���^				*/	FnLeaveMultiTurnLimitSetting,
/* Pointer to the Operator Begin	*/	FunMultiTurnLimitSetBegin,
/* Pointer to the Operator Main		*/	FunMultiTurnLimitSetMain,
/* Pointer to the Operator End		*/	FunMultiTurnLimitSetEnd,
};


/****************************************************************************************************/
/*		Fn014 : Option Card Detection Clear															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn014 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Opt Init   ",
/* �J�n�����֐��|�C���^				*/	FnEnterModuleInfoReset,
/* ���s�����֐��|�C���^				*/	FnExecuteModuleInfoReset,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunOptCardDetClearBegin,
/* Pointer to the Operator Main		*/	FunOptCardDetClearMain,
/* Pointer to the Operator End		*/	FunOptCardDetClearEnd,
};


/****************************************************************************************************/
/*		Fn01B : Init. Threshhold Level for Vibration Detect											*/
/****************************************************************************************************/
CFUNDEF	fndef_fn01B = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"VibLvl Init",
/* �J�n�����֐��|�C���^				*/	FnEnterInitVibDetLevel,
/* ���s�����֐��|�C���^				*/	FnExecuteInitVibDetLevel,
/* �I�������֐��|�C���^				*/	FnLeaveInitVibDetLevel,
/* Pointer to the Operator Begin	*/	FunVibDetLvSetBegin,
/* Pointer to the Operator Main		*/	FunVibDetLvSetMain,
/* Pointer to the Operator End		*/	FunVibDetLvSetEnd,
};


/****************************************************************************************************/
/*		Fn01E : Servo & Motor ID Display															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn01E = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"SvMotOp ID ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunSvMotOpIDBegin,
/* Pointer to the Operator Main		*/	FunSvMotOpIDMain,
/* Pointer to the Operator End		*/	FunSvMotOpIDEnd,
};


/****************************************************************************************************/
/*		Fn020 : Scale Origin Setting																*/
/****************************************************************************************************/
CFUNDEF	fndef_fn020 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"S-Orig Set ",
/* �J�n�����֐��|�C���^				*/	FnEnterScaleZeroSetting,
/* ���s�����֐��|�C���^				*/	FnExecuteScaleZeroSetting,
/* �I�������֐��|�C���^				*/	FnLeaveScaleZeroSetting,
/* Pointer to the Operator Begin	*/	FunScaleOrgSetBegin,
/* Pointer to the Operator Main		*/	FunScaleOrgSetMain,
/* Pointer to the Operator End		*/	FunScaleOrgSetEnd,
};


/****************************************************************************************************/
/*		Fn030 : Software Reset Mode																	*/
/****************************************************************************************************/
CFUNDEF	fndef_fn030 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Soft Reset ",
/* �J�n�����֐��|�C���^				*/	FnEnterSoftwareReset,
/* ���s�����֐��|�C���^				*/	FnExecuteSoftwareReset,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunSftRstBegin,
/* Pointer to the Operator Main		*/	FunSftRstMain,
/* Pointer to the Operator End		*/	FunSftRstEnd,
};


/****************************************************************************************************/
/*		Fn080 : Magnetic Pole Adjust 																*/
/****************************************************************************************************/
CFUNDEF	fndef_fn080 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Pole Detect",
/* �J�n�����֐��|�C���^				*/	FnEnterMagneticPoleDetection,
/* ���s�����֐��|�C���^				*/	FnExecuteMagneticPoleDetection,
/* �I�������֐��|�C���^				*/	FnLeaveMagneticPoleDetection,
/* Pointer to the Operator Begin	*/	FunMagnePoleAdjustBegin,
/* Pointer to the Operator Main		*/	FunMagnePoleAdjustMain,
/* Pointer to the Operator End		*/	FunMagnePoleAdjustEnd,
};


/****************************************************************************************************/
/*		Fn081 : Magnetic Pole Information Copy														*/
/****************************************************************************************************/
CFUNDEF	fndef_fn081 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Pole Copy  ",
/* �J�n�����֐��|�C���^				*/	FnEnterMagneticPoleCopy,
/* ���s�����֐��|�C���^				*/	FnExecuteMagneticPoleCopy,
/* �I�������֐��|�C���^				*/	FnLeaveMagneticPoleCopy,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		Fn100 : Test mode 1: AC On																	*/
/****************************************************************************************************/
CFUNDEF	fndef_fn100 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Test 1     ",
/* �J�n�����֐��|�C���^				*/	FnEnterTetMod,
/* ���s�����֐��|�C���^				*/	FnExecuteTetMod1,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunTstMod1Begin,
/* Pointer to the Operator Main		*/	FunTstMod1Main,
/* Pointer to the Operator End		*/	FunTstModEnd,
};


/****************************************************************************************************/
/*		Fn101 : Test mode 2: Regeneration Off														*/
/****************************************************************************************************/
CFUNDEF	fndef_fn101 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Test 2     ",
/* �J�n�����֐��|�C���^				*/	FnEnterTetMod,
/* ���s�����֐��|�C���^				*/	FnExecuteTetMod2,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunTstMod2Begin,
/* Pointer to the Operator Main		*/	FunTstMod2Main,
/* Pointer to the Operator End		*/	FunTstModEnd,
};


/****************************************************************************************************/
/*		Fn102 : Test mode 3: Overload Mask															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn102 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Test 3     ",
/* �J�n�����֐��|�C���^				*/	FnEnterTetMod,
/* ���s�����֐��|�C���^				*/	FnExecuteTetMod3,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunTstMod3Begin,
/* Pointer to the Operator Main		*/	FunTstMod3Main,
/* Pointer to the Operator End		*/	FunTstModEnd,
};


/****************************************************************************************************/
/*		Fn103 : RAM Editor																			*/
/****************************************************************************************************/
CFUNDEF	fndef_fn103 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"RAM Edit   ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunRamEditBegin,
/* Pointer to the Operator Main		*/	FunRamEditMain,
/* Pointer to the Operator End		*/	FunRamEditEnd,
};


/****************************************************************************************************/
/*		Fn110 : Built-in FAN Lifetime Reset															*/
/****************************************************************************************************/
CFUNDEF	fndef_fn110 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER2,
/* �⏕�@�\����						*/	"B-FAN LtRst",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	FnExecuteBFanLtRst,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunBFanLtRstBegin,
/* Pointer to the Operator Main		*/	FunBFanLtRstMain,
/* Pointer to the Operator End		*/	FunBFanLtRstEnd,
};


/****************************************************************************************************/
/*		Fn200 : Dynamic Auto Tuning Level Setting													*/
/****************************************************************************************************/
CFUNDEF	fndef_fn200 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"TuneLvlSet ",
/* �J�n�����֐��|�C���^				*/	FnEnterSetTuneLessLevel,
/* ���s�����֐��|�C���^				*/	FnExecuteSetTuneLessLevel,
/* �I�������֐��|�C���^				*/	FnLeaveSetTuneLessLevel,
/* Pointer to the Operator Begin	*/	FunDynamicAutoTuningBegin,
/* Pointer to the Operator Main		*/	FunDynamicAutoTuningMain,
/* Pointer to the Operator End		*/	FunDynamicAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn201 : Advanced Autotuning																	*/
/****************************************************************************************************/
CFUNDEF	fndef_fn201 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"AAT        ",
/* �J�n�����֐��|�C���^				*/	FnEnterAdvAutoTuning,
/* ���s�����֐��|�C���^				*/	FnExecuteAdvAutoTuning,
/* �I�������֐��|�C���^				*/	FnLeaveAdvAutoTuning,
/* Pointer to the Operator Begin	*/	FunAdvAutoTuningBegin,
/* Pointer to the Operator Main		*/	FunAdvAutoTuningMain,
/* Pointer to the Operator End		*/	FunAdvAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn202 : Refernce Input type Advanced Autotuning												*/
/****************************************************************************************************/
CFUNDEF	fndef_fn202 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Ref-AAT    ",
/* �J�n�����֐��|�C���^				*/	FnEnterRefInAdvAutoTuning,
/* ���s�����֐��|�C���^				*/	FnExecuteRefInAdvAutoTuning,
/* �I�������֐��|�C���^				*/	FnLeaveRefInAdvAutoTuning,
/* Pointer to the Operator Begin	*/	FunRefInAdvAutoTuningBegin,
/* Pointer to the Operator Main		*/	FunRefInAdvAutoTuningMain,
/* Pointer to the Operator End		*/	FunRefInAdvAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn203 : One Parameter Tuning																*/
/****************************************************************************************************/
CFUNDEF	fndef_fn203 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"OnePrmTun  ",
/* �J�n�����֐��|�C���^				*/	FnEnterOnePrmTuning,
/* ���s�����֐��|�C���^				*/	FnExecuteOnePrmTuning,
/* �I�������֐��|�C���^				*/	FnLeaveOnePrmTuning,
/* Pointer to the Operator Begin	*/	FunOneParamTuningBegin,
/* Pointer to the Operator Main		*/	FunOneParamTuningMain,
/* Pointer to the Operator End		*/	FunOneParamTuningEnd,
};


/****************************************************************************************************/
/*		Fn204 : A-type Vibration Suppression Control Tuning											*/
/****************************************************************************************************/
CFUNDEF	fndef_fn204 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"A-Vib Sup  ",
/* �J�n�����֐��|�C���^				*/	FnEnterAtypeVibTuning,
/* ���s�����֐��|�C���^				*/	FnExecuteAtypeVibTuning,
/* �I�������֐��|�C���^				*/	FnLeaveAtypeVibTuning,
/* Pointer to the Operator Begin	*/	FunAtypeVibTuningBegin,
/* Pointer to the Operator Main		*/	FunAtypeVibTuningMain,
/* Pointer to the Operator End		*/	FunAtypeVibTuningEnd,
};


/****************************************************************************************************/
/*		Fn205 : Model Following Control with Vibration Suppression Tuning							*/
/****************************************************************************************************/
CFUNDEF	fndef_fn205 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Vib Sup    ",
/* �J�n�����֐��|�C���^				*/	FnEnterMfcTuning,
/* ���s�����֐��|�C���^				*/	FnExecuteMfcTuning,
/* �I�������֐��|�C���^				*/	FnLeaveMfcTuning,
/* Pointer to the Operator Begin	*/	FunVibMfcTuningBegin,
/* Pointer to the Operator Main		*/	FunVibMfcTuningMain,
/* Pointer to the Operator End		*/	FunVibMfcTuningEnd,
};


/****************************************************************************************************/
/*		Fn206 : EasyFFT																				*/
/****************************************************************************************************/
CFUNDEF	fndef_fn206 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Easy FFT   ",
/* �J�n�����֐��|�C���^				*/	FnEnterEasyFft,
/* ���s�����֐��|�C���^				*/	FnExecuteEasyFft,
/* �I�������֐��|�C���^				*/	FnLeaveEasyFft,
/* Pointer to the Operator Begin	*/	FunEasyFftBegin,
/* Pointer to the Operator Main		*/	FunEasyFftMain,
/* Pointer to the Operator End		*/	FunEasyFftEnd,
};


/****************************************************************************************************/
/*		Fn207 : Vibration Freq. Display and Notchi Filter Autoset									*/
/****************************************************************************************************/
CFUNDEF	fndef_fn207 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"V-Monitor  ",
/* �J�n�����֐��|�C���^				*/	FnEnterVibrationMonitor,
/* ���s�����֐��|�C���^				*/	FnExecuteVibrationMonitor,
/* �I�������֐��|�C���^				*/	FnLeaveVibrationMonitor,
/* Pointer to the Operator Begin	*/	FunVibMonitorBegin,
/* Pointer to the Operator Main		*/	FunVibMonitorMain,
/* Pointer to the Operator End		*/	FunVibMonitorEnd,
};


/****************************************************************************************************/
/*		Fn300 : Register Editor																		*/
/****************************************************************************************************/
CFUNDEF	fndef_fn300 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"REG Edit   ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	FunRegEditBegin,
/* Pointer to the Operator Main		*/	FunRegEditMain,
/* Pointer to the Operator End		*/	FunRegEditEnd,
};


/****************************************************************************************************/
/*		1090H : Encoder Data Alarm Reset															*/
/****************************************************************************************************/
CFUNDEF	fndef_id1090 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"EncAlmReset",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	FnExecuteEncDataAlmClear,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		200DH :  Velocity Ripple Compensation Information Write										*/
/****************************************************************************************************/
CFUNDEF	fndef_id200D = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"RippleComp ",
/* �J�n�����֐��|�C���^				*/	FnEnterRippleComp,
/* ���s�����֐��|�C���^				*/	FnExecuteRippleComp,
/* �I�������֐��|�C���^				*/	FnLeaveRippleComp,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2001H : Alarm Reset (only Register IF)														*/
/****************************************************************************************************/
CFUNDEF	fndef_id2001 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	FnExecuteAlarmResetting,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2003H : Command Pulse Counter Clear (only Register IF)										*/
/****************************************************************************************************/
CFUNDEF	fndef_id2003 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FunPlsCntClrBegin,
/* ���s�����֐��|�C���^				*/	FunPlsCntClrExe,
/* �I�������֐��|�C���^				*/	FunPlsCntClrEnd,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2004H : Feedback Pulse Counter Clear (only Register IF)										*/
/****************************************************************************************************/
CFUNDEF	fndef_id2004 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FunFbkPlsCntBegin,
/* ���s�����֐��|�C���^				*/	FunFbkPlsCntExe,
/* �I�������֐��|�C���^				*/	FunFbkPlsCntEnd,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2007H : Table Command (only Register IF)													*/
/****************************************************************************************************/
CFUNDEF	fndef_id2007 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterTableCommandMode,
/* ���s�����֐��|�C���^				*/	FnExecuteTableCommandMode,
/* �I�������֐��|�C���^				*/	FnLeaveTableCommandMode,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2008H : Parameter Recalcuration (only Register IF)											*/
/****************************************************************************************************/
CFUNDEF	fndef_id2008 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterPrmRecalc,
/* ���s�����֐��|�C���^				*/	FnExecutePrmRecalc,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2009H : Write RAM Parameter to EEPROM (only Register IF)									*/
/****************************************************************************************************/
CFUNDEF	fndef_id2009 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterStoreParameters,
/* ���s�����֐��|�C���^				*/	FnExecuteStoreParameters,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		200BH : Read Motor Parameters from Scale													*/
/****************************************************************************************************/
CFUNDEF	fndef_id200B = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterMotorParameterRead,
/* ���s�����֐��|�C���^				*/	FnExecuteMotorParameterRead,
/* �I�������֐��|�C���^				*/	FnLeaveMotorParameterRead,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		200CH : Write Motor Parameters to Scale														*/
/****************************************************************************************************/
CFUNDEF	fndef_id200C = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterMotorParameterWrite,
/* ���s�����֐��|�C���^				*/	FnExecuteMotorParameterWrite,
/* �I�������֐��|�C���^				*/	FnLeaveMotorParameterWrite,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		200EH : HW OutPut Signal By Force															*/
/****************************************************************************************************/
CFUNDEF	fndef_id200E = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterForceSignalOut,
/* ���s�����֐��|�C���^				*/	FnExecuteForceSignalOut,
/* �I�������֐��|�C���^				*/	FnLeaveForceSignalOut,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		200FH : ALL Servo Parameter EEPROM Read														*/
/****************************************************************************************************/
CFUNDEF	fndef_id200F = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	RegServoPrmFileReadBegin,
/* ���s�����֐��|�C���^				*/	RegServoPrmFileReadExe,
/* �I�������֐��|�C���^				*/	RegServoPrmFileReadEnd,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2010H : ALL Servo Parameter EEPROM Write													*/
/****************************************************************************************************/
CFUNDEF	fndef_id2010 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	RegServoPrmFileWriteBegin,
/* ���s�����֐��|�C���^				*/	RegServoPrmFileWriteExe,
/* �I�������֐��|�C���^				*/	RegServoPrmFileWriteEnd,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2011H : SDA Parameter Initialization														*/
/****************************************************************************************************/
CFUNDEF	fndef_id2011 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"           ",
/* �J�n�����֐��|�C���^				*/	FnEnterSDAPrmInit,
/* ���s�����֐��|�C���^				*/	FnExecuteSDAPrmInit,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		2012H : DcVoltage Gain Auto Adjustment														*/
/****************************************************************************************************/
CFUNDEF	fndef_id2012 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"P-N AutoAdj",
/* �J�n�����֐��|�C���^				*/	FnEnterDcVoltageAutoAdj,
/* ���s�����֐��|�C���^				*/	FnExecuteDcVoltageAutoAdj,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		3000H : 7SegLed And Push Button Check Test													*/
/****************************************************************************************************/
CFUNDEF	fndef_id3000 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"7SegTest   ",
/* �J�n�����֐��|�C���^				*/	FnEnterSevenSegLedChkTest,
/* ���s�����֐��|�C���^				*/	FnExecuteSevenSegLedChkTest,
/* �I�������֐��|�C���^				*/	FnLeaveSevenSegLedChkTest,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		3001H : Adjust Speed Ref.A/D Gain															*/
/****************************************************************************************************/
CFUNDEF	fndef_id3001 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"AdjVGain   ",
/* �J�n�����֐��|�C���^				*/	FnEnterAdjustVrefGain,
/* ���s�����֐��|�C���^				*/	FnExecuteAdjustVrefGain,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		3002H : Adjust Torque Positive Ref.A/D Gain													*/
/****************************************************************************************************/
CFUNDEF	fndef_id3002 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"AdjTPGain  ",
/* �J�n�����֐��|�C���^				*/	FnEnterAdjustTrefPGain,
/* ���s�����֐��|�C���^				*/	FnExecuteAdjustTrefPGain,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		3003H : Adjust Torque Negative Ref.A/D Gain													*/
/****************************************************************************************************/
CFUNDEF	fndef_id3003 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"AdjTNGain  ",
/* �J�n�����֐��|�C���^				*/	FnEnterAdjustTrefNGain,
/* ���s�����֐��|�C���^				*/	FnExecuteAdjustTrefNGain,
/* �I�������֐��|�C���^				*/	NULL,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};


/****************************************************************************************************/
/*		3004H : Analog Monitor Output Volt Test														*/
/****************************************************************************************************/
CFUNDEF	fndef_id3004 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"VoltTest   ",
/* �J�n�����֐��|�C���^				*/	FnEnterAnlgMonOutputVoltTest,
/* ���s�����֐��|�C���^				*/	FnExecuteAnlgMonOutputVoltTest,
/* �I�������֐��|�C���^				*/	FnLeaveAnlgMonOutputVoltTest,
/* Pointer to the Operator Begin	*/	NULL,
/* Pointer to the Operator Main		*/	NULL,
/* Pointer to the Operator End		*/	NULL,
};





/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\�Ǘ��e�[�u��																		*/
/*																									*/
/****************************************************************************************************/
CFUNTBL	FnFunTbl[] = {
/* FnNo,  FnMode,  FnDef																			*/
/*--------------------------------------------------------------------------------------------------*/
{ 0x0000, 0xFFFF, &fndef_fn000		},	/* Alarm Traceback Data Display: Normal mode				*/
{ 0x0002, 0x1002, &fndef_fn002		},	/* JOG														*/
{ 0x0003, 0x1003, &fndef_fn003		},	/* Origin Point Search										*/
{ 0x0004, 0x1004, &fndef_fn004		},	/* Program JOG												*/
{ 0x0005, 0x1005, &fndef_fn005		},	/* User Parameter Initialization							*/
{ 0x0006, 0x1006, &fndef_fn006		},	/* Alarm Traceback Data Clear								*/
{ 0x0008, 0x1008, &fndef_fn008		},	/* Multi Turn Data Reset & Encoder Alarm Reset				*/
{ 0x0009, 0x1009, &fndef_fn009		},	/* Analog Reference Offset Auto Adjustment					*/
{ 0x000A, 0x100A, &fndef_fn00A		},	/* Velocity Reference Manual Adjustment						*/
{ 0x000B, 0x100B, &fndef_fn00B		},	/* Torque Reference Manual Adjustment						*/
{ 0x000C, 0x100C, &fndef_fn00C		},	/* Analog Monitor Offset Manual Adjustment					*/
{ 0x000D, 0x100D, &fndef_fn00D		},	/* Analog Monitor Gain Manual Adjustment					*/
{ 0x000E, 0x100E, &fndef_fn00E		},	/* Current Offset Auto Adjustment							*/
{ 0x000F, 0x100F, &fndef_fn00F		},	/* Current Offset Manual Adjustment							*/
{ 0x0010, 0x8001, &fndef_fn010		},	/* Rewriting Inhibition Processing Display					*/
{ 0x0011, 0xFFFF, &fndef_fn011		},	/* Motot Type & Y Specification Display						*/
{ 0x0012, 0xFFFF, &fndef_fn012		},	/* Software Version Display									*/
{ 0x0013, 0x1013, &fndef_fn013		},	/* Multi Turn Limit Setting									*/
{ 0x0014, 0x1014, &fndef_fn014		},	/* Option Card Detection Clear								*/
{ 0x001B, 0x101B, &fndef_fn01B		},	/* Init. Threshhold Level for Vibration Detect				*/
{ 0x001E, 0xFFFF, &fndef_fn01E		},	/* Servo & Motor ID Display									*/
{ 0x0020, 0x1020, &fndef_fn020		},	/* Scale Origin Setting										*/
{ 0x0030, 0x2006, &fndef_fn030		},	/* Software Reset Mode										*/
{ 0x0080, 0x1080, &fndef_fn080		},	/* Magnetic Pole Adjust 									*/
{ 0x0081, 0x108F, &fndef_fn081		},	/* Magnetic Pole Information Copy							*/
{ 0x0100, 0x1100, &fndef_fn100		},	/* Test mode 1: AC On										*/
{ 0x0101, 0x1101, &fndef_fn101		},	/* Test mode 2: Regeneration Off							*/
{ 0x0102, 0x1102, &fndef_fn102		},	/* Test mode 3: Overload Mask								*/
{ 0x0103, 0xFFFF, &fndef_fn103		},	/* RAM Editor												*/
{ 0x0110, 0x2014, &fndef_fn110		},	/* Built-in FAN Lifetime Reset								*/
{ 0x0200, 0x1200, &fndef_fn200		},	/* Dynamic Auto Tuning Level Setting						*/
{ 0x0201, 0x1201, &fndef_fn201		},	/* Advanced Autotuning										*/
{ 0x0202, 0x1202, &fndef_fn202		},	/* Refernce Input type Advanced Autotuning					*/
{ 0x0203, 0x1203, &fndef_fn203		},	/* One Parameter Tuning										*/
{ 0x0204, 0x1204, &fndef_fn204		},	/* A-type Vibration Suppression Control Tuning				*/
{ 0x0205, 0x1205, &fndef_fn205		},	/* Model Following Control with Vibration Suppression Tuning	*/
{ 0x0206, 0x1206, &fndef_fn206		},	/* EasyFFT													*/
{ 0x0207, 0x1207, &fndef_fn207		},	/* Vibration Freq. Display and Notchi Filter Autoset		*/
{ 0x0300, 0xFFFF, &fndef_fn300		},	/* Register Editor											*/
{ 0xFFFF, 0x1090, &fndef_id1090		},	/* Encoder Data Alarm Reset									*/
{ 0xFFFF, 0x200D, &fndef_id200D		},	/*  Velocity Ripple Compensation Information Write			*/
{ 0xFFFF, 0x2001, &fndef_id2001		},	/* Alarm Reset (only Register IF)							*/
{ 0xFFFF, 0x2003, &fndef_id2003		},	/* Command Pulse Counter Clear (only Register IF)			*/
{ 0xFFFF, 0x2004, &fndef_id2004		},	/* Feedback Pulse Counter Clear (only Register IF)			*/
{ 0xFFFF, 0x2007, &fndef_id2007		},	/* Table Command (only Register IF)							*/
{ 0xFFFF, 0x2008, &fndef_id2008		},	/* Parameter Recalcuration (only Register IF)				*/
{ 0xFFFF, 0x2009, &fndef_id2009		},	/* Write RAM Parameter to EEPROM (only Register IF)			*/
{ 0xFFFF, 0x200B, &fndef_id200B		},	/* Read Motor Parameters from Scale							*/
{ 0xFFFF, 0x200C, &fndef_id200C		},	/* Write Motor Parameters to Scale							*/
{ 0xFFFF, 0x200E, &fndef_id200E		},	/* HW OutPut Signal By Force								*/
{ 0xFFFF, 0x200F, &fndef_id200F		},	/* ALL Servo Parameter EEPROM Read							*/
{ 0xFFFF, 0x2010, &fndef_id2010		},	/* ALL Servo Parameter EEPROM Write							*/
{ 0xFFFF, 0x2011, &fndef_id2011		},	/* SDA Parameter Initialization								*/
{ 0xFFFF, 0x2012, &fndef_id2012		},	/* DcVoltage Gain Auto Adjustment							*/
{ 0xFFFF, 0x3000, &fndef_id3000		},	/* 7SegLed And Push Button Check Test						*/
{ 0xFFFF, 0x3001, &fndef_id3001		},	/* Adjust Speed Ref.A/D Gain								*/
{ 0xFFFF, 0x3002, &fndef_id3002		},	/* Adjust Torque Positive Ref.A/D Gain						*/
{ 0xFFFF, 0x3003, &fndef_id3003		},	/* Adjust Torque Negative Ref.A/D Gain						*/
{ 0xFFFF, 0x3004, &fndef_id3004		},	/* Analog Monitor Output Volt Test							*/
};
const	UINT	FnFunTblEntNum = sizeof(FnFunTbl)/sizeof(FnFunTbl[0]);







/***************************************** end of file **********************************************/
