/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsAplTbl.c : Operator Application Definition												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. 補助機能定義																				*/
/*																									*/
/*		2. 補助機能管理テーブル																		*/
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
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Alm History",
/* 開始処理関数ポインタ				*/	FunAlmTrcBegin,
/* 実行処理関数ポインタ				*/	FunAlmTrcMain,
/* 終了処理関数ポインタ				*/	FunAlmTrcEnd,
};

/****************************************************************************************************/
/*		Fn002 : JOG																					*/
/****************************************************************************************************/
CFUNDEF	apldef_fn002 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"JOG        ",
/* 開始処理関数ポインタ				*/	FunJogBegin,
/* 実行処理関数ポインタ				*/	FunJogMain,
/* 終了処理関数ポインタ				*/	FunJogEnd,
};


/****************************************************************************************************/
/*		Fn003 : Origin Point Search																	*/
/****************************************************************************************************/
CFUNDEF	apldef_fn003 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Z-Search   ",
/* 開始処理関数ポインタ				*/	FunOriginSearchBegin,
/* 実行処理関数ポインタ				*/	FunOriginSearchMain,
/* 終了処理関数ポインタ				*/	FunOriginSearchEnd,
};


/****************************************************************************************************/
/*		Fn004 : Program JOG																			*/
/****************************************************************************************************/
CFUNDEF	apldef_fn004 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Program JOG",
/* 開始処理関数ポインタ				*/	FunPrgJogBegin,
/* 実行処理関数ポインタ				*/	FunPrgJogMain,
/* 終了処理関数ポインタ				*/	FunPrgJogEnd,
};


/****************************************************************************************************/
/*		Fn005 : User Parameter Initialization														*/
/****************************************************************************************************/
CFUNDEF	apldef_fn005 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Prm Init   ",
/* 開始処理関数ポインタ				*/	FunUserParamInitBegin,
/* 実行処理関数ポインタ				*/	FunUserParamInitMain,
/* 終了処理関数ポインタ				*/	FunUserParamInitEnd,
};


/****************************************************************************************************/
/*		Fn006 : Alarm Traceback Data Clear															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn006 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"AlmHist Clr",
/* 開始処理関数ポインタ				*/	FunAlarmTraceClearBegin,
/* 実行処理関数ポインタ				*/	FunAlarmTraceClearMain,
/* 終了処理関数ポインタ				*/	FunAlarmTraceClearEnd,
};

/****************************************************************************************************/
/*		Fn008 : Multi Turn Data Reset & Encoder Alarm Reset											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn008 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Mturn Clr  ",
/* 開始処理関数ポインタ				*/	FunMultiTurnResetBegin,
/* 実行処理関数ポインタ				*/	FunMultiTurnResetMain,
/* 終了処理関数ポインタ				*/	FunMultiTurnResetEnd,
};


/****************************************************************************************************/
/*		Fn00E : Current Offset Auto Adjustment														*/
/****************************************************************************************************/
CFUNDEF	apldef_fn00E = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* パラメータ書込禁止時、実行不可	*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Cur AutoAdj",
/* 開始処理関数ポインタ				*/	FunCurrentAutoAdjBegin,
/* 実行処理関数ポインタ				*/	FunCurrentAutoAdjMain,
/* 終了処理関数ポインタ				*/	FunCurrentAutoAdjEnd,
};


/****************************************************************************************************/
/*		Fn00F : Current Offset Manual Adjustment													*/
/****************************************************************************************************/
CFUNDEF	apldef_fn00F = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Cur ManuAdj",
/* 開始処理関数ポインタ				*/	FunCurrentManAdjBegin,
/* 実行処理関数ポインタ				*/	FunCurrentManAdjMain,
/* 終了処理関数ポインタ				*/	FunCurrentManAdjEnd,
};


/****************************************************************************************************/
/*		Fn013 : Multi Turn Limit Setting															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn013 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"MturnLmtSet",
/* 開始処理関数ポインタ				*/	FunMultiTurnLimitSetBegin,
/* 実行処理関数ポインタ				*/	FunMultiTurnLimitSetMain,
/* 終了処理関数ポインタ				*/	FunMultiTurnLimitSetEnd,
};

/****************************************************************************************************/
/*		Fn014 : Option Card Detection Clear															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn014 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	FALSE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Opt Init",
/* 開始処理関数ポインタ				*/	FunOptCardDetClearBegin,
/* 実行処理関数ポインタ				*/	FunOptCardDetClearMain,
/* 終了処理関数ポインタ				*/	FunOptCardDetClearEnd,
};

/****************************************************************************************************/
/*		Fn01B : Init. Threshhold Level for Vibration Detect											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn01B = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"VibLvl Init",
/* 開始処理関数ポインタ				*/	FunVibDetLvSetBegin,
/* 実行処理関数ポインタ				*/	FunVibDetLvSetMain,
/* 終了処理関数ポインタ				*/	FunVibDetLvSetEnd,
};

/****************************************************************************************************/
/*		Fn020 Scale Origin Setting																	*/
/****************************************************************************************************/
CFUNDEF	apldef_fn020 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"S-Orig Set ",
/* 開始処理関数ポインタ				*/	FunScaleOrgSetBegin,
/* 実行処理関数ポインタ				*/	FunScaleOrgSetMain,
/* 終了処理関数ポインタ				*/	FunScaleOrgSetEnd,
};

/****************************************************************************************************/
/*		Fn030 Soft Reset Mode																		*/
/****************************************************************************************************/
CFUNDEF	apldef_fn030 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Soft Reset ",
/* 開始処理関数ポインタ				*/	FunSftRstBegin,
/* 実行処理関数ポインタ				*/	FunSftRstMain,
/* 終了処理関数ポインタ				*/	FunSftRstEnd,
};


/****************************************************************************************************/
/*		Fn080 : Magnetic Pole Adjust																*/
/****************************************************************************************************/
CFUNDEF	apldef_fn080 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Pole Detect",
/* 開始処理関数ポインタ				*/	FunMagnePoleAdjustBegin,
/* 実行処理関数ポインタ				*/	FunMagnePoleAdjustMain,
/* 終了処理関数ポインタ				*/	FunMagnePoleAdjustEnd,
};

/****************************************************************************************************/
/*		Fn100 : Test mode 1: AC On																	*/
/****************************************************************************************************/
CFUNDEF	apldef_fn100 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Test 1     ",
/* 開始処理関数ポインタ				*/	NULL,
/* 実行処理関数ポインタ				*/	NULL,
/* 終了処理関数ポインタ				*/	NULL,
};


/****************************************************************************************************/
/*		Fn101 : Test mode 2: Regeneration Off														*/
/****************************************************************************************************/
CFUNDEF	apldef_fn101 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Test 2     ",
/* 開始処理関数ポインタ				*/	NULL,
/* 実行処理関数ポインタ				*/	NULL,
/* 終了処理関数ポインタ				*/	NULL,
};


/****************************************************************************************************/
/*		Fn102 : Test mode 3: Overload Mask															*/
/****************************************************************************************************/
CFUNDEF	apldef_fn102 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Test 3     ",
/* 開始処理関数ポインタ				*/	NULL,
/* 実行処理関数ポインタ				*/	NULL,
/* 終了処理関数ポインタ				*/	NULL,
};

/****************************************************************************************************/
/*		Fn200 : Dynamic Auto Tuning Level Setting													*/
/****************************************************************************************************/
CFUNDEF	apldef_fn200 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"TuneLvlSet ",
/* 開始処理関数ポインタ				*/	FunDynamicAutoTuningBegin,
/* 実行処理関数ポインタ				*/	FunDynamicAutoTuningMain,
/* 終了処理関数ポインタ				*/	FunDynamicAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn201 Advanced Auto Tuning Mode																*/
/****************************************************************************************************/
CFUNDEF	apldef_fn201 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"AAT     ",
/* 開始処理関数ポインタ				*/	FunAdvAutoTuningBegin,
/* 実行処理関数ポインタ				*/	FunAdvAutoTuningMain,
/* 終了処理関数ポインタ				*/	FunAdvAutoTuningEnd,
};


/****************************************************************************************************/
/*		Fn202 Reference Input Type Advanced Auto Tuning Mode										*/
/****************************************************************************************************/
CFUNDEF	apldef_fn202 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Ref-AAT",
/* 開始処理関数ポインタ				*/	FunRefInAdvAutoTuningBegin,
/* 実行処理関数ポインタ				*/	FunRefInAdvAutoTuningMain,
/* 終了処理関数ポインタ				*/	FunRefInAdvAutoTuningEnd,
};

/****************************************************************************************************/
/*		Fn203 : One Parameter Tuning																*/
/****************************************************************************************************/
CFUNDEF	apldef_fn203 = {
/* No.1/2 OpeReg ParaRun			*/	2,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"OnePrmTun  ",
/* 開始処理関数ポインタ				*/	FunOneParamTuningBegin,
/* 実行処理関数ポインタ				*/	FunOneParamTuningMain,
/* 終了処理関数ポインタ				*/	FunOneParamTuningEnd,
};

/****************************************************************************************************/
/*		Fn204 : A-type Vibration Suppression Control Tuning											*/
/****************************************************************************************************/
CFUNDEF	apldef_fn204 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"A-Vib Sup",
/* 開始処理関数ポインタ				*/	FunAtypeVibTuningBegin,
/* 実行処理関数ポインタ				*/	FunAtypeVibTuningMain,
/* 終了処理関数ポインタ				*/	FunAtypeVibTuningEnd,
};

/****************************************************************************************************/
/*		Fn205 : Model Following Control with Vibration Suppression Tuning							*/
/****************************************************************************************************/
CFUNDEF	apldef_fn205 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	FALSE,
/* アクセスレベル					*/	ACCLVL_USER1,
/* 補助機能略称						*/	"Vib Sup    ",
/* 開始処理関数ポインタ				*/	FunVibMfcTuningBegin,
/* 実行処理関数ポインタ				*/	FunVibMfcTuningMain,
/* 終了処理関数ポインタ				*/	FunVibMfcTuningEnd,
};

/****************************************************************************************************/
/*		Fn206 : EasyFFT																				*/
/****************************************************************************************************/
CFUNDEF	apldef_fn206 = {
/* No.1/2 OpeReg ParaRun			*/	1,
/* ParaRun with DataTrace			*/	FALSE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Easy FFT   ",
/* 開始処理関数ポインタ				*/	FunEasyFftBegin,
/* 実行処理関数ポインタ				*/	FunEasyFftMain,
/* 終了処理関数ポインタ				*/	FunEasyFftEnd,
};

/****************************************************************************************************/
/*		Fn207 V-Monitor Mode 																		*/
/****************************************************************************************************/
CFUNDEF	apldef_fn207 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"V-Monitor",
/* 開始処理関数ポインタ				*/	FunVibMonitorBegin,
/* 実行処理関数ポインタ				*/	FunVibMonitorMain,
/* 終了処理関数ポインタ				*/	FunVibMonitorEnd,
};



/****************************************************************************************************/
/*		Fn300 : Register Editor																		*/
/****************************************************************************************************/
CFUNDEF	apldef_fn300 = {
/* No.1/2 OpeReg ParaRun			*/	0,
/* ParaRun with DataTrace			*/	TRUE,
/* Common mode to all Axes			*/	TRUE,
/* Parameter Write prohibited		*/	TRUE,
/* アクセスレベル					*/	ACCLVL_SYSTEM,
/* 補助機能略称						*/	"Reg Edit   ",
/* 開始処理関数ポインタ				*/	FunRegEditBegin,
/* 実行処理関数ポインタ				*/	FunRegEditMain,
/* 終了処理関数ポインタ				*/	FunRegEditEnd,
};


/****************************************************************************************************/
/*																									*/
/*		補助機能管理テーブル																		*/
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
