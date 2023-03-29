/****************************************************************************
Description	: MECHATROLINK Alarm Manager
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
#include "CmdAnalyzer.h"
#include "ComAlmMngr.h"
#include "MtApi.h"
#include "SysApi.h"
#include "AlarmManager.h"
#include "MLib.h"

#define HIST_NUM	10		/* アラームトレース数 */

VOID  AlmSetCommAlmBlkCancel(USHORT data);/* <V923> */

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
/****************************************************************************
 初期化処理
****************************************************************************/
VOID AlmInit( ALM_HNDL *hAlm, ALMMAN_HANDLE *SvAlmMngr )
{
	/* データクリア */
	MlibResetByteMemory( hAlm, sizeof(ALM_HNDL) );

	/* アラーム管理データへのポインタ設定 */
	hAlm->SvAlmMngr = SvAlmMngr;
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif
/****************************************************************************
 アラーム管理処理
****************************************************************************/
VOID AlmHandleMain( ALM_HNDL *hAlm )
{
	/* アラームクリア */
	if(FALSE != hAlm->clrReq)
	{
		if(FALSE != ALMGetAlarmResetStatus(hAlm->SvAlmMngr))
		{/* アラームリセット中はリターン */
			return;
		}
		hAlm->clrReq = FALSE;
	}

	/* アラームステータス更新 */
	hAlm->errCode = ALMGetStatus(hAlm->SvAlmMngr, &hAlm->almSts, &hAlm->wrnSts);

}


/****************************************************************************
 アラーム登録
****************************************************************************/
VOID AlmSetCode( ALM_HNDL *hAlm, LONG ax_no, INT32 alm_no )
{
	ALMID_T	errIdNo;
	errIdNo	 = (ALMID_T)(alm_no & ERR_SVID_MASK);

	/* アラーム情報登録 */
	if(NULL == ALMSetServoAlarm(hAlm->SvAlmMngr, errIdNo))
	{/* アラームマスク時 */
		return;
	}

}
/****************************************************************************
 アラーム検出マスク設定処理
****************************************************************************/
void AlmSetMask( ALM_HNDL *hAlm, INT32 idx, UCHAR enable )
{
	ALMID_T	errIdNo;
	errIdNo	 = (ALMID_T)(idx & ERR_SVID_MASK);

	ALMSetMask(hAlm->SvAlmMngr, errIdNo, (BOOL)enable);	
}

/****************************************************************************
 アラームクリア
****************************************************************************/
INT32 AlmSetClearReq( ALM_HNDL *hAlm, BOOL req )
{
	/* クリア中の場合は、継続中でリターン */
	if(FALSE != hAlm->clrReq)
	{
		return ALM_CONTINUE;
	}

	/* クリア要求発行 */
	if(req != FALSE)
	{ /* 要求あり && エラーあり -> リセット開始 */
		ALMSetAlarmResetRequest(hAlm->SvAlmMngr);
		hAlm->clrReq = TRUE;						/* クリア要求ON */
		return ALM_CONTINUE;
	}
	return ALM_SUCCESS;
}


/****************************************************************************
 アラーム現在発生履歴取得(INDEX指定)
****************************************************************************/
INT32 AlmGetCrtBuf(  ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist )
{
	ALMGetNetAlarmTableInfo( hAlm->SvAlmMngr, (USHORT*)pHist, idx );
	return 0;
}


/****************************************************************************
 アラーム現在発生履歴取得(全て)
****************************************************************************/
INT32 AlmGetCrtBufAll(  ALM_HNDL *hAlm, UCHAR *pHist )
{
	ALMGetNetAlarmTableCode( hAlm->SvAlmMngr, pHist, HIST_NUM );
	return 0;
}


/****************************************************************************
 サーボアラーム発生履歴取得(INDEX指定)
****************************************************************************/
INT32 AlmGetHistory(  ALM_HNDL *hAlm, UCHAR idx, UCHAR *pHist )
{
	ALMGetNetAlarmTraceInfo( hAlm->SvAlmMngr, (USHORT*)pHist, idx );
	return 0;
}


/****************************************************************************
 サーボアラーム発生履歴取得(全て)
****************************************************************************/
INT32 AlmGetHistoryAll(  ALM_HNDL *hAlm, UCHAR *pHist )
{
	ALMGetNetAlarmTraceCode( hAlm->SvAlmMngr, pHist, HIST_NUM );
	return 0;
}


/****************************************************************************
 サーボアラーム発生履歴クリア
****************************************************************************/
void AlmRequestAlarmHistoryClear( ALM_HNDL *hAlm )
{
	ALMSetAlarmHistResetRequest( hAlm->SvAlmMngr );
}
INT32 AlmGetAlarmHistoryClearStatus( ALM_HNDL *hAlm )
{
	return ALMGetAlarmHistResetStatus( hAlm->SvAlmMngr );
}


