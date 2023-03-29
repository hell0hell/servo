/****************************************************************************************************/
/*																									*/
/*																									*/
/*		MtCtrl.c : Motion Control source file 														*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*																									*/
/*		1. Create Motion Command 																	*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*		Rev.1.00 : 2012.09.05  K.Ozaki																*/
/*																									*/
/****************************************************************************************************/
#define ACCESSX_MONITOR
#include "ScanN.h"
#include "ServoIf.h"
#include "Motion.h"
#include "MtCtrl.h"
//#include "XlsAlmDefIxOpt.h"

/****************************************************************************************************/
/*																									*/
/*		Motion Control Definitions																	*/
/*																									*/
/****************************************************************************************************/
#define MOTOR_STP_SPD	(0x10000)			/* ���[�^��~���x 24bit/OS / 256						*/

/*--------------------------------------------------------------------------------------------------*/
#define LPX_SQIGETDEC()   (( SqiV.Sigs ) & SQI_SIGS_DEC )
#define LPX_SQIGETDECEDGE()  ((!(SqiV.LastSigs & SQI_SIGS_DEC )) &&  \
							(( SqiV.Sigs ) & SQI_SIGS_DEC ))

/****************************************************************************************************/
/*																									*/
/*		Motion Control Function Prototypes															*/
/*																									*/
/****************************************************************************************************/
/*--------------------------------------------------------------------------------------------------*/
/* Public functions 																				*/
/*--------------------------------------------------------------------------------------------------*/
VOID MtcInit(MTHNDL* hdl, SV_OPTION_IF* svData);
VOID MtcInputData(MTHNDL* hdl, SV_OPTION_IF  *psvif );
VOID MtcOutputData(MTHNDL* hdl, SV_OPTION_IF *psvif );

VOID MtcCreateInCmd(MTHNDL* hdl);							/* create motion in command */
INT32 MtcReqLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl);
VOID MtcCancelLatch(MTHNDL* hdl, UCHAR mode);
/*--------------------------------------------------------------------------------------------------*/
void	MtcSetHomeMethod(                                   /* set homing method */
				long prm);                                  /* method selection parameter */


/*--------------------------------------------------------------------------------------------------*/
/* Private functions(�V�[�P���X�����֘A) 															*/
/*--------------------------------------------------------------------------------------------------*/
VOID mtcCheckLatchComp(MTHNDL* hdl);

/*--------------------------------------------------------------------------------------------------*/
/* Private functions(Motion�w�ߍ쐬����) 															*/
/*--------------------------------------------------------------------------------------------------*/
UCHAR mtcMakeNopRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeFollowRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakePosingRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeInterpolRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeFeedRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeExFeedRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeHoldRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeExPosingRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeHomingRef(MTHNDL* hdl, MG_INOUT* cmdOut);			/* Homing(/DEC + CPHASE) */
UCHAR mtcMakeHomingRefDec(MTHNDL* hdl, MG_INOUT* cmdOut);		/* Homing(/DEC only) */
UCHAR mtcMakeHomingRefCphs(MTHNDL* hdl, MG_INOUT* cmdOut);		/* Homing(CPHASE only) */
UCHAR mtcMakeHomingRefPush(MTHNDL* hdl, MG_INOUT* cmdOut);		/* Homing(Push) */
UCHAR mtcMakeVelRef(MTHNDL* hdl, MG_INOUT* cmdOut);
UCHAR mtcMakeTrqRef(MTHNDL* hdl, MG_INOUT* cmdOut);

/*--------------------------------------------------------------------------------------------------*/
/* Private functions(���b�`�V�[�P���X)	 															*/
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_SEQUENTIAL_LATCH == 1
	VOID mtcControlLatchSeq(MTHNDL* hdl);
#else
	#define mtcControlLatchSeq(hdl) {;}
#endif

/*--------------------------------------------------------------------------------------------------*/
/* motion state machine function table 																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0
typedef UCHAR (*CTRLFUNC)(MTHNDL* hdl);
const CTRLFUNC _mtcControlState[] = { /* <V715> */
	mtcDisableState,
	mtcWaitPgRdyState,
	mtcReadyState,
	mtcRunState,
	mtcWaitSvOffState,
	mtcRampStopState,
};
#endif

/*--------------------------------------------------------------------------------------------------*/
/* Motion function table																			*/
/*--------------------------------------------------------------------------------------------------*/
typedef UCHAR (*MAKECMDFUNC)(MTHNDL* hdl, MG_INOUT* cmdOut);
const MAKECMDFUNC _mtcMakeCmd[] = { /* <V715> */
	mtcMakeNopRef,					/* 0:Nop���[�V���� */
	mtcMakePosingRef,				/* 1:Posing */
	mtcMakeNopRef,//	mtcMakeExPosingRef,				/* 2:ExPosing */
	mtcMakeNopRef,//	mtcMakeHomingRef,				/* 3:Homing */
	mtcMakeFeedRef,					/* 4:Feed */
	mtcMakeNopRef,//	mtcMakeExFeedRef,				/* 5:ExFeed */
	mtcMakeHoldRef,					/* 6:Hold */
	mtcMakeInterpolRef,				/* 7:Interpolate */
	mtcMakeVelRef,					/* 8:Velctrl */
	mtcMakeTrqRef,					/* 9:Trqctrl */
	mtcMakeFollowRef,				/* A:Folloup */
};

/****************************************************************************************************/
/*																									*/
/*		MtCtrl Variables 																			*/
/*																									*/
/****************************************************************************************************/
MAKECMDFUNC mtcMakeCmd[sizeof(_mtcMakeCmd)/sizeof(MAKECMDFUNC)];

/*--------------------------------------------------------------------------------------------------*/
/* ���b�`���j�^�ԍ�Table 																			*/
/*--------------------------------------------------------------------------------------------------*/
const UCHAR _LtMonTbl[] = {/* <V715> */
	MONSEL_LTPOS_C_L, MONSEL_LTPOS_EXT1_L, MONSEL_LTPOS_EXT2_L, MONSEL_LTPOS_EXT3_L
};

UCHAR LtMonTbl[sizeof(_LtMonTbl)/sizeof(UCHAR)];

/****************************************************************************************************/
/*																									*/
/*		Initialize Motion Control 																	*/
/*																									*/
/****************************************************************************************************/
#pragma arm section code = "IXX_EXCODE"
VOID MtcInit(MTHNDL* hdl, SV_OPTION_IF* svIf)
{
int i;
	MlibResetLongMemory( hdl,  sizeof(MTHNDL)/4 );					/* Reset Memory */
/*--------------------------------------------------------------------------------------------------*/
	PsnInit(&hdl->psnHdl, &hdl->mtPrm.PsnPrm);						/* Initialize Position */
	MtgInit(&hdl->mtgHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.MgPrm);	/* Initialize Generator */
/*--------------------------------------------------------------------------------------------------*/
	hdl->crtMtype = MTYPE_NONE;
	hdl->ctrlMode = MT_NO_CTRLMD;
	hdl->seqState = 0;
	hdl->svCmd = &svIf->CycCmd;
	hdl->svRsp = &svIf->CycRsp;
	
/*--------------------------------------------------------------------------------------------------*/
/* Const�e�[�u�����f�[�^RAM�֓W�J 														*//* <V715> */
/*--------------------------------------------------------------------------------------------------*/
	for( i=0; i < (sizeof(mtcMakeCmd)/4) ; i++) { mtcMakeCmd[i] = _mtcMakeCmd[i]; }

/*--------------------------------------------------------------------------------------------------*/
	return;
}
#pragma arm section code

/****************************************************************************************************/
/*																									*/
/*		Output Motion Data 																			*/
/*																									*/
/****************************************************************************************************/
VOID MtcOutputData(MTHNDL* hdl, SV_OPTION_IF *psvif)
{
SV_CYC_CMD	*psvcmd = &(psvif->CycCmd);
	psvcmd->CmdCtrl.ModSel = hdl->ctrlMode;             /* Set Control Mode */
	psvcmd->Position += hdl->outData.DposOut;			/* Set Position Command */
	psvcmd->Speed = hdl->outData.VrefOut;				/* Set Velocity Command */
	psvcmd->Torque = hdl->outData.TrefOut;				/* Set Torque Command */
}

/****************************************************************************************************/
/*																									*/
/*		Input Motion Data  																			*/
/*																									*/
/****************************************************************************************************/
VOID MtcInputData(MTHNDL* hdl, SV_OPTION_IF *psvif)
{
#define SviGetVPosErr( x )         ((x)->Mon4)
#define SviResetPos( x, data )     ((x)->Position = (data))
LONG fbSpd;
INT pls, nls;
LONG PosErr;	/* <S073> */
SV_CYC_RSP *psvrsp = &(psvif->CycRsp);
//UINT isMotorMoving = IXX_BITCHK( psvrsp->RspCtrl.RspCtrlBit, MOTOR_MOVING_BITNO );

	PosErr = hdl->svRsp->PosError;                                   /* <S073> */
	PsnUpdateFBData(&hdl->psnHdl, psvrsp->FbPosition);               /* update fb position */

	mtcCheckLatchComp(hdl);                                          /* update latch status */

	if(hdl->ctrlMode != MT_POS_CTRLMD)
	{
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		fbSpd = 0;
#else
		fbSpd = (hdl->otState != OTSTAT_NORMAL) ? 0 : PsnGetFBSPD(&hdl->psnHdl);
#endif
		MtgSetCspd(&hdl->mtgHdl, fbSpd);

		/* FollowUp��M2/M3�Ɠ����ɕύX <S073> */
		if( (hdl->svRsp->RspCtrl.RspCtrlBit & MOTOR_MOVING_BITMSK) != FALSE )
		{
			PsnResetCmdPosAll( &hdl->psnHdl, PosErr );
			SviResetPos(hdl->svCmd, psvrsp->FbPosition + PosErr );
		}
		else
		{
			PsnResetCmdPosAll(&hdl->psnHdl, 0);
			SviResetPos(hdl->svCmd, psvrsp->FbPosition );
		}
	}
/*--------------------------------------------------------------------------------------------------*/
	else if( psvrsp->RspCtrl.RspCtrlBit & (1<<PERCLREDGE_BITNO) )								
	{
		/* 
		 * @@ Perr Clear Edge 
		 * 1) If control mode is positioning and /clr signal edge detected,
		 *    reset command position to the latched position.
		 * 2) Command pulse is compensated by ScanB.
		 * */
		LONG err = ( IXX_GET_PERCLRPOS(psvif) - MtHdl.psnHdl.Pgcnt );
		PsnResetCmdPosAll( &(hdl->psnHdl), err );
	}
/*--------------------------------------------------------------------------------------------------*/
	PsnCheckSoftLimitFB( &(hdl->psnHdl), &pls, &nls );	/* check sot status */
}


/****************************************************************************
****************************************************************************/
/* Public functions (����֐�) */
/****************************************************************************
****************************************************************************/

/****************************************************************************************************/
/*																									*/
/*		Requesting Latch 																			*/
/*																									*/
/****************************************************************************************************/
INT32 MtcReqLatch(MTHNDL* hdl, UCHAR mode, UCHAR sgnl)
{
#define GET_LTSGNL_NO(no) (no)
#define SviGetLtReq(Cmd)  ((Cmd)->CmdLtBit)
#define SviSetLtReq(Cmd, data)  ((Cmd)->CmdLtBit = (data))
#define SviSetLtMonSel(Cmd, data) ((Cmd)->SelMon1 = LtMonTbl[(data)])
	UCHAR	ltReq = SviGetLtReq(hdl->svCmd);

	sgnl = GET_LTSGNL_NO(sgnl);

	hdl->latch.Mode = mode;
	hdl->latch.Cmp = 0;
	hdl->latch.Req = TRUE;
	hdl->latch.Sgnl = sgnl;

	if(IXX_BITCHK(ltReq, sgnl+4))
	{
		IXX_BITSET(ltReq, sgnl+4, FALSE);
	}
	else
	{
		IXX_BITSET(ltReq, sgnl+4, TRUE);
	}

	SviSetLtReq(hdl->svCmd, ltReq);
	SviSetLtMonSel(hdl->svCmd, sgnl);

	return 0;
}

/****************************************************************************************************/
/*																									*/
/*		Cancel Latch 																				*/
/*																									*/
/****************************************************************************************************/
VOID MtcCancelLatch(MTHNDL* hdl, UCHAR mode)
{
	hdl->latch.Cmp = 0;
	hdl->latch.Req = 0;
	hdl->latch.Mode = mode;

	hdl->latch.SeqReq = 0;
	hdl->latch.SeqCnt = 0;
	hdl->latch.SeqSts.wdCnt = 0;
}

/****************************************************************************************************/
/*																									*/
/*  �A�����b�`�v������                                                                              */
/*																									*/
/****************************************************************************************************/
#if SVFSEL_SEQUENTIAL_LATCH == 1
VOID mtcControlLatchSeq(MTHNDL* hdl)
{
/*--------------------------------------------------------------------------------------------------*/
/* ���b�`�V�[�P���X�v���`�F�b�N                                                                     */
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->latch.SeqReq != MTLT_SEQ_REQ) return;

/*--------------------------------------------------------------------------------------------------*/
/* �V�[�P���X�J�E���^�X�V                                                                           */
/*--------------------------------------------------------------------------------------------------*/
	hdl->latch.SeqCnt++;
	hdl->latch.SeqSts.cnt[1] = hdl->latch.SeqCnt;
	if(hdl->latch.SeqCnt >= hdl->mtPrm.LtPrm.seqNum)
	{                                                                          /*�V�[�P���X���� */
		hdl->latch.SeqCnt = 0;
		hdl->latch.SeqSts.cnt[0]++;
		if((hdl->mtPrm.LtPrm.loopCnt != 0) &&
			(hdl->latch.SeqSts.cnt[0] >= hdl->mtPrm.LtPrm.loopCnt))
		{                                                                      /* �A�����b�`���� */
			hdl->latch.SeqReq = MTLT_SEQ_DONE;
			return;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* ���b�`�v�����s */
/*--------------------------------------------------------------------------------------------------*/
	MtcReqLatch(hdl, hdl->latch.Mode, 
		hdl->mtPrm.LtPrm.ltSgnl[hdl->latch.SeqCnt]);
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Check latch completion 																		*/
/*																									*/
/****************************************************************************************************/
VOID mtcCheckLatchComp(MTHNDL* hdl)
{
#define	SviChkLtMonSel(Rsp, sgnl)	((Rsp)->SelMon1 == LtMonTbl[sgnl])	/* ���b�`���j�^�ݒ� */
#define SviGetLtPos(Rsp)            ((Rsp)->Mon1)                       /* Latch Position */
#define SviGetLtSts(Rsp)			((Rsp)->RspLtBit)
UINT req;
UINT cmp;
UINT ack;
/*--------------------------------------------------------------------------------------------------*/
	if(!hdl->latch.Req) return;	/* ���b�`�v���Ȃ�.�������^�[�� */
/*--------------------------------------------------------------------------------------------------*/
	req = SviGetLtReq(hdl->svCmd);
	ack = SviGetLtSts(hdl->svRsp);

	req = IXX_BITCHK(req, hdl->latch.Sgnl+4);
	cmp = IXX_BITCHK(ack, hdl->latch.Sgnl+4);
	ack = IXX_BITCHK(ack, hdl->latch.Sgnl);
/*--------------------------------------------------------------------------------------------------*/
	if( (req == ack) && (cmp == TRUE)
		&& SviChkLtMonSel(hdl->svRsp, hdl->latch.Sgnl) )
	{                                                                           /* ���b�`���� */
		if((hdl->latch.LtDisable != FALSE)                                      /* ���b�`�����A���́A*/
			|| (PsnMakeLpos(&hdl->psnHdl, SviGetLtPos(hdl->svRsp)) == FALSE))   /* ���b�`�̈�O   */
		{                                                                       
			MtcReqLatch(hdl, hdl->latch.Mode, hdl->latch.Sgnl);                 /* ���b�`�v�����s */
		}
		else
		{                                                          /* ���b�`�L�� && ���b�`�̈�� */
			hdl->latch.Req = FALSE;			                       /* ���b�`�v��OFF */
			mtcControlLatchSeq(hdl);		                       /* ���b�`�V�[�P���X�N�� */
			hdl->latch.Cmp = MTLT_CMP_STS;	                       /* ���b�`����ON */
		}
	}
}


/****************************************************************************
****************************************************************************/
/* Private functions */
/****************************************************************************
****************************************************************************/

/****************************************************************************
 ���[�V������Ԑ���(Disable���)
****************************************************************************/
#if 0
UCHAR mtcDisableState(MTHNDL* hdl)
{
	BOOL	senCmd;
	UCHAR	rc = MSTAT_DISABLE;
/* ���[�V�����w�ߏ����� */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_NO_CTRLMD;	//MT_VEL_CTRLMD;
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* ���[�V�����L���`�F�b�N */
	if(hdl->cmdData.MtEenable)
	{
		rc = MSTAT_WAITPG_RDY;
		senCmd = (hdl->mtPrm.PsnPrm.absPG != FALSE)	/* abs��:SensOn�w�ߏ��, incr��:1 */
						? hdl->cmdData.SensOn : TRUE;
		SviSetSensOn(hdl->svCmd, senCmd);
		if(senCmd && SviGetSensOnSts(hdl->svRsp) /*&& SviGetOpReadySts(hdl->svRsp)*/)
		{/* �Z���TON��� */
			hdl->cmdData.SensOn = TRUE;
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
			hdl->ctrlMode = MT_POS_CTRLMD;
			PsnResetApos(&hdl->psnHdl, TRUE);
			rc = MSTAT_READY;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 ���[�V������Ԑ���(WaitPGRdy���)
****************************************************************************/
#if 0
UCHAR mtcWaitPgRdyState(MTHNDL* hdl)
{
	BOOL	senCmd;
	UCHAR	rc = MSTAT_WAITPG_RDY;
/* ���[�V�����w�ߏ����� */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_VEL_CTRLMD;
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* ���[�V���������v���������Disble��Ԃ� */
	if(!hdl->cmdData.MtEenable)
		return MSTAT_DISABLE;
/* PGRDY�Ȃ��,Followup=ON�AAPOS�ݒ肵�AReady��Ԃ� */
	senCmd = (hdl->mtPrm.PsnPrm.absPG != FALSE)	/* abs��:SensOn�w�ߏ��, incr��:1 */
						? hdl->cmdData.SensOn : TRUE;
	SviSetSensOn(hdl->svCmd, senCmd);
	if(senCmd && SviGetSensOnSts(hdl->svRsp) /*&& SviGetOpReadySts(hdl->svRsp)*/)
	{
		hdl->cmdData.SensOn = TRUE;
		hdl->cmdData.Mtype = MTYPE_FOLLOW;
		hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
		hdl->ctrlMode = MT_POS_CTRLMD;
		PsnResetApos(&hdl->psnHdl, TRUE);
		rc = MSTAT_READY;
	}
	return rc;
}
#endif

/****************************************************************************
 ���[�V������Ԑ���(Ready���)
****************************************************************************/
#if 0
UCHAR mtcReadyState(MTHNDL* hdl)
{
	UCHAR	rc = MSTAT_READY;
	BOOL	svOnCmp = SviGetServoOnSts(hdl->svRsp);

/* ���[�V���������v���������Disble��Ԃ� */
	if(!hdl->cmdData.MtEenable)
	{
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_NO_CTRLMD;
		return MSTAT_DISABLE;
	}
/* SensOff�w��OFF �Ȃ��WaitPGRdy��Ԃ� */
	if(!hdl->cmdData.SensOn)
	{
		SviSetSensOn(hdl->svCmd, FALSE);
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_VEL_CTRLMD;
		PsnResetApos(&hdl->psnHdl, FALSE);
		return MSTAT_WAITPG_RDY;
	}
/* �T�[�{ON�w�ߔ��s*/
	SviSetServoOn(hdl->svCmd, hdl->cmdData.ServoOn);

	if(!SviGetOpReadySts(hdl->svRsp))
	{/* JOG���AJOG��́A�����I�ɑ��x���䃂�[�h�ɂ��� */
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_VEL_CTRLMD;
	}
	else
	{/* �t�H���[�A�b�v & �T�[�{ON�w�ߔ��s*/
		if(hdl->ctrlMode == MT_POS_CTRLMD)
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
	/* �T�[�{ON�����Ȃ��RUN��Ԃ� */
		if(hdl->cmdData.ServoOn && svOnCmp)
		{
			return MSTAT_RUN;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 ���[�V������Ԑ���(Run���)
****************************************************************************/
#if 0
UCHAR mtcRunState(MTHNDL* hdl)
{
	BOOL	svOnCmp;
	UCHAR	rc = MSTAT_RUN;

/* �T�[�{OFF��ԂȂ��Ready��Ԃ� */
	svOnCmp = SviGetServoOnSts(hdl->svRsp);
	if(!svOnCmp)
	{
		hdl->cmdData.ServoOn = FALSE;
		SviSetServoOn(hdl->svCmd, FALSE);
		if(hdl->ctrlMode != MT_TRQ_CTRLMD)
		{	hdl->cmdData.Mtype = MTYPE_NONE;	/* �w�ߒ�~*/}
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
		return MSTAT_READY;
	}
/* �T�[�{On�w��=OFF�Ȃ�΃T�[�{OFF�҂���Ԃ� */
	if(!hdl->cmdData.ServoOn)
	{
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
		if(hdl->mtPrm.MtSeqPrm.rampTimeOut == 0)
		{/* RampStop�@�\���� */
			SviSetServoOn(hdl->svCmd, FALSE);
			if(hdl->ctrlMode != MT_TRQ_CTRLMD)
			{	hdl->cmdData.Mtype = MTYPE_NONE;	/* �w�ߒ�~*/}
			return MSTAT_WAIT_SVOFF;
		}
		else
		{/* RampStop�@�\�L�� */
			hdl->cmdData.Mtype = MTYPE_HOLD;
			hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
			hdl->ctrlMode = MT_POS_CTRLMD;
			hdl->cmdData.HoldMod = 2;
			hdl->timer = 0;
			return MSTAT_RAMP_STOP;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 ���[�V������Ԑ���(WaitSvOff���)
****************************************************************************/
#if 0
UCHAR mtcWaitSvOffState(MTHNDL* hdl)
{
	BOOL	svOnCmp;
	UCHAR	rc = MSTAT_WAIT_SVOFF;
/* �T�[�{OFF�w�ߔ��s */
	SviSetServoOn(hdl->svCmd, FALSE);
/* �T�[�{OFF����܂Ŏw�ߒ�~ */
	if(hdl->ctrlMode != MT_TRQ_CTRLMD)
	{	hdl->cmdData.Mtype = MTYPE_NONE;}
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* �T�[�{OFF��ԂȂ��Ready��Ԃ� */
	svOnCmp = SviGetServoOnSts(hdl->svRsp);
	if(!svOnCmp)
		return MSTAT_READY;

	return rc;
}
#endif

/****************************************************************************
 ���[�V������Ԑ���(RampStop���)
****************************************************************************/
#if 0
UCHAR mtcRampStopState(MTHNDL* hdl)
{
	BOOL	svOnCmp;
	UCHAR	rc = MSTAT_RAMP_STOP;
	BOOL	den = PsnGetDenSts(&hdl->psnHdl);
/* ������~�w�߃Z�b�g */
	hdl->cmdData.Mtype = MTYPE_HOLD;
	hdl->ctrlMode = MT_POS_CTRLMD;
	hdl->cmdData.HoldMod = 2;
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* �T�[�{OFF��ԂȂ��Ready��Ԃ� */
	svOnCmp = SviGetServoOnSts(hdl->svRsp);
	if(!svOnCmp)
	{
		hdl->cmdData.ServoOn = FALSE;
		SviSetServoOn(hdl->svCmd, FALSE);
		return MSTAT_READY;
	}
/* ������~�A���́A�^�C���A�E�g������WaitSvOff��Ԃ� */
	++hdl->timer;
	if( (hdl->timer > hdl->mtPrm.MtSeqPrm.rampTimeOut)	/* �^�C���A�E�g */
		|| (den && ( ABS( SviGetSpeed(hdl->svRsp)) < MOTOR_STP_SPD)) )/* den=1&&���[�^��~ */
	{
		SviSetServoOn(hdl->svCmd, FALSE);
		return MSTAT_WAIT_SVOFF;
	}
	return rc;
}
#endif

/****************************************************************************
 OT��Ԑ���(OT��~���쒆 rc = 0, ����ȊO rc = 1)
****************************************************************************/
#if 0
#ifndef OT_STOP_POSCTRL
BOOL mtcControlOTState(MTHNDL* hdl)
{
	BOOL p_ot, n_ot;
	LONG dpos, wk, remdist;	/* <V656,V679> */

/* APOS�\�t�g���~�b�g�`�F�b�N */
	PsnCheckSoftLimitFB(&hdl->psnHdl, &p_ot, &n_ot);
/* OT�M���`�F�b�N */
	p_ot |= SviGetPOTSig(hdl->svRsp);
	n_ot |= SviGetNOTSig(hdl->svRsp);
/* OT�w�߃Z�b�g */
	SviSetPOT(hdl->svCmd, p_ot);
	SviSetNOT(hdl->svCmd, n_ot);

/* OT���������`�F�b�N */
	if( ((p_ot | n_ot) == 0)							/* OT�M������ */
		|| (SviGetServoOnSts(hdl->svRsp) == FALSE)		/* �T�[�{OFF��� */
		|| (hdl->cmdData.CtrlMode != MT_POS_CTRLMD)) 	/* ���x�E�g���N���� */
	{/* OT���� */
		hdl->otState = OTSTAT_NORMAL;
		return TRUE;
	}

/* �����߂������`�F�b�N */
	dpos = PsnGetAPOS(&hdl->psnHdl) - PsnGetPOS(&hdl->psnHdl);
/* �v�Z���ʂ��I�[�o�[�t���[����\��������̂ŖO�a���Z�������s��(�w�߃t�B���^�̗��܂蕪��31bit�ȉ�) *//* <V656> */
	wk = MlibSatAdd32(hdl->outData.DposIn, PsnGetIPOS(&hdl->psnHdl) - PsnGetPOS(&hdl->psnHdl));
//	dpos = (hdl->otState != OTSTAT_STOP) ? wk : (wk - dpos);
	dpos = (hdl->otState != OTSTAT_STOP) ? wk : MlibSatAdd32(wk, -dpos);/* <V776> */

	if( (p_ot && (!n_ot) && (dpos < 0))
		|| (n_ot && (!p_ot) && (dpos > 0)) )
	{/* �����߂����� */
//		if((hdl->outData.Tspd != 0) )
		if((hdl->outData.Tspd != 0) || (hdl->otState == OTSTAT_RETRACTION) )/* <V776> */
		{/* <V776> �ڕW���x��0 or �����߂��� */
//			hdl->otState = OTSTAT_WAITNORMAL;
			hdl->otState = OTSTAT_RETRACTION;	/* <V679> */
			return TRUE;
		}
	}
	
/* �����߂��I�������`�F�b�N <V679> */
	if((hdl->otState == OTSTAT_RETRACTION) && (dpos == 0))
	{
		remdist = PsnGetTPOS(&hdl->psnHdl) - PsnGetAPOS(&hdl->psnHdl);
		if( (p_ot && (!n_ot) && (remdist < 0))
			|| (n_ot && (!p_ot) && (remdist > 0)) )
		{
			return TRUE;
		}
	}

/* OT��Ԑ��� */
	hdl->ctrlMode = MT_VEL_CTRLMD;
	hdl->cmdData.Mtype = hdl->crtMtype = MTYPE_NONE;	/* Motion Type Clear */
	mtcMakeNopRef(hdl, &hdl->outData);			/* �w��0 */
	//hdl->cmdData.AccFilSel = ACF_SW_NOFIL;		/* �t�B���^�N���A */
	//AcfClear(&hdl->acfHdl);						/* �t�B���^�N���A */
	PsnResetIPOS(&hdl->psnHdl);					/* IPOS���Z�b�g */
	PsnSetTPOSNoLimit(&hdl->psnHdl, hdl->outData.DposIn);
	if(ABS(SviGetSpeed(hdl->svRsp)) > MOTOR_STP_SPD)
	{
//		if(hdl->otState != OTSTAT_WAITNORMAL) 
		if((hdl->otState != OTSTAT_WAITNORMAL) && (hdl->otState != OTSTAT_RETRACTION))	/* <V679> */
		{	hdl->otState = OTSTAT_STOP;}		/* OTSTAT_WAITNORMAL���͏�Ԃ��̂܂� */
		return FALSE;
	}
	hdl->otState = OTSTAT_WAITNORMAL;

	return TRUE;
}
#else
BOOL mtcControlOTState(MTHNDL* hdl)
{
	BOOL p_ot, n_ot;
	UCHAR f_cmp;
	LONG perr;
	BOOL rc = TRUE;

/* APOS�\�t�g���~�b�g�`�F�b�N */
	PsnCheckSoftLimitFB(&hdl->psnHdl, &p_ot, &n_ot);
/* OT�M���`�F�b�N */
	p_ot |= SviGetPOTSig(hdl->svRsp);
	n_ot |= SviGetNOTSig(hdl->svRsp);
/* OT�w�߃Z�b�g */
	SviSetPOT(hdl->svCmd, p_ot);
	SviSetNOT(hdl->svCmd, n_ot);

/* OT���������`�F�b�N */
	if( ((p_ot | n_ot) == 0)							/* OT�M������ */
		|| (SviGetServoOnSts(hdl->svRsp) == FALSE)		/* �T�[�{OFF��� */
		|| (hdl->cmdData.CtrlMode != MT_POS_CTRLMD)) 	/* ���x�E�g���N���� */
	{/* OT���� */
		hdl->otState = OTSTAT_NORMAL;
		return rc;
	}
	/* �����Ӂ� �{������L���ɂ���ꍇ�́A<V656>�̕ύX���e�𐅕��W�J����K�v���� */
	if( (p_ot && (!n_ot) && (hdl->outData.DposIn < 0))
		|| (n_ot && (!p_ot) && (hdl->outData.DposIn > 0)) )
	{/* �����߂����� */
		hdl->otState = OTSTAT_WAITNORMAL;
		return rc;
	}

/* OT��Ԑ��� */
	switch(hdl->otState)
	{
	case OTSTAT_NORMAL:	/* �ʏ�^�]��� */
		hdl->otState = OTSTAT_STOP;
		/* break; ���������̂܂ܒ�~�����ց����� */

	case OTSTAT_STOP:	/* OT��~�� */
		f_cmp = mtcMakeFollowRef(hdl, &hdl->outData);					/* Followup */
		if(ABS(SviGetSpeed(hdl->svRsp)) < MOTOR_STP_SPD)
		{/* ���[�^��~ */
			if((f_cmp != FALSE) && (PsnGetDenSts(&hdl->psnHdl) != FALSE))
			{/* DEN=ON�ŁA����State�� */
				hdl->otState = OTSTAT_WAITNORMAL;
			}
		}
		else
		{/* ���[�^��]���́A����OT�ɂ��A�m���Ƀ��[�^���~�߂� */
			SviSetPOT(hdl->svCmd, TRUE);							/* P-OT����ON */
			SviSetNOT(hdl->svCmd, TRUE);							/* N-OT����ON */
			hdl->outData.DposIn = PsnGetFBSPD(&hdl->psnHdl);		/* ��s���w�� */
			PsnSetTPOSNoLimit(&hdl->psnHdl, hdl->outData.DposIn);	/* TPOS���Z�b�g */
			rc = FALSE;
		}
		break;

	case OTSTAT_WAITNORMAL:	/* OT�����҂���� */
		mtcMakeFollowRef(hdl, &hdl->outData);
		if(!hdl->mtPrm.BasePrm.ot_zstop)
		{/* ����~��A���[�^���b�N���[�h�ȊO */
			perr = PsnGetPOS(&hdl->psnHdl) - PsnGetAPOS(&hdl->psnHdl);
			if(((perr > 0) & p_ot) || ((perr < 0) & n_ot))
			{/* �΍�����=OT���� */
				break;
			}
		}
		mtcMakeNopRef(hdl, &hdl->outData);		/* NOP�w�� */
		PsnSetTPOSNoLimit(&hdl->psnHdl, 0);		/* TPOS���Z�b�g */
		break;

	default : break;
	}
	return rc;
}
#endif
#endif


/****************************************************************************************************/
/*																									*/
/*		Create Motion Reference																		*/
/*																									*/
/****************************************************************************************************/
VOID MtcCreateInCmd(MTHNDL* hdl)
{
UCHAR	rc;
/*--------------------------------------------------------------------------------------------------*/
/* ���[�V�����؂�ւ�聨���[�V�������X�^�[�g                                                       */
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.Mtype != hdl->crtMtype)
	{
		hdl->seqState = 0;					                   /* Motion �V�[�P���X��ԃN���A */
	}

/*--------------------------------------------------------------------------------------------------*/
/* ������PRM�؂�ւ�                                                                                */
/*--------------------------------------------------------------------------------------------------*/
#if 0
	if(MBnkIsEnable(&hdl->mtPrm)) 
	{/* �o���N�L��(�o���NPRM�Z�b�g) */
		if(PsnGetDenSts(&hdl->psnHdl))
		{	MBnkChange(&hdl->mtPrm);	/* �o���NChange */}

		AcfAttachPRM(&hdl->acfHdl, MBnkGetAcfPrm(&hdl->mtPrm));
		if(hdl->cmdData.CmdRateOn)
		{/* �������w�ߗL��(�w��PRM�Z�b�g)<Rev2.01> */	
			MtgAttachPRM(&hdl->mtgHdl, &hdl->mtPrm.CmdRate);
		}
		else
		{/* �������w�ߖ���(�o���NPRM�Z�b�g) */
			MtgAttachPRM(&hdl->mtgHdl, MBnkGetMgPrm(&hdl->mtPrm));
		}
	}
	else
#endif
	{                                                                         /* �o���N���� */
		//AcfAttachPRM(&hdl->acfHdl, &hdl->mtPrm.AccFilPrm);
		if(0 /* hdl->cmdData.CmdRateOn */)
		{                                                    /* �������w�ߗL��(�w��PRM�Z�b�g)<Rev2.01> */	
			MtgAttachPRM(&hdl->mtgHdl, &hdl->mtPrm.CmdRate);
		}
		else
		{                                                    /* �������w�ߖ���(�f�t�H���gPRM�Z�b�g) */
			MtgAttachPRM(&hdl->mtgHdl, &hdl->mtPrm.MgPrm);
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/* ���[�V�����w�ߍ쐬����                                                                           */
/*--------------------------------------------------------------------------------------------------*/
	rc = mtcMakeCmd[hdl->cmdData.Mtype](hdl, &hdl->outData);
	hdl->crtMtype = hdl->cmdData.Mtype; 

/*--------------------------------------------------------------------------------------------------*/
#if 0
   /* 
    * �ȉ�����������ƁAFollowUp�R�}���h��1��ŏI�����Ă��܂��B
    * (Axctrl��FollowUp�w�߂��o�������Ȃ��ƁA�ʒu�΍������܂�B)
    * Indexer�ł͖����ɂ��Ă����B
    */
	if(rc != FALSE)
	{
		hdl->cmdData.Mtype = MTYPE_NONE;	/* �j���[�g������*/
	}
#endif
	hdl->cmdData.CmdRstReq = FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		Create Nop Referece																			*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeNopRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
	cmdOut->Mtype = MNONE;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return TRUE;
}

/****************************************************************************************************/
/*																									*/
/*		Create Followup reference																	*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeFollowRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
	UCHAR rc = FALSE;

	cmdOut->Mtype = MFOLLOW;
	cmdOut->DposIn = PsnGetAPOS(&hdl->psnHdl) - PsnGetPOS(&hdl->psnHdl);
/*--------------------------------------------------------------------------------------------------*/
/*  7 pulse�ȉ��̎w�߂͂��Ȃ�(Sigma-S)																*/
/*--------------------------------------------------------------------------------------------------*/
//	cmdOut->DposIn = (((MlibABS( cmdOut->DposIn ) >> 3) << 3) ? cmdOut->DposIn : 0);
/*--------------------------------------------------------------------------------------------------*/
	if(cmdOut->DposIn < (-FOLLOWUP_MAX_REF))
		cmdOut->DposIn = -FOLLOWUP_MAX_REF;	                 /* �N�����v*/
	else if(cmdOut->DposIn > FOLLOWUP_MAX_REF)
		cmdOut->DposIn = FOLLOWUP_MAX_REF;	                 /* �N�����v*/
	else
		rc = TRUE;

	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	PsnResetIPOS(&hdl->psnHdl);		                        /* IPOS���Z�b�g */
	PsnSetTPOSNoLimit(&hdl->psnHdl, cmdOut->DposIn);

	return rc;
}

/****************************************************************************************************/
/*																									*/
/*		Create Posing reference																		*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakePosingRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
	if(hdl->cmdData.bHold)													/* Hold */
	{
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
	else if(hdl->cmdData.CmdRstReq)											/* cmd reset */
	{
		PsnSetTPOS(&hdl->psnHdl, hdl->cmdData.TPos);
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		//cmdOut->Tspd = hdl->cmdData.TSpeed;
		cmdOut->Tspd = MlibMulgain29( hdl->cmdData.TSpeed, hdl->cmdData.TspdKx );
	}
/*--------------------------------------------------------------------------------------------------*/
	else
	{
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = MlibMulgain29( hdl->cmdData.TSpeed, hdl->cmdData.TspdKx );
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		Create Interpolate reference																*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeInterpolRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{

	if(hdl->cmdData.CmdRstReq)
	{
		cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, hdl->cmdData.TPos);
		cmdOut->Vref = hdl->cmdData.Velocity;
		cmdOut->Tref = hdl->cmdData.Trq;
		hdl->segCnt = hdl->mtPrm.BasePrm.seg_num;
	}
	else if(hdl->segCnt == 0)
	{/* �c�����o����=0�Ȃ�I�� */
		return mtcMakeNopRef(hdl, cmdOut);
	}
	hdl->segCnt--;

	cmdOut->Mtype = MINTERPOLATE;
	cmdOut->Tspd = cmdOut->DposIn;	/* dummy(OT�����`�F�b�N�p) */

	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		Create Feed Reference																		*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeFeedRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)													/* Hold */
	{
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
	}
/*--------------------------------------------------------------------------------------------------*/
	else if(hdl->cmdData.TSpeed < 0)
	{                                                                       /* ������ */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, -DISTANCE_MAX);
		cmdOut->Tspd = MlibMulgain29( -(hdl->cmdData.TSpeed), hdl->cmdData.TspdKx );
	}
	else
	{                                                                       /* ������ */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, DISTANCE_MAX);
		cmdOut->Tspd = MlibMulgain29( hdl->cmdData.TSpeed, hdl->cmdData.TspdKx );
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		Create Hold Reference																		*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeHoldRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
LONG dpos;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.HoldMod == 1)                                   /* �}����~ */
	{                                                               
		dpos = 0;
		cmdOut->Mtype = MNONE;
	}
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_HOLD_MOD_2 == 1                                         /* ����~�p�����x�L��(Pn826��) */
	else if(hdl->cmdData.HoldMod == 2)                             /* ������~2 */
	{                                                               
		cmdOut->Mtype = MHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL2;
		dpos = MtgGetDctDistance(&hdl->mtgHdl,
					MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
	else                                                           /* ������~1 */
	{                                                               
		cmdOut->Mtype = MHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL1;
		dpos = MtgGetDctDistance(&hdl->mtgHdl, 
					MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, dpos);
	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		Create ExPosing Reference																	*/
/*																									*/
/****************************************************************************************************/
#if 0
UCHAR mtcMakeExPosingRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
long wkx;
long seq;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)                                                /* Hold */
	{
		seq = 4;
	}
	else if(hdl->cmdData.CmdRstReq)
	{
		seq = hdl->seqState = 1;	                                      /* ���Z�b�g���́A�ŏ����� */
	}
	else
	{
		seq = hdl->seqState;
	}

/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                            /* ���b�`������� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                            /* EX_POSING�J�n */
/*--------------------------------------------------------------------------------------------------*/
		MtcCancelLatch(hdl, EXPOSING_LTMD);							   /* ���b�`�L�����Z�� */
		MtcReqLatch(hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl);	   /* ���b�`�v�����s */
		cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, hdl->cmdData.TPos);
		hdl->seqState++;
		break;

/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                            /* ���b�`�҂���� */
/*--------------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == EXPOSING_LTMD))
		{                                                              /* ���b�`���� */
			wkx = MtgGetDctDistance( &(hdl->mtgHdl),                   /* calculate distance */
							MtgGetCspd(&(hdl->mtgHdl)),
							MG_HOLD_CTRL1 );

			if ( wkx > (hdl->cmdData.Rdst) ) 
			{
				IXX_ALARMSET( ALMBITS_REGDISTSHORT ); /* A.E23 ���W�X�g���[�V���������s���A���[�� */
				cmdOut->DposIn = 0;
				hdl->seqState = 3;
			}
			else
			{
				wkx = ( (hdl->mtgHdl.cMinusDir) ?                       /* wkx <- registration offset */
				    ((-1)*(hdl->cmdData.Rdst)) : (hdl->cmdData.Rdst) );
				wkx =  PsnGetLPOS(&hdl->psnHdl) + wkx;                  /* wkx <- updated tpos */
				cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, wkx);
				hdl->seqState = 0;
			}
		}
		else
		{                                                              /* ���b�`���� */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, hdl->cmdData.TPos);
			if(cmdOut->DposIn == 0)	                                   /* �ʒu���ߊ����Ȃ�I�� */
				hdl->seqState = 0;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                            /* �A���[����� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = 0;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                            /* HOLD��� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:     
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = 0;
		break;
/*--------------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	//cmdOut->Tspd = hdl->cmdData.TSpeed;
	//cmdOut->Tspd = (hdl->seqState) ? (hdl->cmdData.TSpeed) : (hdl->cmdData.Rspd);
	wkx = (hdl->seqState) ? (hdl->cmdData.TSpeed) : (hdl->cmdData.Rspd);
	cmdOut->Tspd = (hdl->cmdData.bHold) ? 0 : MlibMulgain29( wkx, hdl->cmdData.TspdKx );
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Create Ex Feed Reference																	*/
/*																									*/
/****************************************************************************************************/
#if 0
UCHAR mtcMakeExFeedRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
/*--------------------------------------------------------------------------------------------------*/
long wkx;
long seq;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)                                                /* Hold */
	{
		seq = 4;
	}
	else if(hdl->cmdData.CmdRstReq)
	{
		seq = hdl->seqState = 1;	                                      /* ���Z�b�g���́A�ŏ����� */
	}
	else
	{
		seq = hdl->seqState;
	}

/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                            /* ���b�`������� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                            /* EX_FEED �J�n */
/*--------------------------------------------------------------------------------------------------*/
		MtcCancelLatch(hdl, EXPOSING_LTMD);							   /* ���b�`�L�����Z�� */
		MtcReqLatch(hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl);	   /* ���b�`�v�����s */
		if(hdl->cmdData.TSpeed < 0)
		{                                                                       /* ������ */
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, -DISTANCE_MAX);
		}
		else
		{                                                                       /* ������ */
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, DISTANCE_MAX);
		}
		hdl->seqState++;
		break;

/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                            /* ���b�`�҂���� */
/*--------------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == EXPOSING_LTMD))
		{                                                              /* ���b�`���� */
			wkx = MtgGetDctDistance( &(hdl->mtgHdl),                   /* calculate distance */
							MtgGetCspd(&(hdl->mtgHdl)),
							MG_HOLD_CTRL1 );

			if ( wkx > (hdl->cmdData.Rdst) ) 
			{
				IXX_ALARMSET( ALMBITS_REGDISTSHORT ); /* A.E23 ���W�X�g���[�V���������s���A���[�� */
				cmdOut->DposIn = 0;
				hdl->seqState = 3;
			}
			else
			{
				wkx = ( (hdl->mtgHdl.cMinusDir) ?                       /* wkx <- registration offset */
				    ((-1)*(hdl->cmdData.Rdst)) : (hdl->cmdData.Rdst) );
				wkx =  PsnGetLPOS(&hdl->psnHdl) + wkx;                  /* wkx <- updated tpos */
				cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, wkx);
				hdl->seqState = 0;
			}
		}
		else
		{                                                              /* ���b�`���� */
			if(hdl->cmdData.TSpeed < 0)
			{                                                                       /* ������ */
				cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, -DISTANCE_MAX);
			}
			else
			{                                                                       /* ������ */
				cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, DISTANCE_MAX);
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                            /* �A���[����� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = 0;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                            /* HOLD��� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		break;
/*--------------------------------------------------------------------------------------------------*/
	default:     
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = 0;
		break;
/*--------------------------------------------------------------------------------------------------*/
	}
/*--------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	//cmdOut->Tspd = (hdl->seqState) ? (MlibABS(hdl->cmdData.TSpeed)) : (hdl->cmdData.Rspd);
	wkx = (hdl->seqState) ? (MlibABS(hdl->cmdData.TSpeed)) : (hdl->cmdData.Rspd);
	cmdOut->Tspd = (hdl->cmdData.bHold) ? 0 : MlibMulgain29( wkx, hdl->cmdData.TspdKx );
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}
#endif



/****************************************************************************************************/
/*																									*/
/*		Create Homing Reference																		*/
/*																									*/
/*		use /DEC and CPHASE																			*/
/*																									*/
/****************************************************************************************************/
#if 0
UCHAR mtcMakeHomingRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
LONG tpos;
LONG seq;
LONG tdst;
LONG dummy;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)                                                /* Hold */
	{
		seq = 5;
	}
	else
	{
		seq = hdl->seqState;
	}
/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                                    /* ���_���A������� */
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                             /* ���Z�b�g�v�����Ȃ���΁A�w��=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				                  /* ���b�`�L�����Z�� */
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING�J�n(DEC=ON�҂����) */
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* ���t�@�����X�_�N���A */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = MlibMulgain29(hdl->cmdData.TSpeed, hdl->cmdData.TspdKx);  /* TPOS,TSPEED�Z�b�g */

		//if(SviGetDecSig(hdl->svRsp))
		if( LPX_SQIGETDEC() )
		{                                                             /* DEC=ON */
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		          /* �A�v���[�`���x1�� */
			hdl->seqState++;								          /* DEC=OFF�҂��� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* DEC=OFF�҂���� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, 	              /* set direction */
						hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;	                  /* TPOS,TSPEED=�A�v���[�`���x1�Z�b�g */

		//if(!SviGetDecSig(hdl->svRsp))
		if( ! LPX_SQIGETDEC() )
		{                                                             /* DEC=OFF */
			MtcReqLatch(hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl);	  /* ���b�`�v�����s */
			hdl->seqState++;										  /* ���b�`�����҂��� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                           /* ���b�`�����҂���� */
/*--------------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == ZRET_LTMD))
		{                                                             /* ���b�`���� */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			      /*�ŏI���s���������t�]*/
			else
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;

			tpos = PsnGetLPOS(&hdl->psnHdl) + tpos;				      /* �ŏI�ʒu�Z�b�g */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			      /* �A�v���[�`���x2�� */
			hdl->seqState++;									      /* �ʒu���ߊ����҂��� */
		}
		else
		{                                                             /* ���b�`���� */
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;				  /* �A�v���[�`���x1 */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                           /* �ʒu���ߊ����҂���� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* �A�v���[�`���x2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* �ʒu���ߊ��� */
	/*----------------------------------------------------------------------------------------------*/
	/* �b��: ���_�ʒu���W�v�Z 																		*/
	/*----------------------------------------------------------------------------------------------*/
			IxxSetOriginPos( &Npos, Npos.HomePos );                   /* set origin position        */
	/*----------------------------------------------------------------------------------------------*/
			MtcCancelLatch(hdl, NONE_LTMD);				              /* ���b�`�L�����Z�� */
			hdl->seqState = 0;							              /* ���_���A������Ԃ� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD��� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}
#endif

#if 0/* SigmaS�ł́ADEC�M���݂̂�Homing�͖��Ή� */
/****************************************************************************************************/
/*																									*/
/*		Create Homing Reference																		*/
/*																									*/
/*		use only /DEC 																				*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeHomingRefDec(MTHNDL* hdl, MG_INOUT* cmdOut)
{
LONG tpos;
LONG seq;
LONG tdst;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)                                                /* Hold */
	{
		seq = 5;
	}
	else
	{
		seq = hdl->seqState;
	}
/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                                    /* ���_���A������� */
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                             /* ���Z�b�g�v�����Ȃ���΁A�w��=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				                  /* ���b�`�L�����Z�� */
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING�J�n(DEC=ON�҂����) */
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* ���t�@�����X�_�N���A */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;                   /* TPOS,TSPEED�Z�b�g */

		if( LPX_SQIGETDECEDGE() )
		{                                                             /* DEC=ON */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			      /*�ŏI���s���������t�]*/
			else
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;

			tpos = PsnGetPOS(&hdl->psnHdl) + tpos;				      /* �ŏI�ʒu�Z�b�g */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			      /* �A�v���[�`���x2�� */
			hdl->seqState++;									      /* �ʒu���ߊ����҂��� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* �ʒu���ߊ����҂���� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* �A�v���[�`���x2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* �ʒu���ߊ��� */
			PsnSetRefPoint(&hdl->psnHdl, TRUE);			              /* ���t�@�����X�_�L�� */
	/*----------------------------------------------------------------------------------------------*/
	/* �b��: ���_�ʒu���W�v�Z 																		*/
	/*----------------------------------------------------------------------------------------------*/
			Npos.SetDst( &Npos, Npos.HomePos, &tdst );                /* Apos=PnB25�ƂȂ�悤��tdst�����߂� */
			dummy = 0;                                                /* dummy */
			tdst = MlibPcmdEgear( tdst, &Iprm.EgearX, &dummy );       /* convert position (10->13) */
			PsnSetCordinate(&hdl->psnHdl,                             /* ���_�ʒu�Z�b�g  */
							0, PsnGetPOS(&hdl->psnHdl) + tdst );
			PsnSetRefPoint(&hdl->psnHdl, TRUE);			              /* ���t�@�����X�_�L�� */
	/*----------------------------------------------------------------------------------------------*/
			hdl->seqState = 0;							              /* ���_���A������Ԃ� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD��� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}
#endif
/****************************************************************************************************/
/*																									*/
/*		Create Homing Reference																		*/
/*																									*/
/*		use only CPHASE																				*/
/*																									*/
/****************************************************************************************************/
#if 0
UCHAR mtcMakeHomingRefCphs(MTHNDL* hdl, MG_INOUT* cmdOut)
{
LONG tpos;
LONG seq;
LONG tdst;
LONG dummy;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)                                                /* Hold */
	{
		seq = 5;
	}
	else
	{
		seq = hdl->seqState;
	}
/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                           /* ���_���A�������           */
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                             /* ���Z�b�g�v�����Ȃ���΁A�w��=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				                  /* ���b�`�L�����Z�� */
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING�J�n                 */
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* ���t�@�����X�_�N���A */
		MtcReqLatch(hdl, ZRET_LTMD, MT_LATCH_CPHASE);	              /* ���b�`�v�����s */
		hdl->seqState = 2;
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* wait latch completion      */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;                   /* TPOS,TSPEED�Z�b�g */
		/*------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == ZRET_LTMD))
		{                                                             /* ���b�`���� */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			      /*�ŏI���s���������t�]*/
			else
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;

			tpos = PsnGetLPOS(&hdl->psnHdl) + tpos;				      /* �ŏI�ʒu�Z�b�g */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			      /* �A�v���[�`���x2�� */
			hdl->seqState++;									      /* �ʒu���ߊ����҂��� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                           /* �ʒu���ߊ����҂���� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* �A�v���[�`���x2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* �ʒu���ߊ��� */
	/*----------------------------------------------------------------------------------------------*/
	/* �b��: ���_�ʒu���W�v�Z 																		*/
	/*----------------------------------------------------------------------------------------------*/
			IxxSetOriginPos( &Npos, Npos.HomePos );                   /* set origin position */
	/*----------------------------------------------------------------------------------------------*/
			hdl->seqState = 0;							              /* ���_���A������Ԃ� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD��� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;

}
#endif

/****************************************************************************************************/
/*																									*/
/*		Create Homing Reference																		*/
/*																									*/
/*		use /DEC and CPHASE																			*/
/*																									*/
/****************************************************************************************************/
#if 0
UCHAR mtcMakeHomingRefPush(MTHNDL* hdl, MG_INOUT* cmdOut)
{
#define SviGetSpeed(Rsp)   ((Rsp)->Mon3)	
#define SviGetTLmtSts(Rsp) (IXX_BITCHK((Rsp)->RspCtrl.RspCtrlBit, TRQ_CLAMP_BITNO))
LONG tpos, fbspd;
LONG seq;
LONG tdst;
LONG dummy;
CHAR followFlag = FALSE;
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.bHold)                                            /* Hold						*/
	{
		seq = 5;
	}
	else
	{
		seq = hdl->seqState;
	}
/*--------------------------------------------------------------------------------------------------*/
	hdl->ZretFspdSum += SviGetSpeed( hdl->svRsp );
	hdl->ZretFspdSumCnt++;
	//fbspd = hdl->psnHdl.FbSpdx << hdl->mtPrm.BasePrm.vel_shift;
	//fbspd = MlibAbs32(fbspd);
	
	if( hdl->ZretFspdSumCnt >= KPI_SCANN_MS( 32 ) )
	{
		fbspd = MlibAbs32( hdl->ZretFspdSum / KPI_SCANN_MS( 32 ) ); /* average of 32ms */
		if( fbspd >= (hdl->mtPrm.MtSeqPrm.zretOSLevel) )
		{
			IXX_ALARMSET( ALMBITS_ZRETOVERSPEED );				  	/* A.E25 : ���_���A���޽�߰��	*/
		}
		hdl->ZretFspdSum = 0;
		hdl->ZretFspdSumCnt = 0;
	}
	//if(fbspd >= (hdl->mtPrm.MtSeqPrm.zretsp2 << 1))
	//{
	//	IXX_ALARMSET( ALMBITS_ZRETOVERSPEED );				  		/* A.E25 : ���_���A���޽�߰��	*/
	//}
/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                           /* ���_���A�������			*/
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                         /* ���Z�b�g�v�����Ȃ���΁A�w��=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING�J�n					*/
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* ���t�@�����X�_�N���A		*/
		hdl->seqState++;											  /* �ٸLimit�҂�				*/
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* �ٸLimit�҂�				*/
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl,
									 hdl->mtPrm.MtSeqPrm.zret_dir);	  /* �ڕW�ʒu�͖���������		*/
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* �A�v���[�`���x2			*/
		hdl->tlimCnt = 0;											  /* �ٸ�ЯČo�ߎ���ؾ��		*/
		
		MlibBITFILTER1( hdl->tlimSig[0], hdl->tlimSig[1], (UCHAR)SviGetTLmtSts(hdl->svRsp) );
		if(hdl->tlimSig[0])
		{
			hdl->psnHdl.TlimPerr = PsnGetPERR(&hdl->psnHdl);		  /* �ٸ�ЯĔ������̈ʒu�΍��ۑ� */
			hdl->tlimCnt++;											  /* �ٸ�ЯČo�ߎ���CntUp		*/
			hdl->seqState++;										  /* ���t�����҂�				*/
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                           /* ���t�����҂�				*/
/*--------------------------------------------------------------------------------------------------*/
																	  /* �ڕW�ʒu�́A�����ňقȂ�B */
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* �A�v���[�`���x2 			*/
		
		MlibBITFILTER1( hdl->tlimSig[0], hdl->tlimSig[1], (UCHAR)SviGetTLmtSts(hdl->svRsp) );
		if(hdl->tlimSig[0])
		{
			if((hdl->mtPrm.MtSeqPrm.tlimTimeCnt1 <= hdl->tlimCnt)
									&&(hdl->tlimCnt < hdl->mtPrm.MtSeqPrm.tlimTimeCnt2))
			{/* �ڕW�ʒu��APOS�{���Ƃ��� */
				tpos = PsnGetAPOS(&hdl->psnHdl) + hdl->psnHdl.TlimPerr;		  /* �ڕW�ʒu�쐬				*/
				cmdOut->DposIn = tpos - PsnGetIPOS(&hdl->psnHdl);			  /* �����o�����쐬				*/
				followFlag = TRUE;
			}
			else if(hdl->mtPrm.MtSeqPrm.tlimTimeCnt2 <= hdl->tlimCnt)
			{
				if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
					tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;
				else
					tpos = hdl->mtPrm.MtSeqPrm.zret_dist;			      /*�ŏI���s���������t�]*/
					
				tpos = PsnGetAPOS(&hdl->psnHdl) + tpos;				      /* �ŏI�ʒu�Z�b�g */
				cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
				hdl->seqState++;									      /* �ʒu���ߊ����҂��� */
			}
			else
			{
				cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl,
										 hdl->mtPrm.MtSeqPrm.zret_dir);	  /* �ڕW�ʒu�͖���������		*/
			}
			hdl->tlimCnt++;												  /* �ٸ�ЯČo�ߎ���CntUp		*/
		}
		else
		{
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl,
									 hdl->mtPrm.MtSeqPrm.zret_dir);	  /* �ڕW�ʒu�͖���������		*/
			if(hdl->mtPrm.MtSeqPrm.tlimTimeCnt1 <= hdl->tlimCnt)
			{/* tlimTimeCnt1�ȏ�o�ߌ���ٸ�������������ꂽ�ꍇ�́A�G���[�Ƃ��� 					*/
				IXX_ALARMSET( ALMBITS_ZRETUNCOMPLETE );			 	 /* A.E24 : ���_���A���s		*/
				hdl->seqState = 5;
			}
			else
			{
				hdl->seqState = 2;									  /* �ٸLimit�҂�				*/
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                           /* �ʒu���ߊ����҂���� */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* �A�v���[�`���x2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* �ʒu���ߊ��� */
	/*----------------------------------------------------------------------------------------------*/
	/* �b��: ���_�ʒu���W�v�Z 																		*/
	/*----------------------------------------------------------------------------------------------*/
			IxxSetOriginPos( &Npos, Npos.HomePos );                   /* set origin position */
	/*----------------------------------------------------------------------------------------------*/
			hdl->seqState = 0;							              /* ���_���A������Ԃ� */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD��� 					*/
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
		hdl->seqState = (hdl->seqState != 4)?2:4;	/* �������ł�bHold=TRUE���ƁA�ēx����Case�ɓ���	*/
		break;
	}
/*--------------------------------------------------------------------------------------------------*/
	cmdOut->Mtype = (!followFlag)?MPOSING:MFOLLOW;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Create Velocity Control Reference															*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeVelRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
	cmdOut->Mtype = MVELCTRL;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	PsnResetIPOS(&hdl->psnHdl);		                          /* IPOS���Z�b�g */
	PsnSetTPOSNoLimit(&hdl->psnHdl, 0);

	return FALSE;
}

/****************************************************************************************************/
/*																									*/
/*		Create Torque Control Reference																*/
/*																									*/
/****************************************************************************************************/
UCHAR mtcMakeTrqRef(MTHNDL* hdl, MG_INOUT* cmdOut)
{
	cmdOut->Mtype = MTRQCTRL;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	PsnResetIPOS(&hdl->psnHdl);		                         /* IPOS���Z�b�g */
	PsnSetTPOSNoLimit(&hdl->psnHdl, 0);

	return FALSE;
}


/****************************************************************************************************/
/*																									*/
/*		Set Homing Method 																			*/
/*																									*/
/****************************************************************************************************/
void	MtcSetHomeMethod( long prm )
{
	switch ( prm )
	{
#if 0
/*--------------------------------------------------------------------------------------------------*/
		case HOMEMETHOD_NOTUSE:
/*--------------------------------------------------------------------------------------------------*/
			NoutV.HomeUse = FALSE;
			NoutV.HomeMethod = HOMEMETHOD_NOTUSE;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case HOMEMETHOD_DEC_C:
/*--------------------------------------------------------------------------------------------------*/
			mtcMakeCmd[MTYPE_HOMING] = mtcMakeHomingRef;
			NoutV.HomeUse = TRUE;
			NoutV.HomeMethod = HOMEMETHOD_DEC_C;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case HOMEMETHOD_DEC:
/*--------------------------------------------------------------------------------------------------*/
			mtcMakeCmd[MTYPE_HOMING] = mtcMakeHomingRef; /* SigmaS�ł́ADEC�M���݂̂�Homing�͖��Ή� */
			NoutV.HomeUse = TRUE;
			NoutV.HomeMethod = HOMEMETHOD_DEC_C;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case HOMEMETHOD_C:
/*--------------------------------------------------------------------------------------------------*/
			mtcMakeCmd[MTYPE_HOMING] = mtcMakeHomingRefCphs;
			NoutV.HomeUse = TRUE;
			NoutV.HomeMethod = HOMEMETHOD_C;
			break;
/*--------------------------------------------------------------------------------------------------*/
		case HOMEMETHOD_PUSH:
/*--------------------------------------------------------------------------------------------------*/
			mtcMakeCmd[MTYPE_HOMING] = mtcMakeHomingRefPush;
			NoutV.HomeUse = TRUE;
			NoutV.HomeMethod = HOMEMETHOD_PUSH;
			break;
/*--------------------------------------------------------------------------------------------------*/
#endif
		default:
			NoutV.HomeUse = FALSE;
			NoutV.HomeMethod = HOMEMETHOD_NOTUSE;
//TODO			KPI_ALARMSET( ALM_OPTPRM_ERR );                   /* �b��: A.04A : ��߼�����Ұ��ݒ�ُ� */
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}
