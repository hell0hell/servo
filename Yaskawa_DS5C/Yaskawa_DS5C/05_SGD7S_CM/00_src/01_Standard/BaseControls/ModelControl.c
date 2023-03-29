/****************************************************************************************************/
/*																									*/
/*																									*/
/*		ModelControl.c : ダイレクトトルクリファレンス方式モデル追従制御関連処理定義					*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : モデル追従制御関連処理																	*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "ModelControl.h"
#include "MLib.h"

/****************************************************************************************************/
/*		PROTOTYPE																					*/
/****************************************************************************************************/
/*	void	MfcInitModelControl( MFCTRL *MFControl );												*/
/*	LONG	MfcControlMain( MFCTRL *MFControl, EXP_CTRL_DATA *hExpCtrl,								*/
/*														BASE_CTRL_HNDL *pBaseCtrl, LONG dPcmd );	*/
/*	SHORT	MfcGetParamEnableSts( MFCTRL *MFControl );												*/
/*	void	MfcSetMfcCtrlSwitch( MFCTRL *MFControl, BOOL Switch );									*/
/****************************************************************************************************/
static	void	mfcControlOneMass( MFCTRL *MFControl, LONG dPcmd );
static	void	mfcControlTwoMass( MFCTRL *MFControl, LONG dPcmd );
static	LONG	mfcDtrCmdFilter( MFCTRL *MFControl, LONG pcmd );
/****************************************************************************************************/



/****************************************************************************************************/
/*																									*/
/*		ＭＦＣ変数初期化																			*/
/*																									*/
/****************************************************************************************************/
void	MfcInitModelControl( MFCTRL *MFControl )
{
	MlibResetLongMemory( &(MFControl->var), sizeof(MFControl->var)/4 );
	/* パラメータ初期化は第1ゲイン */
	MFControl->conf.MfcPrm[0] = MFControl->conf.MfcPrm[1];
	/* 実行用パラメータ初期化 */
	MFControl->conf.MfcPexe = MFControl->conf.MfcPrm[0];
	return;
}



/****************************************************************************************************/
/*																									*/
/*		ＭＦＣ演算																					*/
/*																									*/
/*	概要:	パラメータ設定に応じて各規範モデルのＭＦＣ演算を行う									*/
/*			パラメータが変更された時または，機能OFF->ON，機能ON->OFFされたときは，					*/
/*			「位置指令完了」かつ「溜まりﾊﾟﾙｽなし」で切り替える。位置ずれ防止，ショック低減のため	*/
/*			ＭＦＣ処理前に振動抑制フィルタ処理を行う。ＭＦＣへの入力は振動抑制フィルタ後位置指令。	*/
/*																									*/
/*																									*/
/* arg		: LONG	dPcmd				:	位置差分指令		[pulse]								*/
/* out		: LONG	pcmdout				:	ＭＦＣ後位置指令	[pulse]								*/
/* g-out	: LONG	BoutV.SpdFFCx 		:	速度ＦＦ補償		[2^24/OvrSpd]						*/
/*			  LONG	BoutV.TrqFFCx		:	トルクＦＦ補償		[2^24/MaxTrq]						*/
/*																									*/
/****************************************************************************************************/
LONG	MfcControlMain( MFCTRL *MFControl, LONG dPcmd, LONG *SpdFFCx, LONG *TrqFFCx )
{
	LONG	pcmdout;

/*--------------------------------------------------------------------------------------------------*/
/*	ＭＦＣ強制無効化処理(MFC停止フラグ)																*/
/*--------------------------------------------------------------------------------------------------*/
	if( MFControl->var.MfcStopReq == TRUE )
	{
		MFControl->conf.MfcPrm[0].MFCModel = 0;
	}

/*--------------------------------------------------------------------------------------------------*/
/*	指令完了＆払い出し完了チェック																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (dPcmd == 0)
		&& (MFControl->var.MfcPosErr == 0)
		&& (MFControl->var.FCmd[0] == 0)
		&& (MFControl->var.FCmd[1] == 0)
		&& (MFControl->var.FCmd[2] == 0) )
	{ /* 指令完了 && モデル位置偏差なし->払い出し完了 */
		/* パラメータ切替え実行 */
		MFControl->conf.MfcPexe = MFControl->conf.MfcPrm[0];
	}

/*--------------------------------------------------------------------------------------------------*/
/*	ＭＦＣ演算																						*/
/*--------------------------------------------------------------------------------------------------*/
	/* MFC処理カウンタ更新 */
	MFControl->var.execnt++;

	switch( MFControl->conf.MfcPexe.MFCModel )
	{
	case ONE_MASS:		/*	剛体系モデル			*/
		mfcControlOneMass( MFControl, dPcmd );
		MFControl->var.CurMFCModel = MFControl->conf.MfcPexe.MFCModel;
		break;
	case TWO_MASS1:		/*	２慣性系モデル			*/
	case TWO_MASS2:		/*	２慣性系モデル			*/
	case ONE_BASE:		/*	剛体系＋機台モデル		*/
		mfcControlTwoMass( MFControl, dPcmd );
		MFControl->var.CurMFCModel = MFControl->conf.MfcPexe.MFCModel;
		break;
	default:
		MFControl->conf.MfcPexe.MFCModel = 0;
		MFControl->var.CurMFCModel = 0;
		break;
	}

	if( MFControl->conf.MfcPexe.MFCModel != 0 )
	{ /* モデル追従有効 */
/*--------------------------------------------------------------------------------------------------*/
/*		速度ＦＦ補償, トルクＦＦ補償値へ加算														*/
/*--------------------------------------------------------------------------------------------------*/
		if( (MFControl->var.PffCmd != 0) || (MFControl->conf.MfcPexe.MFCType == SGDX_TYPE) )
		{	/* 位置指令があるときのみＦＦ有効 または MFC処理タイプ(SGDX互換)の場合ＦＦ有効 */
			*SpdFFCx += MFControl->var.VffCmd;				/* ＭＦＣ速度ＦＦ加算		*/
			*TrqFFCx += MFControl->var.TffCmd;				/* ＭＦＣトルクＦＦ加算		*/
		}
/*--------------------------------------------------------------------------------------------------*/
/*		溜まりパルス演算	Σ(位置指令－ＭＦＣ後の位置指令)										*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		MFControl->var.MfcPosErr = FlibPerrcalx( dPcmd, MFControl->var.PffCmd, MFControl->var.Per64 );
#else
		MFControl->var.MfcPosErr = MlibPerrcalx( dPcmd, MFControl->var.PffCmd, MFControl->var.Per64 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		位置指令出力																				*/
/*--------------------------------------------------------------------------------------------------*/
		pcmdout = MFControl->var.PffCmd;
	}
	else
	{ /* モデル追従無効 */
		MlibResetLongMemory( &(MFControl->var), sizeof(MFControl->var)/4 );
		MFControl->var.PffCmd = 0;
		MFControl->var.VffCmd = 0;
		MFControl->var.TffCmd = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		位置指令出力																				*/
/*--------------------------------------------------------------------------------------------------*/
		pcmdout = dPcmd;								/* 機能無効時は指令入力をそのまま出力		*/
	}

	return pcmdout;
}



/****************************************************************************************************/
/*																									*/
/*		剛体ＭＦＣ演算																				*/
/*																									*/
/*	概要:	剛体ＭＦＣ演算を行う																	*/
/*			２次フィルタ×１段を通し，ＭＦＣ後位置指令を作成。										*/
/*			ＦＦ用１階微分は，分解能ＵＰのため，別の２次フィルタを使用する							*/
/*																									*/
/* arg		: LONG	dPcmd		:位置差分指令	[pluse]												*/
/* out		: 																						*/
/* g-out	: 																						*/
/*																									*/
/****************************************************************************************************/
static	void	mfcControlOneMass( MFCTRL *MFControl, LONG dPcmd )
{
	LONG	wk;
	KSGAIN	wk_cmd;
	LONG	tffcmd;

	MFControl->var.PCmdsub = dPcmd;							/* 位置指令セット						*/

	/*----------------------------------------------------------------------------------------------*/
	/*		高次フィルタ演算	剛体系：２次フィルタ												*/
	/*----------------------------------------------------------------------------------------------*/
	/* 剛体モデル → ２次フィルタ×１ */
	/* FCmd(k-1) */
	MFControl->var.FCmd[1] = MFControl->var.FCmd[0];
#if (FLOAT_USE==TRUE)
	/* 位置を４次フィルタとする */
	wk_cmd = FlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 ); //todo
	/* FCmd(k) */
	MFControl->var.FCmd[0] = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#else
	/* 位置を４次フィルタとする */
	wk_cmd = MlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 );
	/* FCmd(k) */
	MFControl->var.FCmd[0] = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#endif /* FLOAT_USE */


	/* １階微分値算出 → １階微分用２次フィルタ */
	/* １階微分の分解能UP */
#if (FLOAT_USE==TRUE)
	wk_cmd = wk_cmd * MFControl->conf.MfcPexe.Fsft;
#else
	wk_cmd = MlibMulgain32( wk_cmd, MFControl->conf.MfcPexe.Fsft );
#endif /* FLOAT_USE */
	/* １階微分値 dFCmd(k) */
	MFControl->var.dFCmd[0] = mfcDtrCmdFilter( MFControl, wk_cmd );
#if (FLOAT_USE==TRUE)
	/* 分解能UP FCmd(k) */
	MFControl->var.FCmdF = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 ); //todo
#else
	/* 分解能UP FCmd(k) */
	MFControl->var.FCmdF = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 );
#endif /* FLOAT_USE */

	/* 位置データ取得 */
	MFControl->var.FCmdFilo[1] = MFControl->var.FCmdFilo[0];
	MFControl->var.FCmdFilo[0] = MFControl->var.FCmdF;

	/*----------------------------------------------------------------------------------------------*/
	/*		メカ係数演算																			*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/* 分解能を戻す */
	wk = MFControl->var.dFCmd[0] * MFControl->conf.MfcPexe.InvFsft;
	/* TrqRef(k) = dFCmd(k) * Kj1 */
	MFControl->var.TrqRef = wk * MFControl->conf.MfcPexe.Kj1;
#else
	/* 分解能を戻す */
	wk = MlibMulgain30( MFControl->var.dFCmd[0], MFControl->conf.MfcPexe.InvFsft );
	/* TrqRef(k) = dFCmd(k) * Kj1 */
	MFControl->var.TrqRef = MlibMulgain( wk, MFControl->conf.MfcPexe.Kj1 );
#endif /* FLOAT_USE */
	/*----------------------------------------------------------------------------------------------*/
	/*		ＦＦ指令演算																			*/
	/*----------------------------------------------------------------------------------------------*/
	/* Pfbk(k) = FCmd(k-1) */
	MFControl->var.Pfbk = MFControl->var.FCmd[1];
	/*	位置ＦＦ指令 */
	MFControl->var.PffCmd = MFControl->var.Pfbk;
	/*----------------------------------------------------------------------------------------------*/
	if( MFControl->conf.MfcPexe.MFCType == SGDX_TYPE )
	{ /* MFC処理タイプ(SGDX互換) 分解能はそのまま */
		/* Vref(k) = FCmd(k-1) */
		MFControl->var.Vref = MFControl->var.FCmd[1];
	}
	else
	{ /* 分解能UPした速度FF */
		/* Vref(k) = FCmd(k-1) */
		MFControl->var.Vref = MFControl->var.FCmdFilo[1];
	}

	/*	速度ＦＦ指令 */
#if (FLOAT_USE==TRUE)
	MFControl->var.VffCmd = MFControl->var.Vref * MFControl->conf.MfcPexe.Kvff;
#else
	MFControl->var.VffCmd = MlibMulgain( MFControl->var.Vref, MFControl->conf.MfcPexe.Kvff );
#endif /* FLOAT_USE */

	/* 指令方向判別 → トルクFFゲイン切替え */
	if( MFControl->var.PCmdsub > 0 )
	{
		MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff1;
	}
	else if( MFControl->var.PCmdsub < 0 )
	{
		MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff2;
	}

	/* トルクＦＦ指令 */
	MFControl->var.FricCmpTrq = 0;

#if (FLOAT_USE==TRUE)
	tffcmd = (MFControl->var.TrqRef * MFControl->var.Ktffcmdx ) + MFControl->var.FricCmpTrq;
	MFControl->var.TffCmd = FlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#else
	tffcmd = MlibMulgain( MFControl->var.TrqRef, MFControl->var.Ktffcmdx ) + MFControl->var.FricCmpTrq;
	MFControl->var.TffCmd = MlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#endif /* FLOAT_USE */

}



/****************************************************************************************************/
/*																									*/
/*		二慣性，剛体＋機台ＭＦＣ演算																*/
/*																									*/
/*	概要:	二慣性ＭＦＣ演算を行う																	*/
/*			２次フィルタ×２段を通し，ＭＦＣ後位置指令を作成。										*/
/*			ＦＦ用１階微分は，分解能ＵＰのため，別の２次フィルタを使用する							*/
/*																									*/
/* arg		: LONG	dPcmd		:位置差分指令	[pluse]												*/
/* out		: 																						*/
/* g-out	: 																						*/
/*																									*/
/****************************************************************************************************/
static	void	mfcControlTwoMass( MFCTRL *MFControl, LONG dPcmd )
{
	LONG	wk;
	KSGAIN 	wk_cmd;
	LONG	posref, posref1, posref2;
	LONG	posref_sft;
	LONG	trqrf1;
	LONG	tffcmd;

/*--------------------------------------------------------------------------------------------------*/
/*		DTR 250us Cycle	Control																		*/
/*--------------------------------------------------------------------------------------------------*/
	if( (MFControl->var.execnt & 0x0001) == 1 )
	{
/*--------------------------------------------------------------------------------------------------*/
/*	前半サイクルの処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		/* 今回サイクル位置指令の保存 */
		MFControl->var.PCmdsub = dPcmd;

	/*----------------------------------------------------------------------------------------------*/
	/*		メカ係数演算																			*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
		/* 分解能を戻す */
		wk = MFControl->var.dFCmd[1] * MFControl->conf.MfcPexe.InvFsft;
		/* 分解能を戻す */
		trqrf1 = MFControl->var.d3FCmd * MFControl->conf.MfcPexe.Kj3;
		/* TrqRef(k) = ( dfCmd(k-1) + d3FCmd(k)*Kj3 ) * Kj1 */
		MFControl->var.TrqRef = ( wk + trqrf1) * MFControl->conf.MfcPexe.Kj1;
#else
		/* 分解能を戻す */
		wk = MlibMulgain30( MFControl->var.dFCmd[1], MFControl->conf.MfcPexe.InvFsft);

		/* 分解能を戻す */
		trqrf1 = MlibMulgain30( MFControl->var.d3FCmd, MFControl->conf.MfcPexe.Kj3 );
		/* TrqRef(k) = ( dfCmd(k-1) + d3FCmd(k)*Kj3 ) * Kj1 */
		MFControl->var.TrqRef = MlibMulgain( wk + trqrf1, MFControl->conf.MfcPexe.Kj1 );
#endif /* FLOAT_USE */

		/* 分解能を戻す */
		posref = MlibPfbkxremNolim( MFControl->var.d2FCmd[1], MFControl->conf.MfcPexe.Kj2, &MFControl->var.wkrem1 );
		posref_sft = MlibPfbkxremNolim( MFControl->var.d2FCmd[1], MFControl->conf.MfcPexe.Kj2_sft, &MFControl->var.wkrem2 );

		if( MFControl->conf.MfcPexe.Fccflag )
		{ /* Full-Closed && 2-mass */
			posref1 = 0;
		}
		else
		{
			posref1 = posref;
		}

		if(MFControl->conf.MfcPexe.MFCType == SGDX_TYPE)
		{ /* MFC処理タイプ(SGDX互換) 分解能はそのまま */
			if( MFControl->conf.MfcPexe.PSFccflag )
			{/* PS-Full-Closed && 2-mass */
				posref2 = 0;
			}
			else
			{
				posref2 = posref;
			}
		}
		else
		{ /* 分解能UPして計算 */
			if( MFControl->conf.MfcPexe.PSFccflag )
			{ /* PS-Full-Closed && 2-mass */
				posref2 = 0;
			}
			else
			{
				posref2 = posref_sft;
			}
		}

	/*----------------------------------------------------------------------------------------------*/
	/*		強制払い出し処理																		*/
	/*----------------------------------------------------------------------------------------------*/
		if( (posref1 == 0)
			&& (MFControl->var.FCmd[0] == MFControl->var.FCmd[1])
			&& (MFControl->var.PosSum[0] != 0) )
		{ /* 今回位置指令加算値＝0 && 1階微分値＝0 && 積算値≠0 */
			if( MFControl->var.PosSum[0] > 0 )
			{
				posref1 = -1;
			}
			else
			{
				posref1 = 1;
			}
		}
		/* 位置指令加算値積算 */
		wk = MlibPerrcalx( posref1, 0, MFControl->var.PosSum );

	/*----------------------------------------------------------------------------------------------*/
	/*		ＦＦ指令演算																			*/
	/*----------------------------------------------------------------------------------------------*/
		/* 位置ＦＦ指令 Pfbk(k-1) */
		MFControl->var.PffCmd = MFControl->var.Pfbk - MFControl->var.PffCmd;
		/* Pfbk(k) */
		MFControl->var.Pfbk = posref1 + MFControl->var.FCmd[2];
	/*----------------------------------------------------------------------------------------------*/
		if(MFControl->conf.MfcPexe.MFCType == SGDX_TYPE)
		{ /* MFC処理タイプ(SGDX互換) 分解能はそのまま */
			/* Vref(k) = d2FCmd[1]*Kj2 + FCmd(k-2) */
			MFControl->var.Vref = posref2 + MFControl->var.FCmd[2];
		}
		else
		{ /* 分解能UPした速度FF */
			/* Vref(k) = d2FCmd[1]*Kj2 + FCmd(k-2) */
			MFControl->var.Vref = posref2 + MFControl->var.FCmdFilo[2];
		}
		/* 速度ＦＦ指令 Vref(k-1)*Kvff *//* VFFを半サンプル進める */
#if (FLOAT_USE==TRUE)
		MFControl->var.VffCmd = MFControl->var.Vref * MFControl->conf.MfcPexe.Kvff;
#else
		MFControl->var.VffCmd = MlibMulgain( MFControl->var.Vref, MFControl->conf.MfcPexe.Kvff );
#endif /* FLOAT_USE */

		/* 指令方向判別 → トルクFFゲイン切替え */
		if( MFControl->var.PCmdsub > 0 )
		{
			MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff1;
		}
		else if( MFControl->var.PCmdsub < 0 )
		{
			MFControl->var.Ktffcmdx = MFControl->conf.MfcPexe.Ktff2;
		}

		/* トルクＦＦ指令 */
#if (FLOAT_USE==TRUE)
		MFControl->var.FricCmpTrq = 0;
		tffcmd = (MFControl->var.TrqRef * MFControl->var.Ktffcmdx) + MFControl->var.FricCmpTrq;
		MFControl->var.TffCmd = FlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#else
		MFControl->var.FricCmpTrq = 0;
		tffcmd = MlibMulgain(MFControl->var.TrqRef, MFControl->var.Ktffcmdx) + MFControl->var.FricCmpTrq;
		MFControl->var.TffCmd = MlibLimitul( tffcmd, 0x01000000, 0xFF000000 );
#endif /* FLOAT_USE */
		}
	else
	{
/*--------------------------------------------------------------------------------------------------*/
/*	後半サイクルの処理																				*/
/*--------------------------------------------------------------------------------------------------*/
		/* 前回と今回周期の位置指令を加算 */
		MFControl->var.PCmdsub = MFControl->var.PCmdsub + dPcmd;

	/*----------------------------------------------------------------------------------------------*/
	/*	位置指令計算																				*/
	/*----------------------------------------------------------------------------------------------*/
		/* 演算周期変換 */
		MFControl->var.PffCmd = (MFControl->var.Pfbk >> 1);
	/*----------------------------------------------------------------------------------------------*/
	/*	速度ＦＦ指令計算																			*/
	/*----------------------------------------------------------------------------------------------*/
		/*	速度ＦＦ指令 */
#if (FLOAT_USE==TRUE)
		MFControl->var.VffCmd = MFControl->var.Vref * MFControl->conf.MfcPexe.Kvff;
#else
		MFControl->var.VffCmd = MlibMulgain( MFControl->var.Vref, MFControl->conf.MfcPexe.Kvff );
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		高次フィルタ演算	２慣性系：２次フィルタ×２											*/
	/*----------------------------------------------------------------------------------------------*/
	/* ２慣性モデル → ２次フィルタ×２ */
		/* FCmd(k-2) */
		MFControl->var.FCmd[2] = MFControl->var.FCmd[1];
		/* FCmd(k-1) */
		MFControl->var.FCmd[1] = MFControl->var.FCmd[0];
		/* FCmd(k) */
#if (FLOAT_USE==TRUE)
		wk_cmd = FlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 );
		MFControl->var.FCmd[0] = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#else
		wk_cmd = MlibPcmdLpfil2( MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk1 );
		MFControl->var.FCmd[0] = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk2 );
#endif /* FLOAT_USE */

	/* １階微分値算出 → ２次フィルタ×１＋１階微分用２次フィルタ */
		/* １階微分値 dFCmd(k-1) */
		MFControl->var.dFCmd[1] = MFControl->var.dFCmd[0];
#if (FLOAT_USE==TRUE)
		/* １階微分の分解能UP */
		wk_cmd = MFControl->var.PCmdsub * MFControl->conf.MfcPexe.Fsft;
#else
		/* １階微分の分解能UP */
		wk_cmd = MlibMulgain32(MFControl->var.PCmdsub, MFControl->conf.MfcPexe.Fsft);
#endif /* FLOAT_USE */
		
		/* １階微分値 dFCmd(k) */
#if (FLOAT_USE==TRUE)
		wk_cmd = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk3 );
		MFControl->var.dFCmd[0] = mfcDtrCmdFilter( MFControl, wk_cmd );
		/* 分解能UP FCmd(k) */
		MFControl->var.FCmdF = FlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 );
#else
		wk_cmd = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk3 );
		MFControl->var.dFCmd[0] = mfcDtrCmdFilter( MFControl, wk_cmd );
		/* 分解能UP FCmd(k) */
		MFControl->var.FCmdF = MlibPcmdLpfil2( wk_cmd, MFControl->conf.MfcPexe.Kf1, MFControl->var.Fwk4 );
#endif /* FLOAT_USE */

		/* 分解能UP位置データ取得 */
		MFControl->var.FCmdFilo[2] = MFControl->var.FCmdFilo[1];
		MFControl->var.FCmdFilo[1] = MFControl->var.FCmdFilo[0];
		MFControl->var.FCmdFilo[0] = MFControl->var.FCmdF;

	/*----------------------------------------------------------------------------------------------*/
	/*		微分演算																				*/
	/*----------------------------------------------------------------------------------------------*/
		/* ２階微分値 d2FCmd(k-1) */
		MFControl->var.d2FCmd[1] = MFControl->var.d2FCmd[0];
		/* ２階微分値 d2FCmd(k) */
		MFControl->var.d2FCmd[0] = MFControl->var.dFCmd[0] - MFControl->var.dFCmd[1];
		/* ３階微分値 d3FCmd(k) */
		MFControl->var.d3FCmd = MFControl->var.d2FCmd[0] - MFControl->var.d2FCmd[1];
	}
}



/****************************************************************************************************/
/*																									*/
/*		１階微分値算出用２次フィルタ演算															*/
/*																									*/
/****************************************************************************************************/
static	LONG	mfcDtrCmdFilter( MFCTRL *MFControl, LONG pcmd )
{
#if (FLOAT_USE==TRUE)
	float	wk;
	float	dFilo;

	wk = ( pcmd - MFControl->var.Filo ) - MFControl->var.dFilwk;
	wk = wk * MFControl->conf.MfcPexe.Kf2;
	MFControl->var.dFilwk += wk;

	dFilo = MFControl->var.dFilwk * MFControl->conf.MfcPexe.Kf3;
	
	wk = MFControl->var.dFilwk * MFControl->conf.MfcPexe.Kf1[1];
	MFControl->var.Filo += wk;

#else
	LONG	wk;
	LONG	dFilo;

	wk = ( pcmd - MFControl->var.Filo ) - MFControl->var.dFilwk;
	wk = MlibPfbkxremNolim( wk, MFControl->conf.MfcPexe.Kf2, &MFControl->var.Fwkrem1 );
	MFControl->var.dFilwk += wk;

	dFilo = MlibMulgain32( MFControl->var.dFilwk, MFControl->conf.MfcPexe.Kf3 );

	wk = MlibPfbkxremNolim( MFControl->var.dFilwk, MFControl->conf.MfcPexe.Kf1[1], &MFControl->var.Fwkrem2 );
	MFControl->var.Filo += wk;
#endif /* FLOAT_USE */

	return( dFilo );
}



/****************************************************************************************************/
/*																									*/
/*		MFCパラメータ切替え完了ステータス取得API													*/
/*																									*/
/*			ワンパラ等での切替確認に使用する。モデルのタイプとモデルゲイン、モデル周波数を比較する。*/
/*			調整機能ではモデルＦＦは扱わないため，モデルタイプとモデルゲイン、モデル周波数を		*/
/*			監視すれば良い。モデルＦＦゲインなどその他のパラメータでの判別は不可のため注意。		*/
/*																									*/
/****************************************************************************************************/
SHORT	MfcGetParamEnableSts( MFCTRL *MFControl )
{
	SHORT	rc;

	if( (MFControl->conf.MfcPexe.MFCModel == MFControl->conf.MfcPrm[0].MFCModel)
		&& (MFControl->conf.MfcPexe.Kj2 == MFControl->conf.MfcPrm[0].Kj2)
		&& (MFControl->conf.MfcPexe.Kf2 == MFControl->conf.MfcPrm[0].Kf2) )
	{ /* 実行モデル/実行モデル周波数/実行モデルゲインがパラメータと同じ場合 */
		rc = TRUE;
	}
	else
	{
		rc = FALSE;
	}
	return rc;
}



/****************************************************************************************************/
/*																									*/
/*		モデル追従制御停止API																		*/
/*			※実際にMFCが停止するのは、モータ停止後になるので注意									*/
/*																									*/
/****************************************************************************************************/
void	MfcSetMfcCtrlSwitch( MFCTRL *MFControl, BOOL Switch )
{
	if( Switch == TRUE )
	{ /* モデル追従制御有効化要求時 */
		MFControl->var.MfcStopReq = FALSE;
	}
	else
	{ /* モデル追従制御無効化要求時 */
		MFControl->var.MfcStopReq = TRUE;
	}
}





/****************************************************************************************************/
/*																									*/
/*		ＭＦＣタイプ選択																			*/
/*																									*/
/****************************************************************************************************/
/*		戻り値	：	Error Status (FALSE:エラーなし, TRUE:パラメータ設定異常)						*/
/****************************************************************************************************/
BOOL	MfcCalculateInitPrm( MFCTRL *MFControl, MFC_CFG_PRM *MfcCfgPrm,
    	                     USHORT jrate, BPRMDAT *Bprm, USHORT mfctype )
{
	BOOL	PrmErr;

	PrmErr = FALSE;

	switch(mfctype & 0x0F)
	{
	case 0x00: /* MFC処理タイプ(SGDX互換) */
		MFControl->conf.MFCTypeSetting = SGDX_TYPE;
		break;
	case 0x01: /* MFC処理タイプ(SGDV) */
		MFControl->conf.MFCTypeSetting = SGDV_TYPE;
		break;
	default:
		PrmErr = TRUE;
		break;
	}

	/* 第１バンク	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 0 );
	/* 第２バンク	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 1 );

	return	PrmErr;
}



/****************************************************************************************************/
/*																									*/
/*		ＭＦＣ選択																					*/
/*																									*/
/****************************************************************************************************/
/*		戻り値	：	Error Status (FALSE:エラーなし, TRUE:パラメータ設定異常)						*/
/****************************************************************************************************/
BOOL	MfcCalculatePrmSW( MFCTRL *MFControl, MFC_CFG_PRM *MfcCfgPrm, USHORT jrate, BPRMDAT *Bprm )
{
	BOOL	PrmErr;
	LONG	wk;

	PrmErr = FALSE;
	wk = MfcCfgPrm->mdlsw & 0xFF;

	switch( wk )
	{
	case 0x00: /* 機能無効 */
	case 0x10: /* 機能無効 */
	case 0x20: /* 機能無効 */
		MFControl->conf.MFCModelSetting = 0;
		break;
	case 0x01: /* モデル追従制御 */
		/* 剛体系モデル追従制御 */
		MFControl->conf.MFCModelSetting = ONE_MASS;
		break;
	case 0x11: /* 振動抑制付モデル追従制御 */
	case 0x21: /* 振動抑制２付モデル追従制御 */
		/* 剛体系＋機台モデル追従制御 */
		MFControl->conf.MFCModelSetting = ONE_BASE;
		break;
	default:
		PrmErr = TRUE;
		break;
	}

	wk = (MfcCfgPrm->mdlsw >> 12) & 0x0F;
	switch(wk)
	{
	case 0x00:
		MFControl->conf.MFCFF = FALSE;
		break;
	case 0x01:
		MFControl->conf.MFCFF = TRUE;
		break;
	default:
		PrmErr = TRUE;
		break;
	}

	/* 第１バンク	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 0 );
	/* 第２バンク	*/
	MfcCalculatePrm( MFControl, MfcCfgPrm, jrate, Bprm, 0, 1 );

	return	PrmErr;
}


/****************************************************************************************************/
/*																									*/
/*		ＭＦＣパラメータ計算																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*			  LONG		cfric			:	クーロン摩擦トルクパラメータ							*/
/*			  LONG		GeslNo			:	パラメータバンク										*/
/*												0：第1バンク，1:第2バンク							*/
/*																									*/
/****************************************************************************************************/
void	MfcCalculatePrm( MFCTRL *MFControl, MFC_CFG_PRM	*MfcCfgPrm,
						 USHORT jrate, BPRMDAT *Bprm, LONG cfric, LONG GselNo )
{
	MFCPRM		wrkp;
	USHORT		mdlgn;
	USHORT		mdlzeta;
	KSGAIN		hz;				/*<S00A>*/
	LONG		kx, sx;
	LONG		ky, sy;
	LONG		wk;
	KSGAIN		scantime;		/*<S00A>*/
	KSGAIN		maxpls;			/*<S00A>*/
#if (FLOAT_USE==TRUE)
	float fw;
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ゲインNo.選択																				*/
/*--------------------------------------------------------------------------------------------------*/
	if( GselNo == 0 )
	{ /* 第一ゲイン設定時 */
		mdlgn = MfcCfgPrm->mdlgn;
		mdlzeta = MfcCfgPrm->mdlzeta;
	}
	else
	{ /* 第二ゲイン設定時 */
		mdlgn = MfcCfgPrm->mdlgn2;
		mdlzeta = MfcCfgPrm->mdlzeta2;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		機械モデル																					*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.MFCModel = MFControl->conf.MFCModelSetting;

/*--------------------------------------------------------------------------------------------------*/
/*		MFCタイプ		<V509>																		*/
/*--------------------------------------------------------------------------------------------------*/
	wrkp.MFCType = MFControl->conf.MFCTypeSetting;

/*--------------------------------------------------------------------------------------------------*/
/*		演算サイクルタイム	[ns]																	*/
/*--------------------------------------------------------------------------------------------------*/
	if( wrkp.MFCModel == ONE_MASS )							/* 剛体モデル							*/
	{
		scantime = (LONG)Bprm->SvCycleNs;
//		scantime = (LONG)MFCONEMASSCYCLE;
	}
	else													/* ２慣性モデル							*/
	{
		scantime = (LONG)MFCTWOMASSCYCLE(Bprm->SvCycleNs);
//		scantime = (LONG)MFCTWOMASSCYCLE;
	}

#if 0
	if( (Kprm.f.FencUse == TRUE)							/* Full-Closed Control					*/
		&& (wrkp.MFCModel != ONE_BASE) )					/* 剛体＋機台でない→二慣性モデル		*/
	{
		wrkp.Fccflag = 1;
	}
	else
#endif
	{
		wrkp.Fccflag = 0;
	}

#if 0 /* PSフルク用フラグ(未対応) */
	wrkp.PSFccflag = (Iprm.f.PsFulcSelect) ? 1 : 0 ;		/* PS-Full-Closed Control				*/
#else
	wrkp.PSFccflag = 0;
#endif

/* 2011.04.04 Y.Oka <V731> */
/*--------------------------------------------------------------------------------------------------*/
/*		モータOS速度パルス計算	[Pluse/Scan]				Bprm.MaxPspd：[Pulse/sec]				*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	maxpls = (scantime * Bprm->MaxPspd / (float)C10POW9);
#else
	maxpls = MlibScalKxkskx( scantime, Bprm->MaxPspd, C10POW9, NULL, -30 );
#endif /* FLOAT_USE */

#if 0
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＴＲ高次フィルタ分解能シフト量 Fsft														*/
/*--------------------------------------------------------------------------------------------------*/
/*						   2^29																		*/
/*				Fsft = ------------		InvFsft = 1/Fsft											*/
/*				   		  FbPulse																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Fsft = 0x20000000 / Bprm->FbPulse;
	if( wrkp.Fsft < 1.0 )
	{
		wrkp.Fsft = 1.0;
	}
	wrkp.InvFsft = 1.0F / wrkp.Fsft;
#else
	wrkp.Fsft = 0x20000000 / Bprm->FbPulse;
	if( wrkp.Fsft < 1 )
	{
		wrkp.Fsft = 1;
	}
	wrkp.InvFsft = MlibScalKxgain( 1, 1, wrkp.Fsft, NULL, 24 );
#endif /* FLOAT_USE */

#endif
/* 2011.04.04 Y.Oka <V731> */

/*--------------------------------------------------------------------------------------------------*/
/*		２次フィルタゲイン１ Kf1																	*/
/*--------------------------------------------------------------------------------------------------*/
/*						   Ts[us] * 0x1000000		    Ts[us] * 0x1000000							*/
/*				Kf1[0] = ---------------------- = --------------------------------					*/
/*				    	    Tf[us] + Ts[us]	        (10^6 / 2ζωn)[us] + Ts[us]					*/
/*																									*/
/*						     ωn * Ts[us]															*/
/*				Kf1[1] = ----------------------														*/
/*						   2 * ζ[1.0] * 10^6														*/
/*--------------------------------------------------------------------------------------------------*/
/*				Ts[us]  = SvCycleUs[us]																*/
/*																									*/
/*							mdlzeta[0.0001]															*/
/*				ζ[1.0] = -------------------														*/
/*							    10000																*/
/*		(剛体)																						*/
/*											    2*mdlzeta[0.001]*mdlgn[0.1/s]						*/
/*				ωn = 2 * ζ[1.0] * Km[1/s] = ----------------------------------					*/
/*														1000 * 10									*/
/*		(二慣性, 剛体＋機台)																		*/
/*		 								        4*mdlzeta[0.001]*mdlgn[0.1/s]						*/
/*				ωn = 4 * ζ[1.0] * Km[1/s] = ----------------------------------					*/
/*														1000 * 10									*/
/*																									*/
/*					  Km : モデルループゲイン(指令フィルタゲイン)									*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		フィルタ周波数 hz[0.1Hz] = ωn*10 / 2*PAI の計算										*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)

	if( wrkp.MFCModel == ONE_MASS )							/* 剛体モデル							*/
	{
		hz = 4.0f * (float)mdlzeta * (float)mdlgn * 10.0f / 62832.0f;
	}
	else													/* 二慣性、剛体＋機台モデル				*/
	{
		hz = 4.0f * (float)mdlzeta * (float)mdlgn * 10.0f / 62832.0f;
	}

#else	
	if( wrkp.MFCModel == ONE_MASS )							/* 剛体モデル							*/
	{
		hz = MlibScalKxgain( 4*mdlzeta, mdlgn*10, 62832, NULL, -24 );
	}
	else													/* 二慣性、剛体＋機台モデル				*/
	{
		hz = MlibScalKxgain( 4*mdlzeta, mdlgn*10, 62832, NULL, -24 );
	}
#endif /* FLOAT_USE */

	/*----------------------------------------------------------------------------------------------*/
	/*		２次フィルタゲイン Kf1[0], Kf1[1] 計算													*/
	/*----------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	FlibPcalKf2gain( hz, mdlzeta, scantime, wrkp.Kf1);
#else
	MlibPcalKf2gain( hz, mdlzeta, scantime, wrkp.Kf1, 0x10 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		２次フィルタゲイン２ Kf2																	*/
/*--------------------------------------------------------------------------------------------------*/
/*						    Kf1[0]																	*/
/*				Kf2 = -----------------																*/
/*				    	  0x1000000																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kf2 = (wrkp.Kf1[0] / (float)0x1000000);
#else
	wrkp.Kf2 = MlibScalKxgain( wrkp.Kf1[0], 1, 0x1000000, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		２次フィルタゲイン３ Kf3																	*/
/*--------------------------------------------------------------------------------------------------*/
/*						Kf1[1] * 1000000000															*/
/*				Kf3 = --------------------															*/
/*				    	 SvCycleUs[ns]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kf3 = wrkp.Kf1[1] * (float)C10POW9 / scantime;
#else
	wrkp.Kf3 = MlibScalKskxkx( wrkp.Kf1[1], C10POW9, scantime, NULL, 24 );
#endif /* FLOAT_USE */

/* 2011.04.04 Y.Oka <V731> */
/*--------------------------------------------------------------------------------------------------*/
/*		DTR高次フィルタ分解能シフト量 Fsft															*/
/*--------------------------------------------------------------------------------------------------*/
/*																									*/
/*				Fsft = 2^30/((maxpls/kf[1]))			maxpls：最大パルス/制御周期					*/
/*																									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	kx = wrkp.Kf1[1] * 0x3FFFFFFF / maxpls;

	wk = 0x20000000 / Bprm->FbPulse;
	wrkp.Fsft = FlibLimitul( kx, wk, 1.0f );								/* 1 < kx < (2^29/plsno)	*/

	wrkp.InvFsft = (1.0F / wrkp.Fsft);			/* invFsft = 1/ Fsft		*/
/* 2011.04.04 Y.Oka <V731> */
#else
	kx = MlibScalKskxkx( wrkp.Kf1[1], 0x3FFFFFFF, maxpls, NULL, -24 );

	wk = 0x20000000 / Bprm->FbPulse;
	wrkp.Fsft = MlibLimitul( kx, wk, 1 );								/* 1 < kx < (2^29/plsno)	*/

	wrkp.InvFsft = MlibScalKxgain( 1, 1, wrkp.Fsft, NULL, 24 );			/* invFsft = 1/ Fsft		*/
/* 2011.04.04 Y.Oka <V731> */
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		メカ係数ゲイン１ Kj1																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		             			  1000000000       100 + jrate   									*/
/*				Kj1 =  Kpvx * ---------------- * ---------------									*/
/*		            			SvCycleUs[ns]         100        									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Kj1 = Bprm->Kpvx * C10POW9 / scantime * ((jrate + 100)/100.0f);
#else
	kx = MlibScalKskxkx( Bprm->Kpvx, C10POW9, scantime, &sx, 0 );
	wrkp.Kj1 = MlibPcalKxgain( kx, (jrate + 100), 100, &sx, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		メカ係数ゲイン２ Kj2																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		             		           100 * 1000000000    											*/
/*				Kj2 = -----------------------------------------------------							*/
/*		            	antfrq[0.1Hz]^2 * (2*PAI)^2 * SvCycleUs[ns] * Fsft							*/
/*																									*/
/*		             	 	           100 * 1000000000    											*/
/*				Kj2_sft = ----------------------------------------------							*/
/*		               	   antfrq[0.1Hz]^2 * (2*PAI)^2 * SvCycleUs[ns]								*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	//wrkp.Kj2 = 1.0 / ((MfcCfgPrm->mdlantfrq*0.1)*(MfcCfgPrm->mdlantfrq*0.1) * (2*PAI)*(2*PAI) * scantime * wrkp.Fsft);
	wrkp.Kj2 = 100.0f * E9C2PAI2 / (MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq * scantime / wrkp.Fsft);

	//wrkp.Kj2_sft = 1.0 / ((MfcCfgPrm->mdlantfrq*0.1)*(MfcCfgPrm->mdlantfrq*0.1) * (2*PAI)*(2*PAI) * scantime * wrkp.Fsft);
	wrkp.Kj2_sft = 100.0f * E9C2PAI2 / (MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq * scantime);
#else
	sx = 0;
	sy = 0;
	wk = MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq;
	kx = MlibPcalKxmulx( 100, E9C2PAI2, 1, &sx );
	ky = MlibPcalKxmulx( wk, scantime, wrkp.Fsft, &sy );
	wrkp.Kj2 = MlibPcalKxdivx( kx, sx, ky, sy, 24 );

	sx = 0;
	sy = 0;
	wk = MfcCfgPrm->mdlantfrq * MfcCfgPrm->mdlantfrq;
	kx = MlibPcalKxmulx( 100, E9C2PAI2, 1, &sx );
	ky = MlibPcalKxmulx( wk, scantime, 1, &sy );
	wrkp.Kj2_sft = MlibPcalKxdivx( kx, sx, ky, sy, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		メカ係数ゲイン３ Kj3																		*/
/*--------------------------------------------------------------------------------------------------*/
/*		             		           100 * 1000000000^2  											*/
/*				Kj3 = --------------------------------------------------------						*/
/*		            	resfrq[0.1Hz]^2 * (2*PAI)^2 * SvCycleUs[ns]^2 * Fsft						*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	//wrkp.Kj3 = 1.0 / ((MfcCfgPrm->mdlresfrq*0.1)*(MfcCfgPrm->mdlresfrq*0.1) * (2*PAI)*(2*PAI) * scantime*scantime * wrkp.Fsft);
	wrkp.Kj3 = (100 * E9C2PAI2 * 1000000000 / wrkp.Fsft) / (MfcCfgPrm->mdlresfrq * MfcCfgPrm->mdlresfrq * scantime * scantime);
#else
	sx = 0;
	sy = 0;
	kx = MlibPcalKxmulx( 100, E9C2PAI2, 1000000000, &sx);
	kx = MlibPcalKxgain( kx, 1, wrkp.Fsft, &sx , 0 );

	wk = MfcCfgPrm->mdlresfrq * MfcCfgPrm->mdlresfrq;
	ky = MlibPcalKxmulx( wk, scantime, scantime, &sy);
	wrkp.Kj3 = MlibPcalKxdivx(  kx, sx, ky, sy, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		ＤＴＲ速度フィードフォワードゲイン Kvff														*/
/*--------------------------------------------------------------------------------------------------*/
/*		             		   mfckvff[0.1%] * 1000000000  											*/
/*				Kvff = Kpx * ---------------------------											*/
/*		            			 1000 * SvCycleUs[ns]												*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	if(wrkp.MFCType == SGDX_TYPE)
	{ /* MFC処理タイプ(SGDX互換) */
		wrkp.Kvff = Bprm->Kpx * 1000000000.0f / scantime * MfcCfgPrm->mdlvff / 1000.0f ;
	}
	else
	{
		wrkp.Kvff = Bprm->Kpx * 1000000000.0f / scantime / wrkp.Fsft * MfcCfgPrm->mdlvff / 1000.0f;
	}
#else
	kx = MlibScalKskxkx( Bprm->Kpx, 1000000000, scantime, &sx, 0 );
	if(wrkp.MFCType == SGDX_TYPE)
	{ /* MFC処理タイプ(SGDX互換) */
		wrkp.Kvff = MlibPcalKxgain( kx, MfcCfgPrm->mdlvff, 1000, &sx, 24 );
	}
	else
	{
		kx = MlibPcalKxgain( kx, 1, wrkp.Fsft, &sx, 0 );
		wrkp.Kvff = MlibPcalKxgain( kx, MfcCfgPrm->mdlvff, 1000, &sx, 24 );
	}
#endif /* FLOAT_USE */
/*--------------------------------------------------------------------------------------------------*/
/*		ＤＴＲトルクフィードフォワードゲイン Ktff1, Ktff2											*/
/*--------------------------------------------------------------------------------------------------*/
/*		                   mfcktff[0.1%]    			  mfctffgn2[0.1%]							*/
/*				Ktff1 = ------------------		Ktff2 = -------------------							*/
/*		       (正転用)	       1000 		   (逆転用)		   1000									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Ktff1 = MfcCfgPrm->mdlptff / 1000.0f;
	wrkp.Ktff2 = MfcCfgPrm->mdlntff / 1000.0f;
#else
	wrkp.Ktff1 = MlibScalKxgain( MfcCfgPrm->mdlptff, 1, 1000, NULL, 24 );
	wrkp.Ktff2 = MlibScalKxgain( MfcCfgPrm->mdlntff, 1, 1000, NULL, 24 );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		クーロン摩擦補償トルク Cfric 																*/
/*--------------------------------------------------------------------------------------------------*/
/*				トルクレベル計算 KpiTorqueLevelCal() : Torque[0.01%] --> NorTrq[2^24/MaxTrq]		*/
/*														cfric[0.1%]									*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	wrkp.Cfric = cfric*10 * Bprm->Ktrqlvl;
#else
	wrkp.Cfric = MlibMulgain( cfric*10, Bprm->Ktrqlvl );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		クーロン摩擦補償フィルタゲイン																*/
/*--------------------------------------------------------------------------------------------------*/
/*							    SvCycleUs[us]														*/
/*				Kfcf = -----------------------------------											*/
/*						     5000000																*/
/*						 ---------------- + SvCycleUs[us]											*/
/*						   mdlgn[0.1/s]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	fw = 5000000.0f / mdlgn;
	if( fw > 100000.0f )
	{
		fw = 100000.0f;
	}
	wrkp.Kfcf = FlibPcalKf1gain( fw, (scantime/1000.0f), 0 );
#else
	kx = (LONG)( 5000000 / (SHORT)mdlgn );
	if( kx > 100000 )
	{
		ky = 100000;
	}
	else
	{
		ky = kx;
	}
	wrkp.Kfcf = MlibPcalKf1gain( ky, (scantime/1000), 0 );
#endif /* FLOAT_USE */

/* 2009.12.24 Y.Oka 割り込み禁止必要？ */
/*--------------------------------------------------------------------------------------------------*/
/*		Set Parameters																				*/
/*--------------------------------------------------------------------------------------------------*/
//	KPI_DI();		/* Disable Interrupt	*/
	MFControl->conf.MfcPrm[GselNo+1] = wrkp;
//	KPI_EI();		/* Enable Interrupt		*/

}



/***************************************** end of file **********************************************/
