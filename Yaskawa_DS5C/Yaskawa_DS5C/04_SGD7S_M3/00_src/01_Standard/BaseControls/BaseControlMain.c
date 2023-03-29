/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseControlMain.c : サーボベースコントロールメイン処理定義									*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
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

	/* ローカル変数の初期化 */
//	MencV	= AxisRscB->MencV;
	BaseControls	= AxisRscB->BaseControls;
	SvControlIf		= AxisRscB->SvControlIf;
//	PosManager		= &(BaseControls->PosManager);


/*--------------------------------------------------------------------------------------------------*/
/*		ポインタ初期化																		 <S004> */
/*--------------------------------------------------------------------------------------------------*/
	BaseControls->MencV = AxisRscB->MencV;
	BaseControls->FencV = AxisRscB->FencV;															/* <S04D> */
	
/*--------------------------------------------------------------------------------------------------*/
/*		モータステータス初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(BaseControls->MotSts), sizeof(BaseControls->MotSts)/4 );

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ基本制御変数初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* 位置制御変数初期化 */
//	MlibResetLongMemory( &(AxisRscB->hPosCtrl->var), sizeof(AxisRscB->hPosCtrl->var)/4 );
	/* 速度制御変数初期化 */
//	MlibResetLongMemory( &(AxisRscB->hSpdCtrl->var), sizeof(AxisRscB->hSpdCtrl->var)/4 );
	/* トルクフィルタ変数初期化 */
//	MlibResetLongMemory( &(BaseControls->TrqFilter.var), sizeof(BaseControls->TrqFilter.var)/4 );
	/* ゼロ速停止制御変数初期化 */
	MlibResetLongMemory( &(BaseControls->ZeroClampCtrl.var), sizeof(BaseControls->ZeroClampCtrl.var)/4 );
	/* サーボゲイン切替用変数初期化 */
	GselInitServoGainChange( &(BaseControls->GainChange) );
#if (CSW_SVCF_EHVOBS_USE != 0)
	/* 位相補償速度オブザーバ変数初期化 */
	EhSpeedObserverInitialize( &(BaseControls->EhVobsCtrl) );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ制御位置管理初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_SVCF_BACKLASHCOMP != 0)
	/* バックラッシ補正無効 */
	BlshInitializeCompensate( &(BaseControls->PosManager.BlshCmp) );
#endif
	/* 位置管理変数初期化処理 */
	PosMngInitPositionManager( BaseControls );
	InitBasePositionControl( &(AxisRscB->BaseLoops->PosCtrl) );		/* <S14C> */
	
	/* 位置指令差分出力[Pulse/Scan]のクリア */
	BaseControls->CtrlCmdMngr.dPosRefo = 0;
/* <S1D0> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		速度フィードフォワード移動平均機能用変数初期化														*/
/*--------------------------------------------------------------------------------------------------*/
	FFCmdFilInitialize( &(BaseControls->VFFcmdFil) );

/*--------------------------------------------------------------------------------------------------*/
/*		トルクフィードフォワード移動平均機能用変数初期化														*/
/*--------------------------------------------------------------------------------------------------*/
	FFCmdFilInitialize( &(BaseControls->TFFcmdFil) );
/* <S1D0> End */
/*--------------------------------------------------------------------------------------------------*/
/*		テーブル運転実行処理初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
	TblDriveInitialize( &(BaseControls->TblDrive), AxisRscB->TableRefBuffer, AxisRscB->TrcHndl );

/*--------------------------------------------------------------------------------------------------*/
/*		サーボ拡張制御変数初期化																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_SVCF_DISTURBOBS != 0)
	/* 外乱オブザーバ変数初期化 <S09B> */
	DisturbObserverInitialize( &(BaseControls->DobsCtrl), &(BaseControls->GainChange.GselDobs[0]) );
#endif

	/* 調整レス関係変数初期化 */
	TuneLessCtrlInitialize( &(BaseControls->TuneLessCtrl) );

#if( CSW_SVCF_VIB_DETECT_USE != 0 )
	/* 振動検出演算用変数初期化 */
	DetVibObsInitialize( AxisRscB->DetVib );
#endif

#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	/* 制振制御変数初期化 */
	ResVibATypeResVibInitialize( &(BaseControls->ResVib) );
#endif

	/* 振動検出オブザーバによる速度偏差トレース初期化 */
	DetVibTraceInitialize( AxisRscB->DetVib );


	/* NET制御コード変数初期化 */
	SvControlIf->NetCtrlMcmd = CTRLMODE_NOCMD;
	BaseControls->TrqLimitData.var.FwTrqLmt = NORMAXVALUE;
	BaseControls->TrqLimitData.var.RvTrqLmt = NORMAXVALUE;

/*--------------------------------------------------------------------------------------------------*/
/*		整定時間測定関連処理変数初期化																*/
/*--------------------------------------------------------------------------------------------------*/
	/* Initialize Settling Time Variable and Start Settling Time Process */
	StlgInitSettlingTime( &(AxisRscB->BaseControls->SettlingTime),
						  AxisRscB->UniPrms->Bprm->SvCycleUs );

/*--------------------------------------------------------------------------------------------------*/
/*		制御パラメータ変換サービス初期化															*/
/*--------------------------------------------------------------------------------------------------*/
	TranlateCtrlPrmInitialize( &(AxisRscB->BaseControls->TransduceCtrl) );

/*--------------------------------------------------------------------------------------------------*/
/*		その他未初期化変数初期化(暫定)																*/
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
	MpFind			= AxisRscB->MpFind;		/* コメントアウト解除<S02F> */
	UnMonitor		= AxisRscB->UnMonitor;
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	IoSignals 		= AxisRscB->IoSignals;	/* <S045> */
#endif
	Bprm			= AxisRscB->UniPrms->Bprm;	/* <S04D> */
	BaseLoops		= AxisRscB->BaseLoops;

#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/*--------------------------------------------------------------------------------------------------*/
/*		Set Encoder Error Alarm																		*/
/*--------------------------------------------------------------------------------------------------*/
	lwk = (LONG)AxisRscB->SvAsicRegs->MicroBitDat.data;
	if( lwk & BITDAT_ABF3ALM )
	{ /* A.BF3 : ScanA同期異常 */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_SCANA_STOP );
	}
	if( lwk & BITDAT_AC90ALM )
	{ /* A.C90 : エンコーダ通信異常 */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_PGCOM );
	}
	if( lwk & BITDAT_AC91ALM )
	{ /* A.C91 : エンコーダ加速度異常 */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_PGCOM1 );
	}
	if( lwk & BITDAT_AC20ALM )
	{ /* A.C20 : ScanA同期異常 */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_PHASE );
	}
	if( lwk & BITDAT_AC21ALM )
	{ /* A.C21 : ScanA同期異常 */
		ALMSetServoAlarm( AxisRscB->AlmManager, ALM_POLESENSOR );
	}
#endif
	SvControlIf->CmdReady = MencV->PhaseReady;

/*--------------------------------------------------------------------------------------------------*/
/*		制御用パラメータ転送処理																	*//* <S066> */
/*--------------------------------------------------------------------------------------------------*/
	/* Control Parameter Translate */
	if( TR_REQ_SCANB == TranlateCtrlGetReq(&(BaseControls->TransduceCtrl)) )
	{
		TranlateCtrlPrm( &(BaseControls->TransduceCtrl) );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		制御指令入力処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	BpiInputControlCMD(BaseControls, SvControlIf, CtrlLoopOut,
						SeqCtrlOut, ((AxisRscB->UniPrms->Prm->b_prm0>>4) & 0x0F), ax_no);/* <S1C0>:軸追加 */

/*--------------------------------------------------------------------------------------------------*/
/* 		<S045> H/WからののPCL/NCL信号入力(不要なら消す) 											*/
/*--------------------------------------------------------------------------------------------------*/
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	BaseControls->TrqLimitData.var.PclSignal |= IoSignals->PclSignal.SignalStatus.v;
	BaseControls->TrqLimitData.var.NclSignal |= IoSignals->NclSignal.SignalStatus.v;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		スキャンＢサーボ制御処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->PhaseReady == FALSE )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		磁極検出制御処理																		*/
	/*----------------------------------------------------------------------------------------------*/
		/* Position Reference for Motor Pole Find */
		BpiMpoleFindMakePosCmd( BaseLoops, SvControlIf, AxisRscB->FnCmnCtrl, Bprm );		/*<S02F>*/

		/* Prepare Motor Pole Find */
		/* モータ速度検出用移動平均時定数変更 */
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
				/* モータ速度検出用移動平均時定数を戻す */
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
		BaseControls->CtrlCmdMngr.SpdLimit = 0x1000000;	/* 速度制限なし	*/

		/* Speed Loop Control for Motor Pole Find */
		BaseControls->CtrlCmdMngr.TrqRefo = MpoleFindSpeedControl( MpFind,
																&(MpfExeCondition),
																BaseControls->BeComplete );

		BaseControls->CtrlCmdMngr.SpdLimit = 0x1000000;	/* 速度制限なし	*/
		BaseControls->CtrlCmdMngr.CtrlMode = CTRLMODE_TRQ;	/* トルク制御モード */
	}
	else
	{
	/*----------------------------------------------------------------------------------------------*/
	/*		サーボ拡張制御																			*/
	/*----------------------------------------------------------------------------------------------*/
#if !defined(_TEST_BOARD_) || defined(_VC_DEBUG_)	/* 2012.12.13 Y.Oka 処理時間対策 */

#if( CSW_SVCF_VIB_DETECT_USE != 0 )
		/* 振動検出速度オブザーバ演算 */
		if(AxisRscB->UniPrms->Bprm->PsFulcSelect != FALSE)/* <S04D>:ﾌﾙｸ処理追加 */
		{/* フルクローズド制御	*/
			DetVibSpeedObserver( DetVib, BaseControls->MotSts.FencSpd, CtrlLoopOut->TrqRefMon_a );
		}
		else
		{/* セミクローズド制御	*/
		DetVibSpeedObserver( DetVib, BaseControls->MotSts.MotSpd, CtrlLoopOut->TrqRefMon_a );
		}
		/* 発振検出用最大・最小速度偏差設定処理 */
		//110208_kira_for_AAT
		if( (FnCmnCtrl->State.TuningPctrlRun) || (FnCmnCtrl->State.TuningRun) )
		{ /* AAT実行中 */
			DetVibSetSpeedError( DetVib, BaseControls->TuneLessCtrl.conf.TuningLessUse, TRUE );
		}
		else
		{ /* AAT実行中以外 */
			DetVibSetSpeedError( DetVib, BaseControls->TuneLessCtrl.conf.TuningLessUse, FALSE );
		}
#endif /* #if( CSW_SVCF_VIB_DETECT_USE != 0 ) */

		/* 振動検出オブザーバによる速度偏差トレース */
		DetVibTraceRuntimeService( DetVib );

#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
		if( BaseControls->ResVib.AresVibSetting == TRUE )
		{ /* A型制振有効時 */
			/* A型制振制御演算 */
			BaseControls->CtrlCmdMngr.SpdFBCx -=
				ResVibATypeResVibCtrl( &(BaseControls->ResVib),
									   BaseControls->CtrlCmdMngr.SpdObsFbki,
									   CtrlLoopOut->TrqRefo_a );
		}
#endif /* #if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 ) */

#endif /* #if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_) */

	/*----------------------------------------------------------------------------------------------*/
	/*		安川サーボ制御メイン																	*/
	/*----------------------------------------------------------------------------------------------*/
		BpiYecServoControl( BaseLoops, SvControlIf, CtrlLoopOut, FnCmnCtrl, Bprm);	/* <S04D> */

	/*----------------------------------------------------------------------------------------------*/
	/*		サーボ応答位置管理																<S14C>	*/
	/*----------------------------------------------------------------------------------------------*/
		PosMngResponseManager( BaseControls );

	/*----------------------------------------------------------------------------------------------*/
	/*		フルク位置ループ制御																	*/
	/*----------------------------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_SB_SUPPORT)/* 使用しない：ソフト整理時に削除可 */
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
/*		トルクリミット演算処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	BpiTorqueLimitControl( &(BaseControls->TrqLimitData),
						   BaseControls,
						   SeqCtrlOut->TrqLimitReq,
						   AxisRscB->AlmManager );	/* <S145> */


/*--------------------------------------------------------------------------------------------------*/
/*		整定時間関連処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	StlgCalculateSettlingTime( &BaseControls->SettlingTime,
							   BaseControls->PosManager.var.dPcmda,
							   BaseControls->PosCtrlSts.CoinSignal );

/*--------------------------------------------------------------------------------------------------*/
/*		最大オーバーシュート量作成 --> Un106														*/
/*--------------------------------------------------------------------------------------------------*/
	StlgMakeMaxOverShoot( &BaseControls->SettlingTime,
						  &(BaseControls->PosManager),
						  (CTRLMODE_POS == BaseControls->CtrlModeSet.CtrlMode.b.cm) );

/*--------------------------------------------------------------------------------------------------*/
/*		位置決め完了失敗累積時間作成 --> Un10B														*/
/*--------------------------------------------------------------------------------------------------*/
	StlgMakeCoinOffTime( &BaseControls->SettlingTime,
						 BaseControls->PosManager.var.dPcmda,
						 BaseControls->PosCtrlSts.CoinSignal,
						 (CTRLMODE_POS == BaseControls->CtrlModeSet.CtrlMode.b.cm) );
						 
/* 2013/04/01MD版は、下記ﾓﾆﾀを作成しない。なぜ？	*/
/*--------------------------------------------------------------------------------------------------*/
/*		モニタ用変数出力																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* 速度指令(アナログモニタデータ設定) */
	UnMonitor->AmonSpdRef = BaseControls->CtrlCmdMngr.SpdRefo + BaseControls->CtrlCmdMngr.SpdFFCx;
//	UnMonitor->AmonSpdRef = BaseControls->CtrlCmdMngr.SpdRefo;//S0CD
	/* トルク指令(アナログモニタデータ設定) */
	UnMonitor->AmonTrqRef = CtrlLoopOut->TrqRefMon;
	/* 位置指令モニタ	[指令単位/Scan]		*/
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
/*		磁極検出サーボオン要求作成作成													 <S21C> 	*/
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
	AC22 = ALMGetOtherAlarmChk( Axis->AlmManager, ALM_PHASEANGLE );	/* 磁極検出許可アラームA.C22 <S226> */
	
	if( MencV->AxisMotType == MOTTYPE_LINEAR )						/* リニアモータ					*/
	{
		if( FnCmnCtrl->FnSvControl == FALSE )						/* 通常中						*/
		{
			if( MencV->PdetReq587 == TRUE )							/* Pn587.0:磁極検出要求 <S226> */
			{
				if(( Alm == TRUE )&&( AC22 == FALSE ))				/* <S226> */
				{
					AlmClearReq( Axis->AlmManager, ALM_PHASEANGLE );	/* A.C22アラームをクリア  */
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
			if(( SvControlIf->CmdSeqBit & SERVO_ON_BIT ) != 0 )		/* サーボオン要求		 <S226> */
			{
				AlmCode = ALM_PDET;									/* 磁極検出未完					*/
			}														/*						 <S226> */
		}
		else														/* ROUND制御中					*/
		{
			if( Alm == FALSE )
			{
				bwk = FnCmnCtrl->PdetReq;
				if( FnCmnCtrl->FnSvonReq == TRUE )					/* サーボオン要求		 <S226> */
				{
					AlmCode = ALM_PDET;								/* 磁極検出未完					*/
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
