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

/* �O���֐� */
extern LONG CpiGetScanCCnt( void );						
extern void KpxMsgManager( void );						
extern void ScuSoftReset( AXIS_HANDLE *AxisHandle );	
extern void UsbFuncTaskControl( void );					

#if 0/* �ۗ� */
extern BOOL FTP_FirmwareUpdateMain(void);
#endif
extern void	MbusFuncSystemDownLoadCheckSum( void );	/* �Í����t�H�[�}�b�g�Ή� <S146> */

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
		/* �������ԑ���pGPIO ON */
//		HALdrv_GpioOutput( TRUE 0 );

		/* ROUND */
	/*----------------------------------------------------------------------*/
		KlibRstLongTimer( &wtimer );	/* ScanC����~�������ł�Round�����������悤�ɏC��<S03F> */
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

		/* �������ԑ���pGPIO OFF */
//		HALdrv_GpioOutput( FALSE, 0 );

#ifndef _VC_DEBUG_ /* 2013.05.31 Y.Oka */
		KpxMsgManager( ); 			/* Message Manager <XXX> */

		/* ���������[�v *//* <S19B> */
		for( ax = 0; ax < NumberOfAxes; ax++ )
		{
			AxisRscR = (AXIS_HANDLE*)GetAxisHandle(ax);
			KpxDownloadEeprom( AxisRscR->pRwAllMng );		/* <S00C> Download Eeprom  */
			/* <S031> Lcd Parameter Copy *//* <S098> */
			if( I2C_DRV_SUCCESS != KpxLcdPdlEeprom(AxisRscR->pRwPdlMng) )
			{ /* A.BF6 : �V�X�e���A���[���U(I2C�h���C�o�ُ�) */
				ALMSetServoAlarm( AxisRscR->AlmManager, ALM_I2C_DRVERR );
			}
		}
		/* 0���֖߂� */
		AxisRscR = (AXIS_HANDLE*)GetAxisHandle(0);/* <S19B> */
#endif	//#ifndef _VC_DEBUG_ /* 2013.05.31 Y.Oka */

		/* �X�^�b�N�̈�`�F�b�N *//*<S127>*/
		if( 0xFFFFFFFF != *(VULONG*)(0x67FAF61C) )
		{/* A.BF1 : �V�X�e���A���[���P(�����v���O��������ُ�)			*/
			ALMSetGlobalAlarm( ALM_STACK_OVF );
		}

		MbusFuncSystemDownLoadCheckSum();	/* �Í����t�H�[�}�b�g�Ή� <S146> */

		/* �\�t�g���Z�b�g <S076> */
		AxisRscR->BeSequence->SoftResetReq = LpxSoftReset( AxisRscR );

		KlibWaitus(500);	/* 0.5ms */

		/* <S0CB> Start */
			/* �X�L����C�J�E���g�擾 : Round�������K���X�L����C�̌�Ɏ��s�����悤�ɂ���B */
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
	/* �l�b�g���[�N���W���[��Round���� */
	if(FALSE == OpiRoundProcedure( AxisRscR ))
	{
#if 0/* �ۗ� */
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
	/* �l�b�g���[�N���b�Z�[�W�ʐM�Ǘ�	*/
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
		/* Fn�@�\���s���� */
		FunRunTimeService(AxisRsc->FnManager, FUN_CH1);
		FunRunTimeService(AxisRsc->FnManager, FUN_CH2);
//		if ( !(AxisRsc->BeSequence->ServoReady) ){ AxisRsc->FnCmnCtrl->FnSvonReq = FALSE; } /* <S0B4> */
		if( AxisRsc->MencV->PhaseReady == TRUE )		/* <S21C> �ʑ����o���� */
		{
			if ( !(AxisRsc->BeSequence->ServoReady) ){ AxisRsc->FnCmnCtrl->FnSvonReq = FALSE; } /* <S0B4> */
		}												/* <S21C> */
		
		/* �����m�b�`�t�B���^�ݒ�V�[�P���X�R�[���p�|�C���^�ݒ� */
		ANotchCtrl.ANotchSeq = AxisRsc->ANotchSeq;
		ANotchCtrl.ANotch = AxisRsc->ANotch;
		ANotchCtrl.FftAna = AxisRsc->FftAna;
		ANotchCtrl.DetVib = AxisRsc->DetVib;
		ANotchCtrl.Prm = AxisRsc->UniPrms->Prm;
		ANotchCtrl.RegCfg = AxisRsc->RegManager->hPnReg;
		/* �����m�b�`�t�B���^�ݒ�p�f�[�^�ݒ菈�� */
		AutoNotchSetCondition( ANotchCtrl.ANotchSeq,
							   AxisRsc->BaseControls->TuneLessCtrl.var.TuningFuncRun,
							   AxisRsc->SeqCtrlOut->BaseEnableReq,
							   AxisRsc->BaseControls->TuneLessCtrl.var.TuneLessAct,
							   AxisRsc->SeqCtrlOut->OverTrvlSts );
		/* �����m�b�`�t�B���^�ݒ�V�[�P���X���s���� */
		AutoNotchFilterSetting( &ANotchCtrl );

		/* ABS�X�P�[�����Ɍ��o���ʏ������݃V�[�P���X���� */
		if(FALSE != PdetWriteMotorPhaseSequence(AxisRsc->PdetAbsSeq,
												AxisRsc->MencV,
												AxisRsc->SvAsicRegs->AsicHwReg,
												AxisRsc->RegManager->hPnReg))
		{
			ALMSetServoAlarm( AxisRsc->AlmManager, ALM_PGCOM );	/* A.C90�F�G���R�[�_�ʐM�ُ� */
		}
		/* DB�����[����񐔗p�p�����[�^��������TO�@Eeprom */
		LifeTimeDBPrmStoreValueToEeprom( AxisRsc->hDBLifeTime,
										AxisRsc->RegManager->hPnReg,
										AxisRsc->UniPrms->Prm );			/* <S192> */
	}

	/* �˖h�����[����񐔗p�p�����[�^��������TO�@Eeprom */
	LifeTimeRushPrmStoreValueToEeprom( &AxisRscR->hLifeTime->RushLifeTimeHdl,
										AxisRscR->RegManager->hPnReg,
										AxisRscR->UniPrms->Cprm );			/* <S192> */

	/* Alarm Manager : EEPROM������������ */
	timer = KlibGetMainPowerOffTimer();
	if((timer <= 0) || (timer >= KLIB_EEPDISABLE_TIME))
	{/* Main Power off����100ms�Ԃ͏������݂��Ȃ� */
		for( ax_no = 0; ax_no < NumberOfAxes; ax_no++ )
		{
			AxisRsc = &AxisRscR[ax_no];
			ALMHistoryWriteService(AxisRsc->AlmManager);
		}
	}
	else
	{
		AlmManager = AxisRscR->AlmManager;
		/* Main Power off��Time Stamp�������� */
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
	/* �T�[�{�p�b�N���xIC�Ǘ� */
	DetTempManager( AxisRscR->TempManager );
#endif

/* 2013/04/01MD�łł́ARndReady�͂Ȃ��B�b��I��SEQ_CTRL_OUT�ɒ�`����	*/
	if(*iniFlg == FALSE)
	{/* ���E���h���f�B�ݒ菈�� */
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
	{/* �u�T�[�{�I���łȂ��v���u���[�^��]���łȂ��v�Ƃ��̂݃��Z�b�g��*/
		ScuSoftReset( AxisRscR );
		resetReq = FALSE;		/* */
	}

	return( resetReq );
}



/**************************** end of file **********************************/
