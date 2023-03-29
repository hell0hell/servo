/****************************************************************************
Description	: MECHATROLINK Motion Control Manager
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
#include "MtCtrl.h"
#include "MtApi.h"
#include "MLib.h"



/****************************************************************************
 Private functions(�V�[�P���X�����֘A) 
****************************************************************************/
/* �A�����b�`�v������ */
static	VOID mtcControlLatchSeq( MT_HNDL *hdl, LONG i );
/* ���[�V�����w�ߍ쐬 */
static	VOID mtcCreateInCmd( MT_HNDL *hdl );
/* OT��Ԑ��䏈�� */
static	BOOL mtcControlOTState( MT_HNDL *hdl );
/* �e���b�`�̊����`�F�b�N */
static	VOID mtcCheckEachLatchComp( MT_HNDL *hdl, LONG i );
/* ���[�V������Ԑ���(Disable���) */
static	LONG mtcDisableState( MT_HNDL *hdl );
/* ���[�V������Ԑ���(WaitPGRdy���) */
static	LONG mtcWaitPgRdyState( MT_HNDL *hdl );
/* ���[�V������Ԑ���(Ready���) */
static	LONG mtcReadyState( MT_HNDL *hdl );
/* ���[�V������Ԑ���(Run���) */
static	LONG mtcRunState( MT_HNDL *hdl );
/* ���[�V������Ԑ���(WaitSvOff���) */
static	LONG mtcWaitSvOffState( MT_HNDL *hdl );
/* ���[�V������Ԑ���(RampStop���) */
static	LONG mtcRampStopState( MT_HNDL *hdl );
/* �����G���R�[�_�ʒu�i64bit���j�擾�֐� */
static	INT32 mtcGetInitPgPos( MT_HNDL* hdl );

/****************************************************************************
 Private functions(Motion�w�ߍ쐬����) 
****************************************************************************/
/* NOP�w�ߍ쐬���� */
static	BOOL mtcMakeNopRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* Followup�w�ߍ쐬���� */
static	BOOL mtcMakeFollowRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* POSING�w�ߍ쐬���� */
static	BOOL mtcMakePosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* SPOSING�w�ߍ쐬���� */
static	BOOL mtcMakeSposingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* INTERPOLATE�w�ߍ쐬���� */
static	BOOL mtcMakeInterpolRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* FEED�w�ߍ쐬���� */
static	BOOL mtcMakeFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* HOLD�w�ߍ쐬���� */
static	BOOL mtcMakeHoldRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* EX_FEED�w�ߍ쐬���� */
static	BOOL mtcMakeExFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* EX_POSING�w�ߍ쐬���� */
static	BOOL mtcMakeExPosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* HOMING�w�ߍ쐬����(DEC�M���Ȃ�) */
static	BOOL mtcMakeHomingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* HOMING�w�ߍ쐬����(DEC�M������) */
static	BOOL mtcMakeHomingRefWithDec( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* VELCTRL�w�ߍ쐬���� */
static	BOOL mtcMakeVelRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* VELCTRL�w�ߍ쐬����(�������x�w�肠��) */
static	BOOL mtcMakeVelWithAccrRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* TRQCTRL�w�ߍ쐬���� */
static	BOOL mtcMakeTrqRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* SHOLD�w�ߍ쐬���� */
static	BOOL mtcMakeSholdRef( MT_HNDL *hdl, MG_INOUT *cmdOut );


/* motion state machine function table */
typedef LONG (*CTRLFUNC)(MT_HNDL *hdl);
const CTRLFUNC mtcControlState[] = {
	mtcDisableState,
	mtcWaitPgRdyState,
	mtcReadyState,
	mtcRunState,
	mtcWaitSvOffState,
	mtcRampStopState,
};

/* motion function table */
typedef BOOL (*MAKECMDFUNC)(MT_HNDL *hdl, MG_INOUT *cmdOut);
const MAKECMDFUNC mtcMakeCmd[] = {
	mtcMakeNopRef,				/* Nop���[�V���� */
	mtcMakePosingRef,			/* Posing */
	mtcMakeExPosingRef,			/* ExPosing */
	mtcMakeHomingRef,			/* Homing */
	mtcMakeHomingRefWithDec,	/* Homing with Dec */
	mtcMakeFeedRef,				/* Feed */
	mtcMakeExFeedRef,			/* ExFeed */
	mtcMakeHoldRef,				/* Hold */
	mtcMakeInterpolRef,			/* Interpolate */
	mtcMakeVelRef,				/* Velctrl */
	mtcMakeVelWithAccrRef,		/* Velctrl with Accr */
	mtcMakeTrqRef,				/* Trqctrl */
	mtcMakeFollowRef,			/* Folloup */
	mtcMakeSposingRef,			/* Sposing */
	mtcMakeSholdRef,			/* Shold */
};

/* ���b�`���j�^�ԍ�Table */
const UCHAR LtMonTbl[] = {
	MONSEL_LTPOS_C_L, MONSEL_LTPOS_EXT1_L, MONSEL_LTPOS_EXT2_L, MONSEL_LTPOS_EXT3_L
};


/****************************************************************************
****************************************************************************/
/* Public functions (Task�֘A) */
/****************************************************************************
****************************************************************************/
/****************************************************************************
 ����������
****************************************************************************/
VOID MtcInit( MT_HNDL *hdl, SV_OPTION_IF *svop_if )
{
	MlibResetByteMemory( hdl, sizeof(MT_HNDL) );

	/* Position Manager������ */
	PsnInit( &hdl->psnHdl, &hdl->mtPrm.PsnPrm );

	/* MG������ */
	MtgInit( &hdl->mtgHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.MgPrm );

	/* AccFilter������ */
	AcfInit( &hdl->acfHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.AccFilPrm );

	hdl->mtState = MSTAT_DISABLE;
	hdl->otState = OTSTAT_NORMAL;
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

//	svop_if->CycCmd.SelMon7 = MONSEL_POSERR_L;			/* �ʒu�΍� */	/* <S0AC> */

/* <S1C0>�FStart */
#if CSW_BOARD_TYPE == SGD7S_TYPE
	svop_if->CycCmd.SelMon7 = MONSEL_INSGNL;			/* ���͐M�� */
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
	svop_if->CycCmd.SelMon7 = MONSEL_INSGNL_2AXIS;		/* ���͐M�� */
#endif /* f CSW_BOARD_TYPE == SGD7S_TYPE */
/* <S1C0>�FEnd */

	svop_if->CycCmd.SelMon8 = MONSEL_LTPOS_EXT1_L;		/* EXT1���b�`�ʒu */
}


/****************************************************************************
 ���[�V�����o�͏���
****************************************************************************/
VOID MtcOutputData( MT_HNDL *hdl )
{
	BOOL den, denOt;

	hdl->ctrlMode = hdl->cmdData.CtrlMode;					/* ���䃂�[�h�Z�b�g */
	hdl->mtState = mtcControlState[hdl->mtState]( hdl );	/* ���[�V������Ԑ��� */
	mtcCreateInCmd( hdl );									/* ���[�V�����w�ߍ쐬 */
	denOt = mtcControlOTState( hdl );						/* OT���� */


	den = MtgGenerate(&hdl->mtgHdl, &hdl->outData);			/* ���[�V�����o�� */

	den = den & denOt;										/* OT��~���́ADen=0�Ƃ��� */
	hdl->outData.CtrlModeLst = hdl->ctrlMode;				/* �O�񐧌䃂�[�h�X�V */

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
VOID MtcInputData( MT_HNDL *hdl )
{
	LONG fbSpd;
	LONG PosErr;

	PosErr = hdl->svRsp->PosError;
	PsnUpdateFBData( &hdl->psnHdl, hdl->svRsp->FbPosition, PosErr );

	/* ���b�`1 */
	if( (hdl->latch[0].Req) && (hdl->svRsp->SelMon1 == LtMonTbl[hdl->latch[0].Sgnl]) )
	{
		mtcCheckEachLatchComp( hdl, 0 );
	}
	/* ���b�`2 */
	if( (hdl->latch[1].Req) && (hdl->svRsp->SelMon8 == LtMonTbl[hdl->latch[1].Sgnl]) )
	{
		mtcCheckEachLatchComp( hdl, 1 );
	}

	if( hdl->ctrlMode != MT_POS_CTRLMD )
	{
		if( hdl->otState != OTSTAT_NORMAL )
		{
			fbSpd = 0;
		}
		else
		{
			fbSpd = hdl->psnHdl.Fbspd;
		}
		MtgSetCspd( &hdl->mtgHdl, fbSpd );

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
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
INT32 MtcReqLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, LONG i )
{
	LONG	ltReq;

	ltReq = hdl->svCmd->CmdLtBit;

	/* ���b�`�M���̊��`�F�b�N */
	if( hdl->ltSignlUse[sgnl] == FALSE )
	{
		hdl->ltSignlUse[sgnl] = TRUE;
	}
	else
	{
		return MCI_LTSGNL_CNFLCT;
	}

	/* ���b�`�ԍ��`�F�b�N */
	if( i > 1 )
	{
		return MCI_LTNO_ERR;
	}

	hdl->latch[i].Mode = mode;
	hdl->latch[i].Cmp = 0;
	hdl->latch[i].Req = TRUE;
	hdl->latch[i].Sgnl = sgnl;

	if( (ltReq >> sgnl) & 0x01 )
	{
		ltReq = ( ltReq & (~(1 << sgnl)) );
	}
	else
	{
		ltReq = ( ltReq | (1 << sgnl) );
	}

	hdl->svCmd->CmdLtBit = ltReq;

	if( i==0 )
	{
		hdl->svCmd->SelMon1 = LtMonTbl[sgnl];
	}
	else if( i==1 )
	{
		hdl->svCmd->SelMon8 = LtMonTbl[sgnl];
	}

	return 0;
}


/****************************************************************************
 ���b�`�v���L�����Z��
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
VOID MtcCancelLatch( MT_HNDL *hdl, LONG mode, LONG i )
{
	hdl->latch[i].Cmp = 0;
	hdl->latch[i].Req = FALSE;
	hdl->latch[i].Mode = mode;

	hdl->latch[i].SeqReq = 0;
	hdl->latch[i].SeqCnt = 0;
	hdl->latch[i].SeqSts.wdCnt = 0;

	hdl->ltSignlUse[hdl->latch[i].Sgnl] = FALSE;
}


/****************************************************************************
 �v���t�@�C���^�C�v�Ɉˑ����郂�[�V�������̐ݒ�(Motion Control)
 
 �ݒ���F
 �E���_���A�ŏI���s�����̕���
 �E�R�}���h�؂�ւ�莞��Zret�R�}���h�̃��b�`������L�����Z�����邩�H
 �����Ftype  �v���t�@�C���^�C�v
****************************************************************************/
INT32 MtcSetMtInfoByProfileType( MT_HNDL *hdl, LONG type )
{
	if( type == M2_PROFILE )
	{
		hdl->mtPrm.ProfPrm.zretDistDir = ZRET_DIST_DIR_IS_RELATIVE;
		hdl->mtPrm.ProfPrm.zretLtCancel = ZRET_LATCH_IS_NOCANCEL;
		hdl->mtPrm.ProfPrm.initPgPos64Enable = FALSE;
	}
	else if( type == M3STD_PROFILE )
	{
		hdl->mtPrm.ProfPrm.zretDistDir = ZRET_DIST_DIR_IS_ABSOLUTE;
		hdl->mtPrm.ProfPrm.zretLtCancel = ZRET_LATCH_IS_CANCEL;
		hdl->mtPrm.ProfPrm.initPgPos64Enable = TRUE;
	}
	else
	{
		hdl->mtPrm.ProfPrm.zretDistDir = ZRET_DIST_DIR_IS_ABSOLUTE;
		hdl->mtPrm.ProfPrm.zretLtCancel = ZRET_LATCH_IS_CANCEL;
		hdl->mtPrm.ProfPrm.initPgPos64Enable = TRUE;
	}

	return 0;
}


/****************************************************************************
 �A�����b�`�v������
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
static	VOID mtcControlLatchSeq( MT_HNDL *hdl, LONG i )
{
	/* ���b�`�V�[�P���X�v���`�F�b�N */
	if( hdl->latch[i].SeqReq != MTLT_SEQ_REQ )
	{
		return;
	}

	/* �V�[�P���X�J�E���^�X�V */
	hdl->latch[i].SeqCnt++;
	hdl->latch[i].SeqSts.cnt[1] = (UCHAR)hdl->latch[i].SeqCnt;
	if( hdl->latch[i].SeqCnt >= hdl->mtPrm.LtPrm.seqNum )
	{ /*�V�[�P���X���� */
		hdl->latch[i].SeqCnt = 0;
		hdl->latch[i].SeqSts.cnt[0]++;
		if( (hdl->mtPrm.LtPrm.loopCnt != 0) &&
			(hdl->latch[i].SeqSts.cnt[0] >= hdl->mtPrm.LtPrm.loopCnt) )
		{ /* �A�����b�`���� */
			hdl->latch[i].SeqReq = MTLT_SEQ_DONE;
			return;
		}
	}
	/* ���b�`�v�����s */
	MtcReqLatch( hdl, hdl->latch[i].Mode, 
					hdl->mtPrm.LtPrm.ltSgnl[hdl->latch[i].SeqCnt], i );
}


/****************************************************************************
 �e���b�`�̊����`�F�b�N
 i�F���b�`�ԍ� �ii<= LT_NUM �ł��邱��)
****************************************************************************/
static	VOID mtcCheckEachLatchComp( MT_HNDL *hdl, LONG i )
{
	LONG req, cmp, ack;
	LONG lt_pos;

	req = hdl->svCmd->CmdLtBit;
	ack = hdl->svRsp->RspLtBit;

	req = ( req >> (hdl->latch[i].Sgnl) ) & 0x01;
	cmp = ( ack >> (hdl->latch[i].Sgnl + 4) ) & 0x01;
	ack = ( ack >> (hdl->latch[i].Sgnl) ) & 0x01;

	if( (req == ack) && (cmp == TRUE) )
	{ /* ���b�`���� */
		/* ���b�`�M�����g�p */
		hdl->ltSignlUse[hdl->latch[i].Sgnl] = FALSE;

		/* �e���b�`�ʒu�擾 */
		if( i==0 )
		{
			lt_pos = hdl->svRsp->Mon1;
		}
		else if( i==1 )
		{
			lt_pos = hdl->svRsp->Mon8;
		}
		else
		{
			lt_pos = 0;
		}

		if( (hdl->latch[i].LtDisable != FALSE)
			|| (PsnMakeLpos(&hdl->psnHdl, lt_pos, i) == FALSE) )
		{ /* ���b�`�����A���́A���b�`�̈�O�Ȃ�ă��b�` */
			/* ���b�`�v�����s */
			MtcReqLatch( hdl, hdl->latch[i].Mode, hdl->latch[i].Sgnl, i );
		}
		else
		{ /* ���b�`�L�� && ���b�`�̈�� */
			hdl->latch[i].Req = FALSE;						/* ���b�`�v��OFF */
			mtcControlLatchSeq( hdl, i );					/* ���b�`�V�[�P���X�N�� */
			hdl->latch[i].Cmp = MTLT_CMP_STS;				/* ���b�`����ON */
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
static	LONG mtcDisableState( MT_HNDL *hdl )
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
				if( mtcGetInitPgPos(hdl) != FALSE )
				{
					hdl->cmdData.SensOn = TRUE;
					hdl->cmdData.Mtype = MTYPE_FOLLOW;
					hdl->ctrlMode = MT_POS_CTRLMD;
					PsnResetApos(&hdl->psnHdl, TRUE);
					rc = MSTAT_READY;
				}
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
			if( mtcGetInitPgPos(hdl) != FALSE )
			{
				hdl->cmdData.SensOn = TRUE;
				hdl->cmdData.Mtype = MTYPE_FOLLOW;
				hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
				hdl->ctrlMode = MT_POS_CTRLMD;
				PsnResetApos(&hdl->psnHdl, TRUE);
				rc = MSTAT_READY;
			}
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
	{/*	�⏕�@�\�ɂ��^�]��ADISCONNECT�����ɏ�ʂ���T�[�{�I���w�߂����� A.0b0 *//* <S0EC> */
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
 �����G���R�[�_�ʒu�i64bit���j�擾�֐�
 SENS_ON�R�}���h�����Ŏ��s
 �⑫�FOMN���g���Ă����ꍇ�ASENS_ON���͑O��f�[�^��\������
       MciGetM3StdMonitorData()�֐��Q�Ƃ̂���
****************************************************************************/
static	INT32 mtcGetInitPgPos( MT_HNDL* hdl )
{
	INT32 ret;

	ret = FALSE;

	if( hdl->mtPrm.ProfPrm.initPgPos64Enable == FALSE )
	{
		/* M2�݊��v���t�@�C���̏ꍇ�͉��������ɔ����� */
		return TRUE;
	}

	if( (hdl->svCmd->SelMon5 != SELMON_FBPOS64L) || (hdl->svCmd->SelMon6 != SELMON_FBPOS64H) )
	{
		/* OMN�̐ݒ���ꎞ�I��PGCNT�ɕύX */
		hdl->svCmd->SelMon5 = SELMON_FBPOS64L;		/* FB�ʒu(����32bit) */
		hdl->svCmd->SelMon6 = SELMON_FBPOS64H;		/* FB�ʒu(���32bit) */
	}

	if( (hdl->svRsp->SelMon5 == SELMON_FBPOS64L) && (hdl->svRsp->SelMon6 == SELMON_FBPOS64H) )
	{
		/* �����G���R�[�_�ʒu�i64bit���j�f�[�^�̎擾 */
		hdl->psnHdl.InitPgPos64[0] = hdl->svRsp->Mon5;
		hdl->psnHdl.InitPgPos64[1] = hdl->svRsp->Mon6;

		/* OMN�̐ݒ�����ɖ߂� */
		hdl->svCmd->SelMon5 = hdl->mtPrm.BasePrm.opMon[0];
		hdl->svCmd->SelMon6 = hdl->mtPrm.BasePrm.opMon[1];

		ret = TRUE;
	}
	return ret;
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


/****************************************************************************
****************************************************************************/
/* Private functions(Motion�w�ߍ쐬����) */
/****************************************************************************
****************************************************************************/
/****************************************************************************
 ���[�V�����w�ߍ쐬
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
 EX_FEED�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeExFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
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
	{ /* EX_FEED�J�n */
		/* ���b�`�L�����Z�� */
		MtcCancelLatch( hdl, EXFEED_LTMD, MTLATCH_NUM );
		if( hdl->cmdData.TSpeed < 0 )
		{ /* ������ */
			cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, -DISTANCE_MAX );
		}
		else
		{ /* ������ */
			cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, DISTANCE_MAX );
		}
		if(hdl->cmdData.MtLatchReq != FALSE )
		{ /* ���b�`�v������ */
			/* ���b�`�v�����s */
			MtcReqLatch( hdl, EXFEED_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
			hdl->seqState++;
		}
	}
	else if( hdl->seqState == 2 )
	{ /* ���b�`�҂���� */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == EXFEED_LTMD) )
		{ /* ���b�`���� */
			tpos = hdl->psnHdl.Lpos[MTLATCH_NUM] + hdl->mtPrm.MtSeqPrm.ex_posing_dist;
			cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, tpos );
			hdl->seqState = 0;
		}
		else
		{ /* ���b�`���� */
			if( hdl->cmdData.TSpeed < 0 )
			{ /* ������ */
				cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, -DISTANCE_MAX );
			}
			else
			{ /* ������ */
				cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, DISTANCE_MAX );
			}
		}
	}

	cmdOut->Mtype = MPOSING;
	cmdOut->Tspd = MlibLABS( hdl->cmdData.TSpeed );
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
		/* ���b�`�L�����Z�� */
		MtcCancelLatch( hdl, EXPOSING_LTMD, MTLATCH_NUM );
		cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		if(hdl->cmdData.MtLatchReq != FALSE )
		{ /* ���b�`�v������ */
			/* ���b�`�v�����s */
			MtcReqLatch( hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
			hdl->seqState++;
		}
	}
	else if( hdl->seqState == 2 )
	{ /* ���b�`�҂���� */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == EXPOSING_LTMD) )
		{ /* ���b�`���� */
			tpos = hdl->psnHdl.Lpos[MTLATCH_NUM] + hdl->mtPrm.MtSeqPrm.ex_posing_dist;
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
 HOMING�w�ߍ쐬�����iDEC�M������j
****************************************************************************/
static	BOOL mtcMakeHomingRefWithDec( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;
	LONG pos_err;
	PSN_HNDL *psn_hdl_ptr;

	psn_hdl_ptr = &hdl->psnHdl;

	if( hdl->mtPrm.ProfPrm.zretLtCancel==ZRET_LATCH_IS_CANCEL )
	{ /* ZRET���b�`�L�����Z�����[�h�Ȃ� seqState=0�Ƀ��Z�b�g */
		if( hdl->cmdData.CmdRstReq )
		{
			/* ���Z�b�g���́A�ŏ����� */
			hdl->seqState = 0;
		}
	}

	switch( hdl->seqState )
	{
	case 0:	/* ���_���A������� */
		if( !hdl->cmdData.CmdRstReq )
		{ /* ���Z�b�g�v�����Ȃ���΁A�w��=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch( hdl, ZRET_LTMD, MTLATCH_NUM );		/* ���b�`�L�����Z�� */
		hdl->seqState = 1;
	case 1:	/* HOMING�J�n(DEC=ON�҂����) */
		psn_hdl_ptr->RefEnable = FALSE;						/* ���t�@�����X�_�N���A */
		cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
		cmdOut->Tspd = hdl->cmdData.TSpeed;					/* TPOS,TSPEED�Z�b�g */

		if( 0 != (hdl->svRsp->RspSigBit & DEC_SIG_BITMSK) )
		{ /* DEC=ON */
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* �A�v���[�`���x1�� */
			hdl->seqState = 2;								/* DEC=OFF�҂��� */
		}
		break;
	case 2:	/* DEC=OFF�҂���� */
		cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;			/* TPOS,TSPEED=�A�v���[�`���x1�Z�b�g */

		if( 0 == (hdl->svRsp->RspSigBit & DEC_SIG_BITMSK) )
		{ /* DEC=OFF */
			if(hdl->cmdData.MtLatchReq != FALSE )
			{ /* ���b�`�v������ */
				/* ���b�`�v�����s */
				MtcReqLatch( hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
				hdl->seqState = 3;							/* ���b�`�����҂��� */
			}
		}
		break;
	case 3:	/* ���b�`�����҂���� */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == ZRET_LTMD) )
		{ /* ���b�`���� */ /* MECHATROLINK-2 �p�Ɉȉ��̏��������{ */
			if( (hdl->mtPrm.MtSeqPrm.zret_dir < 0) &&
				(hdl->mtPrm.ProfPrm.zretDistDir == ZRET_DIST_DIR_IS_RELATIVE) )
			{
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;		/*�ŏI���s���������t�]*/
			}
			else
			{
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;
			}

			tpos = psn_hdl_ptr->Lpos[MTLATCH_NUM] + tpos;	/* �ŏI�ʒu�Z�b�g */
			cmdOut->DposIn = PsnSetTPOS( psn_hdl_ptr, tpos );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;		/* �A�v���[�`���x2�� */
			hdl->seqState = 4;								/* �ʒu���ߊ����҂��� */
		}
		else
		{ /* ���b�`���� */
			cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* �A�v���[�`���x1 */
		}
		break;
	case 4:	/* �ʒu���ߊ����҂���� */
		cmdOut->DposIn = psn_hdl_ptr->Tpos - psn_hdl_ptr->Ipos;
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			/* �A�v���[�`���x2 */

		pos_err = psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos;
		if( (psn_hdl_ptr->Den == TRUE) && ((ULONG)MlibLABS(pos_err) <= psn_hdl_ptr->Prm->psetWidth) )
		{ /* DEN=1 && (Tpos-Apos)<=�ʒu���ߊ����� */
			/* �ʒu���ߊ��� */
			psn_hdl_ptr->RefEnable = TRUE;					/* ���t�@�����X�_�L�� */
			PsnSetCordinate( psn_hdl_ptr, 0, 0 );			/* ���W�n0���Z�b�g */
			if(hdl->mtPrm.ProfPrm.zretLtCancel==ZRET_LATCH_IS_NOCANCEL)
			{ /* ZRET���b�`�L�����Z���������[�h�Ń��b�`�L�����Z�����s */
				/* ���b�`�L�����Z�� */
				MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
			}
			hdl->seqState = 0;								/* ���_���A������Ԃ� */
		}
		break;
	}
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 HOMING�w�ߍ쐬�����iDEC�M���Ȃ��j
****************************************************************************/
static	BOOL mtcMakeHomingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;
	LONG pos_err;
	PSN_HNDL *psn_hdl_ptr;

	psn_hdl_ptr = &hdl->psnHdl;

	if( hdl->cmdData.CmdRstReq )
	{
		hdl->seqState = 1;									/* ���Z�b�g���́A�ŏ����� */
	}

	switch( hdl->seqState )
	{
	case 0:	/* ���_���A������� */
		cmdOut->DposIn = 0;
		cmdOut->Tspd = 0;
		break;
	case 1:	/* �A�v���[�`��� */
		MtcCancelLatch(hdl, ZRET_LTMD, MTLATCH_NUM);		/* ���b�`�L�����Z�� */
		psn_hdl_ptr->RefEnable = FALSE;						/* ���t�@�����X�_�N���A */
		cmdOut->DposIn = PsnSetTPOSDist(psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;			/* TPOS,TSPEED=�A�v���[�`���x1�Z�b�g */

		if(hdl->cmdData.MtLatchReq != FALSE )
		{ /* ���b�`�v������ */
			/* ���b�`�v�����s */
			MtcReqLatch( hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
			hdl->seqState = 2;								/* ���b�`�����҂��� */
		}
		break;
	case 2:	/* ���b�`�����҂���� */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == ZRET_LTMD) )
		{ /* ���b�`���� */
			/* �ŏI�ʒu�Z�b�g */
			tpos = psn_hdl_ptr->Lpos[MTLATCH_NUM] + hdl->mtPrm.MtSeqPrm.zret_dist;

			cmdOut->DposIn = PsnSetTPOS( psn_hdl_ptr, tpos );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;		/* �A�v���[�`���x2�� */
			hdl->seqState = 3;								/* �ʒu���ߊ����҂��� */
		}
		else
		{ /* ���b�`���� */
			cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* �A�v���[�`���x1 */
		}
		break;
	case 3:	/* �ʒu���ߊ����҂���� */
		cmdOut->DposIn = psn_hdl_ptr->Tpos - psn_hdl_ptr->Ipos;
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			/* �A�v���[�`���x2 */

		pos_err = psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos;
		if( (psn_hdl_ptr->Den == TRUE) && ((ULONG)MlibLABS(pos_err) <= psn_hdl_ptr->Prm->psetWidth) )
		{ /* DEN=1 && (Tpos-Apos)<=�ʒu���ߊ����� */
			/* �ʒu���ߊ��� */
			psn_hdl_ptr->RefEnable = TRUE;					/* ���t�@�����X�_�L�� */
			PsnSetCordinate( psn_hdl_ptr, 0, 0 );			/* ���W�n0���Z�b�g */
			hdl->seqState = 0;								/* ���_���A������Ԃ� */
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
 VELCTRL�w�ߍ쐬�����i�������x�w�肠��j
****************************************************************************/
static	BOOL mtcMakeVelWithAccrRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MVELCTRL_WITHACCR;
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


/***************************************************************************
 SPOSING�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeSposingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	MG_SPOSING	*sposing;	/* SPOSING�p�p�����[�^	*/
	LONG	state;

	sposing = &hdl->mtgHdl.sposing;
	state = sposing->State;
	
	if( hdl->cmdData.CmdRstReq )
	{
		PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		cmdOut->Tspd = hdl->cmdData.TSpeed;
	}

	/* �R�}���h�؂�ւ����̏����� */
	if(cmdOut->Mtype != MSPOSING)
	{
		sposing->LastTargetSpd  = 0;		/* SPOSING�O��ڕW���x		*/
		sposing->LastTargetPosition = 0;	/* SPOSING�O��ڕW�ʒu		*/
		sposing->LastDposIn = 0;			/* SPOSING�O��c����		*/
	}

	cmdOut->Mtype = MSPOSING;
	cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	/* �w�ߕύX�`�F�b�N				*/
	/* �ڕW���x���ύX���ꂽ�ꍇ		*/
	if(sposing->LastTargetSpd != hdl->cmdData.TSpeed)
	{
		/* �������łȂ��Ƃ���SPOSING���X�^�[�g	*/
		if((state != SPOSING_DEC_MODE1) && (state != SPOSING_DEC_MODE2))	
		{
			state = SPOSING_INI_MODE;
		}
		/* �������̏ꍇ�͖��� */
		else
		{
			/* no operation */
		}
	}

	/* �ڕW�ʒu���ύX���ꂽ�ꍇ */
	if(sposing->LastTargetPosition != hdl->psnHdl.Tpos)
	{
		/* �������łȂ��Ƃ���SPOSING���X�^�[�g */
		if((state != SPOSING_DEC_MODE1) && (state != SPOSING_DEC_MODE2))	
		{
			state = SPOSING_INI_MODE;
		}
		/* �������̂Ƃ��� �ύX��̈ړ����������� && �c�������������Ƃ����X�^�[�g */
		else
		{
			/* �ڕW�ʒu�𐳕����ɕύX */
			if( sposing->LastDposIn > 0 )
			{
				if(sposing->LastDposIn < cmdOut->DposIn)
				{
					state = SPOSING_INI_MODE;		
				}
			}
			/* �ڕW�ʒu�𕉕����ɕύX */
			else if( sposing->LastDposIn < 0 )
			{
				if(sposing->LastDposIn > cmdOut->DposIn)
				{
					state = SPOSING_INI_MODE;		
				}
			}
			/* sposing->LastDposIn=0�̂Ƃ�(���݂��Ȃ�) */
			else
			{
				state = SPOSING_INI_MODE;		
			}
		}
	}
	
	/* ����l��ێ����� */
	if(state == SPOSING_INI_MODE)
	{
		sposing->LastTargetSpd = hdl->cmdData.TSpeed;		/* SPOSING�O��ڕW���x	*/
		sposing->LastTargetPosition = hdl->psnHdl.Tpos;		/* SPOSING�O��ڕW�ʒu	*/
		sposing->LastDposIn = cmdOut->DposIn;				/* SPOSING�O��c����	*/
	}
	
	sposing->State = state;
	
	return FALSE;
}


/****************************************************************************
 SHOLD�w�ߍ쐬����
****************************************************************************/
static	BOOL mtcMakeSholdRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG dpos;
	
	if( hdl->cmdData.HoldMod == 1 )
	{ /* �}����~ */
		dpos = 0;
		cmdOut->Mtype = MNONE;
	}
	else if( hdl->cmdData.HoldMod == 2 )
	{ /* ������~2 */
		cmdOut->Mtype = MSHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL2;
//		dpos = MtgGetDctDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
		if( (hdl->mtgHdl.sposing.State != SPOSING_DEC_MODE1) 
			&& (hdl->mtgHdl.sposing.State != SPOSING_DEC_MODE2) )
		{
			dpos = MtgGetDctSpDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
		}
	}
	else
	{ /* ������~1 */
		cmdOut->Mtype = MSHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL1;
//		dpos = MtgGetDctDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
		if( (hdl->mtgHdl.sposing.State != SPOSING_DEC_MODE1) 
			&& (hdl->mtgHdl.sposing.State != SPOSING_DEC_MODE2) )
		{
			dpos = MtgGetDctSpDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
		}
	}
	if( (hdl->mtgHdl.sposing.State != SPOSING_DEC_MODE1) 
		&& (hdl->mtgHdl.sposing.State != SPOSING_DEC_MODE2) )
	{
		cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, dpos );
		hdl->mtgHdl.sposing.State = SPOSING_INI_MODE;
	}
	else
	{
		cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	}
	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}

/***************************************** end of file **********************************************/
