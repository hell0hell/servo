/****************************************************************************/
/*																			*/
/*																			*/
/*		POSITION MANAGER 													*/
/*																			*/
/*																			*/
/****************************************************************************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/************** Copyright Yaskawa Electric Corporation **********************/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/

/* includes */
#include "ScanN.h"
//#include "Basedef.h"
//#include "MlibSvc.h"
//#include "ServoIf.h"
//#include "Motion.h"
#include "MtPRM.h"
#include "Position.h"


/* forward declarations */
/* Public functions */
VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm);
VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt);
BOOL PsnGetPsetSts(PSNHNDL* hdl);
BOOL PsnGetNearSts(PSNHNDL* hdl);
BOOL PsnGetZPointSts(PSNHNDL* hdl);
LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos);
LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos);
VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err);
UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos);
UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot);
VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos);
VOID PsnResetApos(PSNHNDL* hdl, BOOL abs);
BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt);


/****************************************************************************
 初期化処理
****************************************************************************/
#pragma arm section code = "IXX_EXCODE"
VOID PsnInit(PSNHNDL* hdl, PSN_PRM* prm)
{
	hdl->Tpos = 0;			/* 目標位置 */
	hdl->Ipos = 0;			/* 指令位置（フィルタ前） */
	hdl->Pos = 0;			/* 指令位置（フィルタ後） */
	hdl->Mpos = 0;			/* 指令位置（フィルタ後） */
	hdl->Apos = 0;			/* FB位置 */
	hdl->Dpos = 0;
	hdl->Perr = 0;			/* 偏差 */
	hdl->Fbspd = 0;			/* FB速度 */
	hdl->Pgcnt = 0;			/* FB速度 */
	hdl->SoftLimit = 0;		/* ソフトリミット状態 */
	hdl->RefEnable = FALSE;	/* リファレンス点無効 */
	hdl->PosEnable = FALSE;	/* 位置データ無効 */
	hdl->Prm = prm;
}
#pragma arm section code


/****************************************************************************
 FBデータ作成処理
 APOS,PERR, FBSPDの更新
****************************************************************************/
VOID PsnUpdateFBData(PSNHNDL* hdl, LONG pgcnt)
{
	LONG data;

/* FB位置、速度作成 */
	data = pgcnt - hdl->Pgcnt;
	hdl->Fbspd = data;					/* FB速度 */
	hdl->Apos += data;					/* FB位置 */
	hdl->Pgcnt = pgcnt;					/* PGCNT保存 */
/* MPOS更新 */
	if(hdl->Prm->TdlyIn <= 0)
	{
		data = (hdl->lstDpos * hdl->Prm->TdlyIn) / hdl->Prm->SegNum;
		hdl->Mpos = (hdl->Pos - hdl->Dpos) + data;
	}
	else
	{
		data = (hdl->Dpos * hdl->Prm->TdlyIn) / hdl->Prm->SegNum;
		hdl->Mpos = (hdl->Pos - hdl->Dpos) - data;
	}
/* PERR更新 */
	hdl->Perr = hdl->Mpos - hdl->Apos;	/* PERR */

/* 32ms平均 */
#if 0
	hdl->FbSpdSum += hdl->Fbspd;
	hdl->FbSpdSumCnt ++;
	if(hdl->FbSpdSumCnt >= KPI_SCANN_MS( 32 ) )
	{
		hdl->FbSpdx = (hdl->FbSpdSum >> 5);
		hdl->FbSpdSum= 0;
		hdl->FbSpdSumCnt = 0;
	}
#endif
}

/****************************************************************************
 PSET状態取得
****************************************************************************/
BOOL PsnGetPsetSts(PSNHNDL* hdl)
{
	LONG data = hdl->Tpos - hdl->Apos;
	if((hdl->Den == TRUE) && ((ULONG)MlibABS(data) <= hdl->Prm->psetWidth))
	{/* DEN=1 && (Tpos-Apos)<=位置決め完了幅 */
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************
 NEAR状態取得
****************************************************************************/
BOOL PsnGetNearSts(PSNHNDL* hdl)
{
	LONG data = hdl->Tpos - hdl->Apos;
	if((ULONG)MlibABS(data) <= hdl->Prm->nearWidth)
	{/* (Tpos-Apos)<=NEAR幅 */
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************
 ZPOINT状態取得
****************************************************************************/
BOOL PsnGetZPointSts(PSNHNDL* hdl)
{
	if((hdl->RefEnable == TRUE) 
		&& ((ULONG)MlibABS(hdl->Apos) <= hdl->Prm->zPointWidth))
	{/* Refernce点有効 && APOS <= 原点近傍幅 */
		return TRUE;
	}
	return FALSE;
}


/****************************************************************************
 TPOS設定処理(絶対値)
 ****************************************************************************/
LONG PsnSetTPOS(PSNHNDL* hdl,LONG tpos)
{
	LONG dpos = tpos - hdl->Ipos;

	PsnCheckSoftLimit(hdl, &dpos);
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}

/****************************************************************************
 TPOS設定処理(相対距離)
 ****************************************************************************/
LONG PsnSetTPOSDist(PSNHNDL* hdl,LONG dpos)
{
	PsnCheckSoftLimit(hdl, &dpos);
	hdl->Tpos = dpos + hdl->Ipos;
	return dpos;
}

/****************************************************************************
 速度・トルク制御時の位置合わせ処理
 TPOS=IPOS=POS=APOS+errにする
****************************************************************************/
VOID PsnResetCmdPosAll(PSNHNDL* hdl, LONG err)
{
	LONG data = hdl->Apos + err;

	hdl->Ipos = data;
	hdl->Pos = data;
	if(hdl->PosEnable)
	{/* 座標系有効時 */
		hdl->Tpos = data;
		hdl->Mpos = data;
		hdl->Perr = hdl->Mpos - hdl->Apos;
	}
	else
	{/* 座標系無効時 */
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
UCHAR PsnCheckSoftLimit(PSNHNDL* hdl, LONG* dpos)
{
	LONG dist, sdist;

/* 指令リミットチェック */	
	if(hdl->Prm->tposLimitEnable && hdl->RefEnable)
	{
		dist = *dpos;
		if(dist >= 0)
		{/* 正側 */
			sdist = hdl->Prm->softLimit_P - hdl->Ipos;
			if((dist >= sdist) && (hdl->Prm->sotEnableP == TRUE))
			{/* ソフトリミット有効 && リミット外 */
				/* リミット値に達してないなら、リミットまでの移動量をセット */
				/* リミット値に達してたら、移動量0をセット */
				*dpos = ((sdist > 0) ? sdist: 0);
			}
		}
		else
		{/* 負側 */
			sdist = hdl->Prm->softLimit_N - hdl->Ipos;
			if((dist <= sdist) && (hdl->Prm->sotEnableN == TRUE))
			{/* ソフトリミット有効 && リミット外 */
				/* リミット値に達してないなら、リミットまでの移動量をセット */
				/* リミット値に達してたら、移動量0をセット */
				*dpos = ((sdist < 0) ? sdist: 0);
			}
		}
	}

	return 0;
}

/****************************************************************************
 ソフトリミット状態チェック処理(APOS)
 1)リファレンス点無効時：SOTステータスクリアし、リターン
 2)APOSのソフトリミットチェック
   APOSがソフトリミット以上ならSOTステータスを更新し、SOT状態をリターン
****************************************************************************/
UCHAR PsnCheckSoftLimitFB(PSNHNDL* hdl, BOOL* p_ot, BOOL* n_ot)
{
	UCHAR	sts = 0;
	*p_ot = FALSE;
	*n_ot = FALSE;
/* リファレンス点無効時、ステータスをクリアし、強制リターン */
	if(!hdl->RefEnable)
	{
		hdl->SoftLimit = 0;
		return 0;
	}
/* FBリミットチェック */	
	if((hdl->Apos >= hdl->Prm->softLimit_P) && (hdl->Prm->sotEnableP == TRUE))
	{/* 正側ソフトリミット状態 */
		sts |= SOFT_LIMIT_P;
		*p_ot = TRUE;
	}
	if((hdl->Apos <= hdl->Prm->softLimit_N) && (hdl->Prm->sotEnableN == TRUE))
	{/* 負側ソフトリミット状態 */
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
VOID PsnSetCordinate(PSNHNDL* hdl, UCHAR mode, LONG pos)
{
	LONG offset;

	/* POS_SETモード：POS基準で位置をリセット */
	offset = pos - hdl->Pos;
	hdl->Pos = pos;
	hdl->Mpos = pos;
	hdl->Ipos += offset;
	hdl->Apos += offset;
	hdl->Tpos += offset;
}

/****************************************************************************
 座標系リセット
 基準で全ての位置をPGCNT基準にリセットする
 また、abs=1&&ABS_PG時は、ABSOffsetを加算し、リファレンス点有効にする
 abs=0のときは、リファレンス点無効となる
****************************************************************************/
VOID PsnResetApos(PSNHNDL* hdl, BOOL abs)
{
	LONG offset;
	LONG apos;

	if(abs == TRUE)
	{/* ABS位置作成モード：原点位置オフセットを加算し、APOS基準で位置をリセット */
		apos = hdl->Pgcnt;
		if(hdl->Prm->absPG)
		{
			apos += hdl->Prm->absOfsset;
			hdl->RefEnable = TRUE;
		}
		offset = apos - hdl->Apos;
		hdl->Apos = apos;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->PosEnable = TRUE;
	}
	else
	{/* APOSリセットモード：APOS基準で位置をリセット */
		offset = hdl->Pgcnt - hdl->Apos;
		hdl->Apos = hdl->Pgcnt;
		hdl->Ipos += offset;
		hdl->Pos += offset;
		hdl->Mpos += offset;
		hdl->Tpos += offset;
		hdl->RefEnable = FALSE;
		hdl->PosEnable = FALSE;
	}
}


/****************************************************************************
 LPOS取得
	ラッチ領域内ならば、Lposを更新し、TRUEを返す。
****************************************************************************/
BOOL PsnMakeLpos(PSNHNDL* hdl, LONG ltcnt)
{
	LONG lpos;
	BOOL ltcmp = FALSE;

	lpos = hdl->Apos + (ltcnt - hdl->Pgcnt);
	if(hdl->Prm->ltRangeP > hdl->Prm->ltRangeN) 
	{/* ラッチ上限値 > ラッチ下限値 の場合、
		ラッチ領域は、ラッチ下限値 <= lpos <= ラッチ上限値 */
		if((lpos >= hdl->Prm->ltRangeN) && (lpos <= hdl->Prm->ltRangeP))
		{
			ltcmp = TRUE;
			hdl->LstLpos = hdl->Lpos;
			hdl->Lpos = lpos;
		}
	}
	else
	{/* ラッチ上限値 <= ラッチ下限値 の場合
		ラッチ領域は、lpos <= ラッチ下限値か、又は、 ラッチ上限値 <= lpos */
		if((lpos <= hdl->Prm->ltRangeP) || (lpos >= hdl->Prm->ltRangeN))
		{
			ltcmp = TRUE;
			hdl->LstLpos = hdl->Lpos;
			hdl->Lpos = lpos;
		}
	}
	return ltcmp;
}


