/****************************************************************************/
/*																			*/
/*		DPM Access Control ソースファイル									*/
/*																			*/
/****************************************************************************/
/*																			*/
/*	Target model: SigmaV Command-Option	ServoDrive							*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	History:																*/
/*	2007.9.12	created														*/
/*																			*/
/*																			*/
/************** Copyright(c) Yaskawa Electric Corporation *******************/

#define	KPI_IOREG
#define	DPM_CTRL_MODULE


/* includes */
#include "KnlStd.h"
#include "Dpm.h"

#ifdef __cplusplus
extern	"C"	{
#endif

/* forward declarations */
/* Public functions */
//void DpmInitialize(DPMCTRLIF *pDpmCtl, SV_OPTION_IF *pSvIf);
//LONG DpmHandshake(DPMCTRLIF *pDpmCtl);
//LONG DpmUpdateHndshkFlag(DPMCTRLIF *pDpmCtl);
//LONG DpmResetScheduler(DPMCTRLIF *pDpmCtl);
//LONG DpmMakeCycDataUpdateReq(DPMCTRLIF *pDpmCtl);
//void DpmWriteCycRSP(DPMCTRLIF *pDpmCtl);
//LONG DpmReadCycCMD(DPMCTRLIF *pDpmCtl);

/* Private functions */
static void dpmCopyRvsdData(void* dst, void* src, ULONG sz);
static void dpmCopyData(void* dst, USHORT src, ULONG sz);
static void dpmGetOptInfomationData(DPMCTRLIF *pDpmCtl);
static LONG dpmSetSvInfomationData(DPMCTRLIF *pDpmCtl);
static USHORT dpmCheckCycle(USHORT cycle, USHORT baseCycle);
static USHORT dpmIDOLState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
static USHORT dpmINTRDYState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
static USHORT dpmStartHndshkState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
static USHORT dpmMemCheckState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
static USHORT dpmMemClrState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
static USHORT dpmAsyncState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
static USHORT dpmSyncState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup);
__inline static void dpmUpdateCSHndshkFlag(NONCYC_STATE *ctrl, VUSHORT *pFlag);
__inline static void dpmUpdateSCHndshkFlag(NONCYC_STATE *ctrl, VUSHORT *pFlag);
__inline static USHORT dpmReadCycData(void* pRamBuf, void* pDpmBuf, USHORT chksum);
__inline static USHORT dpmWriteCycData(void* pRamBuf, void* pDpmBuf, USHORT wrCycle);

#ifdef __cplusplus
}
#endif



/****************************************************************************
****************************************************************************/
/* Public Functions */
/****************************************************************************
****************************************************************************/

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/****************************************************************************
 DPM Optionモジュール初期化
-----------------------------------------------------------------------------
paramter:	pDpmCtl	- pointer to DPM resouce data[OUT]
			pSvIf	- pointer to internal Servo-Option I/F data[IN]
return;		none
****************************************************************************/
void DpmInitialize(DPMCTRLIF *pDpmCtl, SV_OPTION_IF *pSvIf)
{
	MlibResetLongMemory(pDpmCtl, sizeof(DPMCTRLIF)/4 );
	pDpmCtl->pSvOptIf = pSvIf;
	pDpmCtl->pSvOptIf->OptSetting.CycTime = 1;
	pDpmCtl->pSvOptIf->SvInfo.IOCycle = DPM_TIO_BASE_TIME;
/* Set DPM Address */
	pDpmCtl->pHndshk = (HNDSHK_DATA*)(DPM_TOP_ADDR + 0);
	pDpmCtl->pOpInfo = (OPINFO_DATA*)(DPM_TOP_ADDR + DPM_OPINFO_OFS_ADR);
	pDpmCtl->pSvInfo = (SVINFO_DATA*)(DPM_TOP_ADDR + DPM_SVINFO_OFS_ADR);
	pDpmCtl->pCycCmd[0] = (CYCCMD_DATA*)(DPM_TOP_ADDR + DPM_CYCCMDA_OFS_ADR);
	pDpmCtl->pCycRsp[0] = (CYCRSP_DATA*)(DPM_TOP_ADDR + DPM_CYCRSPA_OFS_ADR);
	pDpmCtl->pCycCmd[1] = (CYCCMD_DATA*)(DPM_TOP_ADDR + DPM_CYCCMDB_OFS_ADR);
	pDpmCtl->pCycRsp[1] = (CYCRSP_DATA*)(DPM_TOP_ADDR + DPM_CYCRSPB_OFS_ADR);
	pDpmCtl->pLink  = (DOLINK_DATA*)(DPM_TOP_ADDR + DPM_LINKCMD_OFS_ADR);
	pDpmCtl->pDeg   = (DOLINK_DATA*)(DPM_TOP_ADDR + DPM_DOCMD_OFS_ADR);
	pDpmCtl->pCSMsg = (DPMSG_DATA*)(DPM_TOP_ADDR + DPM_CSMSGCMD_OFS_ADR);
	pDpmCtl->pSCMsg = (DPMSG_DATA*)(DPM_TOP_ADDR + DPM_SCMSGCMD_OFS_ADR);
	pDpmCtl->pDegEx = (DGOPEX_RSP*)(DPM_TOP_ADDR + DPM_DORSPEX_OFS_ADR);	/*<V685>*/
}

/****************************************************************************
 DPM イニシャルシーケンス メイン処理
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		DPM_TIMEOUT_ERR	- Timeout(10s) error of DPM initial sequece
			DPM_FUNC_SUCCESS- normal end
****************************************************************************/
LONG DpmHandshake(DPMCTRLIF *pDpmCtl)
{
	USHORT s_setup, seq;
	LONG rc = DPM_FUNC_SUCCESS;

	seq = pDpmCtl->State;
	s_setup = pDpmCtl->hndshk.S_Setup;
	switch(pDpmCtl->State)
	{
	case DPHNDSHK_IDOL_SEQ:		/* IDOL State */
		seq = dpmIDOLState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;
	case DPHNDSHK_INTRDY_SEQ:	/* Option Ready State<DPM_V103> */
		seq = dpmINTRDYState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;
	case DPHNDSHK_START_SEQ:	/* Handshake Start State */
		seq = dpmStartHndshkState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;
	case DPHNDSHK_MEMCHK_SEQ:	/* Memory Check State */
		seq = dpmMemCheckState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;
	case DPHNDSHK_MEMCLR_SEQ:	/* Memory Clear State */
		seq = dpmMemClrState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;
	case DPHNDSHK_ASYNC_SEQ:	/* Non-Cyclic Exchange State */
		seq = dpmAsyncState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;
	case DPHNDSHK_SYNC_SEQ:		/* Cyclic Exchange  State */
		seq = dpmSyncState(pDpmCtl, pDpmCtl->hndshk.C_Setup, &s_setup); 
		break;	
	case DPHNDSHK_ERR_SEQ:		/* Error State */
		break;
	default:					/* Abnormal State */
		break;
	}
	pDpmCtl->State = seq;
	pDpmCtl->hndshk.S_Setup = s_setup;

/* Timeout check *///<DPM_V103>
	if((seq == DPHNDSHK_SYNC_SEQ) || (seq == DPHNDSHK_IDOL_SEQ))
	{
		KpiRstLongTimer(&pDpmCtl->timerCnt);
	}
	else
	{
		if( KpiGetLongTimer(&pDpmCtl->timerCnt) > DPM_HANDSHAKE_TIMEOUT)
		{
			KpiRstLongTimer(&pDpmCtl->timerCnt);
			pDpmCtl->State = DPHNDSHK_ERR_SEQ;
			rc = DPM_TIMEOUT_ERR;
		}
	}

	return rc;
}
#ifdef	__ghs__
#pragma ghs section text=default
#endif

/****************************************************************************
 Handshake データ更新(IntSync内で実施)
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		DPM_FUNC_SUCCESS- normal end
****************************************************************************/
LONG DpmUpdateHndshkFlag(DPMCTRLIF *pDpmCtl)
{
	USHORT data;
/* Update Cyclic Control Flags */
	data = pDpmCtl->pHndshk->CycCtrl & DPCYC_BUFCHG_R;
	pDpmCtl->hndshk.CycCtrl = (USHORT)(pDpmCtl->hndshk.CycCtrl & DPCYC_CTRL_MASK) | data;
	data |= (data << DPCYC_BUFCHG_ACK_BIT);
	pDpmCtl->pHndshk->CycCtrl = data;

/* Update Link Control Flags */
	dpmUpdateCSHndshkFlag(&pDpmCtl->hndshk.LnkCtrl, &pDpmCtl->pHndshk->LinkCtrl);
/* Update CS Msg Control Flags */
	dpmUpdateCSHndshkFlag(&pDpmCtl->hndshk.CSCtrl, &pDpmCtl->pHndshk->CSCtrl);
/* Update DegOp Control Flags */
	dpmUpdateSCHndshkFlag(&pDpmCtl->hndshk.DOCtrl, &pDpmCtl->pHndshk->DOCtrl);
/* Update SC Msg Control Flags */
	dpmUpdateSCHndshkFlag(&pDpmCtl->hndshk.SCCtrl, &pDpmCtl->pHndshk->SCCtrl);

/* Update Handshake Control Flags */
	++pDpmCtl->hndshk.ServoEnd;
	pDpmCtl->hndshk.C_Setup = pDpmCtl->pHndshk->C_Setup;
	pDpmCtl->pHndshk->S_Setup = pDpmCtl->hndshk.S_Setup;
	pDpmCtl->pHndshk->ServoEnd = pDpmCtl->hndshk.ServoEnd;

	return DPM_FUNC_SUCCESS;
}


/****************************************************************************
 スケジュールカウンタリセット＆同期状態遷移
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		DPM_SYNC_ERR	- synchronization error of IntSync and ScanA/B
			DPM_FUNC_SUCCESS- normal end
****************************************************************************/
LONG DpmResetScheduler(DPMCTRLIF *pDpmCtl)
{
	LONG	rc = DPM_FUNC_SUCCESS;
	USHORT  cycCtl = pDpmCtl->hndshk.CycCtrl;

	if((cycCtl & DPCYC_SYNC_ENABLED) != 0)
	{/* IntSync同期状態 */
		if(pDpmCtl->tioCycCnt != pDpmCtl->pSvOptIf->OptSetting.CycTime)
		{/* 同期状態NG */
			pDpmCtl->wdErrCnt = DPM_WDCCHK_CNT;
			pDpmCtl->pSvOptIf->CtrlReq.SyncChg = TRUE;
			if((cycCtl & DPCYC_WDC_ENABLED) != 0)
			{/* WDCチェック開始状態ならエラー */
				rc = DPM_SYNC_ERR;
			}
		}
	}
	else
	{/* IntSync非同期状態 */
		if((cycCtl & DPCYC_SYNC_REQ) != 0)
		{/* 同期要求あり */
			if((KPI_SYNC_CHKSTATUS() != FALSE) &&
				(pDpmCtl->tioCycCnt == pDpmCtl->pSvOptIf->OptSetting.CycTime))
			{/* (同期要求受付完了 && ScanB実行回数OK)->IntSync同期状態へ */
				pDpmCtl->hndshk.CycCtrl = cycCtl | DPCYC_SYNC_ENABLED;
			}
		}
//		pDpmCtl->pSvOptIf->CtrlReq.SyncChg = TRUE;
	}
	pDpmCtl->tioCycCnt = 0;

	return rc;
}

/****************************************************************************
 Cyclic Command データ取得
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		DPM_SYNC_ERR	- Option Card watch-dog counter error
			DPM_CHKSUM_ERR	- Cyclic Command check-sum error
			DPM_FUNC_SUCCESS- normal end
****************************************************************************/
LONG DpmReadCycCMD(DPMCTRLIF *pDpmCtl)
{
	LONG	rc = DPM_FUNC_SUCCESS;
	USHORT  cycCtl = pDpmCtl->hndshk.CycCtrl;
	USHORT	l_wdc = pDpmCtl->pSvOptIf->CycCmd.Wdc;
	CYCCMD_DATA	*pDpmCyc = pDpmCtl->pCycCmd[cycCtl & DPCYC_BUFCHG_R];

	if((cycCtl & DPCYC_SYNC_ENABLED) == 0)
	{/* 非同期時は、そのままリターン */
		pDpmCtl->pSvOptIf->CtrlReq.CmdUpdate = FALSE;
		pDpmCtl->pSvOptIf->CtrlReq.SyncChg = TRUE;
	}
	else
	{/* 同期時のみ、DPMデータ取得 */
		if( dpmReadCycData(&pDpmCtl->pSvOptIf->CycCmd, 
							&pDpmCyc->Cmd, pDpmCyc->C_Chksum) != 0)
		{/* Checksum NG */
			pDpmCtl->wdErrCnt = DPM_WDCCHK_CNT;
			pDpmCtl->pSvOptIf->CtrlReq.CmdUpdate = FALSE;
			if((cycCtl & DPCYC_WDC_ENABLED) != 0)
			{/* WDCチェック開始状態ならエラー */
				rc = DPM_CHKSUM_ERR;
			}
		}
		else
		{/* Checksum OK */
			pDpmCtl->pSvOptIf->CtrlReq.CmdUpdate = TRUE;
			if( (USHORT)(l_wdc+1) == pDpmCtl->pSvOptIf->CycCmd.Wdc )
			{/* WDCチェックOK */
				if(pDpmCtl->wdErrCnt == 0)
				{/* 正常状態 */
					;
				}
				else if(pDpmCtl->wdErrCnt > 1)
				{/* WDC連続16回OK待ち */
					--pDpmCtl->wdErrCnt;
					pDpmCtl->pSvOptIf->CtrlReq.SyncChg = TRUE;
				}
				else
				{/* WDC連続16回OK後、通常状態復帰待ち */
					if(pDpmCtl->pSvOptIf->CtrlAck.SyncChg == FALSE)
					{
						pDpmCtl->wdErrCnt = 0;
						pDpmCtl->hndshk.CycCtrl = cycCtl | DPCYC_WDC_ENABLED;
					}
					pDpmCtl->pSvOptIf->CtrlReq.SyncChg = FALSE;
				}
			}
			else
			{/* WDCチェックNG */
				pDpmCtl->wdErrCnt = DPM_WDCCHK_CNT;
				pDpmCtl->pSvOptIf->CtrlReq.SyncChg = TRUE;
				if((cycCtl & DPCYC_WDC_ENABLED) != 0)
				{/* WDCチェック開始状態ならエラー */
					rc = DPM_SYNC_ERR;
				}
			}
		}
	}

	return rc;
}

/****************************************************************************
 DPM Cyclic データ更新要求作成(ScanB先頭で実行)
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		DPM_SYNC_ERR	- synchronization error of IntSync and ScanA/B
			DPM_FUNC_SUCCESS- normal end
****************************************************************************/
LONG DpmMakeCycDataUpdateReq(DPMCTRLIF *pDpmCtl)
{
	USHORT cnt = pDpmCtl->tioCycCnt;
	LONG   rc = DPM_FUNC_SUCCESS;

	if((pDpmCtl->hndshk.CycCtrl & DPCYC_SYNC_ENABLED) == 0)
	{/* 同期未完状態 */
		pDpmCtl->pSvOptIf->CtrlReq.SyncChg = TRUE;
		pDpmCtl->pSvOptIf->CtrlReq.RspUpdate = TRUE;
	}
	else
	{/* 同期完状態 */
	/* RSPデータ更新要求作成 */
		if(cnt == (pDpmCtl->pSvOptIf->OptSetting.CycTime - 1))
		{/* RSP更新タイミング */
			pDpmCtl->pSvOptIf->CtrlReq.RspUpdate = TRUE;
		}
		else
		{
			if(cnt >= pDpmCtl->pSvOptIf->OptSetting.CycTime)
			{
				pDpmCtl->tioCycCnt = 0;
				if((pDpmCtl->hndshk.CycCtrl & DPCYC_WDC_ENABLED) != 0)
				{	rc = DPM_SYNC_ERR;}
			}
		}
	}
	return rc;
}


/****************************************************************************
 Cyclic Responce データ設定
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		none
****************************************************************************/
void DpmWriteCycRSP(DPMCTRLIF *pDpmCtl)
{
	CYCRSP_DATA	*pDpmCyc = pDpmCtl->pCycRsp[pDpmCtl->hndshk.CycCtrl & DPCYC_BUFCHG_R];

	if(	((pDpmCtl->hndshk.CycCtrl & DPCYC_SYNC_ENABLED) != 0)
		&& (pDpmCtl->pSvOptIf->CtrlAck.RspUpdate != FALSE) )
	{
		pDpmCtl->lstChkSum = dpmWriteCycData(&pDpmCtl->pSvOptIf->CycRsp,
								 &pDpmCyc->Rsp, pDpmCtl->pSvOptIf->SvInfo.IOCycle);
		pDpmCyc->S_Chksum = pDpmCtl->lstChkSum;
	}
	pDpmCtl->pSvOptIf->CtrlReq.CmdUpdate = FALSE;
	pDpmCtl->pSvOptIf->CtrlReq.RspUpdate = FALSE;

}

/****************************************************************************
 Cyclic Responce データ設定(WDC+Checksum値のみ更新)
-----------------------------------------------------------------------------
paramter:	pDpmCtl			- pointer to DPM resouce data
return;		none
****************************************************************************/
void DpmWriteNonCycRSP(DPMCTRLIF *pDpmCtl)
{
	CYCRSP_DATA	*pDpmCyc = pDpmCtl->pCycRsp[pDpmCtl->hndshk.CycCtrl & DPCYC_BUFCHG_R];

	if((pDpmCtl->hndshk.CycCtrl & DPCYC_SYNC_ENABLED) != 0)
	{
		pDpmCyc->Rsp.Wdc = (USHORT)(pDpmCtl->pSvOptIf->CycRsp.Wdc+1);	/* WDC更新 */
		pDpmCtl->lstChkSum = (USHORT)(pDpmCtl->lstChkSum - 1);
		pDpmCyc->S_Chksum = pDpmCtl->lstChkSum;
	}
}


/****************************************************************************
****************************************************************************/
/* Private Handshake Functions */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/* ワードデータメモリコピー処理(1の補数) */
static void dpmCopyRvsdData(void *dst, void *src, ULONG sz)
{
	ULONG i;
	VUSHORT *ds = (VUSHORT*)dst;
	VUSHORT *sr = (VUSHORT*)src;
	for(i = 0; i < sz; i++)
	{
		ds[i] = ~sr[i];
	}
}
/* ワードデータメモリコピー処理 */
static void dpmCopyData(void *dst, USHORT src, ULONG sz)
{
	ULONG i;
	VUSHORT *ds = (VUSHORT*)dst;
	for(i = 0; i < sz; i++)
	{
		ds[i] = src;
	}
}
/* DPMサイクルチェック処理 */
static USHORT dpmCheckCycle(USHORT cycle, USHORT baseCycle)
{
	LONG i;
//	USHORT sv_cyc = DPM_DEFAULT_CYCLE;
	USHORT sv_cyc = DPM_UNDEFINED_CYCLE;
	const USHORT dpmSuppCycle[] = DPM_SUPP_CYCLE;
	LONG sz = sizeof(dpmSuppCycle)/sizeof(USHORT);

	for (i = 0; i < sz; i++)
	{
		if((dpmSuppCycle[i] == cycle) && (cycle >= baseCycle))
		{
			sv_cyc = cycle;
			break;
		}
	}
	return sv_cyc;
}

/****************************************************************************
 DPM Handshake処理(IDOL状態) <DPM_V103>
****************************************************************************/
static USHORT dpmIDOLState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc;

	*s_setup = 0;
	if(0 != pDpmCtl->hndshk.ServoEnd)
	{
		rc = DPHNDSHK_INTRDY_SEQ;
	}
	else
	{
		rc = DPHNDSHK_IDOL_SEQ;
	}
	return rc;
}

/****************************************************************************
 DPM Handshake処理(IDOL状態) <DPM_V103>
****************************************************************************/
static USHORT dpmINTRDYState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc = DPHNDSHK_INTRDY_SEQ;

	switch(c_setup)
	{
	case DPHNDSHK_IDOL_SEQ:		/* 0x0000 */
		*s_setup = c_setup;
		break;
	case DPHNDSHK_START_SEQ:	/* 0x2222 */
		*s_setup = c_setup;
		rc = DPHNDSHK_START_SEQ;
		break;
	default:
		*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		break;
	}
	return rc;
}

/****************************************************************************
 DPM Handshake処理(イニシャル開始状態)
****************************************************************************/
static USHORT dpmStartHndshkState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc = DPHNDSHK_START_SEQ;

	switch(c_setup)
	{
	case DPHNDSHK_IDOL_SEQ:		/* 0x0000 */
	case DPHNDSHK_START_SEQ:	/* 0x2222 */
		*s_setup = c_setup;
		break;
	case DPHNDSHK_MEMCHK_SEQ:	/* 0x5555 */
		rc = DPHNDSHK_MEMCHK_SEQ;
		break;
	default:
		*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		break;
	}
	return rc;
}

/****************************************************************************
 DPM Handshake処理(メモリチェック状態)
****************************************************************************/
static USHORT dpmMemCheckState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc = DPHNDSHK_MEMCHK_SEQ;
	USHORT old_s = *s_setup;

	*s_setup = c_setup;
	switch(c_setup)
	{
	case DPHNDSHK_IDOL_SEQ:		/* 0x0000 */
	case DPHNDSHK_MEMCHK_SEQ:	/* 0x5555 */
		if(old_s != DPHNDSHK_MEMCHK_SEQ)
		{
			dpmCopyRvsdData(pDpmCtl->pSvInfo,		/* Card Infomata Data Area */
					pDpmCtl->pOpInfo, DPM_OPINFO_BSZ/sizeof(USHORT));
			dpmCopyRvsdData(pDpmCtl->pCycRsp[0],	/* Cyclic Data A Area */
					pDpmCtl->pCycCmd[0], DPM_CYCDATA_BSZ/sizeof(USHORT));
			dpmCopyRvsdData(pDpmCtl->pCycRsp[1],	/* Cyclic Data B Area */
					pDpmCtl->pCycCmd[1], DPM_CYCDATA_BSZ/sizeof(USHORT));
			dpmCopyRvsdData(&pDpmCtl->pLink->Rsp,	/* Link Data Area */
					&pDpmCtl->pLink->Cmd, DPM_LINKDATA_BSZ/sizeof(USHORT));
			dpmCopyRvsdData(&pDpmCtl->pDeg->Cmd,	/* DegOp Data Area */
					&pDpmCtl->pDeg->Rsp, DPM_DODATA_BSZ/sizeof(USHORT));
			dpmCopyRvsdData(pDpmCtl->pCSMsg->Rsp,	/* CSMsg Data Area */
					pDpmCtl->pCSMsg->Cmd, DPM_MSGDATA_BSZ/sizeof(USHORT));
			dpmCopyRvsdData(pDpmCtl->pSCMsg->Cmd,	/* SCMsg Data Area */
					pDpmCtl->pSCMsg->Rsp, DPM_MSGDATA_BSZ/sizeof(USHORT));
		}
		break;
	case DPHNDSHK_START_SEQ:	/* 0x2222 */
		rc = DPHNDSHK_START_SEQ;
		break;
	case DPHNDSHK_MEMCLR_SEQ:	/* 0x9999 */
	/* Card Infomata Data Area */
		dpmCopyData(pDpmCtl->pSvInfo,	0, DPM_OPINFO_BSZ/sizeof(USHORT));
	/* Cyclic Data Area */
		dpmCopyData(pDpmCtl->pCycRsp[0], 0, DPM_CYCDATA_BSZ/sizeof(USHORT));
		dpmCopyData(pDpmCtl->pCycRsp[1], 0, DPM_CYCDATA_BSZ/sizeof(USHORT));
	/* Non-Cyclic Data Area */
		dpmCopyData(&pDpmCtl->pLink->Rsp, 0, DPM_LINKDATA_BSZ/sizeof(USHORT));
		dpmCopyData(&pDpmCtl->pDeg->Cmd,  0, DPM_DODATA_BSZ/sizeof(USHORT));
		dpmCopyData(pDpmCtl->pCSMsg->Rsp, 0, DPM_MSGDATA_BSZ/sizeof(USHORT));
		dpmCopyData(pDpmCtl->pSCMsg->Cmd, 0, DPM_MSGDATA_BSZ/sizeof(USHORT));
		rc = DPHNDSHK_MEMCLR_SEQ;
		break;
	default:
		*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		break;
	}
	return rc;
}

/****************************************************************************
 DPM Handshake処理(サーボ情報セット)
****************************************************************************/
static LONG dpmSetSvInfomationData(DPMCTRLIF *pDpmCtl)
{
	USHORT data, rdcnt;
	USHORT swset[2] = {0};
	USHORT cycle = pDpmCtl->pOpInfo->CycTime;
	USHORT baseCycle = pDpmCtl->pSvOptIf->SvInfo.IOCycle;
	LONG rc;

/* Get cyclic time setting data */
	data = dpmCheckCycle(cycle, baseCycle);
	if( data != DPM_UNDEFINED_CYCLE )
	{
		cycle = data;
		rc = OK;
	}
	else
	{
		cycle = DPM_DEFAULT_CYCLE;
		rc = NG;
	}

/* Set Servo Info. */
	pDpmCtl->pSvInfo->SvCycTime = cycle;
	pDpmCtl->pSvInfo->SvIfSuppVer = DPM_IF_VER;
	pDpmCtl->pSvInfo->SvFirmVer = ((ULONG)SvSw.MswVersion << 16) | (ULONG)SvSw.YspVersion;
	pDpmCtl->pSvInfo->SvAxisAdr = (USHORT)Iprm.AxisAddr;
	for(rdcnt = 0; rdcnt < 4; rdcnt++)
	{
		if(KpiGetInitM2SwInfo(&swset[0],&swset[1]) != FALSE)
		{	break;}
	}
	pDpmCtl->pSvInfo->HwSwSet = (swset[1] << 4) | (swset[0]&0xF);

	return rc;
}

/****************************************************************************
 DPM Handshake処理(オプション情報取得)
****************************************************************************/
static void dpmGetOptInfomationData(DPMCTRLIF *pDpmCtl)
{
	USHORT cycle = pDpmCtl->pSvInfo->SvCycTime;
	USHORT baseCycle = pDpmCtl->pSvOptIf->SvInfo.IOCycle;

/* Get Option Card Info. */	
	pDpmCtl->pSvOptIf->OptSetting = *pDpmCtl->pOpInfo;
	pDpmCtl->pSvOptIf->OptSetting.CycTime = cycle/baseCycle;
	pDpmCtl->pSvOptIf->OptSetting.CmdDelay = pDpmCtl->pSvOptIf->OptSetting.CycTime*2;
/*--------------------------------------------------------------------------*/
/*	インタフェース準拠バージョンによるオプション情報取得条件処理	<V683>	*/
/*--------------------------------------------------------------------------*/
	if( pDpmCtl->pSvOptIf->OptSetting.IfSuppVer < 0x0102 )
	{
//<V691>		dpmCopyData(pDpmCtl->pSvOptIf->OptSetting.OpModelExtStr, 0, sizeof(pDpmCtl->pSvOptIf->OptSetting.OpModelExtStr));/*<V683>*/
		dpmCopyData(pDpmCtl->pSvOptIf->OptSetting.OpModelExtStr, 0, sizeof(pDpmCtl->pSvOptIf->OptSetting.OpModelExtStr)/sizeof(USHORT));/*<V691>*/
		pDpmCtl->pSvOptIf->OptSetting.CycCmdExtOption = 0;	/*<V684>*/
		pDpmCtl->pSvOptIf->OptSetting.NumOfFnMode = 0;		/*<V685>*/
	}
/*--------------------------------------------------------------------------*/

/* Set Option Card Info. to Register data */	
	KpxSetCommandOptID(&pDpmCtl->pSvOptIf->OptSetting);
}

/****************************************************************************
 DPM Handshake処理(メモリクリア状態)
****************************************************************************/
static USHORT dpmMemClrState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc = DPHNDSHK_MEMCLR_SEQ;

	*s_setup = c_setup;
	switch(c_setup)
	{
	case DPHNDSHK_IDOL_SEQ:		/* 0x0000 */
	case DPHNDSHK_MEMCLR_SEQ:	/* 0x9999 */
		break;
	case DPHNDSHK_START_SEQ:	/* 0x2222 */
		rc = DPHNDSHK_START_SEQ;
		break;
	case DPHNDSHK_ASYNC_SEQ:	/* 0xAAAA */
	/* Set Servo Infomation */
		if( dpmSetSvInfomationData(pDpmCtl) != OK )
		{
			*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		}
		else
		{
		/* Get Option Card Information */
			dpmGetOptInfomationData(pDpmCtl);
		/* Set Non-Cyclic Flags */
			pDpmCtl->hndshk.LnkCtrl = Enum_IdolState;
			pDpmCtl->hndshk.CSCtrl = Enum_IdolState;
			pDpmCtl->hndshk.SCCtrl = Enum_IdolState;
			pDpmCtl->hndshk.DOCtrl = Enum_IdolState;
			rc = DPHNDSHK_ASYNC_SEQ;
		}
		break;
	default:
		*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		break;
	}
	return rc;
}

/****************************************************************************
 DPM Handshake処理(データ交換開始状態)
****************************************************************************/
static USHORT dpmAsyncState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc = DPHNDSHK_ASYNC_SEQ;

	switch(c_setup)
	{
	case DPHNDSHK_IDOL_SEQ:	/* 0x0000 */
	case DPHNDSHK_ASYNC_SEQ:/* 0xAAAA */
		*s_setup = c_setup;
		break;
	case DPHNDSHK_START_SEQ:/* 0x2222 */
		*s_setup = c_setup;
// <V881>
		KPI_DI();
		KPI_SYNC_CLEAR_WITHOUT_DI_EI();	/* IntSync同期解除 */
		pDpmCtl->hndshk.CycCtrl = 0;	/* 定周期制御状態クリア */
		KPI_EI();
		rc = DPHNDSHK_START_SEQ;
		break;
	case DPHNDSHK_SYNC_SEQ:	/* 0xCCCC */
		if((pDpmCtl->hndshk.CycCtrl & DPCYC_SYNC_REQ) == 0)
		{/* 同期要求未発行状態 -> IntSync同期要求発行 */ 
			KPI_SYNC_REQUEST( pDpmCtl->pSvOptIf->OptSetting.CycTime );
#if CCSW_NETID == NETID_CMDPM
			KPI_SET_MIN_INTSYNC_INTERVAL(pDpmCtl->pSvInfo->SvCycTime);	/* <V779> */
#endif
			pDpmCtl->wdErrCnt = DPM_WDCCHK_CNT;
			pDpmCtl->hndshk.CycCtrl = DPCYC_SYNC_REQ;
		}
		else
		{/* 同期要求発行状態 */
			if((pDpmCtl->hndshk.CycCtrl & DPCYC_WDC_ENABLED) != 0)
			{/* 同期完了なら次へ */
				*s_setup = c_setup;
				rc = DPHNDSHK_SYNC_SEQ;
			}
		}
		break;
	default:
		*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		break;
	}
	return rc;
}

/****************************************************************************
 DPM Handshake処理(定周期データ交換開始状態)
****************************************************************************/
static USHORT dpmSyncState(DPMCTRLIF *pDpmCtl, USHORT c_setup, USHORT *s_setup)
{
	USHORT rc = DPHNDSHK_SYNC_SEQ;

	*s_setup = c_setup;
	switch(c_setup)
	{
	case DPHNDSHK_IDOL_SEQ:	/* 0x0000 */
	case DPHNDSHK_SYNC_SEQ:	/* 0xCCCC */
		break;
	case DPHNDSHK_START_SEQ:/* 0x2222 */
		if(pDpmCtl->pSvOptIf->CycRsp.Seq.ServoOnComp != FALSE)
		{
			*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		}
		else
		{/* Clear Sync Status */	
// <V881>
			KPI_DI();
			KPI_SYNC_CLEAR_WITHOUT_DI_EI();		/* IntSync同期解除 */
			pDpmCtl->hndshk.CycCtrl = 0;		/* 定周期制御状態クリア */
			KPI_EI();
			KpiRstLongTimer(&pDpmCtl->timerCnt);/* シーケンス監視タイマクリア */
			rc = DPHNDSHK_START_SEQ;
		}
		break;
	case DPHNDSHK_ASYNC_SEQ:/* 0xAAAA */
		if(pDpmCtl->pSvOptIf->CycRsp.Seq.ServoOnComp != FALSE)
		{
			*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		}
		else
		{
		/* Set Servo Infomation */
			if( dpmSetSvInfomationData(pDpmCtl) != OK )
			{
				*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
			}
			else
			{/* Clear Sync Status */
// <V881>
				KPI_DI();
				KPI_SYNC_CLEAR_WITHOUT_DI_EI();		/* IntSync同期解除 */
				pDpmCtl->hndshk.CycCtrl = 0;		/* 定周期制御状態クリア */
				KPI_EI();
				KpiRstLongTimer(&pDpmCtl->timerCnt);/* シーケンス監視タイマクリア */
			/* Get Option Card Information */
				dpmGetOptInfomationData(pDpmCtl);
				rc = DPHNDSHK_ASYNC_SEQ;
			}
		}
		break;
	default:
		*s_setup = (DPHNDSHK_CMDERR | (c_setup & DPHNDSHK_CMDERR_MSK));
		break;
	}
	return rc;
}


#ifdef	__ghs__
#pragma ghs section text=default
#endif

/****************************************************************************
****************************************************************************/
/* Private Cyclic Functions */
/****************************************************************************
****************************************************************************/

/****************************************************************************
 Handshake データ更新処理(CS Control Flag)
****************************************************************************/
static void dpmUpdateCSHndshkFlag(NONCYC_STATE *ctrl, VUSHORT *pFlag)
{
	USHORT			dpVal = *pFlag;		/* DPM->RAM */
	NONCYC_STATE	state = *ctrl;

	if(state == Enum_DisableState)
	{/* CSデータ交換無効状態 */
		;
	}
	else if(state == Enum_IdolState)
	{/* IDOL状態 */
		if((dpVal & DPNOCYC_RES_REQ) != 0)
		{/* リクエスト受信 */
			dpVal = (DPNOCYC_RES_REQ | DPNOCYC_RES_BSY | DPNOCYC_ENABLED);
			*ctrl = Enum_RecvReqState;	/* リクエスト受信状態セット */
		}
		else
		{/* リクエスト未受信 */
			dpVal = dpVal | DPNOCYC_ENABLED;
		}
	}
	else
	{/* 完了待ち状態 */
		if(state == Enum_ReqCompState)
		{/* 処理完了 */
			dpVal = (DPNOCYC_RES_COMP | DPNOCYC_ENABLED);
			*ctrl = Enum_IdolState;		/* IDOL状態セット */
		}
		else
		{/* 処理中 */
			dpVal = (DPNOCYC_RES_REQ | DPNOCYC_RES_BSY | DPNOCYC_ENABLED);
		}
	}
	*pFlag = dpVal;
}

/****************************************************************************
 Handshake データ更新処理(SC Control Flag)
****************************************************************************/
static void dpmUpdateSCHndshkFlag(NONCYC_STATE *ctrl, VUSHORT *pFlag)
{
	USHORT			dpVal = *pFlag;		/* DPM->RAM */
	NONCYC_STATE	state = *ctrl;

	if(state == Enum_RecvReqState)
	{/* 処理要求受信 */
		*ctrl = Enum_WaitCompState;
		*pFlag = (USHORT)(dpVal & DPNOCYC_ENABLED) | DPNOCYC_RES_REQ;
	}
	else if(state == Enum_WaitCompState)
	{/* 完了待ち状態 */
		if((dpVal & DPNOCYC_RES_REQ) == 0)
		{
			*ctrl = Enum_ReqCompState;
		}
	}
	else
	{/* IDOL,DISABLE,処理完了状態 */
		if((dpVal & DPNOCYC_ENABLED) != 0)
		{	*ctrl = Enum_IdolState;}
		else
		{	*ctrl = Enum_DisableState;}
	}
}

/****************************************************************************
 Cyclic Command データリード
****************************************************************************/
static USHORT dpmReadCycData(void* pRamBuf, void* pDpmBuf, USHORT chksum)
{
	union{
		ULONG	ulVal;
		USHORT	usVal[2];
	} cycBuf;
	ULONG* pBuf = (ULONG*)pRamBuf;
	VULONG* pDpmCycBuf = (VULONG*)pDpmBuf;

	cycBuf.ulVal = pDpmCycBuf[0]; pBuf[0] = cycBuf.ulVal;
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[1]; pBuf[1] = cycBuf.ulVal;
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[2]; pBuf[2] = cycBuf.ulVal;	/* Position */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[3]; pBuf[3] = cycBuf.ulVal;	/* Speed */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[4]; pBuf[4] = cycBuf.ulVal;	/* Torque */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[5]; pBuf[5] = cycBuf.ulVal;	/* SelMon1/2/3/4 */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[6]; pBuf[6] = cycBuf.ulVal;	/* SelMon5/6/7/8 */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[7]; pBuf[7] = cycBuf.ulVal;	/* OpAlarm */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[8]; pBuf[8] = cycBuf.ulVal;	/* ExtCmd1 */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[9]; pBuf[9] = cycBuf.ulVal;	/* ExtCmd2 */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[10]; pBuf[10] = cycBuf.ulVal;	/* ExtCmd3 */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pDpmCycBuf[11]; pBuf[11] = cycBuf.ulVal;	/* ExtCmd4 */
	chksum += (cycBuf.usVal[0] + cycBuf.usVal[1]);

	return chksum;
}

/****************************************************************************
 Cyclic Responce データライト
****************************************************************************/
static USHORT dpmWriteCycData(void* pRamBuf, void* pDpmBuf, USHORT wrCycle)
{
	union{
		ULONG	ulVal;
		USHORT	usVal[2];
	} cycBuf;
	ULONG* pBuf = (ULONG*)pRamBuf;
	VULONG* pDpmCycBuf = (VULONG*)pDpmBuf;
	USHORT sum = 0;

	cycBuf.ulVal = pBuf[0]; pDpmCycBuf[0] = cycBuf.ulVal;
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pBuf[1]; pDpmCycBuf[1] = cycBuf.ulVal;
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pBuf[2]; pDpmCycBuf[2] = cycBuf.ulVal;	/* FbPosition */
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pBuf[3]; pDpmCycBuf[3] = cycBuf.ulVal;	/* Mon1 */
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pBuf[4]; pDpmCycBuf[4] = cycBuf.ulVal;	/* Mon2 */
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pBuf[5]; pDpmCycBuf[5] = cycBuf.ulVal;	/* Mon3 */
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	cycBuf.ulVal = pBuf[6]; pDpmCycBuf[6] = cycBuf.ulVal;	/* Mon4 */
	sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);

	if(wrCycle >= DPM_TIO_BASE_TIME)
	{/* DPM周期62.5us以上の場合のみモニタ5/6/7/8を更新 */
		cycBuf.ulVal = pBuf[7]; pDpmCycBuf[7] = cycBuf.ulVal;	/* Mon5 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
		cycBuf.ulVal = pBuf[8]; pDpmCycBuf[8] = cycBuf.ulVal;	/* Mon6 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
		cycBuf.ulVal = pBuf[9]; pDpmCycBuf[9] = cycBuf.ulVal;	/* Mon7 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
		cycBuf.ulVal = pBuf[10]; pDpmCycBuf[10] = cycBuf.ulVal;	/* Mon8 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);

		cycBuf.ulVal = pBuf[11]; pDpmCycBuf[11] = cycBuf.ulVal;	/* SelMon1/2/3/4 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
		cycBuf.ulVal = pBuf[12]; pDpmCycBuf[12] = cycBuf.ulVal;	/* SelMon5/6/7/8 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	}
	else
	{/* DPM周期62.5usの場合モニタ1/2/3/4のみ更新 */
		cycBuf.ulVal = pBuf[11]; pDpmCycBuf[11] = cycBuf.ulVal;	/* SelMon1/2/3/4 */
		sum += (cycBuf.usVal[0] + cycBuf.usVal[1]);
	}

	return (~sum + 1);
}

