/****************************************************************************
  Description   : 電流検出AD調整(Fn00E, Fn00F)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation.
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

  Create Date   : 2010-11-18

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "FunManager.h"
#include "JL086ServoIpHw.h"

#include "PnPrmListTbl.h"
#include "KLib.h"


/****************************************************************************************************/
/*		DEFINES																						*/
/****************************************************************************************************/
#define KPI_RC_NOTADJ	0		/* 調整実施条件不成立 */
#define KPI_RC_ADJOK	1		/* 調整完了 */
#define KPI_RC_ADJERR	-1		/* 調整失敗 */

#define OFFSET_ADJ		0		/* 電流検出オフセット調整 */
//#define GAIN_ADJ		1		/* 電流検出ゲイン調整 */

/* <S057>	Start */
/* -------- Fn00F Current Offset Manual Adjustment -------- */
#define	ZADJI_MAX		511		/* 電流検出信号のオフセット手動調整値　最大値 */
#define	ZADJI_MIN		-512	/* 電流検出信号のオフセット手動調整値　最小値 */

//#define	ACCLVL_SYSTEM	3		/* ユーザレベル = システムレベル */	/* <S101> */
#define	ACCLVL_SYSTEM	4			/* ユーザレベル = システムレベル */	/* <S101> */

//#define	OK	1					/* 電流検出信号のオフセット手動調整　実行可能	*///<S0C7>
//#define	NG	0					/* 電流検出信号のオフセット手動調整　実行不可能	*///<S0C7>
/* <S057>	End */

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
static LONG	KpiAdjustCurAdZero( AXIS_HANDLE *Axis );
static LONG	TciGetCurADOffset( ASICS *SvAsicRegs, PUMP_CTR_STR *pump_if_ptr, LONG AdZero[2], LONG Mode );
static LONG	TciReadCurADMean( ASICS *SvAsicRegs, PUMP_CTR_STR *pump_if_ptr, LONG AdMean[2], LONG Mode );

/****************************************************************************
 Fn00E: Entry function of Motor Current Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterMotorCurrentAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	MOTLESSTST			*MotorLessTest;	/* モータレステストモードIF定義		*//* <S1B7> */

	FnCmnCtrl = Axis->FnCmnCtrl;
	MotorLessTest = Axis->MotorLessTest;									/* <S1B7> */

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* モータレステストモード判定 *//* <S1B7> */
	if( (MotorLessTest->MotorLessTestMode == TRUE)
		&& (MotorLessTest->MotorLessTestDatas.SemiEncConnect == FALSE) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

//	FnCmnCtrl->KnlMode = KPI_KMD_ADJREFAD;
//	FnCmnCtrl->KnlMode = KPI_KMD_ADJCURAD;	/* 電流検出ＡＤ調整中 *//* <S085> */

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************
 Fn00E: Execute function of Motor Current Offset Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteMotorCurrentAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	ASIC_HW_IF_STR	*AsicHwReg;

	LONG			AdjResult;
	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	AsicHwReg = Axis->SvAsicRegs->AsicHwReg;

	/* 実行要求がくるまで、待ち	*/
	if(FunExe->CmdCode != FCMD_EXEC)
	{
		return PRM_RSLT_CONTINUE;
	}

	/* サーボオン中の場合は入らずに終了	*/
	if( (SeqCtrlOut->MainPowerOn == FALSE)
		|| (SeqCtrlOut->HwbbSts != FALSE)
		|| (SeqCtrlOut->MotStop == FALSE)
//		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) != 0) )	/* Base Block */
		|| (SHalDrv_GetFaultStatus(AsicHwReg, SHAL_FAULT_BB) == 0) )	/* Base Enable */
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* 調整実行 */
	AdjResult = KpiAdjustCurAdZero(Axis);

	/* 調整終了 */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;

	if(AdjResult == KPI_RC_NOTADJ){
		return PRM_RSLT_CONDITION_ERR;
	}
	else if(AdjResult == KPI_RC_ADJOK){
		/* 何もしない */
	}
	else if(AdjResult == KPI_RC_ADJERR){
		return PRM_RSLT_CALC_ERR;
	}
	return PRM_RSLT_SUCCESS;
}

/* <S057>	Start */
/****************************************************************************
 Fn00E: End function of Motor Current Offset Auto Adjustment
****************************************************************************/
void FnLeaveMotorCurrentAutoAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	REGIF_CONFIG_T	*RegCfg;			/* <S057> */
	RegCfg = Axis->RegManager->hPnReg;	/* <S057> */

	/* 終了処理		*/
	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiu, 0 );		/* PnE50:電流検出ゼロ調(U相) */
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiv, 0 );		/* PnE51:電流検出ゼロ調(V相) */

	/* <S1B6>  マイクロ書き込み */
	pndef_zadjiu.PrmCalCallback( 0, 0, Axis, 0 );
	pndef_zadjiv.PrmCalCallback( 0, 0, Axis, 0 );
}

/****************************************************************************
 Fn00F: Entry function of Motor Current Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnEnterMotorCurrentManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;
	FnCmnCtrl = Axis->FnCmnCtrl;

	/* モータレステストモード有効 且 エンコーダ未接続の場合は入らずに終了 */
	/* TODO:モータレステストモード現状未対応 */
#if 0
	if( (Kprm.f.MotorLessTestMode && (MotorLessTestDatas.f.SemiEncConnect == FALSE)) )		/*<V245>*/
	{		
		return PRM_RSLT_CONDITION_ERR;
	}
#endif

//	FnCmnCtrl->KnlMode = KPI_KMD_ADJCURAD;	/* 電流検出ＡＤ調整中 */
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;	/* normal mode */	/* <S142> */

	FunExe->HoldFnMode = TRUE;				/* Fnモード自動終了なし */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 Fn00F: Execute function of Motor Current Offset Manual Adjustment
****************************************************************************/
PRM_RSLT FnExecuteMotorCurrentManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRMDATA			*Prm;
	PRM_RSLT		rc;
	LONG			zadjiu_tmp;
	LONG			zadjiv_tmp;
	CHAR			Exe_flg;
	FUN_AXCOMMON	*AxCommon;

	Prm = Axis->UniPrms->Prm;
	AxCommon = ((AXIS_HANDLE*)Axis)->FnManager->AxCommon;

	rc = PRM_RSLT_SUCCESS;
	Exe_flg = NG;

	/* ユーザレベル確認 */
#if 0
	if(AxCommon->AccessLevel < ACCLVL_SYSTEM )
	{
		Exe_flg = NG;
	}
	else
	{
		Exe_flg = OK;
	}
#else
		Exe_flg = OK;
#endif
	switch( FunExe->State )
	{
	case 0:	/* 電流検出ゼロ調(U相) 調整	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp < ZADJI_MAX)
				{
					zadjiu_tmp+=1;
				}
				/* PnE50:電流検出ゼロ調(U相)に更新した値を書き込み */
				Prm->zadjiu = (SHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;
		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE50:電流検出ゼロ調(U相)の値読み込み */
				zadjiu_tmp = (LONG)(SHORT)((Prm->zadjiu) & 0xFFFF);

				if(zadjiu_tmp > ZADJI_MIN)
				{
					zadjiu_tmp-=1;
				}
				/* PnE50:電流検出ゼロ調(U相)に更新した値を書き込み */
				Prm->zadjiu = (USHORT)zadjiu_tmp;
				pndef_zadjiu.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;
			break;

		case FCMD_CH2:
			FunExe->State = 1;
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
		break;

	case 1:	/* 電流検出ゼロ調(V相) 調整	*/
		switch(FunExe->CmdCode)
		{
		case FCMD_UP:
			if(Exe_flg == OK)
			{
				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);


				if(zadjiv_tmp < ZADJI_MAX)
				{
					zadjiv_tmp += 1;
				}
				/* PnE51:電流検出ゼロ調(V相)に更新した値を書き込み */
				Prm->zadjiv = (SHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;

		case FCMD_DOWN:
			if(Exe_flg == OK)
			{
				/* PnE51:電流検出ゼロ調(V相)の値読み込み */
				zadjiv_tmp = (LONG)(SHORT)((Prm->zadjiv) & 0xFFFF);

				if(zadjiv_tmp > ZADJI_MIN)
				{
					zadjiv_tmp -=1;
				}
				/* PnE51:電流検出ゼロ調(U相)に更新した値を書き込み */
				Prm->zadjiv = (USHORT)zadjiv_tmp;
				pndef_zadjiv.PrmCalCallback( 0, 0, Axis, 0 );	/* <S1B6> */
			}
			break;

		case FCMD_CH1:
			FunExe->State = 0;
			break;

		default:	/* other command */
			rc = PRM_RSLT_CONDITION_ERR;	/* 条件エラー */
			break;
		}
		break;

	default:
		break;
	}

	return rc ;

}

/****************************************************************************
 Fn00F: End function of Motor Current Offset Manual Adjustment
****************************************************************************/
void FnLeaveMotorCurrentManualAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL	*FnCmnCtrl;
	REGIF_CONFIG_T	*RegCfg;

	FnCmnCtrl = Axis->FnCmnCtrl;
	RegCfg = Axis->RegManager->hPnReg;

	/* 終了処理		*/
	/* EEPROM更新処理	*/
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiu, 0 );		/* PnE50:電流検出ゼロ調(U相) */
	PrmStoreValueToEeprom( RegCfg, &pndef_zadjiv, 0 );		/* PnE51:電流検出ゼロ調(V相) */

	/* 終了処理	*/
	FnCmnCtrl->KnlMode = KPI_KMD_NORMAL;					/* normal mode */
}

/* <S057>	End */

/****************************************************************************
 1103H : Entry function of the Current Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnEnterMotorCurrentGainAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	FUN_CMN_CONTROL		*FnCmnCtrl;

	FnCmnCtrl = Axis->FnCmnCtrl;

	/* サーボオン中の場合は入らずに終了	*/
	if(FnCmnCtrl->State.FnSvCtrlDisable != FALSE)
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnサーボ運転権獲得 */
	FnCmnCtrl->FnSvControl = TRUE;
	FnCmnCtrl->FnCtrlMcmd = CTRLMODE_JOG;
	FnCmnCtrl->FnSensOnReq = TRUE;
	FnCmnCtrl->JogSpeed = 0;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************
 1103H : Execute function of the Current Gain Auto Adjustment
****************************************************************************/
PRM_RSLT FnExecuteMotorCurrentGainAdj(FUNEXE *FunExe, AXIS_HANDLE *Axis)
{
	PRM_RSLT		errRes;
	FUN_CMN_CONTROL	*FnCmnCtrl;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	MENCV			*MencV;
	ALMMAN_HANDLE	*AlmManager;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	LONG			JogSpdP1;

	FnCmnCtrl = Axis->FnCmnCtrl;
	SeqCtrlOut = Axis->SeqCtrlOut;
	MencV = Axis->MencV;
	AlmManager = Axis->AlmManager;
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	errRes = PRM_RSLT_CONTINUE;

	if(FunExe->State == 0)
	{/* サーボOFF中	*/
		JogSpdP1 = (MencV->AxisMotType == MOTTYPE_LINEAR) ? Prm->jogspd_l : Prm->jogspd;
		if(MencV->LowSpdMotor == FALSE)
		{
			JogSpdP1 = (10 * JogSpdP1);
		}
		JogSpdP1 = BprmSpeedLevelCal(Bprm, JogSpdP1, 0);

		switch(FunExe->CmdCode)
		{
		case FCMD_DOWN:	/* サーボONと同時に逆転	*/
			JogSpdP1 = -JogSpdP1;
		case FCMD_UP:	/* 正転	*/
			FnCmnCtrl->JogSpeed = JogSpdP1;
			if( (SeqCtrlOut->MainPowerOn == FALSE)
				|| (SeqCtrlOut->HwbbSts != FALSE)
				|| (AlmManager->AlmReady == FALSE) )
			{
				errRes = PRM_RSLT_CONDITION_ERR;
			}
			else
			{
				FnCmnCtrl->FnSvonReq = TRUE;
			}
			break;

		default:
			break;
		}

		if(SeqCtrlOut->BaseEnableReq != FALSE)
		{
			FunExe->State = 1;
		}
	}
	else if(FunExe->State == 1)
	{/* サーボON中	*/
		/* 調整実行 */


		/* 調整終了->サーボオフ */
		FnCmnCtrl->FnSvonReq = TRUE;
		FunExe->State = 2;
	}
	else
	{/* 終了処理 */
		if(SeqCtrlOut->BaseEnableReq == FALSE)
		{/* サーボオフ確認後終了 */
			errRes = PRM_RSLT_SUCCESS;
		}
	}

	return errRes;
}



/****************************************************************************************************/
/*																									*/
/*		電流検出ＡＤ零調処理(オンライン用)															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : Ｕ相／Ｖ相の電流検出ＡＤ零調用データを設定する。										*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG KpiAdjustCurAdZero( AXIS_HANDLE *Axis )
{
	LONG				i;
	LONG				AdZero[2];
	LONG				AdjustError;
	BOOL				HwbbSts;/* <HWBB> */
	ASIC_HW_IF_STR		*pAsicHwReg;
	REG_ACC_T			AccessPrm;
	FUN_CMN_CONTROL		*FnCmnCtrl;		/* <S085> */

	/* Initialize */
	AdjustError = FALSE;
	HwbbSts = Axis->SeqCtrlOut->HwbbSts;/* <HWBB> */
	pAsicHwReg = Axis->SvAsicRegs->AsicHwReg;
	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr	= 0;
	AccessPrm.hRegCfg	= Axis->RegManager->hPnReg;
	AccessPrm.ArrayIdx	= 0;
	AccessPrm.EepWrite	= TRUE;		/* EEPROM Write */
	AccessPrm.IfHandle	= NULL;
	FnCmnCtrl = Axis->FnCmnCtrl;		/* <S085> */

/*--------------------------------------------------------------------------------------------------*/
/*		開始処理 (Set Adjust Mode)																	*/
/*--------------------------------------------------------------------------------------------------*/
	FnCmnCtrl->KnlMode = KPI_KMD_ADJCURAD;	/* 電流検出ＡＤ調整中 *//* <S085> */
	/* 呼び出し元でチェックされるので、ここでは何もしない */
//	if( (MainPowerOn == FALSE) || (MotStopSts == FALSE) || (HwbbSts == TRUE) )/* <HWBB> */
//	{
//		return( KPI_RC_NOTADJ );						/* Error Return								*/
//	}
	/* 開始チェック処理しないので、ウェイトも削除 */
//	KlibWaitms( 16 );

/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロックチェック																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* ※knlMode == KPI_KMD_ADJCURADなら、ＤＢ回路制御 LpxDBControlProcess()は何も処理しない		*/
	/* ※本処理内でDB OFFとする																		*/
//	pAsicHwReg->AREG_ALMSET |= BIT5;					/* DB OFF									*/
	/* モータレステストモードでないときOFFする *//* <S1B7> */
	/* TODO: SGD-Vでは以下の条件もORしてある						*/
	/* (1) AxisRscC->BeSequence->StopMode == STOPMODE_DBSTOPFR		*/
	/* (2) AxisRscC->BeSequence->StopMode == STOPMODE_FREERUN		*/
	/* (3) AxisRscC->FnCmnCtrl->KnlMode == KPI_KMD_ADJCURAD			*/
	if( Axis->MotorLessTest->MotorLessTestMode == FALSE )
	{
		SHalDrv_DynamicBrakeOFF(pAsicHwReg);			/* DB OFF									*/
	}
	for( i=0; ; i++ )									/* 終了条件はBE or タイムオーバー			*/
	{
		if( pAsicHwReg->AREG_FLTSTAT & 0x0400 )			/* FLTSTAT_10  1:ベースブロック				*/
		{
			break;
		}
		else if( i >= 1000 )							/* Check Time Over							*/
		{
			return( KPI_RC_NOTADJ );					/* Error Return								*/
		}
		KlibWaitms( 1 );								/* Wait 1ms									*/
	}

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤ零調																			*/
/*--------------------------------------------------------------------------------------------------*/
	AdjustError = TciGetCurADOffset( Axis->SvAsicRegs, &(Axis->BeSequence->ChargePump), AdZero, OFFSET_ADJ );

	if( HwbbSts == TRUE )/* <HWBB> */
	{
		AdjustError = TRUE;								/* HWBBなら結果を採用しない					*/
	}
	else if( AdjustError == FALSE )						/* Check Adjust Error						*/
	{
		/* パラメータ計算関数をcallして調整結果を格納												*/
		/* PnE50 電流検出ゼロ調(U相)																*/
		TunePrmWriteRam( &AccessPrm, &pndef_zadjiu, AdZero[0], 2 );

		/* PnE51 電流検出ゼロ調(V相)																*/
		TunePrmWriteRam( &AccessPrm, &pndef_zadjiv, AdZero[1], 2 );
	}


/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤ零調終了処理																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* 初期化処理でのみ設定されるため、ここではA/D停止、同期フラグ設定を実行しない */
//	pAsicHwReg->AREG_ADFCNT2 = 0x0000;					/* AD停止									*/
	/* KPX_MPCMD_SYNC1( ); */							/* AD同期フラグ１(ホスト割り込みの代わり)	*/
	/* ここでのA/D同期フラグ設定は実行しない */
//	pAsicHwReg->MREG_HSUR = MPCMD_SYNC1;				/* AD同期フラグ１(ホスト割り込みの代わり)	*/
	/* ※knlMode == KPI_KMD_ADJCURADなら、ＤＢ回路制御 LpxDBControlProcess()は何も処理しない		*/
	/* ※本処理内でDB ONとする																		*/
	SHalDrv_DynamicBrakeON(pAsicHwReg);					/* DB ON									*/
//	pAsicHwReg->AREG_ALMSET &= ~(BIT5);					/* DB ON									*/

	if( AdjustError == FALSE )
	{
		return( KPI_RC_ADJOK );							/* Adjust OK								*/
	}
	else
	{
		return( KPI_RC_ADJERR );						/* Adjust Error								*/
	}
}



/****************************************************************************************************/
/*																									*/
/*		電流検出ＡＤオフセット取得処理(オフライン／オンライン用)									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概 要 : Ｕ相／Ｖ相の電流検出ＡＤオフセット調整用データを取得する。								*/
/*			なお、オフセット量が±512範囲を超える場合及び、オフセット設定後のドリフトが				*/
/*			±2内に入らなかった場合は、調整異常としてオフセット量をゼロにする。						*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG TciGetCurADOffset( ASICS *SvAsicRegs, PUMP_CTR_STR *PumpOnCtrl, LONG AdZero[2], LONG Mode )
{
	long	Driftx;
	long	AdMean[2];
	long	AdMeanx[2];
	long	Errflag;

	/* ローカル変数の初期化 */
	Errflag = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤオフセット取得																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( TciReadCurADMean( SvAsicRegs, PumpOnCtrl, AdMean, Mode ) == TRUE )
	{ /* 平均値読込み正常 */
		if( (AdMean[0] >= -512) && (AdMean[0] <= 512) )	/* U相零調データ正常						*/
		{
			AdZero[0] = -AdMean[0];						/* Set Adjust Data							*/
		}
		else											/* U相零調データ異常						*/
		{
			AdZero[0] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	/*----------------------------------------------------------------------------------------------*/
		if( (AdMean[1] >= -512) && (AdMean[1] <= 512) )	/* V相零調データ正常						*/
		{
			AdZero[1] = -AdMean[1];						/* Set Adjust Data							*/
		}
		else											/* V相零調データ異常						*/
		{
			AdZero[1] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{ /* 平均値読込み失敗(OC発生) */
		AdZero[0] = 0;									/* Set Zero									*/
		AdZero[1] = 0;									/* Set Zero									*/
		Errflag = TRUE;									/* Set AdjustError Flag						*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ＡＤドリフトチェック																*/
/*--------------------------------------------------------------------------------------------------*/
	if( TciReadCurADMean( SvAsicRegs, PumpOnCtrl, AdMeanx, Mode ) == TRUE )
	{ /* 平均値読込み正常 */
		Driftx = AdMean[0] - AdMeanx[0];				/* U相ドリフトチェック						*/
		if( (Driftx < -2) || (Driftx > 2) )				/* ドリフト許容範囲チェック					*/
		{
			AdZero[0] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	/*----------------------------------------------------------------------------------------------*/
		Driftx = AdMean[1] - AdMeanx[1];				/* V相ドリフトチェック						*/
		if( (Driftx < -2) || (Driftx > 2) )				/* ドリフト許容範囲チェック					*/
		{
			AdZero[1] = 0;								/* Set Zero									*/
			Errflag = TRUE;								/* Set AdjustError Flag						*/
		}
	}
	/*----------------------------------------------------------------------------------------------*/
	else
	{ /* 平均値読込み失敗(OC発生) */
		AdZero[0] = 0;									/* Set Zero									*/
		AdZero[1] = 0;									/* Set Zero									*/
		Errflag = TRUE;									/* Set AdjustError Flag						*/
	}
	return Errflag;
}



/* オフラインモード処理削除 start futami */
/****************************************************************************************************/
/*																									*/
/*		電流検出ＡＤ平均値読込み(オンライン用)														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 電流検出ＡＤ値を４０９６回読込み、その平均値を返す。									*/
/*																									*/
/*	補 足 : U相電流ADC値の下位2bitは無効なため, シフト2bit処理で捨てる。							*/
/*			V相電流ADC値の下位2bitは無効なため, シフト2bit処理で捨てる。							*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static LONG TciReadCurADMean( ASICS *SvAsicRegs, PUMP_CTR_STR *PumpOnCtrl, long AdMean[2], long Mode )
{
	LONG				i;
	LONG				AdBuf[2];
	LONG				Errflag;
	ASIC_HW_IF_STR		*pAsicHwReg;
	ASIC_U_IF_STR		*pAsicMicroReg;

	/* ローカル変数の初期化 */
	Errflag = FALSE;
	pAsicHwReg = SvAsicRegs->AsicHwReg;
	pAsicMicroReg = SvAsicRegs->AsicMicroReg;

/* KpiAdjustCurAdUGain実行時はJOGモードでの速度指令を入力するため、AdjPumponReq等は不要 */
/*--------------------------------------------------------------------------------------------------*/
/*		Start Charge Pump																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Mode == OFFSET_ADJ )							/* Motor Current Offset Adjust Mode			*/
	{
		/* KPI_ALARM_CLEAR( ); */						/* ASICアラームクリア						*/
		pAsicHwReg->AREG_ALMCLR = 0x802F;				/* ASICアラームクリア						*/
		/* 電流検出A/Dオフセット調整時は実行(通常はScanCのチャージポンプ処理内で実行するため不要)	*/
#ifndef	_JL076_USE_
		pAsicMicroReg->IntAdP.FccRst = 0x01;			/* AD変換デジメーションフィルタのAlmClr		*/
#else
		pAsicMicroReg->MREG_FCCRST = 0x01;				/* AD変換デジメーションフィルタのAlmClr		*/
#endif
		PumpOnCtrl->AdjPumponReq = TRUE;				/* Set PUMPON Request						*/
		KlibWaitms( 25 );								/* Wait Start Charg Pump					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Read AD 4096 Times 																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Mode == OFFSET_ADJ )							/* Motor Current Offset Adjust Mode			*/
	{
		/* KPI_ALARM_CLEAR( ); */						/* ASICアラームクリア 						*/
		pAsicHwReg->AREG_ALMCLR = 0x802F;				/* ASICアラームクリア 						*/
		/* 電流検出A/Dオフセット調整時は実行(通常はScanCのチャージポンプ処理内で実行するため不要)	*/
#ifndef	_JL076_USE_
		pAsicMicroReg->IntAdP.FccRst = 0x01;			/* AD変換デジメーションフィルタのAlmClr		*/
#else
		pAsicMicroReg->MREG_FCCRST = 0x01;				/* AD変換デジメーションフィルタのAlmClr		*/
#endif
		/* 初期化処理でA/D変換設定をしているため、ここではしない */
	//	pAsicHwReg->AREG_ADFCNT2 = 0x0038;				/* AD再スタート								*/
		/* KPX_MPCMD_SYNC1( ); */						/* AD同期フラグ１(ホスト割り込みの代わり)	*/
		/* ここでのA/D同期フラグ設定は実行しない */
	//	pAsicHwReg->MREG_HSUR = MPCMD_SYNC1;			/* AD同期フラグ１(ホスト割り込みの代わり)	*/
		/* 電流検出A/Dオフセット調整時のみ、本処理を実行 */
#ifndef	_JL076_USE_
		pAsicMicroReg->AdinV.IuOffsetIn = 0;			/* U相電流検出ゼロ調						*/
		pAsicMicroReg->AdinV.IvOffsetIn = 0;			/* V相電流検出ゼロ調						*/
#else
		pAsicMicroReg->MREG_IUOFIN = 0;					/* U相電流検出ゼロ調						*/
		pAsicMicroReg->MREG_IUOFIN = 0;					/* V相電流検出ゼロ調						*/
#endif
	}

	/* KPX_REFAD_LATCH( ); */							/* Latch AD Conversion Data					*/
	pAsicHwReg->AREG_ADSET1 |= BIT14;					/* Latch AD Conversion Data					*/

	AdBuf[0] = AdBuf[1] = 0;							/* Clear Buffer								*/
	for( i=0; i < 0x1000; i++ )							/* Read Loop 4096 Times						*/
	{
	/*----------------------------------------------------------------------------------------------*/
		/* KPX_REFAD_LATCH( ); */							/* Latch AD Conversion Data				*/
		pAsicHwReg->AREG_ADSET1 |= BIT14;					/* Latch AD Conversion Data				*/
		AdBuf[0] += ( (SHORT)(pAsicHwReg->AREG_AFUL)>>2 );	/* Read AD and Add Buf.(U-Phase)		*/
		AdBuf[1] += ( (SHORT)(pAsicHwReg->AREG_AFVL)>>2 );	/* Read AD and Add Buf.(V-Phase)		*/
		/* KpiResetWatchdogWaitus( 50 ); */					/* Wait 50us for AD CycleTime			*/
		/* intScanB()で毎回ウォッチドッグリセットを実行するため、ここでは処理しない */
//		pAsicHwReg->AREG_HINT1 = 0x0000;					/* Reset Watchdog						*/
		/* ウォッチドッグリセット処理しないので、ウェイトも削除 */
//		KlibWaitus( 50 );									/* Wait 50us							*/
		KlibWaitus( 50 );									/*  <S1A7> Wait 50us	A/D変換待ち		*/
//		pAsicHwReg->AREG_HINT1 = 0x0000;					/* Reset Watchdog						*/
	/*----------------------------------------------------------------------------------------------*/
		if( (pAsicHwReg->AREG_FLTSTAT & 0x0020) != 0 )		/* Check OC(Over Current)				*/
		{
			Errflag = TRUE;
			break;
		}
	} /* for( i=0; i < 0x1000; i++ ) */

/*--------------------------------------------------------------------------------------------------*/
/*		Stop Charge Pump																			*/
/*--------------------------------------------------------------------------------------------------*/
//	if( Mode == ONLINE_ADJ)								/* Online Adjust Mode						*/
//	{
//		PumpOnCtrl->AdjPumponReq = FALSE;				/* Reset PUMPON Request						*/
//	}
//	/* ASIC_WR( MREG_FCCRST, 0x01  ); */				/* AD変換デジメーションフィルタのAlmClr		*/
//	pAsicMicroReg->MREG_FCCRST	= 0x01;					/* AD変換デジメーションフィルタのAlmClr		*/

	if( Mode == OFFSET_ADJ)								/* Online Adjust Mode						*/
	{
		PumpOnCtrl->AdjPumponReq = FALSE;				/* Reset PUMPON Request						*/
		/* 電流検出A/Dオフセット調整時は実行(通常はScanCのチャージポンプ処理内で実行するため不要)	*/
#ifndef	_JL076_USE_
		pAsicMicroReg->IntAdP.FccRst = 0x01;			/* AD変換デジメーションフィルタのAlmClr		*/
#else
		pAsicMicroReg->MREG_FCCRST = 0x01;				/* AD変換デジメーションフィルタのAlmClr		*/
#endif
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Calculate AD Mean																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Errflag == FALSE )
	{
		AdMean[0] = ( (AdBuf[0]>>11) + 1 ) >> 1;		/* U-Phase Current AD Mean					*/
		AdMean[1] = ( (AdBuf[1]>>11) + 1 ) >> 1;		/* V-Phase Current AD Mean					*/
		return( TRUE );									/* Normal Return							*/
	}
	else
	{
		AdMean[0] = AdMean[1] = 0;						/* Clear Current AD Mean					*/
		return( FALSE );								/* Error Return								*/
	}
}
/* オフラインモード処理削除 end futami */
/******************************* End of File *******************************/
