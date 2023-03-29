/****************************************************************************
Description	: Motor Less Test Mode
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2013 All Rights Reserved

Project       : Mercury

Functions	  : none

----------------------------------------------------------------------------
Changes		:
Name		Date			Description
------------------------------------------------------------------------
K.Sakamoto	2013.11.11		created <S00X>

****************************************************************************/
#ifndef MOTORLESSTEST_H__
#define MOTORLESSTEST_H__

/*==========================================================================*/
/* I N C L U D E															*/
/*==========================================================================*/
#include "Basedef.h"
#include "PnPrmStruct.h"
#include "EncData.h"
#include "MercuryGlobals.h"
#include "BaseControls.h"

/*==========================================================================*/
/* D E F I N E																*/
/*==========================================================================*/

/*==========================================================================*/
/* S T R U C T																*/
/*==========================================================================*/

/*==========================================================================*/
/* E X T E R N   V A R I A B L E											*/
/*==========================================================================*/

/*==========================================================================*/
/* P R O T O T Y P E														*/
/*==========================================================================*/
/* --- ���[�^���X�e�X�g --- */
/* ���[�^���X�e�X�g���[�h���J���f�����Z	*/
void ApxMotorLessTestModeMechaModel( LONG TrqRef, BOOL BeComplete, MOTLESSTST *MotorLessTest );
/* ���[�^���X�e�X�g���[�h�p�����[�^�v�Z	*/
void PcalMotorLessTestMode( MENCV *FencV, BPRMDAT *Bprm, PRMDATA *Prm, MOTLESSMDL *MotorLessMdl );
/* ���[�^���X�e�X�g���[�h�ϐ�������	*/
void ApxInitMotorLessTestMode( AXIS_HANDLE *AxisRsc );
/* ���[�^���X�e�X�g���[�h�d���f�[�^�ݒ�	*/
void ApiSetCurDatasMotorLessTestMode( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut );
/* ���[�^���X�e�X�g���[�h���x�E�ʒu�f�[�^�ݒ�	*/
void ApiSetMechaDatasMotorLessTestMode( BPRMDAT *Bprm, MOTLESSTST *MotorLessTest );
/* ���[�^���X�e�X�g���[�h�p���[�^�p�����[�^�v�Z�F�G���R�[�_���ڑ���	*/
void ApiReadMotPrmMotorLessTestMode( MENCV *MencV, PRMDATA *Prm );
/* ���[�^���X�e�X�g���[�h�p���[�^�p�����[�^�v�Z�F�t���N�G���R�[�_���ڑ���	*/
void ApiReadFencPrmMotorLessTestMode( MENCPRM *MencP );

/* --- ���[�^���X�e�X�g���[�h --- */
/* ���[�^���X�e�X�g���[�h�t���O�ݒ�	*/
void MltSetMotorLessTestMode( AXIS_HANDLE *AxisRscI, INT AxisRscNum );

#if 0
/* --- ���[�^���X�e�X�g���[�hIF��`�A�h���X�擾(�b��) --- */
/* ���[�^���X�e�X�g���[�h�X�e�[�^�X�擾		*/
BOOL MltGetMotorLessTestMode( void );
/* ���[�^���X�e�X�g���[�hIF��`�A�h���X�擾	*/
MOTLESSTST *MltGetMotorLessTestHandle( void );
#endif

#endif // MOTORLESSTEST_H__
/*-------end of file----------------------------------------------------------------------------------------*/
