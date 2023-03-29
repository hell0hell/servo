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
/* --- モータレステスト --- */
/* モータレステストモードメカモデル演算	*/
void ApxMotorLessTestModeMechaModel( LONG TrqRef, BOOL BeComplete, MOTLESSTST *MotorLessTest );
/* モータレステストモードパラメータ計算	*/
void PcalMotorLessTestMode( MENCV *FencV, BPRMDAT *Bprm, PRMDATA *Prm, MOTLESSMDL *MotorLessMdl );
/* モータレステストモード変数初期化	*/
void ApxInitMotorLessTestMode( AXIS_HANDLE *AxisRsc );
/* モータレステストモード電流データ設定	*/
void ApiSetCurDatasMotorLessTestMode( BASE_LOOPCTRLS *BaseLoops, CTRL_LOOP_OUT *CtrlLoopOut );
/* モータレステストモード速度・位置データ設定	*/
void ApiSetMechaDatasMotorLessTestMode( BPRMDAT *Bprm, MOTLESSTST *MotorLessTest );
/* モータレステストモード用モータパラメータ計算：エンコーダ未接続時	*/
void ApiReadMotPrmMotorLessTestMode( MENCV *MencV, PRMDATA *Prm );
/* モータレステストモード用モータパラメータ計算：フルクエンコーダ未接続時	*/
void ApiReadFencPrmMotorLessTestMode( MENCPRM *MencP );

/* --- モータレステストモード --- */
/* モータレステストモードフラグ設定	*/
void MltSetMotorLessTestMode( AXIS_HANDLE *AxisRscI, INT AxisRscNum );

#if 0
/* --- モータレステストモードIF定義アドレス取得(暫定) --- */
/* モータレステストモードステータス取得		*/
BOOL MltGetMotorLessTestMode( void );
/* モータレステストモードIF定義アドレス取得	*/
MOTLESSTST *MltGetMotorLessTestHandle( void );
#endif

#endif // MOTORLESSTEST_H__
/*-------end of file----------------------------------------------------------------------------------------*/
