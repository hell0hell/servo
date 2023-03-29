/****************************************************************************
Description	: Function list of Un Monitor Registers
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.5.22     created

****************************************************************************/
#ifndef __UN_MON_FUNC_H
#define __UN_MON_FUNC_H

#include "PrmAccessIf.h"


extern const PRMTBL UnRegTable[];
extern const ULONG UnRegTableEntNum;

PRM_RSLT UnReadMotorSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadVelocityRef(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadTorqueRef(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMotAngle(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMotPhase(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadDigitalOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadCmdSpeedCounter(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadIsqrSum(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadRegenePower(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadDBPower(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMotorEncoderPosition(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadFullcEncoderPosition(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadTimeStamp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadPoleSensorInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadPfbkCntr(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);				/* <S056> *///<S0C7>
PRM_RSLT UnReadActGain(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadSafetySignals(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMotorRatedSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S11C> */
PRM_RSLT UnReadMotorMaxSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S11C> */
PRM_RSLT UnAmpInstEnvMon( PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue );		/* <S03A> */
PRM_RSLT UnMotInstEnvMon( PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue );		/* <S03A> */
PRM_RSLT UnReadDcVolt(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);				/* <S056> */
PRM_RSLT UnLifeTimeMon( PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue );			/* <S03A> */
PRM_RSLT UnReadRelayCnt( PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue );			/* <S056> *///<S0C7>
PRM_RSLT UnReadDBRelayCnt( PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue );			/* <S056> *///<S0C7>
PRM_RSLT UnReadDigitalInputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT UnReadDigitalAllInputSignals1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S1C2> */
PRM_RSLT UnReadDigitalAllInputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S1C2> */
PRM_RSLT UnReadDigitalAllOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S1C2> */

PRM_RSLT UnReadMpfMaxPositiveMoveDist(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMpfMaxNegativeMoveDist(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMpfDetectingTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMpfPhaseOffset(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadLinearScalePitch(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S056> *///<S0C7>
PRM_RSLT UnReadLinearScalePitchExp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S056> *///<S0C7>
PRM_RSLT UnReadMpfStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadSpeedFeedForward(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadEncoderErrorCount(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadRvibFrequency(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInterSignalX(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S21D> Start */
PRM_RSLT UnReadInterSignalX1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInterSignalX2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInterSignalX3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInterSignalX4(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S21D> End */
PRM_RSLT UnReadInternalInputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S21D> Start */
PRM_RSLT UnReadInternalInputSignals1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInternalInputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInternalInputSignals3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInternalInputSignals4(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
//PRM_RSLT UnReadInternalInputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S1A0> */
/* <S21D> End */
PRM_RSLT UnReadInternalOutputSignals(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S21D> Start */
PRM_RSLT UnReadInternalOutputSignals1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInternalOutputSignals2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInternalOutputSignals3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S21D> End */
PRM_RSLT UnReadDCVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadIqRate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMotorLoad2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadRegeneLoad2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadDbPower2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMaxMotorLoad10s(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadTrefAdMon(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S056> *///<S0C7>
PRM_RSLT UnReadVrefAdMon(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S056> *///<S0C7>
PRM_RSLT UnReadDPulseHwCntMon(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S056> *///<S0C7>
PRM_RSLT UnReadIdRefRate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMemobusErrorInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S056> *///<S0C7>
PRM_RSLT UnReadMotroEncoderTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadUsrPrmCrcValue(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadCPULoad(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMaxCPULoad(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadScanBProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadScanBMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadScanCProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadScanCMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMIntADProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMIntADMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMScanAProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMScanAMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMRoundProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMRoundMaxProcessTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlarmHistory(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT uncal_VrefAdjData(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S00C> */
PRM_RSLT uncal_TrefAdjData(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S00C> */
PRM_RSLT uncal_AmonZadjCh1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT uncal_AmonZadjCh2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT uncal_AmonGadjCh1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT uncal_AmonGadjCh2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT uncal_CurZadjUphase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT uncal_CurZadjVphase(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);
PRM_RSLT UnReadSvStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);				/* <S056> *///<S0C7>
PRM_RSLT UnReadMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);				/* <S056> *///<S0C7>
PRM_RSLT UnReadMotorWattage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S056> *///<S0C7>
PRM_RSLT UnReadEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S056> *///<S0C7>
PRM_RSLT UnReadYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);				/* <S056> *///<S0C7>
PRM_RSLT UnReadFirmVerBase(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S056> *///<S0C7>
PRM_RSLT UnReadEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S056> *///<S0C7>
PRM_RSLT UnReadSvStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);				/* <S056> *///<S0C7>
PRM_RSLT UnReadServoControlMode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlarmTimeStamp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadSamplingTimeUnit(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMaxTorqueConvertValue(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMaxSpeedConvertValue(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMaxSpeedExpConvertValue(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadInternalServoMode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadHSWInputsStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadFBboardID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadMBboardID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadConverterCurrentAD(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadConverterSerialInputs(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT UnReadMbTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadCnvTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadIfTemperature(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* (R)BOOT Version Check---> */
PRM_RSLT UnReadBootsoftVersion(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadBootstrapVersion(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* (R)BOOT Version Check--- */
/* <S056>	Start */
//PRM_RSLT UnReadAlmLtSpdFbkX(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		//<S0C7>	/* <S154> */
/* <S056>	End */
/* <---Alarm Latch Monitor --- *//* <S154> Start */
PRM_RSLT UnReadAlmLtSpdFbk(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtSpdRef(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtTrqRef(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtPosRefSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtPosErrCntr(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInterSig(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInSig(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonOutSig(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInterSig0(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInterSig1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInterSig2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInSig0(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInSig1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInSig2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonInSig3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonOutSig0(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonOutSig1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonOutSig2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtDcVoltx(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtIqRate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMotLoad2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtRegerRate2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtDbrxPower2ms(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMotLoad10s(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtJLRatio(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtSencComErrCnt(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtM2ComErrCnt(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtMonSafetyIOSig(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtAlmCode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAlmLtTmStamp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* ---Alarm Latch Monitor ---> *//* <S154> End */
PRM_RSLT UnReadM3CommPhase(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadM3MainCommandCode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadM3SubCommandCode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT uncal_InternalInputSignal3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* @@ */

PRM_RSLT UnReadDummy(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S020> */

/* <S056>	Start */
PRM_RSLT UnReadApos(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadCpos(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadPerr(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadTpos(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadLpos1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadLpos2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadLpos3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadTspd(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadFbspd(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadCspd(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadTrqLim(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadSpdLim(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S056><S0C7>	End */
 /* <S154> Start */
PRM_RSLT UnReadFanLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadCapaLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadRushLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadDBLifeTime(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadRushSwitchingCount(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadDBSwitchingCount(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT UnReadAvgIntTmrp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
 /* <S154> End */

PRM_RSLT UnMaxSpdOrMaxDiv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue); /* <S225> */

#endif //__UN_MON_FUNC_H

