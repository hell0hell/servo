/****************************************************************************
Description	: Function list of Operation Registers
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
#ifndef __OPE_REG_FUNC_H
#define __OPE_REG_FUNC_H

#include "PrmAccessIf.h"
#if 0 /* 2012.08.18 Y.Oka Make Mercury Base Software */
#include "TrigIo.h"
#endif /* 2012.08.18 Y.Oka Make Mercury Base Software */

extern const PRMTBL OpeRegTable[];
extern const ULONG OpeRegTableEntNum;

extern const PRMTBL TrcRegTable[];
extern const ULONG TrcRegTableEntNum;


PRM_RSLT fnCalcOprationMode1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcOprationCmd1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcOperationFinish(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcAccessLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcOprationMode2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcOprationCmd2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnCalcPrmAccessLock(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnGetPdetStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnGetScaleZsetStatus(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT FnOnePrmTuneSpatMode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnOnePrmTuneSpatLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnOnePrmTuneSpatFFLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT FnEasyFftTrqAmplitude(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcMotorParameterAddress(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcMotorParameterSize(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnGetMotorDownloadState(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnGetPrmDownloadState(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S00C> */

PRM_RSLT FnAdatFilType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnAvibTuneAvibMode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT FnAvibTuneAvibFreq(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnAvibTuneAvibGain(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnMfcTuneMdlVibFreq(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT FnMfcTuneMdlVibFreq2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnMfcTuneMdlVibFil2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnCalcRippleCmpInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S14B> */
PRM_RSLT fnCalcRippleCompGain(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);			/* <S14B> */
PRM_RSLT fnCalcRippleCompCycle1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompAmplitude1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S14B> */
PRM_RSLT fnCalcRippleCompPhase1(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompCycle2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompAmplitude2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S14B> */
PRM_RSLT fnCalcRippleCompPhase2(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompCycle3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompAmplitude3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S14B> */
PRM_RSLT fnCalcRippleCompPhase3(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompCycle4(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompAmplitude4(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S14B> */
PRM_RSLT fnCalcRippleCompPhase4(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompCycle5(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompAmplitude5(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S14B> */
PRM_RSLT fnCalcRippleCompPhase5(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompCycle6(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */
PRM_RSLT fnCalcRippleCompAmplitude6(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S14B> */
PRM_RSLT fnCalcRippleCompPhase6(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);		/* <S14B> */

PRM_RSLT FnSetTLLvlGainLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT FnSetTLLvlJratLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT FnSetCurDetectGainAdjU(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);	/* <S0CB> */
PRM_RSLT FnSetCurDetectGainAdjV(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);	/* <S0CB> */
PRM_RSLT FnSetDcVoltx(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);				/* <S0CB> */
PRM_RSLT FnInputOHAdMon1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);			/* <S0CB> *//* <S0E1> */
PRM_RSLT FnInputOHAdMon2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);			/* <S0E1> */

//PRM_RSLT FnPwmAdjMonU(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);			/* <S0E7> *//* <S0FD> */
//PRM_RSLT FnPwmAdjMonV(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue);			/* <S0E7 *//* <S0FD> */

PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceMode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceCmd(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTraceRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTriggerIOStsMon(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnCalcTriggerIOControl(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnCalcTblDriveRegisters(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnCalcMotorParameter(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnAceessEngneeringInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnDirectEepromAccessIF(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnDirectEepromAccessCNV(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnDirectEepromAccessMB(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnDirectEepromAccessAMP(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnTestMode(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnRamMonitor(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT fnCalcDataIoTraceSel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S0C9> */
PRM_RSLT fnCalcTraceDataSel(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);	/* <S0C9> */

/* <S061> Start */
PRM_RSLT fnAlmTrcRemainingNum(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnAlmTrcWritingStat(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnAlmTrcDataSize(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT fnAlmTrcStopReq(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S061> End */

#endif //__OPE_REG_FUNC_H


