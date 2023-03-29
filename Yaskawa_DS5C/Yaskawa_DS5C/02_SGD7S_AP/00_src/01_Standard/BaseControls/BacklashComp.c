/****************************************************************************************************/
/*																									*/
/*																									*/
/*		BacklashComp.c : バックラッシ補正機能	モジュール定義										*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2011.04.05	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "BacklashComp.h"



#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		バックラッシ補正処理																		*/
/*																									*/
/****************************************************************************************************/
void	BlshCompensateBacklash( BLSHCMP *BlshCmp, EGEAR *Egear,
									LONG *dPosRef, LONG *dPcmd, BOOL *RefZ, BOOL BlshActive )
{
	LONG	dAmountPls;
	LONG	work;		/* <S1D8> */

	/*------------------------------------------------------------------------------------------*/
	/*		ベースブロック時など：補正なしの状態に戻す(モータ位置に戻す）						*/
	/*------------------------------------------------------------------------------------------*/
	if( BlshActive == FALSE )
	{
		*dPosRef -= BlshCmp->var.LastFilo;
#if (FLOAT_USE==TRUE)
		*dPcmd += FlibEgearRvscnv( -(BlshCmp->var.LastFilo), Egear, &BlshCmp->var.Cmpgrrem );
#else
		*dPcmd += MlibEgearRvscnv( -(BlshCmp->var.LastFilo), Egear, &BlshCmp->var.Cmpgrrem );
#endif /* FLOAT_USE */
		BlshCmp->var.BlshCmpAmountPls = -(BlshCmp->var.LastFilo);
		work = BlshCmp->var.BlshCmpAmountPls;								/* <S1D8> */
		MlibResetLongMemory( &BlshCmp->var, sizeof(BlshCmp->var)/4 );
		BlshCmp->var.BlshCmpAmountPls = work;			/* バックラッシ補正量差分値は、対象外	*//* <S1D8> */

	}
	/*------------------------------------------------------------------------------------------*/
	/*		ベースイネーブル時：バックラッシ補正処理演算										*/
	/*------------------------------------------------------------------------------------------*/
	else
	{
		/*--------------------------------------------------------------------------------------*/
		/*	バックラッシ補正方向判別、補正量設定												*/
		/*--------------------------------------------------------------------------------------*/
		if( *dPosRef != 0 )
		{
			if( (*dPosRef > 0) && (BlshCmp->conf.BacklashDir == FOR_BACKLASH) )
			{
				BlshCmp->var.Cmppos = BlshCmp->conf.CompPulse;
			}
			else if( (*dPosRef < 0) && (BlshCmp->conf.BacklashDir == REV_BACKLASH) )
			{
				BlshCmp->var.Cmppos = -BlshCmp->conf.CompPulse;
			}
			else
			{
				BlshCmp->var.Cmppos = 0;
			}
		}
		/*--------------------------------------------------------------------------------------*/
		/*	バックラッシ補正量フィルタ処理														*/
		/*--------------------------------------------------------------------------------------*/
		dAmountPls = MlibMulkxBiasrem( BlshCmp->var.Cmppos - BlshCmp->var.LastFilo,
									   BlshCmp->conf.Kfil,
									   &BlshCmp->var.remx,
									   0 );
		BlshCmp->var.Filo = BlshCmp->var.LastFilo + dAmountPls;		/* 現在までの補正量			*/
		BlshCmp->var.LastFilo = BlshCmp->var.Filo;					/* フィルタ出力前回値更新	*/
		/*--------------------------------------------------------------------------------------*/
		/*	バックラッシ補正量の指令加算処理													*/
		/*		*dPosRef：パルス単位の位置指令に加算、*dPcmd：指令単位の位置指令に加算			*/
		/*--------------------------------------------------------------------------------------*/
		*dPosRef += dAmountPls;
#if (FLOAT_USE==TRUE)
		*dPcmd += FlibEgearRvscnv( dAmountPls, Egear, &BlshCmp->var.Cmpgrrem );
#else
		*dPcmd += MlibEgearRvscnv( dAmountPls, Egear, &BlshCmp->var.Cmpgrrem );
#endif /* FLOAT_USE */
		/*--------------------------------------------------------------------------------------*/
		/*	FB位置情報モニタ処理用バックラッシ補正量差分値更新									*/
		/*		BoutV.BlshCmpAmountPls：パルス単位のFB位置差分値からの減算用					*/
		/*--------------------------------------------------------------------------------------*/
		BlshCmp->var.BlshCmpAmountPls = dAmountPls;
	}
	/*------------------------------------------------------------------------------------------*/
	/*	位置指令払い出し完了（DEN信号）更新														*/
	/*		補正量を加算した指令!=0 or 補正量フィルタバッファ!=0：DEN信号=FALSE(未完)			*/
	/*------------------------------------------------------------------------------------------*/
	if( (*dPosRef!=0) || ((BlshCmp->var.Cmppos - BlshCmp->var.LastFilo) != 0) )
	{
		*RefZ = FALSE;
	}
	else
	{
		/* 上記以外の場合、補正フィルタゲインのパラメータ有効 */
		BlshCmp->conf.Kfil = BlshCmp->conf.KfilTmp;
	}

	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		バックラッシ補正 補正量制限値計算															*/
/*																									*/
/****************************************************************************************************/
void	BlshCalcMaxBlshCompPulse( BLSHCMP *BlshCmp, EGEAR *Egear, BPRMDAT *Bprm )
{
	/*----------------------------------------------------------------------------------------------*/
	/*	2×モータ最大速度パルス計算（MaxCompPulse）：パルス単位/sec→パルス単位/ScanB[us]			*/
	/*		Bprm.MaxPspd：[Pulse/sec]																*/
	/*----------------------------------------------------------------------------------------------*/
	BlshCmp->conf.MaxCompPulse = MlibScalKxkskx( Bprm->SvCycleUs, Bprm->MaxPspd, 500000, NULL, -30 );
//	blcm2_UpLim = ULONGOF( pndef_blcm2l.UpperLimit, pndef_blcm2h.UpperLimit );

	/*----------------------------------------------------------------------------------------------*/
	/*	バックラッシ補正量制限値計算（モニタ用）   ：ﾊﾟﾙｽ単位/ScanB[us]→0.1指令単位/ScanB[us]		*/
	/*----------------------------------------------------------------------------------------------*/
	BlshCmp->conf.MaxCompRef =
		(ULONG)MlibPcalKxgain( BlshCmp->conf.MaxCompPulse*10, Egear->a, Egear->b, NULL, -30 );
	BlshCmp->conf.MaxCompRef =
		( BlshCmp->conf.MaxCompRef > BLSHCOMP_UPLIM )? BLSHCOMP_UPLIM : BlshCmp->conf.MaxCompRef;

	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		バックラッシ補正 補正量（Pn231）パラメータ計算												*/
/*																									*/
/****************************************************************************************************/
void	BlshCalcCompensateAmount( BLSHCMP *BlshCmp, EGEAR *Egear, LONG blcm2l )
{
	LONG	lwk;
	LONG	kx;
	LONG	sx;

	/*----------------------------------------------------------------------------------------------*/
	/*	Pn231、Pn232計算（PrmCompPulse）：指令単位→パルス単位										*/
	/*----------------------------------------------------------------------------------------------*/
	kx = MlibScalKxgain( blcm2l, 1, 10, &sx, 0 );
	lwk = MlibPcalKxgain( kx, Egear->b, Egear->a, &sx, -30 );

	/*----------------------------------------------------------------------------------------------*/
	/*	補正量パルスリミット処理（BlshCmp->conf.CompPulse）											*/
	/*		補正量がモータ最大速度の2倍のパルス以上であれば、モータ最大速度の2倍のパルスにする		*/
	/*----------------------------------------------------------------------------------------------*/
	BlshCmp->conf.CompPulse = MlibLimitul(lwk, BlshCmp->conf.MaxCompPulse, -BlshCmp->conf.MaxCompPulse);
	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		バックラッシ補正 補正時定数（Pn233）パラメータ計算											*/
/*																									*/
/****************************************************************************************************/
void	BlshCalcCompensateTime( BLSHCMP *BlshCmp, LONG SvCycleUs, USHORT blfiltime )
{
	BlshCmp->conf.KfilTmp = MlibScalKxgain( SvCycleUs, 1, (SvCycleUs + (blfiltime * 10)), NULL, 24 );
	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		バックラッシ補正 補正状態無効処理															*/
/*																									*/
/****************************************************************************************************/
void	BlshInvaildCompensate( BLSHCMP *BlshCmp )
{
	LONG work;							/* <S1D8> */

	/* 補正状態無効処理：APOS=FB位置にする */
	BlshCmp->var.BlshCmpAmountPls = -BlshCmp->var.LastFilo;
	/* 補正フィルタゲインのパラメータ有効 */
	BlshCmp->conf.Kfil = BlshCmp->conf.KfilTmp;
	/* バックラッシ補正変数初期化 */
	work = BlshCmp->var.BlshCmpAmountPls;							/* <S1D8> */
	MlibResetLongMemory( &BlshCmp->var, sizeof(BlshCmp->var)/4 );			
	BlshCmp->var.BlshCmpAmountPls = work;			/* バックラッシ補正量差分値は、対象外	*/	/* <S1D8> */
	return;
}
#endif


#if (CSW_SVCF_BACKLASHCOMP == TRUE)
/****************************************************************************************************/
/*																									*/
/*		バックラッシ補正 補正量初期化処理															*/
/*																									*/
/****************************************************************************************************/
void	BlshInitializeCompensate( BLSHCMP *BlshCmp )
{
	/* 補正フィルタゲインのパラメータ有効 */
	BlshCmp->conf.Kfil = BlshCmp->conf.KfilTmp;
	/* バックラッシ補正変数初期化 */
	MlibResetLongMemory( &BlshCmp->var, sizeof(BlshCmp->var)/4 );
	return;
}
#endif



/***************************************** end of file **********************************************/
