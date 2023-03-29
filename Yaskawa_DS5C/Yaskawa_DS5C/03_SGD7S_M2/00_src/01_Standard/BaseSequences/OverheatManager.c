/****************************************************************************************************/
/*																									*/
/*																									*/
/*		OverheatManager.c : 温度過熱監視処理定義					<S0ED>							*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 取得した温度を監視し、温度過熱アラーム/ワーニングを出力する。							*/
/*																									*/
/*			また、Un表示する温度データの作成や、設置環境モニタ値を計算し、APIにて情報提供を			*/
/*			行う。																					*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2014.1.22	Y.Oka	For Mercury												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include	"OverheatManager.h"
#include	"Mlib.h"



/****************************************************************************************************/
/*		API's																						*/
/****************************************************************************************************/
/*	void	OhmInitialize( CHECK_OH* pCheckOh );													*/
/*	void	EnvMonInitialize( ENVMON* pEnvMon );													*/
/*	void	OhmCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,							*/
/*														SHORT CtrlBoadTemp, SHORT PowerBoadTemp );	*/
/*	void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt,					*/
/*																		USHORT PowerBoadErrCnt );	*/
/*	void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp );						*/
/****************************************************************************************************/



/****************************************************************************************************/
/*		Static Function Definition																	*/
/****************************************************************************************************/
static LONG	AdetCheckFirstBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature );
static LONG	AdetCheckBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature );
static void	AdetSetBoardTmprAlm( ALMMAN_HANDLE *AlmMngr, LONG DetectAlm, LONG BoardSel );
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		オーバヒート監視モジュール初期化										<S03A>				*/
/*																									*/
/****************************************************************************************************/
void OhmInitialize( CHECK_OH* pCheckOh )
{
	MlibResetLongMemory( &pCheckOh->var, sizeof( pCheckOh->var )/4 );
	MlibResetLongMemory( &pCheckOh->CtrlTmprAlm.var, sizeof( pCheckOh->CtrlTmprAlm.var )/4 );
	MlibResetLongMemory( &pCheckOh->PwrTmprAlm.var, sizeof( pCheckOh->PwrTmprAlm.var )/4 );
	MlibResetLongMemory( &pCheckOh->var, sizeof( pCheckOh->var )/4 );
}



/****************************************************************************************************/
/*																									*/
/*		設置環境モニタ初期化													<S03A>				*/
/*																									*/
/****************************************************************************************************/
void EnvMonInitialize( ENVMON* pEnvMon )
{
	MlibResetLongMemory( &pEnvMon->var, sizeof( pEnvMon->var )/4 );

}



/****************************************************************************************************/
/*																									*/
/*		サーボパックオーバヒート監視											<S03A>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 温度検出ICからの温度情報を監視し、破壊・動作不良を防ぐ。								*/
/*																									*/
/*	制御電源投入直後はOHアラームクリア可能温度との比較を行い、超えている場合は即アラームとする。	*/
/*	初期状態は1秒間継続し、その後通常の温度監視へ移行する。通常の温度監視時はワーニングレベルに		*/
/*	応じてワーニング出力を行い、その後継続して使用した場合にはアラームへと移行する。				*/
/*																									*/
/****************************************************************************************************/
void	OhmCheckOverHeatAlm( ALMMAN_HANDLE *AlmMngr, CHECK_OH *CheckOh,
														SHORT CtrlBoadTemp, SHORT PowerBoadTemp )
{
//	SHORT	MaxTemp;	/* <S050> */
	LONG	DetAlm;		/* <S0DE> */

	/*----------------------------------------------------------------------------------------------*/
	/*	内気温関連Unモニタデータ作成											<S0DE>		 		*/
	/*----------------------------------------------------------------------------------------------*/
	/* 温度検出値加算 */
	CheckOh->var.CtrlBoadTempSum += CtrlBoadTemp;
	CheckOh->var.PowerBoadTempSum += PowerBoadTemp;
	if( ++CheckOh->var.TemperatureSumCnt >= 10 )
	{
		/* Un170 : 平均内気温度[0.01deg] */
		CheckOh->UnMon.UnAverageTemp =
				((CheckOh->var.CtrlBoadTempSum + CheckOh->var.PowerBoadTempSum) >> 1);
		/* Un171 : 制御基板周囲温度[0.1deg] */
		CheckOh->UnMon.UnCtrlBoadTemp = (SHORT)(CheckOh->var.CtrlBoadTempSum * 0.1f);
		/* Un172 : パワー基板周囲温度[0.1deg] */
		CheckOh->UnMon.UnPowerBoadTemp = (SHORT)(CheckOh->var.PowerBoadTempSum * 0.1f);
		/* 初期化 */
		CheckOh->var.CtrlBoadTempSum = 0;
		CheckOh->var.PowerBoadTempSum = 0;
		CheckOh->var.TemperatureSumCnt = 0;
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	オーバーヒート監視														<S0DE>		 		*/
	/*----------------------------------------------------------------------------------------------*/
	if( CheckOh->var.DetTempFirstPass == FALSE )
	{ /* 温度検出初回 */
		/* 制御基板温度異常アラームチェック処理 */
		DetAlm = AdetCheckFirstBoardTmpr( &(CheckOh->CtrlTmprAlm), CtrlBoadTemp );
		/* 温度異常アラーム設定処理 */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, CTRL_BOARD );

		/* パワー基板温度異常アラームチェック処理 */
		DetAlm = AdetCheckFirstBoardTmpr( &(CheckOh->PwrTmprAlm), PowerBoadTemp );
		/* 温度異常アラーム設定処理 */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, POWER_BOARD );

		/* 経過時間カウントアップ */
		CheckOh->var.DetTempFistPastTime++;
		if( CheckOh->var.DetTempFistPastTime > KPI_SCANC_MS(1000) )
		{
			/* 初期監視通過フラグセット */
			CheckOh->var.DetTempFirstPass = TRUE;
		}
	}
	else
	{ /* 通常時 */
		/* 制御基板温度異常アラームチェック処理 */
		DetAlm = AdetCheckBoardTmpr( &(CheckOh->CtrlTmprAlm), CtrlBoadTemp );
		/* 温度異常アラーム設定処理 */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, CTRL_BOARD );

		/* パワー基板温度異常アラームチェック処理 */
		DetAlm = AdetCheckBoardTmpr( &(CheckOh->PwrTmprAlm), PowerBoadTemp );
		/* 温度異常アラーム設定処理 */
		AdetSetBoardTmprAlm( AlmMngr, DetAlm, POWER_BOARD );
	}

	return;
}



/*****************************************************************************
  Description: 内部基板温度異常アラームチェック初回処理				<S0DE>
------------------------------------------------------------------------------
  Parameters:
	IntTempAlm				内部温度異常
	Temperature				基板温度情報
  Return:
	rc						戻り値
------------------------------------------------------------------------------
  Note:
	内部基板温度異常アラームを監視する
*****************************************************************************/
static LONG	AdetCheckFirstBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature )
{
	LONG	rc;
	rc = CHK_OVERHEAT_NO_OPERATION;

	if( Temperature > IntTmprAlm->conf.BrdAlmClrTmpr )
	{ /* 内部基板温度が異常クリア可能温度を超えている */
//		if( IntTmprAlm->var.DirectAlmDetTime > KPI_SCANC_MS( 500 ) ) /* <S1D1> */
		if( IntTmprAlm->var.AlmDetectTimer > KPI_SCANC_MS( 10000 ) ) /* <S1D1> */
		{
			IntTmprAlm->var.BrdState = BRD_ALARM_DURING;
			rc = CHK_OVERHEAT_SET_ALARM;
		}
		else
		{
//			IntTmprAlm->var.BrdState++;				/* <S109> */
			IntTmprAlm->var.AlmDetectTimer++;		/* <S109> */
		}
	}
	else
	{
//		IntTmprAlm->var.BrdState = 0;				/* <S109> */
		IntTmprAlm->var.AlmDetectTimer = 0;			/* <S109> */
	}
	return( rc );
}



/*****************************************************************************
  Description: 内部基板温度異常アラームチェック処理					<S0DE>
------------------------------------------------------------------------------
  Parameters:
	IntTempAlm				内部温度異常
	Temperature				基板温度情報
  Return:
	rc						戻り値
------------------------------------------------------------------------------
  Note:
	内部基板温度異常アラームを監視する
*****************************************************************************/
static LONG	AdetCheckBoardTmpr( CHECK_INTTMPR *IntTmprAlm, SHORT Temperature )
{
	LONG	rc;
	rc = CHK_OVERHEAT_NO_OPERATION;

	switch ( IntTmprAlm->var.BrdState )
	{
	case BRD_TEMP_NORMAL:
		/* 内部温度が警告温度を超えた場合 */
		if( Temperature > IntTmprAlm->conf.BrdWrnTmpr )
		{
//			if( IntTmprAlm->var.WrnDetectTimer > KPI_SCANC_MS( 500 ) ) /* <S11D> */
			if( IntTmprAlm->var.WrnDetectTimer > KPI_SCANC_MS( 10000 ) )
			{
				rc = CHK_OVERHEAT_SET_WARNING;
//				IntTmprAlm->var.IntTmprTimeCnt = 0U;					/* <S1D1> */
				IntTmprAlm->var.BrdState = BRD_WRN_DURING;
			}
			else
			{
				IntTmprAlm->var.WrnDetectTimer++;
			}
		}
		else
		{
			IntTmprAlm->var.WrnDetectTimer = 0;
		}
		break;

	case BRD_WRN_DURING:
//		IntTmprAlm->var.IntTmprTimeCnt++; /* <S1D1> */
		/* ワーニング状態が規定の時間を超えた場合、また異常検出の閾値温度を超えた場合、またアラーム発生 *//* <S1D1> */
//		if( (IntTmprAlm->var.IntTmprTimeCnt >= (IntTmprAlm->conf.BrdAlmDetTime * KPI_SCANC_MS( 1000 )))
//			|| (Temperature > IntTmprAlm->conf.BrdAlmTmpr) )
		if( (Temperature > IntTmprAlm->conf.BrdAlmTmpr) )
		{
			if( IntTmprAlm->var.AlmDetectTimer > KPI_SCANC_MS( 10000 ) )/* <S1D1> */
			{
				rc = CHK_OVERHEAT_SET_ALARM;
//				IntTmprAlm->var.IntTmprTimeCnt = 0U;					/* <S1D1> */
				IntTmprAlm->var.BrdState = BRD_ALARM_DURING;
			}
			else
			{
				IntTmprAlm->var.AlmDetectTimer++;
			}
		}
		/* 内部温度が警告温度を下回った場合 */
		else if( Temperature < IntTmprAlm->conf.BrdWrnTmpr )
		{
			rc = CHK_OVERHEAT_CLR_WARNING;
//			IntTmprAlm->var.IntTmprTimeCnt = 0U;						/* <S1D1> */
			IntTmprAlm->var.BrdState = BRD_TEMP_NORMAL;
		}
		else
		{
			rc = CHK_OVERHEAT_SET_WARNING;								/* <S1D1> */
			IntTmprAlm->var.AlmDetectTimer = 0;							/* <S1D1> */
		}
		break;

	case BRD_ALARM_DURING:
		/* 内部基板温度が異常クリア可能温度を超えている */
		if( Temperature > IntTmprAlm->conf.BrdAlmClrTmpr )
		{
			rc = CHK_OVERHEAT_SET_ALARM;
		}
		else
		{
			IntTmprAlm->var.BrdState = BRD_TEMP_NORMAL;
		}
		break;

	default:
		IntTmprAlm->var.BrdState = BRD_TEMP_NORMAL;
//		IntTmprAlm->var.IntTmprTimeCnt = 0U;							/* <S1D1> */
		break;

	}
	return( rc );
}



/*****************************************************************************
  Description: 内部温度異常アラーム設定処理							<S0DE>
------------------------------------------------------------------------------
  Parameters:
	AlmMngr					アラーム情報
	DetectAlm				アラーム出力判定
	BoardSel				基板セレクト
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
static void	AdetSetBoardTmprAlm( ALMMAN_HANDLE *AlmMngr, LONG DetectAlm, LONG BoardSel )
{
	if( BoardSel == CTRL_BOARD )
	{
		if( DetectAlm != CHK_OVERHEAT_NO_OPERATION )
		{
			if( DetectAlm == CHK_OVERHEAT_SET_ALARM )
			{
				ALMClearGlobalAlarm( WRN_CTRLBRD );			/* ワーニング(A.912)解除 *//* <S11D> */
				ALMSetGlobalAlarm( ALM_CTRLBRD );			/* アラーム(A.7A1)を出力する *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_SET_WARNING )
			{
				ALMSetGlobalAlarm( WRN_CTRLBRD );			/* ワーニング(A.912)を出力する *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_CLR_WARNING )
			{
				ALMClearGlobalAlarm( WRN_CTRLBRD );			/* ワーニング(A.912)解除 *//* <S11D> */
			}
			else
			{
				/* do nothing */
			}
		}
	}
	else if( BoardSel == POWER_BOARD )
	{
		if( DetectAlm != CHK_OVERHEAT_NO_OPERATION )
		{
			if( DetectAlm == CHK_OVERHEAT_SET_ALARM )
			{
				ALMClearGlobalAlarm(WRN_PWRBRD );				/* ワーニング(A.913)解除 *//* <S11D> */
				ALMSetGlobalAlarm( ALM_PWRBRD );			/* アラーム(A.7A2)を出力する *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_SET_WARNING )
			{
				ALMSetGlobalAlarm( WRN_PWRBRD );			/* ワーニング(A.913)を出力する *//* <S11D> */
			}
			else if( DetectAlm == CHK_OVERHEAT_CLR_WARNING )
			{
				ALMClearGlobalAlarm( WRN_PWRBRD );				/* ワーニング(A.913)解除 *//* <S11D> */
			}
			else
			{
				/* do nothing */
			}
		}
	}
	else
	{
		/* do nothing */
	}
}



/****************************************************************************************************/
/*																									*/
/*		サーボパック温度検出部監視												<S0DE>				*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 温度検出ICの通信エラーを監視する。														*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	AdetCheckTempDeviceErr( ALMMAN_HANDLE *AlmMngr, USHORT CtrlBoadErrCnt, USHORT PowerBoadErrCnt )
{
	if( (CtrlBoadErrCnt >= 3) || (PowerBoadErrCnt >= 3) )
	{
		ALMSetServoAlarm( AlmMngr, WRN_PWRBRD );			/* ワーニング(A.913)を出力する			*/
	}
	return;
}



/****************************************************************************************************/
/*																									*/
/*		アンプ＆モータ設置環境モニタ演算処理									<S03A>				*/
/*																									*/
/****************************************************************************************************/
void	AdetInstEnvMonitor( ENVMON *EnvMon, SHORT AmpTemp, SHORT MotTemp )
{
	EnvMon->var.AmpEnvMonSum += AmpTemp; /* [0.1℃] */
	EnvMon->var.MotEnvMonSum += MotTemp; /* [℃] */

	EnvMon->var.EnvMonSumCnt2s++;
	if( EnvMon->var.EnvMonSumCnt2s >= 1000 )
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	設置環境モニタ値の計算(モニタ値更新:2000ms周期)												*/
	/*----------------------------------------------------------------------------------------------*/
		/* Un025 : アンプ設置環境モニタ [0.001%] */
		EnvMon->var.UnInstEnvMonAmp = EnvMon->var.AmpEnvMonSum;
		/* Un026 : モータ設置環境モニタ [0.001%] */
		EnvMon->var.UnInstEnvMonMot = EnvMon->var.MotEnvMonSum;
		/* 初期化 */
		EnvMon->var.AmpEnvMonSum = 0;
		EnvMon->var.MotEnvMonSum = 0;
		EnvMon->var.EnvMonSumCnt2s = 0;
	}
}


/***************************************** end of file **********************************************/
