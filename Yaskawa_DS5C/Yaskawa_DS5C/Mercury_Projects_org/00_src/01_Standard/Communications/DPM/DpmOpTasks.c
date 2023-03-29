/****************************************************************************/
/*																			*/
/*		DPM Option Task Control ソースファイル								*/
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


/* includes */
#include "KnlStd.h"
#include "XlsTblM2Opt.h"
#include "Dpm.h"
#include "HardSw.h"
#include "NetMsgDrv.h"
#include "OpTasks.h"

#ifdef __cplusplus
extern	"C"	{
#endif

/* globals : LOCAL */
#if defined(__ghs__)
#pragma ghs startzda
#elif !defined(_WIN32)
#pragma section sidata begin
#endif

static DPMCTRLIF	DpmCtrl;
static LONG			bScanAStop;

#if defined(__ghs__)
#pragma ghs endzda
#elif !defined(_WIN32)
#pragma section sidata end
#endif
/* forward declarations */
/* Public functions of Option-module main tasks */
//void DPiInitialize(void);
//void DPiStartDevice(void);
//void DPiIntSync(void);
//void DPiPreCtrlProcedure(void);
//void DPiPostCtrlProcedure(void);
//void DPiScanCProcedure(void);
//void DPiRoundProcedure(void);
/* Public functions API of Option-module message driver */
//LONG DPiReqRecvCSMsg(UCHAR ch);
//LONG DPiGetRecvCSMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz);
//void DPiReadCSMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz);
//void DPiWriteCSMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz);
//LONG DPiReqSendCSMsg(UCHAR ch);
//LONG DpiGetSendCSMsgSts(UCHAR ch);
/* DPM DegOp. operation API */
//LONG DPiGetDegOpState(void);
//LONG DPiReqDegOpPrmRead(USHORT cmd, USHORT no);
//LONG DPiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data);
//LONG DPiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data);
/* DPM SC Message. operation API */
//LONG DPiGetSCMsgState(void);
//LONG DPiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz);
//LONG DPiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz);

#ifdef __cplusplus
}
#endif


/****************************************************************************
****************************************************************************/
/* Public functions of Option-module main tasks */
/****************************************************************************
****************************************************************************/

#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
/****************************************************************************
 DPM Optionモジュール初期化処理
****************************************************************************/
void DPiInitialize(void)
{
/* サーボIFデータ初期化 */
	MlibResetLongMemory(&SvOptIf, sizeof(SV_OPTION_IF)/4 );

/* レジスタIF初期化 */
	SvOptIf.Regif = &NetRegif;
	SvOptIf.Msgif = &NetMsgif;
	OpiInitRegAcc();

/* DPMモジュール初期化 */
	DpmInitialize(&DpmCtrl, &SvOptIf);

/* LED 表示モジュール初期化 */
	HwInitialize();

	bScanAStop = FALSE;
}


/****************************************************************************
 DPM Optionモジュールデバイススタートアップ処理
****************************************************************************/
void DPiStartDevice(void)
{
/* 割り込み有効 */
#ifdef KNLDEF_SYNC_PIN_OLD
	KPX_CLR_INTMSK( CPU_ICR_DPSYNC );	/*IntSync割込みマスク解除	*/
#endif
	KPX_CLR_INTMSK( CPU_ICR_DPSYNCR );	/*IntSync割込みマスク解除	*/

}


/****************************************************************************
 DPM OptionモジュールRound処理
****************************************************************************/
void DPiRoundProcedure(void)
{
/* DPMイニシャルシーケンス処理 */
	if(DpmHandshake(&DpmCtrl) != DPM_FUNC_SUCCESS)
	{/* Handshake異常 A.E00 */
		KPI_ALARMSET(ALM_OPCARD_TOUT);
		/* オプション検出情報(E51A)に通信異常(7)をセット*/
		KpiSetOptDetectInfo( CARDTYPE_CMPOP, STS_DET_ERR );
	}

/* DPMリンクデータ交換処理 */
	DpmLinkRegAcc(&DpmCtrl);
}
#ifdef	__ghs__
#pragma ghs section text=default
#endif


/****************************************************************************
 DPM OptionモジュールIntSync処理
****************************************************************************/
void DPiIntSync(void)
{
	LONG rc;
/* Handshakeフラグ更新 */
	DpmUpdateHndshkFlag(&DpmCtrl);

/* サイクリックデータ取得 */
	rc = DpmReadCycCMD(&DpmCtrl);
	if (rc != DPM_FUNC_SUCCESS)
	{
		if(rc == DPM_CHKSUM_ERR)
		{	KPI_ALARMSET(ALM_DPM_CHKSUM);/* SUM異常 A.E03 */}
		else
		{	KPI_ALARMSET(ALM_WDC_ERR);	/* 同期異常 A.E02 */}
	}

/* ScanA停止時処理 */
	if(DpmCtrl.pSvOptIf->SvInfo.IOCycle < DPM_TIO_BASE_TIME)
	{
		DpmWriteNonCycRSP(&DpmCtrl);
		if((!KnlV.f.ScanAEnable) || bScanAStop)
		{/* ScanA停止 */
			bScanAStop = (KnlV.f.ScanAEnable != FALSE) ? FALSE: TRUE;
			if((!DpmCtrl.pSvOptIf->CtrlReq.SyncChg) && bScanAStop)
			{
				DpmCtrl.pSvOptIf->CtrlReq.SyncChg = TRUE;	/* WDCのみ更新 */
				DpmCtrl.pSvOptIf->CtrlReq.RspUpdate = TRUE;

				ApxAinputNetIf( );	/* ネットワークＩＦ入力処理(62.5us化対応)	*/

				DpmCtrl.pSvOptIf->CtrlReq.CmdUpdate = FALSE;
//				DpmCtrl.pSvOptIf->CtrlReq.RspUpdate = FALSE;
				DpmCtrl.pSvOptIf->CtrlReq.SyncChg = FALSE;
			}
			DpmResetTioCnt(&DpmCtrl);	/* スケジュールカウンタリセット */

			return;	/* 強制リターン */
		}
	}

/* スケジューラリセット＆同期遷移 */
	if(DpmResetScheduler(&DpmCtrl) != DPM_FUNC_SUCCESS)
	{/* Error */
		KPI_ALARMSET(ALM_WDC_ERR);	/* A.E02 */
	}
}


/****************************************************************************
 DPM OptionモジュールScanB先頭処理
****************************************************************************/
void DPiPreCtrlProcedure(void)
{
/* データ更新要求作成 */
	if( DpmMakeCycDataUpdateReq(&DpmCtrl) != DPM_FUNC_SUCCESS)
	{/* Error */
		KPI_ALARMSET(ALM_WDC_ERR);	/* A.E02 */
	}
}

/****************************************************************************
 DPM OptionモジュールScanB後処理
****************************************************************************/
void DPiPostCtrlProcedure(void)
{
/* サイクリックデータライト */
	DpmWriteCycRSP(&DpmCtrl);

/* Tioスケジュールカウンタ更新 */
	DpmUpdateTioCnt(&DpmCtrl);
}

/****************************************************************************
 DPM OptionモジュールScanC処理
****************************************************************************/
void DPiScanCProcedure(void)
{
	USHORT sts = 0;
	USHORT almCode = 0;
	USHORT wrk;

/* LED表示 */
#if defined(MCCSW_SGDVCMOP)	//<V684>
	if(BoutV.CtrlMode.b.cm != CTRLMODE_JOG && BoutV.CtrlMode.b.cm != CTRLMODE_ZSRCH)
	{
		sts |= ((USHORT)CoutV.f.Sv.Pot << LED_POT_BNO);
		sts |= ((USHORT)CoutV.f.Sv.Not << LED_NOT_BNO);
		sts |= ((USHORT)CoutV.f.Sv.Pls << LED_PLS_BNO);
		sts |= ((USHORT)CoutV.f.Sv.Nls << LED_NLS_BNO);
	}
#else
	sts |= ((USHORT)CoutV.f.Sv.Pot << LED_POT_BNO);
	sts |= ((USHORT)CoutV.f.Sv.Not << LED_NOT_BNO);
#endif

#if ( CCSW_SFTID == SFTID_EXIST )																	  /*<V717>Add:Start*/
	sts |= (DpmCtrl.pSvOptIf->SomInfo & 0x01) ? 1 << LED_SFON_BNO : 0x0000;
	sts |= (DpmCtrl.pSvOptIf->SomInfo & 0x02) ? 1 << LED_SAFE_BNO : 0x0000;
	sts |= (DpmCtrl.pSvOptIf->SomInfo & 0x10) ? 1 << LED_ACT_BNO  : 0x0000;							  /*<V729>Add*/
#endif																								  /*<V717>Add:End  */
	sts |= ((USHORT)DpmCtrl.pSvOptIf->CycRsp.InSig.EMG_Sig << LED_HBB_BNO);

	wrk = (USHORT)(((DpmCtrl.State == DPHNDSHK_SYNC_SEQ) && (DpmCtrl.wdErrCnt <= 0)) ? 0x1: 0x0);
	sts |= (wrk << LED_CNCT_BNO);

	sts |= ((USHORT)DpmCtrl.pSvOptIf->CycRsp.Seq.ServoOnComp << LED_SVON_BNO);
	sts |= ((USHORT)DpmCtrl.pSvOptIf->CycRsp.Ctrl.MotorMoving << LED_TGON_BNO);

	wrk = ((USHORT)CoutV.f.Sv.Prefon | (USHORT)CoutV.f.Sv.Vrefon | (USHORT)CoutV.f.Sv.Trefon);
	wrk = (USHORT)(~wrk) & 0x1;
	sts |= (wrk << LED_DEN_BNO);

	sts |= (((CMDOP_NOT_CONNECTED == KPI_GET_CMDOP_TYPE()) ? 1: 0) << LED_OP_DISABLE_BNO); //<DPM_V103>

	if(!KpiGetNetFirstAlarmInfo(&almCode))
	{	KpiGetNetFirstWarningInfo(&almCode);}

	HwDisplayLedData(sts, almCode);
}


/****************************************************************************
****************************************************************************/
/* Public function APIs of Option-module CS message driver */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif

/*****************************************************************************
	CSメッセージ受信開始要求セット
*****************************************************************************/
LONG DPiReqRecvCSMsg(UCHAR ch)
{
	LONG rc = ND_SUCCESS;
	if(ch == 1)
	{
		rc = DpmReqRecvCSMsg(&DpmCtrl);
	}
	return rc;
}

/*****************************************************************************
	CSメッセージ受信完了チェック
*****************************************************************************/
LONG DPiGetRecvCSMsgSts(UCHAR ch, USHORT *rcvWSz, USHORT *rcvBSz)
{
	LONG rc = ND_RECV_BUSY;
	if(ch == 1)
	{
		rc = DpmGetRecvCSMsgSts(&DpmCtrl, rcvWSz, rcvBSz);
	}
	return rc;
}

/*****************************************************************************
	CSメッセージデータリード
	引数：rcvData	・・・受信データ格納先, rdSz	・・・リードワード数
*****************************************************************************/
void DPiReadCSMsgData(UCHAR ch, USHORT *rcvData, USHORT rdSz)
{
	if(ch == 1)
	{
		DpmReadCSMsgData(&DpmCtrl, rcvData, rdSz);
	}
}

/*****************************************************************************
	CSメッセージデータライト
	引数：	sndData	・・・送信データ格納先, wrSz	・・・ライトByte数
*****************************************************************************/
void DPiWriteCSMsgData(UCHAR ch, USHORT *sndData, USHORT wrSz)
{
	if(ch == 1)
	{
		DpmWriteCSMsgData(&DpmCtrl, sndData, wrSz);
	}
}

/*****************************************************************************
	CSメッセージ送信開始要求セット
*****************************************************************************/
LONG DPiReqSendCSMsg(UCHAR ch)
{
	LONG rc = ND_RECV_BUSY;
	if(ch == 1)
	{
		rc = DpmReqSendCSMsg(&DpmCtrl);
	}
	return rc;
}

/*****************************************************************************
	CSメッセージ送信完了チェック
*****************************************************************************/
LONG DpiGetSendCSMsgSts(UCHAR ch)
{
	return ND_SUCCESS;
}

/*****************************************************************************
	CSメッセージ送信キャンセル<DPM_V103>
*****************************************************************************/
LONG DpiSendMsgAbort(UCHAR ch)
{
	LONG rc = ND_SUCCESS;
	if(ch == 1)
	{
		rc = DpmSendMsgAbort(&DpmCtrl);
	}
	return rc;
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif


/****************************************************************************
****************************************************************************/
/* Public function APIs of Option-module DegOp. driver */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
LONG DPiGetDegOpState(DEGOP_PRMRNG *pPrmRange)
{
	return DpmGetDegOpState(&DpmCtrl, pPrmRange);
}

LONG DPiReqDegOpPrmRead(USHORT cmd, USHORT no)
{
	return DpmReqDegOpPrmRead(&DpmCtrl, cmd, no);
}

LONG DPiReqDegOpPrmWrite(USHORT no, USHORT size, LONG data)
{
	return DpmReqDegOpPrmWrite(&DpmCtrl, no, size, data);
}

LONG DPiGetDegOpPrmValue(USHORT *no, DEGOP_ATTR *Attr, LONG *data)
{
	return DpmGetDegOpPrmValue(&DpmCtrl, no, Attr, data);
}
/*------------ <V685> Add Begin -------------------------------------------*/
USHORT DpiGetDegOpNumOfFn(void)
{
	return (DpmCtrl.pSvOptIf->OptSetting.NumOfFnMode);
}

LONG DpiReqDegOpFn(USHORT cmd, USHORT idx, USHORT data)
{
	return DpmReqDegOpFn(&DpmCtrl, cmd, idx, data);
}

LONG DpiGetDegOpFn(DGOP_RSP *pDgopRsp, DGOPEX_RSP *pDgopExRsp)
{
	return DpmGetDegOpFn(&DpmCtrl, pDgopRsp, pDgopExRsp);
}
/*------------ <V685> Add End ---------------------------------------------*/
#ifdef	__ghs__
#pragma ghs section text=default
#endif

/****************************************************************************
****************************************************************************/
/* Public function APIs of Option-module SC message driver */
/****************************************************************************
****************************************************************************/
#ifdef	__ghs__
#pragma ghs section text=".textEX"
#endif
LONG DPiGetSCMsgState(void)
{
	return DpmGetSCMsgState(&DpmCtrl);
}
LONG DPiReqSCMsgTrans(USHORT *pMsgBuf, LONG bSz)
{
	return DpmReqSCMsgTrans(&DpmCtrl, pMsgBuf, bSz);
}
LONG DPiGetSCMsgRspData(USHORT *pMsgBuf, LONG *bSz)
{
	return DpmGetSCMsgRspData(&DpmCtrl, pMsgBuf, bSz);
}
void DPiCancelSCMsgReq(void)
{
//	DpmCancelSCMsgReq(&DpmCtrl);
}

#ifdef	__ghs__
#pragma ghs section text=default
#endif
