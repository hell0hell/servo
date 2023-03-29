/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Generator 													*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/* MtgInit()			:Motion Generator初期化								*/
/* MtgAttachPRM()		:Motionパラメータデータ登録							*/
/* MtgGetDctDistance()	:減速距離取得										*/
/* MtgSetCspd()			:現在指令速度設定(速度・トルク制御時用)				*/
/*																			*/
/* MtgNone()			:モーションニュートラル(NOP)処理					*/
/* MtgFollowup()		:フォローアップモーション							*/
/* MtgVelctrl()			:速度・トルク制御モーション							*/
/* MtgInterpolate()		:補完送りモーション									*/
/* MtgPosing()			:位置決めモーション									*/
/* MtgHold()			:減速停止モーション									*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

/* includes */
#include "Motion.h"
//#include "Basedef.h"
//#include "ServoIf.h"
//#include "Motion.h"
//#include "MtPRM.h"
#include "MtGenerator.h"
//#include "MlibSvc.h"
//#include "UtilityLib.h" /* <V715> */

/* defines */
#define VELREM_BIT_MSK	((1<<MG_VREMBIT_MAX)-1)
#define VELREM_CARRYUP	(1<<MG_VREMBIT_MAX)


typedef struct MG_POSWK_ {
	LONG	err_dist;	/* 残距離（指令位置から目標位置までの距離） ****.[ref] */
	ULONG	err_dist0;	/* 残距離（小数部）（指令位置から目標位置までの距離） .**[ref] */
	ULONG	sd_dist0;	/* 減速距離（小数部）（現在指令速度から停止するまでの減速距離） .**[ref] */
	LONG	sd_dist;	/* 減速距離（整数部）（現在指令速度から停止するまでの減速距離） ****[ref] */
	LONG	sd_rem;		/* 減速時端数（減速距離と残距離の差で発生する払い出し端数） ***.*[ref/cyc] */
	LONG	spdrem1;	/* １段目減速時の速度端数払い出し ***.*[ref/cyc] */
	LONG	spdrem2;	/* ２段目減速時の速度端数払い出し ***.*[ref/cyc] */
	LONG	spd;		/* 現在指令速度 ***.*[ref/cyc] */
	LONG	tspd;		/* 目標速度 ***.*[ref/cyc] */
	LONG	lspd;		/* 前回指令速度 ***.*[ref/cyc] */
	LONG	bspd;		/* 指令出力速度(dpos) ***.*[ref/cyc] */
} MG_POSWK;


/* forward declarations */
/* Public functions */
VOID MtgInit(MGHNDL* hdl, MC_BASE_PRM* bPrm, MG_PRM* prm);
VOID MtgAttachPRM(MGHNDL* hdl, MG_PRM* prm);
BOOL MtgGenerate(MGHNDL* hdl, MG_INOUT* inOut);
LONG MtgGetDctDistance(MGHNDL* hdl, LONG spd, USHORT ctrl);
LONG MtgSetCspd(MGHNDL* hdl, LONG spd);
LONG MtgAccelerateSpd(LONG spd, LONG tspd, MGHNDL* hdl);

/* Public Motion functions */
BOOL MtgNone(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgFollowup(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgVelctrl(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgInterpolate(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgPosing(MGHNDL* hdl, MG_INOUT* inOut);
BOOL MtgHold(MGHNDL* hdl, MG_INOUT* inOut);

/* Private sub functions */
LONG mtgCheckDecelaration(MG_POSWK*	wk, MG_PRM* prm, MC_BASE_PRM* bPrm);
LONG mtgAccelerateDecRate(LONG lspd, LONG Tspd, MG_PRM* prm);
LONG mtgAccelerateMinRate(LONG lspd, LONG Tspd, MG_PRM* prm);
VOID mtgDecreaseSpd(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm);
LONG mtgAccelerateSpd(LONG spd, LONG tspd, MG_PRM* prm);
BOOL mtgCalcStpDistance(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm);


/* motion function table */
typedef BOOL (*MTGFUNC)(MGHNDL* hdl, MG_INOUT* inOut);
//const MTGFUNC _mtgOutputMotion[] = {	/* <V715> */
#if 0
const MTGFUNC mtgOutputMotion[] = {	/* <V715> */
	MtgNone,		/* NOPモーション */
	MtgInterpolate,	/* INTERPOLATE */
	MtgPosing,		/* POSING */ 
	MtgHold,		/* HOLD */
	MtgFollowup,	/* FOLLOW UP */
	MtgVelctrl,		/* VELCTRL */
	MtgVelctrl,		/* TRQCTRL */
};
#endif

/* variable <V715> */
#ifdef	__ghs__
#pragma ghs startzda
#endif
//MTGFUNC mtgOutputMotion[sizeof(_mtgOutputMotion)/sizeof(MTGFUNC)];
#ifdef	__ghs__
#pragma ghs endzda
#endif

/****************************************************************************
 初期化処理
****************************************************************************/
#pragma arm section code = "IXX_EXCODE"
VOID MtgInit(MGHNDL* hdl, MC_BASE_PRM* bPrm, MG_PRM* prm)
{
	hdl->cMinusDir = 0;	/* 現在払い出し方向 */
	hdl->tMinusDir = 0;	/* 現在の指令位置から目標位置の方向 */
	hdl->dpos0 = 0;		/* DPOSの少数部 for POSINGの指令速度（少数部）の積算 */
	hdl->dposRem = 0;	/* 伝送周期移動量(dpos)余り         */
	hdl->Cspd = 0;		/* 現在指令速度 */
	hdl->basePrm = bPrm;/* 基本パラメータデータ参照先登録 */
	hdl->prm = prm;		/* パラメータデータ参照先登録 */

	/* ConstテーブルをデータRAMへ展開 *//* <V715> */
	//UtilMemCpy((MTGFUNC *)mtgOutputMotion, (MTGFUNC *)_mtgOutputMotion, sizeof(mtgOutputMotion));
	//MlibCopyByteMemory ( (UCHAR *)_mtgOutputMotion, (UCHAR *)mtgOutputMotion, sizeof(mtgOutputMotion) );
}
#pragma arm section code

/****************************************************************************
 パラメータデータをMGのパラメータに登録する
****************************************************************************/
VOID MtgAttachPRM(MGHNDL* hdl, MG_PRM* prm)
{
	hdl->prm = prm;		/* パラメータデータ参照先登録 */
}



/****************************************************************************
 モーション生成メイン処理
****************************************************************************/
BOOL MtgGenerate(MGHNDL* hdl, MG_INOUT* inOut)
{
	BOOL out_cmp;

	//out_cmp = mtgOutputMotion[inOut->Mtype](hdl, inOut);
	switch( inOut->Mtype )
	{
	case 0: out_cmp = MtgNone( hdl, inOut ); break;
	case 1: out_cmp = MtgInterpolate( hdl, inOut ); break;
	case 2: out_cmp = MtgPosing( hdl, inOut ); break;
	case 3: out_cmp = MtgHold( hdl, inOut ); break;
	case 4: out_cmp = MtgFollowup( hdl, inOut ); break;
	case 5: out_cmp = MtgVelctrl( hdl, inOut ); break;
	case 6: out_cmp = MtgVelctrl( hdl, inOut ); break;
	default:
		out_cmp = FALSE; break;
	}


	return out_cmp;
}


/****************************************************************************
 減速距離取得
****************************************************************************/
LONG MtgGetDctDistance(MGHNDL* hdl, LONG spd, USHORT ctrl)
{
	MG_POSWK	wk;
	LONG		dist;
	MG_PRM*		prm = hdl->prm;

	if(ctrl == MG_HOLD_CTRL2)
		prm = &hdl->basePrm->StopPrm;

	wk.spd = MlibABS(spd);
	mtgCalcStpDistance(&wk, prm, hdl->basePrm);	/* 減速距離計算 */

	dist = wk.sd_dist;                                  /* buf = sd_dist (整数部) */
	if(((wk.sd_dist | wk.sd_dist0) != 0) && ((wk.sd_dist0 | hdl->dpos0) != 0))
		dist = dist + 1;	/* 減速距離小数部を繰り上げる */

	if(hdl->cMinusDir) return (-dist); 
	return dist;
}

/****************************************************************************
 トルク/速度制御時 現在指令速度設定
****************************************************************************/
LONG MtgSetCspd(MGHNDL* hdl, LONG spd)
{
	LONG maxSpd = hdl->basePrm->vel_max >> 1;

	hdl->cMinusDir = (spd < 0) ? TRUE: FALSE;
	spd = MlibABS(spd);
	if(spd > maxSpd) spd = maxSpd;
	hdl->Cspd = spd << hdl->basePrm->vel_shift;

	return hdl->Cspd;
}


/****************************************************************************
 NOPモーション処理
****************************************************************************/
BOOL MtgNone(MGHNDL* hdl, MG_INOUT* inOut)
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
BOOL MtgFollowup(MGHNDL* hdl, MG_INOUT* inOut)
{
	hdl->cMinusDir = 0;
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	hdl->Cspd = 0;
	inOut->DposOut = inOut->DposIn;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;

	if(MlibABS(inOut->DposIn) >= FOLLOWUP_MAX_REF)
		return FALSE;

	return TRUE;
}

/****************************************************************************
 速度・トルク制御 モーション処理
****************************************************************************/
BOOL MtgVelctrl(MGHNDL* hdl, MG_INOUT* inOut)
{
	hdl->dpos0 = 0;
	hdl->dposRem = 0;
	inOut->DposOut = 0;
	inOut->VrefOut = inOut->Vref /* TODO: MlibMulgain(inOut->Vref, hdl->basePrm->maxToOsKx) */;
	inOut->TrefOut = inOut->Tref /* >> 6 */;
	if(hdl->Cspd != 0) return FALSE;
	return TRUE;
}


/****************************************************************************
 INTERPOLATEモーション処理
****************************************************************************/
BOOL MtgInterpolate(MGHNDL* hdl, MG_INOUT* inOut)
{
	LONG dpos, cSpd;
	BOOL comp = FALSE;

/* 払い出し完了チェック */
	if(inOut->DposIn == 0)
	{
		hdl->dposRem = 0;
		comp = TRUE;
	}
/* 払い出し量作成 */
	dpos = inOut->DposIn + hdl->dposRem;
	inOut->DposOut = dpos / hdl->basePrm->seg_num;
	hdl->dposRem = dpos % hdl->basePrm->seg_num;
/* POSING速度設定 */
	cSpd = MtgSetCspd(hdl, inOut->DposOut);
	hdl->dpos0 = 0;

	//inOut->Tspd = MlibMulgain32(cSpd, hdl->basePrm->PosingSpdInvKx);		// 暫定
	inOut->Tspd = cSpd;														// 暫定
	inOut->VrefOut = inOut->Vref /* @@ MlibMulgain(inOut->Vref, hdl->basePrm->refToOsKx) */;
	inOut->TrefOut = inOut->Tref /* @@ >> 6 */;

	return comp;
}

/****************************************************************************
 Posingモーション処理
****************************************************************************/
BOOL MtgPosing(MGHNDL* hdl, MG_INOUT* inOut)
{
	MG_POSWK	wk;
	LONG		buf;
	BOOL		rc;
	BOOL		check = TRUE;

	hdl->dposRem = 0;
	inOut->VrefOut = 0;
	inOut->TrefOut = 0;
/* POSING初期設定 */
	//wk.tspd = MlibMulgain32(inOut->Tspd, hdl->basePrm->PosingSpdKx);		// 暫定
	wk.tspd = inOut->Tspd;													// 暫定
	if(wk.tspd >= 0x40000000) {wk.tspd = 0x3FFFFFFF;}
	wk.spd =  hdl->Cspd;				/* 指令速度をコピ－***.*[ref/cyc] */
	wk.lspd = wk.spd;					/* 指令速度を保管 */
	wk.bspd = wk.spd;					/* 指令速度(dpos)を保管 */
	wk.err_dist = inOut->DposIn;		/* 残距離計算（方向付き） */
	hdl->tMinusDir = FALSE;				/* +方向 */
	if(wk.err_dist < 0)
	{/* -移動方向 */
		hdl->tMinusDir = TRUE;			/* －方向 */
		wk.err_dist = -inOut->DposIn;	/* 残距離 */
	}
/* POSING開始・終了チェック */
	if(wk.spd == 0)
	{/* 指令速度=0なら, POSING初期状態へリセット */
		hdl->cMinusDir = hdl->tMinusDir;
		hdl->dpos0 = 0;
		inOut->DposOut = 0;
		if(wk.err_dist == 0)	return TRUE;	/* 払い出し完了 */
	}
/* 前回余り補正 */
	if(hdl->dpos0 != 0) 
		wk.err_dist = wk.err_dist - 1;
	wk.err_dist0 = (0 - hdl->dpos0) & VELREM_BIT_MSK;

/* 減速距離計算 */
	rc = mtgCalcStpDistance(&wk, hdl->prm, hdl->basePrm);	/* 減速距離計算 */
	if((hdl->tMinusDir != hdl->cMinusDir) || (rc != 0))
	{/* 目標位置が逆方向 or 減速距離オーバ → 減速 */
		wk.sd_rem = 0;									/* 余計な払い出しはしない */
		mtgDecreaseSpd(&wk, hdl->prm, hdl->basePrm);	/* 減速処理 */
		check = FALSE; 									/* POSING終了できない状態 */
	}
	else
	{
		buf = mtgCheckDecelaration(&wk, hdl->prm, hdl->basePrm);	/* buf = 残距離 - 減速距離 - 指令速度 */
		if(buf < 0)
		{/* 残距離が減速距離未満の時，減速する */
			wk.sd_rem = buf + wk.spd;							/* 減速端数を計算 */
			mtgDecreaseSpd(&wk, hdl->prm, hdl->basePrm);		/* 減速処理 */
			if(wk.sd_rem < 0)	check = FALSE;					/* 同方向で目標位置が手前→引き戻し */
		}
		else
		{/* 残距離が減速距離以上の時 */
			if(wk.spd < wk.tspd)
			{/* 現在速度 < 目標速度に到達していない */
				wk.spd = mtgAccelerateSpd(wk.spd, wk.tspd, hdl->prm);	/* 加速処理 */

				rc = mtgCalcStpDistance(&wk, hdl->prm, hdl->basePrm);	/* 減速距離計算 */
				buf = mtgCheckDecelaration(&wk, hdl->prm, hdl->basePrm);/* buf = 残距離 - 減速距離 - 指令速度 */
				if((buf < 0) || (rc != 0))
				{/* 加速定数で加速できない */
					wk.spd = mtgAccelerateDecRate(wk.lspd, wk.tspd, hdl->prm);	/* 減速定数で加速 */
					rc = mtgCalcStpDistance(&wk, hdl->prm, hdl->basePrm);		/* 減速距離計算 */
					buf = mtgCheckDecelaration(&wk, hdl->prm, hdl->basePrm);	/* buf = 残距離 - 減速距離 - 指令速度 */
					if((buf < 0) || (rc != 0))
					{/* 加速できない */
						if(wk.lspd != 0)
						{/* 前回速度は０か？ */
							wk.bspd = wk.spd = wk.lspd;	/* 前回の速度にする */
						}
						else
						{/* 前回速度=0なら数パルス送り */
							wk.spd = mtgAccelerateMinRate(wk.lspd, wk.tspd, hdl->prm);	/* ﾐﾆﾏﾑ定数で加速 */
							buf = wk.err_dist << hdl->basePrm->vel_shift;
							if(wk.spd >= buf)	wk.bspd = wk.spd = buf;	/* 残距離を速度にする */
							else				wk.bspd = wk.spd;		/* ﾐﾆﾏﾑ加速度で加速した速度にする。 */
						}
					}
					else
					{
						wk.bspd = wk.spd;
						if( wk.spd == wk.lspd )
						{/* 結局,加速できてない */
							wk.sd_rem = (buf << hdl->basePrm->vel_shift);	/* bufは****.[ref] */
							if(wk.sd_rem > wk.lspd)  wk.bspd = wk.sd_rem;	/* 減速端数を払い出す。 */
						}
					}
				}
				else
				{
					wk.bspd = wk.spd;
				}
			}
			else if(wk.spd > wk.tspd)
			{/* 現在速度 > 目標速度を越えている（目標速度変更された）*/
				wk.sd_rem = 0;
				mtgDecreaseSpd(&wk, hdl->prm, hdl->basePrm);	/* 減速処理 */
				if(wk.spd < wk.tspd) wk.spd = wk.tspd;			/* 目標速度にクランプ */
				wk.bspd = wk.spd;								/* for dpos */
			}
			else
			{/* 現在速度 = 目標速度に到達している */
				wk.bspd = wk.spd;		/* for dpos */
			}
		}
	}
	hdl->Cspd = wk.spd;		/* 指令速度 */
	buf = hdl->dpos0 + ((wk.bspd << hdl->basePrm->vel_shift_s) & VELREM_BIT_MSK);	/* 少数部を積算 */
	hdl->dpos0 = buf & VELREM_BIT_MSK;
	if(buf > VELREM_BIT_MSK){ buf = (wk.bspd >> hdl->basePrm->vel_shift) + 1;}
	else					{ buf = (wk.bspd >> hdl->basePrm->vel_shift);}
	if(hdl->cMinusDir)	{ inOut->DposOut = (~buf) + 1;}	/* 符号拡張 */
	else				{ inOut->DposOut = buf;}

/* 終了処理 */
	if((inOut->DposOut == inOut->DposIn) && (check == TRUE))
	{
		hdl->Cspd = 0;		/* 指令速度 */
		return TRUE;
	}

	return FALSE;
}

/****************************************************************************
 Holdモーション処理
****************************************************************************/
BOOL MtgHold(MGHNDL* hdl, MG_INOUT* inOut)
{
	MG_PRM*	prm = hdl->prm;
	BOOL	rc;

	if(inOut->Ctrl == MG_HOLD_CTRL2)
		hdl->prm = &hdl->basePrm->StopPrm;

	rc = MtgPosing(hdl, inOut);

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
LONG mtgCheckDecelaration(MG_POSWK*	wk, MG_PRM* prm, MC_BASE_PRM* bPrm)
{
	LONG result;
	LONG buf0;

	result = wk->err_dist - wk->sd_dist - (wk->spd >> bPrm->vel_shift);
	if( result > -bPrm->vel_max/*-1048576*/)
	{/* 減速距離が巨大(11ﾋﾞｯﾄｼﾌﾄ時の最小データでﾁｪｯｸ) */
		if(result <= 1)
		{/* err_distが巨大だと減速する必要なし、1より大きい時は、少数部を引いてもマイナスにならない。 */
			buf0 = (LONG)(wk->err_dist0 - wk->sd_dist0) 
					- ((wk->spd << bPrm->vel_shift_s) & VELREM_BIT_MSK);  /* 小数部 */
			result = (result << bPrm->vel_shift) + (buf0 >> bPrm->vel_shift_s); /* buf(***.*) = err_dist - sd_dist - spd */
			if(result > 0)
				result = result >> bPrm->vel_shift;  /* 結果ﾌﾟﾗｽなら小数点以下不要 */
		}
	}
	else
	{
		if(wk->err_dist == 0x80000000/*-2147483648*/)
			result = 0x7fffffff/*2147483647*/;    /* これは-214748368の移動量を実現するため */
		else
			result = 0x80000000/*-2147483648*/;
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
LONG mtgAccelerateDecRate(LONG lspd, LONG Tspd, MG_PRM* prm)
{
	LONG spd;
	LONG rate;
	
 /* 加速する減速を選択 */
 	rate = ((lspd >= prm->altspd_d) ? prm->decrate2: prm->decrate1);
	if(lspd >= prm->altspd_a )
	{/* ２段目の時 */
		if(rate >= prm->accrate2) rate = 0;        /* 減速度＞＝加速度なら前回の速度にする */
		spd = lspd + rate;
	}
	else
	{/* １段目の時 */
		if(rate >= prm->accrate1)	rate = 0;		/* 減速度＞＝加速度なら前回の速度にする */
		spd = lspd + rate;
		if(spd >= prm->altspd_a)	spd = lspd;	/* 加速した結果加速度変更速度以上なら前回の速度にする */
	}
	if(spd >= Tspd) spd = lspd;           /* 加速した結果目標速度以上なら前回の速度にする */

	return spd;
}

/****************************************************************************
  加速度と減速度の比較選択
  概要:  加速度と減速度を比較し,小さい方で加速する。
         目標速度以上なら目標速度
         加速度変更速度なら加速度変更速度
****************************************************************************/
LONG mtgAccelerateMinRate(LONG lspd, LONG Tspd, MG_PRM* prm)
{
	LONG spd;
	LONG rate;
	
 /* 加速する減速を選択 */
	rate = ((lspd >= prm->altspd_d) ? prm->decrate2: prm->decrate1);
	if(lspd >= prm->altspd_a )
	{/* ２段目の時 */
		if(rate >= prm->accrate2) rate = prm->accrate2;        /* 減速度＞＝加速度なら加速度にする */
		spd = lspd + rate;
	}
	else
	{/* １段目の時 */
		if(rate >= prm->accrate1) rate = prm->accrate1;   /* 減速度＞＝加速度なら加速度にする */
		spd = lspd + rate;
		if(spd >= prm->altspd_a)  spd = prm->altspd_a;      /* 加速した結果加速度変更速度以上なら加速度変更速度にする */
	}
	if(spd >= Tspd) spd = Tspd;           /* 加速した結果目標速度以上なら目標速度にする */

	return spd;
}

/****************************************************************************
  減速処理
  現在の指令速度から減速させた指令速度を計算する。
  減速時端数を払い出したらクリアする。            
****************************************************************************/
VOID mtgDecreaseSpd(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm)
{
	LONG buf;          /* ワーク */
	LONG lspd;
	
	lspd = wk->spd;
	if( wk->spd > prm->altspd_d )
	{/* ２段目減速処理 */
		buf = ((wk->spdrem2 != 0) ? wk->spdrem2: prm->decrate2);	/*速度端数ありなら速度端数で減速 */
		wk->spd -= buf;

		if(wk->spd < prm->altspd_d)   wk->spd = prm->altspd_d;	/* 減速度変更速度未満のとき */
		
		if(wk->sd_rem > 0)
		{ 
			if(wk->spd <= wk->sd_rem)
			{/* 減速時端数を払い出せるか？ */
				wk->bspd = wk->sd_rem;	/* 減速時端数を払い出す。 */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				LONG mask = (1 << bPrm->vel_shift) - 1;
				wk->bspd = wk->spd + (wk->sd_rem & mask);	/* 減速時端数の小数部を強制的に払い出し */
			}
		}
		else
		{/* 減速時端数を払い出さないので２段目の速度端数を払い出すことになる */
			wk->bspd = wk->spd;
			wk->spdrem2 = 0;	/* 速度端数は減速開始のはじめだけ払い出す，以降はなし */
		}
	}
	else
	{/* １段目減速処理 */
		buf = ((wk->spdrem1 != 0) ? wk->spdrem1: prm->decrate1);	/*速度端数ありなら速度端数で減速 */
		wk->spd -= buf;

		if(wk->spd < 0)	wk->spd = 0;

		if(wk->sd_rem > 0)
		{ 
			if(wk->spd <= wk->sd_rem)
			{/* 減速時端数を払い出せるか？ */
				wk->bspd = wk->sd_rem;	/* 減速時端数を払い出す。 */
				wk->spd = lspd;
				wk->sd_rem = 0;
			}
			else
			{
				LONG mask = (1 << bPrm->vel_shift) - 1;
				wk->bspd = wk->spd + (wk->sd_rem & mask);	/* 減速時端数の小数部を強制的に払い出し */
			}
		}
		else
		{/* 減速時端数を払い出さないので２段目の速度端数を払い出すことになる */
			wk->bspd = wk->spd;
			wk->spdrem1 = 0;	/* 速度端数は減速開始のはじめだけ払い出す，以降はなし */
		}
	}
}


/****************************************************************************
  加速処理
****************************************************************************/
LONG mtgAccelerateSpd(LONG spd, LONG tspd, MG_PRM* prm)
{
	if( spd < prm->altspd_a )
	{   /* １段目加速処理してみる */
		spd += prm->accrate1;
		if(spd > prm->altspd_a)
			spd = prm->altspd_a;
	}
	else
	{/* ２段目加速処理してみる */
		spd += prm->accrate2;
	}

	if(spd > tspd) spd = tspd;

	return spd;
}


/****************************************************************************
  減速距離計算
****************************************************************************/
BOOL mtgCalcStpDistance(MG_POSWK* wk, MG_PRM* prm, MC_BASE_PRM* bPrm)
{
	ULONG cnt;       /* 減速回数 */
	LONG work1[2] = {0};            /* 演算結果格納用ﾜ-ｸﾃﾞ-ﾀ */
	//LONG work2[2] = {0};    /* 演算結果格納用ﾜ-ｸﾃﾞ-ﾀ */
	LONG work3 /*,spd */;               /* 演算結果格納用ﾜｰｸﾃﾞｰﾀ */
	
#ifndef POSING_1RATEONLY
	if(prm->altspd_d != 0)
	{/* 1,2段目有効の場合 */
		spd = wk->spd;
		if( wk->spd > prm->altspd_d )
		{/* 指令速度が減速度変更速度より大きい？ */
		/* ２段目減速距離計算 ------------------------------------ */
			cnt = ( wk->spd - prm->altspd_d ) / prm->decrate2;					/* 減速回数計算 */
			wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt ) - prm->altspd_d;	/* 減速度２端数 */
			if(wk->spdrem2 != 0)   cnt = cnt + 1;
			MlibMul3232( prm->altspd_d*2, cnt , work1 );			/* work1 = altspd * 2 * cnt */
			work3  = prm->decrate2*(cnt-1);
			MlibMul3232( work3, cnt , work2 );						/* work2 = rate * ((cnt*(cnt-1))) */
			MlibAdd6464( work1, work2, work2);						/* work2 = work2 + work1 */
			spd = prm->altspd_d;							/* １段目部分の計算用 */
		}
		/* １段目減速距離計算 ------------------------------------ */
		cnt = spd / prm->decrate1;						/* 減速回数計算 */
		wk->spdrem1 = spd - ( prm->decrate1 * cnt );	/* 減速度１端数 */
		if(wk->spdrem1 != 0)   cnt = cnt + 1;
		work3 = prm->decrate1*(cnt-1);
		MlibMul3232( work3, cnt , work1 );		/* work1 = rate * ((cnt*(cnt+1))) */

		MlibAdd6464( work1, work2, work2);		/* work2 = work2 + work1 ２段目＋１段目減速距離部結果 */
		wk->sd_dist  = ((work2[1] << (31-bPrm->vel_shift))		/* 減速距離(整数部)= (2段目+1段目減速距離部結果)/2 */
					| ((work2[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)));
		wk->sd_dist0 = (work2[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;/* 減速距離(小数部) */

		work3 = work2[1] >> (bPrm->vel_shift-1);	/* sd_dist>0x40000000かチェック */
		if(work3 != 0)	return TRUE;		/* 減速距離オーバフロー */
	}
	else
#endif
	{/* 2段目のみ有効の場合 */
		cnt = wk->spd / prm->decrate2;						/* 減速回数計算 */
		wk->spdrem2 = wk->spd - ( prm->decrate2 * cnt );	/* 減速度１端数 */
		if(wk->spdrem2 != 0)   cnt = cnt + 1;
		work3 = prm->decrate2*(cnt-1);
		MlibMul3232( work3, cnt , work1 );		/* work1 = rate * ((cnt*(cnt+1))) */

		wk->sd_dist  = ((work1[1] << (31-bPrm->vel_shift))  		/* 減速距離(整数部)= (1段目減速距離部結果)/2 */
					| ((work1[0] >> (bPrm->vel_shift+1)) & (bPrm->vel_max-1)));
		wk->sd_dist0 = (work1[0] << (bPrm->vel_shift_s-1)) & VELREM_BIT_MSK;/* 減速距離(小数部)= (1段目減速距離部結果)/2 */

		work3 = work1[1] >> (bPrm->vel_shift-1);	/* sd_dist>0x40000000かチェック */
		if(work3 != 0)	return TRUE;		/* 減速距離オーバフロー */
	}
	return FALSE;
}
/****************************************************************************
  加速処理
****************************************************************************/
LONG MtgAccelerateSpd(LONG spd, LONG tspd, MGHNDL* hdl)
{
LONG speed;

	speed = mtgAccelerateSpd(spd, tspd, hdl->prm);			/* 加速処理 */

	return speed;
}
