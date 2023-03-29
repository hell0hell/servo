/****************************************************************************************************/
/*																									*/
/*																									*/
/*		SequenceIo.c : シーケンスI/O管理処理														*/
/*																									*/
/*																									*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : サーボシーケンス管理処理																*/
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
/*	Note	:	初版	2010.02.23	Y.Oka	For INGRAM												*/
/*																									*/
/*	Log		:	Ver1.00																				*/
/*																									*/
/****************************************************************************************************/
#include "Basedef.h"
#include "SequenceIo.h"


/* 外部関数 */
extern void HALdrv_SetSOutC( int axis, HWSOUT hwsout );


#if 0	/*<S1F6>*/
/****************************************************************************************************/
/*																									*/
/*		セーフティＨＷＢＢ状態検出処理																*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : --ハードワイヤードベースブロック状態（HWBB状態）の検出								*/
/*			BBON1LT/BBON2LT信号のどちらかもしくは両方が1となった場合（HWBB状態中）に、				*/
/*			HWBB状態を示すフラグにTRUEをセットする。両方の信号が0の場合（HWBB状態解除時）には、		*/
/*			HWBB状態を示すフラグにFALSEをセットする。また、HWBB状態解除時には入力エッジ検出結果		*/
/*			もクリアする。																			*/
/*			BBONLT信号のラッチ値をクリアするタイミングは、サーボオフ入力かつラウンド運転権を		*/
/*			取得していない場合に行う。																*/
/*			--BBON信号入力時間差異常アラーム検出用のフラグセット									*/
/*			BBON1LT/BBON2LT信号のどちらかの入力エッジを検出した際に、入力時間差フラグに				*/
/*			TRUEをセットする。BBON1/BBON2信号の両方の入力エッジを検出した際に、入力時間差フラグに	*/
/*			FALSEをセットする。両方の入力エッジを検出できない場合は、前回の状態を保持する。			*/
/*																									*/
/*	引 数 : なし																					*/
/*　戻り値: なし																					*/
/*																									*/
/****************************************************************************************************/
BOOL SeqIoDetectHardWiredBB( DET_HWBB_STS *DetHwbb, LONG svon_req,
										ASIC_HW_IF_STR *pAsicHwReg, BOOL HwbbSupport )
{
	BOOL	Bbon1Lt, Bbon2Lt;
	BOOL	HwbbSts;

/*--------------------------------------------------------------------------------------------------*/
/*		ハードワイヤードベースブロック状態チェック													*/
/*--------------------------------------------------------------------------------------------------*/
	/* サーボオフ入力かつラウンド運転権の取得なしかつ磁極検出要求なし */
//	if( (0 == (CmdSeqBit & SERVO_ON_BIT)) && (0 == (CmdSeqBit & POL_DETECT_BIT)) && (!FnSvControl) )
	if(svon_req == 0)
	{
		/* BBON信号のラッチ値をクリア */
		SHalDrv_ResetBBONLtStatus( pAsicHwReg );
	}

	/* 現在のBBON信号状態読み込み（モニタ用） */
	SHalDrv_GetBBONStatus( pAsicHwReg, &DetHwbb->Bbon1, &DetHwbb->Bbon2 );

	/* 現在のBBON信号ラッチ値読み込み（HWBB検出用） */
	SHalDrv_GetBBONLtStatus( pAsicHwReg, &Bbon1Lt, &Bbon2Lt );

	/* BBON信号のエッジ検出 */
	if( Bbon1Lt )
	{
		DetHwbb->Bbon1Edge = TRUE;
	}
	if( Bbon2Lt )
	{
		DetHwbb->Bbon2Edge = TRUE;
	}

	/* HWBB状態設定処理 */
	if( (Bbon1Lt) || (Bbon2Lt) )
	{
		/* BBON信号どちらかでも入力あり */
		if( HwbbSupport == TRUE )
		{ /* HWBB機能対応機種 */
			DetHwbb->HwbbState = TRUE;
			DetHwbb->GateDrvDefect = FALSE;
		}
		else
		{ /* HWBB機能未対応機種 */
			DetHwbb->HwbbState = FALSE;
			DetHwbb->GateDrvDefect = TRUE;
		}
	}
	else
	{
		/* BBON信号どちらも入力なし */
		if( (DetHwbb->Bbon1Edge) && (DetHwbb->Bbon2Edge) )
		{
			/* BBON1/BBON2信号のエッジを両方検出した時のみHWBB状態解除 */
			DetHwbb->HwbbState = FALSE;
			DetHwbb->Bbon1Edge = FALSE;
			DetHwbb->Bbon2Edge = FALSE;
		}
	}

	/* Set ScanC Output Data */
	if( DetHwbb->HwbbState )
	{
		HwbbSts = TRUE;
	}
	else
	{
		HwbbSts = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		入力信号の時間差異常検出タイマスタートフラグの設定											*/
/*--------------------------------------------------------------------------------------------------*/
	/* BBON信号入力時間差フラグ設定処理 */
	if( (DetHwbb->Bbon1Edge) ^ (DetHwbb->Bbon2Edge) )
	{
		/* BBON信号どちらかのみの入力 */
		DetHwbb->LackOneBbon = TRUE;
	}
	else
	{
		/* BBON信号両方の入力 */
		if( (DetHwbb->Bbon1Edge) && (DetHwbb->Bbon2Edge) )
		{
			DetHwbb->LackOneBbon = FALSE;
		}
	}

	return	HwbbSts;
}
#endif	//#if 0	/*<S1F6>*/

#if 0	/*<S1F6>*/
/****************************************************************************************************/
/*																									*/
/*		セーフティ機能　内部信号状態検出処理											<S0F1>		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*	機 能 : 内部信号状態検出用DPIN1, DPIN2信号の状態を検出する										*/
/*          DPIN1, DPIN2信号有効/無効状態を検出する													*/
/*																									*/
/*	引 数 :																							*/
/*　戻り値:																							*/
/*																									*/
/****************************************************************************************************/
void SeqIoDetectTestPulseSignal( SEQ_IO_HNDL *pIoSignals)
{
	TEST_PULSE_DIAG *pTestPulseDiag;
	DET_HWBB_STS *pDetHwbbSts;

	pTestPulseDiag = &(pIoSignals->TestPulseDiag);
	pDetHwbbSts    = &(pIoSignals->DetHwbbSts);
/*--------------------------------------------------------------------------------------------------*/
/*		信号の前回値を更新																			*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag->DpIn1LastState = pTestPulseDiag->DpIn1State;
	pTestPulseDiag->DpIn2LastState = pTestPulseDiag->DpIn2State;

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1信号の有効／無効状態判別 (BBON状態でない && 前回DPIN1信号有効 ⇒ 今回DPIN1信号は有効	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon1)) && (pTestPulseDiag->DpIn1LastEnable) )
	{
		pTestPulseDiag->DpIn1Enable = TRUE;
	}
	else
	{
		pTestPulseDiag->DpIn1Enable = FALSE;
	}
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN2信号の有効／無効状態判別 (BBON状態でない && 前回DPIN2信号有効 ⇒ 今回DPIN2信号は有効	*/
/*--------------------------------------------------------------------------------------------------*/
	if( (!(pDetHwbbSts->Bbon2)) && (pTestPulseDiag->DpIn2LastEnable) )
	{
		pTestPulseDiag->DpIn2Enable = TRUE;
	}
	else
	{
		pTestPulseDiag->DpIn2Enable = FALSE;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2信号の状態取得																	*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag->DpIn1State = GET_DPIN1( );
	pTestPulseDiag->DpIn2State = GET_DPIN2( );
/*--------------------------------------------------------------------------------------------------*/
/*		DPIN1/DPIN2状態(前回値)の更新																*/
/*--------------------------------------------------------------------------------------------------*/
	pTestPulseDiag->DpIn1LastEnable  = !(pDetHwbbSts->Bbon1);
	pTestPulseDiag->DpIn2LastEnable  = !(pDetHwbbSts->Bbon2);
}
#endif	/*<S1F6>*/

/* <S1F5> Start */
/*****************************************************************************
  Description: FSTP状態検出処理
------------------------------------------------------------------------------
  Parameters:
  	FstpSignal;			Fstp 信号読み込み状態
	hSeqCtrlOut;		Sequence Control Message
	svon_req;			サーボオフ入力
  Return:
	BOOL				FSTP状態
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
BOOL SeqIoDetectFstp( BOOL FstpSignal, SEQ_CTRL_OUT *hSeqCtrlOut, LONG Svon_req )
{
	/* サーボオフ入力かつラウンド運転権の取得なしかつ磁極検出要求なし */
	if ( Svon_req == 0 )
	{
		hSeqCtrlOut->FstpSts = FstpSignal;
	}
	/*FSTP信号が入力された場合*/
	if ( FstpSignal == TRUE )
	{
		hSeqCtrlOut->FstpSts = TRUE;
	}

	return hSeqCtrlOut->FstpSts;
}

/* <S1F5> End */

/****************************************************************************************************/
/*																									*/
/*		保持ブレーキ制御処理																		*/
/*																									*/
/****************************************************************************************************/
/*																									*/
/*		BKシーケンス未検出、または、NET-I/Fからブレーキ解除要求ありの場合にBK解除状態				*/
/*		BKシーケンス検出、かつ、NET-I/Fからの解除要求なしの場合にBK状態								*/
/*																									*/
/****************************************************************************************************/
#ifndef HW_FBA_IO_AXIS
void SeqIoBrakeControl( SEQ_IOSIGNAL *BkSignal, BOOL *BrakeSts,
											BOOL SvBrakeReq, BOOL NetBrkReleaseReq )
{
//	SEQ_IOSIGNAL		*SeqIo;/*<S0C7>*/

	/* BKシーケンス検出なし、または、NET-I/F解除要求あり */
	if( (SvBrakeReq == FALSE) || (NetBrkReleaseReq == TRUE) )
	{ /* BRAKE開放状態(BK Signal -> active) */
		*BrakeSts = TRUE;
	}
	else
	{ /* BRAKE状態(BK Signal -> inactive) */
		*BrakeSts = FALSE;
	}
//<S045>	SeqIo = BkSignal;
//<S045>	SeqIo->SignalStatus = (*BrakeSts & SeqIo->Mask) ^ SeqIo->XorMask;
//<S045>	HALdrv_BrakeControl( SeqIo->PinNumber, SeqIo->SignalStatus );
}
#else
void SeqIoBrakeControlAx( SEQ_IOSIGNAL *BkSignal, BOOL *BrakeSts,
								BOOL SvBrakeReq, BOOL NetBrkReleaseReq, SERIAL_OUTPUT_AX *SerialOutputAx )
{
	SEQ_IOSIGNAL		*SeqIo;

	/* BKシーケンス検出なし、または、NET-I/F解除要求あり */
	if( (SvBrakeReq == FALSE) || (NetBrkReleaseReq == TRUE) )
	{ /* BRAKE開放状態(BK Signal -> active) */
		*BrakeSts = TRUE;
	}
	else
	{ /* BRAKE状態(BK Signal -> inactive) */
		*BrakeSts = FALSE;
	}
	SeqIo = BkSignal;
	SeqIo->SignalStatus = (*BrakeSts & SeqIo->Mask) ^ SeqIo->XorMask;
	HALdrv_BrakeControlAx( SeqIo->PinNumber, SeqIo->SignalStatus, SerialOutputAx );
}
#endif

/****************************************************************************************************/
/*																									*/
/*		スキャンＣネットワークＩＦ出力処理															*/
/*																									*/
/****************************************************************************************************/
void SeqIoOutputNetIf( SEQ_IO_HNDL *SeqIo, BE_SEQ_HNDL *BeSeq, SEQ_CTRL_OUT *SeqCtrlOut,
								 ALMMAN_HANDLE *AlmManager, MENCV *MencV/* <S082> BOOL PhaseReady */, BOOL LockNormalOpeDrive,
								 BOOL MotorLessTestMode )											/* <S1B7> */
{
	LONG	RspSeqBit, RspSigBit, RspCtrlBit;
	BOOL	sts;

	RspSeqBit = 0;
	RspSigBit = 0;
	RspCtrlBit = 0;

/*--------------------------------------------------------------------------------------------------*/
/*		シーケンス制御状態作成																		*/
/*--------------------------------------------------------------------------------------------------*/
	/* NetAlarm */
	RspSeqBit |= (((AlmManager->Status.AlmFlag) ? TRUE : FALSE) << ALM_DETECTED_BITNO);	/* <S0FF> */

	/* NetWarning */
	RspSeqBit |= (((AlmManager->Status.WrnFlag) ? TRUE : FALSE) << WRN_DETECTED_BITNO); /* <S0FF> */

	/* AlarmRstComp */
	RspSeqBit |= ((1^AlmManager->AlmRst) << ALM_RESET_COMP_BITNO);
	RspSeqBit |= ((1^AlmManager->AlmRst) << WRN_RESET_COMP_BITNO);

	/* ServoOnComp */
	sts = (1^SeqCtrlOut->BBSvcRefClrReq) & MencV->PhaseReady /*<S082> PhaseReady*/;
	RspSeqBit |= (sts << SERVO_ON_COMP_BITNO);

	/* SensOnComp */
//	sts = BeSeq->SenStatus;/* & BeSeq->EncReady*/;
	sts = BeSeq->SenStatus & MencV->EncReady /* <S082> */;
	RspSeqBit |= (sts << SENS_ON_COMP_BITNO);

	/* PolDetectComp */
	RspSeqBit |= (MencV->PhaseReady/* <S082> PhaseReady */<< POL_DET_COMP_BITNO);

	/* モータレステストモードでないとき設定する *//* <S1B7> */
	/* TODO:この処理が適当であるか不明 */
	if( MotorLessTestMode == FALSE )
	{
		/* BKシーケンス検出なし、または、NET-I/F解除要求あり 											*/
		RspSeqBit |= (SeqCtrlOut->BrkSigSts << BRK_RELEASING_BITNO);
	}

	/* Main Power ON */
	RspSeqBit |= (SeqCtrlOut->MainPowerOn << MAIN_POWER_ON_BITNO);

	/* サーボレディ信号 */
	RspSeqBit |= (BeSeq->ServoReady << SERVO_READY_BITNO);

	/* Motorレディ信号 */
	RspSeqBit |= (BeSeq->SvReadyPhaseIgnore << MOTOR_READY_BITNO);

	SeqCtrlOut->RspSeqBits = RspSeqBit;

/*--------------------------------------------------------------------------------------------------*/
/*		入力信号状態作成																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* HWBB */
	RspSigBit |= ((SeqCtrlOut->HwbbSts | SeqCtrlOut->FstpSts) << HWBB_SIG_BITNO);  /* <S1F5> */
	/* HWBB1 */
	RspSigBit |= (SeqIo->DetHwbbSts.Bbon1 << HWBB1_SIG_BITNO);
	/* HWBB2 */
	RspSigBit |= (SeqIo->DetHwbbSts.Bbon2 << HWBB2_SIG_BITNO);
	/* P-OT */
	RspSigBit |= (SeqCtrlOut->PotSigSts << P_OT_SIG_BITNO);
	/* N-OT */
	RspSigBit |= (SeqCtrlOut->NotSigSts << N_OT_SIG_BITNO);
	/* E-STOP */
	RspSigBit |= ((SeqCtrlOut->EstpSigSts | SeqCtrlOut->FstpSts) << ESTP_SIG_BITNO); /* <S1F5> */

	/* DEC */
	RspSigBit |= (SeqIo->DecSignal.SignalStatus.v << DEC_SIG_BITNO);		/* <S045> */
	/* EXT1 */
	RspSigBit |= (SeqIo->ExtSignal[0].SignalStatus.v << EXT1_SIG_BITNO);	/* <S045> */
	/* EXT2 */
	RspSigBit |= (SeqIo->ExtSignal[1].SignalStatus.v << EXT2_SIG_BITNO);	/* <S045> */
	/* EXT3 */
	RspSigBit |= (SeqIo->ExtSignal[2].SignalStatus.v << EXT3_SIG_BITNO);	/* <S045> */

	SeqCtrlOut->RspSigBits = RspSigBit;

/*--------------------------------------------------------------------------------------------------*/
/*		制御状態作成																			*/
/*--------------------------------------------------------------------------------------------------*/
	/* SAFETY_STOP_BIT */
	RspCtrlBit |= (SeqCtrlOut->HwbbSts << SAFETY_STOP_BITNO);
	/* モータ回転状態 */
	RspCtrlBit |= (SeqCtrlOut->TgOnSts << MOTOR_MOVING_BITNO);
	/* 速度一致検出処理 */
	RspCtrlBit |= (SeqCtrlOut->VcmpSts << REACH_SPD_BITNO);
	/* 指令受付可能状態	*/
	RspCtrlBit |= ((1^LockNormalOpeDrive) << OP_ENABLED_BITNO);

	SeqCtrlOut->RspCtrlBits = RspCtrlBit;
}




/****************************************************************************************************/
/*																									*/
/*		OT信号マスク設定用API																		*/
/*																									*/
/****************************************************************************************************/
void SeqIoSetOtSigMask( SEQ_IO_HNDL *SeqIo )
{
	SEQ_IOSIGNAL	*IoSignal;

	/* Mask P-OT Signal */
	IoSignal = &(SeqIo->PotSignal);
	IoSignal->Mask = 0;
	IoSignal->XorMask = 0;
	IoSignal->PinNumber = 0;

	/* Mask N-OT Signal */
	IoSignal = &(SeqIo->NotSignal);
	IoSignal->Mask = 0;
	IoSignal->XorMask = 0;
	IoSignal->PinNumber = 0;
}


/****************************************************************************************************/
/*																									*/
/*		OT信号マスク解除用API																		*/
/*																									*/
/****************************************************************************************************/
void SeqIoClearOtSigMask( SEQ_IO_HNDL *SeqIo, USHORT pot_selsw, USHORT not_selsw )
{
	SEQ_IOSIGNAL	*IoSignal;

	/* P-OT mapping */
	IoSignal = &(SeqIo->PotSignal);
	switch( pot_selsw >> 12 )
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)(pot_selsw & 0xFF);
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)(pot_selsw & 0xFF);
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}

	/* N-OT mapping */
	IoSignal = &(SeqIo->NotSignal);
	switch( not_selsw >> 12 )
	{
	case 1:	/* Switch OFF => active(Normal) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 1;
		IoSignal->PinNumber = (ULONG)( not_selsw & 0xFF );
		break;
	case 2:	/* Switch OFF => inactive(Reverse) */
		IoSignal->Mask		= 1;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = (ULONG)( not_selsw & 0xFF );
		break;
	default:/* Always inactive */
		IoSignal->Mask		= 0;
		IoSignal->XorMask   = 0;
		IoSignal->PinNumber = 0;
		break;
	}
}

/****************************************************************************************************/
/*																									*/
/*		<S045> スキャンＣシーケンス信号入力処理														*/
/*																									*/
/****************************************************************************************************/
void SeqIoInputSeqSignals( SEQ_IO_HNDL *IoSignals, ASIC_HW_IF_STR *AsicHwReg)
{
	HWSIN	SeqInputSignals = HALdrv_ReadInputSignals();
/*--------------------------------------------------------------------------------------------------*/
/*	シーケンス入力																					*/
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_START_INSIGX( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_GET_INSIGX( PotSignal, SeqInputSignals.dw );			/* P-OT */
	SEQIO_GET_INSIGX( NotSignal, SeqInputSignals.dw );			/* N-OT */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_END_INSIGX( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
}

/* <S21D> Start */
/*****************************************************************************
  Description:	スキャンＣシーケンス信号入力処理(軸共通)
------------------------------------------------------------------------------
  Parameters:
	IoSignals;				シーケンスI/O
	Prm;					パラメータデータ
  Return:
------------------------------------------------------------------------------
  Note:

*****************************************************************************/
void SeqIoInputSeqSignalsCom( SEQ_IO_HNDL *IoSignals, PRMDATA *Prm )
{
	/*--------------------------------------------------------------------------------------------------*/
	/*	シーケンス入力																					*/
	/*--------------------------------------------------------------------------------------------------*/
#if CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE
	UINT			HwSin;
	HwSin = IoSignals->HwSin;
	SEQIO_START_INSIGX_AP( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_GET_INSIGX_AP( ClinkansSignal, HwSin, ((Prm->insel7 >> 0) & 0x0F) );		/* /CLINKANS */
	SEQIO_GET_INSIGX_AP( CmcansSignal, HwSin, ((Prm->insel7 >> 4) & 0x0F) );		/* /CMCANS */
	SEQIO_GET_INSIGX_AP( ConvreadySignal, HwSin, ((Prm->insel7 >> 8) & 0x0F) );		/* /CONV_READY */
	SEQIO_GET_INSIGX_AP( ConvmconSignal, HwSin, ((Prm->insel7 >> 12) & 0x0F) );		/* /CONV_MCON */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_END_INSIGX_AP( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
#else
	HWSIN	SeqInputSignals = HALdrv_ReadInputSignals();
	SEQIO_START_INSIGX( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_GET_INSIGX( ClinkansSignal, SeqInputSignals.dw );		/* /CLINKANS */
	SEQIO_GET_INSIGX( CmcansSignal, SeqInputSignals.dw );		/* /CMCANS */
	SEQIO_GET_INSIGX( ConvreadySignal, SeqInputSignals.dw );	/* /CONV_READY */
	SEQIO_GET_INSIGX( ConvmconSignal, SeqInputSignals.dw );		/* /CONV_MCON */
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_END_INSIGX( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
#endif
}
/* <S21D> End */

/****************************************************************************************************/
/*																									*/
/*		<S045> スキャンＣシーケンス信号出力処理														*/
/*																									*/
/****************************************************************************************************/
void SeqIoOutputSeqSignals( LONG ax, SEQ_IO_HNDL *SeqIo )		/* SGD7W IO入出力対応<S118> */
{
	HWSOUT OutX; OutX.dw = 0;
/*--------------------------------------------------------------------------------------------------*/
/*		シーケンス出力																				*/
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_START_OUTSIGX( SeqIo );					/* Start Output Signal */
/*--------------------------------------------------------------------------------------------------*/
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) /* <S04B> */
	SEQIO_SET_OUTSIGX( BkSignal,   OutX.dw );		/* /BK */
	SEQIO_SET_OUTSIGX( VcmpSignal, OutX.dw );		/* /VCMP */
	SEQIO_SET_OUTSIGX( TgonSignal, OutX.dw );		/* /TGON */
	SEQIO_SET_OUTSIGX( SrdySignal, OutX.dw );		/* /S-RDY */
	SEQIO_SET_OUTSIGX( WarnSignal, OutX.dw );		/* /WARN */
#endif
/*--------------------------------------------------------------------------------------------------*/
	SEQIO_SET_OUTSIGX( PreWrnSignal, OutX.dw );		/* /PreWrn */ /* <S154> */
	SEQIO_SET_OUTSIGX( ClinkSignal, OutX.dw );		/* /CLINK *//* <S21D> */
	SEQIO_SET_OUTSIGX( CmconSignal, OutX.dw );		/* /CMCON *//* <S21D> */
#if (CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE) /* <S04B> */
	SEQIO_SET_OUTSIGX( Alo1Signal, OutX.dw );		/* ALO1 */
	SEQIO_SET_OUTSIGX( Alo2Signal, OutX.dw );		/* ALO2 */
	SEQIO_SET_OUTSIGX( Alo3Signal, OutX.dw );		/* ALO3 */
#endif
/*--------------------------------------------------------------------------------------------------*/
//-	SEQIO_END_OUTSIGX_ScanC( OutX );				/* End Output Signal */
	SEQIO_END_OUTSIGX_ScanC( ax, OutX );			/* End Output Signal */
}

/***************************************** end of file **********************************************/
