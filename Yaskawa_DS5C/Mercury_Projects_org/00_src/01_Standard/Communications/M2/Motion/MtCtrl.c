/****************************************************************************/
/*																			*/
/*																			*/
/*		Motion Control	 													*/
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
#include "MtCtrl.h"
#include "MtApi.h"
#include "MLib.h"
/****************************************************************************
 Private functions(�V�[�P���X�����֘A) 
****************************************************************************/
/* �A�����b�`�v������ */
static	VOID mtcControlLatchSeq( MT_HNDL *hdl );
/* ���[�V�����w�ߍ쐬 */
static	VOID mtcCreateInCmd(MT_HNDL* hdl);
/* OT��Ԑ��䏈�� */
static	BOOL mtcControlOTState(MT_HNDL* hdl);
/*  ActiveMode��Ԑ���	*/
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
static	BOOL mtcControlAMState(MT_HNDL* hdl);
#endif
/* ���b�`�̊����`�F�b�N */
static	VOID mtcCheckLatchComp(MT_HNDL* hdl);
/* ���[�V������Ԑ���(Disable���) */
static	LONG mtcDisableState(MT_HNDL* hdl);
/* ���[�V������Ԑ���(WaitPGRdy���) */
static	LONG mtcWaitPgRdyState(MT_HNDL* hdl);
/* ���[�V������Ԑ���(Ready���) */
static	LONG mtcReadyState(MT_HNDL* hdl);
/* ���[�V������Ԑ���(Run���) */
static	LONG mtcRunState(MT_HNDL* hdl);
/* ���[�V������Ԑ���(WaitSvOff���) */
static	LONG mtcWaitSvOffState(MT_HNDL* hdl);
/* ���[�V������Ԑ���(RampStop���) */
static	LONG mtcRampStopState(MT_HNDL* hdl);
/* Private functions(Motion�w�ߍ쐬����) */

/****************************************************************************
 Private functions(Motion�w�ߍ쐬����) 
****************************************************************************/
/* NOP�w�ߍ쐬���� */
static	BOOL mtcMakeNopRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* Followup�w�ߍ쐬���� */
static	BOOL mtcMakeFollowRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* POSING�w�ߍ쐬���� */
static	BOOL mtcMakePosingRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* INTERPOLATE�w�ߍ쐬���� */
static	BOOL mtcMakeInterpolRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* FEED�w�ߍ쐬���� */
static	BOOL mtcMakeFeedRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* HOLD�w�ߍ쐬���� */
static	BOOL mtcMakeHoldRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* EX_POSING�w�ߍ쐬���� */
static	BOOL mtcMakeExPosingRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* HOMING�w�ߍ쐬����(DEC�M���Ȃ�) */
static	BOOL mtcMakeHomingRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* VELCTRL�w�ߍ쐬���� */
static	BOOL mtcMakeVelRef(MT_HNDL* hdl, MG_INOUT* cmdOut);
/* TRQCTRL�w�ߍ쐬���� */
static	BOOL mtcMakeTrqRef(MT_HNDL* hdl, MG_INOUT* cmdOut);

/* motion state machine function table */
typedef LONG (*CTRLFUNC)(MT_HNDL* hdl);
const CTRLFUNC mtcControlState[] = { /* <V715> */
	mtcDisableState,
	mtcWaitPgRdyState,
	mtcReadyState,
	mtcRunState,
	mtcWaitSvOffState,
	mtcRampStopState,
};
/* motion function table */
typedef BOOL (*MAKECMDFUNC)(MT_HNDL* hdl, MG_INOUT* cmdOut);
const MAKECMDFUNC mtcMakeCmd[] = { /* <V715> */
	mtcMakeNopRef,		/* Nop���[�V���� */
	mtcMakePosingRef,	/* Posing */
	mtcMakeExPosingRef,	/* ExPosing */
	mtcMakeHomingRef,	/* Homing */
	mtcMakeFeedRef,		/* Feed */
	mtcMakeHoldRef,		/* Hold */
	mtcMakeInterpolRef,	/* Interpolate */
	mtcMakeVelRef,		/* Velctrl */
	mtcMakeTrqRef,		/* Trqctrl */
	mtcMakeFollowRef,	/* Folloup */
};

/* ���b�`���j�^�ԍ�Table */
const UCHAR LtMonTbl[] = {
	MONSEL_LTPOS_C_L, MONSEL_LTPOS_EXT1_L, MONSEL_LTPOS_EXT2_L, MONSEL_LTPOS_EXT3_L
};

#if 0
/* variable <V715> */
#ifdef	__ghs__
#pragma ghs startzda
#endif
CTRLFUNC mtcControlState[sizeof(_mtcControlState)/sizeof(CTRLFUNC)];
MAKECMDFUNC mtcMakeCmd[sizeof(_mtcMakeCmd)/sizeof(MAKECMDFUNC)];
#ifdef	__ghs__
#pragma ghs endzda
#endif
#endif
/****************************************************************************
****************************************************************************/
/* Public functions (Task�֘A) */
/****************************************************************************
****************************************************************************/

/****************************************************************************
 ����������
****************************************************************************/
VOID MtcInit(MT_HNDL* hdl, SV_OPTION_IF* svop_if)
{
	/* �O���[�o���f�[�^���[���N���A */
	MlibResetByteMemory( hdl, sizeof(MT_HNDL) );

	/* Position Manager������ */
	PsnInit(&hdl->psnHdl, &hdl->mtPrm.PsnPrm);
	
	/* MG������ */
	MtgInit(&hdl->mtgHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.MgPrm);
	
	/* AccFilter������ */
	AcfInit(&hdl->acfHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.AccFilPrm);

	hdl->mtState = MSTAT_DISABLE;
	hdl->otState = OTSTAT_NORMAL;
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )											/*<V717>Add:Start*/
	hdl->amState = AMSTAT_NORMAL;
#endif																		/*<V717>Add:End  */
	hdl->crtMtype = MTYPE_NONE;
	hdl->ctrlMode = MT_NO_CTRLMD;
	hdl->seqState = 0;
	hdl->svCmd = &svop_if->CycCmd;
	hdl->svRsp = &svop_if->CycRsp;
	
/* �T�C�N���b�N�f�[�^������ */
	svop_if->CycCtrl.ExtCmdSel1 = EXTCMD_TRQLMT_P;		/* �����g���N���� */
	svop_if->CycCtrl.ExtCmdSel2 = EXTCMD_TRQLMT_N;		/* �����g���N���� */
	svop_if->CycCtrl.ExtCmdSel3 = 0;					/* ���t�Ȃ� */
	svop_if->CycCtrl.ExtCmdSel4 = 0;					/* ���t�Ȃ� */

	svop_if->CycCmd.SelMon1 = MONSEL_LTPOS_C_L;			/* C�����b�`�ʒu */
	svop_if->CycCmd.SelMon2 = MONSEL_TRQREF;			/* �g���N�w�� */
	svop_if->CycCmd.SelMon3 = MONSEL_FBSPD;				/* FB���x */
	svop_if->CycCmd.SelMon4 = MONSEL_VPOSERR_L;			/* ���z�ʒu�΍� */
	svop_if->CycCmd.SelMon5 = 0;						/* OMN1 */
	svop_if->CycCmd.SelMon6 = 0;						/* OMN2 */
	svop_if->CycCmd.SelMon7 = MONSEL_INSGNL;			/* ���͐M�� */
	svop_if->CycCmd.SelMon8 = MONSEL_ALMWRN_CODE;		/* Alarm Code */

#if 0
	/* Const�e�[�u�����f�[�^RAM�֓W�J *//* <V715> */
	UtilMemCpy((CTRLFUNC *)mtcControlState, (CTRLFUNC *)_mtcControlState, sizeof(mtcControlState));
	UtilMemCpy((MAKECMDFUNC *)mtcMakeCmd, (MAKECMDFUNC *)_mtcMakeCmd, sizeof(mtcMakeCmd));
#endif
}

/****************************************************************************
 ���[�V�����o�͏���
****************************************************************************/
VOID MtcOutputData(MT_HNDL* hdl)
{
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
	BOOL den, denOt, denAm;
#else
	BOOL den, denOt;
#endif

	hdl->ctrlMode = hdl->cmdData.CtrlMode;				/* ���䃂�[�h�Z�b�g */
	hdl->mtState = mtcControlState[hdl->mtState](hdl);	/* ���[�V������Ԑ��� */
	mtcCreateInCmd(hdl);								/* ���[�V�����w�ߍ쐬 */
	denOt = mtcControlOTState(hdl);						/* OT���� */
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
	denAm = mtcControlAMState(hdl);						/* OT���� */
#endif

	den = MtgGenerate(&hdl->mtgHdl, &hdl->outData);		/* ���[�V�����o�� */
#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
	den = den & denOt & denAm;							/* OT/AM��~���́ADen=0�Ƃ��� */
#else
	den = den & denOt;									/* OT��~���́ADen=0�Ƃ��� */
#endif

	hdl->psnHdl.Ipos += hdl->outData.DposOut;				/* IPOS�Z�b�g */

	AcfGenerate( &hdl->acfHdl, &hdl->outData.DposOut,
						hdl->cmdData.AccFilSel, &den );		/* �ʒu�w�߃t�B���^���s */

	hdl->psnHdl.Pos += hdl->outData.DposOut;				/* POS�Z�b�g */
	hdl->psnHdl.lstDpos = hdl->psnHdl.Dpos;					/* DPOS�O��l�X�V */
	hdl->psnHdl.Dpos = hdl->outData.DposOut;				/* DPOS����l�X�V */

	hdl->psnHdl.Den = den;									/* DEN��ԃZ�b�g */

	hdl->svCmd->CmdCtrl.ModSel = (UCHAR)hdl->ctrlMode;		/* ���䃂�[�h�Z�b�g */
	hdl->svCmd->Position += hdl->outData.DposOut;			/* �ʒu�w�߃Z�b�g */
	hdl->svCmd->Speed = hdl->outData.VrefOut;				/* ���x�w�߃Z�b�g */
	hdl->svCmd->Torque = hdl->outData.TrefOut;				/* �g���N�w�߃Z�b�g */
}

/****************************************************************************
 ���[�V�����f�[�^�X�V����
****************************************************************************/
VOID MtcInputData(MT_HNDL* hdl)
{
	LONG fbSpd;
	LONG PosErr;

	PosErr = hdl->svRsp->PosError;
	PsnUpdateFBData( &hdl->psnHdl, hdl->svRsp->FbPosition, PosErr );

	mtcCheckLatchComp(hdl);

	if(hdl->ctrlMode != MT_POS_CTRLMD)
	{
#if	( SVFSEL_ANALOG_FEEDBACK == 1)
		fbSpd = 0;
#else
		if( hdl->otState != OTSTAT_NORMAL )
		{
			fbSpd = 0;
		}
		else
		{
			fbSpd = hdl->psnHdl.Fbspd;
		}
#endif
		MtgSetCspd(&hdl->mtgHdl, fbSpd);
		
		PosErr = hdl->svRsp->Mon4;										/* <S157> */
		if( (hdl->otState == OTSTAT_NORMAL)
			|| ((hdl->svRsp->RspCtrl.RspCtrlBit & MOTOR_MOVING_BITMSK) != FALSE) )
		{ /* OT���o���łȂ� || ���[�^���쒆 */
//			PosErr = PosErr + fbSpd;									/* <S007> */
			PsnResetCmdPosAll( &hdl->psnHdl, PosErr );
			hdl->svCmd->Position = hdl->svRsp->FbPosition + PosErr;
		}
		else
		{
			PsnResetCmdPosAll( &hdl->psnHdl, fbSpd );
			hdl->svCmd->Position = hdl->svRsp->FbPosition + fbSpd;
		}
	}
	
	/* PSET�����NEAR�M�����쐬����B	*//* <S0AE> */
	PsnSetPsetState( &hdl->psnHdl );
}


/****************************************************************************
****************************************************************************/
/* Public functions (����֐�) */
/****************************************************************************
****************************************************************************/

/****************************************************************************
 ���b�`�v�����s
****************************************************************************/
INT32 MtcReqLatch(MT_HNDL* hdl, LONG mode, LONG sgnl)
{
	LONG	ltReq;

	ltReq = hdl->svCmd->CmdLtBit;

	hdl->latch.Mode = mode;
	hdl->latch.Cmp = 0;
	hdl->latch.Req = TRUE;
	hdl->latch.Sgnl = sgnl;

	if( (ltReq >> sgnl) & 0x01 )
	{
		ltReq = ( ltReq & (~(1 << sgnl)) );
	}
	else
	{
		ltReq = ( ltReq | (1 << sgnl) );
	}

	hdl->svCmd->CmdLtBit = ltReq;
	hdl->svCmd->SelMon1 = LtMonTbl[sgnl];
	
	return 0;
}

/****************************************************************************
 ���b�`�v���L�����Z��
****************************************************************************/
VOID MtcCancelLatch(MT_HNDL* hdl, LONG mode)
{
	hdl->latch.Cmp = 0;
	hdl->latch.Req = 0;
	hdl->latch.Mode = mode;

	hdl->latch.SeqReq = 0;
	hdl->latch.SeqCnt = 0;
	hdl->latch.SeqSts.wdCnt = 0;
}

/****************************************************************************
 �A�����b�`�v������
****************************************************************************/
static	VOID mtcControlLatchSeq(MT_HNDL* hdl)
{
	/* ���b�`�V�[�P���X�v���`�F�b�N */
	if(hdl->latch.SeqReq != MTLT_SEQ_REQ)
	{
		return;
	}
	
	/* �V�[�P���X�J�E���^�X�V */
	hdl->latch.SeqCnt++;
	hdl->latch.SeqSts.cnt[1] = (UCHAR)hdl->latch.SeqCnt;
	if(hdl->latch.SeqCnt >= hdl->mtPrm.LtPrm.seqNum)
	{/*�V�[�P���X���� */
		hdl->latch.SeqCnt = 0;
		hdl->latch.SeqSts.cnt[0]++;
		if((hdl->mtPrm.LtPrm.loopCnt != 0) &&
			(hdl->latch.SeqSts.cnt[0] >= hdl->mtPrm.LtPrm.loopCnt))
		{/* �A�����b�`���� */
			hdl->latch.SeqReq = MTLT_SEQ_DONE;
			return;
		}
	}
/* ���b�`�v�����s */
	MtcReqLatch(hdl, hdl->latch.Mode, 
		hdl->mtPrm.LtPrm.ltSgnl[hdl->latch.SeqCnt]);
}


/****************************************************************************
 ���b�`�����`�F�b�N
****************************************************************************/
VOID mtcCheckLatchComp(MT_HNDL* hdl)
{
	LONG req, cmp, ack;
	LONG lt_pos;
	
	if(!hdl->latch.Req) return;	/* ���b�`�v���Ȃ�.�������^�[�� */

	req = hdl->svCmd->CmdLtBit;
	ack = hdl->svRsp->RspLtBit;

	req = ( req >> (hdl->latch.Sgnl) ) & 0x01;
	cmp = ( ack >> (hdl->latch.Sgnl + 4) ) & 0x01;
	ack = ( ack >> (hdl->latch.Sgnl) ) & 0x01;

	if( (req == ack) && (cmp == TRUE)
		&&  (hdl->svRsp->SelMon1 == LtMonTbl[hdl->latch.Sgnl]) )
	{/* ���b�`���� */
		lt_pos = hdl->svRsp->Mon1;						/* ���b�`�ʒu�擾 */
		if((hdl->latch.LtDisable != FALSE)
			|| (PsnMakeLpos(&hdl->psnHdl, lt_pos) == FALSE))
		{/* ���b�`�����A���́A���b�`�̈�O�Ȃ�ă��b�` */
			MtcReqLatch(hdl, hdl->latch.Mode, hdl->latch.Sgnl);/* ���b�`�v�����s */
		}
		else
		{/* ���b�`�L�� && ���b�`�̈�� */
			hdl->latch.Req = FALSE;			/* ���b�`�v��OFF */
			mtcControlLatchSeq(hdl);		/* ���b�`�V�[�P���X�N�� */
			hdl->latch.Cmp = MTLT_CMP_STS;	/* ���b�`����ON */
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
static	LONG mtcDisableState(MT_HNDL* hdl)
{
	BOOL	senCmd;
	LONG	rc;

	rc = MSTAT_DISABLE;
	
/* ���[�V�����w�ߏ����� */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_NO_CTRLMD;
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;
	
	/* ���[�V�����L���`�F�b�N */
	if( hdl->cmdData.MtEenable )
	{
		rc = MSTAT_WAITPG_RDY;
		/* abs��:SensOn�w�ߏ��, incr��:1 */
		senCmd = ( hdl->mtPrm.PsnPrm.absPG != FALSE ) ? hdl->cmdData.SensOn : TRUE;

		if( FALSE != senCmd )
		{
			hdl->svCmd->CmdSeqBit |= SENSOR_ON_BIT;
			if( 0 != (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) )
			{ /* �Z���TON��� */
				hdl->cmdData.SensOn = TRUE;
				hdl->cmdData.Mtype = MTYPE_FOLLOW;
				hdl->ctrlMode = MT_POS_CTRLMD;
				PsnResetApos(&hdl->psnHdl, TRUE);
				rc = MSTAT_READY;
			}
		}
		else
		{
			hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		}
	}
	return rc;
}

/****************************************************************************
 ���[�V������Ԑ���(WaitPGRdy���)
****************************************************************************/
static	LONG mtcWaitPgRdyState( MT_HNDL *hdl )
{
	BOOL	senCmd;
	LONG	rc;

	rc = MSTAT_WAITPG_RDY;

	/* ���[�V�����w�ߏ����� */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_VEL_CTRLMD;
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* ���[�V���������v���������Disble��Ԃ� */
	if( !hdl->cmdData.MtEenable )
	{
		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		return MSTAT_DISABLE;
	}

	/* PGRDY�Ȃ��,Followup=ON�AAPOS�ݒ肵�AReady��Ԃ� */
	/* abs��:SensOn�w�ߏ��, incr��:1 */
	senCmd = ( hdl->mtPrm.PsnPrm.absPG != FALSE ) ? hdl->cmdData.SensOn : TRUE;
	if( FALSE != senCmd )
	{
		hdl->svCmd->CmdSeqBit |= SENSOR_ON_BIT;
		if( 0 != (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) )
		{
			hdl->cmdData.SensOn = TRUE;
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
			hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
			hdl->ctrlMode = MT_POS_CTRLMD;
			PsnResetApos(&hdl->psnHdl, TRUE);
			rc = MSTAT_READY;
		}
	}
	else
	{
		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
	}

	return rc;
}

/****************************************************************************
 ���[�V������Ԑ���(Ready���)
****************************************************************************/
static	LONG mtcReadyState( MT_HNDL *hdl )
{
	LONG	rc;
	BOOL	svOnCmp;

	rc = MSTAT_READY;

	/* ���[�V���������v���������Disble��Ԃ� */
	if( !hdl->cmdData.MtEenable )
	{
		if( !hdl->cmdData.SensOn )
		{
			hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		}
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_NO_CTRLMD;
		return MSTAT_DISABLE;
	}

	/* SensOff�w��OFF �Ȃ��WaitPGRdy��Ԃ� */
	if( !hdl->cmdData.SensOn )
	{
		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_VEL_CTRLMD;
		PsnResetApos(&hdl->psnHdl, FALSE);
		return MSTAT_WAITPG_RDY;
	}

	/* �T�[�{ON�w�ߔ��s*/
	if( hdl->cmdData.ServoOn )
	{
		hdl->svCmd->CmdSeqBit |= (SERVO_ON_BIT | POL_DETECT_BIT);
	}
	else
	{
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
	}

	if(0 == (hdl->svRsp->RspCtrl.RspCtrlBit & OP_ENABLED_BITMSK) )
	{/*	�⏕�@�\�ɂ��^�]��ADISCONNECT�����ɏ�ʂ���T�[�{�I���w�߂����� A.0b0 */ /* <S0EC> */
		if( hdl->ctrlMode == MT_POS_CTRLMD )
		{
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
		}
		else
		{
			hdl->cmdData.Mtype = MTYPE_NONE;
			hdl->ctrlMode = MT_VEL_CTRLMD;
		}
	}
	else
	{
		/* �t�H���[�A�b�v & �T�[�{ON�w�ߔ��s*/
		if( hdl->ctrlMode == MT_POS_CTRLMD )
		{
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
		}

		/* �T�[�{ON�����Ȃ��RUN��Ԃ� */
		svOnCmp = hdl->cmdData.ServoOn & (hdl->svRsp->RspSeqBit >> SERVO_ON_COMP_BITNO);
		if( svOnCmp )
		{
			return MSTAT_RUN;
		}
	}

	return rc;
}


/****************************************************************************
 ���[�V������Ԑ���(Run���)
****************************************************************************/
static	LONG mtcRunState( MT_HNDL *hdl )
{
	LONG	rc;

	rc = MSTAT_RUN;

	/* �T�[�{OFF��ԂȂ��Ready��Ԃ� */
	if( 0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) )
	{ /* ScanC����������� && SVON�����̏ꍇ */
		hdl->cmdData.ServoOn = FALSE;
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
		if( hdl->ctrlMode != MT_TRQ_CTRLMD )
		{
			hdl->cmdData.Mtype = MTYPE_NONE;				/* �w�ߒ�~*/
		}
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;
		return MSTAT_READY;
	}

	/* �T�[�{On�w��=OFF�Ȃ�΃T�[�{OFF�҂���Ԃ� */
	if( !hdl->cmdData.ServoOn )
	{
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;
		if( hdl->mtPrm.MtSeqPrm.rampTimeOut == 0 )
		{ /* RampStop�@�\���� */
			hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
			if( hdl->ctrlMode != MT_TRQ_CTRLMD )
			{
				hdl->cmdData.Mtype = MTYPE_NONE;			/* �w�ߒ�~*/
			}
			return MSTAT_WAIT_SVOFF;
		}
		else
		{ /* RampStop�@�\�L�� */
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


/****************************************************************************
 ���[�V������Ԑ���(WaitSvOff���)
****************************************************************************/
static	LONG mtcWaitSvOffState( MT_HNDL *hdl )
{
	LONG	rc;

	rc = MSTAT_WAIT_SVOFF;

	/* �T�[�{OFF�w�ߔ��s */
	hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);

	/* �T�[�{OFF����܂Ŏw�ߒ�~ */
	if( hdl->ctrlMode != MT_TRQ_CTRLMD )
	{
		hdl->cmdData.Mtype = MTYPE_NONE;
	}
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* �T�[�{OFF��ԂȂ��Ready��Ԃ� */
	if( 0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) )
	{
		return MSTAT_READY;
	}

	return rc;
}


/****************************************************************************
 ���[�V������Ԑ���(RampStop���)
****************************************************************************/
static	LONG mtcRampStopState( MT_HNDL *hdl )
{
	BOOL	den;
	LONG	rc;

	rc = MSTAT_RAMP_STOP;
	den = hdl->psnHdl.Den;

	/* ������~�w�߃Z�b�g */
	hdl->cmdData.Mtype = MTYPE_HOLD;
	hdl->ctrlMode = MT_POS_CTRLMD;
	hdl->cmdData.HoldMod = 2;
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* �T�[�{OFF��ԂȂ��Ready��Ԃ� */
	if( 0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) )
	{
		hdl->cmdData.ServoOn = FALSE;
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
		return MSTAT_READY;
	}

	/* ������~�A���́A�^�C���A�E�g������WaitSvOff��Ԃ� */
	++hdl->timer;
	if( (hdl->timer > hdl->mtPrm.MtSeqPrm.rampTimeOut)			/* �^�C���A�E�g */
		|| (den && ( MlibLABS(hdl->svRsp->Mon3) < MOTOR_STP_SPD)) )	/* den=1&&���[�^��~ */
	{
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
		return MSTAT_WAIT_SVOFF;
	}
	return rc;
}



/****************************************************************************
 OT��Ԑ���(OT��~���쒆 rc = 0, ����ȊO rc = 1)
****************************************************************************/
static	BOOL mtcControlOTState( MT_HNDL *hdl )
{
	BOOL p_ot, n_ot;
	LONG dpos;
	LONG remdist;
	LONG wk;

	/* APOS�\�t�g���~�b�g�`�F�b�N */
	PsnCheckSoftLimitFB( &hdl->psnHdl, &p_ot, &n_ot );

	/* OT�M���`�F�b�N */
	p_ot |= ((hdl->svRsp->RspSigBit >> P_OT_SIG_BITNO) & 1);
	n_ot |= ((hdl->svRsp->RspSigBit >> N_OT_SIG_BITNO) & 1);

	/* OT�w�߃Z�b�g */
	if( p_ot )
	{
		hdl->svCmd->CmdSeqBit |= POSITIVE_OT_BIT;
	}
	else
	{
		hdl->svCmd->CmdSeqBit &= ~(POSITIVE_OT_BIT);
	}

	if( n_ot )
	{
		hdl->svCmd->CmdSeqBit |= NEGATIVE_OT_BIT;
	}
	else
	{
		hdl->svCmd->CmdSeqBit &= ~(NEGATIVE_OT_BIT);
	}

	/* OT���������`�F�b�N */
	if( ((p_ot | n_ot) == 0)											/* OT�M������ */
		|| ((hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) == FALSE)	/* �T�[�{OFF��� */
		|| (hdl->cmdData.CtrlMode != MT_POS_CTRLMD) ) 					/* ���x�E�g���N���� */
	{ /* OT���� */
		hdl->otState = OTSTAT_NORMAL;
		return TRUE;
	}

	/* �����߂������`�F�b�N */
	dpos = hdl->psnHdl.Apos - hdl->psnHdl.Pos;
	/* �I�[�o�[�t���[����\�������邽�ߖO�a���Z����(�w�߃t�B���^�̗��܂蕪��31bit�ȉ�) */
	wk = hdl->psnHdl.Ipos - hdl->psnHdl.Pos;
/* 2011.04.04 Y.Oka <V776> */
#if 0
	wk2 = hdl->outData.DposIn + wk1;
	if( (hdl->outData.DposIn > 0) && (wk1 > 0) && (wk2 <= 0) )
	{
		wk2 = 0x7fffffff;
	}
	else if( (hdl->outData.DposIn < 0) && (wk1 < 0) && (wk2 >= 0) )
	{
		wk2 = 0x80000000;
	}
	if( hdl->otState != OTSTAT_STOP )
	{
		dpos = wk2;
	}
	else
	{
		dpos = ( wk2 - dpos );
	}
#endif
	wk = MlibSatAdd32( hdl->outData.DposIn, wk );
	dpos = (hdl->otState != OTSTAT_STOP) ? wk : MlibSatAdd32( wk, -dpos );
/* 2011.04.04 Y.Oka <V776> */

	if( (p_ot && (!n_ot) && (dpos < 0))
		|| (n_ot && (!p_ot) && (dpos > 0)) )
	{ /* �����߂����� */
/* 2011.04.04 Y.Oka <V776> */
//		if(hdl->outData.Tspd != 0)
		if((hdl->outData.Tspd != 0) || (hdl->otState == OTSTAT_RETRACTION) )
/* 2011.04.04 Y.Oka <V776> */
		{ /* �ڕW���x��0 or �����߂��� */
			/* 2011.04.04 Y.Oka <V679> */
//			hdl->otState = OTSTAT_WAITNORMAL;
			hdl->otState = OTSTAT_RETRACTION;
			/* 2011.04.04 Y.Oka <V679> */
			return TRUE;
		}
	}

	/* 2011.04.04 Y.Oka <V679> */
	/* �����߂��I�������`�F�b�N */
	if( (hdl->otState == OTSTAT_RETRACTION) && (dpos == 0) )
	{
		remdist = hdl->psnHdl.Tpos - hdl->psnHdl.Apos;
		if( (p_ot && (!n_ot) && (remdist < 0))
			|| (n_ot && (!p_ot) && (remdist > 0)) )
		{
			return TRUE;
		}
	}

	/* OT��Ԑ��� */
	hdl->ctrlMode = MT_VEL_CTRLMD;
	hdl->cmdData.Mtype = hdl->crtMtype = MTYPE_NONE;	/* Motion Type Clear */
	mtcMakeNopRef(hdl, &hdl->outData);					/* �w��0 */
	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;				/* �t�B���^�N���A */
	AcfClear( &hdl->acfHdl );							/* �t�B���^�N���A */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;					/* IPOS���Z�b�g */
	hdl->psnHdl.Tpos = hdl->outData.DposIn + hdl->psnHdl.Ipos;
	if( MlibLABS(hdl->svRsp->Mon3) > MOTOR_STP_SPD )
	{
		/* 2011.04.04 Y.Oka <V679> */
//		if( hdl->otState != OTSTAT_WAITNORMAL )
		if( (hdl->otState != OTSTAT_WAITNORMAL) && (hdl->otState != OTSTAT_RETRACTION) )
		{
			hdl->otState = OTSTAT_STOP;					/* OTSTAT_WAITNORMAL���͏�Ԃ��̂܂� */
		}
		return FALSE;
	}
	hdl->otState = OTSTAT_WAITNORMAL;

	return TRUE;
}



#if 0
//#if ( CCSW_SFTID == SFTID_EXIST )
/****************************************************************************
 ActiveMode��Ԑ���(���쒆 rc = 0, ����ȊO rc = 1)
****************************************************************************/
/*<V739>SafetyModule���މ��P*/
static BOOL mtcControlAMState(MT_HNDL* hdl)
{
	BOOL am_mod;
	
	
/* AM��ԃ`�F�b�N		*/
	am_mod = SviGetAmSts();
	if( (am_mod == 0) || (0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK)) ) /* ��è��Ӱ��=INACT or SVOFF *//*<V730>Add*/
	{/* AM���� */
		hdl->amState = AMSTAT_NORMAL;
		return TRUE;
	}
/* AM��Ԑ���			*/
	hdl->ctrlMode			= (hdl->cmdData.CtrlMode == MT_TRQ_CTRLMD) ?
												MT_TRQ_CTRLMD : MT_VEL_CTRLMD;	   /*<V730>Mod*/
	hdl->cmdData.Mtype		= hdl->crtMtype = MTYPE_NONE;	/* Motion Type Clear *//*<V730>Mod*/
	mtcMakeNopRef(hdl, &hdl->outData);			/* �w��0 */
	hdl->cmdData.AccFilSel	= ACF_SW_NOFIL;		/* �t�B���^�N���A */
	AcfClear(&hdl->acfHdl);						/* �t�B���^�N���A */
	PsnResetIPOS(&hdl->psnHdl);					/* IPOS���Z�b�g */
	PsnSetTPOSNoLimit(&hdl->psnHdl, hdl->outData.DposIn);
	if(ABS(hdl->svRsp->Mon3) > MOTOR_STP_SPD)
	{
		hdl->amState = AMSTAT_STOP;
		return FALSE;
	}
	else
	{
		hdl->amState = AMSTAT_WAITNORMAL;
		return TRUE;
	}
}
#endif																		/*<V717>Add:End  */


/****************************************************************************
****************************************************************************/
/* Private functions(Motion�w�ߍ쐬����) */
/****************************************************************************
****************************************************************************/

static	VOID mtcCreateInCmd( MT_HNDL *hdl )
{
	LONG	rc;

	/* ���[�V�����؂�ւ�聨���[�V�������X�^�[�g */
	if( hdl->cmdData.Mtype != hdl->crtMtype )
	{
		hdl->seqState = 0;					/* Motion �V�[�P���X��ԃN���A */
	}

#ifdef PRM_BANK_ENABLE
	/* ������PRM�؂�ւ� */
	if( hdl->mtPrm.Bnk.bankNum )
	{ /* �o���N�L��(�o���NPRM�Z�b�g) */
		if( hdl->psnHdl.Den )
		{ /* �o���NChange */
			hdl->mtPrm.Bnk.bankSel = hdl->mtPrm.Bnk.cmdSel;
		}

		hdl->acfHdl.Prm = &( hdl->mtPrm.Bnk.acfPrm[hdl->mtPrm.Bnk.bankSel] );
		if( hdl->cmdData.CmdRateOn )
		{ /* �������w�ߗL��(�w��PRM�Z�b�g)<Rev2.01> */
			hdl->mtgHdl.prm = &hdl->mtPrm.CmdRate;
		}
		else
		{ /* �������w�ߖ���(�o���NPRM�Z�b�g) */
			hdl->mtgHdl.prm = &( hdl->mtPrm.Bnk.mgPrm[hdl->mtPrm.Bnk.bankSel] );
		}
	}
	else
#endif //PRM_BANK_ENABLE
	{ /* �o���N���� */
		hdl->acfHdl.Prm = &( hdl->mtPrm.AccFilPrm );
		if( hdl->cmdData.CmdRateOn )
		{ /* �������w�ߗL��(�w��PRM�Z�b�g)<Rev2.01> */
			hdl->mtgHdl.prm = &hdl->mtPrm.CmdRate;
		}
		else
		{ /* �������w�ߖ���(�f�t�H���gPRM�Z�b�g) */
			hdl->mtgHdl.prm = &hdl->mtPrm.MgPrm;
		}
	}

	/* ���[�V�����w�ߍ쐬���� */
	rc = mtcMakeCmd[hdl->cmdData.Mtype]( hdl, &hdl->outData );
	hdl->crtMtype = hdl->cmdData.Mtype; 

	if( rc == TRUE )
	{
		hdl->cmdData.Mtype = MTYPE_NONE;	/* �j���[�g������*/
	}
	hdl->cmdData.CmdRstReq = FALSE;
}


/****************************************************************************
 NOP�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeNopRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MNONE;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return TRUE;
}


/****************************************************************************
 Followup�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeFollowRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	BOOL rc;

	rc = FALSE;

	cmdOut->Mtype = MFOLLOW;
	cmdOut->DposIn = hdl->psnHdl.Apos - hdl->psnHdl.Pos;
	if( cmdOut->DposIn < (-FOLLOWUP_MAX_REF) )
	{
		cmdOut->DposIn = -FOLLOWUP_MAX_REF;	/* �N�����v*/
	}
	else if( cmdOut->DposIn > FOLLOWUP_MAX_REF )
	{
		cmdOut->DposIn = FOLLOWUP_MAX_REF;	/* �N�����v*/
	}
	else
	{
		rc = TRUE;
	}

	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* �t�B���^�N���A */
	AcfClear(&hdl->acfHdl);					/* �t�B���^�N���A */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOS���Z�b�g */
	hdl->psnHdl.Tpos = cmdOut->DposIn + hdl->psnHdl.Ipos;
	return rc;
}


/****************************************************************************
 POSING�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakePosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	if( hdl->cmdData.CmdRstReq )
	{
		PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		cmdOut->Tspd = hdl->cmdData.TSpeed;
	}

	cmdOut->Mtype = MPOSING;
	cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 INTERPOLATE�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeInterpolRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	if( hdl->cmdData.CmdRstReq )
	{
		cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		cmdOut->Vref = hdl->cmdData.Velocity;
		cmdOut->Tref = hdl->cmdData.Trq;
		hdl->segCnt = hdl->mtPrm.BasePrm.seg_num;
	}
	else if( hdl->segCnt == 0 )
	{ /* �c�����o����=0�Ȃ�I�� */
		return mtcMakeNopRef( hdl, cmdOut );
	}
	hdl->segCnt--;

	cmdOut->Mtype = MINTERPOLATE;
	cmdOut->Tspd = cmdOut->DposIn;

	return FALSE;
}


/****************************************************************************
 FEED�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	if( hdl->cmdData.TSpeed < 0 )
	{ /* ������ */
		cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, -DISTANCE_MAX );
		cmdOut->Tspd = -hdl->cmdData.TSpeed;
	}
	else
	{ /* ������ */
		cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, DISTANCE_MAX );
		cmdOut->Tspd =  hdl->cmdData.TSpeed;
	}

	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 HOLD�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeHoldRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG dpos;

	if( hdl->cmdData.HoldMod == 1 )
	{ /* �}����~ */
		dpos = 0;
		cmdOut->Mtype = MNONE;
	}
	else if( hdl->cmdData.HoldMod == 2 )
	{ /* ������~2 */
		cmdOut->Mtype = MHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL2;
		dpos = MtgGetDctDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
	}
	else
	{ /* ������~1 */
		cmdOut->Mtype = MHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL1;
		dpos = MtgGetDctDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
	}
	cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, dpos );
	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}



/****************************************************************************
 EX_POSING�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeExPosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;

	if( hdl->cmdData.CmdRstReq )
	{
		hdl->seqState = 1;	/* ���Z�b�g���́A�ŏ����� */
	}

	if( hdl->seqState == 0 )
	{ /* ���b�`������� */
		cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	}
	else if( hdl->seqState == 1 )
	{ /* EX_POSING�J�n */
		MtcCancelLatch( hdl, EXPOSING_LTMD );					/* ���b�`�L�����Z�� */
		MtcReqLatch( hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl );/* ���b�`�v�����s */
		
		cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		hdl->seqState++;
	}
	else if( hdl->seqState == 2 )
	{ /* ���b�`�҂���� */
		if( hdl->latch.Cmp && (hdl->latch.Mode == EXPOSING_LTMD) )
		{ /* ���b�`���� */
			tpos = hdl->psnHdl.Lpos + hdl->mtPrm.MtSeqPrm.ex_posing_dist;
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			hdl->seqState = 0;
		}
		else
		{ /* ���b�`���� */
			cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
			if( cmdOut->DposIn == 0 )
			{ /* �ʒu���ߊ����Ȃ�I�� */
				hdl->seqState = 0;
			}
		}
	}

	cmdOut->Mtype = MPOSING;
	cmdOut->Tspd = hdl->cmdData.TSpeed;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}

/****************************************************************************
 HOMING�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeHomingRef(MT_HNDL* hdl, MG_INOUT* cmdOut)
{
	LONG tpos;
	LONG pos_err;
	PSN_HNDL *psn_hdl_ptr;

	psn_hdl_ptr = &hdl->psnHdl;
	
	switch(hdl->seqState)
	{
	case 0:/* ���_���A������� */
		if(!hdl->cmdData.CmdRstReq)
		{/* ���Z�b�g�v�����Ȃ���΁A�w��=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch(hdl, ZRET_LTMD);				/* ���b�`�L�����Z�� */
		hdl->seqState = 1;
	case 1:/* HOMING�J�n(DEC=ON�҂����) */
		psn_hdl_ptr->RefEnable = FALSE;						/* ���t�@�����X�_�N���A */
		cmdOut->DposIn = PsnSetTPOSDist(psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->cmdData.TSpeed;					/* TPOS,TSPEED�Z�b�g */

		if( 0 != (hdl->svRsp->RspSigBit & DEC_SIG_BITMSK))
		{/* DEC=ON */
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* �A�v���[�`���x1�� */
			hdl->seqState = 2;								/* DEC=OFF�҂��� */
		}
		break;
	case 2:/* DEC=OFF�҂���� */
		cmdOut->DposIn = PsnSetTPOSDist(psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;	/* TPOS,TSPEED=�A�v���[�`���x1�Z�b�g */

		if( 0 == (hdl->svRsp->RspSigBit & DEC_SIG_BITMSK) )
		{/* DEC=OFF */
			MtcReqLatch(hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl);	/* ���b�`�v�����s */
			hdl->seqState = 3;										/* ���b�`�����҂��� */
		}
		break;
	case 3:/* ���b�`�����҂���� */
		if(hdl->latch.Cmp && (hdl->latch.Mode == ZRET_LTMD))
		{/* ���b�`���� */
			if(hdl->mtPrm.MtSeqPrm.zret_dir < 0)
			{
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;			/*�ŏI���s���������t�]*/
			}
			else
			{
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;
			}

			tpos = psn_hdl_ptr->Lpos + tpos;				/* �ŏI�ʒu�Z�b�g */
			cmdOut->DposIn = PsnSetTPOS(psn_hdl_ptr, tpos);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			/* �A�v���[�`���x2�� */
			hdl->seqState = 4;									/* �ʒu���ߊ����҂��� */
		}
		else
		{/* ���b�`���� */
			cmdOut->DposIn = PsnSetTPOSDist(psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir);
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;				/* �A�v���[�`���x1 */
		}
		break;
	case 4:/* �ʒu���ߊ����҂���� */
		cmdOut->DposIn = psn_hdl_ptr->Tpos - psn_hdl_ptr->Ipos;
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			/* �A�v���[�`���x2 */

		pos_err = psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos;
		if( (psn_hdl_ptr->Den == TRUE) && ((ULONG)MlibLABS(pos_err) <= psn_hdl_ptr->Prm->psetWidth) )
		{/* �ʒu���ߊ��� =  [DEN=1 && (Tpos-Apos)<=�ʒu���ߊ�����] */
			psn_hdl_ptr->RefEnable = TRUE;				/* ���t�@�����X�_�L�� */
			PsnSetCordinate(&hdl->psnHdl, 0, 0);		/* ���W�n0���Z�b�g */
			MtcCancelLatch(hdl, NONE_LTMD);				/* ���b�`�L�����Z�� */
			hdl->seqState = 0;							/* ���_���A������Ԃ� */
		}
		break;
	}
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 VELCTRL�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeVelRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MVELCTRL;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* �t�B���^�N���A */
	AcfClear( &hdl->acfHdl );				/* �t�B���^�N���A */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOS���Z�b�g */
	hdl->psnHdl.Tpos = hdl->psnHdl.Ipos;
	return FALSE;
}


/****************************************************************************
 TRQCTRL�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeTrqRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MTRQCTRL;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* �t�B���^�N���A */
	AcfClear( &hdl->acfHdl );				/* �t�B���^�N���A */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOS���Z�b�g */
	hdl->psnHdl.Tpos = hdl->psnHdl.Ipos;

	return FALSE;
}


