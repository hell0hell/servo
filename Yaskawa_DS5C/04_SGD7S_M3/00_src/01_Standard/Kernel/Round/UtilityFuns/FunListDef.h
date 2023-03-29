/****************************************************************************************************/
/*																									*/
/*																									*/
/*		FunListDef.h : 補助機能定義ヘッダファイル													*/
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
/*		2014.03.18 : Made by Excel Macro															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#ifndef _FUN_LIST_DEF_
#define _FUN_LIST_DEF_



#include "KnlApi.h"
#include "FunManager.h"





/****************************************************************************************************/
/*																									*/
/*		補助機能実行関数																			*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/*		Fn000 : Alarm Traceback Data Display: Normal mode											*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAlmTrcBegin(FUNMSG *Fmsg);
LONG FunAlmTrcMain(FUNMSG *Fmsg);
void FunAlmTrcEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn002 : JOG																					*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterJOGoperation(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteJOGoperation(FUNEXE *FunExe, void *Axis);
void     FnLeaveJOGoperation(FUNEXE *FunExe, void *Axis);
LONG FunJogBegin(FUNMSG *Fmsg);
LONG FunJogMain(FUNMSG *Fmsg);
void FunJogEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn003 : Origin Point Search																	*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FunEnterOriginSearch(FUNEXE *FunExe, void *Axis);
PRM_RSLT FunExecuteOriginSearch(FUNEXE *FunExe, void *Axis);
void     FunLeaveOriginSearch(FUNEXE *FunExe, void *Axis);
LONG FunOriginSearchBegin(FUNMSG *Fmsg);
LONG FunOriginSearchMain(FUNMSG *Fmsg);
void FunOriginSearchEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn004 : Program JOG																			*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterPrgJog(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecutePrgJog(FUNEXE *FunExe, void *Axis);
void     FnLeavePrgJog(FUNEXE *FunExe, void *Axis);
LONG FunPrgJogBegin(FUNMSG *Fmsg);
LONG FunPrgJogMain(FUNMSG *Fmsg);
void FunPrgJogEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn005 : User Parameter Initialization														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterPrmInit(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecutePrmInit(FUNEXE *FunExe, void *Axis);
LONG FunUserParamInitBegin(FUNMSG *Fmsg);
LONG FunUserParamInitMain(FUNMSG *Fmsg);
void FunUserParamInitEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn006 : Alarm Traceback Data Clear															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnExecuteTracebackClear(FUNEXE *FunExe, void *Axis);
LONG FunAlarmTraceClearBegin(FUNMSG *Fmsg);
LONG FunAlarmTraceClearMain(FUNMSG *Fmsg);
void FunAlarmTraceClearEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn008 : Multi Turn Data Reset & Encoder Alarm Reset											*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMultiTurnReset(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMultiTurnReset(FUNEXE *FunExe, void *Axis);
void     FnLeaveMultiTurnReset(FUNEXE *FunExe, void *Axis);
LONG FunMultiTurnResetBegin(FUNMSG *Fmsg);
LONG FunMultiTurnResetMain(FUNMSG *Fmsg);
void FunMultiTurnResetEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn009 : Analog Reference Offset Auto Adjustment												*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAnlgRefOffsetAutoAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAnlgRefOffsetAutoAdj(FUNEXE *FunExe, void *Axis);
LONG FunAnlgRefAutoAdjBegin(FUNMSG *Fmsg);
LONG FunAnlgRefAutoAdjMain(FUNMSG *Fmsg);
void FunAnlgRefAutoAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn00A : Velocity Reference Manual Adjustment												*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAnlgVelocityRefManualAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAnlgVelocityRefManualAdj(FUNEXE *FunExe, void *Axis);
void     FnLeaveAnlgVelocityRefManualAdj(FUNEXE *FunExe, void *Axis);
LONG FunAnlgVelRefManAdjBegin(FUNMSG *Fmsg);
LONG FunAnlgVelRefManAdjMain(FUNMSG *Fmsg);
void FunAnlgVelRefManAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn00B : Torque Reference Manual Adjustment													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAnlgTorqueRefManualAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAnlgTorqueRefManualAdj(FUNEXE *FunExe, void *Axis);
void     FnLeaveAnlgTorqueRefManualAdj(FUNEXE *FunExe, void *Axis);
LONG FunAnlgTrqRefManAdjBegin(FUNMSG *Fmsg);
LONG FunAnlgTrqRefManAdjMain(FUNMSG *Fmsg);
void FunAnlgTrqRefManAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn00C : Analog Monitor Offset Manual Adjustment												*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAnlgMonOffsetsManualAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAnlgMonOffsetsManualAdj(FUNEXE *FunExe, void *Axis);
void     FnLeaveAnlgMonOffsetsManualAdj(FUNEXE *FunExe, void *Axis);
LONG FunAnlgMonOffManAdjBegin(FUNMSG *Fmsg);
LONG FunAnlgMonOffManAdjMain(FUNMSG *Fmsg);
void FunAnlgMonOffManAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn00D : Analog Monitor Gain Manual Adjustment												*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAnlgMonGainManualAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAnlgMonGainManualAdj(FUNEXE *FunExe, void *Axis);
void     FnLeaveAnlgMonGainManualAdj(FUNEXE *FunExe, void *Axis);
LONG FunAnlgMonGainManAdjBegin(FUNMSG *Fmsg);
LONG FunAnlgMonGainManAdjMain(FUNMSG *Fmsg);
void FunAnlgMonGainManAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn00E : Current Offset Auto Adjustment														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMotorCurrentAutoAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMotorCurrentAutoAdj(FUNEXE *FunExe, void *Axis);
void     FnLeaveMotorCurrentAutoAdj(FUNEXE *FunExe, void *Axis);
LONG FunCurrentAutoAdjBegin(FUNMSG *Fmsg);
LONG FunCurrentAutoAdjMain(FUNMSG *Fmsg);
void FunCurrentAutoAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn00F : Current Offset Manual Adjustment													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMotorCurrentManualAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMotorCurrentManualAdj(FUNEXE *FunExe, void *Axis);
void     FnLeaveMotorCurrentManualAdj(FUNEXE *FunExe, void *Axis);
LONG FunCurrentManAdjBegin(FUNMSG *Fmsg);
LONG FunCurrentManAdjMain(FUNMSG *Fmsg);
void FunCurrentManAdjEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn010 : Rewriting Inhibition Processing Display												*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunAccessSetBegin(FUNMSG *Fmsg);
LONG FunAccessSetMain(FUNMSG *Fmsg);
void FunAccessSetEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn011 : Motot Type & Y Specification Display												*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunMotorTypeBegin(FUNMSG *Fmsg);
LONG FunMotorTypeMain(FUNMSG *Fmsg);
void FunMotorTypeEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn012 : Software Version Display															*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunSoftVersionBegin(FUNMSG *Fmsg);
LONG FunSoftVersionMain(FUNMSG *Fmsg);
void FunSoftVersionEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn013 : Multi Turn Limit Setting															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMultiTurnLimitSetting(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMultiTurnLimitSetting(FUNEXE *FunExe, void *Axis);
void     FnLeaveMultiTurnLimitSetting(FUNEXE *FunExe, void *Axis);
LONG FunMultiTurnLimitSetBegin(FUNMSG *Fmsg);
LONG FunMultiTurnLimitSetMain(FUNMSG *Fmsg);
void FunMultiTurnLimitSetEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn014 : Option Card Detection Clear															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterModuleInfoReset(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteModuleInfoReset(FUNEXE *FunExe, void *Axis);
LONG FunOptCardDetClearBegin(FUNMSG *Fmsg);
LONG FunOptCardDetClearMain(FUNMSG *Fmsg);
void FunOptCardDetClearEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn01B : Init. Threshhold Level for Vibration Detect											*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterInitVibDetLevel(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteInitVibDetLevel(FUNEXE *FunExe, void *Axis);
void     FnLeaveInitVibDetLevel(FUNEXE *FunExe, void *Axis);
LONG FunVibDetLvSetBegin(FUNMSG *Fmsg);
LONG FunVibDetLvSetMain(FUNMSG *Fmsg);
void FunVibDetLvSetEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn01E : Servo & Motor ID Display															*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunSvMotOpIDBegin(FUNMSG *Fmsg);
LONG FunSvMotOpIDMain(FUNMSG *Fmsg);
void FunSvMotOpIDEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn020 : Scale Origin Setting																*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterScaleZeroSetting(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteScaleZeroSetting(FUNEXE *FunExe, void *Axis);
void     FnLeaveScaleZeroSetting(FUNEXE *FunExe, void *Axis);
LONG FunScaleOrgSetBegin(FUNMSG *Fmsg);
LONG FunScaleOrgSetMain(FUNMSG *Fmsg);
void FunScaleOrgSetEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn030 : Software Reset Mode																	*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterSoftwareReset(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteSoftwareReset(FUNEXE *FunExe, void *Axis);
LONG FunSftRstBegin(FUNMSG *Fmsg);
LONG FunSftRstMain(FUNMSG *Fmsg);
void FunSftRstEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn080 : Magnetic Pole Adjust 																*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMagneticPoleDetection(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMagneticPoleDetection(FUNEXE *FunExe, void *Axis);
void     FnLeaveMagneticPoleDetection(FUNEXE *FunExe, void *Axis);
LONG FunMagnePoleAdjustBegin(FUNMSG *Fmsg);
LONG FunMagnePoleAdjustMain(FUNMSG *Fmsg);
void FunMagnePoleAdjustEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn081 : Magnetic Pole Information Copy														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMagneticPoleCopy(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMagneticPoleCopy(FUNEXE *FunExe, void *Axis);
void     FnLeaveMagneticPoleCopy(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn100 : Test mode 1: AC On																	*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterTetMod(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteTetMod1(FUNEXE *FunExe, void *Axis);
LONG FunTstMod1Begin(FUNMSG *Fmsg);
LONG FunTstMod1Main(FUNMSG *Fmsg);
void FunTstModEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn101 : Test mode 2: Regeneration Off														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterTetMod(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteTetMod2(FUNEXE *FunExe, void *Axis);
LONG FunTstMod2Begin(FUNMSG *Fmsg);
LONG FunTstMod2Main(FUNMSG *Fmsg);
void FunTstModEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn102 : Test mode 3: Overload Mask															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterTetMod(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteTetMod3(FUNEXE *FunExe, void *Axis);
LONG FunTstMod3Begin(FUNMSG *Fmsg);
LONG FunTstMod3Main(FUNMSG *Fmsg);
void FunTstModEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn103 : RAM Editor																			*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunRamEditBegin(FUNMSG *Fmsg);
LONG FunRamEditMain(FUNMSG *Fmsg);
void FunRamEditEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn110 : Built-in FAN Lifetime Reset															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnExecuteBFanLtRst(FUNEXE *FunExe, void *Axis);
LONG FunBFanLtRstBegin(FUNMSG *Fmsg);
LONG FunBFanLtRstMain(FUNMSG *Fmsg);
void FunBFanLtRstEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn200 : Dynamic Auto Tuning Level Setting													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterSetTuneLessLevel(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteSetTuneLessLevel(FUNEXE *FunExe, void *Axis);
void     FnLeaveSetTuneLessLevel(FUNEXE *FunExe, void *Axis);
LONG FunDynamicAutoTuningBegin(FUNMSG *Fmsg);
LONG FunDynamicAutoTuningMain(FUNMSG *Fmsg);
void FunDynamicAutoTuningEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn201 : Advanced Autotuning																	*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAdvAutoTuning(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAdvAutoTuning(FUNEXE *FunExe, void *Axis);
void     FnLeaveAdvAutoTuning(FUNEXE *FunExe, void *Axis);
LONG FunAdvAutoTuningBegin(FUNMSG *Fmsg);
LONG FunAdvAutoTuningMain(FUNMSG *Fmsg);
void FunAdvAutoTuningEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn202 : Refernce Input type Advanced Autotuning												*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterRefInAdvAutoTuning(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteRefInAdvAutoTuning(FUNEXE *FunExe, void *Axis);
void     FnLeaveRefInAdvAutoTuning(FUNEXE *FunExe, void *Axis);
LONG FunRefInAdvAutoTuningBegin(FUNMSG *Fmsg);
LONG FunRefInAdvAutoTuningMain(FUNMSG *Fmsg);
void FunRefInAdvAutoTuningEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn203 : One Parameter Tuning																*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterOnePrmTuning(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteOnePrmTuning(FUNEXE *FunExe, void *Axis);
void     FnLeaveOnePrmTuning(FUNEXE *FunExe, void *Axis);
LONG FunOneParamTuningBegin(FUNMSG *Fmsg);
LONG FunOneParamTuningMain(FUNMSG *Fmsg);
void FunOneParamTuningEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn204 : A-type Vibration Suppression Control Tuning											*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAtypeVibTuning(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAtypeVibTuning(FUNEXE *FunExe, void *Axis);
void     FnLeaveAtypeVibTuning(FUNEXE *FunExe, void *Axis);
LONG FunAtypeVibTuningBegin(FUNMSG *Fmsg);
LONG FunAtypeVibTuningMain(FUNMSG *Fmsg);
void FunAtypeVibTuningEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn205 : Model Following Control with Vibration Suppression Tuning							*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMfcTuning(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMfcTuning(FUNEXE *FunExe, void *Axis);
void     FnLeaveMfcTuning(FUNEXE *FunExe, void *Axis);
LONG FunVibMfcTuningBegin(FUNMSG *Fmsg);
LONG FunVibMfcTuningMain(FUNMSG *Fmsg);
void FunVibMfcTuningEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn206 : EasyFFT																				*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterEasyFft(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteEasyFft(FUNEXE *FunExe, void *Axis);
void     FnLeaveEasyFft(FUNEXE *FunExe, void *Axis);
LONG FunEasyFftBegin(FUNMSG *Fmsg);
LONG FunEasyFftMain(FUNMSG *Fmsg);
void FunEasyFftEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn207 : Vibration Freq. Display and Notchi Filter Autoset									*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterVibrationMonitor(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteVibrationMonitor(FUNEXE *FunExe, void *Axis);
void     FnLeaveVibrationMonitor(FUNEXE *FunExe, void *Axis);
LONG FunVibMonitorBegin(FUNMSG *Fmsg);
LONG FunVibMonitorMain(FUNMSG *Fmsg);
void FunVibMonitorEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		Fn300 : Register Editor																		*/
/*--------------------------------------------------------------------------------------------------*/
LONG FunRegEditBegin(FUNMSG *Fmsg);
LONG FunRegEditMain(FUNMSG *Fmsg);
void FunRegEditEnd(FUNMSG *Fmsg);
/*--------------------------------------------------------------------------------------------------*/
/*		1090H : Encoder Data Alarm Reset															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnExecuteEncDataAlmClear(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		200DH :  Velocity Ripple Compensation Information Write										*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterRippleComp(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteRippleComp(FUNEXE *FunExe, void *Axis);
void     FnLeaveRippleComp(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2001H : Alarm Reset (only Register IF)														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnExecuteAlarmResetting(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2003H : Command Pulse Counter Clear (only Register IF)										*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FunPlsCntClrBegin(FUNEXE *FunExe, void *Axis);
PRM_RSLT FunPlsCntClrExe(FUNEXE *FunExe, void *Axis);
void     FunPlsCntClrEnd(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2004H : Feedback Pulse Counter Clear (only Register IF)										*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FunFbkPlsCntBegin(FUNEXE *FunExe, void *Axis);
PRM_RSLT FunFbkPlsCntExe(FUNEXE *FunExe, void *Axis);
void     FunFbkPlsCntEnd(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2007H : Table Command (only Register IF)													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterTableCommandMode(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteTableCommandMode(FUNEXE *FunExe, void *Axis);
void     FnLeaveTableCommandMode(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2008H : Parameter Recalcuration (only Register IF)											*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterPrmRecalc(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecutePrmRecalc(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2009H : Write RAM Parameter to EEPROM (only Register IF)									*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterStoreParameters(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteStoreParameters(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		200BH : Read Motor Parameters from Scale													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMotorParameterRead(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMotorParameterRead(FUNEXE *FunExe, void *Axis);
void     FnLeaveMotorParameterRead(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		200CH : Write Motor Parameters to Scale														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterMotorParameterWrite(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteMotorParameterWrite(FUNEXE *FunExe, void *Axis);
void     FnLeaveMotorParameterWrite(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		200EH : HW OutPut Signal By Force															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterForceSignalOut(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteForceSignalOut(FUNEXE *FunExe, void *Axis);
void     FnLeaveForceSignalOut(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		200FH : ALL Servo Parameter EEPROM Read														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT RegServoPrmFileReadBegin(FUNEXE *FunExe, void *Axis);
PRM_RSLT RegServoPrmFileReadExe(FUNEXE *FunExe, void *Axis);
void     RegServoPrmFileReadEnd(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2010H : ALL Servo Parameter EEPROM Write													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT RegServoPrmFileWriteBegin(FUNEXE *FunExe, void *Axis);
PRM_RSLT RegServoPrmFileWriteExe(FUNEXE *FunExe, void *Axis);
void     RegServoPrmFileWriteEnd(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2011H : SDA Parameter Initialization														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterSDAPrmInit(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteSDAPrmInit(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		2012H : DcVoltage Gain Auto Adjustment														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterDcVoltageAutoAdj(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteDcVoltageAutoAdj(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		3000H : 7SegLed And Push Button Check Test													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterSevenSegLedChkTest(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteSevenSegLedChkTest(FUNEXE *FunExe, void *Axis);
void     FnLeaveSevenSegLedChkTest(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		3001H : Adjust Speed Ref.A/D Gain															*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAdjustVrefGain(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAdjustVrefGain(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		3002H : Adjust Torque Positive Ref.A/D Gain													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAdjustTrefPGain(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAdjustTrefPGain(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		3003H : Adjust Torque Negative Ref.A/D Gain													*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAdjustTrefNGain(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAdjustTrefNGain(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/
/*		3004H : Analog Monitor Output Volt Test														*/
/*--------------------------------------------------------------------------------------------------*/
PRM_RSLT FnEnterAnlgMonOutputVoltTest(FUNEXE *FunExe, void *Axis);
PRM_RSLT FnExecuteAnlgMonOutputVoltTest(FUNEXE *FunExe, void *Axis);
void     FnLeaveAnlgMonOutputVoltTest(FUNEXE *FunExe, void *Axis);
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*																									*/
/*		補助機能管理テーブルExtern宣言																*/
/*																									*/
/****************************************************************************************************/
extern  const FUNTBL FnFunTbl[];				/* 補助機能管理テーブル								*/
extern  const UINT FnFunTblEntNum;				/* 補助機能管理テーブル登録個数						*/





#endif /* _FUN_LIST_DEF_ */

/***************************************** end of file **********************************************/
