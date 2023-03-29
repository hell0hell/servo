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
#include "Basedef.h"
#include "MtApi.h"
#include "MLib.h"


#define MT_STATE_CHECK

/****************************************************************************
 �������w�߃f�[�^�ݒ�
****************************************************************************/
VOID mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd)
{
	if((hdl->mtPrm.BasePrm.SetCmdAccRate != NULL) && (cmd->EnableRate != FALSE))
	{/* �L���R�}���h && �������w�߃p�����[�^SW�L�� */
		if( (cmd->RstReq != FALSE)
			|| (hdl->cmdData.CmdRateOn == FALSE) 
			|| (hdl->psnHdl.Den != FALSE) )
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

/****************************************************************************
 �ʒu�E���x���䎞�g���N�f�[�^�ݒ�
 
 	�g���NFF�̒P�ʕϊ�	�F[2^14/MaxTrq] -> [2^30/MaxTrq]	16Bit�V�t�g
	�g���N�����̒P�ʕϊ��F[2^14/MaxTrq] -> [2^24/MaxTrq]	10Bit�V�t�g
****************************************************************************/
VOID mciSetTrqData(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR trefMode)
{
	switch(trefMode)
	{
	case TREFMODE_TFF:	/*--- TFF�L�� ------------------------------------------*/
		hdl->cmdData.Trq = (LONG)cmd->Trq << 16;			/* TFF�Z�b�g		*/
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;				/* TRQ LIMIT�Z�b�g	*/
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;				/* TRQ LIMIT�Z�b�g	*/
		break;
		
	case TREFMODE_TLIM1:/*--- �g���N���~�b�g�L�� -------------------------------*/
	case TREFMODE_TLIM2:/*--- P_CL/N_CL�Ńg���N���~�b�g�L�� --------------------*/
		hdl->cmdData.Trq = 0;								/* TFF�Z�b�g		*/
		hdl->svCmd->ExtCmd1.dwVal = (cmd->TrqLimitP << 10);	/* TRQ LIMIT�Z�b�g	*/
		hdl->svCmd->ExtCmd2.dwVal = (cmd->TrqLimitN << 10);	/* TRQ LIMIT�Z�b�g	*/
		break;
		
	default: 			/*--- �g���N���~�b�g���� -------------------------------*/
		hdl->cmdData.Trq = 0;								/* TFF�Z�b�g		*/
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;				/* TRQ LIMIT�Z�b�g	*/
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;				/* TRQ LIMIT�Z�b�g	*/
		break;
	}
}


/****************************************************************************
 INTERPOLATE�w�ߐݒ�
****************************************************************************/
INT32 MciSetInterpolateCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - hdl->psnHdl.Ipos;

/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}
/* �w�ߑ��x�`�F�b�N */
	if((ULONG)MlibLABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
	{
		return MCI_SPD_ERR;
	}
/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g			*/
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFF�Z�b�g			*/
	mciSetTrqData(hdl, cmd,	hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIM�Z�b�g */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LT���[�h�N���A */
	}
	
	return MCI_SUCCESS;
}

/****************************************************************************
 LATCH�w�ߐݒ�
****************************************************************************/
INT32 MciSetLatchCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	LONG dpos = cmd->TPos - hdl->psnHdl.Ipos;

/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}

/* �w�ߑ��x�`�F�b�N */
	if((ULONG)MlibLABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref)
	{
		return MCI_SPD_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g	*/
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g			*/
	hdl->cmdData.Velocity = cmd->Velocity;		/* VFF�Z�b�g			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TrefModePos);/*TFF,TLIM�Z�b�g */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/
	
	MciRequestMtLatch(hdl, INTERPOL_LTMD, cmd->LatchSgnl);/* ���b�`�v�� */

	return MCI_SUCCESS;
}

/****************************************************************************
 POSING�w�ߐݒ�
****************************************************************************/
INT32 MciSetPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}

/* �w�ߑ��x�`�F�b�N */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_POSING;			/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g 	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g 			*/
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g 			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/* TFF,TLIM�Z�b�g */
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/
	
	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LT���[�h�N���A */
	}
	
	return MCI_SUCCESS;
}

/****************************************************************************
 EX_POSING�w�ߐݒ�
****************************************************************************/
INT32 MciSetExPosingCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}
	
/* �w�ߑ��x�`�F�b�N */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_EXPOSING;		/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g			*/
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g			*/	
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/
	
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ���b�`�M���Z�b�g		*/
	hdl->latch.Mode = EXPOSING_LTMD;			/* MOTION LT MODE�Z�b�g */

	return MCI_SUCCESS;	
}

/****************************************************************************
 HOMING�w�ߐݒ�
****************************************************************************/
INT32 MciSetHomingCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
	/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}
	
/* �w�ߑ��x�`�F�b�N */
	if((cmd->TSpeed < 0) || (cmd->TSpeed > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_HOMING;			/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/
	
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;	/* ���b�`�M���Z�b�g		*/
	hdl->latch.Mode = ZRET_LTMD;				/* MOTION LT MODE�Z�b�g */

	return MCI_SUCCESS;	
}

/****************************************************************************
 FEED�w�ߐݒ�
****************************************************************************/
INT32 MciSetFeedCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* RUN��ԃ`�F�b�N */
	if(hdl->mtState != MSTAT_RUN)
	{
		return MCI_CNDTN_ERR;
	}

/* �w�ߑ��x�`�F�b�N */
	if((ULONG)MlibLABS(cmd->TSpeed) > hdl->mtPrm.BasePrm.PosingSpdMax)
	{
		return MCI_SPD_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_FEED;			/* MTYPE�Z�b�g 			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.TSpeed = cmd->TSpeed;			/* TSPD�Z�b�g			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	mciSetAccRate(hdl, cmd);					/* �������x�Z�b�g<Rev2.01> */
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LT���[�h�N���A */
	}
	

	return MCI_SUCCESS;	
}

/****************************************************************************
 HOLD�w�ߐݒ�
****************************************************************************/
INT32 MciSetHoldCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* ���[�V������ԃ`�F�b�N */
	if(hdl->mtState == MSTAT_DISABLE)
	{
		return MCI_CNDTN_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_HOLD;			/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g	*/
	hdl->cmdData.HoldMod = cmd->HoldMod;		/* Hold���[�h�Z�b�g		*/
	
	hdl->cmdData.TSpeed = 0;					/* TSPD�Z�b�g 			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.TLimModePos);/*TFF,TLIM�Z�b�g*/
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LT���[�h�N���A */
	}
	

	return MCI_SUCCESS;	
}

/****************************************************************************
 VELCTRL�w�ߐݒ�
****************************************************************************/
INT32 MciSetVelCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* ���[�V������ԃ`�F�b�N */
	if(hdl->mtState == MSTAT_DISABLE)
	{
		return MCI_CNDTN_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;		/* ���x���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_VELCTRL;			/* MTYPE�Z�b�g 			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vref�Z�b�g			*/
	mciSetTrqData(hdl, cmd, hdl->mtPrm.BasePrm.trefMode);/*TFF,TLIM�Z�b�g*/
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g		*/

	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LT���[�h�N���A */
	}
	

	return MCI_SUCCESS;
}

/****************************************************************************
 TRQCTRL�w�ߐݒ�
****************************************************************************/
INT32 MciSetTrqCmd(MT_HNDL* hdl, MCI_CMD* cmd)
{
/* ���[�V������ԃ`�F�b�N */
	if(hdl->mtState == MSTAT_DISABLE)
	{
		return MCI_CNDTN_ERR;
	}

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.CtrlMode = MT_TRQ_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_TRQCTRL;			/* MTYPE�Z�b�g			*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g	*/
	
	hdl->cmdData.Velocity =	cmd->Velocity;		/* Vref�Z�b�g			*/
	hdl->cmdData.Trq = cmd->Trq;				/* Tref�Z�b�g			*/
	hdl->svCmd->ExtCmd1.dwVal = (cmd->TrqLimitP << 10);	/* TRQ LIMIT�Z�b�g	*/
	hdl->svCmd->ExtCmd2.dwVal = (cmd->TrqLimitN << 10);	/* TRQ LIMIT�Z�b�g	*/
	
	MciSetOptionCmd(hdl, cmd->Option);			/* Option�w�߃Z�b�g */
	
	if((hdl->latch.Mode != MODAL_LTMD)
	  && (hdl->latch.Mode != MOTION_MDL_LTMD))
	{
		 MtcCancelLatch(hdl, NONE_LTMD);		/* MOTION LT���[�h�N���A */
	}
	

	return MCI_SUCCESS;
}

/****************************************************************************
 OPTION�w�ߐݒ�
****************************************************************************/
VOID MciSetOptionCmd(MT_HNDL* hdl, ULONG option)
{
	ULONG 	bitVal, CmdSeq;
	USHORT	CmdCtrl;
	
/* �����l�擾	*/
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
	
/* �ݒ肵���l���i�[����	*/
	hdl->svCmd->CmdCtrl.CmdCtrlBit = CmdCtrl;
	hdl->svCmd->CmdSeqBit = CmdSeq;
}


/****************************************************************************
 ���[�V������ԃ��Z�b�g
****************************************************************************/
INT32 MciResetMotion(MT_HNDL* hdl)
{
	INT32 rc;
	
	/* ���[�J���ϐ��̏����� */
	rc = MCI_WAIT_COMP;

	if( (hdl->mtState == MSTAT_READY)
		|| (hdl->mtState == MSTAT_WAITPG_RDY)
		|| (hdl->mtState == MSTAT_DISABLE))
	{
		MciSetOptionCmd(hdl, 0);			/* Option�w�߃N���A */
		MtcCancelLatch(hdl, NONE_LTMD);		/* ���b�`�N���A */
		PsnResetApos(&hdl->psnHdl, FALSE);	/* Position���Z�b�g */
		hdl->cmdData.SensOn = FALSE;		/* SENS_OFF */
		hdl->cmdData.MtEenable = FALSE;		/* Motion���� */

		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		if( hdl->mtState == MSTAT_DISABLE
				 && ( 0 == (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) ) )
		{/* ����DISABLE��ԂȂ犮�� �� �Z���I�t���� */
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
 ���[�V������Ԑ؂�ւ�
****************************************************************************/
INT32 MciEnableState(MT_HNDL* hdl, BOOL enable)
{
	INT32 rc;

	/* ���[�J���ϐ��̏����� */
	rc = MCI_WAIT_COMP;

	if(enable)
	{/* ���[�V�����L���v��ON */
		hdl->cmdData.MtEenable = TRUE;
		if(hdl->mtState != MSTAT_DISABLE)
		{
			rc = MCI_SUCCESS;	/* DISABLE��ԂłȂ���Ί��� */
		}
	}
	else
	{/* ���[�V�����L���v��OFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE))
		{
			hdl->cmdData.MtEenable = FALSE;
			if(hdl->mtState == MSTAT_DISABLE)
			{
				rc = MCI_SUCCESS;	/* ����DISABLE��ԂȂ犮�� */
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
 �T�[�{�I���w�ߔ��s
****************************************************************************/
INT32 MciSetServoOn( MT_HNDL *hdl, BOOL enable, BOOL reset )
{
	INT32 rc;

	rc = MCI_WAIT_COMP;

	if( enable )
	{ /* ServoON */
		if( ((hdl->svRsp->RspSeqBit & MAIN_POWER_ON_BITMSK) == 0)	/* ���H�d��OFF */
			|| ((hdl->svRsp->RspSeqBit & ALM_DETECTED_BITMSK) != 0)			/* �T�[�{�A���[��������� */
			|| (hdl->mtState == MSTAT_DISABLE)
			|| (hdl->mtState == MSTAT_WAITPG_RDY) )
		{ /* ���H�f or ALM or Motion�������� */
			hdl->cmdData.ServoOn = FALSE;
			hdl->cmdData.SvonLock = TRUE;
			rc = MCI_CNDTN_ERR;
		}
//		else if( 0 != (hdl->svRsp->RspSigBit & ESTP_SIG_BITMSK) )
		else if( 0 != (hdl->svRsp->RspSigBit & HWBB_SIG_BITMSK) )/* <S156> */
		{ /* EMG��� */
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
				{ /* RUN��ԂȂ犮�� */
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
		{ /* �T�[�{OFF���� */
			rc = MCI_SUCCESS;
		}
	}

	return rc;
}

/****************************************************************************
 �Z���T�I���w�ߔ��s
****************************************************************************/
INT32 MciSetSensOn( MT_HNDL *hdl, BOOL enable )
{
	INT32 rc;

	rc = MCI_WAIT_COMP;

	if( enable )
	{ /* SensorON */
		if( (0 != (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK))/* ����SvON���(JOG����OK�Ƃ��邽��) */
			|| ((hdl->mtState != MSTAT_WAITPG_RDY) 				/* or SensOn������� */
				&& (hdl->mtState != MSTAT_DISABLE)) )
		{ /* Sensor On ���� */
			rc = MCI_SUCCESS;
		}
		else
		{ /* Sensor On����������� */
			if( 0 == (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) )
			{ /* SensOff��Ԃ��m�F��v���Z�b�g */
				hdl->cmdData.SensOn = TRUE;
			}
		}
	}
	else
	{ /* SensorOFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE) )
		{ /* �T�[�{OFF��Ԃ̂� */
			if( hdl->mtPrm.PsnPrm.absPG )
			{ /* ABS�̏ꍇ�̂݁ASensor Off */
				hdl->cmdData.SensOn = FALSE;
			}
			hdl->psnHdl.RefEnable = FALSE;
			rc = MCI_SUCCESS;
		}
		else
		{ /* SensorOFF�ł��Ȃ��̂ŃG���[ */
			rc = MCI_CNDTN_ERR;
		}
	}

	return rc;
}

/****************************************************************************
 ���b�`�v�����s(Motion�p)
****************************************************************************/
INT32 MciRequestMtLatch(MT_HNDL* hdl, LONG mode, LONG sgnl)
{
	if((hdl->latch.Mode != mode) 		/* ���b�`���[�h�s��v */
		|| !(hdl->latch.Cmp | hdl->latch.Req))	/* ���b�`�Ȃ� */
	{
		MtcCancelLatch(hdl, mode);
		MtcReqLatch(hdl, mode, sgnl);
	}
	return MCI_SUCCESS;
}

/****************************************************************************
 Modal���b�`�v�����s(mode=0:�ʏ탉�b�`�A=1:�A�����b�`)
****************************************************************************/
INT32 MciRequestMdlLatch(MT_HNDL* hdl, UCHAR mode, LONG sgnl)
{
	if(hdl->latch.Mode == NONE_LTMD)
	{/* ���b�`���[�h�Ȃ����̂ݗv�����s */
		if(mode == 1)
		{/* �V�[�P���V�������[�h */
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
	{/* ���݃��b�`���[�h��Modal���b�`���[�h�Ȃ�΃G���[ */
		if(hdl->latch.Mode != MODAL_LTMD)
		{
			return MCI_LTCNFLCT_ERR;
		}
	}
	return MCI_SUCCESS;
}

/****************************************************************************
 ���b�`�L�����Z��
****************************************************************************/
INT32 MciCancelLatch(MT_HNDL* hdl, LONG mode)
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
 �����[�gI/O�o��
****************************************************************************/
VOID MciOutputSignals(MT_HNDL* hdl, USHORT signl)
{
	hdl->svCmd->DOSignal = (UCHAR)signl;
}




/* Motion��Ԏ擾API */
/****************************************************************************
 LATCH�X�e�[�^�X�擾
****************************************************************************/
BOOL MciGetLatchStatus(MT_HNDL* hdl, LONG *ltEdge)
{
	BOOL lcmp;

	*ltEdge = (hdl->latch.Cmp >> 4);		/* bit4��� */
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
 ���j�^�f�[�^�擾
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
 I/O���j�^�f�[�^�擾
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
	
	/*---- PG Pulse���� ----*/
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
	
	/* SV_OPTION_IF���猻�ݒl���擾	*/
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
 �Z�[�t�e�B���W���[�����j�^�f�[�^�擾
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
