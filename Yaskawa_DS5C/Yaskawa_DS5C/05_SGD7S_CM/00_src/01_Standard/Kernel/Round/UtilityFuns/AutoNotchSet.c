/****************************************************************************************************/
/*																									*/
/*																									*/
/*		AutoNotchSet.c : 自動ノッチフィルタ設定機能メインモジュール									*/
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
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2007.02.04  Y.Takei	<V109>														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#include "AutoNotchSet.h"
#include "TuneLib.h"
#include "PrmAccessIf.h"
#include "PnPrmListTbl.h"
#include "KLib.h"
#include "MLib.h"



/****************************************************************************************************/
/*																									*/
/*		Function Definition																			*/
/*																									*/
/****************************************************************************************************/
/* void		AutoNotchFilterSetting( ANOTCHSEQCTRL *ANotchCtrl );									*/
/* void		AutoNotchInitialize( ANOTCHSEQ *ANotchSeq );											*/
/* void		AutoNotchCalculatePrm( ANOTCHSEQ *ANotchSeq, USHORT kv, USHORT jrate);					*/
/* void		AutoNotchCalculateInitPrm( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch,						*/
/*															LONG AmpType, UNI_PRM_STR *UniPrm );	*/
/* void		RpiResetStatusANotch1( ANOTCHSEQ *ANotchSeq );											*/
/* void		RpiResetStatusANotch2( ANOTCHSEQ *ANotchSeq );											*/
/* UCHAR	RpiGetANotchPreStat( ANOTCHSEQ *ANotchSeq );											*/
/* USHORT	RpiGetANotchSetStat( ANOTCHSEQ *ANotchSeq );											*/
/* void		RpiSetRegSpatANotchState( ANOTCHSEQ *ANotchSeq );										*/
/* void		RpiANotchFftReqSet( ANOTCHSEQ *ANotchSeq, FFTANALYZE *FftAna );							*/
/* void		RpiANotchTrqFilCalc( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch,							*/
/*														REGIF_CONFIG_T *RegCfg, PRMDATA *Prm );		*/
/* void		RpiANotchDisableReqSet( ANOTCHSEQ *ANotchSeq );											*/
/* void		AutoNotchSetCondition( ANOTCHSEQ *ANotchSeq, BOOL EepromWriteProtect,					*/
/* 									BOOL BaseEnableSts, BOOL TuneLessSts, BOOL OverTravelSts );		*/
/****************************************************************************************************/
static	SHORT		autoNotchCheckCondition( ANOTCHSEQ *ANotchSeq, DETVIB *DetVib, PRMDATA *Prm );
static	LONG		autoNotchFreqComp( ANOTCHSEQ *ANotchSeq, LONG DFreq );
static	PRM_RSLT	autoNotchPrmWriteEeprom( REGIF_CONFIG_T *RegCfg, BOOL EepromWriteProtect );



/****************************************************************************************************/
/*																									*/
/*		自動ノッチ変数初期化																		*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchInitialize( ANOTCHSEQ *ANotchSeq )
{
	MlibResetLongMemory( &(ANotchSeq->var), sizeof(ANotchSeq->var) / 4 );
	return;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチ用Kv設定																			*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchCalculatePrm( ANOTCHSEQ *ANotchSeq, USHORT kv, USHORT jrate)
{
	/*----------------------------------------------------------------------------------------------*/
	/*	自動ノッチ計算用ゲインパラメータ取得														*/
	/*----------------------------------------------------------------------------------------------*/
	ANotchSeq->conf.kv = kv;
	ANotchSeq->conf.jrate = jrate;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチ用初期設定																		*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchCalculateInitPrm( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, LONG AmpType, UNI_PRM_STR *UniPrm )
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	LONG		VibDetectLevel;

	Prm = UniPrm->Prm;
	Bprm = UniPrm->Bprm;

	if( AmpType == MOTTYPE_LINEAR )
	{
		VibDetectLevel = NOTCH_BASIC_VIB_LEVEL_L;
	}
	else
	{
		VibDetectLevel = NOTCH_BASIC_VIB_LEVEL;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	自動ノッチ用振動レベル																		*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	ANotchSeq->conf.VibCheckLevelForTunLess =
		BprmSpeedLevelCal( Bprm, (VibDetectLevel + (Bprm->SpdDetectUnit /2 )), 0 );
#else
	ANotchSeq->conf.VibCheckLevelForTunLess =
		BprmSpeedLevelCal( Bprm, (VibDetectLevel + (Bprm->SpdDetectUnit >> 1)), 0 );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*	初期値は第１Kv、イナーシャ比設定値															*/
	/*----------------------------------------------------------------------------------------------*/
	ANotchSeq->conf.kv = Prm->loophz;
	ANotchSeq->conf.jrate = Prm->jrate;
	/*----------------------------------------------------------------------------------------------*/
	/*	自動ノッチフィルタタイプ初期値																*/
	/*----------------------------------------------------------------------------------------------*/
	TuneFilGainCalbyFilType( ANotch, NOTCH_FILTYPE );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		調整レス自動ノッチシーケンス																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	概要:通常運転状態で振動検出および周波数解析を行いノッチフィルタを設定する						*/
/*		 設定するのは、ノッチ周波数、ノッチＱ値およびトルク指令フィルタ								*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchFilterSetting( ANOTCHSEQCTRL *ANotchCtrl )
{
	LONG			Freq;
	LONG			FreqCmp;
	LONG			DetectFreqMin;
	ANOTCHSEQ		*ANotchSeq;
	AUTONOTCH		*ANotch;
	FFTANALYZE		*FftAna;
	DETVIB			*DetVib;
	REGIF_CONFIG_T	*RegCfg;
	PRMDATA			*Prm;

	/* 各ポインタの設定 */
	ANotchSeq = ANotchCtrl->ANotchSeq;
	ANotch = ANotchCtrl->ANotch;
	FftAna = ANotchCtrl->FftAna;
	DetVib = ANotchCtrl->DetVib;
	RegCfg = ANotchCtrl->RegCfg;
	Prm = ANotchCtrl->Prm;

	/*----------------------------------------------------------------------------------------------*/
	/*	自動ノッチ実行判別																			*/
	/*----------------------------------------------------------------------------------------------*/
	if( ANotchSeq->conf.TuneLessSts == FALSE )
	{ /* 調整レス無効状態 */
		return;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	サーボ制御用トレース																		*/
	/*----------------------------------------------------------------------------------------------*/
	ANotchSeq->var.FftStat =
		TuneVibrationTrace( DetVib, FftAna, ANotchSeq->var.FftReq, 0, ANotchSeq->conf.TuneLessSts );

	/*----------------------------------------------------------------------------------------------*/
	/*	自動ノッチ実行条件判別																		*/
	/*----------------------------------------------------------------------------------------------*/
	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F00) == 0x0000 )
	{ /* 自動設定可能なノッチがない */
		/* 自動設定不可ステータス設定 */
		ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NOENABLE;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	OT入力中はノッチ設定待ち																	*/
	/*----------------------------------------------------------------------------------------------*/
	if( (ANotchSeq->conf.OverTravelSts == TRUE) && (FftGetForceAnaReq( FftAna ) == FALSE) )
	{ /* OT中 && サーボ調整ノッチ周波数検出強制実行なし */
		/* 振動検出待ちフラグON */
		ANotchSeq->var.NotchChk = TRUE;
		/* 振動検出待ちタイマリセット */
		KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	周波数解析要求																				*/
	/*----------------------------------------------------------------------------------------------*/
	if( autoNotchCheckCondition(ANotchSeq, DetVib, Prm) == OK )
	{
		/* 念のため強制周波数解析要求クリア */
		FftSetForceAnaReq( FftAna, FALSE );
		ANotchSeq->var.FftReq = TRUE;
		ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DETECT;

		/* 差速度ピーク値クリア */
		DetVibResetMaxMinSpeedError( DetVib );
	}

	/*------------------------------------------------------------------------------------------*/
	/*	ノッチフィルタ設定																		*/
	/*------------------------------------------------------------------------------------------*/
	if( ANotchSeq->var.FftStat == TUNE_FREQ_SEARCHEND )
	{ /* 周波数解析正常終了時 */
		/* トレース要求OFF */
		ANotchSeq->var.FftReq = FALSE;

		/*--------------------------------------------------------------------------------------*/
		/*	検出周波数下限設定																	*/
		/*--------------------------------------------------------------------------------------*/
		if( FftGetForceAnaReq( FftAna ) == FALSE )
		{	/* 強制検出OFF */
			DetectFreqMin = ANOTCH_MIN_SET_FREQ;
		}
		else
		{	/* 強制検出 */
			DetectFreqMin = ANOTCH_MIN_SET_FREQ2;
			/* 強制周波数解析要求クリア */
			FftSetForceAnaReq( FftAna, FALSE );
		}

		/*--------------------------------------------------------------------------------------*/
		/*	ノッチ周波数補正																	*/
		/*--------------------------------------------------------------------------------------*/
		/* ノッチ周波数補正値取得 */
		FreqCmp = (LONG)FftGetAnaRsltVibmRf( FftAna );
		/* 自動ノッチ周波数補正 */
		Freq = autoNotchFreqComp( ANotchSeq, FreqCmp );

		/*--------------------------------------------------------------------------------------*/
		/*	自動ノッチフィルタタイプ設定														*/
		/*--------------------------------------------------------------------------------------*/
		if( ANotchSeq->conf.TuneLessSts == TRUE )
		{ /* 調整レス有効 */
			if( Freq >= 2000 )
			{	/* 2000Hz以上はフィルタタイプ3 */
				TuneFilGainCalbyFilType( ANotch, NOTCH_FILTYPE2 );
			}
			else
			{
				TuneFilGainCalbyFilType( ANotch, NOTCH_FILTYPE );
			}
		}

		/* ノッチ設定　*/
		if( Freq >= DetectFreqMin )
		{ /* 200Hz未満はノッチフィルタ使用しない */
			/* 自動ノッチフィルタ設定情報セット */
			TuneSetNotchFilSettingData( ANotch, (USHORT)Freq, ANotchSeq->conf.kv, ANotchSeq->conf.jrate );

			/* 自動ノッチフィルタ設定処理 */
			ANotchSeq->var.ANotchSetStat = TuneNotchFilterSetting( ANotch, Prm, RegCfg );

			/* ノッチ設定ステータス作成 */
			switch( ANotchSeq->var.ANotchSetStat & 0x0F )
			{
			case OPTUNE_STS_NOTCH1_OK: /* 1段目ノッチ設定正常完了 */
				/* ノッチ設定EEPROM書き込み */
				autoNotchPrmWriteEeprom( RegCfg, ANotchSeq->conf.EepromWriteProtect );
				ANotchSeq->var.ANotch1Stat = OPTUNE_STS_NT1_COMP;
				ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NT1_COMP;
				break;

			case OPTUNE_STS_NOTCH2_OK: /* 2段目ノッチ設定正常完了 */
				/* ノッチ設定EEPROM書き込み */
				autoNotchPrmWriteEeprom( RegCfg, ANotchSeq->conf.EepromWriteProtect );
				ANotchSeq->var.ANotch2Stat = OPTUNE_STS_NT2_COMP;
				ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NT2_COMP;
				break;

			case OPTUNE_STS_NOTCHQ1_ERR: /* 1段目ノッチQ値異常 */
				ANotchSeq->var.ANotch1Stat = OPTUNE_STS_NT1_NG;
				break;

			case OPTUNE_STS_NOTCHQ2_ERR: /* 2段目ノッチQ値異常 */
				ANotchSeq->var.ANotch2Stat = OPTUNE_STS_NT2_NG;
				break;

			default:
				break;
			}

			/* 振動未検出ステータス設定 */
			ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NODETECT;
		}
		else
		{ /* 検出最小値以下の場合 */
			/* 検出エラーステータス設定 */
			ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DET_ERR;
		}

		/*------------------------------------------------------------------------------------------*/
		/*	振動検出待ち処理																		*/
		/*		周波数解析後は、ノッチON/OFFに関わらず、一旦検出を中止する。						*/
		/*		直後に再度、周波数解析しても同じ結果にしかならないため。							*/
		/*------------------------------------------------------------------------------------------*/
		/* 振動検出待ちフラグON */
		ANotchSeq->var.NotchChk = TRUE;
		/* 振動検出待ちタイマリセット */
		KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );
	}
	else if( ANotchSeq->var.FftStat == TUNE_FREQ_SEARCHERR )
	{	/* エラー処理 */
		ANotchSeq->var.FftReq = FALSE;
		/* 強制周波数解析要求クリア */
		FftSetForceAnaReq( FftAna, FALSE );
		/* 検出エラーステータス設定 */
		ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DET_ERR;

		/*------------------------------------------------------------------------------------------*/
		/*	振動検出待ち処理																		*/
		/*		周波数解析後は、ノッチON/OFFに関わらず、一旦検出を中止する。						*/
		/*		直後に再度、周波数解析しても同じ結果にしかならないため。							*/
		/*------------------------------------------------------------------------------------------*/
		/* 振動検出待ちフラグON */
		ANotchSeq->var.NotchChk = TRUE;
		/* 振動検出待ちタイマリセット */
		KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );
	}

	/*------------------------------------------------------------------------------------------*/
	/*	ノッチ設定待ち																			*/
	/*------------------------------------------------------------------------------------------*/
	if( ANotchSeq->var.NotchChk == TRUE )
	{	/* ノッチ設定完了時 */
		if( KlibGetLongTimerMs( ANotchSeq->var.NotchChkTimer ) > ANOTCH_NOTCH_SET_CHK_TIME )
		{
			/* 振動収束待ち中はピーク値を常時クリア */
			DetVibResetMaxMinSpeedError( DetVib );
			/* チェック待ち解除 */
			ANotchSeq->var.NotchChk = FALSE;
			/* ノッチ設定初期状態へ移行 */
			ANotchSeq->var.ANotchSetStat = OPTUNE_STS_NOTCH_INIT;
		}
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチ条件チェック																		*/
/*																									*/
/****************************************************************************************************/
static	SHORT	autoNotchCheckCondition( ANOTCHSEQ *ANotchSeq, DETVIB *DetVib, PRMDATA *Prm )
{
	SHORT	rc;
	LONG	SpdErrPeak;
	rc = NG;

	if( ((~Prm->tfuncsw) & Prm->anotchsw & 0x0F00) != 0x0000 )
	{ /* 自動設定可能なノッチあり */
		if( (ANotchSeq->var.NotchChk == FALSE) && (ANotchSeq->conf.BaseEnableSts == TRUE) )
		{ /* チェック待ち解除中 && サーボオン中 */
			/* 振動検出用速度偏差の Peak to Peak 取得 */
			SpdErrPeak = DetVibGetSpdErrPeak( DetVib );
			if( SpdErrPeak > ANotchSeq->conf.VibCheckLevelForTunLess )
			{ /* 振動検出レベルが振動レベル超過 */
				rc = OK;
			}
		}
	}
	return( rc );
}





/****************************************************************************************************/
/*																									*/
/*		自動ノッチ周波数補正(補正式は実験結果より導出)												*/
/*																									*/
/****************************************************************************************************/
static	LONG	autoNotchFreqComp( ANOTCHSEQ *ANotchSeq, LONG DFreq )
{
	LONG	Freq;

	/* 補正周波数初期化 */
	Freq = DFreq;

	if( ANotchSeq->conf.TuneLessSts == TRUE )
	{ /* 調整レス有効 */
		if( DFreq < NOTCH_COMP_MAX_FREQ )
		{
			Freq = DFreq;
		}
		else
		{
			/* 周波数解析分解能×２を補正(検出誤差) */
			Freq = DFreq + (FREQDETUNIT << 1);
		}
	}

	if( Freq < 0 )
	{
		Freq = 0;
	}

	return( Freq );
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチパラメータEEPROM書き込み処理														*/
/*																									*/
/****************************************************************************************************/
static	PRM_RSLT	autoNotchPrmWriteEeprom( REGIF_CONFIG_T *RegCfg, BOOL EepromWriteProtect )
{
	PRM_RSLT	rc = PRM_RSLT_SUCCESS;

	if( EepromWriteProtect == FALSE )
	{ /* EEPROM書き込み禁止中(AAT中)でない */
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );	/* Pn409 */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );		/* Pn40A */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );	/* Pn40C */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );		/* Pn40D */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );	/* Pn401 */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
		rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );	/* Pn408 */
		if( rc != PRM_RSLT_SUCCESS )
		{
			return(rc);
		}
	}
	return	rc;
}



/****************************************************************************************************/
/*																									*/
/*		第１ノッチ解除ステータス初期化API															*/
/*																									*/
/****************************************************************************************************/
void	RpiResetStatusANotch1( ANOTCHSEQ *ANotchSeq )
{
	ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NODETECT;			/* 振動未検出ステータス設定			*/
	ANotchSeq->var.ANotch1Stat = OPTUNE_STS_NT1_NODETECT;		/* 1段目ステータスを未実施に初期化	*/
	ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;		/* 直前ノッチを未実施に初期化		*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		第２ノッチ解除ステータス初期化API															*/
/*																									*/
/****************************************************************************************************/
void	RpiResetStatusANotch2( ANOTCHSEQ *ANotchSeq )
{
	ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_NODETECT;			/* 振動未検出ステータス設定			*/
	ANotchSeq->var.ANotch2Stat = OPTUNE_STS_NT2_NODETECT;		/* 2段目ステータスを未実施に初期化	*/
	ANotchSeq->var.ANotchPreStat = OPTUNE_PSTS_NODETECT;		/* 直前ノッチを未実施に初期化		*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチ前回設定ノッチステータス取得API													*/
/*																									*/
/****************************************************************************************************/
UCHAR	RpiGetANotchPreStat( ANOTCHSEQ *ANotchSeq )
{
	return	ANotchSeq->var.ANotchPreStat;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチステータス取得API																	*/
/*																									*/
/****************************************************************************************************/
USHORT	RpiGetANotchSetStat( ANOTCHSEQ *ANotchSeq )
{
	return	ANotchSeq->var.ANotchSetStat;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチステータス取得API(レジスタi/F)													*/
/*																									*/
/****************************************************************************************************/
void	RpiSetRegSpatANotchState( ANOTCHSEQ *ANotchSeq )
{
	USHORT	uswk;

	uswk = (ANotchSeq->var.ANotchPreStat << 12)
			+ (ANotchSeq->var.ANotch2Stat << 8)
			+ (ANotchSeq->var.ANotch1Stat << 4)
			+ (ANotchSeq->var.ANotchStat);

	ANotchSeq->OpeReg.SpatANotchState = uswk;
	return;
}



/****************************************************************************************************/
/*																									*/
/*		強制周波数解析処理要求セット																*/
/*																									*/
/****************************************************************************************************/
void	RpiANotchFftReqSet( ANOTCHSEQ *ANotchSeq, FFTANALYZE *FftAna )
{
	ANotchSeq->var.FftReq = TRUE;
	ANotchSeq->var.ANotchStat = OPTUNE_STS_NT_DETECT;

	/* 強制周波数解析要求 */
	FftSetForceAnaReq( FftAna, TRUE );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		トルクフィルタ計算																			*/
/*																									*/
/****************************************************************************************************/
void	RpiANotchTrqFilCalc( ANOTCHSEQ *ANotchSeq, AUTONOTCH *ANotch, REGIF_CONFIG_T *RegCfg, PRMDATA *Prm )
{
	TuneNotchQ( ANotch, RegCfg, ANotchSeq->conf.kv, ANotchSeq->conf.jrate, 1, Prm );

	return;
}



/****************************************************************************************************/
/*																									*/
/*		振動検出強制無効要求セット		3秒間振動検出無効											*/
/*																									*/
/****************************************************************************************************/
void	RpiANotchDisableReqSet( ANOTCHSEQ *ANotchSeq )
{
	ANotchSeq->var.NotchChk = TRUE;									/* 振動検出待ちフラグON			*/
	KlibRstLongTimer( &ANotchSeq->var.NotchChkTimer );				/* 振動検出待ちタイマリセット	*/

	return;
}



/****************************************************************************************************/
/*																									*/
/*		自動ノッチフィルタ設定用データ設定API														*/
/*																									*/
/****************************************************************************************************/
void	AutoNotchSetCondition( ANOTCHSEQ *ANotchSeq, BOOL EepromWriteProtect,
										BOOL BaseEnableSts, BOOL TuneLessSts, BOOL OverTravelSts )
{
	/* 各実行条件の設定 */
	ANotchSeq->conf.EepromWriteProtect = EepromWriteProtect;		/* EEPROM書き込み禁止フラグ		*/
	ANotchSeq->conf.BaseEnableSts = BaseEnableSts;					/* サーボON/OFF状態フラグ		*/
	ANotchSeq->conf.TuneLessSts = TuneLessSts;						/* 調整レス有効・無効フラグ		*/
	ANotchSeq->conf.OverTravelSts = OverTravelSts;					/* P-OT/N-OT状態フラグ			*/
}



/***************************************** end of file **********************************************/
