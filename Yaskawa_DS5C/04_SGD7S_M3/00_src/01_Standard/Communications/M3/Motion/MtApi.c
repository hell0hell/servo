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
/* 概要		：指令データのクランプ機能付きゲイン乗算						*/
/****************************************************************************/
/* ULONG inData			入力値（指令データ）								*/
/* LONG  maxData		最大値												*/
/* LONG  kx				ノーマライズ定数									*/
/* LONG* outData		出力値（計算結果）									*/
/****************************************************************************/
/* ret = MCI_SUCCESS		正常終了										*/
/* ret = DATA_SET_WARNG6	データ設定警告(データクランプ動作)				*/
/****************************************************************************//* <S06D> */
static INT32 mciMulgain32WithClampData( ULONG inData, LONG maxData, KSGAIN kx, LONG* outData )
{
	INT32 ret;

	ret = MCI_SUCCESS;

	if( inData <= 0x7FFFFFFF )
	{/* inData <= 0x7FFFFFFF のときは通信データを指令 */
		*outData = MlibMulgain32( (LONG)inData, kx );
	}
	else if( inData == 0xFFFFFFFF )
	{/* inData = 0xFFFFFFFF のときは最大値を指令 */
		*outData = maxData;
	}
	else
	{/* inData > 0x7FFFFFFF のときは inData = 0x7FFFFFFF を指令 */
		*outData = MlibMulgain32( 0x7FFFFFFF, kx );
		ret = MCI_DATA_CLAMP;
	}
	return ret;
}

/***************************************************************************
 加減速単位変換処理(通信データでの加速度指令)
	inRate		加速度指令[単位：ref/cycs^cyc]
	maxData		最大値
	*altspd		加速度切り替え速度
	*outRate	加速度指令[単位：***.*ref/cyc^cyc]
****************************************************************************//* <S06D> */
__inline static	LONG mciChkACCR(LONG inRate, LONG maxData, LONG *outRate, LONG *altspd)
{
	LONG ret = MCI_SUCCESS;

	if(inRate <= 0)
	{/* 加速度 <= 0の場合は異常終了 */
		inRate = 1;
		ret = MCI_ACCR_ERR;
	}
	else if(inRate > maxData)
	{/* 加速度 > maxDataの場合は異常警告 */
		inRate = maxData;
		ret = MCI_ACCR_ERR;
	}
	*altspd = 0;	/* 加速度切り替え速度 = 0 */
	*outRate = inRate;

	return ret;
}


/****************************************************************************
 加減速指令データ設定
****************************************************************************//* <S004> */
//__inline static	VOID mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR CtrlMode)/* <S06D> */
static	LONG mciSetAccRate(MT_HNDL* hdl, MCI_CMD* cmd, UCHAR CtrlMode)/* <S06D> */
{
	LONG ret,ret1,ret2;
	LONG work, dummy;

	ret = MCI_SUCCESS;
	
	if(cmd->RateType == RATE_CMD)
	{/* 指令データに加減速指令がある場合(M3標準サーボプロファイル) */
		if(CtrlMode == MT_POS_CTRLMD)
		{/* 位置制御の場合 */
#if 0/* <S06D>:SGDVの仕様に合わせる */
			/* ACCRセット */
			hdl->mtPrm.CmdRate.accrate2 = MlibMulgain30U(cmd->AccRate,
											hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2);
			/* DECRセット */
			hdl->mtPrm.CmdRate.decrate2 = MlibMulgain30U(cmd->DecRate,
											hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2);
#else
			/* 加速度チェック	*/
			mciMulgain32WithClampData(cmd->AccRate, MT_MAX_ACCR, hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2, &work);
			ret1 = mciChkACCR(work, MT_MAX_ACCR, &hdl->mtPrm.CmdRate.accrate2, &hdl->mtPrm.CmdRate.altspd_a);
			/* 減速度チェック	*/
			mciMulgain32WithClampData(cmd->DecRate, MT_MAX_ACCR, hdl->mtPrm.BasePrm.m3ConvUnit.m3AccUnitToCyc2, &work);
			ret2 = mciChkACCR(work, MT_MAX_ACCR, &hdl->mtPrm.CmdRate.decrate2, &hdl->mtPrm.CmdRate.altspd_d);
			/* 加減速度結果結合	*/
			ret = ret1 != MCI_SUCCESS ? ret1 : ret2;
#endif/* <S06D>:SGDVの仕様に合わせる */
		}
		else if(CtrlMode == MT_VEL_CTRLMD)
		{/* 速度制御の場合 */
#if 0/* <S06D>:SGDVの仕様に合わせる */
			/* ACCRセット */
			hdl->mtPrm.CmdRate.accr_vel = MlibMulgain32U(cmd->AccRate, 1);
			/* DECRセット */
			hdl->mtPrm.CmdRate.decr_vel = MlibMulgain32U(cmd->DecRate, 1);
#else
			/* 加速度チェック	*/
			mciMulgain32WithClampData(cmd->AccRate, MT_MAX_ACCR_VEL, 1, &work);
			ret1 = mciChkACCR(work, MT_MAX_ACCR_VEL, &hdl->mtPrm.CmdRate.accr_vel, &dummy);
			/* 減速度チェック	*/
			mciMulgain32WithClampData(cmd->DecRate, MT_MAX_ACCR_VEL, 1, &work);
			ret2 = mciChkACCR(work, MT_MAX_ACCR_VEL, &hdl->mtPrm.CmdRate.decr_vel, &dummy);
			ret = ret1 != MCI_SUCCESS ? ret1 : ret2;
#endif/* <S06D>:SGDVの仕様に合わせる */
		}
		hdl->cmdData.CmdRateOn = TRUE;
	}
	else if((hdl->mtPrm.BasePrm.SetCmdAccRate != NULL) && (cmd->RateType == RATE_ACC_TIME))
	{/* 有効コマンド && 加減速指令パラメータSW有効(加減速時間) */
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
	{/* パラメータによる加減速設定 */
		hdl->cmdData.CmdRateOn = FALSE;
	}
	
	return ret;/* <S06D>:チェック結果を戻す */
}
/****************************************************************************
 TLIMチェック
 TLIMが設定範囲外の場合は、TLIMに最大値を設定して、エラーコードを返す
****************************************************************************//* <S06D> */
INT32 mciChkTLIM(MT_HNDL* hdl, MCI_CMD* cmd, LONG* tlim)
{
	INT32 ret;

	ret = MCI_SUCCESS;
	
	ret = mciMulgain32WithClampData(cmd->TrqLimitP, MT_MAXTRQ_30, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax, tlim);
	/* TLIMチェック */
	if(hdl->mtPrm.BasePrm.TrefModePos == TREFMODE_TFF_TLIM)
	{/* TLIM有効 */
		if(*tlim > MT_MAXTRQ_30)
		{
			*tlim = MT_MAXTRQ_30;
			ret = MCI_DATA_CLAMP;
		}
	}
	return ret;
}

/****************************************************************************
 INTERPOLATE指令設定
****************************************************************************/
INT32 MciSetInterpolateCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  dpos, tlim;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
	/* 指令速度チェック */
	dpos = cmd->TPos - hdl->psnHdl.Ipos;
	if( (ULONG)MlibLABS(dpos) > hdl->mtPrm.BasePrm.seg_max_ref )
	{
		return MCI_SPD_ERR;
	}
#endif
	/* VFFセット */	
#if (FLOAT_USE==TRUE)
	hdl->cmdData.Velocity = (LONG)( (float)cmd->Velocity * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
	/* TFFセット */
	hdl->cmdData.Trq = (LONG)( (float)cmd->Trq * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/* TRQ LIMITセット */
//	tlim = (ULONG)( (float)cmd->TrqLimitP * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#else	
	hdl->cmdData.Velocity = MlibMulgain32( cmd->Velocity, 
								hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
	/* TFFセット */
	hdl->cmdData.Trq = MlibMulgain32( cmd->Trq, 
								hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/* TRQ LIMITセット */
//	tlim = MlibMulgain32U( cmd->TrqLimitP, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
#endif /* FLOAT_USE */

	/*---------------------------------------------------------------------*/
/* VFFチェック *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Velocity) > hdl->mtPrm.BasePrm.osVel)
	{
		hdl->cmdData.Velocity = MlibLimitul(hdl->cmdData.Velocity, 
							hdl->mtPrm.BasePrm.osVel, -(LONG)hdl->mtPrm.BasePrm.osVel);
		ret = MCI_DATA_CLAMP;
	}
/* TFFチェック *//* <S06D> */
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
	/* MOTION LTモードクリア */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.Mtype = MTYPE_INTERPOL;		/* MTYPEセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */

	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット */
	
//	return MCI_SUCCESS;
	return ret;/* <S11C> */
}



/****************************************************************************
 POSING指令設定
****************************************************************************/
INT32 MciSetPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG tlim;
	LONG tspd;
	
	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif
	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* 指令速度チェック *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	/*---------------------------------------------------------------------*/
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	/* MOTION LTモードクリア */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_POSING;			/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */

	hdl->cmdData.TPos = cmd->TPos;				/* TPOSセット */
	hdl->cmdData.Trq = 0;						/* TFFセット */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}





/****************************************************************************
 SPOSING指令設定
****************************************************************************/
INT32 MciSetSposingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif
	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else
	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#endif /* FLOAT_USE */
	/* 指令速度チェック *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	
	/*---------------------------------------------------------------------*/
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	/*---------------------------------------------------------------------*/
	/* SRATIOセット */
	hdl->mtPrm.CmdRate.Sratio = cmd->Sratio;

	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	/* MOTION LTモードクリア */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype     = MTYPE_SPOSING;		/* MTYPEセット */
	hdl->cmdData.CtrlMode  = MT_POS_CTRLMD;		/* 位置制御モードセット 		*/
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット 			*/

	hdl->cmdData.TPos      = cmd->TPos;			/* TPOSセット  */
	hdl->cmdData.Trq	   = 0;					/* TFFセット */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}




/****************************************************************************
 EX_POSING指令設定
****************************************************************************/
INT32 MciSetExPosingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */
	/* 指令速度チェック *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	
	/*---------------------------------------------------------------------*/
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	hdl->latch[MTLATCH_NUM].Mode = EXPOSING_LTMD;	/* MOTION LT MODEセット */
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ラッチ信号セット */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* モーションラッチ要求セット */

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_EXPOSING;			/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = cmd->RstReq;			/* コマンド更新セット */

	hdl->cmdData.TPos = cmd->TPos;					/* TPOSセット */
	hdl->cmdData.Trq  = 0;							/* TFFセット */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 HOMING指令設定
****************************************************************************/
INT32 MciSetHomingCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* 指令速度チェック *//* <S06D> */
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
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ラッチ信号セット */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* モーションラッチ要求セット */
	hdl->latch[MTLATCH_NUM].Mode = ZRET_LTMD;		/* MOTION LT MODEセット */

	MPrmSetZretDir(&hdl->mtPrm, cmd->HomeDir);		/* HOME DIRセット */

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_HOMING;			/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;		/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = cmd->RstReq;		/* コマンド更新セット */
	hdl->cmdData.Trq	   = 0;					/* TFFセット */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 HOMING指令設定(DEC信号あり)
****************************************************************************/
INT32 MciSetHomingCmdWithDec( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32U( (ULONG)cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* 指令速度チェック *//* <S06D> */
	if((tspd < 0) || (tspd > (LONG)hdl->mtPrm.BasePrm.PosingSpdMax))
	{
		return MCI_SPD_ERR;
	}
	else
	{
		hdl->cmdData.TSpeed = tspd;
	}
	
	/*---------------------------------------------------------------------*/
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ラッチ信号セット */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* モーションラッチ要求セット */
	hdl->latch[MTLATCH_NUM].Mode = ZRET_LTMD;		/* MOTION LT MODEセット */

	MPrmSetZretDir(&hdl->mtPrm, cmd->HomeDir);		/* HOME DIRセット */

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_HOMING_WITH_DEC;		/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = cmd->RstReq;			/* コマンド更新セット */

	hdl->cmdData.Trq	   = 0;						/* TFFセット */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 FEED指令設定
****************************************************************************/
INT32 MciSetFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else
	tspd = MlibMulgain32( cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* 指令速度チェック *//* <S06D> */
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
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	/* MOTION LTモードクリア */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_FEED;				/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;					/* コマンド更新セット */

	hdl->cmdData.Trq	   = 0;						/* TFFセット */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 EX_FEED指令設定
****************************************************************************/
INT32 MciSetExFeedCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;
	LONG tspd;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* RUN状態チェック */
	if( hdl->mtState != MSTAT_RUN )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
	/* TSPDセット *//* <S06D>:一旦ローカル変数に格納し、ﾃﾞｰﾀﾁｪｯｸを実施する様に変更 */
#if (FLOAT_USE==TRUE)
	tspd = ( (float)cmd->TSpeed * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );
#else

	tspd = MlibMulgain32( cmd->TSpeed, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToRef );/* <S06D> */
#endif /* FLOAT_USE */

	/* 指令速度チェック *//* <S06D> */
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
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_POS_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	hdl->cmdData.LatchSgnl = cmd->LatchSgnl;		/* ラッチ信号セット */
	hdl->cmdData.MtLatchReq = cmd->MtLatchReq;		/* モーションラッチ要求セット */
	hdl->latch[MTLATCH_NUM].Mode = EXFEED_LTMD;		/* MOTION LT MODEセット */

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_EXFEED;				/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;			/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = cmd->RstReq;			/* コマンド更新セット */

	hdl->cmdData.Trq	   = 0;						/* TFFセット */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}

#if 0
/****************************************************************************
 HOLD指令設定
****************************************************************************/
INT32 MciSetHoldCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;

	ret = MCI_SUCCESS;

	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}

/* TLIMチェック */
	ret = mciChkTLIM( hdl, cmd, ret );

/* モーション指令データセット */
	hdl->cmdData.Mtype = MTYPE_HOLD;							/* MTYPEセット */
	hdl->cmdData.HoldMod = cmd->HoldMod;						/* Holdモードセット */
	hdl->cmdData.TSpeed = 0;									/* TSPDセット */
	hdl->cmdData.CtrlMode = MT_POS_CTRLMD;						/* 位置制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;								/* コマンド更新セット */
	hdl->cmdData.Trq	   = 0;									/* TFFセット */
	hdl->svCmd->ExtCmd1.dwVal = cmd->TrqLimitP >> 6;			/* TRQ LIMITセット */
	hdl->svCmd->ExtCmd2.dwVal = cmd->TrqLimitN >> 6;			/* TRQ LIMITセット */

	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) && 
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );			/* MOTION LTモードクリア */
	}
	return ret;	
}
#endif

#if 0
/****************************************************************************
 VELCTRL指令設定
****************************************************************************/
INT32 MciSetVelCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	LONG	vref;
	INT32	ret;

	vref  = cmd->Velocity;
	ret = MCI_SUCCESS;

	/* モーション状態チェック */
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
	/* TFFチェック */
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
	/* VREFチェック */
	if( (ULONG)MlibLABS(vref) > MT_OSVEL_30 )
	{
		ret = MCI_DATA_CLAMP;
		/* M-2互換プロファイル用処理なのでVREF値のクランプは行わない */
	}
	/* TLIMチェック */
	ret = mciChkTLIM( hdl, cmd, ret );

	/* モーション指令データセット */
	hdl->cmdData.Mtype = MTYPE_VELCTRL;						/* MTYPEセット */
	hdl->cmdData.Velocity =	vref;							/* Vrefセット */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;					/* 速度制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;							/* コマンド更新セット */
	hdl->cmdData.Trq	   = cmd->Trq;							/* TFFセット */
	hdl->svCmd->ExtCmd1.dwVal = cmd->TrqLimitP >> 6;			/* TRQ LIMITセット */
	hdl->svCmd->ExtCmd2.dwVal = cmd->TrqLimitN >> 6;			/* TRQ LIMITセット */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) && 
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );		/* MOTION LTモードクリア */
	}

	return ret;
}
#endif

/****************************************************************************
 VELCTRL指令設定（加減速度指令あり）
****************************************************************************/
INT32 MciSetVelCmdWithAccr( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;
	LONG  tlim;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* モーション状態チェック */
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
#endif

	/*---------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	/* TFFセット */
	hdl->cmdData.Trq  = ( (float)cmd->Trq * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/* TSPDセット */
	hdl->cmdData.Velocity = ( (float)cmd->Velocity * hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#else
	/* TFFセット */
	hdl->cmdData.Trq  = MlibMulgain32( cmd->Trq, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );
	
	/* TSPDセット */
	hdl->cmdData.Velocity = MlibMulgain32( cmd->Velocity, 
							hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#endif /* FLOAT_USE */
/* TFFチェック *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Trq) > MT_MAXTRQ_30)
	{
		hdl->cmdData.Trq = MlibLimitul(hdl->cmdData.Trq, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
/* VREFチェック *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Velocity) > MT_OSVEL_30)
	{
		hdl->cmdData.Velocity = MlibLimitul(hdl->cmdData.Velocity, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
	/*---------------------------------------------------------------------*/
	/* ACCR,DECRセット *//* <S004> *//* <S06D>:加減速度のデータチェック追加 */
	ret = mciSetAccRate(hdl, cmd, MT_VEL_CTRLMD);
	if(ret != MCI_SUCCESS)
	{	
		return ret;
	}
	
	/*---------------------------------------------------------------------*/
	/* TRQ LIMITセット */
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
	/* MOTION LTモードクリア */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}

	/* FB速度セット(制御切替時指令用) */
	hdl->outData.FbSpeed = hdl->svRsp->Mon3;

	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_VELCTRL_WITH_ACCR;	/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_VEL_CTRLMD;			/* 速度制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;					/* コマンド更新セット */

//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 TRQCTRL指令設定
****************************************************************************/
INT32 MciSetTrqCmd( MT_HNDL *hdl, MCI_CMD *cmd )
{
	INT32 ret;

	ret = MCI_SUCCESS;

	/*---------------------------------------------------------------------*/
#ifdef MT_STATE_CHECK
	/* モーション状態チェック */
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
#endif

#if (FLOAT_USE==TRUE)
	/*---------------------------------------------------------------------*/
	/* TREFセット */
	hdl->cmdData.Trq  = (LONG)( (float)cmd->Trq * hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/*---------------------------------------------------------------------*/
	/* VLIMセット */
	hdl->cmdData.Velocity = (ULONG)( (float)cmd->Velocity *
								hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#else
	/*---------------------------------------------------------------------*/
	/* TREFセット */
	hdl->cmdData.Trq  = MlibMulgain32( cmd->Trq, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqUnitToMax );

	/*---------------------------------------------------------------------*/
	/* VLIMセット */
	hdl->cmdData.Velocity = MlibMulgain32U( (ULONG)cmd->Velocity, 
								hdl->mtPrm.BasePrm.m3ConvUnit.m3SpdUnitToMax );
#endif /* FLOAT_USE */
/* VLIMチェック *//* <S06D> */
	if(hdl->mtPrm.BasePrm.vLimModeTrq == VLIM_MODE_ENABLE)
	{
		if(hdl->cmdData.Velocity > MT_OSVEL_30)
		{
			hdl->cmdData.Velocity = MT_OSVEL_30;
			ret = MCI_DATA_CLAMP;
		}
	}
/* TREFチェック *//* <S06D> */
	if((ULONG)MlibLABS(hdl->cmdData.Trq) > MT_MAXTRQ_30)
	{
		hdl->cmdData.Trq = MlibLimitul(hdl->cmdData.Trq, 0x40000000, 0xC0000000);
		ret = MCI_DATA_CLAMP;
	}
	
	/*---------------------------------------------------------------------*/
	/* MOTION LTモードクリア */
	if( (hdl->latch[MTLATCH_NUM].Mode != MODAL_LTMD) &&
		(hdl->latch[MTLATCH_NUM].Mode != MOTION_MDL_LTMD) )
	{
		MtcCancelLatch( hdl, NONE_LTMD, MTLATCH_NUM );
	}
	
	/*---------------------------------------------------------------------*/
	/* モーション指令データセット *//* <S06D>:処理の位置を変更 */
	hdl->cmdData.Mtype = MTYPE_TRQCTRL;			/* MTYPEセット */
	hdl->cmdData.CtrlMode = MT_TRQ_CTRLMD;		/* トルク制御モードセット */
	hdl->cmdData.CmdRstReq = TRUE;				/* コマンド更新セット */
	hdl->svCmd->ExtCmd1.dwVal = MAX_TRQ24;		/* TRQ LIMITセット */
	hdl->svCmd->ExtCmd2.dwVal = MAX_TRQ24;		/* TRQ LIMITセット */
	
//	return MCI_SUCCESS;/* <S06D> */
	return ret;/* <S06D> */
}


/****************************************************************************
 CMD_PAUSE 指令設定
****************************************************************************/
// ここでは、Mtypeによるチェックはしない。(本関数の呼び出し元でチェックを行う)
// CMD_PAUSE 指令のときは、加減速度が異常でも実行する
INT32 MciSetCmdPause( MT_HNDL *hdl, MCI_CMD *cmd )
{
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}

	if( hdl->cmdData.CtrlMode == MT_POS_CTRLMD )
	{ /* 位置制御 */
		/* 加速度、MTYPE、TLIM は今までの設定を利用する */
		if( cmd->HoldMod == HOLD_MODE_RAPID_STOP )
		{ /* 急速停止の場合は、最大減速度で停止 */
			hdl->mtPrm.CmdRate.altspd_d = 0;
			hdl->mtPrm.CmdRate.decrate2 = MT_MAX_ACCR;
			hdl->cmdData.CmdRateOn = TRUE;			/* 加減速指令有効 */
		}
		hdl->cmdData.TSpeed = 0;					/* TSPDセット */
	}
	else if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
	{ /* 速度制御 */
		/* 加速度、MTYPE、TLIM は今までの設定を利用する */
		if( cmd->HoldMod == HOLD_MODE_RAPID_STOP )
		{ /* 急速停止の場合は、最大減速度で停止 @CHECK@ 最大限速度はOKか？ */
			hdl->cmdData.Mtype = MTYPE_VELCTRL;		/* MTYPEセット */
		}
		hdl->cmdData.Velocity =	0;					/* Vrefセット */
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 CMD_CANCEL 指令設定
****************************************************************************/
// CMD_CANCEL 指令のときは、加減速度が異常でも実行する
INT32 MciSetCmdCancel( MT_HNDL *hdl, MCI_CMD *cmd )
{
	if( hdl->mtState == MSTAT_DISABLE )
	{
		return MCI_CNDTN_ERR;
	}
	
	if( hdl->cmdData.CtrlMode == MT_POS_CTRLMD )
	{ /* 位置制御 */
		/* 加速度、TLIM は今までの設定を利用する */
		if(hdl->cmdData.Mtype == MTYPE_SPOSING)				/* MTYPEセット */
		{
			hdl->cmdData.Mtype = MTYPE_SHOLD;
		}
		else
		{
			hdl->cmdData.Mtype = MTYPE_HOLD;
		}
		hdl->cmdData.HoldMod = cmd->HoldMod;				/* Holdモードセット */
		hdl->cmdData.TSpeed = 0;							/* TSPDセット */
		hdl->cmdData.CmdRstReq = TRUE;						/* コマンド更新セット */
		if( cmd->HoldMod > HOLD_MODE_RAPID_STOP )
		{ /* STOPモードにReserve値が設定された場合は減速停止 */
			hdl->cmdData.HoldMod = HOLD_MODE_RSLOWDOWN_STOP;
		}
	}
	else if( hdl->cmdData.CtrlMode == MT_VEL_CTRLMD )
	{ /* 速度制御 */
		/* 加速度、MTYPE、TLIM は今までの設定を利用する */
		if( cmd->HoldMod == HOLD_MODE_RAPID_STOP )
		{ /* 急速停止の場合は、最大減速度で停止 最大減速度で停止 @CHECK@ 最大限速度はOKか？*/
			hdl->cmdData.Mtype = MTYPE_VELCTRL;				/* MTYPEセット */
		}
		hdl->cmdData.Velocity =	0;							/* Vrefセット */
		hdl->cmdData.CmdRstReq = TRUE;						/* コマンド更新セット */
	}
	return MCI_SUCCESS;	
}


/****************************************************************************
 移動コマンド一時停止指令時のアプローチ速度ゼロクリア設定
****************************************************************************/
// 本関数は(前回cmdPause == FALSE) && (今回cmdPause == TRUE) のときに実行する
VOID MciClearZretSpd( MT_HNDL *hdl )
{
	if( (hdl->cmdData.Mtype == MTYPE_HOMING) || (hdl->cmdData.Mtype == MTYPE_HOMING_WITH_DEC) )
	{ /* 原点復帰中のみ実行 */
		/* アプローチ速度をゼロに設定 */
		hdl->mtPrm.MtSeqPrm.zretsp1 = 0;
		hdl->mtPrm.MtSeqPrm.zretsp2 = 0;
	}
}


/****************************************************************************
 移動コマンド一時停止指令時のアプローチ速度復帰設定
****************************************************************************/
// 本関数は(前回cmdPause == TRUE) && (今回cmdPause == FALSE) のときに実行する
// 本関数はコマンドに関係なく実施する
VOID MciReturnZretSpd( MT_HNDL *hdl )
{
	/* バッファのアプローチ速度を復帰 */
	hdl->mtPrm.MtSeqPrm.zretsp1 = hdl->mtPrm.MtSeqPrm.zretsp1_org;
	hdl->mtPrm.MtSeqPrm.zretsp2 = hdl->mtPrm.MtSeqPrm.zretsp2_org;
}
#if 0 /* M2プロファイル対応 */
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
	
/* 設定した値を格納する	*/
	pMtHdl->svCmd->CmdCtrl.CmdCtrlBit = CmdCtrl;
	pMtHdl->svCmd->CmdSeqBit = CmdSeq;
}
#endif
/****************************************************************************
 モーション状態リセット
****************************************************************************/
INT32 MciResetMotion( MT_HNDL *hdl )
{
	INT32 rc;

	/* ローカル変数の初期化 */
	rc = MCI_WAIT_COMP;

	if( (hdl->mtState == MSTAT_READY)
		|| (hdl->mtState == MSTAT_WAITPG_RDY)
		|| (hdl->mtState == MSTAT_DISABLE) )
	{
//		MciSetOptionCmd( hdl, 0 );							/* Option指令クリア */
		MtcCancelLatch( hdl, NONE_LTMD, 0 );				/* ラッチ１クリア */
		MtcCancelLatch( hdl, NONE_LTMD, 1 );				/* ラッチ２クリア */
		PsnResetApos( &hdl->psnHdl, FALSE );				/* Positionリセット */
		hdl->svCmd->CmdCtrl.CmdCtrlBit &= ~(CLRPOSINTG_BIT);/* P_PPI指令クリア */
		hdl->cmdData.SensOn = FALSE;						/* SENS_OFF */
		hdl->cmdData.MtEenable = FALSE;						/* Motion無効 */

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
INT32 MciEnableState( MT_HNDL *hdl, BOOL enable )
{
	INT32 rc;

	/* ローカル変数の初期化 */
	rc = MCI_WAIT_COMP;

	if( enable )
	{ /* モーション有効要求ON */
		hdl->cmdData.MtEenable = TRUE;
		if( hdl->mtState != MSTAT_DISABLE )
		{
			rc = MCI_SUCCESS;		/* DISABLE状態でなければ完了 */
		}
	}
	else
	{ /* モーション有効要求OFF */
		if( (hdl->mtState == MSTAT_READY)
			|| (hdl->mtState == MSTAT_WAITPG_RDY)
			|| (hdl->mtState == MSTAT_DISABLE) )
		{
			hdl->cmdData.MtEenable = FALSE;
			if( hdl->mtState == MSTAT_DISABLE )
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

 【補足】M-RDY情報の使用について
  アブソリニアの磁極検出処理において、磁極情報をスケールに書き込む時に、
  M-RDY=FALSEとなり、A.95Aが発生する問題があったため、M-RDYは使用しない

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
		else if( 0 != (hdl->svRsp->RspSigBit & HWBB_SIG_BITMSK) )/* <S07C> */
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
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
INT32 MciRequestMtLatch( MT_HNDL *hdl, LONG mode, LONG sgnl, UCHAR i )
{
	if( (hdl->latch[i].Mode != mode) 					/* ラッチモード不一致 */
		|| !(hdl->latch[i].Cmp | hdl->latch[i].Req) )	/* ラッチなし */
	{
		MtcCancelLatch( hdl, mode, i );
		MtcReqLatch( hdl, mode, sgnl, i );
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 Modalラッチ要求発行
 mode=0:通常ラッチ、=1:連続ラッチ
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
INT32 MciRequestMdlLatch( MT_HNDL *hdl, UCHAR mode, LONG sgnl, LONG i )
{
	if( hdl->latch[i].Mode == NONE_LTMD )
	{ /* ラッチモードなし時のみ要求発行 */
		if( mode == 1 )
		{ /* シーケンシャルモード */
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
	{ /* 現在ラッチモード≠Modalラッチモードならばエラー */
		if( hdl->latch[i].Mode != MODAL_LTMD )
		{
			return MCI_LTCNFLCT_ERR;
		}
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 ラッチキャンセル
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
INT32 MciCancelLatch( MT_HNDL *hdl, LONG mode, LONG i )
{
	if( mode == MODAL_LTMD )
	{ /* Modalラッチキャンセル */
		if( (hdl->latch[i].Mode != NONE_LTMD)
			&& (hdl->latch[i].Mode != MODAL_LTMD) )
		{ /* ラッチモードあり && MODAL_LTMD以外は、エラー */
			return MCI_LTCNFLCT_ERR;
		}
		MtcCancelLatch( hdl, NONE_LTMD, i );
	}
	else if( mode == MOTION_MDL_LTMD )
	{ /* Motion Modalキャンセル */
		MtcCancelLatch( hdl, MOTION_MDL_LTMD, i );
	}
	else
	{ /* 通常 Motionキャンセル */
		if( (hdl->latch[i].Mode == MODAL_LTMD)
			|| ((hdl->latch[i].Mode == ZRET_LTMD) && (hdl->seqState != 0) 
			    && (hdl->mtPrm.ProfPrm.zretLtCancel == ZRET_LATCH_IS_NOCANCEL)) )
		{ /* ZRET動作中 or モーダルラッチ中は、何もしない */
			return MCI_SUCCESS;
		}
		MtcCancelLatch( hdl, NONE_LTMD, i );
	}
	return MCI_SUCCESS;
}


/****************************************************************************
 座標系設定
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


/* Motion状態取得API */
/****************************************************************************
 LATCHステータス取得
 i：ラッチ番号 （i<= LT_NUM であること)
****************************************************************************/
BOOL MciGetLatchStatus( MT_HNDL *hdl, LONG *ltEdge, LONG i )
{
	BOOL lcmp;

	*ltEdge = ( hdl->latch[i].Cmp >> 4 );			/* bit4状態 */
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
 共通モニタデータ取得
 SV_STATのデータは、RspMMngerにて作成する
****************************************************************************/
/****************************************************************************
 共通モニタデータ取得
SV_STATのデータだけは、内部のデータが下記のように分類されている
 1Byte目：現在の通信フェーズ
 2Byte目：現在の制御モード 0:位置, 1:速度, 2:トルク
 3Byte目：動作中のコマンドコード
 4byte目：拡張信号モニタ(0bit目)
 4byte目：拡張信号モニタ(1bit目)
 4byte目：拡張信号モニタ(2,3bit目)
 4byte目：拡張信号モニタ(4,5bit目)
 4byte目：拡張信号モニタ(6,7bit目 Reserve)
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
			data = hdl->svCmd->ExtCmd1.dwVal << 6;	/* [24bit/MAX] → [30bit/MAX] */
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
			data = hdl->svCmd->ExtCmd1.dwVal << 6;	/* [24bit/MAX] → [30bit/MAX] */
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
 オプションモニタデータ取得
 cmon_sel  オプションモニタのデータ選択番号
 omon_no   オプションモニタの番号 [1/2]
****************************************************************************/
static	LONG mciGetOptionMonitorData( MT_HNDL *hdl, LONG cmon_sel, LONG omn_no )
{
	LONG data;

	switch( cmon_sel )
	{
	case LST_LPOS1_MON: //ラッチ位置LPOS1前回値
		data = hdl->psnHdl.LstLpos[0];
		break;
	case LST_LPOS2_MON: //ラッチ位置LPOS2前回値
		data = hdl->psnHdl.LstLpos[1];
		break;
	case SEQ_LATCH_STS: //連続ラッチステータス
		data = hdl->latch[1].SeqSts.wdCnt;
		break;
/* <S1D7>:暫定追加 */
	case HIGH_ORDER32_POS:		/* 85:指令座標系の指令位置(フィルタ後)		上位32Bitデータ	*/
	case HIGH_ORDER32_MPOS:		/* 86:指令位置								上位32Bitデータ	*/
	case HIGH_ORDER32_PERR:		/* 87:位置偏差								上位32Bitデータ	*/
	case HIGH_ORDER32_APOS:		/* 88:機械座標系フィードバック位置			上位32Bitデータ	*/
	case HIGH_ORDER32_LPOS1:	/* 89:機械座標系フィードバックラッチ位置1	上位32Bitデータ	*/
	case HIGH_ORDER32_LPOS2:	/* 8A:機械座標系フィードバックラッチ位置2	上位32Bitデータ	*/
	case HIGH_ORDER32_IPOS:		/* 8B:指令座標系指令位置(フィルタ前)		上位32Bitデータ	*/
	case HIGH_ORDER32_TPOS:		/* 8C:指令座標系目標位置					上位32Bitデータ	*/
		data = 0;
		break;
/* <S1D7>:暫定追加 */
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
 モニタデータ取得（M3標準サーボプロファイル）
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
		data = data << 6;					/* [24bit/MAX] → [30bit/MAX] */
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
		data = data << 6;					/* [24bit/MAX] → [30bit/MAX] */
		data = MlibMulgain32Sx48( data, hdl->mtPrm.BasePrm.m3ConvUnit.m3TrqMaxToUnit );
		break;
#endif /* FLOAT_USE */

	case M3STD_MPOS_MON:
		data = hdl->psnHdl.Mpos;
		break;
	case M3STD_CMN1_MON:					/* 共通モニタ */
		data = mciGetCommonMonitorData( hdl, hdl->mtPrm.BasePrm.m3_selMon1 );
		break;
	case M3STD_CMN2_MON:					/* 共通モニタ */
		data = mciGetCommonMonitorData( hdl, hdl->mtPrm.BasePrm.m3_selMon2 );
		break;

	case M3STD_OMN1_MON:
		if( hdl->svRsp->SelMon5 == SELMON_FBPOS64L )
		{ /* 初期エンコーダ位置取得処理中は、前回のOMN1の値を返す */
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
		{ /* 初期エンコーダ位置取得処理中は、前回のOMN2の値を返す */
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
 モニタデータ取得（M2互換プロファイル）
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
#elif( unit == 2 )		/* <S182>コード整理 */
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
	モニタデータ(ポート入力)取得（M2互換プロファイル）
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
 I/Oモニタデータ取得
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

/* 2008.12.24 Y.Oka EXT1, 2, 3未対応 */
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
 ラッチ範囲内/外確認処理
****************************************************************************/
static	BOOL MciGetLtRange( PSN_HNDL *hdl, LONG i )
{
	BOOL ltcmp;

	ltcmp = FALSE;

	if( hdl->Prm->ltRangeP > hdl->Prm->ltRangeN )
	{ /* ラッチ上限値 > ラッチ下限値の場合、ラッチ領域は ラッチ下限値 <= apos <= ラッチ上限値 */
		if( (hdl->Apos >= hdl->Prm->ltRangeN) && (hdl->Apos <= hdl->Prm->ltRangeP) )
		{
			ltcmp = TRUE;
		}
	}
	else
	{ /* ラッチ上限値 <= ラッチ下限値の場合、ラッチ領域は apos <= ラッチ下限値 or ラッチ上限値 <= apos */
		if( (hdl->Apos <= hdl->Prm->ltRangeP) || (hdl->Apos >= hdl->Prm->ltRangeN) )
		{
			ltcmp = TRUE;
		}
	}
	return ltcmp;
}


/****************************************************************************
 LtRdy(LT_EXE1,2)のモニタ信号作成
****************************************************************************/
LONG MciGetLtRdy( MT_HNDL *hdl, LONG i )
{
	if( (hdl->latch[i].LtDisable == FALSE) && (hdl->latch[i].Req == TRUE) )
	{
		/* ラッチ有効 && ラッチリクエストあり */
		/* Lposがラッチ領域内であれば、TRUE */
		return MciGetLtRange( &hdl->psnHdl, i );
	}
	else
	{
		/* ラッチ無効 or ラッチリクエストなし */
		return FALSE;
	}
}





/***************************************** end of file **********************************************/
