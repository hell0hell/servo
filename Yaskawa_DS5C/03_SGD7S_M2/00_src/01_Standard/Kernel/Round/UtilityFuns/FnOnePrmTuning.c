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

	/* ローカル変数の初期化 */
	OnePrmTune = Axis->OnePrmTune;
	Prm = Axis->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	/* Initialize One Parameter Tuning */
	OnePrmTune->OpeReg.SpatLevel = Prm->loophz;
	OnePrmTune->OpeReg.SpatFFLevel = Prm->mdlgn;
	RpiRegSetOnePrmTuningLevel( OnePrmTune, Prm->loophz );
	RpiRegSetOnePrmTuningFfLevel( OnePrmTune, Prm->mdlgn );

	if( Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse == TRUE )
	{ /* 調整レス有効時 */
		rc = PRM_RSLT_CONDITION_ERR;
	}

	/* モータレステストモードでない場合は入らずに終了する *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Get Original Gains */
	OnePrmTune->var.OrgKv    = Prm->loophz;		/* Pn100 : 速度ループゲイン 			*/
	OnePrmTune->var.OrgMdlgn = Prm->mdlgn;		/* Pn141 : モデル追従制御ゲイン[0.1/s]	*/
	OnePrmTune->var.OrgNf1   = Prm->ntchfil1;	/* Pn409 : 1段目ノッチフィルタ周波数 	*/
	OnePrmTune->var.OrgNf2   = Prm->ntchfil2;	/* Pn40C : 2段目ノッチフィルタ周波数 	*/
	OnePrmTune->var.OrgNfQ1  = Prm->ntchq1;		/* Pn40A : 1段目ノッチフィルタQ値 		*/
	OnePrmTune->var.OrgNfQ2  = Prm->ntchq2;		/* Pn40D : 1段目ノッチフィルタQ値 		*/
	OnePrmTune->var.Jrate    = Prm->jrate;		/* Pn103 : 慣性モーメント比 			*/

	/* Get Atype Resvib Gain */
	OnePrmTune->var.OrgAResf   = Prm->avibfrq;	/* Pn161 : Ａ型制振周波数 				*/
	OnePrmTune->var.OrgAResDGn = Prm->avibdamp;	/* Pn163 : Ａ型制振ダンピングゲイン 	*/

	/* Reset One Parameter Tuning Gain */
	OnePrmReSetTuningLevel( OnePrmTune,
							Prm->loophz,
							Prm->posgn,
							Axis->BaseControls->CtrlCmdPrm.IPSpdControl );

	OnePrmTune->OpeReg.SpatFFLevel = OnePrmTune->var.OrgMdlgn;
	OnePrmTune->var.FfSetting      = OnePrmTune->var.OrgMdlgn;
	OnePrmTune->var.TunRun         = TRUE;

	/* Fnモード自動終了なし */
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

	/* EEPROMデータで変更パラメータを元に戻す */
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

	/* ローカル変数の初期化 */
	OnePrmTune = Axis->OnePrmTune;
	pBaseCtrl  = Axis->BaseControls;
	RegCfg     = Axis->RegManager->hPnReg;
	Prm        = Axis->UniPrms->Prm;
	rc         = PRM_RSLT_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* 振動検出トレース要求処理 */
	OnePrmTune->var.FftStat = TuneVibrationTrace( Axis->DetVib,
												  Axis->FftAna,
												  OnePrmTune->var.FftReq,
												  0,
												  TuneLessGetTuneLessSts(&Axis->BaseControls->TuneLessCtrl) );

	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) == 0x0000 )
	{ /* 自動設定可能なノッチがない */
		/* 自動設定不可ステータス設定 */
		OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
	}

	OnePrmTune->OpeReg.SpatANotchState = (OnePrmTune->var.ANotchPreStat << 12)
									   + (OnePrmTune->var.ANotch2Stat 	<<  8)
									   + (OnePrmTune->var.ANotch1Stat 	<<  4)
									   + (OnePrmTune->var.ANotchStat         );

	/*------------------------------------------------------------------------------------------*/
	/*	レジスタ応答データ作成																	*/
	/*------------------------------------------------------------------------------------------*/
	if( ((Prm->avibsw & 0x0F) == TRUE)
		|| (((Prm->avibsw >> 4) & 0x0F) == FALSE)
		|| ((pBaseCtrl->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ) && (Axis->SeqCtrlOut->BaseEnableReq == TRUE)) )
	{ /* 既にＡ型制振有効 || 自動調整無効 || (トルク制御モード && サーボオン) */
		/* 自動設定不可設定 */
		OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOENABLE;
	}
	else if( OnePrmTune->var.AResStat == OPTUNE_STS_ARES_NOENABLE)
	{ /* Ａ型制振有効条件満足だが、ステータスが設定不可 */
		/* 未検出に初期化 */
		OnePrmTune->var.AResStat  = OPTUNE_STS_ARES_NOVIBDET;
		OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NODETECT;
	}
	/* Store Ares State on the Register */
	OnePrmTune->OpeReg.SpatAresState = (OnePrmTune->var.ARes1Stat << 4) + OnePrmTune->var.AResStat;

	switch( FunExe->State )
	{
	case OPTUNE_SEQ_INIT:
		/*--------------------------------------------------------------------------------------*/
		/*	自動ノッチ実行条件判別(初期化)														*/
		/*--------------------------------------------------------------------------------------*/
		if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) != 0x0000 )
		{ /* 自動ノッチ設定可能 */
			/* 発振検出をマスク(計算関数必要) */
			//Prm->vibsel = 0;
			TunePrmWriteRam( &AccessPrm, &pndef_vibsel, 0, pndef_vibsel.DataLength );/*<S09E>*/
		}
		else
		{ /* 自動ノッチ設定不可条件 */
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
		}

		/*--------------------------------------------------------------------------------------*/
		/*	自動Ａ型制振設定可能チェック(初期化)												*/
		/*--------------------------------------------------------------------------------------*/
		/* 未検出に設定 */
		OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
		OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NODETECT;

		if( ((Prm->avibsw & 0x0F) == TRUE)
			|| (((Prm->avibsw >> 4) & 0x0F) == FALSE)
			|| ((pBaseCtrl->CtrlModeSet.CtrlMode.b.cm == CTRLMODE_TRQ) && (Axis->SeqCtrlOut->BaseEnableReq == TRUE)) )
		{	/* 既にＡ型制振有効 || 自動調整無効 || (トルク制御モード && サーボオン) */
			/* 自動設定不可設定　*/
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOENABLE;
		}
		else
		{
			/* 発振検出をマスク(計算関数必要) */
			//Prm->vibsel = 0;
			TunePrmWriteRam( &AccessPrm, &pndef_vibsel, 0, pndef_vibsel.DataLength );
		}
		/* 振動検出用差速度最大値・最小値リセット */
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

		/* FF設定用変数リセット */
		KlibRstLongTimer( &OnePrmTune->var.FfWaitTimer );
		OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFCOMP;
		break;

	case OPTUNE_SEQ_TUNING:		/* チューニングレベル変更処理	*/
		if( OnePrmTune->var.FftWait == TRUE )
		{
			if( KlibGetLongTimerMs(OnePrmTune->var.FftWaitTimer) > 2000 )
			{
				OnePrmTune->var.FftWait = FALSE;
				DetVibResetMaxMinSpeedError( Axis->DetVib );
			}
		}

		if( OnePrmTune->var.NotchChk == TRUE )
		{ /* ノッチチェック中 */
			if( KlibGetLongTimerMs(OnePrmTune->var.NotchChkTimer) > 3000 )
			{
				OnePrmTune->var.NotchChk = FALSE;
				OnePrmTune->var.ANotchSetStat = OPTUNE_STS_NOTCH_INIT;
				OnePrmTune->var.AResSetStat = OPTUNE_STS_ARES_INIT;
			}
		}

		if( OnePrmTune->var.ANotchSetStat > 0x02 )
		{ /* エラー状態 */
			if( KlibGetLongTimerMs( OnePrmTune->var.NotchChkTimer ) > 2000 )
			{
				/* ERR表示のクリア */
				OnePrmTune->var.ANotchSetStat = OPTUNE_STS_NOTCH_INIT;
			}
		}

		if( OnePrmTune->var.AResSetStat >= 0x10 )
		{ /* エラー状態 */
			if( KlibGetLongTimerMs( OnePrmTune->var.NotchChkTimer ) > 2000 )
			{
				/* ERR表示のクリア */
				OnePrmTune->var.AResSetStat = OPTUNE_STS_ARES_INIT;
			}
		}

		/* 周波数解析スタート*/
		if( onePrmANotchCheck( OnePrmTune, Axis->DetVib, Axis->SeqCtrlOut->BaseEnableReq ) == OK )
		{
			OnePrmTune->var.FftReq = TRUE;
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_DETECT;
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_DETECT;
			FunExe->State = OPTUNE_SEQ_VIBSEL;
		}

		/*----------------------------------------------------------------------------------------------*/
		/*	外乱オブザーバ設定について																	*/
		/*		モード2は，無条件に外乱外乱オブザーバ有効とする(設定はワンパラバランス)。				*/
		/*		その他のモードは，機能スイッチONでワンパラバランスの外乱オブザーバ有効とする。			*/
		/*----------------------------------------------------------------------------------------------*/
		/*	FFレベル変更ステータス作成処理＆タイムアウト処理											*/
		/*		タイムアウトでは，FFの設定値を破棄し，現在実行中の設定に強制的に戻す。					*/
		/*		内部処理としては，COINがONするまでFF設定値(表示値)のパラメータ計算等は，				*/
		/*		何も行っていないので表示上のFFレベルのみ元に戻せば良い									*/
		/*----------------------------------------------------------------------------------------------*/
		if( OnePrmTune->OpeReg.SpatFFLevel != OnePrmTune->var.FfSetting )
		{ /* 表示と動作のレベルが不一致 */
			switch( OnePrmTune->OpeReg.SpatMode )
			{ /* チューニングレベル */
			case 2: /* 位置決め専用 */
				//TuneMfcActive( Prm );
				TuneMfcActive( Prm, AccessPrm.hRegCfg );/*<S09E>*/
				break;
			case 3: /* 位置決め専用(オーバーシュート重視) */
				//TuneMfcActiveStMode( Prm );
				TuneMfcActiveStMode( Prm, AccessPrm.hRegCfg );/*<S09E>*/
				break;
			default:
				break;
			}

			/* チューニングデータ設定 */
			onePrmSetTuningGain( OnePrmTune,
								 Prm,
								 &AccessPrm,
								 pBaseCtrl->CtrlCmdPrm.IPSpdControl,
								 pBaseCtrl->DobsCtrl.var.DobsAct );
			onePrmSetTuningNotch( OnePrmTune,
								  Axis->ANotch,
								  &AccessPrm,
								  Prm );
			/* FF変更完了待ちステータスは，"実行"時のみ */
			OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFWAIT;
		}

		if( MfcGetParamEnableSts( &(pBaseCtrl->MFControl) ) == TRUE )
		{
			/* FFレベル表示を動作用に反映 */
			OnePrmTune->OpeReg.SpatFFLevel = OnePrmTune->var.FfSetting;
			if( OnePrmTune->OpeReg.SpatLvlChgState == OPTUNE_STS_FFWAIT )
			{
				/* FF変更完了待ちステータスは，"実行"時のみ */
				OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFCOMP;
			}
		}
		else
		{
			if( KlibGetLongTimerMs( OnePrmTune->var.FfWaitTimer ) > 8000 )
			{ /* FFレベル変更後，8sec経過 */
				/* FF変更失敗(タイムアウト) */
				OnePrmTune->OpeReg.SpatLvlChgState = OPTUNE_STS_FFTMOUT;
				/* FFレベルを元に戻す */
				OnePrmTune->var.FfSetting = OnePrmTune->OpeReg.SpatFFLevel;
				/* チューニングデータ設定 */
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
		case FCMD_STOP:		/* 停止 */
			ulwork = (ULONG)Prm->tfuncsw;

			switch( OnePrmTune->var.ANotchPreStat )
			{ /* ノッチ完了ステータス */
			case OPTUNE_PSTS_NT1_COMP:		/* 前回1段目ノッチON */
				ulwork &= 0x0000FFF0;									/* 第1ノッチOFF						*/
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;	/* 振動未検出ステータス設定			*/
				OnePrmTune->var.ANotch1Stat = OPTUNE_STS_NT1_NODETECT;	/* 1段目ステータスを未実施に初期化	*/
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;	/* 直前ノッチを未実施に初期化		*/
				break;

			case OPTUNE_PSTS_NT2_COMP:		/* 前回2段目ノッチON	*/
				ulwork &= 0x0000F0FF;									/* 第2ノッチOFF						*/
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;	/* 振動未検出ステータス設定			*/
				OnePrmTune->var.ANotch2Stat = OPTUNE_STS_NT2_NODETECT;	/* 2段目ステータスを未実施に初期化	*/
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;	/* 直前ノッチを未実施に初期化		*/
				break;

			default:
				break;
			}

			/* Pn408 トルク関係機能スイッチ */
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, ulwork, 2 );

			/* チューニングデータ設定	*/
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

		case FCMD_EXEC:		/* ゲイン変更 */
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
			/*	自動ノッチ実行条件判別																	*/
			/*------------------------------------------------------------------------------------------*/
			if( (OnePrmTune->var.ANotchStat != OPTUNE_STS_NT_NOENABLE)
				|| (OnePrmTune->var.AResStat != OPTUNE_STS_ARES_NOENABLE) )
			{ /* 自動ノッチ実行可能 or 自動Ａ型実行可能 */
				/*--------------------------------------------------------------------------------------*/
				/*	周波数解析処理実行処理																*/
				/*--------------------------------------------------------------------------------------*/
				if( Axis->SeqCtrlOut->BaseEnableReq == TRUE )
				{ /* サーボオン中のみ実行 */
					OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_DETECT;	/* 振動検出中ステータス設定		*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_DETECT;	/* 振動検出中ステータス設定		*/
					OnePrmTune->var.FftReq = TRUE;						/* 周波数解析処理開始要求		*/
					FunExe->State = OPTUNE_SEQ_VIBSEL;					/* 振動抑制設定フェーズへ移行	*/
					rc = PRM_RSLT_CONTINUE;
				}
				else
				{	/* サーボオフ中の場合 */
					rc = PRM_RSLT_CONDITION_ERR;
				}
			}
			break;

		case FCMD_EWRT:		/* EEPROM書込み */
			if( onePrmUpdateGainInEeprom(RegCfg,
										 pBaseCtrl->DobsCtrl.var.DobsAct,
//										 pBaseCtrl->MFControl.conf.MFCModelSetting) != TRUE )
										 pBaseCtrl->MFControl.conf.MFCModelSetting) != PRM_RSLT_SUCCESS )/* <S085> */
			{ /* EEPROM書込み処理 */
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

	case OPTUNE_SEQ_VIBSEL:		/* 振動抑制設定 */
		if( OnePrmTune->var.FftStat == TUNE_FREQ_SEARCHEND )
		{
			/* FFT解析要求OFF */
			OnePrmTune->var.FftReq = FALSE;

			/* 振動周波数取得 */
			VibFreq = FftGetAnaRsltVibmRf( Axis->FftAna );

			if( (OnePrmTune->var.AResStat != OPTUNE_STS_ARES_NOENABLE)
				&& (VibFreq <= AVIB_FRQ_MAX)
				&& (OnePrmTune->OpeReg.SpatLevel < TUNE_ANOTCH_MINKV) )
			{ /* Ａ型制振設定可能 && 周波数がＡ型制振上限以下 && ノッチON最小Kv以下 */
				/* ノッチ設定最小Kv設定		*/
				KvGain = TUNE_ANOTCH_MINKV;
			}
			else
			{
				/* FBレベル設定値 */
				KvGain = OnePrmTune->OpeReg.SpatLevel;
			}

			/* 自動ノッチフィルタ設定情報セット */
			TuneSetNotchFilSettingData( Axis->ANotch, VibFreq, KvGain, Prm->jrate );
			/* 第1ピーク周波数	*/
			swk = TuneNotchFilterSetting( Axis->ANotch, Prm, RegCfg );

			/* ノッチ設定ステータスセット */
			OnePrmTune->var.ANotchSetStat = (UCHAR)swk;
			/* チューニングデータ設定	*/
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
			case OPTUNE_STS_NOTCH1_OK:		/* 1段目ノッチ設定正常完了	*/
				OnePrmTune->var.ANotch1Stat = OPTUNE_STS_NT1_COMP;
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NT1_COMP;
				break;

			case OPTUNE_STS_NOTCH2_OK:		/* 2段目ノッチ設定正常完了	*/
				OnePrmTune->var.ANotch2Stat = OPTUNE_STS_NT2_COMP;
				OnePrmTune->var.ANotchPreStat = OPTUNE_PSTS_NT2_COMP;
				break;

			case OPTUNE_STS_NOTCHQ1_ERR:	/* 1段目ノッチQ値異常		*/
				OnePrmTune->var.ANotch1Stat = OPTUNE_STS_NT1_NG;
				break;

			case OPTUNE_STS_NOTCHQ2_ERR:	/* 2段目ノッチQ値異常		*/
				OnePrmTune->var.ANotch2Stat = OPTUNE_STS_NT2_NG;
				break;

			default:
				break;
			}

			/*------------------------------------------------------------------------------------------*/
			/*	自動ノッチ実行条件判別																	*/
			/*------------------------------------------------------------------------------------------*/
			if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F0F) == 0x0000 )
			{ /* 自動設定可能なノッチがない */
				/* 自動設定不可ステータス設定 */
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
			}
			else
			{
				/* 振動未検出ステータス設定 */
				OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_NODETECT;
			}

			if( (OnePrmTune->var.ANotchSetStat == OPTUNE_STS_NOTCH1_OK)
				|| (OnePrmTune->var.ANotchSetStat == OPTUNE_STS_NOTCH2_OK) )
			{
				/* ノッチチェック状態へ遷移 */
				OnePrmTune->var.NotchChk = TRUE;
				OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
				FunExe->State = OPTUNE_SEQ_TUNING;
			}
			else
			{ /* ノッチ設定失敗 */
				/*--------------------------------------------------------------------------------------*/
				/*	自動Ａ型制振																		*/
				/*--------------------------------------------------------------------------------------*/
				OnePrmTune->var.AResSetStat = (UCHAR)TuneATypeResSet( VibFreq,
																	  ((Prm->avibsw >> 4) & 0x0F),
																	  Prm,
																	  RegCfg );/*<S09E>*/

				/* レジスタ応答作成		*/
				switch( OnePrmTune->var.AResSetStat & 0xFF )
				{
				case OPTUNE_STS_ARES_OK:	/* Ａ型制振設定正常完了	*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
					OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_COMP;
					/* ノッチチェック状態へ */
					OnePrmTune->var.NotchChk = TRUE;
					FunExe->State = OPTUNE_SEQ_TUNING;
					break;
				case 0x10:					/* Ａ型制振上下限異常	*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
					OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NG;
					break;
				case 0x11:					/* Ａ型制振自動設定不可	*/
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOENABLE;
					break;
				default:
					OnePrmTune->var.AResStat = OPTUNE_STS_ARES_NOVIBDET;
					OnePrmTune->var.ARes1Stat = OPTUNE_STS_ARES_NG;
					break;
				}
				if( (OnePrmTune->var.AResSetStat & 0xFF) != OPTUNE_STS_ARES_OK )
				{	/* ノッチ/Ａ型ともにNG */
					OnePrmTune->var.FftWait = TRUE;
					KlibRstLongTimer( &OnePrmTune->var.FftWaitTimer );
					FunExe->State = OPTUNE_SEQ_NOTCHERR;
				}
			}
			KlibRstLongTimer( &OnePrmTune->var.NotchChkTimer );
		}
		else if( OnePrmTune->var.FftStat == TUNE_FREQ_SEARCHERR )
		{ /* エラー処理 */
			OnePrmTune->var.FftReq = FALSE;
			OnePrmTune->var.ANotchStat = OPTUNE_STS_NT_DET_ERR;		/* 検出エラーステータス設定			*/
			OnePrmTune->var.AResStat = OPTUNE_STS_ARES_DET_ERR;		/* 検出エラーステータス設定			*/
			FunExe->State = OPTUNE_SEQ_TUNING;
		}
		else
		{
			rc = PRM_RSLT_CONTINUE;
		}
		break;

	case OPTUNE_SEQ_NOTCHERR: /* ノッチフィルタ/Ａ型が入らなかったとき */
		FunExe->State = OPTUNE_SEQ_TUNING;
		break;

	default:
		break;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	低周波振動検出																				*/
	/*	  検出の対象は，外乱オブザーバ有効時のイナーシャ誤設定による位置ループの発振。				*/
	/*	  そのため検出レベルは高く設定しておく														*/
	/*	  振動検出時は，振動アラーム(A.520)を出力する												*/
	/*	　ここで処理しないと初期化処理で振動レベルがクリアされる前に振動検出してしまう				*/
	/*----------------------------------------------------------------------------------------------*/
	if( (pBaseCtrl->DobsCtrl.var.DobsAct == TRUE)
		&& (DetVibGetSpdErrPeakFn(Axis->DetVib) > (2 * DetVibGetVibCheckLevel(Axis->DetVib))) )
	{ /* 外乱オブザーバ有効 && 発振レベル×2を超えた場合 */
		ALMSetServoAlarm( Axis->AlmManager, ALM_AT );
		DetVibResetMaxMinSpeedError( Axis->DetVib );
		DetVibResetMaxMinSpeedErrorFn( Axis->DetVib );
	}

	return(rc);
}



/*****************************************************************************************************
	チューニングレベル再設定処理
*****************************************************************************************************/
void	OnePrmReSetTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT loophz, USHORT posgn, BOOL IPSpdControl )
{
	ULONG	Kv, Kp, Kp2Kv;
	ULONG	da, db;

	Kv = (ULONG)loophz;
	Kp = (ULONG)posgn;

	switch( OnePrmTune->OpeReg.SpatMode )
	{
		case 0:	/* 安定性重視 */
			/********************************/
			/* 減衰係数の計算				*/
			/*			 1   Kv				*/
			/*	   ζa = - √--  * 100		*/
			/*			 2   Kp				*/
			/********************************/
			da = MlibSqrtu32(Kv * 2500) / MlibSqrtu32(Kp);
			if( IPSpdControl == TRUE )
			{ /* I-P制御時 */
				db = 133;
				Kp2Kv = Kp * 228 / 100;
			}
			else
			{ /* PI制御時 */
				db = 125;
				Kp2Kv = Kp;
			}

			if( da >= db )
			{ /* ζa ≧ ζb */
				OnePrmTune->OpeReg.SpatLevel = (USHORT)Kp2Kv;
			}
			else
			{ /* ζa < ζb */
				OnePrmTune->OpeReg.SpatLevel = (USHORT)Kv;
			}
			break;

		case 1:	/* 応答性重視 */
		case 2:	/* 位置決め専用 */
		case 3:	/* 位置決め専用(OV) */
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
	/* RAMの値をEEPROMの値と同じにする */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_loophz,   0 );	/* Pn100 速度ループゲイン 					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_pitime,   0 );	/* Pn101 速度ループ積分時定数				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_posgn,	   0 );	/* Pn102 位置ループゲイン					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_jrate,    0 );	/* Pn103 慣性モーメント比					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_trqfil11, 0 );	/* Pn401 1段目第1トルク指令フィルタ時定数 	*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw,  0 );	/* Pn408 トルク関係機能スイッチ 			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );	/* Pn409 1段目ノッチフィルタ周波数 			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1,   0 );	/* Pn40A 1段目ノッチフィルタQ値				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );	/* Pn40C 2段目ノッチフィルタ周波数			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2,   0 );	/* Pn40D 1段目ノッチフィルタQ値				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibsw,   0 );	/* Pn160 制振制御関連スイッチ				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibfrq,  0 );	/* Pn161 A型制振周波数						*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_avibdamp, 0 );	/* Pn163 A型制振ダンピングゲイン			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobgn,    0 );	/* Pn121 摩擦補償ゲイン						*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dtrqgn,   0 );	/* Pn123 摩擦補償係数						*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dlpfil,   0 );	/* Pn124 摩擦補償周波数補正					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_dobjgn,   0 );	/* Pn125 摩擦補償ゲイン補正					*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlsw,    0 );	/* Pn140 モデル追従制御関連スイッチ			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlgn,    0 );	/* Pn141 モデル追従制御ゲイン				*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlzeta,  0 );	/* Pn142 モデル追従制御ゲイン補正			*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlptff,  0 );	/* Pn143 モデル追従制御バイアス(正転方向)	*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlntff,  0 );	/* Pn144 モデル追従制御バイアス(逆転方向)	*/
	PrmReLoadValueFromEeprom( RegCfg, &pndef_mdlvff,   0 );	/* Pn147 モデル追従制御速度フィードフォワード補償 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_vibsel,   0 );	/* Pn310 振動検出スイッチ					*/

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
	{ /* (自動ノッチ実行可能 || 自動Ａ型実行可能)
		  && !自動ノッチ実行待ち && !ノッチ確認中 && 振動が検出レベル以上 && サーボオン中 */
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

	/* ローカル変数の初期化 */
	rc = OK;

	/*------------------------------------------------------------------------------*/
	/*	計算パラメータの設定														*/
	/*------------------------------------------------------------------------------*/
	switch( OnePrmTune->OpeReg.SpatMode )
	{
	case 1:
		if( IPSpdControl == TRUE )
		{ /* I-P制御時 */
			TuneCalV.PIRate = 1;
			TuneCalV.KpZeta = 100;
			TuneCalV.Ti = 200 * 70 * 70;
		}
		else
		{ /* PI制御時 */
			TuneCalV.KpZeta = 70;
		}
		break;
	case 0:
	case 2:
	case 3:
	default:
		if( IPSpdControl )
		{ /* I-P制御時 */
			TuneCalV.PIRate = 1;
			TuneCalV.KpZeta = 228;
			TuneCalV.Ti = 200 * 100 * 100;
		}
		else
		{ /* PI制御時 */
			TuneCalV.KpZeta = 100;
		}
		break;
	}

	/*------------------------------------------------------------------------------*/
	/*	Pn101, Pn102, Pn121の計算													*/
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

		/* 外乱オブザーバ有効 */
		DobsAct = TRUE;/*<S09E>*/
	}
	else
	{
		if( IPSpdControl == TRUE )
		{ /* I-P制御時 */
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
		{ /* PI制御時 */
			/* Pn102 */
			kx = MlibScalKxgain( OnePrmTune->OpeReg.SpatLevel, 100, TuneCalV.KpZeta, NULL, 24 );
			TuneCalV.Gain.posgn = (USHORT)MlibMulgain( 1, kx );

			/* Pn101 */
			kx = MlibPcalKxgain( 800000, 1, OnePrmTune->OpeReg.SpatLevel, NULL, 24 );
			TuneCalV.Gain.pitime = (USHORT)MlibMulgain( 1, kx );
		}

		/*--------------------------------------------------------------------------*/
		/*	ゲイン上下限チェック													*/
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
	/*	RAMへの書込み																*/
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

	/* ローカル変数の初期化 */
	rc = OK;

	/*------------------------------------------------------------------------------*/
	/*	計算パラメータの設定														*/
	/*------------------------------------------------------------------------------*/
	/* ノッチＱ値 & トルク指令フィルタ設定	*/
	TuneNotchQ( ANotch,
				AccessPrm->hRegCfg,
				OnePrmTune->OpeReg.SpatLevel,
				OnePrmTune->var.Jrate,
				0,
				Prm );
	/* 設定結果取得 */
	TuneGetNotchQSetting( ANotch,
						  &TuneCalV.Gain.ntchq1,
						  &TuneCalV.Gain.ntchq2,
						  &TuneCalV.Gain.trqfil11 );

	/*------------------------------------------------------------------------------*/
	/*	上下限チェック																*/
	/*------------------------------------------------------------------------------*/
	/* Pn401 */
	TuneCalV.Gain.trqfil11 = TunePrmLimitUnsigned( TuneCalV.Gain.trqfil11, &pndef_trqfil11 );

	/*------------------------------------------------------------------------------*/
	/*	RAMへの書込み																*/
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

	/* Pn100 速度ループゲイン */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_loophz, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn101 速度ループ積分時定数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_pitime, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn102 位置ループゲイン */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_posgn, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn103 慣性モーメント比 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_jrate, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn140 モデル追従制御関連スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn401 1段目第1トルク指令フィルタ時定数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn408 トルク関係機能スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn409 １段目ノッチフィルタ周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40A 1段目ノッチフィルタQ値 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40C 2段目ノッチフィルタ周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn40D 2段目ノッチフィルタQ値 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	if( DobsAct )
	{
		/* Pn121 摩擦補償ゲイン */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn123 摩擦補償係数 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dtrqgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn124 摩擦補償周波数補正 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dlpfil, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn125 摩擦補償ゲイン補正 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_dobjgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}
	}

	if( MfcModel != 0 )
	{
		/* Pn141 モデル追従制御ゲイン */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlgn, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

//		rc = RpiFunPrmNvmemUpdate( &pndef_mdlzeta );	/* Pn142	*/
		/* Pn142 モデル追従制御ゲイン補正 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlzeta, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn143 モデル追従制御バイアス(正転方向) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlptff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn144 モデル追従制御バイアス(逆転方向) */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlntff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}

		/* Pn147 モデル追従制御速度フィードフォワード補償 */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_mdlvff, 0 );
		if( rc != PRM_RSLT_SUCCESS )
		{
			return( rc );
		}
	}

//	rc = RpiFunPrmNvmemUpdate( &pndef_avibfrq );		/* Pn 161	*/
	/* Pn161 Ａ型制振周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibfrq, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn163 Ａ型制振ダンピングゲイン */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibdamp, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}

	/* Pn160 制振制御関連スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_avibsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return( rc );
	}


	return(rc);
}



/************************************************************************************************/
/*		チューニングモード設定確認処理															*/
/************************************************************************************************/
static void	onePrmCheckTuningMode( ONEPRMTUNE *OnePrmTune )
{
	/*------------------------------------------------------------------------------------------*/
	/*	チューニングモード条件判定																*/
	/*------------------------------------------------------------------------------------------*/
	if( (OnePrmTune->OpeReg.SpatMode < OnePrmTune->var.ModeMin)
		|| (OnePrmTune->OpeReg.SpatMode > OnePrmTune->var.ModeMax) )
	{	/* チューニングモード設定が機能条件と合わない */
		OnePrmTune->var.ModeUnmatch = TRUE;
	}
	else
	{
		OnePrmTune->var.ModeUnmatch = FALSE;
	}
	return;
}



/************************************************************************************************/
/*		チューニングモード設定処理 (レジスタIF)													*/
/************************************************************************************************/
PRM_RSLT	RpiFunSetOnePrmTuningMode( ONEPRMTUNE *OnePrmTune, USHORT Mode )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/*------------------------------------------------------------------------------------------*/
	/*	チューニングモード上下限チェック														*/
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
/*		チューニングレベル設定処理 (レジスタIF)													*/
/************************************************************************************************/
PRM_RSLT	RpiFunSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl )
{
	PRM_RSLT	rc;
	USHORT		LvlLimit;
	rc = PRM_RSLT_SUCCESS;

	/* 上下限値確認 */
	LvlLimit = TunePrmLimitUnsigned( Lvl, &pndef_loophz );

	if( LvlLimit == Lvl )
	{ /* Lvlが上下限値内の場合 */
		OnePrmTune->OpeReg.SpatLevel = Lvl;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}



/************************************************************************************************/
/*		チューニングレベル設定処理 (レジスタIF)													*/
/************************************************************************************************/
PRM_RSLT	RpiFunSetOnePrmTuningFfLevel( ONEPRMTUNE *OnePrmTune, USHORT Lvl )
{
	PRM_RSLT	rc;
	USHORT		LvlLimit;
	rc = PRM_RSLT_SUCCESS;

	/* 上下限値確認 */
	LvlLimit = TunePrmLimitUnsigned( Lvl, &pndef_mdlgn );

	if( LvlLimit == Lvl )
	{ /* Lvlが上下限値内の場合 */
		OnePrmTune->var.FfSetting = Lvl;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	return( rc );
}



/************************************************************************************************/
/*		チューニングレベル(Pn100の値)処理 														*/
/************************************************************************************************/
void	RpiRegSetOnePrmTuningLevel( ONEPRMTUNE *OnePrmTune, USHORT Gain )
{
	OnePrmTune->OpeReg.SpatLevel = Gain;
	return;
}



/************************************************************************************************/
/*		チューニングレベル(Pn141の値)処理														*/
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
