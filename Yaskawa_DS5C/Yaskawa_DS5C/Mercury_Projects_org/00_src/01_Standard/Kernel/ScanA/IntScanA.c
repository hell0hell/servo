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
/* �b�菈�u */
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
	/*		ScanB�N��													*/
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
//	VULONG		ScanATime;		/* �O���[�o���ϐ��Ōv�Z����<S043> */
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	BOOL		Interrupt;
#endif
	MENCV		*MencV;		/* <S14B> */
	RPLCMP		*RippleCmp;	/* <S14B> */


//	HALdrv_OutputDebugSP( INTERRUPT_ScanA );	/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */
	KlibRstLongTimer((ULONG*)&ScanAStartTime);	/*<S0C7>*/
//	TEST_SCANA_CNT++;

	/* ���[�J���ϐ������� */
	AxisRscA = GetAxisHandle(0);
	TaskProcessTime = AxisRscA->TaskProcessTime;		/* <S043> */

#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
#if 0/* ��S052>�FA.B6A�΍�b�菈�� */
	/*------------------------------------------------------------------*/
	/*	Network Synchronization					 						*/
	/*------------------------------------------------------------------*/
	Interrupt = HALdrv_GetIntUDLStatus( INTERRUPT_IntSync,  AxisRscA->SvAsicRegs->AsicHwReg );
	OpiIntSync( AxisRscA, Interrupt );
	/* Clear the interrupt. */
	HALdrv_ReqIntUDLStatusClear( INTERRUPT_IntSync );
#endif/* A.B6A�΍�b�菈�� */
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
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)/* ��S052>�FA.B6A�΍�b�菈�� */
	/*------------------------------------------------------------------*/
	/*	Network Synchronization					 						*/
	/*------------------------------------------------------------------*/
	Interrupt = HALdrv_GetIntUDLStatus( INTERRUPT_IntSync,  AxisRscA->SvAsicRegs->AsicHwReg );
	OpiIntSync( AxisRscA, Interrupt );
	/* Clear the interrupt. */
	HALdrv_ReqIntUDLStatusClear( INTERRUPT_IntSync );
#endif/* A.B6A�΍�b�菈�� */

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
		/* ���x���b�v���⏞ ���Ɍ��o�����m�F */
		MencV = AxisRscA[ax_no].MencV;
		RippleCmp = &(AxisRscA[ax_no].BaseLoops->RippleCmp);

		if(MencV->PhaseReady == TRUE)
		{
			RippleCmp->VelRipplePhaseReady = TRUE;	/* ���x���b�v���⏞ ���Ɍ��o�����t���O = Ture : ���x���b�v���⏞���s�\ */
		}
		else
		{
			RippleCmp->VelRipplePhaseReady = FALSE;	/* ���x���b�v���⏞ ���Ɍ��o�����t���O = False : ���x���b�v���⏞���s�s�� */
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
//	HALdrv_ClearDebugSP( INTERRUPT_ScanA );	/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */


/* <S1A9> Start */
/* ��7W�ł����s�\�ɏC������ׁA�R�����g�A�E�g */
#if 0
/* <S19F> Start */
		if(AxisRscA->PowerManager->MainPowerChecker.conf.TestMode9 == TRUE)		/* ���������@:�Q�[�g�h���C�u�M��ON/OFF */
		{
			if( AxisRscA->PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag == TRUE )	/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA) == TRUE (�Q�[�g�h���C�u�M��ON) */
			{
				SHalDrv_ChargePumpON( AxisRscA->SvAsicRegs->AsicHwReg);				/* N���`���[�W�|���v ON */
				SHalDrv_ChargePumpON_P( AxisRscA->SvAsicRegs->AsicHwReg);			/* P���`���[�W�|���v ON */

				/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA) = FALSE	*/
				/* (����ScanA�ŃQ�[�g�h���C�u�M����OFF�����)						*/
				AxisRscA->PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = FALSE;
			}
			else
			{
				/* �Q�[�g�h���C�u�M�� OFF */
				SHalDrv_ChargePumpOFF(AxisRscA->SvAsicRegs->AsicHwReg);

				/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA) = TRUE	*/
				/* (����ScanA�ŃQ�[�g�h���C�u�M����ON�����)					*/
				AxisRscA->PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = TRUE;
			}
		}
/* <S19F> End */
#endif
/* ��7W�Ή������������� */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		if(AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.TestMode9 == TRUE)		/* ���������@:�Q�[�g�h���C�u�M��ON/OFF */
		{
			if( AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag == TRUE )	/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA) == TRUE (�Q�[�g�h���C�u�M��ON) */
			{
				SHalDrv_ChargePumpON( AxisRscA[ax_no].SvAsicRegs->AsicHwReg);			/* N���`���[�W�|���v ON */
				SHalDrv_ChargePumpON_P( AxisRscA[ax_no].SvAsicRegs->AsicHwReg);			/* P���`���[�W�|���v ON */

				/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA) = FALSE	*/
				/* (����ScanA�ŃQ�[�g�h���C�u�M����OFF�����)						*/
				AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = FALSE;
			}
			else
			{
				/* �Q�[�g�h���C�u�M�� OFF */
				SHalDrv_ChargePumpOFF(AxisRscA[ax_no].SvAsicRegs->AsicHwReg);

				/* ���������@���[�h�F�Q�[�g�h���C�u�M����ԃt���O(ScanA) = TRUE	*/
				/* (����ScanA�ŃQ�[�g�h���C�u�M����ON�����)					*/
				AxisRscA[ax_no].PowerManager->MainPowerChecker.conf.GateDriveSigTestFlag = TRUE;
			}
		}
	}
/* <S1A9> End */

	/*------------------------------------------------------------------*/
	/*	Change Interrupt Cycle						 			<S1D3>	*/
	/*------------------------------------------------------------------*/
	SencSetIntCycleP0Mode( AxisRscA->MencV );

	/* ScanA���s�񐔃J�E���g <S043> */
	TaskProcessTime->SACount++;

	/*----------------------------------------------------------------------*/
	/*		ScanA�������ԃ��j�^�p����				<S043>					*/
	/*----------------------------------------------------------------------*/
	TaskProcessTime->SA_crtTime = KlibGetLongCounter(ScanAStartTime);
	TaskProcessTime->SA_sumTime += TaskProcessTime->SA_crtTime;
	if( TaskProcessTime->SA_crtTime > TaskProcessTime->SA_maxTime)
	{/* �������ԍő�l�X�V */
		TaskProcessTime->SA_maxTime = TaskProcessTime->SA_crtTime;
	}

	if( TaskProcessTime->SB_exeflg == TRUE )
	{/* ScanB���s����ScanA�������Ԃ�ێ����Ă��� */
		TaskProcessTime->SAinB_exeTime += TaskProcessTime->SA_crtTime;
	}
	else
	{
		TaskProcessTime->SAinB_exeTime = 0;
	}
	if( TaskProcessTime->SC_exeflg == TRUE )
	{/* ScanC���s����ScanA�������Ԃ�ێ����Ă��� */
		TaskProcessTime->SAinC_exeTime += TaskProcessTime->SA_crtTime;
	}
	else
	{
		TaskProcessTime->SAinC_exeTime = 0;
	}

}



/**************************** end of file **********************************/
