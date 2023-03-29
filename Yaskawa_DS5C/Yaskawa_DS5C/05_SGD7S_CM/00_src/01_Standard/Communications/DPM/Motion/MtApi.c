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
#include "Basedef.h"
#include "MtApi.h"
#include "MLib.h"


#define MT_STATE_CHECK

/****************************************************************************
 加減速指令データ設定
****************************************************************************/
VOID mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd)
{
	if((hdl->mtPrm.BasePrm.SetCmdAccRate != NULL) && (cmd->EnableRate != FALSE))
	{/* 有効コマンド && 加減速指令パラメータSW有効 */
		if( (cmd->RstReq != FALSE)
			|| (hdl->cmdData.CmdRateOn == FALSE) 
			|| (hdl->psnHdl.Den != FALSE) )
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

/****************************************************************************
 位置・速度制御時トルクデータ設定
 
 	トルクFFの単位変換	：[2^14/MaxTrq] -> [2^30/MaxTrq]	16Bitシフト
	トルク制限の単位変換：[2^14/MaxTrq] -> [2^24/MaxTrq]	10Bitシフト
****************************************************************************/
VOID mciSetTrqData(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR trefMode)
{
	switch(trefMode)
	{
	case TREFMODE_TFF:	/*--- TFF有効 ------------------------------------------*/
		hdl->cmdData.Trq = (LONG)cmd->Trq << 16;			/* TFFセット		*/
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;				/* TRQ LIMITセット	*/
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;				/* TRQ LIMITセット	*/
		break;
		
	case TREFMODE_TLIM1:/*--- トルクリミット有効 -------------------------------*/
	case TREFMODE_TLIM2:/*--- P_CL/N_CLでトルクリミット有効 --------------------*/
		hdl->cmdData.Trq = 0;								/* TFFセット		*/
		hdl->svCmd->ExtCmd1.dwVal = (cmd->TrqLimitP << 10);	/* TRQ LIMITセット	*/
		hdl->svCmd->ExtCmd2.dwVal = (cmd->TrqLimitN << 10);	/* TRQ LIMITセット	*/
		break;
		
	default: 			/*--- トルクリミット無効 -------------------------------*/
		hdl->cmdData.Trq = 0;								/* TFFセット		*/
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;				/* TRQ LIMITセット	*/
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;				/* TRQ LIMITセット	*/
		break;
	}
}


/****************************************************************************
 INTERPOLATE指令設定
****************************************************************************/
INT32 MciSetInterpolateCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - hdl->psnHdl.Ipos;

/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}
/* 指令速度チェック */
	if((ULONG)MlibLABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
	{
		return MCI_SPD_ERR;
	}
/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット			*/
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFFセット			*/
	mciSetTrqData(hdl, cmd,	hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIMセット */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LTモードクリア */
	}
	
	return MCI_SUCCESS;
}

/****************************************************************************
 LATCH指令設定
****************************************************************************/
INT32 MciSetLatchCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - hdl->psnHdl.Ipos;

/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}

/* 指令速度チェック */
	if((ULONG)MlibLABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
	{
		return MCI_SPD_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット	*/
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット			*/
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFFセット			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIMセット */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/
	
	MciRequestMtLatch(hdl, INTERPOL_LTMD, cmd->LatchSgnl);/* ラッチ要求 */

	return MCI_SUCCESS;
}

/****************************************************************************
 POSING指令設定
****************************************************************************/
INT32 MciSetPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}

/* 指令速度チェック */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_POSING;			/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット 	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット 			*/
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット 			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/* TFF,TLIMセット */
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/
	
	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LTモードクリア */
	}
	
	return MCI_SUCCESS;
}

/****************************************************************************
 EX_POSING指令設定
****************************************************************************/
INT32 MciSetExPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}
	
/* 指令速度チェック */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_EXPOSING;		/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* コマンド更新セット	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット			*/
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット			*/	
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/
	
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ラッチ信号セット		*/
	hdl->latch.Mode = EXPOSING_LTMD;			/* MOTION LT MODEセット */

	return MCI_SUCCESS;	
}

/****************************************************************************
 HOMING指令設定
****************************************************************************/
INT32 MciSetHomingCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}
	
/* 指令速度チェック */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_HOMING;			/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* コマンド更新セット	*/
	
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/
	
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ラッチ信号セット		*/
	hdl->latch.Mode = ZRET_LTMD;				/* MOTION LT MODEセット */

	return MCI_SUCCESS;	
}

/****************************************************************************
 FEED指令設定
****************************************************************************/
INT32 MciSetFeedCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* RUN状態チェック */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}

/* 指令速度チェック */
	if((ULONG)MlibLABS(cmd->TSpeed) > hdl->mtPrm.BasePrm.PosingSpdMax)
	{
		return MCI_SPD_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_FEED;			/* MTYPEセット 			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット	*/
	
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPDセット			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	mciSetAccRate(hdl, cmd);					/* 加減速度セット<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LTモードクリア */
	}
	

	return MCI_SUCCESS;	
}

/****************************************************************************
 HOLD指令設定
****************************************************************************/
INT32 MciSetHoldCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* モーション状態チェック */
	if(hdl->mtState == MSTAT_DISABLE)
	{
		return MCI_CNDTN_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_HOLD;			/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット	*/
	hdl->cmdData.HoldMod = cmd->HoldMod;		/* Holdモードセット		*/
	
	hdl->cmdData.TSpeed = 0;					/* TSPDセット 			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIMセット*/
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LTモードクリア */
	}
	

	return MCI_SUCCESS;	
}

/****************************************************************************
 VELCTRL指令設定
****************************************************************************/
INT32 MciSetVelCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* モーション状態チェック */
	if(hdl->mtState == MSTAT_DISABLE)
	{
		return MCI_CNDTN_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;		/* 速度制御モードセット */
	hdl->cmdData.Mtype = MTYPE_VELCTRL;			/* MTYPEセット 			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット	*/
	
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vrefセット			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.trefMode);/*TFF,TLIMセット*/
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LTモードクリア */
	}
	

	return MCI_SUCCESS;
}

/****************************************************************************
 TRQCTRL指令設定
****************************************************************************/
INT32 MciSetTrqCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* モーション状態チェック */
	if(hdl->mtState == MSTAT_DISABLE)
	{
		return MCI_CNDTN_ERR;
	}

/* モーション指令データセット */
	hdl->cmdData.CtrlMode = MT_TRQ_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_TRQCTRL;			/* MTYPEセット			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット	*/
	
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vrefセット			*/
	hdl->cmdData.Trq = cmd->Trq;				/* Trefセット			*/
	hdl->svCmd->ExtCmd1.dwVal = (cmd->TrqLimitP << 10);	/* TRQ LIMITセット	*/
	hdl->svCmd->ExtCmd2.dwVal = (cmd->TrqLimitN << 10);	/* TRQ LIMITセット	*/
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option指令セット */
	
	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LTモードクリア */
	}
	

	return MCI_SUCCESS;
}

/****************************************************************************
 OPTION指令設定
****************************************************************************/
VOID MciSetOptionCmd(MT_HNDL* hdl, ULONG option)
{
	ULONG 	bitVal, CmdSeq;
	USHORT	CmdCtrl;
	
/* 初期値取得	*/
	CmdCtrl = hdl->svCmd->CmdCtrl.CmdCtrlBit & (~(ENBPCTRL_BIT | CLRPOSINTG_BIT));
	CmdSeq = hdl->svCmd->CmdSeqBit & (~(ENB_EXTPTRQLMT_BIT | ENB_EXTNTRQLMT_BIT));

/* ACC_FIL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.accfilBit) & hdl->mtPrm.OptMap.accfilBitMsk);
	hdl->cmdData.AccFilSel = (UCHAR)bitVal;
/* <S1D0> Start */
/* TFF_FIL_INV */
//	hdl->svCmd->CmdCtrl.tffFilSts = (BOOL)(option >> TFFFILINV_BIT) & TFFFILINV_BITMSK;

/* VFF_FIL_INV */
//	hdl->svCmd->CmdCtrl.vffFilSts = (BOOL)(option >> VFFFILINV_BIT) & VFFFILINV_BITMSK;
/* <S1D0> End */
/* G_SEL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.gselBit) & hdl->mtPrm.OptMap.gselBitMsk);
	hdl->svCmd->CmdCtrl.SelBank = (UCHAR)bitVal;

/* P_PI */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.vppiBit) & hdl->mtPrm.OptMap.vppiBitMsk);
	CmdCtrl |=  (USHORT)(bitVal << ENBPCTRL_BITNO);

/* P_I_CLR */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.ppiclrBit) & hdl->mtPrm.OptMap.ppiclrBitMsk);
	CmdCtrl |=  (USHORT)(bitVal << CLRPOSINTG_BITNO);

/* N_CL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.nclBit) & hdl->mtPrm.OptMap.nclBitMsk);
//	CmdSeq |= bitVal << ENB_EXTPTRQLMT_BITNO;
	CmdSeq |= bitVal << ENB_EXTNTRQLMT_BITNO;/* <S16B> */

/* P_CL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.pclBit) & hdl->mtPrm.OptMap.pclBitMsk);
//	CmdSeq |= bitVal << ENB_EXTNTRQLMT_BITNO;
	CmdSeq |= bitVal << ENB_EXTPTRQLMT_BITNO;/* <S16B> */

/* OUT_SIGNAL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.outpBit) & hdl->mtPrm.OptMap.outpBitMsk);
	hdl->svCmd->DOSignal = bitVal;

/* BANKSEL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.bankBit) & hdl->mtPrm.OptMap.bankBitMsk);
	if( bitVal < hdl->mtPrm.Bnk.bankNum )
	{
		hdl->mtPrm.Bnk.cmdSel = bitVal;
	}
	
/* LT_DISABLE */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.ltBit) & hdl->mtPrm.OptMap.ltBitMsk);
	hdl->latch.LtDisable = bitVal;
	
/* 設定した値を格納する	*/
	hdl->svCmd->CmdCtrl.CmdCtrlBit = CmdCtrl;
	hdl->svCmd->CmdSeqBit = CmdSeq;
}


/****************************************************************************
 モーション状態リセット
****************************************************************************/
INT32 MciResetMotion(MT_HNDL* hdl)
{
	INT32 rc;
	
	/* ローカル変数の初期化 */
	rc = MCI_WAIT_COMP;

	if( (hdl->mtState == MSTAT_READY)
		|| (hdl->mtState == MSTAT_WAITPG_RDY)
		|| (hdl->mtState == MSTAT_DISABLE))
	{
		MciSetOptionCmd(hdl, 0);			/* Option指令クリア */
		MtcCancelLatch(hdl, NONE_LTMD);		/* ラッチクリア */
		PsnResetApos(&hdl->psnHdl, FALSE);	/* Positionリセット */
		hdl->cmdData.SensOn = FALSE;		/* SENS_OFF */
		hdl->cmdData.MtEenable = FALSE;		/* Motion無効 */

		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		if( hdl->mtState == MSTAT_DISABLE
				 && ( 0 == (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) ) )
		{/* 既にDISABLE状態なら完了 ＆ センオフ完了 */
			rc = MCI_SUCCESS;
		}
	}
	else
	{
		rc = MCI_CNDTN_ERR;
	}

	return rc;
}

/****************************************************************************
 モーション状態切り替え
****************************************************************************/
INT32 MciEnableState(MT_HNDL* hdl, BOOL enable)
{
	INT32 rc;

	/* ローカル変数の初期化 */
	rc = MCI_WAIT_COMP;

	if(enable)
	{/* モーション有効要求ON */
		hdl->cmdData.MtEenable = TRUE;
		if(hdl->mtState != MSTAT_DISABLE)
		{
			rc = MCI_SUCCESS;	/* DISABLE状態でなければ完了 */
		}
	}
	else
	{/* モーション有効要求OFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{
			hdl->cmdData.MtEenable = FALSE;
			if(hdl->mtState == MSTAT_DISABLE)
			{
				rc = MCI_SUCCESS;	/* 既にDISABLE状態なら完了 */
			}
		}
		else
		{
			rc = MCI_CNDTN_ERR;
		}
	}
	return rc;
}

/****************************************************************************
 サーボオン指令発行
****************************************************************************/
INT32 MciSetServoOn( MT_HNDL *hdl, BOOL enable, BOOL reset )
{
	INT32 rc;

	rc = MCI_WAIT_COMP;

	if( enable )
	{ /* ServoON */
		if( ((hdl->svRsp->RspSeqBit & MAIN_POWER_ON_BITMSK) == 0)	/* 主回路電源OFF */
			|| ((hdl->svRsp->RspSeqBit & ALM_DETECTED_BITMSK) != 0)			/* サーボアラーム発生状態 */
			|| (hdl->mtState == MSTAT_DISABLE)
			|| (hdl->mtState == MSTAT_WAITPG_RDY) )
		{ /* 主回路断 or ALM or Motion準備未完 */
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_CNDTN_ERR;
		}
//		else if( 0 != (hdl->svRsp->RspSigBit & ESTP_SIG_BITMSK) )
		else if( 0 != (hdl->svRsp->RspSigBit & HWBB_SIG_BITMSK) )/* <S156> */
		{ /* EMG状態 */
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_EMGBB_ERR;
		}
		else
		{
			if( !hdl->cmdData.SvonLock || reset )
			{
				hdl->cmdData.SvonLock = FALSE;
				hdl->cmdData.ServoOn = TRUE;
				if( hdl->mtState == MSTAT_RUN )
				{ /* RUN状態なら完了 */
					rc = MCI_SUCCESS;
				}
			}
			else
			{
				rc = MCI_SUCCESS;
			}
		}
	}
	else
	{ /* ServoOFF */
		hdl->cmdData.SvonLock = FALSE;
		hdl->cmdData.ServoOn = FALSE;
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{ /* サーボOFF完了 */
			rc = MCI_SUCCESS;
		}
	}

	return rc;
}

/****************************************************************************
 センサオン指令発行
****************************************************************************/
INT32 MciSetSensOn( MT_HNDL *hdl, BOOL enable )
{
	INT32 rc;

	rc = MCI_WAIT_COMP;

	if( enable )
	{ /* SensorON */
		if( (0 != (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK))/* 既にSvON状態(JOG中もOKとするため) */
			|| ((hdl->mtState != MSTAT_WAITPG_RDY) 				/* or SensOn完了状態 */
				&& (hdl->mtState != MSTAT_DISABLE)) )
		{ /* Sensor On 完了 */
			rc = MCI_SUCCESS;
		}
		else
		{ /* Sensor On処理未完状態 */
			if( 0 == (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) )
			{ /* SensOff状態を確認後要求セット */
				hdl->cmdData.SensOn = TRUE;
			}
		}
	}
	else
	{ /* SensorOFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE) )
		{ /* サーボOFF状態のみ */
			if( hdl->mtPrm.PsnPrm.absPG )
			{ /* ABSの場合のみ、Sensor Off */
				hdl->cmdData.SensOn = FALSE;
			}
			hdl->psnHdl.RefEnable = FALSE;
			rc = MCI_SUCCESS;
		}
		else
		{ /* SensorOFFできないのでエラー */
			rc = MCI_CNDTN_ERR;
		}
	}

	return rc;
}

/****************************************************************************
 ラッチ要求発行(Motion用)
****************************************************************************/
INT32 MciRequestMtLatch(MT_HNDL* hdl, LONG mode, LONG sgnl)
{
	if((hdl->latch.Mode != mode) 		/* ラッチモード不一致 */
		|| !(hdl->latch.Cmp | hdl->latch.Req))	/* ラッチなし */
	{
		MtcCancelLatch(hdl, mode);
		MtcReqLatch(hdl, mode, sgnl);
	}
	return MCI_SUCCESS;
}

/****************************************************************************
 Modalラッチ要求発行(mode=0:通常ラッチ、=1:連続ラッチ)
****************************************************************************/
INT32 MciRequestMdlLatch(MT_HNDL* hdl, UCHAR mode, LONG sgnl)
{
	if(hdl->latch.Mode == NONE_LTMD)
	{/* ラッチモードなし時のみ要求発行 */
		if(mode == 1)
		{/* シーケンシャルモード */
			if(hdl->mtPrm.LtPrm.seqNum == 0)
			{
				return MCI_LTMOD_ERR;
			}

			hdl->latch.SeqReq = MTLT_SEQ_REQ;
			sgnl = hdl->mtPrm.LtPrm.ltSgnl[0];
		}
		MtcReqLatch(hdl, MODAL_LTMD, sgnl);
	}
	else
	{/* 現在ラッチモード≠Modalラッチモードならばエラー */
		if(hdl->latch.Mode != MODAL_LTMD)
		{
			return MCI_LTCNFLCT_ERR;
		}
	}
	return MCI_SUCCESS;
}

/****************************************************************************
 ラッチキャンセル
****************************************************************************/
INT32 MciCancelLatch(MT_HNDL* hdl, LONG mode)
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
INT32 MciSetCordinatePos(MT_HNDL* hdl, LONG data, BOOL ref)
{
	PsnSetCordinate(&hdl->psnHdl, 0, data);
	if(ref == TRUE)
	{
		hdl->psnHdl.RefEnable = ref;
	}
	hdl->cmdData.TPos = hdl->psnHdl.Tpos;

	return MCI_SUCCESS;
}

/****************************************************************************
 リモートI/O出力
****************************************************************************/
VOID MciOutputSignals(MT_HNDL* hdl, USHORT signl)
{
	hdl->svCmd->DOSignal = (UCHAR)signl;
}




/* Motion状態取得API */
/****************************************************************************
 LATCHステータス取得
****************************************************************************/
BOOL MciGetLatchStatus(MT_HNDL* hdl, LONG *ltEdge)
{
	BOOL lcmp;

	*ltEdge = (hdl->latch.Cmp >> 4);		/* bit4状態 */
	hdl->latch.Cmp = hdl->latch.Cmp & 0x1;	/* bit4:OFF */
	if( hdl->latch.SeqReq != 0 )
	{
		lcmp = *ltEdge;
	}
	else
	{
		lcmp = hdl->latch.Cmp;
	}
	return lcmp;
}

/****************************************************************************
 モニタデータ取得
****************************************************************************/
LONG MciGetMonitorData(MT_HNDL* hdl, USHORT sel, UCHAR unit)
{
	LONG data;
	
	switch(sel)
	{
	case POS_MON:	data = hdl->psnHdl.Pos; break;
	case MPOS_MON:	data = hdl->psnHdl.Mpos;break;
	case PERR_MON:	data = hdl->psnHdl.Perr;break;
	case APOS_MON:	data = hdl->psnHdl.Apos;break;
	case LPOS_MON:	data = hdl->psnHdl.Lpos;break;
	case IPOS_MON:	data = hdl->psnHdl.Ipos;break;
	case TPOS_MON:	data = hdl->psnHdl.Tpos;break;
	case FSPD_MON:
		if(unit == 2)	/* [30bit/MAX] */
			data = MlibMulgain32(hdl->svRsp->Mon3, hdl->mtPrm.BasePrm.osToMaxKx);
		else			/* [ref/s] */
			data = MlibMulgain32(hdl->svRsp->Mon3, hdl->mtPrm.BasePrm.osToRefKx);
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
//		data =  hdl->svCmd->ExtCmd1.dwVal;	/* [24bit/MAX] */
		data =  hdl->svRsp->Mon2;	/* [24bit/MAX] *//* <S14D> */
		if(unit == 3)	/* [30bit/MAX] */
			data = data << 6;
		else			/*[%] */
			data = MlibMulgain(data, hdl->mtPrm.BasePrm.trqKx);
		break;
	case OMN1_MON:	
		if(hdl->svCmd->SelMon5 == 0x80)
		{
			data = hdl->psnHdl.Lpos;
		}
		else
		{
			data = hdl->svRsp->Mon5;
		}
		break;
	case OMN2_MON:	
		if(hdl->svCmd->SelMon6 == 0x80)
		{
			data = hdl->psnHdl.Lpos;
		}
		else
		{
			data = hdl->svRsp->Mon6;
		}
		break;
	default:
		data = 0;
		break;
	}
	return data;
}

/****************************************************************************
 I/Oモニタデータ取得
****************************************************************************/
USHORT MciGetInputSignals(MT_HNDL* hdl)
{
	BOOL	bitVal;
	ULONG	pls;
	USHORT	sgnl=0;

	/*---- P_OT ------------*/
	bitVal = ((hdl->svRsp->RspSigBit >> P_OT_SIG_BITNO) & 1);
	sgnl |= (bitVal << P_OT_SGNL);
	
	/*---- N_OT ------------*/
	bitVal = ((hdl->svRsp->RspSigBit >> N_OT_SIG_BITNO) & 1);
	sgnl |= (bitVal << N_OT_SGNL);

	/*---- DEC -------------*/
	bitVal = ((hdl->svRsp->RspSigBit >> DEC_SIG_BITNO) & 1);
	sgnl |= (bitVal << DEC_SGNL);
	
	/*---- PG Pulse入力 ----*/
	pls = ((hdl->svRsp->Mon7 >> 16) & 0x7);
	sgnl |= (pls << PA_SGNL);
	
	/*---- EXT1 ------------*/
	bitVal = ((hdl->svRsp->RspSigBit >> EXT1_SIG_BITNO) & 1);
	sgnl |= (bitVal << EXT1_SGNL);
	
	/*---- EXT2 ------------*/
	bitVal = ((hdl->svRsp->RspSigBit >> EXT2_SIG_BITNO) & 1);
	sgnl |= (bitVal << EXT2_SGNL);
	
	/*---- EXT3 ------------*/
	bitVal = ((hdl->svRsp->RspSigBit >> EXT3_SIG_BITNO) & 1);
	sgnl |= (bitVal << EXT3_SGNL);
	
	/*---- BRK -------------*/
	bitVal = ((~(hdl->svRsp->RspSeqBit) >> BRK_RELEASING_BITNO) & 0x01);
	sgnl |= (bitVal << BK_SGNL);
	
	/*---- EMG(ESTP) -------*/
//	bitVal = ((hdl->svRsp->RspSigBit >> ESTP_SIG_BITNO) & 1);
	bitVal = ((hdl->svRsp->RspSigBit >> HWBB_SIG_BITNO) & 1);/* <S0AF> */
	sgnl |= (bitVal << EMG_SGNL);

	return sgnl;
}

USHORT MciGetInPortSignals(MT_HNDL *hdl)
{
	USHORT	sgnl;
	USHORT	inSig;
	
	/* SV_OPTION_IFから現在値を取得	*/
	inSig = (USHORT)(~hdl->svRsp->Mon7);
	
 	sgnl = ((inSig >> hdl->mtPrm.InSigMap.bitNo[0])&1);			/* IO:0 */
 	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[1])&1) << 1;	/* IO:1 */
 	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[2])&1) << 2;	/* IO:2 */
 	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[3])&1) << 3;	/* IO:3 */
// 	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[4])&1) << 4;	/* IO:4 */
// 	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[5])&1) << 5;	/* IO:5 */
//	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[6])&1) << 6;	/* IO:6 */
// 	sgnl |= ((inSig >> hdl->mtPrm.InSigMap.bitNo[7])&1) << 7;	/* IO:7 */

	return (sgnl & hdl->mtPrm.InSigMap.bitsMsk);
}
#if 0
/****************************************************************************
 セーフティモジュールモニタデータ取得
****************************************************************************/
#if ( CCSW_SFTID == SFTID_EXIST )											 /*<V717>Add:Start*/
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
#endif
