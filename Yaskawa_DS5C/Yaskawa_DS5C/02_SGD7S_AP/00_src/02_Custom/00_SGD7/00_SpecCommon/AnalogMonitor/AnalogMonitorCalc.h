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

/* �A�i���O���j�^�ϐ��擾�֐�	*/
LONG	GetVarData_AmonMotSpd( void *Axis );				/* 00 :���[�^��]���x							*/
LONG	GetVarData_AmondPosCmdSmp( void *Axis );			/* 05:�ʒu�w�ߑ��x								*/
LONG	GetVarData_AmonFencSpdsmp( void *Axis );			/* 0D:�O���G���R�[�_���x						*/
LONG	GetVarData_TrqRefBeforeComp( void *Axis );			/* 43:�t�B���^�O�g���N�w��						*/
LONG	GetVarData_AmonIdRef( void *Axis );					/* 44:d���d���w��								*/

/*�A�i���O���j�^�̃Q�C���v�Z�֐�	*/
KSGAIN	AmonGcalMotorSpeed( void *Axis );					/* ���[�^���x,���x�w�� (2^24/OvrSpd --> 1V/1000min-1)	*/
KSGAIN	AmonGcalMotPfbkSpd( void *Axis );					/* ���[�^��]���x (dMotPos/ScanB --> 1V/1000min-1)		*/
KSGAIN	AmonGcalPosCmdaSpd( void *Axis );					/* �ʒu�w�ߑ��x (dPosCmda/ScanB --> 1V/1000min-1)		*/
KSGAIN	AmonGcalTorqueRef( void *Axis );					/* �g���N�w�� (2^24/MaxTrq --> 1V/100%)					*/
KSGAIN	AmonGcalTrqRef15K( void *Axis );					/* �g���N�w�� (15000/MaxTrq --> 1V/100%)				*/
KSGAIN	AmonGcalPulseError( void *Axis );					/* �ʒu�΍� (0.05V/1�w�ߒP��)							*/
KSGAIN	AmonGcalPulseErr01( void *Axis );					/* ���[�^���׊Ԉʒu�΍� (0.01V/1�w�ߒP��)				*/
KSGAIN	AmonGcalBitSignal( void *Axis );					/* �r�b�g�M�� (ON:5V, OFF:0V)							*/
KSGAIN	AmonGcalActGain( void *Axis );						/* �L���Q�C�� (��1:1V, ��2:2V, ��3:3V, ��4:4V)			*/


void	PcalAnalogMonitorGain( AXIS_HANDLE *Axis, LONG chx );	/* �A�i���O���j�^�o�̓Q�C���v�Z			*/
void	PcalAnalogMonitorOffset( AXIS_HANDLE *Axis, LONG chx );	/* �A�i���O���j�^�o�̓I�t�Z�b�g�v�Z		*/
LONG	LpxSearchAmonDefTable( LONG SelNo );					/* �A�i���O���j�^��`�e�[�u������		*/
void	LpxGcalPrmVariable( AXIS_HANDLE *Axis, AMONOUT *AmonOutx, LONG Hgain );	/* Prm�w��ϐ�			*/


void	BpxOutputAnalogMonitor( AXIS_HANDLE *Axis );		/* �A�i���O���j�^�o�͏���						*/

//void	FnOutputAnalogMonitor( AXIS_HANDLE *Axis, LONG Value, LONG Ch_No );		/* �A�i���O���j�^�o�͏���(���W�X�^�A�N�Z�X) *//* <S0E7> */
void	FnOutputAnalogMonitor( AXIS_HANDLE *Axis, SHORT Value, LONG Ch_No );	/* �A�i���O���j�^�o�͏���(���W�X�^�A�N�Z�X) *//* <S0FD> */

#endif //__ANALOG_MONITOR_CALC_H

