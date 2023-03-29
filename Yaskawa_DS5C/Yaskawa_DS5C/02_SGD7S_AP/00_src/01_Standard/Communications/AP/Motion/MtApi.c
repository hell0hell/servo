/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion API		 													*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	加速度指令追加									*/
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
//#include "Basedef.h"
//#include "ServoIf.h"
#include "KnlApi.h"
#include "Motion.h"
#//include "MlibSvc.h"
#include "MtCtrl.h"
#include "MtApi.h"

/* defines */
#define TREFMODE_NON		0
#define TREFMODE_TLIM1		1
#define TREFMODE_TFF		2
#define TREFMODE_TLIM2		3
#define TREFMODE_TFF_TLIM	4

/* forward declarations */
/* Public functions */
INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd);
INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd);
VOID MciSetOptionCmd(MTHNDL* hdl, ULONG option);
INT32 MciEnableState(MTHNDL* hdl, BOOL enable);
INT32 MciSetServoOn(MTHNDL* hdl, BOOL enable, BOOL reset);
INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable);
INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR signl);
INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode);
INT32 MciSetCordinatePos(MTHNDL* hdl, LONG data, BOOL ref);
VOID MciOutputSignals(MTHNDL* hdl, USHORT signl);
VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable);
INT32 MciResetMotion(MTHNDL* hdl);

/* Motion状態取得API */
//UCHAR MciGetMType(MTHNDL* hdl);
//BOOL MciGetServoOnStatus(MTHNDL* hdl);
//BOOL MciGetSensOnStatus(MTHNDL* hdl);
//UCHAR MciGetCtrlMode(MTHNDL* hdl);
//LONG MciGetMonitorData(MTHNDL* hdl, USHORT sel, UCHAR unit);
//BOOL MciGetDenStatus(MTHNDL* hdl);
//BOOL MciGetLatchStatus(MTHNDL* hdl ,UCHAR *signl);
//USHORT MciGetLatchSeqStatus(MTHNDL* hdl);
//UCHAR MciGetLatchSeqMode(MTHNDL* hdl);
//BOOL MciGetInPosStatus(MTHNDL* hdl);
//BOOL MciGetNearStatus(MTHNDL* hdl);
//BOOL MciGetZpointStatus(MTHNDL* hdl);
//UCHAR MciGetSoftLimitStatus(MTHNDL* hdl);
//BOOL MciGetTGOnStatus(MTHNDL* hdl);
//BOOL MciGetMainPowerStatus(MTHNDL* hdl);
//BOOL MciGetVlimitStatus(MTHNDL* hdl);
//BOOL MciGetTrqLimitStatus(MTHNDL* hdl);
//BOOL MciGetVcmpStatus(MTHNDL* hdl);
//USHORT MciGetInputSignals(MTHNDL* hdl);
//USHORT MciGetInPortSignals(MTHNDL* hdl);
//LONG MciGetOutputCmdDist(MTHNDL* hdl);
#if ( CCSW_SFTID == SFTID_EXIST )											  /*<V717>Add:Start*/
USHORT MciGetSomStatus(void);
#endif																		  /*<V717>Add:End  */



/* Private functions */
#define mciClearMtLatch(hdl)		if((hdl->latch.Mode != MODAL_LTMD)\
									  && (hdl->latch.Mode != MOTION_MDL_LTMD))\
									{ MtcCancelLatch(hdl, NONE_LTMD);}
#define mciSetMtLtMode(hdl,md)		(hdl->latch.Mode = md)

#define MciSetOptionCmd(x, y)	{;}	/* SGDV+INDEXER(暫定) */

/****************************************************************************
 加減速指令データ設定
****************************************************************************/
#if 0
VOID mciSetAccRate(MTHNDL* hdl, MCI_CMD* cmd)
{
	if((hdl->mtPrm.BasePrm.SetCmdAccRate != NULL) && (cmd->EnableRate != FALSE))
	{/* 有効コマンド && 加減速指令パラメータSW有効 */
		if( (cmd->RstReq != FALSE)
			|| (hdl->cmdData.CmdRateOn == FALSE) 
			|| (PsnGetDenSts(&hdl->psnHdl) != FALSE) )
		{/* コマンド切り替わり && DEN=1 */
			hdl->mtPrm.BasePrm.SetCmdAccRate(&hdl->mtPrm, 
									cmd->TSpeed, cmd->AccRate, cmd->DecRate);
		}
		hdl->cmdData.CmdRateOn = TRUE;
	}
	else
	{
		hdl->cmdData.CmdRateOn = FALSE;
	}
}
#endif

/****************************************************************************
 位置・速度制御時トルクデータ設定
****************************************************************************/
#if 0
VOID mciSetTrqData(MTHNDL* hdl, MCI_CMD* cmd, UCHAR trefMode)
{
	switch(trefMode)
	{
	case TREFMODE_TFF:	/* TFF有効 */
		hdl->cmdData.Trq = (LONG)cmd->Trq;		/* TFFセット */
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMITセット */
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMITセット */
		break;
	case TREFMODE_TLIM1:/* トルクリミット有効 */
	case TREFMODE_TLIM2:
		hdl->cmdData.Trq = 0;								/* TFFセット */
		SviSetTrqLimitP(hdl->svCmd, cmd->TrqLimitP >> 6);	/* TRQ LIMITセット */
		SviSetTrqLimitN(hdl->svCmd, cmd->TrqLimitN >> 6);	/* TRQ LIMITセット */
		break;
	default: 			/* トルクリミット無効 */
		hdl->cmdData.Trq = 0;					/* TFFセット */
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMITセット */
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMITセット */
		break;
	}
}
#endif


/****************************************************************************
 INTERPOLATE指令設定
****************************************************************************/
#if 0
INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - PsnGetIPOS(&hdl->psnHdl);

/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* 指令速度チェック */
	if((ULONG)MlibABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
		return MCI_SPD_ERR;

/* モーション指令データセット */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPEセット */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット */
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFFセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIMセット*/	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciClearMtLatch(hdl);						/* MOTION LTモードクリア */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 LATCH指令設定
****************************************************************************/
#if 0
INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - PsnGetIPOS(&hdl->psnHdl);

/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* 指令速度チェック */
	if((ULONG)MlibABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
		return MCI_SPD_ERR;

/* モーション指令データセット */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPEセット */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット */
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFFセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIMセット*/
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	MciRequestMtLatch(hdl, INTERPOL_LTMD, cmd->LatchSgnl);/* ラッチ要求 */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 POSING指令設定
****************************************************************************/
#if 0
INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* 指令速度チェック */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_POSING;			/* MTYPEセット */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciClearMtLatch(hdl);						/* MOTION LTモードクリア */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 EX_POSING指令設定
****************************************************************************/
#if 0
INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* 指令速度チェック */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_EXPOSING;		/* MTYPEセット */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット */
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ラッチ信号セット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciSetMtLtMode(hdl, EXPOSING_LTMD);			/* MOTION LT MODEセット */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 HOMING指令設定
****************************************************************************/
#if 0
INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* 指令速度チェック */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_HOMING;			/* MTYPEセット */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット */
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ラッチ信号セット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciSetMtLtMode(hdl, ZRET_LTMD);				/* MOTION LT MODEセット */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 FEED指令設定
****************************************************************************/
#if 0
INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* 指令速度チェック */
	if((ULONG)MlibABS(cmd->TSpeed) > hdl->mtPrm.BasePrm.PosingSpdMax)
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_FEED;			/* MTYPEセット */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciClearMtLatch(hdl);						/* MOTION LTモードクリア */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 HOLD指令設定
****************************************************************************/
#if 0
INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	if(hdl->mtState == MSTAT_DISABLE)
		return MCI_CNDTN_ERR;

	hdl->cmdData.Mtype = MTYPE_HOLD;			/* MTYPEセット */
	hdl->cmdData.HoldMod = cmd->HoldMod;		/* Holdモードセット */
	hdl->cmdData.TSpeed = 0;					/* TSPDセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciClearMtLatch(hdl);						/* MOTION LTモードクリア */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 VELCTRL指令設定
****************************************************************************/
#if 0
INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	if(hdl->mtState == MSTAT_DISABLE)
		return MCI_CNDTN_ERR;

	hdl->cmdData.Mtype = MTYPE_VELCTRL;			/* MTYPEセット */
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vrefセット */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;		/* 速度制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.trefMode);	/*TFF,TLIM*/
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciClearMtLatch(hdl);						/* MOTION LTモードクリア */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 TRQCTRL指令設定
****************************************************************************/
#if 0
INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	if(hdl->mtState == MSTAT_DISABLE)
		return MCI_CNDTN_ERR;

	hdl->cmdData.Mtype = MTYPE_TRQCTRL;			/* MTYPEセット */
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vrefセット */
	hdl->cmdData.Trq = cmd->Trq;				/* TFFセット */
	hdl->cmdData.CtrlMode = MT_TRQ_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	SviSetTrqLimitP(hdl->svCmd, cmd->TrqLimitP >> 6);/* TRQ LIMITセット */
	SviSetTrqLimitN(hdl->svCmd, cmd->TrqLimitN >> 6);/* TRQ LIMITセット */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	mciClearMtLatch(hdl);						/* MOTION LTモードクリア */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 OPTION指令設定
****************************************************************************/
#if 0
VOID MciSetOptionCmd(MTHNDL* hdl, ULONG option)
{
	UCHAR bitVal;
/* ACC_FIL */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.accfilBit) & hdl->mtPrm.OptMap.accfilBitMsk);
	hdl->cmdData.AccFilSel = bitVal;
/* G_SEL */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.gselBit) & hdl->mtPrm.OptMap.gselBitMsk);
	SviSetBankSel(hdl->svCmd, bitVal);
/* P_PI */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.vppiBit) & hdl->mtPrm.OptMap.vppiBitMsk);
	SviSetEnbPCtrl(hdl->svCmd, bitVal);
/* P_I_CLR */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.ppiclrBit) & hdl->mtPrm.OptMap.ppiclrBitMsk);
	SviSetClrPosIntg(hdl->svCmd, bitVal);
/* N_CL */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.nclBit) & hdl->mtPrm.OptMap.nclBitMsk);
	SviSetEnbTrqLmtN(hdl->svCmd, bitVal);
/* P_CL */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.pclBit) & hdl->mtPrm.OptMap.pclBitMsk);
	SviSetEnbTrqLmtP(hdl->svCmd, bitVal);
/* OUT_SIGNAL */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.outpBit) & hdl->mtPrm.OptMap.outpBitMsk);
	SviSetOutSig(hdl->svCmd, bitVal);
/* BANKSEL */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.bankBit) & hdl->mtPrm.OptMap.bankBitMsk);
	MBnkSetSelector(&hdl->mtPrm, bitVal);
/* LT_DISABLE */
	bitVal = (UCHAR)((option >> hdl->mtPrm.OptMap.ltBit) & hdl->mtPrm.OptMap.ltBitMsk);
	hdl->latch.LtDisable = bitVal;
}
#endif

/****************************************************************************
 モーション状態リセット
****************************************************************************/
#if 0
INT32 MciResetMotion(MTHNDL* hdl)
{
	INT32 rc = MCI_WAIT_COMP;

	if( (hdl->mtState == MSTAT_READY)
		|| (hdl->mtState == MSTAT_WAITPG_RDY)
		|| (hdl->mtState == MSTAT_DISABLE))
	{
		MciSetOptionCmd(hdl, 0);			/* Option指令クリア */
		MtcCancelLatch(hdl, NONE_LTMD);		/* ラッチクリア */
		PsnResetApos(&hdl->psnHdl, FALSE);	/* Positionリセット */
		hdl->cmdData.SensOn = FALSE;		/* SENS_OFF */
		hdl->cmdData.MtEenable = FALSE;		/* Motion無効 */

		SviSetSensOn( hdl->svCmd, FALSE );										/*<V887>*/
		if( hdl->mtState == MSTAT_DISABLE && ! SviGetSensOnSts( hdl->svRsp ) )	/*<V887>*/
			rc = MCI_SUCCESS;	/* 既にDISABLE状態なら完了 */
	}
	else
	{	rc = MCI_CNDTN_ERR;}

	return rc;
}
#endif

/****************************************************************************
 モーション状態切り替え
****************************************************************************/
#if 0
INT32 MciEnableState(MTHNDL* hdl, BOOL enable)
{
	INT32 rc = MCI_WAIT_COMP;

	if(enable)
	{/* モーション有効要求ON */
		hdl->cmdData.MtEenable = TRUE;
		if(hdl->mtState != MSTAT_DISABLE)
			rc = MCI_SUCCESS;	/* DISABLE状態でなければ完了 */
	}
	else
	{/* モーション有効要求OFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{
			hdl->cmdData.MtEenable = FALSE;
			if(hdl->mtState == MSTAT_DISABLE)
				rc = MCI_SUCCESS;	/* 既にDISABLE状態なら完了 */
		}
		else
		{
			rc = MCI_CNDTN_ERR;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 サーボオン指令発行
****************************************************************************/
#if 0
INT32 MciSetServoOn(MTHNDL* hdl, BOOL enable, BOOL reset)
{
	BOOL pwr, alm, emg;
	INT32 rc = MCI_WAIT_COMP;

	if(enable)
	{/* ServoON */
		pwr = SviGetMainPowerSts(hdl->svRsp);
		emg = SviGetEmgSig(hdl->svRsp);
		alm = SviGetAlmSts(hdl->svRsp);
		if( (pwr == FALSE) || (alm == TRUE) 
			|| (hdl->mtState == MSTAT_DISABLE)
			|| (hdl->mtState == MSTAT_WAITPG_RDY) )
		{/* 主回路断 or ALM or Motion準備未完 */
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_CNDTN_ERR;
		}
		else if(emg)
		{
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_EMGBB_ERR;			/* EMG状態 */
		}
		else
		{
			if(!hdl->cmdData.SvonLock || reset)
			{	
				hdl->cmdData.SvonLock = FALSE;
				hdl->cmdData.ServoOn = TRUE;
				if(hdl->mtState == MSTAT_RUN)
				{	rc = MCI_SUCCESS;		/* RUN状態なら完了 */}
			}
			else
			{	rc = MCI_SUCCESS;	}
		}
	}
	else
	{/* ServoOFF */
		hdl->cmdData.SvonLock = FALSE;
		hdl->cmdData.ServoOn = FALSE;
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{/* サーボOFF完了 */
			rc = MCI_SUCCESS;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 センサオン指令発行
****************************************************************************/
#if 0
INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable)
{
	INT32 rc = MCI_WAIT_COMP;

	if(enable)
	{/* SensorON */
//		hdl->cmdData.SensOn = TRUE;		/*<V544>*/
		if( (SviGetServoOnSts(hdl->svRsp) != FALSE)		/* 既にSvON状態(JOG中もOKとするため) */
			|| ((hdl->mtState != MSTAT_WAITPG_RDY) 		/* or SensOn完了状態 */
				&& (hdl->mtState != MSTAT_DISABLE)) )
		{/* Sensor On 完了 */
			rc = MCI_SUCCESS;
		}
		/*<V544> start */
		else
		{/* Sensor On処理未完状態 */
			if(SviGetSensOnSts(hdl->svRsp) == FALSE)
			{/* SensOff状態を確認後要求ｾｯﾄ */
				hdl->cmdData.SensOn = TRUE;
			}
		}
		/*<V544> end */
	}
	else
	{/* SensorOFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{/* サーボOFF状態のみ */
			if(hdl->mtPrm.PsnPrm.absPG)
			{/* ABSの場合のみ、Sensor Off */
				hdl->cmdData.SensOn = FALSE;
			}
			PsnSetRefPoint(&hdl->psnHdl, FALSE);
			rc = MCI_SUCCESS;
		}
		else
		{/* SensorOFFできないのでエラー */
			rc = MCI_CNDTN_ERR;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 ラッチ要求発行(Motion用)
****************************************************************************/
#if 0
INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl)
{
	if((hdl->latch.Mode != mode) 		/* ラッチモード不一致 */
		|| !(hdl->latch.Cmp | hdl->latch.Req))	/* ラッチなし */
	{
		MtcCancelLatch(hdl, mode);
		MtcReqLatch(hdl, mode, sgnl);
	}
	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 Modalラッチ要求発行(mode=0:通常ラッチ、=1:連続ラッチ)
****************************************************************************/
#if 0
INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl)
{
	if(hdl->latch.Mode == NONE_LTMD)
	{/* ラッチモードなし時のみ要求発行 */
		if(mode == 1)
		{/* シーケンシャルモード */
			if(hdl->mtPrm.LtPrm.seqNum == 0)
				return MCI_LTMOD_ERR;

			hdl->latch.SeqReq = MTLT_SEQ_REQ;
			sgnl = hdl->mtPrm.LtPrm.ltSgnl[0];
		}
		MtcReqLatch(hdl, MODAL_LTMD, sgnl);
	}
	else
	{/* 現在ラッチモード≠Modalラッチモードならばエラー */
		if(hdl->latch.Mode != MODAL_LTMD)
			return MCI_LTCNFLCT_ERR;
	}
	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 ラッチキャンセル
****************************************************************************/
INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode)
{
	if(mode == MODAL_LTMD)
	{/* Modalラッチキャンセル */
		if( (hdl->latch.Mode != NONE_LTMD)
			&& (hdl->latch.Mode != MODAL_LTMD) )
		{/* ラッチモードあり && MODAL_LTMD以外は、エラー */
			return MCI_LTCNFLCT_ERR;
		}
		MtcCancelLatch(hdl, NONE_LTMD);
	}
	else if(mode == MOTION_MDL_LTMD)
	{/* Motion Modalキャンセル */
		MtcCancelLatch(hdl, MOTION_MDL_LTMD);
	}
	else
	{/* 通常 Motionキャンセル */
		if( (hdl->latch.Mode == MODAL_LTMD)	
			|| ((hdl->latch.Mode == ZRET_LTMD) && (hdl->seqState != 0)) )	
		{/* ZRET動作中 or モーダルラッチ中は、何もしない */
			return MCI_SUCCESS;
		}
		MtcCancelLatch(hdl, NONE_LTMD);
	}
	return MCI_SUCCESS;
}

/****************************************************************************
 座標系設定
****************************************************************************/
#if 0
INT32 MciSetCordinatePos(MTHNDL* hdl, LONG data, BOOL ref)
{
	PsnSetCordinate(&hdl->psnHdl, 0, data);
	if(ref == TRUE)
		PsnSetRefPoint(&hdl->psnHdl, ref);
	hdl->cmdData.TPos = PsnGetTPOS(&hdl->psnHdl);

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 リモートI/O出力
****************************************************************************/
#if 0
VOID MciOutputSignals(MTHNDL* hdl, USHORT signl)
{
	SviSetOutSig(hdl->svCmd, (UCHAR)signl);
}
#endif

/****************************************************************************
 BK信号
****************************************************************************/
#if 0
VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable)
{
	SviSetBrkOff(hdl->svCmd, !enable);
}
#endif




/* Motion状態取得API */
/****************************************************************************
 LATCHステータス取得
****************************************************************************/
#if 0
BOOL MciGetLatchStatus(MTHNDL* hdl, UCHAR *ltEdge)
{
	UCHAR lcmp;

	*ltEdge = (hdl->latch.Cmp >> 4);		/* bit4状態 */
	hdl->latch.Cmp = hdl->latch.Cmp & 0x1;	/* bit4:OFF */
	lcmp = (hdl->latch.SeqReq != 0) ? *ltEdge: hdl->latch.Cmp;
	return (BOOL)lcmp;
}
#endif

/****************************************************************************
 LATCHステータス取得(シーケンスラッチ用)
****************************************************************************/
#if 0
USHORT MciGetLatchSeqStatus(MTHNDL* hdl){return hdl->latch.SeqSts.wdCnt;}
UCHAR MciGetLatchSeqMode(MTHNDL* hdl){return hdl->latch.SeqReq;}
#endif



/****************************************************************************
 モニタデータ取得
****************************************************************************/
#if 0
LONG MciGetMonitorData(MTHNDL* hdl, USHORT sel, UCHAR unit)
{
	LONG data;
	
	switch(sel)
	{
	case POS_MON:	data = PsnGetPOS(&hdl->psnHdl); break;
	case MPOS_MON:	data = PsnGetMPOS(&hdl->psnHdl);break;
	case PERR_MON:	data = PsnGetPERR(&hdl->psnHdl);break;
	case APOS_MON:	data = PsnGetAPOS(&hdl->psnHdl);break;
	case LPOS_MON:	data = PsnGetLPOS(&hdl->psnHdl);break;
	case IPOS_MON:	data = PsnGetIPOS(&hdl->psnHdl);break;
	case TPOS_MON:	data = PsnGetTPOS(&hdl->psnHdl);break;
	case FSPD_MON:
		if(unit == 2)	/* [30bit/MAX] */
			data = MlibMulgain32(SviGetSpeed(hdl->svRsp), hdl->mtPrm.BasePrm.osToMaxKx);
		else			/* [ref/s] */
			data = MlibMulgain32(SviGetSpeed(hdl->svRsp), hdl->mtPrm.BasePrm.osToRefKx);
//			data = MlibMulgain32(PsnGetFBSPD(&hdl->psnHdl), hdl->mtPrm.BasePrm.fspdRefKx);
		break;
	case CSPD_MON:
		if(unit == 2)	/* [30bit/MAX] */
			data = hdl->outData.Vref;
		else			/* [ref/s] */
			data = MlibMulgain32(hdl->outData.DposOut, hdl->mtPrm.BasePrm.fspdRefKx);
		break;
	case TSPD_MON:
		if(unit == 2)	/* [30bit/MAX] */
			data = hdl->outData.Vref;
		else			/* [ref/s] */
			data = hdl->outData.Tspd;
		break;
	case TRQ_MON:
		data = SviGetTrq(hdl->svRsp);	/* [24bit/MAX] */
		if(unit == 3)	/* [30bit/MAX] */
			data = data << 6;
		else			/*[%] */
			data = MlibMulgain(data, hdl->mtPrm.BasePrm.trqKx);
		break;
	case OMN1_MON:	
		data = (SviGetOMN1Sel(hdl->svCmd) == 0x80) ?
			PsnGetLstLPOS(&hdl->psnHdl) : SviGetOMN1(hdl->svRsp);
		break;
	case OMN2_MON:	
		data = (SviGetOMN2Sel(hdl->svCmd) == 0x80) ?
			PsnGetLstLPOS(&hdl->psnHdl) : SviGetOMN2(hdl->svRsp);
		break;
	default:
		data = 0;
		break;
	}
	return data;
}
#endif

/****************************************************************************
 I/Oモニタデータ取得
****************************************************************************/
#if 0
USHORT MciGetInputSignals(MTHNDL* hdl)
{
	BOOL	bitVal;
	ULONG	pls;
	USHORT	sgnl=0;

	bitVal = SviGetPOTSig(hdl->svRsp);
	sgnl |= (bitVal << P_OT_SGNL);
	bitVal = SviGetNOTSig(hdl->svRsp);
	sgnl |= (bitVal << N_OT_SGNL);
	bitVal = SviGetDecSig(hdl->svRsp);
	sgnl |= (bitVal << DEC_SGNL);
	bitVal = SviGetExt1Sig(hdl->svRsp);
	sgnl |= (bitVal << EXT1_SGNL);
	bitVal = SviGetExt2Sig(hdl->svRsp);
	sgnl |= (bitVal << EXT2_SGNL);
	bitVal = SviGetExt3Sig(hdl->svRsp);
	sgnl |= (bitVal << EXT3_SGNL);
	bitVal = !SviGetBrkOffSts(hdl->svRsp);
	sgnl |= (bitVal << BK_SGNL);
	bitVal = SviGetEmgSig(hdl->svRsp);
	sgnl |= (bitVal << EMG_SGNL);
	pls = SviGetPlsSig(hdl->svRsp) & 0x7;
	sgnl |= (pls << PA_SGNL);

	return sgnl;
}
#endif

#if 0
USHORT MciGetInPortSignals(MTHNDL* hdl)
{
	USHORT	sgnl;
	USHORT	inSig = (USHORT)~SviGetInpSig(hdl->svRsp);

	sgnl  = _BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[0]);		/* IO:0 */
	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[1])<<1);	/* IO:1 */
	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[2])<<2);	/* IO:2 */
	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[3])<<3);	/* IO:3 */
//	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[4])<<4);	/* IO:4 */
//	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[5])<<5);	/* IO:5 */
//	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[6])<<6);	/* IO:6 */
//	sgnl |= (_BITCHK(inSig, hdl->mtPrm.InSigMap.bitNo[7])<<7);	/* IO:7 */

	return (sgnl & hdl->mtPrm.InSigMap.bitsMsk);
}
#endif

/****************************************************************************
 セーフティモジュールモニタデータ取得
****************************************************************************/
#if 0 && ( CCSW_SFTID == SFTID_EXIST )											 /*<V717>Add:Start*/
USHORT MciGetSomStatus(void)
{
	USHORT	sgnl=0;
	BOOL	bitVal;
	bitVal = SviGetSfFncOn();
	sgnl |= (bitVal << 11);
	bitVal = SviGetSafeOn();
	sgnl |= (bitVal << 12);
	bitVal = SviGetAmSts();													 /*<V729>Add*/
	sgnl |= (bitVal << 13);													 /*<V729>Add*/
	return sgnl;
}
#endif																		 /*<V717>Add:End  */


/****************************************************************************
 各種データ取得
****************************************************************************/
//UCHAR MciGetMType(MTHNDL* hdl)			{return hdl->crtMtype;}
//BOOL MciGetServoOnStatus(MTHNDL* hdl)	{return SviGetServoOnSts(hdl->svRsp);}
//BOOL MciGetSensOnStatus(MTHNDL* hdl)	{return SviGetSensOnSts(hdl->svRsp);}
//UCHAR MciGetCtrlMode(MTHNDL* hdl)		{return hdl->cmdData.CtrlMode;}
//BOOL MciGetDenStatus(MTHNDL* hdl)		{return PsnGetDenSts(&hdl->psnHdl);}
//BOOL MciGetInPosStatus(MTHNDL* hdl)		{return PsnGetPsetSts(&hdl->psnHdl);}
//BOOL MciGetNearStatus(MTHNDL* hdl)		{return PsnGetNearSts(&hdl->psnHdl);}
//BOOL MciGetZpointStatus(MTHNDL* hdl)	{return PsnGetZPointSts(&hdl->psnHdl);}
//UCHAR MciGetSoftLimitStatus(MTHNDL* hdl){return PsnGetSOTSts(&hdl->psnHdl);}
//BOOL MciGetZspdStatus(MTHNDL* hdl)		{return !SviGetTGonSts(hdl->svRsp);}
//BOOL MciGetMainPowerStatus(MTHNDL* hdl)	{return SviGetMainPowerSts(hdl->svRsp);}
//BOOL MciGetVlimitStatus(MTHNDL* hdl)	{return SviGetVLmtSts(hdl->svRsp);}
//BOOL MciGetTrqLimitStatus(MTHNDL* hdl)	{return SviGetTLmtSts(hdl->svRsp);}
//BOOL MciGetVcmpStatus(MTHNDL* hdl)		{return SviGetVCmpSts(hdl->svRsp);}
//LONG MciGetOutputCmdDist(MTHNDL* hdl)	{return hdl->outData.DposIn;}

