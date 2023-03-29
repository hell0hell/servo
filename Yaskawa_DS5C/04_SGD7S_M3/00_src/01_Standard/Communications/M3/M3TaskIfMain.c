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
#include "M3TaskIfMain.h"
#include "ComManager.h"
#include "CmdAnalyzer.h"
#include "RspManager.h"
#include "ServoIf.h"
#include "SysApi.h"
#include "ComAlmMngr.h"
#include "RegTable.h"
#include "jl100s_gbl.h"
#include "AlarmManager.h"
#include "HwDeviceIf.h"
#include "MLib.h"
#include "KLib.h"
#include "DMAdrv.h"


/* Local functions */
//static VOID eventDrivenProcedure(VOID);


/****************************************************************************
 ネットワークモジュール初期化処理
****************************************************************************/
VOID OpiInitialize( VOID *axRsc )
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

		/* M3 Component初期化 */
		ComInit( &MLAxisHandle->com_hdl, ax_no );
		CmdInit( &MLAxisHandle->cmd_hdl );
		RspInit( &MLAxisHandle->rsp_hdl );

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
				 &MLAxisHandle->com_hdl.NetPrm,
				 prm_data_ptr->Prm,
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
			(AxisRsc->MencV->AxisMotType == MOTTYPE_LINEAR);	//暫定
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
	
	/* 共通パラメータ更新処理	*//* <S027> */
	if( RegSetComDefaultVal( &(MLAxisHandle->reg_hdl), prm_data_ptr ) != REG_SUCCESS )
	{
		AlmSetCode( &(MLAxisHandle->alm_hdl), AxisRsc->AxisNo, ERR_COM_PRM );
	}
}


/****************************************************************************
 ネットワークモジュールデバイス初期化処理
****************************************************************************/
VOID OpiInitDevice( VOID *axRsc, VOID *DeviceAddress )
{
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
//	INT32			rc;
	LONG			ax_no;
	MLNET_HANDLE	*MLAxisHandle;
	ND_PARAM		nd_prm;
	AXIS_HANDLE *AxisRsc = 	(AXIS_HANDLE*)axRsc;

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		MLAxisHandle->com_hdl.my_ax_number = AxisRsc[ax_no].AxisNo + 1;
	}

	/* 局アドレスおよび伝送バイト数は、RSW/DSWから取得する	*/
	nd_prm.NodeAddress = (UCHAR)HALdrv_GetStationAddress();
	nd_prm.DataSize = (UCHAR)HALdrv_GetTransmissionByte();

	nd_prm.MslvEnable = FALSE;
	nd_prm.TCycle = 1000;
#if 0/* <S0B1>:SGDVと同様の仕様に変更 */
	if( (nd_prm.DataSize != DATA_SIZE_32BYTE) &&
		(nd_prm.DataSize != DATA_SIZE_48BYTE) )
	{ /* When System Reserve */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{/* A.E41 : MECHATROLINK通信データサイズ設定異常 */
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			AlmSetCode( &(MLAxisHandle->alm_hdl), ax_no, ERR_CD_LEN_ERR );
		}
	}
#endif/* <S0B1>:SGDVと同様の仕様に変更 */

	/* Station Address Settings */
	if( (nd_prm.NodeAddress <= LOWER_INVALID_ADDRESS) ||
		(nd_prm.NodeAddress >= UPPER_INVALID_ADDRESS) )
	{ /* When Invalid Address Setting */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{/* A.E42 : MECHATROLINK局アドレス設定異常 */
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			AlmSetCode( &(MLAxisHandle->alm_hdl), ax_no, ERR_AXIS_NO_ERR );
		}
	}

	/* 通信モードパラメータ設定	*//* <S07A> */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		ComNetPrmSet(&MLAxisHandle->com_hdl, nd_prm);
	}

	/* 通信デバイスの初期化 */
	if( 0 !=  NdInit( &nd_prm, DeviceAddress ) )
	{
		if( 0 !=  NdInit( &nd_prm, DeviceAddress ) )
		{/* ASIC初期化異常 */
			for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
			{
				MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
				AlmSetCode( &MLAxisHandle->alm_hdl, ax_no, ERR_COM_LSISET );
			}
		}
	}

	/* 通信設定情報登録 */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ ) /* <S108> */
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		RegSetComPrmMon( &(MLAxisHandle->reg_hdl), nd_prm.NodeAddress, nd_prm.DataSize );
	}

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
		if(FALSE == GblState)
		{/* no Sync frame was received. */
			NodeCommons->GblFrameErr++;
			if(NodeCommons->GblFrameErr > GBL_FRAM_ERROR_COUNT)
			{	/* A.E63 will be set if Sync frame is not received three times. */
				for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
				{
					MLHandle = AxisRsc[ax_no].NetAxisIf;
					if(MLHandle->com_hdl.Phase > 1)
					{
						AlmSetCode( &MLHandle->alm_hdl, ax_no, ERR_GBL_COM_A );
						ComReqPhaseDown( &MLHandle->com_hdl, 2 );
					}
				}
				NodeCommons->SyncState = ML_HWSYNC_RESET;
				SyncDisable = TRUE;
			}
		}
		else
		{/* Sync frame was received. */
			NodeCommons->GblFrameErr = 0;
			if(FALSE == HALdrv_GetIntUDLStatus(INTERRUPT_ScanB, AxisRsc->SvAsicRegs->AsicHwReg))
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
		}
		break;

	/* Starting state in phase 0. */
	case ML_HWSYNC_START:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		if(FALSE != GblState)
		{
			KlibRstLongTimer(&NodeCommons->M3IntCounter);
			NodeCommons->SyncState = ML_HWSYNC_WAIT_TIME;
		}
		break;

	/* Waiting state until M3 interrupt is stabilized. */
	case ML_HWSYNC_WAIT_TIME:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
//		if(NodeCommons->SyncWaitTime < KlibGetLongTimerMs(NodeCommons->M3IntCounter))
		if( SYNC_WAIT_TIME < KlibGetLongTimerMs(NodeCommons->M3IntCounter) )		/* <S052> */
		{
			NodeCommons->SyncState = ML_HWSYNC_RESET;
		}
		break;

	/* Hardware Resetting state */
	case ML_HWSYNC_RESET:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		HALdrv_DisableExtSyncInterrupt(pAsicHwReg);
		NodeCommons->M3IntCounter = 0;
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
			NodeCommons->M3IntCounter++;
			if(NodeCommons->M3IntCounter >= 4)
			{
				NodeCommons->SyncState = ML_HWSYNC_INTERRUPT;
				HALdrv_EnableExtSyncInterrupt(pAsicHwReg);
				NodeCommons->M3IntCounter = 0;
			}
		}
		break;

	/* Waiting state for internal tasks. */
	case ML_HWSYNC_INTERRUPT:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		NodeCommons->M3IntCounter++;
		if(NodeCommons->M3IntCounter >= 8)
		{
			NodeCommons->SyncState = ML_HWSYNC_WAIT_COMP;
			SHalDrv_Reset_Intcycle(pAsicHwReg);
			KlibRstLongTimer(&NodeCommons->M3IntCounter);
		}
		break;

	/* Reset ScanTiming state for internal tasks. */
	case ML_HWSYNC_WAIT_COMP:
		SyncDisable = TRUE;
		NodeCommons->SyncExe = TRUE;	/*<1DB>*/
		if(FALSE != HALdrv_GetExtUnSyncStatus())
		{/* ScanB | ScanC割込み要求なし	*/
			if(KlibGetLongTimerMs(NodeCommons->M3IntCounter) > 28)/* TimeOut[4ms]	*/
			{/* Retry to sync, if synchronization was failed. */
				NodeCommons->SyncState = ML_HWSYNC_RESET;
				NodeCommons->M3IntCounter = 0;
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
			NodeCommons->GblFrameErr = 0;
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
BOOL OpiRoundProcedure( VOID *axRsc )
{
	BOOL			connect;
	INT32			ret;
	LONG			ax_no, DiscBusy;
	USHORT			t_mcyc;
	ULONG			MLScanTime, int_ofs;	/* <S0EC> */
	MLNET_HANDLE	*MLAxisHandle;
	ML_NODE_COMMONS *NodeCommons;
	AXIS_HANDLE		*AxisRsc;
	ASIC_HW_IF_STR	*pAsicHwReg;
	
	/* ローカル変数の初期化	*/
	AxisRsc = 	(AXIS_HANDLE*)axRsc;
	pAsicHwReg = AxisRsc->SvAsicRegs->AsicHwReg;
	NodeCommons = AxisRsc->NetNodeIf;
	DiscBusy = 0;

	/*----------------------------------------------------------------------*/
	/* Communication Reset Procedure in Phase 0								*/
	/*----------------------------------------------------------------------*/
	if(NodeCommons->ComResetSeq == ML_COMRST_START)
	{
		/* M3通信状態リセット(DISC処理) */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			DiscBusy |= CmdReqComReset( AxisRsc[ax_no].NetAxisIf );
		}

		if(DiscBusy == 0)
		{
//			if((COM_MODE_SYNC != NdGetComMode())  /* <S0EC> */ 
			if((NodeCommons->SyncState == ML_HWSYNC_DISABLE)
				|| (NodeCommons->SyncState == ML_HWSYNC_ENABLE))
			{
				/* 初期化が完了したら全軸Phase1セット */
				for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
				{
//					(AxisRsc[ax_no].NetAxisIf)->com_hdl.Phase = 1;
					MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
					MLAxisHandle->com_hdl.Phase = 1;

				}
				NodeCommons->ComResetSeq = ML_COMRST_DONE;
			}
		}
	}
//	else /* <S01D> */
	/*----------------------------------------------------------------------*/
	/* Physical Layer Link Up Sequence										*/
	/*----------------------------------------------------------------------*/
	if(DiscBusy == 0) /* <S01D> */
	{
		ret = NdNormalCommunicationIsReady(&t_mcyc, &int_ofs); /* <S0EC> */
		if( ret == ND_SUCCESS )
		{/*--- Start Link Comminication--- */

			if(COM_MODE_SYNC == NdGetComMode())
			{/*--- Cyclic mode ---*/
				MLScanTime = (AxisRsc->UniPrms->Bprm)->SvCycleUs*4;
				NodeCommons->Tcyc_RspTiming = (t_mcyc/MLScanTime) - 1;
				NodeCommons->SyncState = ML_HWSYNC_START;
				NodeCommons->ComResetSeq = ML_COMRST_START;
			}
			else
			{/*--- Event driven mode ---*/
//				for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
//				{
//					(AxisRsc[ax_no].NetAxisIf)->com_hdl.Phase = 1;
//				}
				HALdrv_DisableExtSyncInterrupt(pAsicHwReg);
				NodeCommons->Tcyc_RspTiming = 0;
				NodeCommons->SyncState = ML_HWSYNC_DISABLE;
				NodeCommons->ComResetSeq = ML_COMRST_START;
			}
		}
		else if( ret == ND_RCVED_TMC_FRAME )
		{/*--- Reset Link Comminication --- */
			/* TMCフレームを受信した場合は、
				DISC処理を行い、M3通信準備処理を再実行 */
			HALdrv_DisableExtSyncInterrupt(pAsicHwReg);
			NodeCommons->SyncState = ML_HWSYNC_DISABLE;
			NodeCommons->ComResetSeq = ML_COMRST_START; /* <S01D> */
		}
		else if( ret == ND_DETECT_SLV_CMP_ERROR )
		{/* 局アドレス設定異常全軸検出 */
			ALMSetGlobalAlarm(ALM_AXIS_NO_ERR);
		}
		else if( ret < 0 )
		{/* 異常時は全軸A.B6Aを検出 */
			ALMSetGlobalAlarm(ALM_COMLSI_ERR1);
		}
		else
		{/* Not Ready or Already started. */
			;
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
	LONG			ax_no, gbl_stat;
	MLNET_HANDLE	*MLAxisHandle;
	ML_NODE_COMMONS *NodeCommons;

	/* ローカル変数の初期化 */
	MLAxisHandle = AxisRsc->NetAxisIf;
	NodeCommons = AxisRsc->NetNodeIf;

	/*-----------------------------------*/
	/* 		JL-100アクセス処理			 */
	/*-----------------------------------*/
	gbl_stat = NdBeginCommunication( &ests ); /* ASIC状態取得 */

	/* MECHATROLINK-3同期状態制御 */
	RxDisabled = mlSyncStateControls(AxisRsc, NodeCommons, gbl_stat);

	if( ( 0 != (ests & JL100_ESTS_RXLEN_ERR) ) && (MLAxisHandle->com_hdl.Phase <= 1) )
	{ /* 通信データサイズ異常 Phase1以下でのみ検出 */
		/* Phase2以上で本ビットが立った時は指令データ未受信として処理 */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			/* A.E41 : MECHATROLINK通信データサイズ設定異常 */
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			AlmSetCode( &MLAxisHandle->alm_hdl, ax_no, ERR_CD_LEN_ERR );
		}
	}

	if( 0 != (ests & JL100_ESTS_WDTOVR) )
	{ /* ASIC異常 */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			/* A.B6B : JL-080内部システムエラー */
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			AlmSetCode( &MLAxisHandle->alm_hdl, ax_no, ERR_COM_LSISYS );
		}
	}

	
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )/* <S0BD> */
	{
		/* for error status monitor (However,ESTE is 0 in JL077.) */
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
		RegSetComErrData(&MLAxisHandle->reg_hdl, MLAxisHandle->com_hdl.recvErrCntNum, ests);
	}

	return RxDisabled;
	
}

/****************************************************************************/
BOOL OpiIntSync(VOID *axRsc, BOOL IntStatus)
{
	LONG			ax_no;
	ULONG			ests;
	AXIS_HANDLE 	*AxisRsc;
	ML_NODE_COMMONS *NodeCommons;
	

	/* ローカル変数の初期化 */
	AxisRsc = 	(AXIS_HANDLE*)axRsc;
	NodeCommons = AxisRsc->NetNodeIf;

	if(NodeCommons->ComResetSeq == ML_COMRST_NONE)
	{
		return TRUE;
	}

	/*----------------------------------------------------------------------*/
	/* Start up Procedure													*/
	/*----------------------------------------------------------------------*/
	if(COM_MODE_SYNC == NdGetComMode())
	{/* Cyclic Communication mode */
		if( 0 == (IntStatus & 1) )
		{/* M3割り込みなし時は強制終了 */
			if( (NodeCommons->Tcyc_Counter > NodeCommons->Tcyc_RspTiming)
				&& (NodeCommons->SyncState == ML_HWSYNC_ENABLE) )
			{/* 本条件では、割り込みが実施されるﾀｲﾐﾝｸﾞである。 */
				NodeCommons->InterruptReq = TRUE;
				/* JL-100アクセス処理(ASIC状態確認/CMDデータ受信/異常検出) */
//				if(FALSE != mlOpiIntSync( AxisRsc )) /* <S0EC> */
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
		{/* M3割り込みあり */
			NodeCommons->InterruptReq = TRUE;
			/* JL-100アクセス処理(ASIC状態確認/CMDデータ受信/異常検出) */
			if(FALSE != mlOpiIntSync( AxisRsc ))
			{/* 非同期時は強制終了 */
				return TRUE;
			}
		}
	}
	else
	{
		NodeCommons->InterruptReq = TRUE;/* <S004> */
		NdBeginCommunication(&ests);
		if(0 != ests)
		{
			for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
			{	/* A.B6B : JL-080内部システムエラー */
				AlmSetCode( &(AxisRsc[ax_no].NetAxisIf)->alm_hdl, ax_no, ERR_COM_LSISYS );
			}
		}
	}

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
	INT32			rc;
	COM_HNDL		*com_hdl;

	/*----------------------------------------------------------------------*/
	/* Update Command Buffer and Detect the Communication Error				*/
	/*----------------------------------------------------------------------*/
	com_hdl = &MLAxisHandle->com_hdl;

	/* CMDデータ受信 */
	com_hdl->LrecvErr = com_hdl->RecvErr;
	com_hdl->RecvErr = NdReceiveCMD(com_hdl->my_ax_number, com_hdl->CmdBufPtr);

	/* 	ネットワーク同期処理スケジューリング	*/
//	ComGenerateAplCycle( com_hdl, recvErr );
	ComGenerateAplCycle( com_hdl );
	/* 通信周期タイミングチェック */
	if( 0 == com_hdl->comCycCnt )
	{
		/* 通信異常確認 */
//		rc = ComCheckNetError( com_hdl, recvErr );
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
		RegSetComMonData(&MLAxisHandle->reg_hdl,
							(USHORT*)MLAxisHandle->com_hdl.CmdBufPtr, 
							(USHORT*)MLAxisHandle->com_hdl.RspBufPtr,
							MLAxisHandle->cmd_hdl.cmdInfo.profileType );

//		NdSendRSP( MLAxisHandle->com_hdl.my_ax_number, MLAxisHandle->com_hdl.RspBufPtr );
	}
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
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
	ULONG alt_no = NdGetRecvAltBufferAddress();
#endif

	NodeCommons = AxisRsc->NetNodeIf;
	/*----------------------------------------------------------------------*/
	/* 	通信リセット未完了持														*/
	/*----------------------------------------------------------------------*//* <S14D> */
	if(NodeCommons->ComResetSeq != ML_COMRST_DONE)
	{
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			MLAxisHandle = AxisRsc[ax_no].NetAxisIf;
			MtcInputData( &MLAxisHandle->mt_hdl );
			MtcOutputData( &MLAxisHandle->mt_hdl );
		}
		return;
	}

	/*----------------------------------------------------------------------*/
	/* Motion Input															*/
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;

		/* モーションモニタデータ入力 */
		MtcInputData( &MLAxisHandle->mt_hdl );
	}

	/*----------------------------------------------------------------------*/
	/* Command analysis and Motion output									*/
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MLAxisHandle = AxisRsc[ax_no].NetAxisIf;

		if(NodeCommons->Tcyc_Counter == 0)/* <S004> */
		{/* 伝送周期タイミング */
#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
			if(NodeCommons->SyncState == ML_HWSYNC_ENABLE)/* <S004> */
			{
			while(DMAdrvCheckLinkStatus(DMA_MLDEV_CH, DMA_MLDEV_IN_LINK(alt_no)+ax_no));
			}
#endif
			/* Command Input */
			mlOpiCommandInput(MLAxisHandle);
		}

		/* メインコマンド解析・実行 */
		CmdAnalyze( MLAxisHandle );
		/* サブコマンド解析・実行 */
		CmdAnalyzeEx( MLAxisHandle );

		/* モーション実行 */
		MtcOutputData( &MLAxisHandle->mt_hdl );

		/* アラーム状態更新 */
		AlmClearCmdSate( &MLAxisHandle->alm_hdl,
						 MLAxisHandle->cmd_hdl.cmdInfo.CmdAlm,
						 MLAxisHandle->cmd_hdl.exCmdInfo.CmdAlm);
		AlmHandleMain( &MLAxisHandle->alm_hdl );

		if(NodeCommons->Tcyc_RspTiming == NodeCommons->Tcyc_Counter)
		{/* 伝送周期タイミング */
			mlOpiResponseOutput(MLAxisHandle);
		}
	}

#if (CSW_COMASIC_DMA_TRANSFER != FALSE)
	/*----------------------------------------------------------------------*/
	/* Response Output														*/
	/*----------------------------------------------------------------------*/
	if(NodeCommons->SyncState == ML_HWSYNC_ENABLE)
	{
		if(NodeCommons->Tcyc_RspTiming == NodeCommons->Tcyc_Counter)
		{/* 伝送周期タイミング */
			UINT snd_alt_no = NdGetSndAltBufferAddress();
			DMAdrvStartMemoryTransfer(DMA_MLDEV_CH, DMA_MLDEV_OUT_LINK(snd_alt_no));
		}
	}
#endif

}



/****************************************************************************
 ネットワークモジュールソフトリセット処理
****************************************************************************/
VOID OpiSoftReset( VOID *axRsc )
{
	;
}


/****************************************************************************
 ネットワークモジュールScanC処理
****************************************************************************/
UINT OpiScanCProcess(void *AxisRsc)
{
	return 0;
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

