/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion API		 													*/
/*																			*/
/*																			*/
/****************************************************************************/
/*	Histroy:																*/
/*		Rev2.01 2007.12.14	�����x�w�ߒǉ�									*/
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

/* Motion��Ԏ擾API */
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

#define MciSetOptionCmd(x, y)	{;}	/* SGDV+INDEXER(�b��) */

/****************************************************************************
 �������w�߃f�[�^�ݒ�
****************************************************************************/
#if 0
VOID mciSetAccRate(MTHNDL* hdl, MCI_CMD* cmd)
{
	if((hdl->mtPrm.BasePrm.SetCmdAccRate != NULL) && (cmd->EnableRate != FALSE))
	{/* �L���R�}���h && �������w�߃p�����[�^SW�L�� */
		if( (cmd->RstReq != FALSE)
			|| (hdl->cmdData.CmdRateOn == FALSE) 
			|| (PsnGetDenSts(&hdl->psnHdl) != FALSE) )
		{/* �R�}���h�؂�ւ�� && DEN=1 */
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
 �ʒu�E���x���䎞�g���N�f�[�^�ݒ�
****************************************************************************/
#if 0
VOID mciSetTrqData(MTHNDL* hdl, MCI_CMD* cmd, UCHAR trefMode)
{
	switch(trefMode)
	{
	case TREFMODE_TFF:	/* TFF�L�� */
		hdl->cmdData.Trq = (LONG)cmd->Trq;		/* TFF�Z�b�g */
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMIT�Z�b�g */
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMIT�Z�b�g */
		break;
	case TREFMODE_TLIM1:/* �g���N���~�b�g�L�� */
	case TREFMODE_TLIM2:
		hdl->cmdData.Trq = 0;								/* TFF�Z�b�g */
		SviSetTrqLimitP(hdl->svCmd, cmd->TrqLimitP >> 6);	/* TRQ LIMIT�Z�b�g */
		SviSetTrqLimitN(hdl->svCmd, cmd->TrqLimitN >> 6);	/* TRQ LIMIT�Z�b�g */
		break;
	default: 			/* �g���N���~�b�g���� */
		hdl->cmdData.Trq = 0;					/* TFF�Z�b�g */
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMIT�Z�b�g */
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);	/* TRQ LIMIT�Z�b�g */
		break;
	}
}
#endif


/****************************************************************************
 INTERPOLATE�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetInterpolateCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - PsnGetIPOS(&hdl->psnHdl);

/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* �w�ߑ��x�`�F�b�N */
	if((ULONG)MlibABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
		return MCI_SPD_ERR;

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPE�Z�b�g */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g */
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFF�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIM�Z�b�g*/	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciClearMtLatch(hdl);						/* MOTION LT���[�h�N���A */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 LATCH�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetLatchCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - PsnGetIPOS(&hdl->psnHdl);

/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* �w�ߑ��x�`�F�b�N */
	if((ULONG)MlibABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
		return MCI_SPD_ERR;

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPE�Z�b�g */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g */
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFF�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIM�Z�b�g*/
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	MciRequestMtLatch(hdl, INTERPOL_LTMD, cmd->LatchSgnl);/* ���b�`�v�� */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 POSING�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetPosingCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* �w�ߑ��x�`�F�b�N */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_POSING;			/* MTYPE�Z�b�g */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciClearMtLatch(hdl);						/* MOTION LT���[�h�N���A */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 EX_POSING�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetExPosingCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* �w�ߑ��x�`�F�b�N */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_EXPOSING;		/* MTYPE�Z�b�g */
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g */
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ���b�`�M���Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciSetMtLtMode(hdl, EXPOSING_LTMD);			/* MOTION LT MODE�Z�b�g */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 HOMING�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetHomingCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* �w�ߑ��x�`�F�b�N */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_HOMING;			/* MTYPE�Z�b�g */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g */
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ���b�`�M���Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciSetMtLtMode(hdl, ZRET_LTMD);				/* MOTION LT MODE�Z�b�g */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 FEED�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetFeedCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
		return MCI_CNDTN_ERR;

/* �w�ߑ��x�`�F�b�N */
	if((ULONG)MlibABS(cmd->TSpeed) > hdl->mtPrm.BasePrm.PosingSpdMax)
		return MCI_SPD_ERR;

	hdl->cmdData.Mtype = MTYPE_FEED;			/* MTYPE�Z�b�g */
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciClearMtLatch(hdl);						/* MOTION LT���[�h�N���A */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 HOLD�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetHoldCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	if(hdl->mtState == MSTAT_DISABLE)
		return MCI_CNDTN_ERR;

	hdl->cmdData.Mtype = MTYPE_HOLD;			/* MTYPE�Z�b�g */
	hdl->cmdData.HoldMod = cmd->HoldMod;		/* Hold���[�h�Z�b�g */
	hdl->cmdData.TSpeed = 0;					/* TSPD�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciClearMtLatch(hdl);						/* MOTION LT���[�h�N���A */

	return MCI_SUCCESS;	
}
#endif

/****************************************************************************
 VELCTRL�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetVelCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	if(hdl->mtState == MSTAT_DISABLE)
		return MCI_CNDTN_ERR;

	hdl->cmdData.Mtype = MTYPE_VELCTRL;			/* MTYPE�Z�b�g */
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vref�Z�b�g */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;		/* ���x���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.trefMode);	/*TFF,TLIM*/
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciClearMtLatch(hdl);						/* MOTION LT���[�h�N���A */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 TRQCTRL�w�ߐݒ�
****************************************************************************/
#if 0
INT32 MciSetTrqCmd(MTHNDL* hdl, MCI_CMD* cmd)
{
	if(hdl->mtState == MSTAT_DISABLE)
		return MCI_CNDTN_ERR;

	hdl->cmdData.Mtype = MTYPE_TRQCTRL;			/* MTYPE�Z�b�g */
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vref�Z�b�g */
	hdl->cmdData.Trq = cmd->Trq;				/* TFF�Z�b�g */
	hdl->cmdData.CtrlMode = MT_TRQ_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	SviSetTrqLimitP(hdl->svCmd, cmd->TrqLimitP >> 6);/* TRQ LIMIT�Z�b�g */
	SviSetTrqLimitN(hdl->svCmd, cmd->TrqLimitN >> 6);/* TRQ LIMIT�Z�b�g */
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	mciClearMtLatch(hdl);						/* MOTION LT���[�h�N���A */

	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 OPTION�w�ߐݒ�
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
 ���[�V������ԃ��Z�b�g
****************************************************************************/
#if 0
INT32 MciResetMotion(MTHNDL* hdl)
{
	INT32 rc = MCI_WAIT_COMP;

	if( (hdl->mtState == MSTAT_READY)
		|| (hdl->mtState == MSTAT_WAITPG_RDY)
		|| (hdl->mtState == MSTAT_DISABLE))
	{
		MciSetOptionCmd(hdl, 0);			/* Option�w�߃N���A */
		MtcCancelLatch(hdl, NONE_LTMD);		/* ���b�`�N���A */
		PsnResetApos(&hdl->psnHdl, FALSE);	/* Position���Z�b�g */
		hdl->cmdData.SensOn = FALSE;		/* SENS_OFF */
		hdl->cmdData.MtEenable = FALSE;		/* Motion���� */

		SviSetSensOn( hdl->svCmd, FALSE );										/*<V887>*/
		if( hdl->mtState == MSTAT_DISABLE && ! SviGetSensOnSts( hdl->svRsp ) )	/*<V887>*/
			rc = MCI_SUCCESS;	/* ����DISABLE��ԂȂ犮�� */
	}
	else
	{	rc = MCI_CNDTN_ERR;}

	return rc;
}
#endif

/****************************************************************************
 ���[�V������Ԑ؂�ւ�
****************************************************************************/
#if 0
INT32 MciEnableState(MTHNDL* hdl, BOOL enable)
{
	INT32 rc = MCI_WAIT_COMP;

	if(enable)
	{/* ���[�V�����L���v��ON */
		hdl->cmdData.MtEenable = TRUE;
		if(hdl->mtState != MSTAT_DISABLE)
			rc = MCI_SUCCESS;	/* DISABLE��ԂłȂ���Ί��� */
	}
	else
	{/* ���[�V�����L���v��OFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{
			hdl->cmdData.MtEenable = FALSE;
			if(hdl->mtState == MSTAT_DISABLE)
				rc = MCI_SUCCESS;	/* ����DISABLE��ԂȂ犮�� */
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
 �T�[�{�I���w�ߔ��s
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
		{/* ���H�f or ALM or Motion�������� */
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_CNDTN_ERR;
		}
		else if(emg)
		{
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_EMGBB_ERR;			/* EMG��� */
		}
		else
		{
			if(!hdl->cmdData.SvonLock || reset)
			{	
				hdl->cmdData.SvonLock = FALSE;
				hdl->cmdData.ServoOn = TRUE;
				if(hdl->mtState == MSTAT_RUN)
				{	rc = MCI_SUCCESS;		/* RUN��ԂȂ犮�� */}
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
		{/* �T�[�{OFF���� */
			rc = MCI_SUCCESS;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 �Z���T�I���w�ߔ��s
****************************************************************************/
#if 0
INT32 MciSetSensOn(MTHNDL* hdl, BOOL enable)
{
	INT32 rc = MCI_WAIT_COMP;

	if(enable)
	{/* SensorON */
//		hdl->cmdData.SensOn = TRUE;		/*<V544>*/
		if( (SviGetServoOnSts(hdl->svRsp) != FALSE)		/* ����SvON���(JOG����OK�Ƃ��邽��) */
			|| ((hdl->mtState != MSTAT_WAITPG_RDY) 		/* or SensOn������� */
				&& (hdl->mtState != MSTAT_DISABLE)) )
		{/* Sensor On ���� */
			rc = MCI_SUCCESS;
		}
		/*<V544> start */
		else
		{/* Sensor On����������� */
			if(SviGetSensOnSts(hdl->svRsp) == FALSE)
			{/* SensOff��Ԃ��m�F��v����� */
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
		{/* �T�[�{OFF��Ԃ̂� */
			if(hdl->mtPrm.PsnPrm.absPG)
			{/* ABS�̏ꍇ�̂݁ASensor Off */
				hdl->cmdData.SensOn = FALSE;
			}
			PsnSetRefPoint(&hdl->psnHdl, FALSE);
			rc = MCI_SUCCESS;
		}
		else
		{/* SensorOFF�ł��Ȃ��̂ŃG���[ */
			rc = MCI_CNDTN_ERR;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 ���b�`�v�����s(Motion�p)
****************************************************************************/
#if 0
INT32 MciRequestMtLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl)
{
	if((hdl->latch.Mode != mode) 		/* ���b�`���[�h�s��v */
		|| !(hdl->latch.Cmp | hdl->latch.Req))	/* ���b�`�Ȃ� */
	{
		MtcCancelLatch(hdl, mode);
		MtcReqLatch(hdl, mode, sgnl);
	}
	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 Modal���b�`�v�����s(mode=0:�ʏ탉�b�`�A=1:�A�����b�`)
****************************************************************************/
#if 0
INT32 MciRequestMdlLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl)
{
	if(hdl->latch.Mode == NONE_LTMD)
	{/* ���b�`���[�h�Ȃ����̂ݗv�����s */
		if(mode == 1)
		{/* �V�[�P���V�������[�h */
			if(hdl->mtPrm.LtPrm.seqNum == 0)
				return MCI_LTMOD_ERR;

			hdl->latch.SeqReq = MTLT_SEQ_REQ;
			sgnl = hdl->mtPrm.LtPrm.ltSgnl[0];
		}
		MtcReqLatch(hdl, MODAL_LTMD, sgnl);
	}
	else
	{/* ���݃��b�`���[�h��Modal���b�`���[�h�Ȃ�΃G���[ */
		if(hdl->latch.Mode != MODAL_LTMD)
			return MCI_LTCNFLCT_ERR;
	}
	return MCI_SUCCESS;
}
#endif

/****************************************************************************
 ���b�`�L�����Z��
****************************************************************************/
INT32 MciCancelLatch(MTHNDL* hdl, UCHAR mode)
{
	if(mode == MODAL_LTMD)
	{/* Modal���b�`�L�����Z�� */
		if( (hdl->latch.Mode != NONE_LTMD)
			&& (hdl->latch.Mode != MODAL_LTMD) )
		{/* ���b�`���[�h���� && MODAL_LTMD�ȊO�́A�G���[ */
			return MCI_LTCNFLCT_ERR;
		}
		MtcCancelLatch(hdl, NONE_LTMD);
	}
	else if(mode == MOTION_MDL_LTMD)
	{/* Motion Modal�L�����Z�� */
		MtcCancelLatch(hdl, MOTION_MDL_LTMD);
	}
	else
	{/* �ʏ� Motion�L�����Z�� */
		if( (hdl->latch.Mode == MODAL_LTMD)	
			|| ((hdl->latch.Mode == ZRET_LTMD) && (hdl->seqState != 0)) )	
		{/* ZRET���쒆 or ���[�_�����b�`���́A�������Ȃ� */
			return MCI_SUCCESS;
		}
		MtcCancelLatch(hdl, NONE_LTMD);
	}
	return MCI_SUCCESS;
}

/****************************************************************************
 ���W�n�ݒ�
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
 �����[�gI/O�o��
****************************************************************************/
#if 0
VOID MciOutputSignals(MTHNDL* hdl, USHORT signl)
{
	SviSetOutSig(hdl->svCmd, (UCHAR)signl);
}
#endif

/****************************************************************************
 BK�M��
****************************************************************************/
#if 0
VOID MciOutputBkSignal(MTHNDL* hdl, BOOL enable)
{
	SviSetBrkOff(hdl->svCmd, !enable);
}
#endif




/* Motion��Ԏ擾API */
/****************************************************************************
 LATCH�X�e�[�^�X�擾
****************************************************************************/
#if 0
BOOL MciGetLatchStatus(MTHNDL* hdl, UCHAR *ltEdge)
{
	UCHAR lcmp;

	*ltEdge = (hdl->latch.Cmp >> 4);		/* bit4��� */
	hdl->latch.Cmp = hdl->latch.Cmp & 0x1;	/* bit4:OFF */
	lcmp = (hdl->latch.SeqReq != 0) ? *ltEdge: hdl->latch.Cmp;
	return (BOOL)lcmp;
}
#endif

/****************************************************************************
 LATCH�X�e�[�^�X�擾(�V�[�P���X���b�`�p)
****************************************************************************/
#if 0
USHORT MciGetLatchSeqStatus(MTHNDL* hdl){return hdl->latch.SeqSts.wdCnt;}
UCHAR MciGetLatchSeqMode(MTHNDL* hdl){return hdl->latch.SeqReq;}
#endif



/****************************************************************************
 ���j�^�f�[�^�擾
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
 I/O���j�^�f�[�^�擾
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
 �Z�[�t�e�B���W���[�����j�^�f�[�^�擾
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
 �e��f�[�^�擾
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

