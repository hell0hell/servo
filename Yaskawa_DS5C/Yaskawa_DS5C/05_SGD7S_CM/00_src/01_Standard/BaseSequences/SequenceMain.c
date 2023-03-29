/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SequenceMain.c : サーボシーケンスメイン処理定義												*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : Sequenceメイン処理																		*/
/*																									*/
/*		アラーム監視、アラーム管理、運転シーケンス制御などを実施する。								*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/*																									*/
/************** Copyright (C) Yaskawa Electric Corporation ******************************************/
/*																									*/
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "SerialEncIf.h"
#include "SequenceIo.h"
#include "PowerManager.h"
#include "CheckMotSts.h"
#include "CheckAlarm.h"
#include "RemVibFreq.h"
#include "AlarmManager.h"
#include "HwDeviceIf.h"
#include "MLib.h"
#include "KLib.h"
#include "JL086ServoIpHw.h"
#include "AdvancedAutoTuning.h"
#include "SftDiagFunc.h"	/*<S1F6>*/
#include "RippleComp.h"	/*<S1FD>*/
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE    /* <S1F5> */
#include "Sqi.h"
#endif

#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
#include "PnlOp.h"	/* Network用パネオペ */
#else
/* TODO: AP用パネオペ */
#endif

/* ---futami---> */
//#include "OverheatManager.h"
/*<---futami---> */



/****************************************************************************************************/
/*		LOCAL FUNCTION																				*/
/****************************************************************************************************/
static void SyscMakeMonitorData( AXIS_HANDLE *AxisRscC );
static void SyscUpdateBaseEnableStatus( BE_SEQ_HNDL *BeSeq, LONG AxisNo,
                                       SEQ_CTRL_OUT *SeqCtrlOut, BOOL PowerOn, BOOL faultState,
									   BOOL MotorLessTestMode );									/* <S1B7> */

extern void	CpxZeroClampSequence( ZC_CTRL_DATA *Zcv, BASE_CTRL_OUT	*Bout,
													 FUN_CMN_CONTROL *Fctrl, SEQ_CTRL_OUT *Cout );
extern void SencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
									 BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag );
extern void SencOutputPsoSerial( MENCV*, ASIC_HW_IF_STR*, SHORT, BOOL );
extern void FencSencOutputPaoSerial( MENCV *MencV, BPRMDAT *Bprm, ASIC_HW_IF_STR *pAsicHwReg,
									   BOOL DbBeReq, BOOL SenReq, USHORT AlmCode, BOOL AlmFlag );
extern void FencSencOutputPsoSerial( MENCV*, ASIC_HW_IF_STR*, SHORT, BOOL );
extern void HALdrv_SetAlmOut( int axis, UINT x );
extern void HALdrv_NetConLedControl( BOOL ledOut );
extern void HALdrv_TstOutputPnlLedC( void );
extern BOOL LedChkTest(  PNLOP_DATA_HANDLE *hLedData, ULONG LightIntervalTime );

static void	LpxMakeOpDispAttr( AXIS_HANDLE *AxisRscC );		/* <S00C> */
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE
static void	LpxAlmRstSequence( AXIS_HANDLE *AxisRscC );		/* <S00C> */
#else
#define LpxAlmRstSequence( x ) { ;}
#endif	
//static USHORT MakeServoStatus( AXIS_HANDLE *AxisRscC );		/* <S049> *//* <S0A1> */
static void MakeServoStatus( AXIS_HANDLE *AxisRscC );		/* <S049> *//* <S0A1> */

#ifdef CSW_SVON_INTERLOCK

#define SVON_LOCK_GRP1			0x00000006		// Axis 1 and 2 on CB2 board
#define SVON_LOCK_GRP2			0x000000f8		// Axis 3 and 8 on CC board
#define SVON_LOCK_GRP1_MAX		1
#define SVON_LOCK_GRP2_MAX		3

typedef struct SYS_BE_INTERLOCK {
	UINT	SvOnAxisBits;
	UINT	waitForSvOnAxisBits;
	UINT	LockAxesBits;
	UINT	BeReqAxesBits;
} SYS_BE_INTERLOCK;

static SYS_BE_INTERLOCK SysBeInterlock = {0};

static UINT sysGetBitONCount(ULONG bits, ULONG mask)
{
	UINT cnt;
	ULONG tmp;

	cnt = 0;
	for(tmp = (bits&mask); tmp != 0; tmp = (tmp >> 1))
	{
		if(0 != (tmp & 1))
		{
			cnt++;
		}
	}
	return cnt;
}

/****************************************************************************************************/
/*		Special functions at Multi-Axis SERVO ON													*/
/****************************************************************************************************/
static BOOL SysInterlockServoOnRequest(SYS_BE_INTERLOCK *BELock, BOOL BkBeReq, BOOL BeReqSeqOut, LONG ax_no)
{
	UINT ax_bit;
	ax_bit = (1 << ax_no);

	if( BkBeReq )
	{
		if(BeReqSeqOut == FALSE)
		{
			BELock->waitForSvOnAxisBits |= ax_bit;
			BELock->SvOnAxisBits &= ~ax_bit;

			if(0 != (SVON_LOCK_GRP1 & ax_bit))
			{
				BELock->LockAxesBits |= ax_bit;
				if(SVON_LOCK_GRP1_MAX < sysGetBitONCount(BELock->LockAxesBits, SVON_LOCK_GRP1))
				{
					BkBeReq = (BELock->BeReqAxesBits >> ax_no) & 1;
				}
				else
				{
					BELock->BeReqAxesBits |= ax_bit;
				}
			}
			if(0 != (SVON_LOCK_GRP2 & ax_bit))
			{
				BELock->LockAxesBits |= ax_bit;
				if(SVON_LOCK_GRP2_MAX < sysGetBitONCount(BELock->LockAxesBits, SVON_LOCK_GRP2))
				{
					BkBeReq = (BELock->BeReqAxesBits >> ax_no) & 1;
				}
				else
				{
					BELock->BeReqAxesBits |= ax_bit;
				}
			}
		}
		else
		{
			BELock->SvOnAxisBits |= ax_bit;
			BELock->LockAxesBits &= ~ax_bit;
			BELock->BeReqAxesBits &= ~ax_bit;
		}
	}
	else
	{
		BELock->LockAxesBits &= ~ax_bit;
		BELock->waitForSvOnAxisBits &= ~ax_bit;
		BELock->SvOnAxisBits &= ~ax_bit;
		BELock->BeReqAxesBits &= ~ax_bit;
	}

	return BkBeReq;
}


static BOOL SysMakeServoONInterlockState(SYS_BE_INTERLOCK *BELock, BOOL BkBeReq, BOOL BeReqSeqOut, LONG ax_no)
{
	UINT	SvOnAxisBits;
	UINT	waitForSvOnAxisBits;

	SvOnAxisBits = BELock->SvOnAxisBits;
	waitForSvOnAxisBits = BELock->waitForSvOnAxisBits;

	if(waitForSvOnAxisBits == (waitForSvOnAxisBits & SvOnAxisBits))
	{
		BELock->waitForSvOnAxisBits = 0;
	}
	else
	{
		BeReqSeqOut = FALSE;
	}

	return BeReqSeqOut;
}

#endif	// CSW_SVON_INTERLOCK



/****************************************************************************************************/
/*																									*/
/*		Initialize Servo Sequence Variables															*/
/*																									*/
/****************************************************************************************************/
void SyscInitServoSequence( AXIS_HANDLE *AxisRscC )
{
	BE_SEQ_HNDL		*BeSeqData;
	BeSeqData = AxisRscC->BeSequence;

/*--------------------------------------------------------------------------------------------------*/
/*		Reset Servo Sequence Variables																*/
/*--------------------------------------------------------------------------------------------------*/
	MlibResetLongMemory( &(BeSeqData->AlmStop.var), sizeof( BeSeqData->AlmStop.var )/4 );
	MlibResetLongMemory( &(BeSeqData->OtSeqData.var), sizeof( BeSeqData->OtSeqData.var )/4 );
	MlibResetLongMemory( &(BeSeqData->BkSeqData.var), sizeof( BeSeqData->BkSeqData.var )/4 );
	MlibResetLongMemory( &(BeSeqData->DbSeqData), sizeof( BeSeqData->DbSeqData )/4 );
	MlibResetLongMemory( &(BeSeqData->ZcCtrlData.f), 		/* <S00C> */
				( sizeof( BeSeqData->ZcCtrlData ) - sizeof( BeSeqData->ZcCtrlData.P ))/4 );
	MlibResetLongMemory( &(BeSeqData->ChargePump), sizeof( BeSeqData->ChargePump )/4 );

	/* Reset Servo Sequence Special Variables */
	AxisRscC->SeqCtrlOut->CtrlModeReq.dw = CTRLMODE_NOCMD;
	AxisRscC->CheckAlarm->faultState |= SHAL_FAULT_BB;	/* <S00D> */
	BeSeqData->DbOn = TRUE;
	BeSeqData->SvonEnable = TRUE;

	/* <S154> Start */
/*--------------------------------------------------------------------------------------------------*/
/*		アラームラッチ処理関数用データ作成																	*/
/*--------------------------------------------------------------------------------------------------*/
	AxisRscC->hAlmLatchHndl->UnMonitor		= AxisRscC->UnMonitor;
	AxisRscC->hAlmLatchHndl->SeqMotSts		= AxisRscC->SeqMotSts;
	AxisRscC->hAlmLatchHndl->Bprm			= AxisRscC->UniPrms->Bprm;
	AxisRscC->hAlmLatchHndl->BaseLoops		= AxisRscC->BaseLoops;
	AxisRscC->hAlmLatchHndl->PowerManager	= AxisRscC->PowerManager;
	AxisRscC->hAlmLatchHndl->SeqCtrlOut		= AxisRscC->SeqCtrlOut;
	AxisRscC->hAlmLatchHndl->MencV			= AxisRscC->MencV;
	AxisRscC->hAlmLatchHndl->BeSequence		= AxisRscC->BeSequence;
	AxisRscC->hAlmLatchHndl->BaseControls	= AxisRscC->BaseControls;
	AxisRscC->hAlmLatchHndl->BaseCtrlOut	= AxisRscC->BaseCtrlOut;
	AxisRscC->hAlmLatchHndl->AlmManager		= AxisRscC->AlmManager;
	AxisRscC->hAlmLatchHndl->IoSignals		= AxisRscC->IoSignals;
	AxisRscC->hAlmLatchHndl->SvControlIf	= AxisRscC->SvControlIf;
	AxisRscC->hAlmLatchHndl->pPlsIfV		= AxisRscC->pPlsIfV;
	AxisRscC->hAlmLatchHndl->SftOpt			= &(AxisRscC->OptCardHdl->SftOpt);
	AxisRscC->hAlmLatchHndl->UniPrm			= AxisRscC->UniPrms;					/* <S212> */
	/* <S154> End */

/*--------------------------------------------------------------------------------------------------*/
/*		その他未初期化変数初期化(暫定)																	*/
/*--------------------------------------------------------------------------------------------------*/
	AxisRscC->IoSignals->DetHwbbSts.GateDrvDefect = FALSE;
	AxisRscC->PowerManager->DetectDcVolt.DcAdFilo[0] = 0;
	AxisRscC->PowerManager->DetectDcVolt.DcAdFilo[1] = 0;
	AxisRscC->PowerManager->DetectDcVolt.DcAdFilo[2] = 0;
	MlibResetLongMemory( &(AxisRscC->CheckAlarm->DbOL.var), sizeof(AxisRscC->CheckAlarm->DbOL.var)/4 );

}


/****************************************************************************************************/
/*																									*/
/*		Kernel Input Service for ScanC																*/
/*																									*/
/****************************************************************************************************/
void SyscInputSeqStatus( AXIS_HANDLE *AxisRscC )
{
	BASE_CTRL_OUT		*BaseCtrlOut;
	SEQ_CTRL_OUT		*SeqCtrlOut;
	LONG				lwk0;
//	BOOL				hw_hwbb_support;/* <S04F> */
//	FBA_INPUT			SeqInputSignals;
//	SEQ_IOSIGNAL		*SeqIo;
	SEQ_IO_HNDL			*IoSignals;
	BOOL FstpSignal;   /* <S1F5> */

	BaseCtrlOut = AxisRscC->BaseCtrlOut;
	SeqCtrlOut = AxisRscC->SeqCtrlOut;
	IoSignals = AxisRscC->IoSignals;

	/* Update Motor status for Sequence modules */
	SMotSts_InputMotorStatus(AxisRscC->SeqMotSts,
	                         BaseCtrlOut,
	                         SeqCtrlOut,
	                         AxisRscC->UniPrms->Bprm);

	/* Update Id reference value for Sequence modules */
	AxisRscC->CtrlLoopOut->IdRefMon = (LONG)((SHORT)((AxisRscC->SvAsicRegs->AsicMicroReg)->WeakFV.IdOut));		/* 暫定 */
	BaseCtrlOut->IdRefMon			= AxisRscC->CtrlLoopOut->IdRefMon;

#if 1 /* <S0E4> *//*<S1F6>*/
	/* @@<S1F5> 2軸目FSTP用のlwk0=svon_reqが不定になっていた */
	/* Get HWBB Status for Safety */
	lwk0 = (BaseCtrlOut->CmdSeqBit & (SERVO_ON_BIT | POL_DETECT_BIT)) | (AxisRscC->FnCmnCtrl)->FnSvControl;
	SeqCtrlOut->HwbbSts = SeqIoDetectHardWiredBB( &(IoSignals->DetHwbbSts),
											 lwk0,
											 (ASIC_HW_IF_STR *)HW_SVASIC_ADDRESS,		/* HWBBは1軸目固定で入力<S1F6>*/
											 IoSignals->DetHwbbSts.hwbbsupport);
//												 hw_hwbb_support /* <S0F0> CSW_HW_HWBB_SUPPORT */);

	/* Get DPIN1,DPIN2 Status for Safety <S0F1> */
	SeqIoDetectTestPulseSignal( AxisRscC->IoSignals, AxisRscC->AxisNo );
#endif /* <S0E4> */

	/* Get FSTP Status	*/ /* <S1F5> Start */
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE
	FstpSignal = ((SqiV.Sigs & SQI_SIGS_FSTP ) >> SQI_BNO_FSTP);	/* Fstp */
	SeqCtrlOut->FstpSts = SeqIoDetectFstp( FstpSignal, SeqCtrlOut , lwk0 );
#else
	FstpSignal =  IoSignals->FstpSignal.SignalStatus.v;
 	SeqCtrlOut->FstpSts = SeqIoDetectFstp( FstpSignal, SeqCtrlOut , lwk0 );
#endif

#if 1 /*  CHECK暫定処理  出荷値設定のピン割り当て固定対応 <S02E><S032> */
	/* Update I/O status */
	SeqIoInputSeqSignals(IoSignals, AxisRscC->SvAsicRegs->AsicHwReg);
	SeqCtrlOut->PotSigSts = IoSignals->PotSignal.SignalStatus.v;		/* <S045> */
	SeqCtrlOut->NotSigSts = IoSignals->NotSignal.SignalStatus.v;		/* <S045> */
	SeqCtrlOut->EstpSigSts = IoSignals->EstpSignal.SignalStatus.v;		/* <S045> */
#endif
	/* Make Monitor Data for Un*** */
	SyscMakeMonitorData( AxisRscC );

	/* AP Alarm Reset <S00C> */
	LpxAlmRstSequence( AxisRscC );
}

/****************************************************************************************************/
/*																									*/
/*		Alarm detections <S00C>																		*/
/*																									*/
/****************************************************************************************************/
#if CSW_NETIF_TYPE	== NETIF_ANLGPLS_TYPE
void	LpxAlmRstSequence( AXIS_HANDLE *AxisRscC )
{
LONG AlmRst;
BASE_CTRL_OUT		*BaseCtrlOut;
SEQ_CTRL_OUT		*SeqCtrlOut;
BaseCtrlOut = AxisRscC->BaseCtrlOut;
SeqCtrlOut = AxisRscC->SeqCtrlOut;
AlmRst = (BaseCtrlOut->CmdSeqBit & ALM_RESET_BIT);

	if( AlmRst && (SeqCtrlOut->AlmRstLast == FALSE) )	/* リセット指令：OFF --> ON ?			*/
	{
		ALMSetAlarmResetRequest( AxisRscC->AlmManager );
	}
	SeqCtrlOut->AlmRstLast = (AlmRst) ? TRUE : FALSE;	
}
#endif

/****************************************************************************************************/
/*																									*/
/*		Alarm detections 																			*/
/*																									*/
/****************************************************************************************************/
void SyscAlarmDetections( AXIS_HANDLE *AxisRscC )
{
	LONG			SvStatus;
	ALMMAN_HANDLE	*AlmManager;
	BE_SEQ_HNDL		*BeSeqData;
	BASE_CTRL_OUT	*BaseCtrlOut;
	CHECK_ALARM		*CheckAlarm;
	MENCV			*MencV;
	ENVMON			*EnvMonitor; /* <S0ED> */
	TEST_PULSE_DIAG *TestPulseDiag1;	/* <S0F1> *//*<S1F6>*/
	TEST_PULSE_DIAG *TestPulseDiag2;	/* <S1F6> */

	AlmManager = AxisRscC->AlmManager;
	BeSeqData = AxisRscC->BeSequence;
	BaseCtrlOut = AxisRscC->BaseCtrlOut;
	CheckAlarm = AxisRscC->CheckAlarm;
	MencV = AxisRscC->MencV;
	EnvMonitor = AxisRscC->EnvMonitor; /* <S0ED> */
	TestPulseDiag1 = &(AxisRscC->IoSignals->TestPulseDiag[0]);	/* <S1F6> */
	TestPulseDiag2 = &(AxisRscC->IoSignals->TestPulseDiag[1]);	/* <S1F6> */

	/*------------------------------------------------------------------*/
	/*		Detect JL-077 Alarms (Watch-dog, OC, OH)					*/
	/*------------------------------------------------------------------*/
	AdetCheckAsicHwAlarm( CheckAlarm, AxisRscC->SvAsicRegs, MencV->EncDisable, AlmManager );

/*--------------------------------------------------------------------------------------------------*/
/*		サーボアラーム監視＆サーボ制御監視処理														*/
/*--------------------------------------------------------------------------------------------------*/
	/* Check the Encoder Alarm */
#if CSW_BOARD_TYPE == SGD7W_TYPE	/*	SGD7W（2軸対応）のみ<S102>	*/
//+ 注意)シリアルエンコーダI/Fは1軸目のサーボIP共用となるため、AsicHwReg = 1軸目とする
	AdetCheckSencAlarm( AlmManager,
						MencV,
						SHal_GetEncAlmCode(AxisRscC->MencV->EncIf.SencSetReg, AxisRscC->AxisNo) );
#elif CSW_BOARD_TYPE == SGD7S_TYPE	/*	SGD7S（1軸対応）のみ	*/
	AdetCheckSencAlarm( AlmManager,
						MencV,
						SHal_GetEncAlmCode(AxisRscC->SvAsicRegs->AsicHwReg, 0) );
#endif	/* #if CSW_BOARD_TYPE == SGD7W_TYPE <S102> */

	/* Check Over Speed */
	if( MencV->EncDisable == FALSE )
	{ /* エンコーダレディ時 */
		AdetCheckOverSpeed(AxisRscC->AlmManager,
		                   CheckAlarm,
		                   AxisRscC->SeqMotSts->var.AbsMotSpd,
						   AxisRscC->UniPrms->Bprm );

/*--------------------------------------------------------------------------*/
#if	(SVFSEL_DIVOUT_USE == TRUE)  /*<S02D>*/
		/*------------------------------------------------------------------*/
		/*		A.511:分周パルス出力過速度監視処理							*/
		/*------------------------------------------------------------------*/
		if( AxisRscC->UniPrms->Bprm->FencUse == FALSE )
		{ /* セミクローズ制御 */
			if( MencV->DivInfo.DivOut == TRUE )
			{
				AdetCheckDivOverSpeed(AxisRscC->AlmManager,
									  AxisRscC->SeqMotSts->var.AbsMotSpd,
									  MencV->DivInfo.DivOvrSpd );
			}
		}
/*--------------------------------------------------------------------------*/
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
		else
		{
			if( AxisRscC->FencV->DivInfo.DivOut == TRUE )
			{ /* フルクローズ制御時、分周出力有効 */
				AdetCheckDivOverSpeed(AxisRscC->AlmManager,
									  AxisRscC->SeqMotSts->var.AbsMotSpd,
									  AxisRscC->FencV->DivInfo.DivOvrSpd );
			}
		}
		/*------------------------------------------------------------------*/
#endif	//(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
/*--------------------------------------------------------------------------*/
#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/
/*--------------------------------------------------------------------------*/
	}

#if 0 /* <S0DE> */
	/* Over Heat *//* <S03A> */
	AdetCheckOverHeatAlm( AlmManager,
						  &(CheckAlarm->OverHeat),
						  DetTempGetCtrlBoardTemperature(AxisRscC->TempManager),
						  DetTempGetPowerBoardTemperature(AxisRscC->TempManager) );
	/* 設置環境モニタ TODO アンプはユニット毎、モータは軸毎に分ける？？ *//* <S03A> */
	AdetInstEnvMonitor( &(CheckAlarm->EnvMon),
						DetTempGetInternalTemperature(AxisRscC->TempManager),
						MencV->Temperature );
#endif

	/* Check Over Load */
	/* アンプ＆モータ過負荷チェック処理(SGDS方式) */
	AdetCheckOverLoadAmpMot( AlmManager,
							CheckAlarm,
							AxisRscC->CtrlLoopOut->IdRefMon,
							AxisRscC->CtrlLoopOut->IqRefMon,
							AxisRscC->SeqMotSts->var.AbsMotSpd,
							MencV->EncConnect );

	/* DB過負荷チェック処理 */
	AdetCheckDbrakeOverLoad( AlmManager,
							&(CheckAlarm->DbOL),
							BeSeqData->DbOn,
							AxisRscC->SeqMotSts->var.AbsMotSpd );

	/* 速度リップル補償情報不一致ワーニングチェック処理 *//* <S1FD> */
	AdetCheckRippleWrn( AlmManager,
						RippleCompGetRippleWrnState( &AxisRscC->BaseLoops->RippleCmp) );

/*--------------------------------------------------------------------------------------------------*/
/*		ハード異常関連アラーム監視																	*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* OH検出はユニット処理に変更 *//* <S0ED> */
	/* Over Heat *//* <S03A> *//* <S0DE> */
	AdetCheckOverHeatAlm( AlmManager,
						  &(CheckAlarm->OverHeat),
						  DetTempGetCtrlBoardTemperature(AxisRscC->TempManager),
						  DetTempGetPowerBoardTemperature(AxisRscC->TempManager) );

	/* Detect Temperature IC Error *//* <S0DE> */
	AdetCheckTempDeviceErr( AlmManager,
							DetTempGetCtrlBoardErrorCount(AxisRscC->TempManager),
							DetTempGetPowerBoardErrorCount(AxisRscC->TempManager) );
#endif /* <S0ED> */

	/* 設置環境モニタ *//* <S03A> *//* <S0DE> */
	AdetInstEnvMonitor( EnvMonitor,
						DetTempGetInternalTemperature(AxisRscC->TempManager),
						MencV->Temperature );

	/* Check Control Error */
	/* モータ暴走チェック処理 */
	AdetCheckMotorOverrun( AlmManager,
						  &(CheckAlarm->OvrRun),
						  AxisRscC->SeqMotSts->var.MotSpd,
						  AxisRscC->CtrlLoopOut->TrqRefMon,
						  CheckAlarm->faultState & SHAL_FAULT_BB,
						  (MOTTYPE_LINEAR == MencV->AxisMotType) );

	/* サーボオン時位置偏差過大チェック処理 */
	if( (BaseCtrlOut->CtrlModeOut.b.cm != CTRLMODE_POS) ||
		((BeSeqData->DbSeqData.DbBeReq == FALSE) || (BeSeqData->BeReqSeqOut == TRUE)) )
	{
		ALMClearWarning( AlmManager, WRN_BEOF );	/* A.901 : サーボオン時位置偏差過大	*/
	}
	else
	{
		AdetCheckSvonPerrOverFlow( AlmManager,
								   &(CheckAlarm->OvrPosErr),
								   BaseCtrlOut->PositionError );
	}

	/* 位置偏差過大チェック処理 */
	AdetCheckPerrOverFlow( AlmManager,
						  &(CheckAlarm->OvrPosErr),
						  (CTRLMODE_POS == BaseCtrlOut->CtrlModeOut.b.cm),
						  BaseCtrlOut->PositionError,
						  BaseCtrlOut->SvonSpdLmtReq );

	/* 位置データオーバ監視(リニア) */
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		if( MlibLABS( MencV->MotAbsPos ) > POSDATALMT )
		{
			ALMSetServoAlarm( AlmManager, ALM_POSOF );
		}
	}

#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
	/* 位置データオーバ監視(フルク) */
	if( AxisRscC->UniPrms->Bprm->FencUse != FALSE )
	{
		AdetCheckFullcEncoderAlarm(&(CheckAlarm->OvrFPosErr),
		                       AxisRscC->FencV,
							  (CTRLMODE_POS == BaseCtrlOut->CtrlModeOut.b.cm),
		                       AxisRscC->SeqMotSts->var.MotPos,
		                       AxisRscC->BaseCtrlOut->FencPos,
		                       AlmManager );
	}
#endif	//(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)

	if( TRUE == AxisRscC->IoSignals->DetHwbbSts.hwbbsupport )	/*処理順見直し<S1F6>*/
	{
		/* Check Safety Input Error */
		/* 入力信号タイミング異常チェック処理 */
		if( FALSE != AdetCheckBbonSignal(&(CheckAlarm->HwbbTiming), (AxisRscC->IoSignals)->DetHwbbSts.LackOneBbon) )
		{
			/* A.Eb1 : 停止信号入力時間差過大(カード無) */
			ALMSetServoAlarm( AlmManager, ALM_BBON_ERR0 );
		}

		/* Check DPIN1 Error <S0F1> */
		if( FALSE != AdetTestPulseDiagnostic(TestPulseDiag1) )
		{
			/* A.EC8 : セーフティ機能Drv内部信号異常１ */
			ALMSetServoAlarm( AlmManager, ALM_DPIN1_ERR );
		}

		/* Check DPIN2 Error <S0F1> */
		if( FALSE != AdetTestPulseDiagnostic(TestPulseDiag2) )
		{
			/* A.EC9 : セーフティ機能Drv内部信号異常２ */
			ALMSetServoAlarm( AlmManager, ALM_DPIN2_ERR );
		}
	}
	else
	{
		/* Check Gate Drive Error */
		/* ゲートドライブ異常チェック処理 */
		if( FALSE != (AxisRscC->IoSignals)->DetHwbbSts.GateDrvDefect )
		{
			/* A.B40 : ゲートドライブ異常アラーム */
			ALMSetServoAlarm( AlmManager, ALM_HWBB_UNSUPPORT );
		}
	}

	/* Detect Vibration */
	if( BeSeqData->BeReqSeqOut == FALSE )
	{ /* サーボOFF中 */
		/* A.911 : 振動(発振)ワーニング */
		ALMClearWarning( AlmManager, WRN_VIB );
		/* サーボ振動(発振)チェックリセット処理 */
		DetVibResetRequest( AxisRscC->DetVib );
		/* サーボ制御発振(低周波)チェックリセット処理 */
		DetVibLowFreqResetRequest( AxisRscC->DetVib );
	}
	else
	{ /* サーボON中 */
		/* サーボ振動(発振)チェック処理 */
		SvStatus = DetVibDetectVibration( AxisRscC->DetVib );
		if( SvStatus & DETECT_VIB_ALARM )
		{
			/* A.520 : 振動(発振)アラーム */
			ALMSetServoAlarm( AlmManager, ALM_VIB );
			/* A.911 : 振動(発振)ワーニング */
			ALMClearWarning( AlmManager, WRN_VIB );
		}
		else if( SvStatus & DETECT_VIB_WARNING )
		{
			/* A.911 : 振動(発振)ワーニング */
			ALMSetServoAlarm( AlmManager, WRN_VIB );
		}
		else
		{
			/* A.911 : 振動(発振)ワーニング */
			ALMClearWarning( AlmManager, WRN_VIB );
		}

		/* サーボ制御発振(低周波)監視処理 */
		SvStatus = DetVibDetectLowFreqVibration( AxisRscC->DetVib );
		if( (SvStatus & DETECT_VIB_ALARM)
			&& (AxisRscC->BaseControls->TuneLessCtrl.var.TuneLessAct == TRUE) )
		{ /* アラーム検出 && 調整レス有効時 */
			/* A.521 : ATアラーム */
			ALMSetServoAlarm( AlmManager, ALM_AT );
		}
	}

	/* カーネルアラーム管理処理 */
	ALMRuntimeProcess( AlmManager, (UCHAR)(BeSeqData->AlmStop.conf.StopMode) );
	ALMRuntimeProcess2( AlmManager, (UCHAR)(BeSeqData->AlmStop.conf.StopMode) );	/* <S226> */

}

/****************************************************************************************************/
/*																									*/
/*		ScanC Main Function																			*/
/*																									*/
/****************************************************************************************************/
void SyscSequenceMain( AXIS_HANDLE *AxisRscC )
{
	BASE_CTRL_HNDL	*BaseCtrlData;
	SEQ_CTRL_OUT	*SeqCtrlOut;
	BE_SEQ_HNDL		*BeSequence;


	/* ローカル変数の初期化 */
	BaseCtrlData =AxisRscC->BaseControls;
	SeqCtrlOut = AxisRscC->SeqCtrlOut;
	BeSequence = AxisRscC->BeSequence;
/*------------------------------------------------------------------------------------------------------*/
/*		サーボ運転シーケンス																			*/
/*------------------------------------------------------------------------------------------------------*/

	/* プログラムJOGシーケンス */
	CpxPrgJogPosSequence( &BaseCtrlData->PJogHdl,
						  AxisRscC->SeqCtrlOut,
						  AxisRscC->AlmManager,
						  AxisRscC->FnCmnCtrl->FnCtrlMcmd ); 

	/* オフラインイナーシャ同定 */
	CpxJatOfflineCalc(	&BaseCtrlData->JatHdl,
						AxisRscC->UnMonitor->UnMotSpdx,
						AxisRscC->DetVib,
//						AxisRscC->CtrlLoopOut->PConActFlg, /* <S064> */
						AxisRscC->BaseLoops->PConActFlg,
						BaseCtrlData->CtrlCmdMngr.PerrClrReq);

	/* アドバンストオートチューニング */
	CpxAdvancedAutoTuning(	AxisRscC->AdatHndl,
							AxisRscC->FnCmnCtrl);
	
/*------------------------------------------------------------------------------------------------------*/
/*		残留振動モニタ用処理																			*/
/*------------------------------------------------------------------------------------------------------*/
	/* Calculate Remaining Vibration Frequency */
	/* Check the RemVibCalculate Execution Condition */
	RemVibChkCondition( AxisRscC->RemVibFreq,
						(CTRLMODE_POS == BaseCtrlData->CtrlModeSet.CtrlMode.b.cm) );
	/* Calculate RemVibration */
	RemVibCalculateFreq( AxisRscC->RemVibFreq,
	                     BaseCtrlData->PosManager.var.PosErr,
						 BaseCtrlData->PosCtrlSts.CoinSignal,
						 BaseCtrlData->PosCtrlSts.RefZSignal );


	/* Outputs Sequence I/O signals */
#ifndef HW_FBA_IO_AXIS
	SeqIoBrakeControl( &(AxisRscC->IoSignals->BkSignal),
						 &(SeqCtrlOut->BrkSigSts),
						 BeSequence->BkSeqData.var.Brake,
						 ((AxisRscC->BaseCtrlOut->CmdSeqBit & BRK_RELEASE_BIT) >> BRK_RELEASE_BITNO) );
#else
	SeqIoBrakeControlAx( &(AxisRscC->IoSignals->BkSignal),
						 &(SeqCtrlOut->BrkSigSts),
						 BeSequence->BkSeqData.var.Brake,
						 ((AxisRscC->BaseCtrlOut->CmdSeqBit & BRK_RELEASE_BIT) >> BRK_RELEASE_BITNO),
						 &(AxisRscC->IoSignals->SerialOutputAx));

	HALdrv_AlmOutAx( AxisRscC->AlmManager->AlmFlag, &(AxisRscC->IoSignals->SerialOutputAx) ); /* <S145> */
	HALdrv_OutputSerialIOAx( &(AxisRscC->IoSignals->SerialOutputAx), AxisRscC->SvAsicRegs->AsicHwReg);
#endif

	/* Make Response data for Network Interface */
	SeqIoOutputNetIf( AxisRscC->IoSignals,
	                 BeSequence,
					 SeqCtrlOut,
					 AxisRscC->AlmManager,
					 AxisRscC->MencV/* <S082> AxisRscC->MencV->PhaseReady*/,
					 AxisRscC->FnCmnCtrl->State.LockNormalOpeDrive,
					 AxisRscC->MotorLessTest->MotorLessTestMode );						/* <S1B7> */

}


/****************************************************************************************************/
/*																									*/
/*		Servo Sequence Main Program																	*/
/*																									*/
/****************************************************************************************************/
 void SyscBaseDriveSequence( AXIS_HANDLE *AxisRscC )
{
	BASE_CTRL_OUT		*BaseCtrlOut;
	SEQ_CTRL_OUT		*SeqCtrlOut;
	ASICS				*SvAsicRegs;
	MENCV				*MencV;
	BE_SEQ_HNDL			*BeSequence;
	ALMMAN_HANDLE		*AlmManager;
//	CHECK_MAIN_POWER	*PowerChecker;
	POW_MNG_HNDL		*PowerManager;
//	BOOL				RelaySts;
	BOOL				spot, snot;
	BOOL				ready_cond;
	ALMID_T				alm_id;
	BOOL				almflag;	/* <S215> */

//	SeqCtrlData = AxisRscC->SeqCtrlData;
	BaseCtrlOut = AxisRscC->BaseCtrlOut;
	SeqCtrlOut = AxisRscC->SeqCtrlOut;
	BeSequence = AxisRscC->BeSequence;
	SvAsicRegs = AxisRscC->SvAsicRegs;
	AlmManager = AxisRscC->AlmManager;
//	PowerChecker = &(AxisRscC->PowerManager->MainPowerChecker);
	PowerManager = AxisRscC->PowerManager;
	MencV = AxisRscC->MencV;

	/************************************/
	/* シーケンス制御用基本信号作成	*/
	/************************************/
	/* Make Servo Ready Status */
	almflag = (AxisRscC->FnCmnCtrl->forcedAlmReady != TRUE)?  AlmManager->AlmReady : TRUE;/* <S215> */
	ready_cond = PowerManager->PowerSts.PowerOn
				& AxisRscC->PowerManager->MainPowerChecker.var.CapaChargeOk				/* <S21D> */
				& almflag & MencV->EncReady & (!(SeqCtrlOut->HwbbSts)) & (!(SeqCtrlOut->FstpSts)); 	/* <S1F5> *//* <S215> */
	BeSeqMakeServoReadySignal( BeSequence, ready_cond, MencV->PhaseReady );

	/* Make Servo On and Sensor On Request */
	alm_id = BeSeqMakeServoOnRequest(BeSequence,
	                                 BaseCtrlOut->CmdSeqBit,
	                                 BaseCtrlOut->CtrlModeOut,
	                                 AxisRscC->FnCmnCtrl, MencV);
	if( NO_ALARM != alm_id )
	{
		if(ALM_PDET == alm_id)
		{/* A.C52 */
			if(FALSE == PdetCheckPdetCondition(AxisRscC->PdetAbsSeq, MencV))
			{	ALMSetServoAlarm( AlmManager, alm_id );}
		}
		else
		{/* A.0B0 */
			ALMSetServoAlarm( AlmManager, alm_id );
		}
	}

	/************************************/
	/* Set Alarm Stop Mode for Sequence Control */
	/************************************/
//	BeSeqInputAlmStopMode( &BeSequence->AlmStop,    /* <S1F5> */
	BeSeqInputAlmStopMode( BeSequence,     /* <S1F5> */
	                       AlmManager->Status.StopMode,	/* <S0FF> */
	                       BaseCtrlOut->CtrlModeOut,
	                       MencV->PhaseReady,
//	                       SeqCtrlOut->HwbbSts,    /* <S1F5> */
//	                       SeqCtrlOut->MotStop );    /* <S1F5> */
	                       SeqCtrlOut );    /* <S1F5> */
	/************************************/
	/* Set Force Stop Mode for Sequence Control */
	/************************************/
	BeSeqForceStopMode( BeSequence, 
	                    BaseCtrlOut->CtrlModeOut, 
	                    SeqCtrlOut->MotStop, 
	                    SeqCtrlOut->FstpSts );

	/************************************/
	/* メカニカルブレーキシーケンス		*/
	/************************************/
	BeSeqMechaBrakeSequence(BeSequence,
	                        BaseCtrlOut->CtrlModeOut,
	                        AxisRscC->SeqMotSts->var.AbsMotSpd,
	                        SeqCtrlOut->HwbbSts,
	                        AxisRscC->CheckAlarm->faultState,
	    					SeqCtrlOut->BeComplete,							/* <S1B7> */
							AxisRscC->MotorLessTest->MotorLessTestMode);	/* <S1B7> */


#ifdef CSW_SVON_INTERLOCK
	/* 複数軸同時サーボオンインタロック */
	SeqCtrlData->BkBeReq = SysInterlockServoOnRequest(&SysBeInterlock,
	                                                  SeqCtrlData->BkBeReq,
	                                                  SeqCtrlData->BeReqSeqOut,
	                                                  AxisRscC->AxisNo);
#endif //CSW_SVON_INTERLOCK

	/************************************/
	/* オーバートラベルシーケンス		*/
	/************************************/
	/* OT検出処理 */
	spot = (BaseCtrlOut->CmdSeqBit >> POSITIVE_OT_BITNO) & 0x1;
	snot = (BaseCtrlOut->CmdSeqBit >> NEGATIVE_OT_BITNO) & 0x1;
	SeqCtrlOut->OverTrvlSts = BeSeqDetectOTStatus( &BeSequence->OtSeqData,
	                                               BaseCtrlOut,
	                                               AxisRscC->IoSignals->PotSignal.SignalStatus.v | spot,		/* <S045> */
	                                               AxisRscC->IoSignals->NotSignal.SignalStatus.v | snot);		/* <S045> */
	/* OTシーケンス処理 */
	BeSeqOverTravelSequence(&BeSequence->OtSeqData,
							BaseCtrlOut->CtrlModeOut.b.cm,
							SeqCtrlOut->MotStop,
							SeqCtrlOut->BeComplete,						/* <S1F5> */
							BeSequence->LdstpTimeZero);     			/* <S1F5> */
	/* Set OT Torque Limit Request */
	SeqCtrlOut->TrqLimitReq = BeSequence->OtSeqData.var.OtTrqLimit		/* <S1F5> */
							| BeSequence->AlmStop.var.Gr2TrqLimit		/* <S1F5> */
							| BeSequence->ForceStop.var.FstpTrqLimit;	/* <S1F5> */
	/* Set OT Warning */
	if( BeSequence->OtSeqData.var.OtWarningDet == TRUE )
	{	/* A.9A0 : OT信号検出ワーニング */
		ALMSetServoAlarm( AlmManager, WRN_OT );
	}
	else
	{
		ALMClearWarning( AlmManager, WRN_OT );
	}
/*--------------------------------------------------------------------------------------------------*/
/* @@ Zero Clamp Sequence 																			*/
/*--------------------------------------------------------------------------------------------------*/
	CpxZeroClampSequence( &(BeSequence->ZcCtrlData),				/* Zero Clamp Seuqence  		*/
						BaseCtrlOut,								/* Base Control Output 			*/
						AxisRscC->FnCmnCtrl,						/* Function Control				*/
						SeqCtrlOut);								/* Sequence Output 				*/
/*--------------------------------------------------------------------------------------------------*/
	/************************************/
	/* サーボストップモード管理			*/
	/************************************/
	BeSeqStopModeManager(BeSequence);

	/************************************/
	/* Autoチャージポンプ処理				*/
	/************************************/
#if (CSW_AUTO_CHARGE_PUMP != FALSE)
	{
	BOOL OCState = ALMCheckEachState(AxisRscC->AlmManager, ALM_OC);
	BeSeqChargePumpControl( SvAsicRegs, &(BeSequence->ChargePump), OCState,
								AxisRscC->MotorLessTest->MotorLessTestMode );	/* <S1B7> */
	}
#endif

	/************************************/
	/* ダイナミックブレーキシーケンス	*/
	/************************************/
	BeSeqDynamicBrakeSequence( BeSequence,
							   SvAsicRegs, SeqCtrlOut->MotStop,
							   (((AlmManager->Status.MencDataNG) ? TRUE : FALSE) | MencV->EncDataNG),	/* <S0FF> */
							   AxisRscC->MotorLessTest->MotorLessTestMode );		/* <S1B7> */

#if 0 /* PcmRlyDbControlProcess()に統合 */
	/************************************/
	/* 突防リレー制御					*//* <S0A2> */
	/************************************/
	RelaySts = PcmRlyControlProcess( &PowerManager->MainPowerChecker,
									 &PowerManager->PowerSts,
									 AxisRscC->CheckAlarm->faultState,
									 BeSequence->DbOn,
									 AxisRscC->FnCmnCtrl->KnlMode );
	/* モータレステストモードでないときONする *//* <S1B7> */
	/* TODO: SGD-Vでは以下の条件もORしてある						*/
	/* (1) AxisRscC->BeSequence->StopMode == STOPMODE_DBSTOPFR		*/
	/* (2) AxisRscC->BeSequence->StopMode == STOPMODE_FREERUN		*/
	/* (3) AxisRscC->FnCmnCtrl->KnlMode == KPI_KMD_ADJCURAD			*/
	if( (RelaySts == FALSE) || (AxisRscC->MotorLessTest->MotorLessTestMode == FALSE) )
	{
		HALdrv_RelayControl( (void*)SvAsicRegs->AsicHwReg, RelaySts );
	}

	/************************************/
	/* DB回路制御						*/
	/************************************/
#if( CSW_HW_DB_CTRL == TRUE )
	if(AxisRscC->FnCmnCtrl->KnlMode != KPI_KMD_ADJCURAD)
	{
//		BeSeqDBControlProcess( BeSequence->DbOn,								/* <S0CB> Delete */
//							   PowerManager->MainPowerChecker.conf.RlyDbWay,	/* <S0CB> Delete */
//							   AxisRscC->CheckAlarm->faultState,				/* <S0CB> Delete */
//							   SvAsicRegs->AsicHwReg );							/* <S0CB> Delete */

/* <S0CB> Start */
		/* ---TestMode7(自動試験機モード：DB制御を強制 ON/OFF)でない場合、本処理でDB制御を操作--- */
		if( PowerChecker->conf.TestMode7 == FALSE )
		{
			BeSeqDBControlProcess( BeSequence->DbOn,
								   PowerManager->MainPowerChecker.conf.RlyDbWay,
								   AxisRscC->CheckAlarm->faultState,
								   SvAsicRegs->AsicHwReg,
								   AxisRscC->MotorLessTest->MotorLessTestMode );	/* <S1B7> */
		}
/* <S0CB> End */
	}
#endif
#endif /* #if 0  PcmRlyDbControlProcess()に統合 */

	/************************************/
	/* 突防回路/DB回路制御				*//* <S20E> */
	/************************************/
	PcmRlyDbControlProcess( SvAsicRegs->AsicHwReg,
							PowerManager,
							((AxisRscC->CheckAlarm->faultState & SHAL_FAULT_BB) ? TRUE : FALSE),
							BeSequence->DbOn,
							AxisRscC->FnCmnCtrl->KnlMode,
							AxisRscC->MotorLessTest->MotorLessTestMode );

	/************************************/
	/* スキャンＣ制御モード指令			*/
	/************************************/
	SeqCtrlOut->CtrlModeReq.dw = BeSeqControlModeManager(BeSequence, AxisRscC->FnCmnCtrl);
	/*主回路OFF時のIPM N側アーム制御要求 <S201>*/
	BeNegativePumpSequence( &(AxisRscC->PowerManager->PowerSts),
							AxisRscC->AlmManager,
							&(AxisRscC->IoSignals->DetHwbbSts),
							&(AxisRscC->PowerManager->MainPowerChecker),
							&(AxisRscC->BeSequence->DbSeqData),
							AxisRscC->SeqCtrlOut,
							AxisRscC->SvAsicRegs->AsicHwReg );

	/************************************/
	/* シーケンス状態更新				*/
	/************************************/
	SyscUpdateBaseEnableStatus( BeSequence,
								AxisRscC->AxisNo,
								SeqCtrlOut,
								AxisRscC->PowerManager->PowerSts.PowerOn,
								AxisRscC->CheckAlarm->faultState,
								AxisRscC->MotorLessTest->MotorLessTestMode );				/* <S1B7> */

}


/****************************************************************************************************/
/*																									*/
/*		ScanC Common Sequence																		*/
/*																									*/
/****************************************************************************************************/
void SyscSequenceCommon( AXIS_HANDLE *AxisRscC )
{
	LONG		ax_no;
//	UCHAR		LedDisp;
	AXIS_HANDLE	*AxisRsc;
//	HWSOUT ForceOutSignals;					/* <S0CB> */
#ifndef _TEST_BOARD_
	#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	FUN_CMN_CONTROL	*FnCmnCtrl;				/* <S0CB> */
	#endif
#endif
	AxisRsc = &AxisRscC[0];
#ifndef _TEST_BOARD_
	#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	FnCmnCtrl  = AxisRscC->FnCmnCtrl;		/* <S0CB> */
	#endif
#endif

	/*----------------------------------------------------------------------*/
	/*		7seg出力処理	<S0A1>											*/
	/*----------------------------------------------------------------------*/
#ifndef _TEST_BOARD_
	#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE

/* <S0CB> Start */
#if 0
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* Set Servo Status for LED Display */
		PnlOpSetLedStatus( AxisRsc->PnlOpLedDataHndl,
						   AxisRsc->UnMonitor->LedDispStatus );
		/* Make LED Display Data */
		if( TRUE == PnlOpDisplayLedData(AxisRsc->PnlOpLedDataHndl,
										AxisRsc->AlmManager) )
		{
			/* Output Serial I/O Info. */
			HALdrv_Net7SegLedControl(
					ax_no,
					PnlOpGetLedCode(AxisRsc->PnlOpLedDataHndl) );
		}
		AxisRsc++;
	}
	/* Output Connect LED Info. */
	HALdrv_NetConLedControl( PnlOpGetConnectStatus(AxisRscC->PnlOpLedDataHndl) );
#endif
/* <S0CB> End */

/* <S0CB> Start */
			if(FnCmnCtrl->PbAndLedChkTestFlag)
			{/* 7Seg-LED確認モード */
				for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
				{
					if( TRUE == LedChkTest(AxisRsc->PnlOpLedDataHndl,
												FnCmnCtrl->LightIntervalTime) )
					{
						/* Output Serial I/O Info. */
						HALdrv_Net7SegLedControl(
								ax_no,
								PnlOpGetLedCode(AxisRsc->PnlOpLedDataHndl) );
					}
					AxisRsc++;
				}

				/* Output Connect LED Info. */
				HALdrv_NetConLedControl( TRUE );

			}
			else
			{/* 通常モード */
				for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
				{
					/* Set Servo Status for LED Display */
					PnlOpSetLedStatus( AxisRsc->PnlOpLedDataHndl,
									   AxisRsc->UnMonitor->LedDispStatus );
					/* Make LED Display Data */
					if( TRUE == PnlOpDisplayLedData(AxisRsc->PnlOpLedDataHndl,
													AxisRsc->AlmManager) )
					{
						/* Output Serial I/O Info. */
						HALdrv_Net7SegLedControl(
								ax_no,
								PnlOpGetLedCode(AxisRsc->PnlOpLedDataHndl) );
					}
					AxisRsc++;
				}
				/* Output Connect LED Info. */
				HALdrv_NetConLedControl( PnlOpGetConnectStatus(AxisRscC->PnlOpLedDataHndl) );
			}
			AxisRsc = &AxisRscC[0];		/* ポインタを元に戻す */
/* <S0CB> End */

	#else
	HALdrv_TstOutputPnlLedC( );			/* @@ Panel Operator */
	#endif
#else /* _TEST_BOARD_ 削除予定 */
	HALdrv_OutputSerialIO_ScanC();
#endif
	/*----------------------------------------------------------------------*/
	/*		<S045> Output Signals											*/
	/*----------------------------------------------------------------------*/
	//SeqIoOutputSeqSignals( AxisRscC->IoSignals );				/* <S045> 	*/	/* <S0CB> Delte */
/* <S0CB>	Start */
/* <S198> */
//	if( FnCmnCtrl->OutModeFlg)	/* 強制出力モード */
//	{
//		ForceOutSignals.dw = FnCmnCtrl->ForceOutSignalsHW;
//		HALdrv_SetSOutC( 0, ForceOutSignals );
//	}
//	else
	{
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )		/* SGD7W IO入出力対応<S118> */
		{
//-			SeqIoOutputSeqSignals( AxisRsc->IoSignals );
			SeqIoOutputSeqSignals( ax_no, AxisRsc->IoSignals );
			AxisRsc++;
		}
		AxisRsc = &AxisRscC[0];		/* ポインタを元に戻す */
	}
/* <S0CB> End */

	/*----------------------------------------------------------------------*/
	/*		Output Alarm 													*/
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )		/* ALM0,ALM1出力対応<S104> */
	{
		HALdrv_SetAlmOut( ax_no, AxisRsc->AlmManager->Status.AlmFlag );		/* 1st Axis	<S0FF> */
		AxisRsc++;
	}
	AxisRsc = &AxisRscC[0];		/* ポインタを元に戻す */

	/*----------------------------------------------------------------------*/
	/*		Read Temperature <TempSensorIC>									*/
	/*----------------------------------------------------------------------*/
//	TempICI2Cdrv_Read_Temperature((ASIC_HW_IF_STR*)((AxisRscC->ModuleID->AsicNoMap)->asicAddr[0]));

	/*----------------------------------------------------------------------*/
	/*		Make Operator Display Attributes 								*/
	/*----------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )		/* 2軸対応<S1AD> */
	{
		LpxMakeOpDispAttr( AxisRsc );
		AxisRsc++;
	}
	AxisRsc = &AxisRscC[0];		/* ポインタを元に戻す */
	/*----------------------------------------------------------------------*/
	/*		Test Pulse Send for SIL3				<S0F1>					*/
	/*----------------------------------------------------------------------*/
//<S1F6>	HALdrv_ReverceTestPls();
	SftReverceTestPls();	/*<S1F6>*/

}

/****************************************************************************************************/
/*																									*/
/*		TODO: オペレータ状態表示用信号作成															*/
/*		Update signal status of Operator Status Display												*/
/*																									*/
/****************************************************************************************************/
void	LpxMakeOpDispAttr( AXIS_HANDLE *AxisRscC )
{
CTRL_CMD_MNG *CtrlCmdMngr = &(AxisRscC->BaseControls->CtrlCmdMngr);
SEQ_IO_HNDL *IoSignals = AxisRscC->IoSignals; /* <S0B6> */
BPRMDAT	*Bprm = AxisRscC->UniPrms->Bprm;
SEQ_CTRL_OUT *SeqCtrlOut = AxisRscC->SeqCtrlOut;	
SEQ_MOT_STS	*MotSts = AxisRscC->SeqMotSts;
/*--------------------------------------------------------------------------------------------------*/
	switch( CtrlCmdMngr->CtrlMode )
	{
	  /*--------------------------------------------------------------------------------------------*/
		case CTRLMODE_POS:	/* 位置制御モード														*/
	  /*--------------------------------------------------------------------------------------------*/
			 SeqCtrlOut->f.Vrefon  = FALSE;
			 SeqCtrlOut->f.Trefon  = FALSE;
			 SeqCtrlOut->f.Prefon  = (MotSts->var.dPcmdCnt != 0)? TRUE : FALSE;
//			 SeqCtrlOut->f.PerrClr = ( CtrlCmdMngr->PerrClrReq /* BinV.f.PerClrSignal */) ? TRUE : FALSE; /* <S0B6> */
			 SeqCtrlOut->f.PerrClr = (IoSignals->HwSin >> 15) & 1;
			 break;
	  /*--------------------------------------------------------------------------------------------*/
		case CTRLMODE_SPD:	/* 速度制御モード														*/
		case CTRLMODE_JOG:	/* ＪＯＧ運転モード														*/
	  /*--------------------------------------------------------------------------------------------*/
			 if( ( SeqCtrlOut->BeComplete ) 
				&& ( MlibAbs32( CtrlCmdMngr->SpdRefi ) > MotSts->conf.TgonSpdLevel ) )
			 {
				SeqCtrlOut->f.Vrefon = TRUE;
			 }
			 else
			 {
				SeqCtrlOut->f.Vrefon = FALSE;
			 }
			 SeqCtrlOut->f.Prefon  = FALSE;
			 SeqCtrlOut->f.PerrClr = FALSE;
			 break;
	  /*--------------------------------------------------------------------------------------------*/
		case CTRLMODE_TRQ:	/* トルク制御モード														*/
	  /*--------------------------------------------------------------------------------------------*/
			 if( ( SeqCtrlOut->BeComplete ) 
					&& ( MlibAbs32( CtrlCmdMngr->TrqRefo /* AoutV.TrqRefi */ ) > Bprm->TrefonLevel ) )
			 {
				SeqCtrlOut->f.Trefon = TRUE;
			 }
			 else
			 {
				SeqCtrlOut->f.Trefon = FALSE;
			 }
			 SeqCtrlOut->f.Vrefon  = FALSE;
			 SeqCtrlOut->f.Prefon  = FALSE;
			 SeqCtrlOut->f.PerrClr = FALSE;
			 break;
	  /*--------------------------------------------------------------------------------------------*/
		default:			/* その他																*/
	  /*--------------------------------------------------------------------------------------------*/
			 SeqCtrlOut->f.Vrefon  = FALSE;
			 SeqCtrlOut->f.Trefon  = FALSE;
			 SeqCtrlOut->f.Prefon  = FALSE;
			 SeqCtrlOut->f.PerrClr = FALSE;
			 break;
	}
}

/****************************************************************************************************/
/*																									*/
/*		Update status of Base Servo Sequence														*/
/*																									*/
/****************************************************************************************************/
static void SyscUpdateBaseEnableStatus( BE_SEQ_HNDL *BeSeq, LONG AxisNo,
                                       SEQ_CTRL_OUT *SeqCtrlOut, BOOL PowerOn, BOOL faultState,
    								   BOOL MotorLessTestMode )											/* <S1B7> */
{
	BOOL 			BeReqSeqOut;

/*--------------------------------------------------------------------------------------------------*/
/*		Set Main Power Status																		*/
/*--------------------------------------------------------------------------------------------------*/
	SeqCtrlOut->MainPowerOn = PowerOn;

	BeReqSeqOut = BeSeq->BeReqSeqOut;
#ifdef CSW_SVON_INTERLOCK
	/* 複数軸同時サーボオンインタロック */
	BeReqSeqOut = SysMakeServoONInterlockState(&SysBeInterlock,
	                                           BeSeq->BkBeReq,
	                                           BeReqSeqOut,
	                                           AxisRscC->AxisNo);
#endif //CSW_SVON_INTERLOCK

/*--------------------------------------------------------------------------------------------------*/
/*		ベースブロック時位置偏差／制御指令クリア要求(OT時はクリアしない)							*/
/*--------------------------------------------------------------------------------------------------*/
#ifdef _VC_DEBUG_ /* 暫定対応 */
	BeSeq->BkSeqData.var.BkBeReq = TRUE;
#endif

	if( BeSeq->BkSeqData.var.BkBeReq == FALSE )
	{
		SeqCtrlOut->BBSvcRefClrReq = TRUE;				/* ServoOnComp = FALSE */
	}
	else if( BeReqSeqOut || (!BeSeq->OtSeqData.var.OtBeReq) )
	{
		SeqCtrlOut->BBSvcRefClrReq = FALSE;				/* ServoOnComp = TRUE */
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Set Sequence Control Output Data															*/
/*--------------------------------------------------------------------------------------------------*/
	SeqCtrlOut->BaseEnableReq = BeSeq->BeReqSeqOut;
	/* モータレステストモードでない場合のみ faultState を条件に加える *//* <S1B7> */
	if( MotorLessTestMode == FALSE )
	{
		SeqCtrlOut->BeComplete	= BeSeq->BeReqSeqOut & ((faultState & SHAL_FAULT_BB) == 0);
	}
	else
	{
		SeqCtrlOut->BeComplete	= BeSeq->BeReqSeqOut;
	}
	SeqCtrlOut->SvRdyPhaseIgnore = BeSeq->SvReadyPhaseIgnore;
	SeqCtrlOut->UdlBeStatus = ((faultState & SHAL_FAULT_BB) == 0) ? TRUE : FALSE;
}



/****************************************************************************************************/
/*																									*/
/*		モニタ用データ作成																			*/
/*																									*/
/****************************************************************************************************/
static void SyscMakeMonitorData( AXIS_HANDLE *AxisRscC )
{
	LONG				lwk;
	ANLG_UN_MON_DATA	*UnMonitor;
	SEQ_MOT_STS			*MotSts;
//	ASIC_U_IF_STR		*pAsicMicroReg;/*<S0C7>*/
	MENCV				*MencV;
	BPRMDAT				*Bprm;
	POS_MNG_HNDL		*PosManager;	/* <S056> */

	/* ローカル変数の初期化 */
	UnMonitor = AxisRscC->UnMonitor;
	MotSts = AxisRscC->SeqMotSts;
//	pAsicMicroReg = AxisRscC->SvAsicRegs->AsicMicroReg;/*<S0C7>*/
	MencV = AxisRscC->MencV;
	Bprm = AxisRscC->UniPrms->Bprm;
	PosManager = &(AxisRscC->BaseControls->PosManager);			/* <S056> */


/*--------------------------------------------------------------------------------------------------*/
/*		Un000,Un007 : モータ速度,指令パルス速度														*/
/*--------------------------------------------------------------------------------------------------*/
	UnMonitor->UnMotSpd = MotSts->var.UnMotSpd;
	UnMonitor->UnPfbk64ms = MotSts->var.UnPfbk64ms;
	UnMonitor->UnPcmd64ms = MotSts->var.UnPcmd64ms;
	UnMonitor->UnMotSpdx = MotSts->var.MotSpd;					/* 2ms平均モータ速度				*/
	UnMonitor->UnPcmd64Sum = MotSts->var.UnPcmd64Sum;	/* <S1B6> */

/*--------------------------------------------------------------------------------------------------*/
/*		Un002 : トルク指令モニタ																	*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	UnMonitor->UnTrqRef = ( AxisRscC->CtrlLoopOut->TrqRefMon * Bprm->Kmontrq );
#else
	UnMonitor->UnTrqRef = MlibMulgain( AxisRscC->CtrlLoopOut->TrqRefMon, Bprm->Kmontrq );
#endif /* FLOAT_USE */

#if 0 /*  CHECK暫定処理 */
	/*  2013/04/01のSigmaV-MD */

	/*--------------------------------------------------------------------------------------------------*/
	/*		Un003 : 回転角１(回転型：Ｃ相位置，リニア：電気角)	[pulse]									*/
	/*		Un004 : 回転角２(電気角)	[deg]															*/
	/*--------------------------------------------------------------------------------------------------*/
	lwk = pAsicMicroReg->MREG_MOTPHASE + (pAsicMicroReg->MREG_MOTPHASEH << 16);
	UnMonitor->UnMotPhase = SencGetMotPhase( MencV, lwk, Bprm->RvsDir, &UnMonitor->UnMotAngle );
#else
/*--------------------------------------------------------------------------------------------------*/
/*		Un003 : 回転角１(回転型：Ｃ相位置，リニア：電気角)	[pulse]									*/
/*--------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------------------------------------------------*/
	/*		Make Phase Infomation and Motor Angle for Linear										*/
	/*----------------------------------------------------------------------------------------------*/
/* <S214> */
	if( MencV->AxisMotType == MOTTYPE_LINEAR )
	{
		MencV->MotAngle = MlibMulgainNolim( MencV->MotPhase, MencV->Kinvphase );
	}
/* <S214> */
	lwk = MencV->MotAngle;									/* Pulse/Rev						*/
	if( Bprm->RvsDir )
	{ /* Reverse Dir Mode */
		lwk = MencV->PulseNo - lwk;
	}
	if( lwk >= MencV->PulseNo )
	{ /* PulseNo --> 0 */
		UnMonitor->UnMotAngle = 0;
	}
	else
	{
		UnMonitor->UnMotAngle = lwk;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		Un004 : 回転角２(電気角)	[deg]															*/
/*--------------------------------------------------------------------------------------------------*/
	/* 65536 --> 360 deg */
	lwk = ((((USHORT)MencV->MotPhase * 360) >> 15) + 1) >> 1;
	if( Bprm->RvsDir )
	{ /* Reverse Dir Mode */
		lwk = 360 - lwk;
	}
	if( lwk >= 360 )
	{ /* 360 --> 0 */
		UnMonitor->UnMotPhase = 0;
	}
	else
	{
		UnMonitor->UnMotPhase = lwk;
	}
#endif
/*--------------------------------------------------------------------------------------------------*/
/*		Un009 : 実効トルク用データ 	( 10sec２乗電流指令平均値 )			UnIsqrSum					*/
/*		Un142 : 実効トルク用データ	( 2ms２乗電流指令値)				UnCrntRef					*/
/*		Un145 : 実効トルク用データ	( 10sec２乗電流指令平均最大値)		UnIsqrSumMax				*/
/*--------------------------------------------------------------------------------------------------*/
	UnMonitor->UnCrntRef = MotSts->var.UnCrntRef;
	UnMonitor->UnIsqrSum = MotSts->var.UnIsqrSum;
	UnMonitor->UnIsqrSumMax = MotSts->var.UnIsqrSumMax;

/*--------------------------------------------------------------------------------------------------*/
/*		@@ Un00C : 指令パルスカウンタ [pulse]														*/
/*--------------------------------------------------------------------------------------------------*/
	UnMonitor->UnPcmdCntr = UnMonitor->UnPcmdCntr + MotSts->var.dPcmdCnt;

/*--------------------------------------------------------------------------------------------------*/
/*		Un00D : ＦＢパルスカウンタ [pulse]															*/
/*--------------------------------------------------------------------------------------------------*/
	UnMonitor->UnPfbkCntr = UnMonitor->UnPfbkCntr + MotSts->var.dMotPos;

/* <S056>	Start */
/*--------------------------------------------------------------------------------------------------*/
/*		Un013 : ＦＢパルスカウンタ [指令単位]														*/
/*--------------------------------------------------------------------------------------------------*/
	UnMonitor->UnPfbkCntr2 += MlibEgearRvscnv( MotSts->var.dMotPos, &PosManager->conf.Egear, &UnMonitor->RemCntr2 ) ;
/* <S056>	End */


/*--------------------------------------------------------------------------------------------------*/
/*		Un00B : ＤＢ抵抗消費電力用データ(10secＤＢ消費電力平均値)	CoutV.UnMeanDbPower				*/
/*		Un144 : ＤＢ抵抗消費電力用データ(2msＤＢ消費電力値)			CoutV.UnDBPower					*/
/*--------------------------------------------------------------------------------------------------*/
	UnMonitor->UnDbPower = AxisRscC->CheckAlarm->DbOL.var.UnDbPower;
	UnMonitor->TrcDbPower = AxisRscC->CheckAlarm->DbOL.var.UnDbPower;
	UnMonitor->UnMeanDbPower = AxisRscC->CheckAlarm->DbOL.var.UnMeanDbPower;

/*--------------------------------------------------------------------------------------------------*/
/*		Un141 : 電流検出値（２乗電流ＦＢ値） <S0DA>													*/
/*--------------------------------------------------------------------------------------------------*/
	lwk = (AxisRscC->CtrlLoopOut->IdFbMon * AxisRscC->CtrlLoopOut->IdFbMon)
			+ (AxisRscC->CtrlLoopOut->IqFbMon * AxisRscC->CtrlLoopOut->IqFbMon);
	UnMonitor->UnCrntDet = MlibSqrtu32( lwk );

/*--------------------------------------------------------------------------------------------------*/
/*		Un141 : 電流検出値（２乗電流ＦＢ値） 														*/
/*--------------------------------------------------------------------------------------------------*//* <S1D7> */
	UnMonitor->UnInstEnvMonAmp = (AxisRscC->EnvMonitor->var.UnInstEnvMonAmp * 0.0001)
									+ AxisRscC->EnvMonitor->conf.AmpEnvMonOffset;		/* Un025	*/

	UnMonitor->UnInstEnvMonMot = (AxisRscC->EnvMonitor->var.UnInstEnvMonMot * 0.001)
										+ AxisRscC->EnvMonitor->conf.MotEnvMonOffset;	/* Un026	*/
/* <S049>	Start */
/*--------------------------------------------------------------------------------------------------*/
/*		サーボステータスモニタ用データ作成															*/
/*--------------------------------------------------------------------------------------------------*/
//	UnMonitor->SvStatus = MakeServoStatus(AxisRscC); /* <S0A1> */
	MakeServoStatus( AxisRscC );
/* <S049>	End */
}

/****************************************************************************************************/
/*																									*/
/*		ScanC 絶対値エンコーダ処理メイン									<S02D>					*/
/*																									*/
/****************************************************************************************************/
void SyscSencAbsoOperation( AXIS_HANDLE *AxisRscC )
{
#if	(SVFSEL_DIVOUT_USE == TRUE) 
/*--------------------------------------------------------------------------------------------------*/
BPRMDAT 		*Bprm;
/*--------------------------------------------------------------------------------------------------*/
/* DbSequence BaseEnable Request */
BOOL	DbBeReq = AxisRscC->BeSequence->DbSeqData.DbBeReq;
/* アラーム発生中フラグ */
BOOL	almFlag = ((AxisRscC->AlmManager->Status.AlmFlag) ? TRUE : FALSE);	/* <S0FF> */
/* SEN Signal */
//BOOL	SenReq = AxisRscC->BeSequence->SenStatus;		/* <S03F> */
//<S082> BOOL	SenReq = (AxisRscC->BaseCtrlOut->CmdSeqBit & SENSOR_ON_BIT) >> SENSOR_ON_BITNO;
BOOL	SenReq = AxisRscC->BeSequence->SenStatus;		/* <S082> */
/* アラームコード情報 */
USHORT	AlmCode = AxisRscC->AlmManager->AlmCode;

/*--------------------------------------------------------------------------------------------------*/
Bprm  			= AxisRscC->UniPrms->Bprm;
/*--------------------------------------------------------------------------------------------------*/
//#if	(SVFSEL_DIVOUT_USE == TRUE) 

   if( Bprm->FencUse == FALSE )
   { /* ｾﾐｸﾛｰｽﾞ制御の場合 */
	   	if( (AxisRscC->MencV->AbsoEncType != MENC_INCRE_TYPE) && (AxisRscC->MencV->DivInfo.DivOut != FALSE) )
		{ /* 絶対値ｴﾝｺｰﾀﾞ接続の時、分周出力機能有効の場合 */
			/*------------------------------------------------------------------------------------------*/
			/*	回転形モータ＆リニアモータ: シリアルエンコーダＰＡＯ相シリアル出力					    */
			/*------------------------------------------------------------------------------------------*/
			SencOutputPaoSerial( AxisRscC->MencV,
								 Bprm,
								 AxisRscC->SvAsicRegs->AsicHwReg,
								 DbBeReq,
								 SenReq,
								 AlmCode,
								 almFlag );
			/* <S189> PSO_Output*//*<S0C2>*/
			SencOutputPsoSerial(
								 AxisRscC->MencV,
								 AxisRscC->SvAsicRegs->AsicHwReg,
								 AxisRscC->UniPrms->Prm->MencP.limmlt,
								 Bprm->RvsDir
								 );
		}
   }
#if	(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)
   else
   { /* ﾌﾙｸﾛｰｽﾞ制御の場合 */
	   	if( (AxisRscC->FencV->AbsoEncType != MENC_INCRE_TYPE) && (AxisRscC->FencV->DivInfo.DivOut != FALSE) )
		{ /* 絶対値ｴﾝｺｰﾀﾞ接続の時、分周出力機能有効の場合 */
			/*------------------------------------------------------------------------------------------*/
			/*	フルクローズド制御:	シリアルエンコーダＰＡＯ相シリアル出力   					        */
			/*------------------------------------------------------------------------------------------*/
			FencSencOutputPaoSerial( AxisRscC->FencV,
									 Bprm,
									 AxisRscC->SvAsicRegs->AsicHwReg,
									 DbBeReq,
									 SenReq,
									 AlmCode,
									 almFlag );
			/*<S189> PSO_Output*//*<S0C2>*/
			FencSencOutputPsoSerial(
									AxisRscC->FencV,
									AxisRscC->SvAsicRegs->AsicHwReg,
									AxisRscC->UniPrms->Prm->MencP.limmlt,
									Bprm->RvsDir
								   );
		}
   }
#endif//(CSW_SVCF_FULC_MODE != CSW_FULC_NO_SUPPORT)

#endif	/*(SVFSEL_DIVOUT_USE == TRUE)*/

/*--------------------------------------------------------------------------------------------------*/
}

/* <S049>	Start */
/****************************************************************************************************/
/*																									*/
/*		サーボステータスモニタ用データ作成															*/
/*																									*/
/*			A/P以外は本処理にて7SegLEDの状態モニタ作成を行う。										*/
/*																									*/
/****************************************************************************************************/
//USHORT MakeServoStatus( AXIS_HANDLE *AxisRscC )/* <S0A1> */
void MakeServoStatus( AXIS_HANDLE *AxisRscC )
{
	MENCV				*MencV;
	ANLG_UN_MON_DATA	*UnMonitor;
	BASE_LOOPCTRLS		*BaseLoops;
	BASE_CTRL_OUT		*BaseCtrlOut;
	SEQ_CTRL_OUT		*SeqCtrlOut;
	USHORT				SvStatus;
	USHORT				LedDispStatus;
	BOOL				BeComplete;
	BOOL				spot;
	BOOL				snot;

	/* ローカル変数の初期化 */
	UnMonitor = AxisRscC->UnMonitor;
	MencV = AxisRscC->MencV;
	BeComplete = AxisRscC->BaseLoops->BaseCtrls->BeComplete;
	BaseLoops = AxisRscC->BaseLoops;
	BaseCtrlOut = AxisRscC->BaseCtrlOut;
	SeqCtrlOut = AxisRscC->SeqCtrlOut;

	SvStatus = 0x0000;
	LedDispStatus = 0x00;

	if( ALMGetAlarmInfo( AxisRscC->AlmManager ) != 0x00 )	/* アラーム・ワーニング発生中 */
	{
		SvStatus = 0x0001;
		if(BeComplete) /* <S14D> */
		{
			LedDispStatus |= STS_SV_ON;
		}
	}
	else
	{
		/* RUN or BB	*/
		if(BeComplete)
		{
			if( !MencV->PhaseReady )	/* 位相検出完了Flag = On */
			{ /* 磁極検出中 */
				SvStatus = 0x0008;
//				LedDispStatus |= STS_SV_ON; /* <S0A1> *//* <S10C> */
			}
			else						/* 位相検出完了Flag = Off */
			{ /* 通常サーボオン中 */
				SvStatus = 0x0007;
			}
			//STS_SV_ON;//<S0C7>
			LedDispStatus |= STS_SV_ON; /* <S0A1> *//* <S10C> */
		}
		else
		{
			SvStatus = 0x0006;
		}

		/* P-OT信号､N-OT信号の状態取得 */
		spot = (BaseCtrlOut->CmdSeqBit >> POSITIVE_OT_BITNO) & 0x1;
		snot = (BaseCtrlOut->CmdSeqBit >> NEGATIVE_OT_BITNO) & 0x1;

#if defined(MCCSW_SGDVCMOP)
/* TODO */
/* 指令オプション対応時に処理を追加してください。 */
#else
		if( BaseLoops->CtrlMode != CTRLMODE_JOG && BaseLoops->CtrlMode != CTRLMODE_ZSRCH)
		{
			/* POT/NOT	*/
			if( (AxisRscC->IoSignals->PotSignal.SignalStatus.v | spot) && (AxisRscC->IoSignals->NotSignal.SignalStatus.v | snot) )
			{
				SvStatus = 0x0003;
				LedDispStatus |= STS_P_OT | STS_N_OT; /* <S0A1> */
			}
			else if(AxisRscC->IoSignals->PotSignal.SignalStatus.v | spot)
			{
				SvStatus = 0x0004;
				LedDispStatus |= STS_P_OT;
			}
			else if(AxisRscC->IoSignals->NotSignal.SignalStatus.v | snot)
			{
				SvStatus = 0x0005;
				LedDispStatus |= STS_N_OT;
			}
		}
#endif
	}

	/* HWBB */
	if( SeqCtrlOut->HwbbSts == TRUE ) /* <S0A1> */
	{
		LedDispStatus |= STS_HBB;
	}

	/* TGON */
	if( SeqCtrlOut->TgOnSts == TRUE ) /* <S0A1> */
	{
		LedDispStatus |= STS_TGON;
	}

	/* DEN / VREF / TREF */
	switch (BaseLoops->BaseCtrls->CtrlModeSet.CtrlMode.b.cm) /* <S0A1> */
	{
	case CTRLMODE_POS:
		LedDispStatus |= (BaseLoops->BaseCtrls->PosCtrlSts.DenSignal) ? STS_DEN : 0;
		break;
#if 0 /* 将来用 *//* <S10C> */
	case CTRLMODE_SPD:
		LedDispStatus |= (BaseLoops->BaseCtrls->VrefInSts) ? 0 : STS_DEN;
		break;
	case CTRLMODE_TRQ:
		LedDispStatus |= (BaseLoops->BaseCtrls->TrefInSts) ? 0 : STS_DEN;
		break;
#endif
	default:
		break;
	}

	if ( SeqCtrlOut->FstpSts == TRUE)   /* <S1F5> */
	{
		SvStatus = 0x000A;
		LedDispStatus |= STS_FSTP;
	}
	/* TODO :　0ｘ010A、0ｘ020A　理由：安全機能実装されていない*/


	/*SafetyModuleｺｰﾄﾞ*/
	/* TODO */
	/*SafetyModule対応時に処理を追加してください。*/


	UnMonitor->SvStatus = SvStatus;
	UnMonitor->LedDispStatus = LedDispStatus;

	return;
}
/* <S049>	End */


/***************************************** end of file **********************************************/
