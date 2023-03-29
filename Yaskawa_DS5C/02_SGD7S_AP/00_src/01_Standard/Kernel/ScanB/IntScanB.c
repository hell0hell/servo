/****************************************************************************
Description	: Base Scan Task(ScanB) Main module
----------------------------------------------------------------------------
Author        : Yaskawa Electric Corp.,
				Copyright (c) 2010 All Rights Reserved

Project       : INGRAM

Functions	  :
	SysBaseScanTask()			:Main routine of the Base Scan Task

----------------------------------------------------------------------------
Changes		:
Name   Date          Description
------------------------------------------------------------------------
I.M.   2011.01.24     renewal

****************************************************************************/
//#include <INTEGRITY.h>
#include "Basedef.h"
#include "MercuryGlobals.h"
#include "NetworkIf.h"	// <USB-FUNCTION> K.Sakamoto
#include "HwDeviceIf.h"
#include "BaseControlMain.h"
#include "MicroScanAIf.h"
#include "AlarmManager.h"
#include "KLib.h"
#include "MLib.h"
#include "DMAdrv.h"
#include "SerialEncIf.h"
#include "KnlApi.h"		/* @@ */
#include "AnalogMonitorCalc.h"	/* <S016> */
#include "Pls.h"

LONG TEST_SCANB_CNT;

/* Prototype */
void SysScanBInt( void );

static void SysBaseServoReadHWInputs( AXIS_HANDLE *AxisRscB );
static void SysBasePowerControl( AXIS_HANDLE *AxisRscB, ULONG SBCnt, ULONG SBCntInc );
static LONG SysMaekNetIfMonInSignal(AXIS_HANDLE *AxisRscB);	/* <S1C0> */
static void SysBaseServoInputProcess( AXIS_HANDLE *AxisRscB );
static void SysBaseServoOutputProcess( AXIS_HANDLE *AxisRscB );
static void	SysBaseMakeScanCMonitor( AXIS_HANDLE *AxisRscB );
//static LONG	KpiInputSpdRefAd( ASIC_HW_IF_STR *AsicHwReg, PRMDATA *Prm, LONG Kvrefad, UCHAR KnlMode );
static UCHAR GetNetworkTriggerSts( AXIS_HANDLE *AxisRscB ); /* <S012> */

static void KpxInputSeqSignals( SEQ_IO_HNDL *IoSignals );	/* <S045> */
static void KpxOutputSeqSignals( LONG ax, SEQ_IO_HNDL *IoSignals );	/* <S045> *//* SGD7W IO���o�͑Ή�<S118> */


#define AREG_ESYNC			0x0027				/* Sync Encorder          */
#define MREG_HSUR			0x0104				/* Host Cpu cmd port      */
#define MPCMD_RENEWAL		0x0004				/* ��-program cmd renewal */
#define MPCMD_CURADSYNC		0x0008				/* Sync Cur Detect AD Int */

extern LONG	NumberOfAxes;

#if defined(_TEST_BOARD_) && !defined(_VC_DEBUG_)
void IntScanB( void ); /* �b�菈�u */
extern void ClearBaseCtrlInterrupt( void );
extern void	SetSeqCtrlInterrupt( void );
extern void	SetEnableInterrupt( void );


/****************************************************************************
  Description: Interrupt Operation for Basecontrol Scan Task
-----------------------------------------------------------------------------
  Parameters	: none
  Return		: none
*****************************************************************************/
void IntScanB( void )
{
	AXIS_HANDLE	*AxisRscB;
	TASK_MEASURE_TIME	*TaskProcessTime;
	ULONG 				tmp_time;

	/* ���[�J���ϐ������� */
	AxisRscB = GetAxisHandle(0);
	TaskProcessTime = AxisRscB->TaskProcessTime;

//	HALdrv_OutputDebugSP( INTERRUPT_ScanB );
	KPI_EI();							//@@	SetEnableInterrupt( );
	ClearBaseCtrlInterrupt( );

//	if( ScanBStartCtr < 0x0F )
//	{
//		ScanBStartCtr++;
//	}
//	else
//	{
		SysScanBInt( );
//	}
	/*------------------------------------------------------------------*/
//	KpxSifMsgManager( ); 			/* Serial Interface Manager <XXX> 	*/
	/*------------------------------------------------------------------*/

	/*------------------------------------------------------------------*/
	/*		ScanC�N��													*/
	/*------------------------------------------------------------------*/
	if( GetInterruptTiming(INTERRUPT_ScanC) )
	{
		/* Update the ScanC Task data */
		SysBaseMakeScanCMonitor( AxisRscB );

		/* Set Sequence Control Interrupt Request */
		SetSeqCtrlInterrupt( );

		/* �g�[�^���������ԑ���p */
		tmp_time = TaskProcessTime->ScanSumTime;
		if(tmp_time > TaskProcessTime->maxTimePer2ms)
		{
			TaskProcessTime->maxTimePer2ms = tmp_time;
		}
		TaskProcessTime->crtTimePer2ms = tmp_time;
		TaskProcessTime->ScanSumTime = 0;
	}

//	HALdrv_ClearDebugSP( INTERRUPT_ScanB );
}
#endif

#if 0 /* �b�菈�� 2013/04/01MD�łł́A�ȉ��̂悤��Asic�Ɠ�����}�� */
#if	(CSW_HW_ALL_SVASIC_WR_SUPPORT != FALSE)

/*--------------------------------------------------------------------------*/
/*	 Servo ASIC Syncronyzation												*/
/*--------------------------------------------------------------------------*/
static void SyncServoASIC(void)
{
	VUINT16	*MicroAdr;

	/* Synchronize JL-076 and host cpu */
	MicroAdr = (VUINT16 *)(HW_SVASIC_ADDRESS+HW_SVASIC_COMADR_OFS);
	/* Synchronize to maicroElectric Current Detect AD Request */
	MicroAdr[MREG_HSUR] = (MPCMD_CURADSYNC | MPCMD_RENEWAL);
//	MicroAdr[AREG_ESYNC] = 0;
}

#else

/*--------------------------------------------------------------------------*/
/*	 Servo ASIC Syncronyzation												*/
/*--------------------------------------------------------------------------*/
static void SyncServoASIC(void)
{
	VUINT16	*MicroAdr;
	UINT	i;

	for(i = 0; i < MAX_AXIS_NUMBER; i++)
	{
		/* Synchronize JL-076 and host cpu */
		MicroAdr = (VUINT16 *)(HW_SVASIC_ADDRESS + (HW_SVASIC_AXIS_OFS*i));
		/* Synchronize to maicroElectric Current Detect AD Request */
		MicroAdr[MREG_HSUR] = (MPCMD_CURADSYNC | MPCMD_RENEWAL);
		/* Synchronize to Serial Encorder */
//   		MicroAdr[AREG_ESYNC] = 0x0000;
	}
}
#endif
#endif

#ifdef CSW_SCANA_ENABLE
/****************************************************************************
  Description: Main routine of the Base Scan Task
-----------------------------------------------------------------------------
  Parameters	: none
  Return		: none
*****************************************************************************/
void SysScanBInt( void )
{
	AXIS_HANDLE	*AxisRscB;
	TASK_MEASURE_TIME	*TaskProcessTime;
//	ULONG 				clock_time;
//	BOOL				SyncState;			/*<S0C7>*/
	VULONG				ScanBStartTime;		/*<S043>*/
	BOOL				Interrupt;
#if (CSW_MICRO_DMA_TRANSFER != 0)
	UINT32				dma_err;
#endif
	LONG				ax_no;
	volatile ULONG		SAinB_exeTime;		/*<S1CD>*/

//	TEST_SCANB_CNT++;
//	HALdrv_OutputDebugSP( INTERRUPT_ScanB );	/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */
	KlibRstLongTimer((ULONG*)&ScanBStartTime);		/*<S043>*//*<S0C7>*/

	/* �������ԑ���J�n	*/
//	HALdrv_OutputDebugSP( 5 );
#if 0
	clock_time = KlibGetClockTime();
#endif

	/* ���[�J���ϐ������� */
	AxisRscB = GetAxisHandle(0);
	TaskProcessTime = AxisRscB->TaskProcessTime;
#if (CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE)
	Interrupt = AxisRscB->NetNodeIf->InterruptReq;
#else
	Interrupt = FALSE;			/* ���������ϐ��C��<S191> */
#endif

	/* ScanB���s�t���O�Z�b�g <S043> */
	TaskProcessTime->SB_exeflg = TRUE;

#if 0 /* �b�菈�� 2013/04/01MD�łł́A�ȉ��̂悤��Asic�Ɠ�����}�� */
	/*------------------------------------------------------------------*/
	/*	Synchronize to all Servo ASICs			 						*/
	/*------------------------------------------------------------------*/
	SyncServoASIC();
#endif

	/*------------------------------------------------------------------*/
	/*	Set Interrupt Cycle P1mode					 			<S1D3>	*/
	/*------------------------------------------------------------------*/
	SencSetIntCycleP1Mode( AxisRscB->MencV );

	/*------------------------------------------------------------------*/
	/*	Read Input data from Hardware devices	 						*/
	/*------------------------------------------------------------------*/
	SysBaseServoReadHWInputs( AxisRscB );

#if 0 /* 2012.09.05 Y.Oka ��ScanB�ʃA�N�Z�X�����Đ݌v�K�v�� */
	/*------------------------------------------------------------------*/
	/*	Input the JL-077 uP cyclic data									*/
	/*------------------------------------------------------------------*/
#if (CSW_MAICRO_DMA_TRANSFER != 0)
	DMAdrvStartMemoryTransfer(DMA_SVDEV_CH, DMA_SVDEV_IN_LINK);
#else
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MicroSA_InputCycResponse( AxisRscB[ax_no].SvAsicRegs,
								AxisRscB[ax_no].MencV,
								AxisRscB[ax_no].CtrlLoopOut );
	}
#endif
#endif

	/*------------------------------------------------------------------*/
	/*	Main Power Controls						 						*/
	/*------------------------------------------------------------------*/
	SysBasePowerControl( AxisRscB,
						 TaskProcessTime->SBCount,
						 TaskProcessTime->SBCountInc );
	TaskProcessTime->SBCount += (TaskProcessTime->SBCountInc+1);

	/*------------------------------------------------------------------*/
	/*	<S053> �T�[�{������͏���/�f�o�C�X���ʐ��䏈��					*/
	/*------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
#if (CSW_MICRO_DMA_READ != 0)
		while(DMAdrvCheckLinkStatus(DMA_SVDEV_CH, DMA_SVDEV_IN_LINK+ax_no));
#endif
		SysBaseServoInputProcess( &AxisRscB[ax_no] );
	}

	/*------------------------------------------------------------------*/
	/*	Input the Communication Process data 							*/
	/*------------------------------------------------------------------*/
	OpiInputProcess( AxisRscB, Interrupt );/*<S0C7>*/
#if (CSW_MICRO_DMA_TRANSFER != 0)
	if(0 != dma_err)
	{
		if(SyncState)
		{
			ALMSetGlobalAlarm(ALM_WDC_ERR);
		}
	}
#endif

	/*------------------------------------------------------------------*/
	/*		�f�[�^�g���[�X����	<S053>									*/
	/*		��SGDV-MD 20130513�ł��炱���Ɉړ����Ă���					*/
	/*------------------------------------------------------------------*/
#if !defined(_TEST_BOARD_) || defined(_VC_DEBUG_)	/* 2012.12.13 Y.Oka �������ԑ΍� */
	/* Get and Set Network Trigger Info. *//* <S012> */
	DtrcSetNetworkTrigger( AxisRscB->TrcHndl, GetNetworkTriggerSts(AxisRscB) );
	/* Execute Trace Operation */
	DtrcExecute( AxisRscB->TrcHndl );
#endif /* #if !defined(_TEST_BOARD_) || defined(_VC_DEBUG_) */

	/*------------------------------------------------------------------*/
	/* Execute the Network Main Procedure								*/
	/*------------------------------------------------------------------*/
	OpiRunTimeScanProcess( AxisRscB, Interrupt );

	/*------------------------------------------------------------------*/
	/*	�T�[�{���䃁�C������											*/
	/*------------------------------------------------------------------*/
	SysBaseServoOutputProcess( AxisRscB );

	/*------------------------------------------------------------------*/
	/*	Start DMA transfer to output the JL-077 uP cyclic data			*/
	/*------------------------------------------------------------------*/
#if (CSW_MICRO_DMA_TRANSFER != 0)
	DMAdrvStartMemoryTransfer(DMA_SVDEV_CH, DMA_SVDEV_OUT_LINK);
#endif

	/*------------------------------------------------------------------*/
	/*	Output the Communication Process data							*/
	/*------------------------------------------------------------------*/
	OpiOutputProcess( AxisRscB, Interrupt );

/* --> <USB-FUNCTION> 2013.05.20 K.Sakamoto */
//#ifdef _USB_FUNC_USE_	/*<S138>*/
//<S150>	MsgIfPollScanB( AxisRscB->NetMsgIf );
	/* USB Function Interface Manager	*//*<S150>*/
	KpxUsbMsgManager( );
//#endif	//#ifdef _USB_FUNC_USE_	/*<S138>*/
/* <-- <USB-FUNCTION> 2013.05.20 K.Sakamoto */

#if 0	/* <S053> */
#if !defined(_TEST_BOARD_) || defined(_VC_DEBUG_)	/* 2012.12.13 Y.Oka �������ԑ΍� */
	/*------------------------------------------------------------------*/
	/*		�f�[�^�g���[�X����											*/
	/*------------------------------------------------------------------*/
	/* Get and Set Network Trigger Info. *//* <S012> */
	DtrcSetNetworkTrigger( AxisRscB->TrcHndl, GetNetworkTriggerSts(AxisRscB) );
	/* Execute Trace Operation */
	DtrcExecute( AxisRscB->TrcHndl );
#endif /* #if !defined(_TEST_BOARD_) || defined(_VC_DEBUG_) */
#endif

	/* <S016> */
	/*------------------------------------------------------------------*/
	/*		�A�i���O���j�^����											*/
	/*------------------------------------------------------------------*/
	if( *(AxisRscB->ScanB250us) )
	{
		BpxOutputAnalogMonitor(AxisRscB);
	}

	/*------------------------------------------------------------------*/
	//KpxSifMsgManager( ); 			/* Serial Interface Manager 	 	*/
	/*------------------------------------------------------------------*/
	/* ScanB�x�[�X�t���[�����^�C�}�J�E���g�A�b�v */
	KlibCountLongTimer(TaskProcessTime->SBcountInMS);

	/* <S016> */
	*(AxisRscB->ScanB250us) ^= 0x01;	/* ScanB 250us Cycle Flag		*/

	/*------------------------------------------------------------------*/
	/* �������ԑ���pGPIO OFF */
	/*------------------------------------------------------------------*/
#if 0
	clock_time = KlibGetClockTime() - clock_time;
	if(clock_time > TaskProcessTime->SB_maxTime)
	{
		TaskProcessTime->SB_maxTime = clock_time;
	}
	TaskProcessTime->SB_crtTime = clock_time;
	TaskProcessTime->ScanSumTime += clock_time;
#endif

	/* DEBUG for M3	*//* Update the ScanC Task data	Scan�Ԃ��ް��̎󂯓n�������͕K���m�F����	*/
	if(HALdrv_GetIntUDLStatus( INTERRUPT_ScanC,  AxisRscB->SvAsicRegs->AsicHwReg ))
	{
		SysBaseMakeScanCMonitor( AxisRscB );
	}
	
	/*----------------------------------------------------------------------*/
	/*		ScanB�������ԃ��j�^�p����					<S043>				*/
	/*----------------------------------------------------------------------*/
	/* ���荞�݂��l���� ScanA�����̍��v���Ԃ��Ɏ擾���� */ /*<S1CD>*/
	SAinB_exeTime = TaskProcessTime->SAinB_exeTime;
	/* ScanB���s����ScanA�������Ԃ����������Ԃ�ScanB�������ԂƂ��� */
	TaskProcessTime->SB_crtTime = KlibGetLongCounter(ScanBStartTime)
									- SAinB_exeTime;	/*<S1CD>*/
//<S1CD>												- TaskProcessTime->SAinB_exeTime;
	TaskProcessTime->SB_sumTime += TaskProcessTime->SB_crtTime;				/* <S046> */
	if( TaskProcessTime->SB_crtTime > TaskProcessTime->SB_maxTime)
	{/* �������ԍő�l�X�V */
		TaskProcessTime->SB_maxTime = TaskProcessTime->SB_crtTime;
	}

	if( TaskProcessTime->SC_exeflg == TRUE )
	{/* ScanC���s����ScanB�������Ԃ�ێ����Ă��� */
		TaskProcessTime->SBinC_exeTime += TaskProcessTime->SB_crtTime;
	}
	else
	{
		TaskProcessTime->SBinC_exeTime = 0;
	}

	/* ScanB���s�t���O���Z�b�g */
	TaskProcessTime->SB_exeflg = FALSE;
	/* ScanB��ScanA���s���ԃN���A */ /*<S1CD>*/
	TaskProcessTime->SAinB_exeTime = 0;

//	HALdrv_ClearDebugSP( INTERRUPT_ScanB );		/* �����݂̕��򏈗��Ń|�[�g�o�͂ɕύX<S042> */

}
#endif

/****************************************************************************
  Description: Converter Control
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static void SysBasePowerControl( AXIS_HANDLE *AxisRscB, ULONG SBCnt, ULONG SBCntInc )
{
	LONG			ax_no;
//	SHAL_LTSIGNALS	*LtSignal;
	LONG			DcVolt; /* <S039> */
	POW_MNG_HNDL	*PowerManager;
	BASE_CTRL_HNDL	*BaseControls;
	CTRL_LOOP_OUT	*CtrlLoopOut;		/* <S024> */
#if (CSW_HW_REGENERATION_CTRL == TRUE)
	BOOL			AllMotStop;
#endif
#if (CSW_POWER_MONITOR_USE == TRUE) /* <S039> */
	BASE_LOOPCTRLS	*BaseLoops;
	CALC_WATTAGE 	*CalcWattage;
#endif

	PowerManager = AxisRscB->PowerManager;
	/*----------------------------------------------------------------------*/
	/*	Converter Controls													*/
	/*----------------------------------------------------------------------*/
	SBCnt += SBCntInc; /* <S0F4> */
	if( (SBCnt&1) == 0 ) /* ��-7�ł�250us�����̏����Ƃ��� */
	{
#if(CSW_HW_DC_DETECT == TRUE)
		/* P-N�d�����o���� */
		PcmDetectDcVolt(&PowerManager->DetectDcVolt,
						&PowerManager->PowerSts);
		/* OV���o���� */
		PcmCheckOverVoltage(&PowerManager->MainPowerChecker,
							&PowerManager->DetectDcVolt,
							&PowerManager->PowerSts,
							&PowerManager->RegeneCtrl );					  /* <S0F7> */

		/* Detect DC Volt AD Convert Start for Next Time */
//		HALdrv_StartAdConvert( );
#endif

#if (CSW_HW_REGENERATION_CTRL == TRUE)
		/* �S�����[�^��~�m�F */
		AllMotStop = TRUE;
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			if( (AxisRscB[ax_no].SeqCtrlOut)->MotStop == FALSE )
			{ /* ���쒆�̎����������ꍇ */
				AllMotStop = FALSE;
				break;
			}
		}

	/* <S0CB> Start */
#if 0
		/* �񐶃g�����W�X�^���䏈�� */
		if( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)
			&& (PowerManager->DetectDcVolt.conf.VdetPrmOk == TRUE)
			&& (PowerManager->MainPowerChecker.conf.TestMode2 == FALSE) )
		{ /* AC���̓��[�h && �d�����o�ݒ萳�� && !TestMode2 */
			PcmRegeneControl( &PowerManager->RegeneCtrl,
							  &PowerManager->PowerSts,
							  AllMotStop );
		}
#endif
	/* ---TestMode6(���������@���[�h�F�񐶐��������ON/OFF)�łȂ��ꍇ�A�{�����ŉ񐶐���𑀍�--- */		/* <S0CB> */
	if( PowerManager->MainPowerChecker.conf.TestMode6 == FALSE )										/* <S0CB> */
	{
		/* �񐶃g�����W�X�^���䏈�� */
#if 0		/* <S21D> */
		if( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)
#else		/* <S21D> */
		if( ( (PowerManager->MainPowerChecker.conf.Dcbus == FALSE)			/* AC���̓��[�h 	*/
		   || ( (PowerManager->MainPowerChecker.conf.Dcbus == TRUE)			/* DC�d������ 	*/
			 && (PowerManager->RegeneCtrl.conf.RegSyn == PNREGSYN) ) )		/* PN������ʃV�X�e���ɂ����ĉ񐶓����@�\���g�p */
#endif
			&& (PowerManager->DetectDcVolt.conf.VdetPrmOk == TRUE)
			&& (PowerManager->MainPowerChecker.conf.TestMode2 == FALSE) )
		{ /* AC���̓��[�h && �d�����o�ݒ萳�� && !TestMode2 */
			PcmRegeneControl( &PowerManager->RegeneCtrl,
							  &PowerManager->PowerSts,
							  AllMotStop );
		}
	}
	/* <S0CB> End */
#endif
	}
//	SBCnt += SBCntInc;
	if( (SBCnt&1) != 0 )
	{
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			BaseControls = AxisRscB[ax_no].BaseControls;
			CtrlLoopOut = AxisRscB[ax_no].CtrlLoopOut;		/* <S024> */
#if( CSW_SVCF_MPKVVCOMP )
			/* DcVolt Compen.Gain Axis. for Micro */
			PcmMicroKvvCalc( &BaseControls->MpKvvComp,
							 PowerManager->PowerSts.DcVolt,
							 PowerManager->MainPowerChecker.conf.UvLevel );
#endif

#if( CSW_SVCF_WEAKENFIELD_USE )
			/* ��ߊE���p�����[�^�v�Z�pPN�d�����ω� */
			PcmWFPrmCalc( &BaseControls->WeakenField,
						  CtrlLoopOut->MotSpd_a,			/* <S024> */
						  PowerManager->PowerSts.DcVolt );
#endif
		}
	}

#if (CSW_POWER_MONITOR_USE == TRUE) /* <S039> */
	/*----------------------------------------------------------------------*/
	/*	����d�͉��Z														*/
	/*----------------------------------------------------------------------*/
	/* DC�d���d���擾 */
	DcVolt = PcmGetDcVoltage( &(AxisRscB->PowerManager->PowerSts) );
	/* ����d�͉��Z */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		BaseControls = AxisRscB[ax_no].BaseControls;
		BaseLoops = AxisRscB[ax_no].BaseLoops;
		CalcWattage = AxisRscB[ax_no].CalcWattage;
		/* ����d�̓��j�^���Z */
		CalcPowerMoniter( CalcWattage, BaseLoops->PowMonVq, BaseLoops->PowMonIqd,
				BaseLoops->PowMonVd, BaseLoops->PowMonIdd, DcVolt, BaseControls->BeComplete );
	}
#endif /* (CSW_POWER_MONITOR_USE == TRUE) */

}

/****************************************************************************
  Description: Hardware Input Procedure for Base Servo Controls
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static void SysBaseServoReadHWInputs( AXIS_HANDLE *AxisRscB )
{
	LONG			ax_no;
	ASIC_HW_IF_STR 		*axisHwReg;/* <S04D> */

	/*	Input the FBA and COA signal status									*/
	/*----------------------------------------------------------------------*/
	HALdrv_InputSerialIO();

	/*----------------------------------------------------------------------*/
	/*
	 *	Input Analog & Pulse hardware status
	 */
	/*----------------------------------------------------------------------*/
#if  SVFSEL_ADC_USE == 1     /* <S09F> */
	KpxAdcScanBService( AxisRscB->pAdc );		/* ADC ScanB */
#endif                       /* <S09F> */
	KpxPlsScanBService( AxisRscB->pPlsIfV );	/* Puls ScanB */

	/*----------------------------------------------------------------------*/
	/* Input Signal Latch Status */
#if	(CSW_SVCF_FBLATCH == TRUE)/* <S004> */
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		/* <S1D6> 1���ڂ������b�`���Ȃ��s��C�� */
		/* <S04D>:�t���N���[�Y�h�G���R�[�_�Ή��̂��ߑ������ύX */
		if(AxisRscB[ax_no].UniPrms->Bprm->FencUse != FALSE)			/* <S1D6> */
		{
			axisHwReg = AxisRscB[ax_no].FencV->EncIf.PosCmpSetReg;	/* <S1D6> */
		}
		else
		{
			axisHwReg = AxisRscB[ax_no].MencV->EncIf.PosCmpSetReg;	/* <S1D6> */
		}
		SHalDrv_ReadLatchPosition( axisHwReg, AxisRscB[ax_no].LtStsAdr,
								 AxisRscB[ax_no].SvControlIf->ExtLtSts, AxisRscB[ax_no].SvControlIf->ExtLtEnable );
	}
#endif

#if	(CSW_SVCF_FULC_MODE == CSW_FULC_SB_SUPPORT)/* �g�p���Ȃ�	*/
	/*------------------------------------------------------------------*/
	/*	Input the Position of Fully-Closed Encoder						*/
	/*------------------------------------------------------------------*/
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		FencReadPosition(AxisRscB[ax_no].FencV, AxisRscB[ax_no].SvAsicRegs->AsicHwReg);
	}
#endif	/* (CSW_SVCF_FULC_MODE == CSW_FULC_SB_SUPPORT) */

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		AxisRscB[ax_no].IoSignals->HwSin = HALdrv_ReadMonInputSignals( ax_no /*pAx-AxisRscB */ );
		AxisRscB[ax_no].IoSignals->HwSin |= (((AxisRscB[ax_no].pPlsIfV)->f.PerClrSignal == TRUE) << 15); /* ��ŏ��� */
	}

	/* ���o�͐M����� for Option	*//* <S1C0> */
	SysMaekNetIfMonInSignal(AxisRscB);

}

/****************************************************************************
  Description: Input Procedure for Base Servo Controls
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
-----------------------------------------------------------------------------
�P���\��(SGDV�݊�)
	D0�`D7	:SI0�`SI6
	D7�`D9	:SO0�`SO3
	D16�`D18:PgAphase,PgBphase,PgCphase	���ݖ��Ή�
	D24, D25:HWBB1,2
	D26�`D29:STOP1�`4	:�Z�[�t�e�B�I�v�V�����J�[�h���͐M��1�`4	���ݖ��Ή�
	D30, D31:EDM2,3		:�Z�[�t�e�B�I�v�V�����J�[�h�o�͐M��2,3	���ݖ��Ή�
	
2���\��
	D0�`D11	:SI01�`SI06,SI11�`SI16
	D12�`D16	:SO0�`SO5
	D17�`D98:PgAphase,PgBphase,PgCphase	���ݖ��Ή�
	D24, D25:HWBB1,2
	D26�`D29:STOP1�`4	:�Z�[�t�e�B�I�v�V�����J�[�h���͐M��1�`4	���ݖ��Ή�
	D30, D31:EDM2,3		:�Z�[�t�e�B�I�v�V�����J�[�h�o�͐M��2,3	���ݖ��Ή�

*****************************************************************************//* <S1C0> */
static LONG SysMaekNetIfMonInSignal(AXIS_HANDLE *AxisRscB)
{
	if(NumberOfAxes == 1)
	{/* �P���\��	*/

		AxisRscB[0].SvControlIf->NetIfMonInSignal1
			 = (ULONG)(HALdrv_ReadMonInputSignals(0) & 0x7F)					/* SI�M���擾	*/
			 | (ULONG)(((HALdrv_ReadMonAllOutputSignals( ) & 0x0E)>>1) << 7)	/* SO�M���擾(����1Bit:ALM)	*/
			 | (ULONG)((AxisRscB[0].IoSignals->DetHwbbSts.Bbon1 & 0x01) << 24)
			 | (ULONG)((AxisRscB[0].IoSignals->DetHwbbSts.Bbon1 & 0x01) << 25)
				;/* �����܂�	*/
	}
	else if(NumberOfAxes == 2)
	{/* 2���\��	*/
		/* �擪���̂ݏ��������{(���j�b�g����)	*/
		AxisRscB[0].SvControlIf->NetIfMonInSignal2
		=  (HALdrv_ReadMonAllInputSignals( ) & 0xFFF)								/* SI0*,SI1*�M���擾	*/
			 | (ULONG)(((HALdrv_ReadMonAllOutputSignals( ) & 0x7C)>>2) << 12)	/* SO�M���擾(����2Bit:ALM)	*/
			 | (ULONG)((AxisRscB[0].IoSignals->DetHwbbSts.Bbon1 & 0x01) << 24)	/* HWBB1	*/
			 | (ULONG)((AxisRscB[0].IoSignals->DetHwbbSts.Bbon1 & 0x01) << 25)	/* HWBB2	*/
				 ;/* �����܂�	*/

		/* �擪���̃f�[�^�𑼂̎�(1����)�ɃR�s�[����	*/
		AxisRscB[1].SvControlIf->NetIfMonInSignal2 = AxisRscB[0].SvControlIf->NetIfMonInSignal2;
	}
}

/****************************************************************************
  Description: Input Procedure for Base Servo Controls
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static void SysBaseServoInputProcess( AXIS_HANDLE *AxisRscB )
{
//<S053>	LONG			ax_no;
	AXIS_HANDLE		*AxisRsc;

	AxisRsc = AxisRscB;
//<S053>	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
#if 0 /*  ����ScanA��CpuScanA�� */
		/*------------------------------------------------------------------*/
		/* Read ScanA monitor values */
		/*------------------------------------------------------------------*/
		MicroSA_InputCycDataB( AxisRsc->SvAsicRegs,
								AxisRsc->MencV,
								AxisRsc->CtrlLoopOut,
								AxisRsc->UniPrms->Bprm );
#endif

#if 0 /* ����ScanA��CpuScanA�� */
#if defined(CSW_NETIF_SYNQNET_TYPE)
		if(AxisRsc->SvControlIf->CmdReady == FALSE)
#endif
		{
			MicroSA_InputCycDataExB( AxisRsc->SvAsicRegs,
									AxisRsc->CtrlLoopOut,
									AxisRsc->UniPrms->Bprm );
		}
#endif
		/*------------------------------------------------------------------*/
		/* Input Encoder position */
		/*------------------------------------------------------------------*/
		BpiInputMotorPosition( AxisRsc->BaseControls,
		                       AxisRsc->SvAsicRegs->AsicMicroReg,
		                       AxisRsc->UniPrms->Bprm,
		                       AxisRsc->UniPrms->Prm,	/* <S04D>:��������	*/
		                       AxisRsc->CtrlLoopOut);	/* <kira>			*/

		/*------------------------------------------------------------------*/
		/* @@ Input PERR Clear Latched Position (AP ONLY) 					*/
		/*------------------------------------------------------------------*/
		KpiPlsPerClrLatchPosition( 						/* get clr position */
			AxisRsc->pPlsIfV, 							/*  				*/
			AxisRsc->BaseControls->CtrlCmdMngr.PosFbki,	/* Feedback Position */
			AxisRsc->MencV->MotPosX[0],					/* Motor Angle 		*/
			&(AxisRsc->BaseControls->ClrFbPos) );		/* Latched Position */


		/*------------------------------------------------------------------*/
		/* Output Monitor status to Network layer */
		/*------------------------------------------------------------------*/
		BpiOutputNetStatus(AxisRsc->BaseControls,
		                   AxisRsc->SvControlIf,
		                   AxisRsc->CtrlLoopOut,
		                   AxisRsc->SeqCtrlOut,
						   AxisRsc->UniPrms->Bprm,
						   AxisRsc->pPlsIfV);/* <S04D>:�����ǉ� *//* <S121> */

//<S053>		AxisRsc++;
	}

}


/****************************************************************************
  Description: Output Procedure for Base Servo Controls
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
		BOOL Interrupt		: HINT Interrupt status
  Return		:
		none
*****************************************************************************/
static void SysBaseServoOutputProcess( AXIS_HANDLE *AxisRscB )
{

	LONG			ax_no;
	AXIS_HANDLE		*AxisRsc;
//	ASIC_HW_IF_STR	*pAsicComHwReg;
//	BASE_CTRL_OUT	*BaseCtrlOut;	/*<S0C7>*/
	BASE_CTRL_HNDL	*BaseCtrlData;
	ASIC_HW_IF_STR	*pAsicHwReg;
	FUN_CMN_CONTROL	*FnCmnCtrl;				/* <S0CB> */
	HWSOUT ForceOutSignals;					/* <S0CB> */
	POW_MNG_HNDL	*hPowerManager;;		/* <S0CB> */


	AxisRsc = AxisRscB;
	FnCmnCtrl = AxisRsc->FnCmnCtrl;			/* <S0CB> */
	hPowerManager = AxisRsc->PowerManager;	/* <S0CB> */

	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
//		BaseCtrlOut = AxisRsc->BaseCtrlOut;	/*<S0C7>*/
		BaseCtrlData = AxisRsc->BaseControls;

		KpxInputSeqSignals( AxisRsc->IoSignals ); 							/* <S045>		*/
/*--------------------------------------------------------------------------------------------------*/
#if 0 /* 2012.09.07 Y.Oka ��Mercury�p�V�[�P���X���͏������󖢑Ή��� */
		/* ���b�`������2013/04/01MD�łő傫���ύX�������Ă��邽�ߌ��������K�v */
		SeqCtrlData = AxisRsc->SeqCtrlData;
		/* DEC���� */
		SeqIo = &(SeqCtrlData->DecSignal);
		SeqIo->SignalStatus = ((SeqInputSignals.dw >> SeqIo->PinNumber) & SeqIo->Mask) ^ SeqIo->XorMask;
		sigbits = (SeqIo->SignalStatus << DEC_SIG_BITNO);
		/* EXT1���� */
		SeqIo = &(SeqCtrlData->ExtSignal[0]);
		SeqIo->SignalStatus = ((SeqInputSignals.dw >> SeqIo->PinNumber) & SeqIo->Mask) ^ SeqIo->XorMask;
		sigbits |= (SeqIo->SignalStatus << EXT1_SIG_BITNO);
		/* EXT2���� */
		SeqIo = &(SeqCtrlData->ExtSignal[1]);
		SeqIo->SignalStatus = ((SeqInputSignals.dw >> SeqIo->PinNumber) & SeqIo->Mask) ^ SeqIo->XorMask;
		sigbits |= (SeqIo->SignalStatus << EXT2_SIG_BITNO);
		/* EXT3���� */
		SeqIo = &(SeqCtrlData->ExtSignal[2]);
		SeqIo->SignalStatus = ((SeqInputSignals.dw >> SeqIo->PinNumber) & SeqIo->Mask) ^ SeqIo->XorMask;
		sigbits |= (SeqIo->SignalStatus << EXT3_SIG_BITNO);

		/* Save bit status */
		AxisRsc->SvControlIf->RspSigBit = sigbits | AxisRsc->SeqCtrlOut->RspSigBits;

/*--------------------------------------------------------------------------------------------------*/
/*		�V�[�P���X�M������																			*/
/*--------------------------------------------------------------------------------------------------*/
		KPI_SIN_START_SCANB( Inx );
		BinV.f.Ctrl.EnbPCtrl = KPI_SIN_PCON( Inx );
		BinV.f.Sv.Inhibit    = KPI_SIN_INHIBIT( Inx );
		BinV.f.Sv.GainSel0   = KPI_SIN_GAINSEL0( Inx );
		BinV.f.Sv.GainSel1   = KPI_SIN_GAINSEL1( Inx );
		BinV.f.Sv.Pcl        = KPI_SIN_PCL( Inx );										/* <V226>	*/
		BinV.f.Sv.Ncl        = KPI_SIN_NCL( Inx );										/* <V226>	*/
		BinV.f.Sv.Psel       = KPI_SIN_PSEL( Inx );		/* �w�߃p���X���͔{���؂�ւ��M��  <V677>	*/
/*--------------------------------------------------------------------------------------------------*/
/*		�Q�C���ؑ֗p�Q�C���ԍ�																		*/
/*--------------------------------------------------------------------------------------------------*/
		BinV.GselNum = GET_BITSDATA( &BinV.f.Sv, BITNO_BI_GainSel0, 0x03 );

/*--------------------------------------------------------------------------------------------------*/
/*		PerClr Signal																				*/
/*--------------------------------------------------------------------------------------------------*/
		BinK.f.PerClrSignal = KPX_READ_PERCLRSIGNAL( );		/* �΍��N���A�����ݐM���ǂݍ���			*/

/*--------------------------------------------------------------------------------------------------*/
/*		Divided Pulse Output Signal, ExtInterrupt Signal for Data Trace								*/
/*--------------------------------------------------------------------------------------------------*/
		BinK.DivPulseSig  = KPX_READ_DIVPULSESIG( );		/* �����p���X�o�͐M��(A,B,C)			*/
		BinK.ExtIntSignal = KPX_READ_EXTINTSIGNAL( );		/* �O�����b�`�����ݐM��(Ext1,2,3)		*/
#endif /* 2012.09.07 Y.Oka ��Mercury�p�V�[�P���X���͏������󖢑Ή��� */


#if defined (_IO_SERVO_ON_ ) || defined (_TEST_BOARD_ ) /* �����p�b��Ή� 2012.11.05 Y.Oka */
/*--------------------------------------------------------------------------------------------------*/
/*		�V�[�P���X�M������																			*/
/*--------------------------------------------------------------------------------------------------*/
		SeqInputSignals = HALdrv_ReadInputSignals( );
		/* Set Servo ON Request */
		if( SeqInputSignals.flag.SI0 == FALSE )
		{
			BaseCtrlOut->CmdSeqBit = SERVO_ON_BIT;
		}
		else
		{
			BaseCtrlOut->CmdSeqBit = 0;
		}
#endif


#ifdef _IO_SERVO_ON_ /* @@�b�� */
		/* 10rpm */
		AxisRsc->SvControlIf->NetSpdRef = 2542000;
		AxisRsc->SvControlIf->NetCtrlMcmd	= CTRLMODE_SPD;
#endif /* _IO_SERVO_ON_ */ /* @@�b�� */

/*--------------------------------------------------------------------------------------------------*/
/*		@@ �b��F�N���A�M���G�b�W���o																*/
/*--------------------------------------------------------------------------------------------------*/
		AxisRscB->BaseControls->PerClrEdge = (AxisRscB->pPlsIfV->f.PerClrEdge == TRUE);
/*--------------------------------------------------------------------------------------------------*/

		AxisRsc++;
	}

#if (CSW_MAICRO_DMA_TRANSFER != 0)


	AxisRsc = AxisRscB;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MicroSA_InputCycResponse( AxisRsc->SvAsicRegs,
								AxisRsc->MencV,
								AxisRsc->CtrlLoopOut );
		AxisRsc++;
	}
#endif

	/*----------------------------------------------------------------------*/
	/*	ScanB Servo Control													*/
	/*----------------------------------------------------------------------*/
	AxisRsc = AxisRscB;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		BaseCtrlData = AxisRsc->BaseControls;	 /* <S0E4> */
		BpiScanBMain( AxisRsc, ax_no );		/* <S1C0>�F���ǉ� */

#if (CSW_AUTO_CHARGE_PUMP == FALSE)
		{
		BOOL OCState;
		/* Charge Pump Control Service */
		OCState = ALMCheckEachState(AxisRsc->AlmManager, ALM_OC);
//		BeSeqChargePumpControl( AxisRsc->SvAsicRegs, &(AxisRsc->BeSequence->ChargePump), OCState );/* <S0CB> Delete */
/* <S0CB> */
			/* ---TestMode8(���������@���[�h�F�Q�[�h�h���C�u�M�������� ON/OFF)�łȂ��ꍇ�A
				���ATestMode9(���������@���[�h�F�Q�[�h�h���C�u�M����ScanA��ON/OFF)�łȂ��ꍇ�A�{�����ŃQ�[�h�h���C�u�M������𑀍�--- */
//			if( hPowerManager->MainPowerChecker.conf.TestMode8 == FALSE )		/* <S19F> */
			if(	( hPowerManager->MainPowerChecker.conf.TestMode8 == FALSE ) &&	/* <S19F> */
				 (hPowerManager->MainPowerChecker.conf.TestMode9 == FALSE) )	/* <S19F> */
			{
				BeSeqChargePumpControl(AxisRsc->SvAsicRegs, &(AxisRsc->BeSequence->ChargePump), 
				OCState,AxisRsc->MotorLessTest->MotorLessTestMode,AxisRsc->PowerManager->PowerSts.NegPumpReq);
				/* <S1B7> *//*�`���[�W�|���v��ON/OFF������s�� <S201>*/
			}
/* <S0CB> */
		}
#endif
		/*----------------------------------------------------------------------*/
		/*		Calculate Motor Speed for ScanA	[15000/OS]			<S0E4>		*/
		/*----------------------------------------------------------------------*/
		SHalDrv_SetMotSpdToMicro( AxisRsc->SvAsicRegs->AsicMicroReg, BaseCtrlData->MotSts.MotSpd );
		AxisRsc++;
	}

#ifndef CSW_SCANA_ENABLE
	/*----------------------------------------------------------------------*/
	/*	Output the Servo Control Reference to the JL-077 uProgram			*/
	/*----------------------------------------------------------------------*/
	AxisRsc = AxisRscB;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		MicroSA_OutputCycReference( AxisRsc->SvAsicRegs,
								 AxisRsc->MencV,
								 AxisRsc->BaseCtrlData,
								 AxisRsc->SvControlIf->CmdCtrl.CmdCtrlBit );
		AxisRsc++;
	}
#endif

	/*----------------------------------------------------------------------*/
	/* <S045> Make Output Signal				 							*/
	/*----------------------------------------------------------------------*/
	AxisRsc = AxisRscB;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
//-		KpxOutputSeqSignals( AxisRsc->IoSignals );
		KpxOutputSeqSignals( ax_no, AxisRsc->IoSignals );		/* SGD7W IO���o�͑Ή�<S118> */
		AxisRsc++;
	}

	/*----------------------------------------------------------------------*/
	/* Output Serial I/O for COA and FBA									*/
	/*----------------------------------------------------------------------*/
/* <S0CB>	Start */
	if( FnCmnCtrl->OutModeFlg == TRUE )	/* �����o�̓��[�h */
	{
		ForceOutSignals.dw = FnCmnCtrl->ForceOutSignalsHW;
		HALdrv_SetSOutB( 0, ForceOutSignals );
		HALdrv_ForceOutSignalsMask( );			/* <S198> */
	}
/* <S0CB>	End  */

	HALdrv_OutputSerialIO();

	/*----------------------------------------------------------------------*/
	/* Kick JL-086 WDT														*/
	/*----------------------------------------------------------------------*/
	pAsicHwReg = AxisRscB->SvAsicRegs->AsicHwReg;
	SHalDrv_KickWDT( pAsicHwReg, (AxisRscB->PowerManager)->MainPowerChecker.conf.TestMode4 );


	/*----------------------------------------------------------------------*/
	/*	Start DMA transfer to output the JL-077 uP cyclic data				*/
	/*----------------------------------------------------------------------*/
#if (CSW_MAICRO_DMA_TRANSFER != 0)
	DMAdrvStartMemoryTransfer(DMA_SVDEV_CH, DMA_SVDEV_OUT_LINK);
#endif

}



/****************************************************************************
  Description: Make ScanC Output values
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static void	SysBaseMakeScanCMonitor( AXIS_HANDLE *AxisRscB )
{
	BASE_CTRL_HNDL		*BaseControls;
	BASE_LOOPCTRLS		*BaseLoops;		/* <S020> */
	CTRL_LOOP_OUT		*CtrlLoopOut;
	BASE_CTRL_OUT		*BaseCtrlOut;
//	CALC_WATTAGE 		*CalcWattage;	/* <S020> *//* <S039> */
	LONG				ax_no;
	AXIS_HANDLE			*AxisRsc;
//	LONG				DcVolt;			/* <S020> *//* <S039> */

#if 0 /* <S039> */
	/* ���ʏ��� */
#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
	/* ����d�͏�����(�������v�Z�p) */
	ResetPowerConsumption( AxisRscB->CalcWattage );
	/* DC�d���d���擾 */
	DcVolt = PcmGetDcVoltage( &(AxisRscB->PowerManager->PowerSts) );
#endif /* (CSW_POWER_MONITOR_USE == TRUE) */
#endif

	/* �������� */
	AxisRsc	= AxisRscB;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
		BaseLoops = AxisRsc->BaseLoops;			/* <S020> */
		BaseControls= AxisRsc->BaseControls;
		CtrlLoopOut	= AxisRsc->CtrlLoopOut;
		BaseCtrlOut	= AxisRsc->BaseCtrlOut;
//		CalcWattage = AxisRsc->CalcWattage;		/* <S020> *//* <S039> */

		/* ���j�^�p�f�[�^�X�V */
		AxisRsc->UnMonitor->UnSpdRef = AxisRsc->SvControlIf->NetSpdRef;

		/* ScanB->ScanC�p���j�^�f�[�^�X�V */
		BaseCtrlOut->MotPos				= BaseControls->MotSts.MotPos;
//		BaseCtrlOut->FencPos			= BaseControls->CtrlCmdMngr.PosFbki;	/* <S04D>:Dell */
		BaseCtrlOut->FencPos			= BaseControls->MotSts.FencPos;			/* <S04D>:Add */
		BaseCtrlOut->PcmdCntr			= BaseControls->CtrlCmdMngr.PosRef;
		BaseCtrlOut->MotSpdSum			= BaseControls->MotSts.MotSpdSumB;
		BaseCtrlOut->MotSpdCnt			= BaseControls->MotSts.MotSpdCntB;
		BaseCtrlOut->OutputSpdRef		= BaseControls->CtrlCmdMngr.SpdRefo;
		BaseCtrlOut->PositionError		= BaseControls->PosManager.var.PosErrA.per32s;
		BaseCtrlOut->SvonSpdLmtReq		= AxisRsc->CtrlLoopOut->SvonSpdLmtFlag;
		BaseControls->MotSts.MotSpdSumB	= 0;
		BaseControls->MotSts.MotSpdCntB	= 0;
		BaseCtrlOut->CmdSeqBit			= AxisRsc->SvControlIf->CmdSeqBit;
		BaseCtrlOut->TblZclamp			= (BaseControls->TblDrive.var.TblZmd.b.zm == CTRLMODE_ZCLMP) ? TRUE : FALSE;

//		BaseCtrlOut->IdRefMon			= CtrlLoopOut->IdRefMon;
		BaseCtrlOut->IqRefMon			= CtrlLoopOut->IqRefMon;
		BaseCtrlOut->TrqRefMon 			= CtrlLoopOut->TrqRefMon;

		/* �w�߃f�[�^�o�� */
		BaseCtrlOut->TrqCtrlRef = AxisRsc->SvControlIf->NetTrqRef;
		BaseCtrlOut->SpdCtrlRef = BaseControls->CtrlCmdMngr.SpdRefi;
		BaseCtrlOut->CtrlModeOut = BaseControls->CtrlModeSet.CtrlMode;

#if 0 /* <S039> */
#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
		/* ����d�̓��j�^���Z */
		CalcPowerMoniter( CalcWattage, BaseLoops->PowMonVq, BaseLoops->PowMonIqd,
				BaseLoops->PowMonVd, BaseLoops->PowMonIdd, DcVolt, BaseControls->BeComplete );
#endif /* (CSW_POWER_MONITOR_USE == TRUE) */
#endif

		AxisRsc++;
	}

 /* Add <S039> */
	/* ���ʏ��� */
#if (CSW_POWER_MONITOR_USE == TRUE) /* <S020> */
	/* ����d�͒l�𕽋ω� */
	CalcPowerConsumptionAvr( AxisRscB->CalcWattage );
	/* ����d�͏�����(�������v�Z�p) */
	ResetPowerConsumption( AxisRscB->CalcWattage );
	/* DC�d���d���擾 */
//	DcVolt = PcmGetDcVoltage( &(AxisRscB->PowerManager->PowerSts) );
#endif
/* Add End <S039> */

}



/****************************************************************************
  Description: Make Network Trigger Request		<S012>
-----------------------------------------------------------------------------
  Parameters	:
		AXIS_HANDLE *AxisRscB	: Pointer to Axis Handle
  Return		:
		none
*****************************************************************************/
static UCHAR GetNetworkTriggerSts( AXIS_HANDLE *AxisRscB )
{
	UCHAR				ret;
	LONG				ax_no;
	AXIS_HANDLE			*AxisRsc;

	ret = FALSE;
	AxisRsc	= AxisRscB;
	for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
	{
//		ret |= (AxisRsc->SvControlIf->CmdCtrl.CmdCtrlBit >> TRACE_TRIGGER_BITNO); /* <S021> */
		ret |= ((AxisRsc->SvControlIf->CmdCtrl.CmdCtrlBit & TRACE_TRG_BIT) >> TRACE_TRIGGER_BITNO);
	}
	return ret; /* <S021> */
}

/****************************************************************************************************/
/*																									*/
/*		<S045> �X�L�����a�V�[�P���X�M�����͏���														*/
/*																									*/
/****************************************************************************************************/
void KpxInputSeqSignals( SEQ_IO_HNDL *IoSignals )
{
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE /* <S04B> */
	HWSIN			SeqInputSignals;

		SeqInputSignals = HALdrv_ReadInputSignals();				/* Read Input SIgnals 			*/
/*--------------------------------------------------------------------------------------------------*/
/*		 �V�[�P���X�M������(ScanB)																	*/
/*--------------------------------------------------------------------------------------------------*/
		SEQIO_START_INSIGX( IoSignals );							/* Start Input Signals */
/*--------------------------------------------------------------------------------------------------*/
		SEQIO_GET_INSIGX( PclSignal, SeqInputSignals.dw );			/* P-CL */
		SEQIO_GET_INSIGX( NclSignal, SeqInputSignals.dw );			/* N-CL */
		SEQIO_GET_INSIGX( DecSignal, SeqInputSignals.dw );			/* DEC */
		SEQIO_GET_INSIGX( ExtSignal[0], SeqInputSignals.dw );		/* EXT1 */
		SEQIO_GET_INSIGX( ExtSignal[1], SeqInputSignals.dw );		/* EXT2 */
		SEQIO_GET_INSIGX( ExtSignal[2], SeqInputSignals.dw );		/* EXT3 */
		SEQIO_GET_INSIGX( FstpSignal, SeqInputSignals.dw);			/* FSTP */ /* <S1F5> */
/*--------------------------------------------------------------------------------------------------*/
		SEQIO_END_INSIGX( );										/* End Input Signals */
/*--------------------------------------------------------------------------------------------------*/
#endif /* <S04B> */
		return;
}


/****************************************************************************************************/
/*																									*/
/*		<S045> �X�L�����a�V�[�P���X�M���o�͏���														*/
/*		TODO: AP�^�Ə��������ʉ�����																*/
/*																									*/
/****************************************************************************************************/
//- void KpxOutputSeqSignals( SEQ_IO_HNDL *IoSignals )
void KpxOutputSeqSignals( LONG ax, SEQ_IO_HNDL *IoSignals )		/* SGD7W IO���o�͑Ή�<S118> */
{
//<S081> #if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE /* <S04B> */
HWSOUT OutX; OutX.dw = 0;
/*--------------------------------------------------------------------------------------------------*/
		SEQIO_START_OUTSIGX( IoSignals );			/* Start Output Signal */
/*--------------------------------------------------------------------------------------------------*/
		SEQIO_SET_OUTSIGX( NearSignal, OutX.dw );	/* /NEAR */	/* <S1D4> */
		SEQIO_SET_OUTSIGX( CoinSignal, OutX.dw );	/* /COIN */
#if CSW_NETIF_TYPE != NETIF_ANLGPLS_TYPE /* <S081> */
		SEQIO_SET_OUTSIGX( CltSignal,  OutX.dw );	/* /CLT */
		SEQIO_SET_OUTSIGX( VltSignal,  OutX.dw );	/* /VLT */
//		SEQIO_SET_OUTSIGX( NearSignal, OutX.dw );	/* /NEAR */	/* <S1D4> */
#endif /* <S081> */
/*--------------------------------------------------------------------------------------------------*/
//-		SEQIO_END_OUTSIGX_ScanB( OutX );			/* End Output Signal */
		SEQIO_END_OUTSIGX_ScanB( ax, OutX );			/* End Output Signal */		/* SGD7W IO���o�͑Ή�<S118> */
/*--------------------------------------------------------------------------------------------------*/
//<S081> #endif /* <S04B> */
		return;
}

/**************************** end of file **********************************/
