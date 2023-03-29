/****************************************************************************
Description	: Advanced Auto Tuning
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
****************************************************************************/
#include "MercuryGlobals.h"
#include "BaseControlMain.h"
#include "TuneLib.h"
#include "Mlib.h"
#include "KLib.h"
#include "AdvancedAutoTuning.h"
#include "PrmAccessIf.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"
#include "ExCtrlPrmCalc.h"
#include "BaseControlPrmCalc.h"
#include "PnPrmListTbl.h"
#include "ModelControl.h"
#include "PosManager.h"
#include "CheckMotSts.h"
#include "FnJatOffLine.h"
#include "GainChange.h"


/****************************************************************************************************/
/*																									*/
/*		Function Prototype Definition																*/
/*																									*/
/****************************************************************************************************/
static PRM_RSLT	AdatExecuteAutoTuning( ADATHNDL *AdatHndl, FUNEXE *FunExe );
static PRM_RSLT AdatInitial( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP);

static void	 	AdatSetTuningModeMax( ADATV *AdatV, LONG XfbPulse1, LONG AxisMotType );
static PRM_RSLT AdatModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP );
static LONG		AdatPcalMinDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor );
LONG 	AdatPcalDefautDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor ); /* <S145> */

static PRM_RSLT	AdatRefInModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP );
static LONG		AdatRefInExeModeExeCheck( ADATP *AdatP );
static void 	AdatScanCSetReq( ADATV *AdatV );
static void		AdatScanCRequest( USHORT Request, ADATV *AdatV );

static PRM_RSLT	AdatJratSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status);
static PRM_RSLT	AdatNormVibDetect( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT AdatOptGainSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatNotchFilSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatKpGainSet(ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatOptKpSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatMfcAdjustment( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status );
static PRM_RSLT	AdatFinishOpe( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT err_status );

static void		AdatGainDownforVibration( ADATV *AdatV, ADATP *AdatP, USHORT ser_kv, UCHAR down_ratio);
static SHORT	AdatNotchFilReset( ADATV *AdatV, ADATP *AdatP );
static SHORT 	AdatAresCtrlReset( ADATV *AdatV, ADATP *AdatP );
static SHORT 	TuneAdatSetSpdCtrlGain( LONG mode, ADATP *AdatP);
static SHORT	TuneAdatSetPosCtrlGain( LONG mode, ADATP *AdatP);
static SHORT	TuneAdatSetMdlCtrlGain( LONG mode, ADATP *AdatP );
static void		MfcPrmReSet( ADATV *AdatV, ADATP *AdatP );
static void		Mfc2MassReset( ADATV *AdatV, ADATP *AdatP );
static USHORT	SetMfc2MassSequence( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, ULONG ADAT_MFC );

static void		AdatUndoRamGain( ADATV *AdatV, ADATP *AdatP );
static void		AdatUndoRamGainforJrat( ADATV *AdatV, ADATP *AdatP );
static LONG		AdatUpdateGainInEeprom( ADATREG *AdatReg, ADATP *AdatP );
static SHORT	AdatPrgJogDrive( ADATV	*AdatV, ADATP *AdatP );
static void		AdatPJogInit( ADATV	*AdatV, ADATP *AdatP );
static void		AdatPJogDrvSet( ADATV *AdatV, ADATP *AdatP, ULONG pattern );

static SHORT	LpxAdatVibLvlSet( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatCheckKv( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatSearchKv( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatKpGainSet( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatSearchKp( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatAdjustMfcGain( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatCheckMfcGain( ADATV *AdatV, ADATP *AdatP );
static SHORT	LpxAdatRefInTypeDrvStatus( ADATHNDL *AdatHndl, ULONG DrvPattern, BOOL PrefInFlag );


/****************************************************************************/
/*		Start AAT Initialize												*/
/****************************************************************************/
void AdatSetStrPtr( void *Axis )
{
	AXIS_HANDLE *pAxis;
	ADATHNDL *AdatHndl;

	pAxis = ((AXIS_HANDLE*)Axis);
	AdatHndl = pAxis->AdatHndl;

	/* Initialize */
	AdatHndl->AdatP.FnCmnCtrl    = pAxis->FnCmnCtrl;
	AdatHndl->AdatP.UniPrms      = pAxis->UniPrms;
	AdatHndl->AdatP.MencV        = pAxis->MencV;
	AdatHndl->AdatP.BaseCtrlData = pAxis->BaseControls;
	AdatHndl->AdatP.RegManager   = pAxis->RegManager;
	AdatHndl->AdatP.SeqMotSts	 = pAxis->SeqMotSts;
	AdatHndl->AdatP.SeqCtrlOut   = pAxis->SeqCtrlOut;
	AdatHndl->AdatP.AlmManager   = pAxis->AlmManager;
	AdatHndl->AdatP.SettlingTime = &(pAxis->BaseControls->SettlingTime);
	AdatHndl->AdatP.ANotch       = pAxis->ANotch;
	AdatHndl->AdatP.RemVibFreq   = pAxis->RemVibFreq;
	AdatHndl->AdatP.FftAna       = pAxis->FftAna;
	AdatHndl->AdatP.SvAsicRegs   = pAxis->SvAsicRegs;
	AdatHndl->AdatP.MotSpdMafil  = pAxis->MotSpdMafil;
	AdatHndl->AdatP.DetVib       = pAxis->DetVib;
	AdatHndl->AdatP.SvControlIf  = pAxis->SvControlIf;
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode Begin
****************************************************************************/
PRM_RSLT FnEnterAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATREG	 *AdatReg;
	ADATV	 *AdatV;
	ADATP	 *AdatP;
	PRM_RSLT errRes;

	AdatReg	  = &Axis->AdatHndl->AdatReg;
	AdatV	  = &Axis->AdatHndl->AdatV;
	AdatP	  = &Axis->AdatHndl->AdatP;

	/* �v���O����JOG�^�] �ϐ��Z�b�g */
	MlibCopyLongMemory( &AdatP->BaseCtrlData->PJogHdl.PJogV,
						&Axis->AdatHndl->OrgPJogV,						/* @@ check */
						sizeof(AdatP->BaseCtrlData->PJogHdl.PJogV)>>2 );

	/* �v���O����JOG�^�] �w�ߍ쐬�ϐ����Z�b�g */
	PrgJogReset( &AdatP->BaseCtrlData->PJogHdl );

	/* �A�h�o���X�g�I�[�g�`���[�j���O�֘A���N���A	*/
	MlibResetLongMemory( AdatV, sizeof(Axis->AdatHndl->AdatV)>>2 );
	AdatSetTuningModeMax( AdatV, AdatP->UniPrms->Bprm->XfbPulse1, AdatP->MencV->AxisMotType );
	errRes = AdatModeCheck( AdatReg, AdatV, AdatP );

	if( errRes == PRM_RSLT_SUCCESS )
	{
		/* �T�[�{�I�����̏ꍇ�͓��炸�ɏI��	*/
		if(AdatP->FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
		{
			return PRM_RSLT_CONDITION_ERR;
		}
		
		/* ���[�^���X�e�X�g���[�h�̏ꍇ�͓��炸�ɏI�� *//* <S1B7> */
		if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
		{
			return(PRM_RSLT_CONDITION_ERR);
		}

		AdatP->FnCmnCtrl->FnSvControl = TRUE;	/* Fn�T�[�{�^�]���l��	*/
		FunExe->HoldFnMode = TRUE;				/* Fn���[�h�����I���Ȃ� */
	}
	AdatV->f.RefInType = FALSE;
	AdatReg->AdatState = ADAT_STS_INIT;			/* �X�e�[�^�X������		*/

	return(errRes);
}

/****************************************************************************
	Fn201 Tuning mode Check
****************************************************************************/
static void	AdatSetTuningModeMax( ADATV	*AdatV, LONG XfbPulse1, LONG AxisMotType )
{
	AdatV->TunModeMax = ADAT_LEVEL_MAX;						/* �����l�͍ő�ݒ�l	*/

		if( AxisMotType == MOTTYPE_ROTARY )					/* ��]�`���[�^			*/
		{
			//�t���N���Ή�
			//if( Kprm.f.FencUse == FALSE )					/* �Z�~�N���[�Y����		*/
			{
				if( XfbPulse1 <= 0x2000 )					/* 13bit[p/rev]�ȉ�		*/
				{
					AdatV->TunModeMax = ADAT_LEVEL_MIN;		/* �������[�h�F�W���̂�	*/
				}
			}
		}
		return;
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode Condition Check
****************************************************************************/
static PRM_RSLT AdatModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP )
{
	BASE_CTRL_HNDL *BaseCtrlData;
	PRM_RSLT rc;

	BaseCtrlData = AdatP->BaseCtrlData;
	rc = PRM_RSLT_SUCCESS;

	/* �T�[�{�I�����A�g���N����A�����Q�C���ؑ֗L�����́A�f�[�^�ُ�(REGIF_DAMTHERR)�Ƃ���B		*/
	/* �΍����X����A�\������A�m�[�}���I�[�g�`���[�j���O�L�������A�f�[�^�ُ�ł��邪�A���Ή�	*/
	if( ((BaseCtrlData->BeComplete == TRUE) && (BaseCtrlData->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ))
	  || (GselGet_AtGselStatus( &BaseCtrlData->GainChange ) == TRUE))
	{
		rc = PRM_RSLT_CONDITION_ERR;
	}

	/* �`���[�j���O���[�h�������`�F�b�N	*/
	if( AdatV->TunModeMax < AdatReg->AdatLevel )
	{
		rc = PRM_RSLT_CONDITION_ERR;
	}

	AdatV->ModeMin    = ADAT_MODE_MIN;
	AdatV->ModeMax    = ADAT_MODE_MAX;
	AdatV->ModeSetMin = ADAT_SCROLL_MIN;
	AdatV->ModeSetMax = ADAT_SCROLL_MAX;

	return( rc );
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode Execution
****************************************************************************/
PRM_RSLT	FnExecuteAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT rc;
	rc = AdatExecuteAutoTuning( Axis->AdatHndl, FunExe );
	return rc;
}

/****************************************************************************
	Fn201 Advanced Auto Tuning mode End
****************************************************************************/
void FnLeaveAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATV			*AdatV;
	ADATP			*AdatP;
	REG_ACC_T		AccessPrm;
	TUNELESS_CFGPRM	TuneLessCfgPrm;

	AdatV = &Axis->AdatHndl->AdatV;
	AdatP = &Axis->AdatHndl->AdatP;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* ���E���h���䒆��SEN�M��ON�v����I/O�̏�ԂŐݒ肷�� */
	AdatP->FnCmnCtrl->FnSensOnReq = (((AdatP->SeqCtrlOut->RspSeqBits & SENS_ON_COMP_BITMSK)) == TRUE)
																		? TRUE : FALSE;

	AdatP->FnCmnCtrl->State.TuningRun      = FALSE;
	AdatP->FnCmnCtrl->State.TuningPctrlRun = FALSE;

	/* �^�]���J������(ScanC IF)	*/
	AdatP->FnCmnCtrl->FnSvControl = FALSE;

	/* �C�i�[�V������ł�Kv,JL�������ɖ߂��̂�AdatUndoRamGain()�̑O�ł��邱�Ƃ��K�v	*/
	CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );

	MlibCopyLongMemory( &Axis->AdatHndl->OrgPJogV,						/* @@  check */
						&AdatP->BaseCtrlData->PJogHdl.PJogV,
						sizeof( AdatP->BaseCtrlData->PJogHdl.PJogV )>>2 );

	AdatUndoRamGain( AdatV, AdatP );

	DetVibResetMaxMinSpeedErrorFn( AdatP->DetVib );
	DetVibObsCalculateInitPrmAAT( &(AdatP->DetVib->DetVibObs),
	                              AdatP->BaseCtrlData->TuneLessCtrl.conf.TuningLessUse,
	                              AdatP->UniPrms->Bprm->SvCycleNs );

	/* �������X�@�\�X�C�b�`�Čv�Z */
	TuneLessCfgPrm.gnmode	= (AdatP->UniPrms->Prm)->gnmode;
	TuneLessCfgPrm.mfctype	= (AdatP->UniPrms->Prm)->mfctype;
	TuneLessCfgPrm.avibsw	= (AdatP->UniPrms->Prm)->avibsw;
	TuneLessCfgPrm.DatLevel	= (AdatP->UniPrms->Prm)->DatLevel;
	TuneLessCfgPrm.tfuncsw 	= (AdatP->UniPrms->Prm)->tfuncsw;
	TuneLessCfgPrm.flg_wf 	= (AdatP->UniPrms->Prm)->MencP.flg_wf;
	TuneLessCalculateInitPrm( AdatP->BaseCtrlData,
	                          AdatP->UniPrms->Bprm,
	                          AdatP->MencV,
							  AdatP->SvAsicRegs,
						      &TuneLessCfgPrm );

#if 0 /* 2012.08.31 Y.Oka �����[�^���x���o�ړ����ό��󖢑Ή��� */
	/* ���[�^���x���o�p�ړ����ώ��萔��߂�	*/
	if( AdatP->BaseCtrlData->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{
		BpiChangeSpdDetMaFil( AdatP->MotSpdMafil,
							  0,
							  (USHORT*)&(AdatP->SvAsicRegs->AsicMicroReg->MREG_SPDMANUMBITI) );
	}
	else
	{
		BpiChangeSpdDetMaFil( AdatP->MotSpdMafil,
							  -1,
							  (USHORT*)&(AdatP->SvAsicRegs->AsicMicroReg->MREG_SPDMANUMBITI) );
	}
#endif

	/* �I������	*/
	AdatP->FnCmnCtrl->JogSpeed    = 0;
	AdatP->FnCmnCtrl->FnSvonReq   = FALSE;
	AdatP->FnCmnCtrl->FnCtrlMcmd  = CTRLMODE_NOCMD;
	AdatP->FnCmnCtrl->FnSensOnReq = FALSE;
	AdatP->FnCmnCtrl->FnSvControl = FALSE;

}

/****************************************************************************
	Fn202 Reference Input Type Advanced Auto Tuning mode Begin
****************************************************************************/
PRM_RSLT	FnEnterRefInAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATREG		*AdatReg;
	ADATV		*AdatV;
	ADATP		*AdatP;
	PRM_RSLT 	errRes;

	AdatReg   = &Axis->AdatHndl->AdatReg;
	AdatV     = &Axis->AdatHndl->AdatV;
	AdatP     = &Axis->AdatHndl->AdatP;
	errRes    = PRM_RSLT_SUCCESS;

	/* �A�h�o���X�g�I�[�g�`���[�j���O�֘A���N���A */
	MlibResetLongMemory( AdatV, sizeof(Axis->AdatHndl->AdatV)>>2 );

	/* �������[�h�ő�l�ݒ� */
	AdatSetTuningModeMax( AdatV, AdatP->UniPrms->Bprm->XfbPulse1, AdatP->MencV->AxisMotType );

	AdatV->f.RefInType = TRUE;
	errRes = AdatRefInModeCheck(AdatReg, AdatV, AdatP);

	/* ���[�^���X�e�X�g���[�h�̏ꍇ�͓��炸�ɏI�� *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return(PRM_RSLT_CONDITION_ERR);
	}

	AdatReg->AdatState = ADAT_STS_INIT;	/* �X�e�[�^�X������		*/
	FunExe->HoldFnMode = TRUE;			/* Fn���[�h�����I���Ȃ� */

	return(errRes);
}


/****************************************************************************
	Fn202 Advanced Auto Tuning mode Condition Check
****************************************************************************/
static PRM_RSLT	AdatRefInModeCheck( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP )
{
	BASE_CTRL_HNDL *BaseCtrlData;
	PRM_RSLT errRes;

	BaseCtrlData = AdatP->BaseCtrlData;
	errRes = PRM_RSLT_SUCCESS;

	/* �g���N����A���x����ł��T�[�{�I�����A�܂��͎����Q�C���ؑ֗L�����́A�f�[�^�ُ�(REGIF_DAMTHERR)�Ƃ���B*/
	/* �΍����X����A�\������A�m�[�}���I�[�g�`���[�j���O�L�������A�f�[�^�ُ�ł��邪�A���Ή�	*/
	if( ((BaseCtrlData->BeComplete == TRUE) && (BaseCtrlData->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS))
	  || (GselGet_AtGselStatus( &BaseCtrlData->GainChange ) == TRUE)
 	  || (BaseCtrlData->TuneLessCtrl.conf.TuningLessUse  == TRUE) )
	{
		errRes = PRM_RSLT_CONDITION_ERR;
	}

	/* �`���[�j���O���[�h�������`�F�b�N	*/
	if(AdatV->TunModeMax < AdatReg->AdatLevel)
	{
		errRes = PRM_RSLT_CONDITION_ERR;
	}

	AdatV->ModeMin = ADAT_MODE_MIN;
	AdatV->ModeMax = ADAT_MODE_MAX;
	AdatV->ModeSetMin = ADAT_REFIN_SCROLL_MIN;
	AdatV->ModeSetMax = ADAT_REFIN_SCROLL_MAX;

	return(errRes);
}

/****************************************************************************
	Fn202 Advanced Auto Tuning mode Execution
****************************************************************************/
PRM_RSLT	FnExecuteRefInAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT rc;
	rc = AdatExecuteAutoTuning( Axis->AdatHndl, FunExe );
	return rc;
}


/****************************************************************************
	Fn202 Reference Input Type Advanced Auto Tuning mode End
****************************************************************************/
void FnLeaveRefInAdvAutoTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	ADATV	*AdatV;
	ADATP	*AdatP;
	FUN_CMN_CONTROL	*FnCmnCtrl;

	AdatV	   = &Axis->AdatHndl->AdatV;
	AdatP	   = &Axis->AdatHndl->AdatP;
	FnCmnCtrl  = Axis->FnCmnCtrl;

	FnCmnCtrl->State.TuningRun      = FALSE;
	FnCmnCtrl->State.TuningPctrlRun = FALSE;

	AdatUndoRamGain( AdatV, AdatP );

	/* �U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g */
	DetVibResetMaxMinSpeedError( AdatP->DetVib );

	/* �U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g */
	DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs,
	                         VIBOBSGN, 0, DETVIBHP, (AdatP->UniPrms->Bprm)->SvCycleNs);

	/* �I������	*/
	FunExe->HoldFnMode = FALSE;				/* Fn���[�h�����I���Ȃ� */
}


/****************************************************************************
	Advanced Auto Tuning  Execution (Fn201,Fn202 Common)
****************************************************************************/
static PRM_RSLT	AdatExecuteAutoTuning( ADATHNDL *AdatHndl, FUNEXE *FunExe )
{
	ADATREG	*AdatReg;
	ADATV	*AdatV;
	ADATP	*AdatP;
	REG_ACC_T AccessPrm;

	PRM_RSLT rc;
	ULONG	 data;
	LONG	 UnRvibFrequency;

	AdatReg	  = &AdatHndl->AdatReg;
	AdatV	  = &AdatHndl->AdatV;
	AdatP	  = &AdatHndl->AdatP;

	rc  = PRM_RSLT_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/*------------------------------------------------------------------------------------------*/
	/*	���ʑ��쏈��																			*/
	/*------------------------------------------------------------------------------------------*/
	if( AdatV->f.DrvErrDetect == TRUE )
	{
		if(   (AdatP->AlmManager->AlmCode != 0x00)
			|| (AdatP->SeqCtrlOut->MainPowerOn == FALSE)
			|| (AdatP->SeqCtrlOut->OverTrvlSts == TRUE)
			|| ((AdatV->f.RefInType == TRUE) && (AdatP->FnCmnCtrl->State.TuningRun == TRUE) && (AdatP->SeqCtrlOut->BaseEnableReq == FALSE))
			|| (AdatP->BaseCtrlData->GainChange.ActGain != 1)
			|| (AdatP->SeqCtrlOut->HwbbSts == TRUE) ) /* GAIN1�ȊO �������� HWBB���			*/
		{
			AdatV->ErrState = ADAT_NOTRDYERR;			/* �^�]���������G���[ 					*/
			FunExe->State   = ADAT_SEQ_END;				/* �ŏI�ݒ���	    					*/
		}
		else if( AdatV->ErrState <= ADAT_NG )
		{
			FunExe->State = ADAT_SEQ_END;
		}
	}

	switch ( FunExe->CmdCode )
	{
		case FCMD_SV :
			if( AdatV->f.RefInType == FALSE )
			{
//				if( (AdatP->AlmManager->AlmReady    == TRUE)		/* �A���[������ 		*//* <S085> */
				if( (AdatP->AlmManager->AlmReady    == FALSE)		/* �A���[������ 		*/
				 || (AdatP->SeqCtrlOut->HwbbSts     == TRUE)		/* HWBB����				*/
//				 || (AdatP->SeqCtrlOut->MainPowerOn == TRUE)		/* ���HOFF			*//* <S085> */
				 || (AdatP->SeqCtrlOut->MainPowerOn == FALSE)		/* ���HOFF			*/
				 || (AdatP->SeqCtrlOut->OverTrvlSts == TRUE) )		/* OT����				*/
				{
					;
				}
				else
				{
//					if( AdatP->FnCmnCtrl->FnSvonReq == TRUE )
					if( AdatP->FnCmnCtrl->FnSvonReq != FALSE )		/* <S085> */
					{
						AdatP->FnCmnCtrl->FnSvonReq = FALSE;
						FunExe->State = ADAT_SEQ_INIT;
					}
				}
			}
			break;

		case FCMD_STOP :
			AdatV->ErrState = ADAT_COMERR;
			FunExe->State = ADAT_SEQ_END;
			break;

		case FCMD_UP :
		case FCMD_DOWN :
		default :
			break;
	}

	switch ( FunExe->State )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�������																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_INIT :
			AdatP->FnCmnCtrl->State.TuningRun = FALSE;

			if( (AdatV->f.RefInType == FALSE)	 					/* �����w�ߌ^				*/
			 || (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)/* COIN						*/
			 || (AdatP->SeqCtrlOut->MotStop == TRUE) )				/* ���[�^��~���o 			*/
			{	/* �w�ߒ�~����	*/
				FunExe->State = ADAT_SEQ_START;				/* ���[�^��~��A������������		*/
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�������������																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_START :
			AdatV->f.GainTuneRun = FALSE;

			/* �A�h�o���X�gAT����������	*/
			rc = AdatInitial( AdatReg, AdatV, AdatP );
			TuneFilGainCalbyFilType( AdatP->ANotch, (UCHAR)AdatReg->AdatFilType );
			/*----------------------------------------------------------------------------------*/
			/*	�Q�C���̏�����																	*/
			/*----------------------------------------------------------------------------------*/
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );				/* ���ݒ�Kv�ƃo�����X		*/
			data = ADAT_GRAT_50( AdatP->UniPrms->Prm->loophz );		/* Kv��1/2					*/
			TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)data, pndef_posgn.DataLength );

			/* �`���[�j���O���[�h�ݒ�͒ʐM�ōs���邽�߃C�i�[�V���ݒ�� */
			FunExe->State = ADAT_SEQ_JRATSET;

			//�⏕�@�\���s���̃f�[�^�����݋֎~�ݒ�֐�
			//RpiFunDataWriteDisableSet( FPRM_IF, ON, pfexe );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�C�i�[�V���ݒ���																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_JRATSET :
			AdatV->f.GainTuneRun = TRUE;

			/* �A�h�o���X�gAT�C�i�[�V���ݒ菈�� */
			rc = AdatJratSet( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�ʏ�U�����x�����o�����׃g���N������												*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_NORMVIB:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = TRUE;
			AdatReg->AdatState = ADAT_STS_VIBDET; /* �U�����x��������s��						*/

			/* �A�h�o���X�gAT�ʏ�U�����x�����o����	*/
			rc = AdatNormVibDetect( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�œKKv�ATi�Q�C���T�����															*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_SEARCHOPTKV:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = TRUE;
			AdatReg->AdatState = ADAT_STS_OPTGNADJ; /* ����]���Q�C���T�����s��					*/

			/* �A�h�o���X�gAT�œK�Q�C���T������ */
			rc = AdatOptGainSearch( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�m�b�`�t�B���^�ݒ���																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_NOTCHSET:
			AdatV->f.GainTuneRun = TRUE;
			AdatReg->AdatState = ADAT_STS_VIBDISSET; /* �U���}���ݒ���s��						*/

			/* �A�h�o���X�gAT�m�b�`�t�B���^�ݒ菈��	*/
			rc = AdatNotchFilSet( AdatReg, AdatV, AdatP, FunExe, AdatV->FftStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kp�ݒ���																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_KPSET:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = TRUE;
			AdatReg->AdatState = ADAT_STS_OPTGNADJ; /* ����]���Q�C���T�����s�� 				*/

			/* �A�h�o���X�gAT Kp�ݒ菈�� */
			rc = AdatKpGainSet( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�œKKp�Q�C���T�����																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_SEARCHOPTKP:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = FALSE;
			/* ����]���Q�C���T�����s��	*/
			AdatReg->AdatState = ADAT_STS_OPTGNADJ;
			/* �A�h�o���X�gAT�œKKp�Q�C���T������	*/
			rc = AdatOptKpSearch( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	���f���Ǐ]���䒲�����																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_ADJMFC:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = FALSE;
			/* ����]���Q�C���T�����s�� */
			AdatReg->AdatState = ADAT_STS_OPTGNADJ;
			/* �A�h�o���X�gAT���f���Ǐ]���䒲������	*/
			rc = AdatMfcAdjustment( AdatReg, AdatV, AdatP, FunExe, AdatV->ScanCStat );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�I���\�����																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_ENDDISP:
			FunExe->State = ADAT_SEQ_END;
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�ŏI�ݒ���																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_END:
			/* �A�h�o���X�gAT�I���ݒ菈�� */
			rc = AdatFinishOpe( AdatReg, AdatV, AdatP, FunExe, AdatV->ErrState );
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�I���҂����																		*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_WAIT:
			AdatP->FnCmnCtrl->State.TuningRun = FALSE;
			break;
#if 0
		//��-�X-MD�ł͎g�p���Ȃ�
		/*--------------------------------------------------------------------------------------*/
		/*	�㏈�����																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_RETURN:
			if( AdatV->f.RefInType == TRUE )
			{
				FnLeaveRefInAdvAutoTuning( FunExe, Axis );
			}
			else
			{
				FnLeaveAdvAutoTuning( FunExe, Axis);
			}
			break;

		case ADAT_SEQ_NOOPWAIT :
			if( KlibGetLongTimerMs( FunExe->LongTimer ) > 1000 )
			{
				FunExe->State = ADAT_SEQ_RETURN;
			}
			break;
#endif
		/*--------------------------------------------------------------------------------------*/
		/*	MFC(����+�@�䃂�f��)�ݒ���														*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_SEQ_2MASSSET:
			AdatV->f.GainTuneRun = TRUE;
			AdatV->f.StopDetectMode2 = FALSE;

			AdatReg->AdatState = ADAT_STS_VIBDISSET;		/* MFC(����+�@�䃂�f��)�ݒ���s�� 	*/

			/* MFC(����+�@�䃂�f��)�ݒ�ς݃t���OON 											*/
			/* �U�����o����2����MFC��L���ɂ�����AdatV->f.Used2MassMfc = TRUE					*/
			AdatV->f.Used2MassMfc = TRUE;

			/* Kpm�̃f�t�H���g�͔����U���g���ɂ���BKpm [1/s] = wL / 2 [Hz]	*/
			/* �����C�����U���g����1.0Hz�ȉ���������Kpm=1.0[1/s]			*/
			UnRvibFrequency = RemVibGetRemVibFreq( AdatP->RemVibFreq );

			if( UnRvibFrequency <= VIBMFC_DET_FRQ_MIN )
			{
				data = VIBMFC_DET_FRQ_MIN;
			}
			else
			{
				data = ( UnRvibFrequency >> 1 );
			}
			/* Pn141 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)data, pndef_mdlgn.DataLength );

			/* �U�����g����ݒ�CMFC(����+�@�䃂�f��)��ON�ɂ��� */
			data = UnRvibFrequency;
			/* Pn145 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlantfrq, data, pndef_mdlantfrq.DataLength );
			/* Pn146 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, data, pndef_mdlresfrq.DataLength );
			PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
			data = AdatP->UniPrms->Prm->mdlsw | VIBMFC_PRM_ACTIVE;
			/* Pn140 */
			TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, data, pndef_mdlsw.DataLength );
			FunExe->State = AdatV->NextSeqPtr;				/* ��ԃ|�C���^�����ɖ߂� 			*/

			break;
		default:
			break;
	}
	/*------------------------------------------------------------------------------------------*/
	/*	�v���O����JOG�w�߃V�[�P���X																*/
	/*------------------------------------------------------------------------------------------*/
//	if(FunExe->State != ADAT_SEQ_RETURN)
	{
		if( AdatV->f.RefInType == FALSE )
		{
			AdatV->PrgJogStat = AdatPrgJogDrive( AdatV, AdatP );
			if( AdatV->PrgJogStat != ADAT_PJOG_NOP )
			{	/* �^�]���s�X�e�[�^�X��NOP�ȊO �� �^�]�� */
				switch ( FunExe->CmdCode )
				{
					case FCMD_UP:		/* Forward revolution (�p�����s) 						*/
						if( AdatV->MoveDistance > 0 )
						{
							KlibRstLongTimer( &AdatV->PrgJogDrvCntr );
						}
						break;

					case FCMD_DOWN:		/* Reverse revolution (�p�����s) 						*/
						if( AdatV->MoveDistance < 0 )
						{
							KlibRstLongTimer( &AdatV->PrgJogDrvCntr );
						}
						break;

					case FCMD_NONE:		/* ���߂Ȃ�												*/
					default:
						if( (KlibGetLongTimerMs( AdatV->PrgJogDrvCntr ) > (ULONG)(AdatP->FnCmnCtrl->SafeStopTime * 1000)) /* <S195> */
															&& (AdatP->FnCmnCtrl->SafeStopTime != 65535) )
						{	/* ���S��~����[s]�ȏ�Œ�~ */
							AdatP->BaseCtrlData->PJogHdl.PJogV.Var.RoutCmd = PJOGCMD_ABORT; /* �^�]���f */
							AdatV->ErrState = ADAT_COMERR;									/* �ʐM�ُ�	*/
						}
						break;
				}
			}
			else
			{	/* �^�]��~��	*/
				if(AdatV->PrgJogStat < ADAT_PJOG_NOP )
				{/* �ُ�ݒ�Ȃ�G���[�X�e�[�^�X��ݒ�	*/
					AdatV->ErrState = AdatV->PrgJogStat;
				}

				if( AdatV->f.DrvErrDetect == FALSE )
				{/* �^�]��~���͖������ɒʐM�ُ팟�o�E�G�C�g�^�C�}������ */
					KlibRstLongTimer( &AdatV->PrgJogDrvCntr );
				}
			}
		}
	}
	/*------------------------------------------------------------------------------------------*/
	/*	���g����͏����V�[�P���X																*/
	/*------------------------------------------------------------------------------------------*/
	AdatV->FftStat = TuneVibrationTrace( AdatP->DetVib, AdatP->FftAna, AdatV->f.FftReq, 0,  FALSE);

	/*------------------------------------------------------------------------------------------*/
	/*	COIN�o�̓X�e�[�^�X�쐬																	*/
	/*------------------------------------------------------------------------------------------*/
	AdatReg->AdatState |= AdatV->CoinWaitStat;						/* COIN�o�̓X�e�[�^�X		*/

	return rc;
}

/****************************************************************************
	Fn202 Advanced Auto Tuning mode Condition Check
****************************************************************************/
static LONG	AdatRefInExeModeExeCheck( ADATP *AdatP )
{
	LONG rc;
	rc = TRUE;

//	if(((AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS) && (AdatP->BaseCtrlData->BeComplete == TRUE))
	if((AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm != CTRLMODE_POS) || (AdatP->BaseCtrlData->BeComplete == FALSE)	/* <S085> */
	 || (AdatP->AlmManager->AlmCode != 0x00)
	 || (AdatP->SeqCtrlOut->HwbbSts == TRUE)
	 || (AdatP->SeqCtrlOut->MainPowerOn == FALSE)
	 || (AdatP->SeqCtrlOut->OverTrvlSts == TRUE) )
	{
		rc = FALSE;
	}
	return rc;
}

/****************************************************************************
	Fn201 Update One Parameter Tuning Gain In EEPROM
****************************************************************************/
static LONG	AdatUpdateGainInEeprom( ADATREG *AdatReg, ADATP *AdatP )
{
	PRM_RSLT		rc;
	REGIF_CONFIG_T *RegCfg;
	DOBS_CTRL	   *DobsCtrl;
	MFCTRL		   *MFControl;

//	rc        = PRM_RSLT_SUCCESS;/*<S09E>*/
	RegCfg    = AdatP->RegManager->hPnReg;
	DobsCtrl  = &AdatP->BaseCtrlData->DobsCtrl;
	MFControl = &AdatP->BaseCtrlData->MFControl;

	/* Pn100 ���x���[�v�Q�C�� */
	rc = PrmWriteValueToEeprom( RegCfg, &pndef_loophz, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn101 ���x���[�v�ϕ����萔 */
	rc = PrmWriteValueToEeprom( RegCfg, &pndef_pitime, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn102 �ʒu���[�v�Q�C�� */
	rc = PrmWriteValueToEeprom( RegCfg, &pndef_posgn, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn103 �������[�����g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_jrate, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn401 1�i�ڑ�1�g���N�w�߃t�B���^���萔 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn409 1�i�ڃm�b�`�t�B���^���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40A 1�i�ڃm�b�`�t�B���^Q�l */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40B 1�i�ڃm�b�`�t�B���^�[�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchdamp1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40C 2�i�ڃm�b�`�t�B���^���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn40D 2�i�ڃm�b�`�t�B���^Q�l */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	if( DobsCtrl->var.DobsAct == TRUE )
	{
		/* Pn121 ���C�⏞�Q�C�� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn123 ���C�⏞�W�� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dtrqgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn125 ���C�⏞�Q�C���␳ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobjgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
	}

	if(   ( AdatReg->AdatLevel == ADAT_LEVEL_POS )
	   || ( AdatReg->AdatLevel == ADAT_LEVEL_POS_ST ) )
	{

		/* Pn141 ���f���Ǐ]����Q�C�� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn142 ���f���Ǐ]����Q�C���␳ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlzeta, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn143 ���f���Ǐ]����o�C�A�X(���]����) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlptff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		/* Pn144 ���f���Ǐ]����o�C�A�X(�t�]����) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlntff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}

		if(MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS)
		{	/* 2����MFC�L��	*/
			/* Pn145 �U���}���P���g��A */
			rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlantfrq, 0 );
			if( rc != PRM_RSLT_SUCCESS )
			{
				return(rc);
			}

			/* Pn146 �U���}���P���g��B */
			rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlresfrq, 0 );
			if( rc != PRM_RSLT_SUCCESS )
			{
				return(rc);
			}
		}

		/* Pn147 ���f���Ǐ]���䑬�x�t�B�[�h�t�H���[�h�⏞ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlvff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
	}

	/* Pn140 ���f���Ǐ]����֘A�X�C�b�` */
	/* AAT���s�OMFC�L���ŁhLevel=1�h�̂Ƃ���Pn140=xx00��EEPROM�ɕۑ�����K�v������ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn161 �`�^���U���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfrq, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn163 �`�^���U�_���s���O�Q�C�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibdamp, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn160 ���U����֘A�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	/* Pn170 �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_DatLevel, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	return(rc);
}

/****************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O�p�����[�^UNDO����

	�T�v:	�p�����[�^���@�\���s�O��RAM��Ԃɖ߂��B
****************************************************************************/
static void	AdatUndoRamGain( ADATV *AdatV, ADATP *AdatP )
{
	REGIF_CONFIG_T *RegCfg;
	RegCfg   = AdatP->RegManager->hPnReg;

	/* Pn100 ���x���[�v�Q�C�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_loophz,	0 );

	/* Pn101 ���x���[�v�ϕ����萔 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pitime,	0 );

	/* Pn102 �ʒu���[�v�Q�C�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_posgn,		0 );

	/* Pn103 �������[�����g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_jrate,		0 );

	/* Pn401 1�i�ڑ�1�g���N�w�߃t�B���^���萔 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_trqfil11,	0 );

	/* Pn409 1�i�ڃm�b�`�t�B���^���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1,	0 );

	/* Pn40A 1�i�ڃm�b�`�t�B���^Q�l */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1,	0 );

	/* Pn40B 1�i�ڃm�b�`�t�B���^�[�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchdamp1, 0 );

	/* Pn40C 2�i�ڃm�b�`�t�B���^���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2,	0 );

	/* Pn40D 2�i�ڃm�b�`�t�B���^Q�l */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2,	0 );

	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw,	0 );

	/* Pn121 ���C�⏞�Q�C�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobgn,		0 );

	/* Pn123 ���C�⏞�W�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dtrqgn,	0 );

	/* Pn125 ���C�⏞�Q�C���␳ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobjgn,	0 );

	/* Pn141 ���f���Ǐ]����Q�C�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlgn,		0 );

	/* Pn142 ���f���Ǐ]����Q�C���␳ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlzeta,	0 );

	/* Pn143 ���f���Ǐ]����o�C�A�X(���]����) */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlptff,	0 );

	/* Pn144 ���f���Ǐ]����o�C�A�X(�t�]����) */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlntff,	0 );

	/* Pn145 �U���}���P���g��A */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlantfrq, 0 );

	/* Pn146 �U���}���P���g��B */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlresfrq, 0 );

	/* Pn147 ���f���Ǐ]���䑬�x�t�B�[�h�t�H���[�h�⏞ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlvff,	0 );

	/* Pn140 ���f���Ǐ]����֘A�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlsw,		0 );

	/* Pn161 A�^���U���g�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq,	0 );

	/* Pn163 A�^���U�_���s���O�Q�C�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp,	0 );

	/* Pn160 ���U����֘A�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibsw,	0 );

	/* Pn310 �U�����o�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_vibsel,	0 );

	/* Pn311 �U�����o���x */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_vibsens,	0 );

	/* Pn533 �v���O����JOG�ړ����x */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogrotspd,0 );

	/* Pn534 �v���O����JOG���������� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogacctm, 0 );

	/* Pn531 �v���O����JOG�ړ����� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogdistl, 0 );

	/* Pn535 �v���O����JOG�҂����� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjogwaittm,0 );

	/* Pn585 �v���O����JOG�ō����x(���j�A) */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pjoglinspd,0 );

	/* Pn560 �c���U�����o�� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_remdetw,	0 );

	/* Pn170 �_�C�i�~�b�N�I�[�g�`���[�j���O�֘A�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_DatLevel,	0 );

	AdatUndoRamGainforJrat( AdatV, AdatP );

}

/****************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O �������[�����g����UNDO����

	�T�v:	�������[�����g����֘A�̃p�����[�^���@�\���s�O��RAM��Ԃɖ߂��B
****************************************************************************/
static void	AdatUndoRamGainforJrat( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA 		*Prm;
	REGIF_CONFIG_T  *RegCfg;

	Prm      = AdatP->UniPrms->Prm;
	RegCfg   = AdatP->RegManager->hPnReg;
	AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl = (AdatV->f.OrgIPSpdControl)? TRUE : FALSE;

	/* �ʒu�w�߃t�B���^�N�� */
	BpiPcmdFilSwitch( &AdatP->BaseCtrlData->PosManager, TRUE );

	/* MFC������������	 	*/
	MfcSetMfcCtrlSwitch( &AdatP->BaseCtrlData->MFControl, TRUE );

	/*----------------------------------------------------------------------*/
	/*	�����`�^�ݒ�۔��f												*/
	/*		�C�i�[�V������L�����͂`�^����OFF����邽�߁A					*/
	/*		�ēx�AA�^���������L��������s��									*/
	/*----------------------------------------------------------------------*/
	if( ( ((Prm->avibsw >> 4) & 0x0F)  == FALSE) 	/* ������������			*/
		|| ((Prm->avibsw & 0x0F) == TRUE) )			/* �܂��́AA�^���ɗL��	*/
	{
		AdatV->f.UsedAResCtrl = TRUE;				/* �ݒ�ς�				*/
	}
	else
	{
		AdatV->f.UsedAResCtrl = FALSE;				/* ���ݒ�				*/
	}

	/* Pn109 �t�B�[�h�t�H���[�h */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ffgn, 0 );

	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( Prm->tfuncsw & 0xF000 )
	{
		Prm->tfuncsw = Prm->tfuncsw | 0x1000;
		PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	}

	/* Pn10B �Q�C���֌W���p�X�C�b�` */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_gnmode, 0 );

}

/****************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O����w�ߍ쐬����

	�T�v:	�v���O����JOG�𗘗p���āC�ʒu�w�߂��쐬����B
        	�{�̂́C�v���O����JOG�𗘗p
			�^�]��~�v���������Ă��A�^�]�p�^�[���́A��ʂ���s����B
			�v���O����JOG�w�ߊ���(DEN)��̑҂����Ԃ́A300ms�Ƃ���
			�J��Ԃ��񐔂́A�v���O����JOG��PJogP->Times�łȂ��ALEVEL�ݒ�ɏ]����
			�A�h�o���X�g�I�[�g�`���[�j���O�Ǝ��ɊǗ�����B
****************************************************************************/
static SHORT	AdatPrgJogDrive( ADATV *AdatV, ADATP *AdatP )
{
	PJOGV	*PJogV;
	SHORT	rc;

	PJogV = &AdatP->BaseCtrlData->PJogHdl.PJogV;
	rc    = AdatV->PrgJogStat; 					/* ���^�[���R�[�h��O��X�e�[�^�X�ɏ����� 	*/

	if( (PJogV->Var.State == PJOG_ABORT) || (PJogV->Var.State == PJOG_ERROR)
	  || (AdatV->PrgJogPattern == ADAT_PJOG_STOP) )
	{	/* �v���O����JOG�^�]���~ */
		AdatV->PrgJogPtr = 0;					/* �^�]��~�B�V�[�P���X�������ݒ�ɃZ�b�g	*/
		rc = ADAT_PJOG_NOP;						/* �^�]��~��Ԃ�							*/
	}

	switch ( AdatV->PrgJogPtr )
	{
		/*----------------------------------------------------------------------------------*/
		/*	Initial process of Jog sequence 												*/
		/*----------------------------------------------------------------------------------*/
		case 0:
			AdatPJogInit( AdatV, AdatP );
			AdatV->PrgJogPtr = 1;
			AdatV->PrgJogPattern = AdatV->PrgJogPatternReq;
			break;

		case 1:
			AdatV->PrgJogActPattern = (AdatV->PrgJogPattern & 0x0000000F);

			if( AdatV->PrgJogActPattern == ADAT_PJOG_STOP )
			{	/* �^�]��~�v������	*/
				rc = ADAT_PJOG_NOP;							/* �^�]��~��Ԃ�				*/
			}
			else
			{
				AdatV->PrgJogPatternCnt = 0;
				AdatPJogDrvSet( AdatV, AdatP, AdatV->PrgJogActPattern );/* �^�]�p�^�[���ݒ�	*/
				AdatV->PrgJogPtr = 2;						/* �V�[�P���X�|�C���^�X�V		*/
				rc = ADAT_PJOG_EXEC;
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	�^�]�J�n����																	*/
		/*----------------------------------------------------------------------------------*/
		case 2:
			BpiRsetCoinOffTime( AdatP->SettlingTime );
			BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* �I�[�o�V���[�g�ʍő�l�N���A		*/

			if( AdatV->f.GainTuneRun == TRUE )
			{	/* �`���[�j���O���́C��U�C�N���A���� */
				DetVibResetMaxMinSpeedError( AdatP->DetVib );
			}

			PJogV->Var.RoutCmd = PJOGCMD_START;
			KlibRstLongTimer( &AdatV->PrgJogMovStartCntr );
			AdatV->PrgJogPtr = 3;							/* �ړ������҂���				*/
			rc = ADAT_PJOG_EXEC;
			break;

		/*----------------------------------------------------------------------------------*/
		/*	�ړ��w�ߊ����҂�																*/
		/*----------------------------------------------------------------------------------*/
		case 3:
			if( (PJogV->Var.RoutCmd == PJOGCMD_START) && (PJogV->Var.State == PJOG_START) )
			{
				PJogV->Var.RoutCmd = PJOGCMD_NONE;
			}

			if( KlibGetLongTimerMs( AdatV->PrgJogMovStartCntr ) > KPI_SCCYCLEMS * 2 )
			{	/* ScanC��PJOGCMD_START���Z�b�g���邽�߁AScanC���K��1�񑖂鎞�Ԃ����҂� 	*/
				if( (PJogV->Var.State == PJOG_ABORT) || (PJogV->Var.State == PJOG_END)
				 || (PJogV->Var.State == PJOG_ERROR) )
				{
					PJogV->Var.RoutCmd = PJOGCMD_NONE;
					KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
					KlibRstLongTimer( &AdatV->CoinWaitCntr );
					AdatV->CoinWaitStat = ADAT_STS_COINWAIT;
					AdatV->PrgJogPtr = 4;					/* �^�]�����҂���				*/
				}
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	�^�]�����҂�																	*/
		/*----------------------------------------------------------------------------------*/
		case 4:
			if( AdatV->f.CoinIgnore == TRUE )
			{
				if( PJogV->Cnst.RevDir == FALSE)
				{
					rc = ADAT_PJOG_PLUSEND;
				}
				else
				{
					rc = ADAT_PJOG_MINUSEND;
				}
				AdatV->PrgJogPtr = 5;						/* �E�G�C�g��					*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				AdatV->CoinWaitStat = ADAT_STS_COINNOWAIT;
			}
			else if( AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE ) /* COIN���b�`���� */
			{
				if( PJogV->Cnst.RevDir == FALSE)
				{
					rc = ADAT_PJOG_PLUSEND;
				}
				else
				{
					rc = ADAT_PJOG_MINUSEND;
				}
				AdatV->PrgJogPtr = 5;					 	/* �E�G�C�g��					*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				AdatV->CoinWaitStat = ADAT_STS_COINNOWAIT;
			}
			else if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT1 )
			{	/* ��10�b�ʒu���ߊ������Ȃ��Ƃ���COIN�҂��G���[	*/
				AdatV->ErrState = ADAT_COINERR;
				AdatV->CoinWaitStat = ADAT_STS_COINERR;		/* �G���[�o��ON					*/
			}
			else if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT2 )
			{	/* COIN �o�͑҂��x����	*/
				AdatV->CoinWaitStat = ADAT_STS_COINWARN;	/* �x���o��						*/
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	�E�G�C�g																		*/
		/*----------------------------------------------------------------------------------*/
		case 5:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr )
											>=	(ULONG)((PJogV->Cnst.WaitTime + AATPJOGWAITTM)) )
			{	/* �^�]����		*/
				AdatV->PrgJogPtr = 6;					/* ����^�]������					*/
			}
			break;

		/*----------------------------------------------------------------------------------*/
		/*	����^�]����																	*/
		/*----------------------------------------------------------------------------------*/
		case 6:
			AdatV->PrgJogPatternCnt++;
			AdatV->PrgJogActPattern = ((AdatV->PrgJogPattern
											>> (4 * AdatV->PrgJogPatternCnt)) & 0x0000000F);

			if( AdatV->PrgJogActPattern == 0x0F)
			{	/* �^�]�J��Ԃ�	*/
				/* �J��Ԃ�����	*/
				if( ++AdatV->PrgJogDrvCnt >= AdatV->PrgJogTimes )
				{	/* �J��Ԃ��񐔎��s����	*/
					AdatV->PrgJogDrvCnt = 0;				/* �J��Ԃ��J�E���^�����ݒ�		*/
					AdatV->PrgJogPatternCnt++;
				}
				else
				{	/* �J��Ԃ��񐔖���		*/
					AdatV->PrgJogPatternCnt--;
				}
				AdatV->PrgJogActPattern = ((AdatV->PrgJogPattern
											>> (4 * AdatV->PrgJogPatternCnt)) & 0x0000000F);
			}

			/* �V�[�P���X�ݒ�	*/
			if( AdatV->PrgJogActPattern == 0x00 )
			{	/* �^�]����	*/
				AdatV->PrgJogPtr = 7;			/* �^�]��~�B�V�[�P���X�������ݒ�ɃZ�b�g 	*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				rc = ADAT_PJOG_FIN;				/* �^�]�����X�e�[�^�X�Z�b�g					*/
			}
			else
			{	/* �^�]�p��	*/
				AdatPJogDrvSet( AdatV, AdatP, AdatV->PrgJogActPattern );/* �^�]�p�^�[���ݒ�	*/
				AdatV->PrgJogPtr = 2;							/* �V�[�P���X�|�C���^�X�V	*/
				rc = ADAT_PJOG_EXEC;
			}
			break;

		case 7:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr )
										>= (ULONG)((PJogV->Cnst.WaitTime + AATPJOGWAITTM) / 2 ))
			{	/* �^�]����	*/
				AdatV->PrgJogPtr = 0;						/* ����^�]������				*/
			}
			break;

		default:
			AdatV->ErrState = ADAT_PROGERR;					/* �ُ�I��						*/
			break;
	}

	return rc;
}



/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O �v���O����JOG����������
****************************************************************************/
static void	AdatPJogInit( ADATV	*AdatV, ADATP *AdatP )
{
	PJOGHNDL *PJogHdl;
	FUN_CMN_CONTROL *FnCmnCtrl;

	PJogHdl = &AdatP->BaseCtrlData->PJogHdl;
	FnCmnCtrl = AdatP->FnCmnCtrl;

	PJogHdl->PJogV.Var.RoutCmd = PJOGCMD_INIT;
	PrgJogReset( PJogHdl );

	/* �ړ��� */
	PJogHdl->PJogV.Cnst.MTimes = 1;		/* �ړ��񐔂͂P��ɌŒ�			*/
	AdatV->PrgJogTimes  = 3;			/* �v���O�����J��Ԃ��񐔐ݒ�	*/
	AdatV->PrgJogDrvCnt = 0;			/* �J��Ԃ��J�E���^�����ݒ�		*/
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_PJOG;
}

/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O �v���O����JOG�p�����[�^�ݒ菈��

	�T�v:	�^�]�p�^�[���ɏ]���āA�v���O����JOG�֘A��
			�ړ������A���x�A�����x�A�҂����ԁA�J��Ԃ��񐔂�ݒ肷��B
****************************************************************************/
static void	AdatPJogDrvSet( ADATV *AdatV, ADATP *AdatP, ULONG pattern )
{
	LONG	sx;
	LONG	kx;
	LONG	jratex;
	LONG	wk_spd;
	LONG	trq_per;						/* ��i100%�g���N						*/
	CHAR	pjog_switch;					/* �����I���X�C�b�`						*/
	LONG	baseVel_wk;						/* ����x[min-1] DD���[�^[0.1min-1]	*/
	LONG	VelData;
	LONG	AccData;
	LONG	AccGain;

	PJOGV	*PJogV;
	JATHNDL	*JatHdl;
	PRMDATA *Prm;
	BPRMDAT	*Bprm;
	REG_ACC_T AccessPrm;

	PJogV  = &AdatP->BaseCtrlData->PJogHdl.PJogV;
	JatHdl = &AdatP->BaseCtrlData->JatHdl;
	Prm    =  AdatP->UniPrms->Prm;
	Bprm   =  AdatP->UniPrms->Bprm;

	trq_per = TUNEACCTRQ2;
	jratex = Prm->jrate + 100;
	pjog_switch = 0;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( pattern )
	{
		case 0x03:		/* �ʏ푗�萳�]					*/
		case 0x04:		/* �ʏ푗��t�]					*/
			/* �X�g���[�N�ݒ�	[�w�ߒP��]				*/
			/* [1000�w�ߒP��]��[�w�ߒP��]�ϊ�=125�{		*/
			PJogV->Cnst.Distance = MlibAbs32( AdatV->MoveDistance ) / 8;

			/* ���x�ݒ�			[min-1]					*/
			/* �v���O����JOG���x�ɑ΂���{��=1�{		*/
			wk_spd = 1;

			/* �҂����Ԑݒ�		[ms]					*/
			/* �v���O����JOG�J�n�҂�����=300ms+300ms	*/
			PJogV->Cnst.WaitTime = AATPJOGWAITTM;
			AdatV->f.CoinIgnore = FALSE;
			break;

		case 0x06:		/* �C�i�[�V�����葁���萳�]		*/
		case 0x07:		/* �C�i�[�V�����葁����t�]		*/
			/* �X�g���[�N�ݒ�	[�w�ߒP��]				*/
			/* [1000�w�ߒP��]��[�w�ߒP��]�ϊ�=1000�{ 	*/
			if( MlibAbs32( AdatV->MoveDistance ) > AdatV->DefaultDistance )
			{
				PJogV->Cnst.Distance = AdatV->DefaultDistance;
			}
			else
			{
				PJogV->Cnst.Distance = MlibAbs32( AdatV->MoveDistance );
			}

			/* ���x�ݒ�	[min-1]							*/
			/* �v���O����JOG���x�ɑ΂���{��=2�{		*/
			wk_spd = 2;
			/* �C�i�[�V����C�g���N��ݒ� */
			if( AdatV->JratDrvCnt != 0 )
			{
				/* ���蒆�́C������x���ׂ��c���ł��Ă���̂ŁC����l�ɉ����ĉ����g���N������	*/
				trq_per = JRATACCTRQ2;							/* ��i�g���N100%����			*/
				if( AdatV->Jrate > pndef_jrate.UpperLimit )
				{
					jratex = pndef_jrate.UpperLimit + 100;
				}
				else
				{

					jratex = JatHdl->EstimatJrat + 100;			/* �C�i�[�V������l�{100%		*/
				}
			}
			else
			{
				/* �C�i�[�V������ŏ��̓���w��							*/
				/* ���菉��́C�ǂ�ȕ��ׂ����Ă��邩�킩��Ȃ��̂ŁC	*/
				/* �u�����C�i�[�V����8%�����g���N�v�Ƃ���				*/
				trq_per = JRATACCTRQ;
				jratex = (USHORT)Prm->Pn324 + 100;
			}
			pjog_switch = 1;		/* �����x�ݒ�I�� */
			/* �҂����Ԑݒ�[ms] 									*/
			/* �v���O����JOG�J�n�҂�����=0ms, �V�[�P���X�J�ڕ����Z 	*/
			PJogV->Cnst.WaitTime = 0;
			AdatV->f.CoinIgnore = TRUE;
			break;

		case 0x01:		/* �����萳�]					*/
		case 0x02:		/* ������t�]					*/
		default:		/* �f�t�H���g�͑����萳�]		*/
			/* �X�g���[�N�ݒ�	[�w�ߒP��]				*/
			/* [1000�w�ߒP��]��[�w�ߒP��]�ϊ�=1000�{	*/
			PJogV->Cnst.Distance = MlibAbs32( AdatV->MoveDistance );

			/* ���x�ݒ�			[min-1]					*/
			/* �v���O����JOG���x�ɑ΂���{��=2�{		*/
			wk_spd = 2;
			/* �҂����Ԑݒ�		[ms]					*/
			/* �v���O����JOG�J�n�҂�����=300ms+300ms	*/
			PJogV->Cnst.WaitTime = AATPJOGWAITTM;
			AdatV->f.CoinIgnore = FALSE;
			break;
	}

	/* �^�]�����ݒ�				*/
	switch ( pattern )
	{
		case 0x02:		/* ������t�]	*/
		case 0x04:		/* �ʏ푗��t�]	*/
			PJogV->Cnst.MTimes = 1;
			if( AdatV->MoveDistance >= 0 )
			{
				/* �^�]�ݒ�				*/
				PJogV->Cnst.Pattern = TRUE;
				/* �^�]���������ݒ�		*/
				PJogV->Cnst.RevDir = TRUE;
			}
			else
			{
				/* �^�]�ݒ�				*/
				PJogV->Cnst.Pattern = FALSE;
				/* �^�]���������ݒ�		*/
				PJogV->Cnst.RevDir = FALSE;
			}
			break;
		case 0x06:		/* �C�i�[�V�����葁���萳�]	*/
		case 0x07:		/* �C�i�[�V�����葁����t�]	*/
			PJogV->Cnst.MTimes = 4;			/* �ړ��񐔂�2�����ɌŒ� */
			if( AdatV->MoveDistance >= 0 )
			{
				/* �^�]�ݒ�				*/
				PJogV->Cnst.Pattern = 4;
				/* �^�]���������ݒ�		*/
				PJogV->Cnst.RevDir = TRUE;
			}
			else
			{
				/* �^�]�ݒ�				*/
				PJogV->Cnst.Pattern = 5;
				/* �^�]���������ݒ�		*/
				PJogV->Cnst.RevDir = FALSE;
			}
			break;
		case 0x01:		/* �����萳�]	*/
		case 0x03:		/* �ʏ푗�萳�]	*/
		default:
			PJogV->Cnst.MTimes = 1;
			if( AdatV->MoveDistance >= 0 )
			{
				/* �^�]�ݒ�				*/
				PJogV->Cnst.Pattern = FALSE;
				/* �^�]���������ݒ�		*/
				PJogV->Cnst.RevDir = FALSE;
			}
			else
			{
				/* �^�]�ݒ�				*/
				PJogV->Cnst.Pattern = TRUE;
				/* �^�]���������ݒ�		*/
				PJogV->Cnst.RevDir = TRUE;
			}
			break;
	}

	/* ����x		*/
	if( AdatP->MencV->LowSpdMotor == TRUE )
	{	/* DD���[�^	*/
		baseVel_wk = Bprm->NorRatSpd * 10 / 3;	/* ����x=��i���x��3 */
										/* 10�{����̂́CPJOGROTSPD�P�ʂ�[0.1min-1]�ƂȂ邽��	*/
	}
	else if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
	{	/* ���j�A���[�^	*/
		baseVel_wk = MlibMIN( (Bprm->NorRatSpd / 3), (Bprm->NorMaxSpd / 2) );
											/* ����x�́A��i���x��3�ƍō����x��2�̏�������	*/
	}
	else
	{	/* ��]�^���[�^	*/
		baseVel_wk = Bprm->NorRatSpd / 3;							/* ����x�́A��i���x��3	*/
	}

	/*------------------------------------------------------------------------------------------*/
	/*	�����g���N�w��i���x�͊���x�j														*/
	/*------------------------------------------------------------------------------------------*/
	/* ���x�ݒ�	[min-1] */
#if (FLOAT_USE==TRUE)
	VelData = baseVel_wk * wk_spd * Bprm->Kmonspd ;
#else
	VelData = MlibMulgainSx48( baseVel_wk * wk_spd, Bprm->Kmonspd );
#endif /* FLOAT_USE */
	if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )				/* ���j�A���[�^ 			*/
	{
		TunePrmWriteRam( &AccessPrm, &pndef_pjoglinspd, (LONG)VelData, pndef_pjoglinspd.DataLength );
	}
	else
	{
		TunePrmWriteRam( &AccessPrm, &pndef_pjogrotspd, (LONG)VelData, pndef_pjogrotspd.DataLength );
	}

	/* �����x�ݒ� [ms] */
	kx = MlibScalKxgain( jratex, Bprm->PerMaxTrq, 100, &sx, 0 );	/* Bprm.PerMaxTrq[%] /100	*/
	kx = MlibPcalKxkskx( kx, Bprm->OvrSpd, 0x1000000, &sx, 0 );
	kx = MlibPcalKxksks( kx, Bprm->Jmot, Bprm->MaxTrq, &sx, 0 );
	AccGain = MlibPcalKxgain( kx, 10000, trq_per, &sx, 24 );		/* 1/������x	[ms/Nos]	*/
#if (FLOAT_USE==TRUE)
	AccData = PJogV->Cnst.MaxSpd * AccGain;
#else
	AccData = MlibMulgain( PJogV->Cnst.MaxSpd, AccGain );
#endif /* FLOAT_USE */

	if( AccData > pndef_pjogacctm.UpperLimit )
	{
		AccData = pndef_pjogacctm.UpperLimit;
	}
	else if( AccData < pndef_pjogacctm.LowerLimit )
	{
		AccData = pndef_pjogacctm.LowerLimit;
	}
	TunePrmWriteRam( &AccessPrm, &pndef_pjogacctm, (LONG)AccData, pndef_pjogacctm.DataLength );

	if( pjog_switch == 1 )
	{	/* �C�i�[�V������	*/
		/* ��ړ������v�Z	[�w�ߒP��]	*/
		/*--------------------------------------------------------------------------------------*/
		/*	�ڕW�g���N�ɂĉ������C�ő�^�]���x�Ŋ��葬�x���Ԉړ������ꍇ�̈ړ�����			*/
		/*	����w�߂̊�ƂȂ�ړ��������Z�o����												*/
		/*																						*/
		/*	��ړ����� [�w�ߒP��] = �ő�^�]���x[�w�ߒP��/ms] * ���葬�x����[ms]			*/
		/*																						*/
		/*									�ő�^�]���x[�w�ߒP��/ms^2]							*/
		/*								+ -------------------------------						*/
		/*										�����x[�w�ߒP��/ms^2]							*/
		/*																						*/
		/*--------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( PJogV->Cnst.MaxSpd, 1000, Bprm->NorOvrSpd, &sx, 0 );
		kx = MlibPcalKxkskx( kx, Bprm->OvrPspd, 1, &sx, 24 );
#if (FLOAT_USE==TRUE)
		kx = (AATCONSVELTM * kx ) + ( Prm->pjogacctm * kx );
#else
		kx = MlibMulgain( AATCONSVELTM, kx ) + MlibMulgain( Prm->pjogacctm, kx );
#endif /* FLOAT_USE */

		/*	���쑬�x�Čv�Z[min-1] */
		/*--------------------------------------------------------------------------------------*/
		/*	�����x�������Ŋ�ړ������Ǝw�߃X�g���[�N���瓮�쑬�x���Čv�Z					*/
		/*																						*/
		/*											  �w�߃X�g���[�N(L_ref)[�w�ߒP��]			*/
		/*	�C�����x[min-1] = �ő�^�]���x[min-1] * (-----------------------------------)^0.5	*/
		/*												��ړ�����(L_base)[�w�ߒP��]			*/
		/*																						*/
		/*--------------------------------------------------------------------------------------*/
		kx = MlibScalKxgain( PJogV->Cnst.Distance, 1, kx, &sx, 24 );/* L_ref/L_base				*/
#if (FLOAT_USE==TRUE)
		kx = (VelData * VelData) * kx ;				/* = Vmax^2 * L_ref/L_base	*/
#else
		kx = MlibMulgain( (VelData * VelData), kx );				/* = Vmax^2 * L_ref/L_base	*/
#endif /* FLOAT_USE */
		kx = MlibSqrtu32( kx );										/* �C�����x	[min-1]			*/

		/*--------------------------------------------------------------------------------------*/
		/*	�ő�C�ŏ��N�����v																	*/
		/*--------------------------------------------------------------------------------------*/
		if( kx > 2 * VelData )
		{
			kx = 2 * VelData;								/* ����x�~2�ŃN�����v			*/
		}
		else if( kx < VelData / 2 )
		{
			kx = VelData / 2;								/* ����x��2�ŃN�����v			*/
		}

		if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
		{
			TunePrmWriteRam( &AccessPrm, &pndef_pjoglinspd, (LONG)kx, pndef_pjoglinspd.DataLength );
		}
		else
		{
			TunePrmWriteRam( &AccessPrm, &pndef_pjogrotspd, (LONG)kx, pndef_pjogrotspd.DataLength );
		}

		/*	�������ԍČv�Z [ms] */
#if (FLOAT_USE==TRUE)
		AccData = PJogV->Cnst.MaxSpd * AccGain ;	/* �C����������	[ms]			*/
#else
		AccData = MlibMulgain( PJogV->Cnst.MaxSpd, AccGain );	/* �C����������	[ms]			*/
#endif /* FLOAT_USE */
		if( AccData > pndef_pjogacctm.UpperLimit )
		{
			AccData = pndef_pjogacctm.UpperLimit;
		}
		else if( AccData < JRATPJOGACC )
		{
			AccData = JRATPJOGACC;
		}
		else if( AccData < pndef_pjogacctm.LowerLimit )
		{
			AccData = pndef_pjogacctm.LowerLimit;
		}
		TunePrmWriteRam( &AccessPrm, &pndef_pjogacctm, AccData, pndef_pjogacctm.DataLength );
	}

	if( ( PJogV->Cnst.PrmUnMatch == TRUE ) || ( PJogV->Cnst.PrmUnMatch2 == TRUE ) )
	{
		ALMSetServoAlarm( AdatP->AlmManager, ALM_PRMUNMATCH );/* A.042 : �p�����[�^�g���킹�ُ� */
	}

	AdatV->f.DrvPatternSet = TRUE;
}

/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O����������

	�T�v:�A�h�o���X�I�[�g�`���[�j���O�J�n���Ƀt���O�A�֘A�f�[�^�̏��������s���B
****************************************************************************/
static PRM_RSLT AdatInitial( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP)
{
	ULONG	data;
	PRM_RSLT rc;

	PRMDATA *Prm;
	MFCTRL	*MFControl;
	REG_ACC_T AccessPrm;

	Prm       =  AdatP->UniPrms->Prm;
	MFControl = &AdatP->BaseCtrlData->MFControl;
	rc = PRM_RSLT_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	AdatV->ModeSetPtr      = 0;
	AdatV->JratSetPtr      = 0;
	AdatV->VibDetPtr       = 0;
	AdatV->KvTunePtr       = 0;
	AdatV->KpTunePtr       = 0;
	AdatV->KpSetPtr        = 0;
	AdatV->MfcTunePtr      = 0;
	AdatV->NotchSetPtr     = 0;
	AdatV->PrgJogPtr       = 0;
	AdatV->PrgJogPatternReq= ADAT_PJOG_STOP;
	AdatV->PrgJogPattern   = ADAT_PJOG_STOP;
	AdatV->JratChangeTimes = 0;
	AdatV->JratDrvCnt      = 0;
	AdatV->VibMaxKv        = 0;
	AdatV->VibMaxKvAresOFF = 0;			/* A�^ON�O�̃Q�C���B���݂̃Q�C���Ɣ�r����̂ŏ����l��0	*/
	AdatV->MaxKv = Prm->adatgmax;		/* �T���ő�Q�C�����ő�l�Q�C���ɐݒ�  					*/
	AdatV->MaxKp = AdatV->MaxKv;		/* �ݒ�Kp�ő�l�����l�͒T���ő�Kv�Ɠ���					*/
	/* �U�����o���x���̏����� */
	AdatP->DetVib->conf.VibCheckLevel = ADAT_GRAT_70( AdatP->DetVib->conf.VibCheckLevel );
	/* AdatV->VibSpeed�́C�U�����x������ŃZ�b�g����邪�O�̂��ߏ��������s��	*/
	AdatV->VibSpeed = AdatP->DetVib->conf.VibCheckLevel;
	AdatV->JratErr  = ADAT_JRAT_OK;
	AdatV->f.CoinWaitStart = FALSE;		/* COIN�o�͑҂��������Z�b�g								*/
	AdatV->f.Used2MassMfc  = FALSE;		/* MFC(����+�@�䃂�f��)�ݒ�ς݃t���O��������			*/
	AdatV->f.Mfc2MassOK    = FALSE;		/* MFC(����+�@�䃂�f��)�ŏI�����ς݃t���O��������		*/
	AdatV->f.MfcLastSet    = FALSE;		/* MFC(����+�@�䃂�f��)��������							*/
	AdatV->Set2MassMfcSequence = 0;		/* MFC(����+�@�䃂�f��)�ݒ莞�V�[�P���X��������			*/
	AdatV->f.StopDetectMode2   = FALSE;	/* ��~�����COIN�AMotStop���g�p����					*/
	AdatV->frq_buf = Prm->mdlantfrq;	/* ���U�E�����U���g���o�b�t�@							*/
	AdatV->Kpm_buf = Prm->mdlgn; 		/* Kpm�o�b�t�@											*/
	AdatV->Kv_buf  = Prm->loophz;		/* Kv�o�b�t�@											*/

	/* ���ł�MFC(����+�@�䃂�f��)�ݒ�ς݂̎��ɂ�MFC(����+�@�䃂�f��)�̒����͂��Ȃ�	*/
	if(( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
			|| (((Prm->mdlsw >> 8) & 0x000F) != 0x0001))
	{
		AdatV->f.Used2MassOld = TRUE;
	}
	else
	{
		AdatV->f.Used2MassOld = FALSE;
	}

	AdatP->FnCmnCtrl->FnSensOnReq = TRUE;
	AdatReg->AdatState = ADAT_STS_INIT;	/* �X�e�[�^�X������	*/
	AdatV->MinDistance = AdatPcalMinDistance( AdatP,
											  AdatP->MencV->AxisMotType,
	                                          AdatP->MencV->LowSpdMotor );
	/* ��3����؂�̂Ă鏈����ǉ� */
	AdatV->DefaultDistance = AdatPcalDefautDistance( AdatP,
													 AdatP->MencV->AxisMotType,
	                                                 AdatP->MencV->LowSpdMotor ) / 1000 * 1000 ;

	if( (1000*MlibAbs32( AdatReg->AdatMovep.Long )) < AdatV->MinDistance )
	{
		if(AdatReg->AdatMovep.Long < 0)
		{
			AdatV->MoveDistance = -AdatV->DefaultDistance;
		}
		else if(AdatReg->AdatMovep.Long >= 0)
		{
			AdatV->MoveDistance = AdatV->DefaultDistance;
		}
		AdatReg->AdatMovep.Long = AdatV->MoveDistance / 1000;
	}
	else
	{
		AdatV->MoveDistance = 1000 * AdatReg->AdatMovep.Long;
	}

	AdatScanCRequest( ADAT_NO_OPE, AdatV );

	/* �����m�b�`�ݒ�۔��f	*/
	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) == 0x0000 )
	{
		AdatV->f.UsedAllNtFil = TRUE;
	}
	else
	{
		AdatV->f.UsedAllNtFil = FALSE;
	}
	/* ����A�^�ݒ�۔��f		*/
	if( ( ((Prm->avibsw >> 4) & 0x0F)  == FALSE) 	/* ������������						*/
		|| ((Prm->avibsw & 0x0F) == TRUE) )			/* �܂��́AA�^���ɗL��				*/
	{
		AdatV->f.UsedAResCtrl = TRUE;				/* �ݒ�ς� 						*/
	}
	else
	{
		AdatV->f.UsedAResCtrl = FALSE; 				/* ���ݒ� 							*/
	}

	AdatV->ModeSetPtr = AdatV->ModeSetMin;

	/* ���U���o	*/
//	data = 0x0000FFF0 & Prm->vibsens;//<S09E>
	data = 0x0000FFF0 & Prm->vibsel;
	TunePrmWriteRam( &AccessPrm, &pndef_vibsel, (LONG)data, pndef_vibsel.DataLength );

	/* ��~���g���N�n�C�p�X�t�B���^���g��	*/
	AdatV->KTrqHpFil = MlibPcalKf1gain( STPTRQFIL, (KPI_SCCYCLEMS*1000), 1 );

	if(MFControl->conf.MFCModelSetting <= ADATMFC_ONE_MASS)
	{	/* MFC���� or ����MFC	*/
		/* ���f���Ǐ]���䖳��	*/
		data = 0x0000FFF0 & Prm->mdlsw;
		TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)data, pndef_mdlsw.DataLength );
		/* �O���I�u�U�[�o����	*/
		data = 0x00000FFF & Prm->tfuncsw;
		TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)data, pndef_tfuncsw.DataLength );
	}
	/* HPF��ݒ肵�A����g�U������������ */
	DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
	                         	 DETVIBLP, 0, AdatP->UniPrms->Bprm->SvCycleNs );
	/*----------------------------------------------------------------------------------*/
	/*	�c���U�����g�����o���x�̏�����													*/
	/*----------------------------------------------------------------------------------*/
	data = Prm->remdetw * ADAT_LFVIBDET_LVL / 100;
	if( (LONG)data > pndef_remdetw.UpperLimit )
	{
		data = pndef_remdetw.UpperLimit;
	}
	TunePrmWriteRam( &AccessPrm, &pndef_remdetw, (LONG)data, pndef_remdetw.DataLength );

	/*----------------------------------------------------------------------------------*/
	/*	�Q�C���̏�����																	*/
	/*----------------------------------------------------------------------------------*/
	TuneFilGainCalbyFilType( AdatP->ANotch, (UCHAR)AdatReg->AdatFilType );
	AdatV->Jrate = Prm->jrate;
	AdatV->f.OrgIPSpdControl = (AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl)? TRUE : FALSE;
	return rc;
}

/****************************************************************************
	�ŏ��ړ����� [1�w�ߒP��]
****************************************************************************/
static LONG	AdatPcalMinDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor )
{
	BPRMDAT *Bprm;
	POS_MNG_HNDL *PosManager;
	LONG sx;
	LONG kx;
	LONG lwork;

	Bprm = AdatP->UniPrms->Bprm;
	PosManager = &AdatP->BaseCtrlData->PosManager;

	lwork = (LowSpdMotor)? (2*10) : (2*1);

	if( AxisMotType == MOTTYPE_LINEAR )
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, MINIMUM_LINEAR_MM, 1, &sx, 0 );
	}
	else
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, 1, lwork, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, PosManager->conf.Egear.a, PosManager->conf.Egear.b, &sx, -30 );
	return ( kx );

}

/****************************************************************************
	�����ړ������v�Z [1�w�ߒP��]
****************************************************************************/
LONG AdatPcalDefautDistance( ADATP *AdatP, LONG AxisMotType, BOOL LowSpdMotor ) /* <S145> */
{
	BPRMDAT *Bprm;
	POS_MNG_HNDL *PosManager;
	LONG	sx;
	LONG	kx;
	LONG	lwork;

	Bprm = AdatP->UniPrms->Bprm;
	PosManager = &AdatP->BaseCtrlData->PosManager;

	lwork = (LowSpdMotor)? (10*10) : (10*1);


	if( AxisMotType == MOTTYPE_LINEAR )
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, DEFAULT_STROKE_LINEAR, 10, &sx, 0 );
	}
	else
	{
		kx = MlibScalKxgain( Bprm->XfbPulse1, DEFAULT_STROKE_ROTARY, lwork, &sx, 0 );
	}
	kx = MlibPcalKxgain( kx, PosManager->conf.Egear.a, PosManager->conf.Egear.b, &sx, -30 );
	if( kx > ADAT_DIST_MAX )
	{
		kx = ADAT_DIST_MAX;
	}
	if( kx < -ADAT_DIST_MAX )
	{
		kx = -ADAT_DIST_MAX;
	}
	return ( kx );
}

/****************************************************************************
	�A�h�o���X�h�I�[�g�`���[�j���O �X�L����C�T���v��(Round)

	�T�v:	�X�L����C�ւ̃`���[�j���O�v���̃R�}���h�ƃt���O��
			Round�ɂăZ�b�g����B
****************************************************************************/
static void AdatScanCRequest( USHORT Request, ADATV *AdatV )
{
	AdatV->ScanCReqCmd = Request;/* �b��I�ɃZ�b�g */
	AdatV->ScanCStat = ADAT_SEAR_CONTINUE;
	AdatV->ScanCReqFlg = TRUE;
}


/****************************************************************************
	�A�h�o���X�h�I�[�g�`���[�j���O �X�L����C�T����t(ScanC)

	�T�v:	�X�L����C�̃`���[�j���O�v���̃R�}���h�ƃt���O��
			�X�L����C�ɂĎ󂯎��B
****************************************************************************/
static void AdatScanCSetReq( ADATV *AdatV )
{
	if(	AdatV->ScanCReqFlg == TRUE)
	{
		AdatV->ScanCRequest = AdatV->ScanCReqCmd;	/* ScanC �����v���Z�b�g		*/
		AdatV->ScanCStat    = ADAT_NO_OPE;			/* �G���[�X�e�[�^�X�N���A	*/
		AdatV->ScanCReqFlg = FALSE;
	}
}

/****************************************************************************
	���x���[�v�Q�C���ύX����

	�T�v:	���x���[�v�̊e�Q�C����v���ɉ����ĕύX����B
****************************************************************************/
static SHORT	TuneAdatSetSpdCtrlGain( LONG mode, ADATP *AdatP)
{

	PRMDATA   *Prm;
	REG_ACC_T AccessPrm;
	USHORT	kvdata;
	USHORT	data;
	SHORT	rc;
	SHORT	NotchState;

	rc  = ADAT_OK;
	Prm = AdatP->UniPrms->Prm;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( mode )
	{
		case GAIN_UP:
			kvdata = Prm->loophz + KVUP_UNIT;		/* 2.0[Hz] ����					*/
			break;

		case GAIN_DW:
			kvdata = ADAT_GRAT_94( Prm->loophz );	/* 15/16 = 0.9375 6.25%����		*/
			break;

		case GAIN_CAL:
		default:
			kvdata = Prm->loophz;					/* ����̂܂�					*/
			break;
	}

	if( kvdata < KV_UNLMT )
	{
		kvdata = KV_UNLMT;							/* �����l�����͉����l�Ƃ���		*/
		rc = ADAT_NG;								/* �ُ�I���Ƃ���				*/
	}
	else if( kvdata > Prm->adatgmax )
	{
		kvdata = Prm->adatgmax;						/* ����l���͏���l�Ƃ���		*/
	}

	/*------------------------------------------------------------------------------*/
	/*	���x���[�v�Q�C��(Kv)														*/
	/*------------------------------------------------------------------------------*/
	TunePrmWriteRam( &AccessPrm, &pndef_loophz, (LONG)kvdata, pndef_loophz.DataLength );
	/*------------------------------------------------------------------------------*/
	/*	���x���[�v�ϕ����萔(Ti)													*/
	/*------------------------------------------------------------------------------*/
	data = TuneKvFuncTi( kvdata, AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl );
	TunePrmWriteRam( &AccessPrm, &pndef_pitime, (LONG)data, pndef_pitime.DataLength );
	/*------------------------------------------------------------------------------*/
	/*	�O���I�u�U�[�o�֘A�p�����[�^												*/
	/*------------------------------------------------------------------------------*/
	if( AdatP->BaseCtrlData->DobsCtrl.var.DobsAct == TRUE )
	{ /* �O���I�u�U�[�o�L�� */
		data = TuneKvFuncDobsgn( kvdata );
		TunePrmWriteRam( &AccessPrm, &pndef_dobgn, (LONG)data, pndef_dobgn.DataLength );
	}
	/*------------------------------------------------------------------------------*/
	/*	�m�b�`�t�B���^�֘A �� Tf �p�����[�^											*/
	/*------------------------------------------------------------------------------*/
	NotchState = TuneNotchQ( AdatP->ANotch, AdatP->RegManager->hPnReg,
	                         	 	 	 	 	 	 kvdata, Prm->jrate, 1, Prm );
	if( (NotchState < 0) && (NotchState != -3) )
	{	/* �m�b�`�t�B���^�ݒ�ُ�iQ�l������l�ɒB�����j*/
		rc = ADAT_NTCHNG;		/* �m�b�`�v�Z�ُ�		*/
	}
	return rc;
}

/****************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O�C�i�[�V���ݒ菈��

	�T�v:	�C�i�[�V�����菈�����̂́u�I�[�g�`���[�j���O�@�\�v�𗘗p����B
			�����ł́A�u�I�����C���I�[�g�`���[�j���O�@�\�v�����s���邽�߂�
			�K�v�ȃf�[�^���Z�b�g����B
			�C�i�[�V����́A���]������l�Ƌt�]������l�̕��ϒl�Ƃ���B
			�������A���]�^�t�]�̓���l�̍���31.25%�𒴂���ꍇ�́A�G���[�Ƃ���B
****************************************************************************/
static PRM_RSLT	AdatJratSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA *Prm;
	REG_ACC_T AccessPrm;
	PRM_RSLT rc;
	ULONG data;
	BOOL TrqClamp;
	TUNELESS_CFGPRM TuneLessCfgPrm;

	rc  = PRM_RSLT_SUCCESS;
	Prm = AdatP->UniPrms->Prm;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	TrqClamp = (AdatP->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO & 1);
	if( AdatV->f.RefInType == FALSE)
	{	/* J���莞�̃��~�b�g�`�F�b�N�͓����w��AAT�̂� */
		if( TrqClamp == TRUE )
		{
			AdatV->JratSetPtr = 0;
			AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
			AdatV->ErrState = ADAT_J_ERR;
			AdatV->JratErr  = ADAT_JRAT_ERR4;
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	�������X������������																		*/
	/*		�C�i�[�V�����肠��̏ꍇ�͒������X�𖳌��Ƃ���AAT�����s									*/
	/*----------------------------------------------------------------------------------------------*/
	if( AdatP->BaseCtrlData->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{	/* �������X�L��	*/
		if(AdatReg->AdatMode == FALSE)								/* �C�i�[�V�����肠��			*/
		{
			/* RAM�p�����[�^�@�\�X�C�b�`���� */
			data = 0x0000FFF0 & Prm->DatLevel;
			TunePrmWriteRam( &AccessPrm, &pndef_DatLevel, (LONG)data, pndef_DatLevel.DataLength );

			/* �������X�@�\�I���X�C�b�` */
			TuneLessCfgPrm.gnmode	= Prm->gnmode;
			TuneLessCfgPrm.mfctype	= Prm->mfctype;
			TuneLessCfgPrm.avibsw	= Prm->avibsw;
			TuneLessCfgPrm.DatLevel	= Prm->DatLevel;
			TuneLessCfgPrm.tfuncsw 	= Prm->tfuncsw;
			TuneLessCfgPrm.flg_wf 	= Prm->MencP.flg_wf;
			TuneLessCalculateInitPrm(AdatP->BaseCtrlData,
			                         AdatP->UniPrms->Bprm,
			                         AdatP->MencV,
									 AdatP->SvAsicRegs,
			                         &TuneLessCfgPrm);

			/* �U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g */
			DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBSGN, 0,
			                         	 DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs);

#if 0 /* 2012.08.31 Y.Oka �����[�^���x���o�ړ����ό��󖢑Ή��� */
			/* ���[�^���x���o�p�ړ����ώ��萔���f�t�H���g�ɖ߂�		*/
			BpiChangeSpdDetMaFil( AdatP->MotSpdMafil, -1,
						(USHORT*)&(AdatP->SvAsicRegs->AsicMicroReg->MREG_SPDMANUMBITI) );
#endif
		}
		else
		{
			AdatV->ErrState = ADAT_MOSESETERR;						/* ���[�h�ݒ�G���[				*/
		}
	}

	switch ( FunExe->CmdCode )
	{
		case FCMD_SV :
			if( AdatV->f.RefInType == FALSE )
			{
				if( AdatP->AlmManager->Status.AlmFlag || AdatP->SeqCtrlOut->HwbbSts ||
					!AdatP->SeqCtrlOut->MainPowerOn || AdatP->SeqCtrlOut->OverTrvlSts )
				{
					rc = PRM_RSLT_CONDITION_ERR;
				}
				else
				{
					AdatP->FnCmnCtrl->FnSvonReq = ~AdatP->FnCmnCtrl->FnSvonReq;
				}
			}
			return rc;

		default :
			break;
	}

	switch ( AdatV->JratSetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�ړ��������͑҂�																	*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			AdatReg->AdatState = ADAT_STS_DRVWAIT;	/* �����ݒ芮�� ���͑҂� */
			AdatP->FnCmnCtrl->State.TuningRun = FALSE;

			/* ���[�h�X�C�b�`���� */
			data  = Prm->gnmode;
			data &= 0x0000FFF0;
			data |= 0x00000004;
			TunePrmWriteRam( &AccessPrm, &pndef_gnmode, (LONG)data, pndef_gnmode.DataLength );

			/*----------------------------------------------------------------------------------*/
			/*	�`�^���U��������																*/
			/*----------------------------------------------------------------------------------*/
			if( (AdatV->f.RefInType == FALSE) && (AdatReg->AdatMode == FALSE) )	/* �C�i�[�V�����肠��	*/
			{
				/*�`�^���U���� */
				data = 0x0000FFF0 & Prm->avibsw;
				TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (LONG)data, pndef_avibsw.DataLength );
				AdatV->f.UsedAResCtrl = TRUE;	/* �`�^���U�����ݒ�ς݂ɂ��Ă`�^���U�̎����ݒ�@�\��OFF����	*/
			}

			switch ( FunExe->CmdCode )
			{
				case FCMD_UP :
					/* �T�[�{�I���`�F�b�N & �ړ������`�F�b�N */
					if( ((AdatV->f.RefInType == FALSE) && (AdatP->SeqCtrlOut->BaseEnableReq == TRUE) && (AdatV->MoveDistance > 0)) )
					{
						AdatV->f.DrvErrDetect = TRUE;
						AdatP->FnCmnCtrl->State.TuningRun = TRUE;
						AdatV->JratSetPtr = 1;	/* �T�[�{�I�����Ă��āA���ړ������ƃL�[���͂�����	*/
												/* �ꍇ�Ɍ���A�`���[�j���O�J�n��					*/
					}
					else
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					break;

				case FCMD_DOWN :
					if( ((AdatV->f.RefInType == FALSE)
					   && (AdatP->SeqCtrlOut->BaseEnableReq == TRUE)
					   && (AdatV->MoveDistance < 0)) )
					{
						AdatV->f.DrvErrDetect = TRUE;
						AdatP->FnCmnCtrl->State.TuningRun = TRUE;
						AdatV->JratSetPtr = 1;
					}
					else
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					break;

				case FCMD_EXEC :
					if( (AdatV->f.RefInType == TRUE) && (AdatRefInExeModeExeCheck(AdatP)) )
					{
						AdatV->f.DrvErrDetect = TRUE;
						AdatP->FnCmnCtrl->State.TuningRun = TRUE;
						AdatV->JratSetPtr = 1;
					}
					else
					{
						rc = PRM_RSLT_CONDITION_ERR;
					}
					break;

				case FCMD_NONE :
					break;

				default:
					rc = PRM_RSLT_CONDITION_ERR;
					break;
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�^�]�J�n																			*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			TunePrmWriteRam( &AccessPrm, &pndef_ffgn, JRATE_VFFGN, pndef_ffgn.DataLength );
			AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl = FALSE;
			BpiPcmdFilSwitch( &AdatP->BaseCtrlData->PosManager, FALSE );  	/* �ʒu�w�߃t�B���^��~   */
			MfcSetMfcCtrlSwitch( &AdatP->BaseCtrlData->MFControl, FALSE );	/* MFC�������� 		 	  */
			AdatP->BaseCtrlData->DobsCtrl.var.DobsAct = 0;					/* �O���I�u�U�[�o�������� */
			AdatV->LastEstimatJrat = -1;									/* ����l�O��l������	  */

			if( (AdatV->f.RefInType == FALSE) && (AdatReg->AdatMode == FALSE) )
			{	/* �C�i�[�V������L�� 	*/
				AdatReg->AdatState = ADAT_STS_JRUN;				/* �C�i�[�V��������s��			*/
				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN5;	/* �v���O����JOG�^�]�p�^�[��5	*/
				AdatV->JratSetPtr = 2;							/* ���蒆������					*/
				AdatV->Jrate = Prm->Pn324;
				/* Pn103 */
				TunePrmWriteRam( &AccessPrm, &pndef_jrate, AdatV->Jrate, pndef_jrate.DataLength );
			}
			else
			{	/* �C�i�[�V�����薳��	*/
				CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
				AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
				AdatV->JratSetPtr = 0;							/* �����ݒ��Ԃ�				*/
				AdatUndoRamGainforJrat( AdatV, AdatP );
				FunExe->State = ADAT_SEQ_NORMVIB;				/* �ʏ�U�����x���T����			*/
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�^�]�J�n�҂���																		*/
		/*--------------------------------------------------------------------------------------*/
		case 2:
			AdatScanCRequest( ADAT_SEQ_SEARCHOPTKV, AdatV );			   /* �X�L����C�����v�� */
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				CpiInitJatOffLine( &AdatP->BaseCtrlData->JatHdl,
									AdatP->UniPrms,
									AdatP->RegManager,
									AdatP->BaseCtrlData->GainChange,
									1,
									AdatP->BaseCtrlData->PJogHdl.PJogV.Cnst.Distance,
									AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm,
									AdatP->UniPrms->Bprm->SvCycleNs);

				if( AdatV->PrgJogStat == ADAT_PJOG_EXEC )
				{	/* �^�]�J�n	*/
					AdatV->JratSetPtr = 3;
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
				}
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�C�i�[�V�����蒆																	*/
		/*--------------------------------------------------------------------------------------*/
		case 3:
		/*--------------------------------------------------------------------------------------*/
		/*	����G���[����																		*/
		/*--------------------------------------------------------------------------------------*/
			if( CpiGetJatExeErrStatus( &AdatP->BaseCtrlData->JatHdl ) == NG )
			{	/* �C�i�[�V�����蒆���s�G���[	*/
				AdatV->JratSetPtr = 0;
				AdatV->ErrState = ADAT_J_ERR;
				AdatV->JratErr = ADAT_JRAT_ERR5;
			}
			if( CpiGetJatErrStatus( &AdatP->BaseCtrlData->JatHdl ) == NG )
			{
				/*------------------------------------------------------------------------------*/
				/*	�C�i�[�V�������l�␳														*/
				/*		��C�i�[�V������Kp/Kv�o�����X�ɂ��U���΍�B���菉��̂ݍs��			*/
				/*------------------------------------------------------------------------------*/
				if( ( KlibGetLongTimerMs( AdatV->JratVibWaitCntr ) >= ADATVIBWAITTM3 )	/* �U�������҂�100ms	*/
					 || (AdatV->JratChangeTimes == 0) )									/* �G���[�Ȃ�			*/
				{
					if( (CpiGetJatExeErrStatus( &AdatP->BaseCtrlData->JatHdl ) != NG) && (AdatV->JratDrvCnt == 0 )
							&& (AdatV->JratChangeTimes < ADAT_JRAT_CHGCNT) )
					{
						AdatV->f.VibDetectinCycle = TRUE;	/* �U���X�e�[�^�XON */
						AdatV->Jrate = AdatV->Jrate + ADAT_JRAT_CHG_RATE;
						TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)AdatV->Jrate, pndef_jrate.DataLength );
						AdatV->JratChangeTimes++;
						KlibRstLongTimer( &AdatV->JratVibWaitCntr );
					}
					else
					{
						AdatV->JratSetPtr = 0;
						AdatV->ErrState = ADAT_J_ERR;
						AdatV->JratErr = ADAT_JRAT_ERR3;
					}
				}
			}

		/*--------------------------------------------------------------------------------------*/
		/*	�U�����ʎ����m�b�`																	*/
		/*--------------------------------------------------------------------------------------*/
			if( (status == ADAT_SEAR_VIBEND) || (status == ADAT_SEAR_STOPVIBEND) )
			{	/* �U�����o�I��		*/
				TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP );
				if( ( AdatV->f.UsedAllNtFil == FALSE ) || ( AdatV->f.UsedAResCtrl == FALSE ) )
				{
					/* �X�L����C�����v�� */
					AdatV->NextSeqPtr = ADAT_SEQ_JRATSET;
					FunExe->State = ADAT_SEQ_NOTCHSET;
				}
				AdatScanCRequest( ADAT_NO_OPE, AdatV );			/* �X�L����C�����v��			*/
				AdatV->f.VibDetectinCycle = TRUE;				/* �U���X�e�[�^�XON				*/
			}
		/*--------------------------------------------------------------------------------------*/
		/*	�C�i�[�V�����芮�����f																*/
		/*--------------------------------------------------------------------------------------*/
			if(( AdatV->PrgJogStat == ADAT_PJOG_FIN )
					||  ( AdatV->PrgJogStat == ADAT_PJOG_NOP ))	/* �m�b�`�ݒ肩�畜�A��NOP�ɂȂ��Ă���\�����L��	*/
			{
				if( CpiGetJatCompStatus( &AdatP->BaseCtrlData->JatHdl ) == OK )
				{
					KlibRstLongTimer( &AdatV->JratVibWaitCntr2 );
					if( AdatV->f.VibDetectinCycle == TRUE )
					{	/* ����^�]���ɐU�����o����	*/
						AdatV->f.VibDetectinCycle  = FALSE;	/* �U���X�e�[�^�X�N���A				*/
						AdatV->JratSetPtr = 8;				/* �C�i�[�V���ē���J�n�҂��҂���	*/
						CpiInitJatOffLine( &AdatP->BaseCtrlData->JatHdl,
											AdatP->UniPrms,
											AdatP->RegManager,
											AdatP->BaseCtrlData->GainChange,
											0,
											AdatP->BaseCtrlData->PJogHdl.PJogV.Cnst.Distance,
											AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm,
											AdatP->UniPrms->Bprm->SvCycleNs);

					}
					else
					{	/* �U�����o�Ȃ�					*/
						/* ����l���p�����[�^�ɉ����f	*/
						CpiSetEstimateJrat( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
						AdatV->JratSetPtr = 4;					/* �p�����[�^�����ݑ҂���		*/
					}
				}
				else
				{
					AdatV->JratSetPtr = 0;
					AdatV->ErrState = ADAT_J_ERR;
					AdatV->JratErr = ADAT_JRAT_ERR1;
				}
			}
			break;

		case 4:
		/*--------------------------------------------------------------------------------------*/
		/*	�C�i�[�V�����芮��																	*/
		/*--------------------------------------------------------------------------------------*/
			if( ( AdatV->PrgJogStat == ADAT_PJOG_FIN )
					||  ( AdatV->PrgJogStat == ADAT_PJOG_NOP ))	/* �m�b�`�ݒ肩�畜�A��NOP�ɂȂ��Ă���\��������	*/
			{
				if( (AdatV->LastEstimatJrat <= ADAT_ERR_ULMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat ))
							&& (AdatV->LastEstimatJrat >= ADAT_ERR_LLMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat )) )
				{	/* ���t����3.125%�ȓ� �� ����		*/
					CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
					AdatV->JratSetPtr = 5;					/* �p�����[�^�����ݑ҂���			*/
				}
				else if( AdatV->JratDrvCnt > ADAT_JDRV_MAXCNT )
				{
					if( (AdatV->LastEstimatJrat <= ADAT_ERR2_ULMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat ))
							&& (AdatV->LastEstimatJrat >= ADAT_ERR2_LLMT( AdatP->BaseCtrlData->JatHdl.EstimatJrat )) )
					{	/* ���t����31.25%�ȓ��� ����*/
						CpiEndJatOffLine( &AdatP->BaseCtrlData->JatHdl, &AccessPrm );
						AdatV->JratSetPtr = 5;				/* �p�����[�^�����ݑ҂���			*/
					}
					else
					{	/* ���t�������e�l�𒴂��Ă���	*/
						AdatV->ErrState = ADAT_J_ERR;		/* �C�i�[�V������G���[				*/
						AdatV->JratSetPtr = 0;				/* �����ݒ��Ԃ�					*/
						AdatV->JratErr = ADAT_JRAT_ERR2;
					}
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
				}
				else
				{
					AdatV->JratSetPtr = 8;					/* �C�i�[�V���ē���J�n�҂��҂���	*/
					CpiInitJatOffLine( &AdatP->BaseCtrlData->JatHdl,
										AdatP->UniPrms,
										AdatP->RegManager,
										AdatP->BaseCtrlData->GainChange,
										0,
										AdatP->BaseCtrlData->PJogHdl.PJogV.Cnst.Distance,
										AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm,
										AdatP->UniPrms->Bprm->SvCycleNs);

				}
				AdatV->LastEstimatJrat = AdatP->BaseCtrlData->JatHdl.EstimatJrat;
				AdatV->JratDrvCnt++;						/* ����񐔃J�E���^�C���N�������g	*/
			}
			AdatV->Jrate = AdatP->BaseCtrlData->JatHdl.EstimatJrat;
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�p�����[�^�v�Z �� �ʏ�U�����x���T���҂�											*/
		/*--------------------------------------------------------------------------------------*/
		case 5:
			AdatReg->AdatState = ADAT_STS_JEND;					/* �C�i�[�V�����芮��			*/
			if( AdatV->PrgJogStat == ADAT_PJOG_NOP )
			{
				switch ( FunExe->CmdCode )
				{
					case FCMD_UP :
						if( AdatV->MoveDistance > 0 )
						{	/* �ړ������ƃL�[���͂������ꍇ�Ɍ���A����J�n��	*/
							AdatV->JratSetPtr = 6;				/* �p�����[�^�������ݏ�����		*/
						}
						break;

					case FCMD_DOWN :
						if( AdatV->MoveDistance < 0 )
						{
							AdatV->JratSetPtr = 6;				/* �p�����[�^�������ݏ�����		*/
						}
						break;

					case FCMD_EWRT: /* <S195> */
						AccessPrm.EepWrite = TRUE;
						TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)AdatV->Jrate, pndef_jrate.DataLength );
						break;

					case FCMD_NONE :
						break;

					default:
						rc = PRM_RSLT_CONDITION_ERR;
						break;
				}
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�p�����[�^�v�Z �� EEPROM��������													*/
		/*--------------------------------------------------------------------------------------*/
		case 6:
			AdatV->JratSetPtr = 0;								/* �����ݒ��Ԃ�				*/
			/* Pn103 */
			AccessPrm.EepWrite = TRUE;							/* <S195> */
			TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)AdatV->Jrate, pndef_jrate.DataLength );
			AdatUndoRamGainforJrat( AdatV, AdatP );
			FunExe->State = ADAT_SEQ_NORMVIB;					/* �ʏ�U�����x���T����			*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�^�]�J�n�҂���																		*/
		/*--------------------------------------------------------------------------------------*/
		case 7:
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				if( AdatV->PrgJogStat == ADAT_PJOG_EXEC )
				{	/* �^�]�J�n	*/
					AdatV->JratSetPtr = 3;
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
					AdatScanCRequest( ADAT_SEQ_SEARCHOPTKV, AdatV );	/* �X�L����C�U�����o�����v��	*/
				}
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�E�F�C�g																			*/
		/*--------------------------------------------------------------------------------------*/
		case 8:/* �C�i�[�V������J�n�҂�500ms */
			if( KlibGetLongTimerMs( AdatV->JratVibWaitCntr2 ) >= ADATVIBWAITTM1 )
			{
				AdatV->JratSetPtr = 7;							/* �ēx�`���[�j���O�J�n			*/
				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN5;	/* �v���O����JOG�^�]�p�^�[��5	*/
			}
			break;
		default:
			break;
	}
	return rc;
}

/****************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O�ʏ�U�����x���T��

	�T�v:	�ʏ�^�]���̐U�����x���𑪒肷��B
			�ʏ�U�����x���T�������̓X�L����C�ōs��
			�܂��A��~���g���N�̕��ϒl�𑪒肵�A
			GRVLMT���ȏ�ŏd�͎��Ɣ��肵�A�O���g���N�ɕ��׃g���N�����Z����B
****************************************************************************/
static PRM_RSLT	AdatNormVibDetect( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA *Prm;
	PRM_RSLT rc;
	USHORT ret0;
	BOOL TrqClamp;
	REG_ACC_T AccessPrm;

	Prm  = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	TrqClamp = (AdatP->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO & 1);

	/*	ScanC�����ŃG���[���������ꍇ	*/
	if( (status == ADAT_SEAR_ERR) || (TrqClamp == TRUE) )
	{
		AdatV->VibDetPtr = 0;									/* �����ݒ��					*/
		AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;				/* �v���O����JOG�^�]��~		*/
		AdatV->ErrState = ADAT_NG;								/* �ُ�I��						*/
	}

	switch ( AdatV->VibDetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�����ݒ�																			*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			/*----------------------------------------------------------------------------------*/
			/* �Q�C���̏����ݒ�																	*/
			/*----------------------------------------------------------------------------------*/
			Prm->posgn = Prm->loophz / 2;
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* �ݒ�Kv�Ƃ̃o�����X			*/
			TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)Prm->posgn, pndef_posgn.DataLength );

			AdatV->f.DrvPatternSet = FALSE;
			AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;		/* �v���O����JOG�^�]�p�^�[��1	*/
			AdatV->VibSpeed     = 0;							/* �ʏ�U�����x��������			*/
			AdatV->TrqRefatStop = 0;							/* �ʏ��~���g���N������		*/
			AdatV->VibDetPtr    = 1;							/* �ʏ�U�����x���T����Ԃ�		*/
			RemVibRsetRemVibrationFreq( AdatP->RemVibFreq );	/* �c���U�����g���N���A			*/

			if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
			{
				/* HPF��ݒ肵�A����g�U������������	*/
				DetVibObsCalculateGains( &(AdatP->DetVib->DetVibObs), VIBOBS_KS, DETVIBLP,
				                         DETVIBHP2, AdatP->UniPrms->Bprm->SvCycleNs );
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	���]�^�t�]�����׃g���N����v��														*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			AdatScanCRequest( ADAT_SEQ_NORMVIB, AdatV );		/* �ʏ�U�����x������v��		*/
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				AdatV->VibDetPtr = 5;
			}
			AdatV->f.VibDetectinCycle  = FALSE;					/* �U���X�e�[�^�X�N���A			*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�ړ������҂�																		*/
		/*--------------------------------------------------------------------------------------*/
		case 5:
			/*----------------------------------------------------------------------------------*/
			/*	�U�����ʎ����m�b�`																*/
			/* 		�U�����o������m�b�`�����Ɉڍs�B�m�b�`�ݒ�ς݂̏ꍇ�A						*/
			/*		�U�����o�Ȃ��ɂȂ�܂ŃQ�C����������										*/
			/*----------------------------------------------------------------------------------*/
			if( (status == ADAT_SEAR_VIBEND) || (status == ADAT_SEAR_STOPVIBEND) )
			{	/* �U�����o�I�� */
				if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* kv�Q�C��DOWN		*/
				{
					AdatV->ErrState = ADAT_NG;
					return rc;
				}
				if( ( AdatV->f.UsedAllNtFil == FALSE ) || ( AdatV->f.UsedAResCtrl == FALSE ) )
				{
					AdatV->VibDetPtr  = 0;
					AdatV->NextSeqPtr = ADAT_SEQ_NORMVIB;
					FunExe->State = ADAT_SEQ_NOTCHSET;
				}
				AdatScanCRequest( ADAT_NO_OPE, AdatV );				/* �X�L����C�����v��		*/
				AdatV->f.VibDetectinCycle = TRUE;					/* �U���X�e�[�^�XON			*/
			}
			/* �`���[�j���O�O��MFC(����+�@�䃂�f��)��ݒ肵�Ă��Ȃ����C��ԃV�[�P���X�ړ�		*/
			if( AdatV->f.Used2MassOld == FALSE )
			{
				ret0 = SetMfc2MassSequence( AdatReg, AdatV, AdatP, FunExe, ADAT_SEQ_NORMVIB );
				if( ret0 )
				{
					FunExe->State = ADAT_SEQ_2MASSSET;	 /* ���f���Ǐ](����+�@�䃂�f��)�ݒ菈����*/
					AdatV->NextSeqPtr = ADAT_SEQ_NORMVIB;/* �ݒ�㌳�̏����֖߂邽�߂̃o�b�t�@	 */
					AdatV->f.VibDetectinCycle = TRUE;	 /* �U���X�e�[�^�XON					 */
				}
			}
			if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
			{	/* �v���O����JOG�^�]���� �� ��~��ԂƂ݂Ȃ� */
				if( AdatV->f.VibDetectinCycle == TRUE )
				{	/* ����^�]���ɐU�����o����	*/
					AdatV->f.VibDetectinCycle = FALSE;			/* �U���X�e�[�^�X�N���A			*/
					AdatV->VibDetPtr = 0;						/* �U�����Ă�����ăg���C		*/
				}
				else
				{	/* ����U���Ȃ�	*/
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;
					AdatV->VibDetPtr = 0;						/* �����ݒ��					*/
					FunExe->State = ADAT_SEQ_SEARCHOPTKV;		/* �œK�Q�C���T����				*/
				}
			}
			break;

		default:
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc;
}


/****************************************************************************************************
		MFC(����+�@�䃂�f��)�V�[�P���X�Ɉړ�����
****************************************************************************************************/
static USHORT	SetMfc2MassSequence( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, ULONG ADAT_MFC )
{
	UINT set_mfc_f;
	LONG UnRvibFrequency;

	UnRvibFrequency = RemVibGetRemVibFreq( AdatP->RemVibFreq );

	set_mfc_f = ( UnRvibFrequency >= VIBMFC_DET_FRQ_MIN ) &&	/* ���o���g�� >= �U�����o�����l		*/
				( UnRvibFrequency <= VIBMFC_DET_FRQ_MAX ) &&	/* ���o���g�� <= �U�����o����l		*/
				( AdatV->f.Used2MassMfc == FALSE ) &&			/* MFC(����+�@�䃂�f��)���ݒ�		*/
				((AdatReg->AdatLevel == ADAT_LEVEL_POS)			/* �`���[�j���O���x��=�ʒu���ߐ�p	*/
			  || (AdatReg->AdatLevel == ADAT_LEVEL_POS_ST) );	/* �`���[�j���O���x��=�ʒu����(OV)	*/

		if( set_mfc_f )
		{
			AdatV->Set2MassMfcSequence = (USHORT)ADAT_MFC;	/*	�U���}�������V�[�P���X��ۑ� */

			switch( ADAT_MFC )
			{
				case ADAT_SEQ_NORMVIB:				/* �ʏ�U�����x�����菈��						*/
					AdatV->VibDetPtr = 0;
					break;

#if 0	/* �œKKv�T�������̎��ɂ�MFC(����+�@�䃂�f��)�ݒ�͍s��Ȃ�			*/
				case ADAT_SEQ_SEARCHOPTKV:			/* �œKKv�T������(��ԃV�[�P���X�͏��������Ȃ�)	*/
					break;
#endif
				case ADAT_SEQ_ADJMFC:				/* ���f���Ǐ]��������							*/
					AdatV->MfcTunePtr = ADAT_MFC_INI;
					break;
			}
			return 1;
		}
		return 0;
}

/****************************************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O�œK�Q�C���T�����

	�T�v:	�œK�Q�C���T�����X�L�����b�ɗv������B
			�X�L�����b����A�m�b�`�t�B���^�ݒ�̗v�����󂯂��Ƃ��́A
			����A�m�b�`�t�B���^�ݒ菈����v������B
****************************************************************************************************/
static PRM_RSLT	AdatOptGainSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA *Prm;
	REG_ACC_T AccessPrm;
	PRM_RSLT rc;
	ULONG data;
	UCHAR tmpGainRatio;

	Prm  = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;											/* �U��������Q�C������ �����l=80% 			*/

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;


	/*	ScanC�����ŃG���[���������ꍇ	*/
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->KvTunePtr = ADAT_OPTGN_INI;						/* �����ݒ��								*/
		AdatV->ErrState  = ADAT_NG;								/* �ُ�I��									*/
	}

	switch ( AdatV->KvTunePtr )
	{
		/*--------------------------------------------------------------------------------------------------*/
		/*	�����ݒ�																						*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_INI:
			/* �T������ő�Q�C����ݒ� */
			AdatV->MaxKv = Prm->adatgmax;						/* �T���ő�Q�C�����ő�l�Q�C���ɐݒ�		*/
			/*----------------------------------------------------------------------------------------------*/
			/*	�O���I�u�U�[�o�ݒ�																			*/
			/*		�t�@�[�X�g���[�h�́A�������ɊO���I�u�U�[�o�L���Ƃ���(�ݒ�̓����p���o�����X)�B			*/
			/*		�ʒu���ߐ�p���[�h�́A�������ɊO���I�u�U�[�o�AMFC��L���Ƃ���(�ݒ�̓����p���o�����X)�B	*/
			/*		���̑��̃��[�h�́A�@�\�X�C�b�`ON�Ń����p���o�����X�̊O���I�u�U�[�o�L���Ƃ���B			*/
			/*----------------------------------------------------------------------------------------------*/
			switch ( AdatReg->AdatLevel )
			{	/* �`���[�j���O���x�� */
				case ADAT_LEVEL_POS:							/* �ʒu���ߐ�p								*/
				case ADAT_LEVEL_POS_ST:							/* �ʒu���ߐ�p(�I�[�o�[�V���[�g�d��)		*/
//					TuneDobsActive( Prm );						/* �O���I�u�U�[�o�����ݒ�					*/
					TuneDobsActive( Prm, AccessPrm.hRegCfg );	/* �O���I�u�U�[�o�����ݒ�					*///<S09E>
					break;

				default:
					if( AdatP->BaseCtrlData->MFControl.conf.MFCModelSetting != 0 )
					{	/* AAT���s�J�n�O���f���Ǐ]����L��	*/
						/* ���f���Ǐ]���䖳��				*/
						data = 0x0000FF00 & Prm->mdlsw;
						TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, data, pndef_mdlsw.DataLength );
					}

					if( AdatP->BaseCtrlData->DobsCtrl.var.DobsAct == TRUE )
					{	/* �O���I�u�U�[�o�L�� */
//						TuneDobsActive( Prm );		/* �O���I�u�U�[�o�����ݒ� */
						TuneDobsActive( Prm, AccessPrm.hRegCfg );	/* �O���I�u�U�[�o�����ݒ� *///<S09E>
					}
					break;
			}
			/* �Q�C���ݒ� */
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* �Q�C���v�Z								*/
			/* �U�����x���ݒ� 						*/
			/* �Q�C���T�����͒ʏ�U�����x����2�{	*/
			AdatP->DetVib->conf.VibCheckLevel = AdatV->VibSpeed * RESGMAX / 100;
			AdatV->f.VibDetectinCycle = TRUE;					/* �U���X�e�[�^�X�������B�����l�͐U������	*/
			AdatV->f.DrvPatternSet  = FALSE;
			AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;		/* �v���O����JOG�^�]�p�^�[���P				*/
			AdatV->KvTunePtr = ADAT_OPTGN_INIWAIT;				/* �Q�C���ύX�ɂ��U�������҂�				*/
			AdatV->f.KvTuneWait = FALSE;						/* �`���[�j���O��~�t���O�n�e�e				*/
			KlibRstLongTimer( &AdatV->KvTuneWaitCntr );			/* ���[�^��~�҂��^�C�}������				*/

			if( AdatP->MencV->AxisMotType == MOTTYPE_LINEAR )
			{
				/* HPF��ݒ肵�A����g�U������������	*/
				DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
				                         DETVIBLP, DETVIBHP3, AdatP->UniPrms->Bprm->SvCycleNs );
			}
			break;

		/*--------------------------------------------------------------------------------------------------*/
		/*	�Q�C���ύX�ɂ��U�������҂�																	*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_INIWAIT:
			if( AdatV->f.DrvPatternSet == TRUE )
			{
				if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
				{	/* �v���O����JOG�^�]���� �� �P�����҂�	*/
					AdatV->f.DrvPatternSet  = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;		/* �v���O����JOG�^�]�p�^�[��1		*/
					AdatV->KvTunePtr        = ADAT_OPTGN_REQ;			 /* �œK�Q�C���T���v����Ԃ�		*/
				}
			}
			break;

		/*--------------------------------------------------------------------------------------------------*/
		/*	�œK�Q�C���T���v��																				*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_REQ:
			AdatScanCRequest( ADAT_SEQ_SEARCHOPTKV, AdatV );			/* �X�L����C�����v��				*/
			AdatV->KvTunePtr = ADAT_OPTGN_SEARCH;						/* �œK�Q�C���T����Ԃ�				*/
			break;

		/*--------------------------------------------------------------------------------------------------*/
		/*	�œK�Q�C���T�����																				*/
		/*--------------------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_SEARCH:
			if( status == ADAT_SEAR_NORMALEND )
			{	/* ����I���B�T���ő�Q�C���ł��U�����Ȃ������Ƃ�	*/
				AdatV->f.DrvPatternSet  = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;		/* �v���O����JOG�^�]�p�^�[��3			*/
				AdatV->KvTunePtr        = ADAT_OPTGN_CHKKV_REQ;		/* ���݂��œK�Q�C���Ƃ��Ĉ��S�m�F�v��	*/
				Prm->loophz = AdatV->MaxKv;
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* �Q�C���v�Z							*/
			}
			else if( (status == ADAT_SEAR_VIBEND) || (status == ADAT_SEAR_STOPVIBEND) )
			{		/* �U�����o or ��~���U�����o�I��	*/
				TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP );			/* Kv�Q�C��DOWN							*/
				if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
				{
					/* ���Ƀm�b�`�t�B���^�ݒ�ς�	*/
					AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, 50 );		/* �U���㏈�� 			*/
					KlibRstLongTimer( &AdatV->KvTuneCntr );
					AdatV->f.DrvPatternSet  = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* �v���O����JOG�^�]�p�^�[��3 			*/
					AdatV->KvTunePtr     = ADAT_OPTGN_WAIT;
					AdatV->NextSubSeqPtr = ADAT_OPTGN_CHKKV_REQ;
				}
				else
				{	/* �m�b�`�t�B���^�ݒ菈���v��	*/
					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;	/* �v���O����JOG�^�]�p�^�[��1			*/
					AdatV->KvTunePtr = ADAT_OPTGN_REQ;
					AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKV;
					FunExe->State = ADAT_SEQ_NOTCHSET;
				}
			}
			else if( status == ADAT_SEAR_NOVIBDETECT )
			{
				/*------------------------------------------------------------------------------------------*/
				/*	�O���w�߂ł́CKv�T�����Ɏw�߂���~����ꍇ�����肦��									*/
				/*	���̍ہCKv���ǂ�ǂ�オ��̂�h�����߁C�^�]�J�n�O(���[�^��~��)�́C					*/
				/*	�V�[�P���X���X�V�����CKv UP���s��Ȃ��悤�ɂ���											*/
				/*	�{�����́C�O���w�ߎ��̂ݍs��															*/
				/*------------------------------------------------------------------------------------------*/
				if( AdatV->PrgJogStat != ADAT_PJOG_WAIT )
				{
					if( TuneAdatSetSpdCtrlGain( GAIN_UP, AdatP ) == ADAT_NTCHNG )	/* �Q�C��UP */
					{	/* �m�b�`Q�l������ɒB�����ꍇ�͐���I���Ƃ���	*/
						if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
						{
							AdatV->f.DrvPatternSet = FALSE;
							AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* �v���O����JOG�^�]�p�^�[��3	*/
						}
						AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;	/* ���݂��œK�Q�C���Ƃ��Ĉ��S�m�F�v��	*/
					}
					else
					{
						if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
						{
							AdatV->f.DrvPatternSet = FALSE;
							AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;	/* �v���O����JOG�^�]�p�^�[��1	*/
						}
						AdatV->KvTunePtr = ADAT_OPTGN_REQ;					/* �œK�Q�C���T���v����Ԃ�		*/
					}
					AdatV->f.KvTuneWait = FALSE;							/* �`���[�j���O��~�t���OOFF	*/
					KlibRstLongTimer( &AdatV->KvTuneWaitCntr );				/* ���쒆�̓^�C�}�N���A			*/
				}
				else if( ( KlibGetLongTimerMs( AdatV->KvTuneWaitCntr ) <= ADATSTOPWAITTM )/* ��~�҂�3000ms�ȓ�	*/
				&& (AdatV->f.KvTuneWait == FALSE) )
				{
					if( TuneAdatSetSpdCtrlGain( GAIN_UP, AdatP ) == ADAT_NTCHNG )	/* �Q�C��UP 			*/
					{	/* �m�b�`Q�l������ɒB�����ꍇ�͐���I���Ƃ���	*/
						AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;	/* ���݂��œK�Q�C���Ƃ��Ĉ��S�m�F�v��	*/
					}
					else
					{
						AdatV->KvTunePtr = ADAT_OPTGN_REQ;					/* �œK�Q�C���T���v����Ԃ�		*/
					}
				}
				else
				{	/* ��~�҂����Ԍo�߁��Q�C���`���[�j���O��~	*/
					AdatV->f.KvTuneWait = TRUE;								/* �`���[�j���O��~�t���OON		*/
				}
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�U�������҂����																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_WAIT :
			if( KlibGetLongTimerMs( AdatV->KvTuneCntr ) >= ADATVIBWAITTM1 )/* �U�������҂�500ms	*/
			{
				AdatV->KvTunePtr = AdatV->NextSubSeqPtr;
				Prm->loophz = AdatV->MaxKv;					/* �T���Q�C���ő�l�ɖ߂�			*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );	/* �Q�C���v�Z						*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�w�߉������S�m�F�v��																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_CHKKV_REQ:
			AdatScanCRequest( ADAT_CHKKV, AdatV );			/* �X�L����C�����v��				*/
			AdatV->KvTunePtr = ADAT_OPTGN_CHKKV;			/* �œK�Q�C���T�����2��			*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�œK�Q�C���T�����2�i�U�����o��̊m�F��ԁj											*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_OPTGN_CHKKV:
			switch ( status )
			{
				case ADAT_SEAR_STOPVIBEND :				/* ��~���U�����o�I��					*/
					tmpGainRatio = 50;					/* �U��������Q�C������ �����l=80%		*/
				/* Fall Through */
				case ADAT_SEAR_VIBEND :					/* �U�����o�I��							*/
					AdatV->f.VibDetectinCycle = TRUE;	/* �U���X�e�[�^�XON						*/
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* Kv�Q�C��DOWN */
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}

					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;/* �v���O����JOG�^�]�p�^�[��3	*/

					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* ���Ƀm�b�`�t�B���^�ݒ�ς�	�� �������s	*/
						/* �U���㏈��	*/
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->KvTuneCntr );
						AdatV->KvTunePtr = ADAT_OPTGN_WAIT;
						AdatV->NextSubSeqPtr = ADAT_OPTGN_CHKKV_REQ;
					}
					else
					{	/* �m�b�`�t�B���^�ݒ薢��	*/
						AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;
						AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKV;
						FunExe->State = ADAT_SEQ_NOTCHSET;
					}
					break;

				default :
					break;
			}
			/*----------------------------------------------------------------------------------*/
			/*	�^�]�p�^�[���I��																*/
			/*----------------------------------------------------------------------------------*/
			if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
			{	/* �v���O����JOG�^�]���� */
				/*------------------------------------------------------------------------------*/
				/*	�^�]�X�e�[�^�X�N���A														*/
				/*		��U�N���A���Ȃ��ƁC�U����̍ăg���C���ɁC�^�]�X�e�[�^�X��				*/
				/*		ADAT_PRGJOG_FIN �̂܂܂ƂȂ�C���ۂɉ^�]���Ă��Ȃ��̂ɁC�U�������o		*/
				/*		�ƂȂ�̂�h�~���邽��													*/
				/*------------------------------------------------------------------------------*/
				if( ( AdatV->f.VibDetectinCycle == TRUE )
						|| ( AdatV->f.DrvPatternSet == FALSE ) )/* �w�߃p�^�[��������			*/
				{	/* ����^�]�p�^�[�����ŐU������	*/
					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;/* �v���O����JOG�^�]�p�^�[��3	*/
					AdatV->KvTunePtr = ADAT_OPTGN_CHKKV_REQ;	 /* �ăg���C					*/
				}
				else
				{	/* �U���Ȃ� �� ���S�m�F���� */
					/*--------------------------------------------------------------------------*/
					/*	���f���Ǐ]���䒲���V�[�P���X����										*/
					/*		LEVEL 2�̏ꍇ�́C���f���Ǐ]���䒲�����s��							*/
					/*--------------------------------------------------------------------------*/
					AdatScanCRequest( ADAT_NO_OPE, AdatV );	/* �����I����͑ҋ@��Ԃ�v��		*/
					AdatV->KvTunePtr = ADAT_OPTGN_INI;		/* ����`���[�j���O�V�[�P���X�Z�b�g	*/
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;

					if( AdatV->f.RefInType == TRUE )
					{	/* �w�ߓ��͌^AAT */
						FunExe->State = ADAT_SEQ_KPSET;				/* Kp�ݒ菈�� 				*/
					}
					else
					{
						if( ( AdatReg->AdatLevel == ADAT_LEVEL_POS )
								|| ( AdatReg->AdatLevel == ADAT_LEVEL_POS_ST ) )
						{
							FunExe->State = ADAT_SEQ_ADJMFC;		/* ���f���Ǐ]���䒲������	*/
						}
						else
						{
							FunExe->State = ADAT_SEQ_SEARCHOPTKP;	/* Kp�T������ 				*/
						}
					}
				}
				AdatV->f.VibDetectinCycle = FALSE;					/* �U���X�e�[�^�X�N���A		*/
			}
			break;

		default :
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc ;
}

/****************************************************************************************************
	�U���㏈��
	�Q�C�������ĐU�������҂��Ɉڍs
****************************************************************************************************/
static void	AdatGainDownforVibration(
	ADATV 	*AdatV,
	ADATP 	*AdatP,
	USHORT	ser_kv,								/* �ۑ�����T���ő�Q�C��			*/
	UCHAR	down_ratio							/* �Q�C��Down��						*/
	)
{
	PRMDATA *Prm;
	Prm = AdatP->UniPrms->Prm;

	/* �U�������Q�C��80%��T������ő�l�Ƃ���	*/
	AdatV->MaxKv = ADAT_GRAT_80(ser_kv);

	switch ( down_ratio )
	{
		case 80:
			Prm->loophz = ADAT_GRAT_80( Prm->loophz );		/* �Q�C����80%�ɉ����� */
			break;

		case 50:
			Prm->loophz = ADAT_GRAT_50( Prm->loophz );		/* �Q�C����50%�ɉ����� */
			break;

		default:
			break;
	}
	TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );				/* �Q�C���v�Z			*/
	return;
}


/****************************************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O�m�b�`�t�B���^�ݒ���

	�T�v:	���g����͗v���𗧂āA���g����͌�m�b�`�t�B���^��ݒ肷��B
			�m�b�`�t�B���^�́A�ł��s�[�N�̑傫�����g���ɑ΂��Đݒ肷��B
			Q�l��Kv�C�C�i�[�V�����玩���I�Ɍv�Z����B
			���g����ُ͈�̏ꍇ�ȂǂŁC�m�b�`�ݒ�ُ�Ńm�b�`�����̂܂܂̏ꍇ�C
			�T���Q�C�������݂̃Q�C���ɃZ�b�g���C���ȏ�ɃQ�C�����オ��Ȃ��悤�ɂ���
****************************************************************************************************/
static PRM_RSLT	AdatNotchFilSet( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status)
{
	PRM_RSLT 		rc;
	USHORT			KvGain;
	PRMDATA 		*Prm;

	rc  = PRM_RSLT_SUCCESS;
	Prm = AdatP->UniPrms->Prm;
	AdatV->ErrState = ADAT_CONTINUE;

	switch ( AdatV->NotchSetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�����ݒ�																			*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			AdatV->NotchSetStat = ADAT_STS_NF_SETOK;		/* �m�b�`�ݒ�X�e�[�^�X�̏�����		*/
			AdatV->MaxKvbk = Prm->loophz;					/* �U������Kv��ۑ�					*/
			/* �m�b�`���������A�`�^�������� */
			if( (AdatNotchFilReset( AdatV, AdatP ) == TRUE )
					|| (AdatAresCtrlReset( AdatV, AdatP ) == TRUE) )
			{	/* �m�b�`���������ꂽ���m�b�`�Ȃ��ōēx�Q�C���T�����s��	*/
				if(AdatV->NextSeqPtr > ADAT_SEQ_SEARCHOPTKV)
				{	/* Kv�T���V�[�P���X�ȍ~�́A�m�b�`����������Kv�T���V�[�P���X���Ď��s	*/
					AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKV;
				}
				AdatV->NotchSetPtr = 4;						/* �I���ݒ��						*/

				/* �m�b�`������̒����V�[�P���X��擪������s���邽�߃V�[�P���X�|�C���^�N���A	*/
				AdatV->KvTunePtr  = 0;
				AdatV->KpTunePtr  = 0;
				AdatV->MfcTunePtr = 0;
				AdatV->KpSetPtr   = 0;
			}
			else if( ( (AdatV->VibMaxKv > Prm->loophz) && (AdatV->VibMaxKvAresOFF > Prm->loophz) )
						|| ( ((Prm->tfuncsw & 0x0F0F) == 0x0101) && ((Prm->avibsw & 0x0F) != 0x00) ) )
			{	/* �m�b�`or�`�^��ON�����Q�C����茻Kv�����������m�b�`or�`�^ON���Ă��Q�C�����オ��Ȃ�����   */
				/* �܂��� 2�i�Ƃ��m�b�`�L���A�`�^���U�L��													*/
				/* �����U�}���@�\�����p�ł��Ȃ��̂ŏ����͍s��Ȃ�											*/
				AdatV->NotchSetStat = ADAT_STS_NF_SETERR;	/* �m�b�`�ُ�Ƃ���					*/
				AdatV->NotchSetPtr = 4;						/* �I���ݒ��						*/
			}
			else
			{
				AdatV->f.FftReq    = TRUE;					/* ���g����͗v���n�m				*/
				AdatV->NotchSetPtr = 5;						/* �g���[�X�����҂���				*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�g���[�X�����҂�																	*/
		/*--------------------------------------------------------------------------------------*/
		case 5:
			if( DetVibTraceGetStatus( AdatP->DetVib ) == DETVIBTRC_TRCEND )
			{
				Prm->loophz = ADAT_GRAT_50( Prm->loophz );	/* �Q�C����50%�ɉ�����				*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );	/* �Q�C���v�Z						*/
				AdatV->NotchSetPtr = 1;						/* ���g����͑҂���					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	���g����͊����҂�																	*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			if( status == TUNE_FREQ_SEARCHEND )
			{	/* ���g����͊��� */
				AdatV->f.FftReq = FALSE;						/* �ҋ@��Ԃ�					*/
				AdatV->NotchSetPtr = 2;							/* �m�b�`�t�B���^�ݒ��			*/
				Prm->loophz = AdatV->MaxKvbk;					/* Kv��߂�						*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* �Q�C���v�Z					*/
			}
			else if( status == TUNE_FREQ_SEARCHERR )
			{	/* �G���[�I�� */
				AdatV->f.FftReq = FALSE;						/* �ҋ@��Ԃ�					*/
				AdatV->NotchSetStat = ADAT_STS_NF_ANAERR;		/* ���g����ُ͈��ݒ�			*/
				AdatV->NotchSetPtr = 4;							/* �I���ݒ��					*/
				Prm->loophz = AdatV->MaxKvbk;					/* Kv��߂�						*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* �Q�C���v�Z					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�m�b�`�t�B���^�ݒ�																	*/
		/*--------------------------------------------------------------------------------------*/
		case 2:
				/*------------------------------------------------------------------------------*/
				/*	�m�b�`�t�B���^�ݒ�															*/
				/*------------------------------------------------------------------------------*/
				if(AdatV->f.UsedAllNtFil == FALSE)
				{
					/*--------------------------------------------------------------------------*/
					/*	�m�b�`�t�B���^ or �`�^���U�ݒ��������									*/
					/*		ɯ��ݒ��Kv������݂���B�ŏ�Kv�Ńm�b�`Q�l���������悤�Ȏ��g����	*/
					/*		A�^���U��D�悵��ON����												*/
					/*--------------------------------------------------------------------------*/
					if( (AdatV->f.UsedAResCtrl == FALSE)								/* A�^���U�����\			*/
							&& (FftGetAnaRsltVibmRf( AdatP->FftAna ) <= AVIB_FRQ_MAX)	/* ���g����A�^���U����ȉ�	*/
							&& (Prm->loophz < TUNE_ANOTCH_MINKV) )						/* �m�b�`ON�ŏ�Kv�ȉ�		*/
					{
							KvGain = TUNE_ANOTCH_MINKV;				/* �m�b�`�ݒ�ŏ�Kv�ݒ�		*/
					}
					else
					{
							KvGain = Prm->loophz;					/* Kv�ݒ�l					*/
					}

					/* ��P�s�[�N���g��	*/
					AdatP->ANotch->ANotchData.conf.freq = FftGetAnaRsltVibmRf( AdatP->FftAna );
					AdatP->ANotch->ANotchData.conf.KvGain = KvGain;
					/* �m�b�`�ݒ�X�e�[�^�X�Z�b�g		*/
					AdatV->ANotchSetStat = (CHAR)TuneNotchFilterSetting( AdatP->ANotch,
					                                                     Prm, AdatP->RegManager->hPnReg );
				}
				else
				{
					AdatV->ANotchSetStat = 0x10;		/* �S�m�b�`�g�p���̏ꍇ�m�b�`�G���[�ɂ���	*/
				}

				if( AdatV->ANotchSetStat > 0x02 )
				{	/* �m�b�`�ݒ�ُ�	*/
					/*------------------------------------------------------------------------------*/
					/*	�`�^���U�ݒ�																*/
					/*------------------------------------------------------------------------------*/
					if(AdatV->f.UsedAResCtrl == FALSE)
					{
//						AdatV->AResSetStat = (CHAR)TuneATypeResSet( FftGetAnaRsltVibmRf( AdatP->FftAna ),
//						                                            	((Prm->avibsw >> 4) & 0x0F), Prm );
						AdatV->AResSetStat = (CHAR)TuneATypeResSet( FftGetAnaRsltVibmRf( AdatP->FftAna ),
						                                            	((Prm->avibsw >> 4) & 0x0F), Prm,
																		AdatP->RegManager->hPnReg );/*<S09E>*/
					}
					else
					{
						AdatV->AResSetStat = 0x10;					/* �`�^�g�p���̓G���[�ɂ���		*/
					}

					if( AdatV->AResSetStat >= 0x10 )				/* �`�^�ݒ�ُ�					*/
					{
						AdatV->NotchSetStat = ADAT_STS_NF_SETERR;	/* �m�b�`�p�l�C�`�^�ݒ�ُ�		*/
						AdatV->NotchSetPtr = 4;						/* �I���ݒ��					*/
					}
					else
					{
						AdatV->NotchSetPtr = 3;						/* �U�������҂���				*/
						KlibRstLongTimer( &AdatV->NotchSetCntr );	/* �U�������҂��J�E���^������	*/
						AdatV->VibMaxKvAresOFF = Prm->loophz;		/* �`�^OFF�ł̔��U���E�Q�C���擾*/
						AdatV->AResSetStatBk = AdatV->AResSetStat;	/* �`�^�ݒ肵���X�e�[�^�X��ۑ�	*/
					}
				}
				else
				{	/* �m�b�`�ݒ萳��	*/
					AdatV->NotchSetPtr = 3;							/* �U�������҂���					*/
					KlibRstLongTimer( &AdatV->NotchSetCntr );		/* �U�������҂��J�E���^������		*/
					AdatV->VibMaxKv = Prm->loophz;					/* �m�b�`OFF�ł̔��U���E�Q�C���擾	*/
					AdatV->ANotchSetStatBk = AdatV->ANotchSetStat;	/* �m�b�`�ݒ肵���X�e�[�^�X��ۑ�	*/
				}
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* Tf�v�Z�̂��ߕK�v				*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�m�b�`�ݒ��̐U�������҂�															*/
		/*--------------------------------------------------------------------------------------*/
		case 3:
			if( KlibGetLongTimerMs( AdatV->NotchSetCntr ) >= ADATVIBWAITTM3 ) /* �U�������҂�300ms */
			{
				AdatV->NotchSetPtr = 4;							/* �I���ݒ��					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�I���ݒ�																			*/
		/*--------------------------------------------------------------------------------------*/
		case 4:
			switch ( AdatV->NotchSetStat )
			{
				case ADAT_STS_NF_SETERR :				/* �m�b�`�p�l�ُ� �� �t�B���^�ݒ薢��	*/
				case ADAT_STS_NF_ANAERR:				/* ���g����ُ͈�						*/
					/* �Q�C���̍Đݒ�	*/
					AdatV->MaxKv = ADAT_GRAT_80( Prm->loophz );		/* �U���Q�C����80%�ɐݒ� 	*/
					Prm->loophz  = ADAT_GRAT_80( Prm->loophz );		/* �Q�C����80%�ɉ�����	 	*/
					break;

				case ADAT_STS_NF_SETOK:
				default:
					break;
			}
			TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );	/* �Q�C���v�Z 							*/
			AdatV->NotchSetPtr = 0;						/* �����ݒ�� 							*/
			FunExe->State  = AdatV->NextSeqPtr;			/* �ۑ����Ă����V�[�P���X�|�C���^�����̃|�C���^�ɐݒ� */
			AdatV->ErrState = ADAT_OK;					/* ����I��	 							*/
			break;

		default :
			break;
	}
	return rc ;
}


/****************************************************************************************************
	�m�b�`�t�B���^�ݒ��������

	�T�v:	�m�b�`OFF���̔��U���E�Q�C��(AdatV.VibMaxKv)��62.5%�ȉ�
			�ɂȂ�����C��1�C��2�m�b�`�t�B���^�̂����Ō�ɐݒ肵���m�b�`����������B
			�ǂ̃m�b�`���������邩�́Ctune_inf.notch_status���Q�Ƃ���
			�m�b�`�����������ꍇ�́C�ȍ~�̃`���[�j���O�Ńm�b�`���n�m���Ȃ��悤��
			AdatV.f.UsedAllNtFil���h�ݒ肷�݁h�ɂ���
****************************************************************************************************/
static SHORT	AdatNotchFilReset( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA  *Prm;
	AUTONOTCH *ANotch;
	REGIF_CONFIG_T *RegCfg;
	REG_ACC_T AccessPrm;
	SHORT	rc;

	Prm = AdatP->UniPrms->Prm;
	ANotch = AdatP->ANotch;
	RegCfg = AdatP->RegManager->hPnReg;
	rc = FALSE;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	if( (Prm->tfuncsw & 0x0F0F) != 0x0000 )
	{	/* ON���Ă���m�b�`������	*/
		if( ADAT_GRAT_62( AdatV->VibMaxKv ) > Prm->loophz )
		{	/* �m�b�`OFF���̌��E�Q�C����62.5%�ȉ��ɂȂ���	*/
			switch ( AdatV->ANotchSetStatBk )
			{	/* �m�b�`�����X�e�[�^�X	*/
				case 0x01:								/* �O��1�i�ڃm�b�`ON		*/
					/* ��1�m�b�`OFF	*/
					Prm->tfuncsw &= 0xFFF0;
					/* Pn408 */
					TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, Prm->tfuncsw, pndef_tfuncsw.DataLength );
					/* Pn409 */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );
					/* Pn40A */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1, 0 );
					rc = TRUE;
					break;

				case 0x02:								/* �O��2�i�ڃm�b�`ON		*/
					/* ��2�m�b�`OFF	*/
					Prm->tfuncsw &= 0xF0FF;
					/* Pn408 */
					TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, Prm->tfuncsw, pndef_tfuncsw.DataLength );
					/* Pn40C */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );
					/* Pn40D */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2, 0 );
					rc = TRUE;
					break;

				default:
					break;
			}
			TuneNotchQ( ANotch, RegCfg, Prm->loophz, Prm->jrate, 1, Prm );
			/*----------------------------------------------------------------------*/
			/*	�m�b�`������̏���													*/
			/*----------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				AdatV->ANotchSetStat   = 0;				/* �m�b�`�X�e�[�^�X�N���A	*/
				AdatV->ANotchSetStatBk = 0;				/* �m�b�`�X�e�[�^�X�N���A	*/
				AdatV->f.UsedAllNtFil  = TRUE;			/* �t�B���^�ݒ�ς݂ɂ���	*/
			}
		}
	}
	return rc;
}

/****************************************************************************************************
	A�^���U����ݒ��������

	�T�v:	A�^���UOFF���̔��U���E�Q�C��(AdatV.VibMaxKvAresOFF)��62.5%�ȉ�
			�ɂȂ�����AA�^���U����ݒ����������B
			���������ꍇ�́A�ȍ~�̃`���[�j���O��A�^��ON���Ȃ��悤��
			AdatV.f.UsedAResCtrl���h�ݒ肷�݁h�ɂ���

	 out		: SHORT		rc				:	FALSE	:��������
												TRUE	:�`�^���U����
****************************************************************************************************/
static SHORT	AdatAresCtrlReset( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;
	REG_ACC_T AccessPrm;
	SHORT	rc;
	USHORT	data;

	Prm    = AdatP->UniPrms->Prm;
	RegCfg = AdatP->RegManager->hPnReg;
	rc = FALSE;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	if( (Prm->avibsw & 0x0F) != 0x00 )
	{	/* A�^���UON���Ă���	*/
		if( ADAT_GRAT_62( AdatV->VibMaxKvAresOFF ) > Prm->loophz )
		{	/* �m�b�`OFF���̌��E�Q�C����62.5%�ȉ��ɂȂ���	*/
			switch ( AdatV->AResSetStatBk )
			{	/* �m�b�`�����X�e�[�^�X	*/
				case 0x01:							/* �O�� ������A�^���UON			*/
					/* A�^���UOFF */
					data = 0xFFFFFFF0 & Prm->avibsw;
					/* Pn160 */
					TunePrmWriteRam( &AccessPrm, &pndef_avibsw, (LONG)data, pndef_avibsw.DataLength );
					/* Pn161 */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq, 0 );
					/* Pn163 */
					PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp, 0 );
					rc = TRUE;
					break;

				default:
					break;
			}
			/*----------------------------------------------------------------------*/
			/*	�`�^���U������̏���												*/
			/*----------------------------------------------------------------------*/
			if( rc == TRUE )
			{
				AdatV->AResSetStat = 0;				/* �X�e�[�^�X�N���A				*/
				AdatV->AResSetStatBk = 0;			/* �X�e�[�^�X�N���A				*/
				AdatV->f.UsedAResCtrl = TRUE;		/* �`�^���U�����ݒ�ς݂ɂ���	*/
			}
		}
	}
	return rc;
}


/****************************************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���OKp�Q�C���ݒ���

	�T�v:	�œK�Q�C���T����AKp�T���AKpm�T���O��
			Kp�����X�ɑ傫������B
			Kv�T������Kp��Ⴍ�ݒ肵�Ă���̂�Kp�AKpm�T���J�n
			����Kp�������Ȃ�傫�Ȓl�ɐݒ肵���Ƃ��̃V���b�N�ጸ�B
****************************************************************************************************/
static PRM_RSLT	AdatKpGainSet(ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA	*Prm;
	PRM_RSLT rc;
	UCHAR tmpGainRatio;

	Prm  = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;										/* �U��������Q�C������ �����l=80% */

	/* ScanC�����ŃG���[���������ꍇ */
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->KpSetPtr = 0;								/* �����ݒ�� 						*/
		AdatV->ErrState = ADAT_NG;							/* �ُ�I��   						*/
	}

	switch ( AdatV->KpSetPtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�����ݒ�																			*/
		/*--------------------------------------------------------------------------------------*/
		case 0:
			AdatV->f.DrvPatternSet  = FALSE;
			AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* �v���O����JOG�^�]�p�^�[��3		*/

			/* �U�����o�t�B���^�ݒ�	*/
			if( Prm->loophz > VIBOBS_KS )
			{/* HPF�Z�b�g	*/
				DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, Prm->loophz,
				                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs);
			}
			else
			{/* HPF�Z�b�g	*/
				DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
				                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
			}
			AdatP->FnCmnCtrl->State.TuningPctrlRun = TRUE;			/* �ʒu���[�v�T���J�n		*/
			AdatV->KpSetPtr = 1;									/* Kp�ݒ�v����				*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kp�ݒ�v��																			*/
		/*--------------------------------------------------------------------------------------*/
		case 1:
			/* �ݒ肷��ő�Kp��ݒ�		*/
			AdatV->MaxKp = Prm->loophz;						/* �ݒ�Kp�ő�l�͌�Kv�Ɠ���			*/
			AdatScanCRequest( ADAT_SEQ_KPSET, AdatV );		/* �X�L����C�����v��				*/
			AdatV->KpSetPtr = 2;							/* Kp�ݒ��Ԃ�						*/
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	Kp�ݒ���																			*/
		/*--------------------------------------------------------------------------------------*/
		case 2:
			switch ( status )
			{
				case ADAT_SEAR_NORMALEND :							/* ����I��					*/
					AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;		/* �v���O����JOG�I��		*/
					AdatV->KpSetPtr = 10	;						/* �����I����				*/
					break;
				case ADAT_SEAR_NOVIBDETECT :						/* �U������					*/
					/* Kp�Q�C��UP */
					TuneAdatSetPosCtrlGain( GAIN_UP, AdatP );
					AdatV->KpSetPtr = 1;
					break;
				case ADAT_SEAR_STOPVIBEND:							/* ��~���U�����o�I��		*/
					/* �Q�C��������50%�ɕύX */
					tmpGainRatio = 50;
				/* Fall Through */
				case ADAT_SEAR_VIBEND:								/* �U�����o�I��				*/
					/* Kv�Q�C��DOWN */
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}
					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* ���Ƀm�b�`�t�B���^�ݒ�ς� �� �Ď��s	*/
						/* �U���㏈�� */
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->KpSetCntr );
						AdatV->KpSetPtr = 3;
						AdatV->NextSubSeqPtr = 1;
					}
					else
					{	/* �m�b�`�t�B���^�ݒ薢��	*/
						AdatV->KpSetPtr = 1;
						AdatV->NextSeqPtr = ADAT_SEQ_KPSET;
						FunExe->State = ADAT_SEQ_NOTCHSET;
						AdatV->ErrState = ADAT_OK;					/* ����I��					*/
					}
					break;
					
				default :
					break;
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�U�������҂����																	*/
		/*--------------------------------------------------------------------------------------*/
		case 3:
			if( KlibGetLongTimerMs( AdatV->KpSetCntr ) >= ADATVIBWAITTM2 ) /* �U�������҂����� 300ms	*/
			{
				AdatV->KpSetPtr = AdatV->NextSubSeqPtr;
				Prm->loophz = AdatV->MaxKv;							/* �T���Q�C���ő�l�ɖ߂�	*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* �Q�C���v�Z				*/
			}
			break;
		/*--------------------------------------------------------------------------------------*/
		/*	�I������																			*/
		/*--------------------------------------------------------------------------------------*/
		case 10:
			AdatScanCRequest( ADAT_NO_OPE, AdatV );				/* �����I����͑ҋ@��Ԃ�v��	*/
			AdatV->KpSetPtr = 0;								/* �V�[�P���X�N���A				*/
			AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;

			if( ( AdatReg->AdatLevel == ADAT_LEVEL_POS )
					|| ( AdatReg->AdatLevel == ADAT_LEVEL_POS_ST ) )
			{
				FunExe->State = ADAT_SEQ_ADJMFC;				/* ���f���Ǐ]���䒲������		*/
			}
			else
			{
				FunExe->State = ADAT_SEQ_SEARCHOPTKP;			/* Kp�T������					*/
			}
			break;

		default :
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc ;
}



/****************************************************************************************************
	�ʒu���[�v�Q�C���ύX����

	�T�v:	�ʒu���[�v�Q�C����v���ɉ����ĕύX����B
****************************************************************************************************/
static SHORT	TuneAdatSetPosCtrlGain( LONG mode, ADATP *AdatP)
{
	PRMDATA		*Prm;
	SHORT		rc;
	LONG		data;
	REG_ACC_T 	AccessPrm;

	Prm = AdatP->UniPrms->Prm;
	rc  = ADAT_OK;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( mode )
	{
		case GAIN_UP:
			data = Prm->posgn + KPUP_UNIT;			/* 3.0 [1/s] ����							*/
			break;

		case GAIN_DW:
			data = Prm->posgn - KPDOWN_UNIT;		/* 3.0 [1/s] ����							*/
			break;

		case GAIN_BAL:
		case GAIN_BAL2:
			/*----------------------------------------------------------------------------------*/
			/*	�ʒu���[�v�Q�C��(Kp)															*/
			/*----------------------------------------------------------------------------------*/
			if( AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl == TRUE )
			{	/* IP����	*/
				data = 320000 / Prm->pitime;		/*  Kp[1/s] = 0.32 / Ti[s]					*/
			}
			else
			{	/* PI����	*/
				data = ADAT_GRAT_150( Prm->loophz );/* Kp[1/s] = Kv[rad/s] / 4 �� 1.5 *Kv[Hz]	*/
			}

			if( mode == GAIN_BAL2 )
			{
				if( data >= Prm->posgn )
				{
					return rc;						/* ���ݐݒ�l�ȏ�̏ꍇ�́A�������Ȃ�		*/
				}
			}
			break;

		case GAIN_BAL3:								/* ���f���Ǐ]���䎞Kp�o�����X				*/
		case GAIN_BAL4:
			/*----------------------------------------------------------------------------------*/
			/*	�ʒu���[�v�Q�C��(Kp)															*/
			/*----------------------------------------------------------------------------------*/
			data = TuneKvFuncKp( Prm->loophz, Prm->pitime, AdatP->BaseCtrlData->CtrlCmdPrm.IPSpdControl );

			if( mode == GAIN_BAL4 )
			{
				if( data >= Prm->posgn )
				{
					return rc;						/* ���ݐݒ�l�ȏ�̏ꍇ�́A�������Ȃ�		*/
				}
			}
			break;

		default:
			data = Prm->posgn;						/* ����̂܂�							    */
			break;
	}

	if( data < KP_UNLMT )
	{
		data = KP_UNLMT;							/* �����l�𒴂���ꍇ�͉����l�Ƃ���			*/
		rc = ADAT_NG;								/* �ُ�I���Ƃ���							*/
	}
	else if( data > pndef_posgn.UpperLimit )
	{
		data = pndef_posgn.UpperLimit;				/* ����l�𒴂���ꍇ�͏���l�Ƃ���			*/
	}

	TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)data, pndef_posgn.DataLength );
	return rc;
}

/****************************************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�œKKp�Q�C���T�����

	�T�v:	�œKKp�Q�C���T�����X�L����C�ɗv������B
****************************************************************************************************/
static PRM_RSLT	AdatOptKpSearch( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	PRMDATA	 *Prm;
	PRM_RSLT rc;
	LONG user_maxGain;								/* ���[�U���x���ɉ������T���ő�Q�C������		*/
	UCHAR tmpGainRatio;

	Prm = AdatP->UniPrms->Prm;
	rc  = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;								/* �U��������Q�C������ �����l=80%				*/

	AdatV->ErrState = ADAT_CONTINUE;

	/* ScanC�����ŃG���[���������ꍇ */
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->KpTunePtr = 0;						/* �����ݒ��Ԃ�								*/
		AdatV->ErrState = ADAT_NG;					/* �ُ�I��										*/
	}

	switch ( AdatV->KpTunePtr )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	�����ݒ�																				*/
		/*------------------------------------------------------------------------------------------*/
		case ADAT_OPTKP_INI:
			/* ���[�^��~���o(����ؑ֎��̃V���b�N�}���̂���) */
			if( (AdatV->PrgJogStat == ADAT_PJOG_NOP) && (AdatP->SeqCtrlOut->MotStop == TRUE) )
			{	/* �v���O����JOG�^�]����	*/
				/* �T���Q�C���ő�l�ݒ�		*/
				user_maxGain = ADAT_LVL_80;						/* 80���Q�C��						*/
				AdatV->MaxKv = Prm->loophz;						/* ���݂̃Q�C����T���ő�޲݂ɐݒ�	*/
#if (FLOAT_USE==TRUE)
				Prm->loophz = (USHORT)( (LONG)AdatV->MaxKv * user_maxGain );

				/* �m�b�`OFF���̔��U���E�Q�C�����X�V�B												*/
				/* �m�b�`����������Prm->loophz�Ɣ�r���邽��user_maxGain�ɉ����čČv�Z����			*/
				AdatV->VibMaxKv = (USHORT)( (LONG)AdatV->VibMaxKv * user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)( (LONG)AdatV->VibMaxKvAresOFF * user_maxGain );
#else
				Prm->loophz = (USHORT)MlibMulgain( (LONG)AdatV->MaxKv, user_maxGain );

				/* �m�b�`OFF���̔��U���E�Q�C�����X�V�B												*/
				/* �m�b�`����������Prm->loophz�Ɣ�r���邽��user_maxGain�ɉ����čČv�Z����			*/
				AdatV->VibMaxKv = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKv, user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKvAresOFF, user_maxGain );
#endif /* FLOAT_USE */

				/* �Q�C���ݒ�	*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* �Q�C���v�Z     					*/
				TuneAdatSetPosCtrlGain( GAIN_BAL, AdatP );		/* Kp�Q�C���v�Z 					*/

				/* �U�����x���ݒ� 					 */
				/* �Q�C���T�����͒ʏ�U�����x����2�{ */
				AdatP->DetVib->conf.VibCheckLevel = AdatV->VibSpeed * RESGMAX / 100;

				/* ���U���o�p�I�u�U�[�o�Q�C���ύX(Kv > 80[Hz])	*/
				AdatP->FnCmnCtrl->State.TuningPctrlRun = TRUE;	/* �ʒu���[�v�T���J�n				*/
				if( Prm->loophz > VIBOBS_KS )
				{	/* ObsGain = Kv	*/
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, Prm->loophz,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}
				else
				{
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}

				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* �v���O����JOG�^�]�p�^�[��3		*/
				AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;			/* �œKKp�Q�C���T���v����			*/
				AdatV->f.VibDetectinCycle = TRUE;				/* �U���X�e�[�^�X�������B			*/
																/* �����l�͐U������					*/
			}
			break;

			/*--------------------------------------------------------------------------------------*/
			/*	�œKKp�Q�C���T���v��																*/
			/*--------------------------------------------------------------------------------------*/
			case ADAT_OPTKP_CHK_REQ:
				AdatScanCRequest( ADAT_SEQ_SEARCHOPTKP, AdatV );/* �X�L����C�����v��				*/
				AdatV->KpTunePtr = ADAT_OPTKP_CHK;				/* �œK�Q�C���T����					*/
				break;

			/*--------------------------------------------------------------------------------------*/
			/*	�œKKp�Q�C���T�����																*/
			/*--------------------------------------------------------------------------------------*/
			case ADAT_OPTKP_CHK:
				switch ( status )
				{
					case ADAT_SEAR_STOPVIBEND:					/* ��~���U�����o�I��				*/
						tmpGainRatio = 50;						/* �Q�C��������50%�ɕύX			*/
					/* Fall Through */

					case ADAT_SEAR_VIBEND:						/* �U�����o�I��						*/
						AdatV->f.VibDetectinCycle = TRUE;		/* �U���X�e�[�^�X�n�m				*/
						if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* Kv�Q�C��DWON */
						{
							AdatV->ErrState = ADAT_NG;
							return rc;
						}
						TuneAdatSetPosCtrlGain( GAIN_BAL2, AdatP ); /* Kp�Q�C���v�Z 				*/

						if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
						{	/* ���Ƀm�b�`�t�B���^�ݒ�ς� �� �������s */
							AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
							KlibRstLongTimer( &AdatV->KpTuneCntr );
							AdatV->KpTunePtr = ADAT_OPTKP_WAIT;
							AdatV->NextSubSeqPtr = ADAT_OPTKP_CHK_REQ;
																/* �U���㏈��						*/
						}
						else
						{	/* �m�b�`�t�B���^�ݒ薢��	*/
							AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;
							AdatV->NextSeqPtr = ADAT_SEQ_SEARCHOPTKP;
							FunExe->State = ADAT_SEQ_NOTCHSET;
							AdatV->ErrState = ADAT_OK;			/* ����I��							*/
						}
						break;

					case ADAT_SEAR_COINBREAKEND:				/* COIN���ꌟ�o�I��					*/
						AdatV->f.VibDetectinCycle = TRUE;		/* �U���X�e�[�^�X�n�m				*/
						AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;	/* �U�����Ă�����ăg���C			*/

						if( TuneAdatSetPosCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* Kp�Q�C��DOWN */
						{
							AdatV->ErrState = ADAT_NG;
							return rc ;
						}
						break;

					default :
						break;
				}
				/*----------------------------------------------------------------------------------*/
				/*	�^�]�p�^�[���I��																*/
				/*----------------------------------------------------------------------------------*/
				if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
				{	/* �v���O����JOG�^�]���� */
					if( (AdatV->f.VibDetectinCycle == TRUE )
							|| ( AdatV->f.DrvPatternSet == FALSE ) ) /* �w�߃p�^�[��������			*/
					{	/* ����^�]���ɐU�����o����	*/
						AdatV->f.VibDetectinCycle = FALSE;			 /* �U���X�e�[�^�X�N���A		*/
						AdatV->f.DrvPatternSet = FALSE;
						AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;/* �v���O����JOG�^�]�p�^�[��3	*/
						AdatV->KpTunePtr = ADAT_OPTKP_CHK_REQ;		 /* �U�����Ă�����ăg���C		*/
					}
					else
					{	/* ����^�]���U�����Ȃ����� �� �T������ �� �I���ݒ��	*/
						AdatScanCRequest( ADAT_NO_OPE, AdatV );		/* �����I����͑ҋ@��Ԃ�v��	*/
						AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;	/* �v���O����JOG�^�]��~		*/
						AdatV->KpTunePtr = ADAT_OPTKP_INI;			/* ����V�[�P���X�|�C���^�Z�b�g	*/
						FunExe->State = ADAT_SEQ_ENDDISP;			/* ����`���[�j���O�V�[�P���X�Z�b�g	*/
						AdatV->ErrState = ADAT_OK;					/* ����I��						*/
					}
				}
				break;

			/*--------------------------------------------------------------------------------------*/
			/*	�U�������҂����																	*/
			/*--------------------------------------------------------------------------------------*/
			case ADAT_OPTKP_WAIT:
				/* �U�������҂�����	300ms */
				if( KlibGetLongTimerMs( AdatV->KpTuneCntr ) >= ADATVIBWAITTM2 )
				{
					AdatV->KpTunePtr = AdatV->NextSubSeqPtr;
					Prm->loophz = AdatV->MaxKv;							/* �T���Q�C���ő�l�ɖ߂�	*/
					TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );			/* �Q�C���v�Z				*/
				}
				break;

			default :
				AdatV->ErrState = ADAT_NG;
				break;
		}
		return rc ;
}

/****************************************************************************************************
		�A�h�o���X�g�I�[�g�`���[�j���O���f���Ǐ]���䒲�����

	�T�v:	���f���Ǐ]�����L���Ƃ��āC���f�����[�v�Q�C��(Kpm)�����
			�g���N�e�e�̒������s���B���̑��̊֘A�p�����[�^�́C�Œ�l�Ƃ��Ē������Ȃ�
****************************************************************************************************/
static PRM_RSLT	AdatMfcAdjustment( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT status )
{
	MFCTRL   *MFControl;
	PRMDATA	 *Prm;
	REG_ACC_T AccessPrm;
	PRM_RSLT rc;
	LONG	 user_maxGain;
	LONG	 UnRvibFrequency;
	LONG     SettlingTime;
	UCHAR	 tmpGainRatio;
	USHORT	 edata;
	USHORT	 ret0;

	MFControl = &AdatP->BaseCtrlData->MFControl;
	Prm = AdatP->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;
	tmpGainRatio = 80;					/* �U��������Q�C������ �����l=80% 						*/
	UnRvibFrequency = RemVibGetRemVibFreq( AdatP->RemVibFreq );

	AdatV->ErrState = ADAT_CONTINUE;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;


	/*	ScanC�����ŃG���[���������ꍇ	*/
	if( status == ADAT_SEAR_ERR )
	{
		AdatV->MfcTunePtr = 0;			/* �����ݒ��Ԃ�										*/
		AdatV->ErrState = ADAT_NG;		/* �ُ�I��												*/
	}

	switch ( AdatV->MfcTunePtr )
	{
		/*--------------------------------------------------------------------------------------*/
		/*	�����ݒ�																			*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_INI:
			/* ���[�^��~���o(����ؑ֎��̃V���b�N�}���̂���) */
			if( AdatP->SeqCtrlOut->MotStop == TRUE )
			{	/* �v���O����JOG�^�]����	*/
				AdatV->MaxKv = Prm->loophz;				/* ���݂̃Q�C����T���ő�Q�C���ɐݒ�	*/
				user_maxGain = ADAT_LVL_80;				/* �T���ő�Q�C������					*/

#if (FLOAT_USE==TRUE)
				Prm->loophz = (USHORT)( (LONG)AdatV->MaxKv * user_maxGain );
				/*	�m�b�`OFF���̔��U���E�Q�C�����X�V�B											*/
				/*	�m�b�`����������Prm->loophz�Ɣ�r���邽��ADAT_LVL_80�ōČv�Z����			*/
				AdatV->VibMaxKv = (USHORT)( (LONG)AdatV->VibMaxKv * user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)( (LONG)AdatV->VibMaxKvAresOFF * user_maxGain);
#else
				Prm->loophz = (USHORT)MlibMulgain( (LONG)AdatV->MaxKv, user_maxGain );
				/*	�m�b�`OFF���̔��U���E�Q�C�����X�V�B											*/
				/*	�m�b�`����������Prm->loophz�Ɣ�r���邽��ADAT_LVL_80�ōČv�Z����			*/
				AdatV->VibMaxKv = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKv, user_maxGain );
				AdatV->VibMaxKvAresOFF = (USHORT)MlibMulgain( (LONG)AdatV->VibMaxKvAresOFF,user_maxGain);
#endif /* FLOAT_USE */

				/* �Q�C���ݒ�	*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* �Q�C���v�Z					*/
				TuneAdatSetPosCtrlGain( GAIN_BAL3, AdatP );		/* Kp�Q�C���v�Z					*/

				/* �����ő�Kpm������	*/
				AdatV->MaxKpm = Prm->adatgmax << 1;				/* �����l�́APn540�~2			*/

				/*------------------------------------------------------------------------------*/
				/*	���f���Ǐ]�ݒ�																*/
				/*------------------------------------------------------------------------------*/
				/* �p�����[�^�����ݒ�					*/
				/*	ANTFRQ�CRESFRQ�͐ݒ�l���g�p		*/
				/*	�O���w�߂ł́CKpm�ݒ�l���璲���J�n	*/
				switch ( AdatReg->AdatLevel )
				{	/* �`���[�j���O���x�� */
					case ADAT_LEVEL_POS:				/* �ʒu���ߐ�p							*/
//						TuneMfcActive( Prm );
						TuneMfcActive( Prm, AccessPrm.hRegCfg );//<S09E>
						break;
					case ADAT_LEVEL_POS_ST:				/* �ʒu���ߐ�p(�I�[�o�[�V���[�g�d��)	*/
//						TuneMfcActiveStMode( Prm );
						TuneMfcActiveStMode( Prm, AccessPrm.hRegCfg );//<S09E>
						break;
					default:
						break;
				}
				if( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
				{	/* AAT�O�񊵐�MFC���L��	*/
					Prm->mdlgn = TuneMfcKpmCalcAntFreq( Prm->mdlantfrq ) / 2;
				}
				else
				{	/* AAT�OMFC����or����MFC */
					Prm->mdlgn = Prm->posgn;
				}
				/* Kpm�ݒ� 								*/
				TuneAdatSetMdlCtrlGain( GAIN_BAL, AdatP );
				/* �U�����x���ݒ� 						*/
				/* �Q�C���T�����͒ʏ�U�����x����2�{ 	*/
				AdatP->DetVib->conf.VibCheckLevel = AdatV->VibSpeed * RESGMAX / 100;
				/* ���U���o�p�I�u�U�[�o�Q�C���ύX(Kv > 80[Hz])	*/
				/* �ʒu���[�v�T���J�n					*/
				AdatP->FnCmnCtrl->State.TuningPctrlRun = TRUE;
				if( Prm->loophz > VIBOBS_KS )
				{	/* ObsGain = Kv	*/
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, Prm->loophz,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}
				else
				{
					DetVibObsCalculateGains( &AdatP->DetVib->DetVibObs, VIBOBS_KS,
					                         DETVIBLP, DETVIBHP, AdatP->UniPrms->Bprm->SvCycleNs );
				}

				AdatV->f.DrvPatternSet = FALSE;
				AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN1;	/* �v���O����JOG�^�]�p�^�[��1	*/
				AdatV->MfcTunePtr = ADAT_MFC_INIWAIT;			/* �Q�C���ύX�ɂ��U�������҂�	*/
				AdatV->f.VibDetectinCycle = FALSE;				/* �U���X�e�[�^�X�N���A			*/
				RemVibRsetRemVibrationFreq( AdatP->RemVibFreq );/* �c���U�����g���N���A			*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�Q�C���ύX�ɂ��U�������҂�														*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_INIWAIT:
			if( (MFControl->conf.MfcPexe.MFCModel == MFControl->conf.MfcPrm[0].MFCModel)/* ���s���f�����p�����[�^�Ɠ������H			*/
				&& (MFControl->conf.MfcPexe.Kj2   == MFControl->conf.MfcPrm[0].Kj2)		/* ���s���f�����g�����p�����[�^�Ɠ������H	*/
				&& (MFControl->conf.MfcPexe.Kf2   == MFControl->conf.MfcPrm[0].Kf2) )	/* ���s���f���Q�C�����p�����[�^�Ɠ������H	*/
			{
				BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN���ꎞ�ԃN���A				*/
				BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* �I�[�o�[�V���[�g�ʍő�l�N���A	*/
				AdatV->MfcTunePtr = ADAT_MFC_MAXREQ;		/* Kpm�`���[�j���O�v����			*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kpm�`���[�j���O�v��																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_MAXREQ:
			AdatScanCRequest( ADAT_SEQ_ADJMFC, AdatV );		/* �X�L����C�����v��				*/
			AdatV->MfcTunePtr = ADAT_MFC_MAXSEARCH;			/* Kpm�`���[�j���O��				*/
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	Kpm�`���[�j���O���																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_MAXSEARCH:
			switch ( status )
			{
				case ADAT_SEAR_STOPVIBEND:				/* ��~���U�����o�I��					*/
					tmpGainRatio = 50;					/* �Q�C��������50%�ɕύX				*/
				/* Fall Through */
				case ADAT_SEAR_VIBEND:					/* �U�����o�I��							*/
					AdatV->f.VibDetectinCycle = TRUE;	/* �P��͑S�^�]�p�^�[���𓮍삳���邽�� */
														/*	�����l��U���X�e�[�^�X�n�m�Ƃ���	*/
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG )	/* Kv�Q�C��DOWN	*/
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}
					TuneAdatSetPosCtrlGain( GAIN_BAL4, AdatP ); /* Kp�Q�C���v�Z	*/

					/*--------------------------------------------------------------------------*/
					/*	Kpm�T�����ɐU��������Kpm=Kp�Ƃ���Kpm�ő�l���ĒT��						*/
					/*--------------------------------------------------------------------------*/
					if(Prm->mdlgn > Prm->posgn)								/* Kpm > Kp			*/
					{
						Prm->mdlgn = Prm->posgn;							/* Kpm �� Kp		*/
						TuneAdatSetMdlCtrlGain( GAIN_BAL, AdatP );			/* Kpm�ݒ�			*/
						/* ����Kpm�ő�l��1/2�ɂ���	*/
						AdatV->MaxKpm = MlibMAX( (AdatV->MaxKpm >> 1), Prm->mdlgn );
					}

					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* ���Ƀm�b�`�t�B���^�ݒ�ς� �� COIN����m�F�v����	*/
						/* �U���㏈�� */
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->MfcTuneCntr );
						AdatV->MfcTunePtr = ADAT_MFC_WAIT;
						AdatV->NextSubSeqPtr = ADAT_MFC_MAXREQ;
					}
					else
					{	/* �m�b�`�t�B���^�ݒ薢��	*/
						AdatV->MfcTunePtr = ADAT_MFC_MAXREQ;
						AdatV->NextSeqPtr = ADAT_SEQ_ADJMFC;
						FunExe->State = ADAT_SEQ_NOTCHSET;
						AdatV->ErrState = ADAT_OK;				/* ����I��							*/
					}
					break;

				case ADAT_SEAR_COINBREAKEND:					/* COIN���ꌟ�o�I��					*/
					AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;		/* COIN����m�F�v����				*/
					AdatV->f.VibDetectinCycle = TRUE;			/* 1��͑S�^�]�p�^�[���𓮍삳���邽��*/
																/* �����l��U���X�e�[�^�XON�Ƃ���	*/
					TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* Kpm�Q�C��DOWN				*/
					break;

				case ADAT_SEAR_COINOK :
					TuneAdatSetMdlCtrlGain( GAIN_UP, AdatP );	/* Kpm�Q�C��UP						*/
					AdatV->MfcTunePtr = ADAT_MFC_MAXREQ;		/* Kpm�`���[�j���O�m�F�v����		*/

					/*------------------------------------------------------------------------------*/
					/*	Kpm������~�b�g����															*/
					/*------------------------------------------------------------------------------*/
					if( (Prm->mdlgn > AdatV->MaxKpm) || (Prm->mdlgn == pndef_mdlgn.UpperLimit) )
					{	/* ����l�𒴂���	*/
						AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;
						AdatV->f.VibDetectinCycle = TRUE;	/* 1��͑S�^�]�p�^�[���𓮍삳���邽��	*/
															/* �����l��U���X�e�[�^�XON�Ƃ���		*/
						AdatV->ErrState = ADAT_OK;
					}
					/*------------------------------------------------------------------------------*/
					/*  2����MFC�L���ɂ��ւ�炸�A����g�U�������o�����ꍇ�́A�Q�C����������		*/
					/*------------------------------------------------------------------------------*/
					if( (UnRvibFrequency >= VIBMFC_DET_FRQ_MIN)		/* ���o���g�� >= �U�����o�����l	*/
						&& (UnRvibFrequency <= VIBMFC_DET_FRQ_MAX)	/* ���o���g�� <= �U�����o����l	*/
						&& (MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS) ) /* 2����MFC�L�� 	*/
					{
						AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;		/* COIN����m�F�v����					*/
						AdatV->f.VibDetectinCycle = TRUE;			/* 1��͑S�^�]�p�^�[���𓮍삳���邽�߁C*/
																	/* �����l��U���X�e�[�^�XON�Ƃ���		*/
						TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* Kpm�Q�C��DOWN						*/
					}
					if( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
					{	/* 2����MFC or ���́{�@��MFC	*/
						if( Prm->mdlgn > (3 * Prm->mdlantfrq) )
						{	/* ����l�𒴂��� */
							AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;	/* COIN����m�F�v����					*/
							AdatV->f.VibDetectinCycle = TRUE;		/* 1��͑S�^�]�p�^�[���𓮍삳���邽��	*/
																	/* �����l��U���X�e�[�^�XON�Ƃ���		*/
							TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* Kpm�Q�C��DOWN					*/
						}
					}
					break;

				default :
					break;
			}
			/* 2����MFC�ݒ���m�b�`�ݒ�ACOIN���ꌟ�o���D�悷�� */
			if( AdatV->f.Used2MassOld == FALSE )
			{
				ret0 = SetMfc2MassSequence( AdatReg, AdatV, AdatP, FunExe, ADAT_SEQ_ADJMFC );
				if( ret0 )
				{
					FunExe->State = ADAT_SEQ_2MASSSET;	/* ���f���Ǐ](����+�@�䃂�f��)�ݒ菈����*/
					AdatV->NextSeqPtr = ADAT_SEQ_ADJMFC;/* �ݒ�㌳�̏����֖߂邽�߂̃o�b�t�@	*/
					AdatV->f.VibDetectinCycle = TRUE;	/* �U���X�e�[�^�XON						*/
				}
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	�U�������҂����																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_WAIT:
			/* �U�������҂�����	300ms */
			if( KlibGetLongTimerMs( AdatV->MfcTuneCntr ) >= ADATVIBWAITTM2 )
			{
				AdatV->MfcTunePtr = AdatV->NextSubSeqPtr;
				Prm->loophz = AdatV->MaxKv;						/* �T���Q�C���ő�l�ɖ߂�		*/
				TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );		/* �Q�C���v�Z					*/
			}
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	COIN����m�F�v�����																*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_CHK_REQ:
			AdatV->settling_max = 0;							/* ���莞�Ԃ̍ő�l�����Z�b�g	*/
			AdatScanCRequest( ADAT_VIBCOIN, AdatV );			/* �X�L����C�����v��			*/
			AdatV->MfcTunePtr = ADAT_MFC_CHK;					/* COIN����m�F��				*/
			MfcPrmReSet( AdatV, AdatP );
			break;

		/*--------------------------------------------------------------------------------------*/
		/*	COIN����m�F���																	*/
		/*--------------------------------------------------------------------------------------*/
		case ADAT_MFC_CHK:
			switch ( status )
			{
				case ADAT_SEAR_STOPVIBEND:						/* ��~���U�����o�I��			*/
					tmpGainRatio = 50;							/* �Q�C��������50%�ɕύX		*/
				/* Fall Through */
				case ADAT_SEAR_VIBEND:							/* �U�����o�I��					*/
					AdatV->f.VibDetectinCycle = TRUE;			/* �U���X�e�[�^�XON				*/
					if( TuneAdatSetSpdCtrlGain( GAIN_DW, AdatP ) == ADAT_NG ) /* Kv�Q�C��DOWN	*/
					{
						AdatV->ErrState = ADAT_NG;
						return rc ;
					}
					TuneAdatSetPosCtrlGain( GAIN_BAL4, AdatP );	/* Kp�Q�C���v�Z					*/
					TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP );	/* Kpm�Q�C��DOWN				*/

					if( ( AdatV->f.UsedAllNtFil == TRUE ) && ( AdatV->f.UsedAResCtrl == TRUE) )
					{	/* ���Ƀm�b�`�t�B���^�ݒ�ς� �� �������s	*/
						/* �U���㏈�� */
						AdatGainDownforVibration( AdatV, AdatP, Prm->loophz, tmpGainRatio );
						KlibRstLongTimer( &AdatV->MfcTuneCntr );
						AdatV->MfcTunePtr = ADAT_MFC_WAIT;
						AdatV->NextSubSeqPtr = ADAT_MFC_CHK_REQ;
					}
					else
					{	/* �m�b�`�t�B���^�ݒ薢��	*/
						AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;
						AdatV->NextSeqPtr = ADAT_SEQ_ADJMFC;
						FunExe->State = ADAT_SEQ_NOTCHSET;
						AdatV->ErrState = ADAT_OK;					/* ����I��					*/
					}
					break;

				case ADAT_SEAR_COINBREAKEND:						/* COIN���ꌟ�o�I��			*/
					AdatV->f.VibDetectinCycle = TRUE;				/* �U���X�e�[�^�XON			*/
					AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;			/* �U�����Ă�����ăg���C	*/

					if( TuneAdatSetMdlCtrlGain( GAIN_DW, AdatP) == ADAT_NG  )	/* Kpm�Q�C��DOWN */
					{
						AdatV->ErrState = ADAT_NG;
						Mfc2MassReset( AdatV, AdatP );
						return rc ;
					}
					break;

				default :
					break;
			}
			/* ���莞�Ԃ̍ő�l��ێ����� */
			SettlingTime = BpiGetSettlingTime( AdatP->SettlingTime );
			if( AdatV->settling_max < SettlingTime )
			{
				AdatV->settling_max = SettlingTime;
			}

			/*----------------------------------------------------------------------------------*/
			/*	�^�]�p�^�[���I��																*/
			/*----------------------------------------------------------------------------------*/
			if( AdatV->PrgJogStat == ADAT_PJOG_FIN )
			{	/* �v���O����JOG�^�]���� */
				if( (AdatV->f.VibDetectinCycle == TRUE )
						|| ( AdatV->f.DrvPatternSet == FALSE ) )	/* �w�߃p�^�[��������		*/
				{	/* ����^�]���ɐU�����o����	*/
					AdatV->f.VibDetectinCycle = FALSE;				/* �U���X�e�[�^�X�N���A		*/
					AdatV->f.DrvPatternSet = FALSE;
					AdatV->PrgJogPatternReq = ADAT_PJOG_PATTERN3;	/* �v���O����JOG�^�]�p�^�[��3 */
					AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;			/* �U�����Ă�����ăg���C	  */
				}
				else
				{	/* ����^�]���U�����Ȃ����� �� �������� �� �I���ݒ� */
					if( ( AdatV->f.Used2MassMfc == TRUE )
						&& ( AdatV->f.Mfc2MassOK == FALSE )
						&& ( AdatV->f.MfcLastSet == FALSE )
						&& ( AdatV->f.Used2MassOld == FALSE ) )
					{
					/*--------------------------------------------------------------------------*/
					/*		Kv�����������(�ʏ�U�����x������V�[�P���X)�ŐU���}��ON�����ꍇ��	*/
					/*		����MFC�ł̐��莞�Ԋm�F����͍s��Ȃ�								*/
					/*--------------------------------------------------------------------------*/
						if(AdatV->Set2MassMfcSequence == ADAT_SEQ_ADJMFC)
						{	/* MFC�������ɐU���}���ݒ���s����					*/
							/* ���ݍ���+�@�䃂�f���ɂȂ��Ă��邩�ǂ����`�F�b�N	*/
							if( MFControl->conf.MFCModelSetting > ADATMFC_ONE_MASS )
							{
								FunExe->State = ADAT_SEQ_ADJMFC;			/* ����`���[�j���O�V�[�P���X�Z�b�g	*/
								PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
								edata = (USHORT)((Prm->mdlsw) & (RIGMFC_PRM_ACTIVE));	/* Pn140.1 -> 0 */
								TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)edata, pndef_mdlsw.DataLength );

								AdatV->MfcTunePtr   = ADAT_MFC_INI;			/* ����V�[�P���X�|�C���^�Z�b�g		*/
								AdatV->settling_buf = AdatV->settling_max;	/* ���莞�Ԃ��o�b�t�@				*/
								AdatV->Kpm_buf = (USHORT)Prm->mdlgn;		/* Kpm���o�b�t�@					*/
								AdatV->frq_buf = (USHORT)Prm->mdlantfrq;	/* ���U�E�����U���g�����o�b�t�@		*/
								AdatV->Kv_buf  = (USHORT)Prm->loophz;		/* Kv���o�b�t�@						*/
							}
							else
							{
								/* MFC(����+�@�䃂�f��)�̐��莞�Ԃ��Z���ꍇ�̓p�����[�^���Đݒ�	 */
								if( AdatV->settling_max > AdatV->settling_buf )
								{
									/* �p�����[�^��MFC(����+�@�䃂�f��)�ɕύX*/
									TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)AdatV->Kpm_buf, pndef_mdlgn.DataLength );			/* Pn141 */
									TunePrmWriteRam( &AccessPrm, &pndef_mdlantfrq, (LONG)AdatV->frq_buf, pndef_mdlantfrq.DataLength );	/* Pn145 */
									TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, (LONG)AdatV->frq_buf, pndef_mdlresfrq.DataLength );	/* Pn146 */

									PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
									edata =(USHORT)( ( Prm->mdlsw ) | ( VIBMFC_PRM_ACTIVE ) );
									TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)edata, pndef_mdlsw.DataLength );

									Prm->loophz = AdatV->Kv_buf;
									TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );
									TuneAdatSetPosCtrlGain( GAIN_BAL3, AdatP );
								}
								/*------------------------------------------------------------------------------*/
								/* �I������																		*/
								/*------------------------------------------------------------------------------*/
								/*	2����MFC�ݒ�ł̒����������B�ŏI�I�ɑI�������@�\���f���Ɋ֌W�Ȃ��A		*/
								/*	AdatV->f.Mfc2MassOK = TRUE�ƂȂ�											*/
								/*------------------------------------------------------------------------------*/
								AdatV->MfcTunePtr = ADAT_MFC_CHK_REQ;	/* COIN����m�F�v����					*/
								AdatV->f.VibDetectinCycle = TRUE;		/* 1��͑S�^�]�p�^�[���𓮍삳���邽�߁C*/
								AdatV->f.Mfc2MassOK = TRUE;
							}
						}
						else
						{
							AdatV->f.Mfc2MassOK = TRUE;					/* 2�����ŏI��������					*/
						}
					}
					else
					{
						AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;		/* �v���O����JOG�^�]��~				*/
						AdatScanCRequest( ADAT_NO_OPE, AdatV );			/* �����I����͑ҋ@��Ԃ�v��			*/
						AdatV->MfcTunePtr = ADAT_MFC_INI;				/* ����V�[�P���X�|�C���^�Z�b�g			*/
						FunExe->State = ADAT_SEQ_ENDDISP;				/* ����`���[�j���O�V�[�P���X�Z�b�g		*/
						AdatV->ErrState = ADAT_OK;						/* ����I��								*/
					}
				}
			}
			break;

		default :
			AdatV->ErrState = ADAT_NG;
			break;
	}
	return rc ;
}

/****************************************************************************************************
	���f�����[�v�Q�C���ύX����

	�T�v:���f�����[�v�Q�C����v���ɉ����ĕύX����B
****************************************************************************************************/
static SHORT	TuneAdatSetMdlCtrlGain( LONG mode, ADATP *AdatP )
{
	PRMDATA *Prm;
	REG_ACC_T AccessPrm;
	LONG	data;
	SHORT	rc;

	Prm = AdatP->UniPrms->Prm;
	rc  = ADAT_OK;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	switch ( mode )
	{
		case GAIN_UP:
			data = ADAT_GRAT_106( Prm->mdlgn );		/* 17/16 = 1.0625 6.25%����				*/
			break;

		case GAIN_DW:
			data = ADAT_GRAT_94( Prm->mdlgn );		/* 15/16 = 0.9375 6.25%����				*/
			break;

		default:
			data = Prm->mdlgn;						/* ����̂܂�							*/
			break;
	}

	if( data < KPM_UNLMT )
	{
		data = KPM_UNLMT;							/* �����l�𒴂���ꍇ�͉����l�Ƃ���		*/
		rc = ADAT_NG;								/* �ُ�I���Ƃ���						*/
	}
	else if( data > pndef_mdlgn.UpperLimit )
	{
		data = pndef_mdlgn.UpperLimit;				/* ����l�𒴂���ꍇ�̓N�����v�Ƃ���	*/
	}
	TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)data, pndef_mdlgn.DataLength );
	return rc;
}


/****************************************************************************************************
		MFC(����+�@�䃂�f��)�ݒ�ɖ߂�
		����MFC�Œ������A�U���̂��ߍŏ�Kpm�ɂȂ��Ă��܂����ꍇ�A�ݒ��2�����ɖ߂�
****************************************************************************************************/
static void	MfcPrmReSet( ADATV *AdatV, ADATP *AdatP )
{
	PRMDATA *Prm;
	USHORT	edata;
	REG_ACC_T AccessPrm;

	Prm = AdatP->UniPrms->Prm;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = AdatP->RegManager->hPnReg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* MFC(����+�@�䃂�f��)�����ς݂ł���΂����ݒ肷��	*/
	if( AdatV->f.MfcLastSet == TRUE )
	{
		/* �p�����[�^��MFC(����+�@�䃂�f��)�ɕύX*/
		/* Pn141 */
		TunePrmWriteRam( &AccessPrm, &pndef_mdlgn, (LONG)AdatV->Kpm_buf, pndef_mdlgn.DataLength );
		/* Pn145 */
		TunePrmWriteRam( &AccessPrm, &pndef_mdlantfrq, (LONG)AdatV->frq_buf, pndef_mdlantfrq.DataLength );
		/* Pn146 */
//		TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, (LONG)AdatV->Kpm_buf, pndef_mdlresfrq.DataLength );
		TunePrmWriteRam( &AccessPrm, &pndef_mdlresfrq, (LONG)AdatV->frq_buf, pndef_mdlresfrq.DataLength );//<S09E>

		PrmReLoadValueFromEeprom( AdatP->RegManager->hPnReg, &pndef_mdlsw, 0 );
		edata = (USHORT)( ( Prm->mdlsw ) | ( VIBMFC_PRM_ACTIVE ) );
		/* Pn140 */
//		TunePrmWriteRam( &AccessPrm, &pndef_ntchq1, (LONG)edata, pndef_ntchq1.DataLength );
		TunePrmWriteRam( &AccessPrm, &pndef_mdlsw, (LONG)edata, pndef_mdlsw.DataLength );//<S09E>

		Prm->loophz = AdatV->Kv_buf;
		TuneAdatSetSpdCtrlGain( GAIN_CAL, AdatP );
		TuneAdatSetPosCtrlGain( GAIN_BAL3, AdatP );
	}
	return;
}


/****************************************************************************************************
		MFC(����+�@�䃂�f��)�ݒ�ɖ߂�
****************************************************************************************************/
static void	Mfc2MassReset( ADATV *AdatV, ADATP *AdatP )
{
	/* MFC(����+�@�䃂�f��)�����ς݂ł����,����I��												*/
	/* ����MFC�Œ������A�U���̂��ߍŏ�Kpm�ɂȂ��Ă��܂����ꍇ�A�ݒ���Q�����ɖ߂�					*/
	if( ( AdatV->f.Used2MassMfc == TRUE )
		&& ( AdatP->BaseCtrlData->MFControl.conf.MFCModelSetting <= ADATMFC_ONE_MASS) )
		{
			/* ����I���ɕύX */
			AdatV->f.MfcLastSet       = TRUE;
			AdatV->MfcTunePtr         = ADAT_MFC_CHK_REQ;	/* C����m�F�v���� 						*/
			AdatV->f.VibDetectinCycle = TRUE;				/* �P��͑S�^�]�p�^�[���𓮍삳���邽�� */
			AdatV->ErrState           = ADAT_CONTINUE;		/* �p������								*/
		}
		return;
}

/******************************************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�I���ݒ菈��

	�T�v:	����I������шُ�I�����̏������s���B
			����I�����́A���������p�����[�^��EEPROM�ɏ������ށB
			�ُ�I�����́AEEPROM�ɂ͏������܂Ȃ��B
			�����I����opf.func_mode_clr�𗧂ĂāA�⏕�@�\���I������B
			�ύX�����p�����[�^�����ɖ߂��̂́Afunc_clear()�֐��ōs���B
******************************************************************************************************/
static PRM_RSLT	AdatFinishOpe( ADATREG *AdatReg, ADATV *AdatV, ADATP *AdatP, FUNEXE *FunExe, SHORT err_status )
{
	PJOGV *PJogV;
	PRM_RSLT rc;

	PJogV = &AdatP->BaseCtrlData->PJogHdl.PJogV;
	rc = PRM_RSLT_SUCCESS;
	AdatV->f.DrvErrDetect = FALSE;

	/* �G���[���̋��ʏ���																			*/
	if( err_status <= ADAT_NG )
	{	/* �G���[���� 																				*/
		if( AdatV->f.RefInType == FALSE )
		{
			AdatV->PrgJogPatternReq = ADAT_PJOG_STOP;		/* �v���O����JOG�^�]��~				*/
			PJogV->Var.RoutCmd = PJOGCMD_ABORT;				/* �v���O����JOG���~					*/
		}
		AdatV->ScanCRequest = ADAT_NO_OPE;					/* NOP�����v���Z�b�g					*/

		if( err_status > ADAT_COMERR )
		{	/* �G���[�R�[�h���ʐM�ُ���傫�� �� �T�[�{�I�t										*/
			AdatP->FnCmnCtrl->FnSvonReq = FALSE;			/* �ُ�I�����͂Ƃ肠�����A�T�[�{�I�t 	*/
			FunExe->State = ADAT_SEQ_WAIT;					/* �I���҂���Ԃ�					 	*/
		}
		else/* ������~��A�G���[�I��																*/
		{
			/* COIN���b�`���� or ���[�^��~���o														*/
			if( (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)
									|| (AdatP->SeqCtrlOut->MotStop == TRUE) )
			{/* COIN���b�`���� or ���[�^��~���o													*/
				FunExe->State = ADAT_SEQ_WAIT;					/* �I���҂���Ԃ�					*/
			}
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	�G���[����																					*/
	/*----------------------------------------------------------------------------------------------*/
	switch(err_status)
	{
		case ADAT_OK:											/* ����I��							*/
			AdatReg->AdatState = ADAT_STS_DONE;					/* �`���[�j���O����					*/
			if( AdatV->f.RefInType == FALSE )
			{
				AdatP->FnCmnCtrl->FnSvonReq = FALSE;			/* �`���[�j���O����I���ŃT�[�{�I�t	*/
			}

			switch ( FunExe->CmdCode )
			{
				case FCMD_EWRT:									/* EEPROM������ 					*/
					AdatUpdateGainInEeprom( AdatReg, AdatP );
					FunExe->State = ADAT_SEQ_WAIT;				/* ���݂̃V�[�P���X�|�C���^��ۑ� 	*/
					break;

				case FCMD_NONE :
				case FCMD_UP :
				case FCMD_DOWN :
					break;

				default:
					rc = PRM_RSLT_CONDITION_ERR;
					break;
			}
			break;

		case ADAT_JST_ERR:										/* �C�i�[�V������J�n�G���[			*/
		case ADAT_NOTRDYERR:									/* �^�]���������G���[				*/
			FunExe->State = ADAT_SEQ_WAIT;						/* ���݂̃V�[�P���X�|�C���^��ۑ�	*/
			AdatReg->AdatState = ADAT_STS_NG;					/* ����s�� 						*/
			break;

		case ADAT_J_ERR:										/* �C�i�[�V������G���[				*/
			FunExe->State = ADAT_SEQ_WAIT;						/* ���݂̃V�[�P���X�|�C���^��ۑ�	*/
			AdatReg->AdatState = ADAT_STS_JERR;					/* �C�i�[�V������s��				*/
			break;

		case ADAT_COMERR:										/* ���쒆�̒ʐM�G���[				*/
			if( (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)
				|| (AdatP->SeqCtrlOut->MotStop == TRUE) )
			{
				FunExe->State = ADAT_SEQ_WAIT;					/* ���݂̃V�[�P���X�|�C���^��ۑ�	*/
			}
			AdatReg->AdatState = ADAT_STS_ABORT;				/* ���쒆�ُ̈�ɂ�钆�f			*/
			break;

		default:												/* ���̑��̃G���[�̏ꍇ				*/
			FunExe->State = ADAT_SEQ_WAIT;						/* ���݂̃V�[�P���X�|�C���^��ۑ�	*/
			AdatReg->AdatState = ADAT_STS_ERR;					/* �`���[�j���O���s 				*/
			break;
	}
	return rc;
}

/****************************************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�X�L����C���C��

	�T�v:	�X�L����C�ł̃A�h�o���X�g�I�[�g�`���[�j���O�̃��C������
		 	�����v���ɂ��������āA�ȉ��̃`���[�j���O�������s���B
****************************************************************************************************/
void	CpxAdvancedAutoTuning( ADATHNDL	*AdatHndl, FUN_CMN_CONTROL *FnCmnCtrl )
{
	SHORT rc;
	ULONG coinOffTimeCnt;
	LONG  wk1,wk2,wk3;
	BOOL  Prefon;

	ADATV	*AdatV;
	ADATP	*AdatP;

	rc = ADAT_SEAR_NORMALEND;
	AdatV = &AdatHndl->AdatV;
	AdatP = &AdatHndl->AdatP;

	if( AdatV->f.GainTuneRun == TRUE )
	{
		AdatScanCSetReq( AdatV );								/* Round����̗v���Z�b�g			*/
		/*------------------------------------------------------------------------------------------*/
		/*	�U�����o�t���O�Z�b�g																	*/
		/*------------------------------------------------------------------------------------------*/
		AdatV->f.VibDetect = (AdatP->DetVib->var.VibDetFlg == TRUE) ? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	COIN���ꌟ�o�t���O�Z�b�g																*/
		/*------------------------------------------------------------------------------------------*/
		/* �ʒu���ߊ������s�ݐώ��Ԏ擾 															*/
		coinOffTimeCnt = BpiGetCoinOffTime( AdatP->SettlingTime );
		/* ���e���Ԓ��߂�COIN���ꌟ�o																*/
		AdatV->f.CoinBreak = (coinOffTimeCnt > COINOFFLMT) ? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	COIN���b�`�t���O�Z�b�g																	*/
		/*------------------------------------------------------------------------------------------*/
		AdatV->f.CoinLatch = ( AdatP->SettlingTime->StlgCoinLatch ) ? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	�I�[�o�[�V���[�g�ߑ�t���O�Z�b�g														*/
		/*------------------------------------------------------------------------------------------*/
		/*	���e�I�[�o�[�V���[�g���߂ŃI�[�o�[�V���[�g�G���[���o									*/
		AdatV->f.OvsErr = ( AdatP->SettlingTime->UnMaxOverShoot > AdatP->SettlingTime->OvsErrDetLvl )
																						? TRUE : FALSE;
		/*------------------------------------------------------------------------------------------*/
		/*	��~���U�����o����																		*/
		/*------------------------------------------------------------------------------------------*/
		/*	�n�C�p�X�t�B���^����																	*/
		wk3 = MlibHpfilter1( (LONG)AdatP->SeqMotSts->var.TrqRefMon, AdatV->KTrqHpFil, &AdatV->KTrqHpftmp );
		if( AdatP->SettlingTime->StlgCoinLatch == FALSE )
		{
			KlibRstLongTimer( &AdatV->CoinOnCntr );
		}
		if( KlibGetLongTimerMs( AdatV->CoinOnCntr ) >= STPDETTM )
		{	/* COIN���b�`��������100[ms]�o�� �� ��~��ԂƂ݂Ȃ� 									*/
			/* �ő�ŏ��U������																		*/
			if( wk3 >= AdatV->MaxTrqRefatStop )
			{
				AdatV->MaxTrqRefatStop = wk3;
			}
			else if( wk3 <= AdatV->MinTrqRefatStop )
			{
				AdatV->MinTrqRefatStop = wk3;
			}
			/*--------------------------------------------------------------------------------------*/
			/* ��~���U�����o																		*/
			/*	�ʏ�g���N�� STPVIBDET[%]�ȏォ�A��iSTPVIBLMT[%]�ȏ�Ō��o						*/
			/*--------------------------------------------------------------------------------------*/
			wk1 = AdatV->TrqRefatStop * STPVIBDET / 100;			/* ��~���U��臒l�g���N			*/
			wk2 = AdatV->MaxTrqRefatStop - AdatV->MinTrqRefatStop;	/* ��~���U���U��				*/
#if (FLOAT_USE==TRUE)
			if( wk2 < ( STPVIBLMT * AdatP->UniPrms->Bprm->Knortrq ) )
			{
				wk2 = 0;											/* 臒l�ȉ��Ȃ�U���U���N���A	*/
			}
#else
			if( wk2 < MlibMulgain( STPVIBLMT, AdatP->UniPrms->Bprm->Knortrq ) )
			{
				wk2 = 0;											/* 臒l�ȉ��Ȃ�U���U���N���A	*/
			}
#endif /* FLOAT_USE */
			/* 臒l�g���N���Ȃ��~���U�����o */
			AdatV->f.VibDetectatStop = (wk2 > wk1) ? TRUE : FALSE;
		}
		else
		{
			/* ��~���g���N�U��������	*/
			AdatV->MaxTrqRefatStop = wk3;							/* �����l�͌��݂̃g���N�w��		*/
			AdatV->MinTrqRefatStop = wk3;
			AdatV->f.VibDetectatStop = FALSE;						/* ��~���U�����o�t���O������	*/
		}

		/*------------------------------------------------------------------------------------------*/
		/*	�A�h�o���X�g�I�[�g�`���[�j���O�����V�[�P���X											*/
		/*------------------------------------------------------------------------------------------*/
		switch ( AdatV->ScanCRequest )
		{
			case ADAT_NO_OPE:						/* no operation 								*/
				rc  = AdatV->ScanCStat;				/* �O��X�e�[�^�X���Z�b�g						*/
				break;

			case ADAT_SEQ_NORMVIB:					/* �ʏ�U�����x������							*/
				rc = LpxAdatVibLvlSet( AdatV, AdatP );
				rc = LpxAdatCheckKv( AdatV, AdatP );/* �U�����x�����蒆���U���m�F���s��				*/
				break;

			case ADAT_SEQ_SEARCHOPTKV:				/* �œKKv�ATi�Q�C���T��							*/
				rc  = LpxAdatSearchKv( AdatV, AdatP );
				break;

			case ADAT_CHKKV:						/* �w�߉����ɂ����S�m�F						*/
				rc  = LpxAdatCheckKv( AdatV, AdatP );
				break;

			case ADAT_SEQ_KPSET:					/* Kp�ݒ蒲��									*/
				rc = LpxAdatKpGainSet( AdatV, AdatP );
				break;

			case ADAT_SEQ_SEARCHOPTKP:				/* �œKKp�Q�C���T��								*/
				rc = LpxAdatSearchKp( AdatV, AdatP );
				break;

			case ADAT_SEQ_ADJMFC:					/* ���f���Ǐ]���䒲��							*/
				rc = LpxAdatAdjustMfcGain( AdatV, AdatP );
				break;

			case ADAT_VIBCOIN:						/* ���f���Ǐ]����COIN����m�F					*/
				rc = LpxAdatCheckMfcGain( AdatV, AdatP );
				break;

			default:								/* �A�h�o���X�h�I�[�g�`���[�j���O����			*/
				rc = ADAT_SEAR_ERR;					/* �ُ�I���ɂ���								*/
				break;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	�G���[����																				*/
		/*------------------------------------------------------------------------------------------*/
		if( AdatV->ScanCRequest != ADAT_NO_OPE )
		{	/* �`���[�j���O��																		*/
			if( ( AdatV->f.RefInType == FALSE ) && ( FnCmnCtrl->FnSvonReq == FALSE ) )
			{	/* �T�[�{�I�t 																		*/
				rc = ADAT_SEAR_ERR;					/* �T���������ɃT�[�{�I�t������A�G���[�Ƃ���	*/
			}
		}

		/*------------------------------------------------------------------------------------------*/
		/*	����������																				*/
		/*------------------------------------------------------------------------------------------*/
		if((rc != ADAT_SEAR_CONTINUE) || (AdatV->ScanCRequest == ADAT_NO_OPE))
		{	/* �����p�����ȊO �܂��́ANOP															*/
			AdatV->ScanCRequest = ADAT_NO_OPE;		/* �����I����͑ҋ@��ԂɂȂ�					*/
													/* �������s�p�����邽�߂ɂ́Around����			*/
													/* �ēx�w�߂���									*/
			/* �����x�s�[�N�l�N���A																	*/
			DetVibResetMaxMinSpeedError( AdatP->DetVib );

			/* ��~���g���N�U��������																*/
			AdatV->MaxTrqRefatStop = wk3;						/* �����l�͌��݂̃g���N�w��			*/
			AdatV->MinTrqRefatStop = wk3;

			BpiRsetCoinOffTime( AdatP->SettlingTime );			/* COIN����ώZ���ԃN���A			*/
			BpiRsetMaxOverShoot( AdatP->SettlingTime );			/* �I�[�o�[�V���[�g�ʍő�l�N���A	*/
		}
		AdatV->ScanCStat = rc;											/* �����X�e�[�^�X�Z�b�g		*/
		AdatV->f.LastCoinLatch = (AdatV->f.CoinLatch)? TRUE : FALSE;	/* COIN���b�`�M���ۑ� 		*/
		/*------------------------------------------------------------------------------------------*/
		/*	�^�]�X�e�[�^�X�쐬����																	*/
		/*		�O���w�߂̏ꍇ�CCOIN�����Ȃ��w�߂̂悤�ȏꍇ�C										*/
		/*		Round�ł́CCOIN or �[���������o�ł��Ȃ��\�������邽��ScanC�ŏ�������				*/
		/*------------------------------------------------------------------------------------------*/
		if( AdatV->f.RefInType == TRUE )
		{
			Prefon = ( AdatP->SeqMotSts->var.dPcmdCnt != 0)? TRUE : FALSE;
			AdatV->PrgJogStat = LpxAdatRefInTypeDrvStatus( AdatHndl, AdatV->PrgJogPatternReq, Prefon );
		}
	}
}

/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�U�����x������

	�T�v:	�ʏ�^�]���̐U�����x���𑪒肷��B
			�ʏ�Q�C���ł̑��蒆�̔��U���o�p�����x�ő�l�𑪒肷��B
****************************************************************************/
static SHORT	LpxAdatVibLvlSet( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	LONG	lwork;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* �U�����x������															*/
	/*--------------------------------------------------------------------------*/
	lwork = AdatP->DetVib->DetVibObs.var.MaxSpdErr - AdatP->DetVib->DetVibObs.var.MinSpdErr;
	if( lwork >= AdatV->VibSpeed )
	{
		AdatV->VibSpeed = lwork;					/* �ő�U������				*/
	}

	/*--------------------------------------------------------------------------*/
	/*	��~���ʏ�g���N�U������												*/
	/*--------------------------------------------------------------------------*/
	if( KlibGetLongTimerMs( AdatV->CoinOnCntr ) >= STPDETTM )
	{	/* COIN���b�`��������100[ms]�o�� �� ��~��ԂƂ݂Ȃ�	*/
		lwork = AdatV->MaxTrqRefatStop - AdatV->MinTrqRefatStop;
		if( AdatV->TrqRefatStop < lwork )
		{
			AdatV->TrqRefatStop = lwork;			/* �ő�U��					*/
		}
	}
	return rc;
}


/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O �w�߉����ɂ����S�m�F

	�T�v:	�w�߉����ɂ��U���m�F���s���B
			�ʒu�w�ߒ��ɐU�����o���s���A�U��������A�Q�C����6.25%������B
			Ti�́AKv�ƘA��������B
****************************************************************************/
static SHORT	LpxAdatCheckKv( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* ��~���U�����o															*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.VibDetectatStop )
	{	/* ���U���o����	*/
		rc = ADAT_SEAR_STOPVIBEND;					/* ��~���U�����o�I��		*/
	}
	/*--------------------------------------------------------------------------*/
	/* ���U���o																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* �U�����o�I��				*/
	}
	return rc;
}


/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�œK�Q�C���T��

	�T�v:	�w�߉����ɂ��Kv�̍œK�l�����߂�B
			�ʒu�w�ߒ��ɐU�����o���s���A�U�����Ă��Ȃ���΁A
			Kv��2.0[Hz]���グ��B
			Ti�́Akv�ƘA��������B
****************************************************************************/
static SHORT	LpxAdatSearchKv( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc  = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* �Q�C���T���ő�l�ɓ��B�����ꍇ											*/
	/*--------------------------------------------------------------------------*/
	if( AdatP->UniPrms->Prm->loophz >= AdatV->MaxKv )
	{
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* �Q�C���T���J�E���^�N���A	*/
		rc = ADAT_SEAR_NORMALEND;					/* ����I��					*/
	}
	/*--------------------------------------------------------------------------*/
	/* ��~���U�����o															*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetectatStop )
	{	/* ���U���o����	*/
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* �Q�C���T���J�E���^�N���A	*/
		rc = ADAT_SEAR_STOPVIBEND;					/* ��~���U�����o�I��		*/
	}
	/*--------------------------------------------------------------------------*/
	/* ���U���o																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* �Q�C���T���J�E���^�N���A	*/
		rc = ADAT_SEAR_VIBEND;						/* �U�����o�I��				*/
	}
	else if( KlibGetLongTimerMs( AdatV->GainUpCntr ) > GNUPTM2 )
	{	/* ���莞�ԐU�����o���Ȃ����� �� �Q�C��UP	*/
		KlibRstLongTimer( &AdatV->GainUpCntr );		/* �Q�C���T���J�E���^�N���A	*/
		rc = ADAT_SEAR_NOVIBDETECT;					/* �U�������o �Q�C���t�o	*/
	}
	return rc;
}



/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�ő�Kp�T��

	�T�v:	Kp����莞�Ԗ���MaxKp�܂ŏグ��B
			�U�����o���s���A�U�����Ă���΃X�e�[�^�X��Ԃ��B
			COIN�M�������o�����琳��I���ƂȂ�B
****************************************************************************/
static SHORT	LpxAdatKpGainSet( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* ���U���o																	*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.VibDetect )
	{
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* �U�����o�I��				*/
	}
	/*--------------------------------------------------------------------------*/
	/* �U������																	*/
	/*--------------------------------------------------------------------------*/
	else if( KlibGetLongTimerMs( AdatV->KpGainUpCntr ) > GNUPTM2 )
	{	/* ���莞�ԐU�����o���Ȃ����� �� �Q�C��UP	*/
		KlibRstLongTimer( &AdatV->KpGainUpCntr );	/* �Q�C���T���J�E���^�N���A	*/
		rc = ADAT_SEAR_NOVIBDETECT;					/* �U�������o �Q�C��UP		*/
	}
	/*--------------------------------------------------------------------------*/
	/* �Q�C���T���ő�l�ɓ��B�����ꍇ											*/
	/*--------------------------------------------------------------------------*/
	if( (AdatP->UniPrms->Prm->posgn >= AdatV->MaxKp)/* �T��Kp�ő�l�ȏォ�H	*/
			|| (AdatV->f.CoinLatch == TRUE) )		 /* COIN�M�������o�������H	*/
	{
		KlibRstLongTimer( &AdatV->KpGainUpCntr );	/* �Q�C���T���J�E���^�N���A	*/
		rc = ADAT_SEAR_NORMALEND;					/* ����I��					*/
	}
	return rc;
}

/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O�œKKp�Q�C���T��

	�T�v:	�w�߉����ɂ��Kp�̍œK�l�����߂�B
			�ʒu���ߎ���COIN���ꌟ�o���s���ACOIN���ꋖ�e���Ԃ𒴂��Ă����
			�ʒu���ߎ���COIN���ꌟ�o���s���A���ꋖ�e���Ԃ𒴂��Ă���΁A
			Kp��3.0[1/s]��������B
			Kp������������Kv�Ƃ̃o�����X�����ꂽ�ꍇ�́AKv�������A�ăg���C�B
			���̂Ƃ�Kp�ATi�́AKv�ƘA��������B
****************************************************************************/
static SHORT	LpxAdatSearchKp( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* �I�[�o�[�V���[�g�G���[���o												*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.OvsErr )
	{	/* �I�[�o�[�V���[�g���e�l����	*/
		BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* �I�[�o�[�V���[�g�ʃN���A	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN���ꌟ�o�I��			*/
	}
	/*--------------------------------------------------------------------------*/
	/* COIN���ꌟ�o																*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.CoinBreak )
	{	/* COIN���ꋖ�e�l����	*/
		BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN����ώZ���ԃN���A	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN���ꌟ�o�I��			*/
	}
	/*--------------------------------------------------------------------------*/
	/* ���U���o																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{	/* ���U���o����	*/
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* �U�����o�I��				*/
	}
	return rc;
}

/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O���f���Ǐ]���䒲��

	�T�v:	�w�߉����ɂ��Kpm�̍ő�l�����߂�B
			�ʒu���ߎ���COIN���ꌟ�o���s���A�uCOIN����Ȃ��v�Ȃ�A
			Kpm��3.125�����グ��B
****************************************************************************/
static SHORT	LpxAdatAdjustMfcGain( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* �I�[�o�[�V���[�g�G���[���o												*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.OvsErr )
	{	/* �I�[�o�[�V���[�g���e�l����	*/
		BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* �I�[�o�[�V���[�g�ʃN���A	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN���ꌟ�o�I��			*/
	}
	/*--------------------------------------------------------------------------*/
	/* COIN���ꌟ�o																*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.CoinBreak )
	{	/* COIN���ꋖ�e�l����	*/
		BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN����ώZ���ԃN���A	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN���ꌟ�o�I��			*/
	}
	/*--------------------------------------------------------------------------*/
	/* ���U���o																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{	/* ���U���o����	*/
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* �U�����o�I��				*/
	}
	/*--------------------------------------------------------------------------*/
	/* Kpm UP																	*/
	/*--------------------------------------------------------------------------*/
	else if( (AdatV->f.CoinLatch == FALSE) && (AdatV->f.LastCoinLatch == TRUE) )
	{/* COIN�M��OFF�G�b�W���o */
		rc = ADAT_SEAR_COINOK;						/* �U�������COIN����Ȃ�	*/
	}
	return rc;
}

/****************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O���f���Ǐ]����COIN����m�F

	�T�v:	�w�߉����ɂ��Kpm�̍œK�l�����߂�B
			�ʒu���ߎ���COIN���ꌟ�o���s���A�uCOIN����v�Ȃ�A
			Kpm��3.0[1/s]��������B
****************************************************************************/
static SHORT	LpxAdatCheckMfcGain( ADATV *AdatV, ADATP *AdatP )
{
	SHORT	rc;
	rc = ADAT_SEAR_CONTINUE;

	/*--------------------------------------------------------------------------*/
	/* �I�[�o�[�V���[�g�G���[���o												*/
	/*--------------------------------------------------------------------------*/
	if( AdatV->f.OvsErr )
	{	/* �I�[�o�[�V���[�g���e�l����	*/
		BpiRsetMaxOverShoot( AdatP->SettlingTime );	/* �I�[�o�[�V���[�g�ʃN���A	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN���ꌟ�o�I��			*/
	}
	/*--------------------------------------------------------------------------*/
	/* COIN���ꌟ�o																*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.CoinBreak )
	{	/* COIN���ꋖ�e�l���� */
		BpiRsetCoinOffTime( AdatP->SettlingTime );	/* COIN����ώZ���ԃN���A	*/
		rc = ADAT_SEAR_COINBREAKEND;				/* COIN���ꌟ�o�I��			*/
	}
	/*--------------------------------------------------------------------------*/
	/* ���U���o																	*/
	/*--------------------------------------------------------------------------*/
	else if( AdatV->f.VibDetect )
	{	/* ���U���o����	*/
		DetVibResetMaxMinSpeedError( AdatP->DetVib );
		rc = ADAT_SEAR_VIBEND;						/* �U�����o�I��				*/
	}
	return rc;
}

/*****************************************************************************************
	�A�h�o���X�g�I�[�g�`���[�j���O����X�e�[�^�X�쐬����

	�T�v:	�^�]�w�߂��O���w�ߊ���g�p���ăv���O����JOG�𗘗p���Ȃ�
			�ꍇ�̉^�]�X�e�[�^�X���쐬����B
        	�����^�O���w�߂͖{�֐��ŋz������A���̃��W���[���ɂ͉e�����Ȃ�

			�^�]�w�߂̐���
			�ECOIN�M�����m�F��C����w�߂��J�n���邱�ƁBCOIN�M�����m�F���Ȃ��Ŏ���w�߂�
			  �����o���ƁC�`���[�j���O�V�[�P���X���ڂ�Ȃ�
			  COIN�M��ON��̒�~���Ԃ�100ms�ȉ��̏ꍇ�́C��~���U�����o���s���Ȃ�
			�E�^�]���x�C�����x�ɂ͐�����݂��Ȃ��B
			  �������A�C�i�[�V������C�Q�C���T�[�`���ʂ́C���x�C�����x�ɂ���Č��ʂ�
			  �e�������B

			�^�]��~    �E�E�E�t�B���^�O�ʒu�w�߃Z�O�����g=0��COIN���b�`��300[ms]
			�ʒu���ߊ����E�E�E�t�B���^�O�ʒu�w�߃Z�O�����g=0����COIN���b�`
			���^�t����  �E�E�E��~���ɐ��]�^�t�]��؂�ւ�
*****************************************************************************************/
static SHORT LpxAdatRefInTypeDrvStatus( ADATHNDL *AdatHndl, ULONG DrvPattern, BOOL PrefInFlag )
{
	SHORT	rc;
	ADATREG *AdatReg;
	ADATV	*AdatV;
	ADATP	*AdatP;

	AdatReg = &AdatHndl->AdatReg;
	AdatV = &AdatHndl->AdatV;
	AdatP = &AdatHndl->AdatP;
	rc = AdatV->PrgJogStat;					/* ���^�[���R�[�h��O��X�e�[�^�X�ɏ�����	*/

	switch ( AdatV->PrgJogPtr )
	{
		/*------------------------------------------------------------------------------*/
		/*	Initial process of Jog sequence 											*/
		/*------------------------------------------------------------------------------*/
		case 0:
			if( DrvPattern == ADAT_PJOG_STOP )
			{	/* �^�]��~�v������														*/
				rc = ADAT_PJOG_NOP;						/* �^�]��~��Ԃ�				*/
			}
			else
			{
				AdatV->f.DrvPatternSet = TRUE;
				AdatV->PositioningTimes = 0;			/* �ʒu���߉񐔃J�E���^�N���A	*/
				AdatV->PrgJogPtr = 1;					/* �V�[�P���X�|�C���^�X�V		*/
				rc = ADAT_PJOG_WAIT;					/* �^�]�ҋ@						*/
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	�^�]�J�n����					 											*/
		/*------------------------------------------------------------------------------*/
		case 1:
			if( (PrefInFlag == TRUE) || (AdatP->SeqCtrlOut->TgOnSts == TRUE) )
			{	/* �ړ��w�߂���															*/
				AdatV->PrgJogPtr = 2;					/* �ړ������҂���				*/
				rc = ADAT_PJOG_EXEC;
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	�ړ��w�ߊ����҂�				 											*/
		/*------------------------------------------------------------------------------*/
		case 2:
			if(( (AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE)
			  || (AdatP->SeqCtrlOut->MotStop) == TRUE) 	/* COIN���b�`����or���[�^��~���o */
			  || ((PrefInFlag == FALSE)
			  && (AdatV->f.StopDetectMode2 == TRUE) ) )	/* ��~���胂�[�h2���w�ߒ�~	*/
			{	/* �w�ߒ�~����															*/
				AdatV->PrgJogPtr = 3;					/* �^�]�����҂���				*/
			}
		/* Fall Through */	/* �u�^�]�����҂��v�Ɠ����^�C�~���O��COIN���ʂ���������break�Ȃ�*/
		/*------------------------------------------------------------------------------*/
		/*	�^�]�����҂�					 											*/
		/*------------------------------------------------------------------------------*/
		case 3:
			if(( AdatP->BaseCtrlData->PosCtrlSts.CoinSignal == TRUE )/* COIN���b�`���� */
			 || ((PrefInFlag == FALSE)
			 && (AdatV->f.StopDetectMode2 == TRUE)))	/* ��~���胂�[�h2���w�ߒ�~	*/
			{
				/* �^�]�����ʈʒu���ߊ����X�e�[�^�X�Z�b�g								*/
				/* �ʒu���ߊ������ɐ��]�^�t�]���Z�b�g����								*/
				AdatV->PositioningTimes++;				/* �ʒu���߉񐔃J�E���^�N���A	*/
				if( AdatV->PositioningTimes & 0x01 )
				{
					rc = ADAT_PJOG_PLUSEND;
				}
				else
				{
					rc = ADAT_PJOG_MINUSEND;
				}
				AdatV->CoinWaitStat = ADAT_STS_COINNOWAIT;
				AdatV->f.CoinWaitStart = FALSE;				/* COIN�o�͑҂������I��		*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );
				AdatV->PrgJogPtr = 4;						/* �E�F�C�g������			*/
				KlibRstLongTimer( &AdatV->CoinWaitCntr );
			}
			else
			{
				/*----------------------------------------------------------------------*/
				/*	COIN�o�͑҂�����(�ʒu����̂�)										*/
				/*----------------------------------------------------------------------*/
				if( AdatP->BaseCtrlData->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_POS )
				{	/* �ʒu���䃂�[�h 													*/
					if( (PrefInFlag == FALSE) &&			/* �w�߂Ȃ�					*/
						 (AdatV->f.CoinWaitStart == FALSE))	/* COIN�o�͑҂��������łȂ�	*/
					{
						KlibRstLongTimer( &AdatV->CoinWaitCntr );
						AdatV->CoinWaitStat = ADAT_STS_COINWAIT;
						AdatV->f.CoinWaitStart = TRUE;			/* COIN�o�͑҂������J�n	*/
					}
					if(AdatV->f.CoinWaitStart == TRUE)			/* COIN�o�͑҂�������	*/
					{
						if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT1 )
						{
							/* ��10�b�ʒu���ߊ������Ȃ��Ƃ���COIN�҂��G���[				*/
							AdatV->ErrState = ADAT_COINERR;
							AdatV->CoinWaitStat = ADAT_STS_COINERR;	/* �G���[�o��ON		*/
						}
						else if( KlibGetLongTimerMs( AdatV->CoinWaitCntr ) >= ADATCOINWAIT2 )
						{	/* COIN �o�͑҂��x����	*/
							AdatV->CoinWaitStat = ADAT_STS_COINWARN;/* �x���o��			*/
						}
					}
				}
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	�E�F�C�g						 											*/
		/*------------------------------------------------------------------------------*/
		case 4:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr ) >= ADATDRVWAIT )
			{	/* �^�]����																*/
				AdatV->PrgJogPtr = 5;						/* ����^�]������			*/
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	����^�]����					 											*/
		/*------------------------------------------------------------------------------*/
		case 5:
			if( ( AdatV->f.GainTuneRun == TRUE )			/* �`���[�j���O��			*/
				&& (AdatReg->AdatState != ADAT_STS_VIBDET) )/* �U�����x������ł͂Ȃ�	*/
			{	/* �`���[�j���O���́C��U�C�N���A����									*/
				DetVibResetMaxMinSpeedError( AdatP->DetVib );
			}
			if( (AdatV->PositioningTimes & 0x0004) != 0x0000 )
			{	/* 2���������B�^�]�����́C�U���m�F���m���ɍs�����߁C2�����ōs��			*/
				AdatV->PositioningTimes = 0;				/* �ʒu���߉񐔃J�E���^�N���A*/
				AdatV->PrgJogPtr = 6;						/* �^�]��~�E�F�C�g������	*/
				KlibRstLongTimer( &AdatV->PrgJogWaitCntr );	/* ���E���h�J�E���^�ێ�		*/
				rc = ADAT_PJOG_FIN;							/* �^�]�����X�e�[�^�X�Z�b�g	*/
			}
			else
			{
				AdatV->PrgJogPtr = 1;						/* �^�]�J�n������			*/
				rc = ADAT_PJOG_WAIT;						/* �^�]�ҋ@					*/
			}
			break;
		/*------------------------------------------------------------------------------*/
		/*	�^�]��~						 											*/
		/*------------------------------------------------------------------------------*/
		case 6:
			if( KlibGetLongTimerMs( AdatV->PrgJogWaitCntr ) >= ADATDRVWAIT )
			{	/* �^�]����																*/
				AdatV->PrgJogPtr = 0;		/* �^�]��~�B�V�[�P���X�������ݒ�ɃZ�b�g	*/
			}
			break;

		default:
			AdatV->ErrState = ADAT_PROGERR;	/* �ُ�I��									*/
			break;
	}
	return rc;
}


