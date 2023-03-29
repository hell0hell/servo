/****************************************************************************
Description	: Function list of the Analog Monitor <S016>
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
K.Sakamoto   2013.10.03     created

****************************************************************************/
#ifndef __ANALOG_MONITOR_CALC_H
#define __ANALOG_MONITOR_CALC_H

#include "Basedef.h"
#include "MercuryGlobals.h"

/* アナログモニタ変数取得関数	*/
LONG	GetVarData_AmonMotSpd( void *Axis );				/* 00 :モータ回転速度							*/
LONG	GetVarData_AmondPosCmdSmp( void *Axis );			/* 05:位置指令速度								*/
LONG	GetVarData_AmonFencSpdsmp( void *Axis );			/* 0D:外部エンコーダ速度						*/
LONG	GetVarData_TrqRefBeforeComp( void *Axis );			/* 43:フィルタ前トルク指令						*/
LONG	GetVarData_AmonIdRef( void *Axis );					/* 44:d軸電流指令								*/

/*アナログモニタのゲイン計算関数	*/
KSGAIN	AmonGcalMotorSpeed( void *Axis );					/* モータ速度,速度指令 (2^24/OvrSpd --> 1V/1000min-1)	*/
KSGAIN	AmonGcalMotPfbkSpd( void *Axis );					/* モータ回転速度 (dMotPos/ScanB --> 1V/1000min-1)		*/
KSGAIN	AmonGcalPosCmdaSpd( void *Axis );					/* 位置指令速度 (dPosCmda/ScanB --> 1V/1000min-1)		*/
KSGAIN	AmonGcalTorqueRef( void *Axis );					/* トルク指令 (2^24/MaxTrq --> 1V/100%)					*/
KSGAIN	AmonGcalTrqRef15K( void *Axis );					/* トルク指令 (15000/MaxTrq --> 1V/100%)				*/
KSGAIN	AmonGcalPulseError( void *Axis );					/* 位置偏差 (0.05V/1指令単位)							*/
KSGAIN	AmonGcalPulseErr01( void *Axis );					/* モータ負荷間位置偏差 (0.01V/1指令単位)				*/
KSGAIN	AmonGcalBitSignal( void *Axis );					/* ビット信号 (ON:5V, OFF:0V)							*/
KSGAIN	AmonGcalActGain( void *Axis );						/* 有効ゲイン (第1:1V, 第2:2V, 第3:3V, 第4:4V)			*/


void	PcalAnalogMonitorGain( AXIS_HANDLE *Axis, LONG chx );	/* アナログモニタ出力ゲイン計算			*/
void	PcalAnalogMonitorOffset( AXIS_HANDLE *Axis, LONG chx );	/* アナログモニタ出力オフセット計算		*/
LONG	LpxSearchAmonDefTable( LONG SelNo );					/* アナログモニタ定義テーブル検索		*/
void	LpxGcalPrmVariable( AXIS_HANDLE *Axis, AMONOUT *AmonOutx, LONG Hgain );	/* Prm指定変数			*/


void	BpxOutputAnalogMonitor( AXIS_HANDLE *Axis );		/* アナログモニタ出力処理						*/

//void	FnOutputAnalogMonitor( AXIS_HANDLE *Axis, LONG Value, LONG Ch_No );		/* アナログモニタ出力処理(レジスタアクセス) *//* <S0E7> */
void	FnOutputAnalogMonitor( AXIS_HANDLE *Axis, SHORT Value, LONG Ch_No );	/* アナログモニタ出力処理(レジスタアクセス) *//* <S0FD> */

#endif //__ANALOG_MONITOR_CALC_H

