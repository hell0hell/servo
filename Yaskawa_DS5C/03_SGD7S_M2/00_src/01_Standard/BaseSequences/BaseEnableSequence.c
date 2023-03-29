/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BaseEnableSequence.c : ベースイネーブルシーケンス管理処理定義								*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ベースイネーブルシーケンス管理処理														*/
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
/*	Note	:	初版	2011.02.03	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "KLib.h"						/* <S053> */
#include "BaseEnableSequence.h"
#include "PowerManager.h"


#define CHARGE_PUMP_CMP_TIME 0x0080		/* チャージポンプ完了時間：125[us]*128 =16[ms]	*//* <S094> */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
void	CpxZeroClampSequence( 						/* @@Zero Clamp Sequence 						*/
	ZC_CTRL_DATA *Zcv,								/* Zero Clamp Control							*/
	BASE_CTRL_OUT	*Bout,							/* Base Control Output 							*/
	FUN_CMN_CONTROL *Fctrl,							/* Function Control								*/
	SEQ_CTRL_OUT *Cout	);							/* Sequence Output 								*/
/*--------------------------------------------------------------------------------------------------*/
static LONG	LpxCheckServoOnCmdErr( 					/* <S0A7> Check Servo On Cmd Error (A.0B0)		*/
	BE_SEQ_HNDL 	*BeSeq, 						/* 												*/
	BOOL			ServoOnReq,						/* Servo On Command								*/
	FUN_CMN_CONTROL *FnCmnCtrl, 					/* 												*/
	CTRLMD 			CtrlMode );						/* Control Mode									*/

/****************************************************************************************************/
/*																									*/
/*		サーボレディ信号設定処理																	*/
/*																									*/
/****************************************************************************************************/
void BeSeqMakeServoReadySignal( BE_SEQ_HNDL *BeSeq, BOOL ready_cond, LONG PhaseReady )
{
	if(BeSeq->SvonEnable && ready_cond)
	{
		/* Servo Ready flag set (Ignore Phase) */
		BeSeq->SvReadyPhaseIgnore = TRUE;
		/* Servo Ready flag set */
		if( PhaseReady )
		{
			BeSeq->ServoReady = TRUE;
		}
		else
		{
			BeSeq->ServoReady = FALSE;
		}
	}
	else
	{
		/* Servo Ready flag set (Ignore Phase) */
		BeSeq->SvReadyPhaseIgnore = FALSE;
		/* Servo Ready flag set */
		BeSeq->ServoReady = FALSE;
	}
}



/****************************************************************************************************/
/*																									*/
/*		センサオン/サーボオン要求設定処理															*/
/*																									*/
/****************************************************************************************************/
ALMID_T	BeSeqMakeServoOnRequest( BE_SEQ_HNDL *BeSeq, LONG CmdSeqBit,
       	                          CTRLMD CtrlMode, FUN_CMN_CONTROL *FnCmnCtrl, MENCV *MencV )
{
	ALMID_T svon_err;
	BOOL SensOnReq, LastSensOnReq;
	BOOL ServoOnReq,PdetReq;

	svon_err = NO_ALARM;

	if(FnCmnCtrl->KnlMode != KPI_KMD_NORMAL)
	{/* Test mode */
		BeSeq->SvonReq = FALSE;
		SensOnReq = (0 != (CmdSeqBit & SENSOR_ON_BIT));
		FnCmnCtrl->State.FnSvCtrlDisable = FALSE;
	}
	else
	{/* Normal mode */
		if(FnCmnCtrl->FnSvControl != FALSE)
		{/* Fn modeによる運転 */
			ServoOnReq = FnCmnCtrl->FnSvonReq;
			SensOnReq = (0 != (CmdSeqBit & SENSOR_ON_BIT));
			SensOnReq |= FnCmnCtrl->FnSensOnReq;
//			PdetReq = FnCmnCtrl->PdetReq;
			PdetReq = FnCmnCtrl->SvOnPdetReq;					/* <S21C> */

			FnCmnCtrl->State.LockNormalOpeDrive = TRUE;
		}
		else
		{/* 通常指令による運転 */
			ServoOnReq = (0 != (CmdSeqBit & SERVO_ON_BIT));
			SensOnReq = (0 != (CmdSeqBit & SENSOR_ON_BIT));
			PdetReq = (0 != (CmdSeqBit & POL_DETECT_BIT));

			/* サーボON指令チェック */
			if( LpxCheckServoOnCmdErr( BeSeq, ServoOnReq, FnCmnCtrl, CtrlMode ) )	/* <S0A7> */
			{
				svon_err = ALM_SVONCMD;		/* A.0B0 : SGDVと若干条件が違うの注意!! */
				ServoOnReq = FALSE;
			}
#if 0		/* <S0A7> */
			if(FnCmnCtrl->State.LockNormalOpeDrive != FALSE)
			{/* 通常運転禁止*/
				if( (CtrlMode.us.uwd_l == CTRLMODE_SPD) || (CtrlMode.us.uwd_l == CTRLMODE_TRQ) )
				{
					BeSeq->SenStatus = FALSE;
					FnCmnCtrl->State.LockNormalOpeDrive = FALSE;
				}
				else if( (ServoOnReq != FALSE) && (CtrlMode.us.uwd_l == CTRLMODE_POS) )
				{/* サーボオン指令あり & 位置制御モード指令 */
					svon_err = ALM_SVONCMD;	/* A.0B0 : SGDVと若干条件が違うの注意!! */
					ServoOnReq = FALSE;
				}
				else
				{	/* do nothing. */
					;
				}
			}
#endif
		}

		if( MencV->PhaseReady != FALSE)
		{
			BeSeq->SvonReq = ServoOnReq;
			FnCmnCtrl->State.FnSvCtrlDisable = ServoOnReq;
		}
		else
		{
			BeSeq->SvonReq = PdetReq;
			FnCmnCtrl->State.FnSvCtrlDisable = ServoOnReq | PdetReq;
			if((ServoOnReq != FALSE) /*&& (FnCmnCtrl->PdetReq == FALSE)*/ )
			{
				svon_err = ALM_PDET;	/* A.C52 */
			}
		}
	}

	/* SENSER_ON要求取得 */
	LastSensOnReq = BeSeq->SenStatus;
	if( (MencV->AbsoEncType != MENC_INCRE_TYPE) && (MencV->AxisMotType == MOTTYPE_ROTARY) )
	{ /* 絶対値エンコーダ使用時 */
		if( (LastSensOnReq == FALSE) && (SensOnReq != FALSE) )
		{ /* SEN_ON初回 */
#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
			if( MencV->DivInfo.DivOut == FALSE )
			{
				MencV->ReCalcAbsPosReq = TRUE;
			}
			else
			{
				;/*何もしない(シリアルエンコーダＰＡＯ相シリアル出力にて、MencV->ReCalcAbsPosReq = TRUEとする */
			}
#else/*<S02D>*/
			MencV->ReCalcAbsPosReq = TRUE;
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/
		}
	}

	/* 内部状態更新 */
	BeSeq->SenStatus = SensOnReq /* <S082> & MencV->EncReady */;

	return svon_err;
}


#if 0 /* PcmRlyDbControlProcess()に統合 */
/****************************************************************************************************/
/*																									*/
/*		ＤＢ回路制御																				*/
/*																									*/
/****************************************************************************************************/
void BeSeqDBControlProcess( BOOL DbOn, LONG RlyDbWay, USHORT faultState, ASIC_HW_IF_STR *pAsicHwReg,
							BOOL MotorLessTestMode )													/* <S1B7> */
{

	switch( RlyDbWay )
	{
	case DBCTRL_RLY_COMMON:				/* 突入＋ＤＢ：リレー共通 								*/
		/* PcmRlyControlProcess()にて制御する */
		break;

	case DBCTRL_TRIACK_SEPARATE:		/* 突入：リレー；ＤＢ：サイリスタ 						*/
	case DBCTRL_RLY_SEPARATE:			/* 突入：リレー；ＤＢ：リレー 							*/
		if( ((faultState & SHAL_FAULT_BB) != 0) && (DbOn == TRUE) )
		{
			/* DBON */
			SHalDrv_DynamicBrakeON(pAsicHwReg);
		}
		else
		{
			/* モータレステストモードでなければOFFする *//* <S1B7> */
			/* TODO: SGD-Vでは以下の条件もORしてある							*/
			/* (1) AxisRscC->BeSequence->StopMode == STOPMODE_DBSTOPFR			*/
			/* (2) AxisRscC->BeSequence->StopMode == STOPMODE_FREERUN			*/
			/* (3) AxisRscC->FnCmnCtrl->KnlMode == KPI_KMD_ADJCURAD				*/
			if( MotorLessTestMode == FALSE )
			{
				/* DBOFF */
				SHalDrv_DynamicBrakeOFF(pAsicHwReg);
			}
		}
		break;

	case DBCTRL_TRIACK_COMMON:			/* 突入＋ＤＢ：サイリスタ 								*/
		/* 未対応 */
		break;

	default:
		break;
	}

}
#endif


/****************************************************************************************************/
/*																									*/
/*		スキャンＣ制御モード指令																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 各シーケンスからの要求を受けて、優先度に基づき制御モードを決定し、スキャンＢに			*/
/*			対して、制御モードを指令する。															*/
/*			なお、最終のサーボ制御モードは、スキャンＢで決定される(BoutV.CtrlMode)。				*/
/*																									*/
/****************************************************************************************************/
ULONG BeSeqControlModeManager( BE_SEQ_HNDL *BeSeq, FUN_CMN_CONTROL *FnCmnCtrl )
{
	CTRLMD	CtrlMcmd;

	if(FnCmnCtrl->FnSvControl != FALSE)
	{/* Fn modeによる運転 */
		/* ScanC Control Mode <= ROUND要求モード */
		CtrlMcmd.dw = (ULONG)FnCmnCtrl->FnCtrlMcmd;
	}
	else
	{	/* ScanC Control Mode <= NO CMD */
		CtrlMcmd.dw = CTRLMODE_NOCMD;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		ScanC：ゼロ制御モード指令																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( BeSeq->StopMode == STOPMODE_ZEROSTOP )			/* ストップモード管理						*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZSTOP;					/* ゼロ速停止モード							*/
	}
	/*----------------------------------------------------------------------------------------------*/
/* <S1F5> Start */
	else if( BeSeq->StopMode == STOPMODE_LDECSTOP )		/* ストップモード管理						*/
	{
		CtrlMcmd.b.zm = CTRLMODE_LDSTOP;				/* 直線減速度による減速停止モード			*/
	}
	/*----------------------------------------------------------------------------------------------*/
/* <S1F5> End */
	else if( BeSeq->OtSeqData.var.OtZeroClamp )			/* OTシーケンス								*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZCLMP;					/* ゼロクランプモード						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else if( BeSeq->ZcCtrlData.ZcZeroClamp )			/* ゼロクランプシーケンス					*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZCLMP;					/* ゼロクランプモード						*/
	}
	/*----------------------------------------------------------------------------------------------*/
	else												/* デフォルト処理							*/
	{
		CtrlMcmd.b.zm = CTRLMODE_ZNONE;					/* ゼロ制御なし								*/
	}

	return CtrlMcmd.dw;
}



/****************************************************************************************************/
/*																									*/
/*		アラーム停止モード設定処理																	*/
/*																									*/
/****************************************************************************************************/
void BeSeqInputAlmStopMode( BE_SEQ_HNDL *BeSeq, LONG AlmStopMode,   /* <S1F5> */
                            CTRLMD CtrlMode, BOOL PhaseReady, SEQ_CTRL_OUT *Cout )  /* <S1F5> */
// void BeSeqInputAlmStopMode( ALM_STOP *AlmStop, LONG AlmStopMode, /* <S1F5> */
//                           CTRLMD CtrlMode, BOOL PhaseReady, BOOL HwbbSts, BOOL MotStop )  /* <S1F5> */
{
/* <S1F5> Start */
	ALM_STOP *AlmStop;
	BOOL HwbbSts;
	BOOL MotStop;

	/* ローカル変数の初期化 */
	AlmStop = &(BeSeq->AlmStop);
	HwbbSts = Cout->HwbbSts;
	MotStop = Cout->MotStop;
/* <S1F5> End */
/*--------------------------------------------------------------------------------------------------*/
/*		アラームストップモード設定処理																*/
/*--------------------------------------------------------------------------------------------------*/
	if( AlmStopMode == STOPMODE_ZEROSTOP )						/* アラームゼロ速停止？				*/
	{
		if( (CtrlMode.b.cm == CTRLMODE_TRQ) ||					/* トルク制御中？					*/
//			(AlmStop->conf.G2AlmStopMode == TRUE) ||			/* G2ALM停止方法がPn001.0に従う		*/  /* <S1F5> */
			(AlmStop->conf.G2AlmStopMode == GR2STOPMODE_DEFAULT) ||	/* G2ALM停止方法がPn001.0に従う	*/  /* <S1F5> */
			(HwbbSts == TRUE) ||								/* HWBB状態中？						*/
			(PhaseReady == FALSE) )								/* 磁極検出中?						*/
		{
			/* Pn001.0の停止方法に変更 */
			AlmStopMode = AlmStop->conf.StopMode;
		}
/* <S1F5> Start */
		else
		{	/* G2ALM停止方法	*/
			AlmStopMode = AlmStop->conf.G2AlmStopMode;
		}
/* <S1F5> End */
	}
	AlmStop->var.AlmStopMode = AlmStopMode;
#if 0	/* <S1F5> Start	*/
/*--------------------------------------------------------------------------------------------------*/
/*		アラーム：ゼロ速停止要求信号																*/
/*--------------------------------------------------------------------------------------------------*/
	if( AlmStopMode == STOPMODE_ZEROSTOP )
	{ /* 異常停止モード : ゼロ速停止 */
		if( MotStop )
		{ /* モータ停止中 */
			/* ゼロ速停止完了フラグON */
			AlmStop->var.AlmZstopFinish = TRUE;
		}

		if( AlmStop->var.AlmZstopFinish == FALSE )
		{
			/* ゼロ速停止要求フラグON */
			AlmStop->var.AlmZeroStop = TRUE;
		}
		else
		{
			/* ゼロ速停止要求フラグOFF */
			AlmStop->var.AlmZeroStop = FALSE;
		}
	}
	else
	{ /* 異常停止モード : ゼロ速停止以外 */
		AlmStop->var.AlmZeroStop = FALSE;
		AlmStop->var.AlmZstopFinish = FALSE;
	}
#endif	/* <S1F5> End	*/
/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		アラームストップシーケンス																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( BeSeq->LdstpTimeZero == TRUE )
	{	/* Pn30A = 0とした場合、ゼロ速停止となる。ただし、Pn406によるトルク制限は有効。	*/
		if( AlmStop->var.AlmStopMode == GR2STOPMODE_LDSTOPDB )
		{
			AlmStop->var.AlmStopMode = GR2STOPMODE_ZSTOPDB;
		}
		else if( AlmStop->var.AlmStopMode == GR2STOPMODE_LDSTOPFR )
		{
			AlmStop->var.AlmStopMode = GR2STOPMODE_ZSTOPFR;
		}
		else
		{/* do nothing */
			;
		}
	}
	
	switch( AlmStop->var.Gr2StopStep )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:	/* Initial Step																			*/
/*--------------------------------------------------------------------------------------------------*/
		AlmStop->var.Gr2TrqLimit = FALSE;
		AlmStop->var.Gr2ZeroStop = FALSE;
		AlmStop->var.Gr2LinDecStop = FALSE;
		AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
		AlmStop->var.Gr2StopStep = 1;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:	/* Alm停止モードによる分岐																*/
/*--------------------------------------------------------------------------------------------------*/
		switch( AlmStop->var.AlmStopMode )
		{
			case GR2STOPMODE_ZSTOPDB:			/* ゼロ速停止＆DB停止状態							*/
			case GR2STOPMODE_ZSTOPFR:			/* ゼロ速停止＆フリーラン状態						*/
				AlmStop->var.Gr2TrqLimit = TRUE;
				AlmStop->var.Gr2ZeroStop = TRUE;
				AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
				AlmStop->var.Gr2StopStep = 2;
				break;
			case GR2STOPMODE_LDSTOPDB:			/* Pn30Aに従ってモータを減速停止＆DB停止状態		*/
			case GR2STOPMODE_LDSTOPFR:			/* Pn30Aに従ってモータを減速停止＆フリーラン状態	*/
				AlmStop->var.Gr2LinDecStop = TRUE;
				AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
				AlmStop->var.Gr2StopStep = 3;
				break;
			default:
				break;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:	/* ゼロ速停止中、アラームストップ解除／モータ停止待ち									*/
/*--------------------------------------------------------------------------------------------------*/
		if( (AlmStop->var.AlmStopMode != GR2STOPMODE_ZSTOPDB)
		 && (AlmStop->var.AlmStopMode != GR2STOPMODE_ZSTOPFR) )
		{
			AlmStop->var.Gr2TrqLimit = FALSE;
			AlmStop->var.Gr2ZeroStop = FALSE;
			AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
			AlmStop->var.Gr2StopStep = 1;
		}
		else if( MotStop )
		{
			AlmStop->var.Gr2ZeroStop = FALSE;
			if( AlmStop->var.AlmStopMode == GR2STOPMODE_ZSTOPFR )
			{
				AlmStop->var.Gr2DbopReq = GR2DBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:	/* 直線減速停止中、アラームストップ解除／モータ停止待ち									*/
/*--------------------------------------------------------------------------------------------------*/
		if( (AlmStop->var.AlmStopMode != GR2STOPMODE_LDSTOPDB)
		 && (AlmStop->var.AlmStopMode != GR2STOPMODE_LDSTOPFR) )
		{
			AlmStop->var.Gr2LinDecStop = FALSE;
			AlmStop->var.Gr2DbopReq = GR2DBOP_NONE;
			AlmStop->var.Gr2StopStep = 1;
		}
		else if( MotStop )
		{
			AlmStop->var.Gr2LinDecStop = FALSE;
			if( AlmStop->var.AlmStopMode == GR2STOPMODE_LDSTOPFR )
			{
				AlmStop->var.Gr2DbopReq = GR2DBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
	default:
		break;
	}
/* <S1F5> End */
}

/* <S1F5> Start */
/****************************************************************************************************/
/*																									*/
/*		強制停止モード設定処理																		*/
/*																									*/
/****************************************************************************************************/
void BeSeqForceStopMode( BE_SEQ_HNDL *BeSeq, CTRLMD CtrlMode, BOOL MotStop, BOOL FstpSts )
{
	FORCE_STOP *ForceStop;
	ALM_STOP *AlmStop;

	/* ローカル変数の初期化 */
	ForceStop = &( BeSeq->ForceStop );
	AlmStop = &( BeSeq->AlmStop );

	switch( ForceStop->var.FstpStopStep )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:	/* Initial Step																			*/
/*--------------------------------------------------------------------------------------------------*/
		ForceStop->var.FstpTrqLimit = FALSE;
		ForceStop->var.FstpZeroStop = FALSE;
		ForceStop->var.FstpLinDecStop = FALSE;
		ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
		ForceStop->var.FstpMode = STOPMODE_NOTSTOP;
		ForceStop->var.FstpStopStep = 1;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:	/* 強制停止監視 (強制停止でない状態)													*/
/*--------------------------------------------------------------------------------------------------*/
		if( FstpSts == TRUE )
		{
			if( (CtrlMode.b.cm == CTRLMODE_TRQ)
			 || (ForceStop->conf.ForceStopMode == FSTOPMODE_DEFAULT) )
			{
				ForceStop->var.FstpMode = AlmStop->conf.StopMode;

				break;
			}
			else
			{
				ForceStop->var.FstpMode = ForceStop->conf.ForceStopMode;

				if( BeSeq->LdstpTimeZero == TRUE )
				{	/* Pn30A = 0とした場合、ゼロ速停止となる。ただし、Pn406によるトルク制限は有効。	*/
					if( ForceStop->var.FstpMode == FSTOPMODE_LDSTOPDB )
					{
						ForceStop->var.FstpMode = FSTOPMODE_ZSTOPDB;
					}
					else if( ForceStop->var.FstpMode == FSTOPMODE_LDSTOPFR )
					{
						ForceStop->var.FstpMode = FSTOPMODE_ZSTOPFR;
					}
					else
					{/* do nothing */
						;
					}
				}
			}
			
			switch( ForceStop->var.FstpMode )
			{
			case FSTOPMODE_ZSTOPDB:
			case FSTOPMODE_ZSTOPFR:
				ForceStop->var.FstpTrqLimit = TRUE;
				ForceStop->var.FstpZeroStop = TRUE;
				ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
				ForceStop->var.FstpStopStep = 2;
				break;
			case FSTOPMODE_LDSTOPDB:
			case FSTOPMODE_LDSTOPFR:
				ForceStop->var.FstpLinDecStop = TRUE;
				ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
				ForceStop->var.FstpStopStep = 3;
				break;
			default:
				break;
			}
		}
		else
		{
			ForceStop->var.FstpMode = STOPMODE_NOTSTOP;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:	/* ゼロ速停止中、強制停止解除／モータ停止待ち											*/
/*--------------------------------------------------------------------------------------------------*/
		if( FstpSts == FALSE )
		{
			ForceStop->var.FstpTrqLimit = FALSE;
			ForceStop->var.FstpZeroStop = FALSE;
			ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
			ForceStop->var.FstpStopStep = 1;
		}
		else if( MotStop )
		{
			ForceStop->var.FstpZeroStop = FALSE;
			if( ForceStop->var.FstpMode == FSTOPMODE_ZSTOPFR )		/* <S1FF> */
			{
				ForceStop->var.FstpDbopReq = FSTPDBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:	/* 直線減速停止中、強制停止解除／モータ停止待ち											*/
/*--------------------------------------------------------------------------------------------------*/
		if( FstpSts == FALSE )
		{
			ForceStop->var.FstpLinDecStop = FALSE;
			ForceStop->var.FstpDbopReq = FSTPDBOP_NONE;
			ForceStop->var.FstpStopStep = 1;
		}
		else if( MotStop )
		{
			ForceStop->var.FstpLinDecStop = FALSE;
			if( ForceStop->var.FstpMode == FSTOPMODE_LDSTOPFR )		/* <S1FF> */
			{
				ForceStop->var.FstpDbopReq = FSTPDBOP_FREE;
			}
		}
		else
		{/* do nothing */
			;
		}
		break;
	default:
		break;
	}
}
/* <S1F5> End */

/****************************************************************************************************/
/*																									*/
/*		メカニカルブレーキシーケンス(ＢＫシーケンス)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ブレーキモータを使う場合のブレーキ指令処理およびＤＢシーケンスに対するベース			*/
/*			イネーブル指令処理を行う。																*/
/*			ブレーキオン時のサーボオフタイミング、モータ停止時のブレーキタイミング設定は			*/
/*			ユーザー定数により回転数、時間を設定できる。											*/
/*			OT時の異常処理を除いて、サーボ運転のベースイネーブル／ベースブロック指令は				*/
/*			本シーケンスから指令される。															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
BOOL BeSeqMechaBrakeSequence( BE_SEQ_HNDL *BeSeq, CTRLMD CtrlMode,
    	                       LONG AbsMotSpd, BOOL HwbbStatus, USHORT faultState,
    						   BOOL BeComplete, BOOL MotorLessTestMode)								/* <S1B7> */
{
	BK_SEQ_DATA 	*BkSeqData;
//	ALM_STOP 		*alm_stop_ptr;  /* <S1F5> */
	ALM_STOP 		*AlmStop;  /* <S1F5> */
	FORCE_STOP		*ForceStop;  /* <S1F5> */

	/* ローカル変数の初期化 */
	BkSeqData = &BeSeq->BkSeqData;
//	alm_stop_ptr = &BeSeq->AlmStop;  /* <S1F5> */
	AlmStop = &BeSeq->AlmStop;  /* <S1F5> */
	ForceStop = &BeSeq->ForceStop;  /* <S1F5> */

/*--------------------------------------------------------------------------------------------------*/
/*		ブレーキシーケンス処理																		*/
/*--------------------------------------------------------------------------------------------------*/
	switch( BkSeqData->var.BkStep )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:	/* Initial Step																			*/
/*--------------------------------------------------------------------------------------------------*/
		BkSeqData->var.Brake = TRUE;					/* ブレーキ指令出力							*/
		BkSeqData->var.BkZeroStop = FALSE;				/* ゼロ速停止要求オフ						*/
		BkSeqData->var.BkLinDecStop = FALSE;			/* 直線減速停止要求オフ						*/	/* <S1F5> */
		BkSeqData->var.BkWait = 0;
		BkSeqData->var.BkStep = 1;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:	/* サーボオフ中で、サーボオンするのを待っている状態										*/
/*--------------------------------------------------------------------------------------------------*/
		if( BeSeq->SvonReq && BeSeq->SvReadyPhaseIgnore )
		{
			/* Base Enable Request ON */
			BkSeqData->var.BkBeReq = TRUE;

			/* モータレステストモード状態により判定方法を変更する *//* <S1B7> */
			if( MotorLessTestMode == FALSE )	/* モータレスモード無効 */
			{
				if( (faultState & SHAL_FAULT_BB) == 0 )
				{
					/* Brake Release */
					BkSeqData->var.Brake = FALSE;
					BkSeqData->var.BkWait = 0;
					BkSeqData->var.BkStep = 2;
				}
			}
			else								/* モータレスモード有効 */
			{
				if( BeComplete == TRUE )	/* ベースイネーブル */
				{
					/* Brake Release */
					BkSeqData->var.Brake = FALSE;
					BkSeqData->var.BkWait = 0;
					BkSeqData->var.BkStep = 2;
				}
			}
		}
		else
		{
			/* Base Enable Request OFF */
			BkSeqData->var.BkBeReq = FALSE;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2: /* サーボオン中で、サーボオフまたはサーボレディオフを待っている状態						*/
/*--------------------------------------------------------------------------------------------------*/
		if( BeSeq->SvonReq && BeSeq->SvReadyPhaseIgnore )
		{ /* サーボオン要求 */
			;											/* サーボオン要求オフ待ち					*/
		}
//		else if( alm_stop_ptr->var.AlmZeroStop )  /* <S1F5> */
		else if( (AlmStop->var.Gr2ZeroStop == TRUE)			/* <S1F5> */
			|| (ForceStop->var.FstpZeroStop == TRUE))		/* <S1F5> */
		{ /* アラームゼロ速停止中、または強制停止ゼロ速停止中 */		/* <S1F5> */
			;											/* アラームと強制停止：ゼロ速停止中オフ待ち	*/   /* <S1FS1F5> */
		}
/* <S1F5> Start */
		else if( (AlmStop->var.Gr2LinDecStop == TRUE)
			|| (ForceStop->var.FstpLinDecStop == TRUE) )
		{
			BkSeqData->var.BkStep = 3;
		}
/* <S1F5> End */
		else if( AbsMotSpd >= BkSeqData->conf.BkonSpdLevel )
		{ /* モータ速度がブレーキ指令以上 */			/* 出力速度(Pn507)以上なら					*/
			BkSeqData->var.BkBeReq = FALSE;				/* ブレーキをかけずに						*/
			BkSeqData->var.BkStep = 4;					/* ベースイネーブル要求オフ					*/	/* <S1F5> */
		}
//		else if( (alm_stop_ptr->var.AlmStopMode > STOPMODE_ZEROSTOP)  /* <S1F5> */
		else if( (AlmStop->var.AlmStopMode >= STOPMODE_DBSTOPDB)  /* <S1F5> */
			  || (ForceStop->var.FstpMode >= STOPMODE_DBSTOPDB)   /* <S1F5> */
				|| (BkSeqData->conf.BkonDelayTime == 0)
				|| (HwbbStatus == TRUE) )
		{												/* ゼロ速停止以外のアラーム or HWBB状態		*/
			BkSeqData->var.BkBeReq = FALSE;				/* or サーボオフ遅れ時間がゼロ				*/
			BkSeqData->var.Brake = TRUE;				/* ベースイネーブル要求オフと				*/
			BkSeqData->var.BkStep = 1;					/* ブレーキ指令を同時に出力					*/
		}
		else											/* アラーム：無し or ゼロ速停止				*/
		{												/* and サーボオフ待ち時間 > 0				*/
			BkSeqData->var.Brake = TRUE;				/* ブレーキ指令を出力し、					*/
			if( CtrlMode.b.cm == CTRLMODE_TRQ )			/* トルク制御時は、							*/
			{											/* ベースブロックを待つ間、制御継続			*/
				BkSeqData->var.BkZeroStop = FALSE;
			}
			else
			{											/* ベースブロックを待つ間、ゼロ速停止		*/
				BkSeqData->var.BkZeroStop = TRUE;
			}
			BkSeqData->var.BkStep = 5;			/* <S1F5> */
		}
		break;
/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
	case 3: /* サーボオン中で、サーボオフまたはサーボレディオフを待っている状態	<S1F5>					*/
/*--------------------------------------------------------------------------------------------------*/
		if( BeSeq->SvonReq && BeSeq->SvReadyPhaseIgnore )
		{ /* サーボオン要求 */
			;											/* サーボオン要求オフ待ち					*/
		}
		else if( (AlmStop->var.Gr2LinDecStop == TRUE)
			|| (ForceStop->var.FstpLinDecStop == TRUE))
		{ /* 強制停止直線減速停止中 */
			;										/* アラームと強制停止：直線減速停止中オフ待ち	*/
		}
		else if( (AlmStop->var.Gr2ZeroStop == TRUE)
			|| (ForceStop->var.FstpZeroStop == TRUE) )
		{
			BkSeqData->var.BkStep = 2;
		}
		else if( AbsMotSpd >= BkSeqData->conf.BkonSpdLevel )
		{ /* モータ速度がブレーキ指令以上 */			/* 出力速度(Pn507)以上なら					*/
			BkSeqData->var.BkBeReq = FALSE;				/* ブレーキをかけずに						*/
			BkSeqData->var.BkStep = 4;					/* ベースイネーブル要求オフ					*/
		}
		else if( (AlmStop->var.AlmStopMode >= STOPMODE_DBSTOPDB)
				|| (ForceStop->var.FstpMode >= STOPMODE_DBSTOPDB)
				|| (BkSeqData->conf.BkonDelayTime == 0)
				|| (HwbbStatus == TRUE) )
		{												/* ゼロ速停止以外のアラーム or HWBB状態		*/
			BkSeqData->var.BkBeReq = FALSE;				/* or サーボオフ遅れ時間がゼロ				*/
			BkSeqData->var.Brake = TRUE;				/* ベースイネーブル要求オフと				*/
			BkSeqData->var.BkStep = 1;					/* ブレーキ指令を同時に出力					*/
		}
		else											/* アラーム：無し or ゼロ速停止				*/
		{												/* and サーボオフ待ち時間 > 0				*/
			BkSeqData->var.Brake = TRUE;				/* ブレーキ指令を出力し、					*/
			if( CtrlMode.b.cm == CTRLMODE_TRQ )			/* トルク制御時は、							*/
			{											/* ベースブロックを待つ間、制御継続			*/
				BkSeqData->var.BkLinDecStop = FALSE;
			}
			else
			{										/* ベースブロックを待つ間、直線減速停止停止		*/
				BkSeqData->var.BkLinDecStop = TRUE;
			}
			BkSeqData->var.BkStep = 5;
		}
		break;
/* <S1F5> End */
/*--------------------------------------------------------------------------------------------------*/
	case 4:	/* モータ運転中のサーボオフで、モータ速度が低くなるのを待っている状態					*/   /*  */
/*--------------------------------------------------------------------------------------------------*/
		BkSeqData->var.BkWait += KPI_SCCYCLEMS;
		if( (BkSeqData->var.BkWait >= BkSeqData->conf.BkonWaitTime)
			|| (AbsMotSpd <= BkSeqData->conf.BkonSpdLevel) )
		{												/* ブレーキ指令待ち時間経過 or				*/
			BkSeqData->var.Brake = TRUE;				/* モータ速度がブレーキ指令出力速度			*/
			BkSeqData->var.BkWait = 0;					/* 以下になったら							*/
			BkSeqData->var.BkStep = 1;					/* ブレーキ指令を出力する					*/
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5: /* モータ停止中のサーボオフで、ブレーキがかかるまで、ＢＢを待っている状態				*/   /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
		BkSeqData->var.BkWait += KPI_SCCYCLEMS;
		if( (BkSeqData->var.BkWait >= BkSeqData->conf.BkonDelayTime)
//			|| (alm_stop_ptr->var.AlmStopMode > STOPMODE_ZEROSTOP)   /* <S1F5> */
			|| (HwbbStatus == TRUE) )
		{												/* サーボオフ遅れ時間経過					*/
			BkSeqData->var.BkBeReq = FALSE;				/* ゼロ速停止以外のアラーム発生で			*/
			BkSeqData->var.BkZeroStop = FALSE;			/* ベースイネーブル要求オフ					*/
			BkSeqData->var.BkWait = 0;					/* and ゼロストップ要求オフ					*/
			BkSeqData->var.BkStep = 1;					/*											*/
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default: /*	本ケースはシーケンスソフト異常														*/
/*--------------------------------------------------------------------------------------------------*/
		KpiSystemError( );
		break;
	}

	return BkSeqData->var.BkBeReq;
}



/****************************************************************************************************/
/*																									*/
/*		オーバートラベル状態検出処理																*/
/*																									*/
/****************************************************************************************************/
BOOL BeSeqDetectOTStatus( OT_SEQ_DATA *OverTravelSeq, BASE_CTRL_OUT *BaseCtrlOut, BOOL PotSig, BOOL NotSig )
{
	CTRLMD	*CtrlMode;
	LONG	lwk, SpdRef;

	CtrlMode = &BaseCtrlOut->CtrlModeOut;

/*--------------------------------------------------------------------------------------------------*/
/*		OT検出許可判定処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	/*	テーブル運転がゼロ速・ゼロクランプ中はOT検出しない。										*/
	if( BaseCtrlOut->TblZclamp == TRUE )
	{
		/* OT検出禁止 */
		OverTravelSeq->var.OverTrvlSts = FALSE;
		return FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		OT検出処理																					*/
/*--------------------------------------------------------------------------------------------------*/
	switch( CtrlMode->b.cm )
	{
/*--------------------------------------------------------------------------------------------------*/
	case CTRLMODE_SPD:	/* 速度制御モード 															*/
/*--------------------------------------------------------------------------------------------------*/
		/* 速度指令取得 */
		SpdRef = BaseCtrlOut->OutputSpdRef;

		if( (SpdRef >= 0) && PotSig )
		{ /* 速度指令:正側 && POT状態 */
			OverTravelSeq->var.OverTrvlSts = TRUE;
		}
		else if( (SpdRef <= 0) && NotSig )
		{ /* 速度指令:負側 && NOT状態 */
			OverTravelSeq->var.OverTrvlSts = TRUE;
		}
		else
		{ /* OT状態なし */
			OverTravelSeq->var.OverTrvlSts = FALSE;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case CTRLMODE_POS:	/* 位置制御モード															*/
/*--------------------------------------------------------------------------------------------------*/
		lwk = BaseCtrlOut->PositionError;
		if( lwk == 0 )
		{ /* 位置偏差なし */
			if( PotSig || NotSig )
			{ /* POT状態 or NOT状態 */
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else
			{ /* OT状態なし */
				OverTravelSeq->var.OverTrvlSts = FALSE;
			}
		}
		else
		{ /* 位置偏差あり */

			SpdRef = BaseCtrlOut->OutputSpdRef;
			if( (((lwk > 0) || (SpdRef >= 0)) && PotSig) )
			{ /* (位置偏差:正側 || 速度指令:正側) && POT状態 */
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else if( (((lwk < 0) || (SpdRef <= 0)) && NotSig) )
			{ /* (位置偏差:負側 || 速度指令:負側) && NOT状態 */
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else
			{ /* OT状態なし */
				OverTravelSeq->var.OverTrvlSts = FALSE;
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case CTRLMODE_TRQ:	/* トルク制御制御モード														*/
/*--------------------------------------------------------------------------------------------------*/
		lwk = BaseCtrlOut->TrqCtrlRef;
		if( ((lwk >= 0) && PotSig) || ((lwk <= 0) && NotSig) )
		{ /* POT状態 or NOT状態 */
			OverTravelSeq->var.OtOffCnt = 0;
			if( OverTravelSeq->var.OtOnCnt >= KPI_SCANC_MS( 10 ) )
			{
				OverTravelSeq->var.OverTrvlSts = TRUE;
			}
			else
			{
				OverTravelSeq->var.OtOnCnt++;
//				OverTravelSeq->var.OverTrvlSts = FALSE; /* <S0B7> */
			}
		}
		else
		{
			OverTravelSeq->var.OtOnCnt = 0;
			if( OverTravelSeq->var.OtOffCnt >= KPI_SCANC_MS( 10 ) )
			{
				OverTravelSeq->var.OverTrvlSts = FALSE;
			}
			else
			{
				OverTravelSeq->var.OtOffCnt++;
//				OverTravelSeq->var.OverTrvlSts = TRUE; /* <S0B7> */
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:	/* JOGモード,etc																	*/
/*--------------------------------------------------------------------------------------------------*/
		OverTravelSeq->var.OverTrvlSts = FALSE;
		break;
	}
	return OverTravelSeq->var.OverTrvlSts;
}



/****************************************************************************************************/
/*																									*/
/*		オーバートラベルシーケンス(OTシーケンス)													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : マシンのストロークエンドを越えた場合の非常停止処理を行う。								*/
/*			停止モードはユーザー定数の設定により3通り選択できる。									*/
/*																									*/
/*	戻り値：OTシーケンスからのベースイネーブル要求													*/
/*																									*/
/****************************************************************************************************/
// BOOL BeSeqOverTravelSequence( OT_SEQ_DATA *OverTravelSeq, LONG CtrlMode, BOOL MotStop, BOOL BeComplete )  /* <S1F5> */
BOOL BeSeqOverTravelSequence( OT_SEQ_DATA *OverTravelSeq, LONG CtrlMode, BOOL MotStop,     /* <S1F5> */
	                                                      BOOL BeComplete, BOOL LdstpTimeZero )    /* <S1F5> */
{
	LONG	CtrlMode2Lst;
	LONG	OtStopMode;
/*--------------------------------------------------------------------------------------------------*/
/*		OTシーケンスNetWork I/F用制御モード前回値保存												*/
/*--------------------------------------------------------------------------------------------------*/
	OtStopMode = OverTravelSeq->conf.OtStopMode;
	CtrlMode2Lst = OverTravelSeq->var.CtrlMode2;
	OverTravelSeq->var.CtrlMode2 = CtrlMode;

/*--------------------------------------------------------------------------------------------------*/
/*		OTシーケンス処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( OverTravelSeq->var.OtStep )
	{
/*----------------------------------------------------------------------------------------------*/
	case 0: /* Initial Step																		*/
/*----------------------------------------------------------------------------------------------*/
		OverTravelSeq->var.OtBeReq = TRUE;
		OverTravelSeq->var.OtTrqLimit = FALSE;
		OverTravelSeq->var.OtZeroStop = FALSE;
		OverTravelSeq->var.OtLinDecStop = FALSE;   /* <S1F5> */
		OverTravelSeq->var.OtZeroClamp = FALSE;
		OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
		OverTravelSeq->var.OtWait = 0;
		OverTravelSeq->var.OtStep = 1;
		break;
/*----------------------------------------------------------------------------------------------*/
	case 1: /* OT監視 (OTでない状態)															*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == TRUE )	/* OTチェック							*/
		{ /* OT状態 */
			/* 2011.04.04 Y.Oka <V680> */
			if( (BeComplete == TRUE) && (OverTravelSeq->conf.OtWarningSw == TRUE) )
			{ /* BaseEnable && OTワーニング検出あり */
				OverTravelSeq->var.OtWarningDet = TRUE;
				OverTravelSeq->var.OtWarngWaitCnt = 0;
			}
		/*--------------------------------------------------------------------------------------*/
			/* トルク制御時のOT停止方法をDB停止にする											*/
			if( CtrlMode == CTRLMODE_TRQ )
			{ /* トルク制御モード */
				OverTravelSeq->var.OtBeReq = FALSE;		/* トルク制御モードでのOT停止はDB停止	*/
				OverTravelSeq->var.OtDbopReq = OTDBOP_DEF;
				OverTravelSeq->var.OtStep = 2;
				break;
			}
		/*--------------------------------------------------------------------------------------*/
			switch( OtStopMode )						/* OT停止モードによる分岐				*/
			{
			case OTSTOPMODE_DEFAULT:					/* 標準停止モード(Pn001.0)				*/
				OverTravelSeq->var.OtBeReq = FALSE;
				OverTravelSeq->var.OtDbopReq = OTDBOP_DEF;
				OverTravelSeq->var.OtStep = 2;
				break;
			case OTSTOPMODE_ZSTOPZC:					/* ゼロ速停止＆サーボロック				*/
				OverTravelSeq->var.OtTrqLimit = TRUE;
				OverTravelSeq->var.OtZeroStop = TRUE;
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 3;
				break;
			case OTSTOPMODE_ZSTOPFR:					/* ゼロ速停止＆フリーラン				*/
				OverTravelSeq->var.OtTrqLimit = TRUE;
				OverTravelSeq->var.OtZeroStop = TRUE;
/* <S1F5> Start */
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 5;
				break;
			case OTSTOPMODE_LDSTOPZC:					/* 直線減速停止＆サーボロック			*/
				if( LdstpTimeZero == TRUE )
				{	/* Pn30A = 0とした場合、ゼロ速停止となる。ただし、Pn406によるトルク制限は有効。	*/
					OverTravelSeq->var.OtTrqLimit = TRUE;
					OverTravelSeq->var.OtZeroStop = TRUE;
				}
				else
				{
					OverTravelSeq->var.OtLinDecStop = TRUE;
				}
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 3;
				break;
			case OTSTOPMODE_LDSTOPFR:					/* 直線減速停止＆フリーラン				*/
				if( LdstpTimeZero == TRUE )
				{	/* Pn30A = 0とした場合、ゼロ速停止となる。ただし、Pn406によるトルク制限は有効。	*/
					OverTravelSeq->var.OtTrqLimit = TRUE;
					OverTravelSeq->var.OtZeroStop = TRUE;
				}
				else
				{
					OverTravelSeq->var.OtLinDecStop = TRUE;
				}
/* <S1F5> End */
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
				OverTravelSeq->var.OtStep = 5;
				break;
			default:
				 break;
			}
		}
		/* 2011.04.04 Y.Oka <V680> */
		else
		{ /* OT状態ではない場合 */
			if( OverTravelSeq->var.OtWarngWaitCnt > KPI_SCANC_MS(1000) )
			{/* OTワーニングを検出したら、1秒間保持してクリア */
				OverTravelSeq->var.OtWarningDet = FALSE;
			}
			else
			{
				OverTravelSeq->var.OtWarngWaitCnt++;
			}
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 2: /* 標準停止方法で停止中、OT解除待ち／モータ停止待ち									*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT解除チェック					*/
		{
			OverTravelSeq->var.OtWait++;
			if( OverTravelSeq->var.OtWait >= KPI_SCANC_MS( 20 ) )
			{ /* OTチャタリング防止 */
				OverTravelSeq->var.OtBeReq = TRUE;			/* ベースイネーブル要求オン			*/
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;	/* DB操作要求解除					*/
				OverTravelSeq->var.OtWait = 0;
				OverTravelSeq->var.OtStep = 1;
			}
		}
//	<2010.11.17>制御モード変更	
//		else if((( OverTravelSeq->var.CtrlMode2 == POS_CTRL_MODE ) || ( OverTravelSeq->var.CtrlMode2 == SPD_CTRL_MODE ))
//				&& ( CtrlMode2Lst == NO_CTRL_MODE ) 
//				&& ( OtStopMode == OTSTOPMODE_ZSTOPZC ))
		else if((CtrlMode != CTRLMODE_TRQ)
				&& ( CtrlMode2Lst == CTRLMODE_TRQ ) 
				&& ( OtStopMode == OTSTOPMODE_ZSTOPZC ))
		{													/* ゼロ速停止＆サーボロック			*/
			OverTravelSeq->var.OtBeReq = TRUE;				/* ベースイネーブル要求オン			*/
			OverTravelSeq->var.OtTrqLimit = TRUE;
			OverTravelSeq->var.OtZeroStop = TRUE;
			OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;
			OverTravelSeq->var.OtStep = 3;
		}
		else
		{
			OverTravelSeq->var.OtWait = 0;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 3: /* ゼロ速停止中、OT解除待ち／モータ停止待ち											*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT解除チェック					*/
		{
			OverTravelSeq->var.OtTrqLimit = FALSE;			/* OTが解除で、STEP-1に遷移			*/
			OverTravelSeq->var.OtZeroStop = FALSE;
			OverTravelSeq->var.OtLinDecStop = FALSE;   /* <S1F5> */
			OverTravelSeq->var.OtStep = 1;
		}
		else if( MotStop )									/* モータ停止チェック				*/
		{
			OverTravelSeq->var.OtZeroStop = FALSE;			/* モータ停止で、STEP-5に遷移		*/
			OverTravelSeq->var.OtLinDecStop = FALSE;    /* <S1F5> */
			OverTravelSeq->var.OtZeroClamp = TRUE;
			OverTravelSeq->var.OtStep = 4;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 4: /* ゼロ速停止後、ゼロクランプモードで、OT解除待ち									*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT解除チェック					*/
		{
			OverTravelSeq->var.OtTrqLimit = FALSE;			/* OT時トルク制限解除				*/
			OverTravelSeq->var.OtZeroClamp = FALSE;			/* OT時ゼロクランプ解除				*/
			OverTravelSeq->var.OtStep = 1;					/* STEP-1に遷移						*/
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 5: /* ゼロ速停止中、OT解除待ち／モータ停止待ち											*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT解除チェック					*/
		{
			OverTravelSeq->var.OtTrqLimit = FALSE;			/* OTが解除で、STEP-1に遷移			*/
			OverTravelSeq->var.OtZeroStop = FALSE;
			OverTravelSeq->var.OtLinDecStop = FALSE;     /* <S1F5> */
			OverTravelSeq->var.OtStep = 1;
		}
		else if( MotStop )									/* モータ停止チェック				*/
		{
			OverTravelSeq->var.OtBeReq = FALSE;				/* モータ停止で、STEP-5に遷移		*/
			OverTravelSeq->var.OtTrqLimit = FALSE;
			OverTravelSeq->var.OtZeroStop = FALSE;
			OverTravelSeq->var.OtLinDecStop = FALSE;    /* <S1F5> */
			OverTravelSeq->var.OtDbopReq = OTDBOP_FREE;
			OverTravelSeq->var.OtStep = 6;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	case 6: /* ゼロ速停止後、フリーモードで、OT解除待ち											*/
/*----------------------------------------------------------------------------------------------*/
		if( OverTravelSeq->var.OverTrvlSts == FALSE )		/* OT解除チェック					*/
		{
			/* OTチャタリング防止 */
			OverTravelSeq->var.OtWait++;
			if( OverTravelSeq->var.OtWait >= KPI_SCANC_MS( 20 ) )
			{												/* 指令の極性をOTの条件にしている	*/
				OverTravelSeq->var.OtBeReq = TRUE;			/* 指令がゼロ近傍の時、OTがチャタ	*/
				OverTravelSeq->var.OtDbopReq = OTDBOP_NONE;	/* リングを起こすので、OTオフに		*/
				OverTravelSeq->var.OtWait = 0;				/* フィルタをかける。				*/
				OverTravelSeq->var.OtStep = 1;
			}
		}
		else
		{
			OverTravelSeq->var.OtWait = 0;
		}
		break;
/*----------------------------------------------------------------------------------------------*/
	default: /*	本ケースはシーケンスソフト異常													*/
/*----------------------------------------------------------------------------------------------*/
		KpiSystemError( );
		break;
	}
	
	return OverTravelSeq->var.OtBeReq;
}



/****************************************************************************************************/
/*																									*/
/*		サーボストップモード指令																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : カーネルアラーム監視および各シーケンス処理からのストップ要求を受けて、優先度			*/
/*			に基づきストップモードを決定し、ＤＢシーケンスおよびスキャンＣ制御モード指令			*/
/*			処理にストップモードを指令する。														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
LONG BeSeqStopModeManager( BE_SEQ_HNDL *BeSeq)
{
	LONG	StopMode;
	BOOL 	ZeroStopReq;
	BOOL 	LinDecStopReq;    /*  */
	LONG 	OtDbopReq;
	LONG	Gr2DbopReq;       /* <S1F5> */
	LONG	FstpDbopReq;    /* <S1F5> */

	StopMode = BeSeq->StopMode;
	OtDbopReq = BeSeq->OtSeqData.var.OtDbopReq;
	Gr2DbopReq = BeSeq->AlmStop.var.Gr2DbopReq;			/* <S1F5> */
	FstpDbopReq = BeSeq->ForceStop.var.FstpDbopReq;		/* <S1F5> */
//	ZeroStopReq = BeSeq->BkSeqData.var.BkZeroStop     /* <S1F5> */
//				| BeSeq->OtSeqData.var.OtZeroStop | BeSeq->ZcCtrlData.ZcZeroStop;   /* <S1F5> */
	ZeroStopReq = BeSeq->BkSeqData.var.BkZeroStop | BeSeq->AlmStop.var.Gr2ZeroStop    /* <S1F5> */
				| BeSeq->ForceStop.var.FstpZeroStop | BeSeq->OtSeqData.var.OtZeroStop   /* <S1F5> */
				| BeSeq->ZcCtrlData.ZcZeroStop;    /* <S1F5> */

	LinDecStopReq = BeSeq->AlmStop.var.Gr2LinDecStop | BeSeq->BkSeqData.var.BkLinDecStop	/* <S1F5> */
				| BeSeq->ForceStop.var.FstpLinDecStop | BeSeq->OtSeqData.var.OtLinDecStop;   /* <S1F5> */
//	if( BeSeq->AlmStop.var.AlmStopMode > STOPMODE_ZEROSTOP )	/* アラーム：非常停止要求時					*/   /* <S1F5> */
//	{          /* <S1F5> */
//		StopMode = BeSeq->AlmStop.var.AlmStopMode;			/* ゼロ速停止以外の停止モード(DB,etc)		*/   /* <S1F5> */
//	}    /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
//	else if( BeSeq->BkSeqData.var.BkBeReq == FALSE )				/* ベースイネーブル要求オフ時				*/	/* <S1F5> */
	if( (BeSeq->AlmStop.var.AlmStopMode >= STOPMODE_DBSTOPDB)	/* アラーム：DB停止/フリーラン停止	*/	/* <S1F5> */
	 || (BeSeq->ForceStop.var.FstpMode >= STOPMODE_DBSTOPDB))	/* 強制停止：DB停止/フリーラン停止	*/	/* <S1F5> */
	{
		StopMode = BeSeq->AlmStop.conf.StopMode;				/* サーボオフ時の停止モード					*/
	}
	else if(( BeSeq->BkSeqData.var.BkBeReq == FALSE )				/* ベースイネーブル要求オフ時		*/	/* <S1F5> */
		 &&(BeSeq->AlmStop.var.AlmStopMode == STOPMODE_NOTSTOP )	/* アラーム：非停止					*/	/* <S1F5> */
		 &&(BeSeq->ForceStop.var.FstpMode == STOPMODE_NOTSTOP ))	/* 強制停止：非停止					*/	/* <S1F5> */
	{
		StopMode = BeSeq->AlmStop.conf.StopMode;				/* サーボオフ時の停止モード					*/
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( (BeSeq->OtSeqData.var.OtBeReq == FALSE) && (OtDbopReq == OTDBOP_DEF) )
	{ /* OT時標準停止方法 */
		if( BeSeq->AlmStop.conf.StopMode == STOPMODE_DBSTOPDB )
		{ /* DBでモータ停止＆DB継続(Pn001.0==0) */
			/* DBでモータ停止＆DB解除(Pn001.0==1) */
			StopMode = STOPMODE_DBSTOPFR;
		}
		else
		{
			/* サーボオフ時の停止モード */
			StopMode = BeSeq->AlmStop.conf.StopMode;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( ((BeSeq->OtSeqData.var.OtBeReq == FALSE) && (OtDbopReq == OTDBOP_FREE) )  /* <S1F5> */
		  || ((BeSeq->BkSeqData.var.BkBeReq == FALSE) && (Gr2DbopReq == GR2DBOP_FREE))   /* <S1F5> */
		  || ((BeSeq->BkSeqData.var.BkBeReq == FALSE) && (FstpDbopReq == FSTPDBOP_FREE)) )    /* <S1F5> */
	{													/* OT時フリーラン停止						*/
		StopMode = STOPMODE_FREERUN;					/* フリーラン停止モード						*/
	}
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1F5> Start	*/
	else if( BeSeq->AlmStop.var.AlmZeroStop )					/* アラーム：ゼロ速停止要求時				*/
	{
		StopMode = BeSeq->AlmStop.var.AlmStopMode;			/* ゼロ速停止モード							*/
	}
#endif	/* <S1F5> End	*/
/*--------------------------------------------------------------------------------------------------*/
	else if( ZeroStopReq )			/* ALM, FSTP, BK, OT, ZCシーケンス：ゼロ速停止要求時			*/   /* <S1F5> */
	{
		StopMode = STOPMODE_ZEROSTOP;					/* ゼロ速停止モード							*/
	}
/* <S1F5> Start */
/*--------------------------------------------------------------------------------------------------*/
	else if( LinDecStopReq )		/* ALM, FSTP, OTシーケンス：直線減速度による減速停止停止要求時	*/
	{
		StopMode = STOPMODE_LDECSTOP;					/* 直線減速度による減速停止モード			*/
	}
/* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
	else												/* ベースイネーブル要求オン時				*/
	{
		StopMode = STOPMODE_NOTSTOP;					/* 停止モード解除							*/
	}
/*--------------------------------------------------------------------------------------------------*/
	BeSeq->StopMode = StopMode;
	return StopMode;
}



/****************************************************************************************************/
/*																									*/
/*		ダイナミックブレーキシーケンス(DBシーケンス)												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : OTシーケンスとBKシーケンスからのベースイネーブル要求と、ストップモード指令				*/
/*			処理からのストップモード指令を受けて、サーボのベースイネーブル／ベースブロック			*/
/*			およびＤＢオン／オフのタイミングの制御を行う。											*/
/*																									*/
/*			停止モードはユーザー定数(Pn001.0)により，３種類の中から選べる。							*/
/*				Pn001.1=0 : DB停止																	*/
/*				Pn001.1=1 : DB停止、モータ停止後DBオフでモータフリーラン							*/
/*				Pn001.1=2 : DBなし、フリーラン停止													*/
/*																									*/
/*	補 足 : ASIC_PUMP_ON中OC発生時処理、また、ベースイネーブル要求とのインタロックをとる→(STATE5)	*/
/*			DBサイリスタ仕様の場合はモータ停止の前にDBオフ											*/
/*			サイリスタ使用の場合を考慮してsysf.dbonとは別にJL034_000.5(DBON)を直接設定する			*/
/*			サイリスタによるDBではモータ回転中１度サイリスタがオンすれば、DBONをオフしてもDBは		*/
/*			働き続ける。モータが止まってからオフしているとサイリスタがオフする前にモータが			*/
/*			また動いたら、サイリスタが切れなくなる危険がある。よって、サイリスタによる				*/
/*			DBではDBON後次のサンプリング(2ms後）にオフする。										*/
/*			OT時にサーボオフされた場合、サーボオフ時の停止方法に従うよう修正						*/
/*			OT中のサーボオフして再度サーボオンされた場合、OTの停止モード(フリー)にする				*/
/*																									*/
/****************************************************************************************************/
BOOL BeSeqDynamicBrakeSequence( BE_SEQ_HNDL *BeSeq, ASICS *SvAsicRegs, LONG MotStop, BOOL EncDataNG,
								BOOL MotorLessTestMode )											/* <S1B7> */
{
	BOOL				DbOnRequest;
	DB_SEQ_DATA 		*DbSeqData;
	PUMP_CTR_STR 		*PumpOnCtrl;
	/* ローカル変数の初期化 */
	DbSeqData = &BeSeq->DbSeqData;
	PumpOnCtrl = &BeSeq->ChargePump;
	DbOnRequest = BeSeq->DbOn;

/*--------------------------------------------------------------------------------------------------*/
/*		ベースイネーブル要求の作成																	*/
/*--------------------------------------------------------------------------------------------------*/
	DbSeqData->DbBeReq = BeSeq->BkSeqData.var.BkBeReq & BeSeq->OtSeqData.var.OtBeReq;

/*--------------------------------------------------------------------------------------------------*/
/*		DB＆BEシーケンス																			*/
/*--------------------------------------------------------------------------------------------------*/
	switch( DbSeqData->DbStep )
	{
	/*----------------------------------------------------------------------------------------------*/
		case 0: /* Initial Step																		*/
	/*----------------------------------------------------------------------------------------------*/
			/* BaseBlock */
			SHalDrv_BaseBlock(SvAsicRegs->AsicHwReg, SvAsicRegs->AsicMicroReg); /* <S0E4> */

#ifdef CSW_CSTR_DB_REVERSE
			/* Enable DB Control <2010.12.02> */
			SHalDrv_EnableDynamicBrake(SvAsicRegs->AsicHwReg);
#endif

			PumpOnCtrl->PumpOnReq = FALSE;
			BeSeq->BeReqSeqOut = FALSE;

			DbSeqData->DbWait = 0;

			if( BeSeq->StopMode == STOPMODE_FREERUN )
			{
				DbOnRequest = FALSE;
				DbSeqData->DbStep = 1;
			}
			/* DB停止・停止後フリー */
			else if( BeSeq->StopMode == STOPMODE_DBSTOPFR )
			{
				if( EncDataNG == TRUE )
				{ /* エンコーダデータNG時はDBオンする */
					DbOnRequest = TRUE;
				}
				else
				{
					DbOnRequest = FALSE;
				}
				DbSeqData->DbStep = 1;
			}
			/* DB停止・停止後継続 */
			else
			{
				DbOnRequest = TRUE;
				DbSeqData->DbStep = 6;
			}
			break;

	/*----------------------------------------------------------------------------------------------*/
		case 1: /* モータ停止待ち : モータ停止後DBをオフする										*/
	/*----------------------------------------------------------------------------------------------*/
		if(( BeSeq->StopMode == STOPMODE_DBSTOPFR )&&( EncDataNG == TRUE ))
											/* エンコーダデータNG中はDBオンしてここに留まる			*/
		{									/* ここに留まるのはイニシャル中にエンコーダデータNGに	*/
											/* なった場合。運転中NGはステップ6に留まる				*/
			DbOnRequest = TRUE;				/* エンコーダデータNG中はDBオンしてここに留まる			*/
			PumpOnCtrl->PumpOnReq = FALSE;	/* エンコーダデータNG中はDBオンしてここに留まる			*/
											/* チャージポンプ駆動要求オフ -> LpxChargePumpControl	*/
		}									/* エンコーダデータNG中はDBオンしてここに留まる			*/
		else if( MotStop )					/* エンコーダデータが信用出来る時のみモータ停止チェック	*/
		{
			DbOnRequest = FALSE;
			PumpOnCtrl->PumpOnReq = FALSE;	/* チャージポンプ駆動要求オフ -> LpxChargePumpControl 	*/

			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 2;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 2: /* ベースイネーブル要求待ち															*/
	/*				停止方法＝DBなしまたはDB後フリーでDBをオフしているときこの状態					*/
	/*----------------------------------------------------------------------------------------------*/
		/* StopModeが変化(BK.SeqとOT.Seqの処理により)しDB停止、DB継続するステート変更 */
		if( BeSeq->StopMode == STOPMODE_DBSTOPDB )
		{
			DbOnRequest = TRUE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 6;
		}
		else
		{
			/* ベースイネーブルをチェック */
			if( DbSeqData->DbBeReq )
			{
				/* チャージポンプ駆動要求オン */
				PumpOnCtrl->PumpOnReq = TRUE;
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 3;
			}
			/* ベースイネーブルリクエスト待ち中にエンコーダデータNGとなった場合はシーケンスを1に	*/
			else if(( BeSeq->StopMode == STOPMODE_DBSTOPFR )&&( EncDataNG == TRUE ))
			{
				DbSeqData->DbStep = 1;
			}
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 3: /* チャージポンプの制御																*/
	/* ベースイネーブル要求が継続しているかチェックしながらチャージポンプを制御する					*/
	/*----------------------------------------------------------------------------------------------*/
		/* ベースイネーブル要求途絶処理 */
		if( DbSeqData->DbBeReq == FALSE )
		{
			PumpOnCtrl->PumpOnReq = FALSE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 0;
		}
		/* ベースイネーブル要求継続中処理 Wait32 = DBリレーオフ->PumpOnまでの待ち(16)+PumpOn時間(16) */
		else if( PumpOnCtrl->PumpOnComp == TRUE )						/*	チャージポンプ制御完了	*/
		{
			PumpOnCtrl->PumpOnReq = FALSE;

			SHalDrv_ResetAlarmLtStatus(SvAsicRegs->AsicHwReg);

			/* モータレステストモードでないときベースイネーブルする *//* <S1B7> */
			if( MotorLessTestMode == FALSE )
			{
				SHalDrv_BaseEnable(SvAsicRegs->AsicHwReg, SvAsicRegs->AsicMicroReg);
			}

			BeSeq->BeReqSeqOut = TRUE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 4;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 4: /* ベースブロック要求待ち															*/
	/* （ASICへ)ベースイネーブル指令中はここにいる													*/
	/*----------------------------------------------------------------------------------------------*/
		/* ベースイネーブル要求途絶処理 */
		if( DbSeqData->DbBeReq == FALSE )
		{
			/* ベースブロック中は電流検出値が飽和するので、IFBゲインを０にする	*/
			/* PUMPONするとき、電流指令とか電流FBにデータがあるとPWMの			*/
			/* デューティーが50%でなくなり、うまく充電できない					*/
			/* BaseBlock */
			SHalDrv_BaseBlock(SvAsicRegs->AsicHwReg, SvAsicRegs->AsicMicroReg); /* <S0E4> */

			BeSeq->BeReqSeqOut = FALSE;

			/*--------------------------------------------------------------*/
			/*			停止モードの判定									*/
			/*--------------------------------------------------------------*/
			/* フリーラン停止 */
			if( BeSeq->StopMode == STOPMODE_FREERUN )
			{
				DbOnRequest = FALSE;
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 2;
			}
			/* ＤＢ停止・停止後フリー */
			else if( BeSeq->StopMode == STOPMODE_DBSTOPFR )
			{
				if( EncDataNG == TRUE )
				{ /* エンコーダデータNGの場合 */
					/*  エンコーダデータNG時はSTEP1へ遷移 */
					DbSeqData->DbWait = 0;
					DbSeqData->DbStep = 5;
				}
				/* 停止モードがDB停止後フリーの場合でモータが止まっているときdbonしない。*/
				/* 1サンプリング待たないのはOT時指令0近傍でリレーがチャタることの対策 */
				else if( MotStop )
				{
					/* A.C90(エンコーダ通信異常)発生時は1秒間DBをオンする */
					DbOnRequest = FALSE;
					DbSeqData->DbWait = 0;
					DbSeqData->DbStep = 1;
				}
				else
				{
					DbSeqData->DbWait = 0;
					DbSeqData->DbStep = 5;
				}
			}
			/* ＤＢ停止・停止後継続 */
			else
			{
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 5;
			}
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 5: /* ベースブロック要求によるＤＢオン処理												*/
	/* ベースブロック要求から１サイクル遅らせてＤＢをオンする										*/
	/* ※フリーラン停止の場合はここへは来ない														*/
	/*----------------------------------------------------------------------------------------------*/
		/*--------------------------------------------------------------*/
		/*			停止モードの判定									*/
		/*--------------------------------------------------------------*/
		/* ＤＢ停止・停止後フリー */
		if( BeSeq->StopMode == STOPMODE_DBSTOPFR )
		{
			if( EncDataNG == TRUE )
			{ /* エンコーダデータNGの場合 */
				DbOnRequest = TRUE;				/* エンコーダデータNG時はDBオンしてここに留まる		*/
			}
			else if( MotStop )
			{ /* エンコーダデータが信用出来るときは停止をチェック	*/
				/* A.C90(エンコーダ通信異常)発生時はDBをオンする */
				DbOnRequest = FALSE;
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 1;
			}
			else
			{
				DbOnRequest = TRUE;				/* モータ回転中はＤＢオンしステート１で停止を待つ 	*/
				DbSeqData->DbWait = 0;
				DbSeqData->DbStep = 1;
			}
		}
		/* StopModeが変化(BK.SeqとOT.Seqの処理により)しStopModeが新たにフリーに変化した場合  */
		else if( BeSeq->StopMode == STOPMODE_FREERUN )
		{
			DbOnRequest = FALSE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 1;
		}
		/* ＤＢ停止・停止後継続 */
		else
		{
			DbOnRequest = TRUE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 6;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		case 6: /* ＤＢ停止・停止後継続の場合のベースブロック時処理									*/
	/* モータ停止中かつベースイネーブル要求が来たらＤＢをオフする									*/
	/*----------------------------------------------------------------------------------------------*/
		if(((( EncDataNG == FALSE ) && MotStop ) && DbSeqData->DbBeReq )
		   || ( BeSeq->StopMode == STOPMODE_FREERUN )	/* 又は、DB停止解除→OTフリーラン停止有り	 */
		   || ( BeSeq->StopMode == STOPMODE_DBSTOPFR ))	/* 又は、DB停止解除→OT標準停止（DB停止後フリーラン）*/
		{
			DbOnRequest = FALSE;
			DbSeqData->DbWait = 0;
			DbSeqData->DbStep = 2;
		}
		break;

	/*----------------------------------------------------------------------------------------------*/
		default: /*	本ケースはソフト異常															*/
	/*----------------------------------------------------------------------------------------------*/
		KpiSystemError( );
		break;
	}

	BeSeq->DbOn = DbOnRequest;
	return DbOnRequest;
}

#if (CSW_AUTO_CHARGE_PUMP != FALSE)
/****************************************************************************************************/
/*																									*/
/*	機 能 : チャージポンプのON/OFF制御を行う。														*/
/*																									*/
/*	補 足 : ＰＷＭ制御時(ベースイネーブル時)は、ＡＳＩＣでポンプＯＮ指令が無効とされる				*/
/*			ので、ベースイネーブルとのインターロックは不要											*/
/****************************************************************************************************/
void BeSeqChargePumpControl( ASICS *SvAsicRegs, PUMP_CTR_STR *workptr, BOOL OCState,
								BOOL MotorLessTestMode,BOOL NegPumpReq )/* <S1B7> *//*<S201>*/
{
	ASIC_HW_IF_STR *pAsicHwReg = SvAsicRegs->AsicHwReg;
	ASIC_U_IF_STR *pAsicMicroReg = SvAsicRegs->AsicMicroReg;

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤ零調時																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( workptr->AdjPumponReq == TRUE )					/* 零調ポンプＯＮ要求チェック				*/
	{
		/* モータレステストモードでないときONする	*//* <S1B7> */
		if( MotorLessTestMode == FALSE )
		{
			SHalDrv_AutoChargePumpON(pAsicHwReg);		/* 連続チャージポンプON */
		}
		workptr->SvonPumpCnt++;
		workptr->PumpOnComp = FALSE;					/* チャージポンプ未完						*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		スキャンＣ : Charge Pump Control Request													*/
/*--------------------------------------------------------------------------------------------------*/
	else if( workptr->PumpOnReq == TRUE )				/* スキャンＣポンプＯＮ要求チェック			*/
	{
		if( OCState != FALSE )
		{
			SHalDrv_ChargePumpOFF(pAsicHwReg);
			return;
		}

		switch(workptr->PumpOnSeq)
		{
		case 0:	/* Wait for Relay ON */
			if( KlibGetLongTimerMs(workptr->TimerCnt) > 15 )
			{
				/*------------------------------------------------------------------------------*/
				/* ベースイネーブル前にアラームラッチリセットを行なう 							*/
				/*------------------------------------------------------------------------------*/
				/* KPI_ALARM_CLEAR( );ＡＳＩＣアラームクリア */
				SHalDrv_ResetAlarmLtStatus(pAsicHwReg);
				/*ＡＤ変換デジメーションフィルタのアラームクリア */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);

				workptr->SvonPumpCnt = 0;
				workptr->PumpOnSeq = 1;
				KlibRstLongTimer(&workptr->TimerCnt);

				SHalDrv_StartAutoChargePump(pAsicHwReg);
			}
			break;

		case 1:	/* Charg Pump ON/OFF */
			workptr->SvonPumpCnt++;
			if((0 == SHalDrv_GetAutoChargePumpState(pAsicHwReg))
				|| ( KlibGetLongTimerMs(workptr->TimerCnt) > 16 ))
			{
				/* ＡＤ変換デジメーションフィルタのアラームクリア */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);
				workptr->PumpOnComp = TRUE;			/* チャージポンプ完了				*/
				workptr->PumpOnSeq = 2;
			}

			break;

		default: /* Complete state */
			break;
		}
	}
/*<S201> Start*/
/*--------------------------------------------------------------------------------------------------*/
/*		ScanC : The Charge pump ON/OF Switch Control	                                                                               */	
/*--------------------------------------------------------------------------------------------------*/
	 else if( NegPumpReq == TRUE ) /*SCAN C: N-Arm ON required to checking*/
	{
		if( workptr->NegPumpCnt <=128 )/*Hardware Requirements*/
		 {
			if( ( workptr->NegPumpCnt & 0x0001 ) == 0 )/*Bit computing*/
			 {
			     SHalDrv_ChargePumpOFF(pAsicHwReg);/*ChargePumpOFF*/
			 }
			 else
			 {
			     SHalDrv_ChargePumpON(pAsicHwReg);/*ChargePumpON*/
			 }
			 workptr->NegPumpCnt++;/*Bit computing*/
		 }
		 else
		 {
		      SHalDrv_ChargePumpON(pAsicHwReg);/*KPX_PUMPON( );*/
		 }
    }
 /*<S201> END*/	
/*--------------------------------------------------------------------------------------------------*/
/*		要求なし																					*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		KlibRstLongTimer(&workptr->TimerCnt);
		workptr->PumpOnSeq = 0;
		workptr->PumpOnComp = FALSE;						/* チャージポンプ未完					*/
		SHalDrv_ChargePumpOFF(pAsicHwReg);
		workptr->NegPumpCnt = 0;  /*<S201>*/
	}

}

#else	//(CSW_AUTO_CHARGE_PUMP != FALSE)

/****************************************************************************************************/
/*																									*/
/*	機 能 : チャージポンプのON/OFF制御を行う。														*/
/*																									*/
/*	補 足 : ＰＷＭ制御時(ベースイネーブル時)は、ＡＳＩＣでポンプＯＮ指令が無効とされる				*/
/*			ので、ベースイネーブルとのインターロックは不要											*/
/*	★ScanBからcallされているので注意！																*/
/****************************************************************************************************/
void BeSeqChargePumpControl( ASICS *SvAsicRegs, PUMP_CTR_STR *workptr, BOOL OCState,
								BOOL MotorLessTestMode,BOOL NegPumpReq )/* <S1B7> *//*<S201>*/
{
	ASIC_HW_IF_STR *pAsicHwReg = SvAsicRegs->AsicHwReg;
	ASIC_U_IF_STR *pAsicMicroReg = SvAsicRegs->AsicMicroReg;

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤ零調時																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( workptr->AdjPumponReq == TRUE )					/* 零調ポンプＯＮ要求チェック				*/
	{
		if( ( workptr->SvonPumpCnt & 0x0001 ) == 0 )	/* スキャンＢ周期でＯＮ／ＯＦＦ				*/
		{
			SHalDrv_ChargePumpOFF(pAsicHwReg);			/* チャージポンプOFF						*/
		}
		else
		{
			/* モータレステストモードでないときONする	*//* <S1B7> */
			if( MotorLessTestMode == FALSE )
			{
				SHalDrv_ChargePumpON(pAsicHwReg);		/* チャージポンプON							*/
			}
		}
		workptr->SvonPumpCnt++;
		workptr->PumpOnComp = FALSE;					/* チャージポンプ未完						*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		スキャンＣ : Charge Pump Control Request													*/
/*--------------------------------------------------------------------------------------------------*/
	else if( workptr->PumpOnReq == TRUE )				/* スキャンＣポンプＯＮ要求チェック			*/
	{
		if( OCState != FALSE )
		{
			SHalDrv_ChargePumpOFF(pAsicHwReg);
			return;
		}

		switch(workptr->PumpOnSeq)
		{
		case 0:	/* Wait for Relay ON */
			if( KlibGetLongTimerMs(workptr->TimerCnt) > 15 )
			{
				/*------------------------------------------------------------------------------*/
				/* ベースイネーブル前にアラームラッチリセットを行なう 							*/
				/*------------------------------------------------------------------------------*/
				/* KPI_ALARM_CLEAR( );ＡＳＩＣアラームクリア */
				SHalDrv_ResetAlarmLtStatus(pAsicHwReg);
				/*ＡＤ変換デジメーションフィルタのアラームクリア */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);

				KlibRstLongTimer(&workptr->TimerCnt);
				workptr->SvonPumpCnt = 0;
				workptr->PumpOnSeq = 1;
			}
			break;

		case 1:	/* Charg Pump ON/OFF */
			if( ( workptr->SvonPumpCnt & 0x0001 ) == 0 )
			{
				SHalDrv_ChargePumpOFF(pAsicHwReg);
			}
			else
			{
				/* モータレステストモードでないときONする	*//* <S1B7> */
				if( MotorLessTestMode == FALSE )
				{
					SHalDrv_ChargePumpON(pAsicHwReg);
				}
			}
			workptr->SvonPumpCnt++;
/* <S094>:ChargePump完了時は、PumpOffで必ず終わる様にする。 */
/* KlibGetLongTimerMs関数だと、タイマのリセットタイミングやScanBの処理時間等でPumpOnで終わる可能性がる。	*/
#if 0
			if( KlibGetLongTimerMs(workptr->TimerCnt) > 15 )
			{
#else
			if(workptr->SvonPumpCnt > CHARGE_PUMP_CMP_TIME)/* CHARGE_PUMP_CMP_TIME = 80H :16ms */
			{
#endif
				/* ＡＤ変換デジメーションフィルタのアラームクリア */
				SHalDrv_ResetCrtAdAlarmStatus(pAsicMicroReg);
				workptr->PumpOnComp = TRUE;			/* チャージポンプ完了				*/
				workptr->PumpOnSeq = 2;
			}
			break;

		default: /* Complete state */
			break;
		}
	}
/*<S201> Start*/
/*--------------------------------------------------------------------------------------------------*/
/*		ScanC : The Charge pump ON/OF Switch Control	                                                                               */	
/*--------------------------------------------------------------------------------------------------*/
	 else if( NegPumpReq == TRUE ) /*SCAN C: N-Arm ON required to checking*/
	{
		if( workptr->NegPumpCnt <=128 )/*Hardware Requirements*/
		 {
			if( ( workptr->NegPumpCnt & 0x0001 ) == 0 )/*Bit computing*/
			 {
			     SHalDrv_ChargePumpOFF(pAsicHwReg);/*ChargePumpOFF*/
			 }
			 else
			 {
			     SHalDrv_ChargePumpON(pAsicHwReg);/*ChargePumpON*/
			 }
			 workptr->NegPumpCnt++;/*Bit computing*/
		 }
		 else
		 {
		      SHalDrv_ChargePumpON(pAsicHwReg);/*KPX_PUMPON( );*/
		 }
    }
 /*<S201> END*/	
/*--------------------------------------------------------------------------------------------------*/
/*		要求なし																					*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		KlibRstLongTimer(&workptr->TimerCnt);
		workptr->PumpOnSeq = 0;
		workptr->PumpOnComp = FALSE;						/* チャージポンプ未完					*/
		SHalDrv_ChargePumpOFF(pAsicHwReg);
		workptr->NegPumpCnt = 0;  /*<S201>*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}
#endif //(CSW_AUTO_CHARGE_PUMP != FALSE)


/*<S201>Start*/	
/****************************************************************************************************/
/*																									*/
/*		主回路OFF時のIPM N側アーム制御   要求を出しています											         */
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 :                																			*/
/*			IPMのN側アーム動作を制御する。															*/
/*			制御要求は、スキャンＢでチェックされる。                        							*/
/*																							*/
/****************************************************************************************************/
#define N_PUMP_NONE		0											/* N側アーム制御無効			*/
#define N_PUMP_SUSPEND	1											/* N側アーム制御中断			*/
#define N_PUMP_START	2											/* N側アーム制御開始			*/
/*--------------------------------------------------------------------------------------------------*/
/******************************************************************************************************/
void BeNegativePumpSequence( POWER_STATUS*   CoutK,   ALMMAN_HANDLE*     Alm,
						     DET_HWBB_STS*   DetHwbb, CHECK_MAIN_POWER*  PowerChecker,
						     DB_SEQ_DATA*    SvSeqV,  SEQ_CTRL_OUT* CoutV1,
						     ASIC_HW_IF_STR* AsicHwReg
						                   )
{
/*--------------------------------------------------------------------------------------------------*/
/*		以下いずれかの場合、Ｎ側アームをＯＦＦとする												  */
/*		■AC電源入力中			■AC電源検出不感帯中	■ベースイネーブル中						    */
/*		■アラーム発生中		■HWBB状態		    ■DC入力モード						           */
/*--------------------------------------------------------------------------------------------------*/
	if( (CoutK->Acon)||
		(!CoutK->AconDetectReady)||
	    (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB)==0)||
	    (Alm->Status.AlmFlag) || 
	    (DetHwbb->HwbbState) || 
	    (PowerChecker->conf.Dcbus))
	{
		SvSeqV->NegPumpState = N_PUMP_NONE;/* N側アーム制御無効*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		モータ停止検出、及び、モータ中断速度レベル検出によりN側アームを制御							*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		if( SvSeqV->NegPumpState == N_PUMP_START )
		{  
		    /*中断速度レベルの検出でN側アーム制御中断*/
			SvSeqV->NegPumpState = (CoutV1->MotSuspLvl)? N_PUMP_SUSPEND : N_PUMP_START;
		}
		else
		{
			/*	モータ停止検出でN側アーム制御開始		*/
			SvSeqV->NegPumpState = (CoutV1->MotStop)? N_PUMP_START : N_PUMP_SUSPEND;
		}
    }
	/*--------------------------------------------------------------------------------------------------*/
	/*		N側アームオン要求作成																		*/
	/*--------------------------------------------------------------------------------------------------*/
	CoutK->NegPumpReq = (SvSeqV->NegPumpState == N_PUMP_START)? TRUE : FALSE;
}

/*<S201>End*/	

/****************************************************************************************************/
/*																									*/
/*		ゼロクランプシーケンス(ＺＣシーケンス)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 速度制御(Pn000.1=0)、速度制御⇔ゼロクランプ制御(Pn000.1=A)において、外部信号入力		*/
/*			(/ZCLAMP)が有効となった場合、速度指令のオフセット等によるドリフトを押さえるため、		*/
/*			位置指令＝０による位置ループを組みモータをクランプする機能。							*/
/*																									*/
/*			本シーケンス処理は、ゼロクランプ要求信号とモータ回転状態からゼロ速停止要求および		*/
/*			ゼロクランプ要求を指令するシーケンスである。											*/
/*			モータ回転中にゼロクランプのリクエストが来た場合は、速度指令を０とし、モータ回転		*/
/*			を止めるゼロ速停止後にゼロクランプする。												*/
/*			実際に位置指令０で位置ループを組むのはスキャンＢで行う。								*/
/*																									*/
/*			 1. 原点サーチ中どうかの確認															*/
/*				サーチ中なら何もしない。--> 制御モード管理で処理									*/
/*																									*/
/*			 2. ゼロクランプ要求の確認																*/
/*				入力信号からのゼロクランプ要求信号を検出する。										*/
/*				注）オプション基板の設定についてはΣ-Ⅱを踏襲する。									*/
/*																									*/
/*			 3. モータ停止状態の確認																*/
/*				モータが停止しているかあるいは既にゼロクランプして位置ループを組んでいるかを		*/
/*				確認する。																			*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#define	ZCLAMP_MARGIN	12710						/* 2^24 * (5rpm / 6600rpm) = 12710				*/
/*--------------------------------------------------------------------------------------------------*/
void	CpxZeroClampSequence( 						/* Zero Clamp Sequence 							*/
	ZC_CTRL_DATA *Zcv,								/* Zero Clamp Control							*/
	BASE_CTRL_OUT	*Bout,							/* Base Control Output 							*/
	FUN_CMN_CONTROL *Fctrl,							/* Function Control								*/
	SEQ_CTRL_OUT *Cout)								/* Sequence Output 								*/
{
LONG	lwk;
UINT bZclamp = (Bout->CmdSeqBit & ZEROCLAMP_BIT);
#define LPX_CHECK_SPDCTRLMODE( bout )  (((bout)->CtrlModeOut.b.cm) == CTRLMODE_SPD)
#define LPX_CHECK_RNDSVCTRL( fctrl )    ((fctrl)->FnSvControl)
LONG	SpdRefo = Bout->OutputSpdRef;
LONG	ZcSpdLevel = Zcv->P.ZcSpdLevel;				/* <S00C>										*/
/*--------------------------------------------------------------------------------------------------*/
/*		ゼロクランプ処理要求入力信号チェック														*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*	機 能 : 外部入力信号によるゼロクランプ信号が有効の状態において、ソフトスタート後の				*/
/*			速度指令が、パラメータ(Pn501)で設定されるゼロクランプレベルより小さい場合に				*/
/*			ゼロクランプシーケンス処理を開始する要求フラグを立てる。								*/
/*			ただし、検出速度の近くでのチャタリングを防ぐため最初に５rpmのマージンを加えた			*/
/*			予備速度で検出を行い、そのまま正式な検出レベル以下となった場合にシーケンス要求			*/
/*			フラグを立てる。																		*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
		if( bZclamp && LPX_CHECK_SPDCTRLMODE(Bout) && (! LPX_CHECK_RNDSVCTRL(Fctrl) ) )
		{
			lwk = MlibABS( SpdRefo );
			if( lwk < (ZcSpdLevel + ZCLAMP_MARGIN) )
			{
				if( lwk < ZcSpdLevel )
				{
					Zcv->ZcReqSignal = TRUE;
				}
			}
			else
			{
				Zcv->ZcReqSignal = FALSE;
			}
		}
		else
		{
			Zcv->ZcReqSignal = FALSE;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		ゼロクランプシーケンス																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (Zcv->ZcReqSignal) 						/* ゼロクランプレベル検出、または、					*/
												/* テーブル運転からのゼロクランプ指令(TODO:)		*/
		/* || (TblExeV.TblZmd.b.zm==CTRLMODE_ZCLMP) */ )	
	{
		if( (Cout->MotStop) ||					/* モーター停止中、または、							*/
			(Zcv->ZcZeroClamp))					/* ゼロクランプ中									*/
		{
			Zcv->ZcZeroClamp = TRUE ;			/* ゼロクランプ要求設定 							*/
			Zcv->ZcZeroStop = FALSE ;			/* ゼロ速停止要求解除設定 							*/
		}
		else
		{
			Zcv->ZcZeroClamp = FALSE ;			/* ゼロクランプ要求解除設定 						*/
			Zcv->ZcZeroStop = TRUE ;			/* ゼロ速停止要求設定 								*/
		}
	}
	else
	{
			Zcv->ZcZeroClamp = FALSE ;			/* ゼロクランプ要求解除設定 						*/
			Zcv->ZcZeroStop = FALSE ;			/* ゼロ速停止要求解除設定 							*/
	}
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/* 	<S0A7> Check Servo On Command Error (A.0B0)														*/
/*																									*/
/*	1)SGD7-AP型の場合、補助機能運転後、サーボオン指令でアラームとする。								*/
/*	2)SGD7-M2,M3型の場合、とりあえずSGDV-MDと同じ仕様にする。検出条件は以下参照。					*/
/*																									*/
/*	※参考：A.0B0検出条件																			*/
/*	4)SGDV-AP: 補助機能による運転後、上位からサーボオン指令した。									*/
/*	5)SGDV-M2,M3: 補助機能による運転後、DISCONNECTせずに上位からサーボオン指令した。				*/
/*	6)SGDV-MD: 補助機能による運転後、速度制御/トルク制御に切り替えないで上位からサーボオン指令した。*/
/*																									*/
/****************************************************************************************************/
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE
LONG	LpxCheckServoOnCmdErr( 						/* Check Servo On Cmd Error (A.0B0):AP			*/
	BE_SEQ_HNDL 	*BeSeq, 						/* 												*/
	BOOL			ServoOnReq,						/* Servo On Command								*/
	FUN_CMN_CONTROL *FnCmnCtrl, 					/* 												*/
	CTRLMD 			CtrlMode )						/* Control Mode									*/
{
LONG	err = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	if( FnCmnCtrl->State.LockNormalOpeDrive != FALSE )
	{	/* 通常運転禁止*/
		if( ServoOnReq != FALSE )
		{
			err = TRUE;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (err);
}
#else
LONG	LpxCheckServoOnCmdErr( 						/* Check Servo On Cmd Error (A.0B0):M2M3		*/
	BE_SEQ_HNDL 	*BeSeq, 						/* 												*/
	BOOL			ServoOnReq,						/* Servo On Command								*/
	FUN_CMN_CONTROL *FnCmnCtrl, 					/* 												*/
	CTRLMD 			CtrlMode )						/* Control Mode									*/
{
LONG	err = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	if(FnCmnCtrl->State.LockNormalOpeDrive != FALSE)
	{	/* 通常運転禁止*/
		if( (CtrlMode.us.uwd_l == CTRLMODE_SPD) || (CtrlMode.us.uwd_l == CTRLMODE_TRQ) )
		{
			BeSeq->SenStatus = FALSE;
			FnCmnCtrl->State.LockNormalOpeDrive = FALSE;
		}
		else if( (ServoOnReq != FALSE) && (CtrlMode.us.uwd_l == CTRLMODE_POS) )
		{/* サーボオン指令あり & 位置制御モード指令 */
			err = TRUE;
		}
		else
		{	/* do nothing. */
			;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return (err);
}
#endif


/***************************************** end of file **********************************************/
