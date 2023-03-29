/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MotorPoleFind.c : 磁極検出制御用	モジュール定義											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2011.01.25	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "MotorPoleFind.h"
#include "MLib.h"



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	LONG	MpoleFindSpeedControl( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL BeComplete );		*/
/*	ALMID_T	MpoleFindSequence( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq );				*/
/*	ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq );			*/
/*	ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, BASE_CTRL_HNDL *pBaseCtrl, KSGAIN Kmovpos );	*/
/****************************************************************************************************/
static ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo );
static ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, LONG dMotPos, KSGAIN Kmovpos );
static	void	MpoleFindDrive( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo );
static	void	CheckMpFindFail( MPFIND *MpFind, LONG SpdFbki );
static	void	MpFindLauSpdRef( MPFIND *MpFind, LONG SpdFbki );
static	void	MpFindCalPosAbout( MPFIND *MpFind );
static	void	MpFindCalPos( LONG *DegCalc, LONG *Counter, LONG *Degree, BOOL *NoGood_flag );
static	void	MpFindGetMaxTrqSpd( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo );
static	void	JudgeMpFindErrSign( MPFIND *MpFind );
static	void	UpdatePhaseCalculation( MPFIND *MpFind );
static	void	StopMpoleFind( MPFIND *MpFind, BOOL MotStopSts );
static	BOOL	ConfirmMpoleFind( MPFIND *MpFind, KSGAIN Kphasepos, LONG dMotPos,
																			LONG SpdFbki );
static	LONG	MpFindLauTrqRef( MPFIND *MpFind, BOOL *WaitFinish );
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		磁極検出用速度制御演算																		*/
/*																									*/
/****************************************************************************************************/
LONG	MpoleFindSpeedControl( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL BeComplete )
{
	LONG			PacOut;
	LONG			IacOut;
	LONG			TrqRefx;
	LONG			work_integless;
	LONG			work;

/*--------------------------------------------------------------------------------------------------*/
/*		位相補正値確認用トルク制御要求時処理														*/
/*--------------------------------------------------------------------------------------------------*/
	/* Check Control Mode Change Flag */
	if( MpFind->var.PhaseConfFlg == TRUE )
	{
		/* Direct Torque Reference Input */
		MpFind->var.Ivar64[1] = MpFind->var.Ivar64[0] = 0;
		return( MpFind->var.TrqRefPhaConf );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		速度FBフィルタ処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	MpFind->var.SpdFbFilo = MlibLpfilter1( MpfExeInfo->SpdFbMonitor,
										   MpfExeInfo->KVfbFil,
										   MpFind->var.SpdFbFilo );

/*--------------------------------------------------------------------------------------------------*/
/*		速度偏差演算																				*/
/*--------------------------------------------------------------------------------------------------*/
	MpFind->var.SpdErr = MpFind->MpFindSpdRef - MpFind->var.SpdFbFilo;

/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロック時の処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (BeComplete == FALSE) || (MpFind->var.ZeroTorque == TRUE) )
	{
		MpFind->TrqFilOut = 0;
		MpFind->var.Ivar64[1] = MpFind->var.Ivar64[0] = 0;
//		pBaseCtrl->CtrlCmdMngr.SpdRefo = 0;
		return( 0 );
	}

/*--------------------------------------------------------------------------------------------------*/
/*		速度制御処理																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* 比例項計算 */
	PacOut = MpFind->var.SpdErr;

	/* 積分項計算 */
	if( MpFind->var.ClrIntegral )
	{
		IacOut = 0;
		work_integless = 0;
		MpFind->var.Ivar64[0] = 0;
		MpFind->var.Ivar64[1] = 0;
		MpFind->var.ClrIntegral = FALSE;
	}
	else
	{
		if( !MpFind->var.InteglessON )
		{
			/* 通常時 */
			work_integless = 0;
		}
		else
		{
			/* 積分レス時 */
#if (FLOAT_USE==TRUE)
			work_integless = (LONG)( (float)(MpFind->var.Ivar64[1] >> 1) * MpFind->conf.InteglessGain );
#else
			work_integless = MlibMulgainNolim( (MpFind->var.Ivar64[1] >> 1), MpFind->conf.InteglessGain );
#endif /* FLOAT_USE */
		}
	}
	work = MpFind->var.SpdErr - work_integless;
	IacOut =
		MlibIntegral( work, MpFind->conf.Ki, MpFind->var.Ivar64 );

	work = PacOut + IacOut;

	/* ゲインの自動調整 */
	switch( MpFind->var.KvGain )
	{
		case 0 :		/* ゲイン1倍 */
			break;

		case 1 :		/* ゲイン1.5倍 */
			work = work + (work >> 1);
			break;

		case 2 :		/* ゲイン2倍 */
			work = (work << 1);
			break;

		case -1 :		/* ゲイン0.75倍 */
			work = (work >> 1) + (work >> 2);
			break;

		case -2 :		/* ゲイン0.5倍 */
			work = (work >> 1);
			break;

		default :
			break;
	}
#if (FLOAT_USE==TRUE)
	TrqRefx = ( (float)work * MpFind->conf.Kv );
#else
	TrqRefx = MlibMulgain( work, MpFind->conf.Kv );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		１次ローパスフィルタ＆トルク制限処理														*/
/*--------------------------------------------------------------------------------------------------*/
	MpFind->TrqFilOut = MlibLpfilter1( TrqRefx, MpFind->conf.Klpf, MpFind->TrqFilOut );
	return( MlibLimitul(MpFind->TrqFilOut, 0x01000000, -0x01000000) );

}



/****************************************************************************************************/
/*																									*/
/*		磁極誤検出アラーム検出																		*/
/*																									*/
/****************************************************************************************************/
static ALMID_T	MpoleFindDetectAlarm( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo )
{
	ALMID_T		AlarmCode;

	if(MpfExeInfo->HaltCondition != FALSE)
	{
		MpFind->MPJudgeMonitor[0] = 0;	/* 磁極検出判定モニタリセット		*/
		MpFind->MPJudgeMonitor[1] = 0;	/* 磁極検出判定モニタリセット		*/
		MpFind->MPJudgeMonitor[2] = 0;	/* 磁極検出判定モニタリセット		*/
		AlarmCode = ALM_PFINDSTOP;	/* A.C51 : 磁極検出停止 */
	}
	else if(MpFind->var.FailTimes >= TRYCOUNT)
	{
//		MpFind->var.Step = MPF_INIT;
		AlarmCode = ALM_POLE;		/* A.C50 : 磁極誤検出 */
	}
	else
	{
		AlarmCode = MpoleFindCheckMovingRange(MpFind,
		                                      MpfExeInfo->dMotPosition,
		                                      MpfExeInfo->Kmovpos);
	}

	return	AlarmCode;
}



/****************************************************************************************************/
/*																									*/
/*		磁極検出可動範囲確認処理																	*/
/*																									*/
/****************************************************************************************************/
static ALMID_T	MpoleFindCheckMovingRange( MPFIND *MpFind, LONG dMotPos, KSGAIN Kmovpos )
{
	LONG		lwk;
	ALMID_T		AlarmCode;
	AlarmCode = NO_ALARM;

	MpFind->UnMpFindingTime++;

	MpFind->var.MovPos += dMotPos;

	if( MpFind->var.MovPos > MpFind->UnMpfMovePosP )
	{
		MpFind->UnMpfMovePosP = MpFind->var.MovPos;
	}
	else if( MpFind->var.MovPos < MpFind->UnMpfMovePosN )
	{
		MpFind->UnMpfMovePosN = MpFind->var.MovPos;
	}

	if( MpFind->UnMpfMovePosP >= -MpFind->UnMpfMovePosN )
	{
		lwk = MpFind->UnMpfMovePosP;
	}
	else
	{
		lwk = -MpFind->UnMpfMovePosN;
	}

	if( MpFind->conf.DetectRange <  MlibMulgain( lwk, Kmovpos ) )
	{
		/* A.C53 : 磁極検出可動範囲オーバー */
		AlarmCode = ALM_PDET_MOVEOVER;
	}

	return	AlarmCode;
}



/****************************************************************************************************/
/*																									*/
/*		磁極検出処理シーケンス																		*/
/*																									*/
/****************************************************************************************************/
ALMID_T	MpoleFindSequence( MPFIND *MpFind, MPFEXEINFO *MpfExeInfo, BOOL PdetReq, BOOL BaseEnable )
{
	ALMID_T		AlarmCode;
	AlarmCode = NO_ALARM;

	if( PdetReq == FALSE )
	{
		MpFind->var.Step = MPF_INIT;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		磁極検出処理シーケンス																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MpFind->var.Step )
	{
	/*----------------------------------------------------------------------------------------------*/
	case MPF_INIT:			/* Initial Step															*/
	/*----------------------------------------------------------------------------------------------*/
		if( (PdetReq & BaseEnable) == TRUE )
		{ /* 磁極検出シーケンス開始要求 */

			/* 磁極検出シーケンス処理用変数初期化 */
			MlibResetLongMemory( &MpFind->var, sizeof( MpFind->var ) >> 2 );
			/* モニタ用変数初期化 */
			MpFind->UnMpFindingTime = 0;
			MpFind->UnMpfMovePosP = 0;
			MpFind->UnMpfMovePosN = 0;

			MpFind->var.Step = MPF_INIT2;
		}
		break;
	/*----------------------------------------------------------------------------------------------*/
		case MPF_INIT2:		/* Initial Step2														*/
	/*----------------------------------------------------------------------------------------------*/
			MpFind->var.Ivar64[0] = 0;
			MpFind->var.Ivar64[1] = 0;
			MpFind->var.OSFail = FALSE;
			MpFind->var.dAxisMaxTrqPlus = 0;
			MpFind->var.dAxisMaxTrqMinus = 0;
			MpFind->var.qAxisMaxTrqPlus = 0;
			MpFind->var.qAxisMaxTrqMinus = 0;

			MpFind->var.dAxisMaxSpdPlus = 0;
			MpFind->var.dAxisMaxSpdMinus = 0;
			MpFind->var.qAxisMaxSpdPlus = 0;
			MpFind->var.qAxisMaxSpdMinus = 0;

			MpFind->var.DegRev = 0;
			MpFind->var.DegFinal = 0;
			MpFind->var.AreaCheckStep = 0;
			MpFind->var.ReverseCtr = 0;
			MpFind->var.NoGood = 0;

			/* オフセット速度(速度指令開始時の速度) */
			MpFind->var.SpdOffset = MpfExeInfo->SpdFbMonitor;

			MpFind->var.Step = MPF_DRIVE;
			break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_DRIVE:			/* 運転処理																*/
	/*----------------------------------------------------------------------------------------------*/
		MpoleFindDrive( MpFind, MpfExeInfo->SpdFbMonitor, MpfExeInfo->TrefMonitor );
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_CAL:			/* 位相計算																*/
	/*----------------------------------------------------------------------------------------------*/
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		UpdatePhaseCalculation( MpFind );
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_STOP:			/* 停止確認																*/
	/*----------------------------------------------------------------------------------------------*/
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		StopMpoleFind( MpFind, MpfExeInfo->MotStopSts );
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_CONFIRM:		/* 動作確認																*/
	/*----------------------------------------------------------------------------------------------*/
		AlarmCode = MpoleFindDetectAlarm(MpFind, MpfExeInfo);
		MpFind->var.PhaseConfFlg = TRUE;		/* 位相補正値確認のためトルク制御にするフラグ		*/
		if( FALSE != ConfirmMpoleFind( MpFind,
									   MpfExeInfo->Kphasepos,
									   MpfExeInfo->dMotPosition,
									   MpfExeInfo->SpdFbMonitor ) )
		{
			/* A.C54 : 磁極検出確認不可 */
			AlarmCode = ALM_PDET_NG;
		}
		break;
	/*----------------------------------------------------------------------------------------------*/
	case MPF_END:			/* 終了処理																*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.PhaseConfFlg = FALSE;
		MpFind->var.Ivar64[0] = 0;
		MpFind->var.Ivar64[1] = 0;
//		MpFind->var.Step = MPF_INIT;
		break;
	/*----------------------------------------------------------------------------------------------*/
	default:				/* 本ケースはシーケンスソフト異常										*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.Step = MPF_INIT;

		/* A.C50 : 磁極誤検出 */
		AlarmCode = ALM_POLE;
		break;
	}

	return	AlarmCode;
}



/****************************************************************************************************/
/*																									*/
/*		磁極検出運転処理																			*/
/*																									*/
/****************************************************************************************************/
static	void	MpoleFindDrive( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo )
{
/*--------------------------------------------------------------------------------------------------*/
/*		運転シーケンス状態確認																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( !MpFind->var.Fail )
	{
		MpFind->var.PastTime++;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		逆走＆速度オーバーチェック処理																*/
/*--------------------------------------------------------------------------------------------------*/
	CheckMpFindFail( MpFind, SpdFbki );

/*--------------------------------------------------------------------------------------------------*/
/*		速度指令作成処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	MpFindLauSpdRef( MpFind, SpdFbki );

	if( !MpFind->var.RangeFix )
	{
/*--------------------------------------------------------------------------------------------------*/
/*		8分割される全領域チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
		switch( MpFind->var.RangeFixStep )
		{
		case PAREACHECK_000:
			if ( MpFind->var.PhaseChange )
			{
				/* 逆走判定時の処理 */
				MpFind->MpfPhaseOffset += PHASE_DEG180;
				MpFind->var.PhaseChange = FALSE;
				MpFind->var.RangeFixStep = PAREACHECK_001;
			}
			else if ( MpFind->var.PastTime == 0 )
			{
				if ( MpFind->var.AreaCheckStep == 7 )
				{
					/* 速度最大および逆走位置から大まかな位置を計算 */
					MpFindCalPosAbout( MpFind );

					/* やり直しチェック */
					switch ( MpFind->var.NoGood )
					{
					case 0:
						/* ステップ1終わり */
						MpFind->MpfPhaseOffset += MpFind->var.DegFinal + PHASE_DEG090;
						MpFind->var.RangeFix = TRUE;
						MpFind->var.Direction = FALSE;
						MpFind->var.AxisChange = FALSE;
						break;

					case 1:
						/* ゲインを上げる */
						MpFind->var.KvGain++;
						MpFind->MpfPhaseOffset = 0;
						MpFind->var.RangeFixStep = PAREACHECK_000;
						MpFind->var.FailTimes++;						/* 失敗回数を1増やす */
						MpFind->var.Step = MPF_INIT2;					/* 初期化処理からやり直す */
						break;
					}
				}
				else
				{
					MpFind->MpfPhaseOffset += PHASE_DEG045;
					MpFind->var.RangeFixStep = PAREACHECK_000;
					MpFind->var.AreaCheckStep++;
				}
			}
			break;

		case PAREACHECK_001:
			if ( MpFind->var.PastTime == 0 )
			{
				/* 逆走位置取得 */
				if ( MpFind->var.ReverseCtr <= 3 )
				{
					MpFind->var.DegReverse[MpFind->var.ReverseCtr] = PHASE_DEG045 * MpFind->var.AreaCheckStep;
				}
				MpFind->var.ReverseCtr++;

				if ( MpFind->var.AreaCheckStep == 7 )
				{
					/* 速度最大および逆走位置から大まかな位置を計算 */
					MpFindCalPosAbout( MpFind );

					/* やり直しチェック */
					switch ( MpFind->var.NoGood )
					{
					case 0:
						/* ステップ1終わり */
						MpFind->MpfPhaseOffset += MpFind->var.DegFinal - PHASE_DEG090;
						MpFind->var.RangeFix = TRUE;
						MpFind->var.Direction = FALSE;
						MpFind->var.AxisChange = FALSE;
						break;

					case 1:
						/* ゲインを上げる */
						MpFind->var.KvGain++;
						MpFind->MpfPhaseOffset = 0;
						MpFind->var.RangeFixStep = PAREACHECK_000;
						MpFind->var.FailTimes++;						/* 失敗回数を1増やす */
						MpFind->var.Step = MPF_INIT2;					/* 初期化処理からやり直す */
						break;
					}
				}
				else
				{
					MpFind->MpfPhaseOffset -= PHASE_DEG135;
					MpFind->var.RangeFixStep = PAREACHECK_000;
					MpFind->var.AreaCheckStep++;
				}
			}
			break;

		case PAREACHECK_002:
			if ( MpFind->var.PastTime == 0 )
			{
				MpFind->MpfPhaseOffset = 0;
				MpFind->var.KvGain--;									/* ゲインを下げる */
				MpFind->var.RangeFixStep = PAREACHECK_000;
				MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 5;		/* 判定モニタセット */
				MpFind->var.FailTimes++;								/* 失敗回数を1増やす */
				MpFind->var.Step = MPF_INIT2;							/* 初期化処理からやり直す */
			}
			break;

		case PAREACHECK_003:
			MpFind->MpfPhaseOffset = 0;
			MpFind->var.KvGain--;										/* ゲインを下げる */
			MpFind->var.RangeFixStep = PAREACHECK_000;
			MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 4;			/* 判定モニタセット */
			MpFind->var.FailTimes++;									/* 失敗回数を1増やす */
			MpFind->var.Step = MPF_INIT2;								/* 初期化処理からやり直す */
			break;

		default :
			MpFind->var.RangeFixStep = PAREACHECK_000;
			MpFind->MpfPhaseOffset = 0;
			break;
		}
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		ｄ－ｑ軸ずれを基に推定角度絞込み															*/
/*--------------------------------------------------------------------------------------------------*/
		if ( MpFind->var.PhaseChange )
		{
			MpFind->MpfPhaseOffset += PHASE_DEG180;
			MpFind->var.Times = 0;
			MpFind->var.RangeFixStep = PAREACHECK_002;
			MpFind->var.RangeFix = FALSE;
			MpFind->var.PhaseChange = FALSE;
		}
		else
		{
			MpFindGetMaxTrqSpd( MpFind, SpdFbki, TrqRefo );

			if ( MpFind->var.PastTime == 0 )
			{
				if ( MpFind->var.AxisChange )
				{
					MpFind->MpfPhaseOffset -= PHASE_DEG090;
				}
				else
				{
					MpFind->var.Times++;
					JudgeMpFindErrSign( MpFind );

					if ( MpFind->var.Step != MPF_CAL )
					{
						switch ( MpFind->var.AxisErrSign )
						{
							case AXISERR_EQUAL :
							case AXISERR_MINUS :
								MpFind->MpfPhaseOffset += PHASE_DEG090 + (PHASE_DEG030 >> MpFind->var.Times);
								break;

							case AXISERR_PLUS :
								MpFind->MpfPhaseOffset += PHASE_DEG090 - (PHASE_DEG030 >> MpFind->var.Times);
								break;

							default :
								break;
						}
					}
				}
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		エラー検出 : 逆走およびオーバースピード判定を行う。											*/
/*																									*/
/****************************************************************************************************/
static	void	CheckMpFindFail( MPFIND *MpFind, LONG SpdFbki )
{
	LONG	work;
	LONG	OverSpeed;

	/* 逆走判定レベルの設定 */
	if ( MpFind->var.ReverseLevel )
	{
		/* 待ち時間中 */
		work = MpFind->conf.ReverseSpd2;
	}
	else
	{
		/* 速度指令中 */
		work = MpFind->conf.ReverseSpd;
	}

	/* 暴走判定レベルの設定 */
	if ( MpFind->var.OSFail )
	{
		/* オーバースピードにかかった後のレベル */
		OverSpeed = MpFind->conf.MaxSpd;
	}
	else
	{
		/* 通常時のレベル */
		OverSpeed = MpFind->conf.OverSpd;
	}

	/* 常時オーバースピード判定 */
	if ( MlibLABS( SpdFbki ) >= OverSpeed )
	{
		MpFind->var.PastTime = MpFind->conf.WaitTime;
		MpFind->var.Fail = TRUE;
		MpFind->var.OSFail = TRUE;
		MpFind->var.ZeroTorque = TRUE;
	}

	/* 逆走処理中でない＆ステップ1の時、逆走検出を行う。 */
	if ( (!MpFind->var.Fail) && (!MpFind->var.RangeFix) )
	{
		if ( !MpFind->var.Direction )
		{
			/* 正方向速度指令時 */
			if ( (SpdFbki - MpFind->var.SpdOffset) < ( -work ) )
			{
				MpFind->var.Fail = TRUE;
				MpFind->var.PhaseChange = TRUE;
				MpFind->var.PastTime = MpFind->conf.WaitTime;
				MpFind->var.ClrIntegral = TRUE;
			}
		}
		else
		{
			/* 負方向速度指令時 */
			if ( (SpdFbki - MpFind->var.SpdOffset) > work )
			{
				MpFind->var.Fail = TRUE;
				MpFind->var.PhaseChange = TRUE;
				MpFind->var.PastTime = MpFind->conf.WaitTime;
				MpFind->var.ClrIntegral = TRUE;
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		速度指令作成 : 磁極検出時の運転指令を作成する。												*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindLauSpdRef( MPFIND *MpFind, LONG SpdFbki )
{
	LONG	AbsSpdRef;

/*--------------------------------------------------------------------------------------------------*/
/*		エラー検出時の処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( MpFind->var.Fail )
	{
		MpFind->MpFindSpdRef = 0;
		MpFind->var.InteglessON = FALSE;

		if( --MpFind->var.PastTime <= 0 )
		{
			MpFind->var.RefOutTime = 0;
			MpFind->var.Fail = FALSE;
			MpFind->var.RefOutStep = MPFLAU_ACC;
			MpFind->var.Direction = FALSE;
			MpFind->var.AxisChange = FALSE;
			MpFind->var.ZeroTorque = FALSE;
			MpFind->var.ReverseLevel = FALSE;
			MpFind->var.SpdOffset = SpdFbki;

			if ( MpFind->var.OSFail )
			{
				MpFind->var.RangeFixStep = PAREACHECK_003;
			}
		}
		return;
	}

	AbsSpdRef = MlibLABS( MpFind->MpFindSpdRef );

/*--------------------------------------------------------------------------------------------------*/
/*		速度指令作成シーケンス																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MpFind->var.RefOutStep )
	{
	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_INIT:		/* 待ち時間経過															*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.RefOutStep = MPFLAU_ACC;
		MpFind->var.RefOutTime = 0;
		MpFind->var.SpdOffset = SpdFbki;
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_ACC:		/* 加速指令作成															*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = MlibLaufilter( MpFind->conf.MaxSpd, AbsSpdRef, MpFind->conf.dSpd );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.AccTime )
		{
			MpFind->var.RefOutStep = MPFLAU_CONST;
			MpFind->var.RefOutTime = 0;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_CONST:		/* 定常速度指令作成														*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = MpFind->conf.MaxSpd;
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.ClmpTime )
		{
			MpFind->var.RefOutStep = MPFLAU_DEC;
			MpFind->var.RefOutTime = 0;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_DEC:		/* 減速指令作成															*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = MlibLaufilter( 0, AbsSpdRef, MpFind->conf.dSpd );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.AccTime )
		{
			MpFind->var.RefOutStep = MPFLAU_WAIT;
			MpFind->var.RefOutTime = 0;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFLAU_WAIT:		/* 待ち時間経過															*/
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = 0;
		MpFind->var.RefOutTime++;
		MpFind->var.ReverseLevel = TRUE;
		if ( MpFind->var.RefOutTime >= MpFind->conf.InteglessTime )
		{
			MpFind->var.InteglessON = TRUE;
		}

		if( MpFind->var.RefOutTime >= MpFind->conf.WaitTime )
		{
			if( MpFind->var.Direction )
			{
				MpFind->var.PastTime = 0;
				if( MpFind->var.AxisChange )
				{
					MpFind->var.AxisChange = FALSE;
				}
				else
				{
					MpFind->var.AxisChange = TRUE;
				}
			}
			if( MpFind->var.Direction )
			{
				MpFind->var.Direction = FALSE;
			}
			else
			{
				MpFind->var.Direction = TRUE;
			}
			MpFind->var.RefOutStep = MPFLAU_ACC;
			MpFind->var.RefOutTime = 0;
			MpFind->var.InteglessON = FALSE;
			MpFind->var.ReverseLevel = FALSE;
			MpFind->var.SpdOffset = SpdFbki;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	default:
	/*----------------------------------------------------------------------------------------------*/
		AbsSpdRef = 0;
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		速度指令出力																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( MpFind->var.Direction )
	{
		MpFind->MpFindSpdRef = -AbsSpdRef;
	}
	else
	{
		MpFind->MpFindSpdRef = AbsSpdRef;
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		大まかな位置計算関数：逆走位置から大まかな位置を計算										*/
/*			①逆走回数チェック																		*/
/*			②逆走位置より推定位置を計算															*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindCalPosAbout( MPFIND *MpFind )
{
	BOOL	NG_flag;

	/* 逆走回数チェック */
	if( MpFind->var.ReverseCtr >= 5 )
	{
		/* ゲインを上げて、もう一度やり直し */
		MpFind->var.NoGood = TRUE;
		MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 1;			/* 判定モニタセット */
		return;
	}

	/* 逆走回数チェック */
	if( MpFind->var.ReverseCtr <= 1 )
	{
		/* ゲインを上げて、もう一度やり直し */
		MpFind->var.NoGood = TRUE;
		MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 2;			/* 判定モニタセット */
		return;
	}

	/* 逆走位置を計算 */
	NG_flag = FALSE;
	MpFindCalPos( &MpFind->var.DegRev, &MpFind->var.ReverseCtr,
													&MpFind->var.DegReverse[0], &NG_flag );
	/* やり直し判定 */
	if( NG_flag )
	{
		/* ゲインを上げて、もう一度やり直し */
		MpFind->var.NoGood = TRUE;
		MpFind->MPJudgeMonitor[MpFind->var.FailTimes] = 3;			/* 判定モニタセット */
		return;
	}
	MpFind->var.DegRev += PHASE_DEG180;

	MpFind->var.DegFinal = MpFind->var.DegRev;

	return;
}



/****************************************************************************************************/
/*																									*/
/*		位置計算関数：位置を計算(MpFindCalPosAbout()関数で使用)										*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindCalPos( LONG *DegCalc, LONG *Counter, LONG *Degree, BOOL *NoGood_flag )
{
	ULONG	lwork1, lwork2;
	USHORT	work[3];
	USHORT	i;

	*NoGood_flag = FALSE;
	lwork1 = lwork2 = 0;

	if( *Counter == 1 )
	{
		lwork1 = *Degree;
	}
	else if( *Counter == 2 )
	{
		lwork1 = (ULONG)*Degree + (ULONG)*(Degree + 1);
		work[0] = (USHORT)( *(Degree + 1) - *Degree );

		if( work[0] > (USHORT)PHASE_DEG180 )
		{
			lwork1 = (lwork1 - (ULONG)PHASE_DEG360) >> 1;
		}
		else if( work[0] < (USHORT)PHASE_DEG180 )
		{
			lwork1 = lwork1 >> 1;
		}
		else
		{
			/* 逆走した位相が180degずれていたので、NoGood_flagをセット */
			*NoGood_flag = TRUE;
		}
	}
	else if( *Counter == 3 )
	{
		lwork1 = (ULONG)*Degree + (ULONG)*(Degree + 1) + (ULONG)*(Degree + 2);
		work[0] = (USHORT)( *(Degree + 2) - *Degree );
		work[1] = (USHORT)( *(Degree + 1) - *Degree );
		work[2] = (USHORT)( *(Degree + 2) - *(Degree + 1) );

		if( work[0] <= (USHORT)PHASE_DEG135 )
		{
			lwork1 = lwork1 / 3;
		}
		else if( work[1] >= (USHORT)PHASE_DEG225 )
		{
			lwork1 = (lwork1 + (ULONG)PHASE_DEG360) / 3;
		}
		else if( work[2] >= (USHORT)PHASE_DEG225 )
		{
			lwork1 = (lwork1 + (ULONG)PHASE_DEG360 + (ULONG)PHASE_DEG360) / 3;
		}
		else
		{
			/* 逆走した位相が135degの範囲外に存在したので、NoGood_flagをセット */
			*NoGood_flag = TRUE;
		}
	}
	else if( *Counter == 4 )
	{
		lwork1 = (ULONG)*Degree + (ULONG)*(Degree + 1) + (ULONG)*(Degree + 2) + (ULONG)*(Degree + 3);
		work[0] = (USHORT)( *(Degree + 1) - *Degree );
		work[1] = (USHORT)( *(Degree + 2) - *(Degree + 1) );
		work[2] = (USHORT)( *(Degree + 3) - *(Degree + 2) );

		lwork2 = 0;
		for( i = 0; i <= 2; i++ )
		{
			if( (work[i] != (USHORT)PHASE_DEG045) && (work[i] != (USHORT)PHASE_DEG225) )
			{
				/* 逆走した位相が135degの範囲外に存在したので、NoGood_flagをセット */
				*NoGood_flag = TRUE;
				break;
			}
			else if( work[i] == (USHORT)PHASE_DEG225 )
			{
				lwork2 = ((ULONG)PHASE_DEG360 * (i+1));
			}
		}

		if( *NoGood_flag == FALSE )
		{
			lwork1 = (lwork1 + lwork2) >> 2;
		}
	}
	*DegCalc = lwork1;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		最大トルク＆最大速度取得 : 運転中のd-q軸それぞれの最大トルクと最大速度を取得する。			*/
/*																									*/
/****************************************************************************************************/
static	void	MpFindGetMaxTrqSpd( MPFIND *MpFind, LONG SpdFbki, LONG TrqRefo )
{
	if( !MpFind->var.AxisChange )
	{
		/* q軸 */
		if( !MpFind->var.Direction )
		{
			/* 正方向速度指令時 */
			if( MpFind->var.qAxisMaxTrqPlus < TrqRefo )
			{
				MpFind->var.qAxisMaxTrqPlus = TrqRefo;
			}
			if( MpFind->var.qAxisMaxSpdPlus < SpdFbki )
			{
				MpFind->var.qAxisMaxSpdPlus = SpdFbki;
			}
		}
		else
		{
			/* 負方向速度指令時 */
			if( MpFind->var.qAxisMaxTrqMinus > TrqRefo )
			{
				MpFind->var.qAxisMaxTrqMinus = TrqRefo;
			}
			if( MpFind->var.qAxisMaxSpdMinus > SpdFbki )
			{
				MpFind->var.qAxisMaxSpdMinus = SpdFbki;
			}
		}
	}
	else
	{
		/* d軸 */
		if( !MpFind->var.Direction )
		{
			/* 正方向速度指令時 */
			if( MpFind->var.dAxisMaxTrqPlus < TrqRefo )
			{
				MpFind->var.dAxisMaxTrqPlus = TrqRefo;
			}
			if( MpFind->var.dAxisMaxSpdPlus < SpdFbki )
			{
				MpFind->var.dAxisMaxSpdPlus = SpdFbki;
			}
		}
		else
		{
			/* 負方向速度指令時 */
			if( MpFind->var.dAxisMaxTrqMinus > TrqRefo )
			{
				MpFind->var.dAxisMaxTrqMinus = TrqRefo;
			}
			if( MpFind->var.dAxisMaxSpdMinus > SpdFbki )
			{
				MpFind->var.dAxisMaxSpdMinus = SpdFbki;
			}
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		d-q軸ずれ角算出 : d-q軸それぞれの最大トルクの関係からずれ角算出する。						*/
/*																									*/
/****************************************************************************************************/
static	void	JudgeMpFindErrSign( MPFIND *MpFind )
{
	LONG	lwork1, lwork2;
	LONG	lwork3, lwork4;
	LONG	kx;

	lwork1 = MlibLABS( MpFind->var.dAxisMaxTrqPlus - MpFind->var.qAxisMaxTrqPlus );
	lwork2 = MlibLABS( MpFind->var.dAxisMaxTrqMinus - MpFind->var.qAxisMaxTrqMinus );
	lwork3 = MlibLABS( MpFind->var.dAxisMaxSpdPlus - MpFind->var.qAxisMaxSpdPlus );
	lwork4 = MlibLABS( MpFind->var.dAxisMaxSpdMinus - MpFind->var.qAxisMaxSpdMinus );

	if( lwork1 == lwork2 )
	{
		if( lwork3 < lwork4 )
		{
			/* 負方向採用 */
			lwork1 = MlibLABS( MpFind->var.dAxisMaxTrqMinus );
			lwork2 = MlibLABS( MpFind->var.qAxisMaxTrqMinus );
			lwork3 = MlibLABS( MpFind->var.dAxisMaxSpdMinus );
			lwork4 = MlibLABS( MpFind->var.qAxisMaxSpdMinus );
		}
		else
		{
			/* 正方向採用 */
			lwork1 = MpFind->var.dAxisMaxTrqPlus;
			lwork2 = MpFind->var.qAxisMaxTrqPlus;
			lwork3 = MpFind->var.dAxisMaxSpdPlus;
			lwork4 = MpFind->var.qAxisMaxSpdPlus;
		}
	}
	else if( lwork1 < lwork2 )
	{
		/* 負方向採用 */
		lwork1 = MlibLABS( MpFind->var.dAxisMaxTrqMinus );
		lwork2 = MlibLABS( MpFind->var.qAxisMaxTrqMinus );
		lwork3 = MlibLABS( MpFind->var.dAxisMaxSpdMinus );
		lwork4 = MlibLABS( MpFind->var.qAxisMaxSpdMinus );
	}
	else
	{
		/* 正方向採用 */
		lwork1 = MpFind->var.dAxisMaxTrqPlus;
		lwork2 = MpFind->var.qAxisMaxTrqPlus;
		lwork3 = MpFind->var.dAxisMaxSpdPlus;
		lwork4 = MpFind->var.qAxisMaxSpdPlus;
	}

	if( lwork1 == lwork2 )
	{
		lwork2 = 16384;
		lwork1 = AXISERR_EQUAL;
	}
	else if( lwork1 > lwork2 )
	{
		kx = MlibScalKxgain( lwork2, 1, lwork1, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork2 = 16384 * kx;
#else		
		lwork2 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */
		lwork1 = AXISERR_MINUS;
	}
	else
	{
		kx = MlibScalKxgain( lwork1, 1, lwork2, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork2 = 16384 * kx;
#else	
		lwork2 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */

		lwork1 = AXISERR_PLUS;
	}

	if( lwork3 == lwork4 )
	{
		lwork4 = 16384;
		lwork3 = AXISERR_EQUAL;
	}
	else if( lwork3 > lwork4 )
	{
		kx = MlibScalKxgain( lwork4, 1, lwork3, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork4 = 16384 * kx;
#else	
		lwork4 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */
		lwork3 = AXISERR_PLUS;
	}
	else
	{
		kx = MlibScalKxgain( lwork3, 1, lwork4, NULL, 24 );
#if (FLOAT_USE==TRUE)
		lwork4 = 16384 * kx;
#else	
		lwork4 = MlibMulgain( 16384, kx );
#endif /* FLOAT_USE */
		lwork3 = AXISERR_MINUS;
	}

	if( MpFind->var.Times >= MpFind->conf.RepeatNum )
	{
		MpFind->MpFindSpdRef = 0;
		MpFind->var.AdjDegree = SINPHASE_045;
		MpFind->var.AdjSwpDeg = SINPHASE_045;
		MpFind->var.Step = MPF_CAL;
	}
	else
	{
		MpFind->var.dAxisMaxTrqPlus = 0;
		MpFind->var.qAxisMaxTrqPlus = 0;
		MpFind->var.dAxisMaxTrqMinus = 0;
		MpFind->var.qAxisMaxTrqMinus = 0;
		MpFind->var.dAxisMaxSpdPlus = 0;
		MpFind->var.qAxisMaxSpdPlus = 0;
		MpFind->var.dAxisMaxSpdMinus = 0;
		MpFind->var.qAxisMaxSpdMinus = 0;
	}

	if( lwork1 == lwork3 )
	{
		MpFind->var.AxisErrSign = (UCHAR)lwork1;
		MpFind->var.AxisErr = lwork2;
	}
	else if( lwork2 <= lwork4 )
	{
		MpFind->var.AxisErrSign = (UCHAR)lwork1;
		MpFind->var.AxisErr = lwork2;
	}
	else
	{
		MpFind->var.AxisErrSign = (UCHAR)lwork3;
		MpFind->var.AxisErr = lwork4;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		位相計算 : 運転処理で得たデータから位相算出し、位相更新を行う。								*/
/*																									*/
/****************************************************************************************************/
static	void	UpdatePhaseCalculation( MPFIND *MpFind )
{
	LONG	lwk;

	MpFind->var.ClrIntegral = TRUE;
	MpFind->var.AdjSwpDeg >>= 1;

	if( MpFind->var.AxisErrSign != AXISERR_EQUAL )
	{
		lwk = ( MlibSins16( MpFind->var.AdjDegree, 0x11 ) << 14 ) /
				MlibSins16( MpFind->var.AdjDegree + SINPHASE_090, 0x11 );

		if( MpFind->var.AxisErr > MlibLABS( lwk ) )
		{
			MpFind->var.AdjDegree += MpFind->var.AdjSwpDeg;
		}
		else
		{
			MpFind->var.AdjDegree -= MpFind->var.AdjSwpDeg;
		}
	}

	if( (MpFind->var.AdjSwpDeg == 1) || (MpFind->var.AxisErrSign == AXISERR_EQUAL) )
	{
		lwk = MpFind->var.AdjDegree << 2;

		switch( MpFind->var.AxisErrSign )
		{
		case AXISERR_EQUAL :
			MpFind->MpfPhaseOffset += PHASE_DEG045;
			break;

		case AXISERR_MINUS :
			MpFind->MpfPhaseOffset += PHASE_DEG090 - (SHORT)lwk;
			break;

		case AXISERR_PLUS :
			MpFind->MpfPhaseOffset += (SHORT)lwk;
			break;

		default :
			break;
		}
		MpFind->var.Step = MPF_STOP;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		停止確認処理 : 磁極検出の停止確認処理を行う。												*/
/*																									*/
/****************************************************************************************************/
static	void	StopMpoleFind( MPFIND *MpFind, BOOL MotStopSts )
{
	if( MotStopSts == TRUE )
	{
		/* 磁極検出変数初期化 */
		MlibResetLongMemory( &MpFind->var, sizeof( MpFind->var ) >> 2 );

		MpFind->var.RangeFix = TRUE;
		MpFind->var.Step = MPF_CONFIRM;
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		動作確認処理(電流ロック方式) : 磁極検出の動作確認処理を行う。								*/
/*																									*/
/****************************************************************************************************/
static BOOL ConfirmMpoleFind( MPFIND *MpFind, KSGAIN Kphasepos, LONG dMotPos, LONG SpdFbki )
{
	BOOL pdet_err;
	BOOL WaitFinish;
	pdet_err = FALSE;
	WaitFinish = FALSE;

	/* 推力指令作成 */
	MpFind->var.TrqRefPhaConf = MpFindLauTrqRef( MpFind, &WaitFinish );

	switch( MpFind->var.ConfirmStep )
	{
		case MPFCSTEP_WAIT:
			/* ConfirmStepがPHSCHANGE, ADJUST以外の場合、オーバースピードをチェックする。 */
			if( MlibLABS( SpdFbki ) >= MpFind->conf.OverSpd )
			{
				pdet_err = TRUE;
			}
			/* 推力指令作成処理開始確認 */
			if( MpFind->var.RefOutStep == MPFTRQ_ACC )
			{
				MpFind->var.ConfirmStep = MPFCSTEP_PHSCHANGE;
			}
			break;


		case MPFCSTEP_PHSCHANGE:
			/* ConfirmStepがPHSCHANGE, ADJUSTの場合、移動距離をチェックする。 */
			/* 動作確認用位置作成 */
			MpFind->var.MovPosPhaConf += dMotPos;
			if( MlibLABS(MpFind->var.MovPosPhaConf) >= MpFind->conf.ErrOKLevel )
			{
				pdet_err = TRUE;
			}
			/* 検出結果一時退避 */
			MpFind->var.PhsOffsetCalRslt = MpFind->MpfPhaseOffset;
			/* 位相変更(d軸に電流を流す) */
			MpFind->MpfPhaseOffset -= PHASE_DEG090;
			MpFind->var.ConfirmStep = MPFCSTEP_ADJUST;
			break;


		case MPFCSTEP_ADJUST:
			/* ConfirmStepがPHSCHANGE, ADJUSTの場合、移動距離をチェックする。 */
			/* 動作確認用位置作成 */
			MpFind->var.MovPosPhaConf += dMotPos;
			if( MlibLABS(MpFind->var.MovPosPhaConf) >= MpFind->conf.ErrOKLevel )
			{
				pdet_err = TRUE;
			}
			/* 位相を固定するため、動いた分だけオフセットを調整する。 */
#if (FLOAT_USE==TRUE)
			MpFind->MpfPhaseOffset -= ( (float)dMotPos * Kphasepos );
#else
			MpFind->MpfPhaseOffset -= MlibMulgainNolim( dMotPos, Kphasepos );
#endif /* FLOAT_USE */

			/* WAIT終了タイミング確認 */
			if( WaitFinish == TRUE )
			{
				if( MpFind->var.RefOutStep == MPFTRQ_DEC )
				{ /* 定速動作終了 */
					MpFind->var.ConfirmStep = MPFCSTEP_PHSSTORE;
				}
				else if( MpFind->var.RefOutStep == MPFTRQ_END )
				{ /* 減速動作終了 */
					MpFind->var.ConfirmStep = MPFCSTEP_PHSREVERT;
				}
				else
				{ /* 加速動作終了 */
					; /* 加速動作終了時は処理なし */
				}
			}
			break;


		case MPFCSTEP_PHSSTORE:
			/* ConfirmStepがPHSCHANGE, ADJUST以外の場合、オーバースピードをチェックする。 */
			if( MlibLABS( SpdFbki ) >= MpFind->conf.OverSpd )
			{
				pdet_err = TRUE;
			}
			/* 最終位相を保存する。 */
			MpFind->var.PhsOffsetComp = MpFind->MpfPhaseOffset;
			/* 位相を固定するため、動いた分だけオフセットを調整する。 */
#if (FLOAT_USE==TRUE)
			MpFind->MpfPhaseOffset -= ( (float)dMotPos * Kphasepos );
#else
			MpFind->MpfPhaseOffset -= MlibMulgainNolim( dMotPos, Kphasepos );
#endif /* FLOAT_USE */
			MpFind->var.ConfirmStep = MPFCSTEP_ADJUST;
			break;


		case MPFCSTEP_PHSREVERT:
			/* ConfirmStepがPHSCHANGE, ADJUST以外の場合、オーバースピードをチェックする。 */
			if( MlibLABS( SpdFbki ) >= MpFind->conf.OverSpd )
			{
				pdet_err = TRUE;
			}
			/* 位相を元に戻す(q軸に電流を流す) */
			MpFind->MpfPhaseOffset = MpFind->var.PhsOffsetComp + PHASE_DEG090;
			MpFind->var.ConfirmStep = MPFCSTEP_WAIT;
			MpFind->var.Step = MPF_END;
			break;

		default:
			break;
	}

	/* 検出結果の補正量計算(for Monitor) */
	MpFind->var.PhaseOffsetCalComp = MpFind->MpfPhaseOffset - MpFind->var.PhsOffsetCalRslt;

	return pdet_err;
}



/****************************************************************************************************/
/*																									*/
/*		推力指令作成 : 磁極検出確認時の推力指令を作成する。											*/
/*																									*/
/****************************************************************************************************/
static	LONG	MpFindLauTrqRef( MPFIND *MpFind, BOOL *WaitFinish )
{
	LONG	TrqRef;

	/* 平均推力 */
	TrqRef = MpFind->var.TrqRefPhaConf;
/*--------------------------------------------------------------------------------------------------*/
/*		推力指令作成シーケンス																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( MpFind->var.RefOutStep )
	{
	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_INIT:	/* 待ち時間経過																*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = 0;
		MpFind->var.ClrIntegral = TRUE;
		MpFind->var.RefOutStep = MPFTRQ_ACC;
		MpFind->var.RefOutTime = 0;
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_ACC:	/* 推力指令増加																*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = MlibLaufilter( MpFind->conf.MaxTrq, TrqRef, MpFind->conf.dTrq );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.IncTime )
		{
			MpFind->var.RefOutStep = MPFTRQ_CONST;
			MpFind->var.RefOutTime = 0;
			*WaitFinish = TRUE;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_CONST:	/* 	推力指令一定															*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = MpFind->conf.MaxTrq;
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.ConstTime )
		{
			MpFind->var.RefOutStep = MPFTRQ_DEC;
			MpFind->var.RefOutTime = 0;
			*WaitFinish = TRUE;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_DEC:	/* 推力指令減少																*/
	/*----------------------------------------------------------------------------------------------*/
		TrqRef = MlibLaufilter( 0, TrqRef, MpFind->conf.dTrq );
		MpFind->var.RefOutTime++;
		if( MpFind->var.RefOutTime >= MpFind->conf.IncTime )
		{
			MpFind->var.RefOutStep = MPFTRQ_END;
			MpFind->var.RefOutTime = 0;
			*WaitFinish = TRUE;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
	case MPFTRQ_END:	/* 待ち時間経過																*/
	/*----------------------------------------------------------------------------------------------*/
		MpFind->var.ClrIntegral = TRUE;
		TrqRef = 0;
		MpFind->var.RefOutStep = MPFTRQ_ACC;
		MpFind->var.RefOutTime = 0;
		break;

	/*----------------------------------------------------------------------------------------------*/
	default :
	/*----------------------------------------------------------------------------------------------*/
		break;
	}

	return( MlibLimitul(TrqRef, MpFind->conf.MaxTrq, -MpFind->conf.MaxTrq) );
}





/****************************************************************************************************/
/*																									*/
/*		磁極検出用指令速度パラメータ計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		回転型のパラメータ																			*/
/*			Prm.pdetmaxspd_r	: Pn493	磁極検出指令速度			[min-1]							*/
/*																									*/
/*		リニアのパラメータ																			*/
/*			Prm.pdetmaxspd		: Pn485	磁極検出指令速度 			[mm/s]							*/
/*																									*/
/*		共通のパラメータ																			*/
/*			Prm.pdetmaxt		: Pn486 磁極検出指令加減速時間		[ms]							*/
/*			Prm.pdetclmpt		: Pn487 磁極検出指令一定速時間		[ms]							*/
/*			Prm.pdetwait		: Pn488	磁極検出指令待ち時間		[ms]							*/
/*			Prm.pdetgett		: Pn489	磁極検出データ取り始め時間	[ms]							*/
/*			Prm.pdetinteglesst	: Pn492 磁極検出積分レス開始時間	[%]								*/
/*			Prm.pdetrevlvspdref	: Pn499 磁極検出速度指令時逆走レベル[%]								*/
/*			Prm.pdetrevlvwait	: Pn49A 磁極検出待ち時間時逆走レベル[%]								*/
/*			Prm.pdetoslevel		: Pn49B 磁極検出過速度レベル		[%]								*/
/*																									*/
/****************************************************************************************************/
void	MpoleFind_CalcSpeedCommandPrm( MPFIND *MpFind, MENCV *MencV,
    	                               BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm )
{
	LONG		work;

	if( MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型モータ */
		if( MencV->LowSpdMotor )
		{
			work = PdetPrm->pdetmaxspd_r;
		}
		else
		{
			work = 10 * PdetPrm->pdetmaxspd_r;
		}
		MpFind->conf.MaxSpd = BprmSpeedLevelCal( Bprm, work, 0 );
	}
	else if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニアモータ */
		MpFind->conf.MaxSpd = BprmSpeedLevelCal( Bprm, (PdetPrm->pdetmaxspd * 10), 0 );
	}

	MpFind->conf.ReverseSpd		= MpFind->conf.MaxSpd * PdetPrm->pdetrevlvspdref / 100;
	MpFind->conf.ReverseSpd2	= MpFind->conf.MaxSpd * PdetPrm->pdetrevlvwait / 100;
	MpFind->conf.OverSpd		= MpFind->conf.MaxSpd * PdetPrm->pdetoslevel / 100;

	MpFind->conf.dSpd			= MpFind->conf.MaxSpd * Bprm->MPFCycleUs_MR / 1000 / PdetPrm->pdetmaxt;
	MpFind->conf.AccTime		= PdetPrm->pdetmaxt * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.ClmpTime		= PdetPrm->pdetclmpt * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.WaitTime		= PdetPrm->pdetwait * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.InteglessTime	= MpFind->conf.WaitTime * PdetPrm->pdetinteglesst / 100;

	MpFind->conf.MaxTrq		= BprmTorqueLevelCal( Bprm, (PdetPrm->pdetmaxtrq * 100), 0 );
	MpFind->conf.dTrq		= MpFind->conf.MaxTrq * Bprm->MPFCycleUs_MR / 1000 / PdetPrm->pdetinctime;
	MpFind->conf.IncTime	= PdetPrm->pdetinctime * 1000 / Bprm->MPFCycleUs_MR;
	MpFind->conf.ConstTime	= PdetPrm->pdetconsttime * 1000 / Bprm->MPFCycleUs_MR;

}



/****************************************************************************************************/
/*																									*/
/*		磁極検出用速度制御パラメータ計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			Prm.pdetloophz		: Pn481 磁極検出時速度ループゲイン		[0.1Hz]						*/
/*			Prm.pdetpitime		: Pn482 磁極検出時速度ループ積分時間	[0.01ms]					*/
/*			Prm.pdetjrate		: Pn490	磁極検出負荷レベル				[%]							*/
/*			Prm.pdetintegless	: Pn491 磁極検出積分レスゲイン			[0.01%]						*/
/*			磁極検出時は、磁極検出用負荷重量比を使用する											*/
/*			磁極検出時は、i-P制御使用不可															*/
/*																									*/
/****************************************************************************************************/
void	MpoleFind_CalcSpeedControlPrm( MPFIND *MpFind,
    	                               USHORT trqfil11, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  )
{
#if (FLOAT_USE==TRUE)

	float	fwk;

	fwk = Bprm->Kvx * (float)((100 + PdetPrm->pdetjrate) * PdetPrm->pdetloophz) / 1000.0F;
	MpFind->conf.Kv = fwk;

	fwk = 1.0F * (float)Bprm->MPFCycleUs_SC / (float)(10 * PdetPrm->pdetpitime);
	MpFind->conf.Ki = fwk;

	fwk = (float)PdetPrm->pdetintegless * 10.0f * (float)PdetPrm->pdetpitime / 10000.0f;
	MpFind->conf.InteglessGain = fwk / (float)Bprm->MPFCycleUs_SC;

#else
	LONG		s;
	LONG		wk;

	MpFind->conf.Kv = MlibScalKskxkx( Bprm->Kvx, (100 + PdetPrm->pdetjrate) * PdetPrm->pdetloophz, 1000, NULL, 24 );
	MpFind->conf.Ki = MlibScalKxgain( 1, Bprm->MPFCycleUs_SC, 10*PdetPrm->pdetpitime, NULL, 24 );

	wk = MlibScalKxgain( PdetPrm->pdetintegless, 10*PdetPrm->pdetpitime, 10000, &s, 0 );
	MpFind->conf.InteglessGain = MlibPcalKxgain( wk, 1, Bprm->MPFCycleUs_SC, &s, 24 );
#endif /* FLOAT_USE */

	MpFind->conf.Klpf = MlibPcalKf1gain( 10 * trqfil11, Bprm->MPFCycleUs_SC, 0 );

}



/****************************************************************************************************/
/*																									*/
/*		磁極検出エラーレベル計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		回転型の場合																				*/
/*			Prm.pdetdistok_r: Pn494 磁極検出可動範囲		[0.001rev]								*/
/*			Prm.pdetdegreeok: Pn498 磁極検出確認合格範囲	[deg(電気角)]							*/
/*						  pdetdegreeokpole * (2^BitNo)												*/
/*			ErrOKLevel = ------------------------------		[pulse]									*/
/*									pole / 2 														*/
/*																									*/
/*		リニアの場合																				*/
/*			Prm.pdetdistok	: Pn48E 磁極検出可動範囲								[mm]			*/
/*			Prm.pdetdegreeok: Pn498 磁極検出確認合格範囲							[deg(電気角)]	*/
/*						  pdetdegreeokpole * (ppitch/10*10^-3 * 2) * (2^BitNo)						*/
/*			ErrOKLevel = ------------------------------------------------------		[pulse]			*/
/*									360 * ScalePitch/100*10^-6										*/
/*																									*/
/****************************************************************************************************/
void	MpoleFind_CalcErrorLevel( MPFIND *MpFind, MENCV *MencV,
    	                          MENCPRM *MencP, BPRMDAT *Bprm, MPFIND_UPRM *PdetPrm  )
{
	LONG		kx, sx;
	LONG		work;

	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニアモータ */
		MpFind->conf.DetectRange = 100 * PdetPrm->pdetdistok;
		/* 電気角をパルス数に変換する。 */
		work = MencV->NormScalePitch;
		kx = MlibScalKxgain( PdetPrm->pdetdegreeok, MencP->ppitch, 360, &sx, 0 );
		kx = MlibPcalKxgain( kx, (1 << MencP->bit_dp.b.l), work, &sx, 0 );
		kx = MlibPcalKxgain( kx, MencV->NormScaleExp, 1, &sx, 0 );
		kx = MlibPcalKxgain( kx, 200000, 1, &sx, -30 );
		MpFind->conf.ErrOKLevel = kx;
	}
	else
	{ /* 回転型モータ */
		if( MencV->LowSpdMotor )
		{
			MpFind->conf.DetectRange = PdetPrm->pdetdistok_r / 10;
		}
		else
		{
			MpFind->conf.DetectRange = PdetPrm->pdetdistok_r;
		}
		/* 電気角をパルス数に変換する。 */
		MpFind->conf.ErrOKLevel =
			MlibScalKxgain( PdetPrm->pdetdegreeok, Bprm->MotPulse, (180 * MencP->oslv_pol.b.h), NULL, -30 );
	}
}




/***************************************** end of file **********************************************/
