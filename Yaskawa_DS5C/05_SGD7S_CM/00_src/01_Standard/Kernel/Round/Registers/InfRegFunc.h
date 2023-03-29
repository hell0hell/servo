/****************************************************************************
Description	: Function list of Information Registers
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
#ifndef __INF_REG_FUNC_H 
#define __INF_REG_FUNC_H

#include "PrmAccessIf.h"


extern const PRMTBL InfoRegTable[];
extern const ULONG InfoRegTableEntNum;


PRM_RSLT EInfReadMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadEncoderTypeAndBits(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorWattage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorRatedSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorMaxSpeed(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorSpeedExp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorRatedTorque(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorMaxTorque(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMotorTorqueExp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadEncoderResolution(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMultiTurnLimit(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMaxTorqueRef(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EinfReadFullcScaleType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EinfReadAbsScaleZeroSetting(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadMaxSpeedRef(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadLinearScalePitch(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadLinearScalePitchExp(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EinfReadModuleIDProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadModuleIDSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadModuleIDDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadMotorProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadMotorSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadMotorManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EinfReadEncoderProductName(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadEncoderSerialNumber(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S0D8> */
PRM_RSLT EinfReadEncoderManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EinfReadNumberOfAxes(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);/* <S171> */
/* <S0C9>	Start */
PRM_RSLT EInfReadServoID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadServoPrmVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFirmVersion(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadServoVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT EInfReadServoFirmwareBootVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadCmdCardID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadCmdCardFirmwareVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadCmdCardYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetyID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetyFirmwareVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetyYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbFirmwareVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbYspecVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbMotorID(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbMotorWattage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbMotorVoltage(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbEncoderTypeAndBits(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbEncoderBits(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbAbsEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbEncoderVer(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);

PRM_RSLT EInfReadCmdType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadCmdSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadCmdManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetyType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetySerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetyManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbMotorType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbMotorSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbMotorManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbEncoderType(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbEncoderSerialNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadFbEncoderManufactureDate(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadInputSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadOutputSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadSafetyIOSignalInfo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadServoBtoNo(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
PRM_RSLT EInfReadServoSpecialSpec(PRM_ACCCMD Cmd, UINT ArrayIdx, void *Axis, LONG *pValue);
/* <S0C9>	End */


#endif //__INF_REG_FUNC_H 

