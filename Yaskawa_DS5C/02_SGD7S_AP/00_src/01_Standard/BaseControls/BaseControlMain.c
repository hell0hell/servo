/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControlMain.c : �T�[�{�x�[�X�R���g���[�����C��������`									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	����	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "BaseControlMain.h"
#include "SerialEncIf.h"
#include "PosManager.h"
#include "ExControls.h"
#include "GainChange.h"
#include "MotorPoleFind.h"
#include "TableDrive.h"


/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	InitScanBData( AXIS_HANDLE* );															*/
/*	void	PrstScanBData( AXIS_HANDLE* );															*/
/*	void	KpxInputScanB( AXIS_HANDLE* );															*/
/*	void	BpiScanBMain( AXIS_HANDLE* );															*/
/*	void	KpxOutputScanB( AXIS_HANDLE* );															*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		Initialize ScanB 																			*/
/*																									*/
/****************************************************************************************************/
void	InitScanBData( AXIS_HANDLE *AxisRscB )
{
//	MENCV				*MencV;
	BASE_CTRL_HNDL		*BaseControls;
	SERVO_CONTROL_IF	*SvControlIf;
//	POS_MNG_HNDL		*PosManager;

	/* ���[�J���ϐ��̏����� */
//	MencV	= AxisRscB->MencV;
	BaseControls	= AxisRscB->BaseControls;
	SvControlIf		= AxisRscB->SvControlIf;
//	PosManager		= &(BaseControls->PosManager);


/*--------------------------------------------------------------------------------------------------*/
/*		�|�C���^������																		 <S004> */
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->MencV = AxisRscB->MencV;
	BaseControls->FencV = AxisRscB->FencV;															/* <S04D> */
	
/*--------------------------------------------------------------------------------------------------*/
/*		���[�^�X�e�[�^�X������																	*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(BaseControls->MotSts), sizeof(BaseControls->MotSts)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{��{����ϐ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* �ʒu����ϐ������� */
//	MlibResetLongMemory( &(AxisRscB->hPosCtrl->var), sizeof(AxisRscB->hPosCtrl->var)/4 );
	/* ���x����ϐ������� */
//	MlibResetLongMemory( &(AxisRscB->hSpdCtrl->var), sizeof(AxisRscB->hSpdCtrl->var)/4 );
	/* �g���N�t�B���^�ϐ������� */
//	MlibResetLongMemory( &(BaseControls->TrqFilter.var), sizeof(BaseControls->TrqFilter.var)/4 );
	/* �[������~����ϐ������� */
	MlibResetLongMemory( &(BaseControls->ZeroClampCtrl.var), sizeof(BaseControls->ZeroClampCtrl.var)/4 );
	/* �T�[�{�Q�C���ؑ֗p�ϐ������� */
	GselInitServoGainChange( &(BaseControls->GainChange) );
#if (CSW_SVCF_EHVOBS_USE != 0)
	/* �ʑ��⏞���x�I�u�U�[�o�ϐ������� */
	EhSpeedObserverInitialize( &(BaseControls->EhVobsCtrl) );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{����ʒu�Ǘ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_SVCF_BACKLASHCOMP != 0)
	/* �o�b�N���b�V�␳���� */
	BlshInitializeCompensate( &(BaseControls->PosManager.BlshCmp) );
#endif
	/* �ʒu�Ǘ��ϐ����������� */
	PosMngInitPositionManager( BaseControls );
	InitBasePositionControl( &(AxisRscB->BaseLoops->PosCtrl) );		/* <S14C> */
	
	/* �ʒu�w�ߍ����o��[Pulse/Scan]�̃N���A */
	BaseControls->CtrlCmdMngr.dPosRefo = 0;
/* <S1D0> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		���x�t�B�[�h�t�H���[�h�ړ����ϋ@�\�p�ϐ�������														*/
/*--------------------------------------------------------------------------------------------------*/
	FFCmdFilInitialize( &(BaseControls->VFFcmdFil) );

/*--------------------------------------------------------------------------------------------------*/
/*		�g���N�t�B�[�h�t�H���[�h�ړ����ϋ@�\�p�ϐ�������														*/
/*--------------------------------------------------------------------------------------------------*/
	FFCmdFilInitialize( &(BaseControls->TFFcmdFil) );
/* <S1D0> End */
/*--------------------------------------------------------------------------------------------------*/
/*		�e�[�u���^�]���s����������																	*/
/*--------------------------------------------------------------------------------------------------*/
	TblDriveInitialize( &(BaseControls->TblDrive), AxisRscB->TableRefBuffer, AxisRscB->TrcHndl );

/*--------------------------------------------------------------------------------------------------*/
/*		�T�[�{�g������ϐ�������																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_SVCF_DISTURBOBS != 0)
	/* �O���I�u�U�[�o�ϐ������� <S09B> */
	DisturbObserverInitialize( &(BaseControls->DobsCtrl), &(BaseControls->GainChange.GselDobs[0]) );
#endif

	/* �������X�֌W�ϐ������� */
	TuneLessCtrlInitialize( &(BaseControls->TuneLessCtrl) );

#if( CSW_SVCF_VIB_DETECT_USE != 0 )
	/* �U�����o���Z�p�ϐ������� */
	DetVibObsInitialize( AxisRscB->DetVib );
#endif

#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	/* ���U����ϐ������� */
	ResVibATypeResVibInitialize( &(BaseControls->ResVib) );
#endif

	/* �U�����o�I�u�U�[�o�ɂ�鑬�x�΍��g���[�X������ */
	DetVibTraceInitialize( AxisRscB->DetVib );


	/* NET����R�[�h�ϐ������� */
	SvControlIf->NetCtrlMcmd = CTRLMODE_NOCMD;
	BaseControls->TrqLimitData.var.FwTrqLmt = NORMAXVALUE;
	BaseControls->TrqLimitData.var.RvTrqLmt = NORMAXVALUE;

/*--------------------------------------------------------------------------------------------------*/
/*		���莞�ԑ���֘A�����ϐ�������																*/
/*--------------------------------------------------------------------------------------------------*/
	/* Initialize Settling Time Variable and Start Settling Time Process */
	StlgInitSettlingTime( &(AxisRscB->BaseControls->SettlingTime),
						  AxisRscB->UniPrms->Bprm->SvCycleUs );

/*--------------------------------------------------------------------------------------------------*/
/*		����p�����[�^�ϊ��T�[�r�X������															*/
/*--------------------------------------------------------------------------------------------------*/
	TranlateCtrlPrmInitialize( &(AxisRscB->BaseControls->TransduceCtrl) );

/*--------------------------------------------------------------------------------------------------*/
/*		���̑����������ϐ�������(�b��)																*/
/*--------------------------------------------------------------------------------------------------*/
	AxisRscB->PowerManager->MainPowerChecker.conf.TestMode1 = FALSE;
	AxisRscB->PowerManager->MainPowerChecker.conf.TestMode2 = FALSE;
	AxisRscB->PowerManager->MainPowerChecker.conf.TestMode3 = FALSE;
	AxisRscB->PowerManager->MainPowerChecker.conf.TestMode4 = FALSE;
	AxisRscB->PowerManager->MainPowerChecker.conf.TestMode5 = FALSE;

	AxisRscB->SvControlIf->NetTrqFFC = 0;
	AxisRscB->SvControlIf->NetSpdFFC = 0;
}




/****************************************************************************************************/
/*																									*/
/*		ScanB Main Program																			*/
/*																									*/
/****************************************************************************************************/
void	BpiScanBMain( AXIS_HANDLE *AxisRscB, LONG ax_no )/* <S1C0> */
{
//	LONG				lwk;/*<S0C7>*/
	ALMID_T				AlarmCode;
	BOOL				PoleDetectReq;
	MPFEXEINFO			MpfExeCondition;
//	ASIC_U_IF_STR		*pAsicMicroReg;
	MENCV				*MencV;
	BASE_CTRL_HNDL		*BaseControls;
	SERVO_CONTROL_IF	*SvControlIf;
	DETVIB				*DetVib;
	CTRL_LOOP_OUT		*CtrlLoopOut;
	SEQ_CTRL_OUT		*SeqCtrlOut;
	MPFIND				*MpFind;		/* <S02F> */
	ANLG_UN_MON_DATA	*UnMonitor;
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	SEQ_IO_HNDL 		*IoSignals;		/* <S045> */
#endif
	BPRMDAT				*Bprm;					/* <S04D> */
	BASE_LOOPCTRLS		*BaseLoops;

#if( CSW_SVCF_VIB_DETECT_USE != 0 )
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl		= AxisRscB->FnCmnCtrl;
#endif
//	pAsicMicroReg	= AxisRscB->SvAsicRegs->AsicMicroReg;
	MencV			= AxisRscB->MencV;
	BaseControls		= AxisRscB->BaseControls;
	SvControlIf		= AxisRscB->SvControlIf;
	DetVib			= AxisRscB->DetVib;
	CtrlLoopOut		= AxisRscB->CtrlLoopOut;
	SeqCtrlOut		= AxisRscB->SeqCtrlOut;
	MpFind			= AxisRscB->MpFind;		/* �R�����g�A�E�g����<S02F> */
	UnMonitor		= AxisRscB->UnMonitor;
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	IoSignals 		= AxisRscB->IoSignals;	/* <S045> */
#endif
	Bprm			= AxisRscB->UniPrms->Bprm;	/* <S04D> */
	BaseLoops		= AxisRscB->BaseLoops;

#if 0 /* 2012.08.31 Y.Oka ����ScanA��CpuScanA�� */
/*--------------------------------------------------------------------------------------------------*/
/*		Set Encoder Error Alarm																		*/
/*--------------------------------------------------------------------------------------------------*/
	lwk = (LONG)AxisRscB->SvAsicRegs->MicroBitDat.data;
	if( lwk & BITDAT_ABF3ALM )
	{ /* A.BF3 : ScanA�����ُ� */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_SCANA_STOP );
	}
	if( lwk & BITDAT_AC90ALM )
	{ /* A.C90 : �G���R�[�_�ʐM�ُ� */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_PGCOM );
	}
	if( lwk & BITDAT_AC91ALM )
	{ /* A.C91 : �G���R�[�_�����x�ُ� */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_PGCOM1 );
	}
	if( lwk & BITDAT_AC20ALM )
	{ /* A.C20 : ScanA�����ُ� */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_PHASE );
	}
	if( lwk & BITDAT_AC21ALM )
	{ /* A.C21 : ScanA�����ُ� */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_POLESENSOR );
	}
#endif
	SvControlIf->CmdReady = MencV->PhaseReady;

/*--------------------------------------------------------------------------------------------------*/
/*		����p�p�����[�^�]������																	*//* <S066> */
/*--------------------------------------------------------------------------------------------------*/
	/* Control Parameter Translate */
	if( TR_REQ_SCANB == TranlateCtrlGetReq(&(BaseControls->TransduceCtrl)) )
	{
		TranlateCtrlPrm( &(BaseControls->TransduceCtrl) );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		����w�ߓ��͏���																			*/
/*--------------------------------------------------------------------------------------------------*/
	BpiInputControlCMD(BaseControls, SvControlIf, CtrlLoopOut,
						SeqCtrlOut, ((AxisRscB->UniPrms->Prm->b_prm0>>4) & 0x0F), ax_no);/* <S1C0>:���ǉ� */

/*--------------------------------------------------------------------------------------------------*/
/* 		<S045> H/W����̂�PCL/NCL�M������(�s�v�Ȃ����) 											*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	BaseControls->TrqLimitData.var.PclSignal |= IoSignals->PclSignal.SignalStatus.v;
	BaseControls->TrqLimitData.var.NclSignal |= IoSignals->NclSignal.SignalStatus.v;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		�X�L�����a�T�[�{���䏈��																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->PhaseReady == FALSE )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		���Ɍ��o���䏈��																		*/
	/*----------------------------------------------------------------------------------------------*/
		/* Position Reference for Motor Pole Find */
		BpiMpoleFindMakePosCmd( BaseLoops, SvControlIf, AxisRscB->FnCmnCtrl, Bprm );		/*<S02F>*/

		/* Prepare Motor Pole Find */
		/* ���[�^���x���o�p�ړ����ώ��萔�ύX */
		BpiChangeSpdDetMaFil( AxisRscB->MotSpdMafil, 4 );		/*<S02F>*/

		/* Tuning Less Invalid Setting */
		BaseControls->TuneLessCtrl.var.TuneLessAct = FALSE;
		BaseControls->CtrlCmdMngr.TrqFilDisable = TRUE;

		/* Get Motor Pole Find Execution Condition */
		MpfExeCondition.HaltCondition = (!SeqCtrlOut->SvRdyPhaseIgnore)
										| SeqCtrlOut->PotSigSts | SeqCtrlOut->NotSigSts;
		MpfExeCondition.MotStopSts = SeqCtrlOut->MotStop;
		MpfExeCondition.dMotPosition = BaseControls->MotSts.dMotPos;
		MpfExeCondition.TrefMonitor = CtrlLoopOut->TrqRefo_a;
		MpfExeCondition.SpdFbMonitor = BaseControls->CtrlCmdMngr.SpdFbki;
		MpfExeCondition.KVfbFil = BaseControls->CtrlCmdPrm.KVfbFil;		/*<S02F>*/
		MpfExeCondition.Kphasepos = MencV->Kphasepos;
		MpfExeCondition.Kmovpos = MencV->Kmovpos;

//		PdetServoOnMake( SvControlIf, FnCmnCtrl, MencV, AxisRscB->AlmManager->Status.AlmFlag );	/* <S21C> */
		AlarmCode = PdetServoOnMake( SvControlIf, FnCmnCtrl, MencV, AxisRscB );	/* <S226> */
		if( AlarmCode != NO_ALARM )																	/* <S226> */
		{
			ALMSetServoAlarm( AxisRscB->AlmManager, AlarmCode );
		}																							/* <S226> */

		/* Get Pole Detect Request */
//		PoleDetectReq = FnCmnCtrl->PdetReq | ((SvControlIf->CmdSeqBit >> POL_DETECT_BITNO) & 0x1);
		PoleDetectReq = FnCmnCtrl->SvOnPdetReq | ((SvControlIf->CmdSeqBit >> POL_DETECT_BITNO) & 0x1);	/* <S21C> */


		/* Motor Pole Find Main Operation */
		AlarmCode = MpoleFindSequence( MpFind,
									   &MpfExeCondition,
									   PoleDetectReq, BaseControls->BeComplete);
		if( AlarmCode != NO_ALARM )
		{
			ALMSetServoAlarm( AxisRscB->AlmManager, AlarmCode );
		}

		/* Termination Process for Motor Pole Find */
		if( MpFind->var.Step == MPF_END )
		{
			if(FALSE != PdetStartPhaseOffsetWrite(AxisRscB->PdetAbsSeq,
												MencV, MpFind->MpfPhaseOffset))
			{
				/* ���[�^���x���o�p�ړ����ώ��萔��߂� */
				if ( BaseControls->TuneLessCtrl.conf.TuningLessUse == TRUE )
				{
					BpiChangeSpdDetMaFil( AxisRscB->MotSpdMafil, 0 );		/*<S02F>*/
				}
				else
				{
					BpiChangeSpdDetMaFil( AxisRscB->MotSpdMafil, -1 );		/*<S02F>*/
				}

				/* Turn Back the Tuning Less Setting */
				BaseControls->TuneLessCtrl.var.TuneLessAct = BaseControls->TuneLessCtrl.conf.TuningLessUse;

				/* Motor Pole Find Complete */
				MencV->PhaseReady = TRUE;
				BaseControls->CtrlCmdMngr.TrqFilDisable = FALSE;
			}
		}

		/* Set Phase Offset Information to Micro <S02F>	*/
//		pAsicMicroReg->MREG_MPFPHASEOFST = (USHORT)MpFind->MpfPhaseOffset;
//		pAsicMicroReg->MREG_MPFPHASEOFSTH = (USHORT)(MpFind->MpfPhaseOffset << 16);
		MencV->MpfPhaseOffset = MpFind->MpfPhaseOffset;

		/* Update Speed Reference */
		BaseControls->CtrlCmdMngr.SpdRefi = MpFind->MpFindSpdRef;
		BaseControls->CtrlCmdMngr.SpdRefo = BaseControls->CtrlCmdMngr.SpdRefi;
		BaseControls->CtrlCmdMngr.SpdLimit = 0x1000000;	/* ���x�����Ȃ�	*/

		/* Speed Loop Control for Motor Pole Find */
		BaseControls->CtrlCmdMngr.TrqRefo = MpoleFindSpeedControl( MpFind,
																&(MpfExeCondition),
																BaseControls->BeComplete );

		BaseControls->CtrlCmdMngr.SpdLimit = 0x1000000;	/* ���x�����Ȃ�	*/
		BaseControls->CtrlCmdMngr.CtrlMode = CTRLMODE_TRQ;	/* �g���N���䃂�[�h */
	}
	else
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		�T�[�{�g������																			*/
	/*----------------------------------------------------------------------------------------------*/
#if !defined(_TEST_BOARD_) || defined(_VC_DEBUG_)	/* 2012.12.13 Y.Oka �������ԑ΍� */

#if( CSW_SVCF_VIB_DETECT_USE != 0 )
		/* �U�����o���x�I�u�U�[�o���Z */
		if(AxisRscB->UniPrms->Bprm->PsFulcSelect != FALSE)/* <S04D>:�ٸ�����ǉ� */
		{/* �t���N���[�Y�h����	*/
			DetVibSpeedObserver( DetVib, BaseControls->MotSts.FencSpd, CtrlLoopOut->TrqRefMon_a );
		}
		else
		{/* �Z�~�N���[�Y�h����	*/
		DetVibSpeedObserver( DetVib, BaseControls->MotSts.MotSpd, CtrlLoopOut->TrqRefMon_a );
		}
		/* ���U���o�p�ő�E�ŏ����x�΍��ݒ菈�� */
		//110208_kira_for_AAT
		if( (FnCmnCtrl->State.TuningPctrlRun) || (FnCmnCtrl->State.TuningRun) )
		{ /* AAT���s�� */
			DetVibSetSpeedError( DetVib, BaseControls->TuneLessCtrl.conf.TuningLessUse, TRUE );
		}
		else
		{ /* AAT���s���ȊO */
			DetVibSetSpeedError( DetVib, BaseControls->TuneLessCtrl.conf.TuningLessUse, FALSE );
		}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */

		/* �U�����o�I�u�U�[�o�ɂ�鑬�x�΍��g���[�X */
		DetVibTraceRuntimeService( DetVib );

#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
		if( BaseControls->ResVib.AresVibSetting == TRUE )
		{ /* A�^���U�L���� */
			/* A�^���U���䉉�Z */
			BaseControls->CtrlCmdMngr.SpdFBCx -=
				ResVibATypeResVibCtrl( &(BaseControls->ResVib),
									   BaseControls->CtrlCmdMngr.SpdObsFbki,
									   CtrlLoopOut->TrqRefo_a );
		}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */

#endif /* #if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_) */

	/*----------------------------------------------------------------------------------------------*/
	/*		����T�[�{���䃁�C��																	*/
	/*----------------------------------------------------------------------------------------------*/
		BpiYecServoControl( BaseLoops, SvControlIf, CtrlLoopOut, FnCmnCtrl, Bprm);	/* <S04D> */

	/*----------------------------------------------------------------------------------------------*/
	/*		�T�[�{�����ʒu�Ǘ�																<S14C>	*/
	/*----------------------------------------------------------------------------------------------*/
		PosMngResponseManager( BaseControls );

	/*----------------------------------------------------------------------------------------------*/
	/*		�t���N�ʒu���[�v����																	*/
	/*----------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_SB_SUPPORT)/* �g�p���Ȃ��F�\�t�g�������ɍ폜�� */
		if((AxisRscB->UniPrms->Bprm)->FencUse != FALSE)
		{
			BaseControls->CtrlCmdMngr.PicvClrCmd |= (BaseControls->BeComplete^0x1);

			BasePositionControl( &BaseControls->FcPosLoop,
								 &BaseControls->CtrlCmdMngr,
								 &BaseControls->PosManager,
								 BaseControls->GainChange.GselGains[0].Kp,
								 BaseControls->CtrlCmdPrm.KpiFc );
		}
#endif	/*(CSW_SVCF_FULC_MODE == CSW_FULC_SB_SUPPORT) */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		�g���N���~�b�g���Z����																		*/
/*--------------------------------------------------------------------------------------------------*/
	BpiTorqueLimitControl( &(BaseControls->TrqLimitData),
						   BaseControls,
						   SeqCtrlOut->TrqLimitReq,
						   AxisRscB->AlmManager );	/* <S145> */


/*--------------------------------------------------------------------------------------------------*/
/*		���莞�Ԋ֘A����																			*/
/*--------------------------------------------------------------------------------------------------*/
	StlgCalculateSettlingTime( &BaseControls->SettlingTime,
							   BaseControls->PosManager.var.dPcmda,
							   BaseControls->PosCtrlSts.CoinSignal );

/*--------------------------------------------------------------------------------------------------*/
/*		�ő�I�[�o�[�V���[�g�ʍ쐬 --> Un106														*/
/*--------------------------------------------------------------------------------------------------*/
	StlgMakeMaxOverShoot( &BaseControls->SettlingTime,
						  &(BaseControls->PosManager),
						  (CTRLMODE_POS == BaseControls->CtrlModeSet.CtrlMode.b.cm) );

/*--------------------------------------------------------------------------------------------------*/
/*		�ʒu���ߊ������s�ݐώ��ԍ쐬 --> Un10B														*/
/*--------------------------------------------------------------------------------------------------*/
	StlgMakeCoinOffTime( &BaseControls->SettlingTime,
						 BaseControls->PosManager.var.dPcmda,
						 BaseControls->PosCtrlSts.CoinSignal,
						 (CTRLMODE_POS == BaseControls->CtrlModeSet.CtrlMode.b.cm) );
						 
/* 2013/04/01MD�ł́A���L������쐬���Ȃ��B�Ȃ��H	*/
/*--------------------------------------------------------------------------------------------------*/
/*		���j�^�p�ϐ��o��																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* ���x�w��(�A�i���O���j�^�f�[�^�ݒ�) */
	UnMonitor->AmonSpdRef = BaseControls->CtrlCmdMngr.SpdRefo + BaseControls->CtrlCmdMngr.SpdFFCx;
//	UnMonitor->AmonSpdRef = BaseControls->CtrlCmdMngr.SpdRefo;//S0CD
	/* �g���N�w��(�A�i���O���j�^�f�[�^�ݒ�) */
	UnMonitor->AmonTrqRef = CtrlLoopOut->TrqRefMon;
	/* �ʒu�w�߃��j�^	[�w�ߒP��/Scan]		*/
	BaseControls->AmondPosCmd = BaseControls->PosManager.var.dPcmda;		/* <S226> */
//START S0CD
	BaseControls->FrictionM.MotFricEnable = AxisRscB->SeqCtrlOut->BaseEnableReq;

	if((AxisRscB->SeqCtrlOut->BaseEnableReq == TRUE) && BaseControls->FrictionM.InitEnable)
	{
		InitParamFriction( AxisRscB->BaseLoops  );
	}
	if(AxisRscB->SeqCtrlOut->BaseEnableReq == FALSE)
	{
		BaseControls->FrictionM.InitEnable = TRUE;//in order to initialize one time when servo on
	}
//END S0CD
}


/****************************************************************************************************/
/*																									*/
/*		���Ɍ��o�T�[�{�I���v���쐬�쐬													 <S21C> 	*/
/*																									*/
/****************************************************************************************************/
#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
//void	PdetServoOnMake( SERVO_CONTROL_IF *SvControlIf, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV, BOOL Alm )
ALMID_T	PdetServoOnMake( SERVO_CONTROL_IF *SvControlIf, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV, AXIS_HANDLE *Axis )	/* <S226> */
{
	BOOL	bwk;
	BOOL	AC22;						/* <S226> */
	BOOL	Alm;						/* <S226> */
	ALMID_T	AlmCode	= NO_ALARM;			/* <S226> */
	
	Alm  = ( Axis->AlmManager->AlmReady == FALSE );					/* <S226> */
	AC22 = ALMGetOtherAlarmChk( Axis->AlmManager, ALM_PHASEANGLE );	/* ���Ɍ��o���A���[��A.C22 <S226> */
	
	if( MencV->AxisMotType == MOTTYPE_LINEAR )						/* ���j�A���[�^					*/
	{
		if( FnCmnCtrl->FnSvControl == FALSE )						/* �ʏ풆						*/
		{
			if( MencV->PdetReq587 == TRUE )							/* Pn587.0:���Ɍ��o�v�� <S226> */
			{
				if(( Alm == TRUE )&&( AC22 == FALSE ))				/* <S226> */
				{
					AlmClearReq( Axis->AlmManager, ALM_PHASEANGLE );	/* A.C22�A���[�����N���A  */
					SvControlIf->CmdSeqBit |= POL_DETECT_BIT;
				}
				else
				{
					if( Alm == FALSE )
					{
						SvControlIf->CmdSeqBit |= POL_DETECT_BIT;
					}
				}													/* <S226> */
			}
			else
			{
				SvControlIf->CmdSeqBit &= ~( POL_DETECT_BIT );
			}
			if(( SvControlIf->CmdSeqBit & SERVO_ON_BIT ) != 0 )		/* �T�[�{�I���v��		 <S226> */
			{
				AlmCode = ALM_PDET;									/* ���Ɍ��o����					*/
			}														/*						 <S226> */
		}
		else														/* ROUND���䒆					*/
		{
			if( Alm == FALSE )
			{
				bwk = FnCmnCtrl->PdetReq;
				if( FnCmnCtrl->FnSvonReq == TRUE )					/* �T�[�{�I���v��		 <S226> */
				{
					AlmCode = ALM_PDET;								/* ���Ɍ��o����					*/
				}													/* 						 <S226> */
			}
			else
			{
				bwk = FALSE;
				if( AC22 == FALSE )									/* <S226> */
				{
					bwk = FnCmnCtrl->PdetReq;
				}													/* <S226> */
			}
			FnCmnCtrl->SvOnPdetReq = bwk;
		}
	}
	return( AlmCode );		/* <S226> */
}
#endif

/***************************************** end of file **********************************************/
