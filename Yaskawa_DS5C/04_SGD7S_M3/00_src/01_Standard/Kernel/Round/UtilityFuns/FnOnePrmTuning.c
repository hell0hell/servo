/****************************************************************************
  Description   : One Parameter Tuning (Fn203)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.02.10   created

****************************************************************************/
#include "FnOnePrmTuning.h"
#include "MercuryGlobals.h"
#include "ModelControl.h"
#include "PnPrmListTbl.h"
#include "KLib.h"
#include "ExControls.h"

/***************************************************************************************************
	Local Function Prototype Definition
***************************************************************************************************/
static LONG	 onePrmANotchCheck( ONEPRMTUNE *OnePrmTune, DETVIB *DetVib, BOOL BaseEnable );
static LONG	 onePrmSetTuningGain( ONEPRMTUNE *OnePrmTune, PRMDATA *Prm, REG_ACC_T *AccessPrm,
																	BOOL IPSpdControl, BOOL DobsAct );
static LONG	 onePrmSetTuningNotch( ONEPRMTUNE *OnePrmTune, AUTONOTCH *ANotch,
														REG_ACC_T *AccessPrm, PRMDATA *Prm );
static LONG	 onePrmUpdateGainInEeprom( REGIF_CONFIG_T *RegCfg, BOOL DobsAct, LONG MfcModel );
static void onePrmUndoRamGain( REGIF_CONFIG_T *RegCfg );
static void onePrmInitializeTuningMode( ONEPRMTUNE *OnePrmTune, UCHAR CtrlMode, BOOL BaseEnable );
static void onePrmCheckTuningMode( ONEPRMTUNE *OnePrmTune );



/***************************************************************************************************
	Fn203 One Parameter Tuning mode Begin
***************************************************************************************************/
PRM_RSLT	FnEnterOnePrmTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT	rc;
	ONEPRMTUNE	*OnePrmTune;
	PRMDATA		*Prm;

	/* ���[�J���ϐ��̏����� */
	OnePrmTune = Axis->OnePrmTune;
	Prm = Axis->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	/* Initialize One Parameter Tuning */
	OnePrmTune->OpeReg.SpatLevel = Prm->loophz;
	OnePrmTune->OpeReg.SpatFFLevel = Prm->mdlgn;
	RpiRegSetOnePrmTuningLevel( OnePrmTune, Prm->loophz );
	RpiRegSetOnePrmTuningFfLevel( OnePrmTune, Prm->mdlgn );

	if( Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{ /* �������X�L���� */
		rc = PRM_RSLT_CONDITION_ERR;
	}

	/* ���[�^���X�e�X�g���[�h�łȂ��ꍇ�͓��炸�ɏI������ *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Get Original Gains */
	OnePrmTune->var.OrgKv    = Prm->loophz;		/* Pn100 : ���x���[�v�Q�C�� 			*/
	OnePrmTune->var.OrgMdlgn = Prm->mdlgn;		/* Pn141 : ���f���Ǐ]����Q�C��[0.1/s]	*/
	OnePrmTune->var.OrgNf1   = Prm->ntchfil1;	/* Pn409 : 1�i�ڃm�b�`�t�B���^���g�� 	*/
	OnePrmTune->var.OrgNf2   = Prm->ntchfil2;	/* Pn40C : 2�i�ڃm�b�`�t�B���^���g�� 	*/
	OnePrmTune->var.OrgNfQ1  = Prm->ntchq1;		/* Pn40A : 1�i�ڃm�b�`�t�B���^Q�l 		*/
	OnePrmTune->var.OrgNfQ2  = Prm->ntchq2;		/* Pn40D : 1�i�ڃm�b�`�t�B���^Q�l 		*/
	OnePrmTune->var.Jrate    = Prm->jrate;		/* Pn103 : �������[�����g�� 			*/

	/* Get Atype Resvib Gain */
	OnePrmTune->var.OrgAResf   = Prm->avibfrq;	/* Pn161 : �`�^���U���g�� 				*/
	OnePrmTune->var.OrgAResDGn = Prm->avibdamp;	/* Pn163 : �`�^���U�_���s���O�Q�C�� 	*/

	/* Reset One Parameter Tuning Gain */
	OnePrmReSetTuningLevel( OnePrmTune,
							Prm->loophz,
							Prm->posgn,
							Axis->BaseControls->CtrlCmdPrm.IPSpdControl );

	OnePrmTune->OpeReg.SpatFFLevel = OnePrmTune->var.OrgMdlgn;
	OnePrmTune->var.FfSetting      = OnePrmTune->var.OrgMdlgn;
	OnePrmTune->var.TunRun         = TRUE;

	/* Fn���[�h�����I���Ȃ� */
	FunExe->HoldFnMode = TRUE;

	return(rc);
}


/***************************************************************************************************
	Fn203 One Parameter Tuning mode End
****************************************************************************************************/
void	FnLeaveOnePrmTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	REGIF_CONFIG_T	*RegCfg;
	ONEPRMTUNE		*OnePrmTune;

	RegCfg     = Axis->RegManager->hPnReg;
	OnePrmTune = Axis->OnePrmTune;

	/* EEPROM�f�[�^�ŕύX�p�����[�^�����ɖ߂� */
	onePrmUndoRamGain( RegCfg );
	MlibResetLongMemory( &(OnePrmTune->var), sizeof(OnePrmTune->var)/4 );
}

/***************************************************************************************************
	Fn203 One Parameter Tuning mode Execution
***************************************************************************************************/
PRM_RSLT	FnExecuteOnePrmTuning( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	ULONG			ulwork;
	SHORT			swk;
	USHORT			KvGain;
	USHORT			VibFreq;
	REG_ACC_T		AccessPrm;
	ONEPRMTUNE		*OnePrmTune;
	BASE_CTRL_HNDL	*pBaseCtrl;
	REGIF_CONFIG_T	*RegCfg;
	PRMDATA			*Prm;

	/* ���[�J���ϐ��̏����� */
	OnePrmTune = Axis->OnePrmTune;
	pBaseCtrl  = Axis->BaseControls;
	RegCfg     = Axis->RegManager->hPnReg;
	Prm        = Axis->UniPrms->Prm;
	rc         = PRM_RSLT_SUCCESS;

	/* RAM�������ݗp���W�X�^���ݒ� */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* �U�����o�g���[�X�v������ */
	OnePrmTune->var.FftStat = TuneVibrationTrace( Axis->DetVib,
												  Axis->FftAna,
												  OnePrmTune->var.FftReq,
												  0,
												  TuneLessGetTuneLessSts(&Axis->BaseControls->TuneLessCtrl) );

	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) == 0x0000 )
	{ /* �����ݒ�\�ȃm�b�`���Ȃ� */
		/* �����ݒ�s�X�e�[�^�X�ݒ� */
		OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
	}

	OnePrmTune->OpeReg.SpatANotchState = (OnePrmTune->var.ANotchPreStat << 12)
									   + (OnePrmTune->var.ANotch2Stat 	<<  8)
									   + (OnePrmTune->var.ANotch1Stat 	<<  4)
									   + (OnePrmTune->var.ANotchStat         );

	/*------------------------------------------------------------------------------------------*/
	/*	���W�X�^�����f�[�^�쐬																	*/
	/*------------------------------------------------------------------------------------------*/
	if( ((Prm->avibsw & 0x0F) == TRUE)
		|| (((Prm->avibsw >> 4) & 0x0F) == FALSE)
		|| ((pBaseCtrl->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ) && (Axis->SeqCtrlOut->BaseEnableReq == TRUE)) )
	{ /* ���ɂ`�^���U�L�� || ������������ || (�g���N���䃂�[�h && �T�[�{�I��) */
		/* �����ݒ�s�ݒ� */
		OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOENABLE;
	}
	else if( OnePrmTune->var.AResStat == OPTUNE_STS_ARES_NOENABLE)
	{ /* �`�^���U�L���������������A�X�e�[�^�X���ݒ�s�� */
		/* �����o�ɏ����� */
		OnePrmTune->var.AResStat  = OPTUNE_STS_ARES_NOVIBDET;
		OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NODETECT;
	}
	/* Store Ares State on the Register */
	OnePrmTune->OpeReg.SpatAresState = (OnePrmTune->var.ARes1Stat << 4) + OnePrmTune->var.AResStat;

	switch( FunExe->State )
	{
	case OPTUNE_SEQ_INIT:
		/*--------------------------------------------------------------------------------------*/
		/*	�����m�b�`���s��������(������)														*/
		/*--------------------------------------------------------------------------------------*/
		if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) != 0x0000 )
		{ /* �����m�b�`�ݒ�\ */
			/* ���U���o���}�X�N(�v�Z�֐��K�v) */
			//Prm->vibsel = 0;
			TunePrmWriteRam( &AccessPrm, &pndef_vibsel, 0, pndef_vibsel.DataLength );/*<S09E>*/
		}
		else
		{ /* �����m�b�`�ݒ�s���� */
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
		}

		/*--------------------------------------------------------------------------------------*/
		/*	�����`�^���U�ݒ�\�`�F�b�N(������)												*/
		/*--------------------------------------------------------------------------------------*/
		/* �����o�ɐݒ� */
		OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
		OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NODETECT;

		if( ((Prm->avibsw & 0x0F) == TRUE)
			|| (((Prm->avibsw >> 4) & 0x0F) == FALSE)
			|| ((pBaseCtrl->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ) && (Axis->SeqCtrlOut->BaseEnableReq == TRUE)) )
		{	/* ���ɂ`�^���U�L�� || ������������ || (�g���N���䃂�[�h && �T�[�{�I��) */
			/* �����ݒ�s�ݒ�@*/
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOENABLE;
		}
		else
		{
			/* ���U���o���}�X�N(�v�Z�֐��K�v) */
			//Prm->vibsel = 0;
			TunePrmWriteRam( &AccessPrm, &pndef_vibsel, 0, pndef_vibsel.DataLength );
		}
		/* �U�����o�p�����x�ő�l�E�ŏ��l���Z�b�g */
		DetVibResetMaxMinSpeedError( Axis->DetVib );
		DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );

		TuneFilGainCalbyFilType( Axis->ANotch, (UCHAR)OnePrmTune->OpeReg.OnePrmFilType );
		onePrmSetTuningGain( OnePrmTune,
							 Prm,
							 &AccessPrm,
							 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
							 pBaseCtrl->DobsCtrl.var.DobsAct );
		onePrmSetTuningNotch( OnePrmTune,
							  Axis->ANotch,
							  &AccessPrm,
							  Prm );
		FunExe->State = OPTUNE_SEQ_TUNING;

		/* FF�ݒ�p�ϐ����Z�b�g */
		KlibRstLongTimer( &OnePrmTune->var.FfWaitTimer );
		OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFCOMP;
		break;

	case OPTUNE_SEQ_TUNING:		/* �`���[�j���O���x���ύX����	*/
		if( OnePrmTune->var.FftWait == TRUE )
		{
			if( KlibGetLongTimerMs(OnePrmTune->var.FftWaitTimer) > 2000 )
			{
				OnePrmTune->var.FftWait = FALSE;
				DetVibResetMaxMinSpeedError( Axis->DetVib );
			}
		}

		if( OnePrmTune->var.NotchChk == TRUE )
		{ /* �m�b�`�`�F�b�N�� */
			if( KlibGetLongTimerMs(OnePrmTune->var.NotchChkTimer) > 3000 )
			{
				OnePrmTune->var.NotchChk = FALSE;
				OnePrmTune->var.ANotchSetStat = OPTUNE_STS_NOTCH_INIT;
				OnePrmTune->var.AResSetStat = OPTUNE_STS_ARES_INIT;
			}
		}

		if( OnePrmTune->var.ANotchSetStat > 0x02 )
		{ /* �G���[��� */
			if( KlibGetLongTimerMs( OnePrmTune->var.NotchChkTimer ) > 2000 )
			{
				/* ERR�\���̃N���A */
				OnePrmTune->var.ANotchSetStat = OPTUNE_STS_NOTCH_INIT;
			}
		}

		if( OnePrmTune->var.AResSetStat >= 0x10 )
		{ /* �G���[��� */
			if( KlibGetLongTimerMs( OnePrmTune->var.NotchChkTimer ) > 2000 )
			{
				/* ERR�\���̃N���A */
				OnePrmTune->var.AResSetStat = OPTUNE_STS_ARES_INIT;
			}
		}

		/* ���g����̓X�^�[�g*/
		if( onePrmANotchCheck( OnePrmTune, Axis->DetVib, Axis->SeqCtrlOut->BaseEnableReq ) == OK )
		{
			OnePrmTune->var.FftReq = TRUE;
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_DETECT;
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_DETECT;
			FunExe->State = OPTUNE_SEQ_VIBSEL;
		}

		/*----------------------------------------------------------------------------------------------*/
		/*	�O���I�u�U�[�o�ݒ�ɂ���																	*/
		/*		���[�h2�́C�������ɊO���O���I�u�U�[�o�L���Ƃ���(�ݒ�̓����p���o�����X)�B				*/
		/*		���̑��̃��[�h�́C�@�\�X�C�b�`ON�Ń����p���o�����X�̊O���I�u�U�[�o�L���Ƃ���B			*/
		/*----------------------------------------------------------------------------------------------*/
		/*	FF���x���ύX�X�e�[�^�X�쐬�������^�C���A�E�g����											*/
		/*		�^�C���A�E�g�ł́CFF�̐ݒ�l��j�����C���ݎ��s���̐ݒ�ɋ����I�ɖ߂��B					*/
		/*		���������Ƃ��ẮCCOIN��ON����܂�FF�ݒ�l(�\���l)�̃p�����[�^�v�Z���́C				*/
		/*		�����s���Ă��Ȃ��̂ŕ\�����FF���x���̂݌��ɖ߂��Ηǂ�									*/
		/*----------------------------------------------------------------------------------------------*/
		if( OnePrmTune->OpeReg.SpatFFLevel != OnePrmTune->var.FfSetting )
		{ /* �\���Ɠ���̃��x�����s��v */
			switch( OnePrmTune->OpeReg.SpatMode )
			{ /* �`���[�j���O���x�� */
			case 2: /* �ʒu���ߐ�p */
				//TuneMfcActive( Prm );
				TuneMfcActive( Prm, AccessPrm.hRegCfg );/*<S09E>*/
				break;
			case 3: /* �ʒu���ߐ�p(�I�[�o�[�V���[�g�d��) */
				//TuneMfcActiveStMode( Prm );
				TuneMfcActiveStMode( Prm, AccessPrm.hRegCfg );/*<S09E>*/
				break;
			default:
				break;
			}

			/* �`���[�j���O�f�[�^�ݒ� */
			onePrmSetTuningGain( OnePrmTune,
								 Prm,
								 &AccessPrm,
								 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
								 pBaseCtrl->DobsCtrl.var.DobsAct );
			onePrmSetTuningNotch( OnePrmTune,
								  Axis->ANotch,
								  &AccessPrm,
								  Prm );
			/* FF�ύX�����҂��X�e�[�^�X�́C"���s"���̂� */
			OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFWAIT;
		}

		if( MfcGetParamEnableSts( &(pBaseCtrl->MFControl) ) == TRUE )
		{
			/* FF���x���\���𓮍�p�ɔ��f */
			OnePrmTune->OpeReg.SpatFFLevel = OnePrmTune->var.FfSetting;
			if( OnePrmTune->OpeReg.SpatLvlChgState == OPTUNE_STS_FFWAIT )
			{
				/* FF�ύX�����҂��X�e�[�^�X�́C"���s"���̂� */
				OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFCOMP;
			}
		}
		else
		{
			if( KlibGetLongTimerMs( OnePrmTune->var.FfWaitTimer ) > 8000 )
			{ /* FF���x���ύX��C8sec�o�� */
				/* FF�ύX���s(�^�C���A�E�g) */
				OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFTMOUT;
				/* FF���x�������ɖ߂� */
				OnePrmTune->var.FfSetting = OnePrmTune->OpeReg.SpatFFLevel;
				/* �`���[�j���O�f�[�^�ݒ� */
				onePrmSetTuningGain( OnePrmTune,
									 Prm,
									 &AccessPrm,
									 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
									 pBaseCtrl->DobsCtrl.var.DobsAct );
				onePrmSetTuningNotch( OnePrmTune,
									  Axis->ANotch,
									  &AccessPrm,
									  Prm );
			}
		}

		switch( FunExe->CmdCode )
		{
		case FCMD_STOP:		/* ��~ */
			ulwork = (ULONG)Prm->tfuncsw;

			switch( OnePrmTune->var.ANotchPreStat )
			{ /* �m�b�`�����X�e�[�^�X */
			case OPTUNE_PSTS_NT1_COMP:		/* �O��1�i�ڃm�b�`ON */
				ulwork &= 0x0000FFF0;									/* ��1�m�b�`OFF						*/
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;	/* �U�������o�X�e�[�^�X�ݒ�			*/
				OnePrmTune->var.ANotch1Stat = OPTUNE_STS_NT1_NODETECT;	/* 1�i�ڃX�e�[�^�X�𖢎��{�ɏ�����	*/
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;	/* ���O�m�b�`�𖢎��{�ɏ�����		*/
				break;

			case OPTUNE_PSTS_NT2_COMP:		/* �O��2�i�ڃm�b�`ON	*/
				ulwork &= 0x0000F0FF;									/* ��2�m�b�`OFF						*/
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;	/* �U�������o�X�e�[�^�X�ݒ�			*/
				OnePrmTune->var.ANotch2Stat = OPTUNE_STS_NT2_NODETECT;	/* 2�i�ڃX�e�[�^�X�𖢎��{�ɏ�����	*/
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;	/* ���O�m�b�`�𖢎��{�ɏ�����		*/
				break;

			default:
				break;
			}

			/* Pn408 �g���N�֌W�@�\�X�C�b�` */
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, ulwork, 2 );

			/* �`���[�j���O�f�[�^�ݒ�	*/
			onePrmSetTuningGain( OnePrmTune,
								 Prm,
								 &AccessPrm,
								 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
								 pBaseCtrl->DobsCtrl.var.DobsAct );
			onePrmSetTuningNotch( OnePrmTune,
								  Axis->ANotch,
								  &AccessPrm,
								  Prm );
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_EXEC:		/* �Q�C���ύX */
			onePrmInitializeTuningMode( OnePrmTune,
										pBaseCtrl->CtrlModeSet.CtrlMode.b.cm,
										Axis->SeqCtrlOut->BaseEnableReq );
			onePrmCheckTuningMode( OnePrmTune );
			if( OnePrmTune->var.ModeUnmatch == FALSE )
			{
				onePrmSetTuningGain( OnePrmTune,
									 Prm,
									 &AccessPrm,
									 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
									 pBaseCtrl->DobsCtrl.var.DobsAct );
				onePrmSetTuningNotch( OnePrmTune,
									  Axis->ANotch,
									  &AccessPrm,
									  Prm );
				KlibRstLongTimer( &OnePrmTune->var.FfWaitTimer );
			}
			else
			{
				rc = PRM_RSLT_CONDITION_ERR;
			}
			break;

		case FCMD_EXEC2:
		case FCMD_SV:
			/*------------------------------------------------------------------------------------------*/
			/*	�����m�b�`���s��������																	*/
			/*------------------------------------------------------------------------------------------*/
			if( (OnePrmTune->var.ANotchStat != OPTUNE_STS_NT_NOENABLE)
				|| (OnePrmTune->var.AResStat != OPTUNE_STS_ARES_NOENABLE) )
			{ /* �����m�b�`���s�\ or �����`�^���s�\ */
				/*--------------------------------------------------------------------------------------*/
				/*	���g����͏������s����																*/
				/*--------------------------------------------------------------------------------------*/
				if( Axis->SeqCtrlOut->BaseEnableReq == TRUE )
				{ /* �T�[�{�I�����̂ݎ��s */
					OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_DETECT;	/* �U�����o���X�e�[�^�X�ݒ�		*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_DETECT;	/* �U�����o���X�e�[�^�X�ݒ�		*/
					OnePrmTune->var.FftReq = TRUE;						/* ���g����͏����J�n�v��		*/
					FunExe->State = OPTUNE_SEQ_VIBSEL;					/* �U���}���ݒ�t�F�[�Y�ֈڍs	*/
					rc = PRM_RSLT_CONTINUE;
				}
				else
				{	/* �T�[�{�I�t���̏ꍇ */
					rc = PRM_RSLT_CONDITION_ERR;
				}
			}
			break;

		case FCMD_EWRT:		/* EEPROM������ */
			if( onePrmUpdateGainInEeprom(RegCfg,
										 pBaseCtrl->DobsCtrl.var.DobsAct,
//										 pBaseCtrl->MFControl.conf.MFCModelSetting) != TRUE )
										 pBaseCtrl->MFControl.conf.MFCModelSetting) != PRM_RSLT_SUCCESS )/* <S085> */
			{ /* EEPROM�����ݏ��� */
				rc = PRM_RSLT_CONDITION_ERR;
			}
			OnePrmTune->var.FftReq = FALSE;
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
			break;

		case FCMD_NONE:
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	case OPTUNE_SEQ_VIBSEL:		/* �U���}���ݒ� */
		if( OnePrmTune->var.FftStat == TUNE_FREQ_SEARCHEND )
		{
			/* FFT��͗v��OFF */
			OnePrmTune->var.FftReq = FALSE;

			/* �U�����g���擾 */
			VibFreq = FftGetAnaRsltVibmRf( Axis->FftAna );

			if( (OnePrmTune->var.AResStat != OPTUNE_STS_ARES_NOENABLE)
				&& (VibFreq <= AVIB_FRQ_MAX)
				&& (OnePrmTune->OpeReg.SpatLevel < TUNE_ANOTCH_MINKV) )
			{ /* �`�^���U�ݒ�\ && ���g�����`�^���U����ȉ� && �m�b�`ON�ŏ�Kv�ȉ� */
				/* �m�b�`�ݒ�ŏ�Kv�ݒ�		*/
				KvGain = TUNE_ANOTCH_MINKV;
			}
			else
			{
				/* FB���x���ݒ�l */
				KvGain = OnePrmTune->OpeReg.SpatLevel;
			}

			/* �����m�b�`�t�B���^�ݒ���Z�b�g */
			TuneSetNotchFilSettingData( Axis->ANotch, VibFreq, KvGain, Prm->jrate );
			/* ��1�s�[�N���g��	*/
			swk = TuneNotchFilterSetting( Axis->ANotch, Prm, RegCfg );

			/* �m�b�`�ݒ�X�e�[�^�X�Z�b�g */
			OnePrmTune->var.ANotchSetStat = (UCHAR)swk;
			/* �`���[�j���O�f�[�^�ݒ�	*/
			onePrmSetTuningGain( OnePrmTune,
								 Prm,
								 &AccessPrm,
								 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
								 pBaseCtrl->DobsCtrl.var.DobsAct );
			onePrmSetTuningNotch( OnePrmTune,
								  Axis->ANotch,
								  &AccessPrm,
								  Prm );

			switch( OnePrmTune->var.ANotchSetStat & 0x0F )
			{
			case OPTUNE_STS_NOTCH1_OK:		/* 1�i�ڃm�b�`�ݒ萳�튮��	*/
				OnePrmTune->var.ANotch1Stat = OPTUNE_STS_NT1_COMP;
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NT1_COMP;
				break;

			case OPTUNE_STS_NOTCH2_OK:		/* 2�i�ڃm�b�`�ݒ萳�튮��	*/
				OnePrmTune->var.ANotch2Stat = OPTUNE_STS_NT2_COMP;
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NT2_COMP;
				break;

			case OPTUNE_STS_NOTCHQ1_ERR:	/* 1�i�ڃm�b�`Q�l�ُ�		*/
				OnePrmTune->var.ANotch1Stat = OPTUNE_STS_NT1_NG;
				break;

			case OPTUNE_STS_NOTCHQ2_ERR:	/* 2�i�ڃm�b�`Q�l�ُ�		*/
				OnePrmTune->var.ANotch2Stat = OPTUNE_STS_NT2_NG;
				break;

			default:
				break;
			}

			/*------------------------------------------------------------------------------------------*/
			/*	�����m�b�`���s��������																	*/
			/*------------------------------------------------------------------------------------------*/
			if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) == 0x0000 )
			{ /* �����ݒ�\�ȃm�b�`���Ȃ� */
				/* �����ݒ�s�X�e�[�^�X�ݒ� */
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
			}
			else
			{
				/* �U�������o�X�e�[�^�X�ݒ� */
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;
			}

			if( (OnePrmTune->var.ANotchSetStat == OPTUNE_STS_NOTCH1_OK)
				|| (OnePrmTune->var.ANotchSetStat == OPTUNE_STS_NOTCH2_OK) )
			{
				/* �m�b�`�`�F�b�N��Ԃ֑J�� */
				OnePrmTune->var.NotchChk = TRUE;
				OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
				FunExe->State = OPTUNE_SEQ_TUNING;
			}
			else
			{ /* �m�b�`�ݒ莸�s */
				/*--------------------------------------------------------------------------------------*/
				/*	�����`�^���U																		*/
				/*--------------------------------------------------------------------------------------*/
				OnePrmTune->var.AResSetStat = (UCHAR)TuneATypeResSet( VibFreq,
																	  ((Prm->avibsw >> 4) & 0x0F),
																	  Prm,
																	  RegCfg );/*<S09E>*/

				/* ���W�X�^�����쐬		*/
				switch( OnePrmTune->var.AResSetStat & 0xFF )
				{
				case OPTUNE_STS_ARES_OK:	/* �`�^���U�ݒ萳�튮��	*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
					OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_COMP;
					/* �m�b�`�`�F�b�N��Ԃ� */
					OnePrmTune->var.NotchChk = TRUE;
					FunExe->State = OPTUNE_SEQ_TUNING;
					break;
				case 0x10:					/* �`�^���U�㉺���ُ�	*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
					OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NG;
					break;
				case 0x11:					/* �`�^���U�����ݒ�s��	*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOENABLE;
					break;
				default:
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
					OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NG;
					break;
				}
				if( (OnePrmTune->var.AResSetStat & 0xFF) != OPTUNE_STS_ARES_OK )
				{	/* �m�b�`/�`�^�Ƃ���NG */
					OnePrmTune->var.FftWait = TRUE;
					KlibRstLongTimer( &OnePrmTune->var.FftWaitTimer );
					FunExe->State = OPTUNE_SEQ_NOTCHERR;
				}
			}
			KlibRstLongTimer( &OnePrmTune->var.NotchChkTimer );
		}
		else if( OnePrmTune->var.FftStat == TUNE_FREQ_SEARCHERR )
		{ /* �G���[���� */
			OnePrmTune->var.FftReq = FALSE;
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_DET_ERR;		/* ���o�G���[�X�e�[�^�X�ݒ�			*/
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_DET_ERR;		/* ���o�G���[�X�e�[�^�X�ݒ�			*/
			FunExe->State = OPTUNE_SEQ_TUNING;
		}
		else
		{
			rc = PRM_RSLT_CONTINUE;
		}
		break;

	case OPTUNE_SEQ_NOTCHERR: /* �m�b�`�t�B���^/�`�^������Ȃ������Ƃ� */
		FunExe->State = OPTUNE_SEQ_TUNING;
		break;

	default:
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	����g�U�����o																				*/
	/*	  ���o�̑Ώۂ́C�O���I�u�U�[�o�L�����̃C�i�[�V����ݒ�ɂ��ʒu���[�v�̔��U�B				*/
	/*	  ���̂��ߌ��o���x���͍����ݒ肵�Ă���														*/
	/*	  �U�����o���́C�U���A���[��(A.520)���o�͂���												*/
	/*	�@�����ŏ������Ȃ��Ə����������ŐU�����x�����N���A�����O�ɐU�����o���Ă��܂�				*/
	/*----------------------------------------------------------------------------------------------*/
	if( (pBaseCtrl->DobsCtrl.var.DobsAct == TRUE)
		&& (DetVibGetSpdErrPeakFn(Axis->DetVib) > (2 * DetVibGetVibCheckLevel(Axis->DetVib))) )
	{ /* �O���I�u�U�[�o�L�� && ���U���x���~2�𒴂����ꍇ */
		ALMSetServoAlarm( Axis->AlmManager, ALM_AT );
		DetVibResetMaxMinSpeedError( Axis->DetVib );
		DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );
	}

	return(rc);
}



/*****************************************************************************************************
	�`���[�j���O���x���Đݒ菈��
*****************************************************************************************************/
void	OnePrmReSetTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT loophz, USHORT posgn, BOOL IPSpdControl )
{
	ULONG	Kv, Kp, Kp2Kv;
	ULONG	da, db;

	Kv = (ULONG)loophz;
	Kp = (ULONG)posgn;

	switch( OnePrmTune->OpeReg.SpatMode )
	{
		case 0:	/* ���萫�d�� */
			/********************************/
			/* �����W���̌v�Z				*/
			/*			 1   Kv				*/
			/*	   ��a = - ��--  * 100		*/
			/*			 2   Kp				*/
			/********************************/
			da = MlibSqrtu32(Kv * 2500) / MlibSqrtu32(Kp);
			if( IPSpdControl == TRUE )
			{ /* I-P���䎞 */
				db = 133;
				Kp2Kv = Kp * 228 / 100;
			}
			else
			{ /* PI���䎞 */
				db = 125;
				Kp2Kv = Kp;
			}

			if( da >= db )
			{ /* ��a �� ��b */
				OnePrmTune->OpeReg.SpatLevel = (USHORT)Kp2Kv;
			}
			else
			{ /* ��a < ��b */
				OnePrmTune->OpeReg.SpatLevel = (USHORT)Kv;
			}
			break;

		case 1:	/* �������d�� */
		case 2:	/* �ʒu���ߐ�p */
		case 3:	/* �ʒu���ߐ�p(OV) */
		default:
			OnePrmTune->OpeReg.SpatLevel = (USHORT)Kv;
			break;
	}	
	return;
}


/*****************************************************************************************************
	Fn203 Write Gain To Ram With EEPROM Data
*****************************************************************************************************/
static void	onePrmUndoRamGain( REGIF_CONFIG_T *RegCfg )
{
	/* RAM�̒l��EEPROM�̒l�Ɠ����ɂ��� */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_loophz,   0 );	/* Pn100 ���x���[�v�Q�C�� 					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pitime,   0 );	/* Pn101 ���x���[�v�ϕ����萔				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_posgn,	   0 );	/* Pn102 �ʒu���[�v�Q�C��					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_jrate,    0 );	/* Pn103 �������[�����g��					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_trqfil11, 0 );	/* Pn401 1�i�ڑ�1�g���N�w�߃t�B���^���萔 	*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw,  0 );	/* Pn408 �g���N�֌W�@�\�X�C�b�` 			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );	/* Pn409 1�i�ڃm�b�`�t�B���^���g�� 			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1,   0 );	/* Pn40A 1�i�ڃm�b�`�t�B���^Q�l				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );	/* Pn40C 2�i�ڃm�b�`�t�B���^���g��			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2,   0 );	/* Pn40D 1�i�ڃm�b�`�t�B���^Q�l				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibsw,   0 );	/* Pn160 ���U����֘A�X�C�b�`				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq,  0 );	/* Pn161 A�^���U���g��						*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp, 0 );	/* Pn163 A�^���U�_���s���O�Q�C��			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobgn,    0 );	/* Pn121 ���C�⏞�Q�C��						*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dtrqgn,   0 );	/* Pn123 ���C�⏞�W��						*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dlpfil,   0 );	/* Pn124 ���C�⏞���g���␳					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobjgn,   0 );	/* Pn125 ���C�⏞�Q�C���␳					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlsw,    0 );	/* Pn140 ���f���Ǐ]����֘A�X�C�b�`			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlgn,    0 );	/* Pn141 ���f���Ǐ]����Q�C��				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlzeta,  0 );	/* Pn142 ���f���Ǐ]����Q�C���␳			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlptff,  0 );	/* Pn143 ���f���Ǐ]����o�C�A�X(���]����)	*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlntff,  0 );	/* Pn144 ���f���Ǐ]����o�C�A�X(�t�]����)	*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlvff,   0 );	/* Pn147 ���f���Ǐ]���䑬�x�t�B�[�h�t�H���[�h�⏞ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_vibsel,   0 );	/* Pn310 �U�����o�X�C�b�`					*/

	return;
}


/*****************************************************************************************************
	Fn203 One Parameter Tuning mode Initial Set
*****************************************************************************************************/
static void onePrmInitializeTuningMode( ONEPRMTUNE *OnePrmTune, UCHAR CtrlMode, BOOL BaseEnable )
{
	if( (CtrlMode != CTRLMODE_POS) && (BaseEnable == TRUE) )
	{
		OnePrmTune->var.ModeMin = 0;
		OnePrmTune->var.ModeMax = 1;
//		OnePrmTune->OpeReg.SpatMode = 0;/*<S09E>*/
	}
	else
	{
		OnePrmTune->var.ModeMin = 0;
		OnePrmTune->var.ModeMax = OPTUNE_MODE_MAX;
//		OnePrmTune->OpeReg.SpatMode = 2;/*<S09E>*/
	}
}


/*****************************************************************************************************
	Fn203 One Parameter Tuning mode Condition Check
*****************************************************************************************************/
static LONG onePrmANotchCheck( ONEPRMTUNE *OnePrmTune, DETVIB *DetVib, BOOL BaseEnable )
{
	LONG	rc;

	if( ((OnePrmTune->var.ANotchStat != OPTUNE_STS_NT_NOENABLE)
	 || (OnePrmTune->var.AResStat != OPTUNE_STS_ARES_NOENABLE))
	 && (OnePrmTune->var.FftWait == FALSE)
	 && (OnePrmTune->var.NotchChk == FALSE)
	 && (DetVibGetSpdErrPeak(DetVib) > DetVibGetVibCheckLevel(DetVib))
	 && (BaseEnable == TRUE) )
	{ /* (�����m�b�`���s�\ || �����`�^���s�\)
		  && !�����m�b�`���s�҂� && !�m�b�`�m�F�� && �U�������o���x���ȏ� && �T�[�{�I���� */
		rc = OK;
	}
	else
	{
		rc = NG;
	}
	return( rc );
}


/*****************************************************************************************************
	Fn203 Set One Parameter Tuning Gain
*****************************************************************************************************/
static LONG	onePrmSetTuningGain( ONEPRMTUNE *OnePrmTune, PRMDATA *Prm, REG_ACC_T *AccessPrm,
																BOOL IPSpdControl, BOOL DobsAct )
{
	LONG		rc;
	LONG		kx, sx;
	ULONG		ulwork;
	TUNECALV	TuneCalV;

	/* ���[�J���ϐ��̏����� */
	rc = OK;

	/*------------------------------------------------------------------------------*/
	/*	�v�Z�p�����[�^�̐ݒ�														*/
	/*------------------------------------------------------------------------------*/
	switch( OnePrmTune->OpeReg.SpatMode )
	{
	case 1:
		if( IPSpdControl == TRUE )
		{ /* I-P���䎞 */
			TuneCalV.PIRate = 1;
			TuneCalV.KpZeta = 100;
			TuneCalV.Ti = 200 * 70 * 70;
		}
		else
		{ /* PI���䎞 */
			TuneCalV.KpZeta = 70;
		}
		break;
	case 0:
	case 2:
	case 3:
	default:
		if( IPSpdControl )
		{ /* I-P���䎞 */
			TuneCalV.PIRate = 1;
			TuneCalV.KpZeta = 228;
			TuneCalV.Ti = 200 * 100 * 100;
		}
		else
		{ /* PI���䎞 */
			TuneCalV.KpZeta = 100;
		}
		break;
	}

	/*------------------------------------------------------------------------------*/
	/*	Pn101, Pn102, Pn121�̌v�Z													*/
	/*------------------------------------------------------------------------------*/
	if( (OnePrmTune->OpeReg.SpatMode == 2) || (OnePrmTune->OpeReg.SpatMode == 3) )
	{
		/* Pn101 */
		TuneCalV.Gain.pitime = TuneKvFuncTi( OnePrmTune->OpeReg.SpatLevel,
											 IPSpdControl );
		/* Pn102 */
		TuneCalV.Gain.posgn = TuneKvFuncKp( OnePrmTune->OpeReg.SpatLevel,
											TuneCalV.Gain.pitime,
											IPSpdControl );
		/* Pn121 */
		TuneCalV.Gain.dobgn = TuneKvFuncDobsgn( OnePrmTune->OpeReg.SpatLevel );
		/* Pn123, Pn125, Pn408 */
		//TuneDobsActive( Prm );
		TuneDobsActive( Prm, AccessPrm->hRegCfg );//<S09E>

		/* �O���I�u�U�[�o�L�� */
		DobsAct = TRUE;/*<S09E>*/
	}
	else
	{
		if( IPSpdControl == TRUE )
		{ /* I-P���䎞 */
			/* Pn102 */
			kx = MlibScalKxgain( OnePrmTune->OpeReg.SpatLevel, 100, TuneCalV.KpZeta, &sx, 0 );
			kx = MlibPcalKxgain( kx, TuneCalV.PIRate, 1, &sx, 24);
			TuneCalV.Gain.posgn = (USHORT)MlibMulgain( 1, kx );

			/* Pn101 */
			kx = MlibScalKxgain( TuneCalV.Ti, 1000, 3141, &sx, 0 );
			kx = MlibPcalKxgain( kx, 1, OnePrmTune->OpeReg.SpatLevel, &sx, 24 );
			TuneCalV.Gain.pitime = (USHORT)MlibMulgain( 1, kx );
		}
		else
		{ /* PI���䎞 */
			/* Pn102 */
			kx = MlibScalKxgain( OnePrmTune->OpeReg.SpatLevel, 100, TuneCalV.KpZeta, NULL, 24 );
			TuneCalV.Gain.posgn = (USHORT)MlibMulgain( 1, kx );

			/* Pn101 */
			kx = MlibPcalKxgain( 800000, 1, OnePrmTune->OpeReg.SpatLevel, NULL, 24 );
			TuneCalV.Gain.pitime = (USHORT)MlibMulgain( 1, kx );
		}

		/*--------------------------------------------------------------------------*/
		/*	�Q�C���㉺���`�F�b�N													*/
		/*--------------------------------------------------------------------------*/
		/* Pn101 */
		if( TuneCalV.Gain.pitime > TUNE_MAX_TI )
		{
			TuneCalV.Gain.pitime = TUNE_MAX_TI;
		}
		TuneCalV.Gain.pitime = TunePrmLimitUnsigned( TuneCalV.Gain.pitime, &pndef_pitime );

		/* Pn102 */
		TuneCalV.Gain.posgn = TunePrmLimitUnsigned( TuneCalV.Gain.posgn, &pndef_posgn );

		if( DobsAct == TRUE )
		{
			TuneCalV.Gain.dobgn  = TuneKvFuncDobsgn( OnePrmTune->OpeReg.SpatLevel );
			//TuneDobsActive( Prm );
			TuneDobsActive( Prm, AccessPrm->hRegCfg );//<S09E>
		}
	}

	/*------------------------------------------------------------------------------*/
	/*	RAM�ւ̏�����																*/
	/*------------------------------------------------------------------------------*/
//	rc = RpiPrmWriteRamForFn( &pndef_loophz, OnePrmTune->OpeReg.SpatLevel, Hmsg );
//	if(rc != OK)	{return(rc);}
	/* Pn100 */
	TunePrmWriteRam( AccessPrm, &pndef_loophz, OnePrmTune->OpeReg.SpatLevel, 2 );

//	rc = RpiPrmWriteRamForFn( &pndef_pitime, TuneCalV.Gain.pitime, Hmsg );
//	if(rc != OK)	{return(rc);}
	/* Pn101	*/
	TunePrmWriteRam( AccessPrm, &pndef_pitime, TuneCalV.Gain.pitime, 2 );

//	rc = RpiPrmWriteRamForFn( &pndef_posgn, TuneCalV.Gain.posgn, Hmsg );
//	if(rc != OK)	{return(rc);}
	/* Pn102 */
	TunePrmWriteRam( AccessPrm, &pndef_posgn, TuneCalV.Gain.posgn, 2 );

	if( DobsAct == TRUE )
	{
//		rc = RpiPrmWriteRamForFn( &pndef_dobgn, TuneCalV.Gain.dobgn, Hmsg );
//		if(rc != OK)	{return(rc);}
		/* Pn121 */
		TunePrmWriteRam( AccessPrm, &pndef_dobgn, TuneCalV.Gain.dobgn, 2 );
	}

	if( (OnePrmTune->OpeReg.SpatMode == 2) || (OnePrmTune->OpeReg.SpatMode == 3) )
	{
		/* Pn141 */
//		rc = RpiPrmWriteRamForFn( &pndef_mdlgn, OnePrmTune->var.FfSetting, Hmsg );
//		if(rc != OK)	{return(rc);}
		TunePrmWriteRam( AccessPrm, &pndef_mdlgn, OnePrmTune->var.FfSetting, 2 );

		/* Pn140 */
		ulwork = Prm->mdlsw;
		ulwork |= OPTUNE_MFC_ACTIVE;
		TunePrmWriteRam( AccessPrm, &pndef_mdlsw, ulwork, 2 );
//		rc = RpiPrmWriteRamForFn( &pndef_mdlsw, (USHORT)ulwork, Hmsg );	
//		if(rc != OK)	{return(rc);}
	}
	else
	{
		/* Pn140 */
		ulwork = Prm->mdlsw;
		ulwork = ulwork & ~OPTUNE_MFC_ACTIVE;
		TunePrmWriteRam( AccessPrm, &pndef_mdlsw, ulwork, 2 );
//		rc = RpiPrmWriteRamForFn( &pndef_mdlsw, (USHORT)ulwork, Hmsg );
//		if(rc != OK)	{return(rc);}
	}

	return(rc);
}


/*****************************************************************************************************
	Fn203 Set One Parameter Tuning Notch
*****************************************************************************************************/
static LONG	onePrmSetTuningNotch( ONEPRMTUNE *OnePrmTune, AUTONOTCH *ANotch, 
												REG_ACC_T *AccessPrm, PRMDATA *Prm )
{
	LONG		rc;
	TUNECALV	TuneCalV;

	/* ���[�J���ϐ��̏����� */
	rc = OK;

	/*------------------------------------------------------------------------------*/
	/*	�v�Z�p�����[�^�̐ݒ�														*/
	/*------------------------------------------------------------------------------*/
	/* �m�b�`�p�l & �g���N�w�߃t�B���^�ݒ�	*/
	TuneNotchQ( ANotch,
				AccessPrm->hRegCfg,
				OnePrmTune->OpeReg.SpatLevel,
				OnePrmTune->var.Jrate,
				0,
				Prm );
	/* �ݒ茋�ʎ擾 */
	TuneGetNotchQSetting( ANotch,
						  &TuneCalV.Gain.ntchq1,
						  &TuneCalV.Gain.ntchq2,
						  &TuneCalV.Gain.trqfil11 );

	/*------------------------------------------------------------------------------*/
	/*	�㉺���`�F�b�N																*/
	/*------------------------------------------------------------------------------*/
	/* Pn401 */
	TuneCalV.Gain.trqfil11 = TunePrmLimitUnsigned( TuneCalV.Gain.trqfil11, &pndef_trqfil11 );

	/*------------------------------------------------------------------------------*/
	/*	RAM�ւ̏�����																*/
	/*------------------------------------------------------------------------------*/
//	rc = RpiPrmWriteRamForFn( &pndef_trqfil11, TuneCalV.Gain.trqfil11, Hmsg );
//	if(rc != OK)	{return(rc);}
	/* Pn401 */
	TunePrmWriteRam( AccessPrm, &pndef_trqfil11, TuneCalV.Gain.trqfil11, 2 );

//	rc = RpiPrmWriteRamForFn( &pndef_ntchq1, TuneCalV.Gain.ntchq1, Hmsg );
//	if(rc != OK)	{return(rc);}
	/* Pn40A */
	TunePrmWriteRam( AccessPrm, &pndef_ntchq1, TuneCalV.Gain.ntchq1, 2 );

//	rc = RpiPrmWriteRamForFn( &pndef_ntchq2, TuneCalV.Gain.ntchq2, Hmsg );
//	if(rc != OK)	{return(rc);}
	/* Pn40D */
	TunePrmWriteRam( AccessPrm, &pndef_ntchq2, TuneCalV.Gain.ntchq2, 2 );

	return(rc);
}



/*****************************************************************************************************
	Fn203 Update One Parameter Tuning Gain In EEPROM
*****************************************************************************************************/
static LONG	onePrmUpdateGainInEeprom( REGIF_CONFIG_T *RegCfg, BOOL DobsAct, LONG MfcModel )
{
	LONG	rc;

	/* Pn100 ���x���[�v�Q�C�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_loophz, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn101 ���x���[�v�ϕ����萔 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_pitime, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn102 �ʒu���[�v�Q�C�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_posgn, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn103 �������[�����g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_jrate, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn140 ���f���Ǐ]����֘A�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn401 1�i�ڑ�1�g���N�w�߃t�B���^���萔 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn408 �g���N�֌W�@�\�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn409 �P�i�ڃm�b�`�t�B���^���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40A 1�i�ڃm�b�`�t�B���^Q�l */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40C 2�i�ڃm�b�`�t�B���^���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40D 2�i�ڃm�b�`�t�B���^Q�l */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	if( DobsAct )
	{
		/* Pn121 ���C�⏞�Q�C�� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn123 ���C�⏞�W�� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dtrqgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn124 ���C�⏞���g���␳ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dlpfil, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn125 ���C�⏞�Q�C���␳ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobjgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}
	}

	if( MfcModel != 0 )
	{
		/* Pn141 ���f���Ǐ]����Q�C�� */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

//		rc = RpiFunPrmNvmemUpdate( &pndef_mdlzeta );	/* Pn142	*/
		/* Pn142 ���f���Ǐ]����Q�C���␳ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlzeta, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn143 ���f���Ǐ]����o�C�A�X(���]����) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlptff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn144 ���f���Ǐ]����o�C�A�X(�t�]����) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlntff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn147 ���f���Ǐ]���䑬�x�t�B�[�h�t�H���[�h�⏞ */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlvff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}
	}

//	rc = RpiFunPrmNvmemUpdate( &pndef_avibfrq );		/* Pn 161	*/
	/* Pn161 �`�^���U���g�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfrq, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn163 �`�^���U�_���s���O�Q�C�� */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibdamp, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn160 ���U����֘A�X�C�b�` */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}


	return(rc);
}



/************************************************************************************************/
/*		�`���[�j���O���[�h�ݒ�m�F����															*/
/************************************************************************************************/
static void	onePrmCheckTuningMode( ONEPRMTUNE *OnePrmTune )
{
	/*------------------------------------------------------------------------------------------*/
	/*	�`���[�j���O���[�h��������																*/
	/*------------------------------------------------------------------------------------------*/
	if( (OnePrmTune->OpeReg.SpatMode < OnePrmTune->var.ModeMin)
		|| (OnePrmTune->OpeReg.SpatMode > OnePrmTune->var.ModeMax) )
	{	/* �`���[�j���O���[�h�ݒ肪�@�\�����ƍ���Ȃ� */
		OnePrmTune->var.ModeUnmatch = TRUE;
	}
	else
	{
		OnePrmTune->var.ModeUnmatch = FALSE;
	}
	return;
}



/************************************************************************************************/
/*		�`���[�j���O���[�h�ݒ菈�� (���W�X�^IF)													*/
/************************************************************************************************/
PRM_RSLT	RpiFunSetOnePrmTuningMode( ONEPRMTUNE *OnePrmTune, USHORT Mode )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/*------------------------------------------------------------------------------------------*/
	/*	�`���[�j���O���[�h�㉺���`�F�b�N														*/
	/*------------------------------------------------------------------------------------------*/
	if( ((SHORT)Mode >= OPTUNE_MODE_MIN) && (Mode <= OPTUNE_MODE_MAX) )
	{
		OnePrmTune->var.ModeUnmatch = FALSE;
		OnePrmTune->OpeReg.SpatMode = Mode;
	}
	else
	{
		OnePrmTune->var.ModeUnmatch = TRUE;
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}



/************************************************************************************************/
/*		�`���[�j���O���x���ݒ菈�� (���W�X�^IF)													*/
/************************************************************************************************/
PRM_RSLT	RpiFunSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl )
{
	PRM_RSLT	rc;
	USHORT		LvlLimit;
	rc = PRM_RSLT_SUCCESS;

	/* �㉺���l�m�F */
	LvlLimit = TunePrmLimitUnsigned( Lvl, &pndef_loophz );

	if( LvlLimit == Lvl )
	{ /* Lvl���㉺���l���̏ꍇ */
		OnePrmTune->OpeReg.SpatLevel = Lvl;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}



/************************************************************************************************/
/*		�`���[�j���O���x���ݒ菈�� (���W�X�^IF)													*/
/************************************************************************************************/
PRM_RSLT	RpiFunSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl )
{
	PRM_RSLT	rc;
	USHORT		LvlLimit;
	rc = PRM_RSLT_SUCCESS;

	/* �㉺���l�m�F */
	LvlLimit = TunePrmLimitUnsigned( Lvl, &pndef_mdlgn );

	if( LvlLimit == Lvl )
	{ /* Lvl���㉺���l���̏ꍇ */
		OnePrmTune->var.FfSetting = Lvl;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}



/************************************************************************************************/
/*		�`���[�j���O���x��(Pn100�̒l)���� 														*/
/************************************************************************************************/
void	RpiRegSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain )
{
	OnePrmTune->OpeReg.SpatLevel = Gain;
	return;
}



/************************************************************************************************/
/*		�`���[�j���O���x��(Pn141�̒l)����														*/
/************************************************************************************************/
void	RpiRegSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain )
{
	if( OnePrmTune->var.TunRun == FALSE )
	{
		OnePrmTune->OpeReg.SpatFFLevel = Gain;
	}
	return;
}



/****************************************************************************************************/
