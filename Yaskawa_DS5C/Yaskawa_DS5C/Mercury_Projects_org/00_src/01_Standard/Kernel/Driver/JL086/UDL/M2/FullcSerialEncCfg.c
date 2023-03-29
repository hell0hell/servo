/****************************************************************************
Description	: Serial Encoder Command Interface
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




/****************************************************************************
*
*	フルクエンコーダ : タイプコード設定
*	エンコーダＩＤ情報より、インクレ／Ｅｎｄａｔの判断を行う。
*
****************************************************************************/
static BOOL FencSetEncoderType(MENCV *FencV, DBYTEX BitnoDpoint)
{
	FencV->SerialConvType = SCONVTYPE_NONE;
//	FencV->EndatType = FALSE;
	FencV->OrgClrByWrAdrCmd = FALSE;
	FencV->AbsoEncoder = FALSE;

	if((FencV->EncInfo.Model[2] == 'D') && (FencV->EncInfo.Model[3] == 'P'))
	{
		switch(FencV->TypeCode)
		{
		case 'E': /* ＥｎＤａｔタイプ	*/
			FencV->SerialConvType = SCONVTYPE_ENDAT_ABS;
//			FencV->EndatType = TRUE;
			FencV->AbsoEncoder = TRUE;
			break;
		case 'A': /* ミツトヨアブソタイプ	*/
			FencV->SerialConvType = SCONVTYPE_MITUTOYO_ABS;
			FencV->AbsoEncoder = TRUE;
			break;
		case 'D': /* ＳＭＳインクレタイプ */
			FencV->SerialConvType = SCONVTYPE_SONY_INC;
			break;
		case 'L': /* 標準インクレタイプ */
			FencV->SerialConvType = SCONVTYPE_STANDARD_INC;
			FencV->OrgClrByWrAdrCmd = TRUE;
			break;
		default:
			break;
		}
	}
	else
	{
		return TRUE; /* A.CF1 : ＦＰＧ通信異常 */
	}

	/*----------------------------------------------------------------------*/
	/*	絶対値エンコーダ使用方法の設定										*/
	/*----------------------------------------------------------------------*/
	if(FencV->AbsoEncoder != FALSE)
	{
		FencV->AbsoEncType = MENC_MABSO_TYPE;
		FencV->EncType = (0x0100 | BitnoDpoint.b.l);
	}
	else
	{
		FencV->AbsoEncType = MENC_INCRE_TYPE;
		FencV->EncType = (0x0000 | BitnoDpoint.b.l);
	}

	return FALSE;
}


/****************************************************************************
*
*	フルクエンコーダ : パラメータの読込み
*
****************************************************************************/
static SENC_ERROR FencReadParamAll(MENCV *FencV, MENCPRM *MencP)
{
	UCHAR	PrmBuf[8];

	/*----------------------------------------------------------------------*/
	/*	エンコーダＩＤ読み込み処理											*/
	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x00, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}
	SencSetIDINFO(0x00, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x08, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}
	SencSetIDINFO(0x08, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x10, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}
	SencSetIDINFO(0x10, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x18, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}
	SencSetIDINFO(0x18, PrmBuf, &FencV->EncInfo);

	/*----------------------------------------------------------------------*/
	/*	リニアタイプコード													*/
	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x40, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}
	FencV->TypeCode = (LONG)(PrmBuf[0]);

	/*----------------------------------------------------------------------*/
	/*	ビット数,小数点位置													*/
	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x48, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}
	MencP->BitnoDpoint.b.l = PrmBuf[2];
	MencP->BitnoDpoint.b.h = PrmBuf[3];

	/*----------------------------------------------------------------------*/
	/*	エンコーダソフトウェアバージョン									*/
	/*----------------------------------------------------------------------*/
	if(FALSE == SencReadParam8byte(&FencV->EncIf, 0x58, PrmBuf))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}

	/*	ＩＤ情報への終端コード挿入	*/
	SencFillNullIDINFO(&FencV->EncInfo);

	/*----------------------------------------------------------------------*/
	/*	エンコーダタイプの設定												*/
	/*----------------------------------------------------------------------*/
	if(FALSE != FencSetEncoderType(FencV, MencP->BitnoDpoint))
	{
		return (SENCERR_PGCOM);		/* A.CF1 : ＦＰＧ通信異常	*/
	}

	return (SENCERR_SUCCESS);
}

/****************************************************************************
*
*	フルクエンコーダ基本パラメータ計算
*	サーボ基本パラメータ計算(BprmCalc.c)に必要なエンコーダパラメータの計算を行う
*
****************************************************************************/
void FencBasePrmCalc(MENCV *FencV, MENCPRM *MencP, USHORT b_prm2, ULONG fcplsml)
{
	/*----------------------------------------------------------------------*/
	/*	運転方向の設定														*/
	/*----------------------------------------------------------------------*/
	switch((b_prm2 >> 12) & 0x0F)
	{
	case 0: /* 使用しない													*/
		FencV->RevAsmMotor = FALSE;
		break;
	case 1: /* 標準運転方向で使用する										*/
	case 2: /* Ａ,Ｂ,Ｃ相断線ﾏｽｸ無し 										*/
		FencV->RevAsmMotor = FALSE;
		break;
	case 3: /* 逆転運転方向で使用する										*/
	case 4: /* Ａ,Ｂ,Ｃ相断線ﾏｽｸ無し										*/
		FencV->RevAsmMotor = TRUE;
		break;
	default:/* パラメータ設定異常											*/
		FencV->RevAsmMotor = FALSE;
		break;
	}

	/*----------------------------------------------------------------------*/
	/*	分周パルス出力選択Flagの設定										*/
	/*----------------------------------------------------------------------*/
	FencV->DivOut = FALSE;

	/*----------------------------------------------------------------------*/
	/*	モータ１回転換算パルス数の設定 : [pulse/MotRev]						*/
	/*----------------------------------------------------------------------*/
	FencV->PulseNo = fcplsml << MencP->BitnoDpoint.b.l;	/* MotPuseNoから置き換え*/
	FencV->HalfPulseNo = FencV->PulseNo >> 1;

}

/****************************************************************************
*
*	フルクエンコーダの初期化・接続処理
*
****************************************************************************/
SENC_ERROR FencInitEncoder(MENCV *FencV, MENCPRM *MencP, USHORT b_prm2, ULONG fcplsml)
{
	SENC_ERROR err;

	err = SencConnect(FencV);
	if(SENCERR_SUCCESS != err)
	{
		return (err);
	}

	/*	Read Encoder parameters	*/
	err = FencReadParamAll(FencV, MencP);
	if(SENCERR_SUCCESS != err)
	{	/* try again... */
		err = FencReadParamAll(FencV, MencP);
		if(SENCERR_SUCCESS != err)
		{
			return (err);
		}
	}

	/*	Send the Reset command */
	err = SencSendRstOrgCmd(FencV);

	/*	Calculate the base parameters for Fully closed encoder */
	FencBasePrmCalc(FencV, MencP, b_prm2, fcplsml);

	/* 終了処理(P1モード設定) */
	SencChangeP1Mode(FencV);

	return err;
}



/****************************************************************************
*
*	フルクエンコーダ : 初期パラメータ計算
*	サーボ基本パラメータ計算(BprmCalc.c)後、パラメータ計算を行う
*
****************************************************************************/
static void FencInitPrmCalc(MENCV *FencV, MENCPRM *MencP,
                            USHORT scnvorgoffset, BOOL OrgOffsetEnable, LONG KConvLoadSpd)
{
	UCHAR 	BitNo;
	USHORT 	uswk;

	BitNo = MencP->BitnoDpoint.b.l;
	/*----------------------------------------------------------------------*/
	/*	位置演算用パラメータの計算											*/
	/*----------------------------------------------------------------------*/
	FencV->MposSign = (FencV->RevAsmMotor != FALSE) ? -1 : 1;
	FencV->KConvLoadSpd = KConvLoadSpd;

	/*----------------------------------------------------------------------*/
	/*	Ｃ相レスフラグ設定													*/
	/*----------------------------------------------------------------------*/
	FencV->CphaseLess = (FencV->AbsoEncoder) ? TRUE : FALSE;
	/*----------------------------------------------------------------------*/
	/*	位置データ計算用シフトデータ										*/
	/*----------------------------------------------------------------------*/
	/*	モータレステストモードの場合、BitNo=8, Dpoint=68と固定され、
	 * 	シフト量12(=FSX_FPOS_SFT)となるため、条件に含めない
	 *----------------------------------------------------------------------*/
//	if(FbkOptP.OpInfo.IfType == FBKOP_SCNV_MODULE)
//	{
//		FencV->MposSftR = 32 - BitNo;
//		FencV->MposSftX = FencV->MposSftR - (80 - MencP->BitnoDpoint.b.h);
//	}
//	else
	{
		FencV->MposSftR = FSX_FPOS_SFT;
//		FencV->MposSftR = 32 - BitNo;
//		FencV->MposSftX = FSX_FPOS_SFT;
	}

	/*----------------------------------------------------------------------*/
	/*	PnE8C   : シリアル変換ユニット原点オフセット						*/
	/*																		*/
	/*	OrgDetDelayComp = PnE8C [0.01deg]×2^BITNO / 100 / 360[deg]			*/
	/*----------------------------------------------------------------------*/
	/*		※C相位置計算で使用するため、LMX_MPOS_SFTだけシフトしておく		*/
	/*----------------------------------------------------------------------*/
	if(FALSE != OrgOffsetEnable)
	{
//		if((FencV->SenSpec & 0xC4) == 0x84)
//		{	/* Bit7=1,Bit6=0 かつ Bit2(原点オフセット有効)	*/
//			uswk = FencV->SenOrgOffset * 100; /* deg -> 0.01deg 換算 */
//		}
//		else
		{
			uswk = scnvorgoffset;	/* PnE8Cは元々0.01deg */
		}
		FencV->OrgDetDelayComp = (((uswk << BitNo) / 100) << FencV->MposSftR) / 360;
	}
	else
	{
		FencV->OrgDetDelayComp = 0;
	}

}


/****************************************************************************
 *
 *	フルクエンコーダ : 変数初期化処理
 *
 ****************************************************************************/
SENC_ERROR FencInitVariables(MENCV *FencV, ASIC_HW_IF_STR *AsicHwReg,
                             MENCPRM *MencP, USHORT scnvorgoffset, BOOL OrgOffsetEnable,
                             LONG KConvLoadSpd, LONG sfgfail_b_h)
{
	LONG		WorkPos;
	SENC_ERROR	err;

	/*----------------------------------------------------------------------*/
	/*	シリアルエンコーダからのデータ読み込み								*/
	/*----------------------------------------------------------------------*/
	err = SencReadInitialP1Data(FencV);

	/*----------------------------------------------------------------------*/
	/*	フルクエンコーダ初期パラメータ計算									*/
	/*----------------------------------------------------------------------*/
	FencV->ComErrLimit = sfgfail_b_h;/*Prm.spgfail.b.h*/
	FencInitPrmCalc(FencV, MencP, scnvorgoffset, OrgOffsetEnable, KConvLoadSpd);

	/*----------------------------------------------------------------------*/
	/*	初回位置データ(今回,前回)の設定										*/
	/*----------------------------------------------------------------------*/
	FencV->MotPos = 0; /* フルク位置 [32bitRingPulse] */
	FencV->RxPosH[1] = FencV->RxPosH[0];
	FencV->MotPosX[0] = FencV->MposSign *
					   ((FencV->RxPosL[0].Long >> FencV->MposSftR) << FencV->MposSftR);
	FencV->MotPosX[1] = FencV->MotPosX[0];

	/*----------------------------------------------------------------------*/
	/* 初回絶対位置データ(絶対位置36Bitデータの下位32Bitデータを取り出す)	*/
	/*----------------------------------------------------------------------*/
	if(FencV->AbsoEncoder != FALSE)
	{
		FencV->MotAbsPos = (FencV->RxPosH[0] << (32 - FSX_FPOS_SFT))
								| ((ULONG)FencV->RxPosL[0].dw >> FSX_FPOS_SFT);
		FencV->MotAbsPos = FencV->MposSign * FencV->MotAbsPos;
	}
	else
	{
		FencV->MotAbsPos = 0;
	}

	/*----------------------------------------------------------------------*/
	/*		分周パルス初期設定												*/
	/*----------------------------------------------------------------------*/
	FencV->DivOutRem = 0;
	WorkPos = (ULONG)FencV->MotPosX[0] >>  FencV->MposSftR;
	FencV->DivOutPos = MlibPfbkxremNolim(WorkPos, FencV->DivOutGain, &FencV->DivOutRem);
	FencV->InitDivPosOffset = FencV->DivOutPos;

	if(FencV->AbsoEncoder == FALSE)
	{	/*------------------------------------------------------------------*/
		/* Ｃ相フラグ関連：インクレエンコーダ								*/
		/*------------------------------------------------------------------*/
		FencV->Cset = FALSE; 		/* Ｃ相設定Flag							*/
		FencV->CposOk = FALSE; 		/* Ｃ相位置設定完了Flag					*/
		if(FencV->DivOut != FALSE)
		{
			/* パルス変換C相マスクON設定	*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			FencV->DivCMaskStatus = DIV_CMASK_ON;
			/* 通常Ｃ相出力選択				*/
			AsicHwReg->AREG_DIVSET &= 0xf3ff;
			FencV->DivModeStatus = DIV_MODE_NORMAL;

			AsicHwReg->AREG_POSET0 = (USHORT)FencV->DivOutPos;
			AsicHwReg->AREG_POTEST = (USHORT)FencV->DivOutPos;
		}
	}
	else
	{	/*------------------------------------------------------------------*/
		/* Ｃ相フラグ関連：絶対値エンコーダ									*/
		/*------------------------------------------------------------------*/
		FencV->Cset = TRUE; 		/* Ｃ相設定Flag	*/
		FencV->CposOk = TRUE; 		/* Ｃ相位置設定完了Flag */
		if(FencV->DivOut != FALSE)
		{
			/* パルス変換C相マスクON設定	*/
			AsicHwReg->AREG_WDT1SET |= BIT8;
			FencV->DivCMaskStatus = DIV_CMASK_ON;
			/* 通常Ｃ相出力選択				*/
			AsicHwReg->AREG_DIVSET &= 0xf3ff;
			FencV->DivModeStatus = DIV_MODE_NORMAL;
		}
	}
	/*----------------------------------------------------------------------*/
	/* Ｃ相フラグ関連：Ｃ相レスエンコーダ									*/
	/*----------------------------------------------------------------------*/
	FencV->MotCphPass = FALSE; 		/* Ｃ相通過Flag							*/
//	FencV->MotCphPassB = FALSE; 	/* Ｃ相通過Flag	for ScanB				*/

	/*----------------------------------------------------------------------*/
	/* エンコーダレディ設定 : エンコーダ初期化・パラメータ初期化の後で設定 	*/
	/*----------------------------------------------------------------------*/
	if(FencV->EncConnect == FALSE) 		/* エンコーダ未接続時				*/
	{
		FencV->SenReady = FALSE; 		/* SEN信号処理完了Flag				*/
		FencV->IncPulseReq = FALSE; 	/* 初期インクレパルス出力要求		*/
	}
	else if(FencV->AbsoEncType != FALSE) /* アブソエンコーダ時				*/
	{
		FencV->SenReady = FALSE; 		/* SEN信号処理完了Flag				*/
		FencV->IncPulseReq = TRUE; 		/* 初期インクレパルス出力要求		*/
	}
	else 								/* インクレエンコーダ時				*/
	{
		FencV->SenReady = TRUE; 		/* SEN信号処理完了Flag				*/
		FencV->IncPulseReq = FALSE; 	/* 初期インクレパルス出力要求		*/
	}

	/*----------------------------------------------------------------------*/
	/* 機能有無選択：インクレパルス出力要求リセット							*/
	/*----------------------------------------------------------------------*/
	FencV->PaoseqNormal = FALSE; 		/* PAO出力Seq 初回処理設定			*/
//	FencV->IncPulseReq = FALSE; 		/* 初期インクレパルス出力要求		*/

	/*----------------------------------------------------------------------*/
	/* 回転速度データの初期値設定											*/
	/*----------------------------------------------------------------------*/
	FencV->MotSpd = 0; 					/* Motor Speed [2^24/OvrSpd]		*/
	FencV->SpdCmpEnable = FALSE;		/* 速度補正処理有効Flag				*/

	return err;
}





#if 0
/****************************************************************************
 *
 *	フルクエンコーダ : ベースタイマー監視
 *
 ****************************************************************************/
void LpxCheckFencBaseTimer(MENCV *FencV)
{
	USHORT BaseTimer;

	BaseTimer = (USHORT) (FencV->RxTimex.dw >> 8);
	if(FencV->ChkAlarmCnt >= 2)		/* ３回目以降チェック */
	{
		if(BaseTimer == FencV->ChkBaseTimer)	/* タイムベースタイマ変化無し? */
		{
			KPI_ALARMSET(ALM_FPGCOMTIMER); 		/* A.CF2 : ＦＰＧ通信タイマ異常	*/
		}
	}
	FencV->ChkBaseTimer = BaseTimer; /* タイムベースタイマ前回値更新		*/

}
#endif

/****************************************************************************
 *
 *	フルクエンコーダ : アラーム監視
 *
 ****************************************************************************/
UCHAR FencGetFencAlarm(MENCV *FencV)
{
	UCHAR AlmCode;

	if( (FencV->MstErrCnt == 0) &&
		((FencV->EncConnect != FALSE) && (FencV->EncDisable == FALSE)) )
	{
//		LpxCheckFencBaseTimer(FencV); /* ベースタイマーチェック				*/
//		LpxCheckFencAlarmCode(FencV); /* アラームコードチェック				*/

		if(FencV->ChkAlarmCnt >= 2) /* ３回目以降チェック */
		{
			AlmCode = (UCHAR)(FencV->RxAlmCode & 0x003B);
			MlibBITFILTER1( FencV->ChkAlmCode[0], FencV->ChkAlmCode[1], AlmCode );
		}
		else
		{
			FencV->ChkAlmCode[0] = 0;
			FencV->ChkAlmCode[1] = 0;
		}

		if(FencV->ChkAlarmCnt < 10)
		{
			FencV->ChkAlarmCnt++;
		}
	}
	else
	{
		FencV->ChkAlarmCnt = 0;
	}

	return ((UCHAR)FencV->ChkAlmCode[0]);
}











/*** end of file ***/
