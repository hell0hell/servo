/****************************************************************************
Description	: MECHATROLINK Motion API
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
#include "MtApi.h"
#include "MLib.h"


#define MT_STATE_CHECK


/* Local functions */
__inline static	LONG mciGetCommonMonitorData( MT_HNDL *hdl, LONG cmon_sel );
__inline static	LONG mciGetOptionMonitorData( MT_HNDL *hdl, LONG cmon_sel, LONG omn_no );
__inline static	BOOL MciGetLtRange( PSN_HNDL *hdl, LONG i );
//__inline static	VOID mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR CtrlMode);/* <S004> *//* <S06D> */
static	LONG mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR CtrlMode);/* <S004> *//* <S06D> */
static	LONG mciChkACCR(LONG inRate, LONG maxData, LONG *outRate, LONG *altspd);/* <S06D> */
static INT32 mciMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData );/* <S06D> */
static INT32 mciChkTLIM(MT_HNDL* hdl, MCI_CMD* cmd, LONG* tlim);/* <S06D> */


/****************************************************************************/
/* �T�v		�F�w�߃f�[�^�̃N�����v�@�\�t���Q�C����Z						*/
/****************************************************************************/
/* ULONG inData			���͒l�i�w�߃f�[�^�j								*/
/* LONG  maxData		�ő�l												*/
/* LONG  kx				�m�[�}���C�Y�萔									*/
/* LONG* outData		�o�͒l�i�v�Z���ʁj									*/
/****************************************************************************/
/* ret = MCI_SUCCESS		����I��										*/
/* ret = DATA_SET_WARNG6	�f�[�^�ݒ�x��(�f�[�^�N�����v����)				*/
/****************************************************************************//* <S06D> */
static INT32 mciMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )
{
	INT32 ret;

	ret = MCI_SUCCESS;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF �̂Ƃ��͒ʐM�f�[�^���w�� */
		*outData = MlibMulgain32( (LONG)inData, kx );
	}
	else if( inData == 0xFFFFFFFF )
	{/* inData = 0xFFFFFFFF �̂Ƃ��͍ő�l���w�� */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF �̂Ƃ��� inData = 0x7FFFFFFF ���w�� */
		*outData = MlibMulgain32( 0x7FFFFFFF, kx );
		ret = MCI_DATA_CLAMP;
	}
	return ret;
}

/***************************************************************************
 �������P�ʕϊ�����(�ʐM�f�[�^�ł̉����x�w��)
	inRate		�����x�w��[�P�ʁFref/cycs^cyc]
	maxData		�ő�l
	*altspd		�����x�؂�ւ����x
	*outRate	�����x�w��[�P�ʁF***.*ref/cyc^cyc]
****************************************************************************//* <S06D> */
__inline static	LONG mciChkACCR(LONG inRate, LONG maxData, LONG *outRate, LONG *altspd)
{
	LONG ret = MCI_SUCCESS;

	if(inRate <= 0)
	{/* �����x <= 0�̏ꍇ�ُ͈�I�� */
		inRate = 1;
		ret = MCI_ACCR_ERR;
	}
	else if(inRate > maxData)
	{/* �����x > maxData�̏ꍇ�ُ͈�x�� */
		inRate = maxData;
		ret = MCI_ACCR_ERR;
	}
	*altspd = 0;	/* �����x�؂�ւ����x = 0 */
	*outRate = inRate;

	return ret;
}


/****************************************************************************
 �������w�߃f�[�^�ݒ�
****************************************************************************//* <S004> */
//__inline static	VOID mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR CtrlMode)/* <S06D> */
static	LONG mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR CtrlMode)/* <S06D> */
{
	LONG ret,ret1,ret2;
	LONG work, dummy;

	ret = MCI_SUCCESS;
	
	if(cmd->RateType == RATE_CMD)
	{/* �w�߃f�[�^�ɉ������w�߂�����ꍇ(M3�W���T�[�{�v���t�@�C��) */
		if(CtrlMode == MT_POS_CTRLMD)
		{/* �ʒu����̏ꍇ */
#if 0/* <S06D>:SGDV�̎d�l�ɍ��킹�� */
			/* ACCR�Z�b�g */
			hdl->mtPrm.CmdRate.accrate2 = MlibMulgain30U(cmd->AccRate,
											hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2);
			/* DECR�Z�b�g */
			hdl->mtPrm.CmdRate.decrate2 = MlibMulgain30U(cmd->DecRate,
											hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2);
#else
			/* �����x�`�F�b�N	*/
			mciMulgain32WithClampData(cmd->AccRate, MT_MAX_ACCR, hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2, &work);
			ret1 = mciChkACCR(work, MT_MAX_ACCR, &hdl->mtPrm.CmdRate.accrate2, &hdl->mtPrm.CmdRate.altspd_a);
			/* �����x�`�F�b�N	*/
			mciMulgain32WithClampData(cmd->DecRate, MT_MAX_ACCR, hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2, &work);
			ret2 = mciChkACCR(work, MT_MAX_ACCR, &hdl->mtPrm.CmdRate.decrate2, &hdl->mtPrm.CmdRate.altspd_d);
			/* �������x���ʌ���	*/
			ret = ret1 != MCI_SUCCESS ? ret1 : ret2;
#endif/* <S06D>:SGDV�̎d�l�ɍ��킹�� */
		}
		else if(CtrlMode == MT_VEL_CTRLMD)
		{/* ���x����̏ꍇ */
#if 0/* <S06D>:SGDV�̎d�l�ɍ��킹�� */
			/* ACCR�Z�b�g */
			hdl->mtPrm.CmdRate.accr_vel = MlibMulgain32U(cmd->AccRate, 1);
			/* DECR�Z�b�g */
			hdl->mtPrm.CmdRate.decr_vel = MlibMulgain32U(cmd->DecRate, 1);
#else
			/* �����x�`�F�b�N	*/
			mciMulgain32WithClampData(cmd->AccRate, MT_MAX_ACCR_VEL, 1, &work);
			ret1 = mciChkACCR(work, MT_MAX_ACCR_VEL, &hdl->mtPrm.CmdRate.accr_vel, &dummy);
			/* �����x�`�F�b�N	*/
			mciMulgain32WithClampData(cmd->DecRate, MT_MAX_ACCR_VEL, 1, &work);
			ret2 = mciChkACCR(work, MT_MAX_ACCR_VEL, &hdl->mtPrm.CmdRate.decr_vel, &dummy);
			ret = ret1 != MCI_SUCCESS ? ret1 : ret2;
#endif/* <S06D>:SGDV�̎d�l�ɍ��킹�� */
		}
		hdl->cmdData.CmdRateOn = TRUE;
	}
	else if((hdl->mtPrm.BasePrm.SetCmdAccRate != NULL) && (cmd->RateType == RATE_ACC_TIME))
	{/* �L���R�}���h && �������w�߃p�����[�^SW�L��(����������) */
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
	{/* �p�����[�^�ɂ��������ݒ� */
		hdl->cmdData.CmdRateOn = FALSE;
	}
	
	return ret;/* <S06D>:�`�F�b�N���ʂ�߂� */
}
/****************************************************************************
 TLIM�`�F�b�N
 TLIM���ݒ�͈͊O�̏ꍇ�́ATLIM�ɍő�l��ݒ肵�āA�G���[�R�[�h��Ԃ�
****************************************************************************//* <S06D> */
INT32 mciChkTLIM(MT_HNDL* hdl, MCI_CMD* cmd, LONG* tlim)
{
	INT32 ret;

	ret = MCI_SUCCESS;
	
	ret = mciMulgain32WithClampData(cmd->TrqLimitP, MT_MAXTRQ_30, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax, tlim);
	/* TLIM�`�F�b�N */
	if(hdl->mtPrm.BasePrm.TrefModePos == TREFMODE_TFF_TLIM)
	{/* TLIM�L�� */
		if(*tlim > MT_MAXTRQ_30)
		{
			*tlim = MT_MAXTRQ_30;
			ret = MCI_DATA_CLAMP;
		}
	}
	return ret;
}

/****************************************************************************
 INTERPOLATE�w�ߐݒ�
****************************************************************************/
INT32 MciSetInterpolateCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  dpos, tlim;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
	/* �w�ߑ��x�`�F�b�N */
	dpos = cmd->TPos - hdl->psnHdl.Ipos;
	if( (ULONG)MlibLABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref )
	{
		return MCI_SPD_ERR;
	}
#endif
	/* VFF�Z�b�g */	
#if (FLOAT_USE==TRUE)
	hdl->cmdData.Velocity = (LONG)( (float)cmd->Velocity * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
	/* TFF�Z�b�g */
	hdl->cmdData.Trq = (LONG)( (float)cmd->Trq * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/* TRQ LIMIT�Z�b�g */
//	tlim = (ULONG)( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else	
	hdl->cmdData.Velocity = MlibMulgain32( cmd->Velocity, 
								hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
	/* TFF�Z�b�g */
	hdl->cmdData.Trq = MlibMulgain32( cmd->Trq, 
								hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/* TRQ LIMIT�Z�b�g */
//	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */

	/*---------------------------------------------------------------------*/
/* VFF�`�F�b�N *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Velocity) > hdl->mtPrm.BasePrm.osVel)
	{
		hdl->cmdData.Velocity = MlibLimitul(hdl->cmdData.Velocity, 
							hdl->mtPrm.BasePrm.osVel, -(LONG)hdl->mtPrm.BasePrm.osVel);
		ret = MCI_DATA_CLAMP;
	}
/* TFF�`�F�b�N *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Trq) > MT_MAXTRQ_30)
	{
		hdl->cmdData.Trq = MlibLimitul(hdl->cmdData.Trq, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
	/*---------------------------------------------------------------------*/
	ret = mciChkTLIM( hdl, cmd, &tlim );/* <S06D> */
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;

	/*---------------------------------------------------------------------*/
	/* MOTION LT���[�h�N���A */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPE�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */

	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g */
	
//	return MCI_SUCCESS;
	return ret;/* <S11C> */
}



/****************************************************************************
 POSING�w�ߐݒ�
****************************************************************************/
INT32 MciSetPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG tlim;
	LONG tspd;
	
	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif
	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;


	/*---------------------------------------------------------------------*/
	/* MOTION LT���[�h�N���A */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_POSING;			/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */

	hdl->cmdData.TPos = cmd->TPos;				/* TPOS�Z�b�g */
	hdl->cmdData.Trq = 0;						/* TFF�Z�b�g */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}





/****************************************************************************
 SPOSING�w�ߐݒ�
****************************************************************************/
INT32 MciSetSposingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif
	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else
	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#endif /* FLOAT_USE */
	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	/*---------------------------------------------------------------------*/
	/* SRATIO�Z�b�g */
	hdl->mtPrm.CmdRate.Sratio = cmd->Sratio;

	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;
	
	/*---------------------------------------------------------------------*/
	/* MOTION LT���[�h�N���A */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype     = MTYPE_SPOSING;		/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode  = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g 		*/
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g 			*/

	hdl->cmdData.TPos      = cmd->TPos;			/* TPOS�Z�b�g  */
	hdl->cmdData.Trq	   = 0;					/* TFF�Z�b�g */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}




/****************************************************************************
 EX_POSING�w�ߐݒ�
****************************************************************************/
INT32 MciSetExPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */
	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;

	/*---------------------------------------------------------------------*/
	hdl->latch[MTLATCH_NUM].Mode = EXPOSING_LTMD;	/* MOTION LT MODE�Z�b�g */
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ���b�`�M���Z�b�g */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* ���[�V�������b�`�v���Z�b�g */

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_EXPOSING;			/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = cmd->RstReq;			/* �R�}���h�X�V�Z�b�g */

	hdl->cmdData.TPos = cmd->TPos;					/* TPOS�Z�b�g */
	hdl->cmdData.Trq  = 0;							/* TFF�Z�b�g */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 HOMING�w�ߐݒ�
****************************************************************************/
INT32 MciSetHomingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	/*------------------------------------
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;

	/*---------------------------------------------------------------------*/
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ���b�`�M���Z�b�g */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* ���[�V�������b�`�v���Z�b�g */
	hdl->latch[MTLATCH_NUM].Mode = ZRET_LTMD;		/* MOTION LT MODE�Z�b�g */

	MPrmSetZretDir(&hdl->mtPrm, cmd->HomeDir);		/* HOME DIR�Z�b�g */

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_HOMING;			/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* �R�}���h�X�V�Z�b�g */
	hdl->cmdData.Trq	   = 0;					/* TFF�Z�b�g */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 HOMING�w�ߐݒ�(DEC�M������)
****************************************************************************/
INT32 MciSetHomingCmdWithDec( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;

	/*---------------------------------------------------------------------*/
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ���b�`�M���Z�b�g */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* ���[�V�������b�`�v���Z�b�g */
	hdl->latch[MTLATCH_NUM].Mode = ZRET_LTMD;		/* MOTION LT MODE�Z�b�g */

	MPrmSetZretDir(&hdl->mtPrm, cmd->HomeDir);		/* HOME DIR�Z�b�g */

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_HOMING_WITH_DEC;		/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = cmd->RstReq;			/* �R�}���h�X�V�Z�b�g */

	hdl->cmdData.Trq	   = 0;						/* TFF�Z�b�g */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 FEED�w�ߐݒ�
****************************************************************************/
INT32 MciSetFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else
	tspd = MlibMulgain32( cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
//	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	if((ULONG)MlibLABS(tspd) > hdl->mtPrm.BasePrm.PosingSpdMax)/* <S07B> */
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;


	/*---------------------------------------------------------------------*/
	/* MOTION LT���[�h�N���A */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_FEED;				/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;					/* �R�}���h�X�V�Z�b�g */

	hdl->cmdData.Trq	   = 0;						/* TFF�Z�b�g */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 EX_FEED�w�ߐݒ�
****************************************************************************/
INT32 MciSetExFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN��ԃ`�F�b�N */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPD�Z�b�g *//* <S06D>:��U���[�J���ϐ��Ɋi�[���A�ް����������{����l�ɕύX */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32( cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* �w�ߑ��x�`�F�b�N *//* <S06D> */
//	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	if((ULONG)MlibLABS(tspd) > hdl->mtPrm.BasePrm.PosingSpdMax)/* <S07B> */
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}

	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;

	/*---------------------------------------------------------------------*/
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ���b�`�M���Z�b�g */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* ���[�V�������b�`�v���Z�b�g */
	hdl->latch[MTLATCH_NUM].Mode = EXFEED_LTMD;		/* MOTION LT MODE�Z�b�g */

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_EXFEED;				/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = cmd->RstReq;			/* �R�}���h�X�V�Z�b�g */

	hdl->cmdData.Trq	   = 0;						/* TFF�Z�b�g */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}

#if 0
/****************************************************************************
 HOLD�w�ߐݒ�
****************************************************************************/
INT32 MciSetHoldCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;

	ret = MCI_SUCCESS;

	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}

/* TLIM�`�F�b�N */
	ret = mciChkTLIM( hdl, cmd, ret );

/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_HOLD;							/* MTYPE�Z�b�g */
	hdl->cmdData.HoldMod = cmd->HoldMod;						/* Hold���[�h�Z�b�g */
	hdl->cmdData.TSpeed = 0;									/* TSPD�Z�b�g */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;						/* �ʒu���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;								/* �R�}���h�X�V�Z�b�g */
	hdl->cmdData.Trq	   = 0;									/* TFF�Z�b�g */
	hdl->svCmd->ExtCmd1.dwVal = cmd->TrqLimitP >> 6;			/* TRQ LIMIT�Z�b�g */
	hdl->svCmd->ExtCmd2.dwVal = cmd->TrqLimitN >> 6;			/* TRQ LIMIT�Z�b�g */

	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) && 
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );			/* MOTION LT���[�h�N���A */
	}
	return ret;	
}
#endif

#if 0
/****************************************************************************
 VELCTRL�w�ߐݒ�
****************************************************************************/
INT32 MciSetVelCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	LONG	vref;
	INT32	ret;

	vref  = cmd->Velocity;
	ret = MCI_SUCCESS;

	/* ���[�V������ԃ`�F�b�N */
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
	/* TFF�`�F�b�N */
	if( (ULONG)MlibLABS(cmd->Trq) > MT_MAXTRQ_30 )
	{
		if( cmd->Trq > 0x40000000 )
		{
			cmd->Trq = 0x40000000;
		}
		else if( cmd->Trq < 0xC0000000 )
		{
			cmd->Trq = 0xC0000000;
		}
		ret = MCI_DATA_CLAMP;
	}
	/* VREF�`�F�b�N */
	if( (ULONG)MlibLABS(vref) > MT_OSVEL_30 )
	{
		ret = MCI_DATA_CLAMP;
		/* M-2�݊��v���t�@�C���p�����Ȃ̂�VREF�l�̃N�����v�͍s��Ȃ� */
	}
	/* TLIM�`�F�b�N */
	ret = mciChkTLIM( hdl, cmd, ret );

	/* ���[�V�����w�߃f�[�^�Z�b�g */
	hdl->cmdData.Mtype = MTYPE_VELCTRL;						/* MTYPE�Z�b�g */
	hdl->cmdData.Velocity =	vref;							/* Vref�Z�b�g */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;					/* ���x���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;							/* �R�}���h�X�V�Z�b�g */
	hdl->cmdData.Trq	   = cmd->Trq;							/* TFF�Z�b�g */
	hdl->svCmd->ExtCmd1.dwVal = cmd->TrqLimitP >> 6;			/* TRQ LIMIT�Z�b�g */
	hdl->svCmd->ExtCmd2.dwVal = cmd->TrqLimitN >> 6;			/* TRQ LIMIT�Z�b�g */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) && 
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );		/* MOTION LT���[�h�N���A */
	}

	return ret;
}
#endif

/****************************************************************************
 VELCTRL�w�ߐݒ�i�������x�w�߂���j
****************************************************************************/
INT32 MciSetVelCmdWithAccr( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* ���[�V������ԃ`�F�b�N */
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/* TFF�Z�b�g */
	hdl->cmdData.Trq  = ( (float)cmd->Trq * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/* TSPD�Z�b�g */
	hdl->cmdData.Velocity = ( (float)cmd->Velocity * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#else
	/* TFF�Z�b�g */
	hdl->cmdData.Trq  = MlibMulgain32( cmd->Trq, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
	
	/* TSPD�Z�b�g */
	hdl->cmdData.Velocity = MlibMulgain32( cmd->Velocity, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#endif /* FLOAT_USE */
/* TFF�`�F�b�N *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Trq) > MT_MAXTRQ_30)
	{
		hdl->cmdData.Trq = MlibLimitul(hdl->cmdData.Trq, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
/* VREF�`�F�b�N *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Velocity) > MT_OSVEL_30)
	{
		hdl->cmdData.Velocity = MlibLimitul(hdl->cmdData.Velocity, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
	/*---------------------------------------------------------------------*/
	/* ACCR,DECR�Z�b�g *//* <S004> *//* <S06D>:�������x�̃f�[�^�`�F�b�N�ǉ� */
	ret = mciSetAccRate(hdl, cmd, MT_VEL_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMIT�Z�b�g */
#if 0/* <S06D> */
#if (FLOAT_USE==TRUE)
	tlim =  ( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else
	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */
#endif
	ret = mciChkTLIM( hdl, cmd, &tlim );
	hdl->svCmd->ExtCmd1.dwVal = tlim >> 6;
	hdl->svCmd->ExtCmd2.dwVal = tlim >> 6;
	/*---------------------------------------------------------------------*/
	/* MOTION LT���[�h�N���A */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/* FB���x�Z�b�g(����ؑ֎��w�ߗp) */
	hdl->outData.FbSpeed = hdl->svRsp->Mon3;

	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_VELCTRL_WITH_ACCR;	/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;			/* ���x���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;					/* �R�}���h�X�V�Z�b�g */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 TRQCTRL�w�ߐݒ�
****************************************************************************/
INT32 MciSetTrqCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* ���[�V������ԃ`�F�b�N */
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
#endif

#if (FLOAT_USE==TRUE)
	/*---------------------------------------------------------------------*/
	/* TREF�Z�b�g */
	hdl->cmdData.Trq  = (LONG)( (float)cmd->Trq * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/*---------------------------------------------------------------------*/
	/* VLIM�Z�b�g */
	hdl->cmdData.Velocity = (ULONG)( (float)cmd->Velocity *
								hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#else
	/*---------------------------------------------------------------------*/
	/* TREF�Z�b�g */
	hdl->cmdData.Trq  = MlibMulgain32( cmd->Trq, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/*---------------------------------------------------------------------*/
	/* VLIM�Z�b�g */
	hdl->cmdData.Velocity = MlibMulgain32U( (ULONG)cmd->Velocity, 
								hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#endif /* FLOAT_USE */
/* VLIM�`�F�b�N *//* <S06D> */
	if(hdl->mtPrm.BasePrm.vLimModeTrq == VLIM_MODE_ENABLE)
	{
		if(hdl->cmdData.Velocity > MT_OSVEL_30)
		{
			hdl->cmdData.Velocity = MT_OSVEL_30;
			ret = MCI_DATA_CLAMP;
		}
	}
/* TREF�`�F�b�N *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Trq) > MT_MAXTRQ_30)
	{
		hdl->cmdData.Trq = MlibLimitul(hdl->cmdData.Trq, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
	
	/*---------------------------------------------------------------------*/
	/* MOTION LT���[�h�N���A */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}
	
	/*---------------------------------------------------------------------*/
	/* ���[�V�����w�߃f�[�^�Z�b�g *//* <S06D>:�����̈ʒu��ύX */
	hdl->cmdData.Mtype = MTYPE_TRQCTRL;			/* MTYPE�Z�b�g */
	hdl->cmdData.CtrlMode = MT_TRQ_CTRLMD;		/* �g���N���䃂�[�h�Z�b�g */
	hdl->cmdData.CmdRstReq = TRUE;				/* �R�}���h�X�V�Z�b�g */
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;		/* TRQ LIMIT�Z�b�g */
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;		/* TRQ LIMIT�Z�b�g */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 CMD_PAUSE �w�ߐݒ�
****************************************************************************/
// �����ł́AMtype�ɂ��`�F�b�N�͂��Ȃ��B(�{�֐��̌Ăяo�����Ń`�F�b�N���s��)
// CMD_PAUSE �w�߂̂Ƃ��́A�������x���ُ�ł����s����
INT32 MciSetCmdPause( MT_HNDL *hdl, MCI_CMD *cmd )
{
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}

	if( hdl->cmdData.CtrlMode == MT_POS_CTRLMD )
	{ /* �ʒu���� */
		/* �����x�AMTYPE�ATLIM �͍��܂ł̐ݒ�𗘗p���� */
		if( cmd->HoldMod == HOLD_MODE_RAPID_STOP )
		{ /* �}����~�̏ꍇ�́A�ő匸���x�Œ�~ */
			hdl->mtPrm.CmdRate.altspd_d = 0;
			hdl->mtPrm.CmdRate.decrate2 = MT_MAX_ACCR;
			hdl->cmdData.CmdRateOn = TRUE;			/* �������w�ߗL�� */
		}
		hdl->cmdData.TSpeed = 0;					/* TSPD�Z�b�g */
	}
	else if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
	{ /* ���x���� */
		/* �����x�AMTYPE�ATLIM �͍��܂ł̐ݒ�𗘗p���� */
		if( cmd->HoldMod == HOLD_MODE_RAPID_STOP )
		{ /* �}����~�̏ꍇ�́A�ő匸���x�Œ�~ @CHECK@ �ő�����x��OK���H */
			hdl->cmdData.Mtype = MTYPE_VELCTRL;		/* MTYPE�Z�b�g */
		}
		hdl->cmdData.Velocity =	0;					/* Vref�Z�b�g */
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 CMD_CANCEL �w�ߐݒ�
****************************************************************************/
// CMD_CANCEL �w�߂̂Ƃ��́A�������x���ُ�ł����s����
INT32 MciSetCmdCancel( MT_HNDL *hdl, MCI_CMD *cmd )
{
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
	
	if( hdl->cmdData.CtrlMode == MT_POS_CTRLMD )
	{ /* �ʒu���� */
		/* �����x�ATLIM �͍��܂ł̐ݒ�𗘗p���� */
		if(hdl->cmdData.Mtype == MTYPE_SPOSING)				/* MTYPE�Z�b�g */
		{
			hdl->cmdData.Mtype = MTYPE_SHOLD;
		}
		else
		{
			hdl->cmdData.Mtype = MTYPE_HOLD;
		}
		hdl->cmdData.HoldMod = cmd->HoldMod;				/* Hold���[�h�Z�b�g */
		hdl->cmdData.TSpeed = 0;							/* TSPD�Z�b�g */
		hdl->cmdData.CmdRstReq = TRUE;						/* �R�}���h�X�V�Z�b�g */
		if( cmd->HoldMod > HOLD_MODE_RAPID_STOP )
		{ /* STOP���[�h��Reserve�l���ݒ肳�ꂽ�ꍇ�͌�����~ */
			hdl->cmdData.HoldMod = HOLD_MODE_RSLOWDOWN_STOP;
		}
	}
	else if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
	{ /* ���x���� */
		/* �����x�AMTYPE�ATLIM �͍��܂ł̐ݒ�𗘗p���� */
		if( cmd->HoldMod == HOLD_MODE_RAPID_STOP )
		{ /* �}����~�̏ꍇ�́A�ő匸���x�Œ�~ �ő匸���x�Œ�~ @CHECK@ �ő�����x��OK���H*/
			hdl->cmdData.Mtype = MTYPE_VELCTRL;				/* MTYPE�Z�b�g */
		}
		hdl->cmdData.Velocity =	0;							/* Vref�Z�b�g */
		hdl->cmdData.CmdRstReq = TRUE;						/* �R�}���h�X�V�Z�b�g */
	}
	return MCI_SUCCESS;	
}


/****************************************************************************
 �ړ��R�}���h�ꎞ��~�w�ߎ��̃A�v���[�`���x�[���N���A�ݒ�
****************************************************************************/
// �{�֐���(�O��cmdPause == FALSE) && (����cmdPause == TRUE) �̂Ƃ��Ɏ��s����
VOID MciClearZretSpd( MT_HNDL *hdl )
{
	if( (hdl->cmdData.Mtype == MTYPE_HOMING) || (hdl->cmdData.Mtype == MTYPE_HOMING_WITH_DEC) )
	{ /* ���_���A���̂ݎ��s */
		/* �A�v���[�`���x���[���ɐݒ� */
		hdl->mtPrm.MtSeqPrm.zretsp1 = 0;
		hdl->mtPrm.MtSeqPrm.zretsp2 = 0;
	}
}


/****************************************************************************
 �ړ��R�}���h�ꎞ��~�w�ߎ��̃A�v���[�`���x���A�ݒ�
****************************************************************************/
// �{�֐���(�O��cmdPause == TRUE) && (����cmdPause == FALSE) �̂Ƃ��Ɏ��s����
// �{�֐��̓R�}���h�Ɋ֌W�Ȃ����{����
VOID MciReturnZretSpd( MT_HNDL *hdl )
{
	/* �o�b�t�@�̃A�v���[�`���x�𕜋A */
	hdl->mtPrm.MtSeqPrm.zretsp1 = hdl->mtPrm.MtSeqPrm.zretsp1_org;
	hdl->mtPrm.MtSeqPrm.zretsp2 = hdl->mtPrm.MtSeqPrm.zretsp2_org;
}
#if 0 /* M2�v���t�@�C���Ή� */
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

/* G_SEL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.gselBit) & hdl->mtPrm.OptMap.gselBitMsk);
	pMtHdl->svCmd->CmdCtrl.SelBank = (UCHAR)bitVal;

/* P_PI */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.vppiBit) & hdl->mtPrm.OptMap.vppiBitMsk);
	CmdCtrl |=  (USHORT)(bitVal << ENBPCTRL_BITNO);

/* P_I_CLR */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.ppiclrBit) & hdl->mtPrm.OptMap.ppiclrBitMsk);
	CmdCtrl |=  (USHORT)(bitVal << CLRPOSINTG_BITNO);

/* N_CL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.nclBit) & hdl->mtPrm.OptMap.nclBitMsk);
	CmdSeq |= bitVal << ENB_EXTNTRQLMT_BITNO;

/* P_CL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.pclBit) & hdl->mtPrm.OptMap.pclBitMsk);
	CmdSeq |= bitVal << ENB_EXTPTRQLMT_BITNO;

/* OUT_SIGNAL */
	bitVal = (ULONG)((option >> hdl->mtPrm.OptMap.outpBit) & hdl->mtPrm.OptMap.outpBitMsk);
	pMtHdl->svCmd->DOSignal = bitVal;

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
	pMtHdl->svCmd->CmdCtrl.CmdCtrlBit = CmdCtrl;
	pMtHdl->svCmd->CmdSeqBit = CmdSeq;
}
#endif
/****************************************************************************
 ���[�V������ԃ��Z�b�g
****************************************************************************/
INT32 MciResetMotion( MT_HNDL *hdl )
{
	INT32 rc;

	/* ���[�J���ϐ��̏����� */
	rc = MCI_WAIT_COMP;

	if( (hdl->mtState == MSTAT_READY)
		|| (hdl->mtState == MSTAT_WAITPG_RDY)
		|| (hdl->mtState == MSTAT_DISABLE) )
	{
//		MciSetOptionCmd( hdl, 0 );							/* Option�w�߃N���A */
		MtcCancelLatch( hdl, NONE_LTMD, 0 );				/* ���b�`�P�N���A */
		MtcCancelLatch( hdl, NONE_LTMD, 1 );				/* ���b�`�Q�N���A */
		PsnResetApos( &hdl->psnHdl, FALSE );				/* Position���Z�b�g */
		hdl->svCmd->CmdCtrl.CmdCtrlBit &= ~(CLRPOSINTG_BIT);/* P_PPI�w�߃N���A */
		hdl->cmdData.SensOn = FALSE;						/* SENS_OFF */
		hdl->cmdData.MtEenable = FALSE;						/* Motion���� */

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
INT32 MciEnableState( MT_HNDL *hdl, BOOL enable )
{
	INT32 rc;

	/* ���[�J���ϐ��̏����� */
	rc = MCI_WAIT_COMP;

	if( enable )
	{ /* ���[�V�����L���v��ON */
		hdl->cmdData.MtEenable = TRUE;
		if( hdl->mtState != MSTAT_DISABLE )
		{
			rc = MCI_SUCCESS;		/* DISABLE��ԂłȂ���Ί��� */
		}
	}
	else
	{ /* ���[�V�����L���v��OFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE) )
		{
			hdl->cmdData.MtEenable = FALSE;
			if( hdl->mtState == MSTAT_DISABLE )
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

 �y�⑫�zM-RDY���̎g�p�ɂ���
  �A�u�\���j�A�̎��Ɍ��o�����ɂ����āA���ɏ����X�P�[���ɏ������ގ��ɁA
  M-RDY=FALSE�ƂȂ�AA.95A�����������肪���������߁AM-RDY�͎g�p���Ȃ�

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
		else if( 0 != (hdl->svRsp->RspSigBit & HWBB_SIG_BITMSK) )/* <S07C> */
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
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
INT32 MciRequestMtLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, UCHAR i )
{
	if( (hdl->latch[i].Mode != mode) 					/* ���b�`���[�h�s��v */
		|| !(hdl->latch[i].Cmp | hdl->latch[i].Req) )	/* ���b�`�Ȃ� */
	{
		MtcCancelLatch( hdl, mode, i );
		MtcReqLatch( hdl, mode, sgnl, i );
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 Modal���b�`�v�����s
 mode=0:�ʏ탉�b�`�A=1:�A�����b�`
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
INT32 MciRequestMdlLatch( MT_HNDL *hdl, UCHAR mode, LONG sgnl, LONG i )
{
	if( hdl->latch[i].Mode == NONE_LTMD )
	{ /* ���b�`���[�h�Ȃ����̂ݗv�����s */
		if( mode == 1 )
		{ /* �V�[�P���V�������[�h */
			if( hdl->mtPrm.LtPrm.seqNum == 0 )
			{
				return MCI_LTMOD_ERR;
			}

			hdl->latch[i].SeqReq = MTLT_SEQ_REQ;
			sgnl = hdl->mtPrm.LtPrm.ltSgnl[0];
		}
		MtcReqLatch( hdl, MODAL_LTMD, sgnl, i );
	}
	else
	{ /* ���݃��b�`���[�h��Modal���b�`���[�h�Ȃ�΃G���[ */
		if( hdl->latch[i].Mode != MODAL_LTMD )
		{
			return MCI_LTCNFLCT_ERR;
		}
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 ���b�`�L�����Z��
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
INT32 MciCancelLatch( MT_HNDL *hdl, LONG mode, LONG i )
{
	if( mode == MODAL_LTMD )
	{ /* Modal���b�`�L�����Z�� */
		if( (hdl->latch[i].Mode != NONE_LTMD)
			&& (hdl->latch[i].Mode != MODAL_LTMD) )
		{ /* ���b�`���[�h���� && MODAL_LTMD�ȊO�́A�G���[ */
			return MCI_LTCNFLCT_ERR;
		}
		MtcCancelLatch( hdl, NONE_LTMD, i );
	}
	else if( mode == MOTION_MDL_LTMD )
	{ /* Motion Modal�L�����Z�� */
		MtcCancelLatch( hdl, MOTION_MDL_LTMD, i );
	}
	else
	{ /* �ʏ� Motion�L�����Z�� */
		if( (hdl->latch[i].Mode == MODAL_LTMD)
			|| ((hdl->latch[i].Mode == ZRET_LTMD) && (hdl->seqState != 0) 
			    && (hdl->mtPrm.ProfPrm.zretLtCancel == ZRET_LATCH_IS_NOCANCEL)) )
		{ /* ZRET���쒆 or ���[�_�����b�`���́A�������Ȃ� */
			return MCI_SUCCESS;
		}
		MtcCancelLatch( hdl, NONE_LTMD, i );
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 ���W�n�ݒ�
****************************************************************************/
INT32 MciSetCordinatePos( MT_HNDL *hdl, LONG data, BOOL ref, BOOL ref_clr )
{
	PsnSetCordinate( &hdl->psnHdl, 0, data );
	if( (ref == TRUE) || (ref_clr == TRUE) )
	{
		hdl->psnHdl.RefEnable = ref;
	}
	hdl->cmdData.TPos = hdl->psnHdl.Tpos;

	return MCI_SUCCESS;
}


/* Motion��Ԏ擾API */
/****************************************************************************
 LATCH�X�e�[�^�X�擾
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
BOOL MciGetLatchStatus( MT_HNDL *hdl, LONG *ltEdge, LONG i )
{
	BOOL lcmp;

	*ltEdge = ( hdl->latch[i].Cmp >> 4 );			/* bit4��� */
	hdl->latch[i].Cmp = hdl->latch[i].Cmp & 0x1;	/* bit4:OFF */
	if( hdl->latch[i].SeqReq != 0 )
	{
		lcmp = *ltEdge;
	}
	else
	{
		lcmp = hdl->latch[i].Cmp;
	}
	return lcmp;
}


/****************************************************************************
 ���ʃ��j�^�f�[�^�擾
 SV_STAT�̃f�[�^�́ARspMMnger�ɂč쐬����
****************************************************************************/
/****************************************************************************
 ���ʃ��j�^�f�[�^�擾
SV_STAT�̃f�[�^�����́A�����̃f�[�^�����L�̂悤�ɕ��ނ���Ă���
 1Byte�ځF���݂̒ʐM�t�F�[�Y
 2Byte�ځF���݂̐��䃂�[�h 0:�ʒu, 1:���x, 2:�g���N
 3Byte�ځF���쒆�̃R�}���h�R�[�h
 4byte�ځF�g���M�����j�^(0bit��)
 4byte�ځF�g���M�����j�^(1bit��)
 4byte�ځF�g���M�����j�^(2,3bit��)
 4byte�ځF�g���M�����j�^(4,5bit��)
 4byte�ځF�g���M�����j�^(6,7bit�� Reserve)
****************************************************************************/
static	LONG mciGetCommonMonitorData( MT_HNDL *hdl, LONG cmon_sel )
{
	LONG data;

	data = 0;

	switch(cmon_sel)
	{
	case TPOS_MON:
		data = hdl->psnHdl.Tpos;
		break;
	case IPOS_MON:
		data = hdl->psnHdl.Ipos;
		break;
	case POS_OFST_MON:
		data = hdl->psnHdl.PosOfs;
		break;

#if (FLOAT_USE==TRUE)
	case TSPD_MON:
		if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
		{
			data = (LONG)( (float)hdl->outData.Vref * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdMaxToUnit );
		}
		else
		{
			data = (LONG)( (float)hdl->outData.Tspd * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdRefToUnit );
		}
		break;
	case SPD_LIM_MON:
		if(hdl->cmdData.CtrlMode == MT_TRQ_CTRLMD)
		{
			data = (LONG)( (float)hdl->outData.Vref * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdMaxToUnit );
		}
		break;
	case TRQ_LIM_MON:
		if(hdl->cmdData.CtrlMode != MT_TRQ_CTRLMD)
		{
			data = hdl->svCmd->ExtCmd1.dwVal << 6;	/* [24bit/MAX] �� [30bit/MAX] */
			data = (LONG)( (float)data * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqMaxToUnit );
		}
		break;
#else	
	case TSPD_MON:
		if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
		{
			data = MlibMulgain32Sx48( hdl->outData.Vref, hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdMaxToUnit );
		}
		else
		{
			data = MlibMulgain32Sx48( hdl->outData.Tspd, hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdRefToUnit );
		}
		break;
	case SPD_LIM_MON:
		if(hdl->cmdData.CtrlMode == MT_TRQ_CTRLMD)
		{
			data = MlibMulgain32Sx48( hdl->outData.Vref, hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdMaxToUnit );
		}
		break;
	case TRQ_LIM_MON:
		if(hdl->cmdData.CtrlMode != MT_TRQ_CTRLMD)
		{
			data = hdl->svCmd->ExtCmd1.dwVal << 6;	/* [24bit/MAX] �� [30bit/MAX] */
			data = MlibMulgain32Sx48( data, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqMaxToUnit );
		}
		break;
#endif /* FLOAT_USE */

	case INIT_PGPOS_L_MON:
		data = hdl->psnHdl.InitPgPos64[0];
		break;
	case INIT_PGPOS_H_MON:
		data = hdl->psnHdl.InitPgPos64[1];
		break;
	default:
		data = 0;
		break;
	}
	return data;
}


/****************************************************************************
 �I�v�V�������j�^�f�[�^�擾
 cmon_sel  �I�v�V�������j�^�̃f�[�^�I��ԍ�
 omon_no   �I�v�V�������j�^�̔ԍ� [1/2]
****************************************************************************/
static	LONG mciGetOptionMonitorData( MT_HNDL *hdl, LONG cmon_sel, LONG omn_no )
{
	LONG data;

	switch( cmon_sel )
	{
	case LST_LPOS1_MON: //���b�`�ʒuLPOS1�O��l
		data = hdl->psnHdl.LstLpos[0];
		break;
	case LST_LPOS2_MON: //���b�`�ʒuLPOS2�O��l
		data = hdl->psnHdl.LstLpos[1];
		break;
	case SEQ_LATCH_STS: //�A�����b�`�X�e�[�^�X
		data = hdl->latch[1].SeqSts.wdCnt;
		break;
/* <S1D7>:�b��ǉ� */
	case HIGH_ORDER32_POS:		/* 85:�w�ߍ��W�n�̎w�߈ʒu(�t�B���^��)		���32Bit�f�[�^	*/
	case HIGH_ORDER32_MPOS:		/* 86:�w�߈ʒu								���32Bit�f�[�^	*/
	case HIGH_ORDER32_PERR:		/* 87:�ʒu�΍�								���32Bit�f�[�^	*/
	case HIGH_ORDER32_APOS:		/* 88:�@�B���W�n�t�B�[�h�o�b�N�ʒu			���32Bit�f�[�^	*/
	case HIGH_ORDER32_LPOS1:	/* 89:�@�B���W�n�t�B�[�h�o�b�N���b�`�ʒu1	���32Bit�f�[�^	*/
	case HIGH_ORDER32_LPOS2:	/* 8A:�@�B���W�n�t�B�[�h�o�b�N���b�`�ʒu2	���32Bit�f�[�^	*/
	case HIGH_ORDER32_IPOS:		/* 8B:�w�ߍ��W�n�w�߈ʒu(�t�B���^�O)		���32Bit�f�[�^	*/
	case HIGH_ORDER32_TPOS:		/* 8C:�w�ߍ��W�n�ڕW�ʒu					���32Bit�f�[�^	*/
		data = 0;
		break;
/* <S1D7>:�b��ǉ� */
	default:
		if( omn_no == 1 )
		{
			data = hdl->svRsp->Mon5;
		}
		else
		{
			data = hdl->svRsp->Mon6;
		}
		break;
	}
	return data;
}


/****************************************************************************
 ���j�^�f�[�^�擾�iM3�W���T�[�{�v���t�@�C���j
****************************************************************************/
LONG MciGetM3StdMonitorData( MT_HNDL *hdl, LONG sel )
{
	LONG data;

	switch( sel )
	{
	case M3STD_APOS_MON:
		data = hdl->psnHdl.Apos;
		break;
	case M3STD_CPOS_MON:
		data = hdl->psnHdl.Pos;
		break;
	case M3STD_PERR_MON:
		data = hdl->psnHdl.Perr;
//		data = hdl->svRsp->Mon7;
		break;
	case M3STD_LPOS1_MON:
		data = hdl->psnHdl.Lpos[0];
		break;
	case M3STD_LPOS2_MON:
		data = hdl->psnHdl.Lpos[1];
		break;

#if (FLOAT_USE==TRUE)
	case M3STD_FSPD_MON:
		data = (LONG)( (float)hdl->svRsp->Mon3 * hdl->mtPrm.BasePrm.m3ConvUnit.m3Spd24OsToUnit );
		break;
	case M3STD_CSPD_MON:
		if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
		{
			data = (LONG)( hdl->outData.VrefOut * hdl->mtPrm.BasePrm.m3ConvUnit.m3Spd24OsToUnit );
		}
		else
		{
			data = (LONG)( (float)hdl->outData.DposOut * hdl->mtPrm.BasePrm.fspdRefKx );
			data = (LONG)( (float)data * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdRefToUnit );
		}
		break;
	case M3STD_TRQ_MON:
		data = hdl->svRsp->Mon2;
		data = data << 6;					/* [24bit/MAX] �� [30bit/MAX] */
		data = (LONG)( (float)data * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqMaxToUnit );
		break;
#else
	case M3STD_FSPD_MON:
		data = MlibMulgain32Sx48( hdl->svRsp->Mon3, hdl->mtPrm.BasePrm.m3ConvUnit.m3Spd24OsToUnit );
		break;
	case M3STD_CSPD_MON:
		if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
		{
			data = MlibMulgain32Sx48( hdl->outData.VrefOut, hdl->mtPrm.BasePrm.m3ConvUnit.m3Spd24OsToUnit );
		}
		else
		{
			data = MlibMulgain32( hdl->outData.DposOut, hdl->mtPrm.BasePrm.fspdRefKx );
			data = MlibMulgain32Sx48( data, hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdRefToUnit );
		}
		break;
	case M3STD_TRQ_MON:
		data = hdl->svRsp->Mon2;
		data = data << 6;					/* [24bit/MAX] �� [30bit/MAX] */
		data = MlibMulgain32Sx48( data, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqMaxToUnit );
		break;
#endif /* FLOAT_USE */

	case M3STD_MPOS_MON:
		data = hdl->psnHdl.Mpos;
		break;
	case M3STD_CMN1_MON:					/* ���ʃ��j�^ */
		data = mciGetCommonMonitorData( hdl, hdl->mtPrm.BasePrm.m3_selMon1 );
		break;
	case M3STD_CMN2_MON:					/* ���ʃ��j�^ */
		data = mciGetCommonMonitorData( hdl, hdl->mtPrm.BasePrm.m3_selMon2 );
		break;

	case M3STD_OMN1_MON:
		if( hdl->svRsp->SelMon5 == SELMON_FBPOS64L )
		{ /* �����G���R�[�_�ʒu�擾�������́A�O���OMN1�̒l��Ԃ� */
			data = hdl->lstOmnMon[0];
		}
		else
		{
			data = mciGetOptionMonitorData( hdl, hdl->svCmd->SelMon5, 1 );
			hdl->lstOmnMon[0] = data;
		}
		break;

	case M3STD_OMN2_MON:
		if( hdl->svRsp->SelMon6 == SELMON_FBPOS64H )
		{ /* �����G���R�[�_�ʒu�擾�������́A�O���OMN2�̒l��Ԃ� */
			data = hdl->lstOmnMon[1];
		}
		else
		{
			data = mciGetOptionMonitorData( hdl, hdl->svCmd->SelMon6, 2 );
			hdl->lstOmnMon[1] = data;
		}

		break;
	default:
		data = 0;
		break;
	}
	return data;
}

#if 0

/****************************************************************************
 ���j�^�f�[�^�擾�iM2�݊��v���t�@�C���j
****************************************************************************/
LONG MciGetM2MonitorData( MT_HNDL *hdl, USHORT sel, UCHAR unit )
{
	LONG data;
	
	switch( sel )
	{
	case M2_POS_MON:
		data = hdl->psnHdl.Pos;
		break;
	case M2_MPOS_MON:
		data = hdl->psnHdl.Mpos;
		break;
	case M2_PERR_MON:
		data = hdl->psnHdl.Perr;
		break;
	case M2_APOS_MON:
		data = hdl->psnHdl.Apos;
		break;
	case M2_LPOS_MON:
		data = hdl->psnHdl.Lpos[0];
		break;
	case M2_IPOS_MON:
		data = hdl->psnHdl.Ipos;
		break;
	case M2_TPOS_MON:
		data = hdl->psnHdl.Tpos;
		break;

#if (FLOAT_USE==TRUE)
	case M2_FSPD_MON:
		if( unit == 2 )
		{	/* [30bit/MAX] */
			data = (LONG)( (float)hdl->svRsp->Mon3 * hdl->mtPrm.BasePrm.osToMaxKx );
		}
		else
		{	/* [ref/s] */
			data = (LONG)( (float)hdl->svRsp->Mon3 * hdl->mtPrm.BasePrm.osToRefKx );
		}
		break;
	case M2_CSPD_MON:
		if( unit == 2 )
		{	/* [30bit/MAX] */
			data = hdl->outData.Vref;
		}
		else
		{	/* [ref/s] */
			data = (LONG)( (float)hdl->outData.DposOut * hdl->mtPrm.BasePrm.fspdRefKx );
		}
		break;
#elif( unit == 2 )		/* <S182>�R�[�h���� */
		{	/* [30bit/MAX] */
			data = MlibMulgain32( hdl->svRsp->Mon3, hdl->mtPrm.BasePrm.osToMaxKx );
		}
		else
		{	/* [ref/s] */
			data = MlibMulgain32( hdl->svRsp->Mon3, hdl->mtPrm.BasePrm.osToRefKx );
		}
		break;
	case M2_CSPD_MON:
		if( unit == 2 )
		{	/* [30bit/MAX] */
			data = hdl->outData.Vref;
		}
		else
		{	/* [ref/s] */
			data = MlibMulgain32( hdl->outData.DposOut, hdl->mtPrm.BasePrm.fspdRefKx );
		}
		break;
#endif /* FLOAT_USE */


	case M2_TSPD_MON:
		if( unit == 2 )
		{	/* [30bit/MAX] */
			data = hdl->outData.Vref;
		}
		else
		{	/* [ref/s] */
			data = hdl->outData.Tspd;
		}
		break;
	case M2_TRQ_MON:
		data = hdl->svRsp->Mon2;	/* [24bit/MAX] */
		if( unit == 3 )
		{	/* [30bit/MAX] */
			data = data << 6;
		}
		else
		{	/*[%] */

#if (FLOAT_USE==TRUE)
			data = MlibMulgain32( data, hdl->mtPrm.BasePrm.trqKx );
#else
			data = MlibMulgain32( data, hdl->mtPrm.BasePrm.trqKx );
#endif /* FLOAT_USE */


		}
		break;
	case M2_OMN1_MON:
		if( hdl->svCmd->SelMon5 == 0x80 )
		{
			data = hdl->psnHdl.LstLpos[0];
		}
		else
		{
			data = hdl->svRsp->Mon5;
		}
		break;
	case M2_OMN2_MON:
		if( hdl->svCmd->SelMon6 == 0x80 )
		{
			data = hdl->psnHdl.LstLpos[0];
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
	���j�^�f�[�^(�|�[�g����)�擾�iM2�݊��v���t�@�C���j
****************************************************************************/
USHORT MciGetInPortSignals( MT_HNDL *hdl )
{
	USHORT	sgnl;
	USHORT	inSig;

	inSig = (USHORT)~( hdl->svRsp->Mon7 );

	sgnl  = ( (inSig >> hdl->mtPrm.InSigMap.bitNo[0]) & 0x01 );			/* IO:0 */
	sgnl |= ( ((inSig >> hdl->mtPrm.InSigMap.bitNo[1]) & 0x01) << 1 );	/* IO:1 */
	sgnl |= ( ((inSig >> hdl->mtPrm.InSigMap.bitNo[2]) & 0x01) << 2 );	/* IO:2 */
	sgnl |= ( ((inSig >> hdl->mtPrm.InSigMap.bitNo[3]) & 0x01) << 3 );	/* IO:3 */

	sgnl &= hdl->mtPrm.InSigMap.bitsMsk;
	return ( sgnl );
}

/****************************************************************************
 I/O���j�^�f�[�^�擾
****************************************************************************/
USHORT MciGetInputSignals( MT_HNDL *hdl )
{
	LONG	RspSigBit;
	USHORT	sgnl;

	sgnl = 0;

	/* bit0(P-OT) */
//	bitVal = ( hdl->svRsp->RspSigBit & P_OT_SIG_BIT );
//	sgnl |= ( bitVal << P_OT_SGNL );
	if( hdl->svRsp->RspSigBit & P_OT_SIG_BITNO )
	{
		sgnl |= P_OT_SGNL;
	}

	/* bit1(N-OT) */
//	bitVal = ( hdl->svRsp->RspSigBit & N_OT_SIG_BIT );
//	sgnl |= ( bitVal << N_OT_SGNL );
	if( hdl->svRsp->RspSigBit & N_OT_SIG_BITNO )
	{
		sgnl |= N_OT_SGNL;
	}

	/* bit2(DEC) */
//	bitVal = ( hdl->svRsp->RspSigBit & DEC_SIG_BIT );
//	sgnl |= ( bitVal << DEC_SGNL );
	if( hdl->svRsp->RspSigBit & DEC_SIG_BIT )
	{
		sgnl |= DEC_SGNL;
	}

/* 2008.12.24 Y.Oka EXT1, 2, 3���Ή� */
	/* bit6(EXT1) */
//	bitVal = ( hdl->svRsp->RspSigBit & EXT1_SIG_BIT );
//	sgnl |= ( bitVal << EXT1_SGNL );
//	if( hdl->svRsp->RspSigBit & EXT1_SIG_BIT )
//	{
		sgnl |= EXT1_SGNL;
//	}

	/* bit7(EXT2) */
//	bitVal = ( hdl->svRsp->RspSigBit & EXT2_SIG_BIT );
//	sgnl |= ( bitVal << EXT2_SGNL );
//	if( hdl->svRsp->RspSigBit & EXT2_SIG_BIT )
//	{
//		sgnl |= EXT2_SGNL;
//	}

	/* bit8(EXT3) */
//	bitVal = ( hdl->svRsp->RspSigBit & EXT3_SIG_BIT );
//	sgnl |= ( bitVal << EXT3_SGNL );
//	if( hdl->svRsp->RspSigBit & EXT3_SIG_BIT )
//	{
		sgnl |= EXT3_SGNL;
//	}

	/* bit9(BRK_ON) */
//	bitVal = !( hdl->svRsp->RspSeqBit & BRK_RELEASING_BIT );
//	sgnl |= ( bitVal << BK_SGNL );
	if( !(hdl->svRsp->RspSeqBit & BRK_RELEASING_BIT) )
	{
		sgnl |= BK_SGNL;
	}

	/* bit10(EMG) */
//	bitVal = ( hdl->svRsp->RspSigBit & EMG_SIG_BIT );
//	sgnl |= ( bitVal << EMG_SGNL );
	if( hdl->svRsp->RspSigBit & EMG_SIG_BIT )
	{
		sgnl |= EMG_SGNL;
	}

	/* bit3,4,5(PA,PB,PC) */
	pls = ( hdl->svRsp->Mon7 >> 16 ) & 0x7;
	sgnl |= ( pls << PA_SGNL_SFT );

	return sgnl;
}

#endif

/****************************************************************************
 ���b�`�͈͓�/�O�m�F����
****************************************************************************/
static	BOOL MciGetLtRange( PSN_HNDL *hdl, LONG i )
{
	BOOL ltcmp;

	ltcmp = FALSE;

	if( hdl->Prm->ltRangeP > hdl->Prm->ltRangeN )
	{ /* ���b�`����l > ���b�`�����l�̏ꍇ�A���b�`�̈�� ���b�`�����l <= apos <= ���b�`����l */
		if( (hdl->Apos >= hdl->Prm->ltRangeN) && (hdl->Apos <= hdl->Prm->ltRangeP) )
		{
			ltcmp = TRUE;
		}
	}
	else
	{ /* ���b�`����l <= ���b�`�����l�̏ꍇ�A���b�`�̈�� apos <= ���b�`�����l or ���b�`����l <= apos */
		if( (hdl->Apos <= hdl->Prm->ltRangeP) || (hdl->Apos >= hdl->Prm->ltRangeN) )
		{
			ltcmp = TRUE;
		}
	}
	return ltcmp;
}


/****************************************************************************
 LtRdy(LT_EXE1,2)�̃��j�^�M���쐬
****************************************************************************/
LONG MciGetLtRdy( MT_HNDL *hdl, LONG i )
{
	if( (hdl->latch[i].LtDisable == FALSE) && (hdl->latch[i].Req == TRUE) )
	{
		/* ���b�`�L�� && ���b�`���N�G�X�g���� */
		/* Lpos�����b�`�̈���ł���΁ATRUE */
		return MciGetLtRange( &hdl->psnHdl, i );
	}
	else
	{
		/* ���b�`���� or ���b�`���N�G�X�g�Ȃ� */
		return FALSE;
	}
}





/***************************************** end of file **********************************************/
