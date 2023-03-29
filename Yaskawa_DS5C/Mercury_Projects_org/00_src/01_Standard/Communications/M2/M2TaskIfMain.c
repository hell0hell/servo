/****************************************************************************
Description	: MECHATROLINK Task Interface Procedure
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2010.10.18     created

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "M2TaskIfMain.h"
#include "ComManager.h"
#include "CmdAnalyzer.h"
#include "RspManager.h"
#include "ServoIf.h"
#include "SysApi.h"
#include "ComAlmMngr.h"
#include "RegTable.h"
#include "AlarmManager.h"
#include "HwDeviceIf.h"
#include "MLib.h"
#include "KLib.h"
#include "DMAdrv.h"


/****************************************************************************
 ネットワークモジュール初期化処理
****************************************************************************/
VOID OpiInitialize(void *axRsc)
{
	LONG			ax_no;
	MLNET_HANDLE	*MLAxisHandle;
	ML_NODE_COMMONS	*NodeCommons;
	UNI_PRM_STR		*prm_data_ptr;
	SV_OPTION_IF	*SvOptIf;
	AXIS_HANDLE *AxisRsc = 	(AXIS_HANDLE*)axRsc;

	/* ローカル変数の初期化 */
	NodeCommons = AxisRsc->NetNodeIf;
	NodeCommons->Tcyc_RspTiming = 0;
	NodeCommons->Tcyc_Counter = 0;
	NodeCommons->SyncState = ML_HWSYNC_DISABLE;
	NodeCommons->ComResetSeq = ML_COMRST_NONE;
	
	for( ax_no = 0 ; ax_no < MAX_AXIS_NUMBER; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		prm_data_ptr = AxisRsc[ax_no].UniPrms;
		SvOptIf = &((AxisRsc[ax_no].SvControlIf)->CmdOptionIf);

		MLAxisHandle->NodeCommons = NodeCommons;
		
		/* M2 Component初期化 */
		ComInit(&MLAxisHandle->com_hdl, ax_no);
		CmdInit(&MLAxisHandle->cmd_hdl);
		RspInit(&MLAxisHandle->rsp_hdl);

		/* Servo IF初期化 */
		MlibResetByteMemory( SvOptIf, sizeof(SV_OPTION_IF) );
		SvOptIf->CycCtrl.CmdUpdateCycle = 1;
		SvOptIf->CycCtrl.CmdUpdate = TRUE;
		SvOptIf->CycCtrl.RspUpdate = TRUE;
		
		/* Motion Component初期化 */
		MtcInit( &MLAxisHandle->mt_hdl, SvOptIf );

		/* System Component初期化 */
		RegInit( &MLAxisHandle->reg_hdl,
				 &MLAxisHandle->mt_hdl.mtPrm,
				 &MLAxisHandle->com_hdl.NetPrm, prm_data_ptr->Prm,
				 AxisRsc[ax_no].RegManager );

		AlmInit( &MLAxisHandle->alm_hdl, AxisRsc[ax_no].AlmManager );
	}
}

/****************************************************************************
 ネットワークモジュール初期パラメータ計算処理
****************************************************************************/
VOID OpiCalcInitParam( VOID *axRsc )
{
	MLNET_HANDLE	*MLAxisHandle;
	UNI_PRM_STR		*prm_data_ptr;
	AXIS_HANDLE *AxisRsc = 	(AXIS_HANDLE*)axRsc;

	/* ローカル変数の初期化 */
	MLAxisHandle = AxisRsc->NetAxisIf;
	prm_data_ptr = AxisRsc->UniPrms;

	/* サーボパラメータ情報ロード */
	MLAxisHandle->reg_hdl.iniPrm.mt.LinearMotor =
			(AxisRsc->MencV->AxisMotType == MOTTYPE_LINEAR);						//暫定
	MLAxisHandle->reg_hdl.iniPrm.mt.FullcCtrl = (AxisRsc->UniPrms->Bprm)->FencUse;	//暫定

	if( RegLoadDefaultVal( &(MLAxisHandle->reg_hdl), prm_data_ptr ) != REG_SUCCESS )
	{
		AlmSetCode( &(MLAxisHandle->alm_hdl), AxisRsc->AxisNo, ERR_COM_PRM );
	}

	/* 通信パラメータ設定 */
	if( RegCalcDefaultVal(&(MLAxisHandle->reg_hdl) ) != REG_SUCCESS )
	{
		AlmSetCode( &(MLAxisHandle->alm_hdl), AxisRsc->AxisNo, ERR_COM_PRM );
	}
}

/****************************************************************************
 ネットワークモジュールデバイス初期化処理
****************************************************************************/
VOID OpiInitDevice(void *axRsc, void *DeviceAddress)
{
	;
}


/****************************************************************************
 ネットワークモジュールDMAデバイス初期化処理
****************************************************************************/
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
BOOL mlOpiInitDMADevice( VOID *axRsc, UINT AxNum, UINT t_byte )
{
	UINT				i, j;
	MLNET_HANDLE		*MLAxisHandle;
	DMA_ADDR_MAP		DmaMap;
	ULONG				RcvAltBufAddr[2];
	ULONG				SndAltBufAddr[2];
	AXIS_HANDLE *AxisRsc = 	(AXIS_HANDLE*)axRsc;

	for(j = 0; j < 2; j++)
	{
		/* Toggle buffer */
		if(0 != NdChangeTglBufAddress(&RcvAltBufAddr[j], &SndAltBufAddr[j], j))
		{	return TRUE;}

		for(i = 0; i < AxNum; i++)
		{
			MLAxisHandle = AxisRsc[i].NetAxisIf;
#if 1
			/* Sets the DMA Link descriptor for Inputs */
			DmaMap.DstAddr = NdGetRecvCycBufferPointer(MLAxisHandle->com_hdl.my_ax_number);
			DmaMap.SrcAddr = NdGetRecvCycDevBufAddress(MLAxisHandle->com_hdl.my_ax_number);
			DmaMap.Length  = t_byte;
			DmaMap.NextLink= DMA_MLDEV_IN_LINK(j)+i+1;
			if(i == (AxNum-1))
			{	DmaMap.NextLink = 0x100;}

			DMAdrvSetLinkDescriptor(&DmaMap, 1, DMA_MLDEV_IN_LINK(j)+i);
#endif
			/* Sets the DMA Link descriptor for Outputs */
			DmaMap.DstAddr = NdGetSndCycDevBufAddress(MLAxisHandle->com_hdl.my_ax_number);
			DmaMap.SrcAddr = NdGetSndCycBufferPointer(MLAxisHandle->com_hdl.my_ax_number);
			DmaMap.Length  = t_byte;
			DmaMap.NextLink= DMA_MLDEV_OUT_LINK(j)+i+1;
			if(i == (AxNum-1))
			{	DmaMap.NextLink = 0x100;}

			DMAdrvSetLinkDescriptor(&DmaMap, 1, DMA_MLDEV_OUT_LINK(j)+i);
		}
	}

	/* Initialize the DMA device driver */
#if (DMA_SVDEV_CH != DMA_MLDEV_CH)
	DMAdrvInitialize(DMA_MLDEV_CH);
#endif

	return FALSE;
}
#endif

/****************************************************************************
 ネットワークモジュールデバイススタートアップ処理
****************************************************************************/
VOID OpiStartDevice( VOID *axRsc, void *DeviceAddress )
{
	INT32			rc;
	MLNET_HANDLE	*MLAxisHandle;
	ND_PARAM		nd_prm;
	LONG			ax_no;
	UCHAR			rSw, dSw;
	
	/* ローカル変数の初期化 */
	AXIS_HANDLE *AxisRsc = 	(AXIS_HANDLE*)axRsc;

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		MLAxisHandle->com_hdl.my_ax_number = AxisRsc[ax_no].AxisNo + 1;
	}

	/* 局アドレス、伝送バイト数および伝送速度は、RSW/DSWから取得	*/
	HALdrv_GetNetworkHardPrm(&nd_prm.DataSize,
							  &nd_prm.TSpeed,
								&nd_prm.NodeAddress);
								
	/* 伝送速度に合わせた設定を実施	*/
	if(nd_prm.TSpeed == 4)
	{
		nd_prm.TCycle = COM_DEF_M1TCYC;	/* 伝送周期2ms [0.25us] */
		nd_prm.TSlot = 520;				/* 130us タイムスロット[0.25us] */
		nd_prm.DataSize = 17;			/* 伝送バイト数 */
	}
	else
	{
		nd_prm.TCycle = COM_DEF_M2TCYC;						/* 伝送周期4ms [0.25us] */
		nd_prm.TSlot = (nd_prm.DataSize == 32) ? 360: 240;	/* タイムスロット[0.25us] */
	}
	
	/* 通信モードパラメータ設定	*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		ComNetPrmSet(&MLAxisHandle->com_hdl, nd_prm);
	}
	/* 通信デバイスの初期化 */
	rc = NdInit( &nd_prm, DeviceAddress );
	if(rc == ND_SUCCESS)
	{/* 初期化正常終了 */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			AxisRsc[ax_no].NetAxisIf->com_hdl.Phase = 1;	/* 初期化が完了したらPhase1セット */
		}
	}
	else
	{/* ASIC初期化異常 */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			AlmSetCode( &MLAxisHandle->alm_hdl, ax_no, ERR_COM_LSISET );
		}
	}

	/* 通信設定情報登録 */
	RegSetComPrmMon( &(MLAxisHandle->reg_hdl), nd_prm.NodeAddress, nd_prm.DataSize );

	/* INTUDL4をM2割込みに設定する。	*/
	SHalDrv_Enable_MEHCATROLINK2Interrupt(AxisRsc->SvAsicRegs->AsicHwReg);
	
	
/* DMAデバイスの初期化 */
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
	mlOpiInitDMADevice(AxisRsc, NumberOfAxes, nd_prm.DataSize);
#endif
}

#define GBL_FRAM_ERROR_COUNT		64		// max. 32cycle x 2
/****************************************************************************
 ネットワークモジュール同期合わせ処理(called from OpiIntSync)
****************************************************************************/
BOOL mlSyncStateControls( VOID *axRsc, ML_NODE_COMMONS *NodeCommons, BOOL GblState )
{
	LONG			ax_no, sv_on;
	BOOL			SyncDisable;
	MLNET_HANDLE	*MLHandle;
	AXIS_HANDLE *AxisRsc;
	ASIC_HW_IF_STR *pAsicHwReg;
	
	/* ローカル変数の初期化	*/
	AxisRsc = 	(AXIS_HANDLE*)axRsc;
	pAsicHwReg = AxisRsc->SvAsicRegs->AsicHwReg;

	switch(NodeCommons->SyncState)
	{
	/* No synchronizing state  */
	case ML_HWSYNC_DISABLE:
		SyncDisable = TRUE;
		break;

	/* Hardware synchronizing State */
	case ML_HWSYNC_ENABLE:
		SyncDisable = FALSE;
		if(FALSE == HALdrv_GetIntUDLStatus(INTERRUPT_ScanB,  AxisRsc->SvAsicRegs->AsicHwReg))
		{	/* A.E61 will be set if FPGA becomes unsynchrozing state. */
			for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
			{
				MLHandle = AxisRsc[ax_no].NetAxisIf;
				if(MLHandle->com_hdl.Phase > 1)
				{
					AlmSetCode( &MLHandle->alm_hdl, ax_no, ERR_GBL_SYNC );
					ComReqPhaseDown( &MLHandle->com_hdl, 2 );
				}
			}
			NodeCommons->SyncState = ML_HWSYNC_RESET;
			SyncDisable = TRUE;
		}
		break;

	/* Starting state in phase 0. */
	case ML_HWSYNC_START:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		if(FALSE != GblState)
		{
			KlibRstLongTimer(&NodeCommons->M2IntCounter);
			NodeCommons->SyncState = ML_HWSYNC_WAIT_TIME;
		}
		break;

	/* Waiting state until M3 interrupt is stabilized. */
	case ML_HWSYNC_WAIT_TIME:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
//		if(NodeCommons->SyncWaitTime < KlibGetLongTimerMs(NodeCommons->M2IntCounter))
		if(	SYNC_WAIT_TIME < KlibGetLongTimerMs(NodeCommons->M2IntCounter) )			/* <S052> */
		{
			NodeCommons->SyncState = ML_HWSYNC_RESET;
		}
		break;

	/* Hardware Resetting state */
	case ML_HWSYNC_RESET:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		HALdrv_DisableExtSyncInterrupt(pAsicHwReg);
		NodeCommons->M2IntCounter = 0;
		NodeCommons->SyncState = ML_HWSYNC_ENCODER;
		break;

	/* Activating hardware synchronization state */
	case ML_HWSYNC_ENCODER:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/

		sv_on = 0;
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			MLHandle = AxisRsc[ax_no].NetAxisIf;
			sv_on |= ((MLHandle->mt_hdl.svRsp)->RspSeqBit & SERVO_ON_COMP_BITMSK);
		}

		/* if all axes are in Servo-off state and the Sync frame was received 4 times,
		 *  then start to synchronization */
		if((FALSE != GblState) && (sv_on == 0))
		{
			NodeCommons->M2IntCounter++;
			if(NodeCommons->M2IntCounter >= 4)
			{
				NodeCommons->SyncState = ML_HWSYNC_INTERRUPT;
				HALdrv_EnableExtSyncInterrupt(pAsicHwReg);
				NodeCommons->M2IntCounter = 0;
			}
		}
		break;

	/* Waiting state for internal tasks. */
	case ML_HWSYNC_INTERRUPT:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		NodeCommons->M2IntCounter++;
		if(NodeCommons->M2IntCounter >= 8)
		{
			NodeCommons->SyncState = ML_HWSYNC_WAIT_COMP;
			SHalDrv_Reset_Intcycle(pAsicHwReg);
			KlibRstLongTimer(&NodeCommons->M2IntCounter);
		}
		break;

	/* Reset ScanTiming state for internal tasks. */
	case ML_HWSYNC_WAIT_COMP:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		if(FALSE != HALdrv_GetExtUnSyncStatus())
		{/* ScanB | ScanC割込み要求なし	*/
			if(KlibGetLongTimerMs(NodeCommons->M2IntCounter) > 28)/* TimeOut[4ms]	*/
			{/* Retry to sync, if synchronization was failed. */
				NodeCommons->SyncState = ML_HWSYNC_RESET;
				for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
				{
					MLHandle = AxisRsc[ax_no].NetAxisIf;
					AlmSetCode( &MLHandle->alm_hdl, ax_no, ALM_COMLSI_ERR1 );
				}
			}
		}
		else
		{/* ScanB & ScanC割込み要求あり	*/
			NodeCommons->SyncState = ML_HWSYNC_ENABLE;
		}
		break;

	default:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		break;
	}

	return SyncDisable;
}


/****************************************************************************
 ネットワークモジュールRound処理
****************************************************************************/
BOOL OpiRoundProcedure(VOID *axRsc)
{
	USHORT			tCycle, tSlot;
	LONG			MLScanTime;
	BOOL			connect;
	LONG			ax_no;
	AXIS_HANDLE 	*AxisRsc;
	MLNET_HANDLE	*MLAxisHandle;
	ML_NODE_COMMONS *NodeCommons;
	COM_HNDL		*com_hdl;

	/* ローカル変数の初期化 */
	AxisRsc 	= (AXIS_HANDLE*)axRsc;
	NodeCommons = AxisRsc->NetNodeIf;
	com_hdl		= &AxisRsc->NetAxisIf->com_hdl;
	tCycle	= 0;
	tSlot	= 0;
	
	/*----------------------------------------------------------------------*/
	/* Communication Reset Procedure in Phase 1								*/
	/*----------------------------------------------------------------------*/
	if(NodeCommons->ComResetSeq != ML_COMRST_START )
	{
		if(NdReceiveGblInfo(&tCycle, &tSlot) == ND_SUCCESS)/* 同期データ(伝送周期)受信	*/
		{
			if(( NodeCommons->ComResetSeq == ML_COMRST_NONE )
				||((com_hdl->NetPrm.tCycle != tCycle) && (com_hdl->Phase == 1)))
			{
				/* ASIC Setting	*/
				com_hdl->NetPrm.tCycle = tCycle;
				com_hdl->NetPrm.tSlot = tSlot;

				if( ComResetAsicProc(&com_hdl->NetPrm) != COM_FUNC_NORMAL_END)/* ASICリセット */
				{/* 伝送周期対象範囲外	*/
					ALMSetGlobalAlarm(ALM_TCYC_ERR);   /* <S0B0> */
				}
				else
				{/* 伝送周期設定OK	*/
					MLScanTime = (AxisRsc->UniPrms->Bprm)->SvCycleUs*4;
					NodeCommons->Tcyc_RspTiming = (tCycle/MLScanTime) - 1;
					NodeCommons->SyncState = ML_HWSYNC_START;
					NodeCommons->ComResetSeq = ML_COMRST_START;
				}
			}
		}
	}
	else
	{
		if(NodeCommons->SyncState == ML_HWSYNC_ENABLE)
		{
			NodeCommons->ComResetSeq = ML_COMRST_DONE;
		}
	}

	/*----------------------------------------------------------------------*/
	/* Background Procedure													*/
	/*----------------------------------------------------------------------*/
	connect = 0;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;

		/* Phase移行バックグランド処理 */
		ComRunPhaseChange( &MLAxisHandle->com_hdl );
		/* レジスタバックグランド処理 */
		RegRunBackgrndProc( &MLAxisHandle->reg_hdl );
		
		connect |= (MLAxisHandle->com_hdl.Phase > 1);
	}

	return connect;
}


/****************************************************************************
 ネットワークモジュールIntSync処理
****************************************************************************/
static BOOL mlOpiIntSync( AXIS_HANDLE *AxisRsc )
{
	BOOL			RxDisabled;
	ULONG			ests;	/* エラーステータス */
	LONG			ax_no;
	MLNET_HANDLE	*MLAxisHandle;
	ML_NODE_COMMONS *NodeCommons;

	/* ローカル変数の初期化 */
	MLAxisHandle = AxisRsc->NetAxisIf;
	NodeCommons = AxisRsc->NetNodeIf;

	/*-----------------------------------*/
	/* 		ASICアクセス処理			 */
	/*-----------------------------------*/
	NdGetDevStatus( &ests ); /* ASIC(ests)状態取得 */

	/* MECHATROLINK-2同期状態制御 */
	RxDisabled = mlSyncStateControls(AxisRsc, NodeCommons, TRUE);

	/* for error status monitor (However,ESTE is 0 in JL077.) */
	RegSetComErrData(&MLAxisHandle->reg_hdl,
						MLAxisHandle->com_hdl.recvErrCntNum, (USHORT)ests);

	return RxDisabled;
}

/****************************************************************************/
BOOL OpiIntSync(VOID *axRsc, BOOL IntStatus)
{
	ML_NODE_COMMONS *NodeCommons;
	AXIS_HANDLE *AxisRsc = GetAxisHandle(0);
	COM_HNDL		*com_hdl;

	/* ローカル変数の初期化 */
	NodeCommons = AxisRsc->NetNodeIf;

	if( 0 == (IntStatus & 1) )
	{/* M2割り込みなし時は強制終了 */
		if( (NodeCommons->Tcyc_Counter > NodeCommons->Tcyc_RspTiming)
			&& (NodeCommons->SyncState == ML_HWSYNC_ENABLE) )
		{/* 本条件では、割り込みが実施されるﾀｲﾐﾝｸﾞである。 */
			NodeCommons->InterruptReq = TRUE;
			/* JL-100アクセス処理(ASIC状態確認/CMDデータ受信/異常検出) */
//			if(FALSE != mlOpiIntSync( AxisRsc ))	/* <S0EC> */
			{/* 非同期時は強制終了 */
				return TRUE;
			}
		}
		else
		{
			NodeCommons->InterruptReq = FALSE;
			return TRUE;
		}
	}
	else
	{/* M2割り込みあり */
		NodeCommons->InterruptReq = TRUE;
		/* JL-100アクセス処理(ASIC状態確認/CMDデータ受信/異常検出) */
		if(FALSE != mlOpiIntSync( AxisRsc ))
		{/* 非同期時は強制終了 */
			return TRUE;
		}
	}
	/* CMDデータ受信 */
	/* 暫定処理：評価基板では、CMD受信をIntSyncで実施する。	*/
	com_hdl = &AxisRsc->NetAxisIf->com_hdl;
	com_hdl->LrecvErr = com_hdl->RecvErr;
	com_hdl->RecvErr = NdReceiveCMD(com_hdl->my_ax_number, com_hdl->CmdBufPtr);
	NdExchangeMsgPkt();					/* Message送受信 */

	NodeCommons->Tcyc_Counter = 0;

	/*----------------------------------------------------------------------*/
	/* Check DMA Transfer statuses											*/
	/*----------------------------------------------------------------------*/
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
	if(FALSE != DMAdrvWaitForTransferEnd(DMA_MLDEV_CH, 200))
	{/* DMA transfer error. */
		ALMSetGlobalAlarm(ALM_WDC_ERR);
	}
#endif

	return FALSE;
}


/****************************************************************************
 ネットワークモジュールサイクリック入力処理
****************************************************************************/
BOOL OpiInputProcess( VOID *axRsc, BOOL IntStatus )
{
	ML_NODE_COMMONS *NodeCommons;
	AXIS_HANDLE *AxisRsc = 	(AXIS_HANDLE*)axRsc;

	/*----------------------------------------------------------------------*/
	NodeCommons = AxisRsc->NetNodeIf;
	if(IntStatus != FALSE)
	{/* MECHATROLINK割込みあり	*/
		NodeCommons->Tcyc_Counter = 0;
	}
	else
	{/* MECHATROLINK割込みなし	*/
		NodeCommons->Tcyc_Counter++;
	}
	/*----------------------------------------------------------------------*/
	/* Write Cyclic Responce to ASIC										*/
	/*----------------------------------------------------------------------*/
	if(NodeCommons->SyncState == ML_HWSYNC_ENABLE)
	{
		if(NodeCommons->Tcyc_Counter == 0)
		{
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
			ULONG alt_no = NdGetRecvAltBufferAddress();
			DMAdrvStartMemoryTransfer(DMA_MLDEV_CH, DMA_MLDEV_IN_LINK(alt_no));
#endif
		}
		return TRUE;
	}
	return FALSE;
}

/****************************************************************************
 ネットワークモジュール定周期処理
****************************************************************************/
static void mlOpiCommandInput( MLNET_HANDLE *MLAxisHandle )
{
	INT32			rc, recvErr;
	COM_HNDL		*com_hdl;

	/*----------------------------------------------------------------------*/
	/* Update Command Buffer and Detect the Communication Error				*/
	/*----------------------------------------------------------------------*/
	com_hdl = &MLAxisHandle->com_hdl;

/* 暫定処理：評価基板では、CMD受信をIntSyncで実施する。	*/
	/* CMDデータ受信 */
//	recvErr = NdReceiveCMD(com_hdl->my_ax_number, com_hdl->CmdBufPtr);

	/* 	ネットワーク同期処理スケジューリング	*/
	ComGenerateAplCycle( com_hdl );

	/* 通信周期タイミングチェック */
	if( 0 == com_hdl->comCycCnt )
	{
		/* 通信異常確認 */
		rc = ComCheckNetError( com_hdl );
		if( rc == COM_FUNC_NORMAL_END )
		{ /* 通信正常 */
			/* コマンド更新 */
			CmdUpdateData( &MLAxisHandle->cmd_hdl,
					com_hdl, MLAxisHandle->rsp_hdl.rspSetDone );
			CmdUpdateExData( &MLAxisHandle->cmd_hdl,
					com_hdl, MLAxisHandle->rsp_hdl.exRspSetDone );
		}
		//else if( (rc == COM_RECV_GBL_ERR) || (rc == COM_RECV_GBL_WARNG) )
		//{ /* GBL受信異常の場合は指令データの更新をして、アラームセット */
		//	/* コマンド更新 */
		//	CmdUpdateData( &MLAxisHandle->cmd_hdl,
		//			&MLAxisHandle->com_hdl, MLAxisHandle->rsp_hdl.rspSetDone );
		//	CmdUpdateExData( &MLAxisHandle->cmd_hdl,
		//			&MLAxisHandle->com_hdl, MLAxisHandle->rsp_hdl.exRspSetDone );
		//}
		else
		{ /* ！通信エラー発生！ */
			AlmSetCode( &MLAxisHandle->alm_hdl, 0, rc );
			if( rc != COM_RECV_WARNG )	/* warning以外->A.E50,A.E60時 */
			{
				/* Todo: 操作モードクリア */
				;
			}
		}
		/* CMD解析実行要求ON */
		MLAxisHandle->cmd_hdl.cmdInfo.enableCmdAna = TRUE;
		MLAxisHandle->cmd_hdl.exCmdInfo.enableCmdAna = TRUE;
	}

}


/****************************************************************************
 ネットワークモジュールサイクリック出力処理
****************************************************************************/
static void mlOpiResponseOutput( MLNET_HANDLE *MLAxisHandle )
{
	RSP_LINK_DATA	*rsp_buffer;
	RSP_LINK_EXDATA	*exrsp_buffer;
	UCHAR *pBuf;

	/* 通信周期タイミングチェック */
	if( FALSE != ComCheckRspTiming(&MLAxisHandle->com_hdl) )
	{
		/* メインコマンドRSP作成 */
		rsp_buffer = (RSP_LINK_DATA*)MLAxisHandle->com_hdl.RspBufPtr;
		RspMakeData( MLAxisHandle, rsp_buffer );

		/* サブコマンドRSP作成 */
		exrsp_buffer = (RSP_LINK_EXDATA*)(MLAxisHandle->com_hdl.RspBufPtr
											+ MLAxisHandle->com_hdl.exCmdOfs);
		RspMakeExData( MLAxisHandle, exrsp_buffer );

		/* RSPバッファ更新 */
		ComSetRspBuffer( &MLAxisHandle->com_hdl );

		/* PRM Monコマンドデータ更新 *//* <S0BD> */
		RegSetComMonData(&MLAxisHandle->reg_hdl, (USHORT*)MLAxisHandle->com_hdl.CmdBufPtr,
													(USHORT*)MLAxisHandle->com_hdl.RspBufPtr );
	}
	///* PRM Monコマンドデータ更新 */
	NdSendRSP( MLAxisHandle->com_hdl.my_ax_number, MLAxisHandle->com_hdl.RspBufPtr );
}

/****************************************************************************
 ネットワークモジュール定周期処理
****************************************************************************/
VOID OpiRunTimeScanProcess( void *axRsc, BOOL irqSts )
{
	LONG			ax_no;
	MLNET_HANDLE	*MLAxisHandle;
	ML_NODE_COMMONS *NodeCommons;
	AXIS_HANDLE 	*AxisRsc = 	(AXIS_HANDLE*)axRsc;



	/*----------------------------------------------------------------------*/
	/* Command Input														*/
	/*----------------------------------------------------------------------*/
	NodeCommons = AxisRsc->NetNodeIf;
	
	if(NodeCommons->Tcyc_Counter == 0)
	{
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
		if(FALSE != DMAdrvWaitForTransferEnd(DMA_MLDEV_CH, DMA_STD_TIMEOUT))
		{/* DMA transfer error. */
			ALMSetGlobalAlarm(ALM_WDC_ERR);
		}
#endif

		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			mlOpiCommandInput(MLAxisHandle);
		}
	}

	/*----------------------------------------------------------------------*/
	/* Command analysis and Motion output									*/
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;

		/* モーションモニタデータ入力 */
		MtcInputData( &MLAxisHandle->mt_hdl );

		/* メインコマンド解析・実行 */
		CmdAnalyze( MLAxisHandle );
		/* サブコマンド解析・実行 */
		CmdAnalyzeEx( MLAxisHandle );

		/* モーション実行 */
		MtcOutputData( &MLAxisHandle->mt_hdl );

		/* アラーム状態更新 */
		AlmHandleMain( &MLAxisHandle->alm_hdl );
	}

	/*----------------------------------------------------------------------*/
	/* Response Output														*/
	/*----------------------------------------------------------------------*/
	if(NodeCommons->Tcyc_RspTiming == NodeCommons->Tcyc_Counter)
	{/* 伝送周期タイミング */

		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			mlOpiResponseOutput(MLAxisHandle);
		}

#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
		if(NodeCommons->SyncState == ML_HWSYNC_ENABLE)
		{
		ULONG alt_no = NdGetSndAltBufferAddress();
		DMAdrvStartMemoryTransfer(DMA_MLDEV_CH, DMA_MLDEV_OUT_LINK(alt_no));
		}
#endif
	}
	
/* M2 Messageパケット制御 */
	NdControlMsgPkt();

}

/****************************************************************************
 ネットワークモジュールScanC処理
****************************************************************************/
VOID OpiScanCProcedure(VOID)
{
	;
}

/****************************************************************************
 ネットワーク安定状態取得処理		<S1DB>
****************************************************************************/
BOOL OpiCheckStableState( void *axRsc )
{
	AXIS_HANDLE *AxisRsc;
	AxisRsc = (AXIS_HANDLE*)axRsc;

	return AxisRsc->NetNodeIf->SyncExe;
}

/****************************************************************************
同期実行フラグリセット処理		<S1DB>
****************************************************************************/
void OpiResetSyncExe( void *axRsc )
{
	AXIS_HANDLE *AxisRsc;
	AxisRsc = (AXIS_HANDLE*)axRsc;

	AxisRsc->NetNodeIf->SyncExe = FALSE;

	return;
}

