/****************************************************************************
Description	: Serial Encoder Service for Scan task
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

----------------------------------------------------------------------------
Changes	      :
	Name   Date          Description
	------------------------------------------------------------------------
	I.M.   2011.02.08    created

****************************************************************************/
#include "Basedef.h"
#include "SerialEncIf.h"
#include "MLib.h"
#include "KLib.h"
#include "SencScanService.h"	/* <S004> */
#include "JL086ServoIpHw.h"	/* <S004> */
#include "AlarmManager.h"		/* <S018> */
#include "MercuryGlobals.h"	/* <S1B7> */
#include "MotorLessTest.h"	/* <S1B7> */

#define _TIME_BASE_TIMER_//暫定/* <S004> */

#ifdef _TIME_BASE_TIMER_
LONG	LpxTimeBasesMotPosCalc( MENCV *MencV );
#endif /* _TIME_BASE_TIMER_ */


/* フルクローズドエンコーダ位置データ受信処理(HW:JL-086)	*//* <S04D> */
//static ALMID_T	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg );
static void	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE *AlmManager);/* <S0E0>:AlmManager追加 */
/* フルクローズドエンコーダ位置データ受信処理(SW)			*//* <S04D> */
static LONG	FencRcvPosSwProcess( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete ); /* <S0E0>:AlmManager追加 */

/* 回転型：シリアルエンコーダ位置データ受信処理(HW:JL-086) *//* <S004> */ /* <S0C5> */
static void	SencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
															ALMMAN_HANDLE 	*AlmManager );
/* 回転型：シリアルエンコーダ位置データ受信処理(SW) *//* <S004> */ /* <S0C5> */
static void	SencRcvPosSwProcess( MENCV *MencV, ALMMAN_HANDLE 	*AlmManager,
																	BOOL BeComplete );

/* リニアモータ：モータ電流位相更新処理								*/
static void	LmxUpdateMotPhase( MENCV *MencV, ALMMAN_HANDLE *AlmManager, LONG dMotPos );

/* <S1B7> >>>>> */
/* モータレス：マルチターンデータ取得処理	*/
static USHORT LpxGetSencMotLessMturnData( MENCV *MencV, MENCPRM *MencP, USHORT PosH, ULONG PosL );
/* モータレス：マルチターンデータ設定処理	*/
static void LpxSencMotLessMultiTurnSet(MENCPRM *MencP, ENCODER_DATA *EncData, SHORT MturnLimit);
static void MlibAdd6432( LONG *a, LONG b, LONG *x );	/* 暫定対応(関数をINT64を使用して作成) */
/* <<<<< <S1B7> */
/****************************************************************************************************/
/*																									*/
/*		リニアモータ：エンコーダＣ相通過チェック処理												*/
/*																									*/
/****************************************************************************************************//* <S029> */
void	LmxCheckSencCphasePass( MENCV *MencV )
{
//LONG	lwk;	/*<S0C7>*/
//LONG	remx;	/*<S0C7>*/
LONG	CposX;
LONG	CposChange;
/* <S21B>:Start */
LONG	lwk;
LONG	remx;
SENCX		*Enc;
Enc	= &MencV->EncIf;
/* <S21B>：end */

	CposX = 0;				/*<S0C4>*/
	CposChange = FALSE;
/*--------------------------------------------------------------------------------------------------*/
/*		原点通過チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( MencV->CphaseLess == FALSE )
		{
		/*------------------------------------------------------------------------------------------*/
		/*	Ｃ相通過チェック																		*/
		/*------------------------------------------------------------------------------------------*/
			if( (MencV->RxAlmCode & 0x0035) == 0x0001 )/* 固定ビット+オーバーヒートチェック	*/
			{
				if( (MencV->RxPosH[0] != MencV->RxPosH[1]) ||
					( (MencV->Cset==FALSE) && ((MencV->RxAlmCode & 0x0040)==0) ) )
				{
					CposChange = TRUE;
					MencV->Cset = TRUE;
					CposX = (LONG)(MencV->RxPosH[0] << HPOS_TO_XPOS);
					CposX = MencV->MposSign *(CposX + MencV->OrgDetDelayComp);
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	Ｃ相位置設定処理																		*/
		/*------------------------------------------------------------------------------------------*/
			if( MencV->Cset )
			{
			/*--------------------------------------------------------------------------------------*/
			/*	Ｃ相位置ラッチ処理																	*/
			/*--------------------------------------------------------------------------------------*/
				if( CposChange )
				{
					MencV->IncreCposLatch = TRUE;				/* Ｃ相位置ラッチ完了   --> ScanB	*/
					CposX = ((LONG)(MencV->MotPosX[0] - CposX) >> LMX_MPOS_SFT);	/* インクレ補正済C相位置 */
					MencV->MotCposX = MencV->MotPos - CposX;
				}
#if 0
			/*--------------------------------------------------------------------------------------*/
			/*	パルス変換原点補正設定処理															*/
			/*--------------------------------------------------------------------------------------*/
				if( MencP.f.DivOut == TRUE )
				{
					if( CposChange )
					{
						remx = 0;
						lwk = MlibPfbkxremNolim( MencV->MotCposX, MencV->DivOutGain, &remx );
						lwk += MencV->InitDivPosOffset ;											
						ASIC_WR( AREG_POSET1, (USHORT)(lwk - 1) );
						ASIC_WR( AREG_POSET2, (USHORT)(lwk + 0) );
#if	SVFSEL_MICROPROGRAM_DIVOUT == 1
						MencV->DivOutOrgX1 = lwk - 1;				/* パルス変換原点補正１(保持用)	*/
						MencV->DivOutOrgX2 = lwk + 0;				/* パルス変換原点補正２(保持用)	*/
#endif
					}
				/*----------------------------------------------------------------------------------*/
					lwk = MencV->MotPos - MencV->MotCposX;
					if( (lwk < MencP.DivOutCmskChk) && (lwk > -MencV->DivOutCmskChk) )
					{
						KPI_PCMASK_OFF( );					/* パルス変換C相マスクOFF設定			*/
//						KPI_PCMASK_OFF( RoutV.OutModeFlg );	/* パルス変換C相マスクOFF設定			*/
					}
					else
					{
						KPI_PCMASK_ON( );					/* パルス変換C相マスクON設定			*/
					}
				}
#endif
/* <S21B>:Start */
			/*--------------------------------------------------------------------------------------*/
			/*	パルス変換原点補正設定処理															*/
			/*--------------------------------------------------------------------------------------*/
				if(MencV->DivInfo.DivOut != FALSE)
				{
					if(CposChange != FALSE)
					{
						remx = 0;
						lwk = MlibPfbkxremNolim(MencV->MotCposX, MencV->DivInfo.DivOutGain, &remx);
						lwk += MencV->DivInfo.InitDivPosOffset;
						/* パルス変換原点補正1 */
						SHalDrv_SetOriginCorrect1( MencV->EncIf.SencSetReg, (lwk - 1) );
						/* パルス変換原点補正2 */
						SHalDrv_SetOriginCorrect2( MencV->EncIf.SencSetReg, lwk );
#if	SVFSEL_MICROPROGRAM_DIVOUT == 1
						MencV->DivInfo.DivOutOrgX1 = lwk - 1;				/* パルス変換原点補正１(保持用)	*/
						MencV->DivInfo.DivOutOrgX2 = lwk + 0;				/* パルス変換原点補正２(保持用)	*/
#endif
					}
					lwk = MencV->MposFromCpos;
					if((lwk < MencV->DivInfo.DivOutCmskChk) && (lwk > -MencV->DivInfo.DivOutCmskChk))
					{	/* パルス変換C相マスクOFF設定			*/
						Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;
						MencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;
					}
					else
					{	/* パルス変換C相マスクON設定			*/
						Enc->SencSetReg->AREG_WDT1SET |= BIT8;
						MencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
					}
				}
/* <S21B>：end */
			}
		}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相レスエンコーダ(アブソスケールの場合)：Ｃ相マスクＯＮ／ＯＦＦ処理のみ行う				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
#if 0
			/*--------------------------------------------------------------------------------------*/
			/*		絶対位置が原点(=0)近傍でＣ相マスクを解除する									*/
			/*--------------------------------------------------------------------------------------*/
			if( (MencV->MotAbsPos < MencV->DivOutCmskChk) && (MotAbsPos > -MencV->DivOutCmskChk) )
			{
//				KPI_PCMASK_OFF( RoutV.OutModeFlg );			/* パルス変換C相マスクOFF設定			*/
				KPI_PCMASK_OFF( );							/* パルス変換C相マスクOFF設定			*/
			}
			else
			{
				KPI_PCMASK_ON( );							/* パルス変換C相マスクON設定			*/
			}
#endif
/* <S21B>:Start */
			/*--------------------------------------------------------------------------------------*/
			/*		絶対位置が原点(=0)近傍でＣ相マスクを解除する									*/
			/*--------------------------------------------------------------------------------------*/
			if( (MencV->MotAbsPos < MencV->DivInfo.DivOutCmskChk) && (MencV->MotAbsPos > -MencV->DivInfo.DivOutCmskChk) )
			{
				Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;     /* パルス変換C相マスクOFF設定			*/
			}
			else
			{
				Enc->SencSetReg->AREG_WDT1SET |= BIT8;      /* パルス変換C相マスクON設定			*/
			}
/* <S21B>：end */
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}
/****************************************************************************
*
*		リニアモータ：スキャンＢサービス処理(Called from ScanB)
*
****************************************************************************/
BOOL LmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq )
{
	LONG	lwk1;
	LONG	LastMposFromCpos;

	if( MencV->CphaseLess != FALSE )
	{	/*------------------------------------------------------------------*/
		/*	AbsoEncoder : Reset Cphase Pass Signal							*/
		/*------------------------------------------------------------------*/
		MencV->CposOk = TRUE;
		LastMposFromCpos = MencV->MposFromCpos;
		MencV->MposFromCpos = MencV->MotAbsPos;
		lwk1 = LastMposFromCpos ^ MencV->MposFromCpos;
		MencV->MotCphPass = ( lwk1 < 0 ) ? TRUE : FALSE;
	}
	else
	{	/*------------------------------------------------------------------*/
		/*	IncreEncoder : Make MposFromCpos and Motor Cphase Pass Signal	*/
		/*------------------------------------------------------------------*/
		if( MencV->Cset == FALSE )
		{
			MencV->CposOk = FALSE;
			MencV->MotCphPass = FALSE;
		}
		else if( MencV->IncreCposLatch != FALSE )
		{
			MencV->CposOk = TRUE;
			MencV->IncreCposLatch = FALSE;/* <S029> ：コメントアウト解除 */
			*LatchClrReq = TRUE;
			MencV->MotCphPass = TRUE;

			/* <S029> : MD仕様からSigma7仕様に変更 */
			MencV->MposFromCpos = MencV->MotPos - MencV->MotCposX;
//			MencV->MposFromCpos = MencV->CphaseDist;
		}
		else
		{
			MencV->CposOk = TRUE;
			LastMposFromCpos = MencV->MposFromCpos;

			/* <S029> MD仕様からSigma7仕様に変更 */
			MencV->MposFromCpos = MencV->MotPos - MencV->MotCposX;
//			MencV->MposFromCpos = MencV->CphaseDist;
			lwk1 = LastMposFromCpos ^ MencV->MposFromCpos;
			MencV->MotCphPass = ( lwk1 < 0 ) ? TRUE : FALSE;
		}
	}

	return MencV->MotCphPass;
}


/****************************************************************************
*
*		回転型：スキャンＢサービス処理(Called from ScanB)
*
****************************************************************************/
BOOL RmxSencScanServiceB( MENCV *MencV, BOOL *LatchClrReq )
{
	LONG	lwk1;
	LONG	LastMotAngle;

	/*----------------------------------------------------------------------*/
	/*		Latch Encoder Data for ScanB									*/
	/*----------------------------------------------------------------------*/
//	MencV->ScanBPosH = MencV->RxPosH[0];			/* スキャンＡで読込んだデータを使用する。	*/
//	MencV->ScanBPosL = MencV->RxPosL[0].dw;

	/*----------------------------------------------------------------------*
	* Calculate Motor Angle, Motor Position from C-Phase, C-Phase Pass Signal
	*-----------------------------------------------------------------------*/
	if( (MencV->Cset != FALSE) && (MencV->CphaseLess == FALSE) )
	{
		/*------------------------------------------------------------------*/
		/*	MencV->MotAngle [pulse] : 0 ～ (PulseNo - 1)					*/
		/*------------------------------------------------------------------*/
		LastMotAngle = MencV->MotAngle;
		/* 2011.01.07 Y.Oka μプログラムI/F簡略化のため、CphaseDistを使用するよう変更 */
#if 1/* <S00C> */
		lwk1 = (LONG)((LONG)((MencV->MotPosX[0]) - (MencV->MotCposX)) >> MencV->MposSftR); 
#else
		lwk1 = (LONG)( MencV->CphaseDist >> MencV->MposSftR ); /* TODO: CphaseDist未作成のため、Un003が0になっている!! */
#endif
		MencV->MotAngle = ( lwk1 >= 0 ) ? lwk1 : lwk1 + MencV->PulseNo;

		/*------------------------------------------------------------------*/
		/*	MencV->MposFromCpos [pulse] : (-PulseNo/2) ～ (PulseNo/2 - 1)	*/
		/*------------------------------------------------------------------*/
		lwk1 = ( MencV->MotAngle < MencV->HalfPulseNo ) ?
			       MencV->MotAngle : MencV->MotAngle - MencV->PulseNo;

		MencV->MposFromCpos = lwk1;
		/*------------------------------------------------------------------*/
		/*	Make Motor Cphase Pass Signal									*/
		/*------------------------------------------------------------------*/
		/* 2010.08.25 Y.Oka インクレエンコーダ接続時のラッチ不具合修正 */
		if( MencV->IncreCposLatch != FALSE )
		{ /* First-time CPhase Pass (Incremental Encoder) */
			MencV->CposOk = TRUE;
			MencV->IncreCposLatch = FALSE;/* <S029> ：コメントアウト解除 */
			*LatchClrReq = TRUE;
			MencV->MotCphPass = TRUE;
		}
		else
		{	/* Already CPhase Pass
				(In case of Absolute Encoder, IncreCposLatch is not TRUE) */
			MencV->CposOk = TRUE;
			lwk1 = MlibLABS( MencV->MotAngle - LastMotAngle );
			MencV->MotCphPass = ( lwk1 > MencV->HalfPulseNo ) ? TRUE : FALSE;
		}
	}
	else
	{
		MencV->CposOk = FALSE;
		MencV->MotCphPass = FALSE;
	}

	return MencV->MotCphPass;
}




/****************************************************************************
*
*		フルクエンコーダ : Ｃ相通過チェック処理
*
****************************************************************************/
static void FencCheckCphasePass(MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg)
{
	LONG lwk1;
	LONG remx;
	LONG CposX;
	LONG CposChange;

	CposX = 0;
	if(FencV->CphaseLess == FALSE)
	{
		CposChange = FALSE;
		/*------------------------------------------------------------------*/
		/* Ｃ相通過チェック */
		/*------------------------------------------------------------------*/
		if((FencV->RxAlmCode & 0x0035) == 0x0001)
		{
			if( (FencV->RxPosH[0] != FencV->RxPosH[1]) ||
				((FencV->Cset == FALSE) && ((FencV->RxAlmCode & 0x0040) == 0)) )
			{
				CposChange = TRUE;
				FencV->Cset = TRUE;
				CposX = FencV->MposSign *
						(FSX_HposToXpos(FencV->RxPosH[0]) + FencV->OrgDetDelayComp);
			}
		}
		/*------------------------------------------------------------------*/
		/*	Ｃ相位置設定処理												*/
		/*------------------------------------------------------------------*/
		if(FencV->Cset != FALSE)
		{
			/*	Ｃ相位置ラッチ処理＆フルク位置演算	*/
			FencV->CposOk = TRUE;

			if(CposChange)
			{
				FencV->MposFromCpos = (FencV->MotPosX[0] - CposX) >> FencV->MposSftR;
				FencV->MotCposX = FencV->MotPos - FencV->MposFromCpos;
				FencV->MotCphPass = TRUE;
			}
			else
			{
				lwk1 = FencV->MposFromCpos;
				FencV->MposFromCpos = FencV->MotPos - FencV->MotCposX;
				FencV->MotCphPass = ((lwk1 ^ FencV->MposFromCpos) < 0) ? TRUE : FALSE;
			}

			/*	パルス変換原点補正設定処理 */
#if 0 /*<S02D>*/
			if(FencV->DivOut != FALSE)
			{
				if(CposChange != FALSE)
				{
					remx = 0;
					lwk1 = MlibPfbkxremNolim(FencV->MotCposX, FencV->DivOutGain, &remx);
					lwk1 += FencV->InitDivPosOffset;
					AsicHwReg->AREG_POSET1 = (USHORT)(lwk1 - 1);
					AsicHwReg->AREG_POSET2 = (USHORT)(lwk1 + 0);
				}

				lwk1 = FencV->MposFromCpos;
				if((lwk1 < FencV->DivOutCmskChk) && (lwk1 > -FencV->DivOutCmskChk))
				{	/* パルス変換C相マスクOFF設定			*/
					AsicHwReg->AREG_WDT1SET &= ~BIT8;
					FencV->DivCMaskStatus = DIV_CMASK_OFF;
				}
				else
				{	/* パルス変換C相マスクON設定			*/
					AsicHwReg->AREG_WDT1SET |= BIT8;
					FencV->DivCMaskStatus = DIV_CMASK_ON;
				}
			}
#else/*<S02D>*/
			if(FencV->DivInfo.DivOut != FALSE)
			{
				if(CposChange != FALSE)
				{
					remx = 0;
					lwk1 = MlibPfbkxremNolim(FencV->MotCposX, FencV->DivInfo.DivOutGain, &remx);
					lwk1 += FencV->DivInfo.InitDivPosOffset;
#if 0/* <S21B>:Start */
					AsicHwReg->AREG_POSET1 = (USHORT)(lwk1 - 1);
					AsicHwReg->AREG_POSET2 = (USHORT)(lwk1 + 0);
#else
					/* パルス変換原点補正1 */
					SHalDrv_SetOriginCorrect1( FencV->EncIf.SencSetReg, (lwk1 - 1) );
					/* パルス変換原点補正2 */
					SHalDrv_SetOriginCorrect2( FencV->EncIf.SencSetReg, lwk1 );
#endif/* <S21B>：end */
				}

				lwk1 = FencV->MposFromCpos;
				if((lwk1 < FencV->DivInfo.DivOutCmskChk) && (lwk1 > -FencV->DivInfo.DivOutCmskChk))
				{	/* パルス変換C相マスクOFF設定			*/
//					AsicHwReg->AREG_WDT1SET &= ~BIT8;
					FencV->EncIf.SencSetReg->AREG_WDT1SET &= ~BIT8; /* <S21B> */
					FencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;
				}
				else
				{	/* パルス変換C相マスクON設定			*/
//					AsicHwReg->AREG_WDT1SET |= BIT8;
					FencV->EncIf.SencSetReg->AREG_WDT1SET |= BIT8; /* <S21B> */
					FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
				}
			}
#endif/*<S02D>*/
		}

	}
	else
	{
		/*------------------------------------------------------------------*/
		/*		Ｃ相レスエンコーダ(アブソスケールの場合)					*/
		/*------------------------------------------------------------------*/
#if 0 /*<S02D>*/
		if(FencV->DivOut != FALSE)
		{
			/*	絶対位置が原点(=0)近傍でＣ相マスクを解除する */
			if( (FencV->MotAbsPos < FencV->DivOutCmskChk) &&
				(FencV->MotAbsPos > -FencV->DivOutCmskChk) )
			{	/* パルス変換C相マスクOFF設定			*/
				AsicHwReg->AREG_WDT1SET &= ~BIT8;
				FencV->DivCMaskStatus = DIV_CMASK_OFF;
			}
			else
			{	/* パルス変換C相マスクON設定			*/
				AsicHwReg->AREG_WDT1SET |= BIT8;
				FencV->DivCMaskStatus = DIV_CMASK_ON;
			}
		}
#else/*<S02D>*/
		if(FencV->DivInfo.DivOut != FALSE)
		{
			/*	絶対位置が原点(=0)近傍でＣ相マスクを解除する */
			if( (FencV->MotAbsPos < FencV->DivInfo.DivOutCmskChk) &&
				(FencV->MotAbsPos > -FencV->DivInfo.DivOutCmskChk) )
			{	/* パルス変換C相マスクOFF設定			*/
//				AsicHwReg->AREG_WDT1SET &= ~BIT8;
				FencV->EncIf.SencSetReg->AREG_WDT1SET &= ~BIT8; /* <S21B> */
				FencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;
			}
			else
			{	/* パルス変換C相マスクON設定			*/
//				AsicHwReg->AREG_WDT1SET |= BIT8;
				FencV->EncIf.SencSetReg->AREG_WDT1SET |= BIT8; /* <S21B> */
				FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;
			}
		}
#endif/*<S02D>*/
		/*	原点通過チェック */
		lwk1 = FencV->MposFromCpos;
		FencV->MposFromCpos = FencV->MotAbsPos;
		FencV->MotCphPass = ((lwk1 ^ FencV->MposFromCpos) < 0) ? TRUE : FALSE;
	}

}


#if 0
/****************************************************************************
*
*	フルクエンコーダ : タイムベースタイマを利用した速度演算
*
****************************************************************************/
LONG LpxTimeBasesFencSpdCalc(ENCVFBTIMBASE FbTimeBase, LONG dPos, LONG dPosSpd, UCHAR EnableFlag)
{
	LONG lwk;

	if(EnableFlag == FALSE)
	{
		/*------------------------------------------------------------------*/
		/*	補正無効時処理													*/
		/*------------------------------------------------------------------*/
		FbTimeBase->Modflag = FALSE; 		/* 無補正モード設定	*/
		FbTimeBase->fencspeed = dPosSpd; 	/* 無補正のまま出力	*/
	}
	else
	{
		/*------------------------------------------------------------------*/
		/*	補正有効時処理													*/
		/*------------------------------------------------------------------*/
		/*	速度演算実行判別(２パルス未満は通常差分速度) */
		FbTimeBase->Modflag = (MlibABS(dPos) < VFBTINMINPLS) ? FALSE : TRUE;

		/*	速度演算実行 */
		/*------------------------------------------------------------------*/
		if(FbTimeBase->BaseTimeInterval != 0)
		{
			/* ﾍﾞｰｽﾀｲﾏ変化時間なし -> ｴﾝｺｰﾀﾞｴﾗｰ時	*/
			/* ベースタイマを使用した速度演算 */
			lwk = (dPosSpd >> 2) * SAENCCLKNUM;
			lwk = lwk / (LONG) FbTimeBase->BaseTimeInterval;
			FbTimeBase->fencspeed = lwk << 2;
		}
		else
		{
			/* 補正条件未達の場合：エンコーダ位置差分速度演算使用 */
			FbTimeBase->fencspeed = dPosSpd;
		}
	}

	/*	今回値保存	*/
	FbTimeBase->LastBaseTimer = FbTimeBase->BaseTimer;
	FbTimeBase->LastModflag = FbTimeBase->Modflag;

	return (FbTimeBase->fencspeed);
}
#endif

/****************************************************************************
*
*		フルクエンコーダ : 通信異常処理
*
****************************************************************************/
static LONG FencComErrorProc(MENCV *FencV, ALMMAN_HANDLE *AlmManager)/* <S0E0>:AlmManager追加 */
{
	LONG FencPosX;
	LONG lwk;

	FencV->SeRxErr = TRUE;	/* Set Error Flag */
	++FencV->ComErrCnt; 	/* Count up ComErrCnt for Monitor	*/

	FencPosX = FencV->MotPosX[0];
	if(FencV->EncDataNG == FALSE)
	{
		if(FencV->MstErrCnt >= FencV->ComErrLimit)
		{
			FencV->EncDataNG = TRUE; 							/* エンコーダデータ無効	*/
			/* <S0E0>:アラーム処理 */
			ALMSetServoAlarm( AlmManager, ALM_FPGCOM );		/* A.CF1:ＦＰＧ通信異常			*/
			ALMSetMask( AlmManager, ALM_FPGCOMTIMER, TRUE );	/* A.CF2:ＦＰＧ通信タイマ異常	*/
		}
		else
		{
			++FencV->MstErrCnt;

			lwk = FencPosX - FencV->MotPosX[1];
			FencPosX += lwk;
		}
	}
	return FencPosX;
}

/***************************************************************************
*
*		フルクエンコーダ : 位置データ読込み
*
****************************************************************************//* <S04D> */
LONG FencReadPosition(MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	/* <S0E0>:AlmManager追加 */
						void *BaseLoops)	/*<S1B7>*/
{
	LONG	dFencPos, FencPosX;
	LONG	FencSpd;
//	ALMID_T	err_code;	/*<S1B7> 未使用*/

	ASIC_HW_IF_STR *sencSetReg;
	ASIC_HW_IF_STR *posCmpSetReg;

	sencSetReg = FencV->EncIf.SencSetReg;
	posCmpSetReg = FencV->EncIf.PosCmpSetReg;
	/*----------------------------------------------------------------------*/
	/*	未接続状態															*/
	/*----------------------------------------------------------------------*/
	if((FencV->EncConnect == FALSE) || (FencV->EncDisable != FALSE))
	{
		return (0);
	}

/*--------------------------------------------------------------------------------------------------*/
/*		変化時刻タイマによるエンコーダ位置補正処理													*/
/*--------------------------------------------------------------------------------------------------*/
	if( FencV->PosCompMode == POSCOMP_SW )/*<S0C5>rename*/
	{	/*------------------------------------------------------------------------------------------*/
		/*		SWにてエンコーダ位置補正(Σ-Ⅴ互換)													*/
		/*------------------------------------------------------------------------------------------*/
		FencPosX = FencRcvPosSwProcess( FencV, AlmManager, BeComplete );	/* <S0E0>:AlmManager追加 */
	}
	else
	{
//		err_code = FencRcvPosHwProcess( FencV, posCmpSetReg );/*<S0C5>rename*/
		FencRcvPosHwProcess( FencV, posCmpSetReg, AlmManager );/*<S0C5>rename*//* <S0E0>:AlmManager追加 */
		/*------------------------------------------------------------------------------------------*/
		/*		JL-086にてエンコーダ位置補正														*/
		/*------------------------------------------------------------------------------------------*/
		if( FencV->PosCompMode == POSCOMP_ASIC )
		{
			FencPosX = ( SHal_GetCurMotPos( posCmpSetReg ) << FencV->MposSftR );
		}
		/*------------------------------------------------------------------------------------------*/
		/*		エンコーダ位置補正なし																*/
		/*------------------------------------------------------------------------------------------*/
		else
		{
			FencPosX = ( SHal_GetCurRcvPos( posCmpSetReg ) << FencV->MposSftR );
		}
	}
	/*----------------------------------------------------------------------*/
	/*	位置差分演算														*/
	/*----------------------------------------------------------------------*/
	dFencPos = (FencPosX - FencV->MotPosX[0]) >> FencV->MposSftR;
	FencV->ABphCntr = (UCHAR)(FencPosX >> FencV->MposSftR);

	FencV->MotPosX[1] = FencV->MotPosX[0];
	FencV->MotPosX[0] = FencPosX;
	/*----------------------------------------------------------------------*/
	/*	外部PG速度演算														*/
	/*----------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FencSpd = dFencPos * FencV->KConvLoadSpd;
#else
	FencSpd = MlibMulgain(dFencPos, FencV->KConvLoadSpd);
#endif /* FLOAT_USE */
#if 0	/* ★変化時刻タイマによる速度補正は現状未対応★ */
	FencV->MotSpd = LpxTimeBasesFencSpdCalc(dFencPos, FencSpd, FencV->SpdCmpEnable);
	FencV->SpdCmpEnable = TRUE;
#else
	FencV->MotSpd = FencSpd;
#endif
	/*----------------------------------------------------------------------*/
	/*	位置演算 [32bitRingPulse]											*/
	/*----------------------------------------------------------------------*/
	FencV->MotPos += dFencPos;
	FencV->MotAbsPos += dFencPos;

	/*----------------------------------------------------------------------*/
	/*	初期インクレパルス出力処理 or Ｃ相通過チェック処理					*/
	/*----------------------------------------------------------------------*/
#if 0
	if(FencV->IncPulseReq != FALSE)
	{
		/*--------------------------------------------------------------------
		* 電源投入時の初期化中にモータが移動していた場合の分周パルス出力対策
		*	ScanA処理の1回目は、カウンタプリセットとパルス変換位置をセットする。
		*-------------------------------------------------------------------*/
		if(FencV->AbsoEncType == MENC_INCRE_TYPE)
		{
			/* Ｃ相通過チェック処理 */
			if(FencV->SeRxErr == FALSE)
			{
				FencCheckCphasePass();
			}
			/*	分周パルス出力処理	(直前にカウンタプリセットを設定)*/
			if(FencV->DivOut == TRUE)
			{
				FencV->DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivOutGain, &FencV->DivOutRem);
				sencSetReg->AREG_POTEST = (USHORT)FencV->DivOutPos;
				sencSetReg->AREG_POSET0 = (USHORT)FencV->DivOutPos;
			}
			FencV->IncPulseReq = FALSE;
		}
		else
		{
			LpxOutFencIniIncPulse(); /* 初期インクレパルス出力処理	*/
		}
	}
	else
//#endif /*<S02D>削除*/
	{
		/* Ｃ相通過チェック処理 */
		if(FencV->SeRxErr == FALSE)
		{
			FencCheckCphasePass(FencV, sencSetReg);
		}
		/* 分周パルス出力*/
		if(FencV->DivOut != FALSE)
		{
			FencV->DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivOutGain, &FencV->DivOutRem);
			sencSetReg->AREG_POSET0 = (USHORT) FencV->DivOutPos;
		}
	}
#else/*<S02D>*/
	if(FencV->DivInfo.IncPulseReq != FALSE)
	{
		/*----------------------------------------------------------------------*/
		/* 電源投入時の初期化中にモータが移動していた場合の分周パルス出力対策   */
		/*	ScanA処理の1回目は、カウンタプリセットとパルス変換位置をセットする。*/
		/*----------------------------------------------------------------------*/
		if(FencV->AbsoEncType == MENC_INCRE_TYPE)
		{
			/* Ｃ相通過チェック処理 */
			if(FencV->SeRxErr == FALSE)
			{
				FencCheckCphasePass(FencV, sencSetReg);
			}
			/*	分周パルス出力処理	(直前にカウンタプリセットを設定)*/
			if(FencV->DivInfo.DivOut == TRUE)
			{
				FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
				sencSetReg->AREG_POTEST = (USHORT)FencV->DivInfo.DivOutPos;
				sencSetReg->AREG_POSET0 = (USHORT)FencV->DivInfo.DivOutPos;
			}
			FencV->DivInfo.IncPulseReq = FALSE;
		}
		else
		{
			/*----------------------------------------------------------------------*/
			/*	初期インクレパルス出力処理	  									    */
			/*----------------------------------------------------------------------*/
			FencSencOutIniIncPulse( FencV, sencSetReg );
		}
	}
	else
	{
		/* Ｃ相通過チェック処理 */
		if(FencV->SeRxErr == FALSE)
		{
			FencCheckCphasePass(FencV, sencSetReg);
		}
		/* 分周パルス出力*/
		if( (FencV->DivInfo.DivOut != FALSE) && (FencV->DivInfo.IncPulseReq == FALSE))
		{
			FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
			sencSetReg->AREG_POSET0 = (USHORT) FencV->DivInfo.DivOutPos;
		}
	}

#endif/*<S02D>*/
	return (dFencPos);
}


/****************************************************************************************************/
/*																									*/
/*		フルクローズドエンコーダ位置データ受信処理(HW:JL-086)										*/
/*																									*/
/****************************************************************************************************/
//static ALMID_T	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg)/* <S0E0> */
static void	FencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE *AlmManager)/* <S0E0> */
{
//	ALMID_T		err_code;/* <S0E0> */

//	err_code = NO_ALARM;									/* ローカル変数初期化					*//* <S04D> */
	/*----------------------------------------------------------------------------------------------*/
	/*		アラーム時の処理																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->SeRxErr   = SHal_GetSeRxErr( pAsicHwReg );		/* 受信エラーフラグ更新 				*/
	MencV->EncDataNG = SHal_GetEncDataNG( pAsicHwReg );		/* エンコーダデータ異常フラグの更新 	*/
	/*----------------------------------------------------------------------------------------------*/
	if( SHal_GetComAlarm( pAsicHwReg ) )					/* エラー種別判定(通信／加速度)			*/
	{
//		err_code = ALM_FPGCOM;								/* A.CF1 : エンコーダ通信異常			*//* <S0E0> */
		ALMSetServoAlarm( AlmManager, ALM_FPGCOM );			/* A.CF1:ＦＰＧ通信異常					*/
		ALMSetMask( AlmManager, ALM_FPGCOMTIMER, TRUE );	/* A.CF2:ＦＰＧ通信タイマ異常			*/
	}
	if( SHal_GetAccAlarm( pAsicHwReg ) )					/* エラー種別判定(通信／加速度)			*/
	{
//		err_code = ALM_FPGCOM;								/* A.CF1 : エンコーダ通信異常(加速度)	*//* <S0E0> */
		ALMSetServoAlarm( AlmManager, ALM_FPGCOM );			/* A.CF1:ＦＰＧ通信異常					*/
		ALMSetMask( AlmManager, ALM_FPGCOMTIMER, TRUE );	/* A.CF2:ＦＰＧ通信タイマ異常			*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		補正無し位置データ更新																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RcvPosX[2] = MencV->RcvPosX[1];			/*<S0C5>*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];			/*<S0C5>*/
	MencV->RcvPosX[0] = (LONG)( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
	/*----------------------------------------------------------------------------------------------*/
	/*		受信データL,H更新																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RxPosL[1].Long = MencV->RxPosL[0].Long;	/*<S0C5>*/
	MencV->RxPosH[1] 	  = MencV->RxPosH[0];		/*<S0C5>*/
	MencV->RxPosL[0].Long = (LONG) SHal_GetCurRxPosL( pAsicHwReg );
	MencV->RxPosH[0] 	  = (SHORT)SHal_GetCurRxPosH( pAsicHwReg );
	/*----------------------------------------------------------------------------------------------*/
	SHal_SrcClr( pAsicHwReg );								/* 受信完了フラグ更新					*/
	/*----------------------------------------------------------------------------------------------*/
//	return err_code;/* <S0E0> */
	return;
}
/****************************************************************************************************/
/*																									*/
/*		フルクローズドエンコーダ位置データ受信処理(SW)												*/
/*																									*/
/****************************************************************************************************/
static LONG	FencRcvPosSwProcess( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete ) /* <S0E0>:AlmManager追加 */
{
	ULONG	RxFlg;
	LONG	FencPosX;
	USHORT	RxPosH_wk;
	USHORT	BaseTime;
	SENCX	*Enc;
	Enc = &FencV->EncIf;
	
	RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
	if( ((RxFlg & 0x00008000) == 0) /*&& (FencV->SeRxComp == FALSE)*/ )
	{
		/* 受信未完時 : フルクエンコーダ受信異常処理 */
		FencPosX = FencComErrorProc(FencV, AlmManager);
	}
	else
	{
		/* 受信完了時 : フルクエンコーダ受信完了処理 */
		FencV->SeRxComp = FALSE; /* Reset Rx Complete Flag			*/

		/*------------------------------------------------------------------*/
		/*	受信データ取得													*/
		/*------------------------------------------------------------------*/
		SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&Enc->RxData );

		/*------------------------------------------------------------------*/
		/*	受信データを内部変数に設定										*/
		/*------------------------------------------------------------------*/
//		RxPosL_wk = FencV->RxPosL;
		RxPosH_wk = (USHORT)FencV->RxPosH[0];
		FencV->RxPosH[1] = (LONG)RxPosH_wk;

		FencV->RxAlmCode 	= Enc->RxData.UsBuf[2];		/* Alarm Code		*/
		FencV->RxTimex.w.l 	= Enc->RxData.UsBuf[3];		/* TimeBase, etc	*/
		FencV->RxTimex.w.h 	= Enc->RxData.UsBuf[4];		/* TimeBase, etc	*/
		FencV->RxPosL[0].w.l= Enc->RxData.UsBuf[5];		/* Position Low		*/
		FencV->RxPosL[0].w.h= Enc->RxData.UsBuf[6];		/* Position High	*/
		FencV->RxPosH[0] 	= Enc->RxData.UsBuf[7];		/* Incre. Compen.	*/

		/*--------------------------------------------------------------------
		*  ベースタイマが前回値と同じ場合、通信異常とする
		*--------------------------------------------------------------------*/
		BaseTime = (USHORT)(FencV->RxTimex.dw >> 8);
		if(FencV->FbTimeBase.BaseTimer == BaseTime)
		{
			/*--------------------------------------------------------------*/
			/*	通信異常時													*/
			/*--------------------------------------------------------------*/
			FencPosX = FencComErrorProc(FencV, AlmManager);
		}
		else
		{
			/*--------------------------------------------------------------*/
			/*	通信正常時													*/
			/*--------------------------------------------------------------*/
			FencV->MstErrCnt = 0;
			FencV->SeRxErr = FALSE;
			FencV->EncDataNG = FALSE;

			FencPosX = FencV->MposSign *
					((FencV->RxPosL[0].Long >> FencV->MposSftR) << FencV->MposSftR);
		}

		FencV->FbTimeBase.BaseTimer = BaseTime;
	}
	
	return FencPosX;
}
/****************************************************************************************************/
/*																									*/
/*		シリアルエンコーダ通信異常処理																*/
/*																									*/
/****************************************************************************************************/
static void	LpxSencComErrorProc( MENCV *MencV, ALMMAN_HANDLE *AlmManager, LONG RcvPosX, BOOL AccErr )/*<S0C5>*/
{

/*--------------------------------------------------------------------------------------------------*/
/*		アラーム時の処理：アラーム設定																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->MstErrCnt > MencV->ComErrLimit )				/* エラーカウンタチェック				*/
	{
		if( MencV->ComErrCnt >= MencV->AccErrCnt )			/* エラー種別判定(通信／加速度)			*/
		{
			MencV->EncDataNG = TRUE;						/* エンコーダデータ無効					*/
			ALMSetServoAlarm( AlmManager, ALM_PGCOM );		/* A.C90 : エンコーダ通信異常			*//*<S0C5>*/
		}
		else
		{
			MencV->EncDataNG = TRUE;						/* エンコーダデータ無効					*/
			ALMSetServoAlarm( AlmManager, ALM_PGCOM1 );		/* A.C91 : エンコーダ通信異常(加速度)	*//*<S0C5>*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		非アラーム時の処理：前回値で補間演算														*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		MencV->RcvPosX[2]  = MencV->RcvPosX[1];				/* 前々回値更新							*/
		MencV->RcvPosX[1]  = MencV->RcvPosX[0];				/* 前回値更新							*/
		if( AccErr )
		{
			/* 加速度異常時は補間しない */
			MencV->RcvPosX[0] = RcvPosX;
		}
		else
		{
			/* 今回値は補間演算 */
			MencV->RcvPosX[0] += (MencV->RcvPosX[1] - MencV->RcvPosX[2]);
		}
		MencV->MstErrCnt++;									/* Count up Error Counter				*/
	}
/*--------------------------------------------------------------------------------------------------*/
	return;/*<S0C5>*/
}


/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ原点通過チェック													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : アブソインクレ及びインクレ時に原点通過をチェックし、原点位置の設定を行うと共に、		*/
/*			分周パルスの出力を行う。																*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/* ★関数名称変更要 Y.Oka★ */
static void	RmxCheckSencOriginPass( MENCV *MencV )
{
	LONG	Orgx;
	LONG	Zsig;
	LONG	wk1, wk2, wk3;

/*--------------------------------------------------------------------------------------------------*/
/*		位相原点設定＆Ｃ相マスク解除処理															*/
/*--------------------------------------------------------------------------------------------------*/
	if( MencV->Cset == FALSE )
	{
		if( (MencV->RxAlmCode & 0x0035)!=0x0001 )			/* 固定ビットチェック(bit0,2,4,5)	*/
		{
			MencV->RxPosH[0] = MencV->RxPosH[1];			/* 今回のインクレ補正値は次回に持ち	*/
		}														/* 越さず、前回値を使う				*/
		else
		{
			if( MencV->Sigma3Encoder )						/* エンコーダ種別チェック			*/
			{
				Zsig = (MencV->RxPosH[0] & 0x0001);			/* ΣⅢ：原点信号					*/
			}
			else
			{
				Zsig = (MencV->RxPosL[0].w.l & 0x0100);		/* ΣⅡ：原点信号					*/
			}
			Orgx = (MencV->RxPosH[0] >> MencV->IncreCompSft);		/* インクレ補正量			*/
		/*------------------------------------------------------------------------------------------*/
		/*	原点未通過Flag == 0  or インクレ補正量変化												*/
		/*------------------------------------------------------------------------------------------*/
			if( ((MencV->RxAlmCode & 0x0040) == 0) ||
				(Orgx != (MencV->RxPosH[1] >> MencV->IncreCompSft)) )
			{
				MencV->Cset = TRUE;							/* Ｃ相設定完了						*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	原点信号 = 1 and 下位カウンタが原点付近													*/
		/*------------------------------------------------------------------------------------------*/
			else if( Zsig != 0 )
			{
				if( (MencV->RxPosL[0].w.h & 0x000c) == 0x0000 )
				{
					MencV->Cset = TRUE;						/* Ｃ相設定完了						*/
					Orgx = MencV->RxPosL[0].w.h >> 4;
				}
				else if( (MencV->RxPosL[0].w.h & 0x000c) == 0x000c )
				{
					MencV->Cset = TRUE;						/* Ｃ相設定完了						*/
					Orgx = (MencV->RxPosL[0].w.h >> 4) + 1;
				}
			}
		/*------------------------------------------------------------------------------------------*/
		/*	位相原点設定＆Ｃ相マスク解除処理														*/
		/*------------------------------------------------------------------------------------------*/
			if( MencV->Cset )
			{
				MencV->MotOrgX = MencV->MposSign * (Orgx << MencV->MorgSft);
			/*--------------------------------------------------------------------------------------*/
				MencV->IncreCposLatch = TRUE;				/* Ｃ相位置ラッチ完了   --> ScanB	*/
				MencV->MotCposX = MencV->MotOrgX;			/* Ｃ相位置ラッチデータ --> ScanB	*/
			/*--------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
				if( MencV->DivOut == TRUE )					/* モータエンコーダ分周出力選択？	*/
				{
					/* インクレ用原点出力設定 */
					Orgx = MencV->MotOrgX >> MencV->DivOutSft;
					/* パルス変換原点補正1 */
					SHalDrv_SetOriginCorrect1( MencV->EncIf.SencSetReg, (Orgx - 1) );	/* <S04D> 引数変更 */
					/* パルス変換原点補正2 */
					SHalDrv_SetOriginCorrect2( MencV->EncIf.SencSetReg, Orgx );		/* <S04D> 引数変更 */
					/* 分周C相マスクOFF */
					MencV->DivCMaskStatus  = DIV_CMASK_OFF;
				}
#else/*<S02D>*/
				if( MencV->DivInfo.DivOut == TRUE )			/* モータエンコーダ分周出力選択？	*/
				{
					/* インクレ用原点出力設定 */
					Orgx = MencV->MotOrgX >> MencV->DivInfo.DivOutSft;
					/* パルス変換原点補正1 */
					SHalDrv_SetOriginCorrect1( MencV->EncIf.SencSetReg, (Orgx - 1) );
					/* パルス変換原点補正2 */
					SHalDrv_SetOriginCorrect2( MencV->EncIf.SencSetReg, Orgx );
					/* 分周C相マスクOFF */
					MencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF;
					/* パルス変換C相マスクOFF設定	*/
					MencV->EncIf.SencSetReg->AREG_WDT1SET &= ~BIT8;							/*<S02D>*/
				}
#endif/*<S02D>*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Ｃ相通過設定処理																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0/*<S02D>*/
	if( MencV->Cset )
	{
		wk1 = (MencV->MotPosX[0] - MencV->MotOrgX);
		wk2 = (MencV->MotPosX[1] - MencV->MotOrgX);
		wk3 = wk1 >> MencV->DivOutSft;							/* 今回分周原点基準位置			*/
	/*----------------------------------------------------------------------------------------------*/
		wk2 = (wk1 ^ wk2) & 0xc0000000;
		wk1 = wk1 & 0xc0000000;
		if( (wk2 == 0xc0000000) && ((wk1 == 0x00000000) || (wk1 == 0xc0000000)) )
		{
			MencV->Cpass = TRUE;								/* Ｃ相通過設定(極性反転)			*/
			MencV->DivCpassPos = wk3;						/* Ｃ相通過時分周位置保存			*/
		}
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivCpass == FALSE )						/* 分周原点通過チェック				*/
		{
			if( MencV->Cpass )
			{
				if( ((MencV->DivCpassPos >= 0) && (wk3 >  3)) ||
					((MencV->DivCpassPos <  0) && (wk3 < -4))	)
				{
					MencV->DivCpass = TRUE;					/* 分周原点通過設定					*/
				}
			}
		}
		else
		{
			MencV->OrgOk = TRUE;								/* 原点設定完了						*/
		}
	}
#else/*<S02D>*/
	if( MencV->Cset )
	{
		wk1 = (MencV->MotPosX[0] - MencV->MotCposX);
		wk2 = (MencV->MotPosX[1] - MencV->MotCposX);
		wk3 = wk1 >> MencV->DivInfo.DivOutSft;					/* 今回分周原点基準位置			*/
	/*----------------------------------------------------------------------------------------------*/
		wk2 = (wk1 ^ wk2) & 0xc0000000;
		wk1 = wk1 & 0xc0000000;
		if( (wk2 == 0xc0000000) && ((wk1 == 0x00000000) || (wk1 == 0xc0000000)) )
		{
			MencV->Cpass = TRUE;								/* Ｃ相通過設定(極性反転)			*/
			MencV->DivInfo.DivCpassPos = wk3;					/* Ｃ相通過時分周位置保存			*/
			if( (MencV->DivInfo.DivOut == TRUE) && (MencV->DivInfo.DivPass == FALSE) )
			{
				MencV->EncIf.SencSetReg->AREG_DIVSET |= (BIT11 | BIT8);/* 分周Ｃ相出力選択２			*/
			}
		}
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->DivInfo.DivCpass == FALSE )						/* 分周原点通過チェック			*/
		{
			if( MencV->Cpass )
			{
				if( ((MencV->DivInfo.DivCpassPos >= 0) && (wk3 >  3)) ||
					((MencV->DivInfo.DivCpassPos <  0) && (wk3 < -4))	)
				{
					MencV->DivInfo.DivCpass = TRUE;					/* 分周原点通過設定				*/
				}
			}
		}
		else
		{
			if( MencV->DivInfo.DivOut == TRUE )					/* モータエンコーダ分周出力選択？	*/
			{ 
				MencV->EncIf.SencSetReg->AREG_DINCSET &= ~(BIT10); /* 分周常時クリア設定OFF 			*/
			}
			MencV->OrgOk = TRUE;								/* 原点設定完了						*/
		}
	}
#endif/*<S02D>*/

}

/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ位置データ読込み													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : シリアルエンコーダからＡＳＩＣが受信した位置データを、ＡＳＩＣレジスタから				*/
/*			読込み、モータエンコーダ変数(MencV)に設定する。											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	SencReadPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete,	void *BaseLoops )/* <S018> *//*<S0C5>*//*<S1B7>*/
{
	LONG		MotPosX;
	LONG		LastMotPosX;
	LONG		dMotPos;
	SENCX		*Enc;
	LONG		lwk1;  		/*<S02D>*/
	LONG		DivMotPosX; /*<S079>*/

	LastMotPosX	= MencV->MotPosX[0];
	Enc			= &MencV->EncIf;

/*--------------------------------------------------------------------------------------------------*/
/*		未接続状態																					*/
/*--------------------------------------------------------------------------------------------------*/
	if((MencV->EncConnect == FALSE) || (MencV->EncDisable != FALSE))
	{
//		LpxSencComErrorProc(MencV, AlmManager, 0, FALSE);											  /* <S0DD> */
		MotPosX    = ( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
		DivMotPosX = SHal_GetCurRcvPos( pAsicHwReg );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		変化時刻タイマによるエンコーダ位置補正処理													*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		switch(MencV->PosCompMode)
		{
		/*------------------------------------------------------------------------------------------*/
		/*		JL-086にてエンコーダ位置補正(通常処理)												*/
		/*------------------------------------------------------------------------------------------*/
		case POSCOMP_ASIC:
			SencRcvPosHwProcess( MencV, pAsicHwReg, AlmManager );
			MotPosX    = ( SHal_GetCurMotPos( pAsicHwReg ) << MencV->MposSftR );
			DivMotPosX = SHal_GetCurMotPos( pAsicHwReg );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		SWにてエンコーダ位置補正(Σ-Ⅴ互換)													*/
		/*------------------------------------------------------------------------------------------*/
		case POSCOMP_SW:
			SencRcvPosSwProcess( MencV, AlmManager, BeComplete );
			MotPosX    = LpxTimeBasesMotPosCalc( MencV );
			DivMotPosX = SHal_GetCurRcvPos( pAsicHwReg );
			break;
		/*------------------------------------------------------------------------------------------*/
		/*		エンコーダ位置補正なし																*/
		/*------------------------------------------------------------------------------------------*/
		case POSCOMP_NOP:
		default:
			SencRcvPosHwProcess( MencV, pAsicHwReg, AlmManager );
			MotPosX    = ( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
			DivMotPosX = SHal_GetCurRcvPos( pAsicHwReg );
			break;
		}
	}
#if 0
	if( MencV->PosCompCheck == POSCOMP_SW )
	{	/*------------------------------------------------------------------------------------------*/
		/*		SWにてエンコーダ位置補正(Σ-Ⅴ互換)													*/
		/*------------------------------------------------------------------------------------------*/
		err_code = SencRcvPosSwProcess( MencV, BeComplete );
		MotPosX  = LpxTimeBasesMotPosCalc( MencV );
	}
	else
	{
		err_code = SencRcvPosHwProcess( MencV, pAsicHwReg );
		/*------------------------------------------------------------------------------------------*/
		/*		JL-086にてエンコーダ位置補正														*/
		/*------------------------------------------------------------------------------------------*/
		if( MencV->PosCompCheck == POSCOMP_ASIC )
		{
			MotPosX = ( SHal_GetCurMotPos( pAsicHwReg ) << MencV->MposSftR );
			DivMotPosX = SHal_GetCurMotPos( pAsicHwReg );  /*<S079>*/
		}
		/*------------------------------------------------------------------------------------------*/
		/*		エンコーダ位置補正なし																*/
		/*------------------------------------------------------------------------------------------*/
		else
		{
			MotPosX = ( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
		}
	}
#endif
	/*----------------------------------------------------------------------------------------------*/
	/* 補正後位置データ更新 																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->MotPosX[1]	= MencV->MotPosX[0];				/* 前回補正後位置データ					*/
	MencV->MotPosX[0]	= MotPosX;							/* 今回補正後位置データ					*/
/*--------------------------------------------------------------------------------------------------*/
/*		モータ位置出力(32bitRingPulse)																*/
/*--------------------------------------------------------------------------------------------------*/
	dMotPos = ( (MencV->MotPosX[0] - LastMotPosX) >> MencV->MposSftR );
	MencV->MotPos   = MencV->MotPos + dMotPos;

	if( MencV->AxisMotType == MOTTYPE_LINEAR )/*<S017>*/
	{ /* リニア型 */
		LmxUpdateMotPhase( MencV, AlmManager, dMotPos );
	}
	else
	{ /* 回転型 */
		MencV->MotPhase = (USHORT)(( (((MencV->MotPosX[0]) - (MencV->MotOrgX)) >>8) * MencV->Kmotphase)>>8);
	}
/*--------------------------------------------------------------------------------------------------*/
/*		フィードバックパルス出力処理																*/
/*--------------------------------------------------------------------------------------------------*/
	// 初期インクレ出力なし
#if 0/*<S02D>*/
	if( (MencV->OrgOk == FALSE) && (MencV->SeRxErr == FALSE) )
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )/* <S029> : リニア処理追加 */
		{ /* リニア型 */
			LmxCheckSencCphasePass(MencV);					/* 原点通過チェック						*/
		}
		else
		{ /* 回転型 */
			RmxCheckSencOriginPass(MencV);					/* 原点通過チェック						*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	// 初期インクレ出力なし
	if( MencV->DivOut == TRUE )
	{
		SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivOutSft) );
	}
#else/*<S02D>*/
	if( MencV->DivInfo.IncPulseReq == TRUE )				/* 初期インクレ出力要求？				*/
	{
		/*------------------------------------------------------------------------------------------*/
		/* 電源投入時の初期化中にモータが移動していた場合の分周パルス出力対策		     	        */
		/*		ScanA処理の1回目は、カウンタプリセットとパルス変換位置をセットする。		        */
		/*------------------------------------------------------------------------------------------*/
		if( MencV->AbsoEncType == MENC_INCRE_TYPE )
		{
			if( MencV->SeRxErr == FALSE )
			{
				/*---------------------------------------------------------------------------------*/
				/*		原点通過チェック処理													   */
				/*---------------------------------------------------------------------------------*/
				if( MencV->AxisMotType == MOTTYPE_LINEAR )/*リニア処理漏れ追加 <S040> */		/* TODO 分周処理の中にインクレモードの分岐があるのはおかしい！！要修正*/
				{ /* リニア型 */
					LmxCheckSencCphasePass(MencV);
				}
				else
				{ /* 回転型 */
					RmxCheckSencOriginPass(MencV);
				}
			}
			/*-------------------------------------------------------------------------------------*/
			/*		分周パルス出力処理	(直前にカウンタプリセットを設定)						   */
			/*-------------------------------------------------------------------------------------*/
			if( MencV->DivInfo.DivOut == TRUE )
			{
#if 0/*<S079>*/
				Enc->SencSetReg->AREG_POTEST = (ULONG)(MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft);
				SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
#else/*<S079>*/
				if(MencV->DivInfo.DivOutASIC != FALSE)
				{ /* JL-086にて分周の場合 */
					Enc->SencSetReg->AREG_POTEST = (ULONG) DivMotPosX;
					SHalDrv_SetDivOutPosition( Enc->SencSetReg, DivMotPosX );
				}
				else
				{
#if 0 /* <S21B>:Start */
					Enc->SencSetReg->AREG_POTEST = (ULONG)(MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft);
					SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
#else
					if( MencV->AxisMotType == MOTTYPE_LINEAR )
					{ /* リニア型 */
						MencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dMotPos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem);
						SHalDrv_SetDivOutPosition( Enc->SencSetReg, (ULONG)MencV->DivInfo.DivOutPos );
					}
					else
					{ /* 回転型 */
						Enc->SencSetReg->AREG_POTEST = (ULONG)(MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft);
						SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
					}
#endif/* <S21B>：end */
				}
#endif/*<S079>*/
			}
			MencV->DivInfo.IncPulseReq = FALSE;
		}
		else
		{
			/*-------------------------------------------------------------------------------------*/
			/*		初期インクレパルス出力処理												       */
			/*-------------------------------------------------------------------------------------*/
			if( MencV->AxisMotType == MOTTYPE_LINEAR )/* <S209> : リニア処理追加 */
			{ /* リニア型 */
				LmxOutSencIniIncPulse(MencV, pAsicHwReg, LMX_MPOS_SFT);
			}
			else
			{ /* 回転型 */
				SencOutIniIncPulse( MencV, pAsicHwReg );
			}
			if((MencV->DivInfo.DivOutASIC != FALSE) && (MencV->DivInfo.IncPulseReq == FALSE))  /*<S079>*/
			{ /* JL-086にて分周の場合、初期インクレパルス出力完了後JL-086にて分周する */
				Enc->SencSetReg->AREG_DIVSET &= ~BIT1;
			}
		}
	}
	else if( (MencV->OrgOk == FALSE) && (MencV->SeRxErr == FALSE) )
	{
		/*------------------------------------------------------------------------------------------*/
		/*		原点通過チェック処理												                */
		/*------------------------------------------------------------------------------------------*/
//		RmxCheckSencOriginPass(MencV);            /* <S034>DELL */
		if( MencV->AxisMotType == MOTTYPE_LINEAR )/* <S029> : リニア処理追加 */
		{ /* リニア型 */
			LmxCheckSencCphasePass(MencV);					/* 原点通過チェック						*/
		}
		else
		{ /* 回転型 */
			RmxCheckSencOriginPass(MencV);					/* 原点通過チェック						*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		分周パルス出力処理																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MencV->DivInfo.DivOut == TRUE) && (MencV->DivInfo.IncPulseReq == FALSE) && (MencV->DivInfo.DivOutASIC == FALSE) )
	{
		if( MencV->AxisMotType != MOTTYPE_LINEAR )/* <S21B>:リニア処理追加 */
		{ /* 回転型 */
			SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
		/*----------------------------------------------------------------------------------------------*/
		/*  C相出力処理													<S02D>							*/
		/*----------------------------------------------------------------------------------------------*/
			if( MencV->DivInfo.DivPcmaskCtrl )
			{
				lwk1 = ( MencV->MotPosX[0] - MencV->MotOrgX);
				if( ( (lwk1 > -MencV->DivInfo.DivOutCmskChk) && (lwk1 < MencV->DivInfo.DivOutCmskChk)) && (MencV->Cset) )
				{   /* パルス変換C相マスクOFF設定			*/
					Enc->SencSetReg->AREG_WDT1SET &= ~BIT8;
					if( MencV->DivInfo.DivPass == FALSE )
					{
						if( MencV->OrgOk )
						{ /* エンコーダ位置補正チャネル0から分周C相2に設定 */
							Enc->SencSetReg->AREG_DIVSET |= (BIT11 | BIT8);
						}
					}
				}
				else
				{	/* パルス変換C相マスクON設定			*/
					Enc->SencSetReg->AREG_WDT1SET |= BIT8;
					/* エンコーダ位置補正チャネル0から分周C相通常に設定 */
					Enc->SencSetReg->AREG_DIVSET |= BIT8;
				}
			}
		/*----------------------------------------------------------------------------------------------*/
		}
		else/* <S21B>:リニア処理追加 */
		{/* ﾘﾆｱ型 */
			MencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dMotPos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem);
			SHalDrv_SetDivOutPosition( Enc->SencSetReg, (ULONG)MencV->DivInfo.DivOutPos );			
		}
	}

#endif/*<S02D>*/
/*--------------------------------------------------------------------------------------------------*/
	return;
}

/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ位置データ受信処理(HW:JL-086)										*/
/*																									*/
/****************************************************************************************************//* <S004> */
static void	SencRcvPosHwProcess( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg, ALMMAN_HANDLE *AlmManager )/*<S0C5>*/
{
	/*----------------------------------------------------------------------------------------------*/
	/*		アラーム時の処理																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->SeRxErr   = SHal_GetSeRxErr( pAsicHwReg );		/* 受信エラーフラグ更新 				*/
	MencV->EncDataNG = SHal_GetEncDataNG( pAsicHwReg );		/* エンコーダデータ異常フラグの更新 	*/
	/*----------------------------------------------------------------------------------------------*/
	if( SHal_GetComAlarm( pAsicHwReg ) )					/* エラー種別判定(通信／加速度)			*/
	{
		ALMSetServoAlarm( AlmManager, ALM_PGCOM );			/* A.C90 : エンコーダ通信異常			*//*<S0C5>*/
	}
	if( SHal_GetAccAlarm( pAsicHwReg ) )					/* エラー種別判定(通信／加速度)			*/
	{
		ALMSetServoAlarm( AlmManager, ALM_PGCOM1 );			/* A.C91 : エンコーダ通信異常(加速度)	*//*<S0C5>*/
	}
	/*----------------------------------------------------------------------------------------------*/
	/*		補正無し位置データ更新																	*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RcvPosX[2] = MencV->RcvPosX[1];			/*<S0C5>*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];			/*<S0C5>*/
	MencV->RcvPosX[0] = (LONG)( SHal_GetCurRcvPos( pAsicHwReg ) << MencV->MposSftR );
	/*----------------------------------------------------------------------------------------------*/
	/*		受信データL,H更新																		*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->RxPosL[1].Long = MencV->RxPosL[0].Long;	/*<S0C5>*/
	MencV->RxPosH[1] 	  = MencV->RxPosH[0];		/*<S0C5>*/
	MencV->RxPosL[0].Long = (LONG) SHal_GetCurRxPosL( pAsicHwReg );
	MencV->RxPosH[0] 	  = (SHORT)SHal_GetCurRxPosH( pAsicHwReg );
	/*----------------------------------------------------------------------------------------------*/
	/*		変化位置タイマ、ベースタイマ更新														*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->BaseTimer	  = (USHORT)SHal_GetBaseTim( pAsicHwReg );									  /* <S067> */
	MencV->HenkaItiTimer  = (USHORT)SHal_GetHenkaItiTim( pAsicHwReg );								  /* <S067> */

	/*----------------------------------------------------------------------------------------------*/
	SHal_SrcClr( pAsicHwReg );								/* 受信完了フラグ更新					*/
	/*----------------------------------------------------------------------------------------------*/
	return;
}


/****************************************************************************************************/
/*																									*/
/*		回転型：シリアルエンコーダ位置データ受信処理(SW)											*/
/*																									*/
/****************************************************************************************************//* <S004> */
static void	SencRcvPosSwProcess( MENCV *MencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete )	  /* <S0C5> */
{
	ULONG	ulwk;
	LONG	lwk1;
	LONG	RcvPosX;
	SENCX	*Enc;
	ULONG	RxFlg;

	Enc	= &MencV->EncIf;

/*--------------------------------------------------------------------------------------------------*/
/*		受信未完時 : シリアルエンコーダ受信異常処理													*/
/*--------------------------------------------------------------------------------------------------*/
	RxFlg = SHalDrv_SencGetRxStatus( (void*)Enc->RxFlg );
	if( ((RxFlg & 0x8000) == 0) && (MencV->SeRxComp == FALSE) )
	{
		MencV->ComErrCnt++;									/* Count up Error Counter				*/
		MencV->SeRxErr = TRUE;								/* Set Error Flag						*/
		LpxSencComErrorProc(MencV, AlmManager, 0, FALSE);	/*<S0C5>*/
	/*----------------------------------------------------------------------------------------------*/
	/*		エンコーダデータ無効時の処理															*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->EncDataNG == TRUE )
		{
			if( BeComplete == FALSE )								/* ベースブロック時				*/
			{
			/*--------------------------------------------------------------------------------------*/
			/*	前回値・前々回値更新処理															*/
			/*		エンコーダデータ無効時 && ベースブロック時に、									*/
			/*		今回値と前回値の差分で計算される速度を０にするため前回値を更新する。			*/
			/*--------------------------------------------------------------------------------------*/
				MencV->RxPosL[1]  = MencV->RxPosL[0];				/* 前回値更新					*/
				MencV->RxPosH[1]  = MencV->RxPosH[0];				/* 前回値更新					*/
				MencV->RcvPosX[2] = MencV->RcvPosX[1];				/* 前々回値更新					*/
				MencV->RcvPosX[1] = MencV->RcvPosX[0];				/* 前回値更新					*/
			}
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		受信完了時 : シリアルエンコーダ受信完了処理													*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
	/*----------------------------------------------------------------------------------------------*/
	/*	受信データ取得																				*/
	/*----------------------------------------------------------------------------------------------*/
		SHalDrv_SencGetRxData( (void*)Enc->RxBuf, (void*)&Enc->RxData );
	/*----------------------------------------------------------------------------------------------*/
	/*	受信完Flagのリセット：IntSync Set --> ScanA Clear											*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->SeRxComp = FALSE;									/* Reset Rx Complete Flag		*/
	/*----------------------------------------------------------------------------------------------*/
	/*	受信データを内部変数に設定																	*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->RxPosL[1]		= MencV->RxPosL[0];					/* 前回値更新					*/
		MencV->RxPosH[1]		= MencV->RxPosH[0];					/* 前回値更新					*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->RxAlmCode		= (USHORT)(Enc->RxData.UsBuf[2]);	/* 今回値読込み：Alarm Code		*/
		MencV->RxTimex.w.l		= (USHORT)(Enc->RxData.UsBuf[3]);	/* 今回値読込み：TimeBase, etc	*/
		MencV->RxTimex.w.h		= (USHORT)(Enc->RxData.UsBuf[4]);	/* 今回値読込み：TimeBase, etc	*/
		MencV->RxPosL[0].w.l	= (USHORT)(Enc->RxData.UsBuf[5]);	/* 今回値読込み：Position Low	*/
		MencV->RxPosL[0].w.h	= (USHORT)(Enc->RxData.UsBuf[6]);	/* 今回値読込み：Position High	*/
		MencV->RxPosH[0]		= (USHORT)(Enc->RxData.UsBuf[7]);	/* 今回値読込み：Incre. Compen.	*/
	/*----------------------------------------------------------------------------------------------*/
	/*	位置演算																					*/
	/*----------------------------------------------------------------------------------------------*/
		RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long >> MencV->MposSftX) << MencV->MposSftR);
	/*----------------------------------------------------------------------------------------------*/
	/*	加速度演算チェック																			*/
	/*----------------------------------------------------------------------------------------------*/
		if( MencV->AccChkEnable == FALSE )
		{
			lwk1 = 0;
			MencV->AccChkWaitCnt++;
			if (MencV->AccChkWaitCnt > 3)
			{
				MencV->AccChkEnable = TRUE;
			}
			MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = RcvPosX;
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	加速度演算 : 今回速度 - 前回速度 (今回の加速度が異常な場合は,前々回のデータで再計算)		*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			lwk1 = (RcvPosX - MencV->RcvPosX[0]) - (MencV->RcvPosX[0] - MencV->RcvPosX[1]);
			if( (lwk1 > MencV->AccErrLvl) || (lwk1 < -(MencV->AccErrLvl)) )
			{
				lwk1 = ((RcvPosX - MencV->RcvPosX[1])>>1) - (MencV->RcvPosX[1] - MencV->RcvPosX[2]);
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	加速度異常時 : シリアルエンコーダ通信異常処理												*/
	/*----------------------------------------------------------------------------------------------*/
		if( (lwk1 > MencV->AccErrLvl) || (lwk1 < -(MencV->AccErrLvl)) )
		{
			MencV->AccErrCnt++;								/* Count up Error Counter				*/
			MencV->SeRxErr = TRUE;							/* Set Error Flag						*/
			LpxSencComErrorProc(MencV, AlmManager, 0, FALSE);/*<S0C5>*/
		/*------------------------------------------------------------------------------------------*/
		/*		エンコーダデータ無効時の処理														*/
		/*------------------------------------------------------------------------------------------*/
			if( MencV->EncDataNG == TRUE )
			{
				if( BeComplete == FALSE )								/* ベースブロック時			*/
				{
				/*----------------------------------------------------------------------------------*/
				/*	前回値・前々回値更新処理														*/
				/*		エンコーダデータ無効時 && ベースブロック時に、								*/
				/*		今回値と前回値の差分で計算される速度を０にするため前回値を更新する。		*/
				/*----------------------------------------------------------------------------------*/
					MencV->RxPosL[1]  = MencV->RxPosL[0];				/* 前回値更新				*/
					MencV->RxPosH[1]  = MencV->RxPosH[0];				/* 前回値更新				*/
					MencV->RcvPosX[2] = MencV->RcvPosX[1];				/* 前々回値更新				*/
					MencV->RcvPosX[1] = MencV->RcvPosX[0];				/* 前回値更新				*/
				}
			}
		}
	/*----------------------------------------------------------------------------------------------*/
	/*	加速度正常時 : モータ磁極位相演算															*/
	/*----------------------------------------------------------------------------------------------*/
		else
		{
			MencV->MstErrCnt   = 0;							/* Reset Error Counter					*/
			MencV->SeRxErr     = FALSE;						/* Reset Error Flag						*/
			MencV->EncDataNG   = FALSE;						/* Reset Encoder Data NG Flag			*/
			MencV->RcvPosX[2]  = MencV->RcvPosX[1];			/* 前々回位置データ						*/
			MencV->RcvPosX[1]  = MencV->RcvPosX[0];			/* 前回位置データ						*/
			MencV->RcvPosX[0]  = RcvPosX;					/* 今回位置データ						*/
		}
	}

	/*----------------------------------------------------------------------------------------------*/
	/*	ベースタイマ値、変化位置タイマ値作成			<S067>										*/
	/*----------------------------------------------------------------------------------------------*/
	MencV->BaseTimer = (USHORT)(MencV->RxTimex.dw >> 8);
	ulwk = ((ULONG)MencV->RxPosL[0].w.l << 16) + (ULONG)MencV->RxTimex.w.h;
	MencV->HenkaItiTimer = (USHORT)(ulwk >> 8);

	return;
}


/****************************************************************************************************/
/*																									*/
/*		リニアモータ：モータ電流位相更新処理	<S018>												*/
/*																									*/
/****************************************************************************************************/
#define	LMX_MORG_CHKMAX		3
/*--------------------------------------------------------------------------------------------------*/
static void	LmxUpdateMotPhase( MENCV *MencV, ALMMAN_HANDLE *AlmManager, LONG dMotPos )
{
	LONG	lwk1;
	LONG	lwk2;
	LONG	LastMotPhaX;
	LONG	NewPoleInfo;
	LONG	MotPoleInfoX;

/*--------------------------------------------------------------------------------------------------*/
/*		モータ位相用位置演算																		*/
/*--------------------------------------------------------------------------------------------------*/
		LastMotPhaX = MencV->MotPhaX;						/* 前回値保存							*/
		lwk1 = MencV->MotPhaX + dMotPos;					/* 今回値演算							*/
	/*----------------------------------------------------------------------------------------------*/
		if( lwk1 >= (LONG)MencV->PulseNo )					/* Check Upper Limit					*/
		{													/*										*/
			lwk1 -= (LONG)MencV->PulseNo;					/* Adjust MotPos by PulseNo				*/
		}													/*										*/
		else if( lwk1 < 0 )									/* Check Lower Limit					*/
		{													/*										*/
			lwk1 += (LONG)MencV->PulseNo;					/* Adjust MotPos by PulseNo				*/
		}													/*										*/
	/*----------------------------------------------------------------------------------------------*/
		MencV->MotPhaX = lwk1;								/* モータ位相用位置更新					*/

/*--------------------------------------------------------------------------------------------------*/
/*		ポールセンサありの場合																		*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MencV->withPoleSensor) && (!MencV->MotOrgChkEnd) && (MencV->MstErrCnt == 0) )
		{
			if( MencV->Sigma3Encoder )
			{
				NewPoleInfo = (MencV->RxPosH[0]>>1) & 0x07;
			}
			else
			{
				NewPoleInfo = (MencV->RxPosL[0].w.l>>9) & 0x07;
			}
			//if( MencV->PoleReverse == TRUE ){ NewPoleInfo = ~NewPoleInfo & 0x07; }	/* <V808>		*///あとで対応すること！！kira
			MotPoleInfoX = MencV->MotPoleInfo[0];
			MlibBITFILTER1( MencV->MotPoleInfo[0], MencV->MotPoleInfo[1], (UCHAR)NewPoleInfo );
			MotPoleInfoX = ((MencV->MotPoleInfo[0] ^ MotPoleInfoX)<<4) | MencV->MotPoleInfo[0];
		/*------------------------------------------------------------------------------------------*/
		/*	ＵＶＷ信号：異常チェック：UVW = 000 or 111 で異常										*/
		/*------------------------------------------------------------------------------------------*/
			if( (MencV->MotPoleInfo[0] == 0x00) || (MencV->MotPoleInfo[0] == 0x07) )
			{
				ALMSetServoAlarm( AlmManager, ALM_POLESENSOR );	/* A.C21 : ポールセンサ異常 		*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	ＵＶＷ信号：Ｕ相エッジチェック：MotPoleInfoX = UVW-Edge(6,5,4bit) + UVW-Sts(2,1,0bit) 	*/
		/*------------------------------------------------------------------------------------------*/
			else if( (MotPoleInfoX & 0x43) == 0x41 )		/* U-Edge & W-ON : 0 deg				*/
			{
				if( MencV->MotOrgChkCnt < LMX_MORG_CHKMAX )
				{
					++MencV->MotOrgChkCnt;					/* Count up Motor Origin Check Counter	*/
				}
				if( (MencV->MotOrgChkAlways == FALSE) && (MencV->MotOrgChkCnt >= LMX_MORG_CHKMAX) )
				{
					MencV->MotOrgChkEnd = TRUE;				/* Set Motor Origin Check End Flag		*/
				}
			/*--------------------------------------------------------------------------------------*/
			/*	モータ原点チェック＆更新：ビットフィルタの遅れを考慮して前回値を使用する			*/
			/*--------------------------------------------------------------------------------------*/
				lwk1 = MlibABS( (LastMotPhaX - MencV->MotOrgXchk[0]) );
				lwk2 = MlibABS( (LastMotPhaX - MencV->MotOrgXchk[1]) );
				/*----------------------------------------------------------------------------------*/
				if( lwk1 > MencV->HalfPulseNo )		/*<V498>*/
				{
					lwk1 = MencV->PulseNo - lwk1;
				}
				if ( lwk2 > MencV->HalfPulseNo )		/*<V498>*/
				{
					lwk2 = MencV->PulseNo - lwk2;
				}
				/*----------------------------------------------------------------------------------*/
				if( lwk1 <= MencV->PulseNo30deg )
				{
					MencV->MotOrgX = LastMotPhaX;			/* モータ位相原点更新					*/
				}
				else if( (MencV->MotOrgChkCnt >= LMX_MORG_CHKMAX) && (lwk2 > MencV->PulseNo30deg) )
				{
					ALMSetServoAlarm( AlmManager, ALM_PHASE );/* A.C20 : 位相誤検出					*/
				}
			/*--------------------------------------------------------------------------------------*/
				MencV->MotOrgXchk[1] = MencV->MotOrgXchk[0];/* Update Motor Origin Check Data		*/
				MencV->MotOrgXchk[0] = LastMotPhaX;			/* Update Motor Origin Check Data		*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	ＵＶＷ信号：Ｖ相エッジチェック(必要に応じて追加,相順注意要)								*/
		/*------------------------------------------------------------------------------------------*/
			else if( 0 && (MotPoleInfoX & 0x25) == 0x24 )	/* V-Edge & U-ON : 120deg/240deg		*/
			{
				;											/* 相順UVW時：120deg					*/
				;											/* 相順UWV時：240deg					*/
			}
		/*------------------------------------------------------------------------------------------*/
		/*	モータ位相計算：演算結果はUSHORTを使用するので符号補正は不要(試験で確認する)			*/
		/*------------------------------------------------------------------------------------------*/
			lwk1 = MlibMulgainNolim( (MencV->MotPhaX - MencV->MotOrgX), MencV->Kmotphase );
			MencV->MotPhase = (USHORT)(lwk1 + MencV->PhaseOffset);
		}
/*--------------------------------------------------------------------------------------------------*/
/*		ポールセンサなしの場合																		*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			lwk1 = MlibMulgainNolim( (MencV->MotPhaX - MencV->MotOrgX), MencV->Kmotphase );
			MencV->MotPhase = (USHORT)(lwk1 + MencV->PhaseOffset + MencV->MpfPhaseOffset);
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}


#ifdef _TIME_BASE_TIMER_
/****************************************************************************************************/
/*																									*/
/*		変化時刻タイマを利用した位置補正演算														*/
/*																									*/
/*		概要：エンコーダ情報の変化時刻タイマを用いてエンコーダ位置を補正する						*/
/*			  エンコーダのサンプリングと、サーボの制御周期のビート現象対策((M技D1)V03-010)			*/
/*			  サンプリングのずれを補正するために、エンコーダが最後のパルスをラッチしてから			*/
/*			  経過した時間（変化時刻タイマ）を使用する。											*/
/*			  変化時刻タイマ値が2.66us未満のとき、ベースタイマの2.66us前の位置を算出する。			*/
/*																									*/
/*		注意: 演算のオーバフローを防止するため引数の[2^32/rev]単位を[pulse]へ換算し、演算後に		*/
/*			  単位を戻す。																			*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		arg		：	IN		LONG	RcvPosX		：今回エンコーダ位置(補正前)	[2^32/rev]			*/
/*				：	IN		LONG	LastMotPosX	：前回エンコーダ位置(補正後)	[2^32/rev]			*/
/*				：	IN		UCHAR	MposSftR	：位置データシフト数(32-BitNo)	[-]					*/
/*				： OUT		None																	*/
/*--------------------------------------------------------------------------------------------------*/
/*		return	：			LONG				  今回エンコーダ位置(補正後)	[2^32/rev]			*/
/*--------------------------------------------------------------------------------------------------*/
/*		g-ref	：	MencV.RxPosL				: 変化時刻タイマ参照用								*/
/*					MencV.RxTimex.w.h			: 同上												*/
/*					MencP.MposSftR				: [pulse]→[2^32]変換用シフト数						*/
/*--------------------------------------------------------------------------------------------------*/
/*		MACRO	:	RMX_XposToUpos	: [2^32/rev]→[pulse]											*/
/****************************************************************************************************/
LONG	LpxTimeBasesMotPosCalc( MENCV *MencV )
{
	LONG	MotPosCmp;										/* 今回補正後エンコーダ位置[pulse]		*/
	ULONG	ulwk;											/* ワーク								*/
	SHORT	swk;											/* ワーク								*/
	USHORT	HenkaItiTimer;									/* 変化位置タイマ						*/
	LONG	LinEncPos;																				/* <S004> */

	LinEncPos = 1 << (32 - MencV->MposSftR) ;				/* リニア用エンコーダ位置オーバーフロー	*//* <S004> */
		/*------------------------------------------------------------------------------------------*/
		/*	演算準備 ： 補正演算にてオーバフローしないように単位を換算する							*/
		/*				[2^32/rev]→[pulse/rev]														*/
		/*------------------------------------------------------------------------------------------*/
		MotPosCmp = ((ULONG)MencV->RcvPosX[0]) >> MencV->MposSftR;	/* 初期化						*/
		if( MencV->SeRxErr == FALSE )						/* 受信エラー or 加速度異常時補正無し	*/
		{
			/*--------------------------------------------------------------------------------------*/
			/*	変化時刻タイマ取得																	*/
			/*																						*/
			/*				  変化位置タイマ														*/
			/*	___________________↓____________________											*/
			/*	|         |/////////|/////////|         |											*/
			/*	￣￣￣￣￣↑￣￣￣￣￣￣￣￣￣↑￣￣￣￣											*/
			/*	 MencV.RxPosL[0].w.l MencV.RxTimex.w.h												*/
			/*																						*/
			/*--------------------------------------------------------------------------------------*/
			ulwk = ((ULONG)MencV->RxPosL[0].w.l << 16) + (ULONG)MencV->RxTimex.w.h;
			HenkaItiTimer = (USHORT)(ulwk >> 8);
			/*--------------------------------------------------------------------------------------*/
			/*	位置補正																			*/
			/*																						*/
			/*	　計算式：P(t-2.66[us]) = P(t) + (P(t) - P(t-1)) × (Δt-2.66[us])/62.5[us]		*/
			/*																						*/
			/*			(Δt - 2.66[us]) / 62.5[us] = Δt / 62.5[us] - 2.66[us]/62.5[us]			*/
			/*		エンコーダクロック＝6MHzを分母，分子に掛けて，クロック数に変換すると，			*//* <S004> */
			/*			= (Δt*6[MHz] - 2.66[us]*6[MHz])/(62.5[us]*6[MHz])							*/
			/*																						*/
			/*		1 / (62.5[us]*6[MHz]) -> 1 / SAENCCLKNUM(375)									*/
			/*		2.66[us]*6[MHz] -> ENCLCHCLKNUM(16)												*/
			/*		Δt*6[MHz] -> 変化位置タイマカウンタ数											*/
			/*																						*/
			/*--------------------------------------------------------------------------------------*/
			if(HenkaItiTimer <= ENCLCHCLKNUM)
			{	/* エンコーダラッチ周期以下	*//* <S004> */
				swk = (SHORT)(HenkaItiTimer - ENCLCHCLKNUM);
				MotPosCmp = MotPosCmp + ((((MencV->RcvPosX[0] - MencV->MotPosX[0])>>MencV->MposSftR ) * swk) / SAENCCLKNUM);
				if( MotPosCmp >= 0 )
				{
					//no operation 						/* <S004> */
					//MotPosCmp = MotPosCmp;			/* <S004> */
				}
				else
				{
					if(( MencV->AxisMotType == MOTTYPE_LINEAR ))
					{
						MotPosCmp = MotPosCmp + LinEncPos;
					}
					else
					{
						MotPosCmp = MotPosCmp + MencV->PulseNo;
					}
				}
			}
		}
		/*------------------------------------------------------------------------------------------*/
		/*	演算後処理 ： 単位を戻す	[pulse/rev]	→ [2^32/rev]									*/
		/*------------------------------------------------------------------------------------------*/
		return( MotPosCmp << MencV->MposSftR );

}
#endif /* _TIME_BASE_TIMER_ */



/* <S067> */
/****************************************************************************************************/
/*																									*/
/*		タイムベースタイマと変化時刻タイマを利用した速度演算										*/
/*																									*/
/*		概要：エンコーダ情報のベースタイマ，および変化時刻タイマを用いて速度検出する				*/
/*			  従来の位置差分方式では，位置ラッチ間隔が62.5μsであることを前提としているが，			*/
/*			  実際には，エンコーダのラッチ間隔により微妙にズレている。								*/
/*			  これがビートになったりして悪影響を及ぼすことがある。									*/
/*			  エンコーダタイマ情報を利用することにより，速度検出精度の向上が期待できる。			*/
/*			  ただし，エンコーダ位置差分が１パルス以下の場合は，変化時間間隔が異常に小さくなる場合	*/
/*			  があるので，位置差分２パルス以上で本方式を用いる										*/
/*--------------------------------------------------------------------------------------------------*/
/*			  実行要求フラグがTRUE の場合は速度補正演算を行う。										*/
/*			  実行要求フラグがFALSEの場合は Vfbtimbase.LastHenkaTime, Vfbtimbase.LastBaseTimer		*/
/*			  の作成のみ行う。																		*/
/*			  実行要求フラグは電源投入時、パラメータ再計算時にFALSEとなる。							*/
/*			  新設した理由はP1モードで定周期にエンコーダ受信を開始してから処理を始めるため。		*/
/*			  実際にはLastModflag作成にも１スキャン必要なため、電源投入後ScanAが実行されはじめて	*/
/*			  ３スキャン目から補正が開始される。													*/
/*																									*/
/*																									*/
/*		arg		：	LONG	dPos	：エンコーダ位置差分			[pulse/scan]					*/
/*					LONG	dPosSpd	：エンコーダ位置差分速度		[2^24/OvrSpd]					*/
/*					UCHAR	EnableFlag：補正実行許可フラグ											*/
/*		g-ref	：																					*/
/*		out		：	LONG	motspeed：モータ速度					[2^24/OvrSpd]					*/
/*																									*/
/****************************************************************************************************/
LONG	TimeBasesMotSpdCalc( VFBTIMBASE *Vfbtimbase, MENCV *MencV, LONG dPos, LONG dPosSpd,
																	BOOL VcompSts, BOOL EnableFlag )
{

//	ULONG	ulwk;//<S0C7>
	LONG	lwk;

	/*----------------------------------------------------------------------------------------------*/
	/*	変化時刻タイマ取得																			*/
	/*																								*/
	/*				  変化位置タイマ																*/
	/*	___________________↓____________________													*/
	/*	|         |/////////|/////////|         |													*/
	/*	￣￣￣￣￣↑￣￣￣￣￣￣￣￣￣↑￣￣￣￣													*/
	/*	 MencV.RxPosL[0].w.l MencV.RxTimex.w.h														*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
//		ulwk = ((ULONG)MencV->RxPosL[0].w.l << 16) + (ULONG)MencV->RxTimex.w.h;						  /* <S067> */
//		Vfbtimbase->HenkaItiTimer = (USHORT)(ulwk >> 8);											  /* <S067> */
		Vfbtimbase->HenkaItiTimer = MencV->HenkaItiTimer;
	/*----------------------------------------------------------------------------------------------*/
	/*	ベースタイマ取得																			*/
	/*																								*/
	/*				  ベースタイマ																	*/
	/*	___________________↓____________________													*/
	/*	|         |/////////|/////////|         |													*/
	/*	￣￣￣￣￣↑￣￣￣￣￣￣￣￣￣↑￣￣￣￣													*/
	/*	  MencV->RxTimex.w.h    MencV->RxTimex.w.l													*/
	/*																								*/
	/*----------------------------------------------------------------------------------------------*/
//		Vfbtimbase->BaseTimer = (USHORT)(MencV->RxTimex.dw >> 8);									  /* <S067> */
		Vfbtimbase->BaseTimer = MencV->BaseTimer;
	/*----------------------------------------------------------------------------------------------*/
	/*	変化時刻演算																				*/
	/*----------------------------------------------------------------------------------------------*/
		Vfbtimbase->HenkaTime = Vfbtimbase->BaseTimer - Vfbtimbase->HenkaItiTimer;
		Vfbtimbase->dPos = dPos;							/* 今回差分位置ＦＢ取得					*/

	/*----------------------------------------------------------------------------------------------*/
	/*	補正有効判定																				*/
	/*----------------------------------------------------------------------------------------------*/
		/*------------------------------------------------------------------------------------------*/
		/*	補正無効時処理																			*/
		/*------------------------------------------------------------------------------------------*/
		if( EnableFlag == FALSE )
		{
			Vfbtimbase->Modflag = FALSE;				/* 無補正モード設定							*/
			Vfbtimbase->MotSpeed = dPosSpd;				/* 無補正のまま出力							*/
		}
		else
		{
		/*------------------------------------------------------------------------------------------*/
		/*	補正有効時処理																			*/
		/*------------------------------------------------------------------------------------------*/
			/*--------------------------------------------------------------------------------------*/
			/*	変化時間間隔演算																	*/
			/*--------------------------------------------------------------------------------------*/
			Vfbtimbase->TimeInterval = Vfbtimbase->HenkaTime - Vfbtimbase->LastHenkaTime;
			Vfbtimbase->BaseTimeInterval = Vfbtimbase->BaseTimer - Vfbtimbase->LastBaseTimer;
			/*--------------------------------------------------------------------------------------*/
			/*	速度演算実行判別																	*/
			/*--------------------------------------------------------------------------------------*/
			if(MlibABS(dPos) >= VFBTINMINPLS)					/* 差速度が2パルス以上				*/
			{
				if( Vfbtimbase->ModChgCnt < VFBTINMINCNT )		/* 5回連続なら変化位置タイマ方式	*/
				{
					Vfbtimbase->ModChgCnt++;
				}
			}
			else
			{
				Vfbtimbase->ModChgCnt = 0;
			}

			/*--------------------------------------------------------------------------------------*/
			/*	常時変化位置タイマ実行処理															*/
			/*--------------------------------------------------------------------------------------*/
//			if( (Iprm.TuningLessMode & 0x100)					/* 速度検出切替えあり				*/
//				&& ( Gprm.f.TuningLessVComp == FALSE) )			/* 速度制御として使用中				*/
			if( VcompSts == FALSE )								/* 速度検出切替えあり				*/
			{													/* 速度制御として使用中				*/
				if(Vfbtimbase->TimeInterval > SAENCCLKNUMC2)	/* 変化時間間隔が制御周期×2超過	*/
				{
					Vfbtimbase->TimeInterval = SAENCCLKNUMC2;
				}
				else if(MlibAbs32(Vfbtimbase->dPos) <= 1)		/* 今回位置差分FBが1パルス以下		*/
				{
					if(Vfbtimbase->TimeInterval < SAENCCLKNUM)	/* 変化時間間隔が制御周期未満		*/
					{
						Vfbtimbase->TimeInterval = SAENCCLKNUM;
					}
				}
				Vfbtimbase->ModChgCnt = VFBTINMINCNT;			/* 常時変化位置タイマ補正有効		*/
			}

			if ( (Vfbtimbase->ModChgCnt < VFBTINMINCNT)				/* 5回連続2パルス以上ではない	*/
				|| (Vfbtimbase->TimeInterval < SAENCCLKNUMC2D3 )	/* 変化時間が最小値未満			*/
				|| (MencV->SeRxErr == TRUE) )						/* Set Error Flag				*/
			{
				Vfbtimbase->Modflag = FALSE;
			}
			else
			{
				Vfbtimbase->Modflag = TRUE;
			}

			if (Vfbtimbase->Modflag && Vfbtimbase->LastModflag)
			{
				/*----------------------------------------------------------------------------------*/
				/* 変化位置タイマを使用した速度演算													*/
				/*----------------------------------------------------------------------------------*/
				/* 注意：Vfbtimbase->LastModflagの初期値はFALSEである。よって，電源投入初回の速度	*/
				/*		 検出は位置差分方式となる。よって，Vfbtimbase->LastHenkaTimeは必ずセット	*/
				/*		 されてから本if文内に入る。したがってVfbtimbase->LastHenkaTimeの初期値は0で	*/
				/*		 良い																		*/
				/*----------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	モータ速度演算																	*/
				/*																					*/
				/*					dPosSpd(ｴﾝｺｰﾀﾞ位置差分速度) * 速度演算周期[ﾍﾞｰｽﾀｲﾏｸﾛｯｸ数]		*/
				/*	motorspeed = ---------------------------------------------------------------	*/
				/*							TimeInterval(変化時刻間隔)[ﾍﾞｰｽﾀｲﾏｸﾛｯｸ数]				*/
				/*																					*/
				/*	※速度演算周期のﾍﾞｰｽﾀｲﾏｸﾛｯｸ数：速度演算周期:62.5[us]時は375ｸﾛｯｸ(ﾍﾞｰｽﾀｲﾏ:6MHz)	*/
				/*																					*/
				/*	motorspeedを単純に計算すると最高回転付近でDwordを超えてしまう。					*/
				/*	そのため，まずｴﾝｺｰﾀﾞ位置差分速度を２bitｼﾌﾄし演算周期分を乗算，変化時刻で除算後	*/
				/*	２bit元に戻す。	演算上の速度検出分解能が２bit落ちるが実質影響ない。				*/
				/*	本処理は，速度演算周期のﾍﾞｰｽﾀｲﾏｸﾛｯｸ数 < 128の場合は,Dwordを超えないので不要		*/
				/*																					*/
				/*----------------------------------------------------------------------------------*/
				if((Vfbtimbase->dPos == 0)								/* 今回ＦＢ＝ゼロ			*/
					 && (Vfbtimbase->LastdPos > 0))						/* かつ前回ＦＢ≠ゼロ		*/
				{
					/* 前回速度を維持		*/
				}
				else
				{
					lwk = (dPosSpd >> 2) * SAENCCLKNUM;
					lwk = lwk / (LONG)Vfbtimbase->TimeInterval;
					Vfbtimbase->MotSpeed = lwk << 2;
				}	/*<V334>*/
			}
			else if((Vfbtimbase->BaseTimeInterval != 0)		/* ﾍﾞｰｽﾀｲﾏ変化時間なし -> ｴﾝｺｰﾀﾞｴﾗｰ時	*/
				&& (Vfbtimbase->LastSeRxErr == FALSE) )				/* Last Error Flag				*//*<V648>*/
			{
				/*----------------------------------------------------------------------------------*/
				/* ベースタイマを使用した速度演算													*/
				/*----------------------------------------------------------------------------------*/
				/*	モータ速度演算																	*/
				/*																					*/
				/*					dPosSpd(ｴﾝｺｰﾀﾞ位置差分速度) * 速度演算周期[ﾍﾞｰｽﾀｲﾏｸﾛｯｸ数]		*/
				/*	motorspeed = ---------------------------------------------------------------	*/
				/*						BaseTimeInterval(ﾍﾞｰｽﾀｲﾏ変化時間)[ﾍﾞｰｽﾀｲﾏｸﾛｯｸ数]			*/
				/*																					*/
				/*	※速度演算周期のﾍﾞｰｽﾀｲﾏｸﾛｯｸ数：速度演算周期:62.5[us]時は375ｸﾛｯｸ(ﾍﾞｰｽﾀｲﾏ:6MHz)	*/
				/*																					*/
				/*																					*/
				/*	motorspeedを単純に計算すると最高回転付近でDwordを超えてしまう。					*/
				/*	そのため，まずｴﾝｺｰﾀﾞ位置差分速度を２bitｼﾌﾄし演算周期分を乗算，変化時刻で除算後	*/
				/*	２bit元に戻す。	演算上の速度検出分解能が２bit落ちるが実質影響ない。				*/
				/*	本処理は，速度演算周期のﾍﾞｰｽﾀｲﾏｸﾛｯｸ数 < 128の場合は,Dwordを超えないので不要		*/
				/*																					*/
				/*----------------------------------------------------------------------------------*/
				lwk = (dPosSpd >> 2) * SAENCCLKNUM;
				lwk = lwk / (LONG)Vfbtimbase->BaseTimeInterval;
				Vfbtimbase->MotSpeed = lwk << 2;
			}
			else
			{
				/*----------------------------------------------------------------------------------*/
				/* 補正条件未達の場合：エンコーダ位置差分速度演算使用								*/
				/*----------------------------------------------------------------------------------*/
				Vfbtimbase->MotSpeed = dPosSpd;
			}
		}
		/*------------------------------------------------------------------------------------------*/
		/*	今回値保存																				*/
		/*------------------------------------------------------------------------------------------*/
		Vfbtimbase->LastHenkaTime	= Vfbtimbase->HenkaTime;
		Vfbtimbase->LastBaseTimer	= Vfbtimbase->BaseTimer;
		Vfbtimbase->LastModflag		= Vfbtimbase->Modflag;
		Vfbtimbase->LastdPos		= Vfbtimbase->dPos;
		Vfbtimbase->LastSeRxErr		= MencV->SeRxErr;

		return (Vfbtimbase->MotSpeed);
}



#if 1 /*<S02D> 以下の関数は暫定(フルク対応時検討必要) */
#define	SVFSEL_PAOPOUT		1				/* ＰＡＯ相パルス出力			*//*<S02D>*/
/****************************************************************************************************/
/*																									*/
/*		フルクエンコーダ：初期インクレパルス出力処理					<S02D>						*/
/*																									*/
/****************************************************************************************************/
#define FMAXPULSEOUT		170
static void	FencSencOutIniIncPulse( MENCV  *FencV, ASIC_HW_IF_STR  *pAsicHwReg )
{
LONG	lwk;
LONG	MtnCur;
LONG	MtnSub;
LONG	MtnRem;
LONG	MtnSepos;
SHORT	MturnLatchCur;
SHORT	PosH;
DWORDX	PosL;

/*--------------------------------------------------------------------------------------------------*/
/*		通信異常チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( FencV->SeRxErr == TRUE )
		{
			return;											/* 通信異常時は、何もしない				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		スケール逆転(Pn002.3)処理 (36bit絶対値データの反転処理)										*/
/*--------------------------------------------------------------------------------------------------*/
/*	        16bit               20bit              12bit											*/
/*	 |<---------------->|<------------------->|<---------->|   										*/
/*   +------------------+---------------------+------------+   										*/
/*   | FencV->RxPosH[0] |  FencV->RxPosL.dw     xxxxxxxxxxxx| 下位12bitは不定値						*/
/*   +------------------+---------------------+------------+   										*/
/*   |  PosH            |  PosL.dw             000000000000| 下位12bitは0固定値						*/
/*   +------------------+---------------------+------------+   										*/
/*--------------------------------------------------------------------------------------------------*/
		PosH    = FencV->RxPosH[0];
		PosL.dw = (FencV->RxPosL[0].dw >> FencV->MposSftR) << FencV->MposSftR; /* 下位12bitを0にする	*/

		if( FencV->RevAsmMotor )							/* モータ反転接続Flagチェック			*/
		{ 
			PosH    = (~PosH);								/* 符号反転処理							*/
			PosL.dw = (~PosL.dw) + 1;						/* 符号反転処理							*/
			if( PosL.dw == 0 ){ ++PosH;}					/* Carryの場合は、上位に+1				*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		マルチターンデータ取得処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		if( FencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_REQUEST )	/* ラッチ要求コマンドチェック		*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*		絶対位置36Bitデータの上位32Bitデータを取り出し、マルチターンの分周値を計算する		*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur    = ((USHORT)PosH << 16 ) | ((USHORT)PosL.w.h );
			MtnRem    = 0;
			MtnCur    = MlibPfbkxremNolim( MtnCur, FencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (PosH << (32-FSX_FPOS_SFT)) | ((ULONG)PosL.dw >> FSX_FPOS_SFT);
			FencV->DivInfo.DivOutRem = 0;
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )	/* <V526> */
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			FencV->DivInfo.MturnLatch = (SHORT)( MtnCur >> 16);
			FencV->DivInfo.PsoMturnLatch=(SHORT)( MtnCur >> 16);/*<S189>*/
		/*------------------------------------------------------------------------------------------*/
		/*		分周パルスリセット処理																*/
		/*------------------------------------------------------------------------------------------*/
			FencV->DivInfo.DivOutPos   = 0;				/* 分周出力パルスリセット				*/
			FencV->DivInfo.IncPulseOut = 0;				/* インクレパルス出力リセット			*/
			FencV->DivInfo.PaoSeqMcmd  = KPX_MTLATCH_FINISH;/* 応答 ==> FencSencOutputPaoSerial()  */
		}
/*--------------------------------------------------------------------------------------------------*/
/*		初期インクレパルス出力処理																	*/
/*--------------------------------------------------------------------------------------------------*/
		else if( FencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_REQUEST )/* パルス出力コマンドチェック	*/
		{
		/*------------------------------------------------------------------------------------------*/
		/*	マルチターンデータの変化分演算															*/
		/*	絶対位置36Bitデータの下位32Bitデータを取り出し、初期インクレの分周値を計算する			*/
		/*	MtnSeposの値を判断しながらMtnCurを計算する。											*/
		/*------------------------------------------------------------------------------------------*/
			MtnCur        = ((USHORT)PosH << 16 ) | ((USHORT)PosL.w.h );
			MtnRem        = 0;
			MtnCur        = MlibPfbkxremNolim( MtnCur, FencV->DivInfo.DivOutGain, &MtnRem );
			MtnSepos = (PosH << (32-FSX_FPOS_SFT)) | ((ULONG)PosL.dw >> FSX_FPOS_SFT);
			FencV->DivInfo.DivOutRem   = 0;				/* 分周出力パルス余りリセット			*/
			MtnSepos =  MlibPfbkxremNolim( MtnSepos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem );
			if( ( (MtnCur>>15) & 0x00000001 ) ^ ( (MtnSepos>>19) & 0x00000001 ) )
			{
				if( (MtnCur>>15) & 0x00000001 )
				{
					MtnCur = MtnCur + 1;
				}
				else if( (MtnSepos>>19) & 0x00000001 )
				{
					MtnCur = MtnCur - 1;
				}
			}
			MturnLatchCur = (SHORT)( MtnCur >> 16);
			MtnSub = (USHORT)MturnLatchCur - (USHORT)FencV->DivInfo.MturnLatch;
		/*------------------------------------------------------------------------------------------*/
		/*		絶対位置36Bitデータの下位32Bitデータを取り出し、初期インクレの分周値を計算する		*/
		/*------------------------------------------------------------------------------------------*/
			MtnSepos = (MtnSepos & 0x000FFFFF );

			if( FencV->DivInfo.RvsDir )						/* 逆回転モードチェック					*/
			{
				MtnSepos = MtnSepos | 0xFFF00000;			/* 符号反転処理							*/
				MtnSepos = (ULONG)MtnSepos - ( MtnSub << (32-FSX_FPOS_SFT) );
			}
			else
			{
				MtnSepos = (ULONG)MtnSepos + ( MtnSub << (32-FSX_FPOS_SFT) );
			}
		/*------------------------------------------------------------------------------------------*/
		/*		初期インクレパルス出力データチェック＆出力データ設定								*/
		/*------------------------------------------------------------------------------------------*/
			lwk = MtnSepos - FencV->DivInfo.IncPulseOut;
			if( lwk > FMAXPULSEOUT )						/* パルス出力上限						*/
			{
				FencV->DivInfo.IncPulseOut += FMAXPULSEOUT;/* パルス出力データ設定					*/
			}
			else if( lwk < -FMAXPULSEOUT )					/* パルス出力下限						*/
			{
				FencV->DivInfo.IncPulseOut -= FMAXPULSEOUT;/* パルス出力データ設定					*/
			}
			else											/* 最終パルス出力						*/
			{
				FencV->DivInfo.IncPulseOut = MtnSepos;		/* パルス出力データ設定					*/
				FencV->DivInfo.PaoSeqMcmd  = KPX_IPLSOUT_FINISH;/* 応答 ==> FencSencOutputPaoSerial*/
				FencV->DivInfo.IncPulseReq = FALSE;			/* 初期インクレパルス出力完了			*/
			}
			FencV->DivInfo.DivOutPos = FencV->DivInfo.IncPulseOut;	/* 分周出力パルス更新			*/
		/*------------------------------------------------------------------------------------------*/
			pAsicHwReg->AREG_POSET0	= FencV->DivInfo.IncPulseOut;	/* パルス出力回路初期化			*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		コマンド無し																				*/
/*--------------------------------------------------------------------------------------------------*/
		else
		{
			;												/* コマンド待ち							*/
		}
/*--------------------------------------------------------------------------------------------------*/
		return;
}

/****************************************************************************************************/
/*																									*/
/*		フルクエンコーダ：絶対位置データ(64bit)取得						<S02D>						*/
/*																									*/
/****************************************************************************************************/
static void FencSencGetAbsPos( MENCV  *FencV, BOOL RvsDir )
{
ULONG	PosL;
USHORT	PosH;
LONG	AbsPos[2];

/*--------------------------------------------------------------------------------------------------*/
/*		エンコーダデータラッチ																		*/
/*--------------------------------------------------------------------------------------------------*/
		PosH = FencV->RxPosH[0];						/* スキャンＡで読込んだデータを使用して、	*/
		PosL = (ULONG)FencV->RxPosL[0].dw;				/* 絶対位置(36Bit)を計算する。				*/

		AbsPos[1] = (LONG)((SHORT)PosH) >> FSX_FPOS_SFT;
		AbsPos[0] = (PosH<<(32-FSX_FPOS_SFT))+((ULONG)PosL>>FSX_FPOS_SFT);

		/*------------------------------------------------------------------------------------------*/
		if( RvsDir ^ FencV->RevAsmMotor )				/* 逆回転モード XOR Fenc逆回転モード		*/
		{
			AbsPos[1] = (~AbsPos[1]);					/* 符号反転処理								*/
			AbsPos[0] = (~AbsPos[0]) + 1;				/* 符号反転処理								*/
			if( AbsPos[0] == 0 ){ ++AbsPos[1];}			/* Carryの場合は、上位に+1					*/
		}
		FencV->SenOnScalePosH = AbsPos[1];
		FencV->SenOnScalePosL = AbsPos[0];
/*--------------------------------------------------------------------------------------------------*/
		return;
}


/****************************************************************************************************/
/*																									*/
/*		ＰＡＯ相マルチターンデータ出力								<S02D>							*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : マルチターンデータを指定桁で割った商をアスキー文字に変換して指定ポートへ出力し、		*/
/*			その桁を引いた残りを保存します。指定桁がデータの最上位である事が必要です。				*/
/*																									*/
/*			例）*data = 32767 の ５桁目(digit=5)を出力する場合										*/
/*																									*/
/*				出力データ = '3'																	*/
/*				保存データ = 2767																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static void FencSencOutputPaoMtdata( MENCV *FencV, ASIC_HW_IF_STR *pAsicHwReg, ULONG digit )
{
ULONG	x;
ULONG	outx;

/*--------------------------------------------------------------------------------------------------*/
/*		桁データ変換																				*/
/*--------------------------------------------------------------------------------------------------*/
		switch( digit )
		{
		case  1: digit = 1;     break;
		case  2: digit = 10;    break;
		case  3: digit = 100;   break;
		case  4: digit = 1000;  break;
		case  5: digit = 10000; break;
		}
/*--------------------------------------------------------------------------------------------------*/
/*		マルチターンデータ出力																		*/
/*--------------------------------------------------------------------------------------------------*/
		x = FencV->DivInfo.PaoSeqData / digit;
		outx = MlibEvenAsciiTbl[ x ];
		pAsicHwReg->AREG_UARTXD  = (USHORT)outx;
		FencV->DivInfo.PaoSeqData -= (USHORT)(x * digit);
/*--------------------------------------------------------------------------------------------------*/
		return;

}

/****************************************************************************************************/
/*																									*/
/*		フルクエンコーダ：ＰＡＯ相シリアル出力							<S02D>						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : アブソエンコーダ使用時、ＳＥＮ信号によるマルチターンデータ及びアラームの				*/
/*			ＰＡＯからのシリアル出力処理を行う。													*/
/*																									*/
/*			1) 出力フォーマット : ７bit 偶数パリティ												*/
/*				 ＡＳＩＣのＴＸＤは、パリティの付加及び７ビットデータ長送信の機能が					*/
/*				 ないので、パリティをソフト的に付加した８ビットデータとして送信する					*/
/*																									*/
/*			2) マルチターン : Ｐ±＠＠＠＠＠[CR]	( ±@@@@@ : -32768 ～ +32768 )					*/
/*																									*/
/*			3) アラーム		: ＡＬＭ＠＠．[CR]		( @@ : A.@@ )									*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	FencSencOutputPaoSerial( MENCV *FencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
							     BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag )
{
	USHORT	uswk;
	ASIC_HW_IF_STR *pAsicHwReg2;/*<S0C2>*/
	pAsicHwReg2 = (ASIC_HW_IF_STR*)((UCHAR*)pAsicHwReg + 0x1000);/*2-axis pointing address <S0C2>*/


/*--------------------------------------------------------------------------------------------------*/
/*		機能有無チェック																			*/
/*--------------------------------------------------------------------------------------------------*/
		if( SVFSEL_PAOPOUT == 0 )   { return;}				/* Mask Function						*/

/*--------------------------------------------------------------------------------------------------*/
/*		サーボオン時シーケンス初期化																*/
/*--------------------------------------------------------------------------------------------------*/
		if( DbBeReq == TRUE )
		{
			FencV->DivInfo.PaoSeqStep = 0;
			return;
		}

/*--------------------------------------------------------------------------------------------------*/
/*		ＰＡＯシリアル出力シーケンス																*/
/*--------------------------------------------------------------------------------------------------*/
		switch( FencV->DivInfo.PaoSeqStep )
		{
	/*----------------------------------------------------------------------------------------------*/
		case  0: /* Initial Step																	*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqWait = 0;
				FencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				if( FencV->PaoseqNormal == TRUE )				/* 電源投入後初回動作?				*/
				{
					FencV->DivInfo.PaoSeqStep = (SenReq == FALSE)? 1 : 20;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;				/* 電源投入後初回はSEN Lo->Hiを待つ	*/
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  1: /* Wait SEN Lo --> Hi (Output Multi-turn Data)										*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->PaoseqNormal = TRUE;						/* 初回動作モード解除				*/
				FencV->SenReady = FALSE;
				FencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
				FencV->DivInfo.IncPulseReq = TRUE;
				if( SenReq == TRUE )
				{
					/*------------------------------------------------------------------------------*/
					/*		ＤＡ出力モード：ＴＸＤ出力												*/
					/*------------------------------------------------------------------------------*/
					/*		Bit13 :	DASEL(分周出力DA端子選択)	:  0 : DA,  1: TXD_1				*/
					/*------------------------------------------------------------------------------*/
					pAsicHwReg->AREG_PFNCSET  |= BIT13;			/* ＤＡ出力モード：ＴＸＤ出力		*/
					pAsicHwReg2->AREG_PFNCSET &= ~BIT13;		/*PSO NO way <S0C2>	    */
					/*------------------------------------------------------------------------------*/
					pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;		/* パルス出力回路初期化				*/
					pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;		/* パルス出力回路初期化				*/

					pAsicHwReg->AREG_DINCSET  |= BIT10;			/* 分周常時クリアビット立てる		*/
					pAsicHwReg->AREG_WDT1SET |= BIT8;			/* パルス変換Ｃ相マスクＯＮ			*/
					FencV->DivInfo.DivCMaskStatus = DIV_CMASK_ON;/* パルス変換Ｃ相マスクＯＮ		*/

					FencV->DivInfo.PaoSeqStep = 2;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  2: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( ++FencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
				{
					pAsicHwReg->AREG_DINCSET |= BIT5;			/* Ｃ相再同期可						*/
					pAsicHwReg->AREG_DINCSET &= ~(BIT10);		/* 分周常時クリア落とす				*/
					pAsicHwReg->AREG_DINCSET &= ~(BIT5);		/* Ｃ相再同期可解除					*/
					FencV->DivInfo.PaoSeqWait = 0;
					FencV->DivInfo.PaoSeqStep = 3;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  3: /* Multiturn Latch Request															*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqMcmd = KPX_MTLATCH_REQUEST;/* MultiturnLatch指令 ==> ScanA	*/
				FencV->DivInfo.PaoSeqStep = 4;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  4: /* Wait Multiturn Latch															*/
	/*----------------------------------------------------------------------------------------------*/
				if( FencV->DivInfo.PaoSeqMcmd == KPX_MTLATCH_FINISH )	/* MultiturnLatch完了？		*/
				{
					FencV->DivInfo.PaoSeqData = FencV->DivInfo.MturnLatch;
					FencV->DivInfo.PaoSeqStep = 5;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  5: /* Output 'P'																		*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	UARTXD:汎用非同期ポート送信データ：汎用非同期ポート送信データビット７-0   	    */
				/*----------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_UARTXD  = 'P';
				FencV->DivInfo.PaoSeqStep = 6;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case  6: /* Output '+' or '-'																*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*	逆回転モード																	*/
				/*----------------------------------------------------------------------------------*/
				if( FencV->DivInfo.RvsDir )					/* 逆回転モード						*/
				{
					if( (SHORT)FencV->DivInfo.PaoSeqData >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = '-';
						FencV->DivInfo.PaoSeqData = FencV->DivInfo.PaoSeqData + 1;
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = '+';
						FencV->DivInfo.PaoSeqData = ~FencV->DivInfo.PaoSeqData;
					}
				}
				/*----------------------------------------------------------------------------------*/
				/*	正回転モード																	*/
				/*----------------------------------------------------------------------------------*/
				else
				{
					if( (SHORT)FencV->DivInfo.PaoSeqData >= 0 )
					{
						pAsicHwReg->AREG_UARTXD  = '+';
					}
					else
					{
						pAsicHwReg->AREG_UARTXD  = '-';
						FencV->DivInfo.PaoSeqData = ~FencV->DivInfo.PaoSeqData + 1;
					}
				}
				FencV->DivInfo.PaoSeqStep = 7;
				break;
	/*----------------------------------------------------------------------------------------------*/
	/*			１０進データ出力 : ５桁目,４桁目,３桁目,２桁目,１桁目								*/
	/*----------------------------------------------------------------------------------------------*/
		case  7: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 5 ); FencV->DivInfo.PaoSeqStep =  8; break;
		case  8: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 4 ); FencV->DivInfo.PaoSeqStep =  9; break;
		case  9: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 3 ); FencV->DivInfo.PaoSeqStep = 10; break;
		case 10: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 2 ); FencV->DivInfo.PaoSeqStep = 11; break;
		case 11: FencSencOutputPaoMtdata( FencV, pAsicHwReg, 1 ); FencV->DivInfo.PaoSeqStep = 12; break;
	/*----------------------------------------------------------------------------------------------*/
		case 12: /* Output '0x0D'																	*/
	/*----------------------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
				FencV->DivInfo.PaoSeqStep = 13;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 13: /* Wait 2ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqStep = 14;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 14: /* IncPulse Output Request															*/
	/*----------------------------------------------------------------------------------------------*/
				/*----------------------------------------------------------------------------------*/
				/*		ＤＡ出力モード：パルス出力										   		    */
				/*----------------------------------------------------------------------------------*/
				/*		Bit13 :	DASEL(分周出力DA端子選択)	:  0 : DA,  1: TXD_1				    */
				/*----------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* ＤＡ出力モード：パルス出力		*/
				/*----------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqMcmd = KPX_IPLSOUT_REQUEST;/* 初期インクレ出力指令 ==> ScanA	*/
				FencV->DivInfo.PaoSeqStep = 15;
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 15: /* Check IncPulse Output Complete													*/
	/*----------------------------------------------------------------------------------------------*/
				if( FencV->DivInfo.PaoSeqMcmd == KPX_IPLSOUT_FINISH )	/* 初期インクレ出力完了？	*/
				{
					FencV->SenReady = TRUE;
					FencSencGetAbsPos( FencV, Bprm->RvsDir );	/* 絶対位置データの取得	*/
					pAsicHwReg->AREG_WDT1SET &= ~BIT8;			/* ﾊﾟﾙｽ変換Ｃ相ﾏｽｸＯＦＦ  			*/
					FencV->DivInfo.DivCMaskStatus  = DIV_CMASK_OFF ;
					FencV->DivInfo.PaoSeqStep = 0;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 20: /* Wait SEN Hi --> Lo (Output Alarm)												*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					FencV->SenReady = FALSE;
					FencV->DivInfo.PaoSeqMcmd = KPX_PAOSEQ_NOCMD;
					FencV->DivInfo.IncPulseReq = TRUE;
					if( AlmFlag )								/* アラーム発生中					*/
					{
						pAsicHwReg->AREG_PFNCSET  |= BIT13;		/* ＤＡ出力モード：ＴＸＤ出力		*/
						pAsicHwReg->AREG_POSET0	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/
						pAsicHwReg->AREG_POTEST	= 0xFFFFFFFF;	/* パルス出力回路初期化				*/

						FencV->DivInfo.PaoSeqData = AlmCode;	/* アラームコード取得				*/
						FencV->DivInfo.PaoSeqStep = 21;			/* アラーム出力ステップに遷移		*/
					}
					else										/* アラーム発生なし					*/
					{
						FencV->DivInfo.PaoSeqStep =  1;			/* Lo-->Hi監視ステップに遷移		*/
					}
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 21: /* Wait 120ms																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					if( ++FencV->DivInfo.PaoSeqWait > KPI_SCANC_MS( 120 ) )
					{
						FencV->DivInfo.PaoSeqWait = 0;
						FencV->DivInfo.PaoSeqStep = 22;
					}
				}
				else
				{
					FencV->DivInfo.PaoSeqWait = 0;
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 22: /* Output 'A'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = 'A';
					FencV->DivInfo.PaoSeqStep = 23;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 23: /* Output 'L'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = ('L' + 0x80);
					FencV->DivInfo.PaoSeqStep = 24;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 24: /* Output 'M'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = 'M';
					FencV->DivInfo.PaoSeqStep = 25;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 25: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((FencV->DivInfo.PaoSeqData >> 8) & 0x0F)];
					pAsicHwReg->AREG_UARTXD  = uswk;
					FencV->DivInfo.PaoSeqStep = 26;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 26: /* Output Alarm Code																*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					uswk = MlibEvenAsciiTbl[((FencV->DivInfo.PaoSeqData>> 4) & 0x0F)];
					pAsicHwReg->AREG_UARTXD  = uswk;
					FencV->DivInfo.PaoSeqStep = 27;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 27: /* Output '.'																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = '.';
					FencV->DivInfo.PaoSeqStep = 28;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 28: /* Output 0x0D																		*/
	/*----------------------------------------------------------------------------------------------*/
				if( SenReq == FALSE )
				{
					pAsicHwReg->AREG_UARTXD  = (0x0d + 0x80);
					FencV->DivInfo.PaoSeqStep = 29;
				}
				else
				{
					FencV->DivInfo.PaoSeqStep = 1;
				}
				break;
	/*----------------------------------------------------------------------------------------------*/
		case 29: /* End of Output Alarm																*/
	/*----------------------------------------------------------------------------------------------*/
				pAsicHwReg->AREG_PFNCSET  &= ~BIT13;			/* ＤＡ出力モード：パルス出力		*/
				FencV->DivInfo.PaoSeqStep = 1;
				break;
	/*----------------------------------------------------------------------------------------------*/
		default: /* Default																			*/
	/*----------------------------------------------------------------------------------------------*/
				FencV->DivInfo.PaoSeqStep = 0;
				break;
		}
/*--------------------------------------------------------------------------------------------------*/
		return;

}
#endif /*<S02D>*/

/* <S1B7> >>>>> */
/*****************************************************************************
  Description : モータレス：エンコーダ変数初期化処理
----------------------------------------------------------------------------
  Parameters:
	MencV;					モータエンコーダデータへのポインタ
	AsicHwReg;				Asic H/W レジスタアドレス
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxSencMotLessInitVariables( MENCV *MencV, ASIC_HW_IF_STR *AsicHwReg )
{

	/*----------------------------------------------------------------------*/
	/*	初回位置データ(今回,前回,前々回)の設定								*/
	/*----------------------------------------------------------------------*/
	MencV->MotPos  = 0;				/* Motor Position	[32bitRing]			*/
	MencV->MotPhaX = 0;				/* Motor PhasePos.	[PulseNo/360deg]	*/
	MencV->MotCposX = 0;			/* Motor CphasePos.	[32bitRing]			*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosH[1]  = MencV->RxPosH[0] = 0;	/* 前回インクレ補正値初期設定	*/
	MencV->RcvPosX[0] = MencV->RcvPosX[1] = MencV->RcvPosX[2] = 0;
	MencV->MotPosX[0] = MencV->MotPosX[1] = 0;

	/*----------------------------------------------------------------------*/
	/*	初回絶対位置データ													*/
	/*----------------------------------------------------------------------*/
	MencV->MotAbsPos = 0;

	/*----------------------------------------------------------------------*/
	/*	初回マルチターンデータの設定										*/
	/*----------------------------------------------------------------------*/
	MencV->IniMturn = 0;
	MencV->IniIncre = 0;

	/*----------------------------------------------------------------------*/
	/*	位相原点の初期設定													*/
	/*----------------------------------------------------------------------*/
	MencV->MotOrgX  = 0;			/* モータ原点							*/
	MencV->MotCposX = 0;			/* モータＣ相位置						*/
	MencV->MotAngle = 0;			/* モータ回転角(パルス)					*/
	MencV->PhaseOffset = 0;			/* ポールセンサオフセット量 			*/

	/*----------------------------------------------------------------------*/
	/*	モータ原点チェック用変数の初期化									*/
	/*----------------------------------------------------------------------*/
	MencV->MotOrgChkEnd = TRUE;		/* Set Motor Origin Check End Flag		*/

	/*----------------------------------------------------------------------*/
	/*	分周パルス初期設定													*/
	/*----------------------------------------------------------------------*/
	MencV->DivInfo.DivOutRem = 0;
	MencV->DivInfo.DivOutPos = 0;
	MencV->DivInfo.InitDivPosOffset = 0;
	if( MencV->DivInfo.DivOut == TRUE )
	{
		AsicHwReg->AREG_POSET0 = (USHORT)MencV->DivInfo.DivOutPos;
		AsicHwReg->AREG_POTEST = (USHORT)MencV->DivInfo.DivOutPos;
	}

	/*----------------------------------------------------------------------*/
	/*	Ｃ相フラグ関連														*/
	/*----------------------------------------------------------------------*/
	MencV->Cset = TRUE;				/* Ｃ相設定Flag							*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		MencV->CphaseLess = TRUE;	/* Ｃ相レスFlag							*/
	}
//[org]	KPI_PCMASK_OFF( );					/* パルス変換C相マスクOFF設定	*/
	AsicHwReg->AREG_WDT1SET &= ~BIT8;		/* TODO: 必要か！！				*/
	MencV->DivInfo.DivCMaskStatus = DIV_CMASK_OFF;	/* パルス変換C相マスクOFF設定	*/

	/*----------------------------------------------------------------------*/
	/*	エンコーダレディ設定												*/
	/*----------------------------------------------------------------------*/
	MencV->PhaseReady = TRUE;		/* 位相検出完了Flag						*/

	return;
}


/*****************************************************************************
  Description : モータレス：回転型位置データ読込み
----------------------------------------------------------------------------
  Parameters:
	MencV;					モータエンコーダデータへのポインタ
	pAsicHwReg;				Asic H/W レジスタアドレス
	AlmManager;				アラーム管理ＩＦデータ定義へのポインタ
	BeComplete;				ベースイネーブル完了状態
	BaseLoops;				制御ループ演算用データへのポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void RmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops )
{
	BPRMDAT		*Bprm;					/* 基本パラメータ定義へのポインタ			*/
	MENCPRM		*MencP;					/* エンコーダパラメータ定義へのポインタ		*/
	MOTLESSTST	*MotorLessTest;			/* モータレステストモードIF定義へのポインタ	*/
	MOTOR_LESS	*MotorLessTestDatas;	/* エンコーダIF用データへのポインタ			*/
	SENCX		*Enc;					/* シリアルエンコーダIF定義へのポインタ		*/

	LONG		dEncPos;				/* エンコーダ位置差分				*/
	LONG		dMotPos;				/* 位置差分							*/
	LONG		RcvPosX;				/* エンコーダ今回位置				*/
	LONG		LastMotPosX;			/* 前回位置							*/
	SHORT		Mturn;					/* マルチターン						*/

	/* 変数初期化	*/
	Bprm				= ((BASE_LOOPCTRLS*)BaseLoops)->Bprm;
	MotorLessTest		= ((BASE_LOOPCTRLS*)BaseLoops)->MotorLessTest;
	MencP				= &(MotorLessTest->Prm->MencP);
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);
	Enc					= &(MencV->EncIf);

	LastMotPosX = MencV->MotPosX[0];

	/*----------------------------------------------------------------------*/
	/*	モータレステストモード用位置データ設定関数実行						*/
	/*----------------------------------------------------------------------*/
	ApiSetMechaDatasMotorLessTestMode( Bprm, MotorLessTest );

	/*----------------------------------------------------------------------*/
	/*	前回の位置データ退避												*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[1] = MencV->RxPosL[0];			/* 前回値更新			*/
	MencV->RxPosH[1] = MencV->RxPosH[0];			/* 前回値更新			*/

	/*----------------------------------------------------------------------*/
	/*  モータモデル部より位置差分データ取得								*/
	/*----------------------------------------------------------------------*/
	dEncPos = (LONG)MotorLessTestDatas->dSemiModePos << MencV->MposSftX;
	MlibAdd6432( MotorLessTestDatas->SencData.Long, dEncPos, MotorLessTestDatas->SencData.Long );

	/*----------------------------------------------------------------------*/
	/*  マルチターンリミット処理											*/
	/*----------------------------------------------------------------------*/
//	if( MencV->AxisMotType == MOTTYPE_ROTARY )
//	{
//	}

	if( MencP->limmlt != 65535 )
	{
		Mturn = LpxGetSencMotLessMturnData( MencV, MencP,
											MotorLessTestDatas->SencData.EncFormat.RxPosH,
											MotorLessTestDatas->SencData.EncFormat.RxPosL.dw );
		if( Mturn > MencP->limmlt )
		{
			/* マルチターンリミットを超えた場合 ⇒ マルチターンデータに0を設定	*/
			LpxSencMotLessMultiTurnSet( MencP, &MotorLessTestDatas->SencData, 0 );
		}
		else if( Mturn < 0 )
		{
			/* マルチターンが負の場合 ⇒ マルチターンデータにマルチターンリミットを設定		*/
			LpxSencMotLessMultiTurnSet( MencP, &MotorLessTestDatas->SencData, MencP->limmlt );
		}
	}

	/*----------------------------------------------------------------------*/
	/*  位置データをエンコーダ受信データにセット							*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[0].dw = MotorLessTestDatas->SencData.EncFormat.RxPosL.dw;
	MencV->RxPosH[0]    = MotorLessTestDatas->SencData.EncFormat.RxPosH;

	/*----------------------------------------------------------------------*/
	/*	位置演算															*/
	/*----------------------------------------------------------------------*/
	RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long>>MencV->MposSftX)<<MencV->MposSftR);

	/*----------------------------------------------------------------------*/
	/*	今回位置データ、前回位置データ更新									*/
	/*----------------------------------------------------------------------*/
	MencV->SeRxErr    = FALSE;				/* Reset Error Flag				*/
	MencV->EncDataNG  = FALSE;				/* Reset Encoder Data NG Flag	*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];	/* 前回位置データ				*/
	MencV->RcvPosX[0] = RcvPosX;			/* 今回位置データ				*/

	/*----------------------------------------------------------------------*/
	/*	モータ位置出力演算													*/
	/*----------------------------------------------------------------------*/
	MencV->MotPosX[1] = MencV->RcvPosX[1];	/* 前回位置データ				*/
	MencV->MotPosX[0] = MencV->RcvPosX[0];	/* 今回位置データ				*/

	/*----------------------------------------------------------------------*/
	/*	モータ位置出力(32bitRingPulse)										*/
	/*----------------------------------------------------------------------*/
	dMotPos = ( (MencV->MotPosX[0] - LastMotPosX) >> MencV->MposSftR );
	MencV->MotPos    += dMotPos;
	MencV->MotAbsPos += dMotPos;

	// 下から移動
	/*----------------------------------------------------------------------*/
	/*	モータ電流位相更新													*/
	/*----------------------------------------------------------------------*/
	MencV->MotPhase = (USHORT)(( (((MencV->MotPosX[0]) - (MencV->MotOrgX)) >>8) * MencV->Kmotphase)>>8);

	/*----------------------------------------------------------------------*/
	/*	フィードバックパルス出力処理										*/
	/*----------------------------------------------------------------------*/
	//	この処理がMercuryには存在しない
	/* <S20D> Start */
	if( MencV->DivInfo.IncPulseReq == TRUE )	/* 初期インクレ出力要求？	*/
	{
		if( MencV->AxisMotType == MOTTYPE_LINEAR )
		{ /* リニア型 */
			/* 初期インクレパルス出力処理 */
			LmxOutSencIniIncPulse(MencV, pAsicHwReg, LMX_MPOS_SFT);
		}
		else
		{ /* 回転型 */
			/* 初期インクレパルス出力処理 */
			SencOutIniIncPulse( MencV, pAsicHwReg );
		}
	}

	if((MencV->DivInfo.DivOutASIC != FALSE) && (MencV->DivInfo.IncPulseReq == FALSE))
	{ /* JL-086にて分周の場合、初期インクレパルス出力完了後JL-086にて分周する */
		Enc->SencSetReg->AREG_DIVSET &= ~BIT1;
	}
	/* <S20D> End */

	/*----------------------------------------------------------------------*/
	/*	原点通過チェック処理												*/
	/*----------------------------------------------------------------------*/
	if( (MencV->OrgOk == FALSE) && (MencV->SeRxErr == FALSE) )
	{
		RmxCheckSencOriginPass( MencV );		/* 原点通過チェック	*/
	}

//	/*----------------------------------------------------------------------*/
//	/*	モータ電流位相更新													*/
//	/*----------------------------------------------------------------------*/
//	// 上に移動

	/*----------------------------------------------------------------------*/
	/*	分周パルス出力処理													*/
	/*----------------------------------------------------------------------*/
	// 初期インクレ出力なし
	if( MencV->DivInfo.DivOut == TRUE )
	{
		SHalDrv_SetDivOutPosition( Enc->SencSetReg, (MencV->MotPosX[0] >> MencV->DivInfo.DivOutSft) );
	}
	/*----------------------------------------------------------------------*/
	return;

}


/*****************************************************************************
  Description : モータレス：リニア位置データ読込み
----------------------------------------------------------------------------
  Parameters:
	MencV;					モータエンコーダデータへのポインタ
	pAsicHwReg;				Asic H/W レジスタアドレス
	AlmManager;				アラーム管理ＩＦデータ定義へのポインタ
	BeComplete;				ベースイネーブル完了状態
	BaseLoops;				制御ループ演算用データへのポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
	暫定版(作成中)
*****************************************************************************/
void LmxReadMotorLessPosition( MENCV *MencV, ASIC_HW_IF_STR *pAsicHwReg,
									ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops )
{
	BPRMDAT		*Bprm;					/* 基本パラメータ定義へのポインタ			*/
	MOTLESSTST	*MotorLessTest;			/* モータレステストモードIF定義へのポインタ	*/
	MOTOR_LESS	*MotorLessTestDatas;	/* エンコーダIF用データへのポインタ			*/
	SENCX		*Enc;					/* シリアルエンコーダIF定義へのポインタ		*/

	LONG		dEncPos;				/* エンコーダ位置差分				*/
	LONG		dMotPos;				/* 位置差分							*/
	LONG		RcvPosX;				/* エンコーダ今回位置				*/
	LONG		LastMotPosX;			/* 前回位置							*/

	/* 変数初期化	*/
	Bprm				= ((BASE_LOOPCTRLS*)BaseLoops)->Bprm;
	MotorLessTest		= ((BASE_LOOPCTRLS*)BaseLoops)->MotorLessTest;
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);
	Enc					= &(MencV->EncIf);

	LastMotPosX = MencV->MotPosX[0];

	/*----------------------------------------------------------------------*/
	/*	モータレステストモード用位置データ設定関数実行						*/
	/*----------------------------------------------------------------------*/
	ApiSetMechaDatasMotorLessTestMode( Bprm, MotorLessTest );

	/*----------------------------------------------------------------------*/
	/*	前回の位置データ退避												*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[1] = MencV->RxPosL[0];			/* 前回値更新			*/
	MencV->RxPosH[1] = MencV->RxPosH[0];			/* 前回値更新			*/

	/*----------------------------------------------------------------------*/
	/*  モータモデル部より位置差分データ取得								*/
	/*----------------------------------------------------------------------*/
//TODO:(Sigma-V)	dEncPos = (LONG)MotorLessTestDatas->dSemiModePos << MencV->MposSftLm;
	dEncPos = (LONG)MotorLessTestDatas->dSemiModePos << MencV->MposSftX;
	MlibAdd6432( MotorLessTestDatas->SencData.Long, dEncPos, MotorLessTestDatas->SencData.Long );

	/*----------------------------------------------------------------------*/
	/*  位置データをエンコーダ受信データにセット							*/
	/*----------------------------------------------------------------------*/
	MencV->RxPosL[0].dw = MotorLessTestDatas->SencData.EncFormat.RxPosL.dw;
	MencV->RxPosH[0]    = MotorLessTestDatas->SencData.EncFormat.RxPosH;

	/*----------------------------------------------------------------------*/
	/*	位置演算															*/
	/*----------------------------------------------------------------------*/
//TODO:(Sigma-V)	RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long>>MencV->MposSftLm)<<MencV->MposSftLm);
	RcvPosX = MencV->MposSign * ((MencV->RxPosL[0].Long>>MencV->MposSftX)<<MencV->MposSftR);

	/*----------------------------------------------------------------------*/
	/*	今回位置データ、前回位置データ更新									*/
	/*----------------------------------------------------------------------*/
	MencV->SeRxErr    = FALSE;				/* Reset Error Flag				*/
	MencV->EncDataNG  = FALSE;				/* Reset Encoder Data NG Flag	*/
	MencV->RcvPosX[1] = MencV->RcvPosX[0];	/* 前回位置データ				*/
	MencV->RcvPosX[0] = RcvPosX;			/* 今回位置データ				*/

	/*----------------------------------------------------------------------*/
	/*	モータ位置出力演算													*/
	/*----------------------------------------------------------------------*/
	MencV->MotPosX[1] = MencV->RcvPosX[1];	/* 回転形と処理をあわせるため	*/
	MencV->MotPosX[0] = MencV->RcvPosX[0];	/* 回転形と処理をあわせるため	*/
	/*----------------------------------------------------------------------*/
//TODO:(Sigma-V)	dMotPos = (MencV.MotPosX[0] - LastMotPosX) >> MencV->MposSftLm;
	dMotPos = ( (MencV->MotPosX[0] - LastMotPosX) >> MencV->MposSftR );

	MencV->MotPos    += dMotPos;	/* Motor Position (32bitRingPulse)		*/
	MencV->MotAbsPos += dMotPos;	/* Motor Abs Position (32bitRingPulse)	*/

	/*----------------------------------------------------------------------*/
	/*	モータ電流位相更新													*/
	/*----------------------------------------------------------------------*/
	LmxUpdateMotPhase( MencV, AlmManager, dMotPos );

	/*----------------------------------------------------------------------*/
	/*	初期インクレパルス出力処理 or エンコーダＣ相通過チェック処理		*/
	/*----------------------------------------------------------------------*/
	if( MencV->DivInfo.IncPulseReq == TRUE )		/* 初期インクレ出力要求？		*/
	{
//TODO:(Sigma-V)		LmxOutSencIniIncPulse( MencV->MposSftLm );	/* 初期インクレパルス出力処理	*/
	}
	else
	{
		if( MencV->SeRxErr == FALSE )
		{
			LmxCheckSencCphasePass( MencV );	/* エンコーダＣ相通過チェック処理	*/
		}
		/*------------------------------------------------------------------*/
		/*	分周パルス出力													*/
		/*------------------------------------------------------------------*/
		if( MencV->DivInfo.DivOut == TRUE )
		{
			MencV->DivInfo.DivOutPos += MlibPfbkxremNolim( dMotPos, MencV->DivInfo.DivOutGain, &MencV->DivInfo.DivOutRem );
			SHalDrv_SetDivOutPosition( Enc->SencSetReg, MencV->DivInfo.DivOutPos );
		}
	}
	/*----------------------------------------------------------------------*/
	return;
}


/*****************************************************************************
  Description : モータレス：エンコーダパラメータ設定処理
	エンコーダ未接続時、仮想モータパラメータ設定後のエンコーダ情報を設定する
----------------------------------------------------------------------------
  Parameters:
	MencV;					モータエンコーダデータへのポインタ
	MencP;					エンコーダパラメータ定義へのポインタ
	b_prm2;					機能選択応用スイッチ２
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxSetSencInfoMotorLessTestMode( MENCV *MencV, MENCPRM *MencP, USHORT b_prm2 )
{

	/*----------------------------------------------------------------------*/
	/*	エンコーダ未接続時エンコーダタイプ選択								*/
	/*----------------------------------------------------------------------*/
	if( MencP->typm_e.b.h & 0x10 )
	{
		MencV->AbsoEncoder = TRUE;
//TODO:		SencSetAbsoEncType(MencV, b_prm2, MencP->typm_e.w);
// 上記関数を使用したいが、MotorPrmCfg.cに、static宣言されていて利用できないため、展開して使用
		if( 1 == ((b_prm2>>8) & 0x0F) )
		{	/* アブソをインクレとして使用	*/
			MencV->AbsoEncType = MENC_INCRE_TYPE;
		}
		else
		{	/* アブソをアブソとして使用		*/
			MencV->AbsoEncType = (MencP->typm_e.w & 0x1000)? MENC_MABSO_TYPE : MENC_SABSO_TYPE;
		}
	}
	else
	{
		MencV->AbsoEncoder = FALSE;
		MencV->AbsoEncType = MENC_INCRE_TYPE;
	}
	/*----------------------------------------------------------------------*/
	/*	シリアルコンバータスケールピッチ情報無効設定						*/
	/*----------------------------------------------------------------------*/
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		MencV->ScalePitchExp = 0xFF;
	}

		return;
}


/*****************************************************************************
  Description : モータレス：マルチターンデータ取得処理
	エンコーダデータから小数点位置を利用してマルチターンデータ(16bit)を取り出す。
----------------------------------------------------------------------------
  Parameters:
	MencV;					モータエンコーダデータへのポインタ
	MencP;					エンコーダパラメータ定義へのポインタ
	PosH;					位置データ(H)
	PosL;					位置データ(L)
  Return:
	マルチターンデータ(16bit)
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
static USHORT LpxGetSencMotLessMturnData( MENCV *MencV, MENCPRM *MencP, USHORT PosH, ULONG PosL )
{
	USHORT	Mturn;						/* マルチターンデータ	*/

	Mturn = (USHORT)(((PosH<<16) | ((ULONG)PosL>>16)) >> (MencP->bit_dp.b.h - 64));
	if( MencV->RevAsmMotor )
	{
		if( MencP->limmlt == 65535 )
		{
			Mturn = -Mturn;
		}
		else if( Mturn != 0 )
		{
			Mturn = MencP->limmlt - Mturn;
		}
	}

	return( Mturn );

}


/*****************************************************************************
  Description : モータレス：マルチターンデータ設定処理
	マルチターンリミットを考慮して、エンコーダデータにマルチターンデータを設定する
----------------------------------------------------------------------------
  Parameters:
	MencP;					エンコーダパラメータ定義へのポインタ
	EncData;				エンコーダデータへのポインタ
	MturnLimit;				マルチターンデータ値
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
static void LpxSencMotLessMultiTurnSet(MENCPRM *MencP, ENCODER_DATA *EncData, SHORT MturnLimit)
{
	DWORDX	MturnData;					/* マルチターンデータ	*/
	UCHAR	Dpoint;						/* 位置データ小数点位置	*/
	UCHAR	Sftx;						/* シフト数				*/

	/* 変数初期化 */
	Dpoint = MencP->bit_dp.b.h;
	Sftx   = 80 - Dpoint;

	/*----------------------------------------------------------------------*/
	/*	マルチターンデータ計算												*/
	/*----------------------------------------------------------------------*/
	MturnData.dw = MturnLimit << (Dpoint-64);

	/*----------------------------------------------------------------------*/
	/*	マルチターン領域クリア												*/
	/*----------------------------------------------------------------------*/
	EncData->EncFormat.RxPosH = 0;
	EncData->EncFormat.RxPosL.Ulong = ( EncData->EncFormat.RxPosL.Ulong << Sftx) >> Sftx;

	/*----------------------------------------------------------------------*/
	/*	マルチターン領域データセット										*/
	/*----------------------------------------------------------------------*/
	EncData->EncFormat.RxPosH     |= MturnData.w.h;
	EncData->EncFormat.RxPosL.w.h |= MturnData.w.l;

	return;
}


/*****************************************************************************
  Description : モータレス：エンコーダ変数初期化処理
----------------------------------------------------------------------------
  Parameters:
	FencV;					フルクエンコーダデータへのポインタ
	AsicHwReg;				Asic H/W レジスタアドレス
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxFencMotLessInitVariables( MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg )
{

	/*----------------------------------------------------------------------*/
	/*	初回位置データ(今回,前回)の設定										*/
	/*----------------------------------------------------------------------*/
	FencV->MotPos = 0;						/* フルク位置 [32bitRingPulse]	*/
	/*----------------------------------------------------------------------*/
	FencV->RxPosH[1] = FencV->RxPosH[0] = 0;
	FencV->MotPosX[0] = 0;
	FencV->MotPosX[1] = 0;

	/*----------------------------------------------------------------------*/
	/*	初回絶対位置データ													*/
	/*----------------------------------------------------------------------*/
	FencV->MotAbsPos = 0;

	/*----------------------------------------------------------------------*/
	/*	分周パルス初期設定													*/
	/*----------------------------------------------------------------------*/
	FencV->DivInfo.DivOutRem = 0;
	FencV->DivInfo.DivOutPos = 0;

	/*----------------------------------------------------------------------*/
	/*	Ｃ相フラグ関連														*/
	/*----------------------------------------------------------------------*/
	FencV->Cset = TRUE;						/* Ｃ相設定Flag					*/
	FencV->CposOk = TRUE;					/* Ｃ相位置設定完了Flag			*/
	FencV->CphaseLess = TRUE;
	/*----------------------------------------------------------------------*/
	if( FencV->DivInfo.DivOut == TRUE )
	{
		AsicHwReg->AREG_POSET0 = (USHORT)FencV->DivInfo.DivOutPos;
		AsicHwReg->AREG_POTEST = (USHORT)FencV->DivInfo.DivOutPos;
	}

	/*----------------------------------------------------------------------*/
	/*	エンコーダレディ設定												*/
	/*----------------------------------------------------------------------*/
	//処理なし
	/*----------------------------------------------------------------------*/

	/*----------------------------------------------------------------------*/
	/*	機能有無選択：インクレパルス出力要求リセット						*/
	/*----------------------------------------------------------------------*/
	//処理なし
	/*----------------------------------------------------------------------*/

	return;
}


/*****************************************************************************
  Description : モータレス：エンコーダパラメータ設定処理
	エンコーダ未接続時、エンコーダ情報を設定する
----------------------------------------------------------------------------
  Parameters:
	FencV;					フルクエンコーダデータへのポインタ
	MencP;					エンコーダパラメータ定義へのポインタ
  Return:
	none
----------------------------------------------------------------------------
  Note:
*****************************************************************************/
void LpxSetFencInfoMotorLessTestMode( MENCV *FencV, MENCPRM *MencP )
{
	/*----------------------------------------------------------------------*/
	/*	エンコーダビット数、小数点位置										*/
	/*----------------------------------------------------------------------*/
	//処理なし
	/*----------------------------------------------------------------------*/
	/*	エンコーダ未接続時エンコーダタイプ選択								*/
	/*----------------------------------------------------------------------*/
	FencV->AbsoEncoder = FALSE;

	/*----------------------------------------------------------------------*/
	/*	絶対値エンコーダ使用方法の設定										*/
	/*----------------------------------------------------------------------*/
	FencV->AbsoEncType = MENC_INCRE_TYPE;

	/*----------------------------------------------------------------------*/
	/*	ＥｎＤａｔタイプの選択												*/
	/*----------------------------------------------------------------------*/
	//FencP.f.EndatType = FALSE;

	/*----------------------------------------------------------------------*/
	/*	エンコーダタイプ／分解能設定										*/
	/*----------------------------------------------------------------------*/
	FencV->EncType = (0x0000 | MencP->BitnoDpoint.b.l);

	return;
}


/*****************************************************************************
  Description : モータレス：位置データ読込み
----------------------------------------------------------------------------
  Parameters:
	FencV;					フルクエンコーダデータへのポインタ
	AsicHwReg;				Asic H/W レジスタアドレス
	Axis;					軸ハンドル
  Return:
	位置データ
----------------------------------------------------------------------------
  Note:
	暫定版(作成中)
*****************************************************************************/
LONG LpxReadMotorLessFencPosition( MENCV *FencV, ALMMAN_HANDLE *AlmManager, BOOL BeComplete, void *BaseLoops )
{
	MOTLESSTST	*MotorLessTest;
	MOTOR_LESS	*MotorLessTestDatas;	/* エンコーダIF用データへのポインタ	*/

	LONG		dEncPos;				/* エンコーダ位置差分				*/
	LONG		dFencPos;				/* 位置差分							*/
	LONG		FencSpd; 				/* 外部PG速度						*/

	ASIC_HW_IF_STR *sencSetReg;

	/* 変数初期化	*/
	MotorLessTest		= ((BASE_LOOPCTRLS*)BaseLoops)->MotorLessTest;
	MotorLessTestDatas	= &(MotorLessTest->MotorLessTestDatas);
	sencSetReg			= FencV->EncIf.SencSetReg;

	/*----------------------------------------------------------------------*/
	/*	パラメータリセット(再計算)中										*/
	/*----------------------------------------------------------------------*/
	/* dFencPos = 0 とする 		*/
	//if( FnCmnCtrl->KnlMode == KPI_KMD_PARAMRESET ){ return( 0 );}
	// KPI_KMD_PARAMRESETは設定されない
	// FnCmnCtrl *FnCmnCtrl : AxisRsc->FnCmnCtrl

	/*--------------------------------------------------------------------------*/
	/*	受信完Flag(IntSync Set --> ScanB Clear)／受信エラーカウンタのリセット	*/
	/*--------------------------------------------------------------------------*/
	FencV->MstErrCnt = 0;						/* Reset Master Error Counter	*/
	FencV->SeRxErr = FALSE;						/* Reset Error Flag				*/
	FencV->EncDataNG = FALSE;					/* Reset Encoder Data NG Flag	*/
	/*--------------------------------------------------------------------------*/
	/*	前回の位置データ退避													*/
	/*--------------------------------------------------------------------------*/
	FencV->RxPosH[1] = FencV->RxPosH[0];		/* 前回値更新					*/

	/*--------------------------------------------------------------------------*/
	/*	モータモデル部より位置差分データ取得									*/
	/*--------------------------------------------------------------------------*/
	dEncPos = (LONG)MotorLessTestDatas->dFullModePos << FencV->MposSftR;
	MlibAdd6432( MotorLessTestDatas->FencData.Long, dEncPos, MotorLessTestDatas->FencData.Long );

	/*--------------------------------------------------------------------------*/
	/*	位置データをエンコーダ受信データにセット								*/
	/*--------------------------------------------------------------------------*/
	FencV->RxPosL[0].dw	= MotorLessTestDatas->FencData.EncFormat.RxPosL.dw;
	FencV->RxPosH[0]	= MotorLessTestDatas->FencData.EncFormat.RxPosH;

	/*--------------------------------------------------------------------------*/
	/*	位置演算																*/
	/*--------------------------------------------------------------------------*/
	FencV->MotPosX[1] = FencV->MotPosX[0];		/* 前回値更新					*/
	FencV->MotPosX[0] = FencV->MposSign * ((FencV->RxPosL[0].Long >> FencV->MposSftR) << FencV->MposSftR);

	/*--------------------------------------------------------------------------*/
	/*	位置差分演算															*/
	/*--------------------------------------------------------------------------*/
	dFencPos = (FencV->MotPosX[0] - FencV->MotPosX[1]) >> FencV->MposSftR;
	FencV->ABphCntr = (UCHAR)(FencV->MotPosX[0]) >> FencV->MposSftR;

	/*--------------------------------------------------------------------------*/
	/*	外部PG速度演算															*/
	/*--------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FencSpd = dFencPos * FencV->KConvLoadSpd;
#else
	FencSpd = MlibMulgain(dFencPos, FencV->KConvLoadSpd);
#endif /* FLOAT_USE */
#if 0
	FencV->MotSpd = LpxTimeBasesFencSpdCalc(dFencPos, FencSpd, FencV->SpdCmpEnable);
	FencV->SpdCmpEnable = TRUE;
#else
	FencV->MotSpd = FencSpd;
#endif

	/*--------------------------------------------------------------------------*/
	/*	位置演算 [32bitRingPulse]												*/
	/*--------------------------------------------------------------------------*/
	FencV->MotPos += dFencPos;
	FencV->MotAbsPos += dFencPos;

	/*--------------------------------------------------------------------------*/
	/*	初期インクレパルス出力処理 or Ｃ相通過チェック処理						*/
	/*--------------------------------------------------------------------------*/
	if( FencV->DivInfo.IncPulseReq != FALSE )	/* 初期インクレ出力要求？	*/
	{
		/*----------------------------------------------------------------------*/
		/* 電源投入時の初期化中にモータが移動していた場合の分周パルス出力対策   */
		/*	ScanA処理の1回目は、カウンタプリセットとパルス変換位置をセットする。*/
		/*----------------------------------------------------------------------*/
		if(FencV->AbsoEncType == MENC_INCRE_TYPE)
		{
			/* Ｃ相通過チェック処理 */
			if(FencV->SeRxErr == FALSE)
			{
				FencCheckCphasePass(FencV, sencSetReg);
			}
			/*	分周パルス出力処理	(直前にカウンタプリセットを設定)*/
			if(FencV->DivInfo.DivOut == TRUE)
			{
				FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
				sencSetReg->AREG_POTEST = (USHORT)FencV->DivInfo.DivOutPos;
				sencSetReg->AREG_POSET0 = (USHORT)FencV->DivInfo.DivOutPos;
			}
			FencV->DivInfo.IncPulseReq = FALSE;
		}
		else
		{
			/*----------------------------------------------------------------------*/
			/*	初期インクレパルス出力処理	  									    */
			/*----------------------------------------------------------------------*/
			FencSencOutIniIncPulse( FencV, sencSetReg );
		}
	}
	else
	{
		/* Ｃ相通過チェック処理	*/
		if( FencV->SeRxErr == FALSE )
		{
			FencCheckCphasePass(FencV, sencSetReg);
		}
		/* 分周パルス出力 */
		if( (FencV->DivInfo.DivOut != FALSE) && (FencV->DivInfo.IncPulseReq == FALSE) )
		{
			FencV->DivInfo.DivOutPos += MlibPfbkxremNolim(dFencPos, FencV->DivInfo.DivOutGain, &FencV->DivInfo.DivOutRem);
			sencSetReg->AREG_POSET0 = (USHORT) FencV->DivInfo.DivOutPos;
		}
	}

	return( dFencPos );
}

void	MlibAdd6432( LONG *a, LONG b, LONG *x )
{
	INT64	*src;
	INT64	*dst;

	src = (INT64*)a;
	dst = (INT64*)x;
	*dst = *src + b;

	return;
}
/* <<<<< <S1B7> */

/***************************************** end of file **********************************************/
