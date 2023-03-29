/*********************************** Start of file **************************************************/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SftDiagFunc.c		: Safety Diagnostic function for Sigma-7(SGD7S/W)						*/
/*																									*/
/*		Identification																				*/
/*			Module ID		: 0002(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Module version	: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.03.14															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S1FF><S201>																*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SequenceIo.h"
#include "SftDiagFunc.h"
#include "HwDeviceIf.h"
#include "BaseEnableSequence.h"/*<S201>*/


/****************************************************************************************************/
/*																									*/
/*		セーフティ診断部初期化処理																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : --セーフティ診断部初期化																*/
/*			・PnE09.bit8に応じてHWBBサポートの有無を設定する										*/
/*			・PnE40の値をテストパルス異常アラーム検出レベルとして設定する							*/
/*																									*/
/*	引 数 : USHORT			opdetsw			:PnE09.bit8												*/
/* 			DET_HWBB_STS	*DetHwbbSts		:HWBBステータス管理構造体								*/
/*			USHORT			tplsfil			:PnE40設定値											*/
/*			TEST_PULSE_DIAG *TestPulseDiag	:テストパルス診断管理構造体								*/
/*　戻り値: なし																					*/
/*																									*/
/****************************************************************************************************/
void SysInitSafetyDevice( USHORT opdetsw, DET_HWBB_STS *DetHwbbSts, USHORT tplsfil, TEST_PULSE_DIAG *TestPulseDiag )
{
	LONG i;
	if( (opdetsw & 0x0100) == 0x0000 )
	{
		DetHwbbSts->hwbbsupport = FALSE;
	}
	else
	{
		DetHwbbSts->hwbbsupport = TRUE;
	}

	for(i=0;i<2;i++)
	{
		TestPulseDiag->DpInErrLvl = tplsfil;
		TestPulseDiag++;
	}
}


/****************************************************************************************************/
/*																									*/
/*		入力信号タイミング異常チェック処理															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : BBON1LT/BBON2LT信号がアクティブになる時間差がパラメータ設定値以上の時間差の場合に		*/
/*			アラームとする。これは、２重化信号の異常（ハードウェアの故障、入力信号の断線）を		*/
/*			検出するための処置である。本処理では、セーフティオプションカードの有無により			*/
/*			アラームコード（ｶｰﾄﾞ有/無=A.Eb2/A.Eb1）およびアラーム検出に用いる設定値					*/
/*			（カード有/無= PnE4C/Pn620）を変更する。												*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetCheckBbonSignal( CHECK_HWBB_TIMING *pHwbbTmg, BOOL LackOneBbon )
{
	BOOL err;

	err = FALSE;
	/*--------------------------------------------------------------------------------------------------*/
	/*		BBON信号入力タイミング異常チェック処理														*/
	/*--------------------------------------------------------------------------------------------------*/
		if( LackOneBbon )
		{
			pHwbbTmg->var.BbonTimeDiffCnt += KPI_SCCYCLEMS;
			if( pHwbbTmg->var.BbonTimeDiffCnt > pHwbbTmg->conf.MaxBbonTimeDiff )
			{
				/* 上限を設定値でクランプ */
				pHwbbTmg->var.BbonTimeDiffCnt = pHwbbTmg->conf.MaxBbonTimeDiff;

				/* A.Eb1 : 停止信号入力時間差過大(カード無) */
				err = TRUE;
			}
		}
		else		/* <S0E5> */
		{
			pHwbbTmg->var.BbonTimeDiffCnt = 0;
		}
		return err;
}


/****************************************************************************************************/
/*																									*/
/*		セーフティＨＷＢＢ状態検出処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : --ハードワイヤードベースブロック状態（HWBB状態）の検出								*/
/*			BBON1LT/BBON2LT信号のどちらかもしくは両方が1となった場合（HWBB状態中）に、				*/
/*			HWBB状態を示すフラグにTRUEをセットする。両方の信号が0の場合（HWBB状態解除時）には、		*/
/*			HWBB状態を示すフラグにFALSEをセットする。また、HWBB状態解除時には入力エッジ検出結果		*/
/*			もクリアする。																			*/
/*			BBONLT信号のラッチ値をクリアするタイミングは、サーボオフ入力かつラウンド運転権を		*/
/*			取得していない場合に行う。																*/
/*			--BBON信号入力時間差異常アラーム検出用のフラグセット									*/
/*			BBON1LT/BBON2LT信号のどちらかの入力エッジを検出した際に、入力時間差フラグに				*/
/*			TRUEをセットする。BBON1/BBON2信号の両方の入力エッジを検出した際に、入力時間差フラグに	*/
/*			FALSEをセットする。両方の入力エッジを検出できない場合は、前回の状態を保持する。			*/
/*																									*/
/*	引 数 : なし																					*/
/*　戻り値: なし																					*/
/*																									*/
/****************************************************************************************************/
BOOL SeqIoDetectHardWiredBB( DET_HWBB_STS *DetHwbb, LONG svon_req,
										ASIC_HW_IF_STR *pAsicHwReg, BOOL HwbbSupport )
{
	BOOL	Bbon1Lt, Bbon2Lt;
	BOOL	HwbbSts;

/*--------------------------------------------------------------------------------------------------*/
/*		ハードワイヤードベースブロック状態チェック													*/
/*--------------------------------------------------------------------------------------------------*/
	/* サーボオフ入力かつラウンド運転権の取得なしかつ磁極検出要求なし */
//	if( (0 == (CmdSeqBit & SERVO_ON_BIT)) && (0 == (CmdSeqBit & POL_DETECT_BIT)) && (!FnSvControl) )
	if(svon_req == 0)
	{
		/* BBON信号のラッチ値をクリア */
		SHalDrv_ResetBBONLtStatus( pAsicHwReg );
	}

	/* 現在のBBON信号状態読み込み（モニタ用） */
	SHalDrv_GetBBONStatus( pAsicHwReg, &DetHwbb->Bbon1, &DetHwbb->Bbon2 );

	/* 現在のBBON信号ラッチ値読み込み（HWBB検出用） */
	SHalDrv_GetBBONLtStatus( pAsicHwReg, &Bbon1Lt, &Bbon2Lt );

	/* BBON信号のエッジ検出 */
	if( Bbon1Lt )
	{
		DetHwbb->Bbon1Edge = TRUE;
	}
	if( Bbon2Lt )
	{
		DetHwbb->Bbon2Edge = TRUE;
	}

	/* HWBB状態設定処理 */
	if( (Bbon1Lt) || (Bbon2Lt) )
	{
		/* BBON信号どちらかでも入力あり */
		if( HwbbSupport == TRUE )
		{ /* HWBB機能対応機種 */
			DetHwbb->HwbbState = TRUE;
			DetHwbb->GateDrvDefect = FALSE;
		}
		else
		{ /* HWBB機能未対応機種 */
			DetHwbb->HwbbState = FALSE;
			DetHwbb->GateDrvDefect = TRUE;
		}
	}
	else
	{
		/* BBON信号どちらも入力なし */
		if( (DetHwbb->Bbon1Edge) && (DetHwbb->Bbon2Edge) )
		{
			/* BBON1/BBON2信号のエッジを両方検出した時のみHWBB状態解除 */
			DetHwbb->HwbbState = FALSE;
			DetHwbb->Bbon1Edge = FALSE;
			DetHwbb->Bbon2Edge = FALSE;
		}
	}

	/* Set ScanC Output Data */
	if( DetHwbb->HwbbState )
	{
		HwbbSts = TRUE;
	}
	else
	{
		HwbbSts = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		入力信号の時間差異常検出タイマスタートフラグの設定											*/
/*--------------------------------------------------------------------------------------------------*/
	/* BBON信号入力時間差フラグ設定処理 */
	if( (DetHwbb->Bbon1Edge) ^ (DetHwbb->Bbon2Edge) )
	{
		/* BBON信号どちらかのみの入力 */
		DetHwbb->LackOneBbon = TRUE;
	}
	else
	{
		/* BBON信号両方の入力 */
		if( (DetHwbb->Bbon1Edge) && (DetHwbb->Bbon2Edge) )
		{
			DetHwbb->LackOneBbon = FALSE;
		}
	}

	return	HwbbSts;
}



/****************************************************************************************************/
/*																									*/
/*		セーフティ機能　内部信号異常検出処理 (Test Pulse Diagnostic)								*/
/*																									*/
/*		Identification																				*/
/*			Function ID		: 0001(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Function version: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.02.14															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S1FF><S201>																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : アンプ側の内部信号(DPIN1/DPIN2信号)がパルス信号でない場合は異常検出する					*/
/*			HWBB状態の場合は異常検出しない															*/
/*																									*/
/****************************************************************************************************/
BOOL	AdetTestPulseDiagnostic( TEST_PULSE_DIAG *TestPulseDiag )	/*<S1D1>*/
{
	BOOL err;

	err = FALSE;

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN信号チェック処理（BBON=OFF：HWBB状態はチェックしない）									*/
/*--------------------------------------------------------------------------------------------------*/
	if( TestPulseDiag->DpInEnable )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	DPIN信号が4ms周期のパルス信号でない場合は異常											*/
		/*------------------------------------------------------------------------------------------*/
		if( !(TestPulseDiag->DpInState^TestPulseDiag->DpInLastState) )
		{
			TestPulseDiag->DpInDiagState = FALSE;
			if( !((TestPulseDiag->DpInDiagState)^(TestPulseDiag->DpInLastDiagState)) )
			{
				TestPulseDiag->DpInErrCnt++;
				if( (TestPulseDiag->DpInErrCnt) > 2 )
				{
					err = TRUE;
				}
			}
			else
			{
				TestPulseDiag->DpInErrCnt = 0;
			}
		}
		else
		{
			TestPulseDiag->DpInDiagState = TRUE;
			TestPulseDiag->DpInErrCnt = 0;
		}
		TestPulseDiag->DpInLastDiagState = TestPulseDiag->DpInDiagState;
	}
/* <S1D1> >>>>> */
	else if( !(TestPulseDiag->DpInLastEnable) )
	{
		/*------------------------------------------------------------------------------------------*/
		/*	BBON=OFF(HIGH)のとき、DPIN信号がHIGHでない場合は異常										*/
		/*------------------------------------------------------------------------------------------*/
		if( TestPulseDiag->DpInState )
		{
			TestPulseDiag->DpInDiagState = FALSE;
			if( !((TestPulseDiag->DpInDiagState)^(TestPulseDiag->DpInLastDiagState)) )
			{
				TestPulseDiag->DpInErrCnt++;
				if( (TestPulseDiag->DpInErrCnt) > 2 )
				{
					err = TRUE;
				}
			}
			else
			{
				TestPulseDiag->DpInErrCnt = 0;
			}
		}
		else
		{
			TestPulseDiag->DpInDiagState = TRUE;
			TestPulseDiag->DpInErrCnt = 0;
		}
		TestPulseDiag->DpInLastDiagState = TestPulseDiag->DpInDiagState;
	}
/* <<<<< <S1D1> */

	return err;
}

BOOL GetHwbbSupportSts( DET_HWBB_STS *DetHwbbSts )
{
	return DetHwbbSts->hwbbsupport;
}


/****************************************************************************************************/
/*																									*/
/*																									*/
/*		LpxSftMdlEncComErrDetect																	*/
/*							  Encoder alarm detection routine										*/
/*							  Processing in ScanB interruption										*/
/*							  Safety-related part													*/
/*																									*/
/*		Identification																				*/
/*			Function ID		: 0002(h)																*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*		Update history																				*/
/*			Function version: 0001(h)																*/
/*			Update info.	: Created																*/
/*			Date			: 2014.02.14															*/
/*			Issue			: (M-gi-S1) Y.Tanaka													*/
/*			Update keyword	: <S0F1>																*/
/*																									*/
/****************************************************************************************************/
/****************************************************************************************************/
/*																									*/
/*		Check Status of Input Signal													<S0F1>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 内部信号状態検出用DPIN1, DPIN2信号の状態を検出する										*/
/*          DPIN1, DPIN2信号有効/無効状態を検出する													*/
/*																									*/
/*	引 数 :																							*/
/*	戻り値:																							*/
/*																									*/
/****************************************************************************************************/
void SeqIoDetectTestPulseSignal( SEQ_IO_HNDL *pIoSignals, LONG AxisNo )
{
	TEST_PULSE_DIAG *pTestPulseDiag1;
	TEST_PULSE_DIAG *pTestPulseDiag2;
	DET_HWBB_STS *pDetHwbbSts;

	pTestPulseDiag1 = &(pIoSignals->TestPulseDiag[0]);
	pTestPulseDiag2 = &(pIoSignals->TestPulseDiag[1]);
	pDetHwbbSts    = &(pIoSignals->DetHwbbSts);
/*--------------------------------------------------------------------------------------------------*/
/*		信号の前回値を更新																			*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag1->DpInLastState = pTestPulseDiag1->DpInState;
	pTestPulseDiag2->DpInLastState = pTestPulseDiag2->DpInState;

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1信号の有効／無効状態判別 (BBON状態でない && 前回DPIN1信号有効 ⇒ 今回DPIN1信号は有効	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon1)) && (pTestPulseDiag1->DpInLastEnable) )
	{
		pTestPulseDiag1->DpInEnable = TRUE;
	}
	else
	{
		pTestPulseDiag1->DpInEnable = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN2信号の有効／無効状態判別 (BBON状態でない && 前回DPIN2信号有効 ⇒ 今回DPIN2信号は有効	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon2)) && (pTestPulseDiag2->DpInLastEnable) )
	{
		pTestPulseDiag2->DpInEnable = TRUE;
	}
	else
	{
		pTestPulseDiag2->DpInEnable = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2信号の状態取得																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( 0 == AxisNo )
	{
		pTestPulseDiag1->DpInState = GET_DPIN01();
		pTestPulseDiag2->DpInState = GET_DPIN02();
	}
	else
	{
		pTestPulseDiag1->DpInState = GET_DPIN11();
		pTestPulseDiag2->DpInState = GET_DPIN12();
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2状態(前回値)の更新																*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag1->DpInLastEnable  = !(pDetHwbbSts->Bbon1);
	pTestPulseDiag2->DpInLastEnable  = !(pDetHwbbSts->Bbon2);
}


/****************************************************************************/
/* 概要：セーフティ機能　内部信号異常検出用パルス出力処理		<S0F1>		*/
/*       4ms周期パルス出力(ScanC処理)										*/
/****************************************************************************/
/* 引数		:	-															*/
/* 戻り値	:	なし														*/
/****************************************************************************/
void SftReverceTestPls( void )
{
	VULONG	*CheckPostAddr;

	CheckPostAddr = HALdrv_GetCheckPostAddr();
	*CheckPostAddr ^= (1<< SIL3_TEST_PLS0);
#if (CSW_BOARD_TYPE  == SGD7W_TYPE)
	*CheckPostAddr ^=(1<< SIL3_TEST_PLS1);
#endif		/* #if (CSW_BOARD_TYPE  == SGD7W_TYPE) */
}
//void HALdrv_ReverceTestPls( void )
//{
//	*HALdrv_CheckPost ^= (1<< SIL3_TEST_PLS);
//	return;
//}



/*********************************** End of file ****************************************************/
