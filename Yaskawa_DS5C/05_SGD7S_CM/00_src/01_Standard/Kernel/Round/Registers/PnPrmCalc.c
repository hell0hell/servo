/****************************************************************************************************/
/*																									*/
/*																									*/
/*		PnPrmCalc.c : パラメータ計算処理定義														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : パラメータ計算処理																		*/
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
#include "KnlApi.h"/* <S004> */
#include "MercuryGlobals.h"
#include "NetworkIf.h"
#include "ExControls.h"
#include "ModelControl.h"
#include "GainChange.h"
#include "AlarmManager.h"
#include "EepromIF.h"
#include "MLib.h"
#include "BaseControlPrmCalc.h"
#include "ExCtrlPrmCalc.h"
#include "PowerManager.h"
#include "MicroScanAIf.h"
#include "MicroPrmCalc.h"
#include "PnPrmListTbl.h"/* <V004> *//* <S00D> */
#include "FnMfcTuning.h"
#include "PosManager.h"
#include "PcmdFilter.h" /* <S065> */
#include "OverheatManager.h"
#include "BacklashComp.h"
#include "SerialEncIf.h"
#include "AnalogMonitorCalc.h"	/* <S016> */
#include "SvOptCardIf.h"
#include "RippleComp.h"		/* <S1FD> */

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
#include "RspManager.h"
#endif
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
#include "ScanN.h"
#endif


/* 2011.01.11 Y.Oka 位相補償速度オブザーバ未対応 */
//void	PcalEhSpeedObserver( EHVOBS	*EhVobs, PRMDATA *Prm, HPRMDAT *Hprm );
void	PcalOverLoadWarLevelAmpMot( AXIS_HANDLE *Axis );
//void	PcalAnalogMonitorGain( AXIS_HANDLE *Axis, LONG chno0 );		/*AnalogMonitorCalc.cに移動<S016>*/

void	PcalMotorOverrun_Linear( AXIS_HANDLE *Axis );




/****************************************************************************************************/
/*		モデル追従制御パラメータ値コピー															*/
/****************************************************************************************************/
void PcalCopyMFCCfgParam(MFC_CFG_PRM *MfcCfgPrm, const PRMDATA *Prm)
{
	MfcCfgPrm->mdlsw	= Prm->mdlsw;		/* Pn140 : モデル追従制御関連スイッチ */
	MfcCfgPrm->mdlgn	= Prm->mdlgn;		/* Pn141 : モデル追従制御ゲイン [0.1/s] */
	MfcCfgPrm->mdlzeta	= Prm->mdlzeta;		/* Pn142 : モデル追従制御ゲイン補正 */
	MfcCfgPrm->mdlptff	= Prm->mdlptff;		/* Pn143 : モデル追従制御バイアス(正転方向) */
	MfcCfgPrm->mdlntff	= Prm->mdlntff;		/* Pn144 : モデル追従制御バイアス(逆転方向) */
	MfcCfgPrm->mdlantfrq= Prm->mdlantfrq;	/* Pn145 : 振動抑制１周波数Ａ */
	MfcCfgPrm->mdlresfrq= Prm->mdlresfrq;	/* Pn146 : 振動抑制１周波数Ｂ */
	MfcCfgPrm->mdlvff	= Prm->mdlvff;		/* Pn147 : モデル追従制御速度フィードフォワード補償 */
	MfcCfgPrm->mdlgn2	= Prm->mdlgn2;		/* Pn148 : 第２モデル追従制御ゲイン [0.1/s] */
	MfcCfgPrm->mdlzeta2	= Prm->mdlzeta2;	/* Pn149 : 第２モデル追従制御ゲイン補正 */
}

/****************************************************************************************************/
/*		Ａ型制振制御パラメータ値コピー																*/
/****************************************************************************************************/
void PcalCopyResVibCfgParam(RESVIB_CFG_PRM *ResVibCfgPrm, const PRMDATA *Prm)
{
	ResVibCfgPrm->jrate 	= Prm->jrate;		/* Pn103 :　慣性モーメント比				*//*<S09D>*/
	ResVibCfgPrm->avibfrq 	= Prm->avibfrq;		/* Pn161 : Ａ型制振周波数				*/
	ResVibCfgPrm->avibgn	= Prm->avibgn;		/* Pn162 : Ａ型制振ゲイン補正			*/
	ResVibCfgPrm->avibdamp	= Prm->avibdamp;	/* Pn163 : Ａ型制振ダンピングゲイン		*/
	ResVibCfgPrm->avibfil1	= Prm->avibfil1;	/* Pn164 : Ａ型制振フィルタ時定数１補正	*/
	ResVibCfgPrm->avibfil2	= Prm->avibfil2;	/* Pn165 : Ａ型制振フィルタ時定数２補正	*/
	ResVibCfgPrm->avibdamp2	= Prm->avibdamp2;	/* Pn166 : Ａ型制振ダンピングゲイン	 <S03E>	*/
	ResVibCfgPrm->avibfrq2	= Prm->avibfrq2;	/* Pn167 : Ａ型制振フィルタ時定数２補正	 <S03E> */
}

/****************************************************************************************************/
/*		基本速度制御パラメータ値コピー																*/
/****************************************************************************************************/
void PcalCopyBaseSpeedLoopParam(BASE_SPDLOOP_PRM *CfgPrm, const PRMDATA *Prm)
{
	CfgPrm->jrate = Prm->jrate;
	CfgPrm->loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン						*/
	CfgPrm->pitime = Prm->pitime;		/* Pn101 : 速度ループ積分時定数					*/
	CfgPrm->loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン					*/
	CfgPrm->pitime2 = Prm->pitime2;		/* Pn105 : 第２速度ループ積分時定数				*/
	CfgPrm->ipg_rwt = Prm->ipg_rwt;		/* PnE44 : 下位:i-P制御割合, 上位:暴走検出トルク*/
}




#if 0 	/* 2011.01.11 Y.Oka 位相補償速度オブザーバ未対応 */
/* 位相補償オブザーバ演算サイクルタイム(速度ループと同一)[us] */
#define	EHSOBSCYCLE			KPI_MBCYCLENS
/****************************************************************************************************/
/*																									*/
/*		位相補償速度オブザーバパラメータ計算														*/
/*																									*/
/****************************************************************************************************/
void	PcalEhSpeedObserver( EHVOBS	*EhVobs, PRMDATA *Prm, HPRMDAT *Hprm )
{
	LONG	Kstf1;							/* 位相補償VOBSゲイン1（Pn127）							*/
	LONG	Kstf2;							/* 位相補償VOBSゲイン2（Pn127）							*/
	LONG	Kstf3;							/* 位相補償VOBSゲイン3（Pn127）							*/
	LONG	Kj;								/* 位相補償VOBS位相補償ゲイン（Pn128）					*/
	LONG	Lpf;							/* 位相補償VOBSローパスフィルタゲイン（Pn13B + Pn401）	*/
	LONG	KTrqReflpf;						/* VOBSトルク指令ローパスフィルタゲイン					*/

	LONG	wk;
	LONG	sx = 0;

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバゲイン(Pn127 [Hz]) : EhVobs.P.KstfX	( X = 1～3)					*/
	/*		トルク指令フィルタ時定数の乗算は，バンク切替え対応のため，								*/
	/*		制御演算中に行うので，ここでは，トルクフィルタ時定数以外を計算する。					*/
	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.P.Kstf1	(wrkp.Kstf1)														*/
	/*																								*/
	/*					 9*(Prm.evobgn)*2π   (Tf[0.01ms])											*/
	/*	 		kstf1 = ------------------- * ------------											*/
	/*							2         	 	 10^5												*/
	/*----------------------------------------------------------------------------------------------*/
	wk = MlibScalKxgain( (LONG)(Prm->evobgn), 28274, 1000, &sx, 0 );	/* 28274 = 9*π*1000*/
	Kstf1 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );

	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.P.Kstf2	(wrkp.Kstf2)														*/
	/*																								*/
	/*					  27*(Prm.evobgn)^2*4π^2*SvCycleUs     (Tf[0.01ms])						*/
	/*			kstf2 = ------------------------------------ * --------------						*/
	/*								4*10^6                	  	   10^5								*/
	/*----------------------------------------------------------------------------------------------*/
	wk = (LONG)(Prm->evobgn) * (LONG)(Prm->evobgn);
	wk = MlibScalKxgain( wk, 266479, 1000, &sx, 0 );					/* 266479 = 27*π^2*1000	*/
	wk = MlibPcalKxgain( wk, EHSOBSCYCLE, 1000000, &sx, 0 );
	Kstf2 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );

	/*----------------------------------------------------------------------------------------------*/
	/*			EhVobs.P.Kstf3	(wrkp.Kstf3)														*/
	/*																								*/
	/*					  27*(Prm.evobgn)^3*8π^3*SvCycleUs^2    (Tf[0.01ms])						*/
	/*			kstf3 = -------------------------------------- * ------------						*/
	/*				          			8*10^12				   		10^5							*/
	/*----------------------------------------------------------------------------------------------*/
	wk = (LONG)(Prm->evobgn) * (LONG)(Prm->evobgn) * (LONG)(Prm->evobgn);
	wk = MlibScalKxgain( wk, 837169, 1000, &sx, 0 );					/* 837169 = 27*π^3*1000	*/
	wk = MlibPcalKxgain( wk, EHSOBSCYCLE, 1000000, &sx, 0 );
	wk = MlibPcalKxgain( wk, EHSOBSCYCLE, 1000000, &sx, 0 );
	Kstf3 = MlibPcalKxgain( wk, 1, 100000, &sx, 24 );

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバイナーシャゲイン(Pn128 [%]) : EhVobs.P.Kj							*/
	/*----------------------------------------------------------------------------------------------*/
	Kj = BpxNorObsJGainCal( Hprm->Kvx, Prm->jrate, Prm->evobjgn, (EHSOBSCYCLE * 1000) );

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバローパスフィルタゲイン(Pn13B [0.01ms]) : EhVobs.P.Lpf				*/
	/*		フィルタゲインは( (Pn13B:Prm.evobfil)+(Pn401:Prm.trqfil11) [0.01ms])で計算する			*/
	/*----------------------------------------------------------------------------------------------*/
	Lpf = MlibPcalKf1gain( (Prm->trqfil11)*10, EHSOBSCYCLE, 0 );

	/*----------------------------------------------------------------------------------------------*/
	/*		位相補償速度オブザーバトルク指令ローパスフィルタゲイン(Pn13C[0.01ms]) 					*/
	/*----------------------------------------------------------------------------------------------*/
	KTrqReflpf = MlibPcalKf1gain( 0, EHSOBSCYCLE, 0 );

	/*----------------------------------------------------------------------------------------------*/
	/*		Set Parameters																			*/
	/*----------------------------------------------------------------------------------------------*/
//	KPI_DI();												/* Disable Interrupt					*/
	EhVobs->Kstf1		= Kstf1;
	EhVobs->Kstf2		= Kstf2;
	EhVobs->Kstf3		= Kstf3;
	EhVobs->Kj			= Kj;
	EhVobs->Lpf			= Lpf;
	EhVobs->KTrqReflpf	= KTrqReflpf;
//	KPI_EI();												/* Enable Interrupt						*/
}
#endif




/****************************************************************************************************/
/*																									*/
/*		モータ暴走監視パラメータ計算(リニアモータ)													*//* <V051> */
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 暴走監視トルクレベル及び速度レベルの計算を行う											*/
/*																									*/
/*			暴走監視トルク		：PnE44.h,Pn483,Pn484の最も小さい値									*/
/*			暴走監視速度レベル	：オーバースピードの16.7%											*/
/*																									*/
/****************************************************************************************************/
void	PcalMotorOverrun_Linear( AXIS_HANDLE *Axis )
{
	ULONG			work1, work2;
	CHECK_OVRRUN	*OvrRun;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;

	OvrRun = &(Axis->CheckAlarm->OvrRun);
	Prm		= Axis->UniPrms->Prm;
	Bprm	= Axis->UniPrms->Bprm;

	work1 = Prm->ipg_rwt.b.h;

	/* 暴走監視トルク指令レベル */
	if( Prm->tlmtf_l < Prm->tlmtr_l )
	{
		work2 = Prm->tlmtf_l;
	}
	else
	{
		work2 = Prm->tlmtr_l;
	}

	if ( work2 < work1 )
	{
		work1 = work2;
	}
	if ( work1 > (ULONG)Bprm->PerMaxTrq )
	{
		OvrRun->OvrTrqLevel = 0x01000000;
	}
	else
	{
		OvrRun->OvrTrqLevel = MlibScalKxgain( work1, 0x01000000, Bprm->PerMaxTrq, NULL, -24);
	}

	/* 暴走監視モータ速度レベル */
	OvrRun->OvrSpdLevel = 0x01000000 / 6;

}



/****************************************************************************************************/
/*																									*/
/*		アンプ＆モータ過負荷ワーニングレベルの計算													*/
/*																									*/
/****************************************************************************************************/
void	PcalOverLoadWarLevelAmpMot( AXIS_HANDLE *Axis )
{
	CHECK_OL	*MotOL;
	CHECK_OL	*AmpOL;
	PRMDATA		*Prm;
	CHECK_LOWOL *LowOL;  		/* <S154> */
	CHECK_OLP	*AmpLockOLP;	/* <S154> */

	Prm		= Axis->UniPrms->Prm;
	MotOL = &(Axis->CheckAlarm->MotOL);
	AmpOL = &(Axis->CheckAlarm->AmpOL);
	LowOL = &(Axis->CheckAlarm->AmpLowOL);		/* <S154> */
	AmpLockOLP = &(AmpOL->conf.LockArea);		/* <S154> */

	/*----------------------------------------------------------------------------------*/
	/*	モータ過負荷チェック用パラメータ計算												*/
	/*----------------------------------------------------------------------------------*/
//	MotOL->conf.WrnLvlmid = MlibScalKxgain( MotOL->conf.AlmLvlmid, Prm->olwarlvl, 100, NULL, -30 );							/* <S154> */
//	MotOL->conf.WrnLvlmax = MlibScalKxgain( MotOL->conf.AlmLvlmax, Prm->olwarlvl, 100, NULL, -30 );							/* <S154> */
	MotOL->conf.NormalArea.WrnLvlmid = MlibScalKxgain( MotOL->conf.NormalArea.AlmLvlmid, Prm->olwarlvl, 100, NULL, -30 );	/* <S154> */
	MotOL->conf.NormalArea.WrnLvlmax = MlibScalKxgain( MotOL->conf.NormalArea.AlmLvlmax, Prm->olwarlvl, 100, NULL, -30 );	/* <S154> */

	/*----------------------------------------------------------------------------------*/
	/*	アンプ過負荷チェック用パラメータ計算												*/
	/*----------------------------------------------------------------------------------*/
//	AmpOL->conf.WrnLvlmid = MlibScalKxgain( AmpOL->conf.AlmLvlmid, Prm->olwarlvl, 100, NULL, -30 );							/* <S154> */
//	AmpOL->conf.WrnLvlmax = MlibScalKxgain( AmpOL->conf.AlmLvlmax, Prm->olwarlvl, 100, NULL, -30 );							/* <S154> */
	AmpOL->conf.NormalArea.WrnLvlmid = MlibScalKxgain( AmpOL->conf.NormalArea.AlmLvlmid, Prm->olwarlvl, 100, NULL, -30 );	/* <S154> */
	AmpOL->conf.NormalArea.WrnLvlmax = MlibScalKxgain( AmpOL->conf.NormalArea.AlmLvlmax, Prm->olwarlvl, 100, NULL, -30 );	/* <S154> */

 /* <S154> Start */
#if (CSW_SVCF_CHKAMPLOL == 1)                          /* CSW_SVCF_CHKAMPLOL：機種定義（機種による低速ＯＬ機能の有無）*/
	if(LowOL->LowOLChkP.conf.LowSpdOLChkEnable == TRUE)		/* 低速ＯＬ検出実行フラグ：検出可？			*/
	{
	    /* ワーニングレベルの反映		*/
		AmpLockOLP->WrnLvlmid = AmpOL->conf.NormalArea.WrnLvlmid;	/* 連続ＯＬﾜｰﾆﾝｸﾞﾚﾍﾞﾙ			*/
		AmpLockOLP->WrnLvlmax = AmpOL->conf.NormalArea.WrnLvlmax;	/* 瞬時ＯＬﾜｰﾆﾝｸﾞﾚﾍﾞﾙ			*/
	}
#endif
 /* <S154> End */
}



/****************************************************************************************************/
/*																									*/
/*		ＤＢ過負荷レベルの計算																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ＤＢ過負荷チェック用パラメータの計算を行う												*/
/*																									*/
/*			1) ＤＢ過負荷許容電力																	*/
/*			2) ＤＢ過負荷検出レベル																	*/
/*			3) ＤＢ過負荷検出電流変極点速度															*/
/*			4) ＤＢ過負荷検出飽和電力																*/
/*			5) ＤＢ電力演算係数																		*/
/*			6) 温度上昇によるＤＢ過負荷検出レベル													*/
/*																									*/
/*			Pn601 : Prm.dbres			ＤＢ抵抗容量				[10W]							*/
/*			Pn604 : Prm.exdbr			ＤＢ抵抗値					[10mΩ]							*/
/*			PnE12 : Prm.ampw			サーボパック容量			[W]								*/
/*			PnE1B : Prm.dbr				ＤＢ抵抗値(デフォルト)	    [mΩ]							*/
/*			PnE1D : Prm.wdb				ＤＢ抵抗容量(デフォルト)	[5000/ampw]						*/
/*			PnF0D : Prm.motr			電機子巻線抵抗				[10^-3Ω]						*/
/*			PnF0E : Prm.motl			機子インダクタンス			[10^-2mH]						*/
/*			PnF06 : Prm.oslv_pol.b.h	ポール数					[-]								*/
/*			PnF15 : Prm.ppitch			ポールピッチ				[0.1mm/180deg]					*/
/*																									*/
/****************************************************************************************************/
void	PcalDbrakeOverLoadLevel( AXIS_HANDLE *Axis )
{
	LONG		kx, sx;
	LONG		cx;
	USHORT		dbr;
	CHECK_DBOL	*pDbOL;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	/* ローカル変数の初期化 */
	pDbOL = &(Axis->CheckAlarm->DbOL);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ過負荷許容電力の計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		DbolPowBase = ＤＢ抵抗容量 * 5000 / AmpW 		[5000/サーボ定格容量]						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Prm->dbres == 0 )								/* 設定値ゼロの場合							*/
	{
		pDbOL->conf.DbolPowBase = Prm->wdb;				/* ＤＢ抵抗容量(デフォルト)					*/
	}
	else if( (Prm->dbres*10) <= Prm->ampw )				/* サーボ定格出力以内の場合					*/
	{
		pDbOL->conf.DbolPowBase = (Prm->dbres * 50000) / Prm->ampw;
	}
	else												/* サーボ定格出力以上は設定できない			*/
	{
		/* パラメータ設定異常 */
		ALMSetPramError( Axis->AlmManager, pndef_dbres.RegNumber );
		return;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ抵抗値の選択															<S0F5>			*/
/*--------------------------------------------------------------------------------------------------*/
	if( Prm->dbres != 0 )
	{ /* Pn601外付けＤＢ抵抗容量設定済み */
		if( Prm->exdbr != 0 )
		{ /* Pn604外付けＤＢ抵抗値設定済み */
			dbr = Prm->exdbr * 10U;								/* 単位変換[10mΩ]→ [mΩ]			*/
		}
		else
		{
			/* デフォルトＤＢ抵抗値を使用 */
			dbr = Prm->dbr;
		}
	}
	else
	{
		/* デフォルトＤＢ抵抗値を使用 */
		dbr = Prm->dbr;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ過負荷検出レベルの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		SGDS  : AlmLevel = ((PowBase * 2) * 429 * 1000 ) / SCCYCLEMS								*/
/*																									*/
/*		Σ-II : AlmLevel = ((PowBase * 2) * 429 * 1000000 ) / SCCYCLEUS / 65536						*/
/*																									*/
/*		経 緯 : 1) サーボ定格出力が１００ms続いたらアラーム											*/
/*				2) --> ＤＢ許容電力*２が１００sec連続でアラーム										*/
/*				3) --> ＤＢ許容電力*２が５００sec連続でアラーム										*/
/*			    4) --> ５００secだとオーバフローするので４２９secに変更								*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	pDbOL->conf.DbolAlmLevel = pDbOL->conf.DbolPowBase * ((2 * 429000) / KPI_SCCYCLEMS);

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ過負荷検出電流変極点速度																*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		ＤＢ消費電力を計算するための速度。消費電力が飽和する変極点の速度							*/
/*																									*/
/*							(Bprm.MotR + ((DBR/1000)/sqrt(3))) * 2^24								*/
/*		SVOS  : SpdLevel = ---------------------------------------------							*/
/*							  Bprm.MotL * (PoleNo/2) * Bprm.OvrSpd									*/
/*																									*/
/*																									*/
/*							(MotR + DBR/sqrt(3))/1000 * (60/(2*PAI)) * 15000 						*/
/*		Σ-II : SpdLevel = ---------------------------------------------------						*/
/*								(MotL/100000) * (PoleNo/2) * nos									*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*		回転型：Bprm.Keangle = PoleNo / 2		: Motor[rad] ---> Eangle[rad]						*/
/*		リニア：Bprm.Keangle = PAI / PolePitch	: Motor[ m ] ---> Eangle[rad]						*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
//	kx = MlibScalKskxkx( Bprm->MotR, 173205, 1, NULL, -30 ) + (Prm->dbr * 100);						  /* <S0F5> */
	kx = MlibScalKskxkx( Bprm->MotR, 173205, 1, NULL, -30 ) + (dbr * 100);							  /* <S0F5> */
	kx = MlibScalKxgain( kx, 1,          173205,        &sx,   0 );
	kx = MlibPcalKxkxks( kx, 1,          Bprm->Keangle, &sx,   0 );
	kx = MlibPcalKxkxks( kx, 1,          Bprm->MotLdb,  &sx,   0 );
	kx = MlibPcalKxkxks( kx, 0x01000000, Bprm->OvrSpd,  &sx, -30 );
	pDbOL->conf.DbolSpdLevel = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ過負荷検出飽和電力の計算																*/
/*--------------------------------------------------------------------------------------------------*/
/*												   													*/
/*						 |       Bprm.MotEmf       |^2     DBR      5000     (PAI + 3*sqrt(3))		*/
/*		SVOS  : PowMax = |-------------------------|    * ------ * ------ * -------------------		*/
/*						 | (Bprm.MotL * (PoleNo/2))|	      1000     AMPW          PAI			*/
/*																									*/
/*																									*/
/*						 |((MotEmf/10000)*(60/(2PAI)|^2    DBR      5000     (PAI + 3*sqrt(3))		*/
/*		Σ-II : PowMax = |--------------------------|   * ------ * ------ * -------------------		*/
/*						 |((MotL/100000)*(PoleNo/2) |      1000     AMPW          PAI				*/
/*																									*/
/*					   = (MotEmf*22263/PoleNo/MotL)^2 / AMPW * DBR >> 10							*/
/*																									*/
/*		Σ-II : Soft   = (MotEmf*13061/PoleNo/MotL)^2 / AMPW * DBR >> 10							*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
/*		補足１ : PAI + 3*sqrt(3)/2 は３相分を考慮した係数											*/
/*		補足２ : 計算と実際が合わないのでとりあえず1/2する(Sigma2より継承..)						*/
/*--------------------------------------------------------------------------------------------------*/
/*		回転型：Bprm.Keangle = PoleNo / 2		: Motor[rad] ---> Eangle[rad]						*/
/*		リニア：Bprm.Keangle = PAI / PolePitch	: Motor[ m ] ---> Eangle[rad]						*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
		cx = 50000;
	}
	else
	{
		cx = 290531;
	}
	/*----------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 1,  5*5739669,    3141593,       &sx,   0 );
//	kx = MlibPcalKxgain( kx, Prm->dbr,     Prm->ampw,     &sx,   0 );								  /* <S0F5> */
	kx = MlibPcalKxgain( kx, dbr,          Prm->ampw,     &sx,   0 );								  /* <S0F5> */
	kx = MlibPcalKxkxks( kx, 1,            Bprm->Keangle, &sx,   0 );
	kx = MlibPcalKxkxks( kx, 1,            Bprm->Keangle, &sx,   0 );
	kx = MlibPcalKxksks( kx, Bprm->MotEmf, Bprm->MotLdb,  &sx,   0 );
	kx = MlibPcalKxksks( kx, Bprm->MotEmf, Bprm->MotLdb,  &sx,   0 );
	kx = MlibPcalKxgain( kx, cx,           100000,        &sx, -30 );
	pDbOL->conf.DbolPowMax = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ電力演算ゲインの計算																	*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					    PowerMax * 2^18																*/
/*		Kdbpower = ------------------------															*/
/*					 SpdLevel * SpdLevel															*/
/*																									*/
/*					 PowerMax * Speed * Speed														*/
/*		DbPower  = --------------------------- = Kdbpower * (Speed>>9) * (Speed>>9)					*/
/*					   SpdLevel * SpdLevel															*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 1,  pDbOL->conf.DbolPowMax, pDbOL->conf.DbolSpdLevel, &sx,  0 );
	kx = MlibPcalKxgain( kx, (1<<18),                pDbOL->conf.DbolSpdLevel, &sx, 48 );
	pDbOL->conf.Kdbpower = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ抵抗消費電力演算ゲイン(for Un00B)の計算													*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*								100																	*/
/*		Kunrgpower = -------------------------------------											*/
/*					  DbolPowBase * KPI_SCANC_MS( 10000 )											*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 100, 1, pDbOL->conf.DbolPowBase, &sx,  0 );
	kx = MlibPcalKxgain( kx,  1, KPI_SCANC_MS( 10000 ),   &sx, 24 );
	pDbOL->conf.Kundbpower = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＢ抵抗消費電力演算ゲイン(for Un144)の計算													*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*								100																	*/
/*		Kunrgpower = -------------------------------------											*/
/*					  DbolPowBase * KPI_SCANC_MS( 2 )												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( 100, 1, pDbOL->conf.DbolPowBase,   &sx,  24 );
	pDbOL->conf.KundbpowerCycle = kx;

/*--------------------------------------------------------------------------------------------------*/
/*		温度上昇によるＤＢ過負荷検出レベルの計算													*/
/*--------------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( Prm->dbol_warn, 5000, Prm->ampw,      &sx,   0 );
	kx = MlibPcalKxgain( kx,             1000, KPI_SCCYCLEMS,  &sx, -30 );
	pDbOL->conf.DbOHeatWrnLvl = kx;

	kx = MlibScalKxgain( Prm->dbol_alm, 5000, Prm->ampw,      &sx,   0 );
	kx = MlibPcalKxgain( kx,            1000, KPI_SCCYCLEMS,  &sx, -30 );
	pDbOL->conf.DbOHeatAlmLvl = kx;

}
//#endif /* DB_CTRL */

/****************************************************************************************************/
/*		Pn000 : 機能選択基本スイッチ０ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prm0(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn001 : 機能選択応用スイッチ１ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prm1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

#if CSW_BOARD_TYPE == SGD7W_TYPE	/* <Oka06> */
	BOOL		DcVoltSel;
	PRMDATA		*Prm;
//	CMN_PRMDATA	*Cprm;

	Prm = Axis->UniPrms->Prm;
//	Cprm = Axis->UniPrms->Cprm;

	if( Cmd == PRM_ACCCMD_EEPWRITE )
	{ /* Write to Another */
		/* Get Write Status */
		DcVoltSel = (*pValue & 0x0100) >> 8;

		/* Set to Next Axis Address */
		if( Axis->AxisNo == 0 )
			Axis++;
		else
			Axis--;

		/* Write to Another Axis EEPROM */
		Prm = Axis->UniPrms->Prm;
		if( DcVoltSel != ((Prm->b_prm1 & 0x0100) >> 8) )
		{
			if( DcVoltSel == TRUE )
			{
				Prm->b_prm1 |= 0x0100;
			}
			else
			{
				Prm->b_prm1 &= 0xFEFF;
			}
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_b_prm1, 0 );
		}
	}


#if 0 /* 将来用 *//* <Oka06> */
	if( Cmd == PRM_ACCCMD_EEPWRITE )
	{
		/* ユニット共通エリアへの書き込み */
		if( (Prm->b_prm1 & 0x0100) >> 8 )
		{
			Cprm->UniPrmSel |= 0x0001;
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_UniPrmSel, 0 );
		}
		else
		{
			Cprm->UniPrmSel &= 0xFFFE;
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_UniPrmSel, 0 );
		}
	}
	else if( Cmd == PRM_ACCCMD_WRITE )
	{
		;
	}
	else
	{
		/* Current Axis  Pn020.0 ⇒ Pn001.2(電源入力選択)ユニット共通処理 */
		DcVoltSel = (Cprm->UniPrmSel & 0x0001);
		if( DcVoltSel != ((Prm->b_prm1 & 0x0100) >> 8) )
		{
			Prm->b_prm1 = (DcVoltSel==TRUE) ? (Prm->b_prm1 | 0x0100) : (Prm->b_prm1 & 0xFEFF);
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_b_prm1, 0 );
		}

		/* Set to Next Axis Address */
		if( Axis->AxisNo == 0 )
			Axis++;
		else
			Axis--;

		/* Another Axis  Pn020.0 ⇒ Pn001.2(電源入力選択)ユニット共通処理 */
		Prm = Axis->UniPrms->Prm;
		Cprm = Axis->UniPrms->Cprm;

		if( DcVoltSel == TRUE )
		{
			Prm->b_prm1 |= 0x0100;
		}
		else
		{
			Prm->b_prm1 &= 0xFEFF;
		}
	}
#endif
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn002 : 機能選択応用スイッチ２ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prm2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if CSW_OPT_CARD_FBOPTION == CSW_FBOPTION_SUPPORT /* <S0E4> */

	LONG rc;
	MENCV *MencV;
	OPFBK_HNDL* FbkOpt;
	PRMDATA* Prm;
	BPRMDAT* Bprm;

	MencV = Axis->MencV;
	FbkOpt = &Axis->OptCardHdl->FbkOpt;
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		rc = FbOptFullSemiPrmChk(MencV, FbkOpt, Prm, Bprm, TRUE) ;	/* 通常パラメータ計算時					*/
	}
	else
	{
		rc = FbOptFullSemiPrmChk(MencV, FbkOpt, Prm, Bprm, FALSE) ; /* レジスタＩ／Ｆからのパラメータ計算時 */
	}
	if(rc == FALSE)
	{
		/* A.042 : パラメータ組合わせ異常 */
		ALMSetServoAlarm( Axis->AlmManager, ALM_PRMUNMATCH );
	}
#endif

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S06D> */
	if( (Axis->UniPrms->Prm->b_prm2>>4) & 0x000F )
	{
		Axis->NetAxisIf->mt_hdl.mtPrm.BasePrm.vLimModeTrq = VLIM_MODE_ENABLE;
	}
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn006 : 機能選択応用スイッチ６ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prm6(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 0 );				/* アナログモニタ出力ゲイン計算					*//* <S016> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn007 : 機能選択応用スイッチ７ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prm7(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 1 );				/* アナログモニタ出力ゲイン計算					*//* <S016> */
	return PRM_RSLT_SUCCESS;
}

/* <S1F5> Start */
/****************************************************************************************************/
/*		Pn00A : 機能選択応用スイッチＡ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prmA(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	/* フルク対応なし。現状外部エンコーダチェック削除 */
	return PRM_RSLT_SUCCESS;
}
/* <S1F5> End */
/****************************************************************************************************/
/*		Pn00B : 機能選択応用スイッチＢ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_b_prmB(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if CSW_BOARD_TYPE == SGD7W_TYPE	/* <Oka06> */
	BOOL		SnglPhsSel;
	CMN_PRMDATA	*Cprm;
#endif
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
#if CSW_OPT_CARD_FBOPTION == CSW_FBOPTION_SUPPORT  /* <S0E4> */
	LONG rc;
	MENCV *MencV;
	OPFBK_HNDL* FbkOpt;
#endif

	Prm = Axis->UniPrms->Prm;

#if CSW_OPT_CARD_FBOPTION == CSW_FBOPTION_SUPPORT  /* <S0E4> */
	MencV = Axis->MencV;
	FbkOpt = &Axis->OptCardHdl->FbkOpt;
	Bprm = Axis->UniPrms->Bprm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		rc = FbOptFullSemiPrmChk(MencV, FbkOpt, Prm, Bprm, TRUE) ;	/* 通常パラメータ計算時					*/
	}
	else
	{
		rc = FbOptFullSemiPrmChk(MencV, FbkOpt, Prm, Bprm, FALSE) ; /* レジスタＩ／Ｆからのパラメータ計算時 */
	}
	if(rc == FALSE)
	{
		/* A.042 : パラメータ組合わせ異常 */
		ALMSetServoAlarm( Axis->AlmManager, ALM_PRMUNMATCH );
	}
#endif


#if CSW_BOARD_TYPE == SGD7W_TYPE	/* <Oka06> */
	if( Cmd == PRM_ACCCMD_EEPWRITE )
	{ /* Write to Another */
		/* Get Write Status */
		SnglPhsSel = (*pValue & 0x0100) >> 8;

		/* Set to Next Axis Address */
		if( Axis->AxisNo == 0 )
			Axis++;
		else
			Axis--;

		/* Write to Another Axis EEPROM */
		Prm = Axis->UniPrms->Prm;
		if( SnglPhsSel != ((Prm->b_prmB & 0x0100) >> 8) )
		{
			if( SnglPhsSel == TRUE )
			{
				Prm->b_prmB |= 0x0100;
			}
			else
			{
				Prm->b_prmB &= 0xFEFF;
			}
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_b_prmB, 0 );
		}
	}
#endif

#if 0 /* 将来用 *//* <Oka06> */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/* <Oka06> */
	Cprm = Axis->UniPrms->Cprm;
	if( PRM_ACCCMD_EEPWRITE )
	{
		if( (Prm->b_prmB & 0x0100) >> 8 )
		{
			Cprm->UniPrmSel |= 0x0010;
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_UniPrmSel, 0 );
		}
		else
		{
			Cprm->UniPrmSel &= 0xFFEF;
			PrmStoreValueToEeprom( Axis->RegManager->hPnReg, &pndef_UniPrmSel, 0 );
		}
	}
	else
	{
		/* Current Axis  Pn020.0 ⇒ Pn001.2(電源入力選択)ユニット共通処理 */
		SnglPhsSel = (Cprm->UniPrmSel & 0x0010);
		if( SnglPhsSel == TRUE )
		{
			Prm->b_prmB |= 0x0100;
		}
		else
		{
			Prm->b_prmB &= 0xFEFF;
		}

#if 0
	/* Set to Next Axis Address */
	if( Axis->AxisNo == 0 )
		Axis++;
	else
		Axis--;

	/* Another Axis  Pn020.0 ⇒ Pn001.2(電源入力選択)ユニット共通処理 */
	Prm = Axis->UniPrms->Prm;
	Cprm = Axis->UniPrms->Cprm;

	if( SnglPhsSel == TRUE )
	{
		Prm->b_prmB |= 0x0100;
	}
	else
	{
		Prm->b_prmB &= 0xFEFF;
	}
#endif
#endif
	}
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn100 : 速度ループゲイン [0.1Hz]															*/
/****************************************************************************************************/
PRM_RSLT pncal_loophz(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	DQACR_GAIN_PRM	AcrGainPrm;
	BASE_SPDLOOP_PRM SpdCtrlPrm;
#if( CSW_SVCF_DISTURBOBS != 0 )			/* <S0A0> */
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
#endif

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_Kv( Cmd, ArrayIdx, Axis, pValue );
#endif

	PcalCopyBaseSpeedLoopParam(&SpdCtrlPrm, Prm);
	PcalBaseSpeedControl( Axis->BaseControls, Bprm, &SpdCtrlPrm, 1 );

	PcalSpeedLimitGain( Axis->BaseControls, Bprm, Prm->jrate, Prm->loophz, 1 );
	RpiRegSetOnePrmTuningLevel( Axis->OnePrmTune, Prm->loophz );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKviSet( Axis->hGainChg, Prm, Bprm->SvCycleUs );
#endif
	{								
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
	}

	/* 電流ループゲイン 速度ゲイン連動	*/
	AcrGainPrm.ignd = Prm->ignd;		/* PnE20 : 電流ループゲイン(D)					*/
	AcrGainPrm.ignq = Prm->ignq;		/* PnE21 : 電流ループゲイン(Q)					*/
	AcrGainPrm.kid = Prm->kid;			/* PnE22 : 電流ループ積分時定数(D)				*/
	AcrGainPrm.kiq = Prm->kiq;			/* PnE23 : 電流ループ積分時定数(Q)				*/
	AcrGainPrm.ifil = Prm->ifil;		/* PnE26 : 電流ループフィルタ時定数				*/
	AcrGainPrm.ifil2 = Prm->ifil2;		/* PnEA4 : 静音モード電流ループフィルタ時定数	*/

	PcalMicroDqAcrGain( Axis->SvAsicRegs->AsicMicroReg,
						&AcrGainPrm,
						Axis->UniPrms->Bprm,
						MlibMAX(Prm->loophz, Prm->loophz2),
						Prm->KcGainRatio);

#if( CSW_SVCF_DISTURBOBS != 0 )				/* <S0A0> */
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[0]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 0 );
	/*----------------------------------------------------------------------------------------------*/
	{															
		ATGSEL	*pAtGsel;
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);		/* condition A */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate dKs */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);		/* condition B */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate dKs */
	}
	/*----------------------------------------------------------------------------------------------*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn101 : 速度ループ積分時定数 [0.01ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pitime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	BASE_SPDLOOP_PRM SpdCtrlPrm;

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_Tvi( Cmd, ArrayIdx, Axis, pValue );
#endif
	PcalCopyBaseSpeedLoopParam(&SpdCtrlPrm, Prm);
	PcalBaseSpeedControl( Axis->BaseControls, Bprm, &SpdCtrlPrm, 1 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKviSet( Axis->hGainChg, Axis->UniPrms->Prm, KPI_SBCYCLEUS );
#endif
	{	
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn102 : 位置ループゲイン [0.1/s]															*/
/****************************************************************************************************/
PRM_RSLT pncal_posgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_Kp( Cmd, ArrayIdx, Axis, pValue );
#endif
	PcalBasePositionPControl( Axis->BaseControls,
							  Bprm,
							  Axis->BaseControls->PosManager.Egear,
							  Prm->posgn,
							  1 );

	/* ゼロクランプ位置制御ゲイン計算	*/
	PcalZeroPositionControl( Axis->BaseControls, Bprm, Prm->posgn, 1 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKpSet( Axis->hGainChg, Axis->UniPrms->Prm, Bprm->SvCycleUs );
#endif
	{
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKp */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKp */
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn103 : 慣性モーメント比 [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_jrate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	BASE_SPDLOOP_PRM SpdCtrlPrm;
#if( CSW_SVCF_MFC_USE != 0 )
	MFC_CFG_PRM MfcCfgPrm;
#endif
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;
#endif
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
#endif
#if( CSW_SVCF_EHVOBS_USE != 0 )
	EHVOBS_CTRL		*EhVobsCtrl;
	EHVOBS_CFGPRM	EhVobsCfgPrm;
#endif
	TRANSDUCE_CTRL	*TransduceCtrl;
	BOOL			PrmChgSts;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	TransduceCtrl = &(Axis->BaseControls->TransduceCtrl);

	PcalCopyBaseSpeedLoopParam(&SpdCtrlPrm, Prm);
	PcalBaseSpeedControl( Axis->BaseControls, Bprm, &SpdCtrlPrm, 1 );
	PcalBaseSpeedControl( Axis->BaseControls, Bprm, &SpdCtrlPrm, 2 );

	/*----------------------------------------------------------------------------------------------*/
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
	}
	/*----------------------------------------------------------------------------------------------*/

#if( CSW_SVCF_MFC_USE != 0 )
	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
					 &MfcCfgPrm,
					 Axis->UniPrms->Prm->jrate,
					 Axis->UniPrms->Bprm,
					 0, 0 );
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
					 &MfcCfgPrm,
					 Axis->UniPrms->Prm->jrate,
					 Axis->UniPrms->Bprm,
					 0, 1 );
#endif

#if( CSW_SVCF_DISTURBOBS != 0 )
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;			/* Pn103 : 慣性モーメント比		*/
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[0]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 0 );
	DobsCalculatePrm( &(DobsPrm[1]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 1 );
	/*----------------------------------------------------------------------------------------------*/
	{															
		ATGSEL	*pAtGsel;											/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);		/* condition A */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate dKs */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);		/* condition B */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate dKs */
	}
	/*----------------------------------------------------------------------------------------------*/
#endif

#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	PcalCopyResVibCfgParam(&ResVibCfgPrm, Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib), &ResVibCfgPrm,
						Bprm->Kvx, KPI_SBCYCLE_BASE_NS/* <S0DA> KPI_MBCYCLENS */, Axis->SvAsicRegs );
#endif

#if( CSW_SVCF_VIB_DETECT_USE != 0 )
	DetVibObsCalculateJGain( &(Axis->DetVib->DetVibObs), Bprm->Kvx, Prm->jrate, Bprm->SvCycleNs );
#endif

#if( CSW_SVCF_EHVOBS_USE != 0 )
	EhVobsCtrl = &(Axis->BaseControls->EhVobsCtrl);
	EhVobsCfgPrm.jrate = Prm->jrate;		/* Pn103 : 慣性モーメント比					*/
	EhVobsCfgPrm.evobgn = Prm->evobgn;		/* Pn127 : 速度オブザーバゲイン				*/
	EhVobsCfgPrm.evobjgn = Prm->evobjgn;	/* Pn128 : 速度オブザーバ位置補償ゲイン		*/
	EhVobsCfgPrm.trqfil11 = Prm->trqfil11;	/* Pn401 : 1段目第1トルク指令フィルタ時定数	*/
	PrmChgSts = EhVobsCalculatePrm( EhVobsCtrl, Bprm, &EhVobsCfgPrm, TransduceCtrl ); /* <S066> */
	if( FALSE == PrmChgSts )
	{
		ALMSetServoAlarm( Axis->AlmManager, ALM_PRM_TRANSLATE );
	}
#endif

	/* モータレステストモードパラメータ計算			*//* <S1B7> */
	PcalMotorLessTestMode( Axis->FencV, Bprm, Prm, &(Axis->MotorLessTest->MotorLessMdl) );

	PcalSpeedLimitGain( Axis->BaseControls, Bprm, Prm->jrate, Prm->loophz, 1 );
	PcalSpeedLimitGain( Axis->BaseControls, Bprm, Prm->jrate, Prm->loophz2, 2 );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn104 : 第２速度ループゲイン [0.1Hz]														*/
/****************************************************************************************************/
PRM_RSLT pncal_loophz2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	DQACR_GAIN_PRM	AcrGainPrm;
	BASE_SPDLOOP_PRM SpdCtrlPrm;
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
#endif
	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

	PcalCopyBaseSpeedLoopParam(&SpdCtrlPrm, Prm);
	PcalBaseSpeedControl( Axis->BaseControls, Bprm, &SpdCtrlPrm, 2 );

	PcalSpeedLimitGain( Axis->BaseControls, Bprm, Prm->jrate, Prm->loophz2, 2 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKviSet( Axis->hGainChg, Prm, Bprm->SvCycleUs );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
	}

	/* 電流ループゲイン 速度ゲイン連動			*/
	AcrGainPrm.ignd = Prm->ignd;		/* PnE20 : 電流ループゲイン(D)					*/
	AcrGainPrm.ignq = Prm->ignq;		/* PnE21 : 電流ループゲイン(Q)					*/
	AcrGainPrm.kid = Prm->kid;			/* PnE22 : 電流ループ積分時定数(D)				*/
	AcrGainPrm.kiq = Prm->kiq;			/* PnE23 : 電流ループ積分時定数(Q)				*/
	AcrGainPrm.ifil = Prm->ifil;		/* PnE26 : 電流ループフィルタ時定数				*/
	AcrGainPrm.ifil2 = Prm->ifil2;		/* PnEA4 : 静音モード電流ループフィルタ時定数	*/

	PcalMicroDqAcrGain( Axis->SvAsicRegs->AsicMicroReg,
						&AcrGainPrm,
						Axis->UniPrms->Bprm,
						MlibMAX(Prm->loophz, Prm->loophz2),
						Prm->KcGainRatio);

#if( CSW_SVCF_DISTURBOBS != 0 )
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[1]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 1 );
	/*----------------------------------------------------------------------------------------------*/
	/*	<S0A0>																						*/
	/*----------------------------------------------------------------------------------------------*/
	{															
		ATGSEL	*pAtGsel;
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);		/* condition A */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate dKs */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);		/* condition B */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs) );	/* calculate dKs */
	}
	/*----------------------------------------------------------------------------------------------*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn105 : 第２速度ループ積分時定数 [0.01ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pitime2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	BASE_SPDLOOP_PRM SpdCtrlPrm;

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

	PcalCopyBaseSpeedLoopParam(&SpdCtrlPrm, Prm);
	PcalBaseSpeedControl( Axis->BaseControls, Bprm, &SpdCtrlPrm, 2 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKviSet( Axis->hGainChg, Axis->UniPrms->Prm, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKviSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKv, dKvi */
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn106 : 第２位置ループゲイン [0.1/s]														*/
/****************************************************************************************************/
PRM_RSLT pncal_posgn2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

	PcalBasePositionPControl( Axis->BaseControls, 
							  Bprm,
							  Axis->BaseControls->PosManager.Egear,
							  Prm->posgn2,
							  2 );

	/* ゼロクランプ位置制御ゲイン計算	*/
	PcalZeroPositionControl( Axis->BaseControls, Bprm, Prm->posgn2, 2 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKpSet( Axis->hGainChg, Axis->UniPrms->Prm, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKp */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKpSet( pAtGsel, &(pAtGsel->conf.dGain) );	/* calculate dKp */
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn109 : フィードフォワード [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_ffgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA *Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm =Axis->UniPrms->Bprm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_FFGain( Cmd, ArrayIdx, Axis, pValue );
#endif

	Axis->BaseControls->CtrlCmdPrm.KdPff = PcaldPosRefFFGain( Bprm->Kpx, Prm->ffgn, Bprm->SvCycleUs );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn10A : フィードフォワードフィルタ時定数 [0.01ms]											*/
/****************************************************************************************************/
PRM_RSLT pncal_fffil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA *Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm =Axis->UniPrms->Bprm;
	Axis->BaseControls->CtrlCmdPrm.KdPffFil = MlibPcalKf1gain( Prm->fffil*10, Bprm->SvCycleUs, 0 );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn10B : ゲイン関係応用スイッチ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_gnmode(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//		PcalBitParamMSW();
	ULONG		data;
	MODE_SW		*ModeSwData;
	PRMDATA		*Prm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;
	data = Prm->gnmode & 0x0F;
	if( data <= 4 )
	{
		ModeSwData->conf.ModeSWSel = data;
	}
	else
	{
		ALMSetPramError( Axis->AlmManager, pndef_gnmode.RegNumber );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn10C : モードスイッチ(トルク指令) [%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mdswlv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MODE_SW		*ModeSwData;
	PRMDATA		*Prm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;

	ModeSwData->conf.MSWTrqLevel
			= BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->mdswlv ), 0 );	/* [%] --> [2^24/MaxTrq]	*/
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn10D : モードスイッチ(速度指令) [min-1]													*/
/****************************************************************************************************/
PRM_RSLT pncal_mswrf(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MODE_SW		*ModeSwData;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		ModeSwData->conf.MSWSpdLevel = BprmSpeedLevelCal( Bprm, Prm->mswrf*10, 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn10E : モードスイッチ(加速度) [min-1/s]													*/
/****************************************************************************************************/
PRM_RSLT pncal_mswacc(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG		kx, sx;
#endif /* FLOAT_USE */
	MODE_SW		*ModeSwData;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					   Prm.mswacc * Bprm.Knorspd * SCCYCLEUS										*/
/*		MSWAccLevel = ---------------------------------------										*/
/*									 1000000														*/
/*																									*/
/*		Prm.mswacc	 : [r/min/s]																	*/
/*		Bprm.Knorspd : [r/min,mm/s --> NorSpd]														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{
#if (FLOAT_USE==TRUE)
		fw = (float)Prm->mswacc * Bprm->Knorspd;
		fw = fw * (float)KPI_SCCYCLEUS / 1000000.0F;
		if( fw > (float)0x01000000 )
		{
			ModeSwData->conf.MSWAccLevel = 0x01000000;
		}
		else
		{
			ModeSwData->conf.MSWAccLevel = (LONG)fw;
		}
#else
		sx = 0;
		kx = MlibScalKxkskx( Prm->mswacc, Bprm->Knorspd, 1, &sx, 0 );
		kx = MlibPcalKxgain( kx,  KPI_SCCYCLEUS, 1000000, &sx, -24 );
		ModeSwData->conf.MSWAccLevel = kx;
#endif /* FLOAT_USE */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn10F : モードスイッチ(位置偏差) [指令単位]													*/
/****************************************************************************************************/
PRM_RSLT pncal_msplvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MODE_SW		*ModeSwData;
	PRMDATA	*Prm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;

	ModeSwData->conf.MSWErrLevel =
			BprmPositionLevelCal( Axis->UniPrms->Bprm, Prm->msplvl );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn11F : 位置積分時定数 [0.1ms]																*/
/****************************************************************************************************/
PRM_RSLT pncal_pitfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CTRL_CMD_PRM	*CtrlCmdPrm;
	PRMDATA			*Prm;
#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D> */
	BPRMDAT			*Bprm;
	Bprm = Axis->UniPrms->Bprm;
#endif/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT) */
	
	CtrlCmdPrm	= &(Axis->BaseControls->CtrlCmdPrm);
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_pitfil( Cmd, ArrayIdx, Axis, pValue );
#endif

	if( Prm->pitfil )
	{
#if (FLOAT_USE==TRUE)
		CtrlCmdPrm->Kpi = (float)(KPI_SACYCLEUS) / (100.0f * (float)( Prm->pitfil ));
#else
		CtrlCmdPrm->Kpi = MlibScalKxgain( 1, KPI_SACYCLEUS, 100*Prm->pitfil, NULL, 24 );
#endif /* FLOAT_USE */

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT)/* <S04D> */
		CtrlCmdPrm->KpiFc = MlibScalKxgain( 1, Bprm->SvCycleUs, 100*Prm->pitfil, NULL, 24 );
#endif	/* (CSW_SVCF_FULC_MODE == CSW_FULC_BASIC_SUPPORT) */

	}
	else
	{
		CtrlCmdPrm->Kpi = 0;
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn121 : 摩擦補償ゲイン [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_dobgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[0]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 0 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKsSet( Axis->hGainChg, Axis->UniPrms->Prm, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs)); /* calculate dKs */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs)); /* calculate dKs */
	}

#endif /* #if( CSW_SVCF_DISTURBOBS != 0 ) */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn122 : 第２摩擦補償ゲイン [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_dobgn2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[1]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 1 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKsSet( Axis->hGainChg, Axis->UniPrms->Prm, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs)); /* calculate dKs */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs)); /* calculate dKs */
	}

#endif /* #if( CSW_SVCF_DISTURBOBS != 0 ) */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn123 : 摩擦補償係数 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_dtrqgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[0]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 0 );
	DobsCalculatePrm( &(DobsPrm[1]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn124 : 摩擦補償周波数補正 [0.1Hz]															*/
/****************************************************************************************************/
PRM_RSLT pncal_dlpfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[0]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 0 );
	DobsCalculatePrm( &(DobsPrm[1]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn125 : 摩擦補償ゲイン補正 [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_dobjgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_DISTURBOBS != 0 )
	DOBS_PRM		*DobsPrm;
	DOBS_CFGPRM		DobsCfgPrm;
	PRMDATA			*Prm;

	Prm = Axis->UniPrms->Prm;
//<S09B>	DobsPrm = &(Axis->BaseControls->DobsCtrl.DobsPrm[0]);
	DobsPrm = &(Axis->BaseControls->GainChange.GselDobs[1]);		/* <S09B> */

	DobsCfgPrm.jrate = Prm->jrate;
	DobsCfgPrm.loophz = Prm->loophz;		/* Pn100 : 速度ループゲイン		*/
	DobsCfgPrm.loophz2 = Prm->loophz2;		/* Pn104 : 第２速度ループゲイン	*/
	DobsCfgPrm.dobgn = Prm->dobgn;			/* Pn121 : 摩擦補償ゲイン		*/
	DobsCfgPrm.dobgn2 = Prm->dobgn2;		/* Pn122 : 第２摩擦補償ゲイン	*/
	DobsCfgPrm.dtrqgn = Prm->dtrqgn;		/* Pn123 : 摩擦補償係数			*/
	DobsCfgPrm.dlpfil = Prm->dlpfil;		/* Pn124 : 摩擦補償周波数補正	*/
	DobsCfgPrm.dobjgn = Prm->dobjgn;		/* Pn125 : 摩擦補償ゲイン補正	*/

	DobsCalculatePrm( &(DobsPrm[0]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 0 );
	DobsCalculatePrm( &(DobsPrm[1]), Axis->SvAsicRegs, Axis->UniPrms->Bprm, &DobsCfgPrm, 1 );

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKsSet( Axis->hGainChg, Axis->UniPrms->Prm, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs)); /* calculate dKs */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKsSet( pAtGsel, &(pAtGsel->conf.dGainDobs)); /* calculate dKs */
	}

#endif /* #if( CSW_SVCF_DISTURBOBS != 0 ) */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn127 : 速度オブザーバゲイン [Hz]															*/
/****************************************************************************************************/
PRM_RSLT pncal_evobgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_EHVOBS_USE != 0 )
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	EHVOBS_CTRL		*EhVobsCtrl;
	EHVOBS_CFGPRM	EhVobsCfgPrm;
	TRANSDUCE_CTRL	*TransduceCtrl;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	TransduceCtrl = &(Axis->BaseControls->TransduceCtrl);
	EhVobsCtrl = &(Axis->BaseControls->EhVobsCtrl);

	EhVobsCfgPrm.jrate = Prm->jrate;		/* Pn103 : 慣性モーメント比					*/
	EhVobsCfgPrm.evobgn = Prm->evobgn;		/* Pn127 : 速度オブザーバゲイン				*/
	EhVobsCfgPrm.evobjgn = Prm->evobjgn;	/* Pn128 : 速度オブザーバ位置補償ゲイン		*/
	EhVobsCfgPrm.trqfil11 = Prm->trqfil11;	/* Pn401 : 1段目第1トルク指令フィルタ時定数	*/
	EhVobsCalculatePrm( EhVobsCtrl, Bprm, &EhVobsCfgPrm, TransduceCtrl );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn128 : 速度オブザーバ位置補償ゲイン [%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_evobjgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_EHVOBS_USE != 0 )
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	EHVOBS_CTRL		*EhVobsCtrl;
	EHVOBS_CFGPRM	EhVobsCfgPrm;
	TRANSDUCE_CTRL	*TransduceCtrl;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	TransduceCtrl = &(Axis->BaseControls->TransduceCtrl);
	EhVobsCtrl = &(Axis->BaseControls->EhVobsCtrl);

	EhVobsCfgPrm.jrate = Prm->jrate;		/* Pn103 : 慣性モーメント比					*/
	EhVobsCfgPrm.evobgn = Prm->evobgn;		/* Pn127 : 速度オブザーバゲイン				*/
	EhVobsCfgPrm.evobjgn = Prm->evobjgn;	/* Pn128 : 速度オブザーバ位置補償ゲイン		*/
	EhVobsCfgPrm.trqfil11 = Prm->trqfil11;	/* Pn401 : 1段目第1トルク指令フィルタ時定数	*/
	EhVobsCalculatePrm( EhVobsCtrl, Bprm, &EhVobsCfgPrm, TransduceCtrl );
#endif
	return PRM_RSLT_SUCCESS;
}


#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
/****************************************************************************************************/
/*		Pn131 : ゲイン切り替え時間１ [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_gnswfil1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	BPRMDAT	*Bprm;

	Bprm = Axis->UniPrms->Bprm;
	GselCalculatePrm( Axis->hGainChg, Axis->UniPrms->Prm, Bprm->SvCycleUs );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn132 : ゲイン切り替え時間２ [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_gnswfil2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	BPRMDAT	*Bprm;

	Bprm = Axis->UniPrms->Bprm;
	GselCalculatePrm( Axis->hGainChg, Axis->UniPrms->Prm, Bprm->SvCycleUs );
	return PRM_RSLT_SUCCESS;
}
#endif /* #if 0 */

/****************************************************************************************************/
/*		<S09B> Pn131 : ゲイン切り替え時間１ [ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_gnswfil1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	BPRMDAT	*Bprm;
	Bprm = Axis->UniPrms->Bprm;
	PcalAutoGainChange( &(Axis->BaseControls->GainChange.AtGselA),		/* condition A */
	                  	Axis->UniPrms->Prm->gnswfil1,					/* Pn131 */
	                  	Axis->UniPrms->Prm->gnswwait1,					/* Pn135 */
						Bprm->SvCycleUs );								/* ScanB cycle */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		<S09B> Pn132 : ゲイン切り替え時間２ [ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_gnswfil2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	BPRMDAT	*Bprm;

	Bprm = Axis->UniPrms->Bprm;
	PcalAutoGainChange( &(Axis->BaseControls->GainChange.AtGselB),		/* condition B */
	                  	Axis->UniPrms->Prm->gnswfil2,					/* Pn132 */
	                  	Axis->UniPrms->Prm->gnswwait2,					/* Pn136 */
						Bprm->SvCycleUs );								/* ScanB cycle */
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn135 : ゲイン切り替え待ち時間１ [ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_gnswwait1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	BPRMDAT	*Bprm;

	Bprm = Axis->UniPrms->Bprm;
//<S09B>	GselCalculatePrm( &(Axis->BaseControls->GainChange),
//	                  Axis->UniPrms->Prm->gnswwait1,
//	                  Axis->UniPrms->Prm->gnswwait2,
//	                  Bprm->SvCycleUs );
	PcalAutoGainChange( &(Axis->BaseControls->GainChange.AtGselA),		/* condition A <S09B> */
	                  	Axis->UniPrms->Prm->gnswfil1,					/* Pn131 */
	                  	Axis->UniPrms->Prm->gnswwait1,					/* Pn135 */
						Bprm->SvCycleUs );								/* ScanB cycle */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn136 : ゲイン切り替え待ち時間２ [ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_gnswwait2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	BPRMDAT	*Bprm;

	Bprm = Axis->UniPrms->Bprm;
//<S09B>	GselCalculatePrm( &(Axis->BaseControls->GainChange),
//	                  Axis->UniPrms->Prm->gnswwait1,
//	                  Axis->UniPrms->Prm->gnswwait2,
//	                  Bprm->SvCycleUs );
	PcalAutoGainChange( &(Axis->BaseControls->GainChange.AtGselB),		/* condition B <S09B> */
	                  	Axis->UniPrms->Prm->gnswfil2,					/* Pn132 */
	                  	Axis->UniPrms->Prm->gnswwait2,					/* Pn136 */
						Bprm->SvCycleUs );								/* ScanB cycle */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn139 : 自動ゲイン切り替え関係スイッチ１ [－]												*/
/****************************************************************************************************/
PRM_RSLT pncal_gnsw1( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
//		PcalGainChange( Axis );
	ULONG	ucwk;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;

/*--------------------------------------------------------------------------------------------------*/
/*		Pn139.0 : ゲイン切替選択設定																*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->gnsw1 & 0x0F )
	{
	case 0x00:
		Axis->BaseControls->GainChange.AtGsel = FALSE;
		break;
	case 0x01:
		Axis->BaseControls->GainChange.AtGsel = FALSE;
		break;
	case 0x02:
		Axis->BaseControls->GainChange.AtGsel = TRUE;
		break;
	default :
		ALMSetPramError( Axis->AlmManager, pndef_gnsw1.RegNumber );
		break;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Pn139.1 : 自動ゲイン切替条件Ａ選択設定														*/
/*--------------------------------------------------------------------------------------------------*/
	ucwk = ( Prm->gnsw1 >> 4 ) & 0x0F;
	switch( ucwk )
	{
		case 0x00:									/* /COIN Onでゲイン切替							*/
		case 0x01:									/* /COIN Offでゲイン切替						*/
		case 0x02:									/* /NEAR Onでゲイン切替							*/
		case 0x03:									/* /NEAR Offでゲイン切替						*/
		case 0x04:									/* /DEN Onでゲイン切替							*/
		case 0x05:									/* /DEN Offでゲイン切替							*/
			Axis->BaseControls->GainChange.AtGselTrgA = ucwk;
			break;
		default :
			ALMSetPramError( Axis->AlmManager, pndef_gnsw1.RegNumber );
			break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn13D : 電流ゲインレベル [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_KcGainRatio(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	DQACR_GAIN_PRM	AcrGainPrm;

	Prm = Axis->UniPrms->Prm;

	AcrGainPrm.ignd = Prm->ignd;		/* PnE20 : 電流ループゲイン(D)					*/
	AcrGainPrm.ignq = Prm->ignq;		/* PnE21 : 電流ループゲイン(Q)					*/
	AcrGainPrm.kid = Prm->kid;			/* PnE22 : 電流ループ積分時定数(D)				*/
	AcrGainPrm.kiq = Prm->kiq;			/* PnE23 : 電流ループ積分時定数(Q)				*/
	AcrGainPrm.ifil = Prm->ifil;		/* PnE26 : 電流ループフィルタ時定数				*/
	AcrGainPrm.ifil2 = Prm->ifil2;		/* PnEA4 : 静音モード電流ループフィルタ時定数	*/

	/* 電流ループゲイン 速度ゲイン連動	*/
	PcalMicroDqAcrGain( Axis->SvAsicRegs->AsicMicroReg,
						&AcrGainPrm,
						Axis->UniPrms->Bprm,
						MlibMAX(Prm->loophz, Prm->loophz2),
						Prm->KcGainRatio);

	return PRM_RSLT_SUCCESS;
}



/****************************************************************************************************/
/*		Pn140 : モデル追従制御関連スイッチ [－]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlsw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	if(FALSE != MfcCalculatePrmSW( &(Axis->BaseControls->MFControl),
	                               &MfcCfgPrm,
	                               Axis->UniPrms->Prm->jrate,
	                               Axis->UniPrms->Bprm ) )
	{
		ALMSetPramError( Axis->AlmManager, pndef_mdlsw.RegNumber );
	}
#endif

#if ( CSW_SVCF_RESVIB_FIL_USE == TRUE )
//	if(FALSE != PosMngCalculatePrmVibSupFilSW( /* <S066> */
	if(FALSE != PcmdFilCalculatePrmVibSupFilSW(
						&(Axis->BaseControls->VibSupFil),
						Axis->UniPrms->Prm->mdlsw,
						Axis->UniPrms->Prm->ff_frq,
						Axis->UniPrms->Prm->ff_fil,
						Axis->UniPrms->Bprm->SvCycleNs) )
	{
		ALMSetPramError( Axis->AlmManager, pndef_mdlsw.RegNumber );
	}
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn141 : モデル追従制御ゲイン [0.1/s]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
#endif
	RpiRegSetOnePrmTuningFfLevel( Axis->OnePrmTune, Axis->UniPrms->Prm->mdlgn );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn142 : モデル追従制御ゲイン補正 [0.1%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlzeta(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn143 : モデル追従制御バイアス(正転方向) [0.1%]												*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlptff(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn144 : モデル追従制御バイアス(逆転方向) [0.1%]												*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlntff(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn145 : 振動抑制１周波数Ａ [0.1Hz]															*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlantfrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
	RpiRegSetVibMfcTuningFrequency( Axis->MfcTune,
									Axis->UniPrms->Prm->mdlantfrq );
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn146 : 振動抑制１周波数Ｂ [0.1Hz]															*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlresfrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn147 : モデル追従制御速度フィードフォワード補償 [0.1%]										*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlvff(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 0 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn148 : 第２モデル追従制御ゲイン [0.1/s]													*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlgn2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn149 : 第２モデル追従制御ゲイン補正 [0.1%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_mdlzeta2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_MFC_USE == TRUE )
	MFC_CFG_PRM MfcCfgPrm;

	PcalCopyMFCCfgParam(&MfcCfgPrm, Axis->UniPrms->Prm);
	MfcCalculatePrm( &(Axis->BaseControls->MFControl),
                     &MfcCfgPrm,
                     Axis->UniPrms->Prm->jrate,
                     Axis->UniPrms->Bprm,
                     0, 1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn14A : 振動抑制２周波数 [0.1Hz]															*/
/****************************************************************************************************/
PRM_RSLT pncal_ff_frq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	if( CSW_SVCF_RESVIB_FIL_USE == TRUE )
	{
//		PosMngCalculatePrmVibSupFil( &(Axis->BaseControls->VibSupFil), /* <S066> */
		PcmdFilCalculatePrmVibSupFil( &(Axis->BaseControls->VibSupFil),
									  Axis->UniPrms->Prm->ff_frq,
									  Axis->UniPrms->Prm->ff_fil,
									  Axis->UniPrms->Bprm->SvCycleNs );
		RpiRegSetMdlVibFreq2( Axis->MfcTune,
							  Axis->UniPrms->Prm->ff_frq );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn14B : 振動抑制２補正 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_ff_fil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	if( CSW_SVCF_RESVIB_FIL_USE == TRUE )
	{
//		PosMngCalculatePrmVibSupFil( &(Axis->BaseControls->VibSupFil), /* <S066> */
		PcmdFilCalculatePrmVibSupFil( &(Axis->BaseControls->VibSupFil),
									  Axis->UniPrms->Prm->ff_frq,
									  Axis->UniPrms->Prm->ff_fil,
									  Axis->UniPrms->Bprm->SvCycleNs );
		RpiRegSetMdlVibFil2( Axis->MfcTune,
							 Axis->UniPrms->Prm->ff_fil );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn160 : 制振制御関連スイッチ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_avibsw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	if( FALSE != ResVibCalculatePrmSW( &(Axis->BaseControls->ResVib),
									   Axis->UniPrms->Prm->avibsw,
									   Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse,
									   Axis->SvAsicRegs ) )
	{
		ALMSetPramError( Axis->AlmManager, pndef_avibsw.RegNumber );
	}
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn161 : Ａ型制振周波数 [0.1Hz]																*/
/****************************************************************************************************/
PRM_RSLT pncal_avibfrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );

	RpiRegSetAtypeVibTuningFrequency( Axis->AvibTune,
									  Axis->UniPrms->Prm->avibfrq );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn162 : Ａ型制振ゲイン補正 [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_avibgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn163 : Ａ型制振ダンピングゲイン [%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_avibdamp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );

	RpiRegSetAtypeVibTuningGain( Axis->AvibTune,
								 Axis->UniPrms->Prm->avibdamp );
#endif
		return PRM_RSLT_SUCCESS;

}

/****************************************************************************************************/
/*		Pn164 : Ａ型制振フィルタ時定数１補正 [0.01ms]												*/
/****************************************************************************************************/
PRM_RSLT pncal_avibfil1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn165 : Ａ型制振フィルタ時定数２補正 [0.01ms]												*/
/****************************************************************************************************/
PRM_RSLT pncal_avibfil2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );
#endif
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn166 : Ａ型制振ダンピングゲイン2 [%]		<S03E>												*/
/****************************************************************************************************/
PRM_RSLT pncal_avibdamp2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );

	RpiRegSetAtypeVibTuningGain( Axis->AvibTune,
								 Axis->UniPrms->Prm->avibdamp2 );
#endif
		return PRM_RSLT_SUCCESS;

}
/****************************************************************************************************/
/*		Pn167 : Ａ型制振フィルタ時定数２補正 [0.01ms]		<S03E>										*/
/****************************************************************************************************/
PRM_RSLT pncal_avibfrq2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if( CSW_SVCF_ATYPE_RES_VIB_USE != 0 )
	RESVIB_CFG_PRM ResVibCfgPrm;

	PcalCopyResVibCfgParam(&ResVibCfgPrm, Axis->UniPrms->Prm);
	ResVibCalculatePrm( &(Axis->BaseControls->ResVib),
	                    &ResVibCfgPrm,
						Axis->UniPrms->Bprm->Kvx,
						//KPI_MBCYCLENS,
						KPI_SBCYCLE_BASE_NS,//<S09D>
						Axis->SvAsicRegs );
#endif
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn170 : Ａダイナミックオートチューニング関連スイッチ										*/
/****************************************************************************************************/
PRM_RSLT pncal_DatLevel(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA				*Prm;
	TUNELESS_SETPRM		SetPrm;
	BASE_SPDLOOP_PRM 	SpdCtrlPrm;	/* <S01E> */
	BOOL				PrmChgSts;

	Prm = Axis->UniPrms->Prm;

	SetPrm.DatLevel = Prm->DatLevel;			/* Pn170 : 調整レス関連スイッチ						*/
	SetPrm.trqfil11 = Prm->trqfil11;			/* Pn401 : 1段目第1トルク指令フィルタ時定数			*/
	SetPrm.ampw = Prm->ampw;					/* PnE12 : サーボパック容量							*/
	SetPrm.ignq = Prm->ignq;					/* PnE21 : 電流ループゲイン(Q)						*/
	SetPrm.kiq = Prm->kiq;						/* PnE23 : 電流ループ積分時定数(Q)					*/
	SetPrm.fbfil2 = Prm->fbfil2;				/* PnEA8 : 第2速度F/Bフィルタ時定数					*/
	SetPrm.TuneLessGain = Prm->TunLessGain;		/* PnEDA : 調整レス外乱オブザーバゲイン				*/
	SetPrm.TuneLessStiff = Prm->TunLessStiff;	/* PnEDB : 調整レスイナーシャ補正ゲイン				*/
	SetPrm.TuneLessTrqLpf = Prm->TunLessTrqLpf;	/* PnEDC : 調整レス外乱トルクローパスフィルタ時定数	*/
	SetPrm.TuneLessVfbLpf = Prm->TunLessVfbLpf;	/* PnEDD : 調整レス速度FBローパスフィルタ時定数		*/
	SetPrm.MencP = &Prm->MencP;					/* PnF00 - PnF1E : エンコーダパラメータ				*/
	SetPrm.DetVib = Axis->DetVib;
	SetPrm.ANotchSeq = Axis->ANotchSeq;

	PcalCopyBaseSpeedLoopParam( &SpdCtrlPrm, Prm );	/* <S01E> */

	PrmChgSts = TuneLessCalculatePrm( Axis->BaseLoops,			/* <S066> */
									  Axis->UniPrms->Bprm,
									  &SetPrm,
									  &SpdCtrlPrm );			/* <S01E> */
	if( FALSE == PrmChgSts )
	{
		ALMSetServoAlarm( Axis->AlmManager, ALM_PRM_TRANSLATE );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn205 : マルチターンリミット設定 [Rev]														*/
/****************************************************************************************************//* <S004> */
PRM_RSLT pncal_mtlmt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
		pncal_m3_convert_mtLimit( Cmd, ArrayIdx, Axis, pValue );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn209 : 位置指令加減速バイアス																*//* <S065> */
/****************************************************************************************************/
PRM_RSLT pncal_expbias1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_SERVO_PCMD_FIL_USE == TRUE)
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	PcmdFilCalculatePrmExpFilBias( &(Axis->BaseControls->PcmdFil.ExpFil),
								   Prm->expbias1,
								   Axis->BaseControls->PosManager.conf.Egear.a,
								   Axis->BaseControls->PosManager.conf.Egear.b,
								   Bprm->SvCycleUs );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn20A : 外付けスケールピッチ数 [P/Rev]														*/
/****************************************************************************************************/
PRM_RSLT pncal_fcplsml(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	CHECK_FENCPOSERR	*ChkFposErr;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	ChkFposErr = &(Axis->CheckAlarm->OvrFPosErr);

	ChkFposErr->Egear = Axis->BaseControls->PosManager.conf.Egear;
	ChkFposErr->fcpgpls = Prm->fcplsml << Prm->MencP.BitnoDpoint.b.l;
	MlibPcalKegear( ChkFposErr->fcpgpls, Bprm->MotPulse, &ChkFposErr->sfgear );
	if( ( ChkFposErr->sfgear.a == 0 ) || ( ChkFposErr->sfgear.b == 0 ) )
	{
		return PRM_RSLT_CONDITION_ERR;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn20E : 電子ギア比(分子) [－]																*/
/****************************************************************************************************//* <S004> */
PRM_RSLT pncal_ratb2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	pncal_m3_convert_ratb( Cmd, ArrayIdx, Axis, pValue );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn210 : 電子ギア比(分母) [－]																*/
/****************************************************************************************************//* <S004> */
PRM_RSLT pncal_rata2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	pncal_m3_convert_rata( Cmd, ArrayIdx, Axis, pValue );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn212 : ＰＧ分周比(パルス単位) [P/Rev]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pgrat2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRM_RSLT	 rc;
#if 0
	DIVSET_ERROR err;

	err = RmxSencAdjustMencDivPulse(Axis->MencV, (Axis->UniPrms->Prm)->pgrat2l);
	if(err == DIVSET_SUCCESS)
	{
		rc = PRM_RSLT_SUCCESS;
	}
	else if(err == DIVSET_DIVERR)
	{
		rc = PRM_RSLT_CALC_ERR;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
#else
#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
	#if 0	/* <S149> */
	DIVSET_ERROR err;
	LONG data = *pValue;
	err = RmxSencAdjustMencDivPulse(Axis->UniPrms->Prm, Axis->MencV, Cmd, data);             
	if(err == DIVSET_SUCCESS)
	{
		rc = PRM_RSLT_SUCCESS;
	}
	else if(err == DIVSET_DIVERR)
	{
		rc = PRM_RSLT_CALC_ERR;
	}
	else
	{
		rc = PRM_RSLT_LIMIT_ERR;
	}
	#endif
	LONG data = *pValue;
	rc = RmxSencAdjustMencDivPulse(Axis->UniPrms->Prm, Axis->MencV, Cmd, data);   /* <S149> */
#else/*<S02D>*/
	rc = PRM_RSLT_SUCCESS;
#endif/*<S02D>*/
#endif
	return rc;
}

/****************************************************************************************************/
/*		Pn216 : 位置指令加減速時定数																*//* <S066> */
/****************************************************************************************************/
PRM_RSLT pncal_expfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_SERVO_PCMD_FIL_USE == TRUE)
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	PcmdFilCalculatePrmExpFilter( &(Axis->BaseControls->PcmdFil.ExpFil),
								  Prm->expfil,
								  Bprm->KMotSpdConv,
								  Bprm->MaxVel,
								  Bprm->SvCycleUs,
								  1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn217 : 位置指令移動平均時間																*//* <S066> */
/****************************************************************************************************/
PRM_RSLT pncal_mafil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_SERVO_PCMD_FIL_USE == TRUE)
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	PcmdFilCalculatePrmMoveAvergeFilter( &(Axis->BaseControls->PcmdFil.MaFil),
										 Prm->mafil,
										 Prm->highrad,//<S0C3>
										 Bprm->SvCycleUs,
										 1 );
	PcmdFilCalculatePrmHighFilter( &(Axis->BaseControls->PcmdFil.MaFil),//<S0C3>
								   Prm->mafil,
								   Prm->highrad,
								   Bprm->SvCycleUs,
								   1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn218 : 指令パルス入力倍率																	*/
/****************************************************************************************************/
PRM_RSLT pncal_mulfrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	AnlgV.MulFrq = (SHORT)*pValue;
#endif
	return PRM_RSLT_SUCCESS;
}

/***************************************************************************/
/*      Pn21B : 位置指令hifh filter radin                                    */
/***************************************************************************///<S0C3>
PRM_RSLT pncal_highrad(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if	(CSW_SERVO_PCMD_HIGH_FIL_USE == TRUE)
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	PcmdFilCalculatePrmMoveAvergeFilter( &(Axis->BaseControls->PcmdFil.MaFil),
										 Prm->mafil,
										 Prm->highrad,
										 Bprm->SvCycleUs,
										 1 );
	PcmdFilCalculatePrmHighFilter( &(Axis->BaseControls->PcmdFil.MaFil),
								   Prm->mafil,
								   Prm->highrad,
								   Bprm->SvCycleUs,
								   1 );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn231 : バックラッシ補正量 [0.1指令単位]													*/
/****************************************************************************************************/
PRM_RSLT pncal_blcm2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
	PRMDATA				*Prm;
	POS_MNG_HNDL		*PosManager;

	Prm = Axis->UniPrms->Prm;
	PosManager = &(Axis->BaseControls->PosManager);

	BlshCalcCompensateAmount( &(PosManager->BlshCmp),
							  &(PosManager->conf.Egear),
							  Prm->blcm2l );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn233 : バックラッシ補正時定数 [0.01ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_blfiltime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_SVCF_BACKLASHCOMP == TRUE)
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;
	POS_MNG_HNDL		*PosManager;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	PosManager = &(Axis->BaseControls->PosManager);

	BlshCalcCompensateTime( &(PosManager->BlshCmp),
							Bprm->SvCycleUs,
							Prm->blfiltime );
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn300 : 速度指令入力ゲイン																	*/
/****************************************************************************************************/
PRM_RSLT pncal_vrefgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
ANLGV *panlg = &AnlgV;										/* TODO : */
LONG RatSpd = Axis->UniPrms->Bprm->RatSpd;
LONG OvrSpd = Axis->UniPrms->Bprm->OvrSpd;
LONG Gadjv = Axis->UniPrms->Prm->gadjv;
LONG Vrefgn = Axis->UniPrms->Prm->vrefgn;
#if( SVFSEL_ADC_USE==1 )    /*  <S19A>	 */
	panlg->VrefKx = KpiPcalSpdRefAdGain( RatSpd, OvrSpd, Vrefgn, Gadjv );
#endif
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn301 : 内部設定速度1(回転)																	*/
/****************************************************************************************************/
PRM_RSLT pncal_speed1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* <S03D> Start */
	/* To Do : パラメータ設定値をスーパーグローバル変数に保持している為、設定値の保持場所について考慮する必要がる */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		if( Axis->MencV->LowSpdMotor )
		{/* Low Speed Motor */
//			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed1, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed1, 0 );	/* <S0CE> */

		}
		else
		{
//			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed1*10, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed1*10, 0 );	/* <S0CE> */
		}
	}
#endif
/* <S03D> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn302 : 内部設定速度2(回転)																	*/
/****************************************************************************************************/
PRM_RSLT pncal_speed2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* <S03D> Start */
	/* To Do : パラメータ設定値をスーパーグローバル変数に保持している為、設定値の保持場所について考慮する必要がる */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		if( Axis->MencV->LowSpdMotor )
		{/* Low Speed Motor */
//			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed2, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed2, 0 );	/* <S0CE> */
		}
		else
		{
//			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed2*10, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed2*10, 0 );	/* <S0CE> */
		}
	}
#endif
/* <S03D> End */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn303 : 内部設定速度3(回転)																	*/
/****************************************************************************************************/
PRM_RSLT pncal_speed3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* <S03D> Start */
	/* To Do : パラメータ設定値をスーパーグローバル変数に保持している為、設定値の保持場所について考慮する必要がる */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;
	//MENCV				*MencV;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		if( Axis->MencV->LowSpdMotor )
		{/* Low Speed Motor */
//			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed3, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed3, 0 );	/* <S0CE> */
		}
		else
		{
//			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed3*10, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed3*10, 0 );	/* <S0CE> */
		}
	}
#endif
/* <S03D> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn304 : ＪＯＧ速度(回転) [min-1]															*/
/****************************************************************************************************/
PRM_RSLT pncal_jogspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn305 : ソフトスタート加速時間 [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_sfsacc(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/****************************************************************************************************/
/*																									*/
/*		ソフトスタート加速量計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ソフトスタート加速時間で最大速度となるように、演算周期当りの							*/
/*			正規化速度変化量を計算します。正規化速度は0x01000000/ＯＳ速度です。						*/
/*																									*/
/*						10000 * 0x01000000 * ScanTime												*/
/*			dSpd = ---------------------------------------											*/
/*					 (PerOvrSpd+10000)	* 1000 * AccTimex											*/
/*																									*/
/*																									*/
/*			AccTimex [ms] :	加速時間																*/
/*			ScanTime [us] : 演算周期																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if (FLOAT_USE==TRUE)
	float fx;
#else
	LONG			kx,sx;
#endif /* FLOAT_USE */
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
#if (FLOAT_USE==TRUE)
	if( Prm->sfsacc == 0 )
	{
		//fx = Bprm->NorOvrSpd;
		fx = 0x01000000;
	}
	else
	{
		//fx = Bprm->NorOvrSpd * 10000.0F / (Bprm->PerOvrSpd + 10000.0F) * (float)(Bprm->SvCycleUs / ( Prm->sfsacc * 0.001 ));
		fx = (10000 * 0x01000000 / ( Bprm->PerOvrSpd + 10000 ))*Bprm->SvCycleUs/ ( 1000 * Prm->sfsacc );
	}
	Axis->BaseControls->CtrlCmdPrm.SpdSfsAcc = fx;

#else
	
	sx = 0;
	if( Prm->sfsacc == 0 )
	{
		kx = 0x01000000;
	}
	else
	{
		kx = MlibScalKxgain( 10000, 0x01000000, ( Bprm->PerOvrSpd + 10000 ), &sx,   0 );
		kx = MlibPcalKxgain( kx, Bprm->SvCycleUs, ( 1000 * Prm->sfsacc ), &sx, -24 );
	}

	Axis->BaseControls->CtrlCmdPrm.SpdSfsAcc = kx;
#endif /* FLOAT_USE */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn306 : ソフトスタート減速時間 [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_sfsdec(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/****************************************************************************************************/
/*																									*/
/*		ソフトスタート減速量計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : ソフトスタート減速時間で最大速度となるように、演算周期当りの							*/
/*			正規化速度変化量を計算します。正規化速度は0x01000000/ＯＳ速度です。						*/
/*																									*/
/*						10000 * 0x01000000 * ScanTime												*/
/*			dSpd = ---------------------------------------											*/
/*					 (PerOvrSpd+10000)	* 1000 * DecTimex											*/
/*																									*/
/*																									*/
/*			DecTimex [ms] :	減速時間																*/
/*			ScanTime [us] : 演算周期																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if (FLOAT_USE==TRUE)
	float fx;
#else
	LONG			kx,sx;
#endif /* FLOAT_USE */
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
#if (FLOAT_USE==TRUE)
	if( Prm->sfsacc == 0 )
	{
		fx = 0x01000000;
	}
	else
	{
		//fx = Bprm->NorOvrSpd * 10000.0F / (Bprm->PerOvrSpd + 10000.0F) * (float)(Bprm->SvCycleUs / ( Prm->sfsdec * 0.001));
		fx = 10000 * 0x01000000 / ( Bprm->PerOvrSpd + 10000 ) * Bprm->SvCycleUs / ( 1000 * Prm->sfsdec );
	}
	Axis->BaseControls->CtrlCmdPrm.SpdSfsDec = fx;

#else
	sx = 0;
	if( Prm->sfsdec == 0 )
	{
		kx = 0x01000000;
	}
	else
	{
		kx = MlibScalKxgain( 10000, 0x01000000, ( Bprm->PerOvrSpd + 10000 ), &sx,   0 );
		kx = MlibPcalKxgain( kx, Bprm->SvCycleUs, ( 1000 * Prm->sfsdec ), &sx, -24 );
	}

	Axis->BaseControls->CtrlCmdPrm.SpdSfsDec = kx;
#endif /* FLOAT_USE */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn307 : 速度指令フィルタ時定数 [0.01ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_srfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
ANLGV *pan = &AnlgV;	/* TODO: */
PRMDATA	*Prm;
BPRMDAT	*Bprm;
	Prm = Axis->UniPrms->Prm;
	Bprm =Axis->UniPrms->Bprm;

#if (FLOAT_USE==TRUE)
	pan->Kvreflpf = FlibPcalKf1gain( Prm->srfil*10, Bprm->SvCycleUs, 0 );
#else
	pan->Kvreflpf = MlibPcalKf1gain( Prm->srfil*10, Bprm->SvCycleUs, 0 );
#endif /* FLOAT_USE */
	
#else
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	
#if (FLOAT_USE==TRUE)
	Axis->BaseControls->CtrlCmdPrm.KVrfFil = FlibPcalKf1gain( Prm->srfil*10, Bprm->SvCycleUs, 0 );
#else
	Axis->BaseControls->CtrlCmdPrm.KVrfFil = MlibPcalKf1gain( Prm->srfil*10, Bprm->SvCycleUs, 0 );
#endif /* FLOAT_USE */
	
#endif
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn308 : 速度Ｆ/Ｂフィルタ時定数 [0.01ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_fbfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CTRL_CMD_PRM	*CtrlCmdPrm;
	BPRMDAT		*Bprm;								/* <S0DA> */
	PRMDATA		*Prm;

	CtrlCmdPrm	= &(Axis->BaseControls->CtrlCmdPrm);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;						/* <S0DA> */

#if (FLOAT_USE==TRUE)
//<S0DA>	CtrlCmdPrm->KVfbFil = FlibPcalKf1gain(( Prm->fbfil * 10 ), KPI_MBCYCLEUS, 0 );
	CtrlCmdPrm->KVfbFil = 	FlibPcalKf1gain( ((Prm->fbfil + Bprm->SpdFbLpassFil) * 10 ), 
								KPI_MBCYCLEUS, 
								0 );	/* <S0DA> */
#else
//<S0DA>	CtrlCmdPrm->KVfbFil = MlibPcalKf1gain(( Prm->fbfil * 10 ), KPI_MBCYCLEUS, 0 );
	CtrlCmdPrm->KVfbFil = 	MlibPcalKf1gain( ((Prm->fbfil + Bprm->SpdFbLpassFil) * 10 ), 
								KPI_MBCYCLEUS, 
								0  );	/* <S0DA> */
#endif /* FLOAT_USE */

	return PRM_RSLT_SUCCESS;
}

/* <S1F5> Start */
/****************************************************************************************************/
/*		Pn30A : 直線減速停止減速時間 [ms]															*/
/*			(ソフトスタート減速時間(Pn306)の処理関数pncal_sfsdecを参照)								*/
/****************************************************************************************************/
PRM_RSLT pncal_svoffdectime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/****************************************************************************************************/
/*																									*/
/*		直線減速停止時の減速量計算																	*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 直線減速停止時の減速時間で最大速度となるように、演算周期当りの							*/
/*			正規化速度変化量を計算します。正規化速度は0x01000000/ＯＳ速度です。						*/
/*																									*/
/*						10000 * 0x01000000 * ScanTime												*/
/*			dSpd = ---------------------------------------											*/
/*					 (PerOvrSpd+10000)	* 1000 * DecTimex											*/
/*																									*/
/*																									*/
/*			DecTimex [ms] :	減速時間																*/
/*			ScanTime [us] : 演算周期																*/
/*																									*/
/*																									*/
/****************************************************************************************************/
#if (FLOAT_USE==TRUE)
	float fx;
#else
	LONG			kx,sx;
#endif /* FLOAT_USE */
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
#if (FLOAT_USE==TRUE)
	if( Prm->svoffdectime == 0 )
	{
		fx = 0x01000000;
		Axis->BeSequence->LdstpTimeZero = TRUE;
	}
	else
	{
		fx = 10000 * 0x01000000 / ( Bprm->PerOvrSpd + 10000 ) * Bprm->SvCycleUs / ( 1000 * Prm->svoffdectime );
		Axis->BeSequence->LdstpTimeZero = FALSE;
	}
	Axis->BaseControls->CtrlCmdPrm.SpdLdstpDec = fx;

#else
	sx = 0;
	if( Prm->svoffdectime == 0 )
	{
		kx = 0x01000000;
		Axis->BeSequence->LdstpTimeZero = TRUE;
	}
	else
	{
		kx = MlibScalKxgain( 10000, 0x01000000, ( Bprm->PerOvrSpd + 10000 ), &sx,   0 );
		kx = MlibPcalKxgain( kx, Bprm->SvCycleUs, ( 1000 * Prm->svoffdectime ), &sx, -24 );
		Axis->BeSequence->LdstpTimeZero = FALSE;
	}

	Axis->BaseControls->CtrlCmdPrm.SpdLdstpDec = kx;
#endif /* FLOAT_USE */
	return PRM_RSLT_SUCCESS;
}
/* <S1F5> End */

/* <S1D0> Start */
/****************************************************************************************************/
/*		Pn30C : 速度フィードフォワード移動平均時間															*/
/****************************************************************************************************/
PRM_RSLT pncal_vffmafil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	PcmdFilCalculatePrmMoveAvergeFilter( &(Axis->BaseControls->VFFcmdFil),
										 Prm->vffmafil,
										 0,
										 Bprm->SvCycleUs,
										 1 );
	FFCmdFilInitialize( &(Axis->BaseControls->VFFcmdFil) );
	return PRM_RSLT_SUCCESS;
}
/* <S1D0> End */
/****************************************************************************************************/
/*		Pn310 : 振動検出スイッチ [－]																*/
/****************************************************************************************************/
PRM_RSLT pncal_vibsel(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	Axis->DetVib->conf.VibCheckSel = Axis->UniPrms->Prm->vibsel & 0x000F;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn311 : 振動検出感度 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_vibsens(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		Axis->DetVib->conf.VibCheckLevel =
			BprmSpeedLevelCal( Bprm, (30 * Prm->vibnorm_l * Prm->vibsens / 100), 0 );
	}
	else
	{ /* 回転型 */
		Axis->DetVib->conf.VibCheckLevel =
			BprmSpeedLevelCal( Bprm, (30 * Prm->vibnorm * Prm->vibsens / 100), 0 );
	}
	/* 設定レベルに変更があった場合も最大最小をクリアする */
	DetVibResetMaxMinSpeedError( Axis->DetVib );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn312 : 振動検出レベル(回転) [min-1]														*/
/****************************************************************************************************/
PRM_RSLT pncal_vibnorm(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		Axis->DetVib->conf.VibCheckLevel =
			BprmSpeedLevelCal( Bprm, ( 30 * Prm->vibnorm * Prm->vibsens / 100 ), 0 );
		/* 設定レベルに変更があった場合も最大最小をクリアする */
		DetVibResetMaxMinSpeedError( Axis->DetVib );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn324 : 慣性モーメント同定開始レベル [%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_Pn324(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn325 : 慣性モーメント同定中の振動検出レベル(回転) [min-1]									*/
/****************************************************************************************************/
PRM_RSLT pncal_Pn325(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		Axis->DetVib->conf.JstVibCheckLevel =
			BprmSpeedLevelCal( Bprm, (Prm->Pn325 * 10), 0 );
		Axis->DetVib->conf.JstVibCheckLevel <<= 3;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn380 : 内部設定速度1(リニア)																*/
/****************************************************************************************************/
PRM_RSLT pncal_speed1_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* <S03D> Start */
	/* To Do : パラメータ設定値をスーパーグローバル変数に保持している為、設定値の保持場所について考慮する必要がる */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		if( Axis->MencV->LowSpdMotor )
		{/* Low Speed Motor */
//			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed1_l, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed1_l, 0 );	/* <S0CE> */
		}
		else
		{
//			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed1_l*10, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed1_l*10, 0 );	/* <S0CE> */
		}
	}
#endif
/* <S03D> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn381 : 内部設定速度2(リニア)																*/
/****************************************************************************************************/
PRM_RSLT pncal_speed2_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* <S03D> Start */
	/* To Do : パラメータ設定値をスーパーグローバル変数に保持している為、設定値の保持場所について考慮する必要がる */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		if( Axis->MencV->LowSpdMotor )
		{/* Low Speed Motor */
//			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed2_l, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed2_l, 0 );	/* <S0CE> */
		}
		else
		{
//			panlg->InterSpdLevel[2] = BprmSpeedLevelCal( Bprm, Prm->speed2_l*10, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed2_l*10, 0 );	/* <S0CE> */
		}
	}
#endif
/* <S03D> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn382 : 内部設定速度3(リニア)																*/
/****************************************************************************************************/
PRM_RSLT pncal_speed3_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
/* <S03D> Start */
	/* To Do : パラメータ設定値をスーパーグローバル変数に保持している為、設定値の保持場所について考慮する必要がる */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	ANLGV *panlg = &AnlgV;
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{ /* リニア型 */
		if( Axis->MencV->LowSpdMotor )
		{/* Low Speed Motor */
//			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed3_l, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed3_l, 0 );	/* <S0CE> */
		}
		else
		{
//			panlg->InterSpdLevel[3] = BprmSpeedLevelCal( Bprm, Prm->speed3_l*10, 0 );	/* <S0CE> */
			panlg->InterSpdLevel[1] = BprmSpeedLevelCal( Bprm, Prm->speed3_l*10, 0 );	/* <S0CE> */
		}
	}
#endif
/* <S03D> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn400 : トルク指令入力ゲイン																*/
/****************************************************************************************************/
PRM_RSLT pncal_trefgn(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
ANLGV *pan = &AnlgV;	/* TODO: */
DBYTEX gadjtp = Axis->UniPrms->Prm->gadjtp_n;
LONG Trefgn = Axis->UniPrms->Prm->trefgn;
LONG PerMaxTrq = Axis->UniPrms->Bprm->PerMaxTrq;

PRMDATA	*Prm  = Axis->UniPrms->Prm;						/* <S18A> */
CADC	*pAdc = Axis->pAdc;								/* <S18A> */
	pAdc->P.ZadjTrqP = (CHAR)Prm->zadjtp_n.b.l;			/* <S18A> */
	pAdc->P.ZadjTrqN = (CHAR)Prm->zadjtp_n.b.h;			/* <S18A> */

#if(SVFSEL_ADC_USE==1)                          /* <S19A> */
	pan->TrefKxP = KpiPcalTrqRefAdGain( PerMaxTrq, Trefgn, gadjtp.b.l, CPCALADC_TREFP );
	pan->TrefKxN = KpiPcalTrqRefAdGain( PerMaxTrq, Trefgn, gadjtp.b.h, CPCALADC_TREFN );
#endif
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn401 : １段目第１トルク指令フィルタ時定数 [0.01ms]											*/
/****************************************************************************************************/
PRM_RSLT pncal_trqfil11(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA				*Prm;
	TUNELESS_SETPRM		SetPrm;
	BASE_SPDLOOP_PRM 	SpdCtrlPrm;	/* <S01E> */
	BOOL				PrmChgSts;

	Prm = Axis->UniPrms->Prm;

	/* トルクフィルタＬＰＦパラメータ計算	*/
	PcalBaseTrqLpassFilter( &(Axis->BaseLoops->TrqFil),
							Axis->BaseControls,
							Prm->trqfil11,
							Prm->trqfil11,
							1 );

	/* 調整レスパラメータ計算	*/
	SetPrm.DatLevel = Prm->DatLevel;			/* Pn170 : 調整レス関連スイッチ						*/
	SetPrm.trqfil11 = Prm->trqfil11;			/* Pn401 : 1段目第1トルク指令フィルタ時定数			*/
	SetPrm.ampw = Prm->ampw;					/* PnE12 : サーボパック容量							*/
	SetPrm.ignq = Prm->ignq;					/* PnE21 : 電流ループゲイン(Q)						*/
	SetPrm.kiq = Prm->kiq;						/* PnE23 : 電流ループ積分時定数(Q)					*/
	SetPrm.fbfil2 = Prm->fbfil2;				/* PnEA8 : 第2速度F/Bフィルタ時定数					*/
	SetPrm.TuneLessGain = Prm->TunLessGain;		/* PnEDA : 調整レス外乱オブザーバゲイン				*/
	SetPrm.TuneLessStiff = Prm->TunLessStiff;	/* PnEDB : 調整レスイナーシャ補正ゲイン				*/
	SetPrm.TuneLessTrqLpf = Prm->TunLessTrqLpf;	/* PnEDC : 調整レス外乱トルクローパスフィルタ時定数	*/
	SetPrm.TuneLessVfbLpf = Prm->TunLessVfbLpf;	/* PnEDD : 調整レス速度FBローパスフィルタ時定数		*/
	SetPrm.MencP = &Prm->MencP;					/* PnF00 - PnF1E : エンコーダパラメータ				*/
	SetPrm.DetVib = Axis->DetVib;
	SetPrm.ANotchSeq = Axis->ANotchSeq;

	PcalCopyBaseSpeedLoopParam( &SpdCtrlPrm, Prm );	/* <S01E> */

	TuneLessCalculatePrm( Axis->BaseLoops,
						  Axis->UniPrms->Bprm,
						  &SetPrm,
						  &SpdCtrlPrm );			/* <S01E> */

#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKlpfSet( Axis, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain));	/* calculate dKlpf */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain));	/* calculate dKlpf */
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn402 : 正転トルク制限 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_tlmtf(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		/* [%] --> [2^24/MaxTrq] */
		Axis->BaseControls->TrqLimitData.conf.FwIntTrqLmt =
				BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->tlmtf ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn403 : 逆転トルク制限 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_tlmtr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		/* [%] --> [2^24/MaxTrq] */
		Axis->BaseControls->TrqLimitData.conf.RvIntTrqLmt =
				BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->tlmtr ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn404 : 正転側外部トルク制限 [%]															*/
/****************************************************************************************************/
PRM_RSLT pncal_outlmf1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_pTrqLmt( Cmd, ArrayIdx, Axis, pValue );
#endif

	/* [%] --> [2^24/MaxTrq] */
	Axis->BaseControls->TrqLimitData.conf.FwExtTrqLmt =
			BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->outlmf1 ), 0 );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn405 : 逆転側外部トルク制限 [%]															*/
/****************************************************************************************************/
PRM_RSLT pncal_outlmr1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_nTrqLmt( Cmd, ArrayIdx, Axis, pValue );
#endif

	/* [%] --> [2^24/MaxTrq] */
	Axis->BaseControls->TrqLimitData.conf.RvExtTrqLmt =
			BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->outlmr1 ), 0 );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn406 : 非常停止トルク [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_emgtrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	/* [%] --> [2^24/MaxTrq] */
	Axis->BaseControls->TrqLimitData.conf.EstopTrqLmt =
			BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->emgtrq ), 0 );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn407 : トルク制御時の速度制限 [min-1]														*/
/****************************************************************************************************/
PRM_RSLT pncal_tcrlmt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		if( 0 != ((Prm->tfuncsw >> 4) & 0x0F) )	/*Pn408.1 : 速度制限選択設定 */
		{
			Axis->BaseControls->CtrlCmdPrm.TConSpdLmtLevel =
					BprmSpeedLevelCal( Bprm, ( Prm->tcrlmt * 10 ), 1 );
		}
		else
		{
			Axis->BaseControls->CtrlCmdPrm.TConSpdLmtLevel =
					BprmSpeedLevelCal( Bprm, ( Prm->tcrlmt * 10 ), 0 );
		}
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn408 : トルク関係機能スイッチ [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_tfuncsw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA			*Prm;
	USHORT			*CswData;
	LONG			PrmSetting;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;
	CswData = &(Axis->SvAsicRegs->MicroCsw.data);

/*--------------------------------------------------------------------------------------------------*/
/*		１段目ノッチフィルタ選択 (ASIC)																*/
/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->tfuncsw & 0x0F )
	{
#ifndef	_JL076_USE_
		case 0x00:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw), CswData, F1DSABL, FALSE );
			break;
		case 0x01:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw), CswData, F1DSABL, TRUE );
			break;
		default:
			ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber );
			break;
#else
	case 0x00:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->MREG_CSW), CswData, CSW_F1DSABL, FALSE );
		break;
	case 0x01:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->MREG_CSW), CswData, CSW_F1DSABL, TRUE );
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber );
		break;
#endif
	}

/*--------------------------------------------------------------------------------------------------*/
/*		２段目ノッチフィルタ選択 (CPU)																*/
/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->tfuncsw >> 8 ) & 0x0F )
	{
#ifndef	_JL076_USE_
	case 0x00:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw), CswData, F2DSABL, FALSE );
		break;
	case 0x01:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw), CswData, F2DSABL, TRUE );
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber );
		break;
#else
		case 0x00:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->MREG_CSW), CswData, CSW_F2DSABL, FALSE );
			break;
		case 0x01:
			KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->MREG_CSW), CswData, CSW_F2DSABL, TRUE );
			break;
		default:
			ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber );
			break;
#endif
	}

/*--------------------------------------------------------------------------------------------------*/
/*		外乱オブザーバ選択																			*/
/*--------------------------------------------------------------------------------------------------*/
	PrmSetting = DobsCalculatePrmSW( &(Axis->BaseControls->DobsCtrl.var.DobsAct),
									 Axis->BaseControls->TuneLessCtrl.conf.TuningLessUse,
									 (Prm->tfuncsw >> 12) );
	if( PrmSetting == PRM_SETTING_NG )
	{
		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn409 : １段目ノッチフィルタ周波数 [Hz]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ntchfil1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil1,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq1,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp1,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf11 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil1,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq1,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp1,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn40A : １段目ノッチフィルタＱ値 [0.01]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ntchq1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil1,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq1,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp1,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf11 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil1,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq1,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp1,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn40B : １段目ノッチフィルタ深さ [0.001]													*/
/****************************************************************************************************/
PRM_RSLT pncal_ntchdamp1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil1,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq1,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp1,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf11 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil1,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq1,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp1,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn40C : ２段目ノッチフィルタ周波数 [Hz]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ntchfil2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil2,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq2,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp2,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf21 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil2,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq2,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp2,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF21 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn40D : ２段目ノッチフィルタＱ値 [0.01]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ntchq2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil2,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq2,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp2,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf21 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil2,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq2,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp2,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF21 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn40E : ２段目ノッチフィルタ深さ [0.001]													*/
/****************************************************************************************************/
PRM_RSLT pncal_ntchdamp2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil2,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq2,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp2,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf21 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil2,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq2,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp2,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF21 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn40F : ２段目２次トルク指令フィルタ周波数 [Hz]												*/
/****************************************************************************************************/
PRM_RSLT pncal_secfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	TuneLessFil;		/* <S1F0> */
	LONG	TrqRefFil;			/* <S1F0> */

	Prm = Axis->UniPrms->Prm;

	if( (TuneLessGetTuneLessUse(&Axis->BaseControls->TuneLessCtrl) == TRUE)			/* TuningLessUse*//* <S1C7> */
		&& (TuneLessGetTuneLessModeEx(&Axis->BaseControls->TuneLessCtrl) == TRUE) ) /* TuningLessEx	*/
	{ /* 新調整レス有効時 */
		TuneLessFil = 3000;
	}
	else
	{
		TuneLessFil = 0;
	}

	if( Prm->secfil >= 5000 )
	{ /* フィルタ周波数 >= 5000Hz */
		TrqRefFil = TuneLessFil;
	}
	else
	{
		TrqRefFil = MlibAbs32( TuneLessFil - Prm->secfil );
	}


	if( (TrqRefFil >= 5000) || (TrqRefFil < 100) )
	{ /* フィルタ周波数 >= 5000Hz */
		/* 2次遅れフィルタ無効 */
		Axis->BaseControls->CtrlCmdPrm.LpassFil2 = FALSE;
	}

	/* トルクフィルタLPF2パラメータ計算 */
	PcalBaseTrqLpassFilter2( &(Axis->BaseLoops->TrqFil),
								 Axis->BaseControls,
//								 Prm->secfil,		/* <S1F0> */
								 TrqRefFil,			/* <S1F0> */
								 Prm->secq );

	if( (TrqRefFil < 5000) && (TrqRefFil >= 100) )
		{ /* フィルタ周波数 < 5000Hz */
			/* 2次遅れフィルタ有効 */
			Axis->BaseControls->CtrlCmdPrm.LpassFil2 = TRUE;
		}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn410 : ２段目２次トルク指令フィルタＱ値 [0.01]												*/
/****************************************************************************************************/
PRM_RSLT pncal_secq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;
	PcalBaseTrqLpassFilter2( &(Axis->BaseLoops->TrqFil),
							 Axis->BaseControls,
							 Prm->secfil,
							 Prm->secq );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn412 : １段目第２トルク指令フィルタ時定数 [0.01ms]											*/
/****************************************************************************************************/
PRM_RSLT pncal_trqfil12(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;

	/* トルクフィルタＬＰＦパラメータ計算	*/
	PcalBaseTrqLpassFilter( &(Axis->BaseLoops->TrqFil),
							Axis->BaseControls,
							Prm->trqfil11,
							Prm->trqfil12,
							2 );
#if 0 /* 2010.11.10 Y.Oka 自動ゲイン切り替え時のスムージング未対応のためコメントアウト */
	PcalAtGseldKlpfSet( Axis, KPI_SBCYCLEUS );
#endif
	{															
		ATGSEL	*pAtGsel;										/* <S09B> */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselA);	/* condition A */
		PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain));	/* calculate dKlpf */
		pAtGsel = &(Axis->BaseControls->GainChange.AtGselB);	/* condition B */
		PcalAtGseldKlpfSet( pAtGsel, &(pAtGsel->conf.dGain));	/* calculate dKlpf */
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn415 : T-REFフィルタ時定数																	*/
/****************************************************************************************************/
PRM_RSLT pncal_treffil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	/* 2012.08.06 Y.Oka Make Mercury Base Software */
	/* ★現状未対応★ */
ANLGV *pan = &AnlgV;	/* TODO: */
BPRMDAT	*Bprm;
PRMDATA	*Prm;			/* <S083> */
TRQFIL *TrqFil;			/* <S083> */

	Bprm =Axis->UniPrms->Bprm;
	Prm =Axis->UniPrms->Prm;													/* <S083> */
	pan->Ktreflpf = MlibPcalKf1gain( (Prm->treffil)*10, Bprm->SvCycleUs, 0 ); 	/* <S083> */

 	TrqFil = &(Axis->BaseLoops->TrqFil);										/* <S083> */
	TrqFil->A.Ktrflpf = MlibPcalKf1gain( (Prm->treffil)*10, KPI_MBCYCLEUS, 0 );	/* <S083> */
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn416 : トルク(推力)関係機能スイッチ２											<S16F>		*/
/****************************************************************************************************/
PRM_RSLT pncal_tfuncsw2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA			*Prm;
	USHORT			*CswData2;
//	LONG			PrmSetting;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;
	CswData2 = &(Axis->SvAsicRegs->MicroCsw2.data);

	/*--------------------------------------------------------------------------------------------------*/
	/*		3段目ノッチフィルタ選択 (ASIC)												<Sxxx>			*/
	/*--------------------------------------------------------------------------------------------------*/
	switch( Prm->tfuncsw2 & 0x0F )
	{
	case 0x00:
		KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw2), CswData2, F4DSABL, FALSE );
		break;
	case 0x01:
		KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw2), CswData2, F4DSABL, TRUE );
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw2.RegNumber );
		break;
	}

	/*--------------------------------------------------------------------------------------------------*/
	/*		4段目ノッチフィルタ選択 (ASIC)												<Sxxx>			*/
	/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->tfuncsw2 >> 4 ) & 0x0F )
	{
	case 0x00:
		KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw2), CswData2, F5DSABL, FALSE );
		break;
	case 0x01:
		KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw2), CswData2, F5DSABL, TRUE );
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw2.RegNumber );
		break;
	}
	/*--------------------------------------------------------------------------------------------------*/
	/*		5段目ノッチフィルタ選択 (ASIC)												<Sxxx>			*/
	/*--------------------------------------------------------------------------------------------------*/
	switch(( Prm->tfuncsw2 >> 8 ) & 0x0F )
	{
	case 0x00:
		KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw2), CswData2, F6DSABL, FALSE );
		break;
	case 0x01:
		KpiManageMicroTorqueFilter( (VUSHORT*)&(AsicMpAdr->IntAdP.CtrlSw2), CswData2, F6DSABL, TRUE );
		break;
	default:
		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw2.RegNumber );
		break;
	}

	return PRM_RSLT_SUCCESS;
}

/* <S0A6> Start */
/****************************************************************************************************/
/*		Pn417 : 3段目ノッチフィルタ周波数 [Hz]														*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchfil3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil3,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq3,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp3,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf41 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil3,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq3,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp3,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn418 : 3段目ノッチフィルタＱ値 [0.01]														*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchq3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil3,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq3,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp3,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf41 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil3,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq3,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp3,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn419 : 3段目ノッチフィルタ深さ [0.001]													*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchdamp3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil3,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq3,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp3,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf41 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil3,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq3,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp3,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn41A : 4段目ノッチフィルタ周波数 [Hz]														*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchfil4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil4,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq4,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp4,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf51 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil4,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq4,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp4,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn41B : 4段目ノッチフィルタＱ値 [0.01]														*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchq4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil4,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq4,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp4,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf51 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil4,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq4,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp4,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn41C : 4段目ノッチフィルタ深さ [0.001]													*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchdamp4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil4,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq4,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp4,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf51 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil4,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq4,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp4,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn41D : 5段目ノッチフィルタ周波数 [Hz]														*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchfil5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil5,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq5,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp5,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf61 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil5,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq5,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp5,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn41E: 5段目ノッチフィルタＱ値 [0.01]														*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchq5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil5,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq5,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp5,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf61 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil5,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq5,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp5,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn41F : 5段目ノッチフィルタ深さ [0.001]													*/
/****************************************************************************************************///<Sxxx>
PRM_RSLT pncal_ntchdamp5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;

#ifndef	_JL076_USE_
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil5,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq5,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp5,				/* フィルタ深さ			[0.001]			*/
							  &AsicMpAdr->IntAdP.Kf61 );	/* フィルタパラメータ設定先頭アドレス	*/
#else
	KpiPcalMicroNotchFilter2( Axis->SvAsicRegs,				/* JL-076アクセス用構造体				*/
							  Prm->ntchfil5,				/* フィルタ周波数		[Hz]			*/
							  Prm->ntchq5,					/* フィルタＱ定数		[0.01]			*/
							  Prm->ntchdamp5,				/* フィルタ深さ			[0.001]			*/
							  (SHORT*)&AsicMpAdr->MREG_KF1 );	/* フィルタパラメータ設定先頭アドレス	*/
#endif
	return PRM_RSLT_SUCCESS;
}
/* <S0A6> End */

/* <S14B> Start */
/****************************************************************************************************/
/*		Pn423 : 速度リップル補償機能選択スイッチ 		[－]										*/
/****************************************************************************************************/
/*																									*/
/*		Pn423.0 : 速度リップル補償機能の使用／不使用の選択				（オンライン）				*/
/*		Pn423.1 : 速度リップル補償情報不一致ワーニング(A.942)検出の選択	（オフライン）				*/
/*		Pn423.2 : 速度リップル補償機能の有効化しきい値条件の選択		（オフライン）				*/
/*		Pn423.3 : 速度リップル補償機能位相選択（非公開）				（オフライン）				*/
/*																									*/
/****************************************************************************************************/
PRM_RSLT pncal_RippleCompSW(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	ALMMAN_HANDLE	*AlmManager;	/* <S1FD> */
	PRMDATA			*Prm;
	RPLCMP			*RippleCmp;
	ULONG			MotorSnReadRslt;

//	AlmManager	= Axis->AlmManager;	/* <S1FD> */
	Prm			= Axis->UniPrms->Prm;
	RippleCmp	= &(Axis->BaseLoops->RippleCmp);

/*--------------------------------------------------------------------------------------------------*/
/*	Pn423.0 : 速度リップル補償機能の使用／不使用の選択												*/
/*--------------------------------------------------------------------------------------------------*/
	if( (Prm->RippleCompSw & 0x0F) == 0x00 )
	{
		/* 速度リップル補償情報状態 : 正常 */
		RippleCmp->var.RippleWrnState = TRUE;

		/* Pn423.0 = 0 : 速度リップル補償機能を使用しない */
		RippleCmp->RplCmpPrm.RplCmpUse = FALSE;
	}
	else
	{
		/* 今回取得モータシリアルNo.、前回取得モータシリアルNo. */
		MotorSnReadRslt = ChkMotorSerialNo( Axis->AxisNo, Axis->MencV );
//		if( MotorSnReadRslt != 0 )/*<S1FD>*/
		if( (MotorSnReadRslt != 0) && (RippleCmp->RplCmpPrm.RippleWrnMode == FALSE) )
		{	/* モータシリアルNo.が異なる && Pn423.1=0 のとき */
//			ALMSetServoAlarm( AlmManager, WRN_RIPPLE );
			/* 速度リップル補償情報状態 : 不一致(A.942) */
			RippleCmp->var.RippleWrnState = FALSE;

			/* Pn423.0 = 0 : 速度リップル補償機能を使用しない */
			RippleCmp->RplCmpPrm.RplCmpUse = FALSE;
		}
		else
		{
			/* 速度リップル補償情報状態 : 正常 */
			RippleCmp->var.RippleWrnState = TRUE;

			/* Pn423.0 = 1 : 速度リップル補償機能を使用する*/
			RippleCmp->RplCmpPrm.RplCmpUse = TRUE;
		}
	}

	return PRM_RSLT_SUCCESS;

}
/* <S14B> End */

/****************************************************************************************************/
/*		Pn424, Pn425 : 主回路電圧降下時トルク制限 [%]												*/
/****************************************************************************************************/
PRM_RSLT pncal_UvTrqLmt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG			lwrk;
	LONG			sx;
	UV_TLIM_DATA	*UvTrqLimits;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;

	UvTrqLimits = &(Axis->BaseControls->TrqLimitData.UvTrqLimits);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	/* [%] --> [2^24/MaxTrq]	*/
	UvTrqLimits->conf.UvTLimSetting = BprmTorqueLevelCal( Bprm, 100 * Prm->UvTrqLmt, 0 );
	lwrk = NORMAXVALUE;

	/* [ms] -> [ScanB回数] */
	UvTrqLimits->conf.UvTrqLmtOffFil = Prm->UvTrqLmtOffFil*(1000/Bprm->SvCycleUs);

	if( Prm->UvTrqLmtOffFil != 0 )
	{
		sx = 0;
		UvTrqLimits->conf.DeltaUvTrqLmt =
			MlibScalKxgain( (LONG)(lwrk - UvTrqLimits->conf.UvTLimSetting),
							1, UvTrqLimits->conf.UvTrqLmtOffFil, &sx, -24 );

		if( (UvTrqLimits->conf.DeltaUvTrqLmt == 0) &&
			(Prm->UvTrqLmt != pndef_UvTrqLmt.UpperLimit) )
		{
			UvTrqLimits->conf.DeltaUvTrqLmt = 1;
		}
	}
	else
	{
		UvTrqLimits->conf.DeltaUvTrqLmt = (LONG)(lwrk - UvTrqLimits->conf.UvTLimSetting);
	}

	return PRM_RSLT_SUCCESS;
}
/* <S1D0> Start */
/****************************************************************************************************/
/*		Pn426 : トルクフィードフォワード移動平均時間															*/
/****************************************************************************************************/
PRM_RSLT pncal_tffmafil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA				*Prm;
	BPRMDAT				*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	PcmdFilCalculatePrmMoveAvergeFilter( &(Axis->BaseControls->TFFcmdFil),
										 Prm->tffmafil,
										 0,
										 Bprm->SvCycleUs,
										 1 );
	FFCmdFilInitialize( &(Axis->BaseControls->TFFcmdFil) );
	return PRM_RSLT_SUCCESS;
}
/* <S1D0> End */
/* <S14B> Start */
/****************************************************************************************************/
/*		Pn427 : 速度リップル補償有効速度 		[min-1]												*/
/****************************************************************************************************/
PRM_RSLT pncal_VelRippleCompSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	RPLCMP		*RippleCmp;

	Prm			= Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	RippleCmp	= &(Axis->BaseLoops->RippleCmp);

	/* Speed [0.1r/min, 0.1mm/s] --> NormalizedSpeed [2^24/OvrSpd] */
	RippleCmp->VelRippleCompSpd = BprmSpeedLevelCal( Bprm,10 *  Prm->VelRippleCompSpd, 0 );

	return PRM_RSLT_SUCCESS;

}
/* <S14B> End */

/****************************************************************************************************/
/*		Pn460 : ノッチフィルタ調整スイッチ [－]														*/
/****************************************************************************************************/
PRM_RSLT pncal_anotchsw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn470 : grivate torque 													<S0CD>*/
/****************************************************************************************************/
PRM_RSLT pncal_gcomptrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;

	BPRMDAT			*Bprm;
	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

	Axis->BaseControls->FrictionM.GrvTrq = BprmTorqueLevelCal(Bprm,(10*Prm->gcomptrq),0);

	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn471 : Cullenfor torque 													<S0CD>*/
/****************************************************************************************************/
PRM_RSLT pncal_pqfriqcomptrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;
	/* [%] --> [2^24/MaxTrq] */
	Axis->BaseControls->FrictionM.PosCTrq = BprmTorqueLevelCal(Bprm,(10*Prm->pqfriqcomptrq),0);

	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn472 : Cullunneg torque  													<S0CD>*/
/****************************************************************************************************/
PRM_RSLT pncal_nqfriqcomptrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT	 *Bprm;
	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;
	Axis->BaseControls->FrictionM.NegCTrq = -BprmTorqueLevelCal(Bprm,(10*Prm->nqfriqcomptrq),0);

	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn473 : viscocity torque  													<S0CD>*/
/****************************************************************************************************/
PRM_RSLT pncal_vfrigcomptrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	 *Prm;
	BPRMDAT	  *Bprm;
	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;
	Axis->BaseControls->FrictionM.VisTrq = BprmTorqueLevelCal(Bprm,(10*Prm->vfrigcomptrq),0);

	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn474 : 内部設定速度hysteresis 														<S0CD>*/
/****************************************************************************************************/
PRM_RSLT pncal_spdhys(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	 *Prm;
	BPRMDAT	  *Bprm;
	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;
	Axis->BaseControls->FrictionM.SpdHys = BprmSpeedLevelCal(Bprm,(10*Prm->spdhys), 0);
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn501 : ゼロクランプレベル [min-1]															*/
/****************************************************************************************************/
PRM_RSLT pncal_clamp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;
//	GPRMDAT	*Gprm;
ZC_CTRL_DATA *pzc;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	//Gprm = Axis->UniPrms->gprm_ptr;
	pzc = &(Axis->BeSequence->ZcCtrlData);

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		//Gprm->ZclampSpdLevel =
		pzc->P.ZcSpdLevel = 												/* <S00C> */
			BprmSpeedLevelCal( Bprm, ( Prm->clamp * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn502 : 回転検出レベル [min-1]																*/
/****************************************************************************************************/
PRM_RSLT pncal_tgonlv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )/* <S02B> */
	{ /* 回転型 */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
		pncal_m3_convert_zSpdLv( Cmd, ArrayIdx, Axis, pValue );
#endif
		Axis->SeqMotSts->conf.TgonSpdLevel =
				BprmSpeedLevelCal( Bprm, ( Prm->tgonlv * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn503 : 速度一致信号出力幅 [min-1]															*/
/****************************************************************************************************/
PRM_RSLT pncal_vcmplv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )/* <S02B> */
	{ /* 回転型 */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
		pncal_m3_convert_vCmpLv( Cmd, ArrayIdx, Axis, pValue );
#endif
		Axis->SeqMotSts->conf.VcmpSpdLevel =
				BprmSpeedLevelCal( Bprm, ( Prm->vcmplv * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn506 : ブレーキ指令－サーボオフ遅れ時間 [10ms]												*/
/****************************************************************************************************/
PRM_RSLT pncal_brktim(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ULONG			ulwk;
	PRMDATA			*Prm;
	BK_SEQ_DATA		*BkSeqData;

	Prm = Axis->UniPrms->Prm;
	BkSeqData = &(Axis->BeSequence->BkSeqData);

	ulwk = 10 * Prm->brktim;
	if( ulwk <= 500 )
	{
		BkSeqData->conf.BkonDelayTime = (USHORT)ulwk;
	}
	else
	{
		ALMSetPramError( Axis->AlmManager, pndef_brktim.RegNumber );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn507 : ブレーキ指令出力速度レベル [min-1]													*/
/****************************************************************************************************/
PRM_RSLT pncal_brkspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	BK_SEQ_DATA		*BkSeqData;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	BkSeqData = &(Axis->BeSequence->BkSeqData);

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		BkSeqData->conf.BkonSpdLevel =
			BprmSpeedLevelCal( Bprm, ( Prm->brkspd * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn508 : サーボオフ－ブレーキ指令待ち時間 [10ms]												*/
/****************************************************************************************************/
PRM_RSLT pncal_brkwai(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ULONG			ulwk;
	PRMDATA			*Prm;
	BK_SEQ_DATA		*BkSeqData;

	Prm = Axis->UniPrms->Prm;
	BkSeqData = &(Axis->BeSequence->BkSeqData);

	ulwk = 10 * Prm->brkwai;
	if( ulwk <= 1000 )
	{
		BkSeqData->conf.BkonWaitTime = (USHORT)ulwk;
	}
	else
	{
		ALMSetPramError( Axis->AlmManager, pndef_brkwai.RegNumber );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn509 : 瞬停保持時間 [ms]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_acoftm(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ULONG	ulwk;

	ulwk = (Axis->UniPrms->Cprm)->acoftm + 16;
	if( ulwk >= 0xFFF0 )
	{
		Axis->PowerManager->MainPowerChecker.conf.AcoffDetctTime = 16;
		ALMSetPramError( Axis->AlmManager, pndef_acoftm.RegNumber );
	}
	else
	{
		Axis->PowerManager->MainPowerChecker.conf.AcoffDetctTime = ulwk;

	}
	/* <S21D> Start */
	if( (((Axis->UniPrms->Prm)->b_prm1 >> 8) & 0x0FU) == (USHORT)TRUE )		/* DC電源入力モード			*/
	{
		Axis->PowerManager->MainPowerChecker.conf.AcoffDetctTime += DCOFF_FILTER;
	}
	/* <S21D> End */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn50A : 入力信号選択１ [－]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_insel1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_insel1( Cmd, ArrayIdx, Axis, pValue );
#endif
	return( PRM_RSLT_SUCCESS );
}

/****************************************************************************************************/
/*		Pn50B : 入力信号選択２ [－]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_insel2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_insel2( Cmd, ArrayIdx, Axis, pValue );
#endif
	return( PRM_RSLT_SUCCESS );
}

/****************************************************************************************************/
/*		Pn518 : 出力信号選択７ [－]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_outsel7(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn51B : モータ-負荷位置間偏差過大検出レベル [指令単位]										*/
/****************************************************************************************************/
PRM_RSLT pncal_fpgoverlv2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	CHECK_FENCPOSERR	*ChkFposErr;

	Prm = Axis->UniPrms->Prm;
	ChkFposErr = &(Axis->CheckAlarm->OvrFPosErr);

	ChkFposErr->fpgoverLevel = Prm->fpgoverlv2l;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn51E : 位置偏差過大ワーニングレベル [%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_overwrnlv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	Axis->CheckAlarm->OvrPosErr.PerWrnLevel =
			MlibScalKxgain( Axis->CheckAlarm->OvrPosErr.PerAlmLevel, Prm->overwrnlv, 100, NULL, -30 );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn520 : 位置偏差過大アラームレベル [指令単位]												*/
/****************************************************************************************************/
PRM_RSLT pncal_overlv2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	Axis->CheckAlarm->OvrPosErr.PerAlmLevel = Prm->overlv2l;
	Axis->CheckAlarm->OvrPosErr.PerWrnLevel =
			MlibScalKxgain( Axis->CheckAlarm->OvrPosErr.PerAlmLevel, Prm->overwrnlv, 100, NULL, -30 );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn522 : 位置決め完了幅 [指令単位]															*/
/****************************************************************************************************/
PRM_RSLT pncal_coinlv2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)||(CSW_NETIF_TYPE == NETIF_M2_TYPE)/* <S004> */
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
#endif

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_psetWidth( Cmd, ArrayIdx, Axis, pValue );
#elif (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	(&MtHdl)->psnHdl.Prm->psetWidth = Prm->coinlv2l;				/* <S00C> */
#endif

#if 0 /* 2011.01.06 Y.Oka 位置偏差を指令単位で作成するよう変更したため、以下不要 */
	/* 20100826 Y.Oka 位置偏差過大はパルス単位で検出するよう変更 */
	/* CoinLevel   Upper Limit : 0x7FFFFFFF, Lower Limit : 0 */
	Axis->hPosMng->conf.CoinLevel = MlibDiv6432( Prm->coinlv2l, Prm->ratb2l, Prm->rata2l );
	if( (Axis->hPosMng->conf.CoinLevel == 0) && (Prm->coinlv2l != 0) )
	{ /* 電子ギア設定により意図せずにCOINレベルが0となった場合 */
		Axis->hPosMng->conf.CoinLevel = 1;
	}
#endif
	Axis->BaseControls->PosManager.conf.CoinLevel = Prm->coinlv2l;
	RemVibCalculateMonitorPrm( Axis->RemVibFreq,
	                           &(Axis->BaseControls->PosManager.conf.Egear),
	                           Prm->remdetw,
	                           Prm->coinlv2l,
	                           Bprm->SvCycleUs );
	StlgCalculateOverShootErrLevel( &(Axis->BaseControls->SettlingTime), Prm->coinlv2l, Prm->ovserrdetlvl );

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)||(CSW_NETIF_TYPE == NETIF_M2_TYPE)/* <S004> */
	/*---- MECHATROLINK用変数書込み -----*/
	regHdl->mtPrm->PsnPrm.psetWidth = Prm->coinlv2l;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn524 : ＮＥＡＲ信号幅 [指令単位]															*/
/****************************************************************************************************/
PRM_RSLT pncal_nearlv2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)||(CSW_NETIF_TYPE == NETIF_M2_TYPE)/* <S004> */
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
#endif

	Prm = Axis->UniPrms->Prm;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)/* <S004> */
	pncal_m3_convert_nearWidth( Cmd, ArrayIdx, Axis, pValue );
#endif

#if 0 /* 2011.01.06 Y.Oka 位置偏差を指令単位で作成するよう変更したため、以下不要 */
	/* 20100826 Y.Oka 位置偏差過大はパルス単位で検出するよう変更 */
	/* NearLevel   Upper Limit : 0x7FFFFFFF, Lower Limit : 1 */
	Axis->hPosMng->conf.NearLevel = MlibDiv6432( Prm->nearlv2l, Prm->ratb2l, Prm->rata2l );
	if( Axis->hPosMng->conf.NearLevel == 0 )
	{ /* 最小設定単位 */
		Axis->hPosMng->conf.CoinLevel = 1;
	}
#endif
	Axis->BaseControls->PosManager.conf.NearLevel = Prm->nearlv2l;

#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)||(CSW_NETIF_TYPE == NETIF_M2_TYPE)/* <S004> */
	/*----通信IF用変数書込み （CoEは？）-----*/
	regHdl->mtPrm->PsnPrm.nearWidth = Prm->nearlv2l;
#elif(CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE)
	/* TODO たじくか */
	(&MtHdl)->psnHdl.Prm->nearWidth = Prm->nearlv2l;
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn526 : サーボオン時位置偏差過大アラームレベル [指令単位]									*/
/****************************************************************************************************/
PRM_RSLT pncal_svonoverlv2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	Axis->CheckAlarm->OvrPosErr.SvonPerAlmLevel = Prm->svonoverlv2l;
	Axis->CheckAlarm->OvrPosErr.SvonPerWrnLevel =
		MlibScalKxgain( Axis->CheckAlarm->OvrPosErr.SvonPerAlmLevel, Prm->svonoverwrnlv, 100, NULL, -30 );

	return PRM_RSLT_SUCCESS;

}

/****************************************************************************************************/
/*		Pn528 : サーボオン時位置偏差過大ワーニングレベル [%]										*/
/****************************************************************************************************/
PRM_RSLT pncal_svonoverwrnlv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	Axis->CheckAlarm->OvrPosErr.SvonPerWrnLevel =
		MlibScalKxgain( Axis->CheckAlarm->OvrPosErr.SvonPerAlmLevel, Prm->svonoverwrnlv, 100, NULL, -30 );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn529 : サーボオン時速度制限レベル [min-1]													*/
/****************************************************************************************************/
PRM_RSLT pncal_svonspdlmtlv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CTRL_CMD_PRM	*CtrlCmdPrm;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;

	CtrlCmdPrm	= &(Axis->BaseControls->CtrlCmdPrm);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_ROTARY )
	{ /* 回転型 */
		CtrlCmdPrm->SvonSpdLmtLevel =
			BprmSpeedLevelCal( Bprm, ( Prm->svonspdlmtlv * 10 ), 0 );

		if( CtrlCmdPrm->SvonSpdLmtLevel < 10 )
		{
			CtrlCmdPrm->SvonSpdLmtValue = 1;
		}
		else
		{
			CtrlCmdPrm->SvonSpdLmtValue = CtrlCmdPrm->SvonSpdLmtLevel / 10;
		}
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn52A : フルクローズ1回転当たりの乗算値 [%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_fpgcutrate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG 				sx;
	PRMDATA				*Prm;
	CHECK_FENCPOSERR	*ChkFposErr;

	Prm = Axis->UniPrms->Prm;
	ChkFposErr = &(Axis->CheckAlarm->OvrFPosErr);

	sx = 0;
	ChkFposErr->fpgcutratelv = MlibPcalKxgain( Prm->fpgcutrate, 1, 100, &sx, 24 );
	ChkFposErr->fpgcutrate = Prm->fpgcutrate;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn530 : プログラムＪＯＧ運転関係スイッチ [－]												*/
/****************************************************************************************************/
PRM_RSLT pncal_pjogsw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogSw( Axis );						/* プログラムＪＯＧパラメータ計算	*/
			return PRM_RSLT_SUCCESS;
		}
}
/****************************************************************************************************/
/*		Pn531 : プログラムＪＯＧ移動距離 [指令単位]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pjogdistl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogDistance( Axis );				/* プログラムＪＯＧパラメータ計算	*/
			return PRM_RSLT_SUCCESS;
		}
}
/****************************************************************************************************/
/*		Pn533 : プログラムＪＯＧ移動速度 [min-1]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pjogrotspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogRotspd( Axis );					/* プログラムＪＯＧパラメータ計算	*/
			return PRM_RSLT_SUCCESS;
		}
}
/****************************************************************************************************/
/*		Pn534 : プログラムＪＯＧ加減速時間 [ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pjogacctm(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogAcctime( Axis );				/* プログラムＪＯＧパラメータ計算	*/
			return PRM_RSLT_SUCCESS;
		}
}
/****************************************************************************************************/
/*		Pn535 : プログラムＪＯＧ待ち時間 [ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pjogwaittm(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogWaitTime( Axis );				/* プログラムＪＯＧパラメータ計算	*/
			return PRM_RSLT_SUCCESS;
		}
}
/****************************************************************************************************/
/*		Pn536 : プログラムＪＯＧ移動回数 [回]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pjognum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogNum( Axis );				/* プログラムＪＯＧパラメータ計算	*/
			return PRM_RSLT_SUCCESS;
		}
}

/****************************************************************************************************/
/*		Pn52B : 過負荷ワーニングレベル [%]															*/
/****************************************************************************************************/
PRM_RSLT pncal_olwarlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalOverLoadWarLevelAmpMot( Axis );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn550 : アナログモニタ１オフセット電圧 [0.1V]												*/
/****************************************************************************************************/
PRM_RSLT pncal_monoffset1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorOffset( Axis, 0 );		/* アナログモニタ出力オフセット計算		*//* <S016> */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn551 : アナログモニタ２オフセット電圧 [0.1V]												*/
/****************************************************************************************************/
PRM_RSLT pncal_monoffset2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorOffset( Axis, 1 );		/* アナログモニタ出力オフセット計算		*//* <S016> */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn552 : アナログモニタ1倍率 [0.01倍]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mongain1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 0 );		/* アナログモニタ出力ゲイン計算		*//* <S016> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn553 : アナログモニタ2倍率 [0.01倍]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mongain2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 1 );		/* アナログモニタ出力ゲイン計算		*//* <S016> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn560 : 残留振動検出幅 [0.1%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_remdetw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Bprm = Axis->UniPrms->Bprm;
	Prm = Axis->UniPrms->Prm;

	RemVibCalculateMonitorPrm( Axis->RemVibFreq,
	                           &(Axis->BaseControls->PosManager.conf.Egear),
	                           Prm->remdetw,
	                           Prm->coinlv2l,
	                           Bprm->SvCycleUs );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn561 : オーバーシュート検出レベル [%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ovserrdetlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;
	StlgCalculateOverShootErrLevel( &(Axis->BaseControls->SettlingTime), Prm->coinlv2l, Prm->ovserrdetlvl );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn600 : 回生抵抗容量 [10W]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_regres(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CMN_PRMDATA	*Cprm;
	Cprm = Axis->UniPrms->Cprm;
	/* 回生過負荷レベルの計算						*/
	if(FALSE != PcalRegeneOverLoadLevel( &(Axis->PowerManager->RegeneCtrl),
	                                     Cprm->regres, Cprm->wre, Cprm->ConvW ))
	{
		ALMSetPramError( Axis->AlmManager, pndef_regres.RegNumber );
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn601 : ＤＢ抵抗容量 [10W]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_dbres(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalDbrakeOverLoadLevel( Axis );				/* DB過負荷レベルの計算							*/
	return PRM_RSLT_SUCCESS;
}

/* <S016> */
/****************************************************************************************************/
/*		PnE42 : モニタ１ゲイン [－]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_mnrng0(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 0 );				/* アナログモニタ出力ゲイン計算					*/
	return PRM_RSLT_SUCCESS;
}

/* <S016> */
/****************************************************************************************************/
/*		PnE43 : モニタ２ゲイン [－]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_mnrng1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 1 );				/* アナログモニタ出力ゲイン計算					*/
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE44 : 下位:i-P制御割合, 上位:暴走検出トルク [%]											*/
/****************************************************************************************************/
PRM_RSLT pncal_ipg_rwt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CTRL_CMD_PRM	*CtrlCmdPrm;
	PRMDATA			*Prm;

	CtrlCmdPrm	= &(Axis->BaseControls->CtrlCmdPrm);
	Prm = Axis->UniPrms->Prm;

	if( CtrlCmdPrm->IPSpdControl )
	{
#if (FLOAT_USE==TRUE)
		CtrlCmdPrm->PI_rate = (KSGAIN)( 100 - Prm->ipg_rwt.b.l ) / 100.0f;
#else
		CtrlCmdPrm->PI_rate = MlibScalKxgain( 100 - Prm->ipg_rwt.b.l, 1, 100, NULL, 24 );
#endif
	}
	else
	{
#if (FLOAT_USE==TRUE)
		CtrlCmdPrm->PI_rate = 1.0f;
#else
		CtrlCmdPrm->PI_rate = 1;
#endif
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE48 : モニタ１アドレス [－]																*/
/****************************************************************************************************/
PRM_RSLT pncal_mnptr0(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 0 );				/* アナログモニタ出力ゲイン計算					*//* <S016> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE4A : モニタ２アドレス [－]																*/
/****************************************************************************************************/
PRM_RSLT pncal_mnptr1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 1 );				/* アナログモニタ出力ゲイン計算					*//* <S016> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE50 : 電流検出ゼロ調(U相) [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_zadjiu(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;
	Prm = Axis->UniPrms->Prm;

#ifndef	_JL076_USE_
	AsicMpAdr->AdinV.IuOffsetIn = (USHORT)Prm->zadjiu;
#else
	AsicMpAdr->MREG_IUOFIN = (USHORT)Prm->zadjiu;
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE51 : 電流検出ゼロ調(V相) [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_zadjiv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ASIC_U_IF_STR	*AsicMpAdr;
	PRMDATA	*Prm;

	AsicMpAdr = Axis->SvAsicRegs->AsicMicroReg;
	Prm = Axis->UniPrms->Prm;

#ifndef	_JL076_USE_
	AsicMpAdr->AdinV.IvOffsetIn = (USHORT)Prm->zadjiv;
#else
	AsicMpAdr->MREG_IVOFIN = (USHORT)Prm->zadjiu;		//<-???
#endif
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE52 : 電流検出ゲイン調(U,V相) [－]														*/
/****************************************************************************************************/
PRM_RSLT pncal_gadjiu_v(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	SHORT	kx;
/*--------------------------------------------------------------------------------------------------*/
/*		電流検出ゲイン調																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* Ｕ相電流ゲイン調 */
	kx = (SHORT)((CHAR)Axis->UniPrms->Prm->gadjiu_v.b.l + 0x100);
#ifndef	_JL076_USE_
	LpxSetPrmToASIC( &(Axis->SvAsicRegs->AsicMicroReg->AdinV.KcuIn), kx, 0 );
#else
	LpxSetPrmToASIC( (SHORT*)&(Axis->SvAsicRegs->AsicMicroReg->MREG_KCUIN), kx, 0 );
#endif
	/*----------------------------------------------------------------------------------------------*/
	/* Ｖ相電流ゲイン調 */
	kx = (SHORT)((CHAR)Axis->UniPrms->Prm->gadjiu_v.b.h + 0x100);
#ifndef	_JL076_USE_
	LpxSetPrmToASIC( &(Axis->SvAsicRegs->AsicMicroReg->AdinV.KcvIn), kx, 0 );
#else
	LpxSetPrmToASIC( (SHORT*)&(Axis->SvAsicRegs->AsicMicroReg->MREG_KCVIN), kx, 0 );
#endif

	return PRM_RSLT_SUCCESS;
}

/* <S016> */
/****************************************************************************************************/
/*		PnE58 : モニタ１,２ゼロ調 [－]																*/
/****************************************************************************************************/
PRM_RSLT pncal_mnaz0_1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorOffset( Axis, 0 );			/* アナログモニタ出力オフセット計算				*/
	PcalAnalogMonitorOffset( Axis, 1 );			/* アナログモニタ出力オフセット計算				*/
	return PRM_RSLT_SUCCESS;
}

/* <S016> */
/****************************************************************************************************/
/*		PnE59 : モニタ１,２ゲイン調 [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_mnag0_1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PcalAnalogMonitorGain( Axis, 0 );			/* アナログモニタ出力ゲイン計算					*/
	PcalAnalogMonitorGain( Axis, 1 );			/* アナログモニタ出力ゲイン計算					*/
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE5B : P-N電圧検出レベル微調, 上位:空き [－]												*/
/****************************************************************************************************/
PRM_RSLT pncal_vdetadj(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	Axis->PowerManager->DetectDcVolt.conf.VdetAdadj
					= (LONG)((CHAR)(Axis->UniPrms->Cprm)->pnadj.b.l);
	return PRM_RSLT_SUCCESS;
}

 /* <S154> Start */
/*****************************************************************************
  Description:	PnE80 : 内蔵FAN寿命寿命計算用周囲温度オフセット
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_fantemperatureoffset(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	Axis->hLifeTime->FanLifeTimeHdl.conf.FanTemperatureOffset = (float)Axis->UniPrms->Cprm->FanTemperatureOffset;
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	PnE81 : FAN交換オフセット時間
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_fantimeoffset(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	Axis->hLifeTime->FanLifeTimeHdl.conf.FanTimeOffset = (float)Axis->UniPrms->Cprm->FanTimeOffset;
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	PnE82 : 突防回路寿命
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_rushlifetime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	Axis->hLifeTime->RushLifeTimeHdl.conf.RushLifeTimeReciprocal
		= 1.0f / (1000.0f * (float)Axis->UniPrms->Cprm->RushLifeTime);	/*単位は１回,そして逆数にする*/
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	PnE83 : DB回路寿命
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_dblifetime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
#if 0		/* <S180> */
	Axis->hLifeTime->DBLifeTimeHdl.conf.DBLifeTimeReciprocal
			= 1.0f / (1000.0f * (float)Axis->UniPrms->Cprm->DBLifeTime);	/*単位は１回,そして逆数にする*/
#else		/* <S180> */
	Axis->hDBLifeTime->conf.DBLifeTimeReciprocal =
			1.0f / (1000.0f * (float)Axis->UniPrms->Prm->DBLifeTime);	/*単位は１回,そして逆数にする*//* <S192> */
#endif
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	PnE84 : 現在の突防リレー動作回数
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_rushswitchcount(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	CMN_PRMDATA	*Cprm;
	Cprm = Axis->UniPrms->Cprm;
	/* 現在の突防リレー動作回数 */
//	Axis->hLifeTime->RushLifeTimeHdl.var.RushSwitchCount		/* <S180> */
//						=(( Cprm->DRSwithingCount & 0x00FFU) << 16 )+ Cprm->RushSwichCount;
	Axis->hLifeTime->RushLifeTimeHdl.var.RushSwitchCount = Cprm->RushSwichCount;	/* <S180> */
#endif
	return PRM_RSLT_SUCCESS;
}

/*****************************************************************************
  Description:	PnE86 : 現在の現在のDB動作回数
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_dbswitchcount(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
#if 0		/* <S180> */
	CMN_PRMDATA	*Cprm;
	Cprm = Axis->UniPrms->Cprm;
	/*現在のDBリレー動作回数（PnE86.2,3桁目+PnE85）*/
	Axis->hLifeTime->DBLifeTimeHdl.var.DBSwitchCount
						= (( Cprm->DRSwithingCount & 0xFF00U ) << 8 )+ Cprm->DBSwichCount;
#else		/* <S180> */
	PRMDATA	*Prm;		/* <S192> */
	Prm = Axis->UniPrms->Prm;	/* <S192> */
	/* 現在のDBリレー動作回数 */
	Axis->hDBLifeTime->var.DBSwitchCount = Prm->DBSwichCount;	/* <S192> */
#endif
#endif
	return PRM_RSLT_SUCCESS;
}
/* <S180> Start */
#if 0
/*****************************************************************************
  Description:	PnE86 : 現在の突防/DBリレー動作回数（上位） 0,1桁目：
  	  	  	  	  現在の突防動作回数（上位8bit） 2,3桁目：現在のDB動作回数（上位8bit）
------------------------------------------------------------------------------
  Parameters:
	Cmd;			PRM_ACCCMD
	ArrayIdx;		Array Index
	AXIS_HANDLE;	軸 ハンドル
	pValue;			value
  Return:
	PRM_RSLT；		PRM_RSLT_SUCCESS:正常終了、PRM_RSLT_CONTINUE：正常終了 継続中(Fn用)など
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
PRM_RSLT pncal_drswitchcount(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CSW_SVCF_LIFEPRED_USE == TRUE )
	CMN_PRMDATA	*Cprm;
	Cprm = Axis->UniPrms->Cprm;
	/* 現在の突防リレー動作回数（PnE86.0,1桁目+PnE83） */
	Axis->hLifeTime->RushLifeTimeHdl.var.RushSwitchCount
							=(( Cprm->DRSwithingCount & 0x00FFU ) << 16 ) + Cprm->RushSwichCount;
	/*現在のDBリレー動作回数（PnE86.2,3桁目+PnE85）*/
	Axis->hLifeTime->DBLifeTimeHdl.var.DBSwitchCount
							= (( Cprm->DRSwithingCount & 0xFF00U ) << 8 ) + Cprm->DBSwichCount;
#endif
	return PRM_RSLT_SUCCESS;
}
#endif
/* <S180> End */
 /* <S154>  End */

/****************************************************************************************************/
/*		PnE8D : 電流零位相位置 [65536/360deg]														*/
/****************************************************************************************************/
PRM_RSLT pncal_phsofst(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MENCV	*MencV;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	MencV = Axis->MencV;

	if( (MencV->PhaseReady == TRUE) && (MencV->AbsoEncType != MENC_INCRE_TYPE)	/* ABS使用	*/
		&& ((MencV->SerialConvType == SCONVTYPE_MITUTOYO_ABS)	/* シリアルコンバータがMitutoyo	*/
		|| (MencV->SerialConvType == SCONVTYPE_ENDAT_ABS)) )		/* シリアルコンバータがEndat	*/
	{
		Axis->MpFind->MpfPhaseOffset = Prm->phsofst;
	}
	return PRM_RSLT_SUCCESS;
}

/* <S21D> Start */
/****************************************************************************************************/
/*		PnE9A : 回生同期時の回生ONレベル調整値 [V]															*/
/****************************************************************************************************/
PRM_RSLT pncal_regsynonlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CMN_PRMDATA			*Cprm;
	BPRMDAT				*Bprm;
	REGENE_CTRL			*RegeneCtrl;

	Cprm  = Axis->UniPrms->Cprm;		/* パラメータ設定データ */
	Bprm = Axis->UniPrms->Bprm;			/* 初期パラメータ定義(電源再投入で変更有効) */
	RegeneCtrl = &(Axis->PowerManager->RegeneCtrl);

	if( ((Cprm->b_prmE >> 12) & 0x0F) == PNREGSYN )
	{	/* 回生同期有効の場合 */
		RegeneCtrl->conf.RegOnLevel = (USHORT)( (Cprm->regonlvl + Cprm->regsynonlvl) * (Bprm->VdetMax / Cprm->pnvoltgn) );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE9B : 回生同期時の回生ON待ち時間 [ScanB周期]															*/
/****************************************************************************************************/
PRM_RSLT pncal_regsynontime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CMN_PRMDATA			*Cprm;
	REGENE_CTRL			*RegeneCtrl;

	Cprm  = Axis->UniPrms->Cprm;		/* パラメータ設定データ */
	RegeneCtrl = &(Axis->PowerManager->RegeneCtrl);

	RegeneCtrl->conf.RegSynWaitTime = Cprm->regsynontime;

	return PRM_RSLT_SUCCESS;
}
/* <S21D> End */
/****************************************************************************************************/
/*		PnE9C : 制御基板異常温度/警告温度											<S0DE>			*/
/****************************************************************************************************/
PRM_RSLT pncal_ctrlboardalmtmpr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CHECK_OH	*OverHeat;
	CMN_PRMDATA	*Cprm;

	OverHeat = Axis->OverHeat; /* <S0ED> */
	Cprm = Axis->UniPrms->Cprm;

	OverHeat->CtrlTmprAlm.conf.BrdAlmTmpr = (( Cprm->ctrlboardalmtmpr & 0xFF00U ) >> 8U) * 10;
	OverHeat->CtrlTmprAlm.conf.BrdWrnTmpr = ( Cprm->ctrlboardalmtmpr & 0x00FFU ) * 10;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE9D : パワー基板異常温度/警告温度											<S0DE>			*/
/****************************************************************************************************/
PRM_RSLT pncal_pwrboardalmtmpr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CHECK_OH	*OverHeat;
	CMN_PRMDATA	*Cprm;

	OverHeat = Axis->OverHeat;/* <S0ED> */
	Cprm = Axis->UniPrms->Cprm;

	OverHeat->PwrTmprAlm.conf.BrdAlmTmpr = (( Cprm->pwrboardalmtmpr & 0xFF00U ) >> 8U) * 10;
	OverHeat->PwrTmprAlm.conf.BrdWrnTmpr = ( Cprm->pwrboardalmtmpr & 0x00FFU ) * 10;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE9E : 制御基板異常クリア可能温度/異常温度検出時間							<S0DE>			*/
/****************************************************************************************************/
PRM_RSLT pncal_ctrlboardalmclrtmpr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CHECK_OH	*OverHeat;
	CMN_PRMDATA	*Cprm;

	OverHeat = Axis->OverHeat; /* <S0ED> */
	Cprm = Axis->UniPrms->Cprm;

	OverHeat->CtrlTmprAlm.conf.BrdAlmClrTmpr = (( Cprm->ctrlboardalmclrtmpr & 0xFF00U ) >> 8U) * 10;
	OverHeat->CtrlTmprAlm.conf.BrdAlmDetTime = ( Cprm->ctrlboardalmclrtmpr & 0x00FFU );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnE9F : パワー基板異常クリア可能温度/異常温度検出時間						<S0DE>			*/
/****************************************************************************************************/
PRM_RSLT pncal_pwrboardalmclrtmpr(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CHECK_OH	*OverHeat;
	CMN_PRMDATA	*Cprm;

	OverHeat = Axis->OverHeat; /* <S0ED> */
	Cprm = Axis->UniPrms->Cprm;

	OverHeat->PwrTmprAlm.conf.BrdAlmClrTmpr = (( Cprm->pwrboardalmclrtmpr & 0xFF00U ) >> 8U) * 10;	  /* <S0ED> */
	OverHeat->PwrTmprAlm.conf.BrdAlmDetTime = ( Cprm->pwrboardalmclrtmpr & 0x00FFU );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEB0 : 電圧フィードバックループ比例ゲイン [Hz]												*/
/****************************************************************************************************/
PRM_RSLT pncal_kv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;

	PcalVFBKp( &(Axis->BaseControls->WeakenField), Axis->SvAsicRegs,
	           Axis->UniPrms->Bprm, Prm->kv, Prm->tv );

	PcalVFBKi( &(Axis->BaseControls->WeakenField), Axis->SvAsicRegs,
	           Axis->UniPrms->Bprm, Prm->kv );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEB1 : 電圧フィードバック積分時定数 [us]													*/
/****************************************************************************************************/
PRM_RSLT pncal_tv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;

	PcalVFBKp( &(Axis->BaseControls->WeakenField), Axis->SvAsicRegs,
	           Axis->UniPrms->Bprm, Prm->kv, Prm->tv );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEB3 : 電圧指令制限レベル(回転用) [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_v1max(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;

	if( Axis->MencV->AxisMotType != MOTTYPE_LINEAR )
	{
		Prm = Axis->UniPrms->Prm;

		PcalIdrefLimTerm1( &(Axis->BaseControls->WeakenField), Axis->SvAsicRegs,
		    	           Axis->UniPrms->Bprm, Prm->v1max, Prm->vmaxid );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEB4 : d軸電流指令リミット用電圧制限レベル [%]												*/
/****************************************************************************************************/
PRM_RSLT pncal_vmaxid(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;
	PcalIdrefLimTerm1( &(Axis->BaseControls->WeakenField), Axis->SvAsicRegs,
	    	           Axis->UniPrms->Bprm, Prm->v1max, Prm->vmaxid );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEB5 : 主回路電圧平均時間 [0.1ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_vpnavg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;
	Axis->BaseControls->WeakenField.var.VdcAvgCnt = ( KPI_SCANB_MS( Prm->vpnavg ) / 20 );

	return PRM_RSLT_SUCCESS;
}

/*--------------------------------------------------------------------------------------------------*/
/*==== for MECHATROLINK2-IF	===================================================================================	*//* <S004> */
#if (CSW_NETIF_TYPE == NETIF_M2_TYPE)
/****************************************************************************************************/
/*		Pn800 : 通信制御機能選択											for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_comCtrl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	ALM_HNDL	*almHdl;
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;
	UCHAR		chk;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	almHdl = &Axis->NetAxisIf->alm_hdl;

	/* 通信エラーマスク設定 */
	regHdl->netPrm->recvErrMask = (( Prm->comCtrl & 0x01 ) != 0 ) ? TRUE : FALSE;
	regHdl->netPrm->wdtErrMask  = (( Prm->comCtrl & 0x2 ) != 0 ) ? TRUE : FALSE;
	regHdl->netPrm->recvWrnMask = (( Prm->comCtrl & 0x40 ) != 0 ) ? TRUE : FALSE;

	/* 通信異常カウント */
	regHdl->netPrm->recvErrLimt = (( Prm->comCtrl >> 8 ) & 0xF ) + 2;

	/* A.E6*マスクチェック */
	chk = (( Prm->comCtrl & 0x01 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_COM_RCVSTP,      chk );

	/* A.E5*マスクチェック */
	chk = (( Prm->comCtrl & 0x02 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_COM_WDC,   chk );

	/* A.94*マスクチェック */
	chk = (( Prm->comCtrl & 0x10 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_PRM_NO,     chk );
	AlmSetMask( almHdl, ERR_DATA_SET,   chk );
	AlmSetMask( almHdl, ERR_DATA_CALC,  chk );
	AlmSetMask( almHdl, ERR_DATA_SZ,    chk );
	AlmSetMask( almHdl, ERR_DATA_LTMOD, chk );

	/* A.95*マスクチェック */
	chk = (( Prm->comCtrl & 0x20 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_CMD_CNDTN,   chk );
	AlmSetMask( almHdl, ERR_CMD_NOSUP,   chk );
	AlmSetMask( almHdl, ERR_CMD_PRM,     chk );
	AlmSetMask( almHdl, ERR_CMD_CNFLCT,  chk );
	AlmSetMask( almHdl, ERR_SUBCMD,      chk );
	AlmSetMask( almHdl, ERR_FUNC_CNFLCT, chk );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn801 : ソフトリミット機能選択										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_sLimitSw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* P-SOT(0:無効,1:有効) */
	regHdl->mtPrm->PsnPrm.sotEnableP = ((*pValue & 0x1) != 0) ? FALSE: TRUE;

	/* N-SOT(0:無効,1:有効) */
	regHdl->mtPrm->PsnPrm.sotEnableN = ((*pValue & 0x2) != 0) ? FALSE: TRUE;

	/* 指令入力SOT	*/
	regHdl->mtPrm->PsnPrm.tposLimitEnable = ((*pValue & 0x100) != 0) ? TRUE: FALSE;

	return( PRM_RSLT_SUCCESS );
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn802 : コマンド動作機能選択										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_cmdCtrlSw( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn803 : 原点位置範囲 [[ref]]										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zpWidth(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* 原点位置範囲設定処理 */
	regHdl->mtPrm->PsnPrm.zPointWidth = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn804 : 正側ソフトリミット値 L [[ref]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_p_sLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 正側ソフトリミット値	*/
	regHdl->mtPrm->PsnPrm.softLimit_P = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn806 : 負側ソフトリミット値 L [[ref]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_n_sLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 負側ソフトリミット値	*/
	regHdl->mtPrm->PsnPrm.softLimit_N = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn808 : アブソＰＧ原点位置オフセット L [[ref]]						for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_absOffsetL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* アブソＰＧ原点位置オフセット設定処理 */
	regHdl->mtPrm->PsnPrm.absOfsset = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80A : １段目直線加速定数 [[1000ref/s^s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_accRate1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* １段目直線加速定数	*/
	MPrmSetAccRate1( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80A);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80B : ２段目直線加速定数 [[1000ref/s^s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_accRate2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* ２段目直線加速定数	*/
	MPrmSetAccRate2(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80B);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80C : 加速定数切替え速度 [[100ref/s]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_accChgSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* 加速定数切替え速度	*/
//	MPrmSetAccAltSpd(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);/* <S07E> */
#if (FLOAT_USE==TRUE)/* <S07E> */
	regHdl->mtPrm->MgPrm.altspd_a = (LONG)( (float)*pValue * 100 * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_a = MlibMulgain32( *pValue * 100, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->accChgSpd, MTBNK_PN80C);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80D : １段目直線減速定数 [[1000ref/s^s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* １段目直線減速定数	*/
	MPrmSetDecRate1(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80D);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80E : ２段目直線減速定数 [[1000ref/s^s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* ２段目直線減速定数	*/
	MPrmSetDecRate2(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80E);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80F : 減速定数切替え速度 [[100ref/s]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decChgSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* 減速定数切替え速度	*/
//	MPrmSetDecAltSpd(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);/* <S07E> */
#if (FLOAT_USE==TRUE)
	regHdl->mtPrm->MgPrm.altspd_d = (LONG)( (float)*pValue * 100 * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_d = MlibMulgain32( *pValue * 100, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->decChgSpd, MTBNK_PN80F);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn810 : 指数関数加減速バイアス [[100ref/s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_filExpBias(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG		data;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 指数関数加減速バイアス設定 */
	data = (*pValue) * 100;
	MPrmSetExpFilBias(regHdl->mtPrm, &regHdl->mtPrm->AccFilPrm, data);
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->filExpBias, MTBNK_PN810);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn811 : 指数関数加減速時定数 [[0.1ms]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_filExpTb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;


	/* 指数関数加減速時定数 */
	MPrmSetExpFilTb(regHdl->mtPrm, &regHdl->mtPrm->AccFilPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->filExpTb, MTBNK_PN811);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn812 : 移動平均時間 [[0.1ms]]										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_filMaTb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 移動平均時間設定処理 */
	MPrmSetMaFilTb(regHdl->mtPrm, &regHdl->mtPrm->AccFilPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->filMaTb, MTBNK_PN812);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn814 : 外部位置決め最終走行距離 L [[ref]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_exDistL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 外部位置決め最終走行距離設定処理 */
	regHdl->mtPrm->MtSeqPrm.ex_posing_dist = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn816 : 原点復帰モード設定											for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretDir(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	MPrmSetZretDir(regHdl->mtPrm, (BOOL)(*pValue));
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn817,Pn842 : 原点復帰アプローチ速度１ [[100ref/s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	LONG		data;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 有効ﾊﾟﾗﾒｰﾀ選択	*/
	if( Prm->zretSpd1 )
	{
		data = Prm->zretSpd1 * 100;
	}
	else
	{
		data = Prm->zretSpd12L * 100;
	}
	/*  原点復帰アプローチ速度１設定処理 */
	regHdl->mtPrm->MtSeqPrm.zretsp1     = data;
//	regHdl->mtPrm->MtSeqPrm.zretsp1_org = data;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn818,Pn844 : 原点復帰アプローチ速度２ [[100ref/s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	LONG		data;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 有効ﾊﾟﾗﾒｰﾀ選択	*/
	if( Prm->zretSpd2 )
	{
		data = Prm->zretSpd2 * 100;
	}
	else
	{
		data = regHdl->Prm->zretSpd22L * 100;
	}
	
	/*  原点復帰アプローチ速度１設定処理 */
	regHdl->mtPrm->MtSeqPrm.zretsp2     = data;
//	regHdl->mtPrm->MtSeqPrm.zretsp2_org = data;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn819 : 原点復帰最終走行距離 L [[ref]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretDistL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 原点復帰最終走行距離の設定 */
	regHdl->mtPrm->MtSeqPrm.zret_dist = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn81E : 入力信号モニタ選択											for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_ioMonSel(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	MPrmSetInSigMap(regHdl->mtPrm, *pValue);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*		Pn820 : 正側ラッチ可能領域 L [[ref]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_p_ltLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチ領域設定 */
	regHdl->mtPrm->PsnPrm.ltRangeP = Prm->p_ltLimitL;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn822 : 逆側ラッチ可能領域 L [[ref]]								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_n_ltLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチ領域設定 */
	regHdl->mtPrm->PsnPrm.ltRangeN = Prm->n_ltLimitL;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn824 : オプションモニタ１選択										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_opMon1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MT_HNDL		*mtHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	mtHdl = &Axis->NetAxisIf->mt_hdl;

	mtHdl->svCmd->SelMon5 = Prm->opMon1;
	mtHdl->mtPrm.BasePrm.opMon[0] = Prm->opMon1;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn825 : オプションモニタ２選択										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_opMon2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MT_HNDL		*mtHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	mtHdl = &Axis->NetAxisIf->mt_hdl;

	mtHdl->svCmd->SelMon6 = Prm->opMon2;
	mtHdl->mtPrm.BasePrm.opMon[1] = Prm->opMon2;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn826 : 非常停止用１段目直線減速定数 [[1000ref/s^s]]				for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate1Emg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG data;

	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;
	
	/* 単位計算	*/
	data = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	
	/* データセット	*/
	regHdl->mtPrm->BasePrm.StopPrm.decrate1 = data;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn827 : 非常停止用２段目直線減速定数 [[1000ref/s^s]]				for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate2Emg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG data;

	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* 単位計算	*/
	data = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	
	/* データセット	*/
	regHdl->mtPrm->BasePrm.StopPrm.decrate2 = data;
	
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn828 : 非常停止用減速定数切替え速度 [[100ref/s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decChgSpdEmg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG		data;

	/* <S07E>;ﾛｰｶﾙ変数設定 */
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;
	
	/* 単位計算	*/
#if (FLOAT_USE==TRUE)
	data = (LONG)( (float)*pValue * 100 * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	data = MlibMulgain32( *pValue * 100, regHdl->mtPrm->BasePrm.PosingRateKx);
#endif /* FLOAT_USE */
	
	/* データセット	*/
	regHdl->mtPrm->BasePrm.StopPrm.altspd_d = data;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn829 : SVOFF待ち時間(減速停止SVOFF時)								for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_svOffTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	data;
	LONG	sx, kx;
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	sx = 0;

	/* サーボOFF減速停止用タイマの設定 */
	data = Prm->svOffTime * 10;
	kx = MlibPcalKxgain( data, 4000, regHdl->mtPrm->BasePrm.MotionCycle, &sx, 24 );
	regHdl->mtPrm->MtSeqPrm.rampTimeOut = MlibMulgain32( 1, kx );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn830 : 機械一回転当たりの指令単位量L [[ref]]						for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_rotrefL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		return( TRUE );
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn832 : 機械一回転当たりのステーション数							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_st_num(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		return( TRUE );
}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*		Pn834 : １段目直線加速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_accRate12L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;

	MPrmSetAccRate1( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->accRate12L );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN834);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn836 : ２段目直線加速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_accRate22L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	MPrmSetAccRate2( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->accRate22L );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN836);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn838 : 加速定数切替え速度 2nd L [[ref/s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_accChgSpd2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;

	/* 加速度切り替え速度設定 */
#if (FLOAT_USE==TRUE)/* <S07E> */
	regHdl->mtPrm->MgPrm.altspd_a = (LONG)( (float)Prm->accChgSpd2L * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_a = MlibMulgain32( Prm->accChgSpd2L, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN838);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn83A : １段目直線減速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decRate12L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	MPrmSetDecRate1(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->decRate12L);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN83A);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn83C : ２段目直線減速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decRate22L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	MPrmSetDecRate2( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->decRate22L );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN83C);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn83E : 減速定数切替え速度 2nd L [[ref/s]]							for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decChgSpd2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	/* 減速度り替え速度設定 */
#if (FLOAT_USE==TRUE)/* <S07E> */
	regHdl->mtPrm->MgPrm.altspd_d = (LONG)( (float)Prm->decChgSpd2L * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_d = MlibMulgain32( Prm->decChgSpd2L, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN83E);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn840 : 非常停止用２段目直線減速定数　2nd L	[[1000ref/s^s]]			for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decRateEmg2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG data;
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;

	/* 非常停止用減速度2設定 */
	data = MlibMulgain32( Prm->decRateEmg2L, regHdl->mtPrm->BasePrm.PosingRateKx );
	if( data >= 0x40000000 )
	{
		data = 0x3FFFFFFF;
	}
	regHdl->mtPrm->BasePrm.StopPrm.altspd_d = 0;
	regHdl->mtPrm->BasePrm.StopPrm.decrate2 = data;
	regHdl->mtPrm->BasePrm.StopPrm.decrate1 = data;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn842 : 原点復帰アプローチ速度１ 2nd L [[100ref/s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd12L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
PRM_RSLT rc;

	rc = pncal_zretSpd1(Cmd, ArrayIdx, Axis, pValue);
	
	return rc;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn844 : 原点復帰アプローチ速度２ 2nd L [[100ref/s]]					for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd22L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
PRM_RSLT rc;

	rc = pncal_zretSpd2(Cmd, ArrayIdx, Axis, pValue);
	
	return rc;
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn850 : 連続ラッチシーケンス数										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltSeqNum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチシーケンス数設定 */
	regHdl->mtPrm->LtPrm.seqNum = (USHORT)Prm->ltSeqNum;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn851 : 連続ラッチ回数												for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltLoopNum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチシーケンス回数設定 */
	regHdl->mtPrm->LtPrm.loopCnt = (USHORT)Prm->ltLoopNum;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn852 : 連続ラッチシーケンス設定1-4									for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltSeqSet1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチシーケンス信号設定 */
	regHdl->mtPrm->LtPrm.ltSgnl[0] = (UCHAR)( Prm->ltSeqSet1       & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[1] = (UCHAR)((Prm->ltSeqSet1>>4  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[2] = (UCHAR)((Prm->ltSeqSet1>>8  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[3] = (UCHAR)((Prm->ltSeqSet1>>12 ) & 0xF );

	regHdl->mtPrm->LtPrm.ltSgnl[4] = (UCHAR)((Prm->ltSeqSet2     ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[5] = (UCHAR)((Prm->ltSeqSet2>>4  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[6] = (UCHAR)((Prm->ltSeqSet2>>8  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[7] = (UCHAR)((Prm->ltSeqSet2>>12 ) & 0xF );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn853 : 連続ラッチシーケンス設定5-8									for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltSeqSet2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	pncal_ltSeqSet1( Cmd, ArrayIdx, Axis, pValue );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn884 : 通信異常時動作設定											for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_commErrBehavior(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	/* COMM_ALM発生時 BlkRelease設定 */
	if( (*pValue & 0x00000001) != 0 )
	{
		Axis->NetAxisIf->alm_hdl.CommAlmBlkCancel = TRUE;
	}
	else
	{
		Axis->NetAxisIf->alm_hdl.CommAlmBlkCancel = FALSE;
	}
		
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*		Pn88F : 通信CMD/RSPデータ制御										for	MECHATROLINK2-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_comDataCtrl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*		Pn920-Pn95F : バンクデータ設定																*/
/****************************************************************************************************/
static PRM_RSLT	pncal_bnkData( AXIS_HANDLE *Axis, USHORT no )
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);

	MBnkSetData(regHdl->mtPrm, &regHdl->Prm->bnkData[0x00], no);/* <S080> */
	
	return PRM_RSLT_SUCCESS;
}

PRM_RSLT pncal_bnkData00( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x00);}
PRM_RSLT pncal_bnkData01( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x01);}
PRM_RSLT pncal_bnkData02( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x02);}
PRM_RSLT pncal_bnkData03( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x03);}
PRM_RSLT pncal_bnkData04( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x04);}
PRM_RSLT pncal_bnkData05( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x05);}
PRM_RSLT pncal_bnkData06( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x06);}
PRM_RSLT pncal_bnkData07( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x07);}
PRM_RSLT pncal_bnkData08( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x08);}
PRM_RSLT pncal_bnkData09( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x09);}
PRM_RSLT pncal_bnkData0A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0A);}
PRM_RSLT pncal_bnkData0B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0B);}
PRM_RSLT pncal_bnkData0C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0C);}
PRM_RSLT pncal_bnkData0D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0D);}
PRM_RSLT pncal_bnkData0E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0E);}
PRM_RSLT pncal_bnkData0F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0F);}
PRM_RSLT pncal_bnkData10( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x10);}
PRM_RSLT pncal_bnkData11( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x11);}
PRM_RSLT pncal_bnkData12( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x12);}
PRM_RSLT pncal_bnkData13( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x13);}
PRM_RSLT pncal_bnkData14( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x14);}
PRM_RSLT pncal_bnkData15( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x15);}
PRM_RSLT pncal_bnkData16( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x16);}
PRM_RSLT pncal_bnkData17( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x17);}
PRM_RSLT pncal_bnkData18( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x18);}
PRM_RSLT pncal_bnkData19( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x19);}
PRM_RSLT pncal_bnkData1A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1A);}
PRM_RSLT pncal_bnkData1B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1B);}
PRM_RSLT pncal_bnkData1C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1C);}
PRM_RSLT pncal_bnkData1D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1D);}
PRM_RSLT pncal_bnkData1E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1E);}
PRM_RSLT pncal_bnkData1F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1F);}
PRM_RSLT pncal_bnkData20( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x20);}
PRM_RSLT pncal_bnkData21( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x21);}
PRM_RSLT pncal_bnkData22( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x22);}
PRM_RSLT pncal_bnkData23( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x23);}
PRM_RSLT pncal_bnkData24( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x24);}
PRM_RSLT pncal_bnkData25( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x25);}
PRM_RSLT pncal_bnkData26( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x26);}
PRM_RSLT pncal_bnkData27( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x27);}
PRM_RSLT pncal_bnkData28( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x28);}
PRM_RSLT pncal_bnkData29( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x29);}
PRM_RSLT pncal_bnkData2A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2A);}
PRM_RSLT pncal_bnkData2B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2B);}
PRM_RSLT pncal_bnkData2C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2C);}
PRM_RSLT pncal_bnkData2D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2D);}
PRM_RSLT pncal_bnkData2E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2E);}
PRM_RSLT pncal_bnkData2F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2F);}
PRM_RSLT pncal_bnkData30( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x30);}
PRM_RSLT pncal_bnkData31( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x31);}
PRM_RSLT pncal_bnkData32( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x32);}
PRM_RSLT pncal_bnkData33( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x33);}
PRM_RSLT pncal_bnkData34( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x34);}
PRM_RSLT pncal_bnkData35( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x35);}
PRM_RSLT pncal_bnkData36( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x36);}
PRM_RSLT pncal_bnkData37( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x37);}
PRM_RSLT pncal_bnkData38( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x38);}
PRM_RSLT pncal_bnkData39( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x39);}
PRM_RSLT pncal_bnkData3A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3A);}
PRM_RSLT pncal_bnkData3B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3B);}
PRM_RSLT pncal_bnkData3C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3C);}
PRM_RSLT pncal_bnkData3D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3D);}
PRM_RSLT pncal_bnkData3E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3E);}
PRM_RSLT pncal_bnkData3F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3F);}

#endif //CSW_NETIF_TYPE == NETIF_M2_TYPE









/*==== for MECHATROLINK3-IF	===================================================================================	*//* <S004> */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
/* conversion function for Alias parameters	*/
/****************************************************************************************************/
/*		Pn100-->PnAC2 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
// Pn100の単位を変換してPnAC2に格納する。PnAC2はlong型なのでPnAC3は不要である。
static PRM_RSLT pncal_m3_convert_Kv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
	/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_KvL = (*pValue) * 100;

	/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_KvL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn101-->PnAC4 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
// Pn101の単位を変換してPnAC4に格納する。PnAC4はlong型なのでPnAC5は不要である。
static PRM_RSLT pncal_m3_convert_Tvi(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_TviL =(*pValue) * 10;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_TviL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn102-->PnAC6 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
// Pn102の単位を変換してPnAC6に格納する。PnAC6はlong型なのでPnAC7は不要である。
static PRM_RSLT pncal_m3_convert_Kp(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_KpL = (*pValue) * 100;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_KpL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn109-->PnAC8 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_FFGain(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_FFGainL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_FFGainL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn11F-->PnACA 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_pitfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_TpiL = (*pValue) * 100;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_TpiL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn205-->PnA48														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_mtLimit(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_mtLimitL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_mtLimitL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn20E-->PnA42														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_ratb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_ratbL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_ratbL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn210-->PnA44														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_rata(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_rataL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_rataL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn282 : 															for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
//static PRM_RSLT pncal_m3_convert_scalepitch(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
//{
//	PRMDATA		*Prm;
//
//	Prm	= Axis->prm_hndl_ptr->pprm_ptr;
//	Prm->m3_scalepitchL = Prm->scalepitch2l * 10;
//	if(Cmd == PRM_ACCCMD_EEPWRITE)
//	{
//		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_scalepitchL, 0);
//	}
//	return PRM_RSLT_SUCCESS;
//}

/****************************************************************************************************/
/*		Pn404-->PnB18														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_pTrqLmt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_pTrqLmtL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_pTrqLmtL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn405-->PnB1A														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_nTrqLmt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_nTrqLmtL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_nTrqLmtL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn502,Pn581-->PnB1C													for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_zSpdLv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		/* 単位変換処理  min-1 → パラメータ設定*/
		Prm->m3_zSpdLvL = *pValue * 1000;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_zSpdLvL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn503,Pn582-->PnB1E													for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_vCmpLv(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		/* 単位変換処理   min-1 → パラメータ設定 */
		Prm->m3_vCmpLvL = *pValue * 1000;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_vCmpLvL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn50A-->PnA4A,PnB28(保存用ｱﾄﾞﾚｽ)									for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_insel1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	LONG	 data, otMask, Pn50A, PnB28;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		if ((*pValue & 0xF000) != 0x8000)
		{/* Pn50A.3でOT無効以外の設定	*/
			PnB28 = (*pValue >> 12) & 0x000F;		/* マスクしない場合は、保持した値を使うため	*/
			Pn50A = 0x0000;							/* 別名パラメータ作成用	*/
			/*---- 保存用パラメータRAM書込み ----------*/
			Prm->m3_Pn50A = PnB28;
			/*---- 保存用パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
			if(Cmd == PRM_ACCCMD_EEPWRITE)
			{
				PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_Pn50A, 0);
			}
#endif /* _NON_EEPROM_ */
		}
		else
		{/* Pn50A.3でOT無効の設定	*/
			Pn50A = 0x0001;							/* 別名パラメータ作成用	*/
		}
		
		/*---- 別名パラメータRAM読込み ----------*/
		otMask = Prm->m3_limitSwL & 0x0032;
		/*---- 別名パラメータ値作成 ----------*/
		data = otMask | Pn50A;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_limitSwL = data;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_limitSwL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn50B-->PnA4A,PnB29(保存用ｱﾄﾞﾚｽ)									for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_insel2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	LONG	 data, otMask, Pn50B, PnB29;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
//		if ((*pValue & 0xF000) != 0x8000)		/* <S091>：DELL */
		if ((*pValue & 0x000F) != 0x0008)		/* <S091>：ADD */
		{/* Pn50B.3でOT無効以外の設定	*/
//			PnB29 = (*pValue >> 12) & 0x000F;		/* マスクしない場合は、保持した値を使うため	*//* <S091>：DELL */
			PnB29 = (*pValue) & 0x000F;		/* マスクしない場合は、保持した値を使うため	*//* <S091>：ADD */
			Pn50B = 0x0000;							/* 別名パラメータ作成用	*/
			/*---- 保存用パラメータRAM書込み ----------*/
			Prm->m3_Pn50B = PnB29;
			/*---- 保存用パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
			if(Cmd == PRM_ACCCMD_EEPWRITE)
			{
				PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_Pn50B, 0);
			}
#endif /* _NON_EEPROM_ */
		}
		else
		{/* Pn50B.3でOT無効の設定	*/
			Pn50B = 0x0002;							/* 別名パラメータ作成用	*/
		}
		
		/*---- 別名パラメータRAM読込み ----------*/
		otMask = Prm->m3_limitSwL & 0x0031;
		/*---- 別名パラメータ値作成 ----------*/
		data = otMask | Pn50B;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_limitSwL = data;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_limitSwL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn522-->PnACC														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_psetWidth(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_psetWidthL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_psetWidthL, 0);		/*<S008>*/
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn524-->PnACE														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_nearWidth(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_nearWidthL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_nearWidthL, 0);		/*<S008>*/
		}
#endif /* _NON_EEPROM_ */
	}	
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn801-->PnA4A 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_sLimitSw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	LONG		 data, otMask, Pn801;

	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM読込み ----------*/
		otMask = Prm->m3_limitSwL & 0x0003;
		/*---- 別名パラメータ値作成 ----------*/
		Pn801 = 3 - (*pValue & 0x000F);
		data = otMask | ((Pn801 << 4) & 0x0030);
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_limitSwL = data;
		/*---- 別名パラメータEEPROM書き込み -----*/
	#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_limitSwL, 0);
		}
	#endif
	}
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn808-->PnA46 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_absOffset(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_absOffsetL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_absOffsetL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn804-->PnA4C														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_p_sLimit(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_psLimitL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_psLimitL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn806-->PnA50 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_n_sLimit(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_nsLimitL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_nsLimitL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn811-->PnB02 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_filExpTb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_filExpTbL = *pValue * 100;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_filExpTbL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn812-->PnB04 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_filMaTb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_filMaTbL = *pValue * 100;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_filMaTbL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn814-->PnB06 														for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_exDist(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_exDistL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_exDistL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn817,Pn842-->PnB08 												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_zretSpd1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		regHdl = &Axis->NetAxisIf->reg_hdl;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_zretSpd1L = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm);	/* ref/s -> 10^-3rpm	*/
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_zretSpd1L, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn818,Pn844-->PnB0A 												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_zretSpd2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		regHdl = &Axis->NetAxisIf->reg_hdl;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_zretSpd2L = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.m3ConvUnit.m3ReftoRpm);	/* ref/s -> 10^-3rpm	*/
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_zretSpd2L, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn819-->PnB0C		 												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_zretDist(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_zretDistL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_zretDistL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn803-->PnB16		 												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT pncal_m3_convert_zpWidth(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	
	if(Cmd != PRM_ACCCMD_RECALC)
	{
		Prm	= Axis->UniPrms->Prm;
		/*---- 別名パラメータRAM書込み ----------*/
		Prm->m3_zpWidthL = *pValue;
		
		/*---- 別名パラメータEEPROM書き込み -----*/
#if !defined(_NON_EEPROM_)
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_m3_zpWidthL, 0);
		}
#endif /* _NON_EEPROM_ */
	}
	return PRM_RSLT_SUCCESS;
}



/* So for, conversion function for Alias parameters	*/


/****************************************************************************************************/
/*		Pn800 : 通信制御機能選択											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_comCtrl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CMD_HNDL	*cmdHdl;
	ALM_HNDL	*almHdl;
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;
	UCHAR		chk;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	cmdHdl = &Axis->NetAxisIf->cmd_hdl;
	almHdl = &Axis->NetAxisIf->alm_hdl;

	/* 通信エラーマスク設定 */
	regHdl->netPrm->recvErrMask = (( Prm->comCtrl & 0x01 ) != 0 ) ? TRUE : FALSE;
	regHdl->netPrm->wdtErrMask  = (( Prm->comCtrl & 0x2 ) != 0 ) ? TRUE : FALSE;
	regHdl->netPrm->recvWrnMask = (( Prm->comCtrl & 0x40 ) != 0 ) ? TRUE : FALSE;

	/* 通信異常カウント */
	regHdl->netPrm->recvErrLimt = (( Prm->comCtrl >> 8 ) & 0xF ) + 2;

	/* A.E6*マスクチェック */
	chk = (( Prm->comCtrl & 0x01 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_COM_RCVSTP,      chk );
	AlmSetMask( almHdl, ERR_GBL_SYNC,        chk );
	AlmSetMask( almHdl, ERR_COM_FCS_A,       chk );
	AlmSetMask( almHdl, ERR_GBL_COM_A,       chk );
	AlmSetMask( almHdl, ERR_MON_SLV_COM_A,   chk );
	AlmSetMask( almHdl, ERR_COM_RCV_ASYNC_A, chk );
	AlmSetMask( almHdl, ERR_COM_TOUT_ASYNC,  chk );
	AlmSetMask( almHdl, ERR_COM_SEND_ASYNC,  chk );

	/* A.E5*マスクチェック */
	chk = (( Prm->comCtrl & 0x02 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_COM_WDC,   chk );
	AlmSetMask( almHdl, ERR_SYNC_TOUT, chk );

	/* A.94*マスクチェック */
	chk = (( Prm->comCtrl & 0x10 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_PRM_NO,     chk );
	AlmSetMask( almHdl, ERR_DATA_SET,   chk );
	AlmSetMask( almHdl, ERR_DATA_CALC,  chk );
	AlmSetMask( almHdl, ERR_DATA_SZ,    chk );
	AlmSetMask( almHdl, ERR_DATA_LTMOD, chk );

	/* A.95*マスクチェック */
	chk = (( Prm->comCtrl & 0x20 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_CMD_CNDTN,   chk );
	AlmSetMask( almHdl, ERR_CMD_NOSUP,   chk );
	AlmSetMask( almHdl, ERR_CMD_PRM,     chk );
	AlmSetMask( almHdl, ERR_CMD_CNFLCT,  chk );
	AlmSetMask( almHdl, ERR_SUBCMD,      chk );
	AlmSetMask( almHdl, ERR_FUNC_CNFLCT, chk );

	/* A.96*マスクチェック */
	chk = (( Prm->comCtrl & 0x40 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_COM_RCV,         chk );
	AlmSetMask( almHdl, ERR_COM_FCS_W,       chk );
	AlmSetMask( almHdl, ERR_GBL_COM_W,       chk );
	AlmSetMask( almHdl, ERR_MON_SLV_COM_W,   chk );
	AlmSetMask( almHdl, ERR_COM_RCV_ASYNC_W, chk );

	/* A.97*マスクチェック */
	chk = (( Prm->comCtrl & 0x80 ) != 0 ) ? TRUE : FALSE;
	AlmSetMask( almHdl, ERR_COM_PHASE,   chk );
	AlmSetMask( almHdl, ERR_ACT_COP_STP, chk );

	/* CMD_ALM自動クリアモード設定 */
	AlmSetCmdAlmClearMode( almHdl, cmdHdl->cmdInfo.profileType, Prm->comCtrl);

	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn801 : ソフトリミット機能選択										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_sLimitSw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* P-SOT(0:無効,1:有効) */
	regHdl->mtPrm->PsnPrm.sotEnableP = ((*pValue & 0x1) != 0) ? FALSE: TRUE;

	/* N-SOT(0:無効,1:有効) */
	regHdl->mtPrm->PsnPrm.sotEnableN = ((*pValue & 0x2) != 0) ? FALSE: TRUE;

	/* 指令入力SOT	*/
	regHdl->mtPrm->PsnPrm.tposLimitEnable = ((*pValue & 0x100) != 0) ? TRUE: FALSE;

	pncal_m3_convert_sLimitSw( Cmd, ArrayIdx, Axis, pValue );
																	
	return( PRM_RSLT_SUCCESS );
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn802 : コマンド動作機能選択										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_cmdCtrlSw( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue )
{
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn803 : 原点位置範囲 [[ref]]										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zpWidth(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	pncal_m3_convert_zpWidth( Cmd, ArrayIdx, Axis, pValue );
	
	/* 原点位置範囲設定処理 */
	regHdl->mtPrm->PsnPrm.zPointWidth = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn804 : 正側ソフトリミット値 L [[ref]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_p_sLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	pncal_m3_convert_p_sLimit( Cmd, ArrayIdx, Axis, pValue );
	/* 正側ソフトリミット値	*/
	regHdl->mtPrm->PsnPrm.softLimit_P = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn806 : 負側ソフトリミット値 L [[ref]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_n_sLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	pncal_m3_convert_n_sLimit( Cmd, ArrayIdx, Axis, pValue );
	/* 負側ソフトリミット値	*/
	regHdl->mtPrm->PsnPrm.softLimit_N = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn808 : アブソＰＧ原点位置オフセット L [[ref]]						for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_absOffsetL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	pncal_m3_convert_absOffset( Cmd, ArrayIdx, Axis, pValue );
	/* アブソＰＧ原点位置オフセット設定処理 */
	regHdl->mtPrm->PsnPrm.absOfsset = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80A : １段目直線加速定数 [[1000ref/s^s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_accRate1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* １段目直線加速定数	*/
	MPrmSetAccRate1( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80A);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80B : ２段目直線加速定数 [[1000ref/s^s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_accRate2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* ２段目直線加速定数	*/
	MPrmSetAccRate2(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80B);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80C : 加速定数切替え速度 [[100ref/s]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_accChgSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* 加速定数切替え速度	*/
//	MPrmSetAccAltSpd(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);/* <S07E> */
#if (FLOAT_USE==TRUE)/* <S07E> */
	regHdl->mtPrm->MgPrm.altspd_a = (LONG)( (float)*pValue * 100 * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_a = MlibMulgain32( *pValue * 100, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->accChgSpd, MTBNK_PN80C);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80D : １段目直線減速定数 [[1000ref/s^s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* １段目直線減速定数	*/
	MPrmSetDecRate1(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80D);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80E : ２段目直線減速定数 [[1000ref/s^s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* ２段目直線減速定数	*/
	MPrmSetDecRate2(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN80E);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn80F : 減速定数切替え速度 [[100ref/s]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decChgSpd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* 減速定数切替え速度	*/
//	MPrmSetDecAltSpd(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, *pValue);/* <S07E> */
#if (FLOAT_USE==TRUE)/* <S07E> */
	regHdl->mtPrm->MgPrm.altspd_d = (LONG)( (float)*pValue * 100 * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_d = MlibMulgain32( *pValue * 100, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->decChgSpd, MTBNK_PN80F);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn810 : 指数関数加減速バイアス [[100ref/s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_filExpBias(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG		data;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 指数関数加減速バイアス設定 */
	data = (*pValue) * 100;
	MPrmSetExpFilBias(regHdl->mtPrm, &regHdl->mtPrm->AccFilPrm, data);
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->filExpBias, MTBNK_PN810);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn811 : 指数関数加減速時定数 [[0.1ms]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_filExpTb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	pncal_m3_convert_filExpTb( Cmd, ArrayIdx, Axis, pValue );

	/* 指数関数加減速時定数 */
	MPrmSetExpFilTb(regHdl->mtPrm, &regHdl->mtPrm->AccFilPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->filExpTb, MTBNK_PN811);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn812 : 移動平均時間 [[0.1ms]]										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_filMaTb(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	pncal_m3_convert_filMaTb( Cmd, ArrayIdx, Axis, pValue );

	/* 移動平均時間設定処理 */
	MPrmSetMaFilTb(regHdl->mtPrm, &regHdl->mtPrm->AccFilPrm, *pValue);
	MBnkSetDefData(regHdl->mtPrm, regHdl->Prm->filMaTb, MTBNK_PN812);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn814 : 外部位置決め最終走行距離 L [[ref]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_exDistL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	pncal_m3_convert_exDist( Cmd, ArrayIdx, Axis, pValue );

	/* 外部位置決め最終走行距離設定処理 */
	regHdl->mtPrm->MtSeqPrm.ex_posing_dist = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn816 : 原点復帰モード設定											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretDir(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	MPrmSetZretDir(regHdl->mtPrm, (BOOL)(*pValue));
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn817,Pn842 : 原点復帰アプローチ速度１ [[100ref/s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	LONG		data;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 有効ﾊﾟﾗﾒｰﾀ選択	*/
	if( Prm->zretSpd1 )
	{
		data = Prm->zretSpd1 * 100;
	}
	else
	{
		data = Prm->zretSpd12L * 100;
	}
	/*  原点復帰アプローチ速度１設定処理 */
	regHdl->mtPrm->MtSeqPrm.zretsp1     = data;
	regHdl->mtPrm->MtSeqPrm.zretsp1_org = data;
	
	pncal_m3_convert_zretSpd1( Cmd, ArrayIdx, Axis, &data );
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn818,Pn844 : 原点復帰アプローチ速度２ [[100ref/s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	LONG		data;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* 有効ﾊﾟﾗﾒｰﾀ選択	*/
	if( Prm->zretSpd2 )
	{
		data = Prm->zretSpd2 * 100;
	}
	else
	{
		data = regHdl->Prm->zretSpd22L * 100;
	}
	
	/*  原点復帰アプローチ速度１設定処理 */
	regHdl->mtPrm->MtSeqPrm.zretsp2     = data;
	regHdl->mtPrm->MtSeqPrm.zretsp2_org = data;
	
	pncal_m3_convert_zretSpd2( Cmd, ArrayIdx, Axis, &data );
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn819 : 原点復帰最終走行距離 L [[ref]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretDistL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	pncal_m3_convert_zretDist( Cmd, ArrayIdx, Axis, pValue );

	/* 原点復帰最終走行距離の設定 */
	regHdl->mtPrm->MtSeqPrm.zret_dist = *pValue;
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn81E : 入力信号モニタ選択											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_ioMonSel(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	MPrmSetInSigMap(regHdl->mtPrm, *pValue);
	
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*		Pn820 : 正側ラッチ可能領域 L [[ref]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_p_ltLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチ領域設定 */
	regHdl->mtPrm->PsnPrm.ltRangeP = Prm->p_ltLimitL;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn822 : 逆側ラッチ可能領域 L [[ref]]								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_n_ltLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチ領域設定 */
	regHdl->mtPrm->PsnPrm.ltRangeN = Prm->n_ltLimitL;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn824 : オプションモニタ１選択										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_opMon1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MT_HNDL		*mtHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	mtHdl = &Axis->NetAxisIf->mt_hdl;

	mtHdl->svCmd->SelMon5 = Prm->opMon1;
	mtHdl->mtPrm.BasePrm.opMon[0] = Prm->opMon1;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn825 : オプションモニタ２選択										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_opMon2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MT_HNDL		*mtHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	mtHdl = &Axis->NetAxisIf->mt_hdl;

	mtHdl->svCmd->SelMon6 = Prm->opMon2;
	mtHdl->mtPrm.BasePrm.opMon[1] = Prm->opMon2;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn826 : 非常停止用１段目直線減速定数 [[1000ref/s^s]]				for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate1Emg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG data;

	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;
	
	/* 単位計算	*/
	data = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	
	/* データセット	*/
	regHdl->mtPrm->BasePrm.StopPrm.decrate1 = data;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn827 : 非常停止用２段目直線減速定数 [[1000ref/s^s]]				for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decRate2Emg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG data;

	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;

	/* 単位計算	*/
	data = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.PosingRateKx);
	if(data >= 0x40000000)
	{
		data = 0x3FFFFFFF;
	}
	
	/* データセット	*/
	regHdl->mtPrm->BasePrm.StopPrm.decrate2 = data;
	
	return PRM_RSLT_SUCCESS;
}
/****************************************************************************************************/
/*		Pn828 : 非常停止用減速定数切替え速度 [[100ref/s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_decChgSpdEmg(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	LONG		data;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw != 0)	return PRM_RSLT_SUCCESS;
	
	/* 単位計算	*/
#if (FLOAT_USE==TRUE)
	data = (LONG)( (float)*pValue * 100 * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	data = MlibMulgain32( *pValue * 100, regHdl->mtPrm->BasePrm.PosingRateKx);
#endif /* FLOAT_USE */
	/* データセット	*/
	regHdl->mtPrm->BasePrm.StopPrm.altspd_d = data;
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn829 : SVOFF待ち時間(減速停止SVOFF時)								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_svOffTime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG	data;
	LONG	sx, kx;
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	sx = 0;

	/* サーボOFF減速停止用タイマの設定 */
	data = Prm->svOffTime * 10;
	kx = MlibPcalKxgain( data, 4000, regHdl->mtPrm->BasePrm.MotionCycle, &sx, 24 );
#if (FLOAT_USE==TRUE)
	regHdl->mtPrm->MtSeqPrm.rampTimeOut = 1 * kx;
#else
	regHdl->mtPrm->MtSeqPrm.rampTimeOut = MlibMulgain32( 1, kx );
#endif /* FLOAT_USE */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn830 : 機械一回転当たりの指令単位量L [[ref]]						for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_rotrefL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		return( TRUE );
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn832 : 機械一回転当たりのステーション数							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_st_num(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
		return( TRUE );
}
/*--------------------------------------------------------------------------------------------------*/


/****************************************************************************************************/
/*		Pn834 : １段目直線加速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_accRate12L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;

	MPrmSetAccRate1( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->accRate12L );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN834);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn836 : ２段目直線加速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_accRate22L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	MPrmSetAccRate2( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->accRate22L );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN836);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn838 : 加速定数切替え速度 2nd L [[ref/s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_accChgSpd2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;

	/* 加速度切り替え速度設定 */
#if (FLOAT_USE==TRUE)
	regHdl->mtPrm->MgPrm.altspd_a = (LONG)( (float)Prm->accChgSpd2L * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_a = MlibMulgain32( Prm->accChgSpd2L, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN838);

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn83A : １段目直線減速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decRate12L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	MPrmSetDecRate1(regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->decRate12L);
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN83A);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn83C : ２段目直線減速定数 2nd L [[1000ref/s^s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decRate22L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	MPrmSetDecRate2( regHdl->mtPrm, &regHdl->mtPrm->MgPrm, Prm->decRate22L );
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN83C);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn83E : 減速定数切替え速度 2nd L [[ref/s]]							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decChgSpd2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;
	
	/* 減速度り替え速度設定 */
#if (FLOAT_USE==TRUE)
	regHdl->mtPrm->MgPrm.altspd_d = (LONG)( (float)Prm->decChgSpd2L * regHdl->mtPrm->BasePrm.PosingSpdKx );
#else
	regHdl->mtPrm->MgPrm.altspd_d = MlibMulgain32( Prm->decChgSpd2L, regHdl->mtPrm->BasePrm.PosingSpdKx );
#endif /* FLOAT_USE */
	MBnkSetDefData(regHdl->mtPrm, *pValue, MTBNK_PN83E);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn840 : 非常停止用２段目直線減速定数　2nd L	[[1000ref/s^s]]			for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_decRateEmg2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG data;
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	/* <S07E>;ﾛｰｶﾙ変数設定箇所変更 */
	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	
	/* Pn833:POSING定数選択 -> "Short Parameter" or "Long Parameter"	*/
	if(regHdl->iniPrm.rateChgSw == 0)	return PRM_RSLT_SUCCESS;

	/* 非常停止用減速度2設定 */
#if (FLOAT_USE==TRUE)
	data = (LONG)( (float)Prm->decRateEmg2L * regHdl->mtPrm->BasePrm.PosingRateKx );
#else
	data = MlibMulgain32( Prm->decRateEmg2L, regHdl->mtPrm->BasePrm.PosingRateKx );
#endif /* FLOAT_USE */
	if( data >= 0x40000000 )
	{
		data = 0x3FFFFFFF;
	}
	regHdl->mtPrm->BasePrm.StopPrm.altspd_d = 0;
	regHdl->mtPrm->BasePrm.StopPrm.decrate2 = data;
	regHdl->mtPrm->BasePrm.StopPrm.decrate1 = data;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn842 : 原点復帰アプローチ速度１ 2nd L [[100ref/s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd12L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
PRM_RSLT rc;

	rc = pncal_zretSpd1(Cmd, ArrayIdx, Axis, pValue);
	
	return rc;
}
/*--------------------------------------------------------------------------------------------------*/



/****************************************************************************************************/
/*		Pn844 : 原点復帰アプローチ速度２ 2nd L [[100ref/s]]					for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT	pncal_zretSpd22L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
PRM_RSLT rc;

	rc = pncal_zretSpd2(Cmd, ArrayIdx, Axis, pValue);
	
	return rc;
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn850 : 連続ラッチシーケンス数										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltSeqNum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチシーケンス数設定 */
	regHdl->mtPrm->LtPrm.seqNum = (USHORT)Prm->ltSeqNum;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn851 : 連続ラッチ回数												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltLoopNum(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチシーケンス回数設定 */
	regHdl->mtPrm->LtPrm.loopCnt = (USHORT)Prm->ltLoopNum;

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn852 : 連続ラッチシーケンス設定1-4									for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltSeqSet1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	PRMDATA		*Prm;

	Prm = Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* ラッチシーケンス信号設定 */
	regHdl->mtPrm->LtPrm.ltSgnl[0] = (UCHAR)( Prm->ltSeqSet1       & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[1] = (UCHAR)((Prm->ltSeqSet1>>4  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[2] = (UCHAR)((Prm->ltSeqSet1>>8  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[3] = (UCHAR)((Prm->ltSeqSet1>>12 ) & 0xF );

	regHdl->mtPrm->LtPrm.ltSgnl[4] = (UCHAR)((Prm->ltSeqSet2     ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[5] = (UCHAR)((Prm->ltSeqSet2>>4  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[6] = (UCHAR)((Prm->ltSeqSet2>>8  ) & 0xF );
	regHdl->mtPrm->LtPrm.ltSgnl[7] = (UCHAR)((Prm->ltSeqSet2>>12 ) & 0xF );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn853 : 連続ラッチシーケンス設定5-8									for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_ltSeqSet2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	pncal_ltSeqSet1( Cmd, ArrayIdx, Axis, pValue );

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn860 : SVCMD_IO(入力信号モニタ)割付機能1							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
	
	MPrmSetInSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 0);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn861 : SVCMD_IO(入力信号モニタ)割付機能2							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
	
	MPrmSetInSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 2);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn862 : SVCMD_IO(入力信号モニタ)割付機能3							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
	
	MPrmSetInSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 4);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn863 : SVCMD_IO(入力信号モニタ)割付機能4							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo4(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
	
	MPrmSetInSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 6);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn864 : SVCMD_IO(入力信号モニタ)割付機能5							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo5(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)/* <S1C0> */
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
	
	MPrmSetInSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 8);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn865 : SVCMD_IO(入力信号モニタ)割付機能6							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo6(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)/* <S1C0> */
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);
	
	MPrmSetInSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 10);
	
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn866 : SVCMD_IO(入力信号モニタ)割付機能7							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIo7(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn868 : SVCMD_IO(出力信号モニタ)割付機能1							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIoOut1(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CCSW_SFTID == SFTID_EXIST )
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);

	MPrmSetOutSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 0);
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn869 : SVCMD_IO(出力信号モニタ)割付機能2							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_cmdIoOut2(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CCSW_SFTID == SFTID_EXIST )
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);

	MPrmSetOutSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 2);
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn86A : SVCMD_IO(出力信号モニタ)割付機能3							for	MECHATROLINK3-IF	*/
/****************************************************************************************************//* <S1C0> */
PRM_RSLT pncal_cmdIoOut3(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if ( CCSW_SFTID == SFTID_EXIST )
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);

	MPrmSetOutSigMonitor(regHdl->mtPrm, (USHORT)*pValue, 4);
#endif

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn884 : 通信異常時動作設定											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_commErrBehavior(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	/* COMM_ALM発生時 BlkRelease設定 */
	if( (*pValue & 0x00000001) != 0 )
	{
		Axis->NetAxisIf->alm_hdl.CommAlmBlkCancel = TRUE;
	}
	else
	{
		Axis->NetAxisIf->alm_hdl.CommAlmBlkCancel = FALSE;
	}
		
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*		Pn88F : 通信CMD/RSPデータ制御										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_comDataCtrl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/
/****************************************************************************************************/
/*		Pn920-Pn95F : バンクデータ設定										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
static PRM_RSLT	pncal_bnkData( AXIS_HANDLE *Axis, USHORT no )
{
	REG_HNDL	*regHdl;
	regHdl = &(Axis->NetAxisIf->reg_hdl);

	MBnkSetData(regHdl->mtPrm, &regHdl->Prm->bnkData[0x00], no);/* <S080> */
	
	return PRM_RSLT_SUCCESS;
}

PRM_RSLT pncal_bnkData00( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x00);}
PRM_RSLT pncal_bnkData01( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x01);}
PRM_RSLT pncal_bnkData02( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x02);}
PRM_RSLT pncal_bnkData03( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x03);}
PRM_RSLT pncal_bnkData04( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x04);}
PRM_RSLT pncal_bnkData05( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x05);}
PRM_RSLT pncal_bnkData06( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x06);}
PRM_RSLT pncal_bnkData07( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x07);}
PRM_RSLT pncal_bnkData08( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x08);}
PRM_RSLT pncal_bnkData09( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x09);}
PRM_RSLT pncal_bnkData0A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0A);}
PRM_RSLT pncal_bnkData0B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0B);}
PRM_RSLT pncal_bnkData0C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0C);}
PRM_RSLT pncal_bnkData0D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0D);}
PRM_RSLT pncal_bnkData0E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0E);}
PRM_RSLT pncal_bnkData0F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x0F);}
PRM_RSLT pncal_bnkData10( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x10);}
PRM_RSLT pncal_bnkData11( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x11);}
PRM_RSLT pncal_bnkData12( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x12);}
PRM_RSLT pncal_bnkData13( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x13);}
PRM_RSLT pncal_bnkData14( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x14);}
PRM_RSLT pncal_bnkData15( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x15);}
PRM_RSLT pncal_bnkData16( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x16);}
PRM_RSLT pncal_bnkData17( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x17);}
PRM_RSLT pncal_bnkData18( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x18);}
PRM_RSLT pncal_bnkData19( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x19);}
PRM_RSLT pncal_bnkData1A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1A);}
PRM_RSLT pncal_bnkData1B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1B);}
PRM_RSLT pncal_bnkData1C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1C);}
PRM_RSLT pncal_bnkData1D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1D);}
PRM_RSLT pncal_bnkData1E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1E);}
PRM_RSLT pncal_bnkData1F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x1F);}
PRM_RSLT pncal_bnkData20( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x20);}
PRM_RSLT pncal_bnkData21( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x21);}
PRM_RSLT pncal_bnkData22( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x22);}
PRM_RSLT pncal_bnkData23( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x23);}
PRM_RSLT pncal_bnkData24( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x24);}
PRM_RSLT pncal_bnkData25( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x25);}
PRM_RSLT pncal_bnkData26( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x26);}
PRM_RSLT pncal_bnkData27( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x27);}
PRM_RSLT pncal_bnkData28( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x28);}
PRM_RSLT pncal_bnkData29( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x29);}
PRM_RSLT pncal_bnkData2A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2A);}
PRM_RSLT pncal_bnkData2B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2B);}
PRM_RSLT pncal_bnkData2C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2C);}
PRM_RSLT pncal_bnkData2D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2D);}
PRM_RSLT pncal_bnkData2E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2E);}
PRM_RSLT pncal_bnkData2F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x2F);}
PRM_RSLT pncal_bnkData30( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x30);}
PRM_RSLT pncal_bnkData31( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x31);}
PRM_RSLT pncal_bnkData32( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x32);}
PRM_RSLT pncal_bnkData33( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x33);}
PRM_RSLT pncal_bnkData34( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x34);}
PRM_RSLT pncal_bnkData35( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x35);}
PRM_RSLT pncal_bnkData36( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x36);}
PRM_RSLT pncal_bnkData37( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x37);}
PRM_RSLT pncal_bnkData38( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x38);}
PRM_RSLT pncal_bnkData39( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x39);}
PRM_RSLT pncal_bnkData3A( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3A);}
PRM_RSLT pncal_bnkData3B( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3B);}
PRM_RSLT pncal_bnkData3C( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3C);}
PRM_RSLT pncal_bnkData3D( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3D);}
PRM_RSLT pncal_bnkData3E( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3E);}
PRM_RSLT pncal_bnkData3F( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue ){ return pncal_bnkData(Axis, 0x3F);}

#ifdef M3FSEL_ZONEPOUT_ENABLE
/****************************************************************************************************/
/*		Pn870/871 : 位置通過信号出力座標															*/
/****************************************************************************************************/
PRM_RSLT pncal_p_zone1L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	LONG data;
	REGHNDL* regHdl       = GetRegHndl(0);
	ZONE_INFO* zneInfo    = GetZoneInfo(0);

	data = ((regHdl->Prm->p_zone1H << 16) | regHdl->Prm->p_zone1L);
	ZneGetZoneLimt(&zneInfo->pZone1Limt, data);

	return PRM_RSLT_SUCCESS;

}
/****************************************************************************************************/
/*		Pn872/873 : 位置通過信号出力座標															*/
/****************************************************************************************************/
PRM_RSLT pncal_n_zone1L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	LONG data;
	REGHNDL* regHdl    = GetRegHndl(0);
	ZONE_INFO* zneInfo = GetZoneInfo(0);

	data = ((regHdl->Prm->n_zone1H << 16) | regHdl->Prm->n_zone1L);
	ZneGetZoneLimt(&zneInfo->nZone1Limt, data);

	return PRM_RSLT_SUCCESS;

}
/****************************************************************************************************/
/*		Pn874/875 : 位置通過信号出力座標															*/
/****************************************************************************************************/
PRM_RSLT pncal_p_zone2L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	LONG data;
	REGHNDL* regHdl    = GetRegHndl(0);
	ZONE_INFO* zneInfo = GetZoneInfo(0);

	data = ((regHdl->Prm->p_zone2H << 16) | regHdl->Prm->p_zone2L);
	ZneGetZoneLimt(&zneInfo->pZone2Limt, data);

	return PRM_RSLT_SUCCESS;

}
/****************************************************************************************************/
/*		Pn876/877 : 位置通過信号出力座標															*/
/****************************************************************************************************/
PRM_RSLT pncal_n_zone2L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	LONG data;
	REGHNDL* regHdl    = GetRegHndl(0);
	ZONE_INFO* zneInfo = GetZoneInfo(0);

	data = ((regHdl->Prm->n_zone2H << 16) | regHdl->Prm->n_zone2L);
	ZneGetZoneLimt(&zneInfo->nZone2Limt, data);

	return PRM_RSLT_SUCCESS;

}
#else	/* @CHECK@ 暫定処理 */
PRM_RSLT pncal_p_zone1L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet ){return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_n_zone1L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet ){return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_p_zone2L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet ){return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_n_zone2L( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet ){return( PRM_RSLT_SUCCESS );}
#endif

/*--------------------------------------------------------------------------------------------------*/

#ifdef M3FSEL_MSLV_ENABLE
/****************************************************************************************************/
/*		Pn980 : 他局モニタアドレス																	*/
/****************************************************************************************************/
PRM_RSLT pncal_monAdr( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	REGHNDL* regHdl    = GetRegHndl(0);

	MComGetMonAdr(GetMComHndl(0, 0), regHdl->Prm->monAdr);

	return PRM_RSLT_SUCCESS;

}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn981 : 他局通信データサイズ																*/
/****************************************************************************************************/
PRM_RSLT pncal_monDataSize( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	REGHNDL* regHdl    = GetRegHndl(0);

	MComGetMonDataSize(GetMComHndl(0, 0), regHdl->Prm->monDataSize);

	return PRM_RSLT_SUCCESS;

}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn982 : 他局モニタ機能選択スイッチ															*/
/****************************************************************************************************/
PRM_RSLT pncal_monFuncSel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	REGHNDL* regHdl    = GetRegHndl(0);

	MCmdGetMonFuncMode( GetMCmdHndl(0, 0), regHdl->Prm->monFuncSel & 0x0F );
	MCmdGetMonStopCtrlMode( GetMCmdHndl(0, 0), (regHdl->Prm->monFuncSel & 0xF0) >> 4 );
	ComSetMslvEnable(GetComHndl(0), regHdl->Prm->monFuncSel & 0x0F );

	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn983 : 停止動作の入力指令 モニタ設定														*/
/****************************************************************************************************/
PRM_RSLT pncal_monDataSel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	REGHNDL* regHdl    = GetRegHndl(0);

	MCmdGetMonDataSel( GetMCmdHndl(0,0), regHdl->Prm->monDataSel );
	MComGetMonDataType( GetMComHndl(0,0), regHdl->Prm->monDataSel );

	return PRM_RSLT_SUCCESS;
}
/*--------------------------------------------------------------------------------------------------*/

/****************************************************************************************************/
/*		Pn984 : 追従停止速度 																		*/
/****************************************************************************************************/
PRM_RSLT pncal_trceStpSpd( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	REGHNDL* regHdl = GetRegHndl(0);

	MCmdGetTraceStopSpd(GetMCmdHndl(0,0), regHdl->Prm->trceStpSpd);

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn985 : 協調停止調整パラメータ																*/
/****************************************************************************************************/
PRM_RSLT pncal_monAdj( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )
{
	REGHNDL* regHdl    = GetRegHndl(0);

	MCmdGetPmafbunm(GetMCmdHndl(0,0), (regHdl->Prm->monAdj&0xFF));
	MCmdGetRatioOfVFF(GetMCmdHndl(0,0), ((regHdl->Prm->monAdj>>8)&0xFF));

	return PRM_RSLT_SUCCESS;
}
#else
PRM_RSLT pncal_monAdr( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )		{return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_monDataSize( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )	{return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_monFuncSel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )	{return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_monDataSel( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )	{return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_trceStpSpd( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )	{return( PRM_RSLT_SUCCESS );}
PRM_RSLT pncal_monAdj( PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValuet )		{return( PRM_RSLT_SUCCESS );}
#endif


/****************************************************************************************************/
/*		PnA16 : リニアスケールピッチ										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_scalepitchL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;

	Prm	= Axis->UniPrms->Prm;
	switch(Cmd)
	{
	case PRM_ACCCMD_WRITE:
		Prm->scalepitch2l = *pValue / 10;
		break;
	case PRM_ACCCMD_EEPWRITE:
		Prm->scalepitch2l = *pValue / 10;
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_scalepitch2l, 0);
		break;
	case PRM_ACCCMD_READ:
		*pValue = Prm->scalepitch2l * 10;
		break;
	default:
		break;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA42 :電子ギア比（分子） 																	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_ratbL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->ratb2l = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_ratb2l(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_ratb2l, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA44 : 電子ギア比（分母）																	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_rataL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->rata2l = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_rata2l(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_rata2l, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA46 : アブソPG原点位置オフセット									for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_absOffsetL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->absOffsetL = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_absOffsetL(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_absOffsetL, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA48 : マルチターンリミット設定							for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_mtLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->mtlmt = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_mtlmt(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_mtlmt, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA4A : リミット設定												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_limitSwL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	USHORT	 	pOtMask, nOtMask, SotMask;
	LONG		data;

	Prm	= Axis->UniPrms->Prm;
	regHdl = &Axis->NetAxisIf->reg_hdl;

	/* マスク情報読み出し */
	pOtMask =  *pValue & 0x01;			/* 0:有効、1:無効 */
	nOtMask = (*pValue & 0x02) >> 1;	/* 0:有効、1:無効 */
	SotMask = (*pValue & 0x30) >> 4;	/* 0:無効、1:有効 */

	/*==========================================================================*/
	/* Bit0:P-OT(0:有効、1：無効) ->Pn50Aを書き換える 								*/
	/*==========================================================================*/
	/*---- 実パラメータ値作成	----------*/
	data = Prm->insel1 & 0x0FFF;
	if( pOtMask == TRUE )
	{/* P_OT無効　Pn50A.3に8をセットする。 */
		data = data | 0x8000;
	}
	else
	{/* P_OT有効　保存した値をPn50A.3にセットする。 */
		data = data | ((Prm->m3_Pn50A << 12) & 0xF000);
	}
	/*---- 実パラメータRAM書込み ----------*/
	Prm->insel1 = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_insel1(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_insel1, 0);
	}

	/*==========================================================================*/
	/* Bit1:N-OT(0:有効、1：無効) ->Pn50Bを書き換える  								*/
	/*==========================================================================*/
	/*---- 実パラメータ値作成	----------*/
	data = Prm->insel2 & 0xFFF0;
	if( nOtMask == TRUE )
	{/* N_OT無効　Pn50B.0に8をセットする。 */
		data = data | 0x0008;
	}
	else
	{/* N_OT有効　保存した値をPn50B.0にセットする。 */
		data = data | (Prm->m3_Pn50B & 0x000F);
	}
	/*---- 実パラメータRAM書込み ----------*/
	Prm->insel2 = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_insel1(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_insel2, 0);
	}

	/*==========================================================================*/
	/* Bit4:P-SOT(0:無効,1:有効) ->Pn801を書き換える  								*/
	/* Bit5:N-SOT(0:無効,1:有効) ->Pn801を書き換える  								*/
	/*==========================================================================*/
	/*---- 実パラメータ値作成	----------*/
	data = Prm->sLimitSw & 0xFFF0;
	SotMask = 3 - SotMask;	/* ビットのオン・オフが反対 */
	data = data | (SotMask & 0x0003);
	/*---- 実パラメータRAM書込み ----------*/
	Prm->sLimitSw = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_sLimitSw(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_sLimitSw, 0);
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA4C : 正側ソフトリミット値										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_psLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->p_sLimitL = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_p_sLimitL(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_p_sLimitL, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnA50 : 負側ソフトリミット値										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_nsLimitL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->n_sLimitL = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_n_sLimitL(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_n_sLimitL, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnAC2 : 速度ループゲイン											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_KvL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	loophz;

	Prm	= Axis->UniPrms->Prm;
	/*---- 実パラメータRAM書込み ----------*/
	loophz = (*pValue / 100);
	Prm->loophz = (USHORT)loophz;
	/*----  実パラメータ計算実行 ----------*/
	pncal_loophz(PRM_ACCCMD_RECALC, 0, Axis, &loophz);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_loophz, 0);
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnAC4 : 速度ループ積分時定数										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_TviL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	pitime;

	Prm	= Axis->UniPrms->Prm;

	pitime = (*pValue / 10);
	/*---- 実パラメータRAM書込み ----------*/
	Prm->pitime = (USHORT)pitime;
	/*----  実パラメータ計算実行 ----------*/
	pncal_pitime(PRM_ACCCMD_RECALC, 0, Axis, &pitime);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_pitime, 0);
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnAC6 : 位置ループゲイン											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_KpL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	posgn;

	Prm	= Axis->UniPrms->Prm;

	posgn = (*pValue / 100);
	/*---- 実パラメータRAM書込み ----------*/
	Prm->posgn = (USHORT)posgn;
	/*----  実パラメータ計算実行 ----------*/
	pncal_posgn(PRM_ACCCMD_RECALC, 0, Axis, &posgn);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_posgn, 0);
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnAC8 : フィードフォワード [%]										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_FFGainL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->ffgn = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_ffgn(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_ffgn, 0);
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnACA : 位置ループ積分時定数										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_TpiL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{

	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = (*pValue)/100;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->pitfil = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_pitfil(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_pitfil, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnACC : 位置決め完了幅																		*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_psetWidthL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->coinlv2l = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_coinlv2l(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_coinlv2l, 0);
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnACE : 位置決め近傍幅																		*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_nearWidthL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->nearlv2l = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_nearlv2l(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_nearlv2l, 0);
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnB02 : 指数関数加減速時定数										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_filExpTbL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue / 100;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->filExpTb = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_filExpTb(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_filExpTb, 0);
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnB04 : 移動平均時間												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_filMaTbL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue / 100;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->filMaTb = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_filMaTb(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_filMaTb, 0);
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnB06 : 外部信号位置決め最終走行距離								for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_exDistL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->exDistL = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_exDistL(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_exDistL, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB08 : 原点復帰アプローチ速度										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_zretSpd1L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	REGIF_CONFIG_T	*hReg;/* <S027> */
	LONG		data, chekData;/* <S027> */

	if(Cmd == PRM_ACCCMD_RECALC)/* <S027> */
	{/* 初期化時の計算および再計算では、実施しない	*/
		return PRM_RSLT_SUCCESS;
	}

	Prm	= Axis->UniPrms->Prm;		/*<S008>*/
	regHdl = &Axis->NetAxisIf->reg_hdl;		/*<S008>*/
	hReg = Axis->RegManager->hPnReg;			/* <S027> */
	/*-- 単位変換処理 パラメータ設定 → [ref/s]-----------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	data = (LONG)((float)*pValue * regHdl->mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef);
#else
	data = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef);
#endif /* FLOAT_USE */
	data = data/100;
	
	/*-- Pn817有効無効判断 ----------------------------------------------------------------  <S027> --*/
//	if( (0 < *pValue) && (*pValue <= 0xFFFF) )	
	if( (0 < data) && (data <= 0xFFFF) )		/* <S0BF> */
	{
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{/* EEPROMの値を利用する。	*/
			if(PrmLoadValueFromEeprom( hReg, &pndef_m3_zretSpd1L, 0, &chekData ) != PRM_RSLT_SUCCESS)
			{
				return PRM_RSLT_CONDITION_ERR;
			}
		}
		else
		{/* RAMの値を利用する。	*/
			chekData = Prm->zretSpd1;
		}
	}
	else
	{/* Pn842に値を書込む	*/
		chekData = 0;
	}
	
	/*-- 連携ﾊﾟﾗﾒｰﾀ書込み処理 ----------------------------------------------------------------------*/
//	if( (0 < *pValue) && (*pValue <= 0xFFFF) )
	if( chekData != 0 )/* <S027> */
	{/* Pn817と連携	*/
		/*---- 実パラメータRAM書込み ----------*/
		Prm->zretSpd1 = (USHORT)data;
		/*----  実パラメータ計算実行 ----------*/
		pncal_zretSpd1(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretSpd1, 0);
		}
	}
	else
	{/* Pn842と連携する	*/
		/*---- 実パラメータRAM書込み ----------*/
		Prm->zretSpd12L = data;
		Prm->zretSpd1 = 0;		/* <S122> */
		/*----  実パラメータ計算実行 ----------*/
		pncal_zretSpd12L(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretSpd12L, 0);
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretSpd1, 0);
		}
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB0A : 原点復帰クリープ速度										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_zretSpd2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	REG_HNDL	*regHdl;
	REGIF_CONFIG_T	*hReg;/* <S027> */
	LONG		data, chekData;/* <S027> */

	if(Cmd == PRM_ACCCMD_RECALC)/* <S027> */
	{/* 初期化時の計算および再計算では、実施しない	*/
		return PRM_RSLT_SUCCESS;
	}

	Prm	= Axis->UniPrms->Prm;		/*<S008>*/
	regHdl = &Axis->NetAxisIf->reg_hdl;		/*<S008>*/
	hReg = Axis->RegManager->hPnReg;			/* <S027> */

	/*-- 単位変換処理 パラメータ設定 → [ref/s]-----------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	data = ((float)*pValue * regHdl->mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef);
#else
	data = MlibMulgain32(*pValue, regHdl->mtPrm->BasePrm.m3ConvUnit.m3RpmtoRef);
#endif /* FLOAT_USE */
	data = data/100;

	/*-- Pn818有効無効判断 ----------------------------------------------------------------  <S027> --*/
//	if( (0 < *pValue) && (*pValue <= 0xFFFF) )	
	if( (0 < data) && (data <= 0xFFFF) )		/* <S0BF> */
	{
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{/* EEPROMの値を利用する。	*/
			if(PrmLoadValueFromEeprom( hReg, &pndef_m3_zretSpd2L, 0, &chekData ) != PRM_RSLT_SUCCESS)
			{
				return PRM_RSLT_CONDITION_ERR;
			}
		}
		else
		{/* RAMの値を利用する。	*/
//			chekData = Prm->zretSpd2;
			chekData = data;			/*  <S122> */
		}
	}
	else
	{/* Pn844に値を書込む	*/
		chekData = 0;
	}
	
	
	/*-- 連携ﾊﾟﾗﾒｰﾀ書込み処理 ----------------------------------------------------------------------*/
//	if( (0 < *pValue) && (*pValue <= 0xFFFF) )
	if( chekData != 0 )/* <S027> */
	{/* Pn818と連携	*/
		/*---- 実パラメータRAM書込み ----------*/
		Prm->zretSpd2 = (USHORT)data;
		/*----  実パラメータ計算実行 ----------*/
		pncal_zretSpd2(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretSpd2, 0);
		}
	}
	else
	{/* Pn844と連携する	*/
		/*---- 実パラメータRAM書込み ----------*/
		Prm->zretSpd22L = data;
		Prm->zretSpd2 = 0;			/* <S122> */
		/*----  実パラメータ計算実行 ----------*/
		pncal_zretSpd22L(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretSpd22L, 0);
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretSpd2, 0);	/* <S122> */
		}
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnB0C : 原点復帰最終走行距離										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_zretDistL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->zretDistL = data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_zretDistL(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zretDistL, 0);
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnB0E : 固定モニタ選択１											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_cPrmMon1L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	RSP_HNDL	*rspHdl;
	rspHdl = &Axis->NetAxisIf->rsp_hdl;

	/* 固定モニタ選択１作成 */
	RspSetcPrmMon(rspHdl, *pValue, 0);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB10 : 固定モニタ選択２											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_cPrmMon2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	RSP_HNDL	*rspHdl;
	rspHdl = &Axis->NetAxisIf->rsp_hdl;

	/* 固定モニタ選択１作成 */
	RspSetcPrmMon(rspHdl, *pValue, 1);
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB12 : SEL_MONのモニタ選択１										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_selMon1L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	regHdl->mtPrm->BasePrm.m3_selMon1 = *pValue;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB14 : SEL_MONのモニタ選択２										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_selMon2L(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	REG_HNDL	*regHdl;
	regHdl = &Axis->NetAxisIf->reg_hdl;
	regHdl->mtPrm->BasePrm.m3_selMon2 = *pValue;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB16 : 原点検出幅													for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_zpWidthL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->zpWidth = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_zpWidth(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_zpWidth, 0);
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnB18 : 正転トルク制限値 [%]										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_pTrqLmtL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->outlmf1 = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_outlmf1(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_outlmf1, 0);
	}
	return PRM_RSLT_SUCCESS;

}
/****************************************************************************************************/
/*		PnB1A : 逆転トルク制限値 [%]										for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_nTrqLmtL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	LONG	data;

	Prm	= Axis->UniPrms->Prm;
	data = *pValue;
	/*---- 実パラメータRAM書込み ----------*/
	Prm->outlmr1 = (USHORT)data;
	/*----  実パラメータ計算実行 ----------*/
	pncal_outlmr1(PRM_ACCCMD_RECALC, 0, Axis, &data);
	/*----  実パラメータEEPROM書き込み-----*/
	if(Cmd == PRM_ACCCMD_EEPWRITE)
	{
		PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_outlmr1, 0);
	}
	return PRM_RSLT_SUCCESS;

}

/****************************************************************************************************/
/*		PnB1C : ゼロ速度検出幅												for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_zSpdLvL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	LONG		data;

	Prm = Axis->UniPrms->Prm;

	/* 単位変換処理  min-1 → パラメータ設定*/
	data = (*pValue / 1000);

	if(Axis->MencV->AxisMotType == MOTTYPE_ROTARY)
	{/* 回転型 */
		/*---- 実パラメータRAM書込み ----------*/
		Prm->tgonlv = (USHORT)data;
		/*----  実パラメータ計算実行 ----------*/
		pncal_tgonlv(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_tgonlv, 0);
		}
	}
	else
	{/* リニア型 */
		/*---- 実パラメータRAM書込み ----------*/
		Prm->tgonlv_l = (USHORT)data;
		/*----  実パラメータ計算実行 ----------*/
		pncal_tgonlv_l(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_tgonlv_l, 0);
		}
	}

	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		PnB1E : 速度一致信号検出幅											for	MECHATROLINK3-IF	*/
/****************************************************************************************************/
PRM_RSLT pncal_m3_vCmpLvL(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	LONG		data;

	Prm = Axis->UniPrms->Prm;

	/* 単位変換処理  min-1 → パラメータ設定*/
	data = (*pValue / 1000);

	if(Axis->MencV->AxisMotType == MOTTYPE_ROTARY)
	{/* 回転型 */
		/*---- 実パラメータRAM書込み ----------*/
		Prm->vcmplv = (USHORT)data;
		/*----  実パラメータ計算実行 ----------*/
		pncal_vcmplv(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_vcmplv, 0);
		}
	}
	else
	{/* リニア型 */
		/*---- 実パラメータRAM書込み ----------*/
		Prm->vcmplv_l = (USHORT)data;
		/*----  実パラメータ計算実行 ----------*/
		pncal_vcmplv_l(PRM_ACCCMD_RECALC, 0, Axis, &data);
		/*----  実パラメータEEPROM書き込み-----*/
		if(Cmd == PRM_ACCCMD_EEPWRITE)
		{
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_vcmplv_l, 0);
		}
	}

	return PRM_RSLT_SUCCESS;
}
#endif //CSW_NETIF_TYPE == NETIF_M3_TYPE




/* ==========================================================================================================	*/
/* ここからリニア用パラメータ */
/****************************************************************************************************/
/*		Pn181 : モードスイッチ(速度指令) [mm/s]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mswrf_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MODE_SW		*ModeSwData;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		ModeSwData->conf.MSWSpdLevel =
			BprmSpeedLevelCal( Bprm, Prm->mswrf_l*10, 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn182 : モードスイッチ(加速度) [mm/s2]														*/
/****************************************************************************************************/
PRM_RSLT pncal_mswacc_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
#if (FLOAT_USE==TRUE)
	float	fw;
#else
	LONG		kx, sx;
#endif /* FLOAT_USE */

	MODE_SW		*ModeSwData;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	ModeSwData = &(Axis->BaseControls->ModeSwData);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

#if (FLOAT_USE==FALSE)
	sx = 0;
#endif
	
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*					   Prm.mswacc * Bprm.Knorspd * SCCYCLEUS										*/
/*		MSWAccLevel = ---------------------------------------										*/
/*									 1000000														*/
/*																									*/
/*		Prm.mswacc_l   : [mm/s/s]																	*/
/*		Bprm.Knorspd : [r/min,mm/s --> NorSpd]														*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
#if (FLOAT_USE==TRUE)
		fw = (float)Prm->mswacc_l * Bprm->Knorspd;
		fw = fw * (float)KPI_SCCYCLEUS / 1000000.0F;
		if( fw > (float)0x01000000 )
		{
			ModeSwData->conf.MSWAccLevel = 0x01000000;
		}
		else
		{
			ModeSwData->conf.MSWAccLevel = (LONG)fw;
		}
#else
		kx = MlibScalKxkskx( Prm->mswacc_l, Bprm->Knorspd, 1, &sx, 0 );
		kx = MlibPcalKxgain( kx,  KPI_SCCYCLEUS, 1000000, &sx, -24 );
		ModeSwData->conf.MSWAccLevel = kx;
#endif /* FLOAT_USE */
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn282 : リニアスケールピッチ [0.01um]														*/
/****************************************************************************************************/
PRM_RSLT pncal_scalepitch2l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	/* 共通パラメータ変更処理 */
//	pncal_m3_convert_scalepitch( Cmd, ArrayIdx, Axis, pValue );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn384 : 振動検出レベル(リニア) [mm/s]														*/
/****************************************************************************************************/
PRM_RSLT pncal_vibnorm_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		Axis->DetVib->conf.VibCheckLevel =
			BprmSpeedLevelCal( Bprm, ( 30 * Prm->vibnorm_l * Prm->vibsens / 100 ), 0 );
		/* 設定レベルに変更が合った場合も最大最小をクリアする */
		DetVibResetMaxMinSpeedError( Axis->DetVib );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn387 : 慣性モーメント同定中の振動検出レベル(リニア) [mm/s]									*/
/****************************************************************************************************/
PRM_RSLT pncal_Pn387(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		Axis->DetVib->conf.JstVibCheckLevel =
			BprmSpeedLevelCal( Bprm, (Prm->Pn387 * 10), 0 );
		Axis->DetVib->conf.JstVibCheckLevel <<= 3;
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn480 : 推力制御時の速度制限 [mm/s]															*/
/****************************************************************************************************/
PRM_RSLT pncal_tcrlmt_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		if( 0 != ((Prm->tfuncsw >> 4) & 0x0F) )	/*Pn408.1 : 速度制限選択設定 */
		{
			Axis->BaseControls->CtrlCmdPrm.TConSpdLmtLevel =
					BprmSpeedLevelCal( Bprm, ( Prm->tcrlmt_l * 10 ), 1 );
		}
		else
		{
			Axis->BaseControls->CtrlCmdPrm.TConSpdLmtLevel =
					BprmSpeedLevelCal( Bprm, ( Prm->tcrlmt_l * 10 ), 0 );
		}
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn481 : 磁極検出速度ループゲイン [0.1Hz]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetloophz(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedControlPrm( Axis->MpFind,
	                           Axis->UniPrms->Prm->trqfil11,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn482 : 磁極検出速度ループ積分時間 [0.01ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetpitime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedControlPrm( Axis->MpFind,
	                           Axis->UniPrms->Prm->trqfil11,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn483 : 正転推力制限 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_tlmtf_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	Prm = Axis->UniPrms->Prm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		/* [%] --> [2^24/MaxTrq] */
		Axis->BaseControls->TrqLimitData.conf.FwIntTrqLmt =
				BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->tlmtf_l ), 0 );

		/* モータ暴走監視パラメータ計算 */
		PcalMotorOverrun_Linear( Axis );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn484 : 逆転推力制限 [%]																	*/
/****************************************************************************************************/
PRM_RSLT pncal_tlmtr_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		/* [%] --> [2^24/MaxTrq] */
		Axis->BaseControls->TrqLimitData.conf.RvIntTrqLmt =
				BprmTorqueLevelCal( Axis->UniPrms->Bprm, ( 100 * Prm->tlmtr_l ), 0 );

		/* モータ暴走監視パラメータ計算 */
		PcalMotorOverrun_Linear( Axis );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn485 : 磁極検出指令速度 [mm/s]																*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetmaxspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn486 : 磁極検出指令加減速時間 [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetmaxt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn487 : 磁極検出指令一定速時間 [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetclmpt(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn488 : 磁極検出指令待ち時間 [ms]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetwait(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn48D : 磁極検出繰り返し回数 [－]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetrepeat(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	Axis->MpFind->conf.RepeatNum = Axis->UniPrms->Prm->PdetP.pdetrepeat;
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn48E : 磁極検出可動範囲 [mm]																*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetdistok(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcErrorLevel( Axis->MpFind,
	                         Axis->MencV,
	                         &(Axis->UniPrms->Prm->MencP),
	                         Axis->UniPrms->Bprm,
	                         &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn490 : 磁極検出負荷レベル [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetjrate(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedControlPrm( Axis->MpFind,
	                           Axis->UniPrms->Prm->trqfil11,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn491 : 磁極検出積分レスゲイン [0.01%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetintegless(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedControlPrm( Axis->MpFind,
	                           Axis->UniPrms->Prm->trqfil11,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn492 : 磁極検出積分レス開始時間 [%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetinteglesst(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn493 : 磁極検出指令速度 [min-1]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetmaxspd_r(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn494 : 磁極検出可動範囲 [0.001rev]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetdistok_r(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcErrorLevel( Axis->MpFind,
	                         Axis->MencV,
	                         &(Axis->UniPrms->Prm->MencP),
	                         Axis->UniPrms->Bprm,
	                         &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn495 : 磁極検出確認トルク指令 [%]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetmaxtrq(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn496 : 磁極検出確認トルク指令加減時間 [ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetinctime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn497 : 磁極検出確認トルク指令一定時間 [ms]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetconsttime(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn498 : 磁極検出誤差許容範囲 [deg]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetdegreeok(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcErrorLevel( Axis->MpFind,
	                         Axis->MencV,
	                         &(Axis->UniPrms->Prm->MencP),
	                         Axis->UniPrms->Bprm,
	                         &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn499 : 磁極検出速度指令時逆走レベル [%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetrevlvspdref(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn49A : 磁極検出待ち時間時逆走レベル [%]													*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetrevlvwait(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn49B : 磁極検出時過速度レベル [%]															*/
/****************************************************************************************************/
PRM_RSLT pncal_pdetoslevel(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	MpoleFind_CalcSpeedCommandPrm( Axis->MpFind,
	                           Axis->MencV,
	                           Axis->UniPrms->Bprm,
	                           &(Axis->UniPrms->Prm->PdetP) );
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn580 : ゼロクランプレベル [mm/s]															*/
/****************************************************************************************************/
PRM_RSLT pncal_clamp_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)/* <S02B> */
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;
	ZC_CTRL_DATA *pzc;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	pzc = &(Axis->BeSequence->ZcCtrlData);

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{/* リニア */
		pzc->P.ZcSpdLevel =
			BprmSpeedLevelCal( Bprm, ( Prm->clamp_l * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn581 : 回転検出レベル [mm/s]																*/
/****************************************************************************************************//* <S004> */
PRM_RSLT pncal_tgonlv_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )/* <S02B> */
	{/* リニア */
	#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
		pncal_m3_convert_zSpdLv( Cmd, ArrayIdx, Axis, pValue );
	#endif
		Axis->SeqMotSts->conf.TgonSpdLevel =
				BprmSpeedLevelCal( Bprm, ( Prm->tgonlv_l * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn582 : 速度一致信号出力幅 [mm/s]															*/
/****************************************************************************************************/
PRM_RSLT pncal_vcmplv_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;
	BPRMDAT	*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )/* <S02B> */
	{/* リニア */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
		pncal_m3_convert_vCmpLv( Cmd, ArrayIdx, Axis, pValue );
#endif
		Axis->SeqMotSts->conf.VcmpSpdLevel =
				BprmSpeedLevelCal( Bprm, ( Prm->vcmplv_l * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}


/****************************************************************************************************/
/*		Pn583 : ブレーキ指令出力速度レベル [mm/s]													*/
/****************************************************************************************************/
PRM_RSLT pncal_brkspd_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;
	BK_SEQ_DATA		*BkSeqData;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	BkSeqData = &(Axis->BeSequence->BkSeqData);

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		BkSeqData->conf.BkonSpdLevel =
			BprmSpeedLevelCal( Bprm, ( Prm->brkspd_l * 10 ), 0 );
	}
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn584 : サーボオン時速度制限レベル [mm/s]													*/
/****************************************************************************************************/
PRM_RSLT pncal_svonspdlmtlv_l(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CTRL_CMD_PRM	*CtrlCmdPrm;
	PRMDATA			*Prm;
	BPRMDAT			*Bprm;

	CtrlCmdPrm	= &(Axis->BaseControls->CtrlCmdPrm);
	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		CtrlCmdPrm->SvonSpdLmtLevel =
			BprmSpeedLevelCal( Bprm, ( Prm->svonspdlmtlv_l * 10 ), 0 );

		if( CtrlCmdPrm->SvonSpdLmtLevel < 10 )
		{
			CtrlCmdPrm->SvonSpdLmtValue = 1;

		}
		else
		{
			CtrlCmdPrm->SvonSpdLmtValue = CtrlCmdPrm->SvonSpdLmtLevel / 10;
		}
	}

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn585 : プログラムＪＯＧ最高速度(リニア) [mm/s]												*/
/****************************************************************************************************/
//PRM_RSLT pncal_pjoglinspd( ULONG Cmd, ULONG Data, ULONG Digit )
PRM_RSLT pncal_pjoglinspd(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)	/* <S213> */
{
#if 0	/* <S213> */
		if( BoutV.CtrlMode.us[0] == CTRLMODE_PJOG && OpeReg.PjogState == 1 )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogLinspd();		/* プログラムＪＯＧパラメータ計算	*/
		}
#else	/* <S213> */
		if( (Axis->FnCmnCtrl->FnCtrlMcmd == CTRLMODE_PJOG)
										&& (Axis->BaseControls->PJogHdl.PJogState == 1) )
		{
			return( PRM_RSLT_CONDITION_ERR );		/* プログラムＪＯＧ運転未完了		*/
		}
		else
		{
			PcalPjogLinspd( Axis );					/* プログラムＪＯＧパラメータ計算	*/
		}
#endif	/* <S213> */
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn586 : モータ自走冷却率 [%/maxvel]															*/
/****************************************************************************************************/
PRM_RSLT pncal_msrcra(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	LONG		kx, sx;
	LONG		LinearCoolRateGain;
	CHECK_OL	*MotOL;
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;

	Prm = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;
	MotOL = &(Axis->CheckAlarm->MotOL);

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		/*------------------------------------------------------------------------------------------*/
		/*			  1		   Prm.msrcra															*/
		/*	Gain = -------- * ------------ * 2^14													*/
		/*			MaxSpd		 100																*/
		/*------------------------------------------------------------------------------------------*/
		/*	MaxSpd	: [2^24/(rad/s)]	: 回転														*/
		/*			: [2^24/(m/s)]		: リニア													*/
		/*																							*/
		/*	Prm.msrcra : [%]																		*/
		/*------------------------------------------------------------------------------------------*/
		sx = 0;
		kx = MlibScalKxgain( 16384, Prm->msrcra, 100, &sx, 0 );
		LinearCoolRateGain = MlibPcalKxgain( kx, 1, Bprm->NorMaxSpd, &sx, 24 );
	}
	else
	{
		/* リニア以外のときは Gain = 0 */
		LinearCoolRateGain = 0 ;
	}
	/*----------------------------------------------------------------------------------------------*/
	/*	モータ自走冷却率パラメータ計算																*/
	/*----------------------------------------------------------------------------------------------*/
//	MotOL->conf.CoolRateGain = LinearCoolRateGain;					/* <S154> */
	MotOL->conf.NormalArea.CoolRateGain = LinearCoolRateGain;		/* <S154> */

	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		Pn587 : 絶対値スケール用磁極検出実行選択 [－]												*/
/****************************************************************************************************/
PRM_RSLT pncal_pldetsw(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA		*Prm;
	MENCV		*MencV;			/* <S226> */

	Prm = Axis->UniPrms->Prm;
	MencV = Axis->MencV;		/* <S226> */

	if(Cmd == PRM_ACCCMD_READ)
	{
		*pValue = PdetGetPhaseOffsetWriteReq( Axis->PdetAbsSeq );
	}
//	else
	else if((( *pValue & 0x0F ) == 1 )&&( MencV->withPoleSensor == FALSE )&&( MencV->PhaseReady == FALSE ))	/* <S226> */
	{
		if(FALSE != PdetEnablePhaseOffsetWrite( Axis->PdetAbsSeq,
												Axis->MencV,
												(BOOL)(*pValue & 1)) )
		{	/* Clear Pdet complete flag */
			Prm->syssw3 = Prm->syssw3 & (~0x0020);
			PrmStoreValueToEeprom(Axis->RegManager->hPnReg, &pndef_syssw3, 0);
		}
//		Axis->MencV->PdetReq587 = *pValue & 1;		/* <S21C> */
		MencV->PdetReq587 = 1;						/* <S226> */
	}
	return PRM_RSLT_SUCCESS;
}

/* <S0F9> Start */
/****************************************************************************************************/
/*		PnE53 : 速度指令ゼロ調																		*/
/****************************************************************************************************/
PRM_RSLT pncal_zadjvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	CADC	*pAdc;
	PRMDATA	*Prm;

	Prm = Axis->UniPrms->Prm;
	pAdc = Axis->pAdc;

	pAdc->P.ZadjVelo = Prm->zadjvl;

	return PRM_RSLT_SUCCESS;
}
/* <S0F9> End */

/****************************************************************************************************/
/*		PnEE9 : 電圧指令制限レベル(リニア用) [%]																*/
/****************************************************************************************************/
PRM_RSLT pncal_v1max_for_linear(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	PRMDATA	*Prm;

	if( Axis->MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		Prm = Axis->UniPrms->Prm;

		PcalIdrefLimTerm1( &(Axis->BaseControls->WeakenField), Axis->SvAsicRegs,
		    	           Axis->UniPrms->Bprm, Prm->v1max_L, Prm->vmaxid );
	}
	return PRM_RSLT_SUCCESS;
}

/* ---futami---> */
/****************************************************************************************************/
/*		PnEF0 : 温度過熱検出フィルタ時定数 [ms]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ohfil(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEF1 : ユニット内部温度過熱アラームレベル [℃]												*/
/****************************************************************************************************/
PRM_RSLT pncal_mbohlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEF0 : EF2 : コンバータ温度過熱アラームレベル [℃]											*/
/****************************************************************************************************/
PRM_RSLT pncal_convohlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEF3 : 制御基板温度過熱アラームレベル [℃]													*/
/****************************************************************************************************/
PRM_RSLT pncal_ifohlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}

/****************************************************************************************************/
/*		PnEF4 : 温度過熱ワーニングレベル [%]														*/
/****************************************************************************************************/
PRM_RSLT pncal_ohwrnlvl(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
	return PRM_RSLT_SUCCESS;
}
/* <---futami--- */

/* <S14B> Start */
/****************************************************************************************************/
/*		PnFA0 : 速度リップル補償ゲイン(電気角) [%]													*/
/*		PnFA1 : 速度リップル補償第1成分周期(電気角) [電気角周期]									*/
/*		PnFA2 : 速度リップル補償第1成分振幅値(電気角) [0.01%]										*/
/*		PnFA3 : 速度リップル補償第1成分位相(電気角) [deg]											*/
/*		PnFA4 : 速度リップル補償第2成分周期(電気角) [電気角周期]									*/
/*		PnFA5 : 速度リップル補償第2成分振幅値(電気角) [0.01%]										*/
/*		PnFA6 : 速度リップル補償第2成分位相(電気角) [deg]											*/
/*		PnFA7 : 速度リップル補償第3成分周期(電気角) [電気角周期]									*/
/*		PnFA8 : 速度リップル補償第3成分振幅値(電気角) [0.01%]										*/
/*		PnFA9 : 速度リップル補償第3成分位相(電気角) [deg]											*/
/*		PnFAA : 速度リップル補償第4成分周期(電気角) [電気角周期]									*/
/*		PnFAB : 速度リップル補償第4成分振幅値(電気角) [0.01%]										*/
/*		PnFAC : 速度リップル補償第4成分位相(電気角) [deg]											*/
/*		PnFAD : 速度リップル補償第5成分周期(電気角) [電気角周期]									*/
/*		PnFAE : 速度リップル補償第5成分振幅値(電気角) [0.01%]										*/
/*		PnFAF : 速度リップル補償第5成分位相(電気角) [deg]											*/
/*		PnFB0 : 速度リップル補償第6成分周期(電気角) [電気角周期]									*/
/*		PnFB1 : 速度リップル補償第6成分振幅値(電気角) [0.01%]										*/
/*		PnFB2 : 速度リップル補償第6成分位相(電気角) [deg]											*/
/****************************************************************************************************/
PRM_RSLT pncal_VelRippleCompE(PRM_ACCCMD Cmd, UINT ArrayIdx, AXIS_HANDLE *Axis, LONG *pValue)
{
//	BOOL		ans;	/*<S1FD>*/
	PRMDATA		*Prm;
	BPRMDAT		*Bprm;
	Prm  = Axis->UniPrms->Prm;
	Bprm = Axis->UniPrms->Bprm;

	CalVelRippleCompE( &(Axis->BaseLoops->RippleCmp),
					   Bprm,
					   Prm->RippleCompGainE,			/* PnFA0 : 速度リップル補償ゲイン(電気角) */
					   Prm->RippleCompCycleE,			/* PnFA1, PnFA4, …, PnFB0 : 速度リップル補償成分周期(電気角  */
					   Prm->RippleCompAmplitudeE,		/* PnFA2, PnFA5, …, PnFB1 : 速度リップル補償成分振幅(電気角) */
					   Prm->RippleCompPhaseE );			/* PnFA3, PnFA6, …, PnFB2 : 速度リップル補償成分位相(電気角) */

//	ans = CalVelRippleCompE( &(Axis->BaseLoops->RippleCmp),	/*<S1FD>*/
//							 Bprm,
//							 Prm->RippleCompGainE,			/* PnFA0 : 速度リップル補償ゲイン(電気角) */
//							 &(Prm->RippleCompCycleE),		/* PnFA1, PnFA4, …, PnFB0 : 速度リップル補償成分周期(電気角  */
//							 &(Prm->RippleCompAmplitudeE),	/* PnFA2, PnFA5, …, PnFB1 : 速度リップル補償成分振幅(電気角) */
//							 &(Prm->RippleCompPhaseE));		/* PnFA3, PnFA6, …, PnFB2 : 速度リップル補償成分位相(電気角) */

//	if( ans != FALSE )
//	{
//		ALMSetPramError( Axis->AlmManager, pndef_tfuncsw.RegNumber );
//	}

	return PRM_RSLT_SUCCESS;
}
/* <S14B> End */
/***************************************** end of file **********************************************/
