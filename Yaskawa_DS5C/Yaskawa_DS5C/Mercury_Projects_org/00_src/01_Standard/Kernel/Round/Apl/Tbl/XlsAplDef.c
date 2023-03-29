/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsAplTbl.c : Operator Application Definition												*/
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
/*		2013.05.17 : K.Ozaki																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "Function.h"



/****************************************************************************************************/
/*																									*/
/*		Operator Application Definiton																*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*		Fn000 : Alarm Traceback Data Display: Normal Mode											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn000 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	1,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Alm History",
/* �J�n�����֐��|�C���^				*/	FunAlmTrcBegin,
/* ���s�����֐��|�C���^				*/	FunAlmTrcMain,
/* �I�������֐��|�C���^				*/	FunAlmTrcEnd,
};

/****************************************************************************************************/
/*		Fn002 : JOG																					*/
/****************************************************************************************************/
CFUNDEF	apldef_fn002 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"JOG        ",
/* �J�n�����֐��|�C���^				*/	FunJogBegin,
/* ���s�����֐��|�C���^				*/	FunJogMain,
/* �I�������֐��|�C���^				*/	FunJogEnd,
};


/****************************************************************************************************/
/*		Fn003 : Origin Point Search																	*/
/****************************************************************************************************/
CFUNDEF	apldef_fn003 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Z-Search   ",
/* �J�n�����֐��|�C���^				*/	FunOriginSearchBegin,
/* ���s�����֐��|�C���^				*/	FunOriginSearchMain,
/* �I�������֐��|�C���^				*/	FunOriginSearchEnd,
};


/****************************************************************************************************/
/*		Fn004 : Program JOG																			*/
/****************************************************************************************************/
CFUNDEF	apldef_fn004 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Program JOG",
/* �J�n�����֐��|�C���^				*/	FunPrgJogBegin,
/* ���s�����֐��|�C���^				*/	FunPrgJogMain,
/* �I�������֐��|�C���^				*/	FunPrgJogEnd,
};


/****************************************************************************************************/
/*		Fn005 : User Parameter Initialization														*/
/****************************************************************************************************/
CFUNDEF	apldef_fn005 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Prm Init   ",
/* �J�n�����֐��|�C���^				*/	FunUserParamInitBegin,
/* ���s�����֐��|�C���^				*/	FunUserParamInitMain,
/* �I�������֐��|�C���^				*/	FunUserParamInitEnd,
};


/****************************************************************************************************/
/*		Fn006 : Alarm Traceback Data Clear															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn006 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"AlmHist Clr",
/* �J�n�����֐��|�C���^				*/	FunAlarmTraceClearBegin,
/* ���s�����֐��|�C���^				*/	FunAlarmTraceClearMain,
/* �I�������֐��|�C���^				*/	FunAlarmTraceClearEnd,
};

/****************************************************************************************************/
/*		Fn008 : Multi Turn Data Reset & Encoder Alarm Reset											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn008 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Mturn Clr  ",
/* �J�n�����֐��|�C���^				*/	FunMultiTurnResetBegin,
/* ���s�����֐��|�C���^				*/	FunMultiTurnResetMain,
/* �I�������֐��|�C���^				*/	FunMultiTurnResetEnd,
};


/****************************************************************************************************/
/*		Fn00E : Current Offset Auto Adjustment														*/
/****************************************************************************************************/
CFUNDEF	apldef_fn00E = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* �p�����[�^�����֎~���A���s�s��	*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Cur AutoAdj",
/* �J�n�����֐��|�C���^				*/	FunCurrentAutoAdjBegin,
/* ���s�����֐��|�C���^				*/	FunCurrentAutoAdjMain,
/* �I�������֐��|�C���^				*/	FunCurrentAutoAdjEnd,
};


/****************************************************************************************************/
/*		Fn00F : Current Offset Manual Adjustment													*/
/****************************************************************************************************/
CFUNDEF	apldef_fn00F = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Cur ManuAdj",
/* �J�n�����֐��|�C���^				*/	FunCurrentManAdjBegin,
/* ���s�����֐��|�C���^				*/	FunCurrentManAdjMain,
/* �I�������֐��|�C���^				*/	FunCurrentManAdjEnd,
};


/****************************************************************************************************/
/*		Fn013 : Multi Turn Limit Setting															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn013 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"MturnLmtSet",
/* �J�n�����֐��|�C���^				*/	FunMultiTurnLimitSetBegin,
/* ���s�����֐��|�C���^				*/	FunMultiTurnLimitSetMain,
/* �I�������֐��|�C���^				*/	FunMultiTurnLimitSetEnd,
};

/****************************************************************************************************/
/*		Fn014 : Option Card Detection Clear															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn014 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Opt Init",
/* �J�n�����֐��|�C���^				*/	FunOptCardDetClearBegin,
/* ���s�����֐��|�C���^				*/	FunOptCardDetClearMain,
/* �I�������֐��|�C���^				*/	FunOptCardDetClearEnd,
};

/****************************************************************************************************/
/*		Fn01B : Init. Threshhold Level for Vibration Detect											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn01B = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"VibLvl Init",
/* �J�n�����֐��|�C���^				*/	FunVibDetLvSetBegin,
/* ���s�����֐��|�C���^				*/	FunVibDetLvSetMain,
/* �I�������֐��|�C���^				*/	FunVibDetLvSetEnd,
};

/****************************************************************************************************/
/*		Fn020 Scale Origin Setting																	*/
/****************************************************************************************************/
CFUNDEF	apldef_fn020 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"S-Orig Set ",
/* �J�n�����֐��|�C���^				*/	FunScaleOrgSetBegin,
/* ���s�����֐��|�C���^				*/	FunScaleOrgSetMain,
/* �I�������֐��|�C���^				*/	FunScaleOrgSetEnd,
};

/****************************************************************************************************/
/*		Fn030 Soft Reset Mode																		*/
/****************************************************************************************************/
CFUNDEF	apldef_fn030 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Soft Reset ",
/* �J�n�����֐��|�C���^				*/	FunSftRstBegin,
/* ���s�����֐��|�C���^				*/	FunSftRstMain,
/* �I�������֐��|�C���^				*/	FunSftRstEnd,
};


/****************************************************************************************************/
/*		Fn080 : Magnetic Pole Adjust																*/
/****************************************************************************************************/
CFUNDEF	apldef_fn080 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Pole Detect",
/* �J�n�����֐��|�C���^				*/	FunMagnePoleAdjustBegin,
/* ���s�����֐��|�C���^				*/	FunMagnePoleAdjustMain,
/* �I�������֐��|�C���^				*/	FunMagnePoleAdjustEnd,
};

/****************************************************************************************************/
/*		Fn100 : Test mode 1: AC On																	*/
/****************************************************************************************************/
CFUNDEF	apldef_fn100 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Test 1     ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
};


/****************************************************************************************************/
/*		Fn101 : Test mode 2: Regeneration Off														*/
/****************************************************************************************************/
CFUNDEF	apldef_fn101 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Test 2     ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
};


/****************************************************************************************************/
/*		Fn102 : Test mode 3: Overload Mask															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn102 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Test 3     ",
/* �J�n�����֐��|�C���^				*/	NULL,
/* ���s�����֐��|�C���^				*/	NULL,
/* �I�������֐��|�C���^				*/	NULL,
};

/****************************************************************************************************/
/*		Fn200 : Dynamic Auto Tuning Level Setting													*/
/****************************************************************************************************/
CFUNDEF	apldef_fn200 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"TuneLvlSet ",
/* �J�n�����֐��|�C���^				*/	FunDynamicAutoTuningBegin,
/* ���s�����֐��|�C���^				*/	FunDynamicAutoTuningMain,
/* �I�������֐��|�C���^				*/	FunDynamicAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn201 Advanced Auto Tuning Mode																*/
/****************************************************************************************************/
CFUNDEF	apldef_fn201 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"AAT     ",
/* �J�n�����֐��|�C���^				*/	FunAdvAutoTuningBegin,
/* ���s�����֐��|�C���^				*/	FunAdvAutoTuningMain,
/* �I�������֐��|�C���^				*/	FunAdvAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn202 Reference Input Type Advanced Auto Tuning Mode										*/
/****************************************************************************************************/
CFUNDEF	apldef_fn202 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Ref-AAT",
/* �J�n�����֐��|�C���^				*/	FunRefInAdvAutoTuningBegin,
/* ���s�����֐��|�C���^				*/	FunRefInAdvAutoTuningMain,
/* �I�������֐��|�C���^				*/	FunRefInAdvAutoTuningEnd,
};

/****************************************************************************************************/
/*		Fn203 : One Parameter Tuning																*/
/****************************************************************************************************/
CFUNDEF	apldef_fn203 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"OnePrmTun  ",
/* �J�n�����֐��|�C���^				*/	FunOneParamTuningBegin,
/* ���s�����֐��|�C���^				*/	FunOneParamTuningMain,
/* �I�������֐��|�C���^				*/	FunOneParamTuningEnd,
};

/****************************************************************************************************/
/*		Fn204 : A-type Vibration Suppression Control Tuning											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn204 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"A-Vib Sup",
/* �J�n�����֐��|�C���^				*/	FunAtypeVibTuningBegin,
/* ���s�����֐��|�C���^				*/	FunAtypeVibTuningMain,
/* �I�������֐��|�C���^				*/	FunAtypeVibTuningEnd,
};

/****************************************************************************************************/
/*		Fn205 : Model Following Control with Vibration Suppression Tuning							*/
/****************************************************************************************************/
CFUNDEF	apldef_fn205 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* �A�N�Z�X���x��					*/	ACCLVL_USER1,
/* �⏕�@�\����						*/	"Vib Sup    ",
/* �J�n�����֐��|�C���^				*/	FunVibMfcTuningBegin,
/* ���s�����֐��|�C���^				*/	FunVibMfcTuningMain,
/* �I�������֐��|�C���^				*/	FunVibMfcTuningEnd,
};

/****************************************************************************************************/
/*		Fn206 : EasyFFT																				*/
/****************************************************************************************************/
CFUNDEF	apldef_fn206 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Easy FFT   ",
/* �J�n�����֐��|�C���^				*/	FunEasyFftBegin,
/* ���s�����֐��|�C���^				*/	FunEasyFftMain,
/* �I�������֐��|�C���^				*/	FunEasyFftEnd,
};

/****************************************************************************************************/
/*		Fn207 V-Monitor Mode 																		*/
/****************************************************************************************************/
CFUNDEF	apldef_fn207 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"V-Monitor",
/* �J�n�����֐��|�C���^				*/	FunVibMonitorBegin,
/* ���s�����֐��|�C���^				*/	FunVibMonitorMain,
/* �I�������֐��|�C���^				*/	FunVibMonitorEnd,
};



/****************************************************************************************************/
/*		Fn300 : Register Editor																		*/
/****************************************************************************************************/
CFUNDEF	apldef_fn300 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* �A�N�Z�X���x��					*/	ACCLVL_SYSTEM,
/* �⏕�@�\����						*/	"Reg Edit   ",
/* �J�n�����֐��|�C���^				*/	FunRegEditBegin,
/* ���s�����֐��|�C���^				*/	FunRegEditMain,
/* �I�������֐��|�C���^				*/	FunRegEditEnd,
};


/****************************************************************************************************/
/*																									*/
/*		�⏕�@�\�Ǘ��e�[�u��																		*/
/*																									*/
/****************************************************************************************************/
CFUNTBL	FnAplTbl[] = {
/* FnNo,  FnMode,  FnDef																			*/
/*--------------------------------------------------------------------------------------------------*/
{ 0x0000, 0xFFFF, &apldef_fn000		},	/* Alarm Traceback Data Display								*/
{ 0x0002, 0xFFFF, &apldef_fn002		},	/* JOG														*/
{ 0x0003, 0xFFFF, &apldef_fn003		},	/* Origin Point Search										*/
{ 0x0004, 0xFFFF, &apldef_fn004		},	/* Program JOG												*/
{ 0x0005, 0xFFFF, &apldef_fn005		},	/* User Parameter Initialization							*/
{ 0x0006, 0xFFFF, &apldef_fn006		},	/* Alarm Traceback Data Clear								*/
{ 0x0008, 0xFFFF, &apldef_fn008		},	/* Multi Turn Data Reset & Encoder Alarm Reset				*/
{ 0x000E, 0xFFFF, &apldef_fn00E		},	/* Current Offset Auto Adjustment							*/
{ 0x000F, 0xFFFF, &apldef_fn00F		},	/* Current Offset Manual Adjustment							*/
{ 0x0013, 0xFFFF, &apldef_fn013		},	/* Multi Turn Limit Setting									*/
{ 0x0014, 0xFFFF, &apldef_fn014		},	/* Option Card Detection Clear								*/
{ 0x001B, 0xFFFF, &apldef_fn01B		},	/* Init. Threshhold Level for Vibration Detect				*/
{ 0x0020, 0xFFFF, &apldef_fn020		},	/* Scale Origin Setting 									*/
{ 0x0030, 0xFFFF, &apldef_fn030		},	/* Fn030 Soft Reset Mode									*/
{ 0x0080, 0xFFFF, &apldef_fn080		},	/* Magnetic Pole Adjust										*/
{ 0x0100, 0xFFFF, &apldef_fn100		},	/* Test mode 1: AC On										*/
{ 0x0101, 0xFFFF, &apldef_fn101		},	/* Test mode 2: Regeneration Off							*/
{ 0x0102, 0xFFFF, &apldef_fn102		},	/* Test mode 3: Overload Mask								*/
{ 0x0200, 0xFFFF, &apldef_fn200		},	/* Fn200 Dynamic Auto Tuning 								*/
{ 0x0201, 0xFFFF, &apldef_fn201		},	/* Fn201 Advanced Auto Tuning Mode							*/
{ 0x0202, 0xFFFF, &apldef_fn202		},	/* Fn202 Reference Input Type Advanced Auto Tuning Mode		*/
{ 0x0203, 0xFFFF, &apldef_fn203		},	/* Fn203 One Parameter Tuning 								*/
{ 0x0204, 0xFFFF, &apldef_fn204		},	/* A-type Vibration Suppression Control Tuning				*/
{ 0x0205, 0xFFFF, &apldef_fn205		},	/* Model Following Control with Vibration Suppression Tuning*/
{ 0x0206, 0xFFFF, &apldef_fn206		},	/* Fn206 Easy FFT			 								*/
{ 0x0207, 0xFFFF, &apldef_fn207		},	/* Fn207 V-Monitor Mode 									*/
{ 0x0300, 0xFFFF, &apldef_fn300		},	/* Reg Edit													*/
};
const	UINT	FnAplTblEntNum = sizeof(FnAplTbl)/sizeof(FnAplTbl[0]);







/***************************************** end of file **********************************************/
