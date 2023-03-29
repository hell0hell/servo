/*****************************************************************************
Description		: アラームラッチ機能処理
-----------------------------------------------------------------------------
Author		: SHCORE,
			Copyright (c) 2013 All Rights Reserved

Project		: Mercury

Functions	  :
	-- APIs --
		AlmLatchMonitorInitial		アラームラッチ初期化処理
		AlmLatchMonitorSequence		アラームラッチ処理

	-- Locals --
		AlmLatchMonitorData			アラームラッチモニタをデータ作成
		AlmLatchMonDataWrite		アラームラッチモニタ、 EEPROMへ格納します

-----------------------------------------------------------------------------
Changes		:
Name		Date			Description
-----------------------------------------------------------------------------
K.Liu		2013.10.11		created

*****************************************************************************/

#include "AlmLatch.h"
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
#include "Sqi.h"
#endif
#define	MONSETBIT( sig, bit )	((sig)? (1<<bit) : 0x0000)
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
#define LPX_CHECK_ON( psqi, sig )  (((psqi)->Sigs & (sig)))
#endif
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
extern SQIV	SqiV;
#endif
/****************************************************************************************************/
/*																									*/
/*		API関数一覧																					*/
/*																									*/
/****************************************************************************************************/
void AlmLatchMonitorInitial( ALMLATCH_HNDL *hAlmLatchHndl );
void AlmLatchMonitorSequence( ALMLATCH_HNDL *hAlmLatchHndl );

/****************************************************************************************************/
/*																									*/
/*		Local関数一覧																				*/
/*																									*/
/****************************************************************************************************/
static void AlmLatchMonitorData( ALMLATCH_HNDL *hAlmLatchHndl );
//static void AlmLatchMonDataWrite( ALMLTMON *hAlmMonitor, ALMMAN_HANDLE *hAlmHndl );



/*****************************************************************************
  Description: アラームラッチ初期化処理
------------------------------------------------------------------------------
  Parameters:
	hAlmLatchHndl			Alarm Latch handle

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
void AlmLatchMonitorInitial( ALMLATCH_HNDL *hAlmLatchHndl )
{
	MlibResetLongMemory( &(hAlmLatchHndl->var), sizeof( hAlmLatchHndl->var )/4 );
	
	return ;
}



/*****************************************************************************
  Description: アラームラッチ処理
------------------------------------------------------------------------------
  Parameters:
	hAlmLatchHndl			Alarm Latch handle

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
void AlmLatchMonitorSequence( ALMLATCH_HNDL *hAlmLatchHndl )
{

/*--------------------------------------------------------------------------------------------------*/
/*		主回路母線電圧がＵＶレベルより下がったら、データ更新＆書込み要求はしない					*/
/*--------------------------------------------------------------------------------------------------*/
	if ( hAlmLatchHndl->PowerManager->PowerSts.DcVolt >=
			hAlmLatchHndl->PowerManager->MainPowerChecker.conf.UvLevel )
	{
#if 0	/* <S212> */
		/*----------------------------------------------------------------------------------*/
		/*		アラーム未検出の場合、データ更新											*/
		/*----------------------------------------------------------------------------------*/
		if ( hAlmLatchHndl->AlmManager->Status.AlmFlag == FALSE )
		{
			AlmLatchMonitorData( hAlmLatchHndl );

		}
		/*----------------------------------------------------------------------------------*/
		/*		前回アラームなし＆今回アラーム検出で、EEPROM書込み待ち行列への登録			*/
		/*----------------------------------------------------------------------------------*/
		else if ( hAlmLatchHndl->var.LastAlmStatus == FALSE )
		{
#else	/* <S212> */
		if( hAlmLatchHndl->AlmManager->LtAlmReq == 4 )
		{
			hAlmLatchHndl->var.LastAlmStatus    = FALSE;
			hAlmLatchHndl->AlmManager->LtAlmReq = 0;
		}
		if( hAlmLatchHndl->var.LastAlmStatus == FALSE )
		{
			hAlmLatchHndl->var.LastAlmStatus = TRUE;

			AlmLatchMonitorData( hAlmLatchHndl );
#endif	/* <S212> */
			/*--------------------------------------------------------------------------*/
			/*		0x833 : アラーム番号 [－]											*/
			/*		0x834 : タイムスタンプ [－]											*/
			/*--------------------------------------------------------------------------*/
#if 0	/* <S1CF>  ラッチデータを書き込む。ただし、書き込み中は無視する。	*/
			hAlmLatchHndl->AlmLtMon.AlmCode = ALMGetAlarmHistory( hAlmLatchHndl->AlmManager, 0,
										&hAlmLatchHndl->AlmLtMon.TmStamp );
//			AlmLatchMonDataWrite( &hAlmLatchHndl->AlmLtMon, hAlmLatchHndl->AlmManager );	  /* <S194> */

			/* モニタコピー */
			MlibCopyLongMemory( &(hAlmLatchHndl->LatchMon), &(hAlmLatchHndl->AlmLtMon),
								(sizeof(ALMLTMON) >> 2) );
#else
			if( hAlmLatchHndl->AlmManager->LtAlmReq == 0 )
			{
				/* モニタコピー */
				MlibCopyLongMemory( &(hAlmLatchHndl->LatchMon), &(hAlmLatchHndl->AlmLtMon),
																	(sizeof( ALMLTMON ) >> 2) );

				/* 書き込み要求 */
				hAlmLatchHndl->AlmManager->LtRomAdr = EEP_NOSUM_ADDRESS( hAlmLatchHndl->AlmManager->myAsicNo, EEP_ALMMON_OFS );
				hAlmLatchHndl->AlmManager->LtMemAdr = (USHORT*)&( hAlmLatchHndl->LatchMon );
				hAlmLatchHndl->AlmManager->AlmCdAdr = (USHORT*)&( hAlmLatchHndl->LatchMon.AlmCode );
				hAlmLatchHndl->AlmManager->TmStpAdr = (ULONG *)&( hAlmLatchHndl->LatchMon.TmStamp );
				hAlmLatchHndl->AlmManager->LtDtWtCt = sizeof( ALMLTMON ) >> 1;
				hAlmLatchHndl->AlmManager->LtAlmReq = 1;
			}
#endif
		}
		else
		{
			/* Do Nothing */
		}
	}
/*--------------------------------------------------------------------------------------------------*/
/*		アラーム状態保存																			*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S212> */
	if ( hAlmLatchHndl->AlmManager->Status.AlmFlag == TRUE )
	{
		hAlmLatchHndl->var.LastAlmStatus = TRUE;
	}
	else
	{
		hAlmLatchHndl->var.LastAlmStatus = FALSE;
	}
#endif
	return;
}

/*****************************************************************************
  Description: アラームラッチモニタをデータ作成
------------------------------------------------------------------------------
  Parameters:
	hAlmLtUnitHndl		Alarm Latch Unit Handle

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
static void AlmLatchMonitorData( ALMLATCH_HNDL *hAlmLatchHndl )
{
	LONG	lwk;
	LONG	val;
	ALMLTMON	*pAlmLtMon;
	PLSIFV		*pPls;
	
	pAlmLtMon	= &hAlmLatchHndl->AlmLtMon;
	pPls		= hAlmLatchHndl->pPlsIfV;
/*--------------------------------------------------------------------------------------------------*/
/*		0x800 : モータ回転/移動速度 [min-1, mm/s]													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1CF> */
	pAlmLtMon->MotSpd =
			(SHORT)MlibLimitul(hAlmLatchHndl->UnMonitor->UnMotSpdx, 0x00007FFF, 0xFFFF8000);
#else
	pAlmLtMon->MotSpd = hAlmLatchHndl->UnMonitor->UnMotSpd;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		0x801 : 速度指令 [min-1, mm/s]																*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1CF> */
	val = MlibMulgainSx48( hAlmLatchHndl->UnMonitor->UnSpdRef, hAlmLatchHndl->Bprm->Kmonspd );
	pAlmLtMon->SpdRef = (SHORT)MlibLimitul(val, 0x00007FFF, 0xFFFF8000) ;
#else
	val = MlibMulgainSx48( hAlmLatchHndl->BaseControls->CtrlCmdMngr.SpdRefo, hAlmLatchHndl->Bprm->Kmonspd);
	pAlmLtMon->SpdRef = MlibLimitul(val, 0x00007FFF, 0xFFFF8000);
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		0x802 : 内部トルク/推力指令 [%]																*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->TrqRef = (SHORT)( hAlmLatchHndl->UnMonitor->UnTrqRef );

/*--------------------------------------------------------------------------------------------------*/
/*		0x803 : 入力指令パルス速度 [min-1, mm/s]													*/
/*--------------------------------------------------------------------------------------------------*/
#if 0	/* <S1CF> */
	val = MlibMulgain( hAlmLatchHndl->SeqMotSts->var.dPcmdCnt,
							hAlmLatchHndl->Bprm->Kmonpcmdspd2ms );
	pAlmLtMon->PcmdSpd = (SHORT)MlibLimitul(val, 0x00007FFF, 0xFFFF8000) ;
#else
	pAlmLtMon->PcmdSpd = hAlmLatchHndl->UnMonitor->UnPcmd64ms;
#endif

/*--------------------------------------------------------------------------------------------------*/
/*		0x804 : 偏差カウンタ(位置偏差量) [pulse]													*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->PosErr = hAlmLatchHndl->BaseLoops->BaseCtrls->PosManager.var.PosErrA.per32s;

/*--------------------------------------------------------------------------------------------------*/
/*		0x810 : 内部信号モニタ [－]																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->InterSignal = (
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->MainPowerOn,	0	) |			/* 主回路ON状態 */
								MONSETBIT( hAlmLatchHndl->MencV->EncReady,					1	) |	/* Encoder Ready */
								MONSETBIT( hAlmLatchHndl->BeSequence->DbSeqData.DbBeReq,	2	) |	/* モータ通電(要求) */
								MONSETBIT( hAlmLatchHndl->BeSequence->DbOn,					3	) |	/* DB ON */
								MONSETBIT( hAlmLatchHndl->Bprm->RvsDir,						4	) |	/* 逆回転モード */
								MONSETBIT( hAlmLatchHndl->BaseControls->ModeSwData.var.ModeSWFlag,	5	) |	/* モードスイッチON Flag	 */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.Vrefon,				6	) |	/* Speed  Reference Input On */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.Trefon,				7	) |	/* Torque Reference Input On */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.Prefon,				8	) |	/* Torque Reference Input On */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.PrefSign,			9	) |	/* Pulse  Reference Sign  (plus:1, minus:0) */
								MONSETBIT( hAlmLatchHndl->SeqCtrlOut->f.PerrClr,			10	) |	/* PosErr Clear Input On */
								MONSETBIT( 0,							11	) |	/*MONSETBIT( BoutV.f.BiasONFlag,	11	)、0固定 */
								MONSETBIT( hAlmLatchHndl->PowerManager->PowerSts.Acon,		12	) |	/* AC電源検出状態 */
								MONSETBIT( hAlmLatchHndl->PowerManager->PowerSts.ChargeOk,	13	) |	/* DC充電完了状態*/
								MONSETBIT( HALdrv_GetRegeneControlState(),					14	) |	/* 回生Tr指令オン */
								MONSETBIT( hAlmLatchHndl->PowerManager->RegeneCtrl.RegChk.RegeAlmSignal,	15	) |	/* 回生異常検出オン */
								MONSETBIT( hAlmLatchHndl->BeSequence->BeReqSeqOut,			16	) |	/* BaseEnable Request(シーケンス処理出力) */
								MONSETBIT( 0,												17	) |
								MONSETBIT( ALMCheckEachState(hAlmLatchHndl->AlmManager, ALM_OC),			18	) |	/* 過電流 */
								MONSETBIT((hAlmLatchHndl->MencV->RxAlmCode & 0x0040),		19	) |	/* 原点未通過完了 */
								MONSETBIT( hAlmLatchHndl->BaseControls->JatHdl.jatv.enable,	20	) |	/* イナーシャ同定中 */
								MONSETBIT( (hAlmLatchHndl->BeSequence->BeReqSeqOut != FALSE)
										&& (hAlmLatchHndl->MencV->PhaseReady == FALSE),		21	) |	/* 磁極検出中 */
								MONSETBIT( hAlmLatchHndl->MencV->PhaseReady,				22	) 	/* 磁極認識完了 */
								);

/*--------------------------------------------------------------------------------------------------*/
/*		0x814 : 内部入力信号モニタ [－]																*/
/*--------------------------------------------------------------------------------------------------*/
	KPX_READ_PCMD_STATUS( lwk );
	pAlmLtMon->InSignal = (
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & SERVO_ON_BIT)),	0	) |	/* サーボオン */
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & ENBPCTRL_BIT)),	1	) |	/* P/PI切り替え */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->PotSigSts,							2	) |	/* 正転駆動禁止状態 */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->NotSigSts,							3	) |	/* 逆転駆動禁止状態 */
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmRst,								4	) |	/* ALMリセット要求 */
							MONSETBIT( hAlmLatchHndl->BaseControls->TrqLimitData.var.PclSignal ,		5	) |	/* PCL Signal */
							MONSETBIT( hAlmLatchHndl->BaseControls->TrqLimitData.var.NclSignal,			6	) |	/* PCL Signal */
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & SENSOR_ON_BIT)),	7	) |	/* センサオン	*/
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							/* 内部設定速度制御 */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_SPDD ),		8	) |	/* SPD-D Signal */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_SPDA ),		9	) |	/* SPD-A Signal */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_SPDB ),		10	) |	/* SPD-B Signal */
							/* 制御方式切り替え */
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_CSEL ),		11	) |	/* C-SEL Signal */
#else
							MONSETBIT( 0,		8	) |
							MONSETBIT( 0,		9	) |
							MONSETBIT( 0,		10	) |
							MONSETBIT( 0,		11	) |
#endif
							MONSETBIT( hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & ZEROCLAMP_BIT,	12	) |	/* Zero Clamp */
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_INHIBIT ),					13	) |	/* INHIBIT Signal */
#else
							MONSETBIT( 0,		13	) |
#endif
							MONSETBIT( hAlmLatchHndl->BaseControls->GselNum & 0x03,				14	) |	/* マニュアルゲイン切替用ゲイン番号(bit14,15) */
							MONSETBIT( (0 != (hAlmLatchHndl->BaseCtrlOut->CmdSeqBit & POL_DETECT_BIT)),		16	) |	/* P-DET Signal */
							MONSETBIT( hAlmLatchHndl->IoSignals->EstpSignal.SignalStatus.v,		17	) |	/* E-Stop Signal *//* <S045> */
							MONSETBIT( hAlmLatchHndl->IoSignals->DecSignal.SignalStatus.v,		18	) |	/* E-Stop Signal *//* <S045> */
							/* 外部ラッチ内部入力状態(Ext1,Ext2,Ext3) */
							MONSETBIT( hAlmLatchHndl->IoSignals->ExtSignal[0].SignalStatus.v,	19	) | /* <S045> */
							MONSETBIT( hAlmLatchHndl->IoSignals->ExtSignal[1].SignalStatus.v,	20	) | /* <S045> */
							MONSETBIT( hAlmLatchHndl->IoSignals->ExtSignal[2].SignalStatus.v,	21	) | /* <S045> */

							MONSETBIT( KPX_CHECK_PCMD_CADAT( lwk ),	24	) |
							MONSETBIT( KPX_CHECK_PCMD_CBDAT( lwk ),	25	) |
							MONSETBIT( hAlmLatchHndl->BaseControls->CtrlCmdMngr.PerrClrReq,		26	) |	/* 位置偏差クリア要求(テーブル運転/PJOG専用) */
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_PSEL ),					27	)	/* 指令パルス入力切替(/PSEL) */
#else
							MONSETBIT( 0,		27	)
#endif
							);

/*--------------------------------------------------------------------------------------------------*/
/*		0x818 : 内部出力信号モニタ [－]																*/
/*--------------------------------------------------------------------------------------------------*/
	val = 0;
	if(hAlmLatchHndl->Bprm->FencUse != FALSE)
	{
		val = hAlmLatchHndl->FencV->CposOk & (hAlmLatchHndl->FencV->MposFromCpos == 0);
	}
	else
	{
		val = hAlmLatchHndl->MencV->CposOk & (hAlmLatchHndl->MencV->MposFromCpos == 0);
	}
	
	pAlmLtMon->OutSignal = (
							MONSETBIT( hAlmLatchHndl->AlmManager->Status.AlmFlag,			0	) |	/* アラーム発生中 */
							MONSETBIT( hAlmLatchHndl->BaseControls->PosCtrlSts.CoinSignal,	1	) |	/* 位置決め完了 */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->VcmpSts,					2	) |	/* 速度一致 */
							MONSETBIT( hAlmLatchHndl->SeqCtrlOut->TgOnSts,					3	) |	/* モータ回転中 */
							MONSETBIT( hAlmLatchHndl->BeSequence->ServoReady,				4	) |/* サーボレディ */
							/* トルク制限中 */
							MONSETBIT( ((hAlmLatchHndl->SvControlIf->RspCtrl.RspCtrlBit >> TRQ_CLAMP_BITNO) & 1) ,	5	) |
							/* 速度制限中 */
							MONSETBIT( ((hAlmLatchHndl->SvControlIf->RspCtrl.RspCtrlBit >> SPD_CLAMP_BITNO) & 1),	6	) |
							MONSETBIT( (!(hAlmLatchHndl->SeqCtrlOut->BrkSigSts)),			7	) |	/* ブレーキ解除中 */
							MONSETBIT( hAlmLatchHndl->AlmManager->Status.WrnFlag,			8	) |	/* ワーニング発生中 */
							MONSETBIT( hAlmLatchHndl->BaseControls->PosCtrlSts.NearSignal,	9	) |	/* Near Signal */
							MONSETBIT( val,													10	) |	/* PgCphase Signal */
							/* アラームコード信号出力	*/
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmCodeOut.Alo1,			16	) |
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmCodeOut.Alo2,			17	) |
							MONSETBIT( hAlmLatchHndl->AlmManager->AlmCodeOut.Alo3,			18	) |
						/*	((BinK.DivPulseSig & 0x07)			 << 19  ) |		*//*分周パルス出力未対応*/
#if ( CSW_NETIF_TYPE == NETIF_ANLGPLS_TYPE )
							MONSETBIT( LPX_CHECK_ON( &SqiV, SQI_SIGS_PSEL ),				22	)	/* 指令パルス入力切替応答(/PSELA ) */
#else
							MONSETBIT( 0 /*BoutV.f.Sv.PselA */,			22	)
#endif
							);

/*--------------------------------------------------------------------------------------------------*/
/*		0x820 : 主回路母線電圧 [V]																	*/
/*--------------------------------------------------------------------------------------------------*/
#if ( FLOAT_USE == TRUE )
	pAlmLtMon->DcVoltage = hAlmLatchHndl->PowerManager->PowerSts.DcVolt *
							hAlmLatchHndl->PowerManager->DetectDcVolt.conf.Kdcvolt;
#else
	pAlmLtMon->DcVoltage = MlibMulgain(hAlmLatchHndl->PowerManager->PowerSts.DcVolt,
								hAlmLatchHndl->PowerManager->DetectDcVolt.conf.Kdcvolt );
#endif /* FLOAT_USE */
	
/*--------------------------------------------------------------------------------------------------*/
/*		0x821 : 電流検出値 [%]																		*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	pAlmLtMon->IFBMon = hAlmLatchHndl->UnMonitor->UnCrntDet *
						hAlmLatchHndl->Bprm->Kmoncrnt;
#else
	pAlmLtMon->IFBMon =
			MlibMulgain( hAlmLatchHndl->UnMonitor->UnCrntDet, hAlmLatchHndl->Bprm->Kmoncrnt );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		0x822 : 累積負荷率 [%(周期2ms)]																*/
/*--------------------------------------------------------------------------------------------------*/
#if (FLOAT_USE==TRUE)
	pAlmLtMon->MotLoad = hAlmLatchHndl->UnMonitor->UnCrntRef *
						hAlmLatchHndl->Bprm->Kmoncrnt;
#else
	pAlmLtMon->MotLoad =
			MlibMulgain( hAlmLatchHndl->UnMonitor->UnCrntRef, hAlmLatchHndl->Bprm->Kmoncrnt );
#endif /* FLOAT_USE */

/*--------------------------------------------------------------------------------------------------*/
/*		0x823 : 回生負荷率 [10%(周期2ms)]															*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->RgPower = hAlmLatchHndl->UnMonitor->UnRgPower;

/*--------------------------------------------------------------------------------------------------*/
/*		0x824 : ＤＢ抵抗消費電力 [%(周期2ms)]														*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->DbPower = hAlmLatchHndl->UnMonitor->UnDbPower;

/*--------------------------------------------------------------------------------------------------*/
/*		0x825 : 最大累積負荷率 [%]																	*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->MaxMotLoad10s = (USHORT)hAlmLatchHndl->UnMonitor->UnIsqrSumMax ;

/*--------------------------------------------------------------------------------------------------*/
/*		0x826 : 慣性モーメント比/重量比 [%]															*/
/*--------------------------------------------------------------------------------------------------*/
	/*	制御で使用している慣性比を保存	*/
//	pAlmLtMon->JLRatio = hAlmLatchHndl->BaseControls->JatHdl.EstimatJrat;
	if( hAlmLatchHndl->BaseControls->JatHdl.jatv.enable == TRUE )		/* イナーシャ同定有効		*/
	{
		pAlmLtMon->JLRatio = hAlmLatchHndl->BaseControls->JatHdl.EstimatJrat;
	}
	else
	{
		pAlmLtMon->JLRatio = hAlmLatchHndl->UniPrm->Prm->jrate;
	}

/*--------------------------------------------------------------------------------------------------*/
/*		0x830 : シリアルエンコーダ通信異常回数 [－]													*/
/*--------------------------------------------------------------------------------------------------*/
	pAlmLtMon->EncComErrCnt = hAlmLatchHndl->MencV->ComErrCnt;

/*--------------------------------------------------------------------------------------------------*/
/*		0x832 : セーフティ入出力信号モニタ [－]														*/
/*--------------------------------------------------------------------------------------------------*/
	/* セーフティオプションカード無しの時 */
	if( hAlmLatchHndl->SftOpt->conf.OpInfo.f.Connect == TRUE )
	{
		/* セーフティオプションカード有りの時 */
		pAlmLtMon->SafetyIOSignal = (
									MONSETBIT( FALSE,					0	) |
									MONSETBIT( FALSE,					1	) |
									MONSETBIT( FALSE,					2	) |
									MONSETBIT( FALSE,					3	) |
									MONSETBIT( FALSE,					4	) |
									MONSETBIT( FALSE,					5	) );
	}
	else
	{
		/* セーフティオプションカード無しの時 */
		pAlmLtMon->SafetyIOSignal = (
									MONSETBIT( hAlmLatchHndl->IoSignals->DetHwbbSts.Bbon1,	0	) |
									MONSETBIT( hAlmLatchHndl->IoSignals->DetHwbbSts.Bbon2,	1	) |
									MONSETBIT( FALSE,					2	) |
									MONSETBIT( FALSE,					3	) |
									MONSETBIT( FALSE,					4	) |
									MONSETBIT( FALSE,					5	) );
	}

	return;
}

#if 0
/*****************************************************************************
  Description: アラームラッチモニタ、 EEPROMへ格納します
------------------------------------------------------------------------------
  Parameters:
	hAlmMonitor			アラームラッチ用モニタのポインタ
	hAlmHndl			アラーム用データのポインタ

  Return:
	none
------------------------------------------------------------------------------
  Note:
*****************************************************************************/
static void AlmLatchMonDataWrite( ALMLTMON *hAlmMonitor, ALMMAN_HANDLE *hAlmHndl )
{
	ULONG *pMonitorVal;
	ULONG MonitorVal;
	ULONG Offset;
	USHORT MonitorSize;		/* アラームラッチモニタのサイズ */
	USHORT i;

	pMonitorVal = (ULONG *)(hAlmMonitor);
	/* アラームラッチモニタのサイズ値を計算する */
	MonitorSize = EEPSIZE_ALMMON >> 2 ;

	for(i = 0U; i < MonitorSize; i++)
	{
		/* Write Alarm Latch Monitor to EEPROM */
		Offset = EEP_NOSUM_ADDRESS( hAlmHndl->myAsicNo, EEP_ALMMON_OFS ) + (i << 2);
		MonitorVal = *(pMonitorVal + i);	/* アラームラッチモニタ値を取得する */
		EepdevPutQueue(&hAlmHndl->EepQue, 0, Offset, MonitorVal, 4U );
	}

	return ;
}
#endif



/***************************************** end of file **********************************************/
