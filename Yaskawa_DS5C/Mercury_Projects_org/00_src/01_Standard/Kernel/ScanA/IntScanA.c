/****************************************************************************
Description	: Baseloop Scan Interrupt(ScanA) Main module
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : Mercury

Functions	  :
	SysScanAInt()			:Main routine of the Baseloop Scan Interrupt

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.01.24     renewal
Y.Oka  2012.08.27     renewal

****************************************************************************/
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "BaseControlMain.h"
#include "MicroScanAIf.h"
#include "AlarmManager.h"
#include "KLib.h"
#include "MLib.h"
#include "HwDeviceIf.h"
#include "DMAdrv.h"
#include "SerialEncIf.h" //

LONG TEST_SCANA_CNT;

/* Prototype */
void SysScanAInt( void );

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
/* 暫定処置 */
void IntScanA( void );
extern void	SetBaseCtrlInterrupt( void );
extern void	SetEnableInterrupt( void );

/****************************************************************************
  Description: Interrupt Operation for Base Scan Task
-----------------------------------------------------------------------------
  Parameters	: none
  Return		: none
*****************************************************************************/
void IntScanA( void )
{
	HALdrv_OutputDebugSP( INTERRUPT_ScanA );
	KPI_EI();					//@@	SetEnableInterrupt( );

//	if( ScanAStartCtr < 0x0F )
//	{
//		ScanAStartCtr++;
//	}
//	else
//	{
		SysScanAInt( );
//	}

	/*------------------------------------------------------------------*/
	/*		ScanB起動													*/
	/*------------------------------------------------------------------*/
	if( GetInterruptTiming(INTERRUPT_ScanB) )
	{
		/* Set Base Control Interrupt Request */
		SetBaseCtrlInterrupt( );
	}

	HALdrv_ClearDebugSP( INTERRUPT_ScanA );

}
#endif




/****************************************************************************
  Description: Main routine of the Base Scan Task
-----------------------------------------------------------------------------
  Parameters	: none
  Return		: none
*****************************************************************************/
void SysScanAInt( void )
{
	LONG		ax_no;
	AXIS_HANDLE	*AxisRscA;
	VULONG		ScanAStartTime;
	TASK_MEASURE_TIME	*TaskProcessTime;		/* <S043> */
//	VULONG		ScanATime;		/* グローバル変数で計算する<S043> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	BOOL		Interrupt;
#endif
	MENCV		*MencV;		/* <S14B> */
	RPLCMP		*RippleCmp;	/* <S14B> */


//	HALdrv_OutputDebugSP( INTERRUPT_ScanA );	/* 割込みの分岐処理でポート出力に変更<S042> */
	KlibRstLongTimer((ULONG*)&ScanAStartTime);	/*<S0C7>*/
//	TEST_SCANA_CNT++;

	/* ローカル変数初期化 */
	AxisRscA = GetAxisHandle(0);
	TaskProcessTime = AxisRscA->TaskProcessTime;		/* <S043> */

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
#if 0/* ＜S052>：A.B6A対策暫定処理 */
	/*------------------------------------------------------------------*/
	/*	Network Synchronization					 						*/
	/*------------------------------------------------------------------*/
	Interrupt = HALdrv_GetIntUDLStatus( INTERRUPT_IntSync,  AxisRscA->SvAsicRegs->AsicHwReg );
	OpiIntSync( AxisRscA, Interrupt );
	/* Clear the interrupt. */
	HALdrv_ReqIntUDLStatusClear( INTERRUPT_IntSync );
#endif/* A.B6A対策暫定処理 */
#else
	KpxPlsScanAService( AxisRscA->pPlsIfV );	/* command pulse interface ScanA */
#if( SVFSEL_ADC_USE==1 )    /* <S19A> */
	KpxAdcScanAService( AxisRscA->pAdc  );		/* ADC interface ScanA */
#endif
	AxisRscA->BaseLoops->TRefAd =
			NpiInputTrqRefAd( AxisRscA->pAdc->Var[ADC1_TREFAD] );		/* <S051> */
#endif /* (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE) */



	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* Get Current Loop Status from MicroProgram */
		MicroIfInputCycData( AxisRscA[ax_no].BaseLoops,
							 AxisRscA[ax_no].CtrlLoopOut,
							 AxisRscA[ax_no].SvAsicRegs );
	}

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		KpxInputScanA( AxisRscA[ax_no].BaseLoops );/*<S05C>*/
	}
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* ＜S052>：A.B6A対策暫定処理 */
	/*------------------------------------------------------------------*/
	/*	Network Synchronization					 						*/
	/*------------------------------------------------------------------*/
	Interrupt = HALdrv_GetIntUDLStatus( INTERRUPT_IntSync,  AxisRscA->SvAsicRegs->AsicHwReg );
	OpiIntSync( AxisRscA, Interrupt );
	/* Clear the interrupt. */
	HALdrv_ReqIntUDLStatusClear( INTERRUPT_IntSync );
#endif/* A.B6A対策暫定処理 */

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* Get Base Control Output */
//		SysInputScanAProcessData( AxisRscA[ax_no].BaseLoops,			/* <S050> */
//								  AxisRscA[ax_no].BaseControls,
//								  AxisRscA[ax_no].BaseCtrlOut );
		SysInputScanAProcessData( AxisRscA[ax_no].BaseLoops,
								  AxisRscA[ax_no].BaseControls,
								  AxisRscA[ax_no].BaseCtrlOut,
								  &(AxisRscA[ax_no].SvControlIf->CmdCtrl) );

		/* <S14B> Start */
		/* 速度リップル補償 磁極検出完了確認 */
		MencV = AxisRscA[ax_no].MencV;
		RippleCmp = &(AxisRscA[ax_no].BaseLoops->RippleCmp);

		if(MencV->PhaseReady == TRUE)
		{
			RippleCmp->VelRipplePhaseReady = TRUE;	/* 速度リップル補償 磁極検出完了フラグ = Ture : 速度リップル補償実行可能 */
		}
		else
		{
			RippleCmp->VelRipplePhaseReady = FALSE;	/* 速度リップル補償 磁極検出完了フラグ = False : 速度リップル補償実行不可 */
		}
		/* <S14B> End */

		/* ScanA Main Operation */
		ApxYecServoControl( AxisRscA[ax_no].BaseLoops, AxisRscA[ax_no].CtrlLoopOut ); /* <S0B87> */
	}

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* Make ScanA Output Data */
		KpxOutputScanA( AxisRscA[ax_no].BaseLoops,
						AxisRscA[ax_no].CtrlLoopOut,
						AxisRscA[ax_no].SvAsicRegs,
						AxisRscA[ax_no].UniPrms->Prm );

		SysOutputScanAProcessData( AxisRscA[ax_no].BaseLoops,
								   AxisRscA[ax_no].CtrlLoopOut );
	}

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* Output ScanA Data for Micro Program */
		MicroIfOutputCycData( AxisRscA[ax_no].BaseLoops,
							  AxisRscA[ax_no].CtrlLoopOut,
							  AxisRscA[ax_no].SvAsicRegs );
	}

//	ScanATime = KlibGetLongTimerUs(ScanAStartTime);	/*<S00E>*//*<S043>*/
//	HALdrv_ClearDebugSP( INTERRUPT_ScanA );	/* 割込みの分岐処理でポート出力に変更<S042> */


/* <S1A9> Start */
/* Σ7Wでも実行可能に修正する為、コメントアウト */
#if 0
/* <S19F> Start */
		if(AxisRscA->PowerManager->MainPowerChecker.conf.TestMode9 == TRUE)		/* 自動試験機:ゲートドライブ信号ON/OFF */
		{
			if( AxisRscA->PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag == TRUE )	/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA) == TRUE (ゲートドライブ信号ON) */
			{
				SHalDrv_ChargePumpON( AxisRscA->SvAsicRegs->AsicHwReg);				/* N側チャージポンプ ON */
				SHalDrv_ChargePumpON_P( AxisRscA->SvAsicRegs->AsicHwReg);			/* P側チャージポンプ ON */

				/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA) = FALSE	*/
				/* (次回ScanAでゲートドライブ信号をOFFする為)						*/
				AxisRscA->PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = FALSE;
			}
			else
			{
				/* ゲートドライブ信号 OFF */
				SHalDrv_ChargePumpOFF(AxisRscA->SvAsicRegs->AsicHwReg);

				/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA) = TRUE	*/
				/* (次回ScanAでゲートドライブ信号をONする為)					*/
				AxisRscA->PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = TRUE;
			}
		}
/* <S19F> End */
#endif
/* Σ7W対応処理ここから */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		if(AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.TestMode9 == TRUE)		/* 自動試験機:ゲートドライブ信号ON/OFF */
		{
			if( AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag == TRUE )	/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA) == TRUE (ゲートドライブ信号ON) */
			{
				SHalDrv_ChargePumpON( AxisRscA[ax_no].SvAsicRegs->AsicHwReg);			/* N側チャージポンプ ON */
				SHalDrv_ChargePumpON_P( AxisRscA[ax_no].SvAsicRegs->AsicHwReg);			/* P側チャージポンプ ON */

				/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA) = FALSE	*/
				/* (次回ScanAでゲートドライブ信号をOFFする為)						*/
				AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = FALSE;
			}
			else
			{
				/* ゲートドライブ信号 OFF */
				SHalDrv_ChargePumpOFF(AxisRscA[ax_no].SvAsicRegs->AsicHwReg);

				/* 自動試験機モード：ゲートドライブ信号状態フラグ(ScanA) = TRUE	*/
				/* (次回ScanAでゲートドライブ信号をONする為)					*/
				AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = TRUE;
			}
		}
	}
/* <S1A9> End */

	/*------------------------------------------------------------------*/
	/*	Change Interrupt Cycle						 			<S1D3>	*/
	/*------------------------------------------------------------------*/
	SencSetIntCycleP0Mode( AxisRscA->MencV );

	/* ScanA実行回数カウント <S043> */
	TaskProcessTime->SACount++;

	/*----------------------------------------------------------------------*/
	/*		ScanA処理時間モニタ用処理				<S043>					*/
	/*----------------------------------------------------------------------*/
	TaskProcessTime->SA_crtTime = KlibGetLongCounter(ScanAStartTime);
	TaskProcessTime->SA_sumTime += TaskProcessTime->SA_crtTime;
	if( TaskProcessTime->SA_crtTime > TaskProcessTime->SA_maxTime)
	{/* 処理時間最大値更新 */
		TaskProcessTime->SA_maxTime = TaskProcessTime->SA_crtTime;
	}

	if( TaskProcessTime->SB_exeflg == TRUE )
	{/* ScanB実行中のScanA処理時間を保持しておく */
		TaskProcessTime->SAinB_exeTime += TaskProcessTime->SA_crtTime;
	}
	else
	{
		TaskProcessTime->SAinB_exeTime = 0;
	}
	if( TaskProcessTime->SC_exeflg == TRUE )
	{/* ScanC実行中のScanA処理時間を保持しておく */
		TaskProcessTime->SAinC_exeTime += TaskProcessTime->SA_crtTime;
	}
	else
	{
		TaskProcessTime->SAinC_exeTime = 0;
	}

}



/**************************** end of file **********************************/
