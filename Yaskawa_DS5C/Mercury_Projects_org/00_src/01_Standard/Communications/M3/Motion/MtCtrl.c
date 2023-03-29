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
 Private functions(シーケンス処理関連) 
****************************************************************************/
/* 連続ラッチ要求処理 */
static	VOID mtcControlLatchSeq( MT_HNDL *hdl, LONG i );
/* モーション指令作成 */
static	VOID mtcCreateInCmd( MT_HNDL *hdl );
/* OT状態制御処理 */
static	BOOL mtcControlOTState( MT_HNDL *hdl );
/* 各ラッチの完了チェック */
static	VOID mtcCheckEachLatchComp( MT_HNDL *hdl, LONG i );
/* モーション状態制御(Disable状態) */
static	LONG mtcDisableState( MT_HNDL *hdl );
/* モーション状態制御(WaitPGRdy状態) */
static	LONG mtcWaitPgRdyState( MT_HNDL *hdl );
/* モーション状態制御(Ready状態) */
static	LONG mtcReadyState( MT_HNDL *hdl );
/* モーション状態制御(Run状態) */
static	LONG mtcRunState( MT_HNDL *hdl );
/* モーション状態制御(WaitSvOff状態) */
static	LONG mtcWaitSvOffState( MT_HNDL *hdl );
/* モーション状態制御(RampStop状態) */
static	LONG mtcRampStopState( MT_HNDL *hdl );
/* 初期エンコーダ位置（64bit分）取得関数 */
static	INT32 mtcGetInitPgPos( MT_HNDL* hdl );

/****************************************************************************
 Private functions(Motion指令作成処理) 
****************************************************************************/
/* NOP指令作成処理 */
static	BOOL mtcMakeNopRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* Followup指令作成処理 */
static	BOOL mtcMakeFollowRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* POSING指令作成処理 */
static	BOOL mtcMakePosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* SPOSING指令作成処理 */
static	BOOL mtcMakeSposingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* INTERPOLATE指令作成処理 */
static	BOOL mtcMakeInterpolRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* FEED指令作成処理 */
static	BOOL mtcMakeFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* HOLD指令作成処理 */
static	BOOL mtcMakeHoldRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* EX_FEED指令作成処理 */
static	BOOL mtcMakeExFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* EX_POSING指令作成処理 */
static	BOOL mtcMakeExPosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* HOMING指令作成処理(DEC信号なし) */
static	BOOL mtcMakeHomingRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* HOMING指令作成処理(DEC信号あり) */
static	BOOL mtcMakeHomingRefWithDec( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* VELCTRL指令作成処理 */
static	BOOL mtcMakeVelRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* VELCTRL指令作成処理(加減速度指定あり) */
static	BOOL mtcMakeVelWithAccrRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* TRQCTRL指令作成処理 */
static	BOOL mtcMakeTrqRef( MT_HNDL *hdl, MG_INOUT *cmdOut );
/* SHOLD指令作成処理 */
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
	mtcMakeNopRef,				/* Nopモーション */
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

/* ラッチモニタ番号Table */
const UCHAR LtMonTbl[] = {
	MONSEL_LTPOS_C_L, MONSEL_LTPOS_EXT1_L, MONSEL_LTPOS_EXT2_L, MONSEL_LTPOS_EXT3_L
};


/****************************************************************************
****************************************************************************/
/* Public functions (Task関連) */
/****************************************************************************
****************************************************************************/
/****************************************************************************
 初期化処理
****************************************************************************/
VOID MtcInit( MT_HNDL *hdl, SV_OPTION_IF *svop_if )
{
	MlibResetByteMemory( hdl, sizeof(MT_HNDL) );

	/* Position Manager初期化 */
	PsnInit( &hdl->psnHdl, &hdl->mtPrm.PsnPrm );

	/* MG初期化 */
	MtgInit( &hdl->mtgHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.MgPrm );

	/* AccFilter初期化 */
	AcfInit( &hdl->acfHdl, &hdl->mtPrm.BasePrm, &hdl->mtPrm.AccFilPrm );

	hdl->mtState = MSTAT_DISABLE;
	hdl->otState = OTSTAT_NORMAL;
	hdl->crtMtype = MTYPE_NONE;
	hdl->ctrlMode = MT_NO_CTRLMD;
	hdl->seqState = 0;
	hdl->svCmd = &svop_if->CycCmd;
	hdl->svRsp = &svop_if->CycRsp;

	/* サイクリックデータ初期化 */
	svop_if->CycCtrl.ExtCmdSel1 = EXTCMD_TRQLMT_P;		/* 正側トルク制限 */
	svop_if->CycCtrl.ExtCmdSel2 = EXTCMD_TRQLMT_N;		/* 負側トルク制限 */
	svop_if->CycCtrl.ExtCmdSel3 = 0;					/* 割付なし */
	svop_if->CycCtrl.ExtCmdSel4 = 0;					/* 割付なし */

	svop_if->CycCmd.SelMon1 = MONSEL_LTPOS_C_L;			/* C相ラッチ位置 */
	svop_if->CycCmd.SelMon2 = MONSEL_TRQREF;			/* トルク指令 */
	svop_if->CycCmd.SelMon3 = MONSEL_FBSPD;				/* FB速度 */
	svop_if->CycCmd.SelMon4 = MONSEL_VPOSERR_L;			/* 仮想位置偏差 */
	svop_if->CycCmd.SelMon5 = 0;						/* OMN1 */
	svop_if->CycCmd.SelMon6 = 0;						/* OMN2 */

//	svop_if->CycCmd.SelMon7 = MONSEL_POSERR_L;			/* 位置偏差 */	/* <S0AC> */

/* <S1C0>：Start */
#if CSW_BOARD_TYPE == SGD7S_TYPE
	svop_if->CycCmd.SelMon7 = MONSEL_INSGNL;			/* 入力信号 */
#else /* CSW_BOARD_TYPE == SGD7W_TYPE */
	svop_if->CycCmd.SelMon7 = MONSEL_INSGNL_2AXIS;		/* 入力信号 */
#endif /* f CSW_BOARD_TYPE == SGD7S_TYPE */
/* <S1C0>：End */

	svop_if->CycCmd.SelMon8 = MONSEL_LTPOS_EXT1_L;		/* EXT1ラッチ位置 */
}


/****************************************************************************
 モーション出力処理
****************************************************************************/
VOID MtcOutputData( MT_HNDL *hdl )
{
	BOOL den, denOt;

	hdl->ctrlMode = hdl->cmdData.CtrlMode;					/* 制御モードセット */
	hdl->mtState = mtcControlState[hdl->mtState]( hdl );	/* モーション状態制御 */
	mtcCreateInCmd( hdl );									/* モーション指令作成 */
	denOt = mtcControlOTState( hdl );						/* OT制御 */


	den = MtgGenerate(&hdl->mtgHdl, &hdl->outData);			/* モーション出力 */

	den = den & denOt;										/* OT停止中は、Den=0とする */
	hdl->outData.CtrlModeLst = hdl->ctrlMode;				/* 前回制御モード更新 */

	hdl->psnHdl.Ipos += hdl->outData.DposOut;				/* IPOSセット */

	AcfGenerate( &hdl->acfHdl, &hdl->outData.DposOut,
						hdl->cmdData.AccFilSel, &den );		/* 位置指令フィルタ実行 */

	hdl->psnHdl.Pos += hdl->outData.DposOut;				/* POSセット */
	hdl->psnHdl.lstDpos = hdl->psnHdl.Dpos;					/* DPOS前回値更新 */
	hdl->psnHdl.Dpos = hdl->outData.DposOut;				/* DPOS今回値更新 */

	hdl->psnHdl.Den = den;									/* DEN状態セット */

	hdl->svCmd->CmdCtrl.ModSel = (UCHAR)hdl->ctrlMode;		/* 制御モードセット */
	hdl->svCmd->Position += hdl->outData.DposOut;			/* 位置指令セット */
	hdl->svCmd->Speed = hdl->outData.VrefOut;				/* 速度指令セット */
	hdl->svCmd->Torque = hdl->outData.TrefOut;				/* トルク指令セット */
}


/****************************************************************************
 モーションデータ更新処理
****************************************************************************/
VOID MtcInputData( MT_HNDL *hdl )
{
	LONG fbSpd;
	LONG PosErr;

	PosErr = hdl->svRsp->PosError;
	PsnUpdateFBData( &hdl->psnHdl, hdl->svRsp->FbPosition, PosErr );

	/* ラッチ1 */
	if( (hdl->latch[0].Req) && (hdl->svRsp->SelMon1 == LtMonTbl[hdl->latch[0].Sgnl]) )
	{
		mtcCheckEachLatchComp( hdl, 0 );
	}
	/* ラッチ2 */
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
		{ /* OT検出中でない || モータ動作中 */
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
	/* PSETおよびNEAR信号を作成する。	*//* <S0AE> */
	PsnSetPsetState( &hdl->psnHdl );
}


/****************************************************************************
****************************************************************************/
/* Public functions (操作関数) */
/****************************************************************************
****************************************************************************/
/****************************************************************************
 ラッチ要求発行
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
INT32 MtcReqLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, LONG i )
{
	LONG	ltReq;

	ltReq = hdl->svCmd->CmdLtBit;

	/* ラッチ信号の干渉チェック */
	if( hdl->ltSignlUse[sgnl] == FALSE )
	{
		hdl->ltSignlUse[sgnl] = TRUE;
	}
	else
	{
		return MCI_LTSGNL_CNFLCT;
	}

	/* ラッチ番号チェック */
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
 ラッチ要求キャンセル
 i：ラッチ番号 （i<= LT_NUM であること)
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
 プロファイルタイプに依存するモーション情報の設定(Motion Control)
 
 設定情報：
 ・原点復帰最終走行距離の方向
 ・コマンド切り替わり時にZretコマンドのラッチ動作をキャンセルするか？
 引数：type  プロファイルタイプ
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
 連続ラッチ要求処理
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
static	VOID mtcControlLatchSeq( MT_HNDL *hdl, LONG i )
{
	/* ラッチシーケンス要求チェック */
	if( hdl->latch[i].SeqReq != MTLT_SEQ_REQ )
	{
		return;
	}

	/* シーケンスカウンタ更新 */
	hdl->latch[i].SeqCnt++;
	hdl->latch[i].SeqSts.cnt[1] = (UCHAR)hdl->latch[i].SeqCnt;
	if( hdl->latch[i].SeqCnt >= hdl->mtPrm.LtPrm.seqNum )
	{ /*シーケンス完了 */
		hdl->latch[i].SeqCnt = 0;
		hdl->latch[i].SeqSts.cnt[0]++;
		if( (hdl->mtPrm.LtPrm.loopCnt != 0) &&
			(hdl->latch[i].SeqSts.cnt[0] >= hdl->mtPrm.LtPrm.loopCnt) )
		{ /* 連続ラッチ完了 */
			hdl->latch[i].SeqReq = MTLT_SEQ_DONE;
			return;
		}
	}
	/* ラッチ要求発行 */
	MtcReqLatch( hdl, hdl->latch[i].Mode, 
					hdl->mtPrm.LtPrm.ltSgnl[hdl->latch[i].SeqCnt], i );
}


/****************************************************************************
 各ラッチの完了チェック
 i：ラッチ番号 （i<= LT_NUM であること)
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
	{ /* ラッチ完了 */
		/* ラッチ信号未使用 */
		hdl->ltSignlUse[hdl->latch[i].Sgnl] = FALSE;

		/* 各ラッチ位置取得 */
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
		{ /* ラッチ無効、又は、ラッチ領域外なら再ラッチ */
			/* ラッチ要求発行 */
			MtcReqLatch( hdl, hdl->latch[i].Mode, hdl->latch[i].Sgnl, i );
		}
		else
		{ /* ラッチ有効 && ラッチ領域内 */
			hdl->latch[i].Req = FALSE;						/* ラッチ要求OFF */
			mtcControlLatchSeq( hdl, i );					/* ラッチシーケンス起動 */
			hdl->latch[i].Cmp = MTLT_CMP_STS;				/* ラッチ完了ON */
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
static	LONG mtcDisableState( MT_HNDL *hdl )
{
	BOOL	senCmd;
	LONG	rc;

	rc = MSTAT_DISABLE;

	/* モーション指令初期化 */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_NO_CTRLMD;
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* モーション有効チェック */
	if( hdl->cmdData.MtEenable )
	{
		rc = MSTAT_WAITPG_RDY;
		/* abs時:SensOn指令状態, incr時:1 */
		senCmd = ( hdl->mtPrm.PsnPrm.absPG != FALSE ) ? hdl->cmdData.SensOn : TRUE;

		if( FALSE != senCmd )
		{
			hdl->svCmd->CmdSeqBit |= SENSOR_ON_BIT;
			if( 0 != (hdl->svRsp->RspSeqBit & SENS_ON_COMP_BITMSK) )
			{ /* センサON状態 */
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
 モーション状態制御(WaitPGRdy状態)
****************************************************************************/
static	LONG mtcWaitPgRdyState( MT_HNDL *hdl )
{
	BOOL	senCmd;
	LONG	rc;

	rc = MSTAT_WAITPG_RDY;

	/* モーション指令初期化 */
	hdl->cmdData.Mtype = MTYPE_NONE;
	hdl->cmdData.ServoOn = FALSE;
	hdl->ctrlMode = MT_VEL_CTRLMD;
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* モーション無効要求があればDisble状態へ */
	if( !hdl->cmdData.MtEenable )
	{
		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		return MSTAT_DISABLE;
	}

	/* PGRDYならば,Followup=ON、APOS設定し、Ready状態へ */
	/* abs時:SensOn指令状態, incr時:1 */
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
 モーション状態制御(Ready状態)
****************************************************************************/
static	LONG mtcReadyState( MT_HNDL *hdl )
{
	LONG	rc;
	BOOL	svOnCmp;

	rc = MSTAT_READY;

	/* モーション無効要求があればDisble状態へ */
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

	/* SensOff指令OFF ならばWaitPGRdy状態へ */
	if( !hdl->cmdData.SensOn )
	{
		hdl->svCmd->CmdSeqBit &= ~(SENSOR_ON_BIT);
		hdl->cmdData.Mtype = MTYPE_NONE;
		hdl->ctrlMode = MT_VEL_CTRLMD;
		PsnResetApos(&hdl->psnHdl, FALSE);
		return MSTAT_WAITPG_RDY;
	}

	/* サーボON指令発行*/
	if( hdl->cmdData.ServoOn )
	{
		hdl->svCmd->CmdSeqBit |= (SERVO_ON_BIT | POL_DETECT_BIT);
	}
	else
	{
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
	}

	if(0 == (hdl->svRsp->RspCtrl.RspCtrlBit & OP_ENABLED_BITMSK) )
	{/*	補助機能による運転後、DISCONNECTせずに上位からサーボオン指令したら A.0b0 *//* <S0EC> */
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
		/* フォローアップ & サーボON指令発行*/
		if( hdl->ctrlMode == MT_POS_CTRLMD )
		{
			hdl->cmdData.Mtype = MTYPE_FOLLOW;
		}

		/* サーボON完了ならばRUN状態へ */
		svOnCmp = hdl->cmdData.ServoOn & (hdl->svRsp->RspSeqBit >> SERVO_ON_COMP_BITNO);
		if( svOnCmp )
		{
			return MSTAT_RUN;
		}
	}

	return rc;
}


/****************************************************************************
 モーション状態制御(Run状態)
****************************************************************************/
static	LONG mtcRunState( MT_HNDL *hdl )
{
	LONG	rc;

	rc = MSTAT_RUN;

	/* サーボOFF状態ならばReady状態へ */
	if( 0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) )
	{ /* ScanC処理完了状態 && SVON未完の場合 */
		hdl->cmdData.ServoOn = FALSE;
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
		if( hdl->ctrlMode != MT_TRQ_CTRLMD )
		{
			hdl->cmdData.Mtype = MTYPE_NONE;				/* 指令停止*/
		}
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;
		return MSTAT_READY;
	}

	/* サーボOn指令=OFFならばサーボOFF待ち状態へ */
	if( !hdl->cmdData.ServoOn )
	{
		hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
		hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;
		if( hdl->mtPrm.MtSeqPrm.rampTimeOut == 0 )
		{ /* RampStop機能無効 */
			hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
			if( hdl->ctrlMode != MT_TRQ_CTRLMD )
			{
				hdl->cmdData.Mtype = MTYPE_NONE;			/* 指令停止*/
			}
			return MSTAT_WAIT_SVOFF;
		}
		else
		{ /* RampStop機能有効 */
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
 モーション状態制御(WaitSvOff状態)
****************************************************************************/
static	LONG mtcWaitSvOffState( MT_HNDL *hdl )
{
	LONG	rc;

	rc = MSTAT_WAIT_SVOFF;

	/* サーボOFF指令発行 */
	hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);

	/* サーボOFFするまで指令停止 */
	if( hdl->ctrlMode != MT_TRQ_CTRLMD )
	{
		hdl->cmdData.Mtype = MTYPE_NONE;
	}
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* サーボOFF状態ならばReady状態へ */
	if( 0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) )
	{
		return MSTAT_READY;
	}

	return rc;
}


/****************************************************************************
 モーション状態制御(RampStop状態)
****************************************************************************/
static	LONG mtcRampStopState( MT_HNDL *hdl )
{
	BOOL	den;
	LONG	rc;

	rc = MSTAT_RAMP_STOP;
	den = hdl->psnHdl.Den;

	/* 減速停止指令セット */
	hdl->cmdData.Mtype = MTYPE_HOLD;
	hdl->ctrlMode = MT_POS_CTRLMD;
	hdl->cmdData.HoldMod = 2;
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;

	/* サーボOFF状態ならばReady状態へ */
	if( 0 == (hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) )
	{
		hdl->cmdData.ServoOn = FALSE;
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
		return MSTAT_READY;
	}

	/* 減速停止、又は、タイムアウトしたらWaitSvOff状態へ */
	++hdl->timer;
	if( (hdl->timer > hdl->mtPrm.MtSeqPrm.rampTimeOut)			/* タイムアウト */
		|| (den && ( MlibLABS(hdl->svRsp->Mon3) < MOTOR_STP_SPD)) )	/* den=1&&モータ停止 */
	{
		hdl->svCmd->CmdSeqBit &= ~(SERVO_ON_BIT | POL_DETECT_BIT);
		return MSTAT_WAIT_SVOFF;
	}
	return rc;
}


/****************************************************************************
 初期エンコーダ位置（64bit分）取得関数
 SENS_ONコマンド処理で実行
 補足：OMNを使っていた場合、SENS_ON中は前回データを表示する
       MciGetM3StdMonitorData()関数参照のこと
****************************************************************************/
static	INT32 mtcGetInitPgPos( MT_HNDL* hdl )
{
	INT32 ret;

	ret = FALSE;

	if( hdl->mtPrm.ProfPrm.initPgPos64Enable == FALSE )
	{
		/* M2互換プロファイルの場合は何もせずに抜ける */
		return TRUE;
	}

	if( (hdl->svCmd->SelMon5 != SELMON_FBPOS64L) || (hdl->svCmd->SelMon6 != SELMON_FBPOS64H) )
	{
		/* OMNの設定を一時的にPGCNTに変更 */
		hdl->svCmd->SelMon5 = SELMON_FBPOS64L;		/* FB位置(下位32bit) */
		hdl->svCmd->SelMon6 = SELMON_FBPOS64H;		/* FB位置(上位32bit) */
	}

	if( (hdl->svRsp->SelMon5 == SELMON_FBPOS64L) && (hdl->svRsp->SelMon6 == SELMON_FBPOS64H) )
	{
		/* 初期エンコーダ位置（64bit分）データの取得 */
		hdl->psnHdl.InitPgPos64[0] = hdl->svRsp->Mon5;
		hdl->psnHdl.InitPgPos64[1] = hdl->svRsp->Mon6;

		/* OMNの設定を元に戻す */
		hdl->svCmd->SelMon5 = hdl->mtPrm.BasePrm.opMon[0];
		hdl->svCmd->SelMon6 = hdl->mtPrm.BasePrm.opMon[1];

		ret = TRUE;
	}
	return ret;
}


/****************************************************************************
 OT状態制御(OT停止動作中 rc = 0, それ以外 rc = 1)
****************************************************************************/
static	BOOL mtcControlOTState( MT_HNDL *hdl )
{
	BOOL p_ot, n_ot;
	LONG dpos;
	LONG remdist;
	LONG wk;

	/* APOSソフトリミットチェック */
	PsnCheckSoftLimitFB( &hdl->psnHdl, &p_ot, &n_ot );

	/* OT信号チェック */
	p_ot |= ((hdl->svRsp->RspSigBit >> P_OT_SIG_BITNO) & 1);
	n_ot |= ((hdl->svRsp->RspSigBit >> N_OT_SIG_BITNO) & 1);

	/* OT指令セット */
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

	/* OT解除条件チェック */
	if( ((p_ot | n_ot) == 0)											/* OT信号解除 */
		|| ((hdl->svRsp->RspSeqBit & SERVO_ON_COMP_BITMSK) == FALSE)	/* サーボOFF状態 */
		|| (hdl->cmdData.CtrlMode != MT_POS_CTRLMD) ) 					/* 速度・トルク制御 */
	{ /* OT解除 */
		hdl->otState = OTSTAT_NORMAL;
		return TRUE;
	}

	/* 引き戻し条件チェック */
	dpos = hdl->psnHdl.Apos - hdl->psnHdl.Pos;
	/* オーバーフローする可能性があるため飽和加算処理(指令フィルタの溜まり分は31bit以下) */
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
	{ /* 引き戻しあり */
/* 2011.04.04 Y.Oka <V776> */
//		if(hdl->outData.Tspd != 0)
		if((hdl->outData.Tspd != 0) || (hdl->otState == OTSTAT_RETRACTION) )
/* 2011.04.04 Y.Oka <V776> */
		{ /* 目標速度≠0 or 引き戻し中 */
			/* 2011.04.04 Y.Oka <V679> */
//			hdl->otState = OTSTAT_WAITNORMAL;
			hdl->otState = OTSTAT_RETRACTION;
			/* 2011.04.04 Y.Oka <V679> */
			return TRUE;
		}
	}

	/* 2011.04.04 Y.Oka <V679> */
	/* 引き戻し終了条件チェック */
	if( (hdl->otState == OTSTAT_RETRACTION) && (dpos == 0) )
	{
		remdist = hdl->psnHdl.Tpos - hdl->psnHdl.Apos;
		if( (p_ot && (!n_ot) && (remdist < 0))
			|| (n_ot && (!p_ot) && (remdist > 0)) )
		{
			return TRUE;
		}
	}

	/* OT状態制御 */
	hdl->ctrlMode = MT_VEL_CTRLMD;
	hdl->cmdData.Mtype = hdl->crtMtype = MTYPE_NONE;	/* Motion Type Clear */
	mtcMakeNopRef(hdl, &hdl->outData);					/* 指令0 */
	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;				/* フィルタクリア */
	AcfClear( &hdl->acfHdl );							/* フィルタクリア */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;					/* IPOSリセット */
	hdl->psnHdl.Tpos = hdl->outData.DposIn + hdl->psnHdl.Ipos;
	if( MlibLABS(hdl->svRsp->Mon3) > MOTOR_STP_SPD )
	{
		/* 2011.04.04 Y.Oka <V679> */
//		if( hdl->otState != OTSTAT_WAITNORMAL )
		if( (hdl->otState != OTSTAT_WAITNORMAL) && (hdl->otState != OTSTAT_RETRACTION) )
		{
			hdl->otState = OTSTAT_STOP;					/* OTSTAT_WAITNORMAL時は状態そのまま */
		}
		return FALSE;
	}
	hdl->otState = OTSTAT_WAITNORMAL;

	return TRUE;
}


/****************************************************************************
****************************************************************************/
/* Private functions(Motion指令作成処理) */
/****************************************************************************
****************************************************************************/
/****************************************************************************
 モーション指令作成
****************************************************************************/
static	VOID mtcCreateInCmd( MT_HNDL *hdl )
{
	LONG	rc;

	/* モーション切り替わり→モーションリスタート */
	if( hdl->cmdData.Mtype != hdl->crtMtype )
	{
		hdl->seqState = 0;					/* Motion シーケンス状態クリア */
	}

#ifdef PRM_BANK_ENABLE
	/* 加減速PRM切り替え */
	if( hdl->mtPrm.Bnk.bankNum )
	{ /* バンク有効(バンクPRMセット) */
		if( hdl->psnHdl.Den )
		{ /* バンクChange */
			hdl->mtPrm.Bnk.bankSel = hdl->mtPrm.Bnk.cmdSel;
		}

		hdl->acfHdl.Prm = &( hdl->mtPrm.Bnk.acfPrm[hdl->mtPrm.Bnk.bankSel] );
		if( hdl->cmdData.CmdRateOn )
		{ /* 加減速指令有効(指令PRMセット)<Rev2.01> */
			hdl->mtgHdl.prm = &hdl->mtPrm.CmdRate;
		}
		else
		{ /* 加減速指令無効(バンクPRMセット) */
			hdl->mtgHdl.prm = &( hdl->mtPrm.Bnk.mgPrm[hdl->mtPrm.Bnk.bankSel] );
		}
	}
	else
#endif //PRM_BANK_ENABLE
	{ /* バンク無効 */
		hdl->acfHdl.Prm = &( hdl->mtPrm.AccFilPrm );
		if( hdl->cmdData.CmdRateOn )
		{ /* 加減速指令有効(指令PRMセット)<Rev2.01> */
			hdl->mtgHdl.prm = &hdl->mtPrm.CmdRate;
		}
		else
		{ /* 加減速指令無効(デフォルトPRMセット) */
			hdl->mtgHdl.prm = &hdl->mtPrm.MgPrm;
		}
	}

	/* モーション指令作成処理 */
	rc = mtcMakeCmd[hdl->cmdData.Mtype]( hdl, &hdl->outData );
	hdl->crtMtype = hdl->cmdData.Mtype; 

	if( rc == TRUE )
	{
		hdl->cmdData.Mtype = MTYPE_NONE;	/* ニュートラルへ*/
	}
	hdl->cmdData.CmdRstReq = FALSE;
}


/****************************************************************************
 NOP指令作成処理
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
 Followup指令作成処理
****************************************************************************/
static	BOOL mtcMakeFollowRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	BOOL rc;

	rc = FALSE;

	cmdOut->Mtype = MFOLLOW;
	cmdOut->DposIn = hdl->psnHdl.Apos - hdl->psnHdl.Pos;
	if( cmdOut->DposIn < (-FOLLOWUP_MAX_REF) )
	{
		cmdOut->DposIn = -FOLLOWUP_MAX_REF;	/* クランプ*/
	}
	else if( cmdOut->DposIn > FOLLOWUP_MAX_REF )
	{
		cmdOut->DposIn = FOLLOWUP_MAX_REF;	/* クランプ*/
	}
	else
	{
		rc = TRUE;
	}

	cmdOut->Tspd = 0;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* フィルタクリア */
	AcfClear(&hdl->acfHdl);					/* フィルタクリア */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOSリセット */
	hdl->psnHdl.Tpos = cmdOut->DposIn + hdl->psnHdl.Ipos;
	return rc;
}


/****************************************************************************
 POSING指令作成処理
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
 INTERPOLATE指令作成処理
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
	{ /* 残払い出し回数=0なら終了 */
		return mtcMakeNopRef( hdl, cmdOut );
	}
	hdl->segCnt--;

	cmdOut->Mtype = MINTERPOLATE;
	cmdOut->Tspd = cmdOut->DposIn;

	return FALSE;
}


/****************************************************************************
 FEED指令作成処理
****************************************************************************/
static	BOOL mtcMakeFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	if( hdl->cmdData.TSpeed < 0 )
	{ /* 負方向 */
		cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, -DISTANCE_MAX );
		cmdOut->Tspd = -hdl->cmdData.TSpeed;
	}
	else
	{ /* 正方向 */
		cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, DISTANCE_MAX );
		cmdOut->Tspd =  hdl->cmdData.TSpeed;
	}

	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 HOLD指令作成処理
****************************************************************************/
static	BOOL mtcMakeHoldRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG dpos;

	if( hdl->cmdData.HoldMod == 1 )
	{ /* 急速停止 */
		dpos = 0;
		cmdOut->Mtype = MNONE;
	}
	else if( hdl->cmdData.HoldMod == 2 )
	{ /* 減速停止2 */
		cmdOut->Mtype = MHOLD;
		cmdOut->Ctrl = MG_HOLD_CTRL2;
		dpos = MtgGetDctDistance( &hdl->mtgHdl, hdl->mtgHdl.Cspd, cmdOut->Ctrl );
	}
	else
	{ /* 減速停止1 */
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
 EX_FEED指令作成処理
****************************************************************************/
static	BOOL mtcMakeExFeedRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;

	if( hdl->cmdData.CmdRstReq )
	{
		hdl->seqState = 1;	/* リセット時は、最初から */
	}

	if( hdl->seqState == 0 )
	{ /* ラッチ完了状態 */
		cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	}
	else if( hdl->seqState == 1 )
	{ /* EX_FEED開始 */
		/* ラッチキャンセル */
		MtcCancelLatch( hdl, EXFEED_LTMD, MTLATCH_NUM );
		if( hdl->cmdData.TSpeed < 0 )
		{ /* 負方向 */
			cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, -DISTANCE_MAX );
		}
		else
		{ /* 正方向 */
			cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, DISTANCE_MAX );
		}
		if(hdl->cmdData.MtLatchReq != FALSE )
		{ /* ラッチ要求あり */
			/* ラッチ要求発行 */
			MtcReqLatch( hdl, EXFEED_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
			hdl->seqState++;
		}
	}
	else if( hdl->seqState == 2 )
	{ /* ラッチ待ち状態 */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == EXFEED_LTMD) )
		{ /* ラッチ完了 */
			tpos = hdl->psnHdl.Lpos[MTLATCH_NUM] + hdl->mtPrm.MtSeqPrm.ex_posing_dist;
			cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, tpos );
			hdl->seqState = 0;
		}
		else
		{ /* ラッチ未完 */
			if( hdl->cmdData.TSpeed < 0 )
			{ /* 負方向 */
				cmdOut->DposIn = PsnSetTPOSDist( &hdl->psnHdl, -DISTANCE_MAX );
			}
			else
			{ /* 正方向 */
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
 EX_POSING指令作成処理
****************************************************************************/
static	BOOL mtcMakeExPosingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;

	if( hdl->cmdData.CmdRstReq )
	{
		hdl->seqState = 1;	/* リセット時は、最初から */
	}

	if( hdl->seqState == 0 )
	{ /* ラッチ完了状態 */
		cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	}
	else if( hdl->seqState == 1 )
	{ /* EX_POSING開始 */
		/* ラッチキャンセル */
		MtcCancelLatch( hdl, EXPOSING_LTMD, MTLATCH_NUM );
		cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		if(hdl->cmdData.MtLatchReq != FALSE )
		{ /* ラッチ要求あり */
			/* ラッチ要求発行 */
			MtcReqLatch( hdl, EXPOSING_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
			hdl->seqState++;
		}
	}
	else if( hdl->seqState == 2 )
	{ /* ラッチ待ち状態 */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == EXPOSING_LTMD) )
		{ /* ラッチ完了 */
			tpos = hdl->psnHdl.Lpos[MTLATCH_NUM] + hdl->mtPrm.MtSeqPrm.ex_posing_dist;
			cmdOut->DposIn = PsnSetTPOS(&hdl->psnHdl, tpos);
			hdl->seqState = 0;
		}
		else
		{ /* ラッチ未完 */
			cmdOut->DposIn = PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
			if( cmdOut->DposIn == 0 )
			{ /* 位置決め完了なら終了 */
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
 HOMING指令作成処理（DEC信号あり）
****************************************************************************/
static	BOOL mtcMakeHomingRefWithDec( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;
	LONG pos_err;
	PSN_HNDL *psn_hdl_ptr;

	psn_hdl_ptr = &hdl->psnHdl;

	if( hdl->mtPrm.ProfPrm.zretLtCancel==ZRET_LATCH_IS_CANCEL )
	{ /* ZRETラッチキャンセルモードなら seqState=0にリセット */
		if( hdl->cmdData.CmdRstReq )
		{
			/* リセット時は、最初から */
			hdl->seqState = 0;
		}
	}

	switch( hdl->seqState )
	{
	case 0:	/* 原点復帰完了状態 */
		if( !hdl->cmdData.CmdRstReq )
		{ /* リセット要求がなければ、指令=0 */
			cmdOut->DposIn = 0;
			cmdOut->Tspd = 0;
			break;
		}
		MtcCancelLatch( hdl, ZRET_LTMD, MTLATCH_NUM );		/* ラッチキャンセル */
		hdl->seqState = 1;
	case 1:	/* HOMING開始(DEC=ON待ち状態) */
		psn_hdl_ptr->RefEnable = FALSE;						/* リファレンス点クリア */
		cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
		cmdOut->Tspd = hdl->cmdData.TSpeed;					/* TPOS,TSPEEDセット */

		if( 0 != (hdl->svRsp->RspSigBit & DEC_SIG_BITMSK) )
		{ /* DEC=ON */
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* アプローチ速度1へ */
			hdl->seqState = 2;								/* DEC=OFF待ちへ */
		}
		break;
	case 2:	/* DEC=OFF待ち状態 */
		cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;			/* TPOS,TSPEED=アプローチ速度1セット */

		if( 0 == (hdl->svRsp->RspSigBit & DEC_SIG_BITMSK) )
		{ /* DEC=OFF */
			if(hdl->cmdData.MtLatchReq != FALSE )
			{ /* ラッチ要求あり */
				/* ラッチ要求発行 */
				MtcReqLatch( hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
				hdl->seqState = 3;							/* ラッチ完了待ちへ */
			}
		}
		break;
	case 3:	/* ラッチ完了待ち状態 */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == ZRET_LTMD) )
		{ /* ラッチ完了 */ /* MECHATROLINK-2 用に以下の処理を実施 */
			if( (hdl->mtPrm.MtSeqPrm.zret_dir < 0) &&
				(hdl->mtPrm.ProfPrm.zretDistDir == ZRET_DIST_DIR_IS_RELATIVE) )
			{
				tpos = -hdl->mtPrm.MtSeqPrm.zret_dist;		/*最終走行距離符号逆転*/
			}
			else
			{
				tpos = hdl->mtPrm.MtSeqPrm.zret_dist;
			}

			tpos = psn_hdl_ptr->Lpos[MTLATCH_NUM] + tpos;	/* 最終位置セット */
			cmdOut->DposIn = PsnSetTPOS( psn_hdl_ptr, tpos );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;		/* アプローチ速度2へ */
			hdl->seqState = 4;								/* 位置決め完了待ちへ */
		}
		else
		{ /* ラッチ未完 */
			cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* アプローチ速度1 */
		}
		break;
	case 4:	/* 位置決め完了待ち状態 */
		cmdOut->DposIn = psn_hdl_ptr->Tpos - psn_hdl_ptr->Ipos;
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			/* アプローチ速度2 */

		pos_err = psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos;
		if( (psn_hdl_ptr->Den == TRUE) && ((ULONG)MlibLABS(pos_err) <= psn_hdl_ptr->Prm->psetWidth) )
		{ /* DEN=1 && (Tpos-Apos)<=位置決め完了幅 */
			/* 位置決め完了 */
			psn_hdl_ptr->RefEnable = TRUE;					/* リファレンス点有効 */
			PsnSetCordinate( psn_hdl_ptr, 0, 0 );			/* 座標系0リセット */
			if(hdl->mtPrm.ProfPrm.zretLtCancel==ZRET_LATCH_IS_NOCANCEL)
			{ /* ZRETラッチキャンセル無効モードでラッチキャンセル実行 */
				/* ラッチキャンセル */
				MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
			}
			hdl->seqState = 0;								/* 原点復帰完了状態へ */
		}
		break;
	}
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 HOMING指令作成処理（DEC信号なし）
****************************************************************************/
static	BOOL mtcMakeHomingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG tpos;
	LONG pos_err;
	PSN_HNDL *psn_hdl_ptr;

	psn_hdl_ptr = &hdl->psnHdl;

	if( hdl->cmdData.CmdRstReq )
	{
		hdl->seqState = 1;									/* リセット時は、最初から */
	}

	switch( hdl->seqState )
	{
	case 0:	/* 原点復帰完了状態 */
		cmdOut->DposIn = 0;
		cmdOut->Tspd = 0;
		break;
	case 1:	/* アプローチ状態 */
		MtcCancelLatch(hdl, ZRET_LTMD, MTLATCH_NUM);		/* ラッチキャンセル */
		psn_hdl_ptr->RefEnable = FALSE;						/* リファレンス点クリア */
		cmdOut->DposIn = PsnSetTPOSDist(psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir);
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;			/* TPOS,TSPEED=アプローチ速度1セット */

		if(hdl->cmdData.MtLatchReq != FALSE )
		{ /* ラッチ要求あり */
			/* ラッチ要求発行 */
			MtcReqLatch( hdl, ZRET_LTMD, hdl->cmdData.LatchSgnl, MTLATCH_NUM );
			hdl->seqState = 2;								/* ラッチ完了待ちへ */
		}
		break;
	case 2:	/* ラッチ完了待ち状態 */
		if( hdl->latch[MTLATCH_NUM].Cmp && (hdl->latch[MTLATCH_NUM].Mode == ZRET_LTMD) )
		{ /* ラッチ完了 */
			/* 最終位置セット */
			tpos = psn_hdl_ptr->Lpos[MTLATCH_NUM] + hdl->mtPrm.MtSeqPrm.zret_dist;

			cmdOut->DposIn = PsnSetTPOS( psn_hdl_ptr, tpos );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;		/* アプローチ速度2へ */
			hdl->seqState = 3;								/* 位置決め完了待ちへ */
		}
		else
		{ /* ラッチ未完 */
			cmdOut->DposIn = PsnSetTPOSDist( psn_hdl_ptr, hdl->mtPrm.MtSeqPrm.zret_dir );
			cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp1;		/* アプローチ速度1 */
		}
		break;
	case 3:	/* 位置決め完了待ち状態 */
		cmdOut->DposIn = psn_hdl_ptr->Tpos - psn_hdl_ptr->Ipos;
		cmdOut->Tspd = hdl->mtPrm.MtSeqPrm.zretsp2;			/* アプローチ速度2 */

		pos_err = psn_hdl_ptr->Tpos - psn_hdl_ptr->Apos;
		if( (psn_hdl_ptr->Den == TRUE) && ((ULONG)MlibLABS(pos_err) <= psn_hdl_ptr->Prm->psetWidth) )
		{ /* DEN=1 && (Tpos-Apos)<=位置決め完了幅 */
			/* 位置決め完了 */
			psn_hdl_ptr->RefEnable = TRUE;					/* リファレンス点有効 */
			PsnSetCordinate( psn_hdl_ptr, 0, 0 );			/* 座標系0リセット */
			hdl->seqState = 0;								/* 原点復帰完了状態へ */
		}
		break;
	}
	cmdOut->Mtype = MPOSING;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	return FALSE;
}


/****************************************************************************
 VELCTRL指令作成処理
****************************************************************************/
static	BOOL mtcMakeVelRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MVELCTRL;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* フィルタクリア */
	AcfClear( &hdl->acfHdl );				/* フィルタクリア */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOSリセット */
	hdl->psnHdl.Tpos = hdl->psnHdl.Ipos;
	return FALSE;
}


/****************************************************************************
 VELCTRL指令作成処理（加減速度指定あり）
****************************************************************************/
static	BOOL mtcMakeVelWithAccrRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MVELCTRL_WITHACCR;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* フィルタクリア */
	AcfClear( &hdl->acfHdl );				/* フィルタクリア */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOSリセット */
	hdl->psnHdl.Tpos = hdl->psnHdl.Ipos;

	return FALSE;
}


/****************************************************************************
 TRQCTRL指令作成処理
****************************************************************************/
static	BOOL mtcMakeTrqRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	cmdOut->Mtype = MTRQCTRL;
	cmdOut->DposIn = 0;
	cmdOut->Tspd = 0;
	cmdOut->Vref = hdl->cmdData.Velocity;
	cmdOut->Tref = hdl->cmdData.Trq;

	hdl->cmdData.AccFilSel = ACF_SW_NOFIL;	/* フィルタクリア */
	AcfClear( &hdl->acfHdl );				/* フィルタクリア */
	hdl->psnHdl.Ipos = hdl->psnHdl.Pos;		/* IPOSリセット */
	hdl->psnHdl.Tpos = hdl->psnHdl.Ipos;

	return FALSE;
}


/***************************************************************************
 SPOSING指令作成処理
****************************************************************************/
static	BOOL mtcMakeSposingRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	MG_SPOSING	*sposing;	/* SPOSING用パラメータ	*/
	LONG	state;

	sposing = &hdl->mtgHdl.sposing;
	state = sposing->State;
	
	if( hdl->cmdData.CmdRstReq )
	{
		PsnSetTPOS( &hdl->psnHdl, hdl->cmdData.TPos );
		cmdOut->Tspd = hdl->cmdData.TSpeed;
	}

	/* コマンド切り替え時の初期化 */
	if(cmdOut->Mtype != MSPOSING)
	{
		sposing->LastTargetSpd  = 0;		/* SPOSING前回目標速度		*/
		sposing->LastTargetPosition = 0;	/* SPOSING前回目標位置		*/
		sposing->LastDposIn = 0;			/* SPOSING前回残距離		*/
	}

	cmdOut->Mtype = MSPOSING;
	cmdOut->DposIn = hdl->psnHdl.Tpos - hdl->psnHdl.Ipos;
	cmdOut->Vref = 0;
	cmdOut->Tref = 0;

	/* 指令変更チェック				*/
	/* 目標速度が変更された場合		*/
	if(sposing->LastTargetSpd != hdl->cmdData.TSpeed)
	{
		/* 減速中でないときはSPOSINGリスタート	*/
		if((state != SPOSING_DEC_MODE1) && (state != SPOSING_DEC_MODE2))	
		{
			state = SPOSING_INI_MODE;
		}
		/* 減速中の場合は無視 */
		else
		{
			/* no operation */
		}
	}

	/* 目標位置が変更された場合 */
	if(sposing->LastTargetPosition != hdl->psnHdl.Tpos)
	{
		/* 減速中でないときはSPOSINGリスタート */
		if((state != SPOSING_DEC_MODE1) && (state != SPOSING_DEC_MODE2))	
		{
			state = SPOSING_INI_MODE;
		}
		/* 減速中のときは 変更後の移動方向が同じ && 残距離が増えたときリスタート */
		else
		{
			/* 目標位置を正方向に変更 */
			if( sposing->LastDposIn > 0 )
			{
				if(sposing->LastDposIn < cmdOut->DposIn)
				{
					state = SPOSING_INI_MODE;		
				}
			}
			/* 目標位置を負方向に変更 */
			else if( sposing->LastDposIn < 0 )
			{
				if(sposing->LastDposIn > cmdOut->DposIn)
				{
					state = SPOSING_INI_MODE;		
				}
			}
			/* sposing->LastDposIn=0のとき(存在しない) */
			else
			{
				state = SPOSING_INI_MODE;		
			}
		}
	}
	
	/* 今回値を保持する */
	if(state == SPOSING_INI_MODE)
	{
		sposing->LastTargetSpd = hdl->cmdData.TSpeed;		/* SPOSING前回目標速度	*/
		sposing->LastTargetPosition = hdl->psnHdl.Tpos;		/* SPOSING前回目標位置	*/
		sposing->LastDposIn = cmdOut->DposIn;				/* SPOSING前回残距離	*/
	}
	
	sposing->State = state;
	
	return FALSE;
}


/****************************************************************************
 SHOLD指令作成処理
****************************************************************************/
static	BOOL mtcMakeSholdRef( MT_HNDL *hdl, MG_INOUT *cmdOut )
{
	LONG dpos;
	
	if( hdl->cmdData.HoldMod == 1 )
	{ /* 急速停止 */
		dpos = 0;
		cmdOut->Mtype = MNONE;
	}
	else if( hdl->cmdData.HoldMod == 2 )
	{ /* 減速停止2 */
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
	{ /* 減速停止1 */
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
