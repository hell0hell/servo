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
void IntScanC( void ); /* 暫定処置 */
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

	/* 処理時間測定用GPIO ON */;
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

	/* 処理時間測定用GPIO OFF */
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
	ULONG 			CurrentTimerCnt;	/* 現在稼働時間	*/
	SHORT 			CurrentTemperature;
	LONG			PreWrnSts;
#endif
	/* <S154> End */
	ASICS				*svAsicRegs;		/*<S126>*/
	volatile ULONG		SABinC_exeTime;		/*<S1CD>*/
	BASE_CTRL_OUT		*BaseCtrlOut;		/* <S21D> */
	SEQ_IO_HNDL			*IoSignals;			/* <S21D> */

	/* 処理時間測定用GPIO ON */;
#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
//	HALdrv_OutputDebugSP( INTERRUPT_ScanC );		/* 割込みの分岐処理でポート出力に変更<S042> */
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

	/* ScanC実行フラグセット <S043> */
	TaskProcessTime->SC_exeflg = TRUE;

	/*----------------------------------------------------------------------*/
	/*	前2ms間の実行時間計算処理				<S043>						*/
	/*----------------------------------------------------------------------*/
	TaskProcessTime->crtTimePer2ms = TaskProcessTime->SA_sumTime
										+TaskProcessTime->SB_sumTime
										+TaskProcessTime->SC_crtTime;
	TaskProcessTime->SA_sumTime = 0;	/* 累積処理時間リセット */
	TaskProcessTime->SB_sumTime = 0;	/* 累積処理時間リセット */
	if( TaskProcessTime->crtTimePer2ms > TaskProcessTime->maxTimePer2ms )
	{
		TaskProcessTime->maxTimePer2ms = TaskProcessTime->crtTimePer2ms;
	}


	/* スキャンカウントが1より大きく、ネットワーク安定時にアラームチェックを行う */ /*<S1DB>*/
	if( (TEST_SCANC_CNT > 1) && (OpiCheckStableState( AxisRscC ) == FALSE) )
	{
/* <S1CD> >>>>> */
		/* スキャンＣサイクル時間オーバーチェック(カウント値で比較) */
		if( TaskProcessTime->crtTimePer2ms > ((KPI_SCCYCLEMS * 1000000) / ITMR_NSEC_PER_COUNT) )
		{
			/* A.BF0 : システムアラーム０(内部プログラム処理異常) */
			ALMSetGlobalAlarm( ALM_SAMP_OVER );
		}
/* <<<<< <S1CD> */
	}
	else
	{
		TaskProcessTime->maxTimePer2ms = 0;		/* 2ms最大処理時間をリセットする <S1F3> */
		OpiResetSyncExe( AxisRscC );
	}

	/*----------------------------------------------------------------------*/
	/*	ユニット共通ScanC入力処理											*/
	/*----------------------------------------------------------------------*/
	SeqInputSignals = HALdrv_ReadInputSignals( );
	PcmInputPowerStatus( PowerManager,
						 SeqInputSignals.flag.Acon1,
						 SeqInputSignals.flag.RegeneAlm );
	/*----------------------------------------------------------------------*/
#if (SVFSEL_ADC_USE == 1)||(SVFSEL_ONLY_OH_ADC_USE == 1)		/* <S09F> *//* <S0E1>:SVFSEL_ONLY_OH_ADC_USE追加 */
	KpxAdcScanCService( AxisRscC->pAdc );		/* A/D converter ScanC service */
#endif                     /* <S09F> */
	KpxPlsScanCService( AxisRscC->pPlsIfV );	/* Command Pulse counter ScanC service */
	/*----------------------------------------------------------------------*/
	SeqIoInputSeqSignalsCom( IoSignals, AxisRscC->UniPrms->Prm );		/* <S21D> */

	/* < S154> Start  */
	/*----------------------------------------------------------------------*/
	/*		平均内部温度算出処理 											*/
	/*----------------------------------------------------------------------*/
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	CurrentTimerCnt = KlibGetTimeStamp();		/* 現在稼働時間取得 */
	/* 累積稼働時間カウントから30分を求めるために、14bit右シフトを用いる。	*/
	/* 累積稼働時間の単位が[100ms]であることから、						*/
	/* 30分で18000カウント≒16384 = 2^14から来るものである。				*/
	if ( ((CurrentTimerCnt - hAvgTmprHdl->var.Timer100msCntLast) >> 14) > 0 )
	{	/*30 分周期処理*/
	
		/* 現在最高温度取得  */
		CurrentTemperature = DetTempGetInternalTemperature(AxisRscC->TempManager);

		/*	平均内部温度算出処理 	*/
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

		/* @@多軸対応時再検討すること 2013.07.14 Y.Oka (>_<) */
		/*------------------------------------------------------------------*/
		/* ユニット処理用各軸データ取得 */
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
			{/* A.7A0 : ヒートシンク過熱				*/
				ALMSetServoAlarm( AxisRsc->AlmManager, ALM_OH );
			}
		}
		if((OH_ALM_SLOT_GRP2 >> AxisRsc->AxisNo) & 1)
		{
			faultState2 |= AxisRsc->SeqCtrlData->faultState;
			if(0 != (SHAL_FAULT_OH & faultState2))
			{/* A.7A0 : ヒートシンク過熱				*/
				ALMSetServoAlarm( AxisRsc->AlmManager, ALM_OH );
			}
		}
		AxisRsc++;
	}
#endif	//CSW_BSP_SGDZ_IFBS80

	/*----------------------------------------------------------------------*/
	/* 電源管理処理実行(ユニット単位) */
	/*----------------------------------------------------------------------*/
#if (CSW_HW_POWER_MANAGE == TRUE)
	/* <S21D> Start */
	/*----------------------------------------------------------------------*/
	/*		シーケンス入力信号を読み出し											*/
	/*----------------------------------------------------------------------*/
	PowerManager->PowerSts.ClinkAnsSig =
			IoSignals->ClinkansSignal.SignalStatus.v;				/* コンデンサバンク接続監視入力信号	*/
	PowerManager->PowerSts.CmcAnsSig =
			IoSignals->CmcansSignal.SignalStatus.v;					/* 突防リレー状態監視入力信号		*/
	PowerManager->PowerSts.ConvRdySig =
			IoSignals->ConvreadySignal.SignalStatus.v;				/* コンバータレディ入力信号			*/
	PowerManager->PowerSts.ConvMconSig =
			IoSignals->ConvmconSignal.SignalStatus.v;				/* 突入電流防止用リレー入力信号		*/
	/* <S21D> End */
//	PcmPowerManager( PowerManager, MotStopSts );							  /* <S0F6> */
	PcmPowerManager( PowerManager, MotStopSts, BaseEnableSts );				  /* <S0F6S0F6> */
#endif

	/* 回生過負荷検出処理実行(ユニット単位) */
#if (CSW_HW_REGENERATION_CTRL == TRUE)
	/*----------------------------------------------------------------------*/
	/*	Delta Regene.Tr On Count for ScanC Alarm Check						*/
	/*----------------------------------------------------------------------*/
	/* 回生トランジスタON時間計測 */
	PcmCountRegeneOnTime( PowerManager );

	/* 回生過負荷チェック */
#if 0		/* <S21D> */
	if( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)
		&& (PowerManager->MainPowerChecker.conf.TestMode2 == FALSE) )
	{ /* AC電源入力 && !TestMode2 */
#else		/* <S21D> */
	if( ( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)			/* AC電源入力 */
	   || ( (PowerManager->MainPowerChecker.conf.Dcbus == TRUE)			/* DC電源入力 */
		 && (PowerManager->RegeneCtrl.conf.RegSyn == PNREGSYN) ) )		/* PN母線共通システムにおいて回生同期機能を使用 */
	 && (PowerManager->MainPowerChecker.conf.TestMode2 == FALSE) )		/* !TestMode2 */
	{
#endif
		if( ALMCheckGlobalAlarm(WRN_EXREG) == FALSE )					/* A.924（回生警告）なしの場合 *//* <S21D> */
		{
			ULONG	AlmOLInfo = (AxisRscC->AlmManager)->Status.AlmOLInfo; 		/* <S0FF> */
			AlmWrnSts = PcmCheckRegeneOverLoad( &(PowerManager->RegeneCtrl),
											   &AlmOLInfo, 						/* <S0FF> */
											   AxisRscC->UnMonitor);
			if( AlmWrnSts != CHK_REGOL_NO_OPERATION )
			{
				/* 回生過負荷アラーム・ワーニング設定 */
				if(0 != (AlmWrnSts & CHK_REGOL_SET_ALARM))
				{	/* A.320 : 回生過負荷 */
					ALMSetGlobalAlarm( ALM_RGOL );
				}
				else if(0 != (AlmWrnSts & CHK_REGOL_SET_WARNING))
				{	/* A.920 : 回生過負荷ワーニング */
					ALMSetGlobalAlarm( WRN_RGOLF );
				}
				else
				{	/* A.920 : 回生過負荷ワーニング */
					ALMClearGlobalAlarm( WRN_RGOLF );
				}
			}
		}
	}
#endif

#if 0 /* 軸毎処理へ移動 *//* <S0A2> */
	/************************************/
	/* 突入防止回路制御(分離)			*/
	/************************************/
	PcmRlyControlProcess( &PowerManager->MainPowerChecker,
						  &PowerManager->PowerSts,
						  FaultState,
						  DBRequest,
						  AxisRscC->FnCmnCtrl->KnlMode );
#endif

	/* 突入防止抵抗過負荷検出処理(ユニット単位) */
#if (CSW_HW_CHKRUSH == TRUE)
	if( CHK_RUSH_NO_OPERATION != PcmCheckRushRxOverLoad( &(PowerManager->Rush),
														 &(PowerManager->PowerSts) ) )
	{	/* A.740 : 突入抵抗過負荷 */
		ALMSetGlobalAlarm( ALM_RSOL );
	}
#endif

	/* 回生異常検出処理(ユニット単位) */
#if (CSW_HW_REGENERATION_CTRL == TRUE)
#if 0		/* <S21D> */
	if( PowerManager->MainPowerChecker.conf.Dcbus == FALSE )
	{
		if( CHK_REGENE_NO_OPERATION != PcmCheckRegeneAlarm( &(PowerManager->RegeneCtrl),
														    PowerManager->PowerSts.PowerOn) )
		{	/* A.300 : 回生異常 */
			ALMSetGlobalAlarm( ALM_RG );
		}
	}
#else		/* <S21D> */
	if( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)
	 || ( (PowerManager->MainPowerChecker.conf.Dcbus == TRUE)			/* DC電源入力 */
	   && (PowerManager->RegeneCtrl.conf.RegSyn == PNREGSYN) ) )		/* PN母線共通システムにおいて回生同期機能を使用 */
	{
		if( CHK_REGENE_NO_OPERATION != PcmCheckRegeneAlarm( &(PowerManager->RegeneCtrl),
														    PowerManager->PowerSts.PowerOn) )
		{
			if( (PowerManager->RegeneCtrl.conf.RegSyn == PNEXREG)		/* 外部回生機能を利用	*/
			 && (PowerManager->RegeneCtrl.conf.RegRxIn == FALSE) )		/* 回生抵抗外置き		*/
			{
				if( PowerManager->MainPowerChecker.var.WrnExRegInhibit == FALSE )
				{ /* A.924 : 回生警告 */
					ALMSetGlobalAlarm( WRN_EXREG );
					PowerManager->MainPowerChecker.var.WrnExRegInhibit = TRUE;
				}
			}
			else
			{ /* A.300 : 回生異常 */
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
	/* サーボパック内蔵ファン停止検出処理(ユニット単位) */
	AlmWrnSts = PcmCheckFanLock( &(PowerManager->FanLock),
								 SeqInputSignals.flag.FanAlm,
								 BaseEnableSts );
	/* サーボパック内蔵ファン停止アラーム・ワーニング設定 */
	if(0 != (AlmWrnSts & CHK_FANLOCK_SET_ALARM))
	{	/* A.7AB : サーボパック内蔵ファン停止アラーム */
		ALMSetGlobalAlarm( ALM_FANLOCK );
	}
	else if(0 != (AlmWrnSts & CHK_FANLOCK_SET_WARNING))
	{	/* A.923 : サーボパック内蔵ファン停止 */
		ALMSetGlobalAlarm( WRN_FANLOCK );
	}
	else
	{
		;
	}
#endif


	/* 消費電力量計算(ユニット単位) *//* <S020> */
#if (CSW_POWER_MONITOR_USE == TRUE)
	SetPowerMoniter( AxisRscC->PowerMonitor, GetPowerConsumption(AxisRscC->CalcWattage) );
#endif

	/* OC検出処理	*//* <S126> */
	svAsicRegs = AxisRscC[0].SvAsicRegs;		/* OC検出は、0軸目のUDL領域を使用する。	*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];
		AdetCheckOCAlarmDetections(AxisRsc->AlmManager, svAsicRegs, ax_no, AxisRsc->SeqCtrlOut->UdlBeStatus);		/* <S137> */

		/* モータ過電流検出チェック処理	*//* <S1F7> */
		AdetCheckMotorOverCurrent( AxisRsc->AlmManager, AxisRsc->SvAsicRegs, AxisRsc->SeqCtrlOut->UdlBeStatus,
																								BaseEnableSts );
	}

	/* モータ過電流検出チェック処理	*//* <S1F5> */
//	AdetCheckMotorOverCurrent( AxisRsc->AlmManager, svAsicRegs, AxisRsc->SeqCtrlOut->UdlBeStatus,
//			BaseEnableSts );/*<S1F5> */


	/* < S154> Start  */
	/*----------------------------------------------------------------------*/
	/*		寿命予測機能処理												*/
	/*----------------------------------------------------------------------*/
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	/* <S180> Start */
#if 0
	AxisRscC->hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk = PowerManager->PowerSts.ChargeOk;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];
		/* DBリレー動作回数はモータ回転中にDBオンした場合のみカウントアップする 、軸対応*/
		LifeTimeDBSwitchingCount( AxisRscC->hLifeTime, AxisRsc->AdatHndl->AdatP.SeqCtrlOut->TgOnSts , AxisRsc->BeSequence->DbOn);
	}
	/* 寿命残存率計算Main処理													*/
	LifeTimeSurvivorCalculateMain( AxisRscC->hLifeTime , AxisRscC->RegManager->hPnReg , AxisRscC->UniPrms->Cprm );

	/* 予防保全ワーニング出力 */
	PreWrnSts = AdetCheckPreWarningSigOut( AxisRscC->hLifeTime,
								&(AxisRscC->PowerManager->MainPowerChecker) );
	if( PreWrnSts == CHK_PRE_SET_WRN )
	{
		/* ワーニング（A.9B0）を出力する */
		ALMSetGlobalAlarm( WRN_PREVENTMAINT );
	}
#else
	AxisRscC->hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk = PowerManager->PowerSts.ChargeOk;		/* <S192> */
	/* 寿命残存率計算Main処理													*/
	LifeTimeSurvivorCalculateMain( AxisRscC->hLifeTime, AxisRscC->RegManager->hPnReg, AxisRscC->UniPrms->Cprm );

//	AxisRscC->hLifeTime->RushLifeTimeHdl.var.CurrentChargeOk = PowerManager->PowerSts.ChargeOk;	/* <S192> */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];
		/* DBリレー動作回数はモータ回転中にDBオンした場合のみカウントアップする 、軸対応*/
		LifeTimeDBSwitchingCount( AxisRsc->hDBLifeTime, AxisRsc->AdatHndl->AdatP.SeqCtrlOut->TgOnSts,
								AxisRsc->BeSequence->DbOn);
		/* 寿命機能用パラメータ書き込みTO　Eeprom */
//		LifeTimePrmStoreValueToEeprom( AxisRsc->hDBLifeTime,AxisRsc->RegManager->hPnReg,
//								AxisRsc->UniPrms->Cprm);		/* <S186> *//* <S192> */
		/* DB寿命残存率計算関数 */
		LifeTimeDBSurvivorCalculate( AxisRsc->hDBLifeTime );

		/* 予防保全ワーニング出力 */
		PreWrnSts = AdetCheckPreWarningSigOut( AxisRscC->hLifeTime,
											&(AxisRscC->PowerManager->MainPowerChecker),
											AxisRsc->hDBLifeTime->var.DBSurvivor);
		if( PreWrnSts == CHK_PRE_SET_WRN )
		{
			/* ワーニング（A.9B0）を出力する */
//			ALMSetGlobalAlarm( WRN_PREVENTMAINT );						/* <S192> */
			ALMSetServoAlarm( AxisRsc->AlmManager, WRN_PREVENTMAINT );	/* <S192> */
		}
	}
#endif
	/* <S180> End */
#endif
	/* <S154> End  */

	/*----------------------------------------------------------------------*/
	/* スキャンＣメイン関数実行 */
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscC[ax_no];

		SyscAlarmDetections( AxisRsc );

		SyscBaseDriveSequence( AxisRsc );

		SyscSequenceMain( AxisRsc );

		SyscSencAbsoOperation( AxisRsc ); /* 絶対値エンコーダ処理メイン     *//*<S02D>*/
	}
	/*----------------------------------------------------------------------*/
	/*		スキャンC共通出力処理											*/
	/*----------------------------------------------------------------------*/
	SyscSequenceCommon( AxisRscC );

/* <S150> >>>>> 下記、USBファンクション用の割り込みを使用せず、ポーリングで検出 */
	/*----------------------------------------------------------------------*/
	/*	USB Function Interface												*/
	/*----------------------------------------------------------------------*/
	UsbFuncIntIntu2f();					/* プラグイン／プラグアウト処理				*/
	UsbFuncIntIntu2fepc();				/* バス・リセット／バス・スピード変更			*/
/* <<<<< <S150> */

	/* <S154> Start */
	/*----------------------------------------------------------------------*/
	/* アラームラッチメイン関数実行 */
	/*----------------------------------------------------------------------*/
/* <S212> */
//	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
//	{
//		AxisRsc = &AxisRscC[ax_no];
//
//		/* アラームラッチ処理 */
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
	/*		タイムスタンプ処理												*/
	/*----------------------------------------------------------------------*/
	KlibCountTimeStamp( KPI_SCCYCLEMS, PowerManager->PowerSts.PowerOnEep );

	/*----------------------------------------------------------------------*/
	/*		ScanC処理時間モニタ用処理				<S043>					*/
	/*----------------------------------------------------------------------*/
	/* 割り込みを考慮し ScanA,ScanB処理の合計時間を先に計算する */ /*<S1CD>*/
	SABinC_exeTime = TaskProcessTime->SAinC_exeTime + TaskProcessTime->SBinC_exeTime;
	/* ScanC実行中のScanA,ScanB処理時間を引いた時間をScanC処理時間とする */
	TaskProcessTime->SC_crtTime = KlibGetLongCounter(ScanCStartTime)
									- SABinC_exeTime;							/*<S1CD>*/
//<S1CD>									- TaskProcessTime->SAinC_exeTime	/*<S042>*/
//<S1CD>									- TaskProcessTime->SBinC_exeTime;	/*<S042>*/
	if( TaskProcessTime->SC_crtTime > TaskProcessTime->SC_maxTime)
	{/* 処理時間最大値更新 */
		TaskProcessTime->SC_maxTime = TaskProcessTime->SC_crtTime;
	}

	/* ScanC実行フラグリセット <S043> */
	TaskProcessTime->SC_exeflg = FALSE;
	/* ScanC内ScanA,ScanB実行時間クリア */ /*<S1CD>*/
	TaskProcessTime->SAinC_exeTime = 0;
	TaskProcessTime->SBinC_exeTime = 0;



#if CSW_NETIF_TYPE	!= NETIF_ANLGPLS_TYPE
//	HALdrv_ClearDebugSP( INTERRUPT_ScanC );		/* 割込みの分岐処理でポート出力に変更<S042> */
#endif

}


/****************************************************************************
  Description: Return Scan C Counter
-----------------------------------------------------------------------------
  Parameters	:
  Return		:
		none
		

		後で消す
*****************************************************************************/
LONG	CpiGetScanCCnt(){ return TEST_SCANC_CNT; }


/**************************** end of file **********************************/
