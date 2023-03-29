/****************************************************************************
Description	: ScanC Task module
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	SysScanCTask()			:Main routine of the Scan C Task

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.01.24     renewal

****************************************************************************/
//#include <INTEGRITY.h>
//#include "Basedef.h"
#include "KnlApi.h"
#include "MercuryGlobals.h"
#include "SequenceMain.h"
#include "CheckAlarm.h"
#include "SequenceIo.h"
#include "PowerManager.h"
#include "AlarmManager.h"
#include "KLib.h"
#include "MLib.h"
#include "HwDeviceIf.h"
LONG TEST_SCANC_CNT;

/* Prototype */
void SysScanCInt( void );

extern LONG	NumberOfAxes;

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
void IntScanC( void ); /* �b�菈�u */
extern void ClearSeqCtrlInterrupt( void );
extern void	SetEnableInterrupt( void );

/****************************************************************************
  Description: Interrupt Operation for Sequence Scan Task
-----------------------------------------------------------------------------
  Parameters	: none
  Return		: none
*****************************************************************************/
void IntScanC( void )
{
	ULONG		clock_time, sbTime;
	UINT		netled;
	AXIS_HANDLE	*AxisRscC;

	TEST_SCANC_CNT++;

	AxisRscC = GetAxisHandle(0);

	/* �������ԑ���pGPIO ON */;
#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
	HALdrv_OutputDebugSP( INTERRUPT_ScanC );
#endif
#if 0
	clock_time = KlibGetClockTime();
#endif
	sbTime = AxisRscC->TaskProcessTime->ScanSumTime;

	KPI_EI (); 							//@@	SetEnableInterrupt( );
	ClearSeqCtrlInterrupt( );

//	if( ScanCStartCtr < 0x0F )
//	{
//		ScanCStartCtr++;
//	}
//	else
//	{
		SysScanCInt( );
//	}

	/* �������ԑ���pGPIO OFF */
#if 0
	clock_time = KlibGetClockTime() - clock_time;
	if(clock_time > AxisRscC->TaskProcessTime->SC_maxTime)
	{
		AxisRscC->TaskProcessTime->SC_maxTime = clock_time;
	}
	AxisRscC->TaskProcessTime->SC_crtTime = clock_time;
	AxisRscC->TaskProcessTime->ScanSumTime = clock_time + sbTime;
#endif
	HALdrv_ClearDebugSP( INTERRUPT_ScanC );
}
#endif



/****************************************************************************
  Description: Scan C Main Procedure
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscC	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
void SysScanCInt( void )
{
	LONG				ax_no;
	HWSIN				SeqInputSignals;
	LONG				AlmWrnSts;
	USHORT				FaultState;
	BOOL				BaseEnableSts;
	BOOL				DBRequest;
	BOOL				MotStopSts;
	AXIS_HANDLE			*AxisRscC;
	AXIS_HANDLE			*AxisRsc;
	POW_MNG_HNDL		*PowerManager;
//	SEQ_IOSIGNAL		*SeqIo;				/*<S0C7>*/
	VULONG				ScanCStartTime;		/*<S043>*/
	TASK_MEASURE_TIME	*TaskProcessTime;	/*<S043>*/
	/* <S154> Start */
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	AVGTMPR_HANDLE	*hAvgTmprHdl;
	ULONG 			CurrentTimerCnt;	/* ���݉ғ�����	*/
	SHORT 			CurrentTemperature;
	LONG			PreWrnSts;
#endif
	/* <S154> End */
	ASICS				*svAsicRegs;		/*<S126>*/
	volatile ULONG		SABinC_exeTime;		/*<S1CD>*/
	BASE_CTRL_OUT		*BaseCtrlOut;		/* <S21D> */
	SEQ_IO_HNDL			*IoSignals;			/* <S21D> */

	/* �������ԑ���pGPIO ON */;
#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
//	HALdrv_OutputDebugSP( INTERRUPT_ScanC );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */
#endif
	KlibRstLongTimer((ULONG*)&ScanCStartTime);		/*<S043>*//*<S0C7>*/
	/*----------------------------------------------------------------------*/
	TEST_SCANC_CNT++;			/* @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ 	*/
	/*----------------------------------------------------------------------*/

	AxisRscC = GetAxisHandle(0);
	PowerManager = AxisRscC->PowerManager;
	TaskProcessTime = AxisRscC->TaskProcessTime;		/*<S043>*/
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	hAvgTmprHdl = &(AxisRscC->hLifeTime->AvgTmprHdl);	/* <S154> */
#endif
	BaseCtrlOut = AxisRscC->BaseCtrlOut;		/* <S21D> */
	IoSignals = AxisRscC->IoSignals;			/* <S21D> */

	/* ScanC���s�t���O�Z�b�g <S043> */
	TaskProcessTime->SC_exeflg = TRUE;

	/*----------------------------------------------------------------------*/
	/*	�O2ms�Ԃ̎��s���Ԍv�Z����				<S043>						*/
	/*----------------------------------------------------------------------*/
	TaskProcessTime->crtTimePer2ms = TaskProcessTime->SA_sumTime
										+TaskProcessTime->SB_sumTime
										+TaskProcessTime->SC_crtTime;
	TaskProcessTime->SA_sumTime = 0;	/* �ݐϏ������ԃ��Z�b�g */
	TaskProcessTime->SB_sumTime = 0;	/* �ݐϏ������ԃ��Z�b�g */
	if( TaskProcessTime->crtTimePer2ms > TaskProcessTime->maxTimePer2ms )
	{
		TaskProcessTime->maxTimePer2ms = TaskProcessTime->crtTimePer2ms;
	}


	/* �X�L�����J�E���g��1���傫���A�l�b�g���[�N���莞�ɃA���[���`�F�b�N���s�� */ /*<S1DB>*/
	if( (TEST_SCANC_CNT > 1) && (OpiCheckStableState( AxisRscC ) == FALSE) )
	{
/* <S1CD> >>>>> */
		/* �X�L�����b�T�C�N�����ԃI�[�o�[�`�F�b�N(�J�E���g�l�Ŕ�r) */
		if( TaskProcessTime->crtTimePer2ms > ((KPI_SCCYCLEMS * 1000000) / ITMR_NSEC_PER_COUNT) )
		{
			/* A.BF0 : �V�X�e���A���[���O(�����v���O���������ُ�) */
			ALMSetGlobalAlarm( ALM_SAMP_OVER );
		}
/* <<<<< <S1CD> */
	}
	else
	{
		TaskProcessTime->maxTimePer2ms = 0;		/* 2ms�ő又�����Ԃ����Z�b�g���� <S1F3> */
		OpiResetSyncExe( AxisRscC );
	}

	/*----------------------------------------------------------------------*/
	/*	���j�b�g����ScanC���͏���											*/
	/*----------------------------------------------------------------------*/
	SeqInputSignals = HALdrv_ReadInputSignals( );
	PcmInputPowerStatus( PowerManager,
						 SeqInputSignals.flag.Acon1,
						 SeqInputSignals.flag.RegeneAlm );
	/*----------------------------------------------------------------------*/
#if (SVFSEL_ADC_USE == 1)||(SVFSEL_ONLY_OH_ADC_USE == 1)		/* <S09F> *//* <S0E1>:SVFSEL_ONLY_OH_ADC_USE�ǉ� */
	KpxAdcScanCService( AxisRscC->pAdc );		/* A/D converter ScanC service */
#endif                     /* <S09F> */
	KpxPlsScanCService( AxisRscC->pPlsIfV );	/* Command Pulse counter ScanC service */
	/*----------------------------------------------------------------------*/
	SeqIoInputSeqSignalsCom( IoSignals, AxisRscC->UniPrms->Prm );		/* <S21D> */

	/* < S154> Start  */
	/*----------------------------------------------------------------------*/
	/*		���ϓ������x�Z�o���� 											*/
	/*----------------------------------------------------------------------*/
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	CurrentTimerCnt = KlibGetTimeStamp();		/* ���݉ғ����Ԏ擾 */
	/* �ݐωғ����ԃJ�E���g����30�������߂邽�߂ɁA14bit�E�V�t�g��p����B	*/
	/* �ݐωғ����Ԃ̒P�ʂ�[100ms]�ł��邱�Ƃ���A						*/
	/* 30����18000�J�E���g��16384 = 2^14���痈����̂ł���B				*/
	if ( ((CurrentTimerCnt - hAvgTmprHdl->var.Timer100msCntLast) >> 14) > 0 )
	{	/*30 ����������*/
	
		/* ���ݍō����x�擾  */
		CurrentTemperature = DetTempGetInternalTemperature(AxisRscC->TempManager);

		/*	���ϓ������x�Z�o���� 	*/
//		AvgInternalTemperatureCal(AxisRscC->AlmManager->EepDevice, hAvgTmprHdl,		/* <S192> */
//								(CurrentTemperature), CurrentTimerCnt );
		AvgInternalTemperatureCal( &AxisRscC->AlmManager->EepQue, hAvgTmprHdl,		/* <S192> */
								(CurrentTemperature), CurrentTimerCnt );
	}
#endif
	/* < S154 > End  */

	/*----------------------------------------------------------------------*/
	/* Kernel Input  Service for ScanC */
	/*----------------------------------------------------------------------*/
	AxisRsc = AxisRscC;
//	FaultState = DBRequest = MotStopSts = BaseEnableSts = 0; /* <S0A3> *//* <S1D5> */
	FaultState = DBRequest = BaseEnableSts = 0;	/* <S1D5> */
	MotStopSts = 1;	/* <S1D5> */

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		SyscInputSeqStatus( AxisRsc );

		/* @@�����Ή����Č������邱�� 2013.07.14 Y.Oka (>_<) */
		/*------------------------------------------------------------------*/
		/* ���j�b�g�����p�e���f�[�^�擾 */
		/*------------------------------------------------------------------*/
		FaultState |= AxisRsc->CheckAlarm->faultState;
//		BaseEnableSts |= AxisRscC->SeqCtrlOut->BeComplete;					  /* <S0A3> *//* <S0F6> */
//		BaseEnableSts |= AxisRscC->SeqCtrlOut->UdlBeStatus;					  /* <S0F6> */
		BaseEnableSts |= AxisRsc->SeqCtrlOut->UdlBeStatus;					  /* <S131> */
		DBRequest |= AxisRsc->BeSequence->DbOn;
//		MotStopSts |= AxisRscC->SeqCtrlOut->MotStop;
//		MotStopSts |= AxisRsc->SeqCtrlOut->MotStop;							  /* <S131> *//* <S1D5> */
		MotStopSts &= AxisRsc->SeqCtrlOut->MotStop;							  /* <S1D5> */

		AxisRsc++;
	}

#ifdef CSW_BSP_SGDZ_IFBS80
	/*----------------------------------------------------------------------*/
	/* Over heat alarm detection to every board */
	/*----------------------------------------------------------------------*/
	AxisRsc = AxisRscC;
	faultState1 = faultState2 = 0;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		if((OH_ALM_SLOT_GRP1 >> AxisRsc->AxisNo) & 1)
		{
			faultState1 |= AxisRsc->SeqCtrlData->faultState;
			if(0 != (SHAL_FAULT_OH & faultState1))
			{/* A.7A0 : �q�[�g�V���N�ߔM				*/
				ALMSetServoAlarm( AxisRsc->AlmManager, ALM_OH );
			}
		}
		if((OH_ALM_SLOT_GRP2 >> AxisRsc->AxisNo) & 1)
		{
			faultState2 |= AxisRsc->SeqCtrlData->faultState;
			if(0 != (SHAL_FAULT_OH & faultState2))
			{/* A.7A0 : �q�[�g�V���N�ߔM				*/
				ALMSetServoAlarm( AxisRsc->AlmManager, ALM_OH );
			}
		}
		AxisRsc++;
	}
#endif	//CSW_BSP_SGDZ_IFBS80

	/*----------------------------------------------------------------------*/
	/* �d���Ǘ��������s(���j�b�g�P��) */
	/*----------------------------------------------------------------------*/
#if (CSW_HW_POWER_MANAGE == TRUE)
	/* <S21D> Start */
	/*----------------------------------------------------------------------*/
	/*		�V�[�P���X���͐M����ǂݏo��											*/
	/*----------------------------------------------------------------------*/
	PowerManager->PowerSts.ClinkAnsSig =
			IoSignals->ClinkansSignal.SignalStatus.v;				/* �R���f���T�o���N�ڑ��Ď����͐M��	*/
	PowerManager->PowerSts.CmcAnsSig =
			IoSignals->CmcansSignal.SignalStatus.v;					/* �˖h�����[��ԊĎ����͐M��		*/
	PowerManager->PowerSts.ConvRdySig =
			IoSignals->ConvreadySignal.SignalStatus.v;				/* �R���o�[�^���f�B���͐M��			*/
	PowerManager->PowerSts.ConvMconSig =
			IoSignals->ConvmconSignal.SignalStatus.v;				/* �˓��d���h�~�p�����[���͐M��		*/
	/* <S21D> End */
//	PcmPowerManager( PowerManager, MotStopSts );							  /* <S0F6> */
	PcmPowerManager( PowerManager, MotStopSts, BaseEnableSts );				  /* <S0F6S0F6> */
#endif

	/* �񐶉ߕ��׌��o�������s(���j�b�g�P��) */
#if (CSW_HW_REGENERATION_CTRL == TRUE)
	/*----------------------------------------------------------------------*/
	/*	Delta Regene.Tr On Count for ScanC Alarm Check						*/
	/*----------------------------------------------------------------------*/
	/* �񐶃g�����W�X�^ON���Ԍv�� */
	PcmCountRegeneOnTime( PowerManager );

	/* �񐶉ߕ��׃`�F�b�N */
#if 0		/* <S21D> */
	if( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)
		&& (PowerManager->MainPowerChecker.conf.TestMode2 == FALSE) )
	{ /* AC�d������ && !TestMode2 */
#else		/* <S21D> */
	if( ( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)			/* AC�d������ */
	   || ( (PowerManager->MainPowerChecker.conf.Dcbus == TRUE)			/* DC�d������ */
		 && (PowerManager->RegeneCtrl.conf.RegSyn == PNREGSYN) ) )		/* PN������ʃV�X�e���ɂ����ĉ񐶓����@�\���g�p */
	 && (PowerManager->MainPowerChecker.conf.TestMode2 == FALSE) )		/* !TestMode2 */
	{
#endif
		if( ALMCheckGlobalAlarm(WRN_EXREG) == FALSE )					/* A.924�i�񐶌x���j�Ȃ��̏ꍇ *//* <S21D> */
		{
			ULONG	AlmOLInfo = (AxisRscC->AlmManager)->Status.AlmOLInfo; 		/* <S0FF> */
			AlmWrnSts = PcmCheckRegeneOverLoad( &(PowerManager->RegeneCtrl),
											   &AlmOLInfo, 						/* <S0FF> */
											   AxisRscC->UnMonitor);
			if( AlmWrnSts != CHK_REGOL_NO_OPERATION )
			{
				/* �񐶉ߕ��׃A���[���E���[�j���O�ݒ� */
				if(0 != (AlmWrnSts & CHK_REGOL_SET_ALARM))
				{	/* A.320 : �񐶉ߕ��� */
					ALMSetGlobalAlarm( ALM_RGOL );
				}
				else if(0 != (AlmWrnSts & CHK_REGOL_SET_WARNING))
				{	/* A.920 : �񐶉ߕ��׃��[�j���O */
					ALMSetGlobalAlarm( WRN_RGOLF );
				}
				else
				{	/* A.920 : �񐶉ߕ��׃��[�j���O */
					ALMClearGlobalAlarm( WRN_RGOLF );
				}
			}
		}
	}
#endif

#if 0 /* ���������ֈړ� *//* <S0A2> */
	/************************************/
	/* �˓��h�~��H����(����)			*/
	/************************************/
	PcmRlyControlProcess( &PowerManager->MainPowerChecker,
						  &PowerManager->PowerSts,
						  FaultState,
						  DBRequest,
						  AxisRscC->FnCmnCtrl->KnlMode );
#endif

	/* �˓��h�~��R�ߕ��׌��o����(���j�b�g�P��) */
#if (CSW_HW_CHKRUSH == TRUE)
	if( CHK_RUSH_NO_OPERATION != PcmCheckRushRxOverLoad( &(PowerManager->Rush),
														 &(PowerManager->PowerSts) ) )
	{	/* A.740 : �˓���R�ߕ��� */
		ALMSetGlobalAlarm( ALM_RSOL );
	}
#endif

	/* �񐶈ُ팟�o����(���j�b�g�P��) */
#if (CSW_HW_REGENERATION_CTRL == TRUE)
#if 0		/* <S21D> */
	if( PowerManager->MainPowerChecker.conf.Dcbus == FALSE )
	{
		if( CHK_REGENE_NO_OPERATION != PcmCheckRegeneAlarm( &(PowerManager->RegeneCtrl),
														    PowerManager->PowerSts.PowerOn) )
		{	/* A.300 : �񐶈ُ� */
			ALMSetGlobalAlarm( ALM_RG );
		}
	}
#else		/* <S21D> */
	if( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)
	 || ( (PowerManager->MainPowerChecker.conf.Dcbus == TRUE)			/* DC�d������ */
	   && (PowerManager->RegeneCtrl.conf.RegSyn == PNREGSYN) ) )		/* PN������ʃV�X�e���ɂ����ĉ񐶓����@�\���g�p */
	{
		if( CHK_REGENE_NO_OPERATION != PcmCheckRegeneAlarm( &(PowerManager->RegeneCtrl),
														    PowerManager->PowerSts.PowerOn) )
		{
			if( (PowerManager->RegeneCtrl.conf.RegSyn == PNEXREG)		/* �O���񐶋@�\�𗘗p	*/
			 && (PowerManager->RegeneCtrl.conf.RegRxIn == FALSE) )		/* �񐶒�R�O�u��		*/
			{
				if( PowerManager->MainPowerChecker.var.WrnExRegInhibit == FALSE )
				{ /* A.924 : �񐶌x�� */
					ALMSetGlobalAlarm( WRN_EXREG );
					PowerManager->MainPowerChecker.var.WrnExRegInhibit = TRUE;
				}
			}
			else
			{ /* A.300 : �񐶈ُ� */
				ALMSetGlobalAlarm( ALM_RG );
			}
		}
	}
#endif
#endif

#if (CSW_DETECT_OH_FROM_TEMPIC == TRUE) /* <S0A3> *//* <S0ED> */
	/* Over Heat *//* <S03A> *//* <S0DE> */
	OhmCheckOverHeatAlm( AxisRscC->AlmManager,
						 AxisRscC->OverHeat,
						 DetTempGetCtrlBoardTemperature(AxisRscC->TempManager),
						 DetTempGetPowerBoardTemperature(AxisRscC->TempManager) );

	/* Detect Temperature IC Error *//* <S0DE> */
	AdetCheckTempDeviceErr( AxisRscC->AlmManager,
							DetTempGetCtrlBoardErrorCount(AxisRscC->TempManager),
							DetTempGetPowerBoardErrorCount(AxisRscC->TempManager) );
#endif


#if (CSW_HW_INTERNAL_FAN == TRUE) /* <S0A3> */
	/* �T�[�{�p�b�N�����t�@����~���o����(���j�b�g�P��) */
	AlmWrnSts = PcmCheckFanLock( &(PowerManager->FanLock),
								 SeqInputSignals.flag.FanAlm,
								 BaseEnableSts );
	/* �T�[�{�p�b�N�����t�@����~�A���[���E���[�j���O�ݒ� */
	if(0 != (AlmWrnSts & CHK_FANLOCK_SET_ALARM))
	{	/* A.7AB : �T�[�{�p�b�N�����t�@����~�A���[�� */
		ALMSetGlobalAlarm( ALM_FANLOCK );
	}
	else if(0 != (AlmWrnSts & CHK_FANLOCK_SET_WARNING))
	{	/* A.923 : �T�[�{�p�b�N�����t�@����~ */
		ALMSetGlobalAlarm( WRN_FANLOCK );
	}
	else
	{
		;
	}
#endif


	/* ����d�͗ʌv�Z(���j�b�g�P��) *//* <S020> */
#if (CSW_POWER_MONITOR_USE == TRUE)
	SetPowerMoniter( AxisRscC->PowerMonitor, GetPowerConsumption(AxisRscC->CalcWattage) );
#endif

	/* OC���o����	*//* <S126> */
	svAsicRegs = AxisRscC[0].SvAsicRegs;		/* OC���o�́A0���ڂ�UDL�̈���g�p����B	*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];
		AdetCheckOCAlarmDetections(AxisRsc->AlmManager, svAsicRegs, ax_no, AxisRsc->SeqCtrlOut->UdlBeStatus);		/* <S137> */

		/* ���[�^�ߓd�����o�`�F�b�N����	*//* <S1F7> */
		AdetCheckMotorOverCurrent( AxisRsc->AlmManager, AxisRsc->SvAsicRegs, AxisRsc->SeqCtrlOut->UdlBeStatus,
																								BaseEnableSts );
	}

	/* ���[�^�ߓd�����o�`�F�b�N����	*//* <S1F5> */
//	AdetCheckMotorOverCurrent( AxisRsc->AlmManager, svAsicRegs, AxisRsc->SeqCtrlOut->UdlBeStatus,
//			BaseEnableSts );/*<S1F5> */


	/* < S154> Start  */
	/*----------------------------------------------------------------------*/
	/*		�����\���@�\����												*/
	/*----------------------------------------------------------------------*/
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	/* <S180> Start */
#if 0
	AxisRscC->hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk = PowerManager->PowerSts.ChargeOk;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];
		/* DB�����[����񐔂̓��[�^��]����DB�I�������ꍇ�̂݃J�E���g�A�b�v���� �A���Ή�*/
		LifeTimeDBSwitchingCount( AxisRscC->hLifeTime, AxisRsc->AdatHndl->AdatP.SeqCtrlOut->TgOnSts , AxisRsc->BeSequence->DbOn);
	}
	/* �����c�����v�ZMain����													*/
	LifeTimeSurvivorCalculateMain( AxisRscC->hLifeTime , AxisRscC->RegManager->hPnReg , AxisRscC->UniPrms->Cprm );

	/* �\�h�ۑS���[�j���O�o�� */
	PreWrnSts = AdetCheckPreWarningSigOut( AxisRscC->hLifeTime,
								&(AxisRscC->PowerManager->MainPowerChecker) );
	if( PreWrnSts == CHK_PRE_SET_WRN )
	{
		/* ���[�j���O�iA.9B0�j���o�͂��� */
		ALMSetGlobalAlarm( WRN_PREVENTMAINT );
	}
#else
	AxisRscC->hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk = PowerManager->PowerSts.ChargeOk;		/* <S192> */
	/* �����c�����v�ZMain����													*/
	LifeTimeSurvivorCalculateMain( AxisRscC->hLifeTime, AxisRscC->RegManager->hPnReg, AxisRscC->UniPrms->Cprm );

//	AxisRscC->hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk = PowerManager->PowerSts.ChargeOk;	/* <S192> */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];
		/* DB�����[����񐔂̓��[�^��]����DB�I�������ꍇ�̂݃J�E���g�A�b�v���� �A���Ή�*/
		LifeTimeDBSwitchingCount( AxisRsc->hDBLifeTime, AxisRsc->AdatHndl->AdatP.SeqCtrlOut->TgOnSts,
								AxisRsc->BeSequence->DbOn);
		/* �����@�\�p�p�����[�^��������TO�@Eeprom */
//		LifeTimePrmStoreValueToEeprom( AxisRsc->hDBLifeTime,AxisRsc->RegManager->hPnReg,
//								AxisRsc->UniPrms->Cprm);		/* <S186> *//* <S192> */
		/* DB�����c�����v�Z�֐� */
		LifeTimeDBSurvivorCalculate( AxisRsc->hDBLifeTime );

		/* �\�h�ۑS���[�j���O�o�� */
		PreWrnSts = AdetCheckPreWarningSigOut( AxisRscC->hLifeTime,
											&(AxisRscC->PowerManager->MainPowerChecker),
											AxisRsc->hDBLifeTime->var.DBSurvivor);
		if( PreWrnSts == CHK_PRE_SET_WRN )
		{
			/* ���[�j���O�iA.9B0�j���o�͂��� */
//			ALMSetGlobalAlarm( WRN_PREVENTMAINT );						/* <S192> */
			ALMSetServoAlarm( AxisRsc->AlmManager, WRN_PREVENTMAINT );	/* <S192> */
		}
	}
#endif
	/* <S180> End */
#endif
	/* <S154> End  */

	/*----------------------------------------------------------------------*/
	/* �X�L�����b���C���֐����s */
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];

		SyscAlarmDetections( AxisRsc );

		SyscBaseDriveSequence( AxisRsc );

		SyscSequenceMain( AxisRsc );

		SyscSencAbsoOperation( AxisRsc ); /* ��Βl�G���R�[�_�������C��     *//*<S02D>*/
	}
	/*----------------------------------------------------------------------*/
	/*		�X�L����C���ʏo�͏���											*/
	/*----------------------------------------------------------------------*/
	SyscSequenceCommon( AxisRscC );

/* <S150> >>>>> ���L�AUSB�t�@���N�V�����p�̊��荞�݂��g�p�����A�|�[�����O�Ō��o */
	/*----------------------------------------------------------------------*/
	/*	USB Function Interface												*/
	/*----------------------------------------------------------------------*/
	UsbFuncIntIntu2f();					/* �v���O�C���^�v���O�A�E�g����				*/
	UsbFuncIntIntu2fepc();				/* �o�X�E���Z�b�g�^�o�X�E�X�s�[�h�ύX			*/
/* <<<<< <S150> */

	/* <S154> Start */
	/*----------------------------------------------------------------------*/
	/* �A���[�����b�`���C���֐����s */
	/*----------------------------------------------------------------------*/
/* <S212> */
//	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
//	{
//		AxisRsc = &AxisRscC[ax_no];
//
//		/* �A���[�����b�`���� */
//		AlmLatchMonitorSequence( AxisRsc->hAlmLatchHndl );
//	}
	/* <S154> End */
	/*----------------------------------------------------------------------*/
	/*	@@ Serial Interface													*/
	/*----------------------------------------------------------------------*/
	KpxSifMsgManager( ); 				/* Serial Interface Manager 	 	*/
	KpxVirSifManager( ); 				/* Virtual Serial Interface Manager */
	/*----------------------------------------------------------------------*/

	/*----------------------------------------------------------------------*/
	/*		�^�C���X�^���v����												*/
	/*----------------------------------------------------------------------*/
	KlibCountTimeStamp( KPI_SCCYCLEMS, PowerManager->PowerSts.PowerOnEep );

	/*----------------------------------------------------------------------*/
	/*		ScanC�������ԃ��j�^�p����				<S043>					*/
	/*----------------------------------------------------------------------*/
	/* ���荞�݂��l���� ScanA,ScanB�����̍��v���Ԃ��Ɍv�Z���� */ /*<S1CD>*/
	SABinC_exeTime = TaskProcessTime->SAinC_exeTime + TaskProcessTime->SBinC_exeTime;
	/* ScanC���s����ScanA,ScanB�������Ԃ����������Ԃ�ScanC�������ԂƂ��� */
	TaskProcessTime->SC_crtTime = KlibGetLongCounter(ScanCStartTime)
									- SABinC_exeTime;							/*<S1CD>*/
//<S1CD>									- TaskProcessTime->SAinC_exeTime	/*<S042>*/
//<S1CD>									- TaskProcessTime->SBinC_exeTime;	/*<S042>*/
	if( TaskProcessTime->SC_crtTime > TaskProcessTime->SC_maxTime)
	{/* �������ԍő�l�X�V */
		TaskProcessTime->SC_maxTime = TaskProcessTime->SC_crtTime;
	}

	/* ScanC���s�t���O���Z�b�g <S043> */
	TaskProcessTime->SC_exeflg = FALSE;
	/* ScanC��ScanA,ScanB���s���ԃN���A */ /*<S1CD>*/
	TaskProcessTime->SAinC_exeTime = 0;
	TaskProcessTime->SBinC_exeTime = 0;



#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
//	HALdrv_ClearDebugSP( INTERRUPT_ScanC );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */
#endif

}


/****************************************************************************
  Description: Return Scan C Counter
-----------------------------------------------------------------------------
  Parameters	:
  Return		:
		none
		

		��ŏ���
*****************************************************************************/
LONG	CpiGetScanCCnt(){ return TEST_SCANC_CNT; }


/**************************** end of file **********************************/
