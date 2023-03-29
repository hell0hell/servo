/****************************************************************************************************/
/*																									*/
/*																									*/
/*		XlsAplDef.h : Operator Application Header File												*/
/*																									*/
/*																									*/
/****************************************************************************************************/
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
#ifndef		___XLSAPLTBL_H___
#define		___XLSAPLTBL_H___
#include "KnlApi.h"
#include "FunManager.h" 			
#include "FunListDef.h"

/****************************************************************************************************/
/*																									*/
/*		Applications																				*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Fn000 : Alarm Trace Back Data Display														*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAlmTrcBegin(FUNMSG *Fmsg); 					/* Fn000 Alarm Trace Data Display Mode Begin	*/
LONG FunAlmTrcMain(FUNMSG *Fmsg);					/* Fn000 Alarm Trace Data Display Mode Main		*/
void FunAlmTrcEnd(FUNMSG *Fmsg);					/* Fn000 Alarm Trace Data Display Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn002 : JOG																					*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunJogBegin(FUNMSG *Fmsg); 					/* Fn002 Jog Mode Begin							*/
LONG FunJogMain(FUNMSG *Fmsg);						/* Fn002 Jog Mode Main							*/
void FunJogEnd(FUNMSG *Fmsg);						/* Fn002 Jog Mode End							*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn003 : Origin Point Search																	*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunOriginSearchBegin(FUNMSG *Fmsg); 			/* Fn003 Origin Point Search Mode Begin			*/
LONG FunOriginSearchMain(FUNMSG *Fmsg);				/* Fn003 Origin Point Search Mode Main			*/
void FunOriginSearchEnd(FUNMSG *Fmsg);				/* Fn003 Origin Point Search Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn004 : Program JOG																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunPrgJogBegin(FUNMSG *Fmsg); 						/* Fn004 Program Jog mode Begin				*/
LONG FunPrgJogMain(FUNMSG *Fmsg);						/* Fn004 Program Jog mode Display			*/
void FunPrgJogEnd(FUNMSG *Fmsg);						/* Fn004 Program Jog mode End				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn005 : Prm Init 																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunUserParamInitBegin(FUNMSG *Fmsg);				/* Fn005 ParamInit Mode Begin				*/
LONG FunUserParamInitMain(FUNMSG *Fmsg);				/* Fn005 ParamInit Mode Display				*/
void FunUserParamInitEnd(FUNMSG *Fmsg);					/* Fn005 ParamInit Mode End					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn006 : Alarm Traceback Data Clear															*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAlarmTraceClearBegin(FUNMSG *Fmsg); 	/* Fn006 Alarm Traceback Data Clear Begin			*/
LONG FunAlarmTraceClearMain(FUNMSG *Fmsg);		/* Fn006 Alarm Traceback Data Clear mode Display	*/
void FunAlarmTraceClearEnd(FUNMSG *Fmsg);		/* Fn006 Alarm Traceback Data Clear End				*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn008 : Multi Turn Data Reset & Encoder Alarm Reset											*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMultiTurnResetBegin(FUNMSG *Fmsg); 			/* Fn008 Multi Turn Data Reset & Encoder Alarm Reset Mode Begin	*/
LONG FunMultiTurnResetMain(FUNMSG *Fmsg);			/* Fn008 Multi Turn Data Reset & Encoder Alarm Reset Mode Main	*/
void FunMultiTurnResetEnd(FUNMSG *Fmsg);			/* Fn008 Multi Turn Data Reset & Encoder Alarm Reset Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn00E :  Motor Current Offset Auto Adjustment												*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunCurrentAutoAdjBegin(FUNMSG *Fmsg);			/* Fn00E Motor Current Offset Auto Adjustment Mode Begin	*/
LONG FunCurrentAutoAdjMain(FUNMSG *Fmsg);			/* Fn00E Motor Current Offset Auto Adjustment Mode Display	*/
void FunCurrentAutoAdjEnd(FUNMSG *Fmsg);			/* Fn00E Motor Current Offset Auto Adjustment Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn00F : Current Offset Manual Adjustment													*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunCurrentManAdjBegin(FUNMSG *Fmsg); 			/* Fn00F Current Offset Manual Adjustment Mode Begin	*/
LONG FunCurrentManAdjMain(FUNMSG *Fmsg);			/* Fn00F Current Offset Manual Adjustment Mode Main		*/
void FunCurrentManAdjEnd(FUNMSG *Fmsg);				/* Fn00F Current Offset Manual Adjustment Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn013 :  Multi Turn Limit Setting															*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMultiTurnLimitSetBegin(FUNMSG *Fmsg); 		/* Fn013 Multi Turn Limit Setting Mode Begin	*/
LONG FunMultiTurnLimitSetMain(FUNMSG *Fmsg);		/* Fn013 Multi Turn Limit Setting Mode Display	*/
void FunMultiTurnLimitSetEnd(FUNMSG *Fmsg);			/* Fn013 Multi Turn Limit Setting Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn014 :  Option Card Detection Clear														*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunOptCardDetClearBegin(FUNMSG *Fmsg); 		/* Fn014 Option Card Detection Clear Mode Begin	*/
LONG FunOptCardDetClearMain(FUNMSG *Fmsg);			/* Fn014 Option Card Detection Clear Main		*/
void FunOptCardDetClearEnd(FUNMSG *Fmsg);			/* Fn014 Option Card Detection Clear Mode End	*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn01B :  Init. Threshhold Level for Vibration Detect										*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunVibDetLvSetBegin(FUNMSG *Fmsg); 			/* Fn01B Init. Threshhold Level for Vibration Detect Mode Begin		*/
LONG FunVibDetLvSetMain(FUNMSG *Fmsg);				/* Fn01B Init. Threshhold Level for Vibration Detect Mode Display	*/
void FunVibDetLvSetEnd(FUNMSG *Fmsg);				/* Fn01B Init. Threshhold Level for Vibration Detect Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn020 :	 Scale Origin Setting																*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunScaleOrgSetBegin(FUNMSG *Fmsg); 			/* Fn020 Scale Origin Setting Mode Begin		*/
LONG FunScaleOrgSetMain(FUNMSG *Fmsg);				/* Fn020 Scale Origin Setting Mode Display		*/
void FunScaleOrgSetEnd(FUNMSG *Fmsg);				/* Fn020 Scale Origin Setting Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn030 Soft Reset Mode																		*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunSftRstBegin(FUNMSG *Fmsg);					/* Fn030 Soft Reset Mode Begin					*/
LONG FunSftRstMain(FUNMSG *Fmsg);					/* Fn030 Soft Reset Mode Display				*/
void FunSftRstEnd(FUNMSG *Fmsg);					/* Fn030 Soft Reset Mode End					*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn080 :  Magnetic Pole Adjust																*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMagnePoleAdjustBegin(FUNMSG *Fmsg); 		/* Fn080 Magnetic Pole Adjust Mode Begin		*/
LONG FunMagnePoleAdjustMain(FUNMSG *Fmsg);			/* Fn080 Magnetic Pole Adjust Mode Display		*/
void FunMagnePoleAdjustEnd(FUNMSG *Fmsg);			/* Fn080 Magnetic Pole Adjust Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn200 :  Dynamic Auto Tuning																*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunDynamicAutoTuningBegin(FUNMSG *Fmsg); 		/* Fn200 Dynamic Auto Tuning Mode Begin			*/
LONG FunDynamicAutoTuningMain(FUNMSG *Fmsg);		/* Fn200 Dynamic Auto Tuning Mode Display		*/
void FunDynamicAutoTuningEnd(FUNMSG *Fmsg);			/* Fn200 Dynamic Auto Tuning Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn201 Advanced Auto Tuning Mode																*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAdvAutoTuningBegin(FUNMSG *Fmsg);				/* Fn201 Advanced Auto Tuning Mode Begin	*/
LONG FunAdvAutoTuningMain(FUNMSG *Fmsg);				/* Fn201 Advanced Auto Tuning Mode Main		*/
void FunAdvAutoTuningEnd(FUNMSG *Fmsg);					/* Fn201 Advanced Auto Tuning Mode End		*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn202 Reference Input Type Advanced Auto Tuning Mode										*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunRefInAdvAutoTuningBegin(FUNMSG *Fmsg);		/* Fn202 Reference Input Type Advanced Auto Tuning Mode Begin		*/
LONG FunRefInAdvAutoTuningMain(FUNMSG *Fmsg);		/* Fn202 Reference Input Type Advanced Auto Tuning Mode Main		*/
void FunRefInAdvAutoTuningEnd(FUNMSG *Fmsg);		/* Fn202 Reference Input Type Advanced Auto Tuning Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn203 :  One Parameter Tuning																*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunOneParamTuningBegin(FUNMSG *Fmsg); 			/* Fn203 One Parameter Tuning Mode Begin		*/
LONG FunOneParamTuningMain(FUNMSG *Fmsg);			/* Fn203 One Parameter Tuning Mode Display		*/
void FunOneParamTuningEnd(FUNMSG *Fmsg);			/* Fn203 One Parameter Tuning Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn204 :  A-type Vibration Suppression Control Tuning										*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAtypeVibTuningBegin(FUNMSG *Fmsg); 			/* Fn204 A-type Vibration Suppression Control Tuning Mode Begin			*/
LONG FunAtypeVibTuningMain(FUNMSG *Fmsg);			/* Fn204 A-type Vibration Suppression Control Tuning Mode Display		*/
void FunAtypeVibTuningEnd(FUNMSG *Fmsg);			/* Fn204 A-type Vibration Suppression Control Tuning Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn205 :  Model Following Control with Vibration Suppression Tuning							*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunVibMfcTuningBegin(FUNMSG *Fmsg); 			/* Fn205 Model Following Control with Vibration Suppression Tuning Mode Begin			*/
LONG FunVibMfcTuningMain(FUNMSG *Fmsg);				/* Fn205 Model Following Control with Vibration Suppression Tuning Mode Display		*/
void FunVibMfcTuningEnd(FUNMSG *Fmsg);				/* Fn205 Model Following Control with Vibration Suppression Tuning Mode End			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Fn206 :  Easy FFT																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunEasyFftBegin(FUNMSG *Fmsg); 				/* Fn206 Easy FFT Mode Begin					*/
LONG FunEasyFftMain(FUNMSG *Fmsg);					/* Fn206 Easy FFT Mode Display					*/
void FunEasyFftEnd(FUNMSG *Fmsg);					/* Fn206 Easy FFT Mode End						*/

/*--------------------------------------------------------------------------------------------------*/
/*		 Fn207 V-Monitor Mode																		*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunVibMonitorBegin(FUNMSG *Fmsg);				/* Fn207 V-Monitor Mode Begin				*/
LONG FunVibMonitorMain(FUNMSG *Fmsg);				/* Fn207 V-Monitor Mode Display				*/
void FunVibMonitorEnd(FUNMSG *Fmsg);				/* Fn207 V-Monitor Mode End					*/


/*--------------------------------------------------------------------------------------------------*/
/*		Fn300 : REG EDIT																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunRegEditBegin(FUNMSG *Fmsg); 				/* Fn300 Register Editor Display Mode Begin		*/
LONG FunRegEditMain(FUNMSG *Fmsg);					/* Fn300 Register Editor Display Mode Main		*/
void FunRegEditEnd(FUNMSG *Fmsg);					/* Fn300 Register Editor Display Mode End		*/


/****************************************************************************************************/
/*																									*/
/*		Application Management Table																*/
/*																									*/
/****************************************************************************************************/
extern  const FUNTBL FnAplTbl[];				/* 補助機能管理テーブル								*/
extern  const UINT FnAplTblEntNum;				/* 補助機能管理テーブル登録個数						*/



#endif
/***************************************** end of file **********************************************/
