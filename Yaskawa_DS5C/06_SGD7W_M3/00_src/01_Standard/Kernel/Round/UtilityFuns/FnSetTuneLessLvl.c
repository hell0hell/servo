/****************************************************************************
  Description   : Set Tuning Less Level (Fn200)
-----------------------------------------------------------------------------
  Author        : Yaskawa Electric Corporation. 
                  Copyright (c) 2010 All Rights Reserved

  Project       : INGRAM

----------------------------------------------------------------------------
  Changes		:
	Name   Date          Description
----------------------------------------------------------------------------
	Y.Oka   2011.01.31   created

****************************************************************************/
#include "FnSetTuneLessLvl.h"
#include "MercuryGlobals.h"
#include "KLib.h"
#include "PnPrmListTbl.h"


/****************************************************************************************************/
/* Set Tuning Less Level (Fn200)																	*/
/****************************************************************************************************/
static PRM_RSLT	setTLLvlChkCondition( TUNELESS_CTRL *TuneLessCtrl );
static PRM_RSLT	setTLLvlUpdateInEeprom( REGIF_CONFIG_T *RegCfg );
static void		setTLLvlUndoRamGain( REGIF_CONFIG_T *RegCfg );
static PRM_RSLT	setTLLvlSetLevel( REG_ACC_T *AccessPrm, USHORT GainLevel, USHORT JratLevel, USHORT DetLevel );



/****************************************************************************************************/
/*																									*/
/*		Fn200 : Set Tuning Less Level																*/
/*																									*/
/****************************************************************************************************/
/* -------- Fn200 Set Tuning Less Level Begin	-------- */
PRM_RSLT	FnEnterSetTuneLessLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	TUNELESS_CTRL	*TuneLessCtrl;
	PRMDATA			*Prm;

	TuneLessCtrl = &(Axis->BaseControls->TuneLessCtrl);
	Prm = Axis->UniPrms->Prm;
	rc = PRM_RSLT_SUCCESS;

	/* Initialize Register Data */
	RpiRegSetTuneLessLevel( Prm->DatLevel, TuneLessCtrl );

	/* Check Execution Condition */
	rc = setTLLvlChkCondition( TuneLessCtrl );

	/* モータレステストモードの場合は入らずに終了する *//* <S1B7> */
	if( Axis->MotorLessTest->MotorLessTestMode == TRUE )
	{
		return PRM_RSLT_CONDITION_ERR;
	}

	/* Fnモード自動終了なし */
	FunExe->HoldFnMode = TRUE;

	return(rc);
}

/* -------- Fn200 Set Tuning Less Level End	-------- */
void	FnLeaveSetTuneLessLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	REGIF_CONFIG_T	*RegCfg;
	RegCfg = Axis->RegManager->hPnReg;

	/* EEPROMデータで変更パラメータを元に戻す */
	setTLLvlUndoRamGain( RegCfg );

	/* Fn200実行中フラグOFF */
	Axis->BaseControls->TuneLessCtrl.var.TuningFuncRun = FALSE;

	/* Fnモード自動終了設定 */
	FunExe->HoldFnMode = FALSE;
}

/* -------- Fn200 Set Tuning Less Level Execution -------- */
PRM_RSLT	FnExecuteSetTuneLessLevel( FUNEXE *FunExe, AXIS_HANDLE *Axis )
{
	PRM_RSLT		rc;
	ULONG			ulwork;
	REG_ACC_T		AccessPrm;
	TUNELESS_CTRL	*TuneLessCtrl;
	PRMDATA			*Prm;
	REGIF_CONFIG_T	*RegCfg;

	TuneLessCtrl = &(Axis->BaseControls->TuneLessCtrl);
	Prm = Axis->UniPrms->Prm;
	RegCfg = Axis->RegManager->hPnReg;
	rc = PRM_RSLT_SUCCESS;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr = 0;
	AccessPrm.hRegCfg = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

	/* Set Auto Notch Status in Register */
	RpiSetRegSpatANotchState( Axis->ANotchSeq );

	switch( FunExe->State )
	{
	case DAT_SEQ_INIT:
		/* Fn200実行中フラグON */
		TuneLessCtrl->var.TuningFuncRun = TRUE;
		FunExe->State = DAT_SEQ_TUNING;
		break;

	case DAT_SEQ_TUNING:	/* ゲインレベル変更処理	*/
		switch( FunExe->CmdCode )
		{
		case FCMD_UP:		/* チューニングレベル値UP		*/
			if( TuneLessCtrl->OpeReg.DatGnLevel < DAT_GAINLEVEL_MAX )
			{
				TuneLessCtrl->OpeReg.DatGnLevel++;
			}
			else
			{
				TuneLessCtrl->OpeReg.DatGnLevel = DAT_GAINLEVEL_MAX;
			}
			/*------------------------------------------------------------------------------------------*/
			/*	チューニングレベル設定																	*/
			/*------------------------------------------------------------------------------------------*/
			setTLLvlSetLevel( &(AccessPrm),
							  TuneLessCtrl->OpeReg.DatGnLevel,
							  TuneLessCtrl->OpeReg.DatJratLevel,
							  Prm->DatLevel );

			/*------------------------------------------------------------------------------------------*/
			/*	ノッチ設定読込み																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	自動設定ノッチ解除処理																	*/
			/*------------------------------------------------------------------------------------------*/
			if( Prm->anotchsw & 0x0F00 )						/* 自動設定ノッチを抽出					*/
			{
				ulwork &= 0x0000F0FF;							/* 第2ノッチOFF							*/
			}
			RpiResetStatusANotch1( Axis->ANotchSeq );			/* 関連ステータスリセット				*/
			RpiResetStatusANotch2( Axis->ANotchSeq );			/* 関連ステータスリセット				*/
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );

			/*------------------------------------------------------------------------------------------*/
			/*	トルクフィルタ計算																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	振動レベルクリア																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_DOWN:		/* ゲインレベル値DOWN		*/
			if( TuneLessCtrl->OpeReg.DatGnLevel > DAT_GAINLEVEL_MIN )
			{
				TuneLessCtrl->OpeReg.DatGnLevel--;
			}
			else
			{
				TuneLessCtrl->OpeReg.DatGnLevel = DAT_GAINLEVEL_MIN;
			}
			/*------------------------------------------------------------------------------------------*/
			/*	チューニングレベル設定																	*/
			/*------------------------------------------------------------------------------------------*/
			setTLLvlSetLevel( &(AccessPrm),
							  TuneLessCtrl->OpeReg.DatGnLevel,
							  TuneLessCtrl->OpeReg.DatJratLevel,
							  Prm->DatLevel );

			/*------------------------------------------------------------------------------------------*/
			/*	ノッチ設定読込み																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	自動設定ノッチ解除処理																	*/
			/*------------------------------------------------------------------------------------------*/
			if( Prm->anotchsw & 0x0F00 )						/* 自動設定ノッチを抽出					*/
			{
				ulwork &= 0x0000F0FF;							/* 第2ノッチOFF							*/
			}
			RpiResetStatusANotch1( Axis->ANotchSeq );			/* 関連ステータスリセット				*/
			RpiResetStatusANotch2( Axis->ANotchSeq );			/* 関連ステータスリセット				*/
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );

			/*------------------------------------------------------------------------------------------*/
			/*	トルクフィルタ計算																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	振動レベルクリア																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_STOP:		/* 停止	→ ノッチ強制OFF	*/
			/*------------------------------------------------------------------------------------------*/
			/*	ノッチ設定読込み																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	自動設定ノッチ解除処理																	*/
			/*------------------------------------------------------------------------------------------*/
			switch( RpiGetANotchPreStat(Axis->ANotchSeq) )
			{ /* ノッチ完了ステータス取得 */
				case OPTUNE_PSTS_NT1_COMP:							/* 前回1段目ノッチON				*/
					ulwork &= 0x0000FFF0;							/* 第1ノッチOFF						*/
					RpiResetStatusANotch1( Axis->ANotchSeq );		/* 関連ステータスリセット			*/
					break;

				case OPTUNE_PSTS_NT2_COMP:							/* 前回2段目ノッチON				*/
					ulwork &= 0x0000F0FF;							/* 第2ノッチOFF						*/
					RpiResetStatusANotch2( Axis->ANotchSeq );		/* 関連ステータスリセット			*/
					break;

				default :
					break;
			}
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );

			/*------------------------------------------------------------------------------------------*/
			/*	トルクフィルタ計算																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	振動レベルクリア																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );

			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_EXEC:		/* 実行　→　レベル変更		*/
			/*------------------------------------------------------------------------------------------*/
			/*	チューニングレベル設定																	*/
			/*------------------------------------------------------------------------------------------*/
			if( PRM_RSLT_SUCCESS != setTLLvlSetLevel(&(AccessPrm),
													 TuneLessCtrl->OpeReg.DatGnLevel,
													 TuneLessCtrl->OpeReg.DatJratLevel,
													 Prm->DatLevel) )
			{
				rc = PRM_RSLT_CALC_ERR;
			}

			/*------------------------------------------------------------------------------------------*/
			/*	ノッチ設定読込み																		*/
			/*------------------------------------------------------------------------------------------*/
			ulwork = (ULONG)Prm->tfuncsw;

			/*------------------------------------------------------------------------------------------*/
			/*	自動設定ノッチ解除処理																	*/
			/*------------------------------------------------------------------------------------------*/
			if( Prm->anotchsw & 0x0F00 )						/* 自動設定ノッチを抽出					*/
			{
				ulwork &= 0x0000F0FF;							/* 第2ノッチOFF							*/
			}
			RpiResetStatusANotch1( Axis->ANotchSeq );			/* 関連ステータスリセット				*/
			RpiResetStatusANotch2( Axis->ANotchSeq );			/* 関連ステータスリセット				*/
//			rc = RpiPrmWriteRamForFn( &pndef_tfuncsw, (USHORT)ulwork, pfexe->App.Hmsg );
			rc = TunePrmWriteRam( &AccessPrm, &pndef_tfuncsw, (LONG)ulwork, 2 );
			if( rc != PRM_RSLT_SUCCESS )
			{
				rc = PRM_RSLT_CALC_ERR;
			}

			/*------------------------------------------------------------------------------------------*/
			/*	トルクフィルタ計算																		*/
			/*------------------------------------------------------------------------------------------*/
			RpiANotchTrqFilCalc( Axis->ANotchSeq, Axis->ANotch, RegCfg, Prm );

			/*------------------------------------------------------------------------------------------*/
			/*	振動レベルクリア																		*/
			/*------------------------------------------------------------------------------------------*/
			DetVibResetMaxMinSpeedError( Axis->DetVib );
			break;

		case FCMD_EXEC2 :	/* 実行2				*/
		case FCMD_SV :		/* サーボオンキー		*/
			/* 強制周波数解析処理要求セット */
			RpiANotchFftReqSet( Axis->ANotchSeq, Axis->FftAna );
			rc = PRM_RSLT_CONTINUE;
			break;

		case FCMD_EWRT:		/* EEPROM書込み			*/
			if( setTLLvlUpdateInEeprom(RegCfg) != PRM_RSLT_SUCCESS )
			{ /* EEPROM書込み失敗時 */
				rc = PRM_RSLT_CALC_ERR;
			}
			break;

		case FCMD_NONE :
			break;

		default:
			rc = PRM_RSLT_CMD_ERR;
			break;
		}
		break;

	default:
		break;
	}
	return(rc);
}


/****************************************************************************************************/
/*		ゲインレベル(Pn170.2,Pn170.3の値)処理 														*/
/****************************************************************************************************/
void	RpiRegSetTuneLessLevel( ULONG Gain, TUNELESS_CTRL *TuneLessCtrl )
{
	LONG	GainLevel;
	LONG	JratLevel;

	/*------------------------------------------------------------------------------------------*/
	/*	レベル設定取得																			*/
	/*------------------------------------------------------------------------------------------*/
	GainLevel = (Gain >> 8) & 0x0F;
	JratLevel = (Gain >> 12) & 0x0F;

	/*------------------------------------------------------------------------------------------*/
	/*	ゲインレベル上下限チェック																*/
	/*------------------------------------------------------------------------------------------*/
	if( GainLevel > DAT_GAINLEVEL_MAX )
	{
		GainLevel = DAT_GAINLEVEL_MAX;
	}
	else if( GainLevel < DAT_GAINLEVEL_MIN )
	{
		GainLevel = DAT_GAINLEVEL_MIN;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	負荷レベル上下限チェック																*/
	/*------------------------------------------------------------------------------------------*/
	if( JratLevel > DAT_JRATLEVEL_MAX )
	{
		JratLevel = DAT_JRATLEVEL_MAX;
	}
	else if( JratLevel < DAT_JRATLEVEL_MIN )
	{
		JratLevel = DAT_JRATLEVEL_MIN;
	}

	/*------------------------------------------------------------------------------------------*/
	/*	レジスタ設定																			*/
	/*------------------------------------------------------------------------------------------*/
	TuneLessCtrl->OpeReg.DatGnLevel = (USHORT)GainLevel;
	TuneLessCtrl->OpeReg.DatJratLevel = (USHORT)JratLevel;

	return;
}


/* -------- Fn200 Check the Condition for Setting Tuning Less Level	-------- */
static PRM_RSLT	setTLLvlChkCondition( TUNELESS_CTRL *TuneLessCtrl )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/* 調整レス機能無効時は、データ異常とする */
	if( TuneLessCtrl->conf.TuningLessUse != TRUE )
	{
		rc = PRM_RSLT_CONDITION_ERR;
	}
	return rc;
}



/* -------- Fn200 Update Parmeter In EEPROM -------- */
static PRM_RSLT	setTLLvlUpdateInEeprom( REGIF_CONFIG_T *RegCfg )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/* Pn170 ダイナミックオートチューニング関連スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_DatLevel, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn401 １段目第１トルク指令フィルタ時定数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_trqfil11, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn408 トルク関係機能スイッチ */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_tfuncsw, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn409 １段目ノッチフィルタ周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn40A １段目ノッチフィルタＱ値 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq1, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn40C ２段目ノッチフィルタ周波数 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchfil2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}
	/* Pn40D ２段目ノッチフィルタＱ値 */
	rc = PrmStoreValueToEeprom( RegCfg, &pndef_ntchq2, 0 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	return(rc);
}





/* -------- Fn200 Write Prm To Ram With EEPROM Data	-------- */
static void	setTLLvlUndoRamGain( REGIF_CONFIG_T *RegCfg )
{
	/* RAMの値をEEPROMの値と同じにする */
	/* Pn170 ダイナミックオートチューニング関連スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_DatLevel, 0 );
	/* Pn401 １段目第１トルク指令フィルタ時定数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_trqfil11, 0 );
	/* Pn408 トルク関係機能スイッチ */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_tfuncsw, 0 );
	/* Pn409 １段目ノッチフィルタ周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil1, 0 );
	/* Pn40A １段目ノッチフィルタＱ値 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq1, 0 );
	/* Pn40C ２段目ノッチフィルタ周波数 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchfil2, 0 );
	/* Pn40D ２段目ノッチフィルタＱ値 */
	PrmReLoadValueFromEeprom( RegCfg, &pndef_ntchq2, 0 );

	return;
}


/****************************************************************************************************/
/*		調整レベルのパラメータ設定処理 																*/
/****************************************************************************************************/
PRM_RSLT	setTLLvlSetLevel( REG_ACC_T *AccessPrm, USHORT GainLevel, USHORT JratLevel, USHORT DetLevel )
{
	PRM_RSLT	rc;
	ULONG		uswk, uswk2;
	ULONG		ulwk;

	rc = PRM_RSLT_SUCCESS;

	uswk = (GainLevel << 8);
	uswk2 = (JratLevel << 12);
	ulwk = (ULONG)DetLevel & 0xFFFF00FF;
	ulwk |= (ULONG)uswk + (ULONG)uswk2;
//	rc = RpiPrmWriteRamForFn( &pndef_DatLevel, ulwk, Hmsg );
	rc = TunePrmWriteRam( AccessPrm, &pndef_DatLevel, ulwk, 2 );
	if( rc != PRM_RSLT_SUCCESS )
	{
		return(rc);
	}

	return(rc);
}



/****************************************************************************************************/
/*		ゲインレベル設定処理 (レジスタIF)															*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetDATGainLevel( TUNELESS_CTRL *TuneLessCtrl, USHORT Level )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/*------------------------------------------------------------------------------------------*/
	/*	ゲインレベル上下限チェック																*/
	/*------------------------------------------------------------------------------------------*/
	if( ((LONG)Level >= DAT_GAINLEVEL_MIN) && ((LONG)Level <= DAT_GAINLEVEL_MAX) )
	{
		TuneLessCtrl->OpeReg.DatGnLevel = Level;
	}
	else
	{
		TuneLessCtrl->OpeReg.DatGnLevel = DAT_GAINLEVEL_MIN;
		rc = PRM_RSLT_REGNO_ERR;
	}
	return( rc );
}


/****************************************************************************************************/
/*		負荷レベル設定処理 (レジスタIF)																*/
/****************************************************************************************************/
PRM_RSLT	RpiFunSetDATJratLevel( TUNELESS_CTRL *TuneLessCtrl, USHORT Level )
{
	PRM_RSLT	rc;
	rc = PRM_RSLT_SUCCESS;

	/*------------------------------------------------------------------------------------------*/
	/*	負荷レベル上下限チェック																*/
	/*------------------------------------------------------------------------------------------*/
	if( ((LONG)Level >= DAT_JRATLEVEL_MIN) && ((LONG)Level <= DAT_JRATLEVEL_MAX) )
	{
		TuneLessCtrl->OpeReg.DatJratLevel = Level;
	}
	else
	{
		TuneLessCtrl->OpeReg.DatJratLevel = DAT_JRATLEVEL_MIN;
		rc = PRM_RSLT_REGNO_ERR;
	}
	return( rc );
}



/****************************************************************************************************/
