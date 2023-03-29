/****************************************************************************************************/
/*																									*/
/*	JatOffLine.c : オフラインオートチューニングモジュール											*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*	概要：	オフライン方式のイナーシャ同定を行う。位置制御での実行のため，垂直軸でも同定可能。		*/
/*			ＤＤモータ，リニアモータも同定可能														*/
/*			イナーシャ比は，正転／逆転を１セットで行う。そのため，									*/
/*			イナーシャ同定時の動作は，正転／逆転(同定)→逆転／正転(同定)→Jrate更新となる			*/
/*																									*/
/*                         同定実行(正)   同定実行(逆)           J更新								*/
/*                       |--------------|---------------|       |-----|								*/
/*               _______________                                									*/
/*           ____|              |_______________                  ____  							*/
/*                                              |________________|									*/
/*																									*/
/*		注意：																						*/
/*		  同定指令																					*/
/*			①正転→逆転，逆転→正転切替時の停止時間はほぼゼロである必要があります。				*/
/*			  停止時間が長いと，静止摩擦の影響で同定精度が悪化します。								*/
/*			②速度=500min-1以上(定格3000min-1)，加速時間は，30ms以上が理想です。					*/
/*			  三角指令の場合，同定実行判別精度が悪くなるので，出来るだけ，							*/
/*			  一定速時間を設けた方が良いです。														*/
/*			③同定値の反映は，「同定完了かつゼロ速」で判別，指令停止時間は，300ms程度設けること		*/
/*																									*/
/*		  サーボゲイン																				*/
/*			①本方式は，往復動作後にイナーシャを更新するため，出荷時ゲインでは						*/
/*			  大イナーシャ時の同定でKp>>Kvとなって，位置ループの振動が発生する。					*/
/*			  従ってﾊﾞﾗﾝｽ位置制御で実行するが，未知のイナーシャを同定するため，Kp/Kv→小が必要		*/
/*			  よってKp=Kv/10，VFF=100%とする。														*/
/*			②サーボゲインは，Pn100，Pn102：第一Kv，Kpを使用する(同定中のゲイン切替え不可)			*/
/*			③イナーシャ同定開始時のイナーシャ比設定はPn103ではなく，専用パラメータとする			*/
/*			  イナーシャ比=300%の場合は，約100倍負荷まで同定可能(ただしトルク飽和しないこと)。		*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*																									*/
/*	T.Kira 2011.03.01   初版																		*/
/****************************************************************************************************/
#include "Basedef.h"
#include "JatOffLine.h"
#include "ServoIf.h"
#include "PnPrmListTbl.h"

/****************************************************************************************************/
/*																									*/
/*		関数一覧																					*/
/*																									*/
/****************************************************************************************************/
static void jat_offline_st_detect( JATV *jatv, LONG ActPos);
static void jat_estimate( JATHNDL *JatHdl );
static void jat_modelcal( JATHNDL *JatHdl, CTRL_CMD_PRM *CtrlCmdPrm, LONG spdref );
static void jat_sumtrf_offline( JATHNDL *JatHdl, LONG TrqRef);


/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定スキャンＣ処理														*/
/*																									*/
/*	概要:	同定開始判別，イナーシャ比計算，振動検出を行う。										*/
/*																									*/
/*	arg		:																						*/
/*	g-ref	:	LONG	CoutV.UnMotSpdx			:	ScanCモータ速度				[2^24/OvrSpd]		*/
/*				UINT	BoutV.f.Ctrl.TrqClamp	:	トルク制限中フラグ								*/
/*				UINT	BinV.f.BaseEnable		:	ベースイネーブルフラグ							*/
/*				UINT	BoutV.f.PerClrCmd		:	偏差クリアフラグ								*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
void CpxJatOfflineCalc( JATHNDL *JatHdl, LONG UnMotSpdx, DETVIB *DetVib, BOOL PConActFlag, BOOL PerrClrReq )
{
	LONG	wk;
//	BOOL	PConActFlag;
	JATV	*jatv;
	JATP	*jatp;

	jatv = &JatHdl->jatv;
	jatp = &JatHdl->jatp;
//	PConActFlag = ((CmdCtrlBit >> ENBPCTRL_BITNO) & 0x01);

	/*--------------------------------------------------------------------------*/
	/*	機能制限																*/
	/*--------------------------------------------------------------------------*/
	if (jatv->enable == TRUE)				/* イナーシャ同定有効				*/
	{
		/*----------------------------------------------------------------------*/
		/*	同定エラー検出														*/
		/*----------------------------------------------------------------------*/
		wk = jatv->mnfb - UnMotSpdx;			/* モデル速度 － ＦＢ速度		*/
		jatv->jverr = MlibLpfilter1(wk, jatp->jverrfil, jatv->jverr);

		if((DetVib->conf.JstVibCheckLevel << 1) < MlibABS(jatv->jverr))
		{	/* エラー検出レベル超過	*/
			jatv->status |= JATERR;				/* エラーステータスセット		*/
		}

		if( ( PConActFlag == TRUE) || ( PerrClrReq == TRUE ))
		{	/* 比例制御中 or 偏差クリア中 */
			jatv->status |= JATEXEERR;			/* 実行エラーステータスセット	*/
		}
		/*----------------------------------------------------------------------*/
		/*	同定開始判別														*/
		/*----------------------------------------------------------------------*/
		jat_offline_st_detect(jatv, jatv->fbsum);

		/*----------------------------------------------------------------------*/
		/*	同定イナーシャ比計算												*/
		/*----------------------------------------------------------------------*/
		jat_estimate( JatHdl );
	}
}

/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定開始／終了判別														*/
/*																									*/
/*	概要:ＦＢ位置を監視してイナーシャ同定処理の実行開始／終了フラグを作成する						*/
/*		 また，大イナーシャ時の同定初回の振動を検出する。振動検出したら同定を中止し，				*/
/*		 エラーステータスを返す。																	*/
/*																									*/
/*	arg		:	LONG	ActPos				: 実移動量		[指令単位]								*/
/*	g-ref	:	UINT	CoutV.f.Sv.MotStop	: モータ停止フラグ										*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_offline_st_detect( JATV *jatv, LONG ActPos )
{
	LONG	wk1, wk2;

	wk1 = MlibABS(jatv->TargetDist);				/* 設定移動量	[指令単位]						*/
	wk2 = MlibABS(ActPos);							/* 実移動量		[指令単位]						*/

	switch(jatv->jarte_start)
	{
	case JATINITIAL:	/* イナーシャ同定(トルク積算)未実行	*/
		if(wk1 <= wk2)								/* 同定開始からの移動量が設定値の１／２以上		*/
		{
			jatv->jarte_start = JATCWSTART;			/* 正転→逆転イナーシャ同定実行ON				*/
		}
		break;
	case JATCWSTART:	/* 正転→逆転イナーシャ同定(トルク積算)実行中	*/
		if(wk1 >= wk2)								/* 同定開始からの移動量が設定値の１／２以以下	*/
		{
			jatv->jarte_start = JATCCWSTART;		/* 逆転→正転イナーシャ同定実行ON				*/
		}
		break;
	case JATCCWSTART:	/* 逆転→正転イナーシャ同定(トルク積算)実行中	*/
		if(wk1 <= wk2)								/* 同定開始からの移動量が設定値の１／２以上		*/
		{
			jatv->jarte_start = JATSUMEND;			/* トルク積算実行完了							*/
		}
		break;
	case JATSUMEND:		/* トルク積算完了	*/
		if(wk1 >= wk2)								/* 同定開始からの移動量が設定値の１／２以下		*/
		{
			jatv->jarte_start = JATEXEEND;			/* イナーシャ同定実行完了						*/
		}
		break;
	case JATEXEEND:		/* イナーシャ同定動作完了		*/
		/* 何もしない	*/
	default:
		break;
	}
}

/****************************************************************************************************/
/*																									*/
/*		イナーシャ計算																				*/
/*																									*/
/*	概要:イナーシャを推定し，イナーシャ比を適正に設定する。											*/
/*			本処理はリアルタイム性を要求されないため，スキャンＣで行う								*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_estimate( JATHNDL *JatHdl )
{
	LONG	wk,wk1,wk2,s;
	JATV	*jatv;
	JATP	*jatp;

	jatv = &JatHdl->jatv;
	jatp = &JatHdl->jatp;

	if(jatv->jarte_start == JATEXEEND )
	{	/* トルク積算完了	*/
		/*----------------------------------------------------------------------*/
		/* イナーシャ同定値計算 												*/
		/*		重力軸など一定外乱がある場合，jat.j0，j2は負値を取る場合がある	*/
		/*		正逆転の平均を取ることで一定外乱の影響を除去している			*/
		/*----------------------------------------------------------------------*/
		/* 正転→逆転方向	*/
		if(jatv->msum1 != 0)
		{
			s = 0;
			wk1 = MlibPcalKxgain(jatv->sum1, (jatp->mjrate << JSHBITNUM), jatv->msum1, &s, 24);
			jatv->j0 = MlibKsgain2Long(wk1);
			jatv->status |= JATCWCOMP;								/* 正転→逆転イナーシャ同定実行した	*/
		}
		/* 逆転→正転方向	*/
		if(jatv->msum2 != 0)
		{
			s = 0;
			wk1 = MlibPcalKxgain(jatv->sum2, (jatp->mjrate << JSHBITNUM), jatv->msum2, &s, 24);
			jatv->j2 = MlibKsgain2Long(wk1);
			jatv->status |= JATCCWCOMP;								/* 逆転→正転イナーシャ同定実行した	*/
		}
	}

	/* モニタの更新 イナーシャ比 */
	if((jatv->status & JATCOMP) == JATCOMP)
	{	/* 両方向同定完了	*/
		wk = (jatv->j0 + jatv->j2) >> 1;							/* 総イナーシャ平均値算出			*/

		/* 上下限チェック	*/
		wk1  = (LONG)( pndef_jrate.UpperLimit + 100 );
		wk2  = (LONG)( pndef_jrate.LowerLimit + 100 );
		wk  = MlibLimitul(wk, wk1, wk2);
		jatv->jrate = (USHORT)(wk - 100);							/* イナーシャ比算出					*/

		JatHdl->EstimatJrat = jatv->jrate;							/* 同定値セット						*/

		jatv->status |= JATJRATECOMP;								/* イナーシャ同定完了ステータス		*/
	}
}

/****************************************************************************************************/
/*																									*/
/*		オフラインイナーシャ同定スキャンＢ処理														*/
/*																									*/
/*	概要:	モデル計算，トルク積算を行う。															*/
/*			モデルへの速度指令入力は速度ＦＦを含む，トルク指令入力はフィルタ後トルク指令を使用		*/
/*																									*/
/*	arg		:																						*/
/*	g-ref	:	LONG	AoutV.SpdRefi	:	速度指令				[2^24/OvrSpd]					*/
/*				LONG	BoutV.SpdFFCx	:	速度ＦＦ補償			[2^24/OvrSpd]					*/
/*				LONG	BoutV.dPosFbki	:	位置ＦＢ差分			[Pulse/Scan]					*/
/*				LONG	AoutV.TrqRefo_a	:	フィルタ後トルク指令	[2^24/MaxTrq]					*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
void BpxJatOfflineCalc( JATHNDL	*JatHdl, EGEAR *Egear, CTRL_CMD_PRM *CtrlCmdPrm, 
														CTRL_CMD_MNG *CtrlCmdMngr, LONG	TrqRefo_a )
{
	LONG spdref;
	JATV *jatv;
	
	jatv = &JatHdl->jatv;

	/*--------------------------------------------------------------------------*/
	/*	機能制限																*/
	/*--------------------------------------------------------------------------*/
	if (jatv->enable == TRUE)				/* イナーシャ同定有効				*/
	{
		//BoutV.f.InertiaTuning = TRUE;		/* イナーシャ同定中					*/
		/*----------------------------------------------------------------------*/
		/*	パラメータセット要求がTRUEのときは、パラメータをセット				*/
		/*----------------------------------------------------------------------*/
		if( jatv->JatPrmSetReq == TRUE )
		{
			MlibCopyLongMemory( &JatHdl->jatp, &JatHdl->jatp_wk, sizeof( JatHdl->jatp )>>2 );
			jatv->JatPrmSetReq = FALSE;
		}

		spdref = CtrlCmdMngr->SpdRefi + CtrlCmdMngr->SpdFFCx;		/* <S160>	*/
//		spdref = CtrlCmdMngr->SpdRefo;

		/*----------------------------------------------------------------------*/
		/*	モータモデル計算													*/
		/*----------------------------------------------------------------------*/
		jat_modelcal( JatHdl, CtrlCmdPrm, spdref );

		/*----------------------------------------------------------------------*/
		/*	トルク積算															*/
		/*----------------------------------------------------------------------*/
		jat_sumtrf_offline( JatHdl, TrqRefo_a );
	}
	//else
	//{
		//BoutV.f.InertiaTuning = FALSE;	/* イナーシャ同定未実行				*/
	//}
	/*--------------------------------------------------------------------------*/
	/*	同定開始後フィードバック位置計算										*/
	/*--------------------------------------------------------------------------*/
	jatv->fbsum += MlibEgearRvscnv( CtrlCmdMngr->dPosFbki,
									Egear,
									&jatv->fbsumrem );	/*  実位置	[指令単位]	*/
}

/****************************************************************************************************/
/*																									*/
/*		モデル計算																					*/
/*																									*/
/*	概要:速度指令から，現状のイナーシャモデルの応答を計算する。										*/
/*																									*/
/*	arg		:																						*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_modelcal( JATHNDL *JatHdl, CTRL_CMD_PRM *CtrlCmdPrm, LONG spdref )
{
	LONG	wk1, wk2, wk3, wk4;
	JATV	*jatv;
	JATP	*jatp;

	jatv = &JatHdl->jatv;
	jatp = &JatHdl->jatp;

	jatv->mvref = spdref; 		/* 速度指令入力->モデル速度指令 */
#if (FLOAT_USE==TRUE)
	/* 速度FB計算 */
	jatv->mnfb  = MlibIntegral( jatv->mtrf , jatp->mkj, jatv->mnfbwrk );
	/* 速度制御 */
	wk1 = (LONG)( (float)jatv->mvref * CtrlCmdPrm->PI_rate);
	jatv->mnerp = wk1 - jatv->mnfb;
	wk3 = jatv->mnerp * jatp->mkv;

	jatv->mneri = jatv->mvref - jatv->mnfb;
	wk2 = MlibIntegral(jatv->mneri, jatp->mkvi, jatv->mneribf);
	wk4	= wk3 + wk2;

	/* モデルトルク */
	jatv->mtrf = MlibLpfilter1(wk4, jatp->mklpf, jatv->mtrf);
#else
	/* 速度FB計算 */
	jatv->mnfb  = MlibIntegral( jatv->mtrf , jatp->mkj, jatv->mnfbwrk );
	/* 速度制御 */
	wk1 = MlibMulgainNolim( jatv->mvref, CtrlCmdPrm->PI_rate );
	jatv->mnerp = wk1 - jatv->mnfb;
	wk3 = MlibMulgain(jatv->mnerp, jatp->mkv);

	jatv->mneri = jatv->mvref - jatv->mnfb;
	wk2 = MlibIntegral(jatv->mneri, jatp->mkvi, jatv->mneribf);
	wk4	= wk3 + wk2;

	/* モデルトルク */
	jatv->mtrf = MlibLpfilter1(wk4, jatp->mklpf, jatv->mtrf);
#endif /* FLOAT_USE */
}

/****************************************************************************************************/
/*																									*/
/*		オフライン用トルク積算																		*/
/*																									*/
/*	概要:イナーシャ推定用にモデル演算とトルクの積算をする。											*/
/*																									*/
/*	arg		:	LONG	TrqRef	：トルク指令						[2^24/MaxTrq]					*/
/*	out		:													 									*/
/*																									*/
/****************************************************************************************************/
static void jat_sumtrf_offline( JATHNDL *JatHdl, LONG TrqRef )
{
	JATV	*jatv;
	jatv = &JatHdl->jatv;

	if(jatv->jarte_start == JATCWSTART)
	{	/* 正転→逆転同定開始	*/
		jatv->sum1  += (TrqRef >> JSHBITNUM);
		jatv->msum1 += jatv->mtrf;
		if( (MlibABS(jatv->sum1) > 0x7D000000) || (MlibABS(jatv->msum1) > 0x7D000000) )
		{
			jatv->sum1 = (jatv->sum1 >> 1);
			jatv->msum1 = (jatv->msum1 >> 1);
		}
	}
	else if(jatv->jarte_start == JATCCWSTART)
	{	/* 逆転→正転同定開始	*/
		jatv->sum2  += (TrqRef >> JSHBITNUM);
		jatv->msum2 += jatv->mtrf;
		if( (MlibABS(jatv->sum2) > 0x7D000000) || (MlibABS(jatv->msum2) > 0x7D000000) )
		{
			jatv->sum2 = (jatv->sum2 >> 1);
			jatv->msum2 = (jatv->msum2 >> 1);
		}
	}
}

