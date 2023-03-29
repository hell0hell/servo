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
#define MOTOR_STP_SPD	(0x10000)			/* モータ停止速度 24bit/OS / 256						*/

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
/* Private functions(シーケンス処理関連) 															*/
/*--------------------------------------------------------------------------------------------------*/
VOID mtcCheckLatchComp(MTHNDL* hdl);

/*--------------------------------------------------------------------------------------------------*/
/* Private functions(Motion指令作成処理) 															*/
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
/* Private functions(ラッチシーケンス)	 															*/
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
	mtcMakeNopRef,					/* 0:Nopモーション */
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
/* ラッチモニタ番号Table 																			*/
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
/* ConstテーブルをデータRAMへ展開 														*//* <V715> */
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

		/* FollowUpをM2/M3と同じに変更 <S073> */
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
/* Public functions (操作関数) */
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
/*  連続ラッチ要求処理                                                                              */
/*																									*/
/****************************************************************************************************/
#if SVFSEL_SEQUENTIAL_LATCH == 1
VOID mtcControlLatchSeq(MTHNDL* hdl)
{
/*--------------------------------------------------------------------------------------------------*/
/* ラッチシーケンス要求チェック                                                                     */
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->latch.SeqReq != MTLT_SEQ_REQ) return;

/*--------------------------------------------------------------------------------------------------*/
/* シーケンスカウンタ更新                                                                           */
/*--------------------------------------------------------------------------------------------------*/
	hdl->latch.SeqCnt++;
	hdl->latch.SeqSts.cnt[1] = hdl->latch.SeqCnt;
	if(hdl->latch.SeqCnt >= hdl->mtPrm.LtPrm.seqNum)
	{                                                                          /*シーケンス完了 */
		hdl->latch.SeqCnt = 0;
		hdl->latch.SeqSts.cnt[0]++;
		if((hdl->mtPrm.LtPrm.loopCnt != 0) &&
			(hdl->latch.SeqSts.cnt[0] >= hdl->mtPrm.LtPrm.loopCnt))
		{                                                                      /* 連続ラッチ完了 */
			hdl->latch.SeqReq = MTLT_SEQ_DONE;
			return;
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/* ラッチ要求発行 */
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
#define	SviChkLtMonSel(Rsp, sgnl)	((Rsp)->SelMon1 == LtMonTbl[sgnl])	/* ラッチモニタ設定 */
#define SviGetLtPos(Rsp)            ((Rsp)->Mon1)                       /* Latch Position */
#define SviGetLtSts(Rsp)			((Rsp)->RspLtBit)
UINT req;
UINT cmp;
UINT ack;
/*--------------------------------------------------------------------------------------------------*/
	if(!hdl->latch.Req) return;	/* ラッチ要求なし.強制リターン */
/*--------------------------------------------------------------------------------------------------*/
	req = SviGetLtReq(hdl->svCmd);
	ack = SviGetLtSts(hdl->svRsp);

	req = IXX_BITCHK(req, hdl->latch.Sgnl+4);
	cmp = IXX_BITCHK(ack, hdl->latch.Sgnl+4);
	ack = IXX_BITCHK(ack, hdl->latch.Sgnl);
/*--------------------------------------------------------------------------------------------------*/
	if( (req == ack) && (cmp == TRUE)
		&& SviChkLtMonSel(hdl->svRsp, hdl->latch.Sgnl) )
	{                                                                           /* ラッチ完了 */
		if((hdl->latch.LtDisable != FALSE)                                      /* ラッチ無効、又は、*/
			|| (PsnMakeLpos(&hdl->psnHdl, SviGetLtPos(hdl->svRsp)) == FALSE))   /* ラッチ領域外   */
		{                                                                       
			MtcReqLatch(hdl, hdl->latch.Mode, hdl->latch.Sgnl);                 /* ラッチ要求発行 */
		}
		else
		{                                                          /* ラッチ有効 && ラッチ領域内 */
			hdl->latch.Req = FALSE;			                       /* ラッチ要求OFF */
			mtcControlLatchSeq(hdl);		                       /* ラッチシーケンス起動 */
			hdl->latch.Cmp = MTLT_CMP_STS;	                       /* ラッチ完了ON */
		}
	}
}


/****************************************************************************
****************************************************************************/
/* Private functions */
/****************************************************************************
****************************************************************************/

/****************************************************************************
 モーション状態制御(Disable状態)
****************************************************************************/
#if 0
UCHAR mtcDisableState(MTHNDL* hdl)
{
	BOOL	senCmd;
	UCHAR	rc = MSTAT_DISABLE;
/* モーション指令初期化 */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_NO_CTRLMD;	//MT_VEL_CTRLMD;
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* モーション有効チェック */
	if(hdl->cmdData.MtEenable)
	{
		rc = MSTAT_WAITPG_RDY;
		senCmd = (hdl->mtPrm.PsnPrm.absPG != FALSE)	/* abs時:SensOn指令状態, incr時:1 */
						? hdl->cmdData.SensOn : TRUE;
		SviSetSensOn(hdl->svCmd, senCmd);
		if(senCmd && SviGetSensOnSts(hdl->svRsp) /*&& SviGetOpReadySts(hdl->svRsp)*/)
		{/* センサON状態 */
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
 モーション状態制御(WaitPGRdy状態)
****************************************************************************/
#if 0
UCHAR mtcWaitPgRdyState(MTHNDL* hdl)
{
	BOOL	senCmd;
	UCHAR	rc = MSTAT_WAITPG_RDY;
/* モーション指令初期化 */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_VEL_CTRLMD;
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* モーション無効要求があればDisble状態へ */
	if(!hdl->cmdData.MtEenable)
		return MSTAT_DISABLE;
/* PGRDYならば,Followup=ON、APOS設定し、Ready状態へ */
	senCmd = (hdl->mtPrm.PsnPrm.absPG != FALSE)	/* abs時:SensOn指令状態, incr時:1 */
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
 モーション状態制御(Ready状態)
****************************************************************************/
#if 0
UCHAR mtcReadyState(MTHNDL* hdl)
{
	UCHAR	rc = MSTAT_READY;
	BOOL	svOnCmp = SviGetServoOnSts(hdl->svRsp);

/* モーション無効要求があればDisble状態へ */
	if(!hdl->cmdData.MtEenable)
	{
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_NO_CTRLMD;
		return MSTAT_DISABLE;
	}
/* SensOff指令OFF ならばWaitPGRdy状態へ */
	if(!hdl->cmdData.SensOn)
	{
		SviSetSensOn(hdl->svCmd, FALSE);
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_VEL_CTRLMD;
		PsnResetApos(&hdl->psnHdl, FALSE);
		return MSTAT_WAITPG_RDY;
	}
/* サーボON指令発行*/
	SviSetServoOn(hdl->svCmd, hdl->cmdData.ServoOn);

	if(!SviGetOpReadySts(hdl->svRsp))
	{/* JOG中、JOG後は、強制的に速度制御モードにする */
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_VEL_CTRLMD;
	}
	else
	{/* フォローアップ & サーボON指令発行*/
		if(hdl->ctrlMode == MT_POS_CTRLMD)
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
	/* サーボON完了ならばRUN状態へ */
		if(hdl->cmdData.ServoOn && svOnCmp)
		{
			return MSTAT_RUN;
		}
	}
	return rc;
}
#endif

/****************************************************************************
 モーション状態制御(Run状態)
****************************************************************************/
#if 0
UCHAR mtcRunState(MTHNDL* hdl)
{
	BOOL	svOnCmp;
	UCHAR	rc = MSTAT_RUN;

/* サーボOFF状態ならばReady状態へ */
	svOnCmp = SviGetServoOnSts(hdl->svRsp);
	if(!svOnCmp)
	{
		hdl->cmdData.ServoOn = FALSE;
		SviSetServoOn(hdl->svCmd, FALSE);
		if(hdl->ctrlMode != MT_TRQ_CTRLMD)
		{	hdl->cmdData.Mtype = MTYPE_NONE;	/* 指令停止*/}
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
		return MSTAT_READY;
	}
/* サーボOn指令=OFFならばサーボOFF待ち状態へ */
	if(!hdl->cmdData.ServoOn)
	{
		SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
		SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
		if(hdl->mtPrm.MtSeqPrm.rampTimeOut == 0)
		{/* RampStop機能無効 */
			SviSetServoOn(hdl->svCmd, FALSE);
			if(hdl->ctrlMode != MT_TRQ_CTRLMD)
			{	hdl->cmdData.Mtype = MTYPE_NONE;	/* 指令停止*/}
			return MSTAT_WAIT_SVOFF;
		}
		else
		{/* RampStop機能有効 */
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
 モーション状態制御(WaitSvOff状態)
****************************************************************************/
#if 0
UCHAR mtcWaitSvOffState(MTHNDL* hdl)
{
	BOOL	svOnCmp;
	UCHAR	rc = MSTAT_WAIT_SVOFF;
/* サーボOFF指令発行 */
	SviSetServoOn(hdl->svCmd, FALSE);
/* サーボOFFするまで指令停止 */
	if(hdl->ctrlMode != MT_TRQ_CTRLMD)
	{	hdl->cmdData.Mtype = MTYPE_NONE;}
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* サーボOFF状態ならばReady状態へ */
	svOnCmp = SviGetServoOnSts(hdl->svRsp);
	if(!svOnCmp)
		return MSTAT_READY;

	return rc;
}
#endif

/****************************************************************************
 モーション状態制御(RampStop状態)
****************************************************************************/
#if 0
UCHAR mtcRampStopState(MTHNDL* hdl)
{
	BOOL	svOnCmp;
	UCHAR	rc = MSTAT_RAMP_STOP;
	BOOL	den = PsnGetDenSts(&hdl->psnHdl);
/* 減速停止指令セット */
	hdl->cmdData.Mtype = MTYPE_HOLD;
	hdl->ctrlMode = MT_POS_CTRLMD;
	hdl->cmdData.HoldMod = 2;
	SviSetTrqLimitP(hdl->svCmd, MAX_TRQ24);
	SviSetTrqLimitN(hdl->svCmd, MAX_TRQ24);
/* サーボOFF状態ならばReady状態へ */
	svOnCmp = SviGetServoOnSts(hdl->svRsp);
	if(!svOnCmp)
	{
		hdl->cmdData.ServoOn = FALSE;
		SviSetServoOn(hdl->svCmd, FALSE);
		return MSTAT_READY;
	}
/* 減速停止、又は、タイムアウトしたらWaitSvOff状態へ */
	++hdl->timer;
	if( (hdl->timer > hdl->mtPrm.MtSeqPrm.rampTimeOut)	/* タイムアウト */
		|| (den && ( ABS( SviGetSpeed(hdl->svRsp)) < MOTOR_STP_SPD)) )/* den=1&&モータ停止 */
	{
		SviSetServoOn(hdl->svCmd, FALSE);
		return MSTAT_WAIT_SVOFF;
	}
	return rc;
}
#endif

/****************************************************************************
 OT状態制御(OT停止動作中 rc = 0, それ以外 rc = 1)
****************************************************************************/
#if 0
#ifndef OT_STOP_POSCTRL
BOOL mtcControlOTState(MTHNDL* hdl)
{
	BOOL p_ot, n_ot;
	LONG dpos, wk, remdist;	/* <V656,V679> */

/* APOSソフトリミットチェック */
	PsnCheckSoftLimitFB(&hdl->psnHdl, &p_ot, &n_ot);
/* OT信号チェック */
	p_ot |= SviGetPOTSig(hdl->svRsp);
	n_ot |= SviGetNOTSig(hdl->svRsp);
/* OT指令セット */
	SviSetPOT(hdl->svCmd, p_ot);
	SviSetNOT(hdl->svCmd, n_ot);

/* OT解除条件チェック */
	if( ((p_ot | n_ot) == 0)							/* OT信号解除 */
		|| (SviGetServoOnSts(hdl->svRsp) == FALSE)		/* サーボOFF状態 */
		|| (hdl->cmdData.CtrlMode != MT_POS_CTRLMD)) 	/* 速度・トルク制御 */
	{/* OT解除 */
		hdl->otState = OTSTAT_NORMAL;
		return TRUE;
	}

/* 引き戻し条件チェック */
	dpos = PsnGetAPOS(&hdl->psnHdl) - PsnGetPOS(&hdl->psnHdl);
/* 計算結果がオーバーフローする可能性があるので飽和加算処理を行う(指令フィルタの溜まり分は31bit以下) *//* <V656> */
	wk = MlibSatAdd32(hdl->outData.DposIn, PsnGetIPOS(&hdl->psnHdl) - PsnGetPOS(&hdl->psnHdl));
//	dpos = (hdl->otState != OTSTAT_STOP) ? wk : (wk - dpos);
	dpos = (hdl->otState != OTSTAT_STOP) ? wk : MlibSatAdd32(wk, -dpos);/* <V776> */

	if( (p_ot && (!n_ot) && (dpos < 0))
		|| (n_ot && (!p_ot) && (dpos > 0)) )
	{/* 引き戻しあり */
//		if((hdl->outData.Tspd != 0) )
		if((hdl->outData.Tspd != 0) || (hdl->otState == OTSTAT_RETRACTION) )/* <V776> */
		{/* <V776> 目標速度≠0 or 引き戻し中 */
//			hdl->otState = OTSTAT_WAITNORMAL;
			hdl->otState = OTSTAT_RETRACTION;	/* <V679> */
			return TRUE;
		}
	}
	
/* 引き戻し終了条件チェック <V679> */
	if((hdl->otState == OTSTAT_RETRACTION) && (dpos == 0))
	{
		remdist = PsnGetTPOS(&hdl->psnHdl) - PsnGetAPOS(&hdl->psnHdl);
		if( (p_ot && (!n_ot) && (remdist < 0))
			|| (n_ot && (!p_ot) && (remdist > 0)) )
		{
			return TRUE;
		}
	}

/* OT状態制御 */
	hdl->ctrlMode = MT_VEL_CTRLMD;
	hdl->cmdData.Mtype = hdl->crtMtype = MTYPE_NONE;	/* Motion Type Clear */
	mtcMakeNopRef(hdl, &hdl->outData);			/* 指令0 */
	//hdl->cmdData.AccFilSel = ACF_SW_NOFIL;		/* フィルタクリア */
	//AcfClear(&hdl->acfHdl);						/* フィルタクリア */
	PsnResetIPOS(&hdl->psnHdl);					/* IPOSリセット */
	PsnSetTPOSNoLimit(&hdl->psnHdl, hdl->outData.DposIn);
	if(ABS(SviGetSpeed(hdl->svRsp)) > MOTOR_STP_SPD)
	{
//		if(hdl->otState != OTSTAT_WAITNORMAL) 
		if((hdl->otState != OTSTAT_WAITNORMAL) && (hdl->otState != OTSTAT_RETRACTION))	/* <V679> */
		{	hdl->otState = OTSTAT_STOP;}		/* OTSTAT_WAITNORMAL時は状態そのまま */
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

/* APOSソフトリミットチェック */
	PsnCheckSoftLimitFB(&hdl->psnHdl, &p_ot, &n_ot);
/* OT信号チェック */
	p_ot |= SviGetPOTSig(hdl->svRsp);
	n_ot |= SviGetNOTSig(hdl->svRsp);
/* OT指令セット */
	SviSetPOT(hdl->svCmd, p_ot);
	SviSetNOT(hdl->svCmd, n_ot);

/* OT解除条件チェック */
	if( ((p_ot | n_ot) == 0)							/* OT信号解除 */
		|| (SviGetServoOnSts(hdl->svRsp) == FALSE)		/* サーボOFF状態 */
		|| (hdl->cmdData.CtrlMode != MT_POS_CTRLMD)) 	/* 速度・トルク制御 */
	{/* OT解除 */
		hdl->otState = OTSTAT_NORMAL;
		return rc;
	}
	/* ★注意★ 本処理を有効にする場合は、<V656>の変更内容を水平展開する必要あり */
	if( (p_ot && (!n_ot) && (hdl->outData.DposIn < 0))
		|| (n_ot && (!p_ot) && (hdl->outData.DposIn > 0)) )
	{/* 引き戻しあり */
		hdl->otState = OTSTAT_WAITNORMAL;
		return rc;
	}

/* OT状態制御 */
	switch(hdl->otState)
	{
	case OTSTAT_NORMAL:	/* 通常運転状態 */
		hdl->otState = OTSTAT_STOP;
		/* break; ↓↓↓そのまま停止処理へ↓↓↓ */

	case OTSTAT_STOP:	/* OT停止中 */
		f_cmp = mtcMakeFollowRef(hdl, &hdl->outData);					/* Followup */
		if(ABS(SviGetSpeed(hdl->svRsp)) < MOTOR_STP_SPD)
		{/* モータ停止 */
			if((f_cmp != FALSE) && (PsnGetDenSts(&hdl->psnHdl) != FALSE))
			{/* DEN=ONで、次のStateへ */
				hdl->otState = OTSTAT_WAITNORMAL;
			}
		}
		else
		{/* モータ回転中は、強制OTにし、確実にモータを止める */
			SviSetPOT(hdl->svCmd, TRUE);							/* P-OT強制ON */
			SviSetNOT(hdl->svCmd, TRUE);							/* N-OT強制ON */
			hdl->outData.DposIn = PsnGetFBSPD(&hdl->psnHdl);		/* 先行き指令 */
			PsnSetTPOSNoLimit(&hdl->psnHdl, hdl->outData.DposIn);	/* TPOSリセット */
			rc = FALSE;
		}
		break;

	case OTSTAT_WAITNORMAL:	/* OT解除待ち状態 */
		mtcMakeFollowRef(hdl, &hdl->outData);
		if(!hdl->mtPrm.BasePrm.ot_zstop)
		{/* 非常停止後、モータロックモード以外 */
			perr = PsnGetPOS(&hdl->psnHdl) - PsnGetAPOS(&hdl->psnHdl);
			if(((perr > 0) & p_ot) || ((perr < 0) & n_ot))
			{/* 偏差方向=OT方向 */
				break;
			}
		}
		mtcMakeNopRef(hdl, &hdl->outData);		/* NOP指令 */
		PsnSetTPOSNoLimit(&hdl->psnHdl, 0);		/* TPOSリセット */
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
/* モーション切り替わり→モーションリスタート                                                       */
/*--------------------------------------------------------------------------------------------------*/
	if(hdl->cmdData.Mtype != hdl->crtMtype)
	{
		hdl->seqState = 0;					                   /* Motion シーケンス状態クリア */
	}

/*--------------------------------------------------------------------------------------------------*/
/* 加減速PRM切り替え                                                                                */
/*--------------------------------------------------------------------------------------------------*/
#if 0
	if(MBnkIsEnable(&hdl->mtPrm)) 
	{/* バンク有効(バンクPRMセット) */
		if(PsnGetDenSts(&hdl->psnHdl))
		{	MBnkChange(&hdl->mtPrm);	/* バンクChange */}

		AcfAttachPRM(&hdl->acfHdl, MBnkGetAcfPrm(&hdl->mtPrm));
		if(hdl->cmdData.CmdRateOn)
		{/* 加減速指令有効(指令PRMセット)<Rev2.01> */	
			MtgAttachPRM(&hdl->mtgHdl, &hdl->mtPrm.CmdRate);
		}
		else
		{/* 加減速指令無効(バンクPRMセット) */
			MtgAttachPRM(&hdl->mtgHdl, MBnkGetMgPrm(&hdl->mtPrm));
		}
	}
	else
#endif
	{                                                                         /* バンク無効 */
		//AcfAttachPRM(&hdl->acfHdl, &hdl->mtPrm.AccFilPrm);
		if(0 /* hdl->cmdData.CmdRateOn */)
		{                                                    /* 加減速指令有効(指令PRMセット)<Rev2.01> */	
			MtgAttachPRM(&hdl->mtgHdl, &hdl->mtPrm.CmdRate);
		}
		else
		{                                                    /* 加減速指令無効(デフォルトPRMセット) */
			MtgAttachPRM(&hdl->mtgHdl, &hdl->mtPrm.MgPrm);
		}
	}

/*--------------------------------------------------------------------------------------------------*/
/* モーション指令作成処理                                                                           */
/*--------------------------------------------------------------------------------------------------*/
	rc = mtcMakeCmd[hdl->cmdData.Mtype](hdl, &hdl->outData);
	hdl->crtMtype = hdl->cmdData.Mtype; 

/*--------------------------------------------------------------------------------------------------*/
#if 0
   /* 
    * 以下処理があると、FollowUpコマンドが1回で終了してしまう。
    * (AxctrlにFollowUp指令を出し続けないと、位置偏差がたまる。)
    * Indexerでは無効にしておく。
    */
	if(rc != FALSE)
	{
		hdl->cmdData.Mtype = MTYPE_NONE;	/* ニュートラルへ*/
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
/*  7 pulse以下の指令はしない(Sigma-S)																*/
/*--------------------------------------------------------------------------------------------------*/
//	cmdOut->DposIn = (((MlibABS( cmdOut->DposIn ) >> 3) << 3) ? cmdOut->DposIn : 0);
/*--------------------------------------------------------------------------------------------------*/
	if(cmdOut->DposIn < (-FOLLOWUP_MAX_REF))
		cmdOut->DposIn = -FOLLOWUP_MAX_REF;	                 /* クランプ*/
	else if(cmdOut->DposIn > FOLLOWUP_MAX_REF)
		cmdOut->DposIn = FOLLOWUP_MAX_REF;	                 /* クランプ*/
	else
		rc = TRUE;

	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	PsnResetIPOS(&hdl->psnHdl);		                        /* IPOSリセット */
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
	{/* 残払い出し回数=0なら終了 */
		return mtcMakeNopRef(hdl, cmdOut);
	}
	hdl->segCnt--;

	cmdOut->Mtype = MINTERPOLATE;
	cmdOut->Tspd = cmdOut->DposIn;	/* dummy(OT処理チェック用) */

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
	{                                                                       /* 負方向 */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, -DISTANCE_MAX);
		cmdOut->Tspd = MlibMulgain29( -(hdl->cmdData.TSpeed), hdl->cmdData.TspdKx );
	}
	else
	{                                                                       /* 正方向 */
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
	if(hdl->cmdData.HoldMod == 1)                                   /* 急速停止 */
	{                                                               
		dpos = 0;
		cmdOut->Mtype = MNONE;
	}
/*--------------------------------------------------------------------------------------------------*/
#if SVFSEL_HOLD_MOD_2 == 1                                         /* 非常停止用減速度有効(Pn826他) */
	else if(hdl->cmdData.HoldMod == 2)                             /* 減速停止2 */
	{                                                               
		cmdOut->Mtype = MHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL2;
		dpos = MtgGetDctDistance(&hdl->mtgHdl,
					MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
	else                                                           /* 減速停止1 */
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
		seq = hdl->seqState = 1;	                                      /* リセット時は、最初から */
	}
	else
	{
		seq = hdl->seqState;
	}

/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                            /* ラッチ完了状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                            /* EX_POSING開始 */
/*--------------------------------------------------------------------------------------------------*/
		MtcCancelLatch(hdl, EXPOSING_LTMD);							   /* ラッチキャンセル */
		MtcReqLatch(hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl);	   /* ラッチ要求発行 */
		cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, hdl->cmdData.TPos);
		hdl->seqState++;
		break;

/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                            /* ラッチ待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == EXPOSING_LTMD))
		{                                                              /* ラッチ完了 */
			wkx = MtgGetDctDistance( &(hdl->mtgHdl),                   /* calculate distance */
							MtgGetCspd(&(hdl->mtgHdl)),
							MG_HOLD_CTRL1 );

			if ( wkx > (hdl->cmdData.Rdst) ) 
			{
				IXX_ALARMSET( ALMBITS_REGDISTSHORT ); /* A.E23 レジストレーション距離不足アラーム */
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
		{                                                              /* ラッチ未完 */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, hdl->cmdData.TPos);
			if(cmdOut->DposIn == 0)	                                   /* 位置決め完了なら終了 */
				hdl->seqState = 0;
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                            /* アラーム状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = 0;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                            /* HOLD状態 */
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
		seq = hdl->seqState = 1;	                                      /* リセット時は、最初から */
	}
	else
	{
		seq = hdl->seqState;
	}

/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                            /* ラッチ完了状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                            /* EX_FEED 開始 */
/*--------------------------------------------------------------------------------------------------*/
		MtcCancelLatch(hdl, EXPOSING_LTMD);							   /* ラッチキャンセル */
		MtcReqLatch(hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl);	   /* ラッチ要求発行 */
		if(hdl->cmdData.TSpeed < 0)
		{                                                                       /* 負方向 */
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, -DISTANCE_MAX);
		}
		else
		{                                                                       /* 正方向 */
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, DISTANCE_MAX);
		}
		hdl->seqState++;
		break;

/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                            /* ラッチ待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == EXPOSING_LTMD))
		{                                                              /* ラッチ完了 */
			wkx = MtgGetDctDistance( &(hdl->mtgHdl),                   /* calculate distance */
							MtgGetCspd(&(hdl->mtgHdl)),
							MG_HOLD_CTRL1 );

			if ( wkx > (hdl->cmdData.Rdst) ) 
			{
				IXX_ALARMSET( ALMBITS_REGDISTSHORT ); /* A.E23 レジストレーション距離不足アラーム */
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
		{                                                              /* ラッチ未完 */
			if(hdl->cmdData.TSpeed < 0)
			{                                                                       /* 負方向 */
				cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, -DISTANCE_MAX);
			}
			else
			{                                                                       /* 正方向 */
				cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, DISTANCE_MAX);
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                            /* アラーム状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = 0;
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                            /* HOLD状態 */
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
	case 0:                                                                    /* 原点復帰完了状態 */
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                             /* リセット要求がなければ、指令=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				                  /* ラッチキャンセル */
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING開始(DEC=ON待ち状態) */
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* リファレンス点クリア */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = MlibMulgain29(hdl->cmdData.TSpeed, hdl->cmdData.TspdKx);  /* TPOS,TSPEEDセット */

		//if(SviGetDecSig(hdl->svRsp))
		if( LPX_SQIGETDEC() )
		{                                                             /* DEC=ON */
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		          /* アプローチ速度1へ */
			hdl->seqState++;								          /* DEC=OFF待ちへ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* DEC=OFF待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, 	              /* set direction */
						hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;	                  /* TPOS,TSPEED=アプローチ速度1セット */

		//if(!SviGetDecSig(hdl->svRsp))
		if( ! LPX_SQIGETDEC() )
		{                                                             /* DEC=OFF */
			MtcReqLatch(hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl);	  /* ラッチ要求発行 */
			hdl->seqState++;										  /* ラッチ完了待ちへ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                           /* ラッチ完了待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == ZRET_LTMD))
		{                                                             /* ラッチ完了 */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			      /*最終走行距離符号逆転*/
			else
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;

			tpos = PsnGetLPOS(&hdl->psnHdl) + tpos;				      /* 最終位置セット */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			      /* アプローチ速度2へ */
			hdl->seqState++;									      /* 位置決め完了待ちへ */
		}
		else
		{                                                             /* ラッチ未完 */
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;				  /* アプローチ速度1 */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                           /* 位置決め完了待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* アプローチ速度2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* 位置決め完了 */
	/*----------------------------------------------------------------------------------------------*/
	/* 暫定: 原点位置座標計算 																		*/
	/*----------------------------------------------------------------------------------------------*/
			IxxSetOriginPos( &Npos, Npos.HomePos );                   /* set origin position        */
	/*----------------------------------------------------------------------------------------------*/
			MtcCancelLatch(hdl, NONE_LTMD);				              /* ラッチキャンセル */
			hdl->seqState = 0;							              /* 原点復帰完了状態へ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD状態 */
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

#if 0/* SigmaSでは、DEC信号のみのHomingは未対応 */
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
	case 0:                                                                    /* 原点復帰完了状態 */
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                             /* リセット要求がなければ、指令=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				                  /* ラッチキャンセル */
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING開始(DEC=ON待ち状態) */
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* リファレンス点クリア */
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;                   /* TPOS,TSPEEDセット */

		if( LPX_SQIGETDECEDGE() )
		{                                                             /* DEC=ON */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			      /*最終走行距離符号逆転*/
			else
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;

			tpos = PsnGetPOS(&hdl->psnHdl) + tpos;				      /* 最終位置セット */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			      /* アプローチ速度2へ */
			hdl->seqState++;									      /* 位置決め完了待ちへ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* 位置決め完了待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* アプローチ速度2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* 位置決め完了 */
			PsnSetRefPoint(&hdl->psnHdl, TRUE);			              /* リファレンス点有効 */
	/*----------------------------------------------------------------------------------------------*/
	/* 暫定: 原点位置座標計算 																		*/
	/*----------------------------------------------------------------------------------------------*/
			Npos.SetDst( &Npos, Npos.HomePos, &tdst );                /* Apos=PnB25となるようなtdstを求める */
			dummy = 0;                                                /* dummy */
			tdst = MlibPcmdEgear( tdst, &Iprm.EgearX, &dummy );       /* convert position (10->13) */
			PsnSetCordinate(&hdl->psnHdl,                             /* 原点位置セット  */
							0, PsnGetPOS(&hdl->psnHdl) + tdst );
			PsnSetRefPoint(&hdl->psnHdl, TRUE);			              /* リファレンス点有効 */
	/*----------------------------------------------------------------------------------------------*/
			hdl->seqState = 0;							              /* 原点復帰完了状態へ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD状態 */
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
	case 0:                                                           /* 原点復帰完了状態           */
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                             /* リセット要求がなければ、指令=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				                  /* ラッチキャンセル */
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING開始                 */
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* リファレンス点クリア */
		MtcReqLatch(hdl, ZRET_LTMD, MT_LATCH_CPHASE);	              /* ラッチ要求発行 */
		hdl->seqState = 2;
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* wait latch completion      */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;                   /* TPOS,TSPEEDセット */
		/*------------------------------------------------------------------------------------------*/
		if(hdl->latch.Cmp && (hdl->latch.Mode == ZRET_LTMD))
		{                                                             /* ラッチ完了 */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			      /*最終走行距離符号逆転*/
			else
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;

			tpos = PsnGetLPOS(&hdl->psnHdl) + tpos;				      /* 最終位置セット */
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			      /* アプローチ速度2へ */
			hdl->seqState++;									      /* 位置決め完了待ちへ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                           /* 位置決め完了待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* アプローチ速度2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* 位置決め完了 */
	/*----------------------------------------------------------------------------------------------*/
	/* 暫定: 原点位置座標計算 																		*/
	/*----------------------------------------------------------------------------------------------*/
			IxxSetOriginPos( &Npos, Npos.HomePos );                   /* set origin position */
	/*----------------------------------------------------------------------------------------------*/
			hdl->seqState = 0;							              /* 原点復帰完了状態へ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD状態 */
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
			IXX_ALARMSET( ALMBITS_ZRETOVERSPEED );				  	/* A.E25 : 原点復帰ｵｰﾊﾞｽﾋﾟｰﾄﾞ	*/
		}
		hdl->ZretFspdSum = 0;
		hdl->ZretFspdSumCnt = 0;
	}
	//if(fbspd >= (hdl->mtPrm.MtSeqPrm.zretsp2 << 1))
	//{
	//	IXX_ALARMSET( ALMBITS_ZRETOVERSPEED );				  		/* A.E25 : 原点復帰ｵｰﾊﾞｽﾋﾟｰﾄﾞ	*/
	//}
/*--------------------------------------------------------------------------------------------------*/
	switch( seq )
	{
/*--------------------------------------------------------------------------------------------------*/
	case 0:                                                           /* 原点復帰完了状態			*/
/*--------------------------------------------------------------------------------------------------*/
		if(!hdl->cmdData.CmdRstReq)
		{                                                         /* リセット要求がなければ、指令=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		hdl->seqState = 1;
/*--------------------------------------------------------------------------------------------------*/
	case 1:                                                           /* HOMING開始					*/
/*--------------------------------------------------------------------------------------------------*/
		PsnSetRefPoint(&hdl->psnHdl, FALSE);				          /* リファレンス点クリア		*/
		hdl->seqState++;											  /* ﾄﾙｸLimit待ち				*/
/*--------------------------------------------------------------------------------------------------*/
	case 2:                                                           /* ﾄﾙｸLimit待ち				*/
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl,
									 hdl->mtPrm.MtSeqPrm.zret_dir);	  /* 目標位置は無限遠方へ		*/
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* アプローチ速度2			*/
		hdl->tlimCnt = 0;											  /* ﾄﾙｸﾘﾐｯﾄ経過時間ﾘｾｯﾄ		*/
		
		MlibBITFILTER1( hdl->tlimSig[0], hdl->tlimSig[1], (UCHAR)SviGetTLmtSts(hdl->svRsp) );
		if(hdl->tlimSig[0])
		{
			hdl->psnHdl.TlimPerr = PsnGetPERR(&hdl->psnHdl);		  /* ﾄﾙｸﾘﾐｯﾄ発生時の位置偏差保存 */
			hdl->tlimCnt++;											  /* ﾄﾙｸﾘﾐｯﾄ経過時間CntUp		*/
			hdl->seqState++;										  /* 押付完了待ち				*/
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 3:                                                           /* 押付完了待ち				*/
/*--------------------------------------------------------------------------------------------------*/
																	  /* 目標位置は、条件で異なる。 */
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* アプローチ速度2 			*/
		
		MlibBITFILTER1( hdl->tlimSig[0], hdl->tlimSig[1], (UCHAR)SviGetTLmtSts(hdl->svRsp) );
		if(hdl->tlimSig[0])
		{
			if((hdl->mtPrm.MtSeqPrm.tlimTimeCnt1 <= hdl->tlimCnt)
									&&(hdl->tlimCnt < hdl->mtPrm.MtSeqPrm.tlimTimeCnt2))
			{/* 目標位置をAPOS＋αとする */
				tpos = PsnGetAPOS(&hdl->psnHdl) + hdl->psnHdl.TlimPerr;		  /* 目標位置作成				*/
				cmdOut->DposIn = tpos - PsnGetIPOS(&hdl->psnHdl);			  /* 払い出し両作成				*/
				followFlag = TRUE;
			}
			else if(hdl->mtPrm.MtSeqPrm.tlimTimeCnt2 <= hdl->tlimCnt)
			{
				if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
					tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;
				else
					tpos = hdl->mtPrm.MtSeqPrm.zret_dist;			      /*最終走行距離符号逆転*/
					
				tpos = PsnGetAPOS(&hdl->psnHdl) + tpos;				      /* 最終位置セット */
				cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
				hdl->seqState++;									      /* 位置決め完了待ちへ */
			}
			else
			{
				cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl,
										 hdl->mtPrm.MtSeqPrm.zret_dir);	  /* 目標位置は無限遠方へ		*/
			}
			hdl->tlimCnt++;												  /* ﾄﾙｸﾘﾐｯﾄ経過時間CntUp		*/
		}
		else
		{
			cmdOut->DposIn = PsnSetTPOSDist(&hdl->psnHdl,
									 hdl->mtPrm.MtSeqPrm.zret_dir);	  /* 目標位置は無限遠方へ		*/
			if(hdl->mtPrm.MtSeqPrm.tlimTimeCnt1 <= hdl->tlimCnt)
			{/* tlimTimeCnt1以上経過後にﾄﾙｸ制限が解除された場合は、エラーとする 					*/
				IXX_ALARMSET( ALMBITS_ZRETUNCOMPLETE );			 	 /* A.E24 : 原点復帰失敗		*/
				hdl->seqState = 5;
			}
			else
			{
				hdl->seqState = 2;									  /* ﾄﾙｸLimit待ち				*/
			}
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 4:                                                           /* 位置決め完了待ち状態 */
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = PsnGetTPOS(&hdl->psnHdl) - PsnGetIPOS(&hdl->psnHdl);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			          /* アプローチ速度2 */

		if(PsnGetPsetSts(&hdl->psnHdl))
		{                                                             /* 位置決め完了 */
	/*----------------------------------------------------------------------------------------------*/
	/* 暫定: 原点位置座標計算 																		*/
	/*----------------------------------------------------------------------------------------------*/
			IxxSetOriginPos( &Npos, Npos.HomePos );                   /* set origin position */
	/*----------------------------------------------------------------------------------------------*/
			hdl->seqState = 0;							              /* 原点復帰完了状態へ */
		}
		break;
/*--------------------------------------------------------------------------------------------------*/
	case 5:                                                           /* HOLD状態 					*/
/*--------------------------------------------------------------------------------------------------*/
		cmdOut->DposIn = MtgGetDctDistance(&hdl->mtgHdl, 
						MtgGetCspd(&hdl->mtgHdl), cmdOut->Ctrl);
		cmdOut->Tspd = 0;
		hdl->seqState = (hdl->seqState != 4)?2:4;	/* 次周期でもbHold=TRUEだと、再度このCaseに入る	*/
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

	PsnResetIPOS(&hdl->psnHdl);		                          /* IPOSリセット */
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

	PsnResetIPOS(&hdl->psnHdl);		                         /* IPOSリセット */
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
			mtcMakeCmd[MTYPE_HOMING] = mtcMakeHomingRef; /* SigmaSでは、DEC信号のみのHomingは未対応 */
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
//TODO			KPI_ALARMSET( ALM_OPTPRM_ERR );                   /* 暫定: A.04A : ｵﾌﾟｼｮﾝﾊﾟﾗﾒｰﾀ設定異常 */
			break;
	}
/*--------------------------------------------------------------------------------------------------*/
	return;
}
