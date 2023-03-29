/****************************************************************************
Description	: MECHATROLINK Position Manager
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
#include "Position.h"
#include "MLib.h"



/* ソフトリミット状態チェック処理 */
static	VOID PsnCheckSoftLimit( PSN_HNDL *hdl, LONG *dpos );


/****************************************************************************
 初期化処理
****************************************************************************/
VOID PsnInit( PSN_HNDL *hdl, PSN_PRM *prm )
{
	hdl->Tpos = 0;			/* 目標位置 */
	hdl->Ipos = 0;			/* 指令位置（フィルタ前） */
	hdl->Pos = 0;			/* 指令位置（フィルタ後） */
	hdl->Mpos = 0;			/* FB位置入力タイミングでの指令位置 */
	hdl->Apos = 0;			/* FB位置 */
	hdl->Dpos = 0;			/* 今回出力移動距離 */
	hdl->Perr = 0;			/* 偏差 */
	hdl->Fbspd = 0;			/* FB速度 */
	hdl->Pgcnt = 0;			/* 目標位置 */
	hdl->SoftLimit = 0;		/* ソフトリミット状態 */
	hdl->PosOfs = 0;		/* 共通モニタ POS_OFST */
	hdl->InitPgPos64[0] = 0;/* 初期エンコーダ位置（64bit）*/
	hdl->InitPgPos64[1] = 0;/* 初期エンコーダ位置（64bit）*/
	hdl->RefEnable = FALSE;	/* リファレンス点無効 */
	hdl->PosEnable = FALSE;	/* 位置データ無効 */
	hdl->Prm = prm;
}


/****************************************************************************
 FBデータ作成処理
 Apos, Perr, Fbspd, Pset, Nearの更新
****************************************************************************/
VOID PsnUpdateFBData( PSN_HNDL *hdl, LONG pgcnt, LONG PosErr )
{
	LONG data;
	ULONG pos_err;

	/* FB位置、速度作成 */
	data = pgcnt - hdl->Pgcnt;
	hdl->Fbspd = data;					/* FB速度 */
	hdl->Apos += data;					/* FB位置 */
	hdl->Pgcnt = pgcnt;					/* PGCNT保存 */
#if 0
	/* MPOS更新 */
	if( hdl->Prm->TdlyIn <= 0 )
	{
		data = ( hdl->lstDpos * hdl->Prm->TdlyIn ) / hdl->Prm->SegNum;
		hdl->Mpos = ( hdl->Pos - hdl->Dpos ) + data;
	}
	else
	{
		data = ( hdl->Dpos * hdl->Prm->TdlyIn ) / hdl->Prm->SegNum;
		hdl->Mpos = ( hdl->Pos - hdl->Dpos ) - data;
	}
#endif
	/* PERR更新 */// <1209 I.M.暫定>
//	hdl->Perr = hdl->Mpos - hdl->Apos;
//	hdl->Mpos = hdl->Pos + (hdl->Dpos>>CFG_POSERR_DELAY_SHIFT);
	/* 2011.01.06 Y.Oka Perr作成処理をScanBへ移動 */
	hdl->Perr = PosErr;
	hdl->Mpos = hdl->Apos + hdl->Perr;

#if 0/* <S0AE> */
	/* Pset更新 */
	pos_err = MlibLABS( hdl->Tpos - hdl->Apos );
	hdl->Pset = FALSE;
	if( hdl->Den )
	{
		if( pos_err <= hdl->Prm->psetWidth )
		{ /* DEN=1 && (Mpos-Apos)<=位置決め完了幅 */
			hdl->Pset = TRUE;
		}
	}
	/* Near更新 */
	hdl->Near = FALSE;
	if( pos_err <= hdl->Prm->nearWidth )
	{ /* (Mpos-Apos)<=Near幅 */
		hdl->Near = TRUE;
	}
#endif/* <S0AE> */
}

/****************************************************************************
 PSET_NEAR更新処理
****************************************************************************//* <S0AE> */
VOID PsnSetPsetState( PSN_HNDL *hdl )
{
	ULONG pos_err;

	/* Pset更新 */
	pos_err = MlibLABS( hdl->Tpos - hdl->Apos );
	hdl->Pset = FALSE;
	if( hdl->Den )
	{
		if( pos_err <= hdl->Prm->psetWidth )
		{ /* DEN=1 && (Mpos-Apos)<=位置決め完了幅 */
			hdl->Pset = TRUE;
		}
	}
	/* Near更新 */
	hdl->Near = FALSE;
	if( pos_err <= hdl->Prm->nearWidth )
	{ /* (Mpos-Apos)<=Near幅 */
		hdl->Near = TRUE;
	}
}


/****************************************************************************
 TPOS設定処理(絶対値)
****************************************************************************/
LONG PsnSetTPOS( PSN_HNDL *hdl, LONG tpos )
{
	LONG dpos;

	dpos = tpos - hdl->Ipos;

	PsnCheckSoftLimit( hdl, &dpos );
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}


/****************************************************************************
 TPOS設定処理(相対距離)
****************************************************************************/
LONG PsnSetTPOSDist( PSN_HNDL *hdl, LONG dpos )
{
	PsnCheckSoftLimit( hdl, &dpos );
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}


/****************************************************************************
 速度・トルク制御時の位置合わせ処理
 TPOS=IPOS=POS=APOS+errにする
****************************************************************************/
VOID PsnResetCmdPosAll( PSN_HNDL *hdl, LONG err )
{
	LONG data;

	data = hdl->Apos + err;

	hdl->Ipos = data;
	hdl->Pos = data;
	if( hdl->PosEnable )
	{ /* 座標系有効時 */
		hdl->Tpos = data;
		hdl->Mpos = data;
		hdl->Perr = hdl->Mpos - hdl->Apos;
	}
	else
	{ /* 座標系無効時 */
		hdl->Tpos = hdl->Apos;
		hdl->Mpos = hdl->Apos;
		hdl->Perr = 0;
	}
}


/****************************************************************************
 ソフトリミット状態チェック処理
 1)リファレンス点無効時：SOTステータスクリア、TPOS=dpos+IPOSにし、リターン
 2)TPOSのソフトリミットチェック
   TPOSがソフトリミット以上ならソフトリミットでTPOSをクランプし、
   dpos にソフトリミットまでの距離(TPOS-IPOS)を返す
****************************************************************************/
static	VOID PsnCheckSoftLimit( PSN_HNDL *hdl, LONG *dpos )
{
	LONG dist, sdist;

	/* 指令リミットチェック */	
	if( hdl->Prm->tposLimitEnable && hdl->RefEnable )
	{
		dist = *dpos;
		if( dist >= 0 )
		{ /* 正側 */
			sdist = hdl->Prm->softLimit_P - hdl->Ipos;
			if( (dist >= sdist) && (hdl->Prm->sotEnableP == TRUE) )
			{ /* ソフトリミット有効 && リミット外 */
				/* リミット値に達してないなら、リミットまでの移動量をセット */
				/* リミット値に達してたら、移動量0をセット */
				if( sdist > 0 )
				{
					*dpos = sdist;
				}
				else
				{
					*dpos = 0;
				}
			}
		}
		else
		{ /* 負側 */
			sdist = hdl->Prm->softLimit_N - hdl->Ipos;
			if( (dist <= sdist) && (hdl->Prm->sotEnableN == TRUE) )
			{ /* ソフトリミット有効 && リミット外 */
				/* リミット値に達してないなら、リミットまでの移動量をセット */
				/* リミット値に達してたら、移動量0をセット */
				if( sdist < 0 )
				{
					*dpos = sdist;
				}
				else
				{
					*dpos = 0;
				}
			}
		}
	}
}


/****************************************************************************
 ソフトリミット状態チェック処理(APOS)
 1)リファレンス点無効時：SOTステータスクリアし、リターン
 2)APOSのソフトリミットチェック
   APOSがソフトリミット以上ならSOTステータスを更新し、SOT状態をリターン
****************************************************************************/
LONG PsnCheckSoftLimitFB( PSN_HNDL *hdl, BOOL *p_ot, BOOL *n_ot )
{
	LONG	sts;

	sts = 0;
	*p_ot = FALSE;
	*n_ot = FALSE;

	/* リファレンス点無効時、ステータスをクリアし、強制リターン */
	if( !hdl->RefEnable )
	{
		hdl->SoftLimit = 0;
		return 0;
	}
	/* FBリミットチェック */
	if( (hdl->Apos >= hdl->Prm->softLimit_P) && (hdl->Prm->sotEnableP == TRUE) )
	{ /* 正側ソフトリミット状態 */
		sts |= SOFT_LIMIT_P;
		*p_ot = TRUE;
	}
	if( (hdl->Apos <= hdl->Prm->softLimit_N) && (hdl->Prm->sotEnableN == TRUE) )
	{ /* 負側ソフトリミット状態 */
		sts |= SOFT_LIMIT_N;
		*n_ot = TRUE;
	}

	hdl->SoftLimit = sts;			/* SoftLimit状態更新 */
	return sts;
}


/****************************************************************************
 座標系設定
 POS基準で全ての位置をposにする
****************************************************************************/
VOID PsnSetCordinate( PSN_HNDL *hdl, LONG mode, LONG pos )
{
	LONG offset;

	/* POS_SETモード：POS基準で位置をリセット */
	offset = pos - hdl->Pos;
	hdl->Pos = pos;
	hdl->Mpos = pos;
	hdl->Ipos += offset;
	hdl->Apos += offset;
	hdl->Tpos += offset;
	hdl->PosOfs += offset;		/* 共通モニタ POS_OFST */
}


/****************************************************************************
 座標系リセット
 基準で全ての位置をPGCNT基準にリセットする
 また、abs=1&&ABS_PG時は、ABSOffsetを加算し、リファレンス点有効にする
 abs=0のときは、リファレンス点無効となる
****************************************************************************/
VOID PsnResetApos( PSN_HNDL *hdl, BOOL abs )
{
	LONG offset;
	LONG apos;

	if( abs == TRUE )
	{ /* ABS位置作成モード：原点位置オフセットを加算し、APOS基準で位置をリセット */
		apos = hdl->Pgcnt;
		if( hdl->Prm->absPG )
		{
			apos += hdl->Prm->absOfsset;
			hdl->RefEnable = TRUE;
		}
		else
		{ /* インクレモードで使う場合 */
			;
		}
		offset = apos - hdl->Apos;
		hdl->Apos = apos;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->PosEnable = TRUE;
		hdl->PosOfs = 0;
	}
	else
	{ /* APOSリセットモード：APOS基準で位置をリセット */
		offset = hdl->Pgcnt - hdl->Apos;
		hdl->Apos = hdl->Pgcnt;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->RefEnable = FALSE;
		hdl->PosEnable = FALSE;
		hdl->PosOfs = 0;
	}
}


/****************************************************************************
 LPOS取得
	ラッチ領域内ならば、Lposを更新し、TRUEを返す。
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
BOOL PsnMakeLpos( PSN_HNDL *hdl, LONG ltcnt, LONG i )
{
	LONG lpos;
	BOOL ltcmp;

	ltcmp = FALSE;

	lpos = hdl->Apos + ( ltcnt - hdl->Pgcnt );
	if( hdl->Prm->ltRangeP > hdl->Prm->ltRangeN )
	{ /* ラッチ上限値 > ラッチ下限値 の場合、
		 ラッチ領域は、ラッチ下限値 <= lpos <= ラッチ上限値 */
		if( (lpos >= hdl->Prm->ltRangeN) && (lpos <= hdl->Prm->ltRangeP) )
		{
			ltcmp = TRUE;
			hdl->LstLpos[i] = hdl->Lpos[i];
			hdl->Lpos[i] = lpos;
		}
	}
	else
	{ /* ラッチ上限値 <= ラッチ下限値 の場合
		 ラッチ領域は、lpos <= ラッチ下限値か、又は、 ラッチ上限値 <= lpos */
		if( (lpos <= hdl->Prm->ltRangeP) || (lpos >= hdl->Prm->ltRangeN) )
		{
			ltcmp = TRUE;
			hdl->LstLpos[i] = hdl->Lpos[i];
			hdl->Lpos[i] = lpos;
		}
	}
	return ltcmp;
}



