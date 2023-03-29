/****************************************************************************************************/
/*																									*/
/*																									*/
/*		GainChange.c : ゲイン切り替え関連処理定義													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ゲイン切り替え関連処理																	*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "GainChange.h"
#include "MLib.h"

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	GselInitAutoGainChange( ATGSEL *pAtGselA, ATGSEL *pAtGselB );							*/
/*	LONG	GselDetGainChngTiming( GSELSIG *GselSig, RSP_CTRLBITS *rspctrl_ptr,						*/
/*														POS_CTRL_STS *PosCtrlSts, LONG Condition );	*/
/*	void	GselAutoGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG triger );			*/
/*	void	GselManualGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData,						*/
/*																LONG GselNum, BOOL TuningLess );	*/
/****************************************************************************************************/


/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替初期化																		*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S09B> */
void	GselInitAutoGainChange( ATGSEL *pAtGselA, ATGSEL *pAtGselB )
{
		pAtGselA->var.Timer = 0;					/* 待ち時間1タイマ初期化						*/
		pAtGselB->var.Timer = 0;					/* 待ち時間2タイマ初期化						*/
		return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		サーボゲイン切替変数初期化																	*/
/*																									*/
/****************************************************************************************************/
void	GselInitServoGainChange( GAIN_CHNG_HNDL *hGainChg )
{
ATGSEL *pAtGsel;													/* <S09B> */

	/* 初期化時は第1ゲインを設定 */
	hGainChg->GselGains[0] = hGainChg->GselGains[1];
	hGainChg->GselDobs[0] = hGainChg->GselDobs[1];					/* <S09B> */
	hGainChg->AmonActGain = GAIN1;
	hGainChg->ActGain = 1;

	/* ゲイン切り替え用タイマ初期化 */
	hGainChg->AtGselA.var.Timer = 0;
	hGainChg->AtGselB.var.Timer = 0;

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	MlibResetLongMemory( &(hGainChg->AtGselA.var), sizeof(hGainChg->AtGselA.var)/4 );
	MlibResetLongMemory( &(hGainChg->AtGselB.var), sizeof(hGainChg->AtGselB.var)/4 );
	MlibResetLongMemory( &(hGainChg->AtGselDobsA.var), sizeof(hGainChg->AtGselDobsA.var)/4 );
	MlibResetLongMemory( &(hGainChg->AtGselDobsB.var), sizeof(hGainChg->AtGselDobsB.var)/4 );

	hGainChg->AtGselA.var.prevGain = &(hGainChg->GselGains[1]);
	hGainChg->AtGselA.var.nextGain = &(hGainChg->GselGains[2]);
	hGainChg->AtGselB.var.prevGain = &(hGainChg->GselGains[2]);
	hGainChg->AtGselB.var.nextGain = &(hGainChg->GselGains[1]);

	hGainChg->AtGselDobsA.var.prevGain = &(hGainChg->GselDobs[1]);
	hGainChg->AtGselDobsA.var.nextGain = &(hGainChg->GselDobs[2]);
	hGainChg->AtGselDobsB.var.prevGain = &(hGainChg->GselDobs[2]);
	hGainChg->AtGselDobsB.var.nextGain = &(hGainChg->GselDobs[1]);

	hGainChg->GselTrqFilMin = MlibPcalKf1gain( 10, KPI_SACYCLEUS, 0 );
#endif /* #if 0 */
/*--------------------------------------------------------------------------------------------------*/
/*	<S09B> Auto Gain Change																			*/
/*--------------------------------------------------------------------------------------------------*/
	GselInitAutoGainChange( &(hGainChg->AtGselA),				/* condition A */
							&(hGainChg->GselGains[1]),			/* prevGain */
							&(hGainChg->GselGains[2]),			/* nextGain */
							&(hGainChg->GselDobs[1]),			/* prevGain(Dobs) */
							&(hGainChg->GselDobs[2]) );			/* nextGain(Dobs) */
	GselInitAutoGainChange( &(hGainChg->AtGselB),				/* condition B */
							&(hGainChg->GselGains[2]),			/* prevGain */
							&(hGainChg->GselGains[1]),			/* nextGain */
							&(hGainChg->GselDobs[2]),			/* prevGain(Dobs) */
							&(hGainChg->GselDobs[1]) );			/* nextGain(Dobs) */
/*--------------------------------------------------------------------------------------------------*/
/*	<S09B> Δゲイン パラメータ再計算(念のため)														*/
/*--------------------------------------------------------------------------------------------------*/
	pAtGsel = &(hGainChg->AtGselA);								/* condition A */
	PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kp */
	PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kv, Kvi */
	PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Klpf */
	PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate delta Ks */
	pAtGsel = &(hGainChg->AtGselB);								/* condition B */
	PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kp */
	PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Kv, Kvi */
	PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain) );		/* calculate delta Klpf */
	PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate delta Ks */
/*--------------------------------------------------------------------------------------------------*/

	/* CoinOn, NearOn, RefZOn */
//	hGainChg->GselSig.CoinOn  = TRUE;
//	hGainChg->GselSig.CoinOff = FALSE;
//	hGainChg->GselSig.NearOn  = TRUE;
//	hGainChg->GselSig.NearOff = FALSE;
//	hGainChg->GselSig.RefZOn  = TRUE;
//	hGainChg->GselSig.RefZOff = FALSE;

}


/****************************************************************************************************/
/*																									*/
/*		ゲイン切替条件判定処理																		*/
/*																									*/
/****************************************************************************************************/
LONG	GselDetGainChngTiming( BOOL CoinSignal, BOOL NearSignal, BOOL RefZSignal, LONG Condition )
{
	LONG	triger;

	/* 条件Ａ成立で第２ゲイン遷移または第２ゲイン選択中 */
	switch( Condition )
	{
	case 0x00:
		triger = CoinSignal;
		break;
	case 0x01:
		triger = CoinSignal ^ 0x01;
		break;
	case 0x02:
		triger = NearSignal;
		break;
	case 0x03:
		triger = NearSignal ^ 0x01;
		break;
	case 0x04:
		triger = RefZSignal;
		break;
	case 0x05:
		triger = RefZSignal ^ 0x01;
		break;
	default:
		triger = 0;
		break;
	}
	return triger;
}
#if 0
LONG	GselDetGainChngTiming( GAIN_CHNG_HNDL *hGainChg, BOOL CoinSignal,
    	                       BOOL NearSignal, BOOL RefZSignal, LONG Condition )
{
	LONG	triger;

	/* ローカル変数の初期化 */
	triger = 0;

	/* 自動ゲイン切替用トリガ信号作成 */
	GselSig->CoinOn		= PosCtrlSts->CoinSignal;
	GselSig->CoinOff	= PosCtrlSts->CoinSignal ^ 0x01;
	GselSig->NearOn		= PosCtrlSts->NearSignal;
	GselSig->NearOff	= PosCtrlSts->NearSignal ^ 0x01;
	GselSig->RefZOn		= PosCtrlSts->RefZSignal;
	GselSig->RefZOff	= PosCtrlSts->RefZSignal ^ 0x01;

	/* 条件Ａ成立で第２ゲイン遷移または第２ゲイン選択中 */
	switch( Condition )
	{
	case 0x00:
		triger = GselSig->CoinOn;
		break;
	case 0x01:
		triger = GselSig->CoinOff;
		break;
	case 0x02:
		triger = GselSig->NearOn;
		break;
	case 0x03:
		triger = GselSig->NearOff;
		break;
	case 0x04:
		triger = GselSig->RefZOn;
		break;
	case 0x05:
		triger = GselSig->RefZOff;
		break;
	default:
		triger = 0;
		break;
	}
	return triger;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替																				*/
/*																									*/
/*		ゲイン切替処理をμプログラムにて実施する。CPU側では、条件判定及び待ち時間管理を行い、		*/
/*		μプログラムへゲイン切替要求を送る(MREG_BITCMDI.BITCMD_GAINCHNGON)。						*/
/*		※MREG_BITCMDI.BITCMD_GAINCHNGON = ONで第二ゲインに切り替わる。								*/
/*																									*/
/*		なお、⊿ゲイン切替機能(Pn131)は非対応とする。												*/
/*																									*/
/****************************************************************************************************/
#if 0	/* <S09B> AutoGainChange.cに移動 */
void	GselAutoGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG triger, CTRL_CMD_PRM *CtrlCmdPrm)
{
	ATGSEL			*atgsela_ptr;
	ATGSEL			*atgselb_ptr;

	atgsela_ptr		= &( hGainChg->AtGselA );
	atgselb_ptr		= &( hGainChg->AtGselB );

	if( triger )
	{
		if( atgsela_ptr->var.Timer >= atgsela_ptr->conf.Wait )
		{ /* 第1→第2ゲイン待ち時間経過後 */
			/* 第2ゲイン */
		/*------------------------------------------------------------------------------------------*/
		/*	以下の処理をμプログラムにて実施する。													*/
		/*		hGainChg->GselGains[0]	= hGainChg->GselGains[2];									*/
		/*		pDobsData->param[0]		= pDobsData->param[2];										*/
		/*------------------------------------------------------------------------------------------*/
			hGainChg->GselGains[0].VirtualPosKp = hGainChg->GselGains[2].VirtualPosKp;
			hGainChg->AmonActGain = GAIN2;
			hGainChg->ActGain = 2;
			atgselb_ptr->var.Timer = 0; /* 待ち時間2タイマ初期化 */
		}
		else
		{
			atgsela_ptr->var.Timer++; /* 待ち時間1タイマカウントアップ */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		条件B成立で第1ゲイン遷移または第1ゲイン選択中												*/
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		if( atgselb_ptr->var.Timer >= atgselb_ptr->conf.Wait )
		{ /* 第2→第1ゲイン待ち時間経過後 */
			/* 第1ゲイン */
		/*------------------------------------------------------------------------------------------*/
		/*	以下の処理をμプログラムにて実施する。													*/
		/*		hGainChg->GselGains[0]	= hGainChg->GselGains[1];									*/
		/*		pDobsData->param[0]		= pDobsData->param[1];										*/
		/*------------------------------------------------------------------------------------------*/
			hGainChg->GselGains[0].VirtualPosKp = hGainChg->GselGains[1].VirtualPosKp;
			hGainChg->AmonActGain = GAIN1;
			hGainChg->ActGain = 1;
			atgsela_ptr->var.Timer = 0; /* 待ち時間１タイマ初期化 */
		}
		else
		{
			atgselb_ptr->var.Timer++; /* 待ち時間２タイマカウントアップ */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		条件AでもBでもない状態の時は前回のゲインのまま												*/
/*--------------------------------------------------------------------------------------------------*/
	/* MFCは自動ゲイン切替えなし。第1ゲインのまま */
	pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];

	/*------------------------------------------------------------------------------------------*/
	/*	以下の処理をμプログラムにて実施する。													*/
	/*		速度制限用ゲインは、自動ゲイン切替なし。第１ゲインのまま							*/
	/*		pTrqCtrl->conf.OverTrqLoopGain[0]	= pTrqCtrl->conf.OverTrqLoopGain[1];			*/
	/*		pTrqCtrl->conf.OverTrqIntegGain[0]	= pTrqCtrl->conf.OverTrqIntegGain[1];			*/
	/*------------------------------------------------------------------------------------------*/
	CtrlCmdPrm->OverTrqLoopGain[0]	= CtrlCmdPrm->OverTrqLoopGain[1];
	CtrlCmdPrm->OverTrqIntegGain[0]	= CtrlCmdPrm->OverTrqIntegGain[1];

	return;
}
#endif


/****************************************************************************************************/
/*																									*/
/*		マニュアルゲイン切替																		*/
/*																									*/
/*		ゲイン切替処理をμプログラムにて実施する。CPU側では、MECHATROLINKより受け取ったバンクNoに	*/
/*		応じて、μプログラムへゲイン切替要求を送る(MREG_BITCMDI.BITCMD_GAINCHNGON)。				*/
/*																									*/
/****************************************************************************************************/
//<S09B>void	GselManualGainChange( GAIN_CHNG_HNDL *hGainChg, MFCTRL *pMfcData, LONG GselNum, BOOL TuningLess, 
//<S09B>				CTRL_CMD_PRM *CtrlCmdPrm )
void GselManualGainChange( BASE_CTRL_HNDL *BaseControls )					/* <S09B> */
{
GAIN_CHNG_HNDL *hGainChg	= &(BaseControls->GainChange);					/* <S09B> */
MFCTRL *pMfcData			= &(BaseControls->MFControl);					/* <S09B> */
LONG GselNum				= BaseControls->GselNum;						/* <S09B> */
BOOL TuningLess				= BaseControls->TuneLessCtrl.var.TuneLessAct;	/* <S09B> */
CTRL_CMD_PRM *CtrlCmdPrm	= &(BaseControls->CtrlCmdPrm);					/* <S09B> */
//DOBS_CTRL *pDobs			= &(BaseControls->DobsCtrl);					/* <S09B> *///<S0C7>
	/*------------------------------------------------------------------------------------------*/
	/*	以下の処理をμプログラムにて実施する。													*/
	/*		調整レス用ゲイン選択 (5バンク目)													*/
	/*		if( TuningLess != FALSE )															*/
	/*		{ 調整レス有効 -> ゲイン切替えを無効とし、調整レス用ゲインを使用する				*/
	/*			hGainChg->GselGains[0] = hGainChg->GselGains[5];								*/
	/*			外乱オブザーバ																	*/
	/*			pDobsData->param[0] = pDobsData->param[1];										*/
	/*			速度制限ループゲイン															*/
	/*			pTrqCtrl->conf.OverTrqIntegGain[0] = pTrqCtrl->conf.OverTrqIntegGain[1];		*/
	/*			速度制限積分ゲイン																*/
	/*			pTrqCtrl->conf.OverTrqLoopGain[0] = pTrqCtrl->conf.OverTrqLoopGain[1];			*/
	/*		}																					*/
	/*------------------------------------------------------------------------------------------*/
	if( TuningLess != FALSE )
	{ /* 調整レス有効 -> ゲイン切替えを無効とし、調整レス用ゲインを使用する */
		/* 基本制御用ゲイン */
		hGainChg->GselGains[0] = hGainChg->GselGains[5];
		/* 外乱オブザーバ <S09B> */
		hGainChg->GselDobs[0] = hGainChg->GselDobs[1];					/* <S09B> */
		/* MFCゲイン */
		pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];
		/* 現在ゲイン確認用 */
		hGainChg->ActGain = 1;
		hGainChg->AmonActGain = GAIN1;
		/*			速度制限ループゲイン						*/
		CtrlCmdPrm->OverTrqLoopGain[0] = CtrlCmdPrm->OverTrqLoopGain[1];
		/*			速度制限積分ゲイン							*/
		CtrlCmdPrm->OverTrqIntegGain[0] = CtrlCmdPrm->OverTrqIntegGain[1];
	}
	else
	{
	/*------------------------------------------------------------------------------------------*/
	/*	以下の処理をμプログラムにて実施する。													*/
	/*		if( GselNum < SVCDEF_NETGSEL_SETNUM )												*/
	/*		{ 第1ゲイン、第2ゲイン選択時														*/
	/*			hGainChg->GselGains[0] = hGainChg->GselGains[GselNum+1]							*/
	/*			外乱オブザーバ																	*/
	/*			pDobsData->param[0] = pDobsData->param[GselNum+1];								*/
	/*			速度制限ループゲイン															*/
	/*			pTrqCtrl->conf.OverTrqIntegGain[0] = pTrqCtrl->conf.OverTrqIntegGain[GselNum+1];*/
	/*			速度制限積分ゲイン																*/
	/*			pTrqCtrl->conf.OverTrqLoopGain[0] = pTrqCtrl->conf.OverTrqLoopGain[GselNum+1];	*/
	/*		}																					*/
	/*------------------------------------------------------------------------------------------*/
		if( GselNum < SVCDEF_NETGSEL_SETNUM )
		{ /* 第1ゲイン、第2ゲイン選択時 */
			/* 基本制御用ゲイン */
			hGainChg->GselGains[0] = hGainChg->GselGains[GselNum + 1];
			/* 外乱オブザーバ <S09B> */
			hGainChg->GselDobs[0] = hGainChg->GselDobs[GselNum + 1];			/* <S09B> */
			/* MFCゲイン */
			pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[GselNum + 1];
			/* 現在ゲイン確認用 */
			hGainChg->ActGain = GselNum + 1;
			hGainChg->AmonActGain = GAIN1 << GselNum;
			/*			速度制限ループゲイン						*/
			CtrlCmdPrm->OverTrqLoopGain[0] = CtrlCmdPrm->OverTrqLoopGain[GselNum+1];
			/*			速度制限積分ゲイン							*/
			CtrlCmdPrm->OverTrqIntegGain[0] = CtrlCmdPrm->OverTrqIntegGain[GselNum+1];
		}
		else
		{ /* 第3ゲイン、第4ゲイン選択時は、第1ゲイン固定 */
			/* MFCゲイン */
			pMfcData->conf.MfcPrm[0] = pMfcData->conf.MfcPrm[1];
			/* 外乱オブザーバ <S09B> */
			hGainChg->GselDobs[0] = hGainChg->GselDobs[1];			/* <S09B> */
			/* 現在ゲイン確認用 */
			hGainChg->ActGain = 1;
			hGainChg->AmonActGain = GAIN1;
			/*			速度制限ループゲイン						*/
			CtrlCmdPrm->OverTrqLoopGain[0] = CtrlCmdPrm->OverTrqLoopGain[1];
			/*			速度制限積分ゲイン							*/
			CtrlCmdPrm->OverTrqIntegGain[0] = CtrlCmdPrm->OverTrqIntegGain[1];
		}
	}

	return;
}



/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替パラメータ計算																*/
/*																									*/
/****************************************************************************************************/
void	GselCalculatePrm( GAIN_CHNG_HNDL *hGainChg, USHORT gnswwait1, USHORT gnswwait2, LONG ScanTime )
{
/*--------------------------------------------------------------------------------------------------*/
/*		ゲイン切替待ち時間１(Pn135) [ms]															*/
/*--------------------------------------------------------------------------------------------------*/
	hGainChg->AtGselA.conf.Wait = gnswwait1 * 1000 / ScanTime;

/*--------------------------------------------------------------------------------------------------*/
/*		ゲイン切替待ち時間２(Pn136) [ms]															*/
/*--------------------------------------------------------------------------------------------------*/
	hGainChg->AtGselB.conf.Wait = gnswwait2 * 1000 / ScanTime;

}



/****************************************************************************************************/
/*																									*/
/*		自動ゲイン切替ポインタ取得API																*/
/*																									*/
/****************************************************************************************************/
BOOL	GselGet_AtGselStatus( GAIN_CHNG_HNDL *hGainChg )
{
	return hGainChg->AtGsel;
}

/***************************************** end of file **********************************************/
