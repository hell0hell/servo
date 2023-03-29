/****************************************************************************
Description	: Round Task Main module
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	SysRoundTask()			:Main routine of the Round Task

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.01.24     renewal

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "NetworkIf.h"
#include "AlarmManager.h"
#include "HwDeviceIf.h"
#include "KLib.h"
#include "EepromIF.h"
#include "FunManager.h"
#include "OverheatManager.h"
#include "DetectTemperature.h"	/* <S03A> */

/* --> <USB-FUNCTION> 2013.05.13 K.Sakamoto */
//#ifdef _USB_FUNC_USE_		/*<S008>*//*<S138>*/
//<S146>#include "nbpfahb32vic128.h"
//<S146>#include "usb_f.h"
//#endif	//#ifdef _USB_FUNC_USE_		/*<S008>*//*<S138>*/
/* <-- <USB-FUNCTION> 2013.05.13 K.Sakamoto */

static void RpiRoundMain( AXIS_HANDLE *AxisRscR, int *iniFlg );
static BOOL LpxSoftReset( AXIS_HANDLE *AxisRscR );		/* <S076> */

/* 外部関数 */
extern LONG CpiGetScanCCnt( void );						
extern void KpxMsgManager( void );						
extern void ScuSoftReset( AXIS_HANDLE *AxisHandle );	
extern void UsbFuncTaskControl( void );					

#if 0/* 保留 */
extern BOOL FTP_FirmwareUpdateMain(void);
#endif
extern void	MbusFuncSystemDownLoadCheckSum( void );	/* 暗号化フォーマット対応 <S146> */

/****************************************************************************
  Description: Main routine of the Round Task
-----------------------------------------------------------------------------
  Parameters	: none
  Return		: none
*****************************************************************************/
void SysRoundTask( void )
{
	AXIS_HANDLE	*AxisRscR;
	int iniFlg = FALSE;
	/*----------------------------------------------------------------------*/
	LONG SCCnt = 0;												/* @@@ */
	LONG wkx;													/* @@@ */
	ULONG wtimer;												/* <S03F> */
	ULONG	ax;													/* <S19B> */
	/*----------------------------------------------------------------------*/

	AxisRscR = (AXIS_HANDLE*)GetAxisHandle(0);

	/********************/
	/*		Loop 		*/
	/********************/
#ifndef _VC_DEBUG_ /* 2013.01.10 Y.Oka */
	for( ; ; )
#endif	//#ifndef _VC_DEBUG_ /* 2013.01.10 Y.Oka */
	{
		/* 処理時間測定用GPIO ON */
//		HALdrv_GpioOutput( TRUE 0 );

		/* ROUND */
	/*----------------------------------------------------------------------*/
		KlibRstLongTimer( &wtimer );	/* ScanCが停止した時でもRound処理が動くように修正<S03F> */
		while( 1 )
		{ /* @@ wait for ScanC */
			if( SCCnt != (wkx = CpiGetScanCCnt()) )
			{
				SCCnt = wkx; 						/* @@ get scanC cnt		*/
				break;
			}
			else if( 2 < KlibGetLongTimerMs(wtimer) )
			{
				break;
			}
			else
			{
				; //Wait
			}
		}
	/*----------------------------------------------------------------------*/

		/* Round Loop Main Operation */
		RpiRoundMain( AxisRscR, &iniFlg );

		/* 処理時間測定用GPIO OFF */
//		HALdrv_GpioOutput( FALSE, 0 );

#ifndef _VC_DEBUG_ /* 2013.05.31 Y.Oka */
		KpxMsgManager( ); 			/* Message Manager <XXX> */

		/* 軸数分ループ *//* <S19B> */
		for( ax = 0; ax < NumberOfAxes; ax++ )
		{
			AxisRscR = (AXIS_HANDLE*)GetAxisHandle(ax);
			KpxDownloadEeprom( AxisRscR->pRwAllMng );		/* <S00C> Download Eeprom  */
			/* <S031> Lcd Parameter Copy *//* <S098> */
			if( I2C_DRV_SUCCESS != KpxLcdPdlEeprom(AxisRscR->pRwPdlMng) )
			{ /* A.BF6 : システムアラーム６(I2Cドライバ異常) */
				ALMSetServoAlarm( AxisRscR->AlmManager, ALM_I2C_DRVERR );
			}
		}
		/* 0軸へ戻す */
		AxisRscR = (AXIS_HANDLE*)GetAxisHandle(0);/* <S19B> */
#endif	//#ifndef _VC_DEBUG_ /* 2013.05.31 Y.Oka */

		/* スタック領域チェック *//*<S127>*/
		if( 0xFFFFFFFF != *(VULONG*)(0x67FAF61C) )
		{/* A.BF1 : システムアラーム１(内部プログラム動作異常)			*/
			ALMSetGlobalAlarm( ALM_STACK_OVF );
		}

		MbusFuncSystemDownLoadCheckSum();	/* 暗号化フォーマット対応 <S146> */

		/* ソフトリセット <S076> */
		AxisRscR->BeSequence->SoftResetReq = LpxSoftReset( AxisRscR );

		KlibWaitus(500);	/* 0.5ms */

		/* <S0CB> Start */
			/* スキャンCカウント取得 : Round処理が必ずスキャンCの後に実行されるようにする。 */
			SCCnt = CpiGetScanCCnt();
		/* <S0CB> End */

	}
}

/****************************************************************************
  Description: Round Main Procedure
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscR	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static void RpiRoundMain( AXIS_HANDLE *AxisRscR, int *iniFlg )
{
	LONG			ax_no;
	LONG			timer;
	ULONG			timeStamp, rdTime;
	AXIS_HANDLE		*AxisRsc;
	ANOTCHSEQCTRL	ANotchCtrl;
	ALMMAN_HANDLE	*AlmManager;

#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE
	/* ネットワークモジュールRound処理 */
	if(FALSE == OpiRoundProcedure( AxisRscR ))
	{
#if 0/* 保留 */
		/* write downloaded firmware via FTP */
		FTP_FirmwareUpdateMain();
#endif
		PnlOpSetConnectStatus( AxisRscR->PnlOpLedDataHndl, FALSE );			  /* <S0A1> */
	}
	else
	{
		PnlOpSetConnectStatus( AxisRscR->PnlOpLedDataHndl, TRUE );			  /* <S0A1> */
	}
#endif

/* --> <USB-FUNCTION> 2013.05.18 K.Sakamoto */
//#ifdef _VC_DEBUG_
/* <-- <USB-FUNCTION> 2013.05.18 K.Sakamoto */
	/* ネットワークメッセージ通信管理	*/
//<S150>	MsgIfBackgroundProdedure(AxisRscR->NetMsgIf);
/* <S150> >>>>> */
#if (CSW_NETIF_TYPE == NETIF_M3_TYPE)
	M3MsgIfManager();
#endif
/* <<<<< <S150> */
/* --> <USB-FUNCTION> 2013.05.18 K.Sakamoto */
//#endif
/* <-- <USB-FUNCTION> 2013.05.18 K.Sakamoto */


/* --> <USB-FUNCTION> 2013.05.13 K.Sakamoto */
//#ifdef _USB_FUNC_USE_/*<S138>*/
	UsbFuncTaskControl();
//#endif	//#ifdef _USB_FUNC_USE_		/*<S008>*//*<S138>*/
	//	u2f_task_control();
//	u2f_task_msc();
/* <-- <USB-FUNCTION> 2013.05.13 K.Sakamoto */

	/* ROUND Each Axis Operation */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRsc = &AxisRscR[ax_no];
		/* Fn機能実行処理 */
		FunRunTimeService(AxisRsc->FnManager, FUN_CH1);
		FunRunTimeService(AxisRsc->FnManager, FUN_CH2);
//		if ( !(AxisRsc->BeSequence->ServoReady) ){ AxisRsc->FnCmnCtrl->FnSvonReq = FALSE; } /* <S0B4> */
		if( AxisRsc->MencV->PhaseReady == TRUE )		/* <S21C> 位相検出完了 */
		{
			if ( !(AxisRsc->BeSequence->ServoReady) ){ AxisRsc->FnCmnCtrl->FnSvonReq = FALSE; } /* <S0B4> */
		}												/* <S21C> */
		
		/* 自動ノッチフィルタ設定シーケンスコール用ポインタ設定 */
		ANotchCtrl.ANotchSeq = AxisRsc->ANotchSeq;
		ANotchCtrl.ANotch = AxisRsc->ANotch;
		ANotchCtrl.FftAna = AxisRsc->FftAna;
		ANotchCtrl.DetVib = AxisRsc->DetVib;
		ANotchCtrl.Prm = AxisRsc->UniPrms->Prm;
		ANotchCtrl.RegCfg = AxisRsc->RegManager->hPnReg;
		/* 自動ノッチフィルタ設定用データ設定処理 */
		AutoNotchSetCondition( ANotchCtrl.ANotchSeq,
							   AxisRsc->BaseControls->TuneLessCtrl.var.TuningFuncRun,
							   AxisRsc->SeqCtrlOut->BaseEnableReq,
							   AxisRsc->BaseControls->TuneLessCtrl.var.TuneLessAct,
							   AxisRsc->SeqCtrlOut->OverTrvlSts );
		/* 自動ノッチフィルタ設定シーケンス実行処理 */
		AutoNotchFilterSetting( &ANotchCtrl );

		/* ABSスケール磁極検出結果書き込みシーケンス処理 */
		if(FALSE != PdetWriteMotorPhaseSequence(AxisRsc->PdetAbsSeq,
												AxisRsc->MencV,
												AxisRsc->SvAsicRegs->AsicHwReg,
												AxisRsc->RegManager->hPnReg))
		{
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_PGCOM );	/* A.C90：エンコーダ通信異常 */
		}
		/* DBリレー動作回数用パラメータ書き込みTO　Eeprom */
		LifeTimeDBPrmStoreValueToEeprom( AxisRsc->hDBLifeTime,
										AxisRsc->RegManager->hPnReg,
										AxisRsc->UniPrms->Prm );			/* <S192> */
	}

	/* 突防リレー動作回数用パラメータ書き込みTO　Eeprom */
	LifeTimeRushPrmStoreValueToEeprom( &AxisRscR->hLifeTime->RushLifeTimeHdl,
										AxisRscR->RegManager->hPnReg,
										AxisRscR->UniPrms->Cprm );			/* <S192> */

	/* Alarm Manager : EEPROM履歴書き込み */
	timer = KlibGetMainPowerOffTimer();
	if((timer <= 0) || (timer >= KLIB_EEPDISABLE_TIME))
	{/* Main Power offから100ms間は書き込みしない */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			AxisRsc = &AxisRscR[ax_no];
			ALMHistoryWriteService(AxisRsc->AlmManager);
		}
	}
	else
	{
		AlmManager = AxisRscR->AlmManager;
		/* Main Power offでTime Stamp書き込み */
		rdTime = timeStamp = KlibGetTimeStamp();
		EepdevReadValues( 0,
						  //EEP_NOSUM_ADDRESS(0, EEP_TIMESTAMP_OFS),					/* <S04E>:DELL */
						  EEP_NOSUMCMN_ADDRESS(EEP_TIMESTAMP_OFS),                      /* <S04E>:ADD */
						  (USHORT*)&rdTime, 2 );
		if(timeStamp != rdTime)
		{
			EepdevWriteNoSumValues( 0,
									//EEP_NOSUM_ADDRESS(0, EEP_TIMESTAMP_OFS),			/* <S04E>:DELL */
									EEP_NOSUMCMN_ADDRESS(EEP_TIMESTAMP_OFS),            /* <S04E>:ADD */
									(USHORT*)&timeStamp,
									2 );
		}
	}

#if	(CSW_DETECT_TEMPERATURE == TRUE) /* <S03A> */
	/* サーボパック温度IC管理 */
	DetTempManager( AxisRscR->TempManager );
#endif

/* 2013/04/01MD版では、RndReadyはない。暫定的にSEQ_CTRL_OUTに定義する	*/
	if(*iniFlg == FALSE)
	{/* ラウンドレディ設定処理 */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			AxisRsc = &AxisRscR[ax_no];
			AxisRsc->SeqCtrlOut->RndReady = TRUE;
		}
		*iniFlg = TRUE;
	}
}


/****************************************************************************
  Description: Round SoftReset Procedure		<S076>
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscR	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static BOOL LpxSoftReset( AXIS_HANDLE *AxisRscR )
{
	BOOL	resetReq;

	resetReq = AxisRscR->BeSequence->SoftResetReq;
	
	if( TRUE == resetReq )
	{/* 「サーボオンでない」＆「モータ回転中でない」ときのみリセット可*/
		ScuSoftReset( AxisRscR );
		resetReq = FALSE;		/* */
	}

	return( resetReq );
}



/**************************** end of file **********************************/
