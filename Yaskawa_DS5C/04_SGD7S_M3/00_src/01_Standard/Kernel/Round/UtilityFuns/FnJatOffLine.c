/****************************************************************************************************/
/*																									*/
/*	FnJatOffLine.c : オフラインオートチューニングモジュール											*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*																									*/
/*	T.Kira 2011.03.01   初版																		*/
/****************************************************************************************************/
#include "Basedef.h"
#include "FnJatOffLine.h"
#include "PnPrmListTbl.h"
#include "TuneLib.h"
#include "BaseCmnStruct.h"

/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
static void JatParamCal( JATHNDL *JatHdl, UNI_PRM_STR *UniPrms, 
														REG_MANAGER_HANDLE *hPnReg, LONG SvCycleNs);

/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定ＲＯＵＮＤ処理														*/
/*																									*/
/*	概要:	イナーシャ比をパラメータに反映する。													*/
/*																									*/
/*	arg		:																						*/
/*	g-ref	:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
void CpiSetEstimateJrat( JATHNDL *JatHdl, REG_ACC_T *AccessPrm )
{
	JATV	*jatv;
	PRMBK	*prmbk;

	jatv = &JatHdl->jatv;
	prmbk = &JatHdl->prmbk;

	if (jatv->enable == TRUE)				/* イナーシャ同定有効			*/
	{
		/*------------------------------------------------------------------*/
		/*	同定値のKv反映処理												*/
		/*------------------------------------------------------------------*/
		/* Pn103に反映 */
		TunePrmWriteRam( AccessPrm, &pndef_jrate, (LONG)jatv->jrate, pndef_jrate.DataLength );
		/* バックアップ値を更新	*/
		prmbk->jrate = (ULONG)jatv->jrate;
	}
}


/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定同定エラ−ステータス取得ＡＰＩ										*/
/*																									*/
/*	概要:	同定エラーステータスを取得する。エラーステータスをクリアする							*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
CHAR CpiGetJatErrStatus( JATHNDL *JatHdl )
{
	CHAR	rc;
	JATV	*jatv;

	jatv = &JatHdl->jatv;

	if(jatv->status & JATERR)
	{
		rc = NG;
	}
	else
	{
		rc = OK;
	}
	jatv->status &= ~JATERR;

	return	rc;
}

/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定同定実行中エラ−ステータス取得ＡＰＩ								*/
/*																									*/
/*	概要:	同定実行エラーステータスを取得する。ステータスはクリアしない							*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
CHAR CpiGetJatExeErrStatus( JATHNDL	*JatHdl )
{
	CHAR	rc;
	JATV	*jatv;

	jatv = &JatHdl->jatv;

	if(jatv->status & JATEXEERR)
	{
		rc = NG;
	}
	else
	{
		rc = OK;
	}

	return	rc;
}

/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定同定完了ステータス取得ＡＰＩ										*/
/*	概要:	同定完了ステータスを取得する。															*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
CHAR CpiGetJatCompStatus( JATHNDL	*JatHdl )
{
	CHAR	rc;
	JATV	*jatv;

	jatv = &JatHdl->jatv;

	if(jatv->status & JATJRATECOMP)
	{
		rc = OK;
	}
	else
	{
		rc = NG;
	}

	return	rc;
}

/****************************************************************************************************/
/*																									*/
/*		オフラインAT初期化																			*/
/*																									*/
/*	概要:オフラインAT関係の変数を初期化，パラメータ計算を行う。										*/
/*		 イナーシャ同定有効フラグをONする															*/
/*		 同定移動距離を設定する																		*/
/*		 フラグ関係の初期化も行うので同定開始前には必ず本関数を実行すること							*/
/*       Pn103もこのタイミング同定初期値に書き換える。												*/
/*																									*/
/*																									*/
/*	arg		:	SHORT	InitSw		:	1:変数，パラメータ計算実行									*/
/*										0:変数初期化のみ											*/
/*				LONG	distance	:	総移動量	[指令単位]										*/
/*	out		:	SHORT	rc			:	OK：同定実行可能	NG：同定実行不可						*/
/*																									*/
/****************************************************************************************************/
SHORT CpiInitJatOffLine( JATHNDL *JatHdl, UNI_PRM_STR *UniPrms, REG_MANAGER_HANDLE *RegManager, 
					GAIN_CHNG_HNDL GainChange, SHORT InitSw, LONG distance, UCHAR CtrlMode, LONG SvCycleNs)
{
	SHORT	rc;
	JATV	*jatv;

	jatv = &JatHdl->jatv;

/*--------------------------------------------------------------------------------------------------*/
/*	変数初期化																						*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( jatv, sizeof( JatHdl->jatv )>>2 );

/*--------------------------------------------------------------------------------------------------*/
/*	初期設定																						*/
/*--------------------------------------------------------------------------------------------------*/
	jatv->TargetDist = distance >> 1;				/* 移動距離設定：1/2は同定開始距離設定のため	*/

/*--------------------------------------------------------------------------------------------------*/
/*	実行条件判別																					*/
/*--------------------------------------------------------------------------------------------------*/
	if( (CtrlMode == CTRLMODE_POS)									/* 位置制御						*/
	 && (jatv->TargetDist != 0)										/* 移動距離がゼロでない			*/
	 && (GainChange.AtGsel == FALSE))								/* 自動ゲイン切替えではない		*/
	{
		/*------------------------------------------------------------------------------------------*/
		/*	パラメータ演算																			*/
		/*------------------------------------------------------------------------------------------*/
		if(InitSw == TRUE)
		{
			JatParamCal( JatHdl, UniPrms, RegManager, SvCycleNs );
		}
		jatv->enable = TRUE;						/* イナーシャ同定有効フラグ  ON					*/
		rc = OK;
	}
	else
	{
		jatv->enable = FALSE;						/* イナーシャ同定有効フラグ　OFF				*/
		rc = NG;
	}

	return rc;
}

/****************************************************************************************************/
/*																									*/
/*		パラメータ計算																				*/
/*																									*/
/*	概要:オフラインイナーシャ同定関係の定数を設定する。												*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void JatParamCal( JATHNDL *JatHdl, UNI_PRM_STR *UniPrms, REG_MANAGER_HANDLE *RegManager, LONG SvCycleNs )
{
	ULONG	wrk_kv, wrk_jrat;
	LONG	wrk_kp;
	ULONG	wrk_ki,wrk_klpf;
	LONG	kx,sx;
	LONG	ky,sy;

	PRMDATA *Prm;
	BPRMDAT	*Bprm;
	REGIF_CONFIG_T *RegCfg;
	REG_ACC_T	AccessPrm;
	PRMBK	*prmbk;
	JATP	*jatp_wk;
	JATV	*jatv;

	Prm = UniPrms->Prm;
	Bprm = UniPrms->Bprm;
	RegCfg = RegManager->hPnReg;
	prmbk = &JatHdl->prmbk;
	jatp_wk = &JatHdl->jatp_wk;
	jatv = &JatHdl->jatv;

	/* RAM書き込み用レジスタ情報設定 */
	AccessPrm.MemAddr  = 0;
	AccessPrm.hRegCfg  = RegCfg;
	AccessPrm.ArrayIdx = 0;
	AccessPrm.EepWrite = FALSE;
	AccessPrm.IfHandle = NULL;

/*--------------------------------------------------------------------------------------------------*/
/*		ゲイン関連パラメータ設定																	*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		同定初期イナーシャ比																		*/
/*--------------------------------------------------------------------------------------------------*/
	prmbk->jrate = Prm->jrate;/* Pn103現設定値取得				*/
	wrk_jrat = Prm->Pn324;
	/* Pn103に反映	*/
	TunePrmWriteRam( &AccessPrm, &pndef_jrate, (LONG)wrk_jrat, pndef_jrate.DataLength );

/*--------------------------------------------------------------------------------------------------*/
/*		Ｋｐゲインバランス																			*/
/*--------------------------------------------------------------------------------------------------*/
	prmbk->kp = Prm->posgn;										/* Pn102現設定値取得				*/
	wrk_kv = Prm->loophz;										/* Ｋｖ取得							*/
	wrk_kp = wrk_kv/10;											/* Ｋｐ＝Ｋｖ[Hz]／１０				*/
	/* 下限値チェック	*/
	if( wrk_kp < pndef_posgn.LowerLimit )
	{
		wrk_kp = pndef_posgn.LowerLimit;
	}
	TunePrmWriteRam( &AccessPrm, &pndef_posgn, (LONG)wrk_kp, pndef_posgn.DataLength );

/*--------------------------------------------------------------------------------------------------*/
/*		速度フィードフォワード																		*/
/*--------------------------------------------------------------------------------------------------*/
	prmbk->vffgn = Prm->ffgn;									/* 速度ＦＦゲイン取得				*/
	TunePrmWriteRam( &AccessPrm, &pndef_ffgn, (LONG)JATVFFGN, pndef_ffgn.DataLength );/* VFF=100%	*/

/*--------------------------------------------------------------------------------------------------*/
/*		イナーシャ同定関連パラメータ計算															*/
/*--------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------*/
/*		jatp_wk->mjrate	：モデルイナーシャ(モータ単体)												*/
/*--------------------------------------------------------------------------------------------------*/
	jatp_wk->mjrate = 100;
/*--------------------------------------------------------------------------------------------------*/
/*		jatp_wk->mkv	：モデル速度ループゲイン													*/
/*--------------------------------------------------------------------------------------------------*/
	wrk_kv = Prm->loophz;
	jatp_wk->mkv = MlibScalKskxkx( Bprm->Kvx, (jatp_wk->mjrate * wrk_kv),  1000, NULL, 24 );

/*--------------------------------------------------------------------------------------------------*/
/*		jatp_wk->mkvi	：モデル速度ループ積分ゲイン												*/
/*--------------------------------------------------------------------------------------------------*/
	wrk_ki = Prm->pitime;
	jatp_wk->mkvi = MlibScalKskxkx( jatp_wk->mkv, (SvCycleNs / 1000), 10*wrk_ki, NULL, 24 );
/*--------------------------------------------------------------------------------------------------*/
/*		jatp_wk->mlpf	：モデルトルクフィルタゲイン												*/
/*--------------------------------------------------------------------------------------------------*/
	wrk_klpf = Prm->trqfil11;
	jatp_wk->mklpf = MlibPcalKf1gain( 10*wrk_klpf, (SvCycleNs / 1000), 0 );

/*--------------------------------------------------------------------------------------------------*/
/*		jatp_wk->mkj	：モデルイナーシャゲイン													*/
/*																									*/
/*					      MaxTrq * 100 * CycleNs[ns] * ObsJGain										*/
/*				x = ------------------------------------------------								*/
/*					  OvrSpd * Jmot * wrkp.mjrate * 10^9 * 100										*/
/*																									*/
/*					  2*PAI * 100 * CycleNs * ObsJGain(=100)										*/
/*				  = --------------------------------------											*/
/*					  Kvx * wrkp.mjrate * 10^9 * 100												*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
	sx = 0;
	kx = MlibPcalKxmulx( 100, SvCycleNs, C2PAIE7, &sx );
	ky = MlibScalKskxkx( Bprm->Kvx,C10POW7,1, &sy, 0 );
	ky = MlibPcalKxmulx( ky, C10POW9, jatp_wk->mjrate, &sy );
	jatp_wk->mkj = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
/*--------------------------------------------------------------------------------------------------*/
/*		jatp_wk->jfil	：イナーシャ更新フィルタゲイン												*/
/*--------------------------------------------------------------------------------------------------*/
	jatp_wk->jfil = MlibPcalKf1gain( JATFIL, JRATCALCYCUS, 0);

/*--------------------------------------------------------------------------------------------------*/
/*		同定エラー検出用速度偏差フィルタ															*/
/*--------------------------------------------------------------------------------------------------*/
	jatp_wk->jverrfil = MlibPcalKf1gain( JRATVERRFIL, JRATCALCYCUS, 1 );

/*--------------------------------------------------------------------------------------------------*/
/*		構造体へ代入																				*/
/*--------------------------------------------------------------------------------------------------*/
	jatv->JatPrmSetReq = TRUE;

}

/****************************************************************************************************/
/*																									*/
/*		オフラインAT終了処理																		*/
/*																									*/
/*	概要:オフラインAT関連変数を初期化し，変更したパラメータを元に戻す。								*/
/*		 イナーシャ同定有効フラグをOFFする															*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
void CpiEndJatOffLine( JATHNDL *JatHdl, REG_ACC_T *AccessPrm )
{
	JATV *jatv;
	PRMBK	*prmbk;

	jatv = &JatHdl->jatv;
	prmbk = &JatHdl->prmbk;
/*--------------------------------------------------------------------------------------------------*/
/*	パラメータ再計算																				*/
/*--------------------------------------------------------------------------------------------------*/
	if (jatv->enable == TRUE)									/* イナーシャ同定有効				*/
	{
		/*------------------------------------------------------------------------------------------*/
		/*		同定初期イナーシャ比																*/
		/*------------------------------------------------------------------------------------------*/
		/* Pn103  VFF=100%	*/
		TunePrmWriteRam( AccessPrm, &pndef_jrate, (LONG)JATVFFGN, pndef_jrate.DataLength );

		/*------------------------------------------------------------------------------------------*/
		/*		Ｋｐゲインバランス																	*/
		/*------------------------------------------------------------------------------------------*/
		/* Pn102  VFF=100%	*/
		TunePrmWriteRam( AccessPrm, &pndef_posgn, (LONG)prmbk->kp, pndef_posgn.DataLength );

		/*------------------------------------------------------------------------------------------*/
		/*		速度フィードフォワード																*/
		/*------------------------------------------------------------------------------------------*/
		/* Pn109  VFF=100% */
		TunePrmWriteRam( AccessPrm, &pndef_ffgn, (LONG)prmbk->vffgn, pndef_ffgn.DataLength );
	}
/*--------------------------------------------------------------------------------------------------*/
/*	変数初期化																						*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( jatv, sizeof( JatHdl->jatv )>>2 );

}

