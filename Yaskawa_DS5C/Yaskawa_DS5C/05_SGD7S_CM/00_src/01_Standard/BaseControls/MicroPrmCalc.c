/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ini_sub.c : SVN初期化関連サブ関数モジュール													*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : SVN初期化関連サブ関数を定義。															*/
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
/*	Note	:	初版	2009.06.04	Y.Oka															*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "JL086ServoIpHw.h"
//#include "PowerManager.h"
#include "MLib.h"
#include "KLib.h"
#include "MicroPrmCalc.h"

/* defines */
#define	D_AXIS			0
#define	Q_AXIS			1

#define	REQ_LMT_CHK		1
#define	NO_LMT_CHK		0



/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void KpxPcalMicroProgram( UNI_PRM_STR *uni_prm_ptr, ASICS *SvAsicRegs,							*/
/*													USHORT *mreg_csw_ptr, LONG caller_axis_no );	*/
/*	void KpiPcalMicroNotchFilter2( ASICS *SvAsicRegs, LONG hz, LONG qx, LONG dx, SHORT *MpReg );	*/
/*	LONG KpiChangePwmFrequency( ULONG PwmFx, PRMDATA *prm_ptr );									*/
/*	LONG LpxSetPrmToASIC( SHORT *adr, LONG prm, BOOL limchk );										*/
/*	LONG MicroTranslatePrmReq( ASICS *SvAsicRegs, void *Data, VUSHORT *Address, LONG Size );		*/
/****************************************************************************************************/
static LONG	PcalDqAcrKc( LONG Iadmax, USHORT imax, USHORT imaxs );
static LONG	PcalLDidt( KSGAIN Le, KSGAIN max_cur, KSGAIN vdc );
static LONG	PcalDqLxComp( KSGAIN Lx, KSGAIN over_spd, KSGAIN max_cur, KSGAIN vdc );
static LONG	PcalDqRxComp( KSGAIN Rx, KSGAIN max_cur, KSGAIN vdc );
static LONG	PcalDqEmfComp( KSGAIN Emf, KSGAIN over_spd, KSGAIN vdc );
static LONG	PcalOndelayLevel( LONG Level );
static LONG	PcalOndelaySlope( LONG Slope );
static LONG	PcalPwmFrequency( LONG PwmFx );
static	LONG	PcalPwmCounter( LONG CarrCnt );		/* モータキャリア周波数切り替え<S117> */
static LONG	PcalVFBKiLim( LONG TvLim );
static void	KpiPcalCurrentObs( ASICS *SvAsicRegs, USHORT ignq, USHORT pwmf, BPRMDAT *Bprm, SHORT *MpReg);
static void	KpiPcalMicroLpassFilter1( LONG tf, LONG ts, SHORT *MpReg );



/****************************************************************************************************/
/*																									*/
/*		μプログラム用パラメータ計算メイン関数														*/
/*																									*/
/****************************************************************************************************/
LONG	KpxPcalMicroProgram( MAICRO_CFGPRM *uCfgPrm, BPRMDAT *Bprm,
							 ASICS *SvAsicRegs, WEAKENFIELD* WeakenField )
{
	ASIC_U_IF_STR	*pAsicMicroReg;
	LONG			kp, kx;
	LONG			rc;

#if (FLOAT_USE==FALSE)
	LONG			sx;
#endif /* FLOAT_USE */


	pAsicMicroReg = SvAsicRegs->AsicMicroReg;
	rc  = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		制御軸数の設定																<S0E4>			*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->AsicMicroReg->AxisInfo.AxisNum = MAX_AXIS_NUMBER;

/*--------------------------------------------------------------------------------------------------*/
/*		制御フラグ(CSW)の設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->MicroCsw.data = (USHORT)( uCfgPrm->dq_sw | 0x00F0 );			/* 制御フラグ		*/
//	if( uCfgPrm->pwmf > 4000 )
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* 設定キャリアカウンタ値から周波数を逆算	*//* モータキャリア周波数切り替え<S117> */
	if( kx > 4000 )
	{
		/* キャリア周波数が4kHz以上のアンプでは電流オブザーバ未使用 */
		SvAsicRegs->MicroCsw.data &= 0xFFF7;
	}

	if(( uCfgPrm->flg_wf & 0x0001 ) == 0 )
	{ /* モータ側の弱め界磁有効無効チェック */
		/* モータ側が無効の場合、強制的に無効 */
		SvAsicRegs->MicroCsw.data &= 0xF7FF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x0800 ) == 0 )
	{
		/* 弱め界磁=OFFの場合，弱め界磁2=OFFにする */
		SvAsicRegs->MicroCsw.data &= 0xDFFF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x2000 ) != 0 )
	{
		/* 弱め界磁2=ONの場合，積分停止機能=ONにする */
		SvAsicRegs->MicroCsw.data |= 0x4000;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		電流換算ゲイン＆電圧補正ゲイン																*/
/*--------------------------------------------------------------------------------------------------*/
	/* 電流換算ゲイン */
	kx = PcalDqAcrKc( uCfgPrm->shant, uCfgPrm->imax, uCfgPrm->imaxs );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Kc), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* 電圧補正ゲイン */
	kx = uCfgPrm->vrevgn;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KvvIn), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		d軸電流ループゲイン(比例＆積分＆微分)														*/
/*--------------------------------------------------------------------------------------------------*/
	/* 比例ゲイン */
	kp = PcalDqAcrKp( uCfgPrm->ignd, 100, Bprm, D_AXIS );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KdP), kp, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* 積分ゲイン */
	kx = PcalDqAcrKi( uCfgPrm->kid, kp, Bprm->MotLd, Bprm );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KdI), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* 積分制限値 */
	kx = uCfgPrm->limintd;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.VdLim), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* L(di/dt)のL */
	kx = PcalLDidt( Bprm->MotLd, Bprm->MaxCur, Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.L_dIdt), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		q軸電流ループゲイン(比例＆積分＆微分)														*/
/*--------------------------------------------------------------------------------------------------*/
	/* 比例ゲイン */
	kp = PcalDqAcrKp( uCfgPrm->ignq, 100, Bprm, Q_AXIS );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KqP), kp, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* 積分ゲイン */
	kx = PcalDqAcrKi( uCfgPrm->kiq, kp, Bprm->MotLq, Bprm );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.KqI), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* 積分制限値 */
	kx = uCfgPrm->limintq;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.VqLim), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		電流ループ出力フィルタ時定数																*/
/*--------------------------------------------------------------------------------------------------*/
	/* 出力フィルタゲイン */
	kx = PcalDqAcrFilter( uCfgPrm->ifil );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Tfil), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		インダクタンス補償定数(Ld,Lq)																*/
/*--------------------------------------------------------------------------------------------------*/
	/* d軸インダクタンス */
	kx = PcalDqLxComp( Bprm->MotLd, Bprm->OvrSpd, Bprm->MaxCur,  Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Ld), kx, REQ_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* q軸インダクタンス */
	kx = PcalDqLxComp( Bprm->MotLq, Bprm->OvrSpd, Bprm->MaxCur,  Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Lq), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		機械角 → 電気角変換係数																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* 機械角 → 電気角変換係数 */
#if (FLOAT_USE==TRUE) 
	kx = (LONG)Bprm->Keangle;
#else
	kx = MlibScalKskxkx( Bprm->Keangle, 1, 1, &sx, -24 );
#endif /* FLOAT_USE */
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdV.KEangle), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		電機子抵抗(R1)補償定数																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* R1補償定数 */
	kx = PcalDqRxComp( Bprm->MotR, Bprm->MaxCur, Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.MotResist), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		EMF補償定数(Φ)																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* EMF補償定数 */
	kx = PcalDqEmfComp( Bprm->MotEmf, Bprm->OvrSpd, Bprm->Vdc );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Mag), kx, REQ_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		オンディレイ補償																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* オンディレイ補償時間 */
	kx = uCfgPrm->odt_c.b.h * (ASIC_CLKMHZ/2) / 10;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.OnDelayComp),  kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* オンディレイ補償切替レベル */
	kx = PcalOndelayLevel( uCfgPrm->odlv_slp.b.l );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.OnDelayLvl), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* オンディレイ補償傾き */
	kx = PcalOndelaySlope( uCfgPrm->odlv_slp.b.h );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.OnDelaySlope), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ゼロ調																				*/
/*--------------------------------------------------------------------------------------------------*/
	/* U相電流ゼロ調 */
	kx = (SHORT)(uCfgPrm->zadjiu);
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.IuOffsetIn), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* V相電流ゼロ調 */
	kx = (SHORT)(uCfgPrm->zadjiv);
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.IvOffsetIn), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ゲイン調																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* U相電流ゲイン調 */
	kx = (CHAR)uCfgPrm->gadjiu_v.b.l + 0x100;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.KcuIn), kx, NO_LMT_CHK );
	/*----------------------------------------------------------------------------------------------*/
	/* V相電流ゲイン調 */
	kx = (CHAR)uCfgPrm->gadjiu_v.b.h + 0x100;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->AdinV.KcvIn), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		PWMキャリア周波数カウント値																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* PWMキャリア周波数 */
//	kx =  KpiChangePwmFrequency( 0, uCfgPrm->pwmf, uCfgPrm->pwmf3, uCfgPrm->flg_wf );
////	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.CrFreq), kx, NO_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(pAsicMicroReg->IntAdP.CrFreq), uCfgPrm->NormCarrCnt, NO_LMT_CHK );/*<S0C7>*//* モータキャリア周波数切り替え<S117> */

/*--------------------------------------------------------------------------------------------------*/
/*		電流ループ電圧制限値																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* 電圧制限値 */
	kx = uCfgPrm->lmdata;
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.Vmax), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		トルクフィルタ : 一次遅れフィルタ１(不使用,念のため)										*/
/*--------------------------------------------------------------------------------------------------*/
//	KpiManageMicroTorqueFilter( CSW_LPFDSABL, FALSE, mreg_csw_ptr, pAsicMicroReg );
	SvAsicRegs->MicroCsw.data |= LPFDSABL;
	KpiPcalMicroLpassFilter1( 0, KPI_MACYCLENS, &(pAsicMicroReg->IntAdP.TLpf) );

#if 0 /* 2012.08.31 Y.Oka 機能無効のため、コメントアウト */
/*--------------------------------------------------------------------------------------------------*/
/*		トルクフィルタ : 一次遅れフィルタ２(Pn411,SGDXでは不使用,念のため)							*/
/*--------------------------------------------------------------------------------------------------*/
//	KpiManageMicroTorqueFilter( CSW_LPFCDSABL, FALSE, mreg_csw_ptr, pAsicMicroReg  );
	SvAsicRegs->MicroCsw.data |= LPFCDSABL;
	KpiPcalMicroLpassFilter1( 0, KPI_MACYCLENS, &(pAsicMicroReg->MREG_TLPFC) );
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		モータ共振抑制用ノッチフィルタ																*/
/*--------------------------------------------------------------------------------------------------*/
	if ( (uCfgPrm->motresfrq != 0) && (uCfgPrm->motresq != 0) )
	{ /* 設定周波数≠0のとき、ノッチフィルタ有効 */
	/*----------------------------------------------------------------------------------------------*/
		KpiPcalMicroNotchFilter2(						/* ２次ノッチフィルタのパラメータ計算		*/
			SvAsicRegs,									/* JL-076アクセス用構造体					*/
			uCfgPrm->motresfrq,							/* フィルタ周波数		[Hz]				*/
			uCfgPrm->motresq,							/* フィルタＱ定数		[0.01]				*/
			0,											/* フィルタ深さ			[0.001]				*/
			&(pAsicMicroReg->IntAdP.Kf31) );			/* フィルタパラメータ設定先頭アドレス		*/
	/*----------------------------------------------------------------------------------------------*/
		/* ノッチフィルタ３有効 */
//		KpiManageMicroTorqueFilter( CSW_F3DSABL, TRUE, mreg_csw_ptr, pAsicMicroReg );
		SvAsicRegs->MicroCsw.data &= ~F3DSABL;
	}
	else
	{
		/* ノッチフィルタ３無効 */
//		KpiManageMicroTorqueFilter( CSW_F3DSABL, FALSE, mreg_csw_ptr, pAsicMicroReg );
		SvAsicRegs->MicroCsw.data |= F3DSABL;
	}

	/* Set to ASIC */
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->IntAdP.CtrlSw), SvAsicRegs->MicroCsw.data, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		電圧ループ積分リミット																		*/
/*--------------------------------------------------------------------------------------------------*/
	kx = PcalVFBKiLim( uCfgPrm->idlim );
	rc |= LpxSetPrmToASIC( &(pAsicMicroReg->WeakFV.WfIntegLim), kx, NO_LMT_CHK );

/*--------------------------------------------------------------------------------------------------*/
/*		d軸電流指令リミット値計算用定数の計算														*/
/*--------------------------------------------------------------------------------------------------*/
	PcalIdrefLimTerm234( WeakenField, SvAsicRegs, Bprm, uCfgPrm->oslv_pol, uCfgPrm->pnvoltgn );

/*--------------------------------------------------------------------------------------------------*/
/*		電流オブザーバパラメータ計算																*/
/*--------------------------------------------------------------------------------------------------*/
	/* パラメータ設定 */
//	KpiPcalCurrentObs( SvAsicRegs, uCfgPrm->ignq, uCfgPrm->pwmf, Bprm, &(pAsicMicroReg->DobsP.TsPerL) );		/* モータキャリア周波数切り替え<S117> */
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* 設定キャリアカウンタ値から周波数を逆算	*/
	KpiPcalCurrentObs( SvAsicRegs, uCfgPrm->ignq, kx, Bprm, &(pAsicMicroReg->DobsP.TsPerL) );

	return rc;
}

#if 0		/* ユニットキャリア固定処理不要のため削除<S11E> */
#if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) )		/* モータキャリア周波数切り替え<S117> */
/*----------------------------------------------------------------------------------*/
/*  @@@@@@ SGD7W専用キャリア周波数調整処理 別キャリア設定を確認までの超暫定 @@@@@@@ */
/*----------------------------------------------------------------------------------*/
LONG	KpxRecalcPWMparameter( MAICRO_CFGPRM *uCfgPrm, BPRMDAT *Bprm, ASICS *SvAsicRegs )
{
	ASIC_U_IF_STR	*pAsicMicroReg;
	LONG			kx;
	LONG			rc;

	pAsicMicroReg = SvAsicRegs->AsicMicroReg;
	rc  = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		制御フラグ(CSW)の設定																		*/
/*--------------------------------------------------------------------------------------------------*/
	SvAsicRegs->MicroCsw.data = (USHORT)( uCfgPrm->dq_sw | 0x00F0 );			/* 制御フラグ		*/
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* 設定キャリアカウンタ値から周波数を逆算	*/
	if( kx > 4000 )
	{
		/* キャリア周波数が4kHz以上のアンプでは電流オブザーバ未使用 */
		SvAsicRegs->MicroCsw.data &= 0xFFF7;
	}

	if(( uCfgPrm->flg_wf & 0x0001 ) == 0 )
	{ /* モータ側の弱め界磁有効無効チェック */
		/* モータ側が無効の場合、強制的に無効 */
		SvAsicRegs->MicroCsw.data &= 0xF7FF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x0800 ) == 0 )
	{
		/* 弱め界磁=OFFの場合，弱め界磁2=OFFにする */
		SvAsicRegs->MicroCsw.data &= 0xDFFF;
	}

	if( ( SvAsicRegs->MicroCsw.data & 0x2000 ) != 0 )
	{
		/* 弱め界磁2=ONの場合，積分停止機能=ONにする */
		SvAsicRegs->MicroCsw.data |= 0x4000;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		PWMキャリア周波数カウント値																	*/
/*--------------------------------------------------------------------------------------------------*/
	/* PWMキャリア周波数 */
	rc |= LpxSetPrmToASIC( (SHORT*)&(pAsicMicroReg->IntAdP.CrFreq), uCfgPrm->NormCarrCnt, NO_LMT_CHK );/*<S0C7>*/

/*--------------------------------------------------------------------------------------------------*/
/*		電流オブザーバパラメータ計算																*/
/*--------------------------------------------------------------------------------------------------*/
	/* パラメータ設定 */
	kx = PcalPwmCounter( uCfgPrm->NormCarrCnt );		/* 設定キャリアカウンタ値から周波数を逆算	*/
	KpiPcalCurrentObs( SvAsicRegs, uCfgPrm->ignq, kx, Bprm, &(pAsicMicroReg->DobsP.TsPerL) );

	return rc;
}
#endif	/* #if ( defined(CSW_SGD7W_TYPE) || defined(CSW_COMMON_CARRIER) ) *//* モータキャリア周波数切り替え<S117> */
#endif	/* #if 0		ユニットキャリア固定処理不要のため削除<S11E> */


/*<S1F5> START*/
/*****************************************************************************
  Description: OCレベル
------------------------------------------------------------------------------
  Parameters:
	uCfgPrm             アラーム管理ＩＦデータ
	SvAsicRegs			ASICアドレステーブルへのポインタ
	MencP				エンコーダパラメータ定義
  Return:

------------------------------------------------------------------------------
  Note:

*****************************************************************************/
#if 0	/* <S1F7> */
LONG	KpxMotorOCLevel( MAICRO_CFGPRM *uCfgPrm, ASICS *SvAsicRegs, MENCPRM *MencP )
{
	LONG	kx;
	LONG	rc;
	LONG	OcLevel;

	rc  = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		マスク状態																						*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MencP->motfuncf.f.OCset;
	if( kx == 1 )
	{
		rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_SETMSK), kx, REQ_LMT_CHK );
	}
/*--------------------------------------------------------------------------------------------------*/
/*		OC検出レベル																					*/
/*--------------------------------------------------------------------------------------------------*/
	OcLevel = MlibDiv6432( (MencP->irat * MencP->mot_oc), (uCfgPrm->shant * 4), (uCfgPrm->imaxs * 100) );
	kx = OcLevel;
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVUP), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVUM), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVVP), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVVM), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVWP), kx, REQ_LMT_CHK );
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCLVWM), kx, REQ_LMT_CHK );
/*--------------------------------------------------------------------------------------------------*/
/*		デシメーションフィルタ動作モード																		*/
/*--------------------------------------------------------------------------------------------------*/
	kx = 0;
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_OCDCONF1), kx, REQ_LMT_CHK );
/*--------------------------------------------------------------------------------------------------*/
/*		モータ過電流検出フィルタ回数																		*/
/*--------------------------------------------------------------------------------------------------*/
	kx = uCfgPrm->fltrange;
	rc |= LpxSetPrmToASIC( (SHORT*)&(SvAsicRegs->AsicHwReg->AREG_FLTRANGE), kx, REQ_LMT_CHK );
	return( rc );
}
/*<S1F5> END*/
#else	/* <S1F7> */
LONG	KpxMotorOCLevel( MAICRO_CFGPRM *uCfgPrm, ASICS *SvAsicRegs, MENCPRM *MencP )
{
	LONG			rc;
	LONG			OcMaxU, OcMaxV, OcMaxW;			/* OC検出閾値(正側)								*/
	LONG			OcMinU, OcMinV, OcMinW;			/* OC検出閾値(負側)								*/
	LONG			OcWorkU, OcWorkV, OcWorkW;		/* OC検出閾値演算用ワーク						*/
	LONG			OcLevel;						/* OC検出レベル(ゲイン調、オフセット調前)		*/
	ASIC_HW_IF_STR	*pAsicHwReg = SvAsicRegs->AsicHwReg;

	rc  = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		マスク状態																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( (LONG)MencP->motfuncf.f.OCset == 1 )
	{
		pAsicHwReg->AREG_SETMSK		|= 0x000F;			/* OC検出無効								*/
/*--------------------------------------------------------------------------------------------------*/
/*		OC検出有効																					*/
/*--------------------------------------------------------------------------------------------------*/
		pAsicHwReg->AREG_SETREG1	= 0x00000005;		/* ASIC Write Enable						*/
		pAsicHwReg->AREG_CSTP		|= ( 1 << 26 );		/* OC検出無効								*/
		pAsicHwReg->AREG_SETREG1	= 0x00000001;		/* ASIC Write Disable						*/
/*--------------------------------------------------------------------------------------------------*/
		return( rc );									/* 無効の場合は、他の設定は行わない。		*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		デシメーションフィルタ動作モード															*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_OCDCONF1	&= 0xFFF0;
	pAsicHwReg->AREG_OCSDMCONF3	= 0x00001B38;	/*
								bit0 - 7:OC検出用ADIFサイクル数                0x38(電流ADと同一)
								bit8 -11:AD基本クロックビット                  0x0B(電流ADと同一)
								bit12   :MSB選択                               0x01(電流ADと同一)	*/
	pAsicHwReg->AREG_OCSDMCONF4	= 0x00003839;	/*
								bit0 - 7:OC検出用ADコントロール0立ち上がり設定 0x39(電流ADと同一)
								bit8 -11:OC検出用ADコントロール0降下設定       0x38(電流ADと同一)	*/
/*--------------------------------------------------------------------------------------------------*/
/*		OC検出レベル																				*/
/*		0x0000(-320mV) ～ 0xFFFF(+320mV)	shant max 8192											*/
/*			正側 ＝ 0x8000 +（ PnF2C × PnE28 × 4 ）÷ PnE15										*/
/*			負側 ＝ 0x8000 -（ PnF2C × PnE28 × 4 ）÷ PnE15										*/
/*		PnF2C = MencP->mot_oc  : モータOC検出電流[0.1A]												*/
/*		PnE28 = uCfgPrm->shant : 電流検出ゲイン														*/
/*		PnE15 = uCfgPrm->imaxs : サーボパック最大電流[0.1A]											*/
/*																									*/
/*		ゲイン、オフセット補正計算																	*/
/*			補正後 ＝ 補正前 ÷ ( 電流検出ゲイン調 + 1.00 ) - 電流検出ゼロ調 × 4					*/
/*		PnE50   = uCfgPrm->zadjiu : 電流検出ゼロ調(U相)												*/
/*		PnE51   = uCfgPrm->zadjiv : 電流検出ゼロ調(V相)												*/
/*		PnE52.L = uCfgPrm->gadjiu_v.b.l : 電流検出ゲイン調(U相)										*/
/*		PnE52.H = uCfgPrm->gadjiu_v.b.h : 電流検出ゲイン調(V相)										*/
/*--------------------------------------------------------------------------------------------------*/
	/* OC検出レベル */
	OcLevel = MlibDiv6432(( MencP->mot_oc ), ( uCfgPrm->shant * 4 ), ( uCfgPrm->imaxs ));
	/* U相補正 */
	OcWorkU  = OcLevel * 0x200 / ((CHAR)uCfgPrm->gadjiu_v.b.l + 0x100 );
//	OcWorkU  = (( OcWorkU + 1 ) >> 1 ) - (SHORT)uCfgPrm->zadjiu * 4;								  /* <S1FC> */
	OcWorkU  = (( OcWorkU + 1 ) >> 1 );
	OcMaxU = 0x00008000 + OcWorkU - ((SHORT)uCfgPrm->zadjiu * 4);									  /* <S1FC> */
	if( OcMaxU > 0x0000FFFF ) OcMaxU = 0x0000FFFF;
	OcMinU = 0x00008000 - OcWorkU - ((SHORT)uCfgPrm->zadjiu * 4);									  /* <S1FC> */
	if( OcMinU < 0x00000000 ) OcMinU = 0x00000000;
	pAsicHwReg->AREG_OCLVUP = OcMaxU;
	pAsicHwReg->AREG_OCLVUM = OcMinU;
	/* V相補正 */
	OcWorkV  = OcLevel * 0x200 / ((CHAR)uCfgPrm->gadjiu_v.b.h + 0x100 );
//	OcWorkV  = (( OcWorkV + 1 ) >> 1 ) - (SHORT)uCfgPrm->zadjiv * 4;								  /* <S1FC> */
	OcWorkV  = (( OcWorkV + 1 ) >> 1 );
	OcMaxV = 0x00008000 + OcWorkV - ((SHORT)uCfgPrm->zadjiv * 4);									  /* <S1FC> */
	if( OcMaxV > 0x0000FFFF ) OcMaxV = 0x0000FFFF;
	OcMinV = 0x00008000 - OcWorkV - ((SHORT)uCfgPrm->zadjiv * 4);									  /* <S1FC> */
	if( OcMinV < 0x00000000 ) OcMinV = 0x00000000;
	pAsicHwReg->AREG_OCLVVP = OcMaxV;
	pAsicHwReg->AREG_OCLVVM = OcMinV;
	/* W相補正 */ /* <S1FC> */
	OcWorkW = ( OcWorkU > OcWorkV )? OcWorkU : OcWorkV;
	OcMaxW = 0x00008000 + OcWorkW + ((SHORT)uCfgPrm->zadjiu * 4) + ((SHORT)uCfgPrm->zadjiv * 4);
	if( OcMaxW > 0x0000FFFF ) OcMaxW = 0x0000FFFF;
	OcMinW = 0x00008000 - OcWorkW + ((SHORT)uCfgPrm->zadjiu * 4) + ((SHORT)uCfgPrm->zadjiv * 4);
	if( OcMinW < 0x00000000 ) OcMinW = 0x00000000;
	pAsicHwReg->AREG_OCLVWP = OcMaxW;
	pAsicHwReg->AREG_OCLVWM = OcMinW;
//	pAsicHwReg->AREG_OCLVWP = ( OcMaxU > OcMaxV )? OcMaxU : OcMaxV ;								  /* <S1FC> */
//	pAsicHwReg->AREG_OCLVWM = ( OcMinU < OcMinV )? OcMinU : OcMinV ;								  /* <S1FC> */
/*--------------------------------------------------------------------------------------------------*/
/*		モータ過電流検出フィルタ回数																*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_FLTRANGE = uCfgPrm->fltrange;
/*--------------------------------------------------------------------------------------------------*/
/*		OC検出有効																					*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_SETMSK		&= 0xFFF0;			/* OC検出有効									*/
	pAsicHwReg->AREG_SETREG1	= 0x00000005;		/* ASIC Write Enable							*/
	pAsicHwReg->AREG_CSTP		&= ~( 1 << 26 );	/* OC検出有効									*/
	pAsicHwReg->AREG_SETREG1	= 0x00000001;		/* ASIC Write Disable							*/
/*--------------------------------------------------------------------------------------------------*/
/*		OC検出用ADサイクル同期																		*/
/*--------------------------------------------------------------------------------------------------*/
	pAsicHwReg->AREG_OCVADSYNC = 0x0000;
/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}
#endif	/* <S1F7> */
/****************************************************************************************************/
/*																									*/
/*		電流換算ゲインの計算																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  10000 * 2^9																			*/
/*		Kc = -------------																			*/
/*			    Iadmax																				*/
/*																									*/
/*		Iadmax [-] : 最大電流時のＡＤＣ値															*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqAcrKc( LONG Iadmax, USHORT imax, USHORT imaxs )
{
#if (FLOAT_USE==TRUE)
	float fw;
#else
		LONG	kx,sx;
#endif /* FLOAT_USE */

	if( imax >= imaxs )
	{
#if (FLOAT_USE==TRUE)
		fw = (10000.0F * 512.0F) / (float)Iadmax;
#else
		kx = (10000 * 512) / Iadmax;
#endif /* FLOAT_USE */

	}
	else
	{
#if (FLOAT_USE==TRUE)
		fw = (512.0F * 10000.0F) / (float)Iadmax;
		fw = fw * (float)imaxs / (float)imax;
#else
		kx = MlibScalKxgain( 512, 10000,     Iadmax,   &sx,   0 );
		kx = MlibPcalKxgain( kx,  imaxs, imax, &sx, -24 );
#endif /* FLOAT_USE */
	}

#if (FLOAT_USE==TRUE)
	return( fw );
#else
	return( kx );
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		電流ループ比例ゲインの計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  2*PAI * Gain * Derate * MotLd       2^13     Imax * 512 								*/
/*		Kp = ----------------------------- * ------- * -------------								*/
/*				              100    	      Vdc/2	      15000  									*/
/*																									*/
/*		Gain  [Hz] : 電流ループゲイン																*/
/*		Derate [%] : 電流ループゲインディレーティング												*/
/*		Lx     [H] : モータインダクタンス															*/
/*																									*/
/****************************************************************************************************/
LONG	PcalDqAcrKp( LONG Gain, LONG Derate, BPRMDAT *Bprm, LONG axis_type )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

#if (FLOAT_USE==TRUE)
	fw = (float)Gain * (float)Derate;
#else
	kx = MlibScalKxgain( Gain, Derate, 1, &sx, 0 );
#endif /* FLOAT_USE */

	if(axis_type == D_AXIS)
	{ /* D軸 */
#if (FLOAT_USE==TRUE)
		fw = fw * Bprm->MotLd / Bprm->Vdc;
#else
		kx = MlibPcalKxksks( kx,   Bprm->MotLd,          Bprm->Vdc, &sx,   0 );
#endif /* FLOAT_USE */

	}
	else //if(axis_type == Q_AXIS)
	{ /* Q軸 */
#if (FLOAT_USE==TRUE)
				fw = fw * Bprm->MotLq / Bprm->Vdc;
#else
		kx = MlibPcalKxksks( kx,   Bprm->MotLq,          Bprm->Vdc, &sx,   0 );
#endif /* FLOAT_USE */
	}
#if (FLOAT_USE==TRUE)
	fw = fw * Bprm->MaxCur;
	fw = fw * 26353589.0F / 750000.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibPcalKxkskx( kx,   Bprm->MaxCur, 1,        &sx,   0 );
	rc = MlibPcalKxgain( kx,   26353589,    750000,   &sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		電流ループ積分ゲインの計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  Kp * Ts * 2^4																			*/
/*		Ki = ---------------																		*/
/*				1000 * Ti																			*/
/*																									*/
/*		Ts [ns] : 電流ループ演算周期																*/
/*		Ti [us] : 電流ループ積分時定数																*/
/*																									*/
/****************************************************************************************************/
LONG	PcalDqAcrKi( LONG Ti, LONG Kp, KSGAIN MotL, BPRMDAT *Bprm )
{
	LONG	kx,sx;
	LONG	Tx;
	LONG	MotW;
#if (FLOAT_USE==TRUE)
	float fw;
#endif
	
/*--------------------------------------------------------------------------------------------------*/
/*		電流ループ積分時定数[us]の計算(モータの電気的時定数)										*/
/*																									*/
/*				         Ld[H]		   																*/
/*		Tx[us] =  --------------- × 1000000														*/
/*				     Rx[0.001ohm]																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = MotL * 1000000.0;
	fw = fw / Bprm->MotR;
#else
	kx = MlibScalKskxkx( MotL, 1000000, 1, &sx, 0 );
	kx = MlibPcalKxkxks( kx, 1,	Bprm->MotR, &sx,   -24 );
#endif /* FLOAT_USE */

	/* リミット処理	*/
#if (FLOAT_USE==TRUE)
	Tx = MlibLimitul( (long)fw, 0xFFFF, 0 );
#else
	Tx = MlibLimitul( kx, 0xFFFF, 0 );					/* 0を最小値として使用						*/
#endif /* FLOAT_USE */

	if( Bprm->CurrentLoopTiSw == TRUE )
	{	/* モータ機種に関係なく、モータ電気的積分時定数を設定 */
		Ti = Tx;
	}
	else
	{	/* 指定機種のみ、モータ電気的積分時定数を設定 */
		if( Bprm->MotTypeCode == 0x67 )					/* SGMMVモータコード							*/
		{
#if (FLOAT_USE==TRUE)
			MotW = (LONG)Bprm->MotW;
#else
			MotW = MlibScalKskxkx( Bprm->MotW, 1, 1, NULL, -30 );
#endif /* FLOAT_USE */
			switch( MotW )
			{
				case 0x0003:	/* SGMMV-B3E */
				case 0x0005:	/* SGMMV-B5E */
				case 0x000B:	/* SGMMV-B9E */
					Ti = Tx;
				break;
			
				default:
					break;
			}
		}
	}

/*--------------------------------------------------------------------------------------------------*/

	if( Ti == 0 )
	{
#if (FLOAT_USE==TRUE)
		fw = 0;
#else
		kx = 0;
#endif /* FLOAT_USE */
	}
	else
	{
#if (FLOAT_USE==TRUE)
		fw = (float)Kp * 16.0 * (float)KPI_MACYCLENS / (1000.0 * (float)Ti);
#else
		kx = MlibScalKxgain( Kp, 16*KPI_MACYCLENS, 1000*Ti, NULL, -24 );
#endif /* FLOAT_USE */
	}
#if (FLOAT_USE==TRUE)
	return( fw );
#else
	return( kx );
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		L(di/dt)のインダクタンスパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*						   Imax[A]		8192			10^9										*/
/*		L_DIDT = Le[H] × --------- × -------- × -------------- × 2^9							*/
/*							15000		Vdc/2		 scantime[ns]									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalLDidt( KSGAIN Le, KSGAIN max_cur, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

#if (FLOAT_USE==TRUE)
	fw = Le * max_cur / 15000.0F;
	fw = fw * 16384.0F * 512.0F / vdc;
	fw = fw * 1000000000.0F / (float)KPI_MBCYCLENS;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKskskx( Le,   max_cur,      15000,          &sx,   0 );
	kx = MlibPcalKxkxks( kx,   16384*512,    vdc,            &sx,   0 );
	rc = MlibPcalKxgain( kx,   1000000000,   KPI_MBCYCLENS,  &sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		電流ループフィルタゲインの計算																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			    Ts * 2^14																			*/
/*		Kf = -----------------																		*/
/*			  (1000*Tf) + Ts																		*/
/*																									*/
/*		Ts [ns] : 電流ループ演算周期																*/
/*		Tf [us] : 電流ループフィルタ時定数															*/
/*																									*/
/****************************************************************************************************/
LONG	PcalDqAcrFilter( LONG Tf )
{
#if (FLOAT_USE==TRUE) 
	float fw;

	fw = (float)KPI_MACYCLENS * 16384.0 / ((1000.0*(float)Tf) + (float)KPI_MACYCLENS);

	return( fw );
#else
	LONG	kx;

	kx = MlibScalKxgain( KPI_MACYCLENS, 16384, ((1000*Tf) + KPI_MACYCLENS), NULL, -24 );

	return( kx );
#endif /* FLOAT_USE */
}



/****************************************************************************************************/
/*																									*/
/*		インダクタンス補償定数(Ld,Lq)の計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  2^15 * 2*PAI*Nos     2^15 * Imax      Lx * 2^13										*/
/*		kx = ------------------ * ------------- * --------------		--- SGDS					*/
/*			     60 * 15000        15000 * 10      10^5 * Vdc/2										*/
/*																									*/
/*																									*/
/*			  2^15 * Nmax     2^15 * Imax     Lx * 2^13     PoleNo									*/
/*		kx = ------------- * ------------- * ----------- * --------		--- SVOS					*/
/*				 15000           15000          Vdc/2         2										*/
/*																									*/
/*																									*/
/*		Nmax [rad/s] : Max. Speed																	*/
/*		Imax [A]     : Max. Current																	*/
/*		Lx   [H]     : Inductance																	*/
/*		Vdc  [V]     : DC Voltage																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqLxComp( KSGAIN Lx, KSGAIN over_spd, KSGAIN max_cur, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

/*--------------------------------------------------------------------------------------------------*/
/*		インダクタンス補償定数(Ld,Lq)の計算															*/
/*--------------------------------------------------------------------------------------------------*/
/*		回転型：Bprm.Keangle = PoleNo / 2		: Motor[rad] ---> Eangle[rad]						*/
/*		リニア：Bprm.Keangle = PAI / PolePitch	: Motor[ m ] ---> Eangle[rad]						*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = over_spd * max_cur / vdc;
	fw = fw * Lx;
	fw = fw * 78187.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKsksks( over_spd,	  max_cur,  	vdc,   &sx,   0 );
	kx = MlibPcalKxkskx( kx,          Lx,           1,     &sx,   0 );
	rc = MlibPcalKxgain( kx,          78187,        1,     &sx, -24 );
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		電機子抵抗(R1)補償定数の計算																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   2^15 * Rx      2^13     Imax															*/
/*		kx = ------------ * ------- * -------														*/
/*				   1         Vdc/2     15000														*/
/*																									*/
/*																									*/
/*		Rx   [ohm] : Resistance																		*/
/*		Imax [A]   : Max. Current																	*/
/*		Vdc  [V]   : DC Voltage																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqRxComp( KSGAIN Rx, KSGAIN max_cur, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;
#if (FLOAT_USE==TRUE)
	fw = Rx * 35791.0F;
	fw = fw * max_cur / vdc;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKskxkx( Rx, 35791,       1,        &sx,   0 );
	rc = MlibPcalKxksks( kx, max_cur,	  vdc,		&sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}



/****************************************************************************************************/
/*																									*/
/*		ＥＭＦ補償定数(Φ)の計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   Nmax * 1.4142 * Emf * 2^15      2^13													*/
/*		kx = ------------------------------ * -------												*/
/*			               15000               Vdc/2												*/
/*																									*/
/*		Nmax [rad/s]      : Max. Speed																*/
/*		Emf  [Vrms/rad/s] : EMF Constant															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalDqEmfComp( KSGAIN Emf, KSGAIN over_spd, KSGAIN vdc )
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG	kx, sx;
#endif /* FLOAT_USE */
	LONG	rc;

#if (FLOAT_USE==TRUE)
	fw = Emf * over_spd / vdc;
	fw = fw * 50617.0F;
	if( fw > (float)0x01000000 )
	{
		rc = 0x01000000;
	}
	else
	{
		rc = (LONG)fw;
	}
#else
	kx = MlibScalKsksks( Emf, over_spd,    vdc,		&sx,   0 );
	rc = MlibPcalKxgain( kx,  50617,       1,       &sx, -24 );
#endif /* FLOAT_USE */

	return( rc );
}




/****************************************************************************************************/
/*																									*/
/*		ＡＳＩＣマイクロ用電流ゲインパラメータ計算													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/****************************************************************************************************/
void	PcalMicroDqAcrGain(	ASIC_U_IF_STR	*pAsicMicroReg,	/* 制御変数ポインタ						*/
							DQACR_GAIN_PRM	*Prm,			/* パラメータデータポインタ				*/
							BPRMDAT			*Bprm,			/* ベースパラメータポインタ 			*/
							ULONG			Loophz,			/* 速度ループゲイン			[0.1Hz]		*/
							ULONG			ratio)			/* ゲインバランス			[%]			*/
{

	LONG	kp, ki;
	LONG	kx, sx;
	LONG	Kxd, Kxq, Txd, Txq;
	LONG	kfil, Tfx;
	LONG	MinKpGain;
#if (FLOAT_USE==TRUE)
	float Fwd, Fwq, fw;
#endif /* FLOAT_USE */

	MinKpGain = 900;							/* 電流ループゲイン最小値	[Hz]					*/

/*--------------------------------------------------------------------------------------------------*/
/*	電流ゲイン連動実行判定																			*/
/*--------------------------------------------------------------------------------------------------*/
	if( ratio >= 2000 )
	{	/* 比率が2000%以上の場合は、電流ゲイン連動無効	*/
		/* または、トルク制御設定は無効					*/
		Kxd = Prm->ignd;
		Kxq = Prm->ignq;
		Txd = Prm->kid;
		Txq = Prm->kiq;
		Tfx = Prm->ifil;
	}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*		電流ゲイン[Hz]の計算																			*/
/*																									*/
/*				    Kv[0.1Hz]           ratio[%]													*/
/*		Kxd[Hz] =  ----------- × 4 × ---------- 													*/
/*				       10                 100														*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		Fwd = (float)Loophz * 4 * (float)ratio /1000;
		Fwq = Fwd;

		/*	リミット処理 (パラメータを最大値として使用する)	*/
		if( Fwd < (float)MinKpGain )
		{
			Fwd = (float)MinKpGain;
		}
		if( Fwd > (float)Prm->ignd )
		{ /* 最大値＜最小値の場合は最大値にクランプ */
			Fwd = (float)Prm->ignd;
		}
		if( Fwq < (float)MinKpGain )
		{
			Fwq = (float)MinKpGain;
		}
		if( Fwq > (float)Prm->ignq )
		{
			Fwq =(float) Prm->ignq;
		}

#else
		Kxd = MlibScalKxgain( (LONG)Loophz,   (4 * (LONG)ratio),   1000,  NULL, -24 );
		Kxq = Kxd;


		/*	リミット処理 (パラメータを最大値として使用する)	*/
		if( Kxd < MinKpGain )
		{
			Kxd = MinKpGain;
		}
		if( Kxd > (LONG)Prm->ignd )
		{ /* 最大値＜最小値の場合は最大値にクランプ */
			Kxd = Prm->ignd;
		}
		if( Kxq < MinKpGain )
		{
			Kxq = MinKpGain;
		}
		if( Kxq > (LONG)Prm->ignq )
		{
			Kxq = Prm->ignq;
		}
#endif /* FLOAT_USE */
		/* 電流ループ積分時定数連動なし	*//*<V242>*/
		Txd = Prm->kid;
		Txq = Prm->kiq;

/*--------------------------------------------------------------------------------------------------*/
/*		電圧フィルタ時定数[us]の計算																*/
/*																									*/
/*			電流ゲイン最小値でPnEA4になり、電流ゲイン最大値(PnE21)でフィルタ時定数=0となる傾き		*/
/*																									*/
/*																									*/
/*				            Prm.ifil2[us]		   													*/
/*		Tfx[us] =  --------------------------------- × (Kxq[Hz] - MinKpGain[Hz]) + Prm.ifil2[us]	*/
/*				     (MinKpGain[Hz] - Prm.ignq)[Hz])												*/
/*--------------------------------------------------------------------------------------------------*/
		if( MinKpGain != Prm->ignq )
		{
#if (FLOAT_USE==TRUE)
			fw = (float)Prm->ifil2 * ((float)Kxq - (float)MinKpGain) / ((float)MinKpGain - (float)Prm->ignq);
			(float)Tfx = fw + (float)Prm->ifil2;
#else
			kx = MlibScalKxgain( Prm->ifil2, (Kxq - MinKpGain), (MinKpGain - Prm->ignq), &sx, -24 );
			Tfx = kx + Prm->ifil2;
#endif /* FLOAT_USE */
		}
		else
		{
			Tfx = Prm->ifil2;
		}
		if( Tfx < 0 )
		{
			Tfx = 0;
		}
		if( Tfx > Prm->ifil2 )
		{
			Tfx = Prm->ifil2;
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算																				*/
/*--------------------------------------------------------------------------------------------------*/
#ifndef _JL076_USE_
	/*----------------------------------------------------------------------------------------------*/
	/*		d軸電流ゲイン																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* 比例ゲイン */
	kp = PcalDqAcrKp( Kxd, 100, Bprm, D_AXIS );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KdP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* 積分ゲイン */
	ki = PcalDqAcrKi( Txd, kp, Bprm->MotLd, Bprm );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KdI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		q軸電流ゲイン																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* 比例ゲイン */
	kp = PcalDqAcrKp( Kxq, 100, Bprm, Q_AXIS );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KqP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* 積分ゲイン */
	ki = PcalDqAcrKi( Txq, kp, Bprm->MotLq, Bprm );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.KqI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		電流ループ出力フィルタ時定数															*/
	/*----------------------------------------------------------------------------------------------*/
	/* 出力フィルタゲイン */
	kfil = PcalDqAcrFilter( Tfx );
	LpxSetPrmToASIC( &pAsicMicroReg->IntAdP.Tfil, kfil, 1 );

#else /* _JL076_USE_ */

	/*----------------------------------------------------------------------------------------------*/
	/*		d軸電流ゲイン																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* 比例ゲイン */
	kp = PcalDqAcrKp( Kxd, 100, Bprm, D_AXIS );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KDP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* 積分ゲイン */
	ki = PcalDqAcrKi( Txd, kp, Bprm->MotLd, Bprm );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KDI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		q軸電流ゲイン																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* 比例ゲイン */
	kp = PcalDqAcrKp( Kxq, 100, Bprm, Q_AXIS );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KQP, kp, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/* 積分ゲイン */
	ki = PcalDqAcrKi( Txq, kp, Bprm->MotLq, Bprm );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_KQI, ki, 1 );

	/*----------------------------------------------------------------------------------------------*/
	/*		電流ループ出力フィルタ時定数															*/
	/*----------------------------------------------------------------------------------------------*/
	/* 出力フィルタゲイン */
	kfil = PcalDqAcrFilter( Tfx );
	LpxSetPrmToASIC( (SHORT*)&pAsicMicroReg->MREG_TFIL, kfil, 1 );

#endif /* _JL076_USE_ */
 
}



/****************************************************************************************************/
/*																									*/
/*		オンディレイレベルの計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   Level * 15000																		*/
/*		kx = -----------------		→		= limit( Level *150 ) :0 ～15000でリミット				*/
/*			       Imax       	 																	*/
/*																									*/
/*		Level [%] : オンディレイ切り替えレベル														*/
/*		Imax  [%] : 最大電流[%] (=最大トルク[%])													*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalOndelayLevel( LONG Level )
{
	LONG	kx;

	kx = MlibLimitul( (Level * 150), 15000, 0 );

	return( kx );
}



/****************************************************************************************************/
/*																									*/
/*		オンディレイ補償台形傾きの計算																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			     2^8 * 2^13 	     2^8 * 2^13 * 1000	   139810   								*/
/*		kx = -------------------- = ------------------- = -------- 									*/
/*			  15000 * Slope/1000       15000 * Slope        Slope  									*/
/*																									*/
/*		Odslp [0.1%] : 補償が100%となる電流															*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalOndelaySlope( LONG slope )
{
#if (FLOAT_USE==TRUE)
	float fw;

	if( slope != 0 )
	{
		fw = 139810.0F / (float)slope;
	}
	else
	{
		fw = 139810.0F;
	}

	return( fw );

#else
	LONG	kx;

	if( slope != 0 )
	{
		kx = 139810 / slope;
	}
	else
	{
		kx = 139810;
	}

	return( kx );

#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		ＰＷＭキャリア周波数変更サービス		<V356>												*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : PWM周波数をオンラインで変更する															*/
/*			引数にPnE2C設定下限値未満が指定された場合は、「PnF1E bit2：キャリア周波数選択」			*/
/*			に従って設定する																		*/
/*			PnE2C設定下限値以上の場合は、キャリア周波数を設定に変更する								*/
/*			※変更したキャリア周波数を元に戻す時は引数に”０”を設定する							*/
/*																									*/
/****************************************************************************************************/
#define		PWM_FREQ_LOWER_LIMIT		2000					/* キャリア周波数下限値				*/
/****************************************************************************************************/
LONG	KpiChangePwmFrequency( ULONG PwmFx, USHORT pwmf, USHORT pwmf3, USHORT flg_wf )
{
	LONG	Tc;

	if( PwmFx < PWM_FREQ_LOWER_LIMIT )
	{ /* パラメータ下限未満 */
		if( (flg_wf & 0x0002 )							/* 第2キャリア周波数選択フラグ		*/
			&& (pwmf3 >= PWM_FREQ_LOWER_LIMIT)	)		/* 第2キャリア周波数設定あり		*/
		{
			Tc = PcalPwmFrequency( pwmf3 );				/* 第2キャリア周波数設定			*/
		}
		else
		{
			Tc = PcalPwmFrequency( pwmf );				/* 第1キャリア周波数設定			*/
		}
	}
	else
	{ /* 指定周波数あり */
		Tc = PcalPwmFrequency( PwmFx );					/* 指定キャリア周波数設定			*/
	}

	return( Tc );

}



/****************************************************************************************************/
/*																									*/
/*		ＰＷＭキャリア周波数カウント値の計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   40000000																				*/
/*		Tc = -----------																			*/
/*				  Fc																				*/
/*																									*/
/*		Fc [Hz] : ＰＷＭ周波数																		*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalPwmFrequency( LONG PwmFx )
{
	LONG	Tc;

	switch ( PwmFx )
	{
#if 0 /* 暫定的に80MHz設定(上が正規) */
		case 10667 : Tc = 15000; break;						/* PwmFc = 10.666666... [kHz]			*/
		case  8000 : Tc = 20000; break;						/* PwmFc =  8.0 [kHz]					*/
		case  4000 : Tc = 40000; break;						/* PwmFc =  4.0 [kHz]					*/
#else
		case 10667 : Tc = 3750; break;						/* PwmFc = 10.666666... [kHz]			*//*<V324>*/
		case  8000 : Tc = 5000; break;						/* PwmFc =  8.0 [kHz]					*/
		case  5333 : Tc = 7500; break;						/* PwmFc =  5.333333... [kHz]			*//*<V324>*/
		case  4000 : Tc = 10000; break;					/* PwmFc =  4.0 [kHz]					*/
		case  3556 : Tc = 11250; break;					/* PwmFc =  3.555555... [kHz]			*//*<V324>*/
#endif
		case	 0 : Tc = (ASIC_CLKMHZ/2) * 1000000; break;	/* for escape 0div						*/
		default    : Tc = (ASIC_CLKMHZ/2) * 1000000 / PwmFx; break;	/* PwmFc = Other				*/
	}

	return( Tc );

}



/****************************************************************************************************/
/*																									*/
/*		ＰＷＭキャリア周波数の計算						モータキャリア周波数切り替え<S117>						*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			   40000000																				*/
/*		Fc = -----------																			*/
/*				  Tc																				*/
/*																									*/
/*		Tc [-] : キャリアカウント値																	*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalPwmCounter( LONG CarrCnt )
{
	LONG	Fc;

	switch ( CarrCnt )
	{
#if 0 /* 暫定的に80MHz設定(上が正規) */
		case 15000 : Tc = 10667; break;						/* PwmFc = 10.666666... [kHz]			*/
		case 20000 : Tc =  8000; break;						/* PwmFc =  8.0 [kHz]					*/
		case 40000 : Tc =  4000; break;						/* PwmFc =  4.0 [kHz]					*/
#else
		case  3750 : Fc = 10667; break;						/* PwmFc = 10.666666... [kHz]			*//*<V324>*/
		case  5000 : Fc = 8000; break;							/* PwmFc =  8.0 [kHz]					*/
		case  7500 : Fc = 5333; break;							/* PwmFc =  5.333333... [kHz]			*//*<V324>*/
		case 10000 : Fc = 4000; break;							/* PwmFc =  4.0 [kHz]					*/
		case 11250 : Fc = 3556; break;							/* PwmFc =  3.555555... [kHz]			*//*<V324>*/
#endif
		case	 0 : Fc = (ASIC_CLKMHZ/2) * 1000000; break;	/* for escape 0div						*/
		default    : Fc = (ASIC_CLKMHZ/2) * 1000000 / CarrCnt; break;	/* PwmFc = Other				*/
	}

	return( Fc );

}



/****************************************************************************************************/
/*																									*/
/*		電圧フィードバックループ積分リミットの計算													*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*							 15000																	*/
/*		TvLim = Prm.TvLim * -------																	*/
/*							  100																	*/
/*																									*/
/*		Prm.TvLim [%] : 電圧フィードバックループ積分時定数パラメータ (PnEB2)						*/
/*																									*/
/****************************************************************************************************/
static	LONG	PcalVFBKiLim( LONG TvLim )
{
	LONG	kx;

	kx = TvLim * 150;

	return( kx );
}



/* 2012.10.26 Y.Oka ★電流オブザーバ現状未対応⇒パラメータ一括書き込み対応要★ */
/****************************************************************************************************/
/*																									*/
/*		電流オブザーバパラメータ計算																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		オブザーバゲイン1	: Ts/L																	*/
/*				   scantime[ns]		     1	          Bprm.Vdc[Vop]/2          15000     			*/
/*		wk_TSPL = -------------- * --------------- * ----------------- * ------------------ * 2^7	*/
/*					  10^9			Bprm.MotR[Ω]         2^13            Bprm.MaxCur[Aop]			*/
/*																									*/
/*		オブザーバゲイン2	: 1 - R*L/Ts - exp( -2*pi*Kpi )											*/
/*                       Bprm.MotR[Ω]     scantime[ns]												*/
/*		wk_GOBS = 2^8 - --------------- * -------------- * 2^8										*/
/*                       Bprm.MotLq[H]        10^9													*/
/*																									*/
/*                                                  scantime[ns]									*/
/*					- 2^8 * exp( -2*pi * Kpi[Hz] * -------------- )									*/
/*                                                      10^9										*/
/*																									*/
/*		オブザーバゲイン3	: 1 - R*L/Ts															*/
/*                       Bprm.MotR[Ω]     scantime[ns]												*/
/*		wk_GOBS = 2^4 - --------------- * -------------- * 2^4										*/
/*                       Bprm.MotLq[H]        10^9													*/
/*																									*/
/*		フィルタゲイン		: Ts / ( Ts + Tfil )													*/
/*		                       scantime[ns]															*/
/*		wk_FILOBS = ----------------------------------- * 16384										*/
/*			                               10^9														*/
/*					 scantime[ns] + -----------------												*/
/*					                 2*pi * Pwmf[Hz]												*/
/*																									*/
/*		scantime( KPI_MACYCLENS )	: 電流制御周期[ns]												*/
/*		Bprm.MotLq					: q軸モータインダクタンス[H]									*/
/*		Bprm.MotR					: q軸モータ抵抗値[Ω]											*/
/*		Bprm.MaxCur					: 最大電流[Aop]													*/
/*		Bprm.Vdc					: 最大電圧[Vop]													*/
/*		Kpi							: q軸電流ループゲイン[Hz]	( PnE20 : Prm.ignq )				*/
/*		Pwmf						: キャリア周波数[Hz]		( PnE2C : Prm.pwmf )				*/
/****************************************************************************************************/
static	void	KpiPcalCurrentObs( ASICS *SvAsicRegs, USHORT ignq, USHORT pwmf, BPRMDAT *Bprm, SHORT *MpReg)
{
	LONG	n0,n1,n2;							/* bitシフト量										*/
	LONG	kx, sx;								/* work用変数										*/
	LONG	kx2, sx2;							/* work用変数2										*/
	LONG	kx3, sx3;							/* work用変数3										*/
	LONG	kx4, sx4;							/* work用変数4										*/
	LONG	MaxCur;								/* 最大電流[Aop]									*/
	LONG	Vdc;								/* 最大電圧[Vop]									*/
	LONG	MotLq;								/* q軸モータインダクタンス[H]						*/
	LONG	MotR;								/* q軸モータ抵抗値[Ω]								*/
	USHORT	wk_TSPL;							/* TSPL work用変数									*/
	USHORT	wk_GOBS;							/* GOBS work用変数									*/
	USHORT	wk_RLTS;							/* RLTS work用変数									*/
	USHORT	wk_FILOBS;							/* FILOBS work用変数								*/

	n0 = 1 << (7+8);		/* 2^(7+2) */
	n1 = 1 << (8+8);		/* 2^(8+2) */
	n2 = 1 << (4+8);		/* 2^(4+2) */
	MaxCur	= Bprm->MaxCur;


	Vdc		= Bprm->Vdc;
	MotLq	= Bprm->MotLq;
	MotR	= Bprm->MotR;

	/* TSPLの計算	: Ts/L */
	kx		= MlibScalKxgain( KPI_MACYCLENS,	15,			2,				&sx,	0	);
	kx		= MlibPcalKxgain( kx,				n0,			8192,			&sx,	0	);
	kx		= MlibPcalKxkskx( kx,				Vdc,		1000000,		&sx,	0	);
	kx		= MlibPcalKxkxks( kx,				1,			MotLq,			&sx,	0	);
	wk_TSPL = (USHORT)MlibPcalKxkxks( kx,		1,			MaxCur,			&sx,	-24	);

	/* GOBSの計算	: 1 - R*Ts/L - g = 1 - R*Ts/L - exp( -2*pi*Ts*w ) */
	/* ( g = 1 - x + x^2/2! - x^3/3! + x^4/4! ) */
	/* x : 2*pi*Ts*w */
	/* R*Ts/L */
	kx		= MlibScalKxksks( n1,		MotR,				MotLq,			&sx,	0	);
	kx		= MlibPcalKxgain( kx,		KPI_MACYCLENS,		1000000000,		&sx,	0	);

	/* 1 */
	kx2		= MlibScalKxgain( n1,		1,					1,				&sx2,	0	);

	/* 1 - R*Ts/L */
	kx2		= MlibPcalKxsubx( kx2,		sx2,	kx,		sx,		&sx2 );

	/*  exp( -2*pi*Ts*w ) : 4次のテーラー展開で近似 */
	/* x : 2*pi*Ts*w の計算 */
	kx		= MlibScalKxgain( 62831853,		ignq,	10000000,		&sx,	0	);
	kx		= MlibPcalKxgain( kx,			KPI_MACYCLENS,	1000000000, 	&sx,	0	);
	kx3		= kx;
	sx3		= sx;
	/* exp() 第1項 : 1 */
	kx4		= MlibScalKxgain( 1,		1,		1,		&sx4,	0 );
	/* exp() ( 第1項 - 第2項 ) : 1 - x */
	kx4		= MlibPcalKxsubx( kx4,		sx4,	kx, 	sx,	&sx4	);
	/* exp() 第3項 : x^2/2! */
	sx		+= sx ;
	kx		= MlibPcalKxgain( kx,	kx,	2,		&sx,	0	);
	/* exp() ( 第1項 - 第2項 + 第3項 ) : 1 - x + x^2/2! */
	kx4		= MlibPcalKxaddx( kx4,		sx4,	kx,		sx,  &sx4	);
	/* exp() 第4項 : x^3/3! */
	sx		+= sx3;
	kx		= MlibPcalKxgain( kx,	kx3,	3,		&sx,	0	);
	/* exp() ( 第1項 - 第2項 + 第3項 - 第4項 ) : 1 - x + x^2/2! - x^3/3! */
	kx4		= MlibPcalKxsubx( kx4,		sx4,	kx,		sx,  &sx4	);
	/* exp() 第5項 : x^4/4! */
	sx		+= sx3;
	kx		= MlibPcalKxgain( kx,	kx3,	4,		&sx,	0	);
	/* exp() ( 第1項 - 第2項 + 第3項 - 第4項 + 第5項 ) : 1 - x + x^2/2! - x^3/3! + x^4/4! */
	kx4		= MlibPcalKxaddx( kx4,		sx4,	kx,		sx,  &sx4	);
	/*  exp( -2*pi*Ts*w )*2^8  */
	kx4		= MlibPcalKxgain( kx4,	n1,	1,		&sx4,	0	);
	/* 1 - R*Ts/L - exp( -2*pi*Ts*w ) */
	kx2		= MlibPcalKxsubx( kx2,		sx2,	kx4,	sx4,	&sx2 );
	wk_GOBS	= (USHORT)MlibPcalKxgain( kx2,		1,		1,		&sx2,	-24	 );

	/* RLTSの計算	: 1 - R*Ts/L */
	/* R*Ts/L */
	kx		= MlibScalKxksks( n2,		MotR,				MotLq,			&sx,	0	);
	kx		= MlibPcalKxgain( kx,		KPI_MACYCLENS,		1000000000,		&sx,	0	);
	/* 1 */
	kx2		= MlibScalKxgain( n2,		1,					1,				&sx2,	0	);
	/* 1 - R*Ts/L */
	kx2		= MlibPcalKxsubx( kx2,		sx2,	kx,		sx,		&sx2 );
	wk_RLTS	= (USHORT)MlibPcalKxgain( kx2,		1,		1,		&sx2,	-24	 );

	/* FILOBSの計算	: Ts/(Tf+Ts) */
	/* Tf : 10^6 / ( PWMF*2*pi ) */
	kx		= MlibScalKxgain( 1000000,	1,			pwmf,		&sx,	0	);
	kx		= MlibPcalKxgain( kx,		10000000,	62831853,	&sx,	0	);
	kx		= MlibPcalKxgain( kx,		1000,		1,			&sx,	-24	);
	/* Ts/(Ts+Tf) */
	wk_FILOBS	= (USHORT)MlibPcalKxgain( KPI_MACYCLENS,	16384,	KPI_MACYCLENS+kx,	&sx,	-24	);


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
	if( ( wk_GOBS == 0 ) || ( wk_TSPL == 0 ) || ( wk_RLTS == 0 ) )
	{
		tbuf[0] = 0;											/* TSPL 			*/
		tbuf[1] = 0;											/* GOBS 			*/
		tbuf[2] = 0;											/* RLTS 			*/
		tbuf[3] = wk_FILOBS;									/* FILOBS 			*/
		MicroTranslatePrmReq( SvAsicRegs, tbuf, MpReg, 4 );
	}
	else
	{
		tbuf[0] = wk_TSPL;										/* TSPL 			*/
		tbuf[1] = wk_GOBS;										/* GOBS 			*/
		tbuf[2] = wk_RLTS;										/* RLTS 			*/
		tbuf[3] = wk_FILOBS;									/* FILOBS 			*/
		MicroTranslatePrmReq( SvAsicRegs, tbuf, MpReg, 4 );
	}
#else
	if( ( wk_GOBS == 0 ) || ( wk_TSPL == 0 ) || ( wk_RLTS == 0 ) )
	{
		MpReg[0] = 0;											/* TSPL 			*/
		MpReg[1] = 0;											/* GOBS 			*/
		MpReg[2] = 0;											/* RLTS 			*/
		MpReg[3] = wk_FILOBS;									/* FILOBS 			*/
	}
	else
	{
		MpReg[0] = wk_TSPL;										/* TSPL 			*/
		MpReg[1] = wk_GOBS;										/* GOBS 			*/
		MpReg[2] = wk_RLTS;										/* RLTS 			*/
		MpReg[3] = wk_FILOBS;									/* FILOBS 			*/
	}
#endif

}



/****************************************************************************************************/
/*																									*/
/*		ＡＳＩＣマイクロ：トルクフィルタ管理														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 :	ＡＳＩＣマイクロプログラムのトルクフィルタのＯＮ／ＯＦＦ管理を行う。					*/
/*																									*/
/*	補 足 :	通常、パラメータ計算ルーチンからＣａｌｌされる。										*/
/*			トルクフィルタの排他制御が必要な場合は、本モジュールで対処すること。					*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void KpiManageMicroTorqueFilter( VUSHORT *mreg_csw_ptr, USHORT *CswData, USHORT FilSel, LONG FilOn )
{
	/* Get Current Status */
	*CswData = *mreg_csw_ptr;

/*--------------------------------------------------------------------------------------------------*/
/*		Set/Clear Filter Disable Flag																*/
/*--------------------------------------------------------------------------------------------------*/
	if( FilOn == TRUE )
	{
		*CswData &= ~FilSel;			/* Clear Filter Disable Flag					*/
	}
	else
	{
		*CswData |= FilSel;				/* Set   Filter Disable Flag					*/
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Set to ASIC Micro Register																	*/
/*--------------------------------------------------------------------------------------------------*/
	*mreg_csw_ptr = *CswData;

}



/****************************************************************************************************/
/*																									*/
/*		２次ノッチフィルタのパラメータ計算															*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			     2*(wx^2 - hx^2)				  hx^2 + dx*wx*hx/qx + wx^2  						*/
/*		K1 = ------------------------   	K2 = ---------------------------						*/
/*			  hx^2 + wx*hx/qx + wx^2               hx^2 + wx*hx/qx + wx^2							*/
/*																									*/
/*																									*/
/*			  hx^2 - wx*hx/qx + wx^2 			  hx^2 - dx*wx*hx/qx + wx^2							*/
/*		K3 = ------------------------   	K4 = ---------------------------						*/
/*			  hx^2 + wx*hx/qx + wx^2               hx^2 + wx*hx/qx + wx^2							*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
void	KpiPcalMicroNotchFilter2(				/* ２次ノッチフィルタのパラメータ計算				*/
		ASICS	*SvAsicRegs,					/* JL-076アクセス用構造体							*/
		LONG	hz,								/* フィルタ周波数		[Hz]						*/
		LONG	qx,								/* フィルタＱ定数		[0.01]						*/
		LONG	dx,								/* フィルタ深さ			[0.001]						*/
		SHORT	*MpReg	)						/* フィルタパラメータ設定先頭アドレス				*/
{
	LONG	ts;
	LONG	hx;
	LONG	wx;
	LONG	cx[3];
	LONG	freq;
	USHORT	kf[4];

/*--------------------------------------------------------------------------------------------------*/
/*		パラメータ計算																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( qx < 50 )
	{ /* Lower Limit for qx */
		qx = 50;
	}
	if( dx > 1000 )
	{ /* Upper Limit for dx */
		dx = 1000;
	}
	/*----------------------------------------------------------------------------------------------*/
	/* フィルタ演算周期設定 */
	/*----------------------------------------------------------------------------------------------*/
	ts = KPI_MBCYCLENS/100;
	/*----------------------------------------------------------------------------------------------*/
	/* 周波数補正																					*/
	/*----------------------------------------------------------------------------------------------*/
	if ( hz > 2000 )
	{
		freq = (LONG)( (SHORT)hz * (SHORT)(3 * hz - 1894) + 11788000) / 10000;
	}
	else
	{
		freq = hz;
	}
	/*----------------------------------------------------------------------------------------------*/
	hx = 10000000 / ts;											/* hx = (2/ts)*(1/2)				*/
	wx = (freq * 12868) >> 12;									/* wx = (hz*2*PAI)*(1/2)			*/
	/*----------------------------------------------------------------------------------------------*/
	while( hx > 23170 )
	{ /* Scaling : hx^2 < 2^29 */
		hx = hx>>1; wx = wx>>1;
	}
	while( wx > 23170 )
	{ /* Scaling : wx^2 < 2^29 */
		hx = hx>>1; wx = wx>>1;
	}
	/*----------------------------------------------------------------------------------------------*/
	cx[0] = MlibScalKxgain( 100*wx, hx, qx,   NULL, -30 );		/* cx[0] = wx*hx/qx					*/
	cx[1] = MlibScalKxgain( cx[0],  dx, 1000, NULL, -30 );		/* cx[1] = dx*wx*hx/qx				*/
	cx[2] = hx*hx + cx[0] + wx*wx;								/* cx[2] = hx*hx + wx*hx/qx + wx*wx	*/
	/*----------------------------------------------------------------------------------------------*/
	kf[0] = (USHORT)MlibScalKxgain( 2*(wx*wx - hx*hx),       (1<<13), cx[2], NULL, -24 );	/* K1	*/
	kf[1] = (USHORT)MlibScalKxgain( (hx*hx + cx[1] + wx*wx), (1<<13), cx[2], NULL, -24 );	/* K2	*/
	kf[2] = (USHORT)MlibScalKxgain( (hx*hx - cx[0] + wx*wx), (1<<13), cx[2], NULL, -24 );	/* K3	*/
	kf[3] = (USHORT)MlibScalKxgain( (hx*hx - cx[1] + wx*wx), (1<<13), cx[2], NULL, -24 );	/* K4	*/
/*--------------------------------------------------------------------------------------------------*/
/*		マイクロＩＦレジスタへの書き込み															*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2012.09.04 Y.Oka ★μプログラムへのパラメータ一括書き込み機能追加必要★ */
	MicroTranslatePrmReq( SvAsicRegs, kf, MpReg, 4 );
#else /* 暫定対応 */
	MpReg[0]	= (USHORT)kf[0];						/* Write k[0] to ASIC Micro Register		*/
	MpReg[1]	= (USHORT)kf[1];						/* Write k[1] to ASIC Micro Register		*/
	MpReg[2]	= (USHORT)kf[2];						/* Write k[2] to ASIC Micro Register		*/
	MpReg[3]	= (USHORT)kf[3];						/* Write k[3] to ASIC Micro Register		*/
#endif
/*--------------------------------------------------------------------------------------------------*/

}



/****************************************************************************************************/
/*																									*/
/*		１次ローパスフィルタのパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*				Ts  																				*/
/*		kf = --------- * 2^14																		*/
/*			  Tf + Ts   																			*/
/*																									*/
/*																									*/
/****************************************************************************************************/
static	void	KpiPcalMicroLpassFilter1(		/* １次ローパスフィルタのパラメータ計算				*/
		LONG	tf,								/* フィルタ時定数		[xs]						*/
		LONG	ts,								/* フィルタ演算周期		[xs]						*/
		SHORT	*MpReg )						/* フィルタパラメータ設定先頭アドレス				*/
{
	LONG	kf;

	kf = MlibScalKxgain( ts, (1<<14), (ts + tf), NULL, -24 );
	*MpReg = (USHORT)kf;						/* Write kf to ASIC Micro Register					*/

}



/****************************************************************************************************/
/*																									*/
/*		ＡＳＩＣへのパラメータ設定(書込み)															*/
/*																									*/
/****************************************************************************************************/
LONG	LpxSetPrmToASIC( SHORT *adr, LONG prm, BOOL limchk )
{
	USHORT	rdata;
	VUSHORT	dummy;
	LONG	prmx;
	LONG	rc;

	prmx = prm;
	rc = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		Check Parameter Limit																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( limchk == REQ_LMT_CHK )
	{
		if( (prm > 32767) || (prm < 0) )				/* Limit Check								*/
		{
			prmx = MlibLimitul( prm, 32767, 0 );		/* Limit : 0 <= prm <= 32767				*/
			/* 2009.06.04 Y.Oka (>_<) パラメータリミット必要 */
//			KpiSetPrmLmtInfo( pdef );					/* パラメータリミット情報の設定				*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		Write to ASIC and Verify																	*/
/*--------------------------------------------------------------------------------------------------*/
	*adr	= (USHORT)prmx;								/* Write to ASIC Register					*/
	dummy = *adr;										/*	KPX_ASIC_DUMMY_READ( dummy );			*/
	rdata = *adr;										/* Read from ASIC Register					*/
	if( rdata != (USHORT)prmx )							/* Verify Write Data						*/
	{
		*adr	= (USHORT)prmx;							/* Write to ASIC Register					*/
		dummy = *adr;									/*	KPX_ASIC_DUMMY_READ( dummy );			*/
		rdata = *adr;									/* Read from ASIC Register					*/
		if( rdata != (USHORT)prmx )						/* Verify Write Data						*/
		{
			rc = -1;									/* A.BF2 : システムアラーム２(Micro異常)	*/
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	return(rc);

}


#if 0 /* 2012.08.31 Y.Oka ★μScanA→CpuScanA★ */
/****************************************************************************************************/
/*																									*/
/*		マイクロパラメータ一括転送要求処理															*/
/*																									*/
/*			※Sizeの最大値は32まで																	*/
/*																									*/
/****************************************************************************************************/
LONG	MicroTranslatePrmReq( ASICS *SvAsicRegs, void *Data, VUSHORT *Address, LONG Size )
{
	LONG			i;
	USHORT			*TbufAddress;
	ASIC_U_IF_STR	*pAsicMicroReg;
	ULONG			Timout;

	pAsicMicroReg = SvAsicRegs->AsicMicroReg;

	/* Check Micro Program Running Status */
	if( SvAsicRegs->MicroRunningSts == TRUE )
	{
		/* Check Request Data Size */
		if( Size > MICRO_TR_DATA_MAX )
		{
			return	MICRO_TR_DATA_ERROR;
		}

		/* Check Src Data Address and Dst Data Address */
		if( (Data == NULL) || (Address == NULL) )
		{
			return	MICRO_TR_DATA_ERROR;
		}

		/* Check and Wait Finish Last Micro Translation */
		Timout = 0;
		while( pAsicMicroReg->MREG_TR_END != MICRO_TR_READY )
		{
			/* Wait for Ready */
			KlibWaitus( KPI_SACYCLEUS );
			if((++Timout) > (KPI_SCCYCLEMS*KPI_SACOUNTMS))
			{
				return	MICRO_TR_DATA_ERROR;
			}
		}

		/* Make Target Address (Micro Address) */
		pAsicMicroReg->MREG_TADDR = (USHORT)( ((LONG)Address & MICRO_ADDRESS_CHANGE) >> 1 );

		/* Set Parameter Data */
		TbufAddress = (USHORT*)&(pAsicMicroReg->MREG_TBUF_0);
		for( i=0; i<Size; i++ )
		{
			TbufAddress[i] = ( (USHORT*)Data )[i];
		}

		/* Set Prameter Number (2byte unit) */
		pAsicMicroReg->MREG_TNUM = (USHORT)Size;

		/* Set Parameter Renewal Request (2byte unit) */
		pAsicMicroReg->MREG_TR_END = MICRO_TR_SIZE_SHORT;

		/* サーボASIC不具合No.3 対応(ダミーライト) *//* 2008.04.02 */
		pAsicMicroReg->MREG_DUMMY = 0x0000;									/* ダミーライト				*/
	}
	else
	{ /* Micro Program Stopped State */
		for( i=0; i<Size; i++ )
		{
			( (USHORT*)Address )[i] = ( (USHORT*)Data )[i];
		}
	}
	return	MICRO_TR_SUCCESS;
}
#endif



/***************************************** end of file **********************************************/
