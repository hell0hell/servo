/****************************************************************************
Description	: MECHATROLINK Motion Generator
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp., 
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------

****************************************************************************/
#include "Basedef.h"
#include "MtGenerator.h"
#include "MtPRM.h"
#include "MtApi.h"
#include "MLib.h"

/****************************************************************************
 Private functions
****************************************************************************/
/* NOPモーション処理 */
static	BOOL MtgNone( MG_HNDL *hdl, MG_INOUT *inOut );
/* Follow up モーション処理 */
static	BOOL MtgFollowup( MG_HNDL *hdl, MG_INOUT *inOut );
/* 速度・トルク制御 モーション処理 */
static	BOOL MtgVelctrl( MG_HNDL *hdl, MG_INOUT *inOut );
/* INTERPOLATEモーション処理 */
static	BOOL MtgInterpolate( MG_HNDL *hdl, MG_INOUT *inOut );
/* Posingモーション処理 */
static	BOOL MtgPosing( MG_HNDL *hdl, MG_INOUT *inOut );
/* Holdモーション処理 */
static	BOOL MtgHold( MG_HNDL *hdl, MG_INOUT *inOut );
/* 減速判別用データ演算処理 */
static	LONG mtgCheckDecelaration( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm );
/* 減速度で加減速判断 */
static	LONG mtgAccelerateDecRate( LONG lspd, LONG Tspd, MG_PRM *prm );
/* 加速度と減速度の比較選択 */
static	LONG mtgAccelerateMinRate( LONG lspd, LONG Tspd, MG_PRM *prm );
/* 減速処理 */
static	VOID mtgDecreaseSpd( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm );
/* 加速処理 */
static	LONG mtgAccelerateSpd( LONG spd, LONG tspd, MG_PRM *prm );
/* 減速距離計算 */
static	BOOL mtgCalcStpDistance( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm );


/* motion function table */
typedef BOOL (*MTGFUNC)(MG_HNDL *hdl, MG_INOUT *inOut);
const MTGFUNC mtgOutputMotion[] = {
	MtgNone,					/* NOPモーション */
	MtgInterpolate,				/* INTERPOLATE */
	MtgPosing,					/* POSING */ 
	MtgHold,					/* HOLD */
	MtgFollowup,				/* FOLLOW UP */
	MtgVelctrl,					/* VELCTRL */
	MtgVelctrl,					/* TRQCTRL */
};



/****************************************************************************
 初期化処理
****************************************************************************/
VOID MtgInit( MG_HNDL *hdl, MC_BASE_PRM *bPrm, MG_PRM *prm )
{
	hdl->cMinusDir = 0;		/* 現在払い出し方向 */
	hdl->tMinusDir = 0;		/* 現在の指令位置から目標位置の方向 */
	hdl->dpos0 = 0;			/* DPOSの少数部 for POSINGの指令速度（少数部）の積算 */
	hdl->dposRem = 0;		/* 伝送周期移動量(dpos)余り         */
	hdl->Cspd = 0;			/* 現在指令速度 */
	hdl->basePrm = bPrm;	/* 基本パラメータデータ参照先登録 */
	hdl->prm = prm;			/* パラメータデータ参照先登録 */
}


/****************************************************************************
 モーション生成メイン処理
****************************************************************************/
BOOL MtgGenerate(MG_HNDL* hdl, MG_INOUT* inOut)
{
	BOOL out_cmp;

	out_cmp = mtgOutputMotion[inOut->Mtype](hdl, inOut);

	return out_cmp;
}


/****************************************************************************
 減速距離取得
****************************************************************************/
LONG MtgGetDctDistance( MG_HNDL *hdl, LONG spd, LONG ctrl )
{
	MG_POSWK	wk;
	LONG		dist;
	MG_PRM		*prm;

	prm = hdl->prm;

	if( ctrl == MG_HOLD_CTRL2 )
	{
		prm = &hdl->basePrm->StopPrm;
	}

	wk.spd = MlibLABS( spd );
	mtgCalcStpDistance( &wk, prm, hdl->basePrm );		/* 減速距離計算 */

	dist = wk.sd_dist;                                  /* buf = sd_dist (整数部) */
	if( ((wk.sd_dist | wk.sd_dist0) != 0) && ((wk.sd_dist0 | hdl->dpos0) != 0) )
	{
		dist = dist + 1;								/* 減速距離小数部を繰り上げる */
	}

	if( hdl->cMinusDir )
	{
		return (-dist); 
	}
	return dist;
}


/****************************************************************************
 トルク/速度制御時 現在指令速度設定
****************************************************************************/
LONG MtgSetCspd( MG_HNDL *hdl, LONG spd )
{
	LONG maxSpd;

	maxSpd = hdl->basePrm->vel_max >> 1;

	if( spd < 0 )
	{
		hdl->cMinusDir = TRUE;
	}
	else
	{
		hdl->cMinusDir = FALSE;
	}

	spd = MlibLABS( spd );

	if( spd > maxSpd )
	{
		spd = maxSpd;
	}
	hdl->Cspd = spd << hdl->basePrm->vel_shift;

	return hdl->Cspd;
}


/****************************************************************************
 NOPモーション処理
****************************************************************************/
static	BOOL MtgNone( MG_HNDL *hdl, MG_INOUT *inOut )
{
	hdl->cMinusDir = 0;
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	hdl->Cspd = 0;
	inOut->DposOut = 0;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	return TRUE;
}


/****************************************************************************
 Follow up モーション処理
****************************************************************************/
static	BOOL MtgFollowup( MG_HNDL *hdl, MG_INOUT *inOut )
{
	hdl->cMinusDir = 0;
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	hdl->Cspd = 0;
	inOut->DposOut = inOut->DposIn;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	if( MlibLABS(inOut->DposIn) >= FOLLOWUP_MAX_REF )
	{
		return FALSE;
	}

	return TRUE;
}


/****************************************************************************
 速度・トルク制御 モーション処理
****************************************************************************/
static	BOOL MtgVelctrl( MG_HNDL *hdl, MG_INOUT *inOut )
{
	/*----------------------------------------------*/
	/* 				位置指令のクリア				*/
	/*----------------------------------------------*/
	hdl->dpos0     = 0;
	hdl->dposRem   = 0;
	inOut->DposOut = 0;

	/*----------------------------------------------*/
	/* 				速度指令作成					*/
	/*----------------------------------------------*/
	inOut->VrefOut = MlibMulgain( inOut->Vref, hdl->basePrm->maxToOsKx );

	/*----------------------------------------------*/
	/* 				トルク指令作成					*/
	/*----------------------------------------------*/
	inOut->TrefOut = inOut->Tref >> 6;

	if( hdl->Cspd != 0 )
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

/****************************************************************************
 INTERPOLATEモーション処理
****************************************************************************/
static	BOOL MtgInterpolate( MG_HNDL *hdl, MG_INOUT *inOut )
{
	LONG dpos, cSpd;
	BOOL comp;

	comp = FALSE;

	/* 払い出し完了チェック */
	if( inOut->DposIn == 0 )
	{
		hdl->dposRem = 0;
		comp = TRUE;
	}

	/* 払い出し量作成 */
	dpos = inOut->DposIn + hdl->dposRem;
	inOut->DposOut = dpos / hdl->basePrm->seg_num;
	hdl->dposRem = dpos % hdl->basePrm->seg_num;

	/* POSING速度設定 */
	cSpd = MtgSetCspd( hdl, inOut->DposOut );
	hdl->dpos0 = 0;

	inOut->Tspd = MlibMulgain32( cSpd, hdl->basePrm->PosingSpdInvKx );
	inOut->VrefOut = MlibMulgain( inOut->Vref, hdl->basePrm->refToOsKx );
	inOut->TrefOut = inOut->Tref >> 6;

	return comp;
}


/****************************************************************************
 Posingモーション処理
****************************************************************************/
static	BOOL MtgPosing( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_POSWK	wk;
	LONG		buf;
	LONG		tmp;
	BOOL		rc, check;

	check = TRUE;

	hdl->dposRem = 0;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	tmp = MlibMulgain32( inOut->Tspd, hdl->basePrm->PosingSpdKx );
	if( tmp >= 0x40000000L )
	{
		wk.tspd = 0x3FFFFFFF;
	}
	else
	{
		wk.tspd = (LONG)tmp;
	}

	wk.spd =  hdl->Cspd;				/* 指令速度をコピ－***.*[ref/cyc] */
	wk.lspd = wk.spd;					/* 指令速度を保管 */
	wk.bspd = wk.spd;					/* 指令速度(dpos)を保管 */
	wk.err_dist = inOut->DposIn;		/* 残距離計算（方向付き） */
	hdl->tMinusDir = FALSE;				/* +方向 */
	if( wk.err_dist < 0 )
	{ /* -移動方向 */
		hdl->tMinusDir = TRUE;			/* －方向 */
		wk.err_dist = -inOut->DposIn;	/* 残距離 */
	}

	/* POSING開始・終了チェック */
	if( wk.spd == 0 )
	{ /* 指令速度=0なら, POSING初期状態へリセット */
		hdl->cMinusDir = hdl->tMinusDir;
		hdl->dpos0 = 0;
		inOut->DposOut = 0;
		if( wk.err_dist == 0 )
		{
			return TRUE;				/* 払い出し完了 */
		}
	}

	/* 前回余り補正 */
	if( hdl->dpos0 != 0 )
	{
		wk.err_dist = wk.err_dist - 1;
	}
	wk.err_dist0 = ( 0 - hdl->dpos0 ) & VELREM_BIT_MSK;

	/* 減速距離計算 */
	rc = mtgCalcStpDistance( &wk, hdl->prm, hdl->basePrm );
	if( (hdl->tMinusDir != hdl->cMinusDir) || (rc != 0) )
	{ /* 目標位置が逆方向 or 減速距離オーバ → 減速 */
		wk.sd_rem = 0;											/* 余計な払い出しはしない */
		mtgDecreaseSpd( &wk, hdl->prm, hdl->basePrm );			/* 減速処理 */
		check = FALSE; 											/* POSING終了できない状態 */
	}
	else
	{
		/* buf = 残距離 - 減速距離 - 指令速度 */
		buf = mtgCheckDecelaration( &wk, hdl->prm, hdl->basePrm );
		if( buf < 0 )
		{ /* 残距離が減速距離未満の時，減速する */
			wk.sd_rem = buf + wk.spd;							/* 減速端数を計算 */
			mtgDecreaseSpd( &wk, hdl->prm, hdl->basePrm );		/* 減速処理 */
			if( wk.sd_rem < 0 )
			{
				/* 同方向で目標位置が手前→引き戻し */
				check = FALSE;
			}
		}
		else
		{ /* 残距離が減速距離以上の時 */
			if( wk.spd < wk.tspd )
			{ /* 現在速度 < 目標速度に到達していない */
				/* 加速処理 */
				wk.spd = mtgAccelerateSpd( wk.spd, wk.tspd, hdl->prm );

				/* 減速距離計算 */
				rc = mtgCalcStpDistance( &wk, hdl->prm, hdl->basePrm );
				/* buf = 残距離 - 減速距離 - 指令速度 */
				buf = mtgCheckDecelaration( &wk, hdl->prm, hdl->basePrm );
				if( (buf < 0) || (rc != 0) )
				{ /* 加速定数で加速できない */
					/* 減速定数で加速 */
					wk.spd = mtgAccelerateDecRate( wk.lspd, wk.tspd, hdl->prm );
					/* 減速距離計算 */
					rc = mtgCalcStpDistance( &wk, hdl->prm, hdl->basePrm );
					/* buf = 残距離 - 減速距離 - 指令速度 */
					buf = mtgCheckDecelaration( &wk, hdl->prm, hdl->basePrm );
					if( (buf < 0) || (rc != 0) )
					{/* 加速できない */
						if( wk.lspd != 0 )
						{ /* 前回速度は０か？ */
							/* 前回の速度にする */
							wk.bspd = wk.spd = wk.lspd;
						}
						else
						{ /* 前回速度=0なら数パルス送り */
							/* ミニマム定数で加速 */
							wk.spd = mtgAccelerateMinRate( wk.lspd, wk.tspd, hdl->prm );
							buf = wk.err_dist << hdl->basePrm->vel_shift;
							if( wk.spd >= buf )
							{
								/* 残距離を速度にする */
								wk.bspd = wk.spd = buf;
							}
							else
							{
								/* ミニマム加速度で加速した速度にする。 */
								wk.bspd = wk.spd;
							}
						}
					}
					else
					{
						wk.bspd = wk.spd;
						if( wk.spd == wk.lspd )
						{ /* 結局,加速できてない */
							/* bufは****.[ref] */
							wk.sd_rem = ( buf << hdl->basePrm->vel_shift );
							/* 減速端数を払い出す。 */
							if( wk.sd_rem > wk.lspd )
							{
								wk.bspd = wk.sd_rem;
							}
						}
					}
				}
				else
				{
					wk.bspd = wk.spd;
				}
			}
			else if( wk.spd > wk.tspd )
			{ /* 現在速度 > 目標速度を越えている（目標速度変更された）*/
				wk.sd_rem = 0;
				/* 減速処理 */
				mtgDecreaseSpd( &wk, hdl->prm, hdl->basePrm );
				if( wk.spd < wk.tspd )
				{
					/* 目標速度にクランプ */
					wk.spd = wk.tspd;
				}
				wk.bspd = wk.spd;								/* for dpos */
			}
			else
			{/* 現在速度 = 目標速度に到達している */
				wk.bspd = wk.spd;								/* for dpos */
			}
		}
	}
	hdl->Cspd = wk.spd;											/* 指令速度 */
	/* 少数部を積算 */
	buf = hdl->dpos0 + ( (wk.bspd << hdl->basePrm->vel_shift_s) & VELREM_BIT_MSK );
	hdl->dpos0 = buf & VELREM_BIT_MSK;
	if( buf > VELREM_BIT_MSK )
	{
		buf = ( wk.bspd >> hdl->basePrm->vel_shift ) + 1;
	}
	else
	{
		buf = ( wk.bspd >> hdl->basePrm->vel_shift );
	}

	if( hdl->cMinusDir )
	{
		inOut->DposOut = (~buf) + 1;							/* 符号拡張 */
	}
	else
	{
		inOut->DposOut = buf;
	}

	/* 終了処理 */
	if( (inOut->DposOut == inOut->DposIn) && (check == TRUE) )
	{
		hdl->Cspd = 0;											/* 指令速度 */
		return TRUE;
	}

	return FALSE;
}


/****************************************************************************
 Holdモーション処理
****************************************************************************/
static	BOOL MtgHold( MG_HNDL *hdl, MG_INOUT *inOut )
{
	MG_PRM *prm;
	BOOL	rc;

	prm = hdl->prm;

	if( inOut->Ctrl == MG_HOLD_CTRL2 )
	{
		hdl->prm = &hdl->basePrm->StopPrm;
	}

	rc = MtgPosing( hdl, inOut );

	hdl->prm = prm;

	return rc;
}


/****************************************************************************
 減速判別用データ演算処理
  概要:  残距離－減速距離－指令速度を求める。
         ﾏｲﾅｽとなる時小数部までを計算する。
         ﾌﾟﾗｽとなるときはそのまま（減速ﾁｪｯｸでは小数点以下不要）
         減速距離が巨大の時はﾐﾆﾏﾑ値(-2147483648)とする。
         -2147483648の移動方向を実現する為,残距離が-2147483648の時は
         強制的に214748367にする。
****************************************************************************/
static	LONG mtgCheckDecelaration( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm )
{
	LONG result;
	LONG buf0;

	result = wk->err_dist - wk->sd_dist - ( wk->spd >> bPrm->vel_shift );
	if( result > -bPrm->vel_max/*-1048576*/ )
	{ /* 減速距離が巨大(11ビットシフト時の最小データでチェック) */
		if( result <= 1 )
		{ /* err_distが巨大だと減速する必要なし、1より大きい時は、少数部を引いてもマイナスにならない。 */
			/* 小数部 */
			buf0 = (LONG)( wk->err_dist0 - wk->sd_dist0 ) 
					- ( (wk->spd << bPrm->vel_shift_s) & VELREM_BIT_MSK );
			/* buf(***.*) = err_dist - sd_dist - spd */
			result = ( result << bPrm->vel_shift) + (buf0 >> bPrm->vel_shift_s );
			if( result > 0 )
			{
				/* 結果プラスなら小数点以下不要 */
				result = result >> bPrm->vel_shift;
			}
		}
	}
	else
	{
		if( wk->err_dist == 0x80000000/*-2147483648*/ )
		{
		    /* これは-214748368の移動量を実現するため */
			result = 0x7fffffff /*2147483647*/;
		}
		else
		{
			result = 0x80000000 /*-2147483648*/;
		}
	}
	return result;
}



/****************************************************************************
  減速度で加減速判断
  概要:  現在の速度から減速定数で加速できるか判断し、速度指令を出力する。
         ＜加速できる条件＞
         減速定数＜加速定数なら加速できそう。
         目標速度未満ならＯＫ（目標速度は加速度でチェック済み）
         加速度変更速度未満ならＯＫ（加速度変更速度は加速度でチェック済み）
****************************************************************************/
static	LONG mtgAccelerateDecRate( LONG lspd, LONG Tspd, MG_PRM *prm )
{
	LONG spd;
	LONG rate;
	
	 /* 加速する減速を選択 */
	if( lspd >= prm->altspd_d )
	{
 		rate = prm->decrate2;
	}
	else
	{
		rate = prm->decrate1;
	}
	if( lspd >= prm->altspd_a )
	{ /* ２段目の時 */
		if( rate >= prm->accrate2 )
		{
			rate = 0;					/* 減速度＞＝加速度なら前回の速度にする */
		}
		spd = lspd + rate;
	}
	else
	{ /* １段目の時 */
		if( rate >= prm->accrate1 )
		{
			rate = 0;					/* 減速度＞＝加速度なら前回の速度にする */
		}
		spd = lspd + rate;
		if( spd >= prm->altspd_a )
		{
			spd = lspd;					/* 加速した結果加速度変更速度以上なら前回の速度にする */
		}
	}
	if( spd >= Tspd )
	{
		spd = lspd;						/* 加速した結果目標速度以上なら前回の速度にする */
	}

	return spd;
}



/****************************************************************************
  加速度と減速度の比較選択
  概要:  加速度と減速度を比較し,小さい方で加速する。
         目標速度以上なら目標速度
         加速度変更速度なら加速度変更速度
****************************************************************************/
static	LONG mtgAccelerateMinRate( LONG lspd, LONG Tspd, MG_PRM *prm )
{
	LONG spd;
	LONG rate;

	/* 加速する減速を選択 */
	if( lspd >= prm->altspd_d )
	{
		rate = prm->decrate2;
	}
	else
	{
		rate = prm->decrate1;
	}
	if( lspd >= prm->altspd_a )
	{ /* ２段目の時 */
		if( rate >= prm->accrate2 )
		{
			rate = prm->accrate2;		/* 減速度＞＝加速度なら加速度にする */
		}
		spd = lspd + rate;
	}
	else
	{ /* １段目の時 */
		if( rate >= prm->accrate1 )
		{
			rate = prm->accrate1;		/* 減速度＞＝加速度なら加速度にする */
		}
		spd = lspd + rate;
		if( spd >= prm->altspd_a )
		{
			spd = prm->altspd_a;		/* 加速した結果加速度変更速度以上なら加速度変更速度にする */
		}
	}
	if( spd >= Tspd )
	{
		spd = Tspd;						/* 加速した結果目標速度以上なら目標速度にする */
	}

	return spd;
}


/****************************************************************************
  減速処理
  現在の指令速度から減速させた指令速度を計算する。
  減速時端数を払い出したらクリアする。            
****************************************************************************/
static	VOID mtgDecreaseSpd( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm )
{
	LONG buf;          /* ワーク */
	LONG lspd;
	LONG mask;
	
	lspd = wk->spd;
	if( wk->spd > prm->altspd_d )
	{ /* ２段目減速処理 */
		/*速度端数ありなら速度端数で減速 */
		if( wk->spdrem2 != 0 )
		{
			buf = wk->spdrem2;
		}
		else
		{
			buf = prm->decrate2;
		}
		wk->spd -= buf;

		if( wk->spd < prm->altspd_d )
		{
			wk->spd = prm->altspd_d;	/* 減速度変更速度未満のとき */
		}

		if( wk->sd_rem > 0 )
		{ 
			if( wk->spd <= wk->sd_rem )
			{ /* 減速時端数を払い出せるか？ */
				wk->bspd = wk->sd_rem;	/* 減速時端数を払い出す。 */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				mask = ( 1 << bPrm->vel_shift ) - 1;
				/* 減速時端数の小数部を強制的に払い出し */
				wk->bspd = wk->spd + (wk->sd_rem & mask);
			}
		}
		else
		{ /* 減速時端数を払い出さないので２段目の速度端数を払い出すことになる */
			wk->bspd = wk->spd;
			wk->spdrem2 = 0;			/* 速度端数は減速開始のはじめだけ払い出す，以降はなし */
		}
	}
	else
	{ /* １段目減速処理 */
		/*速度端数ありなら速度端数で減速 */
		if( wk->spdrem1 != 0 )
		{
			buf = wk->spdrem1;
		}
		else
		{
			buf = prm->decrate1;
		}
		wk->spd -= buf;

		if( wk->spd < 0 )
		{
			wk->spd = 0;
		}

		if( wk->sd_rem > 0 )
		{ 
			if( wk->spd <= wk->sd_rem )
			{ /* 減速時端数を払い出せるか？ */
				wk->bspd = wk->sd_rem;	/* 減速時端数を払い出す。 */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				mask = ( 1 << bPrm->vel_shift ) - 1;
				/* 減速時端数の小数部を強制的に払い出し */
				wk->bspd = wk->spd + ( wk->sd_rem & mask );
			}
		}
		else
		{ /* 減速時端数を払い出さないので１段目の速度端数を払い出すことになる */
			wk->bspd = wk->spd;
			wk->spdrem1 = 0;			/* 速度端数は減速開始のはじめだけ払い出す，以降はなし */
		}
	}
}



/****************************************************************************
  加速処理
****************************************************************************/
static	LONG mtgAccelerateSpd( LONG spd, LONG tspd, MG_PRM *prm )
{
	if( spd < prm->altspd_a )
	{ /* １段目加速処理してみる */
		spd += prm->accrate1;
		if( spd > prm->altspd_a )
		{
			spd = prm->altspd_a;
		}
	}
	else
	{ /* ２段目加速処理してみる */
		spd += prm->accrate2;
	}

	if( spd > tspd )
	{
		spd = tspd;
	}

	return spd;
}



/****************************************************************************
  減速距離計算
****************************************************************************/
static	BOOL mtgCalcStpDistance( MG_POSWK *wk, MG_PRM *prm, MC_BASE_PRM *bPrm )
{
	ULONG cnt;					/* 減速回数 */
	LONG work3,spd;				/* 演算結果格納用ﾜｰｸﾃﾞｰﾀ */
	LONG work1[2] = {0};		/* 演算結果格納用ﾜ-ｸﾃﾞ-ﾀ */
	LONG work2[2] = {0};		/* 演算結果格納用ﾜ-ｸﾃﾞ-ﾀ */
	
	if( prm->altspd_d != 0 )
	{ /* 1, 2段目有効の場合 */
		spd = wk->spd;
		if( wk->spd > prm->altspd_d )
		{ /* 指令速度が減速度変更速度より大きい？ */
			/* ２段目減速距離計算 ------------------------------------ */
			/* 減速回数計算 */
			cnt = ( wk->spd - prm->altspd_d ) / prm->decrate2;
			/* 減速度２端数 */
			wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt ) - prm->altspd_d;
			if(wk->spdrem2 != 0)
			{
				cnt = cnt + 1;
			}
			/* work1 = altspd * 2 * cnt */
			MlibMul3232( prm->altspd_d*2, cnt , work1 );
			work3 = prm->decrate2 * (cnt - 1);
			/* work2 = rate * ((cnt*(cnt-1))) */
			MlibMul3232( work3, cnt , work2 );
			/* work2 = work2 + work1 */
			MlibAdd6464( work1, work2, work2);
			/* １段目部分の計算用 */
			spd = prm->altspd_d;
		}

		/* １段目減速距離計算 ------------------------------------ */
		/* 減速回数計算 */
		cnt = spd / prm->decrate1;
		/* 減速度１端数 */
		wk->spdrem1 = spd - ( prm->decrate1 * cnt );
		if( wk->spdrem1 != 0 )
		{
			cnt = cnt + 1;
		}
		work3 = prm->decrate1 * ( cnt-1 );

		/* work1 = rate * ((cnt*(cnt-1))) */
		MlibMul3232( work3, cnt , work1 );

		/* work2 = work2 + work1 ２段目＋１段目減速距離部結果 */
		MlibAdd6464( work1, work2, work2 );
		/* 減速距離(整数部)= (2段目+1段目減速距離部結果)/2 */
		wk->sd_dist  = ( (work2[1] << (31-bPrm->vel_shift)) | 
						((work2[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)) );
		/* 減速距離(小数部) */
		wk->sd_dist0 = ( work2[0] << (bPrm->vel_shift_s-1) ) & VELREM_BIT_MSK;

		/* sd_dist>0x40000000かチェック */
		work3 = work2[1] >> ( bPrm->vel_shift - 1 );
		if( work3 != 0 )
		{
			return TRUE;		/* 減速距離オーバフロー */
		}
	}
	else
	{ /* 2段目のみ有効の場合 */
		cnt = wk->spd / prm->decrate2;						/* 減速回数計算 */
		wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt );	/* 減速度１端数 */
		if(wk->spdrem2 != 0)
		{
			cnt = cnt + 1;
		}
		work3 = prm->decrate2*(cnt-1);

		/* work1 = rate * ((cnt*(cnt+1))) */
		MlibMul3232( work3, cnt , work1 );

		/* 減速距離(整数部)= (1段目減速距離部結果)/2 */
		wk->sd_dist = ( (work1[1] << (31-bPrm->vel_shift)) | 
						((work1[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)) );
		/* 減速距離(小数部)= (1段目減速距離部結果)/2 */
		wk->sd_dist0 = (work1[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;

		/* sd_dist>0x40000000かチェック */
		work3 = work1[1] >> ( bPrm->vel_shift - 1 );
		if( work3 != 0 )
		{
			return TRUE;		/* 減速距離オーバフロー */
		}
	}
	return FALSE;
}
