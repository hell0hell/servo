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


#define HIST_NUM		10		/* アラームトレース数 */



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


/****************************************************************************
 アラーム管理処理
****************************************************************************/
VOID AlmHandleMain( ALM_HNDL *hAlm )
{

	/* CMD_ALM自動クリアチェック */
//	CmdAlmClear( hAlm, pCmdHdl->cmdInfo.CmdAlm, pCmdHdl->exCmdInfo.CmdAlm );

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

	/* 位置データ無効アラーム,M3通信用アラーム情報更新 */
	hAlm->PosDataNG = ALMGetExNetStatus(hAlm->SvAlmMngr, &hAlm->M3StdAlm);

	/* COMM_ALM状態でない場合、COMM_ALMコードをクリア */
	if( hAlm->M3StdAlm.CommAlm == FALSE )
	{
		hAlm->CommAlmCode = 0x00;
	}

//#if 0 /* <S092> */
	/* CMD_ALM状態でない場合、CMD_ALMコードをクリア */
	if( hAlm->M3StdAlm.CmdAlm == FALSE )
	{
		hAlm->CmdAlmCode   = 0x00;
		hAlm->ExCmdAlmCode = 0x00;
	}
//#endif /* <S092> */
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

	/* COMM_ALM設定 */
	if(0 != (alm_no & ERR_COM_BIT))
	{
		/*------------------------------------------------------------------*/
		/* 前COMM_ALM=0の場合(COMM_ALM未発生状態) ⇒ COMM_ALM更新			*/
		/*------------------------------------------------------------------*/
		if(hAlm->CommAlmCode == 0)
		{
			hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
		}
		/*------------------------------------------------------------------*/
		/* 前COMM_ALM<8の場合(前COMM_ALMがﾜｰﾆﾝｸﾞ) ⇒ ｱﾗｰﾑ時のみCOMM_ALM更新	*/
		/*------------------------------------------------------------------*/
		else if(hAlm->CommAlmCode < 0x08)
		{
			if(0 == (alm_no & ERR_WRN_BIT))
			{
				hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
			}
		}
		/*------------------------------------------------------------------*/
		/* 前COMM_ALM≧8の場合(前COMM_ALMがｱﾗｰﾑ) ⇒ COMM_ALM更新しない		*/
		/*------------------------------------------------------------------*/
		else
		{
			;
		}
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

/****************************************************************************/
/*	CMD_ALMコードセット														*/
/*																			*/
/*	パネオペ表示用ワーニングコード更新について								*/
/*		CMD_ALMは、ネットワークモジュール用ScanC処理より前にクリア			*/
/*		される場合があるため、ここで更新しておく。							*/
/****************************************************************************/
VOID AlmSetCmdAlmCode( ALM_HNDL *hAlm, INT32 alm_no, BOOL ExCmd )
{
	ALMID_T	errIdNo;
	errIdNo	 = (ALMID_T)(alm_no & ERR_SVID_MASK);

	/* アラーム情報登録 */
	if(NULL == ALMSetServoAlarm(hAlm->SvAlmMngr, errIdNo))
	{/* アラームマスク時 */
		return;
	}

	/* CMD_ALM設定 */
	if(0 != (alm_no & ERR_CMD_BIT) )
	{
		if( ExCmd != FALSE )
		{
			hAlm->ExCmdAlmCode = GET_ERR_RSP(alm_no);
		}
		else
		{
			hAlm->CmdAlmCode = GET_ERR_RSP(alm_no);
		}
	}
	
	/* COMM_ALM設定 */
	if(0 != (alm_no & ERR_COM_BIT))
	{
		/*------------------------------------------------------------------*/
		/* 前COMM_ALM=0の場合(COMM_ALM未発生状態) ⇒ COMM_ALM更新			*/
		/*------------------------------------------------------------------*/
		if(hAlm->CommAlmCode == 0)
		{
			hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
		}
		/*------------------------------------------------------------------*/
		/* 前COMM_ALM<8の場合(前COMM_ALMがﾜｰﾆﾝｸﾞ) ⇒ ｱﾗｰﾑ時のみCOMM_ALM更新	*/
		/*------------------------------------------------------------------*/
		else if(hAlm->CommAlmCode < 0x08)
		{
			if(0 == (alm_no & ERR_WRN_BIT))
			{
				hAlm->CommAlmCode = GET_ERR_RSP(alm_no);
			}
		}
		/*------------------------------------------------------------------*/
		/* 前COMM_ALM≧8の場合(前COMM_ALMがｱﾗｰﾑ) ⇒ COMM_ALM更新しない		*/
		/*------------------------------------------------------------------*/
		else
		{
			;
		}
	}
}


/****************************************************************************
 CMD_ALMクリア
****************************************************************************/
VOID AlmClearCmdSate( ALM_HNDL *hAlm, BOOL CmdAlm, BOOL ExCmdAlm )
{
	/*----------------------------------------------------------------------*/
	/*	自動クリアなし														*/
	/*----------------------------------------------------------------------*/
	if( hAlm->CmdAlmClrMode == FALSE )
	{
		return;
	}
	
	if( (hAlm->CmdAlmCode != 0) || (hAlm->ExCmdAlmCode != 0) )
	{
		if( CmdAlm == FALSE )		/* メインコマンド解析正常終了 */
		{
			if( ExCmdAlm == FALSE )	/* メイン、サブともに正常終了*/
			{
				hAlm->SvAlmMngr->NetCmdAlmRst = TRUE;
				hAlm->ExCmdAlmCode = 0x00;
			}
			hAlm->CmdAlmCode = 0x00;
		}
		else
		{
			if( ExCmdAlm == FALSE )	/* サブコマンド解析正常終了 */
			{
				hAlm->ExCmdAlmCode = 0x00;
			}
		}
	}
}


/****************************************************************************
 CMD_ALM自動クリアモード設定処理
****************************************************************************/
VOID AlmSetCmdAlmClearMode(ALM_HNDL *hAlm, LONG prof, ULONG comCtrl)
{
	if( (prof == M3STD_PROFILE) || (prof == EVTID_PROFILE) )
	{
		if( (comCtrl & 0x1000) != 0 )
		{
			hAlm->CmdAlmClrMode = TRUE;
		}
		else
		{
			hAlm->CmdAlmClrMode = FALSE;
		}
	}
	else
	{
		hAlm->CmdAlmClrMode = FALSE;
	}
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
 アラーム現在発生履歴取得(全て、２バイト)
****************************************************************************/
INT32 AlmGetCrtDetailBufAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_num )
{
	ALMGetNetAlarmTableDetailCode( hAlm->SvAlmMngr, pHist, hist_num );
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
 サーボアラーム発生履歴取得(全て、２バイト)
****************************************************************************/
INT32 AlmGetHistoryDetailAll( ALM_HNDL *hAlm, USHORT *pHist, LONG hist_num )
{
	ALMGetNetAlarmTraceDetailCode( hAlm->SvAlmMngr, pHist, hist_num );
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



